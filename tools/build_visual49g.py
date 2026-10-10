#!/usr/bin/env python3
"""Build the two independent 49G diagnostic tools with installed GCC/Clang."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    out = args.output_dir.resolve()
    if out.exists():
        parser.error('Use a new output directory')
    compiler = shutil.which(args.compiler)
    if compiler is None:
        parser.error('An installed compiler is required; this tool installs nothing')
    out.mkdir(parents=True)
    flags = ['-std=c++17', '-Wall', '-Wextra', '-Wpedantic',
             '-fno-fast-math', '-frounding-math', '-ffp-contract=off']
    flags += (['-O1', '-g', '-fsanitize=address,undefined',
               '-fno-omit-frame-pointer', '-fno-sanitize-recover=all']
              if args.sanitize else ['-O2'])
    sources = {
        'export_authored49g': ['tools/visual/export_authored49g.cpp',
            'src/assets/scene_asset48d.cpp', 'src/assets/native_asset48b.cpp',
            'src/assets/world_asset48f.cpp', 'src/assets/import_world48f.cpp'],
        'make_camera49g': ['tools/visual/make_camera49g.cpp',
            'src/assets/tv_camera49e.cpp', 'src/game/tv_camera_selection49e.cpp',
            'src/game/tv_camera_update49f.cpp', 'src/game/camera_view49f.cpp',
            'src/game/camera_frame49f.cpp'],
    }
    report = {'status': 'passed', 'sanitized': args.sanitize, 'builds': []}
    for name, inputs in sources.items():
        commands = []
        objects = []
        for i, source in enumerate(inputs):
            obj = out / (name + '-' + str(i) + '.o')
            objects.append(str(obj))
            # New translation units must be warning-clean. Preserve inherited
            # parser sources and their existing compiler warnings unchanged.
            strict = ['-Werror'] if source.startswith('tools/visual/') else []
            commands.append([compiler, *flags, *strict, '-c', source, '-o', str(obj)])
        commands.append([compiler, *flags, *objects, '-o', str(out / name)])
        rows = []
        stdout, stderr = '', ''
        for command in commands:
            run = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
            stdout += run.stdout
            stderr += run.stderr
            rows.append({'command': command, 'exit_code': run.returncode})
            if run.returncode:
                break
        (out / (name + '.stdout')).write_text(stdout)
        (out / (name + '.stderr')).write_text(stderr)
        row = {'commands': rows, 'exit_code': run.returncode,
               'input_sha256': {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest()
                                for p in inputs}}
        report['builds'].append(row)
        if run.returncode:
            report['status'] = 'failed'
            break
        row['output_sha256'] = hashlib.sha256((out / name).read_bytes()).hexdigest()
    (out / 'build-report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report))
    return 0 if report['status'] == 'passed' else 1


if __name__ == '__main__':
    raise SystemExit(main())
