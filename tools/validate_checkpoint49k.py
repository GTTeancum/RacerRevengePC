#!/usr/bin/env python3
"""Validate frozen 49K source, original-word proof, traces, views and baseline."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import zipfile

ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/bodyAdvancement49K'
BASELINE_SHA='6376d5cd58cec22eaa1661ae003d3a73a463f90502bad4a515a0a2975bdaaf8e'
TRACE_SHA='262a69dbcb0a651d69cc207f10174c93c60fccedc72d7c1e859168e8a023a098'
HEADER_SHA='1c479c20842c7334e1112adebfb1903d182b943f9c1bc3ff6aa9e3ed973c3071'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def load(n): return json.loads((E/n).read_text())

def main():
    if not __debug__: raise RuntimeError('Validation requires assertions; do not use Python -O')
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--baseline',type=Path,required=True);a=p.parse_args()
    assert sha(a.baseline)==BASELINE_SHA and a.baseline.stat().st_size==116535643
    changes=[]
    with zipfile.ZipFile(a.baseline) as old:
        old_names={i.filename for i in old.infolist() if not i.is_dir()}
        assert len(old_names)==16788
        for name in sorted(old_names):
            path=ROOT.parent/name;assert path.is_file(),name
            if sha(path)!=hashlib.sha256(old.read(name)).hexdigest(): changes.append(name)
    assert set(changes)==ALLOWED,changes
    header=ROOT/'work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h'
    assert header.stat().st_size==1756 and sha(header)==HEADER_SHA
    r=subprocess.run([sys.executable,'-B','tools/gameplay/advancement_numeric_recover49k.py','--check'],cwd=ROOT,capture_output=True,text=True)
    assert r.returncode==0,r.stderr
    checked=json.loads(r.stdout);assert checked['status']=='verified' and checked['files']==27
    numeric=load('numeric-source/validation-manifest.json');assert numeric['status']=='passed'
    for mode in ('normal','asan_ubsan'):
        x=numeric[mode]
        assert x['status']=='passed' and x['cases']==8173 and x['checks']==54019
        assert x['selected_words']==x['reached_words']==253
        assert x['interpreted_instructions']==227532 and x['hot_allocations']==0 and x['hot_calls']==8797
    assert numeric['safe_mutations_detected']==16
    for name,digest in numeric['files'].items(): assert sha(ROOT/name)==digest,name
    composition=load('composition/validation-manifest.json')
    assert composition['frames']==180 and composition['substeps']==404
    assert composition['normal_checks_with_trace']==214589 and composition['sanitizer_checks_without_trace']==214588
    assert composition['original_word_instructions']==1202596
    assert composition['allocation_free_native_transitions_including_clock49j']==2740
    assert composition['safe_order_sensitivity']['mutations']==composition['safe_order_sensitivity']['detected']==12
    for name,row in composition['files'].items():
        path=ROOT/name;assert path.stat().st_size==row['size'] and sha(path)==row['sha256'],name
    review=load('independent-review/review-summary.json')
    assert review['status']=='passed' and review['blocking_findings']==0 and review['inputs_unchanged']
    assert review['source_word_verification']['new_selected_header_words']==253
    assert review['source_word_verification']['retained_original_words']==3607
    assert review['independent_scalar_fixture']['safe_mutations_detected']==review['independent_scalar_fixture']['safe_mutations_total']==4
    for name,digest in review['frozen_source_hashes'].items():assert sha(ROOT/name)==digest,name
    packaging=load('packaging-review.json')
    assert packaging['status']=='passed' and not packaging['blocking_findings']
    for name,digest in packaging['reviewed_file_hashes'].items():assert sha(ROOT/name)==digest,name
    legacy=load('legacy-regressions/integrated-nearest-test.json')
    assert legacy['mechanical_copy_verified'] and legacy['original_source_still_unchanged']
    original=(ROOT/legacy['original_path']).read_text();adapted=original
    for change in legacy['changes']:
        if change['operation']=='prepend':adapted=change['text']+adapted
        else:
            assert adapted.count(change['old'])==1
            adapted=adapted.replace(change['old'],change['new'],1)
    assert (ROOT/legacy['copy_path']).read_text()==adapted
    assert sha(ROOT/legacy['original_path'])==legacy['original_sha256']
    assert sha(ROOT/legacy['copy_path'])==legacy['copy_sha256']
    assert all(x['build_exit']==x['run_exit']==0 for x in legacy['runs'])
    regression=load('regression-summary.json')
    assert regression['status']=='passed' and regression['all_mode_outputs_identical']
    for mode in ('normal','sanitized'):
        x=regression['runs'][mode]
        assert x['suites']==20 and x['checks']==11037512 and x['tracked_inputs']==132 and x['inputs_unchanged']
        assert sha(E/f'final-{mode}/regression-report.json')==x['report_sha256']
        report=load(f'final-{mode}/regression-report.json')
        assert report['status']=='passed' and not report['errors'] and not report['changed_inputs']
        for name,row in report['input_hashes'].items():assert sha(ROOT/name)==row['sha256'],name
        for test in x['tests']:
            assert test['stderr_bytes']==0
            assert sha(E/f'final-{mode}'/(test['name']+'.stdout'))==test['stdout_sha256']
        assert sha(E/f'final-{mode}/persistent-trace.json')==TRACE_SHA
        assert '214589 checks' in (E/f'final-{mode}/trace-run.stdout').read_text()
    assert sha(E/'composition/persistent-synthetic-trace.json')==TRACE_SHA
    trace=load('composition/persistent-synthetic-trace.json')
    assert len(trace['frames'])==180 and sum(len(f['steps']) for f in trace['frames'])==404
    visual=load('visual/validation-summary.json')
    assert visual['status']=='passed' and visual['trace_sha256']==TRACE_SHA
    assert sha(ROOT/'tools/visual/render_advancement49k.py')==visual['render_script_sha256']
    assert visual['checks']['both_raw_and_captioned_rerender_PNG_bytes_identical']
    assert visual['checks']['exact_native_scene_bits_checked_against_decimals_all404steps']
    assert visual['frame_difference']['pixels_changed']==8278
    assert [f['frame'] for f in visual['frames']]==[0,30]
    for frame in visual['frames']:
        assert frame['actual_draw_calls']==360 and frame['actual_submitted_triangles']==73409
        assert frame['actual_texture_uploads']==26 and frame['GL_payload_bytes']==7604084
        for image in frame['images']:
            path=E/'visual'/Path(image['path']).name
            assert path.stat().st_size==image['bytes'] and sha(path)==image['sha256']
        prefix='frame-'+str(frame['frame']).zfill(3)
        assert (E/'visual'/(prefix+'.png')).read_bytes()==(E/'visual'/(prefix+'-repeat.png')).read_bytes()
        assert (E/'visual'/(prefix+'.raw.png')).read_bytes()==(E/'visual'/(prefix+'-repeat.raw.png')).read_bytes()
    footprint=load('footprint/summary.json');assert footprint['status']=='measured'
    assert footprint['types']['MainBodyState49K']==764 and footprint['types']['MainBodyParameters49K']==304
    assert footprint['persistent_state_parameters_and_clock_bytes']==1136
    for name,text,stack in [('advancement_selectors49k',2688,160),('body_advancement49k',4834,1056)]:
        row=footprint['new_objects'][name]
        assert row['text_bytes']==text and row['data_bytes']==row['bss_bytes']==0
        assert row['maximum_individual_static_frame_bytes']==stack
        assert row['source_sha256']==sha(ROOT/'src/game'/(name+'.cpp'))
    hashes={}
    for path in sorted(ROOT.rglob('*')):
        if not path.is_file() or path==E/'validation.json':continue
        name=path.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/'+name not in old_names or 'RacerRevengePC/'+name in ALLOWED:hashes[name]=sha(path)
    result={'checkpoint':'49K-BodyAdvancement','status':'passed','validated_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
      'baseline':{'archive':a.baseline.name,'sha256':BASELINE_SHA,'bytes':a.baseline.stat().st_size,'members_preserved':len(old_names),'changed_baseline_files':changes,'library_file_id':'libfile_a139a6116b088191bf503d6002871979'},
      'normal_and_asan_ubsan':{'suites_each':20,'checks_each':11037512,'tracked_inputs_unchanged_each':132,'leak_check':'unavailable under ptrace; disabled'},
      'word_proof':{'new_selected':253,'new_reached':253,'independently_checked_retained':3607,'numeric_cases':8173,'numeric_instructions':227532,'connected_instructions':1202596},
      'connected':{'frames':180,'substeps':404,'trace_sha256':TRACE_SHA,'normal_sanitized_trace_identical':True,'native_allocations':0,'hot_transitions':2740},
      'safe_mutations':{'numeric':16,'composition':12,'additional_independent':4,'all_detected':True},
      'restored_reference':{'path':str(header.relative_to(ROOT)),'bytes':1756,'sha256':HEADER_SHA,'library_file_id':'libfile_fb6763efab7481919342fb529fe59cb8'},
      'limitations':['Explicit synthetic post-player/world/contact/query state; not original freeflight or full vehicle','Historical48H FE_DOWNWARD GCC-O2 zero-sign failure retained; new49K admits nearest-even','World collision/reset unresolved;49C excluded and unreleased','No hardware acquisition, physicalPS2 certification, Xbox/fullgame memory or gameplayFPS claim'],
      'source_hashes':hashes}
    (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'status':'passed','baseline_members':len(old_names),'changed_baseline_files':changes,'new_or_changed_files_hashed':len(hashes)}))
if __name__=='__main__':main()
