import os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools', 'a2r'))
import geo_res
m, o, i = geo_res.read_res(sys.argv[1])
for n in sys.argv[2:]:
    for k in m:
        if k.endswith(n):
            print('=====', k); print('\n'.join(l for l in m[k].decode('latin1').splitlines() if '=' in l and l.split('=',1)[1].strip() or l.startswith('[')))
