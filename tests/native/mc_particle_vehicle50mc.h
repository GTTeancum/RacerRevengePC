#pragma once
#include "mc_particle_source50mc.h"
#include "mc_water50mc.h"
#include "native_gc_other_particle_children50kg_fixture.h"
#include "native_gc_water_racer50kg_fixture.h"
#include "../../src/game/original_track_particle_cold50la.h"
namespace mc_particle_vehicle50MC {
namespace g=rr::game;namespace child=native_gc_other_particle_children50KG;
// Strongly retained actual eight-racer producer bank. Canonical50BE controls
// stay in the real racer Bank; no pointer to a transient visited frame survives.
class Owner final {
 std::shared_ptr<const mc_particle_source50MC::Owner> source_;std::shared_ptr<mc_water50MC::Owner> water_;
 std::shared_ptr<g::OriginalTrackParticleCold50LA> cold_;child::SharedAuthoring authored_{};
 std::array<child::Runtime,8> children_{};std::array<native_gc_water_racer50KG::Owner,8> racers_{};
 std::vector<std::uint8_t> spray_;std::size_t initialized_=0,registered_=0;bool attempted_=false,ready_=false,failed_=false;
 const std::thread::id thread_=std::this_thread::get_id();
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 static constexpr std::size_t maximumAdditionalPayloadBytes(){return sizeof(Owner)+1027+128;}
 bool prepare(std::shared_ptr<const mc_particle_source50MC::Owner> source,std::shared_ptr<mc_water50MC::Owner> water,
  std::shared_ptr<g::OriginalTrackParticleCold50LA> cold,const std::array<std::vector<std::uint8_t>,6>& authenticatedEffects,
  std::vector<std::uint8_t> originalSelectedPodSpray,std::size_t available,std::string& error){
  if(thread_!=std::this_thread::get_id()||attempted_){error="MC racer particle prepare repeated/wrong thread";return false;}attempted_=true;
  if(!source||!source->ready()||!water||!water->ready()||water->sourceIdentity()!=source->coldSource()||!cold||!cold->prepared()||available<maximumAdditionalPayloadBytes()){
   error="MC racer particles require same actual source/water/pool/budget";return false;}
  // Each six-member parser authenticates its original retail EMT. The spray
  // parser independently authenticates1027 original bytes before retention.
  g::OriginalGcSprayDefinition50KG spray;rr::assets::OriginalUiPageDiagnostic50KG d;
  if(g::readOriginalGcSprayDefinition50KG(originalSelectedPodSpray,spray,d)!=g::OriginalGcSprayStatus50KG::complete||!authored_.parse(authenticatedEffects).okay()){
   error="MC actual selected POD particle definition rejected";return false;}
  source_=std::move(source);water_=std::move(water);cold_=std::move(cold);spray_=std::move(originalSelectedPodSpray);ready_=true;return true;
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&!failed_&&source_&&source_->ready()&&water_&&water_->ready()&&water_->sourceIdentity()==source_->coldSource()&&cold_&&cold_->prepared();}
 bool initializeRacer(std::size_t slot,g::EffectOwner50BE* effects,std::size_t effectCount,bool bottomPair,
  g::VehicleFrameState49W& frame,g::VehicleFrameBindings49W& bindings,g::VehicleFrameDiagnostic49W& diagnostic,
  const float (&actualStep)[4],std::uint32_t& actualRandom,std::string& error){
  if(!ready()||slot!=initialized_||slot>=8){error="MC racer producer ordinal/lifetime";return false;}
  auto& manager=*cold_->particles();const auto before=manager.emitterRegistryReservation().registeredEmitters;
  if(before!=40+registered_){error="MC unknown producer before source constructor";failed_=true;return false;}
  auto& children=children_[slot];auto& racer=racers_[slot];
  const auto stop=[&](const char* message){error=message;failed_=true;return false;};
  if(!children.bindCold(authored_,effects,effectCount,bottomPair,manager).okay())return stop("MC actual preceding14/8 particle constructors rejected");
  if(!racer.constructSprays(water_->runtime(),manager,0xf6000001u+slot,0xf7000001u,0xf8000001u+slot,actualStep,spray_,actualRandom).okay())return stop("MC actual subsequent two-spray constructor rejected");
  if(!racer.finishConstructor(frame,bindings,diagnostic).okay())return stop("MC actual334/BE0 constructor join rejected");
  registered_+=effectCount+2;++initialized_;error.clear();return true;
 }
 native_gc_water_racer50KG::Owner* racer(std::size_t i)noexcept{return ready()&&i<initialized_?&racers_[i]:nullptr;}
 bool advanceChildren(std::size_t i,g::EffectOwner50BE* effects,std::size_t count,std::uint64_t sequence,float dt,bool paused,
  std::uint32_t& random,g::SharedRandomOwner50ID& selectors,g::HostMonotonicClock50ID& clock,
  std::uint32_t& fireSerial,g::OriginalGcPodSparksGlobals50KG& sparkGlobals)noexcept{
  if(!ready()||i>=initialized_)return false;
  if(!children_[i].advanceChildren(authored_,effects,count,sequence,dt,paused,random,selectors,clock,fireSerial,sparkGlobals).okay()||
     !racers_[i].sprayChildren(sequence,dt,paused,random,selectors,clock).okay()||!children_[i].lateChildrenGate().okay()){failed_=true;return false;}return true;
 }
 std::size_t registeredProducerCount()const noexcept{return ready()?registered_:0;}
 std::size_t initializedRacers()const noexcept{return ready()?initialized_:0;}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?source_->coldSource():nullptr;}
 const g::OriginalTrackParticleCold50LA* poolOwner()const noexcept{return ready()?cold_.get():nullptr;}
 bool matchesParticle(const g::OriginalGcParticle50KG& p)const noexcept{
  if(!ready()||p.ownerId<=40)return false;const auto& manager=*cold_->particles();
  const auto match=[&](std::uint32_t id,std::int32_t group,std::uint32_t flags,float frames,float gravity,float ground,float restitution){
   if(id!=p.ownerId)return false;const auto* row=manager.emitter(id);const auto* range=manager.group(group);
   return row&&range&&row->particleGroup==group&&row->framesPerSet==frames&&row->gravity==gravity&&row->bouncePlane==ground&&row->bounceFactor==restitution&&
    p.frame>=range->first&&p.frame<=range->last&&(p.flags&~0xc000u)==flags;};
  for(std::size_t slot=0;slot<initialized_;++slot){const auto& c=children_[slot];if(!c.bound||c.failed||c.manager!=&manager)return false;
   for(std::size_t i=0;i<c.ownerCount;++i){const auto source=child::sourceMember[i];
    if(source<4){const auto& f=authored_.dust[source];const auto& r=c.dust[i];if(r.bound&&!r.failed&&r.managerIdentity==&manager&&match(r.managerOwnerId,f.particleGroup1B8,f.particleFlags198,f.framesPerSet,f.gravityMultiplier,f.groundLevel,f.restitution))return true;}
    else if(source==4){const auto& f=authored_.sparks;const auto& r=c.sparks[i];if(r.bound&&!r.failed&&r.manager==&manager&&match(r.ownerId,f.group,f.flags,f.framesPerSet,f.gravity,f.groundLevel,f.restitution))return true;}
    else{const auto& f=authored_.fire;const auto& r=c.fire[i];if(r.manager==&manager&&match(r.managerOwnerId,f.emission.particleGroup,f.emission.particleFlags,f.emission.framesPerSet,f.gravityMultiplier,f.groundLevel,f.restitution))return true;}
   }
   for(unsigned side=0;side<2;++side){const auto& spray=const_cast<native_gc_water_racer50KG::Owner&>(racers_[slot]).spray(side);const auto& f=spray.definition();
    if(spray.loaded()&&match(spray.ownerId(),f.particleGroup,f.particleFlags,f.framesPerSet,f.gravityMultiplier,f.groundLevel,f.restitution))return true;}
  }return false;
 }
};
}
