namespace ra_pod_scene50LA {class Owner;}
#pragma once
#include "original_ui_search_list_cold50kg.h"
#include "../../../src/game/original_racer_light_selection50km.h"
#include "../../../src/game/original_ui_light_manager_lifetime50kg.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game::original_gc_gameplay_lighting_candidate {class Owner;}
namespace rr::platform {
class OriginalApplicationAural50KG;
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageModelResource50KG;
class OriginalUiGarageUiLights50KG;
class OriginalUiGarageSceneLighting50KG;
class OriginalUiGaragePodScene50KG;
// Concrete fresh147ED0, one-zone148B60 and typed UI/Scene Light registries.
// Ordinary scalar rendering/gameplay receipts are separate native lifetimes;
// they never manufacture source Light rows or spatial membership.
class OriginalUiLightManagerOwner50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageUiLights50KG;
    friend class OriginalUiGarageSceneLighting50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    using C=game::OriginalWreckComponent50LA;
    using E=game::OriginalApplicationEntities50LA;
    using L=OriginalUiSearchListCold50KG;
    using Words=std::array<std::uint32_t,4>;
public:
    enum class Status : std::uint8_t {complete,owner,affinity,phase,topology,allocation,list,identity};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;};
    struct Observation {
        const C* entity=nullptr;const C* zone=nullptr;const L* list=nullptr;
        L::Snapshot listState{};Words vector30{},default50{},default70{},ambient60{},ambient80{};
        std::uint32_t count90=0;bool constructed=false,zonesComplete=false,zonePublished=false,listPublished=false;
        bool arrays88=false,arrays8C=false,array60=false,array80=false,array44=false,directional44Known=false;
        bool ambientKnown=false,defaultsKnown=false,retired=false;
        Words directional44{};std::uint32_t revision84=0,count94=0,capacity98=0,fieldA0=0;
        std::uintptr_t directional40Identity=0;std::array<std::uintptr_t,2> registryIdentities{};std::array<std::uintptr_t,2> sceneRegistryIdentities{};
        bool directional40Live=false,registryBacking=false;
        const assets::OriginalUiArchiveIdentity50KG* nativeProjectionUi=nullptr;
        std::uint64_t nativeProjectionUiGeneration=0,nativeProjectionGameplayGeneration=0;
    };
    ~OriginalUiLightManagerOwner50KG()noexcept;
    OriginalUiLightManagerOwner50KG(const OriginalUiLightManagerOwner50KG&)=delete;
    OriginalUiLightManagerOwner50KG& operator=(const OriginalUiLightManagerOwner50KG&)=delete;
    bool current()const noexcept;
    // Ordinary renderer/gameplay projections retain this source singleton but
    // cannot concurrently enter its concrete Garage zone/registry producers.
    bool sourceMutationAvailable()const noexcept;
    Observation inspect()const noexcept;
private:
    friend class ::ra_pod_scene50LA::Owner;
    struct ParticleLight149310 {
        std::array<float,3> ambient50{},diffuse3C{};
        std::array<float,4> positionA0{};
        const C* lastType2=nullptr;
    };
    bool readGarageParticleLight149310(const ::original_main_entry50KG::Application&,
        ParticleLight149310&,Diagnostic&)const noexcept;
    struct Zone {
        C entity;
        // Only27F120's explicit cold stores; unknown vectors/matrices absent.
        const void* field30=nullptr;L* first34=nullptr;L* second38=nullptr;
        const void* field3C=nullptr;const void* field40=nullptr;const void* field44=nullptr;
        std::uint32_t index48=0;const void* fieldA0=nullptr;
        bool constructed=false,published=false;
    };
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::weak_ptr<OriginalUiLightManagerOwner50KG> issuedOwner_;
    std::shared_ptr<E> entities_;
    std::shared_ptr<game::OriginalUiLightManagerLifetime50KG> scalar_;
    game::OriginalApplicationAnimationClockView50KG clock_;
    C entity_;std::uint64_t generation_=0;
    Words vector30_{},default50_{},default70_{};
    // Scalar constructor stores. No unwritten directional row is observed.
    const C* field40_=nullptr;
    std::uint32_t field84_=0,count90_=0,count94_=0,capacity98_=0,fieldA0_=0;
    std::unique_ptr<const C*[]> registry9C_;const void* cacheA4_=nullptr;std::int32_t fieldA8_=-1;
    std::unique_ptr<Zone*[]> nodes88_;std::unique_ptr<L*[]> lists8C_;
    std::unique_ptr<Words[]> ambient60_,ambient80_,directional44_;
    std::unique_ptr<Zone> zone_;std::unique_ptr<L> list_;
    bool constructed_=false,retired_=false,zonesAttempted_=false,zonesComplete_=false;
    bool listPublished_=false,ambientKnown_=false,defaultsKnown_=false;
    bool directional44Known_=false,directional40Live_=false,registryFailed_=false;
    //Private concrete RA source-zone custody, distinct from native canonical
    //lighting's authored render rows. No second singleton or scalar reset.
    struct RaPortalLink27F3B0 {
        RaPortalLink27F3B0* next00=nullptr;
        const C* neighbor04=nullptr;const C* portal08=nullptr;
        std::uint32_t field0C=0,field10=0x7f7fffff;
        bool distanceWritten=false; //Only actual231150 overlap publishes both.
    };
    struct RaZones148B60 {
        const ::ra_cold_aggregate50LA::Owner* world=nullptr;
        std::shared_ptr<game::original_gc_gameplay_lighting_candidate::Owner> canonical;
        std::uint64_t gameplayGeneration=0;
        std::array<std::unique_ptr<Zone>,10> zones;
        std::array<std::unique_ptr<L>,10> lists;
        std::array<bool,10> listPublished{},ambientKnown{};
        std::size_t next=0;
        //Actual node30 owns each source-ordered uncounted Portal link record;
        //these unique_ptrs provide private partial-allocation custody, not a
        //second registry/render-list. Both pointers are unretained borrows.
        std::array<const C*,10> portals{};const C* portalParent=nullptr;
        std::array<std::unique_ptr<RaPortalLink27F3B0>,20> portalLinks;
        std::size_t portalLinksPublished=0;
        const C* firstAmbient=nullptr; //Actual RA registry slot0; no separate registry.
        const C* firstDirectional=nullptr; //Actual next registry slot1.
        std::array<const C*,183> localRegistered{}; //Actual9C rows2..184, independent borrows.
        std::array<const C*,183> localPrimary{}; //Actual primary List membership per owner.
        std::size_t localRegisteredCount()const noexcept {std::size_t n=0;for(auto* p:localRegistered)n+=bool(p);return n;}
    };
    std::unique_ptr<RaZones148B60> raZones_;
    bool raZonesScopeCurrent()const noexcept;
    bool raZonesReleasePreflight()const noexcept;
    static bool bindRaWorldZones272A48(const ::original_main_entry50KG::Application&,
        const ::ra_cold_aggregate50LA::Owner&,
        const std::shared_ptr<game::original_gc_gameplay_lighting_candidate::Owner>&,
        std::shared_ptr<OriginalUiLightManagerOwner50KG>&,Diagnostic&)noexcept;
    bool setupRaZones148B60(Diagnostic&)noexcept;
    bool linkRaPortal148D00(const ::ra_cold_aggregate50LA::Owner&,std::size_t ordinal,
        const C& portal,const C& actualParent,std::uint32_t zoneA,std::uint32_t zoneB,Diagnostic&)noexcept;
    bool localMembershipsCurrent()const noexcept;
    bool continueRaLocalPortals280BF0(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool appendRaLocal148F30(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool incrementRaLocal149300(const ::ra_cold_aggregate50LA::Owner&,const C&,std::uint32_t,Diagnostic&)noexcept;
    bool bindRaLocalPrimary2809C0(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool removeRaLocalPrimary1491F0(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool applyRaDirectionalColor1498A0(const ::ra_cold_aggregate50LA::Owner&,std::size_t,Diagnostic&)noexcept;
    bool growRaRegistry149194(Diagnostic&)noexcept;
    bool incrementRaLaterAmbient149300(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool appendRaLaterAmbient148F30(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool incrementRaDirectional149300(const ::ra_cold_aggregate50LA::Owner&,const C&,std::uint32_t,Diagnostic&)noexcept;
    bool appendRaDirectional148F30(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool removeRaDirectional1491F0(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool incrementRaAmbient149300(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool appendRaAmbient148F30(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool removeRaAmbient1491F0(const ::ra_cold_aggregate50LA::Owner&,const C&,Diagnostic&)noexcept;
    bool releaseRaZones148110(const ::ra_cold_aggregate50LA::Owner&,Diagnostic&)noexcept;

    const OriginalUiGarageUiLights50KG* uiLightsOwner_=nullptr;
    OriginalUiGarageSceneLighting50KG* sceneLightsOwner_=nullptr;
    bool raRegistryGrowthFailed_=false; //Native retained old allocation on terminal realloc failure.
    std::array<const C*,190> registryPublished_{};std::size_t registryWritten_=0;
    // Native receipts, not guest Light fields. UI generations survive release;
    // ordinary projection writes never substitute for source148B60/148F30.
    const assets::OriginalUiArchiveIdentity50KG* nativeProjectionUi_=nullptr;
    std::uint64_t nativeProjectionUiGeneration_=0,nativeProjectionGameplayGeneration_=0;
    using Scalar=game::OriginalUiLightManagerLifetime50KG;
    using ProjectionOperation=Scalar::NativeProjectionOperation;
    static bool nativeProjectionBridge(OriginalUiLightManagerOwner50KG*,const Scalar&,
        ProjectionOperation,const assets::OriginalUiArchiveIdentity50KG*,std::uint64_t)noexcept;
    bool nativeProjectionInputsEmpty()const noexcept;
    OriginalUiLightManagerOwner50KG(const ::original_main_entry50KG::Application*,std::shared_ptr<E>,
        std::shared_ptr<game::OriginalUiLightManagerLifetime50KG>,game::OriginalApplicationAnimationClockView50KG)noexcept;
    static std::uint64_t key(const void* p)noexcept{return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));}
    static bool fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept{d={s,pc};return false;}
    // Explicit bounded native extension only. Ordinary fresh factory remains
    // count2/no-history; this overload accepts only Root's concrete original
    //13FE24 Aural prefix and never resets nonRootEnrollmentObserved_.
    static bool constructAfterAuralNative140074(const ::original_main_entry50KG::Application&,
        const OriginalApplicationAural50KG&,std::shared_ptr<OriginalUiLightManagerOwner50KG>&,Diagnostic&)noexcept;
    static bool constructFresh140074(const ::original_main_entry50KG::Application&,
        std::shared_ptr<OriginalUiLightManagerOwner50KG>&,Diagnostic&)noexcept;
    bool sourceIdentity()const noexcept;
    // Actual24BD7C renderer-context call, distinct from per-model14A0A0.
    // The Draw caller owns invocation/publication and retains this manager;
    // values come from its actual40 Light and50 ambient, never zone60.
    bool readGarageGlobalLightRows1499A0(const ::original_main_entry50KG::Application&,
        game::original_racer_lighting50KM::LightMatrices&,Diagnostic&)const noexcept;
    bool selectGarageModel14A0A0(const OriginalUiGarageVisibility50KG&,
        const OriginalUiGarageModelResource50KG&,game::original_racer_lighting50KM::LightMatrices&,Diagnostic&)const noexcept;
    bool update1483E0(float,Diagnostic&)noexcept;
    bool registryCurrent()const noexcept;
    bool bindUiLights(OriginalUiGarageUiLights50KG&,Diagnostic&)noexcept;
    bool incrementUiRevision149300(OriginalUiGarageUiLights50KG&,Diagnostic&)noexcept;
    bool appendUiLight148F30(OriginalUiGarageUiLights50KG&,std::size_t,Diagnostic&)noexcept;
    bool preflightRemoveUiLight1491F0(const OriginalUiGarageUiLights50KG&,std::size_t)const noexcept;
    bool removeUiLight1491F0(OriginalUiGarageUiLights50KG&,std::size_t,Diagnostic&)noexcept;
    bool releaseUiLightsBorrow(OriginalUiGarageUiLights50KG&,Diagnostic&)noexcept;
    bool bindSceneLights(OriginalUiGarageSceneLighting50KG&,Diagnostic&)noexcept;
    bool incrementSceneRevision149300(OriginalUiGarageSceneLighting50KG&,Diagnostic&)noexcept;
    bool serviceSceneLightShared147A00(OriginalUiGarageSceneLighting50KG&,const C&,Diagnostic&)noexcept;
    bool appendSceneLight148F30(OriginalUiGarageSceneLighting50KG&,std::size_t,Diagnostic&)noexcept;
    bool preflightRemoveSceneLight1491F0(const OriginalUiGarageSceneLighting50KG&,std::size_t)const noexcept;
    bool removeSceneLight1491F0(OriginalUiGarageSceneLighting50KG&,std::size_t,Diagnostic&)noexcept;
    bool releaseSceneLightsBorrow(OriginalUiGarageSceneLighting50KG&,Diagnostic&)noexcept;
    bool zoneReleasePreflight()const noexcept;
    bool setupOneZone148B60(Diagnostic&)noexcept;
    bool releaseZones148110(Diagnostic&)noexcept;
    // Supported native envelope requires zone teardown already complete.
    // Source148020 frees60/80/44 before148110, so no auto-reset/reordering.
    bool retireEmpty148020(Diagnostic&)noexcept;
};
}
