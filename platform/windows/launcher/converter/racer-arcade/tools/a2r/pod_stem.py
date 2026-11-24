r"""A converted pod archive (built for stock slot NN) -> the same pod as an ADD-ON pod under its own stem.
One rule for every pod (arcade a2r pods, Demolition Pugwis, any later source); nothing keyed to a pod.

    python -I pod_stem.py SRC_RES NN STEM OUT_DIR [--cam SRC_CAM]
        writes OUT_DIR\PODS\<STEM>\<STEM>.RES (+ <STEM>.CAM) and OUT_DIR\<stem>.pod_stem.json (report)

NOT ORIGINAL BEHAVIOUR: add-on pods are a PC enhancement (work\scratch\dlc\DESIGN.md 2.3 'pod', 4.3). The
original formats every pod-specific name from the pod number (Pod%02d for the folder, PHY and AI file;
'%02d_' for its textures, materials and motions; flame%02dl/r and concring%02d for its flame and ring
materials; audio\speech\pod%02d\ for its speech tables). The stem rule replaces that number, and only that:

  member path   'pod'+NN (any case)            -> STEM             Pods\Pod04\Pod04.phy -> Pods\a2r_ark\a2r_ark.phy
                basename NN_...                -> STEM_...         textures\04_1.psm   -> textures\a2r_ark_1.psm
                basename flameNN / concringNN  -> flameSTEM / concringSTEM
  PSG           material names by the same basename rule (04_1 -> a2r_ark_1, flame04l -> flamea2r_arkl); node
                names untouched (role names such as pilot23 are not pod numbers)
  .mcf          ContentDirectory, motion NAME and VUE by the same rules
  .psm          TextureMap_k by the same rule
  .phy          [Pod Setup] Prefix = "NN" -> "STEM" (every other key byte for byte, incl. ChariotRigidMount /
                RigidBankFactor where the converter wrote them)
  speech .ini   moved to audio\speech\STEM\; their FileName lines are NOT rewritten: they name the stock
                speech clips of slot NN's pilot in the player's install (borrowed voice, reported as a stand-in)
Shared member names (cable, binder, Flame, concring, specmap*, particle\*.emt, audio\p_*.ini, pod_wndw ...)
keep their names: the original ships them byte-identical in every pod archive under fixed names.

Checks before anything is written: every new name unique (case-insensitive), no pod number left in any member
name or PSG material name, every PSG re-parses and re-writes identically after renaming, every psm TextureMap
and every PSG material resolves inside the archive or to a shared name, every MCF motion has its .mot member.
"""
import argparse
import hashlib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import geo_psg  # noqa: E402
import trk_res3  # noqa: E402

BS = chr(92)
STEM_RE = re.compile(r'^[a-z0-9_]{3,16}$')
POD_NN_RE = re.compile(r'^pod\d\d$')
STOCK_PODS = (__import__('os').environ['RR_CONVERT_STOCK']+'/PODS')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def check_stem(stem):
    if not STEM_RE.match(stem) or POD_NN_RE.match(stem):
        raise SystemExit('stem %r: must match [a-z0-9_]{3,16} and must not be pod<NN> (DESIGN.md 2.3)' % stem)


def base_rename(base, nn, stem):
    """basename (no folder) by the stem rule; returns the new basename."""
    b = re.sub(r'(?i)pod' + nn, stem, base)
    b = re.sub(r'^' + nn + r'(?=_)', stem, b)
    b = re.sub(r'(?i)^(flame|concring)' + nn, lambda m: m.group(1) + stem, b)
    return b


def path_rename(path, nn, stem):
    parts = path.split(BS)
    parts = [re.sub(r'(?i)pod' + nn, stem, p) for p in parts[:-1]] + [base_rename(parts[-1], nn, stem)]
    return BS.join(parts)


def name_rename(name, nn, stem):
    """material / texture name (no extension) by the basename rule."""
    return base_rename(name, nn, stem)


def has_token(name, nn):
    low = name.lower()
    return bool(re.search(r'pod' + nn, low) or re.match(nn + r'_', low) or re.match(r'(flame|concring)' + nn, low))


def rewrite_text(member, data, nn, stem, log):
    low = member.lower()
    t = data.decode('latin-1')
    out = t
    if low.endswith('.mcf'):
        out = re.sub(r'(?im)^(ContentDirectory\s*=\s*)(.*)$', lambda m: m.group(1) + re.sub(r'(?i)pod' + nn, stem, m.group(2)), out)
        out = re.sub(r'(?im)^(NAME\s*=\s*)(\S+)', lambda m: m.group(1) + name_rename(m.group(2), nn, stem), out)
        out = re.sub(r'(?im)^(VUE\s*=\s*)(\S+)', lambda m: m.group(1) + name_rename(m.group(2), nn, stem), out)
    elif low.endswith('.psm'):
        out = re.sub(r'(?im)^(\s*TextureMap_\d+\s*=\s*)(\S+)', lambda m: m.group(1) + name_rename(m.group(2), nn, stem), out)
    elif low.endswith('.phy'):
        out, k = re.subn(r'(?im)^(\s*Prefix\s*=\s*")' + nn + r'(")', lambda m: m.group(1) + stem + m.group(2), out)
        if k != 1:
            raise ValueError('%s: [Pod Setup] Prefix = "%s" not found exactly once' % (member, nn))
    if out != t:
        log.append(member)
    return out.encode('latin-1')


def texture_maps(psm):
    return [m.group(1) for m in re.finditer(r'(?im)^\s*TextureMap_\d+\s*=\s*(\S+)', psm.decode('latin-1'))]


def shared_names():
    """material/texture names present in stock pod archives under the same name in several pods (shared)."""
    seen = {}
    for d in sorted(os.listdir(STOCK_PODS)):
        p = os.path.join(STOCK_PODS, d, d + '.RES')
        if not os.path.exists(p):
            continue
        r = trk_res3.read_res3(open(p, 'rb').read())
        for n in r.names():
            b = n.split(BS)[-1].lower()
            seen.setdefault(b.rsplit('.', 1)[0], set()).add(d)
    return {k for k, v in seen.items() if len(v) >= 2}


def convert(src_res, nn, stem, out_dir, cam=None):
    check_stem(stem)
    if not re.match(r'^\d\d$', nn):
        raise SystemExit('slot must be two digits')
    raw = open(src_res, 'rb').read()
    r = trk_res3.read_res3(raw)
    shared = shared_names()
    rows, new_members, texlog, psglog = [], [], [], []
    seen = set()
    for n in r.names():
        d = r.get(n)
        nn_new = path_rename(n, nn, stem)
        low = n.lower()
        if low.endswith(('.mcf', '.psm', '.phy')):
            d2 = rewrite_text(n, d, nn, stem, texlog)
        elif low.endswith('.psg'):
            m = geo_psg.parse(d)
            if geo_psg.write(m) != d:
                raise ValueError('%s: PSG does not re-write identically before renaming' % n)
            mats = [name_rename(x, nn, stem) for x in m['materials']]
            if mats != m['materials']:
                m['materials_raw'] = [None if a != b else raw_ for a, b, raw_ in zip(mats, m['materials'], m['materials_raw'])]
                m['materials'] = mats
                psglog.append(n)
            d2 = geo_psg.write(m)
            m2 = geo_psg.parse(d2)
            if m2['materials'] != mats or geo_psg.write(m2) != d2 or len(d2) != len(d):
                raise ValueError('%s: renamed PSG does not round-trip' % n)
        else:
            d2 = d
        if nn_new.lower() in seen:
            raise ValueError('duplicate member after renaming: %s' % nn_new)
        seen.add(nn_new.lower())
        new_members.append((nn_new, d2))
        rows.append(dict(old=n, new=nn_new, bytes=len(d2), sha256=sha(d2), renamed=nn_new != n, content_changed=d2 != d,
                         speech_table=low.startswith('audio' + BS + 'speech' + BS)))
    # checks
    names = {x.lower(): x for x, _ in new_members}
    left = [x for x, _ in new_members if has_token(x.split(BS)[-1], nn) or re.search(r'(?i)pod' + nn, x)]
    if left:
        raise ValueError('pod number left in member names: %s' % left)
    base_names = {x.split(BS)[-1].lower().rsplit('.', 1)[0] for x in names}
    problems = []
    for x, d in new_members:
        lx = x.lower()
        if lx.endswith('.psm'):
            for t in texture_maps(d):
                if t.lower() not in base_names and t.lower() not in shared:
                    problems.append('%s: TextureMap %s resolves nowhere' % (x, t))
                if has_token(t, nn):
                    problems.append('%s: TextureMap %s keeps the pod number' % (x, t))
        if lx.endswith('.psg'):
            for mt in geo_psg.parse(d)['materials']:
                if has_token(mt, nn):
                    problems.append('%s: material %s keeps the pod number' % (x, mt))
                if (mt.lower() not in base_names) and (mt.lower() not in shared):
                    problems.append('%s: material %s has no psm in the archive and is not shared' % (x, mt))
        if lx.endswith('.mcf'):
            t = d.decode('latin-1')
            cdir = re.search(r'(?im)^ContentDirectory\s*=\s*(.*)$', t).group(1).strip()
            nmot = int(re.search(r'(?im)^NumberOfMotions\s*=\s*(\d+)', t).group(1))
            # only MOTION 1..NumberOfMotions are read (stock POD24 engrt1.mcf carries a dead [MOTION 1] with 0 motions)
            vues = []
            for k in range(1, nmot + 1):
                sec = re.search(r'(?ims)^\[MOTION ' + str(k) + r'\](.*?)(?=^\[|\Z)', t)
                vues += re.findall(r'(?im)^VUE\s*=\s*(\S+)', sec.group(1)) if sec else ['<missing MOTION %d>' % k]
            for v in vues:
                want = (cdir + v.rsplit('.', 1)[0] + '.mot').lower()
                if want not in names:
                    problems.append('%s: motion %s has no member %s' % (x, v, want))
    if problems:
        raise ValueError('stem checks failed:\n  ' + '\n  '.join(problems))
    speech_refs = []
    for x, d in new_members:
        if x.lower().startswith('audio' + BS + 'speech' + BS) and x.lower().endswith('.ini'):
            speech_refs += re.findall(r'(?im)^\s*FileName\s*=\s*(\S+)', d.decode('latin-1'))
    out_res = trk_res3.write_res3(new_members)
    back = trk_res3.read_res3(out_res)
    if [n for n in back.names()] != [x for x, _ in new_members] or any(back.get(x) != d for x, d in new_members):
        raise ValueError('written archive does not read back')
    up = stem.upper()
    pdir = os.path.join(out_dir, 'PODS', up)
    os.makedirs(pdir, exist_ok=True)
    open(os.path.join(pdir, up + '.RES'), 'wb').write(out_res)
    cam_row = None
    if cam:
        c = open(cam, 'rb').read()
        open(os.path.join(pdir, up + '.CAM'), 'wb').write(c)
        stock_cam = os.path.join(STOCK_PODS, 'POD' + nn, 'POD%s.CAM' % nn)
        cam_row = dict(src=cam, bytes=len(c), sha256=sha(c),
                       stock_slot_cam=bool(os.path.exists(stock_cam) and open(stock_cam, 'rb').read() == c))
    rep = dict(source=src_res, source_bytes=len(raw), source_sha256=sha(raw), slot=nn, stem=stem,
               archive='PODS/%s/%s.RES' % (up, up), bytes=len(out_res), sha256=sha(out_res), members=len(new_members),
               renamed=sum(1 for x in rows if x['renamed']), text_rewritten=texlog, psg_materials_renamed=psglog,
               speech_clip_refs=sorted(set(speech_refs)), cam=cam_row, rows=rows)
    json.dump(rep, open(os.path.join(out_dir, stem + '.pod_stem.json'), 'w'), indent=1)
    return rep


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('src_res')
    ap.add_argument('nn')
    ap.add_argument('stem')
    ap.add_argument('out_dir')
    ap.add_argument('--cam', default=None)
    a = ap.parse_args()
    rep = convert(a.src_res, a.nn, a.stem, a.out_dir, a.cam)
    print('%s: %d members (%d renamed), %d bytes, sha %s; text %d, psg %d' % (
        rep['archive'], rep['members'], rep['renamed'], rep['bytes'], rep['sha256'][:16],
        len(rep['text_rewritten']), len(rep['psg_materials_renamed'])))


if __name__ == '__main__':
    main()
