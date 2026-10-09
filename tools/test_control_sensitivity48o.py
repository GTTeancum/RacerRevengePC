#!/usr/bin/env python3
"""Verify that the corrected70-word control oracle rejects legacy zero ties."""
from pathlib import Path
import argparse,hashlib,json,subprocess
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--repo-root',type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument('--build-dir',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True)
a=p.parse_args();ROOT=a.repo_root.resolve();BUILD=a.build_dir.resolve();OUT=a.output_dir.resolve()
if OUT==ROOT or ROOT in OUT.parents:p.error('output must be outside source')
r=json.loads((BUILD/'report.json').read_text())
if r['status']!='passed' or r['sanitizers']:p.error('passed unsanitized minmax runner build required')
for name,expected in r['input_hashes'].items():
 b=(ROOT/name).read_bytes()
 if len(b)!=expected['bytes'] or hashlib.sha256(b).hexdigest()!=expected['sha256']:p.error('changed input: '+name)
OUT.mkdir(exist_ok=False);source=(ROOT/'src/game/control_filter48b.cpp').read_text()
for old,new in [('detail::scalarMax48O(delta,-limit)','delta < -limit ? -limit : delta'),('detail::scalarMin48O(lower,limit)','limit < lower ? limit : lower')]:
 assert source.count(old)==1;source=source.replace(old,new)
cpp=OUT/'legacy-control.cpp';cpp.write_text(source);binary=OUT/'legacy-control'
command=['g++','-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/game'),str(cpp),str(ROOT/'tests/native/control_filter48b.cpp'),'-o',str(binary)]
c=subprocess.run(command,cwd=ROOT,capture_output=True,timeout=180);(OUT/'build.stdout.log').write_bytes(c.stdout);(OUT/'build.stderr.log').write_bytes(c.stderr);assert c.returncode==0,c.stderr
c=subprocess.run([str(binary)],cwd=ROOT,capture_output=True,timeout=120);(OUT/'test.stdout.log').write_bytes(c.stdout);(OUT/'test.stderr.log').write_bytes(c.stderr)
assert c.returncode==1 and b'original-word differential' in c.stderr,(c.returncode,c.stderr)
report={'status':'rejected','mutation':'restore-legacy-control-float-ties','compile_command':command,'exit_code':c.returncode,'diagnostic':c.stderr.decode().strip(),'test_sha256':hashlib.sha256((ROOT/'tests/native/control_filter48b.cpp').read_bytes()).hexdigest(),'mutant_sha256':hashlib.sha256(cpp.read_bytes()).hexdigest(),'original_words_unchanged':True}
(OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({'status':'rejected','exit_code':c.returncode}))
