#pragma once
#include "original_shared_rng50iu.h"
#include "shared_random_owner50id.h"
#include "camera_view49f.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Source-only native candidate for PodRaceStartCamera2D4B90..2D7D94.
// No guest pointers, instruction dispatch, hardware, renderer or racer writes.
// Snapshot order is the actual manager44 order. Source28B180 copies nodeF0
// LOCAL position;28A530 copies nodeE0 forward and nodeD0 up. Never replace
// these with resolved world poses or UI/grid-preview positions.
using RaceStartVector = assets::TVCameraVector49E;
struct RaceStartRacerBody {
    std::uint64_t racerKey=0, bodyNodeKey=0;
    RaceStartVector localPositionF0{}, forwardE0{}, upD0{};
};
struct RaceStartRacerBank {
    std::uint64_t sessionGeneration=0;
    std::array<RaceStartRacerBody,8> racers{};
};
struct RaceStartGridNode {
    RaceStartVector position{};
    // Original node40/44/48/4C: left, right, previous row, next row.
    std::array<std::int32_t,4> neighbor{{-1,-1,-1,-1}};
};
struct RaceStartFilter {
    float value=0, tau=0, blend=1;
};
struct OriginalRaceStartCamera {
    std::uint64_t sessionGeneration=0;
    std::array<std::uint64_t,8> racerKeys{}, bodyNodeKeys{};
    bool prepared=false, active=false;
    std::array<RaceStartGridNode,12> grid{};
    std::array<std::int32_t,12> route{{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}};
    std::int32_t routeCount1FC=0, nextRoute1F8=0;
    std::int32_t target1F0=-1, alternateTarget1F4=-1;
    RaceStartVector initialTarget1B0{}, unfilteredPosition1C0{}, look1D0{}, step1E0{};
    float maxFov04=75, initialFov08=35, reachRadius0C=8;
    float speed10=0, routeFov14=0, currentFov1C=0, bestDot20=0;
    bool flag230=false, zoomIn234=false, zoomOut238=false, handoff23C=false;
    bool complete240=false;
    std::array<RaceStartFilter,3> directionFilter{}, positionFilter{};
    RaceStartFilter fovFilter{};
};
// SAME application owner used by lap/Dax/contact range draws. This alias does
// not create an intro-private cache. Pass the existing shared owner, outside
// racer snapshots; source80A8/80AC guards are initialized only when reached.
using RaceStartRandomGlobals = SharedRandomOwner50ID;
struct RaceStartSeedSamples {
    // Actual source-native101A50 result for each REACHED reseed. This function
    // does not manufacture clock values, access a guest timer, or use a fixed
    // seed. If a clock result is0, the subsequent bounded-integer draw reseeds
    // again exactly as the source. Ordinary probability draws do not reseed.
    const std::uint32_t* values=nullptr;
    std::size_t size=0;
};
struct RaceStartRandomOperation {
    std::uint32_t sourceCallsite=0;
    std::uint32_t word=0; // reseed value or masked308030 return
    bool reseed=false;
};
struct RaceStartPreparationReceipt {
    std::array<RaceStartRandomOperation,16> randomOperations{};
    std::size_t randomOperationCount=0, seedSamplesConsumed=0;
};
enum class RaceStartStatus : std::uint8_t {
    ok, invalidOwner, invalidSourceState, missingClockSample,
    unsupportedNumericDomain, requiresControllerHandoff
};
//2D4DC0(f12=35,f13=75,f14=8)->2D52E0->2D5890. Fixed eight-racer native
// admission matches current actual ALL8, preserving class/input separation.
// The source grid accepts >=8; this candidate deliberately admits exactly8.
// Shared RNG is the SAME application CoupledNoiseContext used by AI/effects.
// Initialization only prepares and registers the child; activation is separate.
RaceStartStatus prepareOriginalRaceStartCamera(OriginalRaceStartCamera&,
    const RaceStartRacerBank&, RaceStartRandomGlobals&, CoupledNoiseContext49M&,
    RaceStartSeedSamples, RaceStartPreparationReceipt&) noexcept;
//2D6DD0. Invoked by controller1762D8 after source selects the registered child.
RaceStartStatus activateOriginalRaceStartCamera(OriginalRaceStartCamera&,
    const RaceStartRacerBank&) noexcept;
// Complete2D5090 pose/motion/completion body:2D6FF0,2D7280,2D7660,2D6F50.
// The source caller checks complete240 BEFORE the next pose update. Therefore
// an update setting completion must still publish its final pose this frame;
// call the controller handoff next frame instead of updating this child again.
// Output mode0 writes position/look/up/FOV and preserves the input target.
RaceStartStatus advanceOriginalRaceStartCamera(OriginalRaceStartCamera&,
    const RaceStartRacerBank&,float elapsed,TVCameraUpdateOutput49F&) noexcept;
// The existing recovered115CB0 scene/view consumer, composed atomically with
// the procedural child. Caller must establish the ordinary scene flags itself.
RaceStartStatus advanceOriginalRaceStartCameraView(OriginalRaceStartCamera&,
    const RaceStartRacerBank&,float elapsed,CameraSceneState49F&,
    TVCameraUpdateOutput49F&,CameraViewOutput49F&,
    std::uint8_t rootFlags228,std::uint8_t sceneFlags29) noexcept;

// Requires FE_TONEAREST and strict no-contraction/no-fast-math binary32. Every
// reached scalar input/intermediate must be normal or zero. Vector copies keep
// unused W lanes as specified; projection matrix behavior belongs to49F.
// Host instruction-step model, NOT universal PS2 Q/MADD/denormal behavior.
// Native rejection preserves all referenced state/output/RNG/receipt. Original
// code does not roll back failures. All records must be live, disjoint, serial;
// actual bank provenance/lifetimes and external race effects are caller-owned.
}
