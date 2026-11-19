#pragma once
#include "native_trigger_group50mc.h"
namespace rr::game {
// Read-only validation for native terminal ownership cleanup, not an original
// destructor or an ordinary non-overlap/exit-service equivalence claim.
struct NativeTriggerTerminalValidation50MC {
 bool valid=false;std::size_t pairs=0,owner=0,slot=0;
};
inline NativeTriggerTerminalValidation50MC validateNativeTriggerTerminalGraph50MC(
 const NativeTriggerGroup50MC& group)noexcept {
 NativeTriggerTerminalValidation50MC r{};
 // Live trigger prefix (capacity store), then the racer colliders.
 const std::size_t live=group.triggerCount;
 if(live>group.triggers.size())return r;
 const auto owner=[&](std::size_t i)->const TriggerColliderContacts50IN& {
  return i<live?group.triggers[i].collider:group.colliders[i-live];
 };
 const std::size_t total=live+group.colliders.size();
 for(std::size_t i=0;i<total;++i){r.owner=i;const auto& a=owner(i);
  if(!a.identity||a.highWaterD0>triggerContactCapacity50IN)return r;
  for(std::size_t j=0;j<i;++j)if(owner(j).identity==a.identity)return r;
  for(std::size_t s=0;s<triggerContactCapacity50IN;++s){r.slot=s;const auto id=a.membersCC[s];if(!id)continue;
   if(s>=a.highWaterD0||id==a.identity)return r;
   for(std::size_t p=0;p<s;++p)if(a.membersCC[p]==id)return r;
   const TriggerColliderContacts50IN* peer=nullptr;
   const std::size_t begin=i<live?live:0,end=i<live?total:live;
   for(std::size_t p=begin;p<end;++p)if(owner(p).identity==id){if(peer)return r;peer=&owner(p);}
   if(!peer||peer->highWaterD0>triggerContactCapacity50IN)return r;
   std::size_t matches=0;for(std::size_t p=0;p<triggerContactCapacity50IN;++p)
    if(peer->membersCC[p]==a.identity){if(p>=peer->highWaterD0)return r;++matches;}
   if(matches!=1)return r;
   if(i<live)++r.pairs;
  }
 }
 r.valid=true;r.owner=r.slot=0;return r;
}
} // namespace rr::game
