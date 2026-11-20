#pragma once
// Common rewrite (W0 freeze): the ONE consumed race stage, for every course.
// Plan PLAN.md 2.1 steps 1-2, B-C04. Replaces the 13 per-course stage headers
// original_consumed_{ra,ta,ga,gc,sa,ma,mb,sc,gb,mc,tb,rc,tc}_stage50du.h; built
// from the common-route (SA) stage, whose begin already runs the original order:
//   audio counted storage -> 1A65C0 constructConsumedRaceCold1A65C0 -> 28C publish
//   -> 1A6AA0 beginConsumedRaceLoad1A6AA0 -> 1AA5A0 populateConsumedRaceResources1AA5A0
//   -> 1A6B5C constructRaceGroups1A6B5C -> 1A6BC0 constructConsumedRaceWorld1A6BC0.
// The launch is matched by its course code as data (stock or registry row), never
// by a per-course HostTrack value. The track load (1A9EE0) runs next, through
// Application::loadConsumedRaceTrack1A9EE0, into the World this stage built.
//
// Owner: R1. Declarations frozen by W0; R1 defines begin/close in the common
// race-application cold file (renamed from ra_application_cold50la.cpp).
#include "common_race_world50cr.h"
#include "../../platform/linux/native/audio_output50hd.h"
#include "../../src/assets/original_standard_material_table50la.h"
#include <cstdint>
#include <memory>

namespace original_main_entry50KG { struct Application; }
namespace original_course_targets50KX { class ConsumedLaunch; }
namespace live_audio50HF { class Owner; }
namespace rr::game { class OriginalRaceConstructor50LA; class OriginalRacerModelRenderGlobals50LA; }

namespace original_consumed_stage50DU {

// Declare before the runtime/GPU borrowers and after its audio owner (as today):
// the last fallback runs only after those later owners released their borrows.
class Owner final {
    const original_main_entry50KG::Application& application_;
    live_audio50HF::Owner& audio_;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    std::shared_ptr<common_track50CR::Track> track_;
    bool attempted_=false;
public:
    // Diagnostics of the last close (filled by close()).
    rr::platform::AudioOutputDiagnostic50HD closeAudioDiagnostic;
    rr::assets::OriginalStandardMaterialDiagnostic50LA closeMaterialDiagnostic;
    Owner(const original_main_entry50KG::Application& app,live_audio50HF::Owner& audio)noexcept;   // R1
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner()noexcept;                                                                           // R1: std::terminate if close() refuses (as today)
    // R1: steps 01-02 for every course. `track` is the race's (unloaded) Track;
    // its identity must name the launch's course code. Logs common_load step=01/02.
    bool begin(original_course_targets50KX::ConsumedLaunch& launch,std::uint64_t session,
        std::uint32_t racer,rr::game::OriginalRacerModelRenderGlobals50LA& modelGlobals,
        std::shared_ptr<common_track50CR::Track> track,
        rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::uint32_t& pc)noexcept;
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race()const noexcept {return race_;}
    const std::shared_ptr<common_track50CR::Track>& track()const noexcept {return track_;}
    bool attempted()const noexcept {return attempted_;}
    // R1: audio close, then Application::closeNativeConsumedRace (World retire 1B2870
    // inside), then the Track's own release (common_track50CR::Track::release).
    bool close()noexcept;
};

} // namespace original_consumed_stage50DU
