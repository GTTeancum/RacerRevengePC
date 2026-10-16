#pragma once
#include "original_attached_camera_controller.h"
#include "control_presets49d.h"
#include <cstring>

namespace rr::game {
enum class OriginalEndingCameraStatus50KG : std::uint8_t {
    needsSelectedPose115CB0,owner,sourcePath,numeric,transition,input
};
struct OriginalEndingCameraResult50KG {
    OriginalEndingCameraStatus50KG status=OriginalEndingCameraStatus50KG::owner;
    std::uint32_t sourcePC=0,randomDraws=0;
    bool prefixCommitted=false,transitionCommitted=false,inputCommitted=false;
    OriginalFollowTransitionResult transition{};
    OriginalFollowResult input{};
};
struct OriginalEndingCameraOwners50KG {
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
    CoupledNoiseContext49M& random;
    GameplayBlurPending& blur;
    const ManagerHeldSources49A& held;
};
// Positive-time ending branch of175D70, including the SAME common prefix and
// real177AB0/179DD0 owners. This is not a pose and never feeds racer controls.
// Call instead of ordinary stable control, only at the original camera child
// cut. After success invoke the actual newly selected Follow/Attached pose,
// then normal scene tick/matrices/visibility exactly once (115CB0).
// Original prefix/RNG/transition effects remain live on a later rejection;
// terminate that frame instead of replaying it. No synthetic finish view,
// private RNG, preset replacement, debug-query result or spatial receipt.
inline OriginalEndingCameraResult50KG advanceOriginalEndingRaceCamera175D70(
    OriginalEndingCameraOwners50KG o,const StableRaceCameraFrame& frame,
    const OriginalFollowSceneTransitionInput& sceneInput) noexcept {
    using S=OriginalEndingCameraStatus50KG;
    namespace d=original_race_camera_detail;
    OriginalEndingCameraResult50KG r;
    auto& c=o.controller;auto& f=o.follow;auto& a=o.attached;
    const auto& binding=c.binding;
    if(!c.bound||!f.prepared||!a.prepared||
       binding.sessionGeneration!=frame.sessionGeneration||binding.racerKey!=frame.racerKey||
       binding.mainBodyKey!=frame.mainBodyKey||f.sessionGeneration!=binding.sessionGeneration||
       f.racerKey!=binding.racerKey||f.mainBodyKey!=binding.mainBodyKey||
       a.binding.sessionGeneration!=binding.sessionGeneration||a.binding.racerKey!=binding.racerKey||
       a.binding.mainBodyKey!=binding.mainBodyKey||o.scene.sceneKey!=binding.sceneKey)return r;
    if((c.mode44!=0&&c.mode44!=5)||!(frame.raceStateFlags58&1u)||
       (frame.raceFlags1B0&0x10u)||frame.racerCD4!=0){
        r.status=S::sourcePath;r.sourcePC=0x17601cu;return r;
    }
    if(std::fegetround()!=FE_TONEAREST||!d::scalar(frame.elapsed)||frame.elapsed<0.f||
       !d::scalar(frame.raceTime94)||frame.raceTime94<=0.f||
       !d::scalar(c.cooldown8C)||!d::scalar(c.cutRemaining90)||!d::scalar(c.specialDelayA4)){
        r.status=S::numeric;r.sourcePC=0x176058u;return r;
    }
    if(!o.followChildren.collection||!o.followChildren.collection->bound||
       o.followChildren.follow64!=&f||!o.followChildren.follow64Key||
       o.attachedChildren.collection!=o.followChildren.collection||
       o.attachedChildren.attached6C!=&a||!o.attachedChildren.attached6CKey)return r;
    const auto& children=*o.followChildren.collection;
    if(children.sessionGeneration!=binding.sessionGeneration||children.sceneKey!=binding.sceneKey||
       !children.controllerKey||!children.registered38||!children.count30||children.count30>64||
       children.selected34>=children.count30||
       (c.mode44==0&&(!f.active||children.registered38[children.selected34]!=o.followChildren.follow64Key))||
       (c.mode44==5&&(!a.active||children.registered38[children.selected34]!=o.attachedChildren.attached6CKey)))return r;
    OriginalCameraShakeChannels shake{f.shakeScale174,f.shakeAmplitude170,
        a.shakeScale2C,a.shakeAmplitude28};
    auto next=c;auto blur=o.blur;bool attemptedBlur=false;std::int32_t strength=0;
    if(!d::advanceCommonPrefix175D70(next,shake,blur,frame.elapsed,attemptedBlur,strength)){
        r.status=S::numeric;r.sourcePC=0x175d90u;return r;
    }
    //175F50..17601C writes only for source states0/1/2; other states retain
    //these bits before the same independent manager-end branch is evaluated.
    if(frame.racerState40>=0&&frame.racerState40<=2){
        const std::uint8_t bits=frame.racerState40==1?8u:frame.racerState40==2?16u:0u;
        next.flags63Low5=static_cast<std::uint8_t>((next.flags63Low5&~0x18u)|bits);
    }
    c=next;o.blur=blur;
    f.shakeScale174=shake.followMagnitude174;f.shakeAmplitude170=shake.followFactor170;
    a.shakeScale2C=shake.attachedMagnitude2C;a.shakeAmplitude28=shake.attachedFactor28;
    r.prefixCommitted=true;
    if(c.specialDelayA4>0.f){
        const float remaining=d::sub(c.specialDelayA4,frame.elapsed);
        if(!d::scalar(remaining)){r.status=S::numeric;r.sourcePC=0x176224u;return r;}
        c.specialDelayA4=remaining; //Crossing zero does not orbit until NEXT update.
    }else{
        if(!(c.flags63Low5&2u)){
            //CVT.S.W applies to the positive31-bit raw draw, then separate
            //binary32 multiplication by0x30000000 (1/2^31). Preserve the
            //different <= and < comparisons and exactly two shared draws.
            volatile float first=static_cast<float>(drawOriginalSharedRng50IU(o.random));
            ++r.randomDraws;
            const float firstUnit=d::mul(4.656612873077392578125e-10f,first);
            const std::int32_t preset=firstUnit<=.5f?1:0;
            r.transition=transitionOriginalFollow177AB0(c,f,o.scene,o.followChildren,
                o.visibility,o.spatial,o.zones,o.workspace,0,&preset,sceneInput);
            if(r.transition.status!=OriginalFollowControllerStatus::complete){
                r.status=S::transition;r.sourcePC=r.transition.sourcePC;return r;
            }
            r.transitionCommitted=true;
            volatile float second=static_cast<float>(drawOriginalSharedRng50IU(o.random));
            ++r.randomDraws;
            const bool direction=d::mul(4.656612873077392578125e-10f,second)<.5f;
            c.flags63Low5=static_cast<std::uint8_t>((c.flags63Low5&~4u)|(direction?4u:0u));
            const std::uint32_t word=direction?0xc01c61a9u:0xc075be09u;
            std::memcpy(&f.yawOffset5C,&word,sizeof word);++f.revision;
            c.cutRemaining90=2.25f; //Exact0x40100000, independently of prefix blur.
            requestGameplayBlur(o.blur,100,0,0); //First request wins, as2AED10.
            c.flags63Low5=static_cast<std::uint8_t>(c.flags63Low5|2u);
        }
        //1761C0 clears the real input record;176210 dispatches Follow vtable10.
        OriginalFollowInput input;
        input.axis20=(c.flags63Low5&4u)?-.65f:.65f;
        if(f.debugCountdown168<=0){
            const auto q=queryManagerHeld49A(o.held,4,0,0xcu,0);
            if(q.status!=ManagerHeldStatus49A::Complete){
                r.status=S::input;r.sourcePC=0x179e10u;return r;
            }
            input.debugQuery4_0_12=q.matched;
        }
        r.input=applyOriginalFollowInput(f,input);
        if(r.input.status!=OriginalFollowStatus::ok){
            r.status=S::input;r.sourcePC=r.input.sourcePC;return r;
        }
        r.inputCommitted=true;
    }
    //All positive ending branches skip normal shake-tail176CCC and arrive
    //at176D9C. Only the selected-camera pose/scene dispatch remains to run.
    c.flags62=static_cast<std::uint8_t>(c.flags62&~0x80u);
    r.status=S::needsSelectedPose115CB0;r.sourcePC=0x176da8u;return r;
}
} // namespace rr::game
