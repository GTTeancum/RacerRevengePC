#pragma once
#include "tv_camera_update49f.h"
#include <array>

namespace rr::game {
// Numeric scene fields consumed by114F90/1150C0/115350. Apart from aspect1F8,
// defaults match1143F0. Aspect1F8 must be supplied from the original viewport
// contract: (viewportH/viewportW)/(scene1DC*(fullH/fullW));1 is a native default.
struct CameraSceneState49F {
    assets::TVCameraVector49E look{0,0,1,1};
    assets::TVCameraVector49E up{0,1,0,1};
    float fovDegrees=77;
    float aspect1F8=1;
    float nearPlane=1;
    float farPlane=100000;
    float rollRadians=0;
};
struct CameraViewOutput49F {
    CameraSceneState49F sceneAfter;
    assets::TVCameraVector49E position;
    // Flat original 16-word storage, row-vector multiplication convention.
    std::array<float,16> view{},projection{},combined{};
    bool targetAccepted=false;
    bool fovWritten=false;
};
// Numeric controller115CB0 -> scene114F90/1150C0 -> matrix115350 closure.
// Its ordinary composed game-frame path requires global[382B80]+228 bit2 clear
// (2363B0 then bypasses input-driven pose overrides) and scene+29 bit2 clear
// (114AE0 then permits virtual+78 to build). Game236DD0 calls115350 directly.
// Debug/pose-override and matrix-build-skipped paths are outside this leaf API;
// a composed-frame caller must guard their raw flags, not silently treat those
// paths as a refreshed ordinary view. Mode zero consumes authored
// Look/Up; every other mode consumes target. Mode nonzero first normalizes
// prior Up, matching115060's copied scene fields before the camera update.
// FOV zero retains prior FOV. Exact target==position preserves prior Look and
// the already normalized prior Up. No invented fallback or epsilon.
//
// View14DC80, projection14DB90, tan13BCA0, sincos13BFE0 and roll14E070 are
// source-recovered numeric leaves. Core matrices precede PS2 viewport/screen
// adjustment; these rendering operations and game scheduling remain external.
// Strict nearest-even binary32, no contraction/fast math. Only reached inputs
// and arithmetic results must be finite. Normalized W executes original W+0
// (so -0 becomes +0); only copied/unconsumed W payloads remain bitwise.
// Normalization uses original finite max reciprocal for zero squared norm.
// This host instruction-step model is
// not universal PS2 Q/MADD/denormal/exception behavior. Unsupported arithmetic
// rejects atomically, leaving output untouched. No heap allocation, selectors,
// world/scene queries or RNG. Live typed inputs/output are required.
bool composeCameraView49F(const CameraSceneState49F& prior,
    const TVCameraUpdateOutput49F& supplied,CameraViewOutput49F& output) noexcept;

// Original 280720 -> 14DAF0: zone viewport projection combined with the SAME
// current camera's view+C0. Bounds are minX,maxX,minY,maxY from zone4C..58.
// This off-axis matrix has different depth terms from camera14DB90; it must
// not replace the camera's core projection. No admission/dispatch is implied.
// Native finite domain requires ordered nonzero bounds, positive aspect/near,
// far>near and 0<FOV<180. A rejected numeric input is not a culled zone.
// Reuses strict binary32 scalar math and the existing native matrix product.
// Output changes only on success; output must be disjoint from the inputs.
struct PortalZoneViewOutput49F {
    std::array<float,16> projection{},matrix60{};
};
bool composePortalZoneView280720(const CameraViewOutput49F& currentCamera,
    const std::array<float,4>& bounds,PortalZoneViewOutput49F& output) noexcept;
}
