#include "spring_callback48n.h"
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>

static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_TEST_NOINLINE __attribute__((noinline))
#else
#define RR_TEST_NOINLINE
#endif
RR_TEST_NOINLINE void* operator new(std::size_t n){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(n?n:1))return p;
    throw std::bad_alloc();
}
RR_TEST_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::game;
namespace {
std::size_t checks=0,failures=0,caseNumber=0;
void check(bool condition,const char* message){
    ++checks;if(!condition){++failures;if(failures<20)std::cerr<<"case "<<caseNumber<<": "<<message<<'\n';}
}
std::array<std::uint32_t,11> words(const SpringRecordState48N& s){return {
    s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.currentCandidate130,s.pointCount7C,
    static_cast<std::uint32_t>(s.generatedCount84),static_cast<std::uint32_t>(s.generatedCapacity88),s.lastMaterialD4};}
SpringRecordState48N state(const std::uint32_t* p){return {
    p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],static_cast<std::int32_t>(p[8]),static_cast<std::int32_t>(p[9]),p[10]};}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;
    std::ifstream file(std::filesystem::path(argv[1])/"evidence/springCallback48N/static-callback-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;file.read(reinterpret_cast<char*>(&count),4);file.read(reinterpret_cast<char*>(&width),4);
    if(!file||count!=640||width!=350)return 2;
    std::size_t totalAppended=0,totalSaturated=0,totalSkipped=0;
    for(caseNumber=0;caseNumber<count;++caseNumber){
        std::array<std::uint32_t,350> row{};file.read(reinterpret_cast<char*>(row.data()),sizeof(row));if(!file)return 2;
        std::array<SpringCumulativePair48N,4> pairs{};std::memcpy(pairs.data(),row.data()+31,sizeof(pairs));
        std::array<SpringRecord48N,5> records{};std::memcpy(records.data(),row.data()+95,sizeof(records));
        std::array<char,32> name{};std::memcpy(name.data(),row.data()+23,name.size());
        auto s=state(row.data()+12);
        SpringStaticCallbackInput48N input{pairs.data(),row[10],row[0],row[1],row[2],row[3],row[4],row[5],row[6],{row[7]},row[8]!=0,name.data(),row[9]};
        const auto oldInput=input;const auto oldPairs=pairs;const auto oldName=name;
        SpringStaticCallbackResult48N result{91,92,93};
        watchAllocations=true;
        const auto status=applySpringStaticCallback48N(input,records.data(),row[11],s,result);
        watchAllocations=false;
        check(status==(row[346]?SpringCallbackStatus48N::Skipped:SpringCallbackStatus48N::Complete),"original callback status");
        const auto got=words(s);
        for(unsigned j=0;j<11;++j)check(got[j]==row[215+j],"all state words match original");
        std::array<std::uint32_t,120> recordWords{};std::memcpy(recordWords.data(),records.data(),sizeof(records));
        for(unsigned j=0;j<120;++j)check(recordWords[j]==row[226+j],"all record bits match original, including stale tail");
        if(row[346]){++totalSkipped;check(result.appended==91&&result.saturated==92&&result.material==93,"skipped result untouched");}
        else{
            check(result.appended==row[347]&&result.saturated==row[348]&&result.material==row[349],"callback diagnostics");
            totalAppended+=result.appended;totalSaturated+=result.saturated;
        }
        check(std::memcmp(&input,&oldInput,sizeof(input))==0,"descriptor unchanged");
        check(std::memcmp(pairs.data(),oldPairs.data(),sizeof(pairs))==0,"pair bits unchanged");
        check(name==oldName,"name unchanged");
    }
    if(file.peek()!=std::ifstream::traits_type::eof())return 2;
    std::array<SpringRecord48N,6> records{};std::memset(records.data(),0xa5,sizeof(records));
    const auto originalRecords=records;
    std::array<SpringCumulativePair48N,2> pairs{};
    const char good[]="trackfr7.col";
    SpringStaticCallbackInput48N base{pairs.data(),2,17,4,0,8,2,4,2,{37},true,good,sizeof(good)};
    const SpringRecordState48N initial{11,12,13,14,15,16,18,19,1,5,21};
    const SpringStaticCallbackResult48N sentinel{123,456,789};
    auto reject=[&](const SpringStaticCallbackInput48N& in,SpringRecordState48N s,SpringRecord48N* r,std::size_t n,SpringCallbackStatus48N expected){
        const auto old=s;auto result=sentinel;
        watchAllocations=true;const auto status=applySpringStaticCallback48N(in,r,n,s,result);watchAllocations=false;
        check(status==expected,"safety status");check(words(s)==words(old),"safety state unchanged");
        check(std::memcmp(records.data(),originalRecords.data(),sizeof(records))==0,"safety records unchanged");
        check(result.appended==sentinel.appended&&result.saturated==sentinel.saturated&&result.material==sentinel.material,"safety result unchanged");
    };
    auto in=base;in.candidateCategoryA4=12;in.queryTypeTag=3;in.pairs=nullptr;in.sceneName=nullptr;
    reject(in,initial,nullptr,0,SpringCallbackStatus48N::Skipped); // skip precedes all unsupported/invalid object reads
    in=base;in.candidateCategoryA4=6;reject(in,initial,records.data(),6,SpringCallbackStatus48N::UnsupportedScope);
    for(auto tag:{0u,1u,3u,5u,6u}){in=base;in.queryTypeTag=tag;reject(in,initial,records.data(),6,SpringCallbackStatus48N::UnsupportedScope);}
    in=base;in.candidateIdentity=0;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.pairs=nullptr;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.pairCount=std::numeric_limits<std::size_t>::max();reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.pairs=reinterpret_cast<const SpringCumulativePair48N*>(reinterpret_cast<const char*>(pairs.data())+1);
    reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.sceneName=nullptr;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.sceneNameBytes=0;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.sceneName="roadfr7";in.sceneNameBytes=7;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    for(const char* shortName:{"","ab","a.fr7"}){in=base;in.sceneName=shortName;in.sceneNameBytes=std::strlen(shortName)+1;
        reject(in,initial,records.data(),6,SpringCallbackStatus48N::UnsupportedScope);}
    reject(base,initial,nullptr,5,SpringCallbackStatus48N::InvalidInput);
    reject(base,initial,records.data(),4,SpringCallbackStatus48N::InsufficientCapacity);
    auto s=initial;s.generatedCount84=-1;reject(base,s,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    s=initial;s.generatedCapacity88=-1;reject(base,s,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    s=initial;s.generatedCount84=8;reject(base,s,records.data(),4,SpringCallbackStatus48N::InsufficientCapacity);
    reject(base,initial,records.data(),std::numeric_limits<std::size_t>::max(),SpringCallbackStatus48N::InvalidInput);
    reject(base,initial,reinterpret_cast<SpringRecord48N*>(reinterpret_cast<char*>(records.data())+1),5,SpringCallbackStatus48N::InvalidInput);
    const auto end=std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(3);
    reject(base,initial,reinterpret_cast<SpringRecord48N*>(end),5,SpringCallbackStatus48N::InvalidInput);
    in=base;in.pairs=reinterpret_cast<const SpringCumulativePair48N*>(end);reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.sceneName=reinterpret_cast<const char*>(end);in.sceneNameBytes=32;reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.pairs=reinterpret_cast<const SpringCumulativePair48N*>(records.data());reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;in.sceneName=reinterpret_cast<const char*>(records.data());reject(in,initial,records.data(),6,SpringCallbackStatus48N::InvalidInput);
    in=base;reject(in,initial,reinterpret_cast<SpringRecord48N*>(&in),6,SpringCallbackStatus48N::InvalidInput);
    // Address checks reject overlapping state/result before mutation.
    s=initial;auto result=sentinel;
    check(applySpringStaticCallback48N(base,reinterpret_cast<SpringRecord48N*>(&s),6,s,result)==SpringCallbackStatus48N::InvalidInput,"state/storage overlap");
    check(words(s)==words(initial),"overlapping state untouched");
    auto* resultAlias=static_cast<SpringStaticCallbackResult48N*>(static_cast<void*>(&s));
    check(applySpringStaticCallback48N(base,records.data(),6,s,*resultAlias)==SpringCallbackStatus48N::InvalidInput,"state/result overlap");
    check(words(s)==words(initial),"result alias state untouched");
    check(applySpringStaticCallback48N(base,records.data(),6,s,*reinterpret_cast<SpringStaticCallbackResult48N*>(records.data()))==SpringCallbackStatus48N::InvalidInput,"record/result overlap");
    check(std::memcmp(records.data(),originalRecords.data(),sizeof(records))==0,"aliased records untouched");
    // Repeated static callbacks intentionally re-append every cumulative pair.
    s=initial;s.generatedCount84=0;s.currentCandidate130=0;result={};
    check(applySpringStaticCallback48N(base,records.data(),6,s,result)==SpringCallbackStatus48N::Complete,"first callback");
    check(s.currentCandidate130==17&&s.generatedCount84==2&&s.lastMaterialD4==7,"first callback state");
    in=base;in.candidateIdentity=19;in.sceneName="trackfr3";in.sceneNameBytes=9;
    check(applySpringStaticCallback48N(in,records.data(),6,s,result)==SpringCallbackStatus48N::Complete,"second callback");
    check(s.currentCandidate130==17&&s.generatedCount84==4&&s.lastMaterialD4==3,"earlier static identity persists, material follows append");
    check(applySpringStaticCallback48N(in,records.data(),6,s,result)==SpringCallbackStatus48N::Complete,"partially saturated callback");
    check(s.generatedCount84==5&&result.appended==1&&result.saturated==1,"original saturation is successful");
    check(std::memcmp(&records[5],&originalRecords[5],sizeof(records[5]))==0,"canary beyond logical capacity");
    s.generatedCapacity88=0;s.generatedCount84=0;in=base;in.pairCount=0;in.pairs=nullptr;
    in.hasMaterialScene=false;in.sceneName=nullptr;in.sceneNameBytes=999;
    check(applySpringStaticCallback48N(in,nullptr,0,s,result)==SpringCallbackStatus48N::Complete,"zero pairs/storage and ignored name");
    check(result.appended==0&&result.saturated==0&&result.material==0,"empty callback diagnostics");
    // All hot-call data are caller-owned stack arrays. Count allocations across
    // a serial reuse run; this timing is a host component observation only.
    constexpr std::size_t repetitions=200000;
    const auto start=std::chrono::steady_clock::now();watchAllocations=true;
    for(std::size_t i=0;i<repetitions;++i){s=initial;s.generatedCount84=0;applySpringStaticCallback48N(base,records.data(),6,s,result);}
    watchAllocations=false;const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    check(observedAllocations==0,"zero hot-path allocations");
    std::cout<<"{\"cases\":"<<count<<",\"checks\":"<<checks<<",\"failures\":"<<failures<<",\"appended\":"<<totalAppended
        <<",\"saturated\":"<<totalSaturated<<",\"skipped\":"<<totalSkipped<<",\"runtime_allocations\":"<<observedAllocations
        <<",\"serial_repetitions\":"<<repetitions<<",\"serial_milliseconds\":"<<elapsed<<"}\n";
    return failures?1:0;
}
