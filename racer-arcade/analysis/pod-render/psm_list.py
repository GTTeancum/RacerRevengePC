import os, sys, glob
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(HERE, '..', '..', 'tools', 'a2r'))
import geo_res
from collections import defaultdict
seen = defaultdict(list)
for p in sorted(glob.glob(r'C:\Games\Star Wars Racer Revenge\PODS\POD*\POD*.RES')):
    m, order, info = geo_res.read_res(p)
    pod = os.path.basename(p)[:5]
    for name, blob in m.items():
        if not name.endswith('.psm'): continue
        t = blob.decode('latin1')
        kv = {}
        for l in t.splitlines():
            if '=' in l:
                k, v = [s.strip() for s in l.split('=', 1)]
                if v and k in ('microProgramName', 'TextureMap_1', 'TextureMap_2', 'alphaBlendingOnOff', 'zWriteDisabled', 'TextureAnimation',
                               'animatedMapFramesPerSecond', 'fixedAlphaValue', 'inputColorB', 'inputAlphaC', 'TotalShaderPasses', 'depthTestMethod', 'textureFunction', 'wrapModeVertical'):
                    kv.setdefault(k, v)
        sig = tuple(sorted(kv.items()))
        seen[(os.path.basename(name).replace(pod[3:5], 'nn'), sig)].append(pod[3:5])
for (n, sig), pods in sorted(seen.items()):
    print('%-22s %-40s %s' % (n, ','.join(pods) if len(pods) < 23 else 'all', dict(sig)))
