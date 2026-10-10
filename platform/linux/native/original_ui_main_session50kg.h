#pragma once
#include "original_ui_main_draw50kg.h"
#include "original_ui_main_audio50kg.h"
#include "../../../src/game/original_ui_main_camera50kg.h"
#include "../../../src/game/original_ui_main_particle_camera50kg.h"
#include "../../../src/game/original_ui_main_particle_geometry50kg.h"
#include "../../../src/game/original_ui_main_particle_render_state50kg.h"
#include "../../../src/game/original_ui_main_particle_lighting50kg.h"
#include "../../../src/game/original_ui_main_fire50kg.h"
#include <memory>

namespace rr::platform {
// This is the sole native profile subset, not a fabricated whole save file.
// An imported profile supplies these exact fields; no filesystem writes occur.
struct OriginalUiMainProfile50KG {
    bool known=false;
    std::uint32_t mode34=0,previousMode38=0;
    std::uint16_t savedSlot6E0=0;
    std::uint8_t availability11018=0,flags11054=0,playerBits1125C=0;
};
// Source1864B0/186EF0: initializes ONLY the Main fields proven by this reset.
// Preserve the high unlock bit and every other caller-owned profile field.
void resetOriginalUiMainProfileFields50KG(OriginalUiMainProfile50KG&) noexcept;
struct OriginalUiMainApplication50KG {
    // Must be the process owners shared with all other native consumers.
    bool known=false;
    std::uint32_t rawRandom308030=0,fireSerial433970=0;
    std::uint32_t globalFlags228=0;
    std::int32_t musicState44=0,musicState48=0;
    // Pending267BE0 transport is unsupported. Source -1 is a proven early
    // return, not a caller-certified successful no-op for an active request.
    std::int32_t transportE00=-1;
    std::uint32_t uiObject3D8=0;
};
enum class OriginalUiMainAudioMode50KG : std::uint8_t { enabled,muted };
enum class OriginalUiMainSessionStatus50KG : std::uint8_t {
    ok,noChange,unavailableDestination,secondControllerRequired,notBound,
    alreadyBound,invalidInput,unsupportedState,resource,widget,transition,
    aperture,camera,scene,particle,audio,draw,allocation,faulted,quit,routeProposed
};
struct OriginalUiMainSessionDiagnostic50KG {
    OriginalUiMainSessionStatus50KG status=OriginalUiMainSessionStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiMainDrawDiagnostic50KG draw{};
    game::OriginalUiMainWidgetDiagnostic50KG widget{};
    game::OriginalUiApertureDiagnostic50KG aperture{};
    game::OriginalUiMainSceneDiagnostic50KG scene{};
};
struct OriginalUiMainSessionAudioReceipt50KG {
    std::uint64_t submitted=0,suppressedByMutedMode=0,suppressedBySourceGate=0,failed=0;
};
struct OriginalUiMainSessionSnapshot50KG {
    bool bound=false,faulted=false,quitRequested=false,inputContainerShown=false;
    bool mainPageShown=false,debugChord748=false,framePublished=false;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::uint8_t promptMask42C=0;
    std::uint32_t packedBackground430=0;
    std::uint64_t updateCount=0,renderCount=0;
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApertureSnapshot50KG aperture{};
    game::OriginalUiExitTextSnapshot50KG exitText{};
    OriginalUiMainSessionAudioReceipt50KG audio{};
    game::OriginalUiMainFireReceipt50KG lastParticleAdvance{},lastFireEmit{};
};
struct OriginalUiMainSessionCold50KG {
    std::shared_ptr<OriginalUiMainDrawResources50KG> resources;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::shared_ptr<const assets::OriginalUiArchive50KG> mainArchive;
    std::shared_ptr<OriginalUiMainProfile50KG> profile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    // Explicit coordinator mode: the actual sole manager lifecycle owner.
    // It must be unbound at cold bind; an already-bound owner is rejected before
    // page/resource side effects. This session retains the same owner but never
    // binds or ticks it. Cold preparation emits no page notifications; the
    // coordinator binds this fresh owner only after
    // all real destination/page/render owners exist. nullptr keeps standalone
    // Main's existing initial-selection/tick behavior unchanged.
    std::shared_ptr<game::OriginalUiTransitionOwner50KG> externalTransition;
};
struct OriginalUiMainSessionInput50KG {
    // Original container enum0..15, never gameplay flags. Host maps real input
    // edges to these values. Main handles previous2,next3,confirm4; back6 is
    // genuinely unhandled and must not quit or substitute a host menu route.
    std::uint8_t controller=0,originalInput=0;
    bool popupActive=false,sourceInputBlocked=false,secondControllerPresent=false;
};
class OriginalUiMainSession50KG;
struct OriginalUiMainRouteProposal50KG {
    // A proposal is an unaccepted original route, never a readiness token.
    // No mode/player-bit/saved-slot/manager/audio effects occur on production.
    const OriginalUiMainSession50KG* source=nullptr;
    std::uint64_t serial=0;
    game::OriginalUiMainDecision50KG decision{};
    game::OriginalUiTransitionSnapshot50KG observedLifecycle{};
};
struct OriginalUiMainSessionInputReceipt50KG {
    bool containerReached=false,pageCallbackRan=false,pageAccepted=false;
    game::OriginalUiMainDecision50KG decision{};
    OriginalUiMainRouteProposal50KG routeProposal{};
    game::OriginalUiMovementNotification50KG movement{};
    OriginalUiMainSessionStatus50KG status=OriginalUiMainSessionStatus50KG::noChange;
};
struct OriginalUiMainSessionUpdate50KG {
    float elapsedSeconds=0;
    // Main itself has no pause branch: it consumes supplied source dt. The
    // native host freezes that clock when paused. A paused sample MUST have
    // elapsedSeconds==0; render must not advance material animations either.
    // Zero-delta lifecycle cases still run, exactly like an actual source tick.
    bool paused=false;
    // Results of the actual source input-level queries(7,6,5,4;2,63).
    // The host supplies its real held-button snapshot, not generated input.
    std::array<bool,4> debugChordHeld{};
};
// Concrete bounded Main lifecycle, tree update and render composition. The
// session owns callback implementations, shared+440 ScreenText and full Main
// close hooks; callers cannot certify arbitrary trace-only callbacks as effects.
// Standalone mode admits no Rider/Course/Options/Gallery destination. External
// mode returns proposals to the coordinator, which alone admits destinations
// and commits source profile/manager changes. Missing admission changes none.
// It never launches gameplay, executes guest code or samples a host clock.
class OriginalUiMainSession50KG final {
public:
    OriginalUiMainSession50KG() noexcept;
    ~OriginalUiMainSession50KG();
    OriginalUiMainSession50KG(const OriginalUiMainSession50KG&)=delete;
    OriginalUiMainSession50KG& operator=(const OriginalUiMainSession50KG&)=delete;
    OriginalUiMainSessionStatus50KG bind(const OriginalUiMainSessionCold50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    OriginalUiMainSessionStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiMainSessionInputReceipt50KG&,OriginalUiMainSessionDiagnostic50KG&) noexcept;
    // External mode only: one-use audio boundary, before coordinator tick.
    // update must follow with identical arguments and consumes this preparation
    // without advancing audio again. Duplicate preparation/mismatched consume
    // faults after any consumed audio, so the host cannot retry that frame.
    OriginalUiMainSessionStatus50KG prepareUpdateAudio(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    OriginalUiMainSessionStatus50KG update(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    // Owns source-prepared scene fog, adjusted particle camera, lighting and
    // canvas offsets. Callers cannot supply guessed fog or certify empty work.
    OriginalUiMainSessionStatus50KG render(const OriginalUiMainFrameClock50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    // Coordinator-only page boundaries. Notifications must be the actual
    // borrowed owner's synchronous callback and match its current snapshot.
    // Poll is read-only; update() skips manager tick in external mode, while
    // render() obtains lifecycle from that SAME owner, not a supplied snapshot.
    OriginalUiMainSessionStatus50KG notifyTransition(const game::OriginalUiTransitionNotification50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    game::OriginalUiTransitionCompletion50KG pollCompletion(game::OriginalUiTransitionRoute50KG,
        game::OriginalUiTransitionPhase50KG) const noexcept;
    const game::OriginalUiTransitionOwner50KG* externalLifecycleOwner() const noexcept;
    // Rejects stale/foreign proposals and any changed selection or lifecycle.
    // Invalidated by another input/update/render/notification. It certifies
    // only source facts, never destination readiness. Coordinator commits
    // requestMain and savedSlot6E0, then calls notifyRouteAccepted immediately,
    // before tick, for the exact once-only source SELECT1 effect. The latter
    // verifies the real manager request/profile writes and writes neither.
    bool routeProposalCurrent(const OriginalUiMainRouteProposal50KG&) const noexcept;
    OriginalUiMainSessionStatus50KG notifyRouteAccepted(const OriginalUiMainRouteProposal50KG&,
        OriginalUiMainSessionDiagnostic50KG&) noexcept;
    // Explicit host window/application quit, separate from original back6.
    void requestQuit() noexcept;
    OriginalUiMainSessionSnapshot50KG snapshot() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
    // Cold and scene-fog snapshots of ONE manager-owned original camera.
    // No second camera entity or PageInfo-derived pose is created.
    const game::OriginalUiMainCameraPreparation50KG* camera() const noexcept;
    const game::OriginalUiMainParticleCamera50KG* particleCamera() const noexcept;
    const game::OriginalUiMainParticleRenderState50KG* particleRenderState() const noexcept;
    const game::OriginalUiMainSceneOwner50KG* scene() const noexcept;
    const game::OriginalUiMainFireOwner50KG* fire() const noexcept;
    const game::OriginalUiMainParticleLighting50KG* lighting() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
