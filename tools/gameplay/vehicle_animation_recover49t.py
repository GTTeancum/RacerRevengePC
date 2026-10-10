#!/usr/bin/env python3
"""Verify complete admitted21A000, with read-only default source/CLI behavior.

Production builds use C++17. The original-word test uses C++20 for the retained
EE-root reference. Outputs are temporary unless --output-dir is explicit.
"""
import argparse,hashlib,json,os,re,struct,subprocess,tempfile
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
EVIDENCE=ROOT/'evidence/chariotAnimation49T/flow'
SHA='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
SOURCES=['src/game/vehicle_animation49t.cpp','src/game/model_motion49t.cpp',
 'src/game/chariot_ik49t.cpp','src/game/source_acos49t.cpp','src/game/scene_graph49p.cpp',
 'src/assets/model_geometry49o.cpp','src/assets/model_bindings49o.cpp']
SOURCE_CONTROLS=[
 ('omit_left_filter_value','21a088','00000000'),
 ('skip_chariot_brakes','21a0f8','1000002d'),
 ('use_body_velocity','21a464','24460070'),
 ('pose_filter_max_step','21a520','4600a228'),
 ('omit_pair','21a618','00000000'),
 ('omit_left_ik','21a62c','00000000'),
 ('omit_right_ik','21a640','00000000'),
 ('wrong_left_hand_target','21a620','8e0501cc'),
]

def sha(b):return hashlib.sha256(b).hexdigest()
def verify():
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes();assert sha(raw)==SHA
 po=struct.unpack_from('<I',raw,28)[0];ps,pn=struct.unpack_from('<HH',raw,42)
 segs=[struct.unpack_from('<8I',raw,po+i*ps)for i in range(pn)]
 def take(a,z):
  return next(raw[o+a-v:o+z-v]for k,o,v,_,n,*_ in segs if k==1 and v<=a<z<=v+n)
 total=0
 for relative in ['flow/source','ik/source']:
  d=ROOT/'evidence/chariotAnimation49T'/relative;m=json.loads((d/'source-manifest.json').read_text());assert m['elf_sha256']==SHA
  for b in m['blocks']:
   a,z=int(b['start'],16),int(b['end_exclusive'],16);data=take(a,z)
   assert sha(data)==b['sha256'] and (d/(b['name']+'.bin')).read_bytes()==data,b['name'];total+=len(data)//4
  if relative.startswith('flow'):
   for row in m['data']:
    data=take(int(row['address'],16),int(row['address'],16)+4)
    assert sha(data)==row['sha256'] and struct.unpack('<I',data)[0]==int(row['word'],16)
   header=(d/'flow_words.h').read_text()
   for b in m['blocks']:
    body=re.search(r'uint32_t '+b['name']+r'\[\]=\{(.*?)\};',header,re.S).group(1)
    words=[int(x,16)for x in re.findall(r'0x([0-9a-f]+)u',body)]
    assert struct.pack('<%dI'%len(words),*words)==take(int(b['start'],16),int(b['end_exclusive'],16))
 assets=json.loads((ROOT/'evidence/authoredGeometry49O/asset-reader/asset-source-manifest.json').read_text())
 for a in assets['members']:
  assert sha((ROOT/'evidence/authoredGeometry49O/asset-reader'/a['fixture']).read_bytes())==a['sha256']
 return dict(elf_sha256=SHA,verified_flow_and_ik_words=total,assets_verified=len(assets['members']))

def account(path):
 report=json.loads(path.read_text());rows=report['pcs'];missing=branches=both=0
 mids=[(0x21a1bc,0x21a220,'actual Pod01 Chariot Mid record absent'),
       (0x21a32c,0x21a390,'actual Pod01 right-engine Mid record absent'),
       (0x21a39c,0x21a400,'actual Pod01 left-engine Mid record absent')]
 one={0x21a1b4:mids[0][2],0x21a324:mids[1][2],0x21a394:mids[2][2]}
 assert [r['pc']for r in rows]==list(range(0x21a000,0x21a65c,4))
 for r in rows:
  p=r['pc']
  if not r['hits']:
   r['unreached_reason']=next((why for a,z,why in mids if a<=p<z),None)
   assert r['unreached_reason'],f'unaccounted PC {p:08x}';missing+=1
  if r['taken'] or r['not_taken']:
   branches+=1;assert r['taken']+r['not_taken']==r['hits']
   if r['taken'] and r['not_taken']:both+=1
   else:assert p in one,f'unaccounted one-sided branch {p:08x}';r['one_sided_reason']=one[p]
 report['accounting']=dict(frozen_words=len(rows),reached_words=len(rows)-missing,
   unreached_words_with_specific_reason=missing,branch_pcs=branches,both_directions=both,
   one_sided_with_specific_reason=branches-both,unaccounted_words_or_branches=0)
 path.write_text(json.dumps(report,indent=2)+'\n');return report['accounting']

def execute(cmd,out,name,env=None,expect=0):
 r=subprocess.run([str(x)for x in cmd],cwd=ROOT,text=True,capture_output=True,env=env)
 (out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr)
 assert r.returncode==expect,f'{name}: exit{r.returncode}\n{r.stdout[-2000:]}\n{r.stderr[-5000:]}'
 return r

def run(args,out):
 out.mkdir(parents=True,exist_ok=True);manifest=verify();build=out/'build';build.mkdir(exist_ok=True)
 flags=['-O2','-g','-Wall','-Wextra','-Werror','-pedantic','-ffp-contract=off','-fno-fast-math','-I',ROOT/'src/game']
 if args.sanitizers:flags+=['-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
 if not args.plain:flags+=['-include',EVIDENCE/'stage_trace.h']
 cxx=os.environ.get('CXX','g++');objects=[]
 for src in SOURCES:
  obj=build/(Path(src).stem+'.o');execute([cxx,'-std=c++17',*flags,'-c',ROOT/src,'-o',obj],out,'compile_'+obj.stem);objects.append(obj)
 test=build/'test.o';execute([cxx,'-std=c++20',*flags,'-c',ROOT/'tests/native/vehicle_animation49t.cpp','-o',test],out,'compile_test')
 exe=build/'vehicle_animation49t';execute([cxx,*flags,test,*objects,'-o',exe],out,'link')
 env=dict(os.environ)
 if args.sanitizers:env['ASAN_OPTIONS']='detect_leaks=0:halt_on_error=1';env['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
 env['RR_VEHICLE_ANIMATION49T_COVERAGE']=str(out/'coverage.json')
 result=execute([exe,ROOT],out,'test',env);summary=json.loads(result.stdout);coverage=account(out/'coverage.json')
 env.pop('RR_VEHICLE_ANIMATION49T_COVERAGE')
 controls=[]
 if not args.skip_controls:
  for name,pc,word in SOURCE_CONTROLS:
   r=execute([exe,ROOT,pc,word],out,'source_'+name,env,1)
   assert 'FAIL vehicle_animation49t' in r.stderr
   controls.append(dict(name=name,kind='original word',pc=pc,replacement=word,diagnostic=r.stderr.strip()))
  src=(ROOT/SOURCES[0]).read_text()
  native=[
   ('omit_brake_samples','const bool brake=chariot&&','const bool brake=false&&chariot&&'),
   ('ignore_chariot_suffix_gate','const bool suffix=chariot&&!in.chariot1B0;','const bool suffix=chariot;'),
   ('filter_clamp_feedback','mirror=s.value0;return f.valid;','mirror=s.value0;s.value0=clamp(s.value0);return f.valid;'),
   ('body_local_instead_of_cached_world','].world100,in.bodyAccelerationC0,f)','].localC0,in.bodyAccelerationC0,f)'),
   ('swap_pair_time_weight','*b.chariotRows,w.state.poseB1C,w.state.blendB14,','*b.chariotRows,w.state.blendB14,w.state.poseB1C,'),
   ('omit_ik','if(suffix&&(b.hands.left.target!=Index::none||b.hands.right.target!=Index::none))','if(false&&suffix&&(b.hands.left.target!=Index::none||b.hands.right.target!=Index::none))'),
   ('reset_shared_orientation','w.visual=visual;','w.visual=visual;w.visual.orientationSentinel4433D8={};'),
   ('wrong_B18_initial_seed','s.poseB18={left,value(0x3dcccccd),1};','s.poseB18={s.poseB1C,value(0x3dcccccd),1};'),
  ]
  variants=[]
  for name,before,after in native:
   assert src.count(before)==1,(name,src.count(before));variants.append((name,src.replace(before,after)))
  if not args.plain:
   begin=src.index('    if(brake){');end=src.index('    if(pair){',begin);section=src[begin:end]
   split=section.index('    if(left)');wrong=section[split:]+section[:split]
   variants.append(('engines_before_chariot_brakes',src[:begin]+wrong+src[end:]))
  for name,text in variants:
   path=build/(name+'.cpp');path.write_text(text);obj=build/(name+'.o');mutant=build/name
   execute([cxx,'-std=c++17',*flags,'-c',path,'-o',obj],out,'compile_native_'+name)
   execute([cxx,*flags,test,obj,*objects[1:],'-o',mutant],out,'link_native_'+name)
   r=execute([mutant,ROOT],out,'native_'+name,env,1);assert 'FAIL vehicle_animation49t' in r.stderr
   controls.append(dict(name=name,kind='native semantic control',diagnostic=r.stderr.strip()))
 report=dict(status='pass',mode='ASan+UBSan; LSan disabled'if args.sanitizers else'normal',
   stage_trace=not args.plain,source=manifest,result=summary,instruction_accounting=coverage,
   controls=controls,source_controls_detected=sum(c['kind']=='original word'for c in controls),
   native_controls_detected=sum(c['kind']=='native semantic control'for c in controls),
   production_standard='C++17',original_test_standard='C++20',
   scope='Complete admitted21A000 internal animation with supplied root/scene/body/control context. No21A660/root copy, later physics, noise, game launch or RAM. Plain mode retains complete final-state source comparisons.')
 (out/'result.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))

if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--output-dir',type=Path);p.add_argument('--sanitizers',action='store_true');p.add_argument('--plain',action='store_true');p.add_argument('--skip-controls',action='store_true');a=p.parse_args()
 if a.check:print(json.dumps(verify()))
 elif a.output_dir:run(a,a.output_dir.resolve())
 else:
  with tempfile.TemporaryDirectory(prefix='vehicle-animation49t-')as t:run(a,Path(t))
