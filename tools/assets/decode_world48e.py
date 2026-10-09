"""Recover original TA scene placements; export a neutral authored static scene.

Offline evidence only. No guessed placements, guest execution, culling, material
replacement, collision substitute, or original-VU bit-exact arithmetic claim.
"""
from pathlib import Path
import argparse
import collections
import hashlib
import json
import math
import re
import struct
import sys

sys.dont_write_bytecode = True
from decode_scene48c import decode_psg3_scene, mesh_bytes, load_container, IDENTITY
from decode_static48b import FormatError, require

ROOT = Path(__file__).resolve().parents[2]
MAX_PARAMETER_BYTES = 2 * 1024 * 1024


def sha(data):
    return hashlib.sha256(data).hexdigest()


def decode_parameter_blocks(data):
    """Exact bounded BINARY1 labelled-block subset, with reversible byte offsets."""
    require(15 <= len(data) <= MAX_PARAMETER_BYTES, 'parameter file size limit')
    require(data[:7] == b'BINARY\0', 'parameter magic')
    def u32(at):
        require(0 <= at <= len(data) - 4, 'parameter truncated u32')
        return struct.unpack_from('<I', data, at)[0]
    def text(at, count, label, padded=False):
        require(1 <= count <= 512 and 0 <= at <= len(data) - count, label + ' extent')
        raw = data[at:at+count]
        require(b'\0' in raw, label + ' terminator')
        stop = raw.index(0)
        require((padded or stop == count - 1) and not any(raw[stop:]), label + ' suffix')
        require(all(32 <= c < 127 for c in raw[:stop]), label + ' ASCII')
        return raw[:stop].decode('ascii')
    require(u32(7) == 1, 'parameter version')
    count = u32(11)
    require(1 <= count <= 8192, 'parameter block count limit')
    at = 15
    names = []
    for unused in range(count):
        n = u32(at); at += 4
        name = text(at, n, 'block name'); at += n
        require(name and name == name.lower(), 'block name lowercase/empty')
        names.append(name)
    require(names == sorted(set(names)), 'block names must be unique and sorted')
    require(count * 8 <= len(data) - at, 'block offset table extent')
    table = at
    physical_next = table + count * 8
    sections = {}
    for i, name in enumerate(names):
        start, size = u32(table+i*8), u32(table+i*8+4)
        require(start == physical_next and size <= len(data)-start-4,
                'block coverage/extent; unproven data-line variant')
        end = start + 4 + size
        number = u32(start)
        require(1 <= number <= 1024 and number * 43 <= size, 'label count/table extent')
        base = start + 4
        fields = []
        coverage = []
        for j in range(number):
            entry = base + j * 43
            key = text(entry, 35, 'label name', True)
            require(key and key == key.lower(), 'label name lowercase/empty')
            offset, length = u32(entry+35), u32(entry+39)
            require(offset >= number*43 and offset <= size and length <= size-offset,
                    'label value outside block')
            value = text(base+offset, length, 'label value')
            fields.append(dict(name=key, value=value, entry_offset=entry,
                               value_offset=base+offset, value_bytes=length))
            coverage.append((base+offset, base+offset+length))
        require([f['name'] for f in fields] == sorted(set(f['name'] for f in fields)),
                'label names must be unique and sorted')
        next_value = base + number*43
        for first, last in sorted(coverage):
            require(first == next_value, 'label value overlap/gap')
            next_value = last
        require(next_value == end, 'unconsumed block bytes')
        sections[name] = dict(offset=start, size_without_count=size, fields=fields)
        physical_next = end
    require(physical_next == len(data), 'unconsumed parameter file bytes')
    result = dict(version=1, header_table_offset=table, sections=sections)
    require(encode_parameter_blocks(result) == data, 'parameter reversible serialization mismatch')
    return result


def encode_parameter_blocks(parsed):
    """Reproduce validated labelled-block bytes, including original value order."""
    names = list(parsed['sections'])
    out = bytearray(b'BINARY\0' + struct.pack('<II', 1, len(names)))
    for name in names:
        raw = name.encode('ascii')+b'\0'
        out.extend(struct.pack('<I', len(raw))); out.extend(raw)
    table = len(out); out.extend(b'\0'*(8*len(names)))
    for i, name in enumerate(names):
        section = parsed['sections'][name]; fields = section['fields']
        start = len(out); out.extend(struct.pack('<I', len(fields)))
        base = len(out); out.extend(b'\0'*(43*len(fields)))
        locations = {}
        for field in sorted(fields, key=lambda f: f['value_offset']):
            raw = field['value'].encode('ascii')+b'\0'
            locations[field['name']] = (len(out)-base, len(raw)); out.extend(raw)
        for j, field in enumerate(fields):
            key = field['name'].encode('ascii')
            require(len(key) < 35, 'label too long to encode')
            pos = base+j*43
            out[pos:pos+35] = key+b'\0'*(35-len(key))
            struct.pack_into('<II', out, pos+35, *locations[field['name']])
        struct.pack_into('<II', out, table+i*8, start, len(out)-start-4)
    return bytes(out)


def values(section):
    return {f['name']: f['value'] for f in section['fields']}


def f32(value):
    try:
        result = struct.unpack('<f', struct.pack('<f', value))[0]
    except (OverflowError, struct.error):
        raise FormatError('scene vector exceeds binary32')
    require(math.isfinite(result), 'nonfinite scene vector')
    return result


def vector(value):
    parts = value.split(',')
    require(len(parts) == 3 and all(re.fullmatch(r'\s*[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?\s*', p) for p in parts),
            'scene vector syntax')
    return tuple(f32(float(p)) for p in parts)


def cross(a, b):
    return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])


def normalize(v):
    square = (v[0]*v[0] + v[1]*v[1]) + v[2]*v[2]
    require(math.isfinite(square) and square > 1e-20, 'degenerate scene orientation')
    length = math.sqrt(square)
    return tuple(x/length for x in v)


def placement_matrix(position, look, up):
    # Original 27ADEC -> 289EC0 with a3=1,t0=0,t1=0:
    # right0=up X look; look1=right0 X up; normalize up and look1;
    # right=normalized_up X normalized_look. Row-major, row-vector convention.
    right0 = cross(up, look)
    look1 = cross(right0, up)
    up1, look2 = normalize(up), normalize(look1)
    right = cross(up1, look2)
    return right+(0.,)+up1+(0.,)+look2+(0.,)+position+(1.,)


def decode_world(scb):
    parsed = decode_parameter_blocks(scb)
    sections = parsed['sections']
    def section(name):
        require(name in sections, 'missing scene section '+name)
        return values(sections[name])
    def fields(row, required):
        require(all(k in row for k in required), 'missing scene parameter')
        return row
    info = fields(section('sceneinfo'), ['numzones','numportals'])
    def number(text, limit, label):
        require(re.fullmatch(r'\d+', text) is not None, label+' syntax')
        n = int(text); require(0 <= n <= limit, label+' limit'); return n
    nz = number(info['numzones'], 256, 'zone count')
    np = number(info['numportals'], 4096, 'portal count')
    require(nz > 0, 'no zones')
    zones, instances, used = [], [], set()
    for z in range(1, nz+1):
        zone = fields(section(f'zone{z}'), ['numberofstaticmodels'])
        count = number(zone['numberofstaticmodels'], 10000, 'zone model count')
        require(len(instances)+count <= 100000, 'total scene instance cap')
        zones.append(dict(zone=z, original_parameters=zone, models=count))
        for model in range(1, count+1):
            key = f'zone{z}-model{model}'
            row = fields(section(key), ['slt','position','lookvector','upvector','uselighting'])
            original = sections[key]
            require(re.fullmatch(r'[A-Za-z0-9_-]+\.slt', row['slt'], re.IGNORECASE) is not None,
                    'unproven SLT asset-reference syntax')
            position, look, up = (vector(row[k]) for k in ['position','lookvector','upvector'])
            require(row['uselighting'] in ('T','F'), 'unproven lighting flag')
            matrix = placement_matrix(position, look, up)
            stem = row['slt'][:-4].lower()
            instances.append(dict(section=key, zone=z, ordinal=model, original_parameters=row,
                source_block_offset=original['offset'], field_provenance=original['fields'],
                position=position, look=look, up=up, matrix=matrix,
                geometry_member='geometry\\'+stem+'.psg', collision_member='geometry\\'+stem+'.col',
                use_lighting=row['uselighting']=='T'))
            used.add(key)
    require(used == {key for key in sections if re.fullmatch(r'zone\d+-model\d+', key)},
            'scene model headers disagree with zone counts')
    portals = []
    for p in range(1,np+1):
        key = f'portal{p}'; row = fields(section(key), ['connect1','connect2']+['vertex'+str(i) for i in range(1,5)])
        connections = [number(row['connect'+str(i)], nz, 'portal connection') for i in (1,2)]
        require(all(connections) and connections[0] != connections[1], 'invalid portal zones')
        portals.append(dict(section=key, zones=connections, vertices=[vector(row['vertex'+str(i)]) for i in range(1,5)], original_parameters=row))
    return dict(parameter_blocks=parsed, zones=zones, portals=portals, instances=instances,
        arithmetic='Decimal vectors rounded to IEEE binary32; derived orientation uses explicit binary64 scalar operations and sqrt. Not bit-exact PS2 VU RSQRT.',
        root_transform_policy='Original scene loader replaces root translation and orientation; do not multiply the saved PSG root bind into this matrix.')


def transform(point, matrix):
    result=[]
    for j in range(3):
        value=0.0
        for k in range(3):value += point[k]*matrix[k*4+j]
        result.append(value+matrix[12+j])
    require(all(math.isfinite(x) for x in result), 'nonfinite placed point')
    return tuple(result)


def run(output):
    members, container = load_container(ROOT/'work/native/project/disc/TRACKS/TA.RES')
    scb, scb_entry = members[r'tracks\ta.scb']
    tob, tob_entry = members[r'tracks\ta.tob']
    world = decode_world(scb)
    toys = decode_parameter_blocks(tob)
    output.mkdir(parents=True, exist_ok=True)
    (output/'original-ta.scb').write_bytes(scb)
    (output/'original-ta.tob').write_bytes(tob)
    (output/'ta-parameter-blocks.json').write_text(json.dumps(world['parameter_blocks'],indent=2)+'\n')
    (output/'ta-toy-parameter-blocks.json').write_text(json.dumps(toys,indent=2)+'\n')
    scenes={}; vertices=[]; indices=[]; spans=[]; stats=collections.Counter(); used=set(); overwritten=[]
    for instance in world['instances']:
        name=instance['geometry_member']; col=instance['collision_member']
        if name not in members:
            require(col in members, 'unresolved scene asset reference '+name)
            instance.update(status='collision_only', source_sha256=sha(members[col][0]),
                source_entry=members[col][1])
            stats['collision_only_instances']+=1
            continue
        source, entry = members[name]
        if name not in scenes:scenes[name]=decode_psg3_scene(source)
        scene=scenes[name]
        # All original TA static visual references currently satisfy this.
        # Refuse a multi-node prop until its root-replacement path is validated.
        require(len(scene['nodes'])==1 and scene['nodes'][0]['parent']==-1, 'unproven static hierarchy '+name)
        require(all(b==0 for lod in scene['lods'] for s in lod['surfaces'] for b in s['bone_indices']), 'static bone invariant')
        if tuple(scene['nodes'][0]['matrix']) != IDENTITY:overwritten.append(instance['section'])
        instance.update(status='placed_static_geometry', source_sha256=sha(source),source_entry=entry,
            world_quantized=bool(scene['quantization'][7]), original_root_matrix=scene['nodes'][0]['matrix'],
            original_root_node=scene['nodes'][0], original_quantization=scene['quantization'],
            root_was_nonidentity=tuple(scene['nodes'][0]['matrix'])!=IDENTITY,
            materials=scene['materials'])
        if scene['quantization'][7]:
            require(instance['position']==(0.,0.,0.) and instance['look']==(0.,0.,1.) and instance['up']==(0.,1.,0.),
                    'world-quantized placement variant needs separate proof')
        stats['world_quantized_instances' if scene['quantization'][7] else 'local_instances']+=1
        stats['visual_instances']+=1; used.add(name)
        instance_spans=[]
        for si,surface in enumerate(scene['lods'][0]['surfaces']):
            vbase, ibase=len(vertices),len(indices)
            for vertex in surface['vertices_local']:
                vertices.append(transform(vertex[:3],instance['matrix'])+tuple(vertex[3:]))
            indices.extend(vbase+i for i in surface['indices'])
            span=dict(section=instance['section'],zone=instance['zone'],member=name,surface=si,
                material_index=surface['material_index'],material=surface['material'],node_index=0,lod_index=0,
                vertex_start=vbase,vertex_count=len(surface['vertices_local']),index_start=ibase,index_count=len(surface['indices']),
                original_vertex_colors=surface['original_colors'][0] is not None,
                source_normal_alignment=surface['normal_alignment'])
            spans.append(span);instance_spans.append(span)
        instance['surface_spans']=instance_spans
    binary=mesh_bytes(vertices,indices)
    (output/'ta-static-placed-neutral.rrmesh').write_bytes(binary)
    require((output/'ta-static-placed-neutral.rrmesh').read_bytes()==binary,'placed mesh readback')
    all_geometry={n for n in members if n.startswith('geometry\\') and n.endswith('.psg')}
    unhandled_animations=[]
    for key,section in toys['sections'].items():
        if re.fullmatch(r'animation\d+',key):
            row=values(section)
            unhandled_animations.append(dict(section=key,source_block_offset=section['offset'],original_parameters=row,
                status='Original keyed animation and dynamic placement not applied to static export'))
    report=dict(schema='original-static-world48E-v1',container=container,
        scene_source={**scb_entry,'sha256':sha(scb)},toy_source={**tob_entry,'sha256':sha(tob)},
        **{k:v for k,v in world.items() if k!='parameter_blocks'},surface_spans=spans,
        unhandled_animations=unhandled_animations,
        summary={**stats,'zones':len(world['zones']),'portals':len(world['portals']),
            'original_instance_entries':len(world['instances']),'unique_visual_members':len(used),
            'root_matrices_replaced':len(overwritten),'vertices':len(vertices),'triangles':len(indices)//3,
            'bounds_min':[min(v[i] for v in vertices) for i in range(3)],'bounds_max':[max(v[i] for v in vertices) for i in range(3)],
            'unplaced_geometry_members':sorted(all_geometry-used),'mesh_sha256':sha(binary)},
        limitations=['All authored static entries drawn together; original portal/exclusion visibility is retained but not executed.',
            'Neutral LOD0 only; original materials, textures, culling and normal-lighting fidelity remain separate gates.',
            '34 collision-only references retained; no collision geometry or gameplay surface has been synthesized.',
            'Toy animations, moving sailbarge, dynamic actors, sky and race simulation are not assembled.',
            'Source proves transform operations; numeric export is native scalar math, not a replay or bit-exact VU claim.'])
    (output/'world.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(report['summary']))
    return report


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=ROOT/'evidence/worldScene48E')
    run(parser.parse_args().output)
