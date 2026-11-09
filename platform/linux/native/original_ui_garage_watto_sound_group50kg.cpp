#include "original_ui_garage_watto_sound_group50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_audio_pending_waves50kg.h"
#include <cmath>
#include <bit>
#include <cstring>
#include <exception>
#include <utility>
namespace rr::platform {
OriginalUiGarageWattoSoundGroup50KG::~OriginalUiGarageWattoSoundGroup50KG()noexcept {
 // Source child teardown order belongs to the real widget owner, not member
 // destruction order. Never silently detach/count away a live group.
 if(constructed_&&!retired_)std::terminate();
}
bool OriginalUiGarageWattoSoundGroup50KG::constructGroup2350D0(
 std::shared_ptr<Grant> grant,C& widget,std::shared_ptr<OriginalUiMainAudio50KG> main,
 std::string& error)noexcept {
 if(attempted_){error="Watto SoundGroup requires a fresh stable owner";return false;}attempted_=true;
 if(!grant||!grant->current()||!main||widget.kind!=game::OriginalWreckComponentKind50LA::wattoWidget1D4320||
  widget.reference00!=1){error="Watto SoundGroup requires the concrete live Garage grant/widget/Main transport";return false;}
 auto* audio=main->garageWattoAudioCold();
 if(!audio||!audio->sourceGroupConstructionCurrent()){error="Watto source group requires authenticated unused cold Watto transport";return false;}
 std::unique_ptr<Sound*[]> initial;
 try{initial=std::make_unique<Sound*[]>(8);}catch(...){error="235900 SoundGroup list allocation failed before source mutation";return false;}
 // The shared grant preflights slot/counter/authority capacity before its
 // genuine141300 enrollment and2351F0 first-free claim. No guessed slot2.
 const auto made=grant->constructWattoSoundGroup2350D0(entity_,slot40_);
 if(!made.okay()){error="2350D0/2351F0 actual shared group construction refused";return false;}
 grant_=std::move(grant);main_=std::move(main);audio_=audio;widget_=&widget;
 constructed_=true;members4C_=std::move(initial);capacity54_=8;growth50_=16;allocated58_=true;
 enabled30_=true;volume38_=100;state44_=0;
 audio_->sourceGroup_=this;audio_->sourceGroupAttempted_=true;
 if(!grant_->attach(widget,entity_,0xffffffffu).okay()){
  poisoned_=true;error="1D4610 actual group attachment failed; typed prefix cleanup required";return false;
 }
 attached_=true;error.clear();return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::current()const noexcept {rr::game::EntityForestReadScope50LA forestReadScope50LA;
 if(!constructed_||retired_||poisoned_||!attached_||!grant_||!main_||!audio_||
  !grant_->wattoSoundGroupCurrent(entity_,slot40_)||entity_.parent1C!=widget_||entity_.firstChild18||
  !enabled30_||volume38_!=100||state44_||growth50_!=16||!allocated58_||!members4C_||
  capacity54_<8||count48_>capacity54_||count48_>OriginalUiGarageWattoAudio50KG::soundCount||
  audio_->sourceGroup_!=this||!audio_->sourceGroupAttempted_||audio_->sourceGroupReleased_||!audio_->current())return false;
 // The guard above has just validated this exact Audio owner, all retained
 // PCM/metadata, and every native voice. This loop only observes the actual
 // Group rows and source Sound fields; it invokes no callback or mutation.
 // Retain each membership/ref check without rescanning all voices per row.
 for(std::size_t i=0;i<count48_;++i){const auto ordinal=i?i-1:OriginalUiGarageWattoAudio50KG::responseCount;
  if(members4C_[i]!=&audio_->sounds_[ordinal]||!audio_->sourceSoundIdentityAfterCurrent(*members4C_[i]))return false;
 }
 return true;
}
OriginalUiGarageWattoSoundGroup50KG::Sound* OriginalUiGarageWattoSoundGroup50KG::append(
 std::size_t ordinal,std::string_view filename,std::string& error)noexcept {
 if(!current()||ordinal>=audio_->soundCount||audio_->sounds_[ordinal].source||
  filename.empty()||filename.size()>259||filename.find('\0')!=std::string_view::npos||
  !filename.ends_with(".vag")){error="232ED0 actual Sound construction phase/filename invalid";return nullptr;}
 std::unique_ptr<Sound*[]> grown;
 if(count48_==capacity54_){
  try{grown=std::make_unique<Sound*[]>(capacity54_+growth50_);}catch(...){error="235A60 Sound list growth failed before mutation";return nullptr;}
  for(std::size_t i=0;i<count48_;++i)grown[i]=members4C_[i];
 }
 auto& sound=audio_->sounds_[ordinal];
 if(grown){members4C_=std::move(grown);capacity54_+=growth50_;}
 //1027B0/1048F0 creates ref1 but NOT an Entity/counter contribution. The
 //constructor registers THIS pointer235270 before233110 resource setup.
 sound.source.emplace();sound.source->group34=this;
 //235280..23529C first initializes the actual shared assertion word434A88,
 //then guard881C. The exact retained Sound pointer below is nonnull by
 //construction, so2352BC's pointer assertion succeeds before235A60 append.
 //current() already authenticated this same group/slot; no callback or
 //other owner mutation can intervene in this main-thread operation.
 if(!grant_->noteWattoSoundInsert235270(entity_,slot40_))std::terminate();
 members4C_[count48_++]=&sound;
 auto& f=*sound.source;
 if(ordinal==audio_->responseCount){f.flags10=1;}
 else {
  f.flags10=2;f.stream1A8=true;
 }
 //233194 copies the authentic source filename before extension handling and
 //either lookup. Every bounded Garage filename already ends in .vag, so the
 //source2331C4 extension replacement preserves these exact bytes.
 std::memcpy(f.filenameA4.data(),filename.data(),filename.size());f.filenameA4[filename.size()]='\0';
 if(ordinal!=audio_->responseCount){
  f.duration20=sound.metadata->admittedDurationSeconds;
  f.basePitch50=f.pitch40=static_cast<std::int32_t>(sound.metadata->originalPitch);
  //233110 stream branch leaves effectivePitch4C at constructor4096.
 }
 error.clear();return &sound;
}
bool OriginalUiGarageWattoSoundGroup50KG::appendWing232DB0(std::string& error)noexcept {
 if(count48_){error="1D4644 wing must be first actual group Sound";return false;}
 if(!append(OriginalUiGarageWattoAudio50KG::responseCount,"sfx\\ui\\watwing3.vag",error))return false;
 //233110's nonstream branch next reaches26DAD0/26DAF0/26DB00 against the
 //actual shared deferred sample registry. Authentic predecoded PCM does NOT
 //perform that source registration. Keep the constructed232ED0 reference
 //for typed cleanup, without pretending original factory failure/success.
 //1D4658 volume17 setter is not reached until the factory really returns.
 failurePC_=0x26DAD0;
 error="233110/26DAD0 awaits the actual retained Application pending namespace";
 return false;
}
bool OriginalUiGarageWattoSoundGroup50KG::registerPendingWing2332A0(
 std::shared_ptr<OriginalAudioPendingWaves50KG> waves,std::string& error)noexcept {
 if(!waves||!wingRegistrationPending()){error="2332A0 requires the actual pending wing and Application-issued namespace";return false;}
 if(!waves->registerWattoWing2332A0(*this,error))return false;
 // Native backing lifetime only; source Sound reference00 stays exactly1.
 pendingWaves_=std::move(waves);wingRegistrationComplete_=true;failurePC_=0;return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::wingRegistrationPending()const noexcept {
 if(!current()||count48_!=1||wingRegistrationComplete_||pendingWaves_||failurePC_!=0x26DAD0)return false;
 const auto& sound=audio_->sounds_[OriginalUiGarageWattoAudio50KG::responseCount];
 return members4C_[0]==&sound&&audio_->sourceSoundCurrent(sound)&&sound.source->flags10==1&&
  !sound.source->stream1A8&&!sound.source->stereo1AC&&!sound.source->loop1B0&&
  !sound.source->resourceA0&&!sound.source->nativeSample28&&sound.source->volume38==100&&
  !std::strcmp(sound.source->filenameA4.data(),"sfx\\ui\\watwing3.vag");
}
bool OriginalUiGarageWattoSoundGroup50KG::storeWingVolume233830(std::string& error)noexcept {
 if(!current()||count48_!=1||!wingRegistrationComplete_||!pendingWaves_){error="1D4658 requires the returned actual wing factory";return false;}
 auto& sound=*members4C_[0];
 if(!audio_->sourceSoundCurrent(sound)||!sound.source->resourceA0||sound.source->nativeSample28||
  sound.source->volume38!=100||sound.source->volume44!=100||sound.source->channel2C!=-1){
  error="1D4658 source wing volume order differs";return false;}
 //233830 clamps17.0 to[0,100] and writes CVT.W.S to+38. Effective+44 is
 //unchanged until a genuine play; this is not native playback admission.
 sound.source->volume38=17;error.clear();return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::wingFlushPending232EC0(
 const std::shared_ptr<OriginalAudioPendingWaves50KG>& waves)const noexcept {
 if(!sourceSoundsComplete()||!waves||pendingWaves_!=waves||pendingWaves_.owner_before(waves)||
  waves.owner_before(pendingWaves_)||reverbAttempted_)return false;
 const auto& sound=*members4C_[0];const auto& f=*sound.source;
 if(!f.resourceA0||f.nativeSample28||f.volume38!=17||f.volume44!=100||f.channel2C!=-1||
  std::strcmp(f.filenameA4.data(),"sfx\\ui\\watwing3.vag"))return false;
 OriginalAudioPendingWaves50KG::RowObservation row;
 return waves->inspect(*f.resourceA0,row)&&row.directory==waves->lookup26DAD0("audio\\sfx\\ui\\watwing3.vag")&&
  row.registrationCount==1&&row.representative==&sound&&row.representativeGroup==this&&
  !row.sceneRepresentativeOwner&&!row.hasResolvedNativeSample;
}
bool OriginalUiGarageWattoSoundGroup50KG::wingResolved26D9F0(
 const std::shared_ptr<OriginalAudioPendingWaves50KG>& waves)const noexcept {
 if(!sourceSoundsComplete()||!waves||pendingWaves_!=waves||pendingWaves_.owner_before(waves)||
  waves.owner_before(pendingWaves_))return false;
 const auto& sound=*members4C_[0];const auto& f=*sound.source;
 if(!f.resourceA0||!f.nativeSample28||!audio_->resolvedSourceSampleCurrent(sound)||
  f.volume38!=17||f.volume44!=100||f.channel2C!=-1||std::strcmp(f.filenameA4.data(),"sfx\\ui\\watwing3.vag"))return false;
 OriginalAudioPendingWaves50KG::RowObservation row;OriginalAudioSampleCache50KG::Observation sample;
 return waves->inspect(*f.resourceA0,row)&&row.directory==waves->lookup26DAD0("audio\\sfx\\ui\\watwing3.vag")&&
  !row.registrationCount&&!row.representative&&!row.representativeGroup&&!row.sceneRepresentativeOwner&&
  row.hasResolvedNativeSample&&row.resolvedNativeSampleCurrent&&sound.nativeSample.inspect(sample)&&
  sample.directory&&sample.directory->row(sample.sourceOrdinal)==row.directory&&sample.sourceOrdinal==*f.resourceA0&&
  std::bit_cast<std::uint32_t>(f.duration20)==sample.observedSourceDurationBits&&
  f.basePitch50==static_cast<std::int32_t>(sample.observedSourcePitch)&&
  f.pitch40==f.basePitch50&&f.pitch4C==f.basePitch50;
}
OriginalUiGarageWattoSoundGroup50KG::Sound* OriginalUiGarageWattoSoundGroup50KG::appendResponse232DB0(
 std::uint32_t event,const assets::OriginalUiGarageWattoAssets50KG& assets,std::string& error)noexcept {
 if(!current()||!wingRegistrationComplete_||!assets.loaded()||!event||event>audio_->responseCount||count48_!=event){error="292650 response source order/asset owner invalid";return nullptr;}
 const auto* row=assets.event(event);
 if(!row||row->id!=event||!row->stream||row->stereo||row->loop||row->metadata!=audio_->sounds_[event-1].metadata){
  error="292650 exact authored response identity/stream flags differ";return nullptr;
 }
 return append(event-1,row->filename,error);
}
bool OriginalUiGarageWattoSoundGroup50KG::ownsResponse(std::uint32_t event,const Sound* sound)const noexcept {
 return current()&&responseMemberIdentityAfterCurrent(event,sound);
}
bool OriginalUiGarageWattoSoundGroup50KG::responseMemberIdentityAfterCurrent(
 std::uint32_t event,const Sound* sound)const noexcept {
 // The full Group proof has already checked all41 real source Sounds and
 // native voice/PCM/registry ownership in this same readonly call. Keep the
 // exact event bounds and BOTH actual pointer-membership comparisons here.
 return audio_&&members4C_&&event&&event<=OriginalUiGarageWattoAudio50KG::responseCount&&
  event<count48_&&event<capacity54_&&sound==members4C_[event]&&sound==&audio_->sounds_[event-1];
}
bool OriginalUiGarageWattoSoundGroup50KG::sourceSoundsComplete()const noexcept {
 return current()&&wingRegistrationComplete_&&count48_==OriginalUiGarageWattoAudio50KG::soundCount;
}
bool OriginalUiGarageWattoSoundGroup50KG::configureReverb235520(std::string& error)noexcept {
 if(!sourceSoundsComplete()||reverbAttempted_){error="1D4694 group reverb source phase invalid";return false;}
 reverbAttempted_=true;
 const auto retained=weak_from_this().lock();
 if(!retained||retained.get()!=this||!main_||
  !main_->enqueueWattoReverb26D6C0(retained,reverbEnqueue_,error)){
  failurePC_=0x235520;return false;}
 //Source235520 returns after26D6C0 has really appended command25. It does
 //NOT wait for that command's wet DSP. No fallible work follows acceptance.
 failurePC_=0;return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::queuedIdentityCurrent(
 const std::shared_ptr<Grant>& grant,std::uint64_t generation,std::int32_t slot,
 const OriginalUiMainAudio50KG* main)const noexcept {
 return sourceSoundsComplete()&&grant&&grant_==grant&&!grant_.owner_before(grant)&&!grant.owner_before(grant_)&&
  grant_->generation_==generation&&main&&main_.get()==main&&slot==slot40_&&slot>=0&&slot<25&&
  grant_->wattoSoundGroupCurrent(entity_,slot)&&entity_.reference00==1&&
  entity_.sessionGeneration==generation&&entity_.kind==game::OriginalWreckComponentKind50LA::soundGroup2350D0&&
  entity_.objectKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity_));
}
bool OriginalUiGarageWattoSoundGroup50KG::reverbEnqueueCurrent()const noexcept {
 return reverbAttempted_&&main_&&main_->queuedWattoReverbCurrent(*this,reverbEnqueue_);
}
bool OriginalUiGarageWattoSoundGroup50KG::routingHistoryCurrent(bool released)const noexcept {
 if(!grant_||!grant_->current()||!main_||!audio_||!audio_->current()||audio_->nextToken_)return false;
 if(released){
  if(!retired_||attached_||entity_.reference00||count48_||members4C_||
     !audio_->sourceGroupReleased_||audio_->sourceGroup_)return false;
 }else if(!sourceSoundsComplete())return false;
 for(const auto& sound:audio_->sounds_){
  if(!sound.source||sound.source->group34!=this||sound.source->reference00!=(released?0u:1u)||
     sound.source->channel2C!=-1||sound.source->elapsedBits24||sound.source->word7C||
     sound.latest!=audio_->instanceCount||sound.latestRevision||sound.latestToken!=-1)return false;
 }
 for(std::size_t i=0;i<audio_->instanceCount;++i)
  if(audio_->soundIds_[i]||audio_->playbackRevisions_[i]||audio_->nativeVoices_[i].retainsBacking()||
     audio_->mixer_->active(audio_->firstVoice+i)||audio_->mixer_->playbackRevision(audio_->firstVoice+i))return false;
 return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::routingSourceCurrent(bool released)const noexcept {
 if(!grant_||!grant_->current()||!main_||!audio_||!audio_->current())return false;
 if(released){
  if(!retired_||attached_||entity_.reference00||count48_||members4C_||
     !audio_->sourceGroupReleased_||audio_->sourceGroup_)return false;
 }else if(!sourceSoundsComplete())return false;
 for(const auto& sound:audio_->sounds_){
  if(!sound.source||sound.source->group34!=this||sound.source->reference00!=(released?0u:1u))return false;
  if(released&&(sound.source->channel2C!=-1||sound.latest!=audio_->instanceCount||
     sound.latestRevision||sound.latestToken!=-1))return false;
 }
 // Audio::current authenticates each exact playback revision and either its
 // retained immutable sample lease or this persistent owner's stream PCM.
 // A stopped slot deliberately keeps that backing while its raw clip exists.
 if(released)for(std::size_t n=0;n<audio_->instanceCount;++n)
  if(audio_->mixer_->active(audio_->firstVoice+n))return false;
 return true;
}
bool OriginalUiGarageWattoSoundGroup50KG::serviceGroup235720(float dt)noexcept {
 if(!sourceSoundsComplete()||!std::isfinite(dt)||dt<0.f)return false;
 //Source235720/233E00 have no native-transport pause predicate. The actual
 //prepared source dt is used even when zero; no PCM progression occurs here.
 //Actual insertion list: wing FIRST, then event1..40. Sound233E00 checks
 //signed channel even after natural PCM completion. No inferred active test.
 for(std::size_t i=0;i<count48_;++i)if(!audio_->serviceSource233E00(*members4C_[i],dt)){poisoned_=true;return false;}
 //The actual group Entity has no Entity children: Sounds are uncounted
 //references in list48. Execute141ED0's real descendants24/head18 gates.
 if(!(entity_.descendants24&2u))return true;
 if(!entity_.firstChild18)return true;
 poisoned_=true;return false; //Unsupported real child can never be skipped.
}
bool OriginalUiGarageWattoSoundGroup50KG::preflightPendingRetirement235150()const noexcept {
 // A pending representative borrows the actual Sound/Group owner. Watto must
 // refuse before its first child mutation, not only at the later Group close.
 // This is not a general retirement readiness predicate.
 //An accepted control remains borrowed until real routing application and
 //source/PCM ownership proof. Epoch invalidation is not completion. Refuse before
 //the enclosing owner destroys its first child when either is unavailable.
 if(main_&&!main_->mayRetireQueuedWattoGroup(*this))return false;
 if(!pendingWaves_)return true;
 return constructed_&&!retired_&&pendingWaves_->canRetire(*this);
}
bool OriginalUiGarageWattoSoundGroup50KG::closeGroup235150(std::string& error)noexcept {
 if(retired_){error.clear();return true;}
 if(!constructed_){error.clear();return true;}
 if(!grant_||!audio_||!audio_->current()||audio_->sourceGroup_!=this||!members4C_){error="235150 retained actual source/audio identity unavailable";return false;}
 if(!preflightPendingRetirement235150()){
  error="235150 requires applied native routing, retained source/PCM custody and no pending sample borrow";return false;
 }
 const C* parent=attached_?widget_:nullptr;
 if(!grant_->preflightWattoSoundGroupRetirement(entity_,parent,slot40_).okay()){
  error="235150 actual group Entity retirement preflight rejected";return false;
 }
 //2353D0 snapshots all current pointers before callbacks. Allocate before
 //irreversible source effects; preserve exact stop/release insertion order.
 std::unique_ptr<Sound*[]> snapshot;
 try{if(count48_)snapshot=std::make_unique<Sound*[]>(count48_);}catch(...){error="2353D0 actual pointer snapshot allocation failed";return false;}
 for(std::size_t i=0;i<count48_;++i){
  if(!members4C_[i]||!audio_->sourceSoundCurrent(*members4C_[i])){error="2353D0 actual Sound reference invalid";return false;}
  snapshot[i]=members4C_[i];
 }
 if(!audio_->mixer_->canRecordControlChanges(count48_+audio_->instanceCount)){
  error="2353D0 native stop revision capacity unavailable";return false;
 }
 if(!grant_->beginWattoSoundGroupRetirement(entity_,parent,slot40_).okay()){
  error="235150 typed begin failed before Sound cleanup";return false;
 }
 //The shared slot is ALREADY clear here, after142590/104970 and before any
 //actual233670/104970 Sound calls. No Sound owns a GameObject count.
 for(std::size_t i=0;i<count48_;++i){
  if(!audio_->stopSource233670(*snapshot[i])||!audio_->releaseSource104970(*snapshot[i]))std::terminate();
 }
 count48_=0;snapshot.reset();
 //Separate native lifetime safety: source latest-stop may leave older finite
 //instances alive. Drain ALL retained PCM instances before backing can die.
 if(!audio_->drainInstances())std::terminate();
 members4C_.reset();allocated58_=false;capacity54_=0;
 if(!grant_->finishWattoSoundGroupRetirement(entity_).okay())std::terminate();
 audio_->sourceGroupReleased_=true;audio_->sourceGroup_=nullptr;
 retired_=true;attached_=false;
 if(pendingWaves_){if(!pendingWaves_->retireGroupBorrow(*this))std::terminate();pendingWaves_.reset();}
 //No source disable/cancel exists. The command was actually applied; detach
 //only the dead group's native range/strong custody, preserving the slot byte.
 if(main_&&!main_->releaseRetiredWattoRouting(*this))std::terminate();
 error.clear();return true;
}
}
