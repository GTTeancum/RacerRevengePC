#pragma once
#include "body_scene48g.h"
#include "body_contact_list48r.h"
#include <cstdint>

namespace rr::game {
struct RootBodyState48H {
    BodyState48C body;
    BodyMatrix48G scene;
    Vector3_48C previousVelocity90, localVelocity80, worldAngularVelocityA0;
    BodyLoads48C loads;
    Vector3_48C positionCorrection1a0, rotationCorrection1b0;
    Vector3_48C accumulator1c0, accumulator1d0;
    // Canonical original body+238 count and ten body+23C processed identities.
    // This list is DISTINCT from body20C/210's symmetric contact partners.
    // 1654B0 clears only count; inactive slots retain their prior bits. Root
    // copies carry both owners through body response and serial racer banking.
    std::uint32_t callbackState238;
    BodyContactIdentity48R processedContactPartners23C[BodyContactCapacity48R]{};
};
// Complete numerical root-scene phase of original 167C40, including the
// observed body's final 1654B0 callback. Child component updates must run before
// this call; native scene observers must receive the resulting transform after
// success. No guest pointers, scene hierarchy traversal or collision producer.
// A missing scene or disabled body skips integration but still clears all six
// accumulators and runs the observed final callback, as originally.
// The original local center offset is body+1E0. Body.position is scene-translation
// output after this call; it is reloaded from the transformed center next call.
// Failure is transactional. Strict binary32 arithmetic is required.
bool updateRootBody48H(const RootBodyState48H&, const BodyParameters48C&,
                      const Vector3_48C& centerOffset, bool sceneAttached,
                      bool enabled, float elapsedFactor, RootBodyState48H&) noexcept;
}
