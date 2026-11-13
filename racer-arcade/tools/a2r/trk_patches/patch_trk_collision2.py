"""One-off edit of trk_convert.py: collision orientation from the arcade winding (a2r_space),
generated vertical walls along both edges of the arcade 0x8000 strips, world-space COLs (bit-identical
shared edges between zones), per-zone floor check under every AI node. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()
i = s.index("        # ---- collision split per zone and surface; one collision-only placed model each")
j = s.index("        # ---- visual models and textures from the geometry conversion (geo_course.py)")
new = r'''        # ---- collision split per zone and surface; one collision-only placed model each
        models = {z: [] for z in range(1, Z + 1)}
        col_names = []
        order = a2r_space.collision_order(self.XF)
        TC = tris[:, order]                                    # Revenge collision triangles (world)
        cr = np.cross(TC[:, 1] - TC[:, 0], TC[:, 2] - TC[:, 0])
        ln = np.linalg.norm(cr, axis=1)
        degenerate = ln < 1e-9
        ny = np.where(degenerate, 0.0, cr[:, 1] / np.where(degenerate, 1.0, ln))
        down = int(((ny < 0) & ~degenerate).sum())
        self.report['collision_down_facing_from_winding'] = down
        if down:
            raise ValueError('%d arcade collision triangles face down after the winding rule' % down)
        self.flags.append('collision orientation: from the arcade winding through a2r_space.collision_order '
                          '(every arcade collision triangle is wound with its normal down; all %d non-degenerate '
                          'faces now face up) [A]' % int((~degenerate).sum()))
        strip = surf == 0x8000

        # walls (G, derived from arcade data): every edge between drivable floor and an 0x8000 strip,
        # and every open outer edge of a strip, gets a vertical wall facing the track
        def vkey(v):
            return tuple(np.float32(v).tolist())
        edge_owner = {}
        for t in range(len(tris_a)):
            if degenerate[t]:
                continue
            for k in range(3):
                a_, b_ = vkey(tris_a[t, k]), vkey(tris_a[t, (k + 1) % 3])
                edge_owner.setdefault(frozenset((a_, b_)), []).append((t, k))
        # visual barrier heights: decoded PSG vertices of the geometry conversion, 8-ft grid
        gdir_ = os.path.join(SCR, 'export', 'a2r', 'course%d' % self.course)
        import geo_psg
        vgrid = {}
        for f_ in json.load(open(os.path.join(gdir_, 'manifest.json')))['files']:
            if '_sky_' in f_['file']:
                continue
            mdl = geo_psg.parse(open(os.path.join(gdir_, 'geometry', f_['file'].split(BS)[-1]), 'rb').read())
            for s_ in geo_psg.vertices(mdl):
                for q in s_['pos']:
                    vgrid.setdefault((int(q[0] // 8), int(q[2] // 8)), []).append(q)
        WALL_MIN = 82.0    # Revenge originals: median wall top 82 ft above the nearest floor (wallcheck.py)
        WALL_MAX = 400.0

        def wall_height(m):
            top = 0.0
            cx, cz = int(m[0] // 8), int(m[2] // 8)
            for dx in (-1, 0, 1):
                for dz in (-1, 0, 1):
                    for q in vgrid.get((cx + dx, cz + dz), ()):
                        if (q[0] - m[0]) ** 2 + (q[2] - m[2]) ** 2 < 36.0 and m[1] + 1.0 < q[1] < m[1] + WALL_MAX:
                            top = max(top, q[1] - m[1])
            return max(WALL_MIN, top), top > WALL_MIN
        wall_tris = {z: [] for z in range(1, Z + 1)}
        n_inner = n_outer = n_visual = 0
        for e, own in edge_owner.items():
            kinds = [strip[t] for t, k in own]
            if len(own) == 2 and kinds[0] != kinds[1]:
                inner_t = own[0][0] if not kinds[0] else own[1][0]      # the drivable floor side
                n_inner += 1
            elif len(own) == 1 and strip[own[0][0]]:
                inner_t = own[0][0]                                      # outer open edge of a strip
                n_outer += 1
            else:
                continue
            t, k = own[0]
            a_ = self.X(tris_a[t, k]); b_ = self.X(tris_a[t, (k + 1) % 3])
            mid = (a_ + b_) / 2
            h, from_visual = wall_height(mid)
            n_visual += from_visual
            up = np.array([0.0, h, 0.0])
            q = [np.array([a_, b_, b_ + up]), np.array([a_, b_ + up, a_ + up])]
            inside = TC[inner_t].mean(0)
            nrm = np.cross(q[0][1] - q[0][0], q[0][2] - q[0][0])
            if (nrm[[0, 2]] @ (inside - mid)[[0, 2]]) < 0:
                q = [x[[0, 2, 1]] for x in q]
            wall_tris[int(tri_zone[inner_t])].extend(q)
        self.report['walls'] = dict(floor_strip_edges=n_inner, strip_outer_edges=n_outer, height_from_visual=n_visual)
        self.flags.append('walls: GENERATED vertical faces (derived from arcade data) on %d floor/strip boundary edges and %d '
                          'outer strip edges, facing the track; height = the visual barrier top above the edge where it is '
                          'higher than %.0f ft (%d edges), else %.0f ft (Revenge originals median); the 0x8000 strips stay as '
                          'up-facing floor, as in the arcade [G]' % (n_inner, n_outer, WALL_MIN, n_visual, WALL_MIN))

        def emit(z, base, T3, prov):
            V = T3.reshape(-1, 3).astype(np.float32).astype(np.float64)    # world coordinates, Position 0
            uniq, inv = np.unique(V, axis=0, return_inverse=True)
            faces = inv.reshape(-1, 3)
            keep = (faces[:, 0] != faces[:, 1]) & (faces[:, 1] != faces[:, 2]) & (faces[:, 0] != faces[:, 2])
            faces = faces[keep]
            col = trk_col.make_col(uniq, faces)
            self.put('geometry' + BS + base + '.col', trk_col.write_col(col), prov)
            col_names.append(base)
            models[z].append(dict(slt=base + '.slt', pos=None, faces=len(faces)))
        for z in range(1, Z + 1):
            ids_z = np.flatnonzero((tri_zone == z) & ~degenerate)
            codes = sorted(set(surf[ids_z].tolist()))
            small = [c_ for c_ in codes if np.sum(surf[ids_z] == c_) < 2]
            for c_ in codes:
                ids = ids_z[surf[ids_z] == c_]
                if c_ in small:
                    continue
                extra = [ids_z[surf[ids_z] == s2] for s2 in small] if c_ == max(codes, key=lambda x: np.sum(surf[ids_z] == x)) else []
                ids = np.concatenate([ids] + extra)
                kind = 'strip' if c_ == 0x8000 else 's%02X' % c_
                emit(z, '%s_z%02d_%s_floor_col' % (self.code, z, kind), TC[ids],
                     'A arcade collision (zone %d, surface %X), oriented by the arcade winding; G tree/planes/split' % (z, c_))
            if len(wall_tris[z]) >= 2:
                emit(z, '%s_z%02d_wall_col' % (self.code, z), np.array(wall_tris[z]),
                     'G vertical walls along the arcade 0x8000 edge strips (derived from arcade data)')
        self.report['collision_files'] = len(col_names)

        # every AI node needs a floor face below it in its own zone's COL files
        missing = []
        for j_, k_ in enumerate(ai_idx):
            z = int(node_zone[j_])
            ids = np.flatnonzero((tri_zone == z) & ~degenerate)
            h = heights_under(TC[ids], PA[j_][0], PA[j_][2])
            if not len(h):
                missing.append(j_)
        self.report['ai_nodes_without_floor_in_own_zone'] = missing
        if missing:
            self.flags.append('AI nodes without a floor face in their own zone: %s' % missing)

'''
s = s[:i] + new + s[j:]
open(p, 'w').write(s)
print('ok')
