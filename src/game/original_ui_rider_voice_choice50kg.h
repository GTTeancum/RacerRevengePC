// NEW unverified source-only reconstruction. Native clock mapping, not EE MMIO.
#pragma once
#include "lap_host_clock50id.h"
#include "original_shared_rng50iu.h"
#include "shared_random_owner50id.h"
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
// Stateless selector of1EA658..1EA784. Borrow the application lifetime lazy
// seed/range owner, actual clock and SAME CoupledNoise RNG as other consumers.
// Neither screen destruction nor raw startup seeding resets the lazy fields.
class OriginalUiRiderVoiceChoice50KG final {
public:
    OriginalUiRiderVoiceChoice50KG()noexcept=default;
    OriginalUiRiderVoiceChoice50KG(const OriginalUiRiderVoiceChoice50KG&)=delete;
    OriginalUiRiderVoiceChoice50KG& operator=(const OriginalUiRiderVoiceChoice50KG&)=delete;
    // Original constructor rows for default-unlocked Riders and retained additions:
    // 1,2,3,4,7,8,9,10,11,12,15,16,17,18,19,20,22,24. This grants no profile unlock.
    bool preflight(std::uint32_t rider,OriginalUiRiderVoiceChoiceDiagnostic50KG&)const noexcept;
    bool choose(std::uint32_t rider,HostMonotonicClock50ID&,CoupledNoiseContext49M&,SharedRandomOwner50ID&,
        OriginalUiRiderVoiceChoiceReceipt50KG&,OriginalUiRiderVoiceChoiceDiagnostic50KG&)noexcept;
    bool preflightRider19(OriginalUiRiderVoiceChoiceDiagnostic50KG&)const noexcept;
    bool chooseRider19(HostMonotonicClock50ID&,CoupledNoiseContext49M&,SharedRandomOwner50ID&,
        OriginalUiRiderVoiceChoiceReceipt50KG&,OriginalUiRiderVoiceChoiceDiagnostic50KG&)noexcept;
};
}
