import pathlib,json,re,hashlib,subprocess,concurrent.futures,sys,os
# Worktree incremental relink (equivalent of build\relink-next.py for a second builder).
#   relink-worktree.py <base-dir> <out-dir> [new.cpp=template.cpp ...]
# <base-dir>/<out-dir> are folder names under D:\Programming\GitHub\RacerRevengePC\build.
# <out-dir> must start with "dlc-" and must not exist.
# Sources come from the worktree (WT). Base receipts made by the main tree name
# MAIN paths; every MAIN\{src,platform,tests,third_party} path is mapped to WT.
# The main tree checks files out with CRLF, the worktree with LF: a file counts as
# unchanged when its bytes equal the receipt hash, or when the receipt hash still
# matches the main-tree file and the two differ only in line endings.
# Objects are reused read-only from parent folders (takeover-* and dlc-*); nothing
# is written outside <out-dir>.
MAIN=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC')
WT=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a6f450de113488656')
TOPS=('src','platform','tests','third_party')
base=MAIN/'build'/sys.argv[1];out=MAIN/'build'/sys.argv[2]
assert out.name.startswith('wt-145d-'),'output folder must be named wt-145d-*'
out.mkdir(exist_ok=False)
pairs=[a.split('=',1) for a in sys.argv[3:]]
def sha(p):
 with open(p,'rb') as f:return hashlib.file_digest(f,'sha256').hexdigest().upper()
def nsha(p):return hashlib.sha256(pathlib.Path(p).read_bytes().replace(b'\r\n',b'\n')).hexdigest().upper()
def tomap(p):
 p=pathlib.Path(p)
 try:rel=p.relative_to(MAIN)
 except ValueError:return p.resolve()
 if rel.parts and rel.parts[0] in TOPS:return (WT/rel).resolve()
 return p.resolve()
def remap_text(t):
 for top in TOPS:
  t=re.sub(re.escape(str(MAIN/top)),lambda m:str(WT/top),t,flags=re.I)
 return t
r=json.loads((base/'build-receipt.json').read_text(encoding='utf-8-sig'))
changed=set();old={}
for x in r['inputHashes']:
 src=pathlib.Path(x['Path']);p=tomap(src);h=x['Hash'].upper();old[p]=h
 if not p.exists():changed.add(p);continue
 if sha(p)==h:continue
 lf=p.read_bytes().replace(b'\r\n',b'\n')
 if hashlib.sha256(lf.replace(b'\n',b'\r\n')).hexdigest().upper()==h:continue
 if src!=p and src.exists() and sha(src)==h and nsha(src)==nsha(p):continue
 changed.add(p)
lookup={}
for folder in TOPS:
 for p in (WT/folder).rglob('*'):
  if p.is_file() and p.suffix in ('.h','.hpp','.cpp','.c','.inl','.inc'):lookup.setdefault(p.name,[]).append(p.resolve())
cache={}
def dependencies(p,seen):
 if p in seen:return
 seen.add(p)
 if not p.exists():return
 if p not in cache:
  targets=[]
  for name in re.findall(r'^\s*#\s*include\s*[<"]([^>"\n]+)',p.read_text(errors='replace'),re.M):
   direct=(p.parent/name).resolve()
   if direct.exists():targets.append(direct)
   else:targets.extend(lookup.get(pathlib.Path(name).name,[]))
  cache[p]=targets
 for q in cache[p]:dependencies(q,seen)
parents=[p.name for p in sorted((MAIN/'build').iterdir(),key=lambda p:p.stat().st_mtime,reverse=True)
         if p.is_dir() and (p.name.startswith('takeover-') or p.name.startswith('dlc-') or p.name.startswith('wt-145d-')) and p!=out]
link=(base/'link.rsp').read_text(encoding='utf-16')
link=link.replace('/SUBSYSTEM:CONSOLE','/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup')
oldexe=re.search(r'/OUT:"([^"]+)"',link)[1];exe=str(out/pathlib.Path(oldexe).name);link=link.replace(oldexe,exe)
commands=[];newobjects=[];all_inputs=set(old)
def find_rsp(source,require_linked=True):
 forms=[str(source).lower()]
 try:forms.append(str(MAIN/source.relative_to(WT)).lower())
 except ValueError:pass
 for parent in parents:
  for p in (MAIN/'build'/parent).glob('*'+source.stem+'.rsp'):
   t=p.read_text(encoding='utf-16');tl=t.lower()
   if any(f in tl for f in forms) and (not require_linked or re.search(r'/Fo"([^"]+)"',t)[1] in link):return p,remap_text(t)
 raise SystemExit('no response file for '+str(source))
known={tomap(row['source']) for row in r['compileResults']}
for row in r['compileResults']:
 source=tomap(row['source']);deps=set();dependencies(source,deps);all_inputs.update(deps)
 if not deps.intersection(changed):continue
 rsp,args=find_rsp(source);oldobj=re.search(r'/Fo"([^"]+)"',args)[1];newobj=str(out/pathlib.Path(oldobj).name)
 if newobj in newobjects:continue
 newobjects.append(newobj);(out/rsp.name).write_text(args.replace(oldobj,newobj),encoding='utf-16');link=link.replace(oldobj,newobj);commands.append((r['compiler'],rsp.name,str(source)))
 row['source']=str(source)
for new,template in pairs:
 source=(WT/new).resolve();tsource=(WT/template).resolve()
 assert source.is_file() and source not in known,('new source missing or already built',source)
 deps=set();dependencies(source,deps);all_inputs.update(deps)
 _,targs=find_rsp(tsource,require_linked=False)
 i=targs.lower().index(str(tsource).lower());args=targs[:i]+str(source)+targs[i+len(str(tsource)):]
 oldobj=re.search(r'/Fo"([^"]+)"',args)[1];newobj=str(out/(source.stem+'.obj'))
 assert newobj not in newobjects and newobj not in link,('object name collision',newobj)
 args=args.replace(oldobj,newobj);name='new-'+source.stem+'.rsp'
 (out/name).write_text(args,encoding='utf-16');newobjects.append(newobj)
 objs=list(re.finditer(r'^"[^"]+\.obj"\s*$',link,re.M));assert objs,'link.rsp has no object lines'
 at=objs[-1].end();link=link[:at]+'\n"'+newobj+'"'+link[at:]
 commands.append((r['compiler'],name,str(source)))
 r['compileResults'].append(dict(source=str(source),state='completed',exitCode=0,stdoutLog=str(out/(name+'.log')),stderrLog=''))
assert commands,'nothing to rebuild'
if os.environ.get('DLC_DRY')=='1':
 import shutil;print('DRY: would rebuild',len(commands),'sources; changed inputs:',len(changed))
 for c in sorted(changed):print('  changed',c)
 shutil.rmtree(out);sys.exit(0)
(out/'link.rsp').write_text(link,encoding='utf-16')
inputs={str(p):sha(p) for p in all_inputs if p.is_file()};removed=[str(p) for p in old if not p.exists()]
objects=[p for p in re.findall(r'^"([^\"]+\.obj)"',link,re.M) if p not in newobjects];object_hashes={p:sha(p) for p in objects}
print('Rebuilding',len(commands),'sources',flush=True)
(out/'plan.json').write_text(json.dumps(dict(changed=sorted(map(str,changed)),commands=commands,removed=removed),indent=2))
def run(command):
 tool,response,*_=command
 with (out/(response+'.log')).open('wb') as f:subprocess.run([tool,'@'+str(out/response)],stdout=f,stderr=subprocess.STDOUT,check=True,cwd=WT)
 print('Completed',response,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=int(os.environ.get('WT_JOBS','2'))) as pool:list(pool.map(run,commands))
run((r['linker'],'link.rsp'))
assert all(sha(p)==h for p,h in inputs.items()),'an input changed during the build'
assert all(sha(p)==h for p,h in object_hashes.items()),'a reused object changed during the build'
r.update(status='built-not-run',executable=exe,executableSha256=sha(exe),parentReceipt=str(base/'build-receipt.json'),
 sourceRoot=str(WT),rebuiltSources=[c[2] for c in commands],reusedObjectHashes=object_hashes,
 inputHashes=[dict(Path=p,Hash=h) for p,h in inputs.items()],removedInputs=removed,
 incrementalNote='Worktree relink (work\\scratch\\dlc\\build\\relink-worktree.py). Sources from sourceRoot; main-tree paths in parent receipts mapped to it; CRLF-only differences from the parent are treated as unchanged. Transitive local include scan over-approximates. Inputs and reused objects verified unchanged after compile/link.')
(out/'build-receipt.json').write_text(json.dumps(r,indent=2));print(exe,sha(exe))
