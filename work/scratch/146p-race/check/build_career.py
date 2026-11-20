import pathlib,subprocess,sys,concurrent.futures
# build_check.py: host-only registry_check.exe from worktree sources (run inside vs.cmd).
WT=pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a1ea896c20dcc435e')
HERE=WT/'work'/'scratch'/'146p-race'/'check';OUT=HERE/'obj';OUT.mkdir(parents=True,exist_ok=True)
flags=['/nologo','/c','/std:c++20','/EHsc','/O2','/MD','/utf-8','/Zc:__cplusplus','/DNDEBUG','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN',
 '/W3','/I'+str(WT/'src'/'assets'),'/I'+str(WT/'third_party'/'native-zlib'),'/I'+str(WT/'third_party'/'native-json')]
srcs=[HERE/'career_check.cpp']+[WT/p for p in ['src/assets/dlc_package50dl.cpp','src/assets/dlc_files50dl.cpp','src/assets/dlc_registry50dl.cpp',
 'src/assets/import_decode48c.cpp','src/assets/dlc_progress50dl.cpp','src/game/dlc_rider_entries50dl.cpp']]+[WT/'third_party'/'native-zlib'/(n+'.c') for n in ('adler32','crc32','inflate','inffast','inftrees','zutil')]
def comp(s):
    obj=OUT/(s.stem+'.obj')
    if s.suffix=='.c' and obj.exists():return (s,0,'cached')
    f=[x for x in flags if not (s.suffix=='.c' and x in ('/std:c++20','/EHsc','/Zc:__cplusplus'))]
    r=subprocess.run(['cl',*f,'/Fo'+str(obj),str(s)],capture_output=True,text=True,errors='replace')
    return (s,r.returncode,r.stdout+r.stderr)
ok=True
with concurrent.futures.ThreadPoolExecutor(4) as ex:
    for s,rc,out in ex.map(comp,srcs):
        lines=[l for l in out.splitlines() if 'error' in l or 'warning' in l]
        print(s.name,'OK' if rc==0 else 'FAIL')
        for l in lines[:30]:print('  ',l[:400])
        ok=ok and rc==0
if not ok:sys.exit(1)
r=subprocess.run(['link','/nologo','/OUT:'+str(HERE/'career_check.exe'),*[str(OUT/(s.stem+'.obj')) for s in srcs]],capture_output=True,text=True,errors='replace')
print('link',r.returncode);print('\n'.join(l for l in (r.stdout+r.stderr).splitlines() if 'error' in l.lower())[:3000])
sys.exit(r.returncode)
