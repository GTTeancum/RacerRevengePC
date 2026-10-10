#pragma once
#include "../assets/model_geometry49o.h"
#include "../assets/model_bindings49o.h"
#include <cstdint>

namespace rr::game {
struct ModelLocalMatrix49O { float m[16]; };
enum class ModelMotionError49O : std::uint32_t {
    none, rounding_mode, input, topology, binding, sample_time, arithmetic
};
struct ModelMotionDiagnostic49O { ModelMotionError49O error; std::uint32_t track; };
// Bounded matrix projection of155E70's non-mirrored route. The original call
// 213830 passes Forward,Back,frameFraction=.5,blendWeight=0. Time is FIRST
// motion duration*frameFraction for BOTH tracks; blend0 is the second endpoint,
// still evaluated through original interpolation and quaternion arithmetic.
// Source hierarchy binding rows select caller-owned PSG local matrices. All
// rotation tracks run in order before all positions; unaffected lanes survive.
// VUT+38/+3C flags are deliberately not consulted:155E70 doesn't read them.
//
// Requires successfully admitted, alive, unchanged MotionView49O objects and
// source-derived bindings. Further admission:1..64 local matrices, finite
// affine lanes; same paired channel counts and row sequence;>=2 keys/track;
// sample in each track's inclusive first/last-time interval; finite fractions
// and blends in[0,1]; finite consumed values/intermediates; FE_TONEAREST.
// Tables are source-software-trig-produced under the retained nearest/even
// scalar model. Acos slot2048 is source cold-start zero BSS padding; no host
// trigonometry fallback or unconditional console FCR/hardware claim.
// No general animation service, mirror path, scene graph dirty-flag execution,
// IK, scale restoration, allocation, I/O, or time advancement is represented.
// False leaves all local matrices untouched; diagnostic is always set.
// Numeric single1559F0 projection for the optional fresh Mid sample213854.
// Uses the same source rotation/position sampler without an extra blend.
bool sampleModelMotion49O(const assets::MotionView49O& motion,
    const assets::MotionNodeBindings49O& bindings,float frameFraction,
    ModelLocalMatrix49O* locals,std::uint32_t nodeCount,
    ModelMotionDiagnostic49O& diagnostic) noexcept;
bool sampleModelMotionBlend49O(const assets::MotionView49O& first,
    const assets::MotionView49O& second,
    const assets::MotionNodeBindings49O& bindings,
    float frameFraction, float blendWeight, ModelLocalMatrix49O* locals,
    std::uint32_t nodeCount, ModelMotionDiagnostic49O& diagnostic) noexcept;
}
