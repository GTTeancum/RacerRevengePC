"""Bounded offline decoder for the observed static PSG3/PST1 asset subset.

No guest runtime or supplied CarsTools executable is loaded. Unsupported format
variants fail closed. Render semantics still requiring reference validation are
reported in provenance; an exported candidate is not an accepted game asset.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import math
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MAX_ASSET = 8 * 1024 * 1024


class FormatError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise FormatError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def unpack(fmt, blob, offset):
    n = struct.calcsize(fmt)
    require(0 <= offset <= len(blob) - n, f'truncated {fmt} at {offset}')
    return struct.unpack_from(fmt, blob, offset)


def string(blob):
    require(b'\0' in blob, 'unterminated fixed string')
    text, padding = blob.split(b'\0', 1)
    require(not any(padding), 'nonzero fixed-string suffix')
    try:
        return text.decode('ascii')
    except UnicodeDecodeError as exc:
        raise FormatError('non-ASCII fixed string') from exc


def extract_members(container, names):
    # Existing verifier has no import-time writes. Keep bytecode disabled when
    # importing it so the original reference tree remains unchanged.
    sys.dont_write_bytecode = True
    scripts = ROOT / 'work/native/project/scripts'
    sys.path.insert(0, str(scripts))
    try:
        from validate_ui_resources07 import decode
    finally:
        sys.path.pop(0)
    require(28 <= container.stat().st_size <= 64*1024*1024, 'unsupported RES file size')
    raw = container.read_bytes()
    rows, blocks = decode(raw)
    flat = b''.join(b['data'] for b in blocks)
    output = {}
    for name in names:
        matches = [r for r in rows if r['name'].casefold() == name.casefold()]
        require(len(matches) == 1, f'member missing/ambiguous: {name}')
        row = matches[0]
        require(0 < row['bytes'] <= MAX_ASSET, 'unsupported member size')
        data = flat[row['logical_offset']:row['logical_offset'] + row['bytes']]
        output[name] = (data, {**row, 'sha256': sha(data)})
    return output, {'path': str(container.resolve()), 'sha256': sha(raw),
                    'bytes': len(raw), 'decoded_bytes': len(flat)}


def triangle_list(xyzw, base):
    """Recover separate source strips from the observed -16,-16,+16 pattern.

    ADC vertices enter the rolling window without emitting a triangle. Native
    winding is normalized to each source-strip start, independently checked
    against the original stored normals by decode_psg3(). No degenerate triangles
    are removed and no points are deduplicated or manufactured.
    """
    indices, lengths = [], []
    start = 0
    while start < len(xyzw):
        require(start + 2 < len(xyzw), 'strip has fewer than three vertices')
        require(xyzw[start][3] == xyzw[start + 1][3] == -16,
                'strip must start with exactly two suppressed vertices')
        end = start + 2
        while end < len(xyzw) and xyzw[end][3] == 16:
            ids = [base + end - 2, base + end - 1, base + end]
            if (end - start) & 1:
                ids[0], ids[1] = ids[1], ids[0]
            indices.extend(ids)
            end += 1
        require(end > start + 2, 'empty strip')
        lengths.append(end - start)
        start = end
    return indices, lengths


def decode_psg3(blob):
    require(368 <= len(blob) <= MAX_ASSET, 'unsupported PSG size')
    require(blob[:8] == b'psg\0\x03\0\0\0', 'expected PSG version 3')
    require(unpack('<I', blob, 8)[0] == 1, 'only one static hierarchy object supported')
    name = string(blob[12:140])
    matrix = unpack('<16f', blob, 140)
    require(matrix == (1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.),
            'non-identity hierarchy transform requires separate recovery')
    bounds = unpack('<7fi', blob, 204)
    require(all(math.isfinite(x) for x in bounds[:7]), 'nonfinite hierarchy bounds')
    require(all(x >= 0 for x in bounds[3:7]) and bounds[7] == -1,
            'invalid bounds or unsupported parent')
    require(unpack('<I', blob, 236)[0] == 1, 'only one material supported')
    material = string(blob[240:304])
    quant = unpack('<7fI', blob, 304)
    require(all(math.isfinite(x) for x in quant[:7]), 'nonfinite quantization')
    require(all(x > 0 for x in quant[:4]), 'nonpositive quantization scale')
    require(quant[7] in (0, 1), 'invalid world-space flag')
    require(quant[7] == 1 or quant[4:7] == (0., 0., 0.),
            'local mesh with nonzero quantization origin needs recovery')
    # Exact observed static, one-LOD, one-surface prefix. Reject variants instead
    # of silently treating animation/instancing/indexed geometry as this subset.
    require(unpack('<III', blob, 336) == (1, 0x7f7fffff, 1), 'unsupported LOD layout')
    require(unpack('<II', blob, 348) == (0, 0x00ff010f), 'unsupported surface flags')
    uv_size = unpack('<f', blob, 356)[0]
    require(math.isfinite(uv_size) and uv_size > 0, 'invalid UV-size metadata')
    debug_vertices, debug_strips, payload_size = unpack('<HHI', blob, 360)
    require(0 < debug_vertices <= 65535 and 0 < debug_strips <= debug_vertices // 3,
            'invalid debug counts')
    require(payload_size == len(blob) - 368 and payload_size % 16 == 0,
            'PSG payload extent/alignment mismatch')
    pos = 368
    commands, groups, vertices, normals, indices = [], [], [], [], []

    def take_unpack(command, count, address, components, fmt):
        nonlocal pos
        at = pos
        word = unpack('<I', blob, pos)[0]
        pos += 4
        expected = (command << 24) | (count << 16) | address
        require(word == expected, f'unsupported VIF command at {at}: {word:08x}, expected {expected:08x}')
        nbytes = struct.calcsize(fmt) * count
        padded = (nbytes + 3) & ~3
        require(pos + padded <= len(blob), 'truncated VIF payload')
        values = [unpack(fmt, blob, pos + i * struct.calcsize(fmt)) for i in range(count)]
        require(all(len(v) == components for v in values), 'decoder component mismatch')
        require(not any(blob[pos + nbytes:pos + padded]), 'nonzero VIF alignment padding')
        commands.append({'offset': at, 'word': f'{word:08x}', 'payload_bytes': nbytes,
                         'padding_bytes': padded - nbytes})
        pos += padded
        return values

    while pos < len(blob) and unpack('<I', blob, pos)[0] != 0:
        group_start = pos
        tag = take_unpack(0x6c, 1, 0x8000, 4, '<4I')[0]
        n = tag[0] & 0x7fff
        require(3 <= n <= 128, 'invalid packed group vertex count')
        require(tag == (0x8000 | n, 0x30024000, 0x412, 0), 'unsupported GIF template')
        xyz = take_unpack(0x6d, n, 0x8001, 4, '<4h')
        ns = take_unpack(0x6a, n, 0x8001 + n, 3, '<3b')
        uv = take_unpack(0x65, n, 0x8001 + 2*n, 2, '<2h')
        rgba = take_unpack(0x6e, n, 0xc001 + 3*n, 4, '<4B')
        require(unpack('<I', blob, pos)[0] == 0x17000000, 'missing end-group MSCNT')
        commands.append({'offset': pos, 'word': '17000000', 'payload_bytes': 0, 'padding_bytes': 0})
        pos += 4
        base = len(vertices)
        require(base + n <= debug_vertices, 'packed groups exceed declared vertex limit')
        new_indices, lengths = triangle_list(xyz, base)
        indices.extend(new_indices)
        for p, normal, st, color in zip(xyz, ns, uv, rgba):
            require(p[3] in (-16, 16), 'unsupported original bone/ADC encoding')
            out = tuple(p[j] * quant[0] + quant[4+j] for j in range(3))
            out += (st[0] * quant[2], st[1] * quant[3]) + tuple(color)
            require(all(math.isfinite(x) for x in out[:5]), 'nonfinite output vertex')
            vertices.append(out)
            normals.append(tuple(k * quant[1] for k in normal))
        groups.append({'offset': group_start, 'vertices': n, 'strip_lengths': lengths,
                       'adc_words_signed': [p[3] for p in xyz]})
    require(len(blob) - pos < 16 and not any(blob[pos:]), 'unexpected trailing VIF data')
    require(len(vertices) == debug_vertices, 'header vertex count mismatch')
    strip_count = sum(len(g['strip_lengths']) for g in groups)
    require(strip_count == debug_strips, 'header strip count mismatch')
    require(len(indices) == 3 * (debug_vertices - 2 * debug_strips), 'topology count mismatch')
    out_min = [min(v[a] for v in vertices) for a in range(3)]
    out_max = [max(v[a] for v in vertices) for a in range(3)]
    expected_min = [bounds[a] - bounds[a+3] for a in range(3)]
    expected_max = [bounds[a] + bounds[a+3] for a in range(3)]
    bound_error = max(abs(a-b) for a,b in zip(out_min+out_max, expected_min+expected_max))
    # Exporter stores pre-quantization bounds. Truncation-to-zero can change an
    # endpoint by less than one position quantum; float storage adds small error.
    require(bound_error <= quant[0] * 1.01 + 0.002, 'decoded bounds disagree with stored hierarchy')
    signs = {'positive': 0, 'negative': 0, 'zero': 0}
    for i in range(0, len(indices), 3):
        ids = indices[i:i+3]
        a,b,c = [vertices[k] for k in ids]
        u = [b[j]-a[j] for j in range(3)]
        v = [c[j]-a[j] for j in range(3)]
        cross = (u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0])
        normal = [sum(normals[k][j] for k in ids) for j in range(3)]
        dot = sum(cross[j]*normal[j] for j in range(3))
        signs['positive' if dot > 0 else 'negative' if dot < 0 else 'zero'] += 1
    require(signs['negative'] == 0, 'recovered winding disagrees with stored normals')
    report = {'object_name': name, 'material': material, 'matrix': matrix,
              'stored_bounds_center_half_radius_parent': bounds, 'quantization': quant,
              'surface_prefix_hex': blob[336:368].hex(), 'uv_size_uninterpreted': uv_size,
              'vertices': len(vertices), 'triangles': len(indices)//3, 'strips': strip_count,
              'groups': groups, 'commands': commands, 'trailing_zero_padding': len(blob)-pos,
              'decoded_bounds_min': out_min, 'decoded_bounds_max': out_max,
              'max_bounds_error': bound_error, 'normal_alignment': signs,
              'color_range': [[min(v[5+k] for v in vertices), max(v[5+k] for v in vertices)] for k in range(4)],
              'uv_range': [[min(v[3+k] for v in vertices), max(v[3+k] for v in vertices)] for k in range(2)],
              'status': 'candidate: bounded structural and independent bounds/normal checks pass',
              'unknowns': ['Original VU shader UV/color arithmetic and material defaults require differential capture validation.',
                           'Per-source-strip winding agrees with stored normals; original viewport/cull convention not accepted.',
                           'Normals are retained in provenance, not in the minimal RRMESH01 contract.',
                           'Stored UV-size metadata has not been interpreted; shader may use it for LOD only, unproven.'],
              'normals': normals}
    return vertices, indices, report


def decode_pst1(blob):
    require(12 <= len(blob) <= MAX_ASSET, 'unsupported PST size')
    revision, flags, kind, mips = unpack('<HHII', blob, 0)
    require((revision, flags, kind) == (1, 0, 1), 'only observed PST1 flags0 type1 palette8 supported')
    require(1 <= mips <= 7 and len(blob) >= 1036, 'invalid PST mip count/palette')
    palette = [tuple(blob[12+i*4:16+i*4]) for i in range(256)]
    pos, surfaces = 1036, []
    prev = None
    for level in range(mips):
        h,w,a,b = unpack('<4I', blob, pos)
        require(0 < w <= 1024 and 0 < h <= 1024 and not(w & (w-1)) and not(h & (h-1)),
                'invalid PST dimensions')
        require((a,b) == (0,0), 'unrecognized PST1 surface words')
        require(prev is None or (w,h) == (prev[0]*2, prev[1]*2), 'invalid ascending mip dimensions')
        start = pos + 16
        require(start + w*h <= len(blob), 'truncated PST indices')
        data = blob[start:start+w*h]
        surfaces.append({'level_smallest_first': level, 'header_offset': pos,
                         'width': w, 'height': h, 'bytes': len(data), 'sha256': sha(data),
                         'indices': data})
        prev = (w,h)
        pos = start + w*h
    require(pos == len(blob), 'PST unconsumed suffix')
    report = {'revision': revision, 'flags': flags, 'kind': kind, 'palette_offset': 12,
              'palette_sha256': sha(blob[12:1036]), 'mip_count': mips,
              'surfaces': [{k:v for k,v in s.items() if k != 'indices'} for s in surfaces],
              'status': 'structural layout accepted for this subset; pixel interpretation candidate',
              'unknowns': ['Original PST1 palette permutation, index swizzle, row origin, RGB/alpha scaling require loader/capture validation.',
                           'Raw palette bytes and indices are preserved; output variants make interpretation explicit.',
                           'No claim of compatibility with CarsTools PST revision2.']}
    return palette, surfaces, report


def mesh_bytes(vertices, indices):
    return (struct.pack('<8sII', b'RRMESH01', len(vertices), len(indices)) +
            b''.join(struct.pack('<5f4B', *v) for v in vertices) +
            struct.pack('<'+'I'*len(indices), *indices))


def texture_bytes(palette, surface, swap_palette_bits=False):
    def index(i):
        return (i & ~24) | ((i & 8) << 1) | ((i & 16) >> 1) if swap_palette_bits else i
    return (struct.pack('<8sII', b'RRTEX001', surface['width'], surface['height']) +
            bytes(c for i in surface['indices'] for c in palette[index(i)]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--container', type=Path, default=ROOT/'work/native/project/disc/TRACKS/TA.RES')
    parser.add_argument('--mesh', default=r'geometry\zone01groundstrips16.psg')
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    members, container = extract_members(args.container, [args.mesh])
    original, mesh_source = members[args.mesh]
    vertices, indices, mesh = decode_psg3(original)
    material_name = 'textures\\' + mesh['material'] + '.psm'
    texture_name = 'textures\\' + mesh['material'] + '.pst'
    members, _ = extract_members(args.container, [material_name, texture_name])
    material, material_source = members[material_name]
    text = material.decode('ascii')
    fields = {}
    for line in text.splitlines():
        line = line.split(';', 1)[0]
        if '=' in line:
            k,v = line.split('=', 1)
            require(k.strip() not in fields, 'duplicate material key')
            fields[k.strip()] = v.strip()
    require(fields == {'TotalShaderPasses': '1', 'microProgramName': 'PreLitCompressed',
                       'TextureMap_1': mesh['material']}, 'unsupported material variant')
    texture, texture_source = members[texture_name]
    palette, surfaces, tex = decode_pst1(texture)
    args.output.mkdir(parents=True, exist_ok=True)
    outputs = {}
    data_outputs = {'mesh.rrmesh': mesh_bytes(vertices, indices),
                    'texture-linear-palette.rrtex': texture_bytes(palette, surfaces[-1]),
                    'texture-csm1-palette.rrtex': texture_bytes(palette, surfaces[-1], True),
                    'original.psg': original, 'original.pst': texture, 'original.psm': material}
    for name, data in data_outputs.items():
        target = args.output/name
        target.write_bytes(data)
        require(target.read_bytes() == data, 'output readback mismatch')
        outputs[name] = {'path': str(target.resolve()), 'bytes': len(data), 'sha256': sha(data)}
    report = {'container': container, 'members': {'mesh': mesh_source, 'material': material_source,
              'texture': texture_source}, 'mesh': mesh, 'material': fields, 'texture': tex,
              'outputs': outputs, 'converter_sha256': sha(Path(__file__).read_bytes()),
              'scope': 'Offline original static asset candidates. No reference runtime executed, no generated replacement geometry.'}
    (args.output/'conversion.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps({'output': str(args.output.resolve()), 'vertices': len(vertices),
                      'triangles': len(indices)//3, 'bounds_error': mesh['max_bounds_error'],
                      'normal_alignment': mesh['normal_alignment'], 'texture_status': tex['status']}))


if __name__ == '__main__':
    main()
