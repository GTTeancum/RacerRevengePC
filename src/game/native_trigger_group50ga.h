#pragma once
#include "trigger_entry50in.h"
#include <array>

namespace rr::game {
// Bounded native GA trigger set. Geometry remains the unchanged explicit
// nativeType2GateOverlap50KG policy; this container adds no hit approximation.
// One shared track owner, never copied into the per-racer execution bank.
inline constexpr std::size_t nativeTriggerCount50GA=10,nativeTriggerColliderCount50GA=16;
struct NativeTriggerGroup50GA {
 std::array<TriggerEntryState50IN,nativeTriggerCount50GA> triggers{};
 std::array<TriggerColliderContacts50IN,nativeTriggerColliderCount50GA> colliders{};
 std::array<std::uint32_t,nativeTriggerCount50GA> ignoreCollider{};
 TriggerActionHooks50IN actions{};
 bool ready=false,actionsFailed=false;
 std::uint64_t contacts=0,dispatches=0;
};
inline std::size_t nativeTriggerCollider50GA(const NativeTriggerGroup50GA& group,std::uint32_t id)noexcept {
 for(std::size_t i=0;i<group.colliders.size();++i)if(group.colliders[i].identity==id)return i;
 return group.colliders.size();
}
inline bool validNativeTriggerGroup50GA(const NativeTriggerGroup50GA& group)noexcept {
 if(!group.ready||group.actionsFailed||!group.actions.preflight||!group.actions.apply)return false;
 for(std::size_t i=0;i<group.colliders.size();++i){
  if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
  for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
 }
 for(std::size_t i=0;i<group.triggers.size();++i){const auto& state=group.triggers[i];
  if(!trigger_entry50IN_detail::valid(state)||state.randomTriggerPercent90!=100||!state.callbackCount98||state.callbackCount98>2)return false;
  if(nativeTriggerCollider50GA(group,state.collider.identity)!=group.colliders.size())return false;
  for(std::size_t j=0;j<i;++j)if(state.collider.identity==group.triggers[j].collider.identity)return false;
  if(group.ignoreCollider[i]&&nativeTriggerCollider50GA(group,group.ignoreCollider[i])==group.colliders.size())return false;
  for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){
   const auto id=state.collider.membersCC[slot];if(!id)continue;
   const auto c=nativeTriggerCollider50GA(group,id);
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
// Additional actual trigger filters. Run in the ordinary indexed collector,
// after its bilateral category/identity filters and before geometric dispatch.
// Existing reciprocal membership suppresses repeat entries before touched or
// chance state changes. PLAYER means the actual source player's main collider.
inline bool nativeTriggerPairAllowed50GA(const NativeTriggerGroup50GA& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 if(trigger>=group.triggers.size()||!group.ready||group.actionsFailed||
    nativeTriggerCollider50GA(group,collider)==group.colliders.size()||group.ignoreCollider[trigger]==collider)return false;
 const auto& state=group.triggers[trigger];
 if(!(state.flags29&1u))return false;
 return trigger_entry50IN_detail::find(state.collider,collider)==state.collider.highWaterD0;
}
inline TriggerEntryReceipt50IN dispatchNativeTriggerContact50GA(NativeTriggerGroup50GA& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 TriggerEntryReceipt50IN result{};
 if(!validNativeTriggerGroup50GA(group)||trigger>=group.triggers.size())return result;
 const auto index=nativeTriggerCollider50GA(group,collider);
 if(index==group.colliders.size())return result;
 if(!nativeTriggerPairAllowed50GA(group,trigger,collider)){result.status=TriggerEntryStatus50IN::Complete;return result;}
 if(group.contacts!=~std::uint64_t{0})++group.contacts;
 result=triggerContact50IN(group.triggers[trigger],group.colliders[index],group.actions);
 if(result.callbacks&&group.dispatches!=~std::uint64_t{0})++group.dispatches;
 // Preserve fired/actions/membership prefixes on downstream failure. Never
 // retry a committed event or fabricate callback success after mixer failure.
 if(group.actionsFailed)result.status=TriggerEntryStatus50IN::ActionInput;
 return result;
}
}
