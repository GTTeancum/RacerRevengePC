#!/usr/bin/env python3
"""Reproduce the bounded49Q engine-flow proof. Never rewrites49O/49P evidence."""
import argparse, hashlib, json, os, struct, subprocess, sys, tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'evidence/engineAnimation49Q/flow'
ASSETS=ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures'
PRODUCTION=['src/game/engine_animation49q.cpp','src/game/model_motion49q.cpp','src/game/scene_graph49p.cpp','src/assets/model_geometry49o.cpp','src/assets/model_bindings49o.cpp']
FLAGS=['-fno-fast-math','-ffp-contract=off','-Wall','-Wextra','-Werror']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def save(name,data):(OUT/name).write_text(json.dumps(data,indent=2)+'\n')
def run(cmd,env=None):
 p=subprocess.run(cmd,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,env=env)
 return p

def verify_sources():
 manifest=json.loads((OUT/'source/source-word-manifest.json').read_text())
 fixture=ROOT/'evidence/authoredGeometry49O/motion/fixture.bin';memory=fixture.read_bytes()
 assert len(memory)==0x800000
 frozen=json.loads((ROOT/'evidence/authoredGeometry49O/motion/motion-proof-manifest.json').read_text())['artifacts']
 immutable_inputs=[fixture,*[ROOT/f'evidence/authoredGeometry49O/motion/trig/{x}-table.bin' for x in ('sin','cos','acos')]]
 for file in immutable_inputs:assert sha(file)==frozen[str(file.relative_to(ROOT))]['sha256']
 single=json.loads((ROOT/'evidence/engineAnimation49Q/single-motion/source-manifest.json').read_text())
 for rec in single['blocks']:
  start=int(rec['start'],16);end=int(rec['end_exclusive'],16)
  assert hashlib.sha256(memory[start:end]).hexdigest()==rec['sha256']
 records=[]
 for rec in manifest['slices']:
  file=OUT/'source'/f"{rec['name']}.bin"
  if not file.exists():continue
  assert sha(file)==rec['sha256']
  start=int(rec['start'],16);end=int(rec['end_exclusive'],16)
  assert memory[start:end]==file.read_bytes(),f"source slice differs from immutable49O fixture: {file}"
  records.append(rec)
 # Reauthenticate every used asset and the exact selected MCF record set.
 selected=[]
 for rec in json.loads((ROOT/'evidence/authoredGeometry49O/asset-reader/expected-members.json').read_text()):
  p=ASSETS/rec[4];assert sha(p)==rec[5];selected.append({'file':rec[4],'sha256':rec[5],'bytes':p.stat().st_size})
 mcfs={p.name:p.read_text() for p in ASSETS.glob('*.mcf')}
 assert len(mcfs)==3
 # Exact authored names and source lookup absence are checked below, not guessed.
 all_text='\n'.join(mcfs.values()).lower()
 assert '01_engineleftmid' not in all_text and '01_enginerightmid' not in all_text
 assert all_text.count('01_engineleft')==1 and all_text.count('01_engineright')==1
 names=[line.split('=',1)[1].strip().lower() for text in mcfs.values() for line in text.splitlines() if line.startswith('NAME=')]
 assert sorted(names)==sorted(['01_engineleft','01_engineright','01_brakeleft','01_brakeright','01_forward','01_back'])
 save('source-manifest.json',{'elf_sha256':manifest['elf_sha256'],'source_ranges':records,'fixture':{'path':str(fixture.relative_to(ROOT)),'sha256':sha(fixture),'bytes':len(memory)},'assets':selected,'frozen_inputs':[{'path':str(p.relative_to(ROOT)),'sha256':sha(p)} for p in immutable_inputs],'single_motion_ranges_verified':len(single['blocks']),'notes':['Original21A000 stops before21A400; warm disabled source stops before21A648.','All normal original words are unchanged. Allocation success is a supplied initializer convention; allocator implementation/failure is excluded.','Source1559F0 and scene operations are supplied by the independent single-motion49Q original-word machine.']})
 save('selection-proof.json',{'mcfs':mcfs,'engine_mid_records_absent':True,'chariot_mid_record_absent':True,'admission':'Only authentic Pod01 selected records; no foreign matching record or replacement. Source1523F0(a2=0) exhausted lookup returns0; exact selection and lookup ranges are pinned.','motion_clock':'MOT duration3FAAAAAB and21 authored keys per track; MCF FPS/FrameCount are not the sampling clock.','runtime_order':'Chariot BrakeLeft, BrakeRight; left2A0, then right29C; engineMid294/298 absent; stop before separate Chariot paired/IK suffix.'})

def coverage():
 rows=[tuple(int(x,16 if i==0 else 10) for i,x in enumerate(line.split())) for line in (OUT/'coverage.tsv').read_text().splitlines()]
 exclusions=[(0x21a1bc,0x21a220,'Chariot Mid motion288 absent in selected authentic Pod01 records'),(0x21a32c,0x21a390,'Right engine Mid294 absent; unexpected engineMid rejected by native context guard'),(0x21a39c,0x21a400,'Left engine Mid298 absent; unexpected engineMid rejected by native context guard'),(0x21a400,0x21a648,'Separate Chariot paired/IK suffix excluded; distinct unparented roots and confined binding rows prevent engine-subtree writes'),(0x21a648,0x21a660,'Epilogue outside observed state boundary; disabled route stops before21A648')]
 missing=[pc for pc,h,_,_ in rows if not h and not any(a<=pc<z for a,z,_ in exclusions)]
 assert not missing,missing
 executed=[pc for pc,h,_,_ in rows if h]
 branches=[{'pc':f'{pc:08x}','taken':t,'not_taken':n}for pc,h,t,n in rows if t+n]
 expected_both=[0x21a018,0x21a038,0x21a0dc,0x21a0ec,0x21a0f8,0x21a104,0x21a224,0x21a230,0x21a240,0x21a2a4,0x21a2b0,0x21a2c0]
 for pc in expected_both:assert any(p==pc and t>0 and n>0 for p,h,t,n in rows)
 save('coverage-accounting.json',{'total_source_words':len(rows),'executed_unique_words':len(executed),'executed_dynamic_instructions_in21A000':sum(h for _,h,_,_ in rows),'admitted_unexecuted_words':missing,'exclusions':[{'start':f'{a:08x}','end_exclusive':f'{z:08x}','words':(z-a)//4,'reason':why}for a,z,why in exclusions],'branches':branches,'every_dynamic_gate_both_directions':True,'initializer':'Original2134A0..213544, with two supplied successful20-byte allocations; outputs+0/+4/+8 compared, bounds+C/+10 checked and excluded from runtime inputs. No allocation implementation/failure investigation.'})

def main():
 parser=argparse.ArgumentParser();parser.add_argument('--skip-sanitized',action='store_true');args=parser.parse_args();verify_sources()
 with tempfile.TemporaryDirectory(prefix='engine49q-') as tmp:
  tmp=Path(tmp);cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version']).stdout.splitlines()[0];objects=[]
  for file in PRODUCTION:
   obj=tmp/(Path(file).stem+'.o');cmd=[cxx,'-std=c++17','-O2',*FLAGS,'-c',file,'-o',str(obj)];p=run(cmd);assert p.returncode==0,p.stdout+p.stderr;objects.append(str(obj))
  env_coverage=dict(os.environ,RR_ENGINE49Q_COVERAGE=str(OUT/'coverage.tsv'))
  exe=tmp/'normal';cmd=[cxx,'-std=c++20','-O2',*FLAGS,'tests/native/engine_animation49q.cpp',*objects,'-o',str(exe)];p=run(cmd);assert p.returncode==0,p.stdout+p.stderr
  p=run([str(exe),str(ROOT)],env_coverage);(OUT/'normal.log').write_text(p.stdout+p.stderr);assert p.returncode==0,p.stdout+p.stderr;normal=json.loads(p.stdout);assert normal['all_match'] and normal['native_allocations']==0 and normal['real_chariot_prefix_effect_cases']>0;save('normal.json',normal);coverage()
  before=sha(OUT/'coverage.tsv');clean_env=dict(os.environ);clean_env.pop('RR_ENGINE49Q_COVERAGE',None)
  readonly=run([str(exe),str(ROOT)],clean_env);assert readonly.returncode==0 and json.loads(readonly.stdout)==normal,readonly.stderr
  assert sha(OUT/'coverage.tsv')==before
  save('read-only-default.json',{'default_run_passed':True,'coverage_unchanged':True,'argv1':'optional repository root','coverage_requires':'RR_ENGINE49Q_COVERAGE explicit output path'})
  mutants=[('cold_enable_literal_zero',0x21a020,0x24030000),('left_alpha_multiply_instead_divide',0x21a06c,0x46001002),('left_difference_add_instead_subtract',0x21a07c,0x46020840),('left_mirror_wrong_store',0x21a08c,0xe6000b0c),('right_target_wrong_channel',0x21a09c,0xc46100e4),('left_bit_gate_always_off',0x21a23c,0x30630000),('right_bit_gate_always_off',0x21a2bc,0x30630000),('right_upper_clamp_half',0x21a2e4,0x3c023f00),('left_sample_uses_right_mirror',0x21a294,0xc60c0b0c),('left_sample_wrong_model',0x21a28c,0x8e0401a4),('right_sample_wrong_model',0x21a30c,0x8e0401a8),('Chariot_left_brake_reads_right_mirror',0x21a18c,0xc60c0b0c)]
  results=[]
  # Chariot-only palette mutation is intentionally outside represented native
  # state; it is a scope control, not falsely advertised as native failure.
  for name,pc,word in mutants:
   p=run([str(exe),str(ROOT),f'{pc:x}',f'{word:x}']);actual_mismatch=any(s in p.stderr for s in ['mismatch','source sample count','source left/right/prefix call order','filter store order'])
   if name=='Chariot_left_brake_reads_right_mirror':
    assert p.returncode==0,p.stderr
    results.append({'name':name,'pc':f'{pc:08x}','replacement':f'{word:08x}','scope_control':True,'result':'Engine projection correctly unchanged by a Chariot-only sampled fraction change; source Chariot still executes.'})
   else:
    assert p.returncode!=0 and actual_mismatch,(name,p.returncode,p.stderr)
    results.append({'name':name,'pc':f'{pc:08x}','replacement':f'{word:08x}','detected':True,'actual_mismatch':p.stderr.strip()})
  save('mutation-controls.json',{'mutations':results,'all_represented_mutants_detected_by_actual_state_or_trace_mismatch':True,'invalid_context_failures_counted':0})
  # Restore normal coverage after the successful Chariot-only scope control.
  p=run([str(exe),str(ROOT)],env_coverage);assert p.returncode==0,p.stderr;coverage()
  sanitized=None
  if not args.skip_sanitized:
   objects=[]
   for file in PRODUCTION:
    obj=tmp/(Path(file).stem+'-san.o');cmd=[cxx,'-std=c++17','-O1','-g',*FLAGS,'-fsanitize=address,undefined','-fno-omit-frame-pointer','-c',file,'-o',str(obj)];p=run(cmd);assert p.returncode==0,p.stdout+p.stderr;objects.append(str(obj))
   exe=tmp/'san';cmd=[cxx,'-std=c++20','-O1','-g',*FLAGS,'-fsanitize=address,undefined','-fno-omit-frame-pointer','tests/native/engine_animation49q.cpp',*objects,'-o',str(exe)];p=run(cmd);assert p.returncode==0,p.stdout+p.stderr
   env=dict(env_coverage,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1');p=run([str(exe),str(ROOT)],env);(OUT/'asan-ubsan.log').write_text(p.stdout+p.stderr);assert p.returncode==0,p.stdout+p.stderr;sanitized=json.loads(p.stdout);save('sanitized.json',sanitized);assert normal==sanitized
  save('validation.json',{'status':'passed','compiler':compiler,'production_language':'C++17','test_language':'C++20','flags':FLAGS,'normal':normal,'asan_ubsan':sanitized,'leak_sanitizer':'disabled, not claimed','mutations':'11 actual mismatches plus1 intentionally out-of-projection Chariot control','files':{str(p.relative_to(ROOT)):sha(p)for p in [ROOT/'tests/native/engine_animation49q.cpp',OUT/'flow_fixture.h',ROOT/'tools/gameplay/engine_animation_recover49q.py',ROOT/'evidence/engineAnimation49Q/single-motion/motion_machine.h',*[ROOT/x for x in PRODUCTION]]}})
 print(json.dumps({'status':'passed','cases':normal['cases'],'checks':normal['checks'],'sanitized':sanitized is not None,'output':str(OUT.relative_to(ROOT))}))
if __name__=='__main__':main()
