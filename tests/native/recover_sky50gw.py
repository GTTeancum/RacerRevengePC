#!/usr/bin/env python3
"""Recover actual TA CUB tiles. No generated imagery or substituted geometry.

The original 1295B0/1289C0/1290E0 loaders frame the retained CUB, and
12A8A0 supplies the cube face geometry. The retained 50FC IPUM decoder is used
only for the original compressed images. No guest code is executed.
"""
import hashlib
import json
from pathlib import Path
import struct
import sys
import zlib

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/assets'))
from decode_source_textures50fc import decode_ipu

TA_SHA = 'b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8'
CUB_SHA = '27e43db810442e9fd90fda8551e6804ab68d48e8398992a1b2997945e88d68f4'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'

def need(ok, message):
    if not ok:
        raise ValueError(message)

def sha(data):
    return hashlib.sha256(data).hexdigest()

def extract_cub():
    b = (ROOT / 'work/native/project/disc/TRACKS/TA.RES').read_bytes()
    need(sha(b) == TA_SHA, 'TA archive identity')
    version, meta, codec, flags, table = struct.unpack_from('<5I', b)
    need((version, codec, flags) == (3, 0x340, 0) and table % 2 == 0 and
         20 + table == 8 + meta, 'RES framing')
    sizes = struct.unpack_from('<' + 'H' * (table // 2), b, 20)
    need(sizes[-1] == 0 and all(sizes[:-1]), 'RES block table')
    at = 20 + table
    count, index_size = struct.unpack_from('<2I', b, at)
    at += 8
    end = at + index_size
    need(count == 383, 'TA directory count')
    selected = []
    for _ in range(count):
        length, = struct.unpack_from('<I', b, at)
        at += 4
        need(0 < length <= 1024 and at + length + 8 <= end, 'RES name extent')
        name = b[at:at + length].decode('ascii')
        at += length
        off, length = struct.unpack_from('<2I', b, at)
        at += 8
        if name == 'skies\\ta.cub':
            selected.append((off, length))
    need(at == end and selected == [(9043968, 164187)], 'Exact CUB resource')
    flat = bytearray()
    for length in sizes[:-1]:
        need(at + length < len(b) and b[at + length] == 255, 'RES compressed extent')
        d = zlib.decompressobj()
        raw = d.decompress(b[at:at + length], 24577)
        need(len(raw) == 24576 and d.eof and not d.unused_data and not d.unconsumed_tail,
             'RES decoded block')
        flat.extend(raw)
        at += length + 1
    need(at == len(b), 'RES complete consumption')
    off, length = selected[0]
    cub = bytes(flat[off:off + length])
    need(sha(cub) == CUB_SHA, 'CUB identity')
    return cub

def tiles(cub):
    need(struct.unpack_from('<4I', cub) == (1, 1, 16, 99100), 'CUB tables')
    # Palette 2E38C0: start/count/compressed,768RGB,32768lookup,65536lookup.
    need(struct.unpack_from('<3I', cub, 16) == (0, 81, 0), 'CUB palette header')
    need(16 + 12 + 768 + 32768 + 65536 == 99100, 'CUB palette extent')
    at = 99100
    need(struct.unpack_from('<IHH', cub, at) == (32, 0, 0), 'CUB IPUM format/flags')
    masks = struct.unpack_from('<6H', cub, at + 8)
    sizes = struct.unpack_from('<6I', cub, at + 20)
    need(masks == (255, 255, 255, 255, 65535, 0), 'Authored face masks')
    need(sizes == (0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaaaaaa, 0), 'Authored tile sizes')
    need(struct.unpack_from('<9f', cub, at + 44) == (0., 0., 0., 0., 0., 1., 0., 1., 0.),
         'Authored origin/look/up')
    at += 80
    result = []
    for face, mask in enumerate(masks):
        for tile in range(16):
            if not mask & (1 << tile):
                continue
            start = at
            need((sizes[face] >> (tile * 2)) & 3 == 2, 'Exact 128-pixel tile')
            # 128D50 reads six compressed descriptor words, then 24F2C0 reads
            # the four unused smaller-level words before each IPUM byte count.
            need(struct.unpack_from('<11I', cub, at) == (0, 32, 0, 16, 4096, 4, 0, 0, 0, 0, 0),
                 'CUB tile descriptor')
            at += 44
            mips = []
            for dimension in (16, 32, 64, 128):
                count, = struct.unpack_from('<I', cub, at)
                at += 4
                need(24 <= count <= 65536 and at + count <= len(cub), 'CUB IPUM extent')
                ipu = cub[at:at + count]
                magic, size, width, height, frames = struct.unpack_from('<4sIHHI', ipu)
                need((magic, size, width, height, frames) ==
                     (b'ipum', count - 8, dimension, dimension, 1), 'CUB IPUM header')
                need(ipu[-8:] == b'\0\0\1\xb0\0\0\1\xb1' and ipu[16] == 0,
                     'CUB IPUM frame markers')
                mips.append(dict(offset=at, bytes=count, dimension=dimension, sha256=sha(ipu)))
                at += count
            result.append(dict(face=face, tile=tile, offset=start, bytes=at-start,
                               sha256=sha(cub[start:at]), mips=mips))
    need(len(result) == 48 and at == len(cub), 'Complete authored tile coverage')
    return result

def main():
    cub = extract_cub()
    records = tiles(cub)
    output = ROOT / 'evidence/sky50GW'
    output.mkdir(parents=True, exist_ok=True)
    evidence = ROOT / 'solo-sky50gw'
    evidence.mkdir(exist_ok=True)
    (evidence / 'original-ta.cub').write_bytes(cub)
    inc = []
    for record in records:
        mip = record['mips'][-1]
        data = cub[mip['offset']:mip['offset'] + mip['bytes']]
        rgba, yuv_sha = decode_ipu(data, 128, 128, 'ffmpeg')
        record['yuv_sha256'] = yuv_sha
        rrtex = struct.pack('<8sII', b'RRTEX001', 128, 128) + rgba
        filename = f'face{record["face"]}-tile{record["tile"]:02d}.rrtex'
        (output / filename).write_bytes(rrtex)
        record['decoded'] = dict(filename=filename, sha256=sha(rrtex), bytes=len(rrtex), width=128, height=128)
        inc.append('    {%d, %d, %d, %d, "%s", "%s", "%s"},' %
                   (record['face'], record['tile'], record['offset'], record['bytes'],
                    record['sha256'], filename, sha(rrtex)))
    (ROOT / 'tests/native/sky50gw_index.inc').write_text('\n'.join(inc) + '\n')
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    need(sha(elf) == ELF_SHA, 'Original ELF identity')
    ranges = {'cube_loader':(0x1295b0,0x129998), 'cube_header':(0x1289c0,0x128cc4),
              'tile_loader':(0x128d50,0x1291b0), 'cube_geometry':(0x12a8a0,0x12acb4),
              'tile_uv':(0x12b34c,0x12b6e0), 'sky_offset':(0x274bb0,0x274bdc),
              'camera_follow':(0x288b00,0x288b84)}
    source = {}
    for label,(a,z) in ranges.items():
        data = elf[a-0x100000+0x80:z-0x100000+0x80]
        (evidence / (label + '.bin')).write_bytes(data)
        source[label] = dict(start=hex(a),end=hex(z),sha256=sha(data))
    manifest = dict(archive_sha256=TA_SHA,member='skies\\ta.cub',member_sha256=CUB_SHA,
                    member_bytes=len(cub),elf_sha256=ELF_SHA,source=source,tiles=records,
                    half_extent=25600,camera_relative_y_offset=1024,face_order=['+Z','-X','-Z','+X','+Y','-Y'],
                    source_uv_range=[0.03125,0.96875],rgba_bytes=48*128*128*4,
                    limitations=['Original IPUM coefficients decoded through retained 50FC FFmpeg scalar path.',
                                 'PS2 hardware inverse-DCT rounding is not framebuffer-validated.',
                                 'Native opaque background rendering is not original VU/GS rasterization certification.'])
    (output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps(dict(status='recovered',tiles=len(records),rgba_bytes=manifest['rgba_bytes'],source_sha256=CUB_SHA)))

if __name__ == '__main__':
    main()
