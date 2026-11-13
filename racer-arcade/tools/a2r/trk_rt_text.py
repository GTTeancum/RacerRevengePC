"""Round trip of every INI-style text member of the 13 original track RES files.
lossless: parse -> write_lossless byte-identical (must be 100 %).
canonical: semantic sections -> write_canonical byte-identical (shows which file types the
original tools wrote in the canonical machine layout that the converter uses).
Also: gc.scn / gc.toy compiled with trk_binparam.compile_ini against gc.scb / gc.tob."""
import sys, os, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_res3 import read_res3
import trk_ini, trk_binparam
R = r'C:/Games/Star Wars Racer Revenge/TRACKS/'
BS = chr(92)
TEXT = ('spl', 'pos', 'tun', 'wtt', 'ini', 'prt', 'emt', 'scn', 'toy', 'acb', 'mcf', 'psm', 'vut')
lossless = collections.Counter(); canon = collections.Counter(); total = collections.Counter()
notcanon = collections.defaultdict(list); fails = []
gc = {}
for t in 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split():
    r = read_res3(open(R + t + '.RES', 'rb').read())
    for n, d in r.items():
        if t == 'GC':
            gc[n.lower()] = d
        e = n.rsplit('.', 1)[-1].lower()
        if e not in TEXT:
            continue
        total[e] += 1
        ini = trk_ini.parse(d)
        if trk_ini.write_lossless(ini) == d: lossless[e] += 1
        else: fails.append((t, n))
        if trk_ini.write_canonical(ini.sections()) == d: canon[e] += 1
        else: notcanon[e].append(t + ':' + n)
for e in sorted(total):
    print('%-4s files %4d lossless %4d canonical %4d  e.g. not canonical: %s' % (
        e, total[e], lossless[e], canon[e], notcanon[e][:3]))
print('lossless failures', fails)
for src, binn in (('gc.scn', 'tracks' + BS + 'gc.scb'), ('tracks' + BS + 'gc.toy', 'tracks' + BS + 'gc.tob')):
    secs = trk_ini.parse(gc[src]).sections()
    b = trk_binparam.compile_ini(secs)
    same = b == gc[binn]
    print('compile', src, '->', binn, 'byte-identical', same, len(b), len(gc[binn]))
    if not same:
        A = trk_binparam.decode(b); B = trk_binparam.decode(gc[binn])
        da = dict((n, kv) for n, kv in A); db = dict((n, kv) for n, kv in B)
        print('  blocks only in compiled', sorted(set(da) - set(db))[:10], 'only in original', sorted(set(db) - set(da))[:10])
        k = 0
        for n in sorted(set(da) & set(db)):
            if da[n] != db[n]:
                print('  differs', n, da[n][:6], '|', db[n][:6]); k += 1
                if k > 6: break
