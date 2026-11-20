#pragma once
// Genuine Player2255B0 ->1A24F0 ->216A30 from228BBC or controller1A17A4.
// No AI role/phase substitution, host checkpoint or grid placement occurs.
#include "original_ai_physical_recovery.h"
#include "../../src/game/original_human_engine_reset50lb.h"
#include "../../src/game/vehicle_base_tail50x.h"

namespace native_original_human_recovery {
using namespace rr::game;
namespace visibility=original_gc_gameplay_lighting_candidate;
// These three/five model-bound descriptors are the SAME physical-model
// projection shared by both216A30 callers, not AI state or an AI controller.
using Models=native_original_ai_recovery::Models;
enum class Stage : std::uint8_t {
    Idle,Select,Placement,PlayerPrefix,Controller,BasePrefix,RemoveVisibility,
    RemoveCollision,InitialPhysics,PreparePod,AttachVisibility,AttachCollision,
    BeginGround,Ground,GroundDiagnostic,FinishPhysics,FinishVisibility,Complete
};
struct State {
    Stage stage=Stage::Idle;
    UndertrackResetDomain50EW domain=UndertrackResetDomain50EW::PlayerPostPackedBytes50S;
    std::uint64_t serial=0,physicalCompleted=0;
    UndertrackProgress50ET selectionProgress{};
    UndertrackSelection50ET selection{};
    UndertrackPlacement50ET placement{};
    UndertrackRegistryProgress50EW registry{};
    ResetProbe50AG probe{};
    ResetGroundResult50IZ ground{};
    std::uint32_t vehicle850=0;
    bool vehicle850Known=false;
};
struct Diagnostic {
    std::uint32_t sourcePC=0;
    const char* message="human physical reset has not been requested";
    UndertrackDiagnostic50ET placement{};
    UndertrackResetResult50EW physics{};
    visibility::Diagnostic visibility{};
    ResetGroundDiagnostic50IZ ground{};
};
// Common rewrite: the ONE human recovery owner set for every course, over the
// map view of the one navigation store (retained by value; it cannot be
// reseated inside this owner). Keep the same immutable selection across an
// unfinished State; State itself remains the original bank-safe scalar continuation.
struct Owners {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const NavigationView50CR mapTrack;
    const UndertrackPhysicsDomain50EW& height;
    const SecondaryActivationGlobals49V& activation;
    const ResetLifecycleGlobals50AT& lifecycle;
    const VisualVector49R& targetRelative2F0;
    const UndertrackQueryOwners50EW& queries;
    ZoneLifecycle50CL& collisionLifecycle;
    LiveZoneWorld50EN& collisionWorld;
    LiveZoneWorkspace50EN& collisionWorkspace;
    original_ai_reset_ground_bridge::GroundBridge& ground;
    Models& models;
    const ResetEffectBindings50BK& effects;
    EffectOwner50BE* effectOwners=nullptr;
    std::size_t effectOwnerCount=0;
};

namespace ordinary_navigation_detail50MB=native_original_ai_recovery::ordinary_navigation_detail50MB;
bool navigationReadyOrdinaryShared50MB(const Owners& owners) noexcept;

bool fail(Diagnostic& d,std::uint32_t pc,const char* message) noexcept;
// Serial inside the actual human arena. Source-completed slices survive
// failure; callers must not replay Player input/navigation/packing. The camera
// tail runs separately only AFTER this lease has released the visibility world.
// No allocation, guest execution or replacement controls are used.

bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;

// The actual228BBC continuation supplies its own saved pre-reset base pose.

bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;

// Distinct1A17A4 event call. It never manufactures a base-frame cursor.

bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;

} // namespace native_original_human_recovery
