"""Audit source selection, binary maps/modules, asset provenance and real captures.

This is an independence/inspection check, not a native gameplay acceptance test.
"""
from pathlib import Path
import argparse
import collections
import hashlib
import json
import re

if not __debug__:
    raise RuntimeError('Audit checks require normal Python execution, without -O/PYTHONOPTIMIZE')

ROOT = Path(__file__).resolve().parents[1]


def js(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))


def sha(path):
    with path.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-evidence', type=Path, required=True)
    parser.add_argument('--run', type=Path, action='append', required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    allowed = {'src/assets/native_asset48b.cpp', 'platform/windows/native/main48b.cpp',
               'tests/native/asset_loader48b.cpp', 'src/game/control_filter48b.cpp',
               'tests/native/control_filter48b.cpp'}
    forbidden = re.compile(r'pcsx2|ps2_runtime|ps2_iop|r5900context|dispatchguest|microvu', re.I)
    builds, runs = [], []
    for arch in ('x64', 'x86'):
        assert (args.build_evidence / (arch + '.exit')).read_text().strip() == '0'
        commands = js(args.build_evidence / (arch + '-compile-commands.json'))
        sources = {Path(c['file']).resolve().relative_to(ROOT).as_posix() for c in commands}
        assert sources <= allowed and len(sources) >= 3, sources
        linkmap = args.build_evidence / (arch + '-native.map')
        assert not forbidden.search(linkmap.read_text(errors='replace'))
        pe = args.build_evidence / (arch + '-pe.txt')
        assert not forbidden.search(pe.read_text(errors='replace'))
        exe = ROOT / f'build/native-pc48B-{arch}/racer_native_pc.exe'
        recorded = js(args.build_evidence / (arch + '-hashes.json'))
        recorded_exe = next(item for item in recorded if Path(item['Path']).name == 'racer_native_pc.exe')
        assert sha(exe) == recorded_exe['Hash'].lower(), 'Build executable differs from frozen build record'
        source_records = js(args.build_evidence / (arch + '-source-hashes.json'))
        for item in source_records:
            assert sha(Path(item['Path'])) == item['Hash'].lower(), 'Source changed since recorded build'
        builds.append(dict(architecture=arch, sources=sorted(sources), executable_sha256=sha(exe),
                           executable_bytes=exe.stat().st_size, map_sha256=sha(linkmap), pe_report_sha256=sha(pe)))
    for directory in args.run:
        report, launch = js(directory / 'report.json'), js(directory / 'launch.json')
        assert (directory / 'native.exit').read_text().strip() == '0'
        assert report['success'] and report['capture_written'] and report['last_frame_completed_readback']
        assert report['frames_submitted'] > 0
        assert report['frames_presented'] == 0, 'These runs should be bounded offscreen inspections'
        modules = report['loaded_modules']
        assert modules and all(not forbidden.search(name) for name in modules)
        for item in launch['input_files']:
            assert sha(directory / item['name']) == item['sha256']
        for item in js(directory / 'output-hashes.json'):
            assert sha(directory / Path(item['Path']).name) == item['Hash'].lower(), 'Recorded run output changed'
        expected_build = next(b for b in builds if b['architecture'] == launch['architecture'])
        assert sha(directory / 'racer_native_pc.exe') == expected_build['executable_sha256']
        assert report['pointer_bits'] == (64 if launch['architecture'] == 'x64' else 32)
        # PPM writer contract is fixed; inspect actual readback without modifying the image.
        capture = (directory / 'capture.ppm').read_bytes()
        magic, dimensions, depth, rgb = capture.split(b'\n', 3)
        assert magic == b'P6' and depth == b'255'
        width, height = map(int, dimensions.split())
        assert len(rgb) == width * height * 3
        colors = collections.Counter(rgb[i:i + 3] for i in range(0, len(rgb), 3))
        non_background = width * height - colors.most_common(1)[0][1]
        assert non_background > 100, 'Readback has no substantial non-background region'
        runs.append(dict(path=str(directory.resolve()), pointer_bits=report['pointer_bits'],
            driver_type=report['driver_type'], vertex_count=report['vertex_count'],
            triangle_count=report['triangle_count'], capture_sha256=sha(directory / 'capture.ppm'),
            non_background_pixels=non_background, unique_rgb_colors=len(colors),
            loaded_module_count=len(modules), input_files=launch['input_files']))
    output = dict(scope='native static asset inspection; no game simulation, input behavior or racing FPS accepted',
        source_allowlist_passed=True, link_map_and_import_name_audit_passed=True,
        loaded_module_name_audit_passed=True, input_hashes_verified=True,
        gpu_readbacks_nonempty=True, builds=builds, runs=runs,
        limitations=['Name/map/source/module audit is not an OS-level trace of every file opened.',
                    'Geometry/material fidelity has separate converter/reference acceptance.',
                    'Non-background pixels prove a nonempty readback, not correct art or gameplay.'])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + '\n')
    print(json.dumps({'builds': len(builds), 'runs': len(runs), 'audit': 'passed', 'output': str(args.output)}))


if __name__ == '__main__':
    main()
