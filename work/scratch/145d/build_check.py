import pathlib,subprocess,sys,concurrent.futures
# build_check.py [extra.cpp ...]: host-only anim_check.exe from worktree sources.
WT=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a6f450de113488656')
OUT=WT/'work'/'scratch'/'145d'/'check'/'obj';OUT.mkdir(parents=True,exist_ok=True)
GUARD=r'D:\Programming\GitHub\RacerRevengePC\build\takeover-base-133a\native-x64-guard.h'
flags=['/nologo','/c','/std:c++20','/EHsc','/O2','/MD','/fp:strict','/bigobj','/utf-8','/Zc:__cplusplus','/DNDEBUG','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN',
 '/FI'+GUARD,'/I'+str(WT/'tests'/'native'),'/I'+str(WT/'src'/'assets'),'/I'+str(WT/'src'/'game'),'/I'+str(WT/'third_party'/'native-json'),'/I'+str(WT/'third_party'/'native-zlib'),'/I'+str(WT/'third_party'/'ffmpeg-7.1.5'/'include')]
srcs=[WT/'work'/'scratch'/'145d'/'check'/'anim_check.cpp']+[WT/p for p in sys.argv[1:]]
def comp(s):
    obj=OUT/(s.stem+'.obj')
    if obj.exists() and obj.stat().st_mtime>s.stat().st_mtime and s.name!='anim_check.cpp':return (s,0,'cached')
    f=[x for x in flags if not (s.suffix=='.c' and x in ('/std:c++20','/EHsc','/Zc:__cplusplus'))]
    r=subprocess.run(['cl',*f,'/Fo'+str(obj),str(s)],capture_output=True,text=True,errors='replace')
    return (s,r.returncode,r.stdout+r.stderr)
ok=True
with concurrent.futures.ThreadPoolExecutor(4) as ex:
    for s,rc,out in ex.map(comp,srcs):
        errs=[l for l in out.splitlines() if 'error' in l]
        print(s.name,'OK' if rc==0 else 'FAIL');
        for l in errs[:30]:print('  ',l[:500])
        ok=ok and rc==0
if not ok:sys.exit(1)
objs=[str(OUT/(s.stem+'.obj')) for s in srcs]
r=subprocess.run(['link','/nologo','/OUT:'+str(WT/'work'/'scratch'/'145d'/'check'/'anim_check.exe'),*objs],capture_output=True,text=True,errors='replace')
print('link',r.returncode);print('\n'.join([l for l in (r.stdout+r.stderr).splitlines() if 'error' in l or 'unresolved' in l.lower()][:60]))
sys.exit(r.returncode)
