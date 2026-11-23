"""build_track_check.py: host-only track_check.exe (PLAN 4.3, writer R11), built after the merge.

Run inside the MSVC environment, e.g. from PowerShell:
  cmd /c "D:\\Programming\\GitHub\\RacerRevengePC\\work\\scratch\\common-rewrite\\tools\\vs.cmd
          D:\\Programming\\GitHub\\RacerRevengePC\\work\\scratch\\common-rewrite\\check\\build_track_check.py
          --build D:\\Programming\\GitHub\\RacerRevengePC\\build\\<merged build> [--root <merged tree>] [--run [track_check args]]"

How (same idea as work\\scratch\\146p-race\\check\\build_career.py, but the common readers
live in the game's own translation units, so it reuses the one build's objects instead of
compiling a short source list):
  1. track_check.cpp is compiled with the response file of a template TU of the merged build
     (default tests\\native\\original_race_hud_scope50la.cpp, the flags W0 checked the common
     headers with), plus /I<merged tree root> so its includes resolve in the merged tree.
  2. It is linked with every object of the build's link.rsp except the TU that defines the
     game's main(), as a console program. Nothing of the game is recompiled or changed.
Outputs: work\\scratch\\common-rewrite\\check\\obj\\track_check.obj and check\\track_check.exe.
"""
import argparse
import json
import pathlib
import re
import subprocess
import sys

MAIN = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC')
HERE = MAIN / 'work' / 'scratch' / 'common-rewrite' / 'check'
OBJ = HERE / 'obj'
TOPS = ('src', 'platform', 'tests', 'third_party')
MAIN_DEF = re.compile(r'^\s*int\s+main\s*\(', re.M)
TOKEN = re.compile(r'"[^"]*"|\S+')


def remapper(root):
    rx = re.compile(re.escape(str(MAIN)) + r'(?:\\\.claude\\worktrees\\[^\\"]+)?\\(' + '|'.join(TOPS) + r')(?=\\)', re.I)
    return lambda text: rx.sub(lambda m: str(root / m.group(1)), text)


def build_folders():
    folders = [d for d in (MAIN / 'build').iterdir() if d.is_dir() and d.name.startswith(('takeover-', 'dlc-', 'wt-', 'cr-'))]
    return sorted(folders, key=lambda d: d.stat().st_mtime, reverse=True)


def template_rsp(build, root, template):
    """Response text of the template TU, from the merged build folder first, then any build folder."""
    remap = remapper(root)
    want = str((root / template).resolve()).lower()
    stem = pathlib.Path(template).stem
    for folder in [build] + [d for d in build_folders() if d != build]:
        for rsp in folder.glob('*' + stem + '.rsp'):
            text = remap(rsp.read_text(encoding='utf-16'))
            if want in text.lower():
                return rsp, text
    raise SystemExit(f'no compile response file for {template} under {MAIN / "build"}')


def main_objects(root, objects):
    """Objects whose source defines main(): matched by object stem against the tree's sources."""
    sources = json.loads((root / 'platform' / 'windows' / 'native' / 'interactive50du.sources.json').read_text(encoding='utf-8-sig'))['sources']
    stems = set()
    for rel in sources:
        path = root / rel
        if path.suffix.lower() == '.cpp' and path.is_file() and MAIN_DEF.search(path.read_text(encoding='utf-8', errors='replace')):
            stems.add(path.stem.lower())
    drop = [o for o in objects if re.sub(r'^\d{4}-', '', pathlib.Path(o.strip('"')).stem).lower() in stems]
    return drop, sorted(stems)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--build', type=pathlib.Path, required=True, help='merged build folder (build-receipt.json, link.rsp)')
    parser.add_argument('--root', type=pathlib.Path, help='merged source tree (default: the build receipt sourceRoot)')
    parser.add_argument('--template', default=r'tests\native\original_race_hud_scope50la.cpp')
    parser.add_argument('--compile-only', action='store_true', help='compile track_check.cpp only (no link); for checking against any tree')
    parser.add_argument('--run', nargs=argparse.REMAINDER, help='run track_check.exe after linking, with these arguments')
    args = parser.parse_args()
    build = args.build.resolve()
    receipt = json.loads((build / 'build-receipt.json').read_text(encoding='utf-8-sig'))
    root = (args.root or pathlib.Path(receipt['sourceRoot'])).resolve()
    for need in ('tests/native/common_track50cr.h', 'tests/native/stock_course_receipts50cr.h', 'tests/native/common_roster50cr.h'):
        if not (root / need).is_file():
            raise SystemExit(f'{root} is not the merged tree (missing {need})')
    OBJ.mkdir(parents=True, exist_ok=True)

    # 1. Compile.
    rsp, text = template_rsp(build, root, args.template)
    src = str((root / args.template).resolve())
    i = text.lower().index(src.lower())
    text = text[:i] + str(HERE / 'track_check.cpp') + text[i + len(src):]
    obj = OBJ / 'track_check.obj'
    text = re.sub(r'/Fo"[^"]+"', lambda m: f'/Fo"{obj}"', text)
    text = re.sub(r'/Fd"[^"]+"', lambda m: f'/Fd"{OBJ / "track_check.pdb"}"', text)
    text = text.replace('/c\n', f'/c\n/I"{root}"\n', 1) if '/c\n' in text else text + f'\n/I"{root}"\n'
    compile_rsp = OBJ / 'track_check.rsp'
    compile_rsp.write_text(text, encoding='utf-16')
    print('template', rsp)
    r = subprocess.run([receipt['compiler'], '@' + str(compile_rsp)], cwd=root, capture_output=True, text=True, errors='replace')
    lines = [l for l in (r.stdout + r.stderr).splitlines() if ' error ' in l or 'fatal error' in l or 'warning C' in l]
    print('compile track_check.cpp', 'OK' if r.returncode == 0 else 'FAIL')
    for l in lines[:40]:
        print('  ', l[:400])
    if r.returncode or args.compile_only:
        return r.returncode

    # 2. Link against the one build's objects (without the game's main()).
    tokens = TOKEN.findall((build / 'link.rsp').read_text(encoding='utf-16'))
    objects = [t for t in tokens if t.strip('"').lower().endswith('.obj')]
    drop, stems = main_objects(root, objects)
    if len(drop) != 1:
        raise SystemExit(f'expected exactly one object defining main(), found {drop} (sources {stems})')
    exe = HERE / 'track_check.exe'
    keep = []
    for t in tokens:
        low = t.strip('"').lower()
        if low.startswith(('/out:', '/map', '/subsystem:', '/entry:')):
            continue
        keep.append(t)
    # The game's main() TU also defines Application members other TUs need, so it stays
    # linked; track_check enters through wmain (wmainCRTStartup), the game's main is unused.
    keep += ['/SUBSYSTEM:CONSOLE', '/ENTRY:wmainCRTStartup', f'/OUT:"{exe}"', f'"{obj}"']
    link_rsp = OBJ / 'track_check.link.rsp'
    link_rsp.write_text('\n'.join(keep) + '\n', encoding='utf-16')
    print('link with', drop[0], '(entry wmain)')
    r = subprocess.run([receipt['linker'], '@' + str(link_rsp)], cwd=root, capture_output=True, text=True, errors='replace')
    errors = [l for l in (r.stdout + r.stderr).splitlines() if 'error' in l.lower()]
    print('link', 'OK' if r.returncode == 0 else 'FAIL', exe)
    for l in errors[:40]:
        print('  ', l[:400])
    if r.returncode:
        return r.returncode
    if args.run is not None:
        return subprocess.run([str(exe), *args.run]).returncode
    return 0


if __name__ == '__main__':
    sys.exit(main())
