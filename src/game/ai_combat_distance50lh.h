#pragma once
// Authenticated retail2A2DC0. Strict native binary32 owner projection.
#include "ai_cold_owners50iu.h"
#include "navigation_consumer_bridge50mb.h"
#include "ai_speed_braking50ir.h"
#include <cstring>

namespace rr::game::ai_combat_distance50LH {
inline constexpr const char* synchronizationDiagnostic =
    "BUG ALERT:  TunnelTrack::GetDistanceBetweenNodesFeet.  TargetNodeIndex and LapNumber are probably out of synch.\n";
//1336C0's returned integer is unused. A supplied sink must perform the native
//diagnostic output exactly once; it must not mutate supplied gameplay owners.
using DiagnosticSink = void (*)(const char*,void*) noexcept;
enum class Error {None,Owner,Rounding,Arithmetic,DiagnosticBoundary};
struct Receipt {
    Error error=Error::None;
    std::uint32_t sourcePC=0;
    bool targetLapAdjusted=false,selfLapAdjusted=false,diagnosticEmitted=false;
    std::int32_t effectiveTargetLap=0,effectiveSelfLap=0;
};
inline std::int32_t incrementWord(std::int32_t value) noexcept {
    // ADDIU keeps low32; CVT.S.W consumes signed low32. Avoid signed C++ UB.
    std::uint32_t bits=0;std::memcpy(&bits,&value,4);++bits;
    std::memcpy(&value,&bits,4);return value;
}
// Arguments map exactly to 2A2DC0: FF4 metadata, a1 target node, a2 target lap,
// a3 self node, t0 self lap. Both node+60 distances belong to the SAME FF4.
// Do not replace this with CC4 subtraction or call it when caller170A34 skips
// equal nodes. Output preserved on failure. Receipt describes reached effects.
template<class Metadata>
inline bool distanceBetweenNodes2A2DC0Shared50MB(const Metadata& source_metadata,
    std::uint32_t targetNode,std::int32_t targetLap,
    std::uint32_t selfNode,std::int32_t selfLap,float& output,Receipt& receipt,
    DiagnosticSink diagnostic,void* diagnosticContext) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::metadataReady(source_metadata)) {receipt={};receipt.error=Error::Owner;receipt.sourcePC=0x2a2de0u;return false;}

    const auto& metadata=navigation_consumer_detail50MB::read(source_metadata);
    receipt={};
    auto fail=[&](Error error,std::uint32_t pc) noexcept {
        receipt.error=error;receipt.sourcePC=pc;return false;
    };
    if(!metadata.bound||!metadata.count||metadata.count>navigation_consumer_detail50MB::capacity(metadata)||
       targetNode>=metadata.count||selfNode>=metadata.count)
        return fail(Error::Owner,0x2a2de0u);
    if(std::fegetround()!=FE_TONEAREST)return fail(Error::Rounding,0x2a2dc0u);
    const float selfDistance=metadata.nodes[selfNode].distanceToFinish60;
    const float targetDistance=metadata.nodes[targetNode].distanceToFinish60;
    using namespace ai_speed_detail50IR;
    if(!admitted(selfDistance)||!admitted(targetDistance))return fail(Error::Arithmetic,0x2a2e04u);
    // Do not merge these branches: target correction jumps straight to2A2E78.
    if(targetNode==1u&&targetDistance<1000.f){
        if(!diagnostic)return fail(Error::DiagnosticBoundary,0x1336c0u);
        diagnostic(synchronizationDiagnostic,diagnosticContext);receipt.diagnosticEmitted=true;
        targetLap=incrementWord(targetLap);receipt.targetLapAdjusted=true;
    }else if(selfNode==1u&&selfDistance<1000.f){
        if(!diagnostic)return fail(Error::DiagnosticBoundary,0x1336c0u);
        diagnostic(synchronizationDiagnostic,diagnosticContext);receipt.diagnosticEmitted=true;
        selfLap=incrementWord(selfLap);receipt.selfLapAdjusted=true;
    }
    receipt.effectiveTargetLap=targetLap;receipt.effectiveSelfLap=selfLap;
    Math math;
    const float targetLapFloat=math.integer(targetLap); //2A2E84
    const float length=math.keep(metadata.trackLength3C); //2A2E80 load
    const float targetLapDistance=math.mul(targetLapFloat,length); //2A2E98
    const float targetRemainder=math.sub(length,targetDistance); //2A2E9C
    const float selfLapFloat=math.integer(selfLap); //2A2EA0
    const float targetTotal=math.add(targetLapDistance,targetRemainder); //2A2EA8
    const float selfLapDistance=math.mul(selfLapFloat,length); //2A2EAC
    const float selfRemainder=math.sub(length,selfDistance); //2A2EB0
    const float selfTotal=math.add(selfLapDistance,selfRemainder); //2A2EB4
    const float result=math.sub(targetTotal,selfTotal); //2A2EBC
    if(!math.valid)return fail(Error::Arithmetic,0x2a2ebcu);
    output=result;return true;
}
inline bool distanceBetweenNodes2A2DC0(const AiTrackMetadata50IU& metadata,
    std::uint32_t targetNode,std::int32_t targetLap,
    std::uint32_t selfNode,std::int32_t selfLap,float& output,Receipt& receipt,
    DiagnosticSink diagnostic=nullptr,void* diagnosticContext=nullptr) noexcept {
    return distanceBetweenNodes2A2DC0Shared50MB(metadata,targetNode,targetLap,selfNode,selfLap,output,receipt,diagnostic,diagnosticContext);
}
inline bool distanceBetweenNodes2A2DC0(const AiTrackMetadataView50IU& metadata,
    std::uint32_t targetNode,std::int32_t targetLap,
    std::uint32_t selfNode,std::int32_t selfLap,float& output,Receipt& receipt,
    DiagnosticSink diagnostic=nullptr,void* diagnosticContext=nullptr) noexcept {
    if(!navigation_consumer_detail50MB::metadataReady(metadata)) {receipt={};receipt.error=Error::Owner;receipt.sourcePC=0x2a2de0u;return false;}
    return distanceBetweenNodes2A2DC0Shared50MB(metadata,targetNode,targetLap,selfNode,selfLap,output,receipt,diagnostic,diagnosticContext);
}

} //namespace rr::game::ai_combat_distance50LH
