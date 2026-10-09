#include "native_camera_view50dx.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Native camera conversion requires finite checks and ordered IEEE arithmetic"
#endif

namespace rr::platform {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix == 2 &&
    std::numeric_limits<float>::digits == 24 &&
    std::numeric_limits<float>::max_exponent == 128,
    "Native camera conversion requires IEEE binary32");

bool fail(NativeCameraViewError50DX& error, NativeCameraViewError50DX value) noexcept {
    error = value;
    return false;
}

bool coreLayout(const game::CameraViewOutput49F& source) noexcept {
    // 14DC80's affine view remains affine after the optional 14E070 roll.
    // Compare values, not zero signs: original arithmetic may produce -0.
    if (source.view[3] != 0 || source.view[7] != 0 || source.view[11] != 0 ||
        source.view[15] != 1 || source.projection[11] != 1)
        return false;
    // 14DB90 writes only h, h/aspect, z, 1 and -z*near into a zero matrix.
    // Do not impose new FOV/near/far/basis restrictions on 49F's admitted domain.
    for (unsigned i = 0; i < 16; ++i)
        if (i != 0 && i != 5 && i != 10 && i != 11 && i != 14 &&
            source.projection[i] != 0)
            return false;
    return true;
}
}

bool convertNativeCameraView50DX(const game::CameraViewOutput49F& source,
    const NativeCameraViewContract50DX& contract, NativeCameraMatrix50DX& output,
    NativeCameraViewError50DX& error) noexcept {
    error = NativeCameraViewError50DX::none;
    if (contract.provenance != NativeCameraViewProvenance50DX::currentCore49FForOutput)
        return fail(error, NativeCameraViewError50DX::unavailableProvenance);
    if (contract.width <= 0 || contract.height <= 0)
        return fail(error, NativeCameraViewError50DX::outputDimensions);
    if (contract.clipDepth != NativeCameraClipDepth50DX::negativeOneToOne &&
        contract.clipDepth != NativeCameraClipDepth50DX::zeroToOne)
        return fail(error, NativeCameraViewError50DX::clipDepth);
    if (std::fegetround() != FE_TONEAREST)
        return fail(error, NativeCameraViewError50DX::unsupportedRounding);
    for (unsigned i = 0; i < 16; ++i)
        if (!std::isfinite(source.view[i]) || !std::isfinite(source.projection[i]) ||
            !std::isfinite(source.combined[i]))
            return fail(error, NativeCameraViewError50DX::nonfiniteMatrix);
    if (!coreLayout(source))
        return fail(error, NativeCameraViewError50DX::coreMatrixLayout);

    NativeCameraMatrix50DX next{};
    next.width = contract.width;
    next.height = contract.height;
    next.clipDepth = contract.clipDepth;
    for (unsigned row = 0; row < 4; ++row) {
        for (unsigned column = 0; column < 4; ++column) {
            const unsigned from = row * 4 + column, to = column * 4 + row;
            if (column == 2 && contract.clipDepth == NativeCameraClipDepth50DX::negativeOneToOne) {
                // Host GL boundary only. Preserve the already-rounded V*P;
                // converting P first and multiplying again is not equivalent.
                volatile float twiceZ = 2.0f * source.combined[from];
                if (!std::isfinite(twiceZ))
                    return fail(error, NativeCameraViewError50DX::matrixRange);
                volatile float glZ = twiceZ - source.combined[row * 4 + 3];
                if (!std::isfinite(glZ))
                    return fail(error, NativeCameraViewError50DX::matrixRange);
                next.worldViewProjection[to] = glZ;
            } else {
                std::memcpy(&next.worldViewProjection[to], &source.combined[from], sizeof(float));
            }
        }
    }
    output = next;
    return true;
}
}
