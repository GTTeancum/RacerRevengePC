#pragma once
#include "gb_particle_authoring50gb.h"
#include "gb_particle_attachments50gb.h"
#include "gb_trigger_authoring50gb.h"
#include "../../src/game/native_trigger_group50gb.h"
#include "../../src/game/original_gc_pod_sparks50kg.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <thread>

namespace gb_particle_runtime50GB {class Owner;}
namespace gb_trigger_actions50GB {class Owner;}
namespace gb_type11_numeric50GB {
namespace g=rr::game;namespace authored=gb_particle_authoring50GB;
namespace triggers=gb_trigger_authoring50GB;
using Status=g::OriginalGcSprayStatus50KG;
using Receipt=g::OriginalGcSprayReceipt50KG;
struct Observation {
 std::size_t attachmentIndex=0;std::uint32_t emitterNumber=0,managerOwnerId=0;
 std::size_t registryPayloadBefore=0,registryPayloadGrowth=0;
 std::int32_t firstFrame=0,lastFrame=0,currentFrame=0;
 float remainingDelay=0,carriedSize=0;std::uint32_t remainingOnBits=0;
 g::EffectControlState50BC controls{};
 bool numericBound=false,schedulingEnabled=false,actionCompleted=false,failed=false,quiesced=false;
 Receipt stoppedAt{};
};
// Independently reviewable numerical/control storage for ONE authentic GB
// Type11 association. This is not a complete runtime owner or readiness gate.
// Every mutation is private to the actual enclosing GB runtime/ActionOwner.
// There is no public update, target handle, graph/visibility receipt or callback.
class Instance final {
 friend class ::gb_particle_runtime50GB::Owner;
 friend class ::gb_trigger_actions50GB::Owner;
 using Math=g::water_wake_detail50jc::Math;
 // Computed only by the enclosing runtime from its privately retained concrete
 // graph/publication owner. It is not an original 16-bit portal-tag receipt.
 enum class PlacementCompletion : std::uint8_t {Unavailable,Stale,Current};
 std::shared_ptr<const authored::Owner> source_;
 std::shared_ptr<gb_particle_attachments50GB::Owner> graphs_;
 std::shared_ptr<const triggers::Owner> triggerSource_;
 std::shared_ptr<g::OriginalTrackParticleCold50LA> cold_;
 const authored::Descriptor* descriptor_=nullptr;
 const authored::Attachment* association_=nullptr;
 const authored::Definition* definition_=nullptr;
 g::OriginalGcParticleManager50KG* manager_=nullptr;
 g::OriginalGcCommonAttachment50KG attachment_{};
 std::uint32_t* raw_=nullptr;g::OriginalGcPodSparksGlobals50KG* globals_=nullptr;
 const ::gb_trigger_actions50GB::Owner* actions_=nullptr;
 g::NativeTriggerGroup50GB* group_=nullptr;
 g::EffectControlState50BC controls_{};
 std::size_t attachmentIndex_=0,registryBefore_=0,registryGrowth_=0;
 std::uint32_t managerOwnerId_=0,duration1E4_=0;
 std::int32_t firstFrame_=0,lastFrame_=0,currentFrame_=0;
 float delay1DC_=0,carriedSize18C_=0;
 std::uint64_t generation_=0,actionGeneration_=0,completedQuery_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,bound_=false,enabled_=false,actionBound_=false,actionCompleted_=false;
 bool visiting_=false,failed_=false,quiesced_=false,closed_=false;
 Receipt stoppedAt_{};
 static float word(std::uint32_t x)noexcept {float f;std::memcpy(&f,&x,4);return f;}
 static void put(std::uint32_t& out,float f)noexcept {std::memcpy(&out,&f,4);}
 static Receipt result(Status s,std::uint32_t pc)noexcept {Receipt r;r.status=s;r.sourcePC=pc;return r;}
 static float root(Math& m,float x,std::uint32_t pc)noexcept {m.read(x,pc);volatile float r=std::sqrt(x);return m.read(r,pc);}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 Receipt fail(Status s,std::uint32_t pc)noexcept {failed_=true;stoppedAt_=result(s,pc);return stoppedAt_;}
 bool sourceCurrent()const noexcept {
  return thread()&&bound_&&!closed_&&source_&&source_->ready()&&cold_&&cold_->prepared()&&
   graphs_&&graphs_->sourceGraphReady()&&graphs_->authoringOwner()==source_.get()&&
   graphs_->coldSource()==source_->sourceOwner()&&
   manager_&&cold_->particles()==manager_&&raw_&&globals_&&generation_&&descriptor_&&association_&&definition_&&
   attachmentIndex_>=62&&attachmentIndex_<67&&association_==&source_->attachment(attachmentIndex_)&&
   descriptor_==&source_->descriptor(association_->descriptor)&&
   definition_==&source_->definition(descriptor_->definitionIndex)&&
   managerOwnerId_&&manager_->emitter(managerOwnerId_);
 }
 bool graphCurrent()const noexcept {
  if(!sourceCurrent()||!attachment_.graph||attachment_.graph!=graphs_->graph(attachmentIndex_)||
     graphs_->placement(attachmentIndex_)!=association_->worldInstance||
     attachment_.node!=static_cast<g::SceneNodeIndex49P>(0)||
     attachment_.offset50!=descriptor_->offset)return false;
  const auto& graph=*attachment_.graph;
  if(!graph.nodes||graph.count!=1||!g::validateSceneGraph49P(graph)||
     graph.nodes[0].parent180!=g::SceneNodeIndex49P::none)return false;
  // GB's five targets are the same authentic static startgate placement.
  // The exact concrete graph owner is retained, with graph access private.
  const auto* placed=association_->worldInstance;if(!placed)return false;
  for(unsigned k=0;k<16;++k){const auto expected=static_cast<float>(placed->matrix[k]);
   if(!std::isfinite(expected)||std::memcmp(&expected,&graph.nodes[0].localC0.v[k],4))return false;}
  return true;
 }
 bool atlasMatches(const authored::Owner& source,const g::OriginalGcParticleManager50KG& manager)const noexcept {
  const auto& a=source.atlas();
  if(manager.frame(130)||manager.group(10)||!manager.atlasTextureMapName()||*manager.atlasTextureMapName()!=a.textureMap)return false;
  for(std::size_t i=0;i<a.groups.size();++i){const auto* actual=manager.group(static_cast<std::int32_t>(i));const auto& expected=a.groups[i];
   if(!actual||actual->type!=expected.type||actual->first!=expected.first||actual->last!=expected.last)return false;}
  for(std::size_t i=0;i<a.frames.size();++i){const auto* actual=manager.frame(static_cast<std::int32_t>(i));
   if(!actual||actual->sourceUv!=a.frames[i].sourceUv)return false;}
  return true;
 }
 // Called in descriptor/instance order by the genuine runtime, after its
 // concrete graph owner has authenticated the real PSG root and placement.
 // This registers a real numeric emitter; it grants no update/query readiness.
 Receipt initialize(std::shared_ptr<const authored::Owner> source,std::size_t actualAttachment,
  std::shared_ptr<gb_particle_attachments50GB::Owner> actualGraphs,
  std::shared_ptr<g::OriginalTrackParticleCold50LA> actualCold,
  std::uint32_t& actualRaw,
  g::OriginalGcPodSparksGlobals50KG& actualSharedGlobals,std::uint64_t generation,
  std::size_t remainingPayloadBytes)noexcept {
  if(!thread())return result(Status::unprepared,0x15f1e0u);
  if(attempted_)return fail(Status::alreadyPrepared,0x15f1e0u);attempted_=true;
  if(!source||!source->ready()||!actualGraphs||!actualGraphs->sourceGraphReady()||
     actualGraphs->authoringOwner()!=source.get()||actualGraphs->coldSource()!=source->sourceOwner()||
     !actualCold||!actualCold->prepared()||!generation||
     actualAttachment<62||actualAttachment>=67||remainingPayloadBytes>32u*1024u*1024u||
     remainingPayloadBytes<sizeof(Instance)||std::fegetround()!=FE_TONEAREST)return fail(Status::unprepared,0x15f1e0u);
  const auto& a=source->attachment(actualAttachment);const auto& desc=source->descriptor(a.descriptor);
  const auto& f=source->definition(desc.definitionIndex);auto* manager=actualCold->particles();
  if(a.descriptor!=actualAttachment-55||a.instanceOrdinal!=0||a.worldIndex!=0||
     a.worldInstance!=&source->sourceOwner()->world().instances[0]||desc.ordinal!=a.descriptor+1||
     desc.kind!=authored::Kind::Sparks11||desc.instances!=1||desc.firstAttachment!=actualAttachment||
     desc.effectiveStartOn!=0||desc.definitionIndex!=(actualAttachment<64?2u:3u)||
     !f.parsed||f.kind!=authored::Kind::Sparks11||f.timing.mode1D8!=1||!manager||!atlasMatches(*source,*manager)||
     actualGraphs->placement(actualAttachment)!=a.worldInstance||!actualGraphs->graph(actualAttachment))
   return fail(Status::assetIdentity,0x15fc10u);
  source_=std::move(source);graphs_=std::move(actualGraphs);cold_=std::move(actualCold);
  descriptor_=&desc;association_=&a;definition_=&f;attachmentIndex_=actualAttachment;
  attachment_={graphs_->graph(actualAttachment),static_cast<g::SceneNodeIndex49P>(0),desc.offset};
  manager_=manager;raw_=&actualRaw;
  globals_=&actualSharedGlobals;generation_=generation;
  controls_={};controls_.flags1D4=1;controls_.position30[3]=controls_.previous40[3]=controls_.velocity60[3]=0x3f800000u;
  for(unsigned k=0;k<3;++k)put(controls_.velocity60[k],f.velocity60[k]);
  delay1DC_=f.timing.delayTime1E0;put(duration1E4_,f.timing.maxTimeOn1EC);carriedSize18C_=0;
  const auto* group=manager_->group(f.particleGroup1B8);
  if(!group||group->first!=(desc.definitionIndex==2?104:129)||group->last!=group->first)
   return fail(Status::authoredData,0x15fd6cu);
  firstFrame_=currentFrame_=group->first;lastFrame_=group->last;
  g::OriginalGcParticleEmitterState50KG row{};row.particleGroup=f.particleGroup1B8;
  row.framesPerSet=f.framesPerSet;row.inverseFramesPerSet=f.inverseFramesPerSet;
  row.gravity=f.gravityMultiplier;row.bouncePlane=f.groundLevel;row.bounceFactor=f.restitution;
  // Existing registerEmitter mutates a std::vector and exposes no reserve
  // API. This post-allocation observation CANNOT enforce a hard shared heap
  // cap, nor measure transient old-plus-new vector storage. Runtime admission
  // is held until the enclosing owner establishes that reservation before
  // calling this method. Retain every attempted prefix for ordered cleanup.
  registryBefore_=manager_->emitterStoragePayloadBytes();managerOwnerId_=manager_->registerEmitter(row);
  if(!managerOwnerId_)return fail(Status::allocation,0x15fd6cu);
  bound_=true;const auto now=manager_->emitterStoragePayloadBytes();registryGrowth_=now>registryBefore_?now-registryBefore_:0;
  if(registryGrowth_>remainingPayloadBytes-sizeof(Instance))return fail(Status::capacity,0x15fd6cu);
  if(!graphCurrent())return fail(Status::storage,0x15f220u);
  // Attached source load does not transform position. Constructor position
  // and previous remain (0,0,0,1); the first common visit still copies/skips.
  // Source constructor carried-size zero; false wrapper StartOn calls Stop84
  // and then disable18. Detached scheduling records that activity explicitly.
  g::stopEffect50BC(controls_);enabled_=false;return result(Status::complete,0x27dbe0u);
 }
 bool bindActionSource(const ::gb_trigger_actions50GB::Owner& actions,g::NativeTriggerGroup50GB& group,
  std::shared_ptr<const triggers::Owner> source)noexcept {
  if(!sourceCurrent()||!graphCurrent()||failed_||quiesced_||visiting_||actionBound_||!source||!source->ready()||
     source->coldSource()!=source_->sourceOwner()||group.ready||group.queryActive||group.actionsFailed||
     group.actions.context!=static_cast<const void*>(&actions)||!group.actions.preflight||!group.actions.resume||
     group.contact.phase!=g::SourceTriggerContactPhase50GB::Idle)return false;
  const auto index=descriptor_->ordinal-8;const auto& trigger=source->source(triggers::particles);
  const auto& action=trigger.actions[index];const auto& state=group.triggers[triggers::particles];
  if(index>=5||trigger.sourceOrdinal!=18||trigger.actionCount!=5||!trigger.oneTimeOnly||
     action.kind!=triggers::Action::Particles||action.emitterNumber!=descriptor_->ordinal||action.instanceNumber!=1||
     action.emitterIndex!=association_->descriptor||action.instanceIndex!=0||!action.turnOn||
     !state.collider.identity||state.callbackCount98!=5||state.collider.highWaterD0)return false;
  triggerSource_=std::move(source);actions_=&actions;group_=&group;actionGeneration_=generation_;actionBound_=true;return true;
 }
 bool matchesAction(const ::gb_trigger_actions50GB::Owner& actions,const triggers::Owner& source,
  const triggers::ActionSource& action,const g::SourceTriggerActionCall50GB& call,bool reached)const noexcept {
  if(!sourceCurrent()||!graphCurrent()||failed_||quiesced_||visiting_||!actionBound_||actions_!=&actions||
     actionGeneration_!=generation_||!triggerSource_||triggerSource_.get()!=&source||!source.ready()||
     source.coldSource()!=source_->sourceOwner()||!group_||!g::validNativeTriggerGroup50GB(*group_)||
     group_->actions.context!=static_cast<const void*>(&actions)||!group_->queryActive||!group_->querySerial||
     call.querySerial!=group_->querySerial||call.source.otherCollider!=group_->queryCollider||
     !call.source.entering||call.source.sourceSite!=0x2a0eccu||call.source.actionIndex>=5||
     call.source.actionIndex!=descriptor_->ordinal-8||actionCompleted_)return false;
  const auto& state=group_->triggers[triggers::particles];const auto& original=source.source(triggers::particles).actions[call.source.actionIndex];
  if(&action!=&original||!action.original||action.kind!=triggers::Action::Particles||!action.turnOn||
     action.emitterNumber!=descriptor_->ordinal||action.instanceNumber!=1||action.emitterIndex!=association_->descriptor||
     action.instanceIndex!=association_->instanceOrdinal||state.collider.identity!=call.source.triggerCollider||
     g::nativeTriggerCollider50GB(*group_,call.source.otherCollider)>=group_->colliders.size()||
     !g::nativeTriggerPairAllowed50GB(*group_,triggers::particles,call.source.otherCollider))return false;
  const auto& c=group_->contact;using P=g::SourceTriggerContactPhase50GB;
  if(!reached)return c.phase==P::Idle||c.phase==P::Complete;
  // E8 passes the traversal position within the actual query index span.
  // It is not the aggregate owner-table index (615+17). Authenticate the
  // exact live cursor and collider below; the enclosing query owner retains
  // the actual candidate/index mapping throughout this dispatch.
  return c.phase==P::Actions&&c.group==group_&&c.hooks==&group_->actions&&
   c.querySerial==call.querySerial&&c.candidateOrdinal==call.candidateOrdinal&&c.triggerIndex==triggers::particles&&
   c.triggerCollider==call.source.triggerCollider&&c.otherCollider==call.source.otherCollider&&
   c.colliderIndex==g::nativeTriggerCollider50GB(*group_,call.source.otherCollider)&&
   c.actionCount==5&&c.nextAction==call.source.actionIndex&&(state.flags1B8&0x18u)==0x18u;
 }
 bool preflightSourceAction(const ::gb_trigger_actions50GB::Owner& actions,const triggers::Owner& source,
  const triggers::ActionSource& action,const g::SourceTriggerActionCall50GB& call)const noexcept {
  return matchesAction(actions,source,action,call,false);
 }
 g::SourceTriggerActionResult50GB activateSourceAction(const ::gb_trigger_actions50GB::Owner& actions,
  const triggers::Owner& source,const triggers::ActionSource& action,const g::SourceTriggerActionCall50GB& call)noexcept {
  using S=g::SourceTriggerActionStatus50GB;
  if(!matchesAction(actions,source,action,call,true)){
   if(thread()&&bound_)fail(Status::unprepared,0x15a500u);
   return {S::InvalidBinding,0x15a500u,0};}
  // All fallible checks precede these admitted mode-1 scalar operations.
  // No emission, RNG, allocation, graph update or provider callback occurs.
  delay1DC_=definition_->timing.delayTime1E0; //15AD68
  put(duration1E4_,definition_->timing.maxTimeOn1EC); //15AD78..80
  g::startEffect50BC(controls_); //15ACE0: started bit, timer19C=0
  enabled_=true; // Approved detached scheduling projection of enable20.
  completedQuery_=call.querySerial;actionCompleted_=true;
  return {S::Complete,0x15a544u,0};
 }
 static g::OriginalGcCommonEmitterDispatch50KG commonDispatch(Instance& self)noexcept {
  return {&self,[](void* p,g::EffectControlState50BC& c){auto& s=*static_cast<Instance*>(p);
    if(&c!=&s.controls_||!s.definition_||s.definition_->timing.mode1D8!=1)return false;g::startEffect50BC(c);return true;},
   [](void* p,g::EffectControlState50BC& c){auto& s=*static_cast<Instance*>(p);
    if(&c!=&s.controls_||!s.definition_||s.definition_->timing.mode1D8!=1)return false;g::stopEffect50BC(c);return true;}};
 }
 Receipt advance(float dt,bool actualApplicationPaused,PlacementCompletion completion)noexcept {
  if(!thread())return result(Status::unprepared,0x15f380u);
  if(failed_)return stoppedAt_;
  if(!sourceCurrent()||!graphCurrent()||quiesced_||visiting_||
     (group_&&(group_->queryActive||group_->actionsFailed)))return fail(Status::unprepared,0x15f380u);
  // Authentic initially disabled instances are not scheduled at all.
  if(!enabled_)return result(Status::complete,0x27dbe0u);
  visiting_=true;struct End{bool& v;~End(){v=false;}}end{visiting_};
  const auto calls=commonDispatch(*this);
  auto common=g::advanceOriginalGcCommonEmitter50KG(controls_,definition_->timing,delay1DC_,duration1E4_,
    true,dt,actualApplicationPaused,*raw_,&calls,&attachment_);
  if(!common.okay()){failed_=true;stoppedAt_=common;return common;}
  if(common.commonSkipped)return common;
  return afterCommon(dt,completion);
 }
 Receipt afterCommon(float dt,PlacementCompletion completion)noexcept;
 bool unbindActionSource(const ::gb_trigger_actions50GB::Owner& actions)noexcept {
  if(!thread()||visiting_||actions_!=&actions||!group_||group_->queryActive)return false;
  const auto p=group_->contact.phase;
  if(p!=g::SourceTriggerContactPhase50GB::Idle&&p!=g::SourceTriggerContactPhase50GB::Complete&&!group_->actionsFailed)return false;
  actions_=nullptr;group_=nullptr;triggerSource_.reset();actionBound_=false;return true;
 }
 bool quiesce()noexcept {if(!thread()||visiting_||(group_&&group_->queryActive))return false;quiesced_=true;return true;}
 bool retireAfterPool()noexcept {
  if(!thread()||visiting_||!quiesced_||actionBound_||(cold_&&!cold_->closed()))return false;
  attachment_={};manager_=nullptr;raw_=nullptr;globals_=nullptr;descriptor_=nullptr;association_=nullptr;definition_=nullptr;
  graphs_.reset();source_.reset();cold_.reset();bound_=false;closed_=true;return true;
 }
public:
 Instance()=default;Instance(const Instance&)=delete;Instance& operator=(const Instance&)=delete;
 Instance(Instance&&)=delete;Instance& operator=(Instance&&)=delete;
 ~Instance(){if(!thread()||visiting_||actionBound_||(cold_&&!cold_->closed()))std::terminate();}
 Observation observation()const noexcept {
  if(!thread())return {};return {attachmentIndex_,descriptor_?descriptor_->ordinal:0,managerOwnerId_,
   registryBefore_,registryGrowth_,
   firstFrame_,lastFrame_,currentFrame_,delay1DC_,carriedSize18C_,duration1E4_,controls_,
   bound_,enabled_,actionCompleted_,failed_,quiesced_,stoppedAt_};
 }
};

inline Receipt Instance::afterCommon(float dt,PlacementCompletion completion)noexcept {
    const auto& f=*definition_;auto& manager=*manager_;auto& raw=*raw_;auto& globals=*globals_;
    auto r=result(Status::complete,0x15fbc8);Math m;
    auto fail=[&](Status s,std::uint32_t pc){r.status=s;r.sourcePC=pc;failed_=true;stoppedAt_=r;return r;};
    auto arithmetic=[&](){return fail(Status::arithmetic,m.badPC);};
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x15f3c0);
    m.read(dt,0x15f450);if(m.badPC)return arithmetic();
    if(!(dt>0)||dt>1.f)return fail(Status::unsupported,0x15f450);
    auto& c=controls_;
    const auto timer=m.read(word(c.word19C),0x15f3c0);if(m.badPC)return arithmetic();
    if(timer>.25f){std::memcpy(c.previous40,c.position30,16);return r;}
    // The approved native completed-placement gate belongs to the real GB
    // graph/visual owner. It is checked AFTER the source timer shortcut.
    // This private scalar input is not a public original-tag authority.
    if(completion==PlacementCompletion::Unavailable)return fail(Status::unprepared,0x15f3f0);
    if(completion==PlacementCompletion::Stale){
        carriedSize18C_=f.initialSize184;c.word19C=0;r.sourcePC=0x15fbb8;return r;}
    if(completion!=PlacementCompletion::Current)return fail(Status::unprepared,0x15f3f0);
    const float emission=m.mul(f.emitRate188,dt,0x15f450);
    if(m.badPC)return arithmetic();
    if(emission<0||emission>=2147483648.f)return fail(Status::unsupported,0x15fba4);
    float velocity[3],position[3];for(unsigned i=0;i<3;++i){
        velocity[i]=m.read(word(c.velocity60[i]),0x15f454u+4u*i);
        position[i]=m.read(word(c.position30[i]),0x15f5c8u+8u*i);}
    float speedFactor=1;
    if(f.movingEmitter21C){
        const auto square=m.add(m.add(m.mul(velocity[0],velocity[0],0x15f464),
            m.mul(velocity[1],velocity[1],0x15f468),0x15f46c),m.mul(velocity[2],velocity[2],0x15f470),0x15f470);
        const auto speed=root(m,square,0x15f474);
        if(!globals.lazy84D0){globals.speedScale433980=20.f;globals.lazy84D0=true;}
        const auto scale=m.read(globals.speedScale433980,0x15f4a0);
        if(speed<scale)speedFactor=m.div(speed,scale,0x15f4bc);
    }
    if(m.badPC)return arithmetic();
    auto draw=[&](){++r.randomDraws;return g::drawOriginalGcParticleRandom50KG(raw);};
    auto finish=[&](){carriedSize18C_=f.initialSize184;c.word19C=0;return r;};
    // Source compares CVT.S(s0) directly to rate*dt: positive fractional
    // demand runs one last stream; do not truncate it as a particle count.
    for(std::int32_t stream=0;m.integer(stream,0x15fba0)<emission;++stream){
        const float randomX=draw(),randomY=draw(),randomZ=draw();
        const float random[3]{randomX,randomY,randomZ};float spread[3];
        for(unsigned axis=0;axis<3;++axis)spread[axis]=m.add(f.minRange[axis],
            m.mul(random[axis],m.sub(f.maxRange[axis],f.minRange[axis],0x15f550u+28u*axis),
                0x15f554u+28u*axis),0x15f558u+28u*axis);
        float point[3],particleVelocity[3];
        for(unsigned axis=0;axis<3;++axis){point[axis]=m.sub(position[axis],m.mul(dt,velocity[axis],
            0x15f5b8u+4u*axis),0x15f5e4u+4u*axis);particleVelocity[axis]=m.add(velocity[axis],spread[axis],0x15f6a8u+4u*axis);}
        const float growth=m.mul(f.growth180,draw(),0x15f630);
        const float life=m.mul(f.particleLife17C,draw(),0x15f67c);
        (void)m.div(1,life,0x15f694); // Source computes before the first capacity guard.
        std::uint32_t flags=f.particleFlags198;
        if(f.randomMirrorX&&randomX>.5f)flags|=0x4000;else flags&=~0x4000u;
        if(f.randomMirrorY&&randomY>.5f)flags|=0x8000;else flags&=~0x8000u;
        const auto squared=m.add(m.add(m.mul(particleVelocity[0],particleVelocity[0],0x15f780),
            m.mul(particleVelocity[1],particleVelocity[1],0x15f784),0x15f788),
            m.mul(particleVelocity[2],particleVelocity[2],0x15f78c),0x15f78c);
        float step[3]{};
        if(squared!=0){const auto inverse=m.div(1,root(m,squared,0x15f7bc),0x15f7bc);
            for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(particleVelocity[axis],inverse,0x15f7ccu+12u*axis);}
        const auto separation=m.mul(f.particleSeparation208,f.initialSize184,0x15f7fc);
        for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(step[axis],separation,0x15f800u+12u*axis);
        const float lengthRandom=draw();float length;
        if(f.movingEmitter21C){const auto spreadLength=m.mul(m.sub(f.maxStreamLength204,f.minStreamLength200,0x15f85c),lengthRandom,0x15f860);
            length=m.add(.5f,m.add(1.f,m.mul(speedFactor,m.add(m.sub(f.minStreamLength200,1.f,0x15f864),
                spreadLength,0x15f868),0x15f86c),0x15f870),0x15f87c);
        }else length=m.add(.5f,m.add(f.minStreamLength200,m.mul(m.sub(f.maxStreamLength204,f.minStreamLength200,
            0x15f8bc),lengthRandom,0x15f8c0),0x15f8c4),0x15f8c8);
        const auto sections=m.cvtWord(length,f.movingEmitter21C?0x15f880u:0x15f8ccu);
        const auto sectionFloat=m.integer(sections,0x15f8e0);
        if(m.badPC)return arithmetic();
        if(sections<=0)return fail(Status::unsupported,0x15f91c);
        const auto reciprocal=m.div(1,sectionFloat,0x15f91c);
        for(unsigned axis=0;axis<3;++axis)particleVelocity[axis]=m.add(particleVelocity[axis],
            m.mul(sectionFloat,step[axis],0x15f8f8u+4u*axis),0x15f928u+12u*axis);
        for(std::int32_t sectionIndex=0;sectionIndex<sections;++sectionIndex){
            const auto thickness=m.add(.5f,m.add(f.tailThickness210,m.mul(reciprocal,m.mul(
                m.integer(sectionIndex,0x15f95c),m.sub(f.headThickness214,f.tailThickness210,0x15f960),0x15f964),
                0x15f968),0x15f96c),0x15f978);
            auto copies=m.cvtWord(thickness,0x15f97c);
            if(sectionIndex==sections-1&&f.superbrightHead218){if(copies==std::numeric_limits<std::int32_t>::max())
                return fail(Status::unsupported,0x15f99c);++copies;}
            const auto ratio=m.div(m.integer(sectionIndex+1,0x15f9bc),sectionFloat,0x15f9c0);
            const float taper=std::max(word(0x3dcccccdu),ratio); //MAX.S15F9CC, not POW.
            const auto complement=m.sub(1,taper,0x15f9f4);
            if(m.badPC)return arithmetic();
            for(std::int32_t copy=0;copy<copies;++copy){
                auto* p=manager.nextParticle();if(!p){r.poolFull=true;return finish();}
                p->ageMilliseconds=0;p->position={point[0],point[1],point[2]};p->positionW=1;
                p->frame=currentFrame_;p->growth=growth;p->size=p->initialSize=f.initialSize184;p->ownerId=managerOwnerId_;
                //15FA48's zero-growth flag clear is overwritten unconditionally
                // by15FA9C. Do not propagate stale free-slot flags into a particle.
                p->lifeMilliseconds=m.sub(life,m.mul(complement,m.mul(f.tailLife20C,life,0x15fa74),0x15fa78),0x15fa7c);
                p->inverseLife=m.div(1,p->lifeMilliseconds,0x15fa88);
                p->velocity={particleVelocity[0],particleVelocity[1],particleVelocity[2]};p->velocityW=1;p->flags=flags;
                if(m.badPC)return arithmetic();
                if(!manager.commitParticle(managerOwnerId_))return fail(Status::storage,0x15fab0);
                ++r.spawned;
            }
            const auto taperSquared=m.mul(taper,taper,0x15fae0);
            particleVelocity[0]=m.sub(particleVelocity[0],m.mul(step[0],taperSquared,0x15faf4),0x15faf8);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(complement,step[1],0x15fb04),0x15fb08);
            particleVelocity[2]=m.sub(particleVelocity[2],m.mul(step[2],taperSquared,0x15fb14),0x15fb18);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(step[1],taper,0x15fb24),0x15fb28);
            for(unsigned axis=0;axis<3;++axis)point[axis]=m.add(point[axis],step[axis],0x15fb34u+12u*axis);
            if(m.badPC)return arithmetic();
        }
        if(currentFrame_==std::numeric_limits<std::int32_t>::max())return fail(Status::unsupported,0x15fb68);
        ++currentFrame_;if(currentFrame_>lastFrame_)currentFrame_=firstFrame_;
    }
    if(m.badPC)return arithmetic();return finish();
}
} // namespace gb_type11_numeric50GB
