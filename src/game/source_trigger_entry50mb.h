#pragma once
// Explicitly fallible source-action continuation. This does not change the
// legacy infallible50IN route, invent a geometric hit, or authorize a reset.
#include "native_trigger_group50ga.h"
#include <cstdint>
#include <limits>

namespace rr::game {
struct SourceTriggerActionCall50MB {
    TriggerActionCall50IN source{};
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0;
};
enum class SourceTriggerActionStatus50MB : std::uint8_t {Complete,Blocked,InvalidBinding};
struct SourceTriggerActionResult50MB {
    SourceTriggerActionStatus50MB status=SourceTriggerActionStatus50MB::InvalidBinding;
    std::uint32_t sourcePC=0,ownerOffset=0;
};
struct SourceTriggerActionHooks50MB {
    void* context=nullptr;
    // Pure structural admission, independent of earlier action effects.
    bool (*preflight)(void*,const SourceTriggerActionCall50MB&) noexcept=nullptr;
    // Performs/resumes this actual action. It may mutate only its separately
    // admitted physical owners, never group/hook/contact-membership storage.
    SourceTriggerActionResult50MB (*resume)(void*,const SourceTriggerActionCall50MB&) noexcept=nullptr;
};
enum class SourceTriggerContactPhase50MB : std::uint8_t {
    Idle,Actions,AddTriggerMember,AddOtherMember,Complete
};
struct SourceTriggerContactState50MB {
    SourceTriggerContactPhase50MB phase=SourceTriggerContactPhase50MB::Idle;
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0,triggerIndex=0,colliderIndex=0;
    std::size_t nextAction=0,actionCount=0;
    std::uint32_t triggerCollider=0,otherCollider=0;
    const NativeTriggerGroup50GA* group=nullptr;
    const SourceTriggerActionHooks50MB* hooks=nullptr;
    bool dispatchCounted=false;
};
enum class SourceTriggerContactStatus50MB : std::uint8_t {
    Complete,Input,Capacity,ActionInput,ActionBlocked,OwnerChanged
};
struct SourceTriggerContactReceipt50MB {
    SourceTriggerContactStatus50MB status=SourceTriggerContactStatus50MB::Input;
    std::size_t callbacks=0,addedPairs=0,removedPairs=0;
    bool serviceRan=false;
    SourceTriggerActionResult50MB action{};
};
// A course TOB that authors no triggers (absent [Triggers] or 0) has no
// trigger state selected: every slot stays cold-empty (no identity, callback
// or contact). The group still carries the racers' real colliders, so the
// source query owner stays bound and queries run; nothing can dispatch.
inline bool emptySourceTriggerSet50MB(const NativeTriggerGroup50GA& group) noexcept {
    for(const auto& s:group.triggers)
        if(s.collider.identity||s.collider.highWaterD0||s.callbackCount98)return false;
    return true;
}
inline bool validEmptySourceTriggerStorage50MB(const NativeTriggerGroup50GA& group) noexcept {
    if(!group.ready||group.actionsFailed||!emptySourceTriggerSet50MB(group))return false;
    for(std::size_t i=0;i<group.colliders.size();++i){
        if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
        for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
    }
    return true;
}
inline bool validSourceTriggerGroup50MB(const NativeTriggerGroup50GA& group,
    const SourceTriggerActionHooks50MB& hooks) noexcept {
    // No fake legacy callbacks, mixed modes, random-input stand-ins or missing
    // source provider. Authored MB has actual percent100, as checked by50GA.
    return hooks.context&&hooks.preflight&&hooks.resume&&!group.actions.context&&
        !group.actions.preflight&&!group.actions.apply&&
        (validNativeTriggerStorage50GA(group)||validEmptySourceTriggerStorage50MB(group));
}
// Discard only a completed observation before a different candidate. Never
// discard an unfinished action or either reciprocal-add prefix.
inline bool releaseCompletedSourceContact50MB(SourceTriggerContactState50MB& state) noexcept {
    if(state.phase!=SourceTriggerContactPhase50MB::Idle&&
       state.phase!=SourceTriggerContactPhase50MB::Complete)return false;
    state={};return true;
}
inline SourceTriggerContactReceipt50MB resumeNativeSourceTriggerContact50MB(
    NativeTriggerGroup50GA& group,std::size_t trigger,std::uint32_t collider,
    const SourceTriggerActionHooks50MB& hooks,SourceTriggerContactState50MB& state,
    std::uint64_t querySerial,std::size_t candidateOrdinal) noexcept {
    namespace d=trigger_entry50IN_detail;
    using P=SourceTriggerContactPhase50MB;using S=SourceTriggerContactStatus50MB;
    SourceTriggerContactReceipt50MB r;
    if(!querySerial||!validSourceTriggerGroup50MB(group,hooks)||trigger>=group.triggers.size())return r;
    const auto ci=nativeTriggerCollider50GA(group,collider);
    if(ci==group.colliders.size())return r;
    auto& s=group.triggers[trigger];auto& other=group.colliders[ci];
    const auto call=[&](std::size_t action) noexcept {
        return SourceTriggerActionCall50MB{{s.collider.identity,collider,action,true,0x2a0eccu},
            querySerial,candidateOrdinal};
    };
    if(state.phase!=P::Idle){
        if(state.group!=&group||state.hooks!=&hooks||state.querySerial!=querySerial||
           state.candidateOrdinal!=candidateOrdinal||state.triggerIndex!=trigger||
           state.colliderIndex!=ci||state.triggerCollider!=s.collider.identity||
           state.otherCollider!=collider||state.actionCount!=s.callbackCount98||
           state.nextAction>state.actionCount){r.status=S::OwnerChanged;return r;}
        if(state.phase==P::Complete){r.status=S::Complete;return r;}
    }else{
        if(!nativeTriggerPairAllowed50GA(group,trigger,collider)){
            r.status=S::Complete;return r;
        }
        // Existing50GA native prefilter suppresses sustained pairs before
        // touched/chance. Keep a second explicit fresh gate before actions.
        const bool fresh=d::find(s.collider,collider)==s.collider.highWaterD0;
        if(!fresh){r.status=S::Complete;return r;}
        if(group.contacts!=std::numeric_limits<std::uint64_t>::max())++group.contacts;
        const auto touched=static_cast<std::uint8_t>(s.flags1B8|0x10u);
        if((touched&4u)&&(touched&8u)){
            s.flags1B8=touched;r.status=S::Complete;return r;
        }
        // Source MB only: randomPercent100 is genuine authored admission. Do
        // not consume RNG or pretend unsupported percentages are satisfied.
        if(s.randomTriggerPercent90!=100)return r;
        if(!d::room(s.collider,collider)||!d::room(other,s.collider.identity)){
            r.status=S::Capacity;return r;
        }
        const auto actions=(touched&2u)?s.callbackCount98:0u;
        for(std::size_t i=0;i<actions;++i)if(!hooks.preflight(hooks.context,call(i))){
            r.status=S::ActionInput;return r;
        }
        state.group=&group;state.hooks=&hooks;state.querySerial=querySerial;
        state.candidateOrdinal=candidateOrdinal;state.triggerIndex=trigger;state.colliderIndex=ci;
        state.triggerCollider=s.collider.identity;state.otherCollider=collider;
        state.actionCount=s.callbackCount98;state.nextAction=actions?0u:s.callbackCount98;
        state.phase=P::Actions;
        s.flags1B8=static_cast<std::uint8_t>(touched|8u);s.lastCollider1B4=collider;
    }
    while(state.phase==P::Actions&&state.nextAction<state.actionCount){
        r.action=hooks.resume(hooks.context,call(state.nextAction));
        if(r.action.status!=SourceTriggerActionStatus50MB::Complete){
            r.status=S::ActionBlocked;return r;
        }
        ++state.nextAction;++r.callbacks;
    }
    if(state.phase==P::Actions)state.phase=P::AddTriggerMember;
    // The admitted action contract excludes mutation of these lists. Check
    // before the first reciprocal write; never repair an unexpected mutation.
    if(state.phase==P::AddTriggerMember){
        if(!d::valid(s.collider)||!d::valid(other)||
           d::find(s.collider,collider)!=s.collider.highWaterD0||
           d::find(other,s.collider.identity)!=other.highWaterD0||
           !d::room(s.collider,collider)||!d::room(other,s.collider.identity)){
            r.status=S::OwnerChanged;return r;
        }
        d::add(s.collider,collider);state.phase=P::AddOtherMember;
    }
    if(state.phase==P::AddOtherMember){
        // No fallible operation is inserted between the two source additions.
        d::add(other,s.collider.identity);state.phase=P::Complete;r.addedPairs=1;
        if((s.flags1B8&2u)&&state.actionCount&&!state.dispatchCounted){
            if(group.dispatches!=std::numeric_limits<std::uint64_t>::max())++group.dispatches;
            state.dispatchCounted=true;
        }
    }
    r.status=S::Complete;return r;
}

// MB LostAI exits are source no-ops. This retains the ordinary2A0B30 removal
// chronology while propagating a provider failure before any later exit. A
// failure is terminal for this service call; do not replay removed-slot work.
inline SourceTriggerContactReceipt50MB serviceNativeSourceTrigger50MB(
    NativeTriggerGroup50GA& group,std::size_t trigger,const TriggerOverlap50IN* observations,
    std::size_t count,const SourceTriggerActionHooks50MB& hooks,
    const SourceTriggerContactState50MB& pending,bool eligible=true) noexcept {
    namespace d=trigger_entry50IN_detail;using S=SourceTriggerContactStatus50MB;
    SourceTriggerContactReceipt50MB r;
    if(!validSourceTriggerGroup50MB(group,hooks)||trigger>=group.triggers.size()||
       (pending.phase!=SourceTriggerContactPhase50MB::Idle&&
        pending.phase!=SourceTriggerContactPhase50MB::Complete))return r;
    auto& s=group.triggers[trigger];
    if(!eligible||!(s.flags29&1u)||(s.flags29&0xcu)){r.status=S::Complete;return r;}
    if(count!=s.collider.highWaterD0||(count&&!observations))return r;
    const auto call=[&](std::uint32_t other,std::size_t action,std::size_t slot) noexcept {
        return SourceTriggerActionCall50MB{{s.collider.identity,other,action,false,0x2a0d20u},0,slot};
    };
    for(std::size_t i=0;i<count;++i){
        const auto id=s.collider.membersCC[i];const auto* other=observations[i].collider;
        if(!id){if(other)return r;continue;}
        if(!other||other==&s.collider||!d::valid(*other)||other->identity!=id||
           nativeTriggerCollider50GA(group,id)==group.colliders.size()||
           other!=&group.colliders[nativeTriggerCollider50GA(group,id)])return r;
        if(!observations[i].overlap&&(s.flags1B8&1u))
            for(std::size_t a=0;a<s.callbackCount98;++a)
                if(!hooks.preflight(hooks.context,call(id,a,i))){r.status=S::ActionInput;return r;}
    }
    s.flags1B8=static_cast<std::uint8_t>((s.flags1B8&~0x30u)|((s.flags1B8&0x10u)<<1));
    r.serviceRan=true;
    for(std::size_t i=0;i<count;++i){
        auto* other=observations[i].collider;if(!other||observations[i].overlap)continue;
        const auto id=other->identity;
        d::remove(*other,s.collider.identity);d::remove(s.collider,id);s.collider.membersCC[i]=0;
        ++r.removedPairs;
        if(s.flags1B8&1u)for(std::size_t a=0;a<s.callbackCount98;++a){
            r.action=hooks.resume(hooks.context,call(id,a,i));
            if(r.action.status!=SourceTriggerActionStatus50MB::Complete){r.status=S::ActionBlocked;return r;}
            ++r.callbacks;
        }
    }
    r.status=S::Complete;return r;
}
} // namespace rr::game
