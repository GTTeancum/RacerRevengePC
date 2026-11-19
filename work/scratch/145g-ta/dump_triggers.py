# Dump every stock course's [Triggers] blocks (read-only; stock install).
# Usage: python -I -B dump_triggers.py [CODE ...]   (no codes = all 13, IgnoreTrigger summary)
import os, re, sys, collections
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import trk_res3, trk_binparam
T = r'C:\Games\Star Wars Racer Revenge\TRACKS'
codes = sys.argv[1:]
summary = not codes
codes = codes or 'TA TB TC SA SC MA MB MC GA GB GC RA RC'.split()
ignore = collections.Counter(); kinds = collections.Counter()
for code in codes:
    r = trk_res3.read_res3(open(os.path.join(T, code + '.RES'), 'rb').read())
    m = {n.lower(): d for n, d in r.items()}
    tob = m.get('tracks\\%s.tob' % code.lower())
    if tob is None:
        print(code, 'no tob'); continue
    blocks = trk_binparam.decode(tob)
    bd = {n: dict(kv) for n, kv in blocks}
    print(code, 'NumberOfTriggers', bd.get('triggers', {}).get('numberoftriggers'))
    for name, kv in blocks:
        if re.fullmatch(r'trigger\d+(action\d+)?', name):
            d = dict(kv)
            if 'ignoretrigger' in d: ignore[(code, d['ignoretrigger'])] += 1
            if 'actiontype' in d: kinds[(code, d['actiontype'])] += 1
            if not summary: print(' ', name, d)
if summary:
    for k, v in sorted(ignore.items()): print('ignore', k, v)
    for k, v in sorted(kinds.items()): print('kind', k, v)
