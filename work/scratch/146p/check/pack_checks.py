"""Runs tools\\dlc\\dlc_pack.py check over the same roots as run_checks.py (offline tool = port rules)."""
import subprocess, sys
from pathlib import Path
PACK = r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a061eb9837b6bc01f\tools\dlc\dlc_pack.py'
STOCK = r'C:\Games\Star Wars Racer Revenge'
ROOTS = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots')
def run(root, show_all=False):
    r = subprocess.run([sys.executable, '-I', PACK, 'check', str(root), '--stock', STOCK], capture_output=True, text=True)
    print(f'===== {root} (exit {r.returncode})')
    for l in (r.stdout + r.stderr).splitlines():
        if show_all or not l.startswith('  ') or 'refused' in l:
            print(l)
    return r.returncode
codes = [run(ROOTS / r / 'DLC', True) for r in ('addon-arcade-pods-146', 'addon-arcade-146', 'addon-pods-146', 'addon-all-146')]
for c in sorted((ROOTS / 'addon-pods-146-broken').iterdir()):
    codes.append(run(c / 'DLC'))
for old in ('pods', 'etti'):  # phase-1 replacement roots stay as before
    if (ROOTS / old).exists():
        run(ROOTS / old)
print('exit codes', codes)
