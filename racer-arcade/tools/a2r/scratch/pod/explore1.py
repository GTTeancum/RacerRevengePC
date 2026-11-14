import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
names = {}
for n in ['04','11','15','17','23','24']:
    m, order, info = read_res(R % (n, n))
    names[n] = [o[0] for o in order]
    print(n, info['members'], info['bytes'])
base = [x.replace('04', 'NN') for x in names['04']]
for n in names:
    s = [x.replace(n, 'NN') for x in names[n]]
    print(n, 'extra vs 04:', [x for x in s if x not in base], 'missing:', [x for x in base if x not in s])
for n in ['04','11','15','17','23','24']:
    m, order, info = read_res(R % (n, n))
    c = geo_psg.parse(m[r'pods\pod%s\chariot.psg' % n])
    print(n, 'chariot nodes', len(c['nodes']), [ (i,nd['name'],nd['parent']) for i,nd in enumerate(c['nodes'])])
    print('   mats', c['materials'], 'quant', c['quant'], 'world', c['world'], 'lods', len(c['lods']), [len(l['surfaces']) for l in c['lods']], [l['cutoff'] for l in c['lods']])
