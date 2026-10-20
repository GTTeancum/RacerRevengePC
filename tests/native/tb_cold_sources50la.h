#pragma once
#include "pod_materials50ed_fixture.h"
#include "source_grid50ig_fixture.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/game/track_navigation_view50tb.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include <array>
#include <exception>
#include <memory>
#include <algorithm>
#include <limits>
#include <string>
#include <thread>

namespace tb_navigation50TB { class Owner; }
namespace tb_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace storage=g::navigation_storage_detail50TB;
inline constexpr std::size_t archiveBytes=3749557,decodedBytes=38240256,compactBytes=7111377;
inline constexpr std::uint32_t archiveEntries=1465,zoneCount=10,portalCount=11,mapNodeCount=270,aiNodeCount=269;
inline constexpr const char* archiveSha="1188206d7b1f64910c119192829fc357b9d4a9f021d64e8a2b7561f2c113a09b";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\tb.scb",196608,448107,"c3536c6e76dc4d6836ee154aabc80f5e73bd80e85189b841538b3a3ce53e8184"},
 {"tracks\\mtb.spl",25067520,57520,"160859d0b195cd0b60611a6720c0bccbf49c4e561886fd60c1123ceb638369c4"},
 {"tracks\\atb.spl",24993792,56589,"a32271a27aeaae45648f94dac7870a158fb6e7af5169763ddf0ccc7a18b1769f"},
 {"tracks\\tb.tob",0,106139,"12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc"},
 {"tracks\\tb.tun",38215680,977,"2cc0bf9d38f5c294fe0232452f5511b552022f02afcf1b457199f600b3f9ca8b"}
}};
enum class Phase {Empty,Authenticate,Archive,World,Tob,MapGates,MapMetadata,AiGates,AiMetadata,Tuning,Ready,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::string message;
 bool mapGatesCommitted=false,aiGatesCommitted=false;
 std::size_t retainedPayloadBytes=0,phaseReservationBytes=0,maximumReservationBytes=0;
 a::DecodeError48C decode{};g::TrackNavigationDiagnostic50EU navigation{};
 a::AiAuthoringDiagnostic50IU metadata{};
};
// A single authenticated cold identity; no aggregate back-reference.
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ::tb_navigation50TB::Owner;
 std::unique_ptr<a::IndexedResArchive48C> archive_;
 a::World48F world_;a::ParameterDocument48F tob_;
 std::shared_ptr<storage::Storage> navigation_;
 g::AiTrackTuning50IU tuning_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;Phase phase_=Phase::Empty;
 static void add(std::size_t& n,std::size_t x)noexcept {
  n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;
 }
 template<class T> static void vectorPayload(std::size_t& n,const std::vector<T>& v)noexcept {
  if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();
  else add(n,v.capacity()*sizeof(T));
 }
 static void textPayload(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity());add(n,1);}
 static void parametersPayload(std::size_t& n,const a::ParameterDocument48F& p)noexcept {
  vectorPayload(n,p.original_bytes);vectorPayload(n,p.blocks);
  for(const auto& b:p.blocks){textPayload(n,b.name);vectorPayload(n,b.fields);
   for(const auto& f:b.fields){textPayload(n,f.name);textPayload(n,f.value);}}
 }
 void requireThread()const noexcept{if(thread_!=std::this_thread::get_id())std::terminate();}
 bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
 bool reserve(std::size_t limit,std::size_t extra,Diagnostic& d)const noexcept {
  d.retainedPayloadBytes=retainedPayloadBytes();d.phaseReservationBytes=d.retainedPayloadBytes;add(d.phaseReservationBytes,extra);
  d.maximumReservationBytes=std::max(d.maximumReservationBytes,d.phaseReservationBytes);return d.phaseReservationBytes<=limit;
 }
 bool member(std::size_t i,std::vector<std::uint8_t>& out,Diagnostic& d)const {
  const auto& pin=sources[i];const a::ResourceEntry48C* match=nullptr;
  for(const auto& e:archive_->entries())if(a::OriginalStandardArchiveProvider50LA::sameName(e.name,pin.name)){
   if(match)return false;match=&e;}
  return match&&match->logical_offset==pin.offset&&match->bytes==pin.bytes&&
   archive_->extractMember(pin.name,out,d.decode,pin.bytes)&&out.size()==pin.bytes&&
   pod_materials50ed_fixture::detail::sha256(out)==pin.sha;
 }
 static bool vectorField(const a::ParameterBlock48F& block,const char* key,g::Vector3_48C& out){
  const std::string* value=nullptr;for(const auto& f:block.fields)if(f.name==key){if(value)return false;value=&f.value;}
  if(!value)return false;std::size_t begin=0;
  for(unsigned axis=0;axis<3;++axis){auto end=value->find(',',begin);
   if((axis<2&&end==std::string::npos)||(axis==2&&end!=std::string::npos))return false;
   if(end==std::string::npos)end=value->size();auto first=begin,last=end;
   while(first<last&&(*value)[first]==' ')++first;while(last>first&&(*value)[last-1]==' ')--last;
   if(!a::parseTrackNumber50AD(value->data()+first,last-first,out.v[axis]))return false;begin=end+1;}
  return begin==value->size()+1;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){requireThread();}
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_;}
 Phase phase()const noexcept{return thread_==std::this_thread::get_id()?phase_:Phase::Empty;}
 // Conservative source-specific phase reservation for the pinned native
 // parser/libstdc++ growth. It includes old/new vector allocations, string and
 // reversible-encode work, the World used-name tree and decimal temporaries.
 // Parent allocation observation must corroborate it before composition.
 static constexpr std::size_t parameterDecodeReservation(std::size_t bytes,std::size_t blocks,
     std::size_t fields,std::size_t instances)noexcept {
  return 3*blocks*sizeof(a::ParameterBlock48F)+3*fields*sizeof(a::ParameterField48F)+
   3*instances*sizeof(a::WorldInstance48F)+12*bytes+
   instances*(sizeof(std::string)+4*sizeof(void*)+256)+65536;
 }
 static constexpr std::size_t navigationPayloadBytes()noexcept{return sizeof(storage::Storage);}
 static constexpr std::size_t gateScratchBytes()noexcept{return sizeof(storage::Scratch<270>)+sizeof(storage::Metadata<270>)+270u;}
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;std::size_t n=sizeof(Owner);
  if(archive_)add(n,archive_->retainedPayloadBytes());
  parametersPayload(n,world_.parameters);vectorPayload(n,world_.zones);vectorPayload(n,world_.portals);
  for(const auto& p:world_.portals)textPayload(n,p.section);
  vectorPayload(n,world_.instances);for(const auto& i:world_.instances){textPayload(n,i.section);textPayload(n,i.geometry_member);textPayload(n,i.collision_member);}
  parametersPayload(n,tob_);if(navigation_)add(n,sizeof(storage::Storage));return n;
 }
 bool decodeOwned(std::vector<std::uint8_t>&& original,g::SourceLibmState49T& actualLibm,
     std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id()){d.message="TB source thread mismatch";return false;}
  if(attempted_)return fail(d,"TB cold source preparation is one-attempt");attempted_=true;phase_=Phase::Authenticate;
  try {
   if(original.size()!=archiveBytes)return fail(d,"TB original archive extent differs");
   if(original.capacity()>(std::numeric_limits<std::size_t>::max()-65536)/4||
      !reserve(availableCpuPayloadBytes,4*original.capacity()+65536,d))return fail(d,"TB authentication scratch exceeds existing payload budget");
   if(pod_materials50ed_fixture::detail::sha256(original)!=archiveSha)return fail(d,"TB original archive identity differs");
   phase_=Phase::Archive;archive_=std::make_unique<a::IndexedResArchive48C>();
   a::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   const auto owned=sizeof(Owner);if(availableCpuPayloadBytes<owned||
      !archive_->loadOwned(std::move(original),availableCpuPayloadBytes-owned,d.decode,nullptr,limits)||
      archive_->entries().size()!=archiveEntries||archive_->blocks().size()!=1556||
      archive_->decodedExtent()!=decodedBytes||archive_->compactBytes()!=compactBytes)return fail(d,"TB complete indexed original archive rejected");
   d.maximumReservationBytes=std::max(d.maximumReservationBytes,owned+archive_->preparationPeakPayloadBytes());
   // Exact existing full-name case rule, allocation-free duplicate check.
   for(std::size_t i=0;i<archiveEntries;++i)for(std::size_t j=0;j<i;++j)
    if(a::OriginalStandardArchiveProvider50LA::sameName(archive_->entries()[i].name,archive_->entries()[j].name))
     return fail(d,"TB ambiguous archive member identity");
   if(!reserve(availableCpuPayloadBytes,sizeof(storage::Storage),d))return fail(d,"TB exact navigation storage exceeds remaining budget");
   navigation_.reset(new storage::Storage(this));
   phase_=Phase::World;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[0].bytes,1238,6600,1112)+4*sources[0].bytes,d))return fail(d,"TB World parse phase reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[0].bytes;options.blocks=1238;options.fields_per_block=11;options.zones=10;options.portals=11;options.instances=1112;
    if(!member(0,raw,d)||!a::decodeWorld48F(raw,world_,d.decode,nullptr,options)||world_.zones.size()!=zoneCount||world_.portals.size()!=portalCount||world_.instances.size()!=1112)
     return fail(d,"TB authentic World rejected");
    for(std::size_t i=0;i<zoneCount;++i)if(world_.zones[i].zone!=i+1)return fail(d,"TB source zone order rejected");
    for(std::size_t i=0;i<portalCount;++i){
     const auto& p=world_.portals[i];if(p.parameter_block>=world_.parameters.blocks.size())return fail(d,"TB portal block invalid");
     auto& target=navigation_->authoredPortals[i];target.zoneA=p.zones[0];target.zoneB=p.zones[1];
     for(unsigned j=0;j<4;++j)if(!vectorField(world_.parameters.blocks[p.parameter_block],("vertex"+std::to_string(j+1)).c_str(),target.vertices[j]))return fail(d,"TB original portal binary32 vertex rejected");}}
   phase_=Phase::Tob;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[3].bytes,742,1687,0)+4*sources[3].bytes,d))return fail(d,"TB TOB parse reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[3].bytes;options.blocks=742;options.fields_per_block=13;options.retainDuplicateLabels=true;
    // Authentic TB alone: retain both ordered vertexcolor fields in each
    // sunflare1/2. No winning-value policy; strict SCB remains unchanged.
    if(!member(3,raw,d)||!a::decodeParameters48F(raw,tob_,d.decode,nullptr,options))return fail(d,"TB complete retained-label TOB rejected");}
   phase_=Phase::MapGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[1].bytes+gateScratchBytes(),d))return fail(d,"TB Map gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(1,raw,d))return fail(d,"TB original full Map spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<270>>();
     if(!g::navigation_detail50MB::read<270>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->map,d.navigation))return fail(d,"TB full270 Map gates rejected");
     d.mapGatesCommitted=true;
     if(navigation_->map.track.count!=270||!navigation_->map.gatesPrepared||navigation_->map.indicesPrepared)return fail(d,"TB exact Map spline extent rejected");
     std::copy(std::begin(scratch->fieldMask),std::end(scratch->fieldMask),navigation_->mapFieldMask.begin());
    }
    phase_=Phase::MapMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<270>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->map,navigation_->mapMetadata,d.metadata))return fail(d,"TB full Map metadata rejected");}
   phase_=Phase::AiGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[2].bytes+gateScratchBytes(),d))return fail(d,"TB Ai gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(2,raw,d))return fail(d,"TB original full Ai spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<269>>();
     if(!g::navigation_detail50MB::read<269>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->ai,d.navigation))return fail(d,"TB full269 Ai gates rejected");
     d.aiGatesCommitted=true;
     if(navigation_->ai.track.count!=269||!navigation_->ai.gatesPrepared||navigation_->ai.indicesPrepared)return fail(d,"TB exact Ai spline extent rejected");
    }
    phase_=Phase::AiMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<269>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->ai,navigation_->aiMetadata,d.metadata))return fail(d,"TB full Ai metadata rejected");}
   phase_=Phase::Tuning;
   {if(!reserve(availableCpuPayloadBytes,4*sources[4].bytes,d))return fail(d,"TB tuning reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(4,raw,d)||!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),tuning_,d.metadata))return fail(d,"TB source tuning rejected");}
   if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"TB retained source exceeds remaining budget");
   navigation_->gatesReady=true;ready_=true;phase_=Phase::Ready;d.phase=phase_;return true;
  }catch(const std::exception& e){return fail(d,e.what());}catch(...){return fail(d,"TB source failure; reached libm prefix retained");}
 }
 const a::IndexedResArchive48C& archive()const noexcept{requireThread();if(!archive_)std::terminate();return *archive_;}
 const a::World48F& world()const noexcept{requireThread();return world_;}
 const std::vector<std::uint8_t>& originalTob()const noexcept{requireThread();return tob_.original_bytes;}
 const a::ParameterDocument48F& triggerParameters()const noexcept{requireThread();return tob_;}
 const g::AiTrackTuning50IU& aiTuning()const noexcept{requireThread();return tuning_;}
 bool extractMember(const std::string& name,std::vector<std::uint8_t>& out,a::DecodeError48C& error,std::size_t maxBytes)const noexcept {
  if(!ready()){error={a::DecodeKind48C::unsupported,"tb.source_not_ready",0,"TB authentic cold source required"};return false;}
  return archive_->extractMember(name,out,error,maxBytes);
 }
 const std::array<std::uint32_t,270>& mapFieldMask() const noexcept {
  requireThread();static const std::array<std::uint32_t,270> empty{};return navigation_?navigation_->mapFieldMask:empty;
 }
 g::TrackNavigationView50TB map()const noexcept {
  auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50TB{std::move(self),navigation_,{},g::NavigationRole50TB::Map}:g::TrackNavigationView50TB{};
 }
 g::TrackNavigationView50TB ai()const noexcept {
  auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50TB{std::move(self),navigation_,{},g::NavigationRole50TB::Ai}:g::TrackNavigationView50TB{};
 }
 g::AiTrackMetadataView50TB mapMetadata()const noexcept{return g::AiTrackMetadataView50TB{map()};}
 g::AiTrackMetadataView50TB aiMetadata()const noexcept{return g::AiTrackMetadataView50TB{ai()};}
 bool prepareGrid(std::uint32_t rank,g::GridCenterResult50AC& out)const noexcept {
  return ready()&&source_grid50IG::detail::prepare(navigation_->map,mapNodeCount,rank,out);
 }
 // Read-only lookup of the existing authentic TB mount. An absent mount
 // succeeds with an empty result; an incompatible or foreign-thread mount
 // fails without changing the caller's result. No new namespace is published.
 static bool reuseCanonicalSource(const a::OriginalStandardArchiveProvider50LA& provider,
     std::shared_ptr<const Owner>& out,a::OriginalStandardArchiveProvider50LA::Diagnostic& d)noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  if(out||!provider.retainedSourcesAccessible())return Provider::fail(d,Provider::Status::input);
  for(std::size_t i=0;i<provider.mounts_.size();++i){const auto& mount=*provider.mounts_[i];
   if(!Provider::sameName(mount.name,"TRACKS/TB.RES"))continue;
   d.mount=i;const auto& source=mount.tbSource;
   if(!source||!source->ready()||mount.tbThread!=std::this_thread::get_id()||
      mount.digest!=archiveSha||mount.archiveBytes!=archiveBytes||mount.retainedBytes!=compactBytes||
      mount.backing||mount.ui||mount.rcSource||mount.rows.size()!=archiveEntries)
    return Provider::fail(d,Provider::Status::identity);
   for(std::size_t j=0;j<archiveEntries;++j){const auto member=source->archive_->member(j);const auto& row=mount.rows[j];
    d.ordinal=j;
    if(!member.identity||row.name!=member.identity->name||row.data!=member.data||
       row.size!=member.size||row.offset!=member.identity->logical_offset)
     return Provider::fail(d,Provider::Status::identity);
   }
   out=source;return true;
  }
  return true;
 }
 // Only this authenticated typed owner may introduce its complete compact
 // namespace. Provider holds this SAME source, so member pointers never expire.
 std::size_t canonicalMountReservationBytes()const noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;if(!ready())return 0;
  std::size_t n=sizeof(typename Provider::Mount)+2*Provider::maxMounts*sizeof(std::unique_ptr<const typename Provider::Mount>);
  add(n,archiveEntries*sizeof(typename Provider::Row));
  for(const auto& e:archive_->entries())add(n,2*std::max<std::size_t>(15,e.name.size())+1);
  add(n,256);return n;
 }
 bool mountCanonicalArchive(a::OriginalStandardArchiveProvider50LA& provider,std::size_t availableAdditionalPayloadBytes,
     a::OriginalStandardArchiveProvider50LA::Diagnostic& d)const noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  auto self=weak_from_this().lock();if(!ready()||!self)return Provider::fail(d,Provider::Status::identity);
  // A verified same-source mount already owns every descriptor/member. This
  // read-only path preserves the actual seal/revision; caller still performs
  // its normal seal and resolver bind. A different authentic owner rejects.
  std::shared_ptr<const Owner> existing;
  if(!reuseCanonicalSource(provider,existing,d))return false;
  if(existing){
   if(existing.get()!=this)return Provider::fail(d,Provider::Status::identity);
   d={};return true;
  }
  if(canonicalMountReservationBytes()>availableAdditionalPayloadBytes)return Provider::fail(d,Provider::Status::capacity);
  try {auto next=std::make_unique<Provider::Mount>();next->name="TRACKS/TB.RES";next->digest=archiveSha;next->archiveBytes=archiveBytes;
   next->tbSource=std::move(self);next->tbThread=thread_;next->retainedBytes=compactBytes;next->rows.reserve(archiveEntries);
   for(std::size_t i=0;i<archiveEntries;++i){const auto m=archive_->member(i);d.ordinal=i;
    if(!m.identity||!m.data||m.size!=m.identity->bytes||!Provider::nameOkay(m.identity->name))return Provider::fail(d,Provider::Status::identity);
    next->rows.push_back({m.identity->name,m.data,m.size,m.identity->logical_offset});}
   return provider.commit(std::move(next),d);
  }catch(...){return Provider::fail(d,Provider::Status::allocation);}
 }
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id()||weak_from_this().use_count()>1||(navigation_&&navigation_.use_count()!=1))return false;
  ready_=false;navigation_.reset();archive_.reset();world_={};tob_={};tuning_={};attempted_=true;phase_=Phase::Closed;return true;
 }
};
}
