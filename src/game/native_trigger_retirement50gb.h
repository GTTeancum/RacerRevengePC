#pragma once
#include "native_trigger_group50gb.h"
namespace rr::game {
// Read-only validation for native terminal ownership cleanup, not an original
// destructor or an ordinary non-overlap/exit-service equivalence claim.
struct NativeTriggerTerminalValidation50GB {
 bool valid=false;std::size_t pairs=0,owner=0,slot=0;
};
inline NativeTriggerTerminalValidation50GB validateNativeTriggerTerminalGraph50GB(
 const NativeTriggerGroup50GB& group)noexcept {
 NativeTriggerTerminalValidation50GB r{};
 const std::size_t triggers=group.triggers.size(),owners=triggers+group.colliders.size();
 const auto owner=[&](std::size_t i)->const TriggerColliderContacts50IN& {
  return i<triggers?group.triggers[i].collider:group.colliders[i-triggers];
 };
 // Live trigger count plus the bound racer colliders (stock GB: 18 + 16 = 34).
 for(std::size_t i=0;i<owners;++i){r.owner=i;const auto& a=owner(i);
  if(!a.identity||a.highWaterD0>triggerContactCapacity50IN)return r;
  for(std::size_t j=0;j<i;++j)if(owner(j).identity==a.identity)return r;
  for(std::size_t s=0;s<triggerContactCapacity50IN;++s){r.slot=s;const auto id=a.membersCC[s];if(!id)continue;
   if(s>=a.highWaterD0||id==a.identity)return r;
   for(std::size_t p=0;p<s;++p)if(a.membersCC[p]==id)return r;
   const TriggerColliderContacts50IN* peer=nullptr;
   const std::size_t begin=i<triggers?triggers:0,end=i<triggers?owners:triggers;
   for(std::size_t p=begin;p<end;++p)if(owner(p).identity==id){if(peer)return r;peer=&owner(p);}
   if(!peer||peer->highWaterD0>triggerContactCapacity50IN)return r;
   std::size_t matches=0;for(std::size_t p=0;p<triggerContactCapacity50IN;++p)
    if(peer->membersCC[p]==a.identity){if(p>=peer->highWaterD0)return r;++matches;}
   if(matches!=1)return r;
   if(i<triggers)++r.pairs;
  }
 }
 r.valid=true;r.owner=r.slot=0;return r;
}
} // namespace rr::game
