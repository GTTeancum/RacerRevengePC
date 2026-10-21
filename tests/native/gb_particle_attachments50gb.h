#pragma once
#include "gb_particle_authoring50gb.h"
#include "gb_original_world50lm.h"
#include "gb_particle_attachment_pins50gb.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/scene_graph49p.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace gb_particle_runtime50GB {class Owner;}
namespace gb_type11_numeric50GB {class Instance;}
namespace gb_particle_attachments50GB {
namespace a=rr::assets;namespace g=rr::game;namespace source=gb_particle_authoring50GB;
namespace world=gb_original_world50LM;namespace tm=track_materials50fa_fixture;
inline constexpr std::size_t sourceRootCount=5,placementCount=55,associationCount=67;
inline constexpr std::size_t sourceSurfaceReferences=106;
// All seven actual attachment materials have one authored pass, hence 106
// selected spans. The whole GB table includes second passes elsewhere. Scan
// and check every matching span below; never truncate a placement at pass one.
// This exact source reference capacity is not a new generic model/render limit.
inline constexpr std::size_t maximumSpanReferences=sourceSurfaceReferences;
enum class Status {Empty,Input,Capacity,Source,Association,Graph,Spans,SourceGraphReady};
struct Diagnostic {
 Status status=Status::Empty;std::size_t ordinal=0,retainedPayloadBytes=0;
 std::size_t namedPreparationTemporaryBytes=0,readOnlyPinPayloadBytes=0,reservedAdditionalPayloadBytes=0,spanReferences=0;
 a::GeometryDiagnostic49O geometry{};std::string message;
};
// Graph-only projection of the exact 55 static placements. The immutable source
// root and actual static-world spans are retained, with no copied geometry.
// 67 source associations share those 55 graphs. There is no observer, completed
// draw, render-attempt, activation, generic readiness or visibility authority.
class Owner final {
 friend class ::gb_particle_runtime50GB::Owner;
 friend class ::gb_type11_numeric50GB::Instance;
 struct SourceRoot {a::IndexedResArchive48C::Member member{};a::PsgNode49O original{};};
 struct Placement {
  g::SceneNode49P node{};g::SceneGraph49P graph{};
  const a::WorldInstance48F* source=nullptr;
  std::size_t worldIndex=0,sourceRoot=0,firstSpan=0,spanCount=0;
 };
 struct PrepareScratch {a::PsgHierarchyView49O hierarchy{};g::BodyMatrix48G placement{};};
 // Retained before every pointer/view below; these owners must outlive them.
 std::shared_ptr<const source::Owner> source_;
 std::shared_ptr<const world::Owner> world_;
 std::array<SourceRoot,sourceRootCount> roots_{};
 std::array<Placement,placementCount> placements_{};
 std::array<std::size_t,associationCount> associationPlacement_{};
 std::array<std::uint32_t,maximumSpanReferences> spanIndices_{};
 std::size_t spanCount_=0;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,sourceGraphReady_=false;
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static bool sameGeometry(const tm::Span& x,const tm::Span& y)noexcept {
  return x.vertexStart==y.vertexStart&&x.vertexCount==y.vertexCount&&
   x.indexStart==y.indexStart&&x.indexCount==y.indexCount&&x.worldInstanceIndex==y.worldInstanceIndex&&
   x.surfaceIndex==y.surfaceIndex&&x.hasOriginalColors==y.hasOriginalColors;
 }
 const Placement* association(std::size_t i)const noexcept {
  return sourceGraphReady()&&i<associationCount?&placements_[associationPlacement_[i]]:nullptr;
 }
 // Only the two concrete particle owners can borrow the stable graph. The
 // source remains strongly retained by their shared_ptr<Owner> for that borrow.
 const g::SceneGraph49P* graph(std::size_t i)const noexcept {
  const auto* p=association(i);return p?&p->graph:nullptr;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(!thread())std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner);}
 // Additional immutable compile-time tables are charged once explicitly.
 // Includes the complete literal member names, not just their pointer slots.
 static constexpr std::size_t readOnlyPinPayloadBytes()noexcept {
  return sizeof(pins::members)+sizeof(pins::placements)+sizeof(pins::associations)+
   sizeof("geometry\\gam_temple.psg")+sizeof("geometry\\stone_head.psg")+
   sizeof("geometry\\pig_head.psg")+sizeof("geometry\\shade.psg")+sizeof("geometry\\startgate.psg");
 }
 // Named data payload only, not a measured ABI stack high-water mark. Include
 // the existing helpers' full arrays even though every admitted graph has one
 // root. The sum deliberately over-reserves mutually exclusive helper locals.
 static constexpr std::size_t namedPreparationTemporaryBytes()noexcept {
  return sizeof(PrepareScratch)+sizeof(a::PsgNode49O)+
   g::maxSceneNodes49P*(sizeof(bool)+sizeof(g::SceneNodeIndex49P)+sizeof(g::BodyMatrix48G));
 }
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  return retainedPayloadBytes()+readOnlyPinPayloadBytes()+namedPreparationTemporaryBytes();
 }
 bool prepare(std::shared_ptr<const source::Owner> authored,
  std::shared_ptr<const world::Owner> staticWorld,std::size_t availableAdditionalPayloadBytes,Diagnostic& d)noexcept {
  d={};
  if(!thread()||attempted_){d.status=Status::Input;try{d.message="GB graph requires one owner-thread attempt";}catch(...){}return false;}
  attempted_=true;
  try {
   d.status=Status::Input;
   need(authored&&authored->ready()&&staticWorld&&staticWorld->loaded()&&
    staticWorld->sourceOwner().get()==authored->sourceOwner()&&std::fegetround()==FE_TONEAREST,
    "GB graph requires the same actual cold source, authored placements and static world");
   d.status=Status::Capacity;
   need(availableAdditionalPayloadBytes>=maximumAdditionalPayloadBytes(),"GB graph exceeds remaining shared payload reservation");
   d.namedPreparationTemporaryBytes=namedPreparationTemporaryBytes();
   d.readOnlyPinPayloadBytes=readOnlyPinPayloadBytes();
   d.retainedPayloadBytes=retainedPayloadBytes();
   d.reservedAdditionalPayloadBytes=maximumAdditionalPayloadBytes();
   source_=std::move(authored);world_=std::move(staticWorld);
   const auto& cold=*source_->sourceOwner();const auto& archive=cold.archive();
   const auto& entries=archive.entries();const auto& instances=cold.world().instances;
   const auto* spans=world_->spans();const auto* mesh=world_->mesh();
   const auto* selections=world_->selections();const auto* textures=world_->textures();
   need(entries.size()==world::entryCount&&instances.size()==world::expectedInstances&&
    spans&&spans->size()==world::expectedSurfaces+world::expectedSecondPasses&&mesh&&selections&&textures,
    "GB actual source/static-world extent changed");
   PrepareScratch scratch{};
   d.status=Status::Source;
   for(std::size_t i=0;i<sourceRootCount;++i){
    d.ordinal=i;const auto& pin=pins::members[i];auto& root=roots_[i];
    need(pin.ordinal<entries.size(),"GB PSG member ordinal");const auto& entry=entries[pin.ordinal];
    // Cold Owner has already authenticated the complete original GB.RES and
    // owns immutable compact bytes for every member. Bind the exact indexed
    // member BEFORE interpreting any root word. No extraction/hash copy here.
    need(entry.name==pin.name&&entry.logical_offset==pin.logicalOffset&&entry.bytes==pin.bytes,
     "GB actual PSG directory identity changed");
    root.member=archive.member(pin.ordinal);
    need(root.member.identity==&entry&&root.member.data&&root.member.size==pin.bytes&&
     root.member.size>=pin.hierarchy.size(),"GB retained PSG view identity changed");
    need(std::memcmp(root.member.data,pin.hierarchy.data(),pin.hierarchy.size())==0,
     "GB exact original PSG hierarchy words changed");
    need(a::readPsgHierarchy49O(root.member.data,root.member.size,scratch.hierarchy,d.geometry)&&
     scratch.hierarchy.nodeCount==1&&a::readPsgNode49O(scratch.hierarchy,0,root.original)&&root.original.parent==-1,
     "GB actual one-root PSG rejected");
   }
   d.status=Status::Graph;
   for(std::size_t i=0;i<placementCount;++i){
    d.ordinal=i;auto& p=placements_[i];const auto& pin=pins::placements[i];
    need(pin.worldIndex<instances.size()&&pin.sourceRoot<roots_.size(),"GB placement pin range");
    p.source=&instances[pin.worldIndex];p.worldIndex=pin.worldIndex;p.sourceRoot=pin.sourceRoot;
    need(source::detail::same(p.source->geometry_member,pins::members[p.sourceRoot].name),"GB source placement/PSG join");
    const auto& original=roots_[p.sourceRoot].original;
    std::memcpy(p.node.localC0.v,original.local,sizeof original.local);
    std::memcpy(p.node.world100.v,original.local,sizeof original.local);
    p.node.parent180=p.node.firstChild184=p.node.nextSibling188=g::SceneNodeIndex49P::none;
    p.node.sceneFlags190=0;p.graph={&p.node,1};
    // Bounded NATIVE replacement policy, matching the actual static world's
    // SCB matrix. Four original PSG locals are nonidentity. Do not multiply
    // them into this placement or claim original EE/VU setter chronology.
    for(unsigned k=0;k<16;++k){scratch.placement.v[k]=static_cast<float>(p.source->matrix[k]);
     need(std::isfinite(p.source->matrix[k])&&std::isfinite(scratch.placement.v[k]),"GB placement binary32 projection");}
    need(g::setParentlessWorldMatrix49P(p.graph,static_cast<g::SceneNodeIndex49P>(0),scratch.placement)&&
     g::refreshSceneWorld49P(p.graph,static_cast<g::SceneNodeIndex49P>(0)),"GB native static placement graph");
   }
   d.status=Status::Association;
   for(std::size_t i=0;i<associationCount;++i){
    d.ordinal=i;const auto& pin=pins::associations[i];const auto& actual=source_->attachment(i);
    need(pin.placement<placementCount&&pin.descriptor<source::descriptorCount,"GB association pin range");
    const auto& p=placements_[pin.placement];const auto& descriptor=source_->descriptor(pin.descriptor);
    need(actual.descriptor==pin.descriptor&&actual.instanceOrdinal==pin.instanceOrdinal&&
     actual.worldIndex==p.worldIndex&&actual.worldInstance==p.source&&descriptor.childObject=="NONE",
     "GB exact source association/placement identity changed");
    associationPlacement_[i]=pin.placement;
   }
   d.status=Status::Spans;std::size_t sourceSurfaces=0;
   for(std::size_t i=0;i<placementCount;++i){
    d.ordinal=i;auto& p=placements_[i];p.firstSpan=spanCount_;
    std::size_t surface=0,pass=1,passCount=0;const tm::Span* previous=nullptr;
    // Scan the entire existing table. Every span for this actual placement is
    // included, in its original source-surface/material-pass order.
    for(std::size_t si=0;si<spans->size();++si){const auto& s=(*spans)[si];if(s.worldInstanceIndex!=p.worldIndex)continue;
     need(s.surfaceIndex==surface&&s.sourcePass==pass&&s.sourcePassCount>=1&&s.sourcePassCount<=2,
      "GB full source surface/material-pass ordering");
     need(s.vertexCount&&s.indexCount&&s.indexCount%3==0&&s.vertexStart<=mesh->vertices.size()&&
      s.vertexCount<=mesh->vertices.size()-s.vertexStart&&s.indexStart<=mesh->indices.size()&&
      s.indexCount<=mesh->indices.size()-s.indexStart&&s.materialSelection<selections->size()&&s.textureIndex<textures->size(),
      "GB actual static span extent/selection");
     const auto& selection=(*selections)[s.materialSelection];
     need(selection.pass==s.sourcePass&&selection.passCount==s.sourcePassCount&&selection.textureIndex==s.textureIndex&&
      s.projectionPass==(selection.projectedPrelit?selection.pass:0),"GB actual material-pass identity");
     if(pass==1)passCount=s.sourcePassCount;
     else need(previous&&passCount==s.sourcePassCount&&sameGeometry(*previous,s),"GB second pass must share actual geometry");
     need(spanCount_<spanIndices_.size(),"GB source span-reference capacity");
     spanIndices_[spanCount_++]=static_cast<std::uint32_t>(si);previous=&s;
     if(pass==passCount){++surface;pass=1;}else ++pass;
    }
    p.spanCount=spanCount_-p.firstSpan;
    need(p.spanCount&&surface==pins::members[p.sourceRoot].surfaceCount&&pass==1,
     "GB source placement requires every surface and every material pass");sourceSurfaces+=surface;
   }
   need(sourceSurfaces==sourceSurfaceReferences&&spanCount_==maximumSpanReferences,
    "GB exact 55-placement source-surface/full-pass span extent");
   sourceGraphReady_=true;d.status=Status::SourceGraphReady;
   d.retainedPayloadBytes=retainedPayloadBytes();d.spanReferences=spanCount_;d.message.clear();return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
   catch(...){try{d.message="GB attachment graph preparation failed";}catch(...){}return false;}
 }
 bool sourceGraphReady()const noexcept {
  return thread()&&sourceGraphReady_&&source_&&source_->ready()&&world_&&world_->loaded()&&
   world_->sourceOwner().get()==source_->sourceOwner();
 }
 const source::Owner* authoringOwner()const noexcept{return sourceGraphReady()?source_.get():nullptr;}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return sourceGraphReady()?source_->sourceOwner():nullptr;}
 const world::Owner* staticWorldOwner()const noexcept{return sourceGraphReady()?world_.get():nullptr;}
 const a::WorldInstance48F* placement(std::size_t i)const noexcept{const auto* p=association(i);return p?p->source:nullptr;}
 const g::BodyMatrix48G* currentWorld(std::size_t i)const noexcept{const auto* p=association(i);return p?&p->node.world100:nullptr;}
 const a::PsgNode49O* originalRoot(std::size_t i)const noexcept{const auto* p=association(i);return p?&roots_[p->sourceRoot].original:nullptr;}
 std::size_t placementIndex(std::size_t i)const noexcept{return association(i)?associationPlacement_[i]:placementCount;}
 std::size_t spanReferenceCount()const noexcept{return sourceGraphReady()?spanCount_:0;}
 std::size_t spanCount(std::size_t i)const noexcept{const auto* p=association(i);return p?p->spanCount:0;}
 const std::uint32_t* spanIndices(std::size_t i)const noexcept{const auto* p=association(i);return p?spanIndices_.data()+p->firstSpan:nullptr;}
 // Observations borrow this Owner's lifetime. No raw pointer grants a renderer
 // publication, Type11 reset/start/enable, numerical-position or live query right.
};
} // namespace gb_particle_attachments50GB
