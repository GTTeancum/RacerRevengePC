#pragma once
#include "control_presets49d.h"
#include "original_attached_camera_controller.h"
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rr::game {
// Source-only candidate. Original ELF c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
// Numeric cycle slice of camera press176DF0, with concrete177AB0 transitions.
// This is NOT a replacement root input router. Invoke once for EACH genuine
// press callback reaching this camera. The source deliberately ignores event
// code/device here and queries current held actions; no timer debounce exists.
// Keep the real event queue, application gate, and final142350 child route.
// A native camera-only device may feed these held records independently of AI;
// never feed its controls into any racer or change the ALL8 racer classes.
struct OriginalCameraModeInputEnvironment {
    const ManagerHeldSources49A& held;
    const ControlPresets49D& preferences;
    // Actual raceB4->3C[chosen input slot]->44, not common racer+40.
    //1AFB5C stores racer+C90 in3C[i], so this IS racerCD4 in initial
    //manager order. Borrow the actual banked LapTimingFields50ID.wordCD4.
    const std::uint32_t* raceViewState44=nullptr;
    std::size_t raceViewStateCount=0;
    std::uint32_t racerClass58=0;
    std::uint8_t cameraViewA8=0,raceFlags29=0,raceStateFlags58=0,raceFlags1B0=0;
};
enum class OriginalCameraCycleOperation : std::uint8_t {
    none,selectAttached5,selectFollow0ThenPresetId0,cycleFollowRecord
};
enum class OriginalCameraModeInputStatus : std::uint8_t {
    // These are explicit source cuts, not a claim that the press returned.
    needsChildRoute142350,readyForNumericChange,needsCycleAudio1A4B90,
    requiresTemporaryLookPress,requiresOtherCameraMode,
    queryUnavailable,inputSlot,owner,authoringWouldNotTerminate,
    transitionUnavailable
};
struct OriginalCameraModeInputResult {
    OriginalCameraModeInputStatus status=OriginalCameraModeInputStatus::owner;
    OriginalCameraCycleOperation operation=OriginalCameraCycleOperation::none;
    std::uint32_t sourcePC=0,inputSlot=0,queriesCompleted=0;
    bool prefixFlag62Written=false;
    ManagerHeldStatus49A queryStatus=ManagerHeldStatus49A::Complete;
    OriginalFollowTransitionResult transition{};
};
namespace original_camera_mode_input_detail {
inline std::int32_t signedView(std::uint8_t a8) noexcept {
    const auto n=static_cast<std::int32_t>(a8&15u);
    return n<8?n:n-16;
}
inline bool followBound(const OriginalFollowCamera& follow,
    const OriginalRaceCameraOwner& controller) noexcept {
    return controller.bound&&follow.prepared&&follow.bank.memberIdentity&&
        follow.bank.records&&follow.bank.count&&follow.current100<follow.bank.count&&
        follow.sessionGeneration==controller.binding.sessionGeneration&&
        follow.racerKey==controller.binding.racerKey&&
        follow.mainBodyKey==controller.binding.mainBodyKey&&
        follow.bank.count<=static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max());
}
}
//176DF0..177450 before actual177AB0/cycle calls. On query failure the already
//written flags62 prefix remains, matching source order; no rollback is claimed.
// Temporary look actions6A8/6B0 have priority. The supplied owner executes
//them in source order; an inspect-only caller retains the explicit cut.
template<class TemporaryLook>
inline OriginalCameraModeInputResult inspectOriginalCameraCyclePress176DF0(
    OriginalRaceCameraOwner& controller,const OriginalFollowCamera& follow,
    const OriginalCameraModeInputEnvironment& env,TemporaryLook&& temporaryLook) noexcept {
    using S=OriginalCameraModeInputStatus;using O=OriginalCameraCycleOperation;
    OriginalCameraModeInputResult result;
    if(!controller.bound)return result;
    //176E18: only class58==0 uses signed cameraA8 low nibble. AI/nonzero
    //classes intentionally query device0; they are NOT rejected or humanized.
    const auto slot=env.racerClass58==0?
        original_camera_mode_input_detail::signedView(env.cameraViewA8):0;
    if(slot<0||slot>=8){result.status=S::inputSlot;result.sourcePC=0x176e18u;return result;}
    result.inputSlot=static_cast<std::uint32_t>(slot);
    auto held=[&](std::int32_t code,std::uint32_t device,bool& hit) noexcept {
        const auto q=queryManagerHeld49A(env.held,code,2,0x3fu,device);
        result.queryStatus=q.status;
        if(q.status!=ManagerHeldStatus49A::Complete){result.status=S::queryUnavailable;return false;}
        ++result.queriesCompleted;hit=q.matched;return true;
    };
    bool hit=false;
    result.sourcePC=0x176e58u;
    if(!held(2,result.inputSlot,hit))return result;
    controller.flags62=static_cast<std::uint8_t>((controller.flags62&0x7fu)|(hit?0x80u:0u));
    result.prefixFlag62Written=true;
    //Two-view intro accepts code2 held on either view; the first true query
    //short-circuits the device1 query exactly as176EBC..176EF0.
    if((env.raceFlags1B0&3u)==2u&&(controller.mode44==4||controller.mode44==7)&&!hit){
        result.sourcePC=0x176ee8u;if(!held(2,1,hit))return result;
        controller.flags62=static_cast<std::uint8_t>((controller.flags62&0x7fu)|(hit?0x80u:0u));
    }
    if(env.raceFlags29&4u)controller.flags62=static_cast<std::uint8_t>(controller.flags62&0x7fu);
    result.status=S::needsChildRoute142350;result.sourcePC=0x17746cu;
    if((env.raceFlags29&4u)||(env.raceStateFlags58&1u))return result;
    if(!env.raceViewState44||result.inputSlot>=env.raceViewStateCount){
        result.status=S::inputSlot;result.sourcePC=0x176f7cu;return result;
    }
    if(env.raceViewState44[result.inputSlot]!=0u||!(env.raceStateFlags58&6u)||
       controller.mode44==2||controller.mode44==4||controller.mode44==7)return result;
    //Original row index is THIS input slot, not racer manager index or+FFC.
    if(result.inputSlot>=controlPresetSlots49D){result.status=S::inputSlot;result.sourcePC=0x176ffcu;return result;}
    const auto& actions=env.preferences.slots[result.inputSlot].actions;
    result.sourcePC=0x177008u;
    if(!held(actions.action6A8,result.inputSlot,hit))return result;
    if(hit&&!(controller.flags62&0x60u)){
        result.status=S::requiresTemporaryLookPress;result.sourcePC=0x17703cu;
        if(!temporaryLook(3,0x40u,result))return result;
    }
    result.sourcePC=0x177164u;
    if(!held(actions.action6B0,result.inputSlot,hit))return result;
    if(hit&&!(controller.flags62&0x60u)){
        result.status=S::requiresTemporaryLookPress;result.sourcePC=0x177198u;
        if(!temporaryLook(1,0x20u,result))return result;
    }
    result.sourcePC=0x1772c0u;
    if(!held(actions.action6A4,result.inputSlot,hit))return result;
    result.status=S::needsChildRoute142350;result.sourcePC=0x17746cu;
    if(!hit||(controller.flags62&0x60u))return result;
    if(controller.mode44!=0&&controller.mode44!=5){
        //Source still executes sound4 +178670 for these other eligible modes.
        //Their active camera owners are outside this closed native slice.
        result.status=S::requiresOtherCameraMode;result.sourcePC=0x17744cu;return result;
    }
    if(!original_camera_mode_input_detail::followBound(follow,controller)){
        result.status=S::owner;result.sourcePC=0x177318u;return result;
    }
    if(controller.mode44==5){
        result.operation=O::selectFollow0ThenPresetId0;result.sourcePC=0x177384u;
    }else{
        const auto lastId=(env.raceFlags1B0&3u)==1u?2:1;
        if(follow.bank.records[follow.current100].id00==lastId){
            result.operation=O::selectAttached5;result.sourcePC=(lastId==2)?0x17733cu:0x1773e0u;
        }else{
            result.operation=O::cycleFollowRecord;result.sourcePC=(lastId==2)?0x177358u:0x1773fcu;
        }
    }
    result.status=S::readyForNumericChange;return result;
}
//Read-only numeric inspection retains an explicit boundary for callers that
//have not supplied the genuine temporary-look owners.
inline OriginalCameraModeInputResult inspectOriginalCameraCyclePress176DF0(
    OriginalRaceCameraOwner& controller,const OriginalFollowCamera& follow,
    const OriginalCameraModeInputEnvironment& env) noexcept {
    return inspectOriginalCameraCyclePress176DF0(controller,follow,env,
        [](std::int32_t,std::uint8_t,OriginalCameraModeInputResult&) noexcept {return false;});
}
//17BDA0->2843E0->285580: cycle by ordered record INDEX, not incrementing ID.
//There is no offset save/restore. Exactly one record is selected even if its
//CycleThruThisPreset is0; with >1 records and no eligible record the original
//loop never terminates. Report that domain instead of inventing a selection.
inline OriginalCameraModeInputStatus cycleOriginalFollowPreset17BDA0(
    OriginalFollowCamera& follow,const OriginalRaceCameraOwner& controller) noexcept {
    using S=OriginalCameraModeInputStatus;
    if(!original_camera_mode_input_detail::followBound(follow,controller))return S::owner;
    std::size_t selected=0;
    if(follow.bank.count!=1){
        bool found=false;auto i=follow.current100;
        for(std::size_t visited=0;visited<follow.bank.count;++visited){
            if(++i==follow.bank.count)i=0;
            if(follow.bank.records[i].cycle24!=0){selected=i;found=true;break;}
        }
        if(!found)return S::authoringWouldNotTerminate;
    }
    auto next=follow;
    next.scratch180={0,0,0,1};next.collision.localPoint1A0={0,0,0,1};
    const auto& preset=next.bank.records[selected];
    if(next.bank.records[next.current100].filter28==0&&preset.filter28==1)next.snap10C=true;
    next.current100=selected;
    next.angleY08=preset.angleY04;next.angleX0C=preset.angleX08;
    next.distance04=preset.distance0C;next.fov10=preset.fov10;
    next.cameraSwing14=preset.cameraSwing38;next.lookSwing18=preset.lookSwing3C;
    next.distanceOffset58=0;next.yawOffset5C=0;next.pitchOffset60=0;next.fovOffset64=0;
    //17BEC0 and17BEE0 both rebind the SAME racer340/racer194 already represented
    //by these stable native owners; there is no alternate/replacement subject.
    ++next.revision;follow=next;return S::needsCycleAudio1A4B90;
}
struct OriginalCameraModeInputOwners {
    OriginalRaceCameraOwner& controller;
    OriginalFollowCamera& follow;
    OriginalAttachedCamera& attached;
    OriginalFollowSceneBinding& scene;
    OriginalFollowChildRegistry& followChildren;
    OriginalAttachedChildRegistry& attachedChildren;
    ResetVisibilityWorld50EY& visibility;
    OriginalCameraSceneOwner& spatial;
    const OriginalFollowVisibilityZones& zones;
    LiveZoneWorkspace50EN& workspace;
};
//Actual numeric cut. Both target owners must already be source-constructed and
//registered. Mode changes execute the real scene-registration functions. A
//Follow->Follow preset cycle changes the true Follow owner without a scene cut.
//No blur timer/request, RNG call, driving input or default preference is created.
inline OriginalCameraModeInputResult advanceOriginalCameraCyclePress176DF0(
    OriginalCameraModeInputOwners owners,const OriginalCameraModeInputEnvironment& env,
    const OriginalFollowSceneTransitionInput& sceneInput) noexcept {
    using S=OriginalCameraModeInputStatus;using O=OriginalCameraCycleOperation;
    auto temporaryLook=[&](std::int32_t preset,std::uint8_t bit,
        OriginalCameraModeInputResult& result) noexcept {
        auto& c=owners.controller;auto& f=owners.follow;
        const auto oldMode=c.mode44;
        if(oldMode==0||oldMode==5){
            if(!original_camera_mode_input_detail::followBound(f,c)){
                result.status=S::owner;return false;
            }
            if(oldMode==5){
                result.transition=transitionOriginalFollow177AB0(c,f,owners.scene,
                    owners.followChildren,owners.visibility,owners.spatial,owners.zones,
                    owners.workspace,0,nullptr,sceneInput);
                if(result.transition.status!=OriginalFollowControllerStatus::complete){
                    result.status=S::transitionUnavailable;result.sourcePC=result.transition.sourcePC;return false;
                }
            }
            //2842F0 returns the OLD record INDEX, not its authored ID.
            const auto oldIndex=static_cast<std::uint16_t>(f.current100&31u);
            if(selectOriginalFollowPreset(f,preset,true,false).status!=OriginalFollowStatus::ok){
                result.status=S::owner;result.sourcePC=0x17bd00u;return false;
            }
            c.temporaryLook60=static_cast<std::uint16_t>((c.temporaryLook60&~0xffcu)|
                (oldIndex<<2)|(static_cast<std::uint16_t>(oldMode&31)<<7));
        }
        c.flags62=static_cast<std::uint8_t>(c.flags62|bit);
        return true;
    };
    auto result=inspectOriginalCameraCyclePress176DF0(owners.controller,owners.follow,env,temporaryLook);
    if(result.status!=S::readyForNumericChange)return result;
    if(result.operation==O::cycleFollowRecord){
        result.status=cycleOriginalFollowPreset17BDA0(owners.follow,owners.controller);
        result.sourcePC=result.status==S::needsCycleAudio1A4B90?0x177454u:0x284460u;
        return result;
    }
    if(result.operation==O::selectAttached5){
        result.transition=transitionOriginalAttached177AB0(owners.controller,owners.attached,
            owners.scene,owners.attachedChildren,owners.visibility,owners.spatial,
            owners.zones,owners.workspace,5,sceneInput);
    }else if(result.operation==O::selectFollow0ThenPresetId0){
        //Source177384 first activates mode0 WITHOUT a preset argument; the
        //separate virtual17BD00(0,0,0) runs only after that complete transition.
        result.transition=transitionOriginalFollow177AB0(owners.controller,owners.follow,
            owners.scene,owners.followChildren,owners.visibility,owners.spatial,
            owners.zones,owners.workspace,0,nullptr,sceneInput);
    }else{result.status=S::owner;return result;}
    if(result.transition.status!=OriginalFollowControllerStatus::complete){
        result.status=S::transitionUnavailable;result.sourcePC=result.transition.sourcePC;return result;
    }
    if(result.operation==O::selectFollow0ThenPresetId0&&
       selectOriginalFollowPreset(owners.follow,0,false,false).status!=OriginalFollowStatus::ok){
        result.status=S::owner;result.sourcePC=0x1773a0u;return result;
    }
    result.status=S::needsCycleAudio1A4B90;result.sourcePC=0x177454u;return result;
}
//1774A0 release queries CURRENT held actions, not the event descriptor.
//An unrelated release must not restore a look still held. Simultaneous looks
//keep6A8 priority on press and restore only the corresponding active flag.
inline OriginalCameraModeInputResult advanceOriginalCameraLookRelease1774A0(
    OriginalCameraModeInputOwners o,const OriginalCameraModeInputEnvironment& env,
    const OriginalFollowSceneTransitionInput& in) noexcept {
    using S=OriginalCameraModeInputStatus;OriginalCameraModeInputResult r;
    if(!o.controller.bound)return r;
    const auto slot=env.racerClass58==0?original_camera_mode_input_detail::signedView(env.cameraViewA8):0;
    if(slot<0||slot>=8){r.status=S::inputSlot;r.sourcePC=0x1774d4u;return r;}
    r.inputSlot=static_cast<std::uint32_t>(slot);
    r.status=S::needsChildRoute142350;r.sourcePC=0x1776ecu; //Release child is1422A0.
    if((env.raceFlags29&4u)||(env.raceStateFlags58&1u))return r;
    if(!env.raceViewState44||r.inputSlot>=env.raceViewStateCount){
        r.status=S::inputSlot;r.sourcePC=0x17751cu;return r;
    }
    if(env.raceViewState44[r.inputSlot]||!(env.raceStateFlags58&6u)||
       o.controller.mode44==2||o.controller.mode44==4||o.controller.mode44==7)return r;
    if(r.inputSlot>=controlPresetSlots49D){r.status=S::inputSlot;r.sourcePC=0x1775a0u;return r;}
    const auto& a=env.preferences.slots[r.inputSlot].actions;
    const std::int32_t codes[]={a.action6A8,a.action6B0};
    const std::uint8_t bits[]={0x40u,0x20u};
    for(unsigned i=0;i<2;++i){
        const auto q=queryManagerHeld49A(env.held,codes[i],2,0x3fu,r.inputSlot);
        r.queryStatus=q.status;
        if(q.status!=ManagerHeldStatus49A::Complete){r.status=S::queryUnavailable;r.sourcePC=i?0x17765cu:0x1775acu;return r;}
        ++r.queriesCompleted;
        if(!q.matched&&(o.controller.flags62&bits[i])){
            r.transition=restoreOriginalTemporaryLook1775D0(o.controller,o.follow,o.attached,o.scene,
                o.attachedChildren,o.visibility,o.spatial,o.zones,o.workspace,in,bits[i]);
            if(r.transition.status!=OriginalFollowControllerStatus::complete){
                r.status=S::transitionUnavailable;r.sourcePC=r.transition.sourcePC;return r;
            }
        }
    }
    return r;
}
//Closed178670 leaf. Invoke in source order AFTER actual1A4B90(race10C,4)
//returns, then continue actual142350. This function does not certify or supply
//that missing audio call, child routing, or persistence to disk.
//1A4B90 calls292360(race10C->160,4,1,0); exact sound owner remains an external
//join. 178670 writes BOTH mode and current Follow ID, even while Attached5.
inline bool saveOriginalCameraModePreference178670(OriginalRaceCameraOwner& controller,
    OriginalFollowCamera& follow,ControlPresets49D& preferences,std::uint8_t cameraViewA8) noexcept {
    if(!original_camera_mode_input_detail::followBound(follow,controller))return false;
    auto slot=original_camera_mode_input_detail::signedView(cameraViewA8);
    if(slot>=2)slot=0;
    if(slot<0)return false; //Source negative-index memory is outside typed domain.
    auto& preference=preferences.slots[static_cast<std::uint32_t>(slot)];
    preference.field11030=static_cast<std::uint32_t>(controller.mode44);
    preference.field11038=static_cast<std::uint32_t>(follow.bank.records[follow.current100].id00);
    //284550 resets LIVE offsets only, preserving savedOffsets68.
    follow.distanceOffset58=0;follow.yawOffset5C=0;follow.pitchOffset60=0;follow.fovOffset64=0;
    ++follow.revision;return true;
}
}
