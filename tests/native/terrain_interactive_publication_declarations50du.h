#pragma once
// Borrowed publication members require their exact declared identities, not
// the live loop Context or cold/frontend implementation include environment.
#include "terrain_interactive_loop_types50du.h"
#include "terrain_interactive_cold_types50du.h"
#include "terrain_interactive_publication_types50du.h"
#include "terrain_interactive_frame_part50du.h"
// Nested types cannot be forward-declared independently of their owners:
// Session::GameplayFogRenderPhase and RiderProfile::CourseRaceSelection.
// The cold-types header likewise retains the original collision Owner::Borrow
// providers and canonical contact/renderer array extents.
// W0 common rewrite (I3): per-course owner declarations removed; the one
// track's role owners come from common_track50cr.h.
#include "original_camera_session50kx.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "../../src/game/engine_flare50gv.h"
#include "../../src/game/race_event_queue50id.h"
#include "json.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace rr::assets {
struct AudioBank50HC;
struct Mesh48B;
class OriginalTaIntroAuthoringCandidate;
class OriginalUiMaterialTemplates50KG;
class VehicleDrawAssets50CR;
struct WorldNormal50HK;
}
namespace rr::game {
struct AudioCommand50HE;
struct DamageEventCursor50ID;
struct DynamicContactEventCursor50IW;
struct DynamicContactPodEffect50IW;
struct LapEventCursor50ID;
class OriginalGcParticleRenderOwner50KG;
class OriginalPositionStatsHud50LA;
class OriginalRacerModelRenderGlobals50LA;
struct PodCableBindings50FD;
struct PodCableDiagnostic50FD;
struct PodCableMesh50FD;
struct PodCableWorkspace50FD;
struct RaceAudioSettings50IC;
struct VehicleDrawWorkspace50CM;
}
namespace rr::platform {
struct AudioOutputDiagnostic50HD;
class GameHost50ER;
struct GameHostDiagnostic50ER;
enum class HostTrack50IM : std::uint8_t;
struct HostTrackAdmission50IM;
class MaterialTextureGl50EB;
struct MaterialTextureGlDiagnostic50EB;
class OriginalVideoBlockingNativeBackend;
struct SelectedControlInput50KY;
struct SelectedControllerReceipt50KY;
}
namespace track_animation_window50AN { class Owner; }
namespace track_animation_runtime50AN { class Owner; struct Diagnostic; }
namespace gameplay_binder_window_candidate_fixture { class Owner; }
namespace track_animation_binding50AN { class Binding; struct Diagnostic; }
namespace gc_particle_materials50kg_fixture { class Owner; }
namespace track_uv_animation50uv_fixture { class Owner; }
namespace opponent_audio50HX { class Owner; }
namespace original_all8_frame { class Schedule; }
namespace original_camera_input50KX { class Owner; }
namespace original_camera_session50KX { struct AuthoredStorage; }
namespace original_course_targets50KX { class Catalog; class ColdLoadRequest; }
namespace original_main_entry50KG {
struct Frontend;
struct PositionStatsResourceResult50LA;
class RaceDisplayLifetime50LA;
}
namespace original_subtype3_record { class Owner; }
namespace original_track_sky50KX { class Owner; }
namespace phase_timing50GK { struct Interval; }
namespace race_audio50HQ { class Owner; }
namespace race_mode1_host50ID { struct Effects; }
namespace render_diagnostics50GQ { struct Interval; }
namespace sky50gw_fixture { class Owner; }
namespace track_materials50fa_fixture { class Owner; struct Diagnostic; struct Request; }
namespace track_session_assets50IG { struct SessionAssets; }
namespace track_source_fixture { enum class Track : std::uint8_t; }
namespace windows_capture50LD { class Run; }
