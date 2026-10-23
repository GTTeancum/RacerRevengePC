#pragma once
#include "audio_mixer50hd.h"
#include "original_audio_sample_cache50kg.h"
#include "../../../src/assets/audio_assets50hc.h"
#include "../../../src/assets/original_stream_metadata50kz.h"
#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
namespace rr::platform {
class OriginalUiMainAudio50KG;
class OriginalUiGarageDroidAudio50GN;
class OriginalUiGarageWattoSoundGroup50KG;
class OriginalUiGarageWattoOwner50KG;
class OriginalAudioPendingWaves50KG;
// Retains authenticated PCM for forty stream-marked responses and one wing.
// Actual source Sounds are staged only by the concrete SoundGroup owner;
// the historical eager native transport does not construct source Entities.
// Exactly41 native instance slots form a bounded exclusive subgroup, NOT an
// assertion about original hardware channel count. Repeat flags0 may overlap:
// each Sound retains its newest handle, while older live instances finish.
// Stable main-thread owner. Mixer and voice storage must outlive this object.
class OriginalUiGarageWattoAudio50KG final {
public:
 static constexpr std::size_t responseCount=40, soundCount=41, instanceCount=41;
 static constexpr std::size_t firstVoice=8, requiredFrontendVoices=49;
 // Bounded to the authenticated 22050-Hz Garage assets. Original stream
 // metadata and PS2SOUND.IRX sample loaders agree on pitch1881. LoadFile
 // clamps the quotient and LoadCDFile does not; neither clamp changes1881.
 // Native float-ratio resampling represents the SPU rate; it is not a claim
 // of bit-identical SPU interpolation or hardware/output scheduling.
 static constexpr std::uint32_t originalBasePitch=(22050u<<12)/48000u;
 static constexpr float nativeRendererPitch=(float(originalBasePitch)*48000.f/4096.f)/22050.f;
 struct Observation {
  const OriginalUiGarageWattoAudio50KG* owner=nullptr;
  const AudioMixer50HD* mixer=nullptr;
  const assets::OriginalStreamMetadata50KZ* metadata=nullptr;
  std::uint64_t revision=0,mixerRevision=0,mixerEpoch=0,mixedFrames=0;
  std::size_t activeInstances=0,freeInstances=0;
  std::array<AudioVoiceObservation50HD,instanceCount> voices{};
  // 0 means no instance.1..40 are response IDs;41 identifies the wing.
  std::array<std::uint32_t,instanceCount> soundIds{};
 };
 OriginalUiGarageWattoAudio50KG()=default;
 ~OriginalUiGarageWattoAudio50KG() noexcept {reset();}
 OriginalUiGarageWattoAudio50KG(const OriginalUiGarageWattoAudio50KG&)=delete;
 OriginalUiGarageWattoAudio50KG& operator=(const OriginalUiGarageWattoAudio50KG&)=delete;
 bool ready() const noexcept;
 bool quiet() const noexcept;
 bool inspect(Observation&) const noexcept;
 // Read-only aggregate preflight before shared RNG/model/source mutations.
 // Conservatively reserves free instances even if a later stop could free one.
 // A tick can select a response then wing: request2. Exhaustion never steals.
 bool canExecute(std::size_t maximumPlays,bool mayStopResponses) const noexcept;
 bool queryDuration(std::uint32_t event,float& actualSound20) const noexcept;
 const assets::OriginalStreamMetadataRow50KZ* eventMetadata(std::uint32_t event) const noexcept;
 bool playResponse(std::uint32_t event,std::uint32_t restart=0,std::uint32_t immediate=0) noexcept;
 // Original2924A0 visits forty Sounds in event order; each233670 stops that
 // Sound's most recent handle only. Older overlapping instances are retained.
 bool stopAllResponses() noexcept;
 // flags2 selects immediate command dispatch in26B5E0, NOT looping.
 bool playWing(std::uint32_t flags=2) noexcept;
 // Drain every finite instance at the actual widget destruction boundary.
 // Retains authenticated cold PCM/group allocation for the frontend lifetime.
 // Does not claim scene detachment or source widget reconstruction.
 bool drainInstances() noexcept;
 // Full native group teardown: drains ALL instances before owned PCM dies.
 void reset() noexcept;
private:
 friend class OriginalUiMainAudio50KG;
 friend class OriginalUiGarageWattoSoundGroup50KG;
 friend class OriginalUiGarageWattoOwner50KG;
 friend class OriginalAudioPendingWaves50KG;
 bool prepare(const std::filesystem::path&,std::shared_ptr<const assets::OriginalStreamMetadata50KZ>,AudioMixer50HD&,std::string&,const OriginalUiGarageDroidAudio50GN* sceneAudio=nullptr) noexcept;
 bool current() const noexcept;
 bool canRevise(std::uint64_t count) const noexcept;
 bool playSound(std::size_t sound) noexcept;
 bool publishNativeVoice(std::size_t sound,std::size_t& instance)noexcept;
 // Source fields exist only when the concrete2350D0 owner constructs THIS
 // retained Sound. Native channel tokens are revision-safe backend identities,
 // never fabricated PS2 channel/sample IDs. Unknown guest fields are absent.
 struct SourceSound {
  std::uint64_t reference00=1;
  OriginalUiGarageWattoSoundGroup50KG* group34=nullptr;
  std::uint32_t flags10=0,elapsedBits24=0,word7C=0;
  std::int32_t channel2C=-1;
  std::int32_t volume38=100,pan3C=0,pitch40=4096;
  std::int32_t volume44=100,pan48=0,pitch4C=4096,basePitch50=4096;
  float duration20=0.f;
  // Actual233194 filename copy, before stream lookup/nonstream registration.
  // Only bytes through the terminator are source-visible in this native owner.
  std::array<char,260> filenameA4{};
  bool stream1A8=false,stereo1AC=false,loop1B0=false;
  // Absent until actual2332A0 registration stores an authenticated directory
  // ordinal. This is NOT source sample28 or a native voice/sample handle.
  std::optional<std::uint32_t> resourceA0;
  // Typed successful native equivalent of source28, not a fabricated guest ID.
  // Negative/unavailable backend outcomes remain unadmitted in this slice.
  std::optional<OriginalAudioSampleCache50KG::NativeSampleHandle> nativeSample28;
 };
 struct Sound {
  assets::AudioPcm50HC pcm;
  AudioClip50HD clip{};
  const assets::OriginalStreamMetadataRow50KZ* metadata=nullptr;
  std::size_t latest=instanceCount;
  std::uint64_t latestRevision=0;
  std::int32_t latestToken=-1;
  std::optional<SourceSound> source;
  // Retained through source release until ALL actual native instances drain.
  // It is not cleared merely when the latest Sound handle is stopped.
  OriginalAudioSampleCache50KG::NativeSampleBinding nativeSample;
 };
 bool sourceSoundCurrent(const Sound&)const noexcept;
 // Same-call Group validation only, after its genuine full Audio::current.
 // Standalone Sound users must retain sourceSoundCurrent's full preflight.
 bool sourceSoundIdentityAfterCurrent(const Sound&)const noexcept;
 bool resolvedSourceSampleCurrent(const Sound& sound)const noexcept {
  AudioClip50HD clip;
  return sourceSoundCurrent(sound)&&sound.source->nativeSample28&&
   sound.nativeSample.clipForResolved(this,*sound.source->nativeSample28,clip);
 }
 bool sourceSoundActive(const Sound&)const noexcept;
 bool sourceSampleStartCurrent(std::size_t,const AudioClip50HD&,AudioMixer50HD::NativeSourceSampleStart50KG&)const noexcept;
 bool stopSource233670(Sound&)noexcept;
 bool serviceSource233E00(Sound&,float)noexcept;
 bool releaseSource104970(Sound&)noexcept;
 bool sourceGroupCold()const noexcept;
 bool sourceGroupConstructionCurrent()const noexcept;
 bool stoppedInstancesCurrent()const noexcept;
 bool canRearmRetiredSourceGroupRetainingHistory()const noexcept;
 bool rearmRetiredSourceGroupRetainingHistory()noexcept;
 bool retainedSourceRearm_=false;
 bool nativeSampleLeasesEmpty()const noexcept;
 bool canRearmRetiredSourceGroupCold()const noexcept;
 bool rearmRetiredSourceGroupCold()noexcept;
 std::array<Sound,soundCount> sounds_{};
 std::array<std::uint32_t,instanceCount> soundIds_{};
 std::array<std::uint64_t,instanceCount> playbackRevisions_{};
 // One strong backing per actual mixer clip. A stopped slot still stores its
 // raw clip, so retain until that slot is replaced or the transport resets.
 std::array<OriginalAudioSampleCache50KG::NativeSampleBinding,instanceCount> nativeVoices_{};
 std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata_;
 OriginalUiGarageWattoSoundGroup50KG* sourceGroup_=nullptr;
 // Once bound to real source construction, historical aggregate readiness is
 // permanently closed until every required original backend operation exists.
 bool sourceGroupAttempted_=false,sourceGroupReleased_=false;
 std::int32_t nextToken_=0;
 AudioMixer50HD* mixer_=nullptr;
 const OriginalUiGarageDroidAudio50GN* sceneAudio_=nullptr;
 std::size_t frontendVoices_=0;
 const OriginalSoundMode50KG* mode_=nullptr;
 std::uint64_t epoch_=0,revision_=0;
 std::uint32_t rate_=0;
 bool attempted_=false,prepared_=false,poisoned_=false,paused_=false;
};
}
