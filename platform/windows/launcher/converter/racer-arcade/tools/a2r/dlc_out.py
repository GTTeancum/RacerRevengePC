r"""DLC package output (--dlc-out) shared by every converter and installer. Step A: replacement packages.

NOT ORIGINAL BEHAVIOUR: add-on content is a PC enhancement of the port. Field rules:
D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\DLC-JSON.md (schema rrpc-dlc/1); design:
D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\DESIGN.md sections 2, 5, 7. This is the only place that
writes a package folder or a dlc.json; the converters call it:

    trk_silo.py multi --dlc-out DIR ma=ROOT ...    one package per track, each replacing its own stock course
    trk_silo.py CODE --src ROOT --dlc-out DIR      track packed under a stock course code (also reached from
    trk_convert.py N --code-as CODE --dlc-out DIR   and racer-n64 n2r_convert.py MODEL --code-as CODE --dlc-out DIR)
    pod_silo.py install NN... --dlc-out DIR        pods in stock slots (also pod_convert.py / pugwis_convert.py --dlc-out)
    ui_silo.py install --dlc-out DIR               menu and text archives (also ui_convert.py / pugwis_ui.py --dlc-out)

With --dlc-out an installer runs its usual checks and writes DIR\<package id>\ (files at their game-relative
paths + dlc.json) INSTEAD of installing into a silo asset copy; nothing else is written.

Packages from an asset copy that is already installed (reads its converted-content.json, never writes the copy):

    python -I dlc_out.py from-set --set <asset copy> --out DIR --track [--code ma sc ...] [--disabled]
        one package per converted track row (an asset copy may hold several tracks, each replacing a
        different stock course: trk_silo.py multi); --code limits it to those course codes
    python -I dlc_out.py from-set --set <asset copy> --out DIR --pods 04 11 15 17 23 24 --id arcade-pods
    python -I dlc_out.py from-set --set <asset copy> --out DIR --ui
    python -I dlc_out.py all --out DIR             the seven step-A packages from the sets listed in STEP_A

Add-on packages (kind course / pod, contract dlc-contract-2, DLC-JSON.md "Add-on items"), one per addon_out.py folder:

    python -I dlc_out.py addons --out DIR [--src racer-arcade\addon-out] [--only FOLDER ...] [--stock INSTALL]
        reads each folder's addon-manifest.json (never shipped), writes DIR\<id>\ (files + dlc.json with items,
        strings and facts); every contract-2 rule is checked first (codes, archives, strings, inherit, preview
        format, pod stem / PHY Prefix / camera / material names / UI members)

Every replacement package: kind "replacement", "personal": true with the do-not-share note, one replacements row per file
with expects = the stock size/SHA-256 the file was built for, folder name == package id. The converter facts
ride along under "facts" (informational; the port recomputes everything from the bytes).
"""
import hashlib
import json
import os
import re
import shutil
import sys

SCHEMA = 'rrpc-dlc/1'
CONTRACT = 'dlc-contract-1'
AUTHOR = 'RacerRevengePC converters'
PERSONAL_NOTE = 'built from your own copy of the source games; do not share'
SHARED_UI = 'shared-ui-arcade-demolition'
SHARED_UI_RANGE = '>=1.0.0 <2.0.0'
ID = re.compile(r'^[a-z0-9][a-z0-9-]{2,47}$')
VERSION = re.compile(r'^\d{1,6}\.\d{1,6}\.\d{1,6}$')
REFUSED = ('.exe', '.dll', '.com', '.bat', '.cmd', '.ps1', '.vbs', '.js', '.py', '.sh', '.lua', '.so',
           '.dylib', '.scr', '.msi', '.jar', '.wsf', '.hta')
MANIFEST_BYTES, MAX_FILES, PACKAGE_BYTES, PATH_CHARS, PATH_DEPTH = 1 << 20, 4096, 512 << 20, 240, 8
# the loading presenter reads LOC.RES with a cap of exactly the stock size (DESIGN.md "Phase 1 as built"):
# a larger replacement is refused there for now. Warned about here, never silently shipped.
STOCK_SIZE_CAPPED = ('loc.res',)

# Step-A package names (data, agreed with the port session: DESIGN.md "Step A decisions").
# Tracks: keyed by the converter's natural course code; the package id gets "-<stock code>" appended.
TRACKS = {
    'eb': ('arcade-etti-iv', 'Etti IV'),
    'lc': ('arcade-malastare', 'Malastare'),
    'sm': ('n64-spice-mine-run', 'Spice Mine Run'),
    'bw': ('n64-beedos-wild-ride', "Beedo's Wild Ride"),
}
PACKAGES = {
    'arcade-pods': dict(name='Racer Arcade pods', what='Star Wars Racer Arcade pods', depends=[SHARED_UI]),
    'demolition-pugwis': dict(name='Pugwis (Star Wars: Demolition)', what='Pugwis from Star Wars: Demolition',
                              depends=[SHARED_UI]),
    SHARED_UI: dict(name='Shared menu files for arcade and Demolition pods',
                    what='Menu, text and physics-table archives shared by arcade-pods and demolition-pugwis',
                    depends=[]),
}


def sha(b):
    return hashlib.sha256(b).hexdigest()


def display_title(s):
    """'ETTI IV' -> 'Etti IV', "SMUGGLER'S COVE" -> "Smuggler's Cove" (roman numerals stay capitals)."""
    return ' '.join(w if re.fullmatch(r'[IVX]+', w) else w.capitalize() if not w.startswith(('"', "'")) else w[0] + w[1:].capitalize()
                    for w in s.split(' '))


def pop_option(args, flag, has_value=True):
    """Take FLAG [VALUE] out of a plain argv list (the converters that parse sys.argv by hand)."""
    if flag not in args:
        return None, args
    i = args.index(flag)
    if not has_value:
        return True, args[:i] + args[i + 1:]
    if i + 1 >= len(args):
        raise SystemExit('%s needs a value' % flag)
    return args[i + 1], args[:i] + args[i + 2:]


def norm(raw):
    parts = re.split(r'[\\/]', raw) if isinstance(raw, str) and raw and len(raw) <= PATH_CHARS else []
    if not parts or len(parts) > PATH_DEPTH:
        return None
    for p in parts:
        if p in ('', '.', '..') or p[-1] in ' .' or any(ord(c) < 0x20 or c in ':*?"<>|\x7f' for c in p):
            return None
    return '/'.join(parts)


def _printable(s, limit):
    return isinstance(s, str) and len(s.encode('utf-8')) <= limit and not any(ord(c) < 0x20 or ord(c) == 0x7f for c in s)


def _is_program(data):
    if data[:4] == b'\x7fELF':
        return True
    if len(data) >= 64 and data[:2] == b'MZ':
        o = int.from_bytes(data[60:64], 'little')
        return data[o:o + 4] == b'PE\0\0'
    return False


def entry(path, data, replaces, role=None, facts=None):
    """One package file that replaces the stock file at the same path. replaces = {bytes, sha256} of the stock file."""
    return dict(path=path, data=data, replaces=dict(bytes=int(replaces['bytes']), sha256=str(replaces['sha256']).lower()),
                role=role, facts=facts)


def _existing_is_ours(folder, new_paths):
    """An existing package folder may be replaced only when nothing in it is foreign: every file is dlc.json, a
    file its own dlc.json lists, or (interrupted earlier write, no dlc.json yet) a path about to be written."""
    listed = set(new_paths)
    m = os.path.join(folder, 'dlc.json')
    if os.path.isfile(m):
        try:
            old = json.loads(open(m, 'rb').read().decode('utf-8'))
            if old.get('id') != os.path.basename(folder):
                return False
            listed |= {(norm(f.get('path')) or '').lower() for f in old.get('files', [])}
        except (ValueError, UnicodeDecodeError, AttributeError):
            return False
    for base, dirs, files in os.walk(folder):
        if os.path.islink(base):
            return False
        for n in files:
            rel = os.path.relpath(os.path.join(base, n), folder).replace(os.sep, '/').lower()
            if rel != 'dlc.json' and rel not in listed:
                return False
    return True


def write_package(out_root, pkg_id, name, description, entries, enabled=True, dependencies=(), facts=None,
                  version='1.0.0', priority=0):
    """Write <out_root>\\<pkg_id>\\ (the files + dlc.json), re-read it and return the folder.
    Every rule of DLC-JSON.md is checked BEFORE anything is written; a package that would not fit is refused."""
    def need(ok, why):
        if not ok:
            raise SystemExit('dlc-out %s: %s' % (pkg_id, why))
    need(ID.match(pkg_id or '') is not None, 'package id is not 3-48 characters of a-z, 0-9 and -')
    need(name and _printable(name, 63), 'name must be 1-63 printable characters')
    description = (description.rstrip() + ' ' if description else '') + PERSONAL_NOTE[0].upper() + PERSONAL_NOTE[1:] + '.'
    need(_printable(description, 1024), 'description must be at most 1024 printable characters')
    need(VERSION.match(version) is not None, 'version is not MAJOR.MINOR.PATCH')
    need(isinstance(priority, int) and -1000 <= priority <= 1000, 'priority must be -1000..1000')
    need(entries and len(entries) <= MAX_FILES, 'a replacement package needs 1..4096 files')
    seen, total, clean = set(), 0, []
    for e in entries:
        p = norm(e['path'])
        need(p, 'path "%s" is not a plain relative path' % e['path'])
        low = p.lower()
        need(low != 'dlc.json' and not low.startswith('dlc/'), 'path "%s" is not a stock path' % p)
        need(not low.endswith(REFUSED) and not _is_program(e['data']), '"%s" is a program or script; packages carry data only' % p)
        need(low not in seen, '"%s" is listed twice' % p)
        need(re.fullmatch(r'[0-9a-f]{64}', e['replaces']['sha256']) is not None and e['replaces']['bytes'] >= 0,
             'stock size/SHA-256 of "%s" is missing' % p)
        need(sha(e['data']) != e['replaces']['sha256'], '"%s" is the stock file itself: nothing to replace' % p)
        seen.add(low); total += len(e['data'])
        clean.append(dict(e, path=p))
        if low in STOCK_SIZE_CAPPED and len(e['data']) > e['replaces']['bytes']:
            print('WARNING %s: %s is %d bytes, stock is %d: a replacement larger than stock is refused by the loading '
                  'screen for now (DESIGN.md "Phase 1 as built")' % (pkg_id, p, len(e['data']), e['replaces']['bytes']))
    need(total <= PACKAGE_BYTES, 'package is larger than 512 MiB')
    deps = []
    for d in dependencies:
        need(ID.match(d['id']) is not None and d['id'] != pkg_id, 'dependency "%s" is not another package id' % d['id'])
        deps.append(dict(id=d['id'], version=d.get('version', '')))
    need(len(deps) <= 64, 'too many dependencies')

    folder = _prepare_folder(out_root, pkg_id, seen, need)

    doc = dict(schema=SCHEMA, id=pkg_id, name=name, author=AUTHOR, version=version, description=description,
               enabled=bool(enabled), kind='replacement', priority=priority, requires_port=CONTRACT,
               personal=True, personal_note=PERSONAL_NOTE, replace=True,
               replacements=[{'target': e['path'], 'expects': e['replaces'], 'with': e['path']} for e in clean],
               items=[],
               files=[dict(dict(path=e['path'], bytes=len(e['data']), sha256=sha(e['data'])),
                           **({'role': e['role']} if e.get('role') else {})) for e in clean])
    if deps:
        doc['dependencies'] = deps
    info = dict(facts or {})
    per_file = {e['path']: e['facts'] for e in clean if e.get('facts')}
    if per_file:
        info['files'] = per_file
    if info:
        doc['facts'] = info
    raw = (json.dumps(doc, indent=1, ensure_ascii=False, default=float) + '\n').encode('utf-8')
    need(len(raw) <= MANIFEST_BYTES, 'dlc.json would be larger than 1 MiB')
    return _write_folder(folder, raw, clean, total, enabled)


def _prepare_folder(out_root, pkg_id, seen, need):
    """<out_root>\\<pkg_id>: refuses an install root or a foreign folder; removes an earlier output of this package."""
    out_root = os.path.abspath(out_root)
    for stock_mark in ('SLUS_202.68', 'TRACKS', 'PODS'):  # an install root or an asset copy is never a DLC folder
        need(not os.path.exists(os.path.join(out_root, stock_mark)),
             '%s looks like a game install (it holds %s); give the DLC folder, not the install' % (out_root, stock_mark))
    folder = os.path.join(out_root, pkg_id)
    if os.path.exists(folder):
        need(os.path.isdir(folder) and not os.path.islink(folder) and _existing_is_ours(folder, seen),
             '%s exists and holds files that are not an earlier output of this package: refusing to replace it' % folder)
        shutil.rmtree(folder)
    return folder


def _write_folder(folder, raw, clean, total, enabled):
    """Write the files, then dlc.json (last), re-read everything and compare with the file list."""
    for e in clean:
        dst = os.path.join(folder, *e['path'].split('/'))
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        with open(dst, 'wb') as fh:
            fh.write(e['data'])
    with open(os.path.join(folder, 'dlc.json'), 'wb') as fh:  # last: an interrupted write leaves no dlc.json
        fh.write(raw)
    # re-read what was written
    back = json.loads(open(os.path.join(folder, 'dlc.json'), 'rb').read().decode('utf-8'))
    on_disk = set()
    for base, dirs, files in os.walk(folder):
        on_disk |= {os.path.relpath(os.path.join(base, n), folder).replace(os.sep, '/') for n in files}
    assert on_disk == {f['path'] for f in back['files']} | {'dlc.json'}, 'package folder differs from its file list'
    for f in back['files']:
        b = open(os.path.join(folder, *f['path'].split('/')), 'rb').read()
        assert len(b) == f['bytes'] and sha(b) == f['sha256'], 'written file differs from its row: ' + f['path']
    print('dlc-out: %s  (%d file%s, %d bytes, %s)' % (folder, len(clean), '' if len(clean) == 1 else 's', total,
                                                     'enabled' if enabled else 'disabled'))
    return folder


# ---------------------------------------------------------------- the three package shapes
def track_identity(source, code):
    """Package id and display name of a track packed under a stock course code, from the converter's own
    "source" record (converted-content.json / manifest). Known step-A tracks use the agreed names; any other
    track gets <source kind>-<its name>-<code>."""
    kind = source.get('kind') or 'arcade'
    full = source.get('name') or source.get('arcade_name') or 'track'
    base, short = TRACKS.get((source.get('natural_code') or '').lower(), (None, None))
    if base is None:
        short = full
        base = (kind + '-' + re.sub(r'[^a-z0-9]+', '-', full.lower().replace("'", '')).strip('-'))[:44].strip('-')
    game = ('Star Wars Racer Arcade course %s, %s' % (source.get('arcade_course'), full) if kind == 'arcade' else
            'Episode I Racer (N64) %s' % full if kind == 'n64' else '%s %s' % (kind, full))
    return ('%s-%s' % (base, code.lower()), '%s (replaces stock course %s)' % (short, code.upper()),
            '%s, converted and packed as the %s course.' % (game, code.upper()))


def track_package(out_root, source, code, data, replaces, facts=None, limits=None, extra=None, enabled=True,
                  pkg_id=None, version='1.0.0'):
    auto_id, name, description = track_identity(source, code)
    rel = 'TRACKS/%s.RES' % code.upper()
    info = dict(source=source)
    if limits:
        info['limits'] = limits
    info.update(extra or {})
    return write_package(out_root, pkg_id or auto_id, name, description,
                         [entry(rel, data, replaces, 'course archive', facts)], enabled=enabled, facts=info, version=version)


def _slots(entries):
    return sorted({m.group(1) for e in entries for m in [re.match(r'(?i)PODS/POD(\d\d)/', e['path'])] if m})


def _known(pkg_id, name, what):
    info = PACKAGES.get(pkg_id, {})
    name, what = name or info.get('name'), what or info.get('what')
    if not name or not what:
        raise SystemExit('dlc-out: package id "%s" has no agreed name; pass a name and a description' % pkg_id)
    return name, what, [dict(id=d, version=SHARED_UI_RANGE) for d in info.get('depends', [SHARED_UI])]


def pods_package(out_root, pkg_id, entries, enabled=True, name=None, what=None, facts=None, version='1.0.0'):
    """Pod archives (and changed CAM files) in their stock slots; depends on the shared menu package."""
    name, what, deps = _known(pkg_id, name, what)
    s = _slots(entries)
    slots = s[0] if len(s) == 1 else ', '.join(s[:-1]) + ' and ' + s[-1]
    return write_package(out_root, pkg_id, name, '%s in the stock slot%s %s.' % (what, '' if len(s) == 1 else 's', slots),
                         entries, enabled=enabled, dependencies=deps, facts=facts, version=version)


def ui_package(out_root, entries, pkg_id=SHARED_UI, enabled=True, name=None, what=None, facts=None, version='1.0.0'):
    name, what, deps = _known(pkg_id, name, what)
    return write_package(out_root, pkg_id, name, what + '.', entries, enabled=enabled, dependencies=deps, facts=facts,
                         version=version)


# ---------------------------------------------------------------- add-on packages (kind course / pod / bundle)
# Field rules: DLC-JSON.md "Add-on items" (contract dlc-contract-2). Every rule below is checked BEFORE anything is
# written; a package that would not fit is refused. One rule for every course and every pod (nothing keyed to one).
CONTRACT_ADDON = 'dlc-contract-2'
ADDON_KINDS = ('course', 'pod', 'bundle')
ITEM_ID = re.compile(r'^[a-z0-9][a-z0-9-]{1,46}$')
STRING_KEY = re.compile(r'^[A-Z0-9_]{1,47}$')
COURSE_CODE = re.compile(r'^[a-z][a-z0-9]$')
STEM = re.compile(r'^[a-z0-9_]{3,16}$')
RECORDS_KEY = re.compile(r'^[a-z0-9-]{1,47}$')
STOCK_COURSES = ('ta', 'tb', 'tc', 'ra', 'rc', 'ma', 'mb', 'mc', 'ga', 'gb', 'gc', 'sa', 'sc', 'sb')
CLASSES = ('A', 'B', 'C')
MODES = ('single', 'time_trial', 'vs')   # DLC-JSON.md: free play and two-player VS; never the tournament
ENHANCEMENTS = ('surface_rules', 'zero_g', 'n64_floor_table', 'boost_strips', 'n64_hazards')
UI_LIST_FIELDS = ('character', 'character_pages', 'pod_pages', 'shadow', 'portraits', 'background', 'garage_model')
UI_ONE_FIELDS = ('rider_model', 'results_icon')
ADDON_MANIFEST = 'addon-manifest.json'      # the converters' own manifest: never a package file
STOCK_INSTALL = (__import__('os').environ['RR_CONVERT_STOCK']+'')


def _font_text(s):
    """menu text: 1..255 characters the menu fonts draw (every stock menu font covers ASCII 32..126)."""
    return isinstance(s, str) and 1 <= len(s) <= 255 and all(32 <= ord(c) <= 126 for c in s)


# Rider names are drawn by every name font, including the race HUD font, which lacks the double quote (ARK "BUMPY"
# ROOSE crashed the Results standings, main port 2026-11-19). Rule (main port's set): letters, digits, space, ! ' ( ) + , - . / : ;
RIDER_NAME_CHARS = set("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 !'()+,-./:;")  # main port's settled set


def _rider_text(s):
    return _font_text(s) and all(c in RIDER_NAME_CHARS for c in s)


def _archive_members(data):
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import trk_res3
    return trk_res3.read_res3(data)


def _shared_pod_names(stock):
    """Shared original names: material/texture names the stock pod archives carry under one name in two or more
    pods, plus the stock menu archives' names (spcgarge, specmap1 ...). None when no stock install is at hand."""
    pods = os.path.join(stock, 'PODS')
    if not os.path.isdir(pods):
        return None
    seen = {}
    for d in sorted(os.listdir(pods)):
        p = os.path.join(pods, d, d + '.RES')
        if os.path.isfile(p):
            for n in _archive_members(open(p, 'rb').read()).names():
                seen.setdefault(n.replace('/', '\\').split('\\')[-1].lower().rsplit('.', 1)[0], set()).add(d)
    shared = {k for k, v in seen.items() if len(v) >= 2}
    for base, _, fs in os.walk(os.path.join(stock, 'UI')):
        for f in fs:
            if f.upper().endswith('.RES'):
                try:
                    names = _archive_members(open(os.path.join(base, f), 'rb').read()).names()
                except (AssertionError, ValueError, OSError, UnicodeDecodeError):
                    continue
                shared |= {n.replace('/', '\\').split('\\')[-1].lower().rsplit('.', 1)[0] for n in names
                           if not re.search(r'\d\d', n.split('\\')[-1])}
    return shared


def _pod_names(data):
    """every material / texture name in a pod or menu archive: texture and material members, psm TextureMaps,
    PSG material names."""
    r = _archive_members(data)          # also puts this folder on the import path
    import geo_psg
    out = set()
    for n, d in r.items():
        low = n.lower()
        base = low.split('\\')[-1].rsplit('.', 1)[0]
        if low.endswith(('.pst', '.psm', '.atm', '.tga')):
            out.add(base)
        if low.endswith('.psm'):
            out |= {m.group(1).lower() for m in re.finditer(r'(?im)^\s*TextureMap_\d+\s*=\s*(\S+)', d.decode('latin-1'))}
        if low.endswith('.psg'):
            out |= {m.lower() for m in geo_psg.parse(d)['materials']}
    return r, out


def _stem_name(name, stem):
    return any(name.startswith(p) for p in (stem + '_', stem + 'c', stem + 'e', 'flame' + stem, 'concring' + stem)) or name == stem


def check_addon(pkg_id, kind, items, strings, files, need, stock=STOCK_INSTALL):
    """Contract-2 rules of DLC-JSON.md for the items, strings and the archives they name. files = {path: bytes}."""
    low_files = {p.lower(): p for p in files}
    need(kind in ADDON_KINDS, 'kind must be course, pod or bundle for add-on items')
    need(isinstance(items, list) and 1 <= len(items) <= 256, 'an add-on package needs 1..256 items')
    need(ADDON_MANIFEST not in {p.split('/')[-1].lower() for p in files}, '%s must not be shipped' % ADDON_MANIFEST)
    need(isinstance(strings, dict) and isinstance(strings.get('english'), dict), 'strings needs an "english" table')
    for lang, table in strings.items():
        need(re.fullmatch(r'[a-z]{2,16}', lang) is not None and isinstance(table, dict), 'strings: bad language "%s"' % lang)
        for k, v in table.items():
            need(STRING_KEY.match(k) is not None, 'strings: key "%s" is not [A-Z0-9_]{1,47}' % k)
            need(_font_text(v), 'strings: %s "%s" is not 1..255 characters the menu fonts draw (ASCII 32..126)' % (k, v))
            if k.upper().startswith('RIDER_'):
                need(_rider_text(v), 'strings: rider name %s "%s" uses a character the race HUD font cannot draw (allowed: letters, digits, space and the main port punctuation set)' % (k, v))
    used = set()

    def key(it, field, required=True):
        k = it.get(field)
        if k is None and not required:
            return
        need(isinstance(k, str) and k in strings['english'], 'item %s: %s "%s" has no English string' % (it.get('id'), field, k))
        used.add(k)

    def listed(path, what):
        need(isinstance(path, str) and norm(path) and norm(path).lower() in low_files, '%s "%s" is not a listed file' % (what, path))
        return files[low_files[norm(path).lower()]]

    ids, codes, stems = set(), set(), set()
    shared = None
    for it in items:
        need(isinstance(it, dict) and it.get('kind') in ('course', 'pod'), 'every item needs kind course or pod')
        need(kind == 'bundle' or it['kind'] == kind, 'kind "%s" holds only %s items' % (kind, kind))
        iid = it.get('id')
        need(isinstance(iid, str) and ITEM_ID.match(iid) is not None and iid not in ids,
             'item id "%s" is not [a-z0-9][a-z0-9-]{1,46} or is used twice' % iid)
        ids.add(iid)
        if it['kind'] == 'course':
            code = it.get('code')
            need(isinstance(code, str) and COURSE_CODE.match(code) is not None, 'course %s: code must be 2 characters [a-z][a-z0-9]' % iid)
            need(code not in STOCK_COURSES and code not in codes, 'course %s: code "%s" is a stock code or used twice' % (iid, code))
            codes.add(code)
            need(it.get('archive') == 'TRACKS/%s.RES' % code.upper(), 'course %s: archive must be TRACKS/%s.RES' % (iid, code.upper()))
            r = _archive_members(listed(it['archive'], 'course %s archive' % iid))
            names = {n.lower() for n in r.names()}
            for need_member in ('tracks\\%s.scb' % code, 'tracks\\a%s.spl' % code, 'tracks\\m%s.spl' % code):
                need(need_member in names, 'course %s: archive has no %s' % (iid, need_member))
            left = [n for n in names for s_ in STOCK_COURSES
                    if re.match(r'^(a|m)?' + s_ + r'(\d\d)?(tvcam)?\.', n.split('\\')[-1])]
            need(not left, 'course %s: members still named by a stock code: %s' % (iid, left[:4]))
            key(it, 'name_key')
            key(it, 'planet_name_key', False)
            need(it.get('phy_class') in CLASSES and it.get('ai_class') in CLASSES, 'course %s: phy_class / ai_class must be A, B or C' % iid)
            rv = it.get('rival')
            m = re.fullmatch(r'stock:(\d\d)', rv) if isinstance(rv, str) else None
            need(rv == 'none' or (m and 1 <= int(m.group(1)) <= 26) or
                 (isinstance(rv, str) and re.fullmatch(r'[a-z0-9][a-z0-9-]{2,47}/[a-z0-9][a-z0-9-]{1,46}', rv)),
                 'course %s: rival must be stock:NN (01..26), <package-id>/<pod-item-id> or none' % iid)
            ls = it.get('loading_screen')
            need(isinstance(ls, dict) and len(ls) == 1 and ('file' in ls or 'stock' in ls), 'course %s: loading_screen needs file or stock' % iid)
            if 'file' in ls:
                need(ls['file'] == 'UI/TEXTURES/LD_%s.PST' % code.upper(), 'course %s: loading screen must be UI/TEXTURES/LD_%s.PST' % (iid, code.upper()))
                b = listed(ls['file'], 'loading screen')
                w = lambda at: int.from_bytes(b[at:at + 4], 'little')   # noqa: E731  (loading presenter layout rule)
                need(len(b) == 287772 and (w(0), w(4), w(8), w(1036), w(1040), w(1044), w(1048)) == (1, 1, 1, 448, 640, 0, 0),
                     'course %s: loading screen is not the 640x448 one-level layout the loading presenter reads' % iid)
            else:
                need(ls['stock'] in STOCK_COURSES, 'course %s: loading_screen.stock is not a stock code' % iid)
            cs = it.get('course_screen')
            if cs is not None:
                need(isinstance(cs, dict) and set(cs) <= {'stock_template', 'preview'} and cs.get('stock_template') in STOCK_COURSES,
                     'course %s: course_screen needs stock_template (a stock code) and optionally preview' % iid)
                if 'preview' in cs:
                    b = listed(cs['preview'], 'course %s preview' % iid)
                    need(cs['preview'].upper().endswith('.PST'), 'course %s: preview must be a PST' % iid)
                    import geo_pst
                    t = geo_pst.parse(b)
                    got = (t['kind'], len(t['levels']), t['levels'][-1]['w'], t['levels'][-1]['h'])
                    ui = os.path.join(stock, 'UI', 'UICOURSE', 'UICOURSE.RES')
                    want = (1, 1, 256, 256)        # every stock holo_<code>.pst (checked against the install when present)
                    if os.path.isfile(ui):
                        st = geo_pst.parse(_archive_members(open(ui, 'rb').read()).get('ui\\textures\\holo_%s.pst' % cs['stock_template']))
                        want = (st['kind'], len(st['levels']), st['levels'][-1]['w'], st['levels'][-1]['h'])
                    need(got == want, 'course %s: preview is %s (kind, levels, w, h); the stock texture it stands in for is %s' % (iid, got, want))
            cb = it.get('course_background')
            if cb is not None:
                # DLC-JSON.md course_background: {"file": listed PST} the size/format of the stock course-screen background
                # (UICOURSE.RES <planet>_bkg.pst, read by src\assets\original_ui_course_background50kg.cpp), or "stock:<code>"
                if isinstance(cb, str):
                    need(cb.startswith('stock:') and cb[6:] in STOCK_COURSES, 'course %s: course_background string must be stock:<stock code>' % iid)
                else:
                    need(isinstance(cb, dict) and set(cb) == {'file'}, 'course %s: course_background needs {"file": ...} or "stock:<code>"' % iid)
                    b = listed(cb['file'], 'course %s background' % iid)
                    need(cb['file'].upper().endswith('.PST'), 'course %s: course_background must be a PST' % iid)
                    import geo_pst
                    t = geo_pst.parse(b)
                    got = (len(b), t['kind'], len(t['levels']), t['levels'][0]['w'], t['levels'][0]['h'])
                    want = (287772, 1, 1, 640, 448)   # every stock <planet>_bkg.pst (checked against the install when present)
                    ui = os.path.join(stock, 'UI', 'UICOURSE', 'UICOURSE.RES')
                    if os.path.isfile(ui):
                        sb = _archive_members(open(ui, 'rb').read()).get('ui\\textures\\tat_bkg.pst')
                        st = geo_pst.parse(sb)
                        want = (len(sb), st['kind'], len(st['levels']), st['levels'][0]['w'], st['levels'][0]['h'])
                    need(got == want, 'course %s: course_background is %s (bytes, kind, levels, w, h); the stock background is %s' % (iid, got, want))
                    w = lambda at: int.from_bytes(b[at:at + 4], 'little')   # noqa: E731  (the port decoder's layout rule)
                    need((w(0), w(4), w(8), w(1036), w(1040), w(1044), w(1048)) == (1, 1, 1, 448, 640, 0, 0),
                         'course %s: course_background is not the 640x448 one-level layout the course-background decoder reads' % iid)
            inh = it.get('inherit') or {}
            need(isinstance(inh, dict) and set(inh) <= {'tuning', 'particles'} and all(v in STOCK_COURSES for v in inh.values()),
                 'course %s: inherit takes tuning / particles = a stock code' % iid)
            need('tracks\\%s.tun' % code in names or 'tuning' in inh, 'course %s: no tracks\\%s.tun and no inherit.tuning' % (iid, code))
            need(all('particle\\demoprt.' + e in names for e in ('prt', 'pst', 'psm')) or 'particles' in inh,
                 'course %s: no particle\\demoprt.* and no inherit.particles' % iid)
            modes = it.get('modes', list(MODES))
            need(isinstance(modes, list) and modes and set(modes) <= set(MODES) and len(set(modes)) == len(modes),
                 'course %s: modes must be a subset of single, time_trial, vs' % iid)
            need(RECORDS_KEY.match(it.get('records_key', iid)) is not None, 'course %s: records_key is not [a-z0-9-]{1,47}' % iid)
            need(set(it.get('enhancements', [])) <= set(ENHANCEMENTS), 'course %s: unknown enhancement' % iid)
            need('hall_defaults' not in it, 'course %s: hall_defaults is not written by this tool' % iid)
        else:
            stem = it.get('stem')
            need(isinstance(stem, str) and STEM.match(stem) is not None and not re.fullmatch(r'pod\d\d', stem) and stem not in stems,
                 'pod %s: stem must be [a-z0-9_]{3,16}, not pod<NN>, unique' % iid)
            stems.add(stem)
            up = stem.upper()
            need(it.get('archive') == 'PODS/%s/%s.RES' % (up, up), 'pod %s: archive must be PODS/%s/%s.RES' % (iid, up, up))
            need(it.get('camera') == 'PODS/%s/%s.CAM' % (up, up), 'pod %s: camera must be PODS/%s/%s.CAM' % (iid, up, up))
            listed(it['camera'], 'pod %s camera' % iid)
            key(it, 'name_key')
            key(it, 'short_name_key', False)
            r, names = _pod_names(listed(it['archive'], 'pod %s archive' % iid))
            phy = [n for n in r.names() if n.lower() == 'pods\\%s\\%s.phy' % (stem, stem)]
            need(len(phy) == 1, 'pod %s: archive has no Pods\\%s\\%s.phy' % (iid, stem, stem))
            pre = re.search(r'(?im)^\s*Prefix\s*=\s*"([^"]*)"', r.get(phy[0]).decode('latin-1'))
            need(pre is not None and pre.group(1) == stem, 'pod %s: PHY [Pod Setup] Prefix must equal the stem' % iid)
            ui = it.get('ui')
            need(isinstance(ui, dict) and ui.get('archive') == 'UI/%s_UI.RES' % up, 'pod %s: ui.archive must be UI/%s_UI.RES' % (iid, up))
            need(set(ui) <= {'archive', 'layout_slot', 'huttese_name_key'} | set(UI_LIST_FIELDS) | set(UI_ONE_FIELDS), 'pod %s: unknown ui field (no stats_image)' % iid)
            if 'huttese_name_key' in ui:
                hk = ui['huttese_name_key']; used.add(hk)
                need(isinstance(hk, str) and all(hk in t for t in strings.values()), 'pod %s: ui.huttese_name_key names a missing string' % iid)
                need(all(set(t.get(hk, '')) <= set(' ()ABCDEFGHIJKLMNOPQRSTUVWXYZ') for t in strings.values()),
                     'pod %s: Huttese name may use only A-Z, space and brackets' % iid)
            else:
                nk = 'RIDER_%s' % up
                need(all(set(t.get(nk, '')) <= set(' ()ABCDEFGHIJKLMNOPQRSTUVWXYZ') for t in strings.values()),
                     'pod %s: name has characters the Huttese font lacks; add ui.huttese_name_key' % iid)
            if 'layout_slot' in ui:
                need(isinstance(ui['layout_slot'], str) and re.fullmatch(r'\d\d', ui['layout_slot']) is not None and int(ui['layout_slot']) <= 24,
                     'pod %s: ui.layout_slot must name a stock rider "00".."24"' % iid)
            ur, unames = _pod_names(listed(ui['archive'], 'pod %s ui archive' % iid))
            members = {n.lower() for n in ur.names()}
            named = set()
            for f_ in UI_ONE_FIELDS:
                need(isinstance(ui.get(f_), str), 'pod %s: ui.%s is required' % (iid, f_))
                named.add(ui[f_].lower())
            for f_ in UI_LIST_FIELDS:
                v = ui.get(f_, [] if f_ == 'garage_model' else None)
                need(isinstance(v, list) and all(isinstance(x, str) for x in v), 'pod %s: ui.%s must be a list of members' % (iid, f_))
                named |= {x.lower() for x in v}
            need(named <= members, 'pod %s: ui names members the archive lacks: %s' % (iid, sorted(named - members)[:4]))
            need(members <= named, 'pod %s: ui archive members no field names: %s' % (iid, sorted(members - named)[:4]))
            if shared is None:
                shared = _shared_pod_names(stock)
            if shared is not None:
                bad = sorted(n for n in names | unames if not _stem_name(n, stem) and n not in shared)
                need(not bad, 'pod %s: material/texture names neither start with the stem nor are shared original names: %s' % (iid, bad[:6]))
            else:
                print('WARNING %s: no stock install at %s, pod material names not checked against the shared names' % (pkg_id, stock))
            for f_ in ('voice', 'announcer'):
                need(f_ not in it, 'pod %s: %s is not written by this tool' % (iid, f_))
            need(it.get('ai', 'addon_courses') in ('addon_courses', 'never'), 'pod %s: ai must be addon_courses or never' % iid)
            need(it.get('unlock', 'always') == 'always', 'pod %s: unlock must be always' % iid)
    unused = set(strings['english']) - used
    need(not unused, 'strings: keys no item uses: %s' % sorted(unused))


def write_addon_package(out_root, pkg_id, name, description, kind, items, strings, entries, enabled=True, facts=None,
                        version='1.0.0', priority=0, stock=STOCK_INSTALL):
    """Write <out_root>\\<pkg_id>\\ for an add-on package (kind course / pod / bundle, contract 2): the files + dlc.json.
    entries: [dict(path, data, role)] (new files, nothing replaced). Everything is checked before anything is written."""
    def need(ok, why):
        if not ok:
            raise SystemExit('dlc-out %s: %s' % (pkg_id, why))
    need(ID.match(pkg_id or '') is not None, 'package id is not 3-48 characters of a-z, 0-9 and -')
    need(name and _printable(name, 63), 'name must be 1-63 printable characters')
    description = (description.rstrip() + ' ' if description else '') + PERSONAL_NOTE[0].upper() + PERSONAL_NOTE[1:] + '.'
    need(_printable(description, 1024), 'description must be at most 1024 printable characters (%d)' % len(description))
    need(VERSION.match(version) is not None, 'version is not MAJOR.MINOR.PATCH')
    need(isinstance(priority, int) and -1000 <= priority <= 1000, 'priority must be -1000..1000')
    need(entries and len(entries) <= MAX_FILES, 'a package needs 1..4096 files')
    seen, total, clean = set(), 0, []
    for e in entries:
        p = norm(e['path'])
        need(p, 'path "%s" is not a plain relative path' % e['path'])
        low = p.lower()
        need(low != 'dlc.json' and not low.startswith('dlc/'), 'path "%s" is not a package path' % p)
        need(not low.endswith(REFUSED) and not _is_program(e['data']), '"%s" is a program or script; packages carry data only' % p)
        need(low not in seen, '"%s" is listed twice' % p)
        seen.add(low); total += len(e['data'])
        clean.append(dict(e, path=p))
    need(total <= PACKAGE_BYTES, 'package is larger than 512 MiB')
    check_addon(pkg_id, kind, items, strings, {e['path']: e['data'] for e in clean}, need, stock)
    doc = dict(schema=SCHEMA, id=pkg_id, name=name, author=AUTHOR, version=version, description=description,
               enabled=bool(enabled), kind=kind, priority=priority, requires_port=CONTRACT_ADDON,
               personal=True, personal_note=PERSONAL_NOTE, strings=strings, items=items,
               files=[dict(dict(path=e['path'], bytes=len(e['data']), sha256=sha(e['data'])),
                           **({'role': e['role']} if e.get('role') else {})) for e in clean])
    if facts:
        doc['facts'] = facts
    raw = (json.dumps(doc, indent=1, ensure_ascii=False, default=float) + '\n').encode('utf-8')
    need(len(raw) <= MANIFEST_BYTES, 'dlc.json would be larger than 1 MiB')
    folder = _prepare_folder(out_root, pkg_id, seen, need)
    return _write_folder(folder, raw, clean, total, enabled)


# ---------------------------------------------------------------- add-on packages from addon_out.py folders
def _value(v):
    return v.get('value') if isinstance(v, dict) else v


def _strength(v):
    return v.get('strength') if isinstance(v, dict) else None


def addon_item(mi):
    """addon-manifest item -> dlc.json item (the published fields only; everything else goes to facts)."""
    if mi['kind'] == 'course':
        up = mi['code'].upper()
        it = dict(kind='course', id=mi['id'], code=mi['code'], archive=mi['archive'], name_key='COURSE_%s' % up,
                  planet_name_key='PLANET_%s' % up, phy_class=_value(mi['phy_class']), ai_class=_value(mi['ai_class']),
                  rival=_value(mi['rival']) or 'none', loading_screen=mi['loading_screen'])
        if mi.get('course_screen'):
            it['course_screen'] = {k: mi['course_screen'][k] for k in ('stock_template', 'preview') if k in mi['course_screen']}
        if mi.get('course_background'):
            cb = mi['course_background']
            it['course_background'] = cb if isinstance(cb, str) else {'file': cb['file']}
        if mi.get('inherit'):
            it['inherit'] = dict(mi['inherit'])
        it.update(modes=list(mi.get('modes') or MODES), records_key=mi.get('records_key_final', mi['id']))
        if mi.get('enhancements'):
            it['enhancements'] = list(mi['enhancements'])
        return it
    up = mi['stem'].upper()
    ui = {k: v for k, v in mi['ui'].items() if k == 'archive' or k in UI_LIST_FIELDS or k in UI_ONE_FIELDS}
    if mi['ui'].get('huttese_name_key'):
        ui['huttese_name_key'] = mi['ui']['huttese_name_key']
    if mi['ui'].get('layout_slot'):
        ui['layout_slot'] = mi['ui']['layout_slot']   # stock rider whose Rider-screen layout places pod and character
    return dict(kind='pod', id=mi['id'], stem=mi['stem'], archive=mi['archive'], camera=mi['camera_presets'],
                name_key='RIDER_%s' % up, ui=ui, ai='addon_courses', unlock='always')


def _course_description(mf, mi):
    dn = mi['display_name']
    src = mf['source']['game']
    ai, rv = mi['ai_class'], mi['rival']
    rival = _value(rv)
    parts = ['%s on %s, from %s, as an add-on course (code %s, physics class %s).' % (
        display_title(dn['track']), display_title(dn['planet']), src, mi['code'], _value(mi['phy_class']))]
    parts.append('AI class %s (%s evidence)%s.' % (_value(ai), _strength(ai),
                 '; rival %s, %s (%s evidence: the source track favourite pilot)' % (rival, rv.get('pilot') or rv.get('stock_pod_name', ''), _strength(rv)) if rival else
                 '; no rival (%s)' % ('the source has no per-course rival' if _strength(rv) == 'none' else
                                      'its source favourite has no Revenge pod')))
    if mi.get('inherit'):
        parts.append('Race tuning from stock %s and pod effect particles from stock %s, read from your install.' % (
            mi['inherit']['tuning'].upper(), mi['inherit']['particles'].upper()))
    parts.append('%s loading screen, generated course-screen preview%s; no announcer intro lines.' % (
        'Stock' if 'stock' in mi['loading_screen'] else 'Custom',
        ' and course background' if isinstance(mi.get('course_background'), dict) else ''))
    return ' '.join(parts)


POD_STANDIN_LABELS = (('stats', 'stats'), ('pilot voice', 'pilot voice'), ('PODS/', 'camera presets'),
                      ('AI combat', 'AI behaviour'), ('material definitions', 'material set-ups'),
                      ('menu material definitions', 'material set-ups'), ('stock member', 'a few stock model members'))


def _pod_description(mf, mi):
    m = re.match(r'stock slot (\d\d)', mi.get('replaces_in_step_a', ''))
    labels = []
    for s_ in mf['stand_ins']:
        if s_['kind'].startswith(('BORROWED', 'STOCK DEFINITIONS')):
            labels += [lab for key, lab in POD_STANDIN_LABELS if s_['what'].startswith(key) or key in s_['what']][:1]
    notes = ' Menu character note in facts.' if any(s_['kind'] == 'NOTE' for s_ in mf['stand_ins']) else ''
    return ('%s from %s, as an add-on pod (stem %s, %d wreck pieces). Borrowed from stock pod %s until own data exists: %s.%s' % (
        display_title(mi['display_name']['english']), mf['source']['game'].split(' pilot')[0].split(' character')[0], mi['stem'],
        mi['wreck_pieces_total'], m.group(1) if m else '?', ', '.join(dict.fromkeys(labels)), notes))


def from_addon_folder(src, out_root, enabled=True, version='1.0.0', stock=STOCK_INSTALL):
    """One add-on package from an addon_out.py folder: reads addon-manifest.json (never shipped), re-checks every
    listed file against its size / SHA-256, writes <out_root>\\<id>\\ with the files and dlc.json."""
    mf = json.load(open(os.path.join(src, ADDON_MANIFEST), encoding='utf-8'))
    if mf.get('schema') != 'rrpc-addon-manifest/1':
        raise SystemExit('%s: unexpected manifest schema' % src)
    pp = mf['proposed_package']
    entries = []
    for f in mf['files']:
        data = open(os.path.join(src, *f['path'].split('/')), 'rb').read()
        if len(data) != f['bytes'] or sha(data) != f['sha256']:
            raise SystemExit('%s: %s differs from its manifest row' % (src, f['path']))
        entries.append(dict(path=f['path'], data=data, role=f.get('role')))
    on_disk = set()
    for base, dirs, files in os.walk(src):
        on_disk |= {os.path.relpath(os.path.join(base, n), src).replace(os.sep, '/') for n in files}
    extra = on_disk - {f['path'] for f in mf['files']} - {ADDON_MANIFEST}
    if extra:
        raise SystemExit('%s: files the manifest does not list: %s' % (src, sorted(extra)))
    items = [addon_item(mi) for mi in mf['items']]
    kinds = {it['kind'] for it in items}
    kind = kinds.pop() if len(kinds) == 1 else 'bundle'
    mi = mf['items'][0]
    desc = _course_description(mf, mi) if mi['kind'] == 'course' else _pod_description(mf, mi)
    published = {k for it in items for k in it}
    facts = dict(tool=r'racer-arcade\tools\a2r\dlc_out.py addons', built_from=os.path.abspath(src),
                 not_original_behaviour=mf.get('not_original_behaviour'), source=mf.get('source'),
                 items={x['id']: {k: v for k, v in x.items() if k not in published or isinstance(v, dict) and k in
                                  ('phy_class', 'ai_class', 'rival')} for x in mf['items']},
                 stand_ins=mf.get('stand_ins'), validation=mf.get('validation'))
    for k in ('members', 'pod_members', 'ui_members', 'converter_flags'):
        if mf.get(k):
            facts[k] = mf[k]
    return write_addon_package(out_root, pp['id'], pp['name'], desc, kind, items, mf['strings'], entries, enabled, facts,
                               version or pp.get('version', '1.0.0'), stock=stock)


def addons(src_root, out_root, only=(), enabled=True, version='1.0.0', stock=STOCK_INSTALL):
    out = []
    for d in sorted(os.listdir(src_root)):
        if only and d not in only:
            continue
        if os.path.isfile(os.path.join(src_root, d, ADDON_MANIFEST)):
            out.append(from_addon_folder(os.path.join(src_root, d), out_root, enabled, version, stock))
    if not out:
        raise SystemExit('%s: no add-on folders' % src_root)
    return out


# ---------------------------------------------------------------- from an installed asset copy
def _set_doc(set_dir):
    cc = os.path.join(set_dir, 'converted-content.json')
    doc = json.load(open(cc))
    if doc.get('schema') != 'a2r-converted-content/1' or not isinstance(doc.get('files'), list):
        raise SystemExit('%s: unexpected schema' % cc)
    return doc


def _set_entries(set_dir, rows, role):
    out = []
    for row in rows:
        data = open(os.path.join(set_dir, *row['path'].split('/')), 'rb').read()
        if len(data) != row['bytes'] or sha(data) != row['sha256']:
            raise SystemExit('%s: %s is not the file its converted-content.json row describes' % (set_dir, row['path']))
        out.append(entry(row['path'], data, row['replaces'], role, row.get('facts')))
    return out


def from_set(set_dir, out_root, what, pods=(), pkg_id=None, enabled=True, version='1.0.0', codes=()):
    """what = 'track' | 'pods' | 'ui'. Reads the asset copy and its converted-content.json; writes only out_root."""
    codes = [c.lower() for c in codes or ()]
    doc = _set_doc(set_dir)
    note = dict(built_from=os.path.abspath(set_dir))
    if what == 'track':
        # one package per converted track row (an asset copy may hold several, each replacing a different
        # stock course); codes = the course codes to package (default: every track row)
        rows = [r for r in doc['files'] if r.get('course_code') and (not codes or r['course_code'].lower() in codes)]
        if not rows:
            raise SystemExit('%s: no converted track row%s' % (set_dir, ' for %s' % ', '.join(codes) if codes else ''))
        if pkg_id and len(rows) > 1:
            raise SystemExit('--id names one package; %s holds %d tracks (pick one with --code)' % (set_dir, len(rows)))
        out = []
        for row in rows:
            rec = row.get('track') or doc     # rows written before the multi-track layout: the record is top level
            e = _set_entries(set_dir, [row], None)[0]
            extra = dict(note, **{k: rec[k] for k in ('borrowed_members', 'kept_original', 'port_standins_today') if k in rec})
            if 'sky' in rec:
                extra['sky'] = {k: v for k, v in rec['sky'].items() if k != 'note'}
            out.append(track_package(out_root, rec['source'], row['course_code'], e['data'], e['replaces'], row.get('facts'),
                                     rec.get('limits'), extra, enabled, pkg_id, version))
        return out[0] if len(out) == 1 else out
    if what == 'pods':
        want = ['%02d' % int(p) for p in pods]
        rows = [r for r in doc['files'] if r.get('pod') in want]
        missing = sorted(set(want) - {r['pod'] for r in rows})
        if missing:
            raise SystemExit('%s: no converted pod row for %s' % (set_dir, ', '.join(missing)))
        note['sources'] = {r['path']: r.get('source') for r in rows}
        return pods_package(out_root, pkg_id or 'arcade-pods', _set_entries(set_dir, rows, 'pod archive'), enabled,
                            facts=note, version=version)
    if what == 'ui':
        rows = sorted((r for r in doc['files'] if r.get('kind') == 'ui'), key=lambda r: r['path'].lower())
        if not rows:
            raise SystemExit('%s: no converted menu rows' % set_dir)
        note['sources'] = sorted({str(r.get('source')) for r in rows})
        note['pods'] = sorted({p for r in rows for p in r.get('pods', []) if p != 'LOC'})
        return ui_package(out_root, _set_entries(set_dir, rows, 'menu archive'), pkg_id or SHARED_UI, enabled,
                          facts=note, version=version)
    raise SystemExit('unknown package shape %s' % what)


# The seven step-A packages and the asset copy each is built from (folders under racer-arcade\silo).
# The four tracks all replace TRACKS/SC.RES at equal priority, so only one is enabled (DESIGN.md step A).
STEP_A = [
    ('assets', 'track', (), None, True),               # arcade-etti-iv-sc
    ('assets-lc', 'track', (), None, False),           # arcade-malastare-sc
    ('assets-smr2', 'track', (), None, False),         # n64-spice-mine-run-sc (smr2 = the current converter's output)
    ('assets-bwr', 'track', (), None, False),          # n64-beedos-wild-ride-sc
    ('assets', 'pods', ('04', '11', '15', '17', '23', '24'), 'arcade-pods', True),
    ('assets', 'pods', ('16',), 'demolition-pugwis', True),
    ('assets', 'ui', (), SHARED_UI, True),
]


def main(argv=None):
    import argparse
    silo = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'silo'))
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    f = sub.add_parser('from-set')
    f.add_argument('--set', required=True, help='asset copy folder (absolute, or a name under silo)')
    f.add_argument('--out', required=True)
    g = f.add_mutually_exclusive_group(required=True)
    g.add_argument('--track', action='store_true'); g.add_argument('--pods', nargs='+'); g.add_argument('--ui', action='store_true')
    f.add_argument('--id', default=None); f.add_argument('--disabled', action='store_true')
    f.add_argument('--version', default='1.0.0')
    f.add_argument('--code', nargs='+', default=(), help='with --track: only these course codes (default every track row)')
    a_ = sub.add_parser('all')
    a_.add_argument('--out', required=True); a_.add_argument('--silo', default=silo)
    ad = sub.add_parser('addons')
    ad.add_argument('--src', default=os.path.normpath(os.path.join(silo, '..', 'addon-out')))
    ad.add_argument('--out', required=True); ad.add_argument('--only', nargs='+', default=())
    ad.add_argument('--disabled', action='store_true'); ad.add_argument('--version', default='1.0.0')
    ad.add_argument('--stock', default=STOCK_INSTALL, help='stock install (read only) for the shared-name and preview checks')
    a = ap.parse_args(argv)
    if a.cmd == 'addons':
        addons(a.src, a.out, a.only, not a.disabled, a.version, a.stock)
    elif a.cmd == 'from-set':
        set_dir = a.set if os.path.isabs(a.set) else os.path.join(silo, a.set)
        from_set(set_dir, a.out, 'track' if a.track else 'pods' if a.pods else 'ui', a.pods or (), a.id,
                 not a.disabled, a.version, a.code)
    else:
        for name, what, pods, pkg_id, enabled in STEP_A:
            from_set(os.path.join(a.silo, name), a.out, what, pods, pkg_id, enabled)


if __name__ == '__main__':
    main()
