#pragma once
#include "sc_cold_sources50la.h"
#include "authored_runtime_ground50cp_fixture.h"
#include <memory>
#include <stdexcept>

// SC-only same-decoded-source adapter. Every COL placement remains separately
// owned and source ordered. This performs no archive/SCB or navigation-gate read.
namespace sc_runtime_ground50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace original=authored_runtime_ground50cp_fixture;
class Owner final {
 std::shared_ptr<const sc_cold_sources50LA::Owner> source_;
 std::unique_ptr<original::ColdScene> scene_;
 bool attempted_=false,humanLiftBound_=false;
 static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
 static std::array<float,3> vector(const a::ParameterBlock48F& block,const char* key){
  const std::string* text=nullptr;
  for(const auto& field:block.fields)if(field.name==key){need(!text,"SC duplicate placement vector");text=&field.value;}
  need(text,"SC missing placement vector");std::array<float,3> out{};std::size_t begin=0;
  for(unsigned axis=0;axis<3;++axis){auto end=text->find(',',begin);
   need((end==std::string::npos)==(axis==2),"SC placement vector extent");
   if(end==std::string::npos)end=text->size();auto first=begin,last=end;
   while(first<last&&(*text)[first]==' ')++first;while(last>first&&(*text)[last-1]==' ')--last;
   need(a::parseTrackNumber50AD(text->data()+first,last-first,out[axis]),"SC placement numeric source");begin=end+1;
  }return out;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool prepare(std::shared_ptr<const sc_cold_sources50LA::Owner> source,
     a::TrackCollisionMasks50AN actualMasks,const std::vector<std::uint8_t>& actualSelectedPhy,
     unsigned actualGridRank,std::string& error)noexcept {
  if(attempted_){error="SC ground owner already attempted";return false;}attempted_=true;
  try{
   const auto* data=source?source->sourcesDecoded():nullptr;
   // Counts are the cold owner's own (read from the data, bounded there).
   need(data&&source->zeroSkyReceipt()&&data->world.zones.size()==source->zeroSkyReceipt()->zoneCount()&&
       data->map.gatesPrepared&&data->map.track.count&&data->ai.gatesPrepared&&data->ai.track.count,
       "SC ground requires the decoded source owner");
   auto next=std::make_unique<original::ColdScene>();next->world=data->world;
   next->stackCount=1;next->audit=nlohmann::json::array();
   for(std::size_t zone=0;zone<next->world.zones.size();++zone){
    const auto selection=a::readZoneSearchSelection50CN(next->world,zone);
    need(selection.status==a::ZoneSearchReadStatus50CN::Complete&&
        selection.selection.collisionStatic118==a::ZoneSearchKind50CN::List&&
        selection.selection.collisionDynamic11C==a::ZoneSearchKind50CN::List,
        // FLAG stand-in: the native ground walks List searches only; another
        // authored method is refused here until the common path grows it.
        "SC zone collision search method other than List is not supported natively");
   }
   std::size_t count=0;for(const auto& instance:next->world.instances)
    if(original::hasCollisionMember50IG(data->archive,instance.collision_member))++count;
   // Upper limit, not the stock 252: the native owner store's real capacity.
   course_archive_receipt50CI::countReceipt("sc_runtime_ground","placed_col",count,252);
   need(count&&count<=original::hardCollisionOwners50IG,"SC placed COL count exceeds the native owner store");
   // A single cold reservation precedes every mesh/name borrow. No subsequent
   // resize, sorting or deduplication is allowed after publication.
   next->owners.reserve(count);a::DecodeError48C decode;
   for(const auto& instance:next->world.instances){
    if(!original::hasCollisionMember50IG(data->archive,instance.collision_member))continue;
    need(next->owners.size()<count&&instance.parameter_block<next->world.parameters.blocks.size(),"SC actual COL source row");
    next->owners.emplace_back();auto& out=next->owners.back();
    out.name=instance.collision_member;out.zone=instance.zone;out.ordinal=instance.ordinal;
    std::vector<std::uint8_t> bytes,cache;a::Collision48G col;std::string why;
    need(a::extractResMember48C(data->archive,out.name,bytes,decode)&&a::decodeCollision48G(bytes,col,decode),"SC actual placed COL decode");
    need(a::encodeCollisionAsset48G(col,cache,why)&&a::loadCollisionMesh48I(cache,out.mesh,why),"SC native COL mesh preparation");
    const auto& block=next->world.parameters.blocks[instance.parameter_block];
    const auto position=vector(block,"position"),look=vector(block,"lookvector"),up=vector(block,"upvector");
    need(a::prepareTrackCollider50AO(out.mesh,position,look,up,out.name.data(),out.name.size(),actualMasks,out.instance),
        "SC actual COL placement/category preparation");
    next->stackCount=std::max(next->stackCount,out.mesh.nodes.size()+out.mesh.faces.size()+1);
   }
   need(next->owners.size()==count,"SC COL source order/extent changed");
   std::string heightError;
   need(track_height_cold50JB::prepareFreshGameTrack(next->world,data->archive,"tracks\\sc.scb",next->height,heightError),
       "SC actual fresh height descriptor rejected");
   // Authenticated SC has absent WaterFile. Retain the real fresh-game owner
   // result; no fabricated provider/terrain activation or generic fallback.
   need(next->height.sourceDomain.water334==g::UndertrackHeightOwner50EW::Absent&&
       next->height.sourceDomain.terrain330==g::UndertrackHeightOwner50EW::Absent&&
       !next->height.waterNativeQueryBound,
       // FLAG stand-in: the SC loader has no water/terrain-height provider yet
       // (GC/MC own one); a course authoring WaterFile is refused here.
       "SC course authors water or terrain height: provider not wired on this loader");
   a::TrackStartScratch50AD startScratch{};a::TrackStart50AD start{};a::TrackStartDiagnostic50AD startDiagnostic{};
   const auto& map=data->memberBytes[1];
   need(a::readTrackStart50AD(reinterpret_cast<const char*>(map.data()),map.size(),actualGridRank,startScratch,start,startDiagnostic)&&
       start.nodeCount==data->map.track.count,"SC genuine selected grid projection");
   a::VisualAuthoredValues49R visual{};a::VisualAuthoringDiagnostic49R visualDiagnostic{};
   need(a::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(actualSelectedPhy.data()),actualSelectedPhy.size(),visual,visualDiagnostic),
       "SC actual selected-racer PHY");
   next->lift=visual.values[static_cast<unsigned>(a::VisualAuthoredKey49R::MaxLiftHeight)];
   need(g::prepareCenterPose50AC(start.grid,next->lift,{{0,1,0}},next->pose),"SC actual selected grid center pose");
   source_=std::move(source);scene_=std::move(next);error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){try{error="SC ground allocation or source preparation failed";}catch(...){}return false;}
 }
 // Called once from the real racer0 rig/class installation, before grounded
 // grid placement. Only this scalar changes; all mesh/world backing stays fixed.
 bool bindHumanLift(const sc_cold_sources50LA::Owner& source,float actualSelectedLift)noexcept {
  if(!scene_||source_.get()!=&source||humanLiftBound_||!std::isfinite(actualSelectedLift))return false;
  scene_->lift=actualSelectedLift;humanLiftBound_=true;return true;
 }
 const original::ColdScene* ground()const noexcept{return scene_.get();}
 const sc_cold_sources50LA::Owner* coldSource()const noexcept{return scene_?source_.get():nullptr;}
};
}
