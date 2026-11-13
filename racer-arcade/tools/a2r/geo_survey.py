r"""Survey every PSG in the installed Revenge files with the repo's read-only decoder
(tools\assets\decode_scene48c.py). Run with python -B -I."""
import sys, os, glob, collections, struct
sys.dont_write_bytecode = True
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets')
from geo_res import read_res
from decode_scene48c import decode_psg3_scene
from decode_static48b import FormatError
G = r'C:\Games\Star Wars Racer Revenge'
files = sorted(glob.glob(G + r'\TRACKS\*.RES')) + sorted(glob.glob(G + r'\PODS\*\*.RES'))
reasons = collections.Counter(); ok = 0; tot = 0
for f in files:
    m, order, info = read_res(f)
    for name, data in m.items():
        if not name.endswith('.psg'): continue
        tot += 1
        try:
            decode_psg3_scene(data, True); ok += 1
        except FormatError as e:
            reasons[str(e)] += 1
        except Exception as e:
            reasons['EXC ' + type(e).__name__ + ' ' + str(e)[:60]] += 1
print('psg', tot, 'accepted', ok)
for r, c in reasons.most_common(): print(c, r)
