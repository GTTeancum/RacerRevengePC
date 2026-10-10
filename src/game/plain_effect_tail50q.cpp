#include "plain_effect_tail50q.h"
namespace rr::game {
PlainEffectResult50Q inspectPlainEffectTail50Q(const PlainEffectOwners50Q& owners) noexcept {
 PlainEffectResult50Q out{};
 if(owners.attachment334){out.status=PlainEffectStatus50Q::UnsupportedAttached;return out;}
 for(unsigned i=0;i<12;++i)if(owners.slots[i].id)
  for(unsigned j=0;j<i;++j)if(owners.slots[i].id==owners.slots[j].id){out.status=PlainEffectStatus50Q::AliasedOwners;return out;}
 for(unsigned i=0;i<12;++i){const auto&slot=owners.slots[i];if(!slot.id||!(slot.flags1D4&2u))continue;
  auto&event=out.events[out.count++];event.kind=ImpactEffectKind50B::Stop84;event.target=slot.id;event.sourcePC=0x2283ccu+i*0x34u;
 }
 return out;
}
}
