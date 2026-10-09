#!/usr/bin/env python3
"""Read-only default49T exact-word sampler proof.

python -B tools/gameplay/model_motion_recover49t.py
Uses a temporary build/report directory, no hooks by default, and never rewrites
retained49O/49P/49Q evidence. --trace enables test-only ordered-effect hooks.
--output-dir explicitly saves a new report. --mutations runs sensitivity tests.
"""
import argparse
import ast
import hashlib
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'evidence/chariotAnimation49T/motion'
OLD = ROOT / 'evidence/authoredGeometry49O/motion'
SOURCE_MUTATIONS = [
 ('omit_single_model18_zero','155a34','00000000'),
 ('single_time_add','155adc','4600a500'),
 ('omit_pair_model18_zero','155ed0','00000000'),
 ('omit_pair_lazy_value','155ec4','00000000'),
 ('omit_interpolation_lazy_value','14fd44','00000000'),
 ('omit_pair_lazy_guard','155ecc','00000000'),
 ('omit_interpolation_lazy_guard','14fd4c','00000000'),
 ('rotation_omit_fourth_zero','28a634','00000000'),
 ('position_omit_z','28ac38','00000000'),
 ('rotation_ready_gate_inverted','28a6b4','1460000b'),
 ('position_ready_gate_inverted','28ac44','1460000b'),
 ('ancestor_or_to_and','28c508','00661824'),
 ('skip_sibling_invalidation','289a34','10000014'),
 ('pair_position_subtract_to_add','156130','46000800'),
]
NATIVE_MUTATIONS = [
 ('reverse_pair_rotation','q=e.blend(b,q,weight);','q=e.blend(q,b,weight);'),
 ('pair_endpoint_shortcut','q=e.blend(b,q,weight);','q=weight==0?b:weight==1?q:e.blend(b,q,weight);'),
 ('pair_position_sign','p[lane]=e.sub(b[lane],e.mul(weight,e.sub(b[lane],p[lane])))','p[lane]=e.add(b[lane],e.mul(weight,e.sub(b[lane],p[lane])))'),
 ('omit_model18_zero','stagedState.model18=0;','stagedState.model18=1;'),
 ('refresh_world_cache','std::memcpy(graph.nodes,workspace.nodes,bytes);','std::memcpy(graph.nodes,workspace.nodes,bytes);graph.nodes[0].world100=graph.nodes[0].localC0;'),
 ('reverse_track_channels','for(std::uint32_t channel=0;channel<2;++channel){','for(std::uint32_t order=0;order<2;++order){const auto channel=1-order;'),
 ('overwrite_translation_fourth','e.rotationMatrix(q,local);','e.rotationMatrix(q,local);local.v[15]=1;'),
 ('drop_scene_effects','if(!setLocalMatrix49P(staged,target,local))','if((void(staged),workspace.nodes[static_cast<unsigned>(target)].localC0=local,false))'),
 ('warm_interpolation_clobber','if(!shared.interpolationGuard3D5D8C){','if(true){'),
 ('warm_pair_clobber','if(second&&!stagedShared.pairGuard3D5DC4){','if(second){'),
 ('pair_time_from_second','e.mul(motion.duration,fraction)','e.mul(second?second->duration:motion.duration,fraction)'),
 ('position_final_copy','p[i]=add(b.position[i],mul(u,sub(b.position[i],a.position[i])))','p[i]=b.position[i]'),
]

def sha(data): return hashlib.sha256(data).hexdigest()

def verify():
 manifest=json.loads((SOURCE/'source-manifest.json').read_text())
 raw=(ROOT/'work/native/project/disc/SLUS_202.68').read_bytes()
 assert sha(raw)==manifest['elf_sha256']
 ph=struct.unpack_from('<I',raw,28)[0];size,count=struct.unpack_from('<HH',raw,42)
 segments=[struct.unpack_from('<8I',raw,ph+i*size)for i in range(count)]
 fixture=(ROOT/manifest['fixture']['path']).read_bytes();assert sha(fixture)==manifest['fixture']['sha256']
 listing={int(l.split()[0],16):int(l.split()[1],16)for l in (SOURCE/'source-disassembly.txt').read_text().splitlines()}
 bounds=[(int(a,16),int(z,16))for a,z in re.findall(r'\{0x([a-f0-9]+),0x([a-f0-9]+)\}',(SOURCE/'blocks.h').read_text())]
 assert bounds==[(int(b['start'],16),int(b['end_exclusive'],16))for b in manifest['blocks']]
 words={}
 for b in manifest['blocks']:
  a,z=int(b['start'],16),int(b['end_exclusive'],16)
  data=next(raw[o+a-v:o+z-v]for k,o,v,_,n,*_ in segments if k==1 and v<=a<z<=v+n)
  assert sha(data)==b['sha256'] and len(data)==b['words']*4 and fixture[a:z]==data
  for i,w in enumerate(struct.unpack('<%dI'%b['words'],data)):
   p=a+i*4;assert listing[p]==w and p not in words;words[p]=w
 assert len(words)==manifest['word_count']==len(listing)
 assert struct.unpack_from('<I',fixture,0x3cd420)[0]==0x145780
 assert struct.unpack_from('<I',fixture,0x3cd484)[0]==0x144eb0
 assert struct.unpack_from('<I',fixture,0x3e3dd0)[0]==0
 for name,pin in manifest['retained_inputs'].items():
  data=(ROOT/name).read_bytes();assert len(data)==pin['bytes'] and sha(data)==pin['sha256'],name+' retained input changed'
 return manifest,words

def conditional(w):
 op=w>>26
 # Decode the instruction, not observed counters: includes BLEZ/BGTZ, REGIMM,
 # branch-likely and COP1. A missing interpreter counter cannot hide a branch.
 return op in (1,4,5,6,7,20,21,22,23) or (op==17 and ((w>>21)&31)==8)

def account(path,words):
 report=json.loads(path.read_text())
 tree=ast.parse((OLD/'account_coverage.py').read_text())
 old={n.targets[0].id:ast.literal_eval(n.value)for n in tree.body if isinstance(n,ast.Assign) and isinstance(n.targets[0],ast.Name) and n.targets[0].id in ('excluded','branch')}
 exclusions=[(a,z,r)for a,z,r in old['excluded'] if a not in (0x28a6bc,0x28ac4c)]
 exclusions += [
 (0x155a5c,0x155ad0,'Out-of-range single fraction diagnostic; admitted finite fraction0..1.'),
 (0x155ae8,0x155b10,'Single rotation exclusion-mask path; mask=null admission.'),
 (0x155b3c,0x155c18,'Single mirrored rotation; mirror=false admission.'),
 (0x155c70,0x155c9c,'Single position exclusion-mask path/duplicate setup; mask=null admission.'),
 (0x155cc8,0x155df4,'Single mirrored position; mirror=false admission.'),
 (0x289b18,0x289b20,'Alignment after descendant invalidation return and delay slot.'),
 ]
 restrictions=dict(old['branch'])
 for pc in (0x155eb4,0x28a6b4,0x28ac44):restrictions.pop(pc,None)
 restrictions.update({0x155a38:'fraction>=0 admission',0x155a54:'fraction<=1 admission',0x155ae0:'mask=null admission',0x155b34:'mirror=false admission',0x155c68:'mask=null admission',0x155cc0:'mirror=false admission'})
 reached=branches=both=0;assert {r['pc']for r in report['pcs']}==set(words)
 for row in report['pcs']:
  p=row['pc'];w=words[p];row['word']='%08x'%w;isbranch=conditional(w);row['conditional_branch']=isbranch
  if not row['hits']:
   row['unreached_reason']=next((r for a,z,r in exclusions if a<=p<z),None)
   assert row['unreached_reason'],'unaccounted original PC %08x'%p
   continue
  reached+=1
  if isbranch:
   branches+=1;assert row['taken']+row['not_taken']==row['hits'],'missing branch counters %08x'%p
   if row['taken'] and row['not_taken']:both+=1
   elif w>>26==4 and (w>>21&31)==(w>>16&31):row['one_sided_reason']='Unconditional BEQ with identical registers.'
   else:
    assert p in restrictions,'unaccounted branch direction %08x'%p
    row['one_sided_reason']=restrictions[p]
  else:assert row['taken']==row['not_taken']==0,'nonbranch counted %08x'%p
 report['accounting']=dict(frozen_words=len(words),reached_words=reached,unreached_words_with_specific_reason=len(words)-reached,reached_branches=branches,both_directions=both,one_sided_with_specific_reason=branches-both,conditional_decode_includes_BGTZ=True,selected_BGTZ_words=sum(w>>26==7 for w in words.values()),unaccounted_words_or_branch_directions=0)
 path.write_text(json.dumps(report,indent=2)+'\n');return report

def execute(cmd,out,name,env=None,expect=0):
 r=subprocess.run([str(x)for x in cmd],cwd=ROOT,env=env,text=True,capture_output=True)
 (out/(name+'.stdout')).write_text(r.stdout);(out/(name+'.stderr')).write_text(r.stderr)
 assert r.returncode==expect,'%s exit%d:\n%s\n%s'%(name,r.returncode,r.stdout,r.stderr)
 return r

def run(args,build):
 manifest,words=verify();build.mkdir(parents=True,exist_ok=True)
 out=args.output_dir.resolve() if args.output_dir else build/'report';out.mkdir(parents=True,exist_ok=True)
 flags=['-std=c++17','-O2','-g','-Wall','-Wextra','-Werror','-pedantic','-ffp-contract=off','-fno-fast-math']
 if args.sanitizers:flags+=['-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
 if args.trace:flags+=['-include',str(SOURCE/'flag_trace.h')]
 def compile(exe,src):
  cmd=[os.environ.get('CXX','g++'),*flags,'-I',ROOT/'src/game',ROOT/'tests/native/model_motion49t.cpp',src,ROOT/'src/game/scene_graph49p.cpp',ROOT/'src/assets/model_geometry49o.cpp',ROOT/'src/assets/model_bindings49o.cpp','-o',exe]
  execute(cmd,out,'compile_'+exe.name);return cmd
 exe=build/'model_motion49t';cmd=compile(exe,ROOT/'src/game/model_motion49t.cpp')
 env=dict(os.environ);env['RR_MODEL49T_COVERAGE']=str(out/'coverage.json')
 if args.sanitizers:env['ASAN_OPTIONS']='detect_leaks=0:halt_on_error=1';env['UBSAN_OPTIONS']='halt_on_error=1:print_stacktrace=1'
 result=execute([exe,ROOT],out,'model_motion49t',env);coverage=account(out/'coverage.json',words);env.pop('RR_MODEL49T_COVERAGE')
 controls=[]
 if args.mutations:
  for name,pc,word in SOURCE_MUTATIONS:
   r=execute([exe,ROOT,pc,word],out,'source_'+name,env,1)
   assert 'mismatch' in r.stderr,name+': '+r.stderr
   controls.append(dict(name=name,kind='original-word substitution',pc=pc,replacement=word,detected=True,diagnostic=r.stderr.strip()))
  src=(ROOT/'src/game/model_motion49t.cpp').read_text()
  for name,before,after in NATIVE_MUTATIONS:
   assert src.count(before)==1,name+' anchor count'
   mutant=build/(name+'.cpp');mutant.write_text(src.replace(before,after));mutantExe=build/name;compile(mutantExe,mutant)
   r=execute([mutantExe,ROOT],out,'native_'+name,env,1)
   assert 'mismatch' in r.stderr or 'native sample rejected' in r.stderr or 'invalid pair admitted' in r.stderr,name+': '+r.stderr
   controls.append(dict(name=name,kind='native semantic control',detected=True,diagnostic=r.stderr.strip()))
 report=dict(status='pass',mode='ASan+UBSan (LSan disabled)'if args.sanitizers else 'normal',ordered_tracing=args.trace,elf_sha256=manifest['elf_sha256'],command=[str(x)for x in cmd],native_result=result.stdout.strip(),source_words_verified=len(words),original_instruction_accounting=coverage['accounting'],native_allocations=0,source_mutations_detected=len(SOURCE_MUTATIONS)if args.mutations else 0,native_mutations_detected=len(NATIVE_MUTATIONS)if args.mutations else 0,sensitivity=controls,scope=manifest['scope'])
 report['artifacts']={str(p.relative_to(ROOT)):dict(bytes=p.stat().st_size,sha256=sha(p.read_bytes())) for p in [ROOT/'src/game/model_motion49t.cpp',ROOT/'src/game/model_motion49t.h',ROOT/'tests/native/model_motion49t.cpp',Path(__file__),SOURCE/'motion_machine.h',SOURCE/'flag_trace.h',SOURCE/'blocks.h',SOURCE/'source-manifest.json',SOURCE/'source-disassembly.txt']}
 (out/'native-test-result.json').write_text(json.dumps(report,indent=2)+'\n')
 print(result.stdout.strip());print('Verified%d original words; detected%d mutation controls; repository evidence read-only unless --output-dir is supplied.'%(len(words),len(controls)))

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');p.add_argument('--build-dir',type=Path);p.add_argument('--output-dir',type=Path);p.add_argument('--sanitizers',action='store_true');p.add_argument('--trace',action='store_true');p.add_argument('--mutations',action='store_true');a=p.parse_args()
 if a.check:
  m,w=verify();print(json.dumps(dict(status='verified',elf_sha256=m['elf_sha256'],words=len(w))))
 elif a.build_dir:run(a,a.build_dir.resolve())
 else:
  with tempfile.TemporaryDirectory(prefix='rr-motion49t-')as d:run(a,Path(d))
if __name__=='__main__':main()
