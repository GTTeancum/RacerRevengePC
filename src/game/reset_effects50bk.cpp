#include "reset_effects50bk.h"
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
struct Range {std::uintptr_t first=0,last=0;};
template<class T> bool range(const T* p,std::size_t count,Range& out) noexcept {
 out={};if(!count)return true;
 const auto a=reinterpret_cast<std::uintptr_t>(p);
 if(!p||a%alignof(T)||count>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
 const auto bytes=count*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;
 out={a,a+bytes};return true;
}
bool overlap(Range a,Range b) noexcept {return a.first<b.last&&b.first<a.last;}
// The old API already requires disjoint descriptors/scratch. The new borrowed
// controls are checked against every caller record before any scratch write.
bool externalStorage(const ResetEffectBindings50BK& b,EffectOwner50BE* owners,
 std::size_t count,ResetEffectState50BK& state,ResetEffectWorkspace50BK& w) noexcept {
 Range fixed[4],external[2];
 if(!range(&b,1,fixed[0])||!range(owners,count,fixed[1])||
    !range(&state,1,fixed[2])||!range(&w,1,fixed[3]))return false;
 for(unsigned i=0;i<2;++i){
  if(!range(b.controls81C[i],b.controls81C[i]?1u:0u,external[i]))return false;
  for(const auto r:fixed)if(overlap(external[i],r))return false;
 }
 return !overlap(external[0],external[1]);
}
}
ResetEffectResult50BK resetEffectStops50BK(const ResetEffectBindings50BK&b,EffectOwner50BE*owners,std::size_t count,ResetEffectState50BK&state,ResetEffectWorkspace50BK&w) noexcept {
 using S=EffectDispatchStatus50BE;if(count>maxEffectOwners50BE||(count&&!owners)||!externalStorage(b,owners,count,state,w))return {S::Storage,0,0,0};
 for(unsigned i=0;i<2;++i)if(b.controls81C[i]&&(!b.table81CPresent||b.owners81C[i]))
  return {S::Binding,i?0x2172e0u:0x2172c0u,0,0};
 if(count)std::memcpy(w.owners,owners,count*sizeof(EffectOwner50BE));w.state=state;
 for(unsigned i=0;i<2;++i)if(b.controls81C[i])w.controls81C[i]=*b.controls81C[i];
 const auto admitted=consumeEffectControls50BE(nullptr,0,w.owners,count,w.control);if(admitted.status!=S::Complete)return {admitted.status,0,0,0};
 ResetEffectResult50BK result{S::Complete,0,0,0};
 auto stop=[&](std::uint32_t id,std::uint32_t pc){if(!id)return true;ImpactEffectEvent50B e{};e.kind=ImpactEffectKind50B::Stop84;e.sourcePC=pc;e.target=id;const auto r=consumeEffectControls50BE(&e,1,w.owners,count,w.control);if(r.status!=S::Complete){result.status=r.status;result.sourcePC=pc;return false;}w.trace[result.calls]={pc,id};++result.calls;result.deferredStops+=r.deferredStops;return true;};
 if(!stop(b.owners808[0],0x217278)||!stop(b.owners808[1],0x217298))return result;
 if(b.table81CPresent)for(unsigned i=0;i<2;++i){
  const std::uint32_t pc=i?0x2172e0u:0x2172c0u;
  if(b.controls81C[i]){
   stopEffect50BC(w.controls81C[i]);
   w.trace[result.calls]={pc,0,ResetEffectTarget50BK::External81C,static_cast<std::uint8_t>(i)};
   ++result.calls;
  }else if(!stop(b.owners81C[i],pc))return result;
 }
 constexpr std::uint32_t pc[4]={0x217300,0x217324,0x217348,0x21736c};for(unsigned i=0;i<4;++i){if(!stop(b.owners804[i],pc[i]))return result;w.state.timers820[i]=0;}
 if(count)std::memcpy(owners,w.owners,count*sizeof(EffectOwner50BE));state=w.state;
 for(unsigned i=0;i<2;++i)if(b.controls81C[i])*b.controls81C[i]=w.controls81C[i];
 return result;
}
}
