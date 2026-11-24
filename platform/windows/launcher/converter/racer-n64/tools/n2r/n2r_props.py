r"""Models the N64 game code places on a track at race load (not in the track model), at their load state,
as extra visual geometry groups in the n2r_io layout (N64 space). One table for every track, keyed on the
trigger type as the original's set-up pass is (0x8007D418..0x8007D9F0; Ando trigger models 0x8007E180).

Evidence (work\bwr\n64-surf\slalom_poles.py, notes\n2r-bwr.md):
  * 108 slalom gate: per trigger a node (0x8007D310 -> 0x8007BB28), translation = trigger centre + (8, 0, -24)
    in world axes, no rotation (0x8007D418..0x8007D464); child = model 192 selector child 0x280 (upright flag,
    3 triangles, texture 916). On pass (0x8007C64C) the flag falls (model 191, 2.2 s once) and then lies flat
    along the pod's travel for the rest of the race: not expressible as static data, the load state is kept.
  * 211 flame / 310 dozer (Mon Gazza, n64-loader-re.md 10.2): the flame is hidden at load; the dozer is an
    animated model (looping) on the affected node: not placed here (Revenge animations are a port gap).
Models with no collision: the props hang off the trigger scene node, not under the track root the
collision walkers descend (inferred, see notes).

Models the race load places by planet / track (the original's own branch on the same data, judge set-up):
  * planet 4 track 0 (Baroo Coast): model 281, the sea plane (0x8005F02C..0x8005F09C: loaded only when judge +0x1AC
    (planet) == 4 and +0x1C0 (planet track) == 0; root stored in scene slot 10 (0x8011AC88), flags1 |= 3 (drawn) by
    0x800181BC; identity transform, so model 281 sits in track coordinates). The collision walkers take the track
    root only (pod +0x13C = judge +0x30 = 0x8011AC8C, 0x8005F1DC..0x8005F1F8 / pod init 0x8007316C): NO collision.
    Its material is texture-flipbooked (anim type 2, 9 keys over 2.0 s, ping-pong 1633-1634-1635-1636-1638, LOOP,
    registered at load by 0x80030964): written at the frame shown at t = 0 (texture 1633) [flag: the flipbook is
    static; Revenge has PSM TextureAnimation, not written by the converter].
"""
import os
import sys

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.normpath(os.path.join(HERE, '..', 'n64r')))

PROP_RULES = {
    108: dict(model=192, node=0x280, offset=(8.0, 0.0, -24.0), what='slalom gate flag (upright, load state)'),
}
TRACK_MODELS = {   # (planet index, planet track number) -> model placed at identity by the race load
    (4, 0): dict(model=281, node=None, what='sea plane (judge set-up 0x8005F02C, scene slot 10, drawn, no collision); flipbook '
                 'material at its t = 0 frame (static)'),
}


def _material_entry(mat, tex_png):
    t = mat['texture']
    e = dict(name='p%03d_%06x' % (0, mat['off']), mesh_material_flags=mat['flags'], texture_offset=mat['tex_offset'],
             cull=('back' if mat['flags'] & 0x8 else 'front' if mat['flags'] & 0x40 else 'none'),
             combine_words=['0x%08x' % x for x in mat.get('combine', [])],
             render_mode_words=['0x%08x' % x for x in mat.get('render_mode', [])], prim_color=mat.get('prim_color'))
    if t and t['texture_index'] is not None:
        e.update(texture=tex_png, texture_index=t['texture_index'], width=t['width'], height=t['height'],
                 mirror_s=t['mirror_s'], mirror_t=t['mirror_t'], clamp_s=t['clamp_s'], clamp_t=t['clamp_t'])
    else:
        e['texture'] = None
    return e


def _flipbook_frame0(M):
    """{material offset: MaterialTexture record shown at t = 0} for the model's texture flipbooks (type 2: the target is a
    pointer slot holding the material, the values are MaterialTexture records, key 0 at time 0)."""
    import struct
    out = {}
    for a in M.anims:
        if a['type'] == 0x2 and a['target'] and a['values']:
            mat = struct.unpack_from('>I', M.d, a['target'])[0]
            out[mat] = M.material_texture(a['values'][0])
    return out


def _model_local(M, root, tag, out_dir, tex_box, frame0=None):
    import model as Mo
    import textures as TX
    local, mats = [], {}
    for n, W, path in M.walk(root, 'all'):
        if n['type'] != Mo.NODE_MESHGROUP:
            continue
        for m in n['meshes']:
            if not m['dl_ptr']:
                continue
            v = M.visual_tris(m)
            if v is None or not len(v['tris']):
                continue
            mat = m['material']
            assert not (mat['flags'] & 0x11), 'prop material with vertex normals'
            t = mat['texture']
            if frame0 and mat['off'] in frame0:
                t = frame0[mat['off']]
                mat = dict(mat, texture=t)
            png = None
            if t and t['texture_index'] is not None:
                tex_box[0] = tex_box[0] or TX.Textures()
                img, note = tex_box[0].get(t['texture_index'], t['format'], t['width'], t['height'])
                img = TX.mirror_bake(img, t['mirror_s'], t['mirror_t'])
                os.makedirs(out_dir, exist_ok=True)
                png = os.path.join(out_dir, 'tex%04d_prop%d.png' % (t['texture_index'], tag))
                Image.fromarray(img).save(png)
                su = 2.0 if t['mirror_s'] else 1.0; sv = 2.0 if t['mirror_t'] else 1.0
                uv = np.stack([v['st'][:, 0] / (4096.0 * su), v['st'][:, 1] / (4096.0 * sv)], 1)
                if t['res'][0]:
                    uv[:, 0] -= mat['tex_offset'][0] / float(t['res'][0]) / su
                if t['res'][1]:
                    uv[:, 1] -= mat['tex_offset'][1] / float(t['res'][1]) / sv
            else:
                uv = np.zeros((len(v['pos']), 2))
            key = 'prop%d_m%06x' % (tag, mat['off'])
            mats[key] = _material_entry(mat, png)
            local.append((Mo.apply(W, v['pos']), uv, v['rgba'][:, :3] / 255.0, v['tris'], key))
    return local, mats


def track_models(D, out_dir):
    """models the race load places for this planet / track (TRACK_MODELS) -> (groups, materials, report)"""
    import model as Mo
    tr = D['track']
    rule = TRACK_MODELS.get((int(tr['planet_index']), int(tr['planet_track_number'])))
    if not rule:
        return [], {}, []
    M = Mo.Model(rule['model'])
    root = M.nodes[rule['node']] if rule['node'] else M.nodes[M.header_nodes[0]]
    local, mats = _model_local(M, root, rule['model'], out_dir, [None], _flipbook_frame0(M))
    groups = [dict(name='model%d_%02d' % (rule['model'], i), mtl=key, P=P[T], C=C[T], UV=UV[T]) for i, (P, UV, C, T, key) in enumerate(local)]
    rep = [dict(model=rule['model'], planet=int(tr['planet_index']), track=int(tr['planet_track_number']), what=rule['what'],
                triangles=int(sum(len(x[3]) for x in local)), materials=sorted(mats),
                textures=sorted(set(m.get('texture_index') for m in mats.values() if m.get('texture_index') is not None)))]
    return groups, mats, rep


def props(D, out_dir):
    """-> (groups, materials, report). groups: n2r_io read_obj layout (P (m,3,3), C (m,3,3), UV (m,3,2) image-v-down),
    materials keyed by group mtl; textures written into out_dir (absolute paths)."""
    import model as Mo
    import textures as TX
    groups, mats, rep = [], {}, []
    tex = None
    for ttype, rule in PROP_RULES.items():
        trs = [t for t in D['triggers']['triggers'] if t['type'] == ttype]
        if not trs:
            continue
        M = Mo.Model(rule['model'])
        local = []
        for n, W, path in M.walk(M.nodes[rule['node']], 'all'):
            if n['type'] != Mo.NODE_MESHGROUP:
                continue
            for m in n['meshes']:
                if not m['dl_ptr']:
                    continue
                v = M.visual_tris(m)
                if v is None or not len(v['tris']):
                    continue
                mat = m['material']
                assert not (mat['flags'] & 0x11), 'prop material with vertex normals'
                t = mat['texture']
                png = None
                if t and t['texture_index'] is not None:
                    tex = tex or TX.Textures()
                    img, note = tex.get(t['texture_index'], t['format'], t['width'], t['height'])
                    img = TX.mirror_bake(img, t['mirror_s'], t['mirror_t'])
                    os.makedirs(out_dir, exist_ok=True)
                    png = os.path.join(out_dir, 'tex%04d_prop%d.png' % (t['texture_index'], rule['model']))
                    Image.fromarray(img).save(png)
                    su = 2.0 if t['mirror_s'] else 1.0; sv = 2.0 if t['mirror_t'] else 1.0
                    uv = np.stack([v['st'][:, 0] / (4096.0 * su), v['st'][:, 1] / (4096.0 * sv)], 1)
                    if t['res'][0]:
                        uv[:, 0] -= mat['tex_offset'][0] / float(t['res'][0]) / su
                    if t['res'][1]:
                        uv[:, 1] -= mat['tex_offset'][1] / float(t['res'][1]) / sv
                else:
                    uv = np.zeros((len(v['pos']), 2))
                key = 'prop%d_m%06x' % (rule['model'], mat['off'])
                mats[key] = _material_entry(mat, png)
                local.append((Mo.apply(W, v['pos']), uv, v['rgba'][:, :3] / 255.0, v['tris'], key))
        for i, tr in enumerate(trs):
            off = np.array(tr['center'], float) + np.array(rule['offset'])
            for P, UV, C, T, key in local:
                groups.append(dict(name='prop%d_%d_%02d' % (rule['model'], ttype, i), mtl=key, P=(P + off)[T], C=C[T], UV=UV[T]))
        rep.append(dict(trigger_type=ttype, model=rule['model'], node=hex(rule['node']), what=rule['what'], count=len(trs),
                        triangles_each=int(sum(len(x[3]) for x in local)), materials=sorted(set(x[4] for x in local))))
    return groups, mats, rep
