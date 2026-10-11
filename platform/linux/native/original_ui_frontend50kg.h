#pragma once
#include "original_ui_options_session50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/assets/original_ui_localization50kg.h"
#include <chrono>
#include <memory>

namespace rr::platform {
enum class OriginalUiFrontendStatus50KG : std::uint8_t {
    ok,noChange,unavailableDestination,secondControllerRequired,routeAccepted,
    notPrepared,alreadyPrepared,notActive,busy,invalidInput,resource,localization,
    gpu,session,transition,renderer,cleanup,allocation,faulted,quit
};
struct OriginalUiFrontendDiagnostic50KG {
    OriginalUiFrontendStatus50KG status=OriginalUiFrontendStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiMainSessionDiagnostic50KG main{};
    OriginalUiOptionsSessionDiagnostic50KG options{};
    OriginalUiOptionsDrawDiagnostic50KG optionsResources{};
    assets::OriginalUiLocalizationDiagnostic50KG localization{};
    MaterialTextureGlDiagnostic50EB gpu{};
    VehicleWindowDiagnostic50DS renderer{};
    game::OriginalUiTransitionStatus50KG transition=game::OriginalUiTransitionStatus50KG::ok;
};
struct OriginalUiFrontendCold50KG {
    // Already-authenticated immutable original archives, retained by this owner.
    // English is decoded once from localization and retained for every lifetime.
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,mainMenu,options,
        fonts,controllerPopup,overlays,localization;
    OriginalUiMainVideoOwner50KG video{};
    assets::OriginalUiMaterialRuntime50KG runtime{};
    std::shared_ptr<OriginalUiMainProfile50KG> profile;
    std::shared_ptr<OriginalUiOptionsProfile50KG> optionsProfile;
    // Retained sole application profile for page4. Optional: without it only
    // Options menu1 is admitted. No reset or imported-save claim is made here.
    std::shared_ptr<game::OriginalUiGameSettingsProfile50KG> gameSettingsProfile;
    std::shared_ptr<OriginalUiMainApplication50KG> application;
    OriginalUiMainAudioMode50KG audioMode=OriginalUiMainAudioMode50KG::enabled;
    // Same actual transport/output owner throughout both screens. Caller owns
    // mixer/device and advances PCM only for an accepted update receipt.
    std::shared_ptr<OriginalUiMainAudio50KG> audio;
    // Exact live context/functions and explicit per-bank GPU budget. Samplers
    // must be empty here: every fresh bank supplies its authenticated policies.
    // video.lifetime must retain the actual context/callback storage.
    MaterialTextureGlRequest50EB textureRequest{};
};
struct OriginalUiFrontendColdInterval50KG {
    // Host loading interval only; no source clock is sampled or advanced here.
    // Exclude this exact interval from the next elapsed source update and
    // material clock accumulation, including a rejected cold preparation.
    bool measured=false;
    std::chrono::steady_clock::time_point begin{},end{};
    std::uint64_t nanoseconds=0;
};
struct OriginalUiFrontendInputReceipt50KG {
    OriginalUiFrontendStatus50KG status=OriginalUiFrontendStatus50KG::noChange;
    game::OriginalUiTransitionRoute50KG source{},requested{};
    bool routeAccepted=false;
    OriginalUiFrontendColdInterval50KG coldInterval{};
    OriginalUiMainSessionInputReceipt50KG main{};
    OriginalUiOptionsSessionInputReceipt50KG options{};
};
struct OriginalUiFrontendUpdateReceipt50KG {
    // True only after exactly one audio preparation, actual manager switch
    // case, and complete source child update. Caller advances its PCM output
    // once for this sequence using its corresponding accepted nanoseconds.
    bool accepted=false;
    std::uint64_t sequence=0;
    OriginalUiMainSessionUpdate50KG update{};
    game::OriginalUiTransitionRoute50KG source{};
};
struct OriginalUiFrontendSnapshot50KG {
    bool prepared=false,active=false,faulted=false,quitRequested=false;
    bool deferredQueue=false,frameSubmitted=false;
    game::OriginalUiTransitionRoute50KG route{},preparedDestination{};
    game::OriginalUiTransitionSnapshot50KG transition{};
    game::OriginalUiApplicationRequest50KG queuedRequest{};
    OriginalUiMainSessionSnapshot50KG main{};
    OriginalUiOptionsSessionSnapshot50KG options{};
    std::uint64_t updates=0,renders=0,submissions=0,switches=0;
};
// Concrete native Main4/1 <-> Options12/{1,4} owner. No generic destination-ready
// callbacks, guest execution, gameplay entry, fallback UI or profile reset.
// One freshly prepared bank/session/manager per screen lifetime; shared process
// profile/application/audio remain the same. Options page changes retain that
// screen's one bank/session/manager. All calls serialized/non-reentrant.
class OriginalUiFrontend50KG final {
public:
    OriginalUiFrontend50KG() noexcept;
    // Explicit close while the retained video context is current is mandatory,
    // including after partial prepare/activation failures that own GPU names.
    ~OriginalUiFrontend50KG() noexcept;
    OriginalUiFrontend50KG(const OriginalUiFrontend50KG&)=delete;
    OriginalUiFrontend50KG& operator=(const OriginalUiFrontend50KG&)=delete;
    OriginalUiFrontendStatus50KG prepare(const OriginalUiFrontendCold50KG&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Before renderer.create. Only sets original-UI fields; verifies the same
    // exact native context/currentness callbacks as the uploaded bank.
    OriginalUiFrontendStatus50KG configureRendererRequest(VehicleWindowRequest50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Renderer is borrowed until close; it must outlive this coordinator.
    // Initial notifications happen here, after the actual renderer accepts bank.
    OriginalUiFrontendStatus50KG activate(VehicleWindowGl50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG input(const OriginalUiMainSessionInput50KG&,
        OriginalUiFrontendInputReceipt50KG&,OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG update(const OriginalUiMainSessionUpdate50KG&,
        OriginalUiFrontendUpdateReceipt50KG&,OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG render(const OriginalUiMainFrameClock50KG&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    OriginalUiFrontendStatus50KG submit(VehicleWindowGl50DS&,std::int32_t width,
        std::int32_t height,OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Call after submit plus any swap/capture/readback consumers have returned.
    // Queued source's child/render work must be submitted first. Completes GPU
    // work, replaces bank, activates a fresh manager, then retires old owners.
    // Never invoked inside a manager callback or before the source frame ends.
    OriginalUiFrontendStatus50KG commitDeferred(VehicleWindowGl50DS&,
        OriginalUiFrontendDiagnostic50KG&) noexcept;
    // Closes the borrowed renderer first, then sessions and GPU owners. May be
    // retried only for unresolved cleanup; no source work can resume after it.
    OriginalUiFrontendStatus50KG close(OriginalUiFrontendDiagnostic50KG&) noexcept;
    void requestQuit() noexcept;
    OriginalUiFrontendSnapshot50KG snapshot() const noexcept;
    const OriginalUiMainSession50KG* mainSession() const noexcept;
    const OriginalUiOptionsSession50KG* optionsSession() const noexcept;
    const OriginalUiMainDrawResources50KG* mainResources() const noexcept;
    const OriginalUiOptionsDrawResources50KG* optionsResources() const noexcept;
    const assets::MaterialTextureAssets50EA* textures() const noexcept;
    const MaterialTextureGl50EB* gpu() const noexcept;
    const OriginalUiNativeDraw50KG* draw() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
