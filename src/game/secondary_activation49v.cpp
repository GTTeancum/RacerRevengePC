#include "secondary_activation49v.h"
#include <cstring>

namespace rr::game {
namespace {
void copyVector(Vector3_48C& xyz, float& w, const std::uint32_t (&words)[4]) noexcept {
    std::memcpy(xyz.v, words, sizeof(xyz.v));
    std::memcpy(&w, words + 3, sizeof(w));
}
void zeroVector(Vector3_48C& vector) noexcept {
    std::memset(vector.v, 0, sizeof(vector.v));
}
}
void initializedSecondaryActivationGlobals49V(SecondaryActivationGlobals49V& globals) noexcept {
    //433B80 comes from original3C9290;382B60 contains four file-backed +0 words.
    constexpr SecondaryActivationGlobals49V initial{{0, 0, 0, 0x3f800000u}, {0, 0, 0, 0}};
    std::memcpy(&globals, &initial, sizeof(globals));
}
void resetSecondaryActivation49V(SecondaryBodyState49M& secondary,
    SecondaryActivationSupplement49V& extra, const SecondaryActivationGlobals49V& globals) noexcept {
    auto& root = secondary.root;
    auto& body = root.body;
    //167A50 preserves the source store order and copies all four vector lanes.
    copyVector(root.loads.force, secondary.force180W, globals.vector433B80);
    copyVector(root.loads.torque, secondary.torque190W, globals.vector433B80);
    copyVector(body.linearAcceleration, extra.linearAccelerationC0W, globals.vector433B80);
    copyVector(body.angularAcceleration, extra.angularAccelerationD0W, globals.vector433B80);
    copyVector(body.velocity, secondary.velocity70W, globals.vector433B80);
    copyVector(root.localVelocity80, extra.localVelocity80W, globals.vector433B80);
    copyVector(root.previousVelocity90, extra.previousVelocity90W, globals.vector433B80);
    copyVector(body.angularVelocity, secondary.angularVelocityB0W, globals.vector433B80);
    std::memset(&secondary.cachedSpeed34, 0, sizeof(secondary.cachedSpeed34));

    //167AD0 has two iterations, each with three12-byte zero fills and a
    // separate16-byte quaternion copy. Preserve those distinct destinations.
    zeroVector(extra.positionDerivative2B8);
    std::memcpy(body.orientationDerivative.v, globals.quaternion382B60, sizeof(body.orientationDerivative.v));
    zeroVector(extra.linearAcceleration2D0);
    zeroVector(extra.angularAcceleration308);
    zeroVector(body.previousPositionDerivative);
    std::memcpy(body.previousOrientationDerivative.v, globals.quaternion382B60, sizeof(body.previousOrientationDerivative.v));
    zeroVector(body.previousLinearAcceleration);
    zeroVector(body.previousAngularAcceleration);
}
void copySecondaryMainVelocities49V(const RootBodyState48H& main,
    const float& main70W, const float& mainB0W, SecondaryBodyState49M& secondary) noexcept {
    std::memcpy(secondary.root.body.velocity.v, main.body.velocity.v, sizeof(main.body.velocity.v));
    std::memcpy(&secondary.velocity70W, &main70W, sizeof(main70W));
    std::memcpy(secondary.root.body.angularVelocity.v, main.body.angularVelocity.v, sizeof(main.body.angularVelocity.v));
    std::memcpy(&secondary.angularVelocityB0W, &mainB0W, sizeof(mainB0W));
}
}
