r"""Install converted tracks packed under original course codes into a silo asset copy. One asset copy
can hold several converted tracks, each replacing a DIFFERENT stock course (one rule for every code).

    python -I trk_silo.py sc [--src ROOT] [--assets assets]          one track (after trk_convert.py N --code-as sc)
    python -I trk_silo.py multi --assets assets-multi ma=ROOT1 mb=ROOT2 gb=ROOT3 sc=ROOT4
        several tracks into one copy: every track is checked first; nothing is written unless all pass
    python -I trk_silo.py check CODE --src ROOT                       the checks only, nothing written

1. checks the packed archive against the port's current limits, which are the same for every course
   code (COMMON_LIMITS, each row with its port file:line), and reports the per-course port stand-ins
   that still refuse that code today (STANDINS: they go away with the common race load; reported, not
   blocking, because they are port stand-ins and not limits of the original);
2. copies each original file being replaced into silo\originals\ (same relative path; never
   overwritten once saved; the saved copy must equal the stock pin of that code);
3. writes the new archive into silo\<assets>\TRACKS\<CODE>.RES and its row into
   silo\<assets>\converted-content.json (rows for other files and other codes are kept);
4. re-reads everything that was written.
Never touches C:\Games (read only, for the identity check).

    python -I trk_silo.py CODE --src ROOT --dlc-out DIR [--dlc-id ID] [--dlc-disabled]
    python -I trk_silo.py multi --dlc-out DIR ma=ROOT1 mb=ROOT2 ...

With --dlc-out: the same checks, then one DLC replacement package per track, DIR\<package id>\
(TRACKS\<CODE>.RES + dlc.json, written by dlc_out.py), INSTEAD of installing; the silo is not touched.
"""
import hashlib
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SCR = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, HERE)
import trk_res3  # noqa: E402
import trk_facts  # noqa: E402
import trk_binparam  # noqa: E402
import trk_ini  # noqa: E402
import geo_pst  # noqa: E402

GAME = (__import__('os').environ['RR_CONVERT_STOCK']+'')
SILO = os.path.join(SCR, 'silo')
BS = chr(92)

# Stock archive pins of the 13 course codes (the port's own stock identities,
# tests\native\material_audio_provider50hi_fixture.h:62-87); the installed original must equal its pin.
ORIGINAL_PINS = {
    'ta': ('TRACKS/TA.RES', 3334562, 'b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8'),
    'gc': ('TRACKS/GC.RES', 4205067, '69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9'),
    'ga': ('TRACKS/GA.RES', 2834942, '063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9'),
    'ma': ('TRACKS/MA.RES', 3691592, '2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228'),
    'sa': ('TRACKS/SA.RES', 3786696, '34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a'),
    'ra': ('TRACKS/RA.RES', 4604561, '1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2'),
    'mb': ('TRACKS/MB.RES', 4421779, '6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca'),
    'sc': ('TRACKS/SC.RES', 2974353, '8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d'),
    'tc': ('TRACKS/TC.RES', 3607868, '7b5486f4029dabe137919127c879faef92b8bd2fb9df5c0d1a1d122e92731490'),
    'rc': ('TRACKS/RC.RES', 4655141, 'ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b'),
    'tb': ('TRACKS/TB.RES', 3749557, '1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b'),
    'gb': ('TRACKS/GB.RES', 4088586, 'e92af24305431dc5a96cebb5ebcf89159adb6d371045b2c5424dacd1f74b7d99'),
    'mc': ('TRACKS/MC.RES', 3822171, 'f8854752f639676257d44e33479a0c31c047a17e4cd592d02f483ad593179c81'),
}
MIB = 1024 * 1024

# The port's limits for a course archive, the same for every course code (read from the port code,
# main c7aa428 == silo tree 3400481 for every row; paths relative to the repo). Stock counts of a course
# (SC 984 entries, 252 COL, 4107 instances, ...) are receipts there (logged, not refused) and are not here.
COMMON_LIMITS = [
    ('archive bytes', 'archive_bytes', '<=', 64 * MIB, 'src\\assets\\import_decode48c.h:17 (DecodeLimits48C, course_archive_receipt50ci.h:25)'),
    ('decoded bytes', 'decoded_bytes', '<=', 128 * MIB, 'src\\assets\\import_decode48c.h:18'),
    ('archive entries', 'entries', '<=', 10000, 'src\\assets\\import_decode48c.h:19'),
    ('largest member bytes (extract / .psg)', 'max_member_bytes', '<=', 8 * MIB, 'src\\assets\\import_decode48c.h:21,65'),
    ('map nodes', 'map_nodes', '<=', 256, 'src\\assets\\track_start50ad.h:6'),
    ('AI nodes', 'ai_nodes', '<=', 256, 'src\\assets\\track_start50ad.h:6'),
    ('spline text bytes', 'spline_text_bytes', '<=', 65536, 'src\\assets\\track_start50ad.h:6 (.cpp:71)'),
    ('zones (sky-selection row store, binding cap)', 'zones', '<=', 28, 'tests\\native\\original_track_sky50kx_fixture.h:24,200'),
    ('portals', 'portals', '<=', 4096, 'src\\assets\\import_world48f.h:13'),
    ('placed collision objects', 'placed_col', '<=', 1021, 'tests\\native\\authored_runtime_ground50cp_fixture.h:38-44 (1024 SearchList slots - 3)'),
    ('instances', 'instances', '<=', 100000, 'src\\assets\\import_world48f.h:13 (.cpp:258-259)'),
    ('largest .scb/.tob/.aub bytes', 'max_param_bytes', '<=', 2 * MIB, 'src\\assets\\import_world48f.h:11-12'),
    ('every zone SearchType List', 'search_types', '==', ['List'], 'tests\\native\\track_runtime_storage50ig_fixture.h:1058-1062'),
    ('textures (materials)', 'images', '<=', 4096, 'src\\assets\\material_texture_assets50ea.h:10'),
    ('texture RGBA bytes', 'rgba_bytes', '<=', 256 * MIB, 'src\\assets\\material_texture_assets50ea.h:12'),
    ('texture source bytes', 'texture_source_bytes', '<=', 256 * MIB, 'src\\assets\\material_texture_assets50ea.h:13'),
    ('largest texture side px', 'max_texture_side', '<=', 1024, 'src\\assets\\material_texture_assets50ea.h (1024 px)'),
    ('cut cameras 1..7', 'cuts', 'in', (1, 7), 'tests\\native\\course_cut_identity50ci.h:75'),
    ('cut cameras cover the intro script', 'cuts_minus_script_camera', '>=', 0, 'audio\\scrptLns.ini CameraNumber (trk_cuts.py)'),
    ('intro events 1..5', 'intro_events', 'in', (1, 5), 'platform\\linux\\native\\native_race_intro_transport50la.inl:89'),
    ('intro events == the code\'s stock intro (offsets from the stock row)', 'intro_events_minus_stock', '==', 0,
     'src\\assets\\original_ta_intro_authoring_candidate.h:24,127-137 (FLAG stand-in)'),
    ('.aub rows readable (C/D 8,12,20 equal or 16,32 with 12; 1..4 materials)', 'aub_legacy_ok', '==', True,
     'src\\assets\\material_audio50hi.cpp:97-103'),
    ('.aub sound rows 8..44', 'aub_rows_total', 'in', (16, 44), 'tests\\native\\material_audio_provider50hi_fixture.h:221'),
    ('.aub default material 1', 'aub_default_material', '==', 1, 'tests\\native\\material_audio_provider50hi_fixture.h:222'),
]

# Per-course port stand-ins that refuse a converted archive at that code TODAY (silo-cv-22 and main
# c7aa428). Not limits of the original: the common race load (work\scratch\common-race\PLAN.md) retires
# them. Reported with every check; never blocking. (key, op, value) rows are evaluated on the facts,
# rows without a key always apply. Source: notes\multi-slot-plan.md section 3 and the port files named.
STANDINS = {
    'sc': [],
    'ma': [(None, None, None, 'ground archive SHA pin', 'tests\\native\\authored_runtime_ground50cp_fixture.h:76-78'),
           (None, None, None, '13 LostAI triggers, 145 static params, ATM, pinned sky row', 'tests\\native\\ma_*_fixture.h')],
    'ga': [(None, None, None, 'ground archive SHA pin', 'tests\\native\\authored_runtime_ground50cp_fixture.h:76-78')],
    'sa': [(None, None, None, 'ground archive SHA pin', 'tests\\native\\authored_runtime_ground50cp_fixture.h:76-78')],
    'gc': [(None, None, None, 'ground archive SHA pin; water session required', 'tests\\native\\native_gc_water_session50kg_fixture.h:42-81')],
    'ta': [(None, None, None, 'ground archive SHA pin; TA.RES audio pin used by every course', 'src\\assets\\audio_assets50hc.cpp:265-273')],
    'mb': [(None, None, None, 'archive identity', 'tests\\native\\terrain_interactive_world50du.cpp:728-737'),
           ('aub_is_stock', '==', True, '.aub size+SHA pinned (MB shape)', 'src\\assets\\material_audio50hi.cpp:93-97')],
    'tb': [(None, None, None, 'own cold source: exact 10 zones / 11 portals / 1112 instances', 'tests\\native\\tb_cold_sources50la.h:141'),
           ('placed_col', '==', 320, 'exact placed COL count', 'tests\\native\\tb_runtime_ground50la.h:93'),
           ('aub_is_stock', '==', True, '.aub size+SHA pinned (TB shape)', 'src\\assets\\material_audio50hi.cpp:87-89')],
    'tc': [(None, None, None, 'own cold source: archive identity not a receipt', 'tests\\native\\tc_cold_sources50la.h:129,134-135'),
           ('placed_col', '==', 69, 'exact placed COL count', 'tests\\native\\tc_runtime_ground50la.h:71'),
           ('aub_is_stock', '==', True, '.aub size+SHA pinned (TC shape)', 'src\\assets\\material_audio50hi.cpp:91-93')],
    'gb': [(None, None, None, 'own cold source: exact 13 zones / 17 portals / 975 instances', 'tests\\native\\gb_cold_sources50la.h:136-139'),
           ('placed_col', '==', 615, 'exact placed COL count', 'tests\\native\\gb_runtime_ground50la.h:93')],
    'rc': [(None, None, None, 'own cold source: stock SCB limits, exact 15/15/1277', 'tests\\native\\rc_cold_sources50la.h:161-164'),
           ('placed_col', '==', 112, 'exact placed COL count', 'tests\\native\\rc_runtime_ground50la.h:75')],
    'mc': [(None, None, None, 'own cold source: exact 28/28/458; water required', 'tests\\native\\mc_cold_sources50la.h:131-135'),
           ('placed_col', '==', 89, 'exact placed COL count', 'tests\\native\\mc_runtime_ground50la.h:93'),
           ('aub_is_stock', '==', True, '.aub size+SHA pinned (MC shape)', 'src\\assets\\material_audio50hi.cpp:79-82')],
    'ra': [(None, None, None, 'whole-archive size+SHA', 'tests\\native\\ra_cold_sources50la.h:166-170')],
}


def sha(b):
    return hashlib.sha256(b).hexdigest()


def _ok(v, op, ref):
    if op == '<=':
        return v is not None and v <= ref
    if op == '>=':
        return v is not None and v >= ref
    if op == '==':
        return v == ref
    if op == '>':
        return v is not None and v > ref
    if op == 'in':
        return v is not None and ref[0] <= v <= ref[1]
    raise ValueError(op)


def check(f, rows):
    out, ok = [], True
    for label, key, op, ref, where in rows:
        v = f.get(key)
        good = _ok(v, op, ref)
        ok &= good
        out.append(dict(check=label, value=v, op=op, limit=list(ref) if isinstance(ref, tuple) else ref, port=where, ok=bool(good)))
    return ok, out


def standins(code, f):
    """Port stand-ins that refuse this code today: [{refusal, port, applies}] (informational)."""
    out = []
    for key, op, ref, what, where in STANDINS.get(code, []):
        applies = True if key is None else not _ok(f.get(key), op, ref)
        if applies:
            out.append(dict(refusal=what, port=where))
    return out


def _packed(code, src_root):
    root = os.path.join(src_root or os.path.join(SCR, 'export', 'a2r', 'override'), code)
    man = json.load(open(os.path.join(root, 'manifest.json')))
    if man.get('code', code) != code:
        raise SystemExit('%s holds a track packed as %s, not %s' % (root, man.get('code'), code))
    # converters for other sources (racer-n64 n2r) name their own geometry manifest; arcade manifests do not
    geo_man = man.get('geometry_manifest') or os.path.join(SCR, 'export', 'a2r', 'course%d' % man['course'], 'manifest.json')
    packed = os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper())
    if os.path.getmtime(geo_man) > os.path.getmtime(packed):
        raise SystemExit('geometry output (%s) is newer than the packed %s: run %s first' % (
            geo_man, packed, man.get('converter_command') or 'trk_convert.py %d --code-as %s' % (man['course'], code)))
    return root, man


def _more_facts(f, r, code):
    """Counts the common limits need beyond trk_facts (all from the archive's own files)."""
    low = {n.lower(): n for n in r.names()}
    f['texture_source_bytes'] = sum(len(d) for n, d in r.items() if n.lower().startswith('textures' + BS) and n.lower().endswith('.pst'))
    f['max_member_bytes'] = max(sz for _, _, sz in r.members)
    f['max_param_bytes'] = max(len(r.get(low[n])) for n in low if n.endswith(('.scb', '.tob', '.aub')))
    f['spline_text_bytes'] = max(len(r.get(low['tracks' + BS + k + code + '.spl'])) for k in 'am')
    side = 0
    for n in low:
        if n.startswith('textures' + BS) and n.endswith('.pst'):
            for lv in geo_pst.parse(r.get(low[n]))['levels']:
                side = max(side, lv['w'], lv['h'])
    f['max_texture_side'] = side
    script = low.get('audio' + BS + 'scrptlns.ini')
    cams = set()
    if script:
        for sec, kv in trk_ini.parse(r.get(script)).sections():
            for k, v in kv:
                if k.lower() == 'cameranumber':
                    cams |= {int(x) for x in v.replace(' ', '').split(',') if x}
    f['script_cameras'] = sorted(cams)
    f['cuts_minus_script_camera'] = f['cuts'] - max(cams or [0])
    stock = trk_res3.read_res3(open(os.path.join(GAME, *ORIGINAL_PINS[code][0].split('/')), 'rb').read())
    sl = {n.lower(): n for n in stock.names()}
    si = sl.get('audio' + BS + 'introaud.ini')
    stock_events = int(trk_ini.parse(stock.get(si)).get('ResponseEmitter', 'NumEvents', 0)) if si else 0
    f['intro_events_minus_stock'] = f['intro_events'] - stock_events
    aub = r.get(low['tracks' + BS + code + '.aub'])
    s = dict(dict(trk_binparam.decode(aub))['sounds'])
    c_, d_, m_ = int(s['nummaterialcollisionsounds']), int(s['nummaterialdriveonsounds']), int(s['nummaterials'])
    import trk_aub
    f['aub_rows'] = [c_, d_, m_]
    f['aub_rows_total'] = c_ + d_
    f['aub_legacy_ok'] = trk_aub.legacy_ok(c_, d_, m_)
    f['aub_default_material'] = int(s.get('defaultmaterial', 0))
    f['aub_is_stock'] = aub == stock.get(sl['tracks' + BS + code + '.aub'])
    return f


def _checked(code, root, quiet=False):
    """The packed archive, its facts, its limit rows and today's port stand-ins; refuses when a common
    limit fails (nothing is written before this)."""
    if code not in ORIGINAL_PINS:
        raise SystemExit('%s is not a stock course code' % code)
    new = open(os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper()), 'rb').read()
    f = trk_facts.facts(new, code)
    r = trk_res3.read_res3(new)
    _more_facts(f, r, code)
    ok, limits = check(f, COMMON_LIMITS)
    today = standins(code, f)
    if not quiet:
        print('-- %s: %s' % (code.upper(), root))
        for row in limits:
            print('  %-58s %14s %2s %-14s %s' % (row['check'], row['value'], row['op'], row['limit'], 'ok' if row['ok'] else 'FAIL'))
        for s_ in today:
            print('  port stand-in refusing %s today (until the common race load): %s [%s]' % (code.upper(), s_['refusal'], s_['port']))
    if not ok:
        raise SystemExit('%s: limits failed: nothing installed' % code.upper())
    return new, f, r, limits, today


def _source(man, code):
    return (dict(arcade_course=man['course'], arcade_name=man['name'], natural_code=__import__('a2r_space').CODES[man['course']],
                 packed_as=code, converter='tools\\a2r\\trk_convert.py %d --code-as %s' % (man['course'], code),
                 transform=man['transform']) if man.get('source_kind') is None else
            dict(kind=man['source_kind'], model=man.get('model'), name=man['name'], natural_code=man.get('natural_code'),
                 packed_as=code, converter=man.get('converter_command'), transform=man['transform']))


def _stock_ok(code):
    rel, pbytes, psha = ORIGINAL_PINS[code]
    g = open(os.path.join(GAME, *rel.split('/')), 'rb').read()
    assert len(g) == pbytes and sha(g) == psha, 'installed original %s does not match its pin' % rel
    return rel, pbytes, psha


def _fact_row(f):
    return {k: v for k, v in f.items() if k not in ('archive_bytes', 'archive_sha256')}


def package(code, src_root, dlc_out_dir, pkg_id=None, enabled=True, checked=None):
    """--dlc-out: the checks of an install (packed archive not older than its geometry, limits, installed
    original == its pin), then the DLC replacement package for TRACKS/<CODE>.RES. Nothing is written into the silo."""
    import dlc_out
    root, man = _packed(code, src_root)
    new, f, r, limits, today = checked or _checked(code, root)
    rel, pbytes, psha = _stock_ok(code)
    borrowed = [n for n, lo, sz in r.members if man['files'].get(n, {}).get('provenance', '').startswith('BORROWED')]
    return dlc_out.track_package(dlc_out_dir, _source(man, code), code, new, dict(bytes=pbytes, sha256=psha),
                                 _fact_row(f), limits,
                                 dict(built_from=os.path.abspath(root), borrowed_members=borrowed,
                                      port_standins_today=today), enabled, pkg_id)


def main(code, src_root=None, assets='assets'):
    """The only way anything is installed into a silo asset copy (geometry and track helpers both use it).
    Refuses when the packed archive is older than the geometry/texture output it was built from
    (rerun trk_convert.py N --code-as CODE first), and holds a lock so two installs cannot interleave."""
    multi({code: src_root}, assets)


def multi(tracks, assets='assets', dlc_out_dir=None, dlc_enabled=True):
    """tracks = {code: converter --out root}. Every track is checked before anything is written; codes must differ."""
    if len(set(tracks)) != len(tracks):
        raise SystemExit('two tracks for one code')
    staged = []
    for code, src in tracks.items():
        root, man = _packed(code, src)
        staged.append((code, root, man, _checked(code, root)))
        _stock_ok(code)
    if dlc_out_dir:
        return [package(code, os.path.dirname(root), dlc_out_dir, None, dlc_enabled, chk)
                for code, root, man, chk in staged]
    lock = os.path.join(SILO, '.install.lock')
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        raise SystemExit('another silo install is running (%s)' % lock)
    os.write(fd, str(os.getpid()).encode()); os.close(fd)
    try:
        for code, root, man, chk in staged:
            _install(code, root, man, assets, chk)
    finally:
        os.remove(lock)


def _install(code, root, man, assets, checked):
    res_rel = 'TRACKS/%s.RES' % code.upper()
    new, f, r, limits, today = checked
    # originals: the silo copy must still be the installed original
    rel, pbytes, psha = _stock_ok(code)
    silo_file = os.path.join(SILO, assets, *rel.split('/'))
    orig_file = os.path.join(SILO, 'originals', *rel.split('/'))
    if not os.path.exists(orig_file):
        cur = open(silo_file, 'rb').read()
        assert sha(cur) == psha, 'silo copy is not the original and no backup exists: refusing'
        os.makedirs(os.path.dirname(orig_file), exist_ok=True)
        shutil.copyfile(silo_file, orig_file)
    assert sha(open(orig_file, 'rb').read()) == psha
    members = []
    for name, lo, sz in r.members:
        prov = man['files'].get(name, {}).get('provenance', '')
        members.append(dict(name=name, offset=lo, bytes=sz, sha256=sha(r.payload[lo:lo + sz]),
                            borrowed=prov.startswith('BORROWED')))
    ld_rel = 'UI/TEXTURES/LD_%s.PST' % code.upper()
    ld = open(os.path.join(SILO, assets, *ld_rel.split('/')), 'rb').read()
    assert ld == open(os.path.join(GAME, *ld_rel.split('/')), 'rb').read(), 'loading screen is not the original'
    cube = 'skies' + BS + '%s.cub' % code
    track = dict(
        source=_source(man, code),
        kept_original=[dict(path=ld_rel, bytes=len(ld), sha256=sha(ld),
                            reason='loading screen of the replaced course kept (the converter writes none under a stock code)')],
        borrowed_members=[m['name'] for m in members if m['borrowed']],
        sky=dict(present=cube in r.names(), member=cube, cube_file='%s.cub' % code,
                 bytes=len(r.get(cube)) if cube in r.names() else 0, sha256=sha(r.get(cube)) if cube in r.names() else None,
                 layout='original CUB: 1 palette + 1 cube, format 32, masks 00ff x4 / ffff / 0000, 48 tiles x 4 IPUM mips 16..128'),
        limits=limits, port_standins_today=today)
    row = dict(path=res_rel, bytes=len(new), sha256=sha(new), replaces=dict(bytes=pbytes, sha256=psha),
               course_code=code, facts=_fact_row(f), members=members, track=track)
    cc = os.path.join(SILO, assets, 'converted-content.json')
    old = json.load(open(cc)) if os.path.exists(cc) else {}
    files = [row] + [x for x in old.get('files', []) if x.get('path', '').lower() != res_rel.lower()]
    # rows written before the multi-track layout carry their track record at the top level: move it into the row
    for x in files:
        if x.get('course_code') and 'track' not in x and old.get('source', {}).get('packed_as') == x['course_code']:
            x['track'] = {k: old[k] for k in ('source', 'kept_original', 'borrowed_members', 'sky', 'limits') if k in old}
    doc = dict(
        schema='a2r-converted-content/1',
        about=('Asset-root admission list for a silo build of the port (notes\\silo-override.md 5; the port no longer reads '
               'it). "files" lists every replaced file: path relative to the asset root with "/" separators, size and '
               'SHA-256 of the new file, size and SHA-256 of the original it replaces, and the content counts ("facts") '
               'computed from the new file the way the port counts them (tools\\a2r\\trk_facts.py). A converted track row '
               '(course_code) carries its own "track" record: source, borrowed members, kept originals, sky, the common '
               'limits it passed and the port stand-ins that still refuse its code today. Several converted tracks may '
               'replace different stock courses in one asset copy.'),
        files=files)
    tracks = [x for x in files if x.get('course_code')]
    if len(tracks) == 1:   # one converted track: also at the top level, as single-track copies always had it
        doc.update({k: v for k, v in tracks[0]['track'].items() if k in ('source', 'kept_original', 'borrowed_members', 'sky', 'limits')})
    else:
        doc['tracks'] = [dict(code=x['course_code'], path=x['path'], name=x['track']['source'].get('name') or
                              x['track']['source'].get('arcade_name')) for x in tracks]
    # write the new archive, then the list
    with open(silo_file, 'wb') as fh:
        fh.write(new)
    open(cc, 'w').write(json.dumps(doc, indent=1))
    # re-read what was written
    back = open(silo_file, 'rb').read()
    assert back == new and sha(back) == row['sha256']
    rr = trk_res3.read_res3(back)
    assert [m['sha256'] for m in members] == [sha(rr.payload[lo:lo + sz]) for _, lo, sz in rr.members]
    json.load(open(cc))
    print('installed %s (%d bytes, %d members) and converted-content.json; original saved in %s' % (
        silo_file, len(new), len(r.members), orig_file))
    print('borrowed:', track['borrowed_members'])
    if today:
        print('NOTE %s will not load on today\'s port builds until the common race load: %s' % (
            code.upper(), '; '.join(s_['refusal'] for s_ in today)))


if __name__ == '__main__':
    import argparse
    if len(sys.argv) > 1 and sys.argv[1] in ('multi', 'check'):
        ap = argparse.ArgumentParser()
        ap.add_argument('cmd')
        ap.add_argument('items', nargs='+', help='multi: CODE=ROOT ...; check: CODE')
        ap.add_argument('--src', default=None)
        ap.add_argument('--assets', default='assets')
        ap.add_argument('--dlc-out', default=None)
        ap.add_argument('--dlc-disabled', action='store_true')
        a = ap.parse_args()
        if a.cmd == 'check':
            for c in a.items:
                _checked(c.lower(), _packed(c.lower(), a.src)[0])
        else:
            tr = {}
            for it in a.items:
                c, _, src = it.partition('=')
                tr[c.lower()] = src or None
            multi(tr, a.assets, a.dlc_out, not a.dlc_disabled)
        sys.exit(0)
    ap = argparse.ArgumentParser()
    ap.add_argument('code', nargs='?', default='sc')
    ap.add_argument('--src', default=None, help='converter --out root (default export/a2r/override)')
    ap.add_argument('--assets', default='assets', help='asset copy folder under silo (default assets)')
    ap.add_argument('--dlc-out', default=None, help='write the DLC replacement package into this folder instead of installing')
    ap.add_argument('--dlc-id', default=None, help='package id (default: from the track, see dlc_out.TRACKS)')
    ap.add_argument('--dlc-disabled', action='store_true', help='write the package with "enabled": false')
    a = ap.parse_args()
    if a.dlc_out:
        package(a.code.lower(), a.src, a.dlc_out, a.dlc_id, not a.dlc_disabled)
    else:
        main(a.code.lower(), a.src, a.assets)
