#pragma once
#include "ai_driving50iv.h"
#include "ai_state_prefix50iv.h"
#include "lap_timing50id.h"

namespace rr::game {
// Per-racer supplemental fields, registered with the actual serial arena.
// Bodies, controls, thermal state, navigation and lap clocks remain canonical
// external owners. No duplicated pose, speed or input histories live here.
struct AiRuntimeState50IV {
    AiProfile50IU profile{};
    AiStatePrefixState50IV state{};
    AiPackingState50IV packing{1.f,0.f,0};
    AiRepairState50IV repair{};
    AiNavigationState50IV navigation{};
    AiBoostDecisionState50IT boost{};
    AiRacerLists50IV lists{};
    std::int32_t referenceFF8=-1;
    std::uint32_t managerIndexFE0=0,racer60=0,fieldFE4=0;
    float engineDifference5C=0.f;
    bool initialized=false;
    std::uint64_t dispatchedFrames=0,brakingFrames=0,startupFrames=0,ordinaryFrames=0,boundBrakeSteerFrames=0;
};
// This owner is populated by native1B0400 over the actual manager field. CCC
// and CC4 must never be copied from HOST standings/centerline progress.
struct AiRaceFields50IV {float remainingCC4=0.f;std::int32_t placeCCC=0;bool bound=false;};
// Process-lifetime original scalar owners. Source startup clears433CB0;
//16FFE0 lazily clears/increments the 64-bit navigation call counter433CC0.
struct AiApplicationGlobals50IV {
    std::uint32_t paused433CB0=0;
    std::uint64_t navigationCalls433CC0=0;
    bool navigationCounterInitialized=false;
};
struct AiRuntimeInput50IV {
    const TrackNavigation50EU* mapFF0=nullptr;
    const TrackNavigation50EU* aiFF4=nullptr;
    const TrackNavigation50EU* orientationTrackBC=nullptr;
    const AiTrackMetadata50IU* aiMetadata=nullptr;
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
};
struct AiRuntimeReceipt50IV {
    bool dispatched=false,attemptedBoost=false,boostHandled=false,braking=false;
    std::uint32_t sourceBoundary=0,stateFE8=0;
    float desiredSpeed=0.f;
    AiDrivingDiagnostic50IV driving{};
    AiPrefixDiagnostic50IV prefix{};
    AiBoostDecisionDiagnostic50IT boost{};
    AiSpeedDiagnostic50IR speed{};
};
inline void initializeAiRuntime50IV(std::uint32_t managerIndex,std::uint32_t racer60,
    std::uint32_t actualCourseRecordPod60,const AiProfile50IU&profile,
    float engineDifference,AiRuntimeState50IV&out) noexcept {
    AiRuntimeState50IV n{};initializeAiStatePrefix50IV(n.state);initializeAiBoostDecision50IT(n.boost);
    n.managerIndexFE0=managerIndex;n.racer60=racer60;n.fieldFE4=racer60==actualCourseRecordPod60?1u:0u;
    n.engineDifference5C=engineDifference;n.profile=profile;n.initialized=true;out=n;
}
//16E8C0's native controller span, called after readAiVehicleFrame50IV and
//before228724 capturesCA0/bindAiVehicleFrame50IV. Prefix failures stop the live
//source boundary; no caller may replay the same frame as if it never started.
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50IV&in,AiRuntimeReceipt50IV&receipt) noexcept {
    using namespace ai_driving_detail50IV;receipt={};
    auto stop=[&](std::uint32_t pc) noexcept {receipt.sourceBoundary=pc;receipt.stateFE8=ai.state.aiStateFE8;return false;};
    if(!ai.initialized||frame.fields.vehicle58!=1u||!frame.fields.nativeAiOwnersBound50IV||
       frame.phase!=VehicleFramePhase49W::PostPlayer)return stop(0x16e8c0);
    parameters.rates.x=4.f; //16E8D4 owns live B94 even when dispatch is skipped.
    if(!in.applicationGlobals)return stop(0x16e950);
    if(in.applicationGlobals->paused433CB0||frame.fields.vehicle3C||!(frame.fields.applicationByte58&2u))return true;
    if(!in.mapFF0||!in.aiFF4||!in.orientationTrackBC||!in.aiMetadata||!in.tuning||
       !in.tuning->bound||!in.racers||!in.racerCount||ai.managerIndexFE0>=in.racerCount||
       !in.raceOwnersBound||!in.raceFields||!in.raceFields->bound||!in.timing)return stop(0x16e9c0);
    auto&m=frame.bodies.main;const auto&scene=frame.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
    const auto&bodyScene=frame.nodes[static_cast<unsigned>(bindings.animation.bodyScene288)];
    const float*position=scene.localC0.v+12;const float*forward=scene.localC0.v+8;
    if(!in.applicationGlobals->navigationCounterInitialized){
        in.applicationGlobals->navigationCalls433CC0=0;
        in.applicationGlobals->navigationCounterInitialized=true;
    }
    ++in.applicationGlobals->navigationCalls433CC0;
    if(!advanceAiNavigation50IV(*in.mapFF0,*in.aiFF4,*in.aiMetadata,previousCA0,position,
        in.actualZoneOrdinal48,progress,lapCC8,frame.fields.impactFlagsCD8,ai.navigation,ai.boost,receipt.driving))return stop(receipt.driving.sourcePC);
    const auto current=progress.reset.vehicleC80,previous=progress.vehicleC84;
    if(current>=in.orientationTrackBC->track.count||previous>=in.orientationTrackBC->track.count)return stop(0x173af0);
    AiOrientationInput50IV orientation{};
    orientation.applicationFlags58=frame.fields.applicationByte58;orientation.vehicleFlagsCD8=frame.fields.impactFlagsCD8;
    orientation.vehicleState40=m.vehicleState40;orientation.preparedFactor=m.frameFactor;
    for(unsigned axis=0;axis<3;++axis){orientation.localForward340_288[axis]=bodyScene.localC0.v[8+axis];
        orientation.currentC80Position[axis]=in.orientationTrackBC->track.nodes[current].position[axis];
        orientation.previousC84Position[axis]=in.orientationTrackBC->track.nodes[previous].position[axis];}
    //3C9410..3C946C initializes433C70 to(0,0,0,1); XYZ fallback is zero.
    AiOrientationReceipt50IV oriented{};
    if(!updateAiOrientation50IV(orientation,ai.state,oriented,receipt.prefix))return stop(receipt.prefix.sourcePC);
    AiNoCombatInput50IV stateInput{};
    stateInput.timerCB8=in.timing->timeCB8;stateInput.combat1054=ai.profile.combativeness1054;
    stateInput.combatWait106C=ai.profile.startRaceNoCombatTime106C;stateInput.remainingCC4=in.raceFields->remainingCC4;
    stateInput.lapCC8=static_cast<std::int32_t>(lapCC8);stateInput.placeCCC=in.raceFields->placeCCC;
    stateInput.fieldFE4=ai.fieldFE4;stateInput.managerField34=in.managerHumanCount34;
    stateInput.applicationFlags58=frame.fields.applicationByte58;stateInput.settings1125C=in.settings1125C;
    stateInput.currentNodeFlags79=in.aiMetadata->nodes[current].flags79;
    stateInput.managerRacer0Present=in.racerCount>0&&in.racers[0].bound;
    stateInput.managerRacer1Present=in.racerCount>1&&in.racers[1].bound;
    if(stateInput.managerRacer0Present)stateInput.managerRacer0State40=in.racers[0].vehicle40;
    if(stateInput.managerRacer1Present)stateInput.managerRacer1State40=in.racers[1].vehicle40;
    for(unsigned axis=0;axis<3;++axis){stateInput.turnSide.currentNodePosition[axis]=in.aiFF4->track.nodes[current].position[axis];
        stateInput.turnSide.scene190Position[axis]=position[axis];stateInput.turnSide.scene190LocalForward[axis]=forward[axis];}
    AiStateDispatchReceipt50IV dispatch{};
    if(!advanceAiStateNoCombat50IV(stateInput,ai.state,dispatch,receipt.prefix))return stop(receipt.prefix.sourcePC);
    if(!dispatch.complete)return stop(dispatch.boundaryPC);
    AiPackingInput50IV packing{};packing.referenceFF8=ai.referenceFF8;
    if(ai.referenceFF8>=0){const auto index=static_cast<std::size_t>(ai.referenceFF8);
        if(index>=in.racerCount)return stop(0x1af330);const auto&r=in.racers[index];
        packing.referencePresent=r.bound;packing.referenceState40=r.vehicle40;
        packing.referenceCachedSpeed34=r.cachedSpeed34;packing.referenceRemainingCC4=r.progressCC4;
        // Observation of the actual per-racer lap word is carried with the roster.
        packing.referenceLapCC8=r.lapCC8;
    }
    packing.remainingCC4=in.raceFields->remainingCC4;packing.trackLength3C=in.aiTrackLength3C;
    packing.lapCC8=static_cast<std::int32_t>(lapCC8);packing.placeCCC=in.raceFields->placeCCC;
    packing.nodeC80=current;packing.aiStateFE8=ai.state.aiStateFE8;packing.settings1125C=in.settings1125C;
    packing.timerCBC=in.timing->valueCBC;packing.track90=in.tuning->farFrontDist90;
    packing.track94=in.tuning->closeFrontDist94;packing.track98=in.tuning->farBehindDist98;
    packing.track9C=in.tuning->closeBehindDist9C;packing.trackA0=in.tuning->slowestSpeedPctA0;packing.trackA4=in.tuning->fastestSpeedPctA4;
    AiPackingReceipt50IV packed{};
    if(!updateAiPacking50IV(packing,ai.packing,thermal.heat7C,packed,receipt.prefix))return stop(receipt.prefix.sourcePC);
    AiRepairInput50IV repair{};repair.preparedFactor=m.frameFactor;repair.damage7D0=frame.fields.damage7D0;
    repair.damage7D4=frame.fields.damage7D4;repair.damage7DC=frame.fields.damage7DC;
    repair.environmentD38=frame.impactSupplement.environmentalD38;repair.environmentD3C=frame.impactSupplement.environmentalD3C;
    repair.impactBA0=frame.fields.impactBA0.v[0];repair.vehicleCFC=frame.fields.vehicleCFC;
    repair.cooldown1060=ai.profile.waitToRepairTime1060;repair.duration1064=ai.profile.repairTime1064;
    repair.trackCooldown6C=in.tuning->waitToRepairEnvTime6C;repair.trackDuration70=in.tuning->repairEnvUntil70;
    repair.trackThreshold74=in.tuning->repairEnvThreshold74;
    AiRepairReceipt50IV repaired{};
    if(!updateAiRepair50IV(repair,ai.repair,m.controls.player.flagsB9C,repaired,receipt.prefix))return stop(receipt.prefix.sourcePC);
    PlayerReadResult48X actions=m.controls.player;Math f;V target{{0,0,0,1}},direction{{0,0,0,1}};
    bool attemptBoost=true;float boostFactor=m.frameFactor;
    if(ai.state.aiStateFE8==4u){
        if(!buildAiRacerLists50IV(*in.aiFF4,progress,ai.profile,in.racers,in.racerCount,ai.managerIndexFE0,
            position,forward,ai.lists,receipt.driving)||
           !aiDesiredPosition50IV(*in.aiFF4,progress,ai.navigation,position,200.f,target,receipt.driving))return stop(receipt.driving.sourcePC);
        direction=normalize(f,sub(f,target,point(position)));
        if(!steerAi50IV(direction.v,forward,scene.localC0.v[5],m.cachedSpeed34,ai.engineDifference5C,
            m.components.root.worldAngularVelocityA0.v[1],ai.racer60,actions,receipt.driving)||
           !aiStartupSpeed50IV(in.aiMetadata->nodes[current],ai.profile,ai.lists,in.racers,in.racerCount,
            position,m.components.root.body.velocity.v,actions,receipt.desiredSpeed,attemptBoost,receipt.driving))return stop(receipt.driving.sourcePC);
        boostFactor=1.f;++ai.startupFrames;
    }else if(ai.state.aiStateFE8==0u||ai.state.aiStateFE8==2u||ai.state.aiStateFE8==3u){
        //172380 early start-line return preserves B60 and1090.
        if(!lapCC8&&current==1u)actions.raw.values[0]=0.f;
        else{
            if(!(frame.fields.applicationByte58&1u)&&m.cachedSpeed34<10.f&&!m.vehicleState40)
                ai.state.stallTimer1090=f.sub(ai.state.stallTimer1090,m.frameFactor);
            else if(!m.components.probe348.field104&&m.response354_3C&&std::fabs(scene.localC0.v[5])<ai_speed_detail50IR::word(0x3e99999au))
                ai.state.stallTimer1090=f.sub(ai.state.stallTimer1090,m.frameFactor);
            else ai.state.stallTimer1090=1.5f;
            V nearTarget{{0,0,0,1}};
            if(!aiDesiredPosition50IV(*in.aiFF4,progress,ai.navigation,position,f.mul(m.cachedSpeed34,in.tuning->lookahead78),target,receipt.driving)||
               !aiDesiredPosition50IV(*in.aiFF4,progress,ai.navigation,position,f.mul(m.cachedSpeed34,ai_speed_detail50IR::word(0x3e4ccccdu)),nearTarget,receipt.driving))return stop(receipt.driving.sourcePC);
            const V velocity=normalize(f,point(m.components.root.body.velocity.v));
            const V distant=normalize(f,sub(f,target,point(position))),nearby=normalize(f,sub(f,nearTarget,point(position)));
            direction=normalize(f,dot(f,distant,velocity)<dot(f,nearby,velocity)?distant:nearby);
            if(!steerAi50IV(direction.v,forward,scene.localC0.v[5],m.cachedSpeed34,ai.engineDifference5C,
                m.components.root.worldAngularVelocityA0.v[1],ai.racer60,actions,receipt.driving))return stop(receipt.driving.sourcePC);
        }
        AiSpeedTargetInput50IR speed{};speed.currentNodeSpeedLimit5C=in.aiMetadata->nodes[current].speedLimit5C;
        speed.currentNodeFlags79=in.aiMetadata->nodes[current].flags79;speed.vehicleSpeedScaleFFC=ai.packing.scaleFFC;
        speed.vehicleMinimumSpeed108C=ai.profile.myMinSpeed108C;speed.trackControlFlags58=frame.fields.applicationByte58;
        speed.vehicleFlagsCD8=frame.fields.impactFlagsCD8;speed.vehicleAiStateFE8=ai.state.aiStateFE8;
        speed.trackRacePace7C=in.tuning->racePace7C;speed.settings188F60Nonzero=in.settings188F60Nonzero;
        speed.vehicleCCC=in.raceFields->placeCCC;speed.vehicleCB0=in.timing->timeCB0;
        speed.vehicleCC4=in.raceFields->remainingCC4;speed.aiTrackLength3C=in.aiTrackLength3C;
        AiSpeedTargetReceipt50IR selected{};if(!selectAiTargetSpeed50IR(speed,selected,receipt.speed))return stop(receipt.speed.sourcePC);
        receipt.desiredSpeed=selected.desiredSpeed;++ai.ordinaryFrames;
    }else return stop(0x16e9fc);
    if(!f.valid)return stop(0x172380);
    if(attemptBoost){AiBoostDecisionInput50IT boost{boostFactor,receipt.desiredSpeed,thermal.heat7C,m.cachedSpeed34,
        ai.state.aiStateFE8,ai.lists.counts[0],ai.lists.counts[1],m.components.probe348.field10C,false,0};
        AiBoostDecisionReceipt50IT boosted{};
        if(!decideAiBoost50IT(boost,actions,actions,ai.boost,frame.noise.sharedRandomState,boosted,receipt.boost))return stop(receipt.boost.sourcePC);
        receipt.attemptedBoost=true;receipt.boostHandled=boosted.handled;
    }
    if(!receipt.boostHandled){AiNonBoostSpeedReceipt50IR braking{};
        if(!applyAiNonBoostSpeed50IR(receipt.desiredSpeed,m.cachedSpeed34,actions,actions,braking,receipt.speed))return stop(receipt.speed.sourcePC);
        receipt.braking=braking.braking;if(braking.braking)++ai.brakingFrames;
    }
    m.controls.player=actions;++ai.dispatchedFrames;receipt.dispatched=true;receipt.stateFE8=ai.state.aiStateFE8;return true;
}
} //namespace rr::game
