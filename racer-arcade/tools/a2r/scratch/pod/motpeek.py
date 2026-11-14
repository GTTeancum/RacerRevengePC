import sys, os, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
from geo_res import read_res
import geo_psg
R = r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES'
def rows(psg):
    # enumeration 152EA0: siblings in source order, then each sibling's child group
    nodes = psg['nodes']
    kids = {}
    for i, n in enumerate(nodes): kids.setdefault(n['parent'], []).append(i)
    out = []
    def group(g):
        out.extend(g)
        for i in g:
            if i in kids: group(kids[i])
    group(kids[-1])
    return out
def mot(b):
    tag, nf, dur, rt, one, npos, nrot = struct.unpack_from('<IIfIIII', b, 0)
    at = 28; P = []; Rr = []
    for ch, cnt in ((0, npos), (1, nrot)):
        st = 40 if ch == 0 else (12 if rt else 8)
        for i in range(cnt):
            k, row = struct.unpack_from('<II', b, at); at += 8
            keys = [b[at + j*st: at + (j+1)*st] for j in range(k)]; at += k*st
            (P if ch == 0 else Rr).append((row, keys))
    assert at == len(b)
    return dict(frames=nf, dur=dur, rtype=rt, pos=P, rot=Rr)
n = sys.argv[1]
m, order, info = read_res(R % (n, n))
for name, lo, nb in order:
    l = name.lower()
    if l.endswith('.mot'):
        part = 'chariot' if '_' in os.path.basename(l)[:3] or os.path.basename(l)[:2]==n else ('englt1' if 'lt' in l else 'engrt1')
        part = 'chariot' if os.path.basename(l).startswith(n) else ('englt1' if 'englt' in l else 'engrt1')
        ps = geo_psg.parse(m[r'pods\pod%s\%s.psg' % (n, part)])
        rw = rows(ps)
        M = mot(m[l])
        print(name, part, 'frames', M['frames'], 'dur', M['dur'], 'rtype', M['rtype'])
        for row, keys in M['pos']:
            k0 = struct.unpack_from('<10f', keys[0]); kl = struct.unpack_from('<10f', keys[-1])
            print('   POS row %d=%s keys %d first %s last %s' % (row, ps['nodes'][rw[row]]['name'], len(keys), [round(x,3) for x in k0[:4]], [round(x,3) for x in kl[:4]]))
        for row, keys in M['rot']:
            fmt = '<fHhhh' if M['rtype'] else '<fBbbb'
            k0 = struct.unpack_from(fmt, keys[0]); kl = struct.unpack_from(fmt, keys[-1])
            print('   ROT row %d=%s keys %d first %s last %s' % (row, ps['nodes'][rw[row]]['name'], len(keys), k0, kl))
