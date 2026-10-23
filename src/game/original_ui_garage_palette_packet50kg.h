#pragma once
#include "original_ui_garage_prelit50kg.h"
#include "original_ui_lit_geometry50km.h"
#include "model_palette49s.h"

namespace rr::game {
struct OriginalRacerModelPacket50LA;
namespace garage_palette50KG {
using Status=garage_prelit50KG::Status;
enum class Family:std::uint8_t {diffuseCompressed,specularMapCompressed};
struct Context {
    // Only camera fields are borrowed here. Actual light rows are read from
    // the retained packet's two real extra matrices, never these light fields.
    OriginalUiLitContext50KM camera;
    float guardX334=0,guardY338=0;
    std::uint8_t modelFlags1B2=0;
};
struct NodePreamble {
    garage_prelit50KG::Preamble geometry;
    OriginalUiLitPreamble50KM lighting;
};
struct Preamble {
    std::array<NodePreamble,maxModelPaletteEntries49S> nodes{};
    OriginalUiSpecularNormalAxes50KM specularNormalUV{};
    std::uint32_t entryCount=0;
    float guardX334=0,guardY338=0;
    Family family=Family::diffuseCompressed;
};
struct RawVertex {
    std::array<std::int16_t,3> position{};
    std::array<std::int8_t,3> normal{};
    std::array<std::int16_t,2> uv{};
    // Actual signed fourth XYZ lane. abs(word)/16-1 selects the REAL packet
    // palette slot. The sign is source ADC, not another transform or bone ID.
    std::int16_t paletteAdcWord=0;
};
// Real241488..241598 matrix payload only. No synthetic identity palette and no
// model-root multiplication appended to a node that already includes parents.
// This numeric leaf is not a phase/capability; its caller must own the genuine
// current packet preparation, visibility and immutable source geometry borrow.
Status prepare(const OriginalRacerModelPacket50LA&,const Context&,Family,Preamble&) noexcept;
// Actual chronological strip records. The current record must have positive
// ADC: startup/restart records do not describe a completed source triangle.
// Retains guard rejection separately from ordinary366430 clipping coordinates.
Status projectTriangle(const Preamble&,const std::array<RawVertex,3>&,
    garage_prelit50KG::Polygon&) noexcept;
}
}
