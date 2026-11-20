#pragma once
// Common rewrite (R5, PLAN B-C13): the ONE trigger action owner of the race path,
// for every course. Generalised from the MC chain (mc_trigger_actions50mc.h,
// the only one in 2A00A0 order, common-race 3.7); it replaces the 11 per-course
// *_trigger_actions* owners and their candidate/geometry companions.
//
// Original: 2A00A0 trigger entry, 2A0E20 entering actions (callbacks 98), 2A0B30
// world service (exits), 2A013C chance roll (2DBDA0 RandomTriggerPercent),
// 2DC630 ignore lists. Actions: LostAI 1B27C0 (racer C80 = TargetNodeIndex,
// 1B5A60 Reset), Animation 101170 (track animation fire), PodSound 1B5B58,
// Sound and Particles through their owners.
//
// Every course goes through the same code. What a course authors comes from
// its TOB (course_trigger_sources50CT); what the race can EXECUTE comes from the
// destinations bound at prepare(): the one track-animation runtime for
// Animation, and one sink per sound/particle/pose-reset kind supplied by the
// owner of that resource (R7 sounds and particles, I2/I3 for the pose reset and
// the gate voice). A kind the TOB authors with no bound destination is REFUSED
// at prepare (logged), never ignored. PORT STAND-IN (flagged): that refusal
// stands in for whatever destination has not been wired yet.
//
// Storage: the group (rr::game::NativeTriggerGroup50MC, Track::roles.triggers)
// is sized once from the TOB's live trigger count; every per-trigger access is
// bounded by that count. Nothing writes guest memory: the only external write
// is the racer's C80 node through the caller's ActionLease, whose pointer the
// caller proves for the duration of one query.
#include "course_trigger_sources50ct.h"
#include "track_animation_runtime50an.h"
#include "../../src/game/native_trigger_group50mc.h"
#include "../../src/game/native_trigger_box_geometry50ma.h"
#include "../../src/game/trigger_sphere50in.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/live_zone_membership50en.h"
#include "../../src/game/zone_lifecycle50cl.h"
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace course_trigger_actions50CT {
namespace g=rr::game;namespace src=course_trigger_sources50CT;
inline constexpr std::size_t racers=8,colliders=racers*2;   // the race field (1AD520: 1..8), main + secondary
// Native registry identities of the trigger candidates (one namespace for every
// course; not guest addresses). Trigger k = first + k.
inline constexpr std::uint32_t firstTriggerIdentity=0x4a000000u,firstTriggerModelIdentity=0x4b000000u;
struct RacerIdentities {std::uint32_t main690=0,secondary=0;};
// The current query's borrow, lent by the genuine query issuer for one query only.
struct ActionLease {
 std::uint32_t racerMain690=0,queryCollider=0;
 std::uint32_t* nodeC80=nullptr;                       // the querying racer's C80 (LostAI destination)
 const g::Vector3_48C* listeners=nullptr;std::size_t listenerCount=0; // PodSound/Sound listeners (camera)
};
// One authored action reaching its destination.
struct ActionEvent {
 std::size_t trigger=0,action=0;                       // 0-based trigger row and action slot
 const src::Source* source=nullptr;const src::ActionSource* authored=nullptr;
 std::uint32_t otherCollider=0;std::size_t racerSlot=0;bool mainCollider=false;
 bool entering=false;                                  // exits run only when the trigger's flags1B8 bit0 is set
 const ActionLease* lease=nullptr;                     // null on the 2A0B30 service
 std::uint64_t querySerial=0;std::size_t candidateOrdinal=0; // the issuing query (0 on the 2A0B30 service)
};
// A destination owned elsewhere. preflight is pure (no effects, no dependence on
// earlier actions); apply performs the action and returns false on failure.
struct ActionSink {
 void* context=nullptr;
 bool (*preflight)(void*,const ActionEvent&)noexcept=nullptr;
 bool (*apply)(void*,const ActionEvent&)noexcept=nullptr;
 std::uint32_t sourcePc=0;                             // original address reported in the action result
 explicit operator bool()const noexcept{return context&&preflight&&apply;}
};
// LostAI Reset TRUE (1B27C0: C80 store at 1B282C, then 2255B0 on the matching
// racer at 1B283C). 2255B0 runs on that racer inside its own main690 E8 query,
// with that racer's live frame, recovery owners and the query's outer-result
// binding, which only the query issuer holds and only for the length of one
// query. So the one sink of every course is a slot: bound once into
// Destinations.lostAiReset at cold, filled by the issuer per racer query
// (Scope), empty otherwise. The issuer's apply performs
// native_original_ai_recovery::resumeTrigger1B27C0 (original_ai_physical_recovery.h).
// PORT STAND-IN (flagged): 2255B0's player branch reached from a trigger (a
// human racer's main690 entering a Reset LostAI trigger) is not carried: the
// issuer binds no reset for a human query, so its preflight refuses (the owner
// logs the refusal). No stock course authors it: MB's UpShaft, the only Reset
// LostAI trigger, ignores PLAYER.
class LostAiResetSink final {
public:
 struct Binding {
  void* context=nullptr;
  std::size_t racerSlot=0;                              // the visited Bank slot of this query
  std::uint32_t main690=0;                              // that racer's main690 identity
  bool (*preflight)(void*,const ActionEvent&)noexcept=nullptr; // pure: this racer's reset owners are present
  bool (*apply)(void*,const ActionEvent&)noexcept=nullptr;     // performs 2255B0 for this racer
  explicit operator bool()const noexcept{return context&&preflight&&apply&&main690;}
 };
 // Lent by the query issuer for exactly one racer query (nested scopes refused).
 class Scope final {
  LostAiResetSink* sink_=nullptr;
 public:
  Scope(LostAiResetSink& sink,const Binding& binding)noexcept{
   if(!sink.current_&&binding){sink.current_=&binding;sink_=&sink;}
  }
  Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
  ~Scope(){if(sink_)sink_->current_=nullptr;}
  explicit operator bool()const noexcept{return sink_;}
 };
 LostAiResetSink()=default;LostAiResetSink(const LostAiResetSink&)=delete;LostAiResetSink& operator=(const LostAiResetSink&)=delete;
 ~LostAiResetSink(){if(current_)std::terminate();}
 ActionSink sink()noexcept{return {this,&preflightThunk,&applyThunk,0x2255b0u};}
private:
 const Binding* current_=nullptr;
 bool matches(const ActionEvent& e)const noexcept{
  return current_&&e.lease&&e.racerSlot==current_->racerSlot&&e.lease->racerMain690==current_->main690&&
   e.otherCollider==current_->main690;
 }
 // A secondary collider completes without the C80 store or the reset (1B27C0
 // scans main690 only); the owner never calls apply for it.
 static bool preflightThunk(void* p,const ActionEvent& e)noexcept{
  const auto& self=*static_cast<const LostAiResetSink*>(p);
  if(!e.entering||!e.authored||e.authored->kind!=src::ActionKind::LostAI||!e.authored->reset)return false;
  if(!e.mainCollider)return true;
  return self.matches(e)&&self.current_->preflight(self.current_->context,e);
 }
 static bool applyThunk(void* p,const ActionEvent& e)noexcept{
  auto& self=*static_cast<LostAiResetSink*>(p);
  if(!e.mainCollider)return true;
  return self.matches(e)&&self.current_->apply(self.current_->context,e);
 }
};
struct Destinations {
 std::shared_ptr<track_animation_runtime50AN::Owner> animations; // Animation 101170 (absent when the TOB has no [Animations])
 ActionSink podSound;      // PodSound 1B5B58 (the gate/pod voice owner)
 ActionSink sound;         // Sound (the one positional-sound owner)
 ActionSink particles;     // Particles (the one particle owner's emitter switch)
 ActionSink lostAiReset;   // LostAI Reset TRUE: the racer pose reset beside the C80 write
};
struct Counters {std::uint64_t entries=0,exits=0,lostAiWrites=0,animationActivations=0,sinkActions=0;};
enum class Status {Complete,Unprepared,Input,Service,Action};

class Owner final {
 std::shared_ptr<const src::Owner> source_;
 std::shared_ptr<g::NativeTriggerGroup50MC> group_;
 Destinations destinations_;
 LostAiResetSink lostAiReset_; // lent per racer query by the frame (one owner lifetime, outlives every query)
 std::array<RacerIdentities,racers> racers_{};
 std::size_t racerCount_=0;
 struct Geometry {g::BodyMatrix48G world{};std::array<float,3> center{},half{};bool valid=false;};
 std::array<Geometry,colliders> geometry_{};
 const ActionLease* lease_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,sourcePrepared_=false,destinationsBound_=false,bindAttempted_=false,ready_=false,servicing_=false,closed_=false,queryBound_=false;
 Counters counters_{};Status terminal_=Status::Complete;std::string error_;
 static void increment(std::uint64_t& v)noexcept{if(v!=~std::uint64_t{0})++v;}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 std::size_t colliderIndex(std::uint32_t id)const noexcept{return group_?g::nativeTriggerCollider50MC(*group_,id):0u;}
 Status fail(Status s)noexcept{if(terminal_==Status::Complete)terminal_=s;return terminal_;}
 bool refuse(const char* why)noexcept{try{error_=why;}catch(...){}std::cerr<<"course_trigger_actions refused: "<<why<<'\n';return false;}
 const ActionSink* sinkFor(const src::ActionSource& a)const noexcept{
  switch(a.kind){
   case src::ActionKind::PodSound:return &destinations_.podSound;
   case src::ActionKind::Sound:return &destinations_.sound;
   case src::ActionKind::Particles:return &destinations_.particles;
   default:return nullptr;
  }
 }
 bool eventFor(const g::SourceTriggerActionCall50MC& call,ActionEvent& e)const noexcept{
  const auto& c=call.source;
  if(!group_||c.triggerCollider<firstTriggerIdentity)return false;
  const auto t=static_cast<std::size_t>(c.triggerCollider-firstTriggerIdentity);const auto k=colliderIndex(c.otherCollider);
  if(t>=source_->liveCount()||t>=group_->triggerCount||k>=group_->colliders.size())return false;
  const auto& trigger=source_->source(t);if(c.actionIndex>=trigger.actions.size())return false;
  e={t,c.actionIndex,&trigger,&trigger.actions[c.actionIndex],c.otherCollider,k/2,(k&1u)==0u,c.entering,c.entering?lease_:nullptr,call.querySerial,call.candidateOrdinal};
  return true;
 }
 bool preflight(const g::SourceTriggerActionCall50MC& call)const noexcept {
  if(!ready()||terminal_!=Status::Complete)return false;
  ActionEvent e;if(!eventFor(call,e))return false;
  if(!e.entering)return servicing_&&!lease_&&!group_->queryActive&&call.querySerial==0;
  if(servicing_||!lease_||!group_->queryActive||!call.querySerial||call.querySerial!=group_->querySerial||
     group_->queryCollider!=e.otherCollider||lease_->queryCollider!=e.otherCollider||
     lease_->racerMain690!=racers_[e.racerSlot].main690)return false;
  const auto& a=*e.authored;
  switch(a.kind){
   case src::ActionKind::LostAI:
    // A secondary collider completes without a C80 store (1B27C0 scans main690s).
    if(!(!e.mainCollider||lease_->nodeC80))return false;
    return !a.reset||(destinations_.lostAiReset&&destinations_.lostAiReset.preflight(destinations_.lostAiReset.context,e));
   case src::ActionKind::Animation:
    return destinations_.animations&&destinations_.animations->ready()&&a.animationIndex<destinations_.animations->rowCount();
   default:{const auto* s=sinkFor(a);return s&&*s&&s->preflight(s->context,e);}
  }
 }
 g::SourceTriggerActionResult50MC resume(const g::SourceTriggerActionCall50MC& call)noexcept {
  using A=g::SourceTriggerActionStatus50MC;const auto site=call.source.sourceSite;
  try{
   if(!preflight(call)){fail(Status::Action);return {A::InvalidBinding,site,0};}
   ActionEvent e;eventFor(call,e);
   if(!e.entering){
    increment(counters_.exits);
    // 2A0B30 exit callbacks: only sink destinations act on an exit.
    if(const auto* s=sinkFor(*e.authored);s&&*s){if(!s->apply(s->context,e)){fail(Status::Action);return {A::Blocked,s->sourcePc,0};}}
    return {A::Complete,site,0};
   }
   increment(counters_.entries);const auto& a=*e.authored;
   if(a.kind==src::ActionKind::LostAI){
    for(std::size_t i=0;i<racerCount_;++i)if(racers_[i].main690==e.otherCollider){
     *lease_->nodeC80=a.targetNodeIndex;increment(counters_.lostAiWrites);
     if(a.reset&&!destinations_.lostAiReset.apply(destinations_.lostAiReset.context,e)){fail(Status::Action);return {A::Blocked,0x1b5a60u,0};}
    }
    return {A::Complete,0x1b27c0u,0xc80u};
   }
   if(a.kind==src::ActionKind::Animation){
    if(!destinations_.animations->fire(a.animationIndex,static_cast<std::uint32_t>(e.trigger+1))){fail(Status::Action);return {A::Blocked,0x101170u,0x90u};}
    increment(counters_.animationActivations);return {A::Complete,0x101170u,0x90u};
   }
   const auto* s=sinkFor(a);
   if(!s||!*s||!s->apply(s->context,e)){fail(Status::Action);return {A::Blocked,s?s->sourcePc:site,0};}
   increment(counters_.sinkActions);return {A::Complete,s->sourcePc,0};
  }catch(...){fail(Status::Action);return {A::Blocked,site,0};}
 }
 g::SourceTriggerActionHooks50MC hooks()noexcept {
  return {this,[](void* p,const g::SourceTriggerActionCall50MC& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
    [](void* p,const g::SourceTriggerActionCall50MC& c)noexcept{return static_cast<Owner*>(p)->resume(c);}};
 }
 bool beginQuery(const ActionLease& lease)noexcept {
  if(!ready()||lease_||servicing_||terminal_!=Status::Complete||group_->actionsFailed||!lease.nodeC80||group_->querySerial==~std::uint64_t{0})return false;
  const auto c=colliderIndex(lease.queryCollider);
  if(c>=group_->colliders.size()||lease.racerMain690!=racers_[c/2].main690||!g::releaseCompletedSourceContact50MC(group_->contact))return false;
  lease_=&lease;++group_->querySerial;group_->queryCollider=lease.queryCollider;group_->queryActive=true;return true;
 }
 bool endQuery(bool queryCompleted)noexcept {
  if(!lease_)return false;lease_=nullptr;group_->queryActive=false;
  if(!queryCompleted)fail(Status::Input);
  if(group_->actionsFailed||terminal_!=Status::Complete)return false;
  if(!g::releaseCompletedSourceContact50MC(group_->contact)){fail(Status::Input);return false;}
  return true;
 }
public:
 LostAiResetSink& lostAiResetSink()noexcept{return lostAiReset_;}
 // The genuine query issuer keeps ActionLease and the current bank visit alive
 // for this scope. Destruction always expires the borrowed pointers.
 class QueryBorrow final {
  Owner* owner_=nullptr;
 public:
  QueryBorrow(Owner& owner,const ActionLease& actual)noexcept{if(owner.beginQuery(actual))owner_=&owner;}
  QueryBorrow(const QueryBorrow&)=delete;QueryBorrow& operator=(const QueryBorrow&)=delete;
  ~QueryBorrow(){if(owner_)owner_->endQuery(false);}
  explicit operator bool()const noexcept{return owner_;}
  bool finish(bool actualQueryCompleted)noexcept{if(!owner_)return false;auto* p=owner_;owner_=nullptr;return p->endQuery(actualQueryCompleted);}
 };
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(lease_||servicing_||(group_&&group_->queryActive)||!thread())std::terminate();}

 // Cold, in two halves (the collision world's prepareStorage needs the source
 // half at step 10; the destinations exist only at the cold contacts).
 // prepareSource: the Track's trigger rows (roles.triggerSource), its one group
 // (roles.triggers, already sized from the live count by readTriggers274E90),
 // the AI spline's live node count (step 8) and the animation runtime (null when
 // the TOB has no [Animations]). Writes the per-trigger entry state (2A00A0,
 // 2A0290 defaults) and installs the hooks.
 bool prepareSource(std::shared_ptr<const src::Owner> source,std::shared_ptr<g::NativeTriggerGroup50MC> group,
     std::uint32_t aiNodeCount,std::shared_ptr<track_animation_runtime50AN::Owner> animations)noexcept {
  if(!thread()||attempted_)return refuse("prepare requires one owner-thread attempt");attempted_=true;
  if(!source||!source->ready()||!group||group->ready)return refuse("trigger source/group not ready");
  const auto live=source->liveCount();
  if(!group->sized||group->triggerCount!=live||group->triggers.size()!=live)return refuse("trigger group not sized to the live trigger count");
  src::Diagnostic sd;
  if(!source->checkTargets(aiNodeCount,sd))return refuse("LostAI target outside the AI spline");
  if(animations&&!animations->ready())return refuse("animation runtime not ready");
  for(std::size_t i=0;i<live;++i)if(source->source(i).randomTriggerPercent<0||source->source(i).randomTriggerPercent>100)
   return refuse("RandomTriggerPercent outside 0..100");
  for(std::size_t i=0;i<live;++i){const auto& s=source->source(i);
   auto& state=group->triggers[i];
   state.flags29=s.startEnabled?3:2;state.flags1B8=static_cast<std::uint8_t>(3u|(s.oneTimeOnly?4u:0u));
   state.randomTriggerPercent90=s.randomTriggerPercent;state.callbackCount98=s.actionCount;
   state.collider.identity=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  }
  source_=std::move(source);group_=std::move(group);destinations_={};destinations_.animations=std::move(animations);
  group_->actions=hooks();sourcePrepared_=true;return true;
 }
 // bindDestinations: every kind the TOB authors must have its destination; an
 // unbound kind is refused (logged, see the header comment). Once, after
 // prepareSource and before bindRacerIdentities; the animation runtime must be
 // the one prepareSource took.
 bool bindDestinations(Destinations destinations)noexcept {
  if(!thread()||!sourcePrepared_||destinationsBound_||bindAttempted_||ready_||closed_)
   return refuse("trigger destinations bind once, after prepareSource and before the racer identities");
  if(destinations.animations!=destinations_.animations)return refuse("trigger destinations name a different animation runtime");
  for(std::size_t i=0;i<source_->liveCount();++i){const auto& s=source_->source(i);
   for(const auto& a:s.actions){
    const bool bound=
     a.kind==src::ActionKind::LostAI?(!a.reset||bool(destinations.lostAiReset)):
     a.kind==src::ActionKind::Animation?bool(destinations.animations):
     a.kind==src::ActionKind::PodSound?bool(destinations.podSound):
     a.kind==src::ActionKind::Sound?bool(destinations.sound):bool(destinations.particles);
    if(!bound){
     std::cerr<<"course_trigger_actions refused trigger="<<(i+1)<<" kind="<<src::actionKindName(a.kind)
              <<(a.kind==src::ActionKind::LostAI?" reset=1":"")<<" reason=no_destination\n";
     return refuse("authored trigger action has no bound destination");
    }
   }
  }
  destinations_=std::move(destinations);destinationsBound_=true;return true;
 }
 // Both halves in one call (host checks and tests).
 bool prepare(std::shared_ptr<const src::Owner> source,std::shared_ptr<g::NativeTriggerGroup50MC> group,
     std::uint32_t aiNodeCount,Destinations destinations)noexcept {
  auto animations=destinations.animations;
  return prepareSource(std::move(source),std::move(group),aiNodeCount,std::move(animations))&&
   bindDestinations(std::move(destinations));
 }
 // Stages the race field's collider identities (humans first, 1AE280) and the
 // 2DC630 ignore lists: PLAYER = the human prefix [0,humans34), AI = the rest.
 bool bindRacerIdentities(const std::array<RacerIdentities,racers>& identities,std::uint32_t humans34,
     const g::OriginalTriggerChance50MC& sharedChance,std::size_t racerCount)noexcept {
  if(!thread()||bindAttempted_||!sourcePrepared_||!destinationsBound_||ready_||lease_||servicing_)return false;bindAttempted_=true;
  if(!racerCount||racerCount>racers||humans34>racerCount||!sharedChance.bound())return false;
  std::array<std::uint32_t,colliders> ids{};
  for(std::size_t i=0;i<racerCount;++i){ids[i*2]=identities[i].main690;ids[i*2+1]=identities[i].secondary;}
  for(std::size_t i=0;i<racerCount*2;++i){
   if(!ids[i]||(ids[i]>=firstTriggerIdentity&&ids[i]<firstTriggerIdentity+group_->triggerCount))return false;
   for(std::size_t j=0;j<i;++j)if(ids[i]==ids[j])return false;
  }
  auto next=*group_;next.chance=sharedChance;
  if(!next.colliders.setCount(racerCount*2))return false;
  for(std::size_t i=0;i<racerCount*2;++i)next.colliders[i].identity=ids[i];
  for(std::size_t i=0;i<next.triggerCount;++i){const auto& s=source_->source(i);
   const auto add=[&](std::size_t first,std::size_t last){
    for(std::size_t j=first;j<last;++j){if(next.ignoreColliderCount[i]>=next.ignoreColliders[i].size())return false;
     next.ignoreColliders[i][next.ignoreColliderCount[i]++]=identities[j].main690;}
    return true;};
   if(s.ignorePlayer&&!add(0,humans34))return false;
   if(s.ignoreAi&&!add(humans34,racerCount))return false;
  }
  next.ready=true;if(!g::validNativeTriggerGroup50MC(next))return false;next.ready=false;
  racers_=identities;racerCount_=racerCount;*group_=std::move(next);return true;
 }
 // The query binding (the collision world's query bracket) publishes readiness
 // once the racer identities are bound. unbindQuery() withdraws it at teardown.
 bool bindQuery()noexcept {
  if(!thread()||queryBound_||!sourcePrepared_||!bindAttempted_||closed_)return false;
  group_->ready=true;if(!g::validNativeTriggerGroup50MC(*group_)){group_->ready=false;return false;}
  queryBound_=ready_=true;return true;
 }
 bool unbindQuery()noexcept {
  if(!thread()||!queryBound_||lease_||servicing_)return false;
  queryBound_=ready_=false;if(group_)group_->ready=false;return true;
 }
 // Stores the real prepared root geometry of one racer collider for the exit service.
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,const g::BodyMatrix48G& world)noexcept {
  const auto c=colliderIndex(collider);
  if(!ready()||servicing_||(lease_?(!group_->queryActive||lease_->queryCollider!=collider):!queryIdle())||
     terminal_!=Status::Complete||c>=group_->colliders.size()||!resource.nodes||!resource.nodeCount||
     !resource.rays||!resource.rayCount||(resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||
     !g::trigger_sphere50in_detail::affine(world))return false;
  const auto& root=resource.nodes[resource.root];Geometry next{};next.world=world;
  for(unsigned k=0;k<3;++k){
   if(!std::isfinite(root.center[k])||!std::isfinite(root.half[k])||root.half[k]<0.f)return false;
   next.center[k]=root.center[k];next.half[k]=root.half[k];
  }
  next.valid=true;geometry_[c]=next;return true;
 }
 // 2A0B30: exit service of every live trigger against its current members.
 Status serviceWorld()noexcept {
  if(!ready())return fail(Status::Unprepared);
  if(lease_||servicing_||group_->queryActive||terminal_!=Status::Complete)return terminal_==Status::Complete?fail(Status::Input):terminal_;
  if(!g::validNativeTriggerGroup50MC(*group_))return fail(Status::Input);
  servicing_=true;struct End{bool& flag;~End(){flag=false;}}end{servicing_};
  for(std::size_t t=0;t<group_->triggerCount;++t){const auto& state=group_->triggers[t];const auto& authored=source_->source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t slot=0;slot<state.collider.highWaterD0&&slot<observations.size();++slot){const auto id=state.collider.membersCC[slot];if(!id)continue;
    const auto c=colliderIndex(id);if(c>=group_->colliders.size()||!geometry_[c].valid)return fail(Status::Input);
    const auto& geometry=geometry_[c];bool overlap=false;
    if(authored.shapeType==6){
     g::Type2Node49X root{};root.center=geometry.center;root.half=geometry.half;
     g::Type2ResourceView49X view{};view.nodes=&root;view.nodeCount=1;
     if(!g::nativeType2RootTriggerBoxOverlap50MA(view,geometry.world,authored.box,overlap))return fail(Status::Service);
    }else{
     g::TriggerSphereRootExitInput50IN input{};input.type2PreparedWorld=geometry.world;
     for(unsigned k=0;k<3;++k){input.sphere.localCenter.v[k]=authored.sphere.center.v[k];input.rootCenter.v[k]=geometry.center[k];input.rootHalf.v[k]=geometry.half[k];}
     input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
     for(unsigned k=0;k<4;++k)input.sphere.world.v[k*5]=1.f;
     input.sphere.radius50=authored.sphere.radius;volatile float square=authored.sphere.radius*authored.sphere.radius;input.sphere.radiusSquared54=square;
     if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)return fail(Status::Service);
    }
    observations[slot]={&group_->colliders[c],overlap};
   }
   const auto receipt=g::serviceNativeSourceTrigger50MC(*group_,t,observations.data(),state.collider.highWaterD0,group_->actions,group_->contact);
   if(receipt.status!=g::SourceTriggerContactStatus50MC::Complete)return fail(Status::Service);
  }
  return Status::Complete;
 }
 // Coarse zone bounds of trigger t (the zone-membership registration of its candidate).
 g::LiveZoneBounds50EN bounds(std::size_t t)const {
  const auto& s=source_->source(t);g::LiveZoneBounds50EN out{};out.bounds.shapeTag=s.shapeType;
  for(unsigned n=0;n<16;n+=5)out.bounds.shapeWorld.v[n]=1.f;
  if(s.shapeType==6){out.bounds.shapeWorld=s.box.world;
   for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=s.box.center.v[n];out.bounds.localHalf.v[n]=s.box.half.v[n];}
  }else for(unsigned n=0;n<3;++n){out.bounds.localCenter.v[n]=s.sphere.center.v[n];out.bounds.localHalf.v[n]=s.sphere.radius;}
  for(unsigned n=0;n<3;++n)out.bounds.cachedCenter.v[n]=s.sphere.center.v[n];
  out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=s.sphere.radius;return out;
 }
 bool queryIdle()const noexcept{return thread()&&!lease_&&!servicing_&&!(group_&&group_->queryActive);}
 bool preparedSourceFor(const src::Owner& s)const noexcept{return thread()&&sourcePrepared_&&source_.get()==&s&&s.ready();}
 bool ready()const noexcept{
  return thread()&&ready_&&queryBound_&&!closed_&&sourcePrepared_&&group_&&group_->ready&&!group_->actionsFailed&&
   source_&&source_->ready()&&(!destinations_.animations||destinations_.animations->ready());
 }
 g::NativeTriggerGroup50MC* nativeGroup()noexcept{return group_.get();}
 const g::NativeTriggerGroup50MC* nativeGroup()const noexcept{return group_.get();}
 const src::Owner* sourceOwner()const noexcept{return source_.get();}
 const track_animation_runtime50AN::Owner* animationOwner()const noexcept{return destinations_.animations.get();}
 std::size_t triggerCount()const noexcept{return group_?group_->triggerCount:0u;}
 const g::TriggerEntryState50IN& state(std::size_t i)const{
  if(!group_||i>=group_->triggerCount)throw std::out_of_range("trigger state outside the live rows");return group_->triggers[i];
 }
 const Counters& counters()const noexcept{return counters_;}
 Status status()const noexcept{return terminal_;}
 const std::string& lastError()const noexcept{return error_;}
 bool closed()const noexcept{return thread()&&closed_;}
 // Teardown: every bank peer's trigger membership must already be removed.
 bool close()noexcept {
  if(!queryIdle()||queryBound_)return false;if(closed_)return true;
  if(group_){
   for(const auto& trigger:group_->triggers)for(auto id:trigger.collider.membersCC)if(id)return false;
   for(const auto& collider:group_->colliders)for(auto id:collider.membersCC)if(id)return false;
   const auto phase=group_->contact.phase;
   if(phase!=g::SourceTriggerContactPhase50MC::Idle&&phase!=g::SourceTriggerContactPhase50MC::Complete&&!group_->actionsFailed)return false;
   group_->ready=false;group_->actions={};group_->chance={};
  }
  ready_=sourcePrepared_=destinationsBound_=false;closed_=true;destinations_={};group_.reset();source_.reset();return true;
 }
};

// Cold append of the live trigger candidates after the collision rows already
// placed (the collision world's candidate and binding vectors, R3). Callers
// reserve the full join first so the resize never relocates. Returns the first
// trigger candidate index. Element types are the collision world's
// (Type2StaticCandidate49X, track_runtime_storage50IG::Binding).
template<class Candidates,class Bindings>
std::size_t prepareCandidates(Owner& actions,Candidates& candidates,Bindings& bindings,std::size_t ownerLimit,
    std::uint8_t tag,std::uint8_t flags31,std::uint8_t flagsF8){
 const auto* source=actions.sourceOwner();auto* group=actions.nativeGroup();
 const auto live=source?source->liveCount():0u;
 if(!source||!group||!actions.queryIdle()||group->ready||!actions.preparedSourceFor(*source)||candidates.size()!=bindings.size()||
    candidates.size()>ownerLimit||live>ownerLimit-candidates.size()||
    candidates.capacity()<candidates.size()+live||bindings.capacity()<bindings.size()+live)
  throw std::runtime_error("trigger candidates: collision prefix/source owner extent");
 const auto first=candidates.size();candidates.resize(first+live);bindings.resize(candidates.size());
 for(std::size_t i=0;i<first;++i)bindings[i].candidate=&candidates[i];
 for(std::size_t i=0;i<live;++i){const auto& row=source->source(i);auto& target=candidates[first+i];
  target.shapeType=row.shapeType;target.nativeTriggersMC=group;target.nativeTriggerIndex=i;target.nativeTriggerBox=row.box;
  auto& c=target.candidate.collision;c.shapeType=row.shapeType;
  c.geometry.id=firstTriggerIdentity+static_cast<std::uint32_t>(i);
  c.filter.identity=c.geometry.id;c.ownerIdentity9C=firstTriggerModelIdentity+static_cast<std::uint32_t>(i);c.typeTag=tag;
  c.filter.categoryA4=8u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|6u);
  c.hasResponseCallbackE8=true;c.sphere=row.sphere;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  for(unsigned n=0;n<16;n+=5)c.geometry.worldMatrix.v[n]=1.f;
  if(row.shapeType==6)c.geometry.worldMatrix=row.box.world;
  bindings[first+i]={std::numeric_limits<std::size_t>::max(),&target,false,row.zone,0,g::ZoneMethod50CL::First,true};
 }
 return first;
}
} // namespace course_trigger_actions50CT
