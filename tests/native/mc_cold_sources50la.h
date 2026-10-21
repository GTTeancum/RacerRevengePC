#pragma once
#include "pod_materials50ed_fixture.h"
#include "mc_world_semantics50mc.h"
#include "source_grid50ig_fixture.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/game/track_navigation_view50mc.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include <array>
#include <exception>
#include <memory>
#include <algorithm>
#include <limits>
#include <string>
#include <thread>

namespace mc_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace storage=g::navigation_storage_detail50MC;
using NavigationStorage=storage::Storage;
inline constexpr std::size_t archiveBytes=3822171,decodedBytes=14303232,compactBytes=6620284;
inline constexpr std::uint32_t archiveEntries=432,zoneCount=28,portalCount=28,mapNodeCount=173,aiNodeCount=173;
inline constexpr const char* archiveSha="f8854752f639676257d44e33479a0c31c047a17e4cd592d02f483ad593179c81";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\mc.scb",270336,187711,"26f5d637533ee9840e916e02d5cef7b06d68352f1b7e0b11a48bf050d73df51e"},
 {"tracks\\mmc.spl",10149888,36276,"6eff5357a4cee85b4fc9a325957de2bae34526de48503def66f35502b0d709fe"},
 {"tracks\\amc.spl",10100736,36187,"5b6f402b3b9516f33e74b2b7dfd46ec54fced99b570093445e3f3537be313d03"},
 {"tracks\\mc.tob",0,186642,"7070700a884c24a98a0a89d4ea58e61e34f9a89e19af3ca239779c56c0dd14b0"},
 {"tracks\\mc.tun",14278656,979,"3d72bc87be7327d79fa4e9214235d2a3deafa901be35fb84a73ccf4cbc01e2cb"}
}};
enum class Phase {Empty,Authenticate,Archive,World,Tob,MapGates,MapMetadata,AiGates,AiMetadata,Tuning,DocumentsReady,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::string message;
 bool mapGatesCommitted=false,aiGatesCommitted=false;
 std::size_t retainedPayloadBytes=0,phaseReservationBytes=0,maximumReservationBytes=0;
 a::DecodeError48C decode{};g::TrackNavigationDiagnostic50EU navigation{};
 a::AiAuthoringDiagnostic50IU metadata{};
};
// A single authenticated cold identity; no aggregate back-reference.
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ::mc_navigation50MC::Owner;
 std::unique_ptr<a::IndexedResArchive48C> archive_;
 a::World48F world_;a::ParameterDocument48F tob_;
 std::shared_ptr<NavigationStorage> navigation_;
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

public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){requireThread();}
 bool ready()const noexcept{return documentsReady();}
 const a::World48F& world()const noexcept{requireThread();return world_;}
 bool documentsReady()const noexcept{return thread_==std::this_thread::get_id()&&ready_;}
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
 static constexpr std::size_t navigationPayloadBytes()noexcept{return sizeof(NavigationStorage);}
 static constexpr std::size_t gateScratchBytes()noexcept{return sizeof(storage::Scratch<173>)+sizeof(storage::Metadata<173>)+173u;}
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;std::size_t n=sizeof(Owner);
  if(archive_)add(n,archive_->retainedPayloadBytes());
  parametersPayload(n,world_.parameters);vectorPayload(n,world_.zones);vectorPayload(n,world_.portals);
  for(const auto& p:world_.portals)textPayload(n,p.section);
  vectorPayload(n,world_.instances);for(const auto& i:world_.instances){textPayload(n,i.section);textPayload(n,i.geometry_member);textPayload(n,i.collision_member);}
  parametersPayload(n,tob_);if(navigation_)add(n,sizeof(NavigationStorage));return n;
 }
 bool decodeOwned(std::vector<std::uint8_t>&& original,g::SourceLibmState49T& actualLibm,
     std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id()){d.message="MC source thread mismatch";return false;}
  if(attempted_)return fail(d,"MC cold source preparation is one-attempt");attempted_=true;phase_=Phase::Authenticate;
  try {
   if(!availableCpuPayloadBytes||availableCpuPayloadBytes>32u*1024u*1024u)return fail(d,"MC source requires the unchanged existing32MiB allowance");
   if(original.size()!=archiveBytes)return fail(d,"MC original archive extent differs");
   if(original.capacity()>(std::numeric_limits<std::size_t>::max()-65536)/4||
      !reserve(availableCpuPayloadBytes,4*original.capacity()+65536,d))return fail(d,"MC authentication scratch exceeds existing payload budget");
   if(pod_materials50ed_fixture::detail::sha256(original)!=archiveSha)return fail(d,"MC original archive identity differs");
   phase_=Phase::Archive;archive_=std::make_unique<a::IndexedResArchive48C>();
   a::DecodeLimits48C limits;limits.resource_file_bytes=archiveBytes;limits.resource_decoded_bytes=decodedBytes;limits.resource_entries=archiveEntries;
   const auto owned=sizeof(Owner);if(availableCpuPayloadBytes<owned||
      !archive_->loadOwned(std::move(original),availableCpuPayloadBytes-owned,d.decode,nullptr,limits)||
      archive_->entries().size()!=archiveEntries||archive_->blocks().size()!=582||
      archive_->decodedExtent()!=decodedBytes||archive_->compactBytes()!=compactBytes)return fail(d,"MC complete indexed original archive rejected");
   d.maximumReservationBytes=std::max(d.maximumReservationBytes,owned+archive_->preparationPeakPayloadBytes());
   // Exact existing full-name case rule, allocation-free duplicate check.
   for(std::size_t i=0;i<archiveEntries;++i)for(std::size_t j=0;j<i;++j)
    if(a::OriginalStandardArchiveProvider50LA::sameName(archive_->entries()[i].name,archive_->entries()[j].name))
     return fail(d,"MC ambiguous archive member identity");
   if(!reserve(availableCpuPayloadBytes,sizeof(NavigationStorage),d))return fail(d,"MC exact navigation storage exceeds remaining budget");
   navigation_.reset(new NavigationStorage(this));
   phase_=Phase::World;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[0].bytes,557,2685,458)+4*sources[0].bytes,d))return fail(d,"MC World reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[0].bytes;options.blocks=557;options.fields_per_block=29;
    options.zones=28;options.portals=28;options.instances=458;
    if(!member(0,raw,d)||!mc_world_semantics50MC::decodeWorld(raw,world_,d.decode,nullptr,options)||world_.zones.size()!=28||world_.portals.size()!=28||world_.instances.size()!=458)
     return fail(d,"MC source-backed complete World rejected");
    for(std::size_t i=0;i<zoneCount;++i)if(world_.zones[i].zone!=i+1)return fail(d,"MC source zone order rejected");
    for(std::size_t i=0;i<portalCount;++i){const auto& p=world_.portals[i];auto& target=navigation_->authoredPortals[i];
     target.zoneA=p.zones[0];target.zoneB=p.zones[1];
     // Source projection retains decimal -> binary64 -> binary32 values.
     for(unsigned j=0;j<4;++j)for(unsigned k=0;k<3;++k)target.vertices[j].v[k]=static_cast<float>(p.vertices[j][k]);}}
   phase_=Phase::Tob;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[3].bytes,1268,3016,0)+4*sources[3].bytes,d))return fail(d,"MC TOB reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[3].bytes;options.blocks=1268;options.fields_per_block=14;
    if(!member(3,raw,d)||!a::decodeParameters48F(raw,tob_,d.decode,nullptr,options)||tob_.blocks.size()!=1268)
     return fail(d,"MC strict TOB rejected");}
   phase_=Phase::MapGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[1].bytes+gateScratchBytes(),d))return fail(d,"MC Map gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(1,raw,d))return fail(d,"MC original full Map spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<173>>();
     if(!g::navigation_detail50MB::read<173>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->map,d.navigation))return fail(d,"MC full173 Map gates rejected");
     d.mapGatesCommitted=true;
     if(navigation_->map.track.count!=173||!navigation_->map.gatesPrepared||navigation_->map.indicesPrepared)return fail(d,"MC exact Map spline extent rejected");
     std::copy(std::begin(scratch->fieldMask),std::end(scratch->fieldMask),navigation_->mapFieldMask.begin());
    }
    phase_=Phase::MapMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<173>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->map,navigation_->mapMetadata,d.metadata))return fail(d,"MC full Map metadata rejected");}
   phase_=Phase::AiGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[2].bytes+gateScratchBytes(),d))return fail(d,"MC Ai gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(2,raw,d))return fail(d,"MC original full Ai spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<173>>();
     if(!g::navigation_detail50MB::read<173>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->ai,d.navigation))return fail(d,"MC full173 Ai gates rejected");
     d.aiGatesCommitted=true;
     if(navigation_->ai.track.count!=173||!navigation_->ai.gatesPrepared||navigation_->ai.indicesPrepared)return fail(d,"MC exact Ai spline extent rejected");
    }
    phase_=Phase::AiMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<173>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->ai,navigation_->aiMetadata,d.metadata))return fail(d,"MC full Ai metadata rejected");}
   phase_=Phase::Tuning;
   {if(!reserve(availableCpuPayloadBytes,4*sources[4].bytes,d))return fail(d,"MC tuning reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(4,raw,d)||!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),tuning_,d.metadata))return fail(d,"MC source tuning rejected");}
   if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"MC retained source exceeds remaining budget");
   navigation_->gatesReady=true;ready_=true;phase_=Phase::DocumentsReady;d.phase=phase_;return true;
  }catch(const std::exception& e){return fail(d,e.what());}catch(...){return fail(d,"MC source failure; reached libm prefix retained");}
 }
 // Cold inspection references require this owner to remain alive on its
 // issuing thread. No public runtime-capable navigation view is manufactured.
 const a::IndexedResArchive48C& archive()const noexcept{requireThread();if(!archive_)std::terminate();return *archive_;}
 const a::ParameterDocument48F& scb()const noexcept{requireThread();return world_.parameters;}
 const a::ParameterDocument48F& tob()const noexcept{requireThread();return tob_;}
 const NavigationStorage* coldNavigation()const noexcept{return documentsReady()?navigation_.get():nullptr;}
 const g::AiTrackTuning50IU& tuning()const noexcept{requireThread();return tuning_;}
 bool extractMember(const std::string& name,std::vector<std::uint8_t>& out,a::DecodeError48C& error,std::size_t maxBytes)const noexcept {
  if(!documentsReady()){error={a::DecodeKind48C::unsupported,"mc.source_not_ready",0,"MC authenticated documents required"};return false;}
  return archive_->extractMember(name,out,error,maxBytes);
 }
 const std::vector<std::uint8_t>& originalTob()const noexcept{requireThread();return tob_.original_bytes;}
 const a::ParameterDocument48F& triggerParameters()const noexcept{requireThread();return tob_;}
 const g::AiTrackTuning50IU& aiTuning()const noexcept{requireThread();return tuning_;}
 const std::array<std::uint32_t,173>& mapFieldMask()const noexcept {
  requireThread();static const std::array<std::uint32_t,173> empty{};return navigation_?navigation_->mapFieldMask:empty;
 }
 g::TrackNavigationView50MC map()const noexcept {
  auto self=weak_from_this().lock();return documentsReady()&&self?g::TrackNavigationView50MC{std::move(self),navigation_,{},g::NavigationRole50MC::Map}:g::TrackNavigationView50MC{};
 }
 g::TrackNavigationView50MC ai()const noexcept {
  auto self=weak_from_this().lock();return documentsReady()&&self?g::TrackNavigationView50MC{std::move(self),navigation_,{},g::NavigationRole50MC::Ai}:g::TrackNavigationView50MC{};
 }
 g::AiTrackMetadataView50MC mapMetadata()const noexcept{return g::AiTrackMetadataView50MC{map()};}
 g::AiTrackMetadataView50MC aiMetadata()const noexcept{return g::AiTrackMetadataView50MC{ai()};}
 bool prepareGrid(std::uint32_t rank,g::GridCenterResult50AC& out)const noexcept {
  return documentsReady()&&source_grid50IG::detail::prepare(navigation_->map,mapNodeCount,rank,out);
 }
 // Read-only lookup of the existing authentic MC mount. An absent mount
 // succeeds with an empty result; an incompatible or foreign-thread mount
 // fails without changing the caller's result. No new namespace is published.
 static bool reuseCanonicalSource(const a::OriginalStandardArchiveProvider50LA& provider,
     std::shared_ptr<const Owner>& out,a::OriginalStandardArchiveProvider50LA::Diagnostic& d)noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  if(out||!provider.retainedSourcesAccessible())return Provider::fail(d,Provider::Status::input);
  for(std::size_t i=0;i<provider.mounts_.size();++i){const auto& mount=*provider.mounts_[i];
   if(!Provider::sameName(mount.name,"TRACKS/MC.RES"))continue;
   d.mount=i;const auto& source=mount.mcSource;
   if(!source||!source->ready()||mount.mcThread!=std::this_thread::get_id()||
      mount.digest!=archiveSha||mount.archiveBytes!=archiveBytes||mount.retainedBytes!=compactBytes||
      mount.backing||mount.ui||mount.rcSource||mount.tbSource||mount.gbSource||mount.rows.size()!=archiveEntries)
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
  try {auto next=std::make_unique<Provider::Mount>();next->name="TRACKS/MC.RES";next->digest=archiveSha;next->archiveBytes=archiveBytes;
   next->mcSource=std::move(self);next->mcThread=thread_;next->retainedBytes=compactBytes;next->rows.reserve(archiveEntries);
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
 static constexpr bool gameplayReady()noexcept{return false;}
};
} // namespace mc_cold_sources50LA
