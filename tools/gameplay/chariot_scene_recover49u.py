#!/usr/bin/env python3
"""Authenticate and prove bounded49U scene/math leaves against original words.

No native-output oracle, substituted call target, RAM, or runtime installation.
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
SOURCE = ROOT / 'evidence/chariotRoot49U/scene/source'
ELF = ROOT / 'work/native/project/disc/SLUS_202.68'
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {
    'axes_getter': (0x28a700, 0x28a888),
    'position_getter': (0x28b1a0, 0x28b2b0),
    'world_direction': (0x28b9b0, 0x28baa8),
    'inverse_direction': (0x28bbb0, 0x28bd48),
    'inverse_position': (0x28bd50, 0x28bee4),
    'relative_matrix': (0x28b550, 0x28b694),
    'parent_conversion': (0x289d50, 0x289ebc),
    'copy_local': (0x28b3e0, 0x28b48c),
    'world_refresh': (0x2897b0, 0x28993c),
    'world_product': (0x289940, 0x2899b4),
    'invalidate_descendants': (0x289a20, 0x289b18),
    'mark_ancestors': (0x28c4f0, 0x28c584),
    'rotate_vector': (0x142b80, 0x142cfc),
    'sincos': (0x13bfe0, 0x13c140),
    'matrix_quaternion': (0x14f950, 0x14fc00),
}

def digest(data):
    return hashlib.sha256(data).hexdigest()

def extract():
    raw = ELF.read_bytes()
    assert digest(raw) == ELF_SHA, 'ELF identity'
    off = struct.unpack_from('<I', raw, 28)[0]
    size, count = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, off + i * size) for i in range(count)]
    result = {}
    for name, (a, z) in BLOCKS.items():
        for kind, offset, va, _, filesz, _, _, _ in segments:
            if kind == 1 and va <= a < z <= va + filesz:
                result[name] = raw[offset + a - va:offset + z - va]
                break
        assert name in result, name + ': non-file-backed block'
    return result

def freeze():
    data = extract()
    SOURCE.mkdir(parents=True, exist_ok=True)
    manifest = {'elf_sha256': ELF_SHA, 'blocks': []}
    h = ['#pragma once', '#include <cstdint>', 'namespace scene49u_words {']
    index = 0
    table = []
    for name, (a, z) in BLOCKS.items():
        chunk = data[name]
        words = struct.unpack('<%dI' % (len(chunk) // 4), chunk)
        (SOURCE / (name + '.bin')).write_bytes(chunk)
        (SOURCE / (name + '.txt')).write_text(''.join('%08x %08x\n' % (a + 4 * i, w) for i, w in enumerate(words)))
        h.append('inline constexpr std::uint32_t %s[]={%s};' % (name, ','.join('0x%08xu' % w for w in words)))
        table.append('{"%s",0x%xu,%s,%d,%d}' % (name, a, name, len(words), index))
        index += len(words)
        manifest['blocks'].append({'name': name, 'start': '%08x' % a,
            'end_exclusive': '%08x' % z, 'words': len(words), 'sha256': digest(chunk)})
    h += ['struct Block { const char* name; std::uint32_t start; const std::uint32_t* words; unsigned count,index; };',
          'inline constexpr Block blocks[]={%s};' % ','.join(table),
          'inline constexpr unsigned total_words=%d;' % index, '}']
    (SOURCE / 'scene_words49u.h').write_text('\n'.join(h) + '\n')
    decoder = SOURCE / 'scene_machine49u.h'
    manifest['decoder'] = {'path': str(decoder.relative_to(ROOT)), 'sha256': digest(decoder.read_bytes()),
        'origin': 'Retained research original-word EE/VU interpreter; only frozen-header namespace and relocatable EE-root include changed. No native calls or stubs.'}
    manifest['retained_provenance'] = [
        {'path': p, 'sha256': digest((ROOT / p).read_bytes())} for p in (
            'src/game/scene_graph49p.cpp', 'src/game/scene_graph49p.h',
            'src/game/body_scene48g.cpp', 'src/game/body_scene48g.h',
            'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h')]
    manifest['authored_scene'] = {'path': 'evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg',
        'sha256': '203232e291b5098ddfeb437d59748105f33a4432bad1a9359d96681fc5d03b59',
        'tested_cable_endpoint_indices': [4, 5]}
    manifest['scope'] = 'Nonnull distinct relative getters; both axes outputs; direction/inverse; relative.parent matrix conversion; complete local copy/dirty setter; Rodrigues rotation nonnull output; matrix quaternion. Serial fixed-memory transactional scratch; finite consumed arithmetic and surviving VU scratch operands used by self-subtraction zeroing (including VF12.W), FE_TONEAREST, strict binary32. Explicit VF9.W lane, finite for focused four-lane proof; arbitrary poisoned VU state is excluded.'
    (SOURCE / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')

def verify():
    data = extract()
    manifest = json.loads((SOURCE / 'manifest.json').read_text())
    assert manifest['elf_sha256'] == ELF_SHA
    header = (SOURCE / 'scene_words49u.h').read_text()
    decoded = {}
    for b in manifest['blocks']:
        name = b['name']; a = int(b['start'], 16)
        assert (a, int(b['end_exclusive'], 16)) == BLOCKS[name]
        assert digest(data[name]) == b['sha256']
        assert (SOURCE / (name + '.bin')).read_bytes() == data[name]
        match = re.search(r'\b' + name + r'\[\]=\{(.*?)\};', header, re.S)
        words = [int(x, 16) for x in re.findall(r'0x([a-f0-9]+)u', match[1])]
        assert len(words) == b['words']
        assert struct.pack('<%dI' % len(words), *words) == data[name]
        listed = [(int(line.split()[0], 16), int(line.split()[1], 16))
                  for line in (SOURCE / (name + '.txt')).read_text().splitlines()]
        assert listed == [(a + 4 * i, w) for i, w in enumerate(words)]
        decoded.update(listed)
    assert len(decoded) == sum(x['words'] for x in manifest['blocks'])
    assert digest((ROOT / manifest['decoder']['path']).read_bytes()) == manifest['decoder']['sha256']
    for p in [*manifest['retained_provenance'], manifest['authored_scene']]:
        assert digest((ROOT / p['path']).read_bytes()) == p['sha256'], p['path']
    return manifest, decoded

def execute(command, out, name, env=None, expected=0):
    r = subprocess.run([str(c) for c in command], cwd=ROOT, env=env, text=True, capture_output=True)
    (out / (name + '.stdout')).write_text(r.stdout)
    (out / (name + '.stderr')).write_text(r.stderr)
    assert r.returncode == expected, '%s failed:\n%s\n%s' % (name, r.stdout, r.stderr)
    return r

def account(path, decoded):
    report = json.loads(path.read_text())
    assert report['ordered_flag_tracing']
    assert {x['pc'] for x in report['pcs']} == set(decoded)
    unknown = []
    for row in report['pcs']:
        assert row['word'] == decoded[row['pc']]
        if row['hits']:
            continue
        pc = row['pc']
        if 0x28a72c <= pc < 0x28a760 or 0x28b1c4 <= pc < 0x28b1e0 or 0x28b574 <= pc < 0x28b59c:
            reason = 'Equal-node relative route excluded: admitted node and relative are distinct.'
        elif pc in (0x28a7a0, 0x28b220, 0x28ba18, 0x28bc18, 0x28bdb8, 0x28b5dc, 0x28981c, 0x2898e4, 0x13c024, 0x14fb70):
            reason = 'Unreachable duplicate setup after unconditional branch delay slot.'
        elif pc in (0x142bc4, 0x142bc8):
            reason = 'Null rotation output route excluded by reference output API.'
        else:
            reason = 'Unexercised original instruction; no universal coverage claim.'
            unknown.append('%08x' % pc)
        row['unreached_reason'] = reason
    assert not unknown, 'Unaccounted original words: ' + str(unknown)
    restrictions = {
        0x28a724: 'Distinct node and relative required.',
        0x28a7e4: 'Forward output is a nonnull reference.',
        0x28a804: 'Up output is a nonnull reference.',
        0x28a824: 'Nonnull relative required.',
        0x28a82c: 'Forward output is a nonnull reference.',
        0x28a84c: 'Up output is a nonnull reference.',
        0x28b1bc: 'Distinct node and relative required.',
        0x28b278: 'Nonnull relative required.',
        0x28b56c: 'Distinct node and relative required.',
        0x28b64c: 'Nonnull relative required.',
        0x142bbc: 'Rotation output is a nonnull reference.',
    }
    branches = both = 0
    for row in report['pcs']:
        if not row['hits']:
            continue
        word = row['word']; op = word >> 26; rs = word >> 21 & 31; rt = word >> 16 & 31
        if op not in (4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17) and not (op == 0x11 and rs == 8):
            assert not (row['taken'] or row['not_taken'])
            continue
        branches += 1
        assert row['taken'] + row['not_taken'] == row['hits']
        if row['taken'] and row['not_taken']:
            both += 1
        elif op == 4 and rs == rt:
            row['unreached_direction_reason'] = 'Unconditional source branch with identical registers.'
        else:
            assert row['pc'] in restrictions, 'Unaccounted branch direction %08x' % row['pc']
            row['unreached_direction_reason'] = restrictions[row['pc']]
    report['accounting'] = {'original_words': len(decoded), 'reached_words': sum(bool(x['hits']) for x in report['pcs']),
        'excluded_or_source_unreachable_words': sum(not x['hits'] for x in report['pcs']),
        'reached_branch_pcs': branches, 'both_directions': both, 'single_direction_with_specific_reason': branches - both,
        'unexercised_pcs': unknown, 'unknown_executed_pcs': 0, 'unaccounted_words_or_directions': 0}
    path.write_text(json.dumps(report, indent=2) + '\n')
    return report

def run(args, build):
    manifest, decoded = verify()
    build.mkdir(parents=True, exist_ok=True)
    out = args.output_dir.resolve() if args.output_dir else build
    out.mkdir(parents=True, exist_ok=True)
    flags = ['-std=c++20', '-O2', '-g', '-Wall', '-Wextra', '-Werror', '-pedantic', '-Wno-misleading-indentation', '-ffp-contract=off', '-fno-fast-math']
    if args.sanitizers:
        flags += ['-O1', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
    binary = build / 'chariot_scene49u'
    units = [ROOT / 'tests/native/chariot_scene49u.cpp', ROOT / 'src/game/scene_graph49p.cpp', ROOT / 'src/game/body_scene48g.cpp']
    ordinary_base = [os.environ.get('CXX', 'g++'), *flags, *units]
    base = [os.environ.get('CXX', 'g++'), *flags, '-include', ROOT / 'evidence/sceneProjection49P/source/flag_trace.h', *units]
    env = dict(os.environ, RR_CHARIOT_SCENE49U_COVERAGE=str(out / 'coverage.json'))
    if args.sanitizers:
        env.update(ASAN_OPTIONS='detect_leaks=0:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    ordinary_binary = build / 'chariot_scene49u_ordinary'
    execute([*ordinary_base, ROOT / 'src/game/chariot_scene49u.cpp', '-o', ordinary_binary], out, 'compile_ordinary')
    ordinary_env = dict(env, RR_CHARIOT_SCENE49U_COVERAGE=str(out / 'ordinary-coverage.json'))
    ordinary = execute([ordinary_binary], out, 'ordinary', ordinary_env)
    execute([*base, ROOT / 'src/game/chariot_scene49u.cpp', '-o', binary], out, 'compile')
    normal = execute([binary], out, 'native', env)
    coverage = account(out / 'coverage.json', decoded)
    controls = []
    mutations = [
        ('axes_swap_forward_up', '28a7ec', 'c6600110'),
        ('relative_position_final_add_to_subtract', '28bebc', '4bc74a6c'),
        ('direction_drop_fourth_lane_increment', '28ba80', '4bc04a68'),
        ('inverse_direction_drop_fourth_lane_increment', '28bd20', '4bc04a68'),
        ('relative_matrix_omit_parent_conversion', '28b674', '00000000'),
        ('copy_local_drop_final_pair', '28b404', '24040007'),
        ('rotation_final_add_to_subtract', '142c9c', '46003801'),
        ('matrix_quaternion_reverse_tie', '14faf8', '46000834'),
        ('sincos_replace_cosine_multiply_with_add', '13c0fc', '46002000'),
    ]
    for name, pc, word in mutations:
        r = execute([binary, pc, word], out, 'control_' + name, env, expected=1)
        assert 'mismatch' in r.stderr, name + ': ' + r.stderr
        controls.append({'name': name, 'kind': 'original-word substitution', 'pc': pc, 'word': word,
                         'detected': True, 'diagnostic': r.stderr.strip()})
    source = (ROOT / 'src/game/chariot_scene49u.cpp').read_text()
    axes_anchor = '''bool axes49U(const SceneGraph49P& g, I i, I relative, SceneVectorContext49U& c,
    V& forward, V& up) noexcept {
    if (!admit(g, i) || !index(g, relative) || i == relative) return false;
    if (!refresh(g, i, c)) return false;'''
    native_controls = [
        ('relative_world_instead_of_its_parent', 'const I parent = node(g, relative).parent180;', 'const I parent = relative;', 'mismatch'),
        ('refresh_relative_before_target_same_terminal_state', axes_anchor,
            axes_anchor.replace('    if (!refresh(g, i, c))', '    if (!refresh(g, relative, c)) return false;\n    if (!refresh(g, i, c))'), 'flag store order node mismatch'),
    ]
    for name, anchor, replacement, diagnostic in native_controls:
        assert source.count(anchor) == 1, name + ': source anchor changed'
        mutant = build / (name + '.cpp'); mutant.write_text(source.replace(anchor, replacement))
        executable = build / name
        execute([*base, '-I', ROOT / 'src/game', mutant, '-o', executable], out, 'compile_' + name)
        r = execute([executable], out, 'control_' + name, env, expected=1)
        assert diagnostic in r.stderr, name + ': ' + r.stderr
        controls.append({'name': name, 'kind': 'native semantic control', 'detected': True, 'diagnostic': r.stderr.strip()})
    result = {'status': 'pass', 'mode': 'ASan+UBSan' if args.sanitizers else 'normal',
        'elf_sha256': ELF_SHA, 'ordinary_result': ordinary.stdout.strip(), 'native_result': normal.stdout.strip(), 'source_word_ranges_verified': len(BLOCKS),
        'source_words_verified': len(decoded), 'accounting': coverage['accounting'],
        'sensitivity': controls, 'scope': manifest['scope'], 'command': [str(c) for c in base]}
    (out / 'native-test-result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))

def main():
    p = argparse.ArgumentParser()
    p.add_argument('--freeze', action='store_true')
    p.add_argument('--verify-only', action='store_true')
    p.add_argument('--sanitizers', action='store_true')
    p.add_argument('--build-dir', type=Path)
    p.add_argument('--output-dir', type=Path)
    args = p.parse_args()
    if args.freeze:
        freeze()
    if args.verify_only:
        manifest, words = verify()
        print(json.dumps({'status': 'pass', 'elf_sha256': ELF_SHA, 'source_words': len(words), 'blocks': len(manifest['blocks'])}))
    elif args.build_dir:
        run(args, args.build_dir.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='chariot-scene49u-') as d:
            run(args, Path(d))

if __name__ == '__main__':
    main()
