#pragma once
#include "ai_driving50iv.h"
#include "native_zero_gravity_step50zg.h"
#include "ai_combat_candidate50lh.h"
#include "ai_combat_state50lh.h"
#include "ai_attack_drive50lh.h"
#include "ai_attack_wide50lh.h"
#include "ai_finish_controls50kg.h"
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
    // Canonical supplemental1008: native stable manager identity, not a copied
    // racer object or guest pointer. Constructor/reset null is represented by
    // absence; a completed scan retains its last actual non-self candidate.
    bool target1008Present=false;
    std::uint32_t target1008ManagerIndex=0;
    // Canonical source scalar owners, constructor16E2B8..16E2C0/16E2F8/
    //16E348. Recovered human combat consumes these same owners;16E830 owns
    // these writes.1050 is an integer mode (171254), not a float timer.
    float timer100C=0.f,timer1010=0.f,timer1014=0.f,field104C=-1.f;
    std::uint32_t mode1050=0;
    bool initialized=false;
    // Native continuation marker, not an original vehicle field. Once the
    // real16E830 has run, FE8 is0 even though2255B0 is still pending. Retain
    // this marker so another call cannot mistake that partial reset for an
    // ordinary state0 frame. Only a complete physical-reset join can clear it.
    bool physicalReset2255B0Pending=false;
    std::uint64_t scalarResets16E830=0;
    std::uint64_t combatScans=0,combatAiRejections=0,attackFrames=0;
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
    AiAttackDriveGlobals50LH attackDrive{};
    AiAttackWideGlobals16EDD0 attackWide{};
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
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// Strong immutable MB views retain their authentic storage and final world.
struct AiRuntimeInput50MB {
    TrackNavigationView50EU mapFF0{};
    TrackNavigationView50EU aiFF4{};
    TrackNavigationView50EU orientationTrackBC{};
    AiTrackMetadataView50IU aiMetadata{};
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// TC-CONSUMER-BEGIN 0
struct AiRuntimeInput50TC {
    TrackNavigationView50TC mapFF0{};
    TrackNavigationView50TC aiFF4{};
    TrackNavigationView50TC orientationTrackBC{};
    AiTrackMetadataView50TC aiMetadata{};
    // Borrow the actual selected aggregate world; final views retain its lifetime.
    const LiveZoneWorld50EN* actualWorld=nullptr;
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
struct AiRuntimeInput50TB {
    TrackNavigationView50TB mapFF0{};
    TrackNavigationView50TB aiFF4{};
    TrackNavigationView50TB orientationTrackBC{};
    AiTrackMetadataView50TB aiMetadata{};
    // Borrow the actual selected aggregate world; final views retain its lifetime.
    const LiveZoneWorld50EN* actualWorld=nullptr;
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// TB-CONSUMER-END 0

// GB-CONSUMER-BEGIN 0
struct AiRuntimeInput50GB {
    TrackNavigationView50GB mapFF0{};
    TrackNavigationView50GB aiFF4{};
    TrackNavigationView50GB orientationTrackBC{};
    AiTrackMetadataView50GB aiMetadata{};
    // Borrow the actual selected aggregate world; final views retain its lifetime.
    const LiveZoneWorld50EN* actualWorld=nullptr;
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// GB-CONSUMER-END 0

// MC-CONSUMER-BEGIN 0
struct AiRuntimeInput50MC {
    TrackNavigationView50MC mapFF0{};
    TrackNavigationView50MC aiFF4{};
    TrackNavigationView50MC orientationTrackBC{};
    AiTrackMetadataView50MC aiMetadata{};
    // Borrow the actual selected aggregate world; final views retain its lifetime.
    const LiveZoneWorld50EN* actualWorld=nullptr;
    const AiTrackTuning50IU* tuning=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
    const AiRaceFields50IV* raceFields=nullptr;
    const LapTimingFields50ID* timing=nullptr;
    //Borrow the actual race94 owner. Consumed only by genuine state6 ending;
    //manager30, racerCB8 and HOST race seconds are not substitutes.
    const float* raceCountdown94=nullptr;
    AiApplicationGlobals50IV* applicationGlobals=nullptr;
    std::uint32_t actualZoneOrdinal48=0,managerHumanCount34=0;
    std::uint8_t settings1125C=0;
    float aiTrackLength3C=0.f;
    bool raceOwnersBound=false,settings188F60Nonzero=false;
    ai_combat_distance50LH::DiagnosticSink combatDiagnostic=nullptr;
    void* combatDiagnosticContext=nullptr;
    BodyContactIdentity48R (*resolveBody340)(const void*,std::uint32_t) noexcept=nullptr;
    const void* body340Context=nullptr;
};
// MC-CONSUMER-END 0

namespace navigation_consumer_detail50MB {
template<class Input> struct RuntimeTypes;
template<> struct RuntimeTypes<AiRuntimeInput50IV> {
    using RouteOwners=AiRouteChoiceOwners50IY;
    using CandidateInput=AiCombatCandidateInput50LH;
    using AttackInput=AiAttackSpeedInput50LH;
};
template<> struct RuntimeTypes<AiRuntimeInput50MB> {
    using RouteOwners=AiRouteChoiceOwners50MB;
    using CandidateInput=AiCombatCandidateInput50MB;
    using AttackInput=AiAttackSpeedInput50MB;
};
// TC-CONSUMER-BEGIN 1
template<> struct RuntimeTypes<AiRuntimeInput50TC> {
    using RouteOwners=AiRouteChoiceOwners50TC;
    using CandidateInput=AiCombatCandidateInput50TC;
    using AttackInput=AiAttackSpeedInput50TC;
};
// TC-CONSUMER-END 1

// TB-CONSUMER-BEGIN 1
template<> struct RuntimeTypes<AiRuntimeInput50TB> {
    using RouteOwners=AiRouteChoiceOwners50TB;
    using CandidateInput=AiCombatCandidateInput50TB;
    using AttackInput=AiAttackSpeedInput50TB;
};
// TB-CONSUMER-END 1

// GB-CONSUMER-BEGIN 1
template<> struct RuntimeTypes<AiRuntimeInput50GB> {
    using RouteOwners=AiRouteChoiceOwners50GB;
    using CandidateInput=AiCombatCandidateInput50GB;
    using AttackInput=AiAttackSpeedInput50GB;
};
// GB-CONSUMER-END 1

// MC-CONSUMER-BEGIN 1
template<> struct RuntimeTypes<AiRuntimeInput50MC> {
    using RouteOwners=AiRouteChoiceOwners50MC;
    using CandidateInput=AiCombatCandidateInput50MC;
    using AttackInput=AiAttackSpeedInput50MC;
};
// MC-CONSUMER-END 1

}
struct AiRuntimeReceipt50IV {
    bool dispatched=false,attemptedBoost=false,boostHandled=false,braking=false;
    bool scalarResetCompleted=false,physicalResetPending=false;
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
// Complete source16E830..16E8B0 on the SAME initialized runtime and B9C owner.
// This is narrower than construction. It preserves1004, all five racer lists,
// profile/identity/track/progress/timing/thermal/body/scene owners, input axes,
// B7C/B80, every other B9C bit and native observation counters. Clearing1008's
// presence makes its native manager-index payload inactive; no racer is reset.
// This leaf does not perform2255B0 or finish an AI dispatch. All stores are
// exact constants/bit operations and have no arithmetic-domain dependency.
inline void resetAiRuntimeScalars50IV(AiRuntimeState50IV& ai,
    std::uint8_t& flagsB9C) noexcept {
    ai.state.aiStateFE8=0;                     //16E834
    ai.referenceFF8=-1;                       //16E838
    ai.packing.scaleFFC=1.f;                  //16E840
    ai.packing.policy1000=0;                  //16E844
    ai.target1008Present=false;               //16E848
    ai.timer100C=0.f;                         //16E84C
    ai.timer1010=0.f;                         //16E850
    ai.timer1014=0.f;                         //16E854
    ai.repair.cooldown1018=0.f;               //16E858
    ai.repair.duration101C=0.f;               //16E85C
    ai.mode1050=0;                           //16E860
    ai.navigation.choice1080=-1;             //16E864
    ai.navigation.choice1084=-1;             //16E868
    ai.field104C=-1.f;                       //16E870
    ai.boost.cooldown109C=6.f;                //16E878
    ai.state.stallTimer1090=1.5f;             //16E880
    ai.state.wrongWayTimer1094=0.f;           //16E884
    ai.state.turnSide1098=0;                  //16E888
    flagsB9C=static_cast<std::uint8_t>(flagsB9C&0xfdu); //16E8A4
    ai.state.orientation10A0=0;              //16E8A8
    ai.repair.repairFEC=0;                    //16E8B0 return delay slot
}
//16E8C0's native controller span, called after readAiVehicleFrame50IV and
//before228724 capturesCA0/bindAiVehicleFrame50IV. Prefix failures stop the live
//source boundary; no caller may replay the same frame as if it never started.
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50IV&) noexcept {return true;}
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50MB& in) noexcept {
    return navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)&&
        navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)&&
        navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)&&
        in.orientationTrackBC.sameOwner(in.aiFF4);
}
// TC-CONSUMER-BEGIN 2
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50TC& in) noexcept {
    return in.actualWorld&&in.mapFF0.sameWorld(*in.actualWorld)&&
        navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)&&
        navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)&&
        navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)&&
        in.orientationTrackBC.sameOwner(in.aiFF4);
}
// TC-CONSUMER-END 2

// TB-CONSUMER-BEGIN 2
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50TB& in) noexcept {
    return in.actualWorld&&in.mapFF0.sameWorld(*in.actualWorld)&&
        navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)&&
        navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)&&
        navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)&&
        in.orientationTrackBC.sameOwner(in.aiFF4);
}
// TB-CONSUMER-END 2

// GB-CONSUMER-BEGIN 2
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50GB& in) noexcept {
    return in.actualWorld&&in.mapFF0.sameWorld(*in.actualWorld)&&
        navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)&&
        navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)&&
        navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)&&
        in.orientationTrackBC.sameOwner(in.aiFF4);
}
// GB-CONSUMER-END 2

// MC-CONSUMER-BEGIN 2
inline bool driveOriginalAiAdmission50MB(const AiRuntimeInput50MC& in) noexcept {
    return in.actualWorld&&in.mapFF0.sameWorld(*in.actualWorld)&&
        navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)&&
        navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)&&
        navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)&&
        in.orientationTrackBC.sameOwner(in.aiFF4);
}
// MC-CONSUMER-END 2

template<class Input>
    requires (std::is_same_v<Input,AiRuntimeInput50IV>||std::is_same_v<Input,AiRuntimeInput50MB>||std::is_same_v<Input,AiRuntimeInput50TC>||std::is_same_v<Input,AiRuntimeInput50TB>||std::is_same_v<Input,AiRuntimeInput50GB>||std::is_same_v<Input,AiRuntimeInput50MC>)
inline bool driveOriginalAi50IVShared50MB(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const Input&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!driveOriginalAiAdmission50MB(in)) {receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;}

    const auto* mapFF0=navigation_consumer_detail50MB::pointer(in.mapFF0);
    const auto* aiFF4=navigation_consumer_detail50MB::pointer(in.aiFF4);
    const auto* orientationTrackBC=navigation_consumer_detail50MB::pointer(in.orientationTrackBC);
    const auto* aiMetadata=navigation_consumer_detail50MB::pointer(in.aiMetadata);
    using namespace ai_driving_detail50IV;receipt={};
    auto stop=[&](std::uint32_t pc) noexcept {receipt.sourceBoundary=pc;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;};
    // A failed prior state5 dispatch owns a partial source frame. Never replay
    // navigation, packing, repair or16E830 after its physical-reset boundary.
    if(ai.physicalReset2255B0Pending)return stop(0x2255b0u);
    if(!ai.initialized||frame.fields.vehicle58!=1u||!frame.fields.nativeAiOwnersBound50IV||
       frame.phase!=VehicleFramePhase49W::PostPlayer)return stop(0x16e8c0);
    parameters.rates.x=4.f; //16E8D4 owns live B94 even when dispatch is skipped.
    if(!in.applicationGlobals)return stop(0x16e950);
    if(in.applicationGlobals->paused433CB0||frame.fields.vehicle3C||!(frame.fields.applicationByte58&2u))return true;
    if(!mapFF0||!aiFF4||!orientationTrackBC||!aiMetadata||!in.tuning||
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
    const typename navigation_consumer_detail50MB::RuntimeTypes<Input>::RouteOwners routeOwners{*aiMetadata,ai.fieldFE4,
        ai.state.aiStateFE8,ai.packing.policy1000,frame.noiseOwner().sharedRandomState,
        &ai.mode1050,&ai.target1008Present,&ai.target1008ManagerIndex,in.racers,in.racerCount};
    if(!advanceAiNavigation50IV(*mapFF0,*aiFF4,*aiMetadata,previousCA0,position,
        in.actualZoneOrdinal48,progress,lapCC8,frame.fields.impactFlagsCD8,ai.navigation,ai.boost,routeOwners,receipt.driving))return stop(receipt.driving.sourcePC);
    const auto current=progress.reset.vehicleC80,previous=progress.vehicleC84;
    if(current>=navigation_consumer_detail50MB::read(*orientationTrackBC).track.count||previous>=navigation_consumer_detail50MB::read(*orientationTrackBC).track.count)return stop(0x173af0);
    AiOrientationInput50IV orientation{};
    orientation.applicationFlags58=frame.fields.applicationByte58;orientation.vehicleFlagsCD8=frame.fields.impactFlagsCD8;
    orientation.vehicleState40=m.vehicleState40;orientation.preparedFactor=m.frameFactor;
    for(unsigned axis=0;axis<3;++axis){orientation.localForward340_288[axis]=bodyScene.localC0.v[8+axis];
        orientation.currentC80Position[axis]=navigation_consumer_detail50MB::read(*orientationTrackBC).track.nodes[current].position[axis];
        orientation.previousC84Position[axis]=navigation_consumer_detail50MB::read(*orientationTrackBC).track.nodes[previous].position[axis];}
    //3C9410..3C946C initializes433C70 to(0,0,0,1); XYZ fallback is zero.
    AiOrientationReceipt50IV oriented{};
    if(!updateAiOrientation50IV(orientation,ai.state,oriented,receipt.prefix))return stop(receipt.prefix.sourcePC);
    AiNoCombatInput50IV stateInput{};
    stateInput.timerCB8=in.timing->timeCB8;stateInput.combat1054=ai.profile.combativeness1054;
    stateInput.combatWait106C=ai.profile.startRaceNoCombatTime106C;stateInput.remainingCC4=in.raceFields->remainingCC4;
    stateInput.lapCC8=static_cast<std::int32_t>(lapCC8);stateInput.placeCCC=in.raceFields->placeCCC;
    stateInput.fieldFE4=ai.fieldFE4;stateInput.managerField34=in.managerHumanCount34;
    stateInput.applicationFlags58=frame.fields.applicationByte58;stateInput.settings1125C=in.settings1125C;
    stateInput.currentNodeFlags79=navigation_consumer_detail50MB::read(*aiMetadata).nodes[current].flags79;
    stateInput.managerRacer0Present=in.racerCount>0&&in.racers[0].bound;
    stateInput.managerRacer1Present=in.racerCount>1&&in.racers[1].bound;
    if(stateInput.managerRacer0Present)stateInput.managerRacer0State40=in.racers[0].vehicle40;
    if(stateInput.managerRacer1Present)stateInput.managerRacer1State40=in.racers[1].vehicle40;
    for(unsigned axis=0;axis<3;++axis){stateInput.turnSide.currentNodePosition[axis]=navigation_consumer_detail50MB::read(*aiFF4).track.nodes[current].position[axis];
        stateInput.turnSide.scene190Position[axis]=position[axis];stateInput.turnSide.scene190LocalForward[axis]=forward[axis];}
    AiStateDispatchReceipt50IV dispatch{};
    if(!advanceAiStateNoCombat50IV(stateInput,ai.state,dispatch,receipt.prefix))return stop(receipt.prefix.sourcePC);
    // Source1705C8..1706B8. The same actual manager scan is called by state2
    // and by state1 after170480 misses. Target1008 stores precede every guard;
    // a genuine false geometry result continues, while a native boundary stops.
    auto scanCandidates=[&](bool& accepted) noexcept {
        accepted=false;
        for(std::size_t index=0;index<in.racerCount;++index){
            if(index==ai.managerIndexFE0)continue;
            const auto& candidate=in.racers[index];
            if(!candidate.bound||candidate.managerIndex!=index)return stop(0x1af330u);
            ai.target1008Present=true;ai.target1008ManagerIndex=candidate.managerIndex;
            ai_prefix_detail50IV::Math scan;
            const float candidateSpeed=scan.convertedCachedSpeed34(candidate.cachedSpeed34); //17061C delay, even AI rejection
            if(!scan.valid)return stop(0x17061cu);
            if(candidate.vehicle58==1u){++ai.combatAiRejections;continue;}
            if(candidate.vehicle40||candidateSpeed<200.f||candidate.vehicle58!=0u)continue;
            typename navigation_consumer_detail50MB::RuntimeTypes<Input>::CandidateInput input{};
            input.trackFF4=in.aiFF4;input.metadata=in.aiMetadata;input.target=&candidate;
            input.selfPosition190=position;input.selfNodeC80=current;
            input.selfLapCC8=static_cast<std::int32_t>(lapCC8);
            input.selfPlaceCCC=in.raceFields->placeCCC;input.combatZoneLength107C=ai.profile.combatZoneLength107C;
            input.diagnostic=in.combatDiagnostic;input.diagnosticContext=in.combatDiagnosticContext;
            AiCombatCandidateReceipt50LH selected{};
            if(!evaluateAiCombatCandidate50LH(input,selected,receipt.driving))return stop(receipt.driving.sourcePC);
            if(!selected.accepted)continue;
            ai.mode1050=selected.mode1050;
            if(selected.clearRouteChoices){ai.navigation.choice1080=-1;ai.navigation.choice1084=-1;}
            accepted=true;++ai.combatScans;return true;
        }
        ++ai.combatScans;return true;
    };
    if(!dispatch.complete){
        if(dispatch.boundaryPC==0x16ec6cu&&ai.state.aiStateFE8==3u){
            if(!advanceOriginalAiState3Timer50LH(m.frameFactor,ai.timer100C,
                ai.state.aiStateFE8,receipt.prefix))return stop(receipt.prefix.sourcePC);
        }else if(dispatch.boundaryPC==0x170480u&&ai.state.aiStateFE8==1u){
            AiCombatContactView50LH contact{};
            contact.count238Bits=m.components.root.callbackState238;
            contact.slots23C=m.components.root.processedContactPartners23C;
            contact.slotCount=10;
            if(static_cast<std::int32_t>(contact.count238Bits)>0&&ai.target1008Present&&
               ai.target1008ManagerIndex<in.racerCount&&in.resolveBody340){
                contact.targetBody340=in.resolveBody340(in.body340Context,ai.target1008ManagerIndex);
                contact.targetBody340Bound=contact.targetBody340.value!=0;
            }
            bool hit=false;
            if(!checkOriginalAiAttackContact17048050LH(contact,ai.profile.waitToRepairTime1060,
                ai.repair.cooldown1018,hit,receipt.prefix))return stop(receipt.prefix.sourcePC);
            if(hit){
                if(!completeOriginalAiAttackHit50LH(ai.target1008Present,ai.state.aiStateFE8,
                    ai.timer100C,ai.profile.attackRecoveryTime1068,in.raceFields->placeCCC,
                    ai.fieldFE4,receipt.prefix))return stop(receipt.prefix.sourcePC);
            }else{
                //1704E0 no-write guards, in the exact same order as state2.
                ai_prefix_detail50IV::Math guard;bool scan=true;
                if(guard.read(stateInput.combat1054)==0.f)scan=false;
                else if(guard.read(stateInput.timerCB8)<guard.read(stateInput.combatWait106C))scan=false;
                else if(stateInput.lapCC8==static_cast<std::int32_t>(stateInput.settings1125C&31u)-1&&
                        guard.read(stateInput.remainingCC4)<2000.f)scan=false;
                else if(stateInput.currentNodeFlags79&2u)scan=false;
                else if(stateInput.fieldFE4&&stateInput.placeCCC!=1)scan=false;
                if(!guard.valid)return stop(0x1704e0u);
                bool accepted=false;if(scan&&!scanCandidates(accepted))return false;
                completeOriginalAiState1Scan50LH(accepted?1u:0u,ai.target1008Present,
                    ai.field104C,ai.state.aiStateFE8);
            }
        }else if(dispatch.boundaryPC==0x1705c8u&&ai.state.aiStateFE8==2u){
            bool accepted=false;if(!scanCandidates(accepted))return false;
            if(accepted)ai.state.aiStateFE8=1; //16ED50 delay slot
        }else return stop(dispatch.boundaryPC);
    }
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
    if(ai.state.aiStateFE8==5u){
        //3A7C34 dispatches to16EA94 only AFTER171C60 and1734C0. The first
        //real operation is16EA98->16E830, followed by16EAA0->2255B0.
        //2255B0's AI virtual78 is216A30 (3CE258), not the player override.
        resetAiRuntimeScalars50IV(ai,m.controls.player.flagsB9C);
        ++ai.scalarResets16E830;receipt.scalarResetCompleted=true;
        ai.physicalReset2255B0Pending=true;
        // Retain the exact physical-reset boundary for the concrete native
        // owner continuation. It must complete2255B0/AI216A30 and return via
        //16EAA8 directly to16EAE8, never fall through the ordinary state0
        // branch below. Until then no drive/boost/brake or dispatch credit is
        // produced, and the canonical frame phase remains PostPlayer.
        return stop(0x2255b0u);
    }
    const bool attackDispatch=ai.state.aiStateFE8==1u;
    PlayerReadResult48X stagedActions=m.controls.player;
    //1711E0 clears B9C before geometry; later source boundaries must retain
    //that write and completed steering. Ordinary existing paths retain their
    //prior staged-controls contract.
    PlayerReadResult48X& actions=attackDispatch?m.controls.player:stagedActions;
    Math f;V target{{0,0,0,1}},direction{{0,0,0,1}};
    bool attemptBoost=true;float boostFactor=m.frameFactor;
    auto ordinarySteering172380=[&]() noexcept {
        //172380 early start-line return preserves B60 and1090.
        if(!lapCC8&&current==1u)actions.raw.values[0]=0.f;
        else{
            if(!(frame.fields.applicationByte58&1u)&&m.cachedSpeed34<10.f&&!m.vehicleState40)
                ai.state.stallTimer1090=f.sub(ai.state.stallTimer1090,m.frameFactor);
            else if(!m.components.probe348.field104&&m.response354_3C&&std::fabs(scene.localC0.v[5])<ai_speed_detail50IR::word(0x3e99999au))
                ai.state.stallTimer1090=f.sub(ai.state.stallTimer1090,m.frameFactor);
            else ai.state.stallTimer1090=1.5f;
            V nearTarget{{0,0,0,1}};
            if(!aiDesiredPosition50IV(*aiFF4,progress,ai.navigation,routeOwners,position,f.mul(m.cachedSpeed34,in.tuning->lookahead78),target,receipt.driving)||
               !aiDesiredPosition50IV(*aiFF4,progress,ai.navigation,routeOwners,position,f.mul(m.cachedSpeed34,ai_speed_detail50IR::word(0x3e4ccccdu)),nearTarget,receipt.driving))return stop(receipt.driving.sourcePC);
            const V velocity=normalize(f,point(m.components.root.body.velocity.v));
            const V distant=normalize(f,sub(f,target,point(position))),nearby=normalize(f,sub(f,nearTarget,point(position)));
            direction=normalize(f,dot(f,distant,velocity)<dot(f,nearby,velocity)?distant:nearby);
            // PC enhancement (native_zero_gravity_step50zg.h): zero-g flight steers about the pod's up.
            rr::game::AiSteerFrame50ZG zeroG{direction.v,forward,scene.localC0.v[5],m.components.root.worldAngularVelocityA0.v[1]};
            if(rr::game::zeroGravityFlightForSlot50ZG(ai.managerIndexFE0))zeroG.podFrame(scene.localC0.v,m.components.root.worldAngularVelocityA0.v);
            if(!steerAi50IV(zeroG.direction,zeroG.forward,zeroG.upY,m.cachedSpeed34,ai.engineDifference5C,
                zeroG.angularA4,ai.racer60,actions,receipt.driving))return stop(receipt.driving.sourcePC);
        }
        return f.valid||stop(0x172380u);
    };
    auto ordinaryTargetSpeed172C50=[&]() noexcept {
        AiSpeedTargetInput50IR speed{};speed.currentNodeSpeedLimit5C=navigation_consumer_detail50MB::read(*aiMetadata).nodes[current].speedLimit5C;
        speed.currentNodeFlags79=navigation_consumer_detail50MB::read(*aiMetadata).nodes[current].flags79;speed.vehicleSpeedScaleFFC=ai.packing.scaleFFC;
        speed.vehicleMinimumSpeed108C=ai.profile.myMinSpeed108C;speed.trackControlFlags58=frame.fields.applicationByte58;
        speed.vehicleFlagsCD8=frame.fields.impactFlagsCD8;speed.vehicleAiStateFE8=ai.state.aiStateFE8;
        speed.trackRacePace7C=in.tuning->racePace7C;speed.settings188F60Nonzero=in.settings188F60Nonzero;
        speed.vehicleCCC=in.raceFields->placeCCC;speed.vehicleCB0=in.timing->timeCB0;
        speed.vehicleCC4=in.raceFields->remainingCC4;speed.aiTrackLength3C=in.aiTrackLength3C;
        AiSpeedTargetReceipt50IR selected{};if(!selectAiTargetSpeed50IR(speed,selected,receipt.speed))return stop(receipt.speed.sourcePC);
        receipt.desiredSpeed=selected.desiredSpeed;
        return true;
    };
    if(attackDispatch){
        AiAttackDriveSelection50LH selection{};
        if(!beginAiAttackDrive1711E0(*aiFF4,current,ai.mode1050,actions.flagsB9C,
            ai.field104C,in.applicationGlobals->attackDrive,selection,receipt.driving))
            return stop(receipt.driving.sourcePC);
        const AiObservedRacer50IV* attackTarget=nullptr;
        if(ai.target1008Present&&ai.target1008ManagerIndex<in.racerCount){
            const auto& candidate=in.racers[ai.target1008ManagerIndex];
            if(candidate.bound&&candidate.managerIndex==ai.target1008ManagerIndex)attackTarget=&candidate;
        }
        if(selection.wideSteering16EDD0){
            if(!attackTarget)return stop(0x16ee10u);
            if(!aiAttackWideSteering16EDD0(*aiFF4,progress,*attackTarget,position,forward,
                scene.localC0.v[5],m.cachedSpeed34,ai.engineDifference5C,
                m.components.root.worldAngularVelocityA0.v[1],ai.racer60,
                in.applicationGlobals->attackWide,ai.field104C,actions,receipt.driving))
                return stop(receipt.driving.sourcePC);
        }else if(!ordinarySteering172380())return false;
        if(selection.attackSpeed16FB50){
            if(!attackTarget)return stop(0x16fb80u);
            typename navigation_consumer_detail50MB::RuntimeTypes<Input>::AttackInput input{};input.trackFF4=in.aiFF4;input.metadata=in.aiMetadata;
            input.target=attackTarget;input.selfPosition190=position;input.selfNodeC80=current;
            input.selfLapCC8=static_cast<std::int32_t>(lapCC8);input.myMinSpeed108C=ai.profile.myMinSpeed108C;
            input.preparedFactor=m.frameFactor;input.selfCachedSpeed34=m.cachedSpeed34;
            input.diagnostic=in.combatDiagnostic;input.diagnosticContext=in.combatDiagnosticContext;
            AiAttackSpeedReceipt50LH attacked{};
            if(!applyAiAttackSpeed16FB50(input,actions,attacked,receipt.driving,receipt.speed))
                return stop(receipt.driving.sourcePC);
            receipt.desiredSpeed=attacked.desiredSpeed;receipt.braking=attacked.speed.braking;
            if(receipt.braking)++ai.brakingFrames;
            //16FB50 terminates at173770. Do not invoke173180 or173770 twice.
            ++ai.attackFrames;++ai.dispatchedFrames;receipt.dispatched=true;
            receipt.stateFE8=ai.state.aiStateFE8;return true;
        }
        if(!ordinaryTargetSpeed172C50())return false; //1712FC, then ordinary boost/brake
    }else if(ai.state.aiStateFE8==4u){
        if(!buildAiRacerLists50IV(*aiFF4,progress,ai.profile,in.racers,in.racerCount,ai.managerIndexFE0,
            position,forward,ai.lists,receipt.driving)||
           !aiDesiredPosition50IV(*aiFF4,progress,ai.navigation,routeOwners,position,200.f,target,receipt.driving))return stop(receipt.driving.sourcePC);
        direction=normalize(f,sub(f,target,point(position)));
        rr::game::AiSteerFrame50ZG zeroG{direction.v,forward,scene.localC0.v[5],m.components.root.worldAngularVelocityA0.v[1]};
        if(rr::game::zeroGravityFlightForSlot50ZG(ai.managerIndexFE0))zeroG.podFrame(scene.localC0.v,m.components.root.worldAngularVelocityA0.v);
        if(!steerAi50IV(zeroG.direction,zeroG.forward,zeroG.upY,m.cachedSpeed34,ai.engineDifference5C,
            zeroG.angularA4,ai.racer60,actions,receipt.driving)||
           !aiStartupSpeed50IV(navigation_consumer_detail50MB::read(*aiMetadata).nodes[current],ai.profile,ai.lists,in.racers,in.racerCount,
            position,m.components.root.body.velocity.v,actions,receipt.desiredSpeed,attemptBoost,receipt.driving))return stop(receipt.driving.sourcePC);
        boostFactor=1.f;++ai.startupFrames;
    }else if(ai.state.aiStateFE8==0u||ai.state.aiStateFE8==2u||ai.state.aiStateFE8==3u||
             ai.state.aiStateFE8==6u){
        if(!ordinarySteering172380())return false;
        if(ai.state.aiStateFE8==6u){
            //16EAB0 invokes the same172380 steering above, then172B50.
            //No target-speed selection, boost draw or ordinary brake call is
            //reached. Earlier navigation/state/packing/repair remain genuine.
            if(!f.valid)return stop(0x172380u);
            if(!in.raceCountdown94)return stop(0x172b98u);
            if(!applyOriginalAiEndingSpeed172B50(m.cachedSpeed34,*in.raceCountdown94,
                actions,receipt.speed))return stop(receipt.speed.sourcePC);
            m.controls.player=actions;++ai.dispatchedFrames;receipt.dispatched=true;
            receipt.stateFE8=ai.state.aiStateFE8;return true;
        }
        if(!ordinaryTargetSpeed172C50())return false;
        ++ai.ordinaryFrames;
    }else return stop(0x16e9fc);
    if(!f.valid)return stop(0x172380);
    if(attemptBoost){AiBoostDecisionInput50IT boost{boostFactor,receipt.desiredSpeed,thermal.heat7C,m.cachedSpeed34,
        ai.state.aiStateFE8,ai.lists.counts[0],ai.lists.counts[1],m.components.probe348.field10C,false,0};
        if(ai.target1008Present&&ai.target1008ManagerIndex<in.racerCount){
            const auto& candidate=in.racers[ai.target1008ManagerIndex];
            boost.target1008Present=candidate.bound&&candidate.managerIndex==ai.target1008ManagerIndex;
            if(boost.target1008Present)boost.target1008FlagsB9C=candidate.flagsB9C;
        }
        AiBoostDecisionReceipt50IT boosted{};
        if(!decideAiBoost50IT(boost,actions,actions,ai.boost,frame.noiseOwner().sharedRandomState,boosted,receipt.boost))return stop(receipt.boost.sourcePC);
        receipt.attemptedBoost=true;receipt.boostHandled=boosted.handled;
    }
    if(!receipt.boostHandled){AiNonBoostSpeedReceipt50IR braking{};
        if(!applyAiNonBoostSpeed50IR(receipt.desiredSpeed,m.cachedSpeed34,actions,actions,braking,receipt.speed))return stop(receipt.speed.sourcePC);
        receipt.braking=braking.braking;if(braking.braking)++ai.brakingFrames;
    }
    m.controls.player=actions;if(attackDispatch)++ai.attackFrames;
    ++ai.dispatchedFrames;receipt.dispatched=true;receipt.stateFE8=ai.state.aiStateFE8;return true;
}
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50IV&in,AiRuntimeReceipt50IV&receipt) noexcept {
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50MB&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // Reject incomplete/mixed authentic owners before a source frame or RNG
    // prefix starts. Same role plus same owner pins the orientation graph too.
    if(!navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)||
       !navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)||
       !navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)||
       !in.orientationTrackBC.sameOwner(in.aiFF4)) {
        receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;
    }
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
// TC-CONSUMER-BEGIN 3
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50TC&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // Reject incomplete/mixed authentic owners before a source frame or RNG
    // prefix starts. Same role plus same owner pins the orientation graph too.
    if(!in.actualWorld||!in.mapFF0.sameWorld(*in.actualWorld)||
       !navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)||
       !navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)||
       !navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)||
       !in.orientationTrackBC.sameOwner(in.aiFF4)) {
        receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;
    }
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
// TC-CONSUMER-END 3

// TB-CONSUMER-BEGIN 3
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50TB&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // Reject incomplete/mixed authentic owners before a source frame or RNG
    // prefix starts. Same role plus same owner pins the orientation graph too.
    if(!in.actualWorld||!in.mapFF0.sameWorld(*in.actualWorld)||
       !navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)||
       !navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)||
       !navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)||
       !in.orientationTrackBC.sameOwner(in.aiFF4)) {
        receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;
    }
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
// TB-CONSUMER-END 3

// GB-CONSUMER-BEGIN 3
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50GB&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // Reject incomplete/mixed authentic owners before a source frame or RNG
    // prefix starts. Same role plus same owner pins the orientation graph too.
    if(!in.actualWorld||!in.mapFF0.sameWorld(*in.actualWorld)||
       !navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)||
       !navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)||
       !navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)||
       !in.orientationTrackBC.sameOwner(in.aiFF4)) {
        receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;
    }
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
// GB-CONSUMER-END 3

// MC-CONSUMER-BEGIN 3
inline bool driveOriginalAi50IV(VehicleFrameState49W&frame,const VehicleFrameBindings49W&bindings,
    VehicleFrameParameters49W&parameters,TrackNavigationProgress50EU&progress,
    const float previousCA0[4],std::uint32_t&lapCC8,BoostState50FK&thermal,
    AiRuntimeState50IV&ai,const AiRuntimeInput50MC&in,AiRuntimeReceipt50IV&receipt) noexcept {
    // Reject incomplete/mixed authentic owners before a source frame or RNG
    // prefix starts. Same role plus same owner pins the orientation graph too.
    if(!in.actualWorld||!in.mapFF0.sameWorld(*in.actualWorld)||
       !navigation_consumer_detail50MB::pairReady(in.mapFF0,in.aiFF4)||
       !navigation_consumer_detail50MB::aiJoined(in.aiFF4,in.aiMetadata)||
       !navigation_consumer_detail50MB::aiReady(in.orientationTrackBC)||
       !in.orientationTrackBC.sameOwner(in.aiFF4)) {
        receipt={};receipt.sourceBoundary=0x16e9c0u;receipt.stateFE8=ai.state.aiStateFE8;
        receipt.physicalResetPending=ai.physicalReset2255B0Pending;return false;
    }
    return driveOriginalAi50IVShared50MB(frame,bindings,parameters,progress,previousCA0,lapCC8,thermal,ai,in,receipt);
}
// MC-CONSUMER-END 3

} //namespace rr::game
