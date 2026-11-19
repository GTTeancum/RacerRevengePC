#pragma once
#include "mc_type2_numeric50mc.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "mc_particle_attachments50mc.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
namespace mc_particle_render50MC {class Owner;}
namespace mc_particle_runtime50MC {
namespace g=rr::game;namespace source=mc_particle_source50MC;
using Status=g::OriginalGcSprayStatus50KG;
struct Diagnostic {std::size_t initialized=0,ordinal=0,additionalPayloadReservation=0;std::string message;
 g::OriginalGcSprayReceipt50KG source{};g::OriginalGcParticleReceipt50KG manager{};g::OriginalGcEmitterRegistryReservation50KG registry{};
 std::uint32_t spawned=0,randomDraws=0,staleSkips=0;bool nativeUndefinedCarryPolicy=false;};
class Owner final {
 friend class ::mc_particle_render50MC::Owner;
 enum class Completion:std::uint8_t{Unavailable,Stale,Current};
 struct Record {const source::Definition* definition=nullptr;const mc_particle_authoring50MC::Source* descriptor=nullptr;
  g::EffectControlState50BC controls{};float delay1DC=0,fractional224=0,carriedSize18C=0;std::uint32_t duration1E4=0,managerOwnerId=0;
  std::int32_t firstFrame=0,lastFrame=0,currentFrame190=0;bool enabled=false;};
 std::shared_ptr<const source::Owner> source_;std::shared_ptr<mc_particle_attachments50MC::Owner> graphs_;
 std::shared_ptr<g::OriginalTrackParticleCold50LA> cold_;
 std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> process_;
 std::shared_ptr<native_gc_water_session50KG::Application> globals_;
 std::shared_ptr<g::HostMonotonicClock50ID> clock_;g::OriginalApplicationAnimationClockView50KG applicationClock_;
 std::array<Record,source::descriptorCount> records_{};std::size_t live_=0;g::OriginalGcParticleManager50KG* manager_=nullptr;
 const ::mc_particle_render50MC::Owner* renderBinding_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();std::size_t initialized_=0,registryQuota_=0;
 std::uint64_t generation_=0;bool attempted_=false,staged_=false,failed_=false,visiting_=false,rendering_=false,quiesced_=false,closed_=false;
 g::OriginalGcSprayReceipt50KG stoppedAt_{};
 static float word(std::uint32_t n){float f;std::memcpy(&f,&n,4);return f;}
 static void put(std::uint32_t& n,float f){std::memcpy(&n,&f,4);}
 bool fail(Diagnostic& d,const char* message,Status status,std::uint32_t pc)noexcept{
  if(!failed_){failed_=true;stoppedAt_.status=status;stoppedAt_.sourcePC=pc;}d.source=stoppedAt_;d.initialized=initialized_;try{d.message=message;}catch(...){}return false;}
 bool receipt(Diagnostic& d,const g::OriginalGcSprayReceipt50KG& r)noexcept {d.spawned+=r.spawned;d.randomDraws+=r.randomDraws;
  if(r.okay())return true;d.source=r;return fail(d,"MC original emitter prefix stopped",r.status,r.sourcePC);}
 bool receipt(Diagnostic& d,const g::OriginalGcFireEmissionReceipt50KG& r)noexcept{d.nativeUndefinedCarryPolicy|=r.nativeUndefinedCarryPolicy;return receipt(d,static_cast<const g::OriginalGcSprayReceipt50KG&>(r));}
 bool backing()const noexcept{return thread_==std::this_thread::get_id()&&!closed_&&source_&&source_->ready()&&graphs_&&graphs_->sourceGraphReady()&&
  graphs_->authoringOwner()==source_.get()&&graphs_->coldSource()==source_->coldSource()&&cold_&&cold_->prepared()&&manager_==cold_->particles()&&
  process_&&process_->known&&clock_&&process_->clock101A50==clock_&&globals_&&generation_&&
  &process_->rawRandom308030==&process_->sharedNoise308030.sharedRandomState&&&process_->rawRandom308030!=&process_->fireSerial433970&&
  manager_->emitterRegistryPayloadReservationBytes()==registryQuota_&&!manager_->emitterRegistryReservation().terminal;}
 static g::OriginalGcCommonEmitterDispatch50KG dispatch(Record& r)noexcept{return {&r,
  [](void* p,g::EffectControlState50BC& c){auto& r=*static_cast<Record*>(p);if(&r.controls!=&c)return false;g::startEffect50BC(c);return true;},
  [](void* p,g::EffectControlState50BC& c){auto& r=*static_cast<Record*>(p);if(&r.controls!=&c)return false;g::stopEffect50BC(c);return true;}};}
 bool advanceRecord(std::size_t i,float dt,Completion completion,std::uint32_t pause,Diagnostic& d)noexcept{
  auto& r=records_[i];const auto& f=*r.definition;const auto& desc=*r.descriptor;
  g::OriginalGcCommonAttachment50KG attached{};if(desc.attached){attached.graph=graphs_->graph(i);attached.node=static_cast<g::SceneNodeIndex49P>(0);
   for(unsigned k=0;k<3;++k)attached.offset50[k]=desc.authoredPositionOrOffset.v[k];}
  const auto calls=dispatch(r);const auto common=g::advanceOriginalGcCommonEmitter50KG(r.controls,f.timing,r.delay1DC,r.duration1E4,
   desc.attached,dt,pause!=0,process_->rawRandom308030,&calls,desc.attached?&attached:nullptr);
  if(!receipt(d,common))return false;if(common.commonSkipped)return true;
  if(f.kind==source::Kind::Sparks2){
   if(word(r.controls.word19C)>.25f){std::memcpy(r.controls.previous40,r.controls.position30,16);return true;}
   if(desc.attached&&completion==Completion::Unavailable)return fail(d,"MC attached Sparks2 needs genuine completed static placement",Status::unprepared,0x15d8ec);
   if(desc.attached&&completion==Completion::Stale){++d.staleSkips;r.carriedSize18C=f.initialSize184;r.controls.word19C=0;return true;}
   return receipt(d,mc_type2_numeric50MC::emitAfterCommon(*source_,source_->definitionIndexForDescriptor(i),*manager_,r.managerOwnerId,r.controls,r.currentFrame190,r.carriedSize18C,dt,process_->rawRandom308030));
  }
  g::water_wake_detail50jc::Math m;std::array<float,3> previous{},delta{},velocity{};
  for(unsigned k=0;k<3;++k){previous[k]=word(r.controls.previous40[k]);delta[k]=m.sub(word(r.controls.position30[k]),previous[k],0x15e128+8*k);}
  const auto inverse=m.div(1.f,dt,0x15e140);velocity[1]=m.mul(delta[1],inverse,0x15e160);velocity[0]=m.mul(delta[0],inverse,0x15e164);velocity[2]=m.mul(delta[2],inverse,0x15e16c);
  if(m.badPC)return fail(d,"MC Fire8 actual velocity arithmetic",Status::arithmetic,m.badPC);
  for(unsigned k=0;k<3;++k)put(r.controls.velocity60[k],velocity[k]);r.controls.velocity60[3]=0x3f800000;
  if(desc.attached&&completion==Completion::Unavailable)return fail(d,"MC attached Fire8 needs genuine completed static placement",Status::unprepared,0x15e198);
  if(desc.attached&&completion==Completion::Stale){++d.staleSkips;return true;}
  g::OriginalGcFireEmissionDefinition50KG emission{};emission.particleGroup=f.particleGroup1B8;emission.firstFrame=r.firstFrame;emission.lastFrame=r.lastFrame;
  emission.particleLife=f.particleLife17C;emission.initialSize=f.initialSize184;emission.growth=f.growth180;emission.emitRate=f.emitRate188;
  emission.framesPerSet=f.framesPerSet;emission.minRange=f.minRange;emission.maxRange=f.maxRange;emission.authoredVelocityY220=f.velocity60[1];
  emission.particleFlags=f.particleFlags198;emission.randomMirrorX=f.randomMirrorX;emission.randomMirrorY=f.randomMirrorY;emission.applySpreadToVelocity=f.applySpreadToVelocity;
  return receipt(d,g::emitOriginalGcFireParticles50KG(emission,*manager_,r.managerOwnerId,r.fractional224,previous,r.controls.previous40[3],velocity,dt,
   process_->rawRandom308030,process_->fireSerial433970,true));
 }
 // Only an actual concrete renderer can enter. No public completion flags or
 // callbacks can mint current/stale authority for the two retained placements.
 bool advance(float dt,const std::array<Completion,2>& completed,Diagnostic& d)noexcept{
  d={};if(!sourceStaged()||!renderBinding_||visiting_||rendering_||quiesced_)return fail(d,"MC particle serial publication unavailable",Status::unprepared,0x161180);
  visiting_=true;struct End{bool& b;~End(){b=false;}}end{visiting_};std::uint32_t pause=0;
  if(!applicationClock_.readPause11C(pause))return fail(d,"MC actual Application pause unavailable",Status::unprepared,0x161180);
  d.manager=manager_->advance(dt,pause!=0);if(!d.manager.okay())return fail(d,"MC shared manager update stopped",Status::storage,0x161180);
  for(std::size_t i=0;i<live_;++i){d.ordinal=i;const auto k=graphs_->placementIndex(i);
   if(!advanceRecord(i,dt,k<completed.size()?completed[k]:Completion::Unavailable,pause,d))return false;}
  d.initialized=initialized_;d.source.status=Status::complete;return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id()||visiting_||rendering_||renderBinding_||(cold_&&!cold_->closed()))std::terminate();}
 static constexpr std::size_t retainedPayloadBytes(){return sizeof(Owner);}
 bool prepare(std::shared_ptr<const source::Owner> src,std::shared_ptr<mc_particle_attachments50MC::Owner> graphs,
  std::shared_ptr<g::OriginalTrackParticleCold50LA> cold,std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> process,
  std::shared_ptr<native_gc_water_session50KG::Application> globals,g::OriginalApplicationAnimationClockView50KG clock,
  std::uint64_t generation,std::size_t reservedRegistryPeakBytes,std::size_t available,Diagnostic& d)noexcept{
  d={};if(thread_!=std::this_thread::get_id())return false;if(attempted_)return fail(d,"MC staging is one attempt",Status::alreadyPrepared,0x27d7a0);attempted_=true;
  std::uint32_t pause=0;if(!src||!src->ready()||!graphs||!graphs->sourceGraphReady()||graphs->authoringOwner()!=src.get()||
   !cold||!cold->prepared()||!process||!process->known||!process->clock101A50||!globals||!generation||!clock.readPause11C(pause)||std::fegetround()!=FE_TONEAREST)
   return fail(d,"MC actual source, graphs, process, particle globals and clock required",Status::unprepared,0x27d7a0);
  auto* manager=cold->particles();if(!manager||manager->emitterRegistryPayloadReservationBytes()||manager->emitterRegistryReservation().registeredEmitters)
   return fail(d,"MC untouched actual manager registry required",Status::storage,0x27d7a0);
  const auto& atlas=src->atlas();if(manager->frame(152)||manager->group(10)||!manager->atlasTextureMapName()||*manager->atlasTextureMapName()!=atlas.textureMap)
   return fail(d,"MC exact atlas extent",Status::assetIdentity,0x1618d0);
  for(std::size_t i=0;i<10;++i){const auto* actual=manager->group(i);const auto& wanted=atlas.groups[i];if(!actual||actual->type!=wanted.type||actual->first!=wanted.first||actual->last!=wanted.last)return fail(d,"MC actual atlas group identity",Status::assetIdentity,0x1618d0);}
  for(std::size_t i=0;i<152;++i){const auto* actual=manager->frame(i);if(!actual||actual->sourceUv!=atlas.frames[i].sourceUv)return fail(d,"MC actual atlas frame identity",Status::assetIdentity,0x161a0c);}
  const auto before=manager->emitterStoragePayloadBytes();if(reservedRegistryPeakBytes<before||reservedRegistryPeakBytes>32u*1024u*1024u||available<sizeof(Owner)||reservedRegistryPeakBytes-before>available-sizeof(Owner))
   return fail(d,"MC prepaid lifetime registry/owner budget",Status::capacity,0x27d7a0);
  d.additionalPayloadReservation=sizeof(Owner)+reservedRegistryPeakBytes-before;source_=std::move(src);graphs_=std::move(graphs);cold_=std::move(cold);process_=std::move(process);globals_=std::move(globals);
  clock_=process_->clock101A50;applicationClock_=std::move(clock);generation_=generation;registryQuota_=reservedRegistryPeakBytes;manager_=manager;
  live_=source_->emitterCount();
  if(manager_->reserveAdditionalEmitters(live_,registryQuota_,d.registry)!=g::OriginalGcParticleStatus50KG::complete)return fail(d,"MC guarded registry allocation",Status::capacity,0x27d7a0);
  for(std::size_t i=0;i<live_;++i){d.ordinal=i;auto& r=records_[i];const auto& desc=source_->descriptor(i);const auto& f=source_->definitionForDescriptor(i);const auto* group=manager_->group(f.particleGroup1B8);
   if(!group||group->type!=(f.kind==source::Kind::Sparks2?6:8)||!desc.effectiveStartOn||desc.attached!=(graphs_->placementIndex(i)<mc_particle_attachments50MC::placementCount))return fail(d,"MC original descriptor/type order",Status::assetIdentity,0x27c800);
   r.definition=&f;r.descriptor=&desc;r.controls.flags1D4=1;r.controls.position30[3]=r.controls.previous40[3]=r.controls.velocity60[3]=0x3f800000;
   for(unsigned k=0;k<3;++k){put(r.controls.velocity60[k],f.velocity60[k]);if(!desc.attached)put(r.controls.position30[k],desc.authoredPositionOrOffset.v[k]);}
   std::memcpy(r.controls.previous40,r.controls.position30,16);r.delay1DC=f.timing.delayTime1E0;put(r.duration1E4,f.timing.maxTimeOn1EC);r.carriedSize18C=0;
   r.firstFrame=r.currentFrame190=group->first;r.lastFrame=group->last;
   g::OriginalGcParticleEmitterState50KG row{};row.particleGroup=f.particleGroup1B8;row.framesPerSet=f.framesPerSet;row.inverseFramesPerSet=f.inverseFramesPerSet;row.gravity=f.gravityMultiplier;row.bouncePlane=f.groundLevel;row.bounceFactor=f.restitution;
   r.managerOwnerId=manager_->registerEmitter(row);if(r.managerOwnerId!=i+1)return fail(d,"MC actual source-ordered registry insertion",Status::allocation,0x15dd98);
   g::startEffect50BC(r.controls);r.enabled=true;++initialized_;
  }
  d.registry=manager_->emitterRegistryReservation();if(d.registry.terminal||!d.registry.allocationBound||d.registry.registeredEmitters!=live_)return fail(d,"MC complete emitter registry rejected",Status::storage,0x27d7a0);
  staged_=true;d.initialized=initialized_;d.source.status=Status::complete;return true;
 }
 bool sourceStaged()const noexcept{return backing()&&staged_&&!failed_&&!quiesced_&&live_&&initialized_==live_;}
 std::size_t emitterCount()const noexcept{return live_;}
 const source::Owner* sourceOwner()const noexcept{return backing()?source_.get():nullptr;}
 const mc_particle_attachments50MC::Owner* attachmentOwner()const noexcept{return backing()?graphs_.get():nullptr;}
 const g::OriginalTrackParticleCold50LA* coldOwner()const noexcept{return cold_.get();}
 std::size_t initializedAssociations()const noexcept{return initialized_;}
 bool quiesce()noexcept{if(thread_!=std::this_thread::get_id()||visiting_||rendering_||renderBinding_)return false;quiesced_=true;staged_=false;return true;}
 bool closeAfterPool()noexcept{if(thread_!=std::this_thread::get_id()||visiting_||rendering_||renderBinding_||!quiesced_||(cold_&&!cold_->closed()))return false;
  records_={};manager_=nullptr;source_.reset();graphs_.reset();cold_.reset();process_.reset();globals_.reset();clock_.reset();applicationClock_={};closed_=true;return true;}
};
}
