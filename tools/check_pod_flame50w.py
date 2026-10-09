#!/usr/bin/env python3
import argparse,pathlib,subprocess,os,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--output',required=True,type=pathlib.Path);p.add_argument('--sanitizers',action='store_true');a=p.parse_args();r=pathlib.Path(__file__).resolve().parents[1];out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
if any(out.iterdir()):raise SystemExit('Use an empty output directory')
sources=['src/assets/pod_flame50w.cpp','src/game/flame_bindings50v.cpp','src/assets/import_decode48c.cpp','src/assets/import_scene48d.cpp','src/assets/import_texture48e.cpp','tests/native/pod_flame50w.cpp'];inputs=sources+['src/assets/pod_flame50w.h','src/assets/pod_flame_runtime50w.h','src/game/flame_bindings50v.h','work/native/project/disc/PODS/POD01/POD01.RES'];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();hashes={x:sha(r/x)for x in inputs};flags=['g++','-std=c++17','-O1','-I'+str(r/'src/assets'),'-I'+str(r/'src/game')]
if a.sanitizers:flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all','-no-pie']
env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1');report=[];binary=out/'pod_flame50w'
for label,cmd in [('build',flags+[str(r/x)for x in sources]+['-lz','-o',str(binary)]),('test',[str(binary),str(r/inputs[-1])])]:
 run=subprocess.run(cmd,cwd=r,env=env,text=True,capture_output=True);(out/(label+'.stdout.log')).write_text(run.stdout);(out/(label+'.stderr.log')).write_text(run.stderr);report.append({'stage':label,'command':cmd,'returncode':run.returncode});(out/'commands.json').write_text(json.dumps(report,indent=2))
 if run.returncode:raise SystemExit(run.returncode)
assert hashes=={x:sha(r/x)for x in inputs};(out/'input-hashes.json').write_text(json.dumps(hashes,indent=2));print('Pod01 cold owners and hot binding passed')
