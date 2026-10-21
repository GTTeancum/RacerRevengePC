#pragma once
#include "mc_particle_runtime50mc.h"
namespace mc_particle_source_runtime50mc_test {
inline bool checkStaged(const mc_particle_source50MC::Owner& source,
 const mc_particle_runtime50MC::Owner& runtime,const rr::game::OriginalGcParticleManager50KG& manager,
 std::string& error){
 const auto need=[&](bool b,const char* message){if(!b)throw std::runtime_error(message);};
 try{need(source.ready()&&runtime.sourceStaged()&&runtime.sourceOwner()==&source,"MC staged source owner join");
  need(runtime.initializedAssociations()==40&&manager.emitterRegistryReservation().registeredEmitters==40,"MC actual40 manager rows");
  std::size_t fire=0,spark=0,attached=0;for(std::size_t i=0;i<40;++i){const auto& desc=source.descriptor(i);const auto& f=source.definitionForDescriptor(i);const auto* row=manager.emitter(i+1);
   need(row&&row->particleGroup==f.particleGroup1B8&&row->framesPerSet==f.framesPerSet&&row->gravity==f.gravityMultiplier,"MC exact row definition join");
   fire+=f.kind==mc_particle_source50MC::Kind::Fire8;spark+=f.kind==mc_particle_source50MC::Kind::Sparks2;attached+=desc.attached;
   need(desc.effectiveStartOn&&desc.attached==(i<8),"MC authored startup/attachment order");
   if(i>=2&&i<8)need(f.timing.mode1D8==3&&f.movingEmitter200&&f.particleGroup1B8==6,"MC actual source2 timing/group");
  }need(fire==34&&spark==6&&attached==8,"MC subtype extent");
  need(source.atlas().frames.size()==152&&source.atlas().groups[6].type==6&&source.atlas().groups[9].type==8,"MC152 atlas subtype routes");
  const auto* graphs=runtime.attachmentOwner();need(graphs&&graphs->coldSource()==source.coldSource()&&graphs->spanReferenceCount()==8,"MC actual same-source graphs/spans");
  need(graphs->spanCount(0)==5&&graphs->spanCount(1)==5&&graphs->spanCount(2)==3,"MC exact5+3 placement spans");
  for(std::size_t i=0;i<8;++i)need(graphs->placement(i)==source.attachment(i).worldInstance,"MC true placement pointer");
  error.clear();return true;
 }catch(const std::exception& e){error=e.what();return false;}
}
// Negative numeric calls require no fake live renderer or random authority.
// The actual staged manager and actual process RNG are passed by the caller;
// invalid source index rejects before either mutable state is touched.
inline bool checkNumericRejection(const mc_particle_source50MC::Owner& source,
 rr::game::OriginalGcParticleManager50KG& manager,std::uint32_t& actualSharedRaw,std::string& error){
 rr::game::EffectControlState50BC controls{};std::int32_t frame=0;float size=0;const auto random=actualSharedRaw;const auto particles=manager.particleCount();
 const auto rejected=mc_type2_numeric50MC::emitAfterCommon(source,0,manager,1,controls,frame,size,.01f,actualSharedRaw);
 if(rejected.okay()||actualSharedRaw!=random||manager.particleCount()!=particles){error="MC subtype2 wrong-definition rejection mutated shared state";return false;}error.clear();return true;
}
}
