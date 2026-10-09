#!/usr/bin/env python3
"""Validate frozen 49L sphere-constraint source, proof, review and baseline."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import zipfile
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/sphereConstraint49L'
BASELINE_SHA='ed009690cd0af3c669e9d45927847d7f3fc9406b3c4aaff7e865199886595009'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads((E/p).read_text())
def main():
    if not __debug__:raise RuntimeError('Validation requires assertions; do not use Python -O')
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--baseline',type=Path,required=True);a=p.parse_args()
    assert a.baseline.stat().st_size==120359363 and sha(a.baseline)==BASELINE_SHA
    changes=[]
    with zipfile.ZipFile(a.baseline) as old:
        old_names={i.filename for i in old.infolist() if not i.is_dir()};assert len(old_names)==17085
        for n in sorted(old_names):
            f=ROOT.parent/n;assert f.is_file(),n
            if sha(f)!=hashlib.sha256(old.read(n)).hexdigest():changes.append(n)
    assert set(changes)==ALLOWED,changes
    for script,files in [('pod_sphere_constraint_recover49l.py',22),('sphere_bindings_provenance49l.py',19)]:
        r=subprocess.run([sys.executable,'-B','tools/gameplay/'+script,'--check'],cwd=ROOT,capture_output=True,text=True)
        assert r.returncode==0,r.stderr
        result=json.loads(r.stdout);assert result['status']=='verified' and result['files']==files
    proof=load('numeric-source/verification.json')
    assert proof['normal']==proof['ASan_UBSan']=='PASS'
    assert proof['cases']==535 and proof['checks']==2102649 and proof['hot_calls']==3202
    assert proof['native_hot_allocations']==0 and proof['original_instructions']==1419380
    assert proof['normal_sanitized_coverage_identical']
    assert proof['coverage']=={'frozen_words':1639,'visited_words':1514,'excluded_constructor_prefix_words':5,'excluded_dirty_scene_words':120,'unaccounted_unvisited_words':0}
    for n,d in proof['file_sha256'].items():assert sha(ROOT/n)==d,n
    review=load('independent-review/verification.json')
    assert review['status']=='pass' and review['inputs_unchanged_during_verification']
    assert review['cpu_affinity']==[0] and review['normal_virtual_address_limit_bytes']==67108864
    for n,d in review['source_pins'].items():assert sha(ROOT/n)==d,n
    assert len(review['results'])==8
    for row in review['results']:assert row['build_exit']==row['run_exit']==0 and row['stderr']==''
    manifest=load('independent-review/manifest.json');assert manifest['status']=='pass'
    for n,d in manifest['files'].items():assert sha(E/'independent-review'/n)==d,n
    audit=load('independent-review/nonfinite-audit.json')
    assert audit['status']=='pass' and audit['probes']==318 and not audit['gaps']
    assert audit['source_sha256']==sha(ROOT/'src/game/pod_sphere_constraint49l.cpp')
    mutations=load('independent-review/scalar-mutations.json')
    assert mutations['all_detected'] and len(mutations['results'])==6
    assert all(x['build_exit']==0 and x['detected'] and x['test_exit']!=0 for x in mutations['results'])
    packaging=load('packaging-review.json');assert packaging['status']=='passed' and not packaging['blocking_findings']
    for n,d in packaging['reviewed_file_hashes'].items():assert sha(ROOT/n)==d,n
    regression=load('regression-summary.json');assert regression['status']=='passed' and regression['all_mode_outputs_identical']
    for mode in ('normal','sanitized'):
        row=regression['runs'][mode]
        assert row['suites']==22 and row['checks']==13166474 and row['tracked_inputs']==142 and row['inputs_unchanged']
        report=load(f'final-{mode}/regression-report.json')
        assert sha(E/f'final-{mode}/regression-report.json')==row['report_sha256']
        assert report['status']=='passed' and not report['errors'] and not report['changed_inputs']
        for n,d in report['input_hashes'].items():assert sha(ROOT/n)==d['sha256'],n
        for test in row['tests']:
            assert test['stderr_bytes']==0
            assert sha(E/f'final-{mode}'/(test['name']+'.stdout'))==test['stdout_sha256']
        coverage=load(f'final-{mode}/coverage.json')
        assert coverage['cases']==535 and coverage['checks']==2102649 and coverage['native_hot_allocations']==0
        assert coverage['hot_calls']==3202 and coverage['original_instructions']==1419380
        assert sum(b['words'] for b in coverage['blocks'])==1639
        assert sum(b['visited_words'] for b in coverage['blocks'])==1514
        assert (E/f'final-{mode}/coverage.json').read_bytes()==(E/'numeric-source/coverage.json').read_bytes()
        assert '2102649 bit/state checks' in (E/f'final-{mode}/coverage-run.stdout').read_text()
    footprint=load('footprint/summary.json');assert footprint['status']=='measured'
    assert footprint['joint_bytes']==388 and footprint['body_bytes']==220 and footprint['joint_plus_two_body_records_bytes']==828
    assert footprint['object_text_bytes']==18428 and footprint['object_data_bytes']==footprint['object_bss_bytes']==0
    assert footprint['maximum_individual_static_frame_bytes']==1248
    assert footprint['source_sha256']==sha(ROOT/'src/game/pod_sphere_constraint49l.cpp')
    hashes={}
    for f in sorted(ROOT.rglob('*')):
        if not f.is_file() or f==E/'validation.json':continue
        n=f.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/'+n not in old_names or 'RacerRevengePC/'+n in ALLOWED:hashes[n]=sha(f)
    result={'checkpoint':'49L-PodSphereConstraint','status':'passed','validated_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
      'baseline':{'archive':a.baseline.name,'sha256':BASELINE_SHA,'bytes':a.baseline.stat().st_size,'members_preserved':len(old_names),'changed_baseline_files':changes,'library_file_id':'libfile_f1af10ae314c8191b118dd735f100577'},
      'normal_and_asan_ubsan':{'suites_each':22,'checks_each':13166474,'tracked_inputs_unchanged_each':142,'LeakSanitizer':'unavailable under ptrace; disabled'},
      'source_proof':{'frozen_words':1639,'visited_words':1514,'base_prefix_exclusions':5,'dirty_scene_exclusions':120,'cases':535,'checks_no_output':2102648,'checks_with_coverage':2102649,'original_instructions':1419380,'hot_calls':3202,'native_allocations':0},
      'api_checks':26314,'api_allocation_free_transitions':12134,
      'independent_review':{'additional_cases':325,'additional_checks':1283431,'safe_mutations_detected':6,'nonfinite_field_probes':318,'remaining_gaps':0},
      'footprint':{'joint_bytes':388,'two_body_records_bytes':440,'combined_bytes':828,'new_text_bytes':18428,'data_bss_bytes':0,'largest_individual_static_frame_bytes':1248},
      'scope':'Original numeric constructor suffix, setup caches and three ordered PodSphereConstraint stages. A-only load writes; B read-only; explicit ready body/scene inputs.',
      'remaining_boundaries':['Base-object lifecycle and dirty-scene traversal','Optional-wrapper other force/noise stages, contact/query processing and secondary integration','Live mode/asset-to-runtime initializer and complete rendered-vehicle assembly','PhysicalPS2 exceptional modes and target-console/fullgame memory/FPS','49C excluded/unreleased; flagged capacity/ownership/reset work untouched'],
      'source_hashes':hashes}
    (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({'status':'passed','baseline_members':len(old_names),'changed_baseline_files':changes,'new_or_changed_files_hashed':len(hashes)}))
if __name__=='__main__':main()
