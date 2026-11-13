r"""Texture encoder check on originals: for every kind-1 PST of the given tracks / pods, decode the
largest level to RGBA (port rule: palette[swap bits 3,4 of index], smallest level first), encode it
again with geo_pst.encode_rgba (own palette, own box-filter mips), decode the result with the repo
decoder (tools\assets\decode_texture48d.py decode_texture) and compare:
  * largest level pixel-exact (an original has <= 256 colours, so quantising must be lossless),
  * level count / sizes equal to the original's,
  * regenerated smaller levels against the original's smaller levels (mean abs difference, 0..255).
python -B -I geo_pst_check.py [RES ...]"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.dont_write_bytecode = True
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
from geo_res import read_res  # noqa: E402
import geo_pst  # noqa: E402
from decode_texture48d import decode_texture  # noqa: E402
G = r'C:\Games\Star Wars Racer Revenge'


def main(paths):
    n = exact = sizes = 0
    mip_err = []
    worst = []
    for path in paths:
        m, _, _ = read_res(path)
        for name, b in m.items():
            if not name.endswith('.pst'):
                continue
            t = geo_pst.parse(b)
            if t['kind'] != 1:
                continue
            lv = geo_pst.rgba_levels(t)
            if min(lv[0].shape[:2]) < 16:
                continue
            n += 1
            blob, info = geo_pst.encode_rgba(lv[-1], max_levels=len(lv))
            dec, _ = decode_texture(blob)
            ref, _ = decode_texture(b)
            big = np.frombuffer(dec[-1]['rgba'], np.uint8).reshape(lv[-1].shape)
            if np.array_equal(big, np.frombuffer(ref[-1]['rgba'], np.uint8).reshape(lv[-1].shape)):
                exact += 1
            else:
                worst.append((name, 'base level differs'))
            if [(d['width'], d['height']) for d in dec] == [(d['width'], d['height']) for d in ref]:
                sizes += 1
            for d, r in zip(dec[:-1], ref[:-1]):
                a = np.frombuffer(d['rgba'], np.uint8).astype(int)
                c = np.frombuffer(r['rgba'], np.uint8).astype(int)
                mip_err.append(float(np.abs(a - c).mean()))
    print('kind-1 textures %d; largest level pixel-exact %d; level sizes equal %d' % (n, exact, sizes))
    if mip_err:
        print('regenerated smaller levels vs original: mean abs diff %.2f, median %.2f, 95th pct %.2f (0..255)'
              % (np.mean(mip_err), np.median(mip_err), np.percentile(mip_err, 95)))
    for w in worst[:5]:
        print('  ', w)


if __name__ == '__main__':
    main(sys.argv[1:] or [os.path.join(G, 'TRACKS', 'TA.RES'), os.path.join(G, 'PODS', 'POD01', 'POD01.RES')])
