#pragma once
#include "mc_static_model_params50mc.h"
#include "mc_trigger_authoring50mc.h"
#include "mc_animation_authoring50mc.h"
#include "mc_particle_authoring50mc.h"

// Parent-owned proposed checker. Not compiled or run in this packet. It must
// receive a genuinely ready MC cold owner; this helper cannot manufacture one.
namespace mc_scene_authoring50MC_test {
inline bool check(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){
 try{using mc_scene_authoring50MC::detail::need;
  need(cold&&cold->ready(),"MC check requires actual cold owner");
  mc_static_model_params50MC::Owner statics;
  mc_trigger_authoring50MC::Owner triggers;
  mc_animation_authoring50MC::Owner animations;
  mc_particle_authoring50MC::Owner particles;
  need(statics.load(cold,error),"MC static decode");need(triggers.decode(cold,error),"MC trigger decode");
  need(animations.decode(cold,error),"MC animation authoring decode");need(particles.decode(cold,error),"MC particle authoring decode");
  need(statics.coldSource()==cold.get()&&triggers.coldSource()==cold.get()&&animations.coldSource()==cold.get()&&particles.coldSource()==cold.get(),"MC actual shared source join");
  for(std::size_t i=0;i<1162;++i){const auto& s=statics.source(i);need(s.requestedOrdinal==i+1&&s.effectiveOrdinal==i+1&&s.selectedRequestedBlock&&s.allowShadowReceiving148==0,"MC full numeric static iteration");}
  std::size_t boxes=0,spheres=0,percent99=0,percent100=0,percent50=0;
  for(std::size_t i=0;i<21;++i){const auto& s=triggers.source(i);boxes+=s.shapeType==6;spheres+=s.shapeType==5;
   percent99+=s.randomTriggerPercent==99;percent100+=s.randomTriggerPercent==100;percent50+=s.randomTriggerPercent==50;
   need(s.original&&s.actionOriginal&&!s.authoredReset&&!s.effectiveReset1B5A60,"MC retained action pointers/default");}
  need(boxes==14&&spheres==7&&percent99==17&&percent100==3&&percent50==1,"MC shape/probability totals");
  need(triggers.source(20).action==mc_trigger_authoring50MC::Action::Animation&&triggers.source(20).animationIndex==0,"MC actual bridge target");
  for(std::size_t i=0;i<14;++i){const auto& s=animations.source(i);need(s.useCollision==(i==0)&&s.sequenceActive==(i!=0)&&s.unconsumedNumberInSequence,"MC animation flags/default provenance");}
  for(std::size_t i=0;i<40;++i){const auto& s=particles.source(i);need(s.effectiveStartOn&&s.attached==(i<8),"MC particle source role");}
  // Negative capacity is latched even though no source state was published.
  mc_trigger_authoring50MC::Owner denied;
  need(!denied.decode(cold,0,error)&&!denied.ready()&&!denied.decode(cold,error),"MC capacity failure/one attempt");
  need(!statics.load(cold,error)&&statics.ready()&&!triggers.decode(cold,error)&&triggers.ready(),"MC repeated preparation preserves ready source");
  error.clear();return true;
 }catch(const std::exception& e){error=e.what();return false;}
}
}
