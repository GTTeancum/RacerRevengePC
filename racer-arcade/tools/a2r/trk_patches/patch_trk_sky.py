"""One-off edit of trk_convert.py: real Revenge sky cube (skies\\<code>.cub) from the arcade dome,
dome removed from the scene, fog colour from the sky horizon. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""        for f in gman['files']:
            name = f['file'].split(BS)[-1][:-4]
            data = open(os.path.join(gdir, 'geometry', name + '.psg'), 'rb').read()""",
    """        for f in gman['files']:
            name = f['file'].split(BS)[-1][:-4]
            if '_sky_' in name:
                continue   # the arcade dome becomes the sky cube below, not a scene model
            data = open(os.path.join(gdir, 'geometry', name + '.psg'), 'rb').read()""")
rep("""        for t in gman['textures']:
            for ext in ('pst', 'psm'):""", """        used_mats = set(f['material'] for f in gman['files'] if '_sky_' not in f['file'])
        for t in gman['textures']:
            if t['material'] not in used_mats:
                continue   # dome-only texture: now inside the sky cube
            for ext in ('pst', 'psm'):""")
rep("""        self.report['visual_models'] = len(gman['files'])""", """        self.report['visual_models'] = sum(1 for f in gman['files'] if '_sky_' not in f['file'])

        # ---- sky: the arcade dome rendered into Revenge's sky cube (trk_sky.py)
        import trk_sky
        import geo_course as GC_
        atris, _ = GC_.arcade_triangles(self.course)
        sky = atris['sky']
        SP = np.array([t['P'] for t in sky]).reshape(-1, 3)
        lo_, hi_ = SP.min(0), SP.max(0)
        eye = np.array([(lo_[0] + hi_[0]) / 2, 0.0, (lo_[2] + hi_[2]) / 2])   # dome centre, arcade ground level
        texs = {}
        for t in sky:
            if t['tid'] is not None and t['tid'] not in texs:
                texs[t['tid']] = GC_.EC.texture_rgba(t['tid'])[0]
        tiles, misses = trk_sky.render_tiles(sky, self.XF, eye, texs)
        rta_ = trk_res3.read_res3(open(REVENGE + 'TA.RES', 'rb').read())
        palette = rta_.get('skies' + BS + 'ta.cub')[16:99100]
        cub = trk_sky.build_cub(tiles, palette)
        self.put('skies' + BS + '%s.cub' % self.code, cub,
                 'A arcade sky dome (%d triangles, texture %s) ray-cast from its centre into the Revenge sky cube; '
                 'G cube layout, IPU encoding; palette block copied from ta.cub [D]' % (len(sky), sorted(texs)))
        # horizon colour = bottom texel rows of the lowest authored side tiles (rows 4..7 -> tile row 1)
        hz = np.concatenate([tiles[(f_, t_)][-3:].reshape(-1, 3) for f_ in range(4) for t_ in (4, 5, 6, 7)])
        self.sky_horizon = tuple(int(round(x)) for x in hz.mean(0))
        self.report['sky'] = dict(member='skies' + BS + '%s.cub' % self.code, bytes=len(cub), tiles=len(tiles),
                                  dome_triangles=len(sky), eye_arcade=eye.tolist(), rays_missing_dome=int(sum(misses.values())),
                                  horizon_rgb=self.sky_horizon)
        self.flags.append('sky: arcade dome -> skies\\\\%s.cub (48 tiles x 4 IPU mips, the originals\\' layout); the dome is no longer a '
                          'scene model; %d of %d rays below the dome rim take the colour of the lowest dome texel in their '
                          'column [A, G]' % (self.code, sum(misses.values()), 48 * 128 * 128))""")
rep("""        fog_rgb = hook['fog_rgb'] if hook else (151, 160, 177)""",
    """        fog_rgb = self.sky_horizon     # clear colour under the sky = fog colour: matched to the sky horizon""")
rep("""                             ('ParticleTextureMapName', 'demoprt.tga'), ('NumberOfSkyCubes', '0')]),""",
    """                             ('ParticleTextureMapName', 'demoprt.tga'), ('CubeFile', '%s.cub' % self.code)]),""")
rep("""            'Environment: NumberOfSkyCubes=0 as SC (the arcade sky dome is a placed model instead of a cube); no water file""",
    """            'Environment: CubeFile %s.cub (single sky cube, as TA/TB/MA/GA/GB/GC) [A-derived]; no water file' % self.code + '""")
open(p, 'w').write(s)
print('ok')
