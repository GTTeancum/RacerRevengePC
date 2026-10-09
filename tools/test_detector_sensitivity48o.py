#!/usr/bin/env python3
"""Reject isolated detector mutations using unchanged original-derived fixtures."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--repo-root',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--build-dir',type=Path,required=True,help='Passed unsanitized detector runner directory')
parser.add_argument('--output-dir',type=Path,required=True,help='Fresh directory outside source')
a=parser.parse_args();ROOT=a.repo_root.resolve();BUILD=a.build_dir.resolve();OUT=a.output_dir.resolve()
if OUT==ROOT or ROOT in OUT.parents:parser.error('output must be outside source')
r=json.loads((BUILD/'report.json').read_text())
if r['status']!='passed' or r['sanitizers']:parser.error('passed release build required')
for path,expected in r['input_hashes'].items():
 b=(ROOT/path).read_bytes()
 if len(b)!=expected['bytes'] or hashlib.sha256(b).hexdigest()!=expected['sha256']:parser.error('changed input: '+path)
OUT.mkdir(exist_ok=False)
source=(ROOT/'src/game/spring_detector48o.cpp').read_text()
mutants={
 'old-float-minmax':('if(key<orderedKey(minimum[axis]))minimum[axis]=world.v[axis];\n                if(key>orderedKey(maximum[axis]))maximum[axis]=world.v[axis];','if(world.v[axis]<minimum[axis])minimum[axis]=world.v[axis];\n                if(world.v[axis]>maximum[axis])maximum[axis]=world.v[axis];'),
 'eager-first-endpoint':('    if(input.resetSpring)resetSpringRecords48N(result.spring);','    for(std::size_t i=0;i<pointCount;++i)scratch.pairs[2*i]=result.center;\n    if(input.resetSpring)resetSpringRecords48N(result.spring);'),
 'omit-reset':('if(input.resetSpring)resetSpringRecords48N(result.spring);','if(false && input.resetSpring)resetSpringRecords48N(result.spring);'),
 'wrong-half-w':('result.half.v[3]=1.f;','result.half.v[3]=0.f;'),
 'omit-dirty-scene-flag':('scene.flags|=1u;','scene.flags|=0u;'),
 'substitute-point-count-for-shape':('input.shapePresent,input.shapePairCount,','input.shapePresent,static_cast<std::uint32_t>(pointCount),'),
 'reuse-local-point-w':('SpringVector4_48N world{};world.v[3]=1.f;','SpringVector4_48N world{};world.v[3]=point.local.v[3];'),
 'omit-first-endpoint-fill':('for(std::size_t i=0;i<pointCount;++i)scratch.pairs[2*i]=result.center;','for(std::size_t i=0;i<0;++i)scratch.pairs[2*i]=result.center;'),
}
flags=['g++','-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(ROOT/'src/game'),'-I'+str(ROOT/'third_party/native-json')]
def run(command,name,expected=0):
 p=subprocess.run(command,cwd=ROOT,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=180)
 (OUT/(name+'.stdout.log')).write_bytes(p.stdout);(OUT/(name+'.stderr.log')).write_bytes(p.stderr)
 if expected is not None:assert p.returncode==expected,(name,p.returncode,p.stderr[-1200:])
 return p
run(flags+['-c',str(ROOT/'tests/native/spring_detector48o.cpp'),'-o',str(OUT/'test.o')],'compile-test')
objects=[str(BUILD/(n+'.o')) for n in ('query_prepare48o','spring_records48n')]
rows=[]
for name,(old,new) in mutants.items():
 assert source.count(old)==1,(name,source.count(old));cpp=OUT/(name+'.cpp');cpp.write_text(source.replace(old,new))
 obj=OUT/(name+'.o');exe=OUT/name
 run(flags+['-c',str(cpp),'-o',str(obj)],name+'-compile')
 run(flags+[str(OUT/'test.o'),str(obj)]+objects+['-o',str(exe)],name+'-link')
 p=run([str(exe),str(ROOT)],name+'-run',None);assert p.returncode==1,(name,p.returncode)
 failures=re.findall(rb'failures=(\d+)',p.stdout);assert failures and int(failures[-1])>0
 row={'mutation':name,'exit_code':p.returncode,'failed_checks':int(failures[-1]),'mutant_source_sha256':hashlib.sha256(cpp.read_bytes()).hexdigest()};rows.append(row);print(json.dumps(row),flush=True)
report={'status':'all_rejected','base_source_sha256':hashlib.sha256(source.encode()).hexdigest(),'test_source_sha256':hashlib.sha256((ROOT/'tests/native/spring_detector48o.cpp').read_bytes()).hexdigest(),'original_oracle_unchanged':True,'mutants':rows}
(OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
