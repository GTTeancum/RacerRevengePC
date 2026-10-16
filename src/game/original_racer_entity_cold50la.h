#pragma once
#include "original_application_entities50la.h"
#include "original_camera_start_native.h"
#include "original_gc_particle_manager50kg.h"
#include "undertrack_reset50ew.h"
#include "../assets/original_standard_screen_material_reference50la.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace native_racer50HW { class Cables; }
namespace native_racer_model50LA {class Owner;}
namespace rr::game {
class OriginalRacerStripOwners50LA;
class OriginalRacerModelSceneOwner50LA;
class OriginalRacerAnimationCold50LA;
class OriginalPodCableAllocation50LA;
enum class OriginalRacerEntityColdStatus50LA : std::uint8_t {
    complete,binding,alreadyConstructed,material,component,attachment,entityType,allocation
};
struct OriginalRacerEntityColdResult50LA {
    OriginalRacerEntityColdStatus50LA status=OriginalRacerEntityColdStatus50LA::binding;
    std::uint32_t sourcePC=0;
    std::size_t initializedComponents=0,constructedArrays=0;
    bool okay()const noexcept{return status==OriginalRacerEntityColdStatus50LA::complete;}
};
enum class OriginalRacerEntityColdCleanupStatus50LA : std::uint8_t {
    complete,binding,topology,attachmentLocked,renderer
};
struct OriginalRacerEntityColdCleanupResult50LA {
    OriginalRacerEntityColdCleanupStatus50LA status=OriginalRacerEntityColdCleanupStatus50LA::binding;
    std::uint32_t sourcePC=0;
    OriginalCameraStartResult renderer{OriginalCameraStartStatus::complete,0};
    bool okay()const noexcept{return status==OriginalRacerEntityColdCleanupStatus50LA::complete;}
};

// Read-only adapter to the SAME full application382B38 used by Standard models.
// Binding never loads Mat_Stbl, resets a row, copies templates, or increments a
// reference. AIDamage primitive owners perform their actual per-primitive retain.
// The adapter may be destroyed after construction: each primitive pins the real
// table independently. No racer or race creates a replacement material manager.
class OriginalAIDamageMaterialRow50LA final {
public:
    OriginalAIDamageMaterialRow50LA()=default;
    OriginalAIDamageMaterialRow50LA(const OriginalAIDamageMaterialRow50LA&)=delete;
    OriginalAIDamageMaterialRow50LA& operator=(const OriginalAIDamageMaterialRow50LA&)=delete;
    bool bindApplication(std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
        rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic)noexcept {
        using S=rr::assets::OriginalStandardMaterialStatus50LA;
        diagnostic={};
        if(table_){diagnostic.status=S::alreadyPrepared;diagnostic.sourcePC=0x299BE0;return false;}
        if(!table||!table->prepared()||table->populationTerminal()||table->demandTerminal()){
            diagnostic.status=S::phase;diagnostic.sourcePC=0x299BE0;return false;
        }
        try {
            std::int32_t index=-1;
            if(!table->find298C80("textures\\AIDamage",index,diagnostic))return false;
            // Immutable first-match identity only. All mutable row state remains
            // in table; even byte offset is read directly from its canonical row.
            registryIndex00_=index;table_=std::move(table);return true;
        }catch(...){diagnostic.status=S::allocation;diagnostic.sourcePC=0x299BE0;return false;}
    }
    bool prepared()const noexcept {
        return table_&&table_->prepared()&&!table_->populationTerminal()&&
            !table_->demandTerminal()&&registryIndex00_>=0;
    }
    const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>& table382B38()const noexcept {
        return table_;
    }
    std::int32_t registryIndex00()const noexcept{return registryIndex00_;}
    bool row(rr::assets::OriginalStandardMaterialRowView50LA& out)const noexcept {
        return table_&&table_->row(registryIndex00_,out);
    }
    // Actual36B970, never a path inferred from the selected model or UI page.
    bool acceptsDirectory(const char* directory,std::size_t capacity)const noexcept {
        return prepared()&&rr::assets::OriginalStandardScreenMaterialReference50LA::
            acceptsAIDamageDirectory299BE0(directory,capacity);
    }
private:
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table_;
    std::int32_t registryIndex00_=-1;
};

// Native CPU ownership of the genuine2E2190 vertex /2E27C0 packet aliases.
// These are data records, not guest addresses, DMA/VIF execution or a GPU
// readiness claim. Four source vertices remain four independent vertices.
struct OriginalAIDamagePacketVertex50LA {
    std::array<std::uint32_t,4> rgba{};
    std::array<float,4> stq{};
    std::array<float,4> xyz{{0.f,0.f,1.f,0.f}};
};
struct OriginalAIDamageVertex50LA {
    std::array<float,4> authored00{{0.f,0.f,1.f,1.f}};
    std::uint32_t copies10=0;
    std::array<std::array<float,4>*,2> stq14{};
    std::array<std::array<std::uint32_t,4>*,2> rgba1C{};
    std::array<std::array<float,4>*,2> xyz24{};
};
struct OriginalAIDamagePrimitive50LA {
    //2461E0->299AA0,2465B0->299BE0 and2E2144 native packet attachment.
    const std::int32_t& material00()const noexcept{return materialReference_.material00();}
    const rr::assets::OriginalStandardMaterialTable50LA* materialOwner()const noexcept {
        return materialReference_.table382B38();
    }
    std::uint8_t passMask0C=0,parameterCount0D=0,field0E=0xffu,flags0F=0;
    std::array<OriginalAIDamagePacketVertex50LA,4>* packet10=nullptr;
    std::array<float,2> scales18{{1.f,1.f}};
    std::uint16_t field20=0,field22=0;
private:
    friend class OriginalAIDamagePrimitiveArray50LA;
    rr::assets::OriginalStandardScreenMaterialReference50LA materialReference_;
};
class OriginalAIDamagePrimitiveArray50LA final {
public:
    std::array<OriginalAIDamageVertex50LA,4> vertices00{};
    std::array<OriginalAIDamagePacketVertex50LA,4> packet{};
    OriginalAIDamagePrimitive50LA primitive;
    std::uint32_t count04=0,count08=0,type0C=2;
    float scale10=1.f,field14=0.f,field18=1.f;
    std::array<float,4> offset20{{0.f,0.f,0.f,1.f}}; //2E1920 SQ vf0, then20/24 zero
    std::uint32_t count34=0,owns38=0,dirty3C=0,batchCount48=0,batchCapacity4C=0;
    std::uint8_t flags40=3;
    OriginalAIDamagePrimitiveArray50LA()=default;
    OriginalAIDamagePrimitiveArray50LA(const OriginalAIDamagePrimitiveArray50LA&)=delete;
    OriginalAIDamagePrimitiveArray50LA& operator=(const OriginalAIDamagePrimitiveArray50LA&)=delete;
    ~OriginalAIDamagePrimitiveArray50LA(){if(attempted_&&cleanupPhase_!=2u)std::terminate();}
    bool construct199950(OriginalAIDamageMaterialRow50LA& material,
        const char* actualDirectory36B970,std::size_t capacity)noexcept {
        if(attempted_)return false;
        // The owner already published this constructed2E1920 array. Even a
        // failed setup must take its two real renderer calls before disposal.
        attempted_=true;
        if(!material.acceptsDirectory(actualDirectory36B970,capacity))return false;
        //2E1AE0(0,4,1,"AIDamage"): one batch, four fresh2E2190 vertices;
        //2E27C0 links their first packet copies. The second aliases stay null.
        type0C=1;count04=count08=4;owns38=1;dirty3C=1;
        count34=batchCount48=batchCapacity4C=1;
        for(unsigned i=0;i<4;++i){auto& v=vertices00[i];
            v.rgba1C[0]=&packet[i].rgba;v.stq14[0]=&packet[i].stq;v.xyz24[0]=&packet[i].xyz;
            v.copies10=1;
        }
        // One real primitive/material association per array, not per vertex.
        if(!primitive.materialReference_.acquireAIDamage299BE0(material.table382B38(),
           actualDirectory36B970,capacity,materialDiagnostic_))return false;
        primitive.passMask0C=15;primitive.flags0F=std::uint8_t(primitive.flags0F&~1u);
        primitive.packet10=&packet;
        //1999D8..199A2C alpha stores touch every existing packet copy.
        for(auto& v:vertices00)for(auto* rgba:v.rgba1C)if(rgba)(*rgba)[3]=0x3fu;
        prepared_=true;return true;
    }
    bool prepared()const noexcept {
        if(!prepared_||!primitive.materialReference_.retained()||!primitive.materialOwner()||
           !primitive.materialOwner()->prepared()||primitive.materialOwner()->populationTerminal()||
           primitive.materialOwner()->demandTerminal()||
           primitive.packet10!=&packet||primitive.material00()<0||
           count04!=4||count08!=4||type0C!=1||batchCount48!=1||batchCapacity4C!=1)return false;
        for(unsigned i=0;i<4;++i){const auto& v=vertices00[i];
            if(v.copies10!=1||v.rgba1C[0]!=&packet[i].rgba||v.stq14[0]!=&packet[i].stq||
               v.xyz24[0]!=&packet[i].xyz||v.rgba1C[1]||v.stq14[1]||v.xyz24[1])return false;}
        return true;
    }
    bool constructed199950()const noexcept{return prepared_;}
    const rr::assets::OriginalStandardMaterialDiagnostic50LA& materialDiagnostic()const noexcept {
        return materialDiagnostic_;
    }
    void primaryColor199A7C()noexcept {
        // Literal00646464 ->133F20, RGB individually unsigned>>1. Alpha stays63.
        for(auto& v:vertices00)for(auto* rgba:v.rgba1C)if(rgba)
            for(unsigned lane=0;lane<3;++lane)(*rgba)[lane]=50u;
    }
private:
    friend class OriginalPodAIDamageOverlay50LA;
    // Exact cold2E1980 order: renderer slot20(1), then slot24 again, then
    // primitive246260->299AC0, packet storage and vertices. This owner never
    // submits an overlay. Native embedded packet/vertex storage dies with it;
    // no guest allocation, DMA execution or full runtime destructor is claimed.
    OriginalRacerEntityColdCleanupResult50LA releaseCold2E1980(
        const OriginalRendererDispatchFields& fields,platform::VehicleWindowGl50DS& renderer,
        platform::VehicleWindowDiagnostic50DS& diagnostic)noexcept {
        using S=OriginalRacerEntityColdCleanupStatus50LA;
        if(cleanupPhase_==2u)return {S::complete,0x2e1ad4u};
        if(!cleanupPhase_){
            std::int32_t actualV0=0;
            const auto r=serviceOriginalRenderer244D60(fields,true,renderer,diagnostic,actualV0,
                OriginalCameraDrainPolicy::initializedContext);
            if(r.status!=OriginalCameraStartStatus::complete)return {S::renderer,r.sourcePC,r};
            cleanupPhase_=1;
        }
        //2E19CC is unconditional even if244D60 took its disabled-D8 return.
        // A genuine cold failure may predate the first committed image.
        if(!renderer.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)
            return {S::renderer,0x2e19ccu,{OriginalCameraStartStatus::rendererDrain,0x2e19ccu}};
        primitive.materialReference_.release299AC0();
        prepared_=false;cleanupPhase_=2;
        return {S::complete,0x2e1ad4u};
    }
    rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic_{};
    unsigned cleanupPhase_=0;
    bool attempted_=false,prepared_=false;
};

// Borrow of the live Session race28C and its actual signed scalar8C.
//1A9284..1A9300 writes this mode; zero is a genuine value.199980 copies
// the word to overlay40, later compared with1 at199E78. It is not a filename.
struct OriginalAIDamageRaceContext50LA {
    std::uint64_t sessionGeneration=0,raceKey=0;
    const std::int32_t* actualMode8C=nullptr;
};
class OriginalPodAIDamageOverlay50LA final {
public:
    OriginalWreckComponent50LA component;
    std::array<OriginalAIDamagePrimitiveArray50LA,2> arrays;
    OriginalAIDamagePrimitiveArray50LA* array30=nullptr;
    OriginalAIDamagePrimitiveArray50LA* array34=nullptr;
    OriginalWreckComponent50LA* racer38=nullptr;
    const OriginalAIDamageRaceContext50LA* race3C=nullptr;
    std::int32_t mode40=0;
    std::array<float,4> position50{{0.f,0.f,0.f,1.f}},position60{{0.f,0.f,0.f,1.f}};
    std::uint32_t enabled70=1,visible74=0;
    OriginalPodAIDamageOverlay50LA()=default;
    OriginalPodAIDamageOverlay50LA(const OriginalPodAIDamageOverlay50LA&)=delete;
    OriginalPodAIDamageOverlay50LA& operator=(const OriginalPodAIDamageOverlay50LA&)=delete;
    ~OriginalPodAIDamageOverlay50LA(){
        // The racer owner must finish the real ordered cleanup while its
        // renderer is alive. Never fall through to unsynchronized RAII release.
        if(component.reference00||array30||array34)std::terminate();
    }
    bool construct199840And199950(OriginalWreckComponent50LA& racer,
        const OriginalAIDamageRaceContext50LA& race,std::uint64_t componentKey,
        std::uint32_t& actualCounter423F60,OriginalAIDamageMaterialRow50LA& material,
        const char* actualDirectory36B970,std::size_t capacity)noexcept {
        if(attempted_||!racer.sessionGeneration||race.sessionGeneration!=racer.sessionGeneration||
           !race.raceKey||!race.actualMode8C||!componentKey)return false;
        attempted_=true;
        const auto r=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::overlay199840,racer.sessionGeneration,componentKey,true,actualCounter423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        racer38=&racer;race3C=&race;mode40=*race.actualMode8C;
        //1999A8 publishes each real primitive-array pointer before setup;
        // a later material failure retains that exact initialized prefix.
        array30=&arrays[0];
        if(!array30->construct199950(material,actualDirectory36B970,capacity))return false;
        array34=&arrays[1];
        if(!array34->construct199950(material,actualDirectory36B970,capacity))return false;
        arrays[0].primaryColor199A7C();prepared_=true;return true;
    }
    bool prepared()const noexcept {
        return prepared_&&racer38&&race3C&&race3C->sessionGeneration==component.sessionGeneration&&
            array30==&arrays[0]&&array34==&arrays[1]&&
            arrays[0].prepared()&&arrays[1].prepared();
    }
    std::size_t constructedArrays()const noexcept {
        return std::size_t(arrays[0].constructed199950())+std::size_t(arrays[1].constructed199950());
    }
private:
    friend class OriginalRacerEntityCold50LA;
    OriginalRacerEntityColdCleanupResult50LA releaseArrays1998A0(
        const OriginalRendererDispatchFields& fields,platform::VehicleWindowGl50DS& renderer,
        platform::VehicleWindowDiagnostic50DS& diagnostic)noexcept {
        using S=OriginalRacerEntityColdCleanupStatus50LA;
        //1998D8..1998FC: array30 completes and its owner slot is cleared
        // before array34 begins. Resume a blocked call at the retained prefix.
        if(array30){const auto r=array30->releaseCold2E1980(fields,renderer,diagnostic);
            if(!r.okay())return r;array30=nullptr;}
        if(array34){const auto r=array34->releaseCold2E1980(fields,renderer,diagnostic);
            if(!r.okay())return r;array34=nullptr;}
        prepared_=false;return {S::complete,0x199904u};
    }
    bool attempted_=false,prepared_=false;
};

struct OriginalRacerEntityColdBinding50LA {
    std::uint64_t sessionGeneration=0,racerKey=0,detached7F8Key=0,overlay240Key=0;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    OriginalEntityTypeRegistry50LA* actualTypes383408=nullptr;
    const OriginalAIDamageRaceContext50LA* actualRace28C=nullptr;
    OriginalAIDamageMaterialRow50LA* actualAIDamageMaterial=nullptr;
    const char* actualMaterialDirectory36B970=nullptr;
    std::size_t materialDirectoryCapacity=0;
    const OriginalRendererDispatchFields* renderer442F70=nullptr;
    platform::VehicleWindowGl50DS* actualRenderer=nullptr;
    platform::VehicleWindowDiagnostic50DS* rendererDiagnostic=nullptr;
};
// Canonical COMPONENT extension of an existing native Session racer. This
// owns represented27E130,7F8/240 and the separately reached arrays/7FC. It does not claim
// that new storage completed all20F390 fields, native vehicle physics, racer
// model constructors, outer manager ancestry, or199BC0/199AC0 runtime/draw.
class OriginalRacerEntityCold50LA final {
    friend class ::native_racer50HW::Cables;
    friend class ::native_racer_model50LA::Owner;
    friend class ::original_cable_selection50LA::Binding;
    // Same racer fields, not AIDamage primitive material00. Initial -1 stores
    //20F894/89C do not prove later213A40/213E90 strip construction finished.
    std::int32_t selectionMaterial244_=-1,selectionMaterial26C_=-1;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> selectionMaterialTable_;
    friend class OriginalRacerStripOwners50LA;
    OriginalRacerStripOwners50LA* lateStrips_=nullptr;
    std::optional<std::uint32_t> color260_; //Actual retained PHY215C08/215C98 producer.
    std::uint32_t mode3C_=0; //20F660; no ghost-mode writer currently joined.
    std::optional<std::uint32_t> selectionMarker38_;
    // Late handles are published by concrete retained primitive owners only.
public:
    OriginalWreckComponent50LA racer,detached7F8,particle7FC;
    // Source210C48/210F0C/211210/21150C/21193C publish the actual animation owners;
    //210C8C/210F50/211254/211550/211980 publish their SAME model04 after1538B0. Native
    // early resource staging lends those exact owners to this Session join.
    // Null here is the pre-publication native owner state, never a substitute
    // for a loaded model or either banked numeric engine scene index.
    const OriginalRacerAnimationCold50LA* animation1A4=nullptr;
    const OriginalRacerAnimationCold50LA* animation1A8=nullptr;
    const OriginalRacerAnimationCold50LA* animation1AC=nullptr;
    const OriginalRacerAnimationCold50LA* animation1B0=nullptr;
    const OriginalRacerAnimationCold50LA* animation1A0=nullptr;
    const OriginalRacerModelSceneOwner50LA* rightModel1B8=nullptr;
    const OriginalRacerModelSceneOwner50LA* leftModel1BC=nullptr;
    // POD02's optional lower pair owns distinct models, never upper aliases.
    const OriginalRacerModelSceneOwner50LA* bottomRightModel1C0=nullptr;
    const OriginalRacerModelSceneOwner50LA* bottomLeftModel1C4=nullptr;
    // Separate chariot branch21191C, with genuine engines already present.
    // The no-engine combined model194 branch is a different, unadmitted owner.
    const OriginalRacerModelSceneOwner50LA* chariotModel1B4=nullptr;
    OriginalPodAIDamageOverlay50LA overlay240;
    std::int32_t cableMaterialMode3C=0; //20F660; distinct from race mode8C.
    //20F94C's original default;211C4C/211CA8 may set2 from genuine anchors.
    std::uint32_t cableMultiplicity258=1;
    // Published only by the persistent Cables owner's actual214218 prefix.
    OriginalPodCableAllocation50LA* cables254=nullptr;
    std::uint16_t entityState30=0;
    std::uint8_t entityType32=0xffu,entityFlags33=0;
    OriginalRacerEntityCold50LA()=default;
    OriginalRacerEntityCold50LA(const OriginalRacerEntityCold50LA&)=delete;
    OriginalRacerEntityCold50LA& operator=(const OriginalRacerEntityCold50LA&)=delete;
    ~OriginalRacerEntityCold50LA(){if(!releaseColdPrefix().okay())std::terminate();}
    OriginalRacerEntityColdResult50LA constructBeforeParticleParent210A18(
        const OriginalRacerEntityColdBinding50LA& b)noexcept {
        using S=OriginalRacerEntityColdStatus50LA;
        if(attempted_||released_)return result(S::alreadyConstructed,0x27e130u);
        if(!b.sessionGeneration||!b.racerKey||!b.detached7F8Key||!b.overlay240Key||
           b.racerKey==b.detached7F8Key||b.racerKey==b.overlay240Key||b.detached7F8Key==b.overlay240Key||
           !b.actualObjectCount423F60||!b.actualAttachmentLock423F58||!b.actualTypes383408||
           !b.actualRace28C||!b.actualRace28C->raceKey||!b.actualRace28C->actualMode8C||
           b.actualRace28C->sessionGeneration!=b.sessionGeneration||!b.actualAIDamageMaterial||
           !b.renderer442F70||!b.actualRenderer||!b.rendererDiagnostic||
           !b.actualAIDamageMaterial->acceptsDirectory(b.actualMaterialDirectory36B970,b.materialDirectoryCapacity))
            return result(S::binding,0x210878u);
        attempted_=true;binding_=b;
        selectionMaterial244_=-1;selectionMaterial26C_=-1;mode3C_=0; //20F894/89C and20F660
        selectionMaterialTable_=b.actualAIDamageMaterial->table382B38();
        auto r=initializeOriginalWreckComponent50LA(racer,OriginalWreckComponentKind50LA::racer27E130,
            b.sessionGeneration,b.racerKey,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return result(S::component,r.sourcePC);
        ++count_;
        //27E154..1B8 occur after141300, before the application's lazy type
        // registry lookup. A failed lookup retains that exact base prefix.
        entityState30=0;entityType32=0xffu;entityFlags33=std::uint8_t(entityFlags33&~7u);
        const auto type=b.actualTypes383408->searchableGameObject27E1BC();
        if(!type.okay())return result(S::entityType,type.sourcePC);
        entityType32=type.type;
        entityFlags33=std::uint8_t((entityFlags33&~7u)|1u); //27E1F8/214..240
        // The enclosing racer20F390 constructor then sets selected bit1 at
        //210414..210430, before the represented210770/210878 cold prefix.
        // This is an actual racer write, not model191, clipping, a draw receipt,
        // or permission to skip229AE0 before the scanner reads this bit.
        entityFlags33=std::uint8_t((entityFlags33&~2u)|2u);
        r=initializeOriginalWreckComponent50LA(detached7F8,OriginalWreckComponentKind50LA::entity141300,
            b.sessionGeneration,b.detached7F8Key,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return result(S::component,r.sourcePC);
        ++count_;
        if(!overlay240.construct199840And199950(racer,*b.actualRace28C,b.overlay240Key,
           *b.actualObjectCount423F60,*b.actualAIDamageMaterial,b.actualMaterialDirectory36B970,
           b.materialDirectoryCapacity))return result(S::material,0x210878u);
        pointers_={{&racer,&detached7F8,&overlay240.component,nullptr}};
        domain_={b.sessionGeneration,pointers_.data(),3u};
        r=appendOriginalWreckComponent50LA(domain_,racer,overlay240.component,0x0cu,*b.actualAttachmentLock423F58);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return result(S::attachment,0x210888u);
        prepared_=true;return result(S::complete,0x210a18u);
    }
    // Native continuation of this SAME racer owner. The caller lends the real
    // track4A0 provider and actual330/334 domain; missing providers are never
    // converted into source-null branches. This admitted caller has launch1,
    // so2109D8 does not consume the unrelated racer3C special-mode field.
    OriginalRacerEntityColdResult50LA constructPostOverlay210890(
        OriginalGcParticleManager50KG& actualParticles4A0,
        const UndertrackPhysicsDomain50EW& actualHeights,
        const std::uint32_t& actualLaunch11258,std::uint64_t particleKey)noexcept {
        using S=OriginalRacerEntityColdStatus50LA;
        if(postOverlayAttempted_||released_)return result(S::alreadyConstructed,0x210890u);
        if(!atParticleParentColdCut210A18()||actualLaunch11258!=1u||
           !actualParticles4A0.prepared()||!particleKey||particleKey==racer.objectKey||
           particleKey==detached7F8.objectKey||particleKey==overlay240.component.objectKey||
           actualHeights.water334==UndertrackHeightOwner50EW::Unbound||
           (actualHeights.water334==UndertrackHeightOwner50EW::Present&&
               (!actualHeights.waterProvider334||!actualHeights.waterGlobals))||
           (actualHeights.water334==UndertrackHeightOwner50EW::Absent&&actualHeights.waterProvider334))
            return result(S::binding,0x210890u);
        postOverlayAttempted_=true;particleKey_=particleKey;particles4A0_=&actualParticles4A0;
        // Source pointer arrays are separate owned allocations; their actual
        // four/four/two/(two)/four/ten NULL entries are not Entity constructors.
        // Native pointer widths replace source32-bit pointers without emulation.
        try {
            slots800_=std::make_unique<Slots4>();
            slots804_=std::make_unique<Slots4>();
            slots808_=std::make_unique<Slots2>();
            if(actualHeights.water334==UndertrackHeightOwner50EW::Present)
                slots81C_=std::make_unique<Slots2>();
            slots80C_=std::make_unique<Slots4>();
            slots818_=std::make_unique<Slots10>();
        }catch(...){return result(S::allocation,0x210890u);}
        // Nonnull actual4A0 and launch1 take the real2109F0/210A04 branch.
        const auto initialized=initializeOriginalWreckComponent50LA(particle7FC,
            OriginalWreckComponentKind50LA::entity141300,binding_.sessionGeneration,
            particleKey,true,*binding_.actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)
            return result(S::component,initialized.sourcePC);
        pointers_[3]=&particle7FC;domain_.count=4;
        const auto attached=appendOriginalWreckComponent50LA(domain_,racer,particle7FC,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)
            return result(S::attachment,0x210a18u);
        postOverlayConstructed_=true;return result(S::complete,0x210a20u);
    }
    bool postOverlayConstructed210A20()const noexcept {
        return !cleanupPhase_&&!released_&&postOverlayConstructed_;
    }
    bool prepared()const noexcept{return !cleanupPhase_&&!released_&&prepared_&&overlay240.prepared();}
    bool atParticleParentColdCut210A18()const noexcept {
        return prepared()&&!racer.parent1C&&!racer.previous10&&!racer.next14&&
            racer.firstChild18==&overlay240.component&&overlay240.component.parent1C==&racer&&
            !overlay240.component.previous10&&!overlay240.component.next14&&overlay240.component.mask20==0x0cu&&
            !detached7F8.parent1C&&!detached7F8.previous10&&!detached7F8.next14;
    }
    OriginalWreckComponentDomain50LA& domain()noexcept{return domain_;}
    std::uint32_t* actualCounter423F60()const noexcept{return binding_.actualObjectCount423F60;}
    const std::uint32_t* actualLock423F58()const noexcept{return binding_.actualAttachmentLock423F58;}
    bool released()const noexcept{return released_;}
    // Native ownership of ONLY the constructed cold prefix. Other concrete
    // children must first close through their own owners. No whole20F390 /
    //2104D0 racer destructor, scene unlink or external ancestor is invented.
    // The Session must call this before closing the borrowed renderer. A
    // renderer failure retains the current stage for a safe explicit retry.
    OriginalRacerEntityColdCleanupResult50LA releaseColdPrefix()noexcept {
        using S=OriginalRacerEntityColdCleanupStatus50LA;
        if(released_)return {S::complete,0x210730u};
        if(!attempted_){released_=true;return {S::complete,0};}
        // The concrete partial cable owner must close first. Never erase its
        // live254 alias while releasing the containing racer.
        if(cables254||lateStrips_)return {S::topology,0x2104d0u};
        if(!binding_.actualObjectCount423F60||!binding_.actualAttachmentLock423F58)
            return {S::binding,0x1425a4u};
        if(*binding_.actualAttachmentLock423F58)return {S::attachmentLocked,0x1425a8u};
        auto& counter=*binding_.actualObjectCount423F60;
        auto& overlay=overlay240.component;
        const bool hasOverlay=overlay.kind!=OriginalWreckComponentKind50LA::unbound;
        if(!cleanupPhase_){
            if(!coldCleanupTopology())return {S::topology,0x142590u};
            //142590 walks the actual last child first. The represented7FC
            // must still be empty; future concrete effects close before here.
            if(particle7FC.kind!=OriginalWreckComponentKind50LA::unbound){
                if(particle7FC.parent1C)overlay.next14=particle7FC.next14;
                --particle7FC.reference00;--counter;
            }
            //14262C..64C repairs the parent's head before104970 releases
            // the overlay's actual64-bit reference. Its own stale parent1C
            // and mask fields are not a fabricated141A70 detach/reset.
            if(hasOverlay){
                if(overlay.parent1C)racer.firstChild18=overlay.next14;
                --overlay.reference00;
            }
            cleanupPhase_=1;prepared_=false;
        }
        if(cleanupPhase_==1u){
            if(hasOverlay){
                if(!binding_.renderer442F70||!binding_.actualRenderer||!binding_.rendererDiagnostic)
                    return {S::binding,0x2e19a8u};
                const auto r=overlay240.releaseArrays1998A0(*binding_.renderer442F70,
                    *binding_.actualRenderer,*binding_.rendererDiagnostic);
                if(!r.okay())return r;
                //199904->1413E0 after both arrays, exactly once.
                --counter;
            }
            cleanupPhase_=2;
        }
        //142654->104970 now releases the racer; its represented2104D0
        // prefix reaches detached7F8's base142590/104970 before its own
        //1413E0. Fresh references are exactly1, checked before any mutation.
        //210538..5AC: actual array storage retires in this order, before
        // detached7F8. unique_ptr clearing is host lifetime state, not a claim
        // that source2104D0 writes null back into its old owner slots.
        if(count_){
            --racer.reference00; //104970 enters the represented2104D0 destructor.
            slots800_.reset();slots804_.reset();slots808_.reset();slots80C_.reset();
            slots818_.reset();slots81C_.reset();
            if(count_==2u){--detached7F8.reference00;--counter;}
            --counter;
        }
        cleanupPhase_=3;released_=true;
        return {S::complete,0x210730u};
    }
private:
    OriginalRacerEntityColdBinding50LA binding_{};
    using Slots2=std::array<OriginalWreckComponent50LA*,2>;
    using Slots4=std::array<OriginalWreckComponent50LA*,4>;
    using Slots10=std::array<OriginalWreckComponent50LA*,10>;
    std::unique_ptr<Slots4> slots800_,slots804_,slots80C_;
    std::unique_ptr<Slots2> slots808_,slots81C_;
    std::unique_ptr<Slots10> slots818_;
    OriginalGcParticleManager50KG* particles4A0_=nullptr;
    std::uint64_t particleKey_=0;
    bool postOverlayAttempted_=false,postOverlayConstructed_=false;
    std::array<OriginalWreckComponent50LA*,4> pointers_{};
    OriginalWreckComponentDomain50LA domain_{};
    std::size_t count_=0;
    unsigned cleanupPhase_=0;
    bool attempted_=false,prepared_=false,released_=false;
    bool coldCleanupTopology()const noexcept {
        using K=OriginalWreckComponentKind50LA;
        const auto& overlay=overlay240.component;
        const bool hasOverlay=overlay.kind!=K::unbound;
        const auto owned=[&](const OriginalWreckComponent50LA& c,K kind,
            std::uint64_t key,bool constructed){
            if(!constructed)return c.kind==K::unbound&&!c.reference00&&!c.objectKey&&
                !c.previous10&&!c.next14&&!c.firstChild18&&!c.parent1C;
            return c.kind==kind&&c.reference00==1u&&c.objectKey==key&&
                c.sessionGeneration==binding_.sessionGeneration;
        };
        if(count_>2u||(hasOverlay&&count_!=2u)||
           !owned(racer,K::racer27E130,binding_.racerKey,count_!=0u)||
           !owned(detached7F8,K::entity141300,binding_.detached7F8Key,count_==2u)||
           !owned(overlay,K::overlay199840,binding_.overlay240Key,hasOverlay)||
           racer.parent1C||racer.previous10||racer.next14||
           detached7F8.parent1C||detached7F8.previous10||detached7F8.next14||detached7F8.firstChild18||
           overlay.previous10||overlay.firstChild18||
           (overlay240.array30&&overlay240.array30!=&overlay240.arrays[0])||
           (overlay240.array34&&overlay240.array34!=&overlay240.arrays[1]))return false;
        const bool hasParticle=particle7FC.kind!=K::unbound;
        if(!owned(particle7FC,K::entity141300,particleKey_,hasParticle)||
           particle7FC.firstChild18||particle7FC.next14)return false;
        if(hasParticle){
            if(!hasOverlay||!particles4A0_)return false;
            const bool linked=particle7FC.parent1C==&racer&&
                particle7FC.previous10==&overlay&&overlay.next14==&particle7FC&&particle7FC.mask20==0x0402u;
            const bool unattached=!particle7FC.parent1C&&!particle7FC.previous10&&
                !particle7FC.mask20&&!overlay.next14;
            if(!linked&&!unattached)return false;
        }else if(overlay.next14)return false;
        if(!hasOverlay)return !racer.firstChild18&&!overlay240.array30&&!overlay240.array34;
        return (overlay.parent1C==&racer&&racer.firstChild18==&overlay&&overlay.mask20==0x0cu)||
            (!overlay.parent1C&&!racer.firstChild18&&!overlay.mask20);
    }
    OriginalRacerEntityColdResult50LA result(OriginalRacerEntityColdStatus50LA s,std::uint32_t pc)const noexcept {
        const auto overlayCount=overlay240.component.kind!=OriginalWreckComponentKind50LA::unbound?1u:0u;
        const auto particleCount=particle7FC.kind!=OriginalWreckComponentKind50LA::unbound?1u:0u;
        return {s,pc,count_+overlayCount+particleCount,overlay240.constructedArrays()};
    }
};
} // namespace rr::game
