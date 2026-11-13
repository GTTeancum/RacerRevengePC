"""Round trip of the cut-camera .vub files of the 13 original tracks:
raw re-write, and rebuild from (template record 0 + per-frame look/up/pos/fov)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_res3 import read_res3
import trk_vub
R = r'C:/Games/Star Wars Racer Revenge/TRACKS/'
n = raw_ok = rebuild_ok = 0
fovs = set(); notes = []
for t in 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split():
    r = read_res3(open(R + t + '.RES', 'rb').read())
    for name, d in r.items():
        if not name.lower().endswith('.vub'):
            continue
        n += 1
        fr = trk_vub.read_vub(d)
        raw_ok += trk_vub.write_vub_raw(fr) == d
        keys = [(f['look'], f['up'], f['pos'], f['fov']) for f in fr]
        rb = trk_vub.make_vub(fr[0]['raw'], fr[0]['name'], keys)
        if rb == d:
            rebuild_ok += 1
        else:
            notes.append(t + ':' + name)
        fovs |= set(round(f['fov'], 2) for f in fr)
        if fr[0]['name'].lower() != name.lower().replace('.vub', '.vue'):
            notes.append('name field %s in %s' % (fr[0]['name'], name))
print('VUB files', n, 'raw byte-identical', raw_ok, 'rebuilt from template+keys', rebuild_ok)
print('not rebuilt identically:', notes[:20])
print('fov values', sorted(fovs)[:20], '...', len(fovs))
