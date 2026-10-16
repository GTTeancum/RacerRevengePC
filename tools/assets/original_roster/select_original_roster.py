#!/usr/bin/env python3
"""Extract unchanged original pod cache objects; do not invent missing authoring.

The complete48F native cache retains geometry/material/texture conversions but
not original PHY/MOT/MCF/VUT bytes. This visual-only catalog explicitly reports
those missing inputs and must not be advertised as complete racer simulation.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import stat
import zipfile

ARCHIVE_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
GENERATION = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
PREFIX = 'evidence/importFull48F-20261003/native-cache/'
PODS = (1, 3, 4, 7, 8, 9, 10, 11)
ROLES = (('Chariot','chariot'),('LeftEngine','englt1'),('RightEngine','engrt1'))


def need(v, why):
    if not v:
        raise ValueError(why)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def norm(s):
    return s.replace('\\', '/').lower()


def summarize(row):
    return {k: row[k] for k in ('container', 'member', 'source_bytes', 'source_sha256', 'logical_offset', 'status')}


class SelectedObjects:
    def __init__(self, archive, output):
        self.archive, self.output = archive, output
        self.counts = collections.Counter(archive.namelist())
        self.files = {}
        self.current_provenance_files = {}

    def copy(self, name, expected):
        need(re.fullmatch(r'[0-9a-f]{64}(?:-mip\d+)?\.(?:json|rrscene|rrmesh|rrtex)', name), 'Unsafe cache object filename')
        full = PREFIX + 'objects/' + name
        need(self.counts[full] == 1, 'Missing or duplicate cached object: ' + full)
        data = self.archive.read(full)
        need(sha(data) == expected, 'Original cache object hash mismatch: ' + name)
        relative = 'cacheobjects/' + name
        if relative not in self.files:
            (self.output / relative).write_bytes(data)
            self.files[relative] = {'sha256': expected, 'bytes': len(data)}
        else:
            need((self.output / relative).read_bytes() == data,
                 'Conflicting historical/current cache object: ' + relative)
        return relative

    def record(self, row):
        native = row['native']
        full = PREFIX + 'objects/' + native['key'] + '.json'
        need(self.counts[full] == 1, 'Missing original metadata')
        data = self.archive.read(full)
        need(json.loads(data) == native, 'Native metadata differs from original manifest')
        return self.copy(native['key'] + '.json', sha(data))

    def current_file(self, relative, data):
        # Only internally constructed paths reach this method. Never overwrite
        # an already selected historical object with a different conversion.
        target = self.output / relative
        if relative in self.files or relative in self.current_provenance_files:
            need(target.read_bytes() == data, 'Conflicting current material object: ' + relative)
        else:
            need(not target.exists(), 'Existing current material output: ' + relative)
            target.parent.mkdir(parents=True, exist_ok=True)
            with target.open('xb') as stream:
                stream.write(data)
            # The runtime's files table admits only cacheobjects/<hash>.*.
            # PSM/report provenance is retained separately, as authoring is.
            table = self.files if relative.startswith('cacheobjects/') else self.current_provenance_files
            table[relative] = {'sha256':sha(data), 'bytes':len(data)}
        return relative


class Pod20CurrentMaterials:
    """Use only an independently pinned current POD20 importer report.

    Original identities come from authenticated Full48F members. Current
    converted metadata comes from the separately authenticated importer report;
    it is never presented as a historical Full48F conversion.
    """
    def __init__(self, raw_imports, expected_sha256, output, identity):
        need(raw_imports is not None and re.fullmatch(r'[0-9a-f]{64}', expected_sha256),
             'Current POD20 materials require --raw-imports and a reviewed import.json SHA-256')
        self.directory = raw_imports.absolute() / 'pod20'
        self.report_path = self.directory / 'import.json'
        self.expected_sha256 = expected_sha256
        self.report_bytes = self.read_regular(self.report_path)
        need(sha(self.report_bytes) == expected_sha256, 'Current POD20 importer report SHA mismatch')
        self.report = json.loads(self.report_bytes)
        need(self.report['schema'] == 'RR_ORIGINAL_POD_IMPORT_V1' and
             norm(self.report['source_container']) == 'pod20.res' and
             self.report['source_container_bytes'] == identity['resource_bytes'] and
             self.report['source_container_sha256'] == identity['resource_sha256'],
             'Current importer must describe the exact original POD20 RES')
        root, destination = raw_imports.resolve(), output.resolve()
        need(root != destination and root not in destination.parents and destination not in root.parents,
             'Current material output must be separate from raw imports')
        self.members = {}
        for row in self.report['members']:
            key = norm(row['source_member'])
            need(key not in self.members, 'Duplicate current POD20 member: ' + key)
            self.members[key] = row
        self.used = {}

    @staticmethod
    def read_regular(path):
        for item in (path, *path.parents):
            info = item.lstat()
            need(not stat.S_ISLNK(info.st_mode) and
                 not (getattr(info, 'st_file_attributes', 0) & 0x400),
                 'Current material input traverses a link/reparse point')
        need(stat.S_ISREG(path.stat().st_mode), 'Current material input must be a regular file')
        return path.read_bytes()

    def select(self, index, container, member):
        need(norm(container) == 'pods/pod20/pod20.res' and
             re.fullmatch(r'textures/[a-z0-9_]+\.psm', norm(member)),
             'Current material fallback is restricted to original POD20 PSM members')
        old = [row for row in index.get(norm(member), []) if norm(row['container']) == norm(container)]
        need(len(old) == 1 and old[0]['status'] == 'unsupported',
             'Current POD20 material needs one historical unsupported local member: ' + member)
        original = old[0]
        current = self.members.get(norm(member))
        need(current is not None and current['kind'] == 'psm' and current['status'] == 'converted_material',
             'Current native importer has not admitted original POD20 material: ' + member)
        need(current['source_sha256'] == original['source_sha256'] and
             current['source_bytes'] == original['source_bytes'] and
             current['logical_offset'] == original['logical_offset'],
             'Current material source identity/extent differs from Full48F: ' + member)
        filename = current['source_file']
        need(re.fullmatch(r'[0-9]+-original\.psm', filename) is not None,
             'Unsafe original current PSM filename')
        data = self.read_regular(self.directory / filename)
        need(len(data) == original['source_bytes'] and sha(data) == original['source_sha256'],
             'Actual original PSM bytes differ from authenticated Full48F member: ' + member)
        passes = current['passes']
        need(isinstance(current['general'], dict) and 1 <= len(passes) <= 8 and
             [part['pass'] for part in passes] == list(range(1, len(passes) + 1)),
             'Current material numbered-pass metadata is malformed')
        for part in passes:
            need(isinstance(part['fields'], dict) and isinstance(part['texture_count_defaulted'], bool) and
                 isinstance(part['program'], str) and 1 <= len(part['textures']) <= 2 and
                 [slot['slot'] for slot in part['textures']] == list(range(1, len(part['textures']) + 1)),
                 'Current material texture-pass metadata is malformed')
        native = dict(converter='rr-native-material-48E-v1', type='material',
                      source_sha256=original['source_sha256'], general=current['general'], passes=passes)
        native['key'] = sha((native['converter'] + '\n' + native['source_sha256']).encode())
        native['record_sha256'] = sha(json.dumps(native, sort_keys=True, separators=(',', ':'),
                                                ensure_ascii=True, allow_nan=False).encode())
        proof = {'origin':'current_native_pod20_import', 'import_report_sha256':self.expected_sha256,
                 'historical_generation':GENERATION, 'historical_member':dict(original),
                 'current_status':current['status'], 'source_file':'authoring/POD20/' + filename}
        row = dict(summarize(original), status='converted_material', native=native,
                   conversion_provenance=proof)
        self.used[norm(member)] = (row, data)
        return row

    def record(self, store, row):
        selected, data = self.used[norm(row['member'])]
        need(row == selected, 'Current material selection changed')
        store.current_file('provenance/pod20-current-import.json', self.report_bytes)
        store.current_file(row['conversion_provenance']['source_file'], data)
        native = row['native']
        encoded = (json.dumps(native, sort_keys=True, indent=2, ensure_ascii=True, allow_nan=False) + '\n').encode()
        return store.current_file('cacheobjects/' + native['key'] + '.json', encoded)

    def provenance(self):
        need(sha(self.read_regular(self.report_path)) == self.expected_sha256,
             'Current POD20 importer report changed during selection')
        return {'origin':'current_native_pod20_import', 'import_report':'provenance/pod20-current-import.json',
                'import_report_sha256':self.expected_sha256,
                'materials':[row['conversion_provenance'] for row, data in self.used.values()]}


def resolve(index, container, member, status):
    all_matches = index.get(norm(member), [])
    local = [r for r in all_matches if norm(r['container']) == norm(container)]
    candidates = local or all_matches
    report = {'member': member, 'scope': 'local' if local else 'global', 'candidates': [summarize(r) for r in candidates]}
    if not candidates:
        return None, dict(report, status='unresolved', reason='missing_original_member')
    if len({r['source_sha256'] for r in candidates}) != 1:
        return None, dict(report, status='unresolved', reason='ambiguous_original_hashes')
    if any(r['status'] != status for r in candidates):
        return None, dict(report, status='unresolved', reason='original_resource_unsupported')
    return candidates[0], dict(report, status='resolved', selected=summarize(candidates[0]))


def attach_original_names(result, audio_root, output=None, identities=None):
    ui = audio_root / 'UIACCEPT.INI'
    raw = ui.read_bytes()
    need(sha(raw) == '245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b', 'Original UI selection authoring SHA mismatch')
    by_prefix, by_event = {}, {}
    comment = None
    event = None
    offset = 0
    for line in raw.splitlines(keepends=True):
        text = line.decode('ascii').strip()
        if text.startswith(';'):
            comment = (text[1:].strip(), offset)
        event_match = re.fullmatch(r'\[Event(\d+)\]', text)
        if event_match:
            event = int(event_match[1])
        match = re.match(r'FileName\s*=\s*speech\\UI\\([a-z0-9]+)sel\d+\.vag', text, re.I)
        if match and comment:
            by_prefix[match[1].lower()] = comment
            by_event[event] = comment
        offset += len(line)
    result['name_source'] = {'path':'AUDIO/UIACCEPT.INI', 'sha256':sha(raw)}
    if output:
        (output / 'name-provenance').mkdir(exist_ok=True)
        (output / 'name-provenance/UIACCEPT.INI').write_bytes(raw)
        result['name_source']['retained_file'] = 'name-provenance/UIACCEPT.INI'
    for racer in result['racers']:
        pod = racer['pod_id']
        source = audio_root / 'SPEECH' / pod / 'TAUNTS.INI'
        speech = source.read_bytes()
        identity = (identities or {}).get(pod)
        if identity:
            need(sha(speech) == identity['speech_sha256']['taunts'],
                 'Original selected pod speech identity differs: ' + pod)
        if output:
            (output / ('name-provenance/' + pod + '-TAUNTS.INI')).write_bytes(speech)
        matches = re.findall(rb'FileName\s*=\s*speech\\' + pod.encode() + rb'\\([a-z0-9]{3})[a-z0-9_]+\.vag', speech, re.I)
        prefixes = {p.decode('ascii').lower() for p in matches}
        need(len(prefixes) == 1, 'Pod speech identity not unique: ' + pod)
        prefix = prefixes.pop()
        if pod == 'POD19':
            # Original constructor1E7CF8/1E7E3C binds rider19 to UI Event39
            # (five choices). Its UI voice prefix a2a differs from in-race an2.
            need(prefix == 'an2' and sha(speech) ==
                 '71177c4e7dee02bbc31ac0bb12958ba4a8df4e4f62e84755913454f8511b4670',
                 'Original POD19 in-race speech identity differs')
            need(39 in by_event, 'Original rider19 UI Event39 name unavailable')
            name, at = by_event[39]
        else:
            need(prefix in by_prefix, 'Original UI selection name unavailable: ' + prefix)
            name, at = by_prefix[prefix]
        racer['display_name'] = name
        racer['display_name_verified'] = True
        racer['display_name_source'] = {'ui_comment_byte_offset':at, 'speech_prefix':prefix,
            'pod_speech_member':'AUDIO/SPEECH/' + pod + '/TAUNTS.INI', 'pod_speech_sha256':sha(speech),
            'method':'Exact original UI selection comment matched to original per-pod speech clip prefix.'}
        if pod == 'POD19':
            racer['display_name_source'].update(ui_event_key=39,
                method='Original Rider19 constructor1E7CF8/1E7E3C maps to authenticated UIACCEPT Event39; own POD19 TAUNTS hash binds its different an2 speech prefix.')
        if output:
            racer['display_name_source']['retained_file'] = 'name-provenance/' + pod + '-TAUNTS.INI'


def attach_original_authoring(result, raw_imports, output):
    for racer in result['racers']:
        pod = racer['pod_id']
        directory = raw_imports / pod.lower()
        source = json.loads((directory / 'import.json').read_text())
        need(source['source_container_sha256'] == racer['container_sha256'], 'Original container identity differs')
        index = {norm(r['source_member']): r for r in source['members']}
        authoring = output / 'authoring' / pod
        authoring.mkdir(parents=True, exist_ok=True)
        def retain(row, member, target_field):
            actual = index[norm(member)]
            data = (directory / actual['source_file']).read_bytes()
            need(sha(data) == row['source_sha256'], 'Original authoring member identity differs')
            name = actual['source_file']
            need(Path(name).name == name, 'Unsafe original source filename')
            out = authoring / name
            if out.exists():
                need(out.read_bytes() == data, 'Original authoring collision')
            else:
                out.write_bytes(data)
            row[target_field] = str(out.relative_to(output))
        for row in racer['missing_authoring']:
            retain(row, row['member'], 'raw_file')
            row['raw_bytes_available'] = True
        for component in racer['components']:
            retain(component, component['member'], 'original_psg')
        physics = [r for r in racer['missing_authoring'] if norm(r['member']).endswith('.phy')]
        need(len(physics) == 1, 'Missing original PHY identity')
        racer['original_phy'] = physics[0]
        racer['all_original_authoring_available'] = True
    result['original_authoring_available'] = True
    result['limits'] = [line for line in result['limits'] if not line.startswith('Opponent original PHY')]
    result['limits'].insert(0, 'Original PHY/MCF/MOT/VUT and selected PSG sources are retained in authoring/ and hash-checked against original48F member identities.')


def main():
    catalog = json.loads(Path(__file__).with_name('additional_pod_identities.json').read_text())
    need(catalog.get('schema') == 'RR_ADDITIONAL_POD_IDENTITIES_V1' and
         catalog.get('source_generation_sha256') == GENERATION, 'Additional identity source differs')
    identities = catalog['pods']
    p = argparse.ArgumentParser()
    p.add_argument('--archive', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--audio-root', type=Path)
    p.add_argument('--raw-imports', type=Path)
    p.add_argument('--pod20-current-import-sha256',
                   help='Independently reviewed SHA-256 of raw-imports/pod20/import.json; enables exact POD20 material fallback only')
    p.add_argument('--additional-pods', nargs='+', type=int, choices=[row['racer60'] for row in identities.values()], default=[])
    args = p.parse_args()
    need(not args.output.exists(), 'Output must be new')
    with args.archive.open('rb') as f:
        need(hashlib.file_digest(f, 'sha256').hexdigest() == ARCHIVE_SHA, 'Archive SHA mismatch')
    with zipfile.ZipFile(args.archive) as z:
        raw = z.read(PREFIX + 'generations/' + GENERATION + '.json')
        need(sha(raw) == GENERATION, 'Full manifest SHA mismatch')
        manifest = json.loads(raw)
        index = collections.defaultdict(list)
        for row in manifest['members']:
            index[norm(row['member'])].append(row)
        containers = {norm(r['name']): r for r in manifest['containers']}
        need(len(set(args.additional_pods)) == len(args.additional_pods), 'Duplicate additional pod')
        need(not set(PODS).intersection(args.additional_pods), 'Additional pod duplicates fixed field')
        for number in args.additional_pods:
            pod = f'POD{number:02d}'
            need(pod in identities, 'Additional pod identity unavailable')
            identity = identities[pod]
            container = containers.get(norm(identity['resource_path']))
            need(container is not None and container['bytes'] == identity['resource_bytes'] and
                 container['sha256'] == identity['resource_sha256'],
                 'Actual48F additional container identity differs: ' + pod)
        current_materials = None
        if args.pod20_current_import_sha256 is not None:
            need(20 in args.additional_pods, 'Current POD20 material conversion requires --additional-pods 20')
            current_materials = Pod20CurrentMaterials(args.raw_imports, args.pod20_current_import_sha256,
                                                     args.output, identities['POD20'])
        args.output.mkdir(parents=True)
        (args.output / 'cacheobjects').mkdir()
        store = SelectedObjects(z, args.output)
        result = {'schema':'RR_ORIGINAL_ROSTER_VISUAL_V1', 'source_archive_sha256':ARCHIVE_SHA,
                  'source_generation':GENERATION, 'player_index':0, 'racers':[],
                  'ready_for_complete_original_simulation':False,
                  'limits':['Opponent original PHY, MCF, MOT and VUT are absent from the native48F cache.',
                            'No name or simulation identity is fabricated from Pod numbers.',
                            'Decoded scene data retains each pod topology, material assignments and original bind transforms.',
                            'Original pass metadata is retained; unresolved secondary textures are explicit.']}
        for number in (*PODS, *args.additional_pods):
            pod = f'POD{number:02d}'
            container = f'PODS/{pod}/{pod}.RES'
            local = [r for r in manifest['members'] if norm(r['container']) == norm(container)]
            racer = {'pod_id':pod, 'container':container, 'container_sha256':containers[norm(container)]['sha256'],
                     'display_name':None, 'display_name_verified':False, 'components':[], 'materials':[], 'textures':[],
                     'missing_authoring':[], 'texture_bindings':[]}
            # POD02 authors two additional independent engine graphics models.
            # Preserve their own original scene identities in the five-model
            # native pipeline; never duplicate the primary engine meshes.
            if number == 2:
                racer['additional_engine_source_scenes'] = [summarize(r) for r in local
                    if norm(r['member']) in ('pods/pod02/englt2.psg', 'pods/pod02/engrt2.psg')]
                need(len(racer['additional_engine_source_scenes']) == 2,
                     'POD02 additional source engine scene identities unavailable')
                racer['additional_engine_assets_prepared'] = True
            materials, textures = {}, {}
            for role, basename in (*ROLES, *((('BottomLeftEngine', 'englt2'), ('BottomRightEngine', 'engrt2')) if number == 2 else ())):
                rows = [r for r in local if norm(r['member']) == f'pods/{pod.lower()}/{basename}.psg']
                need(len(rows)==1 and rows[0]['status']=='converted_geometry', 'Missing actual component ' + pod + '/' + basename)
                row=rows[0];native=row['native'];key=native['key']
                component=dict(summarize(row),role=role,native=native,record=store.record(row),
                               rrscene=store.copy(key+'.rrscene',native['scene_sha256']),
                               rrmesh=store.copy(key+'.rrmesh',native['mesh_sha256']))
                racer['components'].append(component)
                for name in native['materials']:
                    member='textures/'+name+'.psm'
                    material, binding=resolve(index,container,member,'converted_material')
                    if material is None and pod == 'POD20' and current_materials is not None:
                        material = current_materials.select(index, container, member)
                    need(material is not None, 'Actual selected material unavailable: '+pod+'/'+name)
                    if pod == 'POD20' and current_materials is not None:
                        need(norm(material['container']) == norm(container),
                             'POD20 material requires an authenticated local member: ' + member)
                    materials[norm(member)]=material
            for name, row in sorted(materials.items()):
                if 'conversion_provenance' in row:
                    record = current_materials.record(store, row)
                    racer['materials'].append(dict(summarize(row), native=row['native'], record=record,
                                                   conversion_provenance=row['conversion_provenance']))
                else:
                    racer['materials'].append(dict(summarize(row),native=row['native'],record=store.record(row)))
                for shader_pass in row['native']['passes']:
                    for slot in shader_pass['textures']:
                        member='textures/'+slot['name']+'.pst'
                        texture, binding=resolve(index,container,member,'converted_texture')
                        if pod == 'POD20' and current_materials is not None:
                            # Keep the PST unresolved; canonical runtime authenticates
                            # the mounted TGA. This does not prove SA specular rendering.
                            original_specmap_secondary = (
                                texture is None and binding.get('status') == 'unresolved' and
                                binding.get('reason') == 'missing_original_member' and
                                binding.get('candidates') == [] and
                                norm(row['member']) in ('textures/20_1.psm', 'textures/20_2.psm') and
                                shader_pass['pass'] == 1 and slot['slot'] == 2 and
                                shader_pass['program'].lower() == 'specularmapdualcompressed' and
                                len(shader_pass['textures']) == 2 and norm(slot['name']) == 'specmap5')
                            need(original_specmap_secondary or
                                 (texture is not None and norm(texture['container']) == norm(container)),
                                 'POD20 active texture requires an authenticated local Full48F conversion: ' + member)
                        binding.update(material_member=row['member'],pass_index=shader_pass['pass'],slot=slot['slot'])
                        if texture:
                            textures[texture['native']['key']]=texture
                            binding['texture_key']=texture['native']['key']
                        racer['texture_bindings'].append(binding)
            for key, row in sorted(textures.items()):
                native=row['native'];mips=[]
                for mip in native['mips']:
                    mips.append(dict(mip,path=store.copy(key+'-mip'+str(mip['index'])+'.rrtex',mip['sha256'])))
                racer['textures'].append(dict(summarize(row),native=native,record=store.record(row),mips=mips,rrtex=mips[-1]['path']))
            for row in local:
                if norm(row['member']).endswith(('.phy','.mcf','.mot','.vut','.slt')):
                    racer['missing_authoring'].append(dict(summarize(row),raw_bytes_available=False))
            racer['summary']={'component_nodes':[r['native']['nodes'] for r in racer['components']],
                              'component_vertices':[r['native']['vertices'] for r in racer['components']],
                              'materials':len(racer['materials']), 'textures':len(racer['textures']),
                              'unresolved_texture_bindings':sum(b['status']!='resolved' for b in racer['texture_bindings'])}
            result['racers'].append(racer)
        if args.audio_root:
            attach_original_names(result, args.audio_root, args.output, identities)
        if args.raw_imports:
            attach_original_authoring(result, args.raw_imports, args.output)
        if current_materials is not None and current_materials.used:
            result['current_material_conversion'] = current_materials.provenance()
            result['limits'].append('POD20 material records explicitly marked current_native_pod20_import come from the separately authenticated current importer report, not historical Full48F conversion.')
        result['files']=store.files
        (args.output/'roster-visual-selection.json').write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps({'output':str(args.output),'racers':[{k:r[k] for k in ('pod_id','summary')}for r in result['racers']], 'files':len(store.files)}))


if __name__=='__main__':
    main()
