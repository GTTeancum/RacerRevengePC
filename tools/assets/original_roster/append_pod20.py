#!/usr/bin/env python3
"""Cold, one-shot POD20 addition to one authenticated eight-plus-POD19 archive.

Writes only a new output directory. Never launches the game or imports the base
pods. Run centrally with an explicitly reviewed native importer.
"""
import argparse
import copy
import hashlib
import json
import os
from pathlib import Path
import re
import stat
import sys
import tarfile
import zipfile

sys.dont_write_bytecode = True
from prepare_original_roster import prepare_pod, require
from select_original_roster import ARCHIVE_SHA, GENERATION, PREFIX

BASE_SHA = '7e8679cb981f12f0b0444e69fc4bf03b852f6c2d1839a4a2606a5ce7014b3805'
BASE_BYTES = 12860696
POD20_SHA = '8c5f282b53c230861e26acf797b8ab2ba1ef8856b8b476e3cfea55b48e3040ec'
POD20_PHY = 'a047bb90bb1275ad5d31e596cea5eaa02fd63ba9bbdc4ef931835f567c87d19d'
FIELD = tuple(f'POD{i:02d}' for i in (1, 3, 4, 7, 8, 9, 10, 11))
BASE_PODS = (*FIELD, 'POD19')
MAX_FILE = 128 * 1024 * 1024
MAX_TOTAL = 512 * 1024 * 1024
RECEIPTS = ('base-files.json', 'pod20-addition.json')


def digest(data):
    return hashlib.sha256(data).hexdigest()


def file_sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def json_bytes(value):
    return (json.dumps(value, indent=2, sort_keys=True, ensure_ascii=True) + '\n').encode('utf-8')


def read_json(data):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            require(key not in result, 'Duplicate JSON key: ' + key)
            result[key] = value
        return result
    return json.loads(data, object_pairs_hook=unique)


def relative_name(name):
    require(isinstance(name, str) and name and '\\' not in name, 'Unsafe relative path')
    parts = name.split('/')
    for part in parts:
        require(part not in ('', '.', '..') and not part.endswith(('.', ' ')), 'Unsafe path: ' + name)
        require(not re.search(r'[\x00-\x1f<>:"|?*]', part), 'Unsafe path: ' + name)
        require(not re.fullmatch(r'(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\..*)?', part, re.I),
                'Reserved Windows path: ' + name)
    return '/'.join(parts)


def no_links(path):
    # Resolve only after checking symlinks/junctions, including existing parents.
    path = Path(os.path.abspath(path))
    for part in (path, *path.parents):
        if os.path.lexists(part):
            info = part.lstat()
            require(not stat.S_ISLNK(info.st_mode) and
                    not (getattr(info, 'st_file_attributes', 0) & 0x400),
                    'Symlink/reparse-point path is not accepted: ' + str(part))
    return path


def overlap(left, right):
    left, right = str(left.resolve()).casefold(), str(right.resolve()).casefold()
    return left == right or left.startswith(right + os.sep) or right.startswith(left + os.sep)


def write_new(root, name, data):
    path = no_links(root / relative_name(name))
    require(path.is_relative_to(root), 'Output path escape')
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as stream:
        stream.write(data)


def full48_manifest(archive):
    require(file_sha(archive) == ARCHIVE_SHA, 'Full48F archive SHA-256 mismatch')
    member = PREFIX + 'generations/' + GENERATION + '.json'
    with zipfile.ZipFile(archive) as source:
        matches = [entry for entry in source.infolist() if entry.filename == member]
        require(len(matches) == 1 and matches[0].file_size <= MAX_FILE, 'Missing/ambiguous full48F generation')
        raw = source.read(matches[0])
    require(digest(raw) == GENERATION, 'Full48F generation SHA-256 mismatch')
    return read_json(raw)


def archive_files(source, requested_prefix):
    entries, names = [], set()
    for entry in source:
        require(len(entries) < 50000, 'Base archive entry count exceeds bound')
        name = entry.name
        if name.startswith('./'):
            name = name[2:]
        if entry.isdir() and name in ('', '.'):
            continue
        name = relative_name(name.rstrip('/') if entry.isdir() else name)
        require(name.casefold() not in names, 'Duplicate/case-colliding archive path: ' + name)
        names.add(name.casefold())
        require(entry.isdir() or entry.isfile(), 'Archive links/special files are not accepted')
        entries.append((name, entry))
    if requested_prefix is None:
        rosters = [name for name, entry in entries if entry.isfile() and name.split('/')[-1] == 'roster.json']
        require(len(rosters) == 1, 'Supply --base-prefix to select one roster.json in the authenticated archive')
        prefix = rosters[0][:-len('roster.json')]
    else:
        prefix = '' if requested_prefix == '.' else relative_name(requested_prefix) + '/'
    files = {}
    for name, entry in entries:
        if entry.isfile() and name.startswith(prefix):
            relative = relative_name(name[len(prefix):])
            require(entry.size >= 0 and entry.size <= MAX_FILE, 'Base file exceeds bound: ' + relative)
            files[relative] = entry
    require('roster.json' in files and sum(entry.size for entry in files.values()) <= MAX_TOTAL,
            'Missing base roster or base payload exceeds bound')
    for name in files:
        require(not any(parent in files for parent in (str(p).replace('\\', '/') for p in Path(name).parents) if parent != '.'),
                'Archive file/directory collision: ' + name)
        first = name.split('/')[0]
        if re.fullmatch(r'pod\d{2}', first, re.I):
            require(first in {pod.lower() for pod in BASE_PODS}, 'Unexpected base POD payload: ' + first)
    require(not set(RECEIPTS).intersection(files), 'Reserved provenance filename already exists')
    return prefix, files


def read_member(source, files, name):
    name = relative_name(name)
    require(name in files, 'Missing authenticated base file: ' + name)
    with source.extractfile(files[name]) as stream:
        data = stream.read(MAX_FILE + 1)
    require(len(data) == files[name].size and len(data) <= MAX_FILE, 'Base member size differs: ' + name)
    return data


def lookup(rows, key, expected):
    matches = [row for row in rows if row[key].replace('\\', '/').lower() == expected.replace('\\', '/').lower()]
    require(len(matches) == 1, 'Missing/ambiguous authenticated member: ' + expected)
    return matches[0]


def check_pod(pod_id, row, pod, report, read_payload, manifest, identities):
    container = f'PODS/{pod_id}/{pod_id}.RES'
    original = lookup(manifest['containers'], 'name', container)
    require(row['pod_id'] == pod['pod_id'] == pod_id and row['path'] == pod_id.lower() + '/pod.json',
            'Roster POD identity/path differs: ' + pod_id)
    require(row['resource_sha256'] == pod['resource_sha256'] == report['source_container_sha256'] == original['sha256'],
            'Original container identity differs: ' + pod_id)
    members = [item for item in manifest['members'] if item['container'].replace('\\', '/').lower() == container.lower()]
    physics = [item for item in report['members'] if item['kind'] == 'phy']
    require(len(physics) == 1 and physics[0] == pod['phy'] and physics[0]['native_physics_49n_accepted'],
            'Expected one accepted original PHY: ' + pod_id)
    phy = physics[0]
    original_phy = lookup(members, 'member', phy['source_member'])
    require(row['phy_sha256'] == phy['source_sha256'] == original_phy['source_sha256'] ==
            digest(read_payload(relative_name(phy['source_file']))), 'Original PHY hash differs: ' + pod_id)
    if pod_id in identities:
        identity = identities[pod_id]
        require(original['bytes'] == identity['resource_bytes'] and original['sha256'] == identity['resource_sha256'] and
                row['phy_sha256'] == identity['phy_sha256'], 'Supplemental identity differs: ' + pod_id)
    require([part['role'] for part in pod['components']] == ['Chariot', 'LeftEngine', 'RightEngine'],
            'Expected original three-component topology: ' + pod_id)
    for part in pod['components']:
        authored = lookup(members, 'member', part['source_member'])
        require(part['source_sha256'] == authored['source_sha256'] == digest(read_payload(relative_name(part['source_file']))),
                'Original component source differs: ' + pod_id)
        for field, expected in (('rrscene', 'scene_sha256'), ('rrmesh', 'mesh_sha256')):
            require(part[field + '_sha256'] == authored['native'][expected] == digest(read_payload(relative_name(part[field]))),
                    'Native component differs from full48F: ' + pod_id)


def original_resource(root, pod_id, manifest):
    path = root
    for component in ('PODS', pod_id, pod_id + '.RES'):
        matches = [entry for entry in path.iterdir() if entry.name.casefold() == component.casefold()]
        require(len(matches) == 1, 'Missing/ambiguous original path: ' + str(path / component))
        path = no_links(matches[0])
    identity = lookup(manifest['containers'], 'name', f'PODS/{pod_id}/{pod_id}.RES')
    require(path.is_file() and path.stat().st_size == identity['bytes'] and file_sha(path) == identity['sha256'],
            'Cannot rebase to different original RES: ' + pod_id)
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base-archive', type=Path, required=True)
    parser.add_argument('--base-prefix', help='Relative roster root inside pinned TAR.GZ; . means archive root; otherwise inferred only if unique')
    parser.add_argument('--full48f-archive', type=Path, required=True)
    parser.add_argument('--pod20-resource', type=Path, required=True)
    parser.add_argument('--importer', type=Path, required=True)
    parser.add_argument('--importer-sha256', required=True, help='Independently reviewed executable digest, not its source digest')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--rebase-original-assets-root', type=Path, help='Optional original root; hash-check each base RES and record every path change')
    args = parser.parse_args()
    for key in ('base_archive', 'full48f_archive', 'pod20_resource', 'importer', 'output', 'rebase_original_assets_root'):
        value = getattr(args, key)
        if value is not None:
            setattr(args, key, no_links(value))
    require(not os.path.lexists(args.output), 'Output must not exist; choose a fresh directory')
    require(args.output.parent.is_dir(), 'Create the output parent explicitly before preparation')
    inputs = (args.base_archive, args.full48f_archive, args.pod20_resource, args.importer, Path(__file__).resolve().parent)
    for path in (*inputs, *((args.rebase_original_assets_root,) if args.rebase_original_assets_root else ())):
        require(not overlap(args.output, path), 'Output overlaps an input/source path: ' + str(path))
    for path in inputs[:4]:
        require(path.is_file(), 'Missing input file: ' + str(path))
    require(re.fullmatch(r'[0-9a-f]{64}', args.importer_sha256) and file_sha(args.importer) == args.importer_sha256,
            'Native importer executable SHA-256 mismatch')
    require(args.base_archive.stat().st_size == BASE_BYTES and file_sha(args.base_archive) == BASE_SHA,
            'Base must be the authenticated rider19-original-roster-prepared-runtime.tar.gz')
    require(args.pod20_resource.stem.upper() == 'POD20' and args.pod20_resource.suffix.lower() == '.res' and
            args.pod20_resource.stat().st_size == 309724 and file_sha(args.pod20_resource) == POD20_SHA,
            'POD20 requires its exact original RES (309724 bytes); substitutes are forbidden')
    catalog = read_json(Path(__file__).with_name('additional_pod_identities.json').read_bytes())
    require(catalog['schema'] == 'RR_ADDITIONAL_POD_IDENTITIES_V1' and catalog['source_generation_sha256'] == GENERATION,
            'Additional identity catalog generation differs')
    identities = catalog['pods']
    require(identities['POD20']['resource_sha256'] == POD20_SHA and identities['POD20']['phy_sha256'] == POD20_PHY and
            identities['POD20']['resource_bytes'] == 309724, 'Pinned POD20 identity differs')
    manifest = full48_manifest(args.full48f_archive)
    with tarfile.open(args.base_archive, 'r:gz') as source:
        prefix, files = archive_files(source, args.base_prefix)
        base = read_json(read_member(source, files, 'roster.json'))
        require(base['schema'] == 'RR_ORIGINAL_ROSTER_V1' and base['player_index'] == 0 and
                base['complete_eight_racer_field'] is True and tuple(row['pod_id'] for row in base['racers']) == FIELD,
                'Base is not the exact original eight-racer field')
        require([row['pod_id'] for row in base.get('additional_racers', [])] == ['POD19'],
                'Base must retain exactly POD19; duplicate/other additions are rejected')
        replacements, rebases = {}, []
        for row in (*base['racers'], *base['additional_racers']):
            pod_id = row['pod_id']
            directory = pod_id.lower() + '/'
            pod = read_json(read_member(source, files, directory + 'pod.json'))
            report = read_json(read_member(source, files, directory + 'import.json'))
            check_pod(pod_id, row, pod, report, lambda name: read_member(source, files, directory + name), manifest, identities)
            if args.rebase_original_assets_root:
                current = str(original_resource(args.rebase_original_assets_root, pod_id, manifest))
                rebases.append({'pod_id': pod_id, 'from': pod['resource_file'], 'to': current,
                                'resource_sha256': pod['resource_sha256']})
                pod['resource_file'] = current
                replacements[directory + 'pod.json'] = json_bytes(pod)
        canonical = [{'path': name, 'archive_member': files[name].name, 'bytes': files[name].size,
                      'sha256': digest(read_member(source, files, name))} for name in sorted(files)]
        base_manifest = {'schema': 'RR_AUTHENTICATED_BASE_FILES_V1', 'archive_sha256': BASE_SHA,
                         'archive_bytes': BASE_BYTES, 'prefix': prefix, 'files': canonical}
        args.output.mkdir()  # Exclusive; failure never overwrites or deletes an existing tree.
        for record in canonical:
            name = record['path']
            data = read_member(source, files, name)
            require(digest(data) == record['sha256'], 'Base member changed during preparation')
            if name != 'roster.json':
                write_new(args.output, name, replacements.get(name, data))
        write_new(args.output, RECEIPTS[0], json_bytes(base_manifest))
    # Invoke the existing cold importer once, for POD20 only. All existing
    # exact-RES, PHY, speech, geometry and local-material checks remain active.
    require(file_sha(args.importer) == args.importer_sha256, 'Importer changed before invocation')
    pod = prepare_pod(args.importer, args.pod20_resource, args.output / 'pod20', identities)
    row = {'pod_id': 'POD20', 'name': pod['name'], 'path': 'pod20/pod.json',
           'resource_sha256': pod['resource_sha256'], 'phy_sha256': pod['phy']['source_sha256'],
           'component_node_counts': [part['nodes'] for part in pod['components']],
           'component_vertex_counts': [part['vertices'] for part in pod['components']]}
    report = read_json((args.output / 'pod20/import.json').read_bytes())
    check_pod('POD20', row, pod, report, lambda name: (no_links(args.output / 'pod20' / name)).read_bytes(), manifest, identities)
    result = copy.deepcopy(base)
    result['additional_racers'].append(row)
    require(file_sha(args.base_archive) == BASE_SHA and file_sha(args.pod20_resource) == POD20_SHA,
            'Original input changed during preparation')
    receipt = {'schema': 'RR_POD20_ADDITION_V1', 'base_archive_sha256': BASE_SHA,
               'base_files_sha256': digest(json_bytes(base_manifest)), 'full48f_archive_sha256': ARCHIVE_SHA,
               'full48f_generation_sha256': GENERATION, 'importer_sha256': args.importer_sha256,
               'resource_sha256': POD20_SHA, 'phy_sha256': POD20_PHY, 'rebased_resources': rebases,
               'preserved_field': list(FIELD), 'preserved_additional_racers': ['POD19'],
               'added_racers': ['POD20'], 'roster_sha256': digest(json_bytes(result))}
    write_new(args.output, RECEIPTS[1], json_bytes(receipt))
    write_new(args.output, 'roster.json', json_bytes(result))  # Completion marker, always last.
    print(json.dumps({'output': str(args.output), 'roster_sha256': receipt['roster_sha256'], 'added': 'POD20'}))


if __name__ == '__main__':
    main()
