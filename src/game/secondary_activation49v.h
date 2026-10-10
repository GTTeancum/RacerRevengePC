#pragma once
#include "secondary_body49m.h"

namespace rr::game {
// Only the thirteen source words absent from the canonical49M body. No XYZ,
// history, quaternion, clock, scene or full-lane49M field is duplicated here.
struct SecondaryActivationSupplement49V {
    float localVelocity80W, previousVelocity90W;
    float linearAccelerationC0W, angularAccelerationD0W;
    Vector3_48C positionDerivative2B8, linearAcceleration2D0, angularAcceleration308;
};
static_assert(sizeof(SecondaryActivationSupplement49V) == 52,
              "Only thirteen otherwise-unrepresented source words");

// Exact copy sources.433B80 is independent of49R's434630;382B60 is the separate
// file-backed quaternion constant. The explicit word form preserves every bit,
// including signed zero and NaN payloads, without numerical admission rules.
struct SecondaryActivationGlobals49V {
    std::uint32_t vector433B80[4], quaternion382B60[4];
};
void initializedSecondaryActivationGlobals49V(SecondaryActivationGlobals49V&) noexcept;

// Actual concrete secondary35C PhysicsRigidBody slotAC167A50 -> slotA8/167AD0.
// Writes exactly the represented236-byte source domain; no query/contact,
// constructor, scene, placement, integration, callback238 or phase effect.
// Cached34 becomes +0. Current and history derivatives are distinct source
// buffers; quaternion copies use382B60, not an identity quaternion.
void resetSecondaryActivation49V(SecondaryBodyState49M&,
    SecondaryActivationSupplement49V&, const SecondaryActivationGlobals49V&) noexcept;

// Caller21BF28..21BF3C, AFTER reset: copy main full70/B0 into secondary. Cached
// speed, local80, previous90, worldA0 and derivative histories remain untouched.
// Main is a distinct canonical body; neither source global nor supplement may
// overlap mutable secondary storage. All functions are fixed, serial, allocation
// free bit copies, valid in every rounding mode; they do not advance phase864.
void copySecondaryMainVelocities49V(const RootBodyState48H& main,
    const float& main70W, const float& mainB0W, SecondaryBodyState49M&) noexcept;
}
