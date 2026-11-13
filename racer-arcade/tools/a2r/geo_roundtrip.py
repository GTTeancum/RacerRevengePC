r"""Round-trip proof for the geometry writers: decode every original PSG / PST / PSM in the
installed Revenge files (C:\Games\Star Wars Racer Revenge, read-only) with geo_psg / geo_pst /
geo_psm, re-encode, compare byte for byte.   python -B -I geo_roundtrip.py [--quick]"""
import sys, os, glob, collections, hashlib, json
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from geo_res import read_res
import geo_psg, geo_pst, geo_psm
G = r'C:\Games\Star Wars Racer Revenge'
TRACKS = 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split()
files = [os.path.join(G, 'TRACKS', t + '.RES') for t in TRACKS] + sorted(glob.glob(os.path.join(G, 'PODS', '*', '*.RES')))
if '--quick' in sys.argv:
    files = [G + r'\TRACKS\TA.RES', G + r'\PODS\POD01\POD01.RES']
res = {k: collections.Counter() for k in ('psg', 'pst', 'psm')}
fails = collections.defaultdict(list)
seen = set()
for f in files:
    m, order, info = read_res(f)
    for name, b in m.items():
        ext = name[-3:]
        if ext not in res:
            continue
        h = hashlib.sha1(b).digest()
        dup = h in seen
        seen.add(h)
        mod = dict(psg=geo_psg, pst=geo_pst, psm=geo_psm)[ext]
        try:
            out = mod.write(mod.parse(b))
            ok = out == b
        except Exception as e:  # noqa
            ok = False
            fails[ext].append((os.path.basename(f), name, 'EXC %s' % e))
        else:
            if not ok:
                fails[ext].append((os.path.basename(f), name, 'differs'))
        res[ext]['files'] += 1
        res[ext]['unique' if not dup else 'dup'] += 1
        res[ext]['identical' if ok else 'different'] += 1
        if ext == 'psm' and ok:
            res[ext]['canonical' if mod.parse(b)['canonical'] else 'raw_lines_kept'] += 1
for k, c in res.items():
    print(k, dict(c))
    for x in fails[k][:8]:
        print('   ', x)
json.dump({k: dict(c) for k, c in res.items()}, open(os.path.join(HERE, 'scratch', 'roundtrip.json'), 'w'), indent=1)
