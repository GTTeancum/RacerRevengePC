// NEW unverified source reconstruction: read-only Confirm preflight.
#pragma once
#include "../assets/original_ui_rider_character_assets50kg.h"
#include "model_playback50kg.h"
#include "original_shared_rng50iu.h"
#include <array>

namespace rr::game {
// UI-specific1D2550..1D29F0 chronology. Model creation/attachment, rendering
// and observer callbacks remain the widget manager's responsibility. This
// owner never turns an absent model into destination readiness.
class OriginalUiRiderCharacter50KG final {
public:
    OriginalUiRiderCharacter50KG() noexcept=default;
    OriginalUiRiderCharacter50KG(const OriginalUiRiderCharacter50KG&)=delete;
    OriginalUiRiderCharacter50KG& operator=(const OriginalUiRiderCharacter50KG&)=delete;
    // Pin the caller-owned cold owner for this object's lifetime. Exact source
    // names, actual looping bits and actual MOT durations; no FPS timebase.
    bool bind(const assets::OriginalUiRiderCharacterAssets50KG&) noexcept;
    // Call after real model creation/root pose. Starts155800 then154480(.001).
    // Does not mark loaded: original observer attachment and parent callback
    // occur NEXT, before capture155730 and l_IK/r_IK registration156620.
    bool startEntry(const assets::MotionNodeBindings49O&,const SceneGraph49P&,
        ModelMotionSharedState49T&,ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,
        ModelPlaybackDiagnostic50KG&) noexcept;
    // Call after observer attachment and parent callback. Capture source155730
    // before endpoint registration. After registration/IK204, call modelAttached.
    bool captureAfterAttachment(const assets::MotionNodeBindings49O&,const SceneGraph49P&,
        ModelPlaybackDiagnostic50KG&) noexcept;
    bool modelAttached() noexcept;
    // After actual select2A78C0; source does not reset idle count/last idle/ready.
    void selected() noexcept {selectionRequested_=false;}
    // Only after drain/disable/observer removal/detach/destruction completed.
    void modelDestroyed() noexcept {loaded_=false;entryStarted_=false;captured_=false;playback_={};}
    // Executes the real transition leaf on copied playback; no live state or RNG writes.
    bool preflightConfirm(const assets::MotionNodeBindings49O&,const SceneGraph49P&,
        ModelPlaybackDiagnostic50KG&) const noexcept;
    bool confirm(const assets::MotionNodeBindings49O&,const SceneGraph49P&,
        ModelPlaybackDiagnostic50KG&) noexcept;
    // Poll >80% BEFORE advance, real shared RNG, then shared native playback.
    // On success caller must perform observer28EA20, then children141ED0.
    // Even unloaded updates still require children141ED0 in the manager.
    bool advance(float dt,const assets::MotionNodeBindings49O&,const SceneGraph49P&,
        const ModelPlaybackIk50KG&,CoupledNoiseContext49M& applicationRandom,
        ModelMotionSharedState49T&,ModelMode0IkShared50KG&,ModelPlaybackWorkspace50KG&,
        ModelPlaybackDiagnostic50KG&) noexcept;
    bool modelLoaded() const noexcept {return loaded_;}
    bool sourceReadiness1D2710() const noexcept {return !loaded_||ready_;}
    bool selectionRequested() const noexcept {return selectionRequested_;}
    std::uint32_t idleObservationCount() const noexcept {return idleCount_;}
    const ModelPlayback50KG& playback() const noexcept {return playback_;}
private:
    const assets::OriginalUiRiderCharacterAssets50KG* assets_=nullptr;
    // Stable descriptors: entry,selection,breathe,idle1,idle2,idle3.
    std::array<ModelPlaybackMotion50KG,6> motions_{};
    ModelPlayback50KG playback_{};
    bool loaded_=false,entryStarted_=false,captured_=false,selectionRequested_=false,ready_=false;
    std::uint32_t idleCount_=0;
    std::uint8_t lastIdle_=0; // source string NONE before any idle choice
};
}
