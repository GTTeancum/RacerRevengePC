#pragma once
#include <array>
#include <cstdint>

namespace rr::game::type2_axis49x {
using Vector = std::array<float,4>;
struct Input {
    std::array<float,3> center{};
    std::array<float,3> half{};
    std::array<std::array<float,3>,3> vertex{};
    std::array<std::int16_t,3> biasedFaceNormal{};
    // Four columns, target into query coordinates. This helper does not invert.
    std::array<float,16> targetToQuery{};
};
struct Trace {
    // Native logical trace: untested slots have cold constructor values, not
    // the original process's unobservable stale scratch from earlier calls.
    std::array<Vector,15> axes{};
    std::array<Vector,3> transformedVertex{};
    Vector transformedNormal{}, firstRelativeToCenter{};
    Vector firstMinusSecond{}, firstMinusThird{}, normalizedThirdMinusFirst{};
    std::array<float,13> radius{};
    std::array<std::array<float,3>,13> projection{};
    std::array<std::array<float,2>,13> minMax{};
    std::uint32_t tested = 0;
    std::int32_t rejectedAxis = -1;
};
enum class Outcome : std::uint8_t { Separated, Overlap, InvalidNumeric };
// Original107940..1082A4, static float-vertex path. Individually rounded
// binary32, FE_TONEAREST, no fast-math/FMA. Finite intermediates required;
// positive subnormal VU squared norms are outside the retained reference model.
// No allocation, guest addresses, instruction interpreter, or mutable globals.
// Source range, cold initialization and ordering proof are in findings.md.
Outcome overlap(const Input& input, Trace* trace = nullptr) noexcept;
}
