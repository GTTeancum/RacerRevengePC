#pragma once
#include "native_racer_bank50hn_fixture.h"
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
    friend class ::ra_cold_roster50LA::Operation; //Actual staged694/A0 cold operand only.
    friend class ::native_racer50HW::Cables;
    friend class ::original_cable_selection50LA::Binding;
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
        std::array<g::OriginalWreckComponent50LA*,15>& storage)noexcept;
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
