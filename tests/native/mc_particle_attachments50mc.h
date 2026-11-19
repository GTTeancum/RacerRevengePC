#pragma once
#include "mc_particle_source50mc.h"
#include "mc_original_world50lm.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/scene_graph49p.h"
namespace mc_particle_runtime50MC {class Owner;}
namespace mc_particle_attachments50MC {
namespace a=rr::assets;namespace g=rr::game;namespace source=mc_particle_source50MC;
// Capacities: placements = distinct attached world models (stock MC: 2, at
// world 282 and 441, receipts); associations = descriptor slots of the common
// emitter table (stock: rows 0..7 attached). PORT STAND-IN (flagged): the
// native placement store holds 2 models of at most 5 spans each.
inline constexpr std::size_t placementCount=2,associationCount=source::descriptorCount,maxPlacementSpans=5;
struct Diagnostic {std::string message;std::size_t placement=0,spanReferences=0; a::GeometryDiagnostic49O geometry{};};
// Exact original attached placement roots. These graphs do not prove a draw or
// stamp visibility. All static spans are borrowed from the SAME native World.
class Owner final {
 friend class ::mc_particle_runtime50MC::Owner;
 struct Placement {g::SceneNode49P node{};g::SceneGraph49P graph{};a::PsgNode49O original{};
  a::IndexedResArchive48C::Member member{};const a::WorldInstance48F* source=nullptr;std::size_t worldIndex=0,spanCount=0;std::array<std::uint32_t,maxPlacementSpans> spans{};};
 std::shared_ptr<const source::Owner> source_;std::shared_ptr<const mc_original_world50LM::Owner> world_;
 std::array<Placement,placementCount> placements_{};std::size_t placementLive_=0;
 std::array<std::uint8_t,associationCount> placementOf_{};std::size_t associations_=0,spanReferences_=0;const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 static void need(bool b,const char* s){if(!b)throw std::runtime_error(s);}
 bool attached(std::size_t a)const noexcept{return a<source_->emitterCount()&&placementOf_[a]<placementLive_;}
 const g::SceneGraph49P* graph(std::size_t association)const noexcept{return sourceGraphReady()&&attached(association)?&placements_[placementOf_[association]].graph:nullptr;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 static constexpr std::size_t maximumAdditionalPayloadBytes(){return sizeof(Owner)+sizeof(a::PsgHierarchyView49O)+g::maxSceneNodes49P*(sizeof(bool)+sizeof(g::SceneNodeIndex49P)+sizeof(g::BodyMatrix48G));}
 bool prepare(std::shared_ptr<const source::Owner> src,std::shared_ptr<const mc_original_world50LM::Owner> world,std::size_t available,Diagnostic& d)noexcept{
  d={};if(thread_!=std::this_thread::get_id()||attempted_)return false;attempted_=true;
  try{need(src&&src->ready()&&world&&world->loaded()&&world->sourceOwner().get()==src->coldSource(),"MC graph requires same retained source/static world");
   need(available>=maximumAdditionalPayloadBytes()&&std::fegetround()==FE_TONEAREST,"MC graph budget/rounding");source_=std::move(src);world_=std::move(world);
   const auto& archive=source_->coldSource()->archive();const auto& entries=archive.entries();const auto* spans=world_->spans();need(spans,"MC actual spans absent");
   placementOf_.fill(0xff);
   // One placement per distinct attached world model, in descriptor order.
   for(std::size_t row=0;row<source_->emitterCount();++row){if(!source_->descriptor(row).attached)continue;
    const auto& attachment=source_->attachment(row);need(attachment.worldInstance&&attachment.descriptor==row,"MC attached descriptor join");
    std::size_t k=0;while(k<placementLive_&&placements_[k].source!=attachment.worldInstance)++k;
    if(k==placementLive_){need(k<placements_.size(),"PORT STAND-IN: MC native placement store holds 2 attached models");
     d.placement=k;auto& p=placements_[k];p.source=attachment.worldInstance;p.worldIndex=attachment.worldIndex;std::size_t index=entries.size();
     for(std::size_t n=0;n<entries.size();++n)if(source::detail::same(entries[n].name,p.source->geometry_member.c_str())){need(index==entries.size(),"MC duplicate PSG member");index=n;}
     need(index<entries.size(),"MC original attached PSG absent");p.member=archive.member(index);a::PsgHierarchyView49O hierarchy{};
     need(p.member.identity==&entries[index]&&a::readPsgHierarchy49O(p.member.data,p.member.size,hierarchy,d.geometry)&&a::readPsgNode49O(hierarchy,0,p.original)&&hierarchy.nodeCount==1&&p.original.parent==-1,"MC original PSG root");
     p.node.parent180=p.node.firstChild184=p.node.nextSibling188=g::SceneNodeIndex49P::none;p.graph={&p.node,1};g::BodyMatrix48G matrix{};
     for(unsigned m=0;m<16;++m){matrix.v[m]=static_cast<float>(p.source->matrix[m]);need(std::isfinite(matrix.v[m]),"MC native placement finite");}
     // Same bounded native parentless replacement as static World, not a claim
     // of original EE/VU setter chronology or child-geometry replay.
     need(g::setParentlessWorldMatrix49P(p.graph,static_cast<g::SceneNodeIndex49P>(0),matrix)&&g::refreshSceneWorld49P(p.graph,static_cast<g::SceneNodeIndex49P>(0)),"MC actual root graph");
     for(std::size_t si=0;si<spans->size();++si)if((*spans)[si].worldInstanceIndex==p.worldIndex){need(p.spanCount<p.spans.size(),"MC placement span capacity");p.spans[p.spanCount++]=static_cast<std::uint32_t>(si);}
     need(p.spanCount>0,"MC attached placement has no static spans");
     // Stock span counts (5 at world 282, 3 at 441) are receipts.
     if((p.worldIndex==282u&&p.spanCount!=5)||(p.worldIndex==441u&&p.spanCount!=3))std::cerr<<"course_count site=particles_MC placement_spans="<<p.spanCount<<" world="<<p.worldIndex<<'\n';
     d.spanReferences+=p.spanCount;spanReferences_+=p.spanCount;++placementLive_;}
    placementOf_[row]=static_cast<std::uint8_t>(k);++associations_;
   }
   std::cerr<<"particle_attachments site=MC associations="<<associations_<<" placements="<<placementLive_<<" spans="<<spanReferences_<<'\n';
   ready_=true;return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
 }
 bool sourceGraphReady()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready()&&world_&&world_->loaded()&&world_->sourceOwner().get()==source_->coldSource();}
 const source::Owner* authoringOwner()const noexcept{return sourceGraphReady()?source_.get():nullptr;}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return sourceGraphReady()?source_->coldSource():nullptr;}
 const mc_original_world50LM::Owner* staticWorldOwner()const noexcept{return sourceGraphReady()?world_.get():nullptr;}
 const a::WorldInstance48F* placement(std::size_t i)const noexcept{return sourceGraphReady()&&attached(i)?placements_[placementOf_[i]].source:nullptr;}
 // Span references summed over distinct placements (stock: 5 + 3 = 8).
 std::size_t spanReferenceCount()const noexcept{return sourceGraphReady()?spanReferences_:0;}
 std::size_t associationCountLive()const noexcept{return sourceGraphReady()?associations_:0;}
 std::size_t placementCountLive()const noexcept{return sourceGraphReady()?placementLive_:0;}
 std::size_t spanCount(std::size_t i)const noexcept{return sourceGraphReady()&&attached(i)?placements_[placementOf_[i]].spanCount:0;}
 const std::uint32_t* spanIndices(std::size_t i)const noexcept{return sourceGraphReady()&&attached(i)?placements_[placementOf_[i]].spans.data():nullptr;}
 // Placement of association i; placementCount when i is not attached.
 std::size_t placementIndex(std::size_t i)const noexcept{return sourceGraphReady()&&attached(i)?placementOf_[i]:placementCount;}
};
}
