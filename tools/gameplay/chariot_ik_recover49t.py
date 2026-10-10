#!/usr/bin/env python3
"""Authenticate and reproduce bounded49T constructor/IK source differential proof.

Default artifacts live only in a temporary directory. Persist evidence only with
--output-dir. The standalone C++ test emits coverage only when an explicit
RR_CHARIOT_IK49T_COVERAGE path is supplied. Never reads runtime RAM or installs.
"""
import argparse
import hashlib
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path
sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'evidence/chariotAnimation49T/ik/source'
FIXTURES = SOURCE.parent / 'fixtures'
PRODUCTION = ['src/game/chariot_ik49t.cpp', 'src/game/source_acos49t.cpp', 'src/game/scene_graph49p.cpp']
TEST = ROOT / 'tests/native/chariot_ik49t.cpp'
FLAGS = ['-ffp-contract=off', '-fno-fast-math', '-Wall', '-Wextra', '-Werror']
MUTATIONS = [
    ('lower_clamp_omitted', 0x28f894, 0),
    ('acos_domain_errno_store_omitted', 0x3027dc, 0),
    ('projected_cosine_add_instead_of_multiply', 0x28f880, 0x46020b00),
    ('rest_angle_subtracted', 0x28f8c0, 0x46010501),
    ('target_direction_normalization_omitted', 0x28f9c8, 0),
    ('shoulder_sqrt_replaced_with_move', 0x28fa68, 0x46000206),
    ('target_orientation_up_reads_forward', 0x290a5c, 0x27a70040),
    ('constructor_reciprocal_replaced_with_product', 0x28f29c, 0x46002002),
]
def sha(data):
    return hashlib.sha256(data).hexdigest()

def branch_kind(word):
    """Every MIPS scalar conditional branch family, including likely and links."""
    op, rs, rt = word >> 26, word >> 21 & 31, word >> 16 & 31
    if op in (4, 5, 6, 7):
        return 'integer'
    if op in (0x14, 0x15, 0x16, 0x17):
        return 'integer_likely'
    if op == 1 and rt in (0, 1, 2, 3, 16, 17, 18, 19):
        return 'regimm_likely' if rt & 2 else 'regimm'
    if op == 0x11 and rs == 8:
        return 'cop1_likely' if rt & 2 else 'cop1'
    return None

def verify():
    raw = (ROOT / 'work/native/project/disc/SLUS_202.68').read_bytes()
    manifest = json.loads((SOURCE / 'source-manifest.json').read_text())
    assert sha(raw) == manifest['elf_sha256'], 'ELF authentication'
    po = struct.unpack_from('<I', raw, 28)[0]
    ps, pn = struct.unpack_from('<HH', raw, 42)
    segments = [struct.unpack_from('<8I', raw, po+i*ps) for i in range(pn)]
    def take(a, z):
        for k, offset, va, _, length, *_ in segments:
            if k == 1 and va <= a < z <= va + length:
                return raw[offset+a-va:offset+z-va]
        raise AssertionError(f'non-file-backed {a:x}..{z:x}')
    header = (SOURCE / 'ik_words.h').read_text()
    data_header = (SOURCE / 'ik_data.h').read_text()
    words, expected_data, total = {}, [], 0
    for block in manifest['blocks']:
        a, z = int(block['start'],16), int(block['end_exclusive'],16)
        data = take(a,z)
        assert sha(data) == block['sha256']
        assert data == (SOURCE / (block['name']+'.bin')).read_bytes()
        match = re.search(r'\b'+block['name']+r'\[\]=\{(.*?)\};',header,re.S)
        assert match, block['name']
        values = [int(x,16) for x in re.findall(r'0x([0-9a-f]+)u',match[1])]
        assert len(values) == block['words'] and block['index'] == total
        assert struct.pack('<%dI'%len(values),*values) == data
        listing = [tuple(int(x,16) for x in row.split()[:2]) for row in (SOURCE/(block['name']+'.txt')).read_text().splitlines()]
        assert listing == [(a+4*i,x) for i,x in enumerate(values)]
        for pc, word in listing:
            assert pc not in words, f'overlapping freeze {pc:08x}'
            words[pc] = word
        total += len(values)
    assert total == manifest['word_count']
    for block in manifest['data']:
        a, z = int(block['start'],16), int(block['end_exclusive'],16)
        data = take(a,z)
        assert sha(data) == block['sha256'] and data == (SOURCE/(block['name']+'.bin')).read_bytes()
        expected_data.extend((a+i*4,x) for i,x in enumerate(struct.unpack('<%dI'%(len(data)//4),data)))
    emitted_data = [(int(a,16),int(w,16)) for a,w in re.findall(r'\{0x([0-9a-f]+)u,0x([0-9a-f]+)u\}',data_header)]
    assert emitted_data == expected_data
    contract = json.loads((SOURCE/'initialization-contract.json').read_text())
    assert contract['elf_sha256'] == manifest['elf_sha256'] and contract['original_gp'] == '003dd970'
    provenance = contract['provenance_only']
    a,z = int(provenance['start'],16),int(provenance['end_exclusive'],16)
    data = take(a,z)
    assert sha(data) == provenance['sha256'] and data == (SOURCE/(provenance['name']+'.bin')).read_bytes()
    listing = [tuple(int(x,16) for x in row.split()[:2]) for row in (SOURCE/(provenance['name']+'.txt')).read_text().splitlines()]
    assert listing == [(a+i*4,w) for i,w in enumerate(struct.unpack('<%dI'%(len(data)//4),data))]
    assert struct.unpack_from('<I',data,0x100034-a)[0] == 0x3c04003e
    assert struct.unpack_from('<I',data,0x100048-a)[0] == 0x2484d970
    assert struct.unpack_from('<I',data,0x10005c-a)[0] == 0x0080e025
    lo,hi = (int(x,16) for x in contract['startup_zero_range_half_open'])
    for first,last in contract['explicit_fixture_cold_bss']:
        assert lo <= int(first,16) < int(last,16) <= hi
    for name,digest in contract['retained_files'].items():
        assert sha((ROOT/name).read_bytes()) == digest, name
    fixture = json.loads((FIXTURES/'fixture-manifest.json').read_text())
    for name, digest in fixture['files'].items():
        assert sha((FIXTURES/name).read_bytes()) == digest, name
    assert sha((ROOT/fixture['asset_source_manifest']).read_bytes()) == fixture['asset_source_manifest_sha256']
    container=fixture['asset_container'];container_bytes=(ROOT/container['path']).read_bytes()
    assert len(container_bytes)==container['bytes'] and sha(container_bytes)==container['sha256']
    for name, digest in fixture['retained_assets'].items():
        assert sha((ROOT/name).read_bytes()) == digest, name
    binary = (FIXTURES/'pre-ik-samples.bin').read_bytes()
    metadata = json.loads((FIXTURES/'pre-ik-samples.json').read_text())
    assert struct.unpack_from('<4I',binary) == tuple(fixture['header_words'])
    assert len(binary) == 16 + 625*1168 and len(metadata['cases']) == 625
    for i, case in enumerate(metadata['cases']):
        assert case['id'] == i and case['offset'] == 16+i*1168
        params = struct.unpack_from('<4f',binary,case['offset'])
        assert tuple(case['parameters']) == params
        assert binary[case['offset']+16:case['offset']+1168] == (FIXTURES/'sampled-locals'/f'{i:03d}.bin').read_bytes()
    for _, pc, replacement in MUTATIONS:
        assert pc in words and replacement != words[pc]
    # Guard against accidentally dropping likely/REGIMM/COP1 accounting.
    for op in (4,5,6,7,0x14,0x15,0x16,0x17):
        assert branch_kind(op<<26)
    for rt in (0,1,2,3,16,17,18,19):
        assert branch_kind(1<<26|rt<<16)
    for rt in range(4):
        assert branch_kind(0x11<<26|8<<21|rt<<16)
    assert branch_kind(0) is None and branch_kind(3<<26) is None
    return manifest, words

def exclusion(pc, block):
    if block == 'solver':
        for a,z,reason in [
            (0x28f5c4,0x28f5d4,'Source record mode10=1 excludes the alternate28FFD0 call and its bypass return.'),
            (0x28f620,0x28f7e4,'Source record debug40=0 skips all profiling registration/timer work before the solve.'),
            (0x28fa8c,0x28ff00,'Source record debug40=0 skips post-solve profiling/render/MMIO work.')]:
            if a <= pc < z:
                return 'excluded_by_admission', reason
        return 'unexercised_solver_path', 'Preserved original instruction not reached by the explicit admitted fixtures; no universal reachability claim.'
    if block == 'constructor':
        if pc in (0x28ee2c,0x28f014,0x28f1a8,0x28f20c,0x28f258,0x28f26c,0x28f2e4):
            return 'source_unreachable_duplicate', 'Duplicate setup after an unconditional branch and its delay slot; all incoming source branches skip this duplicate word.'
        for a,z,reason in [
            (0x28ec94,0x28ec9c,'Zero distal-segment squared length branch at28EC8C'),
            (0x28ed1c,0x28ed24,'Zero proximal-segment squared length branch at28ED14'),
            (0x28edd4,0x28eddc,'Zero distal-segment squared length branch at28EDCC'),
            (0x28ee24,0x28ee2c,'Zero proximal-segment squared length branch at28EE1C'),
            (0x28f00c,0x28f014,'Zero first pole-projected segment squared length branch at28F004'),
            (0x28f1a0,0x28f1a8,'Zero second pole-projected segment squared length branch at28F198'),
            (0x28f2d4,0x28f2e4,'Zero first projected direction fallback through443410 at28F2CC'),
            (0x28f3d8,0x28f3e8,'Zero second projected direction fallback through443410 at28F3D0')]:
            if a<=pc<z:
                return 'unexercised_constructor_degeneracy', reason+' is not reached by the authored pre-motion scene and tested rigid parents. The full source route remains frozen; no degenerate-input equivalence is claimed.'
        raise AssertionError(f'unclassified constructor word {pc:08x}')
    if block == 'wrapper':
        return 'unexercised_wrapper_path', 'Complete straight-line wrapper retained; any unvisited word is disclosed rather than counted as exercised.'
    if block in ('pole_init','fallback_zero_init'):
        return 'fixture_initialization_provenance', 'Original initializer executes while constructing the source fixture, before the differential counters are cleared. This byte is retained as explicit cold-state provenance, not counted as a production-constructor/runtime instruction.'
    return 'unexercised_helper_path', 'Complete selected original helper retained; alternate lazy/cache/parent or numeric branches beyond supplied fixtures are not claimed exercised.'

def coverage(path, manifest, words):
    rows = {}
    for line in path.read_text().splitlines():
        pc,h,t,n = line.split()
        address = int(pc,16)
        assert address not in rows, f'duplicate PC {pc}'
        rows[address] = (int(h),int(t),int(n))
    assert set(rows) == set(words), 'coverage must include every frozen word exactly once'
    counts, blocks, branch_counts = {}, [], {}
    for block in manifest['blocks']:
        out=[]
        for pc in range(int(block['start'],16),int(block['end_exclusive'],16),4):
            h,t,n = rows[pc];kind=branch_kind(words[pc])
            assert min(h,t,n) >= 0
            assert (t+n == h) if kind else (t == 0 and n == 0), f'branch accounting {pc:08x}'
            row=dict(pc=f'{pc:08x}',word=f'{words[pc]:08x}',hits=h,taken=t,not_taken=n)
            if kind:
                row['branch_kind']=kind
                branch_counts[kind]=branch_counts.get(kind,0)+1
                row['branch_directions']='both' if t and n else 'taken_only' if t else 'not_taken_only' if n else 'unexecuted'
                if h and not(t and n):
                    op,rs,rt=words[pc]>>26,words[pc]>>21&31,words[pc]>>16&31
                    reasons={0x28f5bc:'Source record mode10=1 excludes alternate28FFD0.',0x28f618:'Source record debug40=0 skips pre-solve profiling.',0x28fa84:'Source record debug40=0 skips post-solve profiling.'}
                    row['single_direction_reason']=reasons.get(pc,'Unconditional BEQ with identical registers.' if op==4 and rs==rt else 'Only this direction occurred in the declared fixtures; the other direction is retained and not claimed exercised.')
            if not h:
                cls,reason=exclusion(pc,block['name']);row.update(coverage_class=cls,reason=reason);counts[cls]=counts.get(cls,0)+1
            out.append(row)
        blocks.append(dict(name=block['name'],pcs=out))
    return dict(frozen_words=len(words),executed_unique_words=sum(h>0 for h,t,n in rows.values()),dynamic_instructions=sum(h for h,t,n in rows.values()),unknown_executed_words=0,all_conditional_branch_opcodes_accounted=True,branch_families=branch_counts,uncovered_classes=counts,blocks=blocks)

def dependency_files():
    files=set([Path(__file__).resolve(),TEST,*[ROOT/f for f in PRODUCTION]])
    files.update(p for directory in (SOURCE,FIXTURES) for p in directory.rglob('*') if p.is_file())
    fixture=json.loads((FIXTURES/'fixture-manifest.json').read_text())
    files.update(ROOT/p for p in fixture['retained_assets'])
    files.add(ROOT/fixture['asset_source_manifest']);files.add(ROOT/fixture['asset_container']['path'])
    contract=json.loads((SOURCE/'initialization-contract.json').read_text())
    files.update(ROOT/p for p in contract['retained_files'])
    todo=list(files)
    while todo:
        path=todo.pop()
        if path.suffix not in ('.h','.cpp','.hpp'):continue
        for name in re.findall(r'^\s*#\s*include\s*"([^"]+)"',path.read_text(),re.M):
            include=(path.parent/name).resolve()
            assert include.is_relative_to(ROOT), f'nonportable local include {include}'
            assert include.is_file(), f'missing include {include}'
            if include not in files:files.add(include);todo.append(include)
    return sorted(files)

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--output-dir',type=Path)
    parser.add_argument('--build-dir',type=Path)
    parser.add_argument('--verify-only',action='store_true')
    parser.add_argument('--skip-sanitized',action='store_true')
    parser.add_argument('--skip-mutations',action='store_true')
    args=parser.parse_args();manifest,words=verify()
    if args.verify_only:
        print(json.dumps(dict(status='authenticated',source_words=len(words),fixtures=625)));return
    dependencies=dependency_files()
    before={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in dependencies}
    with tempfile.TemporaryDirectory(prefix='chariot-ik49t-') as temporary:
        build=args.build_dir.resolve() if args.build_dir else Path(temporary)/'build'
        out=args.output_dir.resolve() if args.output_dir else Path(temporary)/'evidence'
        build.mkdir(parents=True,exist_ok=True);out.mkdir(parents=True,exist_ok=True)
        def save(name,value):
            (out/name).write_text(json.dumps(value,indent=2)+'\n')
        def run(command,name,env=None,expect=True,cwd=ROOT):
            result=subprocess.run([str(x) for x in command],cwd=cwd,env=env,text=True,capture_output=True)
            (out/(name+'.stdout')).write_text(result.stdout);(out/(name+'.stderr')).write_text(result.stderr)
            if expect:assert result.returncode==0,f'{name}: {result.stdout}\n{result.stderr}'
            return result
        cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version'],'compiler').stdout.splitlines()[0]
        def compile_set(sanitized=False,traced=False):
            tag='san' if sanitized else 'trace' if traced else 'normal'
            flags=FLAGS+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie'] if sanitized else ['-O2','-g'])
            if traced:flags += ['-DRR_CHARIOT_IK49T_TRACE','-include',SOURCE/'flag_trace.h']
            objects=[]
            for file in PRODUCTION:
                obj=build/(Path(file).stem+'-'+tag+'.o')
                run([cxx,'-std=c++17',*flags,'-c',ROOT/file,'-o',obj],'build-'+Path(file).stem+'-'+tag)
                objects.append(obj)
            exe=build/('chariot_ik49t-'+tag)
            run([cxx,'-std=c++20',*flags,*(['-no-pie'] if sanitized else []),TEST,*objects,'-o',exe],'link-'+tag)
            return exe
        exe=compile_set()
        clean=dict(os.environ);clean.pop('RR_CHARIOT_IK49T_COVERAGE',None)
        traced=dict(clean,RR_CHARIOT_IK49T_COVERAGE=str(out/'coverage.tsv'))
        normal=json.loads(run([exe,ROOT],'normal',traced).stdout)
        assert normal['all_match'] and normal['native_allocations']==0
        assert normal['runtime_cases']>=625 and normal['initializers']>=1
        save('normal.json',normal);save('coverage-accounting.json',coverage(out/'coverage.tsv',manifest,words))
        coverage_hash=sha((out/'coverage.tsv').read_bytes())
        readonly_dir=Path(temporary)/'read-only-cwd';readonly_dir.mkdir()
        readonly=json.loads(run([exe,ROOT],'default-read-only',clean,cwd=readonly_dir).stdout)
        assert not list(readonly_dir.iterdir()), 'default standalone test emitted files'
        assert readonly==normal and sha((out/'coverage.tsv').read_bytes())==coverage_hash
        save('read-only-default.json',dict(default_test_emits_no_files=True,explicit_coverage_unchanged=True,coverage_opt_in='RR_CHARIOT_IK49T_COVERAGE',runner_default_output='temporary directory only'))
        traced_exe=compile_set(traced=True)
        instrumented=json.loads(run([traced_exe,ROOT],'ordered-flag-stores',clean).stdout)
        assert instrumented['all_match'] and instrumented['native_allocations']==0
        for key in ('runtime_cases','initializers','original_instructions','domain_error_calls','lower_clamps'):
            assert instrumented[key]==normal[key], key
        save('ordered-flag-stores.json',instrumented)
        mutants=[]
        if not args.skip_mutations:
            for name,pc,replacement in MUTATIONS:
                result=run([exe,ROOT,f'{pc:x}',f'{replacement:x}'],'mutation-'+name,clean,False)
                assert result.returncode!=0 and 'mismatch' in result.stderr.lower(), (name,result.stdout,result.stderr)
                mutants.append(dict(name=name,pc=f'{pc:08x}',original=f'{words[pc]:08x}',replacement=f'{replacement:08x}',actual_mismatch=result.stderr.strip()))
        save('mutation-controls.json',dict(mutations=mutants,actual_mismatches=len(mutants),setup_failures_counted=0,working_tree_production_mutated=False))
        sanitized=None
        if not args.skip_sanitized:
            exe=compile_set(True)
            env=dict(clean,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
            sanitized=json.loads(run([exe,ROOT],'asan-ubsan',env).stdout)
            assert sanitized==normal;save('sanitized.json',sanitized)
        after={str(p.relative_to(ROOT)):sha(p.read_bytes()) for p in dependencies}
        assert before==after, 'source dependencies changed during verification'
        output_hashes={p.name:sha(p.read_bytes()) for p in sorted(out.iterdir()) if p.is_file() and p.name!='validation.json'}
        validation=dict(status='passed',scope='Original28EB90 constructor and type1/debug0 fixed-pole290A00 pair; supplied authored pre-motion and sampled scenes.',compiler=compiler,production_language='C++17',test_language='C++20',flags=FLAGS,normal=normal,instrumented=instrumented,instrumented_flags=['-DRR_CHARIOT_IK49T_TRACE','-include','evidence/chariotAnimation49T/ik/source/flag_trace.h'],sanitized=sanitized,leak_sanitizer='disabled, not claimed',mutations=len(mutants),source_elf_sha256=manifest['elf_sha256'],files=after,outputs=output_hashes)
        save('validation.json',validation)
        print(json.dumps(dict(status='passed',normal=normal,sanitized=sanitized is not None,mutations=len(mutants),output=str(out) if args.output_dir else 'temporary only')))
if __name__ == '__main__':
    main()
