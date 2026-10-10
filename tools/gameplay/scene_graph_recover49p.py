#!/usr/bin/env python3
"""Verify frozen original scene words and differential persistent-cache behavior.

No RAM, native-output oracle, loader ownership, reset, or runtime installation.
Python -B is required by the enclosing validation workflow.
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
SOURCE = ROOT / 'evidence/sceneProjection49P/source'
ELF = ROOT / 'work/native/project/disc/SLUS_202.68'
PSG = ROOT / 'evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg'
MUTATIONS = [
    ('root_matrix_drop_second_lane_store', '289ce0', '00000000'),
    ('root_relative_add_to_subtract', '28b108', '46000801'),
    ('ancestor_bounds_or_to_and', '28c508', '00661824'),
    ('skip_next_sibling_invalidation', '289a34', '10000014'),
    ('omit_ready_sibling_flag_store', '289a70', '00000000'),
    ('invert_ready_getter_gate', '2897c8', '10600057'),
    ('inline_world_final_add_to_subtract', '289894', '4be5224f'),
    ('recursive_world_final_add_to_subtract', '289964', '4be5224f'),
    ('direct_local_omit_fourth_lane_pairs', '28b4a4', '24040006'),
]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def verify():
    raw = ELF.read_bytes()
    manifest = json.loads((SOURCE / 'scene-manifest.json').read_text())
    assert digest(raw) == manifest['elf_sha256'], 'ELF identity'
    phoff = struct.unpack_from('<I', raw, 28)[0]
    size, count = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, phoff + i * size) for i in range(count)]
    frozen = (SOURCE / 'scene_words.h').read_text()
    decoded = {}
    for block in manifest['blocks']:
        a, z = int(block['start'], 16), int(block['end_exclusive'], 16)
        data = None
        for kind, offset, va, _, filesz, _, _, _ in segments:
            if kind == 1 and va <= a < z <= va + filesz:
                data = raw[offset + a - va:offset + z - va]
                break
        assert data is not None, 'non-file-backed block'
        assert digest(data) == block['sha256'], block['name']
        match = re.search(r'\b' + block['name'] + r'\[\]=\{(.*?)\};', frozen, re.S)
        assert match, block['name']
        words = [int(w, 16) for w in re.findall(r'0x([a-fA-F0-9]+)u', match[1])]
        assert len(words) == block['words']
        assert struct.pack('<%dI' % len(words), *words) == data, block['name']
        text_words = [(int(line.split()[0], 16), int(line.split()[1], 16))
                      for line in (SOURCE / (block['name'] + '.txt')).read_text().splitlines()]
        assert text_words == [(a + 4 * i, w) for i, w in enumerate(words)], block['name']
        decoded.update(text_words)
    context = json.loads((SOURCE / 'boundary-context-manifest.json').read_text())
    assert context['elf_sha256'] == manifest['elf_sha256']
    for block in context['blocks']:
        a, z = int(block['start'], 16), int(block['end_exclusive'], 16)
        data = None
        for kind, offset, va, _, filesz, _, _, _ in segments:
            if kind == 1 and va <= a < z <= va + filesz:
                data = raw[offset + a - va:offset + z - va]
                break
        assert data is not None and digest(data) == block['sha256'], block['name']
        listed = [(int(line.split()[0], 16), int(line.split()[1], 16))
                  for line in (SOURCE / (block['name'] + '.txt')).read_text().splitlines()]
        assert listed == [(a + i * 4, w) for i, w in enumerate(struct.unpack('<%dI' % block['words'], data))]
    provenance = manifest['decoder_provenance']
    assert digest((ROOT / provenance['path']).read_bytes()) == provenance['sha256'], '49O decoder changed'
    assert len(decoded) == manifest['word_count'], 'duplicate source word'
    assert digest(PSG.read_bytes()) == '203232e291b5098ddfeb437d59748105f33a4432bad1a9359d96681fc5d03b59'
    motion = ROOT / 'evidence/authoredGeometry49O/motion/motion-output.json'
    header = (ROOT / 'evidence/authoredGeometry49O/bounds/pod01_live_matrices.h').read_text()
    assert digest(motion.read_bytes()) == re.search(r'motionJsonSha256\[\]="([a-f0-9]+)"', header)[1]
    local_words = [int(w, 16) for w in re.findall(r'0x([a-f0-9]+)u', header)]
    motion_data = json.loads(motion.read_text())
    assert [n['index'] for n in motion_data['nodes']] == list(range(18))
    assert local_words == [int(w, 16) for n in motion_data['nodes'] for w in n['local_matrix_bits']]
    return manifest, decoded


def exclusion(pc):
    if 0x21b9a4 <= pc < 0x21b9d4:
        return 'Separate Chariot adapter gate/dispatch test owns this linked source block.'
    if 0x289b44 <= pc < 0x289b54:
        return 'relative==target is outside parentless relative-null root assignment.'
    if 0x289b5c <= pc < 0x289c98:
        return 'Non-null relative scene conversion is excluded.'
    if 0x289ca4 <= pc < 0x289ccc:
        return 'Parented target conversion and its duplicate setup are excluded.'
    if 0x28afa8 <= pc < 0x28b01c:
        return 'relative==target is outside parentless relative-null translation.'
    if 0x28b024 <= pc < 0x28b0d0:
        return 'Non-null relative conversion and its duplicate setup are excluded.'
    if 0x28b0e0 <= pc < 0x28b0f4:
        return 'Parented translation target conversion is excluded.'
    if pc in (0x28981c, 0x2898e4):
        return 'Unreachable duplicate argument setup after unconditional source branch.'
    if pc in (0x289d4c, 0x28b17c, 0x28b518, 0x28b51c, 0x289b18, 0x289b1c, 0x28993c):
        return 'Alignment word beyond return and its executed delay slot.'
    raise AssertionError('Unaccounted uncovered original instruction %08x' % pc)


def account(path, words):
    report = json.loads(path.read_text())
    assert report['ordered_store_tracing'] is True, 'focused proof requires actual native flag hooks'
    branch_count = both = reached = 0
    restrictions = {
        0x289b3c: 'relative=null differs from valid target node',
        0x289b54: 'relative=null required',
        0x289c9c: 'target.parent=null required',
        0x28afa0: 'relative=null differs from valid target node',
        0x28b01c: 'relative=null required',
        0x28b0d8: 'target.parent=null required',
    }
    assert {row['pc'] for row in report['pcs']} == set(words)
    for row in report['pcs']:
        pc, word = row['pc'], row['word']
        assert word == words[pc]
        if not row['hits']:
            row['unreached_reason'] = exclusion(pc)
            continue
        reached += 1
        op, rs, rt = word >> 26, (word >> 21) & 31, (word >> 16) & 31
        if op not in (4, 5, 7):
            assert not (row['taken'] or row['not_taken'])
            continue
        branch_count += 1
        assert row['taken'] + row['not_taken'] == row['hits']
        if row['taken'] and row['not_taken']:
            both += 1
        elif op == 4 and rs == rt:
            assert row['not_taken'] == 0
            row['unreached_branch_reason'] = 'Unconditional original BEQ with identical registers.'
        else:
            assert pc in restrictions, 'Unaccounted branch direction %08x' % pc
            row['unreached_branch_reason'] = restrictions[pc]
    report['accounting'] = {'frozen_words': len(words), 'reached_words': reached,
                            'unreached_words_with_specific_reason': len(words) - reached,
                            'reached_branch_pcs': branch_count, 'both_directions': both,
                            'single_direction_with_specific_reason': branch_count - both,
                            'unknown_executed_pcs': 0, 'unaccounted_words_or_directions': 0}
    path.write_text(json.dumps(report, indent=2) + '\n')
    return report


def execute(command, out, name, environment=None):
    result = subprocess.run([str(x) for x in command], cwd=ROOT, env=environment,
                            text=True, capture_output=True)
    (out / (name + '.stdout')).write_text(result.stdout)
    (out / (name + '.stderr')).write_text(result.stderr)
    assert result.returncode == 0, '%s failed:\n%s\n%s' % (name, result.stdout, result.stderr)
    return result


def run(args, build):
    manifest, words = verify()
    build.mkdir(parents=True, exist_ok=True)
    output = args.output_dir.resolve() if args.output_dir else build
    output.mkdir(parents=True, exist_ok=True)
    exe = build / 'scene_graph49p'
    flags = ['-std=c++17', '-O2', '-g', '-Wall', '-Wextra', '-Werror', '-pedantic',
             '-ffp-contract=off', '-fno-fast-math']
    if args.sanitizers:
        flags += ['-O1', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    command = [os.environ.get('CXX', 'g++'), *flags,
               '-include', SOURCE / 'flag_trace.h', ROOT / 'tests/native/scene_graph49p.cpp',
               ROOT / 'src/game/scene_graph49p.cpp', '-o', exe]
    execute(command, output, 'compile')
    env = dict(os.environ)
    env['RR_SCENE49P_COVERAGE'] = str(output / 'coverage.json')
    if args.sanitizers:
        env['ASAN_OPTIONS'] = 'detect_leaks=0:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    result = execute([exe, PSG], output, 'scene_graph49p', env)
    coverage = account(output / 'coverage.json', words)
    sensitivity = []
    for name, pc, word in MUTATIONS:
        r = subprocess.run([str(exe), str(PSG), pc, word], cwd=ROOT, env=env,
                           text=True, capture_output=True)
        assert r.returncode == 1 and 'mismatch' in r.stderr, name + ': ' + r.stderr
        sensitivity.append({'name': name, 'kind': 'original-word substitution', 'pc': pc,
                            'word': word, 'detected': True, 'diagnostic': r.stderr.strip()})
    # Current-before-siblings preserves the same terminal graph values for
    # these disjoint known-node trees. Only the ordered flag-store proof detects
    # it, so this control prevents final-state equality hiding a traversal bug.
    src = (ROOT / 'src/game/scene_graph49p.cpp').read_text()
    old = '''    if (node.nextSibling188 != none) invalidate(graph, node.nextSibling188);
    if ((node.sceneFlags190 & 1u) == 0) return;
    storeFlags(graph, index, static_cast<std::uint8_t>(node.sceneFlags190 & 0xfeu));
    if (node.firstChild184 != none) invalidate(graph, node.firstChild184);
}'''
    new = '''    if ((node.sceneFlags190 & 1u) != 0) {
        storeFlags(graph, index, static_cast<std::uint8_t>(node.sceneFlags190 & 0xfeu));
        if (node.firstChild184 != none) invalidate(graph, node.firstChild184);
    }
    if (node.nextSibling188 != none) invalidate(graph, node.nextSibling188);
}'''
    assert src.count(old) == 1, 'traversal control source anchor changed'
    mutant = build / 'scene_graph_current_first.cpp'
    mutant.write_text(src.replace(old, new))
    mutant_exe = build / 'scene_graph_current_first'
    mutant_command = [os.environ.get('CXX', 'g++'), *flags, '-I', ROOT / 'src/game',
                      '-include', SOURCE / 'flag_trace.h', ROOT / 'tests/native/scene_graph49p.cpp',
                      mutant, '-o', mutant_exe]
    execute(mutant_command, output, 'compile_current_first')
    r = subprocess.run([str(mutant_exe), str(PSG)], cwd=ROOT, env=env, text=True, capture_output=True)
    assert r.returncode == 1 and 'flag store order/value mismatch' in r.stderr, r.stderr
    sensitivity.append({'name': 'current_before_siblings_same_terminal_state', 'kind': 'native traversal control',
                        'detected': True, 'diagnostic': r.stderr.strip()})
    report = {'status': 'pass', 'mode': 'ASan+UBSan; leak detection disabled' if args.sanitizers else 'normal',
              'native_result': result.stdout.strip(), 'elf_sha256': manifest['elf_sha256'],
              'original_word_ranges_verified': len(manifest['blocks']), 'source_words_verified': len(words), 'static_boundary_ranges_verified': 5,
              'original_instruction_accounting': coverage['accounting'], 'native_hot_allocations': 0,
              'sensitivity': sensitivity, 'strict_fp': '-ffp-contract=off -fno-fast-math; FE_TONEAREST',
              'source_inputs': {'psg_sha256': digest(PSG.read_bytes()),
                                'motion_locals': '49O original-word motion-output.json verified against frozen liveLocal header'},
              'scope': manifest['scope'], 'command': [str(c) for c in command]}
    (output / 'native-test-result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(result.stdout.strip())
    print('Verified %d frozen ranges/%d words; %d mutation controls detected; accounting complete.' %
          (len(manifest['blocks']), len(words), len(sensitivity)))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='verify ELF/frozen sources/assets only')
    parser.add_argument('--build-dir', type=Path)
    parser.add_argument('--output-dir', type=Path)
    parser.add_argument('--sanitizers', action='store_true')
    args = parser.parse_args()
    if args.check:
        manifest, words = verify()
        print(json.dumps({'status': 'verified', 'elf_sha256': manifest['elf_sha256'],
                          'blocks': len(manifest['blocks']), 'words': len(words)}))
    elif args.build_dir:
        run(args, args.build_dir.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='rr-scene49p-') as path:
            if args.output_dir is None:
                args.output_dir = SOURCE / ('sanitized' if args.sanitizers else 'normal')
            run(args, Path(path))


if __name__ == '__main__':
    main()
