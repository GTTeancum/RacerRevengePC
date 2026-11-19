"""Build dlc.json add-on packages (contract dlc-contract-2) from the converters'
addon-manifest.json folders into a test DLC root. PC enhancement, not original.

  python -I make_addon_root.py [--src <addon-out>] [--out <DLC root>] [--courses] [--only ID ...]

Copies the listed files (never the originals in place, never addon-manifest.json)
and writes dlc.json per work\\scratch\\dlc\\DLC-JSON.md ("Add-on items"). The output
root must not exist yet (nothing is overwritten). Then validate it with
tools\\dlc\\dlc_pack.py check <root> --stock <install>.
"""
import argparse, hashlib, json, shutil, sys
from pathlib import Path

SRC = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\addon-out')
OUT = Path(r'D:\Programming\GitHub\RacerRevengePC\work\scratch\dlc\roots\addon-pods-146\DLC')
NOTE = 'built from your own copy of the source games; do not share'


def sha256(p):
    with open(p, 'rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()


def value(v):
    return v.get('value') if isinstance(v, dict) else v


def key_for(strings, text, prefix):
    eng = strings.get('english', {})
    for k, t in eng.items():
        if t == text:
            return k
    for k in eng:
        if k.startswith(prefix):
            return k
    raise SystemExit(f'no strings key for {text!r}')


def pod_item(it, strings):
    ui = {k: it['ui'][k] for k in ('archive', 'rider_model', 'character', 'character_pages', 'pod_pages', 'shadow',
                                   'portraits', 'background', 'results_icon')}
    if it['ui'].get('garage_model'):
        ui['garage_model'] = it['ui']['garage_model']
    row = dict(kind='pod', id=it['id'], stem=it['stem'], archive=it['archive'],
               camera=it.get('camera') or it['camera_presets'],
               name_key=it.get('name_key') or key_for(strings, it['display_name']['english'], 'RIDER_'), ui=ui,
               ai='addon_courses', unlock='always')
    if it.get('voice'):
        row['voice'] = it['voice']
    if it.get('announcer'):
        row['announcer'] = it['announcer']
    return row


def course_item(it, strings):
    disp = it.get('display_name', {})
    row = dict(kind='course', id=it['id'], code=it['code'], archive=it['archive'],
               name_key=it.get('name_key') or key_for(strings, disp.get('track', ''), 'COURSE_'))
    pk = it.get('planet_name_key') or (key_for(strings, disp['planet'], 'PLANET_') if disp.get('planet') else None)
    if pk:
        row['planet_name_key'] = pk
    row.update(phy_class=value(it['phy_class']), ai_class=value(it['ai_class']),
               rival=value(it['rival']) or 'none', loading_screen=it['loading_screen'])
    cs = it.get('course_screen')
    if cs:
        row['course_screen'] = {k: cs[k] for k in ('stock_template', 'preview') if k in cs}
    if it.get('inherit'):
        row['inherit'] = {k: v for k, v in it['inherit'].items() if k in ('tuning', 'particles') and v}
    for k in ('modes', 'records_key', 'enhancements', 'hall_defaults'):
        if it.get(k):
            row[k] = it[k]
    return row


def build(folder, out, courses):
    man = json.loads((folder / 'addon-manifest.json').read_text(encoding='utf-8'))
    prop = man['proposed_package']
    kinds = {it['kind'] for it in man['items']}
    if 'course' in kinds and not courses:
        return None
    strings = man.get('strings', {})
    items = [pod_item(it, strings) if it['kind'] == 'pod' else course_item(it, strings) for it in man['items']]
    dst = out / prop['id']
    dst.mkdir(parents=True)
    files = []
    for f in man['files']:
        src = folder / f['path']
        assert src.is_file(), src
        q = dst / f['path']
        q.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(src, q)
        row = dict(path=f['path'], bytes=q.stat().st_size, sha256=sha256(q))
        if f.get('role'):
            row['role'] = f['role']
        files.append(row)
    d = dict(schema='rrpc-dlc/1', id=prop['id'], name=prop['name'], author='RacerRevengePC converters',
             version=prop.get('version', '1.0.0'), enabled=True,
             kind=prop['kind'] if prop['kind'] in ('pod', 'course') else 'bundle',
             requires_port='dlc-contract-2', personal=bool(prop.get('personal', True)),
             personal_note=prop.get('personal_note') or NOTE, strings=strings, items=items, files=files,
             facts=dict(built_by='work\\scratch\\146p\\make_addon_root.py', from_manifest=str(folder / 'addon-manifest.json'),
                        manifest_written=man.get('written'), source=man.get('source', {}).get('game'),
                        stand_ins=[s.get('what') + ': ' + s.get('kind') for s in man.get('stand_ins', [])]))
    (dst / 'dlc.json').write_bytes((json.dumps(d, indent=1, ensure_ascii=False) + '\n').encode('utf-8'))
    return dst


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--src', default=str(SRC))
    ap.add_argument('--out', default=str(OUT))
    ap.add_argument('--courses', action='store_true', help='also build the course packages')
    ap.add_argument('--only', nargs='*')
    a = ap.parse_args()
    src, out = Path(a.src), Path(a.out)
    if out.exists():
        raise SystemExit(f'{out} already exists (nothing is overwritten)')
    out.mkdir(parents=True)
    for folder in sorted(p for p in src.iterdir() if (p / 'addon-manifest.json').is_file()):
        if a.only and folder.name not in a.only:
            continue
        d = build(folder, out, a.courses)
        if d:
            print('built', d)
    return 0


if __name__ == '__main__':
    sys.exit(main())
