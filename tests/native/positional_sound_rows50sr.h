#pragma once
// ONE positional-sound owner for every course (PLAN B-C17, W0 1.2 NEW type
// positional_sound_rows50SR::Owner). It replaces the per-course gb/mc/rc/tb
// positional-sound owners and the trigger-driven gb/tb pod sounds and TC global
// sound adapters (the contexts no longer carry them).
//
// Original: the course AUB's [Sounds] lists, read by the ONE common reader
// (src/assets/original_positional_sounds50sr.h): 0x279910 SoultreeSounds::Load,
// 0x2793A0 AmbientSounds::Load, 0x2787B0/0x278B80/0x278D70 descriptor fields.
// Service follows the 291350 shape (evaluate every descriptor, release, then
// admit). Nothing here names a course; the course code is data (log lines and
// receipts). Presence: the owner is authored exactly when the AUB authors
// NumSoultreeSounds > 0 (absent field = 0); ambient rows are retained.
//
// Storage: descriptor, clip and state rows are sized ONCE from the live
// soul-tree count through common_track50CR::Block (BlockId::positionalSounds);
// every write is bounds-checked first.
#include "common_track50cr.h"
#include "material_audio_provider50hi_fixture.h"
#include "../../src/assets/original_positional_sounds50sr.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include "../../src/game/original_ai_aural.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace positional_sound_rows50SR {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;namespace ct=common_track50CR;
static_assert(a::positionalSoundCeiling50SR==4096u,"reader ceiling equals the positional_sounds block row");
// Native transport pool. PORT MAPPING (flagged): the original's sound owners
// are not host mixer voices; the port gives every course's positional sounds
// the one pool 128..151 (24 reusable transports), clear of the material base
// 64..79, GC/Gate 118..126, gameplay 152..158 and the material extra block
// 192..219. The per-course pools it replaces (128x24 GB/MC/TB, 236x28 RC) are gone.
inline constexpr std::size_t firstVoice=128,voiceCount=24;
static_assert(firstVoice>126&&firstVoice+voiceCount<=152);
static_assert(firstVoice+voiceCount<=material_audio50HI::extraFirstVoice);

struct Descriptor {
 std::string name;std::array<float,4> position{{0,0,0,1}};
 float nearDistance=0,farDistance=0,triggerDistance=20,chance=100;
 bool loop=false,oneShot=false,resetWhenInaudible=false;
};
struct AmbientDescriptor {std::string name;float cycleSeconds=0,chance=0;};
struct State {bool played=false,randomAttempted=false;int slot=-1;};
struct Receipt {bool complete=false;std::size_t eligible=0,started=0,stopped=0,controlled=0,deferred=0,randomDraws=0;};

class Owner final {
 const ct::Track* track_=nullptr;           // identity only (the race's one Track)
 a::AudioBank50HC bank_;
 ct::Block<Descriptor> rows_;
 ct::Block<p::AudioClip50HD> clips_;
 ct::Block<State> states_;
 std::vector<AmbientDescriptor> ambient_;   // retained only (no ambient scheduler)
 std::size_t count_=0,ambientCount_=0;
 std::array<int,voiceCount> assigned_{};
 p::LogicalAudioOwner logical_;
 p::AudioMixer50HD* mixer_=nullptr;p::RaceAudioGroups50IC* groups_=nullptr;
 std::thread::id thread_=std::this_thread::get_id();std::uint64_t epoch_=0;
 bool attempted_=false,ready_=false,blocked_=false,closed_=false,authored_=false;
 static void need(bool value,const std::string& text){if(!value)throw std::runtime_error(text);}
 bool current()const noexcept {
  if(thread_!=std::this_thread::get_id()||!track_||!mixer_||!groups_||
     !groups_->uses(*mixer_)||mixer_->groupEpoch50IC()!=epoch_)return false;
  for(std::size_t i=0;i<voiceCount;++i)if(groups_->binding(firstVoice+i)!=g::RaceAudioGroup50IC::World4D8)return false;
  return true;
 }
public:
 Owner(){assigned_.fill(-1);}Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 ~Owner(){if(!closed_&&!close())std::terminate();}
 // 05.12: read the Track's AUB [Sounds] lists, load their PCM and bind the
 // native transport pool. An AUB without soul-tree sounds leaves the owner
 // ready()==false, authored()==false (no transport, no voices bound).
 bool prepare(const ct::Track& track,const material_audio50HI::Provider& material,
     const std::filesystem::path& audioRoot,p::AudioMixer50HD& mixer,p::RaceAudioGroups50IC& groups,std::string& error){
  if(attempted_){error="positional sound requires fresh owner";return false;}attempted_=true;
  try{
   need(thread_==std::this_thread::get_id()&&material.prepared(),"positional sound requires the prepared course material owner");
   need(groups.uses(mixer)&&mixer.count()==p::audioMaximumVoices50HD&&mixer.originalSoundMode50KG(),
      "positional sound: actual mode/mixer/groups unavailable");
   track_=&track;
   const auto code=std::string(track.code());
   const auto& aub=track.documents.aub;
   if(aub.empty()){std::cerr<<"positional_sounds site=common course="<<code<<" aub=absent soultree_read=0\n";error.clear();return true;}
   a::ParameterDocument48F doc;a::DecodeError48C de{};
   need(aub.size()<=2u*1024u*1024u&&a::decodeParameters48F(aub,doc,de),"positional sound AUB authoring rejected");
   auto read=std::make_unique<a::OriginalPositionalSounds50SR>();
   need(a::readOriginalPositionalSounds50SR(doc,"common",*read,error),error);
   const auto& list=read->soulTree;
   count_=list.live;ambientCount_=read->ambient.live;
   // Stores sized once from the live counts read from this AUB.
   need(rows_.size(ct::BlockId::positionalSounds,count_,0x279910u)&&
        clips_.size(ct::BlockId::positionalSounds,count_,0x279910u)&&
        states_.size(ct::BlockId::positionalSounds,count_,0x279910u),"positional sound store sizing refused");
   std::vector<a::AudioRequest50HC> requests;
   for(std::size_t i=0;i<list.live;++i){const auto& s=list.rows[i];
    const auto label="SoultreeSound"+std::to_string(s.ordinal)+" ";
    // PORT STAND-IN (flagged): attached, 2D, stereo or start-disabled soul-tree
    // sounds are not implemented by the fixed-position native adapter; refused.
    need(s.attachToObject.empty()&&!s.force2D&&!s.stereo&&!s.startDisabled,
       label+"AttachToObject/Force2D/Stereo/StartDisabled is not supported natively (PORT STAND-IN)");
    // PORT STAND-IN (flagged): the original default Position (0x442F80) is
    // not traced; an omitted Position is refused.
    need(s.positionAuthored,label+"omitted Position is not supported natively (PORT STAND-IN)");
    auto* row=rows_.slot(i,0x2787B0u);need(row!=nullptr,"positional sound row refused");
    // PORT MAPPING (flagged): the original opens resource+".wav" (308FB8); the
    // port's audio bank holds the disc's VAG file of the same name.
    *row=Descriptor{};row->name=s.resource;for(auto& c:row->name)if(c=='\\')c='/';row->name+=".vag";
    for(unsigned k=0;k<3;++k)row->position[k]=s.position[k];
    row->loop=s.looping!=0;row->oneShot=s.oneShot!=0;row->resetWhenInaudible=s.resetWhenInaudible!=0;
    row->nearDistance=s.minDistance;row->farDistance=s.maxDistance;row->triggerDistance=s.oneShotDistance;row->chance=s.randomTriggerPercent;
    need(row->nearDistance>=0&&row->nearDistance<row->farDistance&&row->triggerDistance>=0&&row->chance>=0&&row->chance<=100,
       label+"ranges rejected");
    auto found=std::find_if(requests.begin(),requests.end(),[&](const auto& q){return q.name==row->name;});
    if(found==requests.end())requests.push_back({row->name,row->loop,false,false});
    else need(found->loop==row->loop,"positional PCM delivery flags conflict");
    if(!states_.slot(i,0x2787B0u))throw std::runtime_error("positional state row refused");
   }
   ambient_.resize(ambientCount_);
   for(std::size_t i=0;i<ambientCount_;++i){const auto& s=read->ambient.rows[i];auto& row=ambient_[i];
    row.name=s.resource;for(auto& c:row.name)if(c=='\\')c='/';row.name+=".vag";
    row.cycleSeconds=s.cycleTime;row.chance=s.randomTriggerPercent;
    if(std::find_if(requests.begin(),requests.end(),[&](const auto& q){return q.name==row.name;})==requests.end())
     requests.push_back({row.name,false,false,false});}
   std::cerr<<"positional_sounds site=common course="<<code<<" soultree_rows="<<count_<<" ambient_rows="<<ambientCount_
       <<" pcm="<<requests.size()<<" ambient_scheduling=unsupported native_transport_slots="<<voiceCount<<"\n";
   // Absent soul-tree data = no positional transport (ambient rows are retained
   // only; PORT STAND-IN, flagged: no ambient scheduler 0x2793A0/0x278D70).
   if(!count_){error.clear();return true;}
   for(std::size_t i=0;i<voiceCount;++i)need(!mixer.active(firstVoice+i)&&!mixer.hasVoiceGroupGain50IC(firstVoice+i)&&
      groups.binding(firstVoice+i)==g::RaceAudioGroup50IC::Unrepresented&&groups.canBind(firstVoice+i,g::RaceAudioGroup50IC::World4D8),
      "positional pool conflicts with an actual owner");
   need(a::loadAudioBank50HC(audioRoot,requests,bank_,error),"positional/retained ambient PCM bank rejected");
   for(std::size_t i=0;i<count_;++i){const auto* clip=bank_.find(rows_.at(i)->name);
    need(clip!=nullptr,"positional PCM unavailable");const auto& pcm=clip->pcm;
    const p::AudioClip50HD value{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,pcm.hasEmbeddedLoop?pcm.loopStart:0,pcm.hasEmbeddedLoop?pcm.loopEnd:0};
    need(p::AudioMixer50HD::validClip(value)&&clips_.write(i,value,0x2787B0u),"positional PCM invalid");}
   authored_=true;
   mixer_=&mixer;groups_=&groups;epoch_=mixer.groupEpoch50IC();
   need(logical_.bind(mixer,firstVoice,voiceCount)&&groups.bindRange(firstVoice,voiceCount,g::RaceAudioGroup50IC::World4D8),"positional transport bind rejected");
   // Each reusable transport is registered with actual PCM. No playback starts
   // here; service assigns the selected descriptor's clip after retirement.
   for(std::size_t i=0;i<voiceCount;++i)need(logical_.registerClip(i,*clips_.at(0),true),"positional slot registration rejected");
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){blocked_=true;try{error=e.what();}catch(...){}return false;}
 }
 bool ready()const noexcept{return ready_&&!blocked_&&!closed_&&current();}
 // This owner's World4D8 transport, for the Results 26D070 typed stop claim only.
 p::LogicalAudioOwner& resultsWorldTransport26D070()noexcept{return logical_;}
 const ct::Track* trackIdentity()const noexcept{return track_;}
 // False when the AUB authors no soul-tree sound: no transport.
 bool authored()const noexcept{return authored_;}
 std::size_t descriptorCount()const noexcept{return count_;}
 std::size_t ambientCount()const noexcept{return ambientCount_;}
 std::span<const Descriptor> descriptors()const noexcept{return rows_.live();}
 std::span<const AmbientDescriptor> ambientDescriptors()const noexcept{return {ambient_.data(),ambient_.size()};}
 const a::AudioBank50HC& bank()const noexcept{return bank_;}
 static constexpr bool ambientSchedulingSupported()noexcept{return false;}
 bool release(std::size_t row,Receipt& r)noexcept {
  auto* state=states_.at(row);if(!state)return false;if(state->slot<0)return true;
  const auto slot=static_cast<std::size_t>(state->slot);auto* sound=logical_.sound(slot);
  if(!sound||logical_.stop233670(*sound)!=p::LogicalAudioOwner::Result::complete)return false;
  assigned_[slot]=-1;state->slot=-1;++r.stopped;return true;
 }
 Receipt service(const g::OriginalAiAudioListener* listeners,std::size_t count,
     g::CoupledNoiseContext49M& shared)noexcept;
 bool close()noexcept{
  if(closed_)return true;if(thread_!=std::this_thread::get_id())return false;
  // A foreign revision may still borrow owned PCM. Do not free it or stop it
  // on behalf of another owner; require that owner to retire first.
  if(mixer_)for(std::size_t slot=0;slot<mixer_->count();++slot)if(mixer_->active(slot)){
   p::AudioVoiceObservation50HD observed{};if(!mixer_->inspectVoice(slot,observed))return false;
   bool borrows=false;for(const auto& clip:bank_.clips)if(observed.voice.clip.samples==clip.pcm.samples.data())borrows=true;
   if(borrows){const auto local=slot>=firstVoice&&slot<firstVoice+voiceCount?slot-firstVoice:voiceCount;
    const auto* owned=local<voiceCount?logical_.sound(local):nullptr;if(!owned||!logical_.active(*owned))return false;}
  }
  logical_.close();ready_=false;closed_=true;assigned_.fill(-1);for(auto& s:states_.live())s.slot=-1;return true;
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
 // the World4D8 mixer layer also pauses the retained playback cursors.
 if(*paused){r.complete=true;return r;}
 auto fail=[&](){blocked_=true;return r;};
 const auto rows=rows_.live();auto states=states_.live();
 if(rows.size()!=count_||states.size()!=count_)return fail();
 // Per-service scratch, sized from the live count (no fixed capacity).
 std::vector<float> distances;std::vector<std::size_t> nearest;std::vector<unsigned char> eligible;
 try{distances.assign(count_,0.f);nearest.assign(count_,0);eligible.assign(count_,0);}catch(...){return fail();}
 // 291350 shape: evaluate every descriptor before starting/stopping any.
 for(std::size_t i=0;i<count_;++i){const auto& row=rows[i];auto& state=states[i];
  float best=std::numeric_limits<float>::max();
  for(std::size_t l=0;l<count;++l){const float d=distance(row.position[0],row.position[2],listeners[l].position[0],listeners[l].position[2]);
   if(!std::isfinite(d))return fail();if(d<best){best=d;nearest[i]=l;}}
  distances[i]=best;eligible[i]=row.oneShot?best<=row.triggerDistance:best<mul(literal(0x3f99999au),row.farDistance);
  if(eligible[i])++r.eligible;
  else if(row.oneShot){if(row.resetWhenInaudible)state.played=false;state.randomAttempted=false;}
 }
 // Release completed/ineligible transport before deterministic admission.
 for(std::size_t i=0;i<count_;++i){auto& state=states[i];if(state.slot<0)continue;
  auto* sound=logical_.sound(static_cast<std::size_t>(state.slot));if(!sound)return fail();
  const bool playing=logical_.active(*sound);
  // Foreign playback is never treated as a free owned transport.
  if(mixer_->active(firstVoice+static_cast<std::size_t>(state.slot))&&!playing)return fail();
  if(!eligible[i]||!playing){if(!eligible[i]&&playing)state.randomAttempted=false;if(!release(i,r))return fail();}
 }
 // Native policy: retain active eligible sounds, then the lowest free slot in
 // authored descriptor order; never steal an active sound. Deferred rows do
 // not draw RNG or change played/random latches.
 for(std::size_t i=0;i<count_;++i){const auto& row=rows[i];auto& state=states[i];
  if(!eligible[i]||state.slot>=0||(row.oneShot&&state.played))continue;
  // A latched unsuccessful random attempt does not create a playback.
  if(row.chance<100&&state.randomAttempted)continue;
  std::size_t slot=0;while(slot<voiceCount&&assigned_[slot]>=0)++slot;
  if(slot==voiceCount){++r.deferred;continue;}
  if(mixer_->active(firstVoice+slot))return fail();
  if(row.chance<100){state.randomAttempted=true;const auto random=g::drawOriginalSharedRng50IU(shared);++r.randomDraws;
   const float sampled=mul(100.f,mul(0x1p-31f,static_cast<float>(random)));if(!(sampled<=row.chance))continue;}
  auto* sound=logical_.sound(slot);if(!sound)return fail();
  const auto* clip=clips_.at(i);if(!clip)return fail();
  sound->clip=*clip;sound->available=true;sound->loop1B0=row.loop?1u:0u;sound->position60=row.position;
  sound->volume38=0;sound->volume44=0;sound->pitch40=4096;sound->pitch4C=4096;
  sound->angle3C=0;sound->angle48=0;sound->multiplier54=1;sound->multiplier58=1;
  if(row.oneShot)state.played=true;
  const auto started=g::startAiLogicalSound233410(logical_,*sound,0);
  if(started.status!=g::AiLogicalSoundStatus::complete||!started.sourceReturned)return fail();
  sound->volume38=100;assigned_[slot]=static_cast<int>(i);state.slot=static_cast<int>(slot);++r.started;
 }
 for(std::size_t i=0;i<count_;++i){const auto& row=rows[i];const auto& state=states[i];
  if(state.slot<0||!count)continue;
  auto* sound=logical_.sound(static_cast<std::size_t>(state.slot));if(!sound||!logical_.active(*sound))return fail();
  const auto& listener=listeners[nearest[i]];const float d=distances[i];
  // PORT STAND-IN (flagged): the four per-course owners differed here (GB
  // clamped attenuation to 0 at or beyond farDistance; MC/RC/TB skipped the
  // control update there). The common owner keeps the clamped GB form; the
  // original 291350 control at d >= farDistance is not traced.
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
} // namespace positional_sound_rows50SR
