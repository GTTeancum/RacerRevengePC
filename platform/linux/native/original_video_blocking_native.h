#pragma once
#include "vehicle_window_gl50ds.h"
#include "game_host50er.h"
#include "../../../src/game/original_camera_start_effects.h"

namespace rr::platform {
enum class OriginalVideoNativePhase : std::uint8_t {
    idle, sourceReady, presentationPending, failed, restoring, retired, cancelled
};
enum class OriginalVideoNativeUse : std::uint8_t {
    blockingSourceLoops, asyncFrozenUnderlay25B5D0
};
// Concrete native display fade260850/261630. This is NOT gameplay blur2623E0.
// Borrows the live renderer and its exact GameHost50ER context; both outlive
// this adapter. The renderer retains all GL names, including failed work and
// the final effect image. No caller-supplied UI receipt is accepted here.
//
// Begin requires a real normal race publication. Blocking OUT additionally
// admits the actual displayed original Pause image, sealed privately by this
// renderer only after its real swap/headless completion. Shared host-menu
// targets or prepared Pause geometry alone never grant this authority.
// It snapshots that exact image once into a distinct immutable RGBA8 texture.
// Every sample recopies it into the unpublished alternate effect surface,
// applies the packed RGBAQ color with native GS-style integer byte blending,
// finishes, and either swaps through GameHost50ER or commits headlessly.
// Other renderer operations are locked out until real restore/retirement.
//
// The immutable caller-selected blend policies must explicitly admit
// signedFloor128, clamp8, PABE0 and packedSourceByte alpha storage. The
// source packet establishes its ALPHA expression, not COLCLAMP/PABE/FBA
// provenance. These are native lowering choices, never recovered GS state.
// Reusing the plain numeric policy type grants no packet/target authority.
// Unspecified/unsupported choices reject before native capture/allocation.
//
// Native policy: cover the full actual race viewport, in its existing pixel
// resolution/orientation. No PS2 scanout, field timing, or general GS parity
// is claimed. Root pauses simulation, handles events/quit, and continues the
// numeric owner's videoClockWait. A resize is an explicit admission failure;
// no changing image size or source capture is hidden inside the transaction.
// The explicit async option admits IN flags&7==6 or OUT flags&7==5 and
// native sourceCopyPC=0. Its caller owns real25B5D0 numeric samples over a
// frozen real race image, including the actual human-ending camera image.
// It does not claim async25B5D0 calls blocking261630/262D80. Use a separate
// backend for blocking and async effects. A successfully retired instance can
// later begin its same policy again, only after a new admitted race or
// completed original Pause publication. Async and blocking IN retain their
// existing race-target admission; those normal banks may already include Pause.
// This transport admission does not authorize a Pause input callback, close,
// Main request or claim that a source caller's full continuation is available.
class OriginalVideoBlockingNativeBackend final : public game::OriginalVideoBlockingBackend {
public:
    // No default mode or blend policy: every actual owner states its native
    // adaptation at construction. A retired reuse retains those same choices.
    OriginalVideoBlockingNativeBackend(VehicleWindowGl50DS&, GameHost50ER&,
        OriginalVideoNativeUse,NativeVideoPacketOverlayPolicies50LA) noexcept;
    ~OriginalVideoBlockingNativeBackend() noexcept override;
    OriginalVideoBlockingNativeBackend(const OriginalVideoBlockingNativeBackend&)=delete;
    OriginalVideoBlockingNativeBackend& operator=(const OriginalVideoBlockingNativeBackend&)=delete;
    game::OriginalCameraStartResult begin(game::OriginalVideoFadeDirection,
        const game::OriginalVideoFadeState&) override;
    game::OriginalCameraStartResult copyDrawAndComplete(const game::OriginalVideoBlockingSample&) override;
    game::OriginalCameraStartResult restoreAndComplete(game::OriginalVideoFadeDirection) override;
    // Explicit abort only: a checked glFinish and native state restoration.
    // Does not complete the numeric fade or write shared video flags/alpha.
    // Failed GL work cannot resume drawing; retire it and abandon that fade.
    // Returns false with ownership retained if retirement itself fails.
    bool cancelAndRetire() noexcept;
    OriginalVideoNativePhase phase() const noexcept {return phase_;}
    std::uint64_t completedSamples() const noexcept {return nextOrdinal_;}
    std::uint64_t windowSwaps() const noexcept {return windowSwaps_;}
    std::uint64_t headlessCompletions() const noexcept {return headlessCompletions_;}
    const VehicleWindowDiagnostic50DS& rendererDiagnostic() const noexcept {return diagnostic_;}
    const GameHostDiagnostic50ER& hostDiagnostic() const noexcept {return hostDiagnostic_;}
private:
    friend struct ::original_main_entry50KG::Application; // Exact retained input renderer/context binding only.
    bool ownersCurrent() noexcept;
    bool pauseReturnSourceCurrent()noexcept; //Private real renderer admission before irreversible row9 effects.
    bool retire(bool cancelled) noexcept;
    game::OriginalCameraStartResult failure(std::uint32_t) const noexcept;
    VehicleWindowGl50DS& renderer_;
    GameHost50ER& host_;
    const OriginalVideoNativeUse use_;
    const NativeVideoPacketOverlayPolicies50LA blendPolicies_;
    bool blendPoliciesSupported()const noexcept {
        using P=NativeVideoPacketOverlayPolicies50LA;
        return blendPolicies_.rounding==P::Rounding::signedFloor128 &&
            blendPolicies_.colorRange==P::ColorRange::clamp8 &&
            blendPolicies_.pabe==P::Pabe::zero &&
            blendPolicies_.alphaStore==P::AlphaStore::packedSourceByte;
    }
    // Sealed by the out-of-line constructor, never caller-supplied callbacks.
    // Keeps optional GameHost link dependencies out of ordinary50DS clients.
    bool window_=false;
    bool (*checkHost_)(GameHost50ER&,bool,std::int32_t,std::int32_t,VehicleWindowDiagnostic50DS&) noexcept=nullptr;
    bool (*swapHost_)(GameHost50ER&,GameHostDiagnostic50ER&) noexcept=nullptr;
    VehicleWindowDiagnostic50DS diagnostic_{};
    GameHostDiagnostic50ER hostDiagnostic_{};
    OriginalVideoNativePhase phase_=OriginalVideoNativePhase::idle;
    game::OriginalVideoFadeDirection direction_=game::OriginalVideoFadeDirection::out260850;
    game::OriginalVideoBlockingSample last_{}, pending_{};
    std::uint64_t nextOrdinal_=0,windowSwaps_=0,headlessCompletions_=0;
    std::int32_t width_=0,height_=0;
    std::uint8_t pendingBank_=0;
};
}
