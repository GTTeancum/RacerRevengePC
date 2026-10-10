#pragma once
#include "tv_camera_selection49e.h"

namespace rr::game {
// Exact scene reads used by2D38C0. Supply the complete manager order, including
// inactive entries: original source index is the zero-based span index. The
// cached XYZ is28B180's local node+F0 value, not28B1A0's resolved world position.
struct TVCameraSceneSample49F {
    std::int32_t inactive40=0;
    assets::TVCameraVector49E localPosition;
    assets::TVCameraVector49E velocity;
};
// Extra numeric fields of the original body-attached TV update. The shared49E
// selector retains positionA0, shakeC0/C4/C8/CC/D0 and filter current00.
struct TVCameraUpdateState49F {
    float rawFov04=60;
    assets::TVCameraVector49E cameraPosition10;
    assets::TVCameraVector49E playerPosition60;
    assets::TVCameraVector49E playerVelocity70;
    assets::TVCameraVector49E shakeVectorB0;
    float filterTau04=0.4f;
    float filterBlend08=1;
};
struct TVCameraUpdateInput49F {
    // Only XYZ is written by original28B1A0. W is deliberately ignored;
    // playerPosition60.w retains the initialized/state value, normally1.
    assets::TVCameraVector49E resolvedPlayerPosition;
    assets::TVCameraVector49E bodyVelocity;
    float targetYOffset=0;
    float elapsed=0;
    assets::TVCameraSpan49E<const TVCameraSceneSample49F> sceneSamples;
};
struct TVCameraUpdateOutput49F {
    std::int32_t mode=1;
    assets::TVCameraVector49E position;
    assets::TVCameraVector49E target;
    assets::TVCameraVector49E look;
    assets::TVCameraVector49E up;
    float fov=60;
};
// Source2D3400 with body-attached base2D2170, exact target/position/FOV virtual
// callees, scene shake2D38C0 and the closed49E RNG. This does not select a camera
// or choose the game's TV/follow mode. Supply already-selected49E state and the
// shared RNG AFTER all preceding game-mode random draws. Current must be valid.
// The game selector uses28B180 local player XYZ; this view uses28B1A0 resolved
// world XYZ. Those inputs are not interchangeable under parent transforms.
//
// Update order: base pose/rawFOV/mode; first eligible scene shake trigger;
// alternating shake transform/expiry; FOV filter. Output position is shaken and
// output FOV filtered; state cameraPosition10/rawFov04 remain the base values.
// Mode1 writes target and preserves look/up; mode0 writes look/up and preserves
// target. In/out output therefore must be a live initialized object.
//
// Body/scene resolving, world queries, debug presentation and game scheduling
// are explicit external boundaries. Every supplied scene sample represents the
// exact original manager entry; no omitted potentially eligible entry or sort.
// The zero-allocation loop is bounded by caller span size <=INT32_MAX; this is
// native admission, not an original engine capacity. Typed live disjoint data,
// state, random, inputs, outputs and immutable loaded backing are preconditions.
//
// Requires strict nearest-even binary32, no fast math/contraction; other host
// rounding modes are rejected as native admission, not an original branch. Only reached
// arithmetic inputs/intermediates must be finite; copied W/unconsumed vectors
// are bit-preserved. Reached divide-by-zero, normalization degeneracy or other
// nonfinite result rejects atomically, preserving state/output/RNG. Negative dt
// and reversed FOV ranges are not silently changed. Host instruction-step
// arithmetic is not a claim of universal PS2 VU Q/MADD silicon equivalence.
// No non-body previous-position/sentinel branch is supplied by this adapter.
bool updateTVCamera49F(const assets::TVCameraView49E&,
    TVCameraSelectorState49E&,TVCameraUpdateState49F&,
    const TVCameraUpdateInput49F&,TVCameraRandom49E&,
    TVCameraUpdateOutput49F&) noexcept;
}
