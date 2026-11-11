#pragma once
#include "tc_trigger_authoring50tc.h"
#include "../../src/assets/audio_assets50hc.h"
#include "../../platform/linux/native/audio_mixer50hd.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"
#include "native_results_world_stop26d070.h"
#include <array>
#include <filesystem>
#include <memory>
#include <thread>

// Native ordinary Sound provider for the two exact TC trigger11 actions.
// This is distinct from PodSound spatialization and does not fabricate a
// source Entity logical-member registration. The real shared World4D8 group
// owns gain/pause; the enclosing race owns mixer/group lifetime and retirement.
namespace tc_global_sound50TC {
namespace a=rr::assets;namespace p=rr::platform;namespace g=rr::game;
inline constexpr std::size_t firstVoice=126,soundCount=2;
static_assert(firstVoice+soundCount<=p::audioMaximumVoices50HD);
struct Original {const char* filename;std::size_t bytes;const char* sha;std::size_t frames;};
inline constexpr std::array<Original,2> originals{{
 {"sfx/race/beambrk2.vag",16960,"0d29f90b84b6e1e291cf29abcf9ad9a23c5339d2f153db1df752392a6c1f1b6c",29568},
 {"sfx/race/latichit.vag",21184,"085cbfc5f33e5b246716fe05eb54671be4dd8823d15bc291a61918449c3c7a79",36960}
}};
class Owner final : public native_results_world_stop26D070::Owner {
 std::shared_ptr<const tc_trigger_authoring50TC::Owner> source_;
 a::AudioBank50HC bank_;std::array<p::AudioClip50HD,2> clips_{};
 p::AudioMixer50HD* mixer_=nullptr;p::RaceAudioGroups50IC* groups_=nullptr;
 std::thread::id thread_{};bool attempted_=false,prepared_=false,closed_=false;
 std::array<bool,2> played_{};
 bool ownership()const noexcept {
  if(!prepared_||closed_||std::this_thread::get_id()!=thread_||!source_||!source_->ready()||
     !mixer_||!groups_||!groups_->uses(*mixer_))return false;
  for(std::size_t i=0;i<2;++i)if(firstVoice+i>=mixer_->count()||
    !mixer_->hasVoiceGroupGain50IC(firstVoice+i)||groups_->binding(firstVoice+i)!=g::RaceAudioGroup50IC::World4D8)return false;
  return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner()noexcept {if(prepared_&&!closed_&&!close())std::terminate();}
 bool prepare(std::shared_ptr<const tc_trigger_authoring50TC::Owner> source,
   p::AudioMixer50HD& mixer,p::RaceAudioGroups50IC& groups,const std::filesystem::path& audioRoot,std::string& error){
  if(attempted_){error="TC ordinary Sound requires a fresh stable owner";return false;}attempted_=true;
  try {
   if(!source||!source->ready()||!source->coldSource()||!groups.uses(mixer)){
    error="TC ordinary Sound actual source/mixer/group unavailable";return false;}
   const auto& trigger=source->source(tc_trigger_authoring50TC::gate);
   if(trigger.actionCount!=3||!trigger.oneTimeOnly){error="TC ordinary Sound source order changed";return false;}
   for(std::size_t i=0;i<2;++i){const auto& action=trigger.actions[i+1];
    const std::string expected=i?"sfx\\race\\latichit":"sfx\\race\\beambrk2";
    if(action.kind!=tc_trigger_authoring50TC::Action::Sound||action.filename!=expected||action.positional||!action.isGlobalSound||
       firstVoice+i>=mixer.count()||mixer.active(firstVoice+i)||mixer.hasVoiceGroupGain50IC(firstVoice+i)||
       groups.binding(firstVoice+i)!=g::RaceAudioGroup50IC::Unrepresented||
       !groups.canBind(firstVoice+i,g::RaceAudioGroup50IC::World4D8)){
      error="TC ordinary Sound descriptor/exclusive voice ownership rejected";return false;}
   }
   std::vector<a::AudioRequest50HC> requests;for(const auto& original:originals)requests.push_back({original.filename,false,false,false});
   if(!a::loadAudioBank50HC(audioRoot,requests,bank_,error))return false;
   if(bank_.clips.size()!=2||bank_.ready!=2){error="TC ordinary Sound requires both actual original PCM clips";return false;}
   for(std::size_t i=0;i<2;++i){const auto& original=originals[i];const auto* clip=bank_.find(original.filename);
    if(!clip||clip->state!=a::AudioAssetState50HC::ready||clip->sourceBytes!=original.bytes||clip->sourceSha256!=original.sha||
       clip->pcm.sourceSha256!=original.sha||clip->pcm.sampleRate!=19500||clip->pcm.channels!=1||
       clip->pcm.frames!=original.frames||clip->pcm.samples.size()!=original.frames||clip->pcm.hasEmbeddedLoop||clip->pcm.loopStart||clip->pcm.loopEnd){
      error="TC ordinary Sound original identity/framing rejected";return false;}
    clips_[i]={clip->pcm.samples.data(),clip->pcm.frames,clip->pcm.sampleRate,clip->pcm.channels,0,0};
    if(!p::AudioMixer50HD::validClip(clips_[i])){error="TC ordinary Sound actual PCM view rejected";return false;}
   }
   if(!groups.bindRange(firstVoice,2,g::RaceAudioGroup50IC::World4D8)){
    error="TC ordinary Sound actual World4D8 membership rejected";return false;}
   source_=std::move(source);mixer_=&mixer;groups_=&groups;thread_=std::this_thread::get_id();prepared_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool readyFor(const tc_trigger_authoring50TC::Owner& source,std::size_t actionIndex)const noexcept {
  return ownership()&&source_.get()==&source&&actionIndex>=1&&actionIndex<=2&&
    !played_[actionIndex-1]&&!mixer_->active(firstVoice+actionIndex-1)&&p::AudioMixer50HD::validClip(clips_[actionIndex-1]);
 }
 bool playEntering(const tc_trigger_authoring50TC::Owner& source,std::size_t actionIndex,
   const tc_trigger_authoring50TC::ActionSource& descriptor,std::string& error){
  if(!readyFor(source,actionIndex)||&descriptor!=&source_->source(tc_trigger_authoring50TC::gate).actions[actionIndex]){
   error="TC ordinary Sound reached action/source ownership rejected";return false;}
  // Source constructor gain100, pitch4096, centered global delivery. Enter
  // calls ordinary start flags0; source one-time trigger prevents replay.
  const auto i=actionIndex-1;
  if(!mixer_->play(firstVoice+i,clips_[i],1.f,1.f,false,0.f)){
   error="TC ordinary Sound actual mixer start rejected";return false;}
  played_[i]=true;error.clear();return true;
 }
 bool close()noexcept {
  if(closed_)return true;if(!ownership())return false;
  for(std::size_t i=0;i<2;++i)mixer_->stop(firstVoice+i);
  closed_=true;return true;
 }
 bool prepared()const noexcept{return ownership();}
 // Results 26D070: each of the two trigger Sounds is claimed only while its
 // own one-shot playback (this owner's PCM) is still active on its voice.
 bool ownsActive26D070(std::size_t i)const noexcept {
  if(i>=2||!played_[i]||!mixer_||!mixer_->active(firstVoice+i))return false;
  p::AudioVoiceObservation50HD observed{};
  return mixer_->inspectVoice(firstVoice+i,observed)&&observed.voice.clip.samples==clips_[i].samples;
 }
 bool canStopResults26D070(const p::AudioMixer50HD& mixer,const p::RaceAudioGroups50IC& groups)const noexcept override {
  return ownership()&&mixer_==&mixer&&groups_==&groups&&mixer.canRecordControlChanges(2);
 }
 std::size_t resultsStopCount26D070()const noexcept override {
  return std::size_t(ownsActive26D070(0))+std::size_t(ownsActive26D070(1));
 }
 std::size_t resultsStopVoice26D070(std::size_t n)const noexcept override {
  for(std::size_t i=0;i<2;++i)if(ownsActive26D070(i)){if(!n)return firstVoice+i;--n;}
  return p::audioMaximumVoices50HD;
 }
 void stopResultsPreflighted26D070()noexcept override {
  for(std::size_t i=0;i<2;++i)if(ownsActive26D070(i))mixer_->stop(firstVoice+i);
 }
};
}
