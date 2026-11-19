"""Scratch copies of the arcade's dlc.json packages (racer-arcade\\dlc-addons, read only) as test roots."""
import shutil, sys
from pathlib import Path
SRC = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\dlc-addons')
ROOTS = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots')
ALL, PODS = ROOTS / 'addon-arcade-146' / 'DLC', ROOTS / 'addon-arcade-pods-146' / 'DLC'
for r in (ALL, PODS):
    if r.exists():
        sys.exit(f'{r} exists (nothing is overwritten)')
for p in sorted(SRC.iterdir()):
    if (p / 'dlc.json').is_file():
        shutil.copytree(p, ALL / p.name)
        if b'"kind": "pod"' in (p / 'dlc.json').read_bytes():
            shutil.copytree(p, PODS / p.name)
print(sorted(x.name for x in ALL.iterdir()), sorted(x.name for x in PODS.iterdir()))
