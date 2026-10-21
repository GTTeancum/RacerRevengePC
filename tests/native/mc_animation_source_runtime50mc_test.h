#pragma once
#include "mc_animation_runtime50mc.h"
namespace mc_animation_source_runtime50MC_test {
// Proposed checker, parent-owned execution. No source or live readiness is minted.
inline bool check(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,
 rr::game::ModelMotionSharedState49T& actualSharedMotion,std::string& error){
 try{using mc_scene_authoring50MC::detail::need;
  namespace source=mc_animation_source50MC;namespace runtime=mc_animation_runtime50MC;
  need(cold&&cold->ready(),"MC animation test actual cold owner");
  auto resources=std::make_shared<source::Owner>();source::Diagnostic sd;
  need(resources->decode(cold,resources->maximumAdditionalPayloadBytes(),sd),sd.message.c_str());
  need(resources->ready()&&resources->coldSource()==cold.get()&&sd.completedOwners==14,"MC actual counted source join");
  std::size_t graphs=0,vertices=0,indices=0,parts=0;
  for(std::size_t i=0;i<14;++i){const auto& s=resources->source(i);graphs+=s.graphCount;parts+=s.collisionCount;
   vertices+=s.scene.lods[0].surfaces[0].vertices.size();indices+=s.scene.lods[0].surfaces[0].indices.size();
   need(s.graphCount==(i?6u:3u)&&s.bindings.count==(i?5u:2u)&&s.motion.data==s.originalMot.data(),"MC actual graph/motion backing");
   need(s.collisionCount==(i?0u:2u),"MC bridge-only collision");
   if(i)need(s.col.empty()&&s.collision.members.empty(),"MC ray cannot borrow bridge COL");
   for(std::size_t j=0;j<i;++j)need(resources->source(j).psg.data()!=s.psg.data()&&resources->source(j).originalMot.data()!=s.originalMot.data(),"MC counted owner storage cannot alias");
  }
  need(graphs==81&&vertices==5806&&indices==13182&&parts==2,"MC complete native source totals");
  runtime::Owner motion;runtime::Diagnostic rd;
  std::array<unsigned char,sizeof(actualSharedMotion)> sharedBefore{};std::memcpy(sharedBefore.data(),&actualSharedMotion,sharedBefore.size());
  need(motion.beginPrepare(resources,actualSharedMotion,motion.requiredPayloadBytes(),rd),rd.message.c_str());
  need(motion.stagedSourceReady()&&!motion.ready()&&motion.sourceOwner()==resources.get()&&motion.coldSource()==cold.get(),"MC stage is not live binding");
  constexpr bool loops[14]={false,true,true,true,true,true,false,false,false,false,false,false,true,true};
  for(std::size_t i=0;i<14;++i){const auto& s=motion.state(i);need(s.motionLoops==loops[i]&&s.sequence.active90==(i!=0)&&s.sequence.loop88==(i!=0)&&
    s.sequence.stopped10==1&&s.sequence.current34==0&&s.sequence.elapsedB0==0&&!s.initialComplete&&!s.initialAttempted,"MC actual staged playback/loop states");}
  need(!motion.activateSequence(0)&&!motion.activateSequence(1),"MC absent genuine binding rejects action");
  // Preparing does not sample/advance shared source motion; compare its typed
  // byte representation as an observation, never restore it as a fake rollback.
  need(std::memcmp(sharedBefore.data(),&actualSharedMotion,sharedBefore.size())==0,"MC staging must not advance actual shared motion");
  source::Owner shortSource;need(!shortSource.decode(cold,0,sd)&&!shortSource.ready()&&!shortSource.decode(cold,shortSource.maximumAdditionalPayloadBytes(),sd),"MC source budget and lifetime negative");
  runtime::Owner shortRuntime;need(!shortRuntime.beginPrepare(resources,actualSharedMotion,0,rd)&&!shortRuntime.stagedSourceReady(),"MC runtime budget negative");
  runtime::Owner missingRuntime;need(!missingRuntime.beginPrepare({},actualSharedMotion,missingRuntime.requiredPayloadBytes(),rd),"MC missing source negative");
  need(!motion.beginPrepare(resources,actualSharedMotion,motion.requiredPayloadBytes(),rd)&&motion.stagedSourceReady(),"MC second preparation cannot replace retained source");
  error.clear();return true;
 }catch(const std::exception& e){error=e.what();return false;}
}
// Invoke only after parent constructs actual Lighting/Collision/Bank Binding.
// This function cannot initialize or publish any of those owners.
inline bool checkActualBoundInitial(const mc_animation_runtime50MC::Owner& runtime,std::string& error){
 try{using mc_scene_authoring50MC::detail::need;need(runtime.ready(),"MC genuine ready animation Binding required");
  for(std::size_t i=0;i<14;++i){const auto& s=runtime.state(i);need(s.visualPrepared&&s.collisionPrepared&&s.initialAttempted&&s.initialComplete&&
    s.sequence.current34==0&&s.sequence.stopped10==1,"MC actual reached initial sample ordering");}
  need(runtime.collision(0).count==2,"MC actual bridge view");for(std::size_t i=1;i<14;++i)need(runtime.collision(i).count==0,"MC no ray compound publication");
  error.clear();return true;
 }catch(const std::exception& e){error=e.what();return false;}
}
}
