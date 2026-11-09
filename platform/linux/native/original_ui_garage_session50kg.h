#pragma once
// Private native COLD custody only. This is not a Garage Page, a completed
// session, a renderer/route capability or an enclosing constructor receipt.
#include "original_ui_garage_actors50gn.h"
#include "original_ui_garage_watto_owner50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_rider_profile50kg.h"
#include "../../../src/assets/original_standard_texture_resource50la.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include <thread>

namespace rr::platform {
// Observation of one actual Page input call, not a route/admission credential.
struct OriginalUiGarageNativeInput50KG {
    enum class Status : std::uint8_t {unavailable,ignored,completed,failed};
    Status status=Status::unavailable;
    std::uint64_t serial=0,transportBoundary=0;
    std::uint32_t sourcePC=0,selectedStat=0;
    bool sourceEffectsCommitted=false,requestWritten=false;
};

// Observations of retained source values, never a caller-created readiness token.
struct OriginalUiGarageNativeState50KG {
    game::OriginalUiApertureSnapshot50KG aperture{};
    std::uint64_t publication=0;
    std::uint32_t managerLifecycle=0,managerTransition=0,selectedStat=0;
    bool loaded=false,rootReturned=false,timingCommitted=false,targetPublished=false,retired=false,paused=false;
    double updateSeconds=0;
};

class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageCameraFrame50KG;
class OriginalUiGarageSession50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend struct ::original_main_entry50KG::Frontend;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageCameraFrame50KG;
    friend class OriginalUiMainAudio50KG;
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiTournamentCareerResources50KG;
    friend class OriginalUiTournamentSession50KG;
    friend class OriginalUiGaragePageOwner50KG;
    using Application=::original_main_entry50KG::Application;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Resources=OriginalUiGarageModelResources50KG;
    using SceneAssets=assets::OriginalUiGarageSceneAssets50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    OriginalUiGarageSession50KG(const Application&,std::shared_ptr<Grant>,
        std::shared_ptr<Resources>,std::shared_ptr<const SceneAssets>,
        std::shared_ptr<OriginalUiGarageVisibility50KG>)noexcept;
    // Native custody is retained before1B823C. Its address identifies this
    // manager epoch; no caller-provided generation or initialized context.
    OriginalUiGarageSession50KG(const Application&,std::shared_ptr<const SceneAssets>)noexcept;
    enum class ManagerEntryPhase : std::uint8_t {
        notStarted,managerConstructed1D1860,grantIssued1D1950,
        transfersReturned2542F0,selectedBankOpened1D1D68,selectedModelConstructed1D1E80,
        selectedPopulationReturned1D1E90,selectedReturned1D1CD0,baseInputsReturned1CCC84
    };
    ManagerEntryPhase managerEntryPhase_=ManagerEntryPhase::notStarted;
    // Native failed-constructor unwind, not a successful loader or normal Back.
    // Only the exact empty detached prefix before selected2403A0 is supported.
    enum class EntryAbortPhase : std::uint8_t {
        notStarted,transfersRetired,selectedBankRetired,managerRetired,closed
    };
    EntryAbortPhase entryAbortPhase_=EntryAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* entryAbortFrontend_=nullptr;
    // Exact failed return from the base-bank image-cache call, before any
    // base source binding/corpus/DAT work. Not a generic failed-entry grant.
    bool managerBaseImagesFailed12CEA0_=false;
    //A distinct observed failure after successful IPUM binding but BEFORE
    //the helper invokes Domain.appendOriginalSources. No corpus rollback.
    bool managerBaseCorpusBeforeAppendFailed_=false;
    assets::OriginalStandardMaterialStatus50LA managerBaseCorpusFailureStatus_=assets::OriginalStandardMaterialStatus50LA::complete;
    std::uint32_t managerBaseCorpusFailurePC_=0;
    enum class BaseEntryAbortPhase : std::uint8_t {
        notStarted,selectedRetired,transfersRetired,bankRetired,managerRetired,audioRetired,closed
    };
    BaseEntryAbortPhase baseEntryAbortPhase_=BaseEntryAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* baseEntryAbortFrontend_=nullptr;
    bool baseImageFailurePrefixCurrent(const Application&)const noexcept;
    bool baseCorpusFailurePrefixCurrent(const Application&)const noexcept;
    bool baseInputsFailurePrefixCurrent(const Application&)const noexcept;
    bool baseInputsOwnerPrefixCurrent(const Application&)const noexcept;
    bool preflightBaseSelectedAbort(D&)const noexcept;
    bool preflightAttachedSelectedAbort(D&)const noexcept;
    bool retireBaseSelectedAbort(D&)noexcept;
    // Only the first shared prompt's real29B638 failure, before Scene adoption.
    enum class SharedPageAbortPhase:std::uint8_t {
        notStarted,pageRetiring,pageRetired,fogRetired,widgetsRetired,fogAliasesReleased,
        directionalRetired,ambientRetired,searchNodeRetired,selectedRetiring,selectedRetired,
        managerRefReleased,transfersRetired,searchPayloadRetired,lightPayloadRetired,
        bankRetired,pageTableRetired,widgetTableRetired,managerRetired,audioRetired,closed
    };
    SharedPageAbortPhase sharedPageAbortPhase_=SharedPageAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* sharedPageAbortFrontend_=nullptr;
    enum class PartialSceneAbortPhase:std::uint8_t {notStarted,pageRetiring,pageRetired,fogRetired,
        widgetsRetired,fogAliasesReleased,directionalRetired,ambientRetired,searchNodeRetired,selectedRetiring,
        selectedRetired,managerRefReleased,transfersRetired,searchPayloadRetired,lightPayloadRetired,
        bankRetired,pageTableRetired,widgetTableRetired,managerRetired,audioRetired,closed};
    PartialSceneAbortPhase partialSceneAbortPhase_=PartialSceneAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* partialSceneAbortFrontend_=nullptr;
    bool hasPartialSceneAttachmentFailure()const noexcept;
    bool partialSceneFailurePrefixCurrent(const Application&)const noexcept;
    bool nativeAudioPartialSceneFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool sceneRetiring)const noexcept;
    bool nativeAudioPartialSceneTerminalFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool managerRetired)const noexcept;
    enum class WattoBindingAbortPhase:std::uint8_t {notStarted,pageRetiring,pageRetired,fogRetired,
        widgetsRetired,fogAliasesReleased,directionalRetired,ambientRetired,searchNodeRetired,selectedRetiring,
        selectedRetired,managerRefReleased,transfersRetired,searchPayloadRetired,lightPayloadRetired,
        bankRetired,pageTableRetired,widgetTableRetired,managerRetired,audioRetired,closed};
    WattoBindingAbortPhase wattoBindingAbortPhase_=WattoBindingAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* wattoBindingAbortFrontend_=nullptr;
    bool hasWattoBindingFailure()const noexcept;
    bool wattoBindingFailurePrefixCurrent(const Application&)const noexcept;
    bool nativeAudioWattoBindingFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool sceneRetiring)const noexcept;
    bool nativeAudioWattoBindingTerminalFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool managerRetired)const noexcept;
    enum class LoadedPageAbortPhase:std::uint8_t {notStarted,pageRetiring,pageRetired,fogRetired,
        widgetsRetired,fogAliasesReleased,directionalRetired,ambientRetired,searchNodeRetired,selectedRetiring,
        selectedRetired,managerRefReleased,transfersRetired,searchPayloadRetired,lightPayloadRetired,
        bankRetired,pageTableRetired,widgetTableRetired,managerRetired,audioRetired,closed};
    LoadedPageAbortPhase loadedPageAbortPhase_=LoadedPageAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* loadedPageAbortFrontend_=nullptr;
    bool hasLoadedPageHideFailure()const noexcept;
    bool loadedPageFailurePrefixCurrent(const Application&)const noexcept;
    bool nativeAudioLoadedPageFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool sceneRetiring)const noexcept;
    bool nativeAudioLoadedPageTerminalFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool managerRetired)const noexcept;
    enum class LateBorrowAbortPhase:std::uint8_t {notStarted,pageRetiring,pageRetired,fogRetired,
        widgetsRetired,fogAliasesReleased,directionalRetired,ambientRetired,searchNodeRetired,selectedRetiring,
        selectedRetired,managerRefReleased,transfersRetired,searchPayloadRetired,lightPayloadRetired,
        bankRetired,pageTableRetired,widgetTableRetired,managerRetired,audioRetired,closed};
    LateBorrowAbortPhase lateBorrowAbortPhase_=LateBorrowAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* lateBorrowAbortFrontend_=nullptr;
    bool hasLateSceneBorrowFailure()const noexcept;
    bool lateBorrowFailurePrefixCurrent(const Application&)const noexcept;
    bool nativeAudioLateBorrowFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool sceneRetiring)const noexcept;
    bool nativeAudioLateBorrowTerminalFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool managerRetired)const noexcept;
    bool hasSharedPromptFailure()const noexcept;
    bool sharedPageFailurePrefixCurrent(const Application&)const noexcept;
    bool preflightSharedPageFailure(D&)const noexcept;
    bool nativeAudioSharedPageFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t,bool managerRetired)const noexcept;


    //Actual temporary selected19E lookup bank. Cached Model/Material/Texture
    //owners survive its source12E5E0 close; failed entry retains the live lease.
    std::unique_ptr<assets::OriginalStandardTextureResolver50LA::GarageBankLease50LA> selectedBankLease_;
    bool selectedArchiveOnlyScope1D1D88_=false;
    ::original_main_entry50KG::Frontend* entryFrontend_=nullptr;
    std::uint64_t entryManagerGeneration_=0;
    assets::OriginalStandardMaterialStatus50LA entryFailureStatus_=assets::OriginalStandardMaterialStatus50LA::complete;
    std::uint32_t entryFailureSourcePC_=0;
    std::shared_ptr<OriginalUiGarageSelectedModel50KG> entrySelected_;
    bool prepareManagerSelected1D1CD0(D&)noexcept;
    //Separate terminal failure after complete selected resource construction,
    //before any successful enrollment, attachment, visibility or audio entry.
    enum class UnenrolledSelectedAbortPhase : std::uint8_t {
        notStarted,selectedRetired,transfersRetired,bankRetired,managerRetired,closed
    };
    UnenrolledSelectedAbortPhase unenrolledSelectedAbortPhase_=UnenrolledSelectedAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* unenrolledSelectedAbortFrontend_=nullptr;
    bool hasUnenrolledSelectedFailure()const noexcept;
    bool unenrolledSelectedFailurePrefixCurrent(const Application&)const noexcept;
    bool preflightUnenrolledSelectedAbort(D&)const noexcept;
    bool retireUnenrolledSelectedAbort(D&)noexcept;
    // Native continuation cursor of the actual detached manager loader. These
    // are completed source calls, not a Page/frame/menu readiness capability.
    enum class ManagerConstructorPhase : std::uint8_t {
        notStarted,rendererCallsReturned1CCD14,searchReturned1CCD78,
        lightsReturned1CCD98,fogAwaitingRenderer13E8A8,fogReturned1CCDA8,
        widgetsStarted1CD7A0,widgets9Returned,wattoRoutingPending,
        wattoReturned1D46B8,widgetsReturned1CCDD4,returned1CCDE4
    };
    ManagerConstructorPhase managerConstructorPhase_=ManagerConstructorPhase::notStarted;
    const game::OriginalWreckComponent50LA* managerDispatcher2AC_=nullptr;
    std::shared_ptr<OriginalUiGarageManagerSearch50KG> entrySearch_;
    std::shared_ptr<OriginalUiGarageUiLights50KG> entryLights_;
    std::shared_ptr<OriginalUiGarageManagerFog50KG> entryFog_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> entryFonts_;
    // The actual Frontend publication may not yet have a prepared sample.
    // RuntimeFrame still requires its genuine serial/updateSeconds later.
    ::original_ui_source_timing50LH::PublicationView entryTiming_;
    bool prepareManagerSearch1CCD14(D&)noexcept;
    //Exact failed Pending retain after borrowing persistent Main, but before
    //beginRetainedGarageEntry, any base bank, DAT or manager child construction.
    bool pendingRetainFailed98A4_=false;
    enum class PendingRetainAbortPhase : std::uint8_t {
        notStarted,selectedRetired,transfersRetired,managerRetired,closed
    };
    PendingRetainAbortPhase pendingRetainAbortPhase_=PendingRetainAbortPhase::notStarted;
    const ::original_main_entry50KG::Frontend* pendingRetainAbortFrontend_=nullptr;
    bool pendingRetainFailurePrefixCurrent(const Application&)const noexcept;
    bool retirePendingSelectedAbort(D&)noexcept;
    //One production Root caller drives the actual source loader helpers.
    //These cursors never stand in for the private owners' own return proofs.
    enum class ManagerDrivePhase : std::uint8_t {
        notStarted,constructors,pageMetadata,sharedLoader,sceneConstructor,
        pageSpecific,scenePrefix,scenePopulation,pageForest,lateForest,managerReturn,returned
    };
    ManagerDrivePhase managerDrivePhase_=ManagerDrivePhase::notStarted;
    std::int32_t managerDriveRequestedPage_=0; //Native input only; source150 stays unknown until2A3C1C.
    std::shared_ptr<const OriginalUiMainProfile50KG> managerDriveProfile_;
    std::shared_ptr<const assets::OriginalUiGaragePageData50KG> managerDrivePageData_;
    static bool constructionInputsCurrent(const Application&,
        const std::shared_ptr<Grant>&,const std::shared_ptr<Resources>&,
        const std::shared_ptr<const SceneAssets>&)noexcept;
    bool managerArchiveScopeCurrent()const noexcept;
    bool releaseManagerPageTable2A3B04(D&)noexcept;
    bool sameRoot(const Application&)const noexcept;
    // Native final observation failure AFTER genuine source publication.
    // This receipt is never a successful Back, frame, or replay permission.
    bool publishedObservationFailed1B8274_=false,publishedObservationAudioRetired_=false;
    std::uint64_t publishedObservationGeneration_=0,publishedObservationEpoch_=0;
    const OriginalUiGaragePageOwner50KG* publishedObservationPage_=nullptr;
    void retainPublishedObservationFailure1B8274(const Application&)noexcept;
    bool hasPublishedObservationFailure()const noexcept;
    bool publishedObservationFailureInitialIdentity(const Application&)const noexcept;
    bool publishedObservationFailurePreflightCurrent(const Application&)const noexcept;
    bool publishedObservationFailureRetirementCurrent(const Application&)const noexcept;
    bool preparePublishedObservationFailureRetirement(const Application&,D&,std::string&)noexcept;
    bool nativeAudioPublishedObservationFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept;
    // Exact failed first Actor afterSample10179C, with real update effects.
    // Neither failed_ nor any successful source completion is synthesized.
    bool actorSampleFailed10179C_=false,actorSampleAudioRetired_=false,actorSampleFrameReleased_=false;
    std::uint64_t actorSampleGeneration_=0,actorSampleEpoch_=0,actorSamplePublication_=0;
    const OriginalUiGaragePageOwner50KG* actorSamplePage_=nullptr;
    const game::OriginalWreckComponent50LA* actorSampleManager_=nullptr;
    void retainActorSampleFailure10179C(const Application&)noexcept;
    bool hasActorSampleFailure()const noexcept;
    bool actorSampleFailureHistoryCurrent(const Application&)const noexcept;
    bool actorSampleFailureInitialIdentity(const Application&)const noexcept;
    bool actorSampleFailurePreflightCurrent(const Application&)const noexcept;
    bool actorSampleFailureRetirementCurrent(const Application&)const noexcept;
    bool prepareActorSampleFailureRetirement(const Application&,D&,std::string&)noexcept;
    bool nativeAudioActorSampleFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept;
    // Native terminal admission from the actual failed synchronous140230
    // return. Not a Source frame completion, Back receipt, or render replay.
    bool unsealedRenderRetirement_=false,unsealedRenderAudioRetired_=false,unsealedRenderFrameReleased_=false;
    std::uint64_t unsealedRenderGeneration_=0,unsealedRenderEpoch_=0,unsealedRenderPublication_=0;
    std::uint64_t unsealedRenderCompletedUpdates_=0;
    std::uint32_t unsealedRenderSourcePC_=0;std::int32_t unsealedRenderMode_=0;
    const OriginalUiGaragePageOwner50KG* unsealedRenderPage_=nullptr;
    const OriginalUiGarageDrawContribution50KG* unsealedRenderDraw_=nullptr;
    const OriginalUiGarageActors50GN::RuntimeFrame* unsealedRenderFrame_=nullptr;
    bool hasUnsealedRenderFailure()const noexcept;
    bool unsealedRenderReturnedFailureCurrent(const Application&)const noexcept;
    bool unsealedRenderFailureHistoryCurrent(const Application&)const noexcept;
    bool unsealedRenderFailureInitialIdentity(const Application&)const noexcept;
    bool unsealedRenderFailurePreflightCurrent(const Application&)const noexcept;
    bool unsealedRenderFailureRetirementCurrent(const Application&)const noexcept;
    bool prepareUnsealedRenderFailureRetirement(const Application&,D&,std::string&)noexcept;
    bool nativeAudioUnsealedRenderFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept;
    bool constructionCurrent(const Application&)const noexcept;
    bool publishedOwnerCurrent(const Application&)const noexcept;
    bool nativeAudioConstructionIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t&,std::uint64_t&)const noexcept;
    // Process-wide manager lifetime order (scope epochs restart per frontend).
    std::uint64_t nativeAudioConstructionSerial()const noexcept{return grant_?grant_->serial_:0;}
    bool nativeAudioRetirementIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t,std::uint64_t)const noexcept;
    bool nativeAudioEmptyEntryFailureIdentity(const Application&,const OriginalUiMainAudio50KG&,
        std::uint64_t generation,std::uint64_t epoch,bool managerRetired)const noexcept;
    bool widgetPredecessorCurrent(const std::shared_ptr<OriginalUiGarageWidget1Owner50KG>&)const noexcept;
    bool retiredEnclosingInputs()const noexcept;
    // Normal Back is disjoint from cold-abort audio/backing teardown. Each
    // cursor advances only after the exact owner returns, so a pending child
    // retains its real source continuation rather than replaying prior calls.
    enum class NormalRetirementPhase : std::uint8_t {
        notStarted,dispatcherPrefix,managerDetached,pageRetired,fogRetired,
        widgetsRetired,fogAliasesReleased,directionalRetired,ambientRetired,
        searchNodeRetired,selectedRetired,managerRefReleased,transfersRetired,
        searchPayloadRetired,deferredReturned,lightPayloadRetired,bankRetired,
        pageTableRetired,widgetTableRetired,managerCountRetired,currentCleared
    };
    NormalRetirementPhase normalRetirementPhase_=NormalRetirementPhase::notStarted;
    const ::original_main_entry50KG::Frontend* normalFrontend_=nullptr;
    std::uint64_t normalManagerGeneration_=0;
    std::shared_ptr<OriginalUiGarageSelectedModel50KG> normalSelected_;
    std::unique_ptr<assets::OriginalStandardTextureResolver50LA::GarageBankLease50LA> managerBankLease_;
    bool normalRetirementFrameCurrent(const Application&)const noexcept;
    bool prepareNormalRetirement(const Application&,D&,std::string&)noexcept;
    bool retireNormalChildren142590(D&,std::string&)noexcept;
    bool retireNormalManagerPayload1CCADC(D&,std::string&)noexcept;
    bool finishNormalManager2A3B04(D&,std::string&)noexcept;
    bool normalSourceOwnersRetired(D&,std::string&)const noexcept;
    const Application* issuer_=nullptr;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    game::OriginalApplicationAnimationClockView50KG issuerLifetime_;
    std::shared_ptr<game::OriginalApplicationEntities50LA> authority_;
    // This exact population already belongs to the enclosing constructor.
    // Never rebuild it, reserve per actor, or replace it with fixture25+Watto5.
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<game::OriginalUiEntityScope50KG> scope_;
    std::shared_ptr<Resources> resources_;
    std::shared_ptr<const SceneAssets> sceneAssets_;
    std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> wattoAssets_;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility_;
    //The genuine Page owns source4C0. This is only its typed current borrow.
    std::shared_ptr<OriginalUiGaragePageOwner50KG> page_;
    OriginalUiGaragePodScene50KG* podScene_=nullptr;
    // Acquired only at the genuine same-loop Widget10 boundary, never used
    // to pretend later Page/Actors or other source-required stages completed.
    std::shared_ptr<OriginalUiGarageWidget1Owner50KG> widgets_;
    std::shared_ptr<OriginalUiMainApplication50KG> application_;
    std::shared_ptr<game::OriginalMotionLibrary50LA> motions_;
    std::shared_ptr<OriginalUiMainAudio50KG> audio_;
    std::shared_ptr<OriginalAudioPendingWaves50KG> pending_;
    std::shared_ptr<const OriginalUiGarageActors50GN::RootInputs> actorInputs_;
    std::shared_ptr<const OriginalUiGarageWattoOwner50KG::RootInputs> wattoInputs_;
    // Stable owner addresses are retained BEFORE any counted constructors.
    std::unique_ptr<OriginalUiGarageActors50GN> actors_;
    std::shared_ptr<OriginalUiGarageWattoOwner50KG> watto_;
    bool published_=false,pageAttempted_=false,pageSceneAttempted_=false,actorsAttempted_=false,actorsFinished_=false;
    bool wattoAttempted_=false,failed_=false,retiring_=false;
    // Actual base-manager payload. Constructor2A3A58/60 initializes148/14C;
    //144 and150 remain unknown until their source stores. The array is native
    //pointer-sized, not a fabricated guest address table. Only authenticated
    //UIBase.TotalPages==1 is admitted by this fresh Garage continuation.
    using Component=game::OriginalWreckComponent50LA;
    std::unique_ptr<Component*[]> managerPageTable144_;
    std::int32_t managerPageCount148_=0,managerSelected150_=0;
    Component* managerCurrentPage14C_=nullptr;
    bool managerPage144Known_=false,managerPage0Stored_=false,manager150Known_=false;
    bool managerPageTableReleased2A3B04_=false;
    // Actual owned temporary DAT298, distinct from immutable cold metadata.
    std::unique_ptr<assets::OriginalUiDat50KG> managerDat298_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> managerArchiveLease_;
    std::shared_ptr<assets::OriginalStandardArchiveProvider50LA> managerProviderLease_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> managerDomainLease_;
    std::uint64_t managerScopeEntryRevision_=0;
    bool managerArchiveOnlyScope170_=false;
    //1D1890 initialized190 null; all authenticated Widget OnTop producers
    //are rejected unless false. Only this genuinely empty list is admitted.
    bool managerDeferred190Empty_=true;
    enum class ManagerLoadPhase : std::uint8_t {
        notStarted,tableAllocated,pageHidden,deferredReturned,textureCallbackReturned,
        datReleased,pageSelected,pageActivated,pageEntered,resourcesReturned,
        scopeReturned,soundReturned,published,failed
    };
    ManagerLoadPhase managerLoadPhase_=ManagerLoadPhase::notStarted;
    // Distinct from this native custody owner's published_ lifetime.
    bool managerPublished1B8274_=false;
    // Canonical reached base-manager fields, produced with the actual2A39FC
    // constructor below, never inferred from a later successful Page entry.
    std::uint32_t managerRestart17C_=0,managerLifecycle16C_=0,managerTransition170_=0;
    std::uint32_t managerField15C_=0,managerField174_=0;
    bool managerQueued17DE10_=false;
    // Native custody of the unchanged profile at the actual queued source
    // request, retained across retirement for its real successor only.
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> returnProfile17DE10_;
    OriginalUiRiderProfileStamp50KG returnProfileStamp17DE10_{};
    bool managerUpdateFieldsConstructed_=false;
    bool managerLifecycleCallbackPending_=false;
    bool updateManagerState2A3C80(D&)noexcept;
    // Neither2A39F0 nor1D1860 constructs164/168. Preserve unknown source
    // words until actual1CCF40/70 writes them; never seed current route13/1.
    std::optional<std::int32_t> managerPendingScreen164_,managerPendingPage168_;
    bool canRequestPage1D6130(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool requestPage1D6130(const OriginalUiGaragePageOwner50KG&,bool& written)noexcept;

    // Native continuation of ONE real prepared source publication. It cannot
    // authorize frame acceptance, timing commit, audio advance or rendering.
    enum class RuntimePhase:std::uint8_t {notStarted,shared470,shared47C,manager,managerChildren,managerReturned,dispatcherReturned,unavailable};
    RuntimePhase runtimePhase_=RuntimePhase::notStarted;
    std::shared_ptr<OriginalUiGarageDrawContribution50KG> drawContribution_;
    bool renderCustodyCurrent140230(std::uint64_t)const noexcept;
    // Actual enclosing invocation custody, separate from dispatcher history.
    // Only this cursor's own source callbacks can advance these phases.
    enum class RootRuntimePhase:std::uint8_t {notStarted,outerChildren,rootChildren,dispatcher,rootReturned,outerReturned,completed,suffixUnavailable,unavailable};
    RootRuntimePhase runtimeRootPhase_=RootRuntimePhase::notStarted;
    // The real suffix is not replayable after an earlier operation succeeds.
    // These retain the exact reached calls even if a later admission fails.
    enum class RootTailPhase:std::uint8_t {notStarted,renderReturned,rendererAdvanced,audioReturned,timingCommitted};
    RootTailPhase runtimeTailPhase_=RootTailPhase::notStarted;
    ::original_ui_source_timing50LH::Sample runtimeTailSample_;
    std::uint64_t runtimeTailPublication_=0,runtimeTailAudioBoundary_=0,runtimeTailAudioFrame_=0;
    std::uint64_t runtimeTailMainRevision_=0,runtimeTailMixerRevision_=0,runtimeTailGroupSequence_=0;
    std::uint32_t runtimeRootReturnPC_=0;
    bool runtimeTargetPublished140A68_=false;
    game::OriginalWreckComponent50LA* runtimeOuterChild_=nullptr;
    game::OriginalWreckComponent50LA* runtimeRootChild_=nullptr;
    std::uint64_t runtimeRootPublication_=0;
    std::uint32_t runtimeRootSourcePC_=0,runtimeLightReturnPC_=0;
    std::uint32_t runtimeRoot10ReturnPC_=0,runtimeOuter26CReturnPC_=0;
    std::shared_ptr<const OriginalUiGarageActors50GN::RuntimeFrame> runtimeFrame_;
    const game::OriginalWreckComponent50LA* runtimeManagerChild_=nullptr;
    std::uint32_t runtimeSourcePC_=0;
    // Historical returns of these exact callbacks, not a Root-frame receipt.
    std::uint32_t runtimeManagerReturnPC_=0,runtimeDispatcherReturnPC_=0;
    std::uint64_t runtimeVisitedChildren_=0,runtimeReturnedTextOuters_=0;
    bool ownersClosed_=false,consumersClosed_=false,backingRetired_=false,closed_=false;
public:
    std::shared_ptr<OriginalUiRiderProfileOwner50KG> profileOwnerForReturn17DE10()const noexcept;
    ~OriginalUiGarageSession50KG()noexcept;
    OriginalUiGarageSession50KG(const OriginalUiGarageSession50KG&)=delete;
    OriginalUiGarageSession50KG& operator=(const OriginalUiGarageSession50KG&)=delete;
    OriginalUiGarageSession50KG(OriginalUiGarageSession50KG&&)=delete;
    OriginalUiGarageSession50KG& operator=(OriginalUiGarageSession50KG&&)=delete;
};
}
