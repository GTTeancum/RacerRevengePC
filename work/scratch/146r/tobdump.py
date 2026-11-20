import sys
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import dlc_out
r = dlc_out._archive_members(open(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC\n64-vengeance\TRACKS\NV.RES', 'rb').read())
n = [n for n in r.names() if n.lower().endswith('nv.scb')][0]
t = r.get(n).decode('latin-1')
i = t.lower().find('.cub')
print(repr(t[max(0, i - 200):i + 80]))
