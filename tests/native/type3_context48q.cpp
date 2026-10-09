// Additive48Q borrowed-context checks. Fixture/parsing/comparison setup is copied
// from retained type3_query48p.cpp; the retained48P file is not modified.
#include "type3_query48p.h"
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
using Status=Type3QueryStatus48P;
template<class T>void fillSentinel(T* storage,std::size_t count,unsigned char value){
    static_assert(std::is_trivially_copyable_v<T>,"byte sentinels require trivially copyable storage");
    auto* bytes=reinterpret_cast<unsigned char*>(storage);
    std::fill_n(bytes,count*sizeof(T),value);
}
std::size_t checks=0,failures=0,differentialCases=0,directCases=0,unsupportedCases=0,safetyCases=0,aliasCases=0,contextAliasCases=0;
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
Type3QueryContext48P contextFrom(const Type3QueryInput48P& q){
    return {q.spring,q.bodyScene,q.pairCount,q.resetSpring,q.bodyScenePresent,q.entry};
}
// Poison every shadowed execution field after capturing a correct context.
// Any accidental read from the template either rejects or changes raw output.
// This runs only in setup, never inside allocation instrumentation.
void poisonTemplate(Type3QueryInput48P& q,const Type3QueryContext48P& context){
    q.spring={0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,
        0xDEADBEEF,0xDEADBEEF,0xDEADBEEF,-1,-1,0xDEADBEEF};
    for(auto& word:q.bodyScene.v)word=std::numeric_limits<float>::quiet_NaN();
    q.pairCount=0;q.resetSpring=!context.resetSpring;
    q.bodyScenePresent=!context.bodyScenePresent;
    q.entry=static_cast<Type3QueryEntry48P>(99);
}
void differential(const Json& j,const std::vector<CollisionMesh48I>& meshes,std::size_t stackCapacity){
    label=j.at("name").get<std::string>();const auto& e=j.at("expected");const bool direct=j.at("entry")=="direct";
    const auto& initial=direct?(e.contains("dispatch_initial")?e.at("dispatch_initial"):e.at("after_prepare")):e.at("initial");const auto& p=initial.at("pair");const auto& prep=initial.at("prepared");const std::size_t slots=p.at("hit_flags").size();
    Type3QueryInput48P q{};q.initial=queryState(p,prep);q.spring=state(j.at("initial_state"));q.filter=filter(j.at("query").at("filter"));q.pairCount=static_cast<std::uint32_t>(j.at("pairs").size());q.normalMode=j.at("mode");q.entry=direct?Type3QueryEntry48P::Dispatcher:Type3QueryEntry48P::LimitedList;q.sentinel={{0,0,0,1}};q.resetSpring=j.at("reset_before");q.skipMask=j.at("masks")[0];q.dynamicMask=j.at("masks")[1];q.materialMask=j.at("masks")[2];floats(j.at("body_world"),q.bodyScene.v);
    const auto& qj=j.at("query");q.collisionTypeTag=qj.at("collision_tag");q.groupTypeTag=qj.at("group_tag");q.callbackEnabled=qj.at("callback");q.initialQueryCurrent=qj.at("initial_current_candidate");rawFloats(qj.at("copied40"),q.copied40);rawFloats(qj.at("copied50"),q.copied50);
    std::vector<std::string> names;for(const auto& c:j.at("candidates"))names.push_back(unhex(c.at("name_hex")));
    std::vector<Type3Candidate48P> pool(j.at("candidates").size());for(std::size_t i=0;i<pool.size();++i){auto& c=pool[i];const auto& cj=j.at("candidates")[i];c.collision.geometry.mesh=&meshes.at(cj.at("mesh"));c.collision.geometry.id=cj.at("id");floats(cj.at("world"),c.collision.geometry.worldMatrix.v);c.collision.sphere=sphere(cj.at("sphere"));c.collision.filter=filter(cj.at("filter"));c.collision.typeTag=cj.at("type_tag");rawFloats(cj.at("copied40"),c.collision.copied40);rawFloats(cj.at("copied50"),c.collision.copied50);c.shapeType=cj.at("shape_tag");c.hasMaterialScene=cj.at("has_scene");c.sceneName=names[i].data();c.sceneNameBytes=names[i].size();if(c.collision.filter.identity==q.spring.currentCandidate130)q.initialSpringCurrentCategory=c.collision.filter.categoryA4;}
    std::vector<Type3Candidate48P> ordered;for(const auto& n:j.at("order"))ordered.push_back(pool.at(n));
    Rays sourceRays(slots,0xCD),scratchRays(slots,0xA5),destinationRays(slots);sourceRays.initialize(p,prep);
    std::vector<SpringVector4_48N> endpoints(prep.at("endpoints").size()/4);for(std::size_t i=0;i<endpoints.size();++i)raw4(prep.at("endpoints"),4*i,endpoints[i]);auto metadata=prep.at("scene_ids").get<std::vector<std::uint32_t>>();
    std::vector<SpringRecord48N> records;for(const auto& r:j.at("initial_records"))records.push_back(record(r));Guarded<SpringRecord48N> scratchRecords(records.size(),2,0xA5),destinationRecords(records.size());Guarded<TerrainCandidateEffects48J> scratchEffects(ordered.size(),2,0xA5),destinationEffects(ordered.size());Guarded<std::uint32_t> stack(stackCapacity,0,0xA5);
    Type3QuerySource48P source{sourceRays.source(),endpoints.data(),endpoints.size(),metadata.data(),metadata.size(),records.data(),records.size()};
    Type3QueryWorkspace48P workspace{{scratchRays.buffers(),scratchRecords.data(),scratchRecords.capacity,scratchEffects.data(),scratchEffects.capacity},stack.data(),stackCapacity};Type3QueryBuffers48P destination{destinationRays.buffers(),destinationRecords.data(),destinationRecords.capacity,destinationEffects.data(),destinationEffects.capacity};Type3QueryResult48P out{};out.state.hitCount=0xBAADF00D;out.currentCandidate=0xCAFEBABE;
    const auto context=contextFrom(q);poisonTemplate(q,context);
    std::vector<Snapshot> immutable,guards,transaction;capture(immutable,&context,sizeof(context));capture(immutable,&q,sizeof(q));captureFilter(immutable,q.filter);capture(immutable,&source,sizeof(source));capture(immutable,&workspace,sizeof(workspace));capture(immutable,&destination,sizeof(destination));captureVector(immutable,ordered);for(const auto& c:ordered)captureFilter(immutable,c.collision.filter);for(const auto& n:names)capture(immutable,n.data(),n.size());sourceRays.captureAll(immutable);captureVector(immutable,endpoints);captureVector(immutable,metadata);captureVector(immutable,records);
    scratchRays.guards(guards);destinationRays.guards(guards);scratchRecords.guards(guards);destinationRecords.guards(guards);scratchEffects.guards(guards);destinationEffects.guards(guards);stack.guards(guards);destinationRays.captureAll(transaction);captureVector(transaction,destinationRecords.v);captureVector(transaction,destinationEffects.v);capture(transaction,&out,sizeof(out));
    bool unsupported=false;for(std::size_t i=0;i<2*context.pairCount;++i)unsupported|=metadata[i]!=0;unsupported|=e.at("requires_mode2").get<bool>();for(const auto& call:e.at("calls")){const auto id=call.at("identity").get<std::uint32_t>();for(const auto& c:ordered)if(c.collision.filter.identity==id&&c.shapeType==1)unsupported=true;}
    watchAllocations=true;const auto status=queryStaticType3WithContextBounded48P(q,context,source,ordered.data(),ordered.size(),workspace,destination,out);watchAllocations=false;
    if(unsupported){check(status==Status::UnsupportedScope,"original-only scope rejected explicitly");for(const auto& s:transaction)s.verify();++unsupportedCases;}
    else{check(status==Status::Complete,"original query completes");if(status!=Status::Complete)std::cerr<<label<<": status="<<static_cast<unsigned>(status)<<'\n';else{
        compareState(out.state,e.at("pair"),e.at("prepared"));compareRays(destinationRays,e.at("pair"),e.at("prepared"));checkState(out.spring,e.at("spring_state"));checkRecords(destinationRecords.data(),records.size(),e.at("records"));check(out.currentCandidate==e.at("current_candidate"),"final query candidate");check(out.queryFlags==e.at("query_flags"),"final query flags");check(out.hasAnyHit==(e.at("returned").get<unsigned>()!=0),"query hit/miss");check(out.candidateCount==ordered.size(),"ordered occurrence count");
        std::size_t geometryHits=0,appended=0,saturated=0;for(const auto& call:e.at("calls"))geometryHits+=call.at("returned").get<unsigned>()!=0;for(const auto& a:e.at("append_events")){const auto before=a.at("count_before").get<unsigned>(),after=a.at("count_after").get<unsigned>();appended+=after-before;saturated+=after==before;}
        check(out.geometryHits==geometryHits,"candidate geometry hit counter");check(out.callbacks==e.at("callback_calls").size(),"callback count including skips");check(out.appended==appended,"record append counter");check(out.saturated==saturated,"per-hit logical saturation counter");
        for(std::size_t i=0;i<ordered.size();++i){const auto& a=destinationEffects.data()[i];const auto& b=e.at("candidates").at(j.at("order")[i].get<std::size_t>());check(a.identity==b.at("identity")&&a.flagsF8==b.at("flags"),"candidate occurrence identity/flags");for(unsigned k=0;k<4;++k){check(bits(a.copied40[k])==b.at("copied40")[k].get<std::uint32_t>(),"candidate copied40 full4");check(bits(a.copied50[k])==b.at("copied50")[k].get<std::uint32_t>(),"candidate copied50 full4");}}
        for(unsigned k=0;k<4;++k){check(bits(q.copied40[k])==e.at("query_copied40")[k].get<std::uint32_t>(),"query copied40 remains unchanged");check(bits(q.copied50[k])==e.at("query_copied50")[k].get<std::uint32_t>(),"query copied50 remains unchanged");}
        if(j.at("reset_after").get<bool>()){watchAllocations=true;resetSpringRecords48N(out.spring);watchAllocations=false;checkState(out.spring,e.at("after_reset").at("spring_state"));checkRecords(destinationRecords.data(),records.size(),e.at("after_reset").at("records"));}
        totalHits+=out.hasAnyHit;totalGeometryHits+=out.geometryHits;totalCallbacks+=out.callbacks;totalAppended+=out.appended;totalSaturated+=out.saturated;
    }if(direct)++directCases;else ++differentialCases;}
    for(const auto& s:immutable)s.verify();
    for(const auto& s:guards)s.verify();
    maxStack=std::max(maxStack,stackCapacity);maxRecords=std::max(maxRecords,records.size());maxCandidates=std::max(maxCandidates,ordered.size());
}
void* rayPointer(Type3RayBuffers48P& r,unsigned i){switch(i){case 0:return r.localStarts;case 1:return r.localDirections;case 2:return r.worldStarts;case 3:return r.worldDirections;case 4:return r.normals;case 5:return r.points;case 6:return r.hits;case 7:return r.fractions;default:return r.lengths;}}
void setRayPointer(Type3RayBuffers48P& r,unsigned i,void* p){switch(i){case 0:r.localStarts=static_cast<Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<SpringVector4_48N*>(p);break;case 5:r.points=static_cast<SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<std::uint32_t*>(p);break;case 7:r.fractions=static_cast<float*>(p);break;default:r.lengths=static_cast<float*>(p);break;}}
void setSourceRay(Type3RaySource48P& r,unsigned i,const void* p){switch(i){case 0:r.localStarts=static_cast<const Vector3_48C*>(p);break;case 1:r.localDirections=static_cast<const Vector3_48C*>(p);break;case 2:r.worldStarts=static_cast<const SpringVector4_48N*>(p);break;case 3:r.worldDirections=static_cast<const SpringVector4_48N*>(p);break;case 4:r.normals=static_cast<const SpringVector4_48N*>(p);break;case 5:r.points=static_cast<const SpringVector4_48N*>(p);break;case 6:r.hits=static_cast<const std::uint32_t*>(p);break;case 7:r.fractions=static_cast<const float*>(p);break;default:r.lengths=static_cast<const float*>(p);break;}}
struct Safety {
    Type3QueryInput48P q{};alignas(std::max_align_t) Type3QueryContext48P context{};CollisionMesh48I m{};
    std::array<Type3Candidate48P,2> candidates{};
    alignas(std::max_align_t) char names[2][64]{};
    Rays inputRays{4,0xCD},scratchRays{4,0xA5},destinationRays{4};
    std::array<SpringVector4_48N,8> endpoints{};std::array<std::uint32_t,8> metadata{};
    Guarded<SpringRecord48N> records{4,2,0xCD},scratchRecords{4,2,0xA5},destinationRecords{4};
    Guarded<TerrainCandidateEffects48J> scratchEffects{2,2,0xA5},destinationEffects{2};Guarded<std::uint32_t> stack{16,0,0xA5};
    Type3QuerySource48P source{};Type3QueryWorkspace48P workspace{};Type3QueryBuffers48P destination{};Type3QueryResult48P out{};
    const Type3Candidate48P* candidatePointer=candidates.data();Type3QueryResult48P* outputPointer=&out;std::size_t count=2;
    Safety(){
        q.pairCount=1;q.sentinel={{0,0,0,1}};q.skipMask=8;q.dynamicMask=16;q.materialMask=4;q.collisionTypeTag=7;q.groupTypeTag=9;q.spring={11,12,13,14,15,16,0,1,0,3,21};q.initial.opaque78=0x13572468;q.initial.opaque7c=0x24681357;q.initial.selectedFraction=.125f;
        for(unsigned i=0;i<16;++i)q.bodyScene.v[i]=(i%5)==0?1.f:0.f;
        q.filter.identity=100;q.filter.categoryA4=4;q.filter.flagsF8=4;q.filter.flags31=1;q.filter.categoryMasks={0};q.filter.identityLists[0].identities={900};q.filter.identityLists[1].identities={901};q.copied40={{1,2,3,4}};q.copied50={{5,6,7,8}};
        endpoints[0]={{.25f,1.f,.125f,fp(0x80000000)}};endpoints[1]={{.25f,-1.f,.125f,fp(11)}};metadata[7]=0x12345678;
        for(std::size_t i=0;i<4;++i){inputRays.lengths.data()[i]=2.f;inputRays.hits.data()[i]=0;inputRays.fractions.data()[i]=.75f;}
        m.vertices={{{-4,0,-4}},{{0,0,4}},{{4,0,-4}},{{16,0,16}},{{20,0,24}},{{24,0,16}}};m.vertex_normals.assign(6,{{0,1,0}});m.nodes.push_back({100,{{10,0,10}},{{20,1,20}},{{collision_mesh_leaf48i,collision_mesh_leaf48i|1}}});m.faces.push_back({0,{{0,32767,0}},{{0,1,2}}});m.faces.push_back({0,{{0,32767,0}},{{3,4,5}}});
        for(unsigned i=0;i<2;++i){std::strcpy(names[i],"trackfr7.col");auto& c=candidates[i];c.collision.geometry={&m,q.bodyScene,10+i};c.collision.filter=q.filter;c.collision.filter.identity=10+i;c.collision.sphere={{{0,0,0}},100};c.collision.typeTag=7;c.collision.copied40={{11,12,13,14}};c.collision.copied50={{15,16,17,18}};c.hasMaterialScene=true;c.sceneName=names[i];c.sceneNameBytes=sizeof(names[i]);}
        source={inputRays.source(),endpoints.data(),endpoints.size(),metadata.data(),metadata.size(),records.data(),records.used};workspace={{scratchRays.buffers(),scratchRecords.data(),scratchRecords.capacity,scratchEffects.data(),scratchEffects.capacity},stack.data(),stack.capacity};destination={destinationRays.buffers(),destinationRecords.data(),destinationRecords.capacity,destinationEffects.data(),destinationEffects.capacity};out.currentCandidate=77;out.state.hitCount=91;out.queryFlags=0xCB;
        context=contextFrom(q);poisonTemplate(q,context);
    }
    std::vector<Snapshot> borrowed()const{
        std::vector<Snapshot> saved;capture(saved,&q,sizeof(q));capture(saved,&context,sizeof(context));captureFilter(saved,q.filter);
        for(const auto& c:candidates)captureFilter(saved,c.collision.filter);
        return saved;
    }
    Status call(){
        const auto saved=borrowed();
        watchAllocations=true;auto s=queryStaticType3WithContextBounded48P(q,context,source,candidatePointer,count,workspace,destination,*outputPointer);watchAllocations=false;
        for(const auto& snapshot:saved)snapshot.verify();
        return s;
    }
    void* write(unsigned i){if(i<9)return rayPointer(workspace.buffers.rays,i);if(i<18)return rayPointer(destination.rays,i-9);switch(i){case 18:return workspace.buffers.records;case 19:return destination.records;case 20:return workspace.buffers.effects;case 21:return destination.effects;case 22:return workspace.treeStack;default:return outputPointer;}}
    void setWrite(unsigned i,void* p){if(i<9){setRayPointer(workspace.buffers.rays,i,p);return;}if(i<18){setRayPointer(destination.rays,i-9,p);return;}switch(i){case 18:workspace.buffers.records=static_cast<SpringRecord48N*>(p);break;case 19:destination.records=static_cast<SpringRecord48N*>(p);break;case 20:workspace.buffers.effects=static_cast<TerrainCandidateEffects48J*>(p);break;case 21:destination.effects=static_cast<TerrainCandidateEffects48J*>(p);break;case 22:workspace.treeStack=static_cast<std::uint32_t*>(p);break;default:outputPointer=static_cast<Type3QueryResult48P*>(p);break;}}
    std::vector<void*> reads(){
        std::vector<void*> r{&context,&q,&source,&workspace,&destination,endpoints.data(),metadata.data(),records.data(),candidates.data(),&m,m.vertices.data(),m.vertex_normals.data(),m.nodes.data(),m.faces.data(),names[0],names[1]};
        auto filterReads=[&](CollisionFilter48J& f){r.push_back(&f);r.push_back(f.categoryMasks.data());for(auto& l:f.identityLists)r.push_back(l.identities.data());};filterReads(q.filter);for(auto& c:candidates)filterReads(c.collision.filter);
        auto ray=inputRays.buffers();for(unsigned i=0;i<9;++i)r.push_back(rayPointer(ray,i));return r;
    }
    std::vector<Snapshot> saved(){std::vector<Snapshot> s;capture(s,&context,sizeof(context));capture(s,&q,sizeof(q));captureFilter(s,q.filter);capture(s,&source,sizeof(source));capture(s,&workspace,sizeof(workspace));capture(s,&destination,sizeof(destination));capture(s,candidates.data(),sizeof(candidates));for(const auto& c:candidates)captureFilter(s,c.collision.filter);captureMesh(s,m);capture(s,names,sizeof(names));capture(s,endpoints.data(),sizeof(endpoints));capture(s,metadata.data(),sizeof(metadata));captureVector(s,records.v);inputRays.captureAll(s);destinationRays.captureAll(s);captureVector(s,destinationRecords.v);captureVector(s,destinationEffects.v);capture(s,&out,sizeof(out));return s;}
    void reject(Status wanted=Status::InvalidInput,bool alias=false){auto s=saved();auto status=call();check(status==wanted,"expected transactional rejection");if(status!=wanted&&failures<40)std::cerr<<label<<": status="<<static_cast<unsigned>(status)<<" expected="<<static_cast<unsigned>(wanted)<<'\n';for(const auto& p:s)p.verify();++safetyCases;aliasCases+=alias;}
};
template<class F>void rejection(const char* name,F change,Status status=Status::InvalidInput){label=name;Safety s;change(s);s.reject(status);}
void safety(){
    label="baseline safety fixture";{Safety s;check(s.call()==Status::Complete,"baseline complete");check(s.out.hasAnyHit&&s.out.geometryHits==2&&s.out.callbacks==2&&s.out.appended==2,"baseline two candidate hits/callbacks");check(s.out.state.selectedNormal.v[3]==2.f&&s.out.state.selectedCorrection.v[3]==3.f,"original transformed W lanes");}
    for(unsigned w=0;w<23;++w){rejection("null declared writable span",[&](auto& s){s.setWrite(w,nullptr);});rejection("misaligned declared writable span",[&](auto& s){s.setWrite(w,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(s.write(w))+1));});}
    for(unsigned r=0;r<9;++r){rejection("null declared source ray",[&](auto& s){setSourceRay(s.source.rays,r,nullptr);});rejection("misaligned declared source ray",[&](auto& s){auto b=s.inputRays.buffers();setSourceRay(s.source.rays,r,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(rayPointer(b,r))+1));});}
    for(unsigned i=0;i<4;++i){rejection("null declared input",[&](auto& s){switch(i){case 0:s.source.endpoints=nullptr;break;case 1:s.source.sceneMetadata=nullptr;break;case 2:s.source.records=nullptr;break;default:s.candidatePointer=nullptr;break;}});rejection("misaligned declared input",[&](auto& s){auto shift=[](auto* p){using T=std::remove_pointer_t<decltype(p)>;return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+1);};switch(i){case 0:s.source.endpoints=shift(s.source.endpoints);break;case 1:s.source.sceneMetadata=shift(s.source.sceneMetadata);break;case 2:s.source.records=shift(s.source.records);break;default:s.candidatePointer=shift(s.candidatePointer);break;}});}
    for(unsigned i=0;i<10;++i)rejection("insufficient bounded capacity",[&](auto& s){switch(i){case 0:s.workspace.buffers.rays.capacity=3;break;case 1:s.destination.rays.capacity=3;break;case 2:s.workspace.buffers.rays.lengthCapacity=3;break;case 3:s.destination.rays.lengthCapacity=3;break;case 4:s.workspace.buffers.recordCapacity=3;break;case 5:s.destination.recordCapacity=3;break;case 6:s.workspace.buffers.effectCapacity=1;break;case 7:s.destination.effectCapacity=1;break;case 8:s.workspace.treeStackCapacity=1;break;default:s.context.spring.generatedCapacity88=5;break;}},Status::InsufficientCapacity);
    rejection("short endpoints",[](auto& s){s.source.endpointCount=1;},Status::InsufficientCapacity);rejection("short scene metadata",[](auto& s){s.source.sceneMetadataCount=1;},Status::InsufficientCapacity);rejection("pair count beyond supplied rays",[](auto& s){s.context.pairCount=5;},Status::InsufficientCapacity);rejection("zero pair count",[](auto& s){s.context.pairCount=0;});rejection("negative signed pair count",[](auto& s){s.context.pairCount=0xFFFFFFFF;});
    constexpr auto maximum=std::numeric_limits<std::size_t>::max();
    for(unsigned i=0;i<16;++i)rejection("overflowed declared extent",[&](auto& s){switch(i){case 0:s.source.rays.capacity=maximum;s.workspace.buffers.rays.capacity=maximum;s.destination.rays.capacity=maximum;break;case 1:s.source.rays.lengthCapacity=maximum;s.workspace.buffers.rays.lengthCapacity=maximum;s.destination.rays.lengthCapacity=maximum;break;case 2:s.source.endpointCount=maximum;break;case 3:s.source.sceneMetadataCount=maximum;break;case 4:s.source.recordCount=maximum;s.workspace.buffers.recordCapacity=maximum;s.destination.recordCapacity=maximum;break;case 5:s.count=maximum;s.workspace.buffers.effectCapacity=maximum;s.destination.effectCapacity=maximum;break;case 6:s.workspace.buffers.rays.capacity=maximum;break;case 7:s.workspace.buffers.rays.lengthCapacity=maximum;break;case 8:s.destination.rays.capacity=maximum;break;case 9:s.destination.rays.lengthCapacity=maximum;break;case 10:s.workspace.buffers.recordCapacity=maximum;break;case 11:s.destination.recordCapacity=maximum;break;case 12:s.workspace.buffers.effectCapacity=maximum;break;case 13:s.destination.effectCapacity=maximum;break;case 14:s.workspace.treeStackCapacity=maximum;break;default:s.candidates[1].sceneNameBytes=maximum;break;}});
    const auto nearEnd=std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15);
    for(unsigned w=0;w<23;++w)rejection("wrapping writable extent",[&](auto& s){s.setWrite(w,reinterpret_cast<void*>(nearEnd));});
    for(unsigned r=0;r<9;++r)rejection("wrapping source ray extent",[&](auto& s){setSourceRay(s.source.rays,r,reinterpret_cast<void*>(nearEnd));});
    for(unsigned i=0;i<5;++i)rejection("wrapping nested/source extent",[&](auto& s){switch(i){case 0:s.source.endpoints=reinterpret_cast<const SpringVector4_48N*>(nearEnd);break;case 1:s.source.sceneMetadata=reinterpret_cast<const std::uint32_t*>(nearEnd);break;case 2:s.source.records=reinterpret_cast<const SpringRecord48N*>(nearEnd);break;case 3:s.candidatePointer=reinterpret_cast<const Type3Candidate48P*>(nearEnd);break;default:s.candidates[1].sceneName=reinterpret_cast<const char*>(nearEnd);break;}});
    // Context is the first immutable span, tested against every mutable kind.
    // Twenty-four mutable spans: 18 ray spans, two records, two effects, stack,
    // and result. Test every pair, plus every nested read span against each kind.
    for(unsigned a=0;a<24;++a)for(unsigned b=a+1;b<24;++b){label="mutable alias "+std::to_string(a)+":"+std::to_string(b);Safety s;s.setWrite(a,s.write(b));s.reject(Status::InvalidInput,true);}
    std::size_t readCount=0;{Safety s;readCount=s.reads().size();}
    for(unsigned w=0;w<24;++w)for(std::size_t r=0;r<readCount;++r){label="nested source alias "+std::to_string(w)+":"+std::to_string(r);Safety s;void* p=s.reads()[r];if(w==23&&reinterpret_cast<std::uintptr_t>(p)%alignof(Type3QueryResult48P))continue;s.setWrite(w,p);s.reject(Status::InvalidInput,true);contextAliasCases+=r==0;}
    // An overlap confined to the context's final aligned bytes must also fail.
    // This catches validators that accidentally protect only its first member.
    for(unsigned w=0;w<24;++w){
        label="context tail alias "+std::to_string(w);Safety s;
        constexpr auto tail=(sizeof(Type3QueryContext48P)-1)&~(alignof(Type3QueryResult48P)-1);
        auto* p=reinterpret_cast<unsigned char*>(&s.context)+tail;
        s.setWrite(w,p);s.reject(Status::InvalidInput,true);++contextAliasCases;
    }
    check(contextAliasCases==48,"all24 writable kinds reject context head and tail overlaps");
    rejection("tail-only ray alias",[](auto& s){s.workspace.buffers.rays.localStarts=s.destinationRays.localStarts.data();s.workspace.buffers.rays.capacity=6;s.destination.rays.localStarts=s.destinationRays.localStarts.data()+4;s.destination.rays.capacity=1;s.source.rays.capacity=1;});
    rejection("unused destination ray tail aliases source",[](auto& s){s.destination.rays.localStarts=s.inputRays.localStarts.v.data();s.source.rays.localStarts=s.inputRays.localStarts.v.data()+4;s.source.rays.capacity=1;});
    rejection("tail-only record overlap",[](auto& s){s.workspace.buffers.records=s.destinationRecords.v.data();s.workspace.buffers.recordCapacity=6;s.destination.records=s.destinationRecords.v.data()+4;s.destination.recordCapacity=1;s.source.recordCount=1;s.context.spring.generatedCapacity88=1;});
    rejection("tail-only effect overlap",[](auto& s){s.workspace.buffers.effects=s.destinationEffects.v.data();s.workspace.buffers.effectCapacity=4;s.destination.effects=s.destinationEffects.v.data()+3;s.destination.effectCapacity=1;s.count=1;});
    rejection("callback needs three length words",[](auto& s){s.source.rays.lengthCapacity=2;},Status::InsufficientCapacity);
    rejection("callback bound exceeds source hit slots",[](auto& s){s.context.spring.pointCount7C=5;},Status::InsufficientCapacity);
    rejection("invalid query entry tag",[](auto& s){s.context.entry=static_cast<Type3QueryEntry48P>(99);});
    rejection("nonzero used endpoint scene",[](auto& s){s.metadata[1]=123;},Status::UnsupportedScope);
    rejection("negative spring record count",[](auto& s){s.context.spring.generatedCount84=-1;});rejection("negative spring logical capacity",[](auto& s){s.context.spring.generatedCapacity88=-1;});
    rejection("late compound branch",[](auto& s){s.candidates[1].shapeType=1;},Status::UnsupportedScope);rejection("late unknown callback",[](auto& s){s.candidates[1].collision.hasResponseCallbackE8=true;},Status::UnsupportedScope);rejection("late group recursion",[](auto& s){s.candidates[1].collision.typeTag=s.q.groupTypeTag;},Status::UnsupportedScope);rejection("late dynamic callback",[](auto& s){s.candidates[1].collision.filter.categoryA4|=s.q.dynamicMask;},Status::UnsupportedScope);
    rejection("late too-short material delimiter",[](auto& s){std::strcpy(s.names[1],"ab");},Status::UnsupportedScope);rejection("late missing material delimiter",[](auto& s){std::memset(s.names[1],'x',sizeof(s.names[1]));});rejection("late missing body scene",[](auto& s){s.context.bodyScenePresent=false;});
    rejection("normal mode2 actually reached",[](auto& s){s.q.normalMode=2;},Status::UnsupportedScope);rejection("late mode1 interpolation boundary",[](auto& s){s.q.normalMode=1;s.candidates[1].collision.geometry.worldMatrix.v[12]=4.25f;s.candidates[1].collision.geometry.worldMatrix.v[14]=4.125f;},Status::UnsupportedInterpolationBoundary);
    rejection("late identity mismatch",[](auto& s){s.candidates[1].collision.geometry.id=99;});rejection("late null mesh",[](auto& s){s.candidates[1].collision.geometry.mesh=nullptr;});rejection("misaligned nested mesh",[](auto& s){s.candidates[1].collision.geometry.mesh=reinterpret_cast<const CollisionMesh48I*>(reinterpret_cast<std::uintptr_t>(&s.m)+1);});
    rejection("nonfinite candidate sphere",[](auto& s){s.candidates[1].collision.sphere.radius=std::numeric_limits<float>::quiet_NaN();});rejection("negative candidate sphere",[](auto& s){s.candidates[1].collision.sphere.radius=-1;});rejection("nonfinite candidate matrix",[](auto& s){s.candidates[1].collision.geometry.worldMatrix.v[12]=std::numeric_limits<float>::infinity();});rejection("nonfinite endpoint",[](auto& s){s.endpoints[0].v[0]=std::numeric_limits<float>::infinity();});
    rejection("malformed tree cycle",[](auto& s){s.m.nodes[0].children[0]=0;});rejection("malformed leaf index",[](auto& s){s.m.nodes[0].children[1]=collision_mesh_leaf48i|99;});rejection("malformed vertex index",[](auto& s){s.m.faces[1].vertices[0]=99;});rejection("nonfinite vertex",[](auto& s){s.m.vertices[0][0]=std::numeric_limits<float>::quiet_NaN();});
    rejection("direct requires one candidate",[](auto& s){s.context.entry=Type3QueryEntry48P::Dispatcher;});rejection("direct empty disallowed",[](auto& s){s.context.entry=Type3QueryEntry48P::Dispatcher;s.count=0;});
    label="exact bounded capacities";{Safety s;s.workspace.buffers.rays.capacity=s.destination.rays.capacity=4;s.workspace.buffers.rays.lengthCapacity=s.destination.rays.lengthCapacity=4;s.workspace.buffers.recordCapacity=s.destination.recordCapacity=4;s.workspace.buffers.effectCapacity=s.destination.effectCapacity=2;s.workspace.treeStackCapacity=2;check(s.call()==Status::Complete,"exact capacities succeed");check(s.out.appended==2,"exact storage keeps both appends");}
    label="zero physical records";{Safety s;s.context.spring.generatedCapacity88=0;s.source.records=nullptr;s.source.recordCount=0;s.workspace.buffers.records=nullptr;s.workspace.buffers.recordCapacity=0;s.destination.records=nullptr;s.destination.recordCapacity=0;check(s.call()==Status::Complete,"zero physical record spans succeed");check(s.out.callbacks==2&&s.out.saturated==2&&s.out.appended==0,"zero backing retains original saturation");}
    label="read-only sources may overlap";{Safety s;s.source.rays.worldStarts=s.endpoints.data();check(s.call()==Status::Complete,"read/read overlap accepted");check(s.out.appended==2,"read/read overlap preserves prepared hits");}
    label="saturation succeeds";{Safety s;s.context.spring.generatedCapacity88=0;check(s.call()==Status::Complete,"zero logical capacity completes");check(s.out.appended==0&&s.out.saturated==2&&s.out.callbacks==2,"each hit saturates without append");check(std::memcmp(s.destination.records,s.source.records,4*sizeof(SpringRecord48N))==0,"saturated records all unchanged");}
    label="overfull count preserved";{Safety s;s.context.spring.generatedCount84=7;check(s.call()==Status::Complete,"count above logical cap succeeds");check(s.out.spring.generatedCount84==7&&s.out.saturated==2,"overfull count unchanged");}
    label="skip precedes dynamic callback";{Safety s;for(auto& c:s.candidates)c.collision.filter.categoryA4|=s.q.skipMask|s.q.dynamicMask;check(s.call()==Status::Complete,"skipped dynamic completes");check(s.out.callbacks==2&&s.out.appended==0&&s.out.saturated==0,"skips counted, no append");}
    label="callback disabled";{Safety s;s.q.callbackEnabled=false;s.context.bodyScenePresent=false;for(auto& c:s.candidates)c.collision.filter.categoryA4|=s.q.dynamicMask;check(s.call()==Status::Complete,"disabled callback avoids body/dynamic branch");check(s.out.geometryHits==2&&s.out.callbacks==0,"geometry retained without callback");}
    label="mode2 without normals";{Safety s;s.q.normalMode=2;s.m.vertex_normals.clear();check(s.call()==Status::Complete,"absent normals avoid mode2 helper");check(s.out.geometryHits==2,"mode2 flat-normal hits retained");}
    label="unreached unsupported candidate";{Safety s;s.candidates[1].collision.sphere.center.v[0]=1000;s.candidates[1].shapeType=1;s.candidates[1].collision.hasResponseCallbackE8=true;std::strcpy(s.names[1],"ab");check(s.call()==Status::Complete,"sphere filter prevents unsupported branch");check(s.out.geometryHits==1,"only first candidate hits");}
    label="unhandled shape tag commits resets";{Safety s;s.candidates[1].shapeType=3;check(s.call()==Status::Complete,"shape3 miss supported");check(s.out.hasAnyHit&&s.out.geometryHits==1&&s.out.state.hitCount==0&&s.destination.rays.hits[0]==0&&s.destination.rays.fractions[0]==-1.f,"later miss preserves cumulative query true and per-candidate resets");check(s.destination.rays.normals[0].v[3]==2.f,"earlier normal remains stale");}
    label="unused scene metadata accepted";{Safety s;s.metadata[7]=0xDEADBEEF;check(s.call()==Status::Complete,"only used endpoint metadata needs zero");}
    label="empty list retains preparation and prior current";{Safety s;s.count=0;s.candidatePointer=nullptr;s.q.initialQueryCurrent=0x12345678;s.context.resetSpring=true;check(s.call()==Status::Complete,"empty list succeeds");check(!s.out.hasAnyHit&&s.out.currentCandidate==0x12345678&&s.out.spring.pending38==1&&s.out.spring.generatedCount84==0,"empty list reset/current semantics");check(s.destination.rays.lengths[0]==2.f&&s.out.state.hitCount==0,"empty list still prepares");}
}
void benchmark(){
    label="serial bounded type3 cost";Safety s;constexpr std::size_t loops=4000;std::array<double,7> samples{};std::size_t complete=0,appended=0;const auto before=observedAllocations;const auto borrowed=s.borrowed();
    for(auto& ns:samples){auto start=std::chrono::steady_clock::now();watchAllocations=true;for(std::size_t i=0;i<loops;++i){complete+=queryStaticType3WithContextBounded48P(s.q,s.context,s.source,s.candidatePointer,s.count,s.workspace,s.destination,s.out)==Status::Complete;appended+=s.out.appended;}watchAllocations=false;ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/loops;}
    for(const auto& snapshot:borrowed)snapshot.verify();
    std::sort(samples.begin(),samples.end());check(complete==loops*samples.size()&&appended==2*complete,"serial reuse has stable complete appends");check(observedAllocations==before,"serial reuse allocates nothing");const auto rayBytes=s.workspace.buffers.rays.capacity*(2*sizeof(Vector3_48C)+4*sizeof(SpringVector4_48N)+sizeof(std::uint32_t)+sizeof(float))+s.workspace.buffers.rays.lengthCapacity*sizeof(float);
    std::cout<<"type3_context48Q_cost_host_ns median="<<samples[3]<<" max_batch_mean="<<samples.back()<<" serial_queries="<<complete<<" pairs="<<s.context.pairCount<<" candidate_count="<<s.count<<" ray_scratch_bytes="<<rayBytes<<" record_scratch_bytes="<<s.workspace.buffers.recordCapacity*sizeof(SpringRecord48N)<<" effect_scratch_bytes="<<s.workspace.buffers.effectCapacity*sizeof(TerrainCandidateEffects48J)<<" tree_stack_bytes="<<s.workspace.treeStackCapacity*sizeof(std::uint32_t)<<" result_bytes="<<sizeof(Type3QueryResult48P)<<" workspace_descriptor_bytes="<<sizeof(Type3QueryWorkspace48P)<<" source_descriptor_bytes="<<sizeof(Type3QuerySource48P)<<" query_context_bytes="<<sizeof(Type3QueryContext48P)<<" query_input_bytes="<<sizeof(Type3QueryInput48P)<<" runtime_allocations="<<(observedAllocations-before)<<" (serial host component observation; no game/Xbox FPS or install claim)\n";
}
}
int main(int argc,char** argv){try{if(argc!=2)return 2;std::fesetround(FE_TONEAREST);std::ifstream f(std::filesystem::path(argv[1])/"evidence/type3Query48P/native-fixtures.json");if(!f)throw std::runtime_error("missing type3Query48P/native-fixtures.json");Json fixture;f>>fixture;if(fixture.at("schema")!=1||fixture.at("array_slots")!=4||fixture.at("cases").size()!=245||fixture.at("meshes").size()!=11)throw std::runtime_error("incomplete type3 fixture");std::vector<CollisionMesh48I> meshes;for(const auto& m:fixture.at("meshes"))meshes.push_back(mesh(m));std::size_t stack=0;for(const auto& m:meshes)stack=std::max(stack,stackBound(m));std::vector<Snapshot> snapshots;for(const auto& m:meshes)captureMesh(snapshots,m);for(const auto& j:fixture.at("cases"))differential(j,meshes,stack);for(const auto& s:snapshots)s.verify();check(directCases==5,"exactly five direct dispatcher projections");check(differentialCases==190&&unsupportedCases==50,"frozen native/original-only scope counts");check(differentialCases+directCases+unsupportedCases==fixture.at("cases").size(),"every original fixture classified and executed");safety();benchmark();check(observedAllocations==0,"all guarded runtime calls allocation-free");std::cout<<"type3_context48Q poisoned_template_cases=245 full_cases="<<differentialCases<<" direct_cases="<<directCases<<" original_only_rejections="<<unsupportedCases<<" checks="<<checks<<" failures="<<failures<<" queries_with_geometry_hits="<<totalHits<<" candidate_geometry_hits="<<totalGeometryHits<<" callbacks="<<totalCallbacks<<" appended="<<totalAppended<<" saturated="<<totalSaturated<<" safety_cases="<<safetyCases<<" alias_cases="<<aliasCases<<" context_alias_cases="<<contextAliasCases<<" fixture_max_tree_stack_entries="<<maxStack<<" fixture_max_records="<<maxRecords<<" fixture_max_candidates="<<maxCandidates<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;}catch(const std::exception& e){watchAllocations=false;std::cerr<<label<<": "<<e.what()<<'\n';return 2;}}
