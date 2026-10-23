#pragma once
#include "../../../src/game/original_ui_main_particle_geometry50kg.h"
#include <vector>

namespace rr::platform {
//Numeric result only. The Draw/FIFO retains all publication, material, image
//and submission authority. A clipped row remains present and carries its real
//group/run transition, even when it has no native geometry.
struct OriginalUiGarageParticlePacketGeometry50KG {
    struct Row {
        game::OriginalUiMainParticleQuad50KG projected;
        std::array<std::array<float,3>,4> sourceSTQ{};
        std::array<float,4> sourceW{{1,1,1,1}};
        std::uint16_t groupOrdinal=0;
        bool firstInGroup=false,firstInRun=false;
        std::int32_t signedRunLength=0;
    };
    std::vector<Row> rows;
    std::uint32_t visibleQuads=0;
    std::uint64_t publication=0,revision=0;
};
}
