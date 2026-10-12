// NEW unverified source-only reconstruction. Native clock mapping, not EE MMIO.
#include "original_ui_rider_voice_choice50kg.h"
#include <cfenv>
namespace rr::game {
namespace {
bool fail(OriginalUiRiderVoiceChoiceDiagnostic50KG& d,OriginalUiRiderVoiceChoiceError50KG e,std::uint32_t pc)noexcept{
    d.error=e;d.sourcePC=pc;return false;
}
}
bool OriginalUiRiderVoiceChoice50KG::preflight(std::uint32_t rider,OriginalUiRiderVoiceChoiceDiagnostic50KG& d)const noexcept{
    d={};if(rider!=19&&rider!=20)return fail(d,OriginalUiRiderVoiceChoiceError50KG::input,0x1EA658);
    return std::fegetround()==FE_TONEAREST||fail(d,OriginalUiRiderVoiceChoiceError50KG::input,0x1EA6B4);
}
bool OriginalUiRiderVoiceChoice50KG::choose(std::uint32_t rider,HostMonotonicClock50ID& clock,CoupledNoiseContext49M& shared,
    SharedRandomOwner50ID& owner,
    OriginalUiRiderVoiceChoiceReceipt50KG& out,OriginalUiRiderVoiceChoiceDiagnostic50KG& d)noexcept{
    d={};out={};if(!preflight(rider,d))return false;
    // 1E7C80/1E7D6C: page+53BC+19*4 count5. 1E7CF8/1E7E3C:
    // page+5428+19*4 starts at39. These are original constructor constants.
    // Rider20: count2 at1E7D74, first44 at1E7E44 (page+53BC/+5428).
    const std::int32_t count=rider==19?5:2;
    const std::uint32_t first=rider==19?39u:44u;
    if(!owner.seed.lazy80A8){owner.seed.seed3D6B30=0;owner.seed.lazy80A8=true;}
    if(!owner.seed.seed3D6B30){
        std::uint32_t timestamp=0;
        if(!sampleHostTimestamp50ID(clock,timestamp))return fail(d,OriginalUiRiderVoiceChoiceError50KG::clock,0x1EA694);
        owner.seed.seed3D6B30=timestamp;
        seedOriginalSharedRng50IU(shared,owner.seed.seed3D6B30);out.seeded=true; //308020 at1EA6A8
    }
    out.seedCache3D6B30=owner.seed.seed3D6B30;
    const auto random=drawOriginalSharedRng50IU(shared); // Exactly one308030 at1EA6B8.
    out.draw308030=random;out.rawRandomAfter=shared.sharedRandomState;
    // Fixed-domain native evaluation of the source positive conversion and
    // choice, not a general EE arithmetic emulator or a host rounding change.
    // CVT.S.W toward zero drops low bits beyond24 significant binary bits.
    std::uint32_t quantized=random,shift=0;
    for(auto magnitude=random;magnitude>=0x01000000u;magnitude>>=1)++shift;
    if(shift)quantized&=~((std::uint32_t{1}<<shift)-1u);
    // *2^-31 is exact. Multiplication by the bounded count (5 or2) and positive truncation can
    // be evaluated as this rational: every integer0..5 is representable,
    // so positive toward-zero binary32 reduction cannot cross its integer
    // floor. No intermediate float rounding can manufacture a choice==count.
    out.choice=static_cast<std::int32_t>((std::uint64_t{quantized}*count)>>31);
    if(!owner.range.lazy80AC){owner.range.rangeCheck3D6B38=1;owner.range.lazy80AC=true;}
    // Retain the source range assertion as fail-closed, never clamp/modulo.
    if(owner.range.rangeCheck3D6B38&&(out.choice<0||out.choice>=count))
        return fail(d,OriginalUiRiderVoiceChoiceError50KG::range,0x1EA734);
    out.soundId=first+static_cast<std::uint32_t>(out.choice);out.accepted=true;return true;
}
bool OriginalUiRiderVoiceChoice50KG::preflightRider19(OriginalUiRiderVoiceChoiceDiagnostic50KG& d)const noexcept{
    return preflight(19,d);
}
bool OriginalUiRiderVoiceChoice50KG::chooseRider19(HostMonotonicClock50ID& clock,CoupledNoiseContext49M& shared,
    SharedRandomOwner50ID& owner,OriginalUiRiderVoiceChoiceReceipt50KG& out,
    OriginalUiRiderVoiceChoiceDiagnostic50KG& d)noexcept{
    return choose(19,clock,shared,owner,out,d);
}
}
