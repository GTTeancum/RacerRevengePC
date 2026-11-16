#pragma once
#include "track_cold_source_services50la.h"
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "source_grid50ig_fixture.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/assets/original_track_cut_identity50la.h"
#include "../../src/game/track_navigation_view50gb.h"
#include "../../src/game/track_navigation_shared50mb.inl"
#include <array>
#include <exception>
#include <memory>
#include <algorithm>
#include <limits>
#include <string>
#include <thread>

namespace gb_navigation50GB { class Owner; }
namespace gb_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace storage=g::navigation_storage_detail50GB;
inline constexpr std::size_t archiveBytes=4088586,decodedBytes=28950528,compactBytes=7305845;
inline constexpr std::uint32_t archiveEntries=1049,zoneCount=13,portalCount=17,mapNodeCount=213,aiNodeCount=257;
inline constexpr const char* archiveSha="e92af24305431dc5a96cebb5ebcf89159adb6d371045b2c5424dacd1f74b7d99";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\gb.scb",73728,401867,"ed0eb494f8c91df5264e120bad3780016f67f297f259572d48a3e7a56aad27bb"},
 {"tracks\\mgb.spl",16244736,44479,"66dc9e7a76732767f819c6e25a721f65702c5d5e4e246a9b558ed21f184a3762"},
 {"tracks\\agb.spl",16171008,53066,"a3a4a558146a4343f817fc551c16c59a0efa93cf87327c2672329936c0002ba2"},
 {"tracks\\gb.tob",0,20617,"96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1"},
 {"tracks\\gb.tun",28925952,998,"06ea81042839962ac172ab2fd0da2cb2b5e72df8ae99210d604e9b7b72644915"}
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
 friend class ::gb_navigation50GB::Owner;
 std::unique_ptr<a::IndexedResArchive48C> archive_;
 a::World48F world_;a::ParameterDocument48F tob_;
 std::shared_ptr<storage::Storage> navigation_;
 g::AiTrackTuning50IU tuning_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;Phase phase_=Phase::Empty;
 static void add(std::size_t& n,std::size_t x)noexcept {
  track_cold_source_services50LA::add(n,x);
 }
 template<class T> static void vectorPayload(std::size_t& n,const std::vector<T>& v)noexcept {
  track_cold_source_services50LA::vectorPayload(n,v.capacity(),sizeof(T));
 }
 static void textPayload(std::size_t& n,const std::string& s)noexcept {
  track_cold_source_services50LA::textPayload(n,s);
 }
 static void parametersPayload(std::size_t& n,const a::ParameterDocument48F& p)noexcept {
  track_cold_source_services50LA::parametersPayload(n,p);
 }
 void requireThread()const noexcept{if(thread_!=std::this_thread::get_id())std::terminate();}
 bool fail(Diagnostic& d,const char* message){d.phase=phase_;d.message=message;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
 bool reserve(std::size_t limit,std::size_t extra,Diagnostic& d)const noexcept {
  d.retainedPayloadBytes=retainedPayloadBytes();d.phaseReservationBytes=d.retainedPayloadBytes;add(d.phaseReservationBytes,extra);
  d.maximumReservationBytes=std::max(d.maximumReservationBytes,d.phaseReservationBytes);return d.phaseReservationBytes<=limit;
 }
 bool member(std::size_t i,std::vector<std::uint8_t>& out,Diagnostic& d)const {
  const auto& pin=sources[i];
  return track_cold_source_services50LA::authenticateMember(
      *archive_,{pin.name,pin.offset,pin.bytes,pin.sha},out,d.decode);
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
 static constexpr std::size_t gateScratchBytes()noexcept{return sizeof(storage::Scratch<g::courseNavigationCapacity50MB>)+sizeof(storage::Metadata<g::courseNavigationCapacity50MB>)+g::courseNavigationCapacity50MB;}
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
  d={};if(thread_!=std::this_thread::get_id()){d.message="GB source thread mismatch";return false;}
  if(attempted_)return fail(d,"GB cold source preparation is one-attempt");attempted_=true;phase_=Phase::Authenticate;
  try {
   if(!availableCpuPayloadBytes||availableCpuPayloadBytes>32u*1024u*1024u)return fail(d,"GB source requires the unchanged existing32MiB allowance");
   if(original.empty()||original.size()>course_archive_receipt50CI::maximumArchiveBytes())return fail(d,"GB archive exceeds the shared course-archive capacity");
   if(original.capacity()>(std::numeric_limits<std::size_t>::max()-65536)/4||
      !reserve(availableCpuPayloadBytes,4*original.capacity()+65536,d))return fail(d,"GB authentication scratch exceeds existing payload budget");
   // Common rule: the stock identity is a receipt (course_archive_receipt50ci.h).
   course_archive_receipt50CI::receipt("gb_cold_sources","GB",original,archiveSha,archiveBytes);
   phase_=Phase::Archive;archive_=std::make_unique<a::IndexedResArchive48C>();
   a::DecodeLimits48C limits=course_archive_receipt50CI::limits();
   const auto owned=sizeof(Owner);if(availableCpuPayloadBytes<owned||
      !archive_->loadOwned(std::move(original),availableCpuPayloadBytes-owned,d.decode,nullptr,limits)||
      archive_->entries().empty())return fail(d,"GB complete indexed original archive rejected");
   course_archive_receipt50CI::countReceipt("gb_cold_sources","entries",archive_->entries().size(),archiveEntries);
   d.maximumReservationBytes=std::max(d.maximumReservationBytes,owned+archive_->preparationPeakPayloadBytes());
   // Exact existing full-name case rule, allocation-free duplicate check.
   for(std::size_t i=0;i<archive_->entries().size();++i)for(std::size_t j=0;j<i;++j)
    if(a::OriginalStandardArchiveProvider50LA::sameName(archive_->entries()[i].name,archive_->entries()[j].name))
     return fail(d,"GB ambiguous archive member identity");
   if(!reserve(availableCpuPayloadBytes,sizeof(storage::Storage),d))return fail(d,"GB exact navigation storage exceeds remaining budget");
   navigation_.reset(new storage::Storage(this));
   phase_=Phase::World;
   // Common course rule (SC's model): the SCB is decoded with the shared
   // decoder limits (WorldLimits48F defaults) and its own counts; the stock
   // extents (13/17/975, 1081 blocks) are receipts. The phase reservation is
   // charged from the member actually read (stock block/field/instance
   // densities as the estimate) and the retained World is re-checked against
   // the budget right after the decode.
   {std::vector<std::uint8_t> raw;if(!member(0,raw,d))return fail(d,"GB World member missing");
    if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(raw.size(),1081,5889,975)+4*raw.size(),d))return fail(d,"GB World parse phase reservation exceeds remaining budget");
    a::WorldLimits48F options;options.input_bytes=std::max(options.input_bytes,raw.size());
    if(!a::decodeWorld48F(raw,world_,d.decode,nullptr,options)||world_.zones.empty()||world_.zones.size()>256||
       world_.portals.size()>g::courseNavigationPortalCapacity50MB)
     return fail(d,"GB World rejected (decode, or zones/portals beyond the shared navigation capacity)");
    course_archive_receipt50CI::countReceipt("gb_cold_sources","zones",world_.zones.size(),zoneCount);
    course_archive_receipt50CI::countReceipt("gb_cold_sources","portals",world_.portals.size(),portalCount);
    course_archive_receipt50CI::countReceipt("gb_cold_sources","instances",world_.instances.size(),975);
    if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"GB retained World exceeds remaining budget");
    for(std::size_t i=0;i<world_.zones.size();++i)if(world_.zones[i].zone!=i+1)return fail(d,"GB source zone order rejected");
    navigation_->authoredZoneCount=static_cast<std::uint32_t>(world_.zones.size());
    navigation_->authoredPortalCount=static_cast<std::uint32_t>(world_.portals.size());
    for(std::size_t i=0;i<world_.portals.size();++i){
     const auto& p=world_.portals[i];if(p.parameter_block>=world_.parameters.blocks.size())return fail(d,"GB portal block invalid");
     auto& target=navigation_->authoredPortals[i];target.zoneA=p.zones[0];target.zoneB=p.zones[1];
     for(unsigned j=0;j<4;++j)if(!vectorField(world_.parameters.blocks[p.parameter_block],("vertex"+std::to_string(j+1)).c_str(),target.vertices[j]))return fail(d,"GB original portal binary32 vertex rejected");}}
   phase_=Phase::Tob;
   {std::vector<std::uint8_t> raw;if(!member(3,raw,d))return fail(d,"GB TOB member missing");
    if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(raw.size(),70,362,0)+4*raw.size(),d))return fail(d,"GB TOB parse reservation exceeds remaining budget");
    a::WorldLimits48F options;options.input_bytes=std::max(options.input_bytes,raw.size());options.retainDuplicateLabels=false;
    // Authentic GB has no duplicate labels. Both complete documents retain
    // the ordinary strict decoder (shared limits); stock 70 blocks is a receipt.
    if(!a::decodeParameters48F(raw,tob_,d.decode,nullptr,options))return fail(d,"GB complete strict TOB rejected");
    course_archive_receipt50CI::countReceipt("gb_cold_sources","tob_blocks",tob_.blocks.size(),70);
    if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"GB retained TOB exceeds remaining budget");}
   phase_=Phase::MapGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[1].bytes+gateScratchBytes(),d))return fail(d,"GB Map gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(1,raw,d))return fail(d,"GB original full Map spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
     if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->map,d.navigation))return fail(d,"GB Map gates rejected");
     d.mapGatesCommitted=true;
     course_archive_receipt50CI::countReceipt("gb_cold_sources","map_nodes",navigation_->map.track.count,mapNodeCount);
     if(!navigation_->map.track.count||!navigation_->map.gatesPrepared||navigation_->map.indicesPrepared)return fail(d,"GB Map spline extent rejected");
     std::copy(std::begin(scratch->fieldMask),std::end(scratch->fieldMask),navigation_->mapFieldMask.begin());
    }
    phase_=Phase::MapMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->map,navigation_->mapMetadata,d.metadata))return fail(d,"GB full Map metadata rejected");}
   phase_=Phase::AiGates;
   {if(!reserve(availableCpuPayloadBytes,4*sources[2].bytes+gateScratchBytes(),d))return fail(d,"GB Ai gate reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(2,raw,d))return fail(d,"GB original full Ai spline rejected");
    {auto scratch=std::make_unique<storage::Scratch<g::courseNavigationCapacity50MB>>();
     if(!g::navigation_detail50MB::read<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,navigation_->ai,d.navigation))return fail(d,"GB Ai gates rejected");
     d.aiGatesCommitted=true;
     course_archive_receipt50CI::countReceipt("gb_cold_sources","ai_nodes",navigation_->ai.track.count,aiNodeCount);
     if(!navigation_->ai.track.count||!navigation_->ai.gatesPrepared||navigation_->ai.indicesPrepared)return fail(d,"GB Ai spline extent rejected");
    }
    phase_=Phase::AiMetadata;
    if(!a::ai_authoring_detail50IU::readTrackMetadata<g::courseNavigationCapacity50MB>(reinterpret_cast<const char*>(raw.data()),raw.size(),navigation_->ai,navigation_->aiMetadata,d.metadata))return fail(d,"GB full Ai metadata rejected");}
   phase_=Phase::Tuning;
   {if(!reserve(availableCpuPayloadBytes,4*sources[4].bytes,d))return fail(d,"GB tuning reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(4,raw,d)||!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),tuning_,d.metadata))return fail(d,"GB source tuning rejected");}
   if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"GB retained source exceeds remaining budget");
   navigation_->gatesReady=true;ready_=true;phase_=Phase::Ready;d.phase=phase_;return true;
  }catch(const std::exception& e){return fail(d,e.what());}catch(...){return fail(d,"GB source failure; reached libm prefix retained");}
 }
 const a::IndexedResArchive48C& archive()const noexcept{requireThread();if(!archive_)std::terminate();return *archive_;}
 const a::World48F& world()const noexcept{requireThread();return world_;}
 const std::vector<std::uint8_t>& originalTob()const noexcept{requireThread();return tob_.original_bytes;}
 const a::ParameterDocument48F& triggerParameters()const noexcept{requireThread();return tob_;}
 const g::AiTrackTuning50IU& aiTuning()const noexcept{requireThread();return tuning_;}
 bool extractMember(const std::string& name,std::vector<std::uint8_t>& out,a::DecodeError48C& error,std::size_t maxBytes)const noexcept {
  if(!ready()){error={a::DecodeKind48C::unsupported,"gb.source_not_ready",0,"GB authentic cold source required"};return false;}
  return archive_->extractMember(name,out,error,maxBytes);
 }
 const std::array<std::uint32_t,g::courseNavigationCapacity50MB>& mapFieldMask() const noexcept {
  requireThread();static const std::array<std::uint32_t,g::courseNavigationCapacity50MB> empty{};return navigation_?navigation_->mapFieldMask:empty;
 }
 g::TrackNavigationView50GB map()const noexcept {
  auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50GB{std::move(self),navigation_,{},g::NavigationRole50GB::Map}:g::TrackNavigationView50GB{};
 }
 g::TrackNavigationView50GB ai()const noexcept {
  auto self=weak_from_this().lock();return ready()&&self?g::TrackNavigationView50GB{std::move(self),navigation_,{},g::NavigationRole50GB::Ai}:g::TrackNavigationView50GB{};
 }
 g::AiTrackMetadataView50GB mapMetadata()const noexcept{return g::AiTrackMetadataView50GB{map()};}
 g::AiTrackMetadataView50GB aiMetadata()const noexcept{return g::AiTrackMetadataView50GB{ai()};}
 bool prepareGrid(std::uint32_t rank,g::GridCenterResult50AC& out)const noexcept {
  return ready()&&source_grid50IG::detail::prepare(navigation_->map,g::courseNavigationCapacity50MB,rank,out);
 }
 // Read-only lookup of the existing authentic GB mount. An absent mount
 // succeeds with an empty result; an incompatible or foreign-thread mount
 // fails without changing the caller's result. No new namespace is published.
 static bool reuseCanonicalSource(const a::OriginalStandardArchiveProvider50LA& provider,
     std::shared_ptr<const Owner>& out,a::OriginalStandardArchiveProvider50LA::Diagnostic& d)noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  if(out||!provider.retainedSourcesAccessible())return Provider::fail(d,Provider::Status::input);
  for(std::size_t i=0;i<provider.mounts_.size();++i){const auto& mount=*provider.mounts_[i];
   if(!Provider::sameName(mount.name,"TRACKS/GB.RES"))continue;
   d.mount=i;const auto& source=mount.gbSource;
   if(!source||!source->ready()||mount.gbThread!=std::this_thread::get_id()||
      mount.digest!=a::originalTrackMountDigest50LA("TRACKS/GB.RES",archiveSha)||
      mount.archiveBytes!=a::originalTrackMountBytes50LA("TRACKS/GB.RES",archiveBytes)||mount.retainedBytes!=source->archive_->compactBytes()||
      mount.backing||mount.ui||mount.rcSource||mount.tbSource||mount.rows.size()!=source->archive_->entries().size())
    return Provider::fail(d,Provider::Status::identity);
   for(std::size_t j=0;j<source->archive_->entries().size();++j){const auto member=source->archive_->member(j);const auto& row=mount.rows[j];
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
  add(n,archive_->entries().size()*sizeof(typename Provider::Row));
  for(const auto& e:archive_->entries())add(n,2*std::max<std::size_t>(15,e.name.size())+1);
  add(n,256);return n;
 }
 // lease (optional): the race session's course-archive lease. A fresh mount
 // or a reactivation of this same source's released mount is recorded in it,
 // so the bank is released at race end (1316A0) like every course's.
 bool mountCanonicalArchive(a::OriginalStandardArchiveProvider50LA& provider,std::size_t availableAdditionalPayloadBytes,
     a::OriginalStandardArchiveProvider50LA::Diagnostic& d,
     a::NativeRaceMountLease50LA* lease=nullptr)const noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  auto self=weak_from_this().lock();if(!ready()||!self)return Provider::fail(d,Provider::Status::identity);
  // A verified same-source mount already owns every descriptor/member. This
  // read-only path preserves the actual seal/revision; caller still performs
  // its normal seal and resolver bind. A different authentic owner rejects.
  std::shared_ptr<const Owner> existing;
  if(!reuseCanonicalSource(provider,existing,d))return false;
  if(existing){
   if(existing.get()!=this)return Provider::fail(d,Provider::Status::identity);
   // Still active: the same mount stays (no new record). Released at an
   // earlier race end: fall through and reactivate it through commit with
   // identical rows from this same source.
   bool active=false;
   for(const auto& mount:provider.mounts_)if(Provider::sameName(mount->name,"TRACKS/GB.RES"))active=mount->active;
   if(active){d={};return true;}
  }
  if(canonicalMountReservationBytes()>availableAdditionalPayloadBytes)return Provider::fail(d,Provider::Status::capacity);
  try {auto next=std::make_unique<Provider::Mount>();next->name="TRACKS/GB.RES";
   // One rule for every course mount (original_track_cut_identity50la.h): the
   // admitted archive's digest/size, so the intro/camera member checks see the
   // archive actually mounted. Stock disc: the stock digest.
   next->digest=a::originalTrackMountDigest50LA(next->name,archiveSha);next->archiveBytes=a::originalTrackMountBytes50LA(next->name,archiveBytes);
   next->gbSource=std::move(self);next->gbThread=thread_;next->retainedBytes=archive_->compactBytes();next->rows.reserve(archive_->entries().size());
   for(std::size_t i=0;i<archive_->entries().size();++i){const auto m=archive_->member(i);d.ordinal=i;
    if(!m.identity||!m.data||m.size!=m.identity->bytes||!Provider::nameOkay(m.identity->name))return Provider::fail(d,Provider::Status::identity);
    next->rows.push_back({m.identity->name,m.data,m.size,m.identity->logical_offset});}
   return provider.commit(std::move(next),d,lease);
  }catch(...){return Provider::fail(d,Provider::Status::allocation);}
 }
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id()||weak_from_this().use_count()>1||(navigation_&&navigation_.use_count()!=1))return false;
  ready_=false;navigation_.reset();archive_.reset();world_={};tob_={};tuning_={};attempted_=true;phase_=Phase::Closed;return true;
 }
};
}
