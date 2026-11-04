#pragma once
#include <cstdint>
#include <memory>

namespace checkpoint_recovery50fi_fixture { struct Plan; }
namespace lap_drive50GJ { struct Continuity; }
namespace native_gc_water_session50KG { struct Application; }
namespace original_main_entry50KG { struct Application; }
namespace original_course_targets50KX { class ColdLoadRequest; class ConsumedLaunch; }
namespace windows_capture50LD { class Run; }
namespace rr::game {
struct HostMonotonicClock50ID;
class CollisionNormalModeOwner50JA;
struct RaceAudioSettings50IC;
struct ControlPresets49D;
struct SharedRandomOwner50ID;
}
namespace rr::platform {
enum class HostTrack50IM : std::uint8_t;
struct HostTrackAdmission50IM;
struct GameHostDiagnostic50ER;
struct VehicleWindowDiagnostic50DS;
}

// Original exit values; shared without importing any gameplay Context.
enum class SessionExit50DU { Quit, Restart, Checkpoint, ReturnOriginalMain, CourseColdLoad, ResultsHallReturn, PauseMenuReturn };

// Original main-TU functions, including the one monotonic clock epoch.
// This borrowed service view owns no state and is consumed synchronously.
struct SessionServices50DU {
    std::uint64_t (*monotonicNow50DU)() noexcept;
    void (*reportHost50DU)(const rr::platform::GameHostDiagnostic50ER&);
    void (*reportRenderer50DU)(const rr::platform::VehicleWindowDiagnostic50DS&);
};

SessionExit50DU runSession50DU(const char* corpusPath,bool headlessLive,const char* capturePath,
    const checkpoint_recovery50fi_fixture::Plan* checkpointInput,checkpoint_recovery50fi_fixture::Plan& checkpointOutput,
    lap_drive50GJ::Continuity& lapContinuity,std::uint64_t sessionIndex,
    rr::game::HostMonotonicClock50ID& applicationClock,
    rr::game::CollisionNormalModeOwner50JA& collisionNormalMode,bool startAtMenu,std::uint32_t& selectedRacer60,
    const rr::game::RaceAudioSettings50IC& effectiveAudioSettings,
    rr::game::RaceAudioSettings50IC& selectedAudioSettings,
    rr::platform::HostTrack50IM effectiveTrack,rr::platform::HostTrack50IM& selectedTrack,
    const rr::platform::HostTrackAdmission50IM& trackAdmission,
    const original_main_entry50KG::Application& originalMainApplication,
    bool enterOriginalUi,rr::game::ControlPresets49D& controlPresets,rr::game::SharedRandomOwner50ID& directApplicationRandom,
    const std::int32_t& cameraStartup350,original_course_targets50KX::ColdLoadRequest& courseColdLoad,
    const std::shared_ptr<native_gc_water_session50KG::Application>& nativeWaterGlobals,windows_capture50LD::Run& captureRun,
    original_course_targets50KX::ConsumedLaunch* consumedLaunch,const SessionServices50DU& services);
