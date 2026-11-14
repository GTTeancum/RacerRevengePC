r"""Menu (UI) data for converted pods: one code path for every pod.

    python -I ui_convert.py                      (the six siloed pods: ARK 04, BOL 15, EBE 23, ELA 24, NEV 11, RAT 17)
    python -I ui_convert.py ARK 04 [NEV 11 ...]

Reads the original game folder (read only), the converted pod archives in
export\a2r\pods\PODnn\install\PODS\PODnn\PODnn.RES and the arcade pilot data (pod_export / racer_assets).
Writes D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\ui\:
    <game-relative path>               every changed archive, laid out as in the game folder
    fragments\<path with _>.json       converted-content row per changed file (silo schema files[] row)
    converted-content.fragment.json    all rows in one files[] list
    manifest.json                      provenance per changed member: A arcade / G generated / D borrowed
    members\<archive>\...              changed members loose
Nothing is written to silo\ or the port.

Per pod the menus use (inventory: notes\ui-convert.md):
  UIRIDER (and the copies in UITOURN / UIUNLOCK): ui\geometry\nn.psg rotating pod, ui\anim\nnc.psg/.mcf/.vut
  + six nnc_*.mot standing character, ui\textures\nnc_1/2.pst character pages, nn_shdw.pst pod shadow,
  nn_flrg.pst rider flag; UIPODTEX nn_1/2/3(.pst) pod pages; UIBKG nn_bkg.pst backdrop; OVERLAYS nn_fsm.pst
  results flag; UIGARAGE\nnE.RES nne.psg + nne_1.pst garage engines; PODPHYS pods\podnn\podnn.phy (stats bars);
  LOC.RES the STR_RIDER_SELECTION_PAGE_* text the rider page's Name widget names (rider.dat -> Widgets.Dat).
The same member name in several archives gets the same new bytes (one rule, no per-archive branch).
UIART1/2/3 artpodNN / artpilNN / artplaNN are gallery page numbers, not pod numbers: not touched.
"""
import hashlib
import json
import math
import os
import re
import struct
import sys

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, TOOLS)
import geo_encode  # noqa: E402
import geo_psg  # noqa: E402
import geo_pst  # noqa: E402
import geo_res  # noqa: E402
import pod_formats as PF  # noqa: E402
import pod_psg  # noqa: E402
import trk_res3  # noqa: E402
F32 = geo_encode.F32

GAME = r'C:\Games\Star Wars Racer Revenge'
OUT = os.path.join(SCR, 'export', 'a2r', 'ui')
PODS_OUT = os.path.join(SCR, 'export', 'a2r', 'pods')
BS = chr(92)
TARGETS = dict(ARK='04', BOL='15', EBE='23', ELA='24', NEV='11', RAT='17')
PILOT_NAMES = dict(ARK='Ark "Bumpy" Roose', BOL='Boles Roor', EBE='Ebe Endocott', ELA='Elan Mak',
                   NEV='Neva Kee', RAT='Ratts Tyerell')
ALL_PODS = ['01', '02', '03', '04', '07', '08', '09', '10', '11', '12', '14', '15', '16', '17', '18', '19', '20',
            '21', '22', '23', '24', '25', '26']
ARCHIVES = [r'LOC.RES', r'UI\PODPHYS.RES', r'UI\TEXTURES\OVERLAYS.RES', r'UI\UIBKG.RES', r'UI\UIPODTEX.RES',
            r'UI\UIRIDER\UIRIDER.RES', r'UI\UITOURN\UITOURN.RES', r'UI\UIUNLOCK\UIUNLOCK.RES', r'UI\UIWIN\UIWIN.RES',
            r'UI\UIHOF\UIHOF.RES', r'UI\UIGARAGE\UIGARAGE.RES'] + [r'UI\UIGARAGE\%sE.RES' % p for p in ALL_PODS]
LANGS = [('english', 'ENG'), ('french', 'FRE'), ('german', 'GER'), ('italian', 'ITA'), ('spanish', 'SPA')]
# arcade pilot motion (GMOT name part) used for each Revenge rider-page motion (A); 'operate' is a 1-frame
# held pose. Revenge suffix -> arcade motion. One rule for every pilot.
MOTION_MAP = dict(ent='_joy_', sel='_joymade_', idl1='_L_chira_', idl2='_R_chira_', idl3='_L_pro_', breathe='_operate_')
ARCADE_FPS = 60.0       # pods.md: one key per 60 Hz frame (guess carried from pod_export)
UI_FPS = 15.0
SEATED_HEIGHT = 0.7     # G: the arcade pilot is a seated figure; its height = 0.7 x the original standing character


def sha(b):
    return hashlib.sha256(b).hexdigest()


def low(n):
    return n.lower()


# ============================================================ archives
class Arch:
    def __init__(self, rel):
        self.rel = rel
        self.raw = open(os.path.join(GAME, rel), 'rb').read()
        self.r = trk_res3.read_res3(self.raw)
        self.by = {low(n): (n, lo, sz) for n, lo, sz in self.r.members}

    def get(self, name):
        n, lo, sz = self.by[low(name)]
        return self.r.payload[lo:lo + sz]

    def has(self, name):
        return low(name) in self.by

    def find_base(self, base):
        for k, (n, lo, sz) in self.by.items():
            if k.split(BS)[-1] == low(base):
                return self.r.payload[lo:lo + sz]
        return None


_ARCH = {}


def arch(rel):
    if rel not in _ARCH:
        _ARCH[rel] = Arch(rel)
    return _ARCH[rel]


def ini(text):
    out, sec = {}, None
    for line in text.replace('\r', '').split('\n'):
        s = line.split(';', 1)[0].strip()
        if s.startswith('[') and s.endswith(']'):
            sec = s[1:-1].strip(); out.setdefault(sec, {}); continue
        if '=' in s and sec is not None:
            k, v = s.split('=', 1); out[sec][k.strip()] = v.strip()
    return out


# ============================================================ textures
def pst_rgba(blob):
    t = geo_pst.parse(blob)
    if t['kind'] == 2:
        return None, t
    return geo_pst.rgba_levels(t)[-1], t


def encode_like(img, orig_blob):
    """RGBA image -> kind-1 PST with the original member's base size and mip count."""
    t = geo_pst.parse(orig_blob)
    w, h = t['levels'][-1]['w'], t['levels'][-1]['h']
    mips = len(t['levels'])
    if img.shape[1] != w or img.shape[0] != h:
        img = np.array(Image.fromarray(img, 'RGBA').resize((w, h), Image.LANCZOS))
    blob, info = geo_pst.encode_rgba(np.ascontiguousarray(img), max_side=max(w, h), min_side=min(16, w, h), max_levels=mips)
    t2 = geo_pst.parse(blob)
    assert (t2['levels'][-1]['w'], t2['levels'][-1]['h'], len(t2['levels'])) == (w, h, mips), 'PST layout'
    return blob, dict(size='%dx%d' % (w, h), mips=mips, kind_original=t['kind'], error=info['mean_abs_error_per_channel'])


def resize(img, w, h):
    return np.array(Image.fromarray(img, 'RGBA').resize((w, h), Image.LANCZOS))


def saturation(rgb):
    rgb = rgb.astype(float)
    mx, mn = rgb.max(-1), rgb.min(-1)
    return np.where(mx > 0, (mx - mn) / np.maximum(mx, 1), 0)


# ============================================================ PSG helpers
def model_tris(blob, lod=0, skip=lambda m: False):
    """World-space triangles per material: list of (material, P (n,3), N (n,3), UV (n,2), tris (m,3))."""
    m = geo_psg.parse(blob)
    W = pod_psg.world_matrices(m['nodes'])
    out = []
    for s in geo_psg.vertices(m, lod):
        if skip(s['material']):
            continue
        P = np.array([(np.r_[p, 1] @ W[b])[:3] for p, b in zip(s['pos'], s['bone'])])
        N = np.array([np.asarray(n, float) @ W[b][:3, :3] for n, b in zip(s['normal'], s['bone'])])
        out.append((s['material'], P, N, np.array(s['uv'], float), np.array(s['tris'])))
    return m, out


def box(P):
    P = np.asarray(P, float)
    return P.min(0), P.max(0)


def surf(material, P, N, UV, tris, bone=None):
    return dict(material=material, P=np.asarray(P, float), N=np.asarray(N, float), UV=np.asarray(UV, float),
                bone=np.zeros(len(P), int) if bone is None else np.asarray(bone, int), tris=np.asarray(tris, int))


# ============================================================ MOT (ShortAxis rotation, Bezier position)
def short_axis(q):
    """Quaternion (w,x,y,z) -> (angle u16 < 4096, axis s16 x3) as decoded by model_motion49t ShortAxis."""
    q = np.asarray(q, float)
    if q[0] < 0:
        q = -q
    half = math.acos(max(-1.0, min(1.0, q[0])))
    k = int(round(2 * half * 4096 / (2 * math.pi)))
    if k >= 4096:
        k = 4095
    v = q[1:]
    vn = np.linalg.norm(v)
    if k == 0 or vn < 1e-12:
        return 0, (32767, 0, 0)
    a = np.clip(np.round(v / vn * 32767), -32767, 32767).astype(int)
    return k, tuple(int(x) for x in a)


def decode_short_axis(k, a):
    h = 0.5 * (k / 4096.0) * 2 * math.pi
    return np.array([math.cos(h)] + [x / 32767.0 * math.sin(h) for x in a])


def write_mot_short(frames, duration, pos_tracks, rot_tracks):
    out = bytearray(struct.pack('<IIfIIII', 0xFFFFFFFE, frames, duration, 1, 1, len(pos_tracks), len(rot_tracks)))
    for row, keys in pos_tracks:
        assert len(keys) >= 2
        out += struct.pack('<II', len(keys), row)
        for t, p in keys:
            out += struct.pack('<10f', t, *p, 0, 0, 0, 0, 0, 0)
    for row, keys in rot_tracks:
        assert len(keys) >= 2
        out += struct.pack('<II', len(keys), row)
        for t, L3 in keys:
            k, a = short_axis(PF.quat_from_rows(L3))
            out += struct.pack('<fHhhh', t, k, *a)
    b = bytes(out)
    check_mot(b)
    return b


def check_mot(b):
    """readMot49O admission (src\\assets\\model_geometry49o.cpp:78-128)."""
    tag, nf, dur, rt, one, npos, nrot = struct.unpack_from('<IIfIIII', b, 0)
    assert tag == 0xFFFFFFFE and 1 <= nf <= 256 and math.isfinite(dur) and dur > 0 and rt <= 1 and one == 1
    assert npos <= 64 and nrot <= 64
    at, total = 28, 0
    for ch, cnt in ((0, npos), (1, nrot)):
        st = 40 if ch == 0 else (12 if rt else 8)
        for _ in range(cnt):
            k, row = struct.unpack_from('<II', b, at); at += 8
            assert 1 <= k <= 256 and row < 64
            total += k
            prev = 0
            for j in range(k):
                t = struct.unpack_from('<f', b, at + j * st)[0]
                assert math.isfinite(t) and 0 <= t <= dur and (j == 0 or t > prev)
                if ch == 1 and rt == 1:
                    assert struct.unpack_from('<H', b, at + j * st + 4)[0] < 4096
                prev = t
            at += k * st
    assert total <= 4096 and at == len(b)
    return dict(frames=nf, duration=round(dur, 4), rotation_type=rt, position_tracks=npos, rotation_tracks=nrot, keys=total)


def read_mot(b):
    tag, nf, dur, rt, one, npos, nrot = struct.unpack_from('<IIfIIII', b, 0)
    at = 28
    pos, rot = [], []
    for ch, cnt in ((0, npos), (1, nrot)):
        st = 40 if ch == 0 else (12 if rt else 8)
        for _ in range(cnt):
            k, row = struct.unpack_from('<II', b, at); at += 8
            keys = []
            for j in range(k):
                p = at + j * st
                if ch == 0:
                    keys.append((struct.unpack_from('<f', b, p)[0], np.array(struct.unpack_from('<3f', b, p + 4))))
                elif rt == 1:
                    t, a, x, y, z = struct.unpack_from('<fHhhh', b, p)
                    keys.append((t, decode_short_axis(a, (x, y, z))))
                else:
                    t, a, x, y, z = struct.unpack_from('<fBbbb', b, p)
                    keys.append((t, PF.decode_byte_axis(a, (x, y, z))))
            at += k * st
            (pos if ch == 0 else rot).append((row, keys))
    return dict(frames=nf, duration=dur, rtype=rt, pos=pos, rot=rot)


def pose_world(model, mot, t=0.0):
    """World matrices of a PSG posed by a MOT at time t (key nearest below t; rotations replace the node's
    local rotation, positions replace its local translation)."""
    rows = pod_psg.rows(model['nodes'])
    L = [np.array(n['matrix'], float).reshape(4, 4).copy() for n in model['nodes']]

    def at(keys):
        best = keys[0]
        for k in keys:
            if k[0] <= t + 1e-6:
                best = k
        return best[1]
    if mot is not None:
        for row, keys in mot['rot']:
            L[rows[row]][:3, :3] = PF.rows_from_quat(at(keys))
        for row, keys in mot['pos']:
            L[rows[row]][3, :3] = at(keys)
    W = []
    for i, n in enumerate(model['nodes']):
        W.append(L[i] if n['parent'] < 0 else L[i] @ W[n['parent']])
    return W


def posed_points(model, W, lod=0):
    pts = []
    for s in geo_psg.vertices(model, lod):
        for p, b in zip(s['pos'], s['bone']):
            pts.append((np.r_[p, 1] @ W[b])[:3])
    return np.array(pts)


# ---------------------------------------------------------------- PSG encoder with optional vertex colour
# Copy of pod_psg.build (pod converter's encoder, left untouched) that also writes the 0x6E colour block
# when the component word carries colour (garage models are prelit: componentWord 0x00FF010F); each
# surface then needs C (n, 4) u8.
def build_psg(nodes, lods, component, materials=None):
    """nodes: list of dict(name, matrix (16 floats, row vectors), parent).
    lods: list of dict(cutoff, surfaces=[dict(material (name), P (n,3) node-local, N (n,3), UV (n,2),
          bone (n,), tris (m,3))]). Triangles must already be oriented so cross(b-a, c-a) follows the
          vertex normals. Returns (bytes, info)."""
    colour = (component & 0xFF) == 0x0F
    allP = [s['P'][np.unique(np.asarray(s['tris']).reshape(-1))] for L in lods for s in L['surfaces'] if len(s['tris'])]
    ext = max(float(np.abs(p).max()) for p in allP)
    scale = F32(max(ext, 1e-6) / 32767.0)
    while max(float(np.abs(np.round(p / scale)).max()) for p in allP) > 32767:
        scale = F32(scale * 1.0000001 + 1e-12)
    mats = []
    out_lods = []
    node_pts = [[] for _ in nodes]
    stats = dict(triangles=0, dropped_turned_slivers=0, packed_vertices=0)
    for L in lods:
        surfs = []
        for s in L['surfaces']:
            tris = np.asarray(s['tris'], int).reshape(-1, 3)
            if not len(tris):
                continue
            P = np.asarray(s['P'], float); N = np.asarray(s['N'], float); UV = np.asarray(s['UV'], float)
            B = np.asarray(s['bone'], int)
            C = np.asarray(s['C'], int) if colour else np.zeros((len(P), 4), int)
            Q = np.round(P / scale).astype(int)
            nl = np.linalg.norm(N, axis=1, keepdims=True); nl[nl == 0] = 1
            NQ = np.clip(np.round(N / nl * 127), -127, 127).astype(int)
            UQ = np.round(UV / geo_encode.UV_SCALE).astype(int)
            used = np.unique(tris)
            if np.abs(UQ[used]).max() > 32767:
                raise ValueError('UV outside +-16')
            key, vid, attrs = {}, np.empty(len(P), int), []
            for i in used:
                k = (tuple(Q[i]), tuple(NQ[i]), tuple(UQ[i]), int(B[i]), tuple(C[i]))
                if k not in key:
                    key[k] = len(attrs); attrs.append(k)
                vid[i] = key[k]
            keep = []
            for t in tris:
                q = Q[t] * scale
                f = P[t]
                if np.cross(f[1] - f[0], f[2] - f[0]) @ N[t].sum(0) > 0 and \
                        np.cross(q[1] - q[0], q[2] - q[0]) @ NQ[t].sum(0) < 0:
                    stats['dropped_turned_slivers'] += 1
                    continue
                if len({tuple(Q[i]) for i in t}) < 3:
                    stats['dropped_turned_slivers'] += 1   # collapsed to a line by quantisation
                    continue
                keep.append(t)
            t2 = [tuple(vid[t]) for t in keep]
            strips = geo_encode.stripify(t2)
            canon = lambda t: min((t[0], t[1], t[2]), (t[1], t[2], t[0]), (t[2], t[0], t[1]))  # noqa: E731
            assert sorted(canon(t) for st in strips for t in geo_encode.strip_triangles(st)) == sorted(canon(t) for t in t2)
            sa = []
            for st in strips:
                row = []
                for j, v in enumerate(st):
                    q, n, uv, b, c = attrs[v]
                    w = 16 * (b + 1)
                    row.append(((q[0], q[1], q[2], -w if j < 2 else w), n, uv, c if colour else None))
                sa.append(row)
            groups = geo_encode.pack(sa)
            nv = sum(len(g) for g in groups)
            assert nv <= 65535
            for q, n, uv, b, c in attrs:
                node_pts[b].append(np.array(q, float) * scale)
            mats.append(s['material'])
            Pk = P[np.unique(np.asarray(keep).reshape(-1))] if keep else P[:1]
            surfs.append(dict(material=len(mats) - 1, component=component,
                              uv_size=F32(geo_encode.uv_size(P, UV, [tuple(t) for t in keep]) if keep else 1.0),
                              vertex_count=nv, strip_count=len(strips), tail_pad=None,
                              groups=[dict(wide=False, xyzw=[v[0] for v in g], normal=[v[1] for v in g],
                                           uv=[v[2] for v in g], rgba=[v[3] for v in g] if colour else None) for g in groups]))
            stats['triangles'] += len(keep)
            stats['packed_vertices'] += nv
        out_lods.append(dict(cutoff=L['cutoff'], surfaces=surfs))
    out_nodes = []
    for i, nd in enumerate(nodes):
        pts = node_pts[i]
        if pts:
            pts = np.array(pts)
            # stored the way the decoder recomputes: decoded float32 positions
            pts = pts.astype(np.float32).astype(float)
            lo, hi = pts.min(0), pts.max(0)
            c, h = (lo + hi) / 2, (hi - lo) / 2
        else:
            c = h = np.zeros(3)
        mtx = [F32(x) for x in nd['matrix']]
        mtx[3] = mtx[7] = mtx[11] = 0.0      # affine (decoder: m[3] = m[7] = m[11] = 0, m[15] = 1)
        mtx[15] = 1.0
        out_nodes.append(dict(name=nd['name'], name_raw=None, matrix=mtx,
                              centre=[F32(x) for x in c], half=[F32(x) for x in h],
                              radius=F32(float(np.linalg.norm(h))), parent=int(nd['parent'])))
    model = dict(nodes=out_nodes, materials=mats, materials_raw=[None] * len(mats),
                 quant=[scale, geo_encode.NORMAL_SCALE, geo_encode.UV_SCALE, geo_encode.UV_SCALE, 0.0, 0.0, 0.0],
                 world=0, lods=out_lods, palette=None)
    blob = geo_psg.write(model)
    assert geo_psg.write(geo_psg.parse(blob)) == blob
    stats.update(bytes=len(blob), nodes=len(nodes), quantum_ft=scale, materials=mats,
                 lods=[len(L['surfaces']) for L in out_lods])
    return blob, stats


# ============================================================ converter
class UiPod:
    def __init__(self, code, nn):
        self.code, self.nn = code, nn
        self.name = PILOT_NAMES[code]
        self.ui_name = menu_name(self.name)
        res = os.path.join(PODS_OUT, 'POD%s' % nn, 'install', 'PODS', 'POD%s' % nn, 'POD%s.RES' % nn)
        self.conv, self.conv_order, _ = geo_res.read_res(res)
        self.conv_res_sha = sha(open(res, 'rb').read())
        self.pages = json.load(open(os.path.join(PODS_OUT, 'POD%s' % nn, 'manifest.json')))['pages']
        self.orig_race, _, _ = geo_res.read_res(os.path.join(GAME, 'PODS', 'POD%s' % nn, 'POD%s.RES' % nn))
        self.rider = arch(r'UI\UIRIDER\UIRIDER.RES')
        self.out = {}          # lower member name -> (bytes, source, flags)
        self.facts = {}

    def put(self, name, blob, src, *flags):
        self.out[low(name)] = (blob, src, list(flags))

    def cget(self, name):
        return self.conv[low(name)]

    def conv_page(self, page):
        img, t = pst_rgba(self.cget('textures' + BS + '%s.pst' % page))
        return img

    # ---------------------------------------------------------------- pod pages (UIPODTEX)
    def podtex(self):
        a = arch(r'UI\UIPODTEX.RES')
        role = {v.lower(): k for k, v in self.pages.items()}
        for n, lo, sz in a.r.members:
            base = n.split(BS)[-1].lower()
            m = re.match(r'(%s)_(\d)\.pst$' % self.nn, base)
            if not m:
                continue
            page = base[:-4]
            if page not in role:
                continue      # page the converted pod does not draw (e.g. 24_4): original kept
            img = self.conv_page(page)
            blob, info = encode_like(img, a.get(n))
            self.put(n, blob, 'A', 'converted pod page %s (%s) resampled to the original UI page %s, %d mip(s) [A texels, G resample]'
                     % (page, {'E': 'engines', 'C': 'hull', 'P': 'pilot'}[role[page]], info['size'], info['mips']),
                     'alpha = the converted page alpha (specular-mask level), as in the race archive')

    # ---------------------------------------------------------------- converted pod geometry (pod frame)
    def pod_geometry(self):
        if hasattr(self, '_geo'):
            return self._geo
        P_page = self.pages['P'].lower()
        parts = {}
        for part in ('chariot', 'englt1', 'engrt1'):
            blob = self.cget('pods' + BS + 'pod%s' % self.nn + BS + '%s.psg' % part)
            m, sl = model_tris(blob, 0, skip=lambda mt: mt.lower().startswith('flame') or mt.lower() == P_page)
            parts[part] = (m, sl)
        self._geo = parts
        return parts

    def node_world(self, part, name):
        m = self.pod_geometry()[part][0]
        W = pod_psg.world_matrices(m['nodes'])
        for i, n in enumerate(m['nodes']):
            if n['name'].lower() == name.lower():
                return W[i][3, :3]
        raise KeyError('%s node %s' % (part, name))

    # ---------------------------------------------------------------- rotating pod (UIRIDER ui\geometry\nn.psg)
    def ui_pod(self):
        orig_blob = self.rider.get('ui' + BS + 'geometry' + BS + '%s.psg' % self.nn)
        om, osl = model_tris(orig_blob)
        shd = '%s_shdw' % self.nn
        body = np.vstack([P for mt, P, N, UV, T in osl if mt.lower() not in (shd, 'cable')])
        ob = box(body)
        geo = self.pod_geometry()
        cb = box(np.vstack([P for part in geo.values() for mt, P, N, UV, T in part[1]]))
        s = float(np.max(ob[1] - ob[0]) / np.max(cb[1] - cb[0]))
        t = (ob[0] + ob[1]) / 2 - s * (cb[0] + cb[1]) / 2
        X = lambda P: np.asarray(P) * s + t  # noqa: E731
        by_mat = {}
        for part in ('englt1', 'engrt1', 'chariot'):
            for mt, P, N, UV, T in geo[part][1]:
                by_mat.setdefault(mt.lower(), []).append((X(P), N, UV, T))
        surfaces = []
        orig_mats = [m.lower() for m in om['materials']]
        for mt in [m for m in orig_mats if m not in (shd, 'cable')] + [m for m in by_mat if m not in orig_mats]:
            if mt not in by_mat:
                continue
            P, N, UV, T, off = [], [], [], [], 0
            for p, n, uv, tr in by_mat[mt]:
                P.append(p); N.append(n); UV.append(uv); T.append(tr + off); off += len(p)
            name = [m for m in om['materials'] if m.lower() == mt]
            surfaces.append(surf(name[0] if name else mt, np.vstack(P), np.vstack(N), np.vstack(UV), np.vstack(T)))
        new_body = np.vstack([sf['P'] for sf in surfaces])
        nb = box(new_body)
        # nodes: original node list (names, matrices); binder nodes moved to the converted engines' binder nodes
        nodes = []
        for i, nd in enumerate(om['nodes']):
            mtx = list(nd['matrix'])
            if i > 0:
                side = 'englt1' if nd['name'].lower().endswith('left1') else 'engrt1'
                p = X(self.node_world(side, nd['name']))
                mtx[12:15] = [float(x) for x in p]
            nodes.append(dict(name=nd['name'], matrix=mtx, parent=nd['parent']))
        # static cables: chariot cable node -> engine cable node, sagging tube [G]
        cab = []
        L = float(np.max(nb[1] - nb[0]))
        for side, (cn, en, ep) in enumerate((('chariotcableleft1', 'enginecableleft1', 'englt1'),
                                             ('chariotcableright1', 'enginecableright1', 'engrt1'))):
            a = X(self.node_world('chariot', cn)); b = X(self.node_world(ep, en))
            cab.append(tube(a, b, radius=0.004 * L, sag=0.03 * np.linalg.norm(b - a)))
        P = np.vstack([c[0] for c in cab]); N = np.vstack([c[1] for c in cab]); UV = np.vstack([c[2] for c in cab])
        T = np.vstack([cab[0][3], cab[1][3] + len(cab[0][0])])
        surfaces.append(surf('cable', P, N, UV, T))
        # shadow quad + texture [G]
        sq, simg = self.shadow(om, osl, new_body, nb, surfaces)
        surfaces.append(sq)
        blob, info = build_psg(nodes, [dict(cutoff=om['lods'][0]['cutoff'], surfaces=surfaces)],
                                   om['lods'][0]['surfaces'][0]['component'])
        self.ui_pod_scale = s
        self.put('ui' + BS + 'geometry' + BS + '%s.psg' % self.nn, blob, 'A',
                 'converted race pod LOD 0 (chariot hull + both engines, pilot page %s and flames left out: the original '
                 'rider-page pods carry no pilot) [A geometry]' % self.pages['P'],
                 'uniform scale %.3f and offset so the converted pod spans the original UI pod box (largest side) [G]' % s,
                 'race layout kept: the original UI art raises the chariot above the engines, the converted pod does not [G]',
                 'nodes: original node list; binderleft1/right1 at the converted engines\' binder nodes [A positions]',
                 'cables: two generated tubes chariotcable -> enginecable nodes, material cable [G]',
                 'shadow quad at y = -(highest point), as on the originals; footprint margins from the originals [G]')
        self.facts['ui_pod'] = dict(scale=round(s, 4), nodes=len(nodes), materials=[sf['material'] for sf in surfaces], **{k: v for k, v in info.items() if k in ('triangles', 'bytes')})
        return simg

    def shadow(self, om, osl, new_body, nb, surfaces):
        shd = '%s_shdw' % self.nn
        quad = [(P, N, UV, T) for mt, P, N, UV, T in osl if mt.lower() == shd and len(P) == 4]
        src = 'target'
        if not quad:       # target authored a mesh shadow: take the layout rule of every quad original
            quad = [quad_of(p) for p in ALL_PODS]
            quad = [q for q in quad if q is not None][:1]
            src = 'first quad original'
        P, N, UV, T = quad[0]
        body_o = np.vstack([p for mt, p, n, uv, t in osl if mt.lower() not in (shd, 'cable')])
        ob = box(body_o)
        ratio = margin_ratios()
        c = (nb[0] + nb[1]) / 2
        h = (nb[1] - nb[0]) / 2 * ratio
        y = -float(nb[1][1])
        # affine (xn, zn) -> uv fitted on the original quad
        oc, oh = (P.min(0) + P.max(0)) / 2, (P.max(0) - P.min(0)) / 2
        A = np.c_[np.ones(4), (P[:, 0] - oc[0]) / oh[0], (P[:, 2] - oc[2]) / oh[2]]
        cu = np.linalg.lstsq(A, UV, rcond=None)[0]
        corners = np.array([[sx, sz] for sx in (-1, 1) for sz in (-1, 1)])
        QP = np.c_[c[0] + corners[:, 0] * h[0], np.full(4, y), c[2] + corners[:, 1] * h[2]]
        QUV = np.c_[np.ones(4), corners] @ cu
        n = N[0] / np.linalg.norm(N[0])
        tris = []
        for tri in ((0, 1, 3), (0, 3, 2)):
            a, b, cc = QP[list(tri)]
            if np.cross(b - a, cc - a) @ n < 0:
                tri = (tri[0], tri[2], tri[1])
            tris.append(tri)
        sq = surf(om['materials'][[m.lower() for m in om['materials']].index(shd)], QP, np.tile(n, (4, 1)), QUV, tris)
        # texture: top-down silhouette of the converted body in the quad's uv space, blurred
        oimg, _ = pst_rgba(self.rider.get('ui' + BS + 'textures' + BS + '%s.pst' % shd))
        H, W = oimg.shape[:2]
        S = 4
        mask = Image.new('L', (W * S, H * S), 0)
        dr = ImageDraw.Draw(mask)
        for sf in surfaces:
            if sf['material'].lower() == 'cable':
                continue
            xn = (sf['P'][:, 0] - c[0]) / h[0]; zn = (sf['P'][:, 2] - c[2]) / h[2]
            uv = np.c_[np.ones(len(xn)), xn, zn] @ cu
            px = uv * [W * S, H * S]
            for tr in sf['tris']:
                dr.polygon([tuple(px[i]) for i in tr], fill=255)
        mask = mask.resize((W, H), Image.BOX).filter(ImageFilter.GaussianBlur(W / 40.0))
        amax = int(oimg[..., 3].max())
        a = np.asarray(mask, float) / 255.0
        img = np.zeros((H, W, 4), np.uint8)
        img[..., :3] = np.median(oimg[..., :3][oimg[..., 3] > 0], 0).astype(np.uint8) if (oimg[..., 3] > 0).any() else 0
        img[..., 3] = np.round(a / max(a.max(), 1e-6) * amax).astype(np.uint8)
        blob, info = encode_like(img, self.rider.get('ui' + BS + 'textures' + BS + '%s.pst' % shd))
        self.put('ui' + BS + 'textures' + BS + '%s.pst' % shd, blob, 'G',
                 'top-down silhouette of the converted UI pod in the shadow quad\'s uv space, blurred, peak alpha %d '
                 '(= the original\'s), colour = the original\'s [G]; uv layout from the %s quad' % (amax, src))
        return sq, img

    # ---------------------------------------------------------------- garage engines (UIGARAGE\nnE.RES)
    def garage(self):
        ga = arch(r'UI\UIGARAGE\%sE.RES' % self.nn)
        gname = 'ui' + BS + 'geometry' + BS + '%se.psg' % self.nn
        om, osl = model_tris(ga.get(gname))
        assert len(om['nodes']) == 1 and len(om['lods']) == 1, 'garage model layout'
        P = np.vstack([p for mt, p, n, uv, t in osl])
        xc = (P[:, 0].min() + P[:, 0].max()) / 2
        sides = {-1: P[P[:, 0] < xc], 1: P[P[:, 0] >= xc]}
        geo = self.pod_geometry()
        cv = {-1: geo['englt1'][1], 1: geo['engrt1'][1]}
        cb = {k: box(np.vstack([p for mt, p, n, uv, t in v])) for k, v in cv.items()}
        ob = {k: box(v) for k, v in sides.items()}
        s = float(np.mean([np.max(ob[k][1] - ob[k][0]) / np.max(cb[k][1] - cb[k][0]) for k in (-1, 1)]))
        mat = om['materials'][0]
        PP, NN, UU, TT, off = [], [], [], [], 0
        for k in (-1, 1):
            t = (ob[k][0] + ob[k][1]) / 2 - s * (cb[k][0] + cb[k][1]) / 2
            t[1] = ob[k][0][1] - s * cb[k][0][1]      # standing on the original's floor
            for mt, p, n, uv, tr in cv[k]:
                PP.append(p * s + t); NN.append(n); UU.append(uv); TT.append(tr + off); off += len(p)
        nd = om['nodes'][0]
        M0 = np.array(nd['matrix'], float).reshape(4, 4)        # world -> the node's own frame
        Mi = np.linalg.inv(M0)
        PW = np.vstack(PP)
        PL = (np.c_[PW, np.ones(len(PW))] @ Mi)[:, :3]
        NL = np.vstack(NN) @ Mi[:3, :3]
        sf = surf(mat, PL, NL, np.vstack(UU), np.vstack(TT))
        comp = om['lods'][0]['surfaces'][0]['component']
        prelit = ''
        if (comp & 0xFF) == 0x0F:
            # prelit original: per-channel linear fit colour = a + b . normal over the original's vertices,
            # applied to the converted normals and clamped to the original's range [G]
            ov = geo_psg.vertices(om, 0)
            ON = np.vstack([np.asarray(v['normal'], float) for v in ov])
            ON /= np.maximum(np.linalg.norm(ON, axis=1, keepdims=True), 1e-9)
            OC = np.vstack([np.asarray(v['rgba'], float) for v in ov])
            A = np.c_[np.ones(len(ON)), ON]
            coef = np.linalg.lstsq(A, OC, rcond=None)[0]
            n = sf['N'] / np.maximum(np.linalg.norm(sf['N'], axis=1, keepdims=True), 1e-9)
            sf['C'] = np.clip(np.round(np.c_[np.ones(len(n)), n] @ coef), OC.min(0), OC.max(0)).astype(int)
            prelit = 'vertex colours: linear fit (colour = a + b.normal) of the original prelit colours, applied to the converted normals [G]'
        blob, info = build_psg([dict(name=nd['name'], matrix=list(nd['matrix']), parent=-1)],
                                   [dict(cutoff=om['lods'][0]['cutoff'], surfaces=[sf])], comp)
        self.put(gname, blob, 'A', 'converted engines (englt1 + engrt1 LOD 0, no flames) [A geometry]',
                 'one node, one LOD, one material %s as the original; each engine centred on the original engine of '
                 'the same side (x/z) and standing on its floor (lowest y), uniform scale %.3f (mean largest-side ratio) [G]' % (mat, s),
                 *([prelit] if prelit else []))
        img = self.conv_page(self.pages['E'])
        tname = 'ui' + BS + 'textures' + BS + '%se_1.pst' % self.nn
        tb, tinfo = encode_like(img, ga.get(tname))
        self.put(tname, tb, 'A', 'converted engine page %s resampled to %s [A texels, G resample]' % (self.pages['E'], tinfo['size']))
        self.facts['garage'] = dict(scale=round(s, 4), triangles=info['triangles'])

    # ---------------------------------------------------------------- rider-page character (UIRIDER ui\anim\nnc.*)
    def character(self):
        import pod_convert as PC
        import pod_export as PE
        import racer_assets as R
        kind = R.CODES.index(self.code)
        flag = 0
        pose, o, pose_name = PC.operate_pose(kind, flag)
        ps = PC.pilot_structure(o, pose)
        pparts, p, _ = PE.pilot_parts(kind, flag, np.eye(4))
        tris = []
        stats = {}
        for q in pparts:
            kk = int(q['name'].split('_n')[1].split('_')[0])
            tris += PC.mesh_tris(q, pose[kk], 'P', bone=kk, tag='pilot')
        tris = PC.orient(tris, stats)
        # two character pages, cells shared out by texel area
        base = '%sc' % self.nn
        keys = {}
        for t in tris:
            keys.setdefault(t['key'], 0)
        def area(k):
            if k[0] == 't':
                img, info = PE.texture_rgba(k[1]); return img.shape[0] * img.shape[1]
            return 64
        load = [0, 0]
        group = {}
        for k in sorted(keys, key=lambda k: -area(k)):
            g = int(load[1] < load[0]); group[k] = g; load[g] += area(k)
        atl = [PC.Atlas('%s_%d' % (base, i + 1), (256, 256)) for i in range(2)]
        for t in tris:
            atl[group[t['key']]].add(t)
        page_imgs, flags = [], []
        for i, a in enumerate(atl):
            if not a.use:
                a.use[('c', (128, 128, 128))] = [np.zeros(2), np.ones(2)]
            a.build()
            img = a.image
            if img.shape[:2] != (256, 256):
                flags.append('%s: atlas %dx%d resampled to 256x256 (UVs are page-relative)' % (a.name, img.shape[1], img.shape[0]))
                img = resize(img, 256, 256)
            flags += a.flags
            page_imgs.append(img)
        for t in tris:
            a = atl[group[t['key']]]
            t['UVp'] = a.uv(t['key'], t['UV']); t['mat'] = a.name
        # scale and placement against the original character
        om = geo_psg.parse(self.rider.get('ui' + BS + 'anim' + BS + '%s.psg' % base))
        omot = read_mot(self.rider.get('ui' + BS + 'anim' + BS + '%s_idl1.mot' % base))
        opts = posed_points(om, pose_world(om, omot, 0.0))
        o_lo, o_hi = box(opts)
        Wr = {m: PC.rv_frame(pose[m]) for m in ps['models']}
        rp = np.vstack([t['P'] for t in tris])
        r_lo, r_hi = box(rp)
        k = SEATED_HEIGHT * float(o_hi[1] - o_lo[1]) / float(r_hi[1] - r_lo[1])
        # root node: the original's resting root (idl1, first key) moved so the converted figure stands on the
        # original's floor, centred on the original's x/z
        orow0 = pod_psg.rows(om['nodes'])[0]
        root_pos = None
        for row, keys_ in omot['pos']:
            if row == 0:
                root_pos = keys_[0][1]
        oW = pose_world(om, omot, 0.0)
        o_root = oW[pod_psg.rows(om['nodes'])[0]][3, :3]
        off = np.array([(o_lo[0] + o_hi[0]) / 2 - k * (r_lo[0] + r_hi[0]) / 2,
                        o_lo[1] - k * r_lo[1],
                        (o_lo[2] + o_hi[2]) / 2 - k * (r_lo[2] + r_hi[2]) / 2])

        def scaled(W):
            W = W.copy(); W[3, :3] *= k; return W
        nodes = [dict(name='root', matrix=[1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0] + list(off) + [1.0], parent=-1)]
        index, Wn = {}, {0: np.eye(4)}
        order, todo = [], [m for m in ps['models'] if ps['parent'][m] is None]
        while todo:
            m = todo.pop(0); order.append(m)
            todo += [c for c in ps['models'] if ps['parent'][c] == m]
        for m in order:
            W = scaled(Wr[m])
            pi = 0 if ps['parent'][m] is None else index[ps['parent'][m]]
            index[m] = len(nodes); Wn[index[m]] = W
            nodes.append(dict(name=ps['names'][m], matrix=(W @ np.linalg.inv(Wn[pi])).ravel().tolist(), parent=pi))
        surfaces = []
        for pg in (atl[0].name, atl[1].name):
            sel = [t for t in tris if t['mat'] == pg]
            if not sel:
                continue
            P, N, UV, B, T = [], [], [], [], []
            for t in sel:
                b = index[t['bone']]; Wb = Wn[b]
                for j in range(3):
                    P.append((t['P'][j] * k - Wb[3, :3]) @ Wb[:3, :3].T)
                    N.append(t['N'][j] @ Wb[:3, :3].T)
                    B.append(b); UV.append(t['UVp'][j])
                T.append((len(P) - 3, len(P) - 2, len(P) - 1))
            surfaces.append(surf(pg, P, N, UV, T, B))
        comp = om['lods'][0]['surfaces'][0]['component']
        blob, info = build_psg(nodes, [dict(cutoff=om['lods'][0]['cutoff'], surfaces=surfaces)], comp & 0xFFFFFF07 if (comp & 0xFF) == 0x0F else comp)
        pre = 'ui' + BS + 'anim' + BS
        self.put(pre + '%s.psg' % base, blob, 'A',
                 'arcade pilot object (%d model nodes) in its "%s" driving pose, each segment rigid on its node [A]' % (len(ps['models']), pose_name),
                 'seated figure (the arcade has no standing pilot or legs for every pilot); height = %.2f x the original standing character, scale %.4f of the race-converted pilot [G]' % (SEATED_HEIGHT, k),
                 'node names by the pod converter role rule (head, upper_body, shoulder/forearm/hand lt/rt, pilotNN) under a root node [G]',
                 'resting place: on the original character\'s floor (lowest point of the original posed by idl1 frame 0), centred on its x/z [G]',
                 'two pages %s_1 / %s_2: arcade pilot textures packed by texel area [A texels, G layout]' % (base, base))
        for i, img in enumerate(page_imgs):
            name = 'ui' + BS + 'textures' + BS + '%s_%d.pst' % (base, i + 1)
            orig = self.rider.get(name)
            oimg, ot = pst_rgba(orig)
            alpha = int(np.median(oimg[..., 3])) if oimg is not None else character_alpha()
            img = img.copy(); img[..., 3] = alpha
            # kind-1, single mip like the decodable originals (11c, 23c, 24c); IPU originals (kind 2) are replaced by kind 1
            t0 = geo_pst.parse(orig)
            pb, pinfo = geo_pst.encode_rgba(np.ascontiguousarray(img), max_side=256, min_side=16, max_levels=1)
            self.put(name, pb, 'A', 'arcade pilot textures, atlas 256x256 kind 1 single mip (original kind %d, %d mips) [A texels, G layout]' % (t0['kind'], len(t0['levels'])),
                     'alpha %d everywhere (%s) [G]' % (alpha, 'median of the original page' if oimg is not None else 'median of the decodable original character pages'),
                     *[f for f in flags if f.startswith(atl[i].name)])
        self.char_pages = page_imgs
        # motions
        ocf = self.rider.get(pre + '%s.mcf' % base).decode('latin1')
        mcf = ini(ocf)
        rows = pod_psg.rows(nodes)
        row_of = {ni: r for r, ni in enumerate(rows)}
        mot_ids = {}
        for mid in p['motions']:
            if mid == 0xFFFFFFFF:
                continue
            nm = R.motion(mid)['name'] or ''
            for suf, key in MOTION_MAP.items():
                if key in nm and suf not in mot_ids:
                    mot_ids[suf] = mid
        new_mcf = ocf
        mfacts = {}
        tpos = [m for m in order if ps['parent'][m] is None]
        for sec, kv in mcf.items():
            if not sec.upper().startswith('MOTION'):
                continue
            mname = kv['NAME']
            suf = mname.split('_', 1)[1]
            md = PE.motion_data(mot_ids[suf])
            F = md['frames']
            if F <= 1:   # held pose: keep the original motion's length
                nf = int(kv['FrameCount'])
            else:
                nf = max(2, int(round((F - 1) / ARCADE_FPS * UI_FPS)) + 1)
            dur = (nf - 1) / UI_FPS
            rot = {m: [] for m in order}
            pos = {m: [] for m in tpos}
            for i in range(nf):
                tt = i / UI_FPS
                f = min(F - 1, int(round(tt * ARCADE_FPS)))
                Wf = {}
                for m in order:
                    if m in md['nodes']:
                        A = PE.key_row(md['keys'][f, md['nodes'].index(m)]) @ PE.key_row(md['root'][f])
                    else:
                        A = pose[m]
                    Wf[m] = scaled(PC.rv_frame(A))
                for m in order:
                    par = ps['parent'][m]
                    Lm = Wf[m] @ np.linalg.inv(Wf[par]) if par is not None else Wf[m]
                    rot[m].append((min(tt, dur), Lm[:3, :3]))
                    if m in pos:
                        pos[m].append((min(tt, dur), Lm[3, :3]))
            rot_tracks = [(row_of[index[m]], rot[m]) for m in order]
            pos_tracks = [(row_of[0], [(0.0, off), (dur, off)])] + [(row_of[index[m]], pos[m]) for m in tpos]
            mb = write_mot_short(nf, dur, sorted(pos_tracks), sorted(rot_tracks))
            self.put(pre + '%s.mot' % mname, mb, 'A',
                     'arcade motion %s (%d frames at %g fps assumed) resampled to %d frames at 15 fps [A]' % (md['name'], F, ARCADE_FPS, nf) if F > 1
                     else 'arcade "%s" pose held for the original length (%d frames) [A pose, G timing]' % (md['name'], nf),
                     'ShortAxis rotation keys on every pilot node; positions on the root (resting place) and the top pilot nodes [G encoding]')
            new_mcf = re.sub(r'(\[%s\][^\[]*?FrameCount\s*=\s*)\d+' % re.escape(sec), lambda mm: mm.group(1) + str(nf), new_mcf, count=1, flags=re.S)
            mfacts[suf] = dict(arcade=md['name'], frames=nf)
        self.put(pre + '%s.mcf' % base, new_mcf.encode('latin1'), 'G', 'original MCF, FrameCount per motion set to the converted motion [G]')
        tset = {0} | {index[m] for m in tpos}
        vut = ''.join('%s=%s%s\r\n' % (n['name'], n['name'], ',T' if i in tset else '') for i, n in enumerate(nodes))
        self.put(pre + '%s.vut' % base, vut.encode('ascii'), 'G', 'identity rename list of the converted character nodes (,T on nodes with position tracks) [G]')
        self.facts['character'] = dict(nodes=len(nodes), scale=round(k, 5), motions=mfacts, triangles=info['triangles'])
        self.char_nodes = nodes

    # ---------------------------------------------------------------- flags / backdrop
    def flags(self):
        hull = self.conv_page(self.pages['C'])[..., :3]
        eng = self.conv_page(self.pages['E'])[..., :3]
        crop, where = livery_crop(hull)
        for name, ar in (('ui' + BS + 'textures' + BS + '%s_flrg.pst' % self.nn, self.rider),
                         ('ui' + BS + 'textures' + BS + '%s_fsm.pst' % self.nn, arch(r'UI\TEXTURES\OVERLAYS.RES'))):
            orig = ar.get(name)
            oimg, _ = pst_rgba(orig)
            a = oimg[..., 3]
            ys, xs = np.nonzero(a > 0)
            y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
            img = oimg.copy()
            img[y0:y1, x0:x1, :3] = np.array(Image.fromarray(crop).resize((x1 - x0, y1 - y0), Image.LANCZOS))
            img[..., 3] = a
            blob, info = encode_like(img, orig)
            self.put(name, blob, 'G', 'rider flag: the most colourful square of the converted hull page %s (%s) [A texels, G choice]' % (self.pages['C'], where),
                     'original alpha (flag shape) kept [D]')
        # backdrop: original pattern recoloured with the converted pod's two main colours
        ba = arch(r'UI\UIBKG.RES')
        name = 'ui' + BS + 'textures' + BS + '%s_bkg.pst' % self.nn
        orig = ba.get(name)
        oimg, _ = pst_rgba(orig)
        c1, c2 = main_colours(np.vstack([hull.reshape(-1, 3), eng.reshape(-1, 3)]))
        img = recolour(oimg, c1, c2)
        blob, info = encode_like(img, orig)
        self.put(name, blob, 'G', 'original backdrop pattern [D] recoloured top -> bottom with the converted pod\'s two main colours %s, %s, '
                 'original saturation and brightness per row kept [G]' % (tuple(int(x) for x in c1), tuple(int(x) for x in c2)))

    # ---------------------------------------------------------------- PHY for the stats page
    def phy(self):
        a = arch(r'UI\PODPHYS.RES')
        name = [n for n, lo, sz in a.r.members if n.lower() == ('pods' + BS + 'pod%s' % self.nn + BS + 'pod%s.phy' % self.nn)][0]
        new = self.cget('pods' + BS + 'pod%s' % self.nn + BS + 'pod%s.phy' % self.nn)
        race = self.orig_race[low('pods' + BS + 'pod%s' % self.nn + BS + 'pod%s.phy' % self.nn)]
        same = a.get(name) == race
        self.facts['podphys_equals_race_phy'] = same
        self.put(name, new, 'D', 'PHY of the converted pod archive (byte copy): Name = %s; stats (Base/MaxAbilities = rider-page bars) still the target\'s [D]' % self.name.replace('"', "'"),
                 'original PODPHYS member %s the original race PHY' % ('equals' if same else 'DIFFERS FROM'))


# ============================================================ data rules shared by every pod
_QUADS = {}


def quad_of(nn):
    if nn not in _QUADS:
        r = arch(r'UI\UIRIDER\UIRIDER.RES')
        om, osl = model_tris(r.get('ui' + BS + 'geometry' + BS + '%s.psg' % nn))
        q = [(P, N, UV, T) for mt, P, N, UV, T in osl if mt.lower() == '%s_shdw' % nn and len(P) == 4]
        body = np.vstack([P for mt, P, N, UV, T in osl if mt.lower() not in ('%s_shdw' % nn, 'cable')])
        _QUADS[nn] = (q[0] if q else None, box(body))
    return _QUADS[nn][0]


def margin_ratios():
    """Median over the original quad shadows of (shadow half-extent / body half-extent) in x and z."""
    rx, rz = [], []
    for nn in ALL_PODS:
        q = quad_of(nn)
        if q is None:
            continue
        P = q[0]
        b = _QUADS[nn][1]
        rx.append((P[:, 0].max() - P[:, 0].min()) / (b[1][0] - b[0][0]))
        rz.append((P[:, 2].max() - P[:, 2].min()) / (b[1][2] - b[0][2]))
    return np.array([np.median(rx), 1.0, np.median(rz)])


_CHAR_ALPHA = None


def character_alpha():
    global _CHAR_ALPHA
    if _CHAR_ALPHA is None:
        r = arch(r'UI\UIRIDER\UIRIDER.RES')
        vals = []
        for p in ALL_PODS:
            for i in (1, 2):
                n = 'ui' + BS + 'textures' + BS + '%sc_%d.pst' % (p, i)
                if r.has(n):
                    img, t = pst_rgba(r.get(n))
                    if img is not None:
                        vals.append(np.median(img[..., 3]))
        _CHAR_ALPHA = int(np.median(vals))
    return _CHAR_ALPHA


def tube(a, b, radius, sag, sides=6, segs=16):
    a, b = np.asarray(a, float), np.asarray(b, float)
    d = b - a
    L = np.linalg.norm(d)
    ax = d / L
    up = np.array([0.0, 1.0, 0.0])
    u = np.cross(ax, up)
    if np.linalg.norm(u) < 1e-6:
        u = np.cross(ax, [1.0, 0, 0])
    u /= np.linalg.norm(u)
    v = np.cross(u, ax)
    P, N, UV = [], [], []
    for i in range(segs + 1):
        t = i / segs
        c = a + d * t - up * sag * 4 * t * (1 - t)
        for j in range(sides + 1):
            ang = 2 * math.pi * j / sides
            n = math.cos(ang) * u + math.sin(ang) * v
            P.append(c + n * radius); N.append(n); UV.append((j / sides, t * 4))
    T = []
    for i in range(segs):
        for j in range(sides):
            p0 = i * (sides + 1) + j
            p1, p2, p3 = p0 + 1, p0 + sides + 1, p0 + sides + 2
            for tri in ((p0, p2, p1), (p1, p2, p3)):
                A, B, C = (np.array(P[k]) for k in tri)
                if np.cross(B - A, C - A) @ (np.array(N[tri[0]]) + N[tri[1]] + N[tri[2]]) < 0:
                    tri = (tri[0], tri[2], tri[1])
                T.append(tri)
    return np.array(P), np.array(N), np.array(UV), np.array(T)


def livery_crop(rgb):
    """Square window (1/4 of the page side) with the highest mean saturation x colour spread."""
    h, w = rgb.shape[:2]
    s = max(16, min(h, w) // 4)
    sat = saturation(rgb)
    lum = rgb.astype(float).mean(-1)
    best = None
    for y in range(0, h - s + 1, max(1, s // 8)):
        for x in range(0, w - s + 1, max(1, s // 8)):
            sc = sat[y:y + s, x:x + s].mean() * (lum[y:y + s, x:x + s].std() + 8) * (lum[y:y + s, x:x + s].mean() > 30)
            if best is None or sc > best[0]:
                best = (sc, x, y)
    _, x, y = best
    return np.ascontiguousarray(rgb[y:y + s, x:x + s]), 'window %d,%d size %d' % (x, y, s)


def main_colours(px):
    """Two most frequent saturated colours (4-colour median cut of the saturated pixels)."""
    px = px.astype(np.uint8)
    sat = saturation(px)
    sel = px[(sat > 0.25) & (px.max(-1) > 50)]
    if len(sel) < 64:
        sel = px
    im = Image.fromarray(sel.reshape(1, -1, 3)).quantize(4, method=Image.MEDIANCUT)
    pal = np.array(im.getpalette()[:12]).reshape(4, 3)
    cnt = np.bincount(np.asarray(im).ravel(), minlength=4)
    order = np.argsort(-cnt)
    return pal[order[0]].astype(float), pal[order[1]].astype(float)


def recolour(oimg, c1, c2):
    rgb = oimg[..., :3].astype(float)
    H = rgb.shape[0]
    lum = rgb.mean(-1)
    out = np.empty_like(rgb)
    for y in range(H):
        row = rgb[y]
        rl = max(lum[y].mean(), 1e-3)
        rs = saturation(row.mean(0)[None])[0]
        t = y / (H - 1)
        c = c1 * (1 - t) + c2 * t
        g = c.mean()
        c = g + (c - g) * min(1.0, rs / max(saturation(c[None])[0], 1e-3))   # keep the original row saturation
        c = c / max(c.mean(), 1e-3)
        out[y] = lum[y][:, None] * c[None, :]
    img = oimg.copy()
    img[..., :3] = np.clip(out, 0, 255).astype(np.uint8)
    return img


# ============================================================ LOC
# Every rider name is drawn with BOTH the English font and the Huttese font (font\swhutt44.cel: space,
# ( ) and A-Z only). The port refuses a glyph the font lacks (fix13-ui3-11: '"' of ARK "BUMPY" ROOSE,
# rider text glyph error, ascii 34). Rule for every name: quote pairs become ( ), any other character
# outside the Huttese set is dropped [G].
HUTTESE_GLYPHS = set(' ()ABCDEFGHIJKLMNOPQRSTUVWXYZ')


def menu_name(name):
    out, open_q = [], True
    for ch in name.upper():
        if ch == '"':
            out.append('(' if open_q else ')'); open_q = not open_q
        elif ch in HUTTESE_GLYPHS:
            out.append(ch)
    return ' '.join(''.join(out).split())


def loc_names(pods):
    """STR_RIDER_SELECTION_PAGE_* key per pod: rider.dat Control (PodIndex) -> WidgetID2 (Name text) ->
    Widgets.Dat TextID. Data, no pod table."""
    r = arch(r'UI\UIRIDER\UIRIDER.RES')
    rd = ini(r.get('UI' + BS + 'UIRider' + BS + 'rider.dat').decode('latin1'))
    wd = ini(r.get('UI' + BS + 'UIRider' + BS + 'Widgets.Dat').decode('latin1'))
    out = {}
    for sec, kv in rd.items():
        if sec.startswith('Control') and 'PodIndex' in kv:
            nn = '%02d' % int(kv['PodIndex'])
            if nn in pods:
                w = wd['Widget %s' % kv['WidgetID2'].split()[0]]
                out[nn] = w['TextID']
    return out


def loc(pods_names):
    """pods_names: {nn: UPPERCASE name}. Returns {member: bytes} for LOC.RES and facts."""
    la = arch('LOC.RES')
    keys_by_pod = loc_names(set(pods_names))
    loc_blob = la.get('loc' + BS + 'english' + BS + 'UI.loc')
    first, table = struct.unpack_from('<II', loc_blob, 4)
    count = struct.unpack_from('<H', loc_blob, 26)[0]
    names = loc_blob[first:table - 1].split(b'\0')[:count]
    idx = {names[i].decode('ascii'): i for i in range(count)}
    out, facts = {}, {}
    texts = {}
    for d, L in LANGS:
        m = 'loc' + BS + d + BS + 'L_UI_%s.bin' % L
        s = la.get(m).split(b'\0')
        assert len(s) >= count
        for nn, key in keys_by_pod.items():
            old = s[idx[key]]
            s[idx[key]] = pods_names[nn].encode('latin1')
            facts.setdefault(nn, dict(key=key, index=idx[key]))['old_' + L] = old.decode('latin1')
        texts[L] = s
        out[la.by[low(m)][0]] = b'\0'.join(s)
    new_loc = bytearray(loc_blob)
    for nn, key in keys_by_pod.items():
        i = idx[key]
        mx = max(len(texts[L][i]) for _, L in LANGS)
        old = struct.unpack_from('<I', loc_blob, table + 8 * i + 4)[0]
        struct.pack_into('<I', new_loc, table + 8 * i + 4, mx)
        facts[nn]['max_length'] = [old, mx]
    for d, L in LANGS:
        m = 'loc' + BS + d + BS + 'UI.loc'
        assert la.get(m) == loc_blob, 'UI.loc differs between languages'
        out[la.by[low(m)][0]] = bytes(new_loc)
    return out, facts


# ============================================================ write
def write_all(pods):
    os.makedirs(OUT, exist_ok=True)
    repl = {}           # lower member name -> (bytes, src, flags, pod)
    facts = {}
    previews = {}
    for code, nn in pods:
        u = UiPod(code, nn)
        u.podtex()
        previews[nn] = dict(shadow=u.ui_pod())
        u.garage()
        u.character()
        u.flags()
        u.phy()
        for k, v in u.out.items():
            repl[k] = v + (nn,)
        facts[nn] = dict(arcade=code, pilot=u.name, menu_name=u.ui_name, **u.facts)
        print(nn, code, 'members', len(u.out))
    lmem, lfacts = loc({nn: menu_name(PILOT_NAMES[c]) for c, nn in pods})
    for k, b in lmem.items():
        repl[low(k)] = (b, 'G', ['%s: rider names replaced (%s) [A names]' % (k, ', '.join('%s=%s' % (nn, menu_name(PILOT_NAMES[c])) for c, nn in pods)),
                                 'UI.loc: only the max-length table entries of those strings changed (max over the five languages); header word +16 kept'], 'LOC')
    for nn, f in lfacts.items():
        facts[nn]['loc'] = f
    rows, man = [], []
    used = set()
    for rel in ARCHIVES:
        a = arch(rel)
        files, changed = [], []
        for n, lo, sz in a.r.members:
            k = low(n)
            if k in repl:
                b, src, fl, pod = repl[k]
                files.append((n, b)); changed.append((n, src, fl, pod)); used.add(k)
            else:
                files.append((n, a.r.payload[lo:lo + sz]))
        if not changed:
            continue
        new = trk_res3.write_res3(files)
        p = os.path.join(OUT, rel)
        os.makedirs(os.path.dirname(p) or OUT, exist_ok=True)
        open(p, 'wb').write(new)
        rr = trk_res3.read_res3(new)
        assert [m[0] for m in rr.members] == [m[0] for m in a.r.members]
        members = []
        for (n, lo, sz), (_, b) in zip(rr.members, files):
            assert rr.payload[lo:lo + sz] == b
            ch = [c for c in changed if c[0] == n]
            members.append(dict(name=n, offset=lo, bytes=sz, sha256=sha(b), changed=bool(ch),
                                source=ch[0][1] if ch else 'original'))
            if ch:
                mp = os.path.join(OUT, 'members', rel.replace(BS, '_'), *n.split(BS))
                os.makedirs(os.path.dirname(mp), exist_ok=True)
                open(mp, 'wb').write(b)
                man.append(dict(archive=rel.replace(BS, '/'), name=n, pod=ch[0][3], source=ch[0][1], flags=ch[0][2],
                                bytes=sz, sha256=sha(b), original_bytes=len(a.get(n)), original_sha256=sha(a.get(n))))
        row = dict(path=rel.replace(BS, '/'), bytes=len(new), sha256=sha(new),
                   replaces=dict(bytes=len(a.raw), sha256=sha(a.raw)),
                   kind='ui', pods=sorted({c[3] for c in changed}),
                   facts=dict(members=len(members), changed_members=len(changed), decoded_bytes=len(rr.payload)),
                   members=members)
        rows.append(row)
        fp = os.path.join(OUT, 'fragments', rel.replace(BS, '_') + '.json')
        os.makedirs(os.path.dirname(fp), exist_ok=True)
        json.dump(dict(schema='a2r-converted-content/1', files=[row]), open(fp, 'w'), indent=1)
        print('%-28s %9d bytes (original %9d), %d members changed' % (rel, len(new), len(a.raw), len(changed)))
    missing = set(repl) - used
    assert not missing, 'generated members without an archive: %s' % sorted(missing)
    json.dump(dict(schema='a2r-converted-content/1', files=rows), open(os.path.join(OUT, 'converted-content.fragment.json'), 'w'), indent=1)
    json.dump(dict(schema='a2r-ui-manifest/1', converter='tools\\a2r\\ui_convert.py', pods=facts, members=man),
              open(os.path.join(OUT, 'manifest.json'), 'w'), indent=1, default=str)
    return rows, facts, previews


def main(args):
    pods = list(zip([a.upper() for a in args[0::2]], args[1::2])) if args else sorted(TARGETS.items(), key=lambda kv: kv[1])
    write_all(pods)


if __name__ == '__main__':
    main(sys.argv[1:])
