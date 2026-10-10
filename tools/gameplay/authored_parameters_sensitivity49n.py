#!/usr/bin/env python3
"""Safe numerical mutation controls for the independently extracted49N word oracle.
Mutates only temporary copies of new production numeric code. No allocation,
ownership/lifetime, process memory, installs, deployment, or inherited edits.
"""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
MUTATIONS=[
 ('gravity-unit','fromBits(0x4200cccdu)','fromBits(0x42000000u)'),
 ('speed-unit','fromBits(0x3fbbbb84u)','fromBits(0x3f800000u)'),
 ('pitch-roll-axis','result.inertia100.v[0]=m.mul(pitch,t)','result.inertia100.v[0]=m.mul(roll,t)'),
 ('inverse-signed-zero','o.v[4]=mul(u,negative(ci));','o.v[4]=mul(u,ci);'),
 ('tuning-C-precedence','selected=AuthoredClass49N::C;','selected=AuthoredClass49N::B;'),
 ('preset-half-steering','m.mul(a.steering100,0.5f)','m.mul(a.steering100,0.25f)'),
 ('profile-speed-clamp','m.mul(units,m.interpolate(minSpeed,maxSpeed,q.speed1924))','m.mul(units,m.clampedProfile(minSpeed,maxSpeed,q.speed1924))'),
 ('preset-vectors-after-profile','result.engineDifference5C=m.sub(e.theoreticalTopSpeed,actualTopSpeed);','if(context.applyProfileOverwrite&&!context.mode58Nonzero&&context.configurationEntryNonzero){e.torqueBasis[0].v[1]=negative(a.steering100);e.torqueBasis[1].v[1]=a.steering100;}\n    result.engineDifference5C=m.sub(e.theoreticalTopSpeed,actualTopSpeed);'),
 ('final-speed-difference','m.sub(e.theoreticalTopSpeed,actualTopSpeed)','m.sub(actualTopSpeed,e.theoreticalTopSpeed)'),
 ('aero-key-binding','a.lateral90=get(K::SideSlipSideForce)','a.lateral90=get(K::RollRateRollMoment)'),
 ('joystick-key-binding','result.controls.speed1B84=get(K::SteerSpeed1)','result.controls.speed1B84=get(K::SteerSpeed2)'),
]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output',type=Path,default=ROOT/'evidence/authoredInitialization49N/numeric-source/mutations');a=ap.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 source=(ROOT/'src/game/authored_parameters49n.cpp').read_text();before=hashlib.sha256(source.encode()).hexdigest();results=[]
 for name,old,new in MUTATIONS:
  if source.count(old)!=1:raise ValueError(f'{name}: expected exactly one mutation anchor, got {source.count(old)}')
  cpp=a.output/(name+'.cpp');exe=a.output/name;cpp.write_text(source.replace(old,new))
  command=['c++','-std=c++17','-O2','-fno-fast-math','-ffp-contract=off','-I'+str(ROOT/'src/game'),str(ROOT/'tests/native/authored_parameters49n.cpp'),str(cpp),'-o',str(exe)]
  build=subprocess.run(command,capture_output=True,text=True);(a.output/(name+'-build.log')).write_text(build.stdout+build.stderr)
  if build.returncode:raise RuntimeError('mutation did not compile: '+name)
  run=subprocess.run([str(exe)],capture_output=True,text=True);(a.output/(name+'-run.log')).write_text(run.stdout+run.stderr)
  result={'name':name,'compile_returncode':build.returncode,'run_returncode':run.returncode,'detected':run.returncode!=0,'failure':(run.stderr+run.stdout).strip()};results.append(result);print(json.dumps(result),flush=True)
  if run.returncode==0:raise RuntimeError('undetected numerical mutation: '+name)
 assert hashlib.sha256((ROOT/'src/game/authored_parameters49n.cpp').read_bytes()).hexdigest()==before
 report={'production_sha256':before,'mutations':results,'all_detected':all(x['detected'] for x in results),'production_files_modified':0,'scope':'Numerical values/order/key-binding only; no allocation-failure, ownership, lifetime, reset or hidden diagnostic controls.'}
 (a.output/'summary.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
