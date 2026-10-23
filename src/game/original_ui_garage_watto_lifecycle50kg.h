#pragma once
#include "../assets/original_ui_garage_watto_assets50kg.h"
#include "model_playback50kg.h"
#include "lap_host_clock50id.h"
#include "shared_random_owner50id.h"
#include "original_shared_rng50iu.h"
#include <array>
#include <memory>
namespace rr::game {
enum class OriginalUiGarageWattoOperation50KG : std::uint8_t { prepare,entry,tick,noMoney,slider,noPurchase };
enum class OriginalUiGarageWattoResponse50KG : std::uint8_t { noMoney=1,slider=2,noPurchase=3 };
enum class OriginalUiGarageWattoLifecycleError50KG : std::uint8_t {
    none,input,alreadyPrepared,unprepared,notEntered,poisoned,preflight,clock,range,audio,observer,base,motion,allocation,drain
};
struct OriginalUiGarageWattoLifecycleDiagnostic50KG {
    OriginalUiGarageWattoLifecycleError50KG error=OriginalUiGarageWattoLifecycleError50KG::none;
    std::uint32_t sourcePC=0;
    ModelPlaybackDiagnostic50KG playback{};
    ModelMotionDiagnostic49T overlay{};
};
struct OriginalUiGarageWattoFields50KG {
    bool exitA0=false,readyA4=false;
    std::uint32_t sideA8=1;
    std::uint8_t lastRandomEventEC=0;
    float wingPhaseF0=0,idleCooldownF4=2,sideCooldownF8=30;
    std::array<std::int32_t,5> historyFC{{-1,-1,-1,-1,-1}};
};
struct OriginalUiGarageWattoReceipt50KG {
    bool effectsStarted=false,eligible=false,accepted=false,seeded=false;
    std::uint32_t draws308030=0,event=0;
};
// Required concrete scene/audio operations. Preflight is READ ONLY and must
// establish the complete eligible resource/voice-capacity closure before any
// RNG, sound, observer or source-state mutation. A metadata-only callback is
// NOT a native backend. The real scene owner supplies the already constructed
// Watto model (child mask2, flag190 bit8, authored transform/materials), retains
// graph/bindings and implements observer membership. The real audio owner
// supplies the enabled source-sized group,40 Sound objects and wing Sound at
// volume17. These obligations are not replaced by this numeric owner.
struct OriginalUiGarageWattoBackend50KG {
    void* context=nullptr;
    bool (*preflight)(void*,OriginalUiGarageWattoOperation50KG,
        const assets::OriginalUiGarageWattoAssets50KG&,const assets::MotionNodeBindings49O&,
        const SceneGraph49P&) noexcept=nullptr;
    bool (*bindObserver1D4800)(void*,const SceneGraph49P&) noexcept=nullptr;
    bool (*baseEntry2A78C0)(void*) noexcept=nullptr;
    bool (*notifyObserver28EA20)(void*,const SceneGraph49P&) noexcept=nullptr;
    bool (*baseTick141ED0)(void*,float) noexcept=nullptr;
    bool (*stopAllResponses)(void*) noexcept=nullptr; //2924A0: latest handle of each Sound only.
    bool (*playResponse)(void*,std::uint32_t event,std::uint32_t restart,std::uint32_t immediate) noexcept=nullptr;
    bool (*responseDuration)(void*,std::uint32_t event,float& sound20) noexcept=nullptr;
    bool (*playWing)(void*,std::uint32_t flags) noexcept=nullptr; //233410(flags2), not a loop bit.
    // Drain actual queued work, detach observer/model/audio ownership before
    // descriptor backing is freed. Must also work after partial-effect poison.
    bool (*drainDetach)(void*) noexcept=nullptr;
};
// Shared numeric tail of the genuine Widget+78. The concrete caller owns the
// preceding 2A78C0 and actual MotionLibrary lookups. This does not bind an
// Observer, claim sound readiness, recapture155730 or construct private shared
// Application state. A failure after155800 preserves that reached state.
bool enterOriginalGarageWattoMotion1D5CA850KG(const ModelPlaybackMotion50KG&,
    const assets::MotionNodeBindings49O&,const SceneGraph49P&,const ModelPlaybackIk50KG&,
    ModelPlayback50KG&,ModelMotionSharedState49T&,ModelMode0IkShared50KG&,
    ModelPlaybackWorkspace50KG&,OriginalUiGarageWattoFields50KG&,
    OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;

// Isolated synchronous source lifecycle, not Garage route/session admission.
// Executes actual shared50KG playback and masked1559F0 against the caller's
// real graph. No private RNG/clock, synthesized duration, skipped animation,
// storage, purchase response0, renderer or audio transport is introduced.
// Serial access; all borrowed owners and graph/bindings stay alive/disjoint.
// Invalid input/preflight failure leaves state unchanged. Any failure after
// effects begin poisons this owner; it must be drained, never retried/replayed.
class OriginalUiGarageWattoLifecycle50KG final {
public:
    OriginalUiGarageWattoLifecycle50KG() noexcept;
    ~OriginalUiGarageWattoLifecycle50KG();
    OriginalUiGarageWattoLifecycle50KG(const OriginalUiGarageWattoLifecycle50KG&)=delete;
    OriginalUiGarageWattoLifecycle50KG& operator=(const OriginalUiGarageWattoLifecycle50KG&)=delete;
    bool prepare(std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>,
        const assets::MotionNodeBindings49O&,const SceneGraph49P&,ModelMotionSharedState49T&,
        ModelMode0IkShared50KG&,CoupledNoiseContext49M&,SharedRandomOwner50ID&,HostMonotonicClock50ID&,
        OriginalUiGarageWattoBackend50KG,OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;
    bool startEntry(OriginalUiGarageWattoReceipt50KG&,OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;
    bool advance(float,OriginalUiGarageWattoReceipt50KG&,OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;
    bool respond(OriginalUiGarageWattoResponse50KG,OriginalUiGarageWattoReceipt50KG&,
        OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;
    bool drain(OriginalUiGarageWattoLifecycleDiagnostic50KG&) noexcept;
    bool prepared() const noexcept;
    bool entered() const noexcept;
    bool poisoned() const noexcept;
    const OriginalUiGarageWattoFields50KG* fields() const noexcept;
    const ModelPlayback50KG* playback() const noexcept;
    const assets::OriginalUiGarageWattoMotion50KG* currentMotion() const noexcept;
    float remainingCurrentMotion() const noexcept; //1D5C10, not an unconditional exit-ready receipt.
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
