#include "optional_probe_effects50p.h"
#include <cstring>
namespace rr::game {
OptionalProbeResult50P inspectOptionalProbeEffects50P(const OptionalProbeOwners50P&o,
 std::uint32_t probe104,std::uint32_t probe118,const float&z) noexcept {
 OptionalProbeResult50P r{};
 if(!o.optional7FC||!o.manager51C)return r;
 if(probe104){
  std::uint32_t bits;std::memcpy(&bits,&z,4);const auto exponent=bits&0x7f800000u;
  if(exponent==0x7f800000u||(!exponent&&(bits&0x7fffffu))){r.status=OptionalProbeStatus50P::Arithmetic;r.sourcePC=0x217a20;return r;}
  if(z>0.f&&!probe118){r.status=OptionalProbeStatus50P::UnsupportedActive;r.sourcePC=0x217a40;return r;}
 }
 if(!o.effect808[0]||!o.effect808[1]){r.status=OptionalProbeStatus50P::Binding;r.sourcePC=0x217d94;return r;}
 r.count=2;
 for(unsigned i=0;i<2;++i){r.events[i].kind=ImpactEffectKind50B::Stop84;r.events[i].sourcePC=i?0x217db8:0x217da0;r.events[i].target=o.effect808[i];}
 return r;
}
}
