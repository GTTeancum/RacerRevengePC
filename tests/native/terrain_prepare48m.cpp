#include "../../src/game/terrain_prepare48m.h"
#include "../../evidence/terrainPrepare48M/fixtures.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>

using namespace rr::game;
static bool trackAllocations=false;
static unsigned allocations=0;
void* operator new(std::size_t size) {
    if(trackAllocations)++allocations;
    if(void* p=std::malloc(size?size:1))return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size){return ::operator new(size);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

static std::uint32_t bits(float x){std::uint32_t u;std::memcpy(&u,&x,4);return u;}
static float fp(std::uint32_t u){float x;std::memcpy(&x,&u,4);return x;}
static_assert(std::is_trivially_copyable<TerrainPrepareState48M>::value,"fixed state storage");
static_assert(std::is_trivially_copyable<TerrainPrepareInput48M>::value,"fixed input storage");
static_assert(sizeof(TerrainPrepareState48M)<=320,"bounded serial state");
static_assert(sizeof(TerrainPrepareInput48M)<=416,"bounded serial input");

struct Reader {
    std::size_t at=0;
    std::uint32_t word(){
        if(at>=sizeof(terrainPrepareFixtures48M)/sizeof(std::uint32_t))throw std::runtime_error("truncated fixture");
        return terrainPrepareFixtures48M[at++];
    }
    template<class T>void vec(T& v){for(auto& x:v)x=fp(word());}
    TerrainPrepareState48M state(){
        TerrainPrepareState48M s{};
        s.follow.active104=word();s.follow.pending10c=word();s.follow.detached11c=word();
        vec(s.follow.previousPoint180.v);vec(s.follow.previousNormal190.v);vec(s.follow.largestVelocityChange1d0.v);
        auto& t=s.terrain;t.surface.forceAngleTest108=word();t.surface.hit110=word();t.surface.hit114=word();
        vec(t.surface.point150.v);vec(t.surface.normal170.v);
        for(auto& x:t.collectedIdentities)x=word();
        t.collectedCount=word();t.excludedOwnerIdentity=word();t.currentCandidateIdentity=word();
        vec(s.segment.first.v);vec(s.segment.second.v);vec(s.segment.worldMatrix.v);
        vec(s.sphere.center.v);s.sphere.radius=fp(word());s.auxiliaryAbove118=word();return s;
    }
    TerrainPrepareInput48M input(){
        TerrainPrepareInput48M r{};
        r.bodyLinked=word()!=0;r.queryEnabled100=word()!=0;
        r.auxiliarySource120=word()!=0;r.auxiliarySource124=word()!=0;
        r.shapeType88=word();r.pairCount=word();r.hasPairTree=word()!=0;
        vec(r.scene.v);vec(r.lowerEndpoint140.v);r.state=state();return r;
    }
};
static std::array<std::uint32_t,67> words(const TerrainPrepareState48M& s){
    std::array<std::uint32_t,67> out{};std::size_t at=0;
    auto put=[&](std::uint32_t w){out.at(at++)=w;};
    auto vec=[&](const auto& v){for(float x:v)put(bits(x));};
    put(s.follow.active104);put(s.follow.pending10c);put(s.follow.detached11c);
    vec(s.follow.previousPoint180.v);vec(s.follow.previousNormal190.v);vec(s.follow.largestVelocityChange1d0.v);
    const auto& t=s.terrain;
    put(t.surface.forceAngleTest108);put(t.surface.hit110);put(t.surface.hit114);
    vec(t.surface.point150.v);vec(t.surface.normal170.v);
    for(auto x:t.collectedIdentities)put(x);
    put(t.collectedCount);put(t.excludedOwnerIdentity);put(t.currentCandidateIdentity);
    vec(s.segment.first.v);vec(s.segment.second.v);vec(s.segment.worldMatrix.v);
    vec(s.sphere.center.v);put(bits(s.sphere.radius));put(s.auxiliaryAbove118);
    if(at!=out.size())throw std::runtime_error("state layout");
    return out;
}

int main(){
    unsigned checks=0,index=0;try {
        auto check=[&](bool value,const std::string& label){++checks;if(!value)throw std::runtime_error(label);};
        check(std::fesetround(FE_TONEAREST)==0,"round to nearest");
        Reader r;check(r.word()==0x4d383452&&r.word()==1,"fixture version");const auto count=r.word();
        TerrainPrepareInput48M saved{};std::array<unsigned,3> statuses{};
        for(;index<count;++index){
            auto input=r.input();const auto expectedStatus=r.word();const auto expected=words(r.state());
            TerrainPrepareState48M output{};
            trackAllocations=true;const auto status=prepareTerrainSinglePair48M(input,output);trackAllocations=false;
            check(static_cast<unsigned>(status)==expectedStatus,"status "+std::to_string(index));
            const auto actual=words(output);
            for(unsigned i=0;i<actual.size();++i)check(actual[i]==expected[i],"case "+std::to_string(index)+" word "+std::to_string(i));
            auto alias=input;trackAllocations=true;
            const auto aliasStatus=prepareTerrainSinglePair48M(alias,alias.state);trackAllocations=false;
            check(aliasStatus==status&&words(alias.state)==expected,"aliased output");
            check(expectedStatus<statuses.size(),"supported status");++statuses[expectedStatus];
            if(index==0)saved=input;
        }
        check(r.at==sizeof(terrainPrepareFixtures48M)/sizeof(std::uint32_t),"all fixture words consumed");
        auto rejected=[&](TerrainPrepareInput48M input,TerrainPrepareStatus48M expected,const std::string& label){
            TerrainPrepareState48M output=saved.state;const auto before=words(output);
            trackAllocations=true;const auto status=prepareTerrainSinglePair48M(input,output);trackAllocations=false;
            check(status==expected,label+" status");check(words(output)==before,label+" transactional");
            const auto inputBefore=words(input.state);trackAllocations=true;
            const auto aliasStatus=prepareTerrainSinglePair48M(input,input.state);trackAllocations=false;
            check(aliasStatus==expected&&words(input.state)==inputBefore,label+" aliased transactional");
        };
        const auto invalid=TerrainPrepareStatus48M::InvalidInput;
        const auto unsupported=TerrainPrepareStatus48M::UnsupportedScope;
        auto input=saved;input.auxiliarySource120=true;rejected(input,unsupported,"auxiliary120");
        input=saved;input.auxiliarySource124=true;rejected(input,unsupported,"auxiliary124");
        input=saved;input.shapeType88=0;rejected(input,unsupported,"wrong shape");
        input=saved;input.hasPairTree=true;rejected(input,unsupported,"pair tree");
        for(auto n:{0u,2u,0xffffffffu}){input=saved;input.pairCount=n;rejected(input,unsupported,"pair count");}
        input=saved;input.scene.v[0]=std::numeric_limits<float>::infinity();rejected(input,invalid,"nonfinite scene");
        input=saved;input.scene.v[3]=1;rejected(input,invalid,"nonaffine basis");
        input=saved;input.scene.v[15]=0;rejected(input,invalid,"nonaffine translation");
        input=saved;input.lowerEndpoint140.v[1]=fp(0x7fc12345);rejected(input,invalid,"nonfinite lower endpoint");
        input=saved;input.state.segment.first.v[0]=fp(0x7fc12345);rejected(input,invalid,"nonfinite pair first");
        input=saved;input.state.segment.second.v[2]=std::numeric_limits<float>::infinity();rejected(input,invalid,"nonfinite pair second");
        const float large=std::numeric_limits<float>::max();
        input=saved;input.scene.v[0]=large;input.lowerEndpoint140.v[0]=2;rejected(input,invalid,"seed overflow");
        input=saved;input.state.segment.first.v[0]=large;input.state.segment.second.v[0]=large;rejected(input,invalid,"midpoint sum overflow");
        input=saved;input.state.segment.first.v[0]=large;input.state.segment.second.v[0]=-large;rejected(input,invalid,"half difference overflow");
        input=saved;input.state.segment.first.v[0]=1e30f;input.state.segment.second.v[0]=-1e30f;rejected(input,invalid,"radius square overflow");
        input=saved;input.lowerEndpoint140={};input.scene.v[0]=large;
        input.state.segment.first={{2,0,0}};input.state.segment.second={{2,0,0}};
        rejected(input,invalid,"sphere transform overflow");
        // Disabled/absent paths must not consume later shape/endpoint inputs.
        input=saved;input.queryEnabled100=false;input.shapeType88=99;input.pairCount=99;input.hasPairTree=true;
        input.state.segment.first.v[0]=fp(0x7fc12345);input.state.sphere.radius=-99;
        TerrainPrepareState48M output{};
        check(prepareTerrainSinglePair48M(input,output)==TerrainPrepareStatus48M::QueryDisabled,"disabled ignores shape");
        check(bits(output.segment.first.v[0])==0x7fc12345&&output.sphere.radius==-99,"disabled preserves unused pair/sphere");
        check(output.terrain.collectedCount==0&&output.terrain.currentCandidateIdentity==input.state.terrain.currentCandidateIdentity,"disabled resets count only");
        input.bodyLinked=false;input.auxiliarySource120=true;input.auxiliarySource124=true;input.scene.v[0]=fp(0x7fc12345);
        check(prepareTerrainSinglePair48M(input,output)==TerrainPrepareStatus48M::NoBody,"no body ignores all later inputs");
        check(words(output)==words(input.state),"no body preserves whole state");
        check(allocations==0,"numeric API allocations");
        check(statuses[0]&&statuses[1]&&statuses[2],"all successful boundaries covered");
        std::cout<<"PASS "<<checks<<" checks; "<<count<<" original-word numeric preparation cases; ready="<<statuses[0]
                 <<", disabled="<<statuses[1]<<", noBody="<<statuses[2]<<"; exact 67-word output, aliasing, malformed guards; allocations="
                 <<allocations<<"; stateBytes="<<sizeof(TerrainPrepareState48M)<<", inputBytes="<<sizeof(TerrainPrepareInput48M)<<'\n';
        return 0;
    }catch(const std::exception& e){trackAllocations=false;std::cerr<<"FAIL after "<<checks<<" checks, case="<<index<<": "<<e.what()<<'\n';return 1;}
}
