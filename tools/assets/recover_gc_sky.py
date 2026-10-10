#!/usr/bin/env python3
"""Cold-convert GC's actual SKY21.CUB using the existing 50GW/50FC decoders.

No guest execution or game/test process. This retains real decoded source
images and their identities; it does not claim PS2 hardware IDCT parity.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.dont_write_bytecode = True


# Same bounded CUB framing as 50GW, with GC's authenticated palette count
# and descriptor words. Each IPUM byte stream goes to the unchanged decoder.
def gc_tiles(cub):
    from decode_source_textures50fc import need, sha
    need(struct.unpack_from('<4I', cub) == (1, 1, 16, 99100), 'CUB tables')
    # Palette 2E38C0: start/count/compressed,768RGB,32768lookup,65536lookup.
    need(struct.unpack_from('<3I', cub, 16) == (0, 216, 0), 'CUB palette header')
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
            need(struct.unpack_from('<11I', cub, at) == (0, 34, 0, 16, 16, 4, 0, 0, 0, 0, 0),
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
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--intake', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    args = parser.parse_args()
    sys.path.insert(0, str(args.repo / 'tests/native'))
    sys.path.insert(0, str(args.repo / 'tools/assets'))
    from decode_source_textures50fc import decode_ipu, need, sha
    archive = (args.intake / 'original/GC.RES').read_bytes()
    need(sha(archive) == '69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9', 'GC archive identity')
    cub = (args.intake / 'intake/original-members/skies/sky21.cub').read_bytes()
    need(sha(cub) == '676a6601d962d66210d80f0ca97f2d6ad7a7b6eb43d7ba55c164a4792d6b392c', 'GC SKY21 identity')
    scb = (args.intake / 'intake/original-members/tracks/gc.scb').read_bytes()
    need(sha(scb) == 'd47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d', 'GC SCB identity')
    need(scb[27583:27593] == b'sky21.cub\0' and scb[27716:27728] == b'151,160,177\0', 'GC sky and fog source fields')
    records = gc_tiles(cub)
    output = args.output_root / 'evidence/skyGC'
    output.mkdir(parents=True, exist_ok=True)
    index = []
    for record in records:
        mip = record['mips'][-1]
        data = cub[mip['offset']:mip['offset'] + mip['bytes']]
        rgba, yuv_sha = decode_ipu(data, 128, 128, 'ffmpeg')
        rrtex = struct.pack('<8sII', b'RRTEX001', 128, 128) + rgba
        filename = f'face{record["face"]}-tile{record["tile"]:02d}.rrtex'
        (output / filename).write_bytes(rrtex)
        record['yuv_sha256'] = yuv_sha
        record['decoded'] = dict(filename=filename, sha256=sha(rrtex), bytes=len(rrtex), width=128, height=128)
        index.append('    {%d, %d, %d, %d, "%s", "%s", "%s"},' %
                     (record['face'], record['tile'], record['offset'], record['bytes'], record['sha256'], filename, sha(rrtex)))
    header = args.output_root / 'tests/native/sky_gc_index.inc'
    header.parent.mkdir(parents=True, exist_ok=True)
    header.write_text('\n'.join(index) + '\n')
    manifest = dict(schema='RR_ORIGINAL_GC_SKY_V1', archive_sha256=sha(archive), archive_bytes=len(archive),
                    member='skies\\sky21.cub', member_sha256=sha(cub), member_bytes=len(cub),
                    scene_member='tracks\\gc.scb', scene_sha256=sha(scb), scene_bytes=len(scb),
                    scene_sky_field_offset=27583, fog_rgb_offset=27716, fog_rgb=[151, 160, 177],
                    tile_count=len(records), rgba_bytes=len(records)*128*128*4, tiles=records,
                    half_extent=25600, camera_relative_y_offset=1024,
                    face_order=['+Z', '-X', '-Z', '+X', '+Y', '-Y'], source_uv_range=[0.03125, 0.96875],
                    source_parser_sha256=hashlib.sha256((args.repo/'tests/native/recover_sky50gw.py').read_bytes()).hexdigest(),
                    decoder_sha256=hashlib.sha256((args.repo/'tools/assets/decode_source_textures50fc.py').read_bytes()).hexdigest(),
                    limitations=['Existing FFmpeg scalar IPUM path; PS2 hardware IDCT precision is not framebuffer-validated.',
                                 'No original game, native client, shader or renderer execution occurred.'])
    (output/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    print(json.dumps(dict(status='converted_original_sky', tiles=len(records), rgba_bytes=manifest['rgba_bytes'], source_sha256=sha(cub))))


if __name__ == '__main__':
    main()
