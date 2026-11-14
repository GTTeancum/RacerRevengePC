r"""Read-only survey of original pod RES members: PSM text keys, MCF/MOT names (C:\Games\...\PODS)."""
import os, sys, glob, re
from collections import Counter, defaultdict
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools', 'a2r'))
import geo_res
keys = Counter(); vals = defaultdict(Counter); ex = {}
mats = Counter()
for p in sorted(glob.glob(r'C:\Games\Star Wars Racer Revenge\PODS\POD*\POD*.RES')):
    m, order, info = geo_res.read_res(p)
    pod = os.path.basename(p)
    for name, blob in m.items():
        if name.endswith('.psm'):
            txt = blob.decode('latin1', 'replace')
            mats[os.path.basename(name)] += 1
            for line in txt.splitlines():
                line = line.strip()
                if '=' in line:
                    k, v = [s.strip() for s in line.split('=', 1)]
                    keys[k] += 1; vals[k][v] += 1
                    ex.setdefault((k, v), '%s %s' % (pod, name))
                elif line.startswith('['):
                    keys[line] += 1
        if pod == 'POD04.RES' and not name.endswith(('.psm',)):
            pass
print('PSM keys:'); [print('  %-40s %5d  %s' % (k, n, dict(vals[k].most_common(8)) if k in vals else '')) for k, n in keys.most_common()]
print('PSM member names (count over pods):', sorted(set(re.sub(r'\d\d', 'nn', k) for k in mats)))
