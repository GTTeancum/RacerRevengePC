"""Byte-compare both ready-cache modes against already frozen 47B2/45Y logs."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('--candidate', choices=('47G', '47H', '47I', '47J', '47J2'), default='47G')
args = p.parse_args()
out = root / f'evidence/{args.candidate}/regressions'
out.mkdir(exist_ok=False)
records = []
for mode in ('0', '1'):
    for suite in ('emulator', 'import', 'import_version', 'compatibility'):
        env = os.environ.copy()
        env.update(RR_IOP_IDLE_BATCH='1', RR_IOP_READY_CACHE47G=mode)
        if args.candidate in ('47I', '47J', '47J2'):
            env.update(RR_IOP_READY_CACHE47G='1', RR_IOP_IMPORT_CACHE47I=mode)
        for key in ('RR_IOP_IDLE_STATS', 'RR_IOP_READY_STATS47G', 'RR_IOP_IMPORT_STATS47I'):
            env.pop(key, None)
        exe = root / f'work/native/ninja-build-release-opt/racer_iop{args.candidate.lower()}_{suite}.exe'
        result = subprocess.run([str(exe)], cwd=root, env=env,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=180)
        name = f'ps2_iop_{suite}_tests-{mode}'
        (out / (name + '.log')).write_bytes(result.stdout)
        (out / (name + '.exit')).write_text(str(result.returncode))
        frozen = root / f'evidence/47B/iop-regressions-b2/{name}'
        record = dict(suite=suite, mode=mode,
                      feature='import_cache' if args.candidate in ('47I', '47J', '47J2') else 'ready_cache', exit=result.returncode,
                      sha256=hashlib.sha256(result.stdout).hexdigest(),
                      matches_frozen45Y_via47B2=result.stdout == frozen.with_suffix('.log').read_bytes(),
                      exit_matches=result.returncode == int(frozen.with_suffix('.exit').read_text(encoding='utf-8-sig')))
        records.append(record)
        print(json.dumps(record), flush=True)
(out / 'comparison.json').write_text(json.dumps(records, indent=2))
raise SystemExit(0 if all(r['matches_frozen45Y_via47B2'] and r['exit_matches'] for r in records) else 1)
