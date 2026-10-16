#!/usr/bin/env python3
"""Root-only original RA sky cold preparation. Never runs guest instructions.

Unexecuted source candidate. Full archive/CUB identities authenticate every
input; the existing scalar native50FC decoder owns pixel conversion. Outputs
must be centrally reviewed and pinned before the native sky owner can load.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import zlib
sys.dont_write_bytecode = True
ARCHIVE_BYTES = 4604561
ARCHIVE_SHA = '1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2'
SOURCE_MEMBER = 'skies\\ra.cub'
SOURCE_BYTES = 161825
SOURCE_SHA = 'c497927d2fe78de6f2f9bbf9e319ac0ea2b4939e020df45a4ae570e5abb73960'
HELPER_SHA = '5f9af6eba0f07732f913cde7aa40e246d218d38230f57a7aacc027afd25a7c49'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read_exact(path, count, digest):
    need(path.is_file() and path.stat().st_size == count, 'Input extent: ' + str(path))
    data = path.read_bytes()
    need(len(data) == count and sha(data) == digest, 'Input identity: ' + str(path))
    return data


def sky_from_ra(blob):
    # Bounded v3 RES archive framing, matching the existing authenticated
    # archive readers. No asset converter or project module is imported.
    def u32(at):
        need(0 <= at <= len(blob) - 4, 'Truncated archive integer')
        return struct.unpack_from('<I', blob, at)[0]
    need((u32(0), u32(8), u32(12)) == (3, 0x340, 0), 'RA RES header')
    meta, table_bytes = u32(4), u32(16)
    need(2 <= table_bytes <= 16384 and table_bytes % 2 == 0 and
         20 + table_bytes == 8 + meta <= len(blob), 'RES metadata')
    sizes = struct.unpack_from('<' + 'H' * (table_bytes // 2), blob, 20)
    need(sizes[-1] == 0 and all(sizes[:-1]) and len(sizes) * 24576 <= 128 * 1024 * 1024,
         'RES block table')
    at = 8 + meta
    count, index_bytes = u32(at), u32(at + 4)
    at += 8
    end = at + index_bytes
    need(count == 1010 and end <= len(blob), 'RA index extent/count')
    records = []
    for _ in range(count):
        n = u32(at)
        at += 4
        need(0 < n <= 1024 and at + n + 8 <= end, 'RES name extent')
        name = blob[at:at + n].decode('ascii')
        need('\0' not in name, 'RES name terminator')
        at += n
        records.append((name, u32(at), u32(at + 4)))
        at += 8
    need(at == end, 'RES index suffix')
    blocks = []
    for size in sizes[:-1]:
        need(at + size < len(blob) and blob[at + size] == 255, 'RES block framing')
        decoder = zlib.decompressobj()
        block = decoder.decompress(blob[at:at + size], 24577)
        need(decoder.eof and not decoder.unused_data and not decoder.unconsumed_tail and
             len(block) == 24576, 'RES decompressed block')
        blocks.append(block)
        at += size + 1
    need(at == len(blob), 'RES trailing bytes')
    data = b''.join(blocks)
    need(len(data) == 29687808 and all(start <= len(data) and length <= len(data) - start
         for _, start, length in records), 'RA logical extents')
    matches = [record for record in records if record[0].replace('/', '\\').lower() == SOURCE_MEMBER]
    need(len(matches) == 1 and matches[0][0] == SOURCE_MEMBER, 'Exact unique RA member')
    _, start, length = matches[0]
    source = data[start:start + length]
    need(length == SOURCE_BYTES and sha(source) == SOURCE_SHA, 'RA sky source identity')
    return source


def write_new(path, data):
    with path.open('xb') as output:
        output.write(data)
    need(path.read_bytes() == data, 'Output readback: ' + str(path))


def sky_tiles(cub):
    need(struct.unpack_from('<4I', cub) == (1, 1, 16, 99100), 'CUB tables')
    # Palette 2E38C0: start/count/compressed,768RGB,32768lookup,65536lookup.
    start, count, compressed = struct.unpack_from('<3I', cub, 16)
    need(start == 0 and 1 <= count <= 256 and compressed == 0, 'Authenticated CUB palette header')
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
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--ra-archive', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    parser.add_argument('--ffmpeg', default='ffmpeg')
    parser.add_argument('--execute-cold-conversion', action='store_true')
    args = parser.parse_args()
    need(args.execute_cold_conversion, 'Explicit root-authorized conversion flag required')
    need(not args.output_root.exists() and not args.output_root.is_symlink(), 'Fresh output root required')
    archive = read_exact(args.ra_archive, ARCHIVE_BYTES, ARCHIVE_SHA)
    cub = sky_from_ra(archive)
    records = sky_tiles(cub)
    helper_path = args.repo / 'tools/assets/decode_source_textures50fc.py'
    need(helper_path.is_file() and helper_path.stat().st_size <= 65536, 'Bounded helper')
    need(sha(helper_path.read_bytes()) == HELPER_SHA, 'Existing50FC helper changed')
    need(args.elf.is_file() and args.elf.stat().st_size <= 16*1024*1024, 'Bounded static ELF')
    need(sha(args.elf.read_bytes()) == ELF_SHA, 'Static ELF identity')
    spec = importlib.util.spec_from_file_location('ra_sky_original_50fc', helper_path)
    need(spec is not None and spec.loader is not None, 'Pinned helper import')
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    anchors = helper.verify_elf(args.elf)
    ffmpeg = shutil.which(args.ffmpeg)
    need(ffmpeg is not None, 'Installed native FFmpeg required')
    ffmpeg_path = Path(ffmpeg).resolve()
    need(ffmpeg_path.is_file() and ffmpeg_path.stat().st_size <= 128*1024*1024, 'Bounded FFmpeg')
    ffmpeg_sha = sha(ffmpeg_path.read_bytes())
    version = subprocess.run([str(ffmpeg_path), '-version'], capture_output=True, text=True,
                             check=True, timeout=15)
    need(version.stdout.startswith('ffmpeg version ') and not version.stderr, 'FFmpeg provenance')
    generator_sha = sha(Path(__file__).read_bytes())
    images, index = [], []
    for record in records:
        mip = record['mips'][-1]
        rgba, yuv_sha = helper.decode_ipu(cub[mip['offset']:mip['offset']+mip['bytes']],
                                        128, 128, str(ffmpeg_path))
        need(len(rgba) == 128*128*4, 'Decoded RGBA extent')
        rrtex = struct.pack('<8sII', b'RRTEX001', 128, 128) + rgba
        filename = f'face{record["face"]}-tile{record["tile"]:02d}.rrtex'
        record['yuv_sha256'] = yuv_sha
        record['decoded'] = dict(filename=filename, sha256=sha(rrtex), bytes=len(rrtex), width=128, height=128)
        images.append(rrtex)
        index.append('    {%d, %d, %d, %d, "%s", "%s", "%s"},' %
                     (record['face'], record['tile'], record['offset'], record['bytes'],
                      record['sha256'], filename, sha(rrtex)))
    need(sha(ffmpeg_path.read_bytes()) == ffmpeg_sha and sha(helper_path.read_bytes()) == HELPER_SHA and
         sha(Path(__file__).read_bytes()) == generator_sha, 'Conversion inputs changed')
    manifest = dict(schema='RR_ORIGINAL_RA_SKY_V1', archive_sha256=ARCHIVE_SHA,
        archive_bytes=ARCHIVE_BYTES, member=SOURCE_MEMBER, member_sha256=SOURCE_SHA,
        member_bytes=SOURCE_BYTES, tile_count=len(records), tiles=records,
        palette_header=struct.unpack_from('<3I', cub, 16),
        elf_sha256=ELF_SHA, elf_verified_anchors=anchors,
        decoder_sha256=HELPER_SHA, generator_sha256=generator_sha,
        ffmpeg_sha256=ffmpeg_sha, ffmpeg_version_report=version.stdout,
        ffmpeg_flags='-cpuflags 0 -flags +bitexact -idct simple -threads 1 -err_detect explode',
        input_policy='Exact IPUM first frame through B0; final four-byte B1 excluded',
        color_policy='50FC integer BT.601; nearest chroma; RGB16 dither; RGB5 high-bit expansion; TEXA TA0=TA1=255 AEM=1; no row flip',
        half_extent=25600, camera_relative_y_offset=1024, source_uv_range=[0.03125,0.96875],
        hardware_pixel_parity_verified=False)
    # Publish exclusively into a fresh root, after every decode has succeeded.
    args.output_root.mkdir(parents=False, exist_ok=False)
    output = args.output_root / 'evidence/skyRA'
    output.mkdir(parents=True)
    for record, image in zip(records, images):
        write_new(output / record['decoded']['filename'], image)
    header = args.output_root / 'tests/native/sky_ra_index.inc'
    header.parent.mkdir(parents=True)
    write_new(header, ('\n'.join(index)+'\n').encode('ascii'))
    manifest['index_sha256'] = sha(header.read_bytes())
    write_new(output / 'manifest.json', (json.dumps(manifest, indent=2)+'\n').encode('utf-8'))
    print(json.dumps(dict(status='converted_original_ra_sky', tiles=len(records),
                         index_sha256=manifest['index_sha256'], output_root=str(args.output_root))))

if __name__ == '__main__':
    main()
