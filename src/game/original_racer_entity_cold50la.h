#pragma once
#include "original_racer_world_borrow50la.h"
#include "original_application_entities50la.h"
#include "chariot_ik49t.h"
#include "original_camera_start_native.h"
#include "original_gc_particle_manager50kg.h"
#include "undertrack_reset50ew.h"
#include "../assets/original_standard_screen_material_reference50la.h"
#include "../assets/pod_physics49n.h"
#include "../assets/native_phy_load50la.h"
#include <string_view>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace ra_source_camera50LA { class OwnershipBridge; }
namespace native_racer50HW { class Cables; }
namespace native_racer_model50LA {class Owner;}
namespace rr::game {
class OriginalPlayerConstructorPrefix50LA;
class OriginalPlayerOverlayRendererLease50LA;
class OriginalRaceConstructor50LA;
class OriginalPodSphereOwner50LA;
class OriginalWreckCollectionStorage50LA;
struct OriginalWreckTerrainProvider50LA;
class OriginalRacerMainBodyOwner50LA;
class OriginalRacerGravityOwner50LA;
class OriginalRacerTerrainQueryOwner50LA;
class OriginalRacerEngine34COwner50LA;
class OriginalRacerAero350Owner50LA;
class OriginalRacerQuery690Owner50LA;
class OriginalRacerQuery694Owner50LA;
class OriginalRacerContact354Owner50LA;
class OriginalRacerDetector358Owner50LA;
class OriginalRacerQuery7A4Owner50LA;
class OriginalRacerQuery79COwner50LA;
class OriginalRacerQuery364Owner50LA;
class OriginalRacerContact368Owner50LA;
class OriginalRacerDetector36COwner50LA;
class OriginalRacerSecondaryBodyOwner50LA;
class OriginalRacerStripOwners50LA;
class OriginalRacerModelSceneOwner50LA;
struct OriginalRacerSceneObject50LA;
class OriginalRacerAnimationCold50LA;
// Same independently allocated source0x60 record, staged by the chariot
// before Session exists, then owned here by the actual racer208/20C lifetime.
// Plain destruction does not inspect or release any borrowed scene identity.
struct OriginalRacerWristAllocation50LA final {
    ChariotIkChain49T record;
    std::array<std::byte,0x60-sizeof(ChariotIkChain49T)> nativePadding;
};
static_assert(sizeof(OriginalRacerWristAllocation50LA)==0x60);
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
    friend class OriginalPlayerConstructorPrefix50LA;
    friend class OriginalPlayerOverlayRendererLease50LA;
    // Distinct nullable source0x50 allocations; inner vertex/packet storage
    // keeps the existing explicitly native embedded adaptation.
    std::array<std::unique_ptr<OriginalAIDamagePrimitiveArray50LA>,2> sourceArrays_;
    bool sourceArraysAttempted_=false;
    bool constructorAttempted_=false,sourceMode_=false,sourceMode40Known_=false;
    const OriginalRaceConstructor50LA* sourceRace3C_=nullptr;
    std::uint32_t sourceDispatch08_=0;
    bool construct199840(std::uint64_t generation,std::uint64_t componentKey,
        std::uint32_t& actualCounter423F60)noexcept {
        if(constructorAttempted_||component.reference00)return false;
        constructorAttempted_=true;
        const auto r=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::overlay199840,generation,componentKey,true,actualCounter423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return false;
        sourceDispatch08_=0x3ce620;
        position50={{0.f,0.f,0.f,1.f}};position60={{0.f,0.f,0.f,1.f}};
        racer38=nullptr;race3C=nullptr;sourceRace3C_=nullptr;
        enabled70=1;visible74=0;array30=nullptr;array34=nullptr;
        return true; //199898 return:40/44..4F/78..7F not source-written here.
    }
    bool bindSource199950(OriginalWreckComponent50LA& racer,
        const OriginalRaceConstructor50LA& race)noexcept {
        if(!constructorAttempted_||!component.reference00||attempted_||sourceMode_||array30||array34)return false;
        sourceMode_=true;sourceRace3C_=&race;racer38=&racer;
        //141C20 identity-return then199978/19997C.40 remains unknown until
        //the actual subsequent Race8C read and19998C store. No native
        //Session RaceContext facade and no primitive/material acquisition.
        return true;
    }
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
        if(!construct199840(racer.sessionGeneration,componentKey,actualCounter423F60))return false;
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
            if(!r.okay())return r;array30=nullptr;
            if(sourceMode_)sourceArrays_[0].reset();}
        if(array34){const auto r=array34->releaseCold2E1980(fields,renderer,diagnostic);
            if(!r.okay())return r;array34=nullptr;
            if(sourceMode_)sourceArrays_[1].reset();}
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
    //Default keeps the old explicit native adapter. Source mode never derives
    //World330/334 from height authoring or defaults; absence stops2107FC.
    bool sourceConstructorOnly=false;
    const OriginalRacerWorldBorrow50LA* actualWorld2107FC=nullptr;
    //Actual210770 incoming t3/stack0, retained before constructor entry.
    //Not pieceB4/B8 stores and not host zone indices.
    const void* actualIncomingVisibility4C0=nullptr;
    const void* actualIncomingCollision4C4=nullptr;
    const std::array<std::uint32_t,4>* actualAnchorOrigin434630=nullptr; //Same App, source-only300 stores.
    const std::uint32_t* actualLoadPod60=nullptr; //Same native210770 argument, not authored filename.
};
// Canonical COMPONENT extension of an existing native Session racer. This
// owns represented27E130,7F8/240 and the separately reached arrays/7FC. It does not claim
// that new storage completed all20F390 fields, native vehicle physics, racer
// model constructors, outer manager ancestry, or199BC0/199AC0 runtime/draw.
class OriginalRacerEntityCold50LA final {
    friend class ::ra_cold_roster50LA::WreckBacking;
    friend class OriginalRacerQuery364Owner50LA;
    friend class OriginalRacerContact368Owner50LA;
    // Native source-load aliases. Unknown until the exact same-world producer;
    // cold storage never publishes its planned members here.
    bool nativeWreckLoadAttempted_=false,nativeWreckWithdrawn_=false;
    //Bound forward by the real native load caller to the pre-existing
    //Operation occurrence. Never constructed here or inferred at close.
    rr::assets::NativePhyLoad50LA* nativePhyLoad_=nullptr;
    std::optional<OriginalWreckComponent50LA*> sourceRacerExplosion814_,sourceRacerSparks810_; //Unknown until212B78/212BEC.
    std::optional<OriginalWreckComponent50LA*> sourceParticle7FC_; //Unknown before actual210A0C.
    std::optional<OriginalRacerTerrainQueryOwner50LA*> sourceTerrain348_;
    std::optional<OriginalRacerEngine34COwner50LA*> sourceEngine34C_; //Unknown until2120D0.
    std::optional<OriginalRacerAero350Owner50LA*> sourceAero350_;
    std::optional<OriginalRacerQuery690Owner50LA*> sourceQuery690_;
    std::optional<OriginalRacerQuery694Owner50LA*> sourceQuery694_;
    std::optional<OriginalRacerContact354Owner50LA*> sourceContact354_; //Unknown until212294.
    std::optional<OriginalRacerDetector358Owner50LA*> sourceDetector358_; //Unknown until212364.
    std::optional<OriginalRacerQuery7A4Owner50LA*> sourceQuery7A4_; //Unknown until2123E4.
    std::optional<OriginalRacerQuery79COwner50LA*> sourceQuery79C_; //Unknown until212484.
    std::optional<OriginalRacerQuery364Owner50LA*> sourceQuery364_; //Unknown until212750.
    std::optional<OriginalRacerContact368Owner50LA*> sourceContact368_; //Unknown until2127E4.
    std::optional<OriginalRacerDetector36COwner50LA*> sourceDetector36C_; //Unknown until2128B0.
    std::optional<OriginalRacerSceneObject50LA*> sourceScene19C_; //Unknown until212504.
    std::optional<OriginalRacerSecondaryBodyOwner50LA*> sourceBody35C_; //Unknown until212554.
    std::optional<std::array<std::uint32_t,4>> sourceAnchor300_; //Actual source-owned vector, not authored geometry.
    std::optional<std::uint32_t> sourceCount384_;
    std::optional<const void*> sourceCandidate388_; //Unknown first inline-array word/address anchor only; no extent.
    std::optional<std::uint32_t> sourcePod60_; //Actual21081C store.
    //Retained source stack1E0 formatter output, not a new Racer field.
    std::optional<std::array<char,6>> sourcePodName1E0_;
    std::optional<const void*> sourceList488_; //20FA44 NULL, later producer not joined.
    std::optional<std::uint32_t> sourceCount48C_; //Unknown: address borrow is not a count read.
    std::optional<const OriginalWreckTerrainProvider50LA*> sourceTerrain330_;
    std::optional<const WaterHeightProvider50JC*> sourceWater334_;
    std::optional<std::uint32_t> sourceWreckCount370_;
    OriginalWreckCollectionStorage50LA* const* sourceWreck374_=nullptr;
    bool sourceWreck374Published_=false;
    std::size_t sourceWreck374Slots_=0;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    bool readReplayMode3C229F94(std::uint32_t& value)const noexcept {
        if(!prepared()||racer.kind!=OriginalWreckComponentKind50LA::racer27E130||
           racer.sessionGeneration!=binding_.sessionGeneration||racer.objectKey!=binding_.racerKey)
            return false;
        value=mode3C_;return true; //Actual20F660-owned field, never a supplied ghost flag.
    }
    // Source20F3D4 concrete dispatch identity, not a synthetic runtime vtable.
    bool nativeDispatch20F390Known_=false,nativePostEmberAttempted_=false;
    std::uint32_t nativePostEmberPC_=0x212dc8u;
    //212ED0 inventory belongs to this SAME racer. B54 is unknown until212EDC;
    //B58 becomes known NULL only at2100BC, then the actual212F50 allocation
    //store publishes its native pointer-width buffer. Unwritten entries are
    //not initialized or exposed as source NULLs. The extent is host custody
    //metadata, separate from both B54 and the retained source stack2C cursor.
    std::optional<std::int32_t> sourceSpinCountB54_;
    std::optional<const OriginalRacerSceneObject50LA* const*> sourceSpinB58_;
    std::unique_ptr<const OriginalRacerSceneObject50LA*[]> spinAllocationB58_;
    std::size_t nativeSpinB58Capacity_=0,nativeSpinB58PublishedRows_=0;
    // Native fixed-backing adoption for the reached212E18 tail. These are
    // NOT guest CC0/D34 pointer words or malloc/calloc results. Bank numeric
    // addresses denote this racer ONLY during the same active slot visit.
    // No consumer may use these descriptors without reauthenticating that
    // Bank/generation/slot; all live consumers retain the existing numeric lanes.
    struct NativeOuterStorage212E18 final {
        bool attempted=false,sourcePhyClose1573B0Unrepresented=false,nativePhyClosed=false;
        OriginalRacerEntityCold50LA* nativeReturnRacer=nullptr; //Typed native return only.
        std::uint32_t pc=0x212e18u;
        const void* bank=nullptr;
        std::uint64_t generation=0;
        std::size_t slot=0;
        float* lapTimesCC0=nullptr; //Exactly the existing100-float timing array.
        struct ImpactD34 final {
            std::uint32_t* partnerId=nullptr;
            std::uint32_t* partnerState=nullptr;
            float* timer08=nullptr;
            float* amount0C=nullptr;
            float* duration10=nullptr;
        } impact;
        std::uint8_t impactStores=0;
        std::uint32_t refreshedNodes=0;
    } nativeOuterStorage_;
    // Only this RA Water-NONE route lacks an existing BE0 owner. The four
    // actual28B2B0 output words remain unknown until212E44. GC/MC Water paths
    // must borrow their own previousBE0_ instead of using this RA-only lane.
    std::optional<std::array<std::uint32_t,4>> nativePreviousBE0_;
    std::optional<float> engineMotionAB0_; //unknown before actual21003C
    // Missing native canonical lanes. Each becomes known only at the actual
    // base20F390 producer below; native class58 never upgrades this dispatch.
    std::optional<const void*> sourceAudioD40_;
    // Native pointer-width D48 alias. Unknown until actual20F63C; later only
    // the authentic same-App speech constructor may publish its exact Entity.
    std::optional<OriginalWreckComponent50LA*> nativeSpeechD48_;
    std::optional<float> explosiveWreckSpeed50_,resetSpeed54_,powerslideYawA90_;
    std::optional<float> binderMeshHalfHeight25C_;
    std::optional<std::array<float,4>> cameraOffsetCE0_;
    // Canonical source lanes. Legacy native handling remains a separate
    // projection, never proof that these later source stores occurred.
    std::optional<float> sourceGravity378_,sourceAirborneGravity37C_,sourceLift7EC_;
    //Unknown until actual690/1202E0 producer, not a cold planned bounds receipt.
    std::optional<std::array<float,3>> sourceCollisionCenter310_,sourceCollisionHalf320_;
    static bool sourceFloat158A70(const std::vector<std::uint8_t>& phy,
        std::string_view section,std::string_view key,std::optional<float>& written)noexcept{
        if(phy.empty()||phy.size()>rr::assets::maxPodPhysicsBytes49N)return false;
        auto trim=[](std::string_view text){
            while(!text.empty()&&(text.front()==' '||text.front()=='\t'||text.front()=='\r'))text.remove_prefix(1);
            while(!text.empty()&&(text.back()==' '||text.back()=='\t'||text.back()=='\r'))text.remove_suffix(1);
            return text;
        };
        auto equal=[](std::string_view a,std::string_view b){
            if(a.size()!=b.size())return false;
            auto lower=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
            for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;return true;
        };
        const std::string_view text(reinterpret_cast<const char*>(phy.data()),phy.size());
        bool selected=false,foundSection=false;
        for(std::size_t at=0;at<text.size();){
            const auto start=at;auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();at=end+1;
            if(end-start>rr::assets::maxPodPhysicsLine49N)return false;
            auto line=trim(text.substr(start,end-start));
            for(const auto c:line)if((static_cast<unsigned char>(c)<32&&c!='\t')||static_cast<unsigned char>(c)>126)return false;
            if(line.empty()||line.front()==';')continue;
            if(line.front()=='['){
                if(line.size()<3||line.back()!=']')return false;
                // Source first-selected block/first-present key. No authored
                // fallback values or translation of unrelated key aliases.
                if(selected)return true;
                selected=!foundSection&&equal(trim(line.substr(1,line.size()-2)),section);
                foundSection|=selected;continue;
            }
            if(!selected)continue;const auto split=line.find('=');
            if(split==std::string_view::npos||!equal(trim(line.substr(0,split)),key))continue;
            const auto token=trim(line.substr(split+1));
            if(token.empty())return false; //Empty-token forms are outside this native lexical admission.
            float value=0;
            if(!rr::assets::parsePodPhysicsNumber49N(token.data(),token.size(),value))return false;
            written=value;return true;
        }
        return true; //Missing section/key skips its store, preserving the actual destination.
    }
    static bool engineMotionSource158A70(const std::vector<std::uint8_t>& phy,
        std::optional<float>& written)noexcept {
        return sourceFloat158A70(phy,"Pod Setup","EngineMotion",written);
    }
    bool beginNativeHandling2146(rr::assets::NativePhyLoad50LA::Reader& phy,std::uint32_t& pc)noexcept{
        pc=nativePostEmberPC_;
        if(!phy||!nativePhyLoad_||phy.owner()!=nativePhyLoad_||
           nativePostEmberAttempted_||nativeWreckWithdrawn_||!nativeWreckLoadAttempted_||
           !prepared()||!postOverlayConstructed210A20()||nativeSource818Published_!=10u||
           !nativeDispatch20F390Known_||!engineMotionAB0_||mode3C_!=0u||
           racer.kind!=OriginalWreckComponentKind50LA::racer27E130||
           racer.sessionGeneration!=binding_.sessionGeneration||racer.objectKey!=binding_.racerKey)return false;
        nativePostEmberAttempted_=true;
        pc=nativePostEmberPC_=0x212ddcu;
        //20F3D4 installs3D1790; actual+A0=212EC0 is JR RA/NOP. This
        //concrete owner proves that call, not a generic virtual no-op policy.
        pc=nativePostEmberPC_=0x2146f4u;bool present=false;
        //The earlier bulk/inventory phase is native-adapted, not historical
        //Config selection. Require this actual first shared selection to
        //succeed; never invent an unknown pre-2146 selected block on a miss.
        auto& reader=phy.cursor();
        if(!reader.select157D00("Pod Setup",present)||!present)return false;
        pc=nativePostEmberPC_=0x214708u;
        std::optional<float> value;
        if(!reader.float158A70("EngineMotion",value))return false;
        if(value)engineMotionAB0_=*value; //actual158AAC reached from214708
        pc=nativePostEmberPC_=0x214710u;
        //The separate physical-properties/source340 producer split remains
        //required. No reapplication/adoption of earlier cold49N writes here.
        return true;
    }
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
    const OriginalRacerMainBodyOwner50LA* body340=nullptr; //Borrowed actual retained body owner.
    const OriginalRacerGravityOwner50LA* gravity344=nullptr; //Borrowed actual retained gravity owner.
    const OriginalPodSphereOwner50LA* joint360=nullptr; //Borrowed actual retained joint owner.
    // Borrowed source aliases of the distinct retained2893A0 allocations.
    const OriginalRacerSceneObject50LA* scene190=nullptr;
    const OriginalRacerSceneObject50LA* scene198=nullptr;
    const OriginalRacerSceneObject50LA* scene194=nullptr;
    // Borrowed independently allocated native28EB90 records; no Entity retain.
    const ChariotIkChain49T* wrist208=nullptr;
    const ChariotIkChain49T* wrist20C=nullptr;
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
        if(b.sourceConstructorOnly){
            const void* visibility=nullptr;const void* collision=nullptr;
            if(!b.actualWorld2107FC||!b.actualWorld2107FC->readIncoming210770(b.sessionGeneration,
                b.actualObjectCount423F60,visibility,collision)||
               visibility!=b.actualIncomingVisibility4C0||collision!=b.actualIncomingCollision4C4)
                return result(S::binding,0x210770u);
        }
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
        nativeDispatch20F390Known_=true; //actual20F3D4, after base27E130 returned
        if(b.sourceConstructorOnly)
            sourceAnchor300_=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}}; //20F440 VF0.
        cameraOffsetCE0_=std::array<float,4>{{0.f,0.f,0.f,1.f}}; //20F5D4 SQ vf0.
        sourceAudioD40_=nullptr; //20F62C, BEFORE the native base+A0 no-op212EC0.
        nativeSpeechD48_=nullptr; //20F63C. Not a constructor/load receipt for D48.
        explosiveWreckSpeed50_=0.f;resetSpeed54_=0.f; //20F6C4/20F6CC.
        binderMeshHalfHeight25C_=2.25f; //20F944, s6=bits40100000.
        if(b.sourceConstructorOnly){
            if(!b.actualAnchorOrigin434630)return result(S::binding,0x20f990u);
            sourceAnchor300_=*b.actualAnchorOrigin434630; //20F99C, before query690 NULL.
        }
        //Original20FA24/2C precede20FAD4, which precedes21003C.
        float gravityUnit;const std::uint32_t gravityWord=0x4200cccdu;
        std::memcpy(&gravityUnit,&gravityWord,4);
        sourceQuery690_=nullptr; //20F9E4, before actual348/350 stores.
        sourceQuery694_=nullptr; //20F9EC, before actual348/350 stores.
        sourceTerrain348_=nullptr; //20FA04, before gravity20FA24/2C.
        sourceAero350_=nullptr; //20FA0C, not inferred absent ownership.
        sourceGravity378_=gravityUnit;sourceAirborneGravity37C_=gravityUnit;
        sourceCount384_=0u; //Actual20FA3C.388 remains unknown, not a fabricated array.
        sourceList488_=nullptr; //20FA44;48C is not written by this prefix.
        sourceLift7EC_=10.f;
        powerslideYawA90_=0.f; //20FF48, never a lifecycle-derived default.
        engineMotionAB0_=0.f; //actual21003C, before selected-bit210414 store
        sourceSpinB58_=nullptr; //Actual2100BC. There is no constructor B54 store.
        // The enclosing racer20F390 constructor then sets selected bit1 at
        //210414..210430, before the represented210770/210878 cold prefix.
        // This is an actual racer write, not model191, clipping, a draw receipt,
        // or permission to skip229AE0 before the scanner reads this bit.
        entityFlags33=std::uint8_t((entityFlags33&~2u)|2u);
        r=initializeOriginalWreckComponent50LA(detached7F8,OriginalWreckComponentKind50LA::entity141300,
            b.sessionGeneration,b.detached7F8Key,true,*b.actualObjectCount423F60);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return result(S::component,r.sourcePC);
        ++count_;
        if(b.sourceConstructorOnly){
            const void* terrain=nullptr;const void* water=nullptr;
            if(!b.actualWorld2107FC||!b.actualWorld2107FC->read2107FC(b.sessionGeneration,
                b.actualObjectCount423F60,terrain,water))return result(S::binding,0x2107fcu);
            sourceTerrain330_.emplace(static_cast<const OriginalWreckTerrainProvider50LA*>(terrain)); //2107FC.
            sourceWater334_.emplace(static_cast<const WaterHeightProvider50JC*>(water)); //210804.
            if(!b.actualLoadPod60)return result(S::binding,0x21081cu);
            sourcePod60_=*b.actualLoadPod60; //Actual argument copy, before210824 formatter.
            //Bounded native308C90 formatting for the admitted Pod01..Pod99
            //slice. The actual21081C store survives an unsupported value.
            if(*sourcePod60_==0u||*sourcePod60_>=100u)return result(S::binding,0x210824u);
            sourcePodName1E0_=std::array<char,6>{{'P','o','d',
                static_cast<char>('0'+*sourcePod60_/10u),static_cast<char>('0'+*sourcePod60_%10u),'\0'}};

        }
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
    // converted into source-null branches.2109D8 only suppresses the parent
    // for launch3 with a nonzero actual racer3C ghost mode. Fresh ordinary
    // racers retain their source20F660 mode3C=0 for all three event types.
    OriginalRacerEntityColdResult50LA constructPostOverlay210890(
        OriginalGcParticleManager50KG& actualParticles4A0,
        const UndertrackPhysicsDomain50EW& actualHeights,
        const std::uint32_t& actualLaunch11258,std::uint64_t particleKey)noexcept {
        using S=OriginalRacerEntityColdStatus50LA;
        if(postOverlayAttempted_||released_)return result(S::alreadyConstructed,0x210890u);
        if(!atParticleParentColdCut210A18()||actualLaunch11258<1u||actualLaunch11258>3u||
           (actualLaunch11258==3u&&mode3C_!=0u)||
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
        // Nonnull actual4A0 and the actual non-ghost branch reach2109F0/210A04.
        const auto initialized=initializeOriginalWreckComponent50LA(particle7FC,
            OriginalWreckComponentKind50LA::entity141300,binding_.sessionGeneration,
            particleKey,true,*binding_.actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)
            return result(S::component,initialized.sourcePC);
        sourceParticle7FC_=&particle7FC; //210A0C, AFTER141300 and BEFORE141960.
        pointers_[3]=&particle7FC;domain_.count=4;
        const auto attached=appendOriginalWreckComponent50LA(domain_,racer,particle7FC,
            0x0402u,*binding_.actualAttachmentLock423F58);
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)
            return result(S::attachment,0x210a18u);
        postOverlayConstructed_=true;return result(S::complete,0x210a20u);
    }
    bool postOverlayConstructed210A20()const noexcept {
        return !cleanupPhase_&&!released_&&postOverlayConstructed_&&sourceParticle7FC_&&
            *sourceParticle7FC_==&particle7FC&&particle7FC.reference00==1u;
    }
    bool prepared()const noexcept{return !cleanupPhase_&&!released_&&prepared_&&overlay240.prepared()&&
        (!binding_.sourceConstructorOnly||(binding_.actualWorld2107FC&&binding_.actualWorld2107FC->current()));}
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
        if(nativeSpeechD48_&&*nativeSpeechD48_)return {S::topology,0x2c10b0u};
        if(nativeWreckLoadAttempted_&&!nativeWreckWithdrawn_)return {S::topology,0x2129c0u};

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
            //2104FC..21050C precedes CC0/800 cleanup.2E8AE0 frees the
            //plain B58 allocation only; it neither visits borrowed SceneObjects
            //nor clears the source alias/count. Models may already be retired.
            spinAllocationB58_.reset();
            //Native CC0 borrows banked inline storage. Withdraw after B58,
            //at the represented21051C lifetime point, without calling305728
            //or reading/freeing/resetting another active Bank slot's array.
            nativeOuterStorage_.lapTimesCC0=nullptr;
            slots800_.reset();slots804_.reset();slots808_.reset();slots80C_.reset();
            slots818_.reset();slots81C_.reset();
            if(count_==2u){--detached7F8.reference00;--counter;}
            //210664..210688 is in2104D0 AFTER142590 retired the racer's
            // attached children and AFTER detached7F8. Borrowed scene records
            // may already be dead: plain allocation deletion never reads them.
            wristAllocations_[0].reset();wristAllocations_[1].reset();
            //210710's D34 lifetime: withdraw split native lane descriptors.
            //No source2E8B00 receipt and no dereference/reset of banked memory.
            nativeOuterStorage_.impact={};nativeOuterStorage_.bank=nullptr;
            nativePhyLoad_=nullptr; //Borrow withdrawal only; Operation owns it.
            --counter;
        }
        cleanupPhase_=3;released_=true;
        return {S::complete,0x210730u};
    }
private:
    OriginalRacerEntityColdBinding50LA binding_{};
    std::array<std::unique_ptr<OriginalRacerWristAllocation50LA>,2> wristAllocations_;
    using Slots2=std::array<OriginalWreckComponent50LA*,2>;
    using Slots4=std::array<OriginalWreckComponent50LA*,4>;
    using Slots10=std::array<OriginalWreckComponent50LA*,10>;
    std::unique_ptr<Slots4> slots800_,slots804_,slots80C_;
    std::unique_ptr<Slots2> slots808_,slots81C_;
    std::unique_ptr<Slots10> slots818_;
    std::size_t nativeSource818Published_=0; // actual212C74/212CFC writes, not authored count

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
            if(!hasOverlay||!particles4A0_||!sourceParticle7FC_||*sourceParticle7FC_!=&particle7FC)return false;
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
