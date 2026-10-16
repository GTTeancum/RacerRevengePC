#pragma once
// Cold authored handling for an original roster member. This composes the
// existing native PHY readers and numeric binders; it is not a second physics
// model or an inferred racer-class selector. The caller supplies this racer's
// authenticated PHY, PSG views, fresh geometry result and decoded podboxf.
//
// Install this racer's graph/IK first. Call only before its first native tick,
// then place its physical/displayed roots using secondaryInputs.targetRelative2F0
// and refresh/rebase its own model bounds. Track tuning and the development
// owner's unrepresented constructor inputs remain explicit previous inputs.
// No source-derived coefficient is borrowed from another racer.
#include "runtime_startup50fl_fixture.h"
#include "boost_controls50fk_fixture.h"
#include "../../src/assets/repair_authoring50a.h"
#include "../../src/assets/spring_authoring50f.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace native_racer50HW {

// These are references to the stable serial execution arena. Register each
// referred-to value in the racer bank; do not register this transient wrapper.
struct HandlingTuning {
    rr::game::BoostParameters50FK& boost;
    float& coolingCF8;
    rr::game::SpringGeneratedParameters48K& mainSpring;
    rr::game::SpringResponseParameters48L& mainResponse;
    rr::game::SpringGeneratedParameters48K& secondarySpring;
    rr::game::SpringResponseParameters48L& secondaryResponse;
};

struct HandlingReceipt {
    rr::game::AuthoredBinding49N authored{};
    float defenseCF0=0.f,repairCF4=0.f,coolingCF8=0.f;
    float authoredWeight=0.f,liftHeight=0.f;
};

namespace handling_detail {
inline float selectRepair(const rr::assets::RepairAuthoring50A& repair,
    const rr::game::AuthoredParameters49N& raw,
    const rr::game::AuthoredBinding49N& authored) {
    using rr::assets::RepairAuthoringKey50A;
    using rr::game::AuthoredClass49N;
    const auto value=[&](RepairAuthoringKey50A key){
        const auto& field=repair.values[static_cast<std::size_t>(key)];
        runtime_startup50fl::require(field.present&&std::isfinite(field.value),
            "racer selected repair authoring missing or nonfinite");
        return field.value;
    };
    float amount=0.f;
    if(authored.usedAuthoredClassValues){
        switch(authored.selectedClass){
            case AuthoredClass49N::A:amount=value(RepairAuthoringKey50A::RepairA);break;
            case AuthoredClass49N::B:amount=value(RepairAuthoringKey50A::RepairB);break;
            case AuthoredClass49N::C:amount=value(RepairAuthoringKey50A::RepairC);break;
            default:runtime_startup50fl::require(false,"racer repair class unavailable");
        }
    }else{
        // Same source ability interpolation as49N/50FK. Do not retain the
        // Pod01-only equal-unit endpoint admission or replace UpgradePercent.
        const float minimum=value(RepairAuthoringKey50A::MinRepair);
        const float maximum=value(RepairAuthoringKey50A::MaxRepair);
        const float base=value(RepairAuthoringKey50A::BaseAbilityRepair);
        const float maxAbility=value(RepairAuthoringKey50A::MaxAbilityRepair);
        const float upgrade=raw.tuning.upgradePercent.present?raw.tuning.upgradePercent.value:0.f;
        volatile float difference=maxAbility-base;
        volatile float delta=upgrade*difference;
        volatile float ability=base+delta;
        volatile float range=maximum-minimum;
        volatile float scaled=ability*range;
        volatile float selected=minimum+scaled;
        runtime_startup50fl::require(std::isfinite(upgrade)&&std::isfinite(difference)&&
            std::isfinite(delta)&&std::isfinite(ability)&&std::isfinite(range)&&
            std::isfinite(scaled)&&std::isfinite(selected),"racer repair arithmetic rejected");
        amount=selected;
    }
    runtime_startup50fl::require(std::isfinite(amount)&&amount>0.f,
        "racer authored repair amount unavailable");
    return amount;
}
}

// Generic cold bind stages readers/projections in local records. With a
// retained constructor, source target/854 were already published; later858
// and joint prefix mutations commit to their actual owners and survive failure.
// Remaining handling records/tuning keep their existing atomic publication.
// The selection context is passed through unchanged, including Tuning overrides.
// The existing client policy requires no profile overwrite and takes CF0 from
// this racer's BaseAbilities.Defense. It does not silently adopt DefenseA/B/C.
// Geometry owns secondary mass/inertia/target/joint; ChariotWeight is not a
// substitute for the recovered secondary constructor and fresh model bounds.
template<class ColdOwner=std::nullptr_t>
inline HandlingReceipt bindColdHandling(
    const std::vector<std::uint8_t>& phy,
    const rr::assets::PsgHierarchyView49O& right,
    const rr::assets::PsgHierarchyView49O& left,
    const rr::game::FreshPodGeometryResult49O& geometry,
    const rr::game::PodShapeSource49W& mainShape,
    const rr::game::AuthoredSelectionContext49N& selection,
    runtime_startup50fl::Fixture& live,const HandlingTuning& tuning,
    const rr::assets::PsgHierarchyView49O* bottomRight=nullptr,
    const rr::assets::PsgHierarchyView49O* bottomLeft=nullptr,ColdOwner* retained=nullptr) {
    using namespace rr::game;
    namespace assets=rr::assets;
    using runtime_startup50fl::require;
    require(!phy.empty()&&!selection.applyProfileOverwrite&&
        validVehicleSceneCount49W(live.state,live.bindings)&&
        live.state.phase==VehicleFramePhase49W::Ready&&
        live.state.bodies.main.frameFactor==0.f,
        "racer handling requires fresh state and explicit no-profile selection");
    const auto* text=reinterpret_cast<const char*>(phy.data());
    const auto bytes=phy.size();
    auto parameters=live.parameters;
    auto state=live.state;
    auto selectors=live.selectors;
    auto secondary=live.secondaryInputs;
    auto mainSpring=tuning.mainSpring;
    auto mainResponse=tuning.mainResponse;
    auto secondarySpring=tuning.secondarySpring;
    auto secondaryResponse=tuning.secondaryResponse;
    BoostParameters50FK boost{};
    HandlingReceipt receipt{};

    AuthoredParameters49N raw{};
    assets::PodPhysicsDiagnostic49N physicsDiagnostic{};
    require(assets::readPodPhysics49N(text,bytes,raw,physicsDiagnostic),
        "racer original PHY admission failed");
    receipt.authoredWeight=raw.values[static_cast<std::size_t>(AuthoredKey49N::Weight)].value;
    require(std::isfinite(receipt.authoredWeight)&&receipt.authoredWeight>0.f,
        "racer PHY must author its own positive body mass");

    assets::VisualAuthoredValues49R visual{};
    assets::VisualAuthoringDiagnostic49R visualDiagnostic{};
    require(assets::readVehicleVisualAuthoring49R(text,bytes,visual,visualDiagnostic)&&
        assets::bindFreshVehicleVisualParameters49R(visual,right,left,
            parameters.visual,visualDiagnostic),"racer visual PHY/geometry binding failed");
    require(bool(bottomRight)==bool(bottomLeft)&&
        bool(bottomRight)==live.bindings.visual.extra1C0Present&&
        bool(bottomLeft)==live.bindings.visual.extra1C4Present,
        "racer bottom-engine authored/bound presence differs");
    if(bottomRight)require(assets::bindFreshBottomVehicleVisualParameters49R(
        *bottomRight,*bottomLeft,parameters.visual,visualDiagnostic),
        "racer bottom-engine own PSG base capture failed");
    receipt.liftHeight=parameters.visual.verticalLimit7EC;
    assets::ChariotRootAuthoredValues49U root{};
    assets::ChariotRootAuthoringDiagnostic49U rootDiagnostic{};
    require(assets::readChariotRootAuthoring49U(text,bytes,root,rootDiagnostic)&&
        assets::bindFreshChariotRootParameters49U(root,geometry,parameters.visual,
            parameters.lifecycle,rootDiagnostic),"racer chariot PHY/geometry binding failed");

    if constexpr(!std::is_same_v<ColdOwner,std::nullptr_t>){
        if(retained){
            auto admission=geometry.parameters;
            retained->preserveColdTargetForHandling(live,admission);
            parameters.lifecycle.restLength854=live.parameters.lifecycle.restLength854;
        }
    }
    parameters.shape=mainShape;
    receipt.authored=runtime_startup50fl::bind(phy,geometry,selection,parameters,
        state,selectors,secondary,retained,&live);
    require(receipt.authored.massSetterWrote,"racer main mass setter was skipped");
    assets::initializeVehicleVisualConstructorRecords49R(state.visual);
    state.visual.right.position=parameters.visual.rightBase2C0;
    state.visual.left.position=parameters.visual.leftBase2B0;
    if(bottomRight){
        state.visual.bottomRight.position=parameters.visual.rightBase2E0;
        state.visual.bottomLeft.position=parameters.visual.leftBase2D0;
    }

    assets::SpringCollisionAuthoring50F collision{};
    require(assets::readSpringCollision50F(text,bytes,collision,physicsDiagnostic)&&
        assets::applySpringCollision50F(collision,parameters.bodies.main.body.mass,
            mainSpring,mainResponse)&&
        assets::applySpringCollision50F(collision,parameters.bodies.secondary.body.mass,
            secondarySpring,secondaryResponse),"racer authored spring binding failed");
    assets::DefenseAuthoring49Y defense{};
    require(assets::readDefenseAuthoring49Y(text,bytes,defense,physicsDiagnostic)&&
        defense.baseDefense.present&&std::isfinite(defense.baseDefense.value),
        "racer explicit base-profile defense unavailable");
    receipt.defenseCF0=defense.baseDefense.value;
    assets::RepairAuthoring50A repair{};
    require(assets::readRepairAuthoring50A(text,bytes,repair,physicsDiagnostic),
        "racer repair PHY admission failed");
    receipt.repairCF4=handling_detail::selectRepair(repair,raw,receipt.authored);
    require(boost50fk_fixture::bind(text,bytes,selection,receipt.authored,
        boost,receipt.coolingCF8),"racer boost/cooling PHY binding failed");
    state.fields.defenseCF0=receipt.defenseCF0;
    state.fields.repairCF4=receipt.repairCF4;

    live.parameters=parameters;live.state=state;
    live.selectors=selectors;live.secondaryInputs=secondary;
    tuning.mainSpring=mainSpring;tuning.mainResponse=mainResponse;
    tuning.secondarySpring=secondarySpring;tuning.secondaryResponse=secondaryResponse;
    tuning.boost=boost;tuning.coolingCF8=receipt.coolingCF8;
    return receipt;
}

// Cold companion to bindColdHandling. The inherited main pose is the supplied
// grid anchor; the secondary offset comes only from this racer's49O geometry.
// Does not rebase graphics or attach query memberships. Call it before those
// operations and before the host's lateral/longitudinal grid displacement.
inline void placeColdHandlingRoots(runtime_startup50fl::Fixture& live) {
    using namespace rr::game;
    using runtime_startup50fl::require;
    require(validVehicleSceneCount49W(live.state,live.bindings)&&
        live.state.phase==VehicleFramePhase49W::Ready&&
        live.state.bodies.main.frameFactor==0.f,"racer root placement requires fresh state");
    const auto main=static_cast<std::size_t>(live.bindings.lifecycle.main190);
    require(main<live.state.sceneNodeCount,"racer root placement main binding unavailable");
    auto state=live.state;
    auto secondaryPose=state.nodes[main].world100;
    const auto& mainPose=state.nodes[main].world100;
    const auto& target=live.secondaryInputs.targetRelative2F0;
    for(unsigned axis=0;axis<3;++axis){
        volatile float a=mainPose.v[axis]*target.v[0];
        volatile float b=mainPose.v[4+axis]*target.v[1];
        volatile float c=mainPose.v[8+axis]*target.v[2];
        volatile float ab=a+b;
        volatile float abc=ab+c;
        volatile float position=mainPose.v[12+axis]+abc;
        require(std::isfinite(position),"racer secondary root arithmetic rejected");
        secondaryPose.v[12+axis]=position;
    }
    const SceneGraph49P graph{state.nodes,state.sceneNodeCount};
    require(setLocalMatrix49P(graph,live.bindings.lifecycle.physical19C,secondaryPose)&&
        refreshSceneWorld49P(graph,live.bindings.lifecycle.physical19C)&&
        setLocalMatrix49P(graph,live.bindings.lifecycle.model1B4,secondaryPose),
        "racer cold physical/displayed root placement rejected");
    // Original217078..217238 places each present engine from its own base
    // through main190. The optional topology must be correctly placed before
    // source model rebase and host grid translation, not deferred to graphics.
    // Keep the historical three-model host cold policy unchanged.
    if(live.bindings.visual.extra1C0Present){
        require(live.bindings.visual.extra1C4Present,"racer partial bottom-engine cold placement");
        const SceneNodeIndex49P roots[]={live.bindings.visual.right1B8,live.bindings.visual.left1BC,
            live.bindings.visual.bottomRight1C0,live.bindings.visual.bottomLeft1C4};
        const VisualVector49R* bases[]={&live.parameters.visual.rightBase2C0,&live.parameters.visual.leftBase2B0,
            &live.parameters.visual.rightBase2E0,&live.parameters.visual.leftBase2D0};
        for(unsigned engine=0;engine<4;++engine){
            auto enginePose=mainPose;
            for(unsigned axis=0;axis<3;++axis){
                volatile float a=mainPose.v[axis]*bases[engine]->v[0];
                volatile float b=mainPose.v[4+axis]*bases[engine]->v[1];
                volatile float c=mainPose.v[8+axis]*bases[engine]->v[2];
                volatile float ab=a+b;
                volatile float abc=ab+c;
                volatile float position=mainPose.v[12+axis]+abc;
                require(std::isfinite(position),"racer own engine cold placement arithmetic");
                enginePose.v[12+axis]=position;
            }
            require(setLocalMatrix49P(graph,roots[engine],enginePose)&&
                refreshSceneWorld49P(graph,roots[engine]),"racer own engine cold root placement rejected");
        }
    }
    runtime_startup50fl::project(state,live.bindings);
    for(auto* root:{&state.bodies.main.components.root,&state.bodies.secondary.root}){
        require(matrixToQuaternion48G(root->scene,root->body.orientation),
            "racer cold root orientation rejected");
        std::memcpy(root->body.position.v,root->scene.v+12,12);
    }
    live.state=state;
}
}
