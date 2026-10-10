#!/usr/bin/env python3
"""Bounded native-only fresh Pod geometry load measurements, never game FPS."""
import argparse
import json
import os
import resource
import statistics
import subprocess
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from measure_authored_geometry49o import ROOT, SOURCES, FLAGS, digest, run, dependency_paths, snapshot_sources, check_stable_sources

EVIDENCE = ROOT / 'evidence/authoredGeometry49O/benchmark'
FIXTURES = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures'
NAMES = ['52-chariot.psg', '43-engrt1.psg', '53-chariot.vut', '56-01_forwd.mot', '57-01_back.mot']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=15)
    parser.add_argument('--iterations', type=int, default=2000)
    args = parser.parse_args()
    if args.samples < 5 or args.iterations < 1:
        parser.error('At least five samples and positive iterations are required.')
    build = args.build_dir.resolve()
    build.mkdir(parents=True, exist_ok=False)
    source = EVIDENCE / 'geometry_benchmark49o.cpp'
    before = snapshot_sources()
    objects, commands, hashes = [], [], {}
    for name in [*SOURCES, 'game/pod_sphere_constraint49l']:
        obj, dep = build / (Path(name).name + '.o'), build / (Path(name).name + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(ROOT / 'src' / (name + '.cpp')), '-o', str(obj)]
        commands.append(command); run(command); objects.append(str(obj))
        for path in dependency_paths(dep):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    exe, probe = build / 'benchmark', build / 'allocation-probe'
    for output, extra in [(exe, []), (probe, ['-DALLOCATION_PROBE49O'])]:
        command = ['g++', *FLAGS, *extra, '-I', str(ROOT / 'src'), str(source), *objects, '-o', str(output)]
        commands.append(command); run(command)
    for path in build.glob('*.su'):
        (EVIDENCE / path.name).write_text(path.read_text())
    cpus = sorted(os.sched_getaffinity(0))
    cpu = 0 if 0 in cpus else cpus[0]
    cap = 64 * 1024 * 1024

    def limits():
        os.sched_setaffinity(0, {cpu})
        resource.setrlimit(resource.RLIMIT_AS, (cap, cap))

    def execute(binary, mode, count):
        command = [str(binary), str(FIXTURES), str(count), mode]
        p = subprocess.Popen(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, preexec_fn=limits)
        _, status, usage = os.wait4(p.pid, 0)
        p.returncode = os.waitstatus_to_exitcode(status)
        stdout, stderr = p.communicate()
        if p.returncode or stderr:
            raise RuntimeError(f'{command}: exit={p.returncode}\n{stdout}{stderr}')
        row = dict(part.split('=', 1) for part in stdout.split())
        row = {k: (v if k in ('mode', 'probe') else int(v)) for k, v in row.items()}
        row.update(peak_rss_kib=usage.ru_maxrss, user_cpu_seconds=usage.ru_utime, system_cpu_seconds=usage.ru_stime)
        return row, stdout

    probe_result, probe_stdout = execute(probe, 'read_bind_build', 1)
    assert probe_result['cpp_new_allocations'] == 0
    (EVIDENCE / 'allocation-probe.stdout').write_text(probe_stdout)
    results, raw = [], []
    # Alternate order each sample to avoid always measuring one mode first.
    for sample in range(args.samples):
        modes = ['build', 'read_bind_build'] if sample % 2 == 0 else ['read_bind_build', 'build']
        for mode in modes:
            row, stdout = execute(exe, mode, args.iterations)
            row['sample'] = sample
            row['nanoseconds_per_iteration'] = row['nanoseconds'] / row['iterations']
            assert row['iterations'] == args.iterations and row['input_bytes'] == 167234
            results.append(row); raw.append(stdout)
    assert len({r['checksum'] for r in results}) == 1
    assert len({r['result_checksum'] for r in results} | {probe_result['result_checksum']}) == 1
    stats = {}
    for mode in ['build', 'read_bind_build']:
        rows = [r for r in results if r['mode'] == mode]
        values = sorted(r['nanoseconds_per_iteration'] for r in rows)
        stats[mode] = {'minimum_ns': min(values), 'median_ns': statistics.median(values),
                       'maximum_ns': max(values), 'mean_ns': statistics.mean(values),
                       'peak_rss_kib_range': [min(r['peak_rss_kib'] for r in rows), max(r['peak_rss_kib'] for r in rows)],
                       'post_exec_vm_rss_kib_range': [min(r['post_exec_vm_rss_kib'] for r in rows), max(r['post_exec_vm_rss_kib'] for r in rows)],
                       'post_exec_vm_hwm_kib_range': [min(r['post_exec_vm_hwm_kib'] for r in rows), max(r['post_exec_vm_hwm_kib'] for r in rows)]}
    for path in [source, Path(__file__), ROOT / 'tools/measure_authored_geometry49o.py', *[FIXTURES / n for n in NAMES]]:
        hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    check_stable_sources(before, hashes)
    result = {
        'status': 'measured', 'compiler': run(['g++', '--version']).splitlines()[0],
        'compiler_target': run(['g++', '-dumpmachine']).strip(),
        'flags': FLAGS, 'compile_commands': commands, 'source_hashes': hashes,
        'timed_executable_sha256': digest(exe), 'allocation_probe_executable_sha256': digest(probe),
        'cpu_affinity': [cpu], 'available_cpu_affinity': cpus,
        'normal_virtual_address_limit_bytes': cap, 'rounding': 'FE_TONEAREST checked in each process',
        'samples_per_mode': args.samples, 'iterations_per_sample': args.iterations,
        'warmup_builds_per_sample': 16, 'statistics': stats, 'results': results,
        'untimed_allocation_probe': probe_result,
        'input_fixture_bytes': {name: (FIXTURES / name).stat().st_size for name in NAMES},
        'limits': [
            'Actual fresh native geometry build from five immutable source fixtures. No expected-value fixture, original-word oracle, emulator, or renderer is linked into timed work.',
            'build mode admits PSG/MOT views and VUT bindings once outside timing. Every iteration then reloads source locals, samples paired motion, computes live aggregate bounds, and binds fresh authored geometry.',
            'read_bind_build mode also repeats PSG/MOT admission and VUT binding on already-loaded bytes each iteration. Neither mode includes file I/O, RES decompression, MCF selection, decoder memory, or cold-cache disk latency.',
            'Caller-owned fixed workspace and output storage are reused; outputs are independent fresh builds, not incremental tick updates. Allocation/free of fixture buffers and warmup are outside timing.',
            'Timed loop includes result checks, branching and a small radius checksum sink. No allocation counter is compiled into the timed executable.',
            'Separate normal untimed probe counts C++ operator new calls during read/bind/build only; static nm audit separately checks direct core allocator references. No allocation-failure testing.',
            'wait4 peak RSS includes the pre-exec Python child, loader/runtime, process startup, fixture buffers, fixed records and stack high-water; it is not the core resident footprint.',
            'Post-exec /proc/self/status VmRSS/VmHWM are sampled after timing and describe the benchmark address space including runtime, fixtures and stack, not a numeric-core-only or whole-game budget.',
            'Single available CPU affinity and successful 64 MiB RLIMIT_AS cap constrain a normal host run, not a target-console simulation or memory budget proof.',
            'Repeated warm host load-time microbenchmarks only. No gameplay FPS, per-frame speedup, or state-memory reduction claim.'
        ]
    }
    (EVIDENCE / 'samples.stdout').write_text(''.join(raw))
    (EVIDENCE / 'verification.json').write_text(json.dumps(result, indent=2) + '\n')
    (EVIDENCE / 'timed-executable-size.txt').write_text(run(['size', '-B', str(exe)]))
    print(json.dumps(result))


if __name__ == '__main__':
    main()
