#include "../../src/game/collision_tree48i.h"
#include "../../tools/gameplay/query_fixture48i.h"
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
static float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
static std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
struct Reader {
    std::size_t cursor=0;
    std::uint32_t word(){if(cursor>=sizeof(queryFixtureData48I)/4)throw std::runtime_error("fixture truncated");return queryFixtureData48I[cursor++];}
    float real(){return fp(word());}
    template<class T>void vec(T& v){for(auto& f:v)f=real();}
};
int main(int argc,char** argv){
    unsigned checks=0,caseIndex=0,hits=0,misses=0,visits=0;
    try {
        auto check=[&](bool yes,const std::string& what){++checks;if(!yes)throw std::runtime_error(what);};
        check(argc==2,"repository root argument");
        std::fesetround(FE_TONEAREST);Reader r;
        if(r.word()!=0x49383452u||r.word()!=1)throw std::runtime_error("fixture format");
        std::vector<CollisionMesh48I> meshes(r.word());
        for(auto& m:meshes){
            const auto nv=r.word(),nn=r.word(),nt=r.word(),nf=r.word();r.vec(m.local_matrix);m.reverse_mode=r.word();
            m.vertices.resize(nv);m.vertex_normals.resize(nn);m.nodes.resize(nt);m.faces.resize(nf);
            for(auto& v:m.vertices)r.vec(v);for(auto& v:m.vertex_normals)r.vec(v);
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
            const auto before=output;std::vector<CollisionVisit48I> trace;
            const auto status=queryOrderedCollision48I(q,candidates.data(),candidates.size(),output,&trace);
            check(static_cast<unsigned>(status)==static_cast<unsigned>(expectedHit),"status got="+std::to_string(static_cast<unsigned>(status))+" expected="+std::to_string(expectedHit));
            check(trace.size()==expectedTrace.size(),"trace count got="+std::to_string(trace.size())+" expected="+std::to_string(expectedTrace.size()));
            unsigned nodes=0,pruned=0,leaves=0,leafHits=0;
            for(std::size_t i=0;i<trace.size();++i){
                const auto& a=trace[i];const auto& b=expectedTrace[i];
                check(a.candidateId==b.candidateId&&a.index==b.index&&a.leaf==b.leaf&&a.accepted==b.accepted,"trace mismatch visit="+std::to_string(i));
                if(b.leaf){++leaves;if(b.accepted)++leafHits;}else{++nodes;if(!b.accepted)++pruned;}
            }
            visits+=static_cast<unsigned>(trace.size());
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
        // Invalid topology terminates transactionally instead of cycling.
        auto mesh=meshes.back();mesh.nodes[0].children[0]=0;mesh.nodes[0].children[1]=0;
        BodyMatrix48G identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
        LocalSegment48I q{{{.25f,1.f,.125f}},{{.25f,-1.f,.125f}},identity};
        CollisionCandidate48I c{&mesh,identity,1};CollisionResult48I out{};const auto before=out;
        check(queryOrderedCollision48I(q,&c,1,out)==CollisionQueryStatus48I::InvalidInput,"cycle accepted");
        check(std::memcmp(&out,&before,sizeof(out))==0,"invalid changed output");
        std::cout<<"PASS "<<checks<<" checks; "<<caseIndex<<" original-word ordered queries, hits="<<hits<<", misses="<<misses<<", exact visits="<<visits<<"; 2 complete native cache meshes and source instance placements used\n";return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL after "<<checks<<" checks case="<<caseIndex<<": "<<e.what()<<"\n";return 1;}
}
