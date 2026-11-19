#pragma once
// Parent-owned native integration checker. Inputs must come from the actual
// Application/selected PHY/track-mask owners. No synthetic callback or racer
// collider identity is accepted or issued. No gameplay query is run here.
#include "mc_animation_binding50mc.h"
#include "mc_animation_source_runtime50mc_test.h"
#include "mc_lighting_authoring50mc.h"
#include "mc_typed_navigation50mc_test.h"
namespace mc_lighting_binding50MC_test {
namespace g=rr::game;namespace light=g::original_gc_gameplay_lighting_candidate;
inline bool check(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
 std::shared_ptr<g::OriginalApplicationEntities50LA> application,
 rr::assets::TrackCollisionMasks50AN masks,const std::vector<std::uint8_t>& selectedPhy,
 const track_static_candidates50IG::Bindings& names,g::ModelMotionSharedState49T& motion,
 const track_runtime_storage50IG::Limits& limits,std::size_t alreadyRetained,
 std::string& error){
 unsigned check=0;auto need=[&](bool x,const std::string& why){++check;if(!x)throw std::runtime_error("check "+std::to_string(check)+": "+why);};
 auto lighting=std::make_shared<light::Owner>();auto resources=std::make_shared<mc_animation_source50MC::Owner>();
 auto runtime=std::make_shared<mc_animation_runtime50MC::Owner>();auto triggers=std::make_shared<mc_trigger_authoring50MC::Owner>();
 auto shapes=std::make_shared<mc_trigger_geometry50MC::Owner>();auto actions=std::make_shared<mc_trigger_actions50MC::Owner>();
 auto collision=std::make_shared<mc_collision_world50MC::Owner>();mc_animation_binding50MC::Binding binding;native_racer50HN::Bank bank;
 light::Diagnostic ld;mc_animation_source50MC::Diagnostic sd;mc_animation_runtime50MC::Diagnostic rd;
 mc_animation_binding50MC::Diagnostic bd;mc_collision_world50MC::Diagnostic cd;
 bool bankSealed=false;
 auto cleanup=[&]()noexcept{
  if(binding.needsClose()&&!binding.close(bd))return false;
  if(collision->phase()!=mc_collision_world50MC::Phase::Closed&&!collision->beginRetirement())return false;
  auto borrow=collision->retirementBorrow();
  if(borrow){auto* storage=borrow.registrationStorage();if(!storage)return false;
   auto remove=[&](){for(std::size_t i=0;i<21;++i){auto* m=borrow.triggerMembership(i);if(!m)return false;
    if(m->state.attached||m->state.object){auto r=g::removeLiveZoneMembership50EN(storage->lifecycle,storage->world,m->state,storage->membershipWork);
     if(r.status!=g::LiveZoneStatus50EN::Complete)return false;}}return true;};
   try{if(bankSealed){if(!bank.visitAll([&](std::size_t){return remove();}))return false;}else if(!remove())return false;}catch(...){return false;}
  }
  return actions->close()&&bank.retireNativeBacking();
 };
 try{
  need(source&&source->ready()&&application&&application->application10(),"actual source/Application required");
  need(limits.coldPayloadBytes<=32u*1024u*1024u&&alreadyRetained<limits.coldPayloadBytes,"bounded actual reservation");
  const auto available=limits.coldPayloadBytes-alreadyRetained;
  need(lighting->prepareMcNative(source,8,ld),"actual eight-racer MC lighting");
  need(lighting->mcSourceIdentity()==source.get()&&lighting->world()==&source->world(),"same retained World identity");
  if(!(resources->decode(source,available,sd)))throw std::runtime_error(sd.message);
  if(!(runtime->beginPrepare(resources,motion,available,rd)))throw std::runtime_error(rd.message);
  if(!(triggers->decode(source,available,error)))throw std::runtime_error(error);if(!(shapes->prepare(triggers,error)))throw std::runtime_error(error);
  need(actions->prepare(triggers,runtime,shapes),"genuine source trigger actions");
  if(!(binding.beginVisual(runtime,lighting,bd)))throw std::runtime_error(bd.message);
  g::original_racer_lighting50KM::LightMatrices rows{};
  need(!lighting->queryMcZoneLight(1,{0,0,0},rows,ld),"incomplete visual prefix must block light query");
  const auto external=alreadyRetained+resources->retainedPayloadBytes()+runtime->requiredPayloadBytes()+
   lighting->mcRetainedPayloadBytes()+sizeof(binding)+sizeof(*triggers)+sizeof(*shapes)+sizeof(*actions);
  if(!(collision->prepareGround(source,masks,selectedPhy,4,external,limits,error)))throw std::runtime_error(error);
  if(!(collision->prepareStorage(application,triggers,actions,runtime,names,limits,cd)))throw std::runtime_error(cd.message);
  need(collision->animationPending()&&!collision->geometryReady(),"unpublished aggregate boundary");
  if(!(binding.completePreparation(collision,motion,bd)))throw std::runtime_error(bd.message);
  need(binding.ready()&&runtime->ready()&&collision->geometryReady()&&!collision->queryReady(),"cold actual publication, no fabricated query authority");
  if(!mc_typed_navigation_test50MC::check(source,*collision,error))throw std::runtime_error(error);
  if(!(mc_animation_source_runtime50MC_test::checkActualBoundInitial(*runtime,error)))throw std::runtime_error(error);
  {
   auto borrow=collision->borrow();need(bool(borrow),"actual aggregate borrow");auto* storage=borrow.registrationStorage();
   need(storage&&storage->count.zones==28&&storage->count.portals==28&&storage->count.membership==29&&storage->count.terrain==111,"closed MC topology extent");
   storage->registerMutableOwners(bank);
   for(auto& candidate:borrow.candidateStorage()){bank.owner(candidate.candidate.collision.filter.flagsF8);bank.owner(candidate.candidate.collision.copied40);bank.owner(candidate.candidate.collision.copied50);}
   for(std::size_t i=0;i<21;++i){auto* m=borrow.triggerMembership(i);need(m&&m->state.attached,"actual trigger membership");bank.owner(m->state);bank.elements(m->links.data(),m->links.size());bank.elements(m->entries.data(),m->entries.size());}
   need(!collision->close(),"real borrow must fence aggregate close");
  }
  if(!(binding.enrollBank(bank,bd)))throw std::runtime_error(bd.message);bank.seal(8);bankSealed=true;
  if(!(binding.activateSealedBank(bd)&&binding.ready()))throw std::runtime_error(bd.message);
  for(std::uint32_t zone=1;zone<=28;++zone){need(lighting->queryMcZoneLight(zone,{1e8f,1e8f,1e8f},rows,ld),"actual zone light rows");
   need((rows.diffuse[0]==std::array<float,4>{229,184,216,0})&&(rows.diffuse[3]==std::array<float,4>{47,39.5f,45,64}),"MC authored diffuse/ambient");}
  need(lighting->mcLightRecord(28)&&lighting->mcLightRecord(28)->directional->hasLensFlare,"actual directional flare record");
  need(!lighting->queryMcZoneLight(29,{0,0,0},rows,ld)&&!lighting->mcLightRecord(29),"closed source bounds");
  need(cleanup(),"real all-peer retirement");need(collision->close(),"aggregate closes after actual membership/Bank retirement");
  error.clear();return true;
 }catch(const std::exception& e){error=e.what();if(!cleanup())error+="; actual reached-prefix retirement blocked";return false;}
}
}
