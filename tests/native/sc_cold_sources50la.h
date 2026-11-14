#pragma once
// Isolated SC source-only preparation. Compile-visible, never instantiated by runtime.
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "track_runtime_storage50ig_fixture.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/game/lap_progress50gf.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace sc_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
// Stock identity and counts: RECEIPTS only (course_archive_receipt50ci.h). The
// original reads whatever SC.RES holds; counts come from the data, bounded by
// the native storage they land in (zones: hardZones; spline nodes: the
// navigation storage, maxUndertrackNodes50ET).
inline constexpr std::size_t archiveBytes=2974353,decodedBytes=26714112;
inline constexpr std::uint32_t archiveEntries=984,zoneCount=30,mapNodeCount=179,aiNodeCount=180;
inline constexpr std::size_t maxZones=track_runtime_storage50IG::hardZones;
inline constexpr std::size_t maxNavigationNodes=rr::game::maxUndertrackNodes50ET;
inline constexpr const char* archiveSha="8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\sc.scb",172032,1651653,"784fb4f60a52ec23ee4c4f7082dd4fb44393421c5336b56f03d49b73f6b8e1ee"},
 {"tracks\\msc.spl",21749760,37633,"958643875d7d228c933cd7cb680dd8e92ab1921dae373f9e007abd19669f4482"},
 {"tracks\\asc.spl",21700608,38045,"50b006304e5defeabf3a41ea8eca4a8143a8070562883aff35086c311daecbb5"},
 {"tracks\\sc.tob",0,64047,"473351310a098008a477f9e72a6fc7e03d8bb808adcd79f5d810f90aa332b727"},
 {"tracks\\sc.tun",26689536,978,"5c0f70f5854a653e62ef6a70a8159fba21e12e36612664289c1b0e3164ba5f8c"}
}};
// Static source fact only, NOT AiFreshCreateSelection or a profile receipt.
// SLUS c1f1b63e..6ce6c30:17EEC0 loads16;17EEC8 stores profile+11310.
// Descriptor11308 =11278+12*12 contains actual "sc". See contract for words.
inline constexpr std::uint32_t originalDescriptorRecordPod=16;
class Owner;
class ZeroSkyReceipt final {
 friend class Owner;
 std::array<std::uint32_t,maxZones> sourceZoneOrdinals_{};
 std::size_t zones_=0;std::uint32_t skyCubes_=0;
 bool authenticated_=false;
 ZeroSkyReceipt()=default;
public:
 ZeroSkyReceipt(const ZeroSkyReceipt&)=delete;ZeroSkyReceipt& operator=(const ZeroSkyReceipt&)=delete;
 // Read-only source ordinal namespace, NOT native visibility/Entity refs.
 // Live prefix [0,zoneCount()) of a hardZones-sized store.
 const std::array<std::uint32_t,maxZones>& sourceZoneOrdinals()const noexcept{return sourceZoneOrdinals_;}
 std::size_t zoneCount()const noexcept{return zones_;}
 // The data branch the original takes: [Environment].NumberOfSkyCubes.
 std::uint32_t authoredSkyCubes()const noexcept{return skyCubes_;}
 bool authoredZeroAndArchiveHasNoCube()const noexcept{return authenticated_&&skyCubes_==0;}
};
struct DecodedSources final {
 a::ResArchive48C archive;
 std::array<std::vector<std::uint8_t>,sources.size()> memberBytes;
 a::World48F world;
 a::ParameterDocument48F triggerParameters;
 g::TrackNavigation50EU map,ai;
 g::LapProgressTrack50GF mapLap;
 g::AiTrackMetadata50IU mapMetadata,aiMetadata;
 g::AiTrackTuning50IU aiTuning;
 DecodedSources()=default;
 DecodedSources(const DecodedSources&)=delete;DecodedSources& operator=(const DecodedSources&)=delete;
 DecodedSources(DecodedSources&&)=delete;DecodedSources& operator=(DecodedSources&&)=delete;
};
struct Diagnostic {
 enum class Stage {unused,archive,members,world,zeroSky,triggers,map,ai,tuning,decoded,failed};
 Stage stage=Stage::unused,failedAt=Stage::unused;
 bool mapGatesCommitted=false,aiGatesCommitted=false;
 const char* failure=nullptr;
 a::DecodeError48C decode;
 g::TrackNavigationDiagnostic50EU navigation;
 g::LapProgressDiagnostic50GF lap;
 a::AiAuthoringDiagnostic50IU ai;
};
class Owner final {
 std::unique_ptr<DecodedSources> data_;
 ZeroSkyReceipt zeroSky_;
 bool attempted_=false,decoded_=false,atm_=false;
 static std::string canonical(std::string s){for(auto& c:s){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c+32);}return s;}
 static const a::ParameterBlock48F* block(const a::ParameterDocument48F& p,const char* name){
  const a::ParameterBlock48F* found=nullptr;
  for(const auto& b:p.blocks)if(b.name==name){if(found)return nullptr;found=&b;}return found;
 }
 static const std::string* field(const a::ParameterBlock48F& b,const char* name){
  const std::string* found=nullptr;
  for(const auto& f:b.fields)if(f.name==name){if(found)return nullptr;found=&f.value;}return found;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 // Caller supplies authentic bytes and its actual serial numeric owner.
 // Later failure can retain earlier M-parser libm effects: one attempt only.
 // Retains MAP lap markers only; no collision geometry, portal index binding,
 // registry, racer state or renderer.
 bool decode(const std::vector<std::uint8_t>& original,g::SourceLibmState49T& actualLibm,Diagnostic& d){
  if(attempted_){d.failure="SC source owner already attempted";d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;return false;}
  attempted_=true;d={};
  const auto fail=[&](const char* reason){d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;d.failure=reason;return false;};
  try{
   d.stage=Diagnostic::Stage::archive;
   course_archive_receipt50CI::receipt("sc_cold_sources","SC",original,archiveSha,archiveBytes);
   data_=std::make_unique<DecodedSources>();auto& s=*data_;
   if(!a::decodeResource48C(original,s.archive,d.decode,nullptr,course_archive_receipt50CI::limits()))
    return fail("SC archive framing exceeds the shared decode capacity");
   course_archive_receipt50CI::countReceipt("sc_cold_sources","entries",s.archive.entries.size(),archiveEntries);
   std::set<std::string> names;
   for(const auto& row:s.archive.entries)if(!names.insert(canonical(row.name)).second)return fail("SC ambiguous member identity");
   // Data branch for the animated-texture owner: the archive authors ATM
   // textures or it does not (FLAG: keyed on member presence; the original
   // loads an ATM when a material names it).
   for(const auto& name:names)if(name.size()>=4&&name.compare(name.size()-4,4,".atm")==0)atm_=true;
   d.stage=Diagnostic::Stage::members;
   for(std::size_t i=0;i<sources.size();++i){const auto& pin=sources[i];const a::ResourceEntry48C* entry=nullptr;
    for(const auto& row:s.archive.entries)if(canonical(row.name)==canonical(pin.name))entry=&row;
    // Found by name (as the original's named member reads); the stock
    // offset/size/SHA is a receipt.
    if(!entry||!a::extractResMember48C(s.archive,entry->name,s.memberBytes[i],d.decode,entry->bytes)||
       s.memberBytes[i].size()!=entry->bytes)
     return fail("SC required member absent or unreadable");
    course_archive_receipt50CI::memberReceipt("sc_cold_sources",pin.name,s.memberBytes[i],pin.offset,entry->logical_offset,pin.bytes,pin.sha);
   }
   d.stage=Diagnostic::Stage::world;
   if(!a::decodeWorld48F(s.memberBytes[0],s.world,d.decode))return fail("SC world rejected");
   if(s.world.zones.empty()||s.world.zones.size()>maxZones||s.world.portals.size()>track_runtime_storage50IG::hardPortals)
    return fail("SC world zones/portals exceed native storage");
   course_archive_receipt50CI::countReceipt("sc_cold_sources","zones",s.world.zones.size(),zoneCount);
   course_archive_receipt50CI::countReceipt("sc_cold_sources","portals",s.world.portals.size(),34);
   course_archive_receipt50CI::countReceipt("sc_cold_sources","instances",s.world.instances.size(),4107);
   d.stage=Diagnostic::Stage::zeroSky;
   const auto* environment=block(s.world.parameters,"environment");
   const auto* count=environment?field(*environment,"numberofskycubes"):nullptr;
   // Branch on the data the original reads. An absent key takes the
   // reader's default 1. Zero cubes: no cube reference or CUB member may exist.
   unsigned long cubes=1;
   if(count){char* end=nullptr;cubes=std::strtoul(count->c_str(),&end,10);
    if(count->empty()||!end||*end||cubes>64)return fail("SC NumberOfSkyCubes unreadable");}
   if(cubes==0){
    for(const auto& f:environment->fields)if(f.name.rfind("cubefile",0)==0)return fail("SC zero sky cubes but a cube reference");
    for(const auto& name:names)if(name.size()>=4&&name.compare(name.size()-4,4,".cub")==0)return fail("SC zero sky cubes but a CUB member");
   }
   zeroSky_.skyCubes_=std::uint32_t(cubes);zeroSky_.zones_=s.world.zones.size();
   if(cubes)std::cerr<<"sc_cold_sources sky_cubes="<<cubes<<" (zero-sky owner not used)\n";
   for(std::size_t i=0;i<s.world.zones.size();++i){const auto& zone=s.world.zones[i];
    if(zone.zone!=i+1||zone.parameter_block>=s.world.parameters.blocks.size())return fail("SC actual zone ordinal coverage");
    zeroSky_.sourceZoneOrdinals_[i]=zone.zone;
   }
   d.stage=Diagnostic::Stage::triggers;
   if(!a::decodeParameters48F(s.memberBytes[3],s.triggerParameters,d.decode))return fail("SC TOB source parameters");
   auto scratch=std::make_unique<g::TrackNavigationScratch50EU>();
   for(std::size_t i=1;i<=2;++i){d.stage=i==1?Diagnostic::Stage::map:Diagnostic::Stage::ai;
    auto& navigation=i==1?s.map:s.ai;auto& metadata=i==1?s.mapMetadata:s.aiMetadata;const auto& bytes=s.memberBytes[i];
    if(!g::readTrackNavigation50EU(reinterpret_cast<const char*>(bytes.data()),bytes.size(),actualLibm,*scratch,navigation,d.navigation))
     return fail("SC independent source spline gates");
    (i==1?d.mapGatesCommitted:d.aiGatesCommitted)=true;
    // Capture genuine MAP ProgressNode-presence data before the AI read reuses
    // this same scratch. No repeated gate parse or serial libm execution.
    if(i==1&&!g::prepareLapProgressTrack50GF(*scratch,s.mapLap,d.lap))
     return fail("SC original map lap markers");
    course_archive_receipt50CI::countReceipt("sc_cold_sources",i==1?"map_nodes":"ai_nodes",navigation.track.count,i==1?mapNodeCount:aiNodeCount);
    if(!navigation.track.count||navigation.track.count>maxNavigationNodes||!navigation.gatesPrepared||navigation.indicesPrepared||
       !a::readAiTrackMetadata50IU(reinterpret_cast<const char*>(bytes.data()),bytes.size(),navigation,metadata,d.ai))
     return fail("SC source spline count or metadata");
   }
   d.stage=Diagnostic::Stage::tuning;const auto& tun=s.memberBytes[4];
   if(!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(tun.data()),tun.size(),s.aiTuning,d.ai))return fail("SC original AI tuning");
   // Publish both receipts only after every admitted source stage succeeds.
   zeroSky_.authenticated_=true;decoded_=true;d.stage=Diagnostic::Stage::decoded;return true;
  }catch(...){return fail("SC cold source allocation or decoder exception; private prefix retained");}
 }
 // Borrowed const views expire with this nonmovable owner. Raw fields retain
 // authored absence. Gate preparation is NOT zone binding or race readiness.
 const DecodedSources* sourcesDecoded()const noexcept{return decoded_?data_.get():nullptr;}
 const ZeroSkyReceipt* zeroSkyReceipt()const noexcept{return decoded_?&zeroSky_:nullptr;}
 bool authorsAnimatedTextures()const noexcept{return decoded_&&atm_;}
};
} // namespace sc_cold_sources50LA
