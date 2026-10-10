#!/usr/bin/env python3
"""Verify the source-frozen single-motion sampler against original instructions.

Run with Python -B. Does not regenerate or modify historical49O/49P evidence.
Expected values come from pinned original ELF words, never native equations.
"""
import argparse
import ast
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'evidence/engineAnimation49Q/single-motion'
ELF = ROOT / 'work/native/project/disc/SLUS_202.68'
OLD = ROOT / 'evidence/authoredGeometry49O/motion'
ASSETS = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures'
SOURCE_MUTATIONS = [
    ('omit_model18_zero', '155a34', '00000000'),
    ('sample_time_add_instead_of_multiply', '155adc', '4600a500'),
    ('rotation_omit_fourth_zero', '28a634', '00000000'),
    ('position_omit_z_write', '28ac38', '00000000'),
    ('rotation_ready_gate_inverted', '28a6b4', '1460000b'),
    ('position_ready_gate_inverted', '28ac44', '1460000b'),
    ('ancestor_bounds_or_to_and', '28c508', '00661824'),
    ('skip_sibling_invalidation', '289a34', '10000014'),
    ('byteaxis_scale_changed', '100eb0', '3c033c00'),
]
NATIVE_MUTATIONS = [
    ('single_is_not_paired_shortcut', 'e.rotationMatrix(q,local);', 'e.rotationMatrix(e.blend(q,q,0),local);'),
    ('position_final_copy_loses_signed_zero', 'p[i]=add(b.position[i],mul(u,sub(b.position[i],a.position[i])))', 'p[i]=b.position[i]'),
    ('omit_model18_zero', 'state.model18=0;', 'state.model18=1;'),
    ('refreshes_world_cache', 'std::memcpy(graph.nodes,workspace.nodes,bytes);', 'std::memcpy(graph.nodes,workspace.nodes,bytes); graph.nodes[0].world100=graph.nodes[0].localC0;'),
    ('wrong_track_order', 'for(std::uint32_t channel=0;channel<2;++channel){', 'for(std::uint32_t order=0;order<2;++order){const auto channel=1-order;'),
    ('overwrite_translation_fourth', 'e.rotationMatrix(q,local);', 'e.rotationMatrix(q,local);local.v[15]=1;'),
    ('drop_per_track_scene_dirty', 'if(!setLocalMatrix49P(staged,target,local))', 'if((void(staged),workspace.nodes[static_cast<unsigned>(target)].localC0=local,false))'),
]


def sha(b):
    return hashlib.sha256(b).hexdigest()


def verify():
    raw = ELF.read_bytes()
    manifest = json.loads((SOURCE / 'source-manifest.json').read_text())
    assert sha(raw) == manifest['elf_sha256']
    phoff = struct.unpack_from('<I', raw, 28)[0]
    size, count = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, phoff + i * size) for i in range(count)]
    fixture = (ROOT / manifest['fixture']['path']).read_bytes()
    assert sha(fixture) == manifest['fixture']['sha256']
    assert sha((ROOT / manifest['oracle_base']['path']).read_bytes()) == manifest['oracle_base']['sha256']
    listing = {int(line.split()[0], 16): int(line.split()[1], 16)
               for line in (SOURCE / 'source-disassembly.txt').read_text().splitlines()}
    bounds = [(int(a, 16), int(z, 16)) for a, z in re.findall(r'\{0x([a-f0-9]+),0x([a-f0-9]+)\}', (SOURCE / 'blocks.h').read_text())]
    assert bounds == [(int(b['start'], 16), int(b['end_exclusive'], 16)) for b in manifest['blocks']]
    words = {}
    for b in manifest['blocks']:
        a, z = int(b['start'], 16), int(b['end_exclusive'], 16)
        data = next(raw[o+a-v:o+z-v] for k, o, v, _, n, *_ in segments if k == 1 and v <= a < z <= v+n)
        assert sha(data) == b['sha256'] and len(data) == b['words']*4
        assert fixture[a:z] == data
        for i, w in enumerate(struct.unpack('<%dI' % b['words'], data)):
            p = a+4*i
            assert listing[p] == w and p not in words
            words[p] = w
    assert len(words) == manifest['word_count'] == len(listing)
    # Pin original software-produced tables and untouched native49O constants.
    historical = json.loads((OLD / 'motion-proof-manifest.json').read_text())
    for name in ('src/game/model_motion49o.cpp', 'src/game/model_motion49o_tables.inc'):
        if name in historical['artifacts']:
            assert sha((ROOT / name).read_bytes()) == historical['artifacts'][name]['sha256']
    for name, pin in manifest['retained_inputs'].items():
        data = (ROOT/name).read_bytes()
        assert len(data) == pin['bytes'] and sha(data) == pin['sha256'], name+' retained input changed'
    return manifest, words


def account(path, words):
    report = json.loads(path.read_text())
    assert report['ordered_store_tracing'], 'exact flag and track hooks required'
    # Read prior proven evaluator exclusions as data, never execute its writer.
    tree = ast.parse((OLD / 'account_coverage.py').read_text())
    assignments = {n.targets[0].id: ast.literal_eval(n.value) for n in tree.body
                   if isinstance(n, ast.Assign) and isinstance(n.targets[0], ast.Name)
                   and n.targets[0].id in ('excluded', 'branch')}
    exclusions = [(a, z, reason) for a, z, reason in assignments['excluded']
                  if 0x100000 <= a < 0x155900 or a == 0x28a670 or a >= 0x28a6f4
                  if not (0x28ac4c <= a < 0x28ac74)]
    exclusions += [
        (0x155a5c, 0x155ad0, 'Fraction outside finite[0,1] enters assertion/debug path, rejected before native mutation.'),
        (0x155ae8, 0x155b10, 'Rotation exclusion-mask reads and branch-likely are outside mask=null admission.'),
        (0x155b3c, 0x155c18, 'Mirrored rotation reflection/remapping is outside mirror=false admission.'),
        (0x155c70, 0x155c9c, 'Position exclusion-mask reads and duplicate argument setup; mask=null jumps to155C9C.'),
        (0x155cc8, 0x155df4, 'Mirrored position coordinate conversion/remapping is outside mirror=false admission.'),
        (0x106790, 0x1067b0, 'Historical paired-wrapper value constructor; no call from selected1559F0 dependency path.'),
        (0x289b18, 0x289b20, 'Alignment after descendant-invalidation return and delay slot.'),
    ]
    restrictions = {p: r for p, r in assignments['branch'].items()
                    if p < 0x155900 or p == 0x28a5bc}
    restrictions.update({0x155a38: 'fraction>=0 admission', 0x155a54: 'fraction<=1 admission',
                         0x155ae0: 'mask=null admission', 0x155b34: 'mirror=false admission',
                         0x155c68: 'mask=null admission', 0x155cc0: 'mirror=false admission'})
    reached = branches = both = 0
    assert {r['pc'] for r in report['pcs']} == set(words)
    for row in report['pcs']:
        p = row['pc']; w = words[p]; row['word'] = '%08x' % w
        if not row['hits']:
            row['unreached_reason'] = next((r for a, z, r in exclusions if a <= p < z), None)
            assert row['unreached_reason'], 'unaccounted original PC %08x' % p
            continue
        reached += 1
        if row['taken'] or row['not_taken']:
            branches += 1
            assert row['taken']+row['not_taken'] == row['hits']
            if row['taken'] and row['not_taken']:
                both += 1
            elif w >> 26 == 4 and (w >> 21 & 31) == (w >> 16 & 31):
                row['one_sided_reason'] = 'Unconditional BEQ with identical registers.'
            else:
                assert p in restrictions, 'unaccounted branch direction %08x' % p
                row['one_sided_reason'] = restrictions[p]
    report['accounting'] = dict(frozen_words=len(words), reached_words=reached,
                                unreached_words_with_specific_reason=len(words)-reached,
                                reached_branches=branches, both_directions=both,
                                one_sided_with_specific_reason=branches-both,
                                unaccounted_words_or_branch_directions=0)
    path.write_text(json.dumps(report, indent=2)+'\n')
    return report


def execute(cmd, out, name, env=None, expect=0):
    r = subprocess.run([str(x) for x in cmd], cwd=ROOT, env=env, text=True, capture_output=True)
    (out/(name+'.stdout')).write_text(r.stdout)
    (out/(name+'.stderr')).write_text(r.stderr)
    assert r.returncode == expect, '%s exit%d:\n%s\n%s' % (name, r.returncode, r.stdout, r.stderr)
    return r


def run(args, build):
    manifest, words = verify(); build.mkdir(parents=True, exist_ok=True)
    out = args.output_dir.resolve() if args.output_dir else SOURCE / ('sanitized' if args.sanitizers else 'normal')
    out.mkdir(parents=True, exist_ok=True)
    flags = ['-std=c++17', '-O2', '-g', '-Wall', '-Wextra', '-Werror', '-pedantic', '-ffp-contract=off', '-fno-fast-math']
    if args.sanitizers:
        flags += ['-O1', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    def compile(exe, src):
        cmd = [os.environ.get('CXX', 'g++'), *flags, '-I', ROOT/'src/game', '-include', SOURCE/'flag_trace.h',
               ROOT/'tests/native/model_motion49q.cpp', src, ROOT/'src/game/scene_graph49p.cpp',
               ROOT/'src/assets/model_geometry49o.cpp', ROOT/'src/assets/model_bindings49o.cpp', '-o', exe]
        execute(cmd, out, 'compile_'+exe.name)
        return cmd
    exe = build/'model_motion49q'; cmd = compile(exe, ROOT/'src/game/model_motion49q.cpp')
    env = dict(os.environ); env['RR_MODEL49Q_COVERAGE'] = str(out/'coverage.json')
    if args.sanitizers:
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'; env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    result = execute([exe, ROOT], out, 'model_motion49q', env)
    coverage = account(out/'coverage.json', words)
    env.pop('RR_MODEL49Q_COVERAGE')
    sensitivity = []
    for name, pc, word in SOURCE_MUTATIONS:
        r = execute([exe, ROOT, pc, word], out, 'source_'+name, env, expect=1)
        assert 'mismatch' in r.stderr, name+': '+r.stderr
        sensitivity.append(dict(name=name, kind='original-word substitution', pc=pc, replacement=word, detected=True, diagnostic=r.stderr.strip()))
    if not args.skip_native_mutations:
        src = (ROOT/'src/game/model_motion49q.cpp').read_text()
        for name, before, after in NATIVE_MUTATIONS:
            assert src.count(before) == 1, name+' anchor count'
            mutant = build/(name+'.cpp'); mutant.write_text(src.replace(before, after))
            mutant_exe = build/name; compile(mutant_exe, mutant)
            r = execute([mutant_exe, ROOT], out, 'native_'+name, env, expect=1)
            assert 'mismatch' in r.stderr or 'native missing flag store' in r.stderr, name+': '+r.stderr
            sensitivity.append(dict(name=name, kind='native semantic control', detected=True, diagnostic=r.stderr.strip()))
    report = dict(status='pass', mode='ASan+UBSan (LSan disabled)' if args.sanitizers else 'normal',
                  elf_sha256=manifest['elf_sha256'], command=[str(x) for x in cmd],
                  native_result=result.stdout.strip(), source_words_verified=len(words),
                  original_instruction_accounting=coverage['accounting'], native_allocations=0,
                  source_mutations_detected=len(SOURCE_MUTATIONS), native_mutations_detected=0 if args.skip_native_mutations else len(NATIVE_MUTATIONS),
                  sensitivity=sensitivity, scope=manifest['scope'])
    (out/'native-test-result.json').write_text(json.dumps(report, indent=2)+'\n')
    print(result.stdout.strip()); print('Verified%d original words; detected%d mutation controls.' % (len(words), len(sensitivity)))


def main():
    p = argparse.ArgumentParser(description=__doc__);p.add_argument('--check', action='store_true');p.add_argument('--build-dir', type=Path);p.add_argument('--output-dir', type=Path);p.add_argument('--sanitizers', action='store_true');p.add_argument('--skip-native-mutations', action='store_true');a = p.parse_args()
    if a.check:
        manifest, words = verify();print(json.dumps(dict(status='verified', elf_sha256=manifest['elf_sha256'], words=len(words))))
    elif a.build_dir:
        run(a, a.build_dir.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='rr-motion49q-') as b:run(a, Path(b))


if __name__ == '__main__':
    main()
