#pragma once
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "source_grid50ig_fixture.h"
#include "../../src/assets/indexed_resource48c.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "../../src/assets/original_standard_archive_provider50la.h"
#include "../../src/game/lap_progress50gf.h"
#include <array>
#include <exception>
#include <memory>
#include <algorithm>
#include <limits>
#include <string>
#include <thread>

namespace rc_navigation50RC { class Owner; class RuntimeBorrow; }
namespace rc_cold_sources50LA {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t archiveBytes=4655141,decodedBytes=31703040,compactBytes=8764370;
inline constexpr std::uint32_t archiveEntries=1121,zoneCount=15,portalCount=15,mapNodeCount=186,aiNodeCount=209;
inline constexpr const char* archiveSha="ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b";
struct MemberPin {const char* name;std::size_t offset,bytes;const char* sha;};
inline constexpr std::array<MemberPin,5> sources{{
 {"tracks\\rc.scb",196608,468918,"1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a"},
 {"tracks\\mrc.spl",28139520,39508,"f36306b7bdac88401b810add285785f770491987dbc55de7e0e5cd05319b270c"},
 {"tracks\\arc.spl",28090368,43780,"5687db7ebe3929fcf455121222d4d98e617332091e7bbbc291886403e6135171"},
 {"tracks\\rc.tob",0,124930,"71b1f9609c0a6ce84d71ff69355d9cfcbc8771eb7ddb8876473b4fddd6e23e6d"},
 {"tracks\\rc.tun",31678464,978,"3084d5216fb70e70d843f53ea7195f51c47fd3d096932534631dbadf07635c35"}
}};
enum class Phase {Empty,Authenticate,Archive,World,Tob,MapGates,MapMetadata,AiGates,AiMetadata,Tuning,Ready,Closed};
struct Diagnostic {
 Phase phase=Phase::Empty;std::string message;
 bool mapGatesCommitted=false,aiGatesCommitted=false;
 std::size_t retainedPayloadBytes=0,phaseReservationBytes=0,maximumReservationBytes=0;
 a::DecodeError48C decode{};g::TrackNavigationDiagnostic50EU navigation{};
 a::AiAuthoringDiagnostic50IU metadata{};g::LapProgressDiagnostic50GF lap{};
};
class Owner;
// This descriptive cold view cannot convert to a legacy runtime graph. In
// particular, no bound indices/TrackNavigation50EU reference can escape here.
class ColdTrack final {
 std::shared_ptr<const Owner> source_;bool ai_=false;
 friend class Owner;
 ColdTrack(std::shared_ptr<const Owner> source,bool ai):source_(std::move(source)),ai_(ai){}
public:
 ColdTrack()=default;
 bool valid()const noexcept;
 bool runtimeValid()const noexcept{return false;}
 std::uint32_t count()const noexcept;
 const g::UndertrackNode50ET* nodes()const noexcept;
 const g::TrackNavigationNode50EU* gates()const noexcept;
 const g::AiTrackNodeMetadata50IU* metadata()const noexcept;
 float trackLength()const noexcept;
 const Owner* sourceIdentity()const noexcept;
};
class Owner final : public std::enable_shared_from_this<Owner> {
 friend class ColdTrack;friend class ::rc_navigation50RC::Owner;friend class ::rc_navigation50RC::RuntimeBorrow;
 struct Navigation {
  g::TrackNavigation50EU map{},ai{};
  g::AiTrackMetadata50IU mapMetadata{},aiMetadata{};
  g::LapProgressTrack50GF mapLap{};
  g::LiveZonePortalInput50EN portals[portalCount]{};
 };
 std::unique_ptr<a::IndexedResArchive48C> archive_;
 a::World48F world_;a::ParameterDocument48F tob_;
 std::unique_ptr<Navigation> navigation_;
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
  // Found by name; the stock offset/size/SHA is a receipt (common rule).
  if(!match||!archive_->extractMember(match->name,out,d.decode,match->bytes)||out.size()!=match->bytes)return false;
  course_archive_receipt50CI::memberReceipt("rc_cold_sources",pin.name,out,pin.offset,match->logical_offset,pin.bytes,pin.sha);
  return true;
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
 static constexpr std::size_t navigationPayloadBytes()noexcept{return sizeof(Navigation);}
 static constexpr std::size_t gateScratchBytes()noexcept{return sizeof(g::TrackNavigationScratch50EU);}
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;std::size_t n=sizeof(Owner);
  if(archive_)add(n,archive_->retainedPayloadBytes());
  parametersPayload(n,world_.parameters);vectorPayload(n,world_.zones);vectorPayload(n,world_.portals);
  for(const auto& p:world_.portals)textPayload(n,p.section);
  vectorPayload(n,world_.instances);for(const auto& i:world_.instances){textPayload(n,i.section);textPayload(n,i.geometry_member);textPayload(n,i.collision_member);}
  parametersPayload(n,tob_);if(navigation_)add(n,sizeof(Navigation));return n;
 }
 bool decodeOwned(std::vector<std::uint8_t>&& original,g::SourceLibmState49T& actualLibm,
     std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id()){d.message="RC source thread mismatch";return false;}
  if(attempted_)return fail(d,"RC cold source preparation is one-attempt");attempted_=true;phase_=Phase::Authenticate;
  try {
   if(original.empty()||original.size()>course_archive_receipt50CI::maximumArchiveBytes())return fail(d,"RC archive exceeds the shared course-archive capacity");
   if(original.capacity()>(std::numeric_limits<std::size_t>::max()-65536)/4||
      !reserve(availableCpuPayloadBytes,4*original.capacity()+65536,d))return fail(d,"RC authentication scratch exceeds existing payload budget");
   // Common rule: the stock identity is a receipt (course_archive_receipt50ci.h).
   course_archive_receipt50CI::receipt("rc_cold_sources","RC",original,archiveSha,archiveBytes);
   phase_=Phase::Archive;archive_=std::make_unique<a::IndexedResArchive48C>();
   a::DecodeLimits48C limits=course_archive_receipt50CI::limits();
   const auto owned=sizeof(Owner);if(availableCpuPayloadBytes<owned||
      !archive_->loadOwned(std::move(original),availableCpuPayloadBytes-owned,d.decode,nullptr,limits)||
      archive_->entries().empty())return fail(d,"RC complete indexed original archive rejected");
   course_archive_receipt50CI::countReceipt("rc_cold_sources","entries",archive_->entries().size(),archiveEntries);
   d.maximumReservationBytes=std::max(d.maximumReservationBytes,owned+archive_->preparationPeakPayloadBytes());
   // Exact existing full-name case rule, allocation-free duplicate check.
   for(std::size_t i=0;i<archive_->entries().size();++i)for(std::size_t j=0;j<i;++j)
    if(a::OriginalStandardArchiveProvider50LA::sameName(archive_->entries()[i].name,archive_->entries()[j].name))
     return fail(d,"RC ambiguous archive member identity");
   if(!reserve(availableCpuPayloadBytes,sizeof(Navigation),d))return fail(d,"RC legacy navigation storage exceeds remaining budget");
   navigation_=std::make_unique<Navigation>();
   phase_=Phase::World;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[0].bytes,1329,6626,1277)+4*sources[0].bytes,d))return fail(d,"RC World parse phase reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[0].bytes;options.blocks=1329;options.fields_per_block=16;options.zones=15;options.portals=15;options.instances=1277;
    if(!member(0,raw,d)||!a::decodeWorld48F(raw,world_,d.decode,nullptr,options)||world_.zones.size()!=15||world_.portals.size()!=15||world_.instances.size()!=1277)
     return fail(d,"RC authentic World rejected");
    for(std::size_t i=0;i<15;++i){if(world_.zones[i].zone!=i+1)return fail(d,"RC source zone order rejected");
     const auto& p=world_.portals[i];if(p.parameter_block>=world_.parameters.blocks.size())return fail(d,"RC portal block invalid");
     auto& target=navigation_->portals[i];target.zoneA=p.zones[0];target.zoneB=p.zones[1];
     for(unsigned j=0;j<4;++j)if(!vectorField(world_.parameters.blocks[p.parameter_block],("vertex"+std::to_string(j+1)).c_str(),target.vertices[j]))return fail(d,"RC original portal binary32 vertex rejected");}}
   phase_=Phase::Tob;
   {if(!reserve(availableCpuPayloadBytes,parameterDecodeReservation(sources[3].bytes,907,1954,0)+4*sources[3].bytes,d))return fail(d,"RC TOB parse reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;a::WorldLimits48F options;options.input_bytes=sources[3].bytes;options.blocks=907;options.fields_per_block=9;
    if(!member(3,raw,d)||!a::decodeParameters48F(raw,tob_,d.decode,nullptr,options))return fail(d,"RC complete strict TOB rejected");}
   for(std::size_t index=1;index<=2;++index){
    phase_=index==1?Phase::MapGates:Phase::AiGates;
    if(!reserve(availableCpuPayloadBytes,4*sources[index].bytes+gateScratchBytes(),d))return fail(d,"RC legacy gate parse reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(index,raw,d))return fail(d,"RC original full spline member rejected");
    auto& track=index==1?navigation_->map:navigation_->ai;
    {auto scratch=std::make_unique<g::TrackNavigationScratch50EU>();
     if(!g::readTrackNavigation50EU(reinterpret_cast<const char*>(raw.data()),raw.size(),actualLibm,*scratch,track,d.navigation))return fail(d,"RC legacy source gate reader rejected");
     (index==1?d.mapGatesCommitted:d.aiGatesCommitted)=true;
     if(index==1&&!g::prepareLapProgressTrack50GF(*scratch,navigation_->mapLap,d.lap))return fail(d,"RC same-read MAP lap markers rejected");}
    if(track.track.count!=(index==1?186u:209u)||!track.gatesPrepared||track.indicesPrepared)return fail(d,"RC exact legacy spline extent rejected");
    phase_=index==1?Phase::MapMetadata:Phase::AiMetadata;
    auto& metadata=index==1?navigation_->mapMetadata:navigation_->aiMetadata;
    if(!a::readAiTrackMetadata50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),track,metadata,d.metadata))return fail(d,"RC full metadata rejected");}
   phase_=Phase::Tuning;
   {if(!reserve(availableCpuPayloadBytes,4*sources[4].bytes,d))return fail(d,"RC tuning reservation exceeds remaining budget");
    std::vector<std::uint8_t> raw;if(!member(4,raw,d)||!a::readAiTrackTuning50IU(reinterpret_cast<const char*>(raw.data()),raw.size(),tuning_,d.metadata))return fail(d,"RC source tuning rejected");}
   if(!reserve(availableCpuPayloadBytes,0,d))return fail(d,"RC retained source exceeds remaining budget");
   ready_=true;phase_=Phase::Ready;d.phase=phase_;return true;
  }catch(const std::exception& e){return fail(d,e.what());}catch(...){return fail(d,"RC source failure; reached libm prefix retained");}
 }
 const a::IndexedResArchive48C& archive()const noexcept{requireThread();if(!archive_)std::terminate();return *archive_;}
 const a::World48F& world()const noexcept{requireThread();return world_;}
 const std::vector<std::uint8_t>& originalTob()const noexcept{requireThread();return tob_.original_bytes;}
 const a::ParameterDocument48F& triggerParameters()const noexcept{requireThread();return tob_;}
 const g::AiTrackTuning50IU& aiTuning()const noexcept{requireThread();return tuning_;}
 bool extractMember(const std::string& name,std::vector<std::uint8_t>& out,a::DecodeError48C& error,std::size_t maxBytes)const noexcept {
  if(!ready()){error={a::DecodeKind48C::unsupported,"rc.source_not_ready",0,"RC authentic cold source required"};return false;}
  return archive_->extractMember(name,out,error,maxBytes);
 }
 ColdTrack map()const noexcept{auto self=weak_from_this().lock();return ready()&&self?ColdTrack{std::move(self),false}:ColdTrack{};}
 ColdTrack ai()const noexcept{auto self=weak_from_this().lock();return ready()&&self?ColdTrack{std::move(self),true}:ColdTrack{};}
 bool prepareGrid(std::uint32_t rank,g::GridCenterResult50AC& out)const noexcept{return ready()&&source_grid50IG::prepare(navigation_->map,rank,out);}
 // Read-only lookup of the existing authentic RC mount. An absent mount
 // succeeds with an empty result; an incompatible or foreign-thread mount
 // fails without changing the caller's result. No new namespace is published.
 static bool reuseCanonicalSource(const a::OriginalStandardArchiveProvider50LA& provider,
     std::shared_ptr<const Owner>& out,a::OriginalStandardArchiveProvider50LA::Diagnostic& d)noexcept {
  using Provider=a::OriginalStandardArchiveProvider50LA;d={};
  if(out||!provider.retainedSourcesAccessible())return Provider::fail(d,Provider::Status::input);
  for(std::size_t i=0;i<provider.mounts_.size();++i){const auto& mount=*provider.mounts_[i];
   if(!Provider::sameName(mount.name,"TRACKS/RC.RES"))continue;
   d.mount=i;const auto& source=mount.rcSource;
   if(!source||!source->ready()||mount.rcThread!=std::this_thread::get_id()||
      mount.digest!=archiveSha||mount.archiveBytes!=archiveBytes||mount.retainedBytes!=source->archive_->compactBytes()||
      mount.backing||mount.ui||mount.rows.size()!=source->archive_->entries().size())
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
  // Sequential exact member hashing uses one source copy plus the SHA
  // padding vector (whose first append may double capacity). Keep this
  // conservative preparation reservation charged by the existing root ledger.
  add(n,3u*(196652u+128u)+1024u);
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
   for(const auto& mount:provider.mounts_)if(Provider::sameName(mount->name,"TRACKS/RC.RES"))active=mount->active;
   if(active){d={};return true;}
  }
  if(canonicalMountReservationBytes()>availableAdditionalPayloadBytes)return Provider::fail(d,Provider::Status::capacity);
  try {auto next=std::make_unique<Provider::Mount>();next->name="TRACKS/RC.RES";next->digest=archiveSha;/*FLAG: mount label stays the stock digest (receipt only; real bytes are the source)*/next->archiveBytes=archiveBytes;
   next->rcSource=std::move(self);next->rcThread=thread_;next->retainedBytes=archive_->compactBytes();next->rows.reserve(archive_->entries().size());
   for(std::size_t i=0;i<archive_->entries().size();++i){const auto m=archive_->member(i);d.ordinal=i;
    if(!m.identity||!m.data||m.size!=m.identity->bytes||!Provider::nameOkay(m.identity->name))return Provider::fail(d,Provider::Status::identity);
    next->rows.push_back({m.identity->name,m.data,m.size,m.identity->logical_offset});}
   // Source12E2B0 ignores later equal-name registration. This bounded native
   // policy uses actual native POD17-before-RC order; complete retail load
   // chronology is not claimed. Preserve both original payloads/namespaces.
   for(std::size_t m=0;m<provider.mounts_.size();++m){const auto& pod=*provider.mounts_[m];
    const auto slash=pod.name.find_last_of("/\\");
    const auto basename=slash==std::string::npos?pod.name:pod.name.substr(slash+1);
    if(!Provider::sameName(basename,"POD17.RES"))continue;
    if(next->rcSpecmapEarlierMount!=static_cast<std::size_t>(-1)||pod.archiveBytes!=377255||
       pod.digest!="10a33edac421f915382c8b171c97c99978a5136789ae38abe9f6e97c68e4a51e")
     return Provider::fail(d,Provider::Status::identity);
    const auto locate=[](const auto& rows){std::size_t found=static_cast<std::size_t>(-1);
     for(std::size_t i=0;i<rows.size();++i)if(Provider::sameName(rows[i].name,"textures\\specmap5.tga")){
      if(found!=static_cast<std::size_t>(-1))return rows.size();found=i;}
     return found;};
    const auto pi=locate(pod.rows),ri=locate(next->rows);
    if(pi>=pod.rows.size()||ri>=next->rows.size())return Provider::fail(d,Provider::Status::identity);
    const auto& earlier=pod.rows[pi];const auto& later=next->rows[ri];
    const auto hashed=[](const auto& row,std::size_t extent,const char* digest){
     if(!row.data||row.size!=extent)return false;
     return pod_materials50ed_fixture::detail::sha256(std::vector<std::uint8_t>(row.data,row.data+row.size))==digest;};
    if(!hashed(earlier,12332,"574712ee163003de4bdbdfbeff4d495c52570b9dd2be3b62ec399d7b3dd27b6b")||
       !hashed(later,196652,"0c9838ec2d3a1936b6dc1a5e218accfd29f47307c5a7d9801614a2d1858bc7ea"))
     return Provider::fail(d,Provider::Status::identity);
    next->rcSpecmapEarlierMount=m;next->rcSpecmapEarlierOrdinal=pi;next->rcSpecmapOrdinal=ri;
   }
   return provider.commit(std::move(next),d,lease);
  }catch(...){return Provider::fail(d,Provider::Status::allocation);}
 }
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id()||weak_from_this().use_count()>1)return false;
  ready_=false;navigation_.reset();archive_.reset();world_={};tob_={};tuning_={};attempted_=true;phase_=Phase::Closed;return true;
 }
};
inline bool ColdTrack::valid()const noexcept{return source_&&source_->ready();}
inline std::uint32_t ColdTrack::count()const noexcept{return valid()?(ai_?source_->navigation_->ai.track.count:source_->navigation_->map.track.count):0;}
inline const g::UndertrackNode50ET* ColdTrack::nodes()const noexcept{return valid()?(ai_?source_->navigation_->ai.track.nodes:source_->navigation_->map.track.nodes):nullptr;}
inline const g::TrackNavigationNode50EU* ColdTrack::gates()const noexcept{return valid()?(ai_?source_->navigation_->ai.nodes:source_->navigation_->map.nodes):nullptr;}
inline const g::AiTrackNodeMetadata50IU* ColdTrack::metadata()const noexcept{return valid()?(ai_?source_->navigation_->aiMetadata.nodes:source_->navigation_->mapMetadata.nodes):nullptr;}
inline float ColdTrack::trackLength()const noexcept{return valid()?(ai_?source_->navigation_->aiMetadata.trackLength3C:source_->navigation_->mapMetadata.trackLength3C):0.f;}
inline const Owner* ColdTrack::sourceIdentity()const noexcept{return valid()?source_.get():nullptr;}
}
