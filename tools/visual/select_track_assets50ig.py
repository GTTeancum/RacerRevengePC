#!/usr/bin/env python3
"""Stage original-track visual closure from the pinned 48F native cache.

Not a gameplay loader or a second-track validation. Does not execute decoders.
The established TA-only selector is unchanged. All track inputs must preserve
container-local resource resolution and authenticated original world identity.

Only the native-cache generation and selected native objects are read. No original
resources, collision payloads, or old source overlays are extracted. The RRWORLD
reader skips original parameter bytes and reads the typed placement metadata.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import math
from pathlib import Path
import re
import struct
import zipfile

ARCHIVE_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
ARCHIVE_BYTES = 256356241
GENERATION = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
MANIFEST_BYTES = 42248551
PREFIX = 'evidence/importFull48F-20261003/native-cache/'
WORLD_KEY = ''
WORLD_SHA = ''
WORLD_BYTES = 0
TRACK = ''
WORLD_MEMBER = ''
WORLD_COUNTS = ()
POD = 'PODS/POD01/POD01.RES'
POD_PARTS = [('Chariot', r'pods\pod01\chariot.psg'), ('LeftEngine', r'pods\pod01\englt1.psg'), ('RightEngine', r'pods\pod01\engrt1.psg')]
SCHEMA = 'RR_TRACK_VISUAL_SELECTION50IG_V1'

# Identities come from retained x64-worlds.json and unchanged RRWORLD1 headers.
TRACK_PROFILES = {'GA': {'container': 'TRACKS/GA.RES',
        'counts': [1, 836, 12, 18, 753],
        'key': '578eeadd6933fa8d95681efddbd4b0889183c91ef4970a0668736125f12888b4',
        'member': 'tracks\\ga.scb',
        'source_sha256': 'ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7',
        'world_bytes': 749005,
        'world_sha256': 'e5694b2826524f0ea41b7e841975c87ac47289d1e11fda56e6c47483ee09efbf'},
 'GB': {'container': 'TRACKS/GB.RES',
        'counts': [1, 1081, 13, 17, 975],
        'key': 'ec02374c215dcb06bea0e6f11f47efb2b6318c36c8d818b4b36b9638cdf1fb24',
        'member': 'tracks\\gb.scb',
        'source_sha256': 'ed0eb494f8c91df5264e120bad3780016f67f297f259572d48a3e7a56aad27bb',
        'world_bytes': 1010359,
        'world_sha256': '8b95e8003ce99fcf396f4481007459258720ac68c734108b901ab0ab09406029'},
 'GC': {'container': 'TRACKS/GC.RES',
        'counts': [1, 1040, 12, 13, 999],
        'key': 'b4394a1b447ed67d094dc239600a8fe1a74a8ba863d2da5473298b92b9d03af5',
        'member': 'tracks\\gc.scb',
        'source_sha256': 'd47a83f4b50812609c923381f3a9038328c81d9a50563b68fb4de022a9be210d',
        'world_bytes': 940917,
        'world_sha256': '23ead718a49238f3d889491f3b15680f25151d9404031ae108628d17b971d3dc'},
 'MA': {'container': 'TRACKS/MA.RES',
        'counts': [1, 1128, 9, 10, 1051],
        'key': '9b4536074df42d9f1f3875c85cc36ca93d94a3f4475ae8c2acf5e9d65089b02f',
        'member': 'tracks\\ma.scb',
        'source_sha256': '5766af3e7a0bba0644058ae6f31eab802cadd9be7cc71911c349f603c7ca873b',
        'world_bytes': 1039858,
        'world_sha256': '49cddec08077f82f9c516786911b627dab7dfc27a984626704f0e2415a20cecd'},
 'MB': {'container': 'TRACKS/MB.RES',
        'counts': [1, 1449, 16, 17, 1018],
        'key': 'da95b79ac80e2647ad1e38ca9ce2cfae45adfec733b9d2436c86bd60e0117a2a',
        'member': 'tracks\\mb.scb',
        'source_sha256': 'e64bd3c7085bd92ef0e6486bbfc2fcfed7aae4462858dcb73a5ef3c893233148',
        'world_bytes': 1425025,
        'world_sha256': '05731aa72728f9f43b7c379b7e2dff468e10af97cebd4e618b027b52f440e714'},
 'RA': {'container': 'TRACKS/RA.RES',
        'counts': [1, 1472, 10, 10, 1261],
        'key': '89c668060947be701ae28acb37b4772156cb6d26f614a353318bfd1f6d0582f8',
        'member': 'tracks\\ra.scb',
        'source_sha256': '57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81',
        'world_bytes': 1342915,
        'world_sha256': '3f3811b6304fa56d4edaccdc0347fe6693a3179f0c0ad98fcc0b3b8571b5f49a'},
 'RC': {'container': 'TRACKS/RC.RES',
        'counts': [1, 1329, 15, 15, 1277],
        'key': 'eeccac5f32592da1735be91de65d9251c1f44f5c59c5eb66fbcb05d8364938fa',
        'member': 'tracks\\rc.scb',
        'source_sha256': '1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a',
        'world_bytes': 1224624,
        'world_sha256': '51ef4b7630711ec82acda41b6ef59c7062712fa18ee430850b94c724d4b2fa76'},
 'SA': {'container': 'TRACKS/SA.RES',
        'counts': [1, 2197, 25, 25, 1869],
        'key': 'fa17b2c503c9a22f84e14b3afcadb3909a36ed6a26737e074b2cbbd6625e3fab',
        'member': 'tracks\\sa.scb',
        'source_sha256': 'dd38f087f0438b2e1279651fe3b4f525b053c90d34cf5365d6bf4ec6c7d1d394',
        'world_bytes': 2005224,
        'world_sha256': '0dfc54e6d556a04d5c36a816ad7119d07db8e6de12f2a5e44983868d2b66e72f'},
 'SC': {'container': 'TRACKS/SC.RES',
        'counts': [1, 4497, 30, 34, 4107],
        'key': 'cfc0bad1a1bad483496be5ca27d08b0a8bff2a20436f34016d28347d2804f80e',
        'member': 'tracks\\sc.scb',
        'source_sha256': '784fb4f60a52ec23ee4c4f7082dd4fb44393421c5336b56f03d49b73f6b8e1ee',
        'world_bytes': 4152477,
        'world_sha256': 'ec222ab62692178887195073d9d760cc2abd5f307e3bb34c3f1b96074b282fb1'},
 'TA': {'container': 'TRACKS/TA.RES',
        'counts': [1, 378, 9, 10, 318],
        'key': '355924f71724924a4de8a942275c2343e0be03fcc2b3039e5ba9c1074235d2cf',
        'member': 'tracks\\ta.scb',
        'source_sha256': 'c3431a55c1f44ae704dae5b031a51f4b7907dbfc3e7787aba4c7ccbf9969031d',
        'world_bytes': 328110,
        'world_sha256': '90f2457c51d4298edbb695e1ae48e4bc860ed49e148e05b4658c6b6c5be105ba'},
 'TB': {'container': 'TRACKS/TB.RES',
        'counts': [1, 1238, 10, 11, 1112],
        'key': '58e8ebe1106d27861bdacbfcf286b63b6c9a72267eb072566dda94ba74646300',
        'member': 'tracks\\tb.scb',
        'source_sha256': 'c3536c6e76dc4d6836ee154aabc80f5e73bd80e85189b841538b3a3ce53e8184',
        'world_bytes': 1133506,
        'world_sha256': '6ff86a6816cfddf41ce59042953c264fd8877a8d2215ccf143f5a69d8701a0d5'},
 'TC': {'container': 'TRACKS/TC.RES',
        'counts': [1, 668, 25, 26, 600],
        'key': 'a712e1477024d5695fea21002deffcf4ebe0522c2137c6ffb13825e080506f15',
        'member': 'tracks\\tc.scb',
        'source_sha256': '795bbbf9882baf233b9d67f777d0bb1e36506dda0f1ad7e71a303faae3993d24',
        'world_bytes': 593192,
        'world_sha256': '2a78521244823534e52f95fd57f0f053e77380a7ccb361300f3f8bcbff597f52'}}


def require(condition, message):
    if not condition:
        raise ValueError(message)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def file_sha(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            digest.update(chunk)
    return digest.hexdigest()


def normal(value):
    return value.replace('\\', '/').upper()


def write_report(path, data):
    temporary = path.with_name(path.name + '.tmp')
    temporary.write_bytes(data)
    temporary.replace(path)


def json_bytes(value):
    return (json.dumps(value, indent=2, sort_keys=True, ensure_ascii=True, allow_nan=False) + '\n').encode('utf-8')


class Reader:
    def __init__(self, data):
        self.data, self.at = data, 0

    def take(self, count):
        require(0 <= count <= len(self.data) - self.at, 'RRWORLD truncated')
        start = self.at
        self.at += count
        return self.data[start:self.at]

    def u32(self):
        return struct.unpack('<I', self.take(4))[0]

    def text(self):
        count = self.u32()
        require(count <= 511, 'RRWORLD text limit')
        data = self.take(count)
        require(all(32 <= x < 127 for x in data), 'RRWORLD text is not printable ASCII')
        return data.decode('ascii')

    def values(self, count):
        values = list(struct.unpack('<' + 'd' * count, self.take(count * 8)))
        require(all(math.isfinite(x) for x in values), 'RRWORLD nonfinite value')
        return values


def world_visual_metadata(data):
    """Read typed RRWORLD1 placement metadata; no raw SCB or COL decoding."""
    r = Reader(data)
    require(r.take(8) == b'RRWORLD1' and r.u32() == 1, 'RRWORLD magic/version')
    require(r.u32() == len(data), 'RRWORLD file size')
    parameter_version, header_offset = r.u32(), r.u32()
    raw_count, blocks, zones, portals, instances, scene_info_block = [r.u32() for _ in range(6)]
    require((parameter_version, blocks, zones, portals, instances) == WORLD_COUNTS, 'Unexpected pinned track RRWORLD counts')
    r.take(raw_count)  # Preserved in the object, never parse original parameter data.
    for _ in range(blocks):
        r.text()
        for _ in range(4):
            r.u32()
        fields = r.u32()
        require(fields <= 1024, 'RRWORLD fields limit')
        for _ in range(fields):
            r.text()
            r.text()
            r.take(12)
    zone_metadata = []
    for _ in range(zones):
        zone_metadata.append(dict(zip(('zone', 'models', 'parameter_block'), [r.u32() for _ in range(3)])))
    for _ in range(portals):
        r.text()
        r.take(12 + 12 * 8)
    result = []
    for index in range(instances):
        section, geometry = r.text(), r.text()
        r.text()  # Skip the collision-name string; never request its payload.
        zone, ordinal, parameter_block, source_block_offset = [r.u32() for _ in range(4)]
        position, look, up, matrix = r.values(3), r.values(3), r.values(3), r.values(16)
        lighting = r.u32()
        require(lighting in (0, 1), 'RRWORLD lighting flag')
        require(matrix[12:15] == position, 'RRWORLD position/matrix mismatch')
        require(section == f'zone{zone}-model{ordinal}', 'RRWORLD placement identity')
        result.append({'world_instance_index': index, 'section': section, 'geometry_member': geometry,
                       'zone': zone, 'ordinal': ordinal, 'parameter_block': parameter_block,
                       'source_block_offset': source_block_offset, 'position': position,
                       'look': look, 'up': up, 'matrix': matrix, 'use_lighting': bool(lighting)})
    require(r.at == len(data), 'RRWORLD trailing bytes')
    return {'instances': result, 'zones': zone_metadata, 'portal_count': portals, 'parameter_blocks': blocks,
            'scene_info_block': scene_info_block, 'parameter_header_offset': header_offset}


def candidate_summary(member):
    fields = ('container', 'member', 'status', 'source_sha256', 'source_bytes', 'logical_offset', 'reason')
    result = {key: member[key] for key in fields if key in member}
    if 'native' in member:
        result['key'] = member['native']['key']
    return result


def resolve_resource(index, container, name, extension, wanted_status):
    """48F: ALL local candidates shadow global before status/hash checks."""
    wanted = normal('textures/' + name + extension)
    global_matches = index.get(wanted, [])
    local = [m for m in global_matches if normal(m['container']) == normal(container)]
    matches = local if local else global_matches
    result = {'container': container, 'name': name, 'requested_member': wanted, 'expected_status': wanted_status,
              'scope': 'local' if local else 'global', 'local_candidates': len(local), 'global_candidates': len(global_matches),
              'shadowed_global_candidates': len(global_matches) - len(local) if local else 0,
              'candidates': [candidate_summary(m) for m in matches]}
    selected = None
    if not matches:
        result.update(status='unresolved', reason='no_exact_member')
    elif any(m['source_sha256'] != matches[0]['source_sha256'] for m in matches):
        result.update(status='unresolved', reason='ambiguous_source_hashes')
    elif any(m['status'] != wanted_status for m in matches):
        result.update(status='unresolved', reason='original_resource_unsupported')
    else:
        selected = matches[0]
        result.update(status='resolved', selected=candidate_summary(selected), key=selected['native']['key'])
    return selected, result



class ObjectStore:
    def __init__(self, archive, output, extract_textures):
        self.archive, self.output, self.extract_textures = archive, output, extract_textures
        self.files, self.records, self.reads = {}, {}, []
        self.name_counts = collections.Counter(archive.namelist())

    def read(self, name):
        require(name.startswith(PREFIX + 'objects/'), 'Outside permitted native object prefix')
        require(self.name_counts[name] == 1, 'Missing or duplicate ZIP object: ' + name)
        self.reads.append(name)
        return self.archive.read(name)

    def add_file(self, filename, role, expected_sha=None, expected_bytes=None, expected_json=None):
        require(re.fullmatch(r'[0-9a-f]{64}(?:-mip\d+)?\.(?:json|rrscene|rrmesh|rrworld|rrtex)', filename), 'Disallowed object filename')
        if filename in self.files:
            previous = self.files[filename]
            require(previous['role'] == role, 'Conflicting object role')
            if expected_sha:
                require(previous['sha256'] == expected_sha, 'Conflicting object hash')
            return previous
        source = PREFIX + 'objects/' + filename
        payload = self.read(source)
        digest = sha(payload)
        require(expected_sha is None or digest == expected_sha, 'Object hash mismatch: ' + filename)
        require(expected_bytes is None or len(payload) == expected_bytes, 'Object size mismatch: ' + filename)
        if expected_json is not None:
            require(json.loads(payload) == expected_json, 'Object record does not match pinned manifest')
        extracted = self.extract_textures or role != 'optional_texture'
        relative = 'cacheobjects/' + filename
        if extracted:
            target = self.output / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            if target.exists():
                require(target.read_bytes() == payload, 'Refusing to replace different existing object: ' + relative)
            else:
                target.write_bytes(payload)
            require(file_sha(target) == digest, 'Written object verification failed')
        report = {'path': relative, 'archive_member': source, 'bytes': len(payload), 'sha256': digest,
                  'role': role, 'extracted': extracted,
                  'hash_basis': 'manifest native payload hash' if expected_sha else 'pinned archive + canonical native record hash'}
        self.files[filename] = report
        return report

    def add_member(self, member, role):
        record = member['native']
        key, source = record['key'], member['source_sha256']
        require(re.fullmatch('[0-9a-f]{64}', key) is not None, 'Invalid native key')
        require(source == record['source_sha256'], 'Source hash mismatch')
        require(sha((record['converter'] + '\n' + source).encode()) == key, 'Native key derivation mismatch')
        fields = dict(record)
        expected = fields.pop('record_sha256')
        canonical = json.dumps(fields, sort_keys=True, separators=(',', ':'), ensure_ascii=True, allow_nan=False).encode()
        require(sha(canonical) == expected, 'Canonical native record hash mismatch')
        if key in self.records:
            require(self.records[key] == record, 'Conflicting metadata for native key')
        self.records[key] = record
        record_role = 'neutral_minimum' if role in ('geometry', 'world') else 'optional_material' if role == 'material' else 'optional_texture'
        self.add_file(key + '.json', record_role, expected_json=record)
        if role == 'geometry':
            self.add_file(key + '.rrscene', 'neutral_minimum', record['scene_sha256'])
            self.add_file(key + '.rrmesh', 'companion_mesh_parity', record['mesh_sha256'])
        elif role == 'world':
            self.add_file(key + '.rrworld', 'neutral_minimum', record['world_sha256'], WORLD_BYTES)
        elif role == 'texture':
            for mip in record['mips']:
                self.add_file(key + f'-mip{mip["index"]}.rrtex', 'optional_texture', mip['sha256'])
        return key


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--manifest', type=Path, help='Optional standalone generation; when omitted use the authenticated archive member')
    parser.add_argument('--track', choices=sorted(TRACK_PROFILES), required=True)
    parser.add_argument('--output', type=Path, required=True, help='New staging directory; never the active TA selection')
    parser.add_argument('--extract-textures', action='store_true', help='Also copy verified optional texture objects')
    global WORLD_KEY, WORLD_SHA, WORLD_BYTES, TRACK, WORLD_MEMBER, WORLD_COUNTS
    args = parser.parse_args()
    profile = TRACK_PROFILES[args.track]
    WORLD_KEY, WORLD_SHA, WORLD_BYTES = profile['key'], profile['world_sha256'], profile['world_bytes']
    TRACK, WORLD_MEMBER, WORLD_COUNTS = profile['container'], profile['member'], tuple(profile['counts'])
    require(not args.output.exists(), 'Use a new output directory; never overwrite a prior track selection')
    require(args.archive.stat().st_size == ARCHIVE_BYTES and file_sha(args.archive) == ARCHIVE_SHA, 'Unrecognized input archive')
    if args.manifest is not None:
        manifest_bytes = args.manifest.read_bytes()
    else:
        with zipfile.ZipFile(args.archive) as source:
            manifest_bytes = source.read(PREFIX + 'generations/' + GENERATION + '.json')
    require(len(manifest_bytes) == MANIFEST_BYTES and sha(manifest_bytes) == GENERATION, 'Unrecognized input manifest')
    manifest = json.loads(manifest_bytes)
    require(manifest['schema'] == 'RRIMPORT48C' and len(manifest['members']) == 34128, 'Unexpected manifest schema/count')
    args.output.mkdir(parents=True, exist_ok=True)
    members = manifest['members']
    by_name = collections.defaultdict(list)
    for m in members:
        by_name[normal(m['member'])].append(m)

    def exact(container, member):
        matches = [m for m in by_name[normal(member)] if normal(m['container']) == normal(container)]
        require(len(matches) <= 1, 'Ambiguous exact resource identity')
        return matches[0] if matches else None

    with zipfile.ZipFile(args.archive) as archive:
        store = ObjectStore(archive, args.output, args.extract_textures)
        archived_manifest = archive.read(PREFIX + 'generations/' + GENERATION + '.json')
        require(archived_manifest == manifest_bytes, 'Standalone manifest differs from archive generation')
        world_member = exact(TRACK, WORLD_MEMBER)
        require(world_member and world_member['status'] == 'converted_world', 'Missing exact selected original world')
        require(world_member['source_sha256'] == profile['source_sha256'] and
                world_member['native']['key'] == WORLD_KEY and
                world_member['native']['world_sha256'] == WORLD_SHA, 'Wrong selected world identity')
        store.add_member(world_member, 'world')
        world_path = args.output / 'cacheobjects' / (WORLD_KEY + '.rrworld')
        world = world_visual_metadata(world_path.read_bytes())
        requests, visual, nondrawn = {}, [], []
        for placement in world['instances']:
            member = exact(TRACK, placement['geometry_member'])
            if member is None or member['status'] != 'converted_geometry':
                nondrawn.append({**placement, 'status': 'non_drawn', 'reason': 'no_exact_PSG_member' if member is None else member['status'],
                                 'candidate': candidate_summary(member) if member else None})
                continue
            key = store.add_member(member, 'geometry')
            request_id = (TRACK, placement['geometry_member'])
            request = requests.setdefault(request_id, {'container': TRACK, 'member': placement['geometry_member'],
                'manifest_member': member['member'], 'key': key, 'source_sha256': member['source_sha256'],
                'status': member['status'], 'native': member['native'], 'world_instance_indices': []})
            request['world_instance_indices'].append(placement['world_instance_index'])
            visual.append({**placement, 'container': TRACK, 'key': key, 'status': 'converted_geometry'})
        require(len(visual)+len(nondrawn) == WORLD_COUNTS[4] and visual, 'Incomplete original world placement closure')
        require(all(row['reason'] == 'no_exact_PSG_member' for row in nondrawn),
                'Referenced original geometry exists but converter did not support it; no partial geometry publication')
        for field, name in POD_PARTS:
            member = exact(POD, name)
            require(member and member['status'] == 'converted_geometry', 'Missing exact Pod Setup component: ' + name)
            key = store.add_member(member, 'geometry')
            requests[(POD, name)] = {'container': POD, 'member': name, 'manifest_member': member['member'],
                'key': key, 'source_sha256': member['source_sha256'], 'status': member['status'],
                'native': member['native'], 'setup_field': field, 'world_instance_indices': []}
        scene_requests = list(requests.values())
        for request in scene_requests:
            request.update(rrscene='cacheobjects/' + request['key'] + '.rrscene', rrmesh='cacheobjects/' + request['key'] + '.rrmesh',
                           record='cacheobjects/' + request['key'] + '.json')
        selection = {'schema': SCHEMA, 'status': 'metadata_only_texture_closure_pending',
                     'world': {**candidate_summary(world_member), 'native': world_member['native'],
                               'rrworld': 'cacheobjects/' + WORLD_KEY + '.rrworld', 'record': 'cacheobjects/' + WORLD_KEY + '.json',
                               'zone_count': len(world['zones']), 'portal_count': world['portal_count']},
                     'scene_requests': scene_requests, 'visual_instances': visual, 'non_drawn_instances': nondrawn}
        write_report(args.output / 'asset-selection.json', json_bytes(selection))
        print(json.dumps({'status': 'metadata_only_visual_objects_selected', 'output': str(args.output), 'scene_requests': len(scene_requests),
                          'unique_scene_keys': len({r['key'] for r in scene_requests})}), flush=True)
        material_uses = collections.defaultdict(list)
        for request in scene_requests:
            for name in request['native']['materials']:
                material_uses[(request['container'], name)].append({'scene_member': request['member'], 'scene_key': request['key']})
        material_reports, texture_uses = [], collections.defaultdict(list)
        for (container, name), uses in sorted(material_uses.items()):
            selected, report = resolve_resource(by_name, container, name, '.psm', 'converted_material')
            report['requested_by'] = uses
            material_reports.append(report)
            if selected is None:
                continue
            store.add_member(selected, 'material')
            report['record'] = 'cacheobjects/' + selected['native']['key'] + '.json'
            for p in selected['native']['passes']:
                for texture in p['textures']:
                    texture_uses[(container, texture['name'])].append({'material_name': name, 'material_key': selected['native']['key'],
                        'pass': p['pass'], 'program': p['program'], 'slot': texture['slot']})
        texture_reports = []
        for (container, name), uses in sorted(texture_uses.items()):
            selected, report = resolve_resource(by_name, container, name, '.pst', 'converted_texture')
            report['requested_by'] = uses
            texture_reports.append(report)
            if selected is not None:
                store.add_member(selected, 'texture')
                report['native'] = selected['native']
            else:
                stem = normal('textures/' + name)
                alternates = [m for m in members if normal(m['container']) == normal(container)
                              and normal(m['member']).rsplit('.', 1)[0] == stem and normal(m['member']) != stem + '.PST']
                report['non_substitutable_same_stem'] = [candidate_summary(m) for m in alternates]
        # Original member metadata only: this does not mean those raw bytes
        # are in the native-object archive or loadable by this runtime.
        track_members = [candidate_summary(m) for m in members if normal(m['container']) == TRACK]
        source_roles = {
            'navigation': [m for m in track_members if normal(m['member']).endswith('.SPL')],
            'environment_tuning': [m for m in track_members if normal(m['member']).endswith('.TUN')],
            'sky': [m for m in track_members if normal(m['member']).endswith('.CUB')],
            'collision': [m for m in track_members if normal(m['member']).endswith('.COL')],
        }
        write_report(args.output / 'original-resource-requirements.json', json_bytes({
            'schema': 'RR_TRACK_ORIGINAL_REQUIREMENTS50IG_V1', 'track_code': args.track,
            'container': TRACK, 'source_roles': source_roles,
            'raw_member_payloads_present': False,
            'notes': 'Manifest identities only. Obtain original container bytes; do not synthesize absent resources.'}))
        # Unsupported local PST/PSM entries stay explicit unresolved entries.
        # Never fill them with another track's same-name resource.
        files = sorted(store.files.values(), key=lambda row: row['path'])
        totals = {}
        for role in ('neutral_minimum', 'companion_mesh_parity', 'optional_material', 'optional_texture'):
            rows = [row for row in files if row['role'] == role]
            totals[role] = {'files': len(rows), 'bytes': sum(row['bytes'] for row in rows), 'extracted_files': sum(row['extracted'] for row in rows),
                            'extracted_bytes': sum(row['bytes'] for row in rows if row['extracted'])}
        count_fields = ('vertices', 'triangles', 'surfaces_all_lods', 'vertices_all_lods', 'nodes', 'lods')
        unique_scenes = {row['key']: row['native'] for row in scene_requests}
        counters = {
            'unique_scene_objects': {field: sum(s[field] for s in unique_scenes.values()) for field in count_fields},
            'track_placed_instances': {field: sum(unique_scenes[row['key']][field] for row in visual) for field in count_fields},
            'POD01_components_once': {field: sum(row['native'][field] for row in scene_requests if row['container'] == POD) for field in count_fields},
            'counter_semantics': 'vertices/triangles are original neutral LOD0 export counts; surfaces_all_lods and vertices_all_lods span all authored LODs. Placement totals count repeated instances.'}
        selection.update(status='selected_native_objects_not_gameplay', track_code=args.track, material_resolution=material_reports, texture_resolution=texture_reports, objects=files, totals=totals, counters=counters,
            counts={'world_instances': WORLD_COUNTS[4], 'track_visual_instances': len(visual), 'track_non_drawn_instances': len(nondrawn), 'track_scene_names': len(scene_requests)-3,
                    'POD01_component_names': 3, 'scene_requests': len(scene_requests), 'unique_scene_keys': len(unique_scenes),
                    'material_bindings': len(material_reports), 'resolved_material_bindings': sum(r['status'] == 'resolved' for r in material_reports),
                    'texture_bindings': len(texture_reports), 'resolved_texture_bindings': sum(r['status'] == 'resolved' for r in texture_reports),
                    'unique_material_keys': len({r['key'] for r in material_reports if r['status'] == 'resolved'}),
                    'unique_texture_keys': len({r['key'] for r in texture_reports if r['status'] == 'resolved'})},
            policy={'geometry': 'Exact container plus normalized member; no geometry fallback.',
                    'materials_and_textures': '48F import_service48c.cpp resolve: all local candidates shadow all global candidates, even unsupported/conflicting; absent local candidates permit global only when all candidate source hashes and expected converted statuses agree.',
                    'pod_selection': 'Original Pod Setup Chariot/LeftEngine/RightEngine producer in tools/assets/decode_scene48c.py:265-285; only the three authorized components.',
                    'matrices': 'Unmodified typed RRWORLD1 arrays. No visibility filtering, collision queries, replacement art, or fabricated placement.',
                    'neutral_minimum': 'Selected original track RRWORLD+record and deduplicated RRSCENE+record; RRSCENE includes neutral LOD0. Companion RRMESH extracted for byte parity.',
                    'textures': 'Inventory of selected static material-pass references; unresolved entries remain explicit. No animation, shader, or unsupported-format substitution.'})
        report_bytes = json_bytes(selection)
        write_report(args.output / 'asset-selection.json', report_bytes)
        provenance = {'schema': 'RR_TRACK_VISUAL_PROVENANCE50IG_V1', 'status': 'selected_source_bytes_only', 'track_code': args.track,
            'archive': {'filename': args.archive.name, 'bytes': ARCHIVE_BYTES, 'sha256': ARCHIVE_SHA, 'library_file_id': 'libfile_063d6101e008819182e86ff7d6b341d8'},
            'manifest': {'filename': args.manifest.name if args.manifest else GENERATION+'.json', 'bytes': MANIFEST_BYTES, 'sha256': GENERATION,
                         'archive_member': PREFIX + 'generations/' + GENERATION + '.json', 'members': len(members)},
            'world': {'key': WORLD_KEY, 'bytes': WORLD_BYTES, 'sha256': WORLD_SHA},
            'selector': {'path': 'tools/visual/select_track_assets50ig.py', 'sha256': file_sha(Path(__file__))},
            'selection': {'path': 'asset-selection.json', 'bytes': len(report_bytes), 'sha256': sha(report_bytes)},
            'checks': {'resolver_selftests_run': False, 'canonical_native_records_verified': len(store.records), 'unique_native_files_verified': len(files),
                       'raw_selected_bytes_unchanged': True, 'standalone_manifest_supplied': args.manifest is not None, 'generation_authenticated': True, 'collision_payloads_read': 0,
                       'original_resource_payloads_extracted': 0, 'archive_source_files_extracted': 0},
            'archive_payload_reads': sorted(set(store.reads)),
            'not_proven': ['original collider closure', 'M/A spline and grid closure',
                          'unsupported material/texture decode', 'sky closure',
                          'native track binding', 'gameplay or video proof'],
            'reproduce': ['python tools/visual/select_track_assets50ig.py --archive /consumer-local/path/' +
                          args.archive.name + ' --track ' + args.track +
                          ' --output /new/staging/path --extract-textures']}
        write_report(args.output / 'provenance.json', json_bytes(provenance))
        print(json.dumps({'status': 'selected_source_bytes_only', 'counts': selection['counts'], 'totals': totals,
                          'unresolved_textures': [{k: r[k] for k in ('container', 'name', 'reason')} for r in texture_reports if r['status'] != 'resolved'],
                          'counters': counters}), flush=True)


if __name__ == '__main__':
    main()
