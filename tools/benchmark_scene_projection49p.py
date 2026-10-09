#!/usr/bin/env python3
"""CPU-pinned native 49P provided-matrix fixture timings, not original gameplay."""
import argparse
import json
import os
import resource
import statistics
import subprocess
import sys
from pathlib import Path

sys.dont_write_bytecode = True
from measure_scene_projection49p import (ROOT, SOURCES, FLAGS, run, digest,
    dependency_paths, snapshot_sources, check_stable_sources, sections)

EVIDENCE = ROOT / 'evidence/sceneProjection49P/benchmark'
FIXTURE = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg'
MODES = ['ordered_copy_later_setters_getters', 'ready_getters']


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=15)
    parser.add_argument('--iterations', type=int, default=20000)
    args = parser.parse_args()
    if args.samples < 5 or args.iterations < 2 or args.iterations % 2:
        parser.error('At least five samples and a positive even iteration count >=2 are required.')
    build = args.build_dir.resolve()
    build.mkdir(parents=True, exist_ok=False)
    EVIDENCE.mkdir(parents=True, exist_ok=True)
    source = EVIDENCE / 'scene_benchmark49p.cpp'
    before = snapshot_sources()
    objects, hashes, commands = [], {}, []
    for name in [*SOURCES, 'assets/model_geometry49o']:
        obj, dep = build / (Path(name).name + '.o'), build / (Path(name).name + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(ROOT / 'src' / (name + '.cpp')), '-o', str(obj)]
        commands.append(command); run(command); objects.append(str(obj))
        for path in dependency_paths(dep):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    exe, probe = build / 'benchmark', build / 'ordinary-new-probe'
    for output, extra in [(exe, []), (probe, ['-DALLOCATION_PROBE49P'])]:
        dep = build / (output.name + '.d')
        command = ['g++', *FLAGS, *extra, '-MMD', '-MF', str(dep), '-I', str(ROOT / 'src'),
                   str(source), *objects, '-o', str(output)]
        commands.append(command); run(command)
        for path in dependency_paths(dep):
            hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    for path in build.glob('*.su'):
        (EVIDENCE / path.name).write_text(path.read_text())
    cpus = sorted(os.sched_getaffinity(0))
    cpu = 0 if 0 in cpus else cpus[0]
    cap = 64 * 1024 * 1024

    def limits():
        os.sched_setaffinity(0, {cpu})
        resource.setrlimit(resource.RLIMIT_AS, (cap, cap))

    def execute(binary, mode, count):
        command = [str(binary), str(FIXTURE), str(count), mode]
        p = subprocess.Popen(command, cwd=ROOT, text=True, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, preexec_fn=limits)
        _, status, usage = os.wait4(p.pid, 0)
        p.returncode = os.waitstatus_to_exitcode(status)
        stdout, stderr = p.communicate()
        if p.returncode or stderr:
            raise RuntimeError(f'{command}: exit={p.returncode}\n{stdout}{stderr}')
        row = dict(part.split('=', 1) for part in stdout.split())
        row = {k: v if k == 'mode' else int(v) for k, v in row.items()}
        row.update(peak_rss_kib=usage.ru_maxrss, user_cpu_seconds=usage.ru_utime,
                   system_cpu_seconds=usage.ru_stime)
        return row, stdout

    probes, raw_probes = [], []
    for mode in MODES:
        row, stdout = execute(probe, mode, 2)
        assert row['cpp_new_allocations'] == 0 and row['nanoseconds'] == 0
        probes.append(row); raw_probes.append(stdout)
    (EVIDENCE / 'ordinary-new-probe.stdout').write_text(''.join(raw_probes))
    results, raw = [], []
    for sample in range(args.samples):
        for mode in MODES if sample % 2 == 0 else MODES[::-1]:
            row, stdout = execute(exe, mode, args.iterations)
            row['sample'] = sample
            row['nanoseconds_per_iteration'] = row['nanoseconds'] / row['iterations']
            assert row['nodes'] == 22 and row['input_bytes'] == FIXTURE.stat().st_size
            results.append(row); raw.append(stdout)
    stats = {}
    for mode in MODES:
        rows = [r for r in results if r['mode'] == mode]
        values = [r['nanoseconds_per_iteration'] for r in rows]
        probe_row = next(r for r in probes if r['mode'] == mode)
        assert len({r['checksum'] for r in rows}) == 1
        assert {r['result_checksum'] for r in rows} == {probe_row['result_checksum']}
        stats[mode] = {'minimum_ns': min(values), 'median_ns': statistics.median(values),
                       'maximum_ns': max(values), 'mean_ns': statistics.mean(values),
                       'peak_rss_kib_range': [min(r['peak_rss_kib'] for r in rows), max(r['peak_rss_kib'] for r in rows)],
                       'post_exec_vm_rss_kib_range': [min(r['post_exec_vm_rss_kib'] for r in rows), max(r['post_exec_vm_rss_kib'] for r in rows)],
                       'post_exec_vm_hwm_kib_range': [min(r['post_exec_vm_hwm_kib'] for r in rows), max(r['post_exec_vm_hwm_kib'] for r in rows)]}
    for path in [source, Path(__file__), ROOT / 'tools/measure_scene_projection49p.py', FIXTURE]:
        hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    check_stable_sources(before, hashes)
    executable_sections, executable_size, detail = sections(exe)
    symbols = run(['nm', '-C', str(exe)])
    assert 'countAllocations' not in symbols and 'operator new(' not in symbols
    assert 'scene49p_oracle' not in symbols and 'original::' not in symbols
    (EVIDENCE / 'timed-executable-symbols.txt').write_text(symbols)
    (EVIDENCE / 'timed-executable-size.txt').write_text(executable_size)
    (EVIDENCE / 'timed-executable-sections.txt').write_text(detail)
    result = {
        'status': 'measured', 'compiler': run(['g++', '--version']).splitlines()[0],
        'compiler_target': run(['g++', '-dumpmachine']).strip(), 'flags': FLAGS,
        'compile_commands': commands, 'source_hashes': hashes,
        'timed_executable_sha256': digest(exe), 'ordinary_new_probe_executable_sha256': digest(probe),
        'timed_executable_sections': executable_sections,
        'cpu_affinity': [cpu], 'cpu0_available': 0 in cpus, 'available_cpu_affinity': cpus,
        'normal_virtual_address_limit_bytes': cap, 'rounding': 'FE_TONEAREST checked in each process',
        'samples_per_mode': args.samples, 'iterations_per_sample': args.iterations,
        'warmup_ordered_iterations': 16, 'statistics': stats, 'results': results,
        'separate_untimed_ordinary_new_probes': probes,
        'fixture': {
            'meaning': 'Explicitly provided matrix/current-scene fixture; not actual recovered live physical or visual producer output.',
            'nodes': 22, 'main190': 0, 'child194': 1, 'separate_secondary19C': 2,
            'separate_chariot1B4': 3, 'actual_psg_nodes': list(range(4, 22)),
            'psg_path': str(FIXTURE.relative_to(ROOT)), 'psg_sha256': digest(FIXTURE),
            'retained_psg_bytes': FIXTURE.stat().st_size,
            'local_inputs': 'PSG locals with explicitly supplied post-first-base nullcgXYZ=0. Main two poses, secondary pose,194local and later translations are literal provided-matrix fixtures in scene_benchmark49p.cpp.',
            'complete_supplied_boundary': 'All locals, cached worlds, flags and links are initialized, then retained across ordered operations. Attestation tags are supplied; the omitted external producers are not executed or proved.',
            'gate_and_phase': 'Supplied838=1,83C=0; pre21B9A4 copy precedes later setters and explicit later getters.',
            'ready_getters_per_iteration': 20,
            'ordered_operations_per_iteration': {'project_physical_chariot_branch': 1, 'main_parentless_matrix_setter': 1,
                'secondary_parentless_translation_setter': 1, '194_world_getter': 1,
                'chariot_root_and_18_node_world_getters': 19}},
        'limits': [
            'Warm native-only host numeric microbenchmark on provided matrices and the actual18-node Pod01PSG hierarchy. No emulation/oracle, expected-word fixture, renderer, animation, engine response or physics is linked into timed work.',
            'ordered mode first copies secondary LOCAL at the supplied pre21B9A4 boundary; later main matrix and secondary translation setters occur before explicit194/Chariot world reads. This preserves the copied Chariot staged delay and is not a post-physics binder.',
            'The provided full-scene fixture attests a boundary but does not prove original external producers reached it. Static provided194local and source-derived current-node locals do not close21C120 or21A000.',
            'ready_getters performs the same20 explicit getters on already-ready caches. It still includes the public APIs complete graph/topology and rounding admission work; it is not raw pointer-read cost.',
            'Caller-owned fixed22-node storage is reused. File I/O, PSG parsing, fixture supply, warmup and initial cache priming occur outside timing.',
            'Timed work includes public API validation, success branches and two small observable matrix-word sinks. Full semantic checksum and memory reads occur after timing. No allocation counter is compiled into the timed executable.',
            'Separate untimed normal probes count ordinary C++ new/new[] during two iterations of each mode; they do not measure malloc/aligned allocations globally or constitute allocation-failure testing.',
            'wait4 peak RSS includes the pre-exec Python child and startup high-water, loader/runtime, input bytes, stack and graph records; it is not49P resident memory.',
            'Post-exec/proc/self/status VmRSS and VmHWM are sampled after timing; they include runtime, retained101596-bytePSG fixture, caller records and stack, not just49P or the whole game.',
            'Successful CPU0 pinning where available and64MiB RLIMIT_AS constrain a normal host run; neither simulates target-console hardware nor proves a console budget.',
            'No comparison to49O timing is valid:49O builds geometry at load time; this49P benchmark performs different scene operations.',
            'No FPS, Xbox/console-performance, complete-frame, all-three-model, memory-reduction or FrameReady claim.'
        ]}
    (EVIDENCE / 'samples.stdout').write_text(''.join(raw))
    (EVIDENCE / 'verification.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
