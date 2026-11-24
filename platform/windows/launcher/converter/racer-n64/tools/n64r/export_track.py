#!/usr/bin/env python3
r"""Export one or all Star Wars Episode I: Racer (N64 USA) tracks.

python -I export_track.py 115          (model index, as in the N64 track table)
python -I export_track.py all
python -I export_track.py 115 --no-preview

Writes work\scratch\racer-n64\export\<model>_<Name>\
  geometry.obj / geometry.mtl     visual mesh, world space, game units, +Z up, per-vertex colour
                                  (OBJ 'v x y z r g b'), vt from N64 s/t, groups per mesh
  skybox.obj / skybox.mtl         skybox model (header slot +0x0C)
  textures\*.png                  decoded textures (mirror wrap baked as a doubled image)
  materials.json                  per material: texture file, N64 format, wrap/clamp, culling,
                                  combiner + render mode words, primitive colour
  collision.obj                   collision triangles, world space, group per mesh, usemtl per surface
  collision_surfaces.json         per collision mesh: triangle range in collision.obj + surface record
  triggers.json                   trigger volumes
  splines.json / splines.obj      the track spline graph (forks, joins, progress) + Bezier polylines
  cameras.json                    intro camera (per-frame eye/target), other camera splines,
                                  animated empties
  track.json                      metadata: names, planet, track number, counts, visibility rule
  preview_persp.png, preview_top.png, preview_camera.png
"""
import json
import math
import os
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import blocks as B  # noqa: E402
import model as Mo  # noqa: E402
import splines as S  # noqa: E402
import textures as TX  # noqa: E402
import cameras as CAM  # noqa: E402

ROOT = B.ROOT
EXPORT = os.path.join(ROOT, 'export')
ROM = os.path.join(ROOT, 'rom', 'racer.z64')
TRACK_TABLE_ROM = 0xA2DB8  # N64 USA copy of g_aTrackInfos, 25 x 12 bytes (found by search)

TRACK_NAMES = ["The Boonta Training Course", "The Boonta Classic", "Beedo's Wild Ride", "Howler Gorge",
               "Andobi Mountain Run", "Ando Prime Centrum", "Aquilaris Classic", "Sunken City",
               "Bumpy's Breakers", "Scrapper's Run", "Dethro's Revenge", "Abyss", "Baroo Coast",
               "Grabvine Gateway", "Fire Mountain Rally", "Inferno", "Mon Gazza Speedway", "Spice Mine Run",
               "Zugga Challenge", "Vengeance", "Executioner", "The Gauntlet", "Malastare 100", "Dug Derby",
               "Sebulba's Legacy"]
PLANETS = ['Tatooine', 'Ando Prime', 'Aquilaris', 'Ord Ibanna', 'Baroonda', 'Mon Gazza', 'Oovo IV', 'Malastare']

SURF_COLOURS = {  # preview colours by first matching surface flag
    'Lava': (230, 60, 20), 'Fall': (120, 0, 120), 'ZOn': (0, 200, 255), 'ZOff': (0, 120, 160),
    'Fast': (40, 220, 40), 'Slow': (200, 170, 40), 'Slip': (170, 230, 255), 'Swmp': (90, 110, 50),
    'Wet': (60, 100, 220), 'Side': (150, 150, 150), 'Snow': (240, 240, 240), 'Ruff': (150, 110, 80),
    'Dust': (200, 180, 140), 'Soft': (220, 150, 190), 'Mirr': (200, 230, 230), 'Swst': (120, 180, 160),
}


def track_table():
    with open(ROM, 'rb') as f:
        f.seek(TRACK_TABLE_ROM)
        raw = f.read(25 * 12)
    out = []
    for k in range(25):
        m, s, tn, pl, fav, pad = struct.unpack_from('>II4B', raw, 12 * k)
        out.append(dict(table_index=k, name=TRACK_NAMES[k], model=m, spline_id=s, track_number=tn,
                        planet_index=pl, planet=PLANETS[pl], favourite_pilot=fav))
    return out


def alpha_analysis(cw):
    """Which inputs the N64 colour combiner reads (words packed as 4 bytes a, b, c, d of
    (a - b) * c + d; colour inputs 1 TEXEL0, 2 TEXEL1, 4 SHADE; alpha inputs 1 TEXEL0, 2 TEXEL1,
    4 SHADE, 7 zero). Final stage = cycle 2 unless cycle 2 just passes 'combined' (d = 0, rest 0x1f/7).
    Interpretation of the packed words is ours (GUESS where the cycle type is unknown)."""
    if len(cw) < 4:
        return dict(uses_texture_colour=None, uses_texture_alpha=None, uses_vertex_alpha=None)
    def b4(w):
        return [(w >> 24) & 255, (w >> 16) & 255, (w >> 8) & 255, w & 255]
    c1, a1, c2, a2 = (b4(w) for w in cw)
    def used(word, zero):
        a, b, c, d = word
        out = set()
        if c != zero and not (a == b):
            out |= {a, b, c}
        out.add(d)
        return out - {zero}
    ua1, ua2 = used(a1, 7), used(a2, 7)
    uc1, uc2 = used(c1, 0x1f), used(c2, 0x1f)
    fa = ua1 | (ua2 - {0}) if 0 in ua2 else ua2
    fc = uc1 | (uc2 - {0}) if 0 in uc2 else uc2
    return dict(uses_texture_colour=bool(fc & {1, 2}), uses_texture_alpha=bool(fa & {1, 2}),
                uses_vertex_alpha=bool(fa & {4}))


def safe(name):
    return ''.join(c if c.isalnum() else '_' for c in name).strip('_').replace('__', '_')


def visible_for_track(node, tn):
    """Planet-track visibility (data rule, see formats.md): node shown for planet track n when
    flags1 bit (0x10 << n) is set; collision likewise on flags2."""
    return bool(node['flags1'] & (0x10 << tn)), bool(node['flags2'] & (0x10 << tn))


class Exporter:
    def __init__(self, tr, mb, sb, tex):
        self.tr = tr
        self.M = Mo.Model(tr['model'], mb)
        self.sb = sb
        self.tex = tex
        self.dir = os.path.join(EXPORT, '%03d_%s' % (tr['model'], safe(tr['name'])))
        os.makedirs(os.path.join(self.dir, 'textures'), exist_ok=True)
        self.tex_files = {}
        self.mat_json = {}
        self.problems = []

    # --- geometry collection --------------------------------------------------------------
    def collect(self, root):
        M, tn = self.M, self.tr['track_number']
        vis, col = [], []
        hidden_v = hidden_c = 0
        for n, W, path in M.walk(root, 'disk'):
            if n['type'] != Mo.NODE_MESHGROUP:
                continue
            chain = [M.nodes[o] for o in path]
            sv = all(visible_for_track(c, tn)[0] for c in chain)
            sc = all(visible_for_track(c, tn)[1] for c in chain)
            mirrored = bool(np.linalg.det(W[:3, :3]) < 0)
            for m in n['meshes']:
                if m['dl_ptr']:
                    if sv:
                        vis.append((m, W, n, mirrored))
                    else:
                        hidden_v += 1
                if m['coll_vtx_count'] >= 3:
                    if sc:
                        col.append((m, W, n))
                    else:
                        hidden_c += 1
        return vis, col, hidden_v, hidden_c

    # --- textures / materials -------------------------------------------------------------
    def texture_file(self, t):
        from PIL import Image
        key = (t['texture_index'], t['format'], t['width'], t['height'], t['mirror_s'], t['mirror_t'])
        if key in self.tex_files:
            return self.tex_files[key]
        # textures with a mip chain (a second tile spec) are stored TMEM-interleaved (odd rows: the two 32-bit halves of every
        # 64-bit word swapped); undo it (textures.py swizzled) - all 29 such textures on the converted tracks decode smooth
        # only this way (racer-n64 work n3 swz_survey.py; Dug Derby rail tex1209 = diagonal blue/white stripes)
        img, note = self.tex.get(t['texture_index'], t['format'], t['width'], t['height'], swizzled=bool(t['has_mip_chain']))
        if note:
            self.problems.append('texture %d: %s' % (t['texture_index'], note))
        img = TX.mirror_bake(img, t['mirror_s'], t['mirror_t'])
        name = 'tex%04d_%s_%dx%d%s%s.png' % (t['texture_index'], TX.FORMAT_NAMES.get(t['format'], 'x'),
                                             t['width'], t['height'], '_ms' if t['mirror_s'] else '',
                                             '_mt' if t['mirror_t'] else '')
        Image.fromarray(img).save(os.path.join(self.dir, 'textures', name))
        self.tex_files[key] = (name, img)
        return self.tex_files[key]

    def material_entry(self, mat):
        key = 'm%06x' % mat['off']
        if key in self.mat_json:
            return key
        t = mat['texture']
        e = dict(name=key, mesh_material_flags=mat['flags'], texture_offset=mat['tex_offset'],
                 vertex_normals=bool(mat['flags'] & 0x11),
                 cull=('back' if mat['flags'] & 0x8 else 'front' if mat['flags'] & 0x40 else 'none'),
                 combine_words=['0x%08x' % x for x in mat.get('combine', [])],
                 render_mode_words=['0x%08x' % x for x in mat.get('render_mode', [])],
                 prim_color=mat.get('prim_color'))
        e.update(alpha_analysis(mat.get('combine', [0, 0, 0, 0])))
        if t and t['texture_index'] is not None:
            fn, img = self.texture_file(t)
            e.update(texture=fn, texture_index=t['texture_index'], format=TX.FORMAT_NAMES.get(t['format']),
                     format_word='0x%04x' % t['format'], width=t['width'], height=t['height'],
                     mirror_s=t['mirror_s'], mirror_t=t['mirror_t'], clamp_s=t['clamp_s'], clamp_t=t['clamp_t'],
                     mip_chain=t['has_mip_chain'], spec0_flags='0x%x' % (t['specs'][0]['flags'] if t['specs'][0] else 0),
                     has_alpha=bool((img[..., 3] < 255).any()))
        else:
            e['texture'] = None
        self.mat_json[key] = e
        return key

    def mesh_uv(self, mat, st):
        t = mat['texture'] if mat else None
        if not t or t['texture_index'] is None:
            return np.zeros_like(st)
        su = 2.0 if t['mirror_s'] else 1.0
        sv = 2.0 if t['mirror_t'] else 1.0
        u = st[:, 0] / (4096.0 * su)
        v = st[:, 1] / (4096.0 * sv)
        if t['res'][0]:
            u = u - mat['tex_offset'][0] / float(t['res'][0]) / su
        if t['res'][1]:
            v = v - mat['tex_offset'][1] / float(t['res'][1]) / sv
        return np.stack([u, v], 1)  # v down (image rows); OBJ writes 1 - v

    # --- OBJ writers ------------------------------------------------------------------------
    def write_visual(self, items, base):
        """Returns arrays for preview: P, T, UV, tex index per tri, list of images."""
        M = self.M
        obj = ['# Star Wars Episode I: Racer (N64 USA) model %d, %s' % (M.index, base),
               '# game units, +Z up; v = position + vertex colour (0..1); vt v axis: 1 - t/4096',
               'mtllib %s.mtl' % base]
        P_all, T_all, UV_all, tex_of, imgs, img_idx = [], [], [], [], [], {}
        nv = 0
        nvt = 0
        nvn = 0
        tri_count = 0
        mats_used = set()
        for m, W, node, mirrored in items:
            vt = M.visual_tris(m)
            if vt is None or not len(vt['tris']):
                continue
            mat = m['material']
            mk = self.material_entry(mat) if mat else None
            if mk:
                mats_used.add(mk)
            P = Mo.apply(W, vt['pos'])
            UV = self.mesh_uv(mat, vt['st'])
            normals = bool(mat and (mat['flags'] & 0x11))
            obj.append('g mesh_%06x' % m['off'])
            if mk:
                obj.append('usemtl %s' % mk)
            rgba = vt['rgba']
            for i, p in enumerate(P):
                if normals:
                    obj.append('v %.3f %.3f %.3f' % tuple(p))
                else:
                    c = rgba[i] / 255.0
                    obj.append('v %.3f %.3f %.3f %.4f %.4f %.4f' % (p[0], p[1], p[2], c[0], c[1], c[2]))
            for uv in UV:
                obj.append('vt %.5f %.5f' % (uv[0], 1.0 - uv[1]))
            if normals:
                Nn = rgba[:, :3].astype(np.int8).astype(float) / 127.0
                Nn = Nn @ W[:3, :3]
                for nn in Nn:
                    l = np.linalg.norm(nn) or 1.0
                    obj.append('vn %.4f %.4f %.4f' % tuple(nn / l))
            Tm = vt['tris']
            if mirrored:
                Tm = Tm[:, ::-1]
            for a, b, c in Tm:
                if normals:
                    obj.append('f %d/%d/%d %d/%d/%d %d/%d/%d' % (
                        a + nv + 1, a + nvt + 1, a + nvn + 1, b + nv + 1, b + nvt + 1, b + nvn + 1,
                        c + nv + 1, c + nvt + 1, c + nvn + 1))
                else:
                    obj.append('f %d/%d %d/%d %d/%d' % (a + nv + 1, a + nvt + 1, b + nv + 1, b + nvt + 1,
                                                        c + nv + 1, c + nvt + 1))
            # preview data
            ti = -1
            t = mat['texture'] if mat else None
            if t and t['texture_index'] is not None:
                fn, img = self.texture_file(t)
                ua = self.mat_json[mk].get('uses_texture_alpha', False)
                if (fn, ua) not in img_idx:
                    img_idx[(fn, ua)] = len(imgs)
                    imgs.append(img if ua else np.ascontiguousarray(img[..., :3]))
                ti = img_idx[(fn, ua)]
            base_i = sum(len(x) for x in P_all)
            P_all.append(P)
            UV_all.append(UV)
            T_all.append(Tm + base_i)
            tex_of.extend([ti] * len(Tm))
            nv += len(P)
            nvt += len(P)
            if normals:
                nvn += len(P)
            tri_count += len(Tm)
        with open(os.path.join(self.dir, base + '.obj'), 'w') as f:
            f.write('\n'.join(obj) + '\n')
        mtl = ['# materials for %s.obj; full N64 state in materials.json' % base]
        for k in sorted(mats_used):
            e = self.mat_json[k]
            mtl += ['newmtl %s' % k, 'Kd 1 1 1', 'Ka 0 0 0', 'Ks 0 0 0', 'illum 1']
            if e.get('texture'):
                mtl.append('map_Kd textures/%s' % e['texture'])
                if e.get('has_alpha') and e.get('uses_texture_alpha'):
                    mtl.append('map_d textures/%s' % e['texture'])
            mtl.append('')
        with open(os.path.join(self.dir, base + '.mtl'), 'w') as f:
            f.write('\n'.join(mtl))
        if P_all:
            return (np.vstack(P_all), np.vstack(T_all), np.vstack(UV_all), np.array(tex_of), imgs, tri_count)
        return (np.zeros((0, 3)), np.zeros((0, 3), int), np.zeros((0, 2)), np.zeros(0, int), [], 0)

    def write_collision(self, items):
        M = self.M
        obj = ['# collision triangles, model %d; game units, +Z up' % M.index]
        surf = []
        nv = 0
        ntri = 0
        Pc, Tc, Sc = [], [], []
        trig_out = []
        for m, W, node in items:
            V, T = M.collision_tris(m)
            if not len(T):
                continue
            P = Mo.apply(W, V)
            b = m['behaviour']
            sname = 'surf_' + ('_'.join(b['surface']) if b and b['surface'] else 'none')
            obj.append('g col_%06x' % m['off'])
            obj.append('usemtl %s' % sname)
            for p in P:
                obj.append('v %.3f %.3f %.3f' % tuple(p))
            for a, bb, c in T:
                obj.append('f %d %d %d' % (a + nv + 1, bb + nv + 1, c + nv + 1))
            rec = dict(mesh='0x%x' % m['off'], node='0x%x' % node['off'], obj_group='col_%06x' % m['off'],
                       tri_start=ntri, tri_count=int(len(T)), node_flags1='0x%x' % node['flags1'],
                       node_flags2='0x%x' % node['flags2'], has_visual=bool(m['dl_ptr']))
            if b:
                bj = {k: v for k, v in b.items() if k not in ('triggers',)}
                bj['off'] = '0x%x' % b['off']
                bj['vehicle_reaction'] = '0x%x' % b['vehicle_reaction']
                bj['word00_flags'] = [n for bit, n in ((0x8, 'spline_ambiguous'), (0x10, 'force_full_raycast'),
                                                        (0x20, 'magnet_gravity')) if b['word00'] & bit]
                if 'unk20_node' in bj and bj['unk20_node']:
                    bj['unk20_node'] = '0x%x' % bj['unk20_node']
                bj['trigger_count'] = len(b['triggers'])
                rec['surface'] = bj
                for t in b['triggers']:
                    tj = dict(t)
                    tj['off'] = '0x%x' % t['off']
                    tj['mesh'] = '0x%x' % m['off']
                    an = t['affected_node']
                    if an:
                        tj['affected_node'] = '0x%x' % an
                        tj['affected_node_type'] = Mo.NODE_NAMES.get(M.u32(an), '?') if an + 4 <= len(M.d) else '?'
                    trig_out.append(tj)
            surf.append(rec)
            Pc.append(P)
            Tc.append(T + sum(len(x) for x in Pc[:-1]))
            Sc.extend([b['surface'] if b else []] * len(T))
            nv += len(P)
            ntri += len(T)
        with open(os.path.join(self.dir, 'collision.obj'), 'w') as f:
            f.write('\n'.join(obj) + '\n')
        # dedupe triggers that are shared by several meshes
        seen = {}
        for t in trig_out:
            if t['off'] in seen:
                seen[t['off']]['meshes'].append(t['mesh'])
            else:
                t['meshes'] = [t.pop('mesh')]
                seen[t['off']] = t
        trigs = list(seen.values())
        P = np.vstack(Pc) if Pc else np.zeros((0, 3))
        T = np.vstack(Tc) if Tc else np.zeros((0, 3), int)
        return surf, trigs, P, T, Sc, ntri

    def write_splines(self):
        s = S.load(self.tr['spline_id'], self.sb)
        g = S.graph_info(s)
        obj = ['# spline %d %s: Bezier polylines per segment (a->b)' % (s['id'], s['name'])]
        nv = 0
        polys = []
        for a, b in S.segments(s):
            C = S.curve(s, a, b, 12)
            obj.append('o seg_%d_%d' % (a, b))
            for p in C:
                obj.append('v %.3f %.3f %.3f' % tuple(p))
            obj.append('l ' + ' '.join(str(nv + k + 1) for k in range(len(C))))
            nv += len(C)
            polys.append((a, b, C))
        with open(os.path.join(self.dir, 'splines.obj'), 'w') as f:
            f.write('\n'.join(obj) + '\n')
        out = dict(id=s['id'], name=s['name'], type=s['type'],
                   type_meaning={0: 'uniform cubic B-spline', 1: 'Bezier, flat (+Z up)',
                                 2: 'Bezier with per-point up vector'}.get(s['type']),
                   point_count=s['point_count'], segment_count=s['segment_count'], unk02=s['unk02'],
                   graph=g, points=s['points'],
                   notes=['segment a->b: P0=a.position, P1=a.handle_out, P2=b.handle_in, P3=b.position',
                          'progress: lap-progress rank, 0 at start/finish (point 0), equal on parallel branches',
                          'branch_table: [0] own index, [1] split id on 2-way splits (game use partly unknown)'])
        with open(os.path.join(self.dir, 'splines.json'), 'w') as f:
            json.dump(out, f, indent=1)
        return s, g, polys

    # --- run ------------------------------------------------------------------------------
    def run(self, preview=True):
        M, tr = self.M, self.tr
        root, sky = M.header_nodes[0], M.header_nodes[2]
        vis, col, hv, hc = self.collect(root)
        Pv, Tv, UVv, texv, imgs, ntri_v = self.write_visual(vis, 'geometry')
        sky_items = self.collect(sky)[0] if sky else []
        sky_res = self.write_visual(sky_items, 'skybox') if sky else None
        surf, trigs, Pc, Tc, Sc, ntri_c = self.write_collision(col)
        s, g, polys = self.write_splines()
        cams = CAM.intro_camera(M, tr, self.sb)
        cam_json = dict(intro=cams,
                        other_camera_splines=CAM.other_camera_splines(s['name'], self.sb),
                        animated_empties=self.animated_empties(),
                        not_decoded=['field of view', 'camera roll (look-at assumed)',
                                     'in-race chase camera (code, not track data)'])
        with open(os.path.join(self.dir, 'cameras.json'), 'w') as f:
            json.dump(cam_json, f, indent=1)
        with open(os.path.join(self.dir, 'collision_surfaces.json'), 'w') as f:
            json.dump(dict(notes=['tri_start/tri_count index the faces of collision.obj in file order',
                                  'vehicle_reaction bits: ' + ', '.join('%s=0x%x' % (n, b) for b, n in Mo.VEHICLE_REACTION),
                                  'meanings (partly guesses): ' + json.dumps(Mo.VEHICLE_REACTION_MEANING),
                                  'unk20..unk38 not understood; unk20 is a node pointer when the relocation mask says so'],
                           meshes=surf, surface_summary=self.surface_summary(surf)), f, indent=1)
        with open(os.path.join(self.dir, 'triggers.json'), 'w') as f:
            json.dump(dict(notes=['center/direction in world units; size_xy, size_z = volume half extents (GUESS: '
                                  'extent semantics not verified)', 'flags: ' + str(Mo.TRIGGER_FLAGS)],
                           triggers=trigs), f, indent=1)
        with open(os.path.join(self.dir, 'materials.json'), 'w') as f:
            json.dump(self.mat_json, f, indent=1)
        bb = (Pv.min(0).tolist(), Pv.max(0).tolist()) if len(Pv) else None
        meta = dict(model_index=M.index, name=tr['name'], track_table_index=tr['table_index'],
                    planet=tr['planet'], planet_index=tr['planet_index'],
                    planet_track_number=tr['track_number'], favourite_pilot_index=tr['favourite_pilot'],
                    spline_id=tr['spline_id'], spline_name=s['name'],
                    track_table_source='N64 ROM 0x%X (25 x {u32 model, u32 spline, u8 track no, u8 planet, '
                                       'u8 favourite pilot, u8 pad})' % TRACK_TABLE_ROM,
                    header_nodes=['0x%x' % x for x in M.header_nodes],
                    shared_model_visibility_rule=('node visible for planet track n when node.flags1 & (0x10 << n); '
                                                  'collision when node.flags2 & (0x10 << n). Inferred from data: on the shared-geometry '
                                                  'models (Aquilaris, Mon Gazza planets) the hidden pieces are barriers lying '
                                                  'across this track racing line (checked by eye on 133 and 143)'),
                    hidden_by_track_number=dict(visual_meshes=hv, collision_meshes=hc),
                    counts=dict(nodes=len(M.nodes), meshes=len(M.meshes), visual_meshes=len(vis),
                                visual_triangles=int(ntri_v), collision_meshes=len(surf),
                                collision_triangles=int(ntri_c), materials=len(self.mat_json),
                                textures=len(self.tex_files), triggers=len(trigs),
                                spline_points=s['point_count'], spline_forks=len(g['forks']),
                                spline_joins=len(g['joins']), animations=len(M.anims),
                                skybox_triangles=int(sky_res[5]) if sky_res else 0),
                    spline_closed=g['closed_loop'],
                    bbox_visual=bb, light_streaks=M.light_streaks,
                    intro_camera_status=cams.get('status'),
                    warnings=sorted(set(M.warnings))[:50], problems=self.problems,
                    units='game units, right-handed, +Z up')
        with open(os.path.join(self.dir, 'track.json'), 'w') as f:
            json.dump(meta, f, indent=1)
        if preview:
            import previews as PV
            PV.perspective(self.dir, Pv, Tv, UVv, texv, imgs, s, Pc, Tc)
            PV.top(self.dir, Pc, Tc, Sc, s, g, polys, trigs)
            PV.camera(self.dir, Pc, Tc, s, polys, cams, vis=(Pv, Tv, UVv, texv, imgs))
        return meta

    def surface_summary(self, surf):
        out = {}
        for r in surf:
            k = '+'.join(r['surface']['surface']) if 'surface' in r and r['surface']['surface'] else 'none'
            out[k] = out.get(k, 0) + r['tri_count']
        return out

    def animated_empties(self):
        M = self.M
        out = []
        for a in M.anims:
            if a['type'] not in (8, 9, 10):
                continue
            out.append(dict(target='0x%x' % a['target'], type=a['type_name'], keys=a['keyframes'],
                            duration=a['duration'], flags='0x%x' % a['flags'],
                            times=a['times'], values=a['values']))
        return out


def main(argv):
    tt = track_table()
    preview = '--no-preview' not in argv
    args = [a for a in argv if not a.startswith('--')]
    if not args or args[0] == 'all':
        todo = tt
    else:
        todo = [t for t in tt if t['model'] in [int(x) for x in args]]
    mb, sb, tex = B.ModelBlock(), B.SplineBlock(), TX.Textures()
    for t in todo:
        e = Exporter(t, mb, sb, tex)
        meta = e.run(preview)
        c = meta['counts']
        print('%3d %-28s vis %6d tris  col %6d tris  tex %3d  trig %3d  spline %3d pts %d forks  cam %s  warn %d prob %d' % (
            t['model'], t['name'], c['visual_triangles'], c['collision_triangles'], c['textures'], c['triggers'],
            c['spline_points'], c['spline_forks'], meta['intro_camera_status'], len(meta['warnings']),
            len(meta['problems'])))


if __name__ == '__main__':
    main(sys.argv[1:])
