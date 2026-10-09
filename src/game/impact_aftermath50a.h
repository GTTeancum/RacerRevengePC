#pragma once
#include <cstdint>
namespace rr::game {
struct ImpactRepairState50A {
    float damage7D0[4]; // Ascending7D0,7D4,7D8,7DC; canonical fields remain elsewhere.
    float timer7E4, environmentalD38, environmentalD3C;
};
struct ImpactRepairInput50A {
    std::uint32_t flagsB9C, componentPresence; // Real player flag, same mask as49Z.
    float factor, repairCF4; // Current authored/profile-selected BaseRepairAmount.
};
enum class ImpactAftermathStatus50A { applied, skipped, arithmetic };
// Full224370. Actual B9C bit1 skip precedes numeric validation. Positive7E4
// decrements without clamp; otherwise .1 timer and source paired repair follow.
// Includes original224660/668 cross-pair7D4 ->7D8 assignment.
// Nearest, normal finite binary32/signed-zero consumed arithmetic only; no
// exceptional EE claim. Reject is atomic; unconsumed bits copy verbatim.
// No allocation. Output may exactly alias state; otherwise ranges disjoint.
//223540 and225C50 remain subsequent source stages, not implied by this function.
ImpactAftermathStatus50A repairImpactDamage50A(const ImpactRepairInput50A&,
    const ImpactRepairState50A&, ImpactRepairState50A& output) noexcept;
}
