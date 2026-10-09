"""Independently rederive every imported mesh from original bytes with frozen Python decoding."""
from pathlib import Path
import argparse
import hashlib
import json
import sys
import struct

ROOT = Path(__file__).resolve().parents[1]
if not __debug__:
    raise RuntimeError('Cache verification requires normal Python execution')
sys.dont_write_bytecode = True
sys.path.insert(0, str(ROOT/'tools/assets'))
from decode_scene48c import decode_psg3_scene, export_lod, mesh_bytes
sys.path.insert(0, str(ROOT/'work/native/project/scripts'))
from validate_ui_resources07 import decode


def sha(data):
    return hashlib.sha256(data).hexdigest()


def ordered_bind_matrices48d(scene):
    """Canonical native contract; Python3.12+ sum uses compensated accumulation.

    Preserve/report every discrepancy; this is not a numerical tolerance.
    Original frozen mesh bytes are independently checked before this operation.
    """
    differences = []
    for node in scene['nodes']:
        original = node['bind_matrix']
        if node['parent'] < 0:
            expected = node['matrix']
        else:
            parent = scene['nodes'][node['parent']]['bind_matrix']; expected = []
            for row in range(4):
                for col in range(4):
                    value = 0.0
                    for k in range(4):
                        value += node['matrix'][row*4+k] * parent[k*4+col]
                    expected.append(value)
        for i, (a,b) in enumerate(zip(original, expected)):
            if struct.pack('<d', a) != struct.pack('<d', b):
                differences.append(dict(node=node['index'],name=node['name'],component=i,
                    python_sum_hex=a.hex(),ordered_hex=b.hex(),absolute_difference=abs(a-b)))
        node['bind_matrix'] = expected
    return differences


def scene_bytes(scene, mesh, spans):
    """Independent RRSCENE1 encoding of frozen Python decoder semantic values."""
    out = bytearray(b'RRSCENE1')
    def add(fmt, *values):
        out.extend(struct.pack('<'+fmt, *values))
    def name(value):
        raw = value.encode('ascii'); add('I', len(raw)); out.extend(raw)
    add('8I', 1, scene['quantization'][7], scene['matrix_palette'] is not None,
        len(scene['nodes']), len(scene['materials']), len(scene['lods']),
        scene['decoded_vertices_all_lods'], scene['surfaces_all_lods'])
    add('7d', *scene['quantization'][:7])
    for node in scene['nodes']:
        name(node['name']); add('i', node['parent'])
        add('39d', *node['matrix'], *node['bind_matrix'], *node['bounds_center_half_radius'])
    for material in scene['materials']:
        name(material)
    for lod in scene['lods']:
        add('dI', lod['cutoff'], len(lod['surfaces']))
        for surface in lod['surfaces']:
            add('IIdII', surface['material_index'], surface['original_colors'][0] is not None,
                surface['uv_size'], len(surface['vertices_local']), len(surface['indices']))
            for v, n, c, bone in zip(surface['vertices_local'],surface['normals_local'],surface['original_colors'],surface['bone_indices']):
                add('8d4BH', *v[:3], *n, *v[3:5], *(c if c is not None else (0,0,0,0)), bone)
            add(str(len(surface['indices']))+'I', *surface['indices'])
    add('I', len(spans))
    for span in spans:
        add('6I', *(span[k] for k in ['material_index','vertex_start','vertex_count','index_start','index_count','original_vertex_colors']))
    add('I', len(mesh)); out.extend(mesh)
    return bytes(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--cache', type=Path, required=True)
    ap.add_argument('--source', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    args = ap.parse_args()
    current = json.loads((args.cache/'current.json').read_text())
    path = args.cache/'generations'/(current['generation']+'.json')
    assert sha(path.read_bytes()) == current['manifest_sha256'] == current['generation']
    manifest = json.loads(path.read_text())
    by_container = {}
    for member in manifest['members']:
        by_container.setdefault(member['container'], []).append(member)
    checked, reused_derivations = [], {}
    total_members = 0
    arithmetic_differences = []
    for container in manifest['containers']:
        raw = (args.source/container['name']).read_bytes()
        assert sha(raw) == container['sha256']
        if container['status'] != 'decoded':
            continue
        rows, blocks = decode(raw)
        flat = b''.join(b['data'] for b in blocks)
        indexed = {(r['name'],r['logical_offset'],r['bytes']) for r in rows}
        for member in by_container[container['name']]:
            key = (member['member'], member['logical_offset'], member['source_bytes'])
            assert key in indexed
            data = flat[key[1]:key[1]+key[2]]
            assert sha(data) == member['source_sha256']
            total_members += 1
            if member['status'] != 'converted_geometry':
                continue
            record = member['native']
            fields = dict(record); digest = fields.pop('record_sha256')
            assert sha(json.dumps(fields,sort_keys=True,separators=(',',':'),ensure_ascii=False).encode()) == digest
            if member['source_sha256'] not in reused_derivations:
                report = decode_psg3_scene(data)
                vertices, indices, spans = export_lod(report)
                expected = mesh_bytes(vertices, indices)
                differences = ordered_bind_matrices48d(report)
                if differences:
                    arithmetic_differences.append(dict(container=container['name'],member=member['member'],
                        source_sha256=member['source_sha256'],fields=differences))
                expected_scene = scene_bytes(report, expected, spans)
                # Direct byte comparison on every distinct original object, then
                # retain only digests/metadata for duplicate references.
                assert (args.cache/'objects'/(record['key']+'.rrmesh')).read_bytes() == expected
                actual_scene = (args.cache/'objects'/(record['key']+'.rrscene')).read_bytes()
                if actual_scene != expected_scene:
                    mismatch = next((i for i,(a,b) in enumerate(zip(actual_scene, expected_scene)) if a != b), min(len(actual_scene),len(expected_scene)))
                    raise AssertionError((container['name'],member['member'],record['key'],len(actual_scene),len(expected_scene),mismatch,actual_scene[mismatch:mismatch+16].hex(),expected_scene[mismatch:mismatch+16].hex()))
                reused_derivations[member['source_sha256']] = (sha(expected), sha(expected_scene),
                    bool(report['quantization'][7]), report['materials'])
            mesh_hash, scene_hash, world_space, materials = reused_derivations[member['source_sha256']]
            cached = (args.cache/'objects'/(record['key']+'.rrmesh')).read_bytes()
            semantic = (args.cache/'objects'/(record['key']+'.rrscene')).read_bytes()
            assert sha(cached) == record['mesh_sha256'] == mesh_hash
            assert sha(semantic) == record['scene_sha256'] == scene_hash
            assert record['world_space'] == world_space and record['materials'] == materials
            checked.append(dict(container=container['name'], member=member['member'],
                                source_sha256=sha(data), mesh_sha256=sha(cached), scene_sha256=sha(semantic)))
    result = dict(passed=True, scope='mesh bytes match frozen48C; scene bytes match frozen48C semantic decode with explicit ordered48D matrix arithmetic; not material fidelity',
        current_generation=current['generation'], containers=len(manifest['containers']), original_members_verified=total_members,
        imported_members_byte_identical=len(checked), unique_original_derivations=len(reused_derivations),
        original_source=str(args.source.resolve()), cache=str(args.cache.resolve()), meshes=checked,
        python_version=sys.version, matrix_arithmetic_differences=arithmetic_differences,
        arithmetic_policy='Explicit left-to-right binary64 addition matches native48D. Frozen decoder is unchanged; no blanket tolerance.')
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('meshes','matrix_arithmetic_differences')}))
    print('Objects with compensated-vs-ordered matrix differences:',len(arithmetic_differences))


if __name__ == '__main__':
    main()
