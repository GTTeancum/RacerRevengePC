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
namespace ga_atm_window_fixture { class Owner; }
namespace track_animation_window50AN { class Owner; }
namespace track_animation_runtime50AN { class Owner; struct Diagnostic; }
namespace gameplay_binder_window_candidate_fixture { class Owner; }
namespace gb_atm_window_fixture { class Owner; }
namespace gb_cold_sources50LA { class Owner; }
namespace track_animation_binding50AN { class Binding; struct Diagnostic; }
namespace gb_original_world50LM { class Owner; }
namespace gb_particle_attachments50GB { class Owner; }
namespace gb_particle_authoring50GB { class Owner; }
namespace gb_particle_render50GB { class Owner; struct Diagnostic; }
namespace gb_particle_runtime50GB { class Owner; }
namespace gb_particle_visual50GB { class Window; }
namespace gb_track_fog50GB { class Owner; }
namespace gc_atm_window_fixture { class Owner; }
namespace gc_particle_materials50kg_fixture { class Owner; }
namespace ma_sa_atm_window_fixture { class Owner; }
namespace track_uv_animation50uv_fixture { class Owner; }
namespace mb_atm_window_fixture { class Owner; }
namespace mb_original_world50LM { class Owner; }
namespace mc_atm_window_fixture { class Owner; }
namespace mc_camera_assets50MC { class Owner; }
namespace mc_cold_sources50LA { class Owner; }
namespace mc_original_world50LM { class Owner; }
namespace mc_particle_attachments50MC { class Owner; }
namespace mc_particle_render50MC { class Owner; struct Diagnostic; }
namespace mc_particle_runtime50MC { class Owner; }
namespace mc_particle_source50MC { class Owner; }
namespace mc_particle_visual50MC { class Window; }
namespace mc_sky50MC { class Owner; }
namespace mc_track_fog50MC { class Owner; }
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
namespace ra_cold_sources50LA { struct DecodedSources; }
namespace race_audio50HQ { class Owner; }
namespace race_mode1_host50ID { struct Effects; }
namespace rc_atm_window_fixture { class Owner; }
namespace rc_cold_sources50LA { class Owner; }
namespace rc_crystal50RC { class Owner; }
namespace rc_original_world50LM { class Owner; }
namespace rc_particle_attachments50RC { class Owner; struct Diagnostic; }
namespace rc_particle_authoring50RC { class Owner; struct Diagnostic; }
namespace rc_particle_render50RC { class Owner; struct Diagnostic; }
namespace rc_particle_runtime50RC { class Owner; struct Diagnostic; }
namespace rc_particle_visual50RC { class Window; }
namespace render_diagnostics50GQ { struct Interval; }
namespace sc_atm_window_fixture { class Owner; }
namespace sc_cold_sources50LA { class Owner; }
namespace sc_original_world50LM { class Owner; }
namespace sc_zero_sky50KX { class Owner; }
namespace sky50gw_fixture { class Owner; }
namespace ta_original_world50LE { class Owner; }
namespace tb_atm_window_fixture { class Owner; }
namespace tb_cold_sources50LA { class Owner; }
namespace tb_original_world50LM { class Owner; }
namespace tb_pod_sound50TB { class Owner; }
namespace tc_atm_window_fixture { class Owner; }
namespace tc_cold_sources50LA { class Owner; }
namespace tc_global_sound50TC { class Owner; }
namespace tc_original_world50LM { class Owner; }
namespace track_materials50fa_fixture { class Owner; struct Diagnostic; struct Request; }
namespace track_session_assets50IG { struct SessionAssets; }
namespace track_source_fixture { enum class Track : std::uint8_t; }
namespace windows_capture50LD { class Run; }
