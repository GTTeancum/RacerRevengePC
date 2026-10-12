#pragma once
// Source-only cold-light pose/default join. Not built or tested.
// Read only the already-selected LightN block. Never reinterpret a neighboring
// section name as one of its fields. The owning session authenticates SCB bytes.
#include "import_world48f.h"
#include "track_start50ad.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Original cold-light pose candidate requires strict finite IEEE arithmetic"
#endif

namespace rr::assets::original_light_pose_defaults_candidate {
using Vec3 = std::array<float, 3>;
using Vec4 = std::array<float, 4>;

enum class Status : std::uint8_t {
    complete, duplicateField, number, initialZone, nativeArithmetic
};
enum class LookSource : std::uint8_t { constructorDefault, lookVector, targetPosition };
struct Diagnostic {
    Status status = Status::complete;
    std::uint32_t sourcePC = 0;
    std::size_t sourceOffset = 0;
};
struct FreshPose {
    // Scene reader277390's heap vectors at +14/+18, before Light::Init.
    Vec4 position14{{0.f, 0.f, 0.f, 1.f}};
    Vec4 look18{{0.f, 0.f, 0.f, 1.f}};
    // Native one-based zone vocabulary; source record+20 contains zone-1.
    std::uint32_t initialZone = 1;
    LookSource lookSource = LookSource::constructorDefault;
    bool positionAuthored = false;
    bool zoneAuthored = false;
    // Fresh146720 ->146F60 projection. B0 starts zero but cached78 starts +X.
    // The zero-look equality path keeps both values, without normalization.
    Vec4 positionA0{{0.f, 0.f, 0.f, 1.f}};
    Vec4 lookB0{{0.f, 0.f, 0.f, 1.f}};
    Vec3 effectiveLook78{{1.f, 0.f, 0.f}};
    bool lookWasNormalized = false;
};

namespace detail {
inline bool fail(Diagnostic& d, Status status, std::uint32_t pc,
                 std::size_t offset) noexcept {
    d = {status, pc, offset};
    return false;
}
inline bool find(const ParameterBlock48F& b, const char* key,
                 const ParameterField48F*& output, Diagnostic& d,
                 std::uint32_t pc) noexcept {
    output = nullptr;
    for (const auto& f : b.fields) {
        if (f.name != key) continue;
        if (output) return fail(d, Status::duplicateField, pc, f.entry_offset);
        output = &f;
    }
    return true;
}
inline bool triple(const ParameterField48F& f, Vec4& output,
                   Diagnostic& d, std::uint32_t pc) noexcept {
    Vec4 next{{0.f, 0.f, 0.f, 1.f}};
    std::size_t at = 0;
    for (unsigned component = 0; component < 3; ++component) {
        const auto comma = f.value.find(',', at);
        if ((component < 2) == (comma == std::string::npos))
            return fail(d, Status::number, pc, f.value_offset + at);
        const auto end = component < 2 ? comma : f.value.size();
        auto first = at, last = end;
        while (first < last && f.value[first] == ' ') ++first;
        while (last > first && f.value[last - 1] == ' ') --last;
        if (!parseTrackNumber50AD(f.value.data() + first, last - first,
                                 next[component]))
            return fail(d, Status::number, pc, f.value_offset + first);
        at = end + 1;
    }
    output = next;
    return true;
}
inline float add(float a, float b) noexcept { volatile float r = a + b; return r; }
inline float mul(float a, float b) noexcept { volatile float r = a * b; return r; }
inline float sub(float a, float b) noexcept { volatile float r = a - b; return r; }
inline bool nonzero(const Vec4& v) noexcept {
    return v[0] != 0.f || v[1] != 0.f || v[2] != 0.f;
}
}

// Narrow admitted finite-decimal projection of277390's pose/zone fields and the
// fresh146720/146F60 initialization branch. Caller has already resolved Type and
// selected an existing nonambient LightN. This is not a complete light reader,
// a general retail parser, or a live light setter. Duplicate used labels and
// out-of-world zones are native admission failures, not original assertions.
//
// Source defaults/branch ordering are exact. Nonzero direction normalization
// uses ordinary native binary32 arithmetic, like the existing TA native policy;
// it does not claim EE/VU RSQRT rounding or exceptional-mode parity.
// Transactional output. Does not mutate authoring or another section.
inline bool readFreshPose(const ParameterBlock48F& lightBlock,
                          std::uint32_t zoneCount, FreshPose& output,
                          Diagnostic& d) noexcept {
    d = {};
    FreshPose next{};
    const ParameterField48F* f = nullptr;

    //2774C8 default1 ->158B30 ->2774FC subtract1. Preserve one-based native form.
    if (!detail::find(lightBlock, "initialzone", f, d, 0x2774ec)) return false;
    if (f) {
        next.zoneAuthored = true;
        // Actual GA/TA InitialZone spelling is a positive decimal integer.
        // Avoid accepting a fractional value merely because float32 rounds it.
        std::uint32_t value = 0;
        if (f->value.empty())
            return detail::fail(d, Status::number, 0x2774ec, f->value_offset);
        for (std::size_t i = 0; i < f->value.size(); ++i) {
            const char c = f->value[i];
            if (c < '0' || c > '9' ||
                value > (std::numeric_limits<std::uint32_t>::max() -
                         static_cast<unsigned>(c - '0')) / 10u)
                return detail::fail(d, Status::number, 0x2774ec,
                                    f->value_offset + i);
            value = value * 10u + static_cast<unsigned>(c - '0');
        }
        next.initialZone = value;
    }
    if (next.initialZone == 0 || next.initialZone > zoneCount)
        return detail::fail(d, Status::initialZone, 0x277500,
                            f ? f->value_offset : lightBlock.offset);

    //277564..27757C /158CC8..158CE0: absent Position copies zero XYZ from442F80.
    //27752C initializes the destination vector's W to1 before that lookup.
    if (!detail::find(lightBlock, "position", f, d, 0x277578)) return false;
    if (f) {
        next.positionAuthored = true;
        if (!detail::triple(*f, next.position14, d, 0x277578)) return false;
    }

    //277588..2775B4: authored LookVector wins even if its XYZ is exactly zero.
    //2775B8..277624: only if absent, read TargetPosition and subtract Position.
    //277628..277634: if both absent, copy full442F80=(0,0,0,1).
    if (!detail::find(lightBlock, "lookvector", f, d, 0x277598)) return false;
    if (f) {
        next.lookSource = LookSource::lookVector;
        if (!detail::triple(*f, next.look18, d, 0x277598)) return false;
    } else {
        if (!detail::find(lightBlock, "targetposition", f, d, 0x2775cc)) return false;
        if (f) {
            next.lookSource = LookSource::targetPosition;
            Vec4 target{};
            if (!detail::triple(*f, target, d, 0x2775cc)) return false;
            for (unsigned c = 0; c < 3; ++c)
                next.look18[c] = detail::sub(target[c], next.position14[c]);
        }
    }

    //146924 initial A0 equals426F10=(0,0,0,1). Init copies nonzero positions;
    //the equal-zero path leaves the same value. No inferred/neighbor position.
    if (detail::nonzero(next.position14)) next.positionA0 = next.position14;

    //1467C0 initializes B0=(0,0,0,1);146888/146890/14689C leave78=(1,0,0).
    //147194..1471F8 compares input XYZ to B0. Only an unequal input reaches
    //147200 normalization and14720C/147218/147220 cached78 writes.
    if (detail::nonzero(next.look18)) {
        const float square = detail::add(
            detail::add(detail::mul(next.look18[0], next.look18[0]),
                        detail::mul(next.look18[1], next.look18[1])),
            detail::mul(next.look18[2], next.look18[2]));
        if (!std::isfinite(square) || square < std::numeric_limits<float>::min())
            return detail::fail(d, Status::nativeArithmetic, 0x147200,
                                lightBlock.offset);
        volatile float inverse = 1.f / std::sqrt(square);
        for (unsigned c = 0; c < 3; ++c) {
            const float value = detail::mul(next.look18[c], inverse);
            if (!std::isfinite(value))
                return detail::fail(d, Status::nativeArithmetic, 0x147200,
                                    lightBlock.offset);
            next.lookB0[c] = value;
            next.effectiveLook78[c] = value;
        }
        next.lookWasNormalized = true;
    }
    //149F68/149F6C/149F70 consume78/80/7C for spotlight cone attenuation.
    output = next;
    return true;
}
}
