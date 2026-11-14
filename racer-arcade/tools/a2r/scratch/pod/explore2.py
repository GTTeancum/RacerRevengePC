import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for name, lo, nb in order:
    l = name.lower()
    if l.endswith(('.mcf', '.vut', '.ini', '.txt', '.ai', '.psm')) and ('pod' in l and 'textures' not in l or l.endswith(('.vut','.mcf'))):
        print('=====', name); print(m[l].decode('latin1'))
