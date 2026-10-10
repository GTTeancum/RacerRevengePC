#!/usr/bin/env python3
"""Reproduce exact Pod01 PHY extraction with the preserved bounded decoder."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--build-dir',type=Path,required=True);a=ap.parse_args()
    b=a.build_dir.resolve();assert not b.exists();b.mkdir(parents=True)
    resource=ROOT/'work/native/project/disc/PODS/POD01/POD01.RES';fixture=ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy'
    assert sha(resource)=='08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3'
    assert sha(fixture)=='47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b'
    sources=['evidence/authoredInitialization49N/extract_pod_fixture49n.cpp','src/assets/import_decode48c.cpp'];results=[]
    for mode in ['normal','sanitized']:
        exe=b/mode;output=b/(mode+'.phy')
        flags=['-std=c++17','-O2'] if mode=='normal' else ['-std=c++17','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all']
        command=['g++',*flags,'-I',str(ROOT/'src/assets'),*[str(ROOT/p) for p in sources],'-lz','-o',str(exe)]
        build=subprocess.run(command,cwd=ROOT,capture_output=True,text=True);assert build.returncode==0,build.stderr
        env=os.environ|{'ASAN_OPTIONS':'detect_leaks=0:halt_on_error=1','UBSAN_OPTIONS':'halt_on_error=1'}
        run=subprocess.run([str(exe),str(resource),str(output)],cwd=ROOT,env=env,capture_output=True,text=True);assert run.returncode==0 and not run.stderr,run.stderr
        assert output.read_bytes()==fixture.read_bytes()
        results.append({'mode':mode,'command':command,'build_returncode':build.returncode,'run_returncode':run.returncode,'stdout':run.stdout,'member_sha256':sha(output),'exact_original_fixture':True})
    paths=sources+['src/assets/import_decode48c.h','tools/verify_pod_resource49n.py']
    result={'status':'passed','resource_sha256':sha(resource),'fixture_sha256':sha(fixture),'results':results,'source_hashes':{p:sha(ROOT/p) for p in paths},'limits':'Valid original resource extraction only. Full RES decompression is separate from the zero-allocation native text/parameter APIs; no allocator-failure tests.'}
    (ROOT/'evidence/authoredInitialization49N/resource-extraction.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
