import sys, glob; sys.path.insert(0, r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/tools/a2r')
import trk_res3, geo_psg
BS = chr(92)
def info(blob):
    m = geo_psg.parse(blob); ss = geo_psg.vertices(m)
    cross = sum(1 for s in ss for a, b, c in s['tris'] if len({s['bone'][a], s['bone'][b], s['bone'][c]}) > 1)
    return dict(nodes=len(m['nodes']), surfaces=len(ss), tris=sum(len(s['tris']) for s in ss), tris_spanning_bones=cross)
want = {('ui' + BS + 'anim' + BS + '%sc.psg' % n) for n in ('17', '04')}
r = trk_res3.read_res3(open('C:/Games/Star Wars Racer Revenge/UI/UIRIDER/UIRIDER.RES', 'rb').read())
for k, d in r.items():
    if k.lower() in want: print('original ', k, info(d))
for p in glob.glob(r'D:/Programming/GitHub/RacerRevengePC/work/scratch/racer-arcade/export/a2r/ui/**/UIRIDER.RES', recursive=True):
    for k, d in trk_res3.read_res3(open(p, 'rb').read()).items():
        if k.lower() in want: print('converted', k, info(d))
