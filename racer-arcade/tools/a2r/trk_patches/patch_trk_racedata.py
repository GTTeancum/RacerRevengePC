"""One-off edit of trk_convert.py: integrate race-data.md sections 13 and 15 (AI racing-line
curves, fog colour/distances, sun direction). Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


# parsers
rep("""def arcade_collision(cid):""", '''def parse_racing_lines():
    """race-tables.txt 'NPC per-course (node, value) tables' = racing-line curves (segment, lateral
    0..1; 0 = left guide list, 1 = right), course order 0..3 (race-data.md 13)."""
    txt = open(os.path.join(SCR, 'notes', 'race-tables.txt')).read()
    out = {}
    for i, nm in enumerate(['SA_COURSE2', 'ETTI4', 'MALASTARE', 'SA_COURSE4']):
        m = re.search('\\n' + nm + r'\\?: ((?:\\([\\d.]+,[\\d.]+\\) ?)+)', txt)
        if m:
            out[i] = [tuple(float(x) for x in pr.split(',')) for pr in re.findall(r'\\(([\\d.]+,[\\d.]+)\\)', m.group(1))]
    return out


def parse_hooks():
    """race-data.md 15 table: per course fog RGB, fog near, fog far, light vector (or None)."""
    txt = open(os.path.join(SCR, 'notes', 'race-data.md')).read()
    out = {}
    for m in re.finditer(r'\\| (\\d) \\w+ \\| (\\d+),(\\d+),(\\d+)[^|]*\\| (\\d+) \\| (\\d+) \\| ([^|]*)\\|', txt):
        v = re.search(r'@[0-9A-F]+ \\(([-\\d.]+),([-\\d.]+),([-\\d.]+)\\)', m.group(7))
        out[int(m.group(1))] = dict(fog_rgb=tuple(int(m.group(k)) for k in (2, 3, 4)), fog_near=float(m.group(5)),
                                    fog_far=float(m.group(6)), light=tuple(float(x) for x in v.groups()) if v and 'position' not in m.group(7) else None,
                                    light_text=m.group(7).strip())
    return out


def arcade_collision(cid):''')

# racing line for the AI spline
rep("""        # ---- distances, speed limits, progress nodes
        nxt = np.roll(np.arange(K), -1)""", """        # ---- AI racing line (race-data.md 13): lateral 0..1 between the guide edge lists per segment
        curves = parse_racing_lines()
        PA = P.copy()
        if guide is not None and self.course in curves:
            bp = np.array(curves[self.course])
            seg_idx = np.arange(n, dtype=float)
            lateral = np.interp(seg_idx, bp[:, 0], bp[:, 1])   # breakpoints past n never reached (13)
            for j, k in enumerate(ai_idx):
                q = gl[k] + (gr[k] - gl[k]) * lateral[k]
                ids = np.concatenate([sect_tris[(sec_of[k] + d_) % nsec] for d_ in (-1, 0, 1)])
                ids = ids[floor[ids]]
                h = heights_under(tris[ids], *q)
                y = h[np.argmin(np.abs(h - C[k][1]))] if len(h) else C[k][1]
                PA[j] = (q[0], y, q[1])
                a = np.dot(np.r_[gl[k][0] - q[0], 0, gl[k][1] - q[1]], R[k])
                b = np.dot(np.r_[gr[k][0] - q[0], 0, gr[k][1] - q[1]], R[k])
                ai_right[j] = max(a, b, 1.0); ai_left[j] = max(-min(a, b), 1.0)
            self.flags.append('AI spline positions: A-derived arcade racing line (curve @0C2F9260+ per course, lateral between the AI edge lists, race-data.md 13); widths to those edges [A]')
        else:
            self.flags.append('AI spline positions: centre line (no arcade racing-line curve for this course) [A centre, G]')

        # ---- distances, speed limits, progress nodes
        nxt = np.roll(np.arange(K), -1)""")
rep("""        def spline(kind):
            tw = AI_TRACKWIDTH if kind == 'a' else MAP_TRACKWIDTH""", """        def dist_to_finish(Q):
            sg = np.linalg.norm(Q[nxt] - Q, axis=1)
            out = np.zeros(K); acc_ = 0.0
            for j in range(K - 1, 0, -1):
                acc_ += sg[j]; out[j] = acc_
            return out

        def spline(kind):
            Q = PA if kind == 'a' else P
            dq = dist_to_finish(Q)
            tw = AI_TRACKWIDTH if kind == 'a' else MAP_TRACKWIDTH""")
rep("""                kv = [('Position', vec(P[j])), ('LeftTrackWidth', '%f' % f32(lw[j])),
                      ('RightTrackWidth', '%f' % f32(rw[j])), ('SpeedLimit', '%f' % speed[j]),
                      ('DistanceToFinish', '%f' % f32(dtf[j])), ('BranchID', '0')]""",
    """                kv = [('Position', vec(Q[j])), ('LeftTrackWidth', '%f' % f32(lw[j])),
                      ('RightTrackWidth', '%f' % f32(rw[j])), ('SpeedLimit', '%f' % speed[j]),
                      ('DistanceToFinish', '%f' % f32(dq[j])), ('BranchID', '0')]""")
rep("""spline('a'), 'A centre line + AI edge lines; G node choice, speed, distances')""",
    """spline('a'), 'A racing line + AI edge lines; G node choice, speed; distances A-derived')""")

# fog and sun
rep("""        fogf = cj['landscape_record']['floats_28_32']""", """        fogf = cj['landscape_record']['floats_28_32']
        hook = parse_hooks().get(self.course)""")
rep("""            ('Light2', [('Type', 'Directional'), ('ColorRGB', '170,170,160'),
                        ('Position', '%.3f,%.3f,%.3f' % (ctr[0] + 3000, cmax[1] + 6000, ctr[2] + 3000)),""",
    """            ('Light2', [('Type', 'Directional'), ('ColorRGB', '170,170,160'),
                        ('Position', '%.3f,%.3f,%.3f' % tuple(sun_pos)),""")
rep("""        sun = ctr + np.array([0.0, 0.0, 0.0])
""", """        sun_dir = np.array([0.6, 1.0, 0.6])
        if hook and hook['light']:
            sun_dir = a2r_space.normals(np.array(hook['light']), self.XF)   # direction toward the light (guess)
        sun_dir = sun_dir / np.linalg.norm(sun_dir)
        sun_pos = np.array([ctr[0], cmin[1], ctr[2]]) + sun_dir * 8000.0
        fog_rgb = hook['fog_rgb'] if hook else (151, 160, 177)
""")
rep("""            ('Fog', [('ColorRGB', '151,160,177'), ('Visibility', '0.5')]),""",
    """            ('Fog', [('ColorRGB', '%d,%d,%d' % tuple(fog_rgb)), ('Visibility', '0.5')]),""")
rep("""            'Fog: ColorRGB/Visibility copied from GC [D]; arcade landscape +28/+32 = %s (meaning unproved, not used)' % fogf,""",
    """            ('Fog: ColorRGB %s = arcade fog colour (hook +40, race-data.md 15) [A]; arcade fog near/far %s units = %s ft; '
             'Visibility 0.5 from GC [D] (mapping from distances to Visibility unknown)' % (
                 fog_rgb, (hook['fog_near'], hook['fog_far']) if hook else '-',
                 (hook['fog_near'] * self.XF['scale'], hook['fog_far'] * self.XF['scale']) if hook else '-')) if hook else
            'Fog: GC values [D] (no arcade fog entry)',
            ('Light2 sun direction from arcade light vector %s (race-data.md 15) [A], mirrored with the course; colour/flags GC [D]' % (hook['light'],))
            if hook and hook['light'] else 'Light2 sun direction G (arcade light data: %s, tag list not decoded)' % (hook['light_text'] if hook else '-'),""")
open(p, 'w').write(s)
print('ok')
