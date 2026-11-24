#!/usr/bin/env python3
r"""Star Wars Episode I: Racer (N64) model reader: header, node tree, meshes, F3DEX2 display lists,
materials, collision, surface (Behaviour) records, triggers, animations.

Layouts are taken from the facts in notes\tooling-survey.md and notes\formats.md (sources: the
SW_RACER_RE PC decompilation types.h, swe1r-assets C# records, blender-swe1r reader); the code is
our own.

Coordinates are the game's: right-handed, +Z up. Node matrices are row-vector 3x4
(rows A, B, C = rotation/scale, D = translation): world = local * parent.

python -I model.py N        dump node tree / counts for model N
"""
import math
import struct
import sys
import os

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import blocks as B  # noqa: E402

NODE_MESHGROUP = 0x3064
NODE_BASIC = 0x5064
NODE_SELECTOR = 0x5065
NODE_LOD = 0x5066
NODE_TRANSFORMED = 0xD064
NODE_TRANSFORMED_PIVOT = 0xD065
NODE_TRANSFORMED_COMPUTED = 0xD066
NODE_NAMES = {NODE_MESHGROUP: 'MeshGroup', NODE_BASIC: 'Basic', NODE_SELECTOR: 'Selector',
              NODE_LOD: 'LODSelector', NODE_TRANSFORMED: 'Transformed',
              NODE_TRANSFORMED_PIVOT: 'TransformedWithPivot', NODE_TRANSFORMED_COMPUTED: 'TransformedComputed'}

VEHICLE_REACTION = [  # bit -> name (swe1r-assets VehicleReaction / SW_RACER_RE swrVehicleReaction)
    (0x1, 'ZOn'), (0x2, 'ZOff'), (0x4, 'Fast'), (0x8, 'Slow'), (0x10, 'Swst'), (0x20, 'Slip'),
    (0x40, 'Dust'), (0x80, 'Snow'), (0x100, 'Wet'), (0x200, 'Ruff'), (0x400, 'Swmp'), (0x800, 'NSnw'),
    (0x1000, 'Mirr'), (0x2000, 'Lava'), (0x4000, 'Fall'), (0x8000, 'Soft'), (0x10000, 'NRSp'),
    (0x20000, 'Flat'), (0x20000000, 'Side')]
VEHICLE_REACTION_MEANING = {
    'ZOn': 'zero-gravity on (Oovo IV)', 'ZOff': 'zero-gravity off', 'Fast': 'speed boost surface',
    'Slow': 'slow surface', 'Swst': 'unknown (Baroo Coast)', 'Slip': 'slippery (ice)',
    'Dust': 'dust kick-up', 'Snow': 'snow', 'Wet': 'wet / water spray', 'Ruff': 'rough',
    'Swmp': 'swamp', 'NSnw': 'no snow (?)', 'Mirr': 'mirror / reflective (?)', 'Lava': 'lava (heat damage)',
    'Fall': 'fall-out / death plane', 'Soft': 'soft', 'NRSp': 'no respawn here (?)', 'Flat': 'flat (?)',
    'Side': 'side wall'}

TRIGGER_TYPES = {  # SW_RACER_RE swrModel_TriggerType / survey list
    100: 'ice break', 101: 'door animation', 102: 'asteroid break', 103: 'tent shortcut',
    104: 'kill (Vengeance)', 105: 'glacier collapse', 106: 'falling asteroids', 107: 'red carpet',
    108: 'slalom flag', 201: 'rock break', 202: 'rock break 2', 203: 'tusken', 208: 'balloon',
    211: 'flame', 212: 'branch break', 213: 'earthquake shortcut', 301: 'Malastare lake',
    304: 'tram', 306: 'earthquake', 307: 'door', 308: 'fish', 310: 'dozer', 312: 'falling pillar',
    314: 'methane', 501: 'lava splash', 503: 'unknown 503', 900: 'teleport to spline'}
TRIGGER_FLAGS = [(0x1, 'enabled'), (0x2, 'needs_speed_over_150'), (0x4, 'skip_lap1'),
                 (0x8, 'skip_lap2'), (0x10, 'skip_lap3'), (0x20, 'not_for_ai')]

ANIM_TYPES = {0x1: 'type1', 0x2: 'texture_flipbook', 0x4: 'type4', 0x5: 'type5', 0x6: 'type6',
              0x7: 'type7', 0x8: 'axis_angle', 0x9: 'translation', 0xA: 'scale',
              0xB: 'uv_scroll_x', 0xC: 'uv_scroll_y'}
ANIM_VALUE_WIDTH = {0x1: 1, 0xB: 1, 0xC: 1, 0x4: 2, 0x6: 4, 0x8: 4, 0x7: 3, 0x9: 3, 0xA: 3}


def bits(v, table):
    return [n for b, n in table if v & b]


class Model:
    def __init__(self, index, mb=None):
        mb = mb or B.ModelBlock()
        self.index = index
        self.mask, self.data = mb.model(index)
        self.d = self.data
        self.reloc = set(w * 4 for w in B.reloc_words(self.mask, len(self.data)))
        self.tag = self.data[:4].decode('latin1')
        self.warnings = []
        self._parse_header()
        self.nodes = {}
        self.meshes = {}
        self.materials = {}
        self.mtex = {}
        self.behaviours = {}
        for o in self.header_nodes:
            if o:
                self.node(o)
        self.anims = [self.animation(a) for a in self.anim_ptrs]

    # --- primitives -------------------------------------------------------------------------
    def u32(self, o):
        return struct.unpack_from('>I', self.d, o)[0]

    def s32(self, o):
        return struct.unpack_from('>i', self.d, o)[0]

    def u16(self, o):
        return struct.unpack_from('>H', self.d, o)[0]

    def s16(self, o):
        return struct.unpack_from('>h', self.d, o)[0]

    def f32(self, o, n=1):
        v = struct.unpack_from('>%df' % n, self.d, o)
        return v[0] if n == 1 else list(v)

    def ptr(self, o):
        """Pointer at o: model-relative offset, or None. Texture refs return ('tex', index)."""
        v = self.u32(o)
        if v == 0:
            return None
        if (v & 0xFF000000) == 0x0A000000:
            return ('tex', v & 0xFFFFFF)
        if o not in self.reloc:
            self.warnings.append('word at 0x%x read as pointer but not in relocation mask (0x%x)' % (o, v))
        return v

    # --- header -----------------------------------------------------------------------------
    def _parse_header(self):
        o = 4
        self.header_nodes = []
        while self.u32(o) != 0xFFFFFFFF:
            self.header_nodes.append(self.u32(o))
            o += 4
        o += 4
        self.light_streaks = []
        self.anim_ptrs = []
        self.altn = []
        while True:
            t = self.d[o:o + 4]
            o += 4
            if t == b'HEnd':
                break
            if t == b'Data':
                n = self.u32(o)
                o += 4
                end = o + 4 * n
                while o < end:
                    if self.d[o:o + 4] == b'LStr':
                        self.light_streaks.append(self.f32(o + 4, 3))
                        o += 16
                    else:
                        o += 4
                o = end
            elif t == b'Anim':
                while self.u32(o):
                    self.anim_ptrs.append(self.u32(o))
                    o += 4
                o += 4
            elif t == b'AltN':
                while self.u32(o):
                    self.altn.append(self.u32(o))
                    o += 4
                o += 4
            else:
                raise ValueError('model %d: unknown header section %r at 0x%x' % (self.index, t, o - 4))
        self.header_end = o

    # --- nodes ------------------------------------------------------------------------------
    def node(self, o, depth=0):
        if o in self.nodes:
            return self.nodes[o]
        t = self.u32(o)
        if t not in NODE_NAMES:
            raise ValueError('model %d: bad node type 0x%x at 0x%x' % (self.index, t, o))
        n = dict(off=o, type=t, name=NODE_NAMES[t], flags1=self.u32(o + 4), flags2=self.u32(o + 8),
                 flags3=self.u16(o + 0xC), light_index=self.u16(o + 0xE), flags5=self.u32(o + 0x10),
                 children=[], meshes=[])
        self.nodes[o] = n
        cnt = self.u32(o + 0x14)
        cptr = self.u32(o + 0x18)
        if t in (NODE_TRANSFORMED, NODE_TRANSFORMED_PIVOT):
            n['matrix'] = self.f32(o + 0x1C, 12)
            if t == NODE_TRANSFORMED_PIVOT:
                n['pivot'] = self.f32(o + 0x4C, 3)
        elif t == NODE_SELECTOR:
            n['selected'] = self.s32(o + 0x1C)
        elif t == NODE_LOD:
            n['lod_distances'] = self.f32(o + 0x1C, 8)
        elif t == NODE_TRANSFORMED_COMPUTED:
            n['follow_model_position'] = self.u16(o + 0x1C)
            n['orientation'] = self.u16(o + 0x1E)
            n['up'] = self.f32(o + 0x20, 3)
        elif t == NODE_MESHGROUP:
            n['aabb'] = self.f32(o + 0x1C, 6)
        n['child_slots'] = []
        for i in range(cnt):
            c = self.u32(cptr + 4 * i) if cptr else 0
            n['child_slots'].append(c)
            if not c:
                continue
            if t == NODE_MESHGROUP:
                n['meshes'].append(self.mesh(c))
            else:
                n['children'].append(self.node(c, depth + 1))
        return n

    # --- materials --------------------------------------------------------------------------
    def material_texture(self, o):
        if o in self.mtex:
            return self.mtex[o]
        d = dict(off=o, unk0=self.u32(o), res=[self.s16(o + 4), self.s16(o + 6)],
                 format=self.u16(o + 0xC), num_children=self.u16(o + 0xE),
                 width=self.u16(o + 0x10), height=self.u16(o + 0x12),
                 scale=[self.u16(o + 0x14), self.u16(o + 0x16)],
                 flags=self.u16(o + 0x18), mask=self.u16(o + 0x1A))
        specs = []
        for i in range(6):
            p = self.u32(o + 0x1C + 4 * i)
            if p and (o + 0x1C + 4 * i) in self.reloc:
                specs.append(dict(off=p, flags=self.u32(p), unk1=self.u32(p + 4), unk2=self.u32(p + 8),
                                  w=self.u16(p + 0xC), h=self.u16(p + 0xE)))
            else:
                specs.append(None)
        d['specs'] = specs
        tv = self.u32(o + 0x38)
        d['texture_index'] = (tv & 0xFFFFFF) if (tv & 0xFF000000) == 0x0A000000 else None
        if d['texture_index'] == 0xFFFFFF:  # 0x0AFFFFFF: index -1, no block texture (set at runtime / flipbook)
            d['texture_index'] = None
            d['texture_minus_one'] = True
        if tv and d['texture_index'] is None:
            d['texture_raw'] = tv
        f0 = specs[0]['flags'] if specs[0] else 0
        # N64 tile wrap bits in the low byte of spec[0].flags (gDPSetTile cms/cmt: 1 mirror,
        # 2 clamp): s in bits 4-5, t in bits 0-1 (the PC release moved them to the top byte)
        d['mirror_s'] = bool(f0 & 0x10)
        d['mirror_t'] = bool(f0 & 0x01)
        d['clamp_s'] = bool(f0 & 0x20)
        d['clamp_t'] = bool(f0 & 0x02)
        d['has_mip_chain'] = specs[1] is not None
        self.mtex[o] = d
        return d

    def material(self, o):
        if o in self.materials:
            return self.materials[o]
        m = dict(off=o, flags=self.u32(o), tex_offset=[self.s16(o + 4), self.s16(o + 6)])
        tp = self.u32(o + 8)
        m['texture'] = self.material_texture(tp) if tp else None
        sp = self.u32(o + 0xC)
        if sp:
            m['combine'] = [self.u32(sp + 6), self.u32(sp + 0xA), self.u32(sp + 0xE), self.u32(sp + 0x12)]
            m['render_mode'] = [self.u32(sp + 0x18), self.u32(sp + 0x1C)]
            m['prim_color'] = list(self.d[sp + 0x22:sp + 0x26])
            m['shader_unk'] = [self.u32(sp), self.u16(sp + 4), self.u16(sp + 0x16), self.u16(sp + 0x20)]
        self.materials[o] = m
        return m

    # --- behaviour / triggers ---------------------------------------------------------------
    def behaviour(self, o):
        if o in self.behaviours:
            return self.behaviours[o]
        d = self.d
        b = dict(off=o, word00=self.u16(o), fog_flags=d[o + 2], fog_color=list(d[o + 3:o + 6]),
                 fog_start=self.u16(o + 6), fog_end=self.u16(o + 8), light_flags=self.u16(o + 0xA),
                 ambient_color=list(d[o + 0xC:o + 0xF]), light_color=list(d[o + 0xF:o + 0x12]),
                 unk12=self.u16(o + 0x12), light_vector=self.f32(o + 0x14, 3))
        # 0x20: pointer in SW_RACER_RE, float in swe1r-assets: decide by the relocation mask
        if (o + 0x20) in self.reloc:
            b['unk20_node'] = self.u32(o + 0x20) or None
        else:
            b['unk20_raw'] = self.u32(o + 0x20)
        b['unk24'] = self.u32(o + 0x24)
        b['unk28'] = self.u32(o + 0x28)
        vr = self.u32(o + 0x2C)
        b['vehicle_reaction'] = vr
        b['surface'] = bits(vr, VEHICLE_REACTION)
        unk = vr & ~sum(x for x, _ in VEHICLE_REACTION)
        if unk:
            b['surface_unknown_bits'] = unk
        b['unk30'] = self.u16(o + 0x30)
        b['unk32'] = self.u16(o + 0x32)
        b['unk34'] = self.s32(o + 0x34)
        b['unk38'] = self.s32(o + 0x38)
        trig = []
        p = self.u32(o + 0x3C) if (o + 0x3C) in self.reloc else 0
        seen = set()
        while p and p not in seen and p + 0x2C <= len(d):
            seen.add(p)
            t = dict(off=p, center=self.f32(p, 3), direction=self.f32(p + 0xC, 3),
                     size_xy=self.f32(p + 0x18), size_z=self.f32(p + 0x1C),
                     affected_node=(self.u32(p + 0x20) or None), type=self.u16(p + 0x24),
                     flags=self.u16(p + 0x26))
            t['type_name'] = TRIGGER_TYPES.get(t['type'], 'unknown')
            t['flag_names'] = bits(t['flags'], TRIGGER_FLAGS)
            trig.append(t)
            p = self.u32(p + 0x28) if (p + 0x28) in self.reloc else 0
        b['triggers'] = trig
        if (o + 0x3C) not in self.reloc and self.u32(o + 0x3C):
            b['unk3c_raw'] = self.u32(o + 0x3C)
        self.behaviours[o] = b
        return b

    # --- meshes -----------------------------------------------------------------------------
    def mesh(self, o):
        if o in self.meshes:
            return self.meshes[o]
        m = dict(off=o)
        mp = self.u32(o)
        m['material'] = self.material(mp) if mp else None
        bp = self.u32(o + 4)
        m['behaviour'] = self.behaviour(bp) if bp else None
        m['aabb'] = self.f32(o + 8, 6)
        m['prim_count'] = self.u16(o + 0x20)
        m['prim_type'] = self.u16(o + 0x22)
        m['prim_sizes_ptr'] = self.u32(o + 0x24)
        m['group_ptr'] = self.u32(o + 0x28)
        m['coll_vtx_ptr'] = self.u32(o + 0x2C)
        m['dl_ptr'] = self.u32(o + 0x30)
        m['vtx_ptr'] = self.u32(o + 0x34)
        m['coll_vtx_count'] = self.u16(o + 0x38)
        m['vtx_count'] = self.u16(o + 0x3A)
        m['unk3c'] = self.u16(o + 0x3C)
        m['vtx_base'] = self.s16(o + 0x3E)
        self.meshes[o] = m
        return m

    def collision_tris(self, m):
        """Collision triangles of mesh m: (verts (N,3) float, tris (M,3) int)."""
        n = m['coll_vtx_count']
        p = m['coll_vtx_ptr']
        if not p or n < 3:
            return np.zeros((0, 3)), np.zeros((0, 3), int)
        V = np.array(struct.unpack_from('>%dh' % (3 * n), self.d, p), float).reshape(n, 3)
        tris = []
        start = 0
        if m['prim_sizes_ptr']:
            sizes = struct.unpack_from('>%dI' % m['prim_count'], self.d, m['prim_sizes_ptr'])
            for s in sizes:
                for k in range(s - 2):
                    if k % 2 == 0:
                        tris.append((start + k, start + k + 1, start + k + 2))
                    else:
                        tris.append((start + k + 1, start + k, start + k + 2))
                start += s
        else:
            s = m['prim_type']
            for _ in range(m['prim_count']):
                for k in range(s - 2):
                    if s == 3:
                        tris.append((start + k, start + k + 1, start + k + 2))
                    elif k % 2 == 0:
                        tris.append((start + k, start + k + 1, start + k + 3))
                    else:
                        tris.append((start + k, start + k + 1, start + k + 2))
                start += s
        T = np.array(tris, int).reshape(-1, 3)
        bad = (T >= n).any(1)
        if bad.any():
            self.warnings.append('mesh 0x%x: %d collision tris index past %d verts' % (m['off'], bad.sum(), n))
            T = T[~bad]
        return V, T

    def visual_tris(self, m):
        """Walk the F3DEX2 display list. Returns dict(pos, st, rgba, tris) or None.
        Vertex cache: G_VTX loads n vertices from an absolute model address into slots v0..v0+n-1;
        G_TRI1/G_TRI2 index slots (index*2 in the command bytes)."""
        dl = m['dl_ptr']
        if not dl:
            return None
        d = self.d
        cache = [None] * 64
        verts = {}  # vertex address -> output index
        order = []
        tris = []
        o = dl
        unknown = {}
        for _ in range(100000):
            if o + 8 > len(d):
                self.warnings.append('mesh 0x%x: display list ran off the data' % m['off'])
                break
            c = d[o]
            if c == 0xDF:
                break
            if c == 0x01:
                w0 = self.u32(o)
                nv = (w0 >> 12) & 0xFF
                end = (w0 >> 1) & 0x7F
                v0 = end - nv
                addr = self.u32(o + 4)
                if (o + 4) not in self.reloc:
                    self.warnings.append('mesh 0x%x: G_VTX address 0x%x not relocated' % (m['off'], addr))
                for k in range(nv):
                    if 0 <= v0 + k < 64:
                        cache[v0 + k] = addr + 16 * k
            elif c == 0x05 or c == 0x06:
                groups = [(d[o + 1], d[o + 2], d[o + 3])]
                if c == 0x06:
                    groups.append((d[o + 5], d[o + 6], d[o + 7]))
                for g in groups:
                    idx = []
                    for a in g:
                        va = cache[a // 2] if a // 2 < 64 else None
                        if va is None:
                            idx = None
                            break
                        if va not in verts:
                            verts[va] = len(order)
                            order.append(va)
                        idx.append(verts[va])
                    if idx is None:
                        self.warnings.append('mesh 0x%x: triangle uses an unloaded vertex slot' % m['off'])
                    else:
                        tris.append(idx)
            elif c == 0x03 or c == 0x00:
                pass  # G_CULLDL / G_NOOP
            else:
                unknown[c] = unknown.get(c, 0) + 1
            o += 8
        if unknown:
            self.warnings.append('mesh 0x%x: unknown DL commands %s' % (
                m['off'], ', '.join('0x%02X x%d' % kv for kv in sorted(unknown.items()))))
        if not order:
            return None
        n = len(order)
        pos = np.zeros((n, 3))
        st = np.zeros((n, 2))
        rgba = np.zeros((n, 4), np.uint8)
        for i, a in enumerate(order):
            x, y, z, _f, s, t = struct.unpack_from('>3hH2h', d, a)
            pos[i] = (x, y, z)
            st[i] = (s, t)
            rgba[i] = tuple(d[a + 12:a + 16])
        return dict(pos=pos, st=st, rgba=rgba, tris=np.array(tris, int).reshape(-1, 3), vaddr=order)

    # --- animations -------------------------------------------------------------------------
    def animation(self, o):
        """Animation record (fields from swe1r-assets Animation.cs offsets 0xDC..0x128)."""
        a = dict(off=o, loop_transition_speed=self.f32(o + 0xDC), transition_speed=self.f32(o + 0xE0),
                 start_time=self.f32(o + 0xF0), end_time=self.f32(o + 0xF4), duration=self.f32(o + 0xF8),
                 duration3=self.f32(o + 0xFC))
        fl = self.u32(o + 0x100)
        a['type'] = fl & 0xF
        a['type_name'] = ANIM_TYPES.get(a['type'], 'unknown')
        a['flags'] = fl & ~0xF
        a['keyframes'] = n = self.u32(o + 0x104)
        a['speed'] = self.f32(o + 0x110)
        tp = self.u32(o + 0x11C)
        vp = self.u32(o + 0x120)
        a['target'] = self.u32(o + 0x124) or None
        a['unk128'] = self.u32(o + 0x128)
        a['times'] = self.f32(tp, n) if (tp and n) else []
        if n == 1 and tp:
            a['times'] = [a['times']]
        w = ANIM_VALUE_WIDTH.get(a['type'], 0)
        if a['type'] == 0x2 and vp:
            a['values'] = [self.u32(vp + 4 * i) for i in range(n)]  # MaterialTexture pointers
        elif w and vp:
            vals = self.f32(vp, n * w)
            if not isinstance(vals, list):
                vals = [vals]
            a['values'] = [vals[i * w:(i + 1) * w] for i in range(n)]
        else:
            a['values'] = []
        return a

    # --- traversal --------------------------------------------------------------------------
    @staticmethod
    def local_matrix(n):
        M = np.eye(4)
        if 'matrix' in n:
            m = n['matrix']
            M[0, :3] = m[0:3]
            M[1, :3] = m[3:6]
            M[2, :3] = m[6:9]
            M[3, :3] = m[9:12]
            if n['type'] == NODE_TRANSFORMED_PIVOT and (n['flags3'] & 0x10):
                p = np.array(n['pivot'])
                M[3, :3] += p - p @ M[:3, :3]
        return M

    def walk(self, root, select='disk', parent=None, path=()):
        """Yield (node, world 4x4 row-vector matrix, path of node offsets) depth first.
        select: 'disk' follows selector values on disk (-1 all, n one, -2 none) and LOD child 0;
                'all' visits every child."""
        if parent is None:
            parent = np.eye(4)
        n = self.nodes[root] if isinstance(root, int) else root
        W = self.local_matrix(n) @ parent
        yield n, W, path + (n['off'],)
        kids = n['children']
        if select == 'disk':
            if n['type'] == NODE_SELECTOR:
                s = n['selected']
                if s == -2:
                    kids = []
                elif s >= 0:
                    slots = n['child_slots']
                    kids = [self.nodes[slots[s]]] if s < len(slots) and slots[s] else []
            elif n['type'] == NODE_LOD:
                kids = kids[:1]
        for c in kids:
            if c['off'] in path:
                continue
            yield from self.walk(c, select, W, path + (n['off'],))


def apply(W, P):
    P = np.asarray(P, float)
    return P @ W[:3, :3] + W[3, :3]


def dump(idx):
    M = Model(idx)
    print('model %d tag %s, %d bytes, header nodes %s' % (idx, M.tag, len(M.d), [hex(x) for x in M.header_nodes]))
    print('light streaks %d, animations %d, AltN %d' % (len(M.light_streaks), len(M.anims), len(M.altn)))

    def rec(n, dep):
        extra = ''
        if 'matrix' in n:
            extra = ' pos=(%.0f %.0f %.0f)' % tuple(n['matrix'][9:12])
        if 'selected' in n:
            extra += ' sel=%d' % n['selected']
        print('%s%s 0x%x f1=%x f2=%x f3=%x f5=%x kids=%d meshes=%d%s' % (
            '  ' * dep, n['name'], n['off'], n['flags1'], n['flags2'], n['flags3'], n['flags5'],
            len(n['children']), len(n['meshes']), extra))
        if dep < 3:
            for c in n['children']:
                rec(c, dep + 1)
    for h in M.header_nodes:
        if h:
            rec(M.nodes[h], 0)
    for a in M.anims[:40]:
        print('anim 0x%x %s n=%d target=%s flags=%x times=%s' % (
            a['off'], a['type_name'], a['keyframes'], hex(a['target'] or 0), a['flags'], a['times'][:4]))
    print('meshes %d, materials %d, textures %d, behaviours %d' % (
        len(M.meshes), len(M.materials), len(M.mtex), len(M.behaviours)))
    for w in sorted(set(M.warnings))[:30]:
        print('WARN', w)


if __name__ == '__main__':
    dump(int(sys.argv[1]))
