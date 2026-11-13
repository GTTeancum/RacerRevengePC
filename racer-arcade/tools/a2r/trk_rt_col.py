import sys, os, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_res3 import read_res3
from trk_col import read_col, write_col
R = r'C:/Games/Star Wars Racer Revenge/TRACKS/'
c = collections.Counter(); bad = []
for t in 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split():
    r = read_res3(open(R + t + '.RES', 'rb').read())
    for n, d in r.items():
        if not n.lower().endswith('.col'):
            continue
        try:
            col = read_col(d)
            ok = write_col(col) == d
        except Exception as x:
            ok = False; n = n + ' ' + repr(x)
        c[(col.type, col.members[0][1].quantized, ok)] += 1
        if not ok: bad.append((t, n))
print('COL (type, quantized, byte-identical): count', dict(c))
print('failures', bad)
