#!/usr/bin/env python3
"""Cold-assemble exact original MA/SA assets; UNEXECUTED source candidate.

Every present referenced PSG must convert. Only genuinely absent original PSG
references are non-drawn. No existing geometry is dropped to meet budgets.
"""
import argparse
import collections
import json
from pathlib import Path
import struct
import sys
sys.dont_write_bytecode = True
from ma_sa_asset_sources import Sources, animation_bindings, need, sha, norm


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--track', choices=('MA', 'SA'), required=True)
    parser.add_argument('--intake', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    args = parser.parse_args()
    sys.path.insert(0, str(args.repo/'tools/assets'))
    from assemble_ga_visual import jb, scene_native
    from decode_world48e import decode_world
    from decode_source_textures50fc import parse_resource, decode_indexed, decode_ipu
    originals = Sources(args.intake, args.track)
    cfg, track = originals.cfg, args.track
    container, archive_sha, scene_sha = 'TRACKS/'+track+'.RES', cfg['archive_sha256'], cfg['scb']['sha256']
    archive = originals.archive
    scb = originals.read(cfg['scb']['name'])
    world = decode_world(scb)
    need((len(world['zones']), len(world['portals']), len(world['instances'])) ==
         (cfg['zones'], cfg['portals'], cfg['instances']), 'Original world extent')
    reference_bytes = (args.intake/'intake/retained48F-world.reference.json').read_bytes()
    need(sha(reference_bytes) == cfg['reference_sha256'], 'Reviewed retained world identity')
    # The native-source decoder returns tuples for vectors/matrices; JSON
    # represents those exact sequences as arrays. Compare the same JSON domain
    # without dropping fields, rounding numbers, or changing source placements.
    need(json.loads(json.dumps(world, allow_nan=False)) == json.loads(reference_bytes),
         'Current world decoder differs from retained original placements')
    inventory_index = originals.members
    native_root = args.intake/'native-import'
    importer = json.loads((native_root/'import.json').read_bytes())
    imported = {norm(row['source_member']): row for row in importer['members']}
    output = args.output_root/('evidence/visual'+track)
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
            rrscene = (native_root/native['rrscene']).read_bytes()
            rrmesh = (native_root/native['rrmesh']).read_bytes()
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
            if texture_kind == 0:
                # This exact authentic MA resource uses 16-entry palette and
                # packed low/high nibbles. Only the pinned native48E decoder
                # expands it; Python's kind1 byte-index path is never invoked.
                need(track == 'MA' and member == 'TEXTURES/MA_SHIELD.PST' and
                     revision == 1 and mip_count == 4, 'Exact original MA kind0 source subset')
                need(native and native['status'] == 'converted_texture' and
                     native['texture_kind'] == 0 and native['mips'] == mip_count and
                     len(native['native_mips']) == mip_count, 'Authenticated native kind0 mip closure')
                need((native_root/native['source_file']).read_bytes() == original_bytes,
                     'Native kind0 source differs from exact original member')
                previous, source_end = None, 12+16*4
                for index, mip in enumerate(native['native_mips']):
                    width, height = mip['width'], mip['height']
                    need(mip['index'] == index and 16 <= width <= 1024 and 16 <= height <= 1024 and
                         not (width & (width-1)) and not (height & (height-1)) and
                         (previous is None or (width, height) == (previous[0]*2, previous[1]*2)),
                         'Native kind0 original mip order/dimensions')
                    source_header, source_indices = mip['source_header_offset'], mip['source_indices_offset']
                    need(source_header == source_end and 0 <= source_header <= len(original_bytes)-16 and
                         source_indices == source_header+16 and
                         width*height//2 <= len(original_bytes)-source_indices and
                         struct.unpack_from('<4I', original_bytes, source_header) == (height, width, 0, 0),
                         'Native kind0 source mip header/packed-index extent')
                    data = (native_root/mip['rrtex']).read_bytes()
                    need(len(data) == 16+width*height*4 and
                         struct.unpack_from('<8sII', data) == (b'RRTEX001', width, height),
                         'Native kind0 retained mip framing')
                    mips.append(dict(index=index, width=width, height=height, sha256=sha(data)))
                    blobs.append(data);previous = (width, height)
                    source_end = source_indices+width*height//2
                need(source_end == len(original_bytes), 'Native kind0 complete source mip coverage')
                need(blobs[-1] == (native_root/native['rrtex']).read_bytes(), 'Native kind0 base mip identity')
                converter = 'rr-native-texture-48E-v1'
            else:
                frames = parse_resource(member.lower(), original_bytes)
                need(len(frames) == 1 and frames[0]['kind'] in (1, 2), 'Original '+track+' PST subset')
                for mip in frames[0]['mips']:
                    if frames[0]['kind'] == 1:
                        rgba = decode_indexed(mip['data'], frames[0]['palette'])
                    else:
                        rgba, _ = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
                    data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
                    mips.append(dict(index=mip['level'], width=mip['width'], height=mip['height'], sha256=sha(data)))
                    blobs.append(data)
                if native and native['status'] == 'converted_texture':
                    need(blobs[-1] == (native_root/native['rrtex']).read_bytes(), 'Original native/Python indexed decode disagreement')
                converter = 'rr-native-texture-48E-v1' if frames[0]['kind'] == 1 else 'rr-original-texture-50FC-v1'
            rec = record(dict(converter=converter, source_sha256=sha(original_bytes), type='texture', mips=mips))
            for mip, data in zip(mips, blobs):
                store(rec['key']+'-mip'+str(mip['index'])+'.rrtex', data, 'native_texture')
            chosen = candidate(member, 'converted_texture', rec)
            row.update(scope='local', local_candidates=1, global_candidates=1, candidates=[chosen], selected=chosen, key=rec['key'], native=rec, status='resolved')
        else:
            need(member not in inventory_index, 'Unsupported required source PST '+member)
            atm = norm('textures/'+name+'.atm')
            need(atm in inventory_index, 'Absent original PST and ATM '+name)
            original_bytes = source(atm)
            frames = parse_resource(atm.lower(), original_bytes, allow_kind0=True)
            bindings = animation_bindings(imported, name)
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
        bindings = animation_bindings(imported, name)
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
    # Exact SA animated-world closure: original kind2 PSTs for the eight
    # authored dynamic materials. Their static/direct counterparts are never
    # substituted from another container, even when texture names coincide.
    dynamic_sources = ('robotred01','robotred02','robox','slide02',
                       'larobo02','larobo01','larobo03','lazerrobo01') if track == 'SA' else ()
    for name in dynamic_sources:
        member = norm('textures/'+name+'.pst')
        original_bytes = source(member)
        frames = parse_resource(member.lower(), original_bytes)
        need(len(frames) == 1 and frames[0]['kind'] == 2, 'Exact original SA dynamic IPUM texture '+name)
        frame, mip = frames[0], frames[0]['mips'][-1]
        rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
        data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
        filename = name+'-dynamic-mip'+str(mip['level'])+'.rrtex'
        target = args.output_root/('evidence/textures'+track)/filename
        target.parent.mkdir(parents=True, exist_ok=True);target.write_bytes(data)
        source_textures.append(dict(member=member, source_sha256=sha(original_bytes),
            source_bytes=len(original_bytes), output_filename=filename, output_sha256=sha(data),
            width=mip['width'], height=mip['height'], mip_index=mip['level'], frame_count=1,
            selected_frame=0, dynamic_owner_source=True, encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha))
    source_textures.sort(key=lambda row: row['member'])
    expected_sources = {norm(r['name']) for r in cfg['atm_members']} | {
        norm('textures/'+name+'.pst') for name in dynamic_sources}
    need(len(source_textures) == len(expected_sources) and
         {r['member'] for r in source_textures} == expected_sources, 'Complete original ATM/dynamic source closure')

    mesh = struct.pack('<8sII', b'RRMESH01', vertex_count, len(indices))+vertices+struct.pack('<'+str(len(indices))+'I', *indices)
    placed_dir = output/'export-placed-normal'
    placed_dir.mkdir(parents=True, exist_ok=True)
    (placed_dir/(track.lower()+'-world.rrmesh')).write_bytes(mesh)
    selection = dict(schema='RR_VISUAL_SELECTION49G_V1', status='complete', container=container, archive_sha256=archive_sha, scene_sha256=scene_sha, scene_requests=list(scene_requests.values()), visual_instances=visuals, non_drawn_instances=nondrawn, material_resolution=materials, texture_resolution=textures, objects=sorted(objects.values(), key=lambda x:x['path']), import_scope='Exact '+track+'.RES only; no external-container substitute', limitations=['This is authentic asset/placement closure, not renderer or gameplay validation.', 'Authored two-pass ProjectedPrelit requires explicit native runtime support; passes may not be silently dropped.'])
    exported = dict(schema='RR_AUTHORED_MESH49G_V1', success=True, container=container, archive_sha256=archive_sha, scene_sha256=scene_sha, world_instances=placed, vertices=vertex_count, indices=len(indices), mesh_sha256=sha(mesh), arithmetic='Ordered binary64 multiply/add and one binary32 cast. Local geometry uses SCB root replacement. Original world-space flag bypasses SCB pose, matching source 0x240B04 and0x241678.')
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
    print(json.dumps(stats))


if __name__ == '__main__':
    main()
