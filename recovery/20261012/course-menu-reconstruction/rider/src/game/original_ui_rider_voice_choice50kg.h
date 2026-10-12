// NEW unverified source-only reconstruction. Native clock mapping, not EE MMIO.
#pragma once
#include "lap_host_clock50id.h"
#include "original_shared_rng50iu.h"
namespace rr::game {
enum class OriginalUiRiderVoiceChoiceError50KG : std::uint8_t {none,input,clock,range};
struct OriginalUiRiderVoiceChoiceDiagnostic50KG {
    OriginalUiRiderVoiceChoiceError50KG error=OriginalUiRiderVoiceChoiceError50KG::none;
    std::uint32_t sourcePC=0;
};
struct OriginalUiRiderVoiceChoiceReceipt50KG {
    bool accepted=false,seeded=false;
    std::uint32_t seedCache3D6B30=0,rawRandomAfter=0,draw308030=0;
    std::int32_t choice=0;
    std::uint32_t soundId=0;
};
// Application-lifetime function statics of1EA658..1EA784. Retain across every
// Rider/Course screen lifetime. There is no private RNG: mutations target the
// SAME application CoupledNoise owner as particles and character idles.
class OriginalUiRiderVoiceChoice50KG final {
public:
    OriginalUiRiderVoiceChoice50KG()noexcept=default;
    OriginalUiRiderVoiceChoice50KG(const OriginalUiRiderVoiceChoice50KG&)=delete;
    OriginalUiRiderVoiceChoice50KG& operator=(const OriginalUiRiderVoiceChoice50KG&)=delete;
    bool preflightRider19(OriginalUiRiderVoiceChoiceDiagnostic50KG&)const noexcept;
    bool chooseRider19(HostMonotonicClock50ID&,CoupledNoiseContext49M&,
        OriginalUiRiderVoiceChoiceReceipt50KG&,OriginalUiRiderVoiceChoiceDiagnostic50KG&)noexcept;
private:
    bool seedGuardGp80A8_=false,rangeGuardGp80AC_=false;
    std::uint32_t seedCache3D6B30_=0,rangeAssert3D6B38_=0;
};
}
