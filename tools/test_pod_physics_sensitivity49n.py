#!/usr/bin/env python3
"""Five source mutations must be caught by the independent49N reader test.
Uses throwaway native files; never modifies production code or retained evidence.
"""
import sys
sys.dont_write_bytecode=True
import hashlib,json,os,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'src/assets/pod_physics49n.cpp'
MUTATIONS={
 'incorrect_normal_rounding':('else m+=63u;','else m+=0u;'),
 'preserve_negative_zero':('std::uint64_t binary64=0;','std::uint64_t binary64=std::uint64_t(negative)<<63;'),
 'wrong_source_key':('{"Physical Properties", "Weight", Key::Weight}', '{"Physical Properties", "Gravity", Key::Weight}'),
 'lost_tuning_presence':('result.tuning.sectionPresent=true;','result.tuning.sectionPresent=false;'),
 'duplicate_key_accepted':('keys[i].section==sectionCount-1 && equal(key,','false && keys[i].section==sectionCount-1 && equal(key,'),
}
def main():
 source=SOURCE.read_bytes();text=source.decode();results=[]
 with tempfile.TemporaryDirectory(prefix='pod-phy49n-sensitivity-') as directory:
  for name,(old,new) in MUTATIONS.items():
   if text.count(old)!=1:raise ValueError('mutation anchor not unique: '+name)
   path=Path(directory)/(name+'.cpp');path.write_text(text.replace(old,new));exe=path.with_suffix('')
   command=[os.environ.get('CXX','g++'),'-std=c++17','-O2','-I'+str(ROOT/'src/assets'),str(ROOT/'tests/native/pod_physics49n.cpp'),str(path),'-o',str(exe)]
   subprocess.run(command,cwd=ROOT,check=True,capture_output=True,text=True)
   run=subprocess.run([str(exe),str(ROOT/'evidence/authoredInitialization49N/authoring-source/Pod01.phy')],cwd=ROOT,capture_output=True,text=True)
   if run.returncode==0:raise RuntimeError('surviving mutation: '+name)
   results.append({'mutation':name,'exit_code':run.returncode,'output':run.stdout+run.stderr})
 if SOURCE.read_bytes()!=source:raise RuntimeError('production source changed during sensitivity test')
 print(json.dumps({'source_sha256':hashlib.sha256(source).hexdigest(),'production_unchanged':True,'mutations':results},indent=2))
if __name__=='__main__':main()
