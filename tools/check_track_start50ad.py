from pathlib import Path
import subprocess,sys,os,json,hashlib
r=Path(__file__).resolve().parents[1];mode=sys.argv[1];o=Path(sys.argv[2]).resolve();o.mkdir(parents=True,exist_ok=True);
if any(o.iterdir()):raise SystemExit('Use a new output directory')
sources=['src/assets/track_start50ad.cpp','src/game/grid_center50ac.cpp','src/assets/import_decode48c.cpp','tests/native/track_start50ad.cpp'];inputs=sources+['src/assets/track_start50ad.h','src/game/grid_center50ac.h','evidence/trackStartReader50AD/numbers.json','work/native/project/disc/TRACKS/TA.RES','evidence/gridCenter50AC/pose-fixtures.json'];flags=['g++','-std=c++20','-O1','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(r/'src/game'),'-I'+str(r/'src/assets'),'-I'+str(r/'third_party/native-json')]
if mode=='sanitized':flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all','-no-pie']
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();hashes={s:sha(r/s)for s in inputs};env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1');rows=[]
for name,cmd in [('build',flags+[str(r/s)for s in sources]+['-lz','-o',str(o/'test')]),('test',[str(o/'test'),*[str(r/s)for s in inputs[-3:]]])]:
 p=subprocess.run(cmd,cwd=r,env=env,text=True,capture_output=True);(o/(name+'.stdout.log')).write_text(p.stdout);(o/(name+'.stderr.log')).write_text(p.stderr);rows.append({'name':name,'command':cmd,'returncode':p.returncode});(o/'commands.json').write_text(json.dumps(rows,indent=2));print(name,p.returncode,p.stdout,p.stderr[-1200:],flush=True)
 if p.returncode:raise SystemExit(p.returncode)
assert hashes=={s:sha(r/s)for s in inputs};(o/'hashes.json').write_text(json.dumps(hashes,indent=2))
