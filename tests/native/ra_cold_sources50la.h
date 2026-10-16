#pragma once
// Isolated, unactivated RA source decoder bundle. No SessionAssets, world
// registration, roster, course capability, generated visuals or gameplay receipt.
#include "pod_materials50ed_fixture.h"
#include "track_height_cold50jb_fixture.h"
#include "../../src/assets/import_collision48g.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/track_collider50ao.h"
#include "../../src/assets/track_start50ad.h"
#include "../../src/assets/zone_search_authoring50cn.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/assets/impact_authoring49y.h"
#include "../../src/game/lap_progress50gf.h"
#include <algorithm>
#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>

namespace ra_static_registry50LA { class ColdStage; }
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace ra_cold_sources50LA {
namespace a=rr::assets;
namespace g=rr::game;
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr MemberPin sources[]{
 {"tracks\\ra.scb",122880,540795,"57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81"},
 {"tracks\\mra.spl",24748032,23820,"99ac08267db1a7fe3a161c46f1e941200390b68c9300bdc59d9e42608aecc52c"},
 {"tracks\\ara.spl",24723456,23404,"a074570af7e199dadab63fabdad37cfa4ea1d36d537ff5ef4003dacadfc397f3"},
 {"tracks\\ra.tun",29663232,922,"fb3e303604a27265585a498943d87099177d99f4701d1a75165d175e053044f8"}
};
inline constexpr const char* archiveSha="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2";
struct Diagnostic {
 enum class Stage {unused,archive,members,world,portals,height,collision,map,mapZones,mapLap,ai,aiZones,tuning,decodedSources,failed};
 Stage stage=Stage::unused,failedAt=Stage::unused;
 std::size_t placement=0;
 // A successful map parser can have committed effects to the actual serial
 // libm owner before a later failure. Do not retry as a fresh native load.
 bool mapGatesCommitted=false,aiGatesCommitted=false;
 a::DecodeError48C decode;
 g::TrackNavigationDiagnostic50EU navigation;
 g::LiveZoneStatus50EN portals=g::LiveZoneStatus50EN::InvalidInput;
 g::LapProgressDiagnostic50GF lap;
 a::AiAuthoringDiagnostic50IU ai;
 a::PodPhysicsDiagnostic49N impact;
 const char* failure=nullptr;
 std::string message; // Optional detail from existing parsers; fixed failure reporting allocates nothing.
};
struct CollisionOwner {
 a::CollisionMesh48I mesh;
 a::TrackCollider50AO instance;
 std::string member;
 std::uint32_t zone=0,ordinal=0;
};
struct DecodedSources {
 DecodedSources()=default;
 DecodedSources(const DecodedSources&)=delete;DecodedSources& operator=(const DecodedSources&)=delete;
 DecodedSources(DecodedSources&&)=delete;DecodedSources& operator=(DecodedSources&&)=delete;
 a::ResArchive48C archive;
 std::array<std::vector<std::uint8_t>,4> memberBytes;
 a::World48F world;
 // Stable, original source-order portal topology. These refs are the actual
 // 1-based SCB zone ordinals, not fabricated collider/Entity identities.
 std::array<g::LiveZonePortalInput50EN,10> authoredPortals{};
 std::array<g::CollectionRef50BX,10> portalHeads{};
 std::array<g::LiveZonePortalLink50EN,20> portalLinks{};
 std::array<g::PortalGeometry50EO,10> portalGeometry{};
 g::LiveZoneWorld50EN liveWorld;
 track_height_cold50JB::Owner height;
 std::vector<CollisionOwner> collisions;
 std::size_t collisionStackCount=1;
 g::TrackNavigation50EU map,ai;
 g::LapProgressTrack50GF mapLap;
 g::AiTrackMetadata50IU mapMetadata,aiMetadata;
 g::AiFreshCreateSelection50IU freshSelection;
 g::AiTrackTuning50IU aiTuning;
 g::StaticImpactTuning49Y aiImpact;
 g::DynamicImpactTuning50IW dynamicImpact;
 a::FinalImpactAuthoring49Y finalImpact{};
};
class Owner final {
 friend class ra_static_registry50LA::ColdStage;
 friend class ra_source_camera50LA::OwnershipBridge;
 std::unique_ptr<DecodedSources> data_;
 // Serial cold/session ownership only; not a cross-thread synchronization API.
 mutable bool liveWorldLeased_=false;
 class LiveWorldLease final {
  friend class Owner;
  friend class ra_static_registry50LA::ColdStage;
  friend class ra_source_camera50LA::OwnershipBridge;
  std::shared_ptr<const Owner> owner_;
  explicit LiveWorldLease(std::shared_ptr<const Owner> owner):owner_(std::move(owner)){
   owner_->liveWorldLeased_=true;
  }
  g::LiveZoneWorld50EN& world()const noexcept{return owner_->data_->liveWorld;}
 public:
  LiveWorldLease(const LiveWorldLease&)=delete;LiveWorldLease& operator=(const LiveWorldLease&)=delete;
  ~LiveWorldLease(){owner_->liveWorldLeased_=false;}
 };
 static std::shared_ptr<LiveWorldLease> leaseLiveWorld(std::shared_ptr<const Owner> owner){
  if(!owner||!owner->decoded_||owner->liveWorldLeased_)return {};
  const auto& s=*owner->data_;const auto& w=s.liveWorld;
  if(w.portalHeads!=s.portalHeads.data()||w.zoneCount!=s.portalHeads.size()||
     w.portalLinks!=s.portalLinks.data()||w.portalLinkCount!=s.portalLinks.size()||
     w.portals!=s.portalGeometry.data()||w.portalCount!=s.portalGeometry.size()||
     !s.map.indicesPrepared||!s.ai.indicesPrepared)return {};
  // If allocation of the shared control block throws, deletion of the new
  // lease restores exclusivity. Neither failure changes the portal cache.
  return std::shared_ptr<LiveWorldLease>(new LiveWorldLease(std::move(owner)));
 }
 bool attempted_=false,decoded_=false;
 static std::string lower(std::string value){for(auto& c:value)if(c>='A'&&c<='Z')c=char(c+32);return value;}
 static const a::ResourceEntry48C* find(const a::ResArchive48C& archive,const std::string& name){
  const auto key=lower(name);for(const auto& row:archive.entries)if(lower(row.name)==key)return &row;return nullptr;
 }
 static bool vector3(const a::ParameterBlock48F& block,const char* key,std::array<float,3>& out){
  const std::string* text=nullptr;
  for(const auto& f:block.fields)if(f.name==key){if(text)return false;text=&f.value;}
  if(!text)return false;
  std::size_t begin=0;
  for(unsigned i=0;i<3;++i){auto end=text->find(',',begin);if(end==std::string::npos)end=text->size();
   auto first=begin,last=end;while(first<last&&(*text)[first]==' ')++first;
   while(last>first&&(*text)[last-1]==' ')--last;
   if(!a::parseTrackNumber50AD(text->data()+first,last-first,out[i]))return false;
   if((i<2&&end==text->size())||(i==2&&end!=text->size()))return false;
   begin=end+1;
  }return true;
 }
 static bool predecessor(const g::TrackNavigation50EU& track,std::uint32_t expected){
  for(std::uint32_t i=0;i<track.track.count;++i){const auto& n=track.track.nodes[i];
   if(!n.nextCount||n.nextCount>2)return false;
   for(std::uint32_t j=0;j<n.nextCount;++j)if(n.next[j]>=track.track.count)return false;
  }
  std::uint32_t first=0;for(;first<track.track.count;++first)if(track.track.nodes[first].next[0]==0)break;
  return first==expected;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 // Caller supplies the exact original archive bytes and ACTUAL shared serial
 // numeric owner/category registry masks. This method reads no files and
 // never changes session/profile/course admission. One attempt per owner.
 bool decode(const std::vector<std::uint8_t>& original,g::SourceLibmState49T& actualLibm,
     a::TrackCollisionMasks50AN actualMasks,Diagnostic& d){
  if(attempted_){d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;d.failure="RA cold source owner already attempted";return false;}
  attempted_=true;d={};
  const auto fail=[&](const char* message){d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;d.failure=message;return false;};
  try {
   d.stage=Diagnostic::Stage::archive;
   if(original.size()!=4604561||pod_materials50ed_fixture::detail::sha256(original)!=archiveSha)
    return fail("exact original RA archive identity differs");
   data_=std::make_unique<DecodedSources>();auto& s=*data_;
   a::DecodeLimits48C limits;limits.resource_file_bytes=4604561;limits.resource_decoded_bytes=29687808;limits.resource_entries=1010;
   if(!a::decodeResource48C(original,s.archive,d.decode,nullptr,limits)||s.archive.entries.size()!=1010||s.archive.decoded.size()!=29687808)
    return fail("RA original resource framing differs");
   std::set<std::string> names;for(const auto& row:s.archive.entries)
    if(!names.insert(lower(row.name)).second)return fail("RA ambiguous source member identity");
   d.stage=Diagnostic::Stage::members;
   for(std::size_t i=0;i<4;++i){const auto& pin=sources[i];const auto* row=find(s.archive,pin.name);
    if(!row||row->logical_offset!=pin.offset||row->bytes!=pin.bytes||
       !a::extractResMember48C(s.archive,pin.name,s.memberBytes[i],d.decode)||
       s.memberBytes[i].size()!=pin.bytes||pod_materials50ed_fixture::detail::sha256(s.memberBytes[i])!=pin.sha)
     return fail("RA exact SCB/SPL/TUN member identity differs");
   }
   d.stage=Diagnostic::Stage::world;
   if(!a::decodeWorld48F(s.memberBytes[0],s.world,d.decode)||s.world.zones.size()!=10||
      s.world.portals.size()!=10||s.world.instances.size()!=1261)return fail("RA source world decode/count differs");
   for(std::size_t zone=0;zone<10;++zone){const auto selected=a::readZoneSearchSelection50CN(s.world,zone);
    if(selected.status!=a::ZoneSearchReadStatus50CN::Complete||
       selected.selection.collisionStatic118!=a::ZoneSearchKind50CN::List||
       selected.selection.collisionDynamic11C!=a::ZoneSearchKind50CN::List)return fail("RA source collision search is not List");
   }
   d.stage=Diagnostic::Stage::portals;
   for(std::size_t i=0;i<s.world.portals.size();++i){const auto& portal=s.world.portals[i];
    if(portal.parameter_block>=s.world.parameters.blocks.size())return fail("RA original portal block index rejected");
    auto& input=s.authoredPortals[i];input.zoneA=portal.zones[0];input.zoneB=portal.zones[1];
    const auto& block=s.world.parameters.blocks[portal.parameter_block];
    for(unsigned v=0;v<4;++v){std::array<float,3> vertex{};
     const auto key="vertex"+std::to_string(v+1);
     if(!vector3(block,key.c_str(),vertex))return fail("RA original portal vertex text rejected");
     for(unsigned lane=0;lane<3;++lane)input.vertices[v].v[lane]=vertex[lane];
    }
   }
   const g::LiveZoneWorldStorage50EN portalStorage{s.portalHeads.data(),s.portalHeads.size(),
       s.portalLinks.data(),s.portalLinks.size(),s.portalGeometry.data(),s.portalGeometry.size()};
   d.portals=g::prepareLiveZoneWorld50EN(s.authoredPortals.data(),s.authoredPortals.size(),
       s.world.zones.size(),portalStorage,s.liveWorld);
   if(d.portals!=g::LiveZoneStatus50EN::Complete)return fail("RA same-SCB portal topology rejected");
   d.stage=Diagnostic::Stage::height;
   if(!track_height_cold50JB::prepareFreshGameTrack(s.world,s.archive,sources[0].name,s.height,d.message)||
      s.height.sourceDomain.terrain330!=g::UndertrackHeightOwner50EW::Absent||
      s.height.sourceDomain.water334!=g::UndertrackHeightOwner50EW::Absent)
    return fail("RA fresh track height branch differs; no substitute provider admitted");
   d.stage=Diagnostic::Stage::collision;
   if(!actualMasks.staticObject||!actualMasks.floor||(actualMasks.staticObject&actualMasks.floor))
    return fail("actual collision registry masks unavailable");
   std::size_t placed=0,collisionOnly=0;
   for(const auto& instance:s.world.instances){const bool col=find(s.archive,instance.collision_member)!=nullptr;
    const bool geometry=find(s.archive,instance.geometry_member)!=nullptr;
    if(!col&&!geometry)return fail("RA placement lacks both original geometry and collision");
    placed+=col;collisionOnly+=col&&!geometry;
   }
   if(placed!=178||collisionOnly!=176)return fail("RA source collision placement closure differs");
   s.collisions.reserve(178);std::array<std::size_t,10> byZone{};std::size_t staticOnly=0,floor=0;
   for(std::size_t i=0;i<s.world.instances.size();++i){const auto& instance=s.world.instances[i];
    if(!find(s.archive,instance.collision_member))continue;d.placement=i;
    if(!instance.zone||instance.zone>10||instance.parameter_block>=s.world.parameters.blocks.size())return fail("RA placement index exceeds original world");
    s.collisions.emplace_back();auto& out=s.collisions.back();out.member=instance.collision_member;out.zone=instance.zone;out.ordinal=instance.ordinal;
    std::vector<std::uint8_t> bytes,cache;a::Collision48G originalCol;std::string why;
    if(!a::extractResMember48C(s.archive,out.member,bytes,d.decode)||!a::decodeCollision48G(bytes,originalCol,d.decode)||
       !a::encodeCollisionAsset48G(originalCol,cache,why)||!a::loadCollisionMesh48I(cache,out.mesh,why))
     return fail("RA original COL/native mesh decode rejected");
    std::array<float,3> p,look,up;const auto& block=s.world.parameters.blocks[instance.parameter_block];
    if(!vector3(block,"position",p)||!vector3(block,"lookvector",look)||!vector3(block,"upvector",up)||
       !a::prepareTrackCollider50AO(out.mesh,p,look,up,out.member.data(),out.member.size(),actualMasks,out.instance))
     return fail("RA original COL placement rejected");
    if(out.instance.category==actualMasks.staticObject)++staticOnly;
    else if(out.instance.category==(actualMasks.staticObject|actualMasks.floor))++floor;
    else return fail("RA collision source category differs");
    ++byZone[out.zone-1];s.collisionStackCount=std::max(s.collisionStackCount,out.mesh.nodes.size()+out.mesh.faces.size()+1);
   }
   if(byZone!=std::array<std::size_t,10>{{20,10,13,35,14,31,10,13,19,13}}||staticOnly!=109||floor!=69)
    return fail("RA placed collision source counts differ");
   auto scratch=std::make_unique<g::TrackNavigationScratch50EU>();
   for(std::size_t i=1;i<=2;++i){d.stage=i==1?Diagnostic::Stage::map:Diagnostic::Stage::ai;
    auto& navigation=i==1?s.map:s.ai;auto& metadata=i==1?s.mapMetadata:s.aiMetadata;const auto& bytes=s.memberBytes[i];
    if(!g::readTrackNavigation50EU(reinterpret_cast<const char*>(bytes.data()),bytes.size(),actualLibm,*scratch,navigation,d.navigation))
     return fail("RA original M/A navigation gate decode rejected");
    (i==1?d.mapGatesCommitted:d.aiGatesCommitted)=true;
    if(navigation.track.count!=(i==1?115u:113u)||!predecessor(navigation,i==1?103u:101u)||
       !a::readAiTrackMetadata50IU(reinterpret_cast<const char*>(bytes.data()),bytes.size(),navigation,metadata,d.ai))
     return fail("RA original M/A graph or AI metadata differs");
    d.stage=i==1?Diagnostic::Stage::mapZones:Diagnostic::Stage::aiZones;
    if(!g::bindTrackNavigationZones50EU(s.liveWorld,navigation,d.navigation))
     return fail("RA same-source navigation portal binding rejected");
    if(i==1){
     // Consume the real M parser scratch before the A parser overwrites it.
     // Zone binding mutates navigation indices only; source marker fieldMask
     // and ordered graph remain those of this exact authenticated M spline.
     d.stage=Diagnostic::Stage::mapLap;
     if(!g::prepareLapProgressTrack50GF(*scratch,s.mapLap,d.lap)||
        s.mapLap.nodeCount!=115||s.mapLap.predecessorOfZero!=103)
      return fail("RA original M scratch lap-track projection rejected");
    }
   }
   d.stage=Diagnostic::Stage::tuning;const auto& tuning=s.memberBytes[3];const auto* text=reinterpret_cast<const char*>(tuning.data());
   if(!a::readAIEnvironmentImpact49Y(text,tuning.size(),s.aiImpact,d.impact)||
      !a::readAiTrackTuning50IU(text,tuning.size(),s.aiTuning,d.ai)||
      !a::readDynamicImpactAuthoring50IW(text,tuning.size(),s.dynamicImpact,d.impact)||
      !a::readFinalImpactAuthoring49Y(text,tuning.size(),s.finalImpact,d.impact)||
      !g::bindFreshAiCreateSelection50IU("RA",2,s.freshSelection))return fail("RA actual track tuning/fresh source selection rejected");
   decoded_=true;d.stage=Diagnostic::Stage::decodedSources;return true;
  }catch(...){return fail("RA cold source allocation/decoder exception");}
 }
 // Only source-decoded CPU data, including same-SCB navigation indices and
 // genuine M-scratch lap-track markers. No per-racer lap state is created.
 // All topology spans borrow this nonmovable owner; consumers must not mutate
 // them or retain any view beyond this owner. No object registries exist here.
 const DecodedSources* sourcesDecoded()const noexcept{return decoded_?data_.get():nullptr;}
};
} // namespace ra_cold_sources50LA
