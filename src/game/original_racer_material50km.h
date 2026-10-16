#pragma once
#include "original_racer_light_selection50km.h"
#include <cstring>

namespace rr::game::original_racer_lighting50KM {
using CompressedNormal = std::array<std::int8_t, 3>;

struct DiffuseProgramInputs {
    // Original effective normal transform, after the selected bone/model
    // basis, light-direction matrix and authored normal quantization have
    // been composed by StripPreamble. Rows correspond to original vf5..7.
    // Index [normal axis][selected light]. No world-normal normalization is
    // performed here, and native derivative normals are not a substitute.
    std::array<Vec3, 3> normalAxes{};
    // Original vf9..12 RGB. Rows 0..2 are light colors; row 3 is ambient.
    // These are source byte-scale color coefficients, not normalized 0..1.
    std::array<Vec3, 4> colorRows{};
};

struct DiffuseVertexRgb {
    // Color bytes before the actual texture function/GS fragment operations.
    // Do not multiply imported vertex RGB into these source-generated colors.
    std::array<std::uint8_t, 3> rgb{};
};

namespace material_detail {
inline float add(float a, float b) noexcept { volatile float r = a + b; return r; }
inline float mul(float a, float b) noexcept { volatile float r = a * b; return r; }
inline bool finite(float x) noexcept { return std::isfinite(x); }
inline bool dot(const std::array<Vec3, 3>& axes, const CompressedNormal& normal,
    unsigned outputAxis, float& output) noexcept {
    float value = mul(axes[0][outputAxis], static_cast<float>(normal[0]));
    if (!finite(value)) return false;
    for (unsigned axis = 1; axis < 3; ++axis) {
        const float product = mul(axes[axis][outputAxis], static_cast<float>(normal[axis]));
        if (!finite(product)) return false;
        value = add(value, product);
        if (!finite(value)) return false;
    }
    output = value;
    return true;
}
}

// Native binary32 reconstruction of the shared DiffuseCompressed and
// SpecularMapDualCompressed RGB equation. This is semantic native math, not
// an instruction interpreter, and does not certify EE/VU exceptional modes.
// The input matrices must come from actual light selection and current source
// transforms. Their provenance, palette and publication identity stay owned by
// the caller; this leaf does not manufacture those inputs or select materials.
// Admitted final RGB is nonnegative. The source negative/wrapping packet case
// remains unsupported rather than being silently replaced with a black clamp.
inline Status projectDiffuseCompressedRgb(const DiffuseProgramInputs& in,
    const CompressedNormal& normal, DiffuseVertexRgb& output) noexcept {
    if (!selection_detail::arithmeticReady()) return Status::arithmetic;
    for (const auto& row : in.normalAxes)
        for (float x : row) if (!material_detail::finite(x)) return Status::input;
    for (const auto& row : in.colorRows)
        for (float x : row) if (!material_detail::finite(x)) return Status::input;
    Vec3 amount{};
    for (unsigned light = 0; light < 3; ++light) {
        float value = 0.f;
        if (!material_detail::dot(in.normalAxes, normal, light, value)) return Status::arithmetic;
        // 337868..337878 and 3378B8; same calculation at 32CA40..32CA70.
        amount[light] = value > 0.f ? value : 0.f;
    }
    DiffuseVertexRgb next;
    for (unsigned c = 0; c < 3; ++c) {
        // ADDAz starts the accumulation with ambient + 2^23, before any
        // directional color contributions. The bias is not a final round().
        // Preserve the three subsequent multiply/add steps in source order.
        float packed = material_detail::add(in.colorRows[3][c], 8388608.f);
        if (!material_detail::finite(packed)) return Status::arithmetic;
        for (unsigned light = 0; light < 3; ++light) {
            const float product = material_detail::mul(in.colorRows[light][c], amount[light]);
            if (!material_detail::finite(product)) return Status::arithmetic;
            packed = material_detail::add(packed, product);
            if (!material_detail::finite(packed)) return Status::arithmetic;
        }
        // 337958 / 32CAC8: upper clamp is float(2^23 + 255), followed by
        // writing raw float bits as packed color. There is no RGB FTOI0 here.
        if (packed > 8388863.f) packed = 8388863.f;
        if (packed < 8388608.f) return Status::arithmetic;
        std::uint32_t bits = 0;
        std::memcpy(&bits, &packed, sizeof(bits));
        next.rgb[c] = static_cast<std::uint8_t>(bits & 0xffu);
    }
    output = next;
    return Status::complete;
}

struct DiffuseRgbaProgramInputs {
    // Same original vf5..7 coefficients as the RGB-only leaf. The caller
    // composes scaled selected direction rows with the actual node basis.
    std::array<Vec3, 3> normalAxes{};
    // Original vf9..12, including the actual selected ambient fourth lane.
    Matrix colorRows{};
};
struct DiffuseVertexRgba {
    std::array<std::uint8_t, 4> rgba{};
};

//3378F8..337978 operates on all four color lanes, as retained by the UI
// source vertex projection. Keep the older RGB-only leaf unchanged. This
// source-generated alpha is packet data; consuming it as framebuffer alpha
// still requires the separate original gameplay alpha/fragment-state owner.
// The same finite/nonnegative packed domain and transactional output apply.
namespace material_detail {
// Caller has admitted arithmeticReady() and every coefficient as finite.
// Inputs and FP mode stay stable throughout the serial, callback-free loop.
inline Status projectDiffuseCompressedRgbaFiniteInputs(const DiffuseRgbaProgramInputs& in,
    const CompressedNormal& normal, DiffuseVertexRgba& output) noexcept {
    Vec3 amount{};
    for (unsigned light = 0; light < 3; ++light) {
        float value = 0.f;
        if (!material_detail::dot(in.normalAxes, normal, light, value)) return Status::arithmetic;
        amount[light] = value > 0.f ? value : 0.f;
    }
    DiffuseVertexRgba next;
    for (unsigned c = 0; c < 4; ++c) {
        // ADDAz ambient + 2^23 precedes each of the three ordered light
        // multiply/add operations. No imported vertex color is multiplied.
        float packed = material_detail::add(in.colorRows[3][c], 8388608.f);
        if (!material_detail::finite(packed)) return Status::arithmetic;
        for (unsigned light = 0; light < 3; ++light) {
            const float product = material_detail::mul(in.colorRows[light][c], amount[light]);
            if (!material_detail::finite(product)) return Status::arithmetic;
            packed = material_detail::add(packed, product);
            if (!material_detail::finite(packed)) return Status::arithmetic;
        }
        if (packed > 8388863.f) packed = 8388863.f;
        if (packed < 8388608.f) return Status::arithmetic;
        std::uint32_t bits = 0;
        std::memcpy(&bits, &packed, sizeof(bits));
        next.rgba[c] = static_cast<std::uint8_t>(bits & 0xffu);
    }
    output = next;
    return Status::complete;
}
} // namespace material_detail

inline Status projectDiffuseCompressedRgba(const DiffuseRgbaProgramInputs& in,
    const CompressedNormal& normal, DiffuseVertexRgba& output) noexcept {
    if (!selection_detail::arithmeticReady()) return Status::arithmetic;
    for (const auto& row : in.normalAxes)
        for (float x : row) if (!material_detail::finite(x)) return Status::input;
    for (const auto& row : in.colorRows)
        for (float x : row) if (!material_detail::finite(x)) return Status::input;
    return material_detail::projectDiffuseCompressedRgbaFiniteInputs(in, normal, output);
}

struct SpecularMapDualInputs {
    // Effective source normal-to-view coefficients, already scaled by the
    // scene's authored normal quantization. This is vf19/vf20/vf13 for the
    // single-palette path, or vf1/vf2/vf3 for the multi-palette path.
    // Only the original two XY output lanes are consumed.
    std::array<std::array<float, 2>, 3> cameraNormalAxes{};
};

// Original second-map coordinates, before perspective S/T/Q packing. The two
// draw packets reuse identical Q so native perspective interpolation of this
// UV is the corresponding semantic representation. Clipping, triangle order,
// sampling, original map-2 blend/depth state and texture admission are external.
// No reflected-vector or guessed shininess equation is introduced.
namespace material_detail {
// Caller has admitted arithmeticReady() and every coefficient as finite.
// Inputs and FP mode stay stable throughout the serial, callback-free loop.
inline Status projectSpecularMapDualUvFiniteInputs(const SpecularMapDualInputs& in,
    const CompressedNormal& normal, std::array<float, 2>& output) noexcept {
    std::array<float, 2> next{};
    for (unsigned c = 0; c < 2; ++c) {
        float projected = material_detail::mul(in.cameraNormalAxes[0][c],
            static_cast<float>(normal[0]));
        if (!material_detail::finite(projected)) return Status::arithmetic;
        for (unsigned axis = 1; axis < 3; ++axis) {
            const float product = material_detail::mul(in.cameraNormalAxes[axis][c],
                static_cast<float>(normal[axis]));
            if (!material_detail::finite(product)) return Status::arithmetic;
            projected = material_detail::add(projected, product);
            if (!material_detail::finite(projected)) return Status::arithmetic;
        }
        // 32CA58..32CA78, 32CAB8, 32CAD8. No unit-length normalization,
        // Y flip or UV clamp occurs in the original coordinate equation.
        next[c] = material_detail::add(material_detail::mul(projected, 0.5f), 0.5f);
        if (!material_detail::finite(next[c])) return Status::arithmetic;
    }
    output = next;
    return Status::complete;
}
} // namespace material_detail

inline Status projectSpecularMapDualUv(const SpecularMapDualInputs& in,
    const CompressedNormal& normal, std::array<float, 2>& output) noexcept {
    if (!selection_detail::arithmeticReady()) return Status::arithmetic;
    for (const auto& row : in.cameraNormalAxes)
        for (float x : row) if (!material_detail::finite(x)) return Status::input;
    return material_detail::projectSpecularMapDualUvFiniteInputs(in, normal, output);
}
} // namespace rr::game::original_racer_lighting50KM
