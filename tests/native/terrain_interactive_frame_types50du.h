#pragma once
// Context contains only references and synchronous borrowed calls. Keep cold
// loaders, renderer owners and frontend implementations out of its declaration.
// These three complete owners are needed only for their existing nested Borrow
// types; their ownership and retirement protocols are deliberately unchanged.
#include "../../src/game/host_race_limits50ht.h"
#include "../../src/game/pod_cables50fd.h"          // W0: was reached through the per-course collision headers
#include "../../src/game/impact_transitions50c.h"    // W0: same
#include "../../src/game/impact_effects50b.h"
#include "../../src/game/candidate_collection50bx.h"
#include "../../src/game/controller_feedback50kz.h"
#include "../../src/game/effect_dispatch50be.h"
#include "../../src/game/spring_records48n.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>

struct MovementRays50N;
namespace rr::assets {
struct EffectNames50BL;
struct FinalImpactAuthoring49Y;
struct PodFlameOwners50W;
}
namespace rr::game {
struct AiRuntimeReceipt50IV;
struct AiRuntimeState50IV;
struct AiTrackMetadata50IU;
class AiTrackMetadataView50GB;
class AiTrackMetadataView50IU;
class AiTrackMetadataView50MC;
class AiTrackMetadataView50TB;
class AiTrackMetadataView50TC;
struct AiTrackTuning50IU;
struct AuthoredBinding49N;
struct BasePoseHistoryState50Y;
struct BaseTailGlobals50X;
struct BaseTailState50X;
struct BaseTailWorkspace50X;
struct BaseUndertrackContinuation50X;
struct BoostDiagnostic50FK;
struct BoostParameters50FK;
struct BoostState50FK;
struct CollisionFilter48J;
class CollisionNormalModeOwner50JA;
struct ContactMaterial50HI;
class ControllerActions50EC;
class ControllerIngress50CX;
struct DamageSpeechEvent50HG;
struct DamageSpeechInput50HG;
struct DynamicContactEventBindings50IW;
struct DynamicImpactProfile50IW;
struct DynamicImpactTuning50IW;
struct EngineDamageState50IA;
struct EventContext48Z;
struct FrameTimingState49I;
struct GameplayBlurPending;
struct HostMonotonicClock50ID;
struct JoystickApplicationBridge48Z;
struct LiveZoneMembership50EN;
struct LiveZoneResult50EN;
struct LiveZoneWorkspace50EN;
struct LiveZoneWorld50EN;
struct MainBodyState49K;
struct PlayerEventEnvironment49A;
struct PlayerEventState49A;
struct PodSpeechState50HG;
struct RaceActorResolver50ID;
template<std::size_t Reserved> struct RaceEventQueue50ID;
struct ResetEffectBindings50BK;
struct ResetLifecycleGlobals50AT;
struct SecondaryActivationGlobals49V;
struct SharedRandomOwner50ID;
struct SpeechPosition50HG;
struct SpringCumulativePair48N;
struct SpringGeneratedParameters48K;
struct SpringResponseParameters48L;
struct StaticSpringState50G;
struct TerrainCandidateEffects48J;
struct TrackNavigation50EU;
struct TrackNavigationDiagnostic50EU;
struct TrackNavigationProgress50EU;
class TrackNavigationView50EU;
class TrackNavigationView50GB;
class TrackNavigationView50MC;
class TrackNavigationView50TB;
class TrackNavigationView50TC;
struct Type2PairMetadata49X;
struct Type2StaticCandidate49X;
struct Type2StaticDestination49X;
struct Type2StaticInitial49X;
struct Type2StaticQuery49X;
struct Type2StaticResult49X;
struct Type2StaticWorkspace49X;
struct Type3Candidate48P;
struct Type3QueryBuffers48P;
struct Type3QueryInput48P;
struct Type3QueryResult48P;
struct Type3QuerySource48P;
struct Type3QueryWorkspace48P;
struct UndertrackNode50ET;
struct UndertrackPhysicsDomain50EW;
struct UndertrackPhysicsWorkspace50EW;
struct UndertrackPlacement50ET;
struct UndertrackQueryOwners50EW;
struct UndertrackSelection50ET;
struct VehicleFrameState49W;
struct VehicleLiveQueryContext50EQ;
struct VehicleLiveQueryResult50EQ;
struct WrongWayProgressContinuation50GO;
struct WrongWayProgressDiagnostic50GO;
struct WrongWayProgressReceipt50GO;
struct ZoneLifecycle50CL;
struct ZoneRegistrationQueue50CQ;
struct ZoneRegistrationScratch50CJ;
namespace original_gc_gameplay_lighting_candidate { class Owner; struct Diagnostic; }
}
namespace authored_runtime_ground50cp_fixture { struct ColdScene; }
namespace checkpoint_recovery50fi_fixture { struct Plan; }
namespace lap_drive50GJ { struct Continuity; struct Owner; }
namespace live_audio50HF { class Owner; }
namespace live_terrain_probe50fr { struct Context; struct Owner; }
namespace material_audio50HI { class Provider; }
namespace native_ai50IV { struct Session; }
namespace native_dynamic_contact50IW { class Registry; struct Snapshot; }
namespace native_gc_other_particle_children50KG { struct Runtime; struct SharedAuthoring; }
namespace native_gc_water_racer50KG { class Owner; } // per-racer water runtime (R7; created only when the SCB names water)
namespace original_subtype3_record { class Owner; }
namespace race_audio50HQ { class Owner; }
namespace native_gc_water_session50KG { struct Application; class Owner; }
namespace native_original_ai_audio { class Owner; }
namespace native_original_ai_recovery { struct Diagnostic; struct Models; struct State; }
namespace native_original_human_recovery { struct Diagnostic; struct State; }
namespace native_racer50HN { class Bank; }
namespace native_racer50HW { class Cables; }
namespace opponent_audio50HX { struct BoostState; }
namespace original_ai_reset_ground_bridge { class GroundBridge; }
namespace race_mode1_host50ID { class Owner; }
namespace runtime_startup50fl { struct Fixture; }
namespace terrain_probe_authoring50fq_fixture { struct Owner; }
namespace track_height_cold50JB { struct Owner; }
namespace track_runtime_storage50IG { struct Counts; template<class T> class FixedBuffer; struct Storage; }
namespace undertrack_owners50ew_fixture { struct Owners; }
