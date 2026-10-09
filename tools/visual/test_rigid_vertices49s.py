#!/usr/bin/env python3
"""Bounded49S rigid diagnostic projection: source/cache parity and rational oracle.

Creates only the requested output directory. Uses installed Python/GCC; no
downloads. Fixture poses are explicitly supplied arithmetic tests, not recovered
runtime palette producers or original VU vertex-program equivalence.
"""
import argparse
import collections
from fractions import Fraction
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import os
sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/assets'))
from decode_scene48c import decode_psg3_scene, export_lod
from decode_static48b import mesh_bytes

def sha(b): return hashlib.sha256(b).hexdigest()
def f32(x): return struct.unpack('<f',struct.pack('<f',x))[0]
def fbits(x): return struct.pack('<f',x)
def add(a,b): return float(Fraction(a)+Fraction(b))
def mul(a,b): return float(Fraction(a)*Fraction(b))
def dot(p,m):
    out=[]
    for axis in range(3):
        s=0.
        for k in range(3):s=add(s,mul(p[k],m[k*4+axis]))
        out.append(f32(add(s,m[12+axis])))
    return out
def identity(): return [float(i%5==0) for i in range(16)]
class Reader:
    def __init__(self,b):self.b=b;self.p=0
    def take(self,n):b=self.b[self.p:self.p+n];assert len(b)==n;self.p+=n;return b
    def read(self,f):return struct.unpack('<'+f,self.take(struct.calcsize('<'+f)))
    def u(self):return self.read('I')[0]
    def s(self):return self.take(self.u()).decode('ascii')
def cache(b):
    r=Reader(b);assert r.take(8)==b'RRSCENE1';assert r.u()==1
    world,palette,n,m,l,vc,sc=r.read('7I');q=r.read('7d');nodes=[]
    for _ in range(n):
        name=r.s();parent=r.read('i')[0];local=r.read('16d');bind=r.read('16d');bounds=r.read('7d')
        nodes.append(dict(name=name,parent=parent,matrix=local,bind_matrix=bind,bounds_center_half_radius=bounds))
    mats=[r.s() for _ in range(m)];lods=[]
    for _ in range(l):
        cutoff=r.read('d')[0];surfaces=[]
        for _ in range(r.u()):
            mi,col=r.read('2I');uvsize=r.read('d')[0];nv,ni=r.read('2I');vs=[];normals=[];bones=[];colors=[]
            for _ in range(nv):
                p=r.read('3d');normal=r.read('3d');uv=r.read('2d');color=r.read('4B');bone=r.read('H')[0]
                vs.append(p+uv+(color if col else (255,255,255,255)));normals.append(normal);colors.append(color if col else None);bones.append(bone)
            surfaces.append(dict(material_index=mi,uv_size=uvsize,vertices_local=vs,normals_local=normals,bone_indices=bones,original_colors=colors,indices=list(r.read(str(ni)+'I'))))
        lods.append(dict(cutoff=cutoff,surfaces=surfaces))
    spans=[r.read('6I') for _ in range(r.u())];neutral=r.take(r.u());assert r.p==len(b)
    return dict(world=world,palette=palette,nodes=nodes,materials=mats,lods=lods,quantization=q,neutral=neutral,spans=spans,vertices_all_lods=vc,surfaces_all_lods=sc)

def fixtures(out):
    selection=json.loads((ROOT/'evidence/visual49G/asset-selection.json').read_text());cases=[];parts=[];pins={}
    names={'LeftEngine':'48-englt1.psg','RightEngine':'43-engrt1.psg'}
    for req in selection['scene_requests']:
        if req.get('setup_field') not in names:continue
        path=ROOT/'evidence/visual49G'/req['rrscene'];sourcepath=ROOT/'evidence/authoredGeometry49O/asset-reader/fixtures'/names[req['setup_field']]
        cb=path.read_bytes();raw=sourcepath.read_bytes();assert sha(cb)==req['native']['scene_sha256'];assert sha(raw)==req['source_sha256'];pins[str(path.relative_to(ROOT))]=sha(cb);pins[str(sourcepath.relative_to(ROOT))]=sha(raw)
        c=cache(cb);s=decode_psg3_scene(raw);assert c['world']==0 and c['palette']==1
        assert c['materials']==s['materials'] and c['quantization']==s['quantization'][:7]
        assert len(c['nodes'])==len(s['nodes'])==14;assert len(c['lods'])==len(s['lods'])==4
        for cn,sn in zip(c['nodes'],s['nodes']):
            for key in cn:assert cn[key]==sn[key]
        for cl,sl in zip(c['lods'],s['lods']):
            assert cl['cutoff']==sl['cutoff'];assert len(cl['surfaces'])==len(sl['surfaces'])
            for cs,ss in zip(cl['surfaces'],sl['surfaces']):
                for key in cs:assert cs[key]==ss[key]
        assert s['matrix_palette']['ids']==list(range(14));assert mesh_bytes(*export_lod(s)[:2])==c['neutral']
        companion=(ROOT/'evidence/visual49G'/req['rrmesh']).read_bytes();assert companion==c['neutral'] and sha(companion)==req['native']['mesh_sha256']
        pins[req['rrmesh'].replace('cacheobjects/','evidence/visual49G/cacheobjects/')]=sha(companion)
        vertices=[(v[:3],bone) for surf in c['lods'][0]['surfaces'] for v,bone in zip(surf['vertices_local'],surf['bone_indices'])]
        base=[[f32(x) for x in node['bind_matrix']] for node in c['nodes']]
        supplied=[]
        for i,m in enumerate(base):
            # Nonsymmetric finite affine current-world inputs, intentionally
            # external to49Q/49R. Each node differs, exposing palette mistakes.
            p=identity();p[0]=.75;p[1]=.125;p[2]=-.25;p[4]=-.375;p[5]=1.25;p[6]=.0625;p[8]=.5;p[9]=-.125;p[10]=.875
            p[12]=f32(m[12]+i*.375+3);p[13]=f32(m[13]-i*.25-2);p[14]=f32(m[14]+i*.5+1);supplied.append(p)
        for label,palette in [('float_promoted_bind',base),('supplied_distinct_affine',supplied),('identity', [identity() for _ in base])]:
            cases.append(dict(label=req['setup_field']+'_'+label,vertices=vertices,palette=palette))
        parts.append(dict(part=req['setup_field'],vertices=len(vertices),triangles=sum(len(x['indices'])//3 for x in c['lods'][0]['surfaces']),nodes=14,palette=list(range(14)),source_sha256=sha(raw),scene_sha256=sha(cb),all_lods_semantic_equal=True,neutral_byte_equal=True))
    # Independent arithmetic witnesses. Fraction performs exact product/sum;
    # float() rounds each rational result to binary64 before the next step.
    m=identity();m[12]=-1;cases.append(dict(label='preserve_double_local',vertices=[((1+2**-25,0,0),0)],palette=[m]))
    m=identity();m[0]=m[4]=m[8]=1;cases.append(dict(label='ordered_cancellation',vertices=[((2**54,-2**54,1),0)],palette=[m]))
    m=identity();m[4]=1+2**-23;a=1+2**-52;p=mul(a,m[4]);cases.append(dict(label='separate_multiply_add',vertices=[((-p,a,0),0)],palette=[m]))
    m=identity();m[12]=2**-24;cases.append(dict(label='nearest_even_halfway_low',vertices=[((1,0,0),0)],palette=[m]))
    m=identity();m[12]=2**-24;cases.append(dict(label='nearest_even_halfway_high',vertices=[((1+2**-23,0,0),0)],palette=[m]))
    m=identity();cases.append(dict(label='subnormal_output',vertices=[((2**-149,-2**-150,0),0)],palette=[m]))
    buf=bytearray(b'RRRIG49S'+struct.pack('<I',len(cases)))
    descriptions=[]
    for c in cases:
        vs,ps=c['vertices'],c['palette'];buf+=struct.pack('<II',len(vs),len(ps))
        for p in ps:buf+=struct.pack('<16f',*p)
        for v,bone in vs:buf+=struct.pack('<3dH3f',*v,bone,*dot(v,ps[bone]))
        descriptions.append(dict(label=c['label'],vertices=len(vs),palette_count=len(ps)))
    (out/'fixture.bin').write_bytes(buf)
    manifest=dict(parts=parts,cases=descriptions,actual_unique_engine_vertices=sum(p['vertices'] for p in parts),actual_unique_engine_triangles=sum(p['triangles'] for p in parts),fixture_sha256=sha(buf),fixture_bytes=len(buf),input_hashes=pins,oracle='Exact Fraction product/add rounded individually to binary64; final IEEE binary32 rounding. Supplied palettes do not claim original runtime production.')
    (out/'fixture-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    return manifest

def run(command,out,name,env=None,expect=0):
    p=subprocess.run(command,cwd=ROOT,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=90)
    (out/(name+'.stdout')).write_text(p.stdout);(out/(name+'.stderr')).write_text(p.stderr)
    record=dict(command=[str(x) for x in command],exit_code=p.returncode)
    if expect is not None and p.returncode!=expect:raise RuntimeError(name+' failed: '+p.stderr)
    return p,record
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output-dir',type=Path,required=True);ap.add_argument('--sanitizers',action='store_true');ap.add_argument('--mutations',action='store_true');args=ap.parse_args()
    out=args.output_dir.resolve();out.mkdir(parents=True,exist_ok=False)
    manifest=fixtures(out);source=ROOT/'src/game/rigid_vertices49s.cpp';test=ROOT/'tests/native/rigid_vertices49s.cpp'
    flags=['-std=c++17','-O2','-Wall','-Wextra','-Werror','-pedantic','-fno-fast-math','-ffp-contract=off','-frounding-math']
    if args.sanitizers:flags+=['-g','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie']
    env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0:halt_on_error=1';env['UBSAN_OPTIONS']='halt_on_error=1'
    commands=[];exe=out/'rigid_vertices49s'
    _,c=run(['g++',*flags,str(source),str(test),'-o',str(exe)],out,'build');commands.append(c)
    p,c=run([str(exe),str(out/'fixture.bin')],out,'test',env);commands.append(c);result=json.loads(p.stdout)
    obj=out/'rigid_vertices49s.o';_,c=run(['g++',*flags,'-c',str(source),'-o',str(obj)],out,'object');commands.append(c)
    nm,c=run(['nm','-C','-u',str(obj)],out,'undefined');commands.append(c)
    if not args.sanitizers:
        assert not any(x in nm.stdout for x in ['operator new','malloc','calloc','realloc','operator delete','free'])
    mutants=[]
    if args.mutations:
        original=source.read_text()
        variations={
            'ignore_palette': [('palette.matrices[v.paletteIndex]','palette.matrices[0]'),('palette.matrices[input.vertices[i].paletteIndex]','palette.matrices[0]')],
            'transpose_basis':[('matrix.v[k * 4 + axis]','matrix.v[axis * 4 + k]')],
            'omit_translation':[('sum + static_cast<double>(matrix.v[12 + axis])','sum')],
            'reverse_terms':[('for (std::uint32_t k = 0; k < 3; ++k)','for (std::uint32_t k = 3; k-- > 0;)')],
            'float_local':[('vertex.positionLocal[k] * static_cast<double>','static_cast<float>(vertex.positionLocal[k]) * static_cast<double>')],
            'fused_sum':[('sum = sum + product;','sum = std::fma(vertex.positionLocal[k], static_cast<double>(matrix.v[k * 4 + axis]), sum);')],
            'duplicate_world':[('output.positions[i] = position;','RigidVertex49S twice{{position.v[0],position.v[1],position.v[2]},input.vertices[i].paletteIndex};\n        (void)project(twice,palette.matrices[twice.paletteIndex],position,i,d);\n        output.positions[i] = position;')],
        }
        for name,replacements in variations.items():
            code=original
            for before,after in replacements:assert before in code;code=code.replace(before,after)
            mp=out/(name+'.cpp');mp.write_text(code);binary=out/name
            _,c=run(['g++',*flags,'-I'+str(source.parent),str(mp),str(test),'-o',str(binary)],out,name+'-build');commands.append(c)
            p,c=run([str(binary),str(out/'fixture.bin'),'--fixture-only'],out,name,env,expect=None);commands.append(c)
            assert p.returncode!=0 and 'AddressSanitizer' not in p.stderr and 'runtime error:' not in p.stderr
            mutants.append(dict(name=name,killed=True,exit_code=p.returncode,reason=p.stderr.strip()))
    for path,digest in manifest['input_hashes'].items():assert sha((ROOT/path).read_bytes())==digest
    tracked=['src/game/rigid_vertices49s.h','src/game/rigid_vertices49s.cpp','tests/native/rigid_vertices49s.cpp','tools/visual/test_rigid_vertices49s.py']
    report=dict(passed=True,sanitizers=args.sanitizers,leak_sanitizer='disabled, not tested' if args.sanitizers else 'not applicable',result=result,fixture=manifest,mutants=mutants,commands=commands,production_source_sha256={p:sha((ROOT/p).read_bytes()) for p in tracked},consumed_assets_unchanged=True,allocation_evidence='Production has no allocator, callbacks or dynamic storage; normal object undefined symbols checked for allocation/free symbols. No allocation-failure or ownership experiment.')
    (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(dict(passed=True,sanitizers=args.sanitizers,result=result,mutants_killed=len(mutants),report=str(out/'report.json'))))
if __name__=='__main__':main()
