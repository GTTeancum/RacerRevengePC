r"""Install converted pods into a silo asset copy and list them in its converted-content.json.

    python -I pod_silo.py install 04 15 23 [--assets assets] [--src export\a2r\pods]
    python -I pod_silo.py testpack 04 [--assets assets-podtest]     (admission test pod, see below)
    python -I pod_silo.py restore 04 [--assets assets-podtest]      (put the original back, drop the row)

install: for each PODnn, reads <src>\PODnn\install\PODS\PODnn\PODnn.RES (and PODnn.CAM when the
converter ships one) plus <src>\PODnn\converted-content.fragment.json, then
1. checks the archive decodes, holds the members the port looks up by name (PHY with Prefix "nn",
   the four speech INIs) and stays inside the port's storage caps (listed in CAPS with file:line);
2. copies each original being replaced into silo\originals\ (same relative path; never
   overwritten once saved; the silo copy must still equal the installed original in C:\Games);
3. writes the new files into silo\<assets>\ and MERGES one row per file into
   silo\<assets>\converted-content.json (rows for other paths, e.g. the track, are kept);
4. re-reads everything written and checks it against its row.
Same guarantees as trk_silo.py: one lock (silo\.install.lock) for every installer, C:\Games is only read.

Row content (schema a2r-converted-content/1): path, bytes, sha256, replaces{bytes,sha256}, members[]
(name, offset, bytes, sha256), facts{} computed here from the file the way the port reads it:
speech_<key> = the nine [Indices] counts of audio\speech\podnn\sounds.ini (the port's speech owners
cross-check them against the same INI), plus provenance counts (members, wreck parts/pieces).

testpack: the admission test before converted pods exist. Re-encodes an ORIGINAL pod archive with a
different deflate (python zlib level 9): same members, same member bytes, different archive bytes and
SHA. Installed the same way as a converted pod.
"""
import hashlib
import json
import os
import shutil
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import trk_res3  # noqa: E402

GAME = r'C:/Games/Star Wars Racer Revenge'
SILO = os.path.join(SCR, 'silo')
SRC = os.path.join(SCR, 'export', 'a2r', 'pods')
BS = chr(92)
SPEECH_KEYS = ['NumberOfLowDamageSounds', 'NumberOfWallHitSounds', 'NumberOfPassedSounds',
               'NumberOfAttackedSounds', 'NumberOfPassingSounds', 'NumberOfAttackingSounds',
               'NumberOfDeathSounds', 'NumberOfWinSounds', 'NumberOfLoseSounds']
# port storage caps a converted pod must fit (file:line in the silo tree)
CAPS = [
    ('archive bytes <= 64 MiB', 'archive_bytes', 64 * 1024 * 1024, 'src\\assets\\converted_content50cv.cpp admittedFile'),
    ('members <= 512', 'members', 512, 'tests\\native\\selected_player_audio50ij_fixture.h:49 (resource_entries 512)'),
    ('decoded bytes <= 16 MiB', 'decoded_bytes', 16 * 1024 * 1024, 'selected_player_audio50ij_fixture.h:49'),
    ('PHY bytes <= 8192', 'phy_bytes', 8192, 'tests\\native\\opponent_engine_audio50hx_fixture.h:107'),
    ('speech INI bytes <= 8192', 'speech_ini_max_bytes', 8192, 'selected_player_audio50ij_fixture.h:54'),
    ('curses events <= 24', 'curses_events', 24, 'tests\\native\\gameplay_audio_provider50he_fixture.h:16-30'),
    ('taunts events <= 12', 'taunts_events', 12, 'gameplay_audio_provider50he_fixture.h:16-30'),
    ('special events <= 11', 'special_events', 11, 'gameplay_audio_provider50he_fixture.h:16-30'),
    ('CAM bytes < 8192', 'cam_bytes', 8191, 'src\\game\\original_follow_cam.cpp (text buffer 8192)'),
]


def sha(b):
    return hashlib.sha256(b).hexdigest()


def ini(text):
    out, sec = {}, None
    for line in text.replace('\r', '').split('\n'):
        line = line.strip()
        if not line or line.startswith(';'):
            continue
        if line.startswith('[') and line.endswith(']'):
            sec = line[1:-1].strip().lower(); out.setdefault(sec, {}); continue
        if '=' in line and sec is not None:
            k, v = line.split('=', 1); out[sec][k.strip().lower()] = v.strip()
    return out


def pod_facts(pod, data, cam=None):
    """Facts read from the archive the way the port reads them. Raises on anything the port refuses."""
    r = trk_res3.read_res3(data)
    names = {}
    for n, lo, sz in r.members:
        k = n.lower()
        if k in names:
            raise SystemExit('POD%s: ambiguous member %s' % (pod, n))
        names[k] = (n, lo, sz)
    def get(name):
        k = name.lower()
        if k not in names:
            raise SystemExit('POD%s: member missing %s' % (pod, name))
        n, lo, sz = names[k]
        return r.payload[lo:lo + sz]
    phy = get('pods' + BS + 'pod%s' % pod + BS + 'pod%s.phy' % pod)
    setup = ini(phy.decode('latin-1'))
    if setup.get('pod setup', {}).get('prefix', '').strip('"') != pod:
        raise SystemExit('POD%s: PHY [Pod Setup] Prefix is not "%s" (native_racer_rig50hw_cold.cpp:57)' % (pod, pod))
    f = dict(archive_bytes=len(data), members=len(r.members), decoded_bytes=len(r.payload), phy_bytes=len(phy))
    wreck = setup.get('wreckage', {})
    try:  # provenance only (the port's wreck reader counts these itself)
        f['wreck_parts'] = int(wreck['numberofparts'])
        f['wreck_pieces'] = sum(int(setup.get(wreck.get('part%d' % i, '').strip('"').lower(), {}).get('numberofparts', 0))
                                for i in range(1, f['wreck_parts'] + 1))
    except (KeyError, ValueError):
        f.pop('wreck_parts', None)
    pre = 'audio' + BS + 'speech' + BS + 'pod%s' % pod + BS
    sounds = ini(get(pre + 'sounds.ini').decode('latin-1')).get('indices', {})
    for k in SPEECH_KEYS:
        if k.lower() not in sounds:
            raise SystemExit('POD%s: sounds.ini lacks %s' % (pod, k))
        f['speech_' + k.lower()] = int(sounds[k.lower()])
    sums = dict(curses=SPEECH_KEYS[0:4], taunts=SPEECH_KEYS[4:6], special=SPEECH_KEYS[6:9])
    mx = 0
    for cat, keys in sums.items():
        b = get(pre + cat + '.ini'); mx = max(mx, len(b))
        declared = int(ini(b.decode('latin-1')).get('responseemitter', {}).get('numevents', '-1'))
        f[cat + '_events'] = declared
        want = sum(f['speech_' + k.lower()] for k in keys)
        if declared != want and not (pod == '26' and cat == 'curses'):
            raise SystemExit('POD%s: %s.ini NumEvents %d != sounds.ini sum %d' % (pod, cat, declared, want))
    f['speech_ini_max_bytes'] = max(mx, len(get(pre + 'sounds.ini')))
    if cam is not None:
        f['cam_bytes'] = len(cam)
    bad = [(label, f[key], cap, where) for label, key, cap, where in CAPS if key in f and f[key] > cap]
    for label, v, cap, where in bad:
        print('  CAP FAIL %-28s %d > %d  %s' % (label, v, cap, where))
    if bad:
        raise SystemExit('POD%s: storage caps failed, nothing installed' % pod)
    members = [dict(name=n, offset=lo, bytes=sz, sha256=sha(r.payload[lo:lo + sz])) for n, lo, sz in r.members]
    return f, members


def lock():
    p = os.path.join(SILO, '.install.lock')
    try:
        fd = os.open(p, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        raise SystemExit('another silo install is running (%s)' % p)
    os.write(fd, str(os.getpid()).encode()); os.close(fd)
    return p


def backup(rel, assets):
    """Save the original of rel into silo\\originals once; the silo copy must still be the installed original."""
    game = open(os.path.join(GAME, *rel.split('/')), 'rb').read()
    orig = os.path.join(SILO, 'originals', *rel.split('/'))
    cur_path = os.path.join(SILO, assets, *rel.split('/'))
    if not os.path.exists(orig):
        cur = open(cur_path, 'rb').read()
        if cur != game:
            raise SystemExit('%s in %s is not the installed original and no backup exists: refusing' % (rel, assets))
        os.makedirs(os.path.dirname(orig), exist_ok=True)
        shutil.copyfile(cur_path, orig)
    if open(orig, 'rb').read() != game:
        raise SystemExit('backup %s differs from the installed original' % orig)
    return dict(bytes=len(game), sha256=sha(game))


def load_manifest(assets):
    cc = os.path.join(SILO, assets, 'converted-content.json')
    if os.path.exists(cc):
        doc = json.load(open(cc))
        if doc.get('schema') != 'a2r-converted-content/1' or not isinstance(doc.get('files'), list):
            raise SystemExit('%s: unexpected schema' % cc)
    else:
        doc = dict(schema='a2r-converted-content/1',
                   about='Asset-root admission list for a silo build of the port. Presence of this file is the switch.',
                   files=[])
    return cc, doc


def merge(doc, rows):
    keep = [f for f in doc['files'] if f['path'].lower() not in {r['path'].lower() for r in rows}]
    doc['files'] = keep + rows


def write_and_check(cc, doc, assets):
    tmp = cc + '.tmp'
    open(tmp, 'w').write(json.dumps(doc, indent=1))
    os.replace(tmp, cc)
    back = json.load(open(cc))
    paths = [f['path'].lower() for f in back['files']]
    assert len(paths) == len(set(paths)), 'duplicate rows'
    for f in back['files']:
        p = os.path.join(SILO, assets, *f['path'].split('/'))
        b = open(p, 'rb').read()
        assert len(b) == f['bytes'] and sha(b) == f['sha256'], 'row does not match file: ' + f['path']


def install_one(pod, data, cam, assets, source, fragment):
    rel = 'PODS/POD%s/POD%s.RES' % (pod, pod)
    f, members = pod_facts(pod, data, cam)
    if fragment:
        rows = fragment.get('files', [fragment]) if isinstance(fragment, dict) else fragment
        for fr in rows:
            if fr.get('path', '').lower() == rel.lower():
                if 'bytes' in fr and fr['bytes'] != len(data) or 'sha256' in fr and fr['sha256'].lower() != sha(data):
                    raise SystemExit('POD%s: fragment row does not describe the shipped RES' % pod)
                for k, v in (fr.get('facts') or {}).items():
                    if k in f and isinstance(v, int) and v != f[k]:
                        raise SystemExit('POD%s: fragment fact %s=%s differs from the file (%s)' % (pod, k, v, f[k]))
                    f.setdefault(k, v)
    rows = []
    rep = backup(rel, assets)
    out = os.path.join(SILO, assets, *rel.split('/'))
    open(out, 'wb').write(data)
    rows.append(dict(path=rel, bytes=len(data), sha256=sha(data), replaces=rep, pod='%s' % pod, source=source,
                     facts={k: v for k, v in f.items() if k != 'archive_bytes'}, members=members))
    if cam is not None:
        crel = 'PODS/POD%s/POD%s.CAM' % (pod, pod)
        crep = backup(crel, assets)
        open(os.path.join(SILO, assets, *crel.split('/')), 'wb').write(cam)
        rows.append(dict(path=crel, bytes=len(cam), sha256=sha(cam), replaces=crep, pod='%s' % pod, source=source))
    print('POD%s: %d bytes, %d members, speech %s' % (pod, len(data), len(members),
          ','.join(str(f['speech_' + k.lower()]) for k in SPEECH_KEYS)))
    return rows


def cmd_install(pods, assets, src):
    lk = lock()
    try:
        cc, doc = load_manifest(assets)
        rows = []
        for pod in pods:
            d = os.path.join(src, 'POD%s' % pod)
            res = os.path.join(d, 'install', 'PODS', 'POD%s' % pod, 'POD%s.RES' % pod)
            cam_p = os.path.join(d, 'install', 'PODS', 'POD%s' % pod, 'POD%s.CAM' % pod)
            frag_p = os.path.join(d, 'converted-content.fragment.json')
            if not os.path.exists(res):
                raise SystemExit('POD%s: %s missing' % (pod, res))
            frag = json.load(open(frag_p)) if os.path.exists(frag_p) else None
            cam = open(cam_p, 'rb').read() if os.path.exists(cam_p) else None
            if cam is not None and cam == open(os.path.join(GAME, 'PODS', 'POD%s' % pod, 'POD%s.CAM' % pod), 'rb').read():
                cam = None  # byte copy of the original: no row needed
            rows += install_one(pod, open(res, 'rb').read(), cam, assets, 'converted (%s)' % d, frag)
        merge(doc, rows)
        write_and_check(cc, doc, assets)
        print('converted-content.json: %d rows (%s)' % (len(doc['files']), ', '.join(f['path'] for f in doc['files'])))
    finally:
        os.remove(lk)


def cmd_testpack(pods, assets):
    lk = lock()
    try:
        cc, doc = load_manifest(assets)
        rows = []
        for pod in pods:
            orig = open(os.path.join(GAME, 'PODS', 'POD%s' % pod, 'POD%s.RES' % pod), 'rb').read()
            r = trk_res3.read_res3(orig)
            blocks = [zlib.compress(r.payload[i:i + trk_res3.BLOCK], 9) for i in range(0, len(r.payload), trk_res3.BLOCK)]
            table = struct.pack('<%dH' % (len(blocks) + 1), *[len(b) for b in blocks], 0)
            idx = bytearray()
            for name, lo, sz in r.members:
                nb = name.encode('ascii'); idx += struct.pack('<I', len(nb)) + nb + struct.pack('<2I', lo, sz)
            out = bytearray(struct.pack('<5I', 3, len(table) + 12, trk_res3.HDR_CODEC, 0, len(table))) + table
            out += struct.pack('<2I', len(r.members), len(idx)) + idx
            for b in blocks:
                out += b + b'\xff'
            new = bytes(out)
            rr = trk_res3.read_res3(new)
            assert rr.members == r.members and rr.payload == r.payload and new != orig
            rows += install_one(pod, new, None, assets, 'testpack: original POD%s re-encoded with zlib level 9' % pod, None)
        merge(doc, rows)
        write_and_check(cc, doc, assets)
        print('converted-content.json: %d rows' % len(doc['files']))
    finally:
        os.remove(lk)


def cmd_restore(pods, assets):
    lk = lock()
    try:
        cc, doc = load_manifest(assets)
        drop = set()
        for pod in pods:
            for ext in ('RES', 'CAM'):
                rel = 'PODS/POD%s/POD%s.%s' % (pod, pod, ext)
                if any(f['path'].lower() == rel.lower() for f in doc['files']):
                    orig = os.path.join(SILO, 'originals', *rel.split('/'))
                    shutil.copyfile(orig, os.path.join(SILO, assets, *rel.split('/')))
                    drop.add(rel.lower())
        doc['files'] = [f for f in doc['files'] if f['path'].lower() not in drop]
        if doc['files']:
            write_and_check(cc, doc, assets)
        elif os.path.exists(cc):
            os.remove(cc)
        print('restored', sorted(drop))
    finally:
        os.remove(lk)


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['install', 'testpack', 'restore'])
    ap.add_argument('pods', nargs='+')
    ap.add_argument('--assets', default='assets')
    ap.add_argument('--src', default=SRC)
    a = ap.parse_args()
    pods = ['%02d' % int(p) for p in a.pods]
    if os.path.normcase(os.path.abspath(os.path.join(SILO, a.assets))).startswith(os.path.normcase(os.path.abspath(GAME))):
        raise SystemExit('refusing to write under C:\\Games')
    {'install': lambda: cmd_install(pods, a.assets, a.src), 'testpack': lambda: cmd_testpack(pods, a.assets),
     'restore': lambda: cmd_restore(pods, a.assets)}[a.cmd]()
