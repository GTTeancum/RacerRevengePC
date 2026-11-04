#pragma once
#include "native_racer_bank50hn_fixture.h"
#include "original_racer_terrain_query_owner50la.h"
#include "../../src/game/original_racer_engine_aero_owners50la.h"
#include "original_racer_query690_owner50la.h"
#include "original_racer_query694_owner50la.h"
#include "original_racer_contact354_owner50la.h"
#include "original_racer_detector358_owner50la.h"
#include "original_racer_query7a4_owner50la.h"
#include "original_racer_query79c_owner50la.h"
#include "original_racer_secondary_body_owner50la.h"
#include "original_racer_query364_owner50la.h"
#include "original_racer_contact368_owner50la.h"
#include "original_racer_detector36c_owner50la.h"
#include "original_racer_animation_setup50la_fixture.h"
#include "../../src/game/original_racer_animation_cold50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/original_racer_cold_scenes50la.h"
#include "../../src/game/original_pod_sphere_owner50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include "../../src/game/native_secondary_contact_owner50la.h"
#include "../../src/game/native_racer_terrain_owner50la.h"
#include "../../src/game/native_racer_aero_owner50la.h"
#include "../../src/game/original_racer_gravity_owner50la.h"
#include "../../src/game/original_racer_model_packets50la.h"
#include "../../src/game/original_racer_entity_cold50la.h"
#include "../../src/game/original_motion_library50la.h"
#include "../../src/game/engine_flare50gv.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include <array>
#include <cfenv>
#include <cstring>
#include <cstddef>
#include <new>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace original_camera_session50KX {class Session;}
namespace rr::game {class OriginalRaceConstructor50LA;}
namespace ra_source_camera50LA {class OwnershipBridge;}
// Explicit friend declarations keep this owner header independent of caller include order.
namespace native_racer50HW { class Cables; }
namespace ra_cold_roster50LA { class Operation; }
namespace original_cable_selection50LA { class Binding; }

namespace native_racer_model50LA {
namespace g=rr::game;
namespace a=rr::assets;
namespace setup=original_racer_animation_setup50LA;
using Rig=native_racer50HW::Rig;

// One real native model owner, role-bound to right1A4/1B8, left1A8/1BC,
// optional POD02 bottomRight1AC/1C0 or bottomLeft1B0/1C4, or the separate
// chariot1A0/1B4 branch21191C.
// Its numeric model is the existing Rig
// subtree in the sealed bank, never another graph/vehicle. Cold source matrix
// setup happens while Rig installs that subtree. Canonical material/resource
// completion precedes the shared material scan; racer append happens later
// at the actual Session caller.
// This split is native lifetime staging, not full retail210770 chronology.
// The bounded Entity list projects three or five real owners in source-relative
// order, with concrete190/198/194 between the last engine and chariot.
// The no-engine combined model194 branch remains unadmitted.
class Owner final {
    friend class ::original_camera_session50KX::Session;
    friend class ::ra_source_camera50LA::OwnershipBridge; //Sole native physical continuation caller.
    friend class ::ra_cold_roster50LA::Operation; //Actual staged694/A0 cold operand only.
    friend class ::native_racer50HW::Cables;
    friend class ::original_cable_selection50LA::Binding;
    //Forward native PHY lifetime, owned by the same retained Operation before
    //the first cold bulk consumer; no source1577D0/file handle receipt.
    a::NativePhyLoad50LA* nativePhyLoad_=nullptr;
    a::NativePhyLoad50LA::Context nativePhyContext_{};
    // Native lifetime staging of source cold operands, not shadow live fields.
    // Only these methods create them, on this retained model's construction
    // path before Rig normalization/motion/placement. No caller data/ready bit.
    struct ColdStack0F70 final {
        std::array<std::uint32_t,4> shared434630;
        std::array<std::uint32_t,4> stack0F70;
        const g::OriginalRacerSceneObject50LA* rightNullcg=nullptr;
    };
    struct ColdChariotLocal final {
        std::array<std::uint32_t,3> xyz; //28B180 writes no W lane.
        const g::OriginalRacerSceneObject50LA* nullcg=nullptr;
        const Owner* rightOwner=nullptr;
    };
    std::optional<ColdStack0F70> coldStack0F70_;
    std::optional<ColdChariotLocal> coldChariotLocal_;
    void captureCold434630BeforeModel(const g::SceneGraph49P& canonical);
    void captureColdNullcgAfterModel(const g::SceneGraph49P& canonical);
    static float coldFloat(std::uint32_t word)noexcept {
        float value;std::memcpy(&value,&word,4);return value;
    }
    static float coldLength(const float* xyz){
        need(std::fegetround()==FE_TONEAREST,"original cold length rounding rejected");
        volatile float xx=xyz[0]*xyz[0],yy=xyz[1]*xyz[1];
        volatile float xy=xx+yy,zz=xyz[2]*xyz[2],sum=xy+zz;
        need(std::isfinite(xx)&&std::isfinite(yy)&&std::isfinite(xy)&&
            std::isfinite(zz)&&std::isfinite(sum),"original cold length arithmetic rejected");
        std::uint32_t bits;const float input=sum;std::memcpy(&bits,&input,4);
        if((bits&0x7f800000u)==0)return coldFloat(bits&0x80000000u);
        volatile float result=static_cast<float>(std::sqrt(static_cast<double>(input)));
        need(std::isfinite(result),"original cold SQRT rejected");return result;
    }
    void publishColdTarget2119C4();
    bool attachedFor(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        return modelReappended_&&racer_==&racer&&animation_.fields_.model04==&scene_&&
            (modelRole_==Rig::right?
                racer.animation1A4==&animation_&&racer.rightModel1B8==&scene_:
             modelRole_==Rig::left?
                racer.animation1A8==&animation_&&racer.leftModel1BC==&scene_:
             modelRole_==Rig::bottomRight?
                racer.animation1AC==&animation_&&racer.bottomRightModel1C0==&scene_:
             modelRole_==Rig::bottomLeft?
                racer.animation1B0==&animation_&&racer.bottomLeftModel1C4==&scene_:
                racer.animation1A0==&animation_&&racer.chariotModel1B4==&scene_);
    }
    //213380 is a canonical material-name pass, not aggregate-bounds production.
    // Reached by finish1538B0 immediately after the actual model alias append.
    // Completion covers only this pass, never intervening effects or late strips.
    g::OriginalRacerModelSceneResult50LA prepareMaterials213380()noexcept;
    // Private2286F0..228724 source cut. No runtime caller is installed here.
    // This owner already binds the actual slot/arena/racer and genuine peers;
    // no caller flag, bounds, matrix or replacement control record is accepted.
    g::OriginalRacerModelSceneResult50LA prepareBounds228704()noexcept;
    static bool findBoundsNullcg28C590(const g::OriginalRacerModelSceneOwner50LA& start,
        const g::SceneGraph49P& graph,
        const std::array<const g::OriginalRacerModelSceneOwner50LA*,5>& models,
        const g::OriginalRacerSceneObject50LA*& found,const char* wanted="nullcg")noexcept;
    // Early staging owns these exact allocations until Session publishes them
    // into the actual racer. No scene reference is acquired by28EB90.
    using ColdWristAllocation=g::OriginalRacerWristAllocation50LA;
    struct ColdWrist final {
        enum class Stage {unreached,absent,allocationNull,allocated,constructed};
        Stage stage=Stage::unreached;
        std::unique_ptr<ColdWristAllocation> allocation;
        const g::OriginalRacerSceneObject50LA *shoulder=nullptr,*elbow=nullptr,*wrist=nullptr;
    };
    std::array<ColdWrist,2> coldWrists_;
    void constructColdWrists211A4C(const g::SceneGraph49P& canonical);
    void closeUnpublishedColdWrists()noexcept;
    runtime_startup50fl::Fixture* fixture_=nullptr; //Exact canonical destination lifetime.
    native_racer50HN::Bank* bank_=nullptr;
    g::VehicleFrameState49W* arena_=nullptr;
    std::size_t slot_=0;
    unsigned modelRole_=Rig::right;
    const Rig* rig_=nullptr;
    // Indexed by the existing five Rig roles; the own-role entry stays null.
    // Stable peers borrow the same bank/graph/domain and own no second model.
    std::array<Owner*,5> peers_{};
    inline static constexpr std::array<unsigned,5> sourceRoles_{{
        Rig::right,Rig::left,Rig::bottomRight,Rig::bottomLeft,Rig::chariot}};
    static unsigned sourceRank(unsigned role)noexcept {
        for(unsigned rank=0;rank<sourceRoles_.size();++rank)
            if(sourceRoles_[rank]==role)return rank;
        return static_cast<unsigned>(sourceRoles_.size());
    }
    std::uint64_t generation_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> application_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<g::OriginalMotionLibrary50LA> motionLibrary_;
    std::shared_ptr<a::OriginalStandardTextureResolver50LA> files_;
    a::OriginalStandardModelFileId50LA mcfId_,psgId_;
    std::string* modelDirectory36B860_=nullptr;
    std::string savedModelDirectory_,psgRequest_;
    bool modelDirectorySaved_=false;
    g::OriginalRacerAnimationCold50LA animation_;
    g::OriginalRacerModelSceneOwner50LA scene_;
    g::OriginalRacerColdScenes50LA coldScenes_;
    std::unique_ptr<g::OriginalRacerMainBodyOwner50LA> body340_;
    bool bodyAllocationAttempted_=false;
    // Mutually exclusive native routes on the SAME retained body. Source mode
    // can only start while the genuine167920 mass/inertia are still untouched.
    // Its private entry is intentionally uncalled until fresh query arena and
    // constructor-only Session admission are joined; no ready flag is widened.
    enum class HandlingRoute {unselected,legacy,source};
    HandlingRoute handlingRoute_=HandlingRoute::unselected;
    bool sourceGeometryAttempted_=false,sourceGeometryReserved_=false,sourceGeometryComplete_=false,sourceModelPrefixComplete_=false;
    std::unique_ptr<g::OriginalRacerTerrainQueryOwner50LA> sourceTerrain348_;
    bool sourceTerrainAttempted_=false,sourceTerrainComplete_=false,sourceTerrainWithdrawn_=false;
    std::unique_ptr<g::OriginalRacerEngine34COwner50LA> sourceEngine34C_;
    std::unique_ptr<g::OriginalRacerAero350Owner50LA> sourceAero350_;
    std::unique_ptr<g::OriginalRacerQuery690Owner50LA> sourceQuery690_;
    bool sourceEngineQueryAttempted_=false,sourceEngineQueryComplete_=false,sourceEngineQueryWithdrawn_=false;
    std::string sourceStack840_; //Actual212140 formatted request, retains failed setup input.
    void constructSourceEngineQuery2120B4(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceEngineQuery2120B4(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceEngineQueryWithdrawn_,"source engine/query Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceEngineQuery2120B4(*fixture_,bridge);});
    }
    std::unique_ptr<g::OriginalRacerQuery694Owner50LA> sourceQuery694_;
    bool sourceQuery694Attempted_=false,sourceQuery694Complete_=false,sourceQuery694Withdrawn_=false;
    void constructSourceQuery6942121D0(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceQuery6942121D0(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceQuery694Withdrawn_,"source694 Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceQuery6942121D0(*fixture_,bridge);});
    }
    std::unique_ptr<g::OriginalRacerContact354Owner50LA> sourceContact354_;
    bool sourceContact354Attempted_=false,sourceContact354Complete_=false,sourceContact354Withdrawn_=false;
    void constructSourceContact354212278(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceContact354212278(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceContact354Withdrawn_,"source354 Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceContact354212278(*fixture_,bridge);});
    }
    std::unique_ptr<g::OriginalRacerDetector358Owner50LA> sourceDetector358_;
    bool sourceDetector358Attempted_=false,sourceDetector358Complete_=false,sourceDetector358Withdrawn_=false;
    void constructSourceDetector358212348(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceDetector358212348(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceDetector358Withdrawn_,"source358 Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceDetector358212348(*fixture_,bridge);});
    }
    std::unique_ptr<g::OriginalRacerQuery7A4Owner50LA> sourceQuery7A4_;
    std::unique_ptr<g::OriginalRacerQuery79COwner50LA> sourceQuery79C_;
    bool sourceAuxiliaryAttempted_=false,sourceAuxiliaryComplete_=false,sourceAuxiliaryWithdrawn_=false;
    void constructSourceAuxiliary2123C8(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceAuxiliary2123C8(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceAuxiliaryWithdrawn_,"source auxiliary query Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceAuxiliary2123C8(*fixture_,bridge);});
    }
    std::unique_ptr<g::OriginalRacerSecondaryBodyOwner50LA> sourceBody35C_;
    bool sourceSecondaryAttempted_=false,sourceSecondaryComplete_=false,sourceSecondaryWithdrawn_=false;
    void constructSourceSecondary2124DC(runtime_startup50fl::Fixture& actual);
    void finishSourceSecondary2124DC() {
        need(bank_&&fixture_&&!sourceSecondaryWithdrawn_,"source secondary Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceSecondary2124DC(*fixture_);});
    }
    bool sourceSecondarySetupAttempted_=false,sourceSecondarySetupComplete_=false,sourceSecondarySetupWithdrawn_=false;
    std::optional<float> sourceSecondaryF20_,sourceSecondaryHalfMass_;
    std::optional<g::BodyMatrix48G> sourceSecondaryInertia0F90_;
    void constructSourceSecondarySetup2125C4(runtime_startup50fl::Fixture& actual);
    void finishSourceSecondarySetup2125C4() {
        need(bank_&&fixture_&&!sourceSecondarySetupWithdrawn_,"source secondary setup Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceSecondarySetup2125C4(*fixture_);});
    }
    std::unique_ptr<g::OriginalRacerQuery364Owner50LA> sourceQuery364_;
    std::unique_ptr<g::OriginalRacerContact368Owner50LA> sourceContact368_;
    std::unique_ptr<g::OriginalRacerDetector36COwner50LA> sourceDetector36C_;
    bool sourceSecondaryContactsAttempted_=false,sourceSecondaryContactsComplete_=false,sourceSecondaryContactsWithdrawn_=false;
    void constructSourceSecondaryContacts212720(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceSecondaryContacts212720(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceSecondaryContactsWithdrawn_,"source secondary contacts Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceSecondaryContacts212720(*fixture_,bridge);});
    }
    bool sourceWreckConfigAttempted_=false,sourceWreckConfigComplete_=false;
    void finishSourceWreckConfig2129A4(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckPieceAttempted_=false,sourceWreckPieceComplete_=false;
    void finishSourceWreckPiece212A50(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckBodyAttempted_=false,sourceWreckBodyComplete_=false;
    void finishSourceWreckBody165E90(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE0Attempted_=false,sourceWreckE0Complete_=false;
    void finishSourceWreckE0166018(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE0LoadAttempted_=false,sourceWreckE0LoadComplete_=false;
    void finishSourceWreckE0Shape166074(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE0PrepareAttempted_=false,sourceWreckE0PrepareComplete_=false;
    void finishSourceWreckE0Prepare16607C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckDCAttempted_=false,sourceWreckDCComplete_=false;
    void finishSourceWreckDC1660A0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckD8Attempted_=false,sourceWreckD8Complete_=false;
    void finishSourceWreckD816616C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE4Attempted_=false,sourceWreckE4Complete_=false;
    void finishSourceWreckE41661D8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE4PrepareAttempted_=false,sourceWreckE4PrepareComplete_=false;
    void finishSourceWreckE4Prepare16629C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckE8Attempted_=false,sourceWreckE8Complete_=false;
    void finishSourceWreckE81662E4(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckMutualAttempted_=false,sourceWreckMutualComplete_=false;
    bool sourceWreckArraysAttempted_=false,sourceWreckArraysComplete_=false;
    bool sourceWreckSplashAttempted_=false,sourceWreckSplashComplete_=false;
    void finishSourceWreckSplash2CB854(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckDustAttempted_=false,sourceWreckDustComplete_=false;
    void finishSourceWreckDust2CB7E0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSmokeAttempted_=false,sourceWreckSmokeComplete_=false;
    bool sourceWreckRemainingPairsAttempted_=false,sourceWreckRemainingPairsComplete_=false;
    void finishSourceWreckRemainingPairs2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPositiveSplashReaderAttempted_=false,sourceWreckSecondPositiveSplashReaderComplete_=false;
    void finishSourceWreckSecondPositiveSplashRead19DDC8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPositiveSplashAttempted_=false,sourceWreckSecondPositiveSplashComplete_=false;
    void constructSourceWreckSecondPositiveSplash(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPositiveDustReaderAttempted_=false,sourceWreckSecondPositiveDustReaderComplete_=false;
    void finishSourceWreckSecondPositiveDustRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPositiveDustAttempted_=false,sourceWreckSecondPositiveDustComplete_=false;
    void constructSourceWreckSecondPositiveDust(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondRemainingPairsAttempted_=false,sourceWreckSecondRemainingPairsComplete_=false;
    void finishSourceWreckSecondRemainingPairs2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondFirstPairAttempted_=false,sourceWreckSecondFirstPairComplete_=false;
    void finishSourceWreckSecondFirstPair2CB778(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckFirstPairAttempted_=false,sourceWreckFirstPairComplete_=false;
    void finishSourceWreckFirstPair2CB778(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondInitialCallerAttempted_=false,sourceWreckSecondInitialCallerComplete_=false;
    void completeSourceWreckSecondInitialChild19A7F8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondInitialReaderAttempted_=false,sourceWreckSecondInitialReaderComplete_=false;
    void readSourceWreckSecondInitialChild19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondSuccessorCallerAttempted_=false,sourceWreckSecondSuccessorCallerComplete_=false;
    void completeSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondSuccessorReaderAttempted_=false,sourceWreckSecondSuccessorReaderComplete_=false;
    void readSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondSuccessorAttempted_=false,sourceWreckSecondSuccessorComplete_=false;
    void constructSourceWreckSecondSuccessorChild(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondInitialChildAttempted_=false,sourceWreckSecondInitialChildComplete_=false;
    void constructSourceWreckSecondInitialChild(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondArraysAttempted_=false,sourceWreckSecondArraysComplete_=false;
    void finishSourceWreckSecondArrays2CB628(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondEffectInputsAttempted_=false,sourceWreckSecondEffectInputsComplete_=false;
    void bindSourceWreckSecondEffectInputs2CB618(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondMutualAttempted_=false,sourceWreckSecondMutualComplete_=false;
    void finishSourceWreckSecondMutual166390(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondE8Attempted_=false,sourceWreckSecondE8Complete_=false;
    void finishSourceWreckSecondE81662E4(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondE4Attempted_=false,sourceWreckSecondE4Complete_=false;
    bool sourceWreckSecondE4PrepareAttempted_=false,sourceWreckSecondE4PrepareComplete_=false;
    void finishSourceWreckSecondE41661D8(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckSecondE4Prepare16629C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondDCAttempted_=false,sourceWreckSecondDCComplete_=false;
    bool sourceWreckSecondD8Attempted_=false,sourceWreckSecondD8Complete_=false;
    void finishSourceWreckSecondDC1660A0(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckSecondD816616C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondE0LoadAttempted_=false,sourceWreckSecondE0LoadComplete_=false;
    bool sourceWreckSecondE0PrepareAttempted_=false,sourceWreckSecondE0PrepareComplete_=false;
    void finishSourceWreckSecondE0Shape166074(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckSecondE0Prepare16607C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondE0Attempted_=false,sourceWreckSecondE0Complete_=false;
    void finishSourceWreckSecondE0166018(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondBodyAttempted_=false,sourceWreckSecondBodyComplete_=false;
    void finishSourceWreckSecondBody165E90(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPieceAttempted_=false,sourceWreckSecondPieceComplete_=false;
    void finishSourceWreckSecondPiece22C0A0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckPieceCursorAttempted_=false,sourceWreckPieceCursorComplete_=false;
    void finishSourceWreckPieceCursor22C25C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckWaterAttempted_=false,sourceWreckWaterComplete_=false;
    void finishSourceWreckWater22C21C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckGroupPrimaryAttempted_=false,sourceWreckGroupPrimaryComplete_=false;
    bool sourceWreckGroupContextAttempted_=false,sourceWreckGroupContextComplete_=false;
    bool sourceWreckGroupSparksAttempted_=false,sourceWreckGroupSparksComplete_=false;
    bool sourceWreckGroupSparksSetupAttempted_=false,sourceWreckGroupSparksSetupComplete_=false;
    bool sourceWreckGroupSparksConfigAttempted_=false,sourceWreckGroupSparksConfigComplete_=false;
    void beginSourceWreckGroupSparksConfig15F2F0(::ra_source_camera50LA::OwnershipBridge& bridge);
    void setupSourceWreckGroupSparks2CC0D4(::ra_source_camera50LA::OwnershipBridge& bridge);
    void constructSourceWreckGroupSparks2CC0B4(::ra_source_camera50LA::OwnershipBridge& bridge);
    void consumeSourceWreckGroupContext2CC088(::ra_source_camera50LA::OwnershipBridge& bridge);
    void bindSourceWreckGroupPrimary22C298(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckGroupConstructorAttempted_=false,sourceWreckGroupConstructorComplete_=false;
    void constructSourceWreckGroup22C278(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPieceCursorAttempted_=false,sourceWreckSecondPieceCursorComplete_=false;
    void finishSourceWreckSecondPieceCursor22C25C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondWaterAttempted_=false,sourceWreckSecondWaterComplete_=false;
    void finishSourceWreckSecondWater22C21C(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSecondPieceAppendAttempted_=false,sourceWreckSecondPieceAppendComplete_=false;
    void finishSourceWreckSecondPieceAppend22C1B8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckPieceAppendAttempted_=false,sourceWreckPieceAppendComplete_=false;
    void finishSourceWreckPieceAppend22C1B8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSplashReadAttempted_=false,sourceWreckSplashReadComplete_=false;
    void finishSourceWreckSplashRead19DDC8(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckDustReadAttempted_=false,sourceWreckDustReadComplete_=false;
    void finishSourceWreckDustRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckFireReadAttempted_=false,sourceWreckFireReadComplete_=false;
    void finishSourceWreckFireRead15E024(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckFireAttempted_=false,sourceWreckFireComplete_=false;
    void finishSourceWreckFire2CB720(::ra_source_camera50LA::OwnershipBridge& bridge);
    bool sourceWreckSmokeReadAttempted_=false,sourceWreckSmokeReadComplete_=false;
    void finishSourceWreckSmokeRead19A7D0(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckSmoke2CB6CC(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckArrays2CB628(::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceWreckMutual166390(::ra_source_camera50LA::OwnershipBridge& bridge);
    ::ra_source_camera50LA::OwnershipBridge* sourceTerrainBridge_=nullptr;
    std::uint32_t sourceConstructorPC_=0;
    void constructSourceTerrain21204C(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    void finishSourceTerrain21204C(::ra_source_camera50LA::OwnershipBridge& bridge) {
        need(bank_&&fixture_&&!sourceTerrainWithdrawn_,"source348 live Bank custody missing");
        bank_->visit(slot_,[&](){constructSourceTerrain21204C(*fixture_,bridge);});
    }

    // Separate native full-publication continuation. Never selects or relaxes
    // HandlingRoute::source, and never treats the old49N projection as a call receipt.
    bool continueNativePhysical214710(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,std::uint32_t& pc)noexcept;
    bool nativePhysicalAttempted_=false,nativePhysicalComplete_=false;
    bool continueNativeRepulsor214894(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,::ra_source_camera50LA::OwnershipBridge& bridge,
        std::uint32_t& pc)noexcept;
    bool nativeRepulsorAttempted_=false,nativeRepulsorComplete_=false;
    bool continueNativeAeroEngine214954(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,::original_camera_session50KX::Session&,
        const g::OriginalRaceConstructor50LA&,::ra_source_camera50LA::OwnershipBridge&,std::uint32_t& pc)noexcept;
    bool nativeAeroEngineAttempted_=false,nativeAeroEngineComplete_=false;
    bool continueNativeAnimationContact2154D8(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,::original_camera_session50KX::Session&,
        ::ra_source_camera50LA::OwnershipBridge&,std::uint32_t& pc)noexcept;
    bool continueNativeHandling215BB8(g::OriginalRacerEntityCold50LA& actualRacer,
        const std::vector<std::uint8_t>& retainedPhy,::original_camera_session50KX::Session&,
        const g::OriginalRaceConstructor50LA&,::ra_source_camera50LA::OwnershipBridge&,std::uint32_t& pc)noexcept;
    bool nativeAnimationContactAttempted_=false,nativeAnimationContactComplete_=false;
    bool nativeWreckTuningComplete_=false;
    bool nativeHandlingTailAttempted_=false,nativeHandlingReturned_=false;
    //Bridge calls only after SAME retained Cables returned212E10, while the
    //actual slot0 Bank visit remains active. No visiting/public wrapper exists.
    bool continueNativeSpin212ED0(g::OriginalRacerEntityCold50LA& actualRacer,
        std::uint32_t& pc)noexcept;
    bool nativeSpinAttempted_=false,nativeSpinReturned_=false;
    bool nativeSpinAllocationAttempted_=false;
    std::optional<std::uint32_t> nativeSpinCursor2C_; //Actual212F54, not B54.
    std::uint32_t nativeSpinPC_=0x212e10u;
    // One retained2146 stack, not detached parsed authoring or live aliases.
    // Empty optionals are genuinely unreached/unknown. Initialization happens
    // only at the corresponding source instruction in the continuation.
    struct NativeHandlingStack final {
        std::optional<unsigned> selectedClass;
        std::optional<bool> useValues;
        std::optional<float> upgrade3F0;
        std::optional<std::int32_t> tuning3F4;
        std::array<std::optional<float>,28> aero100;
        std::optional<float> rampDownElevator3F8,maxElevator3FC;
        std::array<std::optional<float>,5> steering400;
        std::array<std::optional<float>,10> engine414;
        std::array<std::optional<float>,8> abilities43C;
        std::array<std::optional<std::array<float,4>>,2> engineVectors170;
        std::array<std::optional<float>,8> contact45C;
        std::optional<float> gravity47C;
        std::array<std::optional<float>,12> damage480;
        std::array<std::optional<float>,13> profile4C;
        std::optional<std::string_view> binderColor2B0;
        std::array<std::optional<std::int32_t>,3> binderChannels;
    };
    std::optional<NativeHandlingStack> nativeHandlingStack_;
    // Full2E7680 vector destination W words absent from existing XYZ-only48D
    // parameters. Per-model native backing, never counted source34C fields.
    std::array<std::optional<std::uint32_t>,2> nativeEngineBasisW_;
    std::array<std::optional<float>,4> nativeRepulsorStack_;
    std::array<std::optional<float>,5> nativePhysicalStack_;
    g::BodyMatrix48G nativePhysicalMatrix_{};
    bool sourceMassAttempted_=false,sourceMassComplete_=false,sourceInertiaAttempted_=false;
    bool sourcePhysicalAttempted_=false,sourceRepulsorAttempted_=false;
    std::array<std::optional<float>,4> sourceRepulsorStack_;
    void applySourceRepulsor214894(runtime_startup50fl::Fixture& actual,
        ::ra_source_camera50LA::OwnershipBridge& bridge);
    //Actual2146 stack destinations, unknown until their individual158A70
    //writes. Missing Weight/scales stop at consumption, not at the reader.
    std::array<std::optional<float>,5> sourcePhysicalStack_;
    g::BodyMatrix48G sourcePhysicalMatrix_{};
    void applySourcePhysical214710(runtime_startup50fl::Fixture& actual);
    std::vector<std::uint8_t> sourceHandlingPhy_; //Immutable authored backing, not live parameters.
    void reserveSourceHandlingBefore2146(runtime_startup50fl::Fixture& actual);
    bool sourceHandlingBodyCurrent(const runtime_startup50fl::Fixture& actual)const noexcept;
    void applySourceMass165300(runtime_startup50fl::Fixture& actual,float mass);
    void applySourceInertia165340(runtime_startup50fl::Fixture& actual,const g::BodyMatrix48G& input);

    bool nativeSecondaryContactsRequested_=false,nativeSecondaryContactsAttempted_=false;
    std::shared_ptr<g::NativeSecondaryContactOwner50LA> nativeSecondaryContacts_;
    std::shared_ptr<g::NativeRacerTerrainOwner50LA> nativeRacerTerrain_;
    std::shared_ptr<g::NativeRacerAeroOwner50LA> nativeAero_;
    std::unique_ptr<g::OriginalRacerGravityOwner50LA> gravity344_;
    bool gravityAllocationAttempted_=false;
    std::unique_ptr<g::OriginalPodSphereOwner50LA> joint360_;
    bool jointAllocationAttempted_=false;
    std::unique_ptr<setup::Setup> setup_;
    std::unique_ptr<setup::Input> input_;
    std::unique_ptr<setup::Archive> archive_;
    std::unique_ptr<setup::Ini> configuration08_;
    g::OriginalModelMotionSet50LA motions_;
    std::shared_ptr<a::OriginalStandardModelResource50LA> payload_;
    std::shared_ptr<a::OriginalStandardModelGeometry50LA> geometry_;
    a::OriginalStandardMaterialReferences50LA materials_;
    g::OriginalRacerModelPackets50LA packets_;
    g::OriginalRacerEntityCold50LA* racer_=nullptr;
    a::PodFlameOwners50W* bankedFlame_=nullptr;
    bool cachePublished_=false,graphStarted_=false,modelAppended_=false;
    bool resourcesComplete_=false,animationAttached_=false,modelReappended_=false,finished_=false,closed_=false;
    std::uint32_t failurePC_=0;
    static void need(bool value,const char* message){if(!value)throw std::runtime_error(message);}
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    g::OriginalWreckComponent50LA& animationEntity()noexcept{return animation_.entity220_;}
    std::array<const g::OriginalRacerModelSceneOwner50LA*,4> peerScenes()const noexcept {
        std::array<const g::OriginalRacerModelSceneOwner50LA*,4> out{};
        std::size_t count=0;
        for(const auto role:sourceRoles_){const auto* peer=peers_[role];
            if(peer&&peer->scene_.constructed2403A0())out[count++]=&peer->scene_;}
        return out;
    }
    void restoreModelDirectory1543A8()noexcept {
        if(modelDirectorySaved_){modelDirectory36B860_->swap(savedModelDirectory_);modelDirectorySaved_=false;}
    }
    g::SceneGraph49P graph()const noexcept{return {arena_->nodes,rig_->nodeCount};}
    g::OriginalWreckComponentDomain50LA outside(
        std::array<g::OriginalWreckComponent50LA*,29>& storage)noexcept;
    bool admitAnimationClose()noexcept;
    bool closeOnCurrentGraph()noexcept;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    void prepareEarly(const Rig& rig,std::size_t slot,std::uint64_t generation,
        native_racer50HN::Bank& bank,runtime_startup50fl::Fixture& live,
        std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<g::OriginalMotionLibrary50LA> motionLibrary,
        std::string& modelDirectory36B860,unsigned modelRole=Rig::right);
private:
    void admitPeer(const Owner& other)const;
    bool graphOrderAdmitted()const noexcept;
    bool attachmentOrderAdmitted(const g::OriginalRacerEntityCold50LA& racer)const noexcept;
public:
    // Preserve the original two-engine admission. Chariot joins separately
    // only after both exact engine peers have formed this stable pair.
    void bindSibling(Owner& other);
    void bindChariotSiblings(Owner& right,Owner& left,
        Owner* bottomRight=nullptr,Owner* bottomLeft=nullptr);
    // Called only by the actual Rig::install before its generic model stores.
    // Other Rig models retain their existing cold implementation.
    void constructGraph(const g::SceneGraph49P& canonical);
    void prepareAnimation(const g::SceneGraph49P& canonical);
    // Immutable numeric adapter only: allocation/publication remains above.
    // Solver targets are resolved separately and never gate source allocation.
    void bindColdWrists(g::ChariotIkBindings49T& bindings,g::ChariotIkPair49T& records,
        const g::ChariotIkPair49T*& borrowed)const;
    // Actual Rig pre-motion normalization, after retained cold capture. Uses
    //28AB90 so a world cache read for stack0F70 cannot survive the local write.
    void normalizeColdEngineBeforeMotion(const g::SceneGraph49P& canonical);
    // Called by this SAME Rig after its canonical scene install, before any
    // PHY handling working copy. Represented340 and344 constructor writes
    // are admitted; probe348/collision owners remain separate cuts.
    void constructColdMain211F6C(const g::SceneGraph49P& canonical);
    // Join the existing successful numeric PHY transaction's actual165340
    //100 result to this SAME body owner. The no-fail live state/parameter copy
    //immediately follows; a failed handling transaction never reaches here.
    //This does not claim the intervening2146C0 Entity consumers are present.
    void beginLegacyColdHandling(runtime_startup50fl::Fixture& actual);
    void commitMainInertia21488C(runtime_startup50fl::Fixture& actual,
        const g::AuthoredBinding49N& binding);
    // Handling uses a native working copy, but this exact Fixture and its
    // canonical target/854/858 remain the retained source owners. No new visit.
    void preserveColdTargetForHandling(runtime_startup50fl::Fixture& actual,
        g::AuthoredGeometryBinding49O& projection)const;
    // Shared-mode native contact custody only. Ordinary retained owners do
    // nothing here; no secondary Entity/19C scene alias is manufactured.
    void constructNativeSecondaryContacts212538(runtime_startup50fl::Fixture& actual,
        g::VehicleFrameState49W& handlingState);
    void setupColdJoint2126(runtime_startup50fl::Fixture& actual,
        const g::AuthoredGeometryBinding49O& geometry,const g::PodSphereBody49L& main,
        g::PodSphereJoint49L& joint);
    const setup::Setup& animationSetup()const {need(bool(setup_),"original model setup missing");return *setup_;}
    const g::OriginalRacerAnimationCold50LA& animation()const noexcept{return animation_;}
    const g::OriginalRacerAnimationCold50LA& animation1A4()const {
        need(modelRole_==Rig::right,"original animation1A4 requires the actual right role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1A8()const {
        need(modelRole_==Rig::left,"original animation1A8 requires the actual left role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1AC()const {
        need(modelRole_==Rig::bottomRight,"original animation1AC requires the actual bottom-right role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1B0()const {
        need(modelRole_==Rig::bottomLeft,"original animation1B0 requires the actual bottom-left role");return animation_;
    }
    const g::OriginalRacerAnimationCold50LA& animation1A0()const {
        need(modelRole_==Rig::chariot,"original animation1A0 requires the actual chariot role");return animation_;
    }
    unsigned modelRole()const noexcept{return modelRole_;}
    const Rig* actualRig()const noexcept{return rig_;}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
    std::uint32_t failurePC()const noexcept{return failurePC_;}
    bool finished1538B0()const noexcept{return finished_&&!closed_;}
    bool resourcesComplete1538B0()const noexcept{return resourcesComplete_&&!closed_;}
    const g::OriginalRacerModelSceneOwner50LA* modelScene()const noexcept {
        return resourcesComplete1538B0()?&scene_:nullptr;
    }
    g::OriginalRacerModelPackets50LA* modelPackets()noexcept {
        return resourcesComplete1538B0()?&packets_:nullptr;
    }
    const a::OriginalStandardModelGeometry50LA* modelGeometry()const noexcept {
        return resourcesComplete1538B0()?geometry_.get():nullptr;
    }
    const a::OriginalStandardMaterialReferences50LA* materialReferences()const noexcept {
        return resourcesComplete1538B0()?&materials_:nullptr;
    }
    const a::OriginalStandardMaterialDomain50LA* materialDomain()const noexcept{return domain_.get();}
    // Caller is visiting this exact installed racer slot. The stable arena
    // address is retained only so ordered close can clear its banked borrow.
    void bindRendererMaterialFlame(a::PodFlameOwners50W& actualFlame);
    //Dedicated constructor-only Session read; never broadens finishedFor or
    //the legacy runtime admission. This exact cut ends BEFORE348 allocation.
    bool sourceModelPrefixFor(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        return !closed_&&handlingRoute_==HandlingRoute::source&&sourceModelPrefixComplete_&&
            attachedFor(racer)&&racer.body340==body340_.get()&&racer.gravity344==gravity344_.get()&&
            !racer.joint360;
    }
    bool finishedFor(const g::OriginalRacerEntityCold50LA& racer)const noexcept {
        return finished1538B0()&&attachedFor(racer);
    }
    bool close()noexcept {
        if(closed_)return true;
        if(!bank_||!graphStarted_)return closeOnCurrentGraph();
        try{return bank_->visit(slot_,[&](){return closeOnCurrentGraph();});}
        catch(...){return false;}
    }
    // Complete resource/material ownership before the actual shared14D370
    // scan and GPU publication. Numeric graph installation is never replayed.
    void completeResources1538B0(const std::string& directory);
    // Actual Session caller retains the original Entity append/alias order.
    // Resource completion happened once before the shared population scan.
    void finish1538B0(g::OriginalRacerEntityCold50LA& racer,const std::string& directory);
};
} //namespace native_racer_model50LA
