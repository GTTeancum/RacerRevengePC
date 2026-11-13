"""Inventory of every member of the 13 original track RES files (grouped by folder/type, with
per-track counts) and of the per-track files outside TRACKS\\.
Writes export\\a2r\\inventory.txt."""
import sys, os, re, collections, glob
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_res3 import read_res3
G = r'C:/Games/Star Wars Racer Revenge/'
T = 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split()
BS = chr(92)
out = []
groups = collections.defaultdict(lambda: collections.Counter())
examples = collections.defaultdict(list)
fixed = collections.defaultdict(set)
for t in T:
    r = read_res3(open(G + 'TRACKS/' + t + '.RES', 'rb').read())
    for n, d in r.items():
        folder = n.split(BS)[0].lower() if BS in n else '(root)'
        ext = n.rsplit('.', 1)[-1].lower()
        key = (folder, ext)
        groups[key][t] += 1
        if len(examples[key]) < 4:
            examples[key].append(n)
        if folder in ('tracks', 'audio', 'particle', 'skies', '(root)'):
            fixed[key].add(re.sub(t.lower(), 'xx', n.lower()))
out.append('== members of the 13 track RES files, by folder and type (count per track)')
for key in sorted(groups):
    c = groups[key]
    out.append('%-10s %-4s total %5d  %s' % (key[0], key[1], sum(c.values()), ' '.join('%s:%d' % (t, c[t]) for t in T if c[t])))
    out.append('      e.g. ' + ', '.join(examples[key]))
    if key in fixed:
        out.append('      names (xx = code): ' + ', '.join(sorted(fixed[key]))[:600])
out.append('')
out.append('== per-track files outside TRACKS')
for p in sorted(glob.glob(G + 'UI/TEXTURES/LD_*')):
    out.append('UI\\TEXTURES\\' + os.path.basename(p) + '  %d bytes' % os.path.getsize(p))
for res in ('UI/UICOURSE/UICOURSE.RES', 'UI/UIHOF/UIHOF.RES', 'UI/UITOURN/UITOURN.RES', 'UI/TEXTURES/OVERLAYS.RES', 'LOC.RES', 'AUDIO/ANNOUNCE.RES'):
    try:
        r = read_res3(open(G + res, 'rb').read())
    except Exception as e:
        out.append(res + ' unreadable: ' + repr(e)); continue
    names = r.names()
    hits = [n for n in names if re.search(r'(^|[^a-z])(ta|tb|tc|ra|rc|ma|mb|mc|ga|gb|gc|sa|sc)([^a-z]|$)|holo|track|course|map', n.lower())]
    out.append('%s: %d members; track-related %d: %s' % (res, len(names), len(hits), ', '.join(hits[:80])))
for d in sorted(glob.glob(G + 'AUDIO/SPEECH/DAX/TRACKS/*')):
    out.append('AUDIO\\SPEECH\\DAX\\TRACKS\\%s: %s' % (os.path.basename(d), ' '.join(sorted(os.listdir(d)))))
out.append('AUDIO\\MUSIC\\RACE\\INTROS: ' + ' '.join(sorted(os.listdir(G + 'AUDIO/MUSIC/RACE/INTROS'))))
open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'export', 'a2r', 'inventory.txt'), 'w').write('\n'.join(out) + '\n')
print('\n'.join(out))
