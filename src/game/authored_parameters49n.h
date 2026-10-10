#pragma once
#include "coupled_body_advancement49m.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Only keys consumed by the represented numeric parameter adapter. This is not
// a complete PHY schema, body constructor, scene loader, or save-profile reader.
enum class AuthoredKey49N : std::uint32_t {
    Weight, RollInertiaScale, PitchInertiaScale, YawInertiaScale,
    Gravity, AirborneGravity,
    MaxThrustA, MaxThrustB, MaxThrustC, MinMaxThrust, MaxMaxThrust,
    TheoreticalTopSpeedA, TheoreticalTopSpeedB, TheoreticalTopSpeedC,
    MinTheoreticalTopSpeed, MaxTheoreticalTopSpeed,
    BoostThrust, RepairThrust, RampUpFilter, RampDownFilter, RampDownSpeed,
    ActualTopSpeed, BoostSpeed, RepairSpeed,
    DefaultAngleOfAttack, Lift, BasicDrag, PitchMoment, PitchRatePitchMoment,
    SideSlipSideForce, RollRateRollMoment, SideSlipYawMoment, YawRateYawMoment,
    SpoilerDrag, SpoilerBrakeDrag, SpoilerYawMomentScale, SpoilerSideForce,
    PowerslideSideForce, PowerslideSpoilerYawMoment, PowerslideThrust,
    InvertedSideSlipSideForce, InvertedSpoilerSideForce, InvertedSideSlipYawMoment,
    SpoilerYawMomentA, SpoilerYawMomentB, SpoilerYawMomentC,
    MinSpoilerYawMoment, MaxSpoilerYawMoment, SpoilerYawMomentOffset,
    BaseAbilityMaxThrust, MaxAbilityMaxThrust,
    BaseAbilityTheoreticalTopSpeed, MaxAbilityTheoreticalTopSpeed,
    BaseAbilitySpoilerYawMoment, MaxAbilitySpoilerYawMoment,
    SteerSpeed1, SteerScale1, SteerSpeed2, SteerScale2, XAxisRateLimit, YAxisRateLimit,
    Count
};
constexpr std::size_t authoredKeyCount49N = static_cast<std::size_t>(AuthoredKey49N::Count);
struct AuthoredValue49N { float value; bool present; };
struct AuthoredInteger49N { std::int32_t value; bool present; };
struct AuthoredVector49N { Vector3_48C value; bool present; };
struct AuthoredTuning49N {
    bool sectionPresent;
    AuthoredInteger49N useValues, useAValues, useBValues, useCValues;
    AuthoredValue49N upgradePercent;
};
struct AuthoredParameters49N {
    AuthoredValue49N values[authoredKeyCount49N];
    AuthoredVector49N leftEnginePosition, rightEnginePosition;
    AuthoredTuning49N tuning;
};
enum class AuthoredClass49N : std::uint32_t { A=1, B=2, C=3 };
struct AuthoredProfileAbilities49N { float thrust1920, speed1924, steering1928; };
struct AuthoredSelectionContext49N {
    AuthoredClass49N inferredClass;
    bool mode58Nonzero;
    // Resolved manager11040+4*vehicle5C; no inferred option identity.
    bool configurationEntryNonzero;
    // True exactly when global382B80->350==0 invokes216040. The caller must
    // resolve manager100C0 and vehicle60, and supply that selected profile/pod.
    bool applyProfileOverwrite;
    bool profileAbilitiesPresent;
    AuthoredProfileAbilities49N profile;
};
struct AuthoredBinding49N {
    MainBodyParameters49K main;
    ControlBindingParameters48W controls;
    ControlRateLimits48B rates;
    float gravity378, airborneGravity37C;
    // Source's complete matrices expose all inverse lanes, including signed
    // zeros and zero inverse[15]. Main.body consumes only the XYZ3x3 subset.
    BodyMatrix48G inertia100, inverseInertia140;
    float engineDifference5C; // Final engine58 - converted ActualTopSpeed.
    AuthoredClass49N selectedClass;
    bool usedAuthoredClassValues;
    bool massSetterWrote; //165300's positive-mass branch, used by projection.
};

// Exact string selection at214954..214AA0. Length excludes the terminating NUL;
// embedded NUL and length>255 are outside this bounded native input domain.
// This does not discover the manager's selected track or load its string.
bool selectAuthoredClass49N(const char* text, std::size_t length,
                           AuthoredClass49N& out) noexcept;

// Numeric projection of2146C0..216034 and216040..21634C into existing48D/E/W,
// 49K APIs. Required mapped keys must be present on the selected path. This
// complete-input requirement is native admission, NOT original missing-key
// behavior:158A70 skips its write on lookup failure. Only Tuning's explicitly
// initialized defaults are implemented. Unused keys/profile values are ignored.
// Previous is mandatory: unrelated main/body fields, aero.velocityDamping130,
// lazy control globals, pose, history, and secondary parameters are not authored
// here. Nonpositive mass preserves the previous mass/inverseMass as165300 does.
// FE_TONEAREST, finite consumed inputs/intermediates, strict binary32/no FMA.
// Fixed-size/allocation-free; false is atomic; previous/output may alias.
bool bindAuthoredParameters49N(const AuthoredParameters49N&,
    const AuthoredSelectionContext49N&, const AuthoredBinding49N& previous,
    AuthoredBinding49N& out) noexcept;

// Copy only recovered parameter writes into the genuine49M composition.
// Secondary parameters, main limits/axis enables, center/probe/world inputs and
// aero damping remain previous-owned. The complete inverseInertia140 is the
// canonical projection source for BOTH XYZ and supplemental lanes; the mirrored
// main.body.inverseInertiaColumns are ignored by this projection.
bool projectAuthoredCoupledParameters49N(const AuthoredBinding49N&,
    const CoupledBodyParameters49M& previous, CoupledBodyParameters49M& out) noexcept;
// Copy authored gravity into already-resolved selector/secondary contexts;
// remaining flags, geometry, globals and state observations stay caller-owned.
bool projectAuthoredGravity49N(const AuthoredBinding49N&,
    const SubstepSelectionContext49K& previousSelection,
    const SecondaryInputs49M& previousSecondary,
    SubstepSelectionContext49K& outSelection, SecondaryInputs49M& outSecondary) noexcept;
}
