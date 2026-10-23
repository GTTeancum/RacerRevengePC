#pragma once
#include "../../../src/assets/native_asset48b.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
namespace rr::platform {
class OriginalUiGarageDrawContribution50KG;
// Owned CPU output of exactly one REAL queued model record. This is not a
// material binding, a FIFO completion receipt or a complete SourceFrame.
// The Draw's real FIFO continues to hold all source model/table pins while
// this value is composed, bound and submitted. Only that Draw may populate it.
struct OriginalUiGarageOwnedModelGeometry50KG final {
private:
    friend class OriginalUiGarageDrawContribution50KG;
    std::vector<assets::Vertex48B> vertices;
    std::vector<std::array<float,3>> stq;
    std::vector<float> rasterW,fog;
    std::size_t recordOrdinal=0,lod=0,surface=0,packetIndex=0;
    const void* recordIdentity=nullptr;
    std::uint64_t publication=0,matrixRevision=0;
    std::uint32_t epoch=0,materialSlot=0;
    std::int32_t originRegistration=-1,finalRegistration=-1;
    assets::OriginalUiGarageProgram50KG program=assets::OriginalUiGarageProgram50KG::prelitCompressed;
    // Genuine LOCAL range in these owned vectors, not a frame offset or an
    // invented whole-LOD extent. The composer must account it once against
    // the same unchanged whole-frame bound before assigning its final range.
    std::uint32_t firstQuad=0,quadCount=0,sourceTriangleCount=0,clippedTriangleCount=0;
};
}
