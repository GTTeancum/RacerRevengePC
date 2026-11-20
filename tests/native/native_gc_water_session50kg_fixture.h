#pragma once
#include "native_gc_water_racer50kg_fixture.h"
#include "../../src/game/original_gc_pod_sparks50kg.h"
#include <cstdint>

namespace native_gc_water_session50KG {
using namespace rr::game;
// Application-lifetime particle/water globals shared by every race's racers
// (one per process, owned by the session; host memory, never game memory).
// The water runtime's own BSS (443BB0) is owned by the common 05.10 step
// (common_track_media50cr.cpp); the GC-only session owner that held the GC
// water, GC fog emitters and the six GC generator fires is retired into the
// common track path (PLAN B-C16: one water owner when the SCB names water,
// one particle path).
struct Application final {
    // Source3C9FDC writes434630=(0,0,0,1), once per application.
    float initialStep434630[4]{0,0,0,1};
    std::uint32_t directFireSerial433970=0;
    OriginalGcPodSparksGlobals50KG sparksGlobals;
};
} // namespace native_gc_water_session50KG
