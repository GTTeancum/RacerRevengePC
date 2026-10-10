#!/usr/bin/env python3
"""Safe scalar/state sensitivity of the independent original-word 49K suite.
Only copies of the new 49K numerical source are varied, in a temporary directory.
No pointer, ownership, allocation-failure, trap, capacity, or boundary-fault tests.
"""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[2]
MUTATIONS=[
 ('scheduler_plus_one','static_cast<std::uint32_t>(product)+1u','static_cast<std::uint32_t>(product)+2u'),
 ('scheduler_division','f.div(factor,static_cast<float>(count))','f.mul(factor,static_cast<float>(count))'),
 ('incoming_gravity_state','if(!s.incoming348_104)','if(s.incoming348_104)'),
 ('brake_speed_boundary','s.cachedSpeed340_34<fp(0x4212aa7fu)','s.cachedSpeed340_34<=fp(0x4212aa7fu)'),
 ('gravity_up_axis','f.sub(s.sceneUpY,s.effective434738)','f.sub(s.effective434738,s.sceneUpY)'),
 ('engine_boost_second_state','state==1u||state==2u','state==1u'),
 ('engine_repair_bit','(flags>>1u)&1u','(flags>>0u)&1u'),
 ('prepare_scalar_clear','s.field118=0;','s.field118=1;'),
 ('prepare_point_axis','f.mul(scene.v[i],local[0])','f.mul(scene.v[i],local[2])'),
 ('airborne_effective_global','f.mul(live,component)','f.mul(0.3f,component)'),
 ('airborne_cached_angle','f.mul(angle,f4)','f.mul(1.0f,f4)'),
 ('response_pending_state','r.state.field10C=1;','r.state.field10C=0;'),
 ('late_feedback_fields','return {probe,probe,c3C,c44};','return {probe,probe,c44,c3C};'),
 ('vehicle380_raw_word','return value!=0u?1u:0u;','return value;'),
 ('main_body_clear','state=0;','state=1;'),
 ('rounding_admission','if(std::fegetround()!=FE_TONEAREST)return false;',''),
]
def main():
 parser=argparse.ArgumentParser();parser.add_argument('--compiler',default=os.environ.get('CXX','c++'));args=parser.parse_args()
 src=ROOT/'src/game/advancement_selectors49k.cpp';original=src.read_text();rows=[]
 with tempfile.TemporaryDirectory(prefix='49k-safe-scalar-') as td:
  temp=Path(td)
  for name,a,b in MUTATIONS:
   assert a in original,name
   changed=original.replace(a,b);file=temp/(name+'.cpp');file.write_text(changed);binary=temp/name
   command=[args.compiler,'-std=c++17','-O2','-fno-fast-math','-ffp-contract=off','-I'+str(ROOT/'src/game'),str(file),str(ROOT/'tests/native/advancement_selectors49k.cpp'),'-o',str(binary)]
   build=subprocess.run(command,capture_output=True,text=True)
   assert build.returncode==0,build.stderr
   result=subprocess.run([str(binary)],capture_output=True,text=True)
   assert result.returncode==1 and '49K numeric failure' in result.stderr,(name,result.returncode,result.stdout,result.stderr)
   rows.append({'mutation':name,'kind':'safe numerical/state substitution','detected':True,'diagnostic':result.stderr.strip(),'mutant_sha256':hashlib.sha256(changed.encode()).hexdigest()})
 assert src.read_text()==original
 out={'status':'passed','source_sha256':hashlib.sha256(original.encode()).hexdigest(),'mutations':len(rows),'detected':len(rows),'pointer_ownership_failure_capacity_experiments':0,'results':rows}
 target=ROOT/'evidence/bodyAdvancement49K/numeric-source/scalar-state-sensitivity.json';target.write_text(json.dumps(out,indent=2)+'\n');print(json.dumps({'status':'passed','safe_mutations':len(rows),'detected':len(rows)}))
if __name__=='__main__':main()
