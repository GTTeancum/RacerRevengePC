#pragma once
#include "original_ui_main_audio50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include "../../../src/assets/original_ui_garage_watto_assets50kg.h"
#include <memory>
#include <string>
#include <string_view>
namespace rr::platform {
class OriginalUiMainAudio50KG;
class OriginalUiGarageWattoOwner50KG;
// Real2350D0 Entity in the SAME Application authority/25-slot registry. The
// Sound list holds actual retained WattoAudio records, not a detached ledger.
//Source235520 completes at real native ordered enqueue, not wet application.
//Accepted controls pin this same counted group. DSP and entry remain closed.
class OriginalUiGarageWattoSoundGroup50KG final : public std::enable_shared_from_this<OriginalUiGarageWattoSoundGroup50KG> {
 friend class OriginalUiGarageWattoOwner50KG;
 friend class OriginalAudioPendingWaves50KG;
 friend class OriginalUiMainAudio50KG;
 friend class OriginalUiGarageWattoAudio50KG;
 using C=game::OriginalWreckComponent50LA;
 using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
 using Sound=OriginalUiGarageWattoAudio50KG::Sound;
 bool constructGroup2350D0(std::shared_ptr<Grant>,C& actualWidget,
     std::shared_ptr<OriginalUiMainAudio50KG>,std::string&)noexcept;
 bool appendWing232DB0(std::string&)noexcept;
 // Explicit same-constructor continuation. appendWing pauses before the real
 //2332A0 registration; the actual root supplies its retained pending namespace.
 bool registerPendingWing2332A0(std::shared_ptr<OriginalAudioPendingWaves50KG>,std::string&)noexcept;
 bool wingRegistrationPending()const noexcept;
 bool storeWingVolume233830(std::string&)noexcept;
 bool wingFlushPending232EC0(const std::shared_ptr<OriginalAudioPendingWaves50KG>&)const noexcept;
 bool wingResolved26D9F0(const std::shared_ptr<OriginalAudioPendingWaves50KG>&)const noexcept;
 Sound* appendResponse232DB0(std::uint32_t event,
     const assets::OriginalUiGarageWattoAssets50KG&,std::string&)noexcept;
 bool ownsResponse(std::uint32_t,const Sound*)const noexcept;
 // Only the synchronous Watto subtree pass uses this after its genuine
 // sourceSoundsComplete() check. No source mutation/callback may intervene.
 // Standalone callers still use ownsResponse(), which validates all custody.
 bool responseMemberIdentityAfterCurrent(std::uint32_t,const Sound*)const noexcept;
 bool configureReverb235520(std::string&)noexcept;
 bool reverbEnqueueCurrent()const noexcept;
 bool routingHistoryCurrent(bool released=false)const noexcept;
 bool routingSourceCurrent(bool released=false)const noexcept;
 bool queuedIdentityCurrent(const std::shared_ptr<Grant>&,std::uint64_t,std::int32_t,
     const OriginalUiMainAudio50KG*)const noexcept;
 std::uint32_t failureSourcePC()const noexcept{return failurePC_;}
 bool current()const noexcept;
 bool sourceSoundsComplete()const noexcept;
 const C* entity()const noexcept{return constructed_&&!retired_?&entity_:nullptr;}
 bool serviceGroup235720(float)noexcept;
 // Native borrow preflight only; does not retire source children or clear98A4.
 bool preflightPendingRetirement235150()const noexcept;
 bool closeGroup235150(std::string&)noexcept;
 Sound* append(std::size_t,std::string_view,std::string&)noexcept;
 std::shared_ptr<Grant> grant_;
 std::shared_ptr<OriginalUiMainAudio50KG> main_;
 std::shared_ptr<OriginalAudioPendingWaves50KG> pendingWaves_;
 OriginalUiGarageWattoAudio50KG* audio_=nullptr;
 C entity_{};
 C* widget_=nullptr;
 std::unique_ptr<Sound*[]> members4C_;
 std::size_t count48_=0,capacity54_=0;
 std::uint32_t growth50_=0,state44_=0;
 std::int64_t volume38_=0;
 std::int32_t slot40_=2;
 bool enabled30_=false,allocated58_=false;
 bool attempted_=false,constructed_=false,attached_=false,retired_=false,poisoned_=false;
 std::uint32_t failurePC_=0;
 bool reverbAttempted_=false;
 OriginalUiMainAudio50KG::ReverbEnqueueReceipt reverbEnqueue_;
 bool wingRegistrationComplete_=false; // Host receipt, never a guessed source sample ID.
public:
 OriginalUiGarageWattoSoundGroup50KG()noexcept=default;
 ~OriginalUiGarageWattoSoundGroup50KG()noexcept;
 OriginalUiGarageWattoSoundGroup50KG(const OriginalUiGarageWattoSoundGroup50KG&)=delete;
 OriginalUiGarageWattoSoundGroup50KG& operator=(const OriginalUiGarageWattoSoundGroup50KG&)=delete;
};
}
