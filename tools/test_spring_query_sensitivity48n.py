#!/usr/bin/env python3
from pathlib import Path
import json,subprocess,hashlib,re
import argparse
parser=argparse.ArgumentParser(description="Reject isolated48N ordered-query mutations against original-derived fixtures; no source edits")
parser.add_argument('--repo-root',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--build-dir',type=Path,required=True,help="Passed release producer runner directory")
parser.add_argument('--output-dir',type=Path,required=True,help="Fresh directory outside source")
args=parser.parse_args()
REPO=args.repo_root.resolve();BUILD=args.build_dir.resolve();OUT=args.output_dir.resolve()
if OUT==REPO or REPO in OUT.parents:parser.error('output must be outside source')
record=json.loads((BUILD/'report.json').read_text())
if record['status']!='passed' or record['sanitizers']:parser.error('passed unsanitized release build required')
for name,expected in record['input_hashes'].items():
 p=REPO/name
 if p.stat().st_size!=expected['bytes'] or hashlib.sha256(p.read_bytes()).hexdigest()!=expected['sha256']:
  parser.error('input changed since release build: '+name)
OUT.mkdir(exist_ok=False)
source=(REPO/'src/game/spring_query48n.cpp').read_text()
mutants={
 'clear-previous-query-candidate':('result.currentCandidateIdentity=query.initialQueryCurrentIdentity;','result.currentCandidateIdentity=0;'),
 'replace-cumulative-with-candidate':('if(!result.hasGeometryHit||hit.nearestPair.fraction<result.collision.nearestPair.fraction){','if(true){'),
 'replace-strict-pair-ties':('hit.nearestPair.fraction<result.collision.nearestPair.fraction','hit.nearestPair.fraction<=result.collision.nearestPair.fraction'),
 'replace-global-equal-ties':('hit.selected.fraction<=result.collision.selected.fraction','hit.selected.fraction<result.collision.selected.fraction'),
 'only-final-candidate-callback':('if(query.callbackEnabled){','if(query.callbackEnabled && i+1==count){'),
 'force-normal-w-one':('result.pair.normal.v[3]=2.f;','result.pair.normal.v[3]=1.f;'),
 'treat-saturation-as-capacity-error':('result.appended+=appended.appended;','if(appended.saturated)return Status::InsufficientCapacity;\n                result.appended+=appended.appended;'),
}
flags=['g++','-std=c++17','-O2','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off','-I'+str(REPO/'src/game'),'-I'+str(REPO/'src/assets'),'-I'+str(REPO/'third_party/native-json')]
def cmd(args,name,expected=0):
 p=subprocess.run(args,cwd=REPO,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=180)
 (OUT/(name+'.stdout.log')).write_bytes(p.stdout);(OUT/(name+'.stderr.log')).write_bytes(p.stderr)
 if expected is not None:assert p.returncode==expected,(name,p.returncode,p.stderr[-1500:])
 return p
cmd(flags+['-c',str(REPO/'tests/native/spring_query48n.cpp'),'-o',str(OUT/'test.o')],'compile-test')
objects=[str(BUILD/(n+'.o')) for n in ('collision_query48h','collision_tree_workspace48m','spring_records48n','spring_callback48n')]
results=[]
for name,(old,new) in mutants.items():
 assert source.count(old)==1,(name,source.count(old))
 cpp=OUT/(name+'.cpp');cpp.write_text(source.replace(old,new));obj=OUT/(name+'.o');exe=OUT/name
 cmd(flags+['-c',str(cpp),'-o',str(obj)],name+'-compile')
 cmd(flags+[str(OUT/'test.o'),str(obj)]+objects+['-o',str(exe)],name+'-link')
 p=cmd([str(exe),str(REPO)],name+'-run',None)
 assert p.returncode==1,(name,p.returncode)
 matches=re.findall(rb'failures=(\d+)',p.stdout);assert matches and int(matches[-1])>0
 row={'mutation':name,'exit_code':p.returncode,'failed_checks':int(matches[-1]),'mutant_source_sha256':hashlib.sha256(cpp.read_bytes()).hexdigest()};results.append(row);print(json.dumps(row),flush=True)
report={'base_source_sha256':hashlib.sha256(source.encode()).hexdigest(),'test_source_sha256':hashlib.sha256((REPO/'tests/native/spring_query48n.cpp').read_bytes()).hexdigest(),'fixture_sha256':hashlib.sha256((REPO/'evidence/springQuery48N/native-fixtures.json').read_bytes()).hexdigest(),'original_oracle_unchanged':True,'mutants':results,'status':'all_rejected'}
(OUT/'report.json').write_text(json.dumps(report,indent=2)+'\n')
