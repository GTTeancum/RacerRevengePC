#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Temporary projection only. The frame retains canonical ownership of overlaps.
struct ImpactConsumerState49Z {
    float directionalD08[8];
    float timers820[5];
    float damage7D0[4];
    float cooldown7E0, periodic7E8;
    std::uint32_t fieldCFC, fieldD00, fieldD04;
    std::uint32_t partnerId, partnerState;
    float partnerTimer08, partnerAmount0C, partnerDuration10;
    float environmentalD38, environmentalD3C;
};
struct ImpactConsumerInput49Z {
    std::uint32_t managerFlags58, flagsCD8, vehicle3C, vehicle40, vehicle58;
    std::uint32_t vehicle64, player5C;
    // Bits 0..3 represent actual non-null components1B8,1BC,1C0,1C4.
    std::uint32_t componentPresence;
    std::uint32_t profile34, profile10FAC, profileFlags11050;
    float factor, finalDamageThreshold64, finalDamageIncrement68;
};
// Source controller virtual calls, in order. The caller binds these to its
// real controller backend; neither is an audio or gameplay-state substitute.
struct ImpactConsumerEvent49Z {
    std::uint32_t sourcePC, vtableSlot, player, channel, argument2;
    float amount;
};
enum class ImpactConsumerStatus49Z { applied, skipped, unsupportedPartner, arithmetic, capacity };
// Includes original manager bit1 dispatch at2288A8,222B20,2228D0,221710,
// reached221590 and the real188EC0 profile predicate. No BA0 write occurs here.
// Dynamic1AF330/22ADE0 route rejects atomically when reached. All other ordinary
// numerical paths are implemented, including authentic skips, damage gates,
// timer crossings, directional clears and vehicle64=2 periodic damage.
// Needs ordinary finite-normal/signed-zero consumed arithmetic, nearest rounding
// and strict binary32 operations. Subnormal/nonfinite operands/results reject;
// no claim of exceptional physical EE behavior. Unconsumed words copy verbatim.
// At most two events; caller owns bounded storage. Output, events and count stay
// unchanged on rejection. Successful skip copies state and sets count zero.
// Output may alias state exactly; all other ranges must be disjoint (precondition,
// not runtime overlap detection). No allocation; events=null is valid iff capacity=0.
ImpactConsumerStatus49Z consumeImpacts49Z(const ImpactConsumerInput49Z&,
    const ImpactConsumerState49Z&, ImpactConsumerState49Z& output,
    ImpactConsumerEvent49Z* events, std::size_t capacity,
    std::size_t& count) noexcept;
}
