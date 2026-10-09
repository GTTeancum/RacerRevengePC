#pragma once
#include "coupled_body_advancement49m.h"

namespace rr::game {
// Source-ready geometry boundary, not a loader or radius-discovery API.
// Radius is from the Chariot graphics model AFTER2134A0 and its virtual90
// aggregate. nullcg inputs are from BEFORE that animation setup. Never use
// cached/bind-only radius or animated nullcg values as implicit substitutes.
struct AuthoredGeometryInputs49O {
    float postMotionRadius198;
    Vector3_48C preAnimationChariotNullCgLocal;
    float preAnimationRightEngineNullCgWorldZ;
};
struct AuthoredGeometryBinding49O {
    float radius198, mass26C, inverseMass270;
    // Complete source matrices. inverseInertia140 is canonical for projection
    // and joint setup, including signed zeros and its source zero W diagonal.
    BodyMatrix48G inertia100, inverseInertia140;
    PodSphereVector49L targetRelative2F0, secondaryAnchor300, mainAnchor;
    float length854, unclampedLength858;
};
//2125D8..212668 and2119CC..211A2C/2126B0..2126F4, with original mass/inertia
// setters165300/165340/14E910. Fresh vehicle vector initialization establishes
// the complete W lanes; this does not construct a body/physical scene/history.
// Finite radius>=0 and consumed source values/intermediates; exact-zero
// determinant is supported as original all-zero inverse, not a fallback.
bool bindAuthoredGeometry49O(const AuthoredGeometryInputs49O&,
                           AuthoredGeometryBinding49O&) noexcept;
// Projects secondary mass, inverseMass, all inverse-inertia lanes, the four
// source squared limits501502F9, and target2F0. All other49M parameters/context
// remain previous-owned, including centers/W, axes, main and gravity. No body
// state is accepted or modified. Parameter/input outputs must be distinct.
bool projectAuthoredGeometry49O(const AuthoredGeometryBinding49O&,
    const CoupledBodyParameters49M& previous, const SecondaryInputs49M& previousInputs,
    CoupledBodyParameters49M& out, SecondaryInputs49M& outInputs) noexcept;
// Original49L numeric constructor suffix, setup A=secondary with anchor300,
// then B=explicit main with anchor434630. Cache A only AFTER binding its source
// inverse mass/inertia. B setup uses its non-null identity, not its scene pose.
// Preserves no synthetic stiffness/damping override; source49L defaults apply.
bool setupAuthoredPodSphere49O(const AuthoredGeometryBinding49O&,
    const PodSphereBody49L& main, const PodSphereJoint49L& previous,
    PodSphereJoint49L& out) noexcept;
// FE_TONEAREST, strict binary32/no FMA; fixed-size and allocation-free.
// Bool failure is atomic. Corresponding previous/output arguments may alias.
// Constructor/scene roots/pose/centers/histories and later live writes stay
// explicit caller inputs; this is not a complete original vehicle initializer.
}
