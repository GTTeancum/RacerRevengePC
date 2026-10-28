#pragma once
// Synchronous borrowed view; existing world locals retain ownership and order.
#include "native_borrowed_call50du.h"
#include <cstddef>
#include <cstdint>
#include <memory>
namespace rr::game { struct Type2StaticQuery49X; struct CollisionFilter48J;
struct VehicleFrameState49W; struct Type3QueryInput48P;
class OriginalRacerModelRenderGlobals50LA; }
namespace ra_cold_aggregate50LA { class Owner; struct Diagnostic; }
namespace ra_cold_roster50LA { struct SharedOwners; }
namespace track_session_assets50IG { struct SessionAssets; }
namespace original_course_targets50KX { class ConsumedLaunch; }
namespace original_main_entry50KG { struct Application; struct Video; }
namespace original_consumed_ra_stage50DU { class Owner; }
namespace undertrack_owners50ew_fixture { struct Owners; struct SourceGlobals; }
namespace terrain_probe_authoring50fq_fixture { struct Owner; }
namespace interactive_ra_cold50DU {
struct Context {
    std::shared_ptr<ra_cold_aggregate50LA::Owner>& raWorld;
    ra_cold_aggregate50LA::Diagnostic& raDiagnostic;
    rr::game::Type2StaticQuery49X& query;
    rr::game::CollisionFilter48J& qfilter;
    std::shared_ptr<ra_cold_roster50LA::SharedOwners>& raRosterOwners;
    const std::uint32_t& originalAiHumanCount;
    const std::uint32_t& raceRacerCount;
    rr::game::VehicleFrameState49W& s;
    const track_session_assets50IG::SessionAssets*& trackSession;
    original_course_targets50KX::ConsumedLaunch*& consumedLaunch;
    const bool& allAiEnabled;
    const bool& sourceConsumedIntroActivation;
    const bool& originalCameraEnabled;
    const interactive_frame50DU::BorrowedCall<void()>& openNativeVideo;
    const original_main_entry50KG::Application& originalMainApplication;
    std::shared_ptr<original_main_entry50KG::Video>& nativeVideo;
    std::unique_ptr<rr::game::OriginalRacerModelRenderGlobals50LA>& sourceModelRenderGlobals;
    std::uint32_t& nativeOrdinaryLaunch11258;
    original_consumed_ra_stage50DU::Owner& consumedRaStage;
    std::uint64_t& sessionIndex;
    const std::uint32_t& playerRacer60;
    const bool& sourceConsumedPauseActivation;
    undertrack_owners50ew_fixture::Owners& recoveryQueryOwners;
    undertrack_owners50ew_fixture::SourceGlobals& recoveryGlobals;
    terrain_probe_authoring50fq_fixture::Owner& terrainOwner;
    rr::game::Type3QueryInput48P& movement;
    interactive_frame50DU::BorrowedCall<void()> publishNativeRuntime;
    void run() const;
};
}
