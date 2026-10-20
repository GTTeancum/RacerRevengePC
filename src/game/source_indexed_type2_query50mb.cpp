#include "source_indexed_type2_query50mb.h"
#include "native_gate_geometry50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::game { namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
bool finite(const Vector3_48C& v) noexcept {for(float f:v.v)if(!std::isfinite(f))return false;return true;}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
bool finite(const CollisionSphere48J& s) noexcept {return finite(s.center)&&std::isfinite(s.radius)&&s.radius>=0.f;}
bool member(const std::vector<std::uint32_t>& values,std::uint32_t id) noexcept {
    return std::find(values.begin(),values.end(),id)!=values.end();
}
bool identitiesPass(const CollisionIdentityList48J& list,std::uint32_t other) noexcept {
    // Empty identity lists bypass the mode test in the original.
    if(list.identities.empty())return true;
    const bool found=member(list.identities,other);
    if(found&&list.mode==1)return false;
    if(!found&&list.mode==0)return false;
    return true;
}
bool categoriesPass(const CollisionFilter48J& f,std::uint32_t other) noexcept {
    bool found=false;for(auto mask:f.categoryMasks)if(mask&other){found=true;break;}
    // Unlike identity lists, an empty category allow-list rejects.
    return f.categoryModeD4==1?!found:found;
}
bool filter(const CollisionFilter48J& query,const CollisionFilter48J& candidate) noexcept {
    if(query.identity==candidate.identity||!(candidate.flagsF8&4u)||!(candidate.flags31&1u))return false;
    if(!(candidate.categoryA4&query.acceptMaskB8))return false;
    for(unsigned i=0;i<2;++i)
        if(!identitiesPass(query.identityLists[i],candidate.identity)||
           !identitiesPass(candidate.identityLists[i],query.identity))return false;
    return categoriesPass(query,candidate.categoryA4)&&categoriesPass(candidate,query.categoryA4);
}
bool spheres(const CollisionSphere48J& a,const CollisionSphere48J& b,bool& overlap) noexcept {
    const float x=sub(a.center.v[0],b.center.v[0]);
    const float y=sub(a.center.v[1],b.center.v[1]);
    const float z=sub(a.center.v[2],b.center.v[2]);
    const float squared=add(add(mul(x,x),mul(y,y)),mul(z,z));
    const float radius=add(a.radius,b.radius);
    volatile float distance=std::sqrt(squared);
    const float gap=sub(distance,radius);
    if(!std::isfinite(gap))return false;
    overlap=gap<=0.f;return true;
}

struct Span {std::uintptr_t a=0,b=0;};
struct Guard {
 Span writes[64]{};std::size_t count=0;
 template<class T> bool range(const T* p,std::size_t n,Span& s) const noexcept {
  s={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
  const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+bytes};return true;
 }
 static bool overlap(Span a,Span b) noexcept {return a.a<b.b&&b.a<a.b;}
 template<class T> bool write(T* p,std::size_t n) noexcept {Span x;if(count==64||!range(p,n,x))return false;for(std::size_t i=0;i<count;++i)if(overlap(x,writes[i]))return false;writes[count++]=x;return true;}
 template<class T> bool read(const T* p,std::size_t n) const noexcept {Span x;if(!range(p,n,x))return false;for(std::size_t i=0;i<count;++i)if(overlap(x,writes[i]))return false;return true;}
 bool lists(const CollisionFilter48J& f) const noexcept {return read(f.categoryMasks.data(),f.categoryMasks.size())&&read(f.identityLists[0].identities.data(),f.identityLists[0].identities.size())&&read(f.identityLists[1].identities.data(),f.identityLists[1].identities.size());}
};
bool storage(const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
 const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,
 SourceType2QueryOwners50MB& o,Type2StaticCandidate49X* owners,std::size_t ownerCount,
 const std::size_t* indices,std::size_t count,const Type2StaticWorkspace49X& w,
 const Type2StaticDestination49X& d,SourceType2QueryState50MB& state) noexcept {
 Guard g;auto& life=live.lifecycle;auto& membership=live.registrationE0;auto& lw=live.workspace;auto& world=live.world;
 if(!g.write(&o.query,1)||!g.write(&o.filter,1)||!g.write(&o.initial,1)||!g.write(&o.spring,1)||
    !g.write(o.records,o.recordSlots)||!g.write(&o.result,1)||!g.write(o.pairs,o.pairSlots)||!g.write(o.metadata,o.pairSlots)||
    !g.write(o.movementMirrors,o.mirrorCount)||!g.write(&o.normalMode,1)||!g.write(&o.resultBinding,1)||
    !g.write(&o.outerResult,1)||!g.write(&o.triggers,1)||!g.write(&state,1)||!g.write(owners,ownerCount)||
    !g.write(w.pairs,w.pairCapacity)||!g.write(w.metadata,w.metadataCapacity)||!g.write(w.records,w.recordCapacity)||
    !g.write(w.effects,w.effectCapacity)||!g.write(w.stack,w.stackCapacity)||!g.write(w.trace,w.traceCapacity)||
    !g.write(d.pairs,d.pairCapacity)||!g.write(d.metadata,d.metadataCapacity)||!g.write(d.records,d.recordCapacity)||!g.write(d.effects,d.effectCapacity)||
    !g.write(&life,1)||!g.write(&membership,1)||!g.write(&lw,1)||
    !g.write(life.storage.registries,life.storage.registryCapacity)||!g.write(life.storage.registrations,life.storage.registrationCapacity)||
    !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||!g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
    !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||!g.write(membership.storage.links,membership.storage.capacity)||
    !g.write(membership.storage.entries,membership.storage.capacity)||!g.write(lw.registries,lw.registryCapacity)||
    !g.write(lw.links,lw.membershipCapacity)||!g.write(lw.entries,lw.membershipCapacity)||
    !g.write(world.portalLinks,world.portalLinkCount)||!g.write(lw.portalLinks,lw.portalLinkCapacity)||!g.write(const_cast<VehicleFrameState49W*>(&frame),1))return false;
 if(!g.read(&o,1)||!g.read(&bindings,1)||!g.read(&admission,1)||!g.read(&live,1)||!g.read(&w,1)||!g.read(&d,1)||
    !g.read(&o.actions,1)||!g.read(indices,count)||!g.read(&world,1)||!g.read(world.portalHeads,world.zoneCount)||
    !g.read(world.portals,world.portalCount)||!g.read(o.query.resource,1)||!g.lists(o.filter))return false;
 const auto& q=o.query;const auto& r=*q.resource;
 if(!g.read(r.nodes,r.nodeCount)||!g.read(r.rays,r.rayCount)||!r.rayCount||r.rayCount>32768||r.nodeCount>32768||
    r.maxDepth>r.nodeCount+r.rayCount||std::fegetround()!=FE_TONEAREST||q.filter!=&o.filter||
    q.normalMode54!=0u||!q.normalMode54Bound||!q.nativeGatePolicy||!q.callbackEnabled||q.skipMask!=8u||
    o.identity.queryCollider!=o.filter.identity||!o.identity.issuer||!o.identity.generation||!o.identity.serial||
    !validSourceTriggerGroup50MB(o.triggers,o.actions)||o.initial.pairs!=o.pairs||o.initial.metadata!=o.metadata||
    o.initial.pairSlots!=o.pairSlots||o.initial.recordSlots!=o.records||o.initial.recordCount!=o.recordSlots||
    o.pairSlots<r.rayCount||o.mirrorCount!=ownerCount||o.spring.generatedCapacity88<0||
    static_cast<std::size_t>(o.spring.generatedCapacity88)>o.recordSlots||o.spring.generatedCount84<0||
    o.spring.generatedCount84>o.spring.generatedCapacity88||
    w.pairCapacity<o.pairSlots||d.pairCapacity<o.pairSlots||w.metadataCapacity<o.pairSlots||d.metadataCapacity<o.pairSlots||
    w.recordCapacity<o.recordSlots||d.recordCapacity<o.recordSlots||(count&&(!w.effectCapacity||!d.effectCapacity)))return false;
 auto mesh=[&](const assets::CollisionMesh48I* m,const float* radii,std::size_t n) noexcept {
  return g.read(m,1)&&g.read(radii,n)&&g.read(m->nodes.data(),m->nodes.size())&&g.read(m->faces.data(),m->faces.size())&&
   g.read(m->vertices.data(),m->vertices.size())&&g.read(m->vertex_normals.data(),m->vertex_normals.size());
 };
 for(std::size_t i=0;i<ownerCount;++i){const auto& c=owners[i];const auto& t=c.candidate.collision;
  if(!g.lists(t.filter)||!g.lists(o.movementMirrors[i].collision.filter)||
     !g.read(c.candidate.sceneName,c.candidate.sceneNameBytes)||t.filter.identity!=o.movementMirrors[i].collision.filter.identity)return false;
 }
 for(std::size_t i=0;i<count;++i){if(indices[i]>=ownerCount)return false;const auto& c=owners[indices[i]];const auto& t=c.candidate.collision;
  if(c.shapeType==0){if(c.nativeTriggersTB||c.nativeTriggersTC||c.nativeTriggers||c.gate||c.nativeTriggerIndex||t.hasResponseCallbackE8||!mesh(t.geometry.mesh,c.nodeRadii,c.nodeRadiusCount))return false;}
  else if(c.shapeType==1){if(c.nativeTriggersTB||c.nativeTriggersTC||c.nativeTriggers||c.gate||c.nativeTriggerIndex||t.hasResponseCallbackE8||!g.read(c.compound,1)||c.compound->count>maxCompoundMembers50IJ||!g.read(c.compound->parts,c.compound->count))return false;
   for(std::size_t j=0;j<c.compound->count;++j){const auto& a=c.compound->parts[j];if(!mesh(a.mesh,a.nodeRadii,a.nodeRadiusCount))return false;}}
  else if(c.shapeType==5||c.shapeType==6){if(c.nativeTriggersTB||c.nativeTriggersTC||c.gate||c.raOriginalTriggerSegments||c.nativeTriggers!=&o.triggers||
    c.nativeTriggerIndex>=o.triggers.triggers.size()||t.filter.identity!=o.triggers.triggers[c.nativeTriggerIndex].collider.identity||
    t.shapeType!=c.shapeType||t.compound||t.geometry.mesh||t.filter.categoryA4!=8u||!t.hasResponseCallbackE8||
    nativeTriggerCollider50GA(o.triggers,o.filter.identity)==o.triggers.colliders.size()||
    (c.shapeType==6&&!validNativeTriggerBox50MA(c.nativeTriggerBox)))return false;}
  else return false;
 }
 return true;
}
void publishEffect(SourceType2QueryOwners50MB& o,Type2StaticCandidate49X& candidate,std::size_t index,
 const TerrainCandidateEffects48J& e) noexcept {
 auto& c=candidate.candidate.collision;auto& m=o.movementMirrors[index].collision;
 c.filter.flagsF8=e.flagsF8;c.copied40=e.copied40;c.copied50=e.copied50;
 m.filter.flagsF8=e.flagsF8;m.copied40=e.copied40;m.copied50=e.copied50;
}

SourceType2QueryReceipt50MB run(bool secondary,const VehicleFrameState49W& frame,
 const VehicleFrameBindings49W& bindings,const MainStaticDetectorAdmission50H& admission,
 const VehicleLiveQueryContext50EQ& live,SourceType2QueryOwners50MB& o,
 Type2StaticCandidate49X* owners,std::size_t ownerCount,const std::size_t* indices,std::size_t count,
 const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,SourceType2QueryState50MB& state) noexcept {
 using P=SourceType2QueryPhase50MB;using S=SourceType2QueryStatus50MB;SourceType2QueryReceipt50MB r;
 if(!storage(frame,bindings,admission,live,o,owners,ownerCount,indices,count,w,d,state))return r;
 if(state.phase!=P::Idle&&(!sameSourceQueryIdentity50MB(state.identity,o.identity)||state.retainedIndices!=indices||
    state.retainedCount!=count||state.retainedOwners!=owners||state.ownerCount!=ownerCount||state.secondary!=secondary||state.ordinal>count))return r;
 if(state.phase==P::Failed){r.status=S::Binding;return r;}
 if(state.phase==P::Complete){r.status=S::Complete;r.ordinal=state.ordinal;return r;}
 if(state.phase==P::Idle){
  r.live=prepareSourceLiveQuery50MB(secondary,frame,bindings,admission,live,o.query,owners,ownerCount,indices,count,o.initial,w,d,o.result,o.actions);
  if(r.live.status!=VehicleLiveQueryStatus50EQ::Complete){r.status=S::LivePrefix;return r;}
  o.result={};o.result.selected=o.initial.selected;o.result.selected.fraction=1.f;
  o.result.selected.first={{0.f,0.f,0.f,1.f}};o.result.selected.second={{0.f,0.f,0.f,1.f}};
  if(o.query.resetRecordsBefore)resetSpringRecords48N(o.spring);
  o.result.records=o.spring;o.initial.records=o.spring;
  o.result.currentCandidateIdentity=o.query.initialQueryCurrentIdentity;
  o.filter.flagsF8=static_cast<std::uint8_t>((o.filter.flagsF8|0x10u)&~0x20u);o.result.queryFlagsF8=o.filter.flagsF8;
  o.result.normalModeWrite.bound=true;o.normalMode.acceptCompletedWorldReset();
  state.identity=o.identity;state.retainedIndices=indices;state.retainedCount=count;state.retainedOwners=owners;
  state.ownerCount=ownerCount;state.secondary=secondary;state.currentCategory=o.query.initialCurrentCategory;state.phase=P::Candidate;
 }
 if(!o.resultBinding.outerCurrent(o.outerResult,o.identity,o.query,o.result,o.pairs,o.metadata,o.pairSlots)){
  if(o.resultBinding.borrowed()||!o.resultBinding.beginOuter(o.identity,o.query,o.result,o.pairs,o.metadata,o.pairSlots,o.outerResult)){r.status=S::Binding;return r;}
 }
 if(!o.resultBinding.bindOuterScratch(o.outerResult,w,d,indices,count,o.records,o.recordSlots)){r.status=S::Binding;state.phase=P::Failed;return r;}
 struct FailureBoundary {SourceType2QueryState50MB& state;SourceType2QueryReceipt50MB& receipt;
  ~FailureBoundary(){if(receipt.status!=SourceType2QueryStatus50MB::Complete&&
      !(receipt.status==SourceType2QueryStatus50MB::ActionBlocked&&state.phase==SourceType2QueryPhase50MB::Action))state.phase=SourceType2QueryPhase50MB::Failed;}
 } failure{state,r};
 // Parent explicitly closes this borrow on error BEFORE Bank arena restore.
 for(;state.ordinal<count;){r.ordinal=state.ordinal;const auto index=indices[state.ordinal];auto& candidate=owners[index];auto& c=candidate.candidate.collision;
  if(!o.resultBinding.outerCurrent(o.outerResult,o.identity,o.query,o.result,o.pairs,o.metadata,o.pairSlots)||
     o.query.filter!=&o.filter||!o.query.resource||!affine(o.query.worldMatrix)||!finite(o.query.sphere)) {r.status=S::Binding;return r;}
  if(state.phase==P::Action){
   r.contact=resumeNativeSourceTriggerContact50MB(o.triggers,candidate.nativeTriggerIndex,o.filter.identity,o.actions,state.contact,o.identity.serial,state.ordinal);
   if(r.contact.status!=SourceTriggerContactStatus50MB::Complete){r.status=r.contact.status==SourceTriggerContactStatus50MB::ActionBlocked?S::ActionBlocked:S::Binding;return r;}
   if(!releaseCompletedSourceContact50MB(state.contact)){r.status=S::Binding;return r;}
   state.phase=P::Candidate;++state.ordinal;continue;
  }
  ++o.result.candidateCount;
  if(c.typeTag!=o.query.collisionTypeTag){if(c.typeTag==o.query.groupTypeTag&&!(o.filter.flagsF8&1u)){r.status=S::Narrowphase;return r;}++state.ordinal;continue;}
  o.query.initialQueryCurrentIdentity=c.filter.identity;o.result.currentCandidateIdentity=c.filter.identity;
  TerrainCandidateEffects48J effect{};effect.identity=c.filter.identity;effect.flagsF8=static_cast<std::uint8_t>(c.filter.flagsF8|0x10u);effect.copied40=c.copied40;effect.copied50=c.copied50;
  publishEffect(o,candidate,index,effect);
  bool overlap=false;if(!spheres(o.query.sphere,c.sphere,overlap)){r.status=S::Narrowphase;return r;}
  const bool trigger=candidate.shapeType==5||candidate.shapeType==6;
  if((!overlap&&!(trigger&&o.query.nativeGateHasPrevious))||!filter(o.filter,c.filter)||
     (trigger&&!nativeTriggerPairAllowed50GA(o.triggers,candidate.nativeTriggerIndex,o.filter.identity))){++state.ordinal;continue;}
  if(c.geometry.id!=c.filter.identity||!o.normalMode.acceptReachedSourceDispatch50MB(o.query.normalMode54,o.query.normalMode54Bound)){r.status=S::Binding;return r;}
  recordCollisionNormalMode50JA(o.result.normalModeWrite,o.query.normalMode54);
  // Each dispatch rereads the actual cache/resource after the preceding E8.
  if(!trigger){
   Type2StaticResult49X next{};Type2StaticResult49X previous=o.result;previous.queryFlagsF8=o.filter.flagsF8;
   // The single-candidate helper owns its diagnostic candidate increment.
   --previous.candidateCount;previous.records=o.spring;
   o.initial.records=o.spring;Type2WorldContinuation50CC continuation{&previous,state.currentCategory,nullptr,&effect};
   std::uint32_t category=state.currentCategory;
   r.numerical=continueType2WorldCandidate50CC(o.query,candidate,o.initial,w,d,continuation,next,category);
   if(r.numerical!=Type2StaticStatus49X::Complete){r.status=S::Narrowphase;return r;}
   std::memcpy(o.pairs,d.pairs,o.pairSlots*sizeof(*o.pairs));std::memcpy(o.metadata,d.metadata,o.pairSlots*sizeof(*o.metadata));
   if(o.recordSlots)std::memcpy(o.records,d.records,o.recordSlots*sizeof(*o.records));
   o.result=next;o.spring=next.records;o.initial.records=o.spring;o.initial.selected=next.selected;
   o.filter.flagsF8=next.queryFlagsF8;state.currentCategory=category;publishEffect(o,candidate,index,d.effects[0]);++state.ordinal;continue;
  }
  bool hit=false;const auto* prior=o.query.nativeGateHasPrevious?&o.query.nativeGatePreviousWorld:nullptr;
  const bool valid=candidate.shapeType==6?nativeType2TriggerBoxOverlap50MA(*o.query.resource,o.query.worldMatrix,prior,candidate.nativeTriggerBox,hit):
      nativeType2GateOverlap50KG(*o.query.resource,o.query.worldMatrix,prior,c.sphere,hit);
  if(!valid){r.status=S::Narrowphase;return r;}if(!hit){++state.ordinal;continue;}
  ++o.result.geometryHits;o.result.hasGeometryHit=true;o.filter.flagsF8=static_cast<std::uint8_t>(o.filter.flagsF8|0x20u);o.result.queryFlagsF8=o.filter.flagsF8;
  effect.copied40=o.query.copied40;effect.copied50=o.query.copied50;publishEffect(o,candidate,index,effect);
  // Actual294BB0 E4 reads canonical main690 result/pairs. Authored TRIGGER8
  // skips before writes. This executes that skip, rather than inventing E4.
  ++o.result.callbacks;const SpringStaticCallbackInput48N cb{o.pairs,o.result.pairCount,c.filter.identity,c.filter.categoryA4,
      state.currentCategory,o.query.skipMask,o.query.dynamicMask,o.query.materialMask,2,o.query.queryScene,
      candidate.candidate.hasMaterialScene,candidate.candidate.sceneName,candidate.candidate.sceneNameBytes};
  SpringStaticCallbackResult48N cbResult{};
  if(applySpringStaticCallback48N(cb,o.records,o.recordSlots,o.spring,cbResult)!=SpringCallbackStatus48N::Skipped){r.status=S::Narrowphase;return r;}
  state.phase=P::Action;
 }
 o.result.queryFlagsF8=o.filter.flagsF8;o.result.records=o.spring;o.initial.records=o.spring;o.initial.selected=o.result.selected;
 o.query.initialCurrentCategory=state.currentCategory;state.phase=P::Complete;r.status=S::Complete;r.ordinal=state.ordinal;return r;
}
} // namespace
SourceType2QueryReceipt50MB queryMainLiveSourceIndexedVehicle50MB(const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,SourceType2QueryOwners50MB& owners,Type2StaticCandidate49X* candidates,std::size_t ownerCount,const std::size_t* indices,std::size_t count,const Type2StaticWorkspace49X& work,const Type2StaticDestination49X& destination,SourceType2QueryState50MB& state) noexcept {return run(false,frame,bindings,admission,live,owners,candidates,ownerCount,indices,count,work,destination,state);}
SourceType2QueryReceipt50MB querySecondaryLiveSourceIndexedVehicle50MB(const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,const MainStaticDetectorAdmission50H& admission,const VehicleLiveQueryContext50EQ& live,SourceType2QueryOwners50MB& owners,Type2StaticCandidate49X* candidates,std::size_t ownerCount,const std::size_t* indices,std::size_t count,const Type2StaticWorkspace49X& work,const Type2StaticDestination49X& destination,SourceType2QueryState50MB& state) noexcept {return run(true,frame,bindings,admission,live,owners,candidates,ownerCount,indices,count,work,destination,state);}
} // namespace rr::game
