#!/usr/bin/env python3
"""Focused clean Linux build of source unsigned conversion and seven-byte packet."""
import argparse,pathlib,subprocess,os,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=pathlib.Path);p.add_argument('--sanitizers',action='store_true');a=p.parse_args();r=pathlib.Path(__file__).resolve().parents[1];out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
if any(out.iterdir()):raise SystemExit('Use an empty output directory')
flags=['g++','-std=c++17','-O1','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(r/'src/game'),'-I'+str(r/'third_party/native-json')]
if a.sanitizers:flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all','-no-pie']
env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1');report=[]
for name,fixture in [('float_unsigned50r','conversion-fixtures.json'),('packed_vehicle_tail50r','packet-fixtures.json')]:
 sources=['src/game/float_unsigned50r.cpp']+(['src/game/packed_vehicle_tail50r.cpp']if name.startswith('packed')else[])+['tests/native/'+name+'.cpp'];binary=out/name
 for label,cmd in [('build',flags+[str(r/s)for s in sources]+['-o',str(binary)]),('test',[str(binary),str(r/'evidence/packedTail50R'/fixture)])]:
  run=subprocess.run(cmd,cwd=r,env=env,text=True,capture_output=True);(out/(name+'.'+label+'.stdout.log')).write_text(run.stdout);(out/(name+'.'+label+'.stderr.log')).write_text(run.stderr);report.append({'name':name,'stage':label,'command':cmd,'returncode':run.returncode});(out/'commands.json').write_text(json.dumps(report,indent=2))
  if run.returncode:raise SystemExit(run.returncode)
print('Both focused suites passed')
