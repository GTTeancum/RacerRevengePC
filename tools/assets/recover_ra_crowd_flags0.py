#!/usr/bin/env python3
"""Isolated root-authorized cold conversion of original RA crowd flags0 images.

Do not run outside an explicitly authorized central conversion slot. Reads SLUS
only as data through the pinned existing50FC helper. No guest/device emulation.
The output is six image/provenance records, never canonical resource readiness.
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
SOURCE_MEMBER = 'textures\\crowd.atm'
SOURCE_BYTES = 214578
SOURCE_SHA = '76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031'
HELPER_SHA = '5f9af6eba0f07732f913cde7aa40e246d218d38230f57a7aacc027afd25a7c49'
PINS_SHA = '85dd24cc082899044e4a9726008195c96fa443b1b01e46030a8fa0cc43d90bda'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
COMMAND = ('-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact '
           '-idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 '
           '-frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1')
COLOR_POLICY = ('50FC integer BT.601; nearest 2x2 chroma; RGB16 dither; RGB5 high-bit expansion; '
                'TEXA TA0=TA1=255 AEM=1; no row flip; hardware pixel parity unverified')


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


def crowd_from_ra(blob):
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
    need(length == SOURCE_BYTES and sha(source) == SOURCE_SHA, 'RA crowd source identity')
    return source


def write_new(path, data):
    with path.open('xb') as output:
        output.write(data)
    need(path.read_bytes() == data, 'Output readback: ' + str(path))


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
    source = crowd_from_ra(archive)
    pins_path = Path(__file__).with_name('ra-crowd-source-pins.json')
    need(pins_path.is_file() and pins_path.stat().st_size <= 32768, 'Bounded source pins')
    pins_bytes = pins_path.read_bytes()
    need(sha(pins_bytes) == PINS_SHA, 'Source pins identity')
    pins = json.loads(pins_bytes)
    need((pins['archiveSha256'], pins['member'], pins['sourceBytes'], pins['sourceSha256']) ==
         (ARCHIVE_SHA, SOURCE_MEMBER, SOURCE_BYTES, SOURCE_SHA), 'Source pin binding')
    helper_path = args.repo / 'tools/assets/decode_source_textures50fc.py'
    need(helper_path.is_file() and helper_path.stat().st_size <= 65536, 'Bounded helper source')
    need(sha(helper_path.read_bytes()) == HELPER_SHA, 'Existing50FC helper changed')
    need(args.elf.is_file() and args.elf.stat().st_size <= 16 * 1024 * 1024, 'Bounded static ELF')
    need(sha(args.elf.read_bytes()) == ELF_SHA, 'Static ELF identity')
    spec = importlib.util.spec_from_file_location('ra_original_50fc', helper_path)
    need(spec is not None and spec.loader is not None, 'Pinned helper import')
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    anchors = helper.verify_elf(args.elf)
    frames = helper.parse_resource('crowd.atm', source)
    need(len(frames) == len(pins['frames']) == 6, 'Six source frames')
    for ordinal, (frame, expected) in enumerate(zip(frames, pins['frames'])):
        need(expected['frame'] == ordinal and frame['kind'] == 2 and frame['palette'] is None and
             len(frame['mips']) == len(expected['mips']) == 5 and
             (frame['start'], frame['end'] - frame['start']) == (expected['pstOffset'], expected['pstBytes']) and
             sha(source[frame['start']:frame['end']]) == expected['pstSha256'], 'Exact frame source extent')
        for mip, (actual, pin) in enumerate(zip(frame['mips'], expected['mips'])):
            need((pin['ordinal'], pin['loadedFlags0']) == (mip, mip == 4) and
                 (actual['level'], actual['width'], actual['height'], actual['header_offset'],
                  actual['data_offset'], len(actual['data']), sha(actual['data'])) ==
                 (mip, pin['width'], pin['height'], pin['headerOffset'], pin['payloadOffset'],
                  pin['payloadBytes'], pin['payloadSha256']), 'Exact original mip source')
    ffmpeg = shutil.which(args.ffmpeg)
    need(ffmpeg is not None, 'Installed native FFmpeg required')
    ffmpeg_path = Path(ffmpeg).resolve()
    need(ffmpeg_path.is_file() and ffmpeg_path.stat().st_size <= 128 * 1024 * 1024, 'Bounded FFmpeg')
    ffmpeg_sha = sha(ffmpeg_path.read_bytes())
    version = subprocess.run([str(ffmpeg_path), '-version'], capture_output=True, text=True,
                             check=True, timeout=15)
    need(version.stdout.startswith('ffmpeg version ') and not version.stderr, 'FFmpeg provenance')
    generator_sha = sha(Path(__file__).read_bytes())
    images, records = [], []
    for ordinal, frame in enumerate(frames):
        mip = frame['mips'][4]
        need((mip['width'], mip['height']) == (256, 256), 'Flags0 loaded dimensions')
        # Unchanged helper owns a30-second subprocess timeout per decode.
        # Decode each source identity; do not merge distinct children.
        rgba, yuv_sha = helper.decode_ipu(mip['data'], 256, 256, str(ffmpeg_path))
        need(len(rgba) == 256 * 256 * 4, 'Decoded RGBA extent')
        need(all((rgba[i] | rgba[i + 1] | rgba[i + 2]) & 7 == 0 and
                 rgba[i + 3] == (255 if rgba[i] or rgba[i + 1] or rgba[i + 2] else 0)
                 for i in range(0, len(rgba), 4)), '50FC RGB5/TEXA policy')
        image = struct.pack('<8sII', b'RRTEX001', 256, 256) + rgba
        filename = f'crowd-frame{ordinal}-mip4.rrtex'
        images.append(image)
        records.append(dict(frame=ordinal, source_pst_offset=frame['start'],
            source_pst_bytes=frame['end'] - frame['start'], source_pst_sha256=pins['frames'][ordinal]['pstSha256'],
            source_mip_ordinal=4, native_level_slot=0, native_level_count=1, width=256, height=256,
            source_header_offset=mip['header_offset'], source_payload_offset=mip['data_offset'],
            source_payload_bytes=len(mip['data']), source_payload_sha256=sha(mip['data']),
            yuv420p_sha256=yuv_sha, rgba_sha256=sha(rgba), rrtex_sha256=sha(image),
            rrtex_bytes=len(image), filename=filename))
    need(sha(ffmpeg_path.read_bytes()) == ffmpeg_sha and sha(helper_path.read_bytes()) == HELPER_SHA and
         sha(Path(__file__).read_bytes()) == generator_sha and sha(pins_path.read_bytes()) == PINS_SHA,
         'Conversion inputs changed')
    manifest = dict(schema='RR_RA_CROWD_FLAGS0_IMAGES_50LA_1', source_archive_bytes=ARCHIVE_BYTES,
        source_archive_sha256=ARCHIVE_SHA, source_member=SOURCE_MEMBER, source_bytes=SOURCE_BYTES,
        source_sha256=SOURCE_SHA, source_pins_sha256=PINS_SHA, source_frames=pins['frames'],
        elf_sha256=ELF_SHA, elf_verified_anchors=anchors, converter_sha256=HELPER_SHA,
        generator_sha256=generator_sha, ffmpeg_sha256=ffmpeg_sha,
        ffmpeg_version_report=version.stdout, ffmpeg_argv=[str(ffmpeg_path)] + COMMAND.split(),
        codec='FFmpeg native IPU first-frame YUV420P', color_policy=COLOR_POLICY,
        input_policy='Exact IPUM payload excluding final4-byte B1; B0 retained',
        hardware_pixel_parity_verified=False, frames=records,
        limitations=['No original hardware IDCT/framebuffer parity measurement.',
                     'Only source ordinal4 decoded; smaller four mips authenticated but skipped.',
                     'No canonical resource, material, animation, GL or course admission is certified.'])
    # Claim the output root exclusively after all decode work. No existing
    # directory/cache is modified, including a root created concurrently.
    args.output_root.mkdir(parents=False, exist_ok=False)
    for record, image in zip(records, images):
        write_new(args.output_root / record['filename'], image)
    # Last successful publication is the manifest. Failure leaves a partial
    # fresh directory without a success manifest; never overwrite/retry there.
    pending = args.output_root / 'manifest.pending'
    write_new(pending, (json.dumps(manifest, indent=2) + '\n').encode('utf-8'))
    pending.rename(args.output_root / 'manifest.json')
    print(json.dumps(dict(status='converted_original_ra_crowd_flags0', frames=6,
                          source_sha256=SOURCE_SHA, output_root=str(args.output_root))))


if __name__ == '__main__':
    main()
