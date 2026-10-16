// NEW unverified source-only reconstruction. Native clock mapping, not EE MMIO.
#include "original_ui_rider_voice_choice50kg.h"
#include <cfenv>
namespace rr::game {
namespace {
struct OriginalConfirmRange {
    std::uint32_t rider,first;
    std::int32_t count;
    const char* name;
};
// Constructor count page+53BC+rider*4, first page+5428+rider*4.
// Explicit authored ranges, never a compact unlocked-list index.
constexpr OriginalConfirmRange originalConfirmRanges[]{
    {2,5,2,"Ben Quadinaros"},       // count1E7C98, first1E7DB4
    {4,9,2,"Dud Bolt"},            // count1E7CC0, first1E7DC4
    {7,15,2,"Gasgano"},           // count1E7CFC, first1E7DDC
    {9,19,2,"Mawhonic"},          // count1E7D1C, first1E7DEC
    {10,21,2,"Aldar Beedo"},       // count1E7D24, first1E7DF4
    {11,23,2,"Ody Mandrell"},      // count1E7D2C, first1E7DFC
    {12,25,2,"Shrivel Braittrand"},// count1E7D34, first1E7E04
    {15,31,2,"Teemto Pagalies"},   // count1E7D4C, first1E7E1C
    {19,39,5,"Anakin Skywalker"},  // count1E7D6C, first1E7E3C
    {20,44,2,"Occo Ninebar"}       // count1E7D74, first1E7E44
};
const OriginalConfirmRange* confirmRange(std::uint32_t rider) noexcept {
    for(const auto& range:originalConfirmRanges)if(range.rider==rider)return &range;
    return nullptr;
}
bool fail(OriginalUiRiderVoiceChoiceDiagnostic50KG& d,OriginalUiRiderVoiceChoiceError50KG e,std::uint32_t pc)noexcept{
    d.error=e;d.sourcePC=pc;return false;
}
}
bool OriginalUiRiderVoiceChoice50KG::preflight(std::uint32_t rider,OriginalUiRiderVoiceChoiceDiagnostic50KG& d)const noexcept{
    d={};if(!confirmRange(rider))return fail(d,OriginalUiRiderVoiceChoiceError50KG::input,0x1EA658);
    return std::fegetround()==FE_TONEAREST||fail(d,OriginalUiRiderVoiceChoiceError50KG::input,0x1EA6B4);
}
bool OriginalUiRiderVoiceChoice50KG::choose(std::uint32_t rider,HostMonotonicClock50ID& clock,CoupledNoiseContext49M& shared,
    SharedRandomOwner50ID& owner,
    OriginalUiRiderVoiceChoiceReceipt50KG& out,OriginalUiRiderVoiceChoiceDiagnostic50KG& d)noexcept{
    d={};out={};if(!preflight(rider,d))return false;
    const auto& range=*confirmRange(rider); // Guaranteed by read-only preflight.
    const auto count=range.count;
    const auto first=range.first;
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
    // *2^-31 is exact. Multiplication by the authored count (2 or5) and positive truncation can
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
