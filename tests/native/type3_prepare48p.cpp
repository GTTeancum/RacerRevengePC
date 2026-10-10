#include "type3_prepare48p.h"
#include "json.hpp"
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

static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_NOINLINE __attribute__((noinline))
#else
#define RR_NOINLINE
#endif
RR_NOINLINE void* operator new(std::size_t n){if(watchAllocations)++observedAllocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
RR_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
using Json=nlohmann::json;
namespace {
using Status=Type3PrepareStatus48P;
std::size_t checks=0,failures=0,eligible=0,unsupported=0,direct=0;
std::string label;
void check(bool value,const char* detail){++checks;if(!value){++failures;if(failures<25)std::cerr<<label<<": "<<detail<<'\n';}}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
void loadWords(void* dest,const Json& j,std::size_t count,std::size_t offset=0){
    if(j.size()<count+offset)throw std::runtime_error("fixture width");
    auto* p=static_cast<unsigned char*>(dest);
    for(std::size_t i=0;i<count;++i){const auto word=j[i+offset].get<std::uint32_t>();std::memcpy(p+4*i,&word,4);}
}
void equalWords(const void* data,const Json& j,std::size_t count,const char* detail,std::size_t offset=0){
    if(j.size()<count+offset)throw std::runtime_error("expected width");
    const auto* p=static_cast<const unsigned char*>(data);
    for(std::size_t i=0;i<count;++i){std::uint32_t word;std::memcpy(&word,p+4*i,4);check(word==j[i+offset].get<std::uint32_t>(),detail);}
}
template<std::size_t N>struct Storage {
    std::array<SpringVector4_48N,2*N> endpoints{};
    std::array<std::uint32_t,2*N> scenes{};
    std::array<Vector3_48C,N> localStarts{},localDirections{};
    std::array<SpringVector4_48N,N> starts{},directions{},normals{},points{};
    std::array<float,N> lengths{},fractions{};
    std::array<std::uint32_t,N> hits{};
    Type3QueryState48P state{};
    Type3RayBuffers48P buffers(){return {localStarts.data(),localDirections.data(),starts.data(),directions.data(),normals.data(),points.data(),hits.data(),fractions.data(),N,lengths.data(),N};}
    Type3PrepareInput48P input(std::uint32_t n){return {n,endpoints.data(),2*N,scenes.data(),2*N,{{0,0,0,1}}};}
};
void loadState(Storage<4>& a,const Json& snapshot){
    const auto& p=snapshot.at("pair");const auto& r=snapshot.at("prepared");
    loadWords(a.endpoints.data(),r.at("endpoints"),32);loadWords(a.scenes.data(),r.at("scene_ids"),8);
    for(unsigned i=0;i<4;++i){loadWords(&a.localStarts[i],r.at("local_starts"),3,4*i);loadWords(&a.localDirections[i],r.at("local_directions"),3,4*i);}
    loadWords(a.starts.data(),r.at("world_starts"),16);loadWords(a.directions.data(),r.at("world_directions"),16);
    loadWords(a.lengths.data(),r.at("lengths"),4);loadWords(a.normals.data(),p.at("normals"),16);loadWords(a.points.data(),p.at("points"),16);
    loadWords(a.hits.data(),p.at("hit_flags"),4);loadWords(a.fractions.data(),p.at("fractions"),4);
    loadWords(&a.state.localCenter,r.at("centers"),3);loadWords(&a.state.worldCenter,r.at("centers"),4,4);
    a.state.radius=fp(r.at("radius").get<std::uint32_t>());loadWords(&a.state.queryCenter,r.at("query_sphere"),4);
    a.state.queryRadius=fp(r.at("query_sphere")[4].get<std::uint32_t>());
    loadWords(&a.state.result00,p.at("selected"),4);loadWords(&a.state.result10,p.at("selected"),4,4);
    loadWords(&a.state.selectedNormal,p.at("selected"),4,8);loadWords(&a.state.selectedPoint,p.at("selected"),4,12);
    loadWords(&a.state.selectedCorrection,p.at("selected"),4,16);
    a.state.hitCount=p.at("count").get<std::uint32_t>();a.state.selectedFraction=fp(p.at("selected_fraction").get<std::uint32_t>());
    a.state.opaque78=p.at("other_fields")[0].get<std::uint32_t>();a.state.opaque7c=p.at("other_fields")[1].get<std::uint32_t>();
}
void compare(const Storage<4>& a,const Json& snapshot){
    const auto& p=snapshot.at("pair");const auto& r=snapshot.at("prepared");
    equalWords(a.endpoints.data(),r.at("endpoints"),32,"source endpoints unchanged");equalWords(a.scenes.data(),r.at("scene_ids"),8,"metadata including unused tail");
    for(unsigned i=0;i<4;++i){equalWords(&a.localStarts[i],r.at("local_starts"),3,"local-start XYZ unchanged",4*i);equalWords(&a.localDirections[i],r.at("local_directions"),3,"local-direction XYZ unchanged",4*i);}
    equalWords(a.starts.data(),r.at("world_starts"),16,"world-start bits and tails");equalWords(a.directions.data(),r.at("world_directions"),16,"direction bits and tails");
    equalWords(a.lengths.data(),r.at("lengths"),4,"length bits and tails");equalWords(a.normals.data(),p.at("normals"),16,"normal arrays unchanged");equalWords(a.points.data(),p.at("points"),16,"point arrays unchanged");
    equalWords(a.hits.data(),p.at("hit_flags"),4,"flags unchanged");equalWords(a.fractions.data(),p.at("fractions"),4,"fractions unchanged");
    equalWords(&a.state.localCenter,r.at("centers"),3,"local-center XYZ unchanged");equalWords(&a.state.worldCenter,r.at("centers"),4,"world-center bits",4);
    check(bits(a.state.radius)==r.at("radius").get<std::uint32_t>(),"radius bits");equalWords(&a.state.queryCenter,r.at("query_sphere"),4,"query-center bits");
    check(bits(a.state.queryRadius)==r.at("query_sphere")[4].get<std::uint32_t>(),"query-radius bits");
    equalWords(&a.state.result00,p.at("selected"),4,"result00 sentinel bits");equalWords(&a.state.result10,p.at("selected"),4,"result10 sentinel bits",4);
    equalWords(&a.state.selectedNormal,p.at("selected"),4,"selected normal unchanged",8);equalWords(&a.state.selectedPoint,p.at("selected"),4,"selected point unchanged",12);
    equalWords(&a.state.selectedCorrection,p.at("selected"),4,"selected correction unchanged",16);
    check(a.state.hitCount==p.at("count").get<std::uint32_t>(),"hit count");check(bits(a.state.selectedFraction)==p.at("selected_fraction").get<std::uint32_t>(),"selected fraction");
    check(a.state.opaque78==p.at("other_fields")[0].get<std::uint32_t>()&&a.state.opaque7c==p.at("other_fields")[1].get<std::uint32_t>(),"opaque result fields unchanged");
}
void original(const Json& c){
    label=c.at("name").get<std::string>();
    if(c.at("entry")=="direct"){++direct;return;} // 126F40 direct entry never executes 11DD00.
    Storage<4> a;loadState(a,c.at("expected").at("initial"));
    auto input=a.input(static_cast<std::uint32_t>(c.at("pairs").size()));auto buffers=a.buffers();
    const auto old=a;const auto oldInput=input;const auto oldBuffers=buffers;
    bool hasScene=false;for(std::size_t i=0;i<2*input.pairCount;++i)hasScene=hasScene||a.scenes[i]!=0;
    watchAllocations=true;const auto status=prepareType3Rays48P(input,buffers,a.state);watchAllocations=false;
    if(hasScene){++unsupported;check(status==Status::UnsupportedScope,"nonzero endpoint scenes explicitly unsupported");check(!std::memcmp(&a,&old,sizeof(a)),"unsupported full storage unchanged");}
    else {++eligible;check(status==Status::Complete,"original preparation completed");compare(a,c.at("expected").at("after_prepare"));}
    check(!std::memcmp(&input,&oldInput,sizeof(input))&&!std::memcmp(&buffers,&oldBuffers,sizeof(buffers)),"descriptors unchanged");
}
void safety(){
    label="preparation transactional guards";
    Storage<20> a;
    for(unsigned i=0;i<20;++i){a.endpoints[2*i]={{float(i),2,3,fp(0x7fc12345)}};a.endpoints[2*i+1]={{float(i),3,3,fp(0xff812345)}};}
    std::memset(a.starts.data(),0xa5,sizeof(a.starts));std::memset(a.directions.data(),0xb6,sizeof(a.directions));
    std::memset(a.lengths.data(),0xc7,sizeof(a.lengths));
    auto input=a.input(17);auto buffers=a.buffers();
    const auto initial=a;
    watchAllocations=true;auto status=prepareType3Rays48P(input,buffers,a.state);watchAllocations=false;
    check(status==Status::Complete,"17 pairs allowed, no invented16 cap");
    for(unsigned i=0;i<17;++i)check(!std::memcmp(&a.starts[i],&a.endpoints[2*i],16),"raw source W copied including NaN payload");
    check(!std::memcmp(a.starts.data()+17,initial.starts.data()+17,3*sizeof(a.starts[0]))&&
          !std::memcmp(a.directions.data()+17,initial.directions.data()+17,3*sizeof(a.directions[0]))&&
          !std::memcmp(a.lengths.data()+17,initial.lengths.data()+17,3*sizeof(float)),"unused output tails unchanged");
    const auto reject=[&](Type3PrepareInput48P in,Type3RayBuffers48P out,Status expected){
        const auto before=a;const auto oldIn=in;const auto oldOut=out;
        watchAllocations=true;const auto result=prepareType3Rays48P(in,out,a.state);watchAllocations=false;
        check(result==expected,"expected rejection status");check(!std::memcmp(&a,&before,sizeof(a)),"rejection leaves every source/output/state byte");
        check(!std::memcmp(&in,&oldIn,sizeof(in))&&!std::memcmp(&out,&oldOut,sizeof(out)),"rejected descriptors unchanged");
    };
    auto in=input;auto out=buffers;
    in.pairCount=0;reject(in,buffers,Status::InvalidInput);
    in=input;in.pairCount=0x80000000u;reject(in,buffers,Status::InvalidInput);
    in=input;in.pairCount=0x7fffffffu;reject(in,buffers,Status::CapacityExceeded);
    in=input;in.endpointCount=33;reject(in,buffers,Status::CapacityExceeded);
    in=input;in.endpointSceneCount=33;reject(in,buffers,Status::CapacityExceeded);
    out=buffers;out.capacity=16;reject(input,out,Status::CapacityExceeded);
    out=buffers;out.lengthCapacity=16;reject(input,out,Status::CapacityExceeded);
    in=input;in.endpoints=nullptr;reject(in,buffers,Status::InvalidInput);
    in=input;in.endpointScenes=nullptr;reject(in,buffers,Status::InvalidInput);
    out=buffers;out.worldStarts=nullptr;reject(input,out,Status::InvalidInput);
    out=buffers;out.worldDirections=nullptr;reject(input,out,Status::InvalidInput);
    out=buffers;out.lengths=nullptr;reject(input,out,Status::InvalidInput);
    in=input;in.endpointCount=std::numeric_limits<std::size_t>::max();reject(in,buffers,Status::InvalidInput);
    in=input;in.endpointSceneCount=std::numeric_limits<std::size_t>::max();reject(in,buffers,Status::InvalidInput);
    out=buffers;out.capacity=std::numeric_limits<std::size_t>::max();reject(input,out,Status::InvalidInput);
    out=buffers;out.lengthCapacity=std::numeric_limits<std::size_t>::max();reject(input,out,Status::InvalidInput);
    const auto wrappingAddress=std::numeric_limits<std::uintptr_t>::max()-3;
    in=input;in.endpoints=reinterpret_cast<const SpringVector4_48N*>(wrappingAddress);reject(in,buffers,Status::InvalidInput);
    in=input;in.endpointScenes=reinterpret_cast<const std::uint32_t*>(wrappingAddress);reject(in,buffers,Status::InvalidInput);
    out=buffers;out.worldStarts=reinterpret_cast<SpringVector4_48N*>(wrappingAddress);reject(input,out,Status::InvalidInput);
    out=buffers;out.lengths=reinterpret_cast<float*>(wrappingAddress);reject(input,out,Status::InvalidInput);
    in=input;in.endpoints=reinterpret_cast<const SpringVector4_48N*>(reinterpret_cast<const char*>(input.endpoints)+1);reject(in,buffers,Status::InvalidInput);
    in=input;in.endpointScenes=reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const char*>(input.endpointScenes)+1);reject(in,buffers,Status::InvalidInput);
    out=buffers;out.worldStarts=reinterpret_cast<SpringVector4_48N*>(reinterpret_cast<char*>(out.worldStarts)+1);reject(input,out,Status::InvalidInput);
    out=buffers;out.lengths=reinterpret_cast<float*>(reinterpret_cast<char*>(out.lengths)+1);reject(input,out,Status::InvalidInput);
    out=buffers;out.worldStarts=a.endpoints.data();reject(input,out,Status::AliasedBuffers);
    out=buffers;out.worldStarts=reinterpret_cast<SpringVector4_48N*>(a.scenes.data());reject(input,out,Status::AliasedBuffers);
    out=buffers;out.worldStarts=a.directions.data();reject(input,out,Status::AliasedBuffers);
    out=buffers;out.lengths=reinterpret_cast<float*>(a.starts.data());reject(input,out,Status::AliasedBuffers);
    out=buffers;out.localStarts=reinterpret_cast<Vector3_48C*>(a.starts.data());reject(input,out,Status::AliasedBuffers);
    out=buffers;out.localDirections=reinterpret_cast<Vector3_48C*>(a.starts.data());reject(input,out,Status::AliasedBuffers);
    out=buffers;out.points=a.starts.data();reject(input,out,Status::AliasedBuffers);
    out=buffers;out.fractions=reinterpret_cast<float*>(a.starts.data());reject(input,out,Status::AliasedBuffers);
    out=buffers;out.normals=a.starts.data()+17;reject(input,out,Status::AliasedBuffers); // Full declared tails count.
    out=buffers;out.hits=reinterpret_cast<std::uint32_t*>(&a.state);reject(input,out,Status::AliasedBuffers);
    in=input;in.endpoints=reinterpret_cast<const SpringVector4_48N*>(&a.state);reject(in,buffers,Status::AliasedBuffers);
    a.scenes[33]=1;reject(input,buffers,Status::UnsupportedScope);a.scenes[33]=0;
    const float originalValue=a.endpoints[33].v[0];
    for(float value:{std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::max()}){
        a.endpoints[33].v[0]=value;reject(input,buffers,Status::NonFinite);
    }
    a.endpoints[33].v[0]=originalValue;
    const auto savedEndpoints=a.endpoints;
    for(auto& endpoint:a.endpoints)endpoint={{2e38f,0,0,fp(0x7f812345)}};
    reject(input,buffers,Status::NonFinite); // Bounds midpoint sum overflows after all valid zero-length rays.
    for(unsigned i=0;i<20;++i)a.endpoints[2*i]=a.endpoints[2*i+1]={{i%2?1e20f:-1e20f,0,0,1}};
    reject(input,buffers,Status::NonFinite); // Aggregate half-extents squared overflow.
    a.endpoints=savedEndpoints;
    in=input;in.pairCount=1;a.endpoints[1]=a.endpoints[0];in.sentinel={{fp(0x7fc12345),fp(0xff812345),-0.f,fp(0x7f800000)}};
    watchAllocations=true;status=prepareType3Rays48P(in,buffers,a.state);watchAllocations=false;
    check(status==Status::Complete,"nonpositive length uses raw supplied sentinel");
    check(!std::memcmp(&a.directions[0],&in.sentinel,16)&&!std::memcmp(&a.state.result00,&in.sentinel,16)&&!std::memcmp(&a.state.result10,&in.sentinel,16),"every sentinel lane copied without interpretation");
    a.endpoints=savedEndpoints;out=buffers;out.localStarts=nullptr;out.localDirections=nullptr;out.normals=nullptr;out.points=nullptr;out.hits=nullptr;out.fractions=nullptr;
    watchAllocations=true;status=prepareType3Rays48P(input,out,a.state);watchAllocations=false;
    check(status==Status::Complete,"unused arrays may be omitted");
    const auto before=a;const auto beforeInput=input;out=buffers;out.lengths=reinterpret_cast<float*>(&input);
    watchAllocations=true;status=prepareType3Rays48P(input,out,a.state);watchAllocations=false;
    check(status==Status::AliasedBuffers&&!std::memcmp(&input,&beforeInput,sizeof(input))&&!std::memcmp(&a,&before,sizeof(a)),"output/input descriptor alias rejected");
    out=buffers;out.lengths=reinterpret_cast<float*>(&out);const auto beforeOutput=out;
    watchAllocations=true;status=prepareType3Rays48P(input,out,a.state);watchAllocations=false;
    check(status==Status::AliasedBuffers&&!std::memcmp(&out,&beforeOutput,sizeof(out))&&!std::memcmp(&a,&before,sizeof(a)),"output/buffer descriptor alias rejected");
    const auto start=std::chrono::steady_clock::now();
    watchAllocations=true;for(unsigned i=0;i<10000;++i)if(prepareType3Rays48P(input,buffers,a.state)!=Status::Complete)++failures;watchAllocations=false;
    const double milliseconds=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    check(observedAllocations==0,"no preparation runtime allocations");
    std::cout<<"10000 serial 17-pair preparations: "<<milliseconds<<" ms; allocations="<<observedAllocations<<'\n';
}
}
int main(int argc,char** argv){
    try {
        std::fesetround(FE_TONEAREST);
        const auto root=argc>1?std::filesystem::path(argv[1]):std::filesystem::path(".");
        std::ifstream file(root/"evidence/type3Query48P/native-fixtures.json");if(!file)throw std::runtime_error("missing original query fixtures");
        Json fixture;file>>fixture;if(fixture.at("schema")!=1||fixture.at("array_slots")!=4)throw std::runtime_error("fixture schema");
        for(const auto& c:fixture.at("cases"))original(c);
        safety();check(eligible==196&&unsupported==44&&direct==5,"eligible, unsupported, and direct-only coverage");
        std::cout<<(failures?"FAIL ":"PASS ")<<checks<<" checks; "<<eligible<<" original preparation projections; "<<unsupported<<" unsupported scene-endpoint cases; "<<direct<<" direct-only cases skipped; failures="<<failures<<'\n';
        return failures?1:0;
    }catch(const std::exception& e){std::cerr<<"FAIL "<<label<<": "<<e.what()<<'\n';return 1;}
}
