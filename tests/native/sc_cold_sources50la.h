#pragma once
// Isolated SC source-only preparation. Compile-visible, never instantiated by runtime.
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace sc_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t archiveBytes=2974353,decodedBytes=26714112;
inline constexpr std::uint32_t archiveEntries=984,zoneCount=30,mapNodeCount=179,aiNodeCount=180;
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
 std::array<std::uint32_t,zoneCount> sourceZoneOrdinals_{};
 bool authenticated_=false;
 ZeroSkyReceipt()=default;
public:
 ZeroSkyReceipt(const ZeroSkyReceipt&)=delete;ZeroSkyReceipt& operator=(const ZeroSkyReceipt&)=delete;
 // Read-only source ordinal namespace, NOT native visibility/Entity refs.
 const std::array<std::uint32_t,zoneCount>& sourceZoneOrdinals()const noexcept{return sourceZoneOrdinals_;}
 bool authoredZeroAndArchiveHasNoCube()const noexcept{return authenticated_;}
};
struct DecodedSources final {
 a::ResArchive48C archive;
 std::array<std::vector<std::uint8_t>,sources.size()> memberBytes;
 a::World48F world;
 a::ParameterDocument48F triggerParameters;
 g::TrackNavigation50EU map,ai;
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
 a::AiAuthoringDiagnostic50IU ai;
};
class Owner final {
 std::unique_ptr<DecodedSources> data_;
 ZeroSkyReceipt zeroSky_;
 bool attempted_=false,decoded_=false;
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
 // No collision geometry, portal index binding, registry, racer or renderer.
 bool decode(const std::vector<std::uint8_t>& original,g::SourceLibmState49T& actualLibm,Diagnostic& d){
  if(attempted_){d.failure="SC source owner already attempted";d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;return false;}
  attempted_=true;d={};
  const auto fail=[&](const char* reason){d.failedAt=d.stage;d.stage=Diagnostic::Stage::failed;d.failure=reason;return false;};
  try{
   d.stage=Diagnostic::Stage::archive;
   if(original.size()!=archiveBytes||pod_materials50ed_fixture::detail::sha256(original)!=archiveSha)return fail("SC original archive identity");
   data_=std::make_unique<DecodedSources>();auto& s=*data_;
   a::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   if(!a::decodeResource48C(original,s.archive,d.decode,nullptr,limits)||s.archive.entries.size()!=archiveEntries||s.archive.decoded.size()!=decodedBytes)
    return fail("SC original archive framing");
   std::set<std::string> names;
   for(const auto& row:s.archive.entries)if(!names.insert(canonical(row.name)).second)return fail("SC ambiguous member identity");
   d.stage=Diagnostic::Stage::members;
   for(std::size_t i=0;i<sources.size();++i){const auto& pin=sources[i];const a::ResourceEntry48C* entry=nullptr;
    for(const auto& row:s.archive.entries)if(canonical(row.name)==canonical(pin.name))entry=&row;
    if(!entry||entry->logical_offset!=pin.offset||entry->bytes!=pin.bytes||
       !a::extractResMember48C(s.archive,pin.name,s.memberBytes[i],d.decode,pin.bytes)||
       s.memberBytes[i].size()!=pin.bytes||pod_materials50ed_fixture::detail::sha256(s.memberBytes[i])!=pin.sha)
     return fail("SC original member pin");
   }
   d.stage=Diagnostic::Stage::world;
   if(!a::decodeWorld48F(s.memberBytes[0],s.world,d.decode)||s.world.zones.size()!=zoneCount||s.world.portals.size()!=34||s.world.instances.size()!=4107)
    return fail("SC original world counts");
   d.stage=Diagnostic::Stage::zeroSky;
   const auto* environment=block(s.world.parameters,"environment");
   const auto* count=environment?field(*environment,"numberofskycubes"):nullptr;
   if(!count||*count!="0")return fail("SC requires explicit authored zero sky cubes");
   for(const auto& f:environment->fields)if(f.name.rfind("cubefile",0)==0)return fail("SC unexpected cube reference");
   for(const auto& name:names)if(name.size()>=4&&name.compare(name.size()-4,4,".cub")==0)return fail("SC unexpected CUB member");
   for(std::size_t i=0;i<zoneCount;++i){const auto& zone=s.world.zones[i];
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
    if(navigation.track.count!=(i==1?mapNodeCount:aiNodeCount)||!navigation.gatesPrepared||navigation.indicesPrepared||
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
};
} // namespace sc_cold_sources50LA
