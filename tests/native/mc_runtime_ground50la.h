#pragma once
#include "mc_cold_sources50la.h"
#include "authored_runtime_ground50cp_fixture.h"
#include <memory>
#include <stdexcept>
#include <thread>
#include <limits>
#include <exception>

// MC-only same-decoded-source adapter. Every COL placement remains separately
// owned and source ordered. This performs no archive/TOB or navigation-gate read.
namespace mc_runtime_ground50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace original=authored_runtime_ground50cp_fixture;
class Owner final {
 std::shared_ptr<const mc_cold_sources50LA::Owner> source_;
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
  for(const auto& field:block.fields)if(field.name==key){need(!text,"MC duplicate placement vector");text=&field.value;}
  need(text,"MC missing placement vector");std::array<float,3> out{};std::size_t begin=0;
  for(unsigned axis=0;axis<3;++axis){auto end=text->find(',',begin);
   need((end==std::string::npos)==(axis==2),"MC placement vector extent");
   if(end==std::string::npos)end=text->size();auto first=begin,last=end;
   while(first<last&&(*text)[first]==' ')++first;while(last>first&&(*text)[last-1]==' ')--last;
   need(a::parseTrackNumber50AD(text->data()+first,last-first,out[axis]),"MC placement numeric source");begin=end+1;
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
 static std::size_t maximumDescriptorPayloadBytes(const mc_cold_sources50LA::Owner& source)noexcept {
  if(!source.ready()||source.world().instances.size()!=458)return std::numeric_limits<std::size_t>::max();
  std::size_t world=0;worldBytes(world,source.world());
  std::size_t n=sizeof(Owner)+sizeof(original::ColdScene);add(n,world);add(n,world);
  add(n,89u*sizeof(typename decltype(original::ColdScene::owners)::value_type));
  std::size_t count=0;for(const auto& i:source.world().instances)if(hasCollisionMember(source.archive(),i.collision_member)){
   ++count;const auto text=std::max<std::size_t>(15,i.collision_member.size())+1;add(n,text);add(n,text);
  }
  if(count!=89)return std::numeric_limits<std::size_t>::max();
  // Exact MC scene path and retained3097-byte WTT; full original
  //260-byte path extent for both staging and retained string is conservative.
  add(n,4u*260u+2u*3097u+65536u);return n;
 }
 bool prepare(std::shared_ptr<const mc_cold_sources50LA::Owner> source,
     a::TrackCollisionMasks50AN actualMasks,const std::vector<std::uint8_t>& actualSelectedPhy,
     unsigned actualGridRank,std::string& error)noexcept {
  if(thread_!=std::this_thread::get_id()){error="MC ground owner thread mismatch";return false;}
  if(attempted_){error="MC ground owner already attempted";return false;}attempted_=true;
  try{
   need(source&&source->ready()&&source->world().zones.size()==28&&source->world().portals.size()==28&&
       source->world().instances.size()==458&&source->map().gatesPrepared()&&source->map().count()==173&&
       source->ai().gatesPrepared()&&source->ai().count()==173,
       "MC ground requires actual authenticated source and exact gate extents");
   auto next=std::make_unique<original::ColdScene>();next->world=source->world();
   next->stackCount=1; // No retained JSON audit allocation; source receipts are external.
   for(std::size_t zone=0;zone<next->world.zones.size();++zone){
    const auto selection=a::readZoneSearchSelection50CN(next->world,zone);
    need(selection.status==a::ZoneSearchReadStatus50CN::Complete&&
        selection.selection.collisionStatic118==a::ZoneSearchKind50CN::List&&
        selection.selection.collisionDynamic11C==a::ZoneSearchKind50CN::List,
        "MC authored List collision methods");
   }
   std::size_t count=0;for(const auto& instance:next->world.instances)
    if(hasCollisionMember(source->archive(),instance.collision_member))++count;
   need(count==89&&count<=original::hardCollisionOwners50IG,"MC exact placed COL count");
   // A single cold reservation precedes every mesh/name borrow. No subsequent
   // resize, sorting or deduplication is allowed after publication.
   next->owners.reserve(count);a::DecodeError48C decode;
   for(const auto& instance:next->world.instances){
    if(!hasCollisionMember(source->archive(),instance.collision_member))continue;
    need(next->owners.size()<count&&instance.parameter_block<next->world.parameters.blocks.size(),"MC actual COL source row");
    next->owners.emplace_back();auto& out=next->owners.back();
    out.name=instance.collision_member;out.zone=instance.zone;out.ordinal=instance.ordinal;
    std::vector<std::uint8_t> bytes,cache;a::Collision48G col;std::string why;
    need(source->extractMember(out.name,bytes,decode,8u*1024u*1024u)&&a::decodeCollision48G(bytes,col,decode),"MC actual placed COL decode");
    need(a::encodeCollisionAsset48G(col,cache,why)&&a::loadCollisionMesh48I(cache,out.mesh,why),"MC native COL mesh preparation");
    const auto& block=next->world.parameters.blocks[instance.parameter_block];
    std::array<float,3> position{},look{},up{};
    for(unsigned k=0;k<3;++k){position[k]=static_cast<float>(instance.position[k]);look[k]=static_cast<float>(instance.look[k]);up[k]=static_cast<float>(instance.up[k]);}
    need(a::prepareTrackCollider50AO(out.mesh,position,look,up,out.name.data(),out.name.size(),actualMasks,out.instance),
        "MC actual COL placement/category preparation");
    next->stackCount=std::max(next->stackCount,out.mesh.nodes.size()+out.mesh.faces.size()+1);
   }
   need(next->owners.size()==count,"MC COL source order/extent changed");
   // MC retains duplicate unused Environment labels. Use original per-field
   // provenance checks without clearing that policy or rewriting SCB bytes.
   auto& height=next->height;const a::ParameterBlock48F* environment=nullptr;std::size_t environmentIndex=0;
   for(std::size_t i=0;i<source->scb().blocks.size();++i)if(source->scb().blocks[i].name=="environment"){
    need(!environment,"MC duplicate Environment section");environment=&source->scb().blocks[i];environmentIndex=i;}
   need(environment,"MC Environment source missing");
   need(a::track_height50jb_detail::readString(source->scb(),*environment,"terrainfile",height.descriptor.terrainFile0C)==a::TrackHeightReadStatus50JB::Complete&&
    a::track_height50jb_detail::readString(source->scb(),*environment,"waterfile",height.descriptor.waterFile110)==a::TrackHeightReadStatus50JB::Complete,"MC exact height field provenance");
   height.descriptor.environmentBlock=environmentIndex;height.descriptor.bound=true;
   need(!height.descriptor.terrainFile0C.defaulted&&!height.descriptor.waterFile110.defaulted&&
    std::strcmp(height.descriptor.terrainFile0C.value.data(),"nat13.trn")==0&&std::strcmp(height.descriptor.waterFile110.value.data(),"mc.wtt")==0,"MC original height source identities");
   height.sceneMember="tracks\\mc.scb";height.waterMember="tracks\\mc.wtt";
   need(source->extractMember(height.waterMember,height.waterSource,decode,3097)&&height.waterSource.size()==3097&&
    pod_materials50ed_fixture::detail::sha256(height.waterSource)=="43ce93a91873f4b32edcfea72f5f6296d178d9e3d48d2730b566f6f5177d15fb","MC original water source");
   height.waterLogicalOffset=466944;height.sourceBound=true;
   // Fresh1A9EE0 path does not construct terrain. Water IS required; resource
   // retention is not native height-query authority, and remains unbound here.
   height.sourceDomain.terrain330=g::UndertrackHeightOwner50EW::Absent;
   height.sourceDomain.water334=g::UndertrackHeightOwner50EW::Present;
   const auto graph=source->map().track();
   need(actualGridRank<=7&&graph.count()==173,"MC actual grid rank/map bound");
   std::uint32_t previous=0;
   for(;previous<graph.count();++previous){const auto& node=graph.nodes()[previous];
    need(node.nextCount&&node.nextCount<=g::maxUndertrackNext50ET,"MC grid predecessor edge");
    if(node.next[0]==0)break;}
   need(previous<graph.count()&&graph.nodes()[0].nextCount,"MC original grid predecessor");
   g::GridCenterInput50AC grid{};
   std::memcpy(grid.current,graph.nodes()[0].position,sizeof(grid.current));
   std::memcpy(grid.previous,graph.nodes()[previous].position,sizeof(grid.previous));
   grid.rank=actualGridRank;grid.nextNode=graph.nodes()[0].next[0];
   a::VisualAuthoredValues49R visual{};a::VisualAuthoringDiagnostic49R visualDiagnostic{};
   need(a::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(actualSelectedPhy.data()),actualSelectedPhy.size(),visual,visualDiagnostic),
       "MC actual selected-racer PHY");
   next->lift=visual.values[static_cast<unsigned>(a::VisualAuthoredKey49R::MaxLiftHeight)];
   need(g::prepareCenterPose50AC(grid,next->lift,{{0,1,0}},next->pose),"MC actual selected grid center pose");
   source_=std::move(source);scene_=std::move(next);error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="MC ground allocation or source preparation failed";}catch(...){}return false;}
 }
 // Called once from the real racer0 rig/class installation, before grounded
 // grid placement. Only this scalar changes; all mesh/world backing stays fixed.
 bool bindHumanLift(const mc_cold_sources50LA::Owner& source,float actualSelectedLift)noexcept {
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
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ground()?source_.get():nullptr;}
};
}
