#pragma once
#include "vehicle_frame49w.h"
#include "static_contact_response50e.h"
namespace rr::game {
struct StaticSpringState50G {
    SpringRecordState48N records;
    Vector3_48C worldCenter110,contactVelocity120;
};
struct VehicleStaticResponseWorkspace50G {
    StaticResponseWorkspace50E response;
    SpringRecord48N* preparedRecords=nullptr;
    std::size_t preparedRecordCapacity=0;
    VehicleFrameWorkspace49W* frame=nullptr;
};
enum class VehicleStaticResponseStatus50G {
    Complete,WrongPhase,InvalidInput,UnsupportedScope,InsufficientCapacity,FrameRejected
};
// Main354 only: current canonical frame body -> actual static generated records
// -> original2964C0 response -> retained main354/probe/secondary-force stage.
// Requires Post354, an explicitly bound query-scene identity and prepared
// records. No query generation, scene refresh, registration, candidate discovery
// or point/provider callback is inferred. Runtime settings are supplied; cached
// center/velocity fields in parameters are superseded by the sole spring owner.
// Spring/record destinations commit only after the atomic frame call succeeds.
// All caller-owned mutable spans (full capacities) must be mutually disjoint
// and disjoint from immutable descriptors. Errors preserve frame/spring/records;
// workspace can change. Diagnostic belongs to a reached frame call only.
VehicleStaticResponseStatus50G respondMainStaticVehicle50G(
    VehicleFrameState49W&,const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    const SecondaryInputs49M&,SpringSceneId48N,std::uint32_t primaryIdentity,
    StaticSpringState50G&,SpringRecord48N*,std::size_t recordCount,
    const VehicleStaticResponseWorkspace50G&,VehicleFrameDiagnostic49W&) noexcept;
}
