#include "vehicle_view_camera50dg.h"
#include <cmath>
#include <limits>

namespace rr::platform {
namespace {
struct Vector { double x, y, z; };

Vector cross(const Vector& a, const Vector& b) noexcept {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}
double dot(const Vector& a, const Vector& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
bool normalize(Vector& v) noexcept {
    const double squared = dot(v, v);
    if (!std::isfinite(squared) || squared <= 0) return false;
    const double length = std::sqrt(squared);
    v = {v.x / length, v.y / length, v.z / length};
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
bool fail(VehicleViewCameraError50DG& error, VehicleViewCameraError50DG value) noexcept {
    error = value;
    return false;
}
}

bool composeVehicleViewCamera50DG(const VehicleViewCamera50DG& input,
    VehicleViewMatrix50DG& output, VehicleViewCameraError50DG& error) noexcept {
    error = VehicleViewCameraError50DG::none;
    for (unsigned i = 0; i < 3; ++i)
        if (!std::isfinite(input.eye[i]) || !std::isfinite(input.target[i]) ||
            !std::isfinite(input.up[i]))
            return fail(error, VehicleViewCameraError50DG::nonfiniteInput);
    if (!std::isfinite(input.verticalFovRadians) ||
        !std::isfinite(input.aspectWidthOverHeight) ||
        !std::isfinite(input.nearPlane) || !std::isfinite(input.farPlane))
        return fail(error, VehicleViewCameraError50DG::nonfiniteInput);
    constexpr double pi = 3.141592653589793238462643383279502884;
    if (!(input.verticalFovRadians > 0 && input.verticalFovRadians < pi &&
        input.aspectWidthOverHeight > 0 && input.nearPlane > 0 &&
        input.farPlane > input.nearPlane))
        return fail(error, VehicleViewCameraError50DG::projection);

    const Vector eye{input.eye[0], input.eye[1], input.eye[2]};
    const Vector up{input.up[0], input.up[1], input.up[2]};
    Vector backward{eye.x - input.target[0], eye.y - input.target[1],
        eye.z - input.target[2]};
    // Cross before normalization: independently rounded normalizations must
    // not manufacture a right axis from exactly proportional input vectors.
    Vector right = cross(up, backward);
    if (!normalize(backward))
        return fail(error, VehicleViewCameraError50DG::viewDirection);
    if (!normalize(right))
        return fail(error, VehicleViewCameraError50DG::upDirection);
    Vector correctedUp = cross(backward, right);
    if (!normalize(correctedUp))
        return fail(error, VehicleViewCameraError50DG::upDirection);

    const double yScale = 1.0 / std::tan(0.5 * input.verticalFovRadians);
    const double xScale = yScale / input.aspectWidthOverHeight;
    const double near = input.nearPlane, far = input.farPlane;
    const double depth = far - near;
    const double zScale = -(far + near) / depth;
    const double zOffset = -(2.0 * near * far) / depth;
    const double tx = -dot(right, eye), ty = -dot(correctedUp, eye);
    const double tz = -dot(backward, eye);

    // V * P_GL in column-major storage for vec4(world,1) * matrix. Equivalently,
    // start with 48B's P_D3D and replace clip Z by 2 * clip Z - clip W. The
    // DirectX/HLSL CPU transpose was packing, not a different vector convention.
    const double matrix[16] = {
        right.x * xScale, right.y * xScale, right.z * xScale, tx * xScale,
        correctedUp.x * yScale, correctedUp.y * yScale,
        correctedUp.z * yScale, ty * yScale,
        backward.x * zScale, backward.y * zScale,
        backward.z * zScale, tz * zScale + zOffset,
        -backward.x, -backward.y, -backward.z, -tz
    };
    VehicleViewMatrix50DG next{};
    for (unsigned i = 0; i < 16; ++i) {
        if (!std::isfinite(matrix[i]) ||
            std::fabs(matrix[i]) > std::numeric_limits<float>::max())
            return fail(error, VehicleViewCameraError50DG::matrixRange);
        const float value = static_cast<float>(matrix[i]);
        if (!std::isfinite(value) || (value == 0 && matrix[i] != 0))
            return fail(error, VehicleViewCameraError50DG::matrixRange);
        next.worldViewProjection[i] = value;
    }
    output = next;
    return true;
}
}
