#!/usr/bin/env python3
"""Preserve final37-suite runs and independently repeat matching trace/coverage."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/authoredGeometry49O'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def count(s):
 try:return int(json.loads(s)['checks'])
 except (json.JSONDecodeError,KeyError):
  m=re.search(r'checks=(\d+)',s) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',s)
  if not m:raise ValueError('Unknown test stdout: '+s[:150])
  return int(m.group(1))
def main():
 if not __debug__:raise RuntimeError("Collection requires assertions")
 ap=argparse.ArgumentParser();ap.add_argument('--normal',type=Path,required=True);ap.add_argument('--sanitized',type=Path,required=True);a=ap.parse_args()
 summary={'status':'passed','runs':{},'all_mode_outputs_identical':True}
 for mode,directory in [('normal',a.normal),('sanitized',a.sanitized)]:
  report=json.loads((directory/'report.json').read_text());assert report['status']=='passed' and not report['changed_inputs'] and not report['errors']
  assert len(report['tests'])==37 and len(report['input_hashes'])==228
  assert report['sanitizers']==(mode=='sanitized')
  for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
  out=E/f'final-{mode}';out.mkdir(exist_ok=True);(out/'regression-report.json').write_bytes((directory/'report.json').read_bytes())
  tests=[];total=0
  for test in report['tests']:
   assert test['status']=='passed' and test['run']['returncode']==0
   stdout=Path(test['run']['stdout']['path']);stderr=Path(test['run']['stderr']['path']);assert not stderr.read_bytes() and sha(stdout)==test['run']['stdout']['sha256']
   checks=count(stdout.read_text());total+=checks;(out/(test['name']+'.stdout')).write_bytes(stdout.read_bytes())
   tests.append({'name':test['name'],'checks':checks,'stdout_sha256':sha(stdout),'stderr_bytes':0})
  row={'suites':37,'checks':total,'tracked_inputs':228,'inputs_unchanged':True,'report_sha256':sha(out/'regression-report.json'),'tests':tests}
  env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
  fixture=ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures'
  composition=out/'composition';composition.mkdir(exist_ok=True)
  reruns=[('authored_geometry49o',[out/'initialization-coverage.json'],{}),
   ('model_bounds49o',[fixture/'52-chariot.psg'],{'RR_BOUNDS49O_COVERAGE':str(out/'bounds-coverage.json')}),
   ('pod_model_geometry49o',[E,composition],{}),
   ('authored_body_diagnostic49o',[ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy',fixture,out/'trace.json'],{})]
  for binary,args,extra in reruns:
   recorded=next(test['executable']for test in report['tests']if test['name']==binary)
   executable=directory/binary
   assert executable.resolve()==Path(recorded['path']).resolve()
   assert executable.stat().st_size==recorded['bytes'] and sha(executable)==recorded['sha256']
   result=subprocess.run([str(directory/binary),*[str(x)for x in args]],cwd=ROOT,env=env|extra,text=True,capture_output=True)
   assert result.returncode==0 and not result.stderr,(binary,result.stderr)
   assert sha(executable)==recorded['sha256'],binary
   (out/(binary+'-output.stdout')).write_text(result.stdout)
  row['replayed_files']={n:sha(out/n)for n in ['initialization-coverage.json','bounds-coverage.json','trace.json','composition/baseline.json','composition/cases.json','composition/coverage.json']}
  summary['runs'][mode]=row
 assert summary['runs']['normal']['tests']==summary['runs']['sanitized']['tests']
 assert summary['runs']['normal']['replayed_files']==summary['runs']['sanitized']['replayed_files']
 assert (E/'final-normal/trace.json').read_bytes()==(E/'connected/persistent-authored-two-body49o.json').read_bytes()
 for n in ['baseline.json','cases.json','coverage.json']:
  assert (E/'final-normal/composition'/n).read_bytes()==(E/'composition'/n).read_bytes(),n
 summary['checks_each']=summary['runs']['normal']['checks']
 (E/'regression-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
 print(json.dumps({'status':'passed','suites_each':37,'checks_each':summary['checks_each'],'tracked_inputs_each':228,'stdout_trace_coverage_identical':True}))
if __name__=='__main__':main()
