#pragma once
#include "trigger_entry50in.h"
#include <array>
#include <stdexcept>

namespace rr::game {
// Bounded native trigger set, retaining the GA API name. GA defaults to10;
// SA selects4 and MA13 while cold. Sphere5 and box6 geometry are separately
// disclosed native endpoint policies; this state container adds no geometry.
// One shared track owner, never copied into the per-racer execution bank.
inline constexpr std::size_t nativeTriggerCount50GA=10,nativeTriggerColliderCount50GA=16,
    nativeTriggerCapacity50GA=13;
// Inline fixed storage keeps the full action state in the existing writable
// owner-span guard. Logical iteration never visits unowned tail triggers.
class NativeTriggerStates50GA final {
 std::array<TriggerEntryState50IN,nativeTriggerCapacity50GA> storage_{};
 std::size_t count_=nativeTriggerCount50GA;
 bool selected_=false;
public:
 bool setCount(std::size_t count)noexcept {
  if(selected_||!count||count>storage_.size())return false;
  // Cold selection only; a started owner cannot discard reciprocal state.
  for(const auto& s:storage_)if(s.collider.identity||s.collider.highWaterD0||s.callbackCount98)return false;
  count_=count;selected_=true;return true;
 }
 std::size_t size()const noexcept{return count_;}
 auto begin()noexcept{return storage_.begin();}
 auto end()noexcept{return storage_.begin()+count_;}
 auto begin()const noexcept{return storage_.begin();}
 auto end()const noexcept{return storage_.begin()+count_;}
 TriggerEntryState50IN& operator[](std::size_t i)noexcept{return storage_[i];}
 const TriggerEntryState50IN& operator[](std::size_t i)const noexcept{return storage_[i];}
 TriggerEntryState50IN& at(std::size_t i){if(i>=count_)throw std::out_of_range("native trigger index");return storage_[i];}
 const TriggerEntryState50IN& at(std::size_t i)const {if(i>=count_)throw std::out_of_range("native trigger index");return storage_[i];}
};
struct NativeTriggerGroup50GA {
 NativeTriggerStates50GA triggers{};
 TriggerColliderStorage50IN<nativeTriggerColliderCount50GA> colliders{};
 std::array<std::uint32_t,nativeTriggerCapacity50GA> ignoreCollider{}; // legacy single-identity GA policy
 // Authentic PLAYER resolver1B5340 visits [0,source manager+34). These are
 // actual main690 identities in source roster order, never a class58 scan.
 std::array<std::array<std::uint32_t,8>,nativeTriggerCapacity50GA> ignoreColliders{};
 std::array<std::size_t,nativeTriggerCapacity50GA> ignoreColliderCount{};
 TriggerActionHooks50IN actions{};
 bool ready=false,actionsFailed=false;
 std::uint64_t contacts=0,dispatches=0;
};
inline std::size_t nativeTriggerCollider50GA(const NativeTriggerGroup50GA& group,std::uint32_t id)noexcept {
 for(std::size_t i=0;i<group.colliders.size();++i)if(group.colliders[i].identity==id)return i;
 return group.colliders.size();
}
// Structural admission is shared by the distinct fallible source-action route.
// A source-only group is never admitted by the legacy action dispatcher.
inline bool validNativeTriggerStorage50GA(const NativeTriggerGroup50GA& group)noexcept {
 if(!group.ready||group.actionsFailed)return false;
 for(std::size_t i=0;i<group.colliders.size();++i){
  if(!trigger_entry50IN_detail::valid(group.colliders[i]))return false;
  for(std::size_t j=0;j<i;++j)if(group.colliders[i].identity==group.colliders[j].identity)return false;
 }
 for(std::size_t i=0;i<group.triggers.size();++i){const auto& state=group.triggers[i];
  if(!trigger_entry50IN_detail::valid(state)||state.randomTriggerPercent90!=100||!state.callbackCount98||state.callbackCount98>2)return false;
  if(nativeTriggerCollider50GA(group,state.collider.identity)!=group.colliders.size())return false;
  for(std::size_t j=0;j<i;++j)if(state.collider.identity==group.triggers[j].collider.identity)return false;
  if(group.ignoreCollider[i]&&nativeTriggerCollider50GA(group,group.ignoreCollider[i])==group.colliders.size())return false;
  if(group.ignoreColliderCount[i]>group.ignoreColliders[i].size())return false;
  for(std::size_t j=0;j<group.ignoreColliderCount[i];++j){
   const auto ignored=group.ignoreColliders[i][j];
   if(!ignored||nativeTriggerCollider50GA(group,ignored)==group.colliders.size())return false;
   for(std::size_t k=0;k<j;++k)if(group.ignoreColliders[i][k]==ignored)return false;
  }
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
inline bool validNativeTriggerGroup50GA(const NativeTriggerGroup50GA& group)noexcept {
 return group.actions.preflight&&group.actions.apply&&validNativeTriggerStorage50GA(group);
}
// Additional actual trigger filters. Run in the ordinary indexed collector,
// after its bilateral category/identity filters and before geometric dispatch.
// Existing reciprocal membership suppresses repeat entries before touched or
// chance state changes. PLAYER means each source manager+34 prefix main690.
inline bool nativeTriggerPairAllowed50GA(const NativeTriggerGroup50GA& group,
    std::size_t trigger,std::uint32_t collider)noexcept {
 if(trigger>=group.triggers.size()||!group.ready||group.actionsFailed||
    nativeTriggerCollider50GA(group,collider)==group.colliders.size()||group.ignoreCollider[trigger]==collider)return false;
 if(group.ignoreColliderCount[trigger]>group.ignoreColliders[trigger].size())return false;
 for(std::size_t i=0;i<group.ignoreColliderCount[trigger];++i)if(group.ignoreColliders[trigger][i]==collider)return false;
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
