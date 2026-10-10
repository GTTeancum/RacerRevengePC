#include "spring_records48n.h"
#include "json.hpp"
#include <algorithm>
#include <array>
#include <chrono>
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
RR_TEST_NOINLINE void* operator new(std::size_t size){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(size?size:1))return p;
    throw std::bad_alloc();
}
RR_TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

using namespace rr::game;
using Json=nlohmann::json;
namespace {
std::size_t checks=0,failures=0,operations=0;
std::string label;
void check(bool value,const char* detail){++checks;if(!value){++failures;if(failures<25)std::cerr<<label<<": "<<detail<<'\n';}}
std::array<std::uint32_t,11> stateWords(const SpringRecordState48N& s){return {
    s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,
    static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
std::int32_t signedWord(std::uint32_t value){std::int32_t out;std::memcpy(&out,&value,4);return out;}
SpringRecordState48N state(const Json& j){
    if(j.size()!=11)throw std::runtime_error("state fixture width");
    return {j[0].get<std::uint32_t>(),j[1].get<std::uint32_t>(),j[2].get<std::uint32_t>(),
        j[3].get<std::uint32_t>(),j[4].get<std::uint32_t>(),j[5].get<std::uint32_t>(),
        j[6].get<std::uint32_t>(),j[7].get<std::uint32_t>(),signedWord(j[8].get<std::uint32_t>()),
        signedWord(j[9].get<std::uint32_t>()),j[10].get<std::uint32_t>()};
}
std::array<std::uint32_t,24> recordWords(const SpringRecord48N& r){
    std::array<std::uint32_t,24> words{};
    std::memcpy(words.data(),&r.field00,64);
    words[16]=r.scene40.value;words[17]=r.other44.value;std::memcpy(&words[18],&r.margin48,4);words[19]=r.material4C;
    const auto* bytes=reinterpret_cast<const std::uint8_t*>(&r)+offsetof(SpringRecord48N,flags50);
    for(unsigned j=0;j<16;++j)words[20+j/4]|=static_cast<std::uint32_t>(bytes[j])<<(8*(j%4));
    return words;
}
SpringRecord48N record(const Json& j){
    if(j.size()!=24)throw std::runtime_error("record fixture width");
    std::array<std::uint32_t,24> words{};for(unsigned i=0;i<24;++i)words[i]=j[i].get<std::uint32_t>();
    SpringRecord48N r{};std::memcpy(&r.field00,words.data(),64);
    r.scene40={words[16]};r.other44={words[17]};std::memcpy(&r.margin48,&words[18],4);r.material4C=words[19];
    auto* bytes=reinterpret_cast<std::uint8_t*>(&r)+offsetof(SpringRecord48N,flags50);
    for(unsigned i=0;i<16;++i)bytes[i]=static_cast<std::uint8_t>(words[20+i/4]>>(8*(i%4)));
    return r;
}
SpringRecordAppend48N input(const Json& j){
    if(j.size()!=19)throw std::runtime_error("append fixture width");
    std::array<std::uint32_t,16> words{};for(unsigned i=0;i<16;++i)words[i]=j[i].get<std::uint32_t>();
    SpringRecordAppend48N out{};std::memcpy(&out.field00,words.data(),64);
    out.scene40={j[16].get<std::uint32_t>()};out.other44={j[17].get<std::uint32_t>()};out.material4C=j[18].get<std::uint32_t>();return out;
}
void differential(const Json& j){
    label=j.at("name").get<std::string>();auto s=state(j.at("initial_state"));
    std::vector<SpringRecord48N> records;for(const auto& r:j.at("initial_records"))records.push_back(record(r));
    for(const auto& operation:j.at("operations")){
        if(operation.at("kind")=="reset"){
            watchAllocations=true;resetSpringRecords48N(s);watchAllocations=false;
        }else{
            const auto a=input(operation.at("input"));const auto old=a;
            watchAllocations=true;const auto result=appendSpringRecord48N(a,records.data(),records.size(),s);watchAllocations=false;
            check(result==(operation.at("status")=="Appended"?SpringRecordStatus48N::Appended:SpringRecordStatus48N::Saturated),"original status");
            check(std::memcmp(&a,&old,sizeof(a))==0,"immutable payload");
        }
        const auto actual=stateWords(s);const auto& expected=operation.at("state");
        for(unsigned i=0;i<actual.size();++i)check(actual[i]==expected[i].get<std::uint32_t>(),"bit-exact state including preserved point count");
        const auto& expectedRecords=operation.at("records");check(expectedRecords.size()==records.size(),"complete backing extent");
        for(std::size_t i=0;i<records.size();++i){
            const auto actualRecord=recordWords(records[i]);
            for(unsigned k=0;k<24;++k)check(actualRecord[k]==expectedRecords[i][k].get<std::uint32_t>(),"all record bits including stale fields and extra slots");
        }
        ++operations;
    }
}
void safety(){
    label="native boundary guards";
    std::array<SpringRecord48N,4> records{};
    // All bytes, including stale flags/margin and fourth lanes, are observable.
    std::memset(records.data(),0xa5,sizeof(records));
    SpringRecordAppend48N a{};a.scene40={99};a.other44={77};a.material4C=0xffffffff;
    const SpringRecordState48N base{11,12,13,14,15,16,17,18,1,4,21};
    auto reject=[&](SpringRecordState48N s,SpringRecord48N* pointer,std::size_t capacity,SpringRecordStatus48N expected){
        const auto before=s;const auto old=records;
        watchAllocations=true;const auto result=appendSpringRecord48N(a,pointer,capacity,s);watchAllocations=false;
        check(result==expected,"guard status");check(std::memcmp(&s,&before,sizeof(s))==0,"guard preserves state");
        check(std::memcmp(records.data(),old.data(),sizeof(records))==0,"guard preserves backing slots");
    };
    auto s=base;reject(s,records.data(),3,SpringRecordStatus48N::InsufficientCapacity);
    s.generatedCount84=4;reject(s,records.data(),3,SpringRecordStatus48N::InsufficientCapacity);
    s.generatedCount84=5;reject(s,records.data(),4,SpringRecordStatus48N::Saturated);
    s=base;s.generatedCount84=-1;reject(s,records.data(),4,SpringRecordStatus48N::InvalidInput);
    s=base;s.generatedCapacity88=-1;reject(s,records.data(),4,SpringRecordStatus48N::InvalidInput);
    reject(base,nullptr,4,SpringRecordStatus48N::InvalidInput);
    reject(base,nullptr,0,SpringRecordStatus48N::InsufficientCapacity);
    reject(base,records.data(),std::numeric_limits<std::size_t>::max(),SpringRecordStatus48N::InvalidInput);
    const auto nearEnd=std::numeric_limits<std::uintptr_t>::max()&~(static_cast<std::uintptr_t>(alignof(SpringRecord48N))-1);
    reject(base,reinterpret_cast<SpringRecord48N*>(nearEnd),4,SpringRecordStatus48N::InvalidInput);
    reject(base,reinterpret_cast<SpringRecord48N*>(reinterpret_cast<std::uint8_t*>(records.data())+1),4,SpringRecordStatus48N::InvalidInput);
    reject(base,reinterpret_cast<SpringRecord48N*>(&a),4,SpringRecordStatus48N::InvalidInput);
    // Guards operate on addresses before touching an overlapping storage slot.
    s=base;const auto before=s;
    check(appendSpringRecord48N(a,reinterpret_cast<SpringRecord48N*>(&s),4,s)==SpringRecordStatus48N::InvalidInput,"storage/state alias");
    check(std::memcmp(&s,&before,sizeof(s))==0,"aliased state unchanged");
    // The inactive union member is used only for its address; the production
    // overlap guard returns before any access through that reference.
    union InputState {SpringRecordAppend48N input;SpringRecordState48N state;};
    InputState alias{};alias.input=a;const auto oldInput=alias.input;
    check(appendSpringRecord48N(alias.input,records.data(),4,alias.state)==SpringRecordStatus48N::InvalidInput,"input/state alias");
    check(std::memcmp(&alias.input,&oldInput,sizeof(a))==0,"aliased source unchanged");
    const auto old=records;
    s=base;
    check(appendSpringRecord48N(*reinterpret_cast<const SpringRecordAppend48N*>(records.data()),records.data(),4,s)==SpringRecordStatus48N::InvalidInput,"input/storage alias");
    check(std::memcmp(records.data(),old.data(),sizeof(records))==0,"alias preserves records");
    s=base;s.generatedCount84=0;s.generatedCapacity88=0;
    check(appendSpringRecord48N(a,nullptr,0,s)==SpringRecordStatus48N::Saturated,"zero capacity permits null storage");
    // Maximum positive logical capacity remains distinct from backing capacity.
    s=base;s.generatedCount84=std::numeric_limits<std::int32_t>::max();s.generatedCapacity88=std::numeric_limits<std::int32_t>::max();
    reject(s,records.data(),4,SpringRecordStatus48N::InsufficientCapacity);
    // Reset intentionally neither validates capacity nor touches any backing slot.
    s=base;s.generatedCount84=-2;s.generatedCapacity88=-7;
    resetSpringRecords48N(s);
    check(s.generatedCount84==0&&s.pending38==1&&s.contact3C==0&&s.upward40==0&&s.other44==0&&s.point74==0&&s.generated78==0&&s.currentCandidate130==0,"exact reset writes");
    check(s.generatedCapacity88==-7&&s.pointCount7C==18&&s.lastMaterialD4==21,"reset preserves persistent fields even if metadata invalid for append");
    check(std::memcmp(records.data(),old.data(),sizeof(records))==0,"reset leaves backing untouched");
}
void benchmark(){
    label="serial allocation/cost";
    std::array<SpringRecord48N,6> records{};SpringRecordState48N s{};s.generatedCapacity88=6;
    SpringRecordAppend48N a{};a.scene40={7};a.other44={13};a.material4C=9;
    std::array<double,9> samples{};constexpr unsigned loops=10000;std::size_t completed=0;
    const auto before=observedAllocations;
    for(auto& sample:samples){
        watchAllocations=true;const auto start=std::chrono::steady_clock::now();
        for(unsigned i=0;i<loops;++i){resetSpringRecords48N(s);for(unsigned j=0;j<records.size();++j)completed+=appendSpringRecord48N(a,records.data(),records.size(),s)==SpringRecordStatus48N::Appended;}
        const auto end=std::chrono::steady_clock::now();watchAllocations=false;
        sample=std::chrono::duration<double,std::nano>(end-start).count()/loops;
    }
    check(completed==samples.size()*loops*records.size(),"benchmark completed serial appends");
    check(observedAllocations==before,"zero per-call allocations");std::sort(samples.begin(),samples.end());
    std::cout<<"spring_records_cost_host_ns median="<<samples[4]<<" max_batch_mean="<<samples[8]
        <<" records_per_call="<<records.size()<<" caller_backing_bytes="<<sizeof(records)
        <<" state_bytes="<<sizeof(s)<<" input_bytes="<<sizeof(a)<<" record_bytes="<<sizeof(SpringRecord48N)
        <<" runtime_allocations="<<(observedAllocations-before)<<" (microbenchmark; no Xbox or game FPS claim)\n";
}
}
int main(int argc,char** argv){
    try{
        if(argc!=2)return 2;
        std::ifstream in(std::filesystem::path(argv[1])/"evidence/springRecords48N/native-fixtures.json");
        if(!in)throw std::runtime_error("missing springRecords48N/native-fixtures.json; run tools/gameplay/spring_records_fixtures48n.py");
        Json fixture;in>>fixture;const auto& cases=fixture.at("cases");if(cases.size()!=264)throw std::runtime_error("incomplete fixture set");
        for(const auto& j:cases)differential(j);
        safety();benchmark();check(observedAllocations==0,"all differential and instrumented safety calls allocate zero");
        std::cout<<"spring_records48N cases="<<cases.size()<<" operations="<<operations<<" checks="<<checks<<" failures="<<failures<<" runtime_allocations="<<observedAllocations<<'\n';
        return failures?1:0;
    }catch(const std::exception& error){watchAllocations=false;std::cerr<<error.what()<<'\n';return 2;}
}
