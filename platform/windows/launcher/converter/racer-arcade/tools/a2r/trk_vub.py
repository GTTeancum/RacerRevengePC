r"""Cut-camera key files tracks\xxNN.vub (one per [CSCameraN] of the .tob).

Layout (measured on all 57 originals, round trip in trk_rt_misc.py):
  u32 1, u32 frame count N, u32 1
  N records of 116 bytes:
    +0  u32 frame index (0..N-1)      +4 u32 1      +8 u32 0
    +12 char[8]  'camera'             +20 40 bytes the exporter's runtime words (pointers;
                                           copied unchanged from a template record)
    +60 char[16] 'tracks\xxNN.vue'    +76 f32 look xyz   +88 f32 up xyz
    +100 f32 position xyz             +112 f32 field of view (degrees; 78 in ta01)
"""
import struct
import numpy as np

REC = 116


def read_vub(d):
    one, n, one2 = struct.unpack_from('<3I', d, 0)
    assert one == 1 and one2 == 1 and len(d) == 12 + REC * n, 'VUB layout'
    frames = []
    for i in range(n):
        r = d[12 + REC * i:12 + REC * (i + 1)]
        idx, a, b = struct.unpack_from('<3I', r, 0)
        name = r[60:76].split(b'\0', 1)[0].decode('ascii')
        look = struct.unpack_from('<3f', r, 76); up = struct.unpack_from('<3f', r, 88)
        pos = struct.unpack_from('<3f', r, 100); fov, = struct.unpack_from('<f', r, 112)
        frames.append(dict(index=idx, raw=r, name=name, look=look, up=up, pos=pos, fov=fov))
    return frames


def write_vub_raw(frames):
    return struct.pack('<3I', 1, len(frames), 1) + b''.join(f['raw'] for f in frames)


def make_vub(template_record, name, keys):
    """keys: list of (look, up, pos, fov). template_record: one 116-byte original record
    (its runtime words are kept as they are, see module doc)."""
    nb = name.encode('ascii')
    assert len(nb) < 16
    out = bytearray(struct.pack('<3I', 1, len(keys), 1))
    for i, (look, up, pos, fov) in enumerate(keys):
        r = bytearray(template_record)
        struct.pack_into('<3I', r, 0, i, 1, 0)
        r[12:20] = b'camera\0\0'
        r[60:76] = nb + b'\0' * (16 - len(nb))
        struct.pack_into('<3f', r, 76, *look)
        struct.pack_into('<3f', r, 88, *up)
        struct.pack_into('<3f', r, 100, *pos)
        struct.pack_into('<f', r, 112, fov)
        out += r
    return bytes(out)


def look_up(eye, target):
    f = np.asarray(target, float) - np.asarray(eye, float)
    f /= np.linalg.norm(f)
    wup = np.array([0.0, 1.0, 0.0])
    r = np.cross(wup, f); r /= np.linalg.norm(r)
    u = np.cross(f, r)
    return tuple(f), tuple(u)
