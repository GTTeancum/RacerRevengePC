#!/usr/bin/env python3
"""Authenticate49V secondary callback words and verify the native bit-copy effect.

Only the sealed original ELF is a source. Native implementation files are never
read while generating expected words. No game, RAM, constructor, scene refresh,
query/contact or original ownership/allocator execution is performed.
"""
import argparse
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]
DEST = ROOT / 'evidence/chariotLifecycle49V/secondary'
SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
BLOCKS = {
    'secondary_constructor_callsite': (0x212548, 0x21257c),
    'secondary_scene_binding': (0x2125b0, 0x2125c4),
    'rigid_constructor': (0x167920, 0x1679cc),
    'body_scene_setter': (0x1652e0, 0x1652f4),
    'rigid_vtable_type_descriptor': (0x3ce000, 0x3ce004),
    'rigid_vtable_A8_AC': (0x3ce0a8, 0x3ce0b0),
    'rigid_rtti': (0x3a77a0, 0x3a77b0),
    'rigid_type_name': (0x3a76f0, 0x3a7704),
    'secondary_slot_AC': (0x167a50, 0x167ac4),
    'secondary_slot_A8': (0x167ad0, 0x167b7c),
    'fixed_zero_helper': (0x306478, 0x306538),
    'quaternion_zero_constant': (0x382b60, 0x382b70),
    'vector_global_initializer': (0x3c9290, 0x3c9310),
    'reverse_transition': (0x21bec4, 0x21bf4c),
    'transition_final_tail': (0x21c0f4, 0x21c118),
}


def generate():
    elf = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    if hashlib.sha256(elf).hexdigest() != SHA:
        raise ValueError('Original ELF SHA256 mismatch')
    if elf[:6] != b'\x7fELF\x01\x01':
        raise ValueError('Expected little endian ELF32')
    offset = struct.unpack_from('<I', elf, 28)[0]
    size, count = struct.unpack_from('<HH', elf, 42)
    segments = [struct.unpack_from('<8I', elf, offset + i * size) for i in range(count)]

    def take(a, z):
        for kind, off, address, _, extent, *_ in segments:
            if kind == 1 and address <= a < z <= address + extent:
                return elf[off + a - address:off + z - address], off + a - address
        raise ValueError(f'Unmapped ELF source {a:#x}..{z:#x}')

    def word(a):
        return struct.unpack('<I', take(a, a + 4)[0])[0]

    # Authenticate the concrete class producer, installer, descriptor and both
    # virtual destinations. These are provenance-only words, not executed setup.
    assert word(0x21254c) == 0x0c059e48 and word(0x212554) == 0xaec2035c
    assert word(0x167938) == 0x2444e000 and word(0x167948) == 0xac440008
    assert word(0x3ce000) == 0x3a77a0 and word(0x3a77a0) == 0x3a76f0
    assert take(0x3a76f0, 0x3a7704)[0].rstrip(b'\0') == b'PhysicsRigidBody'
    assert word(0x3ce0a8) == 0x167ad0 and word(0x3ce0ac) == 0x167a50
    assert take(0x382b60, 0x382b70)[0] == bytes(16)
    spec = importlib.util.spec_from_file_location('decoder49m', ROOT / 'tools/gameplay/secondary_wrapper_recover49m.py')
    decoder = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(decoder)
    files, listing = {}, []
    header = ['#pragma once', '#include <cstdint>', '#include <cstddef>',
              '// Exact source words from authenticated ELF; no production formulas.',
              'namespace secondary49v_words {']
    manifest = {'elf_sha256': SHA, 'source': 'work/native/project/disc/SLUS_202.68',
                'blocks': {}, 'RAM_reads': 0, 'production_source_reads': 0,
                'scope': 'Concrete secondary callback plus exact immediate caller body-copy sequence. Constructor and scene setter are provenance only.',
                'external_boundaries': ['Caller execution begins at21BF08 after separately proved graph-local setter28B3E0.',
                                        'Caller execution stops at21C0F4; root-tail publication is covered by the separate lifecycle suite.',
                                        'Fixed zero helper executes original words for six12-byte fills; >=32-byte bulk route is not applicable.',
                                        'Initialized synthetic records and explicit source-global words, not race startup, game execution or EE/VU silicon equivalence.']}
    total = 0
    for name, (a, z) in BLOCKS.items():
        raw, off = take(a, z)
        words = struct.unpack('<' + 'I' * (len(raw) // 4), raw)
        files[name + '.bin'] = raw
        manifest['blocks'][name] = {'start': hex(a), 'end_exclusive': hex(z), 'elf_offset': hex(off),
                                    'word_count': len(words), 'index': total, 'sha256': hashlib.sha256(raw).hexdigest()}
        total += len(words)
        listing.append(f'# {name} [{a:#x},{z:#x})')
        header.extend([f'inline constexpr std::uint32_t {name}_start=0x{a:x}u;', f'inline constexpr std::uint32_t {name}[]={{'])
        for i, value in enumerate(words):
            instruction = decoder.decode(a + 4 * i, value)
            listing.append(f'{a + 4 * i:08x} {value:08x} {instruction}')
            header.append(f'  0x{value:08x}u, // {a + 4 * i:08x} {instruction}')
        header.append('};')
    header.extend(['struct Block { const char* name; std::uint32_t start; const std::uint32_t* words; std::size_t count, index; };',
                   'inline constexpr Block blocks[]={'])
    for name, data in manifest['blocks'].items():
        header.append(f' {{"{name}",{name}_start,{name},sizeof({name})/4,{data["index"]}}},')
    header.extend(['};', f'inline constexpr std::size_t total_words={total};', '}'])
    files['secondary_words49v.h'] = ('\n'.join(header) + '\n').encode()
    files['original-disassembly.txt'] = ('\n'.join(listing) + '\n').encode()
    files['source-manifest.json'] = (json.dumps(manifest, indent=2) + '\n').encode()
    return files


def verify_native():
    compiler = shutil.which('clang++') or shutil.which('g++')
    if not compiler:
        raise RuntimeError('No native C++ compiler available')
    out = DEST / 'verification'
    out.mkdir(parents=True, exist_ok=True)
    test = ROOT / 'tests/native/secondary_activation49v.cpp'
    production = ROOT / 'src/game/secondary_activation49v.cpp'
    common = [compiler, '-std=c++17', '-Wall', '-Wextra', '-Werror', '-pedantic', '-fno-fast-math',
              '-ffp-contract=off', '-I' + str(ROOT / 'src/game'), str(test)]
    records = []

    def command(name, args, expected=0):
        result = subprocess.run(args, cwd=ROOT, capture_output=True, text=True,
                                env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0:halt_on_error=1',
                                     'UBSAN_OPTIONS': 'halt_on_error=1'})
        (out / (name + '.stdout')).write_text(result.stdout)
        (out / (name + '.stderr')).write_text(result.stderr)
        records.append({'name': name, 'command': args, 'returncode': result.returncode,
                        'expected': expected, 'passed': result.returncode == 0 if expected == 0 else result.returncode != 0})
        if not records[-1]['passed']:
            raise RuntimeError(f'{name} unexpected result: {result.returncode}; see {out}')
        return result

    for label, flags in [('normal', ['-O2']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer'])]:
        binary = out / label
        command('build-' + label, common + flags + [str(production), '-o', str(binary)])
        command('test-' + label, [str(binary)])

    text = production.read_text()
    mutations = {
        'retain-old-speed': ('std::memset(&secondary.cachedSpeed34, 0, sizeof(secondary.cachedSpeed34));', '(void)secondary.cachedSpeed34;'),
        'quaternion-from-vector-global': ('std::memcpy(body.orientationDerivative.v, globals.quaternion382B60,', 'std::memcpy(body.orientationDerivative.v, globals.vector433B80,'),
        'omit-position-history': ('zeroVector(body.previousPositionDerivative);', '(void)body.previousPositionDerivative;'),
        'omit-linear-current-scratch': ('zeroVector(extra.linearAcceleration2D0);', '(void)extra.linearAcceleration2D0;'),
        'omit-angular-current-scratch': ('zeroVector(extra.angularAcceleration308);', '(void)extra.angularAcceleration308;'),
        'wrong-local-w-destination': ('root.localVelocity80, extra.localVelocity80W,', 'root.localVelocity80, extra.previousVelocity90W,'),
        'reset-unrelated-position': ('auto& body = root.body;', 'auto& body = root.body; zeroVector(body.position);'),
        'wrong-world-angular-source': ('main.body.angularVelocity.v, sizeof(main.body.angularVelocity.v)', 'main.worldAngularVelocityA0.v, sizeof(main.worldAngularVelocityA0.v)'),
        'discard-main-velocity-w': ('std::memcpy(&secondary.velocity70W, &main70W, sizeof(main70W));', '(void)main70W; std::memset(&secondary.velocity70W, 0, sizeof(main70W));'),
        'zero-quaternion-history': ('std::memcpy(body.previousOrientationDerivative.v, globals.quaternion382B60, sizeof(body.previousOrientationDerivative.v));', 'std::memset(body.previousOrientationDerivative.v, 0, sizeof(body.previousOrientationDerivative.v));'),
    }
    for name, (old, new) in mutations.items():
        assert text.count(old) == 1, name
        source = out / ('mutation-' + name + '.cpp')
        source.write_text(text.replace(old, new))
        binary = out / ('mutation-' + name)
        command('build-mutation-' + name, common + ['-O2', str(source), '-o', str(binary)])
        command('test-mutation-' + name, [str(binary)], expected=1)
    summary = {'passed': True, 'normal_and_asan_ubsan': True, 'native_mutations_detected': len(mutations),
               'leak_sanitizer': 'Disabled because the executor uses ptrace; native operator-new allocation counts remain checked.',
               'compiler': subprocess.check_output([compiler, '--version'], text=True).splitlines()[0],
               'records': records}
    (out / 'verification.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps({'passed': True, 'native_mutations_detected': len(mutations), 'verification': str(out)}))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    parser.add_argument('--verify-native', action='store_true')
    args = parser.parse_args()
    files = generate()
    for name, data in files.items():
        path = DEST / 'source' / name
        if args.check:
            if path.read_bytes() != data:
                raise ValueError('Frozen source mismatch: ' + str(path))
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists() and path.read_bytes() != data:
                raise ValueError('Refusing to replace different frozen source: ' + str(path))
            path.write_bytes(data)
    print(json.dumps({'source_verified': True, 'files': len(files), 'elf_sha256': SHA}))
    if args.verify_native:
        verify_native()


if __name__ == '__main__':
    main()
