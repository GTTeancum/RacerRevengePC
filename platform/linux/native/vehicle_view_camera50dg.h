#pragma once
#include <cstdint>

namespace rr::platform {
// Explicit diagnostic-inspection camera, matching the 48B viewer's RH view
// and vertical-FOV convention. No usable camera/projection defaults are given.
// This is not original 49F game-camera recovery or a vehicle-follow policy.
struct VehicleViewCamera50DG {
    float eye[3], target[3], up[3];
    float verticalFovRadians;     // Strictly between zero and pi.
    float aspectWidthOverHeight; // Strictly positive; caller owns viewport fit.
    float nearPlane, farPlane;   // 0 < nearPlane < farPlane; finite distances.
};

struct VehicleViewMatrix50DG {
    // Complete world-to-GL-clip ROW-VECTOR matrix in COLUMN-MAJOR storage:
    // worldViewProjection[column * 4 + row]. Pass directly to 50DD's frame
    // worldViewProjection pointer (GL_FALSE upload); do not transpose again.
    // World positions are already transformed. No root/model matrix is added.
    float worldViewProjection[16];
};

enum class VehicleViewCameraError50DG : std::uint8_t {
    none, nonfiniteInput, projection, viewDirection, upDirection, matrixRange
};

// RH look-at: backward = eye - target, right = up cross backward, and corrected
// up = backward cross right; the camera looks down negative view Z. GL clip Z
// spans [-W,+W], mapping the positive near/far distances to NDC -1/+1. No Y flip,
// reversed/infinite depth, PS2 viewport mapping, bounds/orbit or roll policy.
// All inputs must be finite. Rejects coincident eye/target, zero/parallel up,
// invalid projection and nonfinite/out-of-float-range matrix coefficients;
// also rejects a nonzero coefficient that rounds to zero. No fallback axis or
// clamping. Double intermediates are diagnostic math, not source-step parity.
//
// Allocation-free and stateless. On failure output is unchanged; error always
// describes this call. Inputs, output and error must be separate live storage.
// No references/pointers are retained. Keep output unchanged through the 50DD
// submit call that reads it; 50DD copies all sixteen floats before submission.
bool composeVehicleViewCamera50DG(const VehicleViewCamera50DG& input,
    VehicleViewMatrix50DG& output, VehicleViewCameraError50DG& error) noexcept;
}
