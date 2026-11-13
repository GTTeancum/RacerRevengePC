"""One-off edit of trk_convert.py: race-data.md 18-24 (lights incl. Malastare's 32 flickering point
lights, sun lens flare, hook +44 static objects, water/sound/scroll notes). Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""def arcade_collision(cid):""", '''def parse_lights():
    """race-data.md 20: per course directional lights [(vector, colour)] and point lights
    [(x, y, z)] (Malastare), plus the point light colour."""
    txt = open(os.path.join(SCR, 'notes', 'race-data.md')).read()
    sec = txt[txt.index('## 20.'):txt.index('## 21.')]
    dirs = {}

    def triple(t):
        v = [float(x) for x in re.findall(r'[-\\d.]+', t)]
        return tuple(v) if len(v) == 3 else (v[0],) * 3
    for m in re.finditer(r'^\\| (\\d) \\| ([^|]+)\\| (dir|point) \\| ([^|]+)\\| ([^|]+)\\|', sec, re.M):
        c, kind, vec_t, col_t = int(m.group(1)), m.group(3), m.group(4), m.group(5)
        if kind != 'dir':
            continue
        vecs = [triple(x) for x in vec_t.split(' / ')]
        if 'as course' in col_t:
            cols = [d[1] for d in dirs[int(re.search(r'course (\\d)', col_t).group(1))]]
        else:
            cols = [triple(x) for x in col_t.split(' / ')]
        dirs.setdefault(c, []).extend(zip(vecs, cols))
    pts = [tuple(float(x) for x in mm.groups()) for mm in
           re.finditer(r'\\d+ \\(([-\\d.]+),\\s*([-\\d.]+),\\s*([-\\d.]+)\\)', sec[sec.index('Malastare point lights'):sec.index('Per frame')])]
    return dirs, {2: pts}


def arcade_collision(cid):''')

# hook +44 objects after the geometry merge
rep("""        for z in models:   # visual pieces first in each zone, then its collision models
            models[z] = vis.get(z, []) + models[z]""", """        import trk_hookobjects
        hk = trk_hookobjects.convert(self.course, self.XF, self.code, set(t['material'] for t in gman['textures']))
        for name, blob, info in hk['psg']:
            self.put('geometry' + BS + name + '.psg', blob,
                     'A arcade object placed by the course hook +44 (notes\\\\race-hooks.txt), converted with the geometry helper pipeline')
            c = (np.array(info['bounds_min']) + np.array(info['bounds_max'])) / 2
            k = int(np.argmin(np.linalg.norm((C - c) * np.array([1.0, 3.0, 1.0]), axis=1)))
            vis.setdefault(int(zone_of_sec[sec_of[k]]), []).append(dict(slt=name + '.slt', pos=None, visual=True))
        for mname, pst, psm in hk['textures']:
            self.put('textures' + BS + mname + '.pst', pst, 'A arcade texture of a hook +44 object (geo_pst encoder)')
            self.put('textures' + BS + mname + '.psm', psm, 'G material for a hook +44 object (geo_psm)')
        if hk['placements']:
            self.report['hook_objects'] = hk['placements']
            self.flags.append('hook +44 static objects: %d placements -> %d PSG, %d new textures; placement = object x scale + (x, y, z), '
                              'the 4th value is a scale (f_0C146F3A multiplies the bounding radius by it at 0C146FCC) [A, A-derived]' % (
                                  len(hk['placements']), len(hk['psg']), len(hk['textures'])))
        for z in models:   # visual pieces first in each zone, then its collision models
            models[z] = vis.get(z, []) + models[z]""")

# lights
rep("""        sun_dir = np.array([0.6, 1.0, 0.6])
        if hook and hook['light']:
            sun_dir = a2r_space.normals(np.array(hook['light']), self.XF)   # direction toward the light (guess)
        sun_dir = sun_dir / np.linalg.norm(sun_dir)
        sun_pos = np.array([ctr[0], cmin[1], ctr[2]]) + sun_dir * 8000.0""", """        ldirs, lpoints = parse_lights()
        dl = ldirs.get(self.course, [])
        sun_dir = np.array([0.6, 1.0, 0.6]); sun_col = (170, 170, 160)
        if dl:
            sun_dir = a2r_space.normals(np.array(dl[0][0]), self.XF)   # vector toward the light (sun above: +y)
            sun_col = tuple(int(round(min(1.0, c) * 255)) for c in dl[0][1])
        sun_dir = sun_dir / np.linalg.norm(sun_dir)
        sun_pos = np.array([ctr[0], cmin[1], ctr[2]]) + sun_dir * 8000.0
        flare = self.course != 2     # arcade sun lens flare (flare objects 1219-1225) on courses 0, 1, 3, 4 (race-data.md 18)
        point_lights = []
        for i_, q in enumerate(lpoints.get(self.course, [])):
            wq = self.X(np.array(q))
            j_ = int(np.argmin(np.linalg.norm(C - wq, axis=1)))
            point_lights.append(('Light%d' % (3 + i_), [
                ('Type', 'ObjectPoint'), ('ColorRGB', '242,242,191'), ('Position', vec(wq)),
                ('InitialZone', str(int(zone_of_sec[sec_of[j_]]))), ('EmitsLight', '1'),
                ('Attenuation', '1.000000,0.000000,0.000400'), ('HasLightFlare', '0'),
                ('HasFlasher', 'T'), ('FlashPattern', ','.join(['1', '0'] * 16)), ('FlashPeriod', '0.533333'),
                ('FlashFilter', '0.800000')]))""")
rep("""            ('Lights', [('NumberOfLights', '2')]),""", """            ('Lights', [('NumberOfLights', str(2 + len(point_lights)))]),""")
rep("""            ('Light2', [('Type', 'Directional'), ('ColorRGB', '170,170,160'),""",
    """            ('Light2', [('Type', 'Directional'), ('ColorRGB', '%d,%d,%d' % sun_col),""")
rep("""                        ('HasLensFlare', 'T'), ('LensFlareColorRGB', '255,255,240'), ('LensFlareBrightness', '1.0'),
                        ('LensFlareLayersVisible', 'T,T,T,T,T')]),""", """                        ('HasLensFlare', 'T' if flare else 'F'), ('LensFlareColorRGB', '255,255,240'), ('LensFlareBrightness', '1.0'),
                        ('LensFlareLayersVisible', 'T,T,T,T,T')])] + point_lights + [""")
rep("""            ('Light2 sun direction from arcade light vector %s (race-data.md 15) [A], mirrored with the course; colour/flags GC [D]' % (hook['light'],))
            if hook and hook['light'] else 'Light2 sun direction G (arcade light data: %s, tag list not decoded)' % (hook['light_text'] if hook else '-'),""",
    """('Light2 = arcade light 0: vector %s, colour %s (race-data.md 20) [A], mirrored with the course; flags GC [D]; '
             'HasLensFlare %s (arcade sun lens flare on courses 0, 1, 3, 4, race-data.md 18) [A]' % (dl[0][0], dl[0][1], 'T' if flare else 'F'))
            if dl else 'Light2 sun G (no arcade light entry)',
            ('arcade lights not written: %s (Revenge tracks carry one directional light each; a second one is unproven in the port) [pending]' %
             ['%s %s' % (v, c_) for v, c_ in dl[1:]]) if len(dl) > 1 else 'no further arcade directional lights',
            ('%d arcade point lights (race-data.md 20, @0C3E8120) -> ObjectPoint Light3..; colour 0.95,0.95,0.75 [A]; '
             'range 20..50 units (60..150 ft) -> Attenuation 1,0,0.0004 (10 %% at 150 ft) [G]; per-frame flicker between ranges '
             '(20,50)/(15,45) -> HasFlasher with an on/off pattern, FlashFilter 0.8 [G]' % len(point_lights)) if point_lights else 'no arcade point lights',""")
rep("""            'Environment: NumberOfSkyCubes=0 as SC (the arcade sky dome is a placed model instead of a cube); no water, no ecosystem; terrain keys copied from GC [G/D]',""",
    """            'Environment: NumberOfSkyCubes=0 as SC (the arcade sky dome is a placed model instead of a cube); no water file (the arcade has no water physics, race-data.md 21), no ecosystem; terrain keys copied from GC [G/D]',
            'collision sectors (track slices) read from the course: %d [A]' % nsec,""")
open(p, 'w').write(s)
print('ok')
