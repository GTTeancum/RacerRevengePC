import sys, os, re
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg, geo_pst
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
for n in ['04','11','15','17','23','24']:
    m, order, info = read_res(R % (n, n))
    phy = m[r'pods\pod%s\pod%s.phy' % (n, n)].decode('latin1')
    parts = re.findall(r'^\s*Part\d+\s*=\s*(\S+)', phy, re.M)
    print(n, 'parts', parts, re.findall(r'Name\s*=.*', phy)[0], re.findall(r'EngineMotion.*', phy))
    for part in ('englt1', 'engrt1'):
        c = geo_psg.parse(m[r'pods\pod%s\%s.psg' % (n, part)])
        print('  ', part, [nd['name'] for nd in c['nodes']], c['materials'][:3])
    for nm in [o[0] for o in order if o[0].lower().endswith('.pst') and n in o[0]]:
        t = geo_pst.parse(m[nm.lower()]); print('  ', nm, t['kind'], t['levels'][-1]['w'], t['levels'][-1]['h'], len(t['levels']))
    print('  mcf', m[r'pods\pod%s\chariot.mcf' % n].decode().replace('\r\n',' | ')[:400])
