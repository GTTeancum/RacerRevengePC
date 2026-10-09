#include "reset_effects50bk.h"
#include <cstring>
namespace rr::game {
ResetEffectResult50BK resetEffectStops50BK(const ResetEffectBindings50BK&b,EffectOwner50BE*owners,std::size_t count,ResetEffectState50BK&state,ResetEffectWorkspace50BK&w) noexcept {
 using S=EffectDispatchStatus50BE;if(count>maxEffectOwners50BE||(count&&!owners))return {S::Storage,0,0,0};if(count)std::memcpy(w.owners,owners,count*sizeof(EffectOwner50BE));w.state=state;
 const auto admitted=consumeEffectControls50BE(nullptr,0,w.owners,count,w.control);if(admitted.status!=S::Complete)return {admitted.status,0,0,0};
 ResetEffectResult50BK result{S::Complete,0,0,0};
 auto stop=[&](std::uint32_t id,std::uint32_t pc){if(!id)return true;ImpactEffectEvent50B e{};e.kind=ImpactEffectKind50B::Stop84;e.sourcePC=pc;e.target=id;const auto r=consumeEffectControls50BE(&e,1,w.owners,count,w.control);if(r.status!=S::Complete){result.status=r.status;result.sourcePC=pc;return false;}w.trace[result.calls]={pc,id};++result.calls;result.deferredStops+=r.deferredStops;return true;};
 if(!stop(b.owners808[0],0x217278)||!stop(b.owners808[1],0x217298))return result;
 if(b.table81CPresent&&(!stop(b.owners81C[0],0x2172c0)||!stop(b.owners81C[1],0x2172e0)))return result;
 constexpr std::uint32_t pc[4]={0x217300,0x217324,0x217348,0x21736c};for(unsigned i=0;i<4;++i){if(!stop(b.owners804[i],pc[i]))return result;w.state.timers820[i]=0;}
 if(count)std::memcpy(owners,w.owners,count*sizeof(EffectOwner50BE));state=w.state;return result;
}
}
