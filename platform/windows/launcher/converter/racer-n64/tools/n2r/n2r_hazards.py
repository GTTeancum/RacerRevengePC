r"""N64 track hazards as Revenge trigger and animation rows (converter switch --n64-hazards, default OFF).

Spec: notes\n64-hazards-spec.md. Port design (what the port reads): work\scratch\n64-hazards\NOTES.md section 7.
PORT DATA, NOT ORIGINAL REVENGE: the trigger keys N64Volume / N64Direction / N64HalfWidth / N64HalfHeight /
SurfaceObject and the action types ExplodePod / BreakObject / AnimationToggle exist only for the port's canonical-N64
hazards; an unknown action name stops the shipped game at load (NOTES.md 1), so these rows are written only behind the
switch, for a port build that has the hazard pieces.

One rule for every track:
  * trigger 104 -> ExplodePod row (plane), 102 -> BreakObject row (plane), 101 -> AnimationToggle row (capsule);
  * every 101 / 102 affected node and every looping animated node with collision -> one KeyFramed animation row
    (MCF / PSG / VUT / MOT / COL under anim\), taken out of the static world (n2r_n64col hazards mode);
  * one debris row (the planet's break model, the N64 FX slot 0) when a BreakObject row exists;
  * the carrier meshes of every trigger get their own static COL instances so SurfaceObject can name them.
"""
import math
import os
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
import n2r_space  # noqa: E402

BS = chr(92)
ROW_TYPES = {104: 'ExplodePod', 102: 'BreakObject', 306: 'Earthquake', 101: 'AnimationToggle', 307: 'AnimationToggle', 312: 'AnimationPlay', 314: 'AnimationPlay'}
# trigger flags the N64 load set-up adds (0x8007D3A0..0x8007D9F0): 312 falling pillar 0x8007D95C and 314 methane 0x8007D988
# set 0x20 (local player only)
SETUP_FLAGS = {306: 0x24, 312: 0x20, 314: 0x20}
FX_MODEL = {7: 293}                            # planet index -> FX slot 0 model the 314 handler shows (Malastare: 0x125 = 293,
                                               # planet set-up 0x8007E7A4; 0x8007CC48 moves it to the marker and starts it)
# N64 values written as the port keys (n64-hazards-spec.md 9; distances x track scale)
PILLAR_RATE = 0.6                              # 312 handler 0x8007D248: 0x8007B544(node, 0, ENABLED, 0.6)
RESPAWN_DELAY_S = 2.5                          # N64 death camera 0x800419EC: 1.0 s (set at 0x80044098) + 1.5 s (0x80041A68), then
                                               # respawn (0x80041BF8: RESPAWN_INVINC, 3.0 s); the exploding phase before it is
                                               # Revenge's own wreck sequence [flag: that phase not timed]
PLANE_TYPES = (102, 104)                       # 0x800668B8: 0x66 / 0x68 plane test, every other type the capsule
DEBRIS_MODEL = {6: 291}                         # planet index -> N64 break FX model (Oovo IV: ROM 0x8007E724 loads 0x123 into FX slot 0)
N64_TRIGGER_ROW_CAP = 32                       # port cap for N64 rows (NOTES.md 1)
ANIM_WINDOW_VERTICES = 8192                    # port dynamic-window budget (NOTES.md 4)
ANIM_WINDOW_INDICES = 32768
ANIM_WINDOW_SPANS = 64
MOTION_STEP_S = 0.5                            # door / fan keys densified to this step (N64 slerp between its keys)


def asteroid_impact_wave():
    """Decode Oovo's original sound 0x39 from the USA ROM's main ALBank."""
    import io
    import wave
    import blocks
    from pathlib import Path
    rom = (Path(blocks.ROOT) / 'rom' / 'racer.z64').read_bytes()
    control, table = 0xaf4b0, 0xb8960  # 80007B10 and 80007B9C
    def u32(offset):
        return struct.unpack_from('>I', rom, offset)[0]
    bank = control + u32(control + 4)
    instrument = control + u32(bank + 12)
    sound = control + u32(instrument + 16 + 4 * 0x39)
    wave_table = control + u32(sound + 8)
    book = control + u32(wave_table + 16)
    order, predictors = struct.unpack_from('>II', rom, book)
    size = u32(wave_table + 4)
    if rom[control:control+4] != b'B1\0\1' or rom[wave_table+8] != 0 or order != 2 or size % 9:
        raise ValueError('unsupported N64 asteroid sound bank')
    coefficients = struct.unpack_from('>%dh' % (16 * predictors), rom, book + 8)
    start = table + u32(wave_table)
    pcm = []
    history = (0, 0)
    for offset in range(start, start + size, 9):
        header = rom[offset]
        predictor = header & 15
        if predictor >= predictors:
            raise ValueError('N64 asteroid ADPCM predictor outside its book')
        row = coefficients[16*predictor:16*(predictor+1)]
        for half in (1, 5):
            residual = [((v ^ 8) - 8) << (header >> 4)
                        for byte in rom[offset+half:offset+half+4] for v in (byte >> 4, byte & 15)]
            for i, sample in enumerate(residual):
                prediction = row[i] * history[0] + row[8+i] * history[1]
                prediction += sum(residual[j] * row[7+i-j] for j in range(i))
                pcm.append(max(-32768, min(32767, sample + (prediction >> 11))))
            history = pcm[-2:]
    out = io.BytesIO()
    with wave.open(out, 'wb') as writer:
        writer.setparams((1, 2, 22050, len(pcm), 'NONE', 'not compressed'))
        writer.writeframes(struct.pack('<%dh' % len(pcm), *pcm))
    return out.getvalue()


def stem_of(code, off, kind):
    return '%s_%s_%05x' % (code, {102: 'ast', 101: 'door', 306: 'collapse', 307: 'panel', 312: 'pillar', 314: 'meth'}.get(kind, 'anim'), off)


def perm_matrix(X):
    P = np.zeros((3, 3))
    for k, p in enumerate(X['perm']):
        P[p, k] = 1.0                          # revenge axis k takes N64 axis p (row-vector: r = n @ P)
    return P


def build_psg_multi(name, surfaces, nodes=None, bones=None):
    """One node (identity, node-local positions, world 0), several surfaces (one per material), prelit colour like the
    course pieces (geo_encode.COMPONENT). surfaces: [(material, P (n,3), N (n,3), UV (n,2), C (n,4) uint8, tris)]."""
    import geo_encode
    import geo_psg
    F32 = geo_encode.F32
    allP = np.vstack([s[1] for s in surfaces]) if surfaces else np.zeros((1, 3))
    ext = float(np.abs(allP).max())
    scale = F32(max(ext, 1e-6) / 32767.0)
    while np.abs(np.round(allP / scale)).max() > 32767:
        scale = F32(scale * 1.0000001 + 1e-12)
    out_s, mats, stats = [], [], dict(triangles=0, packed_vertices=0, strips=0, unique_vertices=0, dropped=0)
    pts = []
    for surface_index, (mat, P, N, UV, C, tris) in enumerate(surfaces):
        bone_words = np.broadcast_to(16*(1+np.asarray(bones[surface_index] if bones is not None else 0)),(len(P),))
        mixed = np.any(bone_words!=bone_words[0])
        Q = np.round(P / scale).astype(int)
        NQ = np.clip(np.round(N * 127), -127, 127).astype(int)
        UQ = np.round(UV / geo_encode.UV_SCALE).astype(int)
        used = np.unique(tris)
        if np.abs(UQ[used]).max() > 32767:
            raise ValueError('%s: UV outside +-16' % name)
        key, vid, attrs = {}, np.empty(len(P), int), []
        for i in used:
            k = (tuple(Q[i]), tuple(NQ[i]), tuple(UQ[i]), tuple(int(x) for x in C[i]), int(bone_words[i]))
            if k not in key:
                key[k] = len(attrs); attrs.append(k)
            vid[i] = key[k]
        keep = []
        for t in tris:
            q = Q[t] * scale; f = P[t]
            if not mixed and np.cross(f[1] - f[0], f[2] - f[0]) @ N[t].sum(0) > 0 and np.cross(q[1] - q[0], q[2] - q[0]) @ NQ[t].sum(0) < 0:
                stats['dropped'] += 1; continue
            if len({(tuple(Q[i]),bone_words[i]) for i in t}) < 3:
                stats['dropped'] += 1; continue
            keep.append(t)
        if not keep:
            continue
        t2 = [tuple(vid[t]) for t in keep]
        strips = geo_encode.stripify(t2)
        sa = [[((attrs[v][0][0], attrs[v][0][1], attrs[v][0][2], -attrs[v][4] if j < 2 else attrs[v][4]), attrs[v][1], attrs[v][2], attrs[v][3])
               for j, v in enumerate(st)] for st in strips]
        groups = geo_encode.pack(sa)
        nv = sum(len(g) for g in groups)
        mats.append(mat)
        pts.append(np.array([a[0] for a in attrs], float) * scale)
        out_s.append(dict(material=len(mats) - 1, component=geo_encode.COMPONENT,
                          uv_size=F32(geo_encode.uv_size(P, UV, [tuple(t) for t in keep])), vertex_count=nv,
                          strip_count=len(strips), tail_pad=None,
                          groups=[dict(wide=False, xyzw=[v[0] for v in g], normal=[v[1] for v in g], uv=[v[2] for v in g],
                                       rgba=[v[3] for v in g]) for g in groups]))
        stats['triangles'] += len(keep); stats['packed_vertices'] += nv; stats['strips'] += len(strips)
        stats['unique_vertices'] += len(attrs)
    allq = np.vstack(pts).astype(np.float32).astype(float) if pts else np.zeros((1, 3))
    lo, hi = allq.min(0), allq.max(0); c, h = (lo + hi) / 2, (hi - lo) / 2
    model = dict(nodes=[dict(name=name, name_raw=None, matrix=[1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1., 0, 0, 0, 0, 1.],
                             centre=[F32(x) for x in c], half=[F32(x) for x in h], radius=F32(float(np.linalg.norm(h))),
                             parent=-1)],
                 materials=mats, materials_raw=[None] * len(mats),
                 quant=[scale, geo_encode.NORMAL_SCALE, geo_encode.UV_SCALE, geo_encode.UV_SCALE, 0.0, 0.0, 0.0], world=0,
                 lods=[dict(cutoff=geo_encode.FLT_MAX, surfaces=out_s)], palette=None)
    if nodes is not None:
        model['nodes'] = [dict(name=label, name_raw=None, matrix=matrix.flatten().tolist(), parent=parent,
                               centre=[0., 0., 0.], half=[0., 0., 0.], radius=0.) for label,parent,matrix in nodes]
        merged = {}
        for surface in out_s:
            mat = mats[surface['material']]
            if mat in merged:
                target = merged[mat]
                target['vertex_count'] += surface['vertex_count']
                target['strip_count'] += surface['strip_count']
                target['groups'].extend(surface['groups'])
            else:
                surface['material'] = len(merged)
                merged[mat] = surface
        mats, out_s = list(merged), list(merged.values())
        model.update(materials=mats, materials_raw=[None]*len(mats))
        model['lods'][0]['surfaces'] = out_s
        points_by_bone = {}
        for surface,indices in zip(surfaces,bones):
            points=surface[1]
            lanes=np.broadcast_to(indices,(len(points),))
            for bone in np.unique(lanes):
                points_by_bone.setdefault(int(bone),[]).append(points[lanes==bone])
        for bone,parts in points_by_bone.items():
            points=np.vstack(parts)
            lo,hi = points.min(axis=0),points.max(axis=0)
            model['nodes'][bone].update(centre=((lo+hi)/2).tolist(),half=((hi-lo)/2).tolist(),
                                        radius=float(np.linalg.norm((hi-lo)/2)))
    blob = geo_psg.write(model)
    assert geo_psg.write(geo_psg.parse(blob)) == blob
    stats.update(bytes=len(blob), materials=mats, surfaces=len(out_s), quantum_ft=float(scale))
    return blob, stats


def mcf_text(stem, duration, looping):
    frames = max(1, int(math.ceil(duration - 1e-6)))
    return ('[General info]\r\nNumberOfMotions=1\r\nContentDirectory=anim\\\r\nSLTFile=%s.slt\r\nVUTfilename=%s.vut\r\n\r\n'
            '[MOTION 1]\r\nNAME=%s\r\nVUE=%s.vue\r\nFPS=1\r\nFrameCount=%d\r\nLooping=%s\r\n' % (
                stem, stem, stem, stem, frames, 'yes' if looping else 'no')).encode('ascii')


def motion(frames, duration, X, pivot_n64, linear=False):
    """frames [(t, A 4x4 N64 row-vector rest->posed world)] -> MOT bytes (one row: the single node, whose rest origin is the
    pivot): rotation = the posed 3x3 in Revenge axes, translation = how far the pivot itself moves."""
    import pod_formats
    Pm = perm_matrix(X)
    rot, pos = [], []
    for t, A in frames:
        rot.append((t, Pm.T @ A[:3, :3] @ Pm))
        o = np.asarray(pivot_n64, float)
        pos.append((t, ((o @ A[:3, :3] + A[3, :3] - o) @ Pm * X['scale']).tolist()))
    data = bytearray(pod_formats.write_mot(len(frames), float(duration), [(0, pos)], [(0, rot)]))
    if linear:
        # MOT ShortAxis uses the original 4096-entry half-angle table. ByteAxis
        # quantization visibly displaces contact points on large rotating meshes.
        struct.pack_into('<I', data, 12, 1)
        del data[36 + 40 * len(pos) + 8:]
        for time, matrix in rot:
            q = pod_formats.quat_from_rows(matrix)
            if q[0] < 0:
                q = -q
            half_angle = math.atan2(np.linalg.norm(q[1:]), q[0])
            angle = round(half_angle * 4096 / math.pi)
            axis = q[1:] / np.linalg.norm(q[1:]) if angle else np.array([1., 0., 0.])
            data.extend(struct.pack('<fH3h', time, angle, *np.rint(axis * 32767).astype(int)))
        assert pod_formats.read_mot(data)['rtype'] == 1
        # The N64 translation interpolator is linear. MOT stores Hermite tangents in segment units.
        for i in range(1, len(pos)):
            delta = np.subtract(pos[i][1], pos[i - 1][1])
            struct.pack_into('<3f', data, 36 + 40 * (i - 1) + 28, *delta)
            struct.pack_into('<3f', data, 36 + 40 * i + 16, *delta)
    return bytes(data)


def particle_hierarchy(model, marker, X, visible_seconds=0.):
    """Collapse identity nodes and merge each puff's translation into its spinning child.
    The eight scaled instances stay as parents, so rotation under nonuniform scale is preserved."""
    import n2r_anim
    P = perm_matrix(X)
    def axes(A):
        B = np.eye(4)
        B[:3, :3] = P.T @ A[:3, :3] @ P
        B[3, :3] = A[3, :3] @ P * X['scale']
        return B
    nodes = [('fx_root', -1, np.eye(4))]
    instances, leaves, position, rotation = {}, {}, [], []
    tracks = {a['target']: a for a in model.anims if a['type'] in (n2r_anim.ANIM_TRANSLATION, n2r_anim.ANIM_AXIS_ANGLE)}
    duration = max(a['times'][-1] for a in tracks.values() if not a['flags'] & 0x10)
    duration = max(duration,visible_seconds)
    for node, world, path in model.walk(model.header_nodes[0], 'all'):
        if not node['meshes']:
            continue
        animated = [k for k in path if k in tracks]
        translate, spin = animated
        tr, rot = tracks[translate], tracks[spin]
        assert tr['type'] == n2r_anim.ANIM_TRANSLATION and rot['type'] == n2r_anim.ANIM_AXIS_ANGLE
        assert path[-3:-1] == (translate, spin) and rot['flags'] & 0x10
        parent_path = path[:path.index(translate)]
        spin_rest = model.local_matrix(model.nodes[spin])
        rise_rest = model.local_matrix(model.nodes[translate])
        assert np.allclose(model.local_matrix(node), np.eye(4))
        basis = rise_rest[:3,:3]
        assert np.allclose(basis @ basis.T, np.eye(3), atol=1e-5)
        rest = spin_rest @ rise_rest
        if parent_path not in instances:
            parent_world = np.linalg.solve(rest, world @ marker)
            parent_world[3,:3] -= marker[3,:3]
            instances[parent_path] = len(nodes)
            nodes.append(('fx_inst%d' % len(instances), 0, axes(parent_world)))
        # Bake the fixed parent rotation into bind vertices. Conjugating the
        # animated rotation preserves its original angle in the ShortAxis keys.
        rest[:3,:3] = basis.T @ rest[:3,:3]
        parent = instances[parent_path]
        bone = len(nodes)
        nodes.append(('fx_puff%d' % bone, parent, axes(rest)))
        leaves[path] = (bone, axes(rest) @ nodes[parent][2])
        pos = [(t, (spin_rest[3,:3] @ basis + np.asarray(v[:3])) @ P * X['scale']) for t,v in zip(tr['times'],tr['values'])]
        if pos[-1][0]<duration:
            pos.append((duration,pos[-1][1]))
        period = rot['times'][-1]
        times = {0.0, duration}
        for repeat in range(int(math.ceil(duration / period))):
            times.update(t + repeat*period for t in rot['times'][:-1] if t + repeat*period < duration)
        row = len(position)
        position.append((row, pos))
        rotation.append((row, [(t, P.T @ basis.T @ n2r_anim._pose_at(model, model.nodes[spin], [rot], t)[:3,:3] @ basis @ P) for t in sorted(times)]))
    return nodes, leaves, position, rotation, duration


def skinned_mesh(model, mesh, path, groups):
    """Decode G_VTX's leading vertices using the mesh's referenced-node transform."""
    slots = {}
    vertices = {}
    pos, st, rgba, paths, tris = [], [], [], [], []
    for o in range(mesh['dl_ptr'],len(model.d)-7,8):
        cmd = model.d[o]
        if cmd==0xdf:
            break
        if cmd==1:
            word = model.u32(o)
            count = (word>>12)&255
            base = ((word>>1)&127)-count
            addr = model.u32(o+4)
            if base!=mesh['vtx_base'] or base<0 or base+count>64:
                raise ValueError('invalid skinned G_VTX')
            for i in range(base+count):
                slots[i] = (addr+16*(i-base),groups[mesh['group_ptr']] if i<base else path)
        elif cmd in (5,6):
            for offset in ([1,5] if cmd==6 else [1]):
                tri=[]
                for slot in model.d[o+offset:o+offset+3]:
                    address, owner = slots[slot//2]
                    key = (address, owner)
                    if key not in vertices:
                        vertices[key]=len(pos)
                        x,y,z,_,u,v=struct.unpack_from('>3hH2h',model.d,address)
                        pos.append((x,y,z));st.append((u,v));rgba.append(tuple(model.d[address+12:address+16]));paths.append(owner)
                    tri.append(vertices[key])
                tris.append(tri)
        elif cmd not in (0,3):
            raise ValueError('unsupported skinned display-list opcode %02x' % cmd)
    return dict(pos=np.array(pos), st=np.array(st), rgba=np.array(rgba), paths=paths, tris=np.array(tris))


def fish_hierarchy(track, off, model, marker, X):
    """Aquilaris 8007D80C attaches the creature to the track's animated marker."""
    import n2r_anim
    P = perm_matrix(X)
    scale = X['scale']
    def axes(matrix):
        local = np.eye(4)
        local[:3,:3] = P.T @ matrix[:3,:3] @ P
        local[3,:3] = matrix[3,:3] @ P * scale
        return local

    # Keep the marker's ancestors in rest-world axes, relative to the TOB pivot.
    parent = np.linalg.inv(track.local_matrix(track.nodes[off])) @ marker
    parent[3,:3] -= marker[3,:3]
    nodes = [('fish_world', -1, axes(parent))]
    worlds = [nodes[0][2]]
    leaves, bindings, position, rotation = {}, [], [], []
    def add(parent, matrix):
        i = len(nodes)
        local = axes(matrix)
        nodes.append(('fish_%02d' % i, parent, local))
        worlds.append(local @ worlds[parent])
        return i

    def append(source, node, parent):
        local = source.local_matrix(node)
        tracks = [a for a in source.anims if a['target']==node['off'] and a['type'] in (8,9)]
        pivot = np.asarray(node.get('pivot',[0.,0.,0.])) if node['flags3'] & 0x10 else np.zeros(3)
        # The original pivot descent is T(-p) R T(p+translation) in row-vector axes.
        if tracks and np.any(pivot):
            local[3,:3] = np.asarray(node['matrix'][9:12]) + pivot
        bone = add(parent, local)
        if tracks:
            row = len(bindings)
            bindings.append(bone)
            for anim in tracks:
                keys = []
                for time,value in zip(anim['times'],anim['values']):
                    if anim['type']==8:
                        axis=np.asarray(value[:3])
                        R=n2r_anim.rodrigues(axis,value[3]).T if np.linalg.norm(axis)>1e-8 else np.eye(3)
                        keys.append((time,P.T @ R @ P))
                    else:
                        keys.append((time,(np.asarray(value[:3])+pivot) @ P * scale))
                (rotation if anim['type']==8 else position).append((row,keys))
        if tracks and np.any(pivot):
            unpivot = np.eye(4)
            unpivot[3,:3] = -pivot
            bone = add(bone, unpivot)
        return bone

    marker_bone = append(track,track.nodes[off],0)
    hierarchy = {}
    for node,_,path in model.walk(model.header_nodes[0],'all'):
        bone = append(model,node,hierarchy.get(path[:-1],marker_bone))
        hierarchy[path] = bone
        if node['meshes']:
            leaves[path] = (bone, worlds[bone])
    # A constant change of joint axes preserves slerp and the skinned world pose.
    # Choose axes that represent the source angles closely on the MOT's 4096-step grid.
    # This matters on this creature's long tail, where angular errors accumulate.
    rotations = {bindings[row]: keys for row,keys in rotation}
    rng=np.random.default_rng(308)
    quats=rng.normal(size=(2048,4))
    quats/=np.linalg.norm(quats,axis=1)[:,None]
    candidates=np.array([np.eye(3)]+[n2r_anim.rodrigues(q[:3],2*math.degrees(math.acos(q[3]))) for q in quats])
    bases=[]
    for i,(name,parent,local) in enumerate(nodes):
        parent_axes=bases[parent] if parent>=0 else np.eye(3)
        axes=np.eye(3)
        if i in rotations:
            keys=rotations[i]
            values=np.array([m @ parent_axes.T for _,m in keys])
            traces=np.einsum('nij,kji->nk',candidates,values)
            angles=np.arccos(np.clip((traces-1)*.5,-1.,1.))*(4096/(2*math.pi))
            error=np.abs(angles-angles.round())
            axes=candidates[np.argmin(error.max(axis=1)+.25*error.mean(axis=1))]
            keys[:]=[(t,axes @ m @ parent_axes.T) for t,m in keys]
        bases.append(axes)
        rebased=local.copy()
        rebased[:3,:3]=axes @ local[:3,:3] @ parent_axes.T
        rebased[3,:3]=local[3,:3] @ parent_axes.T
        nodes[i]=(name,parent,rebased)
    for row,keys in position:
        parent=nodes[bindings[row]][1]
        keys[:]=[(t,v @ bases[parent].T) for t,v in keys]
    for path,(bone,rest) in leaves.items():
        rebased=rest.copy()
        rebased[:3,:3]=bases[bone] @ rest[:3,:3]
        leaves[path]=(bone,rebased)
    duration = max(keys[-1][0] for _,keys in position+rotation)
    return nodes,leaves,position,rotation,duration,bindings


def particle_motion(position, rotation, duration):
    """MOT linear Hermite translation and ShortAxis spherical rotation."""
    import pod_formats
    out = bytearray(struct.pack('<IIfIIII', 0xfffffffe, int(math.ceil(duration)), duration, 1, 1, len(position), len(rotation)))
    for row, keys in position:
        out += struct.pack('<II', len(keys), row)
        for i,(t,v) in enumerate(keys):
            incoming = v-keys[max(0,i-1)][1]
            outgoing = keys[min(len(keys)-1,i+1)][1]-v
            out += struct.pack('<10f', t, *v, *incoming, *outgoing)
    for row, keys in rotation:
        out += struct.pack('<II', len(keys), row)
        for t, matrix in keys:
            q = pod_formats.quat_from_rows(matrix)
            if q[0]<0:
                q=-q
            norm=np.linalg.norm(q[1:])
            angle=round(math.atan2(norm,q[0])*4096/math.pi)
            axis=q[1:]/norm if angle else np.array([1.,0.,0.])
            out += struct.pack('<fH3h', t, angle, *np.rint(axis*32767).astype(int))
    return bytes(out)


def col_local(T, pivot):
    """written collision faces (Revenge space, winding already Revenge) -> COL bytes relative to the pivot."""
    import trk_col
    V = (np.asarray(T, float) - pivot).reshape(-1, 3).astype(np.float32).astype(np.float64)
    uniq, inv = np.unique(V, axis=0, return_inverse=True)
    faces = inv.reshape(-1, 3)
    keep = (faces[:, 0] != faces[:, 1]) & (faces[:, 1] != faces[:, 2]) & (faces[:, 0] != faces[:, 2])
    # The character placement is scene index 0. The one PSG node animated by
    # the VUT is its child at index 1 (Revenge compound bind 0x126450).
    col = trk_col.make_col(uniq, faces[keep])
    col.type = 1
    col.members = [(1, col.body)]
    return trk_col.write_col(col), int(keep.sum())


def b(v):
    return 'T' if v else 'F'


def vec3(v):
    return ','.join('%f' % float(x) for x in v)


# ---------------------------------------------------------------- converter side (called by n2r_convert.Conv)
class Plan:
    """What the switch writes for one track, decided once from the N64 collision model (hazards mode) and triggers."""

    def __init__(self, conv, C):
        import n2r_anim
        self.conv, self.C, self.M = conv, C, C.M
        code = conv.natural_code
        self.triggers = [dict(t, flags=int(t.get('flags') or 0) | SETUP_FLAGS.get(t['type'], 0))
                         for t in conv.D['triggers']['triggers'] if t['type'] in ROW_TYPES]
        self.rows = []                                    # animation rows: dict(stem, node, kind, ...)
        names = {g['name'] for g in conv.D['geometry']}
        self.collision_only = []
        animated_nodes = dict(C.anim_nodes)
        # Bumpy's Breakers has three visual-only corvettes. Each node owns
        # seven meshes and two enabled, looping transform channels at load.
        if conv.model == 135:
            for off in (0x898, 0x2210, 0x39b8):
                animated_nodes[off] = 'flyby'
        for off, kind in sorted(animated_nodes.items(), key=lambda kv: (str(kv[1]), kv[0])):
            ms = set(C.subtree_meshes(off))
            if kind == 'loop' and not any('mesh_%06x' % m in names for m in ms):
                self.collision_only.append('0x%x' % off)
            self.rows.append(dict(stem=stem_of(code, off, kind), node=off, kind=kind, meshes=ms))
        # Independent loop clocks may be nested in the source hierarchy. Only the
        # innermost row owns a mesh; its ancestor still contributes its transform.
        by_node = {r['node']: r for r in self.rows}
        for n, _, path in self.M.walk(self.M.header_nodes[0], 'all'):
            if n['off'] not in by_node:
                continue
            ancestors = [by_node[o] for o in path[:-1] if o in by_node]
            if ancestors:
                child = by_node[n['off']]
                child['parent_stem'] = ancestors[-1]['stem']
                for ancestor in ancestors:
                    ancestor['meshes'] -= child['meshes']
                    ancestor['has_animated_child'] = True
        # One original subtree trigger drives every animated panel with the same timer.
        expanded = []
        for t in self.triggers:
            if t['type'] == 307:
                parent = self.M.nodes[int(t['affected_node'], 16)]
                panels = [node['off'] for node, _, _ in self.M.walk(parent, 'all') if C.anim_nodes.get(node['off']) == 307]
                if not panels:
                    raise ValueError('307 trigger has no animated panels')
                expanded.extend(dict(t, affected_node=hex(panel)) for panel in panels)
            else:
                expanded.append(t)
        self.triggers = expanded
        planet = int(conv.D['track']['planet_index'])
        self.fx_rows = []                                 # 314: the planet's FX model at each marker node, hidden until fired
        for t in self.triggers:
            if t['type'] == 314 and t.get('affected_node') and FX_MODEL.get(planet) is not None:
                off = int(t['affected_node'], 16)
                self.fx_rows.append(dict(stem=stem_of(code, off, 314), node=off, kind=314, model=FX_MODEL[planet], meshes=set()))
            if t['type'] == 306:
                off = int(t['affected_node'], 16)
                # 80083D80 returns the first mesh's transformed bounding endpoints.
                node, world, _ = next((n,w,p) for n,w,p in self.M.walk(self.M.nodes[off], 'all') if n['meshes'])
                bounds = np.asarray(node['meshes'][0]['aabb']).reshape(2,3)
                center = bounds.mean(axis=0) @ world[:3,:3] + world[3,:3]
                marker = np.eye(4)
                marker[3,:3] = center
                self.fx_rows.append(dict(stem='%s_splash_%05x' % (code,off), node=off, kind='splash',
                                         model=290, marker=marker, meshes=set()))
        self.triggers = [t for t in self.triggers if t['type'] != 314 or any(r['node'] == int(t['affected_node'], 16) for r in self.fx_rows)]
        # 308's crossing handler has no effect. Both creatures are set up at track load.
        for t in conv.D['triggers']['triggers']:
            if t['type']==308 and planet==2:
                off=int(t['affected_node'],16)
                self.fx_rows.append(dict(stem='%s_fish_%05x' % (code,off), node=off, kind=308,
                                         model=151, marker=n2r_anim.node_world(self.M,off), meshes=set()))
        self.debris = None
        if any(t['type'] == 102 for t in self.triggers):
            pm = DEBRIS_MODEL.get(int(conv.D['track']['planet_index']))
            if pm is not None:
                self.debris = dict(stem='%s_debris' % code, model=pm, kind='debris', node=None, meshes=set())
        self.carriers = set()
        for t in self.triggers:
            own = set()
            if t.get('affected_node'):
                own = set(C.subtree_meshes(int(t['affected_node'], 16)))
            for m in t.get('meshes') or []:
                mo = int(m, 16)
                if not any(mo in r['meshes'] for r in self.rows):
                    self.carriers.add(mo)
        self.carrier_cols = {}                            # mesh offset -> [static COL instance names]

    def mesh_stem(self):
        out = {}
        for r in self.rows:
            for m in r['meshes']:
                out['mesh_%06x' % m] = r['stem']
        return out

    def pivot_n64(self, r):
        """Transformed node: its world origin (the rotation centre); Basic node: the centre of its own geometry box."""
        import n2r_anim
        if 'marker' in r:
            return r['marker'][3,:3].copy()
        n = self.M.nodes[r['node']]
        if 'matrix' in n:
            return n2r_anim.node_world(self.M, r['node'])[3, :3].copy()
        C = self.C
        ids = [i for i, ml in enumerate(C.mesh_list) if ml['mesh']['off'] in r['meshes']]
        P = C.F[np.isin(C.mesh_of, ids)].reshape(-1, 3)
        return (P.min(0) + P.max(0)) / 2

    def debris_groups(self, out_dir):
        """the break FX model (N64 FX slot 0) around its own origin, and each 314 FX row's model placed at its marker node
        (world, rest pose): n2r_io groups, materials, {group name: row stem}"""
        import n2r_anim
        groups, mats, owner = [], {}, {}
        jobs = ([(self.debris, np.eye(4))] if self.debris else []) + [(r, r['marker'] if 'marker' in r else n2r_anim.node_world(self.M, r['node'])) for r in self.fx_rows]
        for r, W0 in jobs:
            g, m = self._model_groups(r['model'], out_dir, W0, r['stem'])
            groups += g
            mats.update(m)
            owner.update({x['name']: (r['stem'], x['model_path']) if r['kind'] in (314, 308, 'splash', 'debris') else r['stem'] for x in g})
        return groups, mats, owner

    def _model_groups(self, model, out_dir, W0, tag):
        import n2r_props
        import model as Mo
        import textures as TX
        from PIL import Image
        groups, mats, tex = [], {}, None
        import n2r_geo
        M = Mo.Model(model)
        scrolls = n2r_geo.model_uv_scrolls(M)
        graph = list(M.walk(M.header_nodes[0], 'all'))
        paths = {n['off']:path for n,_,path in graph}
        worlds = {path:world @ W0 for _,world,path in graph}
        for n, W, path in graph:
            if n['type'] != Mo.NODE_MESHGROUP:
                continue
            for m in n['meshes']:
                if not m['dl_ptr']:
                    continue
                v = skinned_mesh(M,m,path,paths) if model==151 else M.visual_tris(m)
                if v is None or not len(v['tris']):
                    continue
                mat = m['material']; t = mat['texture']; png = None
                if t and t['texture_index'] is not None:
                    tex = tex or TX.Textures()
                    img, note = tex.get(t['texture_index'], t['format'], t['width'], t['height'], swizzled=bool(t['has_mip_chain']))
                    img = TX.mirror_bake(img, t['mirror_s'], t['mirror_t'])
                    os.makedirs(out_dir, exist_ok=True)
                    png = os.path.join(out_dir, 'tex%04d_fx%d.png' % (t['texture_index'], model))
                    Image.fromarray(img).save(png)
                    su = 2.0 if t['mirror_s'] else 1.0; sv = 2.0 if t['mirror_t'] else 1.0
                    uv = np.stack([v['st'][:, 0] / (4096.0 * su), v['st'][:, 1] / (4096.0 * sv)], 1)
                    if t['res'][0]:
                        uv[:, 0] -= mat['tex_offset'][0] / float(t['res'][0]) / su
                    if t['res'][1]:
                        uv[:, 1] -= mat['tex_offset'][1] / float(t['res'][1]) / sv
                else:
                    uv = np.zeros((len(v['pos']), 2))
                key = 'fx%d_m%06x' % (model, mat['off'])
                mats[key] = n2r_props._material_entry(mat, png)
                scroll = scrolls.get('m%06x' % mat['off'])
                if scroll:
                    mats[key]['model_uv_scroll'] = scroll
                T = v['tris']
                if model==151:
                    points=np.array([p @ worlds[owner][:3,:3]+worlds[owner][3,:3] for p,owner in zip(v['pos'],v['paths'])])
                    # Send the binding of each triangle corner through the material/UV conversion.
                    for tri in T:
                        owners=tuple(v['paths'][tri[j]] for j in n2r_space.winding())
                        groups.append(dict(name='%s_%06x_%d' % (tag,m['off'],len(groups)),mtl=key,model_path=owners,
                                           P=points[tri][None],C=(v['rgba'][tri,:3]/255.)[None],UV=uv[tri][None]))
                else:
                    groups.append(dict(name='%s_%06x_%d' % (tag, m['off'], len(groups)), mtl=key, model_path=path,
                                       P=Mo.apply(W @ W0, v['pos'])[T], C=(v['rgba'][:, :3] / 255.0)[T], UV=uv[T]))
        return groups, mats

    def anim_files(self, buckets):
        """PSG / VUT / MCF / MOT / COL per row. buckets: n2r_geo anim_buckets {stem: {material: [(P, N, UV, C)]}}"""
        import n2r_anim
        conv, C, X = self.conv, self.C, self.conv.X
        order = n2r_space.winding()
        files, report = {}, []
        for r in self.rows + ([self.debris] if self.debris else []) + self.fx_rows:
            stem = r['stem']
            if r['kind'] == 'debris':
                pn = np.zeros(3)
            else:
                pn = self.pivot_n64(r)
            pivot = n2r_space.points(pn, X)
            surf = []
            for mname, lst in sorted(buckets.get(stem, {}).items()):
                P = np.vstack([x[0] for x in lst]) - pivot; N = np.vstack([x[1] for x in lst])
                UV = np.vstack([x[2] for x in lst]); Cc = np.vstack([x[3] for x in lst])
                surf.append((mname, P, N, UV, Cc, np.arange(len(P)).reshape(-1, 3)))
            if r['kind'] in (314, 308, 'splash', 'debris'):
                import model as Mo
                marker = np.eye(4) if r['kind']=='debris' else (r['marker'] if 'marker' in r else n2r_anim.node_world(self.M, r['node']))
                if r['kind'] == 308:
                    nodes, leaves, pos, rot, dur, binding = fish_hierarchy(self.M,r['node'],Mo.Model(r['model']),marker,X)
                else:
                    nodes, leaves, pos, rot, dur = particle_hierarchy(Mo.Model(r['model']),marker,X,2.2 if r['kind']=='splash' else 0.)
                    binding = [bone for bone,_ in leaves.values()]
                bones = []
                if r['kind']==308:
                    per_material={}
                    for key,materials in buckets.items():
                        if not isinstance(key,tuple) or key[0]!=stem:
                            continue
                        owners=key[1]
                        for material,triangles in materials.items():
                            output=per_material.setdefault(material,[])
                            for points,normals,uv,colors in triangles:
                                local=[]; normal=[]; indices=[]
                                for point,direction,path in zip(points,normals,owners):
                                    bone,world=leaves[path]
                                    inv=np.linalg.inv(world)
                                    local.append((point-pivot) @ inv[:3,:3]+inv[3,:3])
                                    n=direction @ world[:3,:3].T
                                    normal.append(n/np.linalg.norm(n))
                                    indices.append(bone)
                                output.append((local,normal,uv,colors,indices))
                    for material,triangles in per_material.items():
                        points,normals,uv,colors,indices=[np.vstack(x) for x in zip(*triangles)]
                        surf.append((material,points,normals,uv,colors,np.arange(len(points)).reshape(-1,3)))
                        bones.append(indices.ravel())
                else:
                    for path,(bone,world) in leaves.items():
                        inv = np.linalg.inv(world)
                        for material, triangles in buckets[(stem,path)].items():
                            P = (np.vstack([t[0] for t in triangles]) - pivot) @ inv[:3,:3] + inv[3,:3]
                            N = np.vstack([t[1] for t in triangles]) @ world[:3,:3].T
                            N /= np.linalg.norm(N,axis=1)[:,None]
                            surf.append((material,P,N,np.vstack([t[2] for t in triangles]),np.vstack([t[3] for t in triangles]),
                                         np.arange(len(P)).reshape(-1,3)))
                            bones.append(bone)
                psg, st = build_psg_multi(stem, surf, nodes, bones)
                vut = ''.join('%s=%s\r\n' % (nodes[bone][0],nodes[bone][0]) for bone in binding).encode('ascii')
            else:
                psg, st = build_psg_multi(stem, surf)
                vut = ('%s=%s\r\n' % (stem,stem)).encode('ascii')
            files['anim' + BS + stem + '.psg'] = psg
            files['anim' + BS + stem + '.vut'] = vut
            if r['kind'] in (101, 307, 'loop', 'flyby'):
                fr, dur = n2r_anim.motion_frames(self.M, r['node'], MOTION_STEP_S, source_keys=not surf or r['kind'] in (307, 'flyby'))
                looping = r['kind'] != 307  # 101 and fan carry the N64 LOOP flag (0x11000030); the door's toggle clears it at run time (port)
            elif r['kind'] == 312:
                # the pillar's own fall (set-up 0x8007D944 clears ENABLED | LOOP; the handler plays it once at 0.6 = Rate)
                fr, dur = n2r_anim.motion_frames(self.M, r['node'], 0.1)
                looping = False
            elif r['kind'] in (314, 308, 'splash', 'debris'):
                looping = r['kind']==308
            else:
                fr, dur, looping = [(0.0, np.eye(4)), (1.0, np.eye(4))], 1.0, False
            files['anim' + BS + stem + '.mot'] = (particle_motion(pos,rot,dur) if r['kind'] in (314, 308, 'splash', 'debris') else
                motion(fr, dur, X, pn, linear=not surf or r['kind'] in (307, 'flyby')))
            files['anim' + BS + stem + '.mcf'] = mcf_text(stem, dur, looping)
            ncol = 0
            if r['kind'] not in ('debris', 314, 308, 'splash'):
                ids = [i for i, ml in enumerate(C.mesh_list) if ml['mesh']['off'] in r['meshes'] and C.mesh_track_ok(i, include_anim=True)]
                T = []
                for fi in np.flatnonzero(np.isin(C.mesh_of, ids) & ~C.degenerate):
                    tri = n2r_space.points(C.F[fi], X)[order]
                    T.append(tri)
                    if not C.one_sided[fi]:
                        T.append(tri[[0, 2, 1]])
                if T:
                    files['anim' + BS + stem + '.col'], ncol = col_local(np.array(T), pivot)
            r.update(pivot=pivot, pivot_n64=pn, duration=dur, looping=looping, collision_faces=ncol, psg=st,
                     zone=int(conv.zone_of_points(np.array([pivot]))[0]))
            report.append(dict(stem=stem, kind=str(r['kind']), node=('0x%x' % r['node']) if r['node'] and r['kind'] not in (314, 308, 'splash') else 'model %d' % r['model'],
                               packed_vertices=st['packed_vertices'], unique_vertices=st['unique_vertices'], triangles=st['triangles'],
                               strips=st['strips'], surfaces=st['surfaces'], transform_only=bool(r.get('has_animated_child') and not ncol and not st['triangles']), collision_faces=ncol, motion_s=round(dur, 2),
                               motion_keys=sum(len(keys) for _,keys in pos+rot) if r['kind'] in (314, 308, 'splash', 'debris') else len(fr)))
        if self.debris and self.debris['model'] == 291:
            files['sound' + BS + 'n64_39.wav'] = asteroid_impact_wave()
        return files, report

    def sections(self):
        """TOB sections: Triggers / TriggerN / TriggerNAction1 and Animations / AnimationN (NOTES.md 7)."""
        conv, X = self.conv, self.conv.X
        rows = self.rows + ([self.debris] if self.debris else []) + self.fx_rows
        by_node = {r['node']: r for r in self.rows}
        fx_by_node = {r['node']: r for r in self.fx_rows}
        S = X['scale']
        secs = [('Triggers', [('NumberOfTriggers', str(len(self.triggers)))])]
        for i, t in enumerate(self.triggers, 1):
            c = n2r_space.points(np.array(t['center'], float), X)
            d = n2r_space.dirs(np.array(t['direction'], float), X); d[1] = 0.0; d = d / max(np.linalg.norm(d), 1e-9)
            surf = []
            for m in t.get('meshes') or []:
                mo = int(m, 16)
                if mo in self.carrier_cols:
                    surf += self.carrier_cols[mo]
                else:
                    owner = [r['stem'] for r in self.rows if mo in r['meshes']]
                    surf += owner          # carried only by an animation row's own mesh: never the static floor (N64: never fires in a race)
            kv = [('Name', 'n64_%d_%s' % (t['type'], t['off'][2:])), ('Zone', str(int(conv.zone_of_points(np.array([c]))[0]))),
                  ('Position', vec3(c)), ('StartEnabled', 'T'), ('OneTimeOnly', b(t['type'] in (102, 306, 312, 314))), ('NumberOfActions', '1'),
                  ('N64Volume', 'Plane' if t['type'] in PLANE_TYPES else 'Capsule'), ('N64Direction', vec3(d)),
                  ('N64HalfWidth', '%f' % (t['size_xy'] / 2.0 * X['scale'])), ('N64HalfHeight', '%f' % (t['size_z'] / 2.0 * X['scale'])),
                  ('SurfaceObject', ','.join(surf) if surf else 'none')]
            fl = int(t.get('flags') or 0)
            if fl & 0x2:
                kv.append(('MinSpeed', '150'))
            if fl & 0x20:
                kv.append(('LocalOnly', 'T'))
            if fl & 0x1C:
                kv.append(('Laps', ','.join(str(k + 1) for k, bit in enumerate((0x4, 0x8, 0x10)) if not fl & bit)))
            secs.append(('Trigger%d' % i, kv))
            if t['type'] == 104:
                # Racer init clears flags1 (80072B3C). The last-lap setter of
                # FORCE_GROUND at 80054384 requires LOCAL. Camera LOD does not grant it.
                act = [('ActionType', 'ExplodePod'), ('Side', 'Random'), ('Gate', 'Local'),
                       ('AfterDestroy', 'Respawn'), ('RespawnDelay', '%g' % RESPAWN_DELAY_S)]
            elif t['type'] == 102:
                act = [('ActionType', 'BreakObject'), ('AnimationNameRef', by_node[int(t['affected_node'], 16)]['stem']),
                       ('ThrustDeceleration', '2.5'), ('DamageTotal', '0.15'), ('DamageBoostScale', '4'), ('DamageHits', '3')]
                if self.debris:
                    act += [('EffectNameRef', self.debris['stem']), ('EffectAhead', '%g' % (12 * S)), ('EffectAiScale', '8'), ('EffectFade', '2')]
                if self.debris and self.debris['model'] == 291:
                    act += [('EffectLifetime', '1'), ('Filename', 'sound' + BS + 'n64_39.wav')]
                act += [('Pitch', '0.25'), ('PitchRandom', '0.15'), ('MinDistance', '%g' % (10 * S)), ('MaxDistance', '%g' % (500 * S))]
            elif t['type'] == 306:
                off = int(t['affected_node'], 16)
                act = [('ActionType', 'Earthquake'), ('AnimationNameRef', by_node[off]['stem']),
                       ('EffectNameRef', fx_by_node[off]['stem']), ('FeetPerUnit', '%g' % S)]
            elif t['type'] == 312:
                act = [('ActionType', 'AnimationPlay'), ('AnimationNameRef', by_node[int(t['affected_node'], 16)]['stem']),
                       ('Rate', '%g' % PILLAR_RATE), ('Show', 'F'), ('N64PillarShakeScale', '%g' % S)]
            elif t['type'] == 314:
                act = [('ActionType', 'AnimationPlay'), ('AnimationNameRef', fx_by_node[int(t['affected_node'], 16)]['stem']),
                       ('Rate', '1'), ('Show', 'T')]
            elif t['type'] == 307:
                act = [('ActionType', 'AnimationToggle'), ('AnimationNameRef', by_node[int(t['affected_node'], 16)]['stem']),
                       ('FirstTrigger', 'Arm'), ('ToggleGuard', '3'), ('ToggleRate', '1')]
            else:
                act = [('ActionType', 'AnimationToggle'), ('AnimationNameRef', by_node[int(t['affected_node'], 16)]['stem']),
                       ('FirstTrigger', 'Freeze'), ('ToggleGuard', '3.0'), ('ToggleRate', '20')]
            secs.append(('Trigger%dAction1' % i, act))
        secs.append(('Animations', [('NumberOfAnimations', str(len(rows)))]))
        for i, r in enumerate(rows, 1):
            active = r['kind'] in (101, 307, 308, 'loop', 'flyby')
            kv = [('AnimationType', 'KeyFramed'), ('ReferenceName', r['stem']), ('MCF', r['stem'] + '.mcf'),
                  ('StartingZone', str(r['zone'])), ('Offset', vec3(r['pivot'])), ('UseLighting', 'F'),
                  ('NumberOfMotions', '1'), ('MotionToPlay1', r['stem']), ('NumberOfSequences', '1'), ('NumberInSequence1', '1'),
                  ('MotionSequence1', '1'), ('SequenceActive1', b(active)), ('LoopSequence1', b(active and r['kind'] != 307)),
                  ('UseCollision', b(r['kind'] not in ('debris', 314, 308, 'splash') and r.get('collision_faces', 0) > 0))]
            if r.get('parent_stem'):
                parent = next(j for j, row in enumerate(self.rows, 1) if row['stem'] == r['parent_stem'])
                if parent >= i:
                    raise ValueError('animated parent must precede child')
                kv.append(('N64ParentAnimation', str(parent)))
            if r.get('has_animated_child') and not r['psg']['triangles'] and not r['collision_faces']:
                kv.append(('N64TransformOnly', 'T'))
            if r['kind'] in ('loop', 'flyby', 308):
                kv.append(('N64LoadLoop','T'))
            if r['kind'] == 308:
                kv.append(('N64IndependentChannels', 'T'))
            if r['kind'] == 307:
                kv.append(('HoldLastKey', 'T'))
            if r['kind'] in ('debris', 314, 'splash'):
                kv.append(('StartHidden', 'T'))
            secs.append(('Animation%d' % i, kv))
        return secs