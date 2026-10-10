"""Independent original-header/48B comparisons and bounded-parser adversarial tests."""
import collections, copy, json, struct, sys
from pathlib import Path
sys.dont_write_bytecode=True
from decode_static48b import decode_psg3
from decode_scene48c import (ROOT, IDENTITY, FormatError, sha, mesh_bytes, load_container,
    decode_psg3_scene, export_lod, strip_topology, transform, matmul)

checks=0
def check(ok, label):
    global checks
    checks+=1
    if not ok:raise AssertionError(label)

def rejects(data, label, fn=decode_psg3_scene):
    try:fn(data)
    except FormatError:check(True,label);return
    raise AssertionError('accepted invalid '+label)

def mutation(data,offset,fmt,*values):
    out=bytearray(data);struct.pack_into(fmt,out,offset,*values);return bytes(out)

def main():
    ta,tac=load_container(ROOT/'work/native/project/disc/TRACKS/TA.RES')
    pod,podc=load_container(ROOT/'work/native/project/disc/PODS/POD01/POD01.RES')
    check(tac['sha256']=='b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8','TA source digest')
    check(podc['sha256']=='08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3','POD source digest')
    old_counts=collections.Counter();old_split_names=[];split_count=0;scene_rows=[];scenes={}
    for collection,members in [('TA',ta),('POD01',pod)]:
        for name,(data,record) in members.items():
            if not name.endswith('.psg'):continue
            try:scene=decode_psg3_scene(data)
            except FormatError as exc:
                check(collection=='TA' and name in ('anim\\xsailbarge.psg','anim\\x_sab.psg') and 'unsupported attribute command' in str(exc),'known unbroadened rejection')
                continue
            scenes[name]=(data,scene)
            check(len(scene['nodes'])==struct.unpack_from('<I',data,8)[0],'original hierarchy count')
            # Independent original AABB records, rather than calculated output
            # counts, constrain every rigid bone and all LODs of each member.
            for node,result in zip(scene['nodes'],scene['bounds_checks']):
                bb=struct.unpack_from('<7fi',data,12+224*node['index']+192)
                check(node['parent']==bb[7],'parent record identity')
                check(result['max_error']<=scene['quantization'][0]*1.01+0.002,'stored per-bone AABB invariant')
            for lod in scene['lods']:
                for surface in lod['surfaces']:
                    mat,comp,uv,nv,ns,nb=struct.unpack_from('<IIfHHI',data,surface['header_offset'])
                    topology=surface['topology'];indices=surface['indices']
                    check(surface['material_index']==mat and surface['material']==scene['materials'][mat],'original material mapping')
                    check(len(surface['vertices_local'])==nv,'original debug vertex count')
                    check(topology['packed_strips']-len(topology['continuations'])==ns,'original debug strip reconciliation')
                    check(len(indices)==3*(nv-2*topology['packed_strips']),'independent ADC emission count')
                    check(all(0<=i<nv for i in indices),'triangle bounds')
                    mixed=sum(len({surface['bone_indices'][i] for i in indices[j:j+3]})>1 for j in range(0,len(indices),3))
                    check(mixed==surface['normal_alignment'].get('mixed_bone_unchecked',0),'local-normal diagnostic excludes triangles spanning bone spaces')
                    check((comp&255==15)==all(c is not None for c in surface['original_colors']),'original color presence')
            v,ix,_=export_lod(scene)
            check(len(mesh_bytes(v,ix))==16+24*len(v)+4*len(ix),'RRMESH01 exact extent')
            if collection=='TA':
                try:oldv,oldix,oldreport=decode_psg3(data)
                except FormatError as exc:
                    if 'strip count' in str(exc):
                        old_split_names.append(name)
                        split_count+=sum(len(s['topology']['continuations']) for lod in scene['lods'] for s in lod['surfaces'])
                    old_counts['old_rejected']+=1
                else:
                    old_counts['old_accepted']+=1
                    check(mesh_bytes(v,ix)==mesh_bytes(oldv,oldix),'48B compatible geometry remains byte-identical')
            scene_rows.append({'collection':collection,'name':name,'sha256':sha(data),'nodes':len(scene['nodes']),'lods':len(scene['lods'])})
    check(old_counts=={'old_rejected':88,'old_accepted':140},'independent 48B coverage baseline')
    check(len(old_split_names)==47 and split_count==166,'all 47 historical count discrepancies explained by 166 exact continuations')
    check(len(scene_rows)==241,'228 TA + 13 POD01 supported semantic candidates')

    # A hand-created long strip is split at16 with14 new vertices per chunk.
    # Expected triangle indices are written separately; neither file parser nor
    # exported result is used as the oracle. Changing ANY repeated attribute
    # destroys the proven continuation and must reject the source-strip count.
    xyz=[(i,i%2,0,-16 if i<2 else 16) for i in range(19)]
    g={'xyz':xyz[:16],'normal':[(0,0,127)]*16,'uv':[(i,2*i) for i in range(16)],'color':[(1,2,3,4)]*16}
    h={'xyz':[(i,i%2,0,-16 if i<16 else 16) for i in range(14,19)],'normal':[(0,0,127)]*5,'uv':[(i,2*i) for i in range(14,19)],'color':[(1,2,3,4)]*5}
    ix,tp=strip_topology([g,h],1)
    expected=[]
    for base,n in [(0,16),(16,5)]:
        for t in range(n-2):expected.extend([base+t,base+t+1,base+t+2] if t%2==0 else [base+t+1,base+t,base+t+2])
    check(ix==expected and len(tp['continuations'])==1,'analytic long-strip winding and split')
    for field in ['xyz','normal','uv','color']:
        bad=copy.deepcopy(h);point=list(bad[field][0]);point[0]+=1;bad[field][0]=tuple(point)
        rejects([g,bad],'continuation '+field,lambda value:strip_topology(value,1))
    rejects([g,h],'wrong strip count',lambda value:strip_topology(value,2))
    # Matrix order is observable with rotation plus translation. Independent
    # step-by-step result: (2,3,4)->(-3,2,4)->(7,22,34).
    rotate=(0.,1.,0.,0.,-1.,0.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.)
    translate=IDENTITY[:12]+(10.,20.,30.,1.)
    check(transform((2,3,4),matmul(rotate,translate))==(7.,22.,34.),'analytical child then parent transform')

    # Strict-prefix tests target the real hierarchy, material, surface header,
    # payload command types/sampled boundaries and matrix-palette extent. Full bytewise
    # truncation is retained for the small48B fixture; large pod tests are
    # bounded by semantic field/command boundaries to avoid quadratic work.
    ground=ta['geometry\\zone01groundstrips16.psg'][0]
    for size in range(len(ground)):rejects(ground[:size],f'ground prefix{size}')
    for name in ['pods\\pod01\\chariot.psg','pods\\pod01\\englt1.psg','geometry\\ms_siloz.psg']:
        data,scene=scenes[name];prefixes={0,4,8,len(data)-1}
        for n in scene['nodes']:
            for delta in [0,127,128,191,192,219,220,223]:prefixes.add(n['header_offset']+delta)
        for lod in scene['lods']:
            for surface in lod['surfaces']:
                start=surface['header_offset'];prefixes.update([start,start+3,start+19,start+20])
                commands=surface['topology']['commands']
                selected={0,len(commands)-1}|set(range(min(12,len(commands))))|set(range(0,len(commands),37))
                for ci in selected:
                    command=commands[ci];prefixes.update([command['offset'],command['offset']+3])
                for offset,fmt,value in [(start,'<I',len(scene['materials'])),(start+4,'<I',0),(start+8,'<f',float('nan')),(start+12,'<H',0),(start+14,'<H',0),(start+16,'<I',0xfffffff0)]:
                    rejects(mutation(data,offset,fmt,value),'surface metadata')
        for size in sorted(prefixes):rejects(data[:size],f'{name} prefix{size}')
        rejects(data+b'\0'*4,'unconsumed data')
        rejects(mutation(data,232,'<i',0),'self-parent cycle')
        rejects(mutation(data,140,'<f',float('inf')),'nonfinite matrix')
        rejects(mutation(data,152,'<f',1),'nonaffine matrix')
        rejects(mutation(data,204,'<f',1e8),'independent stored bounds mismatch')
        if scene['matrix_palette']:
            at=scene['matrix_palette']['offset']
            rejects(mutation(data,at,'<I',2),'unsupported palette count')
            rejects(mutation(data,at+8,'<B',1),'palette bone identity contradiction')
            rejects(mutation(data,len(data)-4,'<I',0),'palette actual count mismatch')

    original=pod['pods\\pod01\\pod01.phy'][0].decode('ascii')
    check('Chariot = chariot.slt' in original and 'LeftEngine = englt1.slt' in original and 'RightEngine = engrt1.slt' in original,'original authored pod selection')
    assembly=json.loads((ROOT/'evidence/assets48C/POD01/assembly.json').read_text())
    check(assembly['vertices']==5755 and assembly['triangles']==3571 and not assembly['missing_parts'],'three component assembly totals')
    for p in assembly['parts']:
        data,scene=scenes[p['member']]
        check(sha(data)==p['member_sha256'] and len(scene['lods'])==4,'authored full model source and four LODs')
    report={'status':'PASS','checks':checks,'compatible_48b_members':140,'previous_strip_rejections_resolved':47,'exact_continuations_in_those_members':166,
            'candidate_members':scene_rows,'limitations':['No executed original hierarchy/render validation.','Original shading, runtime pose, placement and collision remain separate acceptance gates.']}
    (ROOT/'evidence/assets48C/tests.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='candidate_members'}))

if __name__=='__main__':main()
