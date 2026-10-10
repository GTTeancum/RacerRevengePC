#pragma once
#include "../../../src/game/camera_view49f.h"
#include <cstdint>

namespace rr::platform {
// Caller attestations, not evidence that a camera/frame scheduler ran.
enum class NativeCameraViewProvenance50DX : std::uint8_t {
    unavailable, currentCore49FForOutput
};
enum class NativeCameraClipDepth50DX : std::uint8_t {
    unspecified, negativeOneToOne, zeroToOne
};

struct NativeCameraViewContract50DX {
    // Set only for a current successful composeCameraView49F or
    // updateTVCameraView49F result, before any PS2 viewport/screen adjustment.
    // Its original scene projection inputs, especially aspect1F8, must have
    // explicit provenance for the declared output. Defaults or a prior result
    // left intact by a failed producer do not establish this contract. A
    // composed game-frame caller also owns 49F's ordinary/build-enabled gates.
    NativeCameraViewProvenance50DX provenance = NativeCameraViewProvenance50DX::unavailable;
    // Actual drawable-pixel viewport dimensions; never logical window units.
    // These bind the supplied projection to an output, not a new aspect rule.
    // No width/height ratio, scene1DC or full-output dimensions are inferred.
    std::int32_t width = 0, height = 0;
    // Must match the receiving GL context. Existing 50DD/50DS clients use
    // negativeOneToOne. zeroToOne requires explicit GL clip-control support
    // and caller-established state; this adapter makes no GL calls.
    NativeCameraClipDepth50DX clipDepth = NativeCameraClipDepth50DX::unspecified;
};

struct NativeCameraMatrix50DX {
    // ROW-VECTOR world-to-GL-clip matrix, COLUMN-MAJOR storage:
    // worldViewProjection[column * 4 + row]. Pass directly to 50CZ's
    // GL_FALSE uniform upload. Never transpose or remap depth a second time.
    // Input vertices are already world-space; no model/root matrix is added.
    float worldViewProjection[16]{};
    std::int32_t width = 0, height = 0;
    NativeCameraClipDepth50DX clipDepth = NativeCameraClipDepth50DX::unspecified;
};

enum class NativeCameraViewError50DX : std::uint8_t {
    none, unavailableProvenance, outputDimensions, clipDepth,
    unsupportedRounding, nonfiniteMatrix, coreMatrixLayout, matrixRange
};

// Consume the already-produced 49F core combined matrix. Do not reconstruct
// eye/target/up, FOV, aspect or V*P; their original binary32 order stays in 49F.
// Native projection has positive view-forward Z, W=Z and clip Z in [0,W].
// zeroToOne copies all sixteen words to the new packing. negativeOneToOne
// changes only each row's Z coefficient to (2 * combined[row,2]) -
// combined[row,3], using a separately rounded binary32 multiply and subtract.
// X/Y/W retain their exact bits. No Y flip, handedness swap or PS2 screen map.
//
// Requires strict IEEE binary32, nearest-even rounding, no fast math or
// contraction. Rejects unavailable/unknown tags, nonpositive output sizes,
// nonfinite matrices, incompatible core matrix layout and nonfinite conversion
// intermediates/results. Structural checks do not authenticate provenance or
// prove that source matrices belong to the same successful producer call.
// No epsilon, clamp, basis repair, projection fallback or camera selection.
//
// Stateless, fixed-size, allocation-free. No retained references or callbacks.
// All four arguments must be distinct live objects; source/contract stay
// unchanged through the call. Failure leaves output unchanged; error always
// describes this call. On success the renderer must use the returned dimensions
// and depth convention. Re-establish projection provenance after a resize;
// this function neither resizes a viewport nor updates original scene state.
bool convertNativeCameraView50DX(const game::CameraViewOutput49F& source,
    const NativeCameraViewContract50DX& contract, NativeCameraMatrix50DX& output,
    NativeCameraViewError50DX& error) noexcept;
}
