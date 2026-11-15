"""Racer Revenge PC DLC package tool (phase 1). PC enhancement, not original behaviour.

Same rules as the game's startup checker (src/assets/dlc_package50dl.cpp and
dlc_files50dl.cpp). Field reference: work/scratch/dlc/DLC-JSON.md.

  dlc_pack.py check   <package-or-DLC-root> [--stock <install>]
      Validate one package folder, or every package under a DLC root (then
      dependencies, conflicts and replacement collisions are resolved as the game
      does). With --stock, replacements are checked against the installed stock
      files. Exit code 0 when every enabled package would be admitted.
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
import sys
from pathlib import Path

SCHEMA = 'rrpc-dlc/1'
CONTRACT = 'dlc-contract-1'
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
    need(not rp or rp == CONTRACT, f'package needs port data contract "{rp}"; this build has "{CONTRACT}"')
    need(p['kind'] in KINDS, f'kind "{p["kind"]}" is not course, pod, menu_assets, replacement or bundle')
    items = d.get('items')
    need(isinstance(items, list), 'dlc.json: missing field "items"' if items is None else 'dlc.json: field "items" must be a list')
    need(not items and p['kind'] == 'replacement',
         f'add-on items (kind "{p["kind"]}") need a later version of the port; this build loads replacement packages only')
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
    return p


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
        except Refused as x:
            e['status'], e['reason'] = 'refused', str(x)
        entries.append(e)
    resolve(entries)
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
