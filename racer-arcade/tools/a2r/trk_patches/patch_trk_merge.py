"""One-off edit of trk_convert.py: merge the geometry helper's PSG/PST/PSM output into the scene
and the RES, sky dome as a placed model, install layout. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""        self.report['collision_files'] = len(col_names)
""", """        self.report['collision_files'] = len(col_names)

        # ---- visual models and textures from the geometry conversion (geo_course.py)
        gdir = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course)
        gman = json.load(open(os.path.join(gdir, 'manifest.json')))
        if gman['code'] != self.code or gman['transform'] != self.XF:
            raise ValueError('geometry output was made with another code/transform: rerun geo_course.py %d' % self.course)
        nvis = 0
        vis = {}
        for f in gman['files']:
            name = f['file'].split(BS)[-1][:-4]
            data = open(os.path.join(gdir, 'geometry', name + '.psg'), 'rb').read()
            self.put('geometry' + BS + name + '.psg', data, 'geometry helper (geo_course.py): arcade landscape, A + G lighting')
            c = (np.array(f['bounds_min']) + np.array(f['bounds_max'])) / 2
            if '_sky_' in name:
                z = 1   # sky dome: one placed model; no ExclusionList, so every zone draws zone 1
            else:
                k = int(np.argmin(np.linalg.norm((C - c) * np.array([1.0, 3.0, 1.0]), axis=1)))
                z = int(zone_of_sec[sec_of[k]])
            # world pieces are placed at 0,0,0 like TA Zone01GroundStrips / GC gc_z1_track_a
            vis.setdefault(z, []).append(dict(slt=name + '.slt', pos=None, visual=True))
            nvis += 1
        for t in gman['textures']:
            for ext in ('pst', 'psm'):
                nm = 'textures' + BS + t['material'] + '.' + ext
                self.put(nm, open(os.path.join(gdir, 'textures', t['material'] + '.' + ext), 'rb').read(),
                         'geometry helper (geo_course.py): arcade texture %s' % t.get('arcade_texture'))
        for z in models:   # visual pieces first in each zone, then its collision models
            models[z] = vis.get(z, []) + models[z]
        self.report['visual_models'] = len(gman['files'])
        self.report['textures'] = len(gman['textures'])
        self.flags.append('visual models: %d PSG world pieces placed at 0,0,0 in the zone of the nearest centre-line point (sky dome in zone 1) [geometry A, zone choice G]' % len(gman['files']))
""")
rep("""                scn.append(('Zone%d-Model%d' % (z, m_i), [('SLT', m['slt']), ('Position', vec(m['pos'])),""",
    """                scn.append(('Zone%d-Model%d' % (z, m_i), [('SLT', m['slt']), ('Position', '0.0,0.0,0.0' if m['pos'] is None else vec(m['pos'])),""")
rep("""            ('Environment', [('CubeFile', '%s.cub' % self.code), ('TerrainWidth', '5'), ('TerrainBreadth', '5'),""",
    """            ('Environment', [('TerrainWidth', '5'), ('TerrainBreadth', '5'),""")
rep("""                             ('ParticleTextureMapName', 'demoprt.tga')]),""",
    """                             ('ParticleTextureMapName', 'demoprt.tga'), ('NumberOfSkyCubes', '0')]),""")
rep("""            'Environment: G (CubeFile %s.cub expected from the geometry helper\\'s sky; no water, no ecosystem; terrain keys copied from GC) [G/D]' % self.code,""",
    """            'Environment: NumberOfSkyCubes=0 as SC (the arcade sky dome is a placed model instead of a cube); no water, no ecosystem; terrain keys copied from GC [G/D]',""")
rep("""            src = os.path.join(SCR, 'export', 'course%d' % self.course, 'preview', 'top_textured.png')""",
    """            src = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course, 'preview', 'top_textured.png')""")
rep("""(export\\\\course%d\\\\preview\\\\top_textured.png) on 640x448 like LD_TA' % self.course)""",
    """of the converted geometry (export\\\\a2r\\\\course%d\\\\preview\\\\top_textured.png) on 640x448 like LD_TA' % self.course)""")

# output layout
i = s.index("    def write(self):")
j = s.index("def main():")
s = s[:i] + '''    def write(self):
        """export\\a2r\\<code>\\: install\\ = files laid out as in the game folder (TRACKS\\<CODE>.RES,
        UI\\TEXTURES\\LD_<CODE>.PST), members\\ = the RES members loose, source\\ = .scn/.toy text,
        menu\\ = LOC strings, manifest.json. The folder is rebuilt from scratch."""
        import shutil
        keep_verify = os.path.join(self.out, 'verify')
        for sub in ('install', 'members', 'source', 'menu', 'outside_res', 'tracks', 'geometry', 'textures',
                    'particle', 'audio'):
            shutil.rmtree(os.path.join(self.out, sub), ignore_errors=True)
        for f_ in os.listdir(self.out) if os.path.isdir(self.out) else []:
            if f_.endswith('.RES'):
                os.remove(os.path.join(self.out, f_))
        os.makedirs(self.out, exist_ok=True)
        src = {nm: d for nm, d in self.files.items() if nm.endswith(('.scn', '.toy'))}
        names = sorted([nm for nm in self.files if nm not in src],
                       key=lambda s_: (0 if s_.endswith('.tob') else 1 if s_.endswith('.aub') else
                                       2 if s_.endswith('.scb') else 3, s_.lower()))
        for name in names:
            pth = os.path.join(self.out, 'members', *name.split(BS))
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(self.files[name])
        for name, d in src.items():
            pth = os.path.join(self.out, 'source', *name.split(BS))
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(d)
        res_path = os.path.join(self.out, 'install', 'TRACKS', '%s.RES' % self.code.upper())
        os.makedirs(os.path.dirname(res_path), exist_ok=True)
        open(res_path, 'wb').write(trk_res3.write_res3([(nm, self.files[nm]) for nm in names]))
        for name, (data, prov) in self.extra.items():
            base = 'menu' if name.startswith('menu') else 'install'
            rel = name.split(BS)[1:] if base == 'menu' else name.split(BS)
            pth = os.path.join(self.out, base, *rel)
            os.makedirs(os.path.dirname(pth), exist_ok=True)
            open(pth, 'wb').write(data)
        man = dict(course=self.course, name=self.cfg['name'], code=self.code, provisional_code=self.cfg['provisional_code'],
                   physics_class=self.cls, transform=self.XF, report=self.report, flags=self.flags,
                   res='install\\\\TRACKS\\\\%s.RES' % self.code.upper(),
                   files={nm: dict(bytes=len(self.files[nm]), provenance=self.prov[nm]) for nm in names},
                   sources={nm: len(d) for nm, d in src.items()},
                   outside_res={n_: dict(bytes=len(d_), provenance=p_) for n_, (d_, p_) in self.extra.items()},
                   pending=[dict(item=a, reason=b) for a, b in self.pending])
        open(os.path.join(self.out, 'manifest.json'), 'w').write(json.dumps(man, indent=1, default=float))


''' + s[j:]
open(p, 'w').write(s)
print('ok')
