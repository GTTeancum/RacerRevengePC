#pragma once
#include "original_ui_garage_prelit50kg.h"
#include "original_ui_lit_geometry50km.h"

namespace rr::game::garage_diffuse50KG {
using Status=garage_prelit50KG::Status;
// Original337690, row41.w==0 only. The caller must supply the actual model
// packet's one matrix. This is not a palette-node adapter or a frame owner.
struct Context {
    garage_prelit50KG::Context geometry;
    OriginalUiLitContext50KM lighting;
    float normalScale1E4=0;
};
struct Preamble {
    garage_prelit50KG::Preamble geometry;
    OriginalUiLitPreamble50KM lighting;
};
struct RawVertex {
    std::array<std::int16_t,3> position{};
    std::array<std::int8_t,3> normal{};
    std::array<std::int16_t,2> uv{};
};
Status prepare(const Context&,Preamble&) noexcept;
// Chronological oldest/middle/current strip records, not parity-normalized
// imported triangle indices. Generates actual light RGBA before366430, retains
// both separately-rounded signed UV lanes and actual radial-fog byte. Output
// is the source packet representation, including raw STQ and clipped fan order.
Status projectTriangle(const Preamble&,const std::array<RawVertex,3>&,
    garage_prelit50KG::Polygon&) noexcept;
}
