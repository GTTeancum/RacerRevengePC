"""Check that native viewer output aliases cannot overwrite imported assets."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--architecture', choices=['x64', 'x86'], required=True)
    args = ap.parse_args()
    exe = ROOT / f'build/native-pc48B-{args.architecture}/racer_native_pc.exe'
    out = Path(tempfile.mkdtemp(prefix=f'native-cli48B-{args.architecture}-', dir=ROOT / 'evidence'))
    mesh = out / 'mesh.rrmesh'
    texture = out / 'texture.rrtex'
    shutil.copyfile(ROOT / 'evidence/assets48B/ground/mesh.rrmesh', mesh)
    shutil.copyfile(ROOT / 'evidence/assets48B/ground/texture-linear-palette.rrtex', texture)
    (out / 'child').mkdir()
    os.link(mesh, out / 'linked.rrmesh')
    original = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in (mesh, texture)}
    cases = [
        ['--mesh', 'mesh.rrmesh', '--capture', 'mesh.rrmesh'],
        ['--mesh', 'mesh.rrmesh', '--report', 'mesh.rrmesh'],
        ['--mesh', 'mesh.rrmesh', '--capture', 'MESH.RRMESH'],
        ['--mesh', 'mesh.rrmesh', '--capture', 'child/../mesh.rrmesh'],
        ['--mesh', 'mesh.rrmesh', '--capture', 'linked.rrmesh'],
        ['--mesh', 'mesh.rrmesh', '--texture', 'texture.rrtex', '--report', 'texture.rrtex'],
        ['--mesh', 'mesh.rrmesh', '--capture', 'shared.out', '--report', 'shared.out'],
        ['--mesh', 'mesh.rrmesh', '--capture', 'shared.out', '--report', 'SHARED.OUT'],
    ]
    results = []
    for command in cases:
        run = subprocess.run([str(exe)] + command, cwd=out, capture_output=True, text=True, timeout=15)
        if run.returncode == 0 or 'alias' not in run.stderr.lower():
            raise RuntimeError(f'Expected pre-render alias rejection: {command}: {run.stderr}')
        for name, digest in original.items():
            if hashlib.sha256((out / name).read_bytes()).hexdigest() != digest:
                raise RuntimeError('Input modified after CLI rejection: ' + name)
        if (out / 'shared.out').exists():
            raise RuntimeError('Output created before alias rejection')
        results.append(dict(arguments=command, exit=run.returncode, stderr=run.stderr))
    report = dict(architecture=args.architecture, executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),
        checks=len(results), passed=True, input_hashes=original, results=results)
    (out / 'checks.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'output': str(out), 'passed': True, 'checks': len(results)}))


if __name__ == '__main__':
    main()
