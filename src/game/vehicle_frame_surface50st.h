#pragma once
// PC ENHANCEMENT, NOT ORIGINAL BEHAVIOUR. Declaration only; the rules, their
// switches and their state live in native_surface_traction50st.h and
// native_zero_gravity50zg.h. Kept separate so those headers stay free of the
// frame types and this one adds no includers to vehicle_frame49w.h.
#include "vehicle_frame49w.h"

namespace rr::game {
// Per-step inputs of the N64 surface / zero-g force mapping. The defaults are
// the original (callers take produceVehicleForces49W when isOriginal()).
struct SurfaceForceInputs50ST {
    float lateralScale=1.f;  // (0,1]: sideslip side-force coefficients x this
    float speedFactor=1.f;   // (0,1]: engine speed endpoints x this
    float speedBonus=0.f;    // [0,1000] ft/s: engine speed window shifted up
    float minimumDrive=0.f;  // 0 or 1: engine drive floor (N64 zero-throttle target)
    bool untiltedAero=false; // zero-g flight: no world-up tilt branch in 16CD90
    bool isOriginal()const noexcept {
        return lateralScale==1.f&&speedFactor==1.f&&speedBonus==0.f&&minimumDrive==0.f&&!untiltedAero;}
};
// Same as produceVehicleForces49W with the inputs above applied to LOCAL
// copies of the racer's aero / engine parameters and engine input (the
// parameter owner is never written). Refuses (VehicleFrameError49W::Input)
// any value outside its domain or non-finite.
bool produceVehicleForcesSurface50ST(VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const VehicleFrameParameters49W&,const PostWorld49W&,VehicleFrameWorkspace49W&,
    VehicleFrameDiagnostic49W&,const SurfaceForceInputs50ST&) noexcept;
}
