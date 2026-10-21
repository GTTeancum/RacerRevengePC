#pragma once
#include "mc_particle_source50mc.h"
#include "mc_original_world50lm.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/game/scene_graph49p.h"
namespace mc_particle_runtime50MC {class Owner;}
namespace mc_particle_attachments50MC {
namespace a=rr::assets;namespace g=rr::game;namespace source=mc_particle_source50MC;
inline constexpr std::size_t placementCount=2,associationCount=8,sourceSurfaceReferences=8;
struct Diagnostic {std::string message;std::size_t placement=0,spanReferences=0; a::GeometryDiagnostic49O geometry{};};
// Exact original attached placement roots. These graphs do not prove a draw or
// stamp visibility. All static spans are borrowed from the SAME native World.
class Owner final {
 friend class ::mc_particle_runtime50MC::Owner;
 struct Placement {g::SceneNode49P node{};g::SceneGraph49P graph{};a::PsgNode49O original{};
  a::IndexedResArchive48C::Member member{};const a::WorldInstance48F* source=nullptr;std::size_t worldIndex=0,spanCount=0;std::array<std::uint32_t,5> spans{};};
 std::shared_ptr<const source::Owner> source_;std::shared_ptr<const mc_original_world50LM::Owner> world_;
 std::array<Placement,2> placements_{};const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 static void need(bool b,const char* s){if(!b)throw std::runtime_error(s);}
 const g::SceneGraph49P* graph(std::size_t association)const noexcept{return sourceGraphReady()&&association<8?&placements_[association<2?0:1].graph:nullptr;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 static constexpr std::size_t maximumAdditionalPayloadBytes(){return sizeof(Owner)+sizeof(a::PsgHierarchyView49O)+g::maxSceneNodes49P*(sizeof(bool)+sizeof(g::SceneNodeIndex49P)+sizeof(g::BodyMatrix48G));}
 bool prepare(std::shared_ptr<const source::Owner> src,std::shared_ptr<const mc_original_world50LM::Owner> world,std::size_t available,Diagnostic& d)noexcept{
  d={};if(thread_!=std::this_thread::get_id()||attempted_)return false;attempted_=true;
  try{need(src&&src->ready()&&world&&world->loaded()&&world->sourceOwner().get()==src->coldSource(),"MC graph requires same retained source/static world");
   need(available>=maximumAdditionalPayloadBytes()&&std::fegetround()==FE_TONEAREST,"MC graph budget/rounding");source_=std::move(src);world_=std::move(world);
   const auto& archive=source_->coldSource()->archive();const auto& entries=archive.entries();const auto* spans=world_->spans();need(spans,"MC actual spans absent");
   for(std::size_t i=0;i<2;++i){d.placement=i;auto& p=placements_[i];const auto& attachment=source_->attachment(i?2:0);p.source=attachment.worldInstance;p.worldIndex=attachment.worldIndex;
    need(p.source&&p.worldIndex==(i?441u:282u),"MC exact attached world placement");std::size_t index=entries.size();
    for(std::size_t n=0;n<entries.size();++n)if(source::detail::same(entries[n].name,p.source->geometry_member.c_str())){need(index==entries.size(),"MC duplicate PSG member");index=n;}
    need(index<entries.size(),"MC original attached PSG absent");p.member=archive.member(index);a::PsgHierarchyView49O hierarchy{};
    need(p.member.identity==&entries[index]&&a::readPsgHierarchy49O(p.member.data,p.member.size,hierarchy,d.geometry)&&a::readPsgNode49O(hierarchy,0,p.original)&&hierarchy.nodeCount==1&&p.original.parent==-1,"MC original PSG root");
    p.node.parent180=p.node.firstChild184=p.node.nextSibling188=g::SceneNodeIndex49P::none;p.graph={&p.node,1};g::BodyMatrix48G matrix{};
    for(unsigned k=0;k<16;++k){matrix.v[k]=static_cast<float>(p.source->matrix[k]);need(std::isfinite(matrix.v[k]),"MC native placement finite");}
    // Same bounded native parentless replacement as static World, not a claim
    // of original EE/VU setter chronology or child-geometry replay.
    need(g::setParentlessWorldMatrix49P(p.graph,static_cast<g::SceneNodeIndex49P>(0),matrix)&&g::refreshSceneWorld49P(p.graph,static_cast<g::SceneNodeIndex49P>(0)),"MC actual root graph");
    for(std::size_t si=0;si<spans->size();++si)if((*spans)[si].worldInstanceIndex==p.worldIndex){need(p.spanCount<p.spans.size(),"MC exact span capacity");p.spans[p.spanCount++]=si;}need(p.spanCount==(i?3u:5u),"MC full static attachment spans absent");d.spanReferences+=p.spanCount;
   }
   for(std::size_t i=0;i<8;++i)need(source_->attachment(i).worldInstance==placements_[i<2?0:1].source&&source_->attachment(i).descriptor==i,"MC exact eight association joins");
   ready_=true;return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}
 }
 bool sourceGraphReady()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&source_&&source_->ready()&&world_&&world_->loaded()&&world_->sourceOwner().get()==source_->coldSource();}
 const source::Owner* authoringOwner()const noexcept{return sourceGraphReady()?source_.get():nullptr;}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return sourceGraphReady()?source_->coldSource():nullptr;}
 const mc_original_world50LM::Owner* staticWorldOwner()const noexcept{return sourceGraphReady()?world_.get():nullptr;}
 const a::WorldInstance48F* placement(std::size_t i)const noexcept{return sourceGraphReady()&&i<8?placements_[i<2?0:1].source:nullptr;}
 std::size_t spanReferenceCount()const noexcept{return sourceGraphReady()?8:0;}
 std::size_t spanCount(std::size_t i)const noexcept{return sourceGraphReady()&&i<8?placements_[i<2?0:1].spanCount:0;}
 const std::uint32_t* spanIndices(std::size_t i)const noexcept{return sourceGraphReady()&&i<8?placements_[i<2?0:1].spans.data():nullptr;}
 std::size_t placementIndex(std::size_t i)const noexcept{return i<8?(i<2?0:1):2;}
};
}
