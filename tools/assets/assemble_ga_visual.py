#!/usr/bin/env python3
"""Assemble source-authenticated GA native assets and placed static geometry.

Consumes the completed native import. It does not re-import PSG/PSM assets,
execute guest code, or run a client/test. Original missing PSG references remain
explicitly non-drawn; an existing but unsupported referenced PSG is fatal.
"""
import argparse
import collections
import hashlib
import json
import math
from pathlib import Path
import struct
import sys

sys.dont_write_bytecode = True
GA = 'TRACKS/GA.RES'
GA_SHA = '063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9'
SCB_SHA = 'ad38b0d47efdda92e36bf36f9b380f7f3111698cb068532068a19229097de5d7'


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def jb(value):
    return (json.dumps(value, sort_keys=True, separators=(',', ':'), ensure_ascii=True, allow_nan=False)+'\n').encode()


def norm(name):
    return name.replace('\\', '/').upper()


class Reader:
    def __init__(self, data):
        self.data, self.at = data, 0

    def take(self, count):
        need(0 <= count <= len(self.data)-self.at, 'RRSCENE truncated')
        start = self.at
        self.at += count
        return self.data[start:self.at]

    def unpack(self, fmt):
        return struct.unpack(fmt, self.take(struct.calcsize(fmt)))

    def u32(self):
        return self.unpack('<I')[0]

    def text(self):
        count = self.u32()
        need(0 < count <= 128, 'RRSCENE name bound')
        raw = self.take(count)
        need(b'\0' not in raw, 'RRSCENE name NUL')
        return raw.decode('ascii')


def scene_native(data, companion):
    """Read the established RRSCENE1 serialization, not original PSG syntax."""
    need(96 <= len(data) <= 64*1024*1024, 'RRSCENE byte bound')
    r = Reader(data)
    need(r.take(8) == b'RRSCENE1' and r.u32() == 1, 'RRSCENE magic/version')
    world, identity, nodes, material_count, lod_count, all_vertices, all_surfaces = r.unpack('<7I')
    need(world in (0, 1) and identity == 0 and nodes == 1 and 0 < material_count <= 4096 and lod_count in (1, 4), 'Static RRSCENE subset')
    quantization = r.unpack('<7d')
    need(all(math.isfinite(x) for x in quantization), 'RRSCENE finite quantization')
    node_name, parent = r.text(), r.u32()
    need(parent == 0xffffffff, 'Static RRSCENE root')
    local = r.unpack('<16d')
    bind = r.unpack('<16d')
    bounds = r.unpack('<7d')
    need(local == bind and all(math.isfinite(x) for x in local+bind+bounds), 'Static RRSCENE root matrix')
    materials = [r.text() for _ in range(material_count)]
    lods, total_v, total_s, total_i = [], 0, 0, 0
    for _ in range(lod_count):
        cutoff = r.unpack('<d')[0]
        count = r.u32()
        need(math.isfinite(cutoff) and cutoff > 0 and 0 < count <= 16384, 'RRSCENE LOD bounds')
        surfaces = []
        for _ in range(count):
            material, colors, uv_size, vc, ic = r.unpack('<IIdII')
            total_v += vc
            total_i += ic
            total_s += 1
            need(material < material_count and colors in (0, 1) and 0 < vc <= 1000000 and 0 < ic <= 3000000 and ic % 3 == 0 and total_v <= 1000000 and total_i <= 3000000 and total_s <= 16384, 'RRSCENE surface bounds')
            vertices = [r.unpack('<8d4BH') for _ in range(vc)]
            need(all(v[-1] == 0 and all(math.isfinite(x) for x in v[:8]) for v in vertices), 'RRSCENE finite static vertices')
            indices = r.unpack('<'+str(ic)+'I')
            need(all(i < vc for i in indices), 'RRSCENE local index bounds')
            surfaces.append(dict(material=material, colors=bool(colors), vertices=vertices, indices=indices))
        lods.append(surfaces)
    count = r.u32()
    need(count == len(lods[0]), 'RRSCENE LOD0 span count')
    spans = [r.unpack('<6I') for _ in range(count)]
    vertex_at, index_at = 0, 0
    for span, surface in zip(spans, lods[0]):
        need(span == (surface['material'], vertex_at, len(surface['vertices']), index_at, len(surface['indices']), int(surface['colors'])), 'RRSCENE source span sequence')
        vertex_at += len(surface['vertices'])
        index_at += len(surface['indices'])
    mesh_bytes = r.u32()
    need(r.take(mesh_bytes) == companion and r.at == len(data), 'RRSCENE companion closure')
    need(total_v == all_vertices and total_s == all_surfaces, 'RRSCENE total counters')
    return dict(materials=materials, lods=lods, spans=spans, local=local, world=bool(world), all_vertices=all_vertices, all_surfaces=all_surfaces)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--intake', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    args = parser.parse_args()
    sys.path.insert(0, str(args.repo/'tools/assets'))
    from decode_world48e import decode_world
    from decode_source_textures50fc import parse_resource, decode_indexed, decode_ipu
    archive = (args.intake/'original/GA.RES').read_bytes()
    need(sha(archive) == GA_SHA, 'Exact original GA archive')
    original = args.intake/'intake/original-members'
    scb = (original/'tracks/ga.scb').read_bytes()
    need(sha(scb) == SCB_SHA, 'Exact original GA SCB')
    world = decode_world(scb)
    need(len(world["zones"]) == 12 and len(world["portals"]) == 18 and len(world["instances"]) == 753, "GA original world extent")
    inventory = json.loads((args.intake/'intake/archive-inventory.json').read_bytes())
    rows = inventory['members'] if isinstance(inventory, dict) else inventory
    inventory_index = {norm(row['name']): row for row in rows}
    native_root = args.intake/'native-import'
    importer = json.loads((native_root/'import.json').read_bytes())
    imported = {norm(row['source_member']): row for row in importer['members']}
    output = args.output_root/'evidence/visualGA'
    output.mkdir(parents=True, exist_ok=True)
    objects, scene_requests, scenes, visuals, nondrawn = {}, {}, {}, [], []

    def source(member):
        row = inventory_index[norm(member)]
        b = (original/norm(member).lower()).read_bytes()
        need(len(b) == row['bytes'] and sha(b) == row['sha256'], 'Original member identity '+member)
        return b

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
        out = dict(container=GA, member=row['name'], logical_offset=row['logical_offset'], source_bytes=row['bytes'], source_sha256=row['sha256'], status=status)
        if native:
            out['key'] = native['key']
        return out

    vertices, indices, placed = bytearray(), [], []
    vertex_count = 0
    for wi, instance in enumerate(world['instances']):
        member = norm(instance['geometry_member'])
        base = {k: instance[k] for k in ('section', 'geometry_member', 'zone', 'ordinal', 'source_block_offset', 'position', 'look', 'up', 'matrix', 'use_lighting')}
        base.update(world_instance_index=wi, container=GA)
        if member not in inventory_index:
            nondrawn.append(dict(base, status='non_drawn', reason='no_exact_PSG_member', collision_member=instance['collision_member'], collision_member_present=norm(instance['collision_member']) in inventory_index))
            continue
        need(member in imported and imported[member]['status'] == 'converted_geometry', 'Referenced source geometry unsupported '+member)
        native = imported[member]
        if member not in scenes:
            original_bytes = source(member)
            rrscene = (native_root/native['rrscene']).read_bytes()
            rrmesh = (native_root/native['rrmesh']).read_bytes()
            parsed = scene_native(rrscene, rrmesh)
            need(parsed['materials'] == native['materials'], 'Imported material names differ')
            rec = record(dict(converter='rr-native-scene-48D-v1', source_sha256=sha(original_bytes), scene_sha256=sha(rrscene), mesh_sha256=sha(rrmesh), vertices=native['vertices'], triangles=native['indices']//3, nodes=native['nodes'], lods=native['lods'], materials=parsed['materials'], world_space=parsed['world'], vertices_all_lods=parsed['all_vertices'], surfaces_all_lods=parsed['all_surfaces']))
            key = rec['key']
            request = dict(container=GA, member=inventory_index[member]['name'], manifest_member=inventory_index[member]['name'], key=key, source_sha256=sha(original_bytes), native=rec, status='converted_geometry', record='cacheobjects/'+key+'.json', rrscene=store(key+'.rrscene', rrscene, 'native_scene'), rrmesh=store(key+'.rrmesh', rrmesh, 'native_mesh'), world_instance_indices=[])
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
        materials.append(dict(container=GA, name=name, requested_member=member, expected_status='converted_material', scope='local', local_candidates=1, global_candidates=1, candidates=[chosen], selected=chosen, key=rec['key'], native=rec, requested_by=users, status='resolved'))
        for material_pass in native['passes']:
            for texture in material_pass['textures']:
                texture_users[texture['name']].append(dict(material_key=rec['key'], material_name=name, pass_=material_pass['pass'], slot=texture['slot'], program=material_pass['program']))
    source_textures = []
    sky_and_texture_codec = "Existing scalar FFmpeg IPUM decode; no guest execution or chipset emulation"
    for name, users in sorted(texture_users.items()):
        for user in users:
            user['pass'] = user.pop('pass_')
        member = norm('textures/'+name+'.pst')
        row = dict(container=GA, name=name, requested_member=member, expected_status='converted_texture', requested_by=users)
        if member in inventory_index:
            native = imported.get(member)
            original_bytes = source(member)
            frames = parse_resource(member.lower(), original_bytes)
            need(len(frames) == 1 and frames[0]['kind'] in (1, 2), 'Original GA PST subset')
            mips, blobs = [], []
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
            rec = record(dict(converter='rr-native-texture-48E-v1' if frames[0]['kind'] == 1 else 'rr-original-texture-50FC-v1', source_sha256=sha(original_bytes), type='texture', mips=mips))
            for mip, data in zip(mips, blobs):
                store(rec['key']+'-mip'+str(mip['index'])+'.rrtex', data, 'native_texture')
            chosen = candidate(member, 'converted_texture', rec)
            row.update(scope='local', local_candidates=1, global_candidates=1, candidates=[chosen], selected=chosen, key=rec['key'], native=rec, status='resolved')
        else:
            need(member not in inventory_index, 'Unsupported required source PST '+member)
            atm = norm('textures/'+name+'.atm')
            need(atm in inventory_index, 'Absent original PST and ATM '+name)
            original_bytes = source(atm)
            frames = parse_resource(atm.lower(), original_bytes)
            original_material = imported[norm('textures/'+name+'.psm')]
            authored = {f['name']: f['value'] for f in original_material['passes'][0]['fields']['authored']}
            selected_frame = int(authored.get('animatedMapInitialFrame', '0'))
            need(0 <= selected_frame < len(frames), 'Authored animation start frame outside source extent')
            frame, mip = frames[selected_frame], frames[selected_frame]['mips'][-1]
            if frame['kind'] == 1:
                rgba = decode_indexed(mip['data'], frame['palette'])
                yuv_sha = None
            else:
                rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
            data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
            filename = name.lower()+'-frame'+str(selected_frame)+'-mip'+str(mip['level'])+'.rrtex'
            target = args.output_root/'evidence/texturesGA'/filename
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            source_textures.append(dict(member=atm, source_sha256=sha(original_bytes), source_bytes=len(original_bytes), output_filename=filename, output_sha256=sha(data), width=mip['width'], height=mip['height'], mip_index=mip['level'], frame_count=len(frames), selected_frame=selected_frame, authored_frames_per_second=authored.get('animatedMapFramesPerSecond'), encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha))
            row.update(scope='global', local_candidates=0, global_candidates=0, candidates=[], status='unresolved', reason='no_exact_member', non_substitutable_same_stem=[candidate(atm, 'original_animation_source')])
        textures.append(row)

    # Exact GA dynamic owner closure. These five originals include three IPUM
    # PSTs that native48E deliberately does not decode. Publish separate pinned
    # source images for the dynamic material owner; never reuse GC pixels.
    dynamic_source_names = ('fience', 'fience_opening', 'market_hut_a01',
                            'opening_a', 'house_stand_e01')
    for name in dynamic_source_names:
        member = norm('textures/'+name+'.pst')
        original_bytes = source(member)
        frames = parse_resource(member.lower(), original_bytes)
        need(len(frames) == 1 and frames[0]['kind'] in (1, 2),
             'GA dynamic original PST subset '+member)
        frame, mip = frames[0], frames[0]['mips'][-1]
        if frame['kind'] == 1:
            rgba = decode_indexed(mip['data'], frame['palette'])
            yuv_sha = None
        else:
            rgba, yuv_sha = decode_ipu(mip['data'], mip['width'], mip['height'], 'ffmpeg')
        data = struct.pack('<8sII', b'RRTEX001', mip['width'], mip['height'])+rgba
        native = imported.get(member)
        if native and native['status'] == 'converted_texture':
            need(data == (native_root/native['rrtex']).read_bytes(),
                 'GA dynamic native/Python indexed decode disagreement')
        filename = name+'-dynamic-mip'+str(mip['level'])+'.rrtex'
        target = args.output_root/'evidence/texturesGA'/filename
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        source_textures.append(dict(member=member, source_sha256=sha(original_bytes),
            source_bytes=len(original_bytes), output_filename=filename,
            output_sha256=sha(data), width=mip['width'], height=mip['height'],
            mip_index=mip['level'], frame_count=1, selected_frame=0,
            dynamic_owner_source=True, encoded_sha256=sha(mip['data']), yuv_sha256=yuv_sha))
    need(len(source_textures) == 9 and len({r['member'] for r in source_textures}) == 9,
         'Exact GA four-ATM/five-dynamic-PST source fallback closure')

    mesh = struct.pack('<8sII', b'RRMESH01', vertex_count, len(indices))+vertices+struct.pack('<'+str(len(indices))+'I', *indices)
    placed_dir = output/'export-placed-normal'
    placed_dir.mkdir(parents=True, exist_ok=True)
    (placed_dir/'ga-world.rrmesh').write_bytes(mesh)
    selection = dict(schema='RR_VISUAL_SELECTION49G_V1', status='complete', container=GA, archive_sha256=GA_SHA, scene_sha256=SCB_SHA, scene_requests=list(scene_requests.values()), visual_instances=visuals, non_drawn_instances=nondrawn, material_resolution=materials, texture_resolution=textures, objects=sorted(objects.values(), key=lambda x:x['path']), import_scope='Exact GA.RES only; no external-container substitute', limitations=['This is authentic asset/placement closure, not renderer or gameplay validation.', 'Authored two-pass ProjectedPrelit requires explicit native runtime support; passes may not be silently dropped.'])
    exported = dict(schema='RR_AUTHORED_MESH49G_V1', success=True, container=GA, archive_sha256=GA_SHA, scene_sha256=SCB_SHA, world_instances=placed, vertices=vertex_count, indices=len(indices), mesh_sha256=sha(mesh), arithmetic='Ordered binary64 multiply/add and one binary32 cast. Local geometry uses SCB root replacement. Original world-space flag bypasses SCB pose, matching source 0x240B04 and0x241678.')
    selected_bytes, export_bytes = jb(selection), jb(exported)
    (output/'asset-selection.json').write_bytes(selected_bytes)
    (placed_dir/'export-report.json').write_bytes(export_bytes)
    source_dir = args.output_root/'evidence/texturesGA'
    source_dir.mkdir(parents=True, exist_ok=True)
    (source_dir/'manifest.json').write_bytes(jb(dict(schema='RR_ORIGINAL_GA_SOURCE_TEXTURES_V1', archive_sha256=GA_SHA, textures=source_textures, limitations=['Genuine authored initial frames; live timing and the remaining original frames belong to the separate GA ATM owner.'])))
    include = '\n'.join(' {"%s", "%s", "%s", "%s", %du, %du, %du, %du},' % (r['member'], r['source_sha256'], r['output_filename'], r['output_sha256'], r['source_bytes'], r['width'], r['height'], r['mip_index']) for r in source_textures)+'\n'
    (args.output_root/'tests/native/source_texture_ga_index.inc').write_text(include)
    stats = dict(archive_bytes=len(archive), archive_sha256=GA_SHA, selection_sha256=sha(selected_bytes), selection_bytes=len(selected_bytes), export_sha256=sha(export_bytes), export_bytes=len(export_bytes), mesh_sha256=sha(mesh), mesh_bytes=len(mesh), vertices=vertex_count, indices=len(indices), scenes=len(scenes), instances=len(placed), surfaces=sum(len(r['surfaces']) for r in placed), materials=len(materials), texture_names=len(textures), native_textures=sum(r['status']=='resolved' for r in textures), source_atm=sum(r['frame_count'] > 1 for r in source_textures), dynamic_source_pst=sum(r.get('dynamic_owner_source', False) for r in source_textures), non_drawn=len(nondrawn), max_world_instance=max(r['world_instance_index'] for r in placed), material_passes=sum(len(r['native']['passes']) for r in materials), projected_materials=sum(any(p['program']=='ProjectedPrelit' for p in r['native']['passes']) for r in materials), draw_spans=sum(len(next(m for m in materials if m['name']==s['material'])['native']['passes']) for r in placed for s in r['surfaces']))
    need(len(placed) == 563 and len(nondrawn) == 190, 'GA exact authored static draw closure')
    need(sum(norm(row['collision_member']) in inventory_index for row in world['instances']) == 450,
         'GA exact authored static collision closure')
    need(stats['vertices'] <= 500000 and stats['indices'] <= 1500000 and
         stats['texture_names'] <= 128 and stats['draw_spans'] <= 4096, 'GA fixed native budgets')
    profile = ('    \"TRACKS/GA.RES\",\n'
               '    \"%s\",\n    \"%s\",\n    \"%s\",\n'
               '    %d,%d,%d,%d,%d,%d,%d,%d,970,2834942,32u*1024u*1024u\n') % (
                   stats['selection_sha256'], stats['export_sha256'], GA_SHA,
                   stats['vertices'], stats['indices'], stats['scenes'], stats['materials'],
                   stats['instances'], stats['surfaces'], stats['draw_spans'], stats['max_world_instance'])
    (args.output_root/'tests/native/ga_visual_profile.inc').write_text(profile)
    stats['source_texture_codec'] = sky_and_texture_codec
    stats['generated_profile_sha256'] = sha(profile.encode())
    (output/'assembly-manifest.json').write_bytes(jb(stats))
    print(json.dumps(stats))


if __name__ == '__main__':
    main()
