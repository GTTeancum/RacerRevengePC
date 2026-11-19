# Stock TA.RES / tracks\ta.tob identities (receipt values only). python -I -B ta_receipt.py
import hashlib, os, sys
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import trk_res3
p = r'C:\Games\Star Wars Racer Revenge\TRACKS\TA.RES'
b = open(p, 'rb').read()
print('archive', len(b), hashlib.sha256(b).hexdigest())
m = {n.lower(): d for n, d in trk_res3.read_res3(b).items()}
t = m['tracks\\ta.tob']
print('tob', len(t), hashlib.sha256(t).hexdigest())
