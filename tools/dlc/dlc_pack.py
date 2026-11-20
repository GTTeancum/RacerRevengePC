"""Racer Revenge PC DLC package tool (phases 1-2). PC enhancement, not original behaviour.

Same rules as the game's startup checker (src/assets/dlc_package50dl.cpp,
dlc_files50dl.cpp and, for add-on items of contract dlc-contract-2,
dlc_registry50dl.cpp). Field reference: work/scratch/dlc/DLC-JSON.md.

  dlc_pack.py check   <package-or-DLC-root> [--stock <install>]
      Validate one package folder, or every package under a DLC root (then
      dependencies, conflicts and replacement collisions are resolved as the game
      does). With --stock, replacements are checked against the installed stock
      files, and add-on pod items get the shared-original-name check (read from
      the stock pod and Garage archives); without --stock that one check is
      skipped (reported). Exit code 0 when every enabled package would be admitted.
  dlc_pack.py files   <package>
      (Re)generate the "files" list of dlc.json from the folder contents (sizes
      and SHA-256). Other fields are kept; the file is written UTF-8 without BOM.
  dlc_pack.py replace <out-folder> --stock <install> --id ID --name NAME --author A
                      --version X.Y.Z --target STOCK/PATH=SOURCE_FILE [...]
                      [--description D] [--priority N] [--personal] [--personal-note T]
                      [--depends ID[:RANGE]] [--enabled|--disabled]
      Write a new replacement package folder: copies each SOURCE_FILE to the
      package under the target's own path and records the stock size/SHA-256
      it was built for. Never writes into the stock install.
"""
import argparse
import hashlib
import json
import re
import shutil
import struct
import sys
import zlib
from pathlib import Path

SCHEMA = 'rrpc-dlc/1'
CONTRACT = 'dlc-contract-1'          # replacement packages
ITEMS_CONTRACT = 'dlc-contract-2'    # add-on items (courses, pods)
PERSONAL_NOTE = 'built from your own copy of the source games; do not share'
REFUSED = ('.exe', '.dll', '.com', '.bat', '.cmd', '.ps1', '.vbs', '.js', '.py', '.sh', '.lua', '.so',
           '.dylib', '.scr', '.msi', '.jar', '.wsf', '.hta')
KINDS = {'course', 'pod', 'menu_assets', 'replacement', 'bundle'}
ID = re.compile(r'^[a-z0-9][a-z0-9-]{2,47}$')
VERSION = re.compile(r'^(\d{1,6})\.(\d{1,6})\.(\d{1,6})$')
MANIFEST_BYTES, MAX_FILES, PACKAGE_BYTES, PATH_CHARS, PATH_DEPTH = 1 << 20, 4096, 512 << 20, 240, 8


class Refused(Exception):
    pass


def sha256(path):
    with open(path, 'rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def norm(raw):
    if not isinstance(raw, str) or not raw or len(raw) > PATH_CHARS:
        return None
    parts = re.split(r'[\\/]', raw)
    if len(parts) > PATH_DEPTH:
        return None
    for p in parts:
        if p in ('', '.', '..') or p[-1] in ' .' or any(ord(c) < 0x20 or c in ':*?"<>|\x7f' for c in p):
            return None
    return '/'.join(parts)


def printable(s, limit):
    return isinstance(s, str) and len(s.encode('utf-8')) <= limit and not any(ord(c) < 0x20 or ord(c) == 0x7f for c in s)


def version(text):
    m = VERSION.match(text or '')
    return tuple(int(x) for x in m.groups()) if m else None


def satisfies(v, rng):
    for term in (rng or '').split():
        m = re.match(r'^(<=|>=|<|>|=)?(.*)$', term)
        op, want = m.group(1) or '=', version(m.group(2))
        if want is None:
            return False
        ok = {'=': v == want, '<': v < want, '<=': v <= want, '>': v > want, '>=': v >= want}[op]
        if not ok:
            return False
    return True


def range_valid(rng):
    terms = (rng or '').split()
    return len(terms) <= 4 and all(re.match(r'^(<=|>=|<|>|=)?\d{1,6}\.\d{1,6}\.\d{1,6}$', t) for t in terms)


def need(ok, reason):
    if not ok:
        raise Refused(reason)


def read_manifest(folder):
    folder = Path(folder)
    m = folder / 'dlc.json'
    need(m.is_file() and not m.is_symlink(), 'no dlc.json in the package folder')
    raw = m.read_bytes()
    need(0 < len(raw) <= MANIFEST_BYTES, 'dlc.json is empty or larger than 1 MiB')
    need(not raw.startswith(b'\xef\xbb\xbf'), 'dlc.json starts with a byte-order mark (save it as UTF-8 without BOM)')
    try:
        d = json.loads(raw.decode('utf-8'))
    except (UnicodeDecodeError, ValueError) as e:
        raise Refused(f'dlc.json is not valid UTF-8 JSON: {e}')
    need(isinstance(d, dict), 'dlc.json must hold one JSON object')

    def s(key, required=True):
        need(not required or key in d, f'dlc.json: missing field "{key}"')
        v = d.get(key, '')
        need(isinstance(v, str), f'dlc.json: field "{key}" must be text')
        return v

    def b(key, required=False):
        need(not required or key in d, f'dlc.json: missing field "{key}"')
        v = d.get(key, False)
        need(isinstance(v, bool), f'dlc.json: field "{key}" must be true or false')
        return v

    need(s('schema') == SCHEMA, f'dlc.json schema "{d.get("schema")}" is not supported (needs "{SCHEMA}")')
    p = dict(folder=folder, id=s('id'))
    need(ID.match(p['id']) is not None, f'package id "{p["id"]}" is not 3-48 characters of a-z, 0-9 and -')
    need(folder.name == p['id'], f'folder name differs from the package id "{p["id"]}"')
    p['name'], p['author'], p['version_text'] = s('name'), s('author'), s('version')
    p['description'], p['kind'] = s('description', False), s('kind')
    need(p['name'] and printable(p['name'], 63), 'name must be 1-63 printable characters')
    need(printable(p['author'], 63), 'author must be at most 63 printable characters')
    need(printable(p['description'], 1024), 'description must be at most 1024 printable characters')
    p['version'] = version(p['version_text'])
    need(p['version'] is not None, f'version "{p["version_text"]}" is not MAJOR.MINOR.PATCH')
    p['enabled'] = b('enabled', True)
    p['replace'], p['personal'] = b('replace'), b('personal')
    note = s('personal_note', False)
    need(printable(note, 255), 'personal_note must be at most 255 printable characters')
    p['personal_note'] = note or (PERSONAL_NOTE if p['personal'] else '')
    pr = d.get('priority', 0)
    need(isinstance(pr, int) and not isinstance(pr, bool), 'priority must be a whole number')
    need(-1000 <= pr <= 1000, 'priority must be -1000..1000')
    p['priority'] = pr
    rp = s('requires_port', False)
    need(not rp or rp in (CONTRACT, ITEMS_CONTRACT),
         f'package needs port data contract "{rp}"; this build has "{CONTRACT}" and "{ITEMS_CONTRACT}"')
    need(p['kind'] in KINDS, f'kind "{p["kind"]}" is not course, pod, menu_assets, replacement or bundle')
    need(p['kind'] != 'menu_assets', 'kind "menu_assets" needs a later version of the port')
    items = d.get('items')
    need(isinstance(items, list), 'dlc.json: missing field "items"' if items is None else 'dlc.json: field "items" must be a list')
    need(len(items) <= 256, 'dlc.json: too many entries in "items"')
    need(p['kind'] != 'replacement' or not items, 'kind "replacement" must list no items')
    need(p['kind'] not in ('course', 'pod') or items, f'kind "{p["kind"]}" lists no items')
    need(not items or rp == ITEMS_CONTRACT, f'add-on items need "requires_port": "{ITEMS_CONTRACT}"')
    p['raw'] = d
    files = d.get('files')
    need(isinstance(files, list), 'dlc.json: field "files" must be a list')
    need(len(files) <= MAX_FILES, 'dlc.json: too many entries in "files"')
    p['files'], seen, total = [], set(), 0
    for row in files:
        need(isinstance(row, dict), 'files: every entry must be an object')
        path = norm(row.get('path'))
        need(path, f'files: path "{row.get("path")}" is not a plain relative path')
        need(path.lower() != 'dlc.json', 'files: dlc.json itself must not be listed')
        need(not path.lower().endswith(REFUSED), f'files: "{path}" is a program or script file; packages carry data only')
        n, h = row.get('bytes'), str(row.get('sha256', '')).lower()
        need(isinstance(n, int) and not isinstance(n, bool) and n >= 0, 'dlc.json: field "bytes" must be a whole number')
        need(re.fullmatch(r'[0-9a-f]{64}', h) is not None, f'files: sha256 of "{path}" is not 64 hex digits')
        need(path.lower() not in seen, f'files: "{path}" is listed twice')
        seen.add(path.lower()); total += n
        need(total <= PACKAGE_BYTES, 'package is larger than 512 MiB')
        p['files'].append(dict(path=path, bytes=n, sha256=h))
    reps = d.get('replacements', [])
    need(isinstance(reps, list), 'dlc.json: field "replacements" must be a list')
    need(not reps or p['replace'], 'replacements are listed but "replace" is not true')
    p['replacements'], targets = [], set()
    for row in reps:
        need(isinstance(row, dict), 'replacements: every entry must be an object')
        need('member' not in row, 'replacements: member-level replacement needs a later version of the port')
        t, w = norm(row.get('target')), norm(row.get('with'))
        need(t, f'replacements: target "{row.get("target")}" is not a plain stock path')
        need(w, f'replacements: with "{row.get("with")}" is not a plain package path')
        need(not (t.lower() == 'dlc' or t.lower().startswith('dlc/')), f'replacements: target "{t}" is inside the DLC folder')
        need(not t.lower().endswith(REFUSED), f'replacements: target "{t}" is a program file')
        need(t.lower() not in targets, f'replacements: target "{t}" is replaced twice')
        targets.add(t.lower())
        e = row.get('expects')
        need(isinstance(e, dict), f'replacements: "{t}" needs "expects" (the stock file it was built for)')
        eb, eh = e.get('bytes'), str(e.get('sha256', '')).lower()
        need(isinstance(eb, int) and not isinstance(eb, bool) and eb >= 0, 'dlc.json: field "bytes" must be a whole number')
        need(re.fullmatch(r'[0-9a-f]{64}', eh) is not None, f'replacements: expects.sha256 of "{t}" is not 64 hex digits')
        need(w.lower() in seen, f'replacements: "{w}" is not listed in files')
        p['replacements'].append(dict(target=t, expect_bytes=eb, expect_sha256=eh, with_=w))
    need(p['kind'] != 'replacement' or p['replacements'], 'kind "replacement" lists no replacements')
    need(p['kind'] not in ('course', 'pod') or not p['replacements'],
         f'kind "{p["kind"]}" must not list replacements (use kind "bundle")')
    p['dependencies'] = []
    for row in d.get('dependencies', []):
        need(isinstance(row, dict), 'dependencies: every entry must be an object')
        did, rng, opt = row.get('id', ''), row.get('version', ''), row.get('optional', False)
        need(isinstance(did, str) and ID.match(did) and did != p['id'], f'dependencies: "{did}" is not another package id')
        need(isinstance(rng, str) and range_valid(rng), f'dependencies: version range "{rng}" is not understood')
        need(isinstance(opt, bool), 'dlc.json: field "optional" must be true or false')
        p['dependencies'].append(dict(id=did, range=rng, optional=opt))
    p['conflicts'] = []
    for row in d.get('conflicts', []):
        need(isinstance(row, dict), 'conflicts: every entry must be an object')
        cid = row.get('id', '')
        need(isinstance(cid, str) and ID.match(cid) and cid != p['id'], f'conflicts: "{cid}" is not another package id')
        p['conflicts'].append(dict(id=cid, reason=str(row.get('reason', ''))))
    p['pods'], p['courses'], p['strings'] = [], [], {}
    if items:
        read_items(p, d)
    return p


# ---------------------------------------------------------------- add-on items (dlc-contract-2)
# Same rules as src/assets/dlc_registry50dl.cpp (readItems / checkItems).
ITEM_ID = re.compile(r'^[a-z0-9][a-z0-9-]{1,46}$')
STEM = re.compile(r'^[a-z0-9_]{3,16}$')
KEY = re.compile(r'^[A-Z0-9_]{1,47}$')
RECORDS_KEY = re.compile(r'^[a-z0-9-]{1,47}$')
STOCK_CODES = {'ta', 'tb', 'tc', 'rc', 'ma', 'mb', 'mc', 'ra', 'ga', 'gb', 'gc', 'sa', 'sc', 'sb'}
STOCK_RACERS = {1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 15, 16, 19, 20, 22, 24, 17, 18, 23, 14, 21, 25, 26}
POD_CAPACITY = dict(file=16 << 20, decoded=16 << 20, entries=512)
UI_CAPACITY = dict(file=16 << 20, decoded=32 << 20, entries=512)
COURSE_CAPACITY = dict(file=64 << 20, decoded=128 << 20, entries=10000)
TEXT_MEMBER, CAMERA_BYTES, LOOSE_PST = 8192, 64 << 10, 4 << 20


def font_text(s, limit):  # printable ASCII: the stock menu fonts draw every glyph of it
    return isinstance(s, str) and 0 < len(s) <= limit and all(0x20 <= ord(c) <= 0x7e for c in s)


# The one allowed character set for display strings (a string can be a racer
# name, drawn by the race HUD font too). Same set as the port's
# src/assets/dlc_name_font50dl.h (dlcNameCharacter), which the HUD source checks
# against its glyph table at compile time.
NAME_PUNCTUATION = "!'()+,-./:;"
NAME_SET_TEXT = "space, A-Z, a-z, 0-9 and ! ' ( ) + , - . / : ;"


def name_character(c):
    return c == ' ' or ('A' <= c <= 'Z') or ('a' <= c <= 'z') or ('0' <= c <= '9') or c in NAME_PUNCTUATION


def read_res(path, cap):
    """RES3 names and decoded storage (layout of src/assets/import_decode48c.cpp)."""
    b = Path(path).read_bytes()
    need(0 < len(b) <= cap['file'], f'{Path(path).name} is outside the archive capacity ({cap["file"]} bytes)')
    try:
        v, meta, codec, z, tb = struct.unpack_from('<5I', b, 0)
        assert v == 3 and tb >= 2 and tb % 2 == 0
        nb = tb // 2 - 1
        sizes = struct.unpack_from('<%dH' % nb, b, 20)
        off = 20 + tb
        cnt, ib = struct.unpack_from('<2I', b, off); off += 8
        assert cnt <= cap['entries']
        ents = []
        for _ in range(cnt):
            n, = struct.unpack_from('<I', b, off); off += 4
            name = b[off:off + n].decode('ascii'); off += n
            lo, sz = struct.unpack_from('<2I', b, off); off += 8
            ents.append((name, lo, sz))
        out = bytearray()
        for s_ in sizes:
            out += zlib.decompress(b[off:off + s_]); off += s_ + 1
            assert len(out) <= cap['decoded']
        for _, lo, sz in ents:
            assert lo + sz <= len(out)
    except (AssertionError, struct.error, zlib.error, UnicodeDecodeError, ValueError):
        raise Refused(f'{Path(path).name} does not decode as a RES archive within its capacity')
    names = {}
    for n, lo, sz in ents:
        need(n.lower() not in names, f'archive member {n} is listed twice')
        names[n.lower()] = (n, lo, sz)
    return names, bytes(out)


def ini(data):
    out, section = {}, ''
    for line in data.decode('latin-1').splitlines():
        line = line.split(';', 1)[0].strip()
        if not line:
            continue
        if line.startswith('[') and line.endswith(']'):
            section = line[1:-1].lower(); continue
        if '=' in line:
            k, v = line.split('=', 1)
            v = v.strip()
            if len(v) >= 2 and v[0] == v[-1] == '"':
                v = v[1:-1]
            out.setdefault(section, {}).setdefault(k.strip().lower(), v)
    return out


def leaf(name):
    return name.replace('/', '\\').rsplit('\\', 1)[-1].rsplit('.', 1)[0]


def ext(name):
    return name.rsplit('.', 1)[-1] if '.' in name else ''


def stem_leaf(lf, stem):
    return lf == stem or any(lf.startswith(stem + t) for t in ('_', 'c', 'e')) or \
        lf.startswith('flame' + stem) or lf.startswith('concring' + stem)


def pod_slot(part):
    return re.fullmatch(r'pod\d\d', part) is not None


_SHARED = {}


def shared_names(stock):
    """Member names present in at least two stock pod (or Garage pod) archives."""
    if stock in _SHARED:
        return _SHARED[stock]
    res = []
    for garage in (False, True):
        seen, read = {}, 0
        for r in sorted(STOCK_RACERS):
            q = Path(stock) / ('UI/UIGARAGE/%02dE.RES' % r if garage else 'PODS/POD%02d/POD%02d.RES' % (r, r))
            if not q.is_file():
                continue
            try:
                names, _ = read_res(q, UI_CAPACITY if garage else POD_CAPACITY)
            except Refused:
                continue
            read += 1
            for n in names:
                seen[n] = seen.get(n, 0) + 1
        need(read >= 2, 'fewer than two stock ' + ('UI/UIGARAGE pod' if garage else 'pod') + ' archives are readable')
        res.append({n for n, k in seen.items() if k >= 2})
    _SHARED[stock] = tuple(res)
    return _SHARED[stock]


def read_items(p, d):
    strings = d.get('strings', {})
    need(isinstance(strings, dict), 'strings must be an object of languages')
    need(len(strings) <= 16, 'strings: too many languages')
    keys = 0
    for lang, table in strings.items():
        need(re.fullmatch(r'[a-z]{1,16}', lang) is not None, f'strings: language "{lang}" is not a lower-case language name')
        need(isinstance(table, dict), f'strings: "{lang}" must be an object of KEY: text')
        for k, v in table.items():
            need(KEY.match(k) is not None, f'strings: key "{k}" is not [A-Z0-9_]{{1,47}}')
            need(font_text(v, 255), f'strings: {lang}.{k} must be 1-255 characters the menu fonts draw (printable ASCII)')
            for c in v:
                shown = 'double quote' if c == '"' else f"'{c}'"
                need(name_character(c), f"strings: {lang}.{k} uses {shown} (0x{ord(c):02x}), which the race HUD font cannot draw (allowed: {NAME_SET_TEXT})")
            keys += 1
            need(keys <= 4096, 'strings: more than 4096 keys')
    p['strings'] = strings
    english = strings.get('english', {})
    listed = {f['path'].lower(): f['path'] for f in p['files']}

    def path(raw, what):
        n = norm(raw) if isinstance(raw, str) else None
        need(n, f'{what} "{raw}" is not a plain package path')
        need(n.lower() in listed, f'{what} "{raw}" is not listed in files')
        return listed[n.lower()]

    def txt(o, k, where, required=True):
        need(not required or k in o, f'{where}: missing field "{k}"')
        v = o.get(k, '')
        need(isinstance(v, str), f'{where}: field "{k}" must be text')
        return v

    def eng(k, where, what):
        need(k in english, f'{where}: {what} "{k}" has no English string')
        return english[k]

    ids = set()
    for o in p['raw']['items']:
        need(isinstance(o, dict), 'items: every entry must be an object')
        kind, iid = txt(o, 'kind', 'items'), txt(o, 'id', 'items')
        need(ITEM_ID.match(iid) is not None, f'items: id "{iid}" is not [a-z0-9][a-z0-9-]{{1,46}}')
        need(iid not in ids, f'items: id "{iid}" is used twice'); ids.add(iid)
        if kind == 'pod':
            w = f'pod item "{iid}"'
            stem = txt(o, 'stem', w)
            need(STEM.match(stem) is not None and not pod_slot(stem),
                 f'{w}: stem "{stem}" is not [a-z0-9_]{{3,16}} (and not pod plus two digits)')
            sp = stem.upper()
            pod = dict(id=iid, key=f'{p["id"]}/{iid}', stem=stem)
            pod['archive'] = path(txt(o, 'archive', w), f'{w}: archive')
            need(pod['archive'].lower() == f'pods/{stem}/{stem}.res', f'{w}: archive must be PODS/{sp}/{sp}.RES')
            pod['camera'] = path(txt(o, 'camera', w), f'{w}: camera')
            need(pod['camera'].lower() == f'pods/{stem}/{stem}.cam', f'{w}: camera must be PODS/{sp}/{sp}.CAM')
            nk = txt(o, 'name_key', w); sk = txt(o, 'short_name_key', w, False) or nk
            pod['name'] = eng(nk, w, 'name_key'); eng(sk, w, 'short_name_key')
            ui = o.get('ui')
            need(isinstance(ui, dict), f'{w}: missing object "ui"')
            uw = w + ' ui'
            pod['ui_archive'] = path(txt(ui, 'archive', uw), f'{uw}: archive')
            need(pod['ui_archive'].lower() == f'ui/{stem}_ui.res', f'{uw}: archive must be UI/{sp}_UI.RES')
            named = [txt(ui, 'rider_model', uw), txt(ui, 'results_icon', uw)]
            need(all(named), f'{uw}: rider_model and results_icon must name members')
            lists = {}
            for k in ('character', 'character_pages', 'pod_pages', 'shadow', 'portraits', 'background', 'garage_model'):
                v = ui.get(k, None)
                need(v is not None or k == 'garage_model', f'{uw}: missing field "{k}"')
                v = v or []
                need(isinstance(v, list) and len(v) <= 512 and all(isinstance(x, str) and x for x in v),
                     f'{uw}: every entry of "{k}" must be a member name')
                lists[k] = v; named += v
            ch = [x.lower() for x in lists['character']]
            base = next((x.rsplit('.', 1)[0] for x in ch if ext(x) == 'psg'), None)
            need(base, f'{uw}: character lists no .psg')
            want = {base + '.psg', base + '.mcf', base + '.vut'} | {base + m + '.mot' for m in ('_ent', '_idl1', '_idl2', '_idl3', '_breathe', '_sel')}
            need(set(ch) == want and len(ch) == len(want),
                 f'{uw}: character must be {base}.psg/.mcf/.vut and the six motions _ent _idl1 _idl2 _idl3 _breathe _sel')
            pod['ui_named'] = named
            pod['loose'] = []
            v = o.get('voice')
            if v is not None:
                need(isinstance(v, dict), f'{w}: voice must be an object')
                pod['loose'].append(path(txt(v, 'ini', w + ' voice'), f'{w}: voice ini'))
                fi, co = v.get('first'), v.get('count')
                need(isinstance(fi, int) and isinstance(co, int) and not isinstance(fi, bool) and 0 <= fi <= 255 and 1 <= co <= 255,
                     f'{w}: voice needs whole numbers first (0-255) and count (1-255)')
            a = txt(o, 'announcer', w, False)
            if a:
                pod['loose'].append(path(a, f'{w}: announcer'))
            ai, unlock = txt(o, 'ai', w, False) or 'addon_courses', txt(o, 'unlock', w, False) or 'always'
            need(ai != 'anywhere', f'{w}: ai "anywhere" needs a later version of the port (contract 2: addon_courses or never)')
            need(ai in ('addon_courses', 'never'), f'{w}: ai must be addon_courses or never')
            need(unlock == 'always', f'{w}: unlock must be "always" (contract 2)')
            need(all(x['stem'] != stem for x in p['pods']), f'items: stem "{stem}" is used twice')
            p['pods'].append(pod)
        elif kind == 'course':
            w = f'course item "{iid}"'
            code = txt(o, 'code', w)
            need(re.fullmatch(r'[a-z][a-z0-9]', code) is not None, f'{w}: code "{code}" must be 2 characters [a-z][a-z0-9]')
            need(code not in STOCK_CODES, f'{w}: code "{code}" is a stock course code')
            c = dict(id=iid, key=f'{p["id"]}/{iid}', code=code, loose=[])
            c['archive'] = path(txt(o, 'archive', w), f'{w}: archive')
            need(c['archive'].lower() == f'tracks/{code}.res', f'{w}: archive must be TRACKS/{code.upper()}.RES')
            c['name'] = eng(txt(o, 'name_key', w), w, 'name_key')
            pk = txt(o, 'planet_name_key', w, False)
            if pk:
                eng(pk, w, 'planet_name_key')
            need(txt(o, 'phy_class', w) in ('A', 'B', 'C'), f'{w}: phy_class must be A, B or C')
            need(txt(o, 'ai_class', w) in ('A', 'B', 'C'), f'{w}: ai_class must be A, B or C')
            rival = txt(o, 'rival', w)
            if rival != 'none':
                m = re.fullmatch(r'stock:(\d\d)', rival)
                if rival.startswith('stock:'):
                    need(m and int(m.group(1)) in STOCK_RACERS, f'{w}: rival "{rival}" is not an existing stock racer (stock:01..26)')
                else:
                    pkg, _, it = rival.partition('/')
                    need(len(pkg) >= 3 and ITEM_ID.match(it or '') is not None,
                         f'{w}: rival must be stock:NN, <package-id>/<pod-item-id> or none')
            ls = o.get('loading_screen')
            need(isinstance(ls, dict), f'{w}: missing object "loading_screen"')
            need(('file' in ls) != ('stock' in ls), f'{w}: loading_screen needs exactly one of file or stock')
            if 'file' in ls:
                f = path(txt(ls, 'file', w + ' loading_screen'), f'{w}: loading_screen file')
                need(f.lower() == f'ui/textures/ld_{code}.pst', f'{w}: loading_screen file must be UI/TEXTURES/LD_{code.upper()}.PST')
                c['loose'].append(f)
            else:
                sc = txt(ls, 'stock', w + ' loading_screen').lower()
                need(sc in STOCK_CODES and sc != 'sb', f'{w}: loading_screen stock must be a stock course code')
            cs = o.get('course_screen')
            if cs is not None:
                need(isinstance(cs, dict), f'{w}: course_screen must be an object')
                need('archive' not in cs and 'hologram' not in cs,
                     f'{w}: own hologram archives need a later version of the port (contract 2: stock_template)')
                st = txt(cs, 'stock_template', w + ' course_screen').lower()
                need(st in STOCK_CODES and st != 'sb', f'{w}: course_screen stock_template must be a stock course code')
                pv = txt(cs, 'preview', w + ' course_screen', False)
                if pv:
                    pv = path(pv, f'{w}: course_screen preview')
                    need(ext(pv.lower()) == 'pst', f'{w}: course_screen preview must be a .PST')
                    c['loose'].append(pv)
            inh = o.get('inherit', {})
            need(isinstance(inh, dict), f'{w}: inherit must be an object')
            c['inherit'] = {}
            for k in ('tuning', 'particles'):
                v = inh.get(k, '')
                need(isinstance(v, str), f'{w} inherit: field "{k}" must be text')
                need(not v or (v.lower() in STOCK_CODES and v.lower() != 'sb'), f'{w}: inherit values must be stock course codes')
                c['inherit'][k] = v.lower()
            if 'modes' in o:
                m = o['modes']
                need(isinstance(m, list) and m, f'{w}: modes must be a non-empty list')
                for x in m:
                    need(x != 'circuit', f'{w}: mode circuit needs a later version of the port')
                    need(x in ('single', 'time_trial'), f'{w}: modes may only hold single and time_trial')
            rk = txt(o, 'records_key', w, False) or iid
            need(RECORDS_KEY.match(rk) is not None, f'{w}: records_key must be [a-z0-9-]{{1,47}}')
            h = o.get('hall_defaults')
            if h is not None:
                need(isinstance(h, dict), f'{w}: hall_defaults must be an object')
                for k in ('lap', 'race', 'knockouts'):
                    rows = h.get(k, [])
                    need(isinstance(rows, list) and len(rows) <= 3, f'{w}: hall_defaults.{k} must be a list of at most 3 rows')
                    for r in rows:
                        need(isinstance(r, dict), f'{w}: hall_defaults.{k} rows must be objects')
                        need(font_text(r.get('name'), 20), f'{w}: hall_defaults names must be 1-20 printable characters (original 21-byte lane with its end byte)')
                        if k == 'knockouts':
                            n = r.get('count')
                            need(isinstance(n, int) and not isinstance(n, bool) and 0 <= n <= 999, f'{w}: hall_defaults knockouts need a whole count')
                        else:
                            t_ = r.get('time')
                            need(isinstance(t_, (int, float)) and not isinstance(t_, bool) and 0 < t_ <= 35999,
                                 f'{w}: hall_defaults times must be seconds above 0')
            e = o.get('enhancements', [])
            need(isinstance(e, list) and len(e) <= 16 and all(font_text(x, 32) for x in e), f'{w}: enhancements must be a short list')
            need(all(x['code'] != code for x in p['courses']), f'items: code "{code}" is used twice')
            p['courses'].append(c)
        else:
            raise Refused(f'items: kind "{kind}" is not course or pod')
    need(p['kind'] != 'pod' or (p['pods'] and not p['courses']), 'kind "pod" must list only pod items (at least one)')
    need(p['kind'] != 'course' or (p['courses'] and not p['pods']), 'kind "course" must list only course items (at least one)')


def check_items(p, stock):
    """Item content (names only; the game's own loaders decode the content again)."""
    for pod in p['pods']:
        stem, w = pod['stem'], f'pod {pod["stem"]}'
        shared = shared_names(stock) if stock else None
        try:
            names, data = read_res(p['folder'] / pod['archive'], POD_CAPACITY)
        except Refused as x:
            raise Refused(f'{w}: archive {pod["archive"]} {x}')
        own, speech = f'pods\\{stem}\\', f'audio\\speech\\{stem}\\'
        for n, (spelling, _, _) in names.items():
            parts = re.split(r'[\\/]', n)
            need(not (len(parts) >= 2 and parts[0] == 'pods' and parts[1] != stem),
                 f'{w}: archive member {spelling} is named for another pod (stem must be {stem})')
            need(not (len(parts) >= 3 and parts[:2] == ['audio', 'speech'] and parts[2] != stem),
                 f'{w}: archive member {spelling} is named for another pod (stem must be {stem})')
            need(not any(pod_slot(x) for x in parts), f'{w}: archive member {spelling} still names an original pod slot')
            if n.startswith(own) or n.startswith(speech) or stem_leaf(leaf(n), stem):
                continue
            need(shared is None or n in shared[0],
                 f'{w}: archive member {spelling} is neither a shared original name nor named with the stem {stem}')

        def text_member(name):
            need(name in names, f'{w}: archive lacks member {name}')
            _, lo, sz = names[name]
            need(0 < sz <= TEXT_MEMBER, f'{w}: archive member {name} is outside the 8 KiB text capacity')
            return data[lo:lo + sz]
        phy = ini(text_member(own + stem + '.phy'))
        prefix = phy.get('pod setup', {}).get('prefix', '')
        need(prefix.lower() == stem, f'{w}: PHY [Pod Setup] Prefix is "{prefix}"; it must equal the stem')
        for sec, label in (('baseabilities', 'BaseAbilities'), ('maxabilities', 'MaxAbilities')):
            for k in ('maxthrust', 'theoreticaltopspeed', 'spoileryawmoment', 'defense', 'baserepairamount', 'basecoolingtime'):
                need(k in phy.get(sec, {}), f'{w}: PHY lacks [{label}] {k} (stats bars)')
        text_member(own + stem + '.ai')
        sounds = ini(text_member(speech + 'sounds.ini'))
        need('indices' in sounds, f'{w}: sounds.ini lacks [Indices]')
        for k, v in sounds['indices'].items():
            if k.startswith('numberof'):
                need(re.fullmatch(r'\d{1,3}', v) is not None and int(v) <= 255, f'{w}: sounds.ini {k} exceeds its uint8 storage')
        for table in ('curses.ini', 'taunts.ini', 'special.ini'):
            text_member(speech + table)
        cam = (p['folder'] / pod['camera']).stat().st_size
        need(0 < cam <= CAMERA_BYTES, f'{w}: camera {pod["camera"]} is empty or larger than 64 KiB')
        try:
            ui_names, _ = read_res(p['folder'] / pod['ui_archive'], UI_CAPACITY)
        except Refused as x:
            raise Refused(f'{w}: menu archive {pod["ui_archive"]} {x}')
        seen = set()
        for m in pod['ui_named']:
            need(m.lower() in ui_names, f'{w}: menu archive lacks member {m} named in dlc.json')
            need(m.lower() not in seen, f'{w}: menu member {m} is named twice in dlc.json'); seen.add(m.lower())
        for n, (spelling, _, _) in ui_names.items():
            need(n in seen, f'{w}: menu archive member {spelling} is not named by any ui field')
            need(stem_leaf(leaf(n), stem) or shared is None or n in shared[1],
                 f'{w}: menu archive member {spelling} is neither a shared original name nor named with the stem {stem}')
        pod['materials'] = {n.rsplit('.', 1)[0] for n in names if ext(n) == 'psm' and stem_leaf(leaf(n), stem)}
        pod['ui_materials'] = {n.rsplit('.', 1)[0] for n in ui_names if ext(n) == 'psm' and stem_leaf(leaf(n), stem)}
        for f in pod['loose']:
            n = (p['folder'] / f).stat().st_size
            need(0 < n <= TEXT_MEMBER, f'{w}: {f} is outside the 8 KiB INI capacity')
        if shared is None:
            print(f'  ({w}: shared original names not checked; pass --stock <install>)')
    for c in p['courses']:
        code, w = c['code'], f'course {c["code"]}'
        try:
            names, _ = read_res(p['folder'] / c['archive'], COURSE_CAPACITY)
        except Refused as x:
            raise Refused(f'{w}: archive {c["archive"]} {x}')
        for want in (f'tracks\\{code}.scb', f'tracks\\{code}.tob', f'tracks\\{code}.aub', f'tracks\\m{code}.spl', f'tracks\\a{code}.spl'):
            need(want in names, f'{w}: archive lacks member {want}')
        for n, (spelling, _, _) in names.items():
            parts = re.split(r'[\\/]', n)
            lf = leaf(n)
            need(not (len(parts) == 2 and parts[0] == 'tracks' and len(lf) == 2 and lf != code and lf in STOCK_CODES),
                 f'{w}: archive member {spelling} is named by stock course code {lf}')
        own_tun = f'tracks\\{code}.tun' in names
        own_part = all(f'particle\\demoprt.{e}' in names for e in ('prt', 'pst', 'psm'))
        need(own_tun or c['inherit']['tuning'], f'{w}: archive has no tracks\\{code}.tun and no inherit.tuning')
        need(own_part or c['inherit']['particles'], f'{w}: archive has no particle\\demoprt.prt/.pst/.psm and no inherit.particles')
        for f in c['loose']:
            n = (p['folder'] / f).stat().st_size
            need(0 < n <= LOOSE_PST, f'{w}: {f} is empty or larger than 4 MiB')


def items_collide(a, b):
    for x in a['pods']:
        for y in b['pods']:
            if x['stem'] == y['stem']:
                return f'pod stem {x["stem"]}'
            for k, label in (('materials', 'material name'), ('ui_materials', 'menu material name')):
                common = sorted(x.get(k, set()) & y.get(k, set()))
                if common:
                    return f'{label} {common[0]}'
    for x in a['courses']:
        for y in b['courses']:
            if x['code'] == y['code']:
                return f'course code {x["code"]}'
    return None


def is_program(path):
    with open(path, 'rb') as f:
        head = f.read(64)
        if head[:4] == b'\x7fELF':
            return True
        if len(head) == 64 and head[:2] == b'MZ':
            f.seek(int.from_bytes(head[60:64], 'little'))
            return f.read(4) == b'PE\0\0'
    return False


def check_files(p):
    listed = {f['path'].lower() for f in p['files']}
    for q in p['folder'].rglob('*'):
        need(not q.is_symlink() and not q.is_junction(), f'package contains a link or junction: {q.name}')
        if q.is_dir():
            continue
        rel = q.relative_to(p['folder']).as_posix()
        if rel.lower() == 'dlc.json':
            continue
        need(rel.lower() in listed, f'file {rel} is in the package folder but not listed in dlc.json')
    for f in p['files']:
        q = p['folder'] / f['path']
        need(q.is_file(), f'file {f["path"]} listed in dlc.json is missing')
        size = q.stat().st_size
        need(size == f['bytes'], f'file {f["path"]} is {size} bytes; dlc.json says {f["bytes"]}')
        need(not is_program(q), f'file {f["path"]} is a program; packages carry data only')
        h = sha256(q)
        need(h == f['sha256'], f'file {f["path"]} sha256 differs from dlc.json (expected {f["sha256"][:12]}..., found {h[:12]}...)')


def check_stock(p, stock):
    for r in p['replacements']:
        q = Path(stock) / r['target']
        need(q.is_file(), f'replacement target {r["target"]} is not a stock file of this install')
        n, h = q.stat().st_size, sha256(q)
        need(n == r['expect_bytes'] and h == r['expect_sha256'],
             f'built for a different stock file: {r["target"]} is {n} bytes sha256 {h[:12]}..., '
             f'package expects {r["expect_bytes"]} bytes sha256 {r["expect_sha256"][:12]}...')


def resolve(entries):
    """Dependencies, cycles, conflicts and replacement collisions, as the game does."""
    by_id = {e['p']['id']: e for e in entries if e.get('p')}
    order = sorted(entries, key=lambda e: (-(e['p']['priority'] if e.get('p') else 0), e['folder'].name))
    ok = lambda e: e['status'] == 'admitted'

    def refuse(e, why):
        e['status'], e['reason'] = 'refused', why

    for _ in range(len(entries) + 2):
        changed = False
        for e in order:
            if not ok(e):
                continue
            for d in e['p']['dependencies']:
                dep = by_id.get(d['id'])
                good = dep and ok(dep) and satisfies(dep['p']['version'], d['range'])
                if good or d['optional']:
                    continue
                why = ('is not installed' if not dep else 'is not loaded' if not ok(dep) else
                       f'is version {dep["p"]["version_text"]}, needs {d["range"]}')
                refuse(e, f'needs package {d["id"]}, which {why}'); changed = True; break
        for start in order:  # required-dependency cycles
            if not ok(start):
                continue
            stack = [(start, iter(start['p']['dependencies']))]
            while stack:
                top, it = stack[-1]
                d = next(it, None)
                if d is None:
                    stack.pop(); continue
                dep = by_id.get(d['id'])
                if d['optional'] or not dep or not ok(dep):
                    continue
                chain = [s[0] for s in stack]
                if dep in chain:
                    for x in chain[chain.index(dep):]:
                        refuse(x, f'dependency cycle through package {dep["p"]["id"]}')
                    changed = True; break
                if len(stack) <= len(entries):
                    stack.append((dep, iter(dep['p']['dependencies'])))
        for e in order:
            if not ok(e):
                continue
            for c in e['p']['conflicts']:
                o = by_id.get(c['id'])
                if o and ok(o):
                    refuse(e, f'conflicts with package {c["id"]}' + (f' ({c["reason"]})' if c['reason'] else '')); changed = True; break
        claims = {}
        for e in order:
            if ok(e):
                for r in e['p']['replacements']:
                    claims.setdefault(r['target'].lower(), []).append(e)
        for target, who in claims.items():
            if len(who) < 2:
                continue
            tie = who[1]['p']['priority'] == who[0]['p']['priority']
            for x in who[0 if tie else 1:]:
                if ok(x):
                    others = ','.join(o['p']['id'] for o in who if o is not x)
                    refuse(x, f'replaces {target} ' + ('at the same priority as ' if tie else 'with lower priority than ') + others)
                    changed = True
        for i, e in enumerate(order):  # pod stem / course code / material name claimed twice
            if not ok(e) or not (e['p']['pods'] or e['p']['courses']):
                continue
            for o in order[:i]:
                what = ok(o) and items_collide(o['p'], e['p'])
                if what:
                    refuse(e, f'{what} is already used by package {o["p"]["id"]}'); changed = True; break
        if not changed:
            break


def check(target, stock):
    target = Path(target)
    folders = [target] if (target / 'dlc.json').is_file() else sorted(p for p in target.iterdir() if p.is_dir())
    entries = []
    for f in folders:
        e = dict(folder=f, status='admitted', reason='', p=None)
        try:
            e['p'] = read_manifest(f)
            if not e['p']['enabled']:
                e['status'], e['reason'] = 'disabled', 'enabled is false in dlc.json'
            else:
                check_files(e['p'])
                if stock:
                    check_stock(e['p'], stock)
                if e['p']['pods'] or e['p']['courses']:
                    check_items(e['p'], stock)
        except Refused as x:
            e['status'], e['reason'] = 'refused', str(x)
        entries.append(e)
    resolve(entries)
    numbers = {'pods': 32, 'courses': 32}
    order = sorted(entries, key=lambda e: (-(e['p']['priority'] if e.get('p') else 0), e['folder'].name))
    for e in order:
        if e['status'] == 'admitted':
            for kind in ('pods', 'courses'):
                for it in e['p'][kind]:
                    it['number'] = numbers[kind]; numbers[kind] += 1
    bad = 0
    for e in entries:
        p = e['p'] or {}
        line = f'{e["folder"].name}: {e["status"]}'
        if e['reason']:
            line += f' - {e["reason"]}'
        print(line)
        if p.get('personal') and e['status'] != 'refused':
            print(f'  personal: {p["personal_note"]}')
        for r in p.get('replacements', []) if e['status'] == 'admitted' else []:
            print(f'  replaces {r["target"]} with {r["with_"]}')
        for it in p.get('pods', []) if e['status'] == 'admitted' else []:
            print(f'  pod {it["key"]} stem={it["stem"]} number={it["number"]} name="{it["name"]}"'
                  f' materials={len(it.get("materials", ()))} ui_materials={len(it.get("ui_materials", ()))}')
        for it in p.get('courses', []) if e['status'] == 'admitted' else []:
            print(f'  course {it["key"]} code={it["code"]} number={it["number"]} name="{it["name"]}"')
        bad += e['status'] == 'refused'
    if not stock:
        print('(stock files not checked: pass --stock <install> to check "expects")')
    return 1 if bad else 0


def write_manifest(folder, d):
    (Path(folder) / 'dlc.json').write_bytes((json.dumps(d, indent=1, ensure_ascii=False) + '\n').encode('utf-8'))


def regenerate_files(folder):
    folder = Path(folder)
    d = json.loads((folder / 'dlc.json').read_bytes().decode('utf-8'))
    rows = []
    for q in sorted(folder.rglob('*')):
        if q.is_file() and q.relative_to(folder).as_posix().lower() != 'dlc.json':
            old = next((r for r in d.get('files', []) if norm(r.get('path', '')) == q.relative_to(folder).as_posix()), {})
            row = dict(path=q.relative_to(folder).as_posix(), bytes=q.stat().st_size, sha256=sha256(q))
            if 'role' in old:
                row['role'] = old['role']
            rows.append(row)
    d['files'] = rows
    write_manifest(folder, d)
    print(f'{folder.name}: {len(rows)} files listed')


def make_replacement(a):
    out, stock = Path(a.out), Path(a.stock)
    if out.exists():
        raise SystemExit(f'{out} already exists')
    if not ID.match(a.id) or out.name != a.id:
        raise SystemExit('--id must be a valid package id and equal the output folder name')
    out.mkdir(parents=True)
    reps, files = [], []
    for spec in a.target:
        tgt, _, src = spec.partition('=')
        t = norm(tgt)
        if not t or not Path(src).is_file() or not (stock / t).is_file():
            raise SystemExit(f'bad --target {spec}')
        dst = out / t
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
        reps.append(dict(target=t, expects=dict(bytes=(stock / t).stat().st_size, sha256=sha256(stock / t)), **{'with': t}))
    d = dict(schema=SCHEMA, id=a.id, name=a.name, author=a.author, version=a.version)
    if a.description:
        d['description'] = a.description
    d.update(enabled=not a.disabled, kind='replacement', priority=a.priority, requires_port=CONTRACT)
    if a.personal:
        d['personal'] = True
        d['personal_note'] = a.personal_note or PERSONAL_NOTE
    d.update(replace=True, replacements=reps, items=[], files=files)
    if a.depends:
        deps = []
        for spec in a.depends:
            did, _, rng = spec.partition(':')
            deps.append(dict(id=did, version=rng))
        d['dependencies'] = deps
    write_manifest(out, d)
    regenerate_files(out)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    c = sub.add_parser('check'); c.add_argument('path'); c.add_argument('--stock')
    f = sub.add_parser('files'); f.add_argument('path')
    r = sub.add_parser('replace'); r.add_argument('out'); r.add_argument('--stock', required=True)
    r.add_argument('--id', required=True); r.add_argument('--name', required=True); r.add_argument('--author', required=True)
    r.add_argument('--version', required=True); r.add_argument('--description', default='')
    r.add_argument('--priority', type=int, default=0); r.add_argument('--personal', action='store_true')
    r.add_argument('--personal-note', default=''); r.add_argument('--depends', action='append', default=[])
    r.add_argument('--disabled', action='store_true')
    r.add_argument('--target', action='append', required=True, help='STOCK/PATH=SOURCE_FILE (repeatable)')
    a = ap.parse_args(argv)
    if a.cmd == 'check':
        return check(a.path, a.stock)
    if a.cmd == 'files':
        regenerate_files(a.path); return 0
    make_replacement(a); return 0


if __name__ == '__main__':
    sys.exit(main())
