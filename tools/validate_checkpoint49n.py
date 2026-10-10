#!/usr/bin/env python3
"""Validate49N authoring bytes, source proofs, native runs, review and baseline."""
import argparse,datetime,hashlib,json,subprocess,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/authoredInitialization49N'
BASELINE_SHA='c272ffc3dcd2098b88dd715a4107c0bbd5928b9e7b111be805679ecfa6d0d1b3'
PHY_SHA='47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b'
RES_SHA='08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads((E/p).read_text())
def pins(m):
    for n,h in m.items():assert sha(ROOT/n)==h,n

def main():
    if not __debug__:raise RuntimeError('Validation requires assertions')
    ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);a=ap.parse_args()
    assert a.baseline.stat().st_size==122349098 and sha(a.baseline)==BASELINE_SHA
    changes=[]
    with zipfile.ZipFile(a.baseline) as old:
        old_names={i.filename for i in old.infolist() if not i.is_dir()};assert len(old_names)==17501
        for n in sorted(old_names):
            p=ROOT.parent/n;assert p.is_file(),n
            if sha(p)!=hashlib.sha256(old.read(n)).hexdigest():changes.append(n)
    assert set(changes)==ALLOWED,changes
    assert sha(E/'authoring-source/Pod01.phy')==PHY_SHA
    assert sha(ROOT/'work/native/project/disc/PODS/POD01/POD01.RES')==RES_SHA
    asset=load('asset-provenance.json');assert asset['container_sha256']==RES_SHA and asset['phy_sha256']==PHY_SHA and asset['phy_logical_offset']==1130496 and asset['phy_bytes']==6584
    extraction=load('resource-extraction.json');assert extraction['status']=='passed' and len(extraction['results'])==2;pins(extraction['source_hashes'])
    assert all(r['build_returncode']==r['run_returncode']==0 and r['member_sha256']==PHY_SHA and r['exact_original_fixture'] for r in extraction['results'])
    commands=[['tools/gameplay/authored_parameters_recover49n.py','--check'],['tools/gameplay/pod_physics_recover49n.py'],['evidence/authoredInitialization49N/connected/verify_mechanical_copy49n.py']]
    for command in commands:
        r=subprocess.run([sys.executable,'-B',*command],cwd=ROOT,capture_output=True,text=True)
        assert r.returncode==0,(command,r.stdout,r.stderr)
    reader=load('authoring-source/focused-validation.json');assert reader['normal']==reader['asan_ubsan'] and '6133 checks' in reader['normal'];pins(reader['source_sha256'])
    assert reader['source_replay']=={'cases':2585,'instructions':3291006,'visited_pcs':566,'verified_files':32,'mode':'verify'}
    manifest=load('authoring-source/source-manifest.json');assert manifest['phy_sha256']==PHY_SHA and manifest['phy_bytes']==6584 and len(manifest['native_scalar_projection'])==61
    mutations=load('authoring-source/sensitivity.json');assert mutations['production_unchanged'] and mutations['source_sha256']==sha(ROOT/'src/assets/pod_physics49n.cpp')
    assert len(mutations['mutations'])==5 and all(r['exit_code']!=0 for r in mutations['mutations'])
    numeric=load('numeric-source/checks/summary.json');assert numeric['normal']==numeric['ASan_UBSan']=='PASS'
    assert numeric['cases']==297 and numeric['checks']==18537 and numeric['native_hot_calls']==297 and numeric['native_hot_allocations']==0 and numeric['original_instructions']==187701
    assert numeric['selected_original_words']==1381 and numeric['visited_original_words']==1375 and numeric['explicitly_excluded_words']==6 and numeric['unique_boundary_contracts']==122 and numeric['explicit_reader_boundaries']==12293
    for key,path in [('source_sha256','src/game/authored_parameters49n.cpp'),('oracle_sha256','tools/gameplay/authored_parameters_oracle49n.h'),('test_sha256','tests/native/authored_parameters49n.cpp'),('fixture_bridge_sha256','evidence/authoredInitialization49N/numeric-source/authored_fixture49n.h')]:assert numeric[key]==sha(ROOT/path),path
    coverage=load('numeric-source/checks/normal-coverage.json');pcs=[p for b in coverage['blocks'] for p in b['pcs']]
    assert len(pcs)==1381 and sum(p['hits']>0 for p in pcs)==1375 and all(p.get('exclusion') for p in pcs if not p['hits'])
    bounds=load('numeric-source/checks/normal-coverage.json.boundaries.json');assert bounds['opaque_game_callbacks_executed']==0 and len(bounds['contracts'])==122 and sum(c['calls'] for c in bounds['contracts'])==12293
    nm=load('numeric-source/mutations/summary.json');assert nm['all_detected'] and not nm['production_files_modified'] and len(nm['mutations'])==11
    assert all(r['compile_returncode']==0 and r['run_returncode']!=0 and r['detected'] for r in nm['mutations'])
    assert nm['production_sha256']==sha(ROOT/'src/game/authored_parameters49n.cpp')
    connected=load('connected/summary.json');assert connected['passed'] and connected['repeat_trace_identical'] and connected['asan_ubsan_trace_identical'];pins(connected['source_sha256'])
    assert connected['frames']==1320 and connected['substeps']==2970 and connected['secondary_steps']==2750 and connected['live838_skips']==220 and connected['checks']==5165776 and connected['hot_transitions']==28897 and connected['hot_allocations']==0
    assert connected['finite_float_words_checked']==150007 and sha(E/'connected/persistent-authored-two-body49n.json')==connected['trace_sha256']
    mechanical=load('connected/mechanical-copy-proof.json');assert mechanical['passed'] and mechanical['source_loop_preserved_with_substitutions'] and len(mechanical['substitutions'])==4
    assert sha(ROOT/mechanical['historical_test'])==mechanical['historical_test_sha256'] and sha(ROOT/'tests/native/authored_body_diagnostic49n.cpp')==mechanical['connected_test_sha256']
    review=load('independent-review/summary.json');assert review['status']=='pass' and not review['blockers'] and review['normal_and_sanitized_identical'] and review['strict_cpp17'] and review['asan'] and review['ubsan']
    assert review['checks_per_native_run']==347103 and review['zero_allocation_native_calls_per_run']==30283 and review['decimal_cases']==4804 and review['host_rounding_modes']==4 and review['original_parameter_routes']==56 and review['demanded_input_probes']==10976
    for n,h in review['review_files'].items():assert sha(E/'independent-review'/n)==h,n
    identity=load('independent-review/source-identity.json');assert identity['unchanged_prior_production_files']==168 and identity['source_blocks_verified']==41;pins(identity['new_production'])
    decimals=load('independent-review/decimal-controls.json');assert decimals['padding_variants']==4 and decimals['original_conversions']==19216 and decimals['source_instruction_count']==22372496
    limited=load('independent-review/single-cpu-64mib.json');assert limited['exit_code']==0 and limited['cpu_count']==1 and limited['address_space_limit_bytes']==67108864 and not limited['stderr']
    rm=load('independent-review/mutations.json');assert rm['all_detected'] and len(rm['safe_mutations'])==5 and all(r['build_exit']==0 and r['test_exit']!=0 and r['detected'] for r in rm['safe_mutations'])
    suffix=load('independent-review/profile-boundary.json');assert suffix['status']=='pass' and not suffix['represented_parameter_writes'] and not suffix['indirect_calls'] and len(suffix['calls'])==7
    assert sha(E/'independent-review/profile-suffix.bin')==suffix['sha256']
    regression=load('regression-summary.json');assert regression['status']=='passed' and regression['all_mode_outputs_identical']
    for mode in ['normal','sanitized']:
        row=regression['runs'][mode];assert row['suites']==30 and row['checks']==32644848 and row['tracked_inputs']==177 and row['inputs_unchanged']
        assert sha(E/f'final-{mode}/regression-report.json')==row['report_sha256']
        report=load(f'final-{mode}/regression-report.json');assert report['status']=='passed' and not report['changed_inputs'] and not report['errors']
        for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
        for test in row['tests']:assert test['stderr_bytes']==0 and sha(E/f'final-{mode}'/(test['name']+'.stdout'))==test['stdout_sha256']
        for n in ['coverage.json','trace.json']:
            assert sha(E/f'final-{mode}'/n)==row[n+'_sha256'] and (E/f'final-{mode}'/n).read_bytes()==(E/'final-normal'/n).read_bytes()
        assert sha(E/f'final-{mode}/coverage.json.boundaries.json')==row['boundaries_sha256']
    footprint=load('footprint/summary.json');assert footprint['status']=='measured' and footprint['sum_new_text_bytes']==22448 and footprint['sum_new_data_bytes']==1480 and footprint['sum_new_bss_bytes']==0 and footprint['maximum_individual_compiler_frame_bytes']==5232
    for n,r in footprint['objects'].items():assert sha(ROOT/'src'/(n+'.cpp'))==r['source_sha256']
    assert footprint['record_bytes']['AuthoredParameters49N']==564 and footprint['record_bytes']['AuthoredBinding49N']==484 and footprint['record_bytes']['PodPhysicsProvenance49N']==816
    bench=load('benchmark/verification.json');assert bench['status']=='measured' and len(bench['results'])==5 and bench['cpu_affinity']==[0] and bench['normal_virtual_address_limit_bytes']==67108864;pins(bench['source_hashes'])
    assert all(r['iterations']==5000 and r['allocations']==0 for r in bench['results'])
    geometry=load('geometry-assessment/preservation.json');assert geometry['production_consumers']==0
    for n,r in geometry['files'].items():assert sha(E/'geometry-assessment'/n)==r['sha256']
    packaging=load('packaging-review.json');assert packaging['status']=='passed' and not packaging['blocking_findings'];pins(packaging['reviewed_file_hashes'])
    hashes={}
    for p in sorted(ROOT.rglob('*')):
        if not p.is_file() or p==E/'validation.json':continue
        n=p.relative_to(ROOT).as_posix()
        if 'RacerRevengePC/'+n not in old_names or 'RacerRevengePC/'+n in ALLOWED:hashes[n]=sha(p)
    result={'checkpoint':'49N-AuthoredPodParameters','status':'passed','validated_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
      'baseline':{'archive':a.baseline.name,'sha256':BASELINE_SHA,'bytes':122349098,'members_preserved':17501,'changed_baseline_files':changes,'library_file_id':'libfile_b835cbb6378081919b3d3818c579efad'},
      'normal_and_asan_ubsan':{'suites_each':30,'checks_each':32644848,'tracked_inputs_unchanged_each':177,'LeakSanitizer':'disabled; unavailable under executor ptrace'},
      'authored_resource':{'resource_bytes':349110,'resource_sha256':RES_SHA,'phy_bytes':6584,'phy_sha256':PHY_SHA,'mapped_scalars':61,'required_vectors':2},
      'reader':{'decimal_cases':2585,'checks':6133,'original_instructions':3291006,'zero_allocation_native_calls':2851,'malformed_domain_rejections':232,'mutations_detected':5},
      'numeric_projection':{'cases':297,'checks_no_output':18535,'checks_with_coverage_boundaries':18537,'source_words':1381,'visited':1375,'excluded':6,'original_instructions':187701,'parsed_reader_contracts':122,'boundary_calls':12293,'mutations_detected':11},
      'connected':{'scenarios':11,'frames':1320,'substeps':2970,'secondary_advances':2750,'live838_skips':220,'checks_with_trace':5165776,'original_instructions':20323828,'zero_allocation_native_calls':28897,'trace_sha256':connected['trace_sha256']},
      'independent_review':{'checks_each':347103,'native_calls_without_allocation':30283,'decimal_cases':4804,'host_modes':4,'defined_stack_patterns':4,'original_conversions':19216,'decimal_instructions':22372496,'source_blocks_verified':41,'independent_mutations_detected':5},
      'footprint':{'authored_record_bytes':564,'binding_bytes':484,'optional_provenance_bytes':816,'new_text_bytes':22448,'constant_schema_relocation_data_bytes':1480,'bss_bytes':0,'maximum_individual_compiler_frame_bytes':5232},
      'scope':'Actual bounded PHY reader and selected source-ordered main parameter-write projection feeding49M with explicit track/preset/profile context. No complete original loader or vehicle-construction claim.',
      'remaining_boundaries':['Optional215FD4 callback and other unmapped loader effects','Live profile/track discovery and hardware input','Initial body poses/history, secondary motion-derived bounds/inertia, unproved damping130','Contact/query/reset and full gameplay/renderer assembly','Target-console build/performance','49C excluded/unreleased; flagged work untouched'],
      'source_hashes':hashes}
    (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'status':'passed','baseline_members':17501,'changed_baseline_files':changes,'new_or_changed_files_hashed':len(hashes)}))
if __name__=='__main__':main()
