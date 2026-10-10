#!/usr/bin/env python3
"""Verify the 49R graphics-container prefix against independently executed original words.
Synthetic initialized fixtures only. No RAM, PC/game launch, 49C, install or push.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'evidence/vehicleVisual49R/container/source'
RETAINED = ROOT / 'evidence/engineAnimation49Q/assessment/container-update/source'
ELF = ROOT / 'work/native/project/disc/SLUS_202.68'
MUTATIONS = [
    ('cold_alpha_word', '21c174', '34429999'),
    ('cold_unconsumed_global_store', '21c1b8', '00000000'),
    ('cold_global_guard_store', '21c200', '00000000'),
    ('probe_alpha_arm', '21c2f0', '14400004'),
    ('blend_one_minus_alpha', '21c318', '461500c0'),
    ('blend_old_plus_current', '21c374', '46000801'),
    ('normalized_up_store', '21c3c0', '00000000'),
    ('corrected_forward_cross', '21c3f4', '4bc429a8'),
    ('repeat_up_normalization', '28a1c4', '00000000'),
    ('forward_normalization', '28a1e4', '00000000'),
    ('final_right_cross', '28a1ec', '4bc63968'),
    ('parent_product_fourth_lane', '289e00', '4beb428f'),
    ('parent_transpose_lane', '289e48', '4a8c03c1'),
    ('local_translation_restore', '28a4c8', '00000000'),
    ('bounds_flag_propagation', '28c508', '00661824'),
    ('child_ready_clear', '28a4fc', '00000000'),
    ('descendant_ready_clear', '289a70', '00000000'),
    ('container_world_product', '289964', '4be5224f'),
    ('publish_refreshed_up', '21c43c', '00000000'),
    ('sentinel_guard_store', '28a0c0', '00000000'),
]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def verify():
    manifest = json.loads((SOURCE / 'container-manifest.json').read_text())
    raw = ELF.read_bytes()
    assert sha(raw) == manifest['elf_sha256'], 'original ELF identity changed'
    phoff = struct.unpack_from('<I', raw, 28)[0]
    size, count = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, phoff + i * size) for i in range(count)]
    frozen = (SOURCE / 'container_words.h').read_text()
    decoded = {}
    for b in manifest['blocks']:
        start, end = int(b['start'], 16), int(b['end_exclusive'], 16)
        candidates = [raw[offset + start - va:offset + end - va]
                      for kind, offset, va, _, filesz, _, _, _ in segments
                      if kind == 1 and va <= start < end <= va + filesz]
        assert len(candidates) == 1, b['name']
        data = candidates[0]
        assert sha(data) == b['sha256'], b['name']
        assert data == (RETAINED / (b['name'] + '.bin')).read_bytes(), b['name']
        match = re.search(r'\b' + b['name'] + r'\[\]=\{(.*?)\};', frozen, re.S)
        assert match, b['name']
        words = [int(x, 16) for x in re.findall(r'0x([a-fA-F0-9]+)u', match[1])]
        assert len(words) == b['word_count'] and struct.pack('<%dI' % len(words), *words) == data
        rows = [(int(line.split()[0], 16), int(line.split()[1], 16))
                for line in (RETAINED / (b['name'] + '.txt')).read_text().splitlines()]
        assert rows == [(start + 4 * i, w) for i, w in enumerate(words)], b['name']
        decoded.update(rows)
    assert len(decoded) == manifest['original_words'] == 1048
    for key in ('reader_source', 'sqrt_source'):
        item = manifest[key]
        assert sha((ROOT / item['path']).read_bytes()) == item['sha256'], key
    return manifest, decoded


EXCLUDED = [
    (0x21c440, 0x21c44c, 'Boundary-only mode834 gate; execution stops before21C440.'),
    (0x2287b0, 0x228828, 'Caller-order provenance block is retained, not executed by the prefix.'),
    (0x2897d0, 0x289928, '2897B0 is called only for main parent after the earlier position getter made it ready; dirty refresh arms are unreachable in this exact prefix topology.'),
    (0x28993c, 0x289940, 'Alignment word after return and delay slot.'),
    (0x2899b4, 0x289a20, 'Alignment and separate2899C0 dirty-helper body after the returned matrix product; no call reaches it.'),
    (0x289b18, 0x289b20, 'Alignment words after return and delay slot.'),
    (0x28a0f0, 0x28a134, 'Orientation flags t0=0,t2=0 skip fast construction and duplicate setup.'),
    (0x28a174, 0x28a1a8, 't0=0 skips optional orthogonalization, regardless of t1=1.'),
    (0x28a220, 0x28a324, 'relative=null differs from valid target; relative==target arm is excluded.'),
    (0x28a32c, 0x28a468, 'relative=null excludes nonnull-relative refresh and composition.'),
    (0x28a498, 0x28a4c4, 'Canonical container194 is parented to main190; parentless copy arm is excluded.'),
    (0x28a72c, 0x28a760, 'Axes getter relative=null differs from its valid target.'),
    (0x28a770, 0x28a7e4, 'Each axes getter follows that same target position getter, which already refreshed its cache.'),
    (0x28a82c, 0x28a86c, 'Axes getter relative=null excludes inverse-relative conversion.'),
    (0x28b1c4, 0x28b1e0, 'Position getter relative=null differs from its valid target.'),
    (0x28b220, 0x28b224, 'Unreachable duplicate argument setup after unconditional source branch.'),
    (0x28b280, 0x28b298, 'Position getter relative=null excludes inverse-relative conversion.'),
    (0x28c538, 0x28c578, 'Only container194 and its parentless main190 are in the upward dirty chain; deeper ancestors are outside the admitted topology.'),
]
RESTRICTED_BRANCHES = {
    0x28a0e0: 't0=0', 0x28a0e8: 't2=0', 0x28a16c: 't0=0 differs from1',
    0x28a218: 'relative=null differs from target', 0x28a324: 'relative=null',
    0x28a46c: 'container.parent=main190 is nonnull',
    0x28b1bc: 'relative=null differs from target', 0x28b278: 'relative=null',
    0x28a724: 'relative=null differs from target',
    0x28a768: 'preceding position getter made this exact target ready',
    0x28a7e4: 'forward output supplied', 0x28a804: 'up output supplied',
    0x28a824: 'relative=null',
    0x2897c8: 'main is already ready from the first position getter',
    0x28c514: 'container.parent=main190 is nonnull',
    0x28c530: 'main190.parent=null',
}


def account(path, words):
    report = json.loads(path.read_text())
    assert report['ordered_flag_store_tracing'], 'focused native proof requires actual flag hooks'
    assert {r['pc'] for r in report['pcs']} == set(words)
    reached = both = branches = 0
    for r in report['pcs']:
        pc, word = r['pc'], r['word']
        assert word == words[pc]
        if not r['hits']:
            reasons = [reason for a, z, reason in EXCLUDED if a <= pc < z]
            assert len(reasons) == 1, 'unaccounted original instruction %08x' % pc
            r['unreached_reason'] = reasons[0]
            continue
        reached += 1
        op, rs, rt = word >> 26, (word >> 21) & 31, (word >> 16) & 31
        if op not in (4, 5, 7):
            assert not (r['taken'] or r['not_taken'])
            continue
        branches += 1
        assert r['hits'] == r['taken'] + r['not_taken']
        if r['taken'] and r['not_taken']:
            both += 1
        elif op == 4 and rs == rt:
            assert not r['not_taken']
            r['restricted_branch_reason'] = 'Unconditional BEQ uses identical registers.'
        else:
            assert pc in RESTRICTED_BRANCHES, 'unaccounted branch %08x' % pc
            r['restricted_branch_reason'] = RESTRICTED_BRANCHES[pc]
    report['accounting'] = {'frozen_words': len(words), 'reached_words': reached,
                            'unreached_words_with_specific_reason': len(words) - reached,
                            'reached_branch_pcs': branches, 'both_directions': both,
                            'single_direction_with_specific_reason': branches - both,
                            'unknown_executed_pcs': 0, 'unaccounted_words_or_directions': 0}
    path.write_text(json.dumps(report, indent=2) + '\n')
    return report


def execute(command, output, name, environment=None):
    r = subprocess.run([str(x) for x in command], cwd=ROOT, env=environment,
                       text=True, capture_output=True)
    (output / (name + '.stdout')).write_text(r.stdout)
    (output / (name + '.stderr')).write_text(r.stderr)
    assert r.returncode == 0, '%s failed:\n%s\n%s' % (name, r.stdout, r.stderr)
    return r


def run(args, build):
    manifest, words = verify()
    build.mkdir(parents=True, exist_ok=True)
    output = args.output_dir.resolve() if args.output_dir else build
    output.mkdir(parents=True, exist_ok=True)
    exe = build / 'visual_container49r'
    flags = ['-std=c++20', '-O2', '-g', '-Wall', '-Wextra', '-Werror', '-pedantic',
             '-ffp-contract=off', '-fno-fast-math']
    if args.sanitizers:
        flags += ['-O1', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    command = [os.environ.get('CXX', 'g++'), *flags, '-include',
               ROOT / 'evidence/sceneProjection49P/source/flag_trace.h',
               ROOT / 'tests/native/visual_container49r.cpp',
               ROOT / 'src/game/visual_container49r.cpp', ROOT / 'src/game/scene_graph49p.cpp', '-o', exe]
    execute(command, output, 'compile')
    env = dict(os.environ)
    env['RR_CONTAINER49R_COVERAGE'] = str(output / 'coverage.json')
    if args.sanitizers:
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    result = execute([exe], output, 'visual_container49r', env)
    coverage = account(output / 'coverage.json', words)
    sensitivity = []
    for name, pc, word in MUTATIONS:
        r = subprocess.run([str(exe), pc, word], cwd=ROOT, env=env, text=True, capture_output=True)
        assert r.returncode == 1 and 'mismatch' in r.stderr, name + ': ' + r.stderr
        sensitivity.append({'name': name, 'kind': 'original-word substitution',
                            'pc': pc, 'word': word, 'detected': True, 'diagnostic': r.stderr.strip()})
    native = (ROOT / 'src/game/visual_container49r.cpp').read_text()
    anchor = 'volatile float root = std::sqrt(denominator);\n        return div(1.0f, keep(root));'
    replacement = 'return keep(static_cast<float>(1.0 / std::sqrt(static_cast<double>(denominator))));'
    assert native.count(anchor) == 1, 'source Q control anchor changed'
    mutant = build / 'visual_container_direct_rsqrt.cpp'
    mutant.write_text(native.replace(anchor, replacement))
    mutant_exe = build / 'visual_container_direct_rsqrt'
    mutant_command = [os.environ.get('CXX', 'g++'), *flags, '-I', ROOT / 'src/game',
                      '-include', ROOT / 'evidence/sceneProjection49P/source/flag_trace.h',
                      ROOT / 'tests/native/visual_container49r.cpp', mutant,
                      ROOT / 'src/game/scene_graph49p.cpp', '-o', mutant_exe]
    execute(mutant_command, output, 'compile_direct_rsqrt')
    r = subprocess.run([str(mutant_exe)], cwd=ROOT, env=env, text=True, capture_output=True)
    assert r.returncode == 1 and 'mismatch' in r.stderr, 'native double rsqrt control: ' + r.stderr
    sensitivity.append({'name': 'native_direct_double_rsqrt', 'kind': 'native arithmetic control',
                        'detected': True, 'diagnostic': r.stderr.strip()})
    report = {'status': 'pass' , 'mode': 'ASan+UBSan; leak detection disabled' if args.sanitizers else 'normal',
              'native_result': result.stdout.strip(), 'original_word_ranges_verified': len(manifest['blocks']),
              'source_words_verified': len(words), 'elf_sha256': manifest['elf_sha256'],
              'original_instruction_accounting': coverage['accounting'], 'native_hot_allocations': 0,
              'sensitivity': sensitivity, 'numeric_domain': manifest['numeric_domain'],
              'scope': manifest['scope'], 'command': [str(c) for c in command],
              'proof_limits': ['Final represented node/state/shared/frame words plus exact ordered scene-flag stores are compared.',
                               'No claim that native reproduces every overwritten local-matrix memory store.',
                               'Explicit staged internal API may mutate scratch on false; public full-call atomicity belongs to wrapper proof.',
                               'Production compiles as C++17; test-only retained ee_sqrt14.h needs C++20.']}
    report['file_sha256'] = {str(p.relative_to(ROOT)): sha(p.read_bytes()) for p in [ROOT / 'src/game/visual_container49r.cpp', ROOT / 'src/game/scene_graph49p.cpp', ROOT / 'src/game/vehicle_visual49r.h', ROOT / 'src/game/vehicle_visual49r_internal.h', ROOT / 'tests/native/visual_container49r.cpp', Path(__file__), SOURCE / 'container_oracle.h', SOURCE / 'container_words.h', SOURCE / 'container-manifest.json', SOURCE / 'vu-reference.md']}
    (output / 'native-test-result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(result.stdout.strip())
    print('Verified %d original ranges/%d words; %d mutations detected; all PC/branch accounting complete.' %
          (len(manifest['blocks']), len(words), len(sensitivity)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--build-dir', type=Path)
    parser.add_argument('--output-dir', type=Path)
    parser.add_argument('--sanitizers', action='store_true')
    args = parser.parse_args()
    if args.check:
        manifest, words = verify()
        print('Verified %d frozen original words and retained reader/sqrt identities.' % len(words))
    elif args.build_dir:
        run(args, args.build_dir.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='rr-container49r-') as directory:
            run(args, Path(directory))


if __name__ == '__main__':
    main()
