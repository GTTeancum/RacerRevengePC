import sys,glob,re,os
sys.path.insert(0,r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/tools/a2r')
import trk_res3
root=r'C:/Games/Star Wars Racer Revenge'
for f in glob.glob(root+'/**/*.RES',recursive=True):
    g=f.replace(os.sep,'/').upper()
    if '/TRACKS/' in g or '/PODS/' in g: continue
    try: r=trk_res3.read_res3(open(f,'rb').read())
    except Exception as e: print('skip',f,e); continue
    hits=[n for n in r.names() if re.search(r'map|radar|_sc[._]|\sc|trk|track|course',n,re.I)]
    if hits: print(f, hits[:40])
