#pragma once
#include "original_ui_garage_palette_packet50kg.h"

namespace rr::game::garage_palette50KG {
// The two halves of projectTriangle(), split at its existing per-vertex loop.
// Each vertex's prepared operand and guard bits are a pure function of the
// retained Preamble and that vertex's own source words, so a caller may reuse
// a previously prepared identical vertex of the same Preamble. projectTriangle
// itself is exactly checkTriangle + three prepareVertex calls + projectPrepared.
Status checkTriangle(const Preamble&,const std::array<RawVertex,3>&) noexcept;
Status prepareVertex(const Preamble&,const RawVertex&,
    garage_prelit50KG::PreparedVertex&,std::uint8_t& guard) noexcept;
Status projectPrepared(const Preamble&,const std::array<garage_prelit50KG::PreparedVertex,3>&,
    std::uint8_t guardAny,garage_prelit50KG::Polygon&) noexcept;
}
