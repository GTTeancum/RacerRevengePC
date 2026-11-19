#pragma once
#include "material_audio_provider50hi_fixture.h"
#include "gb_audio_originals50gb.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include "../../src/game/original_ai_aural.h"
#include "positional_sound_rows50sr.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <stdexcept>
#include <thread>
#include <utility>

namespace gb_positional_sound50GB {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
inline constexpr std::size_t descriptorCapacity=a::soulTreeSoundCapacity50SR,ambientCapacity=a::ambientSoundCapacity50SR,firstVoice=128,voiceCount=24;
static_assert(firstVoice==material_audio50HI::gaExtraFirstVoice);
static_assert(firstVoice+voiceCount==152);
static_assert(firstVoice>126&&firstVoice+voiceCount<=material_audio50HI::gbExtraFirstVoice);
struct Descriptor {
 std::string name;std::array<float,4> position{{0,0,0,1}};
 float nearDistance=0,farDistance=0,triggerDistance=20,chance=100;
 bool loop=false,oneShot=false,resetWhenInaudible=false;
};
struct AmbientDescriptor {std::string name;float cycleSeconds=0,chance=0;};
// Stock values of this slot: receipts only (positional_sound_rows50sr.h). Stock
// GB: 49 SoulTree rows over 4 PCM files, 4 ambient rows.
inline constexpr positional_sound_rows50SR::Stock stock{"GB","tracks\\gb.aub",29684u,"2d42e759aef7b80193d60581b611c572b836de91145ee18f439d45f46e4f530f",49u,4u};
inline constexpr std::size_t stockAmbient=4;
struct State {bool played=false,randomAttempted=false;int slot=-1;};
struct Receipt {bool complete=false;std::size_t eligible=0,started=0,stopped=0,controlled=0,deferred=0,randomDraws=0;};
// Native fixed-position adapter for the course AUB's SoulTree list (the common
// reader's live rows; stock GB authors 49), distinct from the 24 GB-exclusive
// transport slots. The ambient list (stock 4) is retained, not scheduled.
// Native policy: retain active eligible sounds, then use the lowest free slot
// in authored descriptor order; never steal an active sound. Deferred rows do
// not draw RNG or change played/random latches. No source hardware parity claim.
class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> source_;
 a::AudioBank50HC bank_;
 std::array<Descriptor,descriptorCapacity> rows_{};
 std::array<AmbientDescriptor,ambientCapacity> ambient_{}; // Retained only, no invented scheduler.
 std::array<p::AudioClip50HD,descriptorCapacity> clips_{};
 std::array<State,descriptorCapacity> states_{};
 std::size_t count_=0,ambientCount_=0; // live rows from the common AUB reader
 std::array<int,voiceCount> assigned_{};
 p::LogicalAudioOwner logical_;
 p::AudioMixer50HD* mixer_=nullptr;p::RaceAudioGroups50IC* groups_=nullptr;
 std::thread::id thread_=std::this_thread::get_id();std::uint64_t epoch_=0;
 bool attempted_=false,ready_=false,blocked_=false,closed_=false,authored_=false;
 bool current()const noexcept {
  if(thread_!=std::this_thread::get_id()||!source_||!source_->ready()||!mixer_||!groups_||
     !groups_->uses(*mixer_)||mixer_->groupEpoch50IC()!=epoch_)return false;
  for(std::size_t i=0;i<voiceCount;++i)if(groups_->binding(firstVoice+i)!=g::RaceAudioGroup50IC::World4D8)return false;
  return true;
 }
 // The course AUB read by name at its own size (shared 1 MiB member cap).
 static bool readAub(const gb_cold_sources50LA::Owner& source,std::vector<std::uint8_t>& raw,a::DecodeError48C& de){
  std::size_t bytes=0;
  for(const auto& e:source.archive().entries())if(a::OriginalStandardArchiveProvider50LA::sameName(e.name,"tracks\\gb.aub"))bytes=e.bytes;
  return bytes&&bytes<=1024u*1024u&&source.extractMember("tracks\\gb.aub",raw,de,bytes)&&raw.size()==bytes;
 }
 bool release(std::size_t row,Receipt& r)noexcept {
  auto& state=states_[row];if(state.slot<0)return true;
  const auto slot=static_cast<std::size_t>(state.slot);auto* sound=logical_.sound(slot);
  if(!sound||logical_.stop233670(*sound)!=p::LogicalAudioOwner::Result::complete)return false;
  assigned_[slot]=-1;state.slot=-1;++r.stopped;return true;
 }
public:
 // Presence (common course rule): the positional / ambient owner exists
 // exactly when the course AUB's [Sounds] authors NumSoulTreeSounds or
 // NumAmbientSounds (absent field = 0). Stock GB: 49 + 4.
 static bool authoredSounds(const gb_cold_sources50LA::Owner& source,std::size_t& soulTree,std::size_t& ambient,
     std::string& error)noexcept {
  soulTree=ambient=0;
  try{
   // ONE AUB reader for every course (0x279910 / 0x2793A0 / 0x2787B0).
   std::vector<std::uint8_t> raw;a::DecodeError48C de;a::ParameterDocument48F doc;
   if(!source.ready()){error="GB course AUB unavailable";return false;}
   if(!readAub(source,raw,de))return true; // no AUB member: no sounds
   if(!a::decodeParameters48F(raw,doc,de)){error="GB course AUB rejected";return false;}
   auto read=std::make_unique<a::OriginalPositionalSounds50SR>();
   if(!a::readOriginalPositionalSounds50SR(doc,"positional_GB_count",*read,error))return false;
   soulTree=read->soulTree.live;ambient=read->ambient.live;return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
   catch(...){return false;}
 }
 Owner(){assigned_.fill(-1);}Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 ~Owner(){if(!closed_&&!close())std::terminate();}
 bool prepare(std::shared_ptr<const gb_cold_sources50LA::Owner> source,
     const material_audio50HI::Provider& material,const std::filesystem::path& audioRoot,
     p::AudioMixer50HD& mixer,p::RaceAudioGroups50IC& groups,std::string& error){
  namespace d=pod_materials50ed_fixture::detail;
  if(attempted_){error="GB positional sound requires fresh owner";return false;}attempted_=true;
  try{
   d::need(thread_==std::this_thread::get_id()&&source&&source->ready()&&material.prepared()&&
      material.gbSourceIdentity()==source.get()&&material.collisionRowCount()==20&&material.driveRowCount()==20,
      "GB positional sound requires actual same-source20/20 material owner");
   d::need(groups.uses(mixer)&&mixer.count()==p::audioMaximumVoices50HD&&mixer.originalSoundMode50KG(),
      "GB actual mode/mixer/groups unavailable");
   for(std::size_t i=0;i<voiceCount;++i)d::need(!mixer.active(firstVoice+i)&&!mixer.hasVoiceGroupGain50IC(firstVoice+i)&&
      groups.binding(firstVoice+i)==g::RaceAudioGroup50IC::Unrepresented&&groups.canBind(firstVoice+i,g::RaceAudioGroup50IC::World4D8),
      "GB positional pool conflicts with actual owner");
   // ONE AUB reader for every course (original 0x279910 / 0x2793A0 /
   // 0x2787B0, positional_sound_rows50sr.h); the stock AUB identity, row and
   // PCM counts are receipts. GB retains its ambient list itself.
   std::vector<std::uint8_t> raw;a::DecodeError48C de;
   const bool aub=readAub(*source,raw,de);
   std::vector<a::AudioRequest50HC> requests;a::OriginalSoundList50SR<a::ambientSoundCapacity50SR> ambient;
   count_=positional_sound_rows50SR::project(aub?&raw:nullptr,stock,rows_,requests,&ambient);
   course_archive_receipt50CI::countReceipt("positional_GB","ambient_sounds",ambient.live,stockAmbient);
   d::need(ambient.live<=ambient_.size(),"GB ambient list exceeds the retained ambient store");
   for(std::size_t i=0;i<ambient.live;++i){const auto& s=ambient.rows[i];auto& row=ambient_[i];
    row.name=s.resource;for(auto& c:row.name)if(c=='\\')c='/';row.name+=".vag";
    row.cycleSeconds=s.cycleTime;row.chance=s.randomTriggerPercent;
    if(std::find_if(requests.begin(),requests.end(),[&](const auto& q){return q.name==row.name;})==requests.end())
     requests.push_back({row.name,false,false,false});}
   ambientCount_=ambient.live;
   std::cerr<<"gb_native_audio soultree_rows="<<count_<<" ambient_rows="<<ambientCount_
       <<" ambient_scheduling=unsupported native_transport_slots="<<voiceCount<<"\n";
   // Absent soul-tree data = no positional transport (ambient rows are retained
   // only; PORT STAND-IN: no ambient scheduler). The caller drops the owner.
   if(!count_){source_=std::move(source);error.clear();return true;}
   d::need(count_<=rows_.size()&&a::loadAudioBank50HC(audioRoot,requests,bank_,error),"GB positional/retained ambient PCM bank rejected");
   std::size_t stockClips=0;for(const auto& clip:bank_.clips)stockClips+=gb_audio_originals50GB::authentic(clip);
   std::cerr<<"positional_pcm site=positional_GB clips="<<bank_.clips.size()<<" stock="<<(stockClips==bank_.clips.size()?1:0)<<"\n";
   for(std::size_t i=0;i<count_;++i){const auto* clip=bank_.find(rows_[i].name);
    d::need(clip,"GB actual positional PCM unavailable");const auto& pcm=clip->pcm;
    clips_[i]={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
    d::need(p::AudioMixer50HD::validClip(clips_[i]),"GB positional PCM invalid");}
   authored_=true;
   source_=std::move(source);mixer_=&mixer;groups_=&groups;epoch_=mixer.groupEpoch50IC();
   d::need(logical_.bind(mixer,firstVoice,voiceCount)&&groups.bindRange(firstVoice,voiceCount,g::RaceAudioGroup50IC::World4D8),"GB actual positional transport bind rejected");
   // Each reusable transport is registered with actual PCM. No playback starts
   // here; service assigns the actual selected descriptor clip after retirement.
   for(std::size_t i=0;i<voiceCount;++i)d::need(logical_.registerClip(i,clips_[0],true),"GB positional slot registration rejected");
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){blocked_=true;error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_&&!blocked_&&!closed_&&current();}
 // This owner's World4D8 transport, for the Results 26D070 typed stop claim only.
 p::LogicalAudioOwner& resultsWorldTransport26D070()noexcept{return logical_;}
 const gb_cold_sources50LA::Owner* sourceIdentity()const noexcept{return source_.get();}
 // False when the AUB authors no soul-tree sound: no transport (caller drops it).
 bool authored()const noexcept{return authored_;}
 std::size_t descriptorCount()const noexcept{return count_;}
 std::size_t ambientCount()const noexcept{return ambientCount_;}
 const std::array<Descriptor,descriptorCapacity>& descriptors()const noexcept{return rows_;}
 const a::AudioBank50HC& bank()const noexcept{return bank_;}
 const std::array<AmbientDescriptor,ambientCapacity>& ambientDescriptors()const noexcept{return ambient_;}
 static constexpr bool ambientSchedulingSupported()noexcept{return false;}
 Receipt service(const g::OriginalAiAudioListener* listeners,std::size_t count,
     g::CoupledNoiseContext49M& shared)noexcept;
 bool close()noexcept{
  if(closed_)return true;if(thread_!=std::this_thread::get_id())return false;
  // A foreign revision may still borrow owned PCM. Do not free it or stop it
  // on behalf of another owner; require that actual owner to retire first.
  if(mixer_)for(std::size_t slot=0;slot<mixer_->count();++slot)if(mixer_->active(slot)){
   p::AudioVoiceObservation50HD observed{};if(!mixer_->inspectVoice(slot,observed))return false;
   bool borrows=false;for(const auto& clip:bank_.clips)if(observed.voice.clip.samples==clip.pcm.samples.data())borrows=true;
   if(borrows){const auto local=slot>=firstVoice&&slot<firstVoice+voiceCount?slot-firstVoice:voiceCount;
    const auto* owned=local<voiceCount?logical_.sound(local):nullptr;if(!owned||!logical_.active(*owned))return false;}
  }
  logical_.close();ready_=false;closed_=true;assigned_.fill(-1);for(auto& s:states_)s.slot=-1;return true;
 }
};

inline Receipt Owner::service(const g::OriginalAiAudioListener* listeners,std::size_t count,
    g::CoupledNoiseContext49M& shared)noexcept{
 using namespace g::original_ai_aural_detail;
 Receipt r;if(!ready()||count>4||(count&&!listeners)||std::fegetround()!=FE_TONEAREST)return r;
 const auto* actualMode=mixer_->originalSoundMode50KG();if(!actualMode||!actualMode->ready())return r;
 const auto& mode=actualMode->fields();if(!g::validOriginalSoundModeFields50KG(mode))return r;
 for(std::size_t i=0;i<count;++i)for(unsigned axis=0;axis<3;++axis)
  if(!std::isfinite(listeners[i].position[axis])||!std::isfinite(listeners[i].forward[axis]))return r;
 const auto* paused=groups_->sourceState44(g::RaceAudioGroup50IC::World4D8);
 if(!paused)return r;
 // Native pause freezes this adapter's scheduling and source RNG chronology;
 // the actual World4D8 mixer layer also pauses the retained playback cursors.
 if(*paused){r.complete=true;return r;}
 auto fail=[&](){blocked_=true;return r;};
 std::array<float,descriptorCapacity> distances{};std::array<std::size_t,descriptorCapacity> nearest{};
 std::array<bool,descriptorCapacity> eligible{};
 // Actual291350 shape: evaluate every descriptor before starting/stopping any.
 for(std::size_t i=0;i<count_;++i){const auto& row=rows_[i];auto& state=states_[i];
  float best=std::numeric_limits<float>::max();
  for(std::size_t l=0;l<count;++l){const float d=distance(row.position[0],row.position[2],listeners[l].position[0],listeners[l].position[2]);
   if(!std::isfinite(d))return fail();if(d<best){best=d;nearest[i]=l;}}
  distances[i]=best;eligible[i]=row.oneShot?best<=row.triggerDistance:best<mul(literal(0x3f99999au),row.farDistance);
  if(eligible[i])++r.eligible;
  else if(row.oneShot){if(row.resetWhenInaudible)state.played=false;state.randomAttempted=false;}
 }
 // Release completed/ineligible actual transport before deterministic admission.
 for(std::size_t i=0;i<count_;++i){auto& state=states_[i];if(state.slot<0)continue;
  auto* sound=logical_.sound(static_cast<std::size_t>(state.slot));if(!sound)return fail();
  const bool playing=logical_.active(*sound);
  // Foreign playback is never treated as a free owned transport.
  if(mixer_->active(firstVoice+static_cast<std::size_t>(state.slot))&&!playing)return fail();
  if(!eligible[i]||!playing){if(!eligible[i]&&playing)state.randomAttempted=false;if(!release(i,r))return fail();}
 }
 for(std::size_t i=0;i<count_;++i){const auto& row=rows_[i];auto& state=states_[i];
  if(!eligible[i]||state.slot>=0||(row.oneShot&&state.played))continue;
  // A latched unsuccessful random attempt does not create a playback.
  if(row.chance<100&&state.randomAttempted)continue;
  std::size_t slot=0;while(slot<voiceCount&&assigned_[slot]>=0)++slot;
  if(slot==voiceCount){++r.deferred;continue;}
  if(mixer_->active(firstVoice+slot))return fail();
  if(row.chance<100){state.randomAttempted=true;const auto random=g::drawOriginalSharedRng50IU(shared);++r.randomDraws;
   const float sampled=mul(100.f,mul(0x1p-31f,static_cast<float>(random)));if(!(sampled<=row.chance))continue;}
  auto* sound=logical_.sound(slot);if(!sound)return fail();
  sound->clip=clips_[i];sound->available=true;sound->loop1B0=row.loop?1u:0u;sound->position60=row.position;
  sound->volume38=0;sound->volume44=0;sound->pitch40=4096;sound->pitch4C=4096;
  sound->angle3C=0;sound->angle48=0;sound->multiplier54=1;sound->multiplier58=1;
  if(row.oneShot)state.played=true;
  const auto started=g::startAiLogicalSound233410(logical_,*sound,0);
  if(started.status!=g::AiLogicalSoundStatus::complete||!started.sourceReturned)return fail();
  sound->volume38=100;assigned_[slot]=static_cast<int>(i);state.slot=static_cast<int>(slot);++r.started;
 }
 for(std::size_t i=0;i<count_;++i){const auto& row=rows_[i];const auto& state=states_[i];
  if(state.slot<0||!count)continue;
  auto* sound=logical_.sound(static_cast<std::size_t>(state.slot));if(!sound||!logical_.active(*sound))return fail();
  const auto& listener=listeners[nearest[i]];const float d=distances[i];
  const float attenuation=d>=row.farDistance?0.f:d<=row.nearDistance?100.f:sub(100.f,mul(100.f,div(sub(d,row.nearDistance),sub(row.farDistance,row.nearDistance))));
  float forward=0,target=0;
  if(!g::originalAngle48G(listener.forward[2],listener.forward[0],forward)||
     !g::originalAngle48G(sub(row.position[2],listener.position[2]),sub(row.position[0],listener.position[0]),target))return fail();
  const float degrees=literal(0x42652ee1u);const float f=mul(degrees,forward),t=mul(degrees,target);
  if(!std::isfinite(f)||!std::isfinite(t)||f<=-2147483648.f||f>=2147483648.f||t<=-2147483648.f||t>=2147483648.f)return fail();
  auto angle=static_cast<std::int64_t>(static_cast<std::int32_t>(t))-static_cast<std::int32_t>(f);
  if(angle>180)angle-=359;else if(angle< -180)angle+=359;
  if(angle< -360||angle>360)return fail();
  const auto applied=g::spatialAiLogicalSound233AE0(logical_,*sound,mode,attenuation,static_cast<std::int32_t>(angle),listener.position,false);
  if(applied.status!=g::AiLogicalSoundStatus::complete)return fail();++r.controlled;
 }
 r.complete=true;return r;
}
} // namespace gb_positional_sound50GB
