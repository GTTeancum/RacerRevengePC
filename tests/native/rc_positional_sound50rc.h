#pragma once
#include "material_audio_provider50hi_fixture.h"
#include "rc_audio_originals50rc.h"
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

namespace rc_positional_sound50RC {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
inline constexpr std::size_t descriptorCapacity=a::soulTreeSoundCapacity50SR,firstVoice=236,voiceCount=28;
static_assert(firstVoice==p::originalMaterialFirstVoice50HD+16);
static_assert(firstVoice+voiceCount==p::originalMaterialFirstVoice50HD+p::originalMaterialRowsPerRacer50HD);
struct Descriptor {
 std::string name;std::array<float,4> position{{0,0,0,1}};
 float nearDistance=0,farDistance=0,triggerDistance=20,chance=100;
 bool loop=false,oneShot=false,resetWhenInaudible=false;
};
// Stock values of this slot: receipts only (positional_sound_rows50sr.h).
inline constexpr positional_sound_rows50SR::Stock stock{"RC","tracks\\rc.aub",47432u,"8b34867bb6e76e1ddce72ac65c6c4548068d52d30a53b4ca08af87bfe3ca1851",120u,13u};
struct State {bool played=false,randomAttempted=false;int slot=-1;};
struct Receipt {bool complete=false;std::size_t eligible=0,started=0,stopped=0,controlled=0,deferred=0,randomDraws=0;};
// Native fixed-position adapter for the course AUB's SoulTree list (the common
// reader's live rows; stock RC authors 120), distinct from the exclusive transport slots.
// The material rows are not admission pins: a material owner that grew into
// this pool is refused by the pool-conflict check below.
// Native policy: retain active eligible sounds, then use the lowest free slot
// in authored descriptor order; never steal an active sound. Deferred rows do
// not draw RNG or change played/random latches. No source hardware parity claim.
class Owner final {
 std::shared_ptr<const rc_cold_sources50LA::Owner> source_;
 a::AudioBank50HC bank_;
 std::array<Descriptor,descriptorCapacity> rows_{};
 std::array<p::AudioClip50HD,descriptorCapacity> clips_{};
 std::array<State,descriptorCapacity> states_{};
 std::array<int,voiceCount> assigned_{};
 p::LogicalAudioOwner logical_;
 p::AudioMixer50HD* mixer_=nullptr;p::RaceAudioGroups50IC* groups_=nullptr;
 std::thread::id thread_=std::this_thread::get_id();std::uint64_t epoch_=0;
 bool attempted_=false,ready_=false,blocked_=false,closed_=false,authored_=false;
 std::size_t count_=0; // live descriptors rows_[0,count_) from the common AUB reader
 static const std::string* field(const a::ParameterBlock48F& b,const char* key){
  const std::string* result=nullptr;for(const auto& f:b.fields)if(f.name==key){if(result)throw std::runtime_error("RC duplicate sound field");result=&f.value;}return result;
 }
 static float number(const a::ParameterBlock48F& b,const char* key,float fallback){
  const auto* v=field(b,key);if(!v)return fallback;float n=0;
  if(!a::parseTrackNumber50AD(v->data(),v->size(),n)||!std::isfinite(n))throw std::runtime_error("RC sound number rejected");return n;
 }
 static bool flag(const a::ParameterBlock48F& b,const char* key){
  const auto* v=field(b,key);if(!v)return false;if(*v=="T"||*v=="t"||*v=="1")return true;
  if(*v=="F"||*v=="f"||*v=="0")return false;throw std::runtime_error("RC sound flag rejected");
 }
 bool current()const noexcept {
  if(thread_!=std::this_thread::get_id()||!source_||!source_->ready()||!mixer_||!groups_||
     !groups_->uses(*mixer_)||mixer_->groupEpoch50IC()!=epoch_)return false;
  for(std::size_t i=0;i<voiceCount;++i)if(groups_->binding(firstVoice+i)!=g::RaceAudioGroup50IC::World4D8)return false;
  return true;
 }
 bool release(std::size_t row,Receipt& r)noexcept {
  auto& state=states_[row];if(state.slot<0)return true;
  const auto slot=static_cast<std::size_t>(state.slot);auto* sound=logical_.sound(slot);
  if(!sound||logical_.stop233670(*sound)!=p::LogicalAudioOwner::Result::complete)return false;
  assigned_[slot]=-1;state.slot=-1;++r.stopped;return true;
 }
public:
 Owner(){assigned_.fill(-1);}Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 ~Owner(){if(!closed_&&!close())std::terminate();}
 bool prepare(std::shared_ptr<const rc_cold_sources50LA::Owner> source,
     const material_audio50HI::Provider& material,const std::filesystem::path& audioRoot,
     p::AudioMixer50HD& mixer,p::RaceAudioGroups50IC& groups,std::string& error){
  namespace d=pod_materials50ed_fixture::detail;
  if(attempted_){error="RC positional sound requires fresh owner";return false;}attempted_=true;
  try{
   d::need(thread_==std::this_thread::get_id()&&source&&source->ready()&&material.prepared()&&
      material.rcSourceIdentity()==source.get(),
      "RC positional sound requires the actual same-source material owner");
   d::need(groups.uses(mixer)&&mixer.count()==p::audioMaximumVoices50HD&&mixer.originalSoundMode50KG(),
      "RC actual mode/mixer/groups unavailable");
   for(std::size_t i=0;i<voiceCount;++i)d::need(!mixer.active(firstVoice+i)&&!mixer.hasVoiceGroupGain50IC(firstVoice+i)&&
      groups.binding(firstVoice+i)==g::RaceAudioGroup50IC::Unrepresented&&groups.canBind(firstVoice+i,g::RaceAudioGroup50IC::World4D8),
      "RC positional pool conflicts with actual owner");
   // ONE AUB reader for every course (original 0x279910 / 0x2793A0 /
   // 0x2787B0); the stock AUB identity and counts are receipts.
   std::vector<std::uint8_t> raw;a::DecodeError48C de;
   const bool aub=source->extractMember("tracks\\rc.aub",raw,de,positional_sound_rows50SR::maximumAubBytes);
   std::vector<a::AudioRequest50HC> requests;
   count_=positional_sound_rows50SR::project(aub?&raw:nullptr,stock,rows_,requests);
   // Absent data = no positional owner: nothing is bound; the caller drops it.
   if(!count_){source_=std::move(source);error.clear();return true;}
   d::need(count_<=rows_.size()&&a::loadAudioBank50HC(audioRoot,requests,bank_,error),"RC positional PCM bank rejected");
   std::size_t stockClips=0;for(const auto& clip:bank_.clips)stockClips+=rc_audio_originals50RC::authentic(clip);
   std::cerr<<"positional_pcm site=positional_RC clips="<<bank_.clips.size()<<" stock="<<(stockClips==bank_.clips.size()?1:0)<<"\n";
   for(std::size_t i=0;i<count_;++i){const auto* clip=bank_.find(rows_[i].name);
    d::need(clip,"RC actual positional PCM unavailable");const auto& pcm=clip->pcm;
    clips_[i]={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
    d::need(p::AudioMixer50HD::validClip(clips_[i]),"RC positional PCM invalid");}
   authored_=true;
   source_=std::move(source);mixer_=&mixer;groups_=&groups;epoch_=mixer.groupEpoch50IC();
   d::need(logical_.bind(mixer,firstVoice,voiceCount)&&groups.bindRange(firstVoice,voiceCount,g::RaceAudioGroup50IC::World4D8),"RC actual positional transport bind rejected");
   // Each reusable transport is registered with actual PCM. No playback starts
   // here; service assigns the actual selected descriptor clip after retirement.
   for(std::size_t i=0;i<voiceCount;++i)d::need(logical_.registerClip(i,clips_[0],true),"RC positional slot registration rejected");
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){blocked_=true;error=e.what();return false;}
 }
 // False when the AUB authors no soul-tree sound: no owner (caller drops it).
 bool authored()const noexcept{return authored_;}
 std::size_t descriptorCount()const noexcept{return count_;}
 bool ready()const noexcept{return ready_&&!blocked_&&!closed_&&current();}
 // This owner's World4D8 transport, for the Results 26D070 typed stop claim only.
 p::LogicalAudioOwner& resultsWorldTransport26D070()noexcept{return logical_;}
 const rc_cold_sources50LA::Owner* sourceIdentity()const noexcept{return source_.get();}
 const std::array<Descriptor,descriptorCapacity>& descriptors()const noexcept{return rows_;}
 const a::AudioBank50HC& bank()const noexcept{return bank_;}
 Receipt service(const g::OriginalAiAudioListener* listeners,std::size_t count,
     g::CoupledNoiseContext49M& shared)noexcept;
 bool close()noexcept{
  if(closed_)return true;if(thread_!=std::this_thread::get_id())return false;
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
  if(state.slot<0||!count||!(distances[i]<row.farDistance))continue;
  auto* sound=logical_.sound(static_cast<std::size_t>(state.slot));if(!sound||!logical_.active(*sound))return fail();
  const auto& listener=listeners[nearest[i]];const float d=distances[i];
  const float attenuation=d<=row.nearDistance?100.f:sub(100.f,mul(100.f,div(sub(d,row.nearDistance),sub(row.farDistance,row.nearDistance))));
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
} // namespace rc_positional_sound50RC
