#!/usr/bin/env python3
"""Authenticated RA/RC cold assembly, source-only until root review/execution.

Consumes exact original intake and an explicitly reviewed native import report.
Never builds a converter, executes guest code, launches gameplay, or substitutes
other-track assets. Unsupported present geometry/material/texture is fatal.
"""
import argparse
import collections
import hashlib
import io
import json
import math
import re
from pathlib import Path
import stat
import struct
import sys
import zipfile
sys.dont_write_bytecode = True

FULL_SHA = '60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c'
GENERATION = '3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39'
# Original RA source pins, not converted pixel identities. Only the reviewed
# native importer may produce the corresponding sequential mip chain.
RA_TGA_SOURCES = {
 'TEXTURES/LARGEFLAG.TGA': (128, 32, '352286ac266f59e3d06531cfab7a67ce8ad9e7355fe544ba932550278abd613d'),
 'TEXTURES/METALGRATE.TGA': (256, 24, '20cd5335e1d870295983807467defecbf2c45dd1ee1ceeaa10c8bfd2e8e85fa9'),
 'TEXTURES/TWIDESIGNS02.TGA': (128, 24, '693031b2ed08b5437c250ea875eb9e8f6f0444b858530d7a0e0c1737630aba15'),
}
PROFILES = {
 'RA': dict(archive_bytes=4604561, archive_entries=1010, zones=10, portals=10,
  instances=1261, missing_geometry=176, collision_instances=178,
  archive_sha256='1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2',
  reference_sha256='42520c6334dfa53b79f75ae2db855432d23b49a82fc524301f3c717a20d3924d',
  scb=dict(name='tracks\\ra.scb'), cub=dict(name='skies\\ra.cub'),
  sky_palette_count=226, sky_tile_descriptor=[0,32,0,16,4096,4,0,0,0,0,0]),
 'RC': dict(archive_bytes=4655141, archive_entries=1121, zones=15, portals=15,
  instances=1277, missing_geometry=107, collision_instances=112,
  archive_sha256='ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b',
  reference_sha256='01ef7eb45385a33a98a1e265108c7c52e1deecda40ddeccb1ceca0f81252a1ba',
  scb=dict(name='tracks\\rc.scb'), cub=dict(name='skies\\rcSky.cub'),
  sky_palette_count=126, sky_tile_descriptor=[0,32,0,16,4096,4,0,0,0,0,0]),
}
DEPENDENCIES = {
 'ma_sa_asset_sources.py':'3e4d9ba4509ca8e5224c7d4cb6862e6b6d669dcaed0e81ebca4861abc253c164',
 'assemble_ga_visual.py':'714cf1b8bb31850c94e4241bd05cc302fbf4c9b889ff701e22a4f8393b68f835',
 'decode_world48e.py':'f073107368ff6b5f5e398e74126834d585e8c8f965bb3e87718f5d9a7a166f21',
 'decode_source_textures50fc.py':'5f9af6eba0f07732f913cde7aa40e246d218d38230f57a7aacc027afd25a7c49',
 'recover_ma_sa_sky.py':'317e8722afd4471f2c46288d8f52c0875dd2877655bb0af06bfbbf61f87f0399',
 'decode_static48b.py':'875946365699396aaa0dd594a8e2b9cf1c4319cfc6079c53d686db98b998668a',
 'decode_scene48c.py':'e0b6811443c46cd05c621e1915134a031a457b1a66db18751e2956ed7d68af1a',
}

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
         'Unsafe relative file path: '+repr(name))
    return Path(name)

def read(path, maximum=8*1024*1024):
    need(stat.S_ISREG(path.lstat().st_mode) and not path.is_symlink() and
         not any(p.is_symlink() for p in path.parents), 'Expected nonsymlink file: '+str(path))
    need(path.stat().st_size <= maximum, 'Input byte bound: '+str(path))
    return path.read_bytes()

def canonical(value):
    return (json.dumps(value, sort_keys=True, indent=2, ensure_ascii=True, allow_nan=False)+'\n').encode('ascii')

class Sources:
    def __init__(self, intake, track, full48f):
        self.intake, self.cfg = intake, dict(PROFILES[track])
        cfg = self.cfg
        self.archive = read(intake/'original'/(track+'.RES'))
        need(len(self.archive) == cfg['archive_bytes'] and sha(self.archive) == cfg['archive_sha256'],
             'Exact original archive identity')
        full = read(full48f, 300*1024*1024)
        need(sha(full) == FULL_SHA, 'Exact retained Full48F identity')
        with zipfile.ZipFile(io.BytesIO(full)) as archive:
            def member(name, maximum):
                rows = [r for r in archive.infolist() if r.filename == name]
                need(len(rows) == 1 and not rows[0].is_dir() and rows[0].file_size <= maximum,
                     'Missing/ambiguous/oversized retained file '+name)
                return archive.read(rows[0])
            gb = member('evidence/importFull48F-20261003/native-cache/generations/'+GENERATION+'.json', 64*1024*1024)
            need(sha(gb) == GENERATION, 'Retained generation identity')
            generation = json.loads(gb)
            reference = member('evidence/worldImport48F/'+track.lower()+'-tracks_'+track.lower()+'.reference.json', 8*1024*1024)
        need(sha(reference) == cfg['reference_sha256'] and
             read(intake/'retained48F-world.reference.json') == reference,
             'Exact original world reference identity')
        containers = [r for r in generation['containers'] if r['name'] == 'TRACKS/'+track+'.RES']
        need(len(containers) == 1 and containers[0]['sha256'] == cfg['archive_sha256'] and
             containers[0]['bytes'] == cfg['archive_bytes'] and containers[0]['entries'] == cfg['archive_entries'],
             'Retained original container extent')
        rows = [dict(name=r['member'], path=r['member'].replace('\\','/'), bytes=r['source_bytes'],
                     logical_offset=r['logical_offset'], sha256=r['source_sha256'],
                     **{'48F_status':r['status']}) for r in generation['members']
                if r['container'] == 'TRACKS/'+track+'.RES']
        manifest = canonical(dict(schema='RR_ORIGINAL_RYLOTH_MEMBER_INVENTORY_V1',track=track,
            archive_sha256=cfg['archive_sha256'],archive_bytes=cfg['archive_bytes'],
            source_archive_sha256=FULL_SHA,source_generation_sha256=GENERATION,members=rows))
        need(read(intake/'manifest.json') == manifest and len(rows) == cfg['archive_entries'],
             'Bridge inventory differs from exact retained generation')
        complete = json.loads(read(intake/'INTAKE-COMPLETE.json'))
        need(complete['status'] == 'authenticated_ryloth_intake_v1' and complete['track'] == track and
             complete['manifest_sha256'] == sha(manifest) and complete['original_archive_sha256'] == cfg['archive_sha256'] and
             complete['reference_sha256'] == cfg['reference_sha256'] and complete['source_archive_sha256'] == FULL_SHA and
             complete['source_generation_sha256'] == GENERATION, 'Completed original intake provenance')
        self.members = {}
        for row in rows:
            key = norm(row['name']); safe(row['path'])
            need(key not in self.members and norm(row['path']) == key, 'Duplicate original member')
            self.members[key] = row
        # Authenticate the complete intake before writing any native outputs.
        for row in rows:
            self.read(row['name'])
        cfg['scb'] = self.members[norm(cfg['scb']['name'])]
        cfg['cub'] = self.members[norm(cfg['cub']['name'])]
        cfg['atm_members'] = [r for r in rows if norm(r['name']).endswith('.ATM')]

    def read(self, name):
        row = self.members[norm(name)]
        data = read(self.intake/'members'/safe(row['path']))
        need(len(data) == row['bytes'] and sha(data) == row['sha256'], 'Original member identity: '+row['name'])
        return data


def original_initial_phase(text, member):
    # Same bounded decimal subset as originalUiNumber50KG and
    # parsePodPhysicsNumber49N: decimal -> binary64 -> binary32.
    # Do not let Python accept underscores, NaN/Inf, exponent or junk suffixes.
    need(isinstance(text, str) and 0 < len(text) <= 11,
         'Unsupported original ATM initial-frame syntax '+member)
    sign = 1 if text[0] in '+-' else 0
    if text[sign:sign+1] == '.':
        need(len(text) <= 10, 'Unsupported original ATM initial-frame syntax '+member)
        text = text[:sign]+'0'+text[sign:]
    match = re.fullmatch(r'([+-]?)([0-9]+)(?:\.([0-9]+))?', text)
    need(match is not None, 'Unsupported original ATM initial-frame syntax '+member)
    integral, fractional = match.group(2), match.group(3) or ''
    need(len(integral)+len(fractional) <= 9 and len(fractional) <= 6,
         'Original ATM initial-frame decimal outside bounded native subset '+member)
    numerator, denominator = int(integral+fractional), 10**len(fractional)
    # The retained native decimal parser produces +0 even for authored -0.
    value = (-numerator if match.group(1) == '-' else numerator)/denominator
    return struct.unpack('<f', struct.pack('<f', value))[0] if numerator else 0.0


def animation_bindings(imported, name, frame_count):
    need(1 <= frame_count <= 64, 'Original ATM frame count outside native subset '+name)
    result = []
    for member, material in sorted(imported.items()):
        if material['status'] != 'converted_material':
            continue
        for material_pass in material['passes']:
            if not any(norm(t['name']) == norm(name) for t in material_pass['textures']):
                continue
            fields = {f['name']:f['value'] for f in material_pass['fields']['authored']}
            initial_fields = [f for f in material_pass['fields']['authored']
                              if f['name'].lower() == 'animatedmapinitialframe']
            need(len(initial_fields) <= 1, 'Ambiguous original ATM initial frame '+member)
            # Only genuine omission inherits 29B950's float initialFrame=0.
            # Empty/unspecified declarations stay outside this narrow admission.
            if not initial_fields:
                need(not any(f['name'].lower() == 'animatedmapinitialframe'
                             for f in material_pass['fields']['unspecified']),
                     'Empty ATM initial frame requires separate source admission '+member)
            phase = original_initial_phase(initial_fields[0]['value'], member) if initial_fields else 0.0
            need(math.isfinite(phase) and 0 <= phase < frame_count,
                 'Original ATM initial phase outside exact source frame count '+member)
            # 29AFDC/29B028 selects by truncation; the native phase is NOT an int.
            initial = math.trunc(phase)
            # Existing generated animation rows store initialFrame as uint32.
            # Do not erase a valid fractional phase to fit that runtime schema.
            need(phase == float(initial),
                 'Fractional ATM initial phase needs native phase-preserving rows '+member)
            need('animatedMapFramesPerSecond' in fields,
                 'Missing explicit original ATM controls '+member)
            fps = float(fields['animatedMapFramesPerSecond'])
            need(math.isfinite(fps) and 0 < fps <= 60, 'Unsupported ATM timing '+member)
            result.append(dict(material_member=member,material_pass=material_pass['pass'],
                               initial_frame=initial,initial_phase=phase,
                               initial_frame_provenance=dict(
                                   source='authored' if initial_fields else 'original_constructor_default',
                                   source_pc='0x29B950',
                                   authored_field=initial_fields[0] if initial_fields else None,
                                   selection='binary32 phase truncated toward zero at 29AFDC/29B028'),
                               fps=fps,authored=material_pass['fields']))
    need(result, 'ATM has no converted original material user: '+name)
    return result

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--track', choices=('RA', 'RC'), required=True)
    parser.add_argument('--intake', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    parser.add_argument('--full48f', type=Path, required=True)
    parser.add_argument('--native-import', type=Path, required=True)
    parser.add_argument('--import-report-sha256', required=True, help='Root-reviewed import.json identity')
    parser.add_argument('--native-output-index-sha256', required=True, help='Root-reviewed native-output-index.json identity')
    parser.add_argument('--execute-preparation', action='store_true', required=True)
    args = parser.parse_args()
    requested = args.output_root.absolute()
    need('..' not in requested.parts, 'Output root cannot contain parent traversal')
    need(not requested.exists() and not requested.is_symlink(), 'Output root must be new')
    need(requested.parent.is_dir() and not any(p.is_symlink() for p in requested.parents),
         'Output parent must be a real existing nonsymlink directory')
    # Canonicalize the existing parent, then use that exact root for every write.
    # Path.absolute() alone retains '..' and is insufficient for containment.
    output_root = requested.parent.resolve(strict=True)/requested.name
    need(not output_root.exists() and not output_root.is_symlink(), 'Canonical output root must be new')
    repo, intake, native = args.repo.resolve(strict=True), args.intake.resolve(strict=True), args.native_import.resolve(strict=True)
    need(all(root != output_root and root not in output_root.parents for root in (repo,intake,native)), 'Output must be outside inputs/repository')
    args.output_root = output_root
    for name, digest in DEPENDENCIES.items():
        need(sha(read(repo/'tools/assets'/name, 16*1024*1024)) == digest, 'Reviewed assembly dependency differs: '+name)
    sys.path.insert(0, str(args.repo/'tools/assets'))
    from assemble_ga_visual import jb, scene_native
    from decode_world48e import decode_world
    from decode_source_textures50fc import parse_resource, decode_indexed, decode_ipu
    originals = Sources(args.intake, args.track, args.full48f)
    cfg, track = originals.cfg, args.track
    container, archive_sha, scene_sha = 'TRACKS/'+track+'.RES', cfg['archive_sha256'], cfg['scb']['sha256']
    archive = originals.archive
    scb = originals.read(cfg['scb']['name'])
    world = decode_world(scb)
    need((len(world['zones']), len(world['portals']), len(world['instances'])) ==
         (cfg['zones'], cfg['portals'], cfg['instances']), 'Original world extent')
    reference_bytes = (args.intake/'retained48F-world.reference.json').read_bytes()
    need(sha(reference_bytes) == cfg['reference_sha256'], 'Reviewed retained world identity')
    # The native-source decoder returns tuples for vectors/matrices; JSON
    # represents those exact sequences as arrays. Compare the same JSON domain
    # without dropping fields, rounding numbers, or changing source placements.
    need(json.loads(json.dumps(world, allow_nan=False)) == json.loads(reference_bytes),
         'Current world decoder differs from retained original placements')
    inventory_index = originals.members
    native_root = args.native_import
    import_bytes = read(native_root/'import.json', 16*1024*1024)
    need(sha(import_bytes) == args.import_report_sha256, 'Root-reviewed native import report identity')
    importer = json.loads(import_bytes)
    native_index_bytes = read(native_root/'native-output-index.json', 16*1024*1024)
    need(sha(native_index_bytes) == args.native_output_index_sha256, 'Root-reviewed native output index identity')
    native_index = json.loads(native_index_bytes)
    need(native_index['schema'] == 'RR_REVIEWED_RYLOTH_NATIVE_OUTPUTS_V1' and native_index['track'] == track and native_index['archive_sha256'] == archive_sha and native_index['import_report_sha256'] == sha(import_bytes), 'Reviewed native output provenance')
    need(len(native_index['files']) <= 20000, 'Native output index entry bound')
    native_files, total_native_bytes = {}, 0
    for entry in native_index['files']:
        relative = safe(entry['path'])
        need(entry['path'] not in native_files, 'Duplicate native output index entry')
        blob = read(native_root/relative, 64*1024*1024)
        need(len(blob) == entry['bytes'] and sha(blob) == entry['sha256'], 'Reviewed native artifact differs: '+entry['path'])
        total_native_bytes += len(blob)
        need(total_native_bytes <= 512*1024*1024, 'Reviewed native output aggregate bound')
        native_files[entry['path']] = entry
    need(importer['schema'] == 'RR_ORIGINAL_POD_IMPORT_V1' and norm(importer['source_container']) == track+'.RES' and importer['source_container_bytes'] == cfg['archive_bytes'], 'Original native import container')
    imported = {}
    for row in importer['members']:
        key = norm(row['source_member'])
        need(key not in imported and key in inventory_index, 'Native import duplicate/foreign original member')
        expected = inventory_index[key]
        need(row['logical_offset'] == expected['logical_offset'] and row['source_bytes'] == expected['bytes'] and read(native_root/safe(row['source_file'])) == originals.read(key), 'Native import source differs: '+key)
        for field in ('source_file','rrscene','rrmesh','rrtex'):
            if field in row:
                need(row[field] in native_files, 'Unreviewed native artifact: '+row[field])
        for mip in row.get('native_mips',[]):
            need(mip['rrtex'] in native_files, 'Unreviewed native texture mip')
        imported[key] = row
    output = args.output_root/('evidence/visual'+track)
    args.output_root.mkdir(mode=0o700)
    (args.output_root/'tests/native').mkdir(parents=True)
    output.mkdir(parents=True, exist_ok=True)
    objects, scene_requests, scenes, visuals, nondrawn = {}, {}, {}, [], []

    def source(member):
        return originals.read(member)

    def store(name, data, role):
        relative = 'cacheobjects/'+name
        target = output/relative
        target.parent.mkdir(parents=True, exist_ok=True)
        if target.exists():
            need(target.read_bytes() == data, 'Conflicting staged native object '+relative)
        else:
            target.write_bytes(data)
        objects[relative] = dict(path=relative, bytes=len(data), sha256=sha(data), extracted=True, role=role)
        return relative

    def record(fields):
        fields = dict(fields)
        fields['key'] = sha((fields['converter']+'\n'+fields['source_sha256']).encode())
        fields['record_sha256'] = sha(json.dumps(fields, sort_keys=True, separators=(',', ':'), ensure_ascii=True, allow_nan=False).encode())
        store(fields['key']+'.json', jb(fields), 'native_record')
        return fields

    def candidate(member, status, native=None):
        row = inventory_index[norm(member)]
        out = dict(container=container, member=row['name'], logical_offset=row['logical_offset'], source_bytes=row['bytes'], source_sha256=row['sha256'], status=status)
        if native:
            out['key'] = native['key']
        return out

    vertices, indices, placed = bytearray(), [], []
    vertex_count = 0
    for wi, instance in enumerate(world['instances']):
        member = norm(instance['geometry_member'])
        base = {k: instance[k] for k in ('section', 'geometry_member', 'zone', 'ordinal', 'source_block_offset', 'position', 'look', 'up', 'matrix', 'use_lighting')}
        base.update(world_instance_index=wi, container=container)
        if member not in inventory_index:
            nondrawn.append(dict(base, status='non_drawn', reason='no_exact_PSG_member', collision_member=instance['collision_member'], collision_member_present=norm(instance['collision_member']) in inventory_index))
            continue
        need(member in imported and imported[member]['status'] == 'converted_geometry',
             'Referenced existing source geometry unsupported '+member+': '+json.dumps(imported.get(member, {}), sort_keys=True))
        native = imported[member]
        if member not in scenes:
            original_bytes = source(member)
            rrscene = read(native_root/safe(native['rrscene']), 64*1024*1024)
            rrmesh = read(native_root/safe(native['rrmesh']), 64*1024*1024)
            parsed = scene_native(rrscene, rrmesh)
            need(parsed['materials'] == native['materials'], 'Imported material names differ')
            rec = record(dict(converter='rr-native-scene-48D-v1', source_sha256=sha(original_bytes), scene_sha256=sha(rrscene), mesh_sha256=sha(rrmesh), vertices=native['vertices'], triangles=native['indices']//3, nodes=native['nodes'], lods=native['lods'], materials=parsed['materials'], world_space=parsed['world'], vertices_all_lods=parsed['all_vertices'], surfaces_all_lods=parsed['all_surfaces']))
            key = rec['key']
            request = dict(container=container, member=inventory_index[member]['name'], manifest_member=inventory_index[member]['name'], key=key, source_sha256=sha(original_bytes), native=rec, status='converted_geometry', record='cacheobjects/'+key+'.json', rrscene=store(key+'.rrscene', rrscene, 'native_scene'), rrmesh=store(key+'.rrmesh', rrmesh, 'native_mesh'), world_instance_indices=[])
            scenes[member] = parsed
            scene_requests[member] = request
        scene, request = scenes[member], scene_requests[member]
        request['world_instance_indices'].append(wi)
        row = dict(base, key=request['key'], status='converted_geometry')
        visuals.append(row)
        placed_row = dict(row, vertex_start=vertex_count, index_start=len(indices), surfaces=[])
        # Original packet builder0x240B04 and runtime0x241678 bypass the
        # SCB root for world-space single-node geometry. RRSCENE has already
        # reconstructed rawXYZ*scale+quantization-origin in position_local.
        matrix = [1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.] if scene['world'] else instance['matrix']
        placed_row['visual_matrix'] = matrix
        placed_row['world_space_root_bypassed'] = scene['world']
        for surface in scene['lods'][0]:
            vb, ib = vertex_count, len(indices)
            for v in surface['vertices']:
                p = []
                for axis in range(3):
                    value = 0.
                    for k in range(3):
                        value += v[k]*matrix[k*4+axis]
                    p.append(value+matrix[12+axis])
                rgba = v[8:12] if surface['colors'] else (255, 255, 255, 255)
                vertices.extend(struct.pack('<5f4B', *(p+list(v[6:8])+list(rgba))))
                vertex_count += 1
            indices.extend(vb+i for i in surface['indices'])
            placed_row['surfaces'].append(dict(material_index=surface['material'], material=scene['materials'][surface['material']], vertex_start=vb, vertices=vertex_count-vb, index_start=ib, indices=len(indices)-ib, has_original_colors=surface['colors']))
        placed_row['vertices'] = vertex_count-placed_row['vertex_start']
        placed_row['indices'] = len(indices)-placed_row['index_start']
        placed.append(placed_row)

    material_users = collections.defaultdict(list)
    for member, request in scene_requests.items():
        for name in scenes[member]['materials']:
            material_users[name].append(dict(scene_key=request['key'], scene_member=request['member']))
    materials, textures = [], []
    texture_users = collections.defaultdict(list)
    for name, users in sorted(material_users.items()):
        member = norm('textures/'+name+'.psm')
        need(member in imported and imported[member]['status'] == 'converted_material', 'Required original material missing or unsupported '+name)
        native = imported[member]
        rec = record(dict(converter='rr-native-material-48E-v1', source_sha256=sha(source(member)), type='material', general=native['general'], passes=native['passes']))
        chosen = candidate(member, 'converted_material', rec)
        materials.append(dict(container=container, name=name, requested_member=member, expected_status='converted_material', scope='local', local_candidates=1, global_candidates=1, candidates=[chosen], selected=chosen, key=rec['key'], native=rec, requested_by=users, status='resolved'))
        for material_pass in native['passes']:
            for texture in material_pass['textures']:
                texture_users[texture['name']].append(dict(material_key=rec['key'], material_name=name, pass_=material_pass['pass'], slot=texture['slot'], program=material_pass['program']))
    source_textures = []
    sky_and_texture_codec = "Existing scalar FFmpeg IPUM decode; no guest execution or chipset emulation"
    for name, users in sorted(texture_users.items()):
        for user in users:
            user['pass'] = user.pop('pass_')
        member = norm('textures/'+name+'.pst')
        row = dict(container=container, name=name, requested_member=member, expected_status='converted_texture', requested_by=users)
        if member in inventory_index:
            native = imported.get(member)
            original_bytes = source(member)
            need(len(original_bytes) >= 12, 'Original PST header extent')
            revision, texture_kind, mip_count = struct.unpack_from('<3I', original_bytes)
            mips, blobs = [], []
            frames = parse_resource(member.lower(), original_bytes, allow_kind0=True)
            need(revision == 1 and len(frames) == 1 and frames[0]['kind'] in (0,1,2), 'Unsupported original PST subset')
            for mip in frames[0]['mips']:
                if frames[0]['kind'] in (0,1):
                    rgba = decode_indexed(mip['data'], frames[0]['palette'], frames[0]['kind'])
                else:
                    rgba, _ = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
                data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
                need(len(rgba) == mip['width']*mip['height']*4, 'Complete original texture RGBA extent')
                mips.append(dict(index=mip['level'], width=mip['width'], height=mip['height'], sha256=sha(data)))
                blobs.append(data)
            if frames[0]['kind'] in (0,1):
                need(native and native['status'] == 'converted_texture' and blobs[-1] == read(native_root/safe(native['rrtex'])), 'Native/scalar indexed original decode disagreement')
            converter = 'rr-native-texture-48E-v1' if frames[0]['kind'] in (0,1) else 'rr-original-texture-50FC-v1'
            rec = record(dict(converter=converter, source_sha256=sha(original_bytes), type='texture', mips=mips))
            for mip, data in zip(mips, blobs):
                store(rec['key']+'-mip'+str(mip['index'])+'.rrtex', data, 'native_texture')
            chosen = candidate(member, 'converted_texture', rec)
            row.update(scope='local', local_candidates=1, global_candidates=1, candidates=[chosen], selected=chosen, key=rec['key'], native=rec, status='resolved')
        elif norm('textures/'+name+'.tga') in inventory_index:
            member = norm('textures/'+name+'.tga')
            need(track == 'RA' and member in RA_TGA_SOURCES,
                 'Unadmitted original TGA source '+member)
            width, depth, source_digest = RA_TGA_SOURCES[member]
            original_bytes = source(member)
            need(len(original_bytes) == 18+width*width*(depth//8)+26 and
                 sha(original_bytes) == source_digest, 'Original RA TGA identity')
            native = imported.get(member)
            source_format = 'tga_bgr24' if depth == 24 else 'tga_bgra32'
            need(native is not None and native['status'] == 'converted_texture' and
                 native.get('converter') == 'rr-original-tga-50LA-v1' and
                 native.get('source_format') == source_format and
                 native['width'] == width and native['height'] == width,
                 'Required native original TGA conversion missing '+member)
            dimensions = [16, 32, 64, 128] + ([256] if width == 256 else [])
            native_mips = native.get('native_mips', [])
            need(native['mips'] == len(dimensions) and len(native_mips) == len(dimensions),
                 'Incomplete original TGA mip chain')
            mips, blobs = [], []
            for index, (side, mip) in enumerate(zip(dimensions, native_mips)):
                need(mip['index'] == index and mip['width'] == side and mip['height'] == side,
                     'Original TGA mip order/dimensions differ')
                need(mip['rrtex'] in native_files, 'Unreviewed original TGA mip')
                data = read(native_root/safe(mip['rrtex']))
                reviewed = native_files[mip['rrtex']]
                need(len(data) == reviewed['bytes'] and sha(data) == reviewed['sha256'] and
                     len(data) == 16+side*side*4 and
                     data[:16] == struct.pack('<8sII', b'RRTEX001', side, side),
                     'Original TGA native mip identity/header/extent differs')
                mips.append(dict(index=index, width=side, height=side, sha256=sha(data)))
                blobs.append(data)
            need(native['rrtex'] == native_mips[-1]['rrtex'], 'Original TGA base mip differs')
            rec = record(dict(converter='rr-original-tga-50LA-v1',
                              source_sha256=source_digest, source_format=source_format,
                              type='texture', mips=mips))
            for mip, data in zip(mips, blobs):
                store(rec['key']+'-mip'+str(mip['index'])+'.rrtex', data, 'native_texture')
            chosen = candidate(member, 'converted_texture', rec)
            row.update(requested_member=member, scope='local', local_candidates=1,
                       global_candidates=1, candidates=[chosen], selected=chosen,
                       key=rec['key'], native=rec, status='resolved')
        else:
            need(member not in inventory_index, 'Unsupported required source PST '+member)
            atm = norm('textures/'+name+'.atm')
            need(atm in inventory_index, 'Absent original PST and ATM '+name)
            original_bytes = source(atm)
            frames = parse_resource(atm.lower(), original_bytes, allow_kind0=True)
            bindings = animation_bindings(imported, name, len(frames))
            starts = {binding['initial_frame'] for binding in bindings}
            need(len(starts) == 1, 'ATM has distinct authored starts requiring per-material ownership: '+name)
            selected_frame = starts.pop()
            need(0 <= selected_frame < len(frames), 'Authored animation start frame outside source extent')
            frame, mip = frames[selected_frame], frames[selected_frame]['mips'][-1]
            if frame['kind'] in (0, 1):
                rgba = decode_indexed(mip['data'], frame['palette'], frame['kind'])
                yuv_sha = None
            else:
                rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
            data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
            filename = name.lower()+'-frame'+str(selected_frame)+'-mip'+str(mip['level'])+'.rrtex'
            target = args.output_root/('evidence/textures'+track)/filename
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            source_textures.append(dict(member=atm, source_sha256=sha(original_bytes), source_bytes=len(original_bytes), output_filename=filename, output_sha256=sha(data), width=mip['width'], height=mip['height'], mip_index=mip['level'], frame_count=len(frames), selected_frame=selected_frame, material_bindings=bindings, encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha))
            row.update(scope='global', local_candidates=0, global_candidates=0, candidates=[], status='unresolved', reason='no_exact_member', non_substitutable_same_stem=[candidate(atm, 'original_animation_source')])
        textures.append(row)

    # Preserve every original ATM initial frame, including dynamic-only users.
    # These are exact source identities, not another track's same-name pixels.
    retained_atm = {row['member'] for row in source_textures}
    for original_row in cfg['atm_members']:
        member = norm(original_row['name'])
        if member in retained_atm:
            continue
        name = Path(original_row['path']).stem
        original_bytes = source(member)
        frames = parse_resource(member.lower(), original_bytes, allow_kind0=True)
        bindings = animation_bindings(imported, name, len(frames))
        starts = {binding['initial_frame'] for binding in bindings}
        need(len(starts) == 1, 'ATM has distinct authored starts requiring per-material ownership: '+name)
        selected_frame = starts.pop()
        need(0 <= selected_frame < len(frames), 'Authored ATM start outside source extent')
        frame, mip = frames[selected_frame], frames[selected_frame]['mips'][-1]
        if frame['kind'] in (0, 1):
            rgba, yuv_sha = decode_indexed(mip['data'], frame['palette'], frame['kind']), None
        else:
            rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
        data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
        filename = name.lower()+'-frame'+str(selected_frame)+'-mip'+str(mip['level'])+'.rrtex'
        target = args.output_root/('evidence/textures'+track)/filename
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        source_textures.append(dict(member=member, source_sha256=sha(original_bytes),
            source_bytes=len(original_bytes), output_filename=filename, output_sha256=sha(data),
            width=mip['width'], height=mip['height'], mip_index=mip['level'],
            frame_count=len(frames), selected_frame=selected_frame, material_bindings=bindings,
            encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha))
    source_textures.sort(key=lambda row: row['member'])
    expected_sources = {norm(r['name']) for r in cfg['atm_members']}
    need(len(source_textures) == len(expected_sources) and
         {r['member'] for r in source_textures} == expected_sources, 'Complete original ATM source closure')

    mesh = struct.pack('<8sII', b'RRMESH01', vertex_count, len(indices))+vertices+struct.pack('<'+str(len(indices))+'I', *indices)
    placed_dir = output/'export-placed-normal'
    placed_dir.mkdir(parents=True, exist_ok=True)
    (placed_dir/(track.lower()+'-world.rrmesh')).write_bytes(mesh)
    selection = dict(schema='RR_VISUAL_SELECTION49G_V1', status='complete', container=container, world_reference_sha256=cfg['reference_sha256'], world_reference='original-world.reference.json', archive_sha256=archive_sha, scene_sha256=scene_sha, scene_requests=list(scene_requests.values()), visual_instances=visuals, non_drawn_instances=nondrawn, material_resolution=materials, texture_resolution=textures, objects=sorted(objects.values(), key=lambda x:x['path']), import_scope='Exact '+track+'.RES only; no external-container substitute', limitations=['This is authentic asset/placement closure, not renderer or gameplay validation.', 'Authored two-pass ProjectedPrelit requires explicit native runtime support; passes may not be silently dropped.'])
    exported = dict(schema='RR_AUTHORED_MESH49G_V1', success=True, container=container, archive_sha256=archive_sha, scene_sha256=scene_sha, world_instances=placed, vertices=vertex_count, indices=len(indices), mesh_sha256=sha(mesh), arithmetic='Ordered binary64 multiply/add and one binary32 cast. Local geometry uses SCB root replacement. Original world-space flag bypasses SCB pose, matching source 0x240B04 and 0x241678.')
    (output/'original-world.reference.json').write_bytes(reference_bytes)
    selected_bytes, export_bytes = jb(selection), jb(exported)
    (output/'asset-selection.json').write_bytes(selected_bytes)
    (placed_dir/'export-report.json').write_bytes(export_bytes)
    source_dir = args.output_root/('evidence/textures'+track)
    source_dir.mkdir(parents=True, exist_ok=True)
    (source_dir/'manifest.json').write_bytes(jb(dict(schema='RR_ORIGINAL_'+track+'_SOURCE_TEXTURES_V1', archive_sha256=archive_sha, textures=source_textures, limitations=['Genuine authored initial frames; all-frame bank is retained separately and live playback belongs to the selected-track animation owner.'])))
    include = '\n'.join(' {"%s", "%s", "%s", "%s", %du, %du, %du, %du},' % (r['member'], r['source_sha256'], r['output_filename'], r['output_sha256'], r['source_bytes'], r['width'], r['height'], r['mip_index']) for r in source_textures)+'\n'
    (args.output_root/('tests/native/source_texture_'+track.lower()+'_index.inc')).write_text(include)
    stats = dict(archive_bytes=len(archive), archive_sha256=archive_sha, selection_sha256=sha(selected_bytes), selection_bytes=len(selected_bytes), export_sha256=sha(export_bytes), export_bytes=len(export_bytes), mesh_sha256=sha(mesh), mesh_bytes=len(mesh), vertices=vertex_count, indices=len(indices), scenes=len(scenes), instances=len(placed), surfaces=sum(len(r['surfaces']) for r in placed), materials=len(materials), texture_names=len(textures), native_textures=sum(r['status']=='resolved' for r in textures), source_atm=sum(r['frame_count'] > 1 for r in source_textures), non_drawn=len(nondrawn), max_world_instance=max(r['world_instance_index'] for r in placed), material_passes=sum(len(r['native']['passes']) for r in materials), projected_materials=sum(any(p['program']=='ProjectedPrelit' for p in r['native']['passes']) for r in materials), draw_spans=sum(len(next(m for m in materials if m['name']==s['material'])['native']['passes']) for r in placed for s in r['surfaces']))
    need(len(placed) == cfg['instances']-cfg['missing_geometry'] and
         len(nondrawn) == cfg['missing_geometry'], 'Exact authored static draw closure')
    need(sum(norm(row['collision_member']) in inventory_index for row in world['instances']) == cfg['collision_instances'],
         'Exact authored static collision closure')
    need(stats['vertices'] <= 1000000 and stats['indices'] <= 3000000 and
         stats['texture_names'] <= 4096 and stats['draw_spans'] <= 65536,
         'Selected-track fixed native budgets')
    profile = ('    "%s",\n'
               '    "%s",\n    "%s",\n    "%s",\n'
               '    %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,32u*1024u*1024u\n') % (
                   container, stats['selection_sha256'], stats['export_sha256'], archive_sha,
                   stats['vertices'], stats['indices'], stats['scenes'], stats['materials'],
                   stats['instances'], stats['surfaces'], stats['draw_spans'], stats['max_world_instance'],
                   cfg['archive_entries'], cfg['archive_bytes'])
    (args.output_root/('tests/native/'+track.lower()+'_visual_profile.inc')).write_text(profile)
    stats['source_texture_codec'] = sky_and_texture_codec
    stats['generated_profile_sha256'] = sha(profile.encode())
    (output/'assembly-manifest.json').write_bytes(jb(stats))
    recover_source_banks(args, originals, imported, world, placed)
    (output/'ASSEMBLY-COMPLETE.json').write_bytes(jb(dict(status='authenticated_ryloth_cold_assembly_v1', track=track, import_report_sha256=sha(import_bytes), native_output_index_sha256=sha(native_index_bytes), assembly=stats, gameplay_ready=False, game_launched=False)))
    print(json.dumps(stats))


def recover_source_banks(args, originals, imported, world, placed):
    """Recover authentic sky and complete ATM banks; never claim playback."""
    from decode_world48e import decode_parameter_blocks
    from decode_source_textures50fc import parse_resource, decode_indexed, decode_ipu
    from recover_ma_sa_sky import track_tiles
    cfg, track = originals.cfg, args.track
    scb, cub = originals.read(cfg['scb']['name']), originals.read(cfg['cub']['name'])
    blocks = decode_parameter_blocks(scb)['sections']
    need('environment' in blocks, 'Missing original sky environment')
    environment = blocks['environment']['fields']
    env = {f['name']:f['value'] for f in environment}
    need(env.get('numberofskycubes') == '1' and
         norm('skies/'+env.get('cubefile1','')) == norm(cfg['cub']['name']),
         'Unsupported sky cube binding, exact original cube required')
    sky_zones, fog_zones = [], []
    for zone in world['zones']:
        key = 'cubefileforzone'+str(zone['zone'])
        value = env.get(key)
        need(value is None or value in ('0','1'), 'Unsupported original cube index: '+key)
        # Original scene loader 276D7C defaults1,276E24 subtracts1;
        # presentation275914 skips negative indices. No heuristic visibility.
        sky_zones.append(dict(zone=zone['zone'], original_field=key,
            authored_value=value, field=[f for f in environment if f['name']==key],
            original_cube_index=(int(value)-1) if value is not None else 0,
            authored_default_first_cube=value is None))
        fields = blocks['zone'+str(zone['zone'])]['fields']
        fog_zones.append(dict(zone=zone['zone'], authored_fields=[f for f in fields if f['name'].startswith('fog')]))
    records = track_tiles(cub,cfg)
    output = args.output_root/('evidence/sky'+track)
    output.mkdir(parents=True)
    index = []
    for record in records:
        mip = record['mips'][-1]
        raw = cub[mip['offset']:mip['offset']+mip['bytes']]
        rgba, yuv_sha = decode_ipu(raw,128,128,'ffmpeg')
        rrtex = struct.pack('<8sII',b'RRTEX001',128,128)+rgba
        filename = 'face%d-tile%02d.rrtex' % (record['face'],record['tile'])
        (output/filename).write_bytes(rrtex)
        record['yuv_sha256'] = yuv_sha
        record['decoded'] = dict(filename=filename,sha256=sha(rrtex),bytes=len(rrtex),width=128,height=128)
        index.append('    {%d, %d, %d, %d, "%s", "%s", "%s"},' %
                     (record['face'],record['tile'],record['offset'],record['bytes'],record['sha256'],filename,sha(rrtex)))
    (args.output_root/('tests/native/sky_'+track.lower()+'_index.inc')).write_text('\n'.join(index)+'\n')
    sky_manifest = dict(schema='RR_ORIGINAL_'+track+'_SKY_V1',archive_sha256=cfg['archive_sha256'],
        archive_bytes=cfg['archive_bytes'],member=cfg['cub']['name'],member_sha256=sha(cub),member_bytes=len(cub),
        scene_member=cfg['scb']['name'],scene_sha256=sha(scb),scene_bytes=len(scb),
        scene_sky_fields=environment,zone_cube_bindings=sky_zones,zone_fog_bindings=fog_zones,
        visible_authored_zones=[r['zone'] for r in sky_zones if r['original_cube_index']==0],
        tile_count=len(records),rgba_bytes=len(records)*128*128*4,tiles=records,
        half_extent=25600,camera_relative_y_offset=1024,
        face_order=['+Z','-X','-Z','+X','+Y','-Y'],source_uv_range=[0.03125,0.96875],
        limitations=['Actual scene-camera zone must select its original cube binding.',
                    'Fog fields are exact authored per-zone values; no invented global fog default.',
                    'Scalar FFmpeg IPUM IDCT is not a PS2 framebuffer parity claim.'])
    (output/'manifest.json').write_bytes(canonical(sky_manifest))

    output = args.output_root/('evidence/atm'+track)
    output.mkdir(parents=True)
    animations, source_rows, frame_rows, animation_rows = [], [], [], []
    first, rgba_bytes = 0, 0
    for source_index, row in enumerate(sorted(cfg['atm_members'],key=lambda r:norm(r['name']))):
        member, name = row['name'], Path(row['path']).stem
        raw = originals.read(member)
        frames = parse_resource(member.lower(),raw,allow_kind0=True)
        bindings = animation_bindings(imported,name,len(frames))
        record = dict(member=member,name=name,source_sha256=sha(raw),source_bytes=len(raw),
                      first_texture_index=first,frame_count=len(frames),material_bindings=bindings,frames=[])
        for binding in bindings:
            need(binding['initial_frame'] < len(frames),'ATM initial frame outside exact original source')
            material = originals.read(binding['material_member'])
            binding.update(material_sha256=sha(material),material_bytes=len(material))
            material_name = Path(binding['material_member']).stem
            binding['placed_spans'] = sum(1 for instance in placed for surface in instance['surfaces']
                if norm(surface['material'])==norm(material_name))
            animation_rows.append('    {"%s","%s","%s","%s",%du,%du,%du,%du,%sf,%du},' %
                (material_name,name,binding['material_member'],sha(material),len(material),source_index,
                 binding['material_pass'],binding['initial_frame'],repr(float(binding['fps'])),binding['placed_spans']))
        source_rows.append('    {"%s","%s",%du,%du,%du},' % (norm(member),sha(raw),len(raw),len(frames),first))
        for fi, frame in enumerate(frames):
            mip = frame['mips'][-1]
            if frame['kind'] in (0,1):
                rgba, yuv_sha = decode_indexed(mip['data'],frame['palette'],frame['kind']),None
            else:
                rgba, yuv_sha = decode_ipu(mip['data'],mip['width'],mip['height'],'ffmpeg')
            need(len(rgba)==mip['width']*mip['height']*4,'ATM decoded source extent')
            rrtex = struct.pack('<8sII',b'RRTEX001',mip['width'],mip['height'])+rgba
            filename = name.lower()+'-frame'+str(fi)+'-mip'+str(mip['level'])+'.rrtex'
            (output/filename).write_bytes(rrtex);rgba_bytes += len(rgba)
            entry = dict(frame=fi,start=frame['start'],bytes=frame['end']-frame['start'],
                pst_sha256=sha(raw[frame['start']:frame['end']]),native_sha256=sha(rrtex),
                width=mip['width'],height=mip['height'],mip=mip['level'],kind=frame['kind'],
                output_filename=filename,encoded_sha256=sha(mip['data']),yuv_sha256=yuv_sha)
            record['frames'].append(entry)
            frame_rows.append('    {%du,%du,%du,%du,%du,"%s","%s","%s"},' %
                (entry['start'],entry['bytes'],entry['width'],entry['height'],entry['mip'],
                 entry['pst_sha256'],entry['native_sha256'],filename))
        first += len(frames);animations.append(record)
    need(first <= 64*len(animations),'Original ATM frame bank bound')
    generated = args.output_root/'tests/native'
    for suffix, rows in (('source',source_rows),('frame',frame_rows),('animation',animation_rows)):
        (generated/(track.lower()+'_atm_'+suffix+'_index.inc')).write_text('\n'.join(rows)+'\n')
    (output/'manifest.json').write_bytes(canonical(dict(schema='RR_ORIGINAL_'+track+'_ATM_FRAMES_V1',
        archive_sha256=cfg['archive_sha256'],animations=animations,frames=first,rgba_bytes=rgba_bytes,
        limitations=['Complete authentic source frame bank; live playback belongs to native selected-track owner.',
                    'Each original material retains exact timing and start controls.'])))


if __name__ == '__main__':
    main()
