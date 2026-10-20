#pragma once
#include "vehicle_live_query50eq.h"
#include "indexed_type2_contact50cc.h"
#include "source_trigger_entry50mb.h"
#include "collision_result_binding50mb.h"
#include "type3_query48p.h"
namespace rr::game {
enum class SourceType2QueryPhase50MB : std::uint8_t {Idle,Candidate,Action,Complete,Failed};
struct SourceType2QueryState50MB {
    SourceType2QueryPhase50MB phase=SourceType2QueryPhase50MB::Idle;
    SourceQueryIdentity50MB identity{};
    const std::size_t* retainedIndices=nullptr;
    std::size_t retainedCount=0,ordinal=0,ownerCount=0;
    const Type2StaticCandidate49X* retainedOwners=nullptr;
    std::uint32_t currentCategory=0;
    bool secondary=false;
    SourceTriggerContactState50MB contact{};
};
// All are the actual canonical owners already registered with Bank/shared
// track storage. Pair/record destination scratch is never their replacement.
struct SourceType2QueryOwners50MB {
    Type2StaticQuery49X& query;
    CollisionFilter48J& filter;
    Type2StaticInitial49X& initial;
    SpringRecordState48N& spring;
    SpringRecord48N* records;
    std::size_t recordSlots;
    Type2StaticResult49X& result;
    SpringCumulativePair48N* pairs;
    Type2PairMetadata49X* metadata;
    std::size_t pairSlots;
    Type3Candidate48P* movementMirrors;
    std::size_t mirrorCount;
    CollisionNormalModeOwner50JA& normalMode;
    CollisionResultBindingOwner50MB& resultBinding;
    SourceOuterResultLease50MB& outerResult;
    SourceQueryIdentity50MB identity;
    NativeTriggerGroup50GA& triggers;
    const SourceTriggerActionHooks50MB& actions;
};
enum class SourceType2QueryStatus50MB : std::uint8_t {
    Complete,Input,LivePrefix,Narrowphase,ActionBlocked,Binding
};
struct SourceType2QueryReceipt50MB {
    SourceType2QueryStatus50MB status=SourceType2QueryStatus50MB::Input;
    VehicleLiveQueryResult50EQ live{};
    Type2StaticStatus49X numerical=Type2StaticStatus49X::InvalidInput;
    SourceTriggerContactReceipt50MB contact{};
    std::size_t ordinal=0;
};
// Caller begins/completes/suspends its actual Bank reservation. On every error
// close the outer borrow before Bank restores another arena; source prefix and
// state remain. No automatic runtime retry and no whole-list effects commit.
SourceType2QueryReceipt50MB queryMainLiveSourceIndexedVehicle50MB(
    const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const MainStaticDetectorAdmission50H&,const VehicleLiveQueryContext50EQ&,
    SourceType2QueryOwners50MB&,Type2StaticCandidate49X*,std::size_t,
    const std::size_t*,std::size_t,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,SourceType2QueryState50MB&) noexcept;
SourceType2QueryReceipt50MB querySecondaryLiveSourceIndexedVehicle50MB(
    const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    const MainStaticDetectorAdmission50H&,const VehicleLiveQueryContext50EQ&,
    SourceType2QueryOwners50MB&,Type2StaticCandidate49X*,std::size_t,
    const std::size_t*,std::size_t,const Type2StaticWorkspace49X&,
    const Type2StaticDestination49X&,SourceType2QueryState50MB&) noexcept;
} // namespace rr::game
