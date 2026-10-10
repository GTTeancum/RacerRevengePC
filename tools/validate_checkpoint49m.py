#!/usr/bin/env python3
"""Validate final49M source, original-word proof, review, regressions and baseline."""
import argparse,datetime,hashlib,json,subprocess,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/secondaryWrapper49M'
BASELINE_SHA='080b1eceaf1072a258140f046fdb510275ab20e391b66668932862296752d479'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads((E/p).read_text())
def pins(mapping):
    for n,h in mapping.items():assert sha(ROOT/n)==h,n

def main():
    if not __debug__:raise RuntimeError('Validation requires assertions')
    ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);a=ap.parse_args()
    assert a.baseline.stat().st_size==121201615 and sha(a.baseline)==BASELINE_SHA
    changes=[]
    with zipfile.ZipFile(a.baseline) as old:
        old_names={i.filename for i in old.infolist() if not i.is_dir()};assert len(old_names)==17239
        for n in sorted(old_names):
            f=ROOT.parent/n;assert f.is_file(),n
            if sha(f)!=hashlib.sha256(old.read(n)).hexdigest():changes.append(n)
    assert set(changes)==ALLOWED,changes
    commands=[['tools/gameplay/secondary_wrapper_recover49m.py','--check'],['tools/gameplay/gradient_noise_recover49m.py','--check'],['evidence/secondaryWrapper49M/noise-source/helper-audit/recover_helper_audit.py'],['evidence/secondaryWrapper49M/composition/verify_common_phases49m.py','--baseline-archive',str(a.baseline.resolve())]]
    for command in commands:
        r=subprocess.run([sys.executable,'-B',*command],cwd=ROOT,capture_output=True,text=True)
        assert r.returncode==0,(command,r.stdout,r.stderr)
    api=load('api/verification.json');assert api['status']=='passed' and api['checks_per_build']==836 and api['hot_call_allocations_per_build']==0 and api['stdout_identical'];pins(api['source_sha256'])
    proof=load('numeric-source/final-source-pins.json');pins(proof['files'])
    coverage=load('numeric-source/coverage.json')
    assert coverage['cases']==108 and coverage['checks']==43274 and coverage['native_hot_calls']==324 and coverage['native_hot_allocations']==0
    pcs=[p for b in coverage['blocks'] for p in b['pcs']]
    assert len(pcs)==2377 and sum(p['hits']>0 for p in pcs)==2144
    assert all(p.get('exclusion') for p in pcs if not p['hits'])
    assert len(coverage['blocks'][0]['pcs'])==540 and all(p['hits'] for p in coverage['blocks'][0]['pcs'])
    noise=load('noise-source/validation.json');assert noise['status']=='passed';pins(noise['source_sha256'])
    for mode in ['normal','asan_ubsan']:
        assert noise[mode]['checks']==13771587 and noise[mode]['cases']==24514 and noise[mode]['rounding_modes']==4 and noise[mode]['hot_allocations']==0
    assert noise['mutations_detected']==13
    helpers=load('noise-source/helper-audit/manifest.json');assert helpers['helper_count']==13 and helpers['total_words']==639
    ns=load('noise-source/sensitivity.json');assert ns['status']=='passed' and len(ns['mutations'])==13 and all(r['detected'] and r['exit_code']!=0 for r in ns['mutations']);pins(ns['source_sha256'])
    mutations=load('mutations/results.json');assert mutations['mutations']==14 and mutations['all_mutants_compiled'] and mutations['native_files_mutated']==0
    assert len(mutations['results'])==15 and mutations['results'][0]['exit_code']==0
    assert all(r['exit_code']!=0 for r in mutations['results'][1:])
    assert mutations['production_source_sha256']==sha(ROOT/'src/game/secondary_body49m.cpp') and mutations['test_sha256']==sha(ROOT/'tests/native/secondary_wrapper49m.cpp') and mutations['oracle_sha256']==sha(ROOT/'tools/gameplay/secondary_wrapper_oracle49m.h')
    composition=load('composition/summary.json');pins(composition['source_hashes'])
    assert composition['normal_passed'] and composition['asan_ubsan_passed'] and composition['normal_sanitized_trace_identical']
    assert composition['frames']==120 and composition['substeps']==270 and composition['native_hot_transitions']==2620 and composition['hot_allocations']==0 and composition['mutation_cases_rejected']==13 and composition['assertions']==469388
    assert sha(E/'trace/persistent-two-body49m.json')==composition['trace_sha256']==composition['sanitized_trace_sha256']
    cm=load('composition/mutation-sensitivity.json');assert cm['passed'] and cm['mutations_rejected']==13 and not cm['historical_sources_edited'];pins(cm['source_sha256'])
    common=load('composition/common-phase-identity.json');assert common['passed'] and len(common['common_phase_bodies'])==10 and not common['calls_old_public49K_API'] and not common['transient_false_second_body_fact'];pins(common['historical_bytes_identical'])
    review=load('independent-review/verification.json');assert review['status'] in ('pass','passed');pins(review['source_pins'])
    assert not review['blockers'] and review['normal']['status']==review['sanitized']['status']==review['bounded_normal']['status']=='passed'
    assert review['normal']['total_added_checks']==review['sanitized']['total_added_checks']==50627
    assert review['sanitized']['stdout_identical_to_normal'] and review['source_intervals']['count']==39 and review['source_intervals']['words']==3297
    assert review['bounded_normal']['cpu_count']==1 and review['bounded_normal']['address_space_limit_bytes']==67108864
    assert len(review['controls'])==2 and all(r['caught'] and r['exit_code']!=0 for r in review['controls'])
    packaging=load('packaging-review.json');assert packaging['status']=='passed' and not packaging['blocking_findings'];pins(packaging['reviewed_file_hashes'])
    regression=load('regression-summary.json');assert regression['status']=='passed' and regression['all_mode_outputs_identical']
    for mode in ['normal','sanitized']:
        row=regression['runs'][mode];assert row['suites']==26 and row['checks']==27451557 and row['tracked_inputs']==159 and row['inputs_unchanged']
        assert sha(E/f'final-{mode}/regression-report.json')==row['report_sha256']
        report=load(f'final-{mode}/regression-report.json');assert report['status']=='passed' and not report['changed_inputs'] and not report['errors']
        for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
        for test in row['tests']:assert test['stderr_bytes']==0 and sha(E/f'final-{mode}'/(test['name']+'.stdout'))==test['stdout_sha256']
        for n in ['coverage.json','trace.json']:
            assert sha(E/f'final-{mode}'/n)==row[n+'_sha256']
            assert (E/f'final-{mode}'/n).read_bytes()==(E/'final-normal'/n).read_bytes()
    footprint=load('footprint/summary.json');assert footprint['status']=='measured'
    assert footprint['record_bytes']['CoupledBodyState49M']==1580 and footprint['record_bytes']['CoupledNoiseContext49M']==2060 and footprint['record_bytes']['CoupledBodyParameters49M']==448
    assert footprint['sum_new_text_bytes']==19870 and footprint['sum_new_data_bytes']==footprint['sum_new_bss_bytes']==0 and footprint['maximum_individual_compiler_frame_bytes']==4624
    for n,r in footprint['objects'].items():assert sha(ROOT/'src/game'/(n+'.cpp'))==r['source_sha256']
    bench=load('benchmark/verification.json');assert bench['status']=='measured' and len(bench['results'])==10 and bench['cpu_affinity']==[0] and bench['normal_virtual_address_limit_bytes']==67108864;pins(bench['source_hashes'])
    assert all(r['allocations']==0 and r['iterations']==100000 for r in bench['results'])
    hashes={}
    for f in sorted(ROOT.rglob('*')):
        if not f.is_file() or f==E/'validation.json':continue
        n=f.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/'+n not in old_names or 'RacerRevengePC/'+n in ALLOWED:hashes[n]=sha(f)
    result={'checkpoint':'49M-SecondaryBodyAdvancement','status':'passed','validated_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
      'baseline':{'archive':a.baseline.name,'sha256':BASELINE_SHA,'bytes':121201615,'members_preserved':17239,'changed_baseline_files':changes,'library_file_id':'libfile_531b5d997ae881919b478b907be63848'},
      'normal_and_asan_ubsan':{'suites_each':26,'checks_each':27451557,'tracked_inputs_unchanged_each':159,'LeakSanitizer':'disabled; unavailable under executor ptrace'},
      'wrapper_proof':{'cases':108,'checks_with_coverage':43274,'frozen_words':2377,'visited_words':2144,'all_wrapper_words':540,'excluded_words_accounted':233,'native_hot_calls':324,'native_hot_allocations':0},
      'noise_proof':{'cases':24514,'checks':13771587,'host_rounding_modes':4,'projected_words':277,'executed_words':187,'excluded_words':78,'unreachable_words':12,'helper_audit_words':639,'cold_rng_draws':1791,'ready_rng_draws':0},
      'persistent_diagnostic':{'frames':120,'substeps':270,'secondary_advances':250,'live838_skips':20,'checks_with_trace':469388,'native_hot_transitions':2620,'native_hot_allocations':0,'trace_sha256':composition['trace_sha256']},
      'safe_mutation_controls':{'noise':13,'secondary_wrapper':14,'composition':13},
      'footprint':{'per_vehicle_state_bytes':1580,'shared_noise_rng_bytes':2060,'parameters_bytes':448,'new_text_bytes':19870,'data_bss_bytes':0,'maximum_individual_compiler_frame_bytes':4624},
      'scope':'Original ordinary/noisy secondary force wrapper, one-way49L constraint, explicit supplied body/contact stages, secondary48H before main48H, persistent input/timing/body state and feedback.',
      'remaining_boundaries':['Contact/query generation and non-nohit handling','Authored model/PHY initialization and live mode producers','Projected1-D noise only, shared scheduling/seeding supplied','Full vehicle scene/render assembly and unrepresented external effects','Target-console/gameplay performance','49C excluded/unreleased and collision/reset work unresolved'],
      'source_hashes':hashes}
    (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'status':'passed','baseline_members':17239,'changed_baseline_files':changes,'new_or_changed_files_hashed':len(hashes)}))
if __name__=='__main__':main()
