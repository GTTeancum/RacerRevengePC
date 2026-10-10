#!/usr/bin/env python3
"""Validate49O source, authoring, exact-word proofs, regressions and preservation."""
import argparse,datetime,hashlib,json,re,struct,subprocess,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/authoredGeometry49O'
BASELINE_SHA='f669690e4e36961579c49da95cfaf89e2cda45f6bbb3767f5183b655d88bafd2'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
EXPECTED_SUITES={'tv_camera49e', 'coupled_body_advancement49m', 'secondary_body_api49m', 'authored_body_diagnostic49n', 'body_advancement49k', 'model_bounds49o', 'player_events49a', 'event_dispatch48z', 'gradient_noise49m', 'control_shaping48v', 'advancement_selectors49k', 'camera_frame49f', 'model_bindings49o', 'clock_quantization49j', 'physics48d', 'authored_parameters_api49n', 'body_scene48g', 'pod_physics49n', 'frame_timing49i', 'authored_body_diagnostic49o', 'device_held48y', 'authored_geometry49o', 'physics48c', 'body_wrapper_nearest49k', 'player_input48x', 'pod_model_geometry49o', 'secondary_wrapper49m', 'authored_parameters49n', 'control_filter48b', 'pod_sphere_constraint49l', 'physics48e', 'model_geometry49o', 'clock_timing_diagnostics49j', 'model_motion49o', 'pod_sphere_constraint_api49l', 'control_binding48w', 'control_presets49d'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(n):return json.loads((E/n).read_text())
def pins(rows):
 for n,h in rows.items():assert sha(ROOT/n)==h,n

def main():
 if not __debug__:raise RuntimeError('Validation requires assertions')
 ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);a=ap.parse_args()
 assert a.baseline.stat().st_size==127165107 and sha(a.baseline)==BASELINE_SHA
 changes=[]
 with zipfile.ZipFile(a.baseline)as old:
  names={i.filename for i in old.infolist()if not i.is_dir()};assert len(names)==17772
  for n in sorted(names):
   p=ROOT.parent/n;assert p.is_file(),n
   if sha(p)!=hashlib.sha256(old.read(n)).hexdigest():changes.append(n)
 assert set(changes)==ALLOWED,changes
 elf=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert hashlib.sha256(elf).hexdigest()==ELF_SHA
 phoff=struct.unpack_from('<I',elf,28)[0];size,count=struct.unpack_from('<HH',elf,42);segments=[struct.unpack_from('<8I',elf,phoff+i*size)for i in range(count)]
 def original(a,n):
  for k,o,v,_,z,*_ in segments:
   if k==1 and v<=a and a+n<=v+z:return elf[o+a-v:o+a-v+n]
  raise AssertionError('Non-file-backed original range')
 proof=load('independent-review/provenance.json')
 assert len(proof['original_blocks'])==38 and proof['verified_member_hashes']==15 and proof['table_words_verified']==2561 and proof['trig_source_words_verified']==1723
 assert proof['baseline_production_unchanged_count']==162 and not proof['baseline_production_changed']
 for b in proof['original_blocks']:assert hashlib.sha256(original(int(b['start'],16),b['bytes'])).hexdigest()==b['sha256']
 for row in load('independent-review/reviewed-sources.json'):assert sha(ROOT/row['path'])==row['sha256'],row['path']
 independent=load('independent-review/mutation-review.json');assert len(independent)==6 and all(x['detected']and x['exit']!=0 for x in independent)
 asset=load('asset-reader/asset-source-manifest.json');assert len(asset['members'])==15
 for row in [asset['container'],asset['decoder'],asset['elf']]:assert sha(ROOT/row['path'])==row['sha256']
 for row in asset['members']:assert sha(E/'asset-reader'/row['fixture'])==row['sha256']
 av=load('asset-reader/validation.json');assert av['normal']=='passed' and av['checks']==85404 and av['mutation_controls_rejected']==9
 for row in av['production']:assert sha(ROOT/row['path'])==row['sha256']
 for args in [['evidence/authoredGeometry49O/asset-reader/recover_asset_evidence49o.py','--check'],['evidence/authoredGeometry49O/bindings/prove_bindings49o.py'],['evidence/authoredGeometry49O/connected/verify_mechanical_copy49o.py']]:
  run=subprocess.run([sys.executable,'-B',*args],cwd=ROOT,capture_output=True,text=True);assert run.returncode==0,(args,run.stdout,run.stderr)
 binding=load('bindings/validation.json');assert binding['status']=='passed' and len(binding['controls'])==5 and all(x['detected']for x in binding['controls']);pins(binding['source_hashes'])
 motion=load('motion/motion-proof-manifest.json');assert motion['production_tables_words']==2561 and motion['production_tables_match_original_software_execution'] and motion['numeric_mutations_detected']==10
 assert motion['normal_and_sanitized']['all_match'] and motion['normal_and_sanitized']['cases']==1207 and motion['normal_and_sanitized']['native_allocations']==0
 assert motion['source_coverage']['unaccounted_words']==0 and motion['source_coverage']['unaccounted_branch_scopes']==0
 for n,h in motion['artifacts'].items():assert sha(ROOT/n)==h['sha256'],n
 selection=load('motion/selection-source.json')
 for b in selection['slices']:assert hashlib.sha256(original(int(b['start'],16),int(b['end'],16)-int(b['start'],16))).hexdigest()==b['sha256']
 init=load('initialization/proof-manifest.json')
 for b in list(init['blocks'].values())+list(init['provenance'].values()):assert hashlib.sha256(original(int(b['start'],16),int(b['end'],16)-int(b['start'],16))).hexdigest()==b['sha256']
 numeric=load('initialization/final-validation.json');assert numeric['status']=='passed' and numeric['source_word_coverage']==[272,272];pins(numeric['source_hashes'])
 bounds=load('bounds/native-test-result.json');assert bounds['status']=='pass' and len(bounds['sensitivity'])==6 and all(x['detected']for x in bounds['sensitivity'])
 finalBounds=load('bounds/final-validation.json');assert finalBounds['status']=='passed' and finalBounds['signed_zero_correction_verified'];pins(finalBounds['source_hashes'])
 composition=load('composition/proof-manifest.json');assert composition['status']=='pass' and composition['semantic_controls_detected']==8 and composition['normal_and_asan_ubsan_equal']['all_words_match'];pins(composition['sha256'])
 connected=load('connected/summary.json');assert connected['passed']and connected['repeat_trace_identical']and connected['asan_ubsan_trace_identical'];pins(connected['source_sha256'])
 assert connected['scenarios']==14 and connected['frames']==1680 and connected['substeps']==3780 and connected['secondary_steps']==3500 and connected['live838_skips']==280
 assert connected['original49N_starting_state_admitted_unchanged']and not connected['additional_stabilizing_clamps_or_state_changes']and connected['hot_allocations']==0 and connected['finite_traced_float_words']==191618
 assert sha(E/'connected/persistent-authored-two-body49o.json')==connected['trace_sha256']
 regression=load('regression-summary.json');assert regression['status']=='passed'and regression['all_mode_outputs_identical']
 for mode in ['normal','sanitized']:
  row=regression['runs'][mode];assert row['suites']==37 and row['tracked_inputs']==228 and row['inputs_unchanged'] and row['checks']==regression['checks_each']
  report=load(f'final-{mode}/regression-report.json');assert report['status']=='passed'and not report['changed_inputs']and not report['errors'];assert sha(E/f'final-{mode}/regression-report.json')==row['report_sha256']
  assert report['scope']=='all_authored-geometry_checks'
  flags=['-std=c++20','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off']
  flags+=['-O2']if mode=='normal'else['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all']
  assert report['compile_flags']==flags
  assert report['link_flags']==([]if mode=='normal'else['-fsanitize=address,undefined'])
  assert report['sanitizers']==(mode=='sanitized') and not report['leak_detection']
  assert len(report['input_hashes'])==228 and len(report['tests'])==37
  assert len(report['selected_tests'])==37 and set(report['selected_tests'])==EXPECTED_SUITES
  assert {t['name']for t in report['tests']}==EXPECTED_SUITES
  assert {t['name']for t in row['tests']}=={t['name']for t in report['tests']}
  assert len(report['build'])==34 and all(t['status']=='passed'and t['returncode']==0 for t in report['build'])
  assert all(t['status']=='passed'and t['build']['returncode']==0 and t['run']['returncode']==0 for t in report['tests'])
  assert len(row['tests'])==37 and {t['name']for t in row['tests']}==EXPECTED_SUITES
  total=0
  for test in row['tests']:
   text=(E/f'final-{mode}'/(test['name']+'.stdout')).read_text()
   try:checks=int(json.loads(text)['checks'])
   except (ValueError,KeyError):
    match=re.search(r'checks=(\d+)',text)or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',text)
    assert match,test['name'];checks=int(match.group(1))
   assert checks==test['checks'];total+=checks
  assert total==row['checks']==40325214
  for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
  for test in row['tests']:assert test['stderr_bytes']==0 and sha(E/f'final-{mode}'/(test['name']+'.stdout'))==test['stdout_sha256']
  for n,h in row['replayed_files'].items():assert sha(E/f'final-{mode}'/n)==h
 assert regression['runs']['normal']['tests']==regression['runs']['sanitized']['tests']
 assert regression['runs']['normal']['replayed_files']==regression['runs']['sanitized']['replayed_files']
 footprint=load('footprint/summary.json');assert footprint['status']=='measured'and footprint['caller_owned_load_records_bytes']==21236 and footprint['readonly_trig_table_bytes']==10244;pins(footprint['source_hashes'])
 bench=load('benchmark/verification.json');assert bench['status']=='measured'and bench['cpu_affinity']==[0]and bench['normal_virtual_address_limit_bytes']==67108864 and bench['samples_per_mode']==15;pins(bench['source_hashes'])
 packaging=load('packaging-review.json');assert packaging['status']=='passed'and not packaging['blocking_findings'];pins(packaging['reviewed_file_hashes'])
 hashes={}
 for p in sorted(ROOT.rglob('*')):
  if not p.is_file()or p==E/'validation.json':continue
  n=p.relative_to(ROOT).as_posix()
  if 'RacerRevengePC/'+n not in names or 'RacerRevengePC/'+n in ALLOWED:hashes[n]=sha(p)
 result={'checkpoint':'49O-AuthoredPodGeometry','status':'passed','verified_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'baseline_members':17772,'changed_baseline_members':changes,'regression_suites_each':37,'regression_checks_each':regression['checks_each'],'tracked_inputs_each':228,'normal_sanitizer_outputs_identical':True,'source_hashes':hashes,'49c_status':'excluded and unreleased','collision_contract':'unresolved and unchanged','limits':'Fresh authored model numeric projection with explicit arithmetic/table/source context and supplied physical pose/history/contact effects.'}
 (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items()if k!='source_hashes'}))
if __name__=='__main__':main()
