#!/usr/bin/env python3
"""Incremental focused Linux build from a retained g++ command JSON.
Usage: build_focused_incremental50ci.py COMMAND_JSON CACHE_DIR
Hashes dependency files, compiler flags and object bytes before reuse. Serial.
Existing prebuilt .o arguments stay explicit and are hashed in the final receipt.
This runs no tests and makes no validation claim for the supplied prior objects.
"""
from pathlib import Path
import hashlib,json,subprocess,sys,shlex

def digest(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def main():
 if len(sys.argv)!=3:raise SystemExit(__doc__)
 command=json.loads(Path(sys.argv[1]).read_text());cache=Path(sys.argv[2]).resolve();cache.mkdir(parents=True,exist_ok=True)
 compiler=command[0];version=subprocess.check_output([compiler,'--version'],text=True).splitlines()[0]
 sources=[x for x in command[1:]if x.endswith(('.cpp','.cc','.cxx'))]
 outputIndex=command.index('-o');skip={outputIndex,outputIndex+1}
 flags=[x for i,x in enumerate(command[1:],1)if i not in skip and x not in sources and not x.endswith(('.o','.a','.so')) and not x.startswith('-l')]
 identity=hashlib.sha256(json.dumps([version,flags]).encode()).hexdigest();objects={};records=[]
 for source in sources:
  key=hashlib.sha256(str(Path(source).resolve()).encode()).hexdigest()[:24];obj=cache/(key+'.o');meta=cache/(key+'.json');dep=cache/(key+'.d');prior={}
  if meta.exists():
   try:prior=json.loads(meta.read_text())
   except (ValueError,OSError):pass
  valid=False
  if obj.exists() and prior.get('identity')==identity and prior.get('dependencies'):
   valid=prior.get('object_sha256')==digest(obj) and all(Path(p).is_file()and digest(p)==h for p,h in prior['dependencies'].items())
  if not valid:
   args=[compiler]+flags+['-MMD','-MF',str(dep),'-c',source,'-o',str(obj)]
   run=subprocess.run(args,capture_output=True,text=True);(cache/(key+'.stdout.log')).write_text(run.stdout);(cache/(key+'.stderr.log')).write_text(run.stderr)
   if run.returncode:print(run.stderr,file=sys.stderr);raise SystemExit(run.returncode)
   raw=dep.read_text().replace('\\\n',' ');deps=shlex.split(raw.split(':',1)[1]);prior={'identity':identity,'source':source,'dependencies':{str(Path(p).resolve()):digest(p)for p in deps},'object_sha256':digest(obj)};meta.write_text(json.dumps(prior,indent=2))
  objects[source]=str(obj);records.append({'source':source,'reused':valid,'object':str(obj),'sha256':prior['object_sha256']})
 link=[objects.get(x,x)for x in command];p=subprocess.run(link,capture_output=True,text=True);(cache/'link.stdout.log').write_text(p.stdout);(cache/'link.stderr.log').write_text(p.stderr)
 receipt={'compiler':version,'compiled':sum(not x['reused']for x in records),'reused':sum(x['reused']for x in records),'objects':records,'supplied_objects':{x:digest(x)for x in command if x.endswith('.o')},'link_command':link,'link_exit':p.returncode}
 (cache/'last-build.json').write_text(json.dumps(receipt,indent=2));print(json.dumps({k:receipt[k]for k in ['compiled','reused','link_exit']}),flush=True)
 if p.returncode:print(p.stderr,file=sys.stderr);raise SystemExit(p.returncode)
if __name__=='__main__':main()
