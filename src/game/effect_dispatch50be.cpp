#include "effect_dispatch50be.h"
#include <cstring>
namespace rr::game {
EffectDispatchResult50BE consumeEffectControls50BE(const ImpactEffectEvent50B*events,std::size_t count,EffectOwner50BE*owners,std::size_t ownerCount,EffectDispatchWorkspace50BE&w) noexcept {
 using S=EffectDispatchStatus50BE;
 if(count>maxEffectCommands50BE||ownerCount>maxEffectOwners50BE||(count&&!events)||(ownerCount&&!owners))return {S::Storage,0,0,0};
 for(std::size_t i=0;i<ownerCount;++i){if(!owners[i].id||static_cast<std::uint32_t>(owners[i].profile)>static_cast<std::uint32_t>(EffectProfile50BE::Splash19DBE0))return {S::Binding,i,0,0};for(std::size_t j=0;j<i;++j)if(owners[i].id==owners[j].id)return {S::Binding,i,0,0};}
 if(ownerCount)std::memcpy(w.next,owners,ownerCount*sizeof(EffectOwner50BE));std::size_t deferred=0;
 for(std::size_t i=0;i<count;++i){const auto&e=events[i];EffectOwner50BE*owner=nullptr;for(std::size_t j=0;j<ownerCount;++j)if(w.next[j].id==e.target){owner=w.next+j;break;}if(!owner)return {S::Binding,i,0,0};std::uint32_t value[4];std::memcpy(value,e.value,16);
 switch(e.kind){
 case ImpactEffectKind50B::Start80:startEffect50BC(owner->state);break;
 case ImpactEffectKind50B::Stop84:
  if(owner->profile==EffectProfile50BE::Deferred15E800){const auto result=stopDeferredEffect50BD(owner->state,owner->mode1D8,owner->duration1E4);if(result==EffectStopResult50BD::UnsupportedDuration)return {S::UnsupportedDuration,i,0,0};if(result==EffectStopResult50BD::Deferred)++deferred;}
  else stopEffect50BC(owner->state);break;
 case ImpactEffectKind50B::Position15ACB0:positionEffect50BC(owner->state,value);break;
 case ImpactEffectKind50B::Velocity15FBF0:velocityEffect50BC(owner->state,value);break;
 case ImpactEffectKind50B::Intensity19A810:if(owner->profile!=EffectProfile50BE::Particle19A5E0)return {S::UnsupportedCommand,i,0,0};intensityEffect50BC(owner->state,value[0]);break;
 default:return {S::UnsupportedCommand,i,0,0};
 }
 }
 if(ownerCount)std::memcpy(owners,w.next,ownerCount*sizeof(EffectOwner50BE));return {S::Complete,count,count,deferred};
}
}
