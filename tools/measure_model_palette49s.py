#!/usr/bin/env python3
"""49S strict host footprint and actual Pod01 paired palette/rigid subsystem cost."""
import argparse
import json
import math
import os
import resource
import statistics
import struct
import subprocess
import sys
from pathlib import Path
sys.dont_write_bytecode = True
from measure_scene_projection49p import (ROOT, FLAGS, run, digest, dependency_paths,
    snapshot_sources, check_stable_sources, sections)

BASE = ROOT / 'evidence/modelPalette49S'
NEW_UNITS = ['game/model_palette49s', 'game/rigid_vertices49s']
SUPPORT = ['game/scene_graph49p']
KEYS = ['210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3',
        'f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60']
SCENES = ['evidence/visual49G/cacheobjects/' + k + '.rrscene' for k in KEYS]
MODES = ['palette_ready', 'palette_root_setters', 'rigid_ready_palette',
         'combined_ready', 'combined_root_setters']
TYPES = ['SceneNode49P', 'SceneNodes52', 'SceneNodes64', 'SceneGraph49P',
         'BodyMatrix48G', 'ModelPaletteInput49S', 'ModelPalette49S',
         'ModelPaletteWorkspace49S', 'ModelPaletteDiagnostic49S',
         'SourceNodes28', 'PaletteIds28', 'Palettes2', 'PaletteInputs2',
         'RigidVertex49S', 'RigidPosition49S', 'RigidVertexView49S',
         'RigidPaletteView49S', 'RigidPositionView49S', 'RigidVertexDiagnostic49S',
         'RigidVertices2871', 'RigidPositions2871', 'RigidVertices4096',
         'RigidPositions4096', 'RefreshChain64', 'RefreshWorlds64']


def dump(path, value):
    path.write_text(json.dumps(value, indent=2) + '\n')


def footprint(build):
    output = BASE / 'footprint'; output.mkdir(parents=True, exist_ok=True)
    build.mkdir(parents=True, exist_ok=False)
    before = snapshot_sources(); objects = {}; hashes = {}; commands = []; frames = []; reports = []
    for name in [*NEW_UNITS, *SUPPORT]:
        stem = Path(name).name; source = ROOT / 'src' / (name + '.cpp')
        obj, dep = build / (stem + '.o'), build / (stem + '.d')
        command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-c', str(source), '-o', str(obj)]
        commands.append(command); run(command)
        for path in dependency_paths(dep): hashes[path.relative_to(ROOT).as_posix()] = digest(path)
        measured, summary, detail = sections(obj); reports.append(summary)
        (output / (stem + '-sections.txt')).write_text(detail)
        undefined = run(['nm', '-u', '-C', str(obj)])
        measured['direct_allocator_references'] = [s for s in undefined.splitlines() if
            any(t in s for t in ('operator new', 'malloc', 'calloc', 'realloc', 'aligned_alloc'))]
        assert not measured['direct_allocator_references']
        for suffix, content in [('symbols', run(['nm', '-S', '--size-sort', '--radix=d', '-C', str(obj)])),
                                ('undefined', undefined),
                                ('host-disassembly', run(['objdump', '-drC', str(obj)]))]:
            (output / (stem + '-' + suffix + '.txt')).write_text(content)
        measured.update(source_sha256=digest(source), object_sha256=digest(obj)); objects[name] = measured
        su = (build / (stem + '.su')).read_text(); (output / (stem + '.su')).write_text(su)
        for line in su.splitlines():
            f = line.split('\t'); frames.append({'unit':name, 'function':f[0], 'bytes':int(f[1]), 'kind':f[2]})
    code = '''#include "game/model_palette49s.h"
#include "game/rigid_vertices49s.h"
#include <cstdio>
using namespace rr::game;
using SceneNodes52 = SceneNode49P[52];
using SceneNodes64 = SceneNode49P[maxSceneNodes49P];
using SourceNodes28 = SceneNodeIndex49P[28];
using PaletteIds28 = std::uint8_t[28];
using Palettes2 = ModelPalette49S[2];
using PaletteInputs2 = ModelPaletteInput49S[2];
using RigidVertices2871 = RigidVertex49S[2871];
using RigidPositions2871 = RigidPosition49S[2871];
using RigidVertices4096 = RigidVertex49S[4096];
using RigidPositions4096 = RigidPosition49S[4096];
using RefreshChain64 = SceneNodeIndex49P[maxSceneNodes49P];
using RefreshWorlds64 = BodyMatrix48G[maxSceneNodes49P];
int main() {
'''
    code += ''.join(f' std::printf("{t} %zu %zu\\n", sizeof({t}), alignof({t}));\n' for t in TYPES) + '}\n'
    source = build / 'record-sizes.cpp'; source.write_text(code); dep = build / 'record-sizes.d'
    command = ['g++', *FLAGS, '-MMD', '-MF', str(dep), '-I', str(ROOT / 'src'), str(source), '-o', str(build / 'record-sizes')]
    commands.append(command); run(command)
    for path in dependency_paths(dep):
        if path.is_relative_to(ROOT): hashes[path.relative_to(ROOT).as_posix()] = digest(path)
    stdout = run([str(build / 'record-sizes')])
    (output / 'record-sizes.cpp').write_text(code); (output / 'record-sizes.stdout').write_text(stdout)
    (output / 'gnu-size.txt').write_text(''.join(reports))
    records = {r.split()[0]:int(r.split()[1]) for r in stdout.splitlines()}
    alignments = {r.split()[0]:int(r.split()[2]) for r in stdout.splitlines()}
    keys = ['gnu_size_text_including_readonly_and_unwind_bytes','executable_text_bytes','readonly_data_bytes','data_bytes','bss_bytes']
    def frame(fragment):
        rows = [f for f in frames if fragment in f['function']]
        if len(rows) != 1: raise RuntimeError('Missing or ambiguous frame: ' + fragment)
        return rows[0]['bytes']
    palette = frame('projectModelPalette49S('); rigid = frame('projectRigidVertices49S(')
    recursion = frame('void rr::game::{anonymous}::invalidate(')
    caller = ['SceneNodes52','SceneGraph49P','PaletteInputs2','SourceNodes28','PaletteIds28',
              'Palettes2','ModelPaletteWorkspace49S','ModelPaletteDiagnostic49S',
              'RigidVertices2871','RigidPositions2871','RigidVertexView49S','RigidPaletteView49S',
              'RigidPositionView49S','RigidVertexDiagnostic49S']
    hashes['tools/measure_model_palette49s.py'] = digest(Path(__file__))
    hashes['tools/measure_scene_projection49p.py'] = digest(ROOT / 'tools/measure_scene_projection49p.py')
    check_stable_sources(before, hashes)
    result = {'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':{},
        'compile_commands':commands,'source_hashes':hashes,'objects':objects,'record_bytes':records,
        'record_alignments':alignments,'incremental49S_units':NEW_UNITS,
        'totals':{k:sum(objects[n][k] for n in NEW_UNITS) for k in keys},
        'reused_support_units':SUPPORT,
        'caller_payload_records':{n:records[n] for n in caller},
        'caller_payload_total_bytes_excluding_loader_and_frame_padding':sum(records[n] for n in caller),
        'actual_active_palette_bytes':2*(records['BodyMatrix48G']*(1+14)+4),
        'actual_palette_storage_bytes':records['Palettes2'],
        'workspace_total_bytes':records['ModelPaletteWorkspace49S'],
        'workspace_composition':{'scene_nodes64':records['SceneNodes64'],'palette64':records['ModelPalette49S']},
        'rigid_projection_workspace_bytes':0,
        'refresh_explicit_local_array_payload_bytes':records['RefreshChain64']+records['RefreshWorlds64'],
        'frames':frames,'maximum_individual49S_compiler_frame_bytes':max(f['bytes'] for f in frames if f['unit'] in NEW_UNITS),
        'selected_additive_compiler_frame_estimates':[
            {'chain':['projectModelPalette49S','refreshSceneWorld49P','validateSceneGraph49P'],
             'sum_compiler_frame_bytes':palette+frame('refreshSceneWorld49P(')+frame('validateSceneGraph49P(')},
            {'chain':['setLocalMatrix49P','afterLocalWrite','invalidate x <=64'],
             'sum_compiler_frame_bytes':frame('setLocalMatrix49P(')+frame('void rr::game::{anonymous}::afterLocalWrite(')+64*recursion},
            {'chain':['projectRigidVertices49S','project'],
             'sum_compiler_frame_bytes':rigid+frame('bool rr::game::{anonymous}::project(')}],
        'recursive_invalidate':{'maximum_source_invocations_on_stack':64,'individual_compiler_frame_bytes':recursion},
        'limits':[
            'Host strict C++17 sizeof/alignment, relocatable object sections and static compiler frame estimates only. No FPS, Xbox, whole-game memory or process RAM claim.',
            'Only model_palette49s and rigid_vertices49s count as incremental49S. Existing scene_graph49p is separately measured support, not additional new49S code.',
            'All three objects compile with -Wall -Wextra -Wpedantic -Werror and no warning suppressions. No body_scene48g implementation object is required.',
            'GNU size text includes readonly data and unwind metadata; executable .text, .rodata, .data and .bss are explicitly split.',
            'Caller payload is a component sizeof sum with52 existing graph nodes, actual2871 compact semantic vertices and outputs, two64-entry palette records, one serially reused workspace and one set of rigid views/diagnostics. It excludes loader, harness, frame padding and a second concurrent workspace.',
            'The existing graph is itemized, not newly allocated by49S. Compact semantic vertices replace a cache view for this harness; do not also count the full semantic asset unless a caller retains it.',
            'Workspace already contains64scene nodes plus one64-entry palette; do not add its components twice. Two serial engine projections reuse it. Rigid projection uses no caller scratch.',
            'Active root+14entry matrices/count perengine are1928bytes total for two, but actual bounded API output records reserve8328bytes for two64-entry palettes.',
            'Refresh local arrays are already inside the refresh compiler frame; they are not an additional caller charge. .su/additive estimates are not measured stack high-water or exhaustive runtime/ABI bounds; host red zones, runtime functions and harness caller are excluded.',
            'Direct allocator symbol audit and the separate successful-call benchmark probe do not test allocation failures or ownership.']}
    dump(output / 'summary.json', result)
    print(json.dumps({'stage':'footprint','totals':result['totals'],'caller_bytes':result['caller_payload_total_bytes_excluding_loader_and_frame_padding'],'workspace_bytes':result['workspace_total_bytes'],'records':records}), flush=True)
    return result


class Reader:
    def __init__(self,b): self.b,self.p=b,0
    def take(self,n):
        r=self.b[self.p:self.p+n]; assert len(r)==n; self.p+=n; return r
    def read(self,f): return struct.unpack('<'+f,self.take(struct.calcsize('<'+f)))
    def u(self): return self.read('I')[0]
    def string(self): return self.take(self.u()).decode()


def local_vertices(path):
    r=Reader(path.read_bytes()); assert r.take(8)==b'RRSCENE1' and r.u()==1
    world,palette,nodes,materials,lods,_,_=r.read('7I')
    assert world==0 and palette==1 and nodes==14 and lods==4
    r.take(56)
    for _ in range(nodes): r.string(); r.take(4+39*8)
    for _ in range(materials): r.string()
    out=[]
    for li in range(lods):
        r.take(8); ns=r.u()
        for _ in range(ns):
            r.read('2I'); r.take(8); nv,ni=r.read('2I')
            for _ in range(nv):
                xyz=r.read('3d'); r.take(3*8+2*8+4); bone=r.read('H')[0]
                if li==0: out.append((xyz,bone))
            r.take(ni*4)
    spans=r.u(); assert spans==2; r.take(spans*24)
    neutral=r.take(r.u()); assert neutral[:8]==b'RRMESH01'
    assert r.p==len(r.b)
    return out


def fnv(data):
    h=1469598103934665603
    for x in data: h=((h^x)*1099511628211)&((1<<64)-1)
    return h


def prepare_fixture(output,trace_path):
    trace=json.loads(trace_path.read_text()); row=trace['frames'][95]
    assert len(trace['frames'])==96 and len(row['graph'])==52
    assert trace['summary']['passed'] and trace['summary']['source_substitutions']==0
    locals_=[local_vertices(ROOT/p) for p in SCENES]
    assert [len(x) for x in locals_]==[1411,1460]
    data=bytearray(b'RR49SB01')
    data.extend(struct.pack('<3I',52,1411,1460))
    graph_bytes=bytearray(); palette_bytes=bytearray(); position_bytes=bytearray()
    for i,n in enumerate(row['graph']):
        assert n['index']==i
        graph_bytes.extend(struct.pack('<32I3iI',*n['local_bits'],*n['world_bits'],n['parent180'],n['first_child184'],n['next_sibling188'],n['flags190']))
        data.extend(struct.pack('<32I3iB',*n['local_bits'],*n['world_bits'],n['parent180'],n['first_child184'],n['next_sibling188'],n['flags190']))
    for side,name in enumerate(('left_palette','right_palette')):
        pal=row[name]; assert pal['entry_count']==14
        words=pal['root_world_bits']+[w for m in pal['node_world_bits'] for w in m]
        packed=struct.pack('<240I',*words); data.extend(packed)
        palette_bytes.extend(packed); palette_bytes.extend(struct.pack('<I',14))
        for xyz,bone in locals_[side]:
            data.extend(struct.pack('<3dH',*xyz,bone))
            matrix=struct.unpack('<16f',struct.pack('<16I',*pal['node_world_bits'][bone]))
            out=[]
            for axis in range(3):
                total=0.0
                for k in range(3): total=total+xyz[k]*matrix[k*4+axis]
                out.append(total+matrix[12+axis])
            position_bytes.extend(struct.pack('<3f',*out))
    path=output/'actual-frame95.bin'; path.write_bytes(data)
    expected={'graph_checksum':fnv(graph_bytes),'palette_checksum':fnv(palette_bytes),'position_checksum':fnv(position_bytes)}
    dump(output/'fixture.json',{'trace_path':str(trace_path.relative_to(ROOT)),'trace_sha256':digest(trace_path),
        'frame':95,'graph_nodes':52,'left_model_root':0,'right_model_root':15,
        'left_source_nodes':list(range(1,15)),'right_source_nodes':list(range(16,30)),
        'palette_ids':list(range(14)),'left_vertices':1411,'right_vertices':1460,
        'animated_brake_fin_vertices':sum(bone in (2,3,11,12,13) for side in locals_ for _,bone in side),
        'fixture_bytes':len(data),'fixture_sha256':digest(path),'expected_checksums':expected,
        'scene_sources':[{'path':p,'sha256':digest(ROOT/p)} for p in SCENES]})
    return path,expected


HARNESS = r'''// Actual connected frame95 graph and RRSCENE semantic local vertices.
#include "game/model_palette49s.h"
#include "game/rigid_vertices49s.h"
#include <cfenv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
using namespace rr::game;
using U=std::uint32_t;
using H=std::uint64_t;
#ifdef PROBE49S
static H allocations=0,getters=0; static bool count=false,dirtyMode=false,orderOk=true;
void* operator new(std::size_t n){if(count)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p)noexcept{std::free(p);} void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);} void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
void getter49S(SceneNodeIndex49P index)noexcept{
 if(!count)return;
 const U slot=U(getters%(dirtyMode?30u:28u));
 const U expected=dirtyMode?slot:(slot<14?slot+1:slot+2);
 orderOk=orderOk&&U(index)==expected;++getters;
}
#endif
static void need(bool yes,const char*why){if(!yes){std::fprintf(stderr,"FAIL %s\n",why);std::exit(2);}}
static U word(float f){U u;std::memcpy(&u,&f,4);return u;}
static void hashWord(H&h,U u){for(U i=0;i<4;++i){h^=(u>>(8*i))&255u;h*=1099511628211ull;}}
static H graphHash(const SceneNode49P*g){H h=1469598103934665603ull;for(U i=0;i<52;++i){for(float f:g[i].localC0.v)hashWord(h,word(f));for(float f:g[i].world100.v)hashWord(h,word(f));hashWord(h,U(g[i].parent180));hashWord(h,U(g[i].firstChild184));hashWord(h,U(g[i].nextSibling188));hashWord(h,g[i].sceneFlags190);}return h;}
static H paletteHash(const ModelPalette49S*p){H h=1469598103934665603ull;for(U s=0;s<2;++s){for(float f:p[s].rootWorld.v)hashWord(h,word(f));for(U i=0;i<p[s].entryCount;++i)for(float f:p[s].nodeWorld[i].v)hashWord(h,word(f));hashWord(h,p[s].entryCount);}return h;}
static H positionsHash(const RigidPosition49S*p){H h=1469598103934665603ull;for(U i=0;i<2871;++i)for(float f:p[i].v)hashWord(h,word(f));return h;}
static void read(std::FILE*f,void*p,std::size_t n){need(std::fread(p,1,n,f)==n,"fixture read");}
static U readU(std::FILE*f){U u;read(f,&u,4);return u;}
int main(int argc,char**argv){
 need(argc==4,"usage fixture iterations mode");need(std::fegetround()==FE_TONEAREST,"rounding");
 const U iterations=U(std::strtoul(argv[2],nullptr,10));need(iterations>0,"iteration count");
 const char*mode=argv[3];const bool invalidating=std::strstr(mode,"root_setters")!=nullptr;
 const bool doPalette=std::strcmp(mode,"rigid_ready_palette")!=0;
 const bool doRigid=std::strncmp(mode,"palette_",8)!=0;
 need(!std::strcmp(mode,"palette_ready")||!std::strcmp(mode,"palette_root_setters")||!std::strcmp(mode,"rigid_ready_palette")||!std::strcmp(mode,"combined_ready")||!std::strcmp(mode,"combined_root_setters"),"mode");
 SceneNode49P nodes[52]{};SceneGraph49P graph{nodes,52};SceneNodeIndex49P source[2][14]{};std::uint8_t ids[2][14]{};
 ModelPalette49S palettes[2]{},expected[2]{};ModelPaletteWorkspace49S workspace{};ModelPaletteDiagnostic49S paletteDiagnostic{};
 RigidVertex49S vertices[2871]{};RigidPosition49S positions[2871]{};RigidVertexDiagnostic49S rigidDiagnostic{};
 ModelPaletteInput49S inputs[2]{};BodyMatrix48G rootLocals[2]{};
 auto*f=std::fopen(argv[1],"rb");need(f!=nullptr,"fixture open");char magic[8];read(f,magic,8);need(!std::memcmp(magic,"RR49SB01",8),"fixture magic");
 need(readU(f)==52&&readU(f)==1411&&readU(f)==1460,"fixture counts");
 for(auto&n:nodes){read(f,n.localC0.v,64);read(f,n.world100.v,64);n.parent180=SceneNodeIndex49P(readU(f));n.firstChild184=SceneNodeIndex49P(readU(f));n.nextSibling188=SceneNodeIndex49P(readU(f));read(f,&n.sceneFlags190,1);}
 for(U side=0;side<2;++side){read(f,expected[side].rootWorld.v,64);read(f,expected[side].nodeWorld,14*64);expected[side].entryCount=14;
  const U begin=side?1411u:0u,n=side?1460u:1411u;
  for(U i=0;i<n;++i){read(f,vertices[begin+i].positionLocal,24);read(f,&vertices[begin+i].paletteIndex,2);}
  for(U i=0;i<14;++i){source[side][i]=SceneNodeIndex49P((side?16u:1u)+i);ids[side][i]=std::uint8_t(i);}
  inputs[side]={ModelPalettePhase49S::selectedRenderPreparation,ModelPaletteSceneState49S::completeCurrentScene,1,SceneNodeIndex49P(side?15u:0u),source[side],14,ids[side],14};rootLocals[side]=nodes[side?15:0].localC0;
 }
 need(std::fgetc(f)==EOF,"fixture trailing bytes");need(std::fclose(f)==0,"fixture close");
 auto projectPalettes=[&](){for(U side=0;side<2;++side)need(projectModelPalette49S(graph,inputs[side],palettes[side],workspace,paletteDiagnostic),"palette projection");};
 auto projectPositions=[&](){for(U side=0;side<2;++side){const U at=side?1411u:0u,n=side?1460u:1411u;need(projectRigidVertices49S({vertices+at,n},{palettes[side].nodeWorld,14},{positions+at,n},rigidDiagnostic),"rigid projection");}};
 // Setup once. Every selected entry is checked; no periteration seed/reset.
 need(validateSceneGraph49P(graph),"initial graph");projectPalettes();projectPositions();
 need(paletteHash(palettes)==paletteHash(expected),"all source palette words");
 const H initialGraph=graphHash(nodes),initialPalette=paletteHash(palettes),initialPositions=positionsHash(positions);
 H checksum=0;
#ifdef PROBE49S
 allocations=0;getters=0;dirtyMode=invalidating;count=true;
#else
 const auto begin=std::chrono::steady_clock::now();
#endif
 for(U iteration=0;iteration<iterations;++iteration){
  if(invalidating){need(setLocalMatrix49P(graph,SceneNodeIndex49P(0),rootLocals[0]),"left root setter");need(setLocalMatrix49P(graph,SceneNodeIndex49P(15),rootLocals[1]),"right root setter");}
  if(doPalette)projectPalettes();
  if(doRigid)projectPositions();
  // Small observable samples; full graph/palette/position checksums are untimed.
  checksum+=word(palettes[iteration&1u].nodeWorld[iteration%14].v[12]);
  checksum+=word(positions[iteration%2871].v[iteration%3]);
 }
#ifdef PROBE49S
 count=false;const long long ns=0;const H observed=allocations;
 need(orderOk,"every getter order");need(getters==(doPalette?H(iterations)*(invalidating?30u:28u):0),"every getter count");
#else
 const auto end=std::chrono::steady_clock::now();const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
#endif
 const H finalGraph=graphHash(nodes),finalPalette=paletteHash(palettes),finalPositions=positionsHash(positions);
 need(finalGraph==initialGraph&&finalPalette==initialPalette&&finalPositions==initialPositions,"persistent state and exact outputs");
 std::printf("mode=%s iterations=%u nanoseconds=%lld checksum=%llu graph_checksum=%llu palette_checksum=%llu position_checksum=%llu nodes=52 left_vertices=1411 right_vertices=1460 palette_entries_per_engine=14 palette_calls_per_iteration=%u rigid_calls_per_iteration=%u root_setters_per_iteration=%u workspace_bytes=%zu fixture_payload_bytes=%zu",
 mode,iterations,static_cast<long long>(ns),static_cast<unsigned long long>(checksum),static_cast<unsigned long long>(finalGraph),static_cast<unsigned long long>(finalPalette),static_cast<unsigned long long>(finalPositions),doPalette?2u:0u,doRigid?2u:0u,invalidating?2u:0u,sizeof(workspace),sizeof(nodes)+sizeof(vertices)+sizeof(positions));
#ifdef PROBE49S
 std::printf(" cpp_new_allocations=%llu getter_calls=%llu getter_order_verified=1",static_cast<unsigned long long>(observed),static_cast<unsigned long long>(getters));
#endif
 std::puts("");return 0;
}
'''


def percentile(values,fraction):
    ordered=sorted(values); at=(len(ordered)-1)*fraction; lo=math.floor(at); hi=math.ceil(at)
    return ordered[lo]+(ordered[hi]-ordered[lo])*(at-lo)


def benchmark(build,object_build,foot,args):
    output=BASE/'benchmark'; output.mkdir(parents=True,exist_ok=True); build.mkdir(parents=True,exist_ok=False)
    before=snapshot_sources(); hashes={}; commands=[]; objects=[]
    for name,h in foot['source_hashes'].items():
        if name.startswith('src/'):
            assert digest(ROOT/name)==h, name
            hashes[name]=h
    for name in [*NEW_UNITS,*SUPPORT]:
        obj=object_build/(Path(name).name+'.o'); assert digest(obj)==foot['objects'][name]['object_sha256']
        objects.append(obj)
    trace_path=ROOT/args.trace; trace_before=digest(trace_path)
    fixture,expected=prepare_fixture(output,trace_path)
    source=output/'model_palette_benchmark49s.cpp'; source.write_text(HARNESS)
    hook=output/'getter-probe.h'; hook.write_text('#include "game/scene_graph49p.h"\nvoid getter49S(rr::game::SceneNodeIndex49P) noexcept;\n#define RR_MODEL_PALETTE49S_GETTER(index) ::getter49S(index)\n')
    hooked=build/'model_palette49s-probe.o'
    command=['g++',*FLAGS,'-I',str(ROOT/'src'),'-include',str(hook),'-c',str(ROOT/'src/game/model_palette49s.cpp'),'-o',str(hooked)]
    commands.append(command);run(command)
    exe,probe=build/'benchmark',build/'ordinary-new-getter-probe'
    for binary,extra,used in [(exe,[],objects),(probe,['-DPROBE49S'],[hooked,*objects[1:]])]:
        dep=build/(binary.name+'.d')
        command=['g++',*FLAGS,*extra,'-MMD','-MF',str(dep),'-I',str(ROOT/'src'),str(source),*[str(x) for x in used],'-o',str(binary)]
        commands.append(command);run(command)
        for path in dependency_paths(dep):
            if path.is_relative_to(ROOT):hashes[path.relative_to(ROOT).as_posix()]=digest(path)
    for path in build.glob('*.su'):(output/path.name).write_text(path.read_text())
    cpus=sorted(os.sched_getaffinity(0));cpu=0 if 0 in cpus else cpus[0];cap=64*1024*1024
    def limits():
        os.sched_setaffinity(0,{cpu});resource.setrlimit(resource.RLIMIT_AS,(cap,cap));resource.setrlimit(resource.RLIMIT_CPU,(30,30))
    def execute(binary,mode,count):
        command=[str(binary),str(fixture),str(count),mode]
        p=subprocess.run(command,cwd=ROOT,text=True,capture_output=True,preexec_fn=limits,timeout=40)
        if p.returncode or p.stderr:raise RuntimeError(f'{command}: exit={p.returncode}\n{p.stdout}{p.stderr}')
        row=dict(part.split('=',1) for part in p.stdout.split());row={k:v if k=='mode' else int(v) for k,v in row.items()}
        assert all(row[k]==v for k,v in expected.items()), row
        assert row['workspace_bytes']==foot['workspace_total_bytes']
        return row,p.stdout
    calibrations=[];calibration_stdout=[];iterations={}
    for mode in MODES:
        row,stdout=execute(exe,mode,512);calibrations.append(row);calibration_stdout.append(stdout)
        target=math.ceil(150000000*512/row['nanoseconds'])
        iterations[mode]=args.iterations or max(1024,64*math.ceil(target/64))
    (output/'calibration.stdout').write_text(''.join(calibration_stdout))
    probes=[];probe_stdout=[]
    for mode in MODES:
        row,stdout=execute(probe,mode,iterations[mode]);assert row['cpp_new_allocations']==0 and row['nanoseconds']==0
        probes.append(row);probe_stdout.append(stdout)
    (output/'ordinary-new-getter-probe.stdout').write_text(''.join(probe_stdout))
    results=[];raw=[]
    for sample in range(args.samples):
        for mode in MODES if sample%2==0 else MODES[::-1]:
            row,stdout=execute(exe,mode,iterations[mode]);row['sample']=sample
            row['nanoseconds_per_iteration']=row['nanoseconds']/row['iterations'];results.append(row);raw.append(stdout)
    stats={}
    for mode in MODES:
        rows=[r for r in results if r['mode']==mode];values=[r['nanoseconds_per_iteration'] for r in rows]
        probe_row=next(r for r in probes if r['mode']==mode)
        assert {r['checksum'] for r in rows}=={probe_row['checksum']}
        stats[mode]={'minimum_ns':min(values),'p05_ns':percentile(values,.05),'p25_ns':percentile(values,.25),
                     'median_ns':statistics.median(values),'p75_ns':percentile(values,.75),'p95_ns':percentile(values,.95),
                     'maximum_ns':max(values),'mean_ns':statistics.mean(values),
                     'sample_duration_ns_range':[min(r['nanoseconds'] for r in rows),max(r['nanoseconds'] for r in rows)]}
    assert digest(trace_path)==trace_before,'Trace changed; rerun benchmark'
    for p in [Path(__file__),ROOT/'tools/measure_scene_projection49p.py',trace_path,source,fixture,hook,*[ROOT/n for n in SCENES]]:
        hashes[p.relative_to(ROOT).as_posix()]=digest(p)
    check_stable_sources(before,hashes)
    measured,size,detail=sections(exe);symbols=run(['nm','-C',str(exe)])
    assert 'getter49S(' not in symbols and 'countAllocations' not in symbols and 'operator new(' not in symbols
    for forbidden in ('Machine','original::','oracle','advanceEngineAnimation49Q','updateVehicleVisual49R'):
        assert forbidden not in symbols
    (output/'timed-executable-symbols.txt').write_text(symbols);(output/'timed-executable-size.txt').write_text(size)
    (output/'timed-executable-sections.txt').write_text(detail);(output/'samples.stdout').write_text(''.join(raw))
    result={'status':'measured','compiler':run(['g++','--version']).splitlines()[0],
        'compiler_target':run(['g++','-dumpmachine']).strip(),'flags':FLAGS,'unit_extra_flags':{},
        'compile_commands':commands,'source_hashes':hashes,'footprint_summary_sha256':digest(BASE/'footprint/summary.json'),
        'reused_footprint_objects':{n:foot['objects'][n]['object_sha256'] for n in [*NEW_UNITS,*SUPPORT]},
        'timed_executable_sha256':digest(exe),'ordinary_probe_executable_sha256':digest(probe),
        'timed_executable_sections':measured,'cpu_affinity':[cpu],'available_cpu_affinity':cpus,
        'normal_virtual_address_limit_bytes':cap,'cpu_seconds_limit_per_process':30,
        'wall_seconds_timeout_per_process':40,'rounding':'FE_TONEAREST checked each process',
        'samples_per_mode':args.samples,'iterations_per_sample_by_mode':iterations,
        'iteration_justification':'One512-iteration calibration per mode targeting150ms, minimum1024 and rounded up to64. All samples retained; modes run in alternating order.',
        'calibration':calibrations,'statistics':stats,'percentile_method':'Linear interpolation at sorted index(n-1)*p; descriptive host samples, not tail-latency guarantees.',
        'results':results,'separate_untimed_success_probes':probes,
        'fixture':json.loads((output/'fixture.json').read_text()),
        'setup_outside_timing':['Parse compact binary prepared from connected frame95 graph and semantic RRSCENE LOD0 double XYZ and rigid bone indices.',
            'Initialize one52-node persistent graph, two14-entry source mappings/palette-ID arrays, two full64-entry output records, one reusable workspace and2871 output positions.',
            'Project both complete palettes once and compare every active root/node word to the connected source-validated output; project both rigid sets once.',
            'Close fixture input, then capture full graph, active palette and position checksums.'],
        'timed_stages':{
            'palette_ready':'Two complete14-entry palette projections on persistent ready caches, including every entry getter and API admission/transaction copies.',
            'palette_root_setters':'Two source-style setLocalMatrix49P calls using the same current root-local matrices, then two complete14-entry palette projections. Setters invalidate ready descendants each iteration; getter rebuilds naturally restore ready caches.',
            'rigid_ready_palette':'Two complete rigid projections (1411left+1460right), consuming already resolved node-world palettes and original semantic double XYZ.',
            'combined_ready':'Both complete palette projections followed by both complete rigid projections on persistent ready caches.',
            'combined_root_setters':'Both root-local setters, both complete14-entry palette projections, then both complete rigid projections.'},
        'limits':[
            'Native host subsystem timing only, not FPS, Xbox performance, full-game memory, process RAM or a historical speedup claim.',
            '52-node current graph and28palette entries are retained; all28getters execute on ready repeats. Separate untimed instrumented success probes verify exact getter order and28or30calls perpair (30includes dirty roots). No instrumentation links into the timed executable.',
            'All2871 actual engine vertices are projected, including640brake/fin-associated vertices. Geometry-empty palette entries remain visited. All source nodes and complete matrices are retained.',
            'No periteration graph reset, seed reset, constructor rerun or synthetic motion is used. Root invalidation repeats source setters with unchanged current local values; it measures rebuild cost, not an animated49Q/49R frame.',
            'The graph, palette and position bit patterns are equal before/after each timed history; separate Python ordered-double projection computes expected full position checksum. Exact connected frame95 palette words are also checked.',
            'File I/O, semantic cache extraction, fixture loading, initial validation/warmup, full checksums and printed reports are outside timing. Two tiny observable word samples periteration are inside timing.',
            'Caller workspace is serially reused. Transaction copies and perentry topology/rounding admission remain part of palette cost; both validating and committing rigid passes remain part of rigid cost.',
            'Ordinary successful-call probes count scalar/array C++new only; direct allocator symbol audit covers every production object. No global Cmalloc/aligned-allocation instrumentation, allocation-failure or ownership experiment is performed.',
            'CPU pinning and64MiB virtual-address/30CPU-second process caps bound this ordinary host experiment; they do not simulate target hardware. All15samples per mode include scheduling noise.',
            'Timed executable section totals include the harness/runtime linkage and reused49P support; incremental49S code/data totals are in footprint/summary.json.']}
    dump(output/'verification.json',result)
    print(json.dumps({'stage':'benchmark','iterations':iterations,'statistics':stats}),flush=True)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-dir',type=Path,required=True)
    parser.add_argument('--footprint-only',action='store_true')
    parser.add_argument('--benchmark-only',action='store_true')
    parser.add_argument('--trace',default='evidence/modelPalette49S/connected/trace.json')
    parser.add_argument('--samples',type=int,default=15)
    parser.add_argument('--iterations',type=int,default=0)
    args=parser.parse_args()
    if args.footprint_only and args.benchmark_only:parser.error('Choose at most one stage-only mode.')
    if args.samples<5 or args.iterations<0 or args.iterations%64:parser.error('Use>=5samples and zero or a positive multiple of64iterations.')
    build=args.build_dir.resolve()
    foot=json.loads((BASE/'footprint/summary.json').read_text()) if args.benchmark_only else footprint(build/'footprint')
    if not args.footprint_only:benchmark(build/'benchmark',build/'footprint',foot,args)

if __name__=='__main__':main()
