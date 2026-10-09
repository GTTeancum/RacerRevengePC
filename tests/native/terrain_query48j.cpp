#include "../../src/game/terrain_query48j.h"
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
            TerrainQueryResult48J result{};
            const auto status=queryTerrainLimited48J(query,candidates.data(),candidates.size(),result);
            check(status==TerrainQueryStatus48J::Complete,"status "+std::to_string(static_cast<unsigned>(status)));
            check(result.hasGeometryHit==hit,"geometry hit");check(result.queryFlagsF8==flags,"query flags");
            const auto& actual=result.terrain;const auto& a=actual.surface;const auto& b=expected.surface;
            check(a.forceAngleTest108==b.forceAngleTest108&&a.hit110==b.hit110&&a.hit114==b.hit114,"surface flags");
            for(unsigned k=0;k<3;++k){check(bits(a.point150.v[k])==bits(b.point150.v[k]),"surface point");check(bits(a.normal170.v[k])==bits(b.normal170.v[k]),"surface normal");}
            check(actual.collectedCount==expected.collectedCount,"callback count");
            for(unsigned k=0;k<actual.collectedCount;++k)check(actual.collectedIdentities[k]==expected.collectedIdentities[k],"callback identity order");
            check(actual.excludedOwnerIdentity==expected.excludedOwnerIdentity,"exclusion owner");check(actual.currentCandidateIdentity==expected.currentCandidateIdentity,"current candidate");
            check(result.candidates.size()==candidates.size(),"candidate effects count");
            for(unsigned k=0;k<order.size();++k){
                const auto& x=result.candidates[k];const auto& y=effects[order[k]];
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
        // Explicit unsupported paths must not quietly become no-contact results.
        auto invalid=savedCandidates;invalid[1].hasResponseCallbackE8=true;
        TerrainQueryResult48J output{};output.queryFlagsF8=0xab;output.terrain.surface.hit114=123;
        check(queryTerrainLimited48J(savedQuery,invalid.data(),invalid.size(),output)==TerrainQueryStatus48J::UnsupportedScope,"unknown response callback accepted");
        check(output.queryFlagsF8==0xab&&output.terrain.surface.hit114==123&&output.candidates.empty(),"unsupported callback changed output");
        invalid=savedCandidates;invalid[1].typeTag=savedQuery.groupTypeTag;
        check(queryTerrainLimited48J(savedQuery,invalid.data(),invalid.size(),output)==TerrainQueryStatus48J::UnsupportedScope,"group object accepted");
        check(output.queryFlagsF8==0xab&&output.terrain.surface.hit114==123&&output.candidates.empty(),"unsupported group changed output");
        savedQuery.prepared.collectedCount=17;
        check(queryTerrainLimited48J(savedQuery,savedCandidates.data(),savedCandidates.size(),output)==TerrainQueryStatus48J::InvalidInput,"callback overflow input accepted");
        std::cout<<"PASS "<<checks<<" checks; "<<index<<" original-word limited-list queries and contact-state transitions, hits="<<hits<<", misses="<<(index-hits)<<"; actual imported meshes and source instance placements; unsupported paths transactional\n";return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL after "<<checks<<" checks case="<<index<<": "<<e.what()<<"\n";return 1;}
}
