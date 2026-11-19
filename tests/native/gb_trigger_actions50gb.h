#pragma once
#include "gb_trigger_authoring50gb.h"
#include "track_animation_runtime50an.h"
#include "gb_pod_sound50gb.h"
#include "gb_particle_runtime50gb.h"
#include "gb_particle_render50gb.h"
#include "../../src/game/trigger_sphere50in.h"
#include <array>
#include <memory>
#include <thread>

namespace gb_query50GB { class Binding; }
namespace gb_trigger_actions50GB {
namespace g=rr::game;namespace source=gb_trigger_authoring50GB;
inline constexpr std::size_t racers=8,colliders=racers*2;
inline constexpr std::uint32_t firstTriggerIdentity=0x40000000u,firstTriggerModelIdentity=0x41000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
struct ActionLease {
 // The later actual Bank/query issuer must lend these exact current scalars
 // only until the query ends, before Bank restore. This type issues no borrow.
 std::uint32_t racerMain690=0,queryCollider=0;std::uint32_t* nodeC80=nullptr;
 const g::Vector3_48C* listeners=nullptr;std::size_t listenerCount=0;
};
struct Counters {
 std::uint64_t entries=0,exits=0,lostAiWrites=0,animationActivations=0,soundStarts=0,particleActivations=0;
};
enum class Status {Complete,Unprepared,Input,Service,Action,Capacity};

// Genuine source/destination owner and concrete action bodies. Live query
// issuance is deliberately absent until the real renderer/root binding exists.
// prepare and racer identity staging never publish group.ready.
class Owner final {
 friend class ::gb_query50GB::Binding;
 std::shared_ptr<gb_particle_render50GB::Owner> renderer_;
 const ::gb_query50GB::Binding* queryBinding_=nullptr;
 bool retiring_=false;
 std::shared_ptr<const source::Owner> source_;
 std::shared_ptr<track_animation_runtime50AN::Owner> animations_; // the common track-animation runtime
 std::shared_ptr<gb_pod_sound50GB::Owner> sounds_;
 std::shared_ptr<gb_particle_runtime50GB::Owner> particles_;
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 g::NativeTriggerGroup50GB group_{};
 struct Geometry {g::BodyMatrix48G world{};std::array<float,3> center{},half{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 // Only the genuine Bank binding installs this nonescaping current-arena loan.
 const ActionLease* lease_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,sourcePrepared_=false,bindAttempted_=false,identitiesBound_=false;
 bool targetsBound_=false,servicing_=false,closed_=false;
 Counters counters_{};Status terminal_=Status::Complete;std::string error_;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 // The particle renderer exists exactly when the course authors emitters
 // (the GB particle owners); without one a query/service has no renderer leg.
 bool beginRendererQuery()noexcept{return particles_?renderer_&&renderer_->beginActionQuery(*this):!renderer_;}
 void endRendererQuery()noexcept{if(renderer_)renderer_->endActionQuery(*this);}
 bool beginRendererService()noexcept{return particles_?renderer_&&renderer_->beginWorldService(*this):!renderer_;}
 void endRendererService()noexcept{if(renderer_)renderer_->endWorldService(*this);}
 static void increment(std::uint64_t& v)noexcept{if(v!=~std::uint64_t{0})++v;}
 std::size_t colliderIndex(std::uint32_t id)const noexcept{return g::nativeTriggerCollider50GB(group_,id);}
 Status fail(Status status)noexcept {
  // Hooks may not mutate native contact/group storage. The real source
  // dispatcher latches actionsFailed after receiving this failure.
  if(terminal_==Status::Complete)terminal_=status;return terminal_;
 }
 bool reject(Status status)noexcept{fail(status);return false;}
 static bool reservedIdentity(std::uint32_t id)noexcept {
  return (id>=firstTriggerIdentity&&id<firstTriggerIdentity+source::capacity)||
   (id>=firstTriggerModelIdentity&&id<firstTriggerModelIdentity+source::capacity)||
   (id>=track_animation_runtime50AN::firstCollisionIdentity&&id<track_animation_runtime50AN::firstCollisionIdentity+rr::assets::TrackAnimationLimits50AN{}.animations)||
   (id>=track_animation_runtime50AN::firstModelIdentity&&id<track_animation_runtime50AN::firstModelIdentity+rr::assets::TrackAnimationLimits50AN{}.animations);
 }
 bool targetsCurrent()const noexcept {
  // No Particles action authored: no spark targets are bound.
  if(!source_||!source_->authorsParticleActions())return !particles_||!particles_->actions_;
  if(!particles_||!particles_->sourceStaged()||particles_->actions_!=this||
     particles_->group_!=&group_||particles_->actionBindings_!=5)return false;
  for(std::size_t i=0;i<5;++i){const auto& target=particles_->sparks_[i];
   if(!target.sourceCurrent()||!target.graphCurrent()||!target.actionBound_||target.actions_!=this||
      target.group_!=&group_||target.triggerSource_.get()!=source_.get()||
      target.actionGeneration_!=target.generation_||target.attachmentIndex_!=62+i||
      target.descriptor_->ordinal!=8+i||target.failed_||target.quiesced_)return false;
  }
  return true;
 }
 bool structuralEvent(const g::SourceTriggerActionCall50GB& event,bool reached)const noexcept {
  const auto& call=event.source;
  if(!source_||!preparedSourceFor(*source_)||!identitiesBound_||!group_.ready||(particles_&&particles_->visiting_)||
     !g::validNativeTriggerGroup50GB(group_)||call.triggerCollider<firstTriggerIdentity)return false;
  const auto t=call.triggerCollider-firstTriggerIdentity;const auto c=colliderIndex(call.otherCollider);
  if(t>=source_->liveCount()||t>=group_.triggers.size()||c>=group_.colliders.size())return false;
  const auto& authored=source_->source(t);
  if(call.actionIndex>=authored.actionCount||!authored.actions[call.actionIndex].original)return false;
  if(!call.entering)return servicing_&&!lease_&&!group_.queryActive&&event.querySerial==0&&
   call.sourceSite==0x2a0d20u&&event.candidateOrdinal<g::triggerContactCapacity50IN;
  if(servicing_||!lease_||!group_.queryActive||!event.querySerial||event.querySerial!=group_.querySerial||
     call.sourceSite!=0x2a0eccu||group_.queryCollider!=call.otherCollider||
     lease_->queryCollider!=call.otherCollider||lease_->racerMain690!=racers_[c/2].main690||
     !g::nativeTriggerPairAllowed50GB(group_,t,call.otherCollider))return false;
  const auto& pending=group_.contact;using P=g::SourceTriggerContactPhase50GB;
  if(!reached)return pending.phase==P::Idle||pending.phase==P::Complete;
  return pending.phase==P::Actions&&pending.group==&group_&&pending.hooks==&group_.actions&&
   pending.querySerial==event.querySerial&&pending.candidateOrdinal==event.candidateOrdinal&&
   pending.triggerIndex==t&&pending.colliderIndex==c&&pending.triggerCollider==call.triggerCollider&&
   pending.otherCollider==call.otherCollider&&pending.actionCount==authored.actionCount&&
   pending.nextAction==call.actionIndex&&(group_.triggers[t].flags1B8&0x18u)==0x18u;
 }
 bool actionCurrent(const g::SourceTriggerActionCall50GB& event,bool reached)const noexcept {
  if(!structuralEvent(event,reached))return false;const auto& call=event.source;
  if(!call.entering)return true;
  const auto t=call.triggerCollider-firstTriggerIdentity;const auto c=colliderIndex(call.otherCollider);
  const auto& action=source_->source(t).actions[call.actionIndex];
  // LostAI: any trigger/action slot the TOB authors (Reset absent/false).
  if(action.kind==source::Action::LostAI)
   return (!action.authoredReset||!*action.authoredReset)&&!action.effectiveReset1B5A60&&
    action.targetNodeIndex<source_->coldSource()->ai().count()&&((c&1u)||lease_->nodeC80);
  // Animation: the common track-animation runtime holds every row the TOB's
  // [Animations] loads. PORT STAND-IN (flagged): the PodSound/Particles
  // destinations are GB's stock-shaped owners (stock TOB only,
  // gb_trigger_authoring50gb.h), so these keep the stock trigger indices.
  if(action.kind==source::Action::Animation)
   return animations_&&animations_->ready()&&action.animationIndex<animations_->rowCount();
  if(action.kind==source::Action::PodSound)
   return t==source::door&&call.actionIndex==1&&lease_->listenerCount<=2&&
    (!lease_->listenerCount||lease_->listeners)&&sounds_&&sounds_->readyFor(*source_,call.actionIndex);
  if(action.kind!=source::Action::Particles||!particles_||t!=source::particles||call.actionIndex>=5)return false;
  const auto& target=particles_->sparks_[call.actionIndex];
  // Entry preflight is pure before the source contact cursor is installed.
  // Reached dispatch validates that cursor instead; re-running the preflight
  // phase check here would incorrectly reject genuine Actions-phase calls.
  return reached?target.matchesAction(*this,*source_,action,event,true):
   target.preflightSourceAction(*this,*source_,action,event);
 }
 bool preflight(const g::SourceTriggerActionCall50GB& event)const noexcept {return actionCurrent(event,false);}
 g::SourceTriggerActionResult50GB resume(const g::SourceTriggerActionCall50GB& event)noexcept {
  using A=g::SourceTriggerActionStatus50GB;const auto& call=event.source;
  if(!thread())return {A::InvalidBinding,call.sourceSite,0};
  try {
   if(!actionCurrent(event,true)){fail(Status::Action);return {A::InvalidBinding,call.sourceSite,0};}
   if(!call.entering){increment(counters_.exits);return {A::Complete,call.sourceSite,0};}
   increment(counters_.entries);
   const auto& action=source_->source(call.triggerCollider-firstTriggerIdentity).actions[call.actionIndex];
   if(action.kind==source::Action::LostAI){
    // Source scans actual main690 identities in race order. A secondary
    // collider completes without a C80 store. Every GB Reset is absent/false.
    for(std::size_t i=0;i<racerCount_;++i)if(racers_[i].main690==call.otherCollider){
     *lease_->nodeC80=action.targetNodeIndex;increment(counters_.lostAiWrites);}
    return {A::Complete,0x1b27c0u,0xc80u};
   }
   if(action.kind==source::Action::Animation){
    if(!animations_->fire(action.animationIndex,static_cast<std::uint32_t>(call.triggerCollider-firstTriggerIdentity+1))){
     fail(Status::Action);return {A::Blocked,0x101170u,0x90u};}
    increment(counters_.animationActivations);return {A::Complete,0x101170u,0x90u};
   }
   if(action.kind==source::Action::Particles){
    const auto result=particles_->sparks_[call.actionIndex].activateSourceAction(*this,*source_,action,event);
    if(result.status!=A::Complete){fail(Status::Action);return result;}
    increment(counters_.particleActivations);return result;
   }
   const auto starts=sounds_->counters().starts;
   if(!sounds_->playEntering(*source_,call.actionIndex,action,lease_->listeners,lease_->listenerCount,error_)){
    fail(Status::Action);return {A::Blocked,0x1b5b58u,0x20u};}
   if(sounds_->counters().starts!=starts){increment(counters_.soundStarts);return {A::Complete,0x1b5b58u,0x20u};}
   return {A::Complete,0x1b5b60u,0u};
  }catch(...){fail(Status::Action);return {A::Blocked,call.sourceSite,0};}
 }
 static bool preflightHook(void* owner,const g::SourceTriggerActionCall50GB& event)noexcept {
  return static_cast<Owner*>(owner)->preflight(event);
 }
 static g::SourceTriggerActionResult50GB resumeHook(void* owner,const g::SourceTriggerActionCall50GB& event)noexcept {
  return static_cast<Owner*>(owner)->resume(event);
 }
 // Concrete ordinary source exit/removal body, inaccessible until the real
 // live binding supplies an authenticated world-service entry in a successor.
 Status serviceWorld()noexcept {
  if(!thread())return Status::Unprepared;
  if(!source_||!preparedSourceFor(*source_)||!identitiesBound_||!group_.ready)return fail(Status::Unprepared);
  if(!queryIdle()||!g::validNativeTriggerGroup50GB(group_))return fail(Status::Input);
  servicing_=true;struct End{bool& flag;~End(){flag=false;}}end{servicing_};
  for(std::size_t t=0;t<group_.triggers.size();++t){const auto& state=group_.triggers[t];const auto& authored=source_->source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0;++slot){const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c==group_.colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];bool overlap=false;g::TriggerSphereRootExitInput50IN input{};
    input.type2PreparedWorld=geometry.world;
    for(unsigned k=0;k<3;++k){input.sphere.localCenter.v[k]=authored.sphere.center.v[k];
     input.rootCenter.v[k]=geometry.center[k];input.rootHalf.v[k]=geometry.half[k];}
    input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
    for(unsigned k=0;k<4;++k)input.sphere.world.v[k*5]=1.f;
    input.sphere.radius50=authored.sphere.radius;volatile float square=authored.sphere.radius*authored.sphere.radius;
    input.sphere.radiusSquared54=square;
    if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)
     return fail(Status::Service);
    observations[slot]={&group_.colliders[c],overlap};
   }
   const auto receipt=g::serviceNativeSourceTrigger50GB(group_,t,observations.data(),
    state.collider.highWaterD0,group_.actions,group_.contact);
   if(receipt.status!=g::SourceTriggerContactStatus50GB::Complete)return fail(Status::Service);
  }
  return Status::Complete;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(!thread()||lease_||servicing_||group_.queryActive||targetsBound_||queryBinding_)std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  // Conservative fixed CPU scratch for later roster validation and exit
  // service is charged with the retained owner. Destination heaps, control
  // blocks and bounded failure diagnostics remain in their actual domains.
  return sizeof(Owner)+sizeof(g::NativeTriggerGroup50GB)+sizeof(std::array<std::uint32_t,colliders>)+
   sizeof(std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN>)+
   sizeof(g::TriggerSphereRootExitInput50IN)+sizeof(Geometry);
 }
 bool prepare(std::shared_ptr<const source::Owner> authored,
  std::shared_ptr<track_animation_runtime50AN::Owner> animations,std::shared_ptr<gb_pod_sound50GB::Owner> sounds,
  std::shared_ptr<gb_particle_runtime50GB::Owner> particles,std::size_t availableAdditionalPayloadBytes)noexcept {
  if(!thread()||attempted_||closed_)return false;attempted_=true;
  if(availableAdditionalPayloadBytes>32u*1024u*1024u||availableAdditionalPayloadBytes<maximumAdditionalPayloadBytes())
   return reject(Status::Capacity);
  // Each destination owner exists exactly when the course data authors it:
  // the animation runtime when the TOB authors [Animations], PodSound and the
  // particle runtime when a trigger action needs them. A present owner is
  // bound; an action kind with no owner is refused (authoring already refuses
  // a non-stock TOB that authors them).
  if(!authored||!authored->ready()||!authored->coldSource()||!authored->coldSource()->ai().count()||
     (authored->authorsAnimationActions()&&!animations)||
     (animations&&!animations->ready())||
     authored->authorsPodSound()!=bool(sounds)||(sounds&&!sounds->readyFor(*authored,1))||
     (authored->authorsParticleActions()&&!particles)||
     (particles&&(!particles->sourceStaged()||particles->actions_||particles->group_||particles->actionBindings_||
      !particles->sourceOwner()||particles->sourceOwner()->sourceOwner()!=authored->coldSource())))
   return reject(Status::Unprepared);
  // Live trigger prefix of the capacity group (0 = no trigger state selected).
  const auto live=authored->liveCount();
  if(live>source::capacity||(live&&!group_.triggers.setCount(live)))return reject(Status::Capacity);
  for(std::size_t i=0;i<live;++i){const auto& s=authored->source(i);auto& state=group_.triggers[i];
   if(s.sourceOrdinal!=i+1||s.shapeType!=5||!s.actionCount||s.actionCount>source::maxActions||
      s.randomTriggerPercent!=100)return reject(Status::Input);
   state.flags29=s.startEnabled?3:2;state.flags1B8=static_cast<std::uint8_t>(3u|(s.oneTimeOnly?4u:0u));
   state.randomTriggerPercent90=100;state.callbackCount98=s.actionCount;
   state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  }
  source_=std::move(authored);animations_=std::move(animations);sounds_=std::move(sounds);particles_=std::move(particles);
  group_.actions={this,&preflightHook,&resumeHook};
  // Set cleanup ownership before the fallible five-target binding. Its real
  // accepted prefix remains retained if any later target rejects.
  // The five spark targets bind only when the triggers author Particles.
  targetsBound_=particles_&&source_->authorsParticleActions();
  if(targetsBound_&&!particles_->bindActionSource(*this,group_,source_)){
   // No query has been admitted. This closes the failed construction prefix
   // and enables explicit release of any partially bound target cursors.
   group_.actionsFailed=true;return reject(Status::Unprepared);}
  sourcePrepared_=true;
  if(!preparedSourceFor(*source_)){group_.actionsFailed=true;return reject(Status::Unprepared);}
  return true;
 }
 // soundDestination=false skips only the PodSound mixer/voice re-validation
 // (its full shared race-audio check is costly). Per-query geometry admission
 // uses that form; the PodSound play path still runs its own complete check
 // (readyFor/playEntering) before any sound starts, and cold/world-service
 // admission keeps the full form.
 bool preparedSourceFor(const source::Owner& authored,bool soundDestination=true)const noexcept {
  return thread()&&!closed_&&sourcePrepared_&&terminal_==Status::Complete&&source_.get()==&authored&&authored.ready()&&
   !group_.actionsFailed&&group_.actions.context==this&&group_.actions.preflight==&preflightHook&&
   group_.actions.resume==&resumeHook&&
   (!animations_||animations_->ready())&&
   (!sounds_||(sounds_->sourceIdentity()==&authored&&(!soundDestination||sounds_->prepared())))&&
   (!particles_||(particles_->sourceOwner()&&particles_->sourceOwner()->sourceOwner()==authored.coldSource()))&&targetsCurrent();
 }
 // Stages the actual eight-racer roster and source PLAYER/AI main690 lists.
 // It never publishes live readiness or creates a query/Bank borrow.
 bool bindRacerIdentities(const std::array<RacerIdentities,racers>& identities,
  std::uint32_t actualPlayerPrefixCount34,std::size_t racerCount=racers)noexcept {
  if(!thread()||bindAttempted_||!source_||!preparedSourceFor(*source_)||group_.ready||!queryIdle())return false;
  bindAttempted_=true;if(!racerCount||racerCount>racers||actualPlayerPrefixCount34>racerCount)return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||reservedIdentity(ids[i]))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  auto next=group_;
  if(!next.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)next.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<next.triggers.size();++i){const auto ignore=source_->source(i).ignore;
   const auto first=ignore==source::Ignore::AI?actualPlayerPrefixCount34:0u;
   const auto last=ignore==source::Ignore::Player?actualPlayerPrefixCount34:ignore==source::Ignore::AI?racerCount:0u;
   for(std::size_t j=first;j<last;++j)next.ignoreColliders[i][next.ignoreColliderCount[i]++]=identities[j].main690;
  }
  // A temporary scalar copy is validated; the stable published group remains
  // closed. No source callback, current query or target view is copied.
  next.ready=true;if(!g::validNativeTriggerGroup50GB(next))return false;next.ready=false;
  racers_=identities;racerCount_=racerCount;group_=std::move(next);identitiesBound_=true;return true;
 }
private:
 // Stores only real prepared root geometry scalars for later source exits.
 // This observation cannot open query or world-service admission.
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
  const g::BodyMatrix48G& world)noexcept {
  if(!thread()||!source_||!preparedSourceFor(*source_,false)||!identitiesBound_||servicing_||
     (lease_?(!group_.queryActive||lease_->queryCollider!=collider):!queryIdle())||
     !resource.nodes||!resource.nodeCount||!resource.rays||!resource.rayCount||
     (resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||
     !g::trigger_sphere50in_detail::affine(world))return false;
  const auto c=colliderIndex(collider);if(c==group_.colliders.size())return false;
  const auto& root=resource.nodes[resource.root];Geometry next{};next.world=world;
  for(unsigned k=0;k<3;++k){if(!std::isfinite(root.center[k])||!std::isfinite(root.half[k])||root.half[k]<0.f)return false;
   next.center[k]=root.center[k];next.half[k]=root.half[k];}
  next.valid=true;geometry_[c]=next;return true;
 }
public:
 // Renderer leg: present exactly with the particle owners (see beginRendererQuery).
 bool rendererCurrent()const noexcept {
  return particles_?renderer_&&renderer_->ready()&&renderer_->runtimeOwner()==particles_.get():!renderer_;
 }
 bool ready()const noexcept {
  return thread()&&!retiring_&&!closed_&&queryBinding_&&rendererCurrent()&&source_&&preparedSourceFor(*source_)&&identitiesBound_&&
   (!animations_||animations_->ready())&&(!sounds_||sounds_->prepared())&&group_.ready&&!group_.actionsFailed;
 }
 // ready() without the PodSound destination re-validation; see preparedSourceFor.
 bool queryReady()const noexcept {
  return thread()&&!retiring_&&!closed_&&queryBinding_&&rendererCurrent()&&source_&&preparedSourceFor(*source_,false)&&identitiesBound_&&
   (!animations_||animations_->ready())&&group_.ready&&!group_.actionsFailed;
 }
 bool hasParticleOwner()const noexcept{return bool(particles_);}
 bool particleIdle()const noexcept{return !particles_||!particles_->visiting_;}
 bool queryIdle()const noexcept{return thread()&&!lease_&&!servicing_&&!group_.queryActive;}
 const source::Owner* sourceOwner()const noexcept{return source_.get();}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return source_?source_->coldSource():nullptr;}
 const track_animation_runtime50AN::Owner* animationOwner()const noexcept{return animations_.get();}
 const gb_pod_sound50GB::Owner* soundOwner()const noexcept{return sounds_.get();}
 const gb_particle_runtime50GB::Owner* particleOwner()const noexcept{return particles_.get();}
 g::NativeTriggerGroup50GB& nativeGroup()noexcept{return group_;}
 const g::NativeTriggerGroup50GB& nativeGroup()const noexcept{return group_;}
 const Counters& counters()const noexcept{return counters_;}
 Status status()const noexcept{return terminal_;}
 const std::string& lastError()const noexcept{return error_;}
 bool closed()const noexcept{return thread()&&closed_;}
 bool close()noexcept {
  if(!thread()||!queryIdle()||queryBinding_)return false;if(closed_)return true;
  // The root must first remove every real Bank peer's trigger membership.
  // Verify the complete retained native-group side; do not erase contacts or
  // pretend this local check proves external Bank/navigation retirement.
  for(const auto& state:group_.triggers)
   for(const auto id:state.collider.membersCC)if(id)return false;
  for(const auto& collider:group_.colliders)
   for(const auto id:collider.membersCC)if(id)return false;
  const auto phase=group_.contact.phase;
  if(phase!=g::SourceTriggerContactPhase50GB::Idle&&phase!=g::SourceTriggerContactPhase50GB::Complete&&
     !group_.actionsFailed)return false;
  group_.ready=false;
  if(targetsBound_){
   if(!particles_)return false;
   // bindActionSource can reject before retaining this owner at all.
   if(particles_->actions_==this){if(!particles_->unbindActionSource(*this))return false;}
   else if(particles_->actions_||particles_->actionBindings_)return false;
   targetsBound_=false;
  }
  group_.actions={};sourcePrepared_=false;closed_=true;
  renderer_.reset();particles_.reset();sounds_.reset();animations_.reset();source_.reset();return true;
 }
};
} // namespace gb_trigger_actions50GB
