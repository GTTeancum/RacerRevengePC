// PROPOSAL TEST SOURCE ONLY: not compiled or executed by its author.
// Include this directory and the actual/proposed src/game directory. The
// parent may later compile/run with strict binary32, -fno-fast-math and
// -ffp-contract=off. The test contains no guest execution or external assets.
#include "../../src/game/ai_combat_state50lh.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace {
using namespace rr::game;
unsigned checks=0;
void require(bool condition,const char* message) {
    ++checks;
    if(!condition){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}
}
std::uint32_t bits(float x) {
    std::uint32_t result;std::memcpy(&result,&x,sizeof result);return result;
}
float word(std::uint32_t value) {
    float result;std::memcpy(&result,&value,sizeof result);return result;
}
void requireBits(float actual,std::uint32_t expected,const char* message) {
    require(bits(actual)==expected,message);
}
void requireDiagnostic(const AiPrefixDiagnostic50IV& d,AiPrefixError50IV error,
    std::uint32_t pc,const char* message) {
    require(d.error==error&&d.sourcePC==pc,message);
}

void contactTests() {
    // A genuine nonpositive signed count precedes ALL list/target/profile
    // accesses. Missing target/list and a poison float do not create a miss
    // boundary or force floating-point admission on these paths.
    const float poisonProfile=word(0x7fc12345u);
    for(const auto count : {0u,0xffffffffu,0x80000000u}){
        AiCombatContactView50LH view{};view.count238Bits=count;
        float cooldown=7.25f;bool returned=true;
        AiPrefixDiagnostic50IV d{AiPrefixError50IV::Input,0xdeadbeefu};
        require(checkOriginalAiAttackContact17048050LH(view,poisonProfile,cooldown,returned,d),
            "count<=0 must complete before missing list/target/profile");
        require(!returned,"count<=0 returns source false");
        requireBits(cooldown,bits(7.25f),"count<=0 preserves1018");
        requireDiagnostic(d,AiPrefixError50IV::None,0,"count<=0 clears diagnostic");
    }

    const std::array<BodyContactIdentity48R,4> list{{{0x101u},{0x202u},{0x303u},{0x404u}}};
    const auto listBefore=list;
    const float profile=12.5f;
    // Deliberately expose only the prefix through the matching slot, although
    // source count extends farther. A full-list prevalidation would reject
    // these source-earlier matches incorrectly.
    for(std::size_t hit=0;hit<list.size();++hit){
        AiCombatContactView50LH view{11u,list.data(),hit+1,true,list[hit]};
        float cooldown=-7.f;bool returned=false;AiPrefixDiagnostic50IV d{};
        require(checkOriginalAiAttackContact17048050LH(view,profile,cooldown,returned,d),
            "first/middle/last prefix match must precede later bounds failure");
        require(returned,"matching body340 returns source true");
        requireBits(cooldown,bits(profile),"match copies exact1060 to1018");
        requireDiagnostic(d,AiPrefixError50IV::None,0,"match has no boundary");
        require(view.count238Bits==11u,"contact leaf must not alter238");
    }
    require(std::memcmp(list.data(),listBefore.data(),sizeof list)==0,
        "contact search preserves every23C slot");

    {
        AiCombatContactView50LH view{4u,list.data(),list.size(),true,{0x505u}};
        float cooldown=-0.f;bool returned=true;AiPrefixDiagnostic50IV d{};
        require(checkOriginalAiAttackContact17048050LH(view,poisonProfile,cooldown,returned,d),
            "complete active list without matching identity returns normally");
        require(!returned,"exhausted active list returns source false");
        requireBits(cooldown,0x80000000u,"miss preserves1018 bitwise");
    }
    {
        // A matching retained slot beyond active238 is not part of this scan.
        AiCombatContactView50LH view{2u,list.data(),list.size(),true,list[2]};
        float cooldown=4.f;bool returned=true;AiPrefixDiagnostic50IV d{};
        require(checkOriginalAiAttackContact17048050LH(view,profile,cooldown,returned,d),
            "inactive retained slots must not extend the active scan");
        require(!returned,"matching inactive23C slot must not report a hit");
        requireBits(cooldown,bits(4.f),"inactive-slot match preserves1018");
    }
    {
        AiCombatContactView50LH view{3u,list.data(),2u,true,{0x303u}};
        float cooldown=19.f;bool returned=true;AiPrefixDiagnostic50IV d{};
        require(!checkOriginalAiAttackContact17048050LH(view,profile,cooldown,returned,d),
            "unrepresented reached slot is a boundary, not a miss");
        requireDiagnostic(d,AiPrefixError50IV::Input,0x1704a0u,"late list bounds PC");
        require(returned,"failed contact leaf preserves output receipt");
        requireBits(cooldown,bits(19.f),"late list boundary preserves1018");
    }
    {
        AiCombatContactView50LH view{1u,list.data(),1u,false,{0x101u}};
        float cooldown=19.f;bool returned=false;AiPrefixDiagnostic50IV d{};
        require(!checkOriginalAiAttackContact17048050LH(view,profile,cooldown,returned,d),
            "positive count cannot treat missing target340 as source false");
        requireDiagnostic(d,AiPrefixError50IV::Input,0x1704a4u,"target340 dereference PC");
        require(!returned,"target boundary preserves output receipt");
        requireBits(cooldown,bits(19.f),"target boundary preserves1018");
        // Both owners missing: source reads the list word before target340.
        view.slots23C=nullptr;view.slotCount=0;
        require(!checkOriginalAiAttackContact17048050LH(view,profile,cooldown,returned,d),
            "missing list and target must stop");
        requireDiagnostic(d,AiPrefixError50IV::Input,0x1704a0u,"list precedes target dereference");
    }
    {
        const BodyContactIdentity48R zeroIdentity{0};
        AiCombatContactView50LH view{1u,&zeroIdentity,1u,true,zeroIdentity};
        float cooldown=0.f;bool returned=false;AiPrefixDiagnostic50IV d{};
        require(checkOriginalAiAttackContact17048050LH(view,poisonProfile,cooldown,returned,d),
            "bound opaque identity bits and raw profile copy need no FP arithmetic");
        require(returned,"identity comparison must not add a zero-token predicate");
        requireBits(cooldown,0x7fc12345u,"1704BC copies1060 bits without an arithmetic rewrite");
    }
}

void hitTransitionTests() {
    struct ScaleCase {std::int32_t place;std::uint32_t record;bool multiply;};
    const ScaleCase cases[]={{-1,0,true},{1,0,true},{2,0,true},{3,0,false},
        {9,0,false},{3,1,true},{9,0x80000000u,true},{2,1,true}};
    const float profile=3.25f;
    for(const auto c:cases){
        bool present=true;std::uint32_t state=1;float timer=-17.f;
        AiPrefixDiagnostic50IV d{};
        require(completeOriginalAiAttackHit50LH(present,state,timer,profile,c.place,c.record,d),
            "admitted attack-hit transition must complete");
        require(!present&&state==3,"contact hit clears1008 and sets3");
        requireBits(timer,bits(c.multiply?32.5f:3.25f),
            "signed place<3 OR nonzeroFE4 selects one multiplication by10");
        requireDiagnostic(d,AiPrefixError50IV::None,0,"admitted hit has no boundary");
    }
    {
        // No multiplication means this arm performs only a raw profile copy.
        bool present=true;std::uint32_t state=1;float timer=0.f;
        const float poison=word(0x7fc54321u);AiPrefixDiagnostic50IV d{};
        require(completeOriginalAiAttackHit50LH(present,state,timer,poison,3,0,d),
            "unscaled raw1068 copy does not invent arithmetic");
        require(!present&&state==3,"raw copy follows earlier target/state stores");
        requireBits(timer,0x7fc54321u,"raw1068 copy preserves payload bits");
    }
    {
        // Contact cooldown is an earlier source write than all state1 stores.
        const BodyContactIdentity48R token{0x9876u};
        AiCombatContactView50LH view{1u,&token,1u,true,token};
        const float repairProfile=6.5f,recovery=std::numeric_limits<float>::max();
        float cooldown=-2.f,timer=-3.f;bool returned=false,present=true;
        std::uint32_t state=1;AiPrefixDiagnostic50IV d{};
        require(checkOriginalAiAttackContact17048050LH(view,repairProfile,cooldown,returned,d)&&returned,
            "real contact must commit1018 before hit-transition failure test");
        require(!completeOriginalAiAttackHit50LH(present,state,timer,recovery,2,0,d),
            "overflow at16EC34 must be a native arithmetic boundary");
        requireDiagnostic(d,AiPrefixError50IV::Arithmetic,0x16ec34u,"recovery multiply boundary PC");
        requireBits(cooldown,bits(repairProfile),"later failure must retain earlier1704BC store");
        require(!present&&state==3,"multiply failure must retain target-clear and state3 writes");
        requireBits(timer,bits(recovery),"multiply failure must retain raw16EC04 timer copy");
    }
    {
        bool present=true;std::uint32_t state=1;float timer=-2.f;
        AiPrefixDiagnostic50IV d{};
        require(std::fesetround(FE_DOWNWARD)==0,"set non-nearest rounding for hit boundary");
        const bool complete=completeOriginalAiAttackHit50LH(present,state,timer,profile,1,0,d);
        require(std::fesetround(FE_TONEAREST)==0,"restore nearest after hit rounding check");
        require(!complete,"scaled hit must reject non-nearest arithmetic");
        requireDiagnostic(d,AiPrefixError50IV::RoundingMode,0x16ec34u,"hit rounding boundary PC");
        require(!present&&state==3,"rounding boundary must preserve earlier source writes");
        requireBits(timer,bits(profile),"rounding boundary must preserve unscaled timer store");
    }
}

void scanReturnTests() {
    for(const auto sourceReturn:{1u,2u,0xffffffffu}){
        bool present=true;float field=8.5f;std::uint32_t state=1;
        completeOriginalAiState1Scan50LH(sourceReturn,present,field,state);
        require(present&&state==1,"state1 accepts any nonzero completed scan return");
        requireBits(field,bits(8.5f),"successful scan preserves104C");
    }
    bool present=true;float field=8.5f;std::uint32_t state=1;
    completeOriginalAiState1Scan50LH(0,present,field,state);
    require(!present&&state==2,"zero scan return clears1008 and sets2");
    requireBits(field,0xbf800000u,"zero scan return writes104C=-1, not0");
}

void state3Tests() {
    struct TimerCase {std::uint32_t old,dt,wanted;std::uint32_t state;};
    const TimerCase cases[]={
        {0x40200000u,0x3e000000u,0x40200000u,2u}, //2.5-.125 is computed then discarded.
        {0x00000000u,0x3e800000u,0xbe800000u,3u}, //+0-.25=-.25.
        {0x80000000u,0x3e800000u,0xbe800000u,3u}, //-0-.25=-.25.
        {0xbf000000u,0x3e800000u,0xbf400000u,3u}, //-.5-.25=-.75.
        {0xbe800000u,0xbf800000u,0x3f400000u,3u}, //-.25-(-1)=.75, still3 this call.
        {0x80000000u,0x00000000u,0x80000000u,3u} //-0-(+0) preserves negative zero.
    };
    for(const auto c:cases){
        float timer=word(c.old);std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(advanceOriginalAiState3Timer50LH(word(c.dt),timer,state,d),
            "admitted state3 timer branch must complete");
        require(state==c.state,"state3 original nonpositive/positive branch direction");
        requireBits(timer,c.wanted,"state3 exact stored or preserved timer bits");
        requireDiagnostic(d,AiPrefixError50IV::None,0,"admitted state3 has no boundary");
    }
    {
        // The result is discarded by the positive-timer arm but SUB still
        // executes in the branch delay slot and can leave the native domain.
        float timer=std::numeric_limits<float>::max();const auto before=bits(timer);
        std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(!advanceOriginalAiState3Timer50LH(-std::numeric_limits<float>::max(),timer,state,d),
            "positive state3 arm must not skip discarded subtraction");
        requireDiagnostic(d,AiPrefixError50IV::Arithmetic,0x16ec84u,"discarded delay-subtraction PC");
        require(state==3,"delay-slot failure occurs before FE8=2 store");
        requireBits(timer,before,"discarded-subtraction failure preserves timer");
    }
    {
        float timer=-std::numeric_limits<float>::max();const auto before=bits(timer);
        std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(!advanceOriginalAiState3Timer50LH(std::numeric_limits<float>::max(),timer,state,d),
            "nonpositive timer arithmetic failure must stop before store");
        requireDiagnostic(d,AiPrefixError50IV::Arithmetic,0x16ec84u,"stored delay-subtraction PC");
        require(state==3,"failed nonpositive timer arm preservesFE8");
        requireBits(timer,before,"failed nonpositive timer arm preserves100C");
    }
    {
        float timer=word(0x7fc33333u);std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(!advanceOriginalAiState3Timer50LH(.25f,timer,state,d),
            "unsupported comparison input must not select a substitute branch");
        requireDiagnostic(d,AiPrefixError50IV::Arithmetic,0x16ec78u,"timer comparison domain PC");
        require(state==3,"invalid timer comparison preservesFE8");
        requireBits(timer,0x7fc33333u,"invalid timer comparison preserves100C bits");
    }
    {
        float timer=2.f;std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(!advanceOriginalAiState3Timer50LH(word(0x7fc44444u),timer,state,d),
            "positive timer still consumes actualfactor in its delay slot");
        requireDiagnostic(d,AiPrefixError50IV::Arithmetic,0x16ec84u,"factor consumption PC");
        require(state==3,"unsupportedfactor must block beforeFE8 write");
        requireBits(timer,bits(2.f),"unsupportedfactor preserves100C");
    }
    {
        float timer=2.f;std::uint32_t state=3;AiPrefixDiagnostic50IV d{};
        require(std::fesetround(FE_DOWNWARD)==0,"set non-nearest for state3 boundary");
        const bool complete=advanceOriginalAiState3Timer50LH(.25f,timer,state,d);
        require(std::fesetround(FE_TONEAREST)==0,"restore nearest after state3 rounding check");
        require(!complete,"state3 rejects non-nearest before arithmetic");
        requireDiagnostic(d,AiPrefixError50IV::RoundingMode,0x16ec6cu,"state3 rounding boundary PC");
        require(state==3,"state3 rounding boundary preservesFE8");
        requireBits(timer,bits(2.f),"state3 rounding boundary preserves100C");
    }
}
} //namespace

int main() {
    const int originalRound=std::fegetround();
    require(std::fesetround(FE_TONEAREST)==0,"establish FE_TONEAREST");
    contactTests();hitTransitionTests();scanReturnTests();state3Tests();
    if(originalRound!=-1)require(std::fesetround(originalRound)==0,"restore original rounding mode");
    std::printf("PASS: AI combat state/contact leaf assertions (%u checks)\n",checks);
    return 0;
}
