#pragma once
#include "ai_driving50iv.h"
#include "ai_state_prefix50iv.h"
#include "ai_combat_distance50lh.h"

namespace rr::game {
//1706C0..170B40. Actual AI-track gates, actual scene190 local positions and
//actual manager CCC are required. There is no nearest-racer or HOST progress
//substitute. A successful native call may return either original bool value;
//failure names a source boundary and never invents an original return value.
struct AiCombatCandidateInput50LH {
    const TrackNavigation50EU* trackFF4=nullptr;
    const AiTrackMetadata50IU* metadata=nullptr;
    const AiObservedRacer50IV* target=nullptr;
    const float* selfPosition190=nullptr;
    std::uint32_t selfNodeC80=0;
    std::int32_t selfLapCC8=0,selfPlaceCCC=0;
    float combatZoneLength107C=0.f;
    ai_combat_distance50LH::DiagnosticSink diagnostic=nullptr;
    void* diagnosticContext=nullptr;
};
struct AiCombatCandidateReceipt50LH {
    bool accepted=false,clearRouteChoices=false;
    std::uint32_t mode1050=0;
    float selfGateDistance=0.f,targetGateDistance=0.f,signedDistance=0.f;
    ai_combat_distance50LH::Receipt distance{};
};
namespace ai_combat_candidate_detail50LH {
inline bool gateDistance(const TrackNavigation50EU& track,std::uint32_t node,
    const float* position,float& output,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    using ai_driving_detail50IV::fail;
    if(!position||!track.gatesPrepared||!track.track.count||
       track.track.count>maxUndertrackNodes50ET||node>=track.track.count)
        return fail(diagnostic,AiDrivingError50IV::Owner,0x2a2490u,node);
    ai_prefix_detail50IV::Math f;
    const auto& gate=track.nodes[node];
    float segment[3],offset[3];
    for(unsigned axis=0;axis<3;++axis){
        segment[axis]=f.sub(gate.gate20[axis],gate.gate10[axis]);
        offset[axis]=f.sub(position[axis],gate.gate10[axis]);
    }
    //106E70 uses separate XYZ products, (X+Y)+(1*Z). Both dot calls precede
    //the divide, MIN.S then MAX.S. Keep scalar signed-zero ordering.
    const float numerator=f.dot(segment,offset);
    const float denominator=f.dot(segment,segment);
    const float ratio=f.div(numerator,denominator);
    const float factor=detail::scalarMax48O(detail::scalarMin48O(ratio,1.f),0.f);
    float residual[3];
    for(unsigned axis=0;axis<3;++axis){
        const float projection=f.add(f.mul(segment[axis],factor),gate.gate10[axis]);
        residual[axis]=f.sub(position[axis],projection);
    }
    const float x=f.mul(residual[0],residual[0]);
    const float y=f.mul(residual[1],residual[1]);
    const float xy=f.add(x,y);
    const float norm=f.add(xy,f.mul(residual[2],residual[2]));
    if(!f.valid||norm<0.f)
        return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x1706c0u,node);
    //109EF0 is scalar SQRT.S plus a binary32 store/load, not RSQRT.
    volatile float distance=std::sqrt(norm);
    f.read(distance);
    if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x109ef0u,node);
    output=distance;return true;
}
}
inline bool evaluateAiCombatCandidate50LH(const AiCombatCandidateInput50LH& input,
    AiCombatCandidateReceipt50LH& receipt,AiDrivingDiagnostic50IV& diagnostic) noexcept {
    using ai_driving_detail50IV::fail;diagnostic={};receipt={};
    if(std::fegetround()!=FE_TONEAREST)
        return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x1706c0u);
    if(!input.trackFF4||!input.metadata||!input.metadata->bound||
       input.metadata->count!=input.trackFF4->track.count||
       !input.target||!input.target->bound||!input.selfPosition190)
        return fail(diagnostic,AiDrivingError50IV::Owner,0x1706c0u);
    const auto& target=*input.target;
    if(!ai_combat_candidate_detail50LH::gateDistance(*input.trackFF4,
        input.selfNodeC80,input.selfPosition190,receipt.selfGateDistance,diagnostic)||
       !ai_combat_candidate_detail50LH::gateDistance(*input.trackFF4,
        target.vehicleC80,target.position190,receipt.targetGateDistance,diagnostic))return false;
    ai_prefix_detail50IV::Math f;
    receipt.signedDistance=f.sub(receipt.selfGateDistance,receipt.targetGateDistance); //170A38 delay
    if(input.selfNodeC80!=target.vehicleC80){
        float difference=0.f;
        if(!ai_combat_distance50LH::distanceBetweenNodes2A2DC0(*input.metadata,
            target.vehicleC80,target.lapCC8,input.selfNodeC80,input.selfLapCC8,
            difference,receipt.distance,input.diagnostic,input.diagnosticContext))
            return fail(diagnostic,receipt.distance.error==ai_combat_distance50LH::Error::Owner?
                AiDrivingError50IV::Owner:AiDrivingError50IV::Arithmetic,receipt.distance.sourcePC);
        receipt.signedDistance=f.add(receipt.signedDistance,difference);
    }
    const float zone=f.read(input.combatZoneLength107C);
    if(!f.valid)return fail(diagnostic,AiDrivingError50IV::Arithmetic,0x170a6cu);
    if(!(std::fabs(receipt.signedDistance)<=zone))return true;
    // AD(D)IU's low32 comparisons are retained without signed C++ overflow.
    const auto selfPlace=static_cast<std::uint32_t>(input.selfPlaceCCC);
    const auto targetPlace=static_cast<std::uint32_t>(target.placeCCC);
    if(targetPlace!=selfPlace+1u&&targetPlace!=selfPlace-1u)return true;
    if(receipt.signedDistance< -200.f)receipt.mode1050=4;
    else if(receipt.signedDistance<10.f){
        if(target.flagsB9C&2u)return true;
        receipt.mode1050=3;
    }else{receipt.mode1050=1;receipt.clearRouteChoices=true;}
    receipt.accepted=true;return true;
}
} //namespace rr::game
