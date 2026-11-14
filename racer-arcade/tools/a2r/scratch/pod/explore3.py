import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
for n in sys.argv[1:]:
    m, order, info = read_res(R % (n, n))
    for name, lo, nb in order:
        l = name.lower()
        if l.endswith('.psg') and 'chariot' not in l:
            c = geo_psg.parse(m[l])
            print(n, name, nb, 'nodes', [(nd['name'], nd['parent']) for nd in c['nodes']], 'mats', c['materials'], 'world', c['world'], 'lods', [(round(L['cutoff'],2), len(L['surfaces'])) for L in c['lods']], 'q', [round(x,6) for x in c['quant']])
    phy = m[r'pods\pod%s\pod%s.phy' % (n, n)].decode('latin1')
    print(phy)
