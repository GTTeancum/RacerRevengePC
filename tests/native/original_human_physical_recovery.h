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
struct Owners {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigation50EU& mapTrack;
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
// The selected final view is retained by value and cannot be reseated inside
// this owner. Keep the same immutable selection across an unfinished State;
// State itself remains the original bank-safe scalar continuation.
struct Owners50MB {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50EU mapTrack;
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
// TC-CONSUMER-BEGIN 0
struct Owners50TC {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50TC mapTrack;
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
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
struct Owners50TB {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50TB mapTrack;
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
// TB-CONSUMER-END 0

// GB-CONSUMER-BEGIN 0
struct Owners50GB {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50GB mapTrack;
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
// GB-CONSUMER-END 0

// MC-CONSUMER-BEGIN 0
struct Owners50MC {
    visibility::Owner& visibility;
    std::uint32_t& cables254Field50;
    // Genuine19EBA0 reset fields share the existing human audio state.
    // D44 is the actual session pool's human row, never a fresh zero scratch.
    OriginalHumanEngineResetOwner50LB& controllerD40;
    UndertrackPlayerPrefix50ET& playerPrefix;
    std::uint32_t& materialD44Field24C;
    const TrackNavigationView50MC mapTrack;
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
// MC-CONSUMER-END 0

namespace ordinary_navigation_detail50MB=native_original_ai_recovery::ordinary_navigation_detail50MB;
// Constant-true legacy admission leaves every original guard/effect ordered.
bool navigationReadyOrdinaryShared50MB(const Owners&) noexcept;
bool navigationReadyOrdinaryShared50MB(const Owners50MB& owners) noexcept;
// TC-CONSUMER-BEGIN 1
bool navigationReadyOrdinaryShared50MB(const Owners50TC& owners) noexcept;
// TC-CONSUMER-END 1

// TB-CONSUMER-BEGIN 1
bool navigationReadyOrdinaryShared50MB(const Owners50TB& owners) noexcept;
// TB-CONSUMER-END 1

// GB-CONSUMER-BEGIN 1
bool navigationReadyOrdinaryShared50MB(const Owners50GB& owners) noexcept;
// GB-CONSUMER-END 1

// MC-CONSUMER-BEGIN 1
bool navigationReadyOrdinaryShared50MB(const Owners50MC& owners) noexcept;
// MC-CONSUMER-END 1


bool fail(Diagnostic& d,std::uint32_t pc,const char* message) noexcept;
// Serial inside the actual human arena. Source-completed slices survive
// failure; callers must not replay Player input/navigation/packing. The camera
// tail runs separately only AFTER this lease has released the visibility world.
// No allocation, guest execution or replacement controls are used.

bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners50MB& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-BEGIN 2
bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners50TC& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-END 2

// TB-CONSUMER-BEGIN 2
bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners50TB& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TB-CONSUMER-END 2

// GB-CONSUMER-BEGIN 2
bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners50GB& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// GB-CONSUMER-END 2

// MC-CONSUMER-BEGIN 2
bool resume2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,UndertrackResetDomain50EW domain,State& state,
    Owners50MC& owners,UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// MC-CONSUMER-END 2

// The actual228BBC continuation supplies its own saved pre-reset base pose.

bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners50MB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-BEGIN 3
bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners50TC& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-END 3

// TB-CONSUMER-BEGIN 3
bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners50TB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TB-CONSUMER-END 3

// GB-CONSUMER-BEGIN 3
bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners50GB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// GB-CONSUMER-END 3

// MC-CONSUMER-BEGIN 3
bool resume(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    const BaseUndertrackContinuation50X& base,State& state,Owners50MC& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// MC-CONSUMER-END 3

// Distinct1A17A4 event call. It never manufactures a base-frame cursor.

bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners50MB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-BEGIN 4
bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners50TC& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TC-CONSUMER-END 4

// TB-CONSUMER-BEGIN 4
bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners50TB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// TB-CONSUMER-END 4

// GB-CONSUMER-BEGIN 4
bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners50GB& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// GB-CONSUMER-END 4

// MC-CONSUMER-BEGIN 4
bool resetController2255B0(VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const VehicleFrameParameters49W& parameters,TrackNavigationProgress50EU& progress,
    std::uint64_t serial,State& state,Owners50MC& owners,
    UndertrackPhysicsWorkspace50EW& workspace,Diagnostic& diagnostic) noexcept;
// MC-CONSUMER-END 4

} // namespace native_original_human_recovery
