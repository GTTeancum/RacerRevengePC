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
namespace mc_cold_sources50LA { class Owner; }
namespace mc_original_world50LM { class Owner; }
namespace mc_trigger_authoring50MC { class Owner; }
namespace mc_collision_world50MC { class Owner; }
namespace mc_trigger_actions50MC { class Owner; }
namespace mc_particle_attachments50MC { class Owner; }
namespace mc_track_fog50MC { class Owner; }
namespace mc_trigger_geometry50MC { class Owner; }
namespace mc_particle_source50MC { class Owner; }
namespace mc_navigation50MC { class Owner; struct Diagnostic; }
namespace track_animation_runtime50AN { class Owner; }
namespace mc_particle_runtime50MC { class Owner; struct Diagnostic; }
namespace track_animation_binding50AN { class Binding; struct Diagnostic; }

namespace interactive_course_cold50DU {
struct McColdPrefix {
    const bool& mcOriginalDisc;
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
    std::shared_ptr<const mc_cold_sources50LA::Owner>& mcColdSources;
    std::shared_ptr<rr::game::original_gc_gameplay_lighting_candidate::Owner>& mcLightOwner;
    mc_navigation50MC::Owner& mcNavigation;
    mc_navigation50MC::Diagnostic& mcNavigationDiagnostic;
    std::shared_ptr<mc_trigger_authoring50MC::Owner>& mcTriggerAuthoring;
    std::shared_ptr<mc_trigger_geometry50MC::Owner>& mcTriggerGeometry;
    std::shared_ptr<track_animation_runtime50AN::Owner>& trackAnimations;
    std::unique_ptr<track_animation_binding50AN::Binding>& trackAnimationBinding;
    track_animation_binding50AN::Diagnostic& trackAnimationBindingDiagnostic;
    std::shared_ptr<mc_original_world50LM::Owner>& mcOriginalWorld;
    std::shared_ptr<mc_particle_source50MC::Owner>& mcParticleSource;
    std::size_t& mcExternalPayloadReservation;
    std::shared_ptr<mc_particle_attachments50MC::Owner>& mcParticleAttachments;
    std::shared_ptr<rr::game::OriginalTrackParticleCold50LA>& mcTrackParticles;
    std::shared_ptr<mc_particle_runtime50MC::Owner>& mcParticleRuntime;
    mc_particle_runtime50MC::Diagnostic& mcParticleRuntimeDiagnostic;
    std::shared_ptr<mc_trigger_actions50MC::Owner>& mcTriggerActions;
    std::shared_ptr<mc_collision_world50MC::Owner>& mcCollisionWorld;
    std::shared_ptr<mc_track_fog50MC::Owner>& mcFogOwner;
    interactive_frame50DU::BorrowedCall<void(std::size_t)> reserveMcEarly;
    void run() const;
};
} // namespace interactive_course_cold50DU
