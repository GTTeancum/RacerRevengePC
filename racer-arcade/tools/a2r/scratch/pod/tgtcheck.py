import sys
sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\tools\assets'); sys.path.insert(0, r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r')
import decode_scene48c, decode_texture48d, geo_res
for n in ['04', '17']:
    m, o, i = geo_res.read_res(r'C:\Games\Star Wars Racer Revenge\PODS\POD%s\POD%s.RES' % (n, n))
    sc = decode_scene48c.decode_psg3_scene(m[r'pods\pod%s\englt1.psg' % n])
    print(n, [s['normal_alignment'] for L in sc['lods'] for s in L['surfaces']])
    for k in (r'textures\aidamage.psm', r'textures\cable.pst'):
        try:
            (decode_texture48d.parse_material if k.endswith('psm') else decode_texture48d.decode_texture)(m[k]); print(k, 'ok')
        except Exception as e:
            print(n, k, 'target also fails:', e)
