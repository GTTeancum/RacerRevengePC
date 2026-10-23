#pragma once
#include "logical_audio_owner.h"
#include "original_audio_sample_cache50kg.h"
#include "../../../src/assets/audio_assets50hc.h"
#include <array>
#include <filesystem>
#include <memory>
#include <string>
namespace rr::platform {
class OriginalUiMainAudio50KG;
class OriginalUiGarageActors50GN;
class OriginalUiGaragePodScene50KG;
class OriginalUiGaragePageOwner50KG;
// Native backend for fresh Garage's four authored callback rows. Concrete
// source Sound references/group ownership belong to the retained PodScene;
// native-only aggregate preparation does not construct those source objects.
// This owner does NOT model general101DA0 resets/replay: each played108
// is monotonic for its entire page lifetime, so four dedicated native instances
// are sufficient without replacement, stealing or invented restart behavior.
// A retained source position is not permission to invent attenuation or pan:
// this reached Sound vtable updates elapsed time only, not233AE0 spatial audio.
class OriginalUiGarageDroidAudio50GN final {
public:
 static constexpr std::size_t soundCount=4,firstVoice=49,requiredFrontendVoices=53;
 struct Observation {
  const OriginalUiGarageDroidAudio50GN* owner=nullptr;const AudioMixer50HD* mixer=nullptr;
  std::uint64_t revision=0,mixerEpoch=0,mixedFrames=0;
  std::size_t completedActors=0,constructedSounds=0;
  // Per-Sound fields below are meaningful only for constructed rows.
  std::array<bool,4> constructed{};
  std::array<bool,4> played108{};
  std::array<std::int32_t,4> pitch40{},basePitch50{},channel2C{};
  std::array<std::uint32_t,4> elapsedBits24{};
  std::array<float,4> sourceDuration20{};
  std::array<std::array<float,4>,4> position60{};
  std::array<AudioVoiceObservation50HD,4> voices{};
 };
 OriginalUiGarageDroidAudio50GN()noexcept=default;
 ~OriginalUiGarageDroidAudio50GN()noexcept{reset();}
 OriginalUiGarageDroidAudio50GN(const OriginalUiGarageDroidAudio50GN&)=delete;
 OriginalUiGarageDroidAudio50GN& operator=(const OriginalUiGarageDroidAudio50GN&)=delete;
 bool ready()const noexcept;
 bool quiet()const noexcept;
 // Authenticated transport remains valid after scene retirement, while callbacks
 // remain closed. Main uses this to finish mixing/return to other UI pages.
 bool transportValid()const noexcept;
 bool inspect(Observation&)const noexcept;
 // Invoke immediately AFTER this actor's real154480 and11AD80, before the
 // next actor. time/stopped/position are canonical model fields, not timers.
 // Actors1..4,7,8 have no sound row and follow the actual empty-row skip.
 bool afterSample(std::size_t actor,float modelTime34,bool stopped10,
     const std::array<float,4>& actualModelPosition220)noexcept;
 // Source235720 calls233E00 in group insertion order. This must be reached by
 // the actual group Entity owner, not inferred from animation service alone.
 bool serviceGroup235720(float delta)noexcept;
 // Historical native-only aggregate retirement, NOT counted Scene/group or
 // per-Sound source release. Refused while the genuine source owner is bound.
 bool retire2353D0()noexcept;
 void reset()noexcept;
private:
 friend class OriginalUiMainAudio50KG;
 friend class OriginalUiGarageActors50GN;
 friend class OriginalUiGaragePodScene50KG;
 friend class OriginalAudioSampleCache50KG;
 friend class OriginalAudioPendingWaves50KG;
 // Historical eager aggregate checker compatibility only. The real actor join
 // uses the pending transport and calls278F60 in original actor construction
 // order, AFTER that actor's actual initial pose and28EA20 bounds/observers.
 bool prepare(const std::filesystem::path& audioRoot,AudioMixer50HD&,std::string&)noexcept;
 // Authenticates/retains immutable PCM and binds the exclusive native range.
 // No Sound row is registered, gain-owned or service-ready by this operation.
 bool prepareColdTransport(const std::filesystem::path&,AudioMixer50HD&,std::string&)noexcept;
 // Exactly empty pending prefix for concrete Actors/Main coordination. This
 // never reports an eager, partially constructed, paused or retired owner.
 bool coldConstructionCurrent()const noexcept;
 bool sourceConstructionPrefixCurrent()const noexcept;
 bool nativeSampleLeasesEmpty()const noexcept;
 // Includes empty authored sound-row lists: actor ordinals0..9 exactly once.
 // Failure preserves the completed prefix and closes further construction.
 bool appendActorSoundRows278F60(std::size_t actor,std::string&)noexcept;
 bool current()const noexcept;
 bool sourceConstructionComplete()const noexcept;
 // Called only by the concrete Scene's ordered235150 body. The preflight
 // runs before registry mutation; stop/release runs after the real23518C.
 // Native failed-entry cancellation only: no source sample publication or
 // invented guest flush. The actual Scene Group caller authenticates Session.
 bool preflightUnresolvedSceneEntryRetirement(const OriginalUiGaragePodScene50KG&)const noexcept;
 // Exact GUI77 failed Page only; read-only Actor/Animation-manager quiescence.
 bool preflightLateBorrowActorRetirement2353D0(const OriginalUiGaragePageOwner50KG&,
  const OriginalUiGaragePodScene50KG&)const noexcept;
 // Exact GUI78 loaded-Page hide failure; no Group/Sound disposition here.
 bool preflightLoadedPageActorRetirement2353D0(const OriginalUiGaragePageOwner50KG&,
  const OriginalUiGaragePodScene50KG&)const noexcept;
 // GUI80 completed activation but failed before the first Watto binding effect.
 bool preflightWattoBindingActorRetirement2353D0(const OriginalUiGaragePageOwner50KG&,
  const OriginalUiGaragePodScene50KG&)const noexcept;
 bool finishUnresolvedSceneEntryRetirement(OriginalUiGaragePodScene50KG&)noexcept;
 bool preflightSourceSceneRetirement2353D0(const OriginalUiGaragePodScene50KG&)const noexcept;
 bool stopSourceSceneRow233670(OriginalUiGaragePodScene50KG&,std::size_t)noexcept;
 bool finishSourceSceneRetirement235150(OriginalUiGaragePodScene50KG&)noexcept;
 bool retiredSceneTransportCurrent()const noexcept;
 bool retainedStoppedVoiceCurrent(std::size_t)const noexcept;
 bool resolvedRowsCurrent()const noexcept;
 static float sourceRendererPitch()noexcept;
 bool sourceSampleStartCurrent(std::size_t,const AudioClip50HD&,AudioMixer50HD::NativeSourceSampleStart50KG&)const noexcept;
 bool publishSourceSampleVoice(std::size_t)noexcept;
 bool stoppedVoiceHistoryCurrent()const noexcept;
 bool canRearmRetiredSceneRetainingHistory()const noexcept;
 bool rearmRetiredSceneRetainingHistory()noexcept;
 bool retainedSceneRearmed_=false;
 bool canRearmRetiredSceneCold()const noexcept;
 bool rearmRetiredSceneCold()noexcept;
 bool bindResolvedSourceRow(std::size_t,const OriginalAudioSampleCache50KG::NativeSampleBinding&,
     const OriginalAudioSampleCache50KG::NativeSampleHandle&,std::string&)noexcept;
 struct Row {
  const assets::AudioClip50HC* source=nullptr;
  LogicalAudioOwner::Sound* sound=nullptr;
  OriginalAudioSampleCache50KG::NativeSampleBinding nativeSample,nativeVoice;
  std::int32_t basePitch50=4096;
  float sourceDuration20=0.f;
  std::uint64_t playbackRevision=0;
  bool played108=false;
 };
 // Non-owning backlink only. The root-retained PodScene owns the source rows
 // and aliases Main's full native backing; reset cannot erase that ownership.
 OriginalUiGaragePodScene50KG* sourceSceneGroup_=nullptr;
 // Written only after actual Group registry/Sound/list/count retirement.
 // It is a native tombstone, never Scene/Page teardown or playback admission.
 bool sourceSceneGroupReleased_=false;
 bool nativeUnresolvedEntryRetired_=false; //Terminal failure, never normal rearm.
 std::unique_ptr<assets::AudioBank50HC> assets_;
 std::array<Row,4> rows_{};
 // Source Sounds/rows may retire while Mixer still stores their stopped raw
 // clips. Retain exact snapshots and strong immutable PCM until replacement
 // or actual transport retirement, never merely until source104970.
 struct RetiredVoice {
  OriginalAudioSampleCache50KG::NativeSampleBinding pcm;
  AudioVoiceObservation50HD stopped{};
  bool present=false;
 };
 std::array<RetiredVoice,4> retiredVoices_{};
 LogicalAudioOwner logical_;
 AudioMixer50HD* mixer_=nullptr;
 const OriginalSoundMode50KG* mode_=nullptr;
 std::uint32_t outputRate_=0;
 std::uint64_t epoch_=0,revision_=0;
 std::size_t nextActor_=0,constructedSounds_=0;
 bool attempted_=false,prepared_=false,retired_=false,poisoned_=false,paused_=false;
};
}
