#pragma once
#include "original_race_camera_owner.h"
#include "original_race_start_camera.h"
#include "original_camera_child_registry.h"
#include <array>

namespace rr::game {
// Zero-effective-cut controller join. GC/GA author0; TA authors3 but174DF0
// genuinely suppresses those cuts for admitted launch1/config0-or3/app350=0.
// Raw track3E4 is retained separately; this join consumes controllerA8.
// Register1752D0's Follow, other, attached, free, optional TV in that order,
// then register the prepared procedural child if2D4DC0 succeeded. The cold
// controller retains mode4. It selects this procedural child on the first
// admitted175D70 frame; there is no native force-Follow shortcut.
struct GcCameraStartContext {
    std::int32_t sourceControllerCutCountA8=0;
    std::int32_t sourceLaunchKind11258=1;
    // Real per-view preferences read by178700. Defaults are NOT established
    // by this record: provide actual initialized configuration11030/11038.
    std::int32_t savedCameraMode11030=-1, savedFollowVariant11038=-1;
    std::uint8_t rootFlags228=0, sceneFlags29=0;
};
struct OriginalStartChildBinding {
    OriginalRaceStartCamera* start7C=nullptr;
    std::uint64_t start7CKey=0;
    OriginalCameraChildRegistry* collection=nullptr;
};
//Value projection only. Bind/commit these fields against actual BaseTailState50X
//and VehicleFrameFields49W using original_camera_racer_fields.h in real bank
//leases. Do not install this bank as a second mutable racer-field owner.
struct GcStartRacerEffectFields {
    std::uint64_t racerKey=0;
    std::uint32_t wordAE8=0,word844=0;
    std::array<std::uint32_t,5> wordsAD4ThroughAE4{};
};
struct GcStartRacerEffectBank {
    std::array<GcStartRacerEffectFields,8> racers{};
};
enum class GcStartFrameStatus : std::uint8_t {
    posePublished, needsOriginalExit, needsDirectStart, requiresOtherSourcePath,
    invalidBinding, invalidState, unsupportedNumericDomain
};
struct GcStartFrameReceipt {
    GcStartFrameStatus status=GcStartFrameStatus::invalidBinding;
    bool activatedStartCamera=false, requestedBlur=false;
    std::int32_t blurStrength=0;
};
// Closed zero-cut GC primary controller4->7 and continuing7, with source
// constructor-cut timer prefix, genuine2D6DD0 activation,1AEEA0 common-racer
// fields and complete procedural pose+49F view. This specific admission has no
// CameraShake action: source94 must be negative. Event-active frames must use
// the complete controller prefix rather than silently freezing their timers.
// Requirements: owner is actual camera0/racer0; current racer state0; source
// race1B0 bit4 clear; no special58bit0/CD4zero branch; intro skip bit62.7 clear.
// These are runtime source gates, not booleans to set merely to pass admission.
// A completed child returns needsOriginalExit without consuming the frame or
// flipping the mode. The real exit includes177AB0 and other race side effects.
inline GcStartFrameReceipt advanceOriginalGcCameraStartFrame(
    OriginalRaceCameraOwner& owner,OriginalRaceStartCamera& start,OriginalStartChildBinding& child,
    GcStartRacerEffectBank& racerEffects,const RaceStartRacerBank& bank,
    const StableRaceCameraFrame& frame,const GcCameraStartContext& context,
    GameplayBlurPending& pending,CameraSceneState49F& scene,
    TVCameraUpdateOutput49F& pose,CameraViewOutput49F& view) noexcept {
    namespace d=original_race_camera_detail;
    GcStartFrameReceipt result;
    if(child.start7C!=&start||!child.start7CKey||!child.collection||!child.collection->bound)return result;
    auto& collection=*child.collection;
    if(collection.sessionGeneration!=owner.binding.sessionGeneration||
       collection.sceneKey!=owner.binding.sceneKey||!collection.controllerKey||
       !collection.registered38||!collection.count30||collection.count30>64||
       collection.selected34>=collection.count30)return result;
    std::size_t selected=collection.count30;
    for(std::size_t i=0;i<collection.count30;++i){
        if(!collection.registered38[i])return result;
        if(selected==collection.count30&&collection.registered38[i]==child.start7CKey)selected=i;
    }
    if(selected==collection.count30||(owner.mode44==7&&collection.selected34!=selected))return result;
    if(!owner.bound||owner.binding.managerIndex!=0||
        owner.binding.sessionGeneration!=bank.sessionGeneration||
        owner.binding.sessionGeneration!=frame.sessionGeneration||
        owner.binding.racerKey!=bank.racers[0].racerKey||
        owner.binding.racerKey!=frame.racerKey||owner.binding.mainBodyKey!=frame.mainBodyKey)
        return result;
    if(!bank.racerCount||bank.racerCount>8)return result;
    for(std::size_t i=0;i<bank.racerCount;++i)if(racerEffects.racers[i].racerKey!=bank.racers[i].racerKey)return result;
    result.status=GcStartFrameStatus::requiresOtherSourcePath;
    if((context.sourceControllerCutCountA8!=0&&!(context.sourceControllerCutCountA8==3&&owner.mode44==7))||(context.sourceLaunchKind11258<1||context.sourceLaunchKind11258>3)||
        (owner.mode44!=4&&owner.mode44!=7)||(frame.raceFlags1B0&3u)!=1u||frame.racerState40!=0||
        (frame.raceFlags1B0&0x10)||((frame.raceStateFlags58&1)&&frame.racerCD4==0)||
        (owner.flags62&0x80)||owner.shakeRemaining94>=0)return result;
    if(owner.mode44==7&&start.complete240){result.status=GcStartFrameStatus::needsOriginalExit;return result;}
    result.status=GcStartFrameStatus::unsupportedNumericDomain;
    if(std::fegetround()!=FE_TONEAREST||!d::scalar(frame.elapsed)||frame.elapsed<0||
        !d::scalar(owner.cooldown8C)||!d::scalar(owner.cutRemaining90)||!d::scalar(owner.shakeRemaining94))return result;
    auto nextOwner=owner;auto nextStart=start;auto nextEffects=racerEffects;auto nextCollection=collection;
    auto nextPending=pending;auto nextScene=scene;auto nextPose=pose;auto nextView=view;
    if(nextOwner.cooldown8C>0){nextOwner.cooldown8C=d::sub(nextOwner.cooldown8C,frame.elapsed);if(!d::scalar(nextOwner.cooldown8C))return result;}
    bool requested=false;std::int32_t strength=0;
    if(nextOwner.cutRemaining90>=0){
        const float ratio=d::div(nextOwner.cutRemaining90,2),amount=d::mul(100,ratio);
        if(!d::scalar(ratio)||!d::strength(amount,strength))return result;
        requestGameplayBlur(nextPending,strength,0,0);requested=true;
        if(nextOwner.cutRemaining90==0)nextOwner.cutRemaining90=-1;
        else {const float remaining=d::sub(nextOwner.cutRemaining90,frame.elapsed);
            if(!d::scalar(remaining))return result;nextOwner.cutRemaining90=remaining<=0?0:remaining;}
    }
    nextOwner.flags63Low5=static_cast<std::uint8_t>(nextOwner.flags63Low5&~0x18u);
    //176290..1762F4: no procedural route, or actual event type2, selects
    //the distinct direct-start transaction after the common timer prefix.
    if(nextOwner.mode44==4&&(nextStart.route[0]==-1||context.sourceLaunchKind11258==2)){
        owner=nextOwner;pending=nextPending;
        result.status=GcStartFrameStatus::needsDirectStart;return result;
    }
    bool activated=false;
    if(nextOwner.mode44==4){
        if(!nextStart.prepared||nextStart.route[0]==-1){result.status=GcStartFrameStatus::requiresOtherSourcePath;return result;}
        const auto status=activateOriginalRaceStartCamera(nextStart,bank);
        if(status!=RaceStartStatus::ok){result.status=GcStartFrameStatus::invalidState;return result;}
        //1762C4 selects registered child7C.1762D4 sets mode7;1762D8 resets
        //the selected child.1AEEA0 writes common AE8=1 for every actual racer.
        nextCollection.selected34=selected;nextOwner.mode44=7;
        for(auto& racer:nextEffects.racers)racer.wordAE8=1;
        activated=true;
    }
    nextOwner.flags62=static_cast<std::uint8_t>(nextOwner.flags62&~0x80u);
    const auto status=advanceOriginalRaceStartCameraView(nextStart,bank,frame.elapsed,nextScene,nextPose,nextView,
        context.rootFlags228,context.sceneFlags29);
    if(status!=RaceStartStatus::ok){result.status=status==RaceStartStatus::unsupportedNumericDomain?
        GcStartFrameStatus::unsupportedNumericDomain:GcStartFrameStatus::invalidState;return result;}
    owner=nextOwner;start=nextStart;collection=nextCollection;racerEffects=nextEffects;pending=nextPending;scene=nextScene;pose=nextPose;view=nextView;
    result.status=GcStartFrameStatus::posePublished;result.activatedStartCamera=activated;
    result.requestedBlur=requested;result.blurStrength=strength;return result;
}

enum class GcStartExitOperation : std::uint8_t {
    setControllerIntroFinished, resetAllCommonRacers, requestFade500,
    setRaceFlag8, restoreSavedCamera, clearFollowFields58Through64,
    startRaceState, revealRaceUi, setControllerCooldown1,
    setControllerIntroFinishedAgain, stopIntroAudioIfPresent, applyCommonRacerModeCorrection
};
struct GcStartExitPlan {
    // Exact no-skip mode7 completion path176664..176724. Order is semantic.
    std::array<GcStartExitOperation,12> order{{
        GcStartExitOperation::setControllerIntroFinished,GcStartExitOperation::resetAllCommonRacers,
        GcStartExitOperation::requestFade500,GcStartExitOperation::setRaceFlag8,
        GcStartExitOperation::restoreSavedCamera,GcStartExitOperation::clearFollowFields58Through64,
        GcStartExitOperation::startRaceState,GcStartExitOperation::revealRaceUi,
        GcStartExitOperation::setControllerCooldown1,GcStartExitOperation::setControllerIntroFinishedAgain,
        GcStartExitOperation::stopIntroAudioIfPresent,GcStartExitOperation::applyCommonRacerModeCorrection}};
    std::int32_t requestedMode=-1,followVariant=-1;
    std::int32_t oldIntroIndex=-1,newIntroIndex=-1;
    //176780..1767C0: with the admitted current racer state0, modes1/6
    //restore once and then source177AB0 selects Follow0 before the pose.
    bool secondSourceSwitchToFollow=false;
    std::uint32_t fadeMilliseconds=500;
    //2AED60(a1=500,a2=0,a3=0,t0=0). This is a fade request, NOT blur.
};
inline bool describeOriginalGcStartExit(const OriginalRaceCameraOwner& owner,
    const OriginalRaceStartCamera& start,const GcCameraStartContext& context,
    GcStartExitPlan& plan) noexcept {
    if(!owner.bound||owner.mode44!=7||!start.prepared||!start.active||!start.complete240||
        owner.binding.sessionGeneration!=start.sessionGeneration||
        context.sourceControllerCutCountA8!=0||context.savedCameraMode11030<0||
        context.savedCameraMode11030>6||context.savedCameraMode11030==3||
        context.savedCameraMode11030==4||(owner.flags62&0x80u))return false;
    GcStartExitPlan next{};next.requestedMode=context.savedCameraMode11030;
    next.followVariant=context.savedFollowVariant11038;
    next.secondSourceSwitchToFollow=next.requestedMode==1||next.requestedMode==6;
    //Primary view0 takes config11030/11038.178700 only clamps variants0/2
    //for viewCount>=2, which this fresh one-view lifecycle does not claim.
    plan=next;return true;
}
//1AEEE0->2179B0 numeric common-racer part. Invoke at the above ordered stage
//against the actual fields, not AI1090/1094 or a Player reinterpretation.
inline void resetOriginalGcStartRacerFields(GcStartRacerEffectBank& bank) noexcept {
    for(auto& r:bank.racers){r.wordAE8=0;r.word844=0;r.wordsAD4ThroughAE4.fill(0);}
}
//Exit's199770(0,1,1,1,1) reduces to these writes; transitionTau1E0 stays.
inline void revealOriginalGcRaceUi(std::uint8_t& flags1E4) noexcept {
    flags1E4=static_cast<std::uint8_t>((flags1E4|0x1cu)&~1u);
}
//Do not set owner.mode44 from this plan alone. The actual177AB0 target-child
//activation, scene-parent rebinding,1AFD80 race-state effects,2AED60 fade, and
//optional2924A0 audio stop must execute in the indicated source order. This
//header deliberately cannot claim those externally owned effects happened.
}
