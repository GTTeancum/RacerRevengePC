"""Offline PSG3 scene recovery: proven split strips, surfaces, LODs and rigid hierarchy.

48B remains frozen. No PS2 execution, packet playback, guessed scene placements,
supplied tool execution, or synthesized game geometry. Neutral mesh exports are
diagnostic; material/bone/LOD semantics remain in the accompanying JSON.
"""
from pathlib import Path
import argparse, collections, configparser, json, math, struct, sys
sys.dont_write_bytecode = True
from decode_static48b import ROOT, FormatError, require, unpack, string, sha, mesh_bytes

IDENTITY = (1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.)
MAX_VERTICES = 1_000_000


def matmul(a, b):
    return tuple(sum(a[r*4+k]*b[k*4+c] for k in range(4)) for r in range(4) for c in range(4))


def transform(p, m):
    return tuple(sum(p[k]*m[k*4+j] for k in range(3))+m[12+j] for j in range(3))


def crossdot(a,b,c,n):
    u=[b[j]-a[j] for j in range(3)];v=[c[j]-a[j] for j in range(3)]
    return ((u[1]*v[2]-u[2]*v[1])*n[0] + (u[2]*v[0]-u[0]*v[2])*n[1] + (u[0]*v[1]-u[1]*v[0])*n[2])


def bounds(points):
    return [min(p[j] for p in points) for j in range(3)], [max(p[j] for p in points) for j in range(3)]


def strip_topology(groups, expected_source_strips):
    strips=[];indices=[];continuations=[];base=0
    for gi,g in enumerate(groups):
        start=0;n=len(g['xyz'])
        while start<n:
            require(start+2<n and g['xyz'][start][3]<0 and g['xyz'][start+1][3]<0,
                    'strip needs exactly two suppressed initial vertices')
            end=start+2
            while end<n and g['xyz'][end][3]>0:
                ids=[base+end-2,base+end-1,base+end]
                if (end-start)&1:ids[0],ids[1]=ids[1],ids[0]
                indices.extend(ids);end+=1
            require(end>start+2,'empty source strip')
            attributes=[]
            for i in range(start,end):
                attributes.append((g['xyz'][i][:3],abs(g['xyz'][i][3]),g['normal'][i],g['uv'][i],g['color'][i]))
            current={'group':gi,'start':start,'end':end,'vertex_start':base+start,'attributes':attributes}
            if strips:
                previous=strips[-1]
                # Original exporter splits long strips at 16 vertices, repeats
                # exactly two endpoint vertices, and moves to the next packet.
                # Require every attribute to match, not position alone.
                if (previous['end']-previous['start']==16 and previous['start']==0 and
                        previous['group']+1==gi and start==0 and
                        previous['attributes'][-2:]==attributes[:2]):
                    continuations.append({'previous_strip':len(strips)-1,'next_strip':len(strips),
                                          'previous_group':previous['group'],'next_group':gi,
                                          'repeated_attributes_sha256':sha(repr(attributes[:2]).encode())})
            strips.append(current);start=end
        base+=n
    require(len(strips)-len(continuations)==expected_source_strips,
            'source strip count not explained by exact 16-vertex continuations')
    require(len(indices)==3*(base-2*len(strips)),'triangle emission reconciliation')
    return indices,{'packed_strips':len(strips),'source_strips':expected_source_strips,
                    'continuations':continuations,
                    'strip_spans':[{k:v for k,v in s.items() if k!='attributes'} for s in strips]}


def decode_payload(blob, start, end, components, expected_vertices, expected_strips, node_count,
                   allow_signed32_quantized_positions=False):
    pos=start;groups=[];commands=[];vertex_count=0
    def take(cmd,count,address,fmt):
        nonlocal pos
        at=pos;word=unpack('<I',blob,pos)[0];pos+=4
        require(word==(cmd<<24)|(count<<16)|address,f'unsupported attribute command at {at}: {word:08x}')
        size=struct.calcsize(fmt)*count;padded=(size+3)&~3
        require(pos+padded<=end,'attribute extent beyond surface')
        values=[unpack(fmt,blob,pos+i*struct.calcsize(fmt)) for i in range(count)]
        require(not any(blob[pos+size:pos+padded]),'nonzero attribute padding')
        commands.append({'offset':at,'word':f'{word:08x}','bytes':size,'padding':padded-size})
        pos+=padded;return values
    while pos<end and unpack('<I',blob,pos)[0]:
        tag=take(0x6c,1,0x8000,'<4I')[0];n=tag[0]&0x7fff
        require(3<=n<=16 and tag==(0x8000|n,0x30024000,0x412,0),'unsupported original group template')
        require(vertex_count+n<=expected_vertices,'surface exceeds declared vertex count')
        wide_positions = (allow_signed32_quantized_positions and
                          unpack('<I',blob,pos)[0] == ((0x6c<<24)|(n<<16)|0x8001))
        xyz=take(0x6c if wide_positions else 0x6d,n,0x8001,'<4i' if wide_positions else '<4h')
        if wide_positions:
            require(all(-8388608 <= value <= 8388607 for vertex in xyz for value in vertex[:3]),
                    'V4_32 position outside original signed24 quantized subset')
        require(all(v[3]!=0 and abs(v[3])%16==0 and 1<=abs(v[3])//16<=node_count for v in xyz),
                'invalid rigid bone/ADC value')
        normals=take(0x6a,n,0x8001+n,'<3b')
        uv=take(0x65,n,0x8001+2*n,'<2h')
        color=take(0x6e,n,0xc001+3*n,'<4B') if components==15 else [None]*n
        require(unpack('<I',blob,pos)[0]==0x17000000,'missing surface group MSCNT')
        commands.append({'offset':pos,'word':'17000000','bytes':0,'padding':0});pos+=4
        groups.append({'xyz':xyz,'normal':normals,'uv':uv,'color':color});vertex_count+=n
    require(end-pos<16 and not any(blob[pos:end]),'surface trailing data')
    require(vertex_count==expected_vertices,'surface vertex header count mismatch')
    indices,topology=strip_topology(groups,expected_strips)
    return groups,indices,{'commands':commands,'trailing_padding':end-pos,**topology}


def decode_psg3_scene(blob, allow_signed32_quantized_positions=False):
    require(12<=len(blob)<=8*1024*1024,'unsupported PSG size')
    require(blob[:8]==b'psg\0\x03\0\0\0','expected original PSG3')
    n=unpack('<I',blob,8)[0];require(1<=n<=256,'invalid hierarchy count')
    at=12;nodes=[]
    for i in range(n):
        require(at+224<=len(blob),'truncated hierarchy')
        name=string(blob[at:at+128]);m=unpack('<16f',blob,at+128);bb=unpack('<7fi',blob,at+192)
        require(all(math.isfinite(x) for x in m+bb[:7]),'nonfinite hierarchy')
        require(m[3]==m[7]==m[11]==0 and m[15]==1,'nonaffine hierarchy matrix')
        require(all(x>=0 for x in bb[3:7]),'negative hierarchy extent')
        require(-1<=bb[7]<i,'parent must precede child; cyclic or unsupported hierarchy')
        parent=bb[7];global_m=m if parent<0 else matmul(m,nodes[parent]['bind_matrix'])
        nodes.append({'index':i,'name':name,'matrix':m,'bounds_center_half_radius':bb[:7],
                      'parent':parent,'bind_matrix':global_m,'header_offset':at})
        at+=224
    nm=unpack('<I',blob,at)[0];at+=4;require(1<=nm<=4096,'invalid material count')
    require(at+64*nm<=len(blob),'truncated material names')
    materials=[string(blob[at+64*i:at+64*(i+1)]) for i in range(nm)];at+=64*nm
    quant=unpack('<7fI',blob,at);at+=32
    require(all(math.isfinite(x) for x in quant[:7]) and all(x>0 for x in quant[:4]),'invalid quantization')
    require(quant[7] in (0,1),'invalid world-space flag')
    require(quant[7]==1 or quant[4:7]==(0.,0.,0.),'local geometry with unproven nonzero origin')
    flags=unpack('<I',blob,at)[0];at+=4
    require(flags in (1,4),'only observed rigid one/four LOD layouts supported')
    lods=[];all_local=[[] for _ in nodes];total_vertices=0;all_surfaces=0
    for li in range(flags):
        cutoff,ns=unpack('<fI',blob,at);at+=8
        require(math.isfinite(cutoff) and cutoff>0 and 1<=ns<=4096,'invalid LOD metadata')
        surfaces=[]
        for si in range(ns):
            header_at=at;mat,component_word,uv_size,nv,nstrip,size=unpack('<IIfHHI',blob,at);at+=20
            require(mat<nm,'surface material out of bounds')
            require(component_word in (0x00ff010f,0x00000107),'unsupported surface attribute variant')
            require(math.isfinite(uv_size) and uv_size>0 and 3<=nv<=65535 and 1<=nstrip<=nv//3,'invalid surface metadata')
            require(size>0 and size%16==0 and at+size<=len(blob),'invalid surface payload extent')
            total_vertices+=nv;require(total_vertices<=MAX_VERTICES,'total vertex cap')
            groups,indices,topology=decode_payload(blob,at,at+size,component_word&255,nv,nstrip,n,
                                                  allow_signed32_quantized_positions)
            at+=size;vertices=[];normals=[];bones=[];raw_colors=[]
            for g in groups:
                for xyz,normal,uv,color in zip(g['xyz'],g['normal'],g['uv'],g['color']):
                    bone=abs(xyz[3])//16-1
                    p=tuple(xyz[j]*quant[0]+quant[4+j] for j in range(3))
                    all_local[bone].append(p);normals.append(tuple(k*quant[1] for k in normal));bones.append(bone)
                    raw_colors.append(color)
                    # Missing colors stay absent in semantic evidence. RRMESH01
                    # requires RGBA, so its explicitly neutral diagnostic gets
                    # constant white; this is never labeled original shading.
                    rgba=tuple(color) if color is not None else (255,255,255,255)
                    vertices.append(p+(uv[0]*quant[2],uv[1]*quant[3])+rgba)
            signs=collections.Counter()
            for k in range(0,len(indices),3):
                ids=indices[k:k+3];p=[vertices[x] for x in ids]
                if len({bones[x] for x in ids})!=1:
                    signs['mixed_bone_unchecked']+=1;continue
                normal=[sum(normals[x][a] for x in ids) for a in range(3)]
                d=crossdot(*p,normal);signs['positive' if d>0 else 'negative' if d<0 else 'zero']+=1
            surfaces.append({'header_offset':header_at,'material_index':mat,'material':materials[mat],
                             'component_word':component_word,'uv_size':uv_size,'vertex_count':nv,
                             'triangle_count':len(indices)//3,'vertices_local':vertices,'normals_local':normals,
                             'bone_indices':bones,'original_colors':raw_colors,'indices':indices,
                             'normal_alignment':dict(signs),'topology':topology})
            all_surfaces+=1
        lods.append({'level':li,'cutoff':cutoff,'surfaces':surfaces})
    trailer_at=at;trailer=None
    if n==1:
        require(at==len(blob),'unexpected single-object trailer')
    else:
        # Exact observed old-format simple matrix palette: one palette, padded
        # byte count, identity IDs, zero padding, then actual node count. Do not
        # assume the later CarsTools compressed-palette layout is compatible.
        np,nb=unpack('<II',blob,at);at+=8
        require(np==1 and nb==(n+3)&~3,'unsupported matrix palette header')
        require(at+nb+4==len(blob),'matrix palette extent')
        require(blob[at:at+n]==bytes(range(n)) and not any(blob[at+n:at+nb]),'nonidentity matrix palette unsupported')
        require(unpack('<I',blob,at+nb)[0]==n,'matrix palette count mismatch')
        trailer={'offset':trailer_at,'palette_count':np,'padded_bytes':nb,'ids':list(range(n)),'node_count':n};at+=nb+4
    checks=[]
    for node,points in zip(nodes,all_local):
        bb=node['bounds_center_half_radius'];expected_min=[bb[i]-bb[i+3] for i in range(3)];expected_max=[bb[i]+bb[i+3] for i in range(3)]
        lo,hi=bounds(points) if points else ([0.,0.,0.],[0.,0.,0.])
        error=max(abs(a-b) for a,b in zip(lo+hi,expected_min+expected_max))
        require(error<=quant[0]*1.01+0.002,f'node {node["index"]} local bounds disagree with stored hierarchy')
        checks.append({'node':node['index'],'points':len(points),'min':lo,'max':hi,'max_error':error})
    require(at==len(blob),'unconsumed PSG bytes')
    return {'nodes':nodes,'materials':materials,'quantization':quant,'lod_flags':flags,
            'lods':lods,'matrix_palette':trailer,'bounds_checks':checks,
            'decoded_vertices_all_lods':total_vertices,'surfaces_all_lods':all_surfaces,
            'status':'bounded rigid geometry decoded; original shading, runtime pose and scene visibility not accepted'}


def export_lod(scene, level=0, apply_bind=True):
    vertices=[];indices=[];spans=[]
    for surface in scene['lods'][level]['surfaces']:
        vbase,ibase=len(vertices),len(indices)
        for v,bone in zip(surface['vertices_local'],surface['bone_indices']):
            p=transform(v[:3],scene['nodes'][bone]['bind_matrix']) if apply_bind else v[:3]
            vertices.append(p+tuple(v[3:]))
        indices.extend(i+vbase for i in surface['indices'])
        spans.append({'material_index':surface['material_index'],'material':surface['material'],
                      'vertex_start':vbase,'vertex_count':len(surface['vertices_local']),
                      'index_start':ibase,'index_count':len(surface['indices']),
                      'original_vertex_colors':surface['original_colors'][0] is not None})
    return vertices,indices,spans


def load_container(path):
    require(28<=path.stat().st_size<=64*1024*1024,'RES size cap')
    sys.path.insert(0,str(ROOT/'work/native/project/scripts'))
    try:from validate_ui_resources07 import decode
    finally:sys.path.pop(0)
    raw=path.read_bytes();rs,bs=decode(raw);flat=b''.join(b['data'] for b in bs)
    members={r['name'].lower():(flat[r['logical_offset']:r['logical_offset']+r['bytes']],r) for r in rs}
    require(len(members)==len(rs),'case-insensitive duplicate resource')
    return members,{'path':str(path.resolve()),'sha256':sha(raw),'bytes':len(raw),'decoded_bytes':len(flat)}


def run_inventory(output, rel, label):
    members,container=load_container(ROOT/'work/native/project/disc'/rel)
    target=output/label;target.mkdir(parents=True,exist_ok=True)
    accepted=[];rejected=[];world_v=[];world_i=[];world_spans=[];scenes={}
    for name,(data,record) in members.items():
        if not name.endswith('.psg'):continue
        row={**record,'sha256':sha(data)}
        try:scene=decode_psg3_scene(data)
        except FormatError as exc:row['reason']=str(exc);rejected.append(row);continue
        scenes[name]=(scene,row)
        ordinal=len(accepted);v,ix,spans=export_lod(scene)
        mesh=mesh_bytes(v,ix);meshpath=target/f'{ordinal:03d}.rrmesh';meshpath.write_bytes(mesh)
        require(meshpath.read_bytes()==mesh,'mesh readback mismatch')
        row.update({'output':str(meshpath.resolve()),'output_sha256':sha(mesh),'vertices_lod0':len(v),
                    'triangles_lod0':len(ix)//3,'nodes':len(scene['nodes']),'lods':len(scene['lods']),
                    'surfaces':scene['surfaces_all_lods'],'world_flag':scene['quantization'][7],
                    'continuations':sum(len(s['topology']['continuations']) for lod in scene['lods'] for s in lod['surfaces']),
                    'normal_negative_triangles':sum(s['normal_alignment'].get('negative',0) for lod in scene['lods'] for s in lod['surfaces']),
                    'bounds_max_error':max(c['max_error'] for c in scene['bounds_checks']),
                    'surface_spans_lod0':spans})
        (target/f'{ordinal:03d}.json').write_text(json.dumps({'source':row,'scene':scene},indent=2)+'\n')
        accepted.append(row)
        if (scene['quantization'][7]==1 and len(scene['nodes'])==1 and scene['nodes'][0]['matrix']==IDENTITY and
                row['normal_negative_triangles']==0):
            base=len(world_v);start=len(world_i);world_v.extend(v);world_i.extend(j+base for j in ix)
            world_spans.append({'member':name,'sha256':row['sha256'],'vertex_start':base,'vertex_count':len(v),
                                'index_start':start,'index_count':len(ix),'surface_spans':spans})
    aggregate=None
    if world_v:
        p=target/'world-static-neutral.rrmesh';data=mesh_bytes(world_v,world_i);p.write_bytes(data)
        aggregate={'path':str(p.resolve()),'sha256':sha(data),'vertices':len(world_v),'triangles':len(world_i)//3,
                   'members':world_spans,'bounds':bounds(world_v)}
    report={'container':container,'accepted_candidates':accepted,'rejected':rejected,
            'rejection_reasons':dict(collections.Counter(r['reason'] for r in rejected)),'world_aggregate':aggregate,
            'converter_sha256':sha(Path(__file__).read_bytes()),
            'limitations':['LOD0 bind-pose diagnostics, not gameplay poses.','Local props are not placed into the track.',
                           'Original colors remain absent in semantics when absent on disk; neutral export uses explicit white.',
                           'Opposing vertex normals are reported without silently flipping triangles; such meshes are excluded from world aggregate.']}
    (target/'inventory.json').write_text(json.dumps(report,indent=2)+'\n')
    return report,scenes,members


def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,default=ROOT/'evidence/assets48C')
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    ta,_,_=run_inventory(args.output,'TRACKS/TA.RES','TA')
    pod,scenes,members=run_inventory(args.output,'PODS/POD01/POD01.RES','POD01')
    # The original Pod Setup selects these three components; save its bytes and
    # parsed values rather than guessing a pod from similarly named fragments.
    setup_name=r'pods\pod01\pod01.phy';phy,phyrow=members[setup_name]
    (args.output/'POD01/original-Pod01.phy').write_bytes(phy)
    cp=configparser.ConfigParser(interpolation=None,strict=False);cp.read_string(phy.decode('ascii'))
    parts=[];pv=[];pi=[];missing=[]
    for field in ['Chariot','LeftEngine','RightEngine']:
        selected=cp.get('Pod Setup',field);require(selected.lower().endswith('.slt') and '/' not in selected and '\\' not in selected,'unexpected Pod Setup model')
        member='pods\\pod01\\'+selected[:-4].lower()+'.psg'
        if member not in scenes:missing.append(member);continue
        scene,row=scenes[member];v,ix,spans=export_lod(scene);base=len(pv);start=len(pi);pv.extend(v);pi.extend(i+base for i in ix)
        parts.append({'setup_field':field,'setup_value':selected,'member':member,'member_sha256':row['sha256'],
                      'vertex_start':base,'vertex_count':len(v),'index_start':start,'index_count':len(ix),'surface_spans':spans})
    assembly={'setup_source':{**phyrow,'sha256':sha(phy)},'parts':parts,'missing_parts':missing,
              'transform_policy':'PSG original node matrices composed through parents, rigid bone selection from original position W and validated identity palette; no added placement.',
              'limitations':['Authored bind pose, not running vehicle pose.','Cable effects, runtime damage visibility and animation are not applied.',
                             'Materials remain mapped in surface spans; neutral diagnostic does not claim original shading.']}
    if not missing:
        p=args.output/'POD01/pod01-bind-neutral.rrmesh';data=mesh_bytes(pv,pi);p.write_bytes(data)
        assembly.update({'output':str(p.resolve()),'sha256':sha(data),'vertices':len(pv),'triangles':len(pi)//3,'bounds':bounds(pv)})
    (args.output/'POD01/assembly.json').write_text(json.dumps(assembly,indent=2)+'\n')
    summary={label:{'decoded':len(r['accepted_candidates']),'rejected':len(r['rejected']),
                    'reasons':r['rejection_reasons'],'world_meshes':len(r['world_aggregate']['members']) if r['world_aggregate'] else 0}
             for label,r in [('TA',ta),('POD01',pod)]}
    summary['pod_assembly']={k:v for k,v in assembly.items() if k in ('output','sha256','vertices','triangles','missing_parts','bounds')}
    (args.output/'summary.json').write_text(json.dumps(summary,indent=2)+'\n');print(json.dumps(summary))


if __name__=='__main__':main()
