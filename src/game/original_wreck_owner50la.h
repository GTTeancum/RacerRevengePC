#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_tuning50la.h"
#include "original_wreck_queries50la.h"
#include "original_wreck_visibility50la.h"
#include "original_wreck_forces50la.h"
#include "original_wreck_effects50la.h"
#include "original_death_camera50kg.h"
#include <cstring>
#include <memory>
#include <string>

namespace rr::game {
// Actual application3FC/racer330 point-terrain provider. Its reached2E83D0
// runtime method remains separate from the collision namespace and water400.
struct OriginalWreckTerrainProvider50LA;
struct OriginalWreckEffectContextBinding50LA {
    // Addresses of actual source-context fields are mandatory. A missing field
    // is unbound; a bound field whose VALUE is null is the source null branch.
    // The same two source fields reach both2CB5A0 and2CC060.
    const OriginalWreckEffectResourceContext50LA* const* resourceContextField=nullptr;
    OriginalWreckComponent50LA* const* particleParentField=nullptr;
    // The effects are children of this actual EXTERNAL particle forest, not
    // the one-piece collection's Entity list. Stable storage includes its
    // real parent, ancestors and existing siblings, plus four reserved slots.
    OriginalWreckEffectComponentBinding50LA components{};
    OriginalWreckEffectRendererBinding50LA renderer{};
};
struct OriginalWreckOwnerVectors50LA {
    SpringVector4_48N vector4431C0{},vector443550{},vector4435A0{},vector4441F0{};
};
// File-backed static initializers3CAA20/3CAEA0/3CAF20/3CBC30 build each
// first stack vector with SW zero XYZ and raw3F800000 W, then store at
//3CAA7C/3CAEFC/3CAF7C/3CBC8C. These are actual initialized globals, not BSS
// guesses. Retain ONE application-owned returned value and lend its fields.
inline OriginalWreckOwnerVectors50LA initializedOriginalWreckOwnerVectors50LA()noexcept {
    OriginalWreckOwnerVectors50LA out;
    out.vector4431C0={{0.f,0.f,0.f,1.f}};out.vector443550={{0.f,0.f,0.f,1.f}};
    out.vector4435A0={{0.f,0.f,0.f,1.f}};out.vector4441F0={{0.f,0.f,0.f,1.f}};
    return out;
}
struct OriginalWreckOwnerBinding50LA {
    std::uint64_t sessionGeneration=0,racerKey=0;
    const std::uint64_t* componentKeys=nullptr;
    std::size_t componentKeyCount=0;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    const OriginalWreckConstructorGlobals50LA* constructorGlobals=nullptr;
    const SpringVector4_48N* actualVector4431C0=nullptr;
    const SpringVector4_48N* actualVector443550=nullptr;
    const SpringVector4_48N* actualVector4435A0=nullptr;
    const SpringVector4_48N* actualVector4441F0=nullptr;
    const std::uint8_t* soulTreeObjectType=nullptr; // actual289654 registry result
    const std::uint32_t* waterCategory4342A8=nullptr;
    SpringSceneId48N actualModelSceneIdentity{};
    const OriginalWreckTerrainProvider50LA* const* actualTerrain330Field=nullptr;
    const WaterHeightProvider50JC* const* actualWaterB0Field=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals=nullptr;
    OriginalWreckEffectContextBinding50LA effects{};
    OriginalWreckQueryWorld50LA queryWorld{};
    OriginalWreckVisibilityWorld50LA visibilityWorld{};
};
// Concrete D8 fields from295290/2953A0/2955F0, borrowing real DC/E0/terrain.
// There is no invented private collision object or detector point-pair array:
//165E90 passes a3=t0=0 to2953A0, so source60 remains null on this route.
struct OriginalWreckDetector50LA {
    OriginalWreckPhysical50LA* contact30=nullptr;
    const OriginalWreckTerrainProvider50LA* terrain34=nullptr;
    OriginalWreckQueries50LA::Query* query38=nullptr;
    SpringVector4_48N center40{},half50{};
    SpringVector4_48N* pairs60=nullptr;
    std::uint32_t enabled64=0,queryEnabled68=0,reset6C=0,count70=0;
    const CollectionRef50BX* objects74=nullptr;
    bool constructed=false;
};
struct OriginalWreckEffectFields50LA {
    // Source2CB4B0 begins with null children. The concrete effectOwners below
    // owns each genuinely constructed child; these fields describe admission.
    std::uint32_t count174=0,dustActive178=0;
    float dustTime17C=0.f;
    bool pieceNullRouteClosed=false,groupNullRouteClosed=false;
    bool pieceRouteClosed=false,groupRouteClosed=false;
};
enum class OriginalWreckOwnerStatus50LA : std::uint8_t {
    assetsPrepared,storagePrepared,constructed,binding,assets,allocation,components,physical,
    queries,visibility,contactPoints,pieceEffects,groupEffects,tuning,stopped
};
struct OriginalWreckOwnerResult50LA {
    OriginalWreckOwnerStatus50LA status=OriginalWreckOwnerStatus50LA::binding;
    std::uint32_t sourcePC=0;
    std::size_t initializedComponents=0;
    bool sourcePrefixCommitted=false,collectionLoaded=false,tuningCommitted=false;
};

// One authenticated one-piece collection, never a replacement for racer370/374.
// Addresses are stable. Cold preparation allocates/decodes only; source owner
// construction runs once in order and retains its prefix on a later boundary.
// Neither successful construction nor its camera view certifies activation,
// rendering, scheduler/physics updates or the other authored collections.
class OriginalOnePieceWreckOwner50LA final {
public:
    OriginalWreckAssets50LA assets;
    OriginalWreckPhysical50LA physical;
    OriginalWreckPieceFields50LA piece;
    OriginalWreckSingleGroupFields50LA group;
    OriginalWreckGravity50LA gravity;
    OriginalWreckQueries50LA queries;
    OriginalWreckVisibility50LA visibility;
    OriginalWreckDetector50LA detector;
    OriginalWreckEffectFields50LA effects;
    OriginalWreckEffects50LA effectOwners;
    rr::assets::OriginalUiPageDiagnostic50KG effectDiagnostic{};
    std::unique_ptr<OriginalWreckComponent50LA[]> components;
    std::unique_ptr<OriginalWreckComponent50LA*[]> componentPointers;
    OriginalWreckComponentDomain50LA domain{};
    SceneGraph49P deathGraph{}; // persistent descriptor, borrowed by the camera
    std::unique_ptr<std::uint8_t[]> sceneObjectFlags03;
    const WaterHeightProvider50JC* actualWaterB0=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals=nullptr;

    OriginalOnePieceWreckOwner50LA()=default;
    OriginalOnePieceWreckOwner50LA(const OriginalOnePieceWreckOwner50LA&)=delete;
    OriginalOnePieceWreckOwner50LA& operator=(const OriginalOnePieceWreckOwner50LA&)=delete;
    OriginalOnePieceWreckOwner50LA(OriginalOnePieceWreckOwner50LA&&)=delete;
    OriginalOnePieceWreckOwner50LA& operator=(OriginalOnePieceWreckOwner50LA&&)=delete;

    // Load actual geometry BEFORE shared track/query storage is sized. The
    // caller can then charge targetE4's real mesh and bind QueryBridge without
    // placeholder world pointers or a second asset owner.
    OriginalWreckOwnerResult50LA prepareAssetsCold(const OriginalWreckAssetBinding50LA& assetBinding,
        const OriginalWreckEndpointPolicy50LA& endpoint,std::string& error) {
        using S=OriginalWreckOwnerStatus50LA;
        if(assetsAttempted_||assets.prepared())return result(S::binding,0x165c30u);
        assetsAttempted_=true;assetVectors_=assetBinding.constructorVectors;
        if(!assets.prepare(assetBinding,endpoint,error))return result(S::assets,0x165cd4u);
        deathGraph=assets.graph();sceneCount_=deathGraph.count;
        capacity_=sceneCount_+10u;
        if(!sceneCount_||capacity_>maxOriginalWreckComponents50LA)return result(S::binding,0x141300u);
        assetsPrepared_=true;return result(S::assetsPrepared,0u);
    }
    // After actual shared world sizing/binding, allocate remaining native
    // backing. No original component counter, child list, canonical query
    // table or activity write occurs here. StoragePrepared is not constructed.
    OriginalWreckOwnerResult50LA prepareCold(const OriginalWreckOwnerBinding50LA& binding)noexcept {
        using S=OriginalWreckOwnerStatus50LA;
        if(!assetsPrepared_||attempted_||!validBinding(binding)||
           binding.componentKeyCount!=capacity_)return result(S::binding,0x22bfd0u);
        attempted_=true;binding_=binding;
        preparedEffectContext_=*binding.effects.resourceContextField;
        preparedParticleParent_=*binding.effects.particleParentField;
        hasEffects_=preparedEffectContext_&&preparedParticleParent_;
        for(std::size_t i=0;i<capacity_;++i){
            if(!binding.componentKeys[i])return result(S::binding,0x141300u);
            for(std::size_t j=0;j<i;++j)if(binding.componentKeys[j]==binding.componentKeys[i])
                return result(S::binding,0x141300u);
            if(hasEffects_)for(auto key:binding.effects.components.componentKeys)
                if(key==binding.componentKeys[i])return result(S::binding,0x141300u);
        }
        try {
            components=std::make_unique<OriginalWreckComponent50LA[]>(capacity_);
            componentPointers=std::make_unique<OriginalWreckComponent50LA*[]>(capacity_);
            keys_=std::make_unique<std::uint64_t[]>(capacity_);
            sceneObjectFlags03=std::make_unique<std::uint8_t[]>(sceneCount_);
            const auto& resource=assets.queryE0();pointStackCapacity_=resource.nodeCount+resource.rayCount;
            pointStack_=std::make_unique<std::uint16_t[]>(pointStackCapacity_);
            for(std::size_t i=0;i<capacity_;++i){componentPointers[i]=&components[i];keys_[i]=binding.componentKeys[i];}
            domain={binding.sessionGeneration,componentPointers.get(),0};
        }catch(...){return result(S::allocation,0x22c084u);}
        const auto q=queries.prepareCold(assets,*this,physical,binding.queryWorld);
        if(q.status!=OriginalWreckQueryStatus50LA::complete)return result(S::queries,q.sourcePC);
        if(!visibility.prepareCold(assets,binding.visibilityWorld))return result(S::visibility,0x28e8a0u);
        if(hasEffects_){
            const auto e=effectOwners.prepareCold(*preparedEffectContext_,effectDiagnostic);
            if(!e.okay())return result(S::pieceEffects,e.sourcePC);
        }
        storagePrepared_=true;return result(S::storagePrepared,0u);
    }
    bool assetsPrepared()const noexcept{return assetsPrepared_;}
    bool storagePrepared()const noexcept{return storagePrepared_;}
    bool constructed()const noexcept{return constructed_;}
    // Admission alias: an effects-stopped storage owner is never prepared for
    // runtime consumers. The separate storagePrepared() describes cold backing.
    bool prepared()const noexcept{return constructed_&&coherent();}
    // Same source model identity already used by the authentic DC POINT
    // producer. The world contact callback may not supply a second scene key.
    SpringSceneId48N modelSceneIdentity()const noexcept{return binding_.actualModelSceneIdentity;}
    const SpringVector4_48N* contactVector4435A0()const noexcept{return binding_.actualVector4435A0;}
    const SpringVector4_48N* groupVector4441F0()const noexcept{return binding_.actualVector4441F0;}
    std::size_t componentCapacity()const noexcept{return capacity_;}
    OriginalWreckComponent50LA& collectionComponent()noexcept{return components[0];}
    OriginalWreckComponent50LA& pieceComponent()noexcept{return components[1];}
    OriginalWreckComponent50LA& modelComponent()noexcept{return components[2];}
    OriginalWreckComponent50LA& bodyComponent()noexcept{return components[bodyIndex()];}
    OriginalWreckComponent50LA& gravityComponent()noexcept{return components[bodyIndex()+1];}
    OriginalWreckComponent50LA& contactComponent()noexcept{return components[bodyIndex()+3];}
    OriginalWreckComponent50LA& detectorComponent()noexcept{return components[bodyIndex()+4];}
    OriginalWreckComponent50LA& groupComponent()noexcept{return components[bodyIndex()+7];}
    OriginalWreckComponent50LA& queryComponent(unsigned i)noexcept {
        return components[bodyIndex()+(i==0?2:i==1?5:6)];
    }

    OriginalWreckOwnerResult50LA constructCold()noexcept {
        using S=OriginalWreckOwnerStatus50LA;using K=OriginalWreckComponentKind50LA;
        if(!storagePrepared_||constructionAttempted_)return result(S::stopped,stoppedPC_);
        if(!sameEffectContext())return result(S::binding,0x2cb618u);
        constructionAttempted_=true;
        if(!initialize(0,K::collection22BF00))return result(S::components,0x22bf0cu);
        collectionCount30_=1; // actual selected NumberOfParts, not racer370
        if(!initialize(1,K::piece2CB4B0))return result(S::components,0x1659ccu);
        initializeOriginalWreckPieceFields50LA(piece,*binding_.constructorGlobals);
        effects.dustActive178=0;effects.dustTime17C=0.f; //2CB4E4/2CB4E8
        std::memcpy(piece.authored50.v,assetVectors_->position434830.data(),16);
        std::memcpy(piece.look60.v,assetVectors_->look434860.data(),16);
        std::memcpy(piece.up70.v,assetVectors_->up434850.data(),16);
        actualWaterB0=*binding_.actualWaterB0Field;actualWaterGlobals=binding_.actualWaterGlobals;
        for(std::size_t row=0;row<sceneCount_;++row){
            if(!initialize(2+row,row?K::scene2893A0:K::model2403A0))return result(S::components,0x289418u);
            components[2+row].category28=*binding_.soulTreeObjectType;
            sceneObjectFlags03[row]=static_cast<std::uint8_t>((sceneObjectFlags03[row]&~7u)|(row?0u:3u));
            // PSG hierarchy180/184 is separate: do NOT append child rows to
            // the model's or piece's component child18 list.
        }
        if(!append(1,2,0u))return result(S::components,0x165cf0u);
        std::memcpy(piece.authored50.v,assets.authored50().data(),16);
        const auto b=bodyIndex();
        if(!initialize(b,K::body167920)||!initializeOriginalWreckBody50LA(physical,components[b],*binding_.constructorGlobals)||
           !bindOriginalWreckBodyModel50LA(physical,deathGraph,assets.model14C()))return result(S::physical,0x165f10u);
        if(!append(1,b,0xffffffffu))return result(S::components,0x165f3cu);
        if(!initialize(b+1,K::gravity282670)||!initializeOriginalWreckGravity50LA(gravity,physical,
           components[b+1],*binding_.actualVector4431C0))return result(S::physical,0x165f58u);
        if(!append(1,b+1,0u))return result(S::components,0x165f94u);
        if(!publishQuery(0))return result(S::queries,0x16607cu);
        if(!append(1,b+2,0u))return result(S::components,0x166098u);
        if(!initialize(b+3,K::contact295A40)||!initializeOriginalWreckContact50LA(physical,
           components[b+3],*binding_.actualVector4435A0))return result(S::physical,0x1660b4u);
        if(!constructContactPoints())return result(S::contactPoints,0x296190u);
        if(!append(1,b+3,0u))return result(S::components,0x166164u);
        if(!initialize(b+4,K::detector295290))return result(S::components,0x166180u);
        detector.contact30=&physical;detector.terrain34=*binding_.actualTerrain330Field;
        if(!detector.terrain34)physical.generated.pointContacts48=false; //2953D8..E8
        detector.center40=*binding_.actualVector443550;detector.half50=*binding_.actualVector443550;
        detector.pairs60=nullptr;detector.enabled64=1;detector.queryEnabled68=1;detector.reset6C=1;
        detector.count70=0;detector.objects74=nullptr;
        if(!queries.bindContactQuery())return result(S::queries,0x2955f0u);
        detector.query38=&queries.query(0);detector.constructed=true;
        if(!append(1,b+4,0u))return result(S::components,0x1661d0u);
        if(!publishQuery(1))return result(S::queries,0x1662c0u);
        if(!append(1,b+5,0u))return result(S::components,0x1662dcu);
        if(!publishQuery(2))return result(S::queries,0x1662f8u);
        if(!append(1,b+6,0u))return result(S::components,0x166324u);
        if(!queries.bindMutualExclusions())return result(S::queries,0x166390u);
        if(!queries.bindDerivedResponse())return result(S::queries,0x2cb610u);
        const auto& authored=assets.selected();
        effects.count174=authored.pieces[0].emitSmoke?authored.maxNumberOfDamageEmitters:0u;
        //2CB618/620 test the two actual context VALUES. Missing bindings were
        // rejected at preparation. A nonnull pair cannot bypass constructors.
        if(hasEffects_){
            const auto e=effectOwners.constructPiece(binding_.effects.components,effects.count174);
            if(!e.okay())return result(S::pieceEffects,e.sourcePC);
        }else effects.pieceNullRouteClosed=true;
        effects.pieceRouteClosed=true;
        if(!append(0,1,2u))return result(S::components,0x22c1b8u);
        physical.generated.frictionTorqueScaleC4=word(0x3e4ccccdu);
        piece.changeWindow130=2.f;
        physical.parameters.body.linearVelocityLimitSquared=word(0x49034acfu);
        if(!queries.applyCollectionWaterExclusion(*binding_.waterCategory4342A8))return result(S::queries,0x22c214u);
        if(!initialize(b+7,K::group2CBFC0))return result(S::components,0x2c9e4cu);
        group.attachedCount30=0;group.capacity34=0;group.mass4C=0.f;
        group.center50=*binding_.actualVector4441F0;
        group.inertia60=original_wreck_physical_detail50LA::identity();
        if(!bindOriginalOnePieceGroup50LA(group,physical,piece,pieceComponent()))return result(S::physical,0x2ca020u);
        if(hasEffects_){
            const auto e=effectOwners.constructGroup(binding_.effects.components,assets,visibility,
                binding_.effects.renderer);
            if(!e.okay())return result(S::groupEffects,e.sourcePC);
        }else effects.groupNullRouteClosed=true;
        effects.groupRouteClosed=true;
        if(!append(0,b+7,2u))return result(S::components,0x22c2c0u);
        //22CE10: collection141520, group2CC9C0, then piece2CBEB0. The group
        // override resets/stops sparks and explosion, then stops the primary
        // splash before its own141520. Count1740 piece performs no effect
        // calls before141520. Emitter COMPONENT activity is not rewritten.
        if(!setInactive(collectionComponent()))return result(S::components,0x22ce24u);
        if(hasEffects_){
            const auto e=effectOwners.deactivateGroupEffects();
            if(!e.okay())return result(S::groupEffects,e.sourcePC);
        }
        if(!setInactive(groupComponent())||!setInactive(pieceComponent()))
            return result(S::components,0x22ce10u);
        if(setOriginalWreckBodyEnabled50LA(physical,false)!=OriginalWreckBodyControlStatus50LA::complete)
            return result(S::physical,0x22ce80u);
        sceneObjectFlags03[0]=static_cast<std::uint8_t>(sceneObjectFlags03[0]|4u); //28C6E0
        const auto vr=visibility.remove();
        if(vr.status!=OriginalWreckVisibilityStatus50LA::complete)return result(S::visibility,vr.sourcePC);
        for(unsigned i=0;i<3;++i){const auto q=queries.remove(i);
            if(q.status!=OriginalWreckQueryStatus50LA::complete)return result(S::queries,q.sourcePC);}
        collectionLoaded_=true;
        if(!tuneOriginalOnePieceWreck50LA(assets,physical,piece,group))return result(S::tuning,0x22c310u);
        tuningCommitted_=true;gravity.acceleration38=assets.authoring().gravityAcceleration; //22C610
        deathPiece_={&components[1].flags29,&deathGraph,assets.model14C()};
        deathCollection_={&deathPiece_,1u};constructed_=true;stoppedPC_=0;
        return result(S::constructed,0u);
    }

    // Borrow only this selected authored collection. The racer owner must place
    // it at selectedCollection in its real complete ordered374 array; this API
    // never fabricates that array or claims all other collections are present.
    const OriginalDeathPieceCollection50KG* cameraCollection()const noexcept {
        return prepared()?&deathCollection_:nullptr;
    }
    std::size_t authoredCollectionOrdinal()const noexcept{return assets.selected().ordinal;}
    bool coherent()const noexcept {
        if(!constructed_||!queries.constructed()||!visibility.boundTo(assets)||
           !effects.pieceRouteClosed||!effects.groupRouteClosed||!sameEffectContext()||
           domain.objects!=componentPointers.get()||domain.count!=capacity_||
           !original_wreck_component_detail50LA::domain(domain)||
           gravity.target50!=&physical||group.primary3C!=&physical||group.primary40!=&piece||
           detector.contact30!=&physical||detector.query38!=&queries.query(0)||!detector.constructed)return false;
        const auto current=const_cast<OriginalWreckAssets50LA&>(assets).graph();
        if(deathGraph.nodes!=current.nodes||deathGraph.count!=current.count)return false;
        for(unsigned i=0;i<3;++i)if(!queries.coherent(i))return false;
        if(hasEffects_){
            if(!effectOwners.pieceConstructed()||!effectOwners.groupConstructed()||!effectOwners.coherent())return false;
            const auto& d=*binding_.effects.components.domain;
            if(!original_wreck_component_detail50LA::domain(d)||d.sessionGeneration!=binding_.sessionGeneration)return false;
            for(unsigned i=0;i<4;++i){
                const auto& c=const_cast<OriginalWreckEffects50LA&>(effectOwners).component(
                    static_cast<OriginalWreckEffectSlot50LA>(i));
                if(!original_wreck_component_detail50LA::contains(d,&c)||
                   c.parent1C!=preparedParticleParent_||c.mask20!=0x0402u||
                   c.objectKey!=binding_.effects.components.componentKeys[i])return false;
            }
        }
        return true;
    }
private:
    OriginalWreckOwnerBinding50LA binding_{};
    const OriginalWreckEffectResourceContext50LA* preparedEffectContext_=nullptr;
    OriginalWreckComponent50LA* preparedParticleParent_=nullptr;
    bool hasEffects_=false;
    const OriginalWreckConstructorVectors50LA* assetVectors_=nullptr;
    std::unique_ptr<std::uint64_t[]> keys_;
    std::unique_ptr<std::uint16_t[]> pointStack_;
    std::size_t sceneCount_=0,capacity_=0,pointStackCapacity_=0;
    std::uint32_t collectionCount30_=0,stoppedPC_=0;
    bool assetsAttempted_=false,assetsPrepared_=false,attempted_=false;
    bool storagePrepared_=false,constructionAttempted_=false;
    bool constructed_=false,collectionLoaded_=false,tuningCommitted_=false;
    OriginalDeathPiece50KG deathPiece_{};
    OriginalDeathPieceCollection50KG deathCollection_{};
    std::size_t bodyIndex()const noexcept{return 2u+sceneCount_;}
    static float word(std::uint32_t u)noexcept {float f;std::memcpy(&f,&u,4);return f;}
    static bool validBinding(const OriginalWreckOwnerBinding50LA& b)noexcept {
        const bool base=b.sessionGeneration&&b.racerKey&&b.componentKeys&&b.componentKeyCount&&
            b.actualObjectCount423F60&&b.actualAttachmentLock423F58&&b.constructorGlobals&&
            b.actualVector4431C0&&b.actualVector443550&&b.actualVector4435A0&&b.actualVector4441F0&&
            b.soulTreeObjectType&&*b.soulTreeObjectType!=0xffu&&b.waterCategory4342A8&&
            *b.waterCategory4342A8&&b.actualModelSceneIdentity.value&&b.actualWaterB0Field&&
            b.actualTerrain330Field&&
            (!*b.actualWaterB0Field||b.actualWaterGlobals)&&b.effects.resourceContextField&&b.effects.particleParentField;
        if(!base)return false;
        if(!*b.effects.resourceContextField||!*b.effects.particleParentField)return true;
        const auto& e=b.effects.components;
        return e.domain&&e.domain->sessionGeneration==b.sessionGeneration&&
            original_wreck_component_detail50LA::domain(*e.domain)&&e.domainStorage==e.domain->objects&&
            e.domainCapacity>=e.domain->count&&e.domainCapacity-e.domain->count>=4u&&
            e.domainCapacity<=maxOriginalWreckComponents50LA&&
            e.particleParent==*b.effects.particleParentField&&
            original_wreck_component_detail50LA::contains(*e.domain,e.particleParent)&&
            e.actualObjectCount423F60==b.actualObjectCount423F60&&
            e.actualAttachmentLock423F58==b.actualAttachmentLock423F58;
    }
    bool sameEffectContext()const noexcept {
        return binding_.effects.resourceContextField&&binding_.effects.particleParentField&&
            *binding_.effects.resourceContextField==preparedEffectContext_&&
            *binding_.effects.particleParentField==preparedParticleParent_;
    }
    OriginalWreckOwnerResult50LA result(OriginalWreckOwnerStatus50LA status,std::uint32_t pc)noexcept {
        if(pc)stoppedPC_=pc;
        const auto initialized=domain.count+effectOwners.initializedChildren();
        return {status,pc,initialized,initialized!=0,collectionLoaded_,tuningCommitted_};
    }
    bool initialize(std::size_t index,OriginalWreckComponentKind50LA kind)noexcept {
        if(index!=domain.count||index>=capacity_)return false;
        const auto r=initializeOriginalWreckComponent50LA(components[index],kind,binding_.sessionGeneration,
            keys_[index],true,*binding_.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        ++domain.count;return true;
    }
    bool append(std::size_t parent,std::size_t child,std::uint32_t mask)noexcept {
        return appendOriginalWreckComponent50LA(domain,components[parent],components[child],mask,
            *binding_.actualAttachmentLock423F58).status==OriginalWreckComponentStatus50LA::complete;
    }
    bool setInactive(OriginalWreckComponent50LA& c)noexcept {
        return setOriginalWreckBaseActivity50LA(domain,c,false).status==OriginalWreckComponentStatus50LA::complete;
    }
    bool publishQuery(unsigned i)noexcept {
        const auto index=bodyIndex()+(i==0?2:i==1?5:6);
        // Object.flags31 and embedded component.flags29 are one source byte.
        // Preserve the actual canonical fresh storage's high nibble; query
        // publication binds/guards equality with this component thereafter.
        const auto& w=binding_.queryWorld;
        components[index].flags29=w.objects[w.refs[i]-1u].filter.flags31;
        if(!initialize(index,OriginalWreckComponentKind50LA::collision11B190))return false;
        components[index].category28=*w.collisionTypeGp8010;
        return queries.publishColdQuery(i,components[index]).status==OriginalWreckQueryStatus50LA::complete;
    }
    bool constructContactPoints()noexcept {
        const auto& resource=assets.queryE0();
        if(!pointStackCapacity_||!pointStack_||physical.spring.pointCount7C||
           physical.spring.generatedCapacity88!=50)return false;
        std::size_t top=0,remaining=resource.nodeCount+resource.rayCount;
        pointStack_[top++]=resource.root;
        while(top){
            if(!remaining--)return false;
            const auto ref=pointStack_[--top];
            if(ref&type2Leaf49X){
                const auto ray=static_cast<std::size_t>(ref&~type2Leaf49X);
                if(ray>=resource.rayCount)return false;
                if(!appendPoint(resource.rays[ray].second))return false;
            }else{
                if(ref>=resource.nodeCount||top+2>pointStackCapacity_)return false;
                //296308/295FD0 source DFS: first child then second child.
                pointStack_[top++]=resource.nodes[ref].children[1];
                pointStack_[top++]=resource.nodes[ref].children[0];
            }
        }
        return true;
    }
    bool appendPoint(const SpringVector4_48N& point)noexcept {
        auto& s=physical.spring;
        if(s.pointCount7C>=50u)return true; //295E18 saturation precedes duplicate search
        for(std::size_t i=0;i<s.pointCount7C;++i){const auto& old=physical.pointRecords[i];
            if(old.localPoint10.v[0]==point.v[0]&&old.localPoint10.v[1]==point.v[1]&&
               old.localPoint10.v[2]==point.v[2]&&old.scene40.value==binding_.actualModelSceneIdentity.value)return true;
        }
        auto& next=physical.pointRecords[s.pointCount7C];
        next.scene40=binding_.actualModelSceneIdentity;next.localPoint10=point;
        next.flags50=static_cast<std::uint8_t>((next.flags50&~1u)|1u);next.margin48=0.f;
        next.normal20=*binding_.actualVector4435A0;next.surfacePoint30=*binding_.actualVector4435A0;
        ++s.pointCount7C;return true;
    }
};
} // namespace rr::game
