#pragma once
#include "rc_particle_attachments50rc.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include "../../src/game/original_gc_fire_emission50kg.h"
#include "../../src/game/original_smoke_numeric50rc.h"
#include "../../src/game/original_rc_fog_numeric50rc.h"
#include <cfenv>
#include <memory>
#include <thread>

namespace rc_particle_render50RC {class Owner;}
namespace rc_particle_runtime50RC {
namespace g=rr::game;namespace a=rr::assets;namespace authored=rc_particle_authoring50RC;
using Kind=authored::Kind;using Status=g::OriginalGcSprayStatus50KG;
struct Diagnostic {
 std::string message;std::size_t child=0,activatedChildren=0,sharedEmitterPayloadBytes=0,additionalRetainedPayloadBytes=0;
 g::OriginalGcSprayReceipt50KG source{};g::OriginalGcParticleReceipt50KG manager{};
 std::uint32_t spawned=0,randomDraws=0,staleFireSkips=0;
 bool nativeUndefinedCarryPolicy=false;
};
// Explicit bounded native ordered scheduler. Ten authentic subtype states,
// no invented Entity list, parent, constructor counter or component traversal.
// The actual shared manager's own counted cold owner remains unchanged.
class Owner final {
 friend class ::rc_particle_render50RC::Owner;
 struct Child {
  Kind kind=Kind::Smoke0;g::EffectControlState50BC controls{};
  float delay1DC=0,remainder200=0,fractional224=0;std::uint32_t duration1E4=0,managerOwnerId=0;
  std::int32_t currentFrame190=0,firstFrame1B0=0,lastFrame1B4=0;
  bool failed=false,enabled=false;g::OriginalGcSprayReceipt50KG stoppedAt{};
 };
 std::shared_ptr<const authored::Owner> source_;
 std::shared_ptr<rc_particle_attachments50RC::Owner> attachments_;
 std::shared_ptr<g::OriginalTrackParticleCold50LA> cold_;
 g::OriginalGcParticleManager50KG* manager_=nullptr;
 std::array<Child,authored::attachmentCount> children_{};
 std::uint32_t *raw_=nullptr,*fireSerial_=nullptr;
 g::OriginalApplicationAnimationClockView50KG applicationAnimation_;
 g::SharedRandomOwner50ID* shared_=nullptr;g::HostMonotonicClock50ID* clock_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,bound_=false,activationAttempted_=false,active_=false,visiting_=false,rendering_=false,terminal_=false,quiesced_=false,closed_=false;
 std::size_t activatedChildren_=0,initialEmitterPayload_=0,additionalBudget_=0;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static float bits(std::uint32_t u)noexcept{float f;std::memcpy(&f,&u,4);return f;}
 static void put(std::uint32_t& word,float value)noexcept{std::memcpy(&word,&value,4);}
 bool current()const noexcept{return thread()&&bound_&&!closed_&&source_&&source_->ready()&&attachments_&&attachments_->ready()&&
  attachments_->authoringOwner()==source_.get()&&cold_&&cold_->prepared()&&manager_&&cold_->particles()==manager_;}
 bool fail(Diagnostic& d,const char* message,Status status,std::uint32_t pc)noexcept {
  terminal_=true;d.source.status=status;d.source.sourcePC=pc;d.activatedChildren=activatedChildren_;
  try{d.message=message;}catch(...){}return false;
 }
 bool failedReceipt(Diagnostic& d,const g::OriginalGcSprayReceipt50KG& r,const char* message)noexcept {
  d.source=r;return fail(d,message,r.status,r.sourcePC);
 }
 static g::OriginalGcCommonEmitterDispatch50KG dispatch(Child& child)noexcept {
  return {&child,[](void* context,g::EffectControlState50BC& controls){auto& c=*static_cast<Child*>(context);
    if(&controls!=&c.controls)return false;g::startEffect50BC(controls);return true;},
   [](void* context,g::EffectControlState50BC& controls){auto& c=*static_cast<Child*>(context);
    if(&controls!=&c.controls)return false;
    // All authenticated modes are0. Fog160490 then takes exactly the same
    // control suffix; an unproved mode1 stop is never admitted by prepare.
    g::stopEffect50BC(controls);return true;}};
 }
 bool fogPosition(std::size_t i,Child& c,Diagnostic& d)noexcept {
  const auto* graph=attachments_->graph(i);const auto& offset=source_->descriptor(source_->attachment(i).descriptor).offset;
  if(!graph||!g::refreshSceneWorld49P(*graph,static_cast<g::SceneNodeIndex49P>(0)))
   return fail(d,"RC actual fog attachment refresh rejected",Status::storage,0x15ff6cu);
  const auto& m=graph->nodes[0].world100;g::water_wake_detail50jc::Math math;std::array<float,3> rotated{};
  for(unsigned k=0;k<3;++k)put(c.controls.position30[k],m.v[12+k]);
  for(unsigned k=0;k<3;++k){const auto x=math.mul(m.v[k],offset[0],0x28ba74u);
   const auto y=math.mul(m.v[4+k],offset[1],0x28ba78u);const auto xy=math.add(x,y,0x28ba78u);
   rotated[k]=math.add(xy,math.mul(m.v[8+k],offset[2],0x28ba7cu),0x28ba7cu);}
  if(math.badPC)return fail(d,"RC actual fog offset rejected",Status::arithmetic,math.badPC);
  for(unsigned k=0;k<3;++k){const auto value=math.add(bits(c.controls.position30[k]),rotated[k],0x15ff8cu+16u*k);
   if(math.badPC)return fail(d,"RC actual fog position rejected",Status::arithmetic,math.badPC);put(c.controls.position30[k],value);}
  std::memcpy(c.controls.previous40,c.controls.position30,16);return true;
 }
 bool accountRegistry(Diagnostic& d)noexcept {
  d.sharedEmitterPayloadBytes=manager_->emitterStoragePayloadBytes();
  const auto growth=d.sharedEmitterPayloadBytes>initialEmitterPayload_?d.sharedEmitterPayloadBytes-initialEmitterPayload_:0u;
  if(growth>std::numeric_limits<std::size_t>::max()-sizeof(Owner))return fail(d,"RC retained registry payload overflow",Status::capacity,0x27d7a0u);
  d.additionalRetainedPayloadBytes=sizeof(Owner)+growth;
  return d.additionalRetainedPayloadBytes<=additionalBudget_||fail(d,"RC actual retained registry growth exceeded admission",Status::capacity,0x27d7a0u);
 }
 bool initializeChild(std::size_t i,Diagnostic& d)noexcept {
  auto& c=children_[i];const auto& desc=source_->descriptor(source_->attachment(i).descriptor);
  const auto& f=source_->definition(desc.kind);const auto* group=manager_->group(f.particleGroup1B8);
  if(!group||!f.parsed||desc.effectiveStartOn!=1||f.timing.mode1D8!=0)
   return fail(d,"RC actual subtype/StartOn/mode unavailable",Status::unsupported,0x27d7a0u);
  c.kind=desc.kind;c.controls={};c.controls.flags1D4=1;
  c.controls.position30[3]=c.controls.previous40[3]=c.controls.velocity60[3]=0x3f800000u;
  for(unsigned k=0;k<3;++k)put(c.controls.velocity60[k],f.velocity60[k]);
  if(c.kind==Kind::Smoke0)c.controls.intensity204=0x3f800000u;
  c.delay1DC=f.timing.delayTime1E0;put(c.duration1E4,f.timing.maxTimeOn1EC);
  c.currentFrame190=c.firstFrame1B0=group->first;c.lastFrame1B4=group->last;
  // Actual native row, distinct for every attached instance, stored in the
  // same fixed-pool manager. The registry owns scalar copies, not this address.
  g::OriginalGcParticleEmitterState50KG row{};row.particleGroup=f.particleGroup1B8;
  row.framesPerSet=f.framesPerSet;row.inverseFramesPerSet=f.inverseFramesPerSet;
  row.gravity=f.gravityMultiplier;row.bouncePlane=f.groundLevel;row.bounceFactor=f.restitution;
  if(c.kind==Kind::Fog12){row.fogPeriod=f.oscillationPeriod22C;row.fogIncreasing=true;}
  c.managerOwnerId=manager_->registerEmitter(row);
  if(!c.managerOwnerId)return fail(d,"RC actual emitter row allocation rejected",Status::allocation,0x27d7a0u);
  // Actual post-allocation payload admission. A failed check retains this
  // registered prefix; it does not claim a pre-allocation heap ceiling.
  if(!accountRegistry(d))return false;
  if(c.kind==Kind::Fog12){
   if(!fogPosition(i,c,d))return false;
   const auto receipt=g::original_rc_fog_numeric_detail50RC::populate(f,*manager_,c.managerOwnerId,c.currentFrame190,c.controls,*raw_,*shared_,*clock_);
   d.spawned+=receipt.spawned;d.randomDraws+=receipt.randomDraws;
   if(!receipt.okay())return failedReceipt(d,receipt,"RC actual cold fog population stopped");
  }
  // Wrapper StartOn executes source virtual80 then enable. No source Entity
  // ancestry is claimed by this native schedule's explicit enabled state.
  g::startEffect50BC(c.controls);c.enabled=true;++activatedChildren_;d.activatedChildren=activatedChildren_;return true;
 }
 bool advanceChild(std::size_t i,float dt,Diagnostic& d)noexcept {
  auto& c=children_[i];const auto& f=source_->definition(c.kind);
  if(!c.enabled||c.failed)return fail(d,"RC actual child state unavailable",Status::storage,0x27dc60u);
  if(c.kind==Kind::Fog12){auto* row=manager_->emitter(c.managerOwnerId);
   if(!row)return fail(d,"RC actual fog row unavailable",Status::storage,0x160160u);
   const auto phase=g::original_rc_fog_numeric_detail50RC::phase(*row,dt);
   if(!phase.okay())return failedReceipt(d,phase,"RC actual fog phase stopped");
  }
  const auto* graph=attachments_->graph(i);g::OriginalGcCommonAttachment50KG attachment;
  attachment.graph=graph;attachment.node=static_cast<g::SceneNodeIndex49P>(0);
  attachment.offset50=source_->descriptor(source_->attachment(i).descriptor).offset;
  auto calls=dispatch(c);
  std::uint32_t actualPause11C=0;
  if(!applicationAnimation_.readPause11C(actualPause11C))
   return fail(d,"RC actual Application pause owner unavailable",Status::unprepared,0x15a7a0u);
  const auto common=g::advanceOriginalGcCommonEmitter50KG(c.controls,f.timing,c.delay1DC,c.duration1E4,true,dt,actualPause11C!=0,*raw_,&calls,&attachment);
  d.randomDraws+=common.randomDraws;
  if(!common.okay())return failedReceipt(d,common,"RC actual common emitter stopped");
  if(common.commonSkipped||c.kind==Kind::Fog12)return true;
  if(c.kind==Kind::Smoke0){const auto r=g::original_smoke_numeric_detail50RC::emit<0x15bb40u>(c,f,*manager_,c.controls,dt,*raw_,*shared_,*clock_);
   d.randomDraws+=r.randomDraws;d.spawned+=r.spawned;
   return r.okay()||failedReceipt(d,r,"RC actual generic smoke stopped");}
  g::water_wake_detail50jc::Math m;std::array<float,3> previous{},delta{},velocity{};
  for(unsigned k=0;k<3;++k){previous[k]=bits(c.controls.previous40[k]);delta[k]=m.sub(bits(c.controls.position30[k]),previous[k],0x15e128u+8u*k);}
  const auto inverse=m.div(1.f,dt,0x15e140u);
  velocity[1]=m.mul(delta[1],inverse,0x15e160u);velocity[0]=m.mul(delta[0],inverse,0x15e164u);velocity[2]=m.mul(delta[2],inverse,0x15e16cu);
  if(m.badPC)return fail(d,"RC actual fire velocity stopped",Status::arithmetic,m.badPC);
  for(unsigned k=0;k<3;++k)put(c.controls.velocity60[k],velocity[k]);c.controls.velocity60[3]=0x3f800000u;
  // The actual same-source Window privately publishes completed instance
  // stamps. A graph transform or numerical packet cannot mint this gate.
  const auto gate=attachments_->fireGate(i);
  if(gate==rc_particle_attachments50RC::FireGate::Unavailable)return fail(d,"RC actual fire visual owner unavailable",Status::unprepared,0x15e198u);
  if(gate==rc_particle_attachments50RC::FireGate::Stale){++d.staleFireSkips;return true;}
  g::OriginalGcFireEmissionDefinition50KG emission;
  emission.particleGroup=f.particleGroup1B8;emission.firstFrame=c.firstFrame1B0;emission.lastFrame=c.lastFrame1B4;
  emission.particleLife=f.particleLife17C;emission.initialSize=f.initialSize184;emission.growth=f.growth180;emission.emitRate=f.emitRate188;
  emission.framesPerSet=f.framesPerSet;emission.minRange=f.minRange;emission.maxRange=f.maxRange;emission.authoredVelocityY220=f.velocity60[1];
  emission.particleFlags=f.particleFlags198;emission.randomMirrorX=f.randomMirrorX;emission.randomMirrorY=f.randomMirrorY;emission.applySpreadToVelocity=f.applySpreadToVelocity;
  const auto r=g::emitOriginalGcFireParticles50KG(emission,*manager_,c.managerOwnerId,c.fractional224,previous,c.controls.previous40[3],velocity,dt,*raw_,*fireSerial_,true);
  d.spawned+=r.spawned;d.randomDraws+=r.randomDraws;d.nativeUndefinedCarryPolicy=d.nativeUndefinedCarryPolicy||r.nativeUndefinedCarryPolicy;
  return r.okay()||failedReceipt(d,r,"RC actual fire emission stopped");
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||visiting_||rendering_||(cold_&&!cold_->closed()))std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 bool prepare(std::shared_ptr<const authored::Owner> source,std::shared_ptr<rc_particle_attachments50RC::Owner> attachments,
  std::shared_ptr<g::OriginalTrackParticleCold50LA> actualCold,std::uint32_t& raw,g::SharedRandomOwner50ID& shared,
  g::HostMonotonicClock50ID& clock,std::uint32_t& fireSerial,g::OriginalApplicationAnimationClockView50KG actualApplicationAnimation,
  std::size_t remainingBytes,Diagnostic& d)noexcept {
  d={};if(!thread()||attempted_)return fail(d,"RC runtime requires one owner-thread attempt",Status::alreadyPrepared,0x27dc90u);attempted_=true;
  std::uint32_t actualPause11C=0;
  if(!source||!source->ready()||!attachments||!attachments->ready()||attachments->authoringOwner()!=source.get()||
     !actualCold||!actualCold->prepared()||!actualCold->material150()||!actualCold->material150()->retained()||
     remainingBytes<retainedPayloadBytes()||&raw==&fireSerial||!actualApplicationAnimation.readPause11C(actualPause11C))
   return fail(d,"RC actual source/graph/manager/material/shared-state input rejected",Status::unprepared,0x27dc90u);
  try{
   auto* manager=actualCold->particles();const auto& atlas=source->atlas();
   if(!manager||manager->frame(105)||manager->group(9)||!manager->atlasTextureMapName()||*manager->atlasTextureMapName()!=atlas.textureMap)
    return fail(d,"RC actual manager atlas identity rejected",Status::assetIdentity,0x1617a0u);
   for(std::size_t i=0;i<atlas.groups.size();++i){const auto* group=manager->group(static_cast<std::int32_t>(i));const auto& expected=atlas.groups[i];
    if(!group||group->type!=expected.type||group->first!=expected.first||group->last!=expected.last)
     return fail(d,"RC actual manager group differs",Status::assetIdentity,0x1618d0u);}
   for(std::size_t i=0;i<atlas.frames.size();++i){const auto* frame=manager->frame(static_cast<std::int32_t>(i));
    if(!frame||frame->sourceUv!=atlas.frames[i].sourceUv)return fail(d,"RC actual manager frame differs",Status::assetIdentity,0x161a0cu);}
   source_=std::move(source);attachments_=std::move(attachments);cold_=std::move(actualCold);manager_=manager;
   raw_=&raw;shared_=&shared;clock_=&clock;fireSerial_=&fireSerial;applicationAnimation_=std::move(actualApplicationAnimation);bound_=true;
   initialEmitterPayload_=manager_->emitterStoragePayloadBytes();additionalBudget_=remainingBytes;
   if(!accountRegistry(d))return false;
   d.source.status=Status::complete;return true;
  }catch(...){return fail(d,"RC runtime source admission failed",Status::storage,0x27dc90u);}
 }
 bool activate(Diagnostic& d)noexcept {
  d={};if(!current()||terminal_||quiesced_||visiting_||rendering_||activationAttempted_)
   return fail(d,"RC activation requires one actual bound source lifetime",Status::unprepared,0x27d7a0u);
  activationAttempted_=true;visiting_=true;struct End{bool& flag;~End(){flag=false;}}end{visiting_};
  for(std::size_t i=0;i<children_.size();++i){d.child=i;if(!initializeChild(i,d))return false;}
  active_=true;d.source.status=Status::complete;return true;
 }
 bool advance(float dt,Diagnostic& d)noexcept {
  d={};if(!current()||!active_||terminal_||quiesced_||visiting_||rendering_)
   return fail(d,"RC scheduler requires its active serial actual owner",Status::unprepared,0x161180u);
  visiting_=true;struct End{bool& flag;~End(){flag=false;}}end{visiting_};
  std::uint32_t actualPause11C=0;
  if(!applicationAnimation_.readPause11C(actualPause11C))
   return fail(d,"RC actual Application pause owner unavailable",Status::unprepared,0x161180u);
  d.manager=manager_->advance(dt,actualPause11C!=0);
  if(!d.manager.okay())return fail(d,"RC actual shared manager advance stopped",Status::storage,0x161180u);
  for(std::size_t i=0;i<children_.size();++i){d.child=i;if(!advanceChild(i,dt,d))return false;}
  d.source.status=Status::complete;d.activatedChildren=activatedChildren_;return true;
 }
 std::size_t sharedEmitterPayloadBytes()const noexcept{return current()?manager_->emitterStoragePayloadBytes():0u;}
 bool active()const noexcept{return current()&&active_&&!terminal_&&!quiesced_;}
 bool terminal()const noexcept{return terminal_;}
 const authored::Owner* sourceOwner()const noexcept{return current()?source_.get():nullptr;}
 const rc_particle_attachments50RC::Owner* attachmentOwner()const noexcept{return current()?attachments_.get():nullptr;}
 const g::OriginalTrackParticleCold50LA* coldOwner()const noexcept{return cold_.get();}
 const g::OriginalGcParticleManager50KG* manager()const noexcept{return current()?manager_:nullptr;}
 // End active scheduler/render borrowers before closing the shared real pool.
 // No source Stop or invented emitter deregistration is used as destruction.
 bool quiesce()noexcept {if(!thread()||visiting_||rendering_)return false;quiesced_=true;active_=false;return true;}
 bool closeAfterPool()noexcept {
  if(!thread()||visiting_||rendering_||!quiesced_||(cold_&&!cold_->closed()))return false;
  manager_=nullptr;cold_.reset();attachments_.reset();source_.reset();applicationAnimation_={};bound_=false;closed_=true;return true;
 }
};
} // namespace rc_particle_runtime50RC
