#!/usr/bin/env python3
r"""Intro (pre-race fly-in) camera of a track, plus the other camera-type data.

What the data shows (notes\formats.md, section Cameras):
- Trak header slots +0x10/+0x14/+0x18 point at "trackpoint" empties (TransformedWithPivot nodes,
  flags3 0xB) whose translation animation is the LOOK-AT TARGET of the intro shot. The slot used is
  the track's planetTrackNumber (N64 track table), or the only filled slot when the model has one.
- The camera EYE path is the spline-block camera spline <planet>[n]_track{6came,7camm,8camh}
  (slot 0, 1, 2). On 17 of 25 tracks the model also has an eye empty (root-level
  TransformedWithPivot, flags3 0x3) whose translation keys are that spline sampled with the
  game's real timing (checked: exact to 0.2 units on 115, 1, 139, 141; with ease-in/out or a hold
  on 128, 129, 148). Where no eye empty exists, the eye time mapping is a GUESS: uniform per
  segment over the target animation's duration (which is exact on 115, 1, 139, 141).
- Eye empties may also carry an axis-angle animation (vec4 axis xyz + angle in degrees); it is
  exported raw. The camera direction we export is look-at (eye -> target), roll 0 (GUESS for roll).
- Field of view is not in the track data (not decoded).
"""
import math
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import model as Mo  # noqa: E402
import splines as S  # noqa: E402

CAM_SUFFIX = ['track6came', 'track7camm', 'track8camh']


def camera_spline_id(track_spline_name, slot):
    """Name rule (SPLINEID names of the PC decompilation): planetX[n]_track[k] -> planetX[n]_<suffix>."""
    prefix = track_spline_name.split('_track')[0]
    for pre in (prefix, prefix.rstrip('0123456789')):
        name = '%s_%s' % (pre, CAM_SUFFIX[slot])
        if name in S.SPLINE_NAMES:
            return S.SPLINE_NAMES.index(name)
    return None


def sample_keys(times, values, t):
    times = np.asarray(times, float)
    values = np.asarray(values, float)
    if len(times) == 1 or t <= times[0]:
        return values[0]
    if t >= times[-1]:
        return values[-1]
    k = int(np.searchsorted(times, t, side='right') - 1)
    f = (t - times[k]) / max(times[k + 1] - times[k], 1e-9)
    return values[k] * (1 - f) + values[k + 1] * f


def spline_at(s, u):
    n = len(s['points']) - 1
    u = min(max(u, 0.0), float(n))
    k = min(int(u), n - 1)
    f = u - k
    pa, pb = s['points'][k], s['points'][k + 1]
    P0, P1 = np.array(pa['position']), np.array(pa['handle_out'])
    P2, P3 = np.array(pb['handle_in']), np.array(pb['position'])
    return ((1 - f) ** 3) * P0 + 3 * ((1 - f) ** 2) * f * P1 + 3 * (1 - f) * f * f * P2 + f ** 3 * P3


def axis_angle_matrix(ax, deg):
    ax = np.asarray(ax, float)
    n = np.linalg.norm(ax)
    if n < 1e-9:
        return np.eye(3)
    x, y, z = ax / n
    a = math.radians(deg)
    c, s, C = math.cos(a), math.sin(a), 1 - math.cos(a)
    return np.array([[c + x * x * C, x * y * C - z * s, x * z * C + y * s],
                     [y * x * C + z * s, c + y * y * C, y * z * C - x * s],
                     [z * x * C - y * s, z * y * C + x * s, c + z * z * C]])


def intro_camera(M, track, sb, fps=30.0):
    """M: model.Model, track: dict(track_number, spline_id). Returns a JSON-able dict."""
    slots = M.header_nodes[3:6] + [0] * (3 - len(M.header_nodes[3:6]))
    tn = track['track_number']
    filled = [k for k in range(3) if slots[k]]
    if tn < 3 and slots[tn]:
        slot = tn
        rule = 'slot = planetTrackNumber'
    elif len(filled) == 1:
        slot = filled[0]
        rule = 'only filled slot'
    else:
        return dict(status='none', reason='no trackpoint slot for track number %d' % tn)
    target_node = slots[slot]
    anims_by = {}
    for a in M.anims:
        anims_by.setdefault(a['target'], []).append(a)
    tgt = [a for a in anims_by.get(target_node, []) if a['type'] == 9]
    tnode = M.nodes.get(target_node)
    out = dict(status='decoded', slot=slot, slot_rule=rule, target_node='0x%x' % target_node)
    if tgt:
        ta = tgt[0]
        out['target_keys'] = dict(times=ta['times'], positions=ta['values'], interpolation='linear (assumed)')
        duration = ta['duration']
    else:
        p = tnode['matrix'][9:12] if tnode and 'matrix' in tnode else [0, 0, 0]
        out['target_keys'] = dict(times=[0.0], positions=[p], interpolation='static')
        duration = 10.0
    out['duration_s'] = duration
    track_spline_name = S.SPLINE_NAMES[track['spline_id']]
    cid = camera_spline_id(track_spline_name, slot)
    cs = S.load(cid, sb) if cid is not None else None
    out['camera_spline'] = dict(id=cid, name=S.SPLINE_NAMES[cid] if cid is not None else None,
                                rule='name rule planetX[n]_track{6came,7camm,8camh} by slot')
    # eye empty: root-level animated pivot node whose translation starts at the spline's point 0
    eye_node, eye_tr, eye_rot = None, None, None
    root = M.nodes[M.header_nodes[0]]
    cands = []
    for c in root['children']:
        if c['type'] != Mo.NODE_TRANSFORMED_PIVOT or c['off'] in slots:
            continue
        tr = [a for a in anims_by.get(c['off'], []) if a['type'] == 9]
        if tr and cs is not None:
            d0 = np.linalg.norm(np.array(tr[0]['values'][0]) - np.array(cs['points'][0]['position']))
            d1 = np.linalg.norm(np.array(tr[0]['values'][-1]) - np.array(cs['points'][-1]['position']))
            if min(d0, d1) < 10:
                cands.append((d0 + d1, c, tr[0]))
    if cands:
        cands.sort(key=lambda x: x[0])
        _, c, eye_tr = cands[0]
        eye_node = c['off']
        rot = [a for a in anims_by.get(eye_node, []) if a['type'] == 8]
        eye_rot = rot[0] if rot else None
    if eye_tr is not None:
        out['eye_source'] = 'eye empty 0x%x translation keys (game timing)' % eye_node
        out['eye_keys'] = dict(times=eye_tr['times'], positions=eye_tr['values'])
        duration = max(duration, eye_tr['duration'])
        eye_fn = lambda t: sample_keys(eye_tr['times'], eye_tr['values'], t)  # noqa: E731
    elif cs is not None:
        nseg = len(cs['points']) - 1
        out['eye_source'] = ('camera spline %d sampled uniformly per segment over %.2f s '
                             '(GUESS: no eye empty in this model)' % (cid, duration))
        eye_fn = lambda t: spline_at(cs, t / duration * nseg)  # noqa: E731
    else:
        out['status'] = 'partial'
        out['eye_source'] = 'none found'
        eye_fn = None
    if eye_rot is not None:
        out['eye_axis_angle_keys'] = dict(times=eye_rot['times'], axis_angle_deg=eye_rot['values'],
                                          node='0x%x' % eye_node)
    # per-frame path
    frames = []
    n = int(round(duration * fps)) + 1
    for i in range(n):
        t = i / fps
        tp = sample_keys(out['target_keys']['times'], out['target_keys']['positions'], t)
        if eye_fn is None:
            continue
        e = eye_fn(t)
        d = tp - e
        dn = np.linalg.norm(d)
        frames.append(dict(t=round(t, 4), eye=[round(float(x), 3) for x in e],
                           target=[round(float(x), 3) for x in tp],
                           look=[round(float(x), 5) for x in (d / dn if dn > 1e-6 else d)]))
    out['frames_fps'] = fps
    out['frames'] = frames
    return out


def other_camera_splines(track_spline_name, sb):
    """Camera-type splines that belong to the same planet prefix (all three slots, the closed
    '_track4/_track5' loops and cutscene paths are listed by name only)."""
    prefix = track_spline_name.split('_track')[0]
    base = prefix.rstrip('0123456789')
    out = []
    for i, n in enumerate(S.SPLINE_NAMES):
        if n.split('_track')[0] in (prefix, base) and n != track_spline_name and (
                'cam' in n or n.endswith('track4') or n.endswith('track5')):
            s = S.load(i, sb)
            out.append(dict(id=i, name=n, points=s['point_count'],
                            closed=S.graph_info(s)['closed_loop'],
                            polyline=[p['position'] for p in s['points']]))
    return out
