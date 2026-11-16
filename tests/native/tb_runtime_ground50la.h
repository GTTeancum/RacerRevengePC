#pragma once
#include "course_archive_receipt50ci.h"
#include "tb_cold_sources50la.h"
#include "authored_runtime_ground50cp_fixture.h"
#include <memory>
#include <stdexcept>
#include <thread>
#include <limits>
#include <exception>

// TB-only same-decoded-source adapter. Every COL placement remains separately
// owned and source ordered. This performs no archive/TBB or navigation-gate read.
namespace tb_runtime_ground50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace original=authored_runtime_ground50cp_fixture;
class Owner final {
 std::shared_ptr<const tb_cold_sources50LA::Owner> source_;
 std::unique_ptr<original::ColdScene> scene_;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,humanLiftBound_=false;
 static void add(std::size_t& n,std::size_t bytes) noexcept {
  if(bytes>std::numeric_limits<std::size_t>::max()-n)n=std::numeric_limits<std::size_t>::max();else n+=bytes;
 }
 template<class T> static void vectorBytes(std::size_t& n,const std::vector<T>& v) noexcept {
  if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(n,v.capacity()*sizeof(T));
 }
 static void textBytes(std::size_t& n,const std::string& s) noexcept {add(n,s.capacity());add(n,1);}
 static void worldBytes(std::size_t& n,const a::World48F& w) noexcept {
  vectorBytes(n,w.parameters.original_bytes);vectorBytes(n,w.parameters.blocks);
  for(const auto& b:w.parameters.blocks){textBytes(n,b.name);vectorBytes(n,b.fields);for(const auto& f:b.fields){textBytes(n,f.name);textBytes(n,f.value);}}
  vectorBytes(n,w.zones);vectorBytes(n,w.portals);for(const auto& p:w.portals)textBytes(n,p.section);
  vectorBytes(n,w.instances);for(const auto& i:w.instances){textBytes(n,i.section);textBytes(n,i.geometry_member);textBytes(n,i.collision_member);}
 }
 static bool hasCollisionMember(const a::IndexedResArchive48C& archive,const std::string& member){
  for(const auto& entry:archive.entries())if(a::OriginalStandardArchiveProvider50LA::sameName(entry.name,member))return true;
  return false;
 }
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static std::array<float,3> vector(const a::ParameterBlock48F& block,const char* key){
  const std::string* text=nullptr;
  for(const auto& field:block.fields)if(field.name==key){need(!text,"TB duplicate placement vector");text=&field.value;}
  need(text,"TB missing placement vector");std::array<float,3> out{};std::size_t begin=0;
  for(unsigned axis=0;axis<3;++axis){auto end=text->find(',',begin);
   need((end==std::string::npos)==(axis==2),"TB placement vector extent");
   if(end==std::string::npos)end=text->size();auto first=begin,last=end;
   while(first<last&&(*text)[first]==' ')++first;while(last>first&&(*text)[last-1]==' ')--last;
   need(a::parseTrackNumber50AD(text->data()+first,last-first,out[axis]),"TB placement numeric source");begin=end+1;
  }return out;
 }
public:
 Owner()=default;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 // Conservative descriptor reservation before copying the actual World.
 // The independently bounded COL geometry/radii domain is unchanged. Two
 // copies of observed source capacities bound the fresh copy and temporary
 // source strings in this fixed libstdc++ parent profile. Recheck retained
 // capacities after preparation; do not reclaim this reservation early.
 static std::size_t maximumDescriptorPayloadBytes(const tb_cold_sources50LA::Owner& source)noexcept {
  if(!source.ready())return std::numeric_limits<std::size_t>::max();
  std::size_t world=0;worldBytes(world,source.world());
  std::size_t n=sizeof(Owner)+sizeof(original::ColdScene);add(n,world);add(n,world);
  std::size_t count=0;for(const auto& i:source.world().instances)if(hasCollisionMember(source.archive(),i.collision_member)){
   ++count;const auto text=std::max<std::size_t>(15,i.collision_member.size())+1;add(n,text);add(n,text);
  }
  // Placed COL owners from the data (the stock count is a receipt), bounded
  // by the native collision owner store.
  if(!count||count>original::hardCollisionOwners50IG)return std::numeric_limits<std::size_t>::max();
  add(n,count*sizeof(typename decltype(original::ColdScene::owners)::value_type));
  // Exact13-character tracks\\tb.scb plus absent water name; full original
  //260-byte path extent for both staging and retained string is conservative.
  add(n,4u*260u);return n;
 }
 bool prepare(std::shared_ptr<const tb_cold_sources50LA::Owner> source,
     a::TrackCollisionMasks50AN actualMasks,const std::vector<std::uint8_t>& actualSelectedPhy,
     unsigned actualGridRank,std::string& error)noexcept {
  if(thread_!=std::this_thread::get_id()){error="TB ground owner thread mismatch";return false;}
  if(attempted_){error="TB ground owner already attempted";return false;}attempted_=true;
  try{
   need(source&&source->ready()&&!source->world().zones.empty()&&source->map().gatesPrepared()&&source->map().count()&&
       source->ai().gatesPrepared()&&source->ai().count(),
       "TB ground requires the actual cold source and prepared gates");
   auto next=std::make_unique<original::ColdScene>();next->world=source->world();
   next->stackCount=1; // No retained JSON audit allocation; source receipts are external.
   for(std::size_t zone=0;zone<next->world.zones.size();++zone){
    const auto selection=a::readZoneSearchSelection50CN(next->world,zone);
    need(selection.status==a::ZoneSearchReadStatus50CN::Complete&&
        selection.selection.collisionStatic118==a::ZoneSearchKind50CN::List&&
        selection.selection.collisionDynamic11C==a::ZoneSearchKind50CN::List,
        "TB authored List collision methods");
   }
   std::size_t count=0;for(const auto& instance:next->world.instances)
    if(hasCollisionMember(source->archive(),instance.collision_member))++count;
   course_archive_receipt50CI::countReceipt("tb_runtime_ground","placed_col",count,320);
   need(count&&count<=original::hardCollisionOwners50IG,"TB placed COL count exceeds the native collision owner store");
   // A single cold reservation precedes every mesh/name borrow. No subsequent
   // resize, sorting or deduplication is allowed after publication.
   next->owners.reserve(count);a::DecodeError48C decode;
   for(const auto& instance:next->world.instances){
    if(!hasCollisionMember(source->archive(),instance.collision_member))continue;
    need(next->owners.size()<count&&instance.parameter_block<next->world.parameters.blocks.size(),"TB actual COL source row");
    next->owners.emplace_back();auto& out=next->owners.back();
    out.name=instance.collision_member;out.zone=instance.zone;out.ordinal=instance.ordinal;
    std::vector<std::uint8_t> bytes,cache;a::Collision48G col;std::string why;
    need(source->extractMember(out.name,bytes,decode,8u*1024u*1024u)&&a::decodeCollision48G(bytes,col,decode),"TB actual placed COL decode");
    need(a::encodeCollisionAsset48G(col,cache,why)&&a::loadCollisionMesh48I(cache,out.mesh,why),"TB native COL mesh preparation");
    const auto& block=next->world.parameters.blocks[instance.parameter_block];
    const auto position=vector(block,"position"),look=vector(block,"lookvector"),up=vector(block,"upvector");
    need(a::prepareTrackCollider50AO(out.mesh,position,look,up,out.name.data(),out.name.size(),actualMasks,out.instance),
        "TB actual COL placement/category preparation");
    next->stackCount=std::max(next->stackCount,out.mesh.nodes.size()+out.mesh.faces.size()+1);
   }
   need(next->owners.size()==count,"TB COL source order/extent changed");
   std::string heightError;
   need(track_height_cold50JB::prepareFreshGameTrack(source,"tracks\\tb.scb",next->height,heightError),
       "TB actual fresh height descriptor rejected");
   // The fresh-game branch leaves terrain330 absent even though the retained
   // TB descriptor names nat13.trn. WaterFile is absent. No activation is inferred.
   need(next->height.sourceDomain.water334==g::UndertrackHeightOwner50EW::Absent&&
       next->height.sourceDomain.terrain330==g::UndertrackHeightOwner50EW::Absent&&
       !next->height.waterNativeQueryBound&&!next->height.nativeWaterResources&&
       next->height.waterSource.empty(),"TB fresh height owner differs");
   const auto graph=source->map().track();
   need(actualGridRank<=7&&graph.count(),"TB actual grid rank/map bound");
   std::uint32_t previous=0;
   for(;previous<graph.count();++previous){const auto& node=graph.nodes()[previous];
    need(node.nextCount&&node.nextCount<=g::maxUndertrackNext50ET,"TB grid predecessor edge");
    if(node.next[0]==0)break;}
   need(previous<graph.count()&&graph.nodes()[0].nextCount,"TB original grid predecessor");
   g::GridCenterInput50AC grid{};
   std::memcpy(grid.current,graph.nodes()[0].position,sizeof(grid.current));
   std::memcpy(grid.previous,graph.nodes()[previous].position,sizeof(grid.previous));
   grid.rank=actualGridRank;grid.nextNode=graph.nodes()[0].next[0];
   a::VisualAuthoredValues49R visual{};a::VisualAuthoringDiagnostic49R visualDiagnostic{};
   need(a::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(actualSelectedPhy.data()),actualSelectedPhy.size(),visual,visualDiagnostic),
       "TB actual selected-racer PHY");
   next->lift=visual.values[static_cast<unsigned>(a::VisualAuthoredKey49R::MaxLiftHeight)];
   need(g::prepareCenterPose50AC(grid,next->lift,{{0,1,0}},next->pose),"TB actual selected grid center pose");
   source_=std::move(source);scene_=std::move(next);error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="TB ground allocation or source preparation failed";}catch(...){}return false;}
 }
 // Called once from the real racer0 rig/class installation, before grounded
 // grid placement. Only this scalar changes; all mesh/world backing stays fixed.
 bool bindHumanLift(const tb_cold_sources50LA::Owner& source,float actualSelectedLift)noexcept {
  if(thread_!=std::this_thread::get_id()||!scene_||!source_||!source_->ready()||source_.get()!=&source||humanLiftBound_||!std::isfinite(actualSelectedLift))return false;
  scene_->lift=actualSelectedLift;humanLiftBound_=true;return true;
 }
 // Descriptors/duplicated World strings only. Shared cold source is charged
 // once by the aggregate. Geometry and later radii value buffers are reported
 // separately, retaining the existing decoder/resource bounds.
 std::size_t descriptorPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return std::numeric_limits<std::size_t>::max();
  std::size_t n=sizeof(Owner);if(!scene_)return n;add(n,sizeof(original::ColdScene));
  vectorBytes(n,scene_->owners);for(const auto& owner:scene_->owners)textBytes(n,owner.name);
  worldBytes(n,scene_->world);textBytes(n,scene_->height.sceneMember);textBytes(n,scene_->height.waterMember);
  vectorBytes(n,scene_->height.waterSource);return n;
 }
 std::size_t meshPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return std::numeric_limits<std::size_t>::max();
  std::size_t n=0;if(scene_)for(const auto& owner:scene_->owners){const auto& m=owner.mesh;
   vectorBytes(n,m.vertices);vectorBytes(n,m.vertex_normals);vectorBytes(n,m.nodes);vectorBytes(n,m.faces);}
  return n;
 }
 const original::ColdScene* ground()const noexcept{return thread_==std::this_thread::get_id()&&source_&&source_->ready()?scene_.get():nullptr;}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return ground()?source_.get():nullptr;}
};
}
