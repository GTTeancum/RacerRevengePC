#pragma once
#include "pod_physics49n.h"
#include "model_geometry49o.h"
#include "../game/vehicle_visual49r.h"

namespace rr::assets {
// Exactly the live normal two-engine coefficients, in source destination order.
enum class VisualAuthoredKey49R : std::uint32_t {
    ImpactForce, GroundImpactForce, SeparationForce, ThrustForce, LiftForce,
    DragForce, EngineSpring, EngineDamping, BinderSpring, YawTorque,
    PowerslideYawTorque, PowerslidePitchTorque, PitchTorque, RollTorque,
    EngineTorque, EngineTorqueDamping, BinderTorque, MinTurbulenceTimeScale,
    MaxTurbulenceTimeScale, MinTurbulenceScale, MaxTurbulenceScale,
    MaxTurbulenceSpeed, DamageTurbulenceScale, DamageOrientationTurbulenceScale,
    OrientationScale, MaxInputScaleSpeed, MaxLiftHeight, Count
};
constexpr std::size_t visualAuthoredKeyCount49R =
    static_cast<std::size_t>(VisualAuthoredKey49R::Count);
struct VisualAuthoredValues49R { float values[visualAuthoredKeyCount49R]; };
struct VisualAuthoredProvenance49R {
    PodPhysicsToken49N tokens[visualAuthoredKeyCount49R];
};
enum class VisualAuthoringError49R : std::uint32_t {
    none, text, missing_key, rounding_mode, nonfinite, hierarchy, nullcg, arithmetic
};
struct VisualAuthoringDiagnostic49R {
    VisualAuthoringError49R error;
    PodPhysicsDiagnostic49N text;
    VisualAuthoredKey49R key;
};
// Original decoded PHY text; all26 mapped [Pod Animation] scalars and
// [Repulsor] MaxLiftHeight are required. Missing values are rejected, never
// replaced by constructor defaults. Unknown values are not consumed. Reuses
// the independently proved49N scalar conversion verbatim, including +0 for -0.
// Same closed ASCII/line/count/duplicate admission as49N; scalar numbers have
// optional sign, integer digits, optional nonempty decimal fraction, <=9 total
// digits, <=6 fractional digits, no exponent/suffix/NaN/Inf. Fixed storage,
// no allocation/I/O; caller owns bytes/output/provenance. Input, output and
// provenance must not overlap. Failure preserves output and provenance.
bool readVehicleVisualAuthoring49R(const char*, std::size_t,
    VisualAuthoredValues49R&, VisualAuthoringDiagnostic49R&,
    VisualAuthoredProvenance49R* = nullptr) noexcept;

// Explicit FRESH source model context: each admitted unmodified PSG view is
// attached beneath an unparented identity graphics-model root; node0 is its
// sole hierarchy root named nullcg; no motion/extra transform has run. Compute
// nullcg WORLD XYZ by original local*identity arithmetic (including signed-zero
// effects), capture it with constructor W=1, then clear Z as210D00/210FB4.
// This binds actual PSG bytes; PHY EnginePosition and cached radius are unused.
// All raw values must be finite; strict binary32 FE_TONEAREST. A54 uses the
// three separate source multiplies with K=3FBBBB84; A68 one multiply. False
// preserves output. Does not modify live scene/history or establish FrameReady,
// spawn, B54, model ownership, or load a complete MCF/Create graph.
bool bindFreshVehicleVisualParameters49R(const VisualAuthoredValues49R&,
    const PsgHierarchyView49O& right, const PsgHierarchyView49O& left,
    rr::game::VehicleVisualParameters49R&, VisualAuthoringDiagnostic49R&) noexcept;

// Original optional bottom-model nullcg captures211298/211594. Same fresh
// PSG arithmetic as primary engines; no additional physical body is created.
bool bindFreshBottomVehicleVisualParameters49R(const PsgHierarchyView49O& right,
    const PsgHierarchyView49O& left, rr::game::VehicleVisualParameters49R&,
    VisualAuthoringDiagnostic49R&) noexcept;

// Explicit constructor-record provenance only. Call solely when constructing
// a new caller-owned record, never as live-history repair/reset. No model/base
// offset loading, mode inference, frame scheduling, or physics initialization.
void initializeVehicleVisualConstructorRecords49R(
    rr::game::VehicleVisualState49R&) noexcept;
// Separate process-cold graphics record projection: BSS guards/words +0 and
// post-static-init434630=(0,0,0,1). Never selects/resets shared49M noise or RNG.
void initializeVehicleVisualSharedColdRecords49R(rr::game::VisualShared49R&) noexcept;
}
