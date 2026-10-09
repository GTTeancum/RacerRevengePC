// Test-only original219420 projections, caller-span guards and serial host costs.
#include "vehicle_contact48r.h"
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
#else
#define RR_NOINLINE __attribute__((noinline))
#endif
// Watch only public API execution. Parsing, storage and diagnostics are outside.
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
using Json=nlohmann::json;
namespace {
using Status=VehicleContactStatus48R;
std::size_t checks=0,failures=0,differentialCases=0,boundaryCases=0,guardDivergences=0,
    modelSpecificCases=0,safetyCases=0,aliasCases=0,wiringCases=0;
std::size_t originalCallbacks=0,originalAppends=0,maxScenes=0,maxBodies=0,maxCandidates=0,maxRecords=0;
std::string label;
void check(bool yes,const char* why){++checks;if(!yes){++failures;if(failures<50)std::cerr<<label<<": "<<why<<'\n';}}
std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
std::int32_t signedWord(std::uint32_t b){std::int32_t n;std::memcpy(&n,&b,4);return n;}
template<class T>void sentinel(T* p,std::size_t n,unsigned char value){static_assert(std::is_trivially_copyable_v<T>);std::fill_n(reinterpret_cast<unsigned char*>(p),n*sizeof(T),value);}
template<class T>void rawFloats(const Json& j,T& target){if(j.size()!=std::size(target))throw std::runtime_error("raw vector width");for(std::size_t i=0;i<j.size();++i)target[i]=fp(j.at(i).template get<std::uint32_t>());}
SpringRecordState48N state(const Json& j){if(j.size()!=11)throw std::runtime_error("spring state width");return {j[0],j[1],j[2],j[3],j[4],j[5],j[6],j[7],signedWord(j[8]),signedWord(j[9]),j[10]};}
std::array<std::uint32_t,11> stateWords(const SpringRecordState48N& s){return {s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
SpringRecord48N record(const Json& j){if(j.size()!=24)throw std::runtime_error("record width");std::array<std::uint32_t,24> w{};for(unsigned i=0;i<24;++i)w[i]=j[i];SpringRecord48N r{};std::memcpy(&r,w.data(),sizeof(r));return r;}
struct Snapshot {
    const void* p;std::vector<unsigned char> bytes;
    Snapshot(const void* q,std::size_t n):p(q),bytes(n){if(n)std::memcpy(bytes.data(),p,n);}
    void verify()const{check(bytes.empty()||!std::memcmp(p,bytes.data(),bytes.size()),"immutable/transactional complete byte span");}
};
void capture(std::vector<Snapshot>& s,const void* p,std::size_t n){if(n)s.emplace_back(p,n);}
template<class T>void captureVector(std::vector<Snapshot>& s,const std::vector<T>& v){capture(s,v.data(),v.size()*sizeof(T));}
template<class T>struct Guarded {
    std::vector<T> v;std::size_t used,capacity;
    explicit Guarded(std::size_t n,unsigned char value):v(n+4),used(n),capacity(n+2){sentinel(v.data(),v.size(),value);}
    T* data(){return v.data()+1;}
    const T* data()const{return v.data()+1;}
    void outer(std::vector<Snapshot>& s)const{capture(s,v.data(),sizeof(T));capture(s,data()+capacity,sizeof(T));}
    void tail(std::vector<Snapshot>& s)const{capture(s,data()+used,(capacity-used)*sizeof(T));}
};
struct Projection {
    Guarded<VehicleContactScene48R> scenes;
    Guarded<VehicleContactBody48R> bodies;
    Guarded<SpringRecord48N> records;
    Projection(std::size_t s,std::size_t b,std::size_t r,unsigned char value):scenes(s,value),bodies(b,value),records(r,value){}
    VehicleContactBuffers48R buffers(){return {scenes.data(),scenes.capacity,bodies.data(),bodies.capacity,records.data(),records.capacity};}
    void all(std::vector<Snapshot>& s)const{captureVector(s,scenes.v);captureVector(s,bodies.v);captureVector(s,records.v);}
    void guards(std::vector<Snapshot>& s,bool tails)const{scenes.outer(s);bodies.outer(s);records.outer(s);if(tails){scenes.tail(s);bodies.tail(s);records.tail(s);}}
    std::size_t bytes()const{return scenes.capacity*sizeof(VehicleContactScene48R)+bodies.capacity*sizeof(VehicleContactBody48R)+records.capacity*sizeof(SpringRecord48N);}
};
struct Fixture {
    std::vector<VehicleContactScene48R> scenes;
    std::vector<VehicleContactBody48R> bodies;
    std::vector<VehicleContactCandidate48R> candidates;
    std::vector<std::uint32_t> order;
    std::vector<SpringRecord48N> records;
    VehicleContactInput48R input{};
    VehicleContactSource48R source{};
    Projection temporary,final;
    VehicleContactBuffers48R scratch{},destination{};
    VehicleContactResult48R result{};VehicleContactResult48R* resultPointer=&result;
    explicit Fixture(const Json& j):temporary(j.at("scenes").size(),j.at("bodies").size(),j.at("initial_records").size(),0x3c),final(j.at("scenes").size(),j.at("bodies").size(),j.at("initial_records").size(),0x5a){
        for(const auto& v:j.at("scenes")){VehicleContactScene48R s{};rawFloats(v.at("world"),s.world.v);rawFloats(v.at("local"),s.local.v);s.flags=v.at("flags");s.parentPresent=v.at("parent")!=0;scenes.push_back(s);}
        for(const auto& v:j.at("bodies")){VehicleContactBody48R b{};b.identity.value=v.at("id");b.callback=v.at("callback")==0x165430?VehicleContactCallback48R::RigidBody165430:VehicleContactCallback48R::Unsupported;b.contacts.count20C=signedWord(v.at("count"));for(unsigned i=0;i<10;++i)b.contacts.slots210[i].value=v.at("contacts")[i];bodies.push_back(b);}
        for(const auto& v:j.at("candidates")){VehicleContactCandidate48R c{};c.identity=v.at("id");c.shapeTag=v.at("shape_tag");rawFloats(v.at("shape_world"),c.shapeWorld.v);rawFloats(v.at("local_center"),c.localCenter.v);rawFloats(v.at("local_half"),c.localHalf.v);rawFloats(v.at("cached_center"),c.cachedCenter.v);c.cachedRadius=fp(v.at("cached_radius"));c.category=v.at("category");c.bodyIndex=bodyIndex(v.at("body"));candidates.push_back(c);}
        order=j.at("order").get<decltype(order)>();for(const auto& v:j.at("initial_records"))records.push_back(record(v));
        input.candidateCount=signedWord(j.at("count"));input.positionSceneIndex=j.at("position_scene");input.localSceneIndex=j.at("local_scene");input.primaryBodyIndex=bodyIndex(1);input.categoryMask=j.at("category_mask");input.vehicleRadius=fp(j.at("radius"));input.spring=state(j.at("spring_state"));
        for(unsigned i=0;i<2;++i){input.statics.flags[i]=j.at("static_flags")[i];input.statics.words[i]=j.at("static_words")[i];}
        source={scenes.data(),scenes.size(),bodies.data(),bodies.size(),candidates.data(),candidates.size(),order.data(),order.size(),records.data(),records.size()};scratch=temporary.buffers();destination=final.buffers();sentinel(&result,1,0xa5);
    }
    std::uint32_t bodyIndex(std::uint32_t id)const{for(std::size_t i=0;i<bodies.size();++i)if(bodies[i].identity.value==id)return static_cast<std::uint32_t>(i);return std::numeric_limits<std::uint32_t>::max();}
    Status call(){watchAllocations=true;const auto r=runVehicleContacts48R(input,source,scratch,destination,*resultPointer);watchAllocations=false;return r;}
    void immutable(std::vector<Snapshot>& s)const{captureVector(s,scenes);captureVector(s,bodies);captureVector(s,candidates);captureVector(s,order);captureVector(s,records);capture(s,&input,sizeof(input));capture(s,&source,sizeof(source));capture(s,&scratch,sizeof(scratch));capture(s,&destination,sizeof(destination));}
    std::vector<Snapshot> saved(bool failure)const{std::vector<Snapshot> s;immutable(s);temporary.guards(s,false);if(failure){final.all(s);capture(s,&result,sizeof(result));}else final.guards(s,true);return s;}
    void reject(Status wanted=Status::InvalidInput,bool alias=false){auto keep=saved(true);const auto before=observedAllocations;const auto got=call();check(got==wanted,"expected transactional rejection status");if(got!=wanted&&failures<50)std::cerr<<label<<": status="<<int(got)<<" expected="<<int(wanted)<<'\n';for(const auto& s:keep)s.verify();check(observedAllocations==before,"rejection allocation-free");++safetyCases;aliasCases+=alias;}
    void* write(unsigned w){auto& b=w<3?scratch:destination;switch(w%3){case 0:return w==6?static_cast<void*>(resultPointer):static_cast<void*>(b.scenes);case 1:return b.bodies;default:return b.records;}}
    void setWrite(unsigned w,void* p){if(w==6){resultPointer=static_cast<VehicleContactResult48R*>(p);return;}auto& b=w<3?scratch:destination;switch(w%3){case 0:b.scenes=static_cast<VehicleContactScene48R*>(p);break;case 1:b.bodies=static_cast<VehicleContactBody48R*>(p);break;default:b.records=static_cast<SpringRecord48N*>(p);}}
    void capacity(unsigned w,std::size_t n){auto& b=w<3?scratch:destination;switch(w%3){case 0:b.sceneCapacity=n;break;case 1:b.bodyCapacity=n;break;default:b.recordCapacity=n;}}
    std::size_t writeBytes(unsigned w)const{if(w==6)return sizeof(result);const auto& b=w<3?scratch:destination;switch(w%3){case 0:return b.sceneCapacity*sizeof(VehicleContactScene48R);case 1:return b.bodyCapacity*sizeof(VehicleContactBody48R);default:return b.recordCapacity*sizeof(SpringRecord48N);}}
    const void* read(unsigned r)const{switch(r){case 0:return &input;case 1:return &source;case 2:return &scratch;case 3:return &destination;case 4:return source.scenes;case 5:return source.bodies;case 6:return source.candidates;case 7:return source.candidateOrder;default:return source.records;}}
    std::size_t readBytes(unsigned r)const{switch(r){case 0:return sizeof(input);case 1:return sizeof(source);case 2:return sizeof(scratch);case 3:return sizeof(destination);case 4:return source.sceneCount*sizeof(*source.scenes);case 5:return source.bodyCount*sizeof(*source.bodies);case 6:return source.candidateCount*sizeof(*source.candidates);case 7:return source.candidateOrderCount*sizeof(*source.candidateOrder);default:return source.recordCount*sizeof(*source.records);}}
    void setRead(unsigned r,const void* p){switch(r){case 0:source.scenes=static_cast<const VehicleContactScene48R*>(p);break;case 1:source.bodies=static_cast<const VehicleContactBody48R*>(p);break;case 2:source.candidates=static_cast<const VehicleContactCandidate48R*>(p);break;case 3:source.candidateOrder=static_cast<const std::uint32_t*>(p);break;default:source.records=static_cast<const SpringRecord48N*>(p);}}
    void readCount(unsigned r,std::size_t n){switch(r){case 0:source.sceneCount=n;break;case 1:source.bodyCount=n;break;case 2:source.candidateCount=n;break;case 3:source.candidateOrderCount=n;break;default:source.recordCount=n;}}
};
void compare(const Fixture& f,const Json& j){
    const auto& e=j.at("expected");const auto actual=stateWords(f.result.spring);for(unsigned i=0;i<11;++i)check(actual[i]==e.at("spring_state")[i].get<std::uint32_t>(),"all eleven spring state words");
    for(unsigned i=0;i<2;++i){check(f.result.statics.flags[i]==e.at("static_flags")[i].get<std::uint8_t>(),"static initialization flag");check(f.result.statics.words[i]==e.at("static_words")[i].get<std::uint32_t>(),"raw static word");}
    for(std::size_t i=0;i<f.scenes.size();++i){const auto& a=f.final.scenes.data()[i];const auto& b=e.at("scenes")[i];for(unsigned k=0;k<16;++k){check(bits(a.world.v[k])==b.at("world")[k].get<std::uint32_t>(),"full scene world matrix");check(bits(a.local.v[k])==b.at("local")[k].get<std::uint32_t>(),"full scene local matrix");}check(a.flags==b.at("flags").get<std::uint32_t>(),"scene flags");check(a.parentPresent==(b.at("parent")!=0),"scene parent presence");}
    for(std::size_t i=0;i<f.bodies.size();++i){const auto& a=f.final.bodies.data()[i];const auto& b=e.at("bodies")[i];check(a.identity.value==b.at("id").get<std::uint32_t>(),"body identity preserved");check(a.callback==f.bodies[i].callback,"body callback tag preserved");check(static_cast<std::uint32_t>(a.contacts.count20C)==b.at("count").get<std::uint32_t>(),"body signed count");for(unsigned k=0;k<10;++k)check(a.contacts.slots210[k].value==b.at("contacts")[k].get<std::uint32_t>(),"all ten body slots including stale tail");}
    for(std::size_t i=0;i<f.records.size();++i){const auto expected=record(e.at("records")[i]);const auto* a=reinterpret_cast<const unsigned char*>(f.final.records.data()+i);const auto* b=reinterpret_cast<const unsigned char*>(&expected);for(unsigned k=0;k<96;++k)check(a[k]==b[k],"all96 record bytes and all stale record slots");}
}
Status classification(const Json& j){
    const auto& e=j.at("expected");if(!e.at("boundary").is_null()){
        const auto boundary=e.at("boundary").get<std::string>();
        if(boundary=="unsupported-bounds-tag")return Status::UnsupportedBounds;
        if(boundary.find("dirty-parent")!=std::string::npos)return Status::UnsupportedSceneParent;
        if(boundary=="primary-virtual"||boundary=="other-virtual")return Status::UnsupportedBodyCallback;
        if(boundary=="negative-body-contact-count"||boundary=="negative-record-count")return Status::InvalidInput;
        throw std::runtime_error("unclassified original boundary: "+boundary);
    }
    if(!e.at("append_calls").empty()&&(signedWord(j.at("spring_state")[8])<0||signedWord(j.at("spring_state")[9])<0))return Status::InvalidInput;
    const auto scope=j.value("numeric_scope",std::string("ordinary-finite"));
    if(scope=="native-invalid-nonfinite"||scope=="outside-finite-domain")return Status::InvalidInput;
    if(scope!="ordinary-finite"&&scope!="zero-normalization-finite-model"&&scope!="nonfinite-copy-only")throw std::runtime_error("unclassified numeric scope: "+scope);
    return Status::Complete;
}
void differential(const Json& j){
    label=j.at("name").get<std::string>();Fixture f(j);const auto wanted=classification(j);
    maxScenes=std::max(maxScenes,f.scenes.size());maxBodies=std::max(maxBodies,f.bodies.size());maxCandidates=std::max(maxCandidates,f.candidates.size());maxRecords=std::max(maxRecords,f.records.size());
    if(wanted!=Status::Complete){f.reject(wanted);if(j.at("expected").at("boundary").is_null())++guardDivergences;else ++boundaryCases;return;}
    const auto keep=f.saved(false);check(f.call()==Status::Complete,"original completed fixture");compare(f,j);for(const auto& s:keep)s.verify();++differentialCases;
    modelSpecificCases+=j.value("numeric_scope",std::string())=="zero-normalization-finite-model";
    originalCallbacks+=j.at("expected").at("callback_calls").size();originalAppends+=j.at("expected").at("append_calls").size();
}
template<class F>void rejectCase(const Json& base,const std::string& name,F mutate,Status wanted=Status::InvalidInput){label=name;Fixture f(base);mutate(f);f.reject(wanted);}
void safety(const Json& base){
    for(unsigned w=0;w<6;++w){
        rejectCase(base,"null writable "+std::to_string(w),[&](auto& f){f.setWrite(w,nullptr);});
        rejectCase(base,"misaligned writable "+std::to_string(w),[&](auto& f){f.setWrite(w,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(f.write(w))+1));});
        rejectCase(base,"overflow writable "+std::to_string(w),[&](auto& f){f.capacity(w,std::numeric_limits<std::size_t>::max());});
        rejectCase(base,"wrapping writable "+std::to_string(w),[&](auto& f){f.setWrite(w,reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(3)));});
        rejectCase(base,"short writable "+std::to_string(w),[&](auto& f){f.capacity(w,(w%3==0?f.scenes.size():w%3==1?f.bodies.size():f.records.size())-1);},Status::InsufficientCapacity);
    }
    for(unsigned a=0;a<7;++a)for(unsigned b=a+1;b<7;++b)for(unsigned tail=0;tail<2;++tail){label="writable alias "+std::to_string(a)+":"+std::to_string(b)+" tail="+std::to_string(tail);Fixture f(base);auto address=reinterpret_cast<std::uintptr_t>(f.write(b));if(tail)address+=f.writeBytes(b)-4;f.setWrite(a,reinterpret_cast<void*>(address));f.reject(Status::InvalidInput,true);}
    for(unsigned w=0;w<7;++w)for(unsigned r=0;r<9;++r)for(unsigned tail=0;tail<2;++tail){label="source or descriptor alias "+std::to_string(w)+":"+std::to_string(r)+" tail="+std::to_string(tail);Fixture f(base);auto address=reinterpret_cast<std::uintptr_t>(f.read(r));if(tail)address+=f.readBytes(r)-4;f.setWrite(w,reinterpret_cast<void*>(address));f.reject(Status::InvalidInput,true);}
    for(unsigned r=0;r<5;++r){
        rejectCase(base,"null source "+std::to_string(r),[&](auto& f){f.setRead(r,nullptr);});
        rejectCase(base,"misaligned source "+std::to_string(r),[&](auto& f){f.setRead(r,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(f.read(r+4))+1));});
        rejectCase(base,"wrapping source "+std::to_string(r),[&](auto& f){f.setRead(r,reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(3)));});
        rejectCase(base,"overflow source "+std::to_string(r),[&](auto& f){f.readCount(r,std::numeric_limits<std::size_t>::max());if(r==0||r==1||r==4){const auto w=r==4?2:r;f.capacity(w,std::numeric_limits<std::size_t>::max());f.capacity(w+3,std::numeric_limits<std::size_t>::max());}});
    }
    rejectCase(base,"position scene bad index",[](auto& f){f.input.positionSceneIndex=99;});
    rejectCase(base,"local scene bad late index",[](auto& f){f.input.localSceneIndex=99;});
    rejectCase(base,"primary body bad late index",[](auto& f){f.input.primaryBodyIndex=99;});
    rejectCase(base,"candidate body bad late index",[](auto& f){f.candidates[0].bodyIndex=99;});
    rejectCase(base,"candidate order bad late index",[](auto& f){f.order[1]=99;});
    rejectCase(base,"candidate active count exceeds order",[](auto& f){f.input.candidateCount=3;},Status::InsufficientCapacity);
    rejectCase(base,"duplicate body identity labels",[](auto& f){f.bodies[2].identity=f.bodies[0].identity;});
    rejectCase(base,"late bounds after first hit",[](auto& f){f.scenes[0].flags=0;f.candidates[1].shapeTag=3;},Status::UnsupportedBounds);
    rejectCase(base,"late other callback after first hit",[](auto& f){f.scenes[0].flags=0;f.bodies[2].callback=VehicleContactCallback48R::Unsupported;},Status::UnsupportedBodyCallback);
    rejectCase(base,"late other body count after first hit",[](auto& f){f.bodies[2].contacts.count20C=-1;});
    rejectCase(base,"primary unknown callback after scene prepare",[](auto& f){f.scenes[0].flags=0;f.bodies[0].callback=VehicleContactCallback48R::Unsupported;},Status::UnsupportedBodyCallback);
    rejectCase(base,"dirty parent position",[](auto& f){f.scenes[0].flags=0;f.scenes[0].parentPresent=true;},Status::UnsupportedSceneParent);
    rejectCase(base,"dirty parent local after numeric work",[](auto& f){f.scenes[0].flags=0;f.scenes[1].flags=0;f.scenes[1].parentPresent=true;},Status::UnsupportedSceneParent);
    rejectCase(base,"negative generated count reached",[](auto& f){f.input.spring.generatedCount84=-1;});
    rejectCase(base,"negative generated capacity reached",[](auto& f){f.input.spring.generatedCapacity88=-1;});
    rejectCase(base,"full record capacity requires all backing",[](auto& f){f.input.spring.generatedCount84=13;f.input.spring.generatedCapacity88=13;},Status::InsufficientCapacity);
    rejectCase(base,"record full backing independent of destination spare",[](auto& f){f.input.spring.generatedCapacity88=13;},Status::InsufficientCapacity);
    for(auto raw:std::array<std::uint32_t,5>{{0x7f800000,0xff800000,0x7fc12345,0x7f812345,0xffc12345}}){
        for(unsigned field=0;field<8;++field)rejectCase(base,"nonfinite reached numeric "+std::to_string(field)+":"+std::to_string(raw),[&](auto& f){const auto value=fp(raw);switch(field){case 0:f.input.vehicleRadius=value;break;case 1:f.candidates[1].cachedCenter.v[0]=value;break;case 2:f.candidates[1].localHalf.v[0]=value;break;case 3:f.candidates[1].localCenter.v[0]=value;break;case 4:f.candidates[1].shapeWorld.v[0]=value;break;case 5:f.scenes[0].world.v[12]=value;break;case 6:f.scenes[1].world.v[0]=value;break;default:f.input.statics.words[0]=raw;}});
    }
    rejectCase(base,"bounds finite overflow late",[](auto& f){f.candidates[1].localHalf.v[0]=std::numeric_limits<float>::max();f.candidates[1].shapeWorld.v[0]=2;});
    rejectCase(base,"distance finite square overflow",[](auto& f){f.candidates[1].cachedCenter.v[0]=std::numeric_limits<float>::max();});
    rejectCase(base,"threshold finite overflow",[](auto& f){f.input.vehicleRadius=std::numeric_limits<float>::max();f.input.statics.words[0]=0x7f7fffff;});
    rejectCase(base,"inverse point finite overflow",[](auto& f){f.scenes[1].world.v[0]=std::numeric_limits<float>::max();f.scenes[1].world.v[12]=2;});
}
template<class F,class V>void success(const Json& base,const std::string& name,F mutate,V verify){label=name;Fixture f(base);mutate(f);const auto keep=f.saved(false);check(f.call()==Status::Complete,"synthetic gate complete");verify(f);for(const auto& s:keep)s.verify();++wiringCases;}
void wiring(const Json& base){
    success(base,"exact caller capacities",[](auto& f){for(unsigned w=0;w<6;++w)f.capacity(w,w%3==0?f.scenes.size():w%3==1?f.bodies.size():f.records.size());},[](const auto& f){check(f.result.spring.generatedCount84==2,"two ordered appends");});
    success(base,"both callbacks before saturated append",[](auto& f){f.input.spring.generatedCount84=12;},[](const auto& f){check(f.result.spring.generatedCount84==12,"saturated count stays12");check(f.final.bodies.data()[0].contacts.count20C==2&&f.final.bodies.data()[1].contacts.count20C==1&&f.final.bodies.data()[2].contacts.count20C==1,"both body callbacks for each saturated candidate");check(!std::memcmp(f.final.records.data(),f.records.data(),f.records.size()*sizeof(SpringRecord48N)),"saturation preserves all96-byte slots");});
    success(base,"no body count reset",[](auto& f){f.bodies[0].contacts.count20C=9;},[](const auto& f){check(f.final.bodies.data()[0].contacts.count20C==10,"one slot filled then full");check(f.final.bodies.data()[0].contacts.slots210[0].value==f.bodies[0].contacts.slots210[0].value,"old body list retained");check(f.final.bodies.data()[0].contacts.slots210[9].value==2,"ordered candidate owns final slot");});
    success(base,"no spring count reset",[](auto& f){f.input.spring.generatedCount84=3;},[](const auto& f){check(f.result.spring.generatedCount84==5,"append starts at caller count");check(!std::memcmp(f.final.records.data(),f.records.data(),3*sizeof(SpringRecord48N)),"prefix records unchanged");});
    success(base,"self callbacks coalesce one state",[](auto& f){f.candidates[0].bodyIndex=0;f.candidates[1].bodyIndex=0;},[](const auto& f){check(f.final.bodies.data()[0].contacts.count20C==1&&f.final.bodies.data()[0].contacts.slots210[0].value==1,"self append then duplicates on same object");check(f.result.spring.generatedCount84==2,"self does not suppress spring records");});
    rejectCase(base,"reached zero dynamic body identity",[](auto& f){f.bodies[1].identity.value=0;});
    rejectCase(base,"reached zero primary body identity",[](auto& f){f.bodies[0].identity.value=0;});
    success(base,"category-skipped zero identity remains lazy",[](auto& f){f.bodies[1].identity.value=0;f.candidates[0].category=0;},[](const auto& f){check(f.result.spring.generatedCount84==1,"only reached nonzero body appends");check(f.final.bodies.data()[1].identity.value==0&&f.final.bodies.data()[1].contacts.count20C==0,"skipped zero body retained");});
    success(base,"missed zero primary identity remains lazy",[](auto& f){f.bodies[0].identity.value=0;for(auto& c:f.candidates)c.cachedCenter.v[0]=100;},[](const auto& f){check(f.result.spring.generatedCount84==0&&f.final.bodies.data()[0].identity.value==0,"miss does not consume body identity");});
    for(auto n:std::array<std::int32_t,3>{{0,-1,std::numeric_limits<std::int32_t>::min()}})success(base,"signed inactive candidate count "+std::to_string(n),[&](auto& f){f.input.candidateCount=n;f.input.localSceneIndex=99;f.input.primaryBodyIndex=99;f.input.vehicleRadius=fp(0x7fc12345);f.input.spring.generatedCount84=-1;f.input.spring.generatedCapacity88=-1;f.input.statics.flags[0]=f.input.statics.flags[1]=0;f.scenes[0].flags=0x80;f.scenes[0].local.v[12]=3;for(auto& c:f.candidates){c.shapeTag=99;c.bodyIndex=99;}},[](const auto& f){check(f.final.scenes.data()[0].flags==0x81&&f.final.scenes.data()[0].world.v[12]==3,"scene prologue occurs before count gate");check(f.result.statics.flags[0]==1&&f.result.statics.words[0]==0x3f000000&&f.result.statics.words[1]==0x3f800000,"both statics initialize before count gate");check(!std::memcmp(f.final.records.data(),f.records.data(),f.records.size()*sizeof(SpringRecord48N)),"inactive count keeps all records");});
    success(base,"category skips all lazy poison",[](auto& f){for(auto& c:f.candidates){c.category=0;c.shapeTag=99;c.bodyIndex=99;c.localHalf.v[0]=fp(0x7f800000);c.cachedCenter.v[0]=fp(0x7fc12345);}f.input.localSceneIndex=99;f.input.primaryBodyIndex=99;f.input.vehicleRadius=fp(0x7fc12345);f.input.spring.generatedCount84=f.input.spring.generatedCapacity88=-1;f.input.statics.words[0]=0x7fc12345;},[](const auto& f){check(f.result.spring.generatedCount84==-1,"unreached malformed spring ignored");});
    success(base,"strict miss skips callback and local scene poison",[](auto& f){for(auto& c:f.candidates)c.cachedCenter.v[0]=100;f.input.localSceneIndex=99;f.input.primaryBodyIndex=99;f.bodies[0].callback=VehicleContactCallback48R::Unsupported;f.input.spring.generatedCount84=f.input.spring.generatedCapacity88=-1;},[](const auto& f){check(f.result.spring.generatedCount84==-1,"miss keeps malformed unreached spring");});
    success(base,"opaque W and candidate cached radius never consumed",[](auto& f){for(auto& c:f.candidates){c.cachedRadius=fp(0x7f812345);c.cachedCenter.v[3]=c.localCenter.v[3]=c.localHalf.v[3]=fp(0x7fc12345);for(unsigned k:{3u,7u,11u,15u})c.shapeWorld.v[k]=fp(0x7f800000);}for(auto& s:f.scenes)for(unsigned k:{3u,7u,11u,15u})s.world.v[k]=s.local.v[k]=fp(0x7f812345);f.input.statics.words[1]=0x7f812345;},[](const auto& f){check(f.result.spring.generatedCount84==2,"opaque nonfinite words do not reject");check(f.result.statics.words[1]==0x7f812345,"second static is opaque");});
    success(base,"prepared caches ignore parents",[](auto& f){for(auto& s:f.scenes)s.parentPresent=true;},[](const auto& f){check(f.result.spring.generatedCount84==2,"prepared cache parent pointers not followed");});
    success(base,"dirty shared position and local scene coalesce",[](auto& f){f.input.localSceneIndex=f.input.positionSceneIndex;f.scenes[0].flags=0x80;f.scenes[0].local.v[12]=.25f;},[](const auto& f){check(f.final.scenes.data()[0].flags==0x81&&f.final.scenes.data()[0].world.v[12]==.25f,"single shared cache update preserved");check(f.final.records.data()[0].field00.v[0]==0,"local center uses refreshed same scene");});
    success(base,"source tail garbage preserved lazily",[](auto& f){f.scenes[2].flags=0;f.scenes[2].parentPresent=true;f.scenes[2].world.v[0]=fp(0x7f812345);f.candidates[3].shapeTag=99;f.candidates[3].bodyIndex=99;},[](const auto& f){check(!std::memcmp(f.final.scenes.data()+2,f.scenes.data()+2,sizeof(VehicleContactScene48R)),"inactive scene full projection");});
    success(base,"overfull signed metadata remains saturated",[](auto& f){f.input.spring.generatedCount84=std::numeric_limits<std::int32_t>::max();for(auto& b:f.bodies)b.contacts.count20C=std::numeric_limits<std::int32_t>::max();},[](const auto& f){check(f.result.spring.generatedCount84==std::numeric_limits<std::int32_t>::max(),"overfull spring does not reset");for(std::size_t i=0;i<f.bodies.size();++i)check(f.final.bodies.data()[i].contacts.count20C==std::numeric_limits<std::int32_t>::max(),"overfull body callback preserves count");});
    success(base,"zero-sized null arrays allowed when inactive",[](auto& f){f.input.candidateCount=0;f.source.bodies=nullptr;f.source.bodyCount=0;f.source.candidates=nullptr;f.source.candidateCount=0;f.source.candidateOrder=nullptr;f.source.candidateOrderCount=0;f.source.records=nullptr;f.source.recordCount=0;f.scratch.bodies=f.destination.bodies=nullptr;f.scratch.bodyCapacity=f.destination.bodyCapacity=0;f.scratch.records=f.destination.records=nullptr;f.scratch.recordCapacity=f.destination.recordCapacity=0;},[](const auto& f){check(f.result.spring.generatedCount84==0,"inactive zero spans do not inspect stale logical capacity");});
    success(base,"read-only spans may alias",[](auto& f){f.source.candidateOrder=reinterpret_cast<const std::uint32_t*>(&f.bodies[0].contacts.count20C);f.source.candidateOrderCount=1;f.input.candidateCount=1;},[](const auto& f){check(f.result.spring.generatedCount84==1,"read/read alias is safe");});
}
void benchmark(const Json& base){
    label="bounded serial host cost";Fixture f(base);constexpr std::size_t loops=2500;std::array<double,7> ns{};std::size_t complete=0,appended=0;const auto before=observedAllocations;
    for(auto& sample:ns){const auto start=std::chrono::steady_clock::now();watchAllocations=true;for(std::size_t i=0;i<loops;++i){complete+=runVehicleContacts48R(f.input,f.source,f.scratch,f.destination,f.result)==Status::Complete;appended+=static_cast<std::size_t>(f.result.spring.generatedCount84);}watchAllocations=false;sample=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/loops;}
    std::sort(ns.begin(),ns.end());check(complete==loops*ns.size()&&appended==2*complete,"serial reuse deterministic");check(observedAllocations==before,"serial reuse allocation-free");
    std::cout<<"vehicle_contact48r_cost_host_ns median="<<ns[3]<<" max_batch_mean="<<ns.back()<<" serial_calls="<<complete<<" active_candidates="<<f.input.candidateCount<<" source_scenes="<<f.scenes.size()<<" source_bodies="<<f.bodies.size()<<" source_records="<<f.records.size()<<" scratch_backing_bytes="<<f.temporary.bytes()<<" destination_backing_bytes="<<f.final.bytes()<<" result_bytes="<<sizeof(f.result)<<" input_bytes="<<sizeof(f.input)<<" source_descriptor_bytes="<<sizeof(f.source)<<" buffer_descriptor_bytes="<<sizeof(f.scratch)<<" runtime_allocations="<<(observedAllocations-before)<<" (serial host observation, not game/Xbox FPS)\n";
}
}
int main(int argc,char** argv){try{
    if(argc!=2)return 2;
    std::fesetround(FE_TONEAREST);std::ifstream file(std::filesystem::path(argv[1])/"evidence/vehicleContact48R/native-fixtures.json");if(!file)throw std::runtime_error("missing original-derived vehicle contact fixtures");Json fixture;file>>fixture;
    if(fixture.at("schema")!=1||fixture.at("body_slots")!=10||fixture.at("records")!=12||fixture.at("supported_callback")!=0x165430||fixture.at("cases").size()!=338)throw std::runtime_error("incomplete vehicle contact fixture");
    for(const auto& c:fixture.at("cases"))differential(c);
    Json base=fixture.at("cases")[0];base["count"]=2;base["order"]=Json::array({0,1});safety(base);wiring(base);benchmark(base);
    check(differentialCases+boundaryCases+guardDivergences==fixture.at("cases").size(),"all fixtures executed and explicitly classified");check(differentialCases==314&&boundaryCases==14&&guardDivergences==10,"frozen complete/boundary/domain counts");check(modelSpecificCases==3,"three zero-normalization finite-model probes retained");check(observedAllocations==0,"ordinary array and aligned runtime allocations all zero");
    std::cout<<"vehicle_contact48r fixtures="<<fixture.at("cases").size()<<" differential_cases="<<differentialCases<<" original_boundaries="<<boundaryCases<<" guard_divergences="<<guardDivergences<<" zero_normalization_finite_model="<<modelSpecificCases<<" original_callbacks="<<originalCallbacks<<" original_append_calls="<<originalAppends<<" safety_cases="<<safetyCases<<" alias_cases="<<aliasCases<<" wiring_cases="<<wiringCases<<" checks="<<checks<<" failures="<<failures<<" max_scenes="<<maxScenes<<" max_bodies="<<maxBodies<<" max_candidates="<<maxCandidates<<" max_records="<<maxRecords<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;
}catch(const std::exception& e){watchAllocations=false;std::cerr<<label<<": "<<e.what()<<'\n';return 2;}}
