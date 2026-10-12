#pragma once
#include "camera_view49f.h"
#include <cstdint>

namespace rr::game {
//114AE0 numeric owner, separate from the spatial34 record and49F scene pose.
//One persistent record per actual scene. It is updated BOTH by the earlier
//race-child scene call and by115DB0's zero-delta call after the camera pose.
struct OriginalCameraSceneTick {
    std::uint64_t sessionGeneration=0,sceneKey=0;
    bool bound=false;
    assets::TVCameraVector49E previousPosition1B0{0,50,35,1};
    assets::TVCameraVector49E previousLook1C0{0,0,1,1};
    float distance240=0,angle244=0;
    std::uint32_t countdown20C=0,countdown210=0;
};
//1143F0 writes all represented numeric fields. It does not copy the later
//175224 racer position into previous1B0: that source binding changes180 only.
bool bindFreshOriginalCameraSceneTick(OriginalCameraSceneTick&,
    std::uint64_t sessionGeneration,std::uint64_t sceneKey) noexcept;
enum class OriginalCameraSceneTickStatus : std::uint8_t {
    needsMatrixVisibilityAndChildren,skippedBySceneFlag,owner,arithmetic
};
//114AE0..114C08, before virtual+78 matrix build and114C44 registration.
//The scene29 bit2 return still updates both histories/distance/angle but does
//not decrement counters. The actual input LOOK is scene190, already updated
//by114F90/1150C0 when reached through115CB0. No normalization is inserted.
OriginalCameraSceneTickStatus advanceOriginalCameraSceneTick114AE0(
    OriginalCameraSceneTick&,std::uint64_t sessionGeneration,std::uint64_t sceneKey,
    const assets::TVCameraVector49E& actualPosition180,
    const assets::TVCameraVector49E& actualLook190,std::uint8_t sceneFlags29) noexcept;
//Strict finite normal/zero separately rounded binary32 under FE_TONEAREST,
//matching the existing native instruction-step model. This is not universal
//EE scalar accumulator/exception equivalence. Native rejection is atomic.
//Stable live disjoint typed state/input is required. Source-child scheduling,
//matrix build, actual visibility, and141ED0 child calls remain subsequent work.
}
