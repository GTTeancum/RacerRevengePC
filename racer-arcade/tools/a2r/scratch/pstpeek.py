import sys,struct;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
from geo_res import read_res
m,o,i=read_res(r'C:\Games\Star Wars Racer Revenge\TRACKS\TA.RES')
b=m[sys.argv[1]];print(len(b),struct.unpack_from('<HHII',b,0))
at=12
print(b[at:at+64].hex())
