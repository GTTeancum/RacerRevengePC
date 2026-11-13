"""One-off edit of trk_convert.py: racing line computed and kept clear of the walls at every arcade
centre-line point before node selection; node chords checked against it. Kept for the record."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'trk_convert.py')
s = open(p).read()


def rep(a, b):
    global s
    assert s.count(a) == 1, a[:80]
    s = s.replace(a, b)


rep("""        # ---- node selection (shared by AI and map splines)
        tol = NODE_TOLERANCE""", """        # ---- AI racing line at every arcade centre-line point (race-data.md 13), kept AI_WALL_CLEAR_FT
        # inside the drivable floor (the generated walls stand on its edges)
        guide0 = guide_all.get(self.course)
        curves0 = parse_racing_lines()
        RL = cxz.copy()
        rl_shifted = 0
        if guide0 is not None and self.course in curves0:
            gl0 = self.X2(guide0[:n, 0:2]); gr0 = self.X2(guide0[:n, 2:4])
            bp0 = np.array(curves0[self.course])
            lat0 = np.interp(np.arange(n, dtype=float), bp0[:, 0], bp0[:, 1])
            for k in range(n):
                q = gl0[k] + (gr0[k] - gl0[k]) * lat0[k]
                ids = np.concatenate([sect_tris[(sec_of[k] + d_) % nsec] for d_ in (-1, 0, 1)])
                ids = ids[floor[ids]]
                Tq = tris[ids]; Aq = Tq; Bq = np.roll(Tq, -1, axis=1)
                fq = np.array([-R[k][2], 0.0, R[k][0]]); q3 = np.array([q[0], C[k][1], q[1]])
                da = (Aq - q3) @ fq; db = (Bq - q3) @ fq
                with np.errstate(all='ignore'):
                    tt = np.where(np.abs(da - db) > 1e-9, da / (da - db), 0.5)
                Pt = Aq + (Bq - Aq) * tt[..., None]
                okq = ((da * db) <= 0) & (np.abs(Pt[..., 1] - C[k][1]) < 300.0)
                if okq.any():
                    latq = (Pt - q3) @ R[k]
                    lo_q, hi_q = latq[okq].min(), latq[okq].max()
                    shift = 0.0
                    if hi_q - lo_q < 2 * AI_WALL_CLEAR_FT:
                        shift = (lo_q + hi_q) / 2
                    elif hi_q < AI_WALL_CLEAR_FT:
                        shift = hi_q - AI_WALL_CLEAR_FT
                    elif lo_q > -AI_WALL_CLEAR_FT:
                        shift = lo_q + AI_WALL_CLEAR_FT
                    if shift:
                        q = q + shift * R[k][[0, 2]]; rl_shifted += 1
                RL[k] = q

        # ---- node selection (shared by AI and map splines); chords follow the centre line AND the
        # racing line within the tolerance
        tol = NODE_TOLERANCE""")
rep("""                if i - last > 1 and chord > 0:
                    u = (b - a)[[0, 2]] / chord
                    for j in range(last + 1, i):
                        w = (C[order[j]] - a)[[0, 2]]
                        dev = max(dev, abs(w[0] * u[1] - w[1] * u[0]))""", """                if i - last > 1 and chord > 0:
                    u = (b - a)[[0, 2]] / chord
                    for j in range(last + 1, i):
                        w = (C[order[j]] - a)[[0, 2]]
                        dev = max(dev, abs(w[0] * u[1] - w[1] * u[0]))
                    ra, rb = RL[order[last]], RL[order[i]]
                    rc = np.linalg.norm(rb - ra)
                    if rc > 0:
                        ru = (rb - ra) / rc
                        for j in range(last + 1, i):
                            w = RL[order[j]] - ra
                            dev = max(dev, abs(w[0] * ru[1] - w[1] * ru[0]))""")
rep("""            shifted = 0
            for j, k in enumerate(ai_idx):
                q = gl[k] + (gr[k] - gl[k]) * lateral[k]""", """            shifted = rl_shifted
            for j, k in enumerate(ai_idx):
                q = RL[k].copy()""")
rep("""                    if shift:
                        q = q + shift * R[k][[0, 2]]; shifted += 1
                h = heights_under""", """                    if shift:
                        q = q + shift * R[k][[0, 2]]
                h = heights_under""")
rep("""            self.flags.append('AI racing line: %d nodes moved inward""", """            self.flags.append('AI racing line: %d arcade line points moved inward""")
open(p, 'w').write(s)
print('ok')
