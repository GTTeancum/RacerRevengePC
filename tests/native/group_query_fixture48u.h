#pragma once
// Additive48U test-only helpers copied from the retained48P loader. Original
// fixtures remain immutable. Guest addresses are opaque IDs, never dereferenced.
#include "group_query48u.h"
#include "json.hpp"
#include <algorithm>
#include <array>
#include <cfenv>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#if defined(_MSC_VER)
#include <malloc.h>
#define RR48U_NOINLINE __declspec(noinline)
#else
#define RR48U_NOINLINE __attribute__((noinline))
#endif
namespace group_test48u {
using namespace rr::game;
using namespace rr::assets;
using Json=nlohmann::json;
inline bool watching=false;
inline std::size_t allocationKinds[4]{};
inline std::size_t allocations(){std::size_t n=0;for(auto x:allocationKinds)n+=x;return n;}
inline void* allocate(std::size_t n,unsigned kind,std::size_t alignment=0){
 if(watching)++allocationKinds[kind];
 void* p=nullptr;
 if(alignment){
#if defined(_MSC_VER)
 p=_aligned_malloc(n?n:1,alignment);
#else
 if(posix_memalign(&p,alignment,n?n:1))p=nullptr;
#endif
 }else p=std::malloc(n?n:1);
 if(p)return p;
 throw std::bad_alloc();
}
inline void releaseAligned(void* p){
#if defined(_MSC_VER)
 _aligned_free(p);
#else
 std::free(p);
#endif
}
template<class T>void fillSentinel(T* storage,std::size_t count,unsigned char value){
    static_assert(std::is_trivially_copyable_v<T>,"byte sentinels require trivially copyable storage");
    auto* bytes=reinterpret_cast<unsigned char*>(storage);
    std::fill_n(bytes,count*sizeof(T),value);
}
inline std::size_t checks=0,failures=0,differentialCases=0,unsupportedCases=0,safetyCases=0,aliasCases=0;
inline std::size_t totalHits=0,totalGeometryHits=0,totalCallbacks=0,totalAppended=0,totalSaturated=0;
inline std::size_t maxStack=0,maxRecords=0,maxCandidates=0;
inline std::string label;
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
void captureFilter(std::vector<Snapshot>& saved,const CollisionFilter48J& f){capture(saved,&f,sizeof(f));captureVector(saved,f.categoryMasks);for(const auto& l:f.identityLists)captureVector(saved,l.identities);}
void captureMesh(std::vector<Snapshot>& saved,const CollisionMesh48I& m){capture(saved,&m,sizeof(m));captureVector(saved,m.vertices);captureVector(saved,m.vertex_normals);captureVector(saved,m.nodes);captureVector(saved,m.faces);}
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
void* rayPointer(Type3RayBuffers48P& r,unsigned i){switch(i){case 0:return r.localStarts;case 1:return r.localDirections;case 2:return r.worldStarts;case 3:return r.worldDirections;case 4:return r.normals;case 5:return r.points;case 6:return r.hits;case 7:return r.fractions;default:return r.lengths;}}
void setRayPointer(Type3RayBuffers48P& r,unsigned i,void* p){switch(i){case 0:r.localStarts=static_cast<Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<SpringVector4_48N*>(p);break;case 5:r.points=static_cast<SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<std::uint32_t*>(p);break;case 7:r.fractions=static_cast<float*>(p);break;default:r.lengths=static_cast<float*>(p);break;}}
void setSourceRay(Type3RaySource48P& r,unsigned i,const void* p){switch(i){case 0:r.localStarts=static_cast<const Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<const Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<const SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<const SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<const SpringVector4_48N*>(p);break;case 5:r.points=static_cast<const SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<const std::uint32_t*>(p);break;case 7:r.fractions=static_cast<const float*>(p);break;default:r.lengths=static_cast<const float*>(p);break;}}

inline void shapeMatrix(BodyMatrix48G& m,const Json& words,std::size_t first){for(unsigned k=0;k<16;++k)m.v[k]=fp(words.at(first+k).get<std::uint32_t>());}
inline GroupShapeState48T shapeState(const Json& words){GroupShapeState48T s{};shapeMatrix(s.composed00,words,0);shapeMatrix(s.world80,words,32);shapeMatrix(s.previousC0,words,48);return s;}
inline void shapeProject(std::vector<std::uint32_t>& words,const GroupShapeState48T& s){for(unsigned k=0;k<16;++k){words[k]=bits(s.composed00.v[k]);words[32+k]=bits(s.world80.v[k]);words[48+k]=bits(s.previousC0.v[k]);}}
struct Slot {
 std::string name;void* pointer;std::size_t count,size,alignment;
 std::function<void(void*,std::size_t)> set;
};
template<class T>void writable(std::vector<Slot>& out,const std::string& name,T*& p,std::size_t& count){out.push_back({name,p,count,sizeof(T),alignof(T),[&p,&count](void* a,std::size_t n){p=static_cast<T*>(a);count=n;}});}
template<class T>void readable(std::vector<Slot>& out,const std::string& name,const T*& p,std::size_t& count){out.push_back({name,const_cast<T*>(p),count,sizeof(T),alignof(T),[&p,&count](void* a,std::size_t n){p=static_cast<const T*>(a);count=n;}});}
template<class T>void fixedRead(std::vector<Slot>& out,const std::string& name,const T* p,std::size_t count=1){out.push_back({name,const_cast<T*>(p),count,sizeof(T),alignof(T),{}});}
inline Json accounting(){return {{"checks",checks},{"failures",failures},{"differential_cases",differentialCases},{"source_boundary_cases",unsupportedCases},{"native_safety_cases",safetyCases},{"declared_span_overlap_cases",aliasCases},{"observed_allocations",allocations()},{"ordinary_new",allocationKinds[0]},{"array_new",allocationKinds[1]},{"aligned_new",allocationKinds[2]},{"aligned_array_new",allocationKinds[3]},{"queries_with_geometry_hits",totalHits},{"candidate_geometry_hits",totalGeometryHits},{"callbacks",totalCallbacks},{"appended",totalAppended},{"saturated",totalSaturated}};}

}
RR48U_NOINLINE void* operator new(std::size_t n){return group_test48u::allocate(n,0);}
RR48U_NOINLINE void* operator new[](std::size_t n){return group_test48u::allocate(n,1);}
RR48U_NOINLINE void* operator new(std::size_t n,std::align_val_t a){return group_test48u::allocate(n,2,static_cast<std::size_t>(a));}
RR48U_NOINLINE void* operator new[](std::size_t n,std::align_val_t a){return group_test48u::allocate(n,3,static_cast<std::size_t>(a));}
RR48U_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
RR48U_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
RR48U_NOINLINE void operator delete(void* p,std::size_t)noexcept{std::free(p);}
RR48U_NOINLINE void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
RR48U_NOINLINE void operator delete(void* p,std::align_val_t)noexcept{group_test48u::releaseAligned(p);}
RR48U_NOINLINE void operator delete[](void* p,std::align_val_t)noexcept{group_test48u::releaseAligned(p);}
RR48U_NOINLINE void operator delete(void* p,std::size_t,std::align_val_t)noexcept{group_test48u::releaseAligned(p);}
RR48U_NOINLINE void operator delete[](void* p,std::size_t,std::align_val_t)noexcept{group_test48u::releaseAligned(p);}
