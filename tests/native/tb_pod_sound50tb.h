#pragma once
#include "tb_trigger_authoring50tb.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../src/game/trigger_actions50in.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "../../platform/linux/native/original_sound_mode50kg.h"
#include <filesystem>
#include <memory>
#include <thread>

// PCM/transport owner for authentic TB Trigger16 Action2. The caller's actual
// synchronous trigger query owns entry and source action ordering; this class
// never invents a contact, listener publication, clock, or Entity membership.
namespace tb_pod_sound50TB {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
namespace source=tb_trigger_authoring50TB;
inline constexpr std::size_t voice=126,actionIndex=1,originalBytes=26960,originalFrames=47068;
inline constexpr const char* filename="sfx/race/hugedoor.vag";
inline constexpr const char* sourceSha="5fcc343234e2b98dcf14a929fd42554dd806ba3ba8a36a2ea6de15bc3e6613bb";
static_assert(voice<p::audioMaximumVoices50HD);
struct Counters {std::uint64_t requests=0,starts=0,culled=0,rejected=0;};
class Owner final {
 std::shared_ptr<const source::Owner> source_;
 a::AudioBank50HC bank_;p::AudioClip50HD clip_{};
 p::AudioMixer50HD* mixer_=nullptr;p::RaceAudioGroups50IC* groups_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 std::uint64_t epoch_=0,revision_=0;bool attempted_=false,prepared_=false,played_=false,blocked_=false,closed_=false;
 Counters counters_{};
 bool transport()const noexcept {
  return thread_==std::this_thread::get_id()&&prepared_&&mixer_&&groups_&&groups_->uses(*mixer_)&&
   mixer_->groupEpoch50IC()==epoch_&&voice<mixer_->count()&&mixer_->hasVoiceGroupGain50IC(voice)&&
   groups_->binding(voice)==g::RaceAudioGroup50IC::World4D8;
 }
 bool current()const noexcept {
  return !closed_&&!blocked_&&transport()&&source_&&source_->ready()&&source_->coldSource()&&
   mixer_->originalSoundMode50KG()&&mixer_->originalSoundMode50KG()->ready();
 }
 bool reject(std::string& error,const char* text){blocked_=true;++counters_.rejected;error=text;return false;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner()noexcept {if(!closed_&&!close())std::terminate();}
 bool prepare(std::shared_ptr<const source::Owner> authored,p::AudioMixer50HD& mixer,
     p::RaceAudioGroups50IC& groups,const std::filesystem::path& audioRoot,std::string& error){
  if(thread_!=std::this_thread::get_id()){error="TB PodSound owner thread differs";return false;}
  if(attempted_||closed_){error="TB PodSound requires a fresh stable owner";return false;}attempted_=true;
  try {
   if(!authored||!authored->ready()||!authored->coldSource()||!groups.uses(mixer)||
      mixer.count()!=p::audioMaximumVoices50HD||!mixer.originalSoundMode50KG()||!mixer.originalSoundMode50KG()->ready()){
    error="TB PodSound actual source/mixer/mode/groups unavailable";return false;}
   const auto& trigger=authored->source(source::door);const auto& action=trigger.actions[actionIndex];
   if(trigger.sourceOrdinal!=16||trigger.actionCount!=2||!trigger.oneTimeOnly||
      trigger.actions[0].kind!=source::Action::Animation||trigger.actions[0].animationIndex!=0||
      action.kind!=source::Action::PodSound||!action.original||action.filename!="sfx\\race\\hugedoor.vag"||
      action.soundPosition.v[0]!=15107.f||action.soundPosition.v[1]!=1673.f||action.soundPosition.v[2]!=14695.f||
      action.minDistance!=0.f||action.maxDistance!=2500.f){error="TB PodSound exact ordered descriptor differs";return false;}
   if(mixer.active(voice)||mixer.hasVoiceGroupGain50IC(voice)||
      groups.binding(voice)!=g::RaceAudioGroup50IC::Unrepresented||!groups.canBind(voice,g::RaceAudioGroup50IC::World4D8)){
    error="TB PodSound exclusive voice126 unavailable";return false;}
   const std::vector<a::AudioRequest50HC> requests{{filename,false,false,false}};
   if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
   const auto* sound=bank_.find(filename);
   if(!sound||bank_.clips.size()!=1||bank_.ready!=1||sound->state!=a::AudioAssetState50HC::ready||
      sound->sourceBytes!=originalBytes||sound->sourceSha256!=sourceSha||sound->pcm.sourceSha256!=sourceSha||
      sound->pcm.sampleRate!=16500||sound->pcm.channels!=1||sound->pcm.frames!=originalFrames||
      sound->pcm.samples.size()!=originalFrames||sound->pcm.hasEmbeddedLoop||sound->pcm.loopStart||sound->pcm.loopEnd){
    error="TB PodSound original HUGEDOOR PCM identity/framing differs";return false;}
   clip_={sound->pcm.samples.data(),sound->pcm.frames,sound->pcm.sampleRate,sound->pcm.channels,0,0};
   if(!p::AudioMixer50HD::validClip(clip_)){error="TB PodSound actual PCM view rejected";return false;}
   // No playback occurs here. Binding is the last fallible publication; PCM
   // and authentic source remain retained through checked close/destruction.
   if(!groups.bind(voice,g::RaceAudioGroup50IC::World4D8)){error="TB PodSound actual World4D8 bind rejected";return false;}
   source_=std::move(authored);mixer_=&mixer;groups_=&groups;epoch_=mixer.groupEpoch50IC();
   revision_=mixer.playbackRevision(voice);prepared_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool prepared()const noexcept{return current();}
 const source::Owner* sourceIdentity()const noexcept{return source_.get();}
 const a::AudioBank50HC& bank()const noexcept{return bank_;}
 const Counters& counters()const noexcept{return counters_;}
 bool readyFor(const source::Owner& authored,std::size_t index)const noexcept {
  return current()&&source_.get()==&authored&&index==actionIndex&&!played_&&
   mixer_->playbackRevision(voice)==revision_&&!mixer_->active(voice)&&p::AudioMixer50HD::validClip(clip_);
 }
 // actualListeners belongs to the caller's synchronous ActionLease and is
 // never retained. The live root supplies its accepted source camera listener;
 // a cold numerical test may explicitly supply a fixture without a live claim.
 bool playEntering(const source::Owner& authored,std::size_t index,const source::ActionSource& action,
     const g::Vector3_48C* actualListeners,std::size_t listenerCount,std::string& error){
  if(thread_!=std::this_thread::get_id()){error="TB PodSound owner thread differs";return false;}
  ++counters_.requests;
  if(!current()||source_.get()!=&authored||index!=actionIndex||
     &action!=&source_->source(source::door).actions[actionIndex])
   return reject(error,"TB PodSound reached source/action identity differs");
  const auto decision=g::triggerPodSoundAdmission50IN(true,
    {action.soundPosition,action.minDistance,action.maxDistance},actualListeners,listenerCount);
  if(decision.status!=g::TriggerActionStatus50IN::Complete)return reject(error,"TB PodSound actual listener admission rejected");
  if(!decision.play){++counters_.culled;error.clear();return true;}
  if(!readyFor(authored,index))return reject(error,"TB PodSound exclusive transport/replay rejected");
  //1B5B00->104140 passes sound=null, so computed attenuation/pan/doppler do
  //not reach this sound. Constructor volume100/pan0/base pitch survive;
  //233410 flags2 requests immediate start, not looping. World4D8 gain/pause
  //remain solely in the actual shared group layer.
  if(!mixer_->play(voice,clip_,1.f,1.f,false,0.f))return reject(error,"TB PodSound actual mixer start rejected");
  revision_=mixer_->playbackRevision(voice);played_=true;++counters_.starts;error.clear();return true;
 }
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id())return false;if(closed_)return true;
  if(!prepared_){closed_=true;return true;}
  if(!transport())return false;
  // Replaced transport is never stopped by a stale owner. If that foreign
  // revision still borrows our PCM, failed close retains the backing until
  // its actual owner retires it; successful close must not grant dangling PCM.
  if(mixer_->active(voice)){
   if(played_&&mixer_->playbackRevision(voice)==revision_)mixer_->stop(voice);
   else {p::AudioVoiceObservation50HD observed{};
    if(!mixer_->inspectVoice(voice,observed)||observed.voice.clip.samples==clip_.samples)return false;}
  }
  for(std::size_t slot=0;slot<mixer_->count();++slot)if(mixer_->active(slot)){
   p::AudioVoiceObservation50HD observed{};
   if(!mixer_->inspectVoice(slot,observed)||observed.voice.clip.samples==clip_.samples)return false;
  }
  closed_=true;return true;
 }
};
} // namespace tb_pod_sound50TB
