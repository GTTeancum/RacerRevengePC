#!/usr/bin/env python3
"""Preserve final41-suite runs and repeat source coverage/connected scene trace."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/sceneProjection49P'
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
  assert len(report['tests'])==41 and len(report['input_hashes'])==243 and report['sanitizers']==(mode=='sanitized')
  for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
  out=E/f'final-{mode}';out.mkdir(exist_ok=True);(out/'regression-report.json').write_bytes((directory/'report.json').read_bytes())
  tests=[];total=0
  for test in report['tests']:
   assert test['status']=='passed'and test['run']['returncode']==0
   stdout=Path(test['run']['stdout']['path']);stderr=Path(test['run']['stderr']['path']);assert not stderr.read_bytes()and sha(stdout)==test['run']['stdout']['sha256']
   checks=count(stdout.read_text());total+=checks;(out/(test['name']+'.stdout')).write_bytes(stdout.read_bytes())
   tests.append({'name':test['name'],'checks':checks,'stdout_sha256':sha(stdout),'stderr_bytes':0})
  assert total==124535876,total
  row={'suites':41,'checks':total,'tracked_inputs':243,'inputs_unchanged':True,'report_sha256':sha(out/'regression-report.json'),'tests':tests}
  env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
  psg=ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg'
  reruns=[('scene_graph49p',[psg],{'RR_SCENE49P_COVERAGE':str(out/'scene-coverage.json')}),('chariot_projection49p',[],{'RR_CHARIOT49P_COVERAGE':str(out/'chariot-coverage.json')}),('scene_frame_diagnostic49p',[E/'connected/supplied-physical-scenes49p.bin',psg,E/'connected/exact-goldens49p.txt',out/'trace.json'],{})]
  for binary,args,extra in reruns:
   record=next(t['executable']for t in report['tests']if t['name']==binary);exe=directory/binary
   assert exe.resolve()==Path(record['path']).resolve()and exe.stat().st_size==record['bytes']and sha(exe)==record['sha256']
   run=subprocess.run([str(exe),*[str(x)for x in args]],cwd=ROOT,env=env|extra,text=True,capture_output=True)
   assert run.returncode==0 and not run.stderr,(binary,run.stderr);assert sha(exe)==record['sha256']
   (out/(binary+'-output.stdout')).write_text(run.stdout)
  row['replayed_files']={n:sha(out/n)for n in ['scene-coverage.json','chariot-coverage.json','trace.json']}
  assert json.loads((out/'scene-coverage.json').read_text())['ordered_store_tracing']is False
  summary['runs'][mode]=row
 assert summary['runs']['normal']['tests']==summary['runs']['sanitized']['tests']
 assert summary['runs']['normal']['replayed_files']==summary['runs']['sanitized']['replayed_files']
 assert (E/'final-normal/trace.json').read_bytes()==(E/'connected/persistent-scene-frames49p.json').read_bytes()
 summary['checks_each']=124535876;summary['aggregate_store_tracing']='off; separately required and verified in focused normal/sanitized source proof'
 (E/'regression-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
 print(json.dumps({'status':'passed','suites_each':41,'checks_each':124535876,'tracked_inputs_each':243,'stdout_trace_coverage_identical':True}))
if __name__=='__main__':main()
