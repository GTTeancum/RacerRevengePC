#pragma once
#include "camera_view49f.h"

namespace rr::game {
struct CameraViewContext49F {
    // Original2363B0 bit2 enables an extra input-adjustment path;114AE0 scene
    // bit2 suppresses matrix building. Only both-bit-clear context is recovered.
    std::uint8_t rootFlags228=0,sceneFlags29=0;
};
// Complete numeric body-attached TV view boundary: original TV update followed
// by its real scene/view/projection consumer. Camera selection and the game's
// TV/follow choice happen before this call; their additional RNG consumption
// is not skipped or synthesized here. Supply already-selected49E state, the
// shared random state at TV-update entry, resolved player pose, complete ordered
// local scene samples, and original scene projection parameters.
//
// Like controller115CB0/115060, carries prior scene Look/Up into the live output
// before updating. Other mode-unused lanes retain the caller's initialized
// values. The result contains actual original core view/projection/combined
// matrices, before platform viewport/screen adjustments and rendering.
//
// All state is typed/caller-owned, serial and allocation-free. Unsupported
// consumed arithmetic/input domain leaves EVERY referenced output/state intact;
// this staged failure policy is native admission, not original partial-error
// behavior. Enclosing objects must be live, initialized and mutually disjoint.
// Context bit2 in either flag rejects before updating. Other flag bits are not
// reinterpreted. Default context is the explicit ordinary/build-enabled scope,
// not a recovered global initializer. Original scene history/diagnostic counters
// and rendering side effects remain outside this numeric view output.
// No world queries, ownership handling, hardware, renderer or scheduler callbacks.
bool updateTVCameraView49F(const assets::TVCameraView49E& configuration,
    TVCameraSelectorState49E& selected,TVCameraUpdateState49F& updateState,
    const TVCameraUpdateInput49F& input,TVCameraRandom49E& random,
    CameraSceneState49F& scene,TVCameraUpdateOutput49F& liveOutput,
    CameraViewOutput49F& viewOutput,CameraViewContext49F context={}) noexcept;
}
