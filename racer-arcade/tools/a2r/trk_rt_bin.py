import sys, glob, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from trk_binparam import decode, encode
ok = 0; bad = []
for f in sorted(glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../export/a2r/orig/*/tracks__*'))):
    if not f.endswith(('.scb', '.tob', '.aub')):
        continue
    d = open(f, 'rb').read()
    try:
        if encode(decode(d)) == d: ok += 1
        else: bad.append(f)
    except Exception as x:
        bad.append((f, repr(x)))
print('BINARY byte-identical', ok, 'failures', bad)
