#pragma once
#include "original_ui_garage_prelit50kg.h"
#include "original_ui_lit_geometry50km.h"

namespace rr::game::garage_specular50KG {
using Status=garage_prelit50KG::Status;
struct Context {
    garage_prelit50KG::Context geometry{};
    OriginalUiLitContext50KM lighting{};
    float normalScale1E4=0;
};
struct Preamble {
    garage_prelit50KG::Preamble geometry{};
    OriginalUiLitPreamble50KM lighting{};
    OriginalUiSpecularNormalAxes50KM normalUV{};
};
struct RawVertex {
    std::array<std::int16_t,3> position{};
    std::array<std::int8_t,3> normal{};
};
// Actual row41.w==0 branch33B8E8..33B928. This is NOT a one-entry identity
// palette. modelWorld100 is the actual model root; generated UV uses its saved
// modelView and qNormal. Generated RGBA uses actual global1499A0 rows, not the
// stored PreLit colors. F is255 regardless of model1B2 bit02.
Status prepare(const Context&,Preamble&) noexcept;
// Chronological original strip vertices and genuine source366430 clipping.
// No native GL clip substitution and no default light/color/fog values.
Status projectTriangle(const Preamble&,const std::array<RawVertex,3>&,
    garage_prelit50KG::Polygon&) noexcept;
}
