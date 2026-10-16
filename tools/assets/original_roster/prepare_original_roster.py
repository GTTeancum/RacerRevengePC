#!/usr/bin/env python3
"""Prepare a genuine eight-pod field and optional Course asset from own RES files.

The importer runs off the hot path. Every pod uses its own authored scene,
material/texture set and exact PHY bytes. Missing resources cause failure.
"""
import argparse
import configparser
import hashlib
import json
from pathlib import Path
import re
import subprocess


def require(value, why):
    if not value:
        raise ValueError(why)


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def norm(name):
    return name.replace('\\', '/').lower()


def ini(path):
    result = configparser.ConfigParser(interpolation=None, strict=True)
    result.read_string(path.read_bytes().decode('ascii'))
    return result


def strip_quote(value):
    return value.strip().strip('"')


def prepare_pod(importer, resource, out, identities):
    source_hash = sha(resource)
    identity = identities.get(resource.stem.upper())
    if identity:
        require(resource.stat().st_size == identity['resource_bytes'] and
                source_hash == identity['resource_sha256'],
                resource.stem.upper() + ' requires its exact original RES; another pod or UI archive is not a substitute')
    subprocess.run([str(importer), str(resource), str(out)], check=True, stdout=subprocess.DEVNULL)
    require(sha(resource) == source_hash, 'Original resource changed during import')
    report = json.loads((out / 'import.json').read_text())
    report['source_container_sha256'] = sha(resource)
    for row in report['members']:
        row['source_sha256'] = sha(out / row['source_file'])
        for field in ('rrscene', 'rrmesh', 'rrtex'):
            if field in row:
                row[field + '_sha256'] = sha(out / row[field])
    index = {norm(r['source_member']): r for r in report['members']}
    require(len(index) == len(report['members']), 'Duplicate resource names')
    physics = [r for r in report['members'] if r['kind'] == 'phy']
    require(len(physics) == 1, 'Expected one exact pod PHY')
    phy = physics[0]
    if identity:
        require(phy['source_sha256'] == identity['phy_sha256'], 'Original selected PHY identity mismatch')
        for category, digest in identity['speech_sha256'].items():
            key = 'audio/speech/' + resource.stem.lower() + '/' + category + '.ini'
            require(key in index and index[key]['source_sha256'] == digest,
                    'Original selected audio authoring identity mismatch: ' + key)
    require(phy['native_physics_49n_accepted'], 'PHY is outside the native49N reader admission')
    authored = ini(out / phy['source_file'])
    setup = authored['Pod Setup']
    roles = ['Chariot', 'LeftEngine', 'RightEngine']
    bottom = [role in setup for role in ('BottomLeftEngine', 'BottomRightEngine')]
    require(bottom[0] == bottom[1], 'Partial original bottom-engine pair')
    if bottom[0]:
        require(resource.stem.upper() == 'POD02' and strip_quote(setup['EngineMotion']) == '5',
                'Unsupported original bottom-engine topology')
        roles += ['BottomLeftEngine', 'BottomRightEngine']
    prefix = norm(phy['source_member']).rsplit('/', 1)[0] + '/'
    pod = {'pod_id': resource.stem.upper(), 'name': strip_quote(setup['Name']),
           'resource_sha256': report['source_container_sha256'],
           'resource_file': str(resource), 'phy': phy, 'components': [],
           'materials': [], 'textures': [], 'unresolved_texture_bindings': [], 'original_asset_inputs_available': True}
    materials, textures = {}, {}
    for role in roles:
        stem = Path(norm(strip_quote(setup[role]))).stem
        member = prefix + stem + '.psg'
        require(member in index, 'Missing authored component ' + member)
        row = index[member]
        require(row['status'] == 'converted_geometry', 'Unsupported authored component ' + member)
        pod['components'].append(dict(row, role=role))
        for material in row['materials']:
            key = 'textures/' + norm(material) + '.psm'
            require(key in index, 'Missing local material ' + key + '; global substitution is forbidden')
            mat = index[key]
            require(mat['status'] == 'converted_material', 'Unsupported original material ' + key)
            materials[key] = mat
            for shader_pass in mat['passes']:
                for slot in shader_pass['textures']:
                    key = 'textures/' + norm(slot['name']) + '.pst'
                    if key not in index or index[key]['status'] != 'converted_texture':
                        pod['unresolved_texture_bindings'].append({'material': mat['source_member'], 'pass': shader_pass['pass'], 'slot': slot['slot'], 'texture': key})
                    else:
                        textures[key] = index[key]
    pod['materials'] = list(materials.values())
    pod['textures'] = list(textures.values())
    (out / 'import.json').write_text(json.dumps(report, indent=2) + '\n')
    (out / 'pod.json').write_text(json.dumps(pod, indent=2) + '\n')
    return pod


def main():
    catalog = json.loads(Path(__file__).with_name('additional_pod_identities.json').read_text())
    require(catalog.get('schema') == 'RR_ADDITIONAL_POD_IDENTITIES_V1', 'Additional identity catalog schema mismatch')
    identities = catalog['pods']
    parser = argparse.ArgumentParser()
    parser.add_argument('--importer', required=True, type=Path)
    parser.add_argument('--disc', required=True, type=Path)
    parser.add_argument('--pods', nargs='+', default=['POD%02d' % i for i in (1, 3, 4, 7, 8, 9, 10, 11)])
    parser.add_argument('--additional-pods', nargs='+', choices=list(identities), default=[],
                        help='Prepare own Course-selected resources without enlarging the eight-racer field')
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--expected-visual-selection', type=Path)
    args = parser.parse_args()
    args.pods = [p.upper() for p in args.pods]
    require(not args.output.exists(), 'Output must be new; source/cache files are never overwritten')
    require(len(set(args.pods)) == len(args.pods) and args.pods[0].upper() == 'POD01', 'Player must be POD01 and opponents distinct')
    require(all(re.fullmatch(r'POD\d{2}', p.upper()) for p in args.pods), 'Invalid POD identifier')
    selected_pods = args.pods + args.additional_pods
    require(len(set(selected_pods)) == len(selected_pods), 'Duplicate field/additional POD identifier')
    source_index = {norm(str(p.relative_to(args.disc))): p for p in args.disc.rglob('*') if p.is_file()}
    resources = []
    for pod in selected_pods:
        expected = f'pods/{pod.lower()}/{pod.lower()}.res'
        require(expected in source_index, 'Missing retained original resource: ' + expected)
        resource = source_index[expected]
        identity = identities.get(pod)
        if identity:
            require(resource.stat().st_size == identity['resource_bytes'] and
                    sha(resource) == identity['resource_sha256'], 'Original resource identity mismatch: ' + expected)
        resources.append(resource)
    expected = {}
    if args.expected_visual_selection:
        expected = {r['pod_id']: r for r in json.loads(args.expected_visual_selection.read_text())['racers']}
    args.output.mkdir(parents=True)
    roster, additional = [], []
    for resource in resources:
        directory = resource.stem.lower()
        pod = prepare_pod(args.importer.resolve(), resource.resolve(), args.output / directory, identities)
        if expected:
            reference = expected[pod['pod_id']]
            require(pod['resource_sha256'] == reference['container_sha256'], 'Original container differs from authenticated visual source')
            for actual, old in zip(pod['components'], reference['components']):
                require(actual['role'] == old['role'] and actual['source_sha256'] == old['source_sha256'], 'Original component identity mismatch')
                require(actual['rrscene_sha256'] == old['native']['scene_sha256'] and actual['rrmesh_sha256'] == old['native']['mesh_sha256'], 'Fresh native scene differs from authenticated cache')
            physics = [r for r in reference['missing_authoring'] if norm(r['member']).endswith('.phy')]
            require(len(physics) == 1 and pod['phy']['source_sha256'] == physics[0]['source_sha256'], 'Original PHY identity mismatch')
        rows = additional if pod['pod_id'] in args.additional_pods else roster
        rows.append({'pod_id': pod['pod_id'], 'name': pod['name'], 'path': directory + '/pod.json',
                       'resource_sha256': pod['resource_sha256'], 'phy_sha256': pod['phy']['source_sha256'],
                       'component_node_counts': [r['nodes'] for r in pod['components']],
                       'component_vertex_counts': [r['vertices'] for r in pod['components']]})
    require(len({p['name'] for p in roster}) == len(roster), 'Duplicate racer identities')
    result = {'schema': 'RR_ORIGINAL_ROSTER_V1', 'player_index': 0, 'racers': roster,
              'complete_eight_racer_field': len(roster) == 8,
              'boundaries': ['Original component scenes retain independent topology and source material assignments.',
                             'The runtime must animate each pod with its own original rig and motion resources.',
                             'No original gameplay equivalence is implied merely by importing its assets.']}
    if additional:
        result['additional_racers'] = additional
    (args.output / 'roster.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
