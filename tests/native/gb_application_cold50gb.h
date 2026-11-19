#pragma once
// Synchronous borrowed references only; actual retained owners stay in world.
#include "native_borrowed_call50du.h"
#include <cstddef>
#include <cstdint>
#include <memory>
namespace original_main_entry50KG { struct Application; }
namespace track_session_assets50IG { struct SessionAssets; }
namespace runtime_startup50fl { struct Fixture; }
namespace native_gc_water_session50KG { struct Application; }
namespace track_runtime_storage50IG { struct Limits; }
namespace rr::game { struct VehicleFrameState49W; class OriginalTrackParticleCold50LA;
namespace original_gc_gameplay_lighting_candidate { class Owner; } }
namespace gb_cold_sources50LA { class Owner; }
namespace gb_original_world50LM { class Owner; }
namespace gb_trigger_authoring50GB { class Owner; }
namespace gb_collision_world50GB { class Owner; }
namespace gb_trigger_actions50GB { class Owner; }
namespace gb_particle_attachments50GB { class Owner; }
namespace gb_track_fog50GB { class Owner; }
namespace gb_particle_authoring50GB { class Owner; }
namespace gb_pod_sound50GB { class Owner; }
namespace gb_navigation50GB { class Owner; struct Diagnostic; }
namespace track_animation_runtime50AN { class Owner; }
namespace gb_particle_runtime50GB { class Owner; struct Diagnostic; }
namespace track_animation_binding50AN { class Binding; struct Diagnostic; }

namespace interactive_course_cold50DU {
struct GbColdPrefix {
    const bool& gbOriginalDisc;
    const bool& directOriginalHuman;
    const bool& originalAiEnabled;
    const bool& nativeOriginalFleet;
    const original_main_entry50KG::Application& originalMainApplication;
    const track_session_assets50IG::SessionAssets*& trackSession;
    rr::game::VehicleFrameState49W& s;
    runtime_startup50fl::Fixture& x;
    const std::uint32_t& raceRacerCount;
    std::uint64_t& sessionIndex;
    const std::shared_ptr<native_gc_water_session50KG::Application>& nativeWaterGlobals;
    track_runtime_storage50IG::Limits& trackStorageLimits;
    std::shared_ptr<const gb_cold_sources50LA::Owner>& gbColdSources;
    std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner>& gbLightOwner;
    gb_navigation50GB::Owner& gbNavigation;
    gb_navigation50GB::Diagnostic& gbNavigationDiagnostic;
    std::shared_ptr<gb_trigger_authoring50GB::Owner>& gbTriggerAuthoring;
    std::shared_ptr<track_animation_runtime50AN::Owner>& trackAnimations;
    std::unique_ptr<track_animation_binding50AN::Binding>& trackAnimationBinding;
    track_animation_binding50AN::Diagnostic& trackAnimationBindingDiagnostic;
    std::shared_ptr<gb_original_world50LM::Owner>& gbOriginalWorld;
    std::shared_ptr<gb_particle_authoring50GB::Owner>& gbParticleAuthoring;
    std::size_t& gbExternalPayloadReservation;
    std::shared_ptr<gb_particle_attachments50GB::Owner>& gbParticleAttachments;
    std::shared_ptr<rr::game::OriginalTrackParticleCold50LA>& gbTrackParticles;
    std::shared_ptr<gb_particle_runtime50GB::Owner>& gbParticleRuntime;
    gb_particle_runtime50GB::Diagnostic& gbParticleRuntimeDiagnostic;
    std::shared_ptr<gb_trigger_actions50GB::Owner>& gbTriggerActions;
    std::shared_ptr<gb_collision_world50GB::Owner>& gbCollisionWorld;
    std::shared_ptr<gb_track_fog50GB::Owner>& gbFogOwner;
    std::shared_ptr<gb_pod_sound50GB::Owner>& gbPodSound;
    const interactive_frame50DU::BorrowedCall<void(std::shared_ptr<const gb_trigger_authoring50GB::Owner>)>& prepareGbPodSound;
    interactive_frame50DU::BorrowedCall<void(std::size_t)> reserveGbEarly;
    void run() const;
};
} // namespace interactive_course_cold50DU
