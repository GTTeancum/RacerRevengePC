r"""Install a converted track packed under an original course code into the silo asset copy.

    python -I trk_silo.py sc        (after: python -I trk_convert.py 1 --code-as sc)

1. checks the packed archive against every original limit listed in notes\silo-override.md;
2. copies each original file being replaced into silo\originals\ (same relative path; never
   overwritten once saved; the silo copy must still equal the installed original's pinned SHA);
3. writes the new files into silo\assets\ and silo\assets\converted-content.json;
4. re-reads everything that was written.
Never touches C:\Games (read only, for the identity check).
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
import geo_pst  # noqa: E402

GAME = r'C:/Games/Star Wars Racer Revenge'
SILO = os.path.join(SCR, 'silo')
BS = chr(92)

# original limits per target code (notes\silo-override.md 1.1 / 1.2; port file:line in each entry)
LIMITS = {
    'sc': [
        ('archive bytes <= original cap', 'archive_bytes', '<=', 2974353, 'tests\\native\\sc_cold_sources50la.h:15 (== today, cap for converted files)'),
        ('decoded bytes <= original cap', 'decoded_bytes', '<=', 26714112, 'sc_cold_sources50la.h:15,93-95'),
        ('archive entries <= original cap', 'entries', '<=', 984, 'sc_cold_sources50la.h:16,93-95'),
        ('archive provider entries', 'entries', '<=', 65536, 'src\\assets\\original_standard_archive_provider50la.h:408'),
        ('archive provider decoded bytes', 'decoded_bytes', '<=', 256 * 1024 * 1024, 'original_standard_archive_provider50la.h:408'),
        ('particle re-read cap entries', 'entries', '<=', 4096, 'terrain_interactive_publication50du.cpp:798-805'),
        ('particle re-read cap bytes', 'decoded_bytes', '<=', 64 * 1024 * 1024, 'terrain_interactive_publication50du.cpp:798-805'),
        ('map nodes <= 256', 'map_nodes', '<=', 256, 'storage cap 256 (track_start50ad.h:6)'),
        ('AI nodes <= 256', 'ai_nodes', '<=', 256, 'storage cap 256'),
        ('zones <= 256', 'zones', '<=', 256, 'track_navigation_shared50mb.inl worldValid'),
        ('placed COL <= SC original 252', 'placed_col', '<=', 252, 'tests\\native\\sc_runtime_ground50la.h:51 (cap stays)'),
        ('collision owners <= 1021', 'placed_col', '<=', 1021, 'authored_runtime_ground50cp_fixture.h:37-43'),
        ('instances <= SC original 4107', 'instances', '<=', 4107, 'sc_cold_sources50la.h:107'),
        ('textures (materials) <= 64', 'images', '<=', 64, 'terrain_interactive_continuation50du.cpp:2813-2815'),
        ('texture RGBA bytes <= 8 MiB', 'rgba_bytes', '<=', 8 * 1024 * 1024, 'terrain_interactive_continuation50du.cpp:2812'),
        ('texture source bytes <= 32 MiB', 'texture_source_bytes', '<=', 32 * 1024 * 1024, 'terrain_interactive_continuation50du.cpp:2805'),
        ('every zone SearchType List', 'search_types', '==', ['List'], 'sc_runtime_ground50la.h:44-47'),
        ('cut cameras == 3', 'cuts', '==', 3, 'original_ta_cut_authoring_candidate.h:143-145'),
        ('intro events > 0', 'intro_events', '>', 0, 'native_race_intro_transport50la.inl:89'),
    ],
}
ORIGINAL_PINS = {'sc': ('TRACKS/SC.RES', 2974353, '8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d')}


def sha(b):
    return hashlib.sha256(b).hexdigest()


def check(f, rows):
    out, ok = [], True
    for label, key, op, ref, where in rows:
        v = f.get(key)
        good = {'<=': lambda: v <= ref, '==': lambda: v == ref, '>': lambda: v > ref}[op]()
        ok &= good
        out.append(dict(check=label, value=v, op=op, limit=ref, port=where, ok=bool(good)))
    return ok, out


def main(code, src_root=None, assets='assets'):
    """The only way anything is installed into silo\assets (geometry and track helpers both use it).
    Refuses when the packed archive is older than the geometry/texture output it was built from
    (rerun trk_convert.py N --code-as CODE first), and holds a lock so two installs cannot interleave."""
    root = os.path.join(src_root or os.path.join(SCR, 'export', 'a2r', 'override'), code)
    man = json.load(open(os.path.join(root, 'manifest.json')))
    import a2r_space
    geo_man = os.path.join(SCR, 'export', 'a2r', 'course%d' % man['course'], 'manifest.json')
    packed = os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper())
    if os.path.getmtime(geo_man) > os.path.getmtime(packed):
        raise SystemExit('geometry output (%s) is newer than the packed %s: run trk_convert.py %d --code-as %s first' % (
            geo_man, packed, man['course'], code))
    lock = os.path.join(SILO, '.install.lock')
    try:
        fd = os.open(lock, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
    except FileExistsError:
        raise SystemExit('another silo install is running (%s)' % lock)
    os.write(fd, str(os.getpid()).encode()); os.close(fd)
    try:
        _install(code, root, man, assets)
    finally:
        os.remove(lock)


def _install(code, root, man, assets='assets'):
    res_rel = 'TRACKS/%s.RES' % code.upper()
    new = open(os.path.join(root, 'install', 'TRACKS', '%s.RES' % code.upper()), 'rb').read()
    f = trk_facts.facts(new, code)
    r = trk_res3.read_res3(new)
    f['texture_source_bytes'] = sum(len(d) for n, d in r.items() if n.lower().startswith('textures' + BS) and n.lower().endswith('.pst'))
    ok, limits = check(f, LIMITS[code])
    for row in limits:
        print('%-36s %12s %2s %-12s %s' % (row['check'], row['value'], row['op'], row['limit'], 'ok' if row['ok'] else 'FAIL'))
    if not ok:
        raise SystemExit('limits failed: nothing installed')
    # originals: the silo copy must still be the installed original
    rel, pbytes, psha = ORIGINAL_PINS[code]
    silo_file = os.path.join(SILO, assets, *rel.split('/'))
    orig_file = os.path.join(SILO, 'originals', *rel.split('/'))
    game_file = os.path.join(GAME, *rel.split('/'))
    g = open(game_file, 'rb').read()
    assert len(g) == pbytes and sha(g) == psha, 'installed original does not match its pin'
    if not os.path.exists(orig_file):
        cur = open(silo_file, 'rb').read()
        assert sha(cur) == psha, 'silo copy is not the original and no backup exists: refusing'
        os.makedirs(os.path.dirname(orig_file), exist_ok=True)
        shutil.copyfile(silo_file, orig_file)
    assert sha(open(orig_file, 'rb').read()) == psha
    # write the new archive
    with open(silo_file, 'wb') as fh:
        fh.write(new)
    members = []
    for name, lo, sz in r.members:
        prov = man['files'].get(name, {}).get('provenance', '')
        members.append(dict(name=name, offset=lo, bytes=sz, sha256=sha(r.payload[lo:lo + sz]),
                            borrowed=prov.startswith('BORROWED')))
    ld_rel = 'UI/TEXTURES/LD_%s.PST' % code.upper()
    ld = open(os.path.join(SILO, assets, *ld_rel.split('/')), 'rb').read()
    assert ld == open(os.path.join(GAME, *ld_rel.split('/')), 'rb').read(), 'loading screen is not the original'
    doc = dict(
        schema='a2r-converted-content/1',
        about=('Asset-root admission list for a silo build of the port (notes\\silo-override.md 5). Presence of this '
               'file is the switch. "files" lists every replaced file: path relative to the asset root with "/" '
               'separators, size and SHA-256 of the new file, size and SHA-256 of the original it replaces, and the '
               'content counts ("facts") computed from the new file the way the port counts them '
               '(tools\\a2r\\trk_facts.py; definitions checked against the port pins of original SC and TA).'),
        source=dict(arcade_course=man['course'], arcade_name=man['name'], natural_code=__import__('a2r_space').CODES[man['course']],
                    packed_as=code, converter='tools\\a2r\\trk_convert.py %d --code-as %s' % (man['course'], code),
                    transform=man['transform']),
        files=[dict(path=res_rel, bytes=len(new), sha256=sha(new), replaces=dict(bytes=pbytes, sha256=psha),
                    course_code=code, facts={k: v for k, v in f.items() if k not in ('archive_bytes', 'archive_sha256')},
                    members=members)],
        kept_original=[dict(path=ld_rel, bytes=len(ld), sha256=sha(ld),
                            reason='loading screen pinned by hash (authored_loading_presenter50du.h:129-136); shared by SA/SB/SC')],
        borrowed_members=[m['name'] for m in members if m['borrowed']],
        sky=dict(present=True, member='skies' + BS + '%s.cub' % code, cube_file='%s.cub' % code,
                 bytes=len(r.get('skies' + BS + '%s.cub' % code)),
                 sha256=sha(r.get('skies' + BS + '%s.cub' % code)),
                 layout='original CUB: 1 palette + 1 cube, format 32, masks 00ff x4 / ffff / 0000, 48 tiles x 4 IPUM mips 16..128',
                 note=('This converted course HAS a sky cube although original %s has none (sc_cold_sources50la.h:109-114 '
                       'expects NumberOfSkyCubes=0 and no .cub). The port should draw it through the general sky path '
                       '(sky50gw: camera-centred, Y+1024, drawn first without depth/fog), with Fog ColorRGB as the clear colour.') % code.upper()),
        limits=limits)
    cc = os.path.join(SILO, assets, 'converted-content.json')
    open(cc, 'w').write(json.dumps(doc, indent=1))
    # re-read what was written
    back = open(silo_file, 'rb').read()
    assert back == new and sha(back) == doc['files'][0]['sha256']
    rr = trk_res3.read_res3(back)
    assert [m['sha256'] for m in members] == [sha(rr.payload[lo:lo + sz]) for _, lo, sz in rr.members]
    json.load(open(cc))
    print('installed %s (%d bytes, %d members) and converted-content.json; original saved in %s' % (
        silo_file, len(new), len(r.members), orig_file))
    print('facts:', json.dumps({k: v for k, v in f.items() if k != 'archive_sha256'}))
    print('borrowed:', doc['borrowed_members'])


if __name__ == '__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('code', nargs='?', default='sc')
    ap.add_argument('--src', default=None, help='converter --out root (default export/a2r/override)')
    ap.add_argument('--assets', default='assets', help='asset copy folder under silo (default assets)')
    a = ap.parse_args()
    main(a.code, a.src, a.assets)
