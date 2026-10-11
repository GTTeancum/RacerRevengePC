#!/usr/bin/env python3
"""Root-owned cold conversion of the exact Rider19c_1 PST's five IPUM mips.

Source-only reconstruction. Do not execute outside an authorized central
validation slot. Reuses the unchanged50FC native host FFmpeg helper; never
executes guest instructions or emulates a PS2/VU/IPU device. This generates
authentic source-derived image cache bytes and a compile-time output catalog.
It does not assert hardware inverse-DCT or framebuffer bit parity.
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

sys.dont_write_bytecode = True
SOURCE_BYTES = 23096
SOURCE_SHA = 'ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3'
ARCHIVE_BYTES = 5552683
ARCHIVE_SHA = '112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856'
HELPER_SHA = 'e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe'
MIPS = (
    (16, 12, 32, 205, '30260bfadc135b0678023e198c919bbc76769808250bb1d64da95160a5dadc35'),
    (32, 237, 257, 620, '0febb2bfc984a924942b49caefc5c1d024fd5b3fb54b0bf962f1ee40cf1f36a4'),
    (64, 877, 897, 1850, '799b622653d923464c9a478e7aebfab286b7d135c5577d217840bb1f3eed7435'),
    (128, 2747, 2767, 5469, 'e017da2329dc9a9101ed1a33d32e87a3d9bb58e2ccd1055261073f6ee5bc5f7b'),
    (256, 8236, 8256, 14840, 'c8e6de01def72409f2cf85de2e6705b77261feb9194b33d50a5e4097e86474b5'),
)
COMMAND_ARGUMENTS = ('-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact '
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
    need(path.is_file() and path.stat().st_size == count, 'Input byte extent: ' + str(path))
    data = path.read_bytes()
    need(len(data) == count and sha(data) == digest, 'Input identity: ' + str(path))
    return data


def write_new(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as output:
        output.write(data)
    need(path.read_bytes() == data, 'Output readback: ' + str(path))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--source-pst', type=Path, required=True)
    parser.add_argument('--rider-archive', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    parser.add_argument('--ffmpeg', default='ffmpeg')
    parser.add_argument('--execute-cold-conversion', action='store_true')
    args = parser.parse_args()
    need(args.execute_cold_conversion, 'Explicit root-authorized --execute-cold-conversion is required')
    need(not args.output_root.exists(), 'Choose a new output root; existing cache/catalog is never overwritten')
    source = read_exact(args.source_pst, SOURCE_BYTES, SOURCE_SHA)
    archive = read_exact(args.rider_archive, ARCHIVE_BYTES, ARCHIVE_SHA)
    helper_path = args.repo / 'tools/assets/decode_source_textures50fc.py'
    need(helper_path.is_file() and helper_path.stat().st_size <= 64 * 1024, 'Bounded retained helper source')
    helper_source = helper_path.read_bytes()
    need(sha(helper_source) == HELPER_SHA, 'Original50FC decoder source changed; review before updating pins')
    # Import ONLY the authenticated existing host image converter. No project
    # runtime, importer, guest or emulator module is loaded by this generator.
    spec = importlib.util.spec_from_file_location('rider_original_50fc', helper_path)
    need(spec is not None and spec.loader is not None, 'Retained converter module')
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    elf_anchors = helper.verify_elf(args.elf)
    frames = helper.parse_resource('19c_1.pst', source)
    need(len(frames) == 1 and frames[0]['kind'] == 2 and frames[0]['palette'] is None and
         len(frames[0]['mips']) == 5 and frames[0]['start'] == 0 and frames[0]['end'] == SOURCE_BYTES,
         'Exact complete kind2 standalone PST')
    for actual, expected in zip(frames[0]['mips'], MIPS):
        dim, header, payload, count, digest = expected
        need((actual['width'], actual['height'], actual['header_offset'], actual['data_offset'],
              len(actual['data']), sha(actual['data'])) == (dim, dim, header, payload, count, digest),
             'Exact original IPUM mip identity/layout')
    ffmpeg = shutil.which(args.ffmpeg)
    need(ffmpeg is not None, 'An installed FFmpeg with native IPU decoding is required')
    ffmpeg_path = Path(ffmpeg).resolve()
    need(ffmpeg_path.is_file() and ffmpeg_path.stat().st_size <= 128 * 1024 * 1024, 'Bounded FFmpeg executable')
    ffmpeg_sha = sha(ffmpeg_path.read_bytes())
    version_result = subprocess.run([str(ffmpeg_path), '-version'], capture_output=True, text=True,
                                    check=True, timeout=15)
    need(version_result.stdout.startswith('ffmpeg version ') and not version_result.stderr,
         'FFmpeg version provenance')
    version = version_result.stdout.splitlines()[0]
    generator_sha = sha(Path(__file__).read_bytes())
    records = []
    images = []
    # Fully decode and verify all five images before writing any generated
    # success catalog. The unchanged helper retains its30-second decode bound.
    for level, actual in enumerate(frames[0]['mips']):
        dim, header, payload, count, digest = MIPS[level]
        rgba, yuv_sha = helper.decode_ipu(actual['data'], dim, dim, str(ffmpeg_path))
        need(len(rgba) == dim * dim * 4, 'Exact decoded RGBA extent')
        need(all((rgba[i] | rgba[i + 1] | rgba[i + 2]) & 7 == 0 and
                 rgba[i + 3] == (255 if rgba[i] or rgba[i + 1] or rgba[i + 2] else 0)
                 for i in range(0, len(rgba), 4)), 'Source RGB16/TEXA policy')
        image = struct.pack('<8sII', b'RRTEX001', dim, dim) + rgba
        filename = f'19c_1-mip{level}.rrtex'
        record = dict(level=level, width=dim, height=dim, source_header_offset=header,
                      source_payload_offset=payload, source_payload_bytes=count,
                      source_payload_sha256=digest, yuv420p_sha256=yuv_sha,
                      rgba_sha256=sha(rgba), rrtex_sha256=sha(image),
                      rrtex_bytes=len(image), filename=filename)
        records.append(record)
        images.append(image)
    need(sha(ffmpeg_path.read_bytes()) == ffmpeg_sha, 'FFmpeg executable changed during conversion')
    need(sha(helper_path.read_bytes()) == HELPER_SHA, 'Retained helper changed during conversion')
    need(sha(Path(__file__).read_bytes()) == generator_sha, 'Generator changed during conversion')
    manifest = dict(schema='RR_ORIGINAL_RIDER_IPUM_CACHE_50KG_1', source_member='ui\\textures\\19c_1.pst',
                    source_pst_bytes=SOURCE_BYTES, source_pst_sha256=sha(source),
                    source_archive_bytes=len(archive), source_archive_sha256=sha(archive),
                    elf_sha256=helper.ELF_SHA, elf_verified_anchors=elf_anchors,
                    converter_sha256=HELPER_SHA, generator_sha256=generator_sha,
                    ffmpeg_version=version, ffmpeg_version_report=version_result.stdout,
                    ffmpeg_sha256=ffmpeg_sha,
                    ffmpeg_argv=[str(ffmpeg_path)] + COMMAND_ARGUMENTS.split(),
                    codec='FFmpeg AV_CODEC_ID_IPU native host MPEG intra decoder',
                    command_arguments=COMMAND_ARGUMENTS, color_policy=COLOR_POLICY,
                    input_policy='Exact original IPUM bytes excluding only final4-byte B1 sequence terminator; B0 frame terminator retained',
                    hardware_pixel_parity_verified=False,
                    limitations=['No PS2 hardware IDCT or original framebuffer parity measurement.',
                                 'Retaining five mips does not implement renderer mip selection/filtering.',
                                 'No material admission, GL upload, native client or gameplay validation is implied.'],
                    mips=records)
    # C++ string literals are ASCII JSON literals for these bounded metadata
    # fields. No source file names or runtime caller values enter the catalog.
    quote = lambda text: json.dumps(text, ensure_ascii=True)
    lines = ['// Generated from authenticated actual Rider19c_1 source conversion.',
             '// Native host image codec only; hardware pixel parity remains unverified.',
             'constexpr bool retainedCatalogAvailable = true;',
             'constexpr RetainedMipRecord retainedMipRecords[originalUiIpumMipCount50KG] = {']
    for record in records:
        lines.append('    {' + ','.join(quote(record[key]) for key in
                                       ('rrtex_sha256', 'rgba_sha256', 'yuv420p_sha256')) + '},')
    lines.extend(['};', 'constexpr RetainedCodecRecord retainedCodecRecord = {',
                  '    ' + ',\n    '.join(quote(value) for value in
                                         (HELPER_SHA, generator_sha, version, ffmpeg_sha, COMMAND_ARGUMENTS)),
                  '};', ''])
    generated = '\n'.join(lines).encode('ascii')
    manifest['generated_catalog_sha256'] = sha(generated)
    for record, image in zip(records, images):
        write_new(args.output_root / 'evidence/ui-rider-ipum50kg' / record['filename'], image)
    write_new(args.output_root / 'evidence/ui-rider-ipum50kg/manifest.json',
              (json.dumps(manifest, indent=2) + '\n').encode('utf-8'))
    # Publish the enabling catalog last, after all image/provenance readbacks.
    write_new(args.output_root / 'src/assets/original_ui_ipum_texture_cache50kg_generated.inc', generated)
    print(json.dumps(dict(status='converted_original_rider_ipum', source_sha256=SOURCE_SHA,
                          mips=len(records), rgba_bytes=sum(len(image) - 16 for image in images),
                          hardware_pixel_parity_verified=False,
                          generated_catalog_sha256=sha(generated))))


if __name__ == '__main__':
    main()
