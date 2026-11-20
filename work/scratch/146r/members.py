import sys, pathlib
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import dlc_out
D = pathlib.Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-arcade-146\DLC')
STOCK = pathlib.Path(r'C:\Games\Star Wars Racer Revenge\TRACKS')
args = sys.argv[1:]
for a in args:
    if len(a) == 2:
        p = next((q for q in D.rglob(a.upper() + '.RES')), None) or STOCK / (a.upper() + '.RES')
    else:
        p = pathlib.Path(a)
    r = dlc_out._archive_members(p.read_bytes())
    names = sorted(r.names())
    print('==', p, len(names))
    for n in names:
        if '\\textures\\' in n.lower() or n.lower().startswith('textures\\'):
            continue
        print('  ', n, len(r.get(n)))
