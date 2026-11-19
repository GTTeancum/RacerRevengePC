#pragma once
#include "trigger_entry50in.h"
#include "native_trigger_capacity50tg.h"
#include <array>
#include <cstdint>
#include <limits>
namespace rr::game {
// TB trigger store: capacity-sized once from the common trigger reader
// (nativeTriggerCapacity50TG) with the authored live count triggerCount;
// the stock count is a receipt of the authoring owner, not a size here.
// Colliders: main+secondary of up to 8 racers. GA13/callback2 untouched.
inline constexpr std::size_t nativeTriggerColliderCount50TB=16;
struct NativeTriggerGroup50TB;
struct SourceTriggerActionCall50TB {
    TriggerActionCall50IN source{};
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0;
};
enum class SourceTriggerActionStatus50TB : std::uint8_t {Complete,Blocked,InvalidBinding};
struct SourceTriggerActionResult50TB {
    SourceTriggerActionStatus50TB status=SourceTriggerActionStatus50TB::InvalidBinding;
    std::uint32_t sourcePC=0,ownerOffset=0;
};
struct SourceTriggerActionHooks50TB {
    void* context=nullptr;
    // Pure structural admission, independent of earlier action effects.
    bool (*preflight)(void*,const SourceTriggerActionCall50TB&) noexcept=nullptr;
    // Performs this actual action. It may mutate only its separately
    // admitted C80, animation or PodSound owners, never group/hook/contact-membership storage.
    SourceTriggerActionResult50TB (*resume)(void*,const SourceTriggerActionCall50TB&) noexcept=nullptr;
};
enum class SourceTriggerContactPhase50TB : std::uint8_t {
    Idle,Actions,AddTriggerMember,AddOtherMember,Complete
};
struct SourceTriggerContactState50TB {
    SourceTriggerContactPhase50TB phase=SourceTriggerContactPhase50TB::Idle;
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0,triggerIndex=0,colliderIndex=0;
    std::size_t nextAction=0,actionCount=0;
    std::uint32_t triggerCollider=0,otherCollider=0;
    const NativeTriggerGroup50TB* group=nullptr;
    const SourceTriggerActionHooks50TB* hooks=nullptr;
    bool dispatchCounted=false;
};
enum class SourceTriggerContactStatus50TB : std::uint8_t {
    Complete,Input,Capacity,ActionInput,ActionBlocked,OwnerChanged
};
struct SourceTriggerContactReceipt50TB {
    SourceTriggerContactStatus50TB status=SourceTriggerContactStatus50TB::Input;
    std::size_t callbacks=0,addedPairs=0,removedPairs=0;
    bool serviceRan=false;
    SourceTriggerActionResult50TB action{};
};
struct NativeTriggerGroup50TB {
 std::array<TriggerEntryState50IN,nativeTriggerCapacity50TG> triggers{};
 std::size_t triggerCount=0; // live authored prefix [0,triggerCount); 0 = no trigger state
 TriggerColliderStorage50IN<nativeTriggerColliderCount50TB> colliders{};
 std::array<std::uint32_t,nativeTriggerCapacity50TG> ignoreCollider{};
 std::array<std::array<std::uint32_t,8>,nativeTriggerCapacity50TG> ignoreColliders{};
 std::array<std::size_t,nativeTriggerCapacity50TG> ignoreColliderCount{};
 SourceTriggerActionHooks50TB actions{};
 SourceTriggerContactState50TB contact{};
 SourceTriggerContactReceipt50TB lastContact{};
 std::uint64_t querySerial=0;
 std::uint32_t queryCollider=0;
 bool ready=false,actionsFailed=false,queryActive=false;
 std::uint64_t contacts=0,dispatches=0;
};
inline std::size_t nativeTriggerCollider50TB(const NativeTriggerGroup50TB& group,std::uint32_t id)noexcept {
 for(std::size_t i=0;i<group.colliders.size();++i)if(group.colliders[i].identity==id)return i;
 return group.colliders.size();
}
// Structural admission is shared by the distinct fallible source-action route.
// A source-only group is never admitted by the legacy action dispatcher.
inline bool validNativeTriggerStorage50TB(const NativeTriggerGroup50TB& group)noexcept {
 if(group.triggerCount>group.triggers.size())return false;
 if(!group.ready||group.actionsFailed)return false;
 for(std::size_t i=0;i<group.colliders.size();++i){
  if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
  for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
 }
 for(std::size_t i=0;i<group.triggerCount;++i){const auto& state=group.triggers[i];
  if(!trigger_entry50IN_detail::valid(state)||state.randomTriggerPercent90!=100||!state.callbackCount98||state.callbackCount98>nativeTriggerActionSlots50TG)return false;
  if(nativeTriggerCollider50TB(group,state.collider.identity)!=group.colliders.size())return false;
  for(std::size_t j=0;j<i;++j)if(state.collider.identity==group.triggers[j].collider.identity)return false;
  if(group.ignoreCollider[i]&&nativeTriggerCollider50TB(group,group.ignoreCollider[i])==group.colliders.size())return false;
  if(group.ignoreColliderCount[i]>group.ignoreColliders[i].size())return false;
  for(std::size_t j=0;j<group.ignoreColliderCount[i];++j){
   const auto ignored=group.ignoreColliders[i][j];
   if(!ignored||nativeTriggerCollider50TB(group,ignored)==group.colliders.size())return false;
   for(std::size_t k=0;k<j;++k)if(group.ignoreColliders[i][k]==ignored)return false;
  }
  for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
   const auto id=state.collider.membersCC[slot];if(!id)continue;
   const auto c=nativeTriggerCollider50TB(group,id);
   if(c==group.colliders.size()||trigger_entry50IN_detail::find(group.colliders[c],state.collider.identity)==group.colliders[c].highWaterD0)return false;
  }
 }
 for(const auto& collider:group.colliders)for(std::size_t slot=0;slot<collider.highWaterD0;++slot){
  const auto id=collider.membersCC[slot];if(!id)continue;
  const TriggerEntryState50IN* matching=nullptr;
  for(std::size_t t=0;t<group.triggerCount;++t)if(group.triggers[t].collider.identity==id)matching=&group.triggers[t];
  if(!matching||trigger_entry50IN_detail::find(matching->collider,collider.identity)==matching->collider.highWaterD0)return false;
 }
 return true;
}
inline bool validSourceTriggerGroup50TB(const NativeTriggerGroup50TB& group,
 const SourceTriggerActionHooks50TB& hooks) noexcept {
 return &hooks==&group.actions&&hooks.context&&hooks.preflight&&hooks.resume&&validNativeTriggerStorage50TB(group);
}
inline bool validNativeTriggerGroup50TB(const NativeTriggerGroup50TB& group) noexcept {
 return validSourceTriggerGroup50TB(group,group.actions);
}
inline bool nativeTriggerQueryBound50TB(const NativeTriggerGroup50TB& group,std::uint32_t collider)noexcept {
 return validNativeTriggerGroup50TB(group)&&group.queryActive&&group.querySerial&&group.queryCollider==collider&&
  nativeTriggerCollider50TB(group,collider)<group.colliders.size()&&
  (group.contact.phase==SourceTriggerContactPhase50TB::Idle||group.contact.phase==SourceTriggerContactPhase50TB::Complete);
}
inline bool nativeTriggerPairAllowed50TB(const NativeTriggerGroup50TB& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 if(trigger>=group.triggerCount||!group.ready||group.actionsFailed||
    nativeTriggerCollider50TB(group,collider)==group.colliders.size()||group.ignoreCollider[trigger]==collider)return false;
 if(group.ignoreColliderCount[trigger]>group.ignoreColliders[trigger].size())return false;
 for(std::size_t i=0;i<group.ignoreColliderCount[trigger];++i)if(group.ignoreColliders[trigger][i]==collider)return false;
 const auto& state=group.triggers[trigger];
 if(!(state.flags29&1u))return false;
 return trigger_entry50IN_detail::find(state.collider,collider)==state.collider.highWaterD0;
}
// Discard only a completed observation before a different candidate. Never
// discard an unfinished action or either reciprocal-add prefix.
inline bool releaseCompletedSourceContact50TB(SourceTriggerContactState50TB& state) noexcept {
    if(state.phase!=SourceTriggerContactPhase50TB::Idle&&
       state.phase!=SourceTriggerContactPhase50TB::Complete)return false;
    state={};return true;
}
inline SourceTriggerContactReceipt50TB resumeNativeSourceTriggerContact50TB(
    NativeTriggerGroup50TB& group,std::size_t trigger,std::uint32_t collider,
    const SourceTriggerActionHooks50TB& hooks,SourceTriggerContactState50TB& state,
    std::uint64_t querySerial,std::size_t candidateOrdinal) noexcept {
    namespace d=trigger_entry50IN_detail;
    using P=SourceTriggerContactPhase50TB;using S=SourceTriggerContactStatus50TB;
    SourceTriggerContactReceipt50TB r;
    if(!querySerial||!validSourceTriggerGroup50TB(group,hooks)||trigger>=group.triggerCount)return r;
    const auto ci=nativeTriggerCollider50TB(group,collider);
    if(ci==group.colliders.size())return r;
    auto& s=group.triggers[trigger];auto& other=group.colliders[ci];
    const auto call=[&](std::size_t action) noexcept {
        return SourceTriggerActionCall50TB{{s.collider.identity,collider,action,true,0x2a0eccu},
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
        if(!nativeTriggerPairAllowed50TB(group,trigger,collider)){
            r.status=S::Complete;return r;
        }
        // Existing50TB native prefilter suppresses sustained pairs before
        // touched/chance. Keep a second explicit fresh gate before actions.
        const bool fresh=d::find(s.collider,collider)==s.collider.highWaterD0;
        if(!fresh){r.status=S::Complete;return r;}
        if(group.contacts!=std::numeric_limits<std::uint64_t>::max())++group.contacts;
        const auto touched=static_cast<std::uint8_t>(s.flags1B8|0x10u);
        if((touched&4u)&&(touched&8u)){
            s.flags1B8=touched;r.status=S::Complete;return r;
        }
        // Authentic TB only: randomPercent100 is genuine authored admission. Do
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
        if(r.action.status!=SourceTriggerActionStatus50TB::Complete){
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

// TB LostAI/Animation/PodSound exits have no gameplay effects. This retains the ordinary2A0B30 removal
// chronology while propagating a provider failure before any later exit. A
// failure is terminal for this service call; do not replay removed-slot work.
inline SourceTriggerContactReceipt50TB serviceNativeSourceTrigger50TB(
    NativeTriggerGroup50TB& group,std::size_t trigger,const TriggerOverlap50IN* observations,
    std::size_t count,const SourceTriggerActionHooks50TB& hooks,
    const SourceTriggerContactState50TB& pending,bool eligible=true) noexcept {
    namespace d=trigger_entry50IN_detail;using S=SourceTriggerContactStatus50TB;
    SourceTriggerContactReceipt50TB r;
    if(!validSourceTriggerGroup50TB(group,hooks)||trigger>=group.triggerCount||
       (pending.phase!=SourceTriggerContactPhase50TB::Idle&&
        pending.phase!=SourceTriggerContactPhase50TB::Complete))return r;
    auto& s=group.triggers[trigger];
    if(!eligible||!(s.flags29&1u)||(s.flags29&0xcu)){r.status=S::Complete;return r;}
    if(count!=s.collider.highWaterD0||(count&&!observations))return r;
    const auto call=[&](std::uint32_t other,std::size_t action,std::size_t slot) noexcept {
        return SourceTriggerActionCall50TB{{s.collider.identity,other,action,false,0x2a0d20u},0,slot};
    };
    for(std::size_t i=0;i<count;++i){
        const auto id=s.collider.membersCC[i];const auto* other=observations[i].collider;
        if(!id){if(other)return r;continue;}
        if(!other||other==&s.collider||!d::valid(*other)||other->identity!=id||
           nativeTriggerCollider50TB(group,id)==group.colliders.size()||
           other!=&group.colliders[nativeTriggerCollider50TB(group,id)])return r;
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
            if(r.action.status!=SourceTriggerActionStatus50TB::Complete){r.status=S::ActionBlocked;return r;}
            ++r.callbacks;
        }
    }
    r.status=S::Complete;return r;
}

// Called only at a genuine indexed E8. Failure is terminal: no automatic resume,
// no later candidate and no reciprocal additions after the failing action.
inline SourceTriggerContactReceipt50TB dispatchNativeTriggerContact50TB(
 NativeTriggerGroup50TB& group,std::size_t trigger,std::uint32_t collider,std::size_t candidateOrdinal)noexcept {
 SourceTriggerContactReceipt50TB r;
 if(!nativeTriggerQueryBound50TB(group,collider))return r;
 if(!releaseCompletedSourceContact50TB(group.contact))return r;
 r=resumeNativeSourceTriggerContact50TB(group,trigger,collider,group.actions,group.contact,group.querySerial,candidateOrdinal);
 group.lastContact=r;
 if(r.status!=SourceTriggerContactStatus50TB::Complete)group.actionsFailed=true;
 return r;
}
} // namespace rr::game
