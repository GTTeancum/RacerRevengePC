"""Original COL1/type0 float triangle trees; independent offline collision reference.

This deliberately excludes short-quantized vertices and other collision shape types.
Cars v2 material IDs are absent in COL1; never synthesize them.
"""
from pathlib import Path
import argparse
import hashlib
import json
import math
import struct
from collections import Counter
from decode_scene48c import load_container

ROOT = Path(__file__).resolve().parents[2]


def require(ok, message):
    if not ok:
        raise ValueError(message)


def decode(data):
    require(88 <= len(data) <= 8*1024*1024, 'COL size/truncation')
    require(data[:8] == struct.pack('<I', 1234)+b'col\0', 'COL magic')
    version, kind = struct.unpack_from('<2I', data, 8)
    require((version, kind) == (1, 0), 'unsupported COL version/type')
    twelve = struct.unpack_from('<12f', data, 16)
    matrix = [v for i in range(4) for v in (*twelve[i*3:i*3+3], float(i == 3))]
    reverse, nv, nn, nf, quantized = struct.unpack_from('<5I', data, 64)
    require(reverse in (0, 1) and quantized in (0, 1), 'COL nonboolean flag')
    require(0 < nv <= 65535 and 0 < nn <= 32767 and 1 < nf <= 32768, 'COL count bounds')
    require(quantized == 0, 'unsupported COL short vertex quantization')
    require(nn+1 == nf, 'COL full binary tree count')
    o = 84
    require(o+12*nv+4 <= len(data), 'COL truncated vertices')
    vertices = [list(struct.unpack_from('<3f', data, o+12*i)) for i in range(nv)]
    o += 12*nv
    normals_flag_offset = o
    normals_present, = struct.unpack_from('<I', data, o)
    o += 4
    require(normals_present in (0, 1), 'COL normals flag')
    require(o+12*nv*normals_present+32*nn+22*nf == len(data), 'COL exact extent')
    vertex_normals = [list(struct.unpack_from('<3f', data, o+12*i)) for i in range(nv)] if normals_present else []
    o += 12*nv*normals_present
    nodes_offset = o
    nodes = []
    for i in range(nn):
        row = struct.unpack_from('<7f2H', data, o)
        nodes.append(dict(volume=row[0], center=list(row[1:4]), half=list(row[4:7]),
            children=[(x & 32767) | (0x80000000 if x & 32768 else 0) for x in row[7:]], source_offset=o))
        o += 32
    faces_offset = o
    faces = []
    for i in range(nf):
        row = struct.unpack_from('<4f3H', data, o)
        faces.append(dict(d=row[0], normal=list(row[1:4]), vertices=list(row[4:]), source_offset=o))
        require(max(row[4:]) < nv, 'COL face vertex index')
        o += 22
    numbers = matrix+[x for v in vertices+vertex_normals for x in v]
    numbers += [x for n in nodes for x in [n['volume'], *n['center'], *n['half']]]
    numbers += [x for f in faces for x in [f['d'], *f['normal']]]
    require(all(math.isfinite(v) for v in numbers), 'COL nonfinite value')
    require(all(n['volume'] >= 0 and min(n['half']) >= 0 for n in nodes), 'COL invalid AABB')
    seen, stack, depth = set(), [(0, 0)], 0
    while stack:
        key, level = stack.pop()
        require(key not in seen, 'COL shared/cyclic tree')
        seen.add(key)
        depth = max(depth, level)
        if key & 0x80000000:
            require((key & 0x7fffffff) < nf, 'COL leaf link')
        else:
            require(key < nn, 'COL node link')
            stack.extend((child, level+1) for child in nodes[key]['children'])
    require(len(seen) == nn+nf, 'COL unreachable records')
    return dict(version=version, type=kind, reverse_mode=reverse, quantized=quantized,
        normals_present=normals_present, matrix=matrix, vertices_offset=84,
        normals_flag_offset=normals_flag_offset, nodes_offset=nodes_offset, faces_offset=faces_offset,
        vertices=vertices, vertex_normals=vertex_normals, nodes=nodes, faces=faces, maximum_tree_depth=depth)


def placement(position, look, up):
    def normalized(v):
        length = math.sqrt((v[0]*v[0]+v[1]*v[1])+v[2]*v[2])
        require(math.isfinite(length) and length > 0, 'collision zero/nonfinite axis')
        return [x/length for x in v]
    u, l = normalized(up), normalized(look)
    r = [u[1]*l[2]-u[2]*l[1], u[2]*l[0]-u[0]*l[2], u[0]*l[1]-u[1]*l[0]]
    require(sum(x*x for x in r) > 0, 'collision parallel axes')
    return [*r, 0., *u, 0., *l, 0., *position, 1.]


def diagnostics(c):
    # Independent geometric checks are reports, not an asset repair policy.
    v, faces = c['vertices'], c['faces']
    errors = [(max(abs(sum(f['normal'][j]*v[ix][j] for j in range(3))+f['d']) for ix in f['vertices']), i)
              for i, f in enumerate(faces)]
    normal_error = max(abs(math.sqrt(sum(x*x for x in f['normal']))-1) for f in faces)
    winding = Counter()
    for f in faces:
        a,b,d = (v[i] for i in f['vertices'])
        e=[b[i]-a[i] for i in range(3)];g=[d[i]-a[i] for i in range(3)]
        cross=[e[1]*g[2]-e[2]*g[1],e[2]*g[0]-e[0]*g[2],e[0]*g[1]-e[1]*g[0]]
        alignment=sum(cross[i]*f['normal'][i] for i in range(3))
        winding['positive' if alignment > 0 else 'negative' if alignment < 0 else 'zero'] += 1
    # Compare all descendant vertices with each stored AABB, without expanding it.
    stack=[(0,False)];bounds={};worst=0.
    while stack:
        key, visited=stack.pop()
        if key & 0x80000000:
            vs=[v[i] for i in faces[key & 0x7fffffff]['vertices']]
            bounds[key]=([min(p[j] for p in vs) for j in range(3)], [max(p[j] for p in vs) for j in range(3)])
        elif not visited:
            stack.append((key,True));stack.extend((x,False) for x in c['nodes'][key]['children'])
        else:
            n=c['nodes'][key];bs=[bounds[x] for x in n['children']]
            low=[min(x[0][j] for x in bs) for j in range(3)];high=[max(x[1][j] for x in bs) for j in range(3)]
            bounds[key]=(low,high)
            worst=max(worst,*[n['center'][j]-n['half'][j]-low[j] for j in range(3)],
                      *[high[j]-n['center'][j]-n['half'][j] for j in range(3)])
    return dict(maximum_plane_residual=max(errors)[0], worst_plane_face=max(errors)[1],
        maximum_unit_normal_error=normal_error, winding=dict(winding),
        maximum_stored_aabb_undercoverage=worst, bounds_min=bounds[0][0], bounds_max=bounds[0][1])


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--root',type=Path,default=ROOT)
    parser.add_argument('--out',type=Path,default=ROOT/'evidence/importCollision48G');args=parser.parse_args()
    out=args.out;out.mkdir(parents=True,exist_ok=True)
    members, container=load_container(args.root/'work/native/project/disc/TRACKS/TA.RES')
    rows=[];byname={}
    for name,(data,record) in members.items():
        if not name.endswith('.col'):continue
        sha=hashlib.sha256(data).hexdigest();stem=f'{len(rows):02d}-{Path(name.replace(chr(92),"/")).stem}'
        source=out/(stem+'.col');source.write_bytes(data);assert source.read_bytes()==data
        row=dict(member=record['name'],source_sha256=sha,logical_offset=record['logical_offset'],source_bytes=len(data),source_file=source.name)
        try:
            decoded=decode(data);diag=diagnostics(decoded)
            reference=out/(stem+'.reference.json');reference.write_text(json.dumps(decoded,indent=2)+'\n')
            row.update(accepted=True,reference_file=reference.name,vertices=len(decoded['vertices']),nodes=len(decoded['nodes']),faces=len(decoded['faces']),diagnostics=diag)
            byname[name]=decoded
        except ValueError as exc:row.update(accepted=False,reason=str(exc))
        rows.append(row)
    world_path=args.root/'evidence/worldScene48E/world.json'
    world=json.loads(world_path.read_text())
    raw_parameters=json.loads((args.root/'evidence/worldScene48E/ta-parameter-blocks.json').read_text())
    # The original fallback is only asserted for this exact supplied SCB. A general
    # world loader must evaluate authored COL/StaticType/collision-list overrides.
    for i in range(1,10):
        fields={f['name']:f['value'] for f in raw_parameters['sections'][f'zone{i}']['fields']}
        require('numberofcollisionobjects' not in fields,'TA unexpectedly overrides collision list')
    for instance in world['instances']:
        require('col' not in instance['original_parameters'] and 'statictype' not in instance['original_parameters'],
                'TA unexpectedly overrides collision model')
    bindings=[]
    for instance in world['instances']:
        name=instance['collision_member'].lower()
        if name not in byname:continue
        m=placement(instance['position'],instance['look'],instance['up'])
        bindings.append(dict(section=instance['section'],zone=instance['zone'],ordinal=instance['ordinal'],member=name,
            source_block_offset=instance['source_block_offset'],field_provenance=instance['field_provenance'],
            position=instance['position'],look=instance['look'],up=instance['up'],matrix=m,
            geometry_matrix_difference=max(abs(a-b) for a,b in zip(m,instance['matrix'])),
            material_id=None,material_status='COL1 has no material word; runtime binding not yet recovered'))
    report=dict(schema='collision48G-offline-reference-v1',container=container,members=rows,static_bindings=bindings,
        scene_source_sha256=hashlib.sha256((args.root/'evidence/worldScene48E/original-ta.scb').read_bytes()).hexdigest(),
        independent_world_reference_sha256=hashlib.sha256(world_path.read_bytes()).hexdigest(),
        original_gating=dict(zones_without_collision_count=9,models_without_COL_or_StaticType=318,
            policy='Original missing NumberOfCollisionObjects uses ZoneN-ModelK; missing StaticType=Standard; missing COL=SLT'),
        unmatched_collision_members=sorted(set(byname)-{x['member'] for x in bindings}),
        totals={k:sum(r.get(k,0) for r in rows) for k in ('vertices','nodes','faces')},
        placement_policy='normalize up and look independently; right=up cross look; COL local times scene placement; scalar binary64, not VU-bitexact')
    (out/'reference-index.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(accepted=sum(r['accepted'] for r in rows),total=len(rows),totals=report['totals'],bindings=len(bindings))))


if __name__=='__main__':main()
