#!/usr/bin/env python3
"""Independent byte, scope, closure, and negative tests for 49G asset selection."""
import argparse
import collections
import copy
import hashlib
import importlib.util
import json
import math
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zipfile


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--selection', type=Path, default=Path(__file__).resolve().parents[2] / 'evidence/visual49G/asset-selection.json')
    parser.add_argument('--reproduce', action='store_true', help='Repeat selection in a fresh temporary directory and compare all outputs')
    args = parser.parse_args()
    base = args.selection.parent
    data = json.loads(args.selection.read_bytes())
    provenance = json.loads((base / 'provenance.json').read_bytes())
    manifest = json.loads(args.manifest.read_bytes())
    checks, meshes, textures, records, negative = 0, 0, 0, 0, []

    def check(test, label):
        nonlocal checks
        checks += 1
        if not test:
            raise AssertionError(label)

    check(data['schema'] == 'RR_VISUAL_SELECTION49G_V1' and data['status'] == 'complete', 'schema/complete')
    check(digest(args.selection.read_bytes()) == provenance['selection']['sha256'], 'report hash')
    check(len(data['scene_requests']) == 231, 'scene count')
    check(len(data['visual_instances']) == 284 and len(data['non_drawn_instances']) == 34, 'placement partition')
    indices = [r['world_instance_index'] for r in data['visual_instances'] + data['non_drawn_instances']]
    check(sorted(indices) == list(range(318)), 'each authored placement appears exactly once')
    object_by_path = {r['path']: r for r in data['objects']}
    check(len(object_by_path) == 859, 'deduplicated object count')
    check({str(p.relative_to(base)) for p in (base / 'cacheobjects').iterdir()} == set(object_by_path), 'no unrelated cache objects')
    check(all(r['extracted'] for r in data['objects']), 'all reported objects materialized')
    with zipfile.ZipFile(args.archive) as archive:
        for record in data['objects']:
            path = base / record['path']
            payload = path.read_bytes()
            check(payload == archive.read(record['archive_member']), 'raw archived-byte equality')
            check(len(payload) == record['bytes'] and digest(payload) == record['sha256'], 'size/hash')
            check(record['archive_member'].startswith('evidence/importFull48F-20261003/native-cache/objects/'), 'exact archive prefix')
            check(path.suffix in ('.rrscene', '.rrmesh', '.rrworld', '.json', '.rrtex'), 'only selected native formats')
            if path.suffix == '.json':
                fields = json.loads(payload)
                expected = fields.pop('record_sha256')
                check(digest(json.dumps(fields, sort_keys=True, separators=(',', ':'), ensure_ascii=True).encode()) == expected, 'canonical record digest')
                records += 1
            elif path.suffix == '.rrmesh':
                check(payload[:8] == b'RRMESH01', 'mesh magic')
                vertices, indices = struct.unpack_from('<II', payload, 8)
                check(len(payload) == 16 + 24 * vertices + 4 * indices and indices % 3 == 0, 'mesh dimensions')
                for at in range(16, 16 + 24 * vertices, 24):
                    check(all(math.isfinite(x) for x in struct.unpack_from('<5f', payload, at)), 'mesh finite values')
                check(all(x[0] < vertices for x in struct.iter_unpack('<I', payload[16 + 24 * vertices:])), 'mesh indices')
                native = json.loads((base / record['path'].replace('.rrmesh', '.json')).read_bytes())
                check((vertices, indices // 3) == (native['vertices'], native['triangles']), 'mesh metadata counters')
                meshes += 1
            elif path.suffix == '.rrtex':
                check(payload[:8] == b'RRTEX001', 'texture magic')
                width, height = struct.unpack_from('<II', payload, 8)
                check(width > 0 and height > 0 and len(payload) == 16 + width * height * 4, 'texture dimensions')
                textures += 1
    normalize = lambda s: s.replace('\\', '/').upper()
    for scene in data['scene_requests']:
        candidates = [m for m in manifest['members'] if normalize(m['container']) == normalize(scene['container']) and normalize(m['member']) == normalize(scene['member'])]
        check(len(candidates) == 1 and candidates[0]['status'] == 'converted_geometry', 'exact scene binding')
        check(candidates[0]['native'] == scene['native'], 'scene manifest metadata')
    for placement in data['non_drawn_instances']:
        check(not any(normalize(m['container']) == 'TRACKS/TA.RES' and normalize(m['member']) == normalize(placement['geometry_member']) for m in manifest['members']), 'non-drawn is truly missing PSG')
    for kind, extension, required_status in [('material_resolution', '.PSM', 'converted_material'), ('texture_resolution', '.PST', 'converted_texture')]:
        for row in data[kind]:
            wanted = 'TEXTURES/' + normalize(row['name']) + extension
            candidates = [m for m in manifest['members'] if normalize(m['member']) == wanted]
            local = [m for m in candidates if normalize(m['container']) == normalize(row['container'])]
            relevant = local or candidates
            can_resolve = bool(relevant) and len({m['source_sha256'] for m in relevant}) == 1 and all(m['status'] == required_status for m in relevant)
            check(can_resolve == (row['status'] == 'resolved'), 'independent local-shadow/global-status rule')
            if can_resolve:
                check(row['key'] == relevant[0]['native']['key'], 'resolution selected exact original key')
    unresolved = {(r['container'], r['name']) for r in data['texture_resolution'] if r['status'] == 'unresolved'}
    check(unresolved == {('PODS/POD01/POD01.RES', 'specmap5')} | {('TRACKS/TA.RES', n) for n in ('arrowred', 'cord', 'crawlera', 'crowd', 'track', 'trackdark', 'trackside', 'trackwheel')}, 'all nine unresolved remain explicit')
    for role, total in data['totals'].items():
        rows = [r for r in data['objects'] if r['role'] == role]
        check(total['bytes'] == sum(r['bytes'] for r in rows) and total['files'] == len(rows), 'role totals')
    selector_path = Path(__file__).with_name('select_visual_assets49g.py')
    spec = importlib.util.spec_from_file_location('selector49g', selector_path)
    selector = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(selector)
    check(digest(selector_path.read_bytes()) == provenance['selector']['sha256'], 'final selector source provenance')
    check(selector.resolve_selftest() == 7, 'local shadow/global fallback negative cases')
    def rejects(label, action):
        try:
            action()
        except (ValueError, KeyError, struct.error):
            negative.append(label)
            return
        raise AssertionError('Accepted invalid case: ' + label)
    world = (base / data['world']['rrworld']).read_bytes()
    rejects('wrong RRWORLD magic', lambda: selector.world_visual_metadata(b'INVALID!' + world[8:]))
    rejects('truncated RRWORLD', lambda: selector.world_visual_metadata(world[:-1]))
    rejects('RRWORLD trailing bytes', lambda: selector.world_visual_metadata(world + b'\0'))
    member = next(m for m in manifest['members'] if m.get('native', {}).get('key') == data['scene_requests'][0]['key'])
    with zipfile.ZipFile(args.archive) as archive, tempfile.TemporaryDirectory(prefix='selection49g-negative-') as tmp:
        store = selector.ObjectStore(archive, Path(tmp), False)
        mutated = copy.deepcopy(member)
        mutated['native']['triangles'] += 1
        rejects('modified native record', lambda: store.add_member(mutated, 'geometry'))
        mutated = copy.deepcopy(member)
        mutated['source_sha256'] = '0' * 64
        rejects('mismatched source hash', lambda: store.add_member(mutated, 'geometry'))
        rejects('collision filename rejected before read', lambda: store.add_file('0' * 64 + '.rrcol', 'neutral_minimum'))
        rejects('path traversal rejected before read', lambda: store.add_file('../forbidden.json', 'neutral_minimum'))
        check(not list(Path(tmp).iterdir()), 'negative tests write nothing')
    reproduction = {'requested': args.reproduce}
    if args.reproduce:
        with tempfile.TemporaryDirectory(prefix='selection49g-reproduce-') as tmp:
            result = subprocess.run([sys.executable, '-B', str(selector_path), '--archive', str(args.archive),
                                     '--manifest', str(args.manifest), '--output', tmp, '--extract-textures'],
                                    check=True, text=True, capture_output=True)
            fresh = Path(tmp)
            check((fresh / 'asset-selection.json').read_bytes() == args.selection.read_bytes(), 'deterministic selection report')
            check((fresh / 'provenance.json').read_bytes() == (base / 'provenance.json').read_bytes(), 'deterministic provenance report')
            for row in data['objects']:
                check((fresh / row['path']).read_bytes() == (base / row['path']).read_bytes(), 'deterministic selected object')
            reproduction.update(status='PASS', native_files_compared=len(data['objects']), reports_compared=2)
    report = {'schema': 'RR_VISUAL_SELECTION49G_VALIDATION_V1', 'status': 'PASS', 'checks': checks,
              'native_files_compared_to_archive': len(data['objects']), 'verified_native_records': records,
              'verified_meshes': meshes, 'verified_texture_mips': textures,
              'resolver_selftests': 7, 'rejected_negative_cases': negative, 'fresh_reproduction': reproduction,
              'selection_sha256': digest(args.selection.read_bytes()), 'verifier_sha256': digest(Path(__file__).read_bytes()),
              'scope': 'Asset extraction and resolution only. Native RRSCENE semantic validation and placement export are separate checks.'}
    (base / 'asset-selection-validation.json').write_text(json.dumps(report, indent=2, sort_keys=True) + '\n')
    print(json.dumps(report))


if __name__ == '__main__':
    main()
