#pragma once
#include "original_ui_garage_watto_sound_group50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_audio_sample_cache50kg.h"
#include "../../../src/assets/original_audio_directory50kx.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <array>
#include <cstdint>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
// Actual Application-retained interface98A4 namespace, not a per-widget or
// detached ColdLogical domain. This bounded stage represents pending source
// registrations and bounded private positive native2332F0/233E90 resolution.
// Public phase/admission and active playback remain closed.
class OriginalAudioPendingWaves50KG final {
 using Application=::original_main_entry50KG::Application;
 using Authority=game::OriginalApplicationEntities50LA;
 using Group=OriginalUiGarageWattoSoundGroup50KG;
 using Sound=OriginalUiGarageWattoAudio50KG::Sound;
 using Scene=OriginalUiGaragePodScene50KG;
 using SceneSound=Scene::Sound;
 using Directory=assets::OriginalAudioDirectory50KX;
 friend struct ::original_main_entry50KG::Application;
 friend class OriginalAudioSampleCache50KG;
 friend class OriginalUiGaragePodScene50KG;
 // Exact members can register/check retirement, but cannot construct the
 // separate nested root capability needed for namespace root operations.
 friend bool Group::registerPendingWing2332A0(std::shared_ptr<OriginalAudioPendingWaves50KG>,std::string&)noexcept;
 friend bool Group::preflightPendingRetirement235150()const noexcept;
 friend bool Group::closeGroup235150(std::string&)noexcept;
 class RootOperation final {
  friend struct ::original_main_entry50KG::Application;
 public:
  enum class Kind {construct232EA0,clearGameplay17D86C,clearFrontend17DC98,retireInterface2668E0,nativeBackingPrepare,nativeBackingReset17D858,nativeWingResolve26D9F0,nativeSceneResolve1D1B9C};
  const Application* issuer()const noexcept{return issuer_;}
  std::shared_ptr<Authority> authority()const noexcept{return authority_.lock();}
  const game::OriginalApplicationAnimationClockView50KG& lifetime()const noexcept{return lifetime_;}
  std::thread::id ownerThread()const noexcept{return ownerThread_;}
  Kind kind()const noexcept{return kind_;}
 private:
  RootOperation(const Application&,const std::shared_ptr<Authority>&,Kind)noexcept;
  const Application* issuer_=nullptr;
  std::weak_ptr<Authority> authority_;
  game::OriginalApplicationAnimationClockView50KG lifetime_;
  std::thread::id ownerThread_=std::this_thread::get_id();
  Kind kind_=Kind::construct232EA0;
 };
 // Native-only historical custody, not source row+4 or a Sound retain.
 // Address values are captured while owners live and are NEVER dereferenced.
 struct NativeUnresolvedSceneReceipt {
  std::uintptr_t applicationIdentity=0,namespaceIdentity=0,directoryIdentity=0;
  std::uintptr_t sceneIdentity=0,groupIdentity=0,soundIdentity=0;
  std::uint64_t generation=0,sceneKey=0,groupKey=0,grantEpoch=0,abortRevision=0;
  std::uint32_t resourceA0=0;
  std::size_t soundOrdinal=4;
  std::int32_t groupSlot=-1;
  bool groupRetired=false;
 };
 struct Row {
  std::uint32_t count8=0; // Historical registrations, never a source retain.
  Sound* representative4=nullptr;
  Group* representativeGroup=nullptr; // Native lifetime proof, not guest data.
  SceneSound* sceneRepresentative=nullptr;
  Scene* sceneRepresentativeOwner=nullptr;
  // The two typed alternatives are exclusive in this SAME source row+4.
  bool hasRepresentative()const noexcept{return representative4||representativeGroup||sceneRepresentative||sceneRepresentativeOwner;}
  std::optional<OriginalAudioSampleCache50KG::NativeSampleHandle> resolvedNativeSample;
  std::optional<NativeUnresolvedSceneReceipt> nativeUnresolvedSceneReceipt;
 };
 explicit OriginalAudioPendingWaves50KG()noexcept=default;
 static std::shared_ptr<OriginalAudioPendingWaves50KG> prepare232EA0(
     const RootOperation&,std::shared_ptr<const Directory>,std::string&)noexcept;
 // Root-only access to the existing authority publication; no public registry
 // accessor, additional authority friendship, or second ownership slot.
 static std::shared_ptr<OriginalAudioPendingWaves50KG> retainForRoot(
     const RootOperation&,std::string&)noexcept;
 static bool applicationMayRetire(const Application&)noexcept;
 bool sameRoot(const RootOperation&)const noexcept;
 bool prepareNativeSampleBacking(const RootOperation&,std::string&)noexcept;
 bool acquireNativeWingBacking(Group&,OriginalAudioSampleCache50KG::NativePcmLease&,std::string&)noexcept;
 bool bindNativeWingSample(Group&,OriginalAudioSampleCache50KG::NativeSampleBinding&,std::string&)noexcept;
 bool flushNativeWing26D9F0(const RootOperation&,std::string&)noexcept;
 bool resolveNativeWing233E90(Group&,std::string&)noexcept;
 bool resetNativeSampleBacking(const RootOperation&,std::string&)noexcept;
 bool nativeBackingIdentityCurrent(const OriginalAudioSampleCache50KG&,const std::filesystem::path&)const noexcept;
 bool registerWattoWing2332A0(Group&,std::string&)noexcept;
 bool registerSceneSound2332A0(Scene&,std::size_t,std::string_view,std::string&)noexcept;
 bool acquireNativeSceneBacking(Scene&,std::size_t,OriginalAudioSampleCache50KG::NativePcmLease&,std::string&)noexcept;
 bool bindNativeSceneSample(Scene&,std::size_t,OriginalAudioSampleCache50KG::NativeSampleBinding&,std::string&)noexcept;
 // Private later loader phase. No actor-only or earlier Watto factory issues
 // this operation: genuine enclosing1CCBA0/1D1B9C authority remains required.
 bool flushNativeScene26D9F0(const RootOperation&,Scene&,std::string&)noexcept;
 // Private terminal exception retirement. Preflight is before the real
 // Group begin; commit is only after Scene config close/Group ref0. Neither
 // performs source26D960/26D9F0 nor resets/decrements historical count8.
 bool canAbortUnresolvedSceneNative(const Scene&)const noexcept;
 void abortUnresolvedSceneNativePreflighted(Scene&)noexcept;
 bool unresolvedSceneNativeRowsCurrent(const Scene&)const noexcept;
 bool nativeUnresolvedSceneReceiptCurrent(const Row&,std::size_t)const noexcept;
 // Pure device-close proof. Historical counts survive; only the exact four
 // sealed native-aborted Scene receipts may replace zero-count rows.
 bool nativeOutputRegistrationRowsQuiescent()const noexcept;
 bool canRetire(const Scene&)const noexcept;
 bool retireSceneBorrow(Scene&)noexcept;
 bool canRetire(const Group&)const noexcept;
 bool retireGroupBorrow(Group&)noexcept;
 bool clear26D960(const RootOperation&,std::string&)noexcept;
 bool retire2668E0(const RootOperation&,OriginalUiMainAudio50KG*,std::string&)noexcept;
 std::shared_ptr<OriginalAudioSampleCache50KG> nativeSamples_;
 std::weak_ptr<Authority> authority_;
 const Application* issuer_=nullptr;
 game::OriginalApplicationAnimationClockView50KG issuerLifetime_;
 std::shared_ptr<const Directory> directory_;
 std::array<Row,assets::originalAudioDirectoryRows50KX> rows_{};
 struct GroupBorrow {
  Group* watto=nullptr;Scene* scene=nullptr;
  bool empty()const noexcept{return !watto&&!scene;}
 };
 // Exactly the existing shared25 slots, each a typed exclusive borrow. This
 // is not a second registry or capacity and does not add a source retain.
 std::array<GroupBorrow,25> groupBorrows_{};
 std::thread::id ownerThread_=std::this_thread::get_id();
 std::uint64_t revision_=0;
 // Nonzero means this namespace is terminal-only. Reads and genuine final
 // retirement remain available; registration/flush/clear/reset cannot replay
 // positive-count historical rows after their native representative died.
 std::uint64_t nativeUnresolvedSceneAbortRevision_=0;
 bool closed_=false;
public:
 struct RowObservation {
  const assets::OriginalAudioDirectoryRow50KX* directory=nullptr;
  const Sound* representative=nullptr;
  const Group* representativeGroup=nullptr;
  std::uint32_t registrationCount=0;
  std::uint64_t revision=0;
  bool hasResolvedNativeSample=false,resolvedNativeSampleCurrent=false;
  const Scene* sceneRepresentativeOwner=nullptr;
  std::size_t sceneRepresentativeOrdinal=4;
  bool nativeRepresentativeRetired=false;
 };
 ~OriginalAudioPendingWaves50KG()noexcept;
 OriginalAudioPendingWaves50KG(const OriginalAudioPendingWaves50KG&)=delete;
 OriginalAudioPendingWaves50KG& operator=(const OriginalAudioPendingWaves50KG&)=delete;
 bool current()const noexcept;
 std::size_t rowCount()const noexcept;
 const assets::OriginalAudioDirectoryRow50KX* lookup26DAD0(std::string_view)const noexcept;
 bool inspect(std::uint32_t,RowObservation&)const noexcept;
 // Native cache/binding preparation alone is not source Sound/row resolution.
 bool flush26D9F0(std::string&)const noexcept;
};
}
