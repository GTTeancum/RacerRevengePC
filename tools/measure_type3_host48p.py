#!/usr/bin/env python3
"""One isolated Linux headless test-process working-set observation.

This includes JSON fixture loading, assertions, diagnostics and the benchmark.
It is not hot-runtime-only memory, a frame budget, or a target/Xbox estimate.
Requires an already validated executable; installs/downloads nothing.
"""
import argparse
import hashlib
import json
from pathlib import Path
import platform
import resource
import subprocess
import sys


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary',type=Path,required=True)
    parser.add_argument('--repo-root',type=Path,default=Path(__file__).resolve().parents[1])
    args=parser.parse_args()
    if sys.platform!='linux':
        parser.error('Linux ru_maxrss units are required; no target assumption')
    binary=args.binary.resolve();root=args.repo_root.resolve()
    if not binary.is_file() or not root.is_dir():
        parser.error('Existing executable and repository are required')
    # This process has no previous children, so RUSAGE_CHILDREN is this one run.
    result=subprocess.run([str(binary),str(root)],cwd=root,text=True,capture_output=True,check=False)
    usage=resource.getrusage(resource.RUSAGE_CHILDREN)
    report={'scope':'complete Linux native test process including JSON and diagnostics',
        'not_runtime_only':True,'not_xbox_or_frame_budget':True,
        'host':platform.platform(),'binary':str(binary),
        'executable_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),
        'returncode':result.returncode,'peak_resident_set_kib':usage.ru_maxrss,
        'user_seconds':usage.ru_utime,'system_seconds':usage.ru_stime,
        'stdout':result.stdout,'stderr':result.stderr}
    print(json.dumps(report,indent=2))
    return 0 if result.returncode==0 else 1


if __name__=='__main__':sys.exit(main())
