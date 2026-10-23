#pragma once
#include "../../../src/assets/audio_assets50hc.h"
#include "audio_mixer50hd.h"
#include "../../../src/assets/original_audio_directory50kx.h"
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>

namespace rr::platform {
class OriginalAudioPendingWaves50KG;
class OriginalUiMainAudio50KG;
class OriginalUiGarageWattoAudio50KG;
class OriginalUiGarageWattoSoundGroup50KG;
class OriginalUiGarageDroidAudio50GN;
// Application-owned immutable backing and private native transport binding.
// Not original988C/DF0, an RPC receipt/sample28, or public playback admission.
class OriginalAudioSampleCache50KG final {
 struct Backing;
 friend class OriginalAudioPendingWaves50KG;
 friend class OriginalUiMainAudio50KG;
public:
 struct Observation {
  const assets::OriginalAudioDirectory50KX* directory=nullptr;
  std::uint32_t sourceOrdinal=0,encodedBytes=0,payloadBytes=0,sampleRate=0;
  std::uint64_t frames=0,nativeGeneration=0;
  std::uint32_t observedSourceMilliseconds=0,observedSourceDurationBits=0,observedSourcePitch=0;
 };
 // Resolved directory identity only. Weak ownership deliberately permits the
 // real root cache-reset-before-directory-clear order; this is not a voice lease.
 class NativeSampleHandle final {
  friend class OriginalAudioSampleCache50KG;
  std::weak_ptr<const Backing> backing_;
  std::weak_ptr<const OriginalAudioSampleCache50KG> cache_;
  std::uint64_t generation_=0;
 public:
  NativeSampleHandle()noexcept=default;
  bool current()const noexcept;
 };
 class NativeSampleBinding;
 class NativePcmLease final {
  friend class NativeSampleBinding;
  friend class OriginalAudioSampleCache50KG;
  std::shared_ptr<const Backing> backing_;
  std::weak_ptr<const OriginalAudioSampleCache50KG> cache_;
  std::uint64_t generation_=0;
 public:
  NativePcmLease()noexcept=default;
  bool current()const noexcept;
  bool inspect(Observation&)const noexcept;
  bool sameBacking(const NativePcmLease&)const noexcept;
  // No public raw samples/AudioClip escape or playback method. The private
  // transport binding retains this lease; active-voice joins remain separate.
 };
 // Native backend binding, not a guest sample number or a completed Sound
 // factory. The actual transport alone can borrow its clip; every such borrow
 // must retain a copy of this binding until the corresponding voice is drained.
 class NativeSampleBinding final {
  friend class OriginalAudioSampleCache50KG;
  friend class OriginalUiMainAudio50KG;
  friend class OriginalUiGarageWattoSoundGroup50KG;
  friend class OriginalUiGarageWattoAudio50KG;
  friend class OriginalUiGarageDroidAudio50GN;
  NativePcmLease pcm_;
  std::weak_ptr<OriginalUiMainAudio50KG> main_;
  const OriginalUiGarageWattoAudio50KG* audio_=nullptr;
  const OriginalUiGarageDroidAudio50GN* droid_=nullptr; // Exactly one real transport owner.
  const AudioMixer50HD* mixer_=nullptr;
  std::uint64_t epoch_=0;
  std::uint32_t rate_=0;
  std::size_t voices_=0;
  bool clipFor(const OriginalUiGarageWattoAudio50KG*,AudioClip50HD&)const noexcept;
  bool clipForResolved(const OriginalUiGarageWattoAudio50KG*,const NativeSampleHandle&,AudioClip50HD&)const noexcept;
  bool clipForRetainedVoice(const OriginalUiGarageWattoAudio50KG*,AudioClip50HD&)const noexcept;
  bool clipForResolved(const OriginalUiGarageDroidAudio50GN*,const NativeSampleHandle&,AudioClip50HD&)const noexcept;
  bool clipForRetainedVoice(const OriginalUiGarageDroidAudio50GN*,AudioClip50HD&)const noexcept;
  bool retainsBacking()const noexcept; // Structural lifetime, even when backend epoch is stale.
 public:
  NativeSampleBinding()noexcept=default;
  bool current()const noexcept;
  bool inspect(Observation&)const noexcept;
  bool sameBacking(const NativeSampleBinding&)const noexcept;
 };
 ~OriginalAudioSampleCache50KG()noexcept;
 OriginalAudioSampleCache50KG(const OriginalAudioSampleCache50KG&)=delete;
 OriginalAudioSampleCache50KG& operator=(const OriginalAudioSampleCache50KG&)=delete;
 bool current()const noexcept;
private:
 struct Backing {
  assets::AudioPcm50HC pcm;
  std::shared_ptr<const assets::OriginalAudioDirectory50KX> directory;
  const assets::OriginalAudioDirectoryRow50KX* row=nullptr;
  std::uint32_t encodedBytes=0,payloadBytes=0,milliseconds=0,durationBits=0,pitch=0;
 };
 OriginalAudioSampleCache50KG()noexcept=default;
 static std::shared_ptr<OriginalAudioSampleCache50KG> prepareNativeBacking(
  const std::shared_ptr<OriginalAudioPendingWaves50KG>&,
  std::shared_ptr<const assets::OriginalAudioDirectory50KX>,
  const std::filesystem::path&,std::string&)noexcept;
 bool acquireWingBacking(const assets::OriginalAudioDirectoryRow50KX&,NativePcmLease&,std::string&)noexcept;
 bool bindWingNativeSample(const NativePcmLease&,const std::shared_ptr<OriginalUiMainAudio50KG>&,
  NativeSampleBinding&,std::string&)noexcept;
 bool acquireSceneBacking(const assets::OriginalAudioDirectoryRow50KX&,const assets::AudioClip50HC&,
  NativePcmLease&,std::string&)noexcept;
 bool bindSceneNativeSample(const NativePcmLease&,const std::shared_ptr<OriginalUiMainAudio50KG>&,
  NativeSampleBinding&,std::string&)noexcept;
 bool makeNativeHandle(const NativeSampleBinding&,NativeSampleHandle&)const noexcept;
 bool bindingMatchesHandle(const NativeSampleBinding&,const NativeSampleHandle&)const noexcept;
 bool owns(const NativeSampleHandle&)const noexcept;
 bool canRetireNativeBacking()const noexcept;
 bool resetNativeBacking(std::string&)noexcept;
 bool closeNativeBacking(std::string&)noexcept;
 // Only exact Main stopped-voice records may account for residual leases.
 // Every other strong copy, including an external NativePcmLease, rejects.
 bool canCloseAfterStoppedNativeVoices(const OriginalUiMainAudio50KG&,
  const NativeSampleBinding* const*,std::size_t)const noexcept;
 // Private commit suffix: callers have preflighted all lease counts and have
 // removed every corresponding raw Mixer clip before dropping the bindings.
 void closeNativeBackingPreflighted()noexcept;
 bool owns(const NativePcmLease&)const noexcept;
 std::weak_ptr<OriginalAudioPendingWaves50KG> namespace_;
 std::weak_ptr<const OriginalAudioSampleCache50KG> self_;
 std::shared_ptr<const assets::OriginalAudioDirectory50KX> directory_;
 std::shared_ptr<const Backing> wing_;
 std::array<std::shared_ptr<const Backing>,4> scene_; // Actual four authored nonstream Droid samples.
 std::filesystem::path audioRoot_;
 std::thread::id ownerThread_=std::this_thread::get_id();
 std::uint64_t generation_=1;
 bool initialized_=false,closed_=false;
};
}
