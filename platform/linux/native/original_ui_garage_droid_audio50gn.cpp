#include "original_ui_garage_droid_audio50gn.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_sound_mode50kg.h"
#include <bit>
#include <cmath>
#include <limits>
namespace rr::platform {namespace {
struct Definition{std::size_t actor;const char* path;std::uint64_t bytes;const char* sha;float start;};
constexpr std::array<Definition,4> definitions{{
 {0,"SFX/RACE/PDWALK01.VAG",101312,"e5cbeae6e6dd8d0778cf4d0459b16dc28e90b8a58195d1da1c60321430acf90e",0.f},
 {5,"SFX/RACE/ONFIRE01.VAG",34112,"2615805c486ccb533ca0e030b1dfa0f5f3f81354d68f7593748d085a87ec7690",0.f},
 {6,"SFX/RACE/EXTING01.VAG",38304,"fd0872e6406db57dbda33afa04ec7a998ec68a9facdd5ee1f55bc71d9b225eff",0.f},
 {9,"SFX/RACE/PDFALL01.VAG",147504,"db0cc49ce67d82531f878d07577eb777af32e981a969a41bdb8bcd18910f1334",1.f}}};
float renderPitch()noexcept {volatile float a=1881.f*48000.f;volatile float b=a/4096.f;volatile float c=b/22050.f;return c;}
bool sameClip(const AudioClip50HD& a,const AudioClip50HD& b)noexcept {
 return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
}
}
bool OriginalUiGarageDroidAudio50GN::prepare(const std::filesystem::path& root,AudioMixer50HD& mixer,std::string& error)noexcept {
 if(!prepareColdTransport(root,mixer,error))return false;
 // Historical isolated aggregate path, not evidence of actual interleaved
 // character construction. The production Actors owner must use the private
 // per-actor entry point after its real28EA20 continuation instead.
 for(std::size_t actor=0;actor<10;++actor)
  if(!appendActorSoundRows278F60(actor,error))return false;
 return true;
}
bool OriginalUiGarageDroidAudio50GN::prepareColdTransport(const std::filesystem::path& root,AudioMixer50HD& mixer,std::string& error)noexcept {
 if(attempted_){error="Garage droid audio requires fresh lifetime";return false;}attempted_=true;
 try{
  if(mixer.count()!=requiredFrontendVoices||!mixer.sampleRate()||mixer.mixedFrames()||mixer.requiresVoiceGroupGains50IC()||mixer.masterGain()!=1.f||
     !mixer.originalSoundMode50KG()||!mixer.originalSoundMode50KG()->ready()){error="Garage droid subgroup requires the actual cold53-voice frontend mixer";return false;}
  for(std::size_t i=0;i<soundCount;++i)if(mixer.active(firstVoice+i)||mixer.hasVoiceGroupGain50IC(firstVoice+i)||mixer.playbackRevision(firstVoice+i)){error="Garage droid subgroup is not cold";return false;}
  std::vector<assets::AudioRequest50HC> requests;for(const auto& d:definitions)requests.push_back({d.path,false,false,false});
  auto next=std::make_unique<assets::AudioBank50HC>();
  if(!assets::loadAudioBank50HC(root,requests,*next,error))return false;
  if(next->clips.size()!=4||next->ready!=4){error="Garage droid originals unavailable";return false;}
  for(std::size_t i=0;i<soundCount;++i){const auto& c=next->clips[i];const auto& p=c.pcm;const auto& d=definitions[i];
   if(c.state!=assets::AudioAssetState50HC::ready||c.sourceSha256!=d.sha||c.sourceBytes!=d.bytes||p.sourceSha256!=d.sha||p.sampleRate!=22050||p.channels!=1||p.hasEmbeddedLoop||p.samples.size()!=p.frames){error="Garage droid original identity/PCM shape differs";return false;}
  }
  if(!mixer.canRecordControlChanges(4)||!logical_.bind(mixer,firstVoice,4)){error="Garage droid native owner binding rejected";return false;}
  assets_=std::move(next);mixer_=&mixer;epoch_=mixer.groupEpoch50IC();mode_=mixer.originalSoundMode50KG();outputRate_=mixer.sampleRate();
  revision_=1;prepared_=true;error.clear();return true;
 }catch(...){error="Garage droid audio allocation/preparation failed";return false;}
}
bool OriginalUiGarageDroidAudio50GN::coldConstructionCurrent()const noexcept {
 if(!current()||retired_||paused_||nextActor_||constructedSounds_)return false;
 for(std::size_t i=0;i<soundCount;++i)if(retiredVoices_[i].present||mixer_->playbackRevision(firstVoice+i))return false;
 return true;
}
bool OriginalUiGarageDroidAudio50GN::sourceConstructionPrefixCurrent()const noexcept {
 return coldConstructionCurrent()||(retainedSceneRearmed_&&current()&&!retired_&&!paused_&&nextActor_==0&&constructedSounds_==0);
}
bool OriginalUiGarageDroidAudio50GN::appendActorSoundRows278F60(std::size_t actor,std::string& error)noexcept {
 if(!current()||retired_||paused_||actor>=10||actor!=nextActor_||revision_==std::numeric_limits<std::uint64_t>::max()-1){
  poisoned_=true;error="Garage droid actor sound construction is not the next exact live prefix";return false;
 }
 for(std::size_t i=0;i<soundCount;++i){if(definitions[i].actor!=actor)continue;
  if(i!=constructedSounds_||rows_[i].source||rows_[i].sound||!mixer_->canRecordControlChanges(1)){
   poisoned_=true;error="Garage droid next source Sound row is unavailable";return false;
  }
  if(sourceSceneGroup_){
   if(!sourceSceneGroup_->appendSoundPrefix278F60(*this,actor,i,assets_->clips[i],error))return false;
   rows_[i].source=&assets_->clips[i];++constructedSounds_;continue;
  }
  // Historical isolated native-only path. The root production custody now
  // ALWAYS binds its genuine Scene group before Actors can reach this suffix.
  auto& row=rows_[i];row.source=&assets_->clips[i];const auto& p=row.source->pcm;
  if(!logical_.registerClip(i,{p.samples.data(),p.frames,p.sampleRate,p.channels,0,0},true)){
   poisoned_=true;error="Garage droid source Sound registration rejected after completed prefix";return false;
  }
  row.sound=logical_.sound(i);
  if(!row.sound||!mixer_->setVoiceGroupGain50IC(firstVoice+i,1.f)){
   poisoned_=true;error="Garage droid subgroup partially constructed";return false;
  }
  ++constructedSounds_;
 }
 ++nextActor_;++revision_;error.clear();return true;
}
bool OriginalUiGarageDroidAudio50GN::nativeSampleLeasesEmpty()const noexcept {
 for(const auto& retained:retiredVoices_)if(retained.present||retained.pcm.retainsBacking())return false;
 for(const auto& row:rows_)if(row.nativeSample.retainsBacking()||row.nativeVoice.retainsBacking())return false;
 return true;
}
bool OriginalUiGarageDroidAudio50GN::current()const noexcept {
 if(!prepared_||poisoned_||!assets_||assets_->clips.size()!=soundCount||assets_->ready!=soundCount||nextActor_>10||constructedSounds_>soundCount||!mixer_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=requiredFrontendVoices||mixer_->sampleRate()!=outputRate_||mixer_->originalSoundMode50KG()!=mode_||
    !revision_||revision_==std::numeric_limits<std::uint64_t>::max()||!mixer_->canRecordControlChanges()||mixer_->masterGain()!=1.f||mixer_->requiresVoiceGroupGains50IC())return false;
 const auto* mode=mixer_->originalSoundMode50KG();game::OriginalSoundRouting50KG routing;
 if(!mode||!mode->ready()||!game::projectOriginalSoundRouting26CBA050KG(mode->fields(),0,100,routing)||routing.phaseSelector||routing.positional||routing.leftPercent!=100||routing.rightPercent!=100)return false;
 if(sourceSceneGroupReleased_)return retiredSceneTransportCurrent();
 std::size_t expectedSounds=0;
 for(std::size_t i=0;i<soundCount;++i){const auto& r=rows_[i];const auto& source=assets_->clips[i];const auto& p=source.pcm;AudioVoiceObservation50HD voice;
  if(source.state!=assets::AudioAssetState50HC::ready||source.sourceSha256!=definitions[i].sha||source.sourceBytes!=definitions[i].bytes||p.sourceSha256!=definitions[i].sha||p.sampleRate!=22050||p.channels!=1||p.hasEmbeddedLoop||p.samples.size()!=p.frames||!mixer_->inspectVoice(firstVoice+i,voice))return false;
  if(definitions[i].actor>=nextActor_){
   if(r.source||r.sound||r.played108||r.playbackRevision||r.basePitch50!=4096||r.sourceDuration20!=0.f||voice.groupKnown||voice.groupPaused||voice.voice.active||
      (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i)))return false;
   continue;
  }
  ++expectedSounds;
  if(r.source!=&source)return false;
  AudioClip50HD retained{p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
  if(sourceSceneGroup_){
   if(!sourceSceneGroup_->sourceRowIdentityCurrent(*this,i,true))return false;
   const auto& actual=*sourceSceneGroup_->group4D8_.sounds_[i];
   if(!r.sound){
    if(actual.nativeSample28||actual.nativeSound||actual.basePitch50!=4096||actual.duration20!=0.f||
       actual.channel2C!=-1||r.nativeSample.retainsBacking()||r.nativeVoice.retainsBacking()||
       r.played108||r.playbackRevision||voice.groupKnown||voice.groupPaused||voice.voice.active||
       (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i))||
       r.basePitch50!=4096||r.sourceDuration20!=0.f)return false;
    continue;
   }
   AudioClip50HD sourceBinding;
   if(!actual.nativeSample28||!actual.nativeSample28->current()||actual.nativeSound!=r.sound||
      !r.nativeSample.clipForRetainedVoice(this,retained)||
      !actual.nativeSample.clipForRetainedVoice(this,sourceBinding)||!sameClip(retained,sourceBinding)||
      r.basePitch50!=actual.basePitch50||r.sourceDuration20!=actual.duration20)return false;
  }
  if(!r.sound||!voice.groupKnown||voice.groupGain!=1.f||voice.groupPaused||
     (r.played108?voice.playbackRevision!=r.playbackRevision:
      (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i)))||
     !sameClip(r.sound->clip,retained))return false;
  const auto& s=*r.sound;if(s.loop1B0||s.volume38!=100||s.angle3C||s.multiplier54!=1.f||s.multiplier58!=1.f||s.position60[3]!=1.f)return false;
  if(r.played108){
   AudioClip50HD activeBinding;
   if(sourceSceneGroup_&&(!r.nativeVoice.clipForRetainedVoice(this,activeBinding)||!sameClip(activeBinding,s.clip)))return false;
   if(r.basePitch50!=1881||s.pitch40!=1881||s.pitch4C!=1881||!sameClip(voice.voice.clip,s.clip)||voice.voice.gain!=1.f||voice.voice.pitch!=renderPitch()||voice.voice.loop||voice.voice.pan!=0.f||voice.originalLeft!=1.f||voice.originalRight!=1.f)return false;
  }else if(r.basePitch50!=(sourceSceneGroup_?1881:4096)||s.pitch40!=r.basePitch50||voice.voice.active||
      (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i)))return false;

 }
 return expectedSounds==constructedSounds_;
}
bool OriginalUiGarageDroidAudio50GN::bindResolvedSourceRow(std::size_t ordinal,
 const OriginalAudioSampleCache50KG::NativeSampleBinding& binding,
 const OriginalAudioSampleCache50KG::NativeSampleHandle& handle,std::string& error)noexcept {
 if(!current()||revision_>=std::numeric_limits<std::uint64_t>::max()-1||!sourceSceneGroup_||ordinal>=soundCount||!rows_[ordinal].source||rows_[ordinal].sound||
    !sourceSceneGroup_->sourceRowIdentityCurrent(*this,ordinal,true)||
    !mixer_->canRecordControlChanges(1)||logical_.sound(ordinal)){
  error="native Droid binding requires the genuine registered unresolved source row";return false;
 }
 AudioClip50HD clip;OriginalAudioSampleCache50KG::Observation observed;
 if(!binding.clipForResolved(this,handle,clip)||!binding.inspect(observed)||
    observed.sourceOrdinal!=*sourceSceneGroup_->group4D8_.sounds_[ordinal]->resourceA0||
    clip.frames!=rows_[ordinal].source->pcm.frames||clip.sampleRate!=22050||clip.channels!=1||
    observed.observedSourcePitch!=1881||!std::isfinite(std::bit_cast<float>(observed.observedSourceDurationBits))||
    std::bit_cast<float>(observed.observedSourceDurationBits)<=0.f){
  error="native Droid binding/handle/authenticated source identity mismatch";return false;
 }
 auto& row=rows_[ordinal];row.nativeSample=binding; //Pin before borrowed clip.
 if(!logical_.registerClip(ordinal,clip,true)){
  poisoned_=true;error="native Droid logical binding refused; backing retained";return false;
 }
 row.sound=logical_.sound(ordinal);
 // Preserve the exact post-claim publication from the routing predecessor.
 if(!row.sound||!sourceSceneGroup_->main_->publishColdSceneSampleGroupGain(*this,ordinal,clip)){
  poisoned_=true;error="native Droid gain publication failed; partial row and backing retained";return false;
 }
 // Caller owns the atomic source publication immediately after this native
 // borrow is established. Do not move source40/4C writes ahead of source28.
 error.clear();return true;
}
bool OriginalUiGarageDroidAudio50GN::sourceConstructionComplete()const noexcept {
 return sourceSceneGroup_&&current()&&!retired_&&nextActor_==10&&constructedSounds_==soundCount;
}
bool OriginalUiGarageDroidAudio50GN::resolvedRowsCurrent()const noexcept {
 if(!current()||retired_||nextActor_!=10||constructedSounds_!=soundCount)return false;
 for(const auto& row:rows_)if(!row.sound)return false;
 return true;
}
bool OriginalUiGarageDroidAudio50GN::ready()const noexcept {
 if(!resolvedRowsCurrent())return false;
 return !sourceSceneGroup_||(sourceSceneGroup_->main_&&
     sourceSceneGroup_->main_->nativeScenePlaybackCurrent(*this));
}
bool OriginalUiGarageDroidAudio50GN::transportValid()const noexcept{return current();}
bool OriginalUiGarageDroidAudio50GN::quiet()const noexcept{if(!current())return false;for(std::size_t i=0;i<soundCount;++i)if(mixer_->active(firstVoice+i))return false;return true;}
bool OriginalUiGarageDroidAudio50GN::inspect(Observation& o)const noexcept {
 o={};if(!current())return false;o.owner=this;o.mixer=mixer_;o.revision=revision_;o.mixerEpoch=epoch_;o.mixedFrames=mixer_->mixedFrames();
 o.completedActors=nextActor_;o.constructedSounds=constructedSounds_;
 for(std::size_t i=0;i<soundCount;++i){const auto& r=rows_[i];
  if(r.source){o.constructed[i]=true;o.channel2C[i]=-1;o.pitch40[i]=4096;o.basePitch50[i]=r.basePitch50;}
  if(r.sound){o.played108[i]=r.played108;o.channel2C[i]=r.sound->channel2C;o.elapsedBits24[i]=r.sound->elapsedBits24;o.pitch40[i]=r.sound->pitch40;o.basePitch50[i]=r.basePitch50;o.sourceDuration20[i]=r.sourceDuration20;o.position60[i]=r.sound->position60;}
  if(!mixer_->inspectVoice(firstVoice+i,o.voices[i]))return false;
 }return true;
}
float OriginalUiGarageDroidAudio50GN::sourceRendererPitch()noexcept{return renderPitch();}
bool OriginalUiGarageDroidAudio50GN::sourceSampleStartCurrent(std::size_t ordinal,const AudioClip50HD& clip,
 AudioMixer50HD::NativeSourceSampleStart50KG& out)const noexcept {
 if(ordinal>=soundCount||!resolvedRowsCurrent()||paused_||!sourceSceneGroup_||
    !sourceSceneGroup_->sourceRowIdentityCurrent(*this,ordinal,true))return false;
 const auto& row=rows_[ordinal];const auto& source=*sourceSceneGroup_->group4D8_.sounds_[ordinal];
 if(row.played108||row.playbackRevision||!row.sound||!logical_.owns(*row.sound)||!row.sound->available||
    row.sound->channel2C!=-1||row.sound->revision||row.sound->token!=-1||
    source.nativeSound!=row.sound||source.flags10!=1||source.sampleContinuationPending||
    source.stream1A8||source.stereo1AC||source.loop1B0||!source.resourceA0||!source.nativeSample28||
    !source.nativeSample28->current()||row.sound->volume38!=100||row.sound->angle3C)return false;
 AudioClip50HD owned,sourceOwned;OriginalAudioSampleCache50KG::Observation metadata;
 if(!row.nativeSample.clipForResolved(this,*source.nativeSample28,owned)||!sameClip(owned,clip)||
    !source.nativeSample.clipForResolved(this,*source.nativeSample28,sourceOwned)||!sameClip(owned,sourceOwned)||
    !row.nativeSample.inspect(metadata)||metadata.sourceOrdinal!=*source.resourceA0||
    std::bit_cast<std::uint32_t>(source.duration20)!=metadata.observedSourceDurationBits||
    std::bit_cast<std::uint32_t>(row.sourceDuration20)!=metadata.observedSourceDurationBits||
    source.basePitch50!=static_cast<std::int32_t>(metadata.observedSourcePitch)||row.basePitch50!=source.basePitch50||
    row.sound->pitch40!=source.basePitch50||row.sound->pitch4C!=source.basePitch50||source.basePitch50!=1881)return false;
 out={source.duration20,source.loop1B0,source.stream1A8,source.stereo1AC};return true;
}
bool OriginalUiGarageDroidAudio50GN::publishSourceSampleVoice(std::size_t ordinal)noexcept {
 if(!sourceSceneGroup_||!sourceSceneGroup_->main_||ordinal>=soundCount||
    logical_.nextToken_==std::numeric_limits<std::int32_t>::max()||!mixer_->canRecordControlChanges(1))return false;
 auto& row=rows_[ordinal];if(!row.sound)return false;
 const auto slot=firstVoice+ordinal;
 if(mixer_->active(slot)||mixer_->nativeSourceVoiceReserved(slot,mixer_->playbackRevision(slot)))return false;
 AudioMixer50HD::NativeSourceSampleStart50KG source;
 if(!sourceSampleStartCurrent(ordinal,row.sound->clip,source))return false;
 // Pin BEFORE Main can publish a raw Mixer borrow. Logical token/revision
 // stores are the existing native owner semantics, not guest channel IDs.
 row.nativeVoice=row.nativeSample;
 if(!sourceSceneGroup_->main_->publishNativeSceneSample(*this,ordinal,row.sound->clip))return false;
 row.sound->token=++logical_.nextToken_;row.sound->channel2C=row.sound->token;
 row.sound->revision=mixer_->playbackRevision(slot);return true;
}
bool OriginalUiGarageDroidAudio50GN::afterSample(std::size_t actor,float time,bool stopped,const std::array<float,4>& position)noexcept {
 //1017B4 skips playback before the cue threshold. The actual flags2 play is
 //immediate RPC8 even while an earlier Scene25 waits for267BE0; it uses the
 //real retained group byte, not the separate fully-applied ready() contract.
 if(!(sourceSceneGroup_?resolvedRowsCurrent():ready())||paused_||actor>=10||!std::isfinite(time)||time<0)return false;
 for(std::size_t i=0;i<soundCount;++i){if(definitions[i].actor!=actor)continue;auto& r=rows_[i];
  if(r.played108||!(time>definitions[i].start||stopped))return true;
  if(sourceSceneGroup_&&(!sourceSceneGroup_->main_||
     !sourceSceneGroup_->main_->nativeSceneImmediatePlaybackCurrent(*this)))return false;
  for(unsigned axis=0;axis<3;++axis)if(!std::isfinite(position[axis]))return false;
  if(!mixer_->canRecordControlChanges(1)||logical_.active(*r.sound))return false;
  //233DB0 copies XYZ only. W stays constructor vf0=1.
  for(unsigned axis=0;axis<3;++axis)r.sound->position60[axis]=position[axis];
  //233E90 ->26C240 -> original PS2SOUND.IRX RPC16, af4c. Actual22050
  //header is authenticated above; LoadFile23AC/LoadCDFile26B8 produce1881.
  if(sourceSceneGroup_){
   //2332F0/233E90 already published actual native-cache source metadata.
   r.nativeVoice=r.nativeSample; //Retain BEFORE the mixer may borrow PCM.
  }else {
  r.basePitch50=static_cast<std::int32_t>(((r.source->pcm.sampleRate<<12)/48000u)&0xffffu);
  r.sound->pitch40=r.sound->pitch4C=r.basePitch50;
  //RPC15 LoadCDFile2690 stores floor(dataBytes*1000/((rate/28)*16))
  //milliseconds; EE2692D4 divides by1000. No PCM-duration substitution.
  const auto dataBytes=static_cast<std::uint32_t>(r.source->sourceBytes-48u);
  const auto milliseconds=(dataBytes*1000u)/((r.source->pcm.sampleRate/28u)*16u);
  volatile float duration=static_cast<float>(milliseconds)/1000.f;r.sourceDuration20=duration;
  }
  //Source pitch is preserved and rendered through the source integer pitch
  //clock, not nominal VAG-rate1 normalization. This does not emulate SPU2
  //ADPCM interpolation/filter behavior or its command-queue latency.
  const bool started=sourceSceneGroup_?publishSourceSampleVoice(i):
      logical_.startSourceVolume(*r.sound,100,renderPitch(),false,0)==LogicalAudioOwner::Result::complete;
  if(!started){poisoned_=true;return false;}
  r.playbackRevision=mixer_->playbackRevision(firstVoice+i);
  r.sound->volume44=100;r.sound->angle48=0;r.sound->elapsedBits24=0;r.sound->word7C=1;
  retiredVoices_[i]={};r.played108=true;++revision_;return true;
 }
 return true;
}
bool OriginalUiGarageDroidAudio50GN::serviceGroup235720(float dt)noexcept {
 // Source233E00 elapsed bookkeeping is not playback admission.
 if(!resolvedRowsCurrent()||paused_||!std::isfinite(dt)||dt<0)return false;
 for(auto& r:rows_)if(r.sound->channel2C>=0){volatile float elapsed=std::bit_cast<float>(r.sound->elapsedBits24)+dt;
  if(!std::isfinite(elapsed)){poisoned_=true;return false;}r.sound->elapsedBits24=std::bit_cast<std::uint32_t>(static_cast<float>(elapsed));}
 ++revision_;return true;
}
bool OriginalUiGarageDroidAudio50GN::preflightLateBorrowActorRetirement2353D0(
 const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
 // The exact failed Page owns the authorization; Pending proves all four
 // live unresolved rows through the existing nonmutating transport proof.
 // No Sound, registration, accepted control or native voice is retired here.
 return scene.backend_==this&&scene.preflightLateBorrowActorAudio(page);
}
bool OriginalUiGarageDroidAudio50GN::preflightLoadedPageActorRetirement2353D0(
 const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
 // The exact failed Page owns the authorization; Pending proves all four
 // live unresolved rows through the existing nonmutating transport proof.
 // No Sound, registration, accepted control or native voice is retired here.
 return scene.backend_==this&&scene.preflightLoadedPageActorAudio(page);
}
bool OriginalUiGarageDroidAudio50GN::preflightWattoBindingActorRetirement2353D0(
 const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
 // The exact failed Page owns the authorization; Pending proves all four
 // live unresolved rows through the existing nonmutating transport proof.
 // No Sound, registration, accepted control or native voice is retired here.
 return scene.backend_==this&&scene.preflightWattoBindingActorAudio(page);
}
bool OriginalUiGarageDroidAudio50GN::preflightUnresolvedSceneEntryRetirement(
 const OriginalUiGaragePodScene50KG& scene)const noexcept {
 if(sourceSceneGroup_!=&scene||sourceSceneGroupReleased_||nativeUnresolvedEntryRetired_||
    !current()||retired_||paused_||nextActor_!=10||constructedSounds_!=soundCount||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1||!nativeSampleLeasesEmpty()||
    logical_.mixer_!=mixer_||logical_.count_!=soundCount||logical_.nextToken_)return false;
 for(std::size_t i=0;i<soundCount;++i){const auto& row=rows_[i];
  const auto& source=scene.group4D8_.sounds_[i];const auto& logical=logical_.sounds_[i];
  AudioVoiceObservation50HD voice;
  if(!source||!scene.sourceRowIdentityCurrent(*this,i,true)||source->reference00!=1||
     !source->sampleContinuationPending||source->nativeSample28||source->nativeSound||
     source->nativeSample.retainsBacking()||source->channel2C!=-1||
     source->duration20!=0.f||source->basePitch50!=4096||
     row.source!=&assets_->clips[i]||row.sound||row.played108||row.playbackRevision||
     row.basePitch50!=4096||row.sourceDuration20!=0.f||logical_.registered_[i]||
     logical.owner!=&logical_||logical.slot!=firstVoice+i||logical.available||
     logical.channel2C!=-1||logical.revision||logical.token!=-1||
     !mixer_->inspectVoice(firstVoice+i,voice)||voice.groupKnown||voice.groupGain!=1.f||
     voice.groupPaused||voice.playbackRevision||voice.voice.active||
     mixer_->nativeSourceVoiceRoutes_[firstVoice+i].count||voice.voice.clip.samples||
     voice.voice.clip.frames||voice.voice.clip.sampleRate||voice.voice.clip.channels||
     voice.voice.clip.loopStart||voice.voice.clip.loopEnd||voice.voice.sourceFrame!=0.0||
     voice.voice.gain!=1.f||voice.voice.pitch!=1.f||voice.voice.pan!=0.f||voice.voice.loop||
     voice.originalLeft!=1.f||voice.originalRight!=1.f)return false;
 }
 return true;
}
bool OriginalUiGarageDroidAudio50GN::finishUnresolvedSceneEntryRetirement(
 OriginalUiGaragePodScene50KG& scene)noexcept {
 if(sourceSceneGroup_!=&scene||sourceSceneGroupReleased_||nativeUnresolvedEntryRetired_||
    !scene.nativeUnresolvedEntryRetiring_||!scene.soundGroupSourceRetired235150()||
    !nativeSampleLeasesEmpty()||nextActor_!=10||constructedSounds_!=soundCount||
    logical_.mixer_!=mixer_||logical_.count_!=soundCount||logical_.nextToken_||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1)return false;
 for(std::size_t i=0;i<soundCount;++i){AudioVoiceObservation50HD voice;
  if(rows_[i].source!=&assets_->clips[i]||rows_[i].sound||rows_[i].played108||
     rows_[i].playbackRevision||logical_.registered_[i]||
     !mixer_->inspectVoice(firstVoice+i,voice)||voice.groupKnown||voice.groupPaused||
     voice.voice.active||voice.playbackRevision||voice.voice.clip.samples||voice.voice.clip.frames||
     mixer_->nativeSourceVoiceRoutes_[firstVoice+i].count)return false;
 }
 // All logical rows remain unregistered, so close performs no Mixer stop.
 // Authenticated PCM stays owned by this persistent transport until final close.
 logical_.close();rows_={};sourceSceneGroup_=nullptr;sourceSceneGroupReleased_=true;
 nativeUnresolvedEntryRetired_=true;retired_=true;++revision_;return true;
}
bool OriginalUiGarageDroidAudio50GN::preflightSourceSceneRetirement2353D0(
 const OriginalUiGaragePodScene50KG& scene)const noexcept {
 if(sourceSceneGroup_!=&scene||sourceSceneGroupReleased_||!resolvedRowsCurrent()||paused_||
    nextActor_!=10||constructedSounds_!=soundCount||revision_>=std::numeric_limits<std::uint64_t>::max()-1||
    !mixer_->canRecordControlChanges(soundCount+1))return false;
 const bool live=scene.main_&&scene.main_->nativeSceneWetLifetimeCurrent(scene);
 if(scene.main_&&scene.main_->nativeWetProcessor_&&!live)return false;
 for(std::size_t i=0;i<soundCount;++i){const auto& row=rows_[i];
  const auto& source=scene.group4D8_.sounds_[i];AudioVoiceObservation50HD voice;
  if(!source||!scene.sourceRowIdentityCurrent(*this,i,true)||source->reference00!=1||
     source->sampleContinuationPending||!source->nativeSample28||!source->nativeSample28->current()||
     source->nativeSound!=row.sound||!row.sound||row.sound->owner!=&logical_||row.sound->slot!=firstVoice+i||
     !mixer_->inspectVoice(firstVoice+i,voice)||
     (row.played108?voice.playbackRevision!=row.playbackRevision:
      (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i))))return false;
  if(!live&&(row.played108||row.playbackRevision||row.nativeVoice.retainsBacking()||
     row.sound->channel2C!=-1||row.sound->elapsedBits24||row.sound->word7C||
     voice.voice.active||voice.voice.clip.samples||voice.voice.clip.frames||voice.voice.sourceFrame!=0.0))return false;
  if(live&&row.played108){AudioClip50HD owned;
   if(!row.nativeVoice.clipForRetainedVoice(this,owned)||!sameClip(voice.voice.clip,owned)||
      row.sound->revision!=row.playbackRevision||!row.playbackRevision||
      (row.sound->channel2C!=-1&&row.sound->channel2C!=row.sound->token))return false;
  }
 }
 return true;
}
bool OriginalUiGarageDroidAudio50GN::stopSourceSceneRow233670(
 OriginalUiGaragePodScene50KG& scene,std::size_t ordinal)noexcept {
 // Registry is already released. Authenticate the entered source destructor,
 // exact logical slot/revision and retained PCM without calling live current().
 if(sourceSceneGroup_!=&scene||sourceSceneGroupReleased_||ordinal>=soundCount||
    !scene.soundGroupRetirementEntered235150()||!mixer_||mixer_->groupEpoch50IC()!=epoch_||
    mixer_->count()!=requiredFrontendVoices||!scene.main_||
    (scene.main_->nativeWetProcessor_&&!scene.main_->nativeWetInterfaceCurrent()))return false;
 auto& row=rows_[ordinal];auto& source=scene.group4D8_.sounds_[ordinal];
 AudioVoiceObservation50HD voice;
 if(!source||source->ordinal!=ordinal||source->reference00!=1||source->nativeSound!=row.sound||
    !row.sound||row.sound!=logical_.sound(ordinal)||row.sound->owner!=&logical_||
    row.sound->slot!=firstVoice+ordinal||!mixer_->inspectVoice(firstVoice+ordinal,voice)||
    (row.played108?voice.playbackRevision!=row.playbackRevision:
     (voice.playbackRevision&&!retainedStoppedVoiceCurrent(ordinal))))return false;
 if(row.played108){AudioClip50HD owned;
  if(!row.playbackRevision||row.sound->revision!=row.playbackRevision||
     !row.nativeVoice.clipForRetainedVoice(this,owned)||!sameClip(voice.voice.clip,owned))return false;
 }else if(row.playbackRevision||row.nativeVoice.retainsBacking()||voice.voice.active||
          (voice.playbackRevision?!retainedStoppedVoiceCurrent(ordinal):
           (voice.voice.clip.samples||voice.voice.clip.frames)))return false;
 auto& logical=*row.sound;
 if(logical.channel2C!=-1){
  if(logical.channel2C!=logical.token||!row.played108)return false;
  // Decoder EOF leaves a source reservation. Only actual267CB0 expiration or
  // source stop changes it; do not use LogicalAudioOwner's generic PCM test.
  if(mixer_->nativeSourceVoiceReserved(firstVoice+ordinal,row.playbackRevision)){
   mixer_->stop(firstVoice+ordinal);logical.elapsedBits24=0;logical.word7C=0;
  }
  logical.channel2C=-1;
 }
 // The fixed per-Sound native range cannot be borrowed by an older overlap.
 // Drain its exact stopped revision before source104970 can release bindings.
 mixer_->stop(firstVoice+ordinal);
 return !mixer_->active(firstVoice+ordinal)&&
    !mixer_->nativeSourceVoiceReserved(firstVoice+ordinal,row.playbackRevision);
}
bool OriginalUiGarageDroidAudio50GN::finishSourceSceneRetirement235150(
 OriginalUiGaragePodScene50KG& scene)noexcept {
 if(sourceSceneGroup_!=&scene||sourceSceneGroupReleased_||!scene.soundGroupSourceRetired235150()||
    !mixer_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=requiredFrontendVoices||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1)return false;
 // Preflight all four before publishing snapshots or destroying any row lease.
 std::array<AudioVoiceObservation50HD,soundCount> stopped{};
 for(std::size_t i=0;i<soundCount;++i){const auto& row=rows_[i];auto& voice=stopped[i];
  if(!row.sound||row.sound!=logical_.sound(i)||row.sound->channel2C!=-1||
     !mixer_->inspectVoice(firstVoice+i,voice)||
     (row.played108?voice.playbackRevision!=row.playbackRevision:
      (voice.playbackRevision&&!retainedStoppedVoiceCurrent(i)))||
     voice.voice.active||mixer_->nativeSourceVoiceReserved(firstVoice+i,voice.playbackRevision))return false;
  if(row.played108){AudioClip50HD owned;
   if(!row.playbackRevision||!row.nativeVoice.clipForRetainedVoice(this,owned)||!sameClip(owned,voice.voice.clip))return false;
  }else if(row.playbackRevision||row.nativeVoice.retainsBacking()||
       (voice.playbackRevision?!retainedStoppedVoiceCurrent(i):(voice.voice.clip.samples||voice.voice.clip.frames)))return false;
 }
 for(std::size_t i=0;i<soundCount;++i)if(rows_[i].played108){
  retiredVoices_[i].pcm=rows_[i].nativeVoice;retiredVoices_[i].stopped=stopped[i];retiredVoices_[i].present=true;
 }
 // Invalidate logical access before source-row leases leave. Every remaining
 // raw Mixer clip is still pinned by its exact retained stopped-voice binding.
 logical_.close();rows_={};sourceSceneGroup_=nullptr;sourceSceneGroupReleased_=true;
 retired_=true;++revision_;return true;
}
bool OriginalUiGarageDroidAudio50GN::retainedStoppedVoiceCurrent(std::size_t ordinal)const noexcept {
 if(ordinal>=soundCount||!mixer_||mixer_->groupEpoch50IC()!=epoch_)return false;
 const auto& retained=retiredVoices_[ordinal];AudioVoiceObservation50HD now;AudioClip50HD owned;
 if(!retained.present||!retained.stopped.playbackRevision||
    !retained.pcm.clipForRetainedVoice(this,owned)||!mixer_->inspectVoice(firstVoice+ordinal,now)||
    now.voice.active||mixer_->nativeSourceVoiceReserved(firstVoice+ordinal,now.playbackRevision))return false;
 const auto& old=retained.stopped;
 return now.playbackRevision==old.playbackRevision&&sameClip(now.voice.clip,owned)&&sameClip(old.voice.clip,owned)&&
    now.voice.sourceFrame==old.voice.sourceFrame&&now.voice.gain==old.voice.gain&&now.voice.pitch==old.voice.pitch&&
    now.voice.pan==old.voice.pan&&now.voice.loop==old.voice.loop&&now.originalLeft==old.originalLeft&&
    now.originalRight==old.originalRight;
}
bool OriginalUiGarageDroidAudio50GN::retiredSceneTransportCurrent()const noexcept {
 if(!sourceSceneGroupReleased_||sourceSceneGroup_||!retired_||paused_||
    nextActor_!=10||constructedSounds_!=soundCount)return false;
 if(nativeUnresolvedEntryRetired_&&(logical_.mixer_||logical_.count_||logical_.nextToken_||
    !nativeSampleLeasesEmpty()))return false;
 for(std::size_t i=0;i<soundCount;++i){const auto& row=rows_[i];AudioVoiceObservation50HD voice;
  if(nativeUnresolvedEntryRetired_&&(logical_.registered_[i]||logical_.sounds_[i].owner||
     logical_.sounds_[i].channel2C!=-1||logical_.sounds_[i].revision||
     logical_.sounds_[i].token!=-1||mixer_->nativeSourceVoiceRoutes_[firstVoice+i].count))return false;
  if(retiredVoices_[i].present){
   if(row.source||row.sound||row.nativeSample.retainsBacking()||row.nativeVoice.retainsBacking()||
      row.played108||row.playbackRevision||row.basePitch50!=4096||row.sourceDuration20!=0.f||
      !retainedStoppedVoiceCurrent(i)||!mixer_->inspectVoice(firstVoice+i,voice)||
      !voice.groupKnown||voice.groupGain!=1.f||voice.groupPaused)return false;
   continue;
  }
  if(row.source||row.sound||row.nativeSample.retainsBacking()||row.nativeVoice.retainsBacking()||
     row.played108||row.playbackRevision||row.basePitch50!=4096||row.sourceDuration20!=0.f||
     !mixer_->inspectVoice(firstVoice+i,voice)||
     (nativeUnresolvedEntryRetired_?voice.groupKnown:!voice.groupKnown)||
     voice.groupGain!=1.f||voice.groupPaused||voice.playbackRevision||
     voice.voice.active||voice.voice.clip.samples||voice.voice.clip.frames||voice.voice.clip.sampleRate||
     voice.voice.clip.channels||voice.voice.clip.loopStart||voice.voice.clip.loopEnd||
     voice.voice.sourceFrame!=0.0||voice.voice.gain!=1.f||voice.voice.pitch!=1.f||voice.voice.pan!=0.f||
     voice.voice.loop||voice.originalLeft!=1.f||voice.originalRight!=1.f)return false;
 }
 return true;
}
bool OriginalUiGarageDroidAudio50GN::stoppedVoiceHistoryCurrent()const noexcept {
 if(!current())return false;
 for(std::size_t i=0;i<soundCount;++i){AudioVoiceObservation50HD v;
  if(!mixer_->inspectVoice(firstVoice+i,v)||v.voice.active||mixer_->nativeSourceVoiceReserved(firstVoice+i,v.playbackRevision))return false;
  if(v.playbackRevision&&!rows_[i].played108&&!retainedStoppedVoiceCurrent(i))return false;
 }
 return true;
}
bool OriginalUiGarageDroidAudio50GN::canRearmRetiredSceneRetainingHistory()const noexcept {
 if(nativeUnresolvedEntryRetired_||!current()||!retiredSceneTransportCurrent()||!logical_.attempted_||logical_.mixer_||logical_.count_||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1)return false;
 for(const auto& sound:logical_.sounds_)if(sound.owner||sound.channel2C!=-1)return false;
 return true;
}
bool OriginalUiGarageDroidAudio50GN::rearmRetiredSceneRetainingHistory()noexcept {
 // Exact Main retirement preflight ran before clearing group bindings. Those
 // are already unknown now, while the same raw clips and leases remain.
 if(nativeUnresolvedEntryRetired_||!prepared_||poisoned_||!retired_||!sourceSceneGroupReleased_||sourceSceneGroup_||
    nextActor_!=10||constructedSounds_!=soundCount||!mixer_||mixer_->groupEpoch50IC()!=epoch_||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1||!logical_.attempted_||logical_.mixer_||logical_.count_)return false;
 for(std::size_t i=0;i<soundCount;++i){AudioVoiceObservation50HD v;
  if(!mixer_->inspectVoice(firstVoice+i,v)||v.groupKnown||v.groupPaused||v.groupGain!=1.f||v.voice.active||
     mixer_->nativeSourceVoiceReserved(firstVoice+i,v.playbackRevision)||
     (v.playbackRevision&&!retainedStoppedVoiceCurrent(i)))return false;
 }
 logical_.sounds_={};logical_.registered_={};logical_.mixer_=mixer_;logical_.count_=soundCount;
 for(std::size_t i=0;i<soundCount;++i){logical_.sounds_[i].owner=&logical_;logical_.sounds_[i].slot=firstVoice+i;}
 // Preserve nextToken_, mixer epoch and every previous playback revision.
 nextActor_=0;constructedSounds_=0;sourceSceneGroupReleased_=false;retired_=false;retainedSceneRearmed_=true;
 ++revision_;return true;
}
bool OriginalUiGarageDroidAudio50GN::canRearmRetiredSceneCold()const noexcept {
 return !nativeUnresolvedEntryRetired_&&current()&&retiredSceneTransportCurrent()&&logical_.canRebindRetiredDroidCold()&&
  revision_<std::numeric_limits<std::uint64_t>::max()-1;
}
bool OriginalUiGarageDroidAudio50GN::rearmRetiredSceneCold()noexcept {
 // Main has already preflighted the completed Page/manager, exact routing
 // discharge, and all four unborrowed slots. This is not reset()/prepare():
 // retain authenticated assets, mixer, epoch, mode, rate and revision history.
 if(nativeUnresolvedEntryRetired_||!prepared_||poisoned_||!retired_||!sourceSceneGroupReleased_||sourceSceneGroup_||
    nextActor_!=10||constructedSounds_!=soundCount||!mixer_||
    revision_>=std::numeric_limits<std::uint64_t>::max()-1||
    !logical_.rebindRetiredDroidCold(*mixer_))return false;
 nextActor_=0;constructedSounds_=0;sourceSceneGroupReleased_=false;retired_=false;
 ++revision_;return true;
}
bool OriginalUiGarageDroidAudio50GN::retire2353D0()noexcept {
 // Aggregate native close is not the concrete Scene/group source destructor.
 if(sourceSceneGroup_)return false;
 if(retired_)return transportValid()&&quiet();
 if(!current()||!mixer_->canRecordControlChanges(constructedSounds_))return false;
 for(auto& r:rows_)if(r.sound&&logical_.stop233670(*r.sound)!=LogicalAudioOwner::Result::complete){poisoned_=true;return false;}
 logical_.close();retired_=true;++revision_;return true;
}
void OriginalUiGarageDroidAudio50GN::reset()noexcept {
 if(sourceSceneGroup_)std::terminate();
 bool retained=false;for(const auto& row:rows_)retained=retained||row.nativeSample.retainsBacking()||row.nativeVoice.retainsBacking();
 for(const auto& voice:retiredVoices_)retained=retained||voice.present||voice.pcm.retainsBacking();
 if(retained&&(!mixer_||mixer_->groupEpoch50IC()!=epoch_||mixer_->count()!=requiredFrontendVoices))std::terminate();
 //This exclusive range can still borrow owned PCM after an invalid foreign
 //restart. Drain all four same-epoch slots before releasing PCM, even when the
 //ordinary current() admission correctly fails. Never touch a replaced group.
 if(mixer_&&mixer_->groupEpoch50IC()==epoch_&&mixer_->count()==requiredFrontendVoices)
  for(std::size_t i=0;i<soundCount;++i)mixer_->stop(firstVoice+i);
 logical_.close();rows_={};retiredVoices_={};retainedSceneRearmed_=false;assets_.reset();mixer_=nullptr;mode_=nullptr;nextActor_=0;constructedSounds_=0;prepared_=false;retired_=true;
}
}
