#pragma once
#include "vehicle_static_response50g.h"
namespace rr::game {
// Step R4: main354 2964C0 when generated records name ANOTHER body (other44).
// Same projection as respondStaticGenerated50E, except that a record with
// other44 keeps that identity and the coordinator receives the primary plus
// every distinct other body it names (original2969AC/296A60/297190/297390/
// 297610 two-body branches inside updateSpringContacts48L). With no other44
// record and no other body it is the 50E projection exactly.
// bodies[0] is the primary; bodies[1..bodyCount) are other bodies, identities
// nonzero/unique; every record other44 must name one of them.
inline constexpr std::size_t podResponseBodyCapacity50PC=8;
struct PodResponseWorkspace50PC {
    SpringGeneratedRecord48K* projected=nullptr;
    std::size_t projectedCapacity=0;
    SpringGeneratedBuffers48K scratch{},prepared{};
};
struct PodResponseResult50PC {
    SpringRecordState48N records;
    SpringModelBody48K bodies[podResponseBodyCapacity50PC];
    std::size_t bodyCount;
    Vector3_48C worldCenter110,contactVelocity120;
};
StaticResponseStatus50E respondGeneratedWithPods50PC(
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    const SpringRecordState48N&,const SpringModelBody48K* bodies,std::size_t bodyCount,
    const StaticResponseScene50E&,const SpringRecord48N*,std::size_t recordCount,
    const PodResponseWorkspace50PC&,SpringRecord48N* destination,
    std::size_t destinationCapacity,PodResponseResult50PC&) noexcept;
struct VehiclePodResponseWorkspace50PC {
    PodResponseWorkspace50PC response;
    SpringRecord48N* preparedRecords=nullptr;
    std::size_t preparedRecordCapacity=0;
    VehicleFrameWorkspace49W* frame=nullptr;
};
// Counterpart of respondMainStaticVehicle50G for records with other44: same
// Post354 frame commit for the primary; the other bodies' prepared states are
// returned in othersOut[0..otherCount) for the caller's bounds-checked commit
// into those racers (roster_pod_contact50PC::SlotAccess). Nothing is written to
// any other racer here. Errors preserve frame/spring/records/othersOut.
VehicleStaticResponseStatus50G respondMainVehicleWithPods50PC(
    VehicleFrameState49W&,const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    const SecondaryInputs49M&,SpringSceneId48N,std::uint32_t primaryIdentity,
    const SpringModelBody48K* others,std::size_t otherCount,SpringModelBody48K* othersOut,
    StaticSpringState50G&,SpringRecord48N*,std::size_t recordCount,
    const VehiclePodResponseWorkspace50PC&,VehicleFrameDiagnostic49W&) noexcept;
}
