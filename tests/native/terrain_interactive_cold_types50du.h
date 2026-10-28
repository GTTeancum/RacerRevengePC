#pragma once
// Borrowed cold-stage declarations only. Complete providers below supply nested
// Borrow identities and canonical array extents; no root umbrella is imported.
#include "terrain_interactive_frame_types50du.h"
#include "gb_collision_world50gb.h"
#include "mc_collision_world50mc.h"
#include "native_racer_contact50hw_fixture.h"
#include "gc_animated_world50ig_fixture.h"
#include "ga_animated_world_fixture.h"
#include "ga_trigger_authoring_fixture.h"
#include "gc_trigger_authoring50in_fixture.h"
#include "ma_trigger_authoring_fixture.h"
#include "sa_trigger_authoring_fixture.h"
#include "sc_trigger_authoring50la.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/game/scene_graph49p.h"
#include <string>

namespace rr::assets { struct AiAuthoringDiagnostic50IU; class OriginalStandardMaterialDomain50LA; }
namespace rr::host {
struct CourseDriverDiagnostic50GK;
struct CourseDriverOutput50GK;
struct CourseDriverState50GK;
struct CourseDriverTuning50GK;
}
namespace rr::game {
struct AiFreshCreateSelection50IU;
struct AuthoredSelectionContext49N;
struct DevicePacket48Y;
struct DisabledProbeState49K;
struct DynamicContactProfile50IW;
struct InputEvent48Z;
struct InputEventQueue48Z;
struct ModelBoundsResult49O;
struct PlayerEventChildBinding49A;
struct PlayerEffectCall49A;
struct PodLiveQueryContext50IZ;
struct PodSceneRebaseDiagnostic50FB;
struct PodSceneRebaseModel50FB;
struct PodSceneRebaseWorkspace50FB;
struct ResetEffectState50BK;
struct ResetEffectWorkspace50BK;
struct VehicleTerrainProbe50FP;
namespace original_racer_selection50IJ { struct Entry; }
}
namespace rr::platform { struct KeyboardPacketDiagnostic50DR; }
namespace gb_animation_binding50GB { class Binding; struct Diagnostic; }
namespace mc_animation_binding50MC { class Binding; struct Diagnostic; }
namespace gc_gate_audio50IN { class Owner; }
namespace native_ai50IV { struct ObservationReads; }
namespace native_racer50HW { class Rig; class Visual; }
namespace native_racer50HN { class DrawOwner; }
namespace native_racer50IZ { struct FrameReads; }
namespace native_racer_model50LA { class Owner; }
namespace original_consumed_ra_stage50DU { class Owner; }
namespace original_main_entry50KG {
struct Application;
struct Video;
struct DamageColdResult50LA;
struct SpeedBoostColdResult50LA;
struct TimeStatsColdResult50LA;
}
namespace original_pause_cold50LA { class Owner; }
namespace original_position_stats_prestage50LA { class Owner; struct Diagnostic; }
namespace ra_cold_roster50LA { struct RuntimeOwners; class FreshProbe; }
namespace ra_native_session50LA { class Join; struct Diagnostic; }
namespace rc_navigation50RC { class RuntimeBorrow; }
namespace sc_runtime_ground50LA { class Owner; }
namespace source_announcer50IE { class Owner; }
namespace terrain_probe_authoring50fq_fixture { struct Bindings; }
namespace track_runtime_storage50IG { struct Membership; }
namespace undertrack_owners50ew_fixture { struct BorrowedQueries; }
