// NEW unverified source-only reconstruction. Native clock mapping, not EE MMIO.
#include "original_ui_rider_voice_choice50kg.h"
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
bool fail(OriginalUiRiderVoiceChoiceDiagnostic50KG& d,OriginalUiRiderVoiceChoiceError50KG e,std::uint32_t pc)noexcept{
    d.error=e;d.sourcePC=pc;return false;
}
}
bool OriginalUiRiderVoiceChoice50KG::preflightRider19(OriginalUiRiderVoiceChoiceDiagnostic50KG& d)const noexcept{
    d={};return std::fegetround()==FE_TONEAREST||fail(d,OriginalUiRiderVoiceChoiceError50KG::input,0x1EA6B4);
}
bool OriginalUiRiderVoiceChoice50KG::chooseRider19(HostMonotonicClock50ID& clock,CoupledNoiseContext49M& shared,
    OriginalUiRiderVoiceChoiceReceipt50KG& out,OriginalUiRiderVoiceChoiceDiagnostic50KG& d)noexcept{
    d={};out={};if(!preflightRider19(d))return false;
    // 1E7C80/1E7D6C: page+53BC+19*4 count5. 1E7CF8/1E7E3C:
    // page+5428+19*4 starts at39. These are original constructor constants.
    constexpr std::int32_t count=5;constexpr std::uint32_t first=39;
    if(!seedGuardGp80A8_){seedCache3D6B30_=0;seedGuardGp80A8_=true;}
    if(!seedCache3D6B30_){
        std::uint32_t timestamp=0;
        if(!sampleHostTimestamp50ID(clock,timestamp))return fail(d,OriginalUiRiderVoiceChoiceError50KG::clock,0x1EA694);
        seedCache3D6B30_=timestamp;
        seedOriginalSharedRng50IU(shared,seedCache3D6B30_);out.seeded=true; //308020 at1EA6A8
    }
    out.seedCache3D6B30=seedCache3D6B30_;
    const auto random=drawOriginalSharedRng50IU(shared); // Exactly one308030 at1EA6B8.
    out.draw308030=random;out.rawRandomAfter=shared.sharedRandomState;
    volatile float asFloat=static_cast<float>(random);
    volatile float normalized=asFloat*0x1p-31f;
    volatile float chosen=static_cast<float>(count)*normalized;
    if(!std::isfinite(chosen)||chosen<0||chosen>static_cast<float>(count))
        return fail(d,OriginalUiRiderVoiceChoiceError50KG::range,0x1EA6E0);
    out.choice=static_cast<std::int32_t>(chosen); // Source CVT.W.S truncation.
    if(!rangeGuardGp80AC_){rangeAssert3D6B38_=1;rangeGuardGp80AC_=true;}
    // Binary32 may round the largest random values to1.0 and produce5.
    // Original assertion range0..4 is retained as fail-closed, never clamped.
    if(rangeAssert3D6B38_&&(out.choice<0||out.choice>=count))
        return fail(d,OriginalUiRiderVoiceChoiceError50KG::range,0x1EA734);
    out.soundId=first+static_cast<std::uint32_t>(out.choice);out.accepted=true;return true;
}
}
