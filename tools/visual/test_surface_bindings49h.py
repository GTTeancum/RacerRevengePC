#!/usr/bin/env python3
"""Focused valid-data, association and scalar-mutation controls for 49H export.

Only new test-owned copies of the selected visual closure are mutated. No
collision payload, ownership, pointer or allocation-failure tests are used.
"""
import argparse
from copy import deepcopy
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile


def sha(blob):
    return hashlib.sha256(blob).hexdigest()


def encode(value):
    return (json.dumps(value, indent=2, sort_keys=True, allow_nan=False) + '\n').encode()


def canonical(value):
    return json.dumps(value, sort_keys=True, separators=(',', ':'), allow_nan=False).encode()


def require(ok, reason):
    if not ok:
        raise AssertionError(reason)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    source = root / 'evidence/visual49G'
    binary = args.binary.resolve()
    out = args.output_dir.resolve()
    require(not out.exists(), 'Use a new test output directory')
    out.mkdir(parents=True)
    selection_blob = (source / 'asset-selection.json').read_bytes()
    report_blob = (source / 'export-placed-normal/export-report.json').read_bytes()
    selection = json.loads(selection_blob)
    report = json.loads(report_blob)
    oracle = json.loads((root / 'evidence/visual49H/texture-assessment.json').read_bytes())
    original_hashes = {row['path']: sha((source / row['path']).read_bytes()) for row in selection['objects']}
    original_hashes.update({name: sha((source / name).read_bytes()) for name in
                            ('asset-selection.json', 'export-placed-normal/export-report.json',
                             'export-placed-normal/ta-world.rrmesh', 'export-placed-normal/pod01-placed.rrmesh')})
    rows = []
    # LeakSanitizer is unsupported under this executor's ptrace environment;
    # leak/ownership diagnostics are also outside the explicitly scoped tests.
    env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1',
               UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')

    def run(label, s_path, r_path, destination, expect_success, expected_error=None):
        command = [str(binary), '--selection', str(s_path), '--export-report', str(r_path), '--output', str(destination)]
        process = subprocess.run(command, capture_output=True, text=True, env=env, timeout=60)
        (out / (label + '.stdout')).write_text(process.stdout)
        (out / (label + '.stderr')).write_text(process.stderr)
        require(process.returncode == (0 if expect_success else 1), label + ': unexpected return code ' + str(process.returncode))
        require('AddressSanitizer' not in process.stderr and 'runtime error:' not in process.stderr,
                label + ': sanitizer diagnostic')
        if expect_success:
            value = json.loads(destination.read_bytes())
            stdout = json.loads(process.stdout)
            require(stdout['sha256'] == sha(destination.read_bytes()), label + ': output hash independently checked')
        elif label != 'existing_output':
            require(not destination.exists(), label + ': rejected case wrote output')
            value = None
        else:
            value = None
        if expected_error:
            require(expected_error in process.stderr, label + ': unexpected rejection: ' + process.stderr)
        rows.append({'name': label, 'exit_code': process.returncode, 'expected': 'accepted' if expect_success else 'rejected',
                     'expected_error': expected_error, 'status': 'PASS'})
        return value

    valid = run('valid_original', source / 'asset-selection.json', source / 'export-placed-normal/export-report.json',
                out / 'valid.json', True)
    require(valid['summary']['total'] == oracle['coverage']['combined']['total'], 'Oracle total coverage')
    require(valid['summary']['textured'] == oracle['coverage']['combined']['textured'], 'Oracle textured coverage')
    require(valid['summary']['fallback'] == oracle['coverage']['combined']['neutral_fallback'], 'Oracle fallback coverage')
    require(valid['summary']['texture_upload_bytes'] == oracle['cost']['texture_upload_payload_bytes'], 'Oracle texture payload')
    for actual, expected in zip(valid['spans'], oracle['ordered_surfaces']):
        pairs = [('draw_order', 'draw_order'), ('mesh_id', 'group'), ('index_start', 'index_start'),
                 ('index_count', 'indices'), ('vertex_start', 'vertex_start'), ('vertex_count', 'vertices'),
                 ('scene_key', 'scene_key'), ('scene_member', 'scene_member'), ('material_index', 'material_index'),
                 ('material', 'material'), ('status', 'status'), ('surface_index', 'surface_index')]
        for a, b in pairs:
            require(actual[a] == expected[b], 'Independent ordered span oracle: ' + a)
        require(actual['texture_id'] == expected['binding']['texture_key'], 'Independent texture key oracle')
        require(actual['pass'] == 1 and actual['slot'] == 1, 'Exact first pass/slot')
        require(actual['fallback_rgba'] == ([.57,.63,.68,1] if actual['mesh_id'] == 'TA' else [.97,.58,.15,1]), '49G fallback palette')
    for texture in valid['textures']:
        path = (out / texture['path']).resolve()
        blob = path.read_bytes()
        require(sha(blob) == texture['sha256'] and len(blob) == texture['rgba_bytes'] + 16,
                'Independent native texture hash/bytes')
    for m in valid['meshes']:
        require(sha((out / m['path']).read_bytes()) == m['sha256'], 'Independent native mesh hash')
    before = sha((out / 'valid.json').read_bytes())
    run('existing_output', source / 'asset-selection.json', source / 'export-placed-normal/export-report.json',
        out / 'valid.json', False, 'Output must be new')
    require(sha((out / 'valid.json').read_bytes()) == before, 'Existing output left unchanged')

    with tempfile.TemporaryDirectory(prefix='binding49h-', dir=out) as temporary:
        fixture = Path(temporary)
        selected_root = fixture / 'selected'
        selected_root.mkdir()
        cache = selected_root / 'cacheobjects'
        cache.mkdir()
        for row in selection['objects']:
            shutil.copyfile(source / row['path'], selected_root / row['path'])
        exports = fixture / 'exports'
        exports.mkdir()
        for name in ('ta-world.rrmesh', 'pod01-placed.rrmesh'):
            shutil.copyfile(source / 'export-placed-normal' / name, exports / name)
        destinations = fixture / 'outputs'
        destinations.mkdir()
        s_path, r_path = selected_root / 'selection.json', exports / 'report.json'
        s_path.write_bytes(selection_blob)
        r_path.write_bytes(report_blob)
        run('valid_copied', s_path, r_path, destinations / 'copied.json', True)
        run('immutable_output_directory', s_path, r_path, selected_root / 'blocked.json', False,
            'Output must not be in immutable source directory')

        def case(label, mutate_selection=None, mutate_report=None, mutate_files=None, expected_error=None):
            s, r, restored = deepcopy(selection), deepcopy(report), []
            if mutate_selection:
                mutate_selection(s)
            if mutate_report:
                mutate_report(r)
            if mutate_files:
                mutate_files(s, r, restored)
            s_path.write_bytes(encode(s))
            r_path.write_bytes(encode(r))
            try:
                run(label, s_path, r_path, destinations / (label + '.json'), False, expected_error)
            finally:
                for path, content in restored:
                    path.write_bytes(content)

        case('wrong_selection_schema', lambda s: s.update(schema='unknown'), expected_error='Input schema/status mismatch')
        case('selected_hash_mismatch', lambda s: s['objects'][0].update(sha256='0'*64), expected_error='Selected object hash/extent mismatch')
        case('unsafe_selected_path', lambda s: s['objects'][0].update(path='../outside.json'), expected_error='Nonvisual or unsafe selected path')
        case('duplicate_selected_object', lambda s: s['objects'].__setitem__(1, deepcopy(s['objects'][0])), expected_error='Duplicate selected object/hash')
        case('aggregate_selected_byte_limit', lambda s: s['objects'][0].update(bytes=33*1024*1024), expected_error='Selected aggregate byte limit')
        case('wrong_material_name', lambda s: s['material_resolution'][0].update(name='wrong'), expected_error='Resolution requested name/type mismatch')
        case('wrong_texture_name', lambda s: s['texture_resolution'][0].update(name='wrong'), expected_error='Resolution requested name/type mismatch')
        case('wrong_local_scope', lambda s: s['texture_resolution'][0].update(scope='global'), expected_error='Resolution scope/count mismatch')
        case('cross_container_candidate', lambda s: s['texture_resolution'][0]['candidates'][0].update(container='TRACKS/OTHER.RES'), expected_error='Local candidate container mismatch')
        case('invented_fallback', lambda s: s['texture_resolution'][0].update(status='unresolved', reason='no_exact_member'), expected_error='Unproven neutral fallback reason')
        case('duplicate_texture_association', lambda s: s['texture_resolution'].append(deepcopy(s['texture_resolution'][0])), expected_error='Duplicate texture association')
        case('missing_first_texture_association', lambda s: s['texture_resolution'].pop(0), expected_error='Missing selected first-pass texture association')
        case('wrong_texture_requester', lambda s: s['texture_resolution'][0]['requested_by'][0].update(material_key='0'*64), expected_error='Texture association requester mismatch')
        case('surface_material_mutation', mutate_report=lambda r: r['world_instances'][0]['surfaces'][0].update(material='ta_sand8'), expected_error='TA reported material/span mismatch')
        case('surface_index_start_mutation', mutate_report=lambda r: r['world_instances'][0]['surfaces'][0].update(index_start=3), expected_error='TA reported material/span mismatch')
        case('negative_instance_offset', mutate_report=lambda r: r['world_instances'][0].update(vertex_start=-1), expected_error='Invalid scalar: vertex_start')
        case('pod_part_order', mutate_report=lambda r: r['pod_parts'].reverse(), expected_error='Original Pod part order mismatch')
        case('pod_offset_mutation', mutate_report=lambda r: r['pod_parts'][1].update(index_start=4995), expected_error='Instance offset discontinuity')
        case('placement_matrix_mutation', mutate_report=lambda r: r['world_instances'][0]['matrix'].__setitem__(12, 1), expected_error='49G instance differs from selected placement')

        def mesh_edit(offset, fmt, transform, truncate=False):
            def mutate(s, r, restore):
                path = exports / 'ta-world.rrmesh'
                old = path.read_bytes()
                restore.append((path, old))
                if truncate:
                    path.write_bytes(old[:-1])
                else:
                    blob = bytearray(old)
                    position = offset(old) if callable(offset) else offset
                    value = struct.unpack_from(fmt, blob, position)[0]
                    struct.pack_into(fmt, blob, position, transform(value))
                    path.write_bytes(blob)
            return mutate
        case('mesh_uv_mutation', mutate_files=mesh_edit(28, '<f', lambda v: v+.125), expected_error='UV association parity mismatch')
        case('mesh_color_mutation', mutate_files=mesh_edit(36, '<B', lambda v: v ^ 1), expected_error='Source color parity mismatch')
        case('mesh_position_mutation', mutate_files=mesh_edit(16, '<f', lambda v: v+1), expected_error='49G position parity mismatch')
        case('mesh_index_mutation', mutate_files=mesh_edit(lambda b: 16+struct.unpack_from('<I', b, 8)[0]*24, '<I', lambda v: v+1), expected_error='Ordered surface index parity mismatch')
        case('mesh_truncated', mutate_files=mesh_edit(0, '<I', lambda v:v, True), expected_error='Placed mesh parse')

        def record_edit(transform):
            def mutate(s, r, restore):
                m = s['material_resolution'][0]
                path = selected_root / m['record']
                original = path.read_bytes()
                restore.append((path, original))
                value = json.loads(original)
                transform(value)
                value.pop('record_sha256')
                value['record_sha256'] = sha(canonical(value))
                blob = encode(value)
                path.write_bytes(blob)
                row = next(x for x in s['objects'] if x['path'] == m['record'])
                row.update(sha256=sha(blob), bytes=len(blob))
            return mutate
        case('duplicate_material_slot', mutate_files=record_edit(lambda r:r['passes'][0]['textures'][1].update(slot=1)), expected_error='Material texture slot/name ordering')
        case('wrong_material_firstpass', mutate_files=record_edit(lambda r:r['passes'][0].update(**{'pass':2})), expected_error='Material pass/program ordering')
        case('material_texture_association_mutation', mutate_files=record_edit(lambda r:r['passes'][0]['textures'][0].update(name='01_2')), expected_error='Texture association requester mismatch')
        case('unsafe_material_texture_name', mutate_files=record_edit(lambda r:r['passes'][0]['textures'][0].update(name='../bad')), expected_error='Material texture slot/name ordering')

        def texture_header(s, r, restore):
            selected_texture = s['texture_resolution'][0]
            mip = selected_texture['native']['mips'][-1]
            rel = 'cacheobjects/' + selected_texture['key'] + '-mip' + str(mip['index']) + '.rrtex'
            path = selected_root / rel
            original = path.read_bytes()
            restore.append((path, original))
            blob = bytearray(original)
            struct.pack_into('<I', blob, 8, 0)
            path.write_bytes(blob)
            entry = next(x for x in s['objects'] if x['path'] == rel)
            entry['sha256'] = sha(blob)
        case('texture_zero_width', mutate_files=texture_header, expected_error='Native texture parse')

    after = {rel: sha((source / rel).read_bytes()) for rel in original_hashes}
    require(original_hashes == after, 'All immutable 49G assets/reports unchanged')
    result = {'schema': 'texture49h.binding-tests.v1', 'status': 'PASS', 'checks': len(rows),
              'cases': rows, 'binary_sha256': sha(binary.read_bytes()),
              'independent_oracle': 'evidence/visual49H/texture-assessment.json',
              'independent_ordered_spans_compared': len(valid['spans']),
              'immutable_files_hashed_before_and_after': len(original_hashes),
              'mutable_test_copies_removed': True, 'collision_payload_reads': 0,
              'leak_sanitizer': 'not run; unsupported under executor ptrace and outside test scope',
              'scope': 'Valid selected data and bounded malformed scalar/schema/association controls only; no pointer, ownership or allocation-failure tests.'}
    (out / 'report.json').write_bytes(encode(result))
    print(json.dumps({'status': result['status'], 'checks': result['checks'], 'report': str(out / 'report.json')}))


if __name__ == '__main__':
    main()
