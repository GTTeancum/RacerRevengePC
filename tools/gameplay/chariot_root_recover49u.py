#!/usr/bin/env python3
"""Authenticate and reproduce complete admitted21A660 differential proof.

Default outputs are temporary. --output-dir is the only persistent evidence
destination. No runtime RAM, game launch, downloads or installation.
"""
import argparse, hashlib, json, os, re, struct, subprocess, tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'evidence/chariotRoot49U/root/source'
PRODUCTION=['src/game/chariot_root49u.cpp','src/game/chariot_scene49u.cpp',
 'src/game/scene_graph49p.cpp','src/game/body_scene48g.cpp','src/game/gradient_noise49m.cpp']
FLAGS=['-ffp-contract=off','-fno-fast-math','-Wall','-Wextra','-Werror','-Wno-misleading-indentation']
SOURCE_MUTANTS=[
 ('cold_speed_word',0x21a684,0x3c034300),
 ('phase_store_omitted',0x21a93c,0),
 ('previous_error_store_omitted',0x21af84,0),
 ('velocity_x_store_omitted',0x21b5c4,0),
 ('position_x_store_omitted',0x21b620,0),
 ('omega_x_store_omitted',0x21b67c,0),
 ('persistent_normalization_omitted',0x21b708,0),
 ('translation_noise_offset_2',0x21b7d4,0x3c024000),
 ('hidden_root_setter_omitted',0x21b99c,0),
 ('display_root_setter_omitted',0x21b9f4,0),
 ('transition_hidden_seed_omitted',0x21bf60,0),
 ('transition_wrong_relative_matrix',0x21bf7c,0x8e050190),
 ('transition_mode_store_omitted',0x21bfd8,0),
 ('final_probe_store_omitted',0x21c0fc,0),
 ('cold_rng_increment_changed',0x308050,0x2463303a),
]
NATIVE_MUTANTS=[
 ('derivative_sign','f.sub(s.previous85C,error)','f.sub(error,s.previous85C)'),
 ('no_position_integration','f.mul(dt, s.linearVelocity.v[i])','f.mul(0.0f, s.linearVelocity.v[i])'),
 ('phase_reset_each_call','phase=f.add(phase,f.mul(rate,in.frameFactor));','phase=f.mul(rate,in.frameFactor);'),
 ('zero_amplitude_skips_noise','    float value;\n    if (!f.valid || !evaluateGradientNoise49M','    if (amplitude == 0.0f) return true;\n    float value;\n    if (!f.valid || !evaluateGradientNoise49M'),
]
def sha(data):return hashlib.sha256(data).hexdigest()
def verify():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();m=json.loads((SOURCE/'source-manifest.json').read_text());assert sha(raw)==m['elf_sha256']
 po=struct.unpack_from('<I',raw,28)[0];ps,pn=struct.unpack_from('<HH',raw,42);segments=[struct.unpack_from('<8I',raw,po+i*ps)for i in range(pn)]
 def take(a,z):return next(raw[o+a-v:o+z-v]for k,o,v,_,n,*_ in segments if k==1 and v<=a<z<=v+n)
 header=(SOURCE/'root_words.h').read_text();words={}
 for b in m['blocks']:
  a,z=int(b['start'],16),int(b['end_exclusive'],16);data=take(a,z);assert sha(data)==b['sha256'];assert data==(SOURCE/(b['name']+'.bin')).read_bytes()
  match=re.search(r'\b'+b['name']+r'\[\]=\{(.*?)\};',header,re.S);assert match
  values=[int(w,16)for w in re.findall(r'0x([0-9a-f]+)u',match[1])];assert len(values)==b['words'];assert struct.pack('<%dI'%len(values),*values)==data
  rows=[tuple(int(w,16)for w in row.split()[:2])for row in (SOURCE/(b['name']+'.txt')).read_text().splitlines()];assert rows==[(a+4*i,w)for i,w in enumerate(values)]
  for pc,w in rows:assert pc not in words;words[pc]=w
 original=(ROOT/'evidence/vehicleVisual49R/source/visual_machine.h').read_text()
 expected=original.replace('"visual_words.h"','"root_words.h"').replace('visual49r_original','chariot49u_original').replace('visual49r_words','chariot49u_words').replace('"../../../work/','"../../../../work/')
 assert (SOURCE/'root_machine.h').read_text()==expected,'Original machine semantic preservation'
 assert sha(original.encode())==m['retained_machine']['sha256']
 assert len(words)==m['words'];assert sum(0x21a660<=pc<0x21c118 for pc in words)==1710
 return m,words
def coverage(path,m,words):
 counts={int(pc,16):(int(h),int(t),int(n))for pc,h,t,n in(line.split()for line in path.read_text().splitlines())};assert counts.keys()==words.keys()
 rows=[];classes={};root_unaccounted=[]
 for b in m['blocks']:
  block=[]
  for pc in range(int(b['start'],16),int(b['end_exclusive'],16),4):
   h,t,n=counts[pc];row=dict(pc=f'{pc:08x}',word=f'{words[pc]:08x}',hits=h,taken=t,not_taken=n)
   if not h:
    kind='unexercised_helper_branch';why='Retained complete helper, outside the explicitly exercised bounded fixtures; no universal helper coverage claim.'
    if b['name']=='root_response':
     ranges=[(0x21ba04,0x21beb4,'Both-mode blend path excluded; admitted modes are physical-only or virtual-only.'),(0x21beec,0x21bf4c,'Low-speed virtual-to-physical transition excluded by cachedSpeed>=effective434750; includes external vtable+AC callback.'),(0x21bfdc,0x21c0f4,'Nonzero vehicle58 selector route excluded by explicit normal58=0 admission.')]
     matches=[why for a,z,why in ranges if a<=pc<z]
     if matches:kind='excluded_root_mode';why=matches[0]
     else:root_unaccounted.append(pc);kind='unexercised_admitted_root_word';why='This admitted numeric word was not reached; proof is incomplete until covered.'
    elif b['name']=='root_next_boundary':kind='provenance_only';why='Padding and first next-function word establish21C118/21C120 boundary; not part of21A660.'
    elif b['name']=='shared_vector_initializer':kind='provenance_only';why='Shared vectors are supplied effective source state; initializer not called by21A660.'
    row.update(coverage_class=kind,reason=why);classes[kind]=classes.get(kind,0)+1
   if t+n:assert t+n==h;row['branch_directions']='both'if t and n else'single';row['branch_scope']='Directions are actual fixture observations; single does not mean universal unreachability.'
   block.append(row)
  rows.append(dict(name=b['name'],pcs=block))
 assert not root_unaccounted,'Uncovered admitted root words: '+','.join(hex(p)for p in root_unaccounted)
 return dict(frozen_words=len(words),reached_words=sum(h>0 for h,t,n in counts.values()),root_words=1710,root_reached=sum(h>0 for pc,(h,t,n)in counts.items()if 0x21a660<=pc<0x21c118),root_unaccounted=0,uncovered_classes=classes,blocks=rows)
def main():
 p=argparse.ArgumentParser();p.add_argument('--output-dir',type=Path);p.add_argument('--skip-sanitized',action='store_true');p.add_argument('--skip-mutations',action='store_true');args=p.parse_args();manifest,words=verify()
 with tempfile.TemporaryDirectory(prefix='chariot-root49u-')as temporary:
  base=Path(temporary);out=args.output_dir.resolve()if args.output_dir else base/'evidence';out.mkdir(parents=True,exist_ok=True);build=base/'build';build.mkdir()
  def save(name,obj):(out/name).write_text(json.dumps(obj,indent=2)+'\n')
  def run(cmd,name,env=None,expect=True):
   r=subprocess.run([str(x)for x in cmd],cwd=ROOT,capture_output=True,text=True,env=env);(out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr)
   if expect:assert r.returncode==0,f'{name}: {r.stdout}\n{r.stderr}'
   return r
  cxx=os.environ.get('CXX','c++');compiler=run([cxx,'--version'],'compiler').stdout.splitlines()[0]
  def compile_set(tag,traced=True,sanitized=False):
   flags=FLAGS+(['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie']if sanitized else['-O2','-g'])
   if traced:flags+=['-DRR_CHARIOT_ROOT49U_ENABLE_STAGE_TRACE=1','-include',SOURCE/'root_stage_trace.h']
   objects=[]
   for name in PRODUCTION:
    obj=build/(Path(name).stem+'-'+tag+'.o');run([cxx,'-std=c++17',*flags,'-c',ROOT/name,'-o',obj],'build-'+obj.stem);objects.append(obj)
   exe=build/('root-'+tag);run([cxx,'-std=c++20',*flags,*(['-no-pie']if sanitized else[]),ROOT/'tests/native/chariot_root49u.cpp',*objects,'-o',exe],'link-'+tag);return exe,objects,flags
  clean=dict(os.environ);clean.pop('RR_CHARIOT_ROOT49U_COVERAGE',None)
  exe,objects,flags=compile_set('normal');env=dict(clean,RR_CHARIOT_ROOT49U_COVERAGE=str(out/'coverage.tsv'))
  normal=json.loads(run([exe,ROOT],'normal',env).stdout);assert normal['passed']and normal['native_allocations']==0;save('normal.json',normal);save('coverage-accounting.json',coverage(out/'coverage.tsv',manifest,words))
  coverage_hash=sha((out/'coverage.tsv').read_bytes());again=json.loads(run([exe,ROOT],'default-read-only',clean).stdout);assert again==normal and sha((out/'coverage.tsv').read_bytes())==coverage_hash
  save('read-only-default.json',dict(passed=True,evidence_unchanged=True,explicit_output_only=True))
  plain,_,_=compile_set('plain',False);plain_result=json.loads(run([plain,ROOT],'plain',clean).stdout)
  assert {k:v for k,v in plain_result.items()if k!='checks'}=={k:v for k,v in normal.items()if k!='checks'};save('plain.json',plain_result)
  mutations=[]
  if not args.skip_mutations:
   for name,pc,w in SOURCE_MUTANTS:
    r=run([exe,ROOT,f'{pc:x}',f'{w:x}'],'mutant-'+name,clean,False);assert r.returncode!=0 and'mismatch'in r.stderr,(name,r.stderr)
    mutations.append(dict(name=name,kind='original_word',pc=f'{pc:08x}',word=f'{w:08x}',mismatch=r.stderr.strip()))
   source=(ROOT/PRODUCTION[0]).read_text()
   for name,before,after in NATIVE_MUTANTS:
    assert source.count(before)==1,(name,source.count(before));file=build/(name+'.cpp');file.write_text(source.replace(before,after));obj=build/(name+'.o')
    run([cxx,'-std=c++17',*flags,'-I',ROOT/'src/game','-c',file,'-o',obj],'mutant-build-'+name)
    binary=build/name;run([cxx,'-std=c++20',*flags,ROOT/'tests/native/chariot_root49u.cpp',obj,*objects[1:],'-o',binary],'mutant-link-'+name)
    r=run([binary,ROOT],'mutant-'+name,clean,False);assert r.returncode!=0 and'mismatch'in r.stderr,(name,r.stderr)
    mutations.append(dict(name=name,kind='isolated_native_change',before=before,after=after,mismatch=r.stderr.strip()))
  save('mutations.json',dict(count=len(mutations),detected=mutations,production_edited=False,setup_failures_counted=0))
  sanitized=None
  if not args.skip_sanitized:
   san,_,_=compile_set('sanitized',True,True);envsan=dict(clean,ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
   sanitized=json.loads(run([san,ROOT],'asan-ubsan',envsan).stdout);assert sanitized==normal;save('sanitized.json',sanitized)
  files=[Path(__file__),ROOT/'tests/native/chariot_root49u.cpp',*[ROOT/x for x in PRODUCTION],*[x for x in SOURCE.iterdir()if x.is_file()]]
  summary=dict(passed=True,scope='Complete admitted physical-to-virtual seed and stable virtual21A660 through21C110/114; source checks normal58=40=0, high speed, no blend or reverse callback.',elf_sha256=manifest['elf_sha256'],compiler=compiler,production_language='C++17',test_language='C++20',flags=[str(x)for x in FLAGS],normal=normal,plain=plain_result,sanitized=sanitized,leak_sanitizer='disabled; no LeakSanitizer claim',mutations=len(mutations),source_substitutions=0,files={str(f.relative_to(ROOT)):sha(f.read_bytes())for f in files})
  save('validation.json',summary);print(json.dumps(dict(passed=True,cases=normal['cases'],checks=normal['checks'],sanitized=sanitized is not None,mutations=len(mutations),output=str(out)if args.output_dir else'temporary')))
if __name__=='__main__':main()
