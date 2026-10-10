#include "spring_query48n.h"
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
using Status=SpringQueryStatus48N;
template<class T>void fillSentinel(T* storage,std::size_t count,unsigned char value){
    static_assert(std::is_trivially_copyable_v<T>,"byte sentinels require trivially copyable storage");
    auto* bytes=reinterpret_cast<unsigned char*>(storage);
    std::fill_n(bytes,count*sizeof(T),value);
}
std::size_t checks=0,failures=0,differentialCases=0,safetyCases=0,aliasCases=0;
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
void checkPair(const SpringCumulativePair48N& a,const Json& p){for(unsigned k=0;k<4;++k){check(bits(a.first.v[k])==p.at("endpoints")[k].get<std::uint32_t>(),"pair first full4");check(bits(a.second.v[k])==p.at("endpoints")[k+4].get<std::uint32_t>(),"pair second full4");check(bits(a.normal.v[k])==p.at("normal")[k].get<std::uint32_t>(),"pair normal full4");check(bits(a.surfacePoint.v[k])==p.at("surface")[k].get<std::uint32_t>(),"pair surface full4");}}
void checkSelected(const CollisionContact48I& a,const Json& p){const auto& r=p.at("selected");for(unsigned k=0;k<3;++k){check(bits(a.point.v[k])==r[k+12].get<std::uint32_t>(),"selected point original bits");check(bits(a.normal.v[k])==r[k+8].get<std::uint32_t>(),"selected normal original bits");check(bits(a.correction.v[k])==r[k+16].get<std::uint32_t>(),"selected correction original bits");}check(bits(a.fraction)==r[20].get<std::uint32_t>(),"selected fraction original bits");check(a.candidateId==p.at("selected_ids")[0]&&a.face==p.at("selected_ids")[1],"selected identity and face");}
void differential(const Json& j,const std::vector<CollisionMesh48I>& meshes,std::size_t stackCapacity){
    label=j.at("name").get<std::string>();const auto& qj=j.at("query");LocalSegment48I segment{};for(unsigned k=0;k<3;++k){segment.first.v[k]=fp(qj.at("first")[k]);segment.second.v[k]=fp(qj.at("second")[k]);}floats(qj.at("world"),segment.worldMatrix.v);auto qs=sphere(qj.at("sphere"));auto qf=filter(qj.at("filter"));std::array<float,4> copy40{},copy50{};rawFloats(qj.at("copied40"),copy40);rawFloats(qj.at("copied50"),copy50);
    SpringQueryView48N q{segment,qs,qf,copy40,copy50};q.firstW=qj.at("first")[3];q.secondW=qj.at("second")[3];q.queryScene={qj.at("scene_id")};q.skipMask=j.at("masks")[0];q.dynamicMask=j.at("masks")[1];q.materialMask=j.at("masks")[2];q.collisionTypeTag=qj.at("collision_tag");q.groupTypeTag=qj.at("group_tag");q.callbackEnabled=qj.at("callback");q.resetBefore=j.at("reset_before");q.initialQueryCurrentIdentity=qj.value("initial_current_candidate",0u);
    auto initial=state(j.at("initial_state"));std::vector<std::string> names;names.reserve(j.at("candidates").size());for(const auto& c:j.at("candidates"))names.push_back(unhex(c.at("name_hex")));
    std::vector<SpringStaticCandidate48N> pool(j.at("candidates").size());for(std::size_t i=0;i<pool.size();++i){auto& c=pool[i];const auto& p=j.at("candidates")[i];c.collision.geometry.mesh=&meshes.at(p.at("mesh"));c.collision.geometry.id=p.at("id");floats(p.at("world"),c.collision.geometry.worldMatrix.v);c.collision.sphere=sphere(p.at("sphere"));c.collision.filter=filter(p.at("filter"));c.collision.typeTag=p.at("type_tag");rawFloats(p.at("copied40"),c.collision.copied40);rawFloats(p.at("copied50"),c.collision.copied50);c.hasMaterialScene=p.at("has_scene");c.sceneName=names[i].data();c.sceneNameBytes=names[i].size();if(c.collision.filter.identity==initial.currentCandidate130)q.initialCurrentCategory=c.collision.filter.categoryA4;}
    std::vector<SpringStaticCandidate48N> ordered;for(const auto& i:j.at("order"))ordered.push_back(pool.at(i));
    std::vector<SpringRecord48N> input;for(const auto& r:j.at("initial_records"))input.push_back(record(r));
    std::vector<SpringRecord48N> scratch(input.size()+2),destination(input.size()+2);fillSentinel(scratch.data(),scratch.size(),0xA5);fillSentinel(destination.data(),destination.size(),0x5A);
    std::vector<TerrainCandidateEffects48J> effectScratch(ordered.size()+2),effects(ordered.size()+2);fillSentinel(effectScratch.data(),effectScratch.size(),0xA5);fillSentinel(effects.data(),effects.size(),0x5A);std::vector<std::uint32_t> stack(stackCapacity+2,0xA55AA55A);
    SpringQueryWorkspace48N w{scratch.data()+1,input.size(),effectScratch.data()+1,ordered.size(),stack.data()+1,stackCapacity};SpringQueryResult48N out{};
    std::vector<Snapshot> immutable;capture(immutable,&q,sizeof(q));capture(immutable,&segment,sizeof(segment));capture(immutable,&qs,sizeof(qs));captureFilter(immutable,qf);capture(immutable,&initial,sizeof(initial));captureVector(immutable,input);capture(immutable,&w,sizeof(w));captureVector(immutable,ordered);capture(immutable,copy40.data(),sizeof(copy40));capture(immutable,copy50.data(),sizeof(copy50));for(const auto& c:ordered)captureFilter(immutable,c.collision.filter);for(const auto& n:names)capture(immutable,n.data(),n.size());
    std::vector<Snapshot> guards;for(auto* p:{scratch.data(),scratch.data()+scratch.size()-1,destination.data(),destination.data()+destination.size()-1})capture(guards,p,sizeof(*p));for(auto* p:{effectScratch.data(),effectScratch.data()+effectScratch.size()-1,effects.data(),effects.data()+effects.size()-1})capture(guards,p,sizeof(*p));capture(guards,stack.data(),sizeof(stack[0]));capture(guards,stack.data()+stack.size()-1,sizeof(stack[0]));
    watchAllocations=true;const auto status=queryStaticSpringBounded48N(q,ordered.data(),ordered.size(),initial,input.data(),input.size(),w,destination.data()+1,input.size(),effects.data()+1,ordered.size(),out);watchAllocations=false;
    check(status==Status::Complete,"complete original ordered query");if(status!=Status::Complete)std::cerr<<label<<": status="<<static_cast<unsigned>(status)<<'\n';const auto& e=j.at("expected");checkState(out.state,e.at("spring_state"));checkRecords(destination.data()+1,input.size(),e.at("records"));check(out.queryFlagsF8==e.at("query_flags"),"query flags");check(out.currentCandidateIdentity==e.at("current_candidate"),"last enumerated collision candidate");check(out.hasGeometryHit==(e.at("returned").get<unsigned>()!=0),"query hit/miss");check(out.candidateCount==ordered.size(),"ordered occurrence count");
    std::size_t geometryHits=0,appended=0,saturated=0;for(const auto& t:e.at("tree_calls"))geometryHits+=t.at("returned").get<unsigned>()!=0;
    for(const auto& cb:e.at("callback_calls")){const auto before=cb.at("state_before")[8].get<unsigned>(),after=cb.at("state_after")[8].get<unsigned>();appended+=after-before;const auto id=cb.at("identity").get<unsigned>();const auto it=std::find_if(pool.begin(),pool.end(),[&](const auto& c){return c.collision.filter.identity==id;});if(it==pool.end())throw std::runtime_error("callback identity");if(!(it->collision.filter.categoryA4&q.skipMask)&&after==before)++saturated;}
    check(out.geometryHits==geometryHits,"geometry hit counter");check(out.callbacks==e.at("callback_calls").size(),"callback invocation counter includes skips");check(out.appended==appended,"append counter");check(out.saturated==saturated,"logical saturation counter");
    for(std::size_t i=0;i<ordered.size();++i){const auto& a=effects[i+1];const auto& b=e.at("candidates").at(j.at("order")[i].get<std::size_t>());check(a.identity==b.at("identity")&&a.flagsF8==b.at("flags"),"candidate occurrence identity/flags");for(unsigned k=0;k<4;++k){check(bits(a.copied40[k])==b.at("copied40")[k].get<std::uint32_t>(),"candidate copied40 full4");check(bits(a.copied50[k])==b.at("copied50")[k].get<std::uint32_t>(),"candidate copied50 full4");}}
    if(out.hasGeometryHit){++totalHits;const auto& p=e.at("pair");checkPair(out.pair,p);checkSelected(out.collision.selected,p);const auto& a=out.collision.nearestPair;for(unsigned k=0;k<3;++k){check(bits(a.point.v[k])==p.at("surface")[k].get<std::uint32_t>(),"nearest original point");check(bits(a.normal.v[k])==p.at("normal")[k].get<std::uint32_t>(),"nearest original normal");}check(bits(a.fraction)==p.at("fraction").get<std::uint32_t>(),"nearest original fraction");check(a.candidateId==p.at("pair_ids")[0]&&a.face==p.at("pair_ids")[1],"nearest original identity/face");
        // Original pair backing has no correction vector. Check that additional
        // native field against lower-level composition, separately from oracle.
        std::vector<CollisionCandidate48I> eligible;for(const auto& t:e.at("tree_calls")){const auto id=t.at("identity").get<unsigned>();const auto it=std::find_if(pool.begin(),pool.end(),[&](const auto& c){return c.collision.filter.identity==id;});eligible.push_back(it->collision.geometry);}CollisionResult48I composed{};std::size_t used=0;CollisionWorkspace48M cw{stack.data()+1,stackCapacity,nullptr,0};watchAllocations=true;const auto cs=queryOrderedCollisionBounded48M(segment,eligible.data(),eligible.size(),cw,composed,used);watchAllocations=false;check(cs==CollisionQueryStatus48I::Hit,"lower-level native composition hit");check(std::memcmp(&out.collision.nearestPair,&composed.nearestPair,sizeof(composed.nearestPair))==0,"complete nearest contact native composition consistency");
    }else{SpringCumulativePair48N zeroPair{};CollisionContact48I zeroContact{};check(std::memcmp(&out.pair,&zeroPair,sizeof(zeroPair))==0,"completed miss initializes native pair");check(std::memcmp(&out.collision.selected,&zeroContact,sizeof(zeroContact))==0&&std::memcmp(&out.collision.nearestPair,&zeroContact,sizeof(zeroContact))==0,"completed miss initializes native contacts");}
    if(j.at("reset_after").get<bool>()){watchAllocations=true;resetSpringRecords48N(out.state);watchAllocations=false;checkState(out.state,e.at("after_reset").at("spring_state"));checkRecords(destination.data()+1,input.size(),e.at("after_reset").at("records"));}
    for(const auto& s:immutable)s.verify();
    for(const auto& s:guards)s.verify();
    ++differentialCases;totalGeometryHits+=out.geometryHits;totalCallbacks+=out.callbacks;totalAppended+=out.appended;totalSaturated+=out.saturated;maxStack=std::max(maxStack,stackCapacity);maxRecords=std::max(maxRecords,input.size());maxCandidates=std::max(maxCandidates,ordered.size());
}
struct Safety {
    alignas(std::max_align_t) LocalSegment48I segment{};
    alignas(std::max_align_t) CollisionSphere48J sphereValue{{{0,0,0}},20};
    CollisionFilter48J queryFilter{};
    alignas(std::max_align_t) std::array<float,4> copied40{{1,2,3,4}},copied50{{5,6,7,8}};
    CollisionMesh48I meshValue{};
    std::array<SpringStaticCandidate48N,2> candidates{};
    alignas(std::max_align_t) char names[2][64]{};
    alignas(std::max_align_t) SpringRecordState48N initial{11,12,13,14,15,16,0,18,1,3,21};
    alignas(std::max_align_t) std::array<SpringRecord48N,6> input{},recordScratch{},recordDestination{};
    alignas(std::max_align_t) std::array<TerrainCandidateEffects48J,6> effectScratch{},effectDestination{};
    alignas(std::max_align_t) std::array<std::uint32_t,16> stack{};
    SpringQueryView48N query;
    SpringQueryWorkspace48N workspace;
    SpringQueryResult48N output{};
    const SpringStaticCandidate48N* candidatePointer;
    const SpringRecord48N* inputPointer;
    SpringRecord48N* recordPointer;
    TerrainCandidateEffects48J* effectPointer;
    SpringQueryResult48N* outputPointer;
    std::size_t count=2,recordCount=4,recordCapacity=4,effectCapacity=2;
    Safety():query{segment,sphereValue,queryFilter,copied40,copied50},workspace{recordScratch.data()+1,4,effectScratch.data()+1,2,stack.data()+1,4},candidatePointer(candidates.data()),inputPointer(input.data()+1),recordPointer(recordDestination.data()+1),effectPointer(effectDestination.data()+1),outputPointer(&output){
        for(unsigned i=0;i<16;++i)segment.worldMatrix.v[i]=(i%5)==0?1.f:0.f;
        segment.first={{0.25f,2.f,0.125f}};segment.second={{0.25f,-2.f,0.125f}};
        meshValue.vertices={{{-4,0,-4}},{{0,0,4}},{{4,0,-4}},{{16,0,16}},{{20,0,24}},{{24,0,16}}};
        meshValue.vertex_normals.assign(6,{{0,1,0}});
        meshValue.nodes.push_back({100,{{10,0,10}},{{20,1,20}},{{collision_mesh_leaf48i,collision_mesh_leaf48i|1}}});
        meshValue.faces.push_back({0,{{0,32767,0}},{{0,1,2}}});meshValue.faces.push_back({0,{{0,32767,0}},{{3,4,5}}});
        queryFilter.identity=10;queryFilter.categoryA4=1;queryFilter.flagsF8=4;queryFilter.flags31=1;queryFilter.categoryMasks={0};queryFilter.identityLists[0].identities={900};queryFilter.identityLists[1].identities={901};
        for(unsigned i=0;i<2;++i){std::strcpy(names[i],"trackfr7.col");auto& s=candidates[i];auto& c=s.collision;c.geometry={&meshValue,segment.worldMatrix,20+i};c.filter=queryFilter;c.filter.identity=20+i;c.filter.categoryA4=4;c.sphere=sphereValue;c.typeTag=2;c.copied40={{11,12,13,14}};c.copied50={{15,16,17,18}};s.hasMaterialScene=true;s.sceneName=names[i];s.sceneNameBytes=std::strlen(names[i])+1;}
        query.queryScene={7};query.skipMask=8;query.dynamicMask=16;query.materialMask=4;query.collisionTypeTag=2;query.groupTypeTag=3;query.firstW=0x80000000;query.secondW=0x7FC12345;
        fillSentinel(input.data(),input.size(),0xCD);fillSentinel(recordScratch.data(),recordScratch.size(),0xA5);fillSentinel(recordDestination.data(),recordDestination.size(),0x5A);fillSentinel(effectScratch.data(),effectScratch.size(),0xA5);fillSentinel(effectDestination.data(),effectDestination.size(),0x5A);stack.fill(0xA55AA55A);output.queryFlagsF8=0xCB;output.state.pending38=77;output.candidateCount=91;
    }
    Status call(){watchAllocations=true;auto s=queryStaticSpringBounded48N(query,candidatePointer,count,initial,inputPointer,recordCount,workspace,recordPointer,recordCapacity,effectPointer,effectCapacity,*outputPointer);watchAllocations=false;return s;}
    std::vector<Snapshot> snapshots(bool includeScratch=false){std::vector<Snapshot> v;capture(v,&query,sizeof(query));capture(v,&workspace,sizeof(workspace));capture(v,&segment,sizeof(segment));capture(v,&sphereValue,sizeof(sphereValue));capture(v,copied40.data(),sizeof(copied40));capture(v,copied50.data(),sizeof(copied50));captureFilter(v,queryFilter);captureMesh(v,meshValue);capture(v,candidates.data(),sizeof(candidates));for(const auto& c:candidates)captureFilter(v,c.collision.filter);capture(v,names,sizeof(names));capture(v,&initial,sizeof(initial));capture(v,input.data(),sizeof(input));capture(v,recordDestination.data(),sizeof(recordDestination));capture(v,effectDestination.data(),sizeof(effectDestination));capture(v,&output,sizeof(output));if(includeScratch){capture(v,recordScratch.data(),sizeof(recordScratch));capture(v,effectScratch.data(),sizeof(effectScratch));capture(v,stack.data(),sizeof(stack));}return v;}
    void reject(Status expected,bool aliases=false){auto saved=snapshots(aliases);const auto before=observedAllocations;const auto actual=call();check(actual==expected,"rejected status");if(actual!=expected)std::cerr<<label<<": actual="<<static_cast<unsigned>(actual)<<" expected="<<static_cast<unsigned>(expected)<<'\n';check(observedAllocations==before,"rejection makes zero allocations");for(const auto& s:saved)s.verify();++safetyCases;if(aliases)++aliasCases;}
    std::vector<void*> reads(){std::vector<void*> r{&query,&workspace,&segment,&sphereValue,&queryFilter,copied40.data(),copied50.data(),&initial,input.data()+1,candidates.data(),&meshValue,meshValue.vertices.data(),meshValue.vertex_normals.data(),meshValue.nodes.data(),meshValue.faces.data(),names[0],names[1]};r.push_back(queryFilter.categoryMasks.data());for(auto& l:queryFilter.identityLists)r.push_back(l.identities.data());for(auto& c:candidates){r.push_back(c.collision.filter.categoryMasks.data());for(auto& l:c.collision.filter.identityLists)r.push_back(l.identities.data());}return r;}
    void* write(unsigned i){switch(i){case 0:return workspace.records;case 1:return recordPointer;case 2:return workspace.effects;case 3:return effectPointer;case 4:return workspace.treeStack;default:return outputPointer;}}
    void setWrite(unsigned i,void* p){switch(i){case 0:workspace.records=static_cast<SpringRecord48N*>(p);break;case 1:recordPointer=static_cast<SpringRecord48N*>(p);break;case 2:workspace.effects=static_cast<TerrainCandidateEffects48J*>(p);break;case 3:effectPointer=static_cast<TerrainCandidateEffects48J*>(p);break;case 4:workspace.treeStack=static_cast<std::uint32_t*>(p);break;default:outputPointer=static_cast<SpringQueryResult48N*>(p);break;}}
};
template<class Change>void rejection(const char* name,Change change,Status expected=Status::InvalidInput){label=name;Safety s;change(s);s.reject(expected);}
void safety(){
    label="synthetic baseline";{Safety s;check(s.call()==Status::Complete,"synthetic valid baseline");check(s.output.hasGeometryHit&&s.output.geometryHits==2&&s.output.callbacks==2&&s.output.appended==2,"baseline exercises hit/callback/append");}
    label="exact tree stack and larger destination tails";{Safety s;s.workspace.treeStackCapacity=2;s.workspace.recordCapacity=5;s.recordCapacity=5;s.workspace.effectCapacity=4;s.effectCapacity=4;std::vector<Snapshot> tail;capture(tail,s.recordPointer+4,sizeof(SpringRecord48N));capture(tail,s.effectPointer+2,2*sizeof(TerrainCandidateEffects48J));check(s.call()==Status::Complete,"exact two-entry tree stack succeeds");for(const auto& v:tail)v.verify();check(s.output.geometryHits==2&&s.output.appended==2,"larger declared spans preserve normal semantics");}
    for(std::size_t n=0;n<4;++n){rejection("record scratch shortage",[&](auto& s){s.workspace.recordCapacity=n;},Status::InsufficientCapacity);rejection("record destination shortage",[&](auto& s){s.recordCapacity=n;},Status::InsufficientCapacity);}
    for(std::size_t n=0;n<2;++n){rejection("effect scratch shortage",[&](auto& s){s.workspace.effectCapacity=n;},Status::InsufficientCapacity);rejection("effect destination shortage",[&](auto& s){s.effectCapacity=n;},Status::InsufficientCapacity);rejection("tree stack shortage",[&](auto& s){s.workspace.treeStackCapacity=n;},Status::InsufficientCapacity);}
    rejection("full logical record capacity",[](auto& s){s.initial.generatedCapacity88=5;},Status::InsufficientCapacity);
    rejection("negative initial count",[](auto& s){s.initial.generatedCount84=-1;});rejection("negative capacity",[](auto& s){s.initial.generatedCapacity88=-1;});
    rejection("huge logical capacity",[](auto& s){s.initial.generatedCapacity88=std::numeric_limits<std::int32_t>::max();},Status::InsufficientCapacity);
    for(unsigned which=0;which<7;++which)rejection("nonnull used extent required",[&](auto& s){switch(which){case 0:s.candidatePointer=nullptr;break;case 1:s.inputPointer=nullptr;break;case 2:s.workspace.records=nullptr;break;case 3:s.recordPointer=nullptr;break;case 4:s.workspace.effects=nullptr;break;case 5:s.effectPointer=nullptr;break;default:s.workspace.treeStack=nullptr;break;}});
    for(unsigned which=0;which<7;++which)rejection("misaligned typed span",[&](auto& s){auto shift=[](auto* p){using T=std::remove_pointer_t<decltype(p)>;return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+1);};switch(which){case 0:s.candidatePointer=shift(s.candidatePointer);break;case 1:s.inputPointer=shift(s.inputPointer);break;case 2:s.workspace.records=shift(s.workspace.records);break;case 3:s.recordPointer=shift(s.recordPointer);break;case 4:s.workspace.effects=shift(s.workspace.effects);break;case 5:s.effectPointer=shift(s.effectPointer);break;default:s.workspace.treeStack=shift(s.workspace.treeStack);break;}});
    rejection("misaligned nested mesh",[](auto& s){s.candidates[1].collision.geometry.mesh=reinterpret_cast<const CollisionMesh48I*>(reinterpret_cast<std::uintptr_t>(&s.meshValue)+1);});
    constexpr auto maximum=std::numeric_limits<std::size_t>::max();
    for(unsigned which=0;which<7;++which)rejection("overflowed declared extent",[&](auto& s){switch(which){case 0:s.count=maximum;s.workspace.effectCapacity=maximum;s.effectCapacity=maximum;break;case 1:s.recordCount=maximum;s.workspace.recordCapacity=maximum;s.recordCapacity=maximum;break;case 2:s.workspace.recordCapacity=maximum;break;case 3:s.recordCapacity=maximum;break;case 4:s.workspace.effectCapacity=maximum;break;case 5:s.effectCapacity=maximum;break;default:s.workspace.treeStackCapacity=maximum;break;}});
    const auto nearEnd=std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15);
    for(unsigned which=0;which<8;++which)rejection("address wrap extent",[&](auto& s){switch(which){case 0:s.candidatePointer=reinterpret_cast<const SpringStaticCandidate48N*>(nearEnd);break;case 1:s.inputPointer=reinterpret_cast<const SpringRecord48N*>(nearEnd);break;case 2:s.workspace.records=reinterpret_cast<SpringRecord48N*>(nearEnd);break;case 3:s.recordPointer=reinterpret_cast<SpringRecord48N*>(nearEnd);break;case 4:s.workspace.effects=reinterpret_cast<TerrainCandidateEffects48J*>(nearEnd);break;case 5:s.effectPointer=reinterpret_cast<TerrainCandidateEffects48J*>(nearEnd);break;case 6:s.workspace.treeStack=reinterpret_cast<std::uint32_t*>(nearEnd);s.workspace.treeStackCapacity=16;break;default:s.candidates[1].sceneName=reinterpret_cast<const char*>(nearEnd);s.candidates[1].sceneNameBytes=32;break;}});
    // Six writable regions, all 15 pairs. Every source extent, including vector
    // backing and opaque scene-name bytes, is tested against every writable kind.
    for(unsigned i=0;i<6;++i)for(unsigned j=i+1;j<6;++j){label="mutually writable alias "+std::to_string(i)+":"+std::to_string(j);Safety s;s.setWrite(i,s.write(j));s.reject(Status::InvalidInput,true);}
    std::size_t readCount=0;{Safety s;readCount=s.reads().size();}
    for(unsigned w=0;w<6;++w)for(std::size_t r=0;r<readCount;++r){label="nested read/write alias "+std::to_string(w)+":"+std::to_string(r);Safety s;void* p=s.reads()[r];if(w==5&&reinterpret_cast<std::uintptr_t>(p)%alignof(SpringQueryResult48N))continue;s.setWrite(w,p);s.reject(Status::InvalidInput,true);}
    rejection("tail-only record scratch/destination overlap",[](auto& s){s.workspace.records=s.recordDestination.data();s.workspace.recordCapacity=6;s.recordPointer=s.recordDestination.data()+4;s.recordCount=1;s.initial.generatedCapacity88=1;s.recordCapacity=1;});
    rejection("tail-only effects scratch/destination overlap",[](auto& s){s.workspace.effects=s.effectDestination.data();s.workspace.effectCapacity=6;s.effectPointer=s.effectDestination.data()+4;s.count=1;s.effectCapacity=1;});
    rejection("unused destination tail aliases records input",[](auto& s){s.recordPointer=s.input.data();s.recordCount=1;s.recordCapacity=6;s.initial.generatedCapacity88=1;s.inputPointer=s.input.data()+4;});
    for(unsigned which=0;which<4;++which)rejection("zero use still checks declared output span",[&](auto& s){s.count=0;s.recordCount=0;s.initial.generatedCapacity88=0;switch(which){case 0:s.workspace.records=nullptr;break;case 1:s.recordPointer=nullptr;break;case 2:s.workspace.effects=nullptr;break;default:s.effectPointer=nullptr;break;}});
    // Fail after the first candidate has written scratch records and effects.
    rejection("late unknown response callback",[](auto& s){s.candidates[1].collision.hasResponseCallbackE8=true;},Status::UnsupportedScope);
    rejection("late group object",[](auto& s){s.candidates[1].collision.typeTag=s.query.groupTypeTag;},Status::UnsupportedScope);
    rejection("late dynamic spring branch",[](auto& s){s.candidates[1].collision.filter.categoryA4|=s.query.dynamicMask;},Status::UnsupportedScope);
    rejection("late material too-short delimiter",[](auto& s){std::strcpy(s.names[1],"ab");s.candidates[1].sceneNameBytes=3;},Status::UnsupportedScope);
    rejection("late material missing delimiter",[](auto& s){std::memset(s.names[1],'x',sizeof(s.names[1]));s.candidates[1].sceneNameBytes=sizeof(s.names[1]);});
    rejection("late interpolation boundary",[](auto& s){s.candidates[1].collision.geometry.worldMatrix.v[12]=4.25f;s.candidates[1].collision.geometry.worldMatrix.v[14]=4.125f;},Status::UnsupportedInterpolationBoundary);
    rejection("late identity mismatch",[](auto& s){s.candidates[1].collision.geometry.id=99;});
    rejection("late null mesh",[](auto& s){s.candidates[1].collision.geometry.mesh=nullptr;});
    rejection("late nonaffine candidate matrix",[](auto& s){s.candidates[1].collision.geometry.worldMatrix.v[3]=1;});
    rejection("late nonfinite candidate sphere",[](auto& s){s.candidates[1].collision.sphere.radius=std::numeric_limits<float>::quiet_NaN();});
    rejection("query negative sphere",[](auto& s){s.sphereValue.radius=-1;});rejection("query nonfinite segment",[](auto& s){s.segment.first.v[0]=std::numeric_limits<float>::infinity();});rejection("malformed tree cycle",[](auto& s){s.meshValue.nodes[0].children[0]=0;});rejection("malformed leaf index",[](auto& s){s.meshValue.nodes[0].children[1]=collision_mesh_leaf48i|99;});rejection("malformed vertex index",[](auto& s){s.meshValue.faces[1].vertices[0]=99;});rejection("nonfinite vertex",[](auto& s){s.meshValue.vertices[0][0]=std::numeric_limits<float>::quiet_NaN();});
    // A vertex-normal interpolation cut at the extremal vertex has fewer than
    // two initialized original edge slots; it must not become a fabricated hit.
    rejection("unsupported interpolation boundary",[](auto& s){s.segment.first.v[0]=-4;s.segment.second.v[0]=-4;s.segment.first.v[2]=-4;s.segment.second.v[2]=-4;},Status::UnsupportedInterpolationBoundary);
    label="logical saturation remains successful";{Safety s;s.initial.generatedCapacity88=0;check(s.call()==Status::Complete,"zero logical capacity succeeds");check(s.output.appended==0&&s.output.saturated==2&&s.output.callbacks==2,"zero capacity reports saturation");check(std::memcmp(s.recordPointer,s.inputPointer,4*sizeof(SpringRecord48N))==0,"saturation preserves all record bytes");}
    label="count above logical capacity";{Safety s;s.query.resetBefore=false;s.initial.generatedCount84=5;check(s.call()==Status::Complete,"original signed saturation succeeds");check(s.output.state.generatedCount84==5&&s.output.saturated==2,"overfull input count preserved");}
    label="zero storage supports hit saturation";{Safety s;s.recordCount=0;s.recordCapacity=0;s.initial.generatedCapacity88=0;s.inputPointer=nullptr;s.recordPointer=nullptr;s.workspace.records=nullptr;s.workspace.recordCapacity=0;check(s.call()==Status::Complete,"zero physical records succeeds");check(s.output.geometryHits==2&&s.output.saturated==2,"zero storage still executes geometry/callbacks");}
    label="empty list and zero extents";{Safety s;s.count=0;s.recordCount=0;s.recordCapacity=0;s.effectCapacity=0;s.initial.generatedCapacity88=0;s.candidatePointer=nullptr;s.inputPointer=nullptr;s.recordPointer=nullptr;s.effectPointer=nullptr;s.workspace={};check(s.call()==Status::Complete,"empty query succeeds with null arrays");check(!s.output.hasGeometryHit&&s.output.candidateCount==0&&s.output.state.pending38==1&&s.output.state.generatedCount84==0,"empty query commits original reset");}
    label="empty query preserves previous query current";{Safety s;s.query.initialQueryCurrentIdentity=0x12345678;s.count=0;s.candidatePointer=nullptr;check(s.call()==Status::Complete,"empty query with prior identity completes");check(s.output.currentCandidateIdentity==0x12345678&&s.output.state.currentCandidate130==0,"query current preserved independently of reset spring current");}
    label="skipped dynamic callback";{Safety s;for(auto& c:s.candidates)c.collision.filter.categoryA4|=s.query.dynamicMask|s.query.skipMask;check(s.call()==Status::Complete,"skip precedes dynamic branch");check(s.output.callbacks==2&&s.output.appended==0&&s.output.saturated==0,"skipped callbacks still counted");}
    label="callbacks disabled";{Safety s;s.query.callbackEnabled=false;for(auto& c:s.candidates)c.collision.filter.categoryA4|=s.query.dynamicMask;check(s.call()==Status::Complete,"disabled callback avoids dynamic scope");check(s.output.geometryHits==2&&s.output.callbacks==0&&s.output.appended==0,"geometry hit without callback");}
    label="group bypass flag";{Safety s;s.queryFilter.flagsF8|=1;s.candidates[1].collision.typeTag=s.query.groupTypeTag;check(s.call()==Status::Complete,"query bit0 suppresses group recursion");check(s.output.geometryHits==1,"group entry ignored");}
    label="unreached callback and scene semantics";{Safety s;s.candidates[1].collision.sphere.center.v[0]=1000;s.candidates[1].collision.hasResponseCallbackE8=true;std::strcpy(s.names[1],"ab");s.candidates[1].sceneNameBytes=3;check(s.call()==Status::Complete,"broad phase miss does not invoke unsupported callback");check(s.output.geometryHits==1&&s.output.currentCandidateIdentity==21,"miss still enumerates collision candidate");}
}
void benchmark(){
    label="serial bounded query benchmark";Safety s;constexpr std::size_t loops=12000;std::array<double,7> samples{};std::size_t completed=0,appended=0;const auto before=observedAllocations;
    for(auto& ns:samples){const auto start=std::chrono::steady_clock::now();watchAllocations=true;for(std::size_t i=0;i<loops;++i){completed+=queryStaticSpringBounded48N(s.query,s.candidatePointer,s.count,s.initial,s.inputPointer,s.recordCount,s.workspace,s.recordPointer,s.recordCapacity,s.effectPointer,s.effectCapacity,s.output)==Status::Complete;appended+=s.output.appended;}watchAllocations=false;const auto end=std::chrono::steady_clock::now();ns=std::chrono::duration<double,std::nano>(end-start).count()/loops;}
    std::sort(samples.begin(),samples.end());check(completed==loops*samples.size()&&appended==2*completed,"serial repeated calls commit complete results");check(observedAllocations==before,"zero allocation across serial reuse");
    std::cout<<"spring_query_cost_host_ns median="<<samples[samples.size()/2]<<" max_batch_mean="<<samples.back()<<" serial_queries="<<completed<<" candidate_count="<<s.count<<" record_count="<<s.recordCount<<" record_scratch_bytes="<<s.workspace.recordCapacity*sizeof(SpringRecord48N)<<" effect_scratch_bytes="<<s.workspace.effectCapacity*sizeof(TerrainCandidateEffects48J)<<" tree_stack_bytes="<<s.workspace.treeStackCapacity*sizeof(std::uint32_t)<<" result_bytes="<<sizeof(SpringQueryResult48N)<<" workspace_descriptor_bytes="<<sizeof(SpringQueryWorkspace48N)<<" view_bytes="<<sizeof(SpringQueryView48N)<<" record_bytes="<<sizeof(SpringRecord48N)<<" effect_bytes="<<sizeof(TerrainCandidateEffects48J)<<" runtime_allocations="<<(observedAllocations-before)<<" (serial host component observation; no Xbox/gameplay FPS claim)\n";
}
}
int main(int argc,char** argv){try{if(argc!=2)return 2;std::fesetround(FE_TONEAREST);std::ifstream f(std::filesystem::path(argv[1])/"evidence/springQuery48N/native-fixtures.json");if(!f)throw std::runtime_error("missing springQuery48N/native-fixtures.json");Json fixture;f>>fixture;if(fixture.at("schema")!=1||fixture.at("cases").size()!=132||fixture.at("meshes").size()!=4)throw std::runtime_error("incomplete spring query fixture");std::vector<CollisionMesh48I> meshes;for(const auto& m:fixture.at("meshes"))meshes.push_back(mesh(m));std::size_t stack=0;for(const auto& m:meshes)stack=std::max(stack,stackBound(m));std::vector<Snapshot> meshSnapshots;for(const auto& m:meshes)captureMesh(meshSnapshots,m);for(const auto& j:fixture.at("cases"))differential(j,meshes,stack);for(const auto& s:meshSnapshots)s.verify();safety();benchmark();check(observedAllocations==0,"all guarded runtime calls allocation-free");std::cout<<"spring_query48N cases="<<differentialCases<<" checks="<<checks<<" failures="<<failures<<" queries_with_geometry_hits="<<totalHits<<" candidate_geometry_hits="<<totalGeometryHits<<" callbacks="<<totalCallbacks<<" appended="<<totalAppended<<" saturated="<<totalSaturated<<" safety_cases="<<safetyCases<<" alias_cases="<<aliasCases<<" fixture_max_tree_stack_entries="<<maxStack<<" fixture_max_records="<<maxRecords<<" fixture_max_candidates="<<maxCandidates<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;}catch(const std::exception& e){watchAllocations=false;std::cerr<<label<<": "<<e.what()<<'\n';return 2;}}
