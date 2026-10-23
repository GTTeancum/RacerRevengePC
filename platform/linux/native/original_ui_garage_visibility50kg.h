#pragma once
// Retained cold Garage Scene4BC, two real search-node Entities and four
// distinct List owners. This is not Page, collector or renderer admission.
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include "../../../src/game/live_zone_membership50en.h"
#include "../../../src/game/original_world_environment50kg.h"

namespace rr::game {class OriginalFrontendCameraCold50LA;}
namespace rr::assets {class OriginalUiGarageWattoAssets50KG;}
namespace rr::platform {
class OriginalUiGarageCameraFrame50KG;
class OriginalUiLightManagerOwner50KG;
class OriginalUiGarageSelectedModel50KG;
class OriginalUiGarageWattoOwner50KG;
class OriginalUiGaragePodScene50KG;
class OriginalUiGarageStaticModels50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageSceneCollision50KG;
class OriginalUiGarageVisibility50KG final {
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGarageSelectedModel50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiLightManagerOwner50KG;
    using R=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    struct Storage;
    //Actual source emitter74 row, not the model root substituted for it.
    //Read after fire velocity stores; output is only the source stale-tag return.
    bool skipFireObserver15E1B4(const OriginalUiGaragePodScene50KG&,const R&,
        std::uint32_t,const game::OriginalRacerSceneObject50LA&,bool&,D&)const noexcept;
    bool finishSearchNode2805F0(OriginalUiGaragePageOwner50KG&,const OriginalUiGarageCameraFrame50KG&,
        game::OriginalWreckComponent50LA&,D&)noexcept;
    bool renderCameraSearch114E00(OriginalUiGaragePageOwner50KG&,const OriginalUiGarageCameraFrame50KG&,D&)noexcept;
    bool finishModelChildren24186C(const R&,D&)const noexcept;
    bool modelLightZone14A110(const OriginalUiLightManagerOwner50KG&,const R&,std::uint32_t&)const noexcept;
    bool prepareSelectedModelPackets241280(OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageCameraFrame50KG&,std::size_t sourceList,std::size_t sourceRow,
        std::shared_ptr<R>& actualModel,bool& selected,D&)noexcept;
    std::unique_ptr<Storage> storage_;
    bool attempted_=false,closed_=false,searchEntitiesRetired_=false;
    OriginalUiGaragePodScene50KG* sourceSceneOwner_=nullptr; //Actual retained owner, no readiness flag.
    // Private identity preflight for the retained cold custody join. It proves
    // exact owners, not completed static population or renderer admission.
    game::OriginalWreckComponent50LA* pageEntity4BC()noexcept;
    bool attachPage4BC1B48D4(OriginalUiGaragePageOwner50KG&,OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool registerPageCamera1FB608(OriginalUiGaragePageOwner50KG&,
        std::shared_ptr<game::OriginalFrontendCameraCold50LA>,D&)noexcept;
    bool pageCameraRegistrationCurrent(const OriginalUiGaragePageOwner50KG&,
        const game::OriginalFrontendCameraCold50LA&)const noexcept;
    bool prepareCameraSearch114D28(OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageCameraFrame50KG&,D&)noexcept;
    bool updatePageCameraObserver114C44(const OriginalUiGaragePageOwner50KG&,
        game::OriginalFrontendCameraCold50LA&,D&)noexcept;
    bool retirePageCameraObserver1155F4(const OriginalUiGaragePageOwner50KG&,
        game::OriginalFrontendCameraCold50LA&,D&)noexcept;
    bool nativeTerminalRenderQuiescent(const OriginalUiGaragePageOwner50KG&,std::uint64_t)const noexcept;
    bool matchesColdConstruction(const std::shared_ptr<Grant>&,
        const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>&)const noexcept;
    bool matchesRetainedSceneOwnership(const std::shared_ptr<Grant>&,
        const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>&,
        const OriginalUiGaragePageOwner50KG&,const OriginalUiGaragePodScene50KG&)const noexcept;
    bool prepare2729E0(std::shared_ptr<Grant>,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,D&)noexcept;
    friend class OriginalUiGarageSceneCollision50KG;
    bool beginCollisionPopulation2743B0(OriginalUiGarageSceneCollision50KG&,D&)noexcept;
    bool attachCollision1B58A8(OriginalUiGarageSceneCollision50KG&,std::size_t,D&)noexcept;
    bool collisionMembershipCurrent(const OriginalUiGarageSceneCollision50KG&,std::size_t)const noexcept;
    bool releaseCollisionTemporary120200(OriginalUiGarageSceneCollision50KG&,std::size_t,D&)noexcept;
    bool endCollisionPopulationRetirement(OriginalUiGarageSceneCollision50KG&,D&)noexcept;
    bool constructDefaultZoneObserver272F44(D&)noexcept;
    bool defaultZoneObserverCurrent272F90()const noexcept;
    bool retireDefaultZoneObserver270138(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool defaultZoneObserverRetired270144(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool prepareEnvironment272FC0(const game::original_world_environment50KG::SpatialBounds&,D&)noexcept;
    bool environmentCurrent272FC0()const noexcept;
    bool clearEnvironmentGlobal26FF54(D&)noexcept;
    // Indices are authored zero-based slots. No arbitrary zone, method,
    // observer, fallback callback or caller-supplied bounds are admitted.
    bool beginStaticPopulation2741C0(OriginalUiGarageStaticModels50KG&,D&)noexcept;
    bool attachStatic27AE0C(OriginalUiGarageStaticModels50KG&,std::size_t,std::shared_ptr<R>,D&)noexcept;
    bool staticMembershipCurrent(const OriginalUiGarageStaticModels50KG&,std::size_t,const R&)const noexcept;
    bool endStaticPopulationRetirement(OriginalUiGarageStaticModels50KG&,D&)noexcept;
    bool attachSelected1FB67C(OriginalUiGarageSelectedModel50KG&,D&)noexcept;
    bool attachActor2D9350(std::size_t,std::shared_ptr<R>,D&)noexcept;
    bool updateActor28EA20(std::size_t,R&,D&)noexcept;
    bool removeActor28EA80(std::size_t,R&,D&)noexcept;
    bool removeStatic28EA80(std::size_t,R&,D&)noexcept;
    bool pageStaticObserverCurrent(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageStaticModels50KG&,std::size_t,const R&)const noexcept;
    bool preflightPageStaticObserver28976C(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageStaticModels50KG&,std::size_t,const R&)const noexcept;
    bool retirePageStaticObserver28976C(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageStaticModels50KG&,std::size_t,R&,D&)noexcept;

    bool pageActorObserverCurrent(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageActors50GN&,std::size_t,const R&)const noexcept;
    bool preflightPageActorObserver28976C(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageActors50GN&,std::size_t,const R&)const noexcept;
    bool retirePageActorObserver28976C(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageActors50GN&,std::size_t,R&,D&)noexcept;
    bool removeSelected28EA80(OriginalUiGarageSelectedModel50KG&,D&)noexcept;
    //Actual manager-owned Observers outlive Page/Scene Search payloads.
    //These paths never unregister or touch expired04/08 targets.
    bool liveOrExpiredWattoObserverCurrent(const OriginalUiGarageWattoOwner50KG&)const noexcept;
    bool externalSearchTargetExpired(const OriginalUiGaragePageOwner50KG* failedLoadedPage=nullptr,
        const OriginalUiGaragePageOwner50KG* failedWattoBindingPage=nullptr)const noexcept;
    bool externalObserverCurrent(std::size_t,bool targetExpired,const OriginalUiGaragePageOwner50KG* failedLoadedPage=nullptr,
        const OriginalUiGaragePageOwner50KG* failedWattoBindingPage=nullptr)const noexcept;
    bool externalObserverRetirementCurrent(std::size_t,const R&,const OriginalUiGaragePageOwner50KG* failedLoadedPage=nullptr,
        const OriginalUiGaragePageOwner50KG* failedWattoBindingPage=nullptr)const noexcept;
    bool preflightSelectedObserver28976C(const OriginalUiGarageSelectedModel50KG&)const noexcept;
    bool preflightFailedLoadedSelectedObserver28976C(const OriginalUiGarageSelectedModel50KG&,
        const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireFailedLoadedSelectedObserver28976C(OriginalUiGarageSelectedModel50KG&,
        const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool preflightFailedWattoBindingSelectedObserver28976C(const OriginalUiGarageSelectedModel50KG&,
        const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireFailedWattoBindingSelectedObserver28976C(OriginalUiGarageSelectedModel50KG&,
        const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool preflightWattoObserver28976C(const OriginalUiGarageWattoOwner50KG&)const noexcept;
    bool retireSelectedObserver28976C(OriginalUiGarageSelectedModel50KG&,D&)noexcept;
    bool retireWattoObserver28976C(OriginalUiGarageWattoOwner50KG&,D&)noexcept;
    bool retireExternalObserver28976C(std::size_t,R&,D&,const OriginalUiGaragePageOwner50KG* failedLoadedPage=nullptr,
        const OriginalUiGaragePageOwner50KG* failedWattoBindingPage=nullptr)noexcept;
    // The real widget owner supplies its retained animation/model identities
    // AND this exact owner, acquired only at later Page entry1FAC08. Watto's
    // widget-first constructor never receives Visibility or a Scene zone.
    //1D4800 removes the prior observer before mode1 reattachment.28EA20 is a
    // distinct later tick operation, after154480 and wing-overlay1559F0.
    bool attachWatto1D4800(OriginalUiGarageWattoOwner50KG&,D&)noexcept;
    bool updateWatto28EA20(OriginalUiGarageWattoOwner50KG&,D&)noexcept;
    bool removeWatto28EA80(OriginalUiGarageWattoOwner50KG&,D&)noexcept;
    bool wattoModelMatches(const OriginalUiGarageWattoOwner50KG&,std::shared_ptr<R>&,
        std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>&,D&)const noexcept;
    bool wattoModelIdentityMatches(const OriginalUiGarageWattoOwner50KG&,std::shared_ptr<R>&,
        std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>&,D&)const noexcept;
    //Detached component abort only: all its Observers must already be removed.
    //The real joined Page/Scene uses the typed normal callbacks below.
    bool close26FD90(D&)noexcept;
    //Separate detached and genuine Page-owned phases. Normal List destruction
    //retains the two actual external Observers with expired targets; the
    //completed-Page Scope receipt gates their later manager destruction.
    bool retireSearchEntities142590(D&)noexcept;
    bool retirePageSearchEntities142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool normalSearchEntitiesRetired142590()const noexcept;
    bool sourceSearchEntitiesRetiredForPayload()const noexcept;
    bool failedPageSearchEntitiesRetired142590(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireFailedPageSearchEntities142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retirePageSearchEntitiesBody142590(const OriginalUiGaragePageOwner50KG&,bool,D&)noexcept;
    bool pageSearchReferentsDeadBody27F180(const OriginalUiGaragePageOwner50KG&,bool,D&)const noexcept;
    bool retirePrimaryNodePointers26FF14(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool retireSecondaryNodePointers26FF2C(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool sourceNodePointersReleased26FF34()const noexcept;
    bool clearSourceEnvironmentGlobal26FF54(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool sourceEnvironmentGlobalCleared26FF54()const noexcept;

    bool allocateSceneSearchListArrays272E38(D&)noexcept;
    bool sceneSearchListArraysCurrent272E38()const noexcept;
    bool pageSearchReferentsDead27F180(const OriginalUiGaragePageOwner50KG&,D&)const noexcept;
    bool retirePageSceneSearchLists26FF6C(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool sceneSearchListsRetired2700E8(const OriginalUiGaragePodScene50KG&)const noexcept;
    
    bool releaseSceneLists26FF6C(D&)noexcept;
    const game::OriginalWreckComponent50LA* primaryZone4C0()const noexcept;
    const game::OriginalWreckComponent50LA* secondaryZone4C4()const noexcept;
    bool attach(std::size_t,std::shared_ptr<R>,game::ZoneMethod50CL,D&)noexcept;
    bool update(std::size_t,R&,D&)noexcept;
    bool remove(std::size_t,R&,D&)noexcept;
    bool makeBounds28E230(R&,game::LiveZoneBounds50EN&,D&)noexcept;
    bool modelMatches(const R&)const noexcept;
public:
    OriginalUiGarageVisibility50KG()noexcept;
    ~OriginalUiGarageVisibility50KG();
    OriginalUiGarageVisibility50KG(const OriginalUiGarageVisibility50KG&)=delete;
    OriginalUiGarageVisibility50KG& operator=(const OriginalUiGarageVisibility50KG&)=delete;
    // Only cold ownership/coherence, never visible or drawable readiness.
    bool current()const noexcept;
};
}
