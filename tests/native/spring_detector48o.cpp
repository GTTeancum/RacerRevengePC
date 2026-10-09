#include "spring_detector48o.h"
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
// Instrument only calls into the native seam, never JSON/setup/diagnostics.
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
using Status=SpringDetectorStatus48O;
std::size_t checks=0,failures=0,differentialCases=0,safetyCases=0,aliasCases=0,wiringCases=0;
std::array<std::size_t,5> statusCounts{};
std::string label;
void check(bool ok,const char* message){++checks;if(!ok){++failures;if(failures<45)std::cerr<<label<<": "<<message<<'\n';}}
std::uint32_t bits(float f){std::uint32_t b;std::memcpy(&b,&f,4);return b;}
float fp(std::uint32_t b){float f;std::memcpy(&f,&b,4);return f;}
std::int32_t signedWord(std::uint32_t b){std::int32_t value;std::memcpy(&value,&b,4);return value;}
template<class T>void sentinel(T* p,std::size_t n,unsigned char value){static_assert(std::is_trivially_copyable_v<T>);std::memset(static_cast<void*>(p),value,n*sizeof(T));}
struct Snapshot {
    const void* pointer;std::vector<unsigned char> bytes;
    Snapshot(const void* p,std::size_t n):pointer(p),bytes(n){if(n)std::memcpy(bytes.data(),p,n);}
    void verify()const{check(bytes.empty()||!std::memcmp(pointer,bytes.data(),bytes.size()),"immutable/transactional bytes preserved");}
};
void capture(std::vector<Snapshot>& s,const void* p,std::size_t n){if(n)s.emplace_back(p,n);}
std::array<std::uint32_t,11> stateWords(const SpringRecordState48N& s){return {s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
struct Cursor {
    const Json& words;std::size_t at=0;
    std::uint32_t word(){return words.at(at++).get<std::uint32_t>();}
    void floats(float* p,std::size_t n){for(std::size_t i=0;i<n;++i)p[i]=fp(word());}
    SpringRecordState48N spring(){SpringRecordState48N s{};s.pending38=word();s.contact3C=word();s.upward40=word();s.other44=word();s.point74=word();s.generated78=word();s.currentCandidate130=word();s.pointCount7C=word();s.generatedCount84=signedWord(word());s.generatedCapacity88=signedWord(word());s.lastMaterialD4=word();return s;}
};
struct Rig {
    alignas(std::max_align_t) std::array<SpringDetectorPoint48O,6> points{};
    alignas(std::max_align_t) std::array<SpringDetectorScene48O,6> scenes{};
    alignas(std::max_align_t) std::array<SpringVector4_48N,12> pairs{},staged{};
    alignas(std::max_align_t) std::array<std::uint32_t,12> metadata{};
    alignas(std::max_align_t) std::array<SpringDetectorScene48O,8> scratchScenes{},destinationScenes{};
    alignas(std::max_align_t) std::array<SpringVector4_48N,12> scratchPairs{},destinationPairs{},scratchStaged{},destinationStaged{};
    alignas(std::max_align_t) std::array<std::uint32_t,12> scratchMetadata{},destinationMetadata{};
    SpringDetectorInput48O input{};
    SpringDetectorSource48O source{};
    SpringDetectorBuffers48O scratch{},destination{};
    SpringDetectorState48O output{};
    SpringDetectorState48O* outputPointer=&output;
    Rig(){
        for(std::size_t i=0;i<points.size();++i){points[i].sceneIndex=static_cast<std::uint32_t>(i);points[i].local={{static_cast<float>(i)-2,3,static_cast<float>(i)+1,-9}};}
        for(std::size_t i=0;i<scenes.size();++i){for(unsigned k=0;k<4;++k){scenes[i].world.v[k*5]=1;scenes[i].local.v[k*5]=1;}scenes[i].world.v[12]=static_cast<float>(i);scenes[i].local.v[13]=static_cast<float>(i)*2;}
        for(std::size_t i=0;i<pairs.size();++i)for(unsigned j=0;j<4;++j){pairs[i].v[j]=static_cast<float>(300+4*i+j);staged[i].v[j]=static_cast<float>(400+4*i+j);}
        metadata.fill(0x12345678);
        input.state.center={{101,102,103,-11}};input.state.half={{201,202,203,-12}};
        input.state.spring={11,12,13,14,15,16,17,3,18,23,24};input.shapePairCount=3;
        source={points.data(),4,scenes.data(),4,pairs.data(),8,staged.data(),8,metadata.data(),8};
        scratch={scratchScenes.data()+1,6,scratchPairs.data()+1,10,scratchStaged.data()+1,10,scratchMetadata.data()+1,10};
        destination={destinationScenes.data()+1,6,destinationPairs.data()+1,10,destinationStaged.data()+1,10,destinationMetadata.data()+1,10};
        sentinel(scratchScenes.data(),scratchScenes.size(),0xa5);sentinel(scratchPairs.data(),scratchPairs.size(),0xa5);sentinel(scratchStaged.data(),scratchStaged.size(),0xa5);scratchMetadata.fill(0xa5a5a5a5);
        sentinel(destinationScenes.data(),destinationScenes.size(),0x5a);sentinel(destinationPairs.data(),destinationPairs.size(),0x5a);sentinel(destinationStaged.data(),destinationStaged.size(),0x5a);destinationMetadata.fill(0x5a5a5a5a);
        sentinel(&output,1,0x7b);
    }
    Status call(){watchAllocations=true;const auto s=prepareSpringDetector48O(input,source,scratch,destination,*outputPointer);watchAllocations=false;return s;}
    std::vector<Snapshot> immutable(){std::vector<Snapshot> s;capture(s,&input,sizeof(input));capture(s,&source,sizeof(source));capture(s,&scratch,sizeof(scratch));capture(s,&destination,sizeof(destination));capture(s,points.data(),sizeof(points));capture(s,scenes.data(),sizeof(scenes));capture(s,pairs.data(),sizeof(pairs));capture(s,staged.data(),sizeof(staged));capture(s,metadata.data(),sizeof(metadata));return s;}
    std::vector<Snapshot> destinationSnapshot(){std::vector<Snapshot> s;capture(s,destinationScenes.data(),sizeof(destinationScenes));capture(s,destinationPairs.data(),sizeof(destinationPairs));capture(s,destinationStaged.data(),sizeof(destinationStaged));capture(s,destinationMetadata.data(),sizeof(destinationMetadata));capture(s,&output,sizeof(output));return s;}
    std::vector<Snapshot> guards(){std::vector<Snapshot> s;
        for(auto* p:{scratchScenes.data(),scratchScenes.data()+7,destinationScenes.data(),destinationScenes.data()+7})capture(s,p,sizeof(*p));
        for(auto* p:{scratchPairs.data(),scratchPairs.data()+11,scratchStaged.data(),scratchStaged.data()+11,destinationPairs.data(),destinationPairs.data()+11,destinationStaged.data(),destinationStaged.data()+11})capture(s,p,sizeof(*p));
        for(auto* p:{scratchMetadata.data(),scratchMetadata.data()+11,destinationMetadata.data(),destinationMetadata.data()+11})capture(s,p,sizeof(*p));
        capture(s,destination.scenes+source.sceneCount,(destination.sceneCapacity-source.sceneCount)*sizeof(*destination.scenes));
        capture(s,destination.pairs+source.pairVectorCount,(destination.pairVectorCapacity-source.pairVectorCount)*sizeof(*destination.pairs));
        capture(s,destination.staged+source.stagedVectorCount,(destination.stagedVectorCapacity-source.stagedVectorCount)*sizeof(*destination.staged));
        capture(s,destination.stagedMetadata+source.stagedMetadataCount,(destination.stagedMetadataCapacity-source.stagedMetadataCount)*sizeof(*destination.stagedMetadata));return s;
    }
    void reject(Status expected,bool aliases=false){auto imm=immutable(),dest=destinationSnapshot();const auto before=observedAllocations;const auto actual=call();check(actual==expected,"expected rejection status");if(actual!=expected)std::cerr<<label<<": actual="<<static_cast<unsigned>(actual)<<" expected="<<static_cast<unsigned>(expected)<<'\n';for(const auto& s:imm)s.verify();for(const auto& s:dest)s.verify();check(observedAllocations==before,"rejection allocation-free");++safetyCases;if(aliases)++aliasCases;}
    void* write(unsigned index){auto& b=index<4?scratch:destination;switch(index%4){case 0:return b.scenes;case 1:return b.pairs;case 2:return b.staged;default:return b.stagedMetadata;}}
    std::size_t writeBytes(unsigned index){auto& b=index<4?scratch:destination;switch(index%4){case 0:return b.sceneCapacity*sizeof(*b.scenes);case 1:return b.pairVectorCapacity*sizeof(*b.pairs);case 2:return b.stagedVectorCapacity*sizeof(*b.staged);default:return b.stagedMetadataCapacity*sizeof(*b.stagedMetadata);}}
    void setWrite(unsigned index,void* p){auto& b=index<4?scratch:destination;switch(index%4){case 0:b.scenes=static_cast<SpringDetectorScene48O*>(p);break;case 1:b.pairs=static_cast<SpringVector4_48N*>(p);break;case 2:b.staged=static_cast<SpringVector4_48N*>(p);break;default:b.stagedMetadata=static_cast<std::uint32_t*>(p);break;}}
    void writeCapacity(unsigned index,std::size_t n){auto& b=index<4?scratch:destination;switch(index%4){case 0:b.sceneCapacity=n;break;case 1:b.pairVectorCapacity=n;break;case 2:b.stagedVectorCapacity=n;break;default:b.stagedMetadataCapacity=n;break;}}
    const void* read(unsigned index){switch(index){case 0:return source.points;case 1:return source.scenes;case 2:return source.pairs;case 3:return source.staged;default:return source.stagedMetadata;}}
    std::size_t readBytes(unsigned index){switch(index){case 0:return source.pointCapacity*sizeof(*source.points);case 1:return source.sceneCount*sizeof(*source.scenes);case 2:return source.pairVectorCount*sizeof(*source.pairs);case 3:return source.stagedVectorCount*sizeof(*source.staged);default:return source.stagedMetadataCount*sizeof(*source.stagedMetadata);}}
    void setRead(unsigned index,const void* p){switch(index){case 0:source.points=static_cast<const SpringDetectorPoint48O*>(p);break;case 1:source.scenes=static_cast<const SpringDetectorScene48O*>(p);break;case 2:source.pairs=static_cast<const SpringVector4_48N*>(p);break;case 3:source.staged=static_cast<const SpringVector4_48N*>(p);break;default:source.stagedMetadata=static_cast<const std::uint32_t*>(p);break;}}
    void readCapacity(unsigned index,std::size_t n){switch(index){case 0:source.pointCapacity=n;break;case 1:source.sceneCount=n;break;case 2:source.pairVectorCount=n;break;case 3:source.stagedVectorCount=n;break;default:source.stagedMetadataCount=n;break;}}
    std::array<void*,4> descriptors(){return {&input,&source,&scratch,&destination};}
};
void differential(const Json& j){
    label=j.at("name").get<std::string>();const auto& a=j.at("input_words");const auto& e=j.at("output_words");
    if(a.size()!=250||e.size()!=160)throw std::runtime_error("detector fixture projection width");
    Rig r;Cursor c{a};r.input.springPresent=c.word()!=0;r.input.pairsPresent=c.word()!=0;r.input.providerPresent=c.word()!=0;r.input.providerEnabled=c.word()!=0;r.input.queryPresent=c.word()!=0;r.input.queryEnabled=c.word()!=0;r.input.resetSpring=c.word()!=0;r.input.explicitList=c.word()!=0;r.input.shapePresent=c.word()!=0;r.input.shapePairCount=c.word();r.input.state.queryType=c.word();
    c.floats(r.input.state.center.v,4);c.floats(r.input.state.half.v,4);r.input.state.spring=c.spring();
    for(unsigned i=0;i<4;++i){c.floats(r.points[i].local.v,4);c.floats(r.scenes[i].world.v,16);c.floats(r.scenes[i].local.v,16);r.scenes[i].flags=c.word();r.points[i].sceneIndex=i;}
    for(unsigned i=0;i<8;++i)c.floats(r.pairs[i].v,4);
    for(unsigned i=0;i<8;++i)c.floats(r.staged[i].v,4);
    for(unsigned i=0;i<8;++i)r.metadata[i]=c.word();
    check(c.at==250,"consumed complete input projection");auto imm=r.immutable(),guard=r.guards();const auto actual=r.call();const auto original=j.at("status").get<unsigned>();if(original>=5)throw std::runtime_error("unsupported original fixture boundary");
    const std::array<Status,5> statuses{Status::NoSpring,Status::QueryDisabled,Status::RegistrationBoundary,Status::LimitedQueryBoundary,Status::WorldQueryBoundary};
    check(actual==statuses[original],"original phase boundary");if(actual!=statuses[original])std::cerr<<label<<": status="<<static_cast<unsigned>(actual)<<'\n';
    Cursor expected{e};auto eqFloat=[&](const float* p,std::size_t n,const char* message){for(std::size_t i=0;i<n;++i)check(bits(p[i])==expected.word(),message);};
    eqFloat(r.output.center.v,4,"all center raw words");eqFloat(r.output.half.v,4,"all half-extents raw words");for(auto word:stateWords(r.output.spring))check(word==expected.word(),"all11 reset/preserved spring words");
    for(unsigned i=0;i<8;++i)eqFloat(r.destination.pairs[i].v,4,"all pair words including first endpoints/stale tails");
    check(r.output.queryType==expected.word(),"source tag3 staging/unchanged query type");
    for(unsigned i=0;i<8;++i)eqFloat(r.destination.staged[i].v,4,"all staged vector words including stale tails");
    for(unsigned i=0;i<8;++i)check(r.destination.stagedMetadata[i]==expected.word(),"all metadata including stale tails");
    for(unsigned i=0;i<4;++i){eqFloat(r.destination.scenes[i].world.v,16,"every scene world cache word");check(r.destination.scenes[i].flags==expected.word(),"original scene flag byte");check(!std::memcmp(&r.destination.scenes[i].local,&r.scenes[i].local,sizeof(BodyMatrix48G)),"local matrix copied without normalization");check(r.destination.scenes[i].parentPresent==r.scenes[i].parentPresent,"opaque parent state preserved");}
    check(expected.at==160,"compared complete output projection");for(const auto& s:imm)s.verify();for(const auto& s:guard)s.verify();++differentialCases;++statusCounts[original];
}
template<class Change>void rejection(const std::string& name,Change change,Status expected=Status::InvalidInput){label=name;Rig r;change(r);r.reject(expected);}
void safety(){
    label="native wiring baseline";{Rig r;check(r.call()==Status::LimitedQueryBoundary,"baseline reaches source type3 list boundary");check(r.output.queryType==3,"not a48N type2 adapter");}
    for(unsigned w=0;w<8;++w){
        rejection("null declared writable span "+std::to_string(w),[&](auto& r){r.setWrite(w,nullptr);});
        rejection("misaligned writable span "+std::to_string(w),[&](auto& r){r.setWrite(w,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(r.write(w))+1));});
        rejection("overflow writable capacity "+std::to_string(w),[&](auto& r){r.writeCapacity(w,std::numeric_limits<std::size_t>::max());});
        rejection("writable address wrap "+std::to_string(w),[&](auto& r){r.setWrite(w,reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15)));});
        rejection("destination projection capacity "+std::to_string(w),[&](auto& r){r.writeCapacity(w,w%4==0?3:7);},Status::InsufficientCapacity);
    }
    for(unsigned rindex=0;rindex<5;++rindex){
        rejection("null declared source span "+std::to_string(rindex),[&](auto& r){r.setRead(rindex,nullptr);});
        rejection("misaligned source span "+std::to_string(rindex),[&](auto& r){r.setRead(rindex,reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(r.read(rindex))+1));});
        rejection("overflow source capacity "+std::to_string(rindex),[&](auto& r){r.readCapacity(rindex,std::numeric_limits<std::size_t>::max());},rindex==0?Status::InvalidInput:Status::InsufficientCapacity);
        rejection("source address wrap "+std::to_string(rindex),[&](auto& r){r.setRead(rindex,reinterpret_cast<const void*>(std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(15)));});
    }
    // All28 write/write combinations, first at exact starts and then in the
    // declared tail. No safety claim is limited to the currently used prefix.
    for(unsigned a=0;a<8;++a)for(unsigned b=a+1;b<8;++b)for(unsigned tail=0;tail<2;++tail){label="declared write/write alias "+std::to_string(a)+":"+std::to_string(b)+":"+std::to_string(tail);Rig r;auto p=reinterpret_cast<std::uintptr_t>(r.write(b));if(tail)p+=r.writeBytes(b)-4;r.setWrite(a,reinterpret_cast<void*>(p));r.reject(Status::InvalidInput,true);}
    // Every writable kind against every full source span, including unused
    // points and staged metadata. A tail alias catches prefix-only validation.
    for(unsigned w=0;w<8;++w)for(unsigned rd=0;rd<5;++rd)for(unsigned tail=0;tail<2;++tail){label="declared write/read alias "+std::to_string(w)+":"+std::to_string(rd)+":"+std::to_string(tail);Rig r;auto p=reinterpret_cast<std::uintptr_t>(r.read(rd));if(tail)p+=r.readBytes(rd)-4;r.setWrite(w,reinterpret_cast<void*>(p));r.reject(Status::InvalidInput,true);}
    for(unsigned w=0;w<8;++w)for(unsigned rd=0;rd<5;++rd){label="source only in writable unused tail "+std::to_string(w)+":"+std::to_string(rd);Rig r;r.setRead(rd,reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(r.write(w))+r.writeBytes(w)-4));r.reject(Status::InvalidInput,true);}
    for(unsigned w=0;w<8;++w)for(unsigned d=0;d<4;++d){label="write/descriptor alias "+std::to_string(w)+":"+std::to_string(d);Rig r;r.setWrite(w,r.descriptors()[d]);r.reject(Status::InvalidInput,true);}
    for(unsigned w=0;w<8;++w){label="write/result state alias "+std::to_string(w);Rig r;r.setWrite(w,&r.output);r.reject(Status::InvalidInput,true);}
    for(unsigned rd=0;rd<5;++rd){label="result/source alias "+std::to_string(rd);Rig r;r.outputPointer=const_cast<SpringDetectorState48O*>(reinterpret_cast<const SpringDetectorState48O*>(r.read(rd)));r.reject(Status::InvalidInput,true);}
    for(unsigned d=0;d<4;++d){label="result/descriptor alias "+std::to_string(d);Rig r;r.outputPointer=static_cast<SpringDetectorState48O*>(r.descriptors()[d]);r.reject(Status::InvalidInput,true);}
    // Place descriptors in separate padded objects so a writable tail aliases
    // exactly the chosen descriptor, not a neighboring descriptor or result.
    for(unsigned w=0;w<8;++w)for(unsigned d=0;d<4;++d){
        struct InputBox {std::array<unsigned char,512> before{};SpringDetectorInput48O value{};std::array<unsigned char,512> after{};} in;
        struct SourceBox {std::array<unsigned char,512> before{};SpringDetectorSource48O value{};std::array<unsigned char,512> after{};} src;
        struct BufferBox {std::array<unsigned char,512> before{};SpringDetectorBuffers48O value{};std::array<unsigned char,512> after{};} temp,dest;
        struct StateBox {std::array<unsigned char,512> before{};SpringDetectorState48O value{};std::array<unsigned char,512> after{};} out;
        in.value.springPresent=false;
        const std::array<void*,4> descriptor{&in.value,&src.value,&temp.value,&dest.value};
        const std::array<std::size_t,4> element{sizeof(SpringDetectorScene48O),sizeof(SpringVector4_48N),sizeof(SpringVector4_48N),sizeof(std::uint32_t)};
        auto* p=reinterpret_cast<unsigned char*>(descriptor[d])-element[w%4];auto& b=w<4?temp.value:dest.value;
        switch(w%4){case 0:b.scenes=reinterpret_cast<SpringDetectorScene48O*>(p);b.sceneCapacity=2;break;case 1:b.pairs=reinterpret_cast<SpringVector4_48N*>(p);b.pairVectorCapacity=2;break;case 2:b.staged=reinterpret_cast<SpringVector4_48N*>(p);b.stagedVectorCapacity=2;break;default:b.stagedMetadata=reinterpret_cast<std::uint32_t*>(p);b.stagedMetadataCapacity=2;break;}
        std::vector<Snapshot> saved;capture(saved,&in,sizeof(in));capture(saved,&src,sizeof(src));capture(saved,&temp,sizeof(temp));capture(saved,&dest,sizeof(dest));capture(saved,&out,sizeof(out));
        label="isolated descriptor in unused writable tail "+std::to_string(w)+":"+std::to_string(d);
        watchAllocations=true;const auto result=prepareSpringDetector48O(in.value,src.value,temp.value,dest.value,out.value);watchAllocations=false;
        check(result==Status::InvalidInput,"full writable extent excludes isolated descriptor");for(const auto& snapshot:saved)snapshot.verify();++safetyCases;++aliasCases;
    }
    for(auto count:{0u,0xffffffffu,0x80000000u})rejection("nonpositive active signed count",[&](auto& r){r.input.state.spring.pointCount7C=count;});
    rejection("point input capacity",[](auto& r){r.source.pointCapacity=2;},Status::InsufficientCapacity);
    rejection("pair preparation source capacity",[](auto& r){r.source.pairVectorCount=5;},Status::InsufficientCapacity);
    rejection("shape copy source capacity independent of points",[](auto& r){r.input.shapePairCount=5;},Status::InsufficientCapacity);
    rejection("shape staging backing capacity",[](auto& r){r.source.stagedVectorCount=5;},Status::InsufficientCapacity);
    rejection("shape metadata backing capacity",[](auto& r){r.source.stagedMetadataCount=5;},Status::InsufficientCapacity);
    rejection("shape doubled count overflow",[](auto& r){r.input.shapePairCount=0x40000000;});
    rejection("late scene index outside span",[](auto& r){r.points[2].sceneIndex=4;});
    rejection("active provider with pairs",[](auto& r){r.input.providerPresent=r.input.providerEnabled=true;},Status::UnsupportedScope);
    rejection("active provider without pairs",[](auto& r){r.input.pairsPresent=false;r.input.providerPresent=r.input.providerEnabled=true;},Status::UnsupportedScope);
    rejection("late dirty parent scene",[](auto& r){r.scenes[2].flags=2;r.scenes[2].parentPresent=true;},Status::UnsupportedScope);
    rejection("type2 no-pair needs separate query preparation",[](auto& r){r.input.pairsPresent=false;r.input.state.queryType=2;},Status::UnsupportedScope);
    rejection("late local nonfinite",[](auto& r){r.points[2].local.v[1]=std::numeric_limits<float>::quiet_NaN();});
    rejection("late matrix nonfinite",[](auto& r){r.scenes[2].world.v[6]=std::numeric_limits<float>::infinity();});
    rejection("late transform overflow",[](auto& r){r.points[2].local.v[0]=std::numeric_limits<float>::max();r.scenes[2].world.v[0]=2;});
    rejection("bounds addition overflow",[](auto& r){for(unsigned i=0;i<3;++i){r.points[i].local.v[0]=0;r.scenes[i].world.v[12]=std::numeric_limits<float>::max();}});
    rejection("bounds subtraction overflow",[](auto& r){r.points[0].local.v[0]=0;r.points[1].local.v[0]=0;r.scenes[0].world.v[12]=-std::numeric_limits<float>::max();r.scenes[1].world.v[12]=std::numeric_limits<float>::max();});
    // Explicit late transaction probe: first point changes dirty scene cache
    // and endpoint scratch; final point fails. Public destinations/state cannot
    // contain even the earlier, otherwise valid writes.
    rejection("late failure after dirty-cache work",[](auto& r){r.scenes[0].flags=0;r.scenes[0].local.v[12]=97;r.scenes[2].flags=0;r.scenes[2].local.v[0]=std::numeric_limits<float>::infinity();});
    for(unsigned w=0;w<8;++w)rejection("no spring still validates declared writable spans",[&](auto& r){r.input.springPresent=false;r.setWrite(w,nullptr);});
}
void nativeWiring(){
    // These positive probes establish native indexed-scene wiring and safety
    // domain behavior. They are not counted as independent original fixtures.
    label="native wiring duplicate dirty-root scene indices";{
        Rig r;r.input.state.spring.pointCount7C=3;r.input.shapePairCount=3;
        for(unsigned i=0;i<3;++i)r.points[i].sceneIndex=1;
        r.scenes[1].flags=0xabcd0080;r.scenes[1].local.v[12]=8;r.scenes[1].local.v[13]=-5;
        auto imm=r.immutable(),guard=r.guards();check(r.call()==Status::LimitedQueryBoundary,"shared dirty root succeeds");
        check(r.destination.scenes[1].flags==0xabcd0081,"only low flag bit set, upper bytes retained");check(!std::memcmp(&r.destination.scenes[1].world,&r.scenes[1].local,sizeof(BodyMatrix48G)),"shared root cache copies local raw words once");
        for(unsigned i=0;i<3;++i){const auto& p=r.destination.pairs[2*i+1];check(p.v[0]==r.points[i].local.v[0]+8&&p.v[1]==-2&&p.v[2]==r.points[i].local.v[2]&&bits(p.v[3])==0x3f800000,"all duplicate-index points use refreshed common cache");}
        for(const auto& s:imm)s.verify();
        for(const auto& s:guard)s.verify();
        ++wiringCases;
    }
    label="native wiring prepared opaque parent and unused W lanes";{
        Rig r;for(unsigned i=0;i<4;++i){r.scenes[i].parentPresent=true;r.scenes[i].flags=0xfe000003;r.points[i].local.v[3]=fp(0x7fc12345);for(unsigned k=0;k<4;++k)r.scenes[i].world.v[4*k+3]=fp(0x7f800000u+k);r.scenes[i].local.v[0]=fp(0x7fc54321);}
        auto imm=r.immutable();check(r.call()==Status::LimitedQueryBoundary,"prepared scenes ignore opaque parents/local/W");
        for(unsigned i=0;i<4;++i){check(r.destination.scenes[i].flags==r.scenes[i].flags,"prepared flags preserved");check(!std::memcmp(&r.destination.scenes[i].world,&r.scenes[i].world,sizeof(BodyMatrix48G)),"world W payloads preserved in cache");}
        for(const auto& s:imm)s.verify();
        ++wiringCases;
    }
    label="native wiring unused point and scene payloads";{
        Rig r;r.input.state.spring.pointCount7C=1;r.points[3].sceneIndex=0xffffffff;r.points[3].local.v[0]=fp(0x7fc12345);r.scenes[3].flags=0;r.scenes[3].parentPresent=true;r.scenes[3].world.v[0]=fp(0x7f800000);
        check(r.call()==Status::LimitedQueryBoundary,"unconsumed point payloads do not create errors");check(r.destination.scenes[3].parentPresent&&!r.destination.scenes[3].flags,"unconsumed scene state projected unchanged");++wiringCases;
    }
    label="native wiring query-disabled preserved first endpoints";{
        Rig r;r.input.queryEnabled=false;r.input.shapePairCount=0xffffffff;
        check(r.call()==Status::QueryDisabled,"query gate skips malformed unused shape count");for(unsigned i=0;i<4;++i)check(!std::memcmp(&r.destination.pairs[2*i],&r.pairs[2*i],sizeof(SpringVector4_48N)),"disabled query retains every old first endpoint");check(r.output.spring.pending38==1&&r.output.spring.generatedCount84==0,"reset still happens before query gate");++wiringCases;
    }
    label="native wiring no spring projected unchanged";{
        Rig r;r.input.springPresent=false;r.input.providerPresent=r.input.providerEnabled=true;r.input.state.spring.pointCount7C=0xffffffff;r.input.state.center.v[0]=fp(0x7fc12345);r.input.shapePairCount=0xffffffff;r.scenes[0].flags=0;r.scenes[0].parentPresent=true;
        check(r.call()==Status::NoSpring,"no-spring semantic bypass");check(!std::memcmp(&r.output,&r.input.state,sizeof(r.output)),"all detector and spring state preserved");check(!std::memcmp(r.destination.pairs,r.pairs.data(),8*sizeof(SpringVector4_48N)),"all pair bytes projected unchanged");++wiringCases;
    }
    label="native wiring null shape type3 no-copy";{
        Rig r;r.input.shapePresent=false;r.input.shapePairCount=0xffffffff;r.input.state.queryType=2;
        check(r.call()==Status::LimitedQueryBoundary&&r.output.queryType==3,"null shape still sets original tag3");check(!std::memcmp(r.destination.staged,r.staged.data(),8*sizeof(SpringVector4_48N))&&!std::memcmp(r.destination.stagedMetadata,r.metadata.data(),8*sizeof(std::uint32_t)),"null shape preserves staged payloads");++wiringCases;
    }
    for(auto count:{0u,0xffffffffu,0x80000000u}){
        label="native wiring nonpositive no-pair provider bypass";Rig r;r.input.pairsPresent=false;r.input.providerPresent=r.input.providerEnabled=true;r.input.state.spring.pointCount7C=count;
        check(r.call()==Status::RegistrationBoundary,"no point loop means no provider call");check(r.output.spring.pointCount7C==count,"signed raw count preserved");check(!std::memcmp(&r.output.center,&r.input.state.center,sizeof(SpringVector4_48N))&&!std::memcmp(&r.output.half,&r.input.state.half,sizeof(SpringVector4_48N)),"no-pair center/half retain all raw lanes");++wiringCases;
    }
    label="native wiring exact capacities";{
        Rig r;r.scratch.sceneCapacity=r.destination.sceneCapacity=r.source.sceneCount;r.scratch.pairVectorCapacity=r.destination.pairVectorCapacity=r.source.pairVectorCount;r.scratch.stagedVectorCapacity=r.destination.stagedVectorCapacity=r.source.stagedVectorCount;r.scratch.stagedMetadataCapacity=r.destination.stagedMetadataCapacity=r.source.stagedMetadataCount;
        check(r.call()==Status::LimitedQueryBoundary,"exact source-projection capacities sufficient");++wiringCases;
    }
    label="native wiring null zero extents";{
        SpringDetectorInput48O in{};in.springPresent=false;SpringDetectorSource48O source{};SpringDetectorBuffers48O scratch{},dest{};SpringDetectorState48O out{};
        watchAllocations=true;const auto s=prepareSpringDetector48O(in,source,scratch,dest,out);watchAllocations=false;check(s==Status::NoSpring,"null zero spans support absent spring");++wiringCases;
    }
    label="native wiring 1024 point caller-owned storage";{
        constexpr std::size_t n=1024;std::vector<SpringDetectorPoint48O> p(n);std::vector<SpringVector4_48N> pair(n*2),stage(n*2),sp(n*2),dp(n*2),ss(n*2),ds(n*2);std::vector<std::uint32_t> meta(n*2,99),sm(n*2),dm(n*2);std::array<SpringDetectorScene48O,1> scene{},sc{},dc{};for(unsigned k=0;k<4;++k)scene[0].world.v[k*5]=1;for(std::size_t i=0;i<n;++i)p[i].local={{static_cast<float>(i),-1,2,-9}};
        SpringDetectorInput48O in{};in.state.spring.pointCount7C=n;in.shapePairCount=n;SpringDetectorSource48O src{p.data(),p.size(),scene.data(),1,pair.data(),pair.size(),stage.data(),stage.size(),meta.data(),meta.size()};SpringDetectorBuffers48O scratch{sc.data(),1,sp.data(),sp.size(),ss.data(),ss.size(),sm.data(),sm.size()},dest{dc.data(),1,dp.data(),dp.size(),ds.data(),ds.size(),dm.data(),dm.size()};SpringDetectorState48O out{};
        watchAllocations=true;const auto status=prepareSpringDetector48O(in,src,scratch,dest,out);watchAllocations=false;
        check(status==Status::LimitedQueryBoundary,"no fixed fixture-count/gameplay cap");check(dp.back().v[0]==1023&&out.center.v[0]==511.5f&&out.half.v[0]==511.5f&&dm.front()==0&&dm.back()==0,"large complete finite domain preparation/staging");++wiringCases;
    }
}
void benchmark(){
    label="serial host detector preparation benchmark";Rig r;r.scenes[0].flags=0;constexpr std::size_t loops=20000;std::array<double,7> ns{};std::size_t completed=0;std::uint32_t checksum=0;const auto before=observedAllocations;
    for(auto& sample:ns){const auto start=std::chrono::steady_clock::now();watchAllocations=true;for(std::size_t i=0;i<loops;++i){completed+=prepareSpringDetector48O(r.input,r.source,r.scratch,r.destination,r.output)==Status::LimitedQueryBoundary;checksum+=bits(r.destination.pairs[5].v[0])+static_cast<std::uint32_t>(i);}watchAllocations=false;const auto end=std::chrono::steady_clock::now();sample=std::chrono::duration<double,std::nano>(end-start).count()/loops;}
    std::sort(ns.begin(),ns.end());check(completed==loops*ns.size(),"serial repeated preparation completed");check(observedAllocations==before,"serial repeated preparation has zero C++ allocations");
    const auto scratchBytes=r.scratch.sceneCapacity*sizeof(SpringDetectorScene48O)+r.scratch.pairVectorCapacity*sizeof(SpringVector4_48N)+r.scratch.stagedVectorCapacity*sizeof(SpringVector4_48N)+r.scratch.stagedMetadataCapacity*sizeof(std::uint32_t);
    const auto sourceBytes=r.source.pointCapacity*sizeof(SpringDetectorPoint48O)+r.source.sceneCount*sizeof(SpringDetectorScene48O)+(r.source.pairVectorCount+r.source.stagedVectorCount)*sizeof(SpringVector4_48N)+r.source.stagedMetadataCount*sizeof(std::uint32_t);
    std::cout<<"spring_detector_cost_host_ns median="<<ns[3]<<" max_batch_mean="<<ns.back()<<" serial_calls="<<completed<<" points=3 scene_backing=4 pair_vector_backing=8 shape_pairs=3 source_bytes="<<sourceBytes<<" scratch_bytes="<<scratchBytes<<" destination_bytes="<<scratchBytes<<" input_bytes="<<sizeof(SpringDetectorInput48O)<<" source_descriptor_bytes="<<sizeof(SpringDetectorSource48O)<<" buffer_descriptor_bytes="<<sizeof(SpringDetectorBuffers48O)<<" output_state_bytes="<<sizeof(SpringDetectorState48O)<<" point_bytes="<<sizeof(SpringDetectorPoint48O)<<" scene_bytes="<<sizeof(SpringDetectorScene48O)<<" runtime_allocations="<<(observedAllocations-before)<<" checksum="<<checksum<<" (serial host component observation; no FPGA, console, or gameplay FPS claim)\n";
}
}
int main(int argc,char** argv){try{
    if(argc!=2)return 2;
    std::fesetround(FE_TONEAREST);std::ifstream f(std::filesystem::path(argv[1])/"evidence/springDetector48O/fixtures.json");if(!f)throw std::runtime_error("missing springDetector48O/fixtures.json");Json fixture;f>>fixture;
    if(fixture.at("layout").at("input_words")!=250||fixture.at("layout").at("output_words")!=160||fixture.at("cases").size()!=288)throw std::runtime_error("incomplete detector fixture schema");
    for(const auto& j:fixture.at("cases"))differential(j);
    safety();nativeWiring();benchmark();check(statusCounts==std::array<std::size_t,5>{19,44,26,128,71},"all five frozen original boundary counts");check(observedAllocations==0,"all instrumented native calls allocation-free");
    std::cout<<"spring_detector48O cases="<<differentialCases<<" checks="<<checks<<" failures="<<failures<<" safety_cases="<<safetyCases<<" alias_cases="<<aliasCases<<" native_wiring_cases="<<wiringCases<<" original_status_counts="<<statusCounts[0]<<','<<statusCounts[1]<<','<<statusCounts[2]<<','<<statusCounts[3]<<','<<statusCounts[4]<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;
}catch(const std::exception& e){watchAllocations=false;std::cerr<<label<<": "<<e.what()<<'\n';return 2;}}
