#!/usr/bin/env python3
"""Compile isolated native perturbations; never use them as reference values."""
from pathlib import Path
import hashlib,json,subprocess
import argparse
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--repo-root',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--build-dir',type=Path,required=True)
parser.add_argument('--output-dir',type=Path,required=True)
a=parser.parse_args();ROOT=a.repo_root.resolve();OUT=a.output_dir.resolve();BUILD=a.build_dir.resolve()
if OUT==ROOT or ROOT in OUT.parents:parser.error('output must be outside source')
reference=json.loads((BUILD/'report.json').read_text())
if reference['status']!='passed' or reference['sanitizers']:parser.error('passed release detector build required')
for name,expected in reference['input_hashes'].items():
 data=(ROOT/name).read_bytes()
 if len(data)!=expected['bytes'] or hashlib.sha256(data).hexdigest()!=expected['sha256']:parser.error('changed input: '+name)
OUT.mkdir(exist_ok=False)
flags=['g++','-std=c++17','-O2','-fno-fast-math','-ffp-contract=off','-Wall','-Wextra','-Werror','-I',str(ROOT/'src/game'),'-I',str(ROOT/'third_party/native-json')]
source=(ROOT/'src/game/query_prepare48o.cpp').read_text()
mutations=[
 ('wrong-shape-tag','shapeType=3;','shapeType=2;'),
 ('null-metadata-not-zero','input.sourceMetadata?input.sourceMetadata[i]:0','input.sourceMetadata?input.sourceMetadata[i]:1'),
 ('omit-last-staged-vector','i<count;++i','i+1<count;++i'),
 ('scaled-radius','result.radius=math.root(math.add(xy,math.mul(half[2],half[2])));','result.radius=math.mul(2.f,math.root(math.add(xy,math.mul(half[2],half[2]))));'),
 ('omit-center-translation','result.center.v[i]=math.add(xyzCenter,input.preparedScene.v[12+i]);','result.center.v[i]=math.add(xyzCenter,0.f);')]
def run(command):return subprocess.run(command,cwd=ROOT,text=True,capture_output=True,timeout=180)
command=flags+['-c',str(ROOT/'tests/native/query_prepare48o.cpp'),'-o',str(OUT/'test.o')]
build=run(command);assert build.returncode==0,build.stderr
rows=[]
for name,old,new in mutations:
 assert old in source
 path=OUT/(name+'.cpp');path.write_text(source.replace(old,new))
 binary=OUT/name;command=flags+[str(path),str(OUT/'test.o'),'-o',str(binary)]
 compiled=run(command);(OUT/(name+'-build.log')).write_text(compiled.stdout+compiled.stderr)
 assert compiled.returncode==0,(name,compiled.stderr)
 check=run([str(binary),str(ROOT)]);(OUT/(name+'-run.log')).write_text(check.stdout+check.stderr)
 assert check.returncode==1,(name,check.returncode)
 rows.append(dict(name=name,rejected=True,compile_exit=compiled.returncode,test_exit=check.returncode,
  native_source_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),output=check.stdout.strip()))


(OUT/'report.json').write_text(json.dumps(dict(passed=True,mutations=rows,scope='Native implementation perturbations only; original fixtures unchanged.'),indent=2)+'\n')
print(json.dumps(dict(passed=True,rejected_mutations=len(rows))))
