"""Snapshot compiled sources, local includes and named runtime fixtures."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"\n]+)[>"]', re.MULTILINE)

def collect(commands):
    rows = json.loads(commands.read_text(encoding='utf-8'))
    directories = [ROOT/'src/assets', ROOT/'src/game', ROOT/'third_party/native-json',
                   ROOT/'third_party/native-zlib']
    pending = [Path(row['file']).resolve() for row in rows]
    seen = set()
    while pending:
        path = pending.pop()
        if path in seen:
            continue
        if not path.is_relative_to(ROOT):
            raise RuntimeError(f'Unexpected external source: {path}')
        seen.add(path)
        for kind, name in INCLUDE.findall(path.read_text(encoding='utf-8-sig')):
            candidates = ([path.parent/name] if kind == '"' else []) + [p/name for p in directories]
            found = next((p.resolve() for p in candidates if p.is_file()), None)
            if found:
                pending.append(found)
            elif kind == '"':
                raise RuntimeError(f'Unresolved local include {name} in {path}')
            # Unresolved angle includes are toolchain/platform headers. Compiler,
            # SDK version and linked library hashes are recorded by the driver.
    seen.update([ROOT/'CMakeLists.txt', ROOT/'cmake/NativeVehicleFrame49W.cmake',
                 ROOT/'tools/Build-NativeFrame49W.ps1', Path(__file__).resolve()])
    # The portable suite catalog names runtime fixtures as well as source.
    # Importing it constructs metadata only; it does not execute a build/test.
    sys.dont_write_bytecode = True
    import test_native_impact_frame50a as catalog
    registry = catalog.runner
    expected = {str((ROOT/name).resolve()).casefold() for name in registry.SOURCES}
    expected.update(str((ROOT/spec['source']).resolve()).casefold() for spec in registry.TESTS.values())
    actual = {str(Path(row['file']).resolve()).casefold() for row in rows}
    if expected != actual:
        raise RuntimeError(f'CMake/catalog source mismatch: missing={expected-actual}, extra={actual-expected}')
    seen.add(commands.resolve())
    for spec in registry.TESTS.values():
        if spec.get('fixture'):
            seen.add(ROOT/spec['fixture'])
        for extra in spec.get('extra_fixtures', []):
            seen.add(ROOT/extra['fixture'])
    seen.update(ROOT/name for name in registry.EXTRA_INPUT_PATHS)
    seen.update(Path(module.__file__).resolve() for name, module in tuple(sys.modules.items())
                if name.startswith(('test_native_', 'native_frame_runner')) and getattr(module, '__file__', None))
    return [{'Path':str(p),'Algorithm':'SHA256','Hash':hashlib.sha256(p.read_bytes()).hexdigest().upper()}
            for p in sorted(seen)]

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('commands',type=Path)
    parser.add_argument('snapshot',type=Path)
    parser.add_argument('--verify',action='store_true')
    args=parser.parse_args()
    current=collect(args.commands)
    if args.verify:
        before=json.loads(args.snapshot.read_text(encoding='utf-8'))
        if current != before:
            raise RuntimeError('Compiled input graph or source hashes changed during validation')
        print(f'Verified {len(current)} source/runtime input hashes and dependency paths')
    else:
        args.snapshot.write_text(json.dumps(current,indent=2),encoding='utf-8')
        print(f'Snapshotted {len(current)} source/runtime inputs')

if __name__=='__main__':
    main()
