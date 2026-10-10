#!/usr/bin/env python3
"""Fresh bounded five-suite component gate after recovery, not a69-suite replay.
Builds sequentially, records every hash/log, then removes only generated binaries
and objects. Baseline source remains read-only; no full repository copy needed.
"""
from pathlib import Path
import argparse,datetime,hashlib,json,os,re,shutil,subprocess,sys,time
sys.dont_write_bytecode=True
COMPONENT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser();p.add_argument('--baseline',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True);p.add_argument('--sanitizers',action='store_true');a=p.parse_args()
 base=a.baseline.resolve();out=a.output_dir.resolve();out.mkdir(parents=True,exist_ok=False)
 includes=[COMPONENT/'src/game',base/'src/game',base/'src/assets',base/'third_party/native-json']
 sources=[COMPONENT/'src/game'/x for x in ['static_type2_producer49x.cpp','type2_resource49x.cpp','type2_triangle_axes49x.cpp']]+[base/'src/game'/x for x in ['collision_query48h.cpp','spring_callback48n.cpp','spring_records48n.cpp']]+[base/'src/assets'/x for x in ['collision_mesh48i.cpp','collision_runtime48h.cpp','import_collision48g.cpp','import_decode48c.cpp']]
 e=COMPONENT/'evidence/staticType2Producer49X'
 tests=[('opaque-snan',COMPONENT/'tests/native/static_type2_producer49x.cpp',[e/'original-word/opaque-snan-fixtures.json']),('producer',COMPONENT/'tests/native/static_type2_producer49x.cpp',[e/'original-word/native-fixtures.json']),('early-reject',COMPONENT/'tests/native/static_type2_producer49x.cpp',[e/'admission/first-reject-fixture.json']),('finite-overflow',COMPONENT/'tests/native/static_type2_admission49x.cpp',[]),('resource',COMPONENT/'tests/type2_resource49x_test.cpp',[e/'resource/fixtures.json',base])]
 pending=sources+[t[1] for t in tests];inputs=set()
 while pending:
  path=pending.pop().resolve()
  if path in inputs:continue
  assert path.is_file(),path;inputs.add(path)
  for header in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"',path.read_text(),re.M):
   choices=[path.parent/header]+[i/header for i in includes];resolved=next((x for x in choices if x.is_file()),None);assert resolved,header;pending.append(resolved)
 for _,_,args in tests:
  for arg in args:
   if arg.is_file():inputs.add(arg)
 flags=['-std=c++17','-Wall','-Wextra','-Wpedantic','-fno-fast-math','-frounding-math','-ffp-contract=off','-fstack-usage']+(['-O1','-g1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all'] if a.sanitizers else ['-O2'])
 compiler=shutil.which(os.environ.get('CXX','g++'));assert compiler
 report={'scope':'fresh_recovered_component_only','started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'sanitizers':a.sanitizers,'leak_detection':False,'baseline':str(base),'component':str(COMPONENT),'compiler':compiler,'compiler_sha256':sha(Path(compiler).resolve()),'flags':flags,'input_hashes':{str(p):sha(p) for p in sorted(inputs)},'processes':[],'tests':[],'retired_rebuildable_products':[]}
 def save(): (out/'report.json').write_text(json.dumps(report,indent=2))
 env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
 def run(cmd,label):
  assert shutil.disk_usage(out).free>1024**3,'less than1GiB free; stop before exhausting workspace'
  start=time.monotonic();r=subprocess.run(list(map(str,cmd)),cwd=COMPONENT,env=env,capture_output=True,timeout=300)
  entry={'label':label,'command':list(map(str,cmd)),'returncode':r.returncode,'seconds':time.monotonic()-start}
  for key,data in [('stdout',r.stdout),('stderr',r.stderr)]:
   dest=out/(label+'.'+key+'.log');dest.write_bytes(data);entry[key]={'file':dest.name,'bytes':len(data),'sha256':sha(dest)}
  report['processes'].append(entry);save();return r
 def retire(path):
  record={'file':path.name,'bytes':path.stat().st_size,'sha256':sha(path),'rebuildable':True};report['retired_rebuildable_products'].append(record);path.unlink();save();return record
 save();assert run([compiler,'--version'],'compiler-version').returncode==0
 objects=[];passed=True
 for i,source in enumerate(sources):
  obj=out/(str(i)+'-'+source.stem+'.o');cmd=[compiler,*flags,*['-I'+str(p) for p in includes],'-c',source,'-o',obj]
  if run(cmd,'build-'+str(i)+'-'+source.stem).returncode:passed=False;break
  objects.append(obj)
 if passed:
  binaries={}
  for name,source,args in tests:
   # Share one freshly built executable between producer corpus and early-prune.
   if source not in binaries:
    exe=out/(source.stem+'-check');cmd=[compiler,*flags,*['-I'+str(p) for p in includes],source,*objects,'-lz','-o',exe]
    if run(cmd,'link-'+name).returncode:passed=False;break
    binaries[source]=exe
   r=run([binaries[source],*args],'test-'+name);row={'name':name,'status':'passed' if r.returncode==0 else 'failed','stdout':r.stdout.decode(errors='replace'),'stderr_bytes':len(r.stderr)};report['tests'].append(row);save();print(name,row['status'],flush=True)
   if r.returncode:passed=False;break
  for exe in binaries.values():retire(exe)
 for obj in objects:retire(obj)
 report['changed_inputs']=[str(p) for p in sorted(inputs) if sha(p)!=report['input_hashes'][str(p)]]
 report['stack_usage']={p.name:p.read_text() for p in out.glob('*.su')}
 report['status']='passed' if passed and len(report['tests'])==5 and not report['changed_inputs'] else 'failed'
 report['finished_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();save();print(json.dumps({'status':report['status'],'tests':len(report['tests'])}));return 0 if report['status']=='passed' else 1
if __name__=='__main__':sys.exit(main())
