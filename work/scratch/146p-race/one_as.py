import pathlib,re,subprocess,sys,json
# one_as.py <template-rel> <target-rel>: compile target-rel with the response
# file of template-rel (same flags), for a TU the base build has no rsp for.
MAIN=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC')
WT=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a1ea896c20dcc435e')
OUT=WT/'work'/'scratch'/'146p-race'/'obj';OUT.mkdir(exist_ok=True)
BASE=MAIN/'build'/'takeover-b145c'
r=json.loads((BASE/'build-receipt.json').read_text(encoding='utf-8-sig'))
TOPS=('src','platform','tests','third_party')
def remap(t):
    for top in TOPS:t=re.sub(re.escape(str(MAIN/top)),lambda m:str(WT/top),t,flags=re.I)
    return t
tmpl,target=sys.argv[1],sys.argv[2]
src=(WT/tmpl).resolve();mainsrc=str(MAIN/tmpl).lower()
hit=None
folders=sorted([d for d in (MAIN/'build').iterdir() if d.is_dir() and (d.name.startswith('takeover-') or d.name.startswith('wt-'))],key=lambda d:d.stat().st_mtime,reverse=True)
for d in folders:
    for p in d.glob('*'+src.stem+'.rsp'):
        t=p.read_text(encoding='utf-16')
        if mainsrc in t.lower():hit=t;break
    if hit:break
if not hit:print('no rsp for',tmpl);sys.exit(1)
t=remap(hit)
t=re.sub(re.escape(str(WT/tmpl)),lambda m:str(WT/target),t,flags=re.I)
tgt=(WT/target).resolve()
obj=re.search(r'/Fo"([^"]+)"',t)[1];t=t.replace(obj,str(OUT/(tgt.stem+'.obj')))
t=re.sub(r'/Fd"[^"]+"',lambda m:'/Fd"'+str(OUT/(tgt.stem+'.pdb'))+'"',t)
rp=OUT/(tgt.stem+'.rsp');rp.write_text(t,encoding='utf-16')
res=subprocess.run([r['compiler'],'@'+str(rp)],cwd=WT,capture_output=True,text=True,errors='replace')
out=res.stdout+res.stderr
errs=[l for l in out.splitlines() if ' error ' in l or 'fatal error' in l]
print(target,'OK' if res.returncode==0 else 'FAIL',len(errs))
for l in errs[:25]:print('  ',l[:400])
sys.exit(res.returncode)
