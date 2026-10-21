#pragma once
#include "gb_type11_numeric50gb.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/game/original_gc_fire_emission50kg.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../platform/linux/native/original_ui_main_session50kg.h"
#include <array>
#include <cfenv>
#include <limits>
#include <memory>
#include <thread>

namespace gb_particle_render50GB {class Owner;}
namespace gb_trigger_actions50GB {class Owner;}
namespace gb_particle_runtime50GB {
namespace g=rr::game;namespace source=gb_particle_authoring50GB;
using Status=g::OriginalGcSprayStatus50KG;
inline constexpr std::size_t fireCount=62,sparksCount=5;
struct Diagnostic {
 std::size_t association=0,initialized=0,registryPayloadBefore=0,registryQuota=0;
 std::size_t additionalPayloadReservation=0;
 g::OriginalGcEmitterRegistryReservation50KG registry{};
 g::OriginalGcSprayReceipt50KG source{};
 g::OriginalGcParticleReceipt50KG manager{};
 std::uint32_t spawned=0,randomDraws=0,staleFireSkips=0;
 bool nativeUndefinedCarryPolicy=false;std::string message;
};
// The entire authentic source-ordered detached record bank. Source staging
// creates all 67 concrete numerical/control owners against the ONE actual
// manager. It does not issue renderer, ActionOwner, query or root readiness.
class Owner final {
 friend class ::gb_particle_render50GB::Owner;
 friend class ::gb_trigger_actions50GB::Owner;
 using Spark=gb_type11_numeric50GB::Instance;
 using Completion=Spark::PlacementCompletion;
 struct Fire {
  const source::Descriptor* descriptor=nullptr;
  const source::Attachment* association=nullptr;
  const source::Definition* definition=nullptr;
  g::EffectControlState50BC controls{};
  float delay1DC=0,fractional224=0;std::uint32_t duration1E4=0,managerOwnerId=0;
  std::int32_t firstFrame=0,lastFrame=0;
  bool bound=false,enabled=false;
 };
 // All backing owners precede the inline views and all five numeric owners.
 std::shared_ptr<const source::Owner> source_;
 std::shared_ptr<gb_particle_attachments50GB::Owner> graphs_;
 std::shared_ptr<g::OriginalTrackParticleCold50LA> cold_;
 std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> process_;
 std::shared_ptr<native_gc_water_session50KG::Application> particleGlobals_;
 std::shared_ptr<g::HostMonotonicClock50ID> clock_;
 g::OriginalApplicationAnimationClockView50KG applicationClock_;
 std::array<Fire,fireCount> fires_{};
 std::array<Spark,sparksCount> sparks_{};
 g::OriginalGcParticleManager50KG* manager_=nullptr;
 const ::gb_trigger_actions50GB::Owner* actions_=nullptr;
 const ::gb_particle_render50GB::Owner* renderBinding_=nullptr;
 g::NativeTriggerGroup50GB* group_=nullptr;
 std::size_t initialized_=0,actionBindings_=0,registryBefore_=0,registryQuota_=0;
 std::uint64_t generation_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,staged_=false,failed_=false,visiting_=false,rendering_=false,quiesced_=false,closed_=false;
 g::OriginalGcSprayReceipt50KG stoppedAt_{};
 static float word(std::uint32_t x)noexcept {float f;std::memcpy(&f,&x,4);return f;}
 static void put(std::uint32_t& out,float f)noexcept {std::memcpy(&out,&f,4);}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 bool fail(Diagnostic& d,const char* message,Status status,std::uint32_t pc)noexcept {
  d.initialized=initialized_;
  if(!failed_){failed_=true;d.source.status=status;d.source.sourcePC=pc;stoppedAt_=d.source;}
  else d.source=stoppedAt_;
  try{d.message=message;}catch(...){}return false;
 }
 bool failedReceipt(Diagnostic& d,const g::OriginalGcSprayReceipt50KG& r,const char* message)noexcept {
  d.source=r;return fail(d,message,r.status,r.sourcePC);
 }
 bool backingCurrent()const noexcept {
  return thread()&&!closed_&&source_&&source_->ready()&&graphs_&&graphs_->sourceGraphReady()&&
   graphs_->authoringOwner()==source_.get()&&graphs_->coldSource()==source_->sourceOwner()&&
   cold_&&cold_->prepared()&&manager_&&cold_->particles()==manager_&&
   process_&&process_->known&&clock_&&process_->clock101A50==clock_&&particleGlobals_&&
   &process_->rawRandom308030==&process_->sharedNoise308030.sharedRandomState&&
   &process_->rawRandom308030!=&process_->fireSerial433970&&generation_&&
   manager_->emitterRegistryPayloadReservationBytes()==registryQuota_&&registryQuota_&&
   !manager_->emitterRegistryReservation().terminal;
 }
 bool fireCurrent(std::size_t i)const noexcept {
  if(!backingCurrent()||i>=fires_.size())return false;const auto& c=fires_[i];
  const auto& a=source_->attachment(i);const auto& d=source_->descriptor(a.descriptor);
  return c.bound&&c.enabled&&c.association==&a&&c.descriptor==&d&&
   c.definition==&source_->definition(d.definitionIndex)&&d.definitionIndex<2&&
   d.kind==source::Kind::Fire8&&d.effectiveStartOn==1&&
   graphs_->placement(i)==a.worldInstance&&graphs_->graph(i)&&manager_->emitter(c.managerOwnerId);
 }
 static g::OriginalGcCommonEmitterDispatch50KG fireDispatch(Fire& child)noexcept {
  return {&child,[](void* p,g::EffectControlState50BC& c){auto& x=*static_cast<Fire*>(p);
    if(&c!=&x.controls||!x.definition||x.definition->timing.mode1D8)return false;g::startEffect50BC(c);return true;},
   [](void* p,g::EffectControlState50BC& c){auto& x=*static_cast<Fire*>(p);
    if(&c!=&x.controls||!x.definition||x.definition->timing.mode1D8)return false;g::stopEffect50BC(c);return true;}};
 }
 bool initializeFire(std::size_t i,Diagnostic& d)noexcept {
  const auto& a=source_->attachment(i);const auto& desc=source_->descriptor(a.descriptor);
  const auto& f=source_->definition(desc.definitionIndex);auto& c=fires_[i];
  const auto* group=manager_->group(f.particleGroup1B8);
  if(desc.kind!=source::Kind::Fire8||desc.definitionIndex>=2||desc.effectiveStartOn!=1||
     !f.parsed||f.kind!=source::Kind::Fire8||f.timing.mode1D8||f.timing.delayTime1E0||
     !group||group->type!=8||group->first!=52||group->last!=75||
     graphs_->placement(i)!=a.worldInstance||!graphs_->graph(i))
   return fail(d,"GB actual Fire8 definition/association rejected",Status::assetIdentity,0x15e6e0u);
  c.descriptor=&desc;c.association=&a;c.definition=&f;c.controls={};c.controls.flags1D4=1;
  c.controls.position30[3]=c.controls.previous40[3]=c.controls.velocity60[3]=0x3f800000u;
  for(unsigned k=0;k<3;++k)put(c.controls.velocity60[k],f.velocity60[k]);
  c.delay1DC=f.timing.delayTime1E0;put(c.duration1E4,f.timing.maxTimeOn1EC);
  c.firstFrame=group->first;c.lastFrame=group->last;c.fractional224=0;
  g::OriginalGcParticleEmitterState50KG row{};row.particleGroup=f.particleGroup1B8;
  row.framesPerSet=f.framesPerSet;row.inverseFramesPerSet=f.inverseFramesPerSet;
  row.gravity=f.gravityMultiplier;row.bouncePlane=f.groundLevel;row.bounceFactor=f.restitution;
  c.managerOwnerId=manager_->registerEmitter(row);
  if(!c.managerOwnerId)return fail(d,"GB real Fire8 registry insertion failed",Status::allocation,0x15e830u);
  c.bound=true;
  if(c.managerOwnerId!=i+1)return fail(d,"GB Fire8 registry source order changed",Status::storage,0x15e830u);
  // Source attachment construction preserves the constructor position.
  // Wrapper true StartOn reaches start80 then native detached enable20.
  g::startEffect50BC(c.controls);c.enabled=true;return true;
 }
 bool advanceFire(std::size_t i,float dt,Completion completion,Diagnostic& d)noexcept {
  if(!fireCurrent(i))return fail(d,"GB Fire8 backing changed",Status::storage,0x15e0c0u);
  auto& c=fires_[i];const auto& f=*c.definition;std::uint32_t pause=0;
  if(!applicationClock_.readPause11C(pause))return fail(d,"GB actual Application pause unavailable",Status::unprepared,0x15a7a0u);
  const g::OriginalGcCommonAttachment50KG attached{graphs_->graph(i),static_cast<g::SceneNodeIndex49P>(0),c.descriptor->offset};
  const auto calls=fireDispatch(c);
  const auto common=g::advanceOriginalGcCommonEmitter50KG(c.controls,f.timing,c.delay1DC,c.duration1E4,
   true,dt,pause!=0,process_->rawRandom308030,&calls,&attached);
  d.randomDraws+=common.randomDraws;
  if(!common.okay())return failedReceipt(d,common,"GB Fire8 common source prefix stopped");
  if(common.commonSkipped)return true;
  g::water_wake_detail50jc::Math m;std::array<float,3> previous{},delta{},velocity{};
  for(unsigned k=0;k<3;++k){previous[k]=word(c.controls.previous40[k]);
   delta[k]=m.sub(word(c.controls.position30[k]),previous[k],0x15e128u+8u*k);}
  const auto inverse=m.div(1.f,dt,0x15e140u);
  velocity[1]=m.mul(delta[1],inverse,0x15e160u);velocity[0]=m.mul(delta[0],inverse,0x15e164u);
  velocity[2]=m.mul(delta[2],inverse,0x15e16cu);
  if(m.badPC)return fail(d,"GB Fire8 actual attachment velocity stopped",Status::arithmetic,m.badPC);
  for(unsigned k=0;k<3;++k)put(c.controls.velocity60[k],velocity[k]);c.controls.velocity60[3]=0x3f800000u;
  if(completion==Completion::Unavailable)return fail(d,"GB genuine completed-placement binding unavailable",Status::unprepared,0x15e198u);
  if(completion==Completion::Stale){++d.staleFireSkips;return true;}
  if(completion!=Completion::Current)return fail(d,"GB invalid private placement state",Status::unprepared,0x15e198u);
  g::OriginalGcFireEmissionDefinition50KG emission;
  emission.particleGroup=f.particleGroup1B8;emission.firstFrame=c.firstFrame;emission.lastFrame=c.lastFrame;
  emission.particleLife=f.particleLife17C;emission.initialSize=f.initialSize184;emission.growth=f.growth180;
  emission.emitRate=f.emitRate188;emission.framesPerSet=f.framesPerSet;emission.minRange=f.minRange;emission.maxRange=f.maxRange;
  emission.authoredVelocityY220=f.velocity60[1];emission.particleFlags=f.particleFlags198;
  emission.randomMirrorX=f.randomMirrorX;emission.randomMirrorY=f.randomMirrorY;emission.applySpreadToVelocity=f.applySpreadToVelocity;
  const auto r=g::emitOriginalGcFireParticles50KG(emission,*manager_,c.managerOwnerId,c.fractional224,
   previous,c.controls.previous40[3],velocity,dt,process_->rawRandom308030,process_->fireSerial433970,true);
  d.spawned+=r.spawned;d.randomDraws+=r.randomDraws;
  d.nativeUndefinedCarryPolicy=d.nativeUndefinedCarryPolicy||r.nativeUndefinedCarryPolicy;
  return r.okay()||failedReceipt(d,r,"GB Fire8 emission source prefix stopped");
 }
 // No public completion receipt, binding factory or schedule entry exists.
 // A future concrete typed renderer binding must prove its own retained
 // same-world/same-window publication before entering this private method.
 // Its 55 unique placement results are mapped to all 67 actual associations.
 bool advance(float dt,const std::array<Completion,gb_particle_attachments50GB::placementCount>& completed,
  Diagnostic& d)noexcept {
  d={};
  if(!thread()){d.source.status=Status::unprepared;d.source.sourcePC=0x161180u;return false;}
  if(failed_){d.source=stoppedAt_;d.initialized=initialized_;return false;}
  if(!sourceStaged()||!renderBinding_||visiting_||rendering_||quiesced_||(group_&&(group_->queryActive||group_->actionsFailed)))
   return fail(d,"GB scheduler is outside its admitted serial lifetime",Status::unprepared,0x161180u);
  visiting_=true;struct End{bool& v;~End(){v=false;}}end{visiting_};
  std::uint32_t pause=0;if(!applicationClock_.readPause11C(pause))
   return fail(d,"GB actual Application pause unavailable",Status::unprepared,0x161180u);
  d.manager=manager_->advance(dt,pause!=0);
  if(!d.manager.okay())return fail(d,"GB shared particle manager source prefix stopped",Status::storage,0x161180u);
  for(std::size_t i=0;i<source::attachmentCount;++i){d.association=i;
   const auto placement=graphs_->placementIndex(i);
   if(placement>=completed.size())return fail(d,"GB source association lost its placement",Status::storage,0x27dc60u);
   if(i<fireCount){if(!advanceFire(i,dt,completed[placement],d))return false;}
   else {
    if(!applicationClock_.readPause11C(pause))
     return fail(d,"GB actual Type11 Application pause unavailable",Status::unprepared,0x15a7a0u);
    const auto r=sparks_[i-fireCount].advance(dt,pause!=0,completed[placement]);
    d.spawned+=r.spawned;d.randomDraws+=r.randomDraws;
    if(!r.okay())return failedReceipt(d,r,"GB Type11 source prefix stopped");
   }
  }
  d.initialized=initialized_;d.source.status=Status::complete;return true;
 }
 bool bindActionSource(const ::gb_trigger_actions50GB::Owner& actions,g::NativeTriggerGroup50GB& group,
  std::shared_ptr<const gb_trigger_authoring50GB::Owner> triggers)noexcept {
  if(!sourceStaged()||quiesced_||visiting_||rendering_||actions_||group_||actionBindings_||!triggers||
     triggers->coldSource()!=source_->sourceOwner()||group.ready||group.queryActive||group.actionsFailed)return false;
  // Partial accepted bindings remain owned for explicit fail-stop cleanup.
  actions_=&actions;group_=&group;
  for(auto& s:sparks_){if(!s.bindActionSource(actions,group,triggers)){
    failed_=true;stoppedAt_.status=Status::unprepared;stoppedAt_.sourcePC=0x15a500u;return false;}++actionBindings_;}
  return true;
 }
 bool unbindActionSource(const ::gb_trigger_actions50GB::Owner& actions)noexcept {
  if(!thread()||visiting_||rendering_||actions_!=&actions||!group_||group_->queryActive)return false;
  while(actionBindings_){if(!sparks_[actionBindings_-1].unbindActionSource(actions))return false;--actionBindings_;}
  actions_=nullptr;group_=nullptr;return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||visiting_||rendering_||renderBinding_||actions_||actionBindings_||(cold_&&!cold_->closed()))std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 // Caller reserves Owner allocation/control block first, and reserves the
 // lifetime registry quota from the SAME shared 32 MiB ledger before entry.
 // An already charged registry block is subtracted once, never charged twice.
 bool prepare(std::shared_ptr<const source::Owner> authored,
  std::shared_ptr<gb_particle_attachments50GB::Owner> graphs,
  std::shared_ptr<g::OriginalTrackParticleCold50LA> actualCold,
  std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> actualMainProcess,
  std::shared_ptr<native_gc_water_session50KG::Application> actualParticleGlobals,
  g::OriginalApplicationAnimationClockView50KG actualApplicationClock,
  std::uint64_t generation,std::size_t reservedRegistryPeakBytes,
  std::size_t availableAdditionalPayloadBytes,Diagnostic& d)noexcept {
  d={};
  if(!thread())return false;
  if(attempted_)return fail(d,"GB source staging is one attempt",Status::alreadyPrepared,0x27d7a0u);
  attempted_=true;std::uint32_t pause=0;
  if(!authored||!authored->ready()||!graphs||!graphs->sourceGraphReady()||
     graphs->authoringOwner()!=authored.get()||graphs->coldSource()!=authored->sourceOwner()||
     !actualCold||!actualCold->prepared()||!actualMainProcess||!actualMainProcess->known||
     !actualMainProcess->clock101A50||!actualParticleGlobals||!generation||
     &actualMainProcess->rawRandom308030!=&actualMainProcess->sharedNoise308030.sharedRandomState||
     !actualApplicationClock.readPause11C(pause)||std::fegetround()!=FE_TONEAREST)
   return fail(d,"GB requires actual Main process, shared particle globals, clock and source owners",Status::unprepared,0x27d7a0u);
  auto* manager=actualCold->particles();
  if(!manager||manager->emitterRegistryPayloadReservationBytes()||
     manager->emitterRegistryReservation().registeredEmitters||!sparks_[0].atlasMatches(*authored,*manager))
   return fail(d,"GB manager registry reservation is missing or already claimed",Status::storage,0x27d7a0u);
  registryBefore_=manager->emitterStoragePayloadBytes();d.registryPayloadBefore=registryBefore_;
  if(!reservedRegistryPeakBytes||reservedRegistryPeakBytes<registryBefore_||reservedRegistryPeakBytes>32u*1024u*1024u||
     availableAdditionalPayloadBytes>32u*1024u*1024u||availableAdditionalPayloadBytes<sizeof(Owner)||
     reservedRegistryPeakBytes-registryBefore_>availableAdditionalPayloadBytes-sizeof(Owner))
   return fail(d,"GB owner and prepaid registry peak exceed the shared reservation",Status::capacity,0x27d7a0u);
  d.registryQuota=reservedRegistryPeakBytes;
  d.additionalPayloadReservation=sizeof(Owner)+reservedRegistryPeakBytes-registryBefore_;
  source_=std::move(authored);graphs_=std::move(graphs);cold_=std::move(actualCold);
  process_=std::move(actualMainProcess);particleGlobals_=std::move(actualParticleGlobals);
  clock_=process_->clock101A50;applicationClock_=std::move(actualApplicationClock);manager_=manager;
  generation_=generation;registryQuota_=reservedRegistryPeakBytes;
  // Actual allocator quota is installed BEFORE reserve allocates. Its guard
  // persists for every later registerEmitter, including real racer children.
  const auto reserved=manager_->reserveAdditionalEmitters(source::attachmentCount,registryQuota_,d.registry);
  if(reserved!=g::OriginalGcParticleStatus50KG::complete)
   return fail(d,"GB actual guarded registry reservation failed",Status::capacity,0x27d7a0u);
  if(!d.registry.allocationBound||d.registry.terminal||d.registry.reservedPeakPayloadBytes!=registryQuota_||
     d.registry.registeredEmitters>std::numeric_limits<std::size_t>::max()-source::attachmentCount||
     d.registry.reservedEmitters<d.registry.registeredEmitters+source::attachmentCount)
   return fail(d,"GB actual registry did not reserve all 67 source rows",Status::storage,0x27d7a0u);
  for(std::size_t i=0;i<source::attachmentCount;++i){d.association=i;
   if(i<fireCount){if(!initializeFire(i,d))return false;}
   else {
    const auto r=sparks_[i-fireCount].initialize(source_,i,graphs_,cold_,process_->rawRandom308030,
      particleGlobals_->sparksGlobals,generation_,sizeof(Spark));
    if(!r.okay())return failedReceipt(d,r,"GB actual Type11 initialization failed");
    if(sparks_[i-fireCount].managerOwnerId_!=i+1)
     return fail(d,"GB Type11 registry source order changed",Status::storage,0x15fd6cu);
   }
   ++initialized_;
  }
  d.registry=manager_->emitterRegistryReservation();
  if(d.registry.terminal||!d.registry.allocationBound||d.registry.registeredEmitters!=source::attachmentCount||
     d.registry.reservedPeakPayloadBytes!=registryQuota_)
   return fail(d,"GB complete source registry changed during staging",Status::storage,0x27d7a0u);
  staged_=true;d.initialized=initialized_;d.source.status=Status::complete;return true;
 }
 bool sourceStaged()const noexcept {
  if(!backingCurrent()||!staged_||failed_||quiesced_||initialized_!=source::attachmentCount||
     (group_&&group_->actionsFailed))return false;
  for(const auto& s:sparks_)if(s.failed_)return false;
  return true;
 }
 bool failed()const noexcept{return failed_;}
 const source::Owner* sourceOwner()const noexcept{return backingCurrent()?source_.get():nullptr;}
 const gb_particle_attachments50GB::Owner* attachmentOwner()const noexcept{return backingCurrent()?graphs_.get():nullptr;}
 const rr::platform::OriginalUiMainApplication50KG* mainProcessOwner()const noexcept{return process_.get();}
 const native_gc_water_session50KG::Application* sharedParticleGlobalsOwner()const noexcept{return particleGlobals_.get();}
 const g::OriginalTrackParticleCold50LA* coldOwner()const noexcept{return cold_.get();}
 std::size_t initializedAssociations()const noexcept{return initialized_;}
 std::size_t registryPayloadReservationBytes()const noexcept{return registryQuota_;}
 bool quiesce()noexcept {
  if(!thread()||visiting_||rendering_||renderBinding_||(group_&&group_->queryActive))return false;
  for(auto& s:sparks_)if(!s.quiesce())return false;
  quiesced_=true;staged_=false;return true;
 }
 bool closeAfterPool()noexcept {
  if(!thread()||visiting_||rendering_||renderBinding_||!quiesced_||actions_||actionBindings_||(cold_&&!cold_->closed()))return false;
  for(auto& s:sparks_)if(!s.retireAfterPool())return false;
  fires_={};manager_=nullptr;source_.reset();graphs_.reset();cold_.reset();
  process_.reset();particleGlobals_.reset();clock_.reset();applicationClock_={};
  staged_=false;closed_=true;return true;
 }
};
} // namespace gb_particle_runtime50GB
