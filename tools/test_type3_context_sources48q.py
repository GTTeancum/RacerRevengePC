#!/usr/bin/env python3
"""Reject one-at-a-time wrong-template-field mutations of the borrowed context.

Builds temporary copies only, serially. Production runtime, retained48P evidence,
and the independent poisoned-template harness are fingerprinted and unchanged.
Requires a passing release directory from test_native_type3_context48q.py.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys

sys.dont_write_bytecode = True
import test_native_spring48k as runner


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline-dir', required=True, type=Path)
    p.add_argument('--output-dir', required=True, type=Path)
    args = p.parse_args()
    root = Path(__file__).resolve().parents[1]
    baseline = args.baseline_dir.resolve()
    out = args.output_dir.resolve()
    if out == root or root in out.parents:
        p.error('Output must be outside the checkout')
    out.mkdir(parents=True, exist_ok=True)
    if any(out.iterdir()):
        p.error('Output directory must be empty')
    report = json.loads((baseline / 'report.json').read_text())
    if report['status'] != 'passed' or report['sanitizers'] or report['selected_tests'] != ['type3_context48q']:
        p.error('Baseline must be a passing release context-only run')
    for name, fingerprint in report['input_hashes'].items():
        if 'sha256' in fingerprint and sha(root / name) != fingerprint['sha256']:
            p.error('Baseline input changed: ' + name)
    compiler = report['compiler']['command']
    flags = report['compile_flags'] + report['include_flags']
    runtime = root / 'src/game/type3_query48p.cpp'
    test = root / 'tests/native/type3_context48q.cpp'
    fixed = [baseline / (Path(b['source']).stem + '.o') for b in report['build'] if b['source'] != 'src/game/type3_query48p.cpp']
    # Preserve the exact original helper and fixture bytes as well as all runtime inputs.
    tracked = {root / name: sha(root / name) for name, f in report['input_hashes'].items() if 'sha256' in f}
    retained = root / 'tests/native/type3_query48p.cpp'
    tracked[retained] = sha(retained)
    results = {'schema': 1, 'serial': True, 'baseline': str(baseline), 'mutations': [],
               'input_sha256': {str(path.relative_to(root)): h for path, h in tracked.items()}}

    def run(command, name):
        with (out / (name + '.stdout.log')).open('w') as stdout, (out / (name + '.stderr.log')).open('w') as stderr:
            return subprocess.run(command, cwd=root, stdout=stdout, stderr=stderr, check=False, timeout=180).returncode

    baseline_status = run([str(baseline / 'type3_context48q'), str(root)], 'baseline')
    results['baseline_returncode'] = baseline_status
    if baseline_status:
        raise RuntimeError('Unchanged baseline no longer passes')
    test_object = out / 'type3_context48q.o'
    if run(compiler + flags + ['-c', str(test), '-o', str(test_object)], 'build-test'):
        raise RuntimeError('Context test compilation failed')
    source = runtime.read_text()
    for field in ('spring', 'bodyScene', 'pairCount', 'resetSpring', 'bodyScenePresent', 'entry'):
        mutated, count = re.subn(r'\bcontext\.' + field + r'\b', 'input.' + field, source)
        if not count:
            raise RuntimeError('Missing mutation anchor: ' + field)
        path = out / (field + '.cpp')
        path.write_text(mutated)
        obj = out / (field + '.o')
        binary = out / field
        compile_status = run(compiler + flags + ['-c', str(path), '-o', str(obj)], 'build-' + field)
        link_status = run(compiler + [str(test_object), str(obj)] + list(map(str, fixed)) + ['-o', str(binary)], 'link-' + field) if not compile_status else None
        status = run([str(binary), str(root)], 'test-' + field) if link_status == 0 else None
        summary = (out / ('test-' + field + '.stdout.log')).read_text() if status is not None else ''
        match = re.search(r'failures=(\d+)', summary)
        failures = int(match.group(1)) if match else None
        detected = compile_status == 0 and link_status == 0 and status == 1 and failures is not None and failures > 0
        result = {'field': field, 'replacements': count, 'compile_returncode': compile_status,
                  'link_returncode': link_status, 'test_returncode': status, 'failed_checks': failures, 'detected': detected,
                  'mutated_sha256': sha(path)}
        results['mutations'].append(result)
        print(json.dumps(result), flush=True)
    results['changed_inputs'] = [str(path) for path, before in tracked.items() if sha(path) != before]
    results['status'] = 'passed' if not results['changed_inputs'] and all(r['detected'] for r in results['mutations']) else 'failed'
    (out / 'report.json').write_text(json.dumps(results, indent=2) + '\n')
    return 0 if results['status'] == 'passed' else 1


if __name__ == '__main__':
    sys.exit(main())
