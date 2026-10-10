#pragma once
#include "body_integration48c.h"

namespace rr::game {
// Original contiguous 4x4 scene storage. Basis vectors occupy v[0..2],
// v[4..6], v[8..10]; position occupies v[12..14]. No implicit transpose.
struct BodyMatrix48G { float v[16]; };

// Recovered original orientation helpers, including largest-component ties
// and the game's rational angle approximation. These functions deliberately
// do not normalize the input quaternion or replace the approximation by atan2.
// All consumed inputs and intermediates must be finite; failure leaves output
// unchanged. Requires strict binary32 arithmetic, no contraction/fast math.
bool quaternionToMatrix48G(const Quaternion48C&, BodyMatrix48G&) noexcept;
bool matrixToQuaternion48G(const BodyMatrix48G&, Quaternion48C&) noexcept;
bool originalAngle48G(float numerator, float denominator, float&) noexcept;
bool quaternionToAngles48G(const Quaternion48C&, Vector3_48C&) noexcept;
}
