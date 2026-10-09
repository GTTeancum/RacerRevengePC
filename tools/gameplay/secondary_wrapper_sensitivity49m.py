#!/usr/bin/env python3
"""Kill meaningful numerical mutations with independent original-word fixtures.
Copies only the new49M production source into a temporary directory; never edits
inherited49L files or derives expected outputs from production formulas.
"""
import argparse,hashlib,json,os,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'src/game/secondary_body49m.cpp'
COMMON=['gradient_noise49m','pod_sphere_constraint49l','body_wrapper48h','body_integration48c','body_scene48g','vehicle_aero48e','vehicle_forces48d']
MUTATIONS=[
 ('gravity_selector_swapped','c.probe104?c.gravity378:c.gravity37C','c.probe104?c.gravity37C:c.gravity378'),
 ('gravity_mass_sign','f.mul(f.neg(p.body.mass),gravity)','f.mul(p.body.mass,gravity)'),
 ('angular_damping_speed_origin','fp(0x422fffcc)','fp(0x42200000)'),
 ('angular_damping_speed_span','fp(0x41eaaa64)','fp(0x41aaaa64)'),
 ('restore_vertical_not_cleared','error.v[1]=0.0f;','error.v[1]=error.v[1];'),
 ('restore_length_factor_lost','f.scale(f.mul(c.effective4346E0,length),direction)','f.scale(c.effective4346E0,direction)'),
 ('linear_damping_sign','f.scale(f.neg(c.effective4346C8),vec(in.root.body.velocity))','f.scale(c.effective4346C8,vec(in.root.body.velocity))'),
 ('alignment_cross_reversed','f.cross(upA,upB)','f.cross(upB,upA)'),
 ('noise_half_bias','f.add(0.5f,f.mul(0.5f,value))','f.add(0.25f,f.mul(0.5f,value))'),
 ('noise_phase_not_advanced','r.time864=f.add(in.time864,dt);','r.time864=in.time864;'),
 ('noise_force_wrong_speed','f.mul(c.mainCachedSpeed34,f.mul(h,c.effective434700))','f.mul(in.cachedSpeed34,f.mul(h,c.effective434700))'),
 ('supplied_contact_loads_dropped','if(!updateRootBody48H(r.root,p.body,p.centerOffset,r.scenePresent,r.enabled,dt,r.root))','r.root.loads={};if(!updateRootBody48H(r.root,p.body,p.centerOffset,r.scenePresent,r.enabled,dt,r.root))'),
 ('force_clear_W_zero','r.force180W=1.0f;r.torque190W=1.0f;','r.force180W=0.0f;r.torque190W=0.0f;'),
 ('scene_dirty_bit_lost','if(r.scenePresent&&r.enabled)r.sceneFlags190|=0x80u;','if(r.scenePresent&&r.enabled)r.sceneFlags190|=0u;'),
]
def run(cmd):
 p=subprocess.run([str(x) for x in cmd],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 if p.returncode:raise RuntimeError('Compile failed: '+' '.join(map(str,cmd))+'\n'+p.stdout)
 return p.stdout

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output',default='evidence/secondaryWrapper49M/mutations/results.json');ap.add_argument('--compiler',default=os.environ.get('CXX','g++'));args=ap.parse_args()
 source=SOURCE.read_text();flags=['-std=c++20','-O2','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/game')]
 results=[]
 with tempfile.TemporaryDirectory(prefix='secondary49m-mutations-') as temp:
  temp=Path(temp);objects=[]
  for name in COMMON:
   obj=temp/(name+'.o');run([args.compiler,*flags,'-c',ROOT/'src/game'/f'{name}.cpp','-o',obj]);objects.append(obj)
  test=temp/'test.o';run([args.compiler,*flags,'-c',ROOT/'tests/native/secondary_wrapper49m.cpp','-o',test]);objects.append(test)
  for label,old,new in [('baseline',None,None),*MUTATIONS]:
   if old is not None and source.count(old)!=1:raise ValueError(f'{label}: mutation site count {source.count(old)}')
   text=source if old is None else source.replace(old,new);path=temp/(label+'.cpp');path.write_text(text);obj=temp/(label+'.o');exe=temp/label
   run([args.compiler,*flags,'-c',path,'-o',obj]);run([args.compiler,*flags,obj,*objects,'-o',exe])
   p=subprocess.run([str(exe)],cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
   if label=='baseline' and p.returncode:raise RuntimeError('Baseline failed: '+p.stdout)
   if label!='baseline' and p.returncode==0:raise RuntimeError('Mutation survived: '+label)
   results.append({'name':label,'exit_code':p.returncode,'status':'passed' if label=='baseline' else 'killed','output':p.stdout.strip(),'mutated_source_sha256':hashlib.sha256(text.encode()).hexdigest()})
   print(label+': '+results[-1]['status'],flush=True)
 report={'production_source_sha256':hashlib.sha256(source.encode()).hexdigest(),'test_sha256':hashlib.sha256((ROOT/'tests/native/secondary_wrapper49m.cpp').read_bytes()).hexdigest(),'oracle_sha256':hashlib.sha256((ROOT/'tools/gameplay/secondary_wrapper_oracle49m.h').read_bytes()).hexdigest(),'native_files_mutated':0,'mutations':len(MUTATIONS),'all_mutants_compiled':True,'results':results}
 out=ROOT/args.output;out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({'status':'pass','mutations_killed':len(MUTATIONS),'report':str(out.relative_to(ROOT))}))
if __name__=='__main__':main()
