r"""Zones and portals for a converted arcade course: vertical slabs cut on the geometry grid.

Why (notes\arcade-visibility.md):
- The arcade draws its landscape with no occlusion data at all. Each frame f_0C147CA2 tests every
  block (node) of landscape object 0 against the view frustum by its bounding sphere (f_0C14EAA6),
  sorts the survivors by view depth, and f_0C147938 draws the nearest N (landscape record +36,
  overridden per sector by the course hook +44), fading them between two depths (landscape record
  +28/+32) and skipping anything deeper. Collision sectors play no part in drawing.
- Revenge draws a zone's static models only through the portal chain from the camera's zone
  (27F650/27F930), clipped to the screen rectangle of that chain (2805B4 scissor) and box-tested
  against the portal-narrowed matrix (280720). Stock tracks satisfy two rules for that: every zone's
  geometry lies on its own side of its portals' planes, and a portal covers the whole opening
  between two zones, ground to above the tallest scenery (TA: portals up to 8,545 ft wide).
- A converted course meets both rules, and reproduces the arcade's "no occlusion", when its zones
  are vertical slabs between parallel planes on lines of the 2016-ft geometry grid (so no visual
  piece crosses a plane: geo_course.py already clips every piece to that grid) and each portal is
  the whole plane across the world. Every line of sight from one slab to another then passes
  through each portal between them in order, so the portal walk reaches every slab the camera can
  see and the scissor/narrowed matrix never cut a piece that is in view.

One rule for every course; the only inputs are the course's own data (geometry grid, nodes,
camera paths). Generated layout [G]; nothing here is per-course.
"""
import numpy as np

CELL = 2016.0            # geo_course.CELL: visual pieces never cross a multiple of this (both axes)
NODE_CLEAR_FT = 2.0      # no spline node closer than this to a portal plane (a node on the plane makes
                         # both of its segments cross it in the port's node-zone walk)
GROUP_CLEAR_FT = 25.0    # a plane keeps this far from every point set that must stay in one zone
COL_MARGIN_FT = 100.0    # collision faces within this distance past a plane are in both zones (floor
                         # continues past a portal; pods jammed where it did not, a2r-track.md 8.3)
MAX_ZONES = 64           # port: maxTrackZones50ZV / maxVehicleWindowTrackZones50DS


class Slabs:
    def __init__(self, axis, planes, zone_of_slab):
        self.axis = axis                      # 0 = planes x = c, 2 = planes z = c
        self.planes = np.asarray(planes, np.float64)
        self.zone_of_slab = np.asarray(zone_of_slab, np.int64)   # slab index (ascending) -> zone 1..Z
        self.Z = len(self.planes) + 1

    def slab(self, coord):
        return np.searchsorted(self.planes, np.asarray(coord, np.float64), side='right')

    def zone(self, pts):
        """pts (..., 3) Revenge -> zone 1..Z."""
        return self.zone_of_slab[self.slab(np.asarray(pts, np.float64)[..., self.axis])]

    def slab_range(self, s):
        lo = self.planes[s - 1] if s > 0 else -np.inf
        hi = self.planes[s] if s < len(self.planes) else np.inf
        return lo, hi

    def zones_overlapping(self, lo, hi, margin):
        """zones whose slab range overlaps [lo - margin, hi + margin] along the axis."""
        a = self.slab(lo - margin); b = self.slab(hi + margin)
        return [int(self.zone_of_slab[s]) for s in range(int(a), int(b) + 1)]


def choose(cell_lo, cell_hi, nodes, groups, tri_coords, start_point, log=None):
    """cell_lo/cell_hi: (2,) min/max geometry-grid cell index on x and z (visual pieces);
    nodes: (n, 3) every spline node (map, AI, fork branches); groups: list of (m, 3) point sets that
    must each stay inside one zone (start grid with node 0, each cut camera path);
    tri_coords: (t, 3, 3) collision triangles (balance only); start_point: map node 0.
    Returns Slabs. For each axis the candidate planes are the interior grid lines; a line is dropped
    when a node lies within NODE_CLEAR_FT of it or it passes through a group. The axis whose largest
    zone holds the fewest collision faces wins (tie: more zones)."""
    best = None
    for axis, ai in ((0, 0), (2, 1)):
        cands = [c * CELL for c in range(int(cell_lo[ai]) + 1, int(cell_hi[ai]) + 1)]
        keep, why = [], {}
        nc = nodes[:, axis]
        for p in cands:
            if np.any(np.abs(nc - p) < NODE_CLEAR_FT):
                why[p] = 'node within %.0f ft' % NODE_CLEAR_FT
                continue
            hit = [i for i, g in enumerate(groups) if g[:, axis].min() - GROUP_CLEAR_FT < p < g[:, axis].max() + GROUP_CLEAR_FT]
            if hit:
                why[p] = 'splits point set(s) %s' % hit
                continue
            keep.append(p)
        while len(keep) + 1 > MAX_ZONES:      # merge the narrowest pair of slabs
            gaps = np.diff([-np.inf] + keep + [np.inf])
            keep.pop(int(np.argmin(gaps[1:-1])))
        planes = np.array(keep)
        lo = tri_coords[:, :, axis].min(1); hi = tri_coords[:, :, axis].max(1)
        counts = np.zeros(len(planes) + 1, np.int64)
        for s in range(len(planes) + 1):
            a = planes[s - 1] - COL_MARGIN_FT if s > 0 else -np.inf
            b = planes[s] + COL_MARGIN_FT if s < len(planes) else np.inf
            counts[s] = int(((hi >= a) & (lo <= b)).sum())
        score = (int(counts.max()), -len(planes))
        if log is not None:
            log.append(dict(axis='xz'[ai], candidates=len(cands), planes=[float(p) for p in planes],
                            dropped={'%.0f' % k: v for k, v in why.items()}, faces_per_zone=counts.tolist()))
        if best is None or score < best[0]:
            best = (score, axis, planes)
    _, axis, planes = best
    s0 = int(np.searchsorted(planes, start_point[axis], side='right'))
    nslab = len(planes) + 1
    order = [(s0 + i) % nslab for i in range(nslab)]          # zone 1 = the slab of map node 0
    zone_of_slab = np.zeros(nslab, np.int64)
    for zi, s in enumerate(order, 1):
        zone_of_slab[s] = zi
    return Slabs(axis, planes, zone_of_slab)


def portals(sl, lo3, hi3):
    """One portal per plane: the whole plane across the world box lo3..hi3 (already padded).
    Vertex order as the converter always wrote them: (a, ylo), (a, yhi), (b, yhi), (b, ylo)."""
    out = []
    other = 2 if sl.axis == 0 else 0
    for k, c in enumerate(sl.planes):
        za, zb = int(sl.zone_of_slab[k]), int(sl.zone_of_slab[k + 1])
        a = np.zeros(3); b = np.zeros(3)
        a[sl.axis] = b[sl.axis] = c
        a[other], b[other] = lo3[other], hi3[other]
        v = [(a[0], lo3[1], a[2]), (a[0], hi3[1], a[2]), (b[0], hi3[1], b[2]), (b[0], lo3[1], b[2])]
        out.append(dict(v=v, c=(za, zb), plane=float(c)))
    return out
