#include "../../src/game/terrain_query_workspace48m.h"
#include "../../tools/gameplay/terrain_query_fixtures48j.h"
#include "../../tools/gameplay/query_binding48i.h"
#include <cfenv>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <new>
#include <chrono>
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
static std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
static float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
struct Reader {
    std::size_t cursor=0;
    std::uint32_t word(){if(cursor>=sizeof(terrainQueryFixtures48J)/4)throw std::runtime_error("fixture truncated");return terrainQueryFixtures48J[cursor++];}
    float real(){return fp(word());}
    template<class T>void vec(T& v){for(auto& x:v)x=real();}
    CollisionSphere48J sphere(){CollisionSphere48J s{};vec(s.center.v);s.radius=real();return s;}
    CollisionFilter48J filter(){
        CollisionFilter48J f{};f.identity=word();f.categoryA4=word();f.acceptMaskB8=word();
        f.flagsF8=static_cast<std::uint8_t>(word());f.flags31=static_cast<std::uint8_t>(word());
        for(auto& list:f.identityLists){list.mode=static_cast<std::int32_t>(word());list.identities.resize(word());for(auto& x:list.identities)x=word();}
        f.categoryModeD4=static_cast<std::int32_t>(word());f.categoryMasks.resize(word());for(auto& x:f.categoryMasks)x=word();return f;
    }
    PreparedTerrainState48J state(){
        PreparedTerrainState48J s{};s.surface.forceAngleTest108=word();s.surface.hit110=word();s.surface.hit114=word();
        vec(s.surface.point150.v);vec(s.surface.normal170.v);s.collectedCount=word();
        if(s.collectedCount>16)throw std::runtime_error("fixture list overflow");
        for(unsigned i=0;i<s.collectedCount;++i)s.collectedIdentities[i]=word();
        s.excludedOwnerIdentity=word();s.currentCandidateIdentity=word();return s;
    }
};
int main(int argc,char** argv){
    unsigned checks=0,index=0,hits=0;try{
        auto check=[&](bool value,const std::string& label){++checks;if(!value)throw std::runtime_error(label);};
        check(argc==2,"repository root argument");std::fesetround(FE_TONEAREST);
        CollisionMesh48I meshes[2];BodyMatrix48G world[2]{};
        const char* names[]={"17-ta_colcavetop.rrcol","18-zone04_col_floor.rrcol"};
        for(unsigned i=0;i<2;++i){
            std::ifstream file(std::filesystem::path(argv[1])/"evidence/query48I"/names[i],std::ios::binary);check(static_cast<bool>(file),"cache open");
            std::vector<std::uint8_t> data(std::istreambuf_iterator<char>(file),{});std::string error;
            check(loadCollisionMesh48I(data,meshes[i],error),"cache load "+error);
            std::array<float,3> position{},look{},up{};std::array<float,16> placement{},composed{};
            for(unsigned k=0;k<3;++k){position[k]=fp(queryCachePlacement48I[i][k]);look[k]=fp(queryCachePlacement48I[i][3+k]);up[k]=fp(queryCachePlacement48I[i][6+k]);}
            check(placementCollision48I(position,look,up,placement)&&composeCollisionWorld48I(meshes[i].local_matrix,placement,composed),"source instance placement");
            std::memcpy(world[i].v,composed.data(),64);
        }
        std::size_t stackCapacity=1;
        for(const auto& mesh:meshes){
            std::vector<std::uint32_t> pending{0};std::size_t visits=0;
            while(!pending.empty()){
                stackCapacity=std::max(stackCapacity,pending.size());
                check(++visits<=mesh.nodes.size()+mesh.faces.size(),"finite topology for setup bound");
                const auto tag=pending.back();pending.pop_back();const auto at=tag&~collision_mesh_leaf48i;
                if(tag&collision_mesh_leaf48i){check(at<mesh.faces.size(),"topology leaf bound");continue;}
                check(at<mesh.nodes.size(),"topology node bound");
                pending.push_back(mesh.nodes[at].children[1]);pending.push_back(mesh.nodes[at].children[0]);
            }
        }
        std::vector<std::uint32_t> stack(stackCapacity);
        Reader r;check(r.word()==0x4a383452&&r.word()==1,"fixture version");const auto count=r.word();
        TerrainQuery48J savedQuery{};std::vector<TerrainCandidate48J> savedCandidates;
        for(;index<count;++index){
            TerrainQuery48J query{};r.vec(query.segment.first.v);r.vec(query.segment.second.v);r.vec(query.segment.worldMatrix.v);
            query.sphere=r.sphere();query.filter=r.filter();query.callbackMasks={r.word(),r.word(),r.word()};query.prepared=r.state();
            r.vec(query.copied40);r.vec(query.copied50);query.collisionTypeTag=static_cast<std::uint8_t>(r.word());query.groupTypeTag=static_cast<std::uint8_t>(r.word());query.terrainCallbackEnabled=r.word()!=0;
            std::vector<TerrainCandidate48J> pool(r.word());
            for(auto& c:pool){
                const auto mesh=r.word();check(mesh<2,"mesh index");c.geometry.mesh=&meshes[mesh];r.vec(c.geometry.worldMatrix.v);
                check(std::memcmp(c.geometry.worldMatrix.v,world[mesh].v,64)==0,"original instance matrix");c.geometry.worldMatrix=world[mesh];
                c.sphere=r.sphere();c.filter=r.filter();c.geometry.id=c.filter.identity;c.ownerIdentity9C=r.word();c.typeTag=static_cast<std::uint8_t>(r.word());c.hasResponseCallbackE8=r.word()!=0;
                r.vec(c.copied40);r.vec(c.copied50);
            }
            std::vector<unsigned> order(r.word());std::vector<TerrainCandidate48J> candidates;
            for(auto& id:order){id=r.word();candidates.push_back(pool.at(id));}
            const bool hit=r.word()!=0;const auto flags=r.word();const auto expected=r.state();
            std::uint32_t contact[10];for(auto& v:contact)v=r.word();
            std::vector<TerrainCandidateEffects48J> effects(pool.size());
            for(auto& e:effects){e.identity=r.word();e.flagsF8=static_cast<std::uint8_t>(r.word());r.vec(e.copied40);r.vec(e.copied50);}
            TerrainQueryResult48M result{};
            std::vector<TerrainCandidateEffects48J> scratch(candidates.size()+1),destination(candidates.size()+1);
            scratch.back().identity=0x12345678;destination.back().identity=0xAABBCCDD;
            const TerrainQueryWorkspace48M workspace{scratch.data(),candidates.size(),stack.data(),stack.size()};
            const auto allocations=observedAllocations;watchAllocations=true;
            const auto status=queryTerrainLimitedBounded48M(query,candidates.data(),candidates.size(),workspace,destination.data(),candidates.size(),result);
            watchAllocations=false;check(observedAllocations==allocations,"zero runtime allocations");
            check(scratch.back().identity==0x12345678&&destination.back().identity==0xAABBCCDD,"effect canaries");
            check(status==TerrainQueryStatus48M::Complete,"status "+std::to_string(static_cast<unsigned>(status)));
            check(result.hasGeometryHit==hit,"geometry hit");check(result.queryFlagsF8==flags,"query flags");
            const auto& actual=result.terrain;const auto& a=actual.surface;const auto& b=expected.surface;
            check(a.forceAngleTest108==b.forceAngleTest108&&a.hit110==b.hit110&&a.hit114==b.hit114,"surface flags");
            for(unsigned k=0;k<3;++k){check(bits(a.point150.v[k])==bits(b.point150.v[k]),"surface point");check(bits(a.normal170.v[k])==bits(b.normal170.v[k]),"surface normal");}
            check(actual.collectedCount==expected.collectedCount,"callback count");
            for(unsigned k=0;k<actual.collectedCount;++k)check(actual.collectedIdentities[k]==expected.collectedIdentities[k],"callback identity order");
            check(actual.excludedOwnerIdentity==expected.excludedOwnerIdentity,"exclusion owner");check(actual.currentCandidateIdentity==expected.currentCandidateIdentity,"current candidate");
            check(result.candidateCount==candidates.size(),"candidate effects count");
            for(unsigned k=0;k<order.size();++k){
                const auto& x=destination[k];const auto& y=effects[order[k]];
                check(x.identity==y.identity&&x.flagsF8==y.flagsF8,"candidate flags");
                for(unsigned j=0;j<4;++j){check(bits(x.copied40[j])==bits(y.copied40[j]),"candidate copy40");check(bits(x.copied50[j])==bits(y.copied50[j]),"candidate copy50");}
            }
            if(hit){++hits;for(unsigned k=0;k<10;++k){
                const auto& s=result.collision.selected;
                float value=k<3?s.point.v[k]:k<6?s.normal.v[k-3]:k<9?s.correction.v[k-6]:s.fraction;
                check(bits(value)==contact[k],"contact word "+std::to_string(k));
            }}
            if(index==0){savedQuery=query;savedCandidates=candidates;}
        }
        check(r.cursor==sizeof(terrainQueryFixtures48J)/4,"unconsumed fixture data");
        // Composition checks are native wiring checks, not a claim to execute
        // the original spatial-registration or full live-vehicle phase.
        for(unsigned active=0;active<2;++active){
            TerrainPrepareInput48M prep{};prep.state.terrain=savedQuery.prepared;prep.state.segment=savedQuery.segment;prep.state.sphere=savedQuery.sphere;
            prep.state.follow.active104=active;prep.state.follow.pending10c=7;prep.scene=savedQuery.segment.worldMatrix;prep.lowerEndpoint140={{0,-40,0}};
            TerrainPrepareState48M prepared{};check(prepareTerrainSinglePair48M(prep,prepared)==TerrainPrepareStatus48M::QueryReady,"composition prepare");
            std::vector<TerrainCandidateEffects48J> effectsScratch(savedCandidates.size()),expectedEffects(savedCandidates.size()),actualEffects(savedCandidates.size());
            TerrainQueryWorkspace48M work{effectsScratch.data(),effectsScratch.size(),stack.data(),stack.size()};
            const TerrainQueryView48M view{prepared.segment,prepared.sphere,savedQuery.filter,savedQuery.callbackMasks,prepared.terrain,savedQuery.copied40,savedQuery.copied50,savedQuery.collisionTypeTag,savedQuery.groupTypeTag,savedQuery.terrainCallbackEnabled};
            TerrainQueryResult48M expected{};check(queryTerrainViewBounded48M(view,savedCandidates.data(),savedCandidates.size(),work,expectedEffects.data(),expectedEffects.size(),expected)==TerrainQueryStatus48M::Complete,"composition reference query");
            TerrainPreparedQueryResult48M result{};const auto allocations=observedAllocations;watchAllocations=true;
            const auto status=prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),work,actualEffects.data(),actualEffects.size(),result);watchAllocations=false;
            check(status==TerrainPreparedQueryStatus48M::Queried&&result.queryExecuted,"composition queried");check(observedAllocations==allocations,"composition zero allocations");
            prepared.terrain=expected.terrain;check(std::memcmp(&result.state,&prepared,sizeof(prepared))==0,"composition state exact");
            check(result.query.hasGeometryHit==expected.hasGeometryHit&&result.query.queryFlagsF8==expected.queryFlagsF8&&result.query.candidateCount==expected.candidateCount,"composition query flags");
            check(std::memcmp(&result.query.collision,&expected.collision,sizeof(expected.collision))==0,"composition collision exact");
            check(std::memcmp(actualEffects.data(),expectedEffects.data(),actualEffects.size()*sizeof(actualEffects[0]))==0,"composition effects exact");
            const auto effectsBefore=actualEffects;prep.queryEnabled100=false;
            check(prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),work,actualEffects.data(),actualEffects.size(),result)==TerrainPreparedQueryStatus48M::QueryDisabled&&!result.queryExecuted,"composition disabled");
            check(std::memcmp(actualEffects.data(),effectsBefore.data(),actualEffects.size()*sizeof(actualEffects[0]))==0,"disabled effects unchanged");
            prep.bodyLinked=false;prep.scene.v[0]=std::numeric_limits<float>::quiet_NaN();
            check(prepareAndQueryTerrain48M(prep,savedQuery,nullptr,0,{},nullptr,0,result)==TerrainPreparedQueryStatus48M::NoBody&&!result.queryExecuted&&std::memcmp(&result.state,&prep.state,sizeof(prep.state))==0,"composition no-body preserves state and skips geometry");
            prep.bodyLinked=true;prep.queryEnabled100=true;prep.scene=savedQuery.segment.worldMatrix;prep.auxiliarySource120=true;
            const auto oldResult=result;
            check(prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),work,actualEffects.data(),actualEffects.size(),result)==TerrainPreparedQueryStatus48M::UnsupportedScope&&std::memcmp(&result,&oldResult,sizeof(result))==0,"unsupported prep transactional");
            prep.auxiliarySource120=false;work.treeStackCapacity=0;
            const auto capacityStatus=prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),work,actualEffects.data(),actualEffects.size(),result);
            check(capacityStatus==TerrainPreparedQueryStatus48M::InsufficientCapacity&&std::memcmp(&result,&oldResult,sizeof(result))==0,"pipeline tree capacity transactional");
            check(std::memcmp(actualEffects.data(),effectsBefore.data(),actualEffects.size()*sizeof(actualEffects[0]))==0,"pipeline failed effects unchanged");
        }
        // Every failure, including late tree/response failures, is transactional.
        std::vector<TerrainCandidateEffects48J> scratch(savedCandidates.size()+1),destination(savedCandidates.size()+1);
        for(auto& v:destination){v.identity=0xAABBCCDD;v.flagsF8=0xCD;}
        TerrainQueryResult48M output{};output.queryFlagsF8=0xAB;output.terrain.surface.hit114=123;
        const auto original=output;const auto originalEffects=destination;
        const auto reject=[&](const TerrainQuery48J& q,const std::vector<TerrainCandidate48J>& c,const TerrainQueryWorkspace48M& workspace,TerrainCandidateEffects48J* dest,std::size_t cap,TerrainQueryStatus48M expected){
            const auto n=observedAllocations;watchAllocations=true;
            const auto status=queryTerrainLimitedBounded48M(q,c.data(),c.size(),workspace,dest,cap,output);watchAllocations=false;
            check(status==expected,"expected error status");check(observedAllocations==n,"failure zero allocations");
            check(std::memcmp(&output,&original,sizeof(output))==0,"error leaves result unchanged");
            check(std::memcmp(destination.data(),originalEffects.data(),destination.size()*sizeof(destination[0]))==0,"error leaves effects unchanged");
        };
        TerrainQueryWorkspace48M workspace{scratch.data(),savedCandidates.size(),stack.data(),stack.size()};
        auto invalid=savedCandidates;invalid[1].hasResponseCallbackE8=true;
        reject(savedQuery,invalid,workspace,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::UnsupportedScope);
        invalid=savedCandidates;invalid[1].typeTag=savedQuery.groupTypeTag;
        reject(savedQuery,invalid,workspace,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::UnsupportedScope);
        auto invalidQuery=savedQuery;invalidQuery.prepared.collectedCount=17;
        reject(invalidQuery,savedCandidates,workspace,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        auto small=workspace;small.effectCapacity=0;
        reject(savedQuery,savedCandidates,small,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InsufficientCapacity);
        reject(savedQuery,savedCandidates,workspace,destination.data(),0,TerrainQueryStatus48M::InsufficientCapacity);
        small=workspace;small.treeStackCapacity=0;
        reject(savedQuery,savedCandidates,small,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InsufficientCapacity);
        auto overlap=workspace;overlap.effects=destination.data();
        reject(savedQuery,savedCandidates,overlap,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        overlap=workspace;overlap.effects=reinterpret_cast<TerrainCandidateEffects48J*>(stack.data());
        reject(savedQuery,savedCandidates,overlap,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        overlap=workspace;overlap.effects=reinterpret_cast<TerrainCandidateEffects48J*>(savedCandidates.data());
        reject(savedQuery,savedCandidates,overlap,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        overlap=workspace;overlap.treeStack=reinterpret_cast<std::uint32_t*>(savedQuery.filter.categoryMasks.data());overlap.treeStackCapacity=savedQuery.filter.categoryMasks.size();
        reject(savedQuery,savedCandidates,overlap,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        alignas(TerrainCandidateEffects48J) std::array<unsigned char,sizeof(TerrainCandidateEffects48J)*4+1> effectBytes{};
        auto misalignedEffects=reinterpret_cast<TerrainCandidateEffects48J*>(effectBytes.data()+1);
        overlap=workspace;overlap.effects=misalignedEffects;
        reject(savedQuery,savedCandidates,overlap,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        reject(savedQuery,savedCandidates,workspace,misalignedEffects,savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        alignas(TerrainCandidate48J) std::array<unsigned char,sizeof(TerrainCandidate48J)+1> candidateBytes{};
        check(queryTerrainLimitedBounded48M(savedQuery,reinterpret_cast<const TerrainCandidate48J*>(candidateBytes.data()+1),1,workspace,destination.data(),destination.size(),output)==TerrainQueryStatus48M::InvalidInput,"misaligned candidate rejected before dereference");
        alignas(CollisionMesh48I) std::array<unsigned char,sizeof(CollisionMesh48I)+1> meshBytes{};
        invalid=savedCandidates;invalid[0].typeTag=255;invalid[0].geometry.mesh=reinterpret_cast<const CollisionMesh48I*>(meshBytes.data()+1);
        reject(savedQuery,invalid,workspace,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        // Only output addresses are inspected on these rejected calls. Backing
        // arrays are deliberately large/aligned enough; no aliased result is read.
        TerrainPrepareInput48M prep{};prep.state.segment=savedQuery.segment;prep.state.sphere=savedQuery.sphere;prep.state.terrain=savedQuery.prepared;
        prep.scene=savedQuery.segment.worldMatrix;prep.lowerEndpoint140={{0,-28,0}};
        for(unsigned mode=0;mode<3;++mode){
            prep.bodyLinked=mode!=0;prep.queryEnabled100=mode!=1;
            auto meta=savedQuery;meta.filter.categoryMasks.resize(256,0x20);const auto oldMasks=meta.filter.categoryMasks;
            auto* aliasResult=reinterpret_cast<TerrainPreparedQueryResult48M*>(meta.filter.categoryMasks.data());
            check(prepareAndQueryTerrain48M(prep,meta,savedCandidates.data(),savedCandidates.size(),workspace,destination.data(),destination.size(),*aliasResult)==TerrainPreparedQueryStatus48M::InvalidInput,"pipeline output/filter backing overlap");
            check(meta.filter.categoryMasks==oldMasks,"filter backing unchanged");
            auto candidates=savedCandidates;candidates[0].filter.identityLists[0].identities.resize(256,7);const auto oldIds=candidates[0].filter.identityLists[0].identities;
            aliasResult=reinterpret_cast<TerrainPreparedQueryResult48M*>(candidates[0].filter.identityLists[0].identities.data());
            check(prepareAndQueryTerrain48M(prep,savedQuery,candidates.data(),candidates.size(),workspace,destination.data(),destination.size(),*aliasResult)==TerrainPreparedQueryStatus48M::InvalidInput,"pipeline output/candidate backing overlap");
            check(candidates[0].filter.identityLists[0].identities==oldIds,"candidate backing unchanged");
            aliasResult=reinterpret_cast<TerrainPreparedQueryResult48M*>(meshes[0].nodes.data());
            check(prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),workspace,destination.data(),destination.size(),*aliasResult)==TerrainPreparedQueryStatus48M::InvalidInput,"pipeline output/mesh backing overlap");
        }
        invalid=savedCandidates;invalid[1].sphere.radius=std::numeric_limits<float>::quiet_NaN();
        reject(savedQuery,invalid,workspace,destination.data(),savedCandidates.size(),TerrainQueryStatus48M::InvalidInput);
        TerrainQueryWorkspace48M empty{nullptr,0,nullptr,0};TerrainQueryResult48M emptyResult{};
        watchAllocations=true;const auto emptyStatus=queryTerrainLimitedBounded48M(savedQuery,nullptr,0,empty,nullptr,0,emptyResult);watchAllocations=false;
        check(emptyStatus==TerrainQueryStatus48M::Complete&&!emptyResult.hasGeometryHit&&emptyResult.candidateCount==0,"empty list completion");
        prep.bodyLinked=true;prep.queryEnabled100=true;prep.state.follow.active104=1;
        TerrainPreparedQueryResult48M bench{};std::array<double,9> timings{};constexpr unsigned iterations=5000;unsigned completed=0;
        const auto allocationsBefore=observedAllocations;
        for(auto& timing:timings){
            watchAllocations=true;const auto start=std::chrono::steady_clock::now();
            for(unsigned i=0;i<iterations;++i)completed+=prepareAndQueryTerrain48M(prep,savedQuery,savedCandidates.data(),savedCandidates.size(),workspace,destination.data(),savedCandidates.size(),bench)==TerrainPreparedQueryStatus48M::Queried;
            const auto end=std::chrono::steady_clock::now();watchAllocations=false;
            timing=std::chrono::duration<double,std::nano>(end-start).count()/iterations;
        }
        check(completed==timings.size()*iterations,"pipeline benchmark completion");check(observedAllocations==allocationsBefore,"pipeline benchmark zero allocations");
        std::sort(timings.begin(),timings.end());
        std::cout<<"terrain_pipeline_host_ns median="<<timings[4]<<" max_batch_mean="<<timings[8]<<" candidates="<<savedCandidates.size()<<" geometry_hit="<<bench.query.hasGeometryHit<<" output_plus_scratch_effect_bytes="<<2*savedCandidates.size()*sizeof(TerrainCandidateEffects48J)<<" composite_metadata_bytes="<<sizeof(TerrainPreparedQueryResult48M)<<" (numeric preparation and supplied list only; no gameplay/Xbox FPS claim)\n";
        std::cout<<"workspace_stack_bytes="<<stack.size()*sizeof(stack[0])<<" effects_each_bytes="<<sizeof(TerrainCandidateEffects48J)<<" metadata_bytes="<<sizeof(TerrainQueryResult48M)<<" runtime_allocations="<<observedAllocations<<" (asset/filter loading is outside runtime watch)\n";
        std::cout<<"PASS "<<checks<<" checks; "<<index<<" bounded original-word limited-list queries and contact-state transitions, hits="<<hits<<", misses="<<(index-hits)<<"; actual imported meshes and source instance placements; unsupported paths transactional\n";return 0;
    }catch(const std::exception& e){watchAllocations=false;std::cerr<<"FAIL after "<<checks<<" checks case="<<index<<": "<<e.what()<<"\n";return 1;}
}
