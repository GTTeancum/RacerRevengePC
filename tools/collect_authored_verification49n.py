#!/usr/bin/env python3
"""Preserve completed final30-suite runs and reproduce matching coverage/trace."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
E=ROOT/'evidence/authoredInitialization49N'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def count(s):
    try:return int(json.loads(s)['checks'])
    except (json.JSONDecodeError,KeyError):
        m=re.search(r'checks=(\d+)',s) or re.search(r'(\d+)\s+(?:[A-Za-z/-]+\s+){0,4}checks',s)
        if not m:raise ValueError('Unknown test stdout: '+s[:150])
        return int(m.group(1))
def main():
    p=argparse.ArgumentParser();p.add_argument('--normal',type=Path,required=True);p.add_argument('--sanitized',type=Path,required=True);a=p.parse_args()
    summary={'status':'passed','runs':{},'all_mode_outputs_identical':True}
    for mode,directory in [('normal',a.normal),('sanitized',a.sanitized)]:
        report=json.loads((directory/'report.json').read_text())
        assert report['status']=='passed' and not report['changed_inputs'] and not report['errors']
        assert len(report['tests'])==30 and len(report['input_hashes'])==177
        assert report['sanitizers']==(mode=='sanitized')
        for n,h in report['input_hashes'].items():assert sha(ROOT/n)==h['sha256'],n
        out=E/f'final-{mode}';out.mkdir(exist_ok=True)
        (out/'regression-report.json').write_bytes((directory/'report.json').read_bytes())
        tests=[];total=0
        for test in report['tests']:
            assert test['status']=='passed' and test['run']['returncode']==0
            stdout=Path(test['run']['stdout']['path']);stderr=Path(test['run']['stderr']['path'])
            assert not stderr.read_bytes() and sha(stdout)==test['run']['stdout']['sha256']
            checks=count(stdout.read_text());total+=checks
            (out/(test['name']+'.stdout')).write_bytes(stdout.read_bytes())
            tests.append({'name':test['name'],'checks':checks,'stdout_sha256':sha(stdout),'stderr_bytes':0})
        assert total==32644848,total
        row={'suites':30,'checks':total,'tracked_inputs':177,'inputs_unchanged':True,'report_sha256':sha(out/'regression-report.json'),'tests':tests}
        env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
        for binary,filename in [('authored_parameters49n','coverage.json'),('authored_body_diagnostic49n','trace.json')]:
            arguments=([str(ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy')] if binary=='authored_body_diagnostic49n' else [])+[str(out/filename)]
            result=subprocess.run([str(directory/binary),*arguments],cwd=ROOT,env=env,text=True,capture_output=True)
            assert result.returncode==0 and not result.stderr,result.stderr
            (out/(binary+'-output.stdout')).write_text(result.stdout)
            row[filename+'_sha256']=sha(out/filename)
            if binary=='authored_parameters49n':row['boundaries_sha256']=sha(out/'coverage.json.boundaries.json')
        summary['runs'][mode]=row
    assert summary['runs']['normal']['tests']==summary['runs']['sanitized']['tests']
    for name in ['coverage.json','coverage.json.boundaries.json','trace.json']:
        assert (E/'final-normal'/name).read_bytes()==(E/'final-sanitized'/name).read_bytes(),name
    assert (E/'final-normal/coverage.json').read_bytes()==(E/'numeric-source/checks/normal-coverage.json').read_bytes()
    assert (E/'final-normal/trace.json').read_bytes()==(E/'connected/persistent-authored-two-body49n.json').read_bytes()
    (E/'regression-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
    print(json.dumps({'status':'passed','suites_each':30,'checks_each':32644848,'tracked_inputs_each':177,'stdout_coverage_trace_identical':True}))
if __name__=='__main__':main()
