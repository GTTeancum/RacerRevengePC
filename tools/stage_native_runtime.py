#!/usr/bin/env python3
"""Copy a prepared native runtime and original CAM inputs; never build or run.

The input manifest is the central prepared runtime's runtime-assets.json.
Optional overlays use {"files":[{"path":...,"bytes":...,"sha256":...}]}.
Paths in overlays are relative to the runtime, not to the original disc.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil


def require(ok, message):
    if not ok:
        raise ValueError(message)


def sha256(path):
    h = hashlib.sha256()
    with path.open('rb') as source:
        for block in iter(lambda: source.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def relative(value):
    require(isinstance(value, str), 'Non-string input path')
    path = PurePosixPath(value)
    require(value and str(path) == value and not path.is_absolute()
            and all(part not in ('.', '..') and ':' not in part
                    for part in path.parts) and '\\' not in value,
            'Unsafe relative input path: ' + value)
    return path


def regular(root, name):
    result = root
    for part in relative(name).parts:
        result = result / part
        require(not result.is_symlink(), 'Symlink input: ' + str(result))
    require(result.is_file(), 'Missing input: ' + str(result))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime-root', required=True, type=Path)
    parser.add_argument('--runtime-manifest', required=True, type=Path)
    parser.add_argument('--camera-root', required=True, type=Path,
                        help='Directory containing original PODS/ and manifest.json')
    parser.add_argument('--overlay', nargs=2, action='append', default=[],
                        metavar=('ROOT', 'MANIFEST'))
    parser.add_argument('--native-executable', required=True, type=Path)
    parser.add_argument('--platform', choices=('linux', 'windows'), required=True)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    require(not args.output.exists() and not args.output.is_symlink(),
            'Output must be a new directory; existing packages are never overwritten')
    entries = {}
    case_names = {}

    def add(root, row, prefix='', size_key='bytes'):
        name = str(relative(prefix + row['path']))
        require(name.casefold() not in case_names or case_names[name.casefold()] == name,
                'Case-colliding runtime paths: ' + name)
        case_names[name.casefold()] = name
        source = regular(root, row['path'])
        size, digest = row[size_key], row['sha256']
        require(type(size) is int and size > 0 and
                isinstance(digest, str) and re.fullmatch('[0-9a-f]{64}', digest),
                'Invalid size/hash: ' + name)
        require(source.stat().st_size == size and sha256(source) == digest,
                'Source identity mismatch: ' + name)
        value = {'path': name, 'bytes': size, 'sha256': digest}
        require(name not in entries or entries[name][1] == value,
                'Conflicting overlay: ' + name)
        entries[name] = (source, value)

    manifest = json.loads(args.runtime_manifest.read_text(encoding='utf-8'))
    for row in manifest['files']:
        add(args.runtime_root, row)
    for required in ('evidence/staticType2Producer49X/original-word/native-fixtures.json',
                     'work/native/project/disc/TRACKS/GA.RES',
                     'work/native/project/disc/APPSTART.RES'):
        require(required in entries, 'Missing native startup dependency: ' + required)
    cameras = json.loads((args.camera_root / 'manifest.json').read_text(encoding='utf-8'))
    require(cameras['file_count'] == len(cameras['files']) == 23,
            'Expected all 23 transferred original camera files')
    require(len({row['path'] for row in cameras['files']}) == 23,
            'Duplicate original camera path')
    for row in cameras['files']:
        require(re.fullmatch(r'PODS/POD\d{2}/POD\d{2}\.CAM', row['path']),
                'Unexpected camera resource path')
        require(PurePosixPath(row['path']).parent.name ==
                PurePosixPath(row['path']).stem, 'Camera directory/name mismatch')
        add(args.camera_root, row, 'work/native/project/disc/', 'size_bytes')
    for root, filename in args.overlay:
        for row in json.loads(Path(filename).read_text(encoding='utf-8'))['files']:
            add(Path(root), row)
    executable = args.native_executable
    require(executable.is_file() and not executable.is_symlink(), 'Missing native executable')
    name = 'rr-native.exe' if args.platform == 'windows' else 'rr-native'
    require(name not in entries, 'Native executable collides with asset manifest')
    entries[name] = (executable, {'path': name, 'bytes': executable.stat().st_size,
                                  'sha256': sha256(executable)})
    # Everything is identified before writing; a failed copy leaves an explicit
    # incomplete package without a completion manifest. No retry or cleanup can
    # accidentally replace an existing tester package.
    args.output.mkdir(parents=True, exist_ok=False)
    for source, row in entries.values():
        target = args.output / row['path']
        target.parent.mkdir(parents=True, exist_ok=True)
        with source.open('rb') as src, target.open('xb') as dst:
            shutil.copyfileobj(src, dst, 1024 * 1024)
        require(target.stat().st_size == row['bytes'] and sha256(target) == row['sha256'],
                'Copy identity mismatch: ' + row['path'])
    if args.platform == 'linux':
        os.chmod(args.output / name, 0o755)
    output = {'schema': 'RR_NATIVE_RUNTIME_PACKAGE_V1', 'platform': args.platform,
              'native_executable': name, 'files': [entries[k][1] for k in sorted(entries)],
              'scope': 'Byte-preserving packaging only; no build, launch or acceptance test'}
    with (args.output / 'NATIVE-RUNTIME-MANIFEST.json').open('x', encoding='utf-8') as f:
        json.dump(output, f, indent=2, sort_keys=True)
        f.write('\n')
    print(args.output / 'NATIVE-RUNTIME-MANIFEST.json')


if __name__ == '__main__':
    main()
