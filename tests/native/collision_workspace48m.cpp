#include "../../src/game/collision_tree48i.h"
#include "../../tools/gameplay/query_fixture48i.h"
#include "../../tools/gameplay/query_binding48i.h"
#include <cfenv>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>
#include <new>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
// Test-only allocation instrumentation; production has no allocator dependency.
static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_TEST_NOINLINE __attribute__((noinline))
#else
#define RR_TEST_NOINLINE
#endif
RR_TEST_NOINLINE void* operator new(std::size_t size){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(size?size:1))return p;
    throw std::bad_alloc();
}
RR_TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

using namespace rr::game;
using namespace rr::assets;
static float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
static std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
struct Reader {
    std::size_t cursor=0;
    std::uint32_t word(){if(cursor>=sizeof(queryFixtureData48I)/4)throw std::runtime_error("fixture truncated");return queryFixtureData48I[cursor++];}
    float real(){return fp(word());}
    template<class T>void vec(T& v){for(auto& f:v)f=real();}
};
static std::size_t topologyStack(const CollisionMesh48I& mesh){
    std::vector<std::uint32_t> stack{0};std::vector<bool> seen(mesh.nodes.size()+mesh.faces.size());
    std::size_t maximum=1,visited=0;
    while(!stack.empty()){
        const auto tag=stack.back();stack.pop_back();const bool leaf=(tag&collision_mesh_leaf48i)!=0;
        const std::size_t index=tag&~collision_mesh_leaf48i;
        const std::size_t key=leaf?mesh.nodes.size()+index:index;
        if(key>=seen.size()||seen[key])throw std::runtime_error("invalid fixture topology");
        seen[key]=true;++visited;
        if(!leaf){const auto& node=mesh.nodes.at(index);stack.push_back(node.children[1]);stack.push_back(node.children[0]);maximum=std::max(maximum,stack.size());}
    }
    if(visited!=seen.size())throw std::runtime_error("unreachable fixture topology");
    return maximum;
}
int main(int argc,char** argv){
    unsigned checks=0,caseIndex=0,hits=0,misses=0,visits=0;
    std::size_t maximumFixtureStack=0,maximumFixtureTrace=0;
    try {
        auto check=[&](bool yes,const std::string& what){++checks;if(!yes)throw std::runtime_error(what);};
        check(argc==2,"repository root argument");
        std::fesetround(FE_TONEAREST);Reader r;
        if(r.word()!=0x49383452u||r.word()!=1)throw std::runtime_error("fixture format");
        std::vector<CollisionMesh48I> meshes(r.word());
        for(auto& m:meshes){
            const auto nv=r.word(),nn=r.word(),nt=r.word(),nf=r.word();r.vec(m.local_matrix);m.reverse_mode=r.word();
            m.vertices.resize(nv);m.vertex_normals.resize(nn);m.nodes.resize(nt);m.faces.resize(nf);
            for(auto& v:m.vertices)r.vec(v);
            for(auto& v:m.vertex_normals)r.vec(v);
            for(auto& n:m.nodes){n.volume=r.real();r.vec(n.center);r.vec(n.half);for(auto& c:n.children)c=r.word();}
            for(auto& f:m.faces){f.d=r.real();for(auto& n:f.normal)n=static_cast<std::int16_t>(r.word());for(auto& v:f.vertices)v=static_cast<std::uint16_t>(r.word());}
        }
        // Prove that retained imported native caches match every saved runtime
        // record, then discard the copied RAM meshes and query loaded assets.
        BodyMatrix48G importedWorld[2]{};
        const char* caches[]={"17-ta_colcavetop.rrcol","18-zone04_col_floor.rrcol"};
        for(unsigned i=0;i<2;++i){
            const auto path=std::filesystem::path(argv[1])/"evidence/query48I"/caches[i];
            std::ifstream file(path,std::ios::binary);check(static_cast<bool>(file),"native cache open");
            std::vector<std::uint8_t> bytes(std::istreambuf_iterator<char>(file),{});
            CollisionMesh48I imported;std::string error;
            check(loadCollisionMesh48I(bytes,imported,error),"native cache load "+error);
            const auto& saved=meshes[i];
            check(imported.vertices.size()==saved.vertices.size()&&imported.vertex_normals.size()==saved.vertex_normals.size()&&imported.nodes.size()==saved.nodes.size()&&imported.faces.size()==saved.faces.size(),"cache/snapshot counts");
            for(std::size_t j=0;j<saved.vertices.size();++j)check(std::memcmp(imported.vertices[j].data(),saved.vertices[j].data(),12)==0,"cache vertex bits");
            for(std::size_t j=0;j<saved.vertex_normals.size();++j)check(std::memcmp(imported.vertex_normals[j].data(),saved.vertex_normals[j].data(),12)==0,"cache vertex normal bits");
            for(std::size_t j=0;j<saved.nodes.size();++j)check(std::memcmp(&imported.nodes[j],&saved.nodes[j],sizeof(CollisionMeshNode48I))==0,"cache node bits");
            for(std::size_t j=0;j<saved.faces.size();++j)check(std::memcmp(&imported.faces[j],&saved.faces[j],sizeof(CollisionMeshFace48I))==0,"cache face bits");
            std::array<float,3> position{},look{},up{};
            for(unsigned k=0;k<3;++k){position[k]=fp(queryCachePlacement48I[i][k]);look[k]=fp(queryCachePlacement48I[i][3+k]);up[k]=fp(queryCachePlacement48I[i][6+k]);}
            std::array<float,16> placement{},world{};
            check(placementCollision48I(position,look,up,placement),"source instance placement");
            check(composeCollisionWorld48I(imported.local_matrix,placement,world),"source instance composition");
            std::memcpy(importedWorld[i].v,world.data(),64);
            meshes[i]=std::move(imported);
        }
        const auto caveStack=topologyStack(meshes[0]),floorStack=topologyStack(meshes[1]);
        check(caveStack==18&&floorStack==12,"verified topology stack bounds");
        std::size_t stackCapacity=0;for(const auto& mesh:meshes)stackCapacity=std::max(stackCapacity,topologyStack(mesh));
        std::vector<std::uint32_t> stack(stackCapacity+2,0xa55aa55au);
        const auto count=r.word();
        for(;caseIndex<count;++caseIndex){
            LocalSegment48I q{};r.vec(q.first.v);r.vec(q.second.v);r.vec(q.worldMatrix.v);
            std::vector<CollisionCandidate48I> candidates(r.word());
            for(auto& c:candidates){
                const auto mi=r.word();c.mesh=&meshes.at(mi);c.id=r.word();r.vec(c.worldMatrix.v);
                if(mi<2){check(std::memcmp(c.worldMatrix.v,importedWorld[mi].v,64)==0,"source-placement/saved-world exact");c.worldMatrix=importedWorld[mi];}
            }
            const bool expectedHit=r.word()!=0;
            std::uint32_t selected[12],pair[9];for(auto& x:selected)x=r.word();for(auto& x:pair)x=r.word();
            std::vector<CollisionVisit48I> expectedTrace(r.word());
            for(auto& v:expectedTrace){v.candidateId=r.word();v.index=r.word();v.leaf=r.word()!=0;v.accepted=r.word()!=0;}
            CollisionResult48I output{};output.selected.fraction=99.f;output.nearestPair.fraction=98.f;
            const auto before=output;
            const CollisionVisit48I guard{0xa55aa55au,0x5aa55aa5u,true,true};
            std::vector<CollisionVisit48I> trace(expectedTrace.size()+2,guard);
            const CollisionWorkspace48M workspace{stack.data()+1,stackCapacity,trace.data()+1,expectedTrace.size()};
            std::size_t traceUsed=999;
            watchAllocations=true;
            const auto status=queryOrderedCollisionBounded48M(q,candidates.data(),candidates.size(),workspace,output,traceUsed);
            watchAllocations=false;
            check(observedAllocations==0,"bounded query allocated");
            check(stack.front()==0xa55aa55au&&stack.back()==0xa55aa55au,"stack canary");
            const auto sameVisit=[](const CollisionVisit48I& a,const CollisionVisit48I& b){return a.candidateId==b.candidateId&&a.index==b.index&&a.leaf==b.leaf&&a.accepted==b.accepted;};
            check(sameVisit(trace.front(),guard)&&sameVisit(trace.back(),guard),"trace canary");
            CollisionResult48I noTrace=before;std::size_t unused=999;
            const CollisionWorkspace48M runtime{stack.data()+1,stackCapacity,nullptr,0};
            watchAllocations=true;
            const auto noTraceStatus=queryOrderedCollisionBounded48M(q,candidates.data(),candidates.size(),runtime,noTrace,unused);
            watchAllocations=false;
            check(noTraceStatus==status&&unused==0,"trace-disabled status/count");
            check(std::memcmp(&output,&noTrace,sizeof(output))==0,"trace-disabled numerical state");
            if(!expectedTrace.empty()){
                const CollisionWorkspace48M shortTrace{stack.data()+1,stackCapacity,trace.data()+1,expectedTrace.size()-1};
                CollisionResult48I rejected=before;std::size_t shortUsed=999;
                watchAllocations=true;
                const auto shortStatus=queryOrderedCollisionBounded48M(q,candidates.data(),candidates.size(),shortTrace,rejected,shortUsed);
                watchAllocations=false;
                check(shortStatus==CollisionQueryStatus48I::InsufficientCapacity,"short trace accepted");
                check(shortUsed==expectedTrace.size()-1,"short trace prefix count");
                check(std::memcmp(&rejected,&before,sizeof(rejected))==0,"short trace committed collision");
            }
            check(static_cast<unsigned>(status)==static_cast<unsigned>(expectedHit),"status got="+std::to_string(static_cast<unsigned>(status))+" expected="+std::to_string(expectedHit));
            check(traceUsed==expectedTrace.size(),"trace count got="+std::to_string(traceUsed)+" expected="+std::to_string(expectedTrace.size()));
            unsigned nodes=0,pruned=0,leaves=0,leafHits=0;
            std::size_t pending=0;
            maximumFixtureTrace=std::max(maximumFixtureTrace,expectedTrace.size());
            for(std::size_t i=0;i<traceUsed;++i){
                const auto& a=trace[i+1];const auto& b=expectedTrace[i];
                check(a.candidateId==b.candidateId&&a.index==b.index&&a.leaf==b.leaf&&a.accepted==b.accepted,"trace mismatch visit="+std::to_string(i));
                if(!b.leaf&&b.index==0)pending=1;
                check(pending>0,"fixture pending-node accounting");
                --pending;if(!b.leaf&&b.accepted)pending+=2;
                maximumFixtureStack=std::max(maximumFixtureStack,pending);
                if(b.leaf){++leaves;if(b.accepted)++leafHits;}else{++nodes;if(!b.accepted)++pruned;}
            }
            visits+=static_cast<unsigned>(traceUsed);
            if(expectedHit){
                ++hits;
                for(unsigned i=0;i<10;++i){
                    const float f=i<3?output.selected.point.v[i]:i<6?output.selected.normal.v[i-3]:i<9?output.selected.correction.v[i-6]:output.selected.fraction;
                    check(bits(f)==selected[i],"selected lane="+std::to_string(i)+" got="+std::to_string(bits(f))+" expected="+std::to_string(selected[i]));
                }
                check(output.selected.candidateId==selected[10]&&output.selected.face==selected[11],"selected IDs");
                for(unsigned i=0;i<7;++i){
                    const float f=i<3?output.nearestPair.point.v[i]:i<6?output.nearestPair.normal.v[i-3]:output.nearestPair.fraction;
                    check(bits(f)==pair[i],"pair lane="+std::to_string(i));
                }
                check(output.nearestPair.candidateId==pair[7]&&output.nearestPair.face==pair[8],"pair IDs");
                check(output.nodesVisited==nodes&&output.nodesPruned==pruned&&output.leavesTested==leaves&&output.leafHits==leafHits,"visit counters");
            }else{++misses;check(std::memcmp(&output,&before,sizeof(output))==0,"miss changed output");}
        }
        check(r.cursor==sizeof(queryFixtureData48I)/4,"unconsumed fixture words");
        check(maximumFixtureStack<=stackCapacity,"fixture exceeded topology bound");
        // Deliberately malformed/native API inputs; no collision output may commit.
        auto mesh=meshes.back();
        BodyMatrix48G identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
        LocalSegment48I q{{{.25f,1.f,.125f}},{{.25f,-1.f,.125f}},identity};
        CollisionCandidate48I c{&mesh,identity,1};CollisionResult48I out{};
        out.selected.fraction=99.f;const auto before=out;
        std::array<std::uint32_t,16> malformedStack{};
        CollisionWorkspace48M workspace{malformedStack.data(),malformedStack.size(),nullptr,0};
        const auto call=[&](const CollisionWorkspace48M& ws,CollisionQueryStatus48I expected,const char* label){
            out=before;std::size_t used=777;watchAllocations=true;
            const auto status=queryOrderedCollisionBounded48M(q,&c,1,ws,out,used);
            watchAllocations=false;check(status==expected,label);
            if(status!=CollisionQueryStatus48I::Hit)check(std::memcmp(&out,&before,sizeof(out))==0,"failure committed collision");
            return used;
        };
        check(call(workspace,CollisionQueryStatus48I::Hit,"synthetic hit") == 0,"disabled trace count");
        auto small=workspace;small.stackCapacity=2;
        call(small,CollisionQueryStatus48I::Hit,"exact synthetic stack bound");
        small.stackCapacity=1;call(small,CollisionQueryStatus48I::InsufficientCapacity,"short stack accepted");
        small.stackCapacity=0;call(small,CollisionQueryStatus48I::InsufficientCapacity,"empty stack accepted");
        small.stack=nullptr;small.stackCapacity=1;
        check(call(small,CollisionQueryStatus48I::InvalidInput,"null stack accepted")==777,"invalid extent changed count");
        small=workspace;small.trace=nullptr;small.traceCapacity=1;
        check(call(small,CollisionQueryStatus48I::InvalidInput,"null trace accepted")==777,"invalid extent changed count");
        CollisionVisit48I oneTrace{111,222,true,true};small=workspace;small.trace=&oneTrace;small.traceCapacity=0;
        check(call(small,CollisionQueryStatus48I::InsufficientCapacity,"zero trace capacity accepted")==0,"zero trace prefix");
        check(oneTrace.candidateId==111&&oneTrace.index==222,"zero trace wrote storage");
        small=workspace;small.stackCapacity=std::numeric_limits<std::size_t>::max();
        call(small,CollisionQueryStatus48I::InvalidInput,"overflow stack extent accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(reinterpret_cast<unsigned char*>(malformedStack.data())+1);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"misaligned stack accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(std::numeric_limits<std::uintptr_t>::max()-1);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"wrapping stack address accepted");
        small=workspace;small.trace=reinterpret_cast<CollisionVisit48I*>(malformedStack.data());small.traceCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"overlapping scratch accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(&q);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"query/scratch overlap accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(&c);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"candidate/scratch overlap accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(mesh.vertices.data());small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"mesh/scratch overlap accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(&out);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"output/scratch overlap accepted");
        small=workspace;small.stack=reinterpret_cast<std::uint32_t*>(&small);small.stackCapacity=1;
        call(small,CollisionQueryStatus48I::InvalidInput,"workspace/scratch overlap accepted");
        // traceUsed overlap must not clear collision output or its own descriptor.
        out=before;workspace.stack=reinterpret_cast<std::uint32_t*>(&stackCapacity);workspace.stackCapacity=1;
        watchAllocations=true;
        auto status=queryOrderedCollisionBounded48M(q,&c,1,workspace,out,stackCapacity);
        watchAllocations=false;check(status==CollisionQueryStatus48I::InvalidInput,"count/scratch overlap accepted");
        check(stackCapacity==18&&std::memcmp(&out,&before,sizeof(out))==0,"aliased count changed storage");
        workspace={malformedStack.data(),malformedStack.size(),nullptr,0};
        mesh.nodes[0].children[0]=0;mesh.nodes[0].children[1]=0;
        call(workspace,CollisionQueryStatus48I::InvalidInput,"cycle accepted");
        mesh=meshes.back();mesh.nodes[0].children[0]=123456u;
        call(workspace,CollisionQueryStatus48I::InvalidInput,"bad node index accepted");
        mesh=meshes.back();mesh.nodes[0].children[0]=collision_mesh_leaf48i|123456u;
        call(workspace,CollisionQueryStatus48I::InvalidInput,"bad face index accepted");
        mesh=meshes.back();mesh.faces[0].vertices[0]=65535;
        call(workspace,CollisionQueryStatus48I::InvalidInput,"bad vertex index accepted");
        mesh=meshes.back();mesh.nodes[0].center[0]=std::numeric_limits<float>::infinity();
        call(workspace,CollisionQueryStatus48I::InvalidInput,"nonfinite bounds accepted");
        mesh=meshes.back();mesh.reverse_mode=1;
        call(workspace,CollisionQueryStatus48I::InvalidInput,"reverse mode accepted");
        mesh=meshes.back();q.second=q.first;workspace={};
        call(workspace,CollisionQueryStatus48I::Miss,"zero-length segment requires stack");
        std::size_t emptyUsed=999;out=before;watchAllocations=true;
        status=queryOrderedCollisionBounded48M(q,nullptr,0,workspace,out,emptyUsed);
        watchAllocations=false;check(status==CollisionQueryStatus48I::Miss&&emptyUsed==0,"empty query result");
        check(std::memcmp(&out,&before,sizeof(out))==0,"empty query changed output");
        check(observedAllocations==0,"bounded paths allocated");
        std::cout<<"PASS "<<checks<<" checks; "<<caseIndex<<" original-word ordered queries, hits="<<hits<<", misses="<<misses<<", exact visits="<<visits
                 <<"; complete native meshes; topology stack cave="<<caveStack<<" floor="<<floorStack<<" reusable_bytes="<<stackCapacity*sizeof(std::uint32_t)
                 <<" fixture_peak_entries="<<maximumFixtureStack<<" headroom_entries="<<stackCapacity-maximumFixtureStack<<" maximum_trace_bytes="<<maximumFixtureTrace*sizeof(CollisionVisit48I)
                 <<"; bounded allocations="<<observedAllocations<<"; capacity, overlap and malformed tests passed\n";return 0;
    }catch(const std::exception& e){watchAllocations=false;std::cerr<<"FAIL after "<<checks<<" checks case="<<caseIndex<<": "<<e.what()<<"\n";return 1;}
}
