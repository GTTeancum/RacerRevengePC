"""Typed tree fixtures and expected contacts from immutable original words."""
import json,struct,hashlib,random,math
from pathlib import Path
from query_extract48h import ROOT,RAM,ELF
from query_oracle48h import Original,bits,fp,f32
OUT=ROOT/'evidence/query48I';OUT.mkdir(parents=True,exist_ok=True)
LEAF=0x80000000
IDENTITY=[1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.]

def read_mesh(data,shape):
    u=lambda a:struct.unpack_from('<I',data,a)[0]
    h=lambda a:struct.unpack_from('<H',data,a)[0]
    f=lambda a:struct.unpack_from('<f',data,a)[0]
    vec=lambda a:list(struct.unpack_from('<3f',data,a))
    np,lp=u(shape+0x114),u(shape+0x118)
    nodes={};faces={};pending=[h(shape+0x10c)]
    assert pending==[0]
    while pending:
        tag=pending.pop();i=tag&0x7fff
        if tag&0x8000:
            assert i not in faces
            a=lp+i*20
            faces[i]={'d':f(a+8),'normal':list(struct.unpack_from('<3h',data,a+12)),'vertices':[h(a+x) for x in (0,2,4)]}
        else:
            assert i not in nodes
            a=np+i*36;children=[h(a+8),h(a+10)]
            nodes[i]={'volume':f(a),'center':vec(a+12),'half':vec(a+24),'children':[(LEAF if x&0x8000 else 0)|(x&0x7fff) for x in children]}
            pending.extend(reversed(children))
        assert len(nodes)+len(faces)<65536
    assert set(nodes)==set(range(len(nodes))) and set(faces)==set(range(len(faces)))
    vertexcount=max(max(f['vertices']) for f in faces.values())+1
    vertices=[vec(u(shape+0x100)+i*12) for i in range(vertexcount)]
    normals=[vec(u(shape+0x104)+i*12) for i in range(vertexcount)] if u(shape+0x104) else []
    return {'vertices':vertices,'normals':normals,'nodes':[nodes[i] for i in range(len(nodes))],'faces':[faces[i] for i in range(len(faces))],'world':list(struct.unpack_from('<16f',data,shape)),'source_shape':shape}

def run_original(meshes,query,candidates):
    class TraceOriginal(Original):
        def __init__(self):super().__init__(bytes(0x2000000));self.trace=[];self.current_face=0;self.current_id=0;self.selected=None;self.pair=None
        def execute(self,pc):
            if pc==0x107170:
                tag=self.g[5][0]&65535
                self.trace.append([self.current_id,tag&0x7fff,bool(tag&0x8000),False])
            if pc==0x107368:self.trace[-1][3]=bool(self.g[2][0])
            if pc==0x10724c:self.trace[-1][3]=bool(self.g[2][0])
            if pc==0x1082b0:self.current_face=(self.g[4][0]-self.face_base)//20
            if pc==0x1088d4:self.selected=[self.current_id,self.current_face]
            if pc in (0x1087a4,0x1087e8):self.pair=[self.current_id,self.current_face]
            return super().execute(pc)
    m=TraceOriginal();m.g[28][0]=0x3dd970
    executable=ELF.read_bytes()
    assert hashlib.sha256(executable).hexdigest()=='c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30'
    for a in range(0x388af0,0x388af0+7*4,4):m.write(a,struct.unpack_from('<I',executable,a-0x100000+0x80)[0])
    queryobj,shape,result=0x1000,0x2000,0x3000
    m.write(queryobj+0x88,2);m.write(queryobj+0x8c,shape);m.write(queryobj+0x90,result)
    m.vector(shape,query['matrix']);m.write(shape+0x40,0x8000,2);m.write(shape+0x48,1);m.write(shape+0x50,0x2100);m.write(shape+0x54,1)
    m.vector(0x2100,[*query['first'],1.]);m.vector(0x2110,[*query['second'],1.])
    m.write(0x3df5b8,result);m.storef(result+0x74,1.)
    for offset,pointer in [(0x50,0x3100),(0x54,0x3200),(0x58,0x3300),(0x64,0x3400),(0x6c,0x3500),(0x70,0x3600)]:m.write(result+offset,pointer)
    m.write(result+0x5c,3)
    for ci,c in enumerate(candidates):
        mesh=meshes[c['mesh']];obj=0x4000;target=0x5000;vbase=0x10000;nbase=0x20000;nodebase=0x40000;facebase=0x80000
        m.current_id=c['id'];m.face_base=facebase
        m.write(obj+0x88,0);m.write(obj+0x8c,target);m.vector(target,c['matrix'])
        m.write(target+0x100,vbase);m.write(target+0x104,nbase if mesh['normals'] else 0);m.write(target+0x108,0);m.write(target+0x10c,0,2)
        m.write(target+0x114,nodebase);m.write(target+0x118,facebase)
        for i,v in enumerate(mesh['vertices']):m.vector(vbase+i*12,v)
        for i,v in enumerate(mesh['normals']):m.vector(nbase+i*12,v)
        for i,node in enumerate(mesh['nodes']):
            a=nodebase+i*36;m.storef(a,node['volume']);m.vector(a+12,node['center']);m.vector(a+24,node['half'])
            for k,ch in enumerate(node['children']):m.write(a+8+k*2,(0x8000 if ch&LEAF else 0)|(ch&0x7fffffff),2)
        for i,face in enumerate(mesh['faces']):
            a=facebase+i*20;m.storef(a+8,face['d'])
            for k,x in enumerate(face['vertices']):m.write(a+k*2,x,2)
            for k,x in enumerate(face['normal']):m.write(a+12+k*2,x,2)
        m.g[4][0]=queryobj;m.g[5][0]=obj;m.run(0x121410)
    hit=m.selected is not None
    selected=[m.read(result+off+i*4) for off in (0x30,0x20,0x40) for i in range(3)]+[m.read(result+0x74)]
    pair=[m.read(p+i*4) for p in (0x3300,0x3200) for i in range(3)]+[m.read(0x3500)]
    return {'hit':hit,'selected':selected,'selected_ids':m.selected or [0,0],'pair':pair,'pair_ids':m.pair or [0,0],'trace':m.trace,'instructions':m.steps}

def main():
    data=RAM.read_bytes();assert hashlib.sha256(data).hexdigest()=='38682d69c2e2cc08ad1a52367a5198f88ec8624d232c2195fb267050928d3e48'
    meshes=[read_mesh(data,a) for a in (0x9e7e40,0x9f7460)]
    # Two identical position triangles with distinct vertex normals establish
    # global-last-tie versus per-pair-first-tie semantics independently.
    vertices=[[-2.,0.,-2.],[0.,0.,2.],[2.,0.,-2.]]*2
    synthetic={'vertices':vertices,'normals':[[0.,1.,0.]]*3+[[.25,1.,.125]]*3,
      'nodes':[{'volume':64.,'center':[0.,0.,0.],'half':[2.,.25,2.],'children':[LEAF,LEAF|1]}],
      'faces':[{'d':0.,'normal':[0,32767,0],'vertices':[0,1,2]},{'d':0.,'normal':[0,32767,0],'vertices':[3,4,5]}],'world':IDENTITY,'source_shape':None}
    meshes.append(synthetic)
    base=list(struct.unpack_from('<16f',data,0xe6f3d0));cases=[]
    def add_case(name,q,candidates):
        expected=run_original(meshes,q,candidates);cases.append({'name':name,'query':q,'candidates':candidates,'expected':expected})
    baseq={'first':[0.,3.,0.],'second':[0.,-28.,0.],'matrix':base}
    cands=[{'mesh':i,'matrix':mesh['world'],'id':i+1} for i,mesh in enumerate(meshes[:2])]
    add_case('saved47W',baseq,cands)
    saved=cases[-1]['expected'];assert saved['hit']
    expected_saved=[struct.unpack_from('<I',data,0x9512e0+off+i*4)[0] for off in (0x30,0x20,0x40) for i in range(3)]+[struct.unpack_from('<I',data,0x951354)[0]]
    assert saved['selected']==expected_saved,(saved['selected'],expected_saved)
    rng=random.Random(0x48_49)
    for i in range(30):
        matrix=base.copy()
        for k in range(3):matrix[12+k]=f32(matrix[12+k]+rng.uniform(-80,80))
        add_case('saved-world-offset-'+str(i),{'first':[0.,3.,0.],'second':[0.,-28.,0.],'matrix':matrix},cands if i%2==0 else list(reversed(cands)))
    q={'first':[.25,1.,.125],'second':[.25,-1.,.125],'matrix':IDENTITY}
    add_case('equal-fraction-different-normals',q,[{'mesh':2,'matrix':IDENTITY,'id':3}])
    add_case('equal-fraction-different-candidates',q,[{'mesh':2,'matrix':IDENTITY,'id':4},{'mesh':2,'matrix':IDENTITY,'id':5}])
    # Exact separating-axis boundary and just outside, without smooth vertex extremes.
    for x in [1.75,2.,2.000000238418579,3.]:
        add_case('box-boundary-'+str(x),{'first':[x,1.,-.75],'second':[x,-1.,-.75],'matrix':IDENTITY},[{'mesh':2,'matrix':IDENTITY,'id':3}])
    # Exercise transpose-relative math with rotated targets, then deliberately
    # nonorthogonal targets. The original does not replace transpose by inverse.
    for axis in range(3):
        for angle,stretch in [(.4,1.),(-.7,1.03)]:
            matrix=IDENTITY.copy();a=(axis+1)%3;b=(axis+2)%3
            matrix[a*4+a]=f32(math.cos(angle)*stretch);matrix[b*4+b]=f32(math.cos(angle))
            matrix[a*4+b]=f32(math.sin(angle)*stretch);matrix[b*4+a]=f32(-math.sin(angle))
            matrix[12:15]=[11.25,-20.5,30.75]
            add_case('rotated-target-'+str(axis)+'-'+str(stretch),{'first':[.25,1.,.125],'second':[.25,-1.,.125],'matrix':matrix},[{'mesh':2,'matrix':matrix,'id':6}])
    report={'original_RAM_sha256':hashlib.sha256(data).hexdigest(),'oracle':'unchanged tools/gameplay/query_oracle48h.py with exact original48H words','mesh_sizes':[{'vertices':len(m['vertices']),'normals':len(m['normals']),'nodes':len(m['nodes']),'faces':len(m['faces'])} for m in meshes],'cases':cases}
    (OUT/'fixtures.json').write_text(json.dumps(report,indent=2)+'\n')
    (OUT/'meshes.json').write_text(json.dumps(meshes,indent=2)+'\n')
    words=[0x49383452,1,len(meshes)]
    def floats(xs):words.extend(bits(x) for x in xs)
    for m in meshes:
        words.extend([len(m['vertices']),len(m['normals']),len(m['nodes']),len(m['faces'])]);floats(IDENTITY);words.append(0)
        for v in m['vertices']+m['normals']:floats(v)
        for n in m['nodes']:floats([n['volume'],*n['center'],*n['half']]);words.extend(n['children'])
        for f in m['faces']:floats([f['d']]);words.extend(x&0xffffffff for x in f['normal']);words.extend(f['vertices'])
    words.append(len(cases))
    for c in cases:
        floats([*c['query']['first'],*c['query']['second'],*c['query']['matrix']]);words.append(len(c['candidates']))
        for cand in c['candidates']:words.extend([cand['mesh'],cand['id']]);floats(cand['matrix'])
        e=c['expected'];words.append(int(e['hit']));words.extend(e['selected']);words.extend(e['selected_ids']);words.extend(e['pair']);words.extend(e['pair_ids']);words.append(len(e['trace']))
        for cid,index,leaf,accepted in e['trace']:words.extend([cid,index,int(leaf),int(accepted)])
    raw=struct.pack('<'+'I'*len(words),*words);(OUT/'fixtures.bin').write_bytes(raw)
    lines=['#pragma once','#include <cstdint>','static const std::uint32_t queryFixtureData48I[] = {']
    for i in range(0,len(words),12):lines.append(','.join(f'0x{w:08x}u' for w in words[i:i+12])+',')
    lines.append('};');(ROOT/'tools/gameplay/query_fixture48i.h').write_text('\n'.join(lines)+'\n')
    print(json.dumps({'cases':len(cases),'hits':sum(c['expected']['hit'] for c in cases),'fixture_words':len(words),'mesh_sizes':report['mesh_sizes'],'saved_selected_words':[f'{w:08X}' for w in saved['selected']]}))

if __name__=='__main__':main()
