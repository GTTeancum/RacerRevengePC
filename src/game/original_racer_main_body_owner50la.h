#pragma once
#include "original_application_entities50la.h"
#include "original_racer_model_scene50la.h"
#include "vehicle_frame49w.h"

namespace native_racer_model50LA {class Owner;}
namespace rr::game {
// The actual211F6C allocation's Entity/reference and previously unrepresented
// constructor fields. All existing canonical numerical340 fields remain in the SAME
// Fixture/Bank arena; there is no second body or private physics snapshot.
// Native resource staging precedes Session, so construction/scene binding are
// staged early and the original racer340 alias/Entity append are joined later.
// This is not a claim that intervening racer constructors have completed.
class OriginalRacerMainBodyOwner50LA final {
    friend class ::native_racer_model50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using K=OriginalWreckComponentKind50LA;
    using S=OriginalWreckComponentStatus50LA;
    using R=OriginalWreckComponentResult50LA;
    enum class Stage {allocated,base,constructed,sceneBound,appended,released};
    C entity_;
    MainBodyState49K* numeric_=nullptr;
    MainBodyParameters49K* parameters_=nullptr;
    OriginalApplicationEntities50LA* application_=nullptr;
    const OriginalRacerSceneObject50LA* scene288_=nullptr;
    Stage stage_=Stage::allocated;
    // Only source words absent from the existing numerical body owner.
    std::uint32_t word30_=0,flags200_=0,awake208_=0;
    std::uint32_t count264_=0,count268_=0,word284_=0;
    std::array<float,4> vector40_{},vector50_{},vectorF0_{};
    float cachedAnglesE0W_=0.f;
    BodyMatrix48G inertia100_{};
    //164ED0 writes28C=NULL. No cold auxiliary-array allocation has occurred;
    // admitting one later requires its genuine allocation/release owner.
    void* allocation28C_=nullptr;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static void copy(Vector3_48C& xyz,float& w,const std::array<std::uint32_t,4>& words)noexcept {
        std::memcpy(xyz.v,words.data(),12);std::memcpy(&w,words.data()+3,4);
    }
    static float word(std::uint32_t bits)noexcept {float f;std::memcpy(&f,&bits,4);return f;}
    R construct167920(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        VehicleFrameState49W& state,VehicleFrameParameters49W& parameters,
        VehicleFrameBindings49W& bindings)noexcept {
        if(stage_!=Stage::allocated||application_||numeric_)
            return {S::owner,0x167920u};
        application_=&app;numeric_=&state.bodies.main;parameters_=&parameters.bodies.main;
        const auto initialized=initializeOriginalWreckComponent50LA(entity_,K::body164ED0,
            generation,key(this),true,app.objectCount423F60());
        if(initialized.status!=S::complete)return initialized;
        stage_=Stage::base;
        auto& main=*numeric_;auto& root=main.components.root;auto& body=root.body;
        auto& p=parameters_->body;auto& lanes=state.bodies.mainSphere;
        auto& extra=state.mainExtra;auto& supplement=state.mainSupplement;
        const auto& zero=app.bodyOrigin433AD0_;
        //164FD8..165240. Copy-only source words, including the file-backed
        //382B60 quaternion (four +0 words), never a guessed identity quaternion.
        main.domain.bodyEnabled=true;awake208_=0;word30_=0;
        std::memcpy(vector40_.data(),zero.data(),16);
        std::memcpy(vector50_.data(),zero.data(),16);
        copy(body.position,extra.position60W,zero);
        copy(body.velocity,lanes.velocity70W,zero);
        copy(root.localVelocity80,supplement.localVelocity80W,zero);
        copy(root.worldAngularVelocityA0,extra.worldAngularVelocityA0W,zero);
        copy(body.angularVelocity,lanes.angularVelocityB0W,zero);
        copy(body.linearAcceleration,supplement.linearAccelerationC0W,zero);
        copy(body.angularAcceleration,supplement.angularAccelerationD0W,zero);
        copy(root.positionCorrection1a0,extra.positionCorrection1A0W,zero);
        copy(root.rotationCorrection1b0,extra.rotationCorrection1B0W,zero);
        copy(root.accumulator1c0,extra.accumulator1C0W,zero);
        copy(root.accumulator1d0,extra.accumulator1D0W,zero);
        std::memcpy(body.orientation.v,app.bodyQuaternion382B60_.data(),16);
        copy(main.cachedAnglesE0,cachedAnglesE0W_,zero);
        p.linearAccelerationLimitSquared=word(0x4963d240u);
        p.angularAccelerationLimitSquared=word(0x48742400u);
        p.linearVelocityLimitSquared=word(0x468e6200u);
        p.angularVelocityLimitSquared=word(0x441c4000u);
        p.mass=0.f;p.inverseMass=0.f;main.cachedSpeed34=0.f;word284_=0;
        //14D7F0 constructs a ZERO matrix, not identity. Retain100 separately
        //because the canonical numerical body currently represents only140.
        std::memset(inertia100_.v,0,sizeof inertia100_.v);
        for(auto& column:p.inverseInertiaColumns)std::memset(column.v,0,sizeof column.v);
        constexpr float inverseExtra[7]={0.f,0.f,0.f,0.f,0.f,0.f,0.f};
        std::memcpy(parameters.bodies.mainSphere.inverseInertiaExtra,inverseExtra,sizeof inverseExtra);
        std::memcpy(vectorF0_.data(),zero.data(),16);
        bindings.mainDomain.integrationMethod290=GraphBodyIntegrator49W::OriginalAdamsBashforth2;
        copy(parameters_->centerOffset,parameters.bodies.mainSphere.centerOffsetW,zero);
        count264_=count268_=0;allocation28C_=nullptr;scene288_=nullptr;
        main.domain.sceneAttached=false;main.components.body20C=0;root.callbackState238=0;
        //167948 installs3CE000.167950..980 install flags/axis locks, then
        //1679AC dispatches slotAC=167A50, which ends in slotA8=167AD0.
        entity_.kind=K::body167920;flags200_=0;
        for(unsigned i=0;i<3;++i)p.translationEnabled[i]=p.rotationEnabled[i]=true;
        const auto& reset=app.bodyReset433B80_;
        copy(root.loads.force,lanes.force180W,reset);
        copy(root.loads.torque,lanes.torque190W,reset);
        copy(body.linearAcceleration,supplement.linearAccelerationC0W,reset);
        copy(body.angularAcceleration,supplement.angularAccelerationD0W,reset);
        copy(body.velocity,lanes.velocity70W,reset);
        copy(root.localVelocity80,supplement.localVelocity80W,reset);
        copy(root.previousVelocity90,supplement.previousVelocity90W,reset);
        copy(body.angularVelocity,lanes.angularVelocityB0W,reset);
        main.cachedSpeed34=0.f;
        // Two167AD0 iterations, each with three distinct twelve-byte zero
        //fills and one quaternion copy; no temporary SecondaryBody is made.
        std::memset(supplement.positionDerivative2B8.v,0,12);
        std::memcpy(body.orientationDerivative.v,app.bodyQuaternion382B60_.data(),16);
        std::memset(supplement.linearAcceleration2D0.v,0,12);
        std::memset(supplement.angularAcceleration308.v,0,12);
        std::memset(body.previousPositionDerivative.v,0,12);
        std::memcpy(body.previousOrientationDerivative.v,app.bodyQuaternion382B60_.data(),16);
        std::memset(body.previousLinearAcceleration.v,0,12);
        std::memset(body.previousAngularAcceleration.v,0,12);
        main.domain.body320=0;
        // The native routing projection comes from this actual empty Entity,
        //not the inherited fixture's guessed descendant flags.
        main.domain.bodyChildCount=0;
        bindings.mainDomain.componentFlags24=entity_.descendants24;
        bindings.mainDomain.firstComponentChildPresent18=entity_.firstChild18!=nullptr;
        bindings.mainDomain.callbackMethod9C=GraphBodyCallback49W::Clear238;
        //211F98 slot30 is literal identity141C20; there are no extra stores.
        stage_=Stage::constructed;return {S::complete,0x211f98u};
    }
    R bindMain211FB0(const OriginalRacerSceneObject50LA& scene)noexcept {
        if(stage_!=Stage::constructed||!scene.constructed||scene.released||
           scene.entity().sessionGeneration!=entity_.sessionGeneration)
            return {S::owner,0x211fb0u};
        numeric_->domain.body320=1; //211FB8
        scene288_=&scene; //211FC8 ->1652E0, nonnull pointer store, no retain.
        numeric_->domain.sceneAttached=true;
        auto& p=parameters_->body;
        p.linearAccelerationLimitSquared=word(0x501502f9u);
        p.angularAccelerationLimitSquared=word(0x501502f9u);
        p.linearVelocityLimitSquared=word(0x501502f9u);
        p.angularVelocityLimitSquared=word(0x501502f9u);
        stage_=Stage::sceneBound;return {S::complete,0x211ffcu};
    }
    R append211FA8(const OriginalWreckComponentDomain50LA& domain,C& racer)noexcept {
        if(stage_!=Stage::sceneBound||!application_)return {S::owner,0x211fa8u};
        const auto r=appendOriginalWreckComponent50LA(domain,racer,entity_,2u,
            application_->attachmentLock423F58());
        if(r.status==S::complete)stage_=Stage::appended;
        return r;
    }
    C* liveEntity()noexcept{return entity_.reference00&&stage_!=Stage::released?&entity_:nullptr;}
    bool preflightClose(const C* racer,const C* previous,const C* next)const noexcept {
        if(stage_==Stage::released||!entity_.reference00)return true;
        if(!application_||application_->attachmentLock423F58()||entity_.reference00!=1u||
           entity_.firstChild18||entity_.next14!=next||allocation28C_!=nullptr)return false;
        if(stage_!=Stage::appended)return !entity_.parent1C&&!entity_.previous10&&!next;
        return racer&&previous&&entity_.parent1C==racer&&entity_.previous10==previous&&
            previous->next14==&entity_&&entity_.mask20==2u&&
            (!next||(next->parent1C==racer&&next->previous10==&entity_));
    }
    R close142590(const C* racer,const C* previous)noexcept {
        if(!preflightClose(racer,previous,nullptr))return {S::topology,0x142590u};
        if(stage_==Stage::released)return {S::complete,0x167a2cu};
        if(entity_.reference00){
            if(entity_.previous10)entity_.previous10->next14=entity_.next14;
            if(entity_.parent1C&&entity_.parent1C->firstChild18==&entity_)
                entity_.parent1C->firstChild18=entity_.next14;
            --entity_.reference00; //104970 ->1679D0 ->165260 ->1413E0.
            entity_.kind=K::body164ED0;
            //165288 tests actual28C=NULL: no auxiliary array needs freeing.
            entity_.kind=K::entity141300;--application_->objectCount423F60();
        }
        // Borrowed scene/numeric values and source stale sibling links survive.
        // No numeric zeroing, scene dereference, detach, or extra release.
        stage_=Stage::released;return {S::complete,0x167a2cu};
    }
public:
    OriginalRacerMainBodyOwner50LA()=default;
    OriginalRacerMainBodyOwner50LA(const OriginalRacerMainBodyOwner50LA&)=delete;
    OriginalRacerMainBodyOwner50LA& operator=(const OriginalRacerMainBodyOwner50LA&)=delete;
    ~OriginalRacerMainBodyOwner50LA(){if(entity_.reference00)std::terminate();}
    const C& entity()const noexcept{return entity_;}
    const OriginalRacerSceneObject50LA* scene288()const noexcept{return scene288_;}
    // Address comparison only outside the retained owner's actual bank visit.
    const MainBodyState49K* canonicalNumericAddress()const noexcept{return numeric_;}
};
}
