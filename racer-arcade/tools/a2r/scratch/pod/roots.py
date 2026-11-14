import sys, os, glob
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg
for p in sorted(glob.glob(r'C:\Games\Star Wars Racer Revenge\PODS\POD*\POD*.RES')):
    n = os.path.basename(p)[3:5]
    m, order, info = read_res(p)
    out = []
    for part in ('chariot', 'englt1', 'engrt1'):
        c = geo_psg.parse(m[r'pods\pod%s\%s.psg' % (n, part)])
        M = np.array(c['nodes'][0]['matrix']).reshape(4, 4)
        h = c['nodes'][0]['half']
        out.append('%s T=%s half=%s' % (part[:3], np.round(M[3, :3], 2), np.round(h, 2)))
    phy = m[r'pods\pod%s\pod%s.phy' % (n, n)].decode('latin1')
    lep = [l for l in phy.splitlines() if 'EnginePosition' in l or 'FirstPerson' in l]
    print(n, ' | '.join(out), [l.split('=')[1].strip() for l in lep])
