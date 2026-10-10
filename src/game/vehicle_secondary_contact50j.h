#pragma once
#include "vehicle_static_query50h.h"
#include "vehicle_static_response50g.h"
namespace rr::game {
enum class SecondaryQuerySceneStatus50J {Ready,InvalidInput,UnsupportedScope,GraphFailure};
// First-substep, active secondary detector getter. Executes the original lazy
// parentless scene refresh BEFORE secondary query/forces and projects the same
// cache into its canonical body view. No positions/history are reset. This is
// a completed getter stage, not a promise that subsequent query will succeed.
SecondaryQuerySceneStatus50J prepareSecondaryQueryScene50J(VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const MainStaticDetectorAdmission50H&,
    VehicleFrameWorkspace49W&) noexcept;
// Same supplied static-query contract as50H, using already-prepared physical19C.
Type2StaticStatus49X querySecondaryStaticVehicle50J(const VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const MainStaticDetectorAdmission50H&,
    const Type2StaticQuery49X&,const Type2StaticCandidate49X*,std::size_t,
    const Type2StaticInitial49X&,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,Type2StaticResult49X&) noexcept;
// Secondary368 response followed by its actual body integration. Main body,
// frame, spring and records retain the same transaction guarantees as50G.
VehicleStaticResponseStatus50G respondSecondaryStaticVehicle50J(
    VehicleFrameState49W&,const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    SpringSceneId48N,std::uint32_t primaryIdentity,StaticSpringState50G&,
    SpringRecord48N*,std::size_t,const VehicleStaticResponseWorkspace50G&,
    VehicleFrameDiagnostic49W&) noexcept;
}
