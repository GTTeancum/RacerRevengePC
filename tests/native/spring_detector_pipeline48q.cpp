// Test-only 48Q fixture and guard utilities. 48P files remain unchanged.
#include "spring_detector_pipeline48q.h"
#include "json.hpp"
#include <algorithm>
#include <array>
#include <cfenv>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#if defined(_MSC_VER)
#include <malloc.h>
#define RR_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_NOINLINE __attribute__((noinline))
#else
#define RR_NOINLINE
#endif
// Test-only instrumentation: setup/JSON/diagnostics are outside every hot scope.
static bool watchAllocations=false;
static std::size_t observedAllocations=0;
RR_NOINLINE void* operator new(std::size_t n){if(watchAllocations)++observedAllocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
RR_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void* operator new(std::size_t n,std::align_val_t alignment){
    if(watchAllocations)++observedAllocations;
    const auto a=static_cast<std::size_t>(alignment);
#if defined(_MSC_VER)
    void* p=_aligned_malloc(n?n:1,a);
#else
    void* p=nullptr;if(posix_memalign(&p,a,n?n:1))p=nullptr;
#endif
    if(p)return p;
    throw std::bad_alloc();
}
RR_NOINLINE void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
RR_NOINLINE void operator delete(void* p,std::align_val_t) noexcept {
#if defined(_MSC_VER)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
RR_NOINLINE void operator delete[](void* p,std::align_val_t a) noexcept {::operator delete(p,a);}
RR_NOINLINE void operator delete(void* p,std::size_t,std::align_val_t a) noexcept {::operator delete(p,a);}
RR_NOINLINE void operator delete[](void* p,std::size_t,std::align_val_t a) noexcept {::operator delete(p,a);}
using namespace rr::game;
using namespace rr::assets;
using Json=nlohmann::json;
namespace {
using Status=SpringDetectorPipelineStatus48Q;
template<class T>void fillSentinel(T* storage,std::size_t count,unsigned char value){
    static_assert(std::is_trivially_copyable_v<T>,"byte sentinels require trivially copyable storage");
    auto* bytes=reinterpret_cast<unsigned char*>(storage);
    std::fill_n(bytes,count*sizeof(T),value);
}
std::size_t checks=0,failures=0,differentialCases=0,noSpringCases=0,disabledCases=0,unsupportedCases=0,safetyCases=0,aliasCases=0,wiringCases=0;
std::size_t totalHits=0,totalGeometryHits=0,totalCallbacks=0,totalAppended=0,totalSaturated=0;
std::size_t maxStack=0,maxRecords=0,maxCandidates=0;
std::string label;
void check(bool yes,const char* message){++checks;if(!yes){++failures;if(failures<40)std::cerr<<label<<": "<<message<<'\n';}}
std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
std::int32_t signedWord(std::uint32_t b){std::int32_t i;std::memcpy(&i,&b,4);return i;}
std::array<std::uint32_t,11> stateWords(const SpringRecordState48N& s){return {s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
SpringRecordState48N state(const Json& j){if(j.size()!=11)throw std::runtime_error("state width");return {j[0],j[1],j[2],j[3],j[4],j[5],j[6],j[7],signedWord(j[8]),signedWord(j[9]),j[10]};}
SpringRecord48N record(const Json& j){if(j.size()!=24)throw std::runtime_error("record width");std::array<std::uint32_t,24> w{};for(unsigned i=0;i<24;++i)w[i]=j[i];SpringRecord48N r{};std::memcpy(&r,w.data(),sizeof(r));return r;}
void checkState(const SpringRecordState48N& actual,const Json& expected){const auto a=stateWords(actual);for(unsigned i=0;i<11;++i)check(a[i]==expected.at(i).get<std::uint32_t>(),"all raw11 state words");}
void checkRecords(const SpringRecord48N* records,std::size_t n,const Json& expected){check(expected.size()==n,"all record backing slots present");for(std::size_t i=0;i<n;++i){const auto e=record(expected.at(i));const auto* a=reinterpret_cast<const unsigned char*>(records+i);const auto* b=reinterpret_cast<const unsigned char*>(&e);for(std::size_t k=0;k<sizeof(e);++k)check(a[k]==b[k],"every record byte including stale fields and tail slots");}}
template<class T>void floats(const Json& j,T& target){if(j.size()!=std::size(target))throw std::runtime_error("float vector width");for(std::size_t i=0;i<j.size();++i)target[i]=j[i].template get<float>();}
template<class T>void rawFloats(const Json& j,T& target){if(j.size()!=std::size(target))throw std::runtime_error("raw float width");for(std::size_t i=0;i<j.size();++i)target[i]=fp(j[i].template get<std::uint32_t>());}
CollisionSphere48J sphere(const Json& j){return {{{j[0].get<float>(),j[1].get<float>(),j[2].get<float>()}},j[3].get<float>()};}
CollisionFilter48J filter(const Json& j){CollisionFilter48J f{};f.identity=j.at("identity");f.categoryA4=j.at("category");f.acceptMaskB8=j.at("accept_mask");f.flagsF8=j.at("flags");f.flags31=j.at("flags31");for(unsigned i=0;i<2;++i){f.identityLists[i].mode=j.at("identity_lists")[i].at("mode");f.identityLists[i].identities=j.at("identity_lists")[i].at("ids").get<std::vector<std::uint32_t>>();}f.categoryModeD4=j.at("category_mode");f.categoryMasks=j.at("category_masks").get<std::vector<std::uint32_t>>();return f;}
CollisionMesh48I mesh(const Json& j){CollisionMesh48I m{};for(unsigned i=0;i<16;++i)m.local_matrix[i]=(i%5)==0?1.f:0.f;m.vertices=j.at("vertices").get<decltype(m.vertices)>();m.vertex_normals=j.at("normals").get<decltype(m.vertex_normals)>();for(const auto& v:j.at("nodes")){CollisionMeshNode48I n{};n.volume=v.at("volume");floats(v.at("center"),n.center);floats(v.at("half"),n.half);n.children=v.at("children").get<decltype(n.children)>();m.nodes.push_back(n);}for(const auto& v:j.at("faces")){CollisionMeshFace48I f{};f.d=v.at("d");f.normal=v.at("normal").get<decltype(f.normal)>();f.vertices=v.at("vertices").get<decltype(f.vertices)>();m.faces.push_back(f);}if(j.at("root")!=0)throw std::runtime_error("native fixture requires root node0");return m;}
std::size_t stackBound(const CollisionMesh48I& m){std::vector<std::uint32_t> pending{0};std::size_t maximum=1,visits=0;while(!pending.empty()){if(++visits>m.nodes.size()+m.faces.size())throw std::runtime_error("non-tree fixture");auto at=pending.back();pending.pop_back();if(at&collision_mesh_leaf48i){if((at&~collision_mesh_leaf48i)>=m.faces.size())throw std::runtime_error("fixture leaf bounds");}else{const auto& n=m.nodes.at(at);pending.push_back(n.children[1]);pending.push_back(n.children[0]);maximum=std::max(maximum,pending.size());}}return maximum;}
std::string unhex(const std::string& s){if(s.size()%2)throw std::runtime_error("name hex width");std::string out;for(std::size_t i=0;i<s.size();i+=2)out.push_back(static_cast<char>(std::stoul(s.substr(i,2),nullptr,16)));return out;}
struct Snapshot {const void* p;std::vector<unsigned char> bytes;Snapshot(const void* data,std::size_t n):p(data),bytes(n){if(n)std::memcpy(bytes.data(),p,n);}void verify()const{check(bytes.empty()||std::memcmp(p,bytes.data(),bytes.size())==0,"transactional/immutable byte span");}};
void capture(std::vector<Snapshot>& saved,const void* p,std::size_t n){if(n)saved.emplace_back(p,n);}
template<class T>void captureVector(std::vector<Snapshot>& saved,const std::vector<T>& v){capture(saved,v.data(),v.size()*sizeof(T));}
void captureFilter(std::vector<Snapshot>& saved,const CollisionFilter48J& f){capture(saved,&f,sizeof(f));capture(saved,f.categoryMasks.data(),f.categoryMasks.capacity()*sizeof(std::uint32_t));for(const auto& l:f.identityLists)capture(saved,l.identities.data(),l.identities.capacity()*sizeof(std::uint32_t));}
void captureMesh(std::vector<Snapshot>& saved,const CollisionMesh48I& m){capture(saved,&m,sizeof(m));capture(saved,m.vertices.data(),m.vertices.capacity()*sizeof(m.vertices[0]));capture(saved,m.vertex_normals.data(),m.vertex_normals.capacity()*sizeof(m.vertex_normals[0]));capture(saved,m.nodes.data(),m.nodes.capacity()*sizeof(m.nodes[0]));capture(saved,m.faces.data(),m.faces.capacity()*sizeof(m.faces[0]));}
// Every supplied original array slot is represented; local scratch deliberately
// projects XYZ because unused VU-register W carry is not native game state.
template<class T> struct Guarded {
    std::vector<T> v;
    std::size_t used=0,capacity=0;
    Guarded(std::size_t n,std::size_t tail=2,unsigned char byte=0x5A):v(n+tail+2),used(n),capacity(n+tail){fillSentinel(v.data(),v.size(),byte);}
    T* data(){return v.data()+1;}const T* data()const{return v.data()+1;}
    void guards(std::vector<Snapshot>& s)const{capture(s,v.data(),sizeof(T));capture(s,data()+used,(capacity-used+1)*sizeof(T));}
};
struct Rays {
    Guarded<Vector3_48C> localStarts,localDirections;
    Guarded<SpringVector4_48N> worldStarts,worldDirections,normals,points;
    Guarded<std::uint32_t> hits;
    Guarded<float> fractions,lengths;
    explicit Rays(std::size_t n,unsigned char byte=0x5A):localStarts(n,2,byte),localDirections(n,2,byte),worldStarts(n,2,byte),worldDirections(n,2,byte),normals(n,2,byte),points(n,2,byte),hits(n,2,byte),fractions(n,2,byte),lengths(n,2,byte){}
    Type3RayBuffers48P buffers(){return {localStarts.data(),localDirections.data(),worldStarts.data(),worldDirections.data(),normals.data(),points.data(),hits.data(),fractions.data(),localStarts.capacity,lengths.data(),lengths.capacity};}
    Type3RaySource48P source()const{return {localStarts.data(),localDirections.data(),worldStarts.data(),worldDirections.data(),normals.data(),points.data(),hits.data(),fractions.data(),localStarts.used,lengths.data(),lengths.used};}
    void captureAll(std::vector<Snapshot>& s)const{captureVector(s,localStarts.v);captureVector(s,localDirections.v);captureVector(s,worldStarts.v);captureVector(s,worldDirections.v);captureVector(s,normals.v);captureVector(s,points.v);captureVector(s,hits.v);captureVector(s,fractions.v);captureVector(s,lengths.v);}
    void guards(std::vector<Snapshot>& s)const{localStarts.guards(s);localDirections.guards(s);worldStarts.guards(s);worldDirections.guards(s);normals.guards(s);points.guards(s);hits.guards(s);fractions.guards(s);lengths.guards(s);}
    void initialize(const Json& pair,const Json& prep){
        for(std::size_t i=0;i<localStarts.used;++i){
            for(unsigned k=0;k<3;++k){localStarts.data()[i].v[k]=fp(prep.at("local_starts")[i*4+k]);localDirections.data()[i].v[k]=fp(prep.at("local_directions")[i*4+k]);}
            for(unsigned k=0;k<4;++k){worldStarts.data()[i].v[k]=fp(prep.at("world_starts")[i*4+k]);worldDirections.data()[i].v[k]=fp(prep.at("world_directions")[i*4+k]);normals.data()[i].v[k]=fp(pair.at("normals")[i*4+k]);points.data()[i].v[k]=fp(pair.at("points")[i*4+k]);}
            hits.data()[i]=pair.at("hit_flags")[i];fractions.data()[i]=fp(pair.at("fractions")[i]);lengths.data()[i]=fp(prep.at("lengths")[i]);
        }
    }
};
void raw4(const Json& j,std::size_t offset,SpringVector4_48N& v){for(unsigned k=0;k<4;++k)v.v[k]=fp(j[offset+k]);}
Type3QueryState48P queryState(const Json& pair,const Json& prep){
    Type3QueryState48P s{};
    for(unsigned k=0;k<3;++k)s.localCenter.v[k]=fp(prep.at("centers")[k]);
    raw4(prep.at("centers"),4,s.worldCenter);raw4(prep.at("query_sphere"),0,s.queryCenter);
    s.radius=fp(prep.at("radius"));s.queryRadius=fp(prep.at("query_sphere")[4]);
    raw4(pair.at("selected"),0,s.result00);raw4(pair.at("selected"),4,s.result10);raw4(pair.at("selected"),8,s.selectedNormal);raw4(pair.at("selected"),12,s.selectedPoint);raw4(pair.at("selected"),16,s.selectedCorrection);
    s.hitCount=pair.at("count");s.selectedFraction=fp(pair.at("selected_fraction"));s.opaque78=pair.at("other_fields")[0];s.opaque7c=pair.at("other_fields")[1];return s;
}
void check4(const SpringVector4_48N& a,const Json& j,std::size_t offset,const char* why){for(unsigned k=0;k<4;++k)check(bits(a.v[k])==j[offset+k].get<std::uint32_t>(),why);}
void compareState(const Type3QueryState48P& a,const Json& pair,const Json& prep){
    for(unsigned k=0;k<3;++k)check(bits(a.localCenter.v[k])==prep.at("centers")[k].get<std::uint32_t>(),"local center XYZ");
    check4(a.worldCenter,prep.at("centers"),4,"world center full4");check4(a.queryCenter,prep.at("query_sphere"),0,"query center full4");
    check(bits(a.radius)==prep.at("radius").get<std::uint32_t>()&&bits(a.queryRadius)==prep.at("query_sphere")[4].get<std::uint32_t>(),"shape/query radii");
    check4(a.result00,pair.at("selected"),0,"result00 full4");check4(a.result10,pair.at("selected"),4,"result10 full4");check4(a.selectedNormal,pair.at("selected"),8,"selected normal full4");check4(a.selectedPoint,pair.at("selected"),12,"selected point full4");check4(a.selectedCorrection,pair.at("selected"),16,"selected correction full4");
    check(a.hitCount==pair.at("count"),"geometric face hit count");check(bits(a.selectedFraction)==pair.at("selected_fraction").get<std::uint32_t>(),"strict selected fraction raw bits");check(a.opaque78==pair.at("other_fields")[0]&&a.opaque7c==pair.at("other_fields")[1],"opaque result words preserved");
}
void compareRays(const Rays& a,const Json& pair,const Json& prep){
    for(std::size_t i=0;i<a.localStarts.used;++i){
        for(unsigned k=0;k<3;++k){check(bits(a.localStarts.data()[i].v[k])==prep.at("local_starts")[4*i+k].get<std::uint32_t>(),"all local starts XYZ including stale tail");check(bits(a.localDirections.data()[i].v[k])==prep.at("local_directions")[4*i+k].get<std::uint32_t>(),"all local directions XYZ including stale tail");}
        check4(a.worldStarts.data()[i],prep.at("world_starts"),4*i,"all world starts full4");check4(a.worldDirections.data()[i],prep.at("world_directions"),4*i,"all world directions full4");check4(a.normals.data()[i],pair.at("normals"),4*i,"all normals full4");check4(a.points.data()[i],pair.at("points"),4*i,"all points full4");
        check(a.hits.data()[i]==pair.at("hit_flags")[i],"all hit flags active and stale");check(bits(a.fractions.data()[i])==pair.at("fractions")[i].get<std::uint32_t>(),"all fractions active and stale");check(bits(a.lengths.data()[i])==prep.at("lengths")[i].get<std::uint32_t>(),"all lengths active and stale");
    }
}
struct DetectorArrays {
    Guarded<SpringDetectorScene48O> scenes;
    Guarded<SpringVector4_48N> pairs,staged;
    Guarded<std::uint32_t> metadata;
    DetectorArrays(std::size_t sceneCount,std::size_t vectors,unsigned char byte=0x5A):scenes(sceneCount,2,byte),pairs(vectors,2,byte),staged(vectors,2,byte),metadata(vectors,2,byte){}
    SpringDetectorBuffers48O buffers(){return {scenes.data(),scenes.capacity,pairs.data(),pairs.capacity,staged.data(),staged.capacity,metadata.data(),metadata.capacity};}
    void captureAll(std::vector<Snapshot>& s)const{captureVector(s,scenes.v);captureVector(s,pairs.v);captureVector(s,staged.v);captureVector(s,metadata.v);}
    void guards(std::vector<Snapshot>& s)const{scenes.guards(s);pairs.guards(s);staged.guards(s);metadata.guards(s);}
};
void* detectorPointer(SpringDetectorBuffers48O& b,unsigned i){switch(i){case 0:return b.scenes;case 1:return b.pairs;case 2:return b.staged;default:return b.stagedMetadata;}}
void setDetectorPointer(SpringDetectorBuffers48O& b,unsigned i,void* p){switch(i){case 0:b.scenes=static_cast<SpringDetectorScene48O*>(p);break;case 1:b.pairs=static_cast<SpringVector4_48N*>(p);break;case 2:b.staged=static_cast<SpringVector4_48N*>(p);break;default:b.stagedMetadata=static_cast<std::uint32_t*>(p);break;}}
void detectorCapacity(SpringDetectorBuffers48O& b,unsigned i,std::size_t n){switch(i){case 0:b.sceneCapacity=n;break;case 1:b.pairVectorCapacity=n;break;case 2:b.stagedVectorCapacity=n;break;default:b.stagedMetadataCapacity=n;break;}}
void* rayPointer(Type3RayBuffers48P& r,unsigned i){switch(i){case 0:return r.localStarts;case 1:return r.localDirections;case 2:return r.worldStarts;case 3:return r.worldDirections;case 4:return r.normals;case 5:return r.points;case 6:return r.hits;case 7:return r.fractions;default:return r.lengths;}}
void setRayPointer(Type3RayBuffers48P& r,unsigned i,void* p){switch(i){case 0:r.localStarts=static_cast<Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<SpringVector4_48N*>(p);break;case 5:r.points=static_cast<SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<std::uint32_t*>(p);break;case 7:r.fractions=static_cast<float*>(p);break;default:r.lengths=static_cast<float*>(p);break;}}
void setSourceRay(Type3RaySource48P& r,unsigned i,const void* p){switch(i){case 0:r.localStarts=static_cast<const Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<const Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<const SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<const SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<const SpringVector4_48N*>(p);break;case 5:r.points=static_cast<const SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<const std::uint32_t*>(p);break;case 7:r.fractions=static_cast<const float*>(p);break;default:r.lengths=static_cast<const float*>(p);break;}}
void identity(BodyMatrix48G& m){m={};for(unsigned i=0;i<4;++i)m.v[5*i]=1.f;}
std::size_t rayBytes(const Type3RayBuffers48P& r){return r.capacity*(2*sizeof(Vector3_48C)+4*sizeof(SpringVector4_48N)+sizeof(std::uint32_t)+sizeof(float))+r.lengthCapacity*sizeof(float);}
std::size_t detectorBytes(const SpringDetectorBuffers48O& b){return b.sceneCapacity*sizeof(SpringDetectorScene48O)+(b.pairVectorCapacity+b.stagedVectorCapacity)*sizeof(SpringVector4_48N)+b.stagedMetadataCapacity*sizeof(std::uint32_t);}
std::size_t queryBytes(const Type3QueryBuffers48P& b){return rayBytes(b.rays)+b.recordCapacity*sizeof(SpringRecord48N)+b.effectCapacity*sizeof(TerrainCandidateEffects48J);}
struct Storage {
    // Test-only owners. The API receives non-owning bounded descriptors only.
    std::array<SpringDetectorPoint48O,4> points{};
    std::array<SpringDetectorScene48O,4> scenes{};
    std::array<SpringVector4_48N,8> pairs{},staged{};
    std::array<std::uint32_t,8> metadata{};
    DetectorArrays detectorScratch{4,8,0xA5},detectorPrepared{4,8,0xB6},detectorDestination{4,8};
    Rays sourceRays{4,0xCD},scratchRays{4,0xA5},preparedRays{4,0xB6},destinationRays{4};
    Guarded<SpringRecord48N> sourceRecords{4,2,0xCD},scratchRecords{4,2,0xA5},preparedRecords{4,2,0xB6},destinationRecords{4};
    Guarded<TerrainCandidateEffects48J> scratchEffects{2,2,0xA5},preparedEffects{2,2,0xB6},destinationEffects{2};
    Guarded<std::uint32_t> stack{16,0,0xA5};
    CollisionMesh48I m{};
    std::array<Type3Candidate48P,2> candidates{};
    alignas(std::max_align_t) char names[2][64]{};
    Storage(){
        for(unsigned i=0;i<4;++i){identity(scenes[i].world);identity(scenes[i].local);scenes[i].flags=0xCAFE0001;points[i].sceneIndex=i;points[i].local={{.25f,(i&1)?-1.f:1.f,.125f,-9.f}};}
        for(unsigned i=0;i<8;++i){pairs[i]={{.25f,(i&1)?-1.f:1.f,.125f,float(i)}};staged[i]={{float(400+i),float(500+i),float(600+i),float(700+i)}};metadata[i]=0xABC00000u+i;}
        for(unsigned i=0;i<4;++i){sourceRays.localStarts.data()[i]={{3,4,5}};sourceRays.localDirections.data()[i]={{6,7,8}};sourceRays.worldStarts.data()[i]={{9,10,11,12}};sourceRays.worldDirections.data()[i]={{13,14,15,16}};sourceRays.normals.data()[i]={{17,18,19,20}};sourceRays.points.data()[i]={{21,22,23,24}};sourceRays.hits.data()[i]=0;sourceRays.fractions.data()[i]=.75f;sourceRays.lengths.data()[i]=float(i+1);}
        m.vertices={{{-4,-.5f,-4}},{{0,-.5f,4}},{{4,-.5f,-4}},{{16,-.5f,16}},{{20,-.5f,24}},{{24,-.5f,16}}};m.vertex_normals.assign(6,{{0,1,0}});m.nodes.push_back({100,{{10,-.5f,10}},{{20,1,20}},{{collision_mesh_leaf48i,collision_mesh_leaf48i|1}}});m.faces.push_back({.5f,{{0,32767,0}},{{0,1,2}}});m.faces.push_back({.5f,{{0,32767,0}},{{3,4,5}}});
    }
    SpringDetectorSource48O detectorSource(){return {points.data(),points.size(),scenes.data(),scenes.size(),pairs.data(),pairs.size(),staged.data(),staged.size(),metadata.data(),metadata.size()};}
    Type3QueryBuffers48P queryScratch(){return {scratchRays.buffers(),scratchRecords.data(),scratchRecords.capacity,scratchEffects.data(),scratchEffects.capacity};}
    Type3QueryBuffers48P queryPrepared(){return {preparedRays.buffers(),preparedRecords.data(),preparedRecords.capacity,preparedEffects.data(),preparedEffects.capacity};}
    Type3QueryBuffers48P queryDestination(){return {destinationRays.buffers(),destinationRecords.data(),destinationRecords.capacity,destinationEffects.data(),destinationEffects.capacity};}
    void immutable(std::vector<Snapshot>& s)const{capture(s,points.data(),sizeof(points));capture(s,scenes.data(),sizeof(scenes));capture(s,pairs.data(),sizeof(pairs));capture(s,staged.data(),sizeof(staged));capture(s,metadata.data(),sizeof(metadata));sourceRays.captureAll(s);captureVector(s,sourceRecords.v);capture(s,candidates.data(),sizeof(candidates));for(const auto& c:candidates)captureFilter(s,c.collision.filter);capture(s,names,sizeof(names));captureMesh(s,m);}
    void destinations(std::vector<Snapshot>& s)const{detectorDestination.captureAll(s);destinationRays.captureAll(s);captureVector(s,destinationRecords.v);captureVector(s,destinationEffects.v);}
    void guards(std::vector<Snapshot>& s)const{detectorScratch.guards(s);detectorPrepared.guards(s);detectorDestination.guards(s);scratchRays.guards(s);preparedRays.guards(s);destinationRays.guards(s);scratchRecords.guards(s);preparedRecords.guards(s);destinationRecords.guards(s);scratchEffects.guards(s);preparedEffects.guards(s);destinationEffects.guards(s);stack.guards(s);}
};
struct Safety:Storage {
    SpringDetectorPipelineInput48Q input{};
    SpringDetectorPipelineSource48Q source{};
    SpringDetectorPipelineWorkspace48Q workspace{};
    SpringDetectorPipelineBuffers48Q destination{};
    SpringDetectorPipelineResult48Q out{};
    const Type3Candidate48P* candidatePointer=candidates.data();
    SpringDetectorPipelineResult48Q* outputPointer=&out;
    std::size_t count=2;
    Safety(){
        auto& d=input.detector;auto& q=input.query;
        m.vertices.reserve(10);m.vertex_normals.reserve(10);m.nodes.reserve(4);m.faces.reserve(4);
        d.shapePairCount=2;d.state.spring={11,12,13,14,15,16,0,2,0,4,21};d.state.center={{101,102,103,-11}};d.state.half={{201,202,203,-12}};
        q.sentinel={{0,0,0,1}};q.skipMask=8;q.dynamicMask=16;q.materialMask=4;q.collisionTypeTag=7;q.groupTypeTag=9;q.initial.opaque78=0x13572468;q.initial.opaque7c=0x24681357;q.initial.selectedFraction=.125f;identity(q.bodyScene);
        // These are intentionally contradictory: pipeline owns these fields.
        q.spring.generatedCount84=-17;q.pairCount=0;q.resetSpring=true;q.entry=Type3QueryEntry48P::Dispatcher;
        q.filter.identity=100;q.filter.categoryA4=4;q.filter.flagsF8=4;q.filter.flags31=1;q.filter.categoryMasks={0};q.filter.identityLists[0].identities={900};q.filter.identityLists[1].identities={901};q.copied40={{1,2,3,4}};q.copied50={{5,6,7,8}};
        q.filter.categoryMasks.reserve(8);for(auto& list:q.filter.identityLists)list.identities.reserve(8);
        for(unsigned i=0;i<2;++i){std::strcpy(names[i],"trackfr7.col");auto& c=candidates[i];c.collision.geometry={&m,q.bodyScene,10+i};c.collision.filter=q.filter;c.collision.filter.identity=10+i;c.collision.sphere={{{0,0,0}},100};c.collision.typeTag=7;c.collision.copied40={{11,12,13,14}};c.collision.copied50={{15,16,17,18}};c.hasMaterialScene=true;c.sceneName=names[i];c.sceneNameBytes=sizeof(names[i]);c.collision.filter.categoryMasks.reserve(8);for(auto& list:c.collision.filter.identityLists)list.identities.reserve(8);}
        source={detectorSource(),sourceRays.source(),sourceRecords.data(),sourceRecords.used};workspace={detectorScratch.buffers(),detectorPrepared.buffers(),{queryScratch(),stack.data(),stack.capacity},queryPrepared()};destination={detectorDestination.buffers(),queryDestination()};fillSentinel(&out,1,0x7B);
    }
    Status call(){watchAllocations=true;const auto s=runSpringDetectorPipeline48Q(input,source,candidatePointer,count,workspace,destination,*outputPointer);watchAllocations=false;return s;}
    SpringDetectorBuffers48O& detector(unsigned n){return n==0?workspace.detectorScratch:n==1?workspace.detectorPrepared:destination.detector;}
    Type3QueryBuffers48P& query(unsigned n){return n==0?workspace.queryScratch.buffers:n==1?workspace.queryPrepared:destination.query;}
    void* write(unsigned i){if(i<12)return detectorPointer(detector(i/4),i%4);if(i<45){auto& b=query((i-12)/11);unsigned k=(i-12)%11;if(k<9)return rayPointer(b.rays,k);return k==9?static_cast<void*>(b.records):static_cast<void*>(b.effects);}return i==45?static_cast<void*>(workspace.queryScratch.treeStack):static_cast<void*>(outputPointer);}
    void setWrite(unsigned i,void* p){if(i<12){setDetectorPointer(detector(i/4),i%4,p);return;}if(i<45){auto& b=query((i-12)/11);unsigned k=(i-12)%11;if(k<9)setRayPointer(b.rays,k,p);else if(k==9)b.records=static_cast<SpringRecord48N*>(p);else b.effects=static_cast<TerrainCandidateEffects48J*>(p);return;}if(i==45)workspace.queryScratch.treeStack=static_cast<std::uint32_t*>(p);else outputPointer=static_cast<SpringDetectorPipelineResult48Q*>(p);}
    void capacity(unsigned i,std::size_t n){if(i<12){detectorCapacity(detector(i/4),i%4,n);return;}if(i<45){auto& b=query((i-12)/11);unsigned k=(i-12)%11;if(k<8)b.rays.capacity=n;else if(k==8)b.rays.lengthCapacity=n;else if(k==9)b.recordCapacity=n;else b.effectCapacity=n;return;}workspace.queryScratch.treeStackCapacity=n;}
    std::vector<void*> reads(){
        std::vector<void*> r{&input,&source,&workspace,&destination,points.data(),scenes.data(),pairs.data(),staged.data(),metadata.data(),sourceRecords.data(),candidates.data(),&m,m.vertices.data(),m.vertex_normals.data(),m.nodes.data(),m.faces.data(),m.vertices.data()+m.vertices.size(),m.vertex_normals.data()+m.vertex_normals.size(),m.nodes.data()+m.nodes.size(),m.faces.data()+m.faces.size(),names[0],names[1]};
        auto filterReads=[&](CollisionFilter48J& f){r.push_back(&f);r.push_back(f.categoryMasks.data());r.push_back(f.categoryMasks.data()+f.categoryMasks.size());for(auto& l:f.identityLists){r.push_back(l.identities.data());r.push_back(l.identities.data()+l.identities.size());}};filterReads(input.query.filter);for(auto& c:candidates)filterReads(c.collision.filter);
        auto b=sourceRays.buffers();for(unsigned i=0;i<9;++i)r.push_back(rayPointer(b,i));return r;
    }
    std::vector<Snapshot> saved(){std::vector<Snapshot> s;Storage::immutable(s);Storage::destinations(s);capture(s,&input,sizeof(input));captureFilter(s,input.query.filter);capture(s,&source,sizeof(source));capture(s,&workspace,sizeof(workspace));capture(s,&destination,sizeof(destination));capture(s,&out,sizeof(out));return s;}
    void reject(Status wanted=Status::InvalidInput,bool alias=false){auto snapshots=saved();const auto before=observedAllocations;const auto status=call();check(status==wanted,"expected transactional rejection");if(status!=wanted&&failures<40)std::cerr<<label<<": status="<<static_cast<unsigned>(status)<<" expected="<<static_cast<unsigned>(wanted)<<'\n';for(const auto& s:snapshots)s.verify();check(observedAllocations==before,"rejection allocation-free");++safetyCases;aliasCases+=alias;}
};
template<class F>void rejection(const std::string& name,F change,Status wanted=Status::InvalidInput){label=name;Safety s;change(s);s.reject(wanted);}
void safety(){
    label="synthetic composed baseline";{Safety s;auto saved=s.saved();std::vector<Snapshot> guards;s.guards(guards);check(s.call()==Status::Complete,"composed baseline complete");check(s.out.detectorReturnValue==1&&s.out.detectorReturned&&s.out.queryExecuted,"detector return and query execution separate");check(s.out.query.hasAnyHit&&s.out.query.callbacks==2&&s.out.query.appended==2,"two candidate callbacks");for(const auto& g:guards)g.verify();}
    for(unsigned w=0;w<46;++w){rejection("null writable "+std::to_string(w),[&](auto& s){s.setWrite(w,nullptr);});rejection("misaligned writable "+std::to_string(w),[&](auto& s){s.setWrite(w,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(s.write(w))+1));});rejection("overflow writable "+std::to_string(w),[&](auto& s){s.capacity(w,std::numeric_limits<std::size_t>::max());});rejection("wrapping writable "+std::to_string(w),[&](auto& s){s.setWrite(w,reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15)));});}
    for(unsigned a=0;a<47;++a)for(unsigned b=a+1;b<47;++b){label="all-stage mutable alias "+std::to_string(a)+":"+std::to_string(b);Safety s;s.setWrite(a,s.write(b));s.reject(Status::InvalidInput,true);}
    std::size_t readCount;{Safety s;readCount=s.reads().size();}
    for(unsigned w=0;w<47;++w)for(std::size_t r=0;r<readCount;++r){label="all-stage nested input alias "+std::to_string(w)+":"+std::to_string(r);Safety s;auto* p=s.reads()[r];if(w==46&&reinterpret_cast<std::uintptr_t>(p)%alignof(SpringDetectorPipelineResult48Q))continue;s.setWrite(w,p);s.reject(Status::InvalidInput,true);}
    for(unsigned r=0;r<9;++r){rejection("null source ray",[&](auto& s){setSourceRay(s.source.rays,r,nullptr);});rejection("misaligned source ray",[&](auto& s){auto b=s.sourceRays.buffers();setSourceRay(s.source.rays,r,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(rayPointer(b,r))+1));});rejection("wrapping source ray",[&](auto& s){setSourceRay(s.source.rays,r,reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15)));});}
    for(unsigned r=0;r<7;++r)for(unsigned mode=0;mode<3;++mode)rejection("invalid declared detector/records/candidate input",[&](auto& s){
        const void* pointer=nullptr;switch(r){case 0:pointer=s.source.detector.points;break;case 1:pointer=s.source.detector.scenes;break;case 2:pointer=s.source.detector.pairs;break;case 3:pointer=s.source.detector.staged;break;case 4:pointer=s.source.detector.stagedMetadata;break;case 5:pointer=s.source.records;break;default:pointer=s.candidatePointer;break;}
        const auto address=mode==0?std::uintptr_t(0):mode==1?reinterpret_cast<std::uintptr_t>(pointer)+1:std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15);
        switch(r){case 0:s.source.detector.points=reinterpret_cast<const SpringDetectorPoint48O*>(address);break;case 1:s.source.detector.scenes=reinterpret_cast<const SpringDetectorScene48O*>(address);break;case 2:s.source.detector.pairs=reinterpret_cast<const SpringVector4_48N*>(address);break;case 3:s.source.detector.staged=reinterpret_cast<const SpringVector4_48N*>(address);break;case 4:s.source.detector.stagedMetadata=reinterpret_cast<const std::uint32_t*>(address);break;case 5:s.source.records=reinterpret_cast<const SpringRecord48N*>(address);break;default:s.candidatePointer=reinterpret_cast<const Type3Candidate48P*>(address);break;}
    });
    for(unsigned r=0;r<10;++r)rejection("overflow declared source or nested count",[&](auto& s){const auto n=std::numeric_limits<std::size_t>::max();switch(r){case 0:s.source.detector.pointCapacity=n;break;case 1:s.source.detector.sceneCount=n;s.destination.detector.sceneCapacity=n;break;case 2:s.source.detector.pairVectorCount=n;s.destination.detector.pairVectorCapacity=n;break;case 3:s.source.detector.stagedVectorCount=n;s.destination.detector.stagedVectorCapacity=n;break;case 4:s.source.detector.stagedMetadataCount=n;s.destination.detector.stagedMetadataCapacity=n;break;case 5:s.source.rays.capacity=n;s.destination.query.rays.capacity=n;break;case 6:s.source.rays.lengthCapacity=n;s.destination.query.rays.lengthCapacity=n;break;case 7:s.source.recordCount=n;s.destination.query.recordCapacity=n;break;case 8:s.count=n;break;default:s.candidates[1].sceneNameBytes=n;break;}});
    rejection("misaligned nested mesh descriptor",[](auto& s){s.candidates[1].collision.geometry.mesh=reinterpret_cast<const CollisionMesh48I*>(reinterpret_cast<std::uintptr_t>(&s.m)+1);});
    for(unsigned n=0;n<3;++n)for(unsigned k=0;k<4;++k)rejection("short detector capacity",[&](auto& s){detectorCapacity(s.detector(n),k,k?7:3);},Status::InsufficientCapacity);
    for(unsigned n=0;n<3;++n)for(unsigned k=0;k<4;++k)rejection("short query capacity",[&](auto& s){auto& b=s.query(n);switch(k){case 0:b.rays.capacity=3;break;case 1:b.rays.lengthCapacity=3;break;case 2:b.recordCapacity=3;break;default:b.effectCapacity=1;break;}},Status::InsufficientCapacity);
    rejection("short tree stack",[](auto& s){s.workspace.queryScratch.treeStackCapacity=1;},Status::InsufficientCapacity);
    rejection("short point backing",[](auto& s){s.source.detector.pointCapacity=1;},Status::InsufficientCapacity);
    rejection("short pair backing",[](auto& s){s.source.detector.pairVectorCount=3;},Status::InsufficientCapacity);
    rejection("short stage backing",[](auto& s){s.source.detector.stagedVectorCount=3;},Status::InsufficientCapacity);
    rejection("short metadata backing",[](auto& s){s.source.detector.stagedMetadataCount=3;},Status::InsufficientCapacity);
    rejection("short independent shape count backing",[](auto& s){s.input.detector.shapePairCount=5;},Status::InsufficientCapacity);
    rejection("nonpositive active count",[](auto& s){s.input.detector.state.spring.pointCount7C=0;});
    rejection("negative active count",[](auto& s){s.input.detector.state.spring.pointCount7C=0xffffffff;});
    rejection("shape doubled count overflow",[](auto& s){s.input.detector.shapePairCount=0x40000000;});
    rejection("query shape absent",[](auto& s){s.input.detector.shapePresent=false;},Status::UnsupportedScope);
    rejection("provider reached",[](auto& s){s.input.detector.providerPresent=s.input.detector.providerEnabled=true;},Status::UnsupportedScope);
    rejection("dirty parent reached late",[](auto& s){s.scenes[1].flags=2;s.scenes[1].parentPresent=true;},Status::UnsupportedScope);
    rejection("late bad detector index",[](auto& s){s.points[1].sceneIndex=99;});
    rejection("late nonfinite point",[](auto& s){s.points[1].local.v[0]=std::numeric_limits<float>::infinity();});
    rejection("late detector transform overflow",[](auto& s){s.points[1].local.v[0]=std::numeric_limits<float>::max();s.scenes[1].world.v[0]=2;});
    rejection("late compound after detector and first hit",[](auto& s){s.scenes[0].flags=0;s.candidates[1].shapeType=1;},Status::UnsupportedScope);
    rejection("late dynamic callback",[](auto& s){s.candidates[1].collision.filter.categoryA4|=s.input.query.dynamicMask;},Status::UnsupportedScope);
    rejection("late response callback",[](auto& s){s.candidates[1].collision.hasResponseCallbackE8=true;},Status::UnsupportedScope);
    rejection("late group recursion",[](auto& s){s.candidates[1].collision.typeTag=s.input.query.groupTypeTag;},Status::UnsupportedScope);
    rejection("late absent body",[](auto& s){s.input.query.bodyScenePresent=false;});
    rejection("late binding out of range",[](auto& s){s.input.bodySceneIndex=99;});
    rejection("late negative binding",[](auto& s){s.input.bodySceneIndex=-2;});
    rejection("normal helper reached",[](auto& s){s.input.query.normalMode=2;},Status::UnsupportedScope);
    rejection("late interpolation boundary",[](auto& s){s.input.query.normalMode=1;s.candidates[1].collision.geometry.worldMatrix.v[12]=4.25f;s.candidates[1].collision.geometry.worldMatrix.v[14]=4.125f;},Status::UnsupportedInterpolationBoundary);
    rejection("late malformed tree cycle",[](auto& s){s.m.nodes[0].children[0]=0;});
    rejection("late malformed reached node",[](auto& s){s.m.nodes[0].children[1]=99;});
    rejection("late malformed leaf",[](auto& s){s.m.nodes[0].children[1]=collision_mesh_leaf48i|99;});
    rejection("late malformed vertex",[](auto& s){s.m.faces[1].vertices[0]=99;});
    rejection("late nonfinite vertex",[](auto& s){s.m.vertices[0][0]=std::numeric_limits<float>::quiet_NaN();});
    rejection("late identity mismatch",[](auto& s){s.candidates[1].collision.geometry.id=99;});
    rejection("late null mesh",[](auto& s){s.candidates[1].collision.geometry.mesh=nullptr;});
    rejection("late reversed mesh",[](auto& s){s.m.reverse_mode=1;},Status::UnsupportedScope);
    rejection("late unterminated name",[](auto& s){std::memset(s.names[1],'x',sizeof(s.names[1]));});
    rejection("late unsupported name",[](auto& s){std::strcpy(s.names[1],"ab");},Status::UnsupportedScope);
    rejection("tail-only detector overlap",[](auto& s){s.workspace.detectorScratch.pairs=s.detectorDestination.pairs.v.data();s.workspace.detectorScratch.pairVectorCapacity=11;s.destination.detector.pairs=s.detectorDestination.pairs.v.data()+8;s.destination.detector.pairVectorCapacity=1;s.source.detector.pairVectorCount=1;});
    rejection("tail-only detector/query cross overlap",[](auto& s){s.workspace.queryPrepared.rays.worldStarts=s.detectorDestination.pairs.v.data()+8;s.workspace.queryPrepared.rays.capacity=1;s.source.rays.capacity=1;});
    rejection("tail-only source/destination overlap",[](auto& s){s.destination.detector.pairs=s.pairs.data();s.source.detector.pairs=s.pairs.data()+7;s.source.detector.pairVectorCount=1;});
    rejection("tail-only records cross-stage overlap",[](auto& s){s.workspace.queryPrepared.records=s.destinationRecords.v.data()+5;s.workspace.queryPrepared.recordCapacity=1;s.source.recordCount=1;});
    rejection("tail-only effects cross-stage overlap",[](auto& s){s.workspace.queryPrepared.effects=s.destinationEffects.v.data()+3;s.workspace.queryPrepared.effectCapacity=1;s.count=1;});
    for(unsigned w=0;w<46;++w)rejection("no spring retains declared span validation",[&](auto& s){s.input.detector.springPresent=false;s.setWrite(w,nullptr);});
}
void compareDetector(const SpringDetectorState48O& actual,const SpringDetectorBuffers48O& buffers,const Json& e,bool compareSpring=true){
    check4(actual.center,e.at("bounds"),0,"complete detector center");check4(actual.half,e.at("bounds"),4,"complete detector half");check(actual.queryType==e.at("query_type"),"detector query type");if(compareSpring)checkState(actual.spring,e.at("spring_state"));
    for(std::size_t i=0;i<e.at("pairs").size()/4;++i)check4(buffers.pairs[i],e.at("pairs"),4*i,"all detector pair endpoint lanes and tails");
    for(std::size_t i=0;i<e.at("staged").size()/4;++i)check4(buffers.staged[i],e.at("staged"),4*i,"all staged endpoint lanes and tails");
    for(std::size_t i=0;i<e.at("metadata").size();++i)check(buffers.stagedMetadata[i]==e.at("metadata")[i],"all staged metadata including zero writes and stale tails");
    for(std::size_t i=0;i<e.at("scenes").size();++i){const auto& a=buffers.scenes[i];const auto& b=e.at("scenes")[i];for(unsigned k=0;k<16;++k){check(bits(a.world.v[k])==b.at("world")[k].get<std::uint32_t>(),"complete scene world cache raw words");check(bits(a.local.v[k])==b.at("scene_local")[k].get<std::uint32_t>(),"complete scene local raw words");}check(a.flags==b.at("flags"),"scene full cache flags");check(a.parentPresent==(b.at("parent").get<std::uint32_t>()!=0),"scene opaque parent presence");}
}
void differential(const Json& j,const std::vector<CollisionMesh48I>& meshes,std::size_t stackCapacity){
    label=j.at("name").get<std::string>();const auto& e=j.at("expected");const auto& dj=j.at("detector");const auto& initial=e.at("initial");const auto& p=initial.at("pair");const auto& prep=initial.at("prepared");const std::size_t slots=p.at("hit_flags").size();
    SpringDetectorPipelineInput48Q input{};auto& d=input.detector;auto& q=input.query;
    d.springPresent=dj.at("spring_present");d.pairsPresent=dj.at("pairs_present");d.providerPresent=dj.at("provider_present");d.providerEnabled=dj.at("provider_enabled");d.queryPresent=dj.at("query_present");d.queryEnabled=dj.at("query_enabled");d.resetSpring=dj.at("reset");d.explicitList=dj.at("explicit_list");d.shapePresent=dj.at("shape_present");d.shapePairCount=dj.at("shape_count");d.state.queryType=dj.at("initial_query_type");d.state.spring=state(j.at("initial_state"));raw4(dj.at("bounds"),0,d.state.center);raw4(dj.at("bounds"),4,d.state.half);input.bodySceneIndex=dj.at("body_scene_binding");
    q.initial=queryState(p,prep);q.filter=filter(j.at("query").at("filter"));q.normalMode=j.at("mode");q.sentinel={{0,0,0,1}};q.skipMask=j.at("masks")[0];q.dynamicMask=j.at("masks")[1];q.materialMask=j.at("masks")[2];floats(j.at("body_world"),q.bodyScene.v);
    q.spring.generatedCount84=-17;q.pairCount=0xffffffff;q.resetSpring=!d.resetSpring;q.entry=Type3QueryEntry48P::Dispatcher;
    const auto& qj=j.at("query");q.collisionTypeTag=qj.at("collision_tag");q.groupTypeTag=qj.at("group_tag");q.callbackEnabled=qj.at("callback");q.initialQueryCurrent=qj.at("initial_current_candidate");rawFloats(qj.at("copied40"),q.copied40);rawFloats(qj.at("copied50"),q.copied50);
    std::vector<std::string> names;for(const auto& c:j.at("candidates"))names.push_back(unhex(c.at("name_hex")));
    std::vector<Type3Candidate48P> pool(j.at("candidates").size());for(std::size_t i=0;i<pool.size();++i){auto& c=pool[i];const auto& cj=j.at("candidates")[i];c.collision.geometry.mesh=&meshes.at(cj.at("mesh"));c.collision.geometry.id=cj.at("id");floats(cj.at("world"),c.collision.geometry.worldMatrix.v);c.collision.sphere=sphere(cj.at("sphere"));c.collision.filter=filter(cj.at("filter"));c.collision.typeTag=cj.at("type_tag");rawFloats(cj.at("copied40"),c.collision.copied40);rawFloats(cj.at("copied50"),c.collision.copied50);c.shapeType=cj.at("shape_tag");c.hasMaterialScene=cj.at("has_scene");c.sceneName=names[i].data();c.sceneNameBytes=names[i].size();if(c.collision.filter.identity==d.state.spring.currentCandidate130)q.initialSpringCurrentCategory=c.collision.filter.categoryA4;}
    std::vector<Type3Candidate48P> ordered;for(const auto& n:j.at("order"))ordered.push_back(pool.at(n));
    std::array<SpringDetectorPoint48O,4> points{};std::array<SpringDetectorScene48O,4> scenes{};
    for(std::size_t i=0;i<4;++i){const auto& a=dj.at("point_records")[i];rawFloats(a.at("local"),points[i].local.v);points[i].sceneIndex=i;rawFloats(a.at("world"),scenes[i].world.v);rawFloats(a.at("scene_local"),scenes[i].local.v);scenes[i].flags=a.at("flags");scenes[i].parentPresent=a.at("parent").get<std::uint32_t>()!=0;}
    std::array<SpringVector4_48N,8> pairs{},staged{};std::array<std::uint32_t,8> metadata{};for(unsigned i=0;i<8;++i){raw4(dj.at("pairs"),4*i,pairs[i]);raw4(dj.at("staged"),4*i,staged[i]);metadata[i]=dj.at("metadata")[i];}
    DetectorArrays detectorScratch(4,8,0xA5),detectorPrepared(4,8,0xB6),detectorDestination(4,8);
    Rays sourceRays(slots,0xCD),scratchRays(slots,0xA5),preparedRays(slots,0xB6),destinationRays(slots);sourceRays.initialize(p,prep);
    std::vector<SpringRecord48N> records;for(const auto& r:j.at("initial_records"))records.push_back(record(r));Guarded<SpringRecord48N> scratchRecords(records.size(),2,0xA5),preparedRecords(records.size(),2,0xB6),destinationRecords(records.size());Guarded<TerrainCandidateEffects48J> scratchEffects(ordered.size(),2,0xA5),preparedEffects(ordered.size(),2,0xB6),destinationEffects(ordered.size());Guarded<std::uint32_t> stack(stackCapacity,0,0xA5);
    SpringDetectorPipelineSource48Q source{{points.data(),points.size(),scenes.data(),scenes.size(),pairs.data(),pairs.size(),staged.data(),staged.size(),metadata.data(),metadata.size()},sourceRays.source(),records.data(),records.size()};
    SpringDetectorPipelineWorkspace48Q workspace{detectorScratch.buffers(),detectorPrepared.buffers(),{{scratchRays.buffers(),scratchRecords.data(),scratchRecords.capacity,scratchEffects.data(),scratchEffects.capacity},stack.data(),stackCapacity},{preparedRays.buffers(),preparedRecords.data(),preparedRecords.capacity,preparedEffects.data(),preparedEffects.capacity}};
    SpringDetectorPipelineBuffers48Q destination{detectorDestination.buffers(),{destinationRays.buffers(),destinationRecords.data(),destinationRecords.capacity,destinationEffects.data(),destinationEffects.capacity}};SpringDetectorPipelineResult48Q out{};fillSentinel(&out,1,0x7B);
    std::vector<Snapshot> immutable,guards,transaction,effectTransaction;capture(immutable,&input,sizeof(input));captureFilter(immutable,q.filter);capture(immutable,&source,sizeof(source));capture(immutable,&workspace,sizeof(workspace));capture(immutable,&destination,sizeof(destination));captureVector(immutable,ordered);for(const auto& c:ordered)captureFilter(immutable,c.collision.filter);for(const auto& n:names)capture(immutable,n.data(),n.size());sourceRays.captureAll(immutable);capture(immutable,points.data(),sizeof(points));capture(immutable,scenes.data(),sizeof(scenes));capture(immutable,pairs.data(),sizeof(pairs));capture(immutable,staged.data(),sizeof(staged));capture(immutable,metadata.data(),sizeof(metadata));captureVector(immutable,records);
    detectorScratch.guards(guards);detectorPrepared.guards(guards);detectorDestination.guards(guards);scratchRays.guards(guards);preparedRays.guards(guards);destinationRays.guards(guards);scratchRecords.guards(guards);preparedRecords.guards(guards);destinationRecords.guards(guards);scratchEffects.guards(guards);preparedEffects.guards(guards);destinationEffects.guards(guards);stack.guards(guards);detectorDestination.captureAll(transaction);destinationRays.captureAll(transaction);captureVector(transaction,destinationRecords.v);captureVector(transaction,destinationEffects.v);capture(transaction,&out,sizeof(out));captureVector(effectTransaction,destinationEffects.v);
    Status wanted=Status::Complete;const auto boundary=e.at("boundary").is_null()?std::string():e.at("boundary").get<std::string>();if(boundary=="world-registration")wanted=Status::RegistrationBoundary;else if(boundary=="world-query")wanted=Status::WorldQueryBoundary;else if(!boundary.empty())wanted=Status::UnsupportedScope;else if(!d.springPresent)wanted=Status::NoSpring;else if(!d.queryPresent||!d.queryEnabled)wanted=Status::QueryDisabled;
    if(wanted==Status::Complete&&d.shapePairCount==0)wanted=Status::InvalidInput;
    watchAllocations=true;const auto status=runSpringDetectorPipeline48Q(input,source,ordered.data(),ordered.size(),workspace,destination,out);watchAllocations=false;
    check(status==wanted,"original detector flow status");if(status!=wanted&&failures<40)std::cerr<<label<<": status="<<static_cast<unsigned>(status)<<" expected="<<static_cast<unsigned>(wanted)<<'\n';
    if(wanted==Status::UnsupportedScope||wanted==Status::InvalidInput){for(const auto& s:transaction)s.verify();++unsupportedCases;}
    else if(status==wanted){
        compareDetector(out.detector,destination.detector,e.at("detector"));compareState(out.query.state,e.at("pair"),e.at("prepared"));compareRays(destinationRays,e.at("pair"),e.at("prepared"));checkState(out.query.spring,e.at("spring_state"));checkRecords(destinationRecords.data(),records.size(),e.at("records"));
        check(out.query.currentCandidate==e.at("current_candidate"),"final query candidate");check(out.query.queryFlags==e.at("query_flags"),"final query flags");const bool executed=!e.at("query_returned").is_null();const bool returned=!e.at("detector_returned").is_null();check(out.queryExecuted==executed,"query execution distinct from detector gate");check(out.detectorReturned==returned,"original detector returned versus named prefix boundary");check(out.detectorReturnValue==(returned?e.at("detector_returned").get<std::uint32_t>():0u),"detector always-return1 remains distinct from query hit");check(out.query.hasAnyHit==(executed&&e.at("query_returned").get<unsigned>()!=0),"separate query hit or miss");check(out.query.candidateCount==(executed?ordered.size():0),"executed ordered occurrence count");
        std::size_t geometryHits=0,appended=0,saturated=0;for(const auto& call:e.at("calls"))geometryHits+=call.at("returned").get<unsigned>()!=0;for(const auto& a:e.at("append_events")){const auto before=a.at("count_before").get<unsigned>(),after=a.at("count_after").get<unsigned>();appended+=after-before;saturated+=after==before;}
        check(out.query.geometryHits==geometryHits,"candidate geometry hit counter");check(out.query.callbacks==e.at("callback_calls").size(),"callback count including skipped callbacks");check(out.query.appended==appended,"record append counter");check(out.query.saturated==saturated,"per-hit logical saturation counter");
        if(executed){for(std::size_t i=0;i<ordered.size();++i){const auto& a=destinationEffects.data()[i];const auto& b=e.at("candidates").at(j.at("order")[i].get<std::size_t>());check(a.identity==b.at("identity")&&a.flagsF8==b.at("flags"),"candidate occurrence identity/flags");for(unsigned k=0;k<4;++k){check(bits(a.copied40[k])==b.at("copied40")[k].get<std::uint32_t>(),"candidate copied40 all lanes");check(bits(a.copied50[k])==b.at("copied50")[k].get<std::uint32_t>(),"candidate copied50 all lanes");}}
            compareRays(preparedRays,e.at("pair"),e.at("prepared"));checkRecords(preparedRecords.data(),records.size(),e.at("records"));
            for(const auto& stage:e.at("stages"))if(stage.at("stage")=="query_entry")compareDetector(out.detector,workspace.detectorPrepared,stage.at("detector"),false);
        }else for(const auto& s:effectTransaction)s.verify();
        for(const auto& read:e.at("body_scene_reads")){const BodyMatrix48G& bound=input.bodySceneIndex>=0?destination.detector.scenes[input.bodySceneIndex].world:q.bodyScene;for(unsigned k=0;k<16;++k)check(bits(bound.v[k])==read.at("world")[k].get<std::uint32_t>(),"callback body binding reads fresh detector world words");}
        for(unsigned k=0;k<4;++k){check(bits(q.copied40[k])==e.at("query_copied40")[k].get<std::uint32_t>(),"query copied40 preserved");check(bits(q.copied50[k])==e.at("query_copied50")[k].get<std::uint32_t>(),"query copied50 preserved");}
        totalHits+=out.query.hasAnyHit;totalGeometryHits+=out.query.geometryHits;totalCallbacks+=out.query.callbacks;totalAppended+=out.query.appended;totalSaturated+=out.query.saturated;
        ++differentialCases;noSpringCases+=status==Status::NoSpring;disabledCases+=status==Status::QueryDisabled;
    }
    for(const auto& s:immutable)s.verify();
    for(const auto& s:guards)s.verify();
    maxStack=std::max(maxStack,stackCapacity);maxRecords=std::max(maxRecords,records.size());maxCandidates=std::max(maxCandidates,ordered.size());
}
void wiring(){
    label="binding uses refreshed scene rather than stale independent cache";{
        Safety bound,fresh,stale;
        auto prepare=[](Safety& s){s.scenes[0].flags=0xABCD0080;s.scenes[0].local.v[12]=.5f;s.scenes[0].world.v[12]=0.f;};prepare(bound);prepare(fresh);prepare(stale);bound.input.bodySceneIndex=0;fresh.input.query.bodyScene=fresh.scenes[0].local;
        check(bound.call()==Status::Complete&&fresh.call()==Status::Complete&&stale.call()==Status::Complete,"all explicit body matrix variants complete");check(bound.out.query.appended>0,"binding test reaches callback record append");check(bound.destination.detector.scenes[0].flags==0xABCD0081,"dirty body cache preserves upper flags");check(std::memcmp(bound.destination.query.records,fresh.destination.query.records,4*sizeof(SpringRecord48N))==0,"bound callback agrees with independently cached newly refreshed matrix");check(std::memcmp(bound.destination.query.records,stale.destination.query.records,4*sizeof(SpringRecord48N))!=0,"bound callback distinguishes stale matrix");++wiringCases;
    }
    rejection("bound scene does not establish absent body presence",[](auto& s){s.input.bodySceneIndex=0;s.input.query.bodyScenePresent=false;});
    label="absent bound body is lazy on missed query";{Safety s;s.input.bodySceneIndex=0;s.input.query.bodyScenePresent=false;s.count=0;s.candidatePointer=nullptr;check(s.call()==Status::Complete,"empty supplied list never needs callback body");check(s.out.detectorReturnValue==1&&!s.out.query.hasAnyHit&&s.out.queryExecuted,"detector1 versus query miss");++wiringCases;}
    label="absent bound body is lazy on skipped callback";{Safety s;s.input.bodySceneIndex=0;s.input.query.bodyScenePresent=false;for(auto& c:s.candidates)c.collision.filter.categoryA4|=s.input.query.skipMask;check(s.call()==Status::Complete,"skipped callback never needs body");check(s.out.query.callbacks==2&&s.out.query.appended==0,"geometry callbacks skip before body access");++wiringCases;}
    label="no spring leaves all represented state intact";{Safety s;s.input.detector.springPresent=false;s.input.detector.state.spring.pointCount7C=0xffffffff;s.input.detector.providerPresent=s.input.detector.providerEnabled=true;s.input.bodySceneIndex=99;check(s.call()==Status::NoSpring,"no spring skips detector and query semantics");check(s.out.detectorReturned&&s.out.detectorReturnValue==1&&!s.out.queryExecuted,"no spring still original return1");check(std::memcmp(&s.out.detector,&s.input.detector.state,sizeof(s.out.detector))==0,"no spring exact detector state");check(std::memcmp(s.destination.detector.pairs,s.pairs.data(),sizeof(s.pairs))==0,"no spring exact pair backing");check(std::memcmp(s.destination.query.records,s.source.records,s.source.recordCount*sizeof(SpringRecord48N))==0,"no spring exact record backing");++wiringCases;}
    label="query disabled still updates numeric cache and resets";{Safety s;s.input.detector.queryEnabled=false;s.input.detector.shapePairCount=0xffffffff;s.scenes[0].flags=0;s.scenes[0].local.v[12]=.5f;check(s.call()==Status::QueryDisabled,"query gate ignores unusable shape count");check(s.out.detectorReturnValue==1&&!s.out.queryExecuted&&s.out.detector.spring.pending38==1&&s.out.detector.spring.generatedCount84==0,"reset before query gate");for(unsigned i=0;i<4;++i)check(std::memcmp(&s.destination.detector.pairs[2*i],&s.pairs[2*i],sizeof(SpringVector4_48N))==0,"disabled query preserves old first endpoints");check(s.destination.detector.scenes[0].flags==1&&s.destination.detector.scenes[0].world.v[12]==.5f,"disabled query still refreshes numeric cache");++wiringCases;}
    for(auto pointCount:{0u,0xffffffffu,0x80000000u}){label="no-pair signed inactive count registration prefix";Safety s;s.input.detector.pairsPresent=false;s.input.detector.state.spring.pointCount7C=pointCount;s.input.detector.providerPresent=s.input.detector.providerEnabled=true;check(s.call()==Status::RegistrationBoundary,"unreached provider retains registration boundary");check(!s.out.detectorReturned&&!s.out.queryExecuted&&s.out.detectorReturnValue==0,"registration prefix does not claim detector completion");++wiringCases;}
    label="world query is explicit committed prefix";{Safety s;s.input.detector.explicitList=false;check(s.call()==Status::WorldQueryBoundary,"world discovery unsupported named prefix");check(!s.out.detectorReturned&&!s.out.queryExecuted,"world prefix is not completed detector");for(unsigned i=0;i<4;++i)check(s.destination.detector.stagedMetadata[i]==0,"world prefix retains completed zero-metadata staging");++wiringCases;}
    label="query reset is not applied twice";{Safety s;s.input.detector.resetSpring=false;s.input.detector.state.spring.generatedCount84=1;s.input.query.resetSpring=true;check(s.call()==Status::Complete,"nonreset detector query completes");check(s.out.query.spring.generatedCount84==3,"append starts at supplied count rather than resetting twice");check(std::memcmp(s.destination.query.records,s.source.records,sizeof(SpringRecord48N))==0,"preexisting first record unchanged");++wiringCases;}
    label="exact declared capacities";{Safety s;for(unsigned n=0;n<3;++n){auto& d=s.detector(n);d.sceneCapacity=4;d.pairVectorCapacity=d.stagedVectorCapacity=d.stagedMetadataCapacity=8;auto& q=s.query(n);q.rays.capacity=q.rays.lengthCapacity=4;q.recordCapacity=4;q.effectCapacity=2;}s.workspace.queryScratch.treeStackCapacity=2;check(s.call()==Status::Complete,"exact storage works");check(s.out.query.appended==2,"exact storage preserves callbacks");++wiringCases;}
    label="logical record saturation";{Safety s;s.input.detector.state.spring.generatedCapacity88=0;check(s.call()==Status::Complete,"zero logical capacity succeeds");check(s.out.query.callbacks==2&&s.out.query.saturated==2&&s.out.query.appended==0,"saturation is per callback hit");check(std::memcmp(s.destination.query.records,s.source.records,4*sizeof(SpringRecord48N))==0,"saturation preserves every record byte");++wiringCases;}
    label="empty list retains initialized current candidate";{Safety s;s.count=0;s.candidatePointer=nullptr;s.input.query.initialQueryCurrent=0x12345678;check(s.call()==Status::Complete,"empty supplied list is complete");check(s.out.detectorReturnValue==1&&!s.out.query.hasAnyHit&&s.out.query.currentCandidate==0x12345678,"no hit keeps detector1 and original query current");++wiringCases;}
    label="read-only detector and ray inputs may overlap";{Safety s;s.source.rays.worldStarts=s.pairs.data();check(s.call()==Status::Complete,"read/read overlap is accepted");++wiringCases;}
}
void benchmark(){
    label="serial bounded composed cost";Safety s;constexpr std::size_t loops=4000;std::array<double,7> samples{};std::size_t complete=0,appended=0;const auto before=observedAllocations;
    for(auto& ns:samples){auto start=std::chrono::steady_clock::now();watchAllocations=true;for(std::size_t i=0;i<loops;++i){complete+=runSpringDetectorPipeline48Q(s.input,s.source,s.candidatePointer,s.count,s.workspace,s.destination,s.out)==Status::Complete;appended+=s.out.query.appended;}watchAllocations=false;ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/loops;}
    std::sort(samples.begin(),samples.end());check(complete==loops*samples.size()&&appended==2*complete,"serial reuse stable complete appends");check(observedAllocations==before,"serial reuse allocates nothing");
    const auto scratch=detectorBytes(s.workspace.detectorScratch)+detectorBytes(s.workspace.detectorPrepared)+queryBytes(s.workspace.queryScratch.buffers)+queryBytes(s.workspace.queryPrepared)+s.workspace.queryScratch.treeStackCapacity*sizeof(std::uint32_t);
    std::cout<<"spring_detector_pipeline_cost_host_ns median="<<samples[3]<<" max_batch_mean="<<samples.back()<<" serial_calls="<<complete<<" point_count="<<s.input.detector.state.spring.pointCount7C<<" shape_count="<<s.input.detector.shapePairCount<<" candidates="<<s.count<<" detector_scratch_bytes="<<detectorBytes(s.workspace.detectorScratch)<<" detector_prepared_bytes="<<detectorBytes(s.workspace.detectorPrepared)<<" query_scratch_bytes="<<queryBytes(s.workspace.queryScratch.buffers)<<" query_prepared_bytes="<<queryBytes(s.workspace.queryPrepared)<<" tree_stack_bytes="<<s.workspace.queryScratch.treeStackCapacity*sizeof(std::uint32_t)<<" total_workspace_backing_bytes="<<scratch<<" final_destination_bytes="<<detectorBytes(s.destination.detector)+queryBytes(s.destination.query)<<" result_bytes="<<sizeof(SpringDetectorPipelineResult48Q)<<" workspace_descriptor_bytes="<<sizeof(SpringDetectorPipelineWorkspace48Q)<<" source_descriptor_bytes="<<sizeof(SpringDetectorPipelineSource48Q)<<" input_bytes="<<sizeof(SpringDetectorPipelineInput48Q)<<" runtime_allocations="<<(observedAllocations-before)<<" (serial host observation, not game/Xbox FPS)\n";
}
}
int main(int argc,char** argv){try{
    if(argc!=2)return 2;
    std::fesetround(FE_TONEAREST);std::ifstream f(std::filesystem::path(argv[1])/"evidence/springDetectorPipeline48Q/native-fixtures.json");if(!f)throw std::runtime_error("missing springDetectorPipeline48Q/native-fixtures.json");Json fixture;f>>fixture;if(fixture.at("schema")!=1||fixture.at("array_slots")!=4||fixture.at("cases").size()!=170||fixture.at("meshes").size()!=11)throw std::runtime_error("incomplete detector pipeline fixture");std::vector<CollisionMesh48I> meshes;for(const auto& m:fixture.at("meshes"))meshes.push_back(mesh(m));std::size_t stack=0;for(const auto& m:meshes)stack=std::max(stack,stackBound(m));std::vector<Snapshot> snapshots;for(const auto& m:meshes)captureMesh(snapshots,m);for(const auto& j:fixture.at("cases"))differential(j,meshes,stack);for(const auto& s:snapshots)s.verify();check(differentialCases+unsupportedCases==fixture.at("cases").size(),"every original fixture executed and classified");check(differentialCases==159&&unsupportedCases==11&&noSpringCases==8&&disabledCases==11,"frozen supported/rejected and gate counts");safety();wiring();benchmark();check(observedAllocations==0,"all guarded runtime calls allocation-free");
    std::cout<<"spring_detector_pipeline48Q committed_cases="<<differentialCases<<" no_spring="<<noSpringCases<<" query_disabled="<<disabledCases<<" original_only_rejections="<<unsupportedCases<<" checks="<<checks<<" failures="<<failures<<" queries_with_geometry_hits="<<totalHits<<" candidate_geometry_hits="<<totalGeometryHits<<" callbacks="<<totalCallbacks<<" appended="<<totalAppended<<" saturated="<<totalSaturated<<" safety_cases="<<safetyCases<<" alias_cases="<<aliasCases<<" wiring_cases="<<wiringCases<<" fixture_max_tree_stack_entries="<<maxStack<<" fixture_max_records="<<maxRecords<<" fixture_max_candidates="<<maxCandidates<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;
}catch(const std::exception& e){watchAllocations=false;std::cerr<<label<<": "<<e.what()<<'\n';return 2;}}
