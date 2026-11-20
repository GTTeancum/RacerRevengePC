import pathlib,re,subprocess,sys,json
# one.py <source-rel> [...]: compile single worktree TUs with the base build's
# response file into work\scratch\commonrest\obj (syntax/semantic check only).
MAIN=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC')
WT=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a2601093eb9a4eea9')
OUT=WT/'work'/'scratch'/'146c'/'obj';OUT.mkdir(exist_ok=True)
BASE=MAIN/'build'/'takeover-b145a'
r=json.loads((BASE/'build-receipt.json').read_text(encoding='utf-8-sig'))
TOPS=('src','platform','tests','third_party')
def remap(t):
    for top in TOPS:t=re.sub(re.escape(str(MAIN/top)),lambda m:str(WT/top),t,flags=re.I)
    return t
folders=sorted([d for d in (MAIN/'build').iterdir() if d.is_dir() and (d.name.startswith('takeover-') or d.name.startswith('dlc-') or d.name.startswith('wt-'))],key=lambda d:d.stat().st_mtime,reverse=True)
import concurrent.futures,threading
lock=threading.Lock()
ok=True
def job(rel):
    global ok
    lines=[]
    rel,_,tmpl=rel.partition('=')
    src=(WT/rel).resolve();look=(WT/(tmpl or rel)).resolve();mainsrc=str(MAIN/(tmpl or rel)).lower()
    hit=None
    for d in folders:
        for p in d.glob('*'+look.stem+'.rsp'):
            t=p.read_text(encoding='utf-16')
            if mainsrc in t.lower() or str(look).lower() in t.lower():hit=(p,t);break
        if hit:break
    if not hit:
        with lock:print('no rsp for',rel);ok=False
        return
    p,t=hit;t=remap(t)
    if tmpl:
        i=t.lower().index(str(look).lower());t=t[:i]+str(src)+t[i+len(str(look)):]
    obj=re.search(r'/Fo"([^"]+)"',t)[1];t=t.replace(obj,str(OUT/(src.stem+'.obj')))
    pdb='/Fd"'+str(OUT/(src.stem+'.pdb'))+'"';t=re.sub(r'/Fd"[^"]+"',lambda m:pdb,t)
    rp=OUT/(src.stem+'.rsp');rp.write_text(t,encoding='utf-16')
    res=subprocess.run([r['compiler'],'@'+str(rp)],cwd=WT,capture_output=True,text=True,errors='replace')
    out=res.stdout+res.stderr
    errs=[l for l in out.splitlines() if ' error ' in l or 'fatal error' in l]
    with lock:
        print(rel,'OK' if res.returncode==0 else 'FAIL',len(errs))
        for l in errs[:25]:print('  ',l[:400])
        if res.returncode:ok=False
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(job,sys.argv[1:]))
sys.exit(0 if ok else 1)
