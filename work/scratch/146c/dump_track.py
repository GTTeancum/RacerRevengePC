import sys;sys.path.insert(0,r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import dlc_out
r=dlc_out._archive_members(open(r'C:\Games\Star Wars Racer Revenge\UI\UICOURSE\UICOURSE.RES','rb').read())
for n in r.names():
  if n.lower().endswith(sys.argv[1]): print('=====',n);print(r.get(n).decode('latin-1'))
