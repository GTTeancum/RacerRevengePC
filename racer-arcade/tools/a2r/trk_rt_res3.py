"""Round trip of the 13 original track RES3 archives: read -> rebuild from the members alone
(original layout rule) -> encode -> compare with the file byte for byte."""
import sys, os, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_res3 import read_res3, encode_res3, build_res3
ROOT = r'C:/Games/Star Wars Racer Revenge/TRACKS/'
TRACKS = sys.argv[1:] or 'TA TB TC RA RC MA MB MC GA GB GC SA SC'.split()
allok = True
for t in TRACKS:
    t0 = time.time()
    d = open(ROOT + t + '.RES', 'rb').read()
    r = read_res3(d)
    rb = build_res3(list(r.items()))
    layout = rb.payload == r.payload and rb.members == r.members
    enc = encode_res3(rb)
    same = enc == d
    allok &= same and layout
    print(t, len(r.members), 'members', len(r.zblocks), 'blocks', 'layout-rule', layout,
          'byte-identical', same, '%.0fs' % (time.time() - t0), flush=True)
print('RES3 ALL OK' if allok else 'RES3 FAIL')
