#pragma once
#include <cstdint>
namespace rr::game {
// Synchronous scalar observation only. No source pointers, resources, readiness,
// or permission to retain this frame across world ticks/session teardown.
struct OriginalPositionStatsNumbers50LA {
    std::uint64_t sessionGeneration=0,cameraPublication=0;
    std::uint32_t viewIndex=0,cameraManagerIndex=0,managerCount30=0;
    std::int32_t placeCCC=0;
    std::uint32_t activeCount50=0,viewState40=0,viewCountCD0=0;
    std::uint32_t preference10F9C=0,subtype11258=0;
    std::int32_t raceMode8C=0;
    std::uint8_t raceFlags1B0=0,raceEntityFlags29=0,uiFlags1E4=0;
};
}
