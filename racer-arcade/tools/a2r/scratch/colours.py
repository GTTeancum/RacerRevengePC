import sys,collections,os;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
import geo_psg, numpy as np
G=r'C:\Games\Star Wars Racer Revenge'
for t in sys.argv[1:]:
    m,o,i=read_res(os.path.join(G,'TRACKS',t+'.RES'))
    for world in (1,0):
        C=[]
        for name,b in m.items():
            if not name.endswith('.psg'): continue
            M=geo_psg.parse(b)
            if M['world']!=world: continue
            for s in M['lods'][0]['surfaces']:
                for g in s['groups']:
                    if g['rgba']: C+=g['rgba']
        C=np.array(C)
        if len(C)==0: continue
        print(t,'world',world,len(C),'rgb pct 1/50/99',np.percentile(C[:,:3],[1,50,99]).round(),'max',C[:,:3].max(),'alpha',collections.Counter(C[:,3].tolist()).most_common(5))
