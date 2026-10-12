#!/usr/bin/env python3
"""Cold preparation of authenticated fresh-unlocked Rider IPUM sources.

Run only with explicit authorization. Uses the unchanged native host50FC
converter, never guest code. Existing Rider19 cache/catalog is not overwritten.
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

SOURCES = (
    ('04c_1', 39441, 'ca086fb878b398f6fccdcb2161d65ff38bacdb2ee5a3c96ff06e1ed814dbfd27'),
    ('04c_2', 33310, '5f5979fa86f0043be411d84bfb33244921482290eda4c0f4df50e4af1faa32ff'),
    ('10c_1', 23311, '99ccba918bd470d396ebb618b382a81907839a34d0b34b9beab8b5a09e9c0722'),
    ('10c_2', 18494, '9efd4f229c3203e9a27252cac070a1107c83598a7bc536f51a9c46cbb4852ae5'),
    ('12c_1', 22581, 'eecda390eca1ea261f80ba992a37a225613f12d6bc9d0e9545d2380f64662b32'),
    ('12c_4', 22860, 'c3aaa742f80f395f8fc01e0c564acbdea652835038e219c16c3de0a8803f5c7d'),
    ('15c_1', 25470, 'e02462b1c14fc1498a46b91b4245dc8453da45ffe674b70eae080828fdef557d'),
    ('15c_2', 31682, 'cf7093e713ad87e15301435a51beb6cd2cf958dfe74c72dc2657f7cbf1c1ccd5'),
)
ARCHIVE_BYTES = 5552683
ARCHIVE_SHA = '112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856'
HELPER_SHA = 'e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe'
COMMAND = ('-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact '
           '-idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 '
           '-frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1')


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read_exact(path, count, digest):
    need(path.is_file() and path.stat().st_size == count, 'Input extent: ' + str(path))
    data = path.read_bytes()
    need(sha(data) == digest, 'Input identity: ' + str(path))
    return data


def write_new(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as output:
        output.write(data)
    need(path.read_bytes() == data, 'Output readback: ' + str(path))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--source-dir', type=Path, required=True,
                        help='Directory containing exact original flat-named PST files')
    parser.add_argument('--rider-archive', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    parser.add_argument('--source', action='append', choices=[s[0] for s in SOURCES],
                        help='Prepare selected sources; omit to require all eight original inputs')
    parser.add_argument('--ffmpeg', default='ffmpeg')
    parser.add_argument('--execute-cold-conversion', action='store_true')
    args = parser.parse_args()
    need(args.execute_cold_conversion, 'Explicit cold-conversion authorization required')
    need(not args.output_root.exists(), 'Choose a fresh output root')
    read_exact(args.rider_archive, ARCHIVE_BYTES, ARCHIVE_SHA)
    selected = [s for s in SOURCES if args.source is None or s[0] in args.source]
    inputs = [(name, count, digest, read_exact(args.source_dir / (name + '.pst'), count, digest))
              for name, count, digest in selected]
    helper_path = args.repo / 'tools/assets/decode_source_textures50fc.py'
    need(helper_path.is_file() and helper_path.stat().st_size <= 65536, 'Bounded helper')
    need(sha(helper_path.read_bytes()) == HELPER_SHA, 'Retained50FC helper identity')
    spec = importlib.util.spec_from_file_location('unlocked_original_50fc', helper_path)
    need(spec is not None and spec.loader is not None, 'Retained converter module')
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    anchors = helper.verify_elf(args.elf)
    ffmpeg = shutil.which(args.ffmpeg)
    need(ffmpeg is not None, 'Installed native FFmpeg required')
    ffmpeg = Path(ffmpeg).resolve()
    need(ffmpeg.is_file() and ffmpeg.stat().st_size <= 128 * 1024 * 1024, 'Bounded FFmpeg')
    ffmpeg_sha = sha(ffmpeg.read_bytes())
    version = subprocess.run([str(ffmpeg), '-version'], capture_output=True, text=True,
                             check=True, timeout=15)
    need(version.stdout.startswith('ffmpeg version ') and not version.stderr, 'FFmpeg provenance')
    generator_sha = sha(Path(__file__).read_bytes())
    codec = [HELPER_SHA, generator_sha, version.stdout.splitlines()[0], ffmpeg_sha, COMMAND]
    outputs, sources, skipped = [], [], []
    for name, count, digest, source in inputs:
        frames = helper.parse_resource(name + '.pst', source)
        need(len(frames) == 1 and frames[0]['start'] == 0 and frames[0]['end'] == count,
             'Whole standalone PST')
        frame = frames[0]
        if frame['kind'] != 2:
            skipped.append(dict(name=name, kind=frame['kind'], source_sha256=digest))
            continue
        need(frame['palette'] is None and 1 <= len(frame['mips']) <= 5, 'Bounded original IPUM mips')
        records = []
        for level, mip in enumerate(frame['mips']):
            w, h = mip['width'], mip['height']
            need(0 < w <= 1024 and 0 < h <= 1024 and w % 2 == 0 and h % 2 == 0,
                 'Bounded YUV420P source dimensions')
            rgba, yuv_sha = helper.decode_ipu(mip['data'], w, h, str(ffmpeg))
            need(len(rgba) == w * h * 4, 'RGBA extent')
            need(all((rgba[i] | rgba[i+1] | rgba[i+2]) & 7 == 0 and
                     rgba[i+3] == (255 if rgba[i] or rgba[i+1] or rgba[i+2] else 0)
                     for i in range(0, len(rgba), 4)), 'Original RGB16/TEXA policy')
            image = struct.pack('<8sII', b'RRTEX001', w, h) + rgba
            filename = f'{name}-mip{level}.rrtex'
            record = dict(width=w, height=h, header=mip['header_offset'],
                          payload=mip['data_offset'], bytes=len(mip['data']),
                          source_sha256=sha(mip['data']), filename=filename,
                          rrtex_sha256=sha(image), rgba_sha256=sha(rgba), yuv420p_sha256=yuv_sha)
            records.append(record)
            outputs.append((filename, image))
        sources.append(dict(member='ui\\textures\\' + name + '.pst', source_bytes=count,
                            source_sha256=digest, mips=records))
    need(sources, 'No original kind2 sources selected')
    need(sha(ffmpeg.read_bytes()) == ffmpeg_sha and sha(helper_path.read_bytes()) == HELPER_SHA and
         sha(Path(__file__).read_bytes()) == generator_sha, 'Converter changed during preparation')
    q = lambda value: json.dumps(value, ensure_ascii=True)
    lines = ['// Actual authenticated native-host conversion; no hardware parity claim.',
             'constexpr bool additionalCatalogAvailable = true;',
             f'constexpr std::size_t additionalSourceCount = {len(sources)};',
             'constexpr AdditionalSourceRecord additionalSources[additionalSourceCount] = {']
    for source in sources:
        lines.append('    {' + q(source['member']) + ',' + str(source['source_bytes']) + 'u,' +
                     q(source['source_sha256']) + ',' + str(len(source['mips'])) + 'u,{')
        for mip in source['mips']:
            lines.append('        {' + ','.join(str(mip[k]) + 'u' for k in
                         ('width', 'height', 'header', 'payload', 'bytes')) + ',' +
                         q(mip['source_sha256']) + ',' + q(mip['filename']) + '},')
        lines.append('    },{')
        for mip in source['mips']:
            lines.append('        {' + ','.join(q(mip[k]) for k in
                         ('rrtex_sha256', 'rgba_sha256', 'yuv420p_sha256')) + '},')
        lines.append('    },{' + ','.join(q(v) for v in codec) + '}},')
    lines.extend(['};', ''])
    generated = '\n'.join(lines).encode('ascii')
    manifest = dict(schema='RR_UNLOCKED_RIDER_IPUM_50KG_1', source_archive_sha256=ARCHIVE_SHA,
                    elf_sha256=helper.ELF_SHA, elf_verified_anchors=anchors, codec=codec,
                    hardware_pixel_parity_verified=False, sources=sources, indexed_sources=skipped,
                    generated_catalog_sha256=sha(generated))
    for filename, image in outputs:
        write_new(args.output_root / 'evidence/ui-rider-ipum50kg' / filename, image)
    write_new(args.output_root / 'evidence/ui-rider-ipum50kg/unlocked-manifest.json',
              (json.dumps(manifest, indent=2) + '\n').encode('utf-8'))
    write_new(args.output_root / 'src/assets/original_ui_ipum_texture_cache50kg_additional_generated.inc', generated)
    print(json.dumps(dict(status='converted_original_unlocked_rider_ipum', sources=len(sources),
                          mips=len(outputs), hardware_pixel_parity_verified=False)))


if __name__ == '__main__':
    main()
