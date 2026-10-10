#pragma once
#include "collision_type3_leaf48p.h"

namespace rr::game {
// The two independent predicates in original 10E080's mode1 node path.
// A caller tests sphere first, then rays in index order until one overlaps;
// it then visits child+8 before child+A exactly once, with all rays.
enum class Type3NodeStatus48P { Overlap,Separated,InvalidInput };

// Original 10ED7C..10EE40: inclusive XYZ abs(boxCenter-localCenter)
// <= boxHalf+radius. This is its expanded-box test, not a Euclidean sphere/AABB.
// XYZ, radius and half extents must be finite; radius/half must be >=0.
Type3NodeStatus48P type3SphereBox48P(const Vector3_48C& localCenter,float radius,
    const Vector3_48C& boxCenter,const Vector3_48C& boxHalf) noexcept;

// Original 10EE60..10F3FC: start-inside passes before checking length, then
// ordered +X,-X,+Z,-Z,+Y,-Y face tests. Finite negative and zero lengths are
// accepted. Start/direction XYZ and length must be finite; W is not consumed.
// No direction normalization and no replacement slab algorithm. Zero direction
// components are divided by; transient IEEE Inf/NaN follows original ordered
// comparison polarity. Finite-input arithmetic overflow likewise follows those
// comparisons, rather than introducing an extra intermediate validity gate.
// Individually rounded binary32, round-to-nearest, gradual underflow, masked
// FP exceptions, no fast math/FMA. Not physical EE exceptional-mode proof.
// Both APIs are serial, allocation-free, fixed-stack, and mutate no input/state.
Type3NodeStatus48P type3RayBox48P(const Type3Segment48P&,
    const Vector3_48C& boxCenter,const Vector3_48C& boxHalf) noexcept;
}
