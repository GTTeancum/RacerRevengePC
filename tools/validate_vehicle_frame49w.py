#!/usr/bin/env python3
"""Read-only fail-closed49W release validation. Only explicit --seal writes a seal.
No compiler, native executable, renderer, game, RAM, install, historical capacity
or ownership operation runs. Full capture bytes, all baseline members and all new
release files are preserved; negative controls change decoded copies only.
"""
import argparse
import copy
import datetime
import hashlib
import importlib.util
import json
import math
from pathlib import Path, PurePosixPath
import re
import struct
import statistics
import subprocess
import sys
import zipfile
sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
E = ROOT / 'evidence/frameBridge49W'
BASELINE_SHA = 'ca384717125bf27e123c47f3611bf59fe2d8658bf0d79c0b0eaee2384a3f08f5'
BASELINE_BYTES = 253188705
BASELINE_MEMBERS = 28664
BASELINE_MANIFEST = 'CLOUD-MILESTONE-49V-CHARIOTLIFECYCLE-MANIFEST.json'
ALLOWED = {'RacerRevengePC/README.md', 'RacerRevengePC/CMakeLists.txt'}
ELF_SHA = 'c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
EMPTY_SHA = hashlib.sha256(b'').hexdigest()
NEW_UNITS = {'src/game/graph_body49w.cpp', 'src/game/pod_shape49w.cpp', 'src/assets/pod_shape49w.cpp', 'src/game/vehicle_frame49w.cpp', 'src/assets/import_collision48g.cpp', 'src/assets/import_decode48c.cpp'}
NEW_SUITES = {'graph_body49w', 'pod_shape49w', 'vehicle_frame49w', 'vehicle_frame_export49w', 'vehicle_frame_no_support49w'}
BAD_FLAGS = {'-Ofast', '-ffast-math', '-fno-rounding-math', '-ffp-contract=fast', '-fsanitize=leak'}
BODY = {'passed':True,'cases':551,'checks':1850130,'rejections':21,'leaf_cases':384,'stage_comparisons':3653,'ordered_flag_stores':5672,'original_instructions':952237,'rotation_calls':276,'scalar_zero_skips':4,'native_allocations':0}
SHAPE = {'atomic_rejections':98,'checks':5446,'differential_cases':364,'failures':0,'immutable_source_bytes':88,'malformed_authored_records':1782,'native_allocations':0,'native_state_bytes':220,'passed':True,'persistent_transitions':126,'scalar_sqrt_reference_cases':30}
FRAME = {'passed':True,'frames':81,'substeps':132,'graphics_comparisons':81,'cache_comparisons':164,'tail_comparisons':132,'shape_original_comparisons':132,'body_comparisons':242,'force_comparisons':132,'secondary_comparisons':110,'input_comparisons':82,'reverse_cases':1,'persistent_cases':5,'rejections':28,'trace_checks':1,'native_calls':1271,'native_allocations':0,'checks':60440}
SUMMARY = {'body_cache_original_comparisons':288,'body_original_comparisons':403,'checks':101679,'frames':144,'graph_nodes':53,'graphics_original_comparisons':144,'input_original_comparisons':144,'main_force_original_comparisons':240,'mode_transitions':[{'after':[0,1],'before':[1,0],'frame':22},{'after':[1,1],'before':[0,1],'frame':68},{'after':[1,0],'before':[1,1],'frame':129}],'native_calls':2227,'native_hot_allocations':0,'native_postphysics_graphics_pose_copies':0,'native_vertices_all_frames':828720,'orientation_state_changed_frames':116,'palette_entries':46,'passed':True,'position_changed_frames':144,'secondary_original_comparisons':163,'shape_original_comparisons':240,'substeps':240,'tail_original_comparisons':240}
UNSUPPORTED = dict(SUMMARY, body_original_comparisons=260, checks=76506, mode_transitions=[{'after':[0,1],'before':[1,0],'frame':12}], native_calls=2084, orientation_state_changed_frames=143, secondary_original_comparisons=20)
EXPECTED_MUTATIONS = {'body/validation': [('original_word', 'previous_full_vector_copy_omitted'), ('original_word', 'position_derivative_x_stale'), ('original_word', 'linear_current_derivative_x_stale'), ('original_word', 'angular_current_derivative_x_stale'), ('original_word', 'angular_acceleration_W_increment_omitted'), ('original_word', 'point_W_producer_omitted'), ('original_word', 'local_velocity_full_copy_omitted'), ('original_word', 'world_angular_full_copy_omitted'), ('original_word', 'force_full_clear_omitted'), ('original_word', 'accumulator1C0_clear_omitted'), ('original_word', 'rotation_correction_clear_omitted'), ('original_word', 'callback238_clear_omitted'), ('original_word', 'axis_translation_restore_omitted'), ('isolated_native_source', 'setter_before_first_point'), ('isolated_native_source', 'eager_world_equals_local'), ('isolated_native_source', 'retain_old_quaternion_translation'), ('isolated_native_source', 'missing_observer_rejection'), ('isolated_native_source', 'scalar_zero_rotation_not_skipped'), ('isolated_native_source', 'previous90W_forced_one'), ('isolated_native_source', 'position_derivative_not_continued'), ('isolated_native_source', 'angular_D0W_forced_one'), ('isolated_native_source', 'supplement_reset_on_skip'), ('isolated_native_source', 'only_root_graph_commit'), ('isolated_native_source', 'blanket_descendant_invalidation')], 'shape/verification': [('original_word', 'omit_transform_call'), ('original_word', 'history_reads_composed'), ('original_word', 'current_written_over_previous'), ('original_word', 'compose_uses_current_as_local'), ('original_word', 'force_local_W_to_one'), ('original_word', 'center_reads_half_x'), ('original_word', 'radius_omits_z_square'), ('original_word', 'center_W_zero'), ('original_word', 'center_omits_translation'), ('isolated_native_change', 'history_after_current'), ('isolated_native_change', 'incoming_world_ignored'), ('isolated_native_change', 'local_ignored'), ('isolated_native_change', 'omit_W_term'), ('isolated_native_change', 'center_uses_composed'), ('isolated_native_change', 'radius_scaled'), ('isolated_native_change', 'radius_omits_z'), ('isolated_native_change', 'center_W_zero'), ('isolated_native_change', 'fused_composition'), ('isolated_native_change', 'identity_shortcut'), ('isolated_native_change', 'accept_nonnull_E0'), ('isolated_native_change', 'accept_other_type'), ('isolated_native_change', 'rounding_guard_omitted'), ('isolated_native_change', 'nonfinite_publishes'), ('isolated_native_change', 'scalar_root_subnormal_not_flushed')], 'frame/final-verification': [('isolated_native_semantic', 'secondary_uses_duplicate_gravity'), ('isolated_native_semantic', 'fresh_speed_before_graphics'), ('isolated_native_semantic', 'reset_impact_before_graphics'), ('isolated_native_semantic', 'omit_postgraphics_impact_reset'), ('isolated_native_semantic', 'lose_secondary_derivative_buffer'), ('isolated_native_semantic', 'main_clear20c_omitted'), ('isolated_native_semantic', 'query_uses_previous_position'), ('isolated_native_semantic', 'query_forces_w'), ('isolated_native_semantic', 'feedback_omitted'), ('isolated_native_semantic', 'late_shape_omitted'), ('isolated_native_semantic', 'secondary_double_gravity'), ('isolated_native_semantic', 'secondary_early_refresh'), ('isolated_native_semantic', 'reader_uses_duplicate_source')]}
class GateError(ValueError): pass

def require(value, message):
    if not value: raise GateError(message)

def digest(data): return hashlib.sha256(data).hexdigest()
def sha(path):
    h=hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda:f.read(1048576),b''): h.update(chunk)
    return h.hexdigest()
def load(name): return json.loads((E/name).read_text())
def equal(actual, expected, label):
    require(type(actual) is type(expected) and actual == expected, label)
def fields(actual, expected, label):
    for k,v in expected.items(): require(k in actual and type(actual[k]) is type(v) and actual[k]==v,label+': '+k)
def safe_names(names):
    require(len(names)==len(set(names)), 'Duplicate paths')
    for n in names:
        require(isinstance(n,str) and n and not PurePosixPath(n).is_absolute() and not any(c in n for c in ('\\',':','\x00')) and not any(p in ('','.','..') for p in n.split('/')), 'Unsafe path: '+repr(n))
def current_path(n):
    require(isinstance(n,str), 'Non-string path')
    if Path(n).is_absolute():
        require(n.count('/RacerRevengePC/')==1,'Absolute pin lacks unique project root')
        n=n.split('/RacerRevengePC/',1)[1]
    safe_names([n]);p=ROOT/n
    require(p.is_file() and not p.is_symlink() and p.resolve().is_relative_to(ROOT.resolve()),'Missing or unsafe file: '+n)
    return p

def pins(rows, prefix=''):
    require(isinstance(rows,dict) and rows,'Missing file pins')
    safe_names(list(rows))
    for n,v in rows.items():
        r=v if isinstance(v,dict) else {'sha256':v};p=current_path(prefix+n)
        require(sha(p)==r['sha256'],'File pin: '+prefix+n)
        if 'bytes' in r: require(p.stat().st_size==r['bytes'],'File size: '+prefix+n)
def module(name, relative):
    spec=importlib.util.spec_from_file_location(name,ROOT/relative);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);return m

def relocated(value):
    """Compare recorded project paths by relative identity after ZIP extraction."""
    if isinstance(value,dict):return {k:relocated(v)for k,v in value.items()}
    if isinstance(value,list):return [relocated(v)for v in value]
    if isinstance(value,str) and value.startswith('/'):
        if '/RacerRevengePC/' in value:return '<project>/'+value.split('/RacerRevengePC/',1)[1]
        if value.endswith('/RacerRevengePC'):return '<project>'
    return value

def blocked_process(*args, **kwargs): raise GateError('Read-only validator attempted an external process')

def strict_flags(command, sanitized, standard=None, traced=None):
    require(isinstance(command,list) and not BAD_FLAGS.intersection(command),'Unsafe compiler flags')
    require('-fno-fast-math' in command and '-ffp-contract=off' in command,'Missing strict FP flags')
    if standard: require(standard in command,'Wrong C++ language standard')
    require(('-fsanitize=address,undefined' in command)==sanitized,'Wrong sanitizer flags')
    if traced is False: require('-include' not in command and not any('ENABLE_STAGE_TRACE' in s for s in command),'Unexpected production trace')

def baseline_gate(path):
    require(path.is_file() and not path.is_symlink() and path.stat().st_size==BASELINE_BYTES and sha(path)==BASELINE_SHA,'Expected exact verified49V baseline ZIP')
    changed=[]
    with zipfile.ZipFile(path) as archive:
        names=[r.filename for r in archive.infolist() if not r.is_dir()];safe_names(names)
        require(len(names)==BASELINE_MEMBERS and BASELINE_MANIFEST in names,'Baseline membership/manifest')
        require(archive.testzip() is None,'Baseline CRC failure')
        for n in names:
            p=ROOT.parent/n
            require(p.is_file() and not p.is_symlink() and p.resolve().is_relative_to(ROOT.parent.resolve()),'Missing baseline member: '+n)
            if sha(p)!=digest(archive.read(n)): changed.append(n)
    require(set(changed)==ALLOWED,'Unexpected baseline changes: '+repr(changed))
    for p in ROOT.parent.rglob('*'):
        require(not p.is_symlink(),'Archive symlink: '+str(p))
        if p.is_file() and not p.is_relative_to(ROOT):require(p.relative_to(ROOT.parent).as_posix() in names,'New file outside sealed project tree')
    return names,sorted(changed)

def seal_hashes(names):
    names=set(names);out={}
    for p in sorted(ROOT.rglob('*')):
        require(not p.is_symlink(),'Symlink cannot be sealed')
        if not p.is_file() or p==E/'checkpoint-validation.json':continue
        n=p.relative_to(ROOT).as_posix();member=ROOT.name+'/'+n
        if member not in names or member in ALLOWED:
            safe_names([n]);require('__pycache__' not in p.parts and p.suffix not in ('.o','.pyc') and not p.name.endswith('.zip'),'Temporary build/archive file in release: '+n)
            with p.open('rb')as stream:require(stream.read(4)!=b'\x7fELF','Generated host ELF executable in release: '+n)
            out[n]=sha(p)
    return out

def mutation_gate(report, count, label):
    fields(report,{'count':count,'setup_failures_counted':0,'production_edited':False},label)
    rows=report['detected'];equal([(r['kind'],r['name'])for r in rows],EXPECTED_MUTATIONS[label],label+' exact semantic mutation membership');require(len(rows)==count and len({(r['kind'],r['name']) for r in rows})==count,label+' membership')
    for row in rows:
        require(row['kind'] in ('original_word','native_semantic','isolated_native_semantic','native_source','isolated_native_source','isolated_native_change'),label+' mutation kind')
        require(any(isinstance(row.get(k),str) and row[k] for k in ('diagnostic','mismatch','witness')),label+' missing semantic witness')
        if 'diagnostic' in row:require('FAIL ' in row['diagnostic'] and not any(s in row['diagnostic'] for s in ('AddressSanitizer','Segmentation fault','runtime error:')),label+' crash masquerading as semantic rejection')

def focused_gates(visual):
    auth=visual.authenticate();fields(auth,{'elf_sha256':ELF_SHA,'fresh_caller_secondary_words':4413,'body_source_words':1931,'shape_original_words':155,'shape_source_words':155,'source_call_substitutions':0,'shape_decoder_matches_retained_body_decoder':True},'Source authentication')
    equal(load('connected/verification/source-authentication.json'),auth,'Frozen source authentication');pins(auth['retained_files'])
    body=load('body/validation/validation.json');fields(body,{'passed':True,'elf_sha256':ELF_SHA,'native_call_target_substitutions':0,'RAM_reads':0,'old_files_edited':False,'mutations':24,'production_language':'C++17','test_language':'C++20'},'Body proof');pins(body['hashes'])
    shape=load('shape/verification/summary.json');fields(shape,{'passed':True,'differential_cases':364,'source_words':155,'original_instructions':93184,'source_coverage':144,'mutation_count':24,'sanitized':True},'Shape proof');equal(shape['native'],SHAPE,'Shape exact counts');pins(load('shape/verification/proof-inputs.json'))
    frame=load('frame/final-verification/validation.json');fields(frame,{'passed':True,'production_standard':'C++17','test_standard':'C++20','mutants':13,'source_substitutions':0},'Frame proof');pins(frame['files']);equal(frame['source'],visual.FRAME.authenticate(),'Frame source provenance')
    for mode in ('normal','plain','sanitized'):
        b=dict(BODY,checks=403616,stage_comparisons=0,ordered_flag_stores=0) if mode=='plain' else BODY
        f=dict(FRAME,checks=59665,trace_checks=0) if mode=='plain' else FRAME
        for directory,expected,summary in [('body/validation',b,body),('shape/verification',SHAPE,None),('frame/final-verification',f,frame)]:
            equal(load(directory+'/'+mode+'.json'),expected,directory+' '+mode+' counts')
            if summary is not None:equal(summary[mode],expected,directory+' embedded result')
            logmode=('asan-ubsan' if mode=='sanitized' else mode) if directory!='shape/verification' else mode+'-run'
            equal(json.loads((E/directory/(logmode+'.stdout')).read_text()),expected,directory+' raw stdout')
            require(not (E/directory/(logmode+'.stderr')).read_bytes(),directory+' runtime stderr')
    for prefix,count in [('body/validation',24),('shape/verification',24),('frame/final-verification',13)]:mutation_gate(load(prefix+'/mutations.json'),count,prefix)
    bm=visual.module('release_body49w','tools/gameplay/graph_body_recover49w.py');manifest,words=bm.authenticate()
    equal(load('body/validation/coverage-accounting.json'),bm.account(E/'body/validation/coverage.tsv',manifest,words),'Body independent coverage accounting')
    require(words[0x168364]==0x03e00008 and 0x168368 in words,'Body full return/delay')
    coverage=load('shape/verification/source-coverage.json');fields(coverage,{'compiled_words':155,'reached_words':144,'excluded_nonnull_registration_words':11,'unaccounted_words':0},'Shape full source accounting')
    require(len(coverage['rows'])==155 and len({r['pc']for r in coverage['rows']})==155,'Shape coverage membership')
    require(sum(r['cases_reached']>0 for r in coverage['rows'])==144,'Shape executed coverage')
    shape_module=visual.module('release_shape49w','tools/gameplay/pod_shape_recover49w.py');_,shape_words,_,_=shape_module.authenticate()
    equal({int(r['pc'],16):int(r['word'],16)for r in coverage['rows']},shape_words,'Every shape coverage word authenticated')
    commands=load('shape/verification/commands.json');require(len(commands)==68 and len({r['name'] for r in commands})==68,'Shape command membership')
    for row in commands:
        name=row['name'];mutant=name.endswith('-run') and name not in ('normal-run','plain-run','sanitized-run')
        require(row['returncode']==(1 if mutant else 0),'Shape process failure: '+name)
        if '-build-' in name or name.endswith('-build'):strict_flags(row['argv'],name.startswith('sanitized-'),'-std=c++20' if name.endswith('-test') else '-std=c++17')
    adapter=load('connected/shape-adapter-validation.json');fields(adapter,{'passed':True,'cases':364,'all_projected_words_match':True,'source_word_count':155,'source_call_substitutions':0},'Connected shape original adapter');pins(adapter['sha256'])
    return {'body_full_returns':551,'body_leaf_cases':384,'body_mutants':24,'shape_cases':364,'shape_EE_boundary_cases':30,'shape_mutants':24,'frame_cases':81,'frame_substeps':132,'frame_full_shape_calls':132,'frame_mutants':13,'source_words':{'body':1931,'caller_secondary':4413,'shape':155}}

def f32(w):return struct.unpack('<f',struct.pack('<I',w))[0]
def trace_gate(trace, unsupported=False):
    expected=UNSUPPORTED if unsupported else SUMMARY
    equal(trace['summary'],expected,'Exact connected summary')
    equal(trace['scenario'],'no-support' if unsupported else 'synthetic-support','Scenario identity')
    require(trace['schema']=='advancing49w.trace.v1' and len(trace['frames'])==144,'Complete connected trace')
    previous={k:v for k,v in trace['initial'].items() if k in trace['frames'][0]['before']};transitions=[]
    for i,row in enumerate(trace['frames']):
        before,g,a=row['before'],row['after_graphics_before_physics'],row['after']
        require(row['frame']==i and before==previous,'Single canonical owner continuity')
        for s in (before,g,a):
            require(len(s['graph'])==53 and [n['index']for n in s['graph']]==list(range(53)),'Graph node identity')
            require(s['physical838']==s['lifecycle']['vehicle838'],'Single physical mode owner')
            require(s['shape']['type']==s['shape']['registration']==0,'Admitted type0 E0-null shape')
            require(s['main_scene_flags']&255==s['graph'][50]['flags190']&255 and s['secondary_scene_flags']&255==s['graph'][49]['flags190']&255,'Canonical physical cache flags')
            require(s['main_scene_flags']&0xffffff00==2801795072 and s['secondary_scene_flags']&0xffffff00==2876556544,'Physical cache high bytes preserved')
            require(s['main_w_lanes']==[2147483648,1061158912,1065353216,1065353216],'Main supplied W lanes preserved')
        require(before['phase']==a['phase']==0 and g['main_phase']==g['body_phase']==3 and a['main_phase']==a['body_phase']==a['secondary_phase']==0,'Graphics/body transaction phase')
        require(a['phase864']==g['phase864'] and a['lifecycle']==g['lifecycle'],'Graphics phase/lifecycle carried into physics')
        require(a['shape']['current']==a['graph'][50]['world_bits'],'Late shape uses final main world')
        for root in (0,15,30):require(g['graph'][root]['local_bits']==a['graph'][root]['local_bits'],'Body advance overwrote graphics roots')
        require(a['substep_index']==a['substeps']==len(row['supplied354_support_forces']) and a['substeps'] in (1,2),'Completed declared substeps')
        for j,loadrow in enumerate(row['supplied354_support_forces']):
            require(loadrow['frame']==i and loadrow['substep']==j,'Supplied force order')
            require((f32(loadrow['main_support_y_bits'])==0) if unsupported else (f32(loadrow['main_support_y_bits'])>0),'Explicit support/no-support forces')
            if not loadrow['physical838'] or unsupported:require(loadrow['secondary_support_y_bits']==0,'Inactive/no-support secondary force')
        old=[before['lifecycle'][k]for k in ('vehicle838','vehicle83C')];new=[a['lifecycle'][k]for k in ('vehicle838','vehicle83C')]
        if old!=new:transitions.append({'before':old,'after':new,'frame':i})
        previous=a
    equal(transitions,expected['mode_transitions'],'Computed mode transitions')
    if not unsupported:
        row=trace['frames'][68];b,g,a=row['before'],row['after_graphics_before_physics'],row['after']
        require(g['graph'][49]['world_bits']==b['graph'][49]['world_bits'] and g['graph'][49]['world_bits']!=g['graph'][49]['local_bits'],'Reverse returns stale physical cache')
        require(g['graph'][49]['flags190']&1==0 and a['graph'][49]['flags190']&1==1,'Later physics getter refreshes dirty cache')


def connected_gates(visual):
    freeze=load('connected/freeze.json');verification=load('connected/verification/verification.json')
    fields(freeze,{'passed':True,'all_native_read_only_paths_verified':True,'normal_plain_sanitized_bytes_identical':True,'verification_path':'evidence/frameBridge49W/connected/verification/verification.json'},'Frozen capture')
    require(freeze['verification_sha256']==sha(E/'connected/verification/verification.json'),'Frozen verification pin')
    fields(verification,{'passed':True,'all_mode_capture_bytes_identical':True,'production_standard':'C++17','fixture_standard':'C++20','leak_sanitizer_used':False,'frozen_read_only_requested':True},'Connected verification')
    equal(verification['authentication'],visual.authenticate(),'Connected source authentication')
    require(set(verification['runs'])=={'normal','plain','asan-ubsan'},'Connected mode membership')
    for k in ('input_sha256','source_sha256'):
        equal(verification[k],freeze[k],'Freeze exact source/input membership');equal(load('connected/verification/'+k.replace('_','-')+'.json'),freeze[k],'Retained pin file');pins(freeze[k])
    required={'src/game/graph_body49w.cpp','src/game/vehicle_frame49w.cpp','src/game/pod_shape49w.cpp','src/assets/pod_shape49w.cpp','tools/visual/export_vehicle_frame49w.cpp','tools/visual/run_vehicle_frame49w.py','evidence/frameBridge49W/connected/shape_original49w.h','work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h'}
    require(required<=set(freeze['source_sha256']),'Frozen critical source membership')
    results=[]
    for scenario,unsupported,expected,freeze_key in [('connected',False,SUMMARY,'support_native_sha256'),('connected/no-support',True,UNSUPPORTED,'no_support_native_sha256')]:
        directory=E/scenario;trace=load(scenario+'/trace.json');trace_gate(trace,unsupported)
        fresh=visual.validate(directory,unsupported);equal(fresh,load(scenario+'/reconstruction.json'),'Independent full semantic reconstruction');equal(fresh,load(scenario+'/export-report.json'),'Published independent export')
        equal(fresh['native_capture_sha256'],freeze[freeze_key],'Exact frozen capture membership');pins(freeze[freeze_key],'evidence/frameBridge49W/'+scenario+'/')
        expected_names={'trace.json','exact-goldens49w.txt','export-report.native.json'}|{f'frame{x}-full-motion.rrmesh'for x in(0,31,95,143)}
        if not unsupported:expected_names|={'frame31-frozen-roots.rrmesh','frame31-physical-copy-only.rrmesh'}
        require(set(freeze[freeze_key])==expected_names,'Complete retained capture files')
        for mode,row in verification['runs'].items():
            prefix='no_support_' if unsupported else ''
            equal(row[prefix+'capture'],freeze[freeze_key],'Mode capture pins');equal(row[prefix+'frozen_sha256_before_after'],freeze[freeze_key],'Mode read-only pin equality');equal(row[prefix+'summary'],expected,'Mode exact source comparisons')
            require(row['read_only_native_cli'] is True and row['independent_reconstructed_positions']==828720 and row['link_libraries']==['z'],'Mode source/consumer evidence')
            strict_flags(row['flags'],mode=='asan-ubsan',traced=False if mode=='plain' else None)
            prefix_path='connected/verification/'+mode+('/no-support' if unsupported else '')+'/'
            pins(freeze[freeze_key],'evidence/frameBridge49W/'+prefix_path)
            equal(load(prefix_path+'reconstruction.json'),fresh,'Mode reconstruction evidence')
            for action in (('no-support-capture','no-support-readonly')if unsupported else('capture','noargs-readonly')):
                stem='connected/verification/'+mode+'-'+action
                equal(json.loads((E/(stem+'.stdout.log')).read_text()),expected,'Raw connected process result')
                require(not(E/(stem+'.stderr.log')).read_bytes(),'Connected runtime stderr')
        results.append(fresh)
    comparison=visual.compare_scenarios(E/'connected',E/'connected/no-support');equal(load('connected/support-comparison.json'),comparison,'Computed support control comparison')
    delta=comparison['frames'][-1]['supported_main_position'][1]-comparison['frames'][-1]['unsupported_main_position'][1]
    require(delta==626.2255859375,'No-support fallthrough control displacement')
    for mode in verification['runs']:equal(load('connected/verification/'+mode+'/support-comparison.json'),comparison,'Mode support comparison')
    return {'support_frames':144,'no_support_frames':144,'source_compared_substeps_each':240,'independent_vertices_per_mode':1657440,'mode_meshes':10,'modes':['normal','plain','asan-ubsan'],'support_mode_changes':[22,68,129],'no_support_mode_changes':[12],'unsupported_final_y_difference':delta}


def expected_runner():
    sys.path.insert(0,str(ROOT/'tools'))
    m=module('release_runner49w','tools/test_native_vehicle_frame49w.py');r=m.runner
    old=json.loads((ROOT/'evidence/chariotLifecycle49V/final-normal/regression-report.json').read_text())
    require(set(r.SOURCES)=={b['source']for b in old['build']}|NEW_UNITS and len(r.SOURCES)==62,'Direct runner production membership')
    require(set(r.TESTS)==set(old['selected_tests'])|NEW_SUITES and len(r.TESTS)==69,'Direct runner suite membership')
    paths=r.source_inputs(ROOT,list(r.SOURCES)+[s['source']for s in r.TESTS.values()])
    paths.update(ROOT/s['fixture']for s in r.TESTS.values()if s['fixture']);paths.update(ROOT/e['fixture']for s in r.TESTS.values()for e in s.get('extra_fixtures',[]))
    paths.update(ROOT/p for p in ('tools/test_native_spring48k.py','CMakeLists.txt','cmake/NativeSpring48K.cmake','third_party/native-json/json.hpp'));paths.update(ROOT/p for p in r.EXTRA_INPUT_PATHS)
    return r,{p.relative_to(ROOT).as_posix()for p in paths},old

def build_graph_gate(r):
    text=(ROOT/'cmake/NativeVehicleFrame49W.cmake').read_text();top=(ROOT/'CMakeLists.txt').read_text()
    library=re.search(r'add_library\(racer_native_vehicle_frame STATIC(.*?)\)',text,re.S);suite=re.search(r'set\(rr_vehicle_frame_tests(.*?)\)',text,re.S)
    require(library and suite,'Missing CMake frame target registration')
    equal(set(library[1].split()),set(r.SOURCES),'CMake/direct production membership');equal(set(suite[1].split()),set(r.TESTS),'CMake/direct suite membership')
    require(top.index('if(RR_NATIVE_VEHICLE_FRAME_ONLY)')<top.index('if(RR_NATIVE_CHARIOT_LIFECYCLE_ONLY)'),'Frame CMake early dispatch')
    require(re.search(r'if\(RR_NATIVE_VEHICLE_FRAME_ONLY\)\s*include\(cmake/NativeVehicleFrame49W.cmake\)\s*return\(\)\s*endif\(\)',top),'CMake frame dispatch isolation')
    require('COMMAND racer_${rr_test}_check --verify-no-support' in text and r.TESTS['vehicle_frame_no_support49w']['literal_arguments']==['--verify-no-support'],'No-support native_check dispatch')
    for path in r.TESTS['pod_shape49w']['arguments']:require('${CMAKE_CURRENT_SOURCE_DIR}/'+path in text,'CMake shape test fixture arguments')
    require('add_custom_command' not in text and 'execute_process' not in text and '-include' not in text,'Unexpected frame build execution or tracing')
    require('find_package(ZLIB REQUIRED)' in text and 'cxx_std_17' in text and 'cxx_std_20' in text and '-fno-fast-math -frounding-math -ffp-contract=off' in text,'CMake strict language/link contract')
    return {'production_units':62,'suites':69,'cmake_and_direct_sets_exact':True,'no_support_cli_exact':True,'cmake_executed':False}

def report_gate(report,mode,r,inputs,old):
    sanitized=mode=='sanitized';flags=list(r.STRICT_FLAGS)+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all'] if sanitized else ['-O2'])
    prod=[x for x in flags if not x.startswith('-std=')]+['-std=c++17']
    require(type(report['stack_usage_requested']) is bool,'Stack request type')
    if report['stack_usage_requested']:flags+=['-fstack-usage'];prod+=['-fstack-usage']
    fields(report,{'status':'passed','errors':[],'changed_inputs':[],'scope':'all_vehicle-frame_checks','serial':True,'sanitizers':sanitized,'leak_detection':False,'compile_flags':flags,'production_compile_flags':prod,'link_flags':(['-fsanitize=address,undefined']if sanitized else[])+['-lz']},'Aggregate mode')
    require(len(report['tests'])==len(report['selected_tests'])==69 and {t['name']for t in report['tests']}==set(report['selected_tests'])==set(r.TESTS),'Aggregate complete suites')
    require(len(report['build'])==62 and {b['source']for b in report['build']}==set(r.SOURCES),'Aggregate production units')
    equal(set(report['input_hashes']),inputs,'Aggregate exact input membership')
    for n,v in old['input_hashes'].items():
        if n!='CMakeLists.txt':equal(report['input_hashes'][n],v,'Inherited aggregate source: '+n)
    if sanitized:require('detect_leaks=0' in report['environment']['ASAN_OPTIONS'] and 'halt_on_error=1' in report['environment']['UBSAN_OPTIONS'],'Aggregate sanitizer environment')
    objects=set()
    for b in report['build']:
        strict_flags(b['command'],sanitized,'-std=c++17',False);require(b['command'][1:1+len(prod)]==prod,'Production exact flags')
        require(b['status']=='passed' and b['returncode']==0 and '-c' in b['command'],'Production compile success')
        obj=b['command'][b['command'].index('-o')+1];require(obj==b['object']['path'],'Object identity');objects.add(obj)
        equal(relocated(b['command'][b['command'].index('-c')+1]),relocated(str(ROOT/b['source'])),'Production command source identity')
        equal(relocated(b['cwd']),'<project>','Production working directory')
        require(b['stdout']['bytes']==0 and b['stdout']['sha256']==EMPTY_SHA,'Production compile stdout')
        if b['source'] in NEW_UNITS and b['source']!='src/game/vehicle_frame49w.cpp' and b['source'].endswith('49w.cpp'):require(b['stderr']['bytes']==0 and b['stderr']['sha256']==EMPTY_SHA,'Unexpected new production warnings')
    require(len(objects)==62,'Unique ordinary production objects')
    for t in report['tests']:
        b=t['build'];run=t['run'];strict_flags(b['command'],sanitized,'-std=c++20',False)
        require(b['command'][1:1+len(flags)]==flags and objects<=set(b['command']),'Test links all ordinary production objects')
        require(t['source']==r.TESTS[t['name']]['source'] and relocated(str(ROOT/t['source']))in relocated(b['command']),'Test command source identity')
        require(b['command'][b['command'].index('-o')+1]==t['executable']['path'],'Test build executable identity')
        equal(relocated(run['cwd']),'<project>','Runtime working directory')
        require(t['status']==b['status']==run['status']=='passed' and b['returncode']==run['returncode']==0,'Test build/runtime success')
        require(run['command'][0]==t['executable']['path'] and run['stderr']['bytes']==0 and run['stderr']['sha256']==EMPTY_SHA,'Runtime executable/stderr')
        spec=r.TESTS[t['name']];expected_args=spec['literal_arguments']if'literal_arguments'in spec else[str(ROOT/p)for p in spec['arguments']]if'arguments'in spec else[str(ROOT)]
        equal(relocated(run['command'][1:]),relocated(expected_args),'Exact aggregate test invocation: '+t['name'])
    for row in report.get('stack_usage',[]):require(digest(row['text'].encode())==row['sha256'] and len(row['text'].encode())==row['bytes'],'Stack evidence bytes')

def check_count(text):
    try:return int(json.loads(text)['checks'])
    except (ValueError,KeyError):
        m=re.search(r'checks=(\d+)',text) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',text)
        require(m is not None,'Unknown test output count');return int(m[1])

def regression_gates(r,inputs,old):
    summary=load('regression-summary.json');fields(summary,{'status':'passed','all_mode_outputs_identical':True,'semantic_trace_identical_to_focused':True,'connected_replay_exact_bytes':True,'all_64_inherited_suite_outputs_identical':True,'production_units_each':62,'trace_checks':{'support':{'aggregate':101679,'focused':101679},'no-support':{'aggregate':76506,'focused':76506}},'aggregate_store_tracing':'off; separately required and verified in focused normal/sanitized source proof'},'Aggregate summary')
    require(set(summary['runs'])=={'normal','sanitized'},'Aggregate mode membership')
    oldsummary=json.loads((ROOT/'evidence/chariotLifecycle49V/regression-summary.json').read_text())['runs']['normal']['tests'];reports={}
    for mode in ('normal','sanitized'):
        report=load('final-'+mode+'/regression-report.json');reports[mode]=report;report_gate(report,mode,r,inputs,old);pins(report['input_hashes']);row=summary['runs'][mode]
        fields(row,{'suites':69,'inputs_unchanged':True,'tracked_inputs':len(inputs)},'Aggregate retained summary');require(row['report_sha256']==sha(E/('final-'+mode)/'regression-report.json'),'Aggregate report pin')
        require([t for t in row['tests']if t['name']not in NEW_SUITES]==oldsummary,'All64 inherited summaries')
        total=0
        for t in row['tests']:
            data=(E/('final-'+mode)/(t['name']+'.stdout')).read_bytes();run=next(v['run']for v in report['tests']if v['name']==t['name'])
            require(digest(data)==t['stdout_sha256']==run['stdout']['sha256'] and len(data)==run['stdout']['bytes'] and t['stderr_bytes']==0 and check_count(data.decode())==t['checks'],'Raw aggregate output')
            if t['name'] not in NEW_SUITES:equal(data,(ROOT/'evidence/chariotLifecycle49V/final-normal'/(t['name']+'.stdout')).read_bytes(),'Inherited stdout exact')
            else:require(t['checks']=={'graph_body49w':403616,'pod_shape49w':5446,'vehicle_frame49w':59665,'vehicle_frame_export49w':101679,'vehicle_frame_no_support49w':76506}[t['name']],'New aggregate plain counts')
            total+=t['checks']
        require(row['checks']==summary['checks_each']==total,'Aggregate check sum')
        names=[]
        for p in report['build']+[t[k]for t in report['tests']for k in('build','run')]:
            for stream in ('stdout','stderr'):
                meta=p[stream];name=Path(meta['path']).name;safe_names([name]);file=E/('final-'+mode)/'logs'/name
                require(sha(file)==meta['sha256'] and file.stat().st_size==meta['bytes'] and row['process_logs'][name]=={'bytes':meta['bytes'],'sha256':meta['sha256']},'Durable process log');names.append(name)
        require(len(names)==len(set(names))==400 and set(names)==set(row['process_logs']),'Complete process log membership')
        framebuild=next(b for b in report['build']if b['source']=='src/game/vehicle_frame49w.cpp');warnings=(E/('final-'+mode)/'logs'/Path(framebuild['stderr']['path']).name).read_text();require(re.findall(r'warning:.*',warnings)==["warning: this 'if' clause does not guard... [-Wmisleading-indentation]"]*7,'Exactly seven retained compact-style frame warnings, no new warning class')
        expected={}
        for scenario,base in [('support','connected'),('no-support','connected/no-support')]:
            for n,h in load(base+'/reconstruction.json')['native_capture_sha256'].items():expected[scenario+'/'+n]=h
        equal(row['replayed_files'],expected,'Complete both-scenario aggregate recapture')
        for n,h in expected.items():require(sha(E/('final-'+mode)/n)==h,'Aggregate capture exact bytes')
        for test,count in [('vehicle_frame_export49w',SUMMARY),('vehicle_frame_no_support49w',UNSUPPORTED)]:equal(json.loads((E/('final-'+mode)/(test+'-output.stdout')).read_text()),count,'Aggregate capture output')
    equal(summary['runs']['normal']['tests'],summary['runs']['sanitized']['tests'],'Aggregate outputs identical');equal(reports['normal']['input_hashes'],reports['sanitized']['input_hashes'],'Aggregate input sets identical')
    return summary


def independent_gates():
    review=load('independent-review/verification.json');require(review['status']=='pass' and review['strict_cxx17'] is True,'Independent review')
    expected={'cache_numeric_cases':4096,'scheduler_cases':16,'bb0_w_independence_cases':256,'source_mutations_detected':6,'copy_only_payloads_preserved':True,'source_order_trace_checked':True}
    equal(review['normal'],expected,'Independent numeric/order counts');equal(review['asan_ubsan'],expected,'Independent sanitizer counts')
    equal(review['selectors'],{'original_selector_cases':864},'Independent selector source counts')
    secondary={'secondary_original_numeric_cases':64,'dirty_cache_cases':32,'ready_stale_local_cases':32,'early_damping_before_getter':True,'original_joint_calls':True,'source_mutations_detected':3}
    equal(review['secondary_original'],secondary,'Independent dirty secondary source');equal(review['secondary_asan_ubsan'],secondary,'Independent dirty secondary sanitizer');pins(review['artifacts'],'evidence/frameBridge49W/independent-review/')
    fields(review['independent_tiny_shape_check'],{'accepted':True,'squared_bits':71362,'actual_radius_bits':0,'original_radius_bits':0},'Independent subnormal shape boundary')
    final=load('independent-review/final-frame-review.json');fields(final,{'status':'pass','final_build_current_files_match':True,'mismatched_final_files':[],'changed_retained_text_files':[],'mutations':13,'source_substitutions':0},'Independent frame final')
    require(final['final_frame_validation_sha256']==sha(E/'frame/final-verification/validation.json'),'Independent frame validation pin')
    equal(final['normal'],FRAME,'Independent frame result');equal(final['sanitized'],FRAME,'Independent sanitized frame result');equal(final['plain'],dict(FRAME,checks=59665,trace_checks=0),'Independent plain frame result')
    return {'numeric_cache_cases_each':4096,'scheduler_cases_each':16,'bb0_w_cases_each':256,'source_mutations':9,'secondary_cases_each':64,'selector_cases':864,'full_frame_source_execution_claimed':False}


RESOURCE_KEYS=('gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','relocated_readonly_data_bytes','mutable_data_bytes','data_bytes','bss_bytes')

def footprint_gate(f):
    fields(f,{'status':'prepared','production_sources_unchanged':True,'disjoint_runtime_payload_bytes':122904,'actual_mutable_Running_allocation_bytes':85680,'state_inter_member_padding_bytes':0},'Native footprint')
    records=f['record_bytes'];fields(records,{'VehicleFrameState49W':12248,'VehicleFrameWorkspace49W':73388,'VehicleFrameDiagnostic49W':12,'PodShapeSource49W':88,'PodShapeState49W':220,'b::Immutable':12144,'b::Expected':24548,'b::Running':85680,'MotionView49O':1576,'FrameTimingState49I':28,'SceneNode49P':144},'Exact host records')
    expected={'canonical_state':records['VehicleFrameState49W'],'transaction_workspace':records['VehicleFrameWorkspace49W'],'diagnostic':records['VehicleFrameDiagnostic49W'],'native_timing_state':records['FrameTimingState49I'],'sampled_timestamp':4,'immutable_descriptors_MOT_rows_IK':records['b::Immutable'],'six_native_MOT_views':6*records['MotionView49O'],'immutable_MOT_byte_payload':15624}
    equal(f['disjoint_runtime_records'],expected,'Disjoint runtime storage');require(sum(expected.values())==f['disjoint_runtime_payload_bytes'],'Disjoint payload sum')
    state={'nodes53':53*records['SceneNode49P']}
    for name in ('CoupledBodyState49M','SecondaryActivationSupplement49V','GraphBodyExtraLanes49W','VehicleAnimationState49T','VehicleAnimationShared49T','VehicleVisualState49R','VisualShared49R','ChariotLifecycleState49V','ChariotLifecycleShared49V','CoupledNoiseContext49M','PodShapeState49W','VehicleFrameFields49W','VehicleFramePhase49W'):state[name]=records[name]*(2 if name in('SecondaryActivationSupplement49V','GraphBodyExtraLanes49W')else 1)
    equal(f['state_composition_included_not_added'],state,'Canonical state nested records');require(sum(state.values())+f['state_inter_member_padding_bytes']==records['VehicleFrameState49W'],'Canonical state composition sum')
    workspace={n:records[n]for n in('VehicleFrameState49W','VehicleAnimationWorkspace49T','VehicleVisualWorkspace49R','ChariotLifecycleWorkspace49V','GraphBodyWorkspace49W')}
    equal(f['workspace_composition_included_not_added'],workspace,'Workspace nested records');require(sum(workspace.values())==records['VehicleFrameWorkspace49W'],'Workspace nested sum')
    objects=f['objects'];linked=f['linked_native_production_units'];new=f['new49W_units']
    equal(set(new),{'src/game/vehicle_frame49w.cpp','src/game/graph_body49w.cpp','src/game/pod_shape49w.cpp','src/assets/pod_shape49w.cpp'},'New49W footprint units')
    require(len(linked)==len(set(linked))==30 and set(linked).isdisjoint(f['cold_only_production_units']) and set(linked)|set(f['cold_only_production_units'])==set(objects),'Native/cold dependency partition')
    require('src/assets/pod_shape49w.cpp' not in linked and set(new)-{'src/assets/pod_shape49w.cpp'}<=set(linked),'Cold decoder excluded from hot closure')
    for name,row in objects.items():
        sections=row['sections'];require(sha(current_path(name))==row['source_sha256'],'Footprint production source pin')
        derived={'executable_text_bytes':sum(v for k,v in sections.items()if k=='.text' or k.startswith('.text.')),'readonly_data_bytes':sum(v for k,v in sections.items()if k=='.rodata' or k.startswith('.rodata.')),'relocated_readonly_data_bytes':sum(v for k,v in sections.items()if k.startswith('.data.rel.ro')),'mutable_data_bytes':sum(v for k,v in sections.items()if(k=='.data' or k.startswith('.data.'))and not k.startswith('.data.rel.ro')),'bss_bytes':sum(v for k,v in sections.items()if k=='.bss' or k.startswith('.bss.'))}
        fields(row,derived,'Object section arithmetic');require(row['data_bytes']==row['mutable_data_bytes']+row['relocated_readonly_data_bytes'],'Object data categories')
        if name in linked:require(row['direct_allocator_references']==[],'Hot native direct allocator reference')
    for key,names in [('new49W_object_totals',new),('new49W_runtime_object_totals',[n for n in new if n!='src/assets/pod_shape49w.cpp']),('native_linked_object_totals',linked)]:equal(f[key],{k:sum(objects[n][k]for n in names)for k in RESOURCE_KEYS},'Object category totals')
    chains=f['selected_additive_compiler_frame_estimates'];require(len(chains)==3 and[sum(m['bytes']for m in c['members'])for c in chains]==[7560,8344,6728]and[c['sum_compiler_frame_bytes']for c in chains]==[7560,8344,6728],'Selected nested compiler frame sums')
    fields(f['recursive_scene_invalidation'],{'capacity_frame_product_bytes':2048,'current_graph_nodes':53,'individual_frame_bytes':32,'node_capacity_bound':64},'Recursive invalidation compiler estimate')
    require(f['maximum_native_individual_compiler_frame_bytes']==max(r['bytes']for r in f['production_stack_frames']if r['unit']in linked),'Compiler frame maximum')
    require(f['fixture_validation']['passed'] is True and f['fixture_validation']['frames_per_scenario']==144 and f['fixture_validation']['source_authored_compared_words']==415,'Authentic history fixture')
    require(len(f['preflight'])==4 and {(r['scenario'],r['allocation_counter_enabled'])for r in f['preflight']}=={(s,v)for s in('synthetic-support','no-support')for v in(0,1)},'Four ordinary preflight/probe results')
    for row in f['preflight']:
        scenario=row['scenario'];require(scenario in ('synthetic-support','no-support'),'Preflight scenario');i=0 if scenario=='synthetic-support'else 1
        final=f['fixture_validation']['scenarios'][i]['final_state_hash'];probe=row['allocation_counter_enabled']==1
        fields(row,{'cpp_new_allocations':0,'nanoseconds':0,'preflight_frames':288,'preflight_phase_snapshots':864,'retained_motion_bytes':15624,'validated_substeps':240,'validated_physical_substeps':163 if i==0 else 20,'validated_transitions':3 if i==0 else 1,'histories':1 if probe else 0,'frames':144 if probe else 0,'checksum':final if probe else 0,'final_state':final},'Untimed complete native history probe')

def benchmark_gate(b,f):
    fields(b,{'status':'measured','address_space_limit_bytes':67108864,'cpu_seconds_limit':30,'wall_timeout_seconds':40},'Bounded native timing');require(type(b['samples_per_scenario'])is int and b['samples_per_scenario']>=15,'Sufficient timing samples')
    require(len(b['cpu_affinity'])==1 and b['cpu_affinity'][0]in b['available_cpu_affinity'],'Timing affinity');equal(b['source_hashes'],f['source_hashes'],'Timing input set')
    scenarios=('synthetic-support','no-support');require(set(b['histories_per_sample'])==set(scenarios) and len(b['samples'])==2*b['samples_per_scenario'],'Timing scenario/sample membership')
    require(len(b['calibrations'])==len(b['untimed_allocation_probes'])==2,'Calibration/probe scenario membership')
    for i,scenario in enumerate(scenarios):
        histories=b['histories_per_sample'][scenario];require(type(histories)is int and 2<=histories<=1000,'Bounded complete histories')
        selected=[r for r in b['samples']if r['scenario']==scenario];require([r['sample']for r in selected]==list(range(b['samples_per_scenario'])),'All samples retained')
        final=f['fixture_validation']['scenarios'][i]['final_state_hash'];probe=b['untimed_allocation_probes'][i];cal=b['calibrations'][i]
        for row,enabled in [(cal,0),(probe,1)]+[(r,0)for r in selected]:
            h=2 if row is cal else histories
            fields(row,{'scenario':scenario,'histories':h,'frames':144*h,'final_state':final,'checksum':final*h%2**64,'cpp_new_allocations':0,'allocation_counter_enabled':enabled,'retained_motion_bytes':15624,'preflight_frames':288,'preflight_phase_snapshots':864,'validated_substeps':240,'validated_physical_substeps':163 if i==0 else 20,'validated_transitions':3 if i==0 else 1},'Persistent timed/probe counters')
            require(type(row['nanoseconds'])is int and (row['nanoseconds']==0 if enabled else row['nanoseconds']>0),'Separate timing/allocation executable')
            if 'sample' in row:require(row['nanoseconds_per_frame']==row['nanoseconds']/row['frames'],'Timing denominator')
        values=sorted(r['nanoseconds_per_frame']for r in selected)
        def percentile(p):
            x=(len(values)-1)*p;a=int(x);return values[a]+(values[min(a+1,len(values)-1)]-values[a])*(x-a)
        expected={'minimum_ns':min(values),'p05_ns':percentile(.05),'median_ns':statistics.median(values),'p95_ns':percentile(.95),'maximum_ns':max(values),'mean_ns':statistics.mean(values),'sample_duration_ns_range':[min(r['nanoseconds']for r in selected),max(r['nanoseconds']for r in selected)]}
        equal(b['statistics'][scenario],expected,'Independent sample statistics')
    expected=[s for i in range(b['samples_per_scenario'])for s in(scenarios if i%2==0 else scenarios[::-1])];equal([r['scenario']for r in b['samples']],expected,'Alternating timing scenario order')

def resource_gates():
    f=load('budget/footprint.json');b=load('budget/benchmark.json');footprint_gate(f);benchmark_gate(b,f);pins(f['source_hashes']);require(b['footprint_sha256']==sha(E/'budget/footprint.json'),'Timing exact footprint pin')
    fixture=load('budget/fixture-validation.json');expected_fixture=copy.deepcopy(fixture)
    for scenario in expected_fixture['scenarios']:scenario['approved_graph_noise_hashes_verified']=144
    equal(expected_fixture,f['fixture_validation'],'Fixture evidence plus independently verified graph/noise count')
    sizes={};alignments={}
    for line in (E/'budget/record-sizes.stdout').read_text().splitlines():n,size,alignment=line.split();sizes[n]=int(size);alignments[n]=int(alignment)
    equal(sizes,f['record_bytes'],'Raw record sizes');equal(alignments,f['record_alignments'],'Raw record alignments')
    frames=[]
    for n,row in f['objects'].items():
        stem=n.replace('/','_').replace('.cpp','');text=(E/'budget'/(stem+'-sections.txt')).read_text();sections={}
        for line in text.splitlines():
            parts=line.split()
            if len(parts)==3 and parts[0].startswith('.'):sections[parts[0]]=int(parts[1])
        equal(sections,row['sections'],'Raw object section list');parts=(E/'budget'/(stem+'-size.txt')).read_text().splitlines()[1].split();equal([int(v)for v in parts[:3]],[row['gnu_size_text_including_readonly_and_unwind_bytes'],row['data_bytes'],row['bss_bytes']],'Raw GNU object categories')
        undefined=(E/'budget'/(stem+'-undefined.stdout')).read_text();refs=[s for s in undefined.splitlines()if any(x in s for x in('operator new','malloc','calloc','realloc','aligned_alloc'))];equal(refs,row['direct_allocator_references'],'Raw allocator audit')
        for line in (E/'budget'/(stem+'.su')).read_text().splitlines():name,bytes_,kind=line.split('\t');frames.append({'unit':n,'function':name,'bytes':int(bytes_),'kind':kind})
    equal(sorted(frames,key=lambda x:(x['unit'],x['function'])),sorted(f['production_stack_frames'],key=lambda x:(x['unit'],x['function'])),'Every compiler stack record')
    for line in(E/'budget/benchmark.su').read_text().splitlines():name,bytes_,kind=line.split('\t');frames.append({'unit':'native-benchmark-harness','function':name,'bytes':int(bytes_),'kind':kind})
    for chain in f['selected_additive_compiler_frame_estimates']:
        for member in chain['members']:require(member in frames,'Selected compiler stack member matches raw record')
    for c in f['commands']:
        if '-c' in c:strict_flags(c,False, '-std=c++20'if any('prepare_frame49w.cpp'in x for x in c)else'-std=c++17',False)
    for name in ('benchmark','ordinary-new-probe'):
        rows=[c for c in f['commands']if '-o'in c and Path(c[c.index('-o')+1]).name==name];require(len(rows)==1,'Native-only link command')
        actual={Path(p).stem for p in rows[0]if p.endswith('.o')};expected={n.replace('/','_').replace('.cpp','')for n in f['linked_native_production_units']}|{name};equal(actual,expected,'Native-only link object membership')
    symbols=(E/'budget/timed-executable-symbols.stdout').read_text();require(not any(s in symbols for s in('original::','original49','oracle','Machine','frame49w_test','nlohmann','chariotlifecycle49v::','counting','allocated')),'Interpreter/counting code in timed executable')
    def parse(text):return {k:v if k=='scenario'else int(v)for k,v in(x.split('=',1)for x in text.split())}
    for row in f['preflight']:
        binary='ordinary-new-probe'if row['allocation_counter_enabled']else'benchmark';name='preflight-'+binary+'-'+row['scenario'];equal(parse((E/'budget'/(name+'.stdout')).read_text()),row,'Raw native preflight');require(not(E/'budget'/(name+'.stderr')).read_bytes(),'Preflight stderr')
    for rows,prefix in [(b['calibrations'],'calibration-'),(b['untimed_allocation_probes'],'allocation-probe-'),(b['samples'],None)]:
        for row in rows:
            name=prefix+row['scenario']if prefix else f"sample-{row['sample']:02}-{row['scenario']}";equal(parse((E/'budget'/(name+'.stdout')).read_text()),{k:v for k,v in row.items()if k not in('sample','nanoseconds_per_frame')},'Raw timing sample');require(not(E/'budget'/(name+'.stderr')).read_bytes(),'Timing stderr')
    for i,prefix in enumerate(('connected','connected/no-support')):
        scenario=f['fixture_validation']['scenarios'][i];require(scenario['compared_logical_leaves']==1359187 and len(scenario['hashes'])==144,'Complete source fixture comparison')
        golden=(E/prefix/'exact-goldens49w.txt').read_text().splitlines()
        for row,line in zip(scenario['hashes'],golden):
            frame,mesh,graph,noise=line.split();require(row['frame']==int(frame)and row['graph_fnv64']==int(graph)and row['noise_fnv64']==int(noise),'Budget exact history graph/noise goldens')
        require(scenario['final_state_hash']==scenario['hashes'][-1]['after'],'Fixture terminal state hash')
    require((E/'budget/actual-history144.bin').stat().st_size==f['harness_only_memory']['binary_fixture_disk_bytes'],'Binary full-history fixture size')
    return {'disjoint_runtime_payload_bytes':f['disjoint_runtime_payload_bytes'],'canonical_state_bytes':f['record_bytes']['VehicleFrameState49W'],'workspace_bytes':f['record_bytes']['VehicleFrameWorkspace49W'],'new49W_object_totals':f['new49W_object_totals'],'new49W_runtime_object_totals':f['new49W_runtime_object_totals'],'native_linked_units':len(f['linked_native_production_units']),'ordinary_new_hot_allocations':0,'maximum_native_compiler_frame_bytes':f['maximum_native_individual_compiler_frame_bytes'],'statistics':b['statistics'],'samples_per_scenario':b['samples_per_scenario'],'timing_excludes_palette_vertices':True,'process_ram_claimed':False}

def render_report_gate(report,plan):
    fields(report,{'schema':'advancing49w.visual-report.v1','status':'rendered_diagnostic_only','actual_draw_calls':360,'actual_submitted_triangles':73409,'actual_gl_buffer_uploads':4,'actual_texture_uploads':26,'shared_mesh_sets':2,'framebuffer_unchanged_by_caption':True,'opaque_output_alpha':True},'Rendered frame')
    equal(report['plan'],plan,'Render executed exact plan')
    fields(plan,{'schema':'advancing49w.visual-plan.v1','total_draw_calls':360,'submitted_triangles':73409,'wire_draw_calls':0,'inherited49H_plan_sha256':'3c2b9aeb1b74b4edf2f87d6a7a97c4df66c2ffe2e8539f5708a509b75832eb53'},'Render plan')
    fields(plan['profile'],{'original_lighting_or_visibility':False,'original_material_programs':False,'auto_fit_or_reposition':False,'depth_test':'LESS'},'Honest fixed-camera diagnostic')
    require(plan['budget']['gl_geometry_and_texture_payload_bytes']==7604084 and report['gl_error_checks_passed'],'Renderer payload/GL errors')

def render_gates():
    from PIL import Image
    import numpy as np
    from types import SimpleNamespace
    sys.path.insert(0,str(ROOT/'tools/visual'))
    renderer=module('release_render49w','tools/visual/render_vehicle_frame49w.py')
    frames=(0,31,95,143);keys=[('support',f,'full-motion')for f in frames]+[('support',31,p)for p in('physical-copy-only','frozen-roots')]+[('no-support',f,'full-motion')for f in frames]
    verification=load('views/visual-verification.json')
    fields(verification,{'schema':'advancing49w.visual-verification.v1','passed':True,'rendered_views':10,'independent_passes':2,'total_actual_draws':7200,'all_raw_and_caption_pixels_identical':True,'no_support_late_framebuffers_identical':True},'Visual verification')
    require(verification['renderer_sha256']==sha(ROOT/'tools/visual/render_vehicle_frame49w.py') and verification['composer_sha256']==sha(ROOT/'tools/visual/compose_vehicle_frame_views49w.py') and verification['verifier_sha256']==sha(ROOT/'tools/visual/verify_vehicle_frame_views49w.py') and verification['presentation_sha256']==sha(E/'presentation-final/presentation.json'),'View source/presentation pins')
    pins(verification['image_and_capture_files'])
    def pixels(path):
        with Image.open(path) as im:return np.asarray(im.convert('RGBA'))
    def difference(a,b):
        yy,xx=np.where(np.any(a!=b,axis=2));return {'different_framebuffer_pixels':len(xx),'difference_bounds_xyxy':[int(xx.min()),int(yy.min()),int(xx.max()+1),int(yy.max()+1)]if len(xx)else None}
    raw={};caption={};computed=[];expected_files=set()
    for scenario,frame,profile in keys:
        name=f'{scenario}-frame{frame:03d}-{profile}';pair=[];directory=E/'connected'/('no-support'if scenario=='no-support'else'')
        args=SimpleNamespace(export_report=directory/'export-report.json',export_sha256=sha(directory/'export-report.json'),frame=frame,profile=profile)
        freshplan=renderer.load_plan(args,0,np)[3]
        for mode in ('pass1','pass2'):
            base=E/'views'/mode;report=load('views/'+mode+'/'+name+'.json');plan=load('views/'+mode+'/'+name+'.plan.json')
            render_report_gate(report,plan)
            require(digest(renderer.base.json_bytes({k:v for k,v in plan.items()if k!='plan_sha256'}))==plan['plan_sha256'],'Retained render plan digest')
            equal(relocated({k:v for k,v in plan.items()if k not in('plan_sha256','inherited49H_plan_sha256')}),relocated({k:v for k,v in freshplan.items()if k not in('plan_sha256','inherited49H_plan_sha256')}),'Recomputed bounded render plan')
            a=pixels(base/(name+'.raw.png'));b=pixels(base/(name+'.png'))
            require(a.shape==(480,640,4) and b.shape==(568,640,4) and np.array_equal(a,b[:480]),'Caption preserves framebuffer')
            require(digest(a.tobytes())==report['pixel_rgba8_sha256'],'Framebuffer decoded pixel hash')
            require(len(report['images'])==2,'Rendered image count')
            for image in report['images']:
                path=current_path(image['path']);require(sha(path)==image['sha256'] and path.stat().st_size==image['bytes'] and digest(pixels(path).tobytes())==image['rgba8_sha256'],'PNG encoded/decoded evidence')
            for suffix in('.png','.raw.png','.json','.plan.json','.plan.stdout','.plan.stderr','.render.stdout','.render.stderr'):
                path=base/(name+suffix);expected_files.add(path.relative_to(ROOT).as_posix())
                if suffix.endswith('.stderr'):require(not path.read_bytes(),'Renderer stderr')
            if mode=='pass1':raw[(scenario,frame,profile)]=a;caption[(scenario,frame,profile)]=b
            else:require(np.array_equal(a,raw[(scenario,frame,profile)]) and np.array_equal(b,caption[(scenario,frame,profile)]),'Two independently rendered passes differ')
            pair.append(report)
        computed.append({'scenario':scenario,'frame':frame,'profile':profile,'plan_sha256':pair[0]['plan']['plan_sha256'],'pixel_rgba8_sha256':pair[0]['pixel_rgba8_sha256'],'draw_calls':360,'triangles':73409,'gpu_payload_bytes':7604084,'two_pass_raw_and_caption_pixels_identical':True,'maximum_resident_set_bytes':max(r['runtime']['maximum_resident_set_bytes']for r in pair)})
    equal(set(verification['image_and_capture_files']),expected_files,'All160 visual process/image artifacts pinned');equal(verification['views'],computed,'Recomputed visual rows')
    controls={}
    for profile,changed in [('physical-copy-only',{'chariot':2884,'left_engine':0,'right_engine':0}),('frozen-roots',{'chariot':2884,'left_engine':1411,'right_engine':1460})]:
        delta=difference(raw[('support',31,'full-motion')],raw[('support',31,profile)]);require(delta['different_framebuffer_pixels']>0,'Display control visible effect');controls[profile]=dict(delta,changed_component_vertices=changed)
    equal(verification['controls'],controls,'Computed control pixel differences')
    comparisons={str(f):difference(raw[('support',f,'full-motion')],raw[('no-support',f,'full-motion')])for f in frames}
    equal(verification['scenario_comparisons'],comparisons,'Computed scenario pixel differences');require(comparisons['0']['different_framebuffer_pixels']==0 and all(comparisons[str(f)]['different_framebuffer_pixels']>0 for f in frames[1:]),'Scenario image separation')
    progression={f'{a}_to_{b}':difference(raw[('support',a,'full-motion')],raw[('support',b,'full-motion')])for a,b in zip(frames,frames[1:])};equal(verification['support_progression'],progression,'Computed support progression');require(all(r['different_framebuffer_pixels']>0 for r in progression.values()),'Supported visible advancement')
    require(all(np.array_equal(raw[('no-support',31,'full-motion')],raw[('no-support',f,'full-motion')])for f in (95,143)),'No-support depth-hidden late images')
    presentation=load('presentation-final/presentation.json');fields(presentation,{'schema':'advancing49w.presentation.v1','status':'passed','source_pass':'pass1','draws_performed':0,'camera_changed':False,'poses_changed':False,'control_crop_box':[128,96,544,384]},'Presentation limits');pins(presentation['source_pngs']);require(len(presentation['images'])==4,'Four presentation images')
    images=[]
    for row in presentation['images']:
        path=current_path(row['path']);im=pixels(path);require(sha(path)==row['sha256'] and path.stat().st_size==row['bytes'] and list(im.shape[1::-1])==row['dimensions'] and digest(im.tobytes())==row['rgba8_sha256'],'Presentation bytes/pixels');images.append(im)
    for i,f in enumerate(frames):
        for j,scenario in enumerate(('support','no-support')):
            require(np.array_equal(images[0][i*632+64:(i+1)*632,j*640:(j+1)*640],caption[(scenario,f,'full-motion')]),'Contact sheet exact tile')
            if f==31:require(np.array_equal(images[1][64:632,j*640:(j+1)*640],caption[(scenario,f,'full-motion')]),'Paired frame exact tile')
        require(np.array_equal(images[2][40+(i//2)*568:40+(i//2+1)*568,(i%2)*640:(i%2+1)*640],caption[('support',f,'full-motion')]),'Supported sheet exact tile')
    for i,key in enumerate([('support',31,'full-motion'),('support',31,'physical-copy-only'),('support',31,'frozen-roots'),('no-support',31,'full-motion')]):
        expected=np.repeat(np.repeat(raw[key][96:384,128:544],2,axis=0),2,axis=1);x,y=(i%2)*832,(i//2)*648
        require(np.array_equal(images[3][y+72:y+648,x:x+832],expected),'Control crop exact nearest pixels')
    animation=presentation['animation'];fields(animation,{'frames':4,'presentation_frame_duration_ms':900,'loop':True,'native_frame_numbers':list(frames),'dimensions':[1280,632]},'Animation sample/timing boundary');path=current_path(animation['path']);require(sha(path)==animation['sha256'] and path.stat().st_size==animation['bytes'],'Animation file pin')
    with Image.open(path)as image:
        require(image.n_frames==4,'Animation frame count')
        for i,f in enumerate(frames):
            image.seek(i);decoded=np.asarray(image.convert('RGBA'));require(digest(decoded.tobytes())==animation['decoded_frame_rgba8_sha256'][i],'Animation decoded pixels')
            for j,scenario in enumerate(('support','no-support')):require(np.array_equal(decoded[64:632,j*640:(j+1)*640],caption[(scenario,f,'full-motion')]),'Animation exact source tile')
    return {'views_each_pass':10,'independent_passes':2,'actual_draws':7200,'unchanged_camera':True,'recomputed_plans':10,'exact_presentation_artifacts':5,'control_pixels':controls,'no_support_late_pixels_identical':True}


def final_review_gate(report):
    fields(report,{'status':'passed','unresolved_findings':[]},'Final independent release review')
    require(set(report['reviewed_components'])=={'core','connected','render','aggregate','budget','release'},'Independent review areas')
    require(all(v is True or isinstance(v,dict)and v.get('passed')is True for v in report['reviewed_components'].values()),'Every independent release area passed')
    required={'tools/validate_vehicle_frame49w.py','tools/archive_checkpoint49w.py','docs/staged-vehicle-frame49w.md'}
    required.update('evidence/frameBridge49W/'+n for n in('regression-summary.json','final-normal/regression-report.json','final-sanitized/regression-report.json','budget/footprint.json','budget/benchmark.json','budget/fixture-validation.json','views/visual-verification.json','presentation-final/presentation.json','connected/freeze.json','frame/final-verification/validation.json','independent-review/verification.json','independent-review/source-authentication.json','independent-review/final-frame-review.json','independent-review/connected-state-review.json','independent-review/connected-freeze-review.json'))
    require(required<=set(report['input_sha256']),'Final independent review pin membership');pins(report['input_sha256'])


def controls(complete=False):
    rows=[]
    def reject(name,value,mutate,gate):
        changed=copy.deepcopy(value);mutate(changed)
        try:gate(changed)
        except (GateError,AssertionError,KeyError,TypeError,ValueError,IndexError) as error:rows.append({'name':name,'rejected':True,'diagnostic':str(error)})
        else:raise GateError('Negative control escaped: '+name)
    for scenario,unsupported in [('connected',False),('connected/no-support',True)]:
        trace=load(scenario+'/trace.json');gate=lambda x,u=unsupported:trace_gate(x,u)
        reject(scenario+'_missing_frame',trace,lambda x:x['frames'].pop(),gate)
        reject(scenario+'_reset_persistence',trace,lambda x:x['frames'][10]['before'].update(phase864=0xffffffff),gate)
        reject(scenario+'_lost_shape',trace,lambda x:x['frames'][10]['after']['shape'].update(current=[0]*16),gate)
        reject(scenario+'_copy_postphysics_graphics',trace,lambda x:x['frames'][10]['after']['graph'][0]['local_bits'].__setitem__(12,0),gate)
        reject(scenario+'_lost_cache_high_bytes',trace,lambda x:x['frames'][10]['after'].update(main_scene_flags=133),gate)
        reject(scenario+'_premature_phase',trace,lambda x:x['frames'][10]['after_graphics_before_physics'].update(main_phase=0),gate)
        reject(scenario+'_duplicate_mode_owner',trace,lambda x:x['frames'][10]['after'].update(physical838=999),gate)
        reject(scenario+'_missing_original_shape',trace,lambda x:x['summary'].update(shape_original_comparisons=0),gate)
        reject(scenario+'_contact_source_claim',trace,lambda x:x['summary'].update(native_hot_allocations=1),gate)
        reject(scenario+'_wrong_support_force',trace,lambda x,u=unsupported:x['frames'][10]['supplied354_support_forces'][0].update(main_support_y_bits=1065353216 if u else 0),gate)
    for prefix,count in [('body/validation',24),('shape/verification',24),('frame/final-verification',13)]:
        data=load(prefix+'/mutations.json');gate=lambda x,n=count,p=prefix:mutation_gate(x,n,p)
        reject(prefix+'_missing_mutant',data,lambda x:x['detected'].pop(),gate)
        reject(prefix+'_setup_failure_counted',data,lambda x:x.update(setup_failures_counted=1),gate)
    rendered=load('views/pass2/support-frame031-full-motion.json');plan=rendered['plan']
    reject('render_missing_draw',rendered,lambda x:x.update(actual_draw_calls=359),lambda x:render_report_gate(x,plan))
    reject('render_caption_changes_pixels',rendered,lambda x:x.update(framebuffer_unchanged_by_caption=False),lambda x:render_report_gate(x,plan))
    reject('render_claims_original_lighting',rendered,lambda x:x['plan']['profile'].update(original_lighting_or_visibility=True),lambda x:render_report_gate(x,plan))
    pin={'src/game/vehicle_frame49w.cpp':sha(ROOT/'src/game/vehicle_frame49w.cpp')}
    reject('stale_source_pin',pin,lambda x:x.__setitem__('src/game/vehicle_frame49w.cpp','0'*64),pins)
    if complete:
        r,inputs,old=expected_runner()
        for mode in ('normal','sanitized'):
            report=load('final-'+mode+'/regression-report.json');gate=lambda x,m=mode:report_gate(x,m,r,inputs,old)
            reject(mode+'_aggregate_hidden_failure',report,lambda x:x['tests'][0]['run'].update(returncode=1),gate)
            reject(mode+'_aggregate_missing_suite',report,lambda x:x['tests'].pop(),gate)
            reject(mode+'_aggregate_missing_object',report,lambda x:x['build'].pop(),gate)
            reject(mode+'_aggregate_fast_math',report,lambda x:x['build'][0]['command'].append('-ffast-math'),gate)
            reject(mode+'_aggregate_traced_object',report,lambda x:x['build'][0]['command'].extend(['-include','frame_stage_trace.h']),gate)
            reject(mode+'_aggregate_changed_input',report,lambda x:x['changed_inputs'].append('src/game/vehicle_frame49w.cpp'),gate)
            reject(mode+'_aggregate_missing_input',report,lambda x:x['input_hashes'].pop('src/game/vehicle_frame49w.cpp'),gate)
            reject(mode+'_aggregate_stderr',report,lambda x:x['tests'][0]['run']['stderr'].update(bytes=1),gate)
            reject(mode+'_aggregate_no_support_wrong_cli',report,lambda x:next(t for t in x['tests']if t['name']=='vehicle_frame_no_support49w')['run']['command'].pop(),gate)
        f=load('budget/footprint.json');b=load('budget/benchmark.json')
        reject('budget_nested_double_count',f,lambda x:x['disjoint_runtime_records'].update(CoupledBodyState49M=1580),footprint_gate)
        reject('budget_workspace_sum',f,lambda x:x['record_bytes'].update(VehicleFrameWorkspace49W=73384),footprint_gate)
        reject('budget_cold_decoder_in_hot_link',f,lambda x:x['linked_native_production_units'].append('src/assets/pod_shape49w.cpp'),footprint_gate)
        reject('budget_hot_allocator',f,lambda x:x['objects']['src/game/vehicle_frame49w.cpp']['direct_allocator_references'].append('malloc'),footprint_gate)
        reject('budget_wrong_text_sum',f,lambda x:x['new49W_runtime_object_totals'].update(executable_text_bytes=0),footprint_gate)
        reject('budget_missing_state_field',f,lambda x:x['state_composition_included_not_added'].pop('PodShapeState49W'),footprint_gate)
        reject('budget_wrong_compiler_frame',f,lambda x:x.update(maximum_native_individual_compiler_frame_bytes=0),footprint_gate)
        reject('budget_probe_disabled',f,lambda x:x['preflight'][2].update(allocation_counter_enabled=0),footprint_gate)
        reject('budget_probe_allocation',f,lambda x:x['preflight'][2].update(cpp_new_allocations=1),footprint_gate)
        gate=lambda x:benchmark_gate(x,f)
        reject('timing_missing_sample',b,lambda x:x['samples'].pop(),gate)
        reject('timing_wrong_median',b,lambda x:x['statistics']['synthetic-support'].update(median_ns=0),gate)
        reject('timing_wrong_denominator',b,lambda x:x['samples'][0].update(frames=1),gate)
        reject('timing_wrong_checksum',b,lambda x:x['samples'][0].update(checksum=0),gate)
        reject('timing_allocation_counter_enabled',b,lambda x:x['samples'][0].update(allocation_counter_enabled=1),gate)
        reject('timing_missing_preflight',b,lambda x:x['samples'][0].update(preflight_phase_snapshots=0),gate)
        final=load('independent-review/final-review.json')
        reject('independent_open_finding',final,lambda x:x['unresolved_findings'].append('open'),final_review_gate)
        reject('independent_missing_budget',final,lambda x:x['reviewed_components'].pop('budget'),final_review_gate)
        reject('independent_stale_validator',final,lambda x:x['input_sha256'].__setitem__('tools/validate_vehicle_frame49w.py','0'*64),final_review_gate)
    for name,data in [('duplicate',['x','x']),('traversal',['../x']),('absolute',['/x']),('backslash',['x\\y']),('empty_component',['x//y'])]:
        try:safe_names(data)
        except GateError as error:rows.append({'name':name,'rejected':True,'diagnostic':str(error)})
        else:raise GateError('Unsafe path control escaped')
    return {'status':'passed','all_rejected':True,'count':len(rows),'method':'Decoded in-memory evidence mutations; no native execution or file changes.','controls':rows}

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--baseline',type=Path);ap.add_argument('--seal',action='store_true');ap.add_argument('--check-only',action='store_true');ap.add_argument('--components-only',action='store_true');args=ap.parse_args()
    require(__debug__,'Python assertions must be enabled for retained read-only reconstruction')
    require(sum((args.seal,args.check_only,args.components_only))<=1,'Choose one validation mode')
    require(args.components_only or args.baseline is not None,'--baseline required for release validation')
    subprocess.run=blocked_process;subprocess.Popen=blocked_process
    visual=module('release_connected49w','tools/visual/run_vehicle_frame49w.py')
    focused=focused_gates(visual);connected=connected_gates(visual);review=independent_gates();r,inputs,old=expected_runner();graph=build_graph_gate(r);negative=controls()
    if args.components_only:
        print(json.dumps({'status':'passed','scope':'components only; release unsealed','source':focused,'connected':connected,'build_graph':graph,'negative_controls':negative['count']}));return
    resources=resource_gates();renders=render_gates();aggregate=regression_gates(r,inputs,old);final_review_gate(load('independent-review/final-review.json'));negative=controls(True);names,changed=baseline_gate(args.baseline)
    result={'checkpoint':'49W-StagedAdvancingFrame','status':'passed','baseline_sha256':BASELINE_SHA,'baseline_bytes':BASELINE_BYTES,'baseline_members':len(names),'baseline_manifest':BASELINE_MANIFEST,'changed_baseline_members':changed,'regression_suites_each':69,'regression_checks_each':aggregate['checks_each'],'tracked_inputs_each':len(inputs),'production_units_each':62,'new_production_units':4,'newly_linked_existing_production_units':2,'production_warning_note':'Seven compact-style misleading-indentation warnings in new frame bridge; raw diagnostics retained. No warning-free claim.','all64_inherited_outputs_exact':True,'source_accounting':focused,'connected':connected,'independent_review':review,'native_build_graph':graph,'resources':resources,'renders':renders,'gate_controls':negative,'full_captures_preserved':True,'source_hashes':seal_hashes(names),'limits':'Bounded initialized single-owner staged advancing frame with supplied preserving cuts, synthetic354 counter-gravity support or separately verified no-support trajectory, and supplied no-hit query. No original contact producer, spawn, race-ready, complete original frame, original material/VU/lighting/visibility or whole-game claim. Host code/storage/stack/timing are not process RAM, gameplay FPS or console performance. ASan+UBSan only; LeakSanitizer disabled. No PC/RAM/install, excluded historical capacity or ownership operation was executed by this validator.'}
    if args.seal:
        require(not (E/'checkpoint-validation.json').exists(),'Existing seal requires explicit removal before resealing')
        result['verified_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();(E/'checkpoint-validation.json').write_text(json.dumps(result,indent=2)+'\n')
    elif args.check_only or (E/'checkpoint-validation.json').exists():
        previous=load('checkpoint-validation.json');require({k:v for k,v in previous.items()if k!='verified_utc'}==result and isinstance(previous.get('verified_utc'),str),'Existing seal differs from recomputed evidence')
    print(json.dumps({'status':'passed','seal_written':args.seal,'baseline_members':len(names),'regression_suites_each':69,'production_units_each':62,'checks_each':aggregate['checks_each'],'frozen_new_or_allowed_files':len(result['source_hashes']),'negative_controls':negative['count']}))

if __name__=='__main__':
    try:main()
    except (GateError,AssertionError,KeyError,TypeError,OSError,ValueError,IndexError,StopIteration,zipfile.BadZipFile) as error:
        print('49W release validation failed: '+str(error),file=sys.stderr);raise SystemExit(1)
