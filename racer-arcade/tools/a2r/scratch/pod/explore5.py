import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_pst
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for name, lo, nb in order:
    l = name.lower()
    if l.endswith('.psm') and (n in l or 'wndw' in l):
        print('=====', name); print(m[l].decode('latin1'))
    if l.endswith('.pst'):
        t = geo_pst.parse(m[l]); print('PST', name, t['kind'], [(L['w'], L['h']) for L in t['levels']])
