r"""Install converted menu (UI) archives into a silo asset copy and list them in its converted-content.json.

    python -I ui_silo.py install [--assets assets] [--src export\a2r\ui] [--only UI/UIRIDER/UIRIDER.RES ...]
    python -I ui_silo.py restore [--assets assets] [--only ...]      (put the originals back, drop the rows)
    python -I ui_silo.py check   [--assets assets]                   (read-only: rows vs files, nothing written)

Input: <src>\converted-content.fragment.json (written by ui_convert.py; one files[] row per changed archive)
and the archives next to it at their game-relative paths.

install, same guarantees as pod_silo.py / trk_silo.py:
1. one lock for every silo installer (silo\.install.lock);
2. every shipped archive is re-read (RES3 rules), its size/SHA and member list must equal its row, its member
   names/order must equal the original's, and the original in C:\Games must equal the row's replaces{};
3. each original being replaced is copied into silo\originals\ (same relative path) once, never overwritten;
   the silo copy must still be the installed original when no backup exists;
4. the new files are written into silo\<assets>\ and their rows MERGED into silo\<assets>\converted-content.json
   (rows for other paths - the track, the pods - are kept);
5. everything is re-read and checked against its row. C:\Games is only read.
NOTE: the port in the silo tree does not consult converted-content.json for UI or LOC archives (notes\ui-convert.md,
"Port pins"); installing these files without that port change makes the menus refuse to start.
"""
import argparse
import json
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import pod_silo  # noqa: E402  (lock, backup, manifest helpers, paths)
import trk_res3  # noqa: E402

SILO, GAME, sha = pod_silo.SILO, pod_silo.GAME, pod_silo.sha
SRC = os.path.normpath(os.path.join(HERE, '..', '..', 'export', 'a2r', 'ui'))


def rows_of(src, only):
    frag = json.load(open(os.path.join(src, 'converted-content.fragment.json')))
    assert frag.get('schema') == 'a2r-converted-content/1'
    rows = frag['files']
    if only:
        want = {o.replace('\\', '/').lower() for o in only}
        rows = [r for r in rows if r['path'].lower() in want]
        missing = want - {r['path'].lower() for r in rows}
        if missing:
            raise SystemExit('not in the fragment: %s' % sorted(missing))
    return rows


def check_shipped(row, data):
    rel = row['path']
    if len(data) != row['bytes'] or sha(data) != row['sha256']:
        raise SystemExit('%s: shipped file does not match its row' % rel)
    r = trk_res3.read_res3(data)
    got = [(n, lo, sz, sha(r.payload[lo:lo + sz])) for n, lo, sz in r.members]
    want = [(m['name'], m['offset'], m['bytes'], m['sha256']) for m in row['members']]
    if got != want:
        raise SystemExit('%s: member table differs from its row' % rel)
    orig = open(os.path.join(GAME, *rel.split('/')), 'rb').read()
    if len(orig) != row['replaces']['bytes'] or sha(orig) != row['replaces']['sha256']:
        raise SystemExit('%s: the installed original is not the file this row replaces' % rel)
    if [n for n, lo, sz in trk_res3.read_res3(orig).members] != [m[0] for m in got]:
        raise SystemExit('%s: member names/order differ from the original' % rel)


def install(assets, src, only):
    lk = pod_silo.lock()
    try:
        cc, doc = pod_silo.load_manifest(assets)
        rows = rows_of(src, only)
        new_rows = []
        for row in rows:
            data = open(os.path.join(src, *row['path'].split('/')), 'rb').read()
            check_shipped(row, data)
            rep = pod_silo.backup(row['path'], assets)
            assert rep == row['replaces']
            open(os.path.join(SILO, assets, *row['path'].split('/')), 'wb').write(data)
            new_rows.append(dict(row, source='converted UI (%s)' % src))
            print('%-28s %9d bytes, %d members changed' % (row['path'], row['bytes'], row['facts']['changed_members']))
        pod_silo.merge(doc, new_rows)
        pod_silo.write_and_check(cc, doc, assets)
        print('converted-content.json: %d rows' % len(doc['files']))
    finally:
        os.remove(lk)


def restore(assets, src, only):
    lk = pod_silo.lock()
    try:
        cc, doc = pod_silo.load_manifest(assets)
        paths = {r['path'].lower() for r in rows_of(src, only)}
        drop = set()
        for f in doc['files']:
            if f['path'].lower() in paths:
                orig = os.path.join(SILO, 'originals', *f['path'].split('/'))
                game = open(os.path.join(GAME, *f['path'].split('/')), 'rb').read()
                if open(orig, 'rb').read() != game:
                    raise SystemExit('backup %s differs from the installed original' % orig)
                shutil.copyfile(orig, os.path.join(SILO, assets, *f['path'].split('/')))
                drop.add(f['path'].lower())
        doc['files'] = [f for f in doc['files'] if f['path'].lower() not in drop]
        if doc['files']:
            pod_silo.write_and_check(cc, doc, assets)
        elif os.path.exists(cc):
            os.remove(cc)
        print('restored', sorted(drop))
    finally:
        os.remove(lk)


def check(assets, src, only):
    for row in rows_of(src, only):
        data = open(os.path.join(src, *row['path'].split('/')), 'rb').read()
        check_shipped(row, data)
        p = os.path.join(SILO, assets, *row['path'].split('/'))
        cur = open(p, 'rb').read() if os.path.exists(p) else b''
        state = 'installed' if sha(cur) == row['sha256'] else ('original' if sha(cur) == row['replaces']['sha256'] else 'OTHER')
        print('%-28s shipped ok, silo copy: %s' % (row['path'], state))


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['install', 'restore', 'check'])
    ap.add_argument('--assets', default='assets')
    ap.add_argument('--src', default=SRC)
    ap.add_argument('--only', nargs='*')
    a = ap.parse_args()
    if os.path.normcase(os.path.abspath(os.path.join(SILO, a.assets))).startswith(os.path.normcase(os.path.abspath(GAME))):
        raise SystemExit('refusing to write under C:\\Games')
    dict(install=install, restore=restore, check=check)[a.cmd](a.assets, a.src, a.only)
