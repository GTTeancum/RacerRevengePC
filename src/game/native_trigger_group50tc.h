#pragma once
#include "trigger_entry50in.h"
#include <array>
#include <cstdint>
#include <limits>
namespace rr::game {
// Distinct exact TC storage; legacy GA13/callback2 bounds are untouched.
inline constexpr std::size_t nativeTriggerCount50TC=22,nativeTriggerColliderCount50TC=16;
struct NativeTriggerGroup50TC;
struct SourceTriggerActionCall50TC {
    TriggerActionCall50IN source{};
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0;
};
enum class SourceTriggerActionStatus50TC : std::uint8_t {Complete,Blocked,InvalidBinding};
struct SourceTriggerActionResult50TC {
    SourceTriggerActionStatus50TC status=SourceTriggerActionStatus50TC::InvalidBinding;
    std::uint32_t sourcePC=0,ownerOffset=0;
};
struct SourceTriggerActionHooks50TC {
    void* context=nullptr;
    // Pure structural admission, independent of earlier action effects.
    bool (*preflight)(void*,const SourceTriggerActionCall50TC&) noexcept=nullptr;
    // Performs this actual action. It may mutate only its separately
    // admitted C80, animation or Sound owners, never group/hook/contact-membership storage.
    SourceTriggerActionResult50TC (*resume)(void*,const SourceTriggerActionCall50TC&) noexcept=nullptr;
};
enum class SourceTriggerContactPhase50TC : std::uint8_t {
    Idle,Actions,AddTriggerMember,AddOtherMember,Complete
};
struct SourceTriggerContactState50TC {
    SourceTriggerContactPhase50TC phase=SourceTriggerContactPhase50TC::Idle;
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0,triggerIndex=0,colliderIndex=0;
    std::size_t nextAction=0,actionCount=0;
    std::uint32_t triggerCollider=0,otherCollider=0;
    const NativeTriggerGroup50TC* group=nullptr;
    const SourceTriggerActionHooks50TC* hooks=nullptr;
    bool dispatchCounted=false;
};
enum class SourceTriggerContactStatus50TC : std::uint8_t {
    Complete,Input,Capacity,ActionInput,ActionBlocked,OwnerChanged
};
struct SourceTriggerContactReceipt50TC {
    SourceTriggerContactStatus50TC status=SourceTriggerContactStatus50TC::Input;
    std::size_t callbacks=0,addedPairs=0,removedPairs=0;
    bool serviceRan=false;
    SourceTriggerActionResult50TC action{};
};
struct NativeTriggerGroup50TC {
 std::array<TriggerEntryState50IN,nativeTriggerCount50TC> triggers{};
 std::array<TriggerColliderContacts50IN,nativeTriggerColliderCount50TC> colliders{};
 std::array<std::uint32_t,nativeTriggerCount50TC> ignoreCollider{};
 std::array<std::array<std::uint32_t,8>,nativeTriggerCount50TC> ignoreColliders{};
 std::array<std::size_t,nativeTriggerCount50TC> ignoreColliderCount{};
 SourceTriggerActionHooks50TC actions{};
 SourceTriggerContactState50TC contact{};
 SourceTriggerContactReceipt50TC lastContact{};
 std::uint64_t querySerial=0;
 std::uint32_t queryCollider=0;
 bool ready=false,actionsFailed=false,queryActive=false;
 std::uint64_t contacts=0,dispatches=0;
};
inline std::size_t nativeTriggerCollider50TC(const NativeTriggerGroup50TC& group,std::uint32_t id)noexcept {
 for(std::size_t i=0;i<group.colliders.size();++i)if(group.colliders[i].identity==id)return i;
 return group.colliders.size();
}
// Structural admission is shared by the distinct fallible source-action route.
// A source-only group is never admitted by the legacy action dispatcher.
inline bool validNativeTriggerStorage50TC(const NativeTriggerGroup50TC& group)noexcept {
 if(!group.ready||group.actionsFailed)return false;
 for(std::size_t i=0;i<group.colliders.size();++i){
  if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
  for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
 }
 for(std::size_t i=0;i<group.triggers.size();++i){const auto& state=group.triggers[i];
  if(!trigger_entry50IN_detail::valid(state)||state.randomTriggerPercent90!=100||state.callbackCount98!=(i==10?3u:1u))return false;
  if(nativeTriggerCollider50TC(group,state.collider.identity)!=group.colliders.size())return false;
  for(std::size_t j=0;j<i;++j)if(state.collider.identity==group.triggers[j].collider.identity)return false;
  if(group.ignoreCollider[i]&&nativeTriggerCollider50TC(group,group.ignoreCollider[i])==group.colliders.size())return false;
  if(group.ignoreColliderCount[i]>group.ignoreColliders[i].size())return false;
  for(std::size_t j=0;j<group.ignoreColliderCount[i];++j){
   const auto ignored=group.ignoreColliders[i][j];
   if(!ignored||nativeTriggerCollider50TC(group,ignored)==group.colliders.size())return false;
   for(std::size_t k=0;k<j;++k)if(group.ignoreColliders[i][k]==ignored)return false;
  }
  for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
   const auto id=state.collider.membersCC[slot];if(!id)continue;
   const auto c=nativeTriggerCollider50TC(group,id);
   if(c==group.colliders.size()||trigger_entry50IN_detail::find(group.colliders[c],state.collider.identity)==group.colliders[c].highWaterD0)return false;
  }
 }
 for(const auto& collider:group.colliders)for(std::size_t slot=0;slot<collider.highWaterD0;++slot){
  const auto id=collider.membersCC[slot];if(!id)continue;
  const TriggerEntryState50IN* matching=nullptr;
  for(const auto& state:group.triggers)if(state.collider.identity==id)matching=&state;
  if(!matching||trigger_entry50IN_detail::find(matching->collider,collider.identity)==matching->collider.highWaterD0)return false;
 }
 return true;
}
inline bool validSourceTriggerGroup50TC(const NativeTriggerGroup50TC& group,
 const SourceTriggerActionHooks50TC& hooks) noexcept {
 return &hooks==&group.actions&&hooks.context&&hooks.preflight&&hooks.resume&&validNativeTriggerStorage50TC(group);
}
inline bool validNativeTriggerGroup50TC(const NativeTriggerGroup50TC& group) noexcept {
 return validSourceTriggerGroup50TC(group,group.actions);
}
inline bool nativeTriggerQueryBound50TC(const NativeTriggerGroup50TC& group,std::uint32_t collider)noexcept {
 return validNativeTriggerGroup50TC(group)&&group.queryActive&&group.querySerial&&group.queryCollider==collider&&
  nativeTriggerCollider50TC(group,collider)<group.colliders.size()&&
  (group.contact.phase==SourceTriggerContactPhase50TC::Idle||group.contact.phase==SourceTriggerContactPhase50TC::Complete);
}
inline bool nativeTriggerPairAllowed50TC(const NativeTriggerGroup50TC& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 if(trigger>=group.triggers.size()||!group.ready||group.actionsFailed||
    nativeTriggerCollider50TC(group,collider)==group.colliders.size()||group.ignoreCollider[trigger]==collider)return false;
 if(group.ignoreColliderCount[trigger]>group.ignoreColliders[trigger].size())return false;
 for(std::size_t i=0;i<group.ignoreColliderCount[trigger];++i)if(group.ignoreColliders[trigger][i]==collider)return false;
 const auto& state=group.triggers[trigger];
 if(!(state.flags29&1u))return false;
 return trigger_entry50IN_detail::find(state.collider,collider)==state.collider.highWaterD0;
}
// Discard only a completed observation before a different candidate. Never
// discard an unfinished action or either reciprocal-add prefix.
inline bool releaseCompletedSourceContact50TC(SourceTriggerContactState50TC& state) noexcept {
    if(state.phase!=SourceTriggerContactPhase50TC::Idle&&
       state.phase!=SourceTriggerContactPhase50TC::Complete)return false;
    state={};return true;
}
inline SourceTriggerContactReceipt50TC resumeNativeSourceTriggerContact50TC(
    NativeTriggerGroup50TC& group,std::size_t trigger,std::uint32_t collider,
    const SourceTriggerActionHooks50TC& hooks,SourceTriggerContactState50TC& state,
    std::uint64_t querySerial,std::size_t candidateOrdinal) noexcept {
    namespace d=trigger_entry50IN_detail;
    using P=SourceTriggerContactPhase50TC;using S=SourceTriggerContactStatus50TC;
    SourceTriggerContactReceipt50TC r;
    if(!querySerial||!validSourceTriggerGroup50TC(group,hooks)||trigger>=group.triggers.size())return r;
    const auto ci=nativeTriggerCollider50TC(group,collider);
    if(ci==group.colliders.size())return r;
    auto& s=group.triggers[trigger];auto& other=group.colliders[ci];
    const auto call=[&](std::size_t action) noexcept {
        return SourceTriggerActionCall50TC{{s.collider.identity,collider,action,true,0x2a0eccu},
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
        if(!nativeTriggerPairAllowed50TC(group,trigger,collider)){
            r.status=S::Complete;return r;
        }
        // Existing50TC native prefilter suppresses sustained pairs before
        // touched/chance. Keep a second explicit fresh gate before actions.
        const bool fresh=d::find(s.collider,collider)==s.collider.highWaterD0;
        if(!fresh){r.status=S::Complete;return r;}
        if(group.contacts!=std::numeric_limits<std::uint64_t>::max())++group.contacts;
        const auto touched=static_cast<std::uint8_t>(s.flags1B8|0x10u);
        if((touched&4u)&&(touched&8u)){
            s.flags1B8=touched;r.status=S::Complete;return r;
        }
        // Authentic TC only: randomPercent100 is genuine authored admission. Do
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
        if(r.action.status!=SourceTriggerActionStatus50TC::Complete){
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

// TC LostAI/Animation/Sound exits have no gameplay effects. This retains the ordinary2A0B30 removal
// chronology while propagating a provider failure before any later exit. A
// failure is terminal for this service call; do not replay removed-slot work.
inline SourceTriggerContactReceipt50TC serviceNativeSourceTrigger50TC(
    NativeTriggerGroup50TC& group,std::size_t trigger,const TriggerOverlap50IN* observations,
    std::size_t count,const SourceTriggerActionHooks50TC& hooks,
    const SourceTriggerContactState50TC& pending,bool eligible=true) noexcept {
    namespace d=trigger_entry50IN_detail;using S=SourceTriggerContactStatus50TC;
    SourceTriggerContactReceipt50TC r;
    if(!validSourceTriggerGroup50TC(group,hooks)||trigger>=group.triggers.size()||
       (pending.phase!=SourceTriggerContactPhase50TC::Idle&&
        pending.phase!=SourceTriggerContactPhase50TC::Complete))return r;
    auto& s=group.triggers[trigger];
    if(!eligible||!(s.flags29&1u)||(s.flags29&0xcu)){r.status=S::Complete;return r;}
    if(count!=s.collider.highWaterD0||(count&&!observations))return r;
    const auto call=[&](std::uint32_t other,std::size_t action,std::size_t slot) noexcept {
        return SourceTriggerActionCall50TC{{s.collider.identity,other,action,false,0x2a0d20u},0,slot};
    };
    for(std::size_t i=0;i<count;++i){
        const auto id=s.collider.membersCC[i];const auto* other=observations[i].collider;
        if(!id){if(other)return r;continue;}
        if(!other||other==&s.collider||!d::valid(*other)||other->identity!=id||
           nativeTriggerCollider50TC(group,id)==group.colliders.size()||
           other!=&group.colliders[nativeTriggerCollider50TC(group,id)])return r;
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
            if(r.action.status!=SourceTriggerActionStatus50TC::Complete){r.status=S::ActionBlocked;return r;}
            ++r.callbacks;
        }
    }
    r.status=S::Complete;return r;
}

// Called only at a genuine indexed E8. Failure is terminal: no automatic resume,
// no later candidate and no reciprocal additions after the failing action.
inline SourceTriggerContactReceipt50TC dispatchNativeTriggerContact50TC(
 NativeTriggerGroup50TC& group,std::size_t trigger,std::uint32_t collider,std::size_t candidateOrdinal)noexcept {
 SourceTriggerContactReceipt50TC r;
 if(!nativeTriggerQueryBound50TC(group,collider))return r;
 if(!releaseCompletedSourceContact50TC(group.contact))return r;
 r=resumeNativeSourceTriggerContact50TC(group,trigger,collider,group.actions,group.contact,group.querySerial,candidateOrdinal);
 group.lastContact=r;
 if(r.status!=SourceTriggerContactStatus50TC::Complete)group.actionsFailed=true;
 return r;
}
} // namespace rr::game
