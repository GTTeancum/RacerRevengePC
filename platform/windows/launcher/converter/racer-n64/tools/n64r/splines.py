#!/usr/bin/env python3
r"""Spline block records (racing line / AI path / lap progress / camera paths).

Header 0x10: u16 type (0 B-spline, 1 Bezier flat, 2 Bezier with per-point up), u16 unk,
u32 point count, u32 segment count, u32 runtime slot.
Point 0x54 (84 bytes): u16 next_count, u16 prev_count, u16 next[2], u16 prev[4],
vec3 position @0x10, vec3 rotation/up @0x1C, vec3 handle1 (in) @0x28, vec3 handle2 (out) @0x34,
u16 progress @0x40, u16 branch table[8] @0x42, u16 unk @0x52.
Segment a->b (cubic Bezier): P0 = a.pos, P1 = a.handle2, P2 = b.handle1, P3 = b.pos.

Names: SPLINEID enum of the SW_RACER_RE PC decompilation (same numbering on N64; the N64 bytes
differ from PC).
"""
import struct
import sys
import os

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import blocks as B  # noqa: E402

SPLINE_NAMES = [
    'planetd_track', 'planetd_track2', 'planete_track', 'planetf_track', 'planetf_track2',
    'planetf_track3', 'planeth_track', 'planetj_track', 'planetj_track2', 'planetj_track3',
    'tatooine_track', 'planeta1_track', 'planeta_track', 'planeta_track2', 'planeta_track3',
    'planeta_track5', 'planetb_track', 'planeta_track6came', 'planetb_track2', 'planetb_track3',
    'planetb_track5', 'planeta2_track2', 'planeta3_track3', 'cutscene_intro25_scene',
    'planetb1_track', 'tatooine_mini_track6came', 'planetb2_track2', 'tatooine_track4',
    'planetb3_track3', 'planetc1_track', 'planeta_track7camm', 'planeta_track8camh',
    'planetc2_track2', 'planetc3_track3', 'planetb1_track6came', 'planetj1_track6came',
    'planetc_track6came', 'cutscene22_scene', 'planetc_track7camm', 'cutscene27_scene',
    'planetc_track8camh', 'planetf_track6came', 'planetf_track7camm', 'planetf_track8camh',
    'planetd2_track2', 'tatooine_track7camm', 'planeth_track6came', 'planeti_track6came',
    'planetb3_track8camh', 'planetd1_track', 'planeti_track', 'planetd3_track3', 'planete1_track',
    'planete2_track2', 'planete3_track3', 'planeti_track2', 'planete1_track6came',
    'planete2_track7camm', 'planetj2_track2', 'planetj2_track7camm', 'planetj3_track3',
    'planetc_track', 'planetj3_track8camh', 'planetc_track2', 'planetc_track3', 'planetf1_track',
    'planetf2_track2', 'planetf3_track3', 'planetj1_track', 'planetb3_track6came',
    'planetc1_track6came', 'planetc2_track7camm', 'planetf1_track6came', 'planetf2_track7camm',
    'tatooine_mini_track', 'tatooine_mini_track4', 'planetf3_track8camh', 'cutscene_intro19_scene',
    'cutscene_intro30_scene', 'planeta1_track6came', 'planeta2_track7camm', 'planeta3_track8camh',
    'planetb2_track7camm', 'planetd1_track6came', 'planete3_track8camh', 'planetd2_track',
    'planetd2_track3', 'planetd2_track6came', 'planetd2_track7camm', 'planetd3_track8camh',
    'planetc3_track8camh']
assert len(SPLINE_NAMES) == 91


def parse(raw):
    typ, unk, npts, nseg, slot = struct.unpack_from('>HHIII', raw, 0)
    pts = []
    for i in range(npts):
        o = 0x10 + 0x54 * i
        nc, pc = struct.unpack_from('>HH', raw, o)
        nxt = list(struct.unpack_from('>2H', raw, o + 4))[:nc]
        prv = list(struct.unpack_from('>4H', raw, o + 8))[:pc]
        f = struct.unpack_from('>12f', raw, o + 0x10)
        prog = struct.unpack_from('>H', raw, o + 0x40)[0]
        br = list(struct.unpack_from('>8h', raw, o + 0x42))
        u52 = struct.unpack_from('>H', raw, o + 0x52)[0]
        pts.append(dict(index=i, next=nxt, prev=prv, position=list(f[0:3]), up=list(f[3:6]),
                        handle_in=list(f[6:9]), handle_out=list(f[9:12]), progress=prog,
                        branch_table=br, unk52=u52))
    return dict(type=typ, unk02=unk, point_count=npts, segment_count=nseg, runtime_slot=slot,
                points=pts, trailing_bytes=len(raw) - (0x10 + 0x54 * npts))


def load(i, sb=None):
    sb = sb or SplineBlock()
    s = parse(sb.item(i))
    s['id'] = i
    s['name'] = SPLINE_NAMES[i]
    return s


SplineBlock = B.SplineBlock


def segments(s):
    out = []
    for p in s['points']:
        for n in p['next']:
            out.append((p['index'], n))
    return out


def bezier(s, a, b, steps=12):
    pa, pb = s['points'][a], s['points'][b]
    P0, P1 = np.array(pa['position']), np.array(pa['handle_out'])
    P2, P3 = np.array(pb['handle_in']), np.array(pb['position'])
    t = np.linspace(0, 1, steps + 1)[:, None]
    return ((1 - t) ** 3) * P0 + 3 * ((1 - t) ** 2) * t * P1 + 3 * (1 - t) * t * t * P2 + t ** 3 * P3


def bspline(s, a, b, steps=12):
    """Type 0 (uniform cubic B-spline over prev(a), a, b, next(b)); first links used."""
    pts = s['points']
    pa = pts[a]['prev'][0] if pts[a]['prev'] else a
    nb = pts[b]['next'][0] if pts[b]['next'] else b
    P = [np.array(pts[k]['position']) for k in (pa, a, b, nb)]
    t = np.linspace(0, 1, steps + 1)[:, None]
    b0 = (1 - t) ** 3 / 6
    b1 = (3 * t ** 3 - 6 * t ** 2 + 4) / 6
    b2 = (-3 * t ** 3 + 3 * t ** 2 + 3 * t + 1) / 6
    b3 = t ** 3 / 6
    return b0 * P[0] + b1 * P[1] + b2 * P[2] + b3 * P[3]


def curve(s, a, b, steps=12):
    return bspline(s, a, b, steps) if s['type'] == 0 else bezier(s, a, b, steps)


def graph_info(s):
    pts = s['points']
    forks = [p['index'] for p in pts if len(p['next']) > 1]
    joins = [p['index'] for p in pts if len(p['prev']) > 1]
    dead = [p['index'] for p in pts if len(p['next']) == 0]
    closed = any(0 in p['next'] for p in pts)
    # main loop: follow next[0] from 0
    main = [0]
    seen = {0}
    cur = 0
    while pts[cur]['next']:
        cur = pts[cur]['next'][0]
        if cur in seen:
            break
        seen.add(cur)
        main.append(cur)
    branches = []
    for f in forks:
        for n in pts[f]['next'][1:]:
            path = [f]
            c = n
            guard = 0
            while c not in seen or c == n:
                path.append(c)
                if not pts[c]['next'] or guard > 1000:
                    break
                c = pts[c]['next'][0]
                guard += 1
                if c in seen:
                    path.append(c)
                    break
            branches.append(dict(from_point=f, to_point=path[-1], points=path))
    return dict(closed_loop=closed, forks=forks, joins=joins, dead_ends=dead, main_path=main,
                branches=branches)


def main():
    sb = SplineBlock()
    for i in range(sb.count):
        s = load(i, sb)
        g = graph_info(s)
        P = np.array([p['position'] for p in s['points']])
        print('%2d %-26s type %d pts %3d segs %3d forks %2d joins %2d closed %d trail %d  bbox %s..%s' % (
            i, s['name'], s['type'], s['point_count'], s['segment_count'], len(g['forks']), len(g['joins']),
            g['closed_loop'], s['trailing_bytes'], P.min(0).round(), P.max(0).round()))


if __name__ == '__main__':
    main()
