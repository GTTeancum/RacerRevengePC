import sys,os,numpy as np
sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import pod_convert as C
for code,nn in C.TARGETS.items():
    p=C.Pod(code,nn); p.gather(); p.place_origin()
    print(code,nn,'origin',{k:(np.round(v,2).tolist() if isinstance(v,(list,np.ndarray)) else round(v,2)) for k,v in p.origin.items()})
    for side in (0,1):
        ex=p.engine_exits(side)
        print('  side',side,[(e['centre'].round(2).tolist(),round(e['radius'],2)) for e in ex[:6]])
