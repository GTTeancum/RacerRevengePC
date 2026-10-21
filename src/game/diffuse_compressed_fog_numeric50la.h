#pragma once
#ifndef RR_DIFFUSE_COMPRESSED_FOG_NUMERIC50LA_H
#define RR_DIFFUSE_COMPRESSED_FOG_NUMERIC50LA_H
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

// Numeric snapshot evaluator ONLY. No source model, camera publication, clip
// selection, material, texture, lifetime or draw authority is established here.
// Source: StripPreamble 340188..340218, positive-palette DiffuseCompressed
// 338D68..3395F8. Native binary32/sqrt are NOT PS2 VU/EFU emulation or parity.
namespace diffuse_compressed_fog_numeric50LA {
using Matrix = std::array<float, 16>; // Actual packet rows25..28, translation12..14.
using Vec3 = std::array<float, 3>;
enum class Status { complete, input, arithmetic, storage };
enum class Stream { clippedRaw, ordinaryScaled };
constexpr std::size_t maxVertices = 8192, maxPalette = 64, maxGroupVertices = 16;
constexpr std::uint16_t radial02 = 0x02, clip04 = 0x04, compressed40 = 0x40;

struct RawVertex {
    std::array<std::int32_t, 3> xyz{}; // Original integer XYZ, before quantization.
    std::int32_t signedW = 0;        // Original signed bone/ADC: +/-16*(bone+1).
};
struct VifGroup { std::size_t firstVertex = 0, vertexCount = 0; };
struct StripSpan {
    std::size_t group = 0, start = 0, end = 0, vertexStart = 0;
    // Generic source positive-first entry, not claimed for original POD26.
    // Only the first span of a VIF group may be positive-first. Normal source
    // split-strip continuations still repeat two negative entry vertices.
    bool positiveFirst = false;
};
struct Preamble {
    Vec3 localCameraRaw{};
    float endTimesUnscaledSlope = 0, scaledSlope = 0;
};
struct Packet {
    const Matrix* palette = nullptr;
    std::size_t paletteCount = 0;
    const RawVertex* vertices = nullptr;
    std::size_t vertexCount = 0;
    const VifGroup* groups = nullptr;
    std::size_t groupCount = 0;
    const StripSpan* strips = nullptr;
    std::size_t stripCount = 0;
    Vec3 camera180{};
    float positionScale = 0, fogEnd33Z = 0, fogSlope33W = 0;
    std::uint16_t sourceFlags41X = 0;
};
struct Workspace {
    Preamble* palette = nullptr;
    std::size_t paletteCapacity = 0;
    std::uint8_t* fogBytes = nullptr;
    std::size_t fogCapacity = 0;
};
template<class T> struct OutputBuffer { T* fogBytes = nullptr; std::size_t capacity = 0; };
using Output = OutputBuffer<std::uint8_t>;
using FloatOutput = OutputBuffer<float>; // Exact byte-valued floats0..255, NOT divided by256.

namespace detail {
inline float add(float a, float b) noexcept { volatile float r = a + b; return r; }
inline float sub(float a, float b) noexcept { volatile float r = a - b; return r; }
inline float mul(float a, float b) noexcept { volatile float r = a * b; return r; }
inline float div(float a, float b) noexcept { volatile float r = a / b; return r; }
inline std::uint32_t bits(float value) noexcept {
    std::uint32_t result = 0; std::memcpy(&result, &value, sizeof(result)); return result;
}
inline bool finite(float value) noexcept { return std::isfinite(value); }
inline bool ready() noexcept {
#if defined(__FAST_MATH__)
    return false; // Reassociation/finite-math-only violates this numeric contract.
#else
    if (sizeof(float) != sizeof(std::uint32_t) ||
        !std::numeric_limits<float>::is_iec559 ||
        std::numeric_limits<float>::radix != 2 ||
        std::numeric_limits<float>::digits != 24 ||
        std::numeric_limits<float>::max_exponent != 128 ||
        std::fegetround() != FE_TONEAREST) return false;
    // Require native gradual underflow rather than silently inheriting FTZ/DAZ.
    volatile float tiny = std::numeric_limits<float>::denorm_min();
    volatile float normal = std::numeric_limits<float>::min();
    volatile float one = 1.f, half = .5f;
    return bits(mul(tiny, one)) == 1u && bits(mul(normal, half)) == 0x00400000u;
#endif
}
struct Math {
    bool bad = false;
    float check(float value) noexcept { bad = bad || !finite(value); return value; }
    float add(float a, float b) noexcept { return check(detail::add(a, b)); }
    float sub(float a, float b) noexcept { return check(detail::sub(a, b)); }
    float mul(float a, float b) noexcept { return check(detail::mul(a, b)); }
};
inline bool validMatrix(const Matrix& matrix) noexcept {
    for (float value : matrix) if (!finite(value)) return false;
    return matrix[3] == 0.f && matrix[7] == 0.f && matrix[11] == 0.f && matrix[15] == 1.f;
}
inline bool bone(std::int32_t signedW, std::size_t count, std::size_t& result) noexcept {
    // Decode in64 bits, so INT_MIN cannot overflow abs/negation.
    const std::int64_t w = signedW;
    const std::uint64_t magnitude = static_cast<std::uint64_t>(w < 0 ? -w : w);
    if (magnitude == 0 || magnitude > 32767 || (magnitude & 15u) != 0) return false;
    result = static_cast<std::size_t>(magnitude / 16u - 1u);
    return result < count;
}
inline bool rawPosition(const std::array<std::int32_t, 3>& xyz) noexcept {
    for (std::int32_t value : xyz) if (value < -8388608 || value > 8388607) return false;
    return true;
}
struct Range { std::uintptr_t first = 0, last = 0; };
inline bool range(const void* pointer, std::size_t count, std::size_t size, Range& out) noexcept {
    if (!pointer || count == 0 || count > std::numeric_limits<std::size_t>::max() / size) return false;
    const auto first = reinterpret_cast<std::uintptr_t>(pointer);
    const auto bytes = count * size;
    if (bytes > std::numeric_limits<std::uintptr_t>::max() - first) return false;
    out = {first, first + bytes}; return true;
}
inline bool overlaps(Range a, Range b) noexcept { return a.first < b.last && b.first < a.last; }
template<class T>
inline Status validate(const Packet& in, Workspace scratch, OutputBuffer<T> output) noexcept {
    if (in.paletteCount == 0 || in.paletteCount > maxPalette ||
        in.vertexCount == 0 || in.vertexCount > maxVertices ||
        in.groupCount == 0 || in.groupCount > in.vertexCount / 3 ||
        in.stripCount == 0 || in.stripCount > in.vertexCount / 3 ||
        !(in.sourceFlags41X & compressed40) || !finite(in.positionScale) || in.positionScale <= 0 ||
        !finite(in.fogEnd33Z) || !finite(in.fogSlope33W)) return Status::input;
    for (float value : in.camera180) if (!finite(value)) return Status::input;
    if (scratch.paletteCapacity < in.paletteCount || scratch.fogCapacity < in.vertexCount ||
        output.capacity < in.vertexCount) return Status::storage;
    Range reads[5], writes[3];
    if (!range(&in, 1, sizeof(in), reads[0]) ||
        !range(in.palette, in.paletteCount, sizeof(Matrix), reads[1]) ||
        !range(in.vertices, in.vertexCount, sizeof(RawVertex), reads[2]) ||
        !range(in.groups, in.groupCount, sizeof(VifGroup), reads[3]) ||
        !range(in.strips, in.stripCount, sizeof(StripSpan), reads[4])) return Status::input;
    if (!range(scratch.palette, in.paletteCount, sizeof(Preamble), writes[0]) ||
        !range(scratch.fogBytes, in.vertexCount, 1, writes[1]) ||
        !range(output.fogBytes, in.vertexCount, sizeof(T), writes[2])) return Status::storage;
    for (std::size_t i = 0; i < 3; ++i) {
        for (Range read : reads) if (overlaps(writes[i], read)) return Status::storage;
        for (std::size_t j = 0; j < i; ++j) if (overlaps(writes[i], writes[j])) return Status::storage;
    }
    for (std::size_t i = 0; i < in.paletteCount; ++i) if (!validMatrix(in.palette[i])) return Status::input;
    for (std::size_t i = 0; i < in.vertexCount; ++i) {
        std::size_t selected = 0;
        if (!rawPosition(in.vertices[i].xyz) || !bone(in.vertices[i].signedW, in.paletteCount, selected))
            return Status::input;
    }
    // The exact ordered groups and spans must partition every source vertex.
    // Strip boundaries validate metadata; they NEVER reset the radial pipeline.
    std::size_t nextVertex = 0, nextStrip = 0;
    for (std::size_t g = 0; g < in.groupCount; ++g) {
        const auto& group = in.groups[g];
        if (group.firstVertex != nextVertex || group.vertexCount < 3 || group.vertexCount > maxGroupVertices ||
            group.vertexCount > in.vertexCount - nextVertex) return Status::input;
        std::size_t nextLocal = 0;
        while (nextStrip < in.stripCount && in.strips[nextStrip].group == g) {
            const auto& strip = in.strips[nextStrip++];
            if (strip.start != nextLocal || strip.end <= strip.start || strip.end > group.vertexCount ||
                strip.end - strip.start < 3 || strip.vertexStart != nextVertex + strip.start ||
                (strip.positiveFirst && strip.start != 0)) return Status::input;
            for (std::size_t n = strip.start; n < strip.end; ++n) {
                const bool mustBeNegative = !strip.positiveFirst && n - strip.start < 2;
                if ((in.vertices[nextVertex + n].signedW < 0) != mustBeNegative) return Status::input;
            }
            nextLocal = strip.end;
        }
        if (nextLocal != group.vertexCount) return Status::input;
        nextVertex += group.vertexCount;
    }
    return nextVertex == in.vertexCount && nextStrip == in.stripCount ? Status::complete : Status::input;
}
} // namespace detail

// This accepts each ACTUAL current packet matrix; no inversion, bind-matrix
// stand-in, first-palette shortcut or global-camera substitution is performed.
// Output is unchanged on failure. The reciprocal is freshly rounded binary32,
// not the original file's unrelated quantization[3].
inline Status prepareCompressed(const Matrix& matrix, const Vec3& camera180,
    float positionScale, float end33Z, float slope33W, Preamble& output) noexcept {
    using namespace detail;
    if (!ready()) return Status::arithmetic;
    if (!validMatrix(matrix) || !finite(positionScale) || positionScale <= 0 ||
        !finite(end33Z) || !finite(slope33W)) return Status::input;
    for (float value : camera180) if (!finite(value)) return Status::input;
    const float inverseScale = div(1.f, positionScale);
    if (!finite(inverseScale)) return Status::arithmetic;
    Math math; Preamble next; Vec3 delta{};
    for (unsigned c = 0; c < 3; ++c) delta[c] = math.sub(camera180[c], matrix[12 + c]); //340188
    for (unsigned axis = 0; axis < 3; ++axis) {
        const float x = math.mul(delta[0], matrix[axis * 4]);
        const float y = math.mul(delta[1], matrix[axis * 4 + 1]);
        const float z = math.mul(delta[2], matrix[axis * 4 + 2]);
        //3401C0..3401F0: X=(X+Y)+Z, Y=(Y+X)+Z, Z=(Z+X)+Y.
        const float local = axis == 0 ? math.add(math.add(x, y), z) :
            axis == 1 ? math.add(math.add(y, x), z) : math.add(math.add(z, x), y);
        next.localCameraRaw[axis] = local;
    }
    next.endTimesUnscaledSlope = math.mul(end33Z, slope33W); //3401F8, BEFORE compression.
    next.scaledSlope = math.mul(slope33W, positionScale); //340210
    for (float& component : next.localCameraRaw) component = math.mul(component, inverseScale); //340218
    if (math.bad) return Status::arithmetic;
    output = next; return Status::complete;
}

// Single numeric radial leaf. Its caller must choose the pipeline's camera
// operand, not this vertex's own bone. Explicit stream preserves339360 vs338EB8.
inline Status radialByte(const Preamble& in, const std::array<std::int32_t, 3>& rawXYZ,
    Stream stream, float positionScale, std::uint8_t& output) noexcept {
    using namespace detail;
    if (!ready()) return Status::arithmetic;
    if (!rawPosition(rawXYZ) || !finite(positionScale) || positionScale <= 0 ||
        !finite(in.endTimesUnscaledSlope) || !finite(in.scaledSlope) ||
        (stream != Stream::clippedRaw && stream != Stream::ordinaryScaled)) return Status::input;
    Math math; Vec3 delta{};
    for (unsigned c = 0; c < 3; ++c) {
        if (!finite(in.localCameraRaw[c])) return Status::input;
        float position = static_cast<float>(rawXYZ[c]); //exact ITOF0 for signed24 subset.
        if (stream == Stream::ordinaryScaled) position = math.mul(position, positionScale);
        delta[c] = math.sub(position, in.localCameraRaw[c]);
    }
    const float xy = math.add(math.mul(delta[0], delta[0]), math.mul(delta[1], delta[1]));
    const float squared = math.add(xy, math.mul(delta[2], delta[2]));
    if (math.bad || squared < 0) return Status::arithmetic;
    const float distance = std::sqrt(squared); // Native sqrt, NOT PS2 ELENG/EFU parity.
    const float base = math.sub(524288.f, in.endTimesUnscaledSlope);
    float biased = math.add(base, math.mul(distance, in.scaledSlope));
    if (math.bad || !finite(distance)) return Status::arithmetic;
    if (biased < 524288.f) biased = 524288.f;
    if (biased > 524543.f) biased = 524543.f;
    output = static_cast<std::uint8_t>((bits(biased) >> 4) & 255u);
    return Status::complete;
}

// Allocation-free hot API. Scratch may change on failure; output bytes NEVER
// do. Input, scratch and output must be disjoint live arrays of the stated size.
// All source vertices (including suppressed ADC vertices) keep source order.
// Success writes exactly vertexCount elements; output's trailing capacity is kept.
template<class T>
inline Status evaluate(const Packet& in, Workspace scratch, OutputBuffer<T> output) noexcept {
    static_assert(std::is_same<T, std::uint8_t>::value || std::is_same<T, float>::value,
        "Fog output must be bytes or exact byte-valued floats");
    if (!detail::ready()) return Status::arithmetic;
    auto status = detail::validate(in, scratch, output);
    if (status != Status::complete) return status;
    for (std::size_t bone = 0; bone < in.paletteCount; ++bone) {
        status = prepareCompressed(in.palette[bone], in.camera180, in.positionScale,
            in.fogEnd33Z, in.fogSlope33W, scratch.palette[bone]);
        if (status != Status::complete) return status;
    }
    if (!(in.sourceFlags41X & radial02)) {
        // Only source bit02-clear authorizes F255, never FGE0 or a failed radial calculation.
        std::memset(scratch.fogBytes, 255, in.vertexCount);
    } else {
        for (std::size_t g = 0; g < in.groupCount; ++g) {
            const auto& group = in.groups[g];
            const Stream stream = (in.sourceFlags41X & clip04) || in.vertices[group.firstVertex].signedW > 0 ?
                Stream::clippedRaw : Stream::ordinaryScaled; //339338 positive-first override.
            for (std::size_t n = 0; n < group.vertexCount; ++n) {
                // Prefetch precedes palette switch: n0,n1,n2 use camera0;
                // n>=2 uses bone[n-2]. Reset ONLY at a real VIF group boundary.
                const std::size_t operand = group.firstVertex + (n < 2 ? 0 : n - 2);
                std::size_t cameraBone = 0;
                if (!detail::bone(in.vertices[operand].signedW, in.paletteCount, cameraBone)) return Status::input;
                status = radialByte(scratch.palette[cameraBone], in.vertices[group.firstVertex + n].xyz,
                    stream, in.positionScale, scratch.fogBytes[group.firstVertex + n]);
                if (status != Status::complete) return status;
            }
        }
    }
    if constexpr (std::is_same<T, std::uint8_t>::value)
        std::memcpy(output.fogBytes, scratch.fogBytes, in.vertexCount);
    else
        for (std::size_t i = 0; i < in.vertexCount; ++i) output.fogBytes[i] = static_cast<float>(scratch.fogBytes[i]);
    return Status::complete;
}
} // namespace diffuse_compressed_fog_numeric50LA

#endif
