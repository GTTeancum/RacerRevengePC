#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game::original_racer_lighting50KM {
using Vec3 = std::array<float, 3>;
using Vec4 = std::array<float, 4>;
using Matrix = std::array<Vec4, 4>;

enum class Status : std::uint8_t {
    complete, unsupportedRegistry, input, arithmetic
};

struct LightMatrices {
    // Original row-major layout: the XYZ of column i is the direction toward
    // selected light i. The fourth row is preserved separately from XYZ.
    Matrix directions{};
    // Rows 0..2 are selected lights; row 3 is the selected ambient. RGB remain
    // in source byte-scale units after Light::Init's division by two.
    Matrix diffuse{};
    Matrix specular{};
};

struct RegisteredLight {
    // Actual Light+34. This bounded leaf admits type 2 (directional) and
    // type 4 (ambient). Point/spot lights need their original distance path.
    std::uint32_t type = 0;
    Vec3 direction78{};       // Already constructed source look vector.
    Vec3 diffuse3C{};         // Already constructed/updated light RGB.
    Vec3 specular4C{};
};

struct SmallDirectionalRegistry {
    // Exact live manager+9C order, including ambient entries, and manager+94
    // count. Unused entries do not participate. This is not a scene-light list
    // substitute: constructor/registration and live updates belong to caller.
    std::array<RegisteredLight, 4> registered{};
    std::uint32_t count = 0;
    // Actual manager+60 row selected by visibility membership+4/+48; do not
    // replace it with a global ambient when that membership selects a zone.
    Vec4 selectedZoneAmbient{};
};

namespace selection_detail {
inline bool finite(float x) noexcept { return std::isfinite(x); }
inline float add(float a, float b) noexcept { volatile float r = a + b; return r; }
inline float mul(float a, float b) noexcept { volatile float r = a * b; return r; }
inline bool arithmeticReady() noexcept {
    return sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
        std::fegetround() == FE_TONEAREST;
}
}

// Native semantic projection of the directional/ambient subset of 149BB0.
// 14A128..14A150 selects this leaf only for manager+94 < 5, AFTER the real
// 14A860 cache lookup has missed or when the manager has no cache owner.
// Calling this function is not evidence that either cache condition holds.
// The authenticated GC scene's two authored lights fit this subset once their
// actual registration and zone-ambient owners have been joined. TA does not.
// Output is unchanged on every failure; there are no allocations or globals.
inline Status projectSmallDirectionalRegistry(const SmallDirectionalRegistry& in,
    LightMatrices& output) noexcept {
    if (in.count > in.registered.size()) return Status::unsupportedRegistry;
    if (!selection_detail::arithmeticReady()) return Status::arithmetic;
    for (float x : in.selectedZoneAmbient)
        if (!selection_detail::finite(x)) return Status::input;
    LightMatrices next;
    unsigned slot = 0;
    for (std::uint32_t i = 0; i < in.count; ++i) {
        const auto& light = in.registered[i];
        // 149C64..149C70 skips ambient entries without consuming a slot.
        if (light.type == 4) continue;
        if (light.type != 2 || slot == 3) return Status::unsupportedRegistry;
        for (unsigned c = 0; c < 3; ++c) {
            if (!selection_detail::finite(light.direction78[c]) ||
                !selection_detail::finite(light.diffuse3C[c]) ||
                !selection_detail::finite(light.specular4C[c])) return Status::input;
            // 149C80..149CB4 negates each stored look component and writes
            // it down column slot. No distance attenuation or renormalizing.
            next.directions[c][slot] = -light.direction78[c];
            next.diffuse[slot][c] = light.diffuse3C[c];
            next.specular[slot][c] = light.specular4C[c];
        }
        // 149EA8..149EBC clears the selected color fourth lanes, even though
        // the preceding stores briefly wrote raw integer bits 0x40 there.
        ++slot;
    }
    // 149ED8..149EE8 copies all four actual zone-ambient lanes. The source
    // leaves the specular ambient row and direction fourth row zero here.
    next.diffuse[3] = in.selectedZoneAmbient;
    output = next;
    return Status::complete;
}

// Native semantic projection of 149A50..149BA8. This is the final color step
// of the LARGE-registry path at 14A704, not an extra small-registry operation.
// selectedSunSlot must identify the actual sun row inserted at 14A43C..460;
// -1 means that the original optional sun pointer was null. Never infer slot 0.
// Directions, specular rows, and every fourth lane are retained unchanged.
inline Status foldSelectedNegativeLights(const LightMatrices& in,
    std::int32_t selectedSunSlot, LightMatrices& output) noexcept {
    if (selectedSunSlot < -1 || selectedSunSlot > 2) return Status::input;
    if (!selection_detail::arithmeticReady()) return Status::arithmetic;
    for (const auto& row : in.diffuse)
        for (unsigned c = 0; c < 3; ++c)
            if (!selection_detail::finite(row[c])) return Status::input;
    LightMatrices next = in;
    Vec3 negative{};
    // Preserve row-major accumulation order. Source C.LT.S is strict: -0
    // is not negative and is not replaced by this loop.
    for (unsigned row = 0; row < 3; ++row)
        for (unsigned c = 0; c < 3; ++c)
            if (next.diffuse[row][c] < 0.f) {
                negative[c] = selection_detail::add(negative[c], next.diffuse[row][c]);
                if (!selection_detail::finite(negative[c])) return Status::arithmetic;
                next.diffuse[row][c] = 0.f;
            }
    for (unsigned c = 0; c < 3; ++c) {
        if (selectedSunSlot >= 0) {
            auto& sun = next.diffuse[static_cast<unsigned>(selectedSunSlot)][c];
            const float value = selection_detail::add(sun,
                selection_detail::mul(0.75f, negative[c]));
            if (!selection_detail::finite(value)) return Status::arithmetic;
            sun = value > 0.f ? value : 0.f;
        }
        const float ambient = selection_detail::add(next.diffuse[3][c],
            selection_detail::mul(0.5f, negative[c]));
        if (!selection_detail::finite(ambient)) return Status::arithmetic;
        next.diffuse[3][c] = ambient > 0.f ? ambient : 0.f;
    }
    output = next;
    return Status::complete;
}
} // namespace rr::game::original_racer_lighting50KM
