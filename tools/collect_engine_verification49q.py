#!/usr/bin/env python3
"""Preserve final44-suite runs and repeat engine coverage/connected control trace."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/engineAnimation49Q'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def count(text):
 try:return int(json.loads(text)['checks'])
 except (ValueError,KeyError):
  m=re.search(r'checks=(\d+)',text)or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',text)
  if not m:raise ValueError('Unknown test output: '+text[:150])
  return int(m.group(1))
def main():
 if not __debug__:raise RuntimeError('Collection requires assertions')
 ap=argparse.ArgumentParser();ap.add_argument('--normal',type=Path,required=True);ap.add_argument('--sanitized',type=Path,required=True);a=ap.parse_args()
 summary={'status':'passed','runs':{},'all_mode_outputs_identical':True}
 for mode,directory in [('normal',a.normal),('sanitized',a.sanitized)]:
  report=json.loads((directory/'report.json').read_text());assert report['status']=='passed'and not report['changed_inputs']and not report['errors']
  assert len(report['tests'])==44 and len(report['input_hashes'])>=243 and report['sanitizers']==(mode=='sanitized')
  for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
  out=E/f'final-{mode}';out.mkdir(exist_ok=True);(out/'regression-report.json').write_bytes((directory/'report.json').read_bytes())
  # Preserve every compiler, linker and test process log with its exact report
  # hash and size. Old absolute paths remain provenance; copies are durable.
  logs=out/'logs';logs.mkdir(exist_ok=True);log_pins={}
  processes=report['build']+[t['build']for t in report['tests']]+[t['run']for t in report['tests']]
  for process in processes:
   assert process['status']=='passed'and process['returncode']==0
   for stream in ['stdout','stderr']:
    meta=process[stream];original=Path(meta['path']);data=original.read_bytes()
    assert len(data)==meta['bytes']and hashlib.sha256(data).hexdigest()==meta['sha256']
    copy=logs/original.name
    assert original.name not in log_pins,original.name
    copy.write_bytes(data);log_pins[original.name]={'bytes':len(data),'sha256':meta['sha256']}
  tests=[];total=0
  for test in report['tests']:
   assert test['status']=='passed'and test['run']['returncode']==0
   stdout=Path(test['run']['stdout']['path']);stderr=Path(test['run']['stderr']['path']);assert not stderr.read_bytes()and sha(stdout)==test['run']['stdout']['sha256']
   checks=count(stdout.read_text());total+=checks;(out/(test['name']+'.stdout')).write_bytes(stdout.read_bytes())
   tests.append({'name':test['name'],'checks':checks,'stdout_sha256':sha(stdout),'stderr_bytes':0})
  row={'suites':44,'checks':total,'tracked_inputs':len(report['input_hashes']),'inputs_unchanged':True,'report_sha256':sha(out/'regression-report.json'),'tests':tests,'process_logs':log_pins}
  env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
  reruns=[('model_motion49q',[ROOT],{'RR_MODEL49Q_COVERAGE':str(out/'motion-coverage.json')}),('engine_animation49q',[ROOT],{'RR_ENGINE49Q_COVERAGE':str(out/'engine-coverage.tsv')}),('engine_animation_diagnostic49q',[E/'connected/supplied-controls49q.bin',E/'connected/exact-goldens49q.txt',out/'trace.json'],{})]
  for binary,args,extra in reruns:
   record=next(t['executable']for t in report['tests']if t['name']==binary);exe=directory/binary
   assert exe.resolve()==Path(record['path']).resolve()and exe.stat().st_size==record['bytes']and sha(exe)==record['sha256']
   run=subprocess.run([str(exe),*[str(x)for x in args]],cwd=ROOT,env=env|extra,text=True,capture_output=True)
   assert run.returncode==0 and not run.stderr,(binary,run.stderr);assert sha(exe)==record['sha256']
   (out/(binary+'-output.stdout')).write_text(run.stdout)
  row['replayed_files']={n:sha(out/n)for n in ['motion-coverage.json','engine-coverage.tsv','trace.json']}
  assert json.loads((out/'motion-coverage.json').read_text())['ordered_store_tracing']is False
  summary['runs'][mode]=row
 assert summary['runs']['normal']['tests']==summary['runs']['sanitized']['tests']
 assert summary['runs']['normal']['replayed_files']==summary['runs']['sanitized']['replayed_files']
 aggregate_trace=json.loads((E/'final-normal/trace.json').read_text())
 focused_trace=json.loads((E/'connected/control-driven-engine-frames49q.json').read_text())
 # Focused builds include exact ordered-store hooks; production aggregate does
 # not. Only the audit check counter differs; every semantic trace field must
 # remain byte-for-value equal after removing those two explicit counters.
 aggregate_checks=aggregate_trace['summary'].pop('checks')
 focused_checks=focused_trace['summary'].pop('checks')
 assert focused_checks>aggregate_checks
 assert aggregate_trace==focused_trace
 summary['semantic_trace_identical_to_focused']=True
 summary['trace_checks']={'aggregate':aggregate_checks,'focused':focused_checks}
 summary['checks_each']=summary['runs']['normal']['checks'];summary['aggregate_store_tracing']='off; separately required and verified in focused normal/sanitized source proof'
 (E/'regression-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
 print(json.dumps({'status':'passed','suites_each':44,'checks_each':summary['checks_each'],'tracked_inputs_each':summary['runs']['normal']['tracked_inputs'],'stdout_trace_coverage_identical':True}))
if __name__=='__main__':main()
