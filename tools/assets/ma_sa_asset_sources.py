"""Exact original MA/SA identities shared by authorized cold preparation.

Source-only until the root runs preparation in its central slot. This module
never guesses a case-folded disk path, substitutes another archive, or decodes
original data at import time.
"""
import hashlib
import json
from pathlib import Path
import stat


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def norm(name):
    return name.replace('\\', '/').upper()


def safe(name):
    need(name and '\\' not in name and '\0' not in name and
         all(p not in ('', '.', '..') and ':' not in p for p in name.split('/')),
         'Unsafe relative original path: '+repr(name))
    return Path(name)


def regular(path):
    need(stat.S_ISREG(path.lstat().st_mode) and not path.is_symlink() and
         not any(p.is_symlink() for p in path.parents),
         'Expected nonsymlink regular file: '+str(path))


def config(track):
    need(track in ('MA', 'SA'), 'Explicit original MA or SA track required')
    return json.loads(Path(__file__).with_name('ma-sa-source-inputs.json').read_bytes())[track]


def authenticated_inventory(data, cfg):
    """Only the retained exact legacy bytes or reviewed exact V2 bytes."""
    digest = sha(data)
    need(digest == cfg['inventory_sha256'] or
         digest == cfg.get('inventory_v2_sha256'), 'Reviewed intake inventory identity')
    inventory = json.loads(data)
    if digest == cfg.get('inventory_v2_sha256'):
        need(inventory.get('schema') == cfg.get('inventory_v2_schema', 'RR_ORIGINAL_SA_MEMBER_INVENTORY_V2') and
             inventory.get('source_generation_sha256') == cfg['inventory_v2_generation_sha256'],
             'Reviewed V2 provenance identity')
    need(inventory['archive_sha256'] == cfg['archive_sha256'] and
         len(inventory['members']) == cfg['archive_entries'], 'Original inventory extent differs')
    return inventory


class Sources:
    def __init__(self, intake, track):
        self.track, self.cfg = track, config(track)
        self.intake = Path(intake)
        archive_path = self.intake/'original'/(track+'.RES')
        regular(archive_path)
        self.archive = archive_path.read_bytes()
        need(len(self.archive) == self.cfg['archive_bytes'] and
             sha(self.archive) == self.cfg['archive_sha256'], 'Exact original archive identity')
        inventory_path = self.intake/'intake/archive-inventory.json'
        regular(inventory_path)
        inventory_bytes = inventory_path.read_bytes()
        self.inventory = authenticated_inventory(inventory_bytes, self.cfg)
        self.rows = self.inventory['members']
        need(len(self.rows) == self.cfg['archive_entries'], 'Original member count')
        self.members = {}
        for row in self.rows:
            key = norm(row['name'])
            need(key not in self.members and norm(row['path']) == key, 'Original path/name duplicate or mismatch')
            safe(row['path'])
            self.members[key] = row

    def read(self, member):
        row = self.members[norm(member)]
        path = self.intake/'intake/original-members'/safe(row['path'])
        regular(path)
        data = path.read_bytes()
        need(len(data) == row['bytes'] and sha(data) == row['sha256'],
             'Original source member identity: '+row['name'])
        return data


def animation_bindings(imported, name):
    """Read all authentic PSM users; retain differing per-material controls."""
    result = []
    for member, material in sorted(imported.items()):
        if material['status'] != 'converted_material':
            continue
        for material_pass in material['passes']:
            if not any(norm(t['name']) == norm(name) for t in material_pass['textures']):
                continue
            fields = {f['name']: f['value'] for f in material_pass['fields']['authored']}
            initial = int(fields.get('animatedMapInitialFrame', '0'))
            fps = float(fields['animatedMapFramesPerSecond']) if 'animatedMapFramesPerSecond' in fields else None
            result.append(dict(material_member=member, material_pass=material_pass['pass'],
                initial_frame=initial, fps=fps, authored=material_pass['fields']))
    need(result, 'Original ATM has no converted material user: '+name)
    return result
