#pragma once
#include "vehicle_frame49w.h"
#include "static_type2_producer49x.h"
namespace rr::game {
struct MainStaticDetectorAdmission50H {
    bool springPresent,queryPresent,queryEnabled,pairsPresent;
    bool providerPresent,pointContactsEnabled,resetSpring,explicitCandidates;
    std::uint32_t queryType,registrationE0;
    SceneNodeIndex49P queryScene;
};
// Enabled main690, null-pair295630 route at PostWorld, with current source
// admission supplied by the caller. Supports ready canonical main190 and an
// actual tree-backed type2 resource. Non-null registration, active providers,
// inactive/other routes and dirty scenes are explicit boundaries, never misses.
// Derives11F680 matrix/sphere from CURRENT canonical scene and runtime root;
// then runs the source-verified ordered49X query/cumulative static callbacks.
// template.worldMatrix/sphere/resetRecordsBefore are superseded by authoritative
// frame/admission values. Remaining filter/binding/opaque inputs stay explicit.
// No enumeration,219420 handler, secondary query, or live registration inferred.
// Frame and descriptors are read-only; normal49X transaction/buffer rules apply.
Type2StaticStatus49X queryMainStaticVehicle50H(const VehicleFrameState49W&,
    const VehicleFrameBindings49W&,const MainStaticDetectorAdmission50H&,
    const Type2StaticQuery49X& queryTemplate,const Type2StaticCandidate49X*,std::size_t,
    const Type2StaticInitial49X&,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,Type2StaticResult49X&) noexcept;
}
