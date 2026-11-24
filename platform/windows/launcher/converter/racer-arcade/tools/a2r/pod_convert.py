r"""Arcade racer -> complete Revenge pod archive, one code path for every pod.

    python -I pod_convert.py ARK 04 [BOL 15 ...]      (default: the six new pilots, siloed numbers)
    python -I pod_convert.py [...] --dlc-out DIR      also writes the DLC replacement package DIR\arcade-pods\
                                                      (pod_silo.cmd_package -> dlc_out.py) from the pods just converted

Writes D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\pods\PODnn\:
    install\PODS\PODnn\PODnn.RES, PODnn.CAM     packed archive + camera presets
    members\...                                 every member loose (same paths as in the RES)
    manifest.json                               provenance per member (A arcade / G generated / D donor)
    converted-content.fragment.json             files[] entry (silo converted-content schema)
    verify\*.png                                converted vs target renders (same camera)

The packed archive carries exactly the target original's member list (names, order, prefix = target
number). Arcade data where it exists, generated pieces flagged, the rest borrowed from the target
unchanged and flagged D. Rules and evidence: notes\pod-convert.md.

Data differences between pods are the only branches: engine count 0 (NEV: engines cut from the
single body), arm segments 2 or 3, wreck pieces per collection read from the target PHY, motions
read from the target MCF, optional binder (cable) geometry.
"""
import hashlib
import json
import math
import os
import re
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
sys.path.insert(0, TOOLS)
import a2r_space  # noqa: E402
import geo_psg  # noqa: E402
import geo_pst  # noqa: E402
import geo_res  # noqa: E402
import pod_formats as PF  # noqa: E402
import pod_psg  # noqa: E402
import trk_res3  # noqa: E402
import pod_export as PE  # noqa: E402  (arcade tables, meshes, textures, motions)
import racer_assets as R  # noqa: E402

GAME = (__import__('os').environ['RR_CONVERT_STOCK']+'/PODS')
OUT = os.path.join(SCR, 'export', 'a2r', 'pods')
TARGETS = dict(ARK='04', BOL='15', EBE='23', ELA='24', NEV='11', RAT='17')
PILOT_NAMES = dict(ARK='Ark "Bumpy" Roose', BOL='Boles Roor', EBE='Ebe Endocott', ELA='Elan Mak',
                   NEV='Neva Kee', RAT='Ratts Tyerell')
S = a2r_space.SCALE
ZS = a2r_space.z_sign()
AXIS = np.array([S, S, S * ZS])
FLIP = np.diag([1.0, 1.0, ZS])
NONE = 0xFFFFFFFF
BORDER = 2          # texels of wrap/edge padding around every atlas cell (generated)
MAX_TILES = 4       # repeat copies per axis baked into a cell before UVs are clamped (generated)
# Chariot/engine coupling (port src\game\authored_geometry49o.cpp:36-44, original 2125D8..2126F4): the
# chariot is a second body on a fixed-length joint from the main-body origin (x 0, y 0, z = right engine
# nullcg z) to a chariot anchor 3 ft ahead of the chariot nullcg; joint length = |nullcg - main origin| - 3.
# The 23 original pods: chariot nullcg x ~0, y 0..5.15, joint length 13.14 (POD12) .. 38.31 (POD21).
# A joint much shorter than that is unstable in the port's (original) solver: NEV with 1 ft blew up
# (joint force 3e3 -> 3e6 in 20 frames, chariot speed 2900, run fix13-sc-11c).
FACE_NUDGE = 0.01   # ft, every arcade triangle along its own visible-side normals (stand-in for one-sided drawing) [G]
ANCHOR_Z = 3.0      # secondaryAnchor300 z (original constant)
# STAND-IN (switchable): the arcade draws the cockpit 6..9 ft above the engines (BOL 9, RAT 7.5, EBE 6.6,
# ARK 6, ELA 4.5). True draws it at engine height so the borrowed chase cameras (PODnn.CAM, D) do not look
# through it (runs fix13-look2-15 / -23). False = arcade layout. Physics root is unaffected.
COCKPIT_AT_ENGINE_HEIGHT = True
ROD_MIN = 13.14     # shortest joint length of the 23 originals (POD12); only NEV is shorter by itself [D-derived]
ATLAS_FACTORS = (1.0, 0.875, 0.75, 0.625, 0.5)   # arcade texel scales tried at the target page size
# See-through parts: material names are global in the original (APPSTART.RES AppStart\Mat_Stbl.txt,
# 3235 names; the port's material table refuses any other name: run fix13-look-4). The original
# see-through pod material is textures\pod_wndw (POD23: pod_wndw.psm source-alpha pass 1 + its own
# window texture pod_wndw.pst). A pod RES that ships a shared name must ship it byte-identical
# (original_standard_archive_provider50la.h:164-171), so the three members are copied from POD23 [D].
WINDOW_DONOR = '23'
WINDOW_MATERIAL = 'pod_wndw'
WINDOW_MEMBERS = (r'textures\pod_wndwGhost.psm', r'textures\pod_wndw.psm', r'textures\pod_wndw.pst')
# textures\pod_wndw (.psm/.pst) is ONE shared material of the original game: POD01, 10, 19, 21, 23 and 25
# ship it byte-identical, and the port refuses differing bytes under one name across mounted pods (run
# fix13-look4-4: "canonical racer material/texture closure rejected", POD10/POD19 in the field). So the
# arcade glass texels/alpha cannot go into pod_wndw; a pod-own see-through material needs a new global
# name (APPSTART AppStart\Mat_Stbl.txt) = an APPSTART conversion + port admission. Off until then.
WINDOW_OWN_TEXTURE = False
WINDOW_ALPHA_ONE = 128   # page alpha the port draws opaque (vehicle_window_gl50ds_storage.cpp:1024-1037)
_WINDOW_CACHE = {}


def window_members():
    """The three shared see-through members, generated ONCE for the whole converted set (every pod RES
    that ships them must carry identical bytes): pod_wndw.pst = atlas of every arcade translucent
    (poly type 6) texture of the six converted kinds with the arcade texel alpha (ARK canopy glass 17/255,
    frame and sail cut-outs 0/255) in the port's units; pod_wndw.psm = POD23's with pass 1 depth write
    off (arcade poly type 6: blended, no depth write); pod_wndwGhost.psm = POD23's. Returns
    (dict name -> bytes, atlas or None)."""
    if 'members' in _WINDOW_CACHE:
        return _WINDOW_CACHE['members'], _WINDOW_CACHE['atlas']
    dres = geo_res.read_res(os.path.join(GAME, 'POD%s' % WINDOW_DONOR, 'POD%s.RES' % WINDOW_DONOR))[0]
    atlas = Atlas('pod_wndw', (256, 128))
    for code in TARGETS:
        k = R.CODES.index(code)
        pd = R.pod(k)
        for lod in range(4):
            tg = R.texgroup(pd['tg_slot0'][0] if lod == 0 else pd['tg_other'][0])
            for q in PE.pod_parts(k, lod, tg):
                for t in mesh_tris(q, q['node'], 'C'):
                    if t['page'] == 'W' and t['key'][0] == 't':
                        atlas.add(t)
    out = {w.lower(): dres[w.lower()] for w in WINDOW_MEMBERS}
    if atlas.use and WINDOW_OWN_TEXTURE:
        img = atlas.build()
        a = img[..., 3].astype(float)
        img[..., 3] = np.round(a / 255.0 * WINDOW_ALPHA_ONE).clip(0, 255).astype(np.uint8)
        out[WINDOW_MEMBERS[2].lower()] = geo_pst.encode_rgba(img)[0]
        txt = dres[WINDOW_MEMBERS[1].lower()].decode('ascii')
        i0 = txt.index('[ShaderPass_1]'); i1 = txt.index('[ShaderPass_2]')
        p1 = re.sub(r'(?m)^(zWriteDisabled\s*=)[ ]*(?=\r?$)', lambda m: m.group(1) + ' 1', txt[i0:i1], count=1)
        assert p1 != txt[i0:i1]
        out[WINDOW_MEMBERS[1].lower()] = (txt[:i0] + p1 + txt[i1:]).encode('ascii')
    else:
        atlas = None
    _WINDOW_CACHE['members'], _WINDOW_CACHE['atlas'] = out, atlas
    return out, atlas


def sha(b):
    return hashlib.sha256(b).hexdigest()


# ============================================================ target original
class Target:
    def __init__(self, nn):
        self.nn = nn
        self.res_path = os.path.join(GAME, 'POD%s' % nn, 'POD%s.RES' % nn)
        self.cam_path = os.path.join(GAME, 'POD%s' % nn, 'POD%s.CAM' % nn)
        self.raw = open(self.res_path, 'rb').read()
        self.members, self.order, _ = geo_res.read_res(self.res_path)
        self.names = [o[0] for o in self.order]
        self.cam = open(self.cam_path, 'rb').read()

    def get(self, name):
        return self.members[name.lower()]

    def has(self, name):
        return name.lower() in self.members

    def name_of(self, lower):
        for n in self.names:
            if n.lower() == lower.lower():
                return n
        return None

    def psg(self, part):
        return geo_psg.parse(self.get(r'pods\pod%s\%s.psg' % (self.nn, part)))

    def phy_text(self):
        return self.get(r'pods\pod%s\pod%s.phy' % (self.nn, self.nn)).decode('latin1')

    def ini(self, name):
        sec, out = None, {}
        for ln in self.get(name).decode('latin1').splitlines():
            s = ln.split(';', 1)[0].strip()
            if s.startswith('['):
                sec = s.strip('[]').strip(); out.setdefault(sec, {})
            elif '=' in s and sec is not None:
                k, v = s.split('=', 1); out[sec][k.strip()] = v.strip()
        return out

    def wreck(self):
        """[Wreckage] collections and their PartK names (target PHY)."""
        sec = {}
        cur = None
        for ln in self.phy_text().splitlines():
            s = ln.split(';', 1)[0].strip()
            if s.startswith('['):
                cur = s.strip('[]').strip(); sec[cur] = []
            elif cur and re.match(r'Part\d+\s*=', s):
                sec[cur].append(s.split('=', 1)[1].strip())
        return [(c, sec.get(c, [])) for c in sec.get('Wreckage', [])]


def local_points(model, lod=0, materials=None, exclude_prefix=None):
    """Decoded vertices of one LOD in each vertex's node frame -> root-local frame (root matrix
    removed), optionally filtered by material."""
    W = pod_psg.world_matrices(model['nodes'])
    root_inv = np.linalg.inv(W[0])
    pts = []
    for s in geo_psg.vertices(model, lod):
        if materials is not None and s['material'] not in materials:
            continue
        if exclude_prefix and s['material'].lower().startswith(exclude_prefix):
            continue
        for p, b in zip(s['pos'], s['bone']):
            pts.append((np.r_[p, 1.0] @ W[b] @ root_inv)[:3])
    return np.array(pts)


def flame_table_padding(target_model, lods):
    """Flame entries the target engine's material table carries beyond the flame surfaces we copy: the stock
    table may list the flame material once per LOD even where that LOD draws no flame surface (POD12: four
    'flame12l' entries, three flame surfaces). The port's flame owner requires the stock count (4,
    src\assets\pod_flame50w.cpp:70); the missing entries are appended unreferenced, as in the target [D layout]."""
    tf = [m for m in target_model['materials'] if m.lower().startswith('flame')]
    ours = [s['material'] for L in lods for s in L['surfaces'] if s['material'].lower().startswith('flame')]
    extra = []
    for name in dict.fromkeys(tf):
        extra += [name] * max(0, tf.count(name) - ours.count(name))
    return extra


def bbox(P):
    P = np.asarray(P, float)
    return P.min(0), P.max(0)


# ============================================================ arcade side
def operate_pose(kind, flag):
    """Model-node pose matrices (arcade object space, row vectors) of the pilot's 'operate' motion
    frame 0 (the driving pose), root key applied after the node key (pods.md)."""
    p = R.pilot(kind, flag)
    o = R.object_nodes(p['object'])
    md = None
    for mid in p['motions']:
        if mid == NONE:
            continue
        m = R.motion(mid)
        if m['name'] and '_operate_' in m['name']:
            md = PE.motion_data(mid)
    pose = {}
    for k, n in enumerate(o['nodes']):
        pose[k] = np.array(n['mtx']).reshape(4, 4)
        if md is not None and k in md['nodes']:
            pose[k] = PE.key_row(md['keys'][0, md['nodes'].index(k)]) @ PE.key_row(md['root'][0])
    return pose, o, (md['name'] if md else None)


def mesh_tris(q, W, page, bone=None, tag=''):
    """Triangles of one decoded arcade model with matrix W (arcade row vectors) applied, converted to
    the Revenge frame (a2r_space scale + mirror, winding reversed). Returns list of dicts."""
    mm, tg = q['mesh'], q['texgroup']
    P = (np.c_[np.array(mm.positions), np.ones(len(mm.positions))] @ W)[:, :3] * AXIS
    Nn = (np.array(mm.normals) @ W[:3, :3])
    Nn = Nn * np.array([1, 1, ZS])
    nl = np.linalg.norm(Nn, axis=1, keepdims=True); nl[nl == 0] = 1
    Nn = Nn / nl
    out = []
    for a, b, c, mi, nocull in mm.tris:
        mesh = mm.meshes[mi]
        ti = mesh['tex']
        tid = tg[ti][0] if (ti is not None and ti < len(tg)) else None
        if tid is not None:
            img, info = PE.texture_rgba(tid)
            sc = (16.0 if info['uv_texels'] else 1.0) / np.array([info['w'], info['h']])
            UV = np.array([mm.uvs[i] for i in (a, b, c)]) * sc
            key = ('t', tid)
        else:
            d = q['mats'].get(mesh['mat'], {}).get('diffuse', (200, 200, 200))
            UV = np.full((3, 2), 0.5)
            key = ('c', tuple(int(x) for x in d))
        idx = [a, c, b] if ZS < 0 else [a, b, c]       # a2r_space.winding
        uv = UV[[0, 2, 1]] if ZS < 0 else UV
        translucent = mesh['polytype'] == 6
        valpha = float(np.mean([mm.alphas[i] for i in (a, b, c)])) if translucent else 255.0
        t = dict(P=P[idx], N=Nn[idx], UV=uv, key=key, page='W' if translucent else page, bone=bone, tag=tag,
                 translucent=translucent, valpha=valpha, two_side=bool(nocull))
        out.append(t)
        if nocull:   # both faces drawn by the arcade (Odin add_triangle): back face with reversed normals
            out.append(dict(t, P=t['P'][[0, 2, 1]], N=-t['N'][[0, 2, 1]], UV=t['UV'][[0, 2, 1]]))
    return out


def orient(tris, stats):
    """Visible side = winding; normals pointing the other way are turned (course converter rule)."""
    keep = []
    for t in tris:
        e = np.cross(t['P'][1] - t['P'][0], t['P'][2] - t['P'][0])
        if np.linalg.norm(e) < 1e-9:
            stats['degenerate_dropped'] = stats.get('degenerate_dropped', 0) + 1
            continue
        if float(e @ t['N'].sum(0)) < 0:
            t['N'] = -t['N']
            stats['normals_turned_to_winding'] = stats.get('normals_turned_to_winding', 0) + 1
        keep.append(t)
    return keep


def decompose(A):
    """Arcade row-vector pose -> (rotation rows R (det +1), scale per row s, translation t)."""
    M3 = A[:3, :3]
    s = np.linalg.norm(M3, axis=1)
    Rr = M3 / s[:, None]
    if np.linalg.det(Rr) < 0:
        s[0] = -s[0]; Rr[0] = -Rr[0]
    return Rr, s, A[3, :3]


def rv_frame(A):
    """Arcade pose (cockpit frame) -> Revenge node world matrix (chariot-local), rotation only."""
    Rr, s, t = decompose(A)
    W = np.eye(4)
    W[:3, :3] = FLIP @ Rr @ FLIP
    W[3, :3] = t * AXIS
    return W


# ============================================================ pilot skeleton -> chariot nodes
def pilot_structure(o, pose):
    """Role mapping from the arcade skeleton (data, every pilot): model nodes become chariot nodes.
    joint_of(m) = the node's parent joint; parent(m) = model owned by the nearest ancestor joint that
    owns exactly one model; head = central model with the deepest joint; torso = parent of the head;
    arms = lateral models whose lowest common ancestor with the head is deepest; per side the arm
    chain sorted by depth = upper arm, forearm[, hand]; a 2-segment arm gets its hand node at the
    forearm's end joint."""
    nodes = o['nodes']
    par = [(-1 if n['raw'][1] == NONE else int(n['raw'][1])) for n in nodes]
    models = [k for k, n in enumerate(nodes) if n['model'] != NONE]
    joint = {m: par[m] for m in models}
    owner = {}
    for m in models:
        owner.setdefault(joint[m], []).append(m)

    def anc(j):
        out = []
        while j >= 0:
            out.append(j); j = par[j]
        return out
    bindT = {k: np.array(n['mtx'])[12:15] for k, n in enumerate(nodes)}
    central = [m for m in models if abs(bindT[joint[m]][0]) < 1e-4]
    head = max(central, key=lambda m: len(anc(joint[m])))
    parent = {}
    for m in models:
        parent[m] = None
        for a in anc(par[joint[m]]):
            if len(owner.get(a, [])) == 1 and owner[a][0] != m:
                parent[m] = owner[a][0]
                break
    torso = parent[head]
    hanc = set(anc(joint[head]))
    lateral = [m for m in models if abs(bindT[joint[m]][0]) >= 1e-4]

    def lca_depth(m):
        for a in anc(joint[m]):
            if a in hanc:
                return len(anc(a))
        return 0
    best = max(lca_depth(m) for m in lateral) if lateral else 0
    arms = [m for m in lateral if lca_depth(m) == best]
    sides = {}
    for side, sgn in (('lt', -1), ('rt', 1)):
        chain = sorted([m for m in arms if np.sign(bindT[joint[m]][0]) == sgn], key=lambda m: len(anc(joint[m])))
        sides[side] = chain
    names = {m: 'pilot%02d' % m for m in models}
    names[head] = 'head'
    if torso is not None:
        names[torso] = 'upper_body'
    hands = {}
    for side, chain in sides.items():
        if len(chain) < 2:
            raise ValueError('arm chain %s has %d segments' % (side, len(chain)))
        names[chain[0]] = 'shoulder' + side
        names[chain[1]] = 'forearm' + side
        if len(chain) >= 3:
            names[chain[2]] = 'hand' + side
            hands[side] = dict(model=chain[2], end=None)
        else:
            fj = joint[chain[1]]
            kids = [k for k in range(len(nodes)) if par[k] == fj and nodes[k]['model'] == NONE]
            end = max(kids, key=lambda k: np.linalg.norm(bindT[k] - bindT[fj]))
            A = np.array(nodes[end]['mtx']).reshape(4, 4) @ np.linalg.inv(np.array(nodes[chain[1]]['mtx']).reshape(4, 4)) @ pose[chain[1]]
            hands[side] = dict(model=None, end=end, pose=A, parent=chain[1])
    return dict(models=models, parent=parent, names=names, head=head, torso=torso, arms=sides, hands=hands)


# ============================================================ atlas
class Atlas:
    def __init__(self, name, size):
        self.name, self.size = name, size
        self.use = {}                      # key -> [lo, hi] UV extent

    def add(self, t):
        lo, hi = t['UV'].min(0), t['UV'].max(0)
        e = self.use.setdefault(t['key'], [lo.copy(), hi.copy()])
        e[0] = np.minimum(e[0], lo); e[1] = np.maximum(e[1], hi)

    def build(self):
        cells, self.place, flags = [], {}, []
        for key, (lo, hi) in sorted(self.use.items(), key=lambda kv: str(kv[0])):
            if key[0] == 't':
                img, info = PE.texture_rgba(key[1])
                rep = (info['repeat_u'], info['repeat_v']); mir = (info['mirror_u'], info['mirror_v'])
            else:
                img = np.zeros((8, 8, 4), np.uint8); img[..., :3] = key[1]; img[..., 3] = 255
                info = None; rep = (0, 0); mir = (0, 0)
            th, tw = img.shape[:2]
            i0, n = [0, 0], [1, 1]
            for ax in (0, 1):
                if rep[ax]:
                    a = math.floor(lo[ax] + 1e-4); b = math.ceil(hi[ax] - 1e-4)
                    i0[ax], n[ax] = a, max(1, b - a)
                    if n[ax] > MAX_TILES:
                        flags.append('%s: %s repeats %d times along %s, UVs clamped to %d copies' % (self.name, key, n[ax], 'uv'[ax], MAX_TILES))
                        n[ax] = MAX_TILES
            rows = []
            for j in range(n[1]):
                row = []
                for i in range(n[0]):
                    tile = img
                    if mir[0] and (i0[0] + i) % 2:
                        tile = tile[:, ::-1]
                    if mir[1] and (i0[1] + j) % 2:
                        tile = tile[::-1]
                    row.append(tile)
                rows.append(np.concatenate(row, 1))
            tiled = np.concatenate(rows, 0)
            mode = 'wrap' if (rep[0] or rep[1]) else 'edge'
            cell = np.pad(tiled, ((BORDER, BORDER), (BORDER, BORDER), (0, 0)), mode=mode)
            cells.append((key, cell, (tw, th), i0, n, rep))
        W, H = self.size
        # candidates in order: the target page size (64 MB budget: never grow a page while the arcade
        # texels still fit at >= half scale), arcade texels scaled down in steps, with border then
        # without; only then larger pages at full scale (each step flagged)
        cands = []
        for factor in ATLAS_FACTORS:
            for b in (BORDER, 0):
                cands.append((factor, W, H, b))
        for (pw, ph) in [(W, 2 * H), (2 * W, H), (2 * W, 2 * H)]:
            if pw <= 512 and ph <= 512:
                for b in (BORDER, 0):
                    cands.append((1.0, pw, ph, b))
        for factor, pw, ph, b in cands:
            sizes = [(int(math.ceil((c[1].shape[1] - 2 * BORDER) * factor)) + 2 * b,
                      int(math.ceil((c[1].shape[0] - 2 * BORDER) * factor)) + 2 * b) for c in cells]
            pos = PF.pack(sizes, pw, ph)
            if pos is not None:
                break
        if b != BORDER:
            flags.append('%s: cells packed without the %d-texel padding to fit' % (self.name, BORDER))
        page = np.zeros((ph, pw, 4), np.uint8)
        page[..., 3] = 255
        from PIL import Image
        for (key, cell, (tw, th), i0, n, rep), (x, y), (cw, ch) in zip(cells, pos, sizes):
            inner = cell[BORDER:-BORDER, BORDER:-BORDER]
            if inner.shape[1] != cw - 2 * b or inner.shape[0] != ch - 2 * b:
                inner = np.array(Image.fromarray(inner).resize((cw - 2 * b, ch - 2 * b), Image.BOX))
            cell = np.pad(inner, ((b, b), (b, b), (0, 0)), mode='wrap' if (rep[0] or rep[1]) else 'edge') if b else inner
            page[y:y + ch, x:x + cw] = cell
            self.place[key] = dict(x=x, y=y, w=cw, h=ch, tw=tw * factor, th=th * factor, i0=i0, n=n, rep=rep, b=b)
        if (pw, ph) != (W, H):
            flags.append('%s: page %dx%d larger than the target page %dx%d' % (self.name, pw, ph, W, H))
        if factor != 1.0:
            flags.append('%s: arcade texels scaled by %g to fit' % (self.name, factor))
        self.image, self.page_size, self.factor, self.flags = page, (pw, ph), factor, flags
        return page

    def uv(self, key, UV):
        p = self.place[key]
        pw, ph = self.page_size
        out = np.empty_like(UV)
        for ax, (o, tsz, full) in enumerate(((p['x'], p['tw'], pw), (p['y'], p['th'], ph))):
            u = UV[:, ax].copy()
            if p['rep'][ax]:
                u = np.clip(u - p['i0'][ax], 0, p['n'][ax])
            else:
                u = np.clip(u, 0, 1)
            out[:, ax] = (o + p['b'] + u * tsz) / full
        return out


# ============================================================ converter
class Pod:
    def __init__(self, code, nn):
        self.code, self.nn = code, nn
        self.kind = R.CODES.index(code)
        self.T = Target(nn)
        self.out = os.path.join(OUT, 'POD%s' % nn)
        self.flags = {}          # member lower name -> list of flags
        self.source = {}         # member lower name -> A/G/D
        self.stats = {}
        self.notes = []

    def mark(self, name, src, *flags):
        self.source[name.lower()] = src
        self.flags.setdefault(name.lower(), []).extend(flags)

    # ---------------------------------------------------------------- arcade geometry
    def gather(self):
        k = self.kind
        pd = R.pod(k)
        pl = PE.npc_placement(k)
        self.pl = pl
        body = np.array(pl['body']['offset'])
        for e in pl['engine']:
            assert abs(e['roll_deg']) < 1e-6, 'engine roll: add rotation to the engine root matrix'
        self.lods = []           # per LOD: dict(cockpit=[tris], engines=[[tris] per engine], pilot=[tris])
        self.binder = []
        st = self.stats.setdefault('geometry', {})
        for lod in range(4):
            tg = R.texgroup(pd['tg_slot0'][0] if lod == 0 else pd['tg_other'][0])
            parts = PE.pod_parts(k, lod, tg)
            L = dict(cockpit=[], engines=[[] for _ in range(pd['engines'])], pilot=[])
            for q in parts:
                grp = q['name'].split('_')[0]
                if grp == 'cockpit':
                    L['cockpit'] += mesh_tris(q, q['node'], 'C', tag='cockpit')
                elif grp == 'binder':
                    if lod == 0:
                        P = (np.c_[np.array(q['mesh'].positions), np.ones(len(q['mesh'].positions))] @ q['node'])[:, :3]
                        self.binder.append(P)
                else:
                    i = int(grp[len('engine'):])
                    L['engines'][i] += mesh_tris(q, q['node'], 'E', tag='engine')
            self.lods.append(L)
        # pilot: drawn by the arcade for LOD 0 (player pilot table) and LOD 1 (other slots)
        self.pose, self.pobj, self.pose_name = operate_pose(k, 0)
        self.ps = pilot_structure(self.pobj, self.pose)
        for lod, flag in ((0, 0), (1, 1)):
            pose, o, _ = operate_pose(k, flag)
            pparts, p, _ = PE.pilot_parts(k, flag, np.eye(4))
            for q in pparts:
                kk = int(q['name'].split('_n')[1].split('_')[0])
                self.lods[lod]['pilot'] += mesh_tris(q, pose[kk], 'P', bone=kk, tag='pilot')
        for L in self.lods:
            for key in ('cockpit', 'pilot'):
                L[key] = orient(L[key], st)
            L['engines'] = [orient(e, st) for e in L['engines']]
        # The arcade draws every pod triangle one-sided (notes\pod-render.md 2); nocull faces arrive as a front
        # copy and a reversed back copy. Most arcade pods also model shells twice, back to back, in the same
        # place (e.g. ARK's ring tunnel lining, 32 tris). The port draws pods two-sided, so coincident copies
        # fight (hatching inside ARK's rings). STAND-IN for one-sided drawing: every triangle (both copies
        # kept, open shells keep their back faces) moves FACE_NUDGE along its own visible-side vertex
        # normals, so the copy facing the camera is always nearer [G].
        n_off = 0
        for L in self.lods:
            for t in L['cockpit'] + L['pilot'] + [t for e in L['engines'] for t in e]:
                n = t['N'] / np.maximum(np.linalg.norm(t['N'], axis=1, keepdims=True), 1e-12)
                t['P'] = t['P'] + n * FACE_NUDGE
                n_off += 1
        st['face_nudge_tris'] = n_off
        # Revenge-frame part roots (before the pod origin shift): chariot at the arcade body offset
        self.root_c = body * AXIS
        # data: arcade engine count 0 = engines built into the body = one rigid unit (no binder band,
        # no cables, engines and cockpit do not move against each other) -> rigid_unit_phy() [G]
        self.rigid_unit = pd['engines'] == 0
        if pd['engines'] >= 2:
            self.root_e = [np.array(pl['engine'][i]['offset']) * AXIS for i in range(2)]
            for L in self.lods:     # engine tris are already engine-local; cockpit tris cockpit-local
                pass
            self.engine_source = 'arcade engine objects, NPC placement table 0C2F904C [A]'
        else:
            self.split_engines()
        if pd['engines'] > 2:
            raise ValueError('four-engine arcade pod: needs the englt2/engrt2 path')

    def split_engines(self):
        """Arcade kind with no separate engines (data: engine count 0, NEV): cut the side nacelles
        out of the single body by lateral position [G]. Threshold = the widest empty gap in the
        |x| distribution of LOD-0 triangle centroids between the centre line and the outer edge."""
        L0 = self.lods[0]['cockpit']
        cx = np.array([abs(t['P'][:, 0].mean()) for t in L0])
        xs = np.sort(cx)
        hi = xs.max()
        lo_lim = 0.15 * hi
        cand = [(xs[i + 1] - xs[i], (xs[i + 1] + xs[i]) / 2) for i in range(len(xs) - 1) if lo_lim < xs[i] < 0.9 * hi]
        gap, thr = max(cand)
        self.split_threshold_ft = float(thr)
        self.root_e = []
        for L in self.lods:
            eng = [[], []]
            keep = []
            for t in L['cockpit']:
                x = t['P'][:, 0].mean()
                if abs(x) > thr:
                    eng[0 if x < 0 else 1].append(dict(t, page='E', tag='engine'))
                else:
                    keep.append(t)
            L['cockpit'] = keep
            L['engines'] = eng
        # engine roots: box centre of each LOD-0 nacelle (cockpit-local Revenge frame) -> pod frame
        for i in range(2):
            P = np.vstack([t['P'] for t in self.lods[0]['engines'][i]])
            c = (P.min(0) + P.max(0)) / 2
            self.root_e.append(self.root_c + c)
            for L in self.lods:
                for t in L['engines'][i]:
                    t['P'] = t['P'] - c
        self.engine_source = ('cut from the single arcade body: triangles with |x| > %.2f ft (widest gap in the '
                              'centroid distribution, %.2f ft wide) [G]' % (thr, gap))
        self.notes.append('engines split from the body at |x| = %.2f ft (gap %.2f ft)' % (thr, gap))

    # ---------------------------------------------------------------- frames
    def place_origin(self):
        """Pod origin: same fraction along chariot root -> engine-pair centre as the target original
        (the borrowed PHY / CAM were authored around that origin) [G, D-derived]."""
        tc = np.array(self.T.psg('chariot')['nodes'][0]['matrix'][12:15])
        te = np.mean([np.array(self.T.psg(p)['nodes'][0]['matrix'][12:15]) for p in ('englt1', 'engrt1')], 0)
        t = (0 - tc[2]) / (te[2] - tc[2])
        me = np.mean(self.root_e, 0)
        oz = -(self.root_c[2] + t * (me[2] - self.root_c[2]))
        # height: engine nullcgs on the main-body axis (y 0), as in every original (engine y -2.8..0.3,
        # podboxm symmetric about y 0) and every arcade pod with engines (engine offset y 0). Only NEV
        # (engines cut from the body at the body height) moves: -3.63 ft [G]
        shift = np.array([0.0, -me[1], oz])
        self.root_c = self.root_c + shift
        self.root_e = [r + shift for r in self.root_e]
        self.origin = dict(target_fraction=float(t), shift=shift.tolist())
        # Chariot nullcg = the chariot body's physics root. Rule of every original pod (data, no pod
        # branch): on the main-body axis (x 0, y 0: joint straight at rest) behind the right engine nullcg, so the fixed-length
        # joint (ANCHOR_Z ahead of the nullcg) is straight and at its rest length. The arcade cockpit
        # offset (y 1.2..3 arcade units up; NEV even level with / ahead of its engines) is kept as a
        # geometry offset inside the chariot frame, so the drawn cockpit stays where the arcade puts it.
        ez = self.root_e[1][2]
        phys = np.array([0.0, 0.0, min(self.root_c[2], ez - ANCHOR_Z - ROD_MIN)])
        self.chariot_offset = self.root_c - phys          # chariot geometry offset in the chariot frame [G]
        # height: the cockpit is drawn at engine height, as every Revenge chariot (and the borrowed chase
        # cameras expect). The arcade's raised cockpits (BOL 9 ft, RAT 7.5, EBE 6.6, ARK 6 above the
        # engines) cut through the default chase camera's view (runs fix13-look2-15 / -23) [G]
        self.arcade_cockpit_height = float(self.chariot_offset[1])
        if COCKPIT_AT_ENGINE_HEIGHT:
            self.chariot_offset[1] = 0.0
            self.notes.append('STAND-IN: cockpit drawn at engine height (arcade: %.2f ft above); COCKPIT_AT_ENGINE_HEIGHT' % self.arcade_cockpit_height)
        self.cockpit_root = self.root_c
        self.root_c = phys
        L = float(np.linalg.norm(phys - np.array([0.0, 0.0, ez])))
        self.origin.update(chariot_nullcg=phys.tolist(), chariot_geometry_offset=self.chariot_offset.tolist(),
                           joint_rest_length=L - ANCHOR_Z)
        if abs(self.chariot_offset[2]) > 1e-6:
            self.notes.append('chariot nullcg moved %.2f ft back from the cockpit (cockpit level with or ahead of '
                              'the engines): joint length %.2f ft [G]' % (self.chariot_offset[2], L - ANCHOR_Z))

    # ---------------------------------------------------------------- pages
    def pages(self):
        """Arcade textures -> the target's own texture pages: engines -> the page the target engines
        use, chariot hull -> the page with most root-bound chariot vertices, pilot -> the page of the
        target pilot nodes. Other target pages stay borrowed."""
        T = self.T
        eng = T.psg('englt1')
        emat = [s['material'] for s in geo_psg.vertices(eng, 0) if not s['material'].lower().startswith('flame')]
        ch = T.psg('chariot')
        cnt = {}
        for s in geo_psg.vertices(ch, 0):
            nroot = sum(1 for b in s['bone'] if b == 0)
            cnt.setdefault(s['material'], [0, 0])
            cnt[s['material']][0] += nroot
            cnt[s['material']][1] += len(s['bone']) - nroot
        cmat = max(cnt, key=lambda m: cnt[m][0])
        pmat = max((m for m in cnt if m != cmat), key=lambda m: cnt[m][1])
        self.page_mat = dict(E=emat[0], C=cmat, P=pmat)

        def size(m):
            t = geo_pst.parse(T.get('textures\\%s.pst' % m))
            return (t['levels'][-1]['w'], t['levels'][-1]['h'])
        self.atlas = {k: Atlas(v, size(v)) for k, v in self.page_mat.items()}
        for L in self.lods:
            for t in L['cockpit'] + L['pilot'] + [t for e in L['engines'] for t in e]:
                if t['page'] != 'W':
                    self.atlas[t['page']].add(t)
        for k, a in self.atlas.items():
            a.build()
            # pod pages: alpha is not transparency (PSM alpha blend/test off) but the specular-mask
            # level of SpecularMapDualCompressed; arcade has no such map -> the target page's median
            # alpha over its used texels, every texel [G, D-derived]
            ta = geo_pst.rgba_levels(geo_pst.parse(T.get('textures\\%s.pst' % a.name)))[-1][..., 3]
            used = ta[ta > 0]
            a.alpha = int(np.median(used)) if len(used) else 255
            a.flags.append('%s: alpha of opaque cells set to %d = median alpha (specular mask level) of the target page [G]' % (a.name, a.alpha))
            a.image[..., 3] = a.alpha
        for L in self.lods:
            for t in L['cockpit'] + L['pilot'] + [t for e in L['engines'] for t in e]:
                # see-through tris keep their arcade UVs on the window texture (pod_wndw.pst, 64x64,
                # wrap) instead of an atlas cell [G]
                t['UVp'] = (window_members()[1].uv(t['key'], t['UV']) if window_members()[1] else t['UV']) if t['page'] == 'W' else self.atlas[t['page']].uv(t['key'], t['UV'])
        self.translucent = sum(1 for t in self.lods[0]['cockpit'] + self.lods[0]['pilot'] if t['translucent'])

    # ---------------------------------------------------------------- chariot
    def chariot(self):
        T = self.T
        tch = T.psg('chariot')
        ps = self.ps
        nodes = [dict(name='nullcg', matrix=list(np.r_[np.eye(4)[:3].ravel(), self.root_c, 1.0].tolist()), parent=-1)]
        nodes[0]['matrix'] = [1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0] + list(self.root_c) + [1.0]
        nodes.append(dict(name='body', matrix=pod_psg.IDENT, parent=0))
        Wn = {0: np.eye(4), 1: np.eye(4)}          # chariot-local world (root excluded)
        index = {}
        order = []
        todo = [m for m in ps['models'] if ps['parent'][m] is None]
        while todo:                                  # parents first
            m = todo.pop(0)
            order.append(m)
            todo += [c for c in ps['models'] if ps['parent'][c] == m]
        for m in order:
            W = rv_frame(self.pose[m])
            pi = 1 if ps['parent'][m] is None else index[ps['parent'][m]]
            Lm = W @ np.linalg.inv(Wn[pi])
            index[m] = len(nodes)
            Wn[index[m]] = W
            nodes.append(dict(name=ps['names'][m], matrix=Lm.ravel().tolist(), parent=pi))
        hand_node = {}
        for side in ('lt', 'rt'):
            h = ps['hands'][side]
            if h['model'] is not None:
                hand_node[side] = index[h['model']]
            else:
                W = rv_frame(h['pose'])
                pi = index[h['parent']]
                hand_node[side] = len(nodes)
                Wn[len(nodes)] = W
                nodes.append(dict(name='hand' + side, matrix=(W @ np.linalg.inv(Wn[pi])).ravel().tolist(), parent=pi))
        # throttle frames and IK targets at the arcade hands (arcade has no throttle levers) [G]
        dummies = {}
        for side, dn in (('lt', 'l_hand_dummy'), ('rt', 'r_hand_dummy')):
            Wh = Wn[hand_node[side]]
            thr = np.eye(4); thr[3, :3] = Wh[3, :3]
            nodes.append(dict(name='thr' + side, matrix=thr.ravel().tolist(), parent=0))
            Wn[len(nodes) - 1] = thr
            d = np.eye(4); d[:3, :3] = Wh[:3, :3]
            nodes.append(dict(name=dn, matrix=d.ravel().tolist(), parent=len(nodes) - 1))
            Wn[len(nodes) - 1] = d @ thr
            dummies[side] = len(nodes) - 1
        for side, nm in ((0, 'chariotcableleft1'), (1, 'chariotcableright1')):
            m = np.eye(4); m[3, :3] = self.cable_c[side]
            nodes.append(dict(name=nm, matrix=m.ravel().tolist(), parent=0))
            Wn[len(nodes) - 1] = m
        # LOD geometry in node-local frames
        lods = []
        cut = [L['cutoff'] for L in tch['lods']]
        assert len(cut) == 4
        for li in range(4):
            L = self.lods[li]
            surfs = []
            groups = []
            for page, tris, skinned in (('C', L['cockpit'], False), ('P', L['pilot'], li == 0)):
                for see in (False, True):     # see-through tris get the page's window material
                    sel = [t for t in tris if t['translucent'] == see]
                    mat = WINDOW_MATERIAL if see else self.page_mat[page]
                    groups.append((page, sel, skinned, mat))
            for page, tris, skinned, mat in groups:
                if not tris:
                    continue
                P, N, UV, B, TT = [], [], [], [], []
                for t in tris:
                    for j in range(3):
                        if skinned:
                            b = index[t['bone']]
                            Wb = Wn[b]
                            P.append((t['P'][j] - Wb[3, :3]) @ Wb[:3, :3].T)
                            N.append(t['N'][j] @ Wb[:3, :3].T)
                            B.append(b)
                        else:
                            P.append(t['P'][j]); N.append(t['N'][j]); B.append(0)
                        UV.append(t['UVp'][j])
                    TT.append((len(P) - 3, len(P) - 2, len(P) - 1))
                surfs.append(dict(material=mat, P=np.array(P), N=np.array(N), UV=np.array(UV),
                                  bone=np.array(B), tris=np.array(TT)))
            lods.append(dict(cutoff=cut[li], surfaces=surfs))
        # damage00: target's effect point, mapped from the target chariot box to ours [G]
        t_box = self.t_chariot_box
        tW = pod_psg.world_matrices(tch['nodes'])
        rinv = np.linalg.inv(tW[0])
        for i, nd in enumerate(tch['nodes']):
            if nd['name'].lower() == 'damage00':
                p = (tW[i] @ rinv)[3, :3]
                q = PF.remap(p, *t_box, *self.chariot_box)
                m = np.array(nd['matrix']).reshape(4, 4).copy(); m[3, :3] = q
                nodes.append(dict(name='damage00', matrix=m.ravel().tolist(), parent=0))
        # everything above is in the cockpit frame; the nullcg sits at the physics root (place_origin),
        # so the root's children and the root-bound vertices carry the cockpit offset [G]
        off = self.chariot_offset
        for nd in nodes[1:]:
            if nd['parent'] == 0:
                m = np.array(nd['matrix'], float).reshape(4, 4); m[3, :3] += off
                nd['matrix'] = m.ravel().tolist()
        for L in lods:
            for s in L['surfaces']:
                s['P'] = s['P'] + np.where((s['bone'] == 0)[:, None], off, 0.0)
        comp = self.component
        blob, info = pod_psg.build(nodes, lods, comp)
        self.ch_nodes = nodes
        self.ch_index = index
        self.ch_W = Wn
        info['hand_nodes'] = {s: nodes[i]['name'] for s, i in hand_node.items()}
        self.stats['chariot'] = info
        return blob

    def cable_points(self):
        """Per side: (cockpit point in chariot-local Revenge frame, engine point in engine-local frame)."""
        body = np.array(self.pl['body']['offset'])
        cc, ce, src = [None, None], [None, None], ['G', 'G']
        if self.binder and len(self.pl['engine']) == 2:
            V = np.vstack(self.binder)
            for side in (0, 1):
                eng = np.array(self.pl['engine'][side]['offset']) - body      # engine origin, cockpit frame
                sel = V[np.sign(V[:, 0]) == np.sign(eng[0])]
                if len(sel) < 2:
                    continue
                t = (sel @ eng) / (eng @ eng)
                a, b = sel[t < 0.5], sel[t >= 0.5]
                if len(a) and len(b):
                    cc[side] = a.mean(0) * AXIS
                    ce[side] = (b.mean(0) - eng) * AXIS
                    src[side] = 'A'
        self.cable_source = src
        return cc, ce

    def fill_cables_from_target(self):
        """Sides without arcade cable geometry: map the target's cable nodes from its boxes [G]."""
        tch = self.T.psg('chariot')
        tW = pod_psg.world_matrices(tch['nodes']); rinv = np.linalg.inv(tW[0])
        for side, nm in ((0, 'chariotcableleft1'), (1, 'chariotcableright1')):
            if self.cable_c[side] is None:
                i = [k for k, nd in enumerate(tch['nodes']) if nd['name'].lower() == nm][0]
                self.cable_c[side] = PF.remap((tW[i] @ rinv)[3, :3], *self.t_chariot_box, *self.chariot_box)

    # ---------------------------------------------------------------- engines
    def engine(self, side):
        part = ('englt1', 'engrt1')[side]
        T = self.T
        te = T.psg(part)
        tW = pod_psg.world_matrices(te['nodes'])
        rinv = np.linalg.inv(tW[0])
        flame = lambda m: m.lower().startswith('flame')  # noqa: E731
        t_box = bbox(local_points(te, 0, exclude_prefix='flame'))
        my_pts = np.vstack([t['P'] for t in self.lods[0]['engines'][side]])
        m_box = bbox(my_pts)
        nodes = []
        for i, nd in enumerate(te['nodes']):
            m = np.array(nd['matrix']).reshape(4, 4).copy()
            if i == 0:
                m = np.eye(4); m[3, :3] = self.root_e[side]
                nodes.append(dict(name=nd['name'], matrix=m.ravel().tolist(), parent=-1))
                continue
            Wt = tW[i] @ rinv
            p = PF.remap(Wt[3, :3], *t_box, *m_box)
            if nd['name'].lower().startswith('enginecable') and self.cable_e[side] is not None:
                p = self.cable_e[side]
            if nd['name'].lower().startswith('enginecable') and self.rigid_unit:
                # one rigid unit has no cables: the engine end sits on the engine surface point nearest the
                # chariot end, so the cable the port draws between them is as short as the arcade gap [G]
                c = self.cockpit_root + self.cable_c[side] - self.root_e[side]
                p = my_pts[np.argmin(np.linalg.norm(my_pts - c, axis=1))]
                self.flags.setdefault(self.flame_member(part), []).append(
                    'enginecable node on the engine surface %.2f ft from the chariot cable node (one rigid unit, no cables) [G]'
                    % float(np.linalg.norm(p - c)))
            # local translation relative to the parent's (target) frame
            Wp = tW[nd['parent']] @ rinv if nd['parent'] > 0 else np.eye(4)
            if nd['parent'] > 0:
                pp = PF.remap(Wp[3, :3], *t_box, *m_box)
            else:
                pp = np.zeros(3)
            m[3, :3] = (p - pp) @ np.linalg.inv(Wp[:3, :3])
            nodes.append(dict(name=nd['name'], matrix=m.ravel().tolist(), parent=nd['parent']))
        flame_scale = self.place_flames(side, te, tW, rinv, nodes)
        lods = []
        for li, TL in enumerate(te['lods']):
            surfs = []
            tris = self.lods[li]['engines'][side]
            if tris:
                P = np.vstack([t['P'] for t in tris]); N = np.vstack([t['N'] for t in tris])
                UV = np.vstack([t['UVp'] for t in tris])
                surfs.append(dict(material=self.page_mat['E'], P=P, N=N, UV=UV, bone=np.zeros(len(P), int),
                                  tris=np.arange(len(P)).reshape(-1, 3)))
            for s in geo_psg.vertices(te, li):           # flames: target geometry on the flame nodes [D],
                if not flame(s['material']):              # sized to the arcade exit (place_flames) [G]
                    continue
                k = np.array([flame_scale.get(b, 1.0) for b in s['bone']])
                surfs.append(dict(material=s['material'], P=np.array(s['pos']) * k[:, None], N=np.array(s['normal']),
                                  UV=np.array(s['uv']), bone=np.array(s['bone']), tris=np.array(s['tris'])))
            lods.append(dict(cutoff=TL['cutoff'], surfaces=surfs))
        comp = self.component
        blob, info = pod_psg.build(nodes, lods, comp, extra_materials=flame_table_padding(te, lods))
        self.stats[part] = info
        if not hasattr(self, 'eng_box'):
            self.eng_box, self.t_eng_box = {}, {}
        self.eng_box[side], self.t_eng_box[side] = m_box, t_box
        return blob

    # ---------------------------------------------------------------- engine exits / flames
    def engine_exits(self, side):
        """Exhaust exits of the converted arcade engine (engine-local frame) [G]. The arcade has no
        exhaust / flame / glow points (its pod 'light groups' are course lighting, race-data.md 20), so
        the exits are read off the engine model: flat rear-facing (normal z < -0.9) connected faces that
        are round (aspect <= 1.5), cover their own centre (a disc, not a ring around a hole) and have no
        engine geometry behind that centre (the open rear end of a tube or nozzle). Largest first."""
        tris = self.lods[0]['engines'][side]
        P = np.array([t['P'] for t in tris], float)
        n = np.cross(P[:, 1] - P[:, 0], P[:, 2] - P[:, 0])
        area = np.linalg.norm(n, axis=1) / 2
        nn = n / np.maximum(2 * area, 1e-12)[:, None]
        rear = np.where(nn[:, 2] < -0.9)[0]
        key = lambda v: tuple(np.round(v, 3))  # noqa: E731
        par = {i: i for i in rear}

        def find(i):
            while par[i] != i:
                par[i] = par[par[i]]; i = par[i]
            return i
        seen = {}
        for i in rear:
            for v in P[i]:
                k = key(v)
                if k in seen:
                    par[find(i)] = find(seen[k])
                else:
                    seen[k] = i
        groups = {}
        for i in rear:
            groups.setdefault(find(i), []).append(i)

        def cover(pt, idx):
            """Triangles (of idx) whose xy projection contains pt.xy -> their z there."""
            A, B, C = P[idx, 0], P[idx, 1], P[idx, 2]
            v0, v1 = B[:, :2] - A[:, :2], C[:, :2] - A[:, :2]
            v2 = pt[:2] - A[:, :2]
            den = v0[:, 0] * v1[:, 1] - v1[:, 0] * v0[:, 1]
            ok = np.abs(den) > 1e-12
            den = np.where(ok, den, 1.0)
            u = (v2[:, 0] * v1[:, 1] - v1[:, 0] * v2[:, 1]) / den
            w = (v0[:, 0] * v2[:, 1] - v2[:, 0] * v0[:, 1]) / den
            inside = ok & (u >= -1e-6) & (w >= -1e-6) & (u + w <= 1 + 1e-6)
            z = A[:, 2] + u * (B[:, 2] - A[:, 2]) + w * (C[:, 2] - A[:, 2])
            return z[inside]
        out = []
        allidx = np.arange(len(P))
        for g in groups.values():
            g = np.array(g)
            Q = P[g].reshape(-1, 3)
            a = float(area[g].sum())
            c = (P[g].mean(1) * area[g][:, None]).sum(0) / max(a, 1e-12)
            size = Q.max(0) - Q.min(0)
            if size[2] > max(0.25, 0.15 * size[:2].max()):
                continue                               # not flat
            if size[:2].max() > 1.5 * max(size[:2].min(), 1e-6):
                continue                               # not round
            if not len(cover(c, g)):
                continue                               # ring around a hole
            if (cover(c, allidx) < c[2] - 0.05).any():
                continue                               # something of the engine lies behind it
            out.append(dict(centre=c, area=a, radius=math.sqrt(a / math.pi), tris=len(g)))
        return sorted(out, key=lambda e: -e['area'])

    def place_flames(self, side, te, tW, rinv, nodes):
        """Target flame nodes (flame meshes and materials stay the target's [D]) moved onto the arcade
        engine's own exits [G]: the target's flame groups (flame nodes sharing an exhaust) take the
        largest exits in turn; each group is scaled by exit radius / target flame radius (0.4..2.5) and
        keeps the target's distance from its flame front to the node, with the front at the exit face.
        exhaust* nodes (smoke) go to the centre of the used exits. Returns {node index: scale}."""
        part = ('englt1', 'engrt1')[side]
        exits = self.engine_exits(side)
        fidx = [i for i, nd in enumerate(te['nodes']) if 'flame' in nd['name'].lower()]
        xidx = [i for i, nd in enumerate(te['nodes']) if nd['name'].lower().startswith('exhaust')]
        if not fidx:
            return {}
        Wt = {i: tW[i] @ rinv for i in fidx}
        verts = {i: [] for i in fidx}                  # target flame vertices, engine frame, per node
        for s in geo_psg.vertices(te, 0):
            if s['material'].lower().startswith('flame'):
                for p, b in zip(s['pos'], s['bone']):
                    if b in verts:
                        verts[b].append((np.r_[p, 1.0] @ Wt[b])[:3])
        groups = []
        for i in fidx:
            assert te['nodes'][i]['parent'] == 0
            xy = Wt[i][3, :2]
            for g in groups:
                if np.linalg.norm(g['xy'] - xy) < 0.5:
                    g['nodes'].append(i); break
            else:
                groups.append(dict(xy=xy.copy(), nodes=[i]))
        for g in groups:
            V = np.vstack([verts[i] for i in g['nodes'] if verts[i]])
            g['xy'] = np.mean([Wt[i][3, :2] for i in g['nodes']], 0)
            g['front'] = float(V[:, 2].max())
            g['radius'] = float(np.linalg.norm(V[:, :2] - g['xy'], axis=1).max())
        if not exits:
            self.flags.setdefault(self.flame_member(part), []).append(
                'no open rear exit found on the arcade engine: flame nodes keep the box-mapped target positions [G]')
            return {}
        # exits of equal size at the same depth form a set (e.g. tube ends around a nozzle); a set larger
        # than the flame groups still free becomes one ring exit around its centre (radius = ring radius
        # + exit radius), otherwise each exit of the set takes a group
        used, rest = [], list(exits)
        while rest and len(used) < len(groups):
            e0 = rest[0]
            same = [e for e in rest if abs(e['area'] - e0['area']) <= 0.1 * e0['area'] and abs(e['centre'][2] - e0['centre'][2]) <= 0.1]
            rest = [e for e in rest if not any(e is s for s in same)]
            if len(same) <= len(groups) - len(used):
                used += same
            else:
                c = np.mean([e['centre'] for e in same], 0)
                ring = float(np.mean([np.linalg.norm(e['centre'][:2] - c[:2]) for e in same]))
                used.append(dict(centre=c, area=sum(e['area'] for e in same), radius=ring + e0['radius'], tris=0,
                                 ring=len(same)))
        groups.sort(key=lambda g: (round(g['xy'][0], 1), g['xy'][1]))
        order = sorted(range(len(used)), key=lambda k: (round(used[k]['centre'][0], 1), used[k]['centre'][1]))
        scale, rows = {}, []
        for gi, g in enumerate(groups):
            e = used[order[gi % len(used)]]
            s = float(np.clip(e['radius'] / max(g['radius'], 1e-6), 0.4, 2.5))
            for i in g['nodes']:
                p = Wt[i][3, :3]
                q = np.array([e['centre'][0] + (p[0] - g['xy'][0]) * s, e['centre'][1] + (p[1] - g['xy'][1]) * s,
                              e['centre'][2] + (p[2] - g['front']) * s])
                m = np.array(nodes[i]['matrix'], float).reshape(4, 4); m[3, :3] = q
                nodes[i]['matrix'] = m.ravel().tolist()
                scale[i] = s
            rows.append('%s (%.2f, %.2f, %.2f) r %.2f ft <- target flame group of %d nodes r %.2f ft, scale %.2f' % (
                'ring of %d exits' % e['ring'] if e.get('ring') else 'exit',
                e['centre'][0], e['centre'][1], e['centre'][2], e['radius'], len(g['nodes']), g['radius'], s))
        c = np.mean([e['centre'] for e in used], 0)
        for i in xidx:
            assert te['nodes'][i]['parent'] == 0
            m = np.array(nodes[i]['matrix'], float).reshape(4, 4); m[3, :3] = c
            nodes[i]['matrix'] = m.ravel().tolist()
        fl = self.flags.setdefault(self.flame_member(part), [])
        fl.append('flames at the arcade engine exits (%d open rear exits found, %d flame groups in the target): %s [G]' % (
            len(exits), len(groups), '; '.join(rows)))
        if len(exits) > len(groups):
            fl.append('%d smaller exits carry no flame (the target has %d flame group(s); flame node list kept so the '
                      'borrowed engine MCF/VUT bind) [G]' % (len(exits) - len(groups), len(groups)))
        self.stats.setdefault('flames', {})[part] = dict(exits=[dict(centre=e['centre'].round(3).tolist(), radius=round(e['radius'], 3))
                                                                for e in exits], placed=rows)
        return scale

    # PHY keys that make the Revenge pod (chariot + two engine bodies + binder band) behave as one rigid
    # unit. All of them are data the original reads for every pod; nothing is keyed to a pod number.
    RIGID_ZERO = {
        # engine visual bodies (vehicle_visual_authoring49r.cpp; forces in visual_response49r.cpp:380-420):
        # no thrust/drag/lift/impact push, no separation, no torques, no turbulence -> they stay on their
        # spring rest point = the engine nullcg, i.e. where the arcade body has them
        'ImpactForce', 'GroundImpactForce', 'SeparationForce', 'ThrustForce', 'LiftForce', 'DragForce',
        'YawTorque', 'PitchTorque', 'RollTorque', 'PowerslideYawTorque', 'PowerslidePitchTorque',
        'BinderSpring', 'BinderTorque', 'MinTurbulenceScale', 'MaxTurbulenceScale', 'DamageTurbulenceScale',
        'DamageOrientationTurbulenceScale', 'OrientationScale',
        # chariot visual root (chariot_root49u.cpp:144-250): no cable torque, no yaw swing, no turbulence
        'CableForce', 'ChariotMaxYawAngle', 'ChariotMaxPowerslideYawAngle', 'MinChariotTurbulenceScale',
        'MaxChariotTurbulenceScale', 'ChariotOrientationScale',
        # binder band sound (gameplay_audio50he.cpp: binder gain = volume)
        'BinderVolume',
    }

    # f_0C13BD54 0C13BE6E..0C13BE7A: mova @(0C13BEA8),r0; fmov.s @r0,fr3; mov #88,r0; fmov.s @(r0,r14),fr2;
    # mov #80,r0; fmul fr3,fr2; fmov.s fr2,@(r0,r14)  ->  npc+80 (roll) = [0C13BEA8] x npc+88 (yaw rate)
    BANK_CODE = (0x0C13BE6E, bytes.fromhex('0EC708F358E0E6F250E032F227FE'))
    BANK_LITERAL = 0x0C13BEA8

    def arcade_bank_factor(self):
        """Arcade whole-pod bank factor for this kind, or None when the arcade does not roll the pod as one
        (NPC record flag bit0: parts roll separately, MAR). Same literal for every kind [A]."""
        if self.pl['flags'] & 1:
            return None
        at, code = self.BANK_CODE
        assert R.E[at - R.BASE:at - R.BASE + len(code)] == code, 'arcade bank code not where expected'
        f = R.f32(self.BANK_LITERAL)
        assert math.isfinite(f) and f > 0, f
        return f

    def rigid_unit_phy(self, txt):
        flags = []
        for key in sorted(self.RIGID_ZERO):
            pat = re.compile(r'(?m)^(\s*%s\s*=\s*)([^\r\n;]*)' % re.escape(key))
            m = pat.search(txt)
            assert m, 'PHY key %s missing' % key
            flags.append('%s %s -> 0' % (key, m.group(2).strip()))
            txt = txt[:m.start(2)] + '0' + txt[m.end(2):]
        # PORT EXTENSION key (not original; design: work\scratch\rigid-mount\NOTES.md): one line inside the
        # existing [Pod Animation] section. The port then holds the drawn cockpit on the pod frame at the
        # authored chariot offset (chariot nullcg - main origin) in every chariot mode. Absent = original
        # chariot, so it is written only under this rule's data condition (arcade engine count 0). The stock
        # Chariot* lines stay: the strict loader requires them. A duplicate key is refused by the port.
        assert not re.search(r'(?mi)^\s*ChariotRigidMount\s*=', txt), 'ChariotRigidMount already present'
        sec = re.search(r'(?mi)^\s*\[Pod Animation\][^\r\n]*(\r?\n)', txt)
        assert sec, '[Pod Animation] section missing'
        nxt = re.search(r'(?m)^\s*\[', txt[sec.end():])
        body_end = sec.end() + (nxt.start() if nxt else len(txt) - sec.end())
        last = None
        for last in re.finditer(r'(?m)^[ \t]*[A-Za-z0-9]+[ \t]*=[^\r\n]*(\r?\n)', txt[sec.end():body_end]):
            pass
        assert last, '[Pod Animation] has no keys'
        at = sec.end() + last.end()
        line = 'ChariotRigidMount = 1' + last.group(1)
        # PORT EXTENSION key RigidBankFactor (name agreed with the port; absent = no bank): the arcade banks a
        # one-airframe NPC pod as one piece, roll = factor x yaw rate (rad per 60 Hz frame), whole-pod RotZ
        # unless the NPC record flag bit0 is set (f_0C13A8D8 0C13A91C). Factor = the arcade literal read at
        # 0C13BEA8 by f_0C13BD54 (0C13BE6E..BE7A: npc+80 = lit x npc+88); the code bytes are checked so a
        # different image cannot feed a wrong word. Written only for this rule's data condition [A].
        bank = self.arcade_bank_factor()
        if bank is not None:
            line += 'RigidBankFactor = %s' % ('%.6g' % bank) + last.group(1)
        assert not re.search(r'(?mi)^\s*RigidBankFactor\s*=', txt), 'RigidBankFactor already present'
        txt = txt[:at] + line + txt[at:]
        assert len(re.findall(r'(?mi)^\s*ChariotRigidMount\s*=', txt)) == 1 and len(line) < 255
        assert len(re.findall(r'(?mi)^\s*RigidBankFactor\s*=', txt)) == (bank is not None)
        assert len(txt.encode('latin1')) < 8192, 'PHY member must stay under 8192 bytes'
        flags.append('[Pod Animation] ChariotRigidMount = 1 added (port extension key, rigid cockpit mount)')
        if bank is not None:
            flags.append('[Pod Animation] RigidBankFactor = %.6g added (port extension key, arcade whole-pod bank '
                         '= factor x yaw rate, literal 0C13BEA8)' % bank)
        # binder band: [Binder] BinderMeshHalfHeight (read by the original at 215BC8, default 2.25; no
        # original PHY authors it) = 0 -> zero-height band, nothing drawn
        assert not re.search(r'(?mi)^\s*\[Binder\]', txt)
        crlf = chr(13) + chr(10)
        txt = txt.rstrip(crlf) + crlf * 2 + '[Binder]' + crlf + 'BinderMeshHalfHeight = 0' + crlf * 2
        flags.append('[Binder] BinderMeshHalfHeight = 0 added')
        return txt, ['one rigid unit (arcade engine count 0: engines built into the body; no binder band, no '
                     'cables): ' + ', '.join(flags) + ' [G]']

    @staticmethod
    def head_pos(nodes):
        """Chariot-frame position (root excluded) of the node named 'head'."""
        W = pod_psg.world_matrices(nodes)
        i = [k for k, nd in enumerate(nodes) if nd['name'].lower() == 'head'][0]
        return (W[i] @ np.linalg.inv(W[0]))[3, :3]

    def flame_member(self, part):
        return (r'pods\pod%s\%s.psg' % (self.nn, part)).lower()

    # ---------------------------------------------------------------- wreck pieces
    def pieces(self, side, names):
        """Cut the converted engine (LOD 0, no flames) into the target's piece count along the axis
        the target's pieces are laid out on; slab borders at the target pieces' normalised borders [G]."""
        T = self.T
        part = ('englt1', 'engrt1')[side]
        te = T.psg(part)
        te_root = np.array(te['nodes'][0]['matrix'][12:15])
        t_lo, t_hi = self.t_eng_box[side]
        info = []
        for nm in names:
            tp = T.psg(nm)
            r = np.array(tp['nodes'][0]['matrix'][12:15])
            lo, hi = bbox(local_points(tp, 0))
            info.append(dict(name=nm, lo=lo + r - te_root, hi=hi + r - te_root, model=tp))
        allc = np.array([(i['lo'] + i['hi']) / 2 for i in info])
        ax = int(np.argmax(allc.max(0) - allc.min(0))) if len(info) > 1 else int(np.argmax(t_hi - t_lo))
        norm = lambda v: (v - t_lo[ax]) / max(t_hi[ax] - t_lo[ax], 1e-9)  # noqa: E731
        order = sorted(range(len(info)), key=lambda i: (info[i]['lo'][ax] + info[i]['hi'][ax]) / 2)
        borders = []
        for a, b in zip(order[:-1], order[1:]):
            borders.append((norm(info[a]['hi'][ax]) + norm(info[b]['lo'][ax])) / 2)
        m_lo, m_hi = self.eng_box[side]
        tris = self.lods[0]['engines'][side]
        cen = np.array([(t['P'][:, ax].mean() - m_lo[ax]) / max(m_hi[ax] - m_lo[ax], 1e-9) for t in tris])
        slab = np.searchsorted(np.array(borders), cen)
        counts = np.bincount(slab, minlength=len(info))
        if (counts == 0).any():      # a border fell outside the converted geometry: equal-count slabs
            q = np.quantile(cen, np.linspace(0, 1, len(info) + 1)[1:-1])
            slab = np.searchsorted(q, cen)
            self.notes.append('%s pieces: target borders left a piece empty, equal-count slabs used' % part)
        out = {}
        for rank, i in enumerate(order):
            sel = [t for t, s in zip(tris, slab) if s == rank]
            P = np.vstack([t['P'] for t in sel]); N = np.vstack([t['N'] for t in sel])
            UV = np.vstack([t['UVp'] for t in sel])
            lo, hi = P.min(0), P.max(0)
            c = (lo + hi) / 2
            tp = info[i]['model']
            nodes = [dict(name=tp['nodes'][0]['name'], matrix=[1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0] +
                          list(self.root_e[side] + c) + [1.0], parent=-1)]
            tlo, thi = bbox(local_points(tp, 0))
            tW = pod_psg.world_matrices(tp['nodes']); rinv = np.linalg.inv(tW[0])
            for k, nd in enumerate(tp['nodes'][1:], 1):          # e.g. damage00 [G, mapped]
                p = PF.remap((tW[k] @ rinv)[3, :3], tlo, thi, lo - c, hi - c)
                m = np.array(nd['matrix']).reshape(4, 4).copy(); m[3, :3] = p
                assert nd['parent'] == 0
                nodes.append(dict(name=nd['name'], matrix=m.ravel().tolist(), parent=0))
            lods = [dict(cutoff=tp['lods'][0]['cutoff'], surfaces=[dict(
                material=self.page_mat['E'], P=P - c, N=N, UV=UV, bone=np.zeros(len(P), int),
                tris=np.arange(len(P)).reshape(-1, 3))])]
            comp = self.component
            blob, st = pod_psg.build(nodes, lods, comp)
            out[info[i]['name']] = dict(blob=blob, box=(lo - c, hi - c), t_box=(tlo, thi), triangles=len(sel))
            self.stats['piece ' + info[i]['name']] = st
        return out

    # ---------------------------------------------------------------- motions
    def motions(self):
        """Chariot motions listed by the target MCF. Forward / Back: the arcade 'operate' driving
        pose held over the motion (arcade has no lean motion) [A pose, G timing]; brakes and Mid:
        hold the rest pose on the throttle / body node (arcade has no brake flaps) [G]."""
        T = self.T
        mcf = T.ini(r'Pods\Pod%s\chariot.mcf' % self.nn)
        rows = pod_psg.rows(self.ch_nodes)
        row_of = {i: r for r, i in enumerate(rows)}
        out = {}
        for sec, kv in mcf.items():
            if not sec.upper().startswith('MOTION'):
                continue
            name, vue = kv['NAME'], kv['VUE']
            member = T.name_of(r'Pods\Pod%s\%s' % (self.nn, vue[:-4] + '.mot'))
            assert member, 'MCF motion without member: %s' % vue
            tm = PF.read_mot(T.get(member))
            frames, dur = tm['frames'], tm['duration']
            low = name.lower()
            if 'forward' in low or 'back' in low:
                role = 'forward' if 'forward' in low else 'back'
                tracks = []
                for i, nd in enumerate(self.ch_nodes):
                    if i in self.ch_index.values() or nd['name'] in ('handlt', 'handrt'):
                        L3 = np.array(nd['matrix']).reshape(4, 4)[:3, :3]
                        tracks.append((row_of[i], [(0.0, L3), (dur, L3)]))
                blob = PF.write_mot(frames, dur, [], tracks)
                src, fl = 'A', ['%s: arcade "%s" frame 0 pose on every pilot node, held (no arcade lean motion) [A pose, G timing]' % (role, self.pose_name),
                                'ByteAxis rotation quantisation (1.4 deg steps)']
            else:
                if 'brake' in low:
                    node = 'thrlt' if 'left' in low else 'thrrt'
                    role = 'brake ' + ('left' if 'left' in low else 'right')
                else:
                    node, role = 'body', 'mid'
                i = [k for k, nd in enumerate(self.ch_nodes) if nd['name'] == node][0]
                L3 = np.eye(3)
                blob = PF.write_mot(frames, dur, [], [(row_of[i], [(0.0, L3), (dur, L3)])])
                src, fl = 'G', ['%s: arcade has no such motion; rest pose held on %s [G]' % (role, node)]
            out[member] = (blob, src, fl + ['frames/duration from the target motion (%d, %.4f s)' % (frames, dur)])
        return out

    def vut(self):
        lines = ['%s=%s' % (nd['name'], nd['name']) for nd in self.ch_nodes]
        return ('\r\n'.join(lines) + '\r\n').encode('ascii')

    # ---------------------------------------------------------------- collision
    def collisions(self, piece_out):
        T = self.T
        out = {}
        my_parts = {'chariot': self.chariot_box}
        t_parts = {'chariot': self.t_chariot_box}
        # pod frame of podbox*: engine-pair centre (measured on the originals, notes\pod-convert.md)
        tch, te = T.psg('chariot'), [T.psg('englt1'), T.psg('engrt1')]
        t_root_c = np.array(tch['nodes'][0]['matrix'][12:15])
        t_root_e = [np.array(e['nodes'][0]['matrix'][12:15]) for e in te]
        t_ctr = np.mean(t_root_e, 0)
        m_ctr = np.mean(self.root_e, 0)
        t_pair = bbox(np.vstack([np.array(self.t_eng_box[s]) + t_root_e[s] - t_ctr for s in (0, 1)]))
        m_pair = bbox(np.vstack([np.array(self.eng_box[s]) + self.root_e[s] - m_ctr for s in (0, 1)]))
        t_all = bbox(np.vstack([np.array(t_pair), np.array(self.t_chariot_box) + t_root_c - t_ctr]))
        m_all = bbox(np.vstack([np.array(m_pair), np.array(self.chariot_box) + self.root_c - m_ctr]))
        for name in T.names:
            low = name.lower()
            if not low.endswith('.col'):
                continue
            base = os.path.basename(low)[:-4]
            blob = T.get(name)
            if base.startswith('podbox'):
                pts = PF.col_points(blob)
                ext = bbox(pts)
                err = lambda b: float(np.abs(np.array(ext) - np.array(b)).sum())  # noqa: E731
                src, dst, ref = (t_pair, m_pair, 'engine pair') if err(t_pair) <= err(t_all) else (t_all, m_all, 'whole pod')
            elif base.startswith('chariot'):
                src, dst, ref = self.t_chariot_box, self.chariot_box, 'chariot'
            else:
                pname = base[:-1]
                p = piece_out[pname]
                src, dst, ref = p['t_box'], p['box'], 'piece ' + pname
            if base.startswith('podbox') and struct.unpack_from('<I', blob, 12)[0] == 0:
                # type-0 podbox (podboxf, the main contact shape) outlines two parts: the engine-pair box
                # and the chariot box behind it. One affine map over the whole pod squeezed the engine
                # half onto the chariot's width wherever the converted chariot sits between the engines
                # (NEV: the right engine's rear stuck out of the shape -> wall contact launched the pod to
                # 1972 units, fix13-sc-11e; with this file kept original no launch, fix13-exp11-podboxf).
                # Each vertex now maps with its own part (pod_formats.remap_podbox_parts) [G].
                t_ch = (np.array(self.t_chariot_box[0]) + t_root_c - t_ctr, np.array(self.t_chariot_box[1]) + t_root_c - t_ctr)
                m_ch = (np.array(self.chariot_box[0]) + self.root_c - m_ctr, np.array(self.chariot_box[1]) + self.root_c - m_ctr)
                new, rows = PF.remap_podbox_parts(blob, t_pair, m_pair, t_ch, m_ch)
                out[name] = (new, ['target %s structure (type 0); each vertex mapped with its part (engine pair / chariot, pod_formats.remap_podbox_parts): %s; planes/tree boxes recomputed [G]' % (
                    base, rows)])
                continue
            new, kind = PF.remap_col(blob, src, dst)
            out[name] = (new, ['target %s structure (type %d), positions mapped from the target %s box to the converted one, planes/tree boxes recomputed [G]' % (base, kind, ref)])
        return out

    # ---------------------------------------------------------------- assemble
    def convert(self):
        T = self.T
        self.gather()
        self.place_origin()
        self.pages()
        self.component = T.psg('chariot')['lods'][0]['surfaces'][0]['component']
        files = {}
        self.chariot_box = bbox(np.vstack([t['P'] for t in self.lods[0]['cockpit'] + self.lods[0]['pilot']]))
        self.t_chariot_box = bbox(local_points(T.psg('chariot'), 0))
        # cable end points: arcade binder geometry (static cables cockpit -> engines) [A], else target [G]
        self.cable_c, self.cable_e = self.cable_points()
        if any(c is None for c in self.cable_c):
            self.fill_cables_from_target()
        ch = self.chariot()
        self.chariot_box = (self.chariot_box[0] + self.chariot_offset, self.chariot_box[1] + self.chariot_offset)
        pre = r'pods\pod%s' % self.nn
        files[pre + r'\chariot.psg'] = (ch, 'A', [
            'hull: arcade cockpit object, LODs 0-3 = arcade parts 0-3 [A]',
            'pilot merged: arcade pilot object, "%s" frame 0 pose; LOD 0 skinned on %d pilot nodes, LOD 1 rigid, none at LOD 2/3 (arcade draws the pilot for LOD 0/1 only) [A]' % (self.pose_name, len(self.ps['models'])),
            'node set and names generated by one role rule (head, upper_body, shoulder/forearm/hand lt/rt, pilotNN) [G]',
            'thrlt/thrrt + l/r_hand_dummy at the arcade hands (arcade has no throttle levers) [G]',
            'chariotcable nodes: %s' % ', '.join('%s %s' % (s, 'arcade binder geometry end point [A]' if c == 'A' else 'target node mapped by box [G]') for s, c in zip(('left', 'right'), self.cable_source)),
            'damage00: target node mapped from the target chariot box [G]',
            'arcade binder (static cables) not included: Revenge draws cables between nodes at run time',
            'texture page atlas (%s, %s) [G layout, A texels]' % (self.page_mat['C'], self.page_mat['P']),
            'nullcg = chariot physics root on the main-body axis (x 0, y 0, z %.2f; joint rest length %.2f ft); cockpit, '
            'pilot and root-child nodes offset by (%.2f, %.2f, %.2f) ft inside the chariot frame [G]' % (
                self.root_c[2], self.origin['joint_rest_length'], *self.chariot_offset)])
        extra = []          # (insert after member, name, blob, source, flags)
        wmem, watlas = window_members()
        if self.translucent:
            self.flags.setdefault(pre + r'\chariot.psg', []).append(
                '%d arcade see-through (poly type 6) triangles on the original see-through pod material %s (its own '
                'window texture: the arcade glass texels/alpha cannot be carried, see WINDOW_OWN_TEXTURE) [D material, A geometry/UVs]'
                % (self.translucent, WINDOW_MATERIAL))
        if self.translucent or any(T.has(w) for w in WINDOW_MEMBERS):
            anchor = 'textures\\%s.psm' % self.page_mat['C']     # new members after the hull page PSM, POD23's order
            fl = ['byte copy of POD%s: the original shared see-through pod material (POD01/10/19/21/23/25 ship the same '
                  'bytes; a shared name must be identical across mounted pods) [D]' % WINDOW_DONOR] if not WINDOW_OWN_TEXTURE else [
                  'generated once for the converted set (window_members) [G; PSM/ghost structure D POD%s]' % WINDOW_DONOR]
            for wm in WINDOW_MEMBERS:
                if T.has(wm):
                    if WINDOW_OWN_TEXTURE:
                        files[wm.lower()] = (wmem[wm.lower()], 'G', fl)   # e.g. POD23: replace the borrowed copy
                else:
                    extra.append((anchor, wm, wmem[wm.lower()], 'G' if WINDOW_OWN_TEXTURE else 'D', list(fl)))
        for side in (0, 1):
            part = ('englt1', 'engrt1')[side]
            files[pre + '\\%s.psg' % part] = (self.engine(side), 'A', [
                'engine body: %s' % self.engine_source,
                'node list = target %s node list (names/order/parents) so the borrowed MCF/VUT/MOT bind; positions mapped from the target engine box [G]' % part,
                'enginecable node: %s' % ('arcade binder end point [A]' if self.cable_e[side] is not None else 'target node mapped [G]'),
                'flame geometry and flame materials copied from the target %s [D]' % part,
                'texture page %s atlas [G layout, A texels]' % self.page_mat['E']])
        # wreck pieces from the target PHY collections
        piece_out = {}
        for coll, parts in T.wreck():
            for side, part in enumerate(('englt1', 'engrt1')):
                if coll.lower() == part:
                    names = [p for p in parts if T.has(r'pods\pod%s\%s.psg' % (self.nn, p))]
                    piece_out.update(self.pieces(side, names))
        for nm, p in piece_out.items():
            files[pre + '\\%s.psg' % nm] = (p['blob'], 'G', [
                'wreck piece cut from the converted engine (arcade has no wreck pieces): %d triangles [G]' % p['triangles'],
                'child nodes (damage00) mapped from the target piece box [G]'])
        self.piece_out = piece_out
        for name, (blob, fl) in self.collisions(piece_out).items():
            files[name.lower()] = (blob, 'G', fl)
        files[r'pods\pod%s\chariot.vut' % self.nn] = (self.vut(), 'G', ['identity rename list of the converted chariot nodes [G]'])
        for name, (blob, src, fl) in self.motions().items():
            files[name.lower()] = (blob, src, fl)
        for k, a in self.atlas.items():
            pst, pinfo = geo_pst.encode_rgba(a.image)
            files['textures\\%s.pst' % a.name.lower()] = (pst, 'A', [
                'page %s: %d arcade texture cells, atlas %dx%d (target %dx%d) [A texels, G layout]' % (
                    k, len(a.place), a.page_size[0], a.page_size[1], a.size[0], a.size[1]),
                'texel pairs unswapped (pod_export.texture_rgba), 256-colour palette, box mips [G]'] + a.flags)
        # PHY: target unchanged except [Pod Setup] Name
        phy = T.get(r'pods\pod%s\pod%s.phy' % (self.nn, self.nn))
        txt = phy.decode('latin1')
        new = re.sub(r'(?m)^(\s*Name\s*=\s*)"[^"\r\n]*"', lambda m: m.group(1) + '"%s"' % PILOT_NAMES[self.code].replace('"', "'"), txt, count=1)
        assert new != txt
        # first-person eye: the target's offset from its pilot head node, applied at the converted head
        # (FirstPersonCamOffset is chariot-root relative: original_attached_camera.h model1B4) [G]
        fp = re.search(r'(?m)^(\s*FirstPersonCamOffset\s*=\s*)([^\r\n;]*)', new)
        t_head = self.head_pos(self.T.psg('chariot')['nodes'])
        m_head = self.head_pos(self.ch_nodes)
        t_off = np.array([float(v) for v in fp.group(2).split(',')])
        eye = m_head + (t_off - t_head)
        eye_txt = '%.2f, %.2f, %.2f' % tuple(eye)
        new = new[:fp.start(2)] + eye_txt + new[fp.end(2):]
        phy_flags = []
        if self.rigid_unit:
            new, phy_flags = self.rigid_unit_phy(new)
        files[r'pods\pod%s\pod%s.phy' % (self.nn, self.nn)] = (new.encode('latin1'), 'D', [
            'target PHY unchanged except [Pod Setup] Name = %s; stats, wreck tuning, engine positions borrowed [D]' % PILOT_NAMES[self.code].replace('"', "'"),
            '[CameraInfo] FirstPersonCamOffset %s -> %s: target eye offset from its head node, at the converted head [G]' % (fp.group(2).strip(), eye_txt)] + phy_flags)
        if '"' in PILOT_NAMES[self.code]:
            self.notes.append('PHY Name: inner double quotes of %s written as single quotes (the INI value is itself double-quoted)' % PILOT_NAMES[self.code])
        # assemble in target order
        out_members = []
        man = []
        for name in T.names:
            low = name.lower()
            if low in files:
                blob, src, fl = files[low]
            else:
                blob, src, fl = T.get(name), 'D', ['borrowed from the target unchanged [D]']
            fl = fl + self.flags.get(low, [])
            out_members.append((name, blob))
            man.append(dict(name=name, source=src, bytes=len(blob), sha256=sha(blob), flags=fl,
                            target_bytes=len(T.get(name)), target_sha256=sha(T.get(name))))
            for after, xname, xblob, xsrc, xfl in extra:      # new members right after their page PSM
                if after.lower() == low:
                    out_members.append((xname, xblob))
                    man.append(dict(name=xname, source=xsrc, bytes=len(xblob), sha256=sha(xblob), flags=xfl + ['new member (not in the target) [G]'],
                                    target_bytes=None, target_sha256=None))
        assert len(out_members) == len(T.names) + len(extra), 'new member without its page PSM in the target'
        unused = set(files) - {n.lower() for n in T.names}
        assert not unused, 'converted members missing in the target list: %s' % unused
        return out_members, man

    def write(self):
        members, man = self.convert()
        inst = os.path.join(self.out, 'install', 'PODS', 'POD%s' % self.nn)
        os.makedirs(inst, exist_ok=True)
        res = trk_res3.write_res3(members)
        res_path = os.path.join(inst, 'POD%s.RES' % self.nn)
        open(res_path, 'wb').write(res)
        open(os.path.join(inst, 'POD%s.CAM' % self.nn), 'wb').write(self.T.cam)
        mdir = os.path.join(self.out, 'members')
        for name, blob in members:
            p = os.path.join(mdir, *name.split('\\'))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            open(p, 'wb').write(blob)
        for k, a in self.atlas.items():
            from PIL import Image
            os.makedirs(os.path.join(self.out, 'verify'), exist_ok=True)
            Image.fromarray(a.image).save(os.path.join(self.out, 'verify', 'page_%s_%s.png' % (k, a.name)))
        self.res = res
        self.members = members
        self.manifest = man
        return res_path


def main(args):
    import dlc_out
    dlc_dir, args = dlc_out.pop_option(args, '--dlc-out')
    pairs = list(zip(args[0::2], args[1::2])) if args else list(TARGETS.items())
    for code, nn in pairs:
        pod = Pod(code.upper(), nn)
        path = pod.write()
        import pod_verify
        pod_verify.verify(pod)
        print(code, nn, path, len(pod.res))
    if dlc_dir:
        import pod_silo
        pod_silo.cmd_package(sorted('%02d' % int(nn) for code, nn in pairs), OUT, dlc_dir, 'arcade-pods')


if __name__ == '__main__':
    main(sys.argv[1:])
