#pragma once
#include "mb_collision_world50mb.h"
#include "native_racer_bank50hn_fixture.h"
#include "source_trigger_reset50mb.h"
#include "source_indexed_type2_query50mb.h"
#include "trigger_sphere50in.h"
#include <limits>

namespace mb_source_trigger50MB {
namespace g=rr::game;
namespace physical=native_source_trigger_reset50MB;
// Actual callback-only borrows. None survives a Bank restore, including on
// partial failure. Durable cursors and reset prefixes live in Owner instead.
struct QueryBorrow {
 g::VehicleFrameState49W& frame;
 const g::VehicleFrameBindings49W& bindings;
 const g::VehicleFrameParameters49W& parameters;
 g::TrackNavigationProgress50EU& progress;
 physical::Owners* physicalOwners=nullptr;
 g::UndertrackPhysicsWorkspace50EW* physicalWorkspace=nullptr;
 std::uint32_t racer60=0;
};
struct RetirementBorrow {
 g::VehicleFrameState49W& frame;
 const g::VehicleFrameBindings49W& bindings;
 const g::VehicleFrameParameters49W& parameters;
 g::original_gc_gameplay_lighting_candidate::Owner& visibility;
 std::uint32_t& cables254Field50;
 g::UndertrackQueryOwners50EW queries;
 g::ZoneLifecycle50CL& collisionLifecycle;
 g::LiveZoneWorld50EN& collisionWorld;
 g::LiveZoneWorkspace50EN& collisionWorkspace;
 std::uint32_t racer60;
};
class Owner final {
 std::shared_ptr<mb_collision_world50MB::Owner> world_;
 native_racer50HN::Bank* bank_=nullptr;
 native_racer50HN::Bank::QueryReservation reservation_;
 g::CollisionResultBindingOwner50MB resultBinding_;
 g::SourceType2QueryState50MB queryState_{};
 physical::State reset_{};
 physical::Diagnostic resetDiagnostic_{};
 const QueryBorrow* borrow_=nullptr;
 g::SourceTriggerActionHooks50MB hooks_{};
 std::uint64_t generation_=1,serial_=0;
 std::size_t slot_=0;
 bool bound_=false,active_=false,suspended_=false,servicing_=false,faulted_=false;
 struct Geometry {g::BodyMatrix48G world{};g::SpringVector4_48N center{},half{};bool valid=false;};
 std::array<Geometry,16> geometry_{};
 bool targetWritten_=false;
 g::SourceTriggerActionCall50MB targetCall_{};
 static bool sameCall(const g::SourceTriggerActionCall50MB& a,const g::SourceTriggerActionCall50MB& b) noexcept {
  return a.querySerial==b.querySerial&&a.candidateOrdinal==b.candidateOrdinal&&
   a.source.triggerCollider==b.source.triggerCollider&&a.source.otherCollider==b.source.otherCollider&&
   a.source.actionIndex==b.source.actionIndex&&a.source.entering==b.source.entering;
 }
 bool preflight(const g::SourceTriggerActionCall50MB& call)const noexcept {
  if(!bound_||faulted_||call.source.actionIndex||
     call.source.triggerCollider<mb_collision_world50MB::Owner::triggerIdentity)return false;
  const auto trigger=call.source.triggerCollider-mb_collision_world50MB::Owner::triggerIdentity;
  if(trigger>=world_->source_.liveCount())return false;
  const auto collider=g::nativeTriggerCollider50GA(world_->triggers_,call.source.otherCollider);
  if(collider>=world_->triggers_.colliders.size())return false;
  if(!call.source.entering)return servicing_&&!active_&&!suspended_&&!borrow_&&call.querySerial==0;
  if(!active_||!borrow_||call.querySerial!=serial_||collider/2!=slot_||world_->source_.source(trigger).targetNodeIndex>=world_->aiNodeCount_)return false;
  if(!(collider&1u)&&world_->source_.source(trigger).reset)
   return borrow_->physicalOwners&&borrow_->physicalWorkspace&&borrow_->frame.fields.vehicle58==1u;
  return true;
 }
 g::SourceTriggerActionResult50MB apply(const g::SourceTriggerActionCall50MB& call) noexcept {
  using Status=g::SourceTriggerActionStatus50MB;
  if(!preflight(call))return {Status::InvalidBinding,0x1b282cu,0};
  // The original identity scan only matches real racer690. A secondary
  // collider therefore performs neither the C80 write nor physical reset.
  const auto collider=g::nativeTriggerCollider50GA(world_->triggers_,call.source.otherCollider);
  if(!call.source.entering||(collider&1u))return {Status::Complete,0,0};
  const auto& source=world_->source_.source(call.source.triggerCollider-mb_collision_world50MB::Owner::triggerIdentity);
  if(targetWritten_&&!sameCall(targetCall_,call)){
   if(reset_.eventBound&&reset_.stage!=physical::Stage::Complete)return {Status::InvalidBinding,0x1b282cu,0};
   targetWritten_=false;reset_={};
  }
  if(!targetWritten_){
   borrow_->progress.reset.vehicleC80=source.targetNodeIndex; // actual1B282C once.
   targetCall_=call;targetWritten_=true;
  }
  if(!source.reset)return {Status::Complete,0,0};
  physical::Invocation invocation{call,{source.targetNodeIndex,true},static_cast<std::uint32_t>(slot_),
   borrow_->racer60,world_->triggers_.colliders[slot_*2].identity,this,generation_};
  return physical::resumeAfterLostAiTarget50MB(borrow_->frame,borrow_->bindings,borrow_->parameters,
   borrow_->progress,invocation,reset_,*borrow_->physicalOwners,*borrow_->physicalWorkspace,resetDiagnostic_);
 }
public:
 Owner(){hooks_={this,[](void* p,const g::SourceTriggerActionCall50MB& c)noexcept{return static_cast<Owner*>(p)->preflight(c);},
   [](void* p,const g::SourceTriggerActionCall50MB& c)noexcept{return static_cast<Owner*>(p)->apply(c);}};}
 Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(bound_||active_||suspended_||borrow_||resultBinding_.borrowed())std::terminate();}
 bool bind(native_racer50HN::Bank& bank,std::shared_ptr<mb_collision_world50MB::Owner> world,
     const std::array<std::array<std::uint32_t,2>,8>& actualRacerIdentities,std::uint32_t playerPrefixCount34) noexcept {
  if(bound_||world_||(!bank.count()||bank.count()>8)||!world||!world->ready()||world->actionOwner_||playerPrefixCount34>bank.count())return false;
  // The root supplies identities read by genuine visits to the completed Bank.
  // Cold geometry preparation never predicts or publishes racer identities.
  for(std::size_t i=0;i<bank.count()*2;++i){const auto id=actualRacerIdentities[i/2][i%2];
   if(!id||(id>=world->triggerIdentity&&id<world->triggerIdentity+mb_trigger_authoring::capacity))return false;
   for(std::size_t j=0;j<i;++j)if(actualRacerIdentities[j/2][j%2]==id)return false;
  }
  if(!world->triggers_.colliders.setCount(bank.count()*2))return false;
  for(std::size_t i=0;i<bank.count()*2;++i)world->triggers_.colliders[i].identity=actualRacerIdentities[i/2][i%2];
  // IgnoreTrigger PLAYER: the player-prefix main colliders (absent = none).
  for(std::size_t i=0;i<world->source_.liveCount();++i){const bool ignore=world->source_.source(i).ignorePlayer;
   world->triggers_.ignoreColliderCount[i]=ignore?playerPrefixCount34:0;
   if(ignore)for(std::size_t j=0;j<playerPrefixCount34;++j)world->triggers_.ignoreColliders[i][j]=actualRacerIdentities[j][0];
  }
  if(!bank.bindSourceQueryOwner(*this,generation_))return false;
  bank_=&bank;world_=std::move(world);world_->triggers_.ready=true;world_->actionOwner_=this;bound_=true;
  if(!g::validSourceTriggerGroup50MB(world_->triggers_,hooks_)){
   world_->triggers_.ready=false;world_->actionOwner_=nullptr;bank.unbindSourceQueryOwner(*this,generation_);bank_=nullptr;world_.reset();bound_=false;return false;
  }
  return true;
 }
 g::CollisionResultBindingOwner50MB& resultBinding() noexcept{return resultBinding_;}
 g::NativeTriggerGroup50GA& group() noexcept{return world_->triggers_;}
 const g::SourceTriggerActionHooks50MB& hooks()const noexcept{return hooks_;}
 bool pending()const noexcept{return suspended_||active_;}
 bool observePrepared(std::uint32_t collider,const g::Type2ResourceView49X& resource,
     const g::BodyMatrix48G& world) noexcept {
  if(!bound_||faulted_||active_||suspended_||servicing_||!resource.nodes||!resource.nodeCount||
      !resource.rays||!resource.rayCount||(resource.root&g::type2Leaf49X)||resource.root>=resource.nodeCount||
      !g::trigger_sphere50in_detail::affine(world))return false;
  const auto c=g::nativeTriggerCollider50GA(world_->triggers_,collider);
  if(c>=world_->triggers_.colliders.size()||bank_->visitDepth_!=1||bank_->currentSlot_!=c/2)return false;
  Geometry next{};next.world=world;const auto& root=resource.nodes[resource.root];
  for(unsigned k=0;k<3;++k){next.center.v[k]=root.center[k];next.half.v[k]=root.half[k];}
  next.center.v[3]=next.half.v[3]=1.f;next.valid=true;geometry_[c]=next;return true;
 }

 // Must be called inside the actual Bank.visit(slot), including slot0. The
 // Bank independently authenticates its current slot when issuing the token.
 g::SourceType2QueryReceipt50MB query(std::size_t slot,bool secondary,const QueryBorrow& borrow,
     const g::MainStaticDetectorAdmission50H& admission,const g::VehicleLiveQueryContext50EQ& live,
     g::SourceType2QueryOwners50MB& owners,const std::size_t* indices,std::size_t count,
     const g::Type2StaticWorkspace49X& workspace,const g::Type2StaticDestination49X& destination) noexcept {
  g::SourceType2QueryReceipt50MB receipt{};
  if(!bound_||faulted_||servicing_||active_||suspended_||borrow_||slot>=bank_->count()||
     borrow.frame.phase!=g::VehicleFramePhase49W::PostWorld||serial_==std::numeric_limits<std::uint64_t>::max()||
     &owners.triggers!=&world_->triggers_||&owners.actions!=&hooks_||&owners.resultBinding!=&resultBinding_||
     !borrow.racer60)return receipt;
  if(borrow.physicalOwners&&(&borrow.physicalOwners->resultBinding!=&resultBinding_||
     &borrow.physicalOwners->outerResult!=&owners.outerResult||
     !borrow.physicalOwners->aiTrack.sameWorld(world_->world())))return receipt;
  const auto collider=world_->triggers_.colliders[slot*2+(secondary?1:0)].identity;
  if(!owners.query.filter||owners.query.filter->identity!=collider||bank_->currentSlot_!=slot||bank_->visitDepth_!=1)return receipt;
  const auto next=serial_+1;
  if(!bank_->beginSourceQuery(*this,generation_,next,reservation_))return receipt;
  serial_=next;slot_=slot;active_=true;borrow_=&borrow;queryState_={};reset_={};targetWritten_=false;
  owners.identity={this,generation_,serial_,slot,collider};
  bool outer=resultBinding_.beginOuter(owners.identity,owners.query,owners.result,owners.pairs,owners.metadata,owners.pairSlots,owners.outerResult);
  if(outer){
   receipt=secondary?g::querySecondaryLiveSourceIndexedVehicle50MB(borrow.frame,borrow.bindings,admission,live,
       owners,world_->candidates(),world_->candidateCount(),indices,count,workspace,destination,queryState_):
     g::queryMainLiveSourceIndexedVehicle50MB(borrow.frame,borrow.bindings,admission,live,
       owners,world_->candidates(),world_->candidateCount(),indices,count,workspace,destination,queryState_);
   if(!resultBinding_.endOuter(owners.outerResult))std::terminate();
  }
  borrow_=nullptr;active_=false;
  const auto* resource=owners.query.resource;
  if(outer&&receipt.status==g::SourceType2QueryStatus50MB::Complete&&
      (!resource||!observePrepared(collider,*resource,owners.query.worldMatrix)))
   receipt.status=g::SourceType2QueryStatus50MB::Input;
  if(outer&&receipt.status==g::SourceType2QueryStatus50MB::Complete){
   if(!bank_->completeSourceQuery(*this,reservation_))std::terminate();
  }else{
   if(!bank_->suspendSourceQuery(*this,reservation_))std::terminate();
   suspended_=true;faulted_=true;
  }
  return receipt;
 }
 // Retain only actual scalar bounds/matrix observations across bank restores,
 // never a resource/node pointer into another racer's mutable arena.
 bool serviceWorld() noexcept {
  if(!bound_||faulted_||active_||suspended_||servicing_||borrow_||resultBinding_.borrowed()||bank_->sourceQueryPending())return false;
  servicing_=true;
  struct End {Owner& owner;~End(){owner.servicing_=false;}} end{*this};
  const auto fail=[&](){faulted_=true;return false;};
  for(std::size_t t=0;t<world_->source_.liveCount();++t){const auto& state=world_->triggers_.triggers[t];const auto& source=world_->source_.source(t);
   std::array<g::TriggerOverlap50IN,g::triggerContactCapacity50IN> observations{};
   for(std::size_t i=0;i<state.collider.highWaterD0;++i){const auto id=state.collider.membersCC[i];if(!id)continue;
    const auto c=g::nativeTriggerCollider50GA(world_->triggers_,id);if(c>=world_->triggers_.colliders.size()||!geometry_[c].valid)return fail();
    const auto& geometry=geometry_[c];g::TriggerSphereRootExitInput50IN input{};
    input.type2PreparedWorld=geometry.world;
    for(unsigned k=0;k<3;++k){input.sphere.localCenter.v[k]=source.sphere.center.v[k];input.rootCenter.v[k]=geometry.center.v[k];input.rootHalf.v[k]=geometry.half.v[k];}
    input.sphere.localCenter.v[3]=input.rootCenter.v[3]=input.rootHalf.v[3]=1.f;
    for(unsigned k=0;k<4;++k)input.sphere.world.v[k*5]=1.f;
    input.sphere.radius50=source.sphere.radius;volatile float square=source.sphere.radius*source.sphere.radius;input.sphere.radiusSquared54=square;
    bool overlap=false;if(g::overlapTriggerSphereType2Root50IN(input,overlap)!=g::TriggerSphereRootExitStatus50IN::Complete)return fail();
    observations[i]={&world_->triggers_.colliders[c],overlap};
   }
   const auto receipt=g::serviceNativeSourceTrigger50MB(world_->triggers_,t,observations.data(),state.collider.highWaterD0,hooks_,queryState_.contact);
   if(receipt.status!=g::SourceTriggerContactStatus50MB::Complete)return fail();
  }
  return true;
 }
 // The factory constructs real owner references AFTER the Bank restores the
 // failed slot. It cannot report cleanup success; this method performs and
 // checks every actual removal itself before minting the private receipt.
 template<class MakeBorrow> bool retireSuspended(MakeBorrow&& makeBorrow) {
  if(!bound_||!suspended_||active_||borrow_||resultBinding_.borrowed())return false;
  return bank_->visitSuspendedForRetirement(*this,reservation_,[&](){
   auto borrow=makeBorrow(slot_);auto& o=borrow;
   if(!borrow.racer60||&o.collisionWorld!=&world_->world()||borrow.frame.phase!=g::VehicleFramePhase49W::PostWorld)return false;
   const auto ordinal=reset_.eventBound?reset_.invocation.call.candidateOrdinal:queryState_.ordinal;
   const auto action=reset_.eventBound?reset_.invocation.call.source.actionIndex:0u;
   const g::original_gc_gameplay_lighting_candidate::SourceTriggerVisibilityIdentity50MB key{
       this,generation_,serial_,ordinal,action,borrow.racer60};
   if(!o.visibility.retireSourceTriggerVisibility50MB(static_cast<std::uint32_t>(slot_),
       borrow.frame,borrow.bindings,borrow.parameters,&o.cables254Field50,key,resetDiagnostic_.visibility))return false;
   //7F0 precedes the actual690/694/7A4/348 ordered registry removals. Each
   // completed removal is retained on failure; no snapshot restore/replay.
   for(std::size_t i=0;i<4;++i){const auto& q=o.queries.query[i];
    if(!q.membership||!q.object||
       (q.membership->attached&&q.membership->object!=q.object))return false;
    const auto removed=g::removeLiveZoneMembership50EN(o.collisionLifecycle,o.collisionWorld,*q.membership,o.collisionWorkspace);
    if(removed.status!=g::LiveZoneStatus50EN::Complete)return false;
    if(i==1)borrow.frame.shape.registrationE0=0;
    if(q.membership->attached||q.membership->object)return false;
   }
   if(resultBinding_.borrowed()||borrow_)return false;
   RetirementReceipt receipt(*bank_,*this,generation_,serial_,slot_);
   if(!bank_->retireSuspendedSourceQuery(*this,reservation_,receipt))return false;
   suspended_=false;return true;
  });
 }
 // Native cold terminal cleanup, authenticated by this bound issuer and the
 // actual Bank-selected slot. This issues no query/reset completion receipt.
 bool retireColdVisibility(std::size_t slot,g::VehicleFrameState49W& frame,
     const g::VehicleFrameBindings49W& bindings,const g::VehicleFrameParameters49W& parameters,
     g::original_gc_gameplay_lighting_candidate::Owner& visibility,std::uint32_t& cablesField50,
     g::original_gc_gameplay_lighting_candidate::Diagnostic& diagnostic) noexcept {
  if(!bound_||!bank_||!world_||world_->actionOwner_!=this||active_||suspended_||servicing_||borrow_||
      resultBinding_.borrowed()||slot>=bank_->count()||bank_->sourceIssuer_!=this||
      bank_->sourceGeneration_!=generation_||bank_->pendingToken_||bank_->visitDepth_!=1||
      bank_->currentSlot_!=slot)return false;
  return visibility.retireMbColdVehicle(static_cast<std::uint32_t>(slot),frame,bindings,parameters,
      &cablesField50,diagnostic);
 }
 // Caller retires actual renderer/camera users and each racer/query first.
 // This unbind never clears pending work or invents a cleanup receipt.
 bool unbind() noexcept {
  if(!bound_||active_||suspended_||borrow_||resultBinding_.borrowed()||
      !bank_->unbindSourceQueryOwner(*this,generation_))return false;
  world_->triggers_.ready=false;world_->actionOwner_=nullptr;
  bank_=nullptr;world_.reset();bound_=false;return true;
 }
};
} // namespace mb_source_trigger50MB
