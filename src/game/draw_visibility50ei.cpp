#include "draw_visibility50ei.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
static_assert(std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::digits == 24 &&
    std::numeric_limits<double>::is_iec559 &&
    std::numeric_limits<double>::digits >= 53, "IEEE binary32/binary64 required");

bool fail(DrawVisibilityDiagnostic50EI& diagnostic, DrawVisibilityError50EI error,
    std::uint32_t ordinal = UINT32_MAX) noexcept {
    diagnostic = {error, ordinal};
    return false;
}
bool subnormal(float value) noexcept {
    return std::fpclassify(value) == FP_SUBNORMAL;
}
enum class Visibility { retained, culled, uncertain };

Visibility classify(const assets::Bounds48B& bounds, const float* matrix,
    DrawClipDepth50EI depth, bool uncertainMatrix) noexcept {
    if (uncertainMatrix) return Visibility::uncertain;
    for (unsigned axis = 0; axis < 3; ++axis)
        if (subnormal(bounds.minimum[axis]) || subnormal(bounds.maximum[axis]))
            return Visibility::uncertain;

    // Much wider than the roundoff bound for a four-term binary32 dot product
    // (with or without contraction/reassociation). The additive term also
    // covers tiny intermediate/results flushed to zero, with normal inputs.
    constexpr double roundoff = 16.0 * std::numeric_limits<float>::epsilon();
    constexpr double underflow = 16.0 * std::numeric_limits<float>::min();
    constexpr double safeMagnitude = std::numeric_limits<float>::max() / (1.0 + roundoff);
    unsigned commonOutside = 0x3fu;
    for (unsigned corner = 0; corner < 8; ++corner) {
        double point[3];
        for (unsigned axis = 0; axis < 3; ++axis)
            point[axis] = (corner & (1u << axis)) ? bounds.maximum[axis] : bounds.minimum[axis];
        double clip[4], error[4];
        for (unsigned column = 0; column < 4; ++column) {
            const float* m = matrix + column * 4;
            const double a = point[0] * double(m[0]);
            const double b = point[1] * double(m[1]);
            const double c = point[2] * double(m[2]);
            const double d = m[3];
            const double magnitude = std::fabs(a) + std::fabs(b) + std::fabs(c) + std::fabs(d);
            clip[column] = ((a + b) + c) + d;
            // Do not trust cancellation to rescue potentially overflowing
            // shader intermediates. Uncertain arithmetic retains the WHOLE box.
            if (!std::isfinite(clip[column]) || !std::isfinite(magnitude) || magnitude > safeMagnitude)
                return Visibility::uncertain;
            error[column] = roundoff * magnitude + underflow;
        }
        // Inward signed distances, never clip / W. Strictly negative even
        // after the full error allowance is the only evidence for exclusion.
        const double xMargin = error[0] + error[3];
        const double yMargin = error[1] + error[3];
        const double zMargin = error[2] + error[3];
        unsigned outside = 0;
        if (clip[0] + clip[3] < -xMargin) outside |= 1u;
        if (clip[3] - clip[0] < -xMargin) outside |= 2u;
        if (clip[1] + clip[3] < -yMargin) outside |= 4u;
        if (clip[3] - clip[1] < -yMargin) outside |= 8u;
        if (depth == DrawClipDepth50EI::negativeOneToOne) {
            if (clip[2] + clip[3] < -zMargin) outside |= 16u;
        } else if (clip[2] < -error[2]) outside |= 16u;
        if (clip[3] - clip[2] < -zMargin) outside |= 32u;
        commonOutside &= outside;
    }
    return commonOutside ? Visibility::culled : Visibility::retained;
}
}

bool collectVisibleStaticDraws50EI(const DrawVisibilityInput50EI& input,
    const DrawVisibilityBuffer50EI& buffer, DrawVisibilityResult50EI& output,
    DrawVisibilityDiagnostic50EI& diagnostic) noexcept {
    diagnostic = {};
#if defined(__FAST_MATH__)
    return fail(diagnostic, DrawVisibilityError50EI::unsupportedArithmetic);
#endif
    if (std::fegetround() != FE_TONEAREST)
        return fail(diagnostic, DrawVisibilityError50EI::unsupportedArithmetic);
    if (!input.worldViewProjection || (input.drawCount && !input.draws) ||
        (buffer.capacity && !buffer.drawIds))
        return fail(diagnostic, DrawVisibilityError50EI::storage);
    if (input.clipDepth != DrawClipDepth50EI::negativeOneToOne &&
        input.clipDepth != DrawClipDepth50EI::zeroToOne)
        return fail(diagnostic, DrawVisibilityError50EI::clipDepth);
    if (input.uncertainPolicy != DrawUncertainPolicy50EI::retain &&
        input.uncertainPolicy != DrawUncertainPolicy50EI::reject)
        return fail(diagnostic, DrawVisibilityError50EI::uncertainPolicy);
    bool uncertainMatrix = false;
    for (unsigned i = 0; i < 16; ++i) {
        const float value = input.worldViewProjection[i];
        if (!std::isfinite(value))
            return fail(diagnostic, DrawVisibilityError50EI::nonfiniteMatrix);
        uncertainMatrix = uncertainMatrix || subnormal(value);
    }
    DrawVisibilityResult50EI next{};
    for (std::uint32_t i = 0; i < input.drawCount; ++i) {
        const auto& draw = input.draws[i];
        for (unsigned axis = 0; axis < 3; ++axis)
            if (!std::isfinite(draw.worldBounds.minimum[axis]) ||
                !std::isfinite(draw.worldBounds.maximum[axis]) ||
                draw.worldBounds.minimum[axis] > draw.worldBounds.maximum[axis])
                return fail(diagnostic, DrawVisibilityError50EI::invalidBounds, i);
        const auto visibility = classify(draw.worldBounds, input.worldViewProjection,
            input.clipDepth, uncertainMatrix);
        if (visibility == Visibility::culled) {
            ++next.culled;
            continue;
        }
        if (visibility == Visibility::uncertain) {
            if (input.uncertainPolicy == DrawUncertainPolicy50EI::reject)
                return fail(diagnostic, DrawVisibilityError50EI::uncertainTransform, i);
            ++next.uncertainRetained;
        }
        if (next.retained == buffer.capacity)
            return fail(diagnostic, DrawVisibilityError50EI::capacity, i);
        buffer.drawIds[next.retained++] = draw.drawId;
    }
    output = next;
    return true;
}
}
