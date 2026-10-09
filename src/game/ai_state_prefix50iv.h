#pragma once
#include "ai_speed_braking50ir.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {
// Source-only native leaves. No build/runtime validation. Every supplied field
// names its original owner; no manager, track, timer, scene or target is invented.
// Strict binary32/FE_TONEAREST/no contraction; borrowed owners must remain stable.
// bool failure preserves state/output. The state dispatcher additionally returns
// a valid Boundary receipt when the next actual source operation is unrepresented.
enum class AiPrefixError50IV : std::uint8_t { None, RoundingMode, Input, Arithmetic };
struct AiPrefixDiagnostic50IV { AiPrefixError50IV error=AiPrefixError50IV::None; std::uint32_t sourcePC=0; };
namespace ai_prefix_detail50IV {
using ai_speed_detail50IR::word;
inline bool fail(AiPrefixDiagnostic50IV& d,AiPrefixError50IV e,std::uint32_t pc) noexcept {d={e,pc};return false;}
struct Math : ai_speed_detail50IR::Math {
    float read(float v) noexcept {return keep(v);}
    float add(float a,float b) noexcept {read(a);read(b);return ai_speed_detail50IR::Math::add(a,b);}
    float sub(float a,float b) noexcept {read(a);read(b);return ai_speed_detail50IR::Math::sub(a,b);}
    float mul(float a,float b) noexcept {read(a);read(b);return ai_speed_detail50IR::Math::mul(a,b);}
    float div(float a,float b) noexcept {read(a);read(b);return ai_speed_detail50IR::Math::div(a,b);}
    void horizontal(float (&v)[3],const float (&fallback433C70)[3]) noexcept {
        v[1]=0.f;
        const float x=mul(v[0],v[0]),y=mul(v[1],v[1]);
        const float acc=add(x,y),norm=add(acc,mul(v[2],v[2]));
        if(norm==0.f){for(unsigned j=0;j<3;++j)v[j]=read(fallback433C70[j]);return;}
        if(!valid||norm<0.f||!std::isnormal(norm)){valid=false;return;}
        // Same bounded scalar RSQRT convention as retained wrong_way_progress50GO:
        // rounded binary32 root then rounded divide; no guest/VU emulation.
        volatile float root=static_cast<float>(std::sqrt(static_cast<double>(norm)));
        const float reciprocal=div(1.f,read(root));
        for(float& x:v)x=mul(x,reciprocal);
    }
    float dot(const float (&a)[3],const float (&b)[3]) noexcept {
        const float x=mul(a[0],b[0]),y=mul(a[1],b[1]),z=mul(a[2],b[2]);
        const float one=add(0.f,1.f),acc=add(x,y);return add(acc,mul(one,z));
    }
    float angle(float input) noexcept {
        const float a=std::fabs(read(input));
        if(input>1.f)return 0.f;if(input< -1.f)return word(0x40490fdbu);
        float t=add(word(0xc372b84fu),mul(word(0x42a8a11bu),a));
        t=add(word(0x43836812u),mul(t,a));t=add(word(0xc3031cc3u),mul(a,t));
        t=add(word(0x41ed4ad5u),mul(a,t));t=add(word(0xbfb9d5d7u),mul(a,t));
        t=add(word(0x3d06871eu),mul(a,t));
        return input<0.f?add(word(0x3fc90fdbu),t):sub(word(0x3fc90fdbu),t);
    }
};
inline bool nearest(AiPrefixDiagnostic50IV& d,std::uint32_t pc) noexcept {
    d={};return std::fegetround()==FE_TONEAREST||fail(d,AiPrefixError50IV::RoundingMode,pc);
}
}
struct AiStatePrefixState50IV {
    std::uint32_t aiStateFE8,orientation10A0;
    float stallTimer1090,wrongWayTimer1094;
    std::int32_t turnSide1098;
};
// At16E260 construction or16E830 reset only, not a per-frame substitute.
inline void initializeAiStatePrefix50IV(AiStatePrefixState50IV& s) noexcept {s={0,0,1.5f,0.f,0};}
struct AiOrientationInput50IV {
    std::uint8_t applicationFlags58,vehicleFlagsCD8;
    std::uint32_t vehicleState40;
    float preparedFactor;
    float localForward340_288[3]; //28A530 first output, local E0/E4/E8.
    float currentC80Position[3],previousC84Position[3]; //Actual app28C->BC track.
    float effectiveFallback433C70[3]; //Read actual initialized global; no guessed axis.
};
struct AiOrientationReceipt50IV {bool evaluated=false;float directionDot=0.f,sourceAngle=0.f;};
inline bool updateAiOrientation50IV(const AiOrientationInput50IV& i,AiStatePrefixState50IV& state,
    AiOrientationReceipt50IV& out,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x1739d0))return false;
    auto n=state;AiOrientationReceipt50IV r{};n.orientation10A0=0;
    if((i.applicationFlags58&2u)&&!(i.applicationFlags58&1u)&&!(i.vehicleFlagsCD8&1u)&&!i.vehicleState40){
        Math f;float forward[3]={f.read(i.localForward340_288[0]),0.f,f.read(i.localForward340_288[2])};
        f.horizontal(forward,i.effectiveFallback433C70);
        float direction[3]={f.sub(i.currentC80Position[0],i.previousC84Position[0]),
            f.sub(i.currentC80Position[1],i.previousC84Position[1]),f.sub(i.currentC80Position[2],i.previousC84Position[2])};
        f.horizontal(direction,i.effectiveFallback433C70);r.directionDot=f.dot(direction,forward);
        r.sourceAngle=f.angle(r.directionDot);r.evaluated=true;
        n.orientation10A0=(r.sourceAngle<word(0x407b53d1u)&&!(r.sourceAngle<=word(0x4016cbe4u)))?1u:0u;
        if(n.orientation10A0&&f.read(n.wrongWayTimer1094)==0.f)n.wrongWayTimer1094=f.read(i.preparedFactor);
        if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x173c6c);
    }
    state=n;out=r;return true;
}
struct AiTurnSideInput50IV {float currentNodePosition[3],scene190Position[3],scene190LocalForward[3];};
//1737E0 is a cross-product side refresh, NOT a timer update. Called by16EB74
//only after1090<=0. Canonical scene queries must already have been completed.
inline bool refreshAiTurnSide50IV(const AiTurnSideInput50IV& i,AiStatePrefixState50IV& state,
    AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x1737e0))return false;Math f;
    const float x=f.sub(i.currentNodePosition[0],i.scene190Position[0]);
    const float y=f.sub(i.currentNodePosition[1],i.scene190Position[1]);
    const float z=f.sub(i.currentNodePosition[2],i.scene190Position[2]);
    // VOPMULA/VOPMSUB: the source computes all cross lanes before reading Y.
    const float cx=f.sub(f.mul(y,i.scene190LocalForward[2]),f.mul(z,i.scene190LocalForward[1]));
    const float cy=f.sub(f.mul(z,i.scene190LocalForward[0]),f.mul(x,i.scene190LocalForward[2]));
    const float cz=f.sub(f.mul(x,i.scene190LocalForward[1]),f.mul(y,i.scene190LocalForward[0]));
    (void)cx;(void)cz;if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x173898);
    state.turnSide1098=cy<=0.f?1:-1;return true;
}
struct AiNoCombatInput50IV {
    float timerCB8,combat1054,combatWait106C,remainingCC4;
    std::int32_t lapCC8,placeCCC;
    std::uint32_t fieldFE4,managerField34;
    std::uint8_t applicationFlags58,settings1125C,currentNodeFlags79;
    bool managerRacer0Present,managerRacer1Present;
    std::uint32_t managerRacer0State40,managerRacer1State40;
    AiTurnSideInput50IV turnSide; //Consumed only when1090<=0.
};
struct AiStateDispatchReceipt50IV {bool complete=true;std::uint32_t boundaryPC=0;bool turnSideRefreshed=false;};
//16EB00 after1739D0. Implements actual0,4,5 and proven no-combat state2 guards.
//A valid Boundary receipt means STOP before that operation; it is never false
//combat input. Source writes preceding the boundary remain committed to state.
inline bool advanceAiStateNoCombat50IV(const AiNoCombatInput50IV& i,AiStatePrefixState50IV& state,
    AiStateDispatchReceipt50IV& out,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x16eb00))return false;Math f;
    auto n=state;AiStateDispatchReceipt50IV r{};
    if(n.orientation10A0&&!(f.read(n.wrongWayTimer1094)<=5.f))n.aiStateFE8=5;
    if(f.read(n.stallTimer1090)<=0.f){n.aiStateFE8=5;
        if(!refreshAiTurnSide50IV(i.turnSide,n,d))return false;r.turnSideRefreshed=true;}
    auto boundary=[&](std::uint32_t pc) noexcept {r.complete=false;r.boundaryPC=pc;};
    switch(n.aiStateFE8){
    case 0:n.aiStateFE8=4;break;
    case 4:if(!(f.read(i.timerCB8)<=5.f))n.aiStateFE8=2;break;
    case 5:
        //16EDA8 targets16EDBC: the low-timer branch skips BOTH stores.
        if(n.stallTimer1090<1.5f){boundary(0x16ea94);break;}
        n.stallTimer1090=1.5f;n.aiStateFE8=2;break;
    case 2:
        if((i.applicationFlags58&1u)&&i.managerRacer0Present&&!i.managerRacer0State40&&
           (i.managerField34!=2u||(i.managerRacer1Present&&!i.managerRacer1State40))){
            n.aiStateFE8=6;boundary(0x16eab0);break;}
        //1704E0 no-write early returns, in exact source order.
        if(f.read(i.combat1054)==0.f)break;
        if(f.read(i.timerCB8)<f.read(i.combatWait106C))break;
        if(i.lapCC8==static_cast<std::int32_t>(i.settings1125C&31u)-1&&f.read(i.remainingCC4)<2000.f)break;
        if(i.currentNodeFlags79&2u)break;
        if(i.fieldFE4&&i.placeCCC!=1)break;
        boundary(0x1705c8);break; //Next source operation scans real racers and writes1008.
    default:boundary(0x16eb7c);break;
    }
    if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x16eb00);
    state=n;out=r;return true;
}
struct AiPackingState50IV {float scaleFFC,gap1004;std::uint32_t policy1000;};
struct AiPackingInput50IV {
    std::int32_t referenceFF8;bool referencePresent;std::uint32_t referenceState40;
    float referenceCachedSpeed34,referenceRemainingCC4,remainingCC4,trackLength3C;
    std::int32_t referenceLapCC8,lapCC8,placeCCC;
    std::uint32_t nodeC80,aiStateFE8;std::uint8_t settings1125C;
    float timerCBC,track90,track94,track98,track9C,trackA0,trackA4;
};
struct AiPackingReceipt50IV {bool sourceReturnNonzero=false;bool heat7CWritten=false;};
//171C60. referencePresent is the actual1AF330(vehicle338,FF8) resolution.
//thermalHeat7C is the SAME owner later read by173180/2176E0. Failure is atomic.
inline bool updateAiPacking50IV(const AiPackingInput50IV& i,AiPackingState50IV& state,
    float& thermalHeat7C,AiPackingReceipt50IV& out,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x171c60))return false;Math f;
    auto n=state;auto heat=thermalHeat7C;AiPackingReceipt50IV r{};n.scaleFFC=1.f;n.policy1000=0;
    auto finish=[&]() noexcept {if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x171c60);
        state=n;thermalHeat7C=heat;out=r;return true;};
    if(i.referenceFF8<0||!i.referencePresent||i.referenceState40)return finish();
    if(f.mul(word(0x3f2e8bc1u),i.referenceCachedSpeed34)<200.f)return finish();
    const float refLap=f.integer(i.referenceLapCC8);
    const float refProduct=f.mul(refLap,i.trackLength3C);
    const float refRemaining=f.sub(i.trackLength3C,i.referenceRemainingCC4);
    const float ownLap=f.integer(i.lapCC8);
    const float refDistance=f.add(refProduct,refRemaining);
    const float ownProduct=f.mul(ownLap,i.trackLength3C);
    const float ownRemaining=f.sub(i.trackLength3C,i.remainingCC4);
    const float ownDistance=f.add(ownProduct,ownRemaining);
    float gap=f.sub(refDistance,ownDistance); //171D44 delay slot, even on early exit.
    if(i.remainingCC4<1000.f&&i.nodeC80==1u)return finish();
    gap=f.sub(refDistance,ownDistance);n.gap1004=gap;
    if(i.lapCC8==static_cast<std::int32_t>(i.settings1125C&31u)-1&&i.remainingCC4<word(0x45250000u)){
        if(gap<0.f)return finish();if(gap<500.f){r.sourceReturnNonzero=true;return finish();}}
    const float low=-f.read(i.track90),high=-f.read(i.track94);
    const float from=f.read(i.trackA0),to=f.read(i.trackA4);
    if(high==low)n.scaleFFC=0.f;
    else {
        const float x=f.sub(gap,low),y=f.sub(to,from),product=f.mul(x,y),span=f.sub(high,low);
        float value=f.div(product,span);value=f.add(from,value);
        if(!(to<=from)){if(!(value<=to))value=to;else if(value<from)value=from;}
        else {if(value<to)value=to;else if(!(value<=from))value=from;}
        n.scaleFFC=value;
    }
    if(i.placeCCC==2&&!(f.read(i.timerCBC)<=10.f))return finish();
    if(gap<low){n.policy1000=5;heat=word(0x3f733333u);r.heat7CWritten=true;r.sourceReturnNonzero=true;return finish();}
    if(gap<high){n.policy1000=6;r.sourceReturnNonzero=true;return finish();}
    if(!(gap<=f.read(i.track98))){n.policy1000=4;r.sourceReturnNonzero=true;return finish();}
    if(!(gap<=f.read(i.track9C))){n.policy1000=7;r.sourceReturnNonzero=true;return finish();}
    if(!(gap<=0.f)&&i.aiStateFE8!=1u)n.policy1000=8;
    return finish();
}
struct AiRepairState50IV {std::uint32_t repairFEC;float cooldown1018,duration101C;};
struct AiRepairInput50IV {
    float preparedFactor,damage7D0,damage7D4,damage7DC,environmentD38,environmentD3C,impactBA0;
    std::uint32_t vehicleCFC;
    float cooldown1060,duration1064,trackCooldown6C,trackDuration70,trackThreshold74;
};
struct AiRepairReceipt50IV {bool sourceReturnNonzero=false;};
//1734C0 with numeric2234E0 max-damage helper. Shared B9C is borrowed explicitly.
inline bool updateAiRepair50IV(const AiRepairInput50IV& i,AiRepairState50IV& state,
    std::uint8_t& flagsB9C,AiRepairReceipt50IV& out,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x1734c0))return false;Math f;
    auto n=state;auto flags=flagsB9C;AiRepairReceipt50IV r{};
    const float damage=detail::scalarMax48O(detail::scalarMax48O(f.read(i.damage7D4),f.read(i.damage7D0)),f.read(i.damage7DC));
    const float tenth=word(0x3dcccccdu);
    auto clear=[&]() noexcept {flags=static_cast<std::uint8_t>(flags&0xfdu);};
    auto finish=[&]() noexcept {if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x1734c0);
        state=n;flagsB9C=flags;out=r;return true;};
    if(damage<tenth){n.repairFEC=0;clear();}
    if(f.read(i.impactBA0)!=0.f){
        const float track=f.read(i.trackCooldown6C);
        if(!i.vehicleCFC&&!(track<=f.read(n.cooldown1018)))n.cooldown1018=track;
        if(i.vehicleCFC&&!(f.read(i.cooldown1060)<=f.read(n.cooldown1018)))n.cooldown1018=i.cooldown1060;
        clear();
    }
    if(!(f.read(n.cooldown1018)<=0.f)){n.cooldown1018=f.sub(n.cooldown1018,i.preparedFactor);return finish();}
    if(n.repairFEC){
        if(f.read(n.duration101C)<=0.f){clear();n.duration101C=f.read(i.duration1064);n.cooldown1018=f.read(i.trackCooldown6C);return finish();}
        n.duration101C=f.sub(n.duration101C,i.preparedFactor);flags=static_cast<std::uint8_t>(flags|2u);
        r.sourceReturnNonzero=true;return finish();
    }
    const float environmental=detail::scalarMax48O(f.read(i.environmentD38),f.read(i.environmentD3C));
    //173690 also computes max(7D4,7D0); its result is not subsequently consumed.
    const float unused=detail::scalarMax48O(i.damage7D4,i.damage7D0);(void)unused;
    if(!(environmental<=f.read(i.trackThreshold74))){n.duration101C=f.read(i.trackDuration70);n.repairFEC=1;}
    const float right=f.sub(i.damage7D4,i.environmentD38);
    const float left=f.sub(i.damage7D0,i.environmentD3C); //1736FC branch delay slot.
    if(!(right<=tenth)||!(left<=tenth)){n.duration101C=f.read(i.duration1064);n.repairFEC=1;}
    if(n.repairFEC)flags=static_cast<std::uint8_t>(flags|2u);
    r.sourceReturnNonzero=(flags&2u)!=0;return finish();
}
} // namespace rr::game
