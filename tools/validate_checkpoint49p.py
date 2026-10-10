#!/usr/bin/env python3
"""Validate49P exact source, bounded scene proofs, full regressions and baseline."""
import argparse,datetime,hashlib,json,re,struct,subprocess,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/sceneProjection49P'
BASELINE_SHA='e4819740af694cf436dfa03cb4a026481ea57e1d480c912e0de55fdff76a0bbe'
ELF_SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
ALLOWED={'RacerRevengePC/CMakeLists.txt','RacerRevengePC/README.md'}
EXPECTED_SUITES=set('scene_graph_api49p scene_graph49p chariot_projection49p scene_frame_diagnostic49p model_geometry49o model_bindings49o model_motion49o model_bounds49o authored_geometry49o pod_model_geometry49o authored_body_diagnostic49o pod_physics49n authored_parameters_api49n authored_parameters49n authored_body_diagnostic49n gradient_noise49m secondary_body_api49m secondary_wrapper49m coupled_body_advancement49m pod_sphere_constraint_api49l pod_sphere_constraint49l advancement_selectors49k body_advancement49k physics48c physics48d physics48e body_scene48g body_wrapper_nearest49k clock_quantization49j clock_timing_diagnostics49j frame_timing49i camera_frame49f tv_camera49e control_presets49d player_events49a event_dispatch48z device_held48y player_input48x control_binding48w control_shaping48v control_filter48b'.split())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(n):return json.loads((E/n).read_text())
def pins(rows):
 for n,h in rows.items():assert sha(ROOT/n)==h,n

def main():
 if not __debug__:raise RuntimeError('Validation requires assertions')
 ap=argparse.ArgumentParser();ap.add_argument('--baseline',type=Path,required=True);a=ap.parse_args()
 assert a.baseline.stat().st_size==132031768 and sha(a.baseline)==BASELINE_SHA
 changes=[]
 with zipfile.ZipFile(a.baseline)as old:
  names={i.filename for i in old.infolist()if not i.is_dir()};assert len(names)==18203
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
 check=subprocess.run([sys.executable,'-B','tools/gameplay/scene_graph_recover49p.py','--check'],cwd=ROOT,capture_output=True,text=True)
 assert check.returncode==0,(check.stdout,check.stderr)
 assert json.loads(check.stdout)['words']==542
 api=load('api/proof-manifest.json');assert api['tests']['normal']==api['tests']['asan_ubsan']=='PASS'and api['tests']['hot_allocations']==0
 for row in api['files']:assert sha(ROOT/row['path'])==row['sha256']
 for mode in ['normal','sanitized']:
  source=load(f'source/{mode}/native-test-result.json');assert source['status']=='pass'and source['source_words_verified']==542 and source['static_boundary_ranges_verified']==5
  assert source['original_instruction_accounting']['unaccounted_words_or_directions']==0 and source['native_hot_allocations']==0
  assert len(source['sensitivity'])==10 and all(x['detected']for x in source['sensitivity'])
  assert '18757 exact ordered flag stores'in source['native_result']
  assert source['mode']==('normal'if mode=='normal'else'ASan+UBSan; leak detection disabled')
  command=source['command']
  assert all(flag in command for flag in ['-std=c++17','-ffp-contract=off','-fno-fast-math','-include'])
  assert '-ffast-math'not in command and '-ffp-contract=fast'not in command
  assert ('-fsanitize=address,undefined'in command)==(mode=='sanitized')
  assert Path(command[command.index('-include')+1]).name=='flag_trace.h'
  coverage=load(f'source/{mode}/coverage.json')
  assert coverage['ordered_store_tracing']is True and coverage['flag_stores_compared']==18757
  assert coverage['fixtures']==586 and coverage['operations']==14141 and coverage['hot_allocations']==0
  assert coverage['instructions']==862000 and coverage['accounting']['unaccounted_words_or_directions']==0
 assert 'native missing flag store'in(E/'source/missing-hook.stderr').read_text()
 chariot=load('chariot/controls.json');assert chariot['all_detected']and len(chariot['controls'])==16
 assert chariot['native_source_sha256']==sha(ROOT/'src/game/chariot_projection49p.cpp')
 for block in load('chariot/context-manifest.json')['blocks']:
  start=int(block['start'],16);end=int(block['end_exclusive'],16);assert hashlib.sha256(original(start,end-start)).hexdigest()==block['sha256']
 review=load('independent-review/review-manifest.json');assert review['sanitizer_results_identical']
 assert sum(b['words']for b in review['source_blocks'])==542
 for b in review['source_blocks']:assert b['header_and_listing_equal_elf']and hashlib.sha256(original(int(b['start'],16),4*b['words'])).hexdigest()==b['sha256']
 for row in review['reviewed_files']+review['unchanged49K49M']:assert sha(ROOT/row['path'])==row['sha256']
 assert len(review['unchanged49K49M'])==10 and all(x['exact49O_equal']for x in review['unchanged49K49M'])
 observed=review['observed_results'];assert observed['native_operations']==8812 and observed['checks']==10316668 and observed['negative_controls']==9 and observed['native_hot_allocations']==0
 connected=load('connected/summary.json');assert connected['passed']and connected['normal_repeat_sanitized_trace_identical']and connected['readonly_normal_sanitized_pass']and connected['readonly_invocation_writes_no_files']
 assert connected['frames']==1680 and connected['substeps']==3780 and connected['prephysics_copies']==1680 and connected['compared_states']==46074 and connected['hot_allocations']==0
 assert connected['readonly_checks']==37918853 and connected['file_output_checks']==37918854
 assert sha(E/'connected/persistent-scene-frames49p.json')==connected['trace_sha256']
 assert sha(E/'connected/supplied-physical-scenes49p.bin')==connected['fixture_sha256']
 assert sha(E/'connected/exact-goldens49p.txt')==connected['exact_goldens_sha256']
 inputs=load('connected/input-manifest.json')
 for key in ['body_trace','compact_fixture','hierarchy']:
  row=inputs[key];assert sha(ROOT/row['path'])==row['sha256']
 assert connected['retained_guard_files_verified_against49O_zip']
 for k in ['wrong_postphysics_copy_changed_frames','wrong_postphysics_copy_changed_substeps','wrong_eager_world_changed_frames','wrong_added2F0_changed_frames','wrong_cached_world_copy_changed_frames']:assert connected[k]>0
 regression=load('regression-summary.json');assert regression['status']=='passed'and regression['all_mode_outputs_identical']and regression['checks_each']==124535876
 for mode in ['normal','sanitized']:
  row=regression['runs'][mode];assert row['suites']==41 and row['tracked_inputs']==243 and row['inputs_unchanged']and row['checks']==124535876
  report=load(f'final-{mode}/regression-report.json');assert report['status']=='passed'and not report['changed_inputs']and not report['errors'];assert sha(E/f'final-{mode}/regression-report.json')==row['report_sha256']
  assert report['scope']=='all_scene-projection_checks'
  flags=['-std=c++20','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off']
  flags+=['-O2']if mode=='normal'else['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all']
  assert report['compile_flags']==flags and report['link_flags']==([]if mode=='normal'else['-fsanitize=address,undefined'])
  assert report['sanitizers']==(mode=='sanitized')and not report['leak_detection']
  assert len(report['input_hashes'])==243 and len(report['tests'])==41
  assert len(report['selected_tests'])==41 and set(report['selected_tests'])==EXPECTED_SUITES and {t['name']for t in report['tests']}==EXPECTED_SUITES
  assert len(report['build'])==36 and all(t['status']=='passed'and t['returncode']==0 for t in report['build'])
  assert all(t['status']=='passed'and t['build']['status']=='passed'and t['run']['status']=='passed'and t['build']['returncode']==0 and t['run']['returncode']==0 for t in report['tests'])
  if mode=='sanitized':
   assert 'detect_leaks=0'in report['environment']['ASAN_OPTIONS']and 'halt_on_error=1'in report['environment']['ASAN_OPTIONS']
   assert 'halt_on_error=1'in report['environment']['UBSAN_OPTIONS']
  for build in report['build']+[t['build']for t in report['tests']]:
   command=build['command'];assert command[1:1+len(flags)]==flags
   assert not any(flag in command for flag in ['-Ofast','-ffast-math','-fno-rounding-math','-ffp-contract=fast'])
  assert len(row['tests'])==41 and {t['name']for t in row['tests']}==EXPECTED_SUITES
  total=0
  for test in row['tests']:
   path=E/f'final-{mode}'/(test['name']+'.stdout');text=path.read_text()
   actual=next(t for t in report['tests']if t['name']==test['name'])['run']
   assert sha(path)==actual['stdout']['sha256']==test['stdout_sha256']
   assert path.stat().st_size==actual['stdout']['bytes']
   assert actual['stderr']['bytes']==0 and actual['stderr']['sha256']==hashlib.sha256(b'').hexdigest()
   try:checks=int(json.loads(text)['checks'])
   except (ValueError,KeyError):
    match=re.search(r'checks=(\d+)',text)or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',text);assert match;checks=int(match.group(1))
   assert checks==test['checks']and test['stderr_bytes']==0 and sha(path)==test['stdout_sha256'];total+=checks
  assert total==124535876
  for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
  for n,h in row['replayed_files'].items():assert sha(E/f'final-{mode}'/n)==h
  assert load(f'final-{mode}/scene-coverage.json')['ordered_store_tracing']is False
 assert regression['runs']['normal']['tests']==regression['runs']['sanitized']['tests']
 assert regression['runs']['normal']['replayed_files']==regression['runs']['sanitized']['replayed_files']
 footprint=load('footprint/summary.json');assert footprint['status']=='measured'and footprint['caller_owned_22_node_storage_bytes']==3168 and footprint['caller_owned_maximum_64_node_storage_bytes']==9216
 assert footprint['totals']['executable_text_bytes']==2542 and footprint['totals']['readonly_data_bytes']==20 and footprint['totals']['data_bytes']==footprint['totals']['bss_bytes']==0;pins(footprint['source_hashes'])
 bench=load('benchmark/verification.json');assert bench['status']=='measured'and bench['cpu_affinity']==[0]and bench['normal_virtual_address_limit_bytes']==67108864 and bench['samples_per_mode']==15;pins(bench['source_hashes'])
 assessment=load('assessment/preservation.json');assert len(assessment['files'])==11
 for row in assessment['files']:assert sha(E/'assessment'/row['path'])==row['sha256']
 packaging=load('packaging-review.json');assert packaging['status']=='passed'and not packaging['blocking_findings'];pins(packaging['reviewed_file_hashes'])
 hashes={}
 for p in sorted(ROOT.rglob('*')):
  if not p.is_file()or p==E/'validation.json':continue
  n=p.relative_to(ROOT).as_posix()
  if 'RacerRevengePC/'+n not in names or 'RacerRevengePC/'+n in ALLOWED:hashes[n]=sha(p)
 result={'checkpoint':'49P-SceneProjection','status':'passed','verified_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'baseline_members':18203,'changed_baseline_members':changes,'regression_suites_each':41,'regression_checks_each':124535876,'tracked_inputs_each':243,'normal_sanitizer_outputs_identical':True,'source_hashes':hashes,'49c_status':'excluded and unreleased','collision_contract':'unresolved and unchanged','limits':'Fixed-node scene and pre-physics Chariot projection with explicit supplied producer state; old body-domain gates unchanged; no spawn or complete visual-system claim.'}
 (E/'validation.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:v for k,v in result.items()if k!='source_hashes'}))
if __name__=='__main__':main()
