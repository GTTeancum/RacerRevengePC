#include "candidate_merge48s.h"
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
#if defined(__linux__)
#include <sys/mman.h>
#include <unistd.h>
#endif

static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_NOINLINE __declspec(noinline)
#else
#define RR_NOINLINE __attribute__((noinline))
#endif
RR_NOINLINE void* operator new(std::size_t size){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(size?size:1))return p;
    throw std::bad_alloc();
}
RR_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
RR_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

using namespace rr::game;
using Json=nlohmann::json;
namespace {
std::size_t checks=0,failures=0,operations=0;
std::string label;
void check(bool ok,const char* detail){
    ++checks;if(!ok){++failures;if(failures<30)std::cerr<<label<<": "<<detail<<'\n';}
}
CandidateMergeStatus48S call(const CandidateMergeInput48S& input,
    const CandidateMergeBuffer48S& scratch,const CandidateMergeBuffer48S& destination,
    CandidateMergeResult48S& result){
    watchAllocations=true;
    const auto status=mergeCandidates48S(input,scratch,destination,result);
    watchAllocations=false;++operations;return status;
}
void differential(const Json& row){
    label=row.at("name").get<std::string>();
    const auto first=row.at("first").get<std::vector<std::uint32_t>>();
    const auto second=row.at("second").get<std::vector<std::uint32_t>>();
    const auto oldFirst=first,oldSecond=second;
    const CandidateMergeInput48S input{{row.at("first_count").get<std::int32_t>(),
        first.empty()?nullptr:first.data(),first.size()},
        {row.at("second_count").get<std::int32_t>(),second.empty()?nullptr:second.data(),second.size()}};
    const auto oldInput=input;
    std::array<std::uint32_t,72> scratch{},destination{};
    scratch.fill(0x6d6d6d6d);destination.fill(0x9a9a9a9a);
    for(unsigned i=0;i<64;++i)destination[i]=row.at("before")[i].get<std::uint32_t>();
    const CandidateMergeBuffer48S sb{scratch.data(),scratch.size()},db{destination.data(),destination.size()};
    CandidateMergeResult48S result{-73};
    check(call(input,sb,db,result)==CandidateMergeStatus48S::Complete,"complete isolated merge");
    check(result.count==row.at("expected_count").get<std::int32_t>(),"executed original count");
    for(unsigned i=0;i<64;++i)
        check(destination[i]==row.at("expected")[i].get<std::uint32_t>(),"all original output bits and inactive tail");
    for(std::size_t i=0;i<scratch.size();++i){
        check(scratch[i]==(i<static_cast<std::size_t>(result.count)?destination[i]:0x6d6d6d6d),"scratch reached prefix and untouched tail");
        if(i>=64)check(destination[i]==0x9a9a9a9a,"extra destination tail unchanged");
    }
    check(first==oldFirst&&second==oldSecond,"full source spans unchanged");
    check(std::memcmp(&input,&oldInput,sizeof(input))==0,"input descriptor unchanged");
}
void safety(){
    label="transactional full-declared-span guards";
    std::array<std::uint32_t,96> first{},second{},scratch{},destination{};
    for(std::size_t i=0;i<96;++i){first[i]=static_cast<std::uint32_t>(i);second[i]=static_cast<std::uint32_t>(1000+i);}
    scratch.fill(0xabcdef01);destination.fill(0x12345678);
    CandidateMergeInput48S input{{2,first.data(),first.size()},{3,second.data(),second.size()}};
    CandidateMergeBuffer48S sb{scratch.data(),scratch.size()},db{destination.data(),destination.size()};
    CandidateMergeResult48S result{-19};
    auto reject=[&](const CandidateMergeInput48S& in,const CandidateMergeBuffer48S& s,
                    const CandidateMergeBuffer48S& d,CandidateMergeResult48S& out,CandidateMergeStatus48S expected){
        const auto oldFirst=first,oldSecond=second,oldScratch=scratch,oldDestination=destination;
        const auto oldIn=in;const auto oldS=s,oldD=d;const auto oldOut=out;
        check(call(in,s,d,out)==expected,"guard status");
        check(first==oldFirst&&second==oldSecond,"error preserves source spans");
        check(scratch==oldScratch&&destination==oldDestination,"error preserves all scratch/destination bytes");
        check(std::memcmp(&in,&oldIn,sizeof(in))==0&&std::memcmp(&s,&oldS,sizeof(s))==0&&
              std::memcmp(&d,&oldD,sizeof(d))==0,"error preserves descriptors");
        check(std::memcmp(&out,&oldOut,sizeof(out))==0,"error preserves result");
    };
    const auto invalid=CandidateMergeStatus48S::InvalidInput;
    const auto backing=CandidateMergeStatus48S::InsufficientBacking;
    const auto capacity=CandidateMergeStatus48S::InsufficientCapacity;
    auto in=input;in.first.backingCount=1;reject(in,sb,db,result,backing);
    in=input;in.second.backingCount=2;reject(in,sb,db,result,backing);
    auto s=sb,d=db;s.capacity=4;reject(input,s,d,result,capacity);
    s=sb;d.capacity=4;reject(input,s,d,result,capacity);
    s={nullptr,0};d=db;reject(input,s,d,result,capacity);
    s=sb;d={nullptr,0};reject(input,s,d,result,capacity);
    in=input;in.first={2,nullptr,0};reject(in,sb,db,result,backing);
    in=input;in.second={3,nullptr,1};reject(in,sb,db,result,invalid);
    in=input;in.first={0,nullptr,64};reject(in,sb,db,result,invalid);
    s={nullptr,64};reject(input,s,db,result,invalid);
    d={nullptr,64};reject(input,sb,d,result,invalid);
    const auto maximum=std::numeric_limits<std::size_t>::max();
    in=input;in.first.backingCount=maximum;reject(in,sb,db,result,invalid);
    in=input;in.second.backingCount=maximum;reject(in,sb,db,result,invalid);
    s=sb;s.capacity=maximum;reject(input,s,db,result,invalid);
    d=db;d.capacity=maximum;reject(input,sb,d,result,invalid);
    const auto end=std::numeric_limits<std::uintptr_t>::max()&~std::uintptr_t(3);
    in=input;in.first={0,reinterpret_cast<const std::uint32_t*>(end),2};reject(in,sb,db,result,invalid);
    in=input;in.second={0,reinterpret_cast<const std::uint32_t*>(end),2};reject(in,sb,db,result,invalid);
    s={reinterpret_cast<std::uint32_t*>(end),2};reject(input,s,db,result,invalid);
    d={reinterpret_cast<std::uint32_t*>(end),2};reject(input,sb,d,result,invalid);
    in=input;in.first.identities=reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const char*>(first.data())+1);
    reject(in,sb,db,result,invalid);
    in=input;in.second.identities=reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const char*>(second.data())+1);
    reject(in,sb,db,result,invalid);
    s={reinterpret_cast<std::uint32_t*>(reinterpret_cast<char*>(scratch.data())+1),16};reject(input,s,db,result,invalid);
    d={reinterpret_cast<std::uint32_t*>(reinterpret_cast<char*>(destination.data())+1),16};reject(input,sb,d,result,invalid);
    // Every overlap is rejected over the declared spans, even unused tails.
    s={destination.data()+80,16};reject(input,s,db,result,invalid);
    d={scratch.data()+80,16};reject(input,sb,d,result,invalid);
    s={first.data()+80,16};reject(input,s,db,result,invalid);
    s={second.data()+80,16};reject(input,s,db,result,invalid);
    d={first.data()+80,16};reject(input,sb,d,result,invalid);
    d={second.data()+80,16};reject(input,sb,d,result,invalid);
    s={reinterpret_cast<std::uint32_t*>(&result),1};reject(input,s,db,result,invalid);
    d={reinterpret_cast<std::uint32_t*>(&result),1};reject(input,sb,d,result,invalid);
    in=input;in.first={0,reinterpret_cast<const std::uint32_t*>(&result),1};reject(in,sb,db,result,invalid);
    in=input;in.second={0,reinterpret_cast<const std::uint32_t*>(&result),1};reject(in,sb,db,result,invalid);
    s={reinterpret_cast<std::uint32_t*>(&input),sizeof(input)/4};reject(input,s,db,result,invalid);
    d={reinterpret_cast<std::uint32_t*>(&input),sizeof(input)/4};reject(input,sb,d,result,invalid);
    s=sb;s.identities=reinterpret_cast<std::uint32_t*>(&s);s.capacity=sizeof(s)/4;reject(input,s,db,result,invalid);
    d=db;d.identities=reinterpret_cast<std::uint32_t*>(&d);d.capacity=sizeof(d)/4;reject(input,sb,d,result,invalid);
    s={reinterpret_cast<std::uint32_t*>(&db),sizeof(db)/4};reject(input,s,db,result,invalid);
    d={reinterpret_cast<std::uint32_t*>(&sb),sizeof(sb)/4};reject(input,sb,d,result,invalid);
    // The count word may alias any descriptor word; reject before the write.
    auto& inCount=*reinterpret_cast<CandidateMergeResult48S*>(&input.first.count);
    reject(input,sb,db,inCount,invalid);
    // Read-only spans may share all storage and preserve repeated values.
    in={{3,first.data(),96},{4,first.data(),96}};s=sb;d=db;
    check(call(in,s,d,result)==CandidateMergeStatus48S::Complete&&result.count==7,"shared read-only spans accepted");
    for(unsigned i=0;i<7;++i)check(destination[i]==(i<3?first[i]:first[i-3]),"shared ordered copies");
    // Exact backing, minimal destination/scratch and huge logical counts.
    in={{std::numeric_limits<std::int32_t>::max(),first.data(),64},
        {std::numeric_limits<std::int32_t>::max(),nullptr,0}};
    s={scratch.data(),64};d={destination.data(),64};
    check(call(in,s,d,result)==CandidateMergeStatus48S::Complete&&result.count==64,"INT_MAX count saturated with unbacked second");
    for(unsigned i=0;i<64;++i)check(destination[i]==first[i],"exact first64 prefix");
    in={{63,first.data(),63},{std::numeric_limits<std::int32_t>::max(),second.data(),1}};
    check(call(in,s,d,result)==CandidateMergeStatus48S::Complete&&result.count==64&&destination[63]==second[0],"only one second ID required");
    in.first.backingCount=62;reject(in,s,d,result,backing);
    in={{-1,nullptr,0},{std::numeric_limits<std::int32_t>::min(),nullptr,0}};
    s={nullptr,0};d={nullptr,0};
    check(call(in,s,d,result)==CandidateMergeStatus48S::Complete&&result.count==0,"nonpositive counts null zero spans");
    in={{1,first.data(),1},{1,second.data(),1}};s={scratch.data(),2};d={destination.data(),2};
    check(call(in,s,d,result)==CandidateMergeStatus48S::Complete&&result.count==2,"two-slot caller capacity enough");
}

void protectedLaziness(){
    label="protected-page exact read/write laziness";
#if defined(__linux__)
    const auto page=static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));
    struct Mapping {
        void* data;std::size_t length;
        ~Mapping(){if(data!=MAP_FAILED)::munmap(data,length);}
    } maps[4]{};
    for(auto& map:maps){
        map.length=page*3;map.data=::mmap(nullptr,map.length,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if(map.data==MAP_FAILED)throw std::runtime_error("mmap protected laziness test");
        if(::mprotect(static_cast<char*>(map.data)+page,page,PROT_READ|PROT_WRITE))
            throw std::runtime_error("mprotect protected laziness test");
    }
    auto pointer=[&](unsigned index,unsigned words){return reinterpret_cast<std::uint32_t*>(
        static_cast<char*>(maps[index].data)+page*2)-words;};
    auto* first=pointer(0,37);auto* second=pointer(1,27);
    for(unsigned i=0;i<37;++i)first[i]=i;
    for(unsigned i=0;i<27;++i)second[i]=i+1000;
    CandidateMergeInput48S input{{37,first,37},{10000,second,27}};
    CandidateMergeBuffer48S scratch{pointer(2,64),64},destination{pointer(3,64),64};
    CandidateMergeResult48S result{-1};
    check(call(input,scratch,destination,result)==CandidateMergeStatus48S::Complete&&result.count==64,"guarded exact prefixes complete");
    for(unsigned i=0;i<64;++i)check(destination.identities[i]==(i<37?i:i-37+1000),"guarded raw copies");
    first=pointer(0,64);for(unsigned i=0;i<64;++i)first[i]=i+11;
    input={{1000000,first,64},{1000000,pointer(1,0),0}};
    check(call(input,scratch,destination,result)==CandidateMergeStatus48S::Complete,"inaccessible saturated second list never read");
    input={{-1,pointer(0,0),0},{0,pointer(1,0),0}};
    scratch={pointer(2,0),0};destination={pointer(3,0),0};
    check(call(input,scratch,destination,result)==CandidateMergeStatus48S::Complete&&result.count==0,"all empty inaccessible pointers untouched");
    std::cout<<"candidate_merge_protected_pages=passed\n";
#else
    std::cout<<"candidate_merge_protected_pages=not_run_non_linux\n";
#endif
}
void benchmark(){
    label="bounded serial host cost";
    std::array<std::uint32_t,64> first{},second{},scratch{},destination{};
    for(unsigned i=0;i<64;++i){first[i]=i;second[i]=i+1000;}
    CandidateMergeInput48S input{{32,first.data(),64},{std::numeric_limits<std::int32_t>::max(),second.data(),64}};
    CandidateMergeBuffer48S sb{scratch.data(),64},db{destination.data(),64};CandidateMergeResult48S result;
    constexpr unsigned iterations=20000;std::array<double,9> samples{};
    std::size_t complete=0;const auto before=observedAllocations;
    for(auto& sample:samples){
        watchAllocations=true;const auto start=std::chrono::steady_clock::now();
        for(unsigned i=0;i<iterations;++i)complete+=mergeCandidates48S(input,sb,db,result)==CandidateMergeStatus48S::Complete;
        const auto end=std::chrono::steady_clock::now();watchAllocations=false;
        sample=std::chrono::duration<double,std::nano>(end-start).count()/iterations;
    }
    check(complete==iterations*samples.size()&&result.count==64,"all benchmark merges complete");
    check(observedAllocations==before,"zero allocations in serial benchmark");
    std::sort(samples.begin(),samples.end());
    std::cout<<"candidate_merge_cost_host_ns median="<<samples[4]<<" max_batch_mean="<<samples.back()
        <<" max_copied_ids=64 caller_scratch_bytes="<<sizeof(scratch)<<" caller_destination_bytes="<<sizeof(destination)
        <<" input_descriptor_bytes="<<sizeof(input)<<" buffer_descriptor_bytes="<<sizeof(sb)
        <<" result_bytes="<<sizeof(result)<<" runtime_allocations="<<(observedAllocations-before)
        <<" (host microbenchmark only; no Xbox/game FPS claim)\n";
}
}
int main(int argc,char** argv){
    try{
        if(argc!=2)return 2;
        std::ifstream in(std::filesystem::path(argv[1])/"evidence/candidateMerge48S/native-fixtures.json");
        if(!in)throw std::runtime_error("missing candidateMerge48S/native-fixtures.json");
        Json fixtures;in>>fixtures;const auto& cases=fixtures.at("cases");
        if(cases.size()!=274)throw std::runtime_error("incomplete candidate merge fixture set");
        for(const auto& row:cases)differential(row);
        safety();protectedLaziness();benchmark();
        check(observedAllocations==0,"all watched calls allocation-free");
        std::cout<<"candidate_merge48S cases="<<cases.size()<<" operations="<<operations
            <<" checks="<<checks<<" failures="<<failures<<" runtime_allocations="<<observedAllocations<<'\n';
        return failures?1:0;
    }catch(const std::exception& error){watchAllocations=false;std::cerr<<error.what()<<'\n';return 2;}
}
