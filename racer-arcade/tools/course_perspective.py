# Perspective (aerial 3/4 + low) views of a course, textured with sky. Usage: python -I tools/course_perspective.py N
import os, sys, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import course_preview, export_course
import softrender as SR
cap = {}
course_preview.previews = lambda out, scene, texcache, V, R, s, l, path=None: cap.update(out=out, scene=scene, tex=texcache, path=path)
N = int(sys.argv[1]); export_course.export(N)
sc = cap['scene']; P = np.array(sc.P); UV = np.array(sc.UV)
T, tt, tx = course_preview._scene_arrays(sc, cap['tex']); sT, st, sx = course_preview._scene_arrays(sc, cap['tex'], sky=True)
used = np.unique(T); lo, hi = P[used].min(0), P[used].max(0); c = (lo + hi) / 2; r = np.linalg.norm(hi - lo) / 2
path = cap['path']; pc = np.nanmean(path, 0)
views = []
for k, (az, el, dist, fov) in enumerate([(0.7, 0.55, 1.15, 60), (2.6, 0.35, 0.9, 60), (4.3, 0.18, 0.6, 70)]):
    eye = pc + dist * r * np.array([np.cos(az) * np.cos(el), np.sin(el), np.sin(az) * np.cos(el)])
    views.append(dict(eye=eye, target=pc, up=(0, 1, 0), fov=fov, near=1.0))
# one low view from the track itself
i = 0; j = 6
d = path[j] - path[i]; d[1] = 0; d /= np.linalg.norm(d)
views.append(dict(eye=path[i] - d * 40 + [0, 14, 0], target=path[i] + d * 150 + [0, 4, 0], up=(0, 1, 0), fov=70, near=1.0))
pdir = os.path.join(cap['out'], 'preview')
for k, cam in enumerate(views):
    zb = None; img = None
    if len(sT):
        _, zb, img = SR.render(P, sT, cam, 1280, 720, uv=UV, tex_of_tri=st, textures=sx, ambient=1.0); zb[:] = np.inf
    im, _, _ = SR.render(P, T, cam, 1280, 720, uv=UV, tex_of_tri=tt, textures=tx, ambient=0.55, img=img, zbuf=zb)
    SR.save(im, os.path.join(pdir, 'perspective_%d.png' % k))
print('sky tris', len(sT), 'land tris', len(T), 'radius', r)
