#pragma once
#include "trigger_entry50in.h"
#include <array>
#include <cstdint>
#include <limits>
namespace rr::game {
// Distinct exact GB storage; existing GA, TC, TB and generic callback bounds are untouched.
inline constexpr std::size_t nativeTriggerCount50GB=18,nativeTriggerColliderCount50GB=16;
struct NativeTriggerGroup50GB;
struct SourceTriggerActionCall50GB {
    TriggerActionCall50IN source{};
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0;
};
enum class SourceTriggerActionStatus50GB : std::uint8_t {Complete,Blocked,InvalidBinding};
struct SourceTriggerActionResult50GB {
    SourceTriggerActionStatus50GB status=SourceTriggerActionStatus50GB::InvalidBinding;
    std::uint32_t sourcePC=0,ownerOffset=0;
};
struct SourceTriggerActionHooks50GB {
    void* context=nullptr;
    // Pure structural admission, independent of earlier action effects.
    bool (*preflight)(void*,const SourceTriggerActionCall50GB&) noexcept=nullptr;
    // Performs this actual action. It may mutate only its separately
    // admitted C80, animation, PodSound or particle owners, never group/hook/contact-membership storage.
    SourceTriggerActionResult50GB (*resume)(void*,const SourceTriggerActionCall50GB&) noexcept=nullptr;
};
enum class SourceTriggerContactPhase50GB : std::uint8_t {
    Idle,Actions,AddTriggerMember,AddOtherMember,Complete
};
struct SourceTriggerContactState50GB {
    SourceTriggerContactPhase50GB phase=SourceTriggerContactPhase50GB::Idle;
    std::uint64_t querySerial=0;
    std::size_t candidateOrdinal=0,triggerIndex=0,colliderIndex=0;
    std::size_t nextAction=0,actionCount=0;
    std::uint32_t triggerCollider=0,otherCollider=0;
    const NativeTriggerGroup50GB* group=nullptr;
    const SourceTriggerActionHooks50GB* hooks=nullptr;
    bool dispatchCounted=false;
};
enum class SourceTriggerContactStatus50GB : std::uint8_t {
    Complete,Input,Capacity,ActionInput,ActionBlocked,OwnerChanged
};
struct SourceTriggerContactReceipt50GB {
    SourceTriggerContactStatus50GB status=SourceTriggerContactStatus50GB::Input;
    std::size_t callbacks=0,addedPairs=0,removedPairs=0;
    bool serviceRan=false;
    SourceTriggerActionResult50GB action{};
};
struct NativeTriggerGroup50GB {
 std::array<TriggerEntryState50IN,nativeTriggerCount50GB> triggers{};
 std::array<TriggerColliderContacts50IN,nativeTriggerColliderCount50GB> colliders{};
 std::array<std::uint32_t,nativeTriggerCount50GB> ignoreCollider{};
 std::array<std::array<std::uint32_t,8>,nativeTriggerCount50GB> ignoreColliders{};
 std::array<std::size_t,nativeTriggerCount50GB> ignoreColliderCount{};
 SourceTriggerActionHooks50GB actions{};
 SourceTriggerContactState50GB contact{};
 SourceTriggerContactReceipt50GB lastContact{};
 std::uint64_t querySerial=0;
 std::uint32_t queryCollider=0;
 bool ready=false,actionsFailed=false,queryActive=false;
 std::uint64_t contacts=0,dispatches=0;
};
inline std::size_t nativeTriggerCollider50GB(const NativeTriggerGroup50GB& group,std::uint32_t id)noexcept {
 for(std::size_t i=0;i<group.colliders.size();++i)if(group.colliders[i].identity==id)return i;
 return group.colliders.size();
}
// Structural admission is shared by the distinct fallible source-action route.
// A source-only group is never admitted by the legacy action dispatcher.
inline bool validNativeTriggerStorage50GB(const NativeTriggerGroup50GB& group)noexcept {
 if(!group.ready||group.actionsFailed)return false;
 for(std::size_t i=0;i<group.colliders.size();++i){
  if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
  for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
 }
 for(std::size_t i=0;i<group.triggers.size();++i){const auto& state=group.triggers[i];
  if(!trigger_entry50IN_detail::valid(state)||state.randomTriggerPercent90!=100||state.callbackCount98!=(i==15?2u:i==17?5u:1u))return false;
  if(nativeTriggerCollider50GB(group,state.collider.identity)!=group.colliders.size())return false;
  for(std::size_t j=0;j<i;++j)if(state.collider.identity==group.triggers[j].collider.identity)return false;
  if(group.ignoreCollider[i]&&nativeTriggerCollider50GB(group,group.ignoreCollider[i])==group.colliders.size())return false;
  if(group.ignoreColliderCount[i]>group.ignoreColliders[i].size())return false;
  for(std::size_t j=0;j<group.ignoreColliderCount[i];++j){
   const auto ignored=group.ignoreColliders[i][j];
   if(!ignored||nativeTriggerCollider50GB(group,ignored)==group.colliders.size())return false;
   for(std::size_t k=0;k<j;++k)if(group.ignoreColliders[i][k]==ignored)return false;
  }
  for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
   const auto id=state.collider.membersCC[slot];if(!id)continue;
   const auto c=nativeTriggerCollider50GB(group,id);
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
inline bool validSourceTriggerGroup50GB(const NativeTriggerGroup50GB& group,
 const SourceTriggerActionHooks50GB& hooks) noexcept {
 return &hooks==&group.actions&&hooks.context&&hooks.preflight&&hooks.resume&&validNativeTriggerStorage50GB(group);
}
inline bool validNativeTriggerGroup50GB(const NativeTriggerGroup50GB& group) noexcept {
 return validSourceTriggerGroup50GB(group,group.actions);
}
inline bool nativeTriggerQueryBound50GB(const NativeTriggerGroup50GB& group,std::uint32_t collider)noexcept {
 return validNativeTriggerGroup50GB(group)&&group.queryActive&&group.querySerial&&group.queryCollider==collider&&
  nativeTriggerCollider50GB(group,collider)<group.colliders.size()&&
  (group.contact.phase==SourceTriggerContactPhase50GB::Idle||group.contact.phase==SourceTriggerContactPhase50GB::Complete);
}
inline bool nativeTriggerPairAllowed50GB(const NativeTriggerGroup50GB& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 if(trigger>=group.triggers.size()||!group.ready||group.actionsFailed||
    nativeTriggerCollider50GB(group,collider)==group.colliders.size()||group.ignoreCollider[trigger]==collider)return false;
 if(group.ignoreColliderCount[trigger]>group.ignoreColliders[trigger].size())return false;
 for(std::size_t i=0;i<group.ignoreColliderCount[trigger];++i)if(group.ignoreColliders[trigger][i]==collider)return false;
 const auto& state=group.triggers[trigger];
 if(!(state.flags29&1u))return false;
 return trigger_entry50IN_detail::find(state.collider,collider)==state.collider.highWaterD0;
}
// Discard only a completed observation before a different candidate. Never
// discard an unfinished action or either reciprocal-add prefix.
inline bool releaseCompletedSourceContact50GB(SourceTriggerContactState50GB& state) noexcept {
    if(state.phase!=SourceTriggerContactPhase50GB::Idle&&
       state.phase!=SourceTriggerContactPhase50GB::Complete)return false;
    state={};return true;
}
inline SourceTriggerContactReceipt50GB resumeNativeSourceTriggerContact50GB(
    NativeTriggerGroup50GB& group,std::size_t trigger,std::uint32_t collider,
    const SourceTriggerActionHooks50GB& hooks,SourceTriggerContactState50GB& state,
    std::uint64_t querySerial,std::size_t candidateOrdinal) noexcept {
    namespace d=trigger_entry50IN_detail;
    using P=SourceTriggerContactPhase50GB;using S=SourceTriggerContactStatus50GB;
    SourceTriggerContactReceipt50GB r;
    if(!querySerial||!validSourceTriggerGroup50GB(group,hooks)||trigger>=group.triggers.size())return r;
    const auto ci=nativeTriggerCollider50GB(group,collider);
    if(ci==group.colliders.size())return r;
    auto& s=group.triggers[trigger];auto& other=group.colliders[ci];
    const auto call=[&](std::size_t action) noexcept {
        return SourceTriggerActionCall50GB{{s.collider.identity,collider,action,true,0x2a0eccu},
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
        if(!nativeTriggerPairAllowed50GB(group,trigger,collider)){
            r.status=S::Complete;return r;
        }
        // Existing50GB native prefilter suppresses sustained pairs before
        // touched/chance. Keep a second explicit fresh gate before actions.
        const bool fresh=d::find(s.collider,collider)==s.collider.highWaterD0;
        if(!fresh){r.status=S::Complete;return r;}
        if(group.contacts!=std::numeric_limits<std::uint64_t>::max())++group.contacts;
        const auto touched=static_cast<std::uint8_t>(s.flags1B8|0x10u);
        if((touched&4u)&&(touched&8u)){
            s.flags1B8=touched;r.status=S::Complete;return r;
        }
        // Authentic GB only: randomPercent100 is genuine authored admission. Do
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
        if(r.action.status!=SourceTriggerActionStatus50GB::Complete){
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

// GB LostAI/Animation/PodSound/Particles exits have no gameplay effects. This retains the ordinary2A0B30 removal
// chronology while propagating a provider failure before any later exit. A
// failure is terminal for this service call; do not replay removed-slot work.
inline SourceTriggerContactReceipt50GB serviceNativeSourceTrigger50GB(
    NativeTriggerGroup50GB& group,std::size_t trigger,const TriggerOverlap50IN* observations,
    std::size_t count,const SourceTriggerActionHooks50GB& hooks,
    const SourceTriggerContactState50GB& pending,bool eligible=true) noexcept {
    namespace d=trigger_entry50IN_detail;using S=SourceTriggerContactStatus50GB;
    SourceTriggerContactReceipt50GB r;
    if(!validSourceTriggerGroup50GB(group,hooks)||trigger>=group.triggers.size()||
       (pending.phase!=SourceTriggerContactPhase50GB::Idle&&
        pending.phase!=SourceTriggerContactPhase50GB::Complete))return r;
    auto& s=group.triggers[trigger];
    if(!eligible||!(s.flags29&1u)||(s.flags29&0xcu)){r.status=S::Complete;return r;}
    if(count!=s.collider.highWaterD0||(count&&!observations))return r;
    const auto call=[&](std::uint32_t other,std::size_t action,std::size_t slot) noexcept {
        return SourceTriggerActionCall50GB{{s.collider.identity,other,action,false,0x2a0d20u},0,slot};
    };
    for(std::size_t i=0;i<count;++i){
        const auto id=s.collider.membersCC[i];const auto* other=observations[i].collider;
        if(!id){if(other)return r;continue;}
        if(!other||other==&s.collider||!d::valid(*other)||other->identity!=id||
           nativeTriggerCollider50GB(group,id)==group.colliders.size()||
           other!=&group.colliders[nativeTriggerCollider50GB(group,id)])return r;
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
            if(r.action.status!=SourceTriggerActionStatus50GB::Complete){r.status=S::ActionBlocked;return r;}
            ++r.callbacks;
        }
    }
    r.status=S::Complete;return r;
}

// Called only at a genuine indexed E8. Failure is terminal: no automatic resume,
// no later candidate and no reciprocal additions after the failing action.
inline SourceTriggerContactReceipt50GB dispatchNativeTriggerContact50GB(
 NativeTriggerGroup50GB& group,std::size_t trigger,std::uint32_t collider,std::size_t candidateOrdinal)noexcept {
 SourceTriggerContactReceipt50GB r;
 if(!nativeTriggerQueryBound50GB(group,collider))return r;
 if(!releaseCompletedSourceContact50GB(group.contact))return r;
 r=resumeNativeSourceTriggerContact50GB(group,trigger,collider,group.actions,group.contact,group.querySerial,candidateOrdinal);
 group.lastContact=r;
 if(r.status!=SourceTriggerContactStatus50GB::Complete)group.actionsFailed=true;
 return r;
}
} // namespace rr::game
