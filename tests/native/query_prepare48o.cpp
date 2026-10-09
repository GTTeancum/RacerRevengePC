#include "query_prepare48o.h"
#include "json.hpp"
#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <string>
#include <vector>

static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_TEST_NOINLINE __attribute__((noinline))
#else
#define RR_TEST_NOINLINE
#endif
RR_TEST_NOINLINE void* operator new(std::size_t size){if(watchAllocations)++observedAllocations;if(void* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
RR_TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
using Json=nlohmann::json;
namespace {
std::size_t checks=0,failures=0;
std::string label;
void check(bool value,const char* detail){++checks;if(!value){++failures;if(failures<25)std::cerr<<label<<": "<<detail<<'\n';}}
std::uint32_t bits(float f){std::uint32_t value;std::memcpy(&value,&f,4);return value;}
float fromBits(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
void loadWords(void* dest,const Json& j,std::size_t n){if(j.size()!=n)throw std::runtime_error("fixture width");auto* p=static_cast<unsigned char*>(dest);for(std::size_t i=0;i<n;++i){const auto u=j[i].get<std::uint32_t>();std::memcpy(p+i*4,&u,4);}}
void equalWords(const void* p,const Json& j,std::size_t n,const char* detail){check(j.size()==n,"expected width");const auto* data=static_cast<const unsigned char*>(p);for(std::size_t i=0;i<n;++i){std::uint32_t word;std::memcpy(&word,data+4*i,4);check(word==j[i].get<std::uint32_t>(),detail);}}
void staging(const Json& j){
    label=j.at("name").get<std::string>();const auto& a=j.at("input");const auto& e=j.at("output");
    std::array<SpringVector4_48N,32> source{},dest{};std::array<std::uint32_t,32> sourceAux{},destAux{};
    loadWords(source.data(),a.at("source"),128);loadWords(dest.data(),a.at("initial_dest"),128);
    loadWords(sourceAux.data(),a.at("source_aux"),32);loadWords(destAux.data(),a.at("initial_aux"),32);
    const auto oldSource=source;const auto oldAux=sourceAux;
    struct QueryFields {std::uint32_t tag;std::array<std::uint32_t,4> center;std::uint32_t radius;} q{};
    q.tag=a.at("tag").get<std::uint32_t>();loadWords(q.center.data(),a.at("stale_center"),4);q.radius=a.at("stale_radius").get<std::uint32_t>();
    QueryPairStagingInput48O input;input.shapeLinked=a.at("shape_linked").get<bool>();input.pairCount=a.at("count").get<std::uint32_t>();
    input.source=source.data();input.sourceCapacity=source.size();input.sourceMetadata=a.at("has_aux").get<bool>()?sourceAux.data():nullptr;input.sourceMetadataCapacity=sourceAux.size();
    watchAllocations=true;const auto status=stageQueryPairs48O(input,dest.data(),dest.size(),destAux.data(),destAux.size(),q.tag);watchAllocations=false;
    check(status==(input.shapeLinked?QueryPairStagingStatus48O::Staged:QueryPairStagingStatus48O::NoShape),"original branch outcome");
    check(q.tag==e.at("tag").get<std::uint32_t>(),"original tag3");
    equalWords(dest.data(),e.at("dest"),128,"exact full payload and untouched tails");equalWords(destAux.data(),e.at("aux"),32,"exact metadata and tails");
    equalWords(q.center.data(),e.at("stale_center"),4,"stale center unchanged");check(q.radius==e.at("stale_radius").get<std::uint32_t>(),"stale radius unchanged");
    check(!std::memcmp(&source,&oldSource,sizeof(source))&&!std::memcmp(&sourceAux,&oldAux,sizeof(sourceAux)),"immutable source spans");
}
QuerySpherePrepareInput48O sphereInput(const Json& a){
    QuerySpherePrepareInput48O input;loadWords(&input.preparedScene,a.at("scene"),16);
    for(unsigned i=0;i<4;++i){input.first.v[i]=fromBits(a.at("pair")[i].get<std::uint32_t>());input.second.v[i]=fromBits(a.at("pair")[4+i].get<std::uint32_t>());}
    loadWords(&input.treeCenter,a.at("tree_center"),3);loadWords(&input.treeHalf,a.at("tree_half"),3);input.hasPairTree=a.at("has_tree").get<bool>();return input;
}
void sphere(const Json& j){
    label=j.at("name").get<std::string>();const auto& a=j.at("input");const auto& e=j.at("output");auto input=sphereInput(a);
    QuerySpherePrepareOutput48O output;loadWords(&output.world,a.at("initial_world"),16);loadWords(&output.center,a.at("initial_center"),4);output.radius=fromBits(a.at("initial_radius").get<std::uint32_t>());
    watchAllocations=true;const auto status=prepareQuerySphere48O(input,output);watchAllocations=false;
    check(status==QuerySpherePrepareStatus48O::Ready,"original numeric boundary");equalWords(&output.world,e.at("world"),16,"exact copied matrix");
    equalWords(&output.center,e.at("center"),4,"exact transformed center including W");check(bits(output.radius)==e.at("radius").get<std::uint32_t>(),"exact local radius");
}
void stagingSafety(){
    label="staging transactional guards";std::array<SpringVector4_48N,36> source{},dest{};std::array<std::uint32_t,36> aux{},destAux{};
    std::memset(source.data(),0xa5,sizeof(source));std::memset(dest.data(),0x7b,sizeof(dest));destAux.fill(0x12345678);
    QueryPairStagingInput48O input;input.pairCount=17;input.source=source.data();input.sourceCapacity=source.size();input.sourceMetadata=aux.data();input.sourceMetadataCapacity=aux.size();
    std::uint32_t tag=2;watchAllocations=true;auto status=stageQueryPairs48O(input,dest.data(),dest.size(),destAux.data(),destAux.size(),tag);watchAllocations=false;
    check(status==QueryPairStagingStatus48O::Staged&&tag==3,"17 pairs allowed; no invented16 cap");
    check(!std::memcmp(source.data(),dest.data(),34*sizeof(source[0]))&&destAux[34]==0x12345678,"caller-bounded variable extent preserves tails");
    const auto oldDest=dest;const auto oldAux=destAux;const auto oldSource=source;const auto oldSourceAux=aux;
    auto reject=[&](QueryPairStagingInput48O in,SpringVector4_48N* dp,std::size_t dc,std::uint32_t* ap,std::size_t ac,QueryPairStagingStatus48O expected){
        std::array<unsigned char,sizeof(in)> oldInput{};std::memcpy(oldInput.data(),&in,sizeof(in));
        tag=7;watchAllocations=true;auto result=stageQueryPairs48O(in,dp,dc,ap,ac,tag);watchAllocations=false;
        check(result==expected,"expected preflight failure");check(tag==7&&!std::memcmp(&dest,&oldDest,sizeof(dest))&&!std::memcmp(&destAux,&oldAux,sizeof(destAux)),"complete transactional rejection");
        check(!std::memcmp(&source,&oldSource,sizeof(source)),"rejection preserves every source-vector byte");
        check(!std::memcmp(&aux,&oldSourceAux,sizeof(aux)),"rejection preserves every source-metadata byte");
        check(!std::memcmp(&in,oldInput.data(),sizeof(in)),"rejection preserves complete input descriptor bytes");
    };
    reject(input,dest.data(),33,destAux.data(),36,QueryPairStagingStatus48O::InsufficientCapacity);
    reject(input,dest.data(),36,destAux.data(),33,QueryPairStagingStatus48O::InsufficientCapacity);
    auto changed=input;changed.sourceCapacity=33;reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InsufficientCapacity);
    changed=input;changed.sourceMetadataCapacity=33;reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InsufficientCapacity);
    changed=input;changed.source=nullptr;reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    reject(input,nullptr,36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    reject(input,dest.data(),36,nullptr,36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;changed.source=dest.data();reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;changed.source=reinterpret_cast<const SpringVector4_48N*>(reinterpret_cast<const char*>(source.data())+1);reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    reject(input,reinterpret_cast<SpringVector4_48N*>(reinterpret_cast<char*>(dest.data())+1),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    reject(input,dest.data(),36,reinterpret_cast<std::uint32_t*>(dest.data()),36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;changed.sourceMetadata=destAux.data();reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;changed.sourceCapacity=std::numeric_limits<std::size_t>::max();reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    reject(input,dest.data(),std::numeric_limits<std::size_t>::max(),destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;changed.pairCount=0x40000000;reject(changed,dest.data(),36,destAux.data(),36,QueryPairStagingStatus48O::InvalidInput);
    changed=input;std::array<unsigned char,sizeof(changed)> beforeTagAlias{};std::memcpy(beforeTagAlias.data(),&changed,sizeof(changed));
    watchAllocations=true;status=stageQueryPairs48O(changed,dest.data(),36,destAux.data(),36,changed.pairCount);watchAllocations=false;check(status==QueryPairStagingStatus48O::InvalidInput&&changed.pairCount==17,"input/tag overlap rejected");
    check(!std::memcmp(&changed,beforeTagAlias.data(),sizeof(changed))&&!std::memcmp(&dest,&oldDest,sizeof(dest))&&
        !std::memcmp(&destAux,&oldAux,sizeof(destAux))&&!std::memcmp(&source,&oldSource,sizeof(source))&&
        !std::memcmp(&aux,&oldSourceAux,sizeof(aux)),"input/tag rejection preserves descriptor and all backing bytes");
    changed={};changed.shapeLinked=false;changed.pairCount=0xffffffff;tag=2;
    watchAllocations=true;status=stageQueryPairs48O(changed,nullptr,123,nullptr,456,tag);watchAllocations=false;check(status==QueryPairStagingStatus48O::NoShape&&tag==3,"null shape does not consume malformed unused spans/count");
    changed.shapeLinked=true;changed.pairCount=0;tag=2;
    watchAllocations=true;status=stageQueryPairs48O(changed,nullptr,123,nullptr,456,tag);watchAllocations=false;check(status==QueryPairStagingStatus48O::Staged&&tag==3,"zero pairs do not consume unused spans");
    // Deliberately exceed the fixture count by an order of magnitude.
    std::vector<SpringVector4_48N> manySource(2048),manyDest(2048);std::vector<std::uint32_t> manyAux(2048,99);
    changed={};changed.pairCount=1024;changed.source=manySource.data();changed.sourceCapacity=manySource.size();tag=2;
    watchAllocations=true;status=stageQueryPairs48O(changed,manyDest.data(),manyDest.size(),manyAux.data(),manyAux.size(),tag);watchAllocations=false;
    check(status==QueryPairStagingStatus48O::Staged&&manyAux.front()==0&&manyAux.back()==0,"1024 pair caller-storage count, zero runtime allocation");
}
void stagingAliasing(){
    label="staging full-span aliases";
    std::array<SpringVector4_48N,72> vectors{};std::array<std::uint32_t,72> metadata{};
    std::memset(vectors.data(),0x59,sizeof(vectors));metadata.fill(0xa1b2c3d4);
    struct DescriptorBacking {QueryPairStagingInput48O input{};std::array<unsigned char,64> tail{};} descriptor;
    auto& input=descriptor.input;input.pairCount=1;input.source=vectors.data();input.sourceCapacity=36;
    input.sourceMetadata=metadata.data();input.sourceMetadataCapacity=36;
    std::uint32_t tag=7;
    auto reject=[&](SpringVector4_48N* dest,std::size_t capacity,std::uint32_t* destMeta,std::size_t metaCapacity){
        const auto oldVectors=vectors;const auto oldMetadata=metadata;
        std::array<unsigned char,sizeof(descriptor)> oldInput{};std::memcpy(oldInput.data(),&descriptor,sizeof(descriptor));
        watchAllocations=true;const auto status=stageQueryPairs48O(input,dest,capacity,destMeta,metaCapacity,tag);watchAllocations=false;
        check(status==QueryPairStagingStatus48O::InvalidInput,"full declared-span overlap rejected");
        check(!std::memcmp(&vectors,&oldVectors,sizeof(vectors)),"all vector-backing bytes unchanged after alias rejection");
        check(!std::memcmp(&metadata,&oldMetadata,sizeof(metadata)),"all metadata-backing bytes unchanged after alias rejection");
        check(!std::memcmp(&descriptor,oldInput.data(),sizeof(descriptor)),"all descriptor-backing bytes unchanged after alias rejection");
        check(tag==7,"tag unchanged after alias rejection");
    };
    // Only the source's declared unused tail overlaps destination: the two
    // actively copied source vectors and destination vectors are disjoint.
    input.sourceCapacity=37;reject(vectors.data()+36,36,metadata.data()+36,36);input.sourceCapacity=36;
    input.sourceMetadataCapacity=37;reject(vectors.data()+36,36,metadata.data()+36,36);input.sourceMetadataCapacity=36;
    // Metadata destination lies in the vector destination's unused tail.
    reject(vectors.data()+36,36,reinterpret_cast<std::uint32_t*>(vectors.data()+71),4);
    // These deliberately invalid destination views lie fully within descriptor
    // backing on both 32-/64-bit hosts. Reject before dereferencing either view.
    reject(reinterpret_cast<SpringVector4_48N*>(&input),2,metadata.data()+36,36);
    reject(vectors.data()+36,36,reinterpret_cast<std::uint32_t*>(&input),2);
}
void sphereSafety(){
    label="sphere guards and unused inputs";QuerySpherePrepareInput48O input;for(unsigned i=0;i<4;++i)input.preparedScene.v[i*5]=1;
    input.first.v[1]=3;input.second.v[1]=-28;QuerySpherePrepareOutput48O sentinel{};
    std::memset(static_cast<void*>(&sentinel),0x5a,sizeof(sentinel));
    auto reject=[&](const QuerySpherePrepareInput48O& in,QuerySpherePrepareStatus48O expected){auto out=sentinel;watchAllocations=true;auto status=prepareQuerySphere48O(in,out);watchAllocations=false;check(status==expected,"expected sphere guard");check(!std::memcmp(&out,&sentinel,sizeof(out)),"sphere transactional output");};
    auto changed=input;changed.shapeType=3;reject(changed,QuerySpherePrepareStatus48O::UnsupportedScope);
    changed=input;changed.scenePrepared=false;reject(changed,QuerySpherePrepareStatus48O::UnsupportedScope);
    changed=input;changed.shapeLinked=false;reject(changed,QuerySpherePrepareStatus48O::UnsupportedScope);
    changed=input;changed.pairCount=2;reject(changed,QuerySpherePrepareStatus48O::UnsupportedScope);
    changed=input;changed.preparedScene.v[3]=1;reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.preparedScene.v[0]=std::numeric_limits<float>::infinity();reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.first.v[0]=std::numeric_limits<float>::quiet_NaN();reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.first.v[0]=std::numeric_limits<float>::max();changed.second.v[0]=std::numeric_limits<float>::max();reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.hasPairTree=true;changed.treeHalf.v[0]=std::numeric_limits<float>::max();reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.hasPairTree=true;changed.treeCenter.v[0]=std::numeric_limits<float>::max();changed.preparedScene.v[0]=2;reject(changed,QuerySpherePrepareStatus48O::InvalidInput);
    changed=input;changed.first.v[3]=std::numeric_limits<float>::quiet_NaN();changed.second.v[3]=std::numeric_limits<float>::infinity();changed.treeCenter.v[0]=std::numeric_limits<float>::quiet_NaN();
    auto out=sentinel;watchAllocations=true;auto status=prepareQuerySphere48O(changed,out);watchAllocations=false;check(status==QuerySpherePrepareStatus48O::Ready&&out.radius==15.5f,"unused W/tree values not validated");
    changed=input;changed.hasPairTree=true;changed.pairCount=0xffffffff;changed.first.v[0]=std::numeric_limits<float>::quiet_NaN();changed.second.v[0]=std::numeric_limits<float>::infinity();changed.treeHalf.v[0]=-3;changed.treeHalf.v[1]=4;
    watchAllocations=true;status=prepareQuerySphere48O(changed,out);watchAllocations=false;check(status==QuerySpherePrepareStatus48O::Ready&&out.radius==5,"root tree ignores pair count/endpoint fields; signed half preserved");
}
}
int main(int argc,char** argv){try{
    if(argc!=2)return 2;
    std::ifstream file(std::filesystem::path(argv[1])/"evidence/queryPrepare48O/fixtures.json");if(!file)throw std::runtime_error("missing queryPrepare48O fixtures");
    Json fixture;file>>fixture;const auto& sc=fixture.at("staging");const auto& sp=fixture.at("sphere");if(sc.size()!=84||sp.size()!=248)throw std::runtime_error("incomplete query fixture set");
    for(const auto& j:sc)staging(j);
    for(const auto& j:sp)sphere(j);
    stagingSafety();stagingAliasing();sphereSafety();check(observedAllocations==0,"all instrumented runtime calls allocation-free");
    std::cout<<"query_prepare48O staging_cases="<<sc.size()<<" sphere_cases="<<sp.size()<<" checks="<<checks<<" failures="<<failures<<" runtime_allocations="<<observedAllocations
        <<" staging_input_bytes="<<sizeof(QueryPairStagingInput48O)<<" sphere_input_bytes="<<sizeof(QuerySpherePrepareInput48O)<<" sphere_output_bytes="<<sizeof(QuerySpherePrepareOutput48O)<<'\n';return failures?1:0;
}catch(const std::exception& error){watchAllocations=false;std::cerr<<error.what()<<'\n';return 2;}}
