#include "body_contact_list48r.h"
#include <array>
#include <cstddef>
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
void check(bool ok,const char* text){
    ++checks;if(!ok){++failures;if(failures<20)std::cerr<<"case "<<caseNumber<<": "<<text<<'\n';}
}
struct Guarded {
    std::array<std::uint32_t,3> before;
    BodyContactListState48R state;
    std::array<std::uint32_t,7> after;
};
static_assert(sizeof(Guarded)==84&&offsetof(Guarded,state)==12&&offsetof(Guarded,after)==56,
    "Exact original fixture span including adjacent state guards");
BodyContactListStatus48R apply(BodyContactIdentity48R id,BodyContactListState48R& state){
    watchAllocations=true;const auto result=applyBodyContactList48R(id,state);watchAllocations=false;return result;
}
Guarded guarded(std::int32_t count){
    Guarded g{};g.before={{0xa5a50001,0x55aa55aa,0x80000000}};
    g.after={{0xffffffff,0,1,0xdeadbeef,0x80000000,0x7fffffff,0xa5a5a5a5}};
    g.state.count20C=count;
    for(std::size_t i=0;i<g.state.slots210.size();++i)g.state.slots210[i].value=0x9000+std::uint32_t(i);
    return g;
}
}
int main(int argc,char** argv){
    if(argc!=2){std::cerr<<"Expected repository root\n";return 2;}
    std::ifstream file(std::filesystem::path(argv[1])/"evidence/bodyContactList48R/body-contact-list-fixtures.bin",std::ios::binary);
    std::uint32_t count=0,width=0;file.read(reinterpret_cast<char*>(&count),4);file.read(reinterpret_cast<char*>(&width),4);
    if(!file||count!=3260||width!=44){std::cerr<<"Missing or malformed original fixtures\n";return 2;}
    std::array<std::size_t,3> outcomes{};
    for(caseNumber=0;caseNumber<count;++caseNumber){
        std::array<std::uint32_t,44> row{};file.read(reinterpret_cast<char*>(row.data()),sizeof(row));
        if(!file||row[43]>2)return 2;
        Guarded g{};std::memcpy(static_cast<void*>(&g),row.data()+1,sizeof(g));
        const BodyContactIdentity48R other{row[0]};
        const auto status=apply(other,g.state);
        const std::array<BodyContactListStatus48R,3> statuses{{BodyContactListStatus48R::Appended,
            BodyContactListStatus48R::Duplicate,BodyContactListStatus48R::Full}};
        check(status==statuses[row[43]],"executed original branch outcome");
        std::array<std::uint32_t,21> got{};std::memcpy(got.data(),&g,sizeof(g));
        for(std::size_t i=0;i<got.size();++i)check(got[i]==row[22+i],"original count/slot bits and untouched adjacent tails");
        check(other.value==row[0],"source identity preserved");++outcomes[row[43]];
    }
    if(file.peek()!=std::ifstream::traits_type::eof())return 2;
    std::ifstream resets(std::filesystem::path(argv[1])/"evidence/bodyContactList48R/body-contact-reset-fixtures.bin",std::ios::binary);
    std::uint32_t resetCount=0,resetWidth=0;
    resets.read(reinterpret_cast<char*>(&resetCount),4);resets.read(reinterpret_cast<char*>(&resetWidth),4);
    if(!resets||resetCount!=1024||resetWidth!=42)return 2;
    for(caseNumber=0;caseNumber<resetCount;++caseNumber){
        std::array<std::uint32_t,42> row{};resets.read(reinterpret_cast<char*>(row.data()),sizeof(row));if(!resets)return 2;
        Guarded g{};std::memcpy(static_cast<void*>(&g),row.data(),sizeof(g));
        watchAllocations=true;resetBodyContactList48R(g.state);watchAllocations=false;
        std::array<std::uint32_t,21> got{};std::memcpy(got.data(),&g,sizeof(g));
        for(std::size_t i=0;i<got.size();++i)check(got[i]==row[21+i],"original lifecycle reset clears count only, every signed count");
    }
    if(resets.peek()!=std::ifstream::traits_type::eof())return 2;
    const std::array<std::int32_t,10> negatives{{-1,-2,-9,-10,-11,-65536,-0x40000000,-0x40000001,
        std::numeric_limits<std::int32_t>::min()+1,std::numeric_limits<std::int32_t>::min()}};
    for(auto countValue:negatives){
        for(auto id:std::array<std::uint32_t,4>{{0,0x10000,0xffffffff,0x80000000}}){
            auto g=guarded(countValue);const auto before=g;
            check(apply({id},g.state)==BodyContactListStatus48R::InvalidInput,"negative/OOB count rejected before index conversion");
            check(std::memcmp(&g,&before,sizeof(g))==0,"negative guard fully transactional");
        }
    }
    // By-value input may be copied from any active or inactive output slot.
    // This includes the precise append destination, without pointer punning.
    for(std::int32_t n=0;n<=10;++n){
        for(std::size_t source=0;source<10;++source){
            auto g=guarded(n);const auto before=g;auto expected=before;
            const auto oldIdentity=g.state.slots210[source];
            const auto status=apply(g.state.slots210[source],g.state);
            const auto target=n==10?BodyContactListStatus48R::Full:
                (source<static_cast<std::size_t>(n)?BodyContactListStatus48R::Duplicate:BodyContactListStatus48R::Appended);
            if(target==BodyContactListStatus48R::Appended){expected.state.slots210[static_cast<std::size_t>(n)]=oldIdentity;++expected.state.count20C;}
            check(status==target,"source/output by-value alias contract");
            check(std::memcmp(&g,&expected,sizeof(g))==0,"aliased source and all unrelated slots preserved");
        }
    }
    for(auto n:std::array<std::int32_t,8>{{10,11,12,65535,0x3fffffff,0x40000000,0x7ffffffe,0x7fffffff}}){
        auto g=guarded(n);const auto before=g;
        check(apply({0},g.state)==BodyContactListStatus48R::Full,"signed full and large overfull count is original no-op");
        check(std::memcmp(&g,&before,sizeof(g))==0,"overfull metadata and tail remain untouched");
    }
    // Sequential reuse fills the fixed capacity exactly. No reset callback or
    // virtual dispatch is invented; assignment here belongs to the test caller.
    std::size_t repeatedAppends=0,repeatedDuplicates=0,repeatedFull=0;
    watchAllocations=true;
    for(std::uint32_t round=0;round<4096;++round){
        auto g=guarded(0);
        for(std::uint32_t i=0;i<10;++i){
            repeatedAppends+=applyBodyContactList48R({i},g.state)==BodyContactListStatus48R::Appended;
            const auto status=applyBodyContactList48R({i},g.state);
            if(i<9)repeatedDuplicates+=status==BodyContactListStatus48R::Duplicate;
            else repeatedFull+=status==BodyContactListStatus48R::Full;
        }
        repeatedFull+=applyBodyContactList48R({0xffffffff},g.state)==BodyContactListStatus48R::Full;
    }
    watchAllocations=false;
    check(repeatedAppends==40960&&repeatedDuplicates==36864&&repeatedFull==8192,"bounded sequential reuse including full-before-dedup order");
    check(observedAllocations==0,"zero runtime allocations across fixtures, errors, aliases and repeated reuse");
    check(outcomes==std::array<std::size_t,3>{{1595,1452,213}},"all original fixture outcome groups present");
    std::cout<<"body_contact_list48r fixtures="<<count<<" checks="<<checks<<" failures="<<failures
        <<" reset_fixtures="<<resetCount<<" allocations="<<observedAllocations<<" state_bytes="<<sizeof(BodyContactListState48R)
        <<" repeated_calls="<<(4096*21)<<'\n';
    return failures?1:0;
}
