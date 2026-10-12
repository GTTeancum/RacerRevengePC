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
        return relative

    def record(self, row):
        native = row['native']
        full = PREFIX + 'objects/' + native['key'] + '.json'
        need(self.counts[full] == 1, 'Missing original metadata')
        data = self.archive.read(full)
        need(json.loads(data) == native, 'Native metadata differs from original manifest')
        return self.copy(native['key'] + '.json', sha(data))


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


def attach_original_names(result, audio_root, output=None):
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
    p = argparse.ArgumentParser()
    p.add_argument('--archive', required=True, type=Path)
    p.add_argument('--output', required=True, type=Path)
    p.add_argument('--audio-root', type=Path)
    p.add_argument('--raw-imports', type=Path)
    p.add_argument('--additional-pods', nargs='+', type=int, choices=[19], default=[])
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
        need(len(set(args.additional_pods)) == len(args.additional_pods), 'Duplicate additional pod')
        for number in (*PODS, *args.additional_pods):
            pod = f'POD{number:02d}'
            container = f'PODS/{pod}/{pod}.RES'
            local = [r for r in manifest['members'] if norm(r['container']) == norm(container)]
            racer = {'pod_id':pod, 'container':container, 'container_sha256':containers[norm(container)]['sha256'],
                     'display_name':None, 'display_name_verified':False, 'components':[], 'materials':[], 'textures':[],
                     'missing_authoring':[], 'texture_bindings':[]}
            materials, textures = {}, {}
            for role, basename in ROLES:
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
                    need(material is not None, 'Actual selected material unavailable: '+pod+'/'+name)
                    materials[norm(member)]=material
            for name, row in sorted(materials.items()):
                racer['materials'].append(dict(summarize(row),native=row['native'],record=store.record(row)))
                for shader_pass in row['native']['passes']:
                    for slot in shader_pass['textures']:
                        member='textures/'+slot['name']+'.pst'
                        texture, binding=resolve(index,container,member,'converted_texture')
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
            attach_original_names(result, args.audio_root, args.output)
        if args.raw_imports:
            attach_original_authoring(result, args.raw_imports, args.output)
        result['files']=store.files
        (args.output/'roster-visual-selection.json').write_text(json.dumps(result,indent=2)+'\n')
        print(json.dumps({'output':str(args.output),'racers':[{k:r[k] for k in ('pod_id','summary')}for r in result['racers']], 'files':len(store.files)}))


if __name__=='__main__':
    main()
