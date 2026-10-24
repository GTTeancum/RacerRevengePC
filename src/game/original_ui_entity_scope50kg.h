#pragma once
#include "original_application_entities50la.h"
#include "original_racer_model_scene50la.h"
#include "original_racer_animation_cold50la.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <limits>
#include <new>
#include <array>
#include <optional>

namespace original_main_entry50KG { struct Application; struct Frontend; struct Video; class RaceDisplayLifetime50LA; }
namespace rr::assets { class OriginalStandardArchiveProvider50LA; }
namespace rr::platform { class OriginalUiFrontend50KG; class OriginalUiGarageSession50KG;
class OriginalUiGarageSelectedModel50KG; class OriginalUiGarageActors50GN;
class OriginalUiGarageWattoOwner50KG; class OriginalUiGarageWattoSoundGroup50KG;
class OriginalUiGaragePodScene50KG;
class OriginalUiGarageSceneCollision50KG;
class OriginalUiGarageStaticModels50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiSharedDialogs50KG;
class OriginalUiMemoryCardConstructor50KG;
class OriginalUiGarageManagerSearch50KG;
class OriginalUiGarageUiLights50KG;
class OriginalUiGarageManagerFog50KG;
class OriginalUiGarageWidget1Owner50KG;
class OriginalUiGarageTextOverlay50KG;
class OriginalUiGarageMovementOwner50KG;
class OriginalUiGarageVisibility50KG; class OriginalUiGarageModelResources50KG; class OriginalUiGarageDrawContribution50KG; }
namespace rr::game {
class OriginalFrontendCameraCold50LA;
// Exact represented Entity prefixes, not an invented full retail UI forest.
//17C1D8 constructs1B7140 and17C21C attaches it to Application10. Each Garage
// manager1D1860 is prepared detached, then1B8268 attaches it to that dispatcher.
// The same dispatcher survives the native frontend's serial menu managers.
// No page, model or required service child is implicitly constructed here.
class OriginalUiEntityScope50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend struct ::original_main_entry50KG::Video;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
    friend class rr::platform::OriginalUiMemoryCardConstructor50KG;
    friend class rr::platform::OriginalUiFrontend50KG;
    friend class rr::platform::OriginalUiGarageSession50KG;
    friend class rr::platform::OriginalUiGarageSelectedModel50KG;
public:
    enum class Status : std::uint8_t {complete,owner,affinity,phase,topology,reference,
        generation,locked,allocation,capacity};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;
        bool okay()const noexcept{return status==Status::complete;}};
private:
    using C=OriginalWreckComponent50LA;using K=OriginalWreckComponentKind50LA;
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    C dispatcher_,manager_;
    class OptionsConstruction;
    OptionsConstruction* optionsConstruction_=nullptr; // Exclusive native slot, never Garage authority.
    const rr::platform::OriginalUiGarageSession50KG* managerPageTableOwner_=nullptr;
    C** managerPageTable144Borrow_=nullptr;
    std::int32_t managerPageTableCount_=0;
    bool managerPageTableReleased_=false,managerBankRetired12E5E0_=false;

    // Exact constructor fields2A3350/54; load completion is separate history.
    std::int32_t dispatcherGlobalCount34_=0;
    C** dispatcherGlobalTable38_=nullptr;
    enum class GlobalWidgetLoadPhase : std::uint8_t {notAttempted,unresolved,absentReturned};
    GlobalWidgetLoadPhase globalWidgetLoadPhase_=GlobalWidgetLoadPhase::notAttempted;
    std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA> globalWidgetProvider_;
    std::uint64_t globalWidgetLookupRevision_=0;
    // Private source continuation, never a caller-issued completion token.
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> transitionDisplay_;
    std::uint64_t transitionManagerGeneration_=0;
    std::uint8_t transitionDisplayPhase_=0;
    bool transitionDisplayPinned_=false;

    using MemoryCardConstructor=rr::platform::OriginalUiMemoryCardConstructor50KG;
    const MemoryCardConstructor* detachedMemoryCardOwner_=nullptr;
    C* detachedMemoryCard_=nullptr;
    bool detachedMemoryCardRetiring_=false;
    Result constructDetachedMemoryCard1FEC80(MemoryCardConstructor&,C&)noexcept;
    bool detachedMemoryCardCurrent(const MemoryCardConstructor&,const C&,bool retiring)const noexcept;
    Result beginDetachedMemoryCardRetirement142590(MemoryCardConstructor&,C&)noexcept;
    Result finishDetachedMemoryCardRetirement1413E0(MemoryCardConstructor&,C&)noexcept;
    using SharedDialogs=rr::platform::OriginalUiSharedDialogs50KG;
    const SharedDialogs* sharedDialogsOwner_=nullptr;
    std::array<C*,2> sharedDialogs_{};
    std::array<bool,2> sharedDialogRetiring_{},sharedDialogRetired_{},sharedDialogEverAttached_{};
    std::uint8_t sharedDialogReorderOrdinal_=0;
    bool sharedDialogDetached_=false;
    Result constructSharedDialog203C90(SharedDialogs&,C&)noexcept;
    Result constructControllerPopup2054D0(SharedDialogs&,C&)noexcept;
    bool sharedDialogPrefixCurrent(const SharedDialogs&,std::size_t,const C&)const noexcept;
    Result reappendSharedDialog1B86D0(SharedDialogs&,std::size_t,C&)noexcept;
    Result beginSharedDialogRetirement142590(SharedDialogs&,std::size_t,C&)noexcept;
    bool sharedDialogRetirementCurrent(const SharedDialogs&,std::size_t,const C&)const noexcept;
    bool sharedDialogRetiredNodesAbsent(const SharedDialogs&)const noexcept;
    Result finishSharedDialogRetirement1413E0(SharedDialogs&,std::size_t,C&)noexcept;

    //Original1B8250 literal3AC6C0 →1D1950 →1CCBDC strcpy. Only the
    //nine bytes through NUL are source-initialized; the suffix is unclaimed.
    std::array<char,260> managerName194_{};
    std::size_t managerName194Known_=0;
    std::uint64_t managerName194Generation_=0,managerName194Epoch_=0;
    struct GarageChild {C* node;K kind;bool model;bool characterRetiring=false;bool wattoAnimationRetiring=false;bool soundGroupRetiring=false;std::int32_t soundGroupSlot=-1;bool wattoWidgetRetiring=false;bool wattoEmitterRetiring=false;bool staticListRetiring=false;bool toyListRetiring=false;};
    std::vector<GarageChild> garageChildren_;
    // Private native identity/last-reference ledgers, not source fields or
    // construction/readiness substitutes. Payload owners retain their bytes.
    struct PageSceneChild {
        C* node=nullptr;K kind=K::unbound;bool retiring=false,retired=false;
        const C* retirementParent=nullptr;
    };
    using ActorOwner=rr::platform::OriginalUiGarageActors50GN;
    const ActorOwner* actorLoopSoundOwner_=nullptr;
    C* actorLoopSoundToy_=nullptr;
    std::array<PageSceneChild,10> actorLoopSoundLists_{};
    using SceneOwner=rr::platform::OriginalUiGaragePodScene50KG;
    using PageOwner=rr::platform::OriginalUiGaragePageOwner50KG;
    using VisibilityOwner=rr::platform::OriginalUiGarageVisibility50KG;
    const VisibilityOwner* garageVisibilityOwner_=nullptr;
    C* garageVisibilityRoot_=nullptr;
    std::array<C*,3> garageVisibilityNodes_{};
    std::array<bool,3> garageVisibilityNodeRetiring_{},garageVisibilityNodeRetired_{};
    bool garageVisibilityRootRetiring_=false,garageVisibilityRootRetired_=false;

    const SceneOwner* garageSceneOwner_=nullptr;C* garageScene_=nullptr;
    bool garageSceneRetiring_=false;
    std::array<PageSceneChild,2> sceneLights_{};
    C* sceneLightsParent_=nullptr;
    std::array<PageSceneChild,3> sceneFog_{};
    PageSceneChild sceneParticle_{};
    using CollisionOwner=rr::platform::OriginalUiGarageSceneCollision50KG;
    const CollisionOwner* sceneCollisionOwner_=nullptr;C* sceneCollisionParent_=nullptr;
    //Raw27BB40 descriptor List3CC is not the positive query parent StaticModels3C4.
    C* sceneCollisionList3CC_=nullptr;
    std::array<PageSceneChild,30> sceneCollisions_{};
    std::array<PageSceneChild,7> sceneEmitters_{};C* sceneEmitterList_=nullptr;
    PageSceneChild sceneEmitterListLedger_{};
    PageSceneChild pageRouter_{};
    std::array<PageSceneChild,8> pageControls_{};
    std::array<PageSceneChild,3> pageGroups_{};
    struct PageSceneAttachment {C* node=nullptr;std::uint32_t mask=0;bool retired=false;};
    std::array<PageSceneAttachment,10> pageSceneAttachments_{};
    std::size_t pageSceneAttachmentCount_=0;
    //Exact native failed1B4A38 prefix: actual rows0..5 are attached, not10.
    //This receipt never publishes a normal Page or invents a slot6 attachment.
    const PageOwner* partialPageFailureOwner_=nullptr;
    const SceneOwner* partialPageFailureScene_=nullptr;
    C* partialPageFailureRawList_=nullptr;
    bool partialPageFailureRawRetiring_=false,partialPageFailureRawRetired_=false;
    std::array<C*,2> partialPageFailureAudio_{};
    std::array<bool,2> partialPageFailureAudioRetiring_{},partialPageFailureAudioRetired_{};

    // Exact failed late borrow: all ten real initial children and Camera
    // retired, but neither material-sound list was ever attached to Page.
    const PageOwner* lateBorrowAudioPage_=nullptr;
    const SceneOwner* lateBorrowAudioScene_=nullptr;
    std::array<C*,2> lateBorrowAudioLists_{};
    std::array<bool,2> lateBorrowAudioRetiring_{},lateBorrowAudioRetired_{};

    //Only the authentic Garage1B4CA0 nonnull rows:3D8,3BC,3C0.
    //The earlier ten children and Camera keep their distinct lifetimes.
    std::array<PageSceneAttachment,3> pageLateSceneAttachments_{};
    std::size_t pageLateSceneAttachmentCount_=0;
    std::array<PageSceneChild,2> pageLateAudioLifetimes_{};
    PageSceneChild pageLightListLifetime_{};
    bool pageCollisionListRetiring_=false;
    OriginalFrontendCameraCold50LA* pageCameraOwner_=nullptr;
    C* pageCameraEntity_=nullptr;
    bool pageCameraRetiring_=false,pageCameraRetired_=false;


    std::uint64_t scopeGeneration_=0,managerGeneration_=0,managerEpoch_=0;
    bool dispatcherLive_=false,managerLive_=false,managerPublished_=false,closed_=false;
    // Actual2A35A4 detach precedes child142590 while manager ref remains1.
    bool managerForestRetiring_=false;
    // Actual dispatcher30 is published only AFTER141960 at1B8268 and
    // survives detach/child destruction until the caller's2A35C0 store.
    C* dispatcherCurrentManager30_=nullptr;
    std::uint64_t dispatcherCurrentManagerGeneration30_=0;
    //1B717C/80 construct-1.1CCF40/70 later copy actual manager pending
    // words, including unknown pre-first-request provenance, into48/44.
    std::optional<std::int32_t> dispatcherPreviousScreen44_,dispatcherPreviousPage48_;

    const rr::platform::OriginalUiGarageManagerSearch50KG* managerSearchOwner_=nullptr;
    C* managerSearchNode_=nullptr;
    const C* managerSearchSelected_=nullptr;
    bool managerSearchNodeRetiring_=false,managerSearchNodeRetired_=false;
    bool managerRetiring_=false,managerSearchPayloadReleased_=false;
    const rr::platform::OriginalUiGarageUiLights50KG* managerUiLightsOwner_=nullptr;
    std::array<C*,2> managerUiLights_{};
    std::array<bool,2> managerUiLightsRetiring_{},managerUiLightsRetired_{};
    bool managerUiLightsPayloadReleased_=false;
    const rr::platform::OriginalUiGarageManagerFog50KG* managerFogOwner_=nullptr;
    std::array<C*,3> managerFog_{};
    std::array<bool,3> managerFogPublished_{},managerFogAttached_{},managerFogRetiring_{},managerFogRetired_{};
    std::array<const C*,3> managerFogPreviousAtRetire_{};
    std::size_t managerFogAttachCursor_=0;
    bool managerFogPayloadReleased_=false;
    const rr::platform::OriginalUiGarageWidget1Owner50KG* managerWidget1Owner_=nullptr;
    const void* managerWidget1Table_=nullptr; //Exact one source154 allocation identity.
    struct WidgetTextChild {
        const rr::platform::OriginalUiGarageTextOverlay50KG* owner=nullptr;
        C* node=nullptr;bool attached=false,retiring=false,retired=false;
    };
    struct WidgetRow {
        C* outer=nullptr;
        const rr::platform::OriginalUiGarageMovementOwner50KG* movement=nullptr;
        const rr::platform::OriginalUiGarageWattoOwner50KG* watto=nullptr;
        // Exact constructor children, never text-leaf stand-ins or new refs.
        std::array<C*,3> wattoChildren{};C* wattoModel=nullptr;
        std::array<WidgetTextChild,4> children{};
        std::size_t childCount=0;
        bool hidden=false,published=false,attached=false,retiring=false,retired=false,empty=false;
    };
    //Native identity ledger only, not new guest fields or a second DAT/table.
    std::array<WidgetRow,22> widgetRows_{};
    const rr::platform::OriginalUiGaragePageOwner50KG* garagePageOwner_=nullptr;
    C* garagePage_=nullptr;bool garagePageRetiring_=false,garagePageAttached_=false;
    // Comparison-only identity; never dereference after Page1413E0.
    const PageOwner* completedManagerPage1413E0_=nullptr;
    std::uint64_t completedManagerPageGeneration_=0,completedManagerPageEpoch_=0;
    // Separate native failed-loader custody. Never a normal attached-Page receipt.
    const PageOwner* failedSharedPageOwner_=nullptr;
    std::uint64_t failedSharedPageGeneration_=0,failedSharedPageEpoch_=0;
    bool failedSharedPageRetired_=false;
    const PageOwner* wattoBindingPageRetirementOwner_=nullptr;
    const PageOwner* completedWattoBindingPageOwner_=nullptr;
    std::uint64_t completedWattoBindingPageGeneration_=0,completedWattoBindingPageEpoch_=0;
    const PageOwner* loadedHidePageRetirementOwner_=nullptr;
    const PageOwner* completedLoadedHidePageOwner_=nullptr;
    std::uint64_t completedLoadedHidePageGeneration_=0,completedLoadedHidePageEpoch_=0;
    const PageOwner* lateBorrowPageRetirementOwner_=nullptr;
    const PageOwner* completedLateBorrowPageOwner_=nullptr;
    std::uint64_t completedLateBorrowPageGeneration_=0,completedLateBorrowPageEpoch_=0;
    const PageOwner* completedPartialPageOwner_=nullptr;
    std::uint64_t completedPartialPageGeneration_=0,completedPartialPageEpoch_=0;

    const C* garagePagePreviousAtRetire_=nullptr;
    std::size_t widgetCursor_=0;
    bool widgetPopulationRetiring_=false;
    //Compatibility references preserve the exact historical row1 test aperture,
    //not a duplicated set of source/ownership state.
    C*& managerWidget1_=widgetRows_[0].outer;
    bool& managerWidget1Retiring_=widgetRows_[0].retiring;
    bool& managerWidget1Retired_=widgetRows_[0].retired;
    bool managerWidget1DetachedReleased_=false,managerWidget1TableReleased_=false;
    const rr::platform::OriginalUiGarageTextOverlay50KG*& managerTextOverlayOwner_=widgetRows_[0].children[0].owner;
    C*& managerTextOverlay_=widgetRows_[0].children[0].node;
    bool& managerTextOverlayRetiring_=widgetRows_[0].children[0].retiring;
    bool& managerTextOverlayRetired_=widgetRows_[0].children[0].retired;
    bool& managerWidget1Hidden_=widgetRows_[0].hidden;
    bool& managerWidget1RowPublished_=widgetRows_[0].published;
    bool& managerWidget1Attached_=widgetRows_[0].attached;
    static K fogKind(std::size_t ordinal)noexcept {
        return ordinal==0?K::fog13E6B0:ordinal==1?K::fogOff13F850:ordinal==2?K::fogOn13F980:K::unbound;
    }

    static std::uint64_t key(const void* p)noexcept{
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));}
    OriginalUiEntityScope50KG(const ::original_main_entry50KG::Application* issuer,
        std::shared_ptr<OriginalApplicationEntities50LA> app)noexcept
        :application_(std::move(app)),issuer_(issuer),scopeGeneration_(key(this)){}
    Result live(bool allowManagerPayload=false,bool allowOptionsConstruction=false)const noexcept {
        if(!issuer_||!application_)return {Status::owner,0x17C1D8};
        if(application_->nativeOwnerThread_!=std::this_thread::get_id())return {Status::affinity,0x17C1D8};
        if(!application_->constructed_||application_->released_||closed_)
            return {Status::owner,0x17C1D8};
        if(application_->attachmentLock423F58_)return {Status::locked,0x14197C};
        if(optionsConstruction_&&!allowOptionsConstruction)return {Status::phase,0};
        if(dispatcherLive_&&(dispatcher_.kind!=K::uiDispatcher1B7140||
           dispatcher_.sessionGeneration!=scopeGeneration_||dispatcher_.objectKey!=key(&dispatcher_)||
           dispatcher_.parent1C!=&application_->root10_))return {Status::generation,0x17C21C};
        if(dispatcherLive_&&dispatcher_.reference00!=1)return {Status::reference,0x104970};
        if(managerLive_&&(manager_.kind!=K::uiGarageManager1D1860||
           manager_.sessionGeneration!=managerGeneration_||manager_.objectKey!=key(&manager_)||
           manager_.parent1C!=(managerPublished_?&dispatcher_:nullptr)))return {Status::generation,0x1B8268};
        if(managerForestRetiring_&&(!managerLive_||managerPublished_))return {Status::phase,0x2A35A4};
        if((managerPublished_||managerForestRetiring_)&&
           (dispatcherCurrentManager30_!=&manager_||dispatcherCurrentManagerGeneration30_!=managerGeneration_))
            return {Status::owner,0x1B8274};
        if(dispatcherCurrentManager30_&&(dispatcherCurrentManager30_!=&manager_||
           !dispatcherCurrentManagerGeneration30_))return {Status::owner,0x2A35C0};
        if(managerRetiring_&&!allowManagerPayload)return {Status::phase,0x1CCAA0};
        if(managerLive_&&manager_.reference00!=(managerRetiring_?0u:1u))return {Status::reference,0x104970};
        if(managerRetiring_&&contains(application_->entityAuthority_.view(),&manager_))
            return {Status::topology,0x142590};
        if(!original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))return {Status::topology,0x141984};
        return {Status::complete,0};
    }
    bool linked(const C& c,const C* parent)const noexcept {
        const auto domain=application_->entityAuthority_.view();
        if(!contains(domain,&c)||(parent&&!contains(domain,parent))||
           (c.previous10&&!contains(domain,c.previous10))||(c.next14&&!contains(domain,c.next14))||
           c.parent1C!=parent)return false;
        if(c.previous10){if(c.previous10->next14!=&c||c.previous10->parent1C!=parent)return false;}
        else if(parent&&parent->firstChild18!=&c)return false;
        if(c.next14&&(c.next14->previous10!=&c||c.next14->parent1C!=parent))return false;
        return true;
    }
    // Exact childless142590 link repair, then104970/1413E0. NOT141A70:
    // do not clear dying object's links or recompute ancestor service masks.
    Result releaseEmpty(C& c,K kind,std::uint64_t generation,const C* parent)noexcept {
        auto r=live();if(!r.okay())return r;
        if(c.kind!=kind||c.sessionGeneration!=generation||c.objectKey!=key(&c))return {Status::generation,0x142590};
        if(c.reference00!=1||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;--application_->objectCount423F60_;
        return {Status::complete,0x1413E0};
    }
    static Result construct(const ::original_main_entry50KG::Application* issuer,
        std::shared_ptr<OriginalApplicationEntities50LA> app,
        std::shared_ptr<OriginalUiEntityScope50KG>& out)noexcept {
        if(out||!issuer||!app)return {Status::owner,0x17C1D8};
        try {
            auto n=std::shared_ptr<OriginalUiEntityScope50KG>(new OriginalUiEntityScope50KG(issuer,std::move(app)));
            auto r=n->live();if(!r.okay()){n->closed_=true;return r;}
            auto& authority=n->application_->entityAuthority_;
            if(!authority.reserveAdditional(1)){n->closed_=true;return {Status::allocation,0x17C1C4};}
            if(!authority.canEnroll(n->dispatcher_,key(&n->dispatcher_))){n->closed_=true;return {Status::capacity,0x17C1D8};}
            const auto init=initializeOriginalWreckComponent50LA(n->dispatcher_,K::uiDispatcher1B7140,
                n->scopeGeneration_,key(&n->dispatcher_),true,n->application_->objectCount423F60_);
            if(init.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
            if(!authority.enroll(n->dispatcher_))std::terminate();
            n->dispatcherPreviousScreen44_=-1; //1B717C
            n->dispatcherPreviousPage48_=-1; //1B7180
            n->dispatcherLive_=true;
            const auto attach=authority.attachChildren(n->application_->root10_,n->dispatcher_,0xffffffffu,0);
            if(attach.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
            out=std::move(n);return {Status::complete,0x17C21C};
        }catch(...){return {Status::allocation,0x17C1C4};}
    }
    bool ownedGarageChild(const C& c)const noexcept {
        if(garageScene_==&c&&garageSceneRetiring_)return false;
        if(typedPageSceneChildRetiring(c))return false;
        if(garagePageRetiring_&&garagePage_==&c)return false;
        for(const auto& row:widgetRows_){
            if(row.outer==&c&&row.retiring)return false;
            for(const auto& child:row.children)if(child.node==&c&&child.retiring)return false;
        }
        for(std::size_t i=0;i<3;++i)if(managerFog_[i]==&c&&managerFogRetiring_[i])return false;
        for(std::size_t i=0;i<2;++i)if(managerUiLights_[i]==&c&&managerUiLightsRetiring_[i])return false;
        if(managerSearchNodeRetiring_&&managerSearchNode_==&c)return false;
        for(const auto& p:garageChildren_)if(p.node==&c)return !p.characterRetiring&&!p.wattoAnimationRetiring&&!p.soundGroupRetiring&&!p.wattoWidgetRetiring&&!p.wattoEmitterRetiring&&!p.staticListRetiring&&!p.toyListRetiring&&c.sessionGeneration==managerGeneration_&&
            c.kind==p.kind&&c.objectKey==key(&c)&&c.reference00==1;
        return false;
    }
    Result reserveGarageChildren(std::uint64_t generation,std::size_t count)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||managerPublished_||managerForestRetiring_||!garageChildren_.empty())
            return {Status::phase,0x1D1950};
        try{garageChildren_.reserve(count);}catch(...){return {Status::allocation,0x1D1950};}
        return application_->entityAuthority_.reserveAdditional(count)?Result{Status::complete,0}:
            Result{Status::allocation,0x1D1950};
    }
    bool modelUsesApplication(const OriginalRacerModelSceneOwner50LA& scene)const noexcept {
        return scene.binding_.actualObjectCount423F60==&application_->objectCount423F60_&&
            scene.binding_.actualAttachmentLock423F58==&application_->attachmentLock423F58_&&
            scene.binding_.actualTypes383408==&application_->types383408_;
    }
    Result enrollGarageModel(std::uint64_t generation,OriginalRacerModelSceneOwner50LA& scene)noexcept {
        auto r=live();if(!r.okay())return r;
        auto& c=scene.uiScopeModelEntity8();
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||!modelUsesApplication(scene)||c.sessionGeneration!=generation||
           c.kind!=K::model2403A0||c.reference00!=1||c.previous10||c.next14||c.parent1C||c.firstChild18||
           garageChildren_.size()==garageChildren_.capacity())return {Status::phase,0x2403A0};
        // Model constructor owns its original key and counter contribution.
        // Its key must be the actual canonical Entity address for this grant.
        if(c.objectKey!=key(&c)||!application_->entityAuthority_.canEnroll(c,c.objectKey))
            return {Status::generation,0x2403A0};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::model2403A0,true});return {Status::complete,0x2403A0};
    }
    Result enrollConstructedWattoAnimation(std::uint64_t generation,OriginalRacerAnimationCold50LA& animation)noexcept {
        auto r=live();if(!r.okay())return r;
        auto& c=animation.entity220_;const auto& b=animation.binding_;
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           !animation.constructed_||animation.released_||animation.character290_||
           animation.fields_.self00!=&c||b.actualObjectCount423F60!=&application_->objectCount423F60_||
           b.actualAttachmentLock423F58!=&application_->attachmentLock423F58_||
           b.sessionGeneration!=generation||b.animationKey!=key(&c)||
           c.kind!=K::animation1536F0||c.sessionGeneration!=generation||c.objectKey!=key(&c)||
           c.reference00!=1||c.previous10||c.next14||c.parent1C||c.firstChild18||
           garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::generation,0x1536F0};
        //1536F0 already owns this exact counter contribution. Enrollment adds
        // no second Entity and admits neither a derived290 nor another cache.
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::animation1536F0,false});return {Status::complete,0x1536F0};
    }
    Result constructGaragePrefix(std::uint64_t generation,C& c,K kind,bool sourceA1=true)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           (kind!=K::character11A750&&kind!=K::toyList2D9B40&&kind!=K::animationManager101330&&
            kind!=K::entity141300&&kind!=K::staticModels27B610&&kind!=K::searchNode27F120&&kind!=K::wattoWidget1D4320&&kind!=K::wattoEmitter2921C0)||
           c.reference00||c.kind!=K::unbound||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::phase,0x141300};
        const auto init=initializeOriginalWreckComponent50LA(c,kind,generation,key(&c),sourceA1,
            application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,kind,false});return {Status::complete,0x141300};
    }
    Result attachGarageChild(std::uint64_t generation,C& parent,C& child,std::uint32_t mask)noexcept {
        auto r=live();if(!r.okay())return r;
        // The reached2795E0 prefix is never an attached Entity in this scope.
        if(child.kind==K::loopSoundList2795E0||parent.kind==K::loopSoundList2795E0)
            return {Status::phase,0x2797C0};
        if(child.kind==K::uiGaragePage1F9FF0||parent.kind==K::uiGaragePage1F9FF0)
            return {Status::phase,child.kind==K::uiGaragePage1F9FF0?0x1D1C40u:0x1B48A0u};
        if(child.kind==K::uiOverlayText2A8F00||parent.kind==K::uiOverlayText2A8F00||
           child.kind==K::uiTextOverlay29C850||parent.kind==K::uiTextOverlay29C850||
           child.kind==K::uiMovementControl2AAF40||parent.kind==K::uiMovementControl2AAF40||
           child.kind==K::wattoWidget1D4320)
            return {Status::phase,0x1CD9F8};
        if(child.kind==K::light146720)return {Status::phase,0x1CDC8C};
        if(child.kind==K::fog13E6B0||child.kind==K::fogOff13F850||child.kind==K::fogOn13F980||
           parent.kind==K::fog13E6B0||parent.kind==K::fogOff13F850||parent.kind==K::fogOn13F980)
            return {Status::phase,0x1CD080};
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||!ownedGarageChild(child)||
           (&parent!=&manager_&&!ownedGarageChild(parent))||child.parent1C||child.previous10||child.next14)
            return {Status::topology,0x141960};
        const auto step=application_->entityAuthority_.attachChildren(parent,child,mask,0);
        return step.status==OriginalWreckComponentStatus50LA::complete?Result{Status::complete,0x141960}:
            Result{Status::topology,step.sourcePC};
    }
    Result attachGarageModel(std::uint64_t generation,C& parent,OriginalRacerModelSceneOwner50LA& scene,std::uint32_t mask)noexcept {
        if(!modelUsesApplication(scene))return {Status::generation,0x141960};
        return attachGarageChild(generation,parent,scene.uiScopeModelEntity8(),mask);
    }
    void forgetGarageChild(C& c)noexcept {
        for(auto& row:pageSceneAttachments_)if(row.node==&c)row.retired=true;
        for(auto& row:pageLateSceneAttachments_)if(row.node==&c)row.retired=true;
        for(auto i=garageChildren_.begin();i!=garageChildren_.end();++i)if(i->node==&c){
            //Both normal27BB80/count retirement and the existing detached
            //partial-list destructor arrive here only after actual release.
            if(sceneCollisionList3CC_==&c)sceneCollisionList3CC_=nullptr;
            garageChildren_.erase(i);return;
        }
        std::terminate();
    }
    Result preflightGarageModelRetirement(std::uint64_t generation,OriginalRacerModelSceneOwner50LA& scene)const noexcept {
        auto r=live();if(!r.okay())return r;
        auto& c=scene.uiScopeModelEntity8();
        // Native predecessor borrowing enforces the source tail-first close.
        if(managerSearchOwner_&&managerSearchSelected_==&c)return {Status::phase,0x27F180};
        if(!managerLive_||generation!=managerGeneration_||!modelUsesApplication(scene)||!ownedGarageChild(c)||
           !application_->entityAuthority_.canRetire(c))return {Status::generation,0x142590};
        const auto checked=scene.preflightModelReference142590InAuthority(application_->entityAuthority_.view());
        if(!checked.okay())return {Status::topology,checked.sourcePC};
        return {Status::complete,0x142590};
    }
    Result retireGarageModel(std::uint64_t generation,OriginalRacerModelSceneOwner50LA& scene)noexcept {
        const auto checked=preflightGarageModelRetirement(generation,scene);
        if(!checked.okay())return checked;
        auto& c=scene.uiScopeModelEntity8();
        if(!application_->entityAuthority_.retire(c))std::terminate();
        if(!scene.releasePreflightedModelReference142590().okay())std::terminate();
        forgetGarageChild(c);return {Status::complete,0x142654};
    }
    Result retireGaragePrefix(std::uint64_t generation,C& c,K kind,const C* parent)noexcept {
        if(garageScene_==&c||typedPageSceneChild(c))return {Status::phase,0x142590};
        if(kind==K::uiGaragePage1F9FF0)return {Status::phase,0x1FA170};
        if(kind==K::uiOverlayText2A8F00||kind==K::uiTextOverlay29C850||kind==K::uiMovementControl2AAF40)return {Status::phase,0x2A8F70};
        if(kind==K::light146720)return {Status::phase,0x146980};
        if(kind==K::fog13E6B0||kind==K::fogOff13F850||kind==K::fogOn13F980)
            return {Status::phase,0x13E700};
        if(managerSearchOwner_&&managerSearchNode_==&c)return {Status::phase,0x27F180};
        if(kind==K::staticModels27B610)return {Status::phase,0x27B650};
        if(kind==K::toyList2D9B40)return {Status::phase,0x2D9B80};
        if(kind==K::animation1536F0)return {Status::phase,0x153F50};
        if(kind==K::soundGroup2350D0)return {Status::phase,0x235150};
        if(kind==K::wattoWidget1D4320)return {Status::phase,0x1D4480};
        if(kind==K::wattoEmitter2921C0)return {Status::phase,0x292220};
        if(generation!=managerGeneration_||!ownedGarageChild(c))return {Status::generation,0x142590};
        bool prefix=false;for(const auto& p:garageChildren_)if(p.node==&c)prefix=!p.model&&p.kind==kind;
        if(!prefix)return {Status::generation,0x142590};
        auto* manager=application_->garageAnimationManager3DF400_;
        const bool retiringManager=kind==K::animationManager101330&&application_->garageAnimationManagerLive_&&
            manager&&&manager->entity==&c;
        auto r=releaseEmpty(c,kind,generation,parent);
        if(r.okay()){if(retiringManager)application_->garageAnimationManagerLive_=false;forgetGarageChild(c);}return r;
    }
    // Character11A750 has one Entity290. Its last-reference stage precedes
    //153F50's actual motion/cache/table cleanup;1413E0 follows that cleanup.
    // Only the concrete actor owner may cross these private, typed stages.
    Result preflightCharacterRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::character11A750)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.characterRetiring&&p.kind==K::character11A750;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginCharacterRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightCharacterRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.characterRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishCharacterRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::character11A750||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.characterRetiring&&!p.model&&p.kind==K::character11A750;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        return {Status::complete,0x1413E0};
    }
    // Actual detached27B610:142590 children first,104970,27B650 wrappers,1413E0.
    Result preflightStaticListRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::staticModels27B610)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.staticListRetiring&&p.kind==K::staticModels27B610;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(parent||c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginStaticListRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightStaticListRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.staticListRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishStaticListRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::staticModels27B610||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.staticListRetiring&&!p.model&&p.kind==K::staticModels27B610;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        return {Status::complete,0x1413E0};
    }
    // Actual detached2D9B40:142590 children first,104970,2D9B80 records,1413E0.
    Result preflightToyListRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::toyList2D9B40)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.toyListRetiring&&p.kind==K::toyList2D9B40;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(parent||c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginToyListRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightToyListRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.toyListRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishToyListRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::toyList2D9B40||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.toyListRetiring&&!p.model&&p.kind==K::toyList2D9B40;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        if(actorLoopSoundToy_==&c)
            for(const auto& slot:actorLoopSoundLists_)if(slot.node&&!slot.retired)
                return {Status::phase,0x279620};
        --application_->objectCount423F60_;forgetGarageChild(c);
        if(actorLoopSoundToy_==&c){actorLoopSoundLists_={};actorLoopSoundToy_=nullptr;actorLoopSoundOwner_=nullptr;}
        return {Status::complete,0x1413E0};
    }
    // Standalone Watto1536F0 Entity220, not the derived character290 path.
    // The concrete owner runs actual153F50 between these exact stages.
    Result preflightWattoAnimationRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::animation1536F0)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.wattoAnimationRetiring&&p.kind==K::animation1536F0;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginWattoAnimationRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightWattoAnimationRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.wattoAnimationRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishWattoAnimationRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::animation1536F0||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.wattoAnimationRetiring&&!p.model&&p.kind==K::animation1536F0;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        return {Status::complete,0x1413E0};
    }
    Result constructWattoSoundGroup2350D0(std::uint64_t generation,C& c,std::int32_t& slotOut)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||c.reference00||c.kind!=K::unbound||
           garageChildren_.size()==garageChildren_.capacity()||
           application_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::phase,0x2350D0};
        for(const auto* group:application_->soundGroups434A10_)if(group==&c)return {Status::generation,0x2351F0};
        std::size_t slot=0;while(slot<application_->soundGroups434A10_.size()&&(application_->soundGroups434A10_[slot]||application_->soundGroupReserved434A10_[slot]))++slot;
        if(slot==application_->soundGroups434A10_.size())return {Status::capacity,0x2351F0};
        // Concrete group has preallocated real pointer backing. No fallible
        // work or callback lies between actual Entity construction and claim.
        const auto initialized=initializeOriginalWreckComponent50LA(c,K::soundGroup2350D0,
            generation,key(&c),true,application_->objectCount423F60_);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        application_->soundGroups434A10_[slot]=&c;
        garageChildren_.push_back({&c,K::soundGroup2350D0,false,false,false,false,static_cast<std::int32_t>(slot)});
        slotOut=static_cast<std::int32_t>(slot);return {Status::complete,0x2351F0};
    }
    bool wattoSoundGroupCurrent(std::uint64_t generation,const C& c,std::int32_t slot)const noexcept {
        if(!live().okay()||!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::soundGroup2350D0||slot<0||static_cast<std::size_t>(slot)>=application_->soundGroups434A10_.size()||
           application_->soundGroups434A10_[slot]!=&c)return false;
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.soundGroupRetiring&&p.kind==K::soundGroup2350D0&&p.soundGroupSlot==slot;
        if(!exact)return false;
        for(std::size_t i=0;i<application_->soundGroups434A10_.size();++i)
            if(i!=static_cast<std::size_t>(slot)&&application_->soundGroups434A10_[i]==&c)return false;
        return true;
    }
    bool noteWattoSoundInsert235270(std::uint64_t generation,const C& c,std::int32_t slot)noexcept {
        if(!wattoSoundGroupCurrent(generation,c,slot))return false;
        if(!application_->soundInsertGuard881C_){
            application_->soundInsertAssertion434A88_=1;
            application_->soundInsertGuard881C_=1;
        }
        return true;
    }
    Result preflightWattoSoundGroupRetirement(std::uint64_t generation,const C& c,const C* parent,std::int32_t slot)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!wattoSoundGroupCurrent(generation,c,slot)||!application_->entityAuthority_.canRetire(c))
            return {Status::generation,0x235150};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x235150};
    }
    Result beginWattoSoundGroupRetirement(std::uint64_t generation,C& c,const C* parent,std::int32_t slot)noexcept {
        auto r=preflightWattoSoundGroupRetirement(generation,c,parent,slot);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        //23518C releases ONLY the privately recorded actual claimed slot,
        // before the concrete owner's2353D0 ordered Sound stop/release.
        for(auto& p:garageChildren_)if(p.node==&c){
            application_->soundGroups434A10_[p.soundGroupSlot]=nullptr;p.soundGroupRetiring=true;
            return {Status::complete,0x23518C};
        }
        std::terminate();
    }
    Result finishWattoSoundGroupRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::soundGroup2350D0||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.soundGroupRetiring&&!p.model&&p.kind==K::soundGroup2350D0&&p.soundGroupSlot>=0;
        if(!pending||c.reference00||c.firstChild18||contains(application_->entityAuthority_.view(),&c)||
           !application_->objectCount423F60_)return {Status::reference,0x1413E0};
        for(const auto* group:application_->soundGroups434A10_)if(group==&c)return {Status::phase,0x23518C};
        --application_->objectCount423F60_;forgetGarageChild(c);return {Status::complete,0x1413E0};
    }
    // Concrete owner releases actual payload between last reference and1413E0.
    Result preflightWattoWidgetRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        bool owned=ownedGarageChild(c);
        //The Widget loop admits row9 before its children run. That pending
        //row is deliberately not generally current, but its still-ref1 outer
        //must reach the real104970 after those children have been destroyed.
        const auto& row=widgetRows_[9];
        if(!owned&&garageManagerPageRetired1413E0(generation,completedManagerPage1413E0_)&&
           managerWidget1Owner_&&managerWidget1Table_&&!managerWidget1DetachedReleased_&&
           !managerWidget1TableReleased_&&widgetPopulationRetiring_&&
           manager_.reference00==1&&!manager_.parent1C&&!manager_.previous10&&!manager_.next14&&
           row.outer==&c&&row.watto&&!row.empty&&row.published&&row.attached&&row.retiring&&!row.retired&&
           static_cast<C* const*>(managerWidget1Table_)[9]==&c&&laterWidgetRowsRetired(9)&&
           parent==&manager_&&c.parent1C==parent&&c.mask20==0xffffffffu&&
           c.sessionGeneration==generation&&c.kind==K::wattoWidget1D4320&&c.objectKey==key(&c)&&
           c.reference00==1){
            bool childrenGone=row.wattoModel&&!contains(application_->entityAuthority_.view(),row.wattoModel);
            for(const auto* child:row.wattoChildren)
                if(!child||contains(application_->entityAuthority_.view(),child))childrenGone=false;
            for(const auto& p:garageChildren_)if(p.node==&c)
                owned=childrenGone&&!p.model&&p.kind==K::wattoWidget1D4320&&
                    !p.characterRetiring&&!p.wattoAnimationRetiring&&!p.soundGroupRetiring&&
                    !p.wattoWidgetRetiring&&!p.wattoEmitterRetiring&&!p.staticListRetiring&&!p.toyListRetiring;
        }
        if(!managerLive_||generation!=managerGeneration_||!owned||
           c.kind!=K::wattoWidget1D4320)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.wattoWidgetRetiring&&p.kind==K::wattoWidget1D4320;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||c.next14||!linked(c,parent))return {Status::topology,0x1425B8};
        if(widgetRows_[9].outer==&c&&(!widgetRows_[9].retiring||!laterWidgetRowsRetired(9)))
            return {Status::phase,0x1D4480};
        return {Status::complete,0x142590};
    }
    Result beginWattoWidgetRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightWattoWidgetRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.wattoWidgetRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishWattoWidgetRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::wattoWidget1D4320||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.wattoWidgetRetiring&&!p.model&&p.kind==K::wattoWidget1D4320;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        return {Status::complete,0x1413E0};
    }
    // Concrete owner releases actual payload between last reference and1413E0.
    Result preflightWattoEmitterRetirement(std::uint64_t generation,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::wattoEmitter2921C0)return {Status::generation,0x142590};
        bool exact=false;for(const auto& p:garageChildren_)if(p.node==&c)
            exact=!p.model&&!p.wattoEmitterRetiring&&p.kind==K::wattoEmitter2921C0;
        if(!exact||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginWattoEmitterRetirement(std::uint64_t generation,C& c,const C* parent)noexcept {
        auto r=preflightWattoEmitterRetirement(generation,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;
        for(auto& p:garageChildren_)if(p.node==&c){p.wattoEmitterRetiring=true;return {Status::complete,0x104970};}
        std::terminate();
    }
    Result finishWattoEmitterRetirement(std::uint64_t generation,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::wattoEmitter2921C0||c.objectKey!=key(&c))return {Status::generation,0x1413E0};
        bool pending=false;for(const auto& p:garageChildren_)if(p.node==&c)
            pending=p.wattoEmitterRetiring&&!p.model&&p.kind==K::wattoEmitter2921C0;
        if(!pending||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        return {Status::complete,0x1413E0};
    }
    using SearchOwner=rr::platform::OriginalUiGarageManagerSearch50KG;
    Result bindManagerSearchPayload(std::uint64_t generation,const SearchOwner* owner,const C& selected)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||managerSearchOwner_||
           !ownedGarageChild(selected)||selected.kind!=K::model2403A0||selected.mask20!=2u||
           selected.parent1C!=&manager_||selected.previous10||selected.next14||
           manager_.firstChild18!=&selected)return {Status::topology,0x1CCD18};
        managerSearchOwner_=owner;managerSearchSelected_=&selected;
        return {Status::complete,0x1CCD18};
    }
    Result constructManagerSearchNode(std::uint64_t generation,const SearchOwner* owner,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerSearchOwner_||managerSearchNode_||!managerSearchSelected_||
           !ownedGarageChild(*managerSearchSelected_)||manager_.firstChild18!=managerSearchSelected_||
           managerSearchSelected_->next14||managerSearchSelected_->parent1C!=&manager_)
            return {Status::topology,0x1CCD44};
        r=constructGaragePrefix(generation,c,K::searchNode27F120,(manager_.flags29&1u)!=0);
        if(!r.okay())return r;
        // No allocation/callback occurs between construction and this exact
        // source attachment. All topology and lock checks precede mutation.
        managerSearchNode_=&c; // actual manager2A4 store at 1CCD54, before attach
        const auto attached=application_->entityAuthority_.attachChildren(manager_,c,2u,0);
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
        return {Status::complete,0x1CCD60};
    }
    const C* managerSearchNode2A4(std::uint64_t generation,const SearchOwner* owner)const noexcept {
        return owner&&owner==managerSearchOwner_&&generation==managerGeneration_&&live().okay()
            ?managerSearchNode_:nullptr;
    }
    Result releaseUnconstructedSearchPredecessor(std::uint64_t generation,const SearchOwner* owner)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerSearchOwner_||generation!=managerGeneration_||managerSearchNode_||
           !managerSearchSelected_||!ownedGarageChild(*managerSearchSelected_)||
           manager_.firstChild18!=managerSearchSelected_||managerSearchSelected_->next14||
           managerSearchSelected_->parent1C!=&manager_)return {Status::phase,0x1CCD44};
        // Native partial-preparation unwind only: retain List2A0 until the
        // actual manager last-reference payload boundary; no source release here.
        managerSearchSelected_=nullptr;return {Status::complete,0x1CCD44};
    }
    Result preflightManagerSearchNodeRetirement(std::uint64_t generation,const SearchOwner* owner,const C& c)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(managerFogOwner_&&!managerFogPayloadReleased_)return {Status::phase,0x13E700};
        for(std::size_t i=0;i<2;++i)if(managerUiLights_[i]&&!managerUiLightsRetired_[i])
            return {Status::phase,0x146980};
        if(!owner||owner!=managerSearchOwner_||managerSearchNode_!=&c||managerSearchNodeRetiring_||
           managerSearchNodeRetired_||generation!=managerGeneration_||!ownedGarageChild(c)||
           c.kind!=K::searchNode27F120||!application_->entityAuthority_.canRetire(c))
            return {Status::reference,0x104970};
        if(!managerSearchSelected_||!ownedGarageChild(*managerSearchSelected_)||
           c.firstChild18||c.next14||c.previous10!=managerSearchSelected_||
           manager_.firstChild18!=managerSearchSelected_||!linked(c,&manager_))
            return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginManagerSearchNodeRetirement(std::uint64_t generation,const SearchOwner* owner,C& c)noexcept {
        auto r=preflightManagerSearchNodeRetirement(generation,owner,c);if(!r.okay())return r;
        c.previous10->next14=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;managerSearchNodeRetiring_=true;
        return {Status::complete,0x104970};
    }
    Result finishManagerSearchNodeRetirement(std::uint64_t generation,const SearchOwner* owner,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerSearchOwner_||managerSearchNode_!=&c||!managerSearchNodeRetiring_||
           managerSearchNodeRetired_||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=K::searchNode27F120||c.objectKey!=key(&c)||c.reference00||c.firstChild18||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        managerSearchNodeRetiring_=false;managerSearchNodeRetired_=true;
        // The selected model may now retire and free its canonical backing.
        managerSearchSelected_=nullptr;
        return {Status::complete,0x27F23C};
    }
    using UiLightsOwner=rr::platform::OriginalUiGarageUiLights50KG;
    Result bindGarageUiLightsPayload(std::uint64_t generation,const UiLightsOwner* owner,const SearchOwner* search)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||!search||search!=managerSearchOwner_||managerUiLightsOwner_||
           !managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           !managerSearchNode_||managerSearchNodeRetiring_||managerSearchNodeRetired_||
           !ownedGarageChild(*managerSearchNode_)||managerSearchNode_->next14||
           !linked(*managerSearchNode_,&manager_))return {Status::phase,0x1CDB10};
        managerUiLightsOwner_=owner;return {Status::complete,0x1CDB10};
    }
    Result constructGarageUiLight146720(std::uint64_t generation,const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerUiLightsOwner_||ordinal>=2||managerUiLights_[ordinal]||
           generation!=managerGeneration_||!managerLive_||managerPublished_||managerForestRetiring_||
           !managerSearchNode_||managerSearchNodeRetiring_||managerSearchNodeRetired_||
           !ownedGarageChild(*managerSearchNode_))return {Status::phase,0x1CDC08};
        const C* previous=ordinal?managerUiLights_[0]:managerSearchNode_;
        if(!previous||!ownedGarageChild(*previous)||previous->next14||!linked(*previous,&manager_)||
           c.reference00||c.kind!=K::unbound||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::topology,0x1CDC08};
        const auto init=initializeOriginalWreckComponent50LA(c,K::light146720,generation,key(&c),true,
            application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::light146720,false});managerUiLights_[ordinal]=&c;
        return {Status::complete,0x1CDC08};
    }
    Result attachGarageUiLight1CDC8C(std::uint64_t generation,const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerUiLightsOwner_||ordinal>=2||managerUiLights_[ordinal]!=&c||
           generation!=managerGeneration_||!ownedGarageChild(c)||c.kind!=K::light146720||
           c.parent1C||c.previous10||c.next14||c.firstChild18)return {Status::topology,0x1CDC8C};
        const C* previous=ordinal?managerUiLights_[0]:managerSearchNode_;
        if(!previous||!ownedGarageChild(*previous)||previous->next14||!linked(*previous,&manager_))
            return {Status::topology,0x1CDC8C};
        const auto result=application_->entityAuthority_.attachChildren(manager_,c,0u,0);
        return result.status==OriginalWreckComponentStatus50LA::complete?Result{Status::complete,0x1CDC8C}:
            Result{Status::topology,result.sourcePC};
    }
    Result preflightGarageUiLightRetirement(std::uint64_t generation,const UiLightsOwner* owner,std::size_t ordinal,const C& c)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(managerFogOwner_&&!managerFogPayloadReleased_)return {Status::phase,0x13E700};
        if(!owner||owner!=managerUiLightsOwner_||ordinal>=2||managerUiLights_[ordinal]!=&c||
           generation!=managerGeneration_||managerUiLightsRetiring_[ordinal]||managerUiLightsRetired_[ordinal]||
           !ownedGarageChild(c)||c.kind!=K::light146720||!application_->entityAuthority_.canRetire(c))
            return {Status::reference,0x104970};
        for(std::size_t i=ordinal+1;i<2;++i)if(managerUiLights_[i]&&!managerUiLightsRetired_[i])
            return {Status::phase,0x146980};
        if(c.firstChild18||c.next14||c.mask20)return {Status::topology,0x1425B8};
        if(c.parent1C){
            const C* previous=ordinal?managerUiLights_[0]:managerSearchNode_;
            if(c.previous10!=previous||!previous||!ownedGarageChild(*previous)||
               !linked(c,&manager_))return {Status::topology,0x1425B8};
        }else if(c.previous10||!linked(c,nullptr))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginGarageUiLightRetirement(std::uint64_t generation,const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=preflightGarageUiLightRetirement(generation,owner,ordinal,c);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;managerUiLightsRetiring_[ordinal]=true;return {Status::complete,0x104970};
    }
    Result finishGarageUiLightRetirement(std::uint64_t generation,const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerUiLightsOwner_||ordinal>=2||managerUiLights_[ordinal]!=&c||
           !managerUiLightsRetiring_[ordinal]||managerUiLightsRetired_[ordinal]||generation!=managerGeneration_||
           c.sessionGeneration!=generation||c.kind!=K::light146720||c.objectKey!=key(&c)||c.reference00||
           c.firstChild18||contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        managerUiLightsRetiring_[ordinal]=false;managerUiLightsRetired_[ordinal]=true;
        return {Status::complete,0x1469EC};
    }
    bool managerUiLightsPayloadRetirementCurrent(std::uint64_t generation,const UiLightsOwner* owner)const noexcept {
        return owner&&owner==managerUiLightsOwner_&&managerSearchPayloadReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteGarageUiLightsPayloadReleased(std::uint64_t generation,const UiLightsOwner* owner)noexcept {
        if(!managerUiLightsPayloadRetirementCurrent(generation,owner)||managerUiLightsPayloadReleased_)
            return {Status::phase,0x1CCB20};
        managerUiLightsPayloadReleased_=true;return {Status::complete,0x1CCB20};
    }
    using ManagerFogOwner=rr::platform::OriginalUiGarageManagerFog50KG;
    Result bindGarageManagerFogPayload(std::uint64_t generation,const ManagerFogOwner* owner,const UiLightsOwner* lights)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||!lights||lights!=managerUiLightsOwner_||managerFogOwner_||managerUiLightsPayloadReleased_||
           !managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_)
            return {Status::phase,0x1CCFA0};
        for(std::size_t i=0;i<2;++i){
            const auto* c=managerUiLights_[i];const auto* previous=i?managerUiLights_[0]:managerSearchNode_;
            if(!c||!previous||!ownedGarageChild(*c)||c->kind!=K::light146720||c->mask20||
               c->previous10!=previous||!linked(*c,&manager_)||(i==1&&c->next14))
                return {Status::topology,0x1CCFA0};
        }
        managerFogOwner_=owner;return {Status::complete,0x1CCFA0};
    }
    Result constructGarageManagerFogPrefix(std::uint64_t generation,const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerFogOwner_||ordinal>=3||managerFog_[ordinal]||managerFogPayloadReleased_||
           generation!=managerGeneration_||!managerLive_||managerPublished_||managerForestRetiring_||
           (ordinal&&(!managerFogPublished_[ordinal-1]||!managerFog_[ordinal-1]||!ownedGarageChild(*managerFog_[ordinal-1]))))
            return {Status::phase,0x1CCFA0};
        if(c.reference00||c.kind!=K::unbound||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::topology,0x141300};
        const auto kind=fogKind(ordinal);
        const auto init=initializeOriginalWreckComponent50LA(c,kind,generation,key(&c),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,kind,false});managerFog_[ordinal]=&c;
        return {Status::complete,ordinal==0?0x13E6B0u:ordinal==1?0x13F850u:0x13F980u};
    }
    Result publishGarageManagerFogField(std::uint64_t generation,const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerFogOwner_||ordinal>=3||managerFog_[ordinal]!=&c||managerFogPublished_[ordinal]||
           managerFogPayloadReleased_||generation!=managerGeneration_||!ownedGarageChild(c)||c.kind!=fogKind(ordinal)||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20)return {Status::phase,0x1CCFA0};
        // This is a native sequencing witness of the concrete owner's actual
        // source-field publication, not an attachment or page-ready claim.
        managerFogPublished_[ordinal]=true;return {Status::complete,0x1CCFA0};
    }
    bool managerFogWidgetCustodyCurrent(std::uint64_t generation,const ManagerFogOwner* owner,
        const rr::platform::OriginalUiGarageWidget1Owner50KG* widget)const noexcept {
        if(!live().okay()||!managerLive_||generation!=managerGeneration_||!owner||owner!=managerFogOwner_||
           !widget||widget!=managerWidget1Owner_||managerFogPayloadReleased_)return false;
        bool retiredAttached=false,liveAttached=false;
        for(std::size_t i=0;i<3;++i){const auto* c=managerFog_[i];
            if(!c||!managerFogPublished_[i]||managerFogRetiring_[i]||c->kind!=fogKind(i)||
               c->sessionGeneration!=generation||c->objectKey!=key(c)||c->firstChild18)return false;
            if(managerFogRetired_[i]){
                if(!managerFogAttached_[i]||c->reference00||c->parent1C!=&manager_||c->mask20!=0xffffffffu||
                   c->previous10!=managerFogPreviousAtRetire_[i]||c->next14||ownedGarageChild(*c)||
                   contains(application_->entityAuthority_.view(),c))return false;
                retiredAttached=true;
            }else{
                if(!ownedGarageChild(*c)||!application_->entityAuthority_.canRetire(*c)||
                   !linked(*c,managerFogAttached_[i]?&manager_:nullptr)||
                   c->mask20!=(managerFogAttached_[i]?0xffffffffu:0u)||
                   (!managerFogAttached_[i]&&(c->previous10||c->next14)))return false;
                if(managerFogAttached_[i])liveAttached=true;
            }
        }
        // A partial tail unwind must finish before the earlier widget tree.
        return !retiredAttached||!liveAttached;
    }
    bool managerFogAttachedCurrent(std::uint64_t generation,const ManagerFogOwner* owner,
        const rr::platform::OriginalUiGarageWidget1Owner50KG* widget)const noexcept {
        if(managerFogAttachCursor_!=3||!managerFogWidgetCustodyCurrent(generation,owner,widget))return false;
        for(std::size_t i=0;i<3;++i)if(!managerFogAttached_[i]||managerFogRetired_[i])return false;
        return true;
    }
    Result attachGarageManagerFog1CD080(std::uint64_t generation,const ManagerFogOwner* owner,
        const rr::platform::OriginalUiGarageWidget1Owner50KG* widget,std::size_t ordinal,C& c)noexcept {
        constexpr std::array<std::size_t,3> order{0,2,1};
        constexpr std::array<std::uint32_t,3> calls{0x1CD098,0x1CD0D8,0x1CD118};
        if(managerFogAttachCursor_>=3||ordinal!=order[managerFogAttachCursor_]||managerPublished_||managerForestRetiring_||
           !managerFogWidgetCustodyCurrent(generation,owner,widget)||widgetCursor_!=widgetRows_.size()||
           widgetPopulationRetiring_||managerWidget1DetachedReleased_||!managerWidget1Table_||
           managerFog_[ordinal]!=&c||managerFogAttached_[ordinal])return {Status::phase,0x1CD080};
        const C* tail=nullptr;
        for(std::size_t i=0;i<widgetRows_.size();++i){const auto& row=widgetRows_[i];
            if(!row.published||static_cast<C* const*>(managerWidget1Table_)[i]!=row.outer)return {Status::phase,0x1CDA34};
            if(emptyRow(i)){if(!row.empty||row.outer)return {Status::phase,0x1CDA10};}
            else{
                if(!row.outer||!row.hidden||!row.attached||!widgetRowCurrent(generation,widget,i,*row.outer))
                    return {Status::phase,0x1CD080};
                tail=row.outer;
            }
        }
        for(std::size_t i=0;i<managerFogAttachCursor_;++i){const auto prior=order[i];
            if(!managerFogAttached_[prior]||managerFogRetired_[prior])return {Status::phase,0x1CD080};
            tail=managerFog_[prior];
        }
        if(!tail||tail->next14||c.parent1C||c.previous10||c.next14||c.mask20)return {Status::topology,0x141960};
        const auto step=application_->entityAuthority_.attachChildren(manager_,c,0xffffffffu,0);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        const auto pc=calls[managerFogAttachCursor_++];managerFogAttached_[ordinal]=true;
        return {Status::complete,pc};
    }
    Result preflightGarageManagerFogRetirement(std::uint64_t generation,const ManagerFogOwner* owner,std::size_t ordinal,const C& c)const noexcept {
        if(garagePage_)return {Status::phase,0x1FA170};
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerFogOwner_||ordinal>=3||managerFog_[ordinal]!=&c||managerFogPayloadReleased_||
           generation!=managerGeneration_||managerFogRetiring_[ordinal]||managerFogRetired_[ordinal]||
           !ownedGarageChild(c)||c.kind!=fogKind(ordinal)||!application_->entityAuthority_.canRetire(c))
            return {Status::reference,0x104970};
        if(managerFogAttached_[ordinal]){
            constexpr std::array<std::size_t,3> order{0,2,1};bool seen=false;
            for(const auto i:order){if(i==ordinal)seen=true;
                else if(seen&&managerFogAttached_[i]&&!managerFogRetired_[i])return {Status::phase,0x142590};}
            if(c.mask20!=0xffffffffu||c.firstChild18||c.next14||!linked(c,&manager_))return {Status::topology,0x142590};
        }else{
            if(managerWidget1Owner_&&!managerWidget1DetachedReleased_)return {Status::phase,0x2A8F70};
            for(std::size_t i=ordinal+1;i<3;++i)if(managerFog_[i]&&!managerFogRetired_[i])return {Status::phase,0x13E700};
            if(c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||!linked(c,nullptr))
                return {Status::topology,0x142590};
        }
        return {Status::complete,0x142590};
    }
    Result beginGarageManagerFogRetirement(std::uint64_t generation,const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=preflightGarageManagerFogRetirement(generation,owner,ordinal,c);if(!r.okay())return r;
        managerFogPreviousAtRetire_[ordinal]=c.previous10;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;managerFogRetiring_[ordinal]=true;return {Status::complete,0x104970};
    }
    Result finishGarageManagerFogRetirement(std::uint64_t generation,const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerFogOwner_||ordinal>=3||managerFog_[ordinal]!=&c||!managerFogRetiring_[ordinal]||
           managerFogRetired_[ordinal]||generation!=managerGeneration_||c.sessionGeneration!=generation||
           c.kind!=fogKind(ordinal)||c.objectKey!=key(&c)||c.reference00||c.firstChild18||c.next14||
           c.parent1C!=(managerFogAttached_[ordinal]?&manager_:nullptr)||
           c.mask20!=(managerFogAttached_[ordinal]?0xffffffffu:0u)||c.previous10!=managerFogPreviousAtRetire_[ordinal]||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        managerFogRetiring_[ordinal]=false;managerFogRetired_[ordinal]=true;return {Status::complete,0x1413E0};
    }
    Result noteGarageManagerFogPayloadReleased(std::uint64_t generation,const ManagerFogOwner* owner)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerFogOwner_||managerFogPayloadReleased_||generation!=managerGeneration_||
           (managerWidget1Owner_&&!managerWidget1DetachedReleased_))
            return {Status::phase,0x13E700};
        for(std::size_t i=0;i<3;++i)if(managerFogRetiring_[i]||(managerFog_[i]&&!managerFogRetired_[i]))
            return {Status::phase,0x13E700};
        managerFogPayloadReleased_=true;return {Status::complete,0x13E700};
    }
    using Widget1Owner=rr::platform::OriginalUiGarageWidget1Owner50KG;
    using TextOverlayOwner=rr::platform::OriginalUiGarageTextOverlay50KG;
    using MovementOwner=rr::platform::OriginalUiGarageMovementOwner50KG;
    static bool textRow(std::size_t row)noexcept{return row==0||row==1||row==2||row==6||row==7||(row>=10&&row<22);}
    static bool emptyRow(std::size_t row)noexcept{return row==3||row==4||row==8;}
    static std::size_t textLines(std::size_t row)noexcept{return textRow(row)?(row>=10&&row<=15?4u:row==6||row==7?2u:1u):0u;}
    std::size_t widgetRowIndex(const C& outer)const noexcept {
        for(std::size_t i=0;i<widgetRows_.size();++i)if(widgetRows_[i].outer==&outer)return i;
        return widgetRows_.size();
    }
    bool widgetChildrenExact(const WidgetRow& row,const C& outer)const noexcept {
        const C* previous=nullptr;const C* next=outer.firstChild18;
        for(std::size_t i=0;i<row.childCount;++i){const auto& child=row.children[i];
            if(!child.attached||child.retiring||child.retired)continue;
            if(!child.node||next!=child.node||!contains(application_->entityAuthority_.view(),next)||
               !ownedGarageChild(*next)||!application_->entityAuthority_.canRetire(*next)||
               next->kind!=K::uiTextOverlay29C850||next->firstChild18||next->mask20!=0xffffffffu||
               next->parent1C!=&outer||next->previous10!=previous)return false;
            previous=next;next=next->next14;
        }
        return !next;
    }
    bool wattoWidgetChildrenExact(const WidgetRow& row,const C& outer)const noexcept {
        if(!row.watto||!row.wattoModel||outer.firstChild18!=row.wattoChildren[0])return false;
        constexpr std::array<K,3> kinds{K::soundGroup2350D0,K::animation1536F0,K::wattoEmitter2921C0};
        for(std::size_t i=0;i<row.wattoChildren.size();++i){const C* child=row.wattoChildren[i];
            if(!child||!contains(application_->entityAuthority_.view(),child)||!ownedGarageChild(*child)||
               !application_->entityAuthority_.canRetire(*child)||child->kind!=kinds[i]||
               child->parent1C!=&outer||child->previous10!=(i?row.wattoChildren[i-1]:nullptr)||
               child->next14!=(i+1<row.wattoChildren.size()?row.wattoChildren[i+1]:nullptr)||
               child->mask20!=(i==1?2u:0xffffffffu)||
               child->firstChild18!=(i==1?row.wattoModel:nullptr)||
               (child->flags29&3u)!=(row.hidden&&!(child->flags29&8u)?0u:3u))return false;
        }
        const auto& model=*row.wattoModel;
        return contains(application_->entityAuthority_.view(),&model)&&ownedGarageChild(model)&&
            application_->entityAuthority_.canRetire(model)&&model.kind==K::model2403A0&&
            model.parent1C==row.wattoChildren[1]&&!model.previous10&&!model.next14&&
            !model.firstChild18&&!model.mask20&&
            (model.flags29&3u)==(row.hidden&&!(row.wattoChildren[1]->flags29&8u)&&!(model.flags29&8u)?0u:3u);
    }
    bool widgetRowCurrent(std::uint64_t generation,const Widget1Owner* owner,std::size_t row,const C& outer)const noexcept {
        return !managerPublished_&&widgetRowIdentityCurrent(generation,owner,row,outer);
    }
    bool widgetRowIdentityCurrent(std::uint64_t generation,const Widget1Owner* owner,std::size_t row,const C& outer)const noexcept {
        if(!live().okay()||!managerLive_||generation!=managerGeneration_||
           !owner||owner!=managerWidget1Owner_||row>=widgetRows_.size())return false;
        const auto& r=widgetRows_[row];
        return r.outer==&outer&&!r.empty&&!r.retiring&&!r.retired&&ownedGarageChild(outer)&&
            application_->entityAuthority_.canRetire(outer)&&
            outer.kind==(row==5?K::uiMovementControl2AAF40:row==9?K::wattoWidget1D4320:K::uiOverlayText2A8F00)&&
            linked(outer,r.attached?&manager_:nullptr)&&outer.mask20==(r.attached?0xffffffffu:0u)&&
            (r.attached||(!outer.previous10&&!outer.next14))&&
            (row==9?wattoWidgetChildrenExact(r,outer):widgetChildrenExact(r,outer));
    }
    bool widgetPopulationCurrent(std::uint64_t generation,const Widget1Owner* owner,std::size_t row)const noexcept {
        if(!live().okay()||!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           !owner||owner!=managerWidget1Owner_||!managerWidget1Table_||
           managerWidget1DetachedReleased_||widgetPopulationRetiring_||row!=widgetCursor_||row>=widgetRows_.size()||
           !garageWidget1PredecessorsCurrent(generation,managerFogOwner_))return false;
        const C* expectedTail=nullptr;
        for(std::size_t i=0;i<row;++i){const auto& r=widgetRows_[i];
            if(emptyRow(i)){if(!r.empty||!r.published||r.outer||static_cast<C* const*>(managerWidget1Table_)[i])return false;}
            else {
                if(static_cast<C* const*>(managerWidget1Table_)[i]!=r.outer||!r.outer||!r.hidden||!r.published||!r.attached||!widgetRowCurrent(generation,owner,i,*r.outer))return false;
                expectedTail=r.outer;
            }
        }
        if(widgetRows_[row].attached)expectedTail=widgetRows_[row].outer;
        return !expectedTail||!expectedTail->next14;
    }
    Result bindGarageWattoRow1CD8B4(std::uint64_t generation,const Widget1Owner* owner,
        const rr::platform::OriginalUiGarageWattoOwner50KG* watto,C& outer,
        const C& group,const C& animation,const C& emitter,const C& model)noexcept {
        if(!widgetPopulationCurrent(generation,owner,9)||!watto||widgetRows_[9].outer||
           widgetRows_[9].published||!ownedGarageChild(outer)||
           !application_->entityAuthority_.canRetire(outer)||outer.kind!=K::wattoWidget1D4320||
           outer.parent1C||outer.previous10||outer.next14||outer.mask20)
            return {Status::phase,0x1CD8B4};
        WidgetRow candidate;candidate.outer=&outer;candidate.watto=watto;
        const auto enrolled=[&](const C& c)noexcept -> C* {
            for(const auto& entry:garageChildren_)if(entry.node==&c)return entry.node;return nullptr;
        };
        candidate.wattoChildren={enrolled(group),enrolled(animation),enrolled(emitter)};
        candidate.wattoModel=enrolled(model);
        if(!wattoWidgetChildrenExact(candidate,outer))return {Status::topology,0x1CD8B4};
        widgetRows_[9]=candidate;return {Status::complete,0x1CD8B4};
    }
    bool garageWattoRowRetirementCurrent(std::uint64_t generation,const Widget1Owner* owner,
        const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)const noexcept {
        return watto&&widgetRows_[9].watto==watto&&widgetRowCurrent(generation,owner,9,outer)&&
            laterWidgetRowsRetired(9)&&!outer.next14;
    }
    Result beginGarageWattoRowRetirement142590(std::uint64_t generation,const Widget1Owner* owner,
        const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)noexcept {
        if(!garageWattoRowRetirementCurrent(generation,owner,watto,outer))return {Status::phase,0x142590};
        widgetPopulationRetiring_=true;widgetRows_[9].retiring=true;return {Status::complete,0x142590};
    }
    Result noteGarageWattoRowRetired1D4480(std::uint64_t generation,const Widget1Owner* owner,
        const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)noexcept {
        auto r=live();if(!r.okay())return r;auto& row=widgetRows_[9];
        if(generation!=managerGeneration_||owner!=managerWidget1Owner_||!watto||row.watto!=watto||
           row.outer!=&outer||!row.retiring||row.retired||!laterWidgetRowsRetired(9)||outer.reference00||
           outer.firstChild18||outer.next14||outer.kind!=K::wattoWidget1D4320||
           contains(application_->entityAuthority_.view(),&outer)||ownedGarageChild(outer))
            return {Status::phase,0x1D4480};
        for(const auto* child:row.wattoChildren)
            if(!child||contains(application_->entityAuthority_.view(),child))
                return {Status::phase,0x1D4480};
        if(!row.wattoModel||contains(application_->entityAuthority_.view(),row.wattoModel))return {Status::phase,0x1D4480};
        widgetPopulationRetiring_=true;row.retiring=false;row.retired=true;return {Status::complete,0x1D4480};
    }
    Result constructGarageOverlayText2A8F00(std::uint64_t generation,const Widget1Owner* owner,std::size_t row,C& c)noexcept {
        if(!widgetPopulationCurrent(generation,owner,row)||!textRow(row)||widgetRows_[row].outer||
           widgetRows_[row].published||c.reference00||c.kind!=K::unbound)
            return {Status::phase,0x2A8F00};
        if(garageChildren_.size()==garageChildren_.capacity()||!application_->entityAuthority_.canEnroll(c,key(&c)))
            return {Status::capacity,0x141300};
        const auto init=initializeOriginalWreckComponent50LA(c,K::uiOverlayText2A8F00,generation,key(&c),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::uiOverlayText2A8F00,false});widgetRows_[row].outer=&c;
        return {Status::complete,0x2A8F00};
    }
    bool garageTextOverlayCurrent(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
        return !managerPublished_&&garageTextOverlayIdentityCurrent(generation,widget,owner,outer,child);
    }
    bool garageTextOverlayIdentityCurrent(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
        const auto index=widgetRowIndex(outer);
        if(!textRow(index)||!widgetRowIdentityCurrent(generation,widget,index,outer)||!owner)return false;
        const auto& row=widgetRows_[index];
        for(std::size_t i=0;i<row.childCount;++i){const auto& r=row.children[i];if(r.node!=&child)continue;
            return r.owner==owner&&!r.retiring&&!r.retired&&ownedGarageChild(child)&&
                application_->entityAuthority_.canRetire(child)&&child.kind==K::uiTextOverlay29C850&&
                !child.firstChild18&&linked(child,r.attached?&outer:nullptr)&&
                child.mask20==(r.attached?0xffffffffu:0u)&&
                (r.attached||(!child.previous10&&!child.next14));
        }
        return false;
    }
    Result constructGarageTextOverlay29C850(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,C& outer,C& child)noexcept {
        const auto index=widgetRowIndex(outer);
        if(!textRow(index)||!widgetPopulationCurrent(generation,widget,index)||
           !widgetRowCurrent(generation,widget,index,outer)||!owner)return {Status::phase,0x29C850};
        auto& row=widgetRows_[index];
        if(row.hidden||row.published||row.attached||row.childCount>=textLines(index))return {Status::phase,0x29C850};
        for(const auto& other:widgetRows_)for(const auto& c:other.children)
            if(c.owner==owner)return {Status::owner,0x29C850};
        for(std::size_t i=0;i<row.childCount;++i){const auto& r=row.children[i];
            if(!r.node||!r.attached||!garageTextOverlayCurrent(generation,widget,r.owner,outer,*r.node))return {Status::phase,0x29C850};}
        if(child.kind!=K::unbound||child.reference00||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(child,key(&child)))return {Status::capacity,0x141300};
        const auto init=initializeOriginalWreckComponent50LA(child,K::uiTextOverlay29C850,generation,key(&child),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(child))std::terminate();
        garageChildren_.push_back({&child,K::uiTextOverlay29C850,false});
        auto& r=row.children[row.childCount++];r.owner=owner;r.node=&child;
        return {Status::complete,0x29C850};
    }
    Result attachGarageTextOverlay2A9C64(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,C& outer,C& child)noexcept {
        const auto index=widgetRowIndex(outer);
        if(!garageTextOverlayCurrent(generation,widget,owner,outer,child)||
           !widgetPopulationCurrent(generation,widget,index)||child.parent1C||child.mask20)
            return {Status::topology,0x2A9C64};
        auto& row=widgetRows_[index];
        if(row.hidden||row.published||row.attached||!row.childCount||row.children[row.childCount-1].node!=&child)
            return {Status::phase,0x2A9C64};
        const auto a=application_->entityAuthority_.attachChildren(outer,child,0xffffffffu,0);
        if(a.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,a.sourcePC};
        row.children[row.childCount-1].attached=true;
        return {Status::complete,(index==7||(index>=10&&index<=15))?0x2A9710u:0x2A9C64u};
    }
    Result preflightGarageTextOverlayRetirement(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
        if(!garageTextOverlayCurrent(generation,widget,owner,outer,child))return {Status::generation,0x142590};
        const auto index=widgetRowIndex(outer);const auto& row=widgetRows_[index];
        if(!laterWidgetRowsRetired(index)||(row.attached&&outer.next14))return {Status::topology,0x1425D8};
        bool seen=false;for(std::size_t i=0;i<row.childCount;++i){const auto& r=row.children[i];
            if(r.node==&child)seen=true;else if(seen&&r.node&&!r.retired)return {Status::topology,0x1425D8};}
        if(child.next14)return {Status::topology,0x1425D8};
        return {Status::complete,0x142590};
    }
    Result beginGarageTextOverlayRetirement(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,C& outer,C& child)noexcept {
        auto r=preflightGarageTextOverlayRetirement(generation,widget,owner,outer,child);if(!r.okay())return r;
        //Exact142590 link repair; dying links and descendant masks stay stale.
        if(child.previous10)child.previous10->next14=child.next14;
        if(child.next14)child.next14->previous10=child.previous10;
        if(child.parent1C&&child.parent1C->firstChild18==&child)child.parent1C->firstChild18=child.next14;
        if(!application_->entityAuthority_.retire(child))std::terminate();
        --child.reference00;widgetPopulationRetiring_=true;
        for(auto& c:widgetRows_[widgetRowIndex(outer)].children)if(c.node==&child){c.retiring=true;break;}
        return {Status::complete,0x104970};
    }
    Result finishGarageTextOverlayRetirement(std::uint64_t generation,const Widget1Owner* widget,
        const TextOverlayOwner* owner,C& outer,C& child)noexcept {
        auto r=live();if(!r.okay())return r;const auto index=widgetRowIndex(outer);
        if(!textRow(index)||!widgetRowCurrent(generation,widget,index,outer)||!owner)return {Status::generation,0x29CA00};
        for(auto& c:widgetRows_[index].children)if(c.node==&child){
            if(c.owner!=owner||!c.retiring||c.retired||child.sessionGeneration!=generation||
               child.objectKey!=key(&child)||child.kind!=K::uiTextOverlay29C850||child.reference00||child.firstChild18||
               child.parent1C!=(c.attached?&outer:nullptr)||child.mask20!=(c.attached?0xffffffffu:0u)||
               child.next14||(!c.attached&&child.previous10)||
               contains(application_->entityAuthority_.view(),&child)||!application_->objectCount423F60_)
                return {Status::reference,0x29CA00};
            --application_->objectCount423F60_;forgetGarageChild(child);c.retiring=false;c.retired=true;
            return {Status::complete,0x1413E0};
        }
        return {Status::owner,0x29CA00};
    }
    Result hideGarageWidgetRow1CD908(std::uint64_t generation,const Widget1Owner* owner,std::size_t index,C& outer)noexcept {
        if(!widgetPopulationCurrent(generation,owner,index)||!widgetRowCurrent(generation,owner,index,outer))return {Status::phase,0x1CD908};
        auto& row=widgetRows_[index];
        if(row.hidden||row.published||row.attached||row.childCount!=textLines(index))return {Status::phase,0x1CD908};
        for(std::size_t i=0;i<row.childCount;++i){const auto& c=row.children[i];
            if(!c.attached||!c.node||!garageTextOverlayCurrent(generation,owner,c.owner,outer,*c.node))return {Status::topology,0x1415A0};}
        if(index==5&&outer.firstChild18)return {Status::topology,0x1415A0};
        // Watto, group, animation, model and emitter concrete +1C slots all
        // resolve to1415A0. Traverse the authenticated tree, preserving bit3's
        // skip of the ENTIRE child call/subtree, not merely that child's flags.
        if(index==9){
            const auto hide=[](C& c)noexcept {
                c.flags29=static_cast<std::uint8_t>(c.flags29&~1u);
                c.flags29=static_cast<std::uint8_t>(c.flags29&~2u);
            };
            hide(outer);
            for(std::size_t i=0;i<row.wattoChildren.size();++i){auto& child=*row.wattoChildren[i];
                if(child.flags29&8u)continue;hide(child);
                if(i==1&&!(row.wattoModel->flags29&8u))hide(*row.wattoModel);
            }
            row.hidden=true;return {Status::complete,0x1CD908};
        }
        //Text/Movement concrete vtables use1415A0 too.
        outer.flags29=static_cast<std::uint8_t>(outer.flags29&~1u);
        outer.flags29=static_cast<std::uint8_t>(outer.flags29&~2u);
        for(std::size_t i=0;i<row.childCount;++i){auto& c=*row.children[i].node;if(c.flags29&8u)continue;
            c.flags29=static_cast<std::uint8_t>(c.flags29&~1u);c.flags29=static_cast<std::uint8_t>(c.flags29&~2u);}
        row.hidden=true;return {Status::complete,0x1CD908};
    }
    Result activateGaragePageWidget90(std::uint64_t generation,const Widget1Owner* owner,
        const PageOwner& page,std::size_t index,C& outer)noexcept {
        //The private Widget caller proves the current actual Page90 group/
        //control cursor. This is distinct from later2A7A00 and selection78.
        const bool sourceRow=index<3||index==6||index==7||index==9||(index>=10&&index<=21);
        if(!sourceRow||managerPublished_||managerForestRetiring_||widgetPopulationRetiring_||
           widgetCursor_!=widgetRows_.size()||!garagePage_||!garageAttachedPageCurrent(generation,&page,*garagePage_)||
           !widgetRowCurrent(generation,owner,index,outer))return {Status::phase,0x2A5C30};
        auto& row=widgetRows_[index];
        if(row.empty||!row.published||!row.attached||row.childCount!=textLines(index))return {Status::owner,0x2A79E0};
        if(index!=9){
            if(outer.kind!=K::uiOverlayText2A8F00||row.watto)return {Status::owner,0x2A79E0};
            for(std::size_t i=0;i<row.childCount;++i){const auto& c=row.children[i];
                if(!c.node||!c.attached||c.retiring||c.retired||
                   !garageTextOverlayCurrent(generation,owner,c.owner,outer,*c.node))return {Status::topology,0x141630};
            }
        }else if(!row.watto||!wattoWidgetChildrenExact(row,outer))return {Status::topology,0x141630};
        const auto show=[](C& c)noexcept {
            c.flags29=static_cast<std::uint8_t>((c.flags29&~1u)|1u);
            c.flags29=static_cast<std::uint8_t>((c.flags29&~2u)|2u);
        };
        show(outer);
        if(index==9){
            for(std::size_t i=0;i<row.wattoChildren.size();++i){auto& child=*row.wattoChildren[i];
                if(child.flags29&8u)continue;show(child);
                if(i==1&&!(row.wattoModel->flags29&8u))show(*row.wattoModel);
            }
        }else for(std::size_t i=0;i<row.childCount;++i){auto& child=*row.children[i].node;
            if(!(child.flags29&8u))show(child);
        }
        row.hidden=false;return {Status::complete,0x141630};
    }
    Result applyGaragePageTextVisibility2A7A00(std::uint64_t generation,const Widget1Owner* owner,
        const PageOwner& page,std::size_t index,C& outer,bool shown)noexcept {
        //Only Widget's private exact Page-entry callback may call this private
        //grant method. That owner checks Page's actual phase/control cursor.
        const bool sourceRow=shown?(index>=10&&index<=21):(index==0||index==1||index==2||index==6);
        if(!sourceRow||managerPublished_||managerForestRetiring_||widgetPopulationRetiring_||
           widgetCursor_!=widgetRows_.size()||!garagePage_||!garageAttachedPageCurrent(generation,&page,*garagePage_)||
           !widgetRowCurrent(generation,owner,index,outer))return {Status::phase,0x2A7A00};
        auto& row=widgetRows_[index];
        if(row.empty||row.watto||!row.published||!row.attached||row.childCount!=textLines(index)||
           outer.kind!=K::uiOverlayText2A8F00)return {Status::owner,0x2A7A00};
        for(std::size_t i=0;i<row.childCount;++i){const auto& child=row.children[i];
            if(!child.node||!child.attached||child.retiring||child.retired||
               !garageTextOverlayCurrent(generation,owner,child.owner,outer,*child.node))
                return {Status::topology,shown?0x141630u:0x1415A0u};
        }
        const auto write=[shown](C& c)noexcept {
            c.flags29=static_cast<std::uint8_t>((c.flags29&~1u)|(shown?1u:0u));
            c.flags29=static_cast<std::uint8_t>((c.flags29&~2u)|(shown?2u:0u));
        };
        write(outer);
        //Source141630/1415A0 skips a child's entire virtual call on bit3.
        //All these concrete children are authentic leaf TextOverlay Entities.
        for(std::size_t i=0;i<row.childCount;++i){auto& child=*row.children[i].node;
            if(!(child.flags29&8u))write(child);
        }
        row.hidden=!shown;return {Status::complete,shown?0x141630u:0x1415A0u};
    }
    Result applyGarageRuntimePromptVisibility2A7A00(std::uint64_t generation,const Widget1Owner* owner,
        const PageOwner& page,std::size_t index,C& outer,bool shown)noexcept {
        // Widget authenticates the actual pending 1FAFA0/1FAFDC group-1
        // hook before this private call. These are its authored prompt rows,
        // not the cold entry or per-draw description visibility family.
        const bool sourceRow=index==0||index==1||index==2||index==6;
        if(!sourceRow||!publishedPopulatedGarageManagerCurrent(generation)||widgetPopulationRetiring_||
           widgetCursor_!=widgetRows_.size()||!garagePage_||!garageAttachedPageCurrent(generation,&page,*garagePage_)||
           !widgetRowIdentityCurrent(generation,owner,index,outer))return {Status::phase,0x2A7A00};
        auto& row=widgetRows_[index];
        if(row.empty||row.watto||!row.published||!row.attached||row.childCount!=textLines(index)||
           outer.kind!=K::uiOverlayText2A8F00)return {Status::owner,0x2A7A00};
        for(std::size_t i=0;i<row.childCount;++i){const auto& child=row.children[i];
            if(!child.node||!child.attached||child.retiring||child.retired||
               !garageTextOverlayIdentityCurrent(generation,owner,child.owner,outer,*child.node))
                return {Status::topology,shown?0x141630u:0x1415A0u};
        }
        const auto write=[shown](C& c)noexcept {
            c.flags29=static_cast<std::uint8_t>((c.flags29&~1u)|(shown?1u:0u));
            c.flags29=static_cast<std::uint8_t>((c.flags29&~2u)|(shown?2u:0u));
        };
        write(outer);
        //Source141630/1415A0 skips a child's entire virtual call on bit3.
        //All these concrete children are authentic leaf TextOverlay Entities.
        for(std::size_t i=0;i<row.childCount;++i){auto& child=*row.children[i].node;
            if(!(child.flags29&8u))write(child);
        }
        row.hidden=!shown;return {Status::complete,shown?0x141630u:0x1415A0u};
    }
    Result applyGarageDisplayTextVisibility2A7A00(std::uint64_t generation,const Widget1Owner* owner,
        const PageOwner& page,std::size_t index,C& outer,bool shown)noexcept {
        // Separate actual published Page display; cold entry remains unchanged.
        const bool sourceRow=index>=10&&index<=15;
        if(!sourceRow||!publishedPopulatedGarageManagerCurrent(generation)||widgetPopulationRetiring_||
           widgetCursor_!=widgetRows_.size()||!garagePage_||!garageAttachedPageCurrent(generation,&page,*garagePage_)||
           !widgetRowIdentityCurrent(generation,owner,index,outer))return {Status::phase,0x2A7A00};
        auto& row=widgetRows_[index];
        if(row.empty||row.watto||!row.published||!row.attached||row.childCount!=textLines(index)||
           outer.kind!=K::uiOverlayText2A8F00)return {Status::owner,0x2A7A00};
        for(std::size_t i=0;i<row.childCount;++i){const auto& child=row.children[i];
            if(!child.node||!child.attached||child.retiring||child.retired||
               !garageTextOverlayIdentityCurrent(generation,owner,child.owner,outer,*child.node))
                return {Status::topology,shown?0x141630u:0x1415A0u};
        }
        const auto write=[shown](C& c)noexcept {
            c.flags29=static_cast<std::uint8_t>((c.flags29&~1u)|(shown?1u:0u));
            c.flags29=static_cast<std::uint8_t>((c.flags29&~2u)|(shown?2u:0u));
        };
        write(outer);
        //Source141630/1415A0 skips a child's entire virtual call on bit3.
        //All these concrete children are authentic leaf TextOverlay Entities.
        for(std::size_t i=0;i<row.childCount;++i){auto& child=*row.children[i].node;
            if(!(child.flags29&8u))write(child);
        }
        row.hidden=!shown;return {Status::complete,shown?0x141630u:0x1415A0u};
    }
    Result publishGarageWidgetRow1CD918(std::uint64_t generation,const Widget1Owner* owner,std::size_t index,C& outer)noexcept {
        if(!widgetPopulationCurrent(generation,owner,index)||!widgetRowCurrent(generation,owner,index,outer))return {Status::phase,0x1CD918};
        auto& row=widgetRows_[index];
        if(!row.hidden||row.published||row.attached||
           static_cast<C* const*>(managerWidget1Table_)[index]!=&outer)return {Status::phase,0x1CD918};
        row.published=true;return {Status::complete,0x1CD918};
    }
    Result attachGarageWidgetRow1CD9F8(std::uint64_t generation,const Widget1Owner* owner,std::size_t index,C& outer)noexcept {
        if(!widgetPopulationCurrent(generation,owner,index)||!widgetRowCurrent(generation,owner,index,outer))return {Status::phase,0x1CD9F8};
        auto& row=widgetRows_[index];if(!row.hidden||!row.published||row.attached)return {Status::phase,0x1CD9F8};
        const auto a=application_->entityAuthority_.attachChildren(manager_,outer,0xffffffffu,0);
        if(a.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,a.sourcePC};
        row.attached=true;return {Status::complete,0x1CD9F8};
    }
    Result noteGarageEmptyRow1CDA10(std::uint64_t generation,const Widget1Owner* owner,std::size_t index,C* const* table)noexcept {
        if(!widgetPopulationCurrent(generation,owner,index)||!emptyRow(index)||table!=managerWidget1Table_||
           widgetRows_[index].outer||widgetRows_[index].published||table[index])return {Status::phase,0x1CDA10};
        widgetRows_[index].empty=true;widgetRows_[index].published=true;return {Status::complete,0x1CDA10};
    }
    Result advanceGarageWidgetRow1CDA1C(std::uint64_t generation,const Widget1Owner* owner,std::size_t index)noexcept {
        if(!widgetPopulationCurrent(generation,owner,index))return {Status::phase,0x1CDA1C};
        const auto& r=widgetRows_[index];
        if(!r.published||(emptyRow(index)?(!r.empty||r.outer):(!r.outer||!r.hidden||!r.attached||
           !widgetRowCurrent(generation,owner,index,*r.outer))))return {Status::phase,0x1CDA1C};
        ++widgetCursor_;return {Status::complete,0x1CDA1C};
    }
    Result hideGarageWidget1_1CD908(std::uint64_t g,const Widget1Owner* w,C& o,C& c)noexcept {
        if(widgetRows_[0].children[0].node!=&c)return {Status::owner,0x1CD908};return hideGarageWidgetRow1CD908(g,w,0,o);
    }
    Result publishGarageWidget1Row1_1CD918(std::uint64_t g,const Widget1Owner* w,C& o,C& c)noexcept {
        if(widgetRows_[0].children[0].node!=&c)return {Status::owner,0x1CD918};return publishGarageWidgetRow1CD918(g,w,0,o);
    }
    Result attachGarageWidget1_1CD9F8(std::uint64_t g,const Widget1Owner* w,C& o)noexcept {
        return attachGarageWidgetRow1CD9F8(g,w,0,o);
    }
    bool garageWidget1PredecessorsCurrent(std::uint64_t generation,const ManagerFogOwner* fog)const noexcept {
        if(!fog||fog!=managerFogOwner_||managerFogPayloadReleased_||!managerLive_||managerPublished_||managerForestRetiring_||
           generation!=managerGeneration_)return false;
        for(std::size_t i=0;i<3;++i){const auto* c=managerFog_[i];
            if(!c||!managerFogPublished_[i]||!ownedGarageChild(*c)||c->kind!=fogKind(i)||
               c->parent1C||c->previous10||c->next14||c->firstChild18||c->mask20||!linked(*c,nullptr))return false;
        }
        return true;
    }
    Result bindGarageWidget1Payload(std::uint64_t generation,const Widget1Owner* owner,const ManagerFogOwner* fog)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||managerWidget1Owner_||!garageWidget1PredecessorsCurrent(generation,fog))
            return {Status::phase,0x1CD7A0};
        managerWidget1Owner_=owner;return {Status::complete,0x1CD7A0};
    }
    Result publishGarageWidget1Table(std::uint64_t generation,const Widget1Owner* owner,const void* table)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerWidget1Owner_||!table||managerWidget1Table_||managerWidget1_||
           managerWidget1DetachedReleased_||!garageWidget1PredecessorsCurrent(generation,managerFogOwner_))
            return {Status::phase,0x1CD850};
        // Concrete owner has written count158=22 then allocated the actual
        // pointer array. No row is known/published by this native witness.
        managerWidget1Table_=table;return {Status::complete,0x1CD850};
    }
    Result constructGarageWidget1Prefix(std::uint64_t generation,const Widget1Owner* owner,C& c)noexcept {
        return constructGarageOverlayText2A8F00(generation,owner,0,c);
    }
    bool laterWidgetRowsRetired(std::size_t index)const noexcept {
        if(garagePage_)return false;
        for(std::size_t i=index+1;i<widgetRows_.size();++i)
            if(widgetRows_[i].outer&&!widgetRows_[i].retired)return false;
        return true;
    }
    Result preflightWidgetOuterRetirement(std::uint64_t generation,const Widget1Owner* owner,const C& c)const noexcept {
        const auto index=widgetRowIndex(c);
        if(!widgetRowCurrent(generation,owner,index,c))return {Status::generation,0x142590};
        const auto& row=widgetRows_[index];
        if(!laterWidgetRowsRetired(index)||c.firstChild18||c.next14)return {Status::topology,0x1425D8};
        for(const auto& child:row.children)if(child.node&&!child.retired)return {Status::phase,0x142590};
        return {Status::complete,0x142590};
    }
    Result beginWidgetOuterRetirement(std::uint64_t generation,const Widget1Owner* owner,C& c)noexcept {
        auto r=preflightWidgetOuterRetirement(generation,owner,c);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;widgetPopulationRetiring_=true;
        widgetRows_[widgetRowIndex(c)].retiring=true;return {Status::complete,0x104970};
    }
    Result preflightWidgetOuterRetirementFinish(std::uint64_t generation,const Widget1Owner* owner,const C& c,K kind)const noexcept {
        auto r=live();if(!r.okay())return r;const auto index=widgetRowIndex(c);
        if(!owner||owner!=managerWidget1Owner_||index>=widgetRows_.size()||generation!=managerGeneration_)
            return {Status::generation,0x1413E0};
        auto& row=widgetRows_[index];
        if(!row.retiring||row.retired||c.sessionGeneration!=generation||c.objectKey!=key(&c)||
           c.kind!=kind||c.reference00||c.firstChild18||c.next14||
           c.parent1C!=(row.attached?&manager_:nullptr)||c.mask20!=(row.attached?0xffffffffu:0u)||
           (!row.attached&&c.previous10)||contains(application_->entityAuthority_.view(),&c)||
           !application_->objectCount423F60_)return {Status::reference,0x1413E0};
        return {Status::complete,0x1413E0};
    }
    Result finishWidgetOuterRetirement(std::uint64_t generation,const Widget1Owner* owner,C& c,K kind)noexcept {
        auto r=preflightWidgetOuterRetirementFinish(generation,owner,c,kind);if(!r.okay())return r;
        auto& row=widgetRows_[widgetRowIndex(c)];
        --application_->objectCount423F60_;forgetGarageChild(c);row.retiring=false;row.retired=true;
        return {Status::complete,0x1413E0};
    }
    Result preflightGarageWidget1Retirement(std::uint64_t generation,const Widget1Owner* owner,const C& c)const noexcept {
        if(!textRow(widgetRowIndex(c)))return {Status::owner,0x2A8F70};
        return preflightWidgetOuterRetirement(generation,owner,c);
    }
    Result beginGarageWidget1Retirement(std::uint64_t generation,const Widget1Owner* owner,C& c)noexcept {
        if(!textRow(widgetRowIndex(c)))return {Status::owner,0x2A8F70};
        return beginWidgetOuterRetirement(generation,owner,c);
    }
    Result finishGarageWidget1Retirement(std::uint64_t generation,const Widget1Owner* owner,C& c)noexcept {
        if(!textRow(widgetRowIndex(c)))return {Status::owner,0x2A8FC4};
        auto r=finishWidgetOuterRetirement(generation,owner,c,K::uiOverlayText2A8F00);
        if(r.okay())r.sourcePC=0x2A8FC4;return r;
    }
    Result constructGarageMovement2AAF40(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        if(!movement||!widgetPopulationCurrent(generation,owner,5)||widgetRows_[5].outer||
           widgetRows_[5].movement||c.reference00||c.kind!=K::unbound)return {Status::phase,0x2AAF40};
        if(garageChildren_.size()==garageChildren_.capacity()||!application_->entityAuthority_.canEnroll(c,key(&c)))
            return {Status::capacity,0x141300};
        const auto init=initializeOriginalWreckComponent50LA(c,K::uiMovementControl2AAF40,generation,key(&c),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::uiMovementControl2AAF40,false});
        widgetRows_[5].outer=&c;widgetRows_[5].movement=movement;return {Status::complete,0x2AAF40};
    }
    bool garageMovementCurrent(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
        return !managerPublished_&&garageMovementIdentityCurrent(generation,owner,movement,c);
    }
    bool garageMovementIdentityCurrent(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
        return movement&&widgetRows_[5].movement==movement&&widgetRowIdentityCurrent(generation,owner,5,c)&&!c.firstChild18;
    }
    Result preflightGarageMovementRetirement(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
        if(!garageMovementCurrent(generation,owner,movement,c))return {Status::owner,0x2AAFA0};
        return preflightWidgetOuterRetirement(generation,owner,c);
    }
    Result beginGarageMovementRetirement(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        auto r=preflightGarageMovementRetirement(generation,owner,movement,c);if(!r.okay())return r;
        return beginWidgetOuterRetirement(generation,owner,c);
    }
    Result finishGarageMovementRetirement(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        if(!movement||widgetRows_[5].movement!=movement||widgetRows_[5].outer!=&c)return {Status::owner,0x2AAFA0};
        return finishWidgetOuterRetirement(generation,owner,c,K::uiMovementControl2AAF40);
    }
    Result hideGarageMovement1CD908(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        if(!garageMovementCurrent(generation,owner,movement,c))return {Status::owner,0x1CD908};
        return hideGarageWidgetRow1CD908(generation,owner,5,c);
    }
    Result publishGarageMovementRow1CD918(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        if(!garageMovementCurrent(generation,owner,movement,c))return {Status::owner,0x1CD918};
        return publishGarageWidgetRow1CD918(generation,owner,5,c);
    }
    Result attachGarageMovement1CD9F8(std::uint64_t generation,const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
        if(!garageMovementCurrent(generation,owner,movement,c))return {Status::owner,0x1CD9F8};
        return attachGarageWidgetRow1CD9F8(generation,owner,5,c);
    }
    Result noteGarageWidget1DetachedReleased(std::uint64_t generation,const Widget1Owner* owner)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerWidget1Owner_||generation!=managerGeneration_||managerWidget1DetachedReleased_)
            return {Status::phase,0x2A8F70};
        for(const auto& row:widgetRows_){
            if(row.retiring||(row.outer&&!row.retired))return {Status::phase,0x2A8F70};
            for(const auto& child:row.children)if(child.retiring||(child.node&&!child.retired))return {Status::phase,0x2A8F70};
        }
        //Includes failed allocation prefixes; the one154 table stays owned.
        widgetPopulationRetiring_=true;managerWidget1DetachedReleased_=true;return {Status::complete,0x2A8F70};
    }
    Result bindGarageManagerPageTable144(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner,C** table,std::int32_t count)noexcept {
        const auto r=live();if(!r.okay())return r;
        if(!owner||!table||count!=1||managerPageTableOwner_||managerPageTable144Borrow_||
           !managerLive_||managerPublished_||managerForestRetiring_||managerRetiring_||
           generation!=managerGeneration_||garagePage_||!managerWidget1Owner_||
           widgetCursor_!=widgetRows_.size()||widgetPopulationRetiring_||
           managerFogAttachCursor_!=3||managerWidget1DetachedReleased_)
            return {Status::phase,0x1CEC5C};
        managerPageTableOwner_=owner;managerPageTable144Borrow_=table;
        managerPageTableCount_=count;managerPageTableReleased_=false;
        return {Status::complete,0x1CEC5C};
    }
    Result beginFailedGarageSharedLoaderRetirement(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner,const PageOwner* page,const C& c)noexcept {
        if(!owner||owner!=managerPageTableOwner_||!page||page!=garagePageOwner_||garagePage_!=&c||
           failedSharedPageOwner_||managerPublished_||managerForestRetiring_||managerRetiring_||
           dispatcherCurrentManager30_||!managerPageTable144Borrow_||managerPageTableCount_!=1||
           managerPageTableReleased_||!garagePageCurrent(generation,page,c)||garagePageAttached_||
           c.parent1C||c.previous10||c.next14||c.firstChild18||garageScene_||garageSceneOwner_||
           pageCameraOwner_||pageRouter_.node||!populatedGarageManagerForestCurrent(generation,false))
            return {Status::phase,0x29B638};
        for(const auto& row:pageControls_)if(row.node)return {Status::phase,0x29B638};
        for(const auto& row:pageGroups_)if(row.node)return {Status::phase,0x29B638};
        failedSharedPageOwner_=page;failedSharedPageGeneration_=generation;
        failedSharedPageEpoch_=managerEpoch_;failedSharedPageRetired_=false;
        return {Status::complete,0x29B638};
    }
    bool failedGarageSharedLoaderPayloadCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&!managerPublished_&&!managerForestRetiring_&&
            !dispatcherCurrentManager30_&&failedSharedPageOwner_&&failedSharedPageRetired_&&
            failedSharedPageGeneration_==generation&&failedSharedPageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garageScene_&&!garageSceneOwner_&&
            managerSearchNodeRetired_&&!managerSearchSelected_&&
            managerUiLightsRetired_[0]&&managerUiLightsRetired_[1]&&
            managerFogPayloadReleased_&&managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    bool failedGaragePartialScenePayloadCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&!managerPublished_&&!managerForestRetiring_&&
            !dispatcherCurrentManager30_&&completedPartialPageOwner_&&!failedSharedPageOwner_&&
            completedPartialPageGeneration_==generation&&completedPartialPageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garageScene_&&!garageSceneOwner_&&
            !partialPageFailureOwner_&&managerSearchNodeRetired_&&!managerSearchSelected_&&
            managerUiLightsRetired_[0]&&managerUiLightsRetired_[1]&&
            managerFogPayloadReleased_&&managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteFailedPartialSceneBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!failedGaragePartialScenePayloadCurrent(generation,owner)||managerBankRetired12E5E0_||
           !managerSearchPayloadReleased_||!managerUiLightsPayloadReleased_)
            return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    bool failedGarageWattoBindingPayloadCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&!managerPublished_&&!managerForestRetiring_&&
            !dispatcherCurrentManager30_&&completedWattoBindingPageOwner_&&!failedSharedPageOwner_&&
            completedWattoBindingPageGeneration_==generation&&completedWattoBindingPageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garageScene_&&!garageSceneOwner_&&
            !partialPageFailureOwner_&&!completedPartialPageOwner_&&!wattoBindingPageRetirementOwner_&&
            !completedLoadedHidePageOwner_&&!loadedHidePageRetirementOwner_&&
            !completedLateBorrowPageOwner_&&!lateBorrowPageRetirementOwner_&&!lateBorrowAudioPage_&&
            managerSearchNodeRetired_&&!managerSearchSelected_&&
            managerUiLightsRetired_[0]&&managerUiLightsRetired_[1]&&
            managerFogPayloadReleased_&&managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    bool failedGarageLoadedHidePayloadCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&!managerPublished_&&!managerForestRetiring_&&
            !dispatcherCurrentManager30_&&completedLoadedHidePageOwner_&&!failedSharedPageOwner_&&
            completedLoadedHidePageGeneration_==generation&&completedLoadedHidePageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garageScene_&&!garageSceneOwner_&&
            !partialPageFailureOwner_&&!completedPartialPageOwner_&&!loadedHidePageRetirementOwner_&&
            !completedLateBorrowPageOwner_&&!lateBorrowPageRetirementOwner_&&!lateBorrowAudioPage_&&
            managerSearchNodeRetired_&&!managerSearchSelected_&&
            managerUiLightsRetired_[0]&&managerUiLightsRetired_[1]&&
            managerFogPayloadReleased_&&managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    bool failedGarageLateBorrowPayloadCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&!managerPublished_&&!managerForestRetiring_&&
            !dispatcherCurrentManager30_&&completedLateBorrowPageOwner_&&!failedSharedPageOwner_&&
            completedLateBorrowPageGeneration_==generation&&completedLateBorrowPageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garageScene_&&!garageSceneOwner_&&
            !partialPageFailureOwner_&&!completedPartialPageOwner_&&!lateBorrowPageRetirementOwner_&&
            managerSearchNodeRetired_&&!managerSearchSelected_&&
            managerUiLightsRetired_[0]&&managerUiLightsRetired_[1]&&
            managerFogPayloadReleased_&&managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteFailedWattoBindingBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!failedGarageWattoBindingPayloadCurrent(generation,owner)||managerBankRetired12E5E0_||
           !managerSearchPayloadReleased_||!managerUiLightsPayloadReleased_)
            return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    Result noteFailedLoadedHideBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!failedGarageLoadedHidePayloadCurrent(generation,owner)||managerBankRetired12E5E0_||
           !managerSearchPayloadReleased_||!managerUiLightsPayloadReleased_)
            return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    Result noteFailedLateBorrowBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!failedGarageLateBorrowPayloadCurrent(generation,owner)||managerBankRetired12E5E0_||
           !managerSearchPayloadReleased_||!managerUiLightsPayloadReleased_)
            return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    Result noteFailedGarageBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!failedGarageSharedLoaderPayloadCurrent(generation,owner)||managerBankRetired12E5E0_||
           !managerSearchPayloadReleased_||!managerUiLightsPayloadReleased_)
            return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    bool garageBankRetirement12E5E0Current(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&managerForestRetiring_&&
            managerPageTable144Borrow_&&managerPageTableCount_==1&&!managerPageTableReleased_&&
            !managerBankRetired12E5E0_&&completedManagerPage1413E0_&&
            completedManagerPageGeneration_==generation&&completedManagerPageEpoch_==managerEpoch_&&
            managerSearchPayloadReleased_&&managerUiLightsPayloadReleased_&&managerFogPayloadReleased_&&
            managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteGarageBankRetired12E5E0(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!garageBankRetirement12E5E0Current(generation,owner))return {Status::phase,0x12E5E0};
        managerBankRetired12E5E0_=true;return {Status::complete,0x12E5E0};
    }
    bool managerPageTableRetirementCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
        return owner&&owner==managerPageTableOwner_&&managerPageTable144Borrow_&&
            managerPageTableCount_==1&&!managerPageTableReleased_&&managerBankRetired12E5E0_&&
            managerFogPayloadReleased_&&managerUiLightsPayloadReleased_&&managerSearchPayloadReleased_&&
            managerWidget1DetachedReleased_&&!managerWidget1TableReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteGarageManagerPageTableReleased(std::uint64_t generation,
        const rr::platform::OriginalUiGarageSession50KG* owner)noexcept {
        if(!managerPageTableRetirementCurrent(generation,owner))return {Status::phase,0x2A3B04};
        managerPageTableReleased_=true;return {Status::complete,0x2A3B04};
    }
    bool managerWidget1TableRetirementCurrent(std::uint64_t generation,const Widget1Owner* owner)const noexcept {
        return owner&&owner==managerWidget1Owner_&&managerWidget1DetachedReleased_&&
            (!managerPageTableOwner_||managerPageTableReleased_)&&
            managerFogPayloadReleased_&&managerUiLightsPayloadReleased_&&managerSearchPayloadReleased_&&
            managerPayloadRetirementCurrent(generation,managerSearchOwner_);
    }
    Result noteGarageWidget1TableReleased(std::uint64_t generation,const Widget1Owner* owner)noexcept {
        if(!managerWidget1TableRetirementCurrent(generation,owner)||managerWidget1TableReleased_)
            return {Status::phase,0x2A3B18};
        //154 remains a stale identity if it was produced. This note does NOT
        // discharge unjoined443A60->44 effects in the base destructor.
        managerWidget1TableReleased_=true;return {Status::complete,0x2A3B18};
    }

    bool typedPageSceneChild(const C& c)const noexcept {
        for(const auto& x:actorLoopSoundLists_)if(x.node==&c)return true;
        if(garageVisibilityRoot_==&c)return true;
        for(const auto* node:garageVisibilityNodes_)if(node==&c)return true;
        if(sceneEmitterListLedger_.node==&c)return true;
        if(pageRouter_.node==&c||sceneParticle_.node==&c)return true;
        for(const auto& x:sceneEmitters_)if(x.node==&c)return true;
        for(const auto& x:sceneCollisions_)if(x.node==&c)return true;
        for(const auto& x:sceneLights_)if(x.node==&c)return true;
        for(const auto& x:sceneFog_)if(x.node==&c)return true;
        for(const auto& x:pageControls_)if(x.node==&c)return true;
        for(const auto& x:pageGroups_)if(x.node==&c)return true;
        return false;
    }
    bool typedPageSceneChildRetiring(const C& c)const noexcept {
        if(garageVisibilityRoot_==&c&&garageVisibilityRootRetiring_)return true;
        for(std::size_t i=0;i<garageVisibilityNodes_.size();++i)
            if(garageVisibilityNodes_[i]==&c&&garageVisibilityNodeRetiring_[i])return true;
        if(sceneEmitterListLedger_.node==&c&&sceneEmitterListLedger_.retiring)return true;
        for(const auto& x:sceneEmitters_)if(x.node==&c&&x.retiring)return true;
        for(const auto& x:sceneCollisions_)if(x.node==&c&&x.retiring)return true;
        if((pageRouter_.node==&c&&pageRouter_.retiring)||
           (sceneParticle_.node==&c&&sceneParticle_.retiring))return true;
        for(const auto& x:sceneLights_)if(x.node==&c&&x.retiring)return true;
        for(const auto& x:sceneFog_)if(x.node==&c&&x.retiring)return true;
        for(const auto& x:pageControls_)if(x.node==&c&&x.retiring)return true;
        for(const auto& x:pageGroups_)if(x.node==&c&&x.retiring)return true;
        return false;
    }
    Result constructPageSceneChild(std::uint64_t generation,PageSceneChild& slot,
        C& c,K kind,std::uint32_t pc)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||slot.node||
           c.reference00||c.kind!=K::unbound||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::phase,pc};
        const auto init=initializeOriginalWreckComponent50LA(c,kind,generation,key(&c),true,
            application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,kind,false});slot.node=&c;slot.kind=kind;
        return {Status::complete,pc};
    }
    bool pageSceneChildLive(const PageSceneChild& slot)const noexcept {
        return slot.node&&!slot.retiring&&!slot.retired&&ownedGarageChild(*slot.node)&&
            slot.node->kind==slot.kind&&slot.node->reference00==1&&
            application_->entityAuthority_.canRetire(*slot.node);
    }
    Result preflightPageSceneChildRetirement(std::uint64_t generation,
        const PageSceneChild& slot,const C& c,const C* parent)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(generation!=managerGeneration_||slot.node!=&c||!pageSceneChildLive(slot))
            return {Status::reference,0x104970};
        // Source142590 releases children before the parent's own104970.
        // Only immediate links are read; higher stale ancestry is not repaired.
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginPageSceneChildRetirement(std::uint64_t generation,
        PageSceneChild& slot,C& c,const C* parent)noexcept {
        auto r=preflightPageSceneChildRetirement(generation,slot,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;slot.retiring=true;slot.retirementParent=parent;
        return {Status::complete,0x104970};
    }
    Result finishPageSceneChildRetirement(std::uint64_t generation,
        PageSceneChild& slot,C& c,const C* parent)noexcept {
        auto r=live();if(!r.okay())return r;
        if(generation!=managerGeneration_||slot.node!=&c||!slot.retiring||slot.retired||
           slot.retirementParent!=parent||c.parent1C!=parent||c.reference00||
           c.kind!=slot.kind||c.sessionGeneration!=generation||c.objectKey!=key(&c)||
           c.firstChild18||contains(application_->entityAuthority_.view(),&c)||
           !application_->objectCount423F60_)return {Status::reference,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        slot.retiring=false;slot.retired=true;return {Status::complete,0x1413E0};
    }
    // Only the exact early KeyFramed reader may own these temporary lists.
    // This does not widen constructGaragePrefix, attach, or generic retirement.
    Result constructActorLoopSound2795E0(std::uint64_t generation,const ActorOwner& owner,
        std::size_t ordinal,C& toys,C& c)noexcept {
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           ordinal>=actorLoopSoundLists_.size()||!ownedGarageChild(toys)||
           toys.kind!=K::toyList2D9B40||toys.reference00!=1||toys.parent1C||
           toys.previous10||toys.next14||toys.firstChild18||toys.mask20||
           (actorLoopSoundOwner_&&actorLoopSoundOwner_!=&owner)||
           (actorLoopSoundToy_&&actorLoopSoundToy_!=&toys))return {Status::phase,0x2D8FB0};
        for(std::size_t i=0;i<ordinal;++i)
            if(!actorLoopSoundLists_[i].node||!actorLoopSoundLists_[i].retired)
                return {Status::phase,0x2D8FEC};
        for(std::size_t i=ordinal;i<actorLoopSoundLists_.size();++i)
            if(actorLoopSoundLists_[i].node)return {Status::phase,0x2D8FB0};
        const auto step=constructPageSceneChild(generation,actorLoopSoundLists_[ordinal],
            c,K::loopSoundList2795E0,0x2795E0);
        if(step.okay()){actorLoopSoundOwner_=&owner;actorLoopSoundToy_=&toys;}
        return step;
    }
    Result retireActorLoopSoundReadFalse142590(std::uint64_t generation,const ActorOwner& owner,
        std::size_t ordinal,const C& toys,C& c)noexcept {
        if(!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_||
           actorLoopSoundOwner_!=&owner||actorLoopSoundToy_!=&toys||
           ordinal>=actorLoopSoundLists_.size()||!ownedGarageChild(toys)||
           toys.kind!=K::toyList2D9B40||toys.reference00!=1||toys.parent1C||toys.firstChild18||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||c.descendants24||
           c.kind!=K::loopSoundList2795E0)return {Status::phase,0x2D8FE4};
        auto& slot=actorLoopSoundLists_[ordinal];
        const auto step=beginPageSceneChildRetirement(generation,slot,c,nullptr);
        if(!step.okay())return step;
        //279620 sees the owner's genuine null34 payload, then279668 calls
        //1413E0. No positive wrapper array or Aural callback is implied.
        return finishPageSceneChildRetirement(generation,slot,c,nullptr);
    }
    bool sceneOwnerCurrent(std::uint64_t generation,const SceneOwner* owner,bool allowRetiring)const noexcept {
        if(!live().okay()||!owner||owner!=garageSceneOwner_||!garageScene_||
           generation!=managerGeneration_||garageScene_->kind!=K::entity141300||
           garageScene_->sessionGeneration!=generation||garageScene_->objectKey!=key(garageScene_))return false;
        if(garageSceneRetiring_)return allowRetiring&&!garageScene_->reference00&&
            !contains(application_->entityAuthority_.view(),garageScene_);
        return ownedGarageChild(*garageScene_)&&garageScene_->reference00==1;
    }
    Result constructGaragePodScene1FA308(std::uint64_t generation,const SceneOwner* owner,C& c)noexcept {
        if(!owner||garageSceneOwner_||garageScene_)return {Status::phase,0x1FA308};
        auto r=constructGaragePrefix(generation,c,K::entity141300);if(!r.okay())return r;
        garageSceneOwner_=owner;garageScene_=&c;return {Status::complete,0x1FA308};
    }
    Result preflightScenePodRetirement142590(std::uint64_t generation,const SceneOwner& owner,
        const C& c,const C* parent)const noexcept {
        if(!sceneOwnerCurrent(generation,&owner,false)||garageScene_!=&c||
           !application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.firstChild18||!linked(c,parent))return {Status::topology,0x1425B8};
        return {Status::complete,0x142590};
    }
    Result beginScenePodRetirement142590(std::uint64_t generation,const SceneOwner& owner,
        C& c,const C* parent)noexcept {
        auto r=preflightScenePodRetirement142590(generation,owner,c,parent);if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C&&c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;garageSceneRetiring_=true;return {Status::complete,0x104970};
    }
    Result finishScenePodRetirement1413E0(std::uint64_t generation,const SceneOwner& owner,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,&owner,true)||!garageSceneRetiring_||garageScene_!=&c||
           c.firstChild18||!application_->objectCount423F60_)return {Status::reference,0x1413E0};
        if(sceneCollisionList3CC_)return {Status::phase,0x27BB80};
        if(sceneEmitterListLedger_.node&&!sceneEmitterListLedger_.retired)return {Status::phase,0x27DCD0};
        for(const auto& x:sceneEmitters_)if(x.node&&!x.retired)return {Status::phase,0x15A720};
        for(const auto& x:sceneCollisions_)if(x.node&&!x.retired)return {Status::phase,0x11B4A0};
        for(const auto& x:sceneLights_)if(x.node&&!x.retired)return {Status::phase,0x146980};
        for(const auto& x:sceneFog_)if(x.node&&!x.retired)return {Status::phase,0x13E700};
        if(sceneParticle_.node&&!sceneParticle_.retired)return {Status::phase,0x160B80};
        --application_->objectCount423F60_;forgetGarageChild(c);
        garageScene_=nullptr;garageSceneOwner_=nullptr;garageSceneRetiring_=false;
        sceneLights_={};sceneLightsParent_=nullptr;sceneFog_={};sceneParticle_={};
        sceneCollisions_={};sceneCollisionOwner_=nullptr;sceneCollisionParent_=nullptr;
        garageVisibilityOwner_=nullptr;garageVisibilityRoot_=nullptr;garageVisibilityNodes_={};
        garageVisibilityNodeRetiring_={};garageVisibilityNodeRetired_={};
        garageVisibilityRootRetiring_=false;garageVisibilityRootRetired_=false;
        sceneEmitters_={};sceneEmitterList_=nullptr;sceneEmitterListLedger_={};
        return {Status::complete,0x1413E0};
    }
    Result preflightGarageSceneParticleList27DCD0(std::uint64_t generation,const SceneOwner* owner,
        const C& c)const noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||sceneEmitterList_!=&c||
           sceneEmitterListLedger_.node!=&c||(c.descendants24!=0&&c.descendants24!=0x402u)||
           c.parent1C||c.previous10||c.next14)return {Status::topology,0x27DCD0};
        for(const auto& slot:sceneEmitters_)if(slot.node&&!slot.retired)return {Status::phase,0x27DCD0};
        return preflightPageSceneChildRetirement(generation,sceneEmitterListLedger_,c,nullptr);
    }
    Result beginGarageSceneParticleList27DCD0(std::uint64_t generation,const SceneOwner* owner,
        C& c,const C* parent)noexcept {
        const auto r=preflightGarageSceneParticleList27DCD0(generation,owner,c);if(!r.okay())return r;
        if(parent)return {Status::topology,0x27DCD0};
        return beginPageSceneChildRetirement(generation,sceneEmitterListLedger_,c,parent);
    }
    Result finishGarageSceneParticleList1413E0(std::uint64_t generation,const SceneOwner* owner,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||sceneEmitterList_!=&c)
            return {Status::owner,0x1413E0};
        return finishPageSceneChildRetirement(generation,sceneEmitterListLedger_,c,nullptr);
    }
    Result constructGarageSceneEmitter27C7B0(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,C& list)noexcept {
        constexpr std::array<K,7> kinds{K::sceneEmitter15F100,K::sceneEmitter15DE00,
            K::sceneEmitter15B860,K::sceneEmitter15E800,K::sceneEmitter15F100,
            K::sceneEmitter15B860,K::sceneEmitter15F100};
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=sceneEmitters_.size()||
           !ownedGarageChild(list)||list.kind!=K::entity141300||
           (sceneEmitterList_&&sceneEmitterList_!=&list))return {Status::owner,0x27C7B0};
        if(ordinal){const auto& prior=sceneEmitters_[ordinal-1];
            if(!prior.node||(!prior.retired&&(!pageSceneChildLive(prior)||!linked(*prior.node,&list))))
                return {Status::phase,0x27C7B0};}
        else if(list.firstChild18)return {Status::topology,0x27C7B0};
        const auto r=constructPageSceneChild(generation,sceneEmitters_[ordinal],c,kinds[ordinal],0x27C7B0);
        if(r.okay()){
            if(!sceneEmitterList_){sceneEmitterListLedger_.node=&list;sceneEmitterListLedger_.kind=list.kind;}
            sceneEmitterList_=&list;
        }
        return r;
    }
    Result enableGarageSceneEmitter27D7A0(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,bool enabled)noexcept {
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=sceneEmitters_.size()||
           sceneEmitters_[ordinal].node!=&c||!pageSceneChildLive(sceneEmitters_[ordinal])||
           c.firstChild18||(c.parent1C&&c.parent1C!=sceneEmitterList_))return {Status::owner,0x27D7A0};
        // Only the reached base activity stores. Concrete emitter payload
        //effects remain in the typed source owner, never implied by this hook.
        // This exact enrolled emitter is required childless above. Borrow its
        // actual address for the existing base kernel; no recursive forest or
        // copied Entity is introduced, and the Application identity checks stay.
        C* const node=&c;
        const OriginalWreckComponentDomain50LA reached{c.sessionGeneration,&node,1};
        const auto r=setOriginalWreckBaseActivity50LA(reached,c,enabled);
        return r.status==OriginalWreckComponentStatus50LA::complete?
            Result{Status::complete,r.sourcePC}:Result{Status::topology,r.sourcePC};
    }
    Result attachGarageSceneEmitter27C7B0(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,C& list)noexcept {
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=sceneEmitters_.size()||
           sceneEmitterList_!=&list||!ownedGarageChild(list)||sceneEmitters_[ordinal].node!=&c||
           !pageSceneChildLive(sceneEmitters_[ordinal])||c.parent1C||c.previous10||c.next14||c.firstChild18)
            return {Status::topology,0x27C7B0};
        const auto r=application_->entityAuthority_.attachChildren(list,c,0x402u,0);
        return r.status==OriginalWreckComponentStatus50LA::complete?
            Result{Status::complete,r.sourcePC}:Result{Status::topology,r.sourcePC};
    }
    Result preflightGarageSceneEmitter15A720(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,const C& c)const noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneEmitters_.size()||
           (c.parent1C&&c.parent1C!=sceneEmitterList_))return {Status::owner,0x15A720};
        return preflightPageSceneChildRetirement(generation,sceneEmitters_[ordinal],c,c.parent1C);
    }
    Result beginGarageSceneEmitter15A720(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,const C* parent)noexcept {
        const auto r=preflightGarageSceneEmitter15A720(generation,owner,ordinal,c);if(!r.okay())return r;
        if(parent!=c.parent1C)return {Status::topology,0x15A720};
        return beginPageSceneChildRetirement(generation,sceneEmitters_[ordinal],c,parent);
    }
    Result finishGarageSceneEmitter15A720(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneEmitters_.size())
            return {Status::owner,0x15A720};
        return finishPageSceneChildRetirement(generation,sceneEmitters_[ordinal],c,
            sceneEmitters_[ordinal].retirementParent);
    }
    Result constructGarageSceneLight273AA0(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,C& list)noexcept {
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=sceneLights_.size()||
           !ownedGarageChild(list)||list.kind!=K::entity141300||list.reference00!=1||
           (sceneLightsParent_&&sceneLightsParent_!=&list)||
           (ordinal&&(!pageSceneChildLive(sceneLights_[ordinal-1])||
            sceneLights_[ordinal-1].node->parent1C!=&list)))return {Status::phase,0x273AA0};
        auto r=constructPageSceneChild(generation,sceneLights_[ordinal],c,K::light146720,0x146720);
        if(r.okay())sceneLightsParent_=&list;
        return r;
    }
    Result attachGarageSceneLight273B98(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,C& list)noexcept {
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=sceneLights_.size()||
           sceneLightsParent_!=&list||sceneLights_[ordinal].node!=&c||!pageSceneChildLive(sceneLights_[ordinal])||
           !ownedGarageChild(list)||c.parent1C||c.previous10||c.next14||c.firstChild18||
           (ordinal==0?list.firstChild18!=nullptr:list.firstChild18!=sceneLights_[0].node||
            sceneLights_[0].node->next14||!linked(*sceneLights_[0].node,&list)))
            return {Status::topology,0x273B98};
        const auto r=application_->entityAuthority_.attachChildren(list,c,2u,0);
        return r.status==OriginalWreckComponentStatus50LA::complete?Result{Status::complete,0x273B98}:Result{Status::topology,r.sourcePC};
    }
    Result constructGarageSceneCollisionList27BB40(std::uint64_t generation,const SceneOwner* scene,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,scene,false)||sceneCollisionList3CC_)
            return {Status::phase,0x27BB40};
        const auto r=constructGaragePrefix(generation,c,K::entity141300); //Actual141300(a1=1).
        if(r.okay())sceneCollisionList3CC_=&c;
        return r;
    }
    Result constructGarageSceneCollision1B5590(std::uint64_t generation,const SceneOwner* scene,
        const CollisionOwner* owner,std::size_t ordinal,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,scene,false)||!owner||ordinal>=sceneCollisions_.size()||
           (sceneCollisionOwner_&&sceneCollisionOwner_!=owner)||
           (ordinal&&!sceneCollisions_[ordinal-1].node))return {Status::phase,0x1B5610};
        auto r=constructPageSceneChild(generation,sceneCollisions_[ordinal],c,K::collision11B190,0x11B190);
        if(r.okay())sceneCollisionOwner_=owner;
        return r;
    }
    Result attachGarageSceneCollision1B577C(std::uint64_t generation,const SceneOwner* scene,
        const CollisionOwner* owner,std::size_t ordinal,C& c,C& list)noexcept {
        if(!sceneOwnerCurrent(generation,scene,false)||!owner||sceneCollisionOwner_!=owner||
           ordinal>=sceneCollisions_.size()||sceneCollisions_[ordinal].node!=&c||
           !pageSceneChildLive(sceneCollisions_[ordinal])||!ownedGarageChild(list)||
           list.kind!=K::staticModels27B610||(sceneCollisionParent_&&sceneCollisionParent_!=&list)||
           c.parent1C||c.previous10||c.next14||c.firstChild18)return {Status::topology,0x1B577C};
        const auto r=application_->entityAuthority_.attachChildren(list,c,0u,0);
        if(r.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,r.sourcePC};
        sceneCollisionParent_=&list;return {Status::complete,0x1B577C};
    }
    Result preflightGarageSceneCollisionRetirement(std::uint64_t generation,const SceneOwner* scene,
        const CollisionOwner* owner,std::size_t ordinal,const C& c,const C* parent)const noexcept {
        if(!sceneOwnerCurrent(generation,scene,true)||!owner||owner!=sceneCollisionOwner_||
           ordinal>=sceneCollisions_.size()||(parent&&parent!=sceneCollisionParent_))return {Status::generation,0x142590};
        return preflightPageSceneChildRetirement(generation,sceneCollisions_[ordinal],c,parent);
    }
    Result beginGarageSceneCollisionRetirement(std::uint64_t generation,const SceneOwner* scene,
        const CollisionOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,scene,true)||!owner||owner!=sceneCollisionOwner_||
           ordinal>=sceneCollisions_.size()||(parent&&parent!=sceneCollisionParent_))return {Status::generation,0x142590};
        return beginPageSceneChildRetirement(generation,sceneCollisions_[ordinal],c,parent);
    }
    Result finishGarageSceneCollisionRetirement(std::uint64_t generation,const SceneOwner* scene,
        const CollisionOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,scene,true)||!owner||owner!=sceneCollisionOwner_||
           ordinal>=sceneCollisions_.size()||(parent&&parent!=sceneCollisionParent_))return {Status::generation,0x142590};
        return finishPageSceneChildRetirement(generation,sceneCollisions_[ordinal],c,parent);
    }
    Result constructGarageSceneFog273C30(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c)noexcept {
        static constexpr std::array<K,3> kinds{K::fog13E6B0,K::fogOff13F850,K::fogOn13F980};
        if(!sceneOwnerCurrent(generation,owner,false)||ordinal>=3||
           (ordinal&&!pageSceneChildLive(sceneFog_[ordinal-1])))return {Status::phase,0x273C30};
        return constructPageSceneChild(generation,sceneFog_[ordinal],c,kinds[ordinal],0x273C30);
    }
    Result constructGarageSceneParticleManager1B4DC0(std::uint64_t generation,
        const SceneOwner* owner,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,owner,false))return {Status::phase,0x1B4DC0};
        return constructPageSceneChild(generation,sceneParticle_,c,K::particleManager160B10,0x160B10);
    }
    Result constructGaragePageRouter2A6FD0(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&owner,*garagePage_))return {Status::phase,0x2A6FD0};
        return constructPageSceneChild(generation,pageRouter_,c,K::uiRouter2A6FD0,0x2A6FD0);
    }
    Result attachGaragePageRouter1D6DE4(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&owner,*garagePage_)||pageRouter_.node!=&c||
           !pageSceneChildLive(pageRouter_)||c.parent1C||c.previous10||c.next14||c.firstChild18||
           garagePage_->firstChild18)return {Status::topology,0x1D6DE4};
        const auto r=application_->entityAuthority_.attachChildren(*garagePage_,c,0xffffffffu,0);
        return r.status==OriginalWreckComponentStatus50LA::complete?Result{Status::complete,0x1D6DE4}:Result{Status::topology,r.sourcePC};
    }
    bool garagePageRouterCurrent(std::uint64_t generation,const PageOwner& owner)const noexcept {
        return garagePage_&&garagePageOwnedCurrent(generation,&owner,*garagePage_)&&pageSceneChildLive(pageRouter_)&&
            linked(*pageRouter_.node,garagePage_)&&garagePage_->firstChild18==pageRouter_.node&&
            !pageRouter_.node->next14&&!pageRouter_.node->firstChild18&&pageRouter_.node->mask20==0xffffffffu;
    }
    Result constructGaragePageControl1D5D10(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRouterCurrent(generation,owner)||ordinal>=8||
           (ordinal&&!pageSceneChildLive(pageControls_[ordinal-1])))return {Status::phase,0x1D5D10};
        return constructPageSceneChild(generation,pageControls_[ordinal],c,K::uiControl1D5D10,0x1D5D10);
    }
    Result constructGaragePageGroup2A4B70(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRouterCurrent(generation,owner)||ordinal>=3||
           (ordinal&&!pageSceneChildLive(pageGroups_[ordinal-1])))return {Status::phase,0x2A4B70};
        for(const auto& x:pageControls_)if(!pageSceneChildLive(x)||!linked(*x.node,nullptr))return {Status::phase,0x2A4B70};
        return constructPageSceneChild(generation,pageGroups_[ordinal],c,K::uiControlGroup2A4B70,0x2A4B70);
    }
    bool garagePageCollectionsCurrent(std::uint64_t generation,const PageOwner& owner)const noexcept {
        if(!garagePageRouterCurrent(generation,owner))return false;
        for(const auto& x:pageControls_)if(!pageSceneChildLive(x)||!linked(*x.node,nullptr)||x.node->firstChild18)return false;
        for(const auto& x:pageGroups_)if(!pageSceneChildLive(x)||!linked(*x.node,nullptr)||x.node->firstChild18)return false;
        return true;
    }
    Result preflightGarageSceneLightRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,const C& c,const C* parent)const noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneLights_.size())return {Status::generation,0x142590};
        return preflightPageSceneChildRetirement(generation,sceneLights_[ordinal],c,parent);
    }
    Result beginGarageSceneLightRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneLights_.size())return {Status::generation,0x142590};
        return beginPageSceneChildRetirement(generation,sceneLights_[ordinal],c,parent);
    }
    Result finishGarageSceneLightRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneLights_.size())return {Status::generation,0x142590};
        return finishPageSceneChildRetirement(generation,sceneLights_[ordinal],c,parent);
    }
    Result preflightGarageSceneFogRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,const C& c,const C* parent)const noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneFog_.size())return {Status::generation,0x142590};
        return preflightPageSceneChildRetirement(generation,sceneFog_[ordinal],c,parent);
    }
    Result beginGarageSceneFogRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneFog_.size())return {Status::generation,0x142590};
        return beginPageSceneChildRetirement(generation,sceneFog_[ordinal],c,parent);
    }
    Result finishGarageSceneFogRetirement(std::uint64_t generation,const SceneOwner* owner,
        std::size_t ordinal,C& c,const C* parent)noexcept {
        if(!sceneOwnerCurrent(generation,owner,true)||ordinal>=sceneFog_.size())return {Status::generation,0x142590};
        return finishPageSceneChildRetirement(generation,sceneFog_[ordinal],c,parent);
    }

    // The one concrete1F9FF0 object. Its Entity constructor is reached only
    // after all22 real widget slots and Fog0/On2/Off1 attachments. Generic
    // construct/attach/retire apertures cannot impersonate Page completion.
    Result constructGaragePage1F9FF0(std::uint64_t generation,
        const rr::platform::OriginalUiGaragePageOwner50KG* owner,
        const Widget1Owner* widgets,const ManagerFogOwner* fog,C& c)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||garagePageOwner_||garagePage_||!managerLive_||managerPublished_||managerForestRetiring_||
           generation!=managerGeneration_||widgets!=managerWidget1Owner_||!widgets||
           fog!=managerFogOwner_||!fog||widgetCursor_!=22||widgetPopulationRetiring_||
           managerFogAttachCursor_!=3||managerWidget1DetachedReleased_||managerFogPayloadReleased_||
           !managerFogAttached_[0]||!managerFogAttached_[1]||!managerFogAttached_[2]||
           c.reference00||c.kind!=K::unbound||garageChildren_.size()==garageChildren_.capacity()||
           !application_->entityAuthority_.canEnroll(c,key(&c)))return {Status::phase,0x1D1C14};
        const auto init=initializeOriginalWreckComponent50LA(c,K::uiGaragePage1F9FF0,
            generation,key(&c),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,init.sourcePC};
        if(!application_->entityAuthority_.enroll(c))std::terminate();
        garageChildren_.push_back({&c,K::uiGaragePage1F9FF0,false});
        garagePageOwner_=owner;garagePage_=&c;return {Status::complete,0x2A5C7C};
    }
    bool garageScenePageChildCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||ordinal>=pageSceneAttachmentCount_)return false;
        const auto& row=pageSceneAttachments_[ordinal];
        return row.node==&c&&!row.retired&&ownedGarageChild(c)&&c.reference00==1&&
            c.parent1C==garagePage_&&linked(c,garagePage_)&&c.mask20==row.mask;
    }
    Result noteGaragePageAttachmentFailure1B4A38(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const std::array<C*,10>& nodes)noexcept {
        if(partialPageFailureOwner_||!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||managerPublished_||managerForestRetiring_||
           garagePageAttached_||garagePageRetiring_||pageSceneAttachmentCount_!=6||
           pageLateSceneAttachmentCount_||pageCameraOwner_||pageCameraEntity_||
           !pageSceneChildLive(pageRouter_)||!linked(*pageRouter_.node,garagePage_)||
           garagePage_->firstChild18!=pageRouter_.node||pageRouter_.node->firstChild18||
           pageRouter_.node->mask20!=0xffffffffu||nodes[6]!=sceneCollisionList3CC_)
            return {Status::phase,0x1B4A38};
        constexpr std::array<K,10> kinds{{K::entity141300,K::entity141300,K::fog13E6B0,
            K::fogOff13F850,K::fogOn13F980,K::staticModels27B610,K::entity141300,
            K::animationManager101330,K::toyList2D9B40,K::particleManager160B10}};
        const C* previous=pageRouter_.node;const C* cursor=previous?previous->next14:nullptr;
        for(std::size_t i=0;i<nodes.size();++i){const auto* node=nodes[i];
            if(!node||!ownedGarageChild(*node)||node->kind!=kinds[i]||node->reference00!=1||
               node->sessionGeneration!=generation||node->objectKey!=key(node))return {Status::owner,0x1B4A38};
            for(std::size_t j=0;j<i;++j)if(nodes[j]==node)return {Status::owner,0x1B4A38};
            if(i<6){
                if(cursor!=node||node->previous10!=previous||
                   !garageScenePageChildCurrent(generation,page,scene,i,*node))return {Status::topology,0x1B4A38};
                previous=node;cursor=node->next14;
            }else if(node->parent1C||node->previous10||node->next14||node->mask20||
                     pageSceneAttachments_[i].node||pageSceneAttachments_[i].retired)
                return {Status::topology,0x1B4A38};
        }
        if(cursor||nodes[6]->firstChild18)return {Status::topology,0x1B4A38};
        partialPageFailureOwner_=&page;partialPageFailureScene_=&scene;partialPageFailureRawList_=nodes[6];
        return {Status::complete,0x1B4A38};
    }
    bool garagePartialPageFailureCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene)const noexcept {
        return partialPageFailureOwner_==&page&&partialPageFailureScene_==&scene&&partialPageFailureRawList_&&
            garagePage_&&(garagePageRetiring_?garagePageRetirementCurrent(generation,&page):
                garagePageOwnedCurrent(generation,&page,*garagePage_))&&
            sceneOwnerCurrent(generation,&scene,garagePageRetiring_)&&!managerPublished_&&!managerForestRetiring_&&
            !garagePageAttached_&&pageSceneAttachmentCount_==6&&
            !pageLateSceneAttachmentCount_&&!pageCameraOwner_&&!pageCameraEntity_;
    }
    Result preflightGarageFailedCollisionList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        if(!garagePartialPageFailureCurrent(generation,page,scene)||partialPageFailureRawList_!=&c||
           sceneCollisionList3CC_!=&c||partialPageFailureRawRetiring_||partialPageFailureRawRetired_||
           !ownedGarageChild(c)||c.kind!=K::entity141300||c.reference00!=1||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           !application_->entityAuthority_.canRetire(c))return {Status::phase,0x27BB80};
        return {Status::complete,0x142590};
    }
    Result beginGarageFailedCollisionList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        const auto r=preflightGarageFailedCollisionList142590(generation,page,scene,c);if(!r.okay())return r;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;partialPageFailureRawRetiring_=true;return {Status::complete,0x104970};
    }
    bool garageFailedCollisionListBaseCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        if(!garagePartialPageFailureCurrent(generation,page,scene)||partialPageFailureRawList_!=&c||
           sceneCollisionList3CC_!=&c||!partialPageFailureRawRetiring_||partialPageFailureRawRetired_||
           c.kind!=K::entity141300||c.sessionGeneration!=generation||c.objectKey!=key(&c)||
           c.reference00||c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)return false;
        for(const auto& row:garageChildren_)if(row.node==&c)return !row.model&&row.kind==K::entity141300;
        return false;
    }
    Result finishGarageFailedCollisionList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garageFailedCollisionListBaseCurrent(generation,page,scene,c))return {Status::phase,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        partialPageFailureRawRetiring_=false;partialPageFailureRawRetired_=true;
        return {Status::complete,0x1413E0};
    }
    bool garageLateBorrowAudioContextCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene)const noexcept {
        if(partialPageFailureOwner_||!garagePage_||
           !(garagePageRetiring_?garagePageRetirementCurrent(generation,&page):
               garagePageOwnedCurrent(generation,&page,*garagePage_))||
           !sceneOwnerCurrent(generation,&scene,garagePageRetiring_)||
           managerPublished_||managerForestRetiring_||garagePageAttached_||
           pageSceneAttachmentCount_!=pageSceneAttachments_.size()||
           !pageCameraOwner_||!pageCameraEntity_||pageCameraRetiring_||!pageCameraRetired_||
           pageCameraEntity_->reference00||pageLateSceneAttachmentCount_||
           (lateBorrowAudioPage_&&lateBorrowAudioPage_!=&page)||
           (lateBorrowAudioScene_&&lateBorrowAudioScene_!=&scene))return false;
        for(const auto& row:pageSceneAttachments_)if(!row.node||!row.retired)return false;
        for(const auto& row:pageLateSceneAttachments_)if(row.node||row.retired)return false;
        for(const auto& row:pageLateAudioLifetimes_)if(row.node||row.retiring||row.retired)return false;
        return true;
    }
    // The private Scene caller separately proves the immutable failed1B4D24
    // Page receipt and supplies its real3BC/3C0 payload. No late slot is forged.
    Result beginGarageLateBorrowAudioList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garageLateBorrowAudioContextCurrent(generation,page,scene)||
           lateBorrowAudioLists_[ordinal-1]||lateBorrowAudioRetiring_[ordinal-1]||
           lateBorrowAudioRetired_[ordinal-1]||
           (ordinal==1&&!lateBorrowAudioRetired_[1])||
           !ownedGarageChild(c)||c.kind!=K::entity141300||c.reference00!=1||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           !application_->entityAuthority_.canRetire(c))return {Status::phase,0x142590};
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;lateBorrowAudioPage_=&page;lateBorrowAudioScene_=&scene;
        lateBorrowAudioLists_[ordinal-1]=&c;lateBorrowAudioRetiring_[ordinal-1]=true;
        return {Status::complete,0x104970};
    }
    Result finishGarageLateBorrowAudioList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garageLateBorrowAudioContextCurrent(generation,page,scene)||
           lateBorrowAudioPage_!=&page||lateBorrowAudioScene_!=&scene||
           lateBorrowAudioLists_[ordinal-1]!=&c||!lateBorrowAudioRetiring_[ordinal-1]||
           lateBorrowAudioRetired_[ordinal-1]||c.kind!=K::entity141300||
           c.sessionGeneration!=generation||c.objectKey!=key(&c)||c.reference00||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::phase,0x1413E0};
        bool found=false;for(const auto& row:garageChildren_)if(row.node==&c)
            found=!row.model&&row.kind==K::entity141300;
        if(!found)return {Status::owner,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        lateBorrowAudioRetiring_[ordinal-1]=false;lateBorrowAudioRetired_[ordinal-1]=true;
        return {Status::complete,0x1413E0};
    }
    bool garageLateBorrowAudioListsRetired(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene)const noexcept {
        return garageLateBorrowAudioContextCurrent(generation,page,scene)&&
            lateBorrowAudioPage_==&page&&lateBorrowAudioScene_==&scene&&
            lateBorrowAudioLists_[0]&&lateBorrowAudioLists_[1]&&
            !lateBorrowAudioRetiring_[0]&&!lateBorrowAudioRetiring_[1]&&
            lateBorrowAudioRetired_[0]&&lateBorrowAudioRetired_[1];
    }
    //Only the typed Scene wrapper supplies its actual3BC/3C0 allocation.
    //These native failure receipts never populate the unattained late Page ledger.
    Result beginGarageFailedAudioList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garagePartialPageFailureCurrent(generation,page,scene)||
           !partialPageFailureRawRetired_||partialPageFailureRawRetiring_||
           partialPageFailureAudio_[ordinal-1]||partialPageFailureAudioRetiring_[ordinal-1]||
           partialPageFailureAudioRetired_[ordinal-1]||
           (ordinal==1&&!partialPageFailureAudioRetired_[1])||
           !ownedGarageChild(c)||c.kind!=K::entity141300||c.reference00!=1||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           !application_->entityAuthority_.canRetire(c))return {Status::phase,0x142590};
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;partialPageFailureAudio_[ordinal-1]=&c;
        partialPageFailureAudioRetiring_[ordinal-1]=true;return {Status::complete,0x104970};
    }
    Result finishGarageFailedAudioList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garagePartialPageFailureCurrent(generation,page,scene)||
           partialPageFailureAudio_[ordinal-1]!=&c||!partialPageFailureAudioRetiring_[ordinal-1]||
           partialPageFailureAudioRetired_[ordinal-1]||c.kind!=K::entity141300||
           c.sessionGeneration!=generation||c.objectKey!=key(&c)||c.reference00||
           c.parent1C||c.previous10||c.next14||c.firstChild18||c.mask20||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::phase,0x1413E0};
        bool found=false;for(const auto& row:garageChildren_)if(row.node==&c)
            found=!row.model&&row.kind==K::entity141300;
        if(!found)return {Status::owner,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        partialPageFailureAudioRetiring_[ordinal-1]=false;
        partialPageFailureAudioRetired_[ordinal-1]=true;return {Status::complete,0x1413E0};
    }
    bool garageFailedPageSceneTailCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(!garagePartialPageFailureCurrent(generation,page,scene)||!partialPageFailureRawRetired_||
           partialPageFailureRawRetiring_||ordinal>=6||
           !garageScenePageChildCurrent(generation,page,scene,ordinal,c)||c.next14)return false;
        for(std::size_t i=ordinal+1;i<6;++i)if(!pageSceneAttachments_[i].retired)return false;
        return true;
    }
    bool garagePageSceneTailCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(garageFailedPageSceneTailCurrent(generation,page,scene,ordinal,c))return true;
        if(ordinal>=10||pageSceneAttachmentCount_!=10||!garageLatePageRowsRetired()||
           !pageCameraOwner_||!pageCameraRetired_||
           !garageScenePageChildCurrent(generation,page,scene,ordinal,c)||c.next14)return false;
        for(std::size_t i=ordinal+1;i<10;++i)if(!pageSceneAttachments_[i].retired)return false;
        return true;
    }
    Result beginGaragePageLightList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageSceneTailCurrent(generation,page,scene,1,c)||sceneLightsParent_!=&c||
           pageLightListLifetime_.node!=&c)return {Status::topology,0x2781B0};
        for(const auto& light:sceneLights_)if(light.node&&!light.retired)return {Status::phase,0x146980};
        return beginPageSceneChildRetirement(generation,pageLightListLifetime_,c,garagePage_);
    }
    Result finishGaragePageLightList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||pageSceneAttachments_[1].node!=&c||
           sceneLightsParent_!=&c||pageLightListLifetime_.node!=&c)return {Status::owner,0x1413E0};
        return finishPageSceneChildRetirement(generation,pageLightListLifetime_,c,garagePage_);
    }
    Result bindGarageVisibility272E94(std::uint64_t generation,const VisibilityOwner& owner,
        const SceneOwner& scene,C& root,C& primary,C& secondary)noexcept {
        if(!sceneOwnerCurrent(generation,&scene,false)||garageVisibilityOwner_||
           !ownedGarageChild(root)||root.kind!=K::entity141300||root.parent1C||root.previous10||root.next14||
           root.firstChild18!=&primary||!ownedGarageChild(primary)||!ownedGarageChild(secondary)||
           primary.kind!=K::searchNode27F120||secondary.kind!=K::searchNode27F120||
           primary.parent1C!=&root||primary.previous10||primary.next14!=&secondary||primary.firstChild18||
           secondary.parent1C!=&root||secondary.previous10!=&primary||secondary.next14||secondary.firstChild18||
           primary.mask20!=0x400u||secondary.mask20)return {Status::topology,0x272E94};
        garageVisibilityOwner_=&owner;garageVisibilityRoot_=&root;
        garageVisibilityNodes_={&primary,&secondary,nullptr};return {Status::complete,0x272EB0};
    }
    Result bindGarageEnvironment2730F8(std::uint64_t generation,const VisibilityOwner& owner,
        const SceneOwner& scene,C& c)noexcept {
        if(!sceneOwnerCurrent(generation,&scene,false)||garageVisibilityOwner_!=&owner||
           !garageVisibilityRoot_||garageVisibilityNodes_[2]||!ownedGarageChild(c)||
           c.kind!=K::searchNode27F120||c.parent1C!=garageVisibilityRoot_||
           c.previous10!=garageVisibilityNodes_[1]||c.next14||c.firstChild18||c.mask20||
           !linked(c,garageVisibilityRoot_))return {Status::topology,0x273100};
        garageVisibilityNodes_[2]=&c;return {Status::complete,0x273100};
    }
    bool garagePageVisibilityRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,const C& root)const noexcept {
        if(garageVisibilityOwner_==&owner&&garageVisibilityRoot_==&root&&
           !garageVisibilityRootRetiring_&&!garageVisibilityRootRetired_&&
           partialPageFailureAudioRetired_[0]&&partialPageFailureAudioRetired_[1]&&
           garageFailedPageSceneTailCurrent(generation,page,scene,0,root))return true;
        if(garageVisibilityOwner_!=&owner||garageVisibilityRoot_!=&root||
           garageVisibilityRootRetiring_||garageVisibilityRootRetired_||pageSceneAttachmentCount_<10||
           !garageLatePageRowsRetired()||!garageScenePageChildCurrent(generation,page,scene,0,root)||root.next14||
           (pageCameraOwner_&&!pageCameraRetired_))return false;
        for(std::size_t i=1;i<pageSceneAttachmentCount_;++i)
            if(!pageSceneAttachments_[i].retired)return false;
        return true;
    }
    Result preflightGaragePageSearchNode27F180(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,const C& c)const noexcept {
        if(ordinal>=3||!garageVisibilityRoot_||garageVisibilityNodes_[ordinal]!=&c||
           garageVisibilityNodeRetiring_[ordinal]||garageVisibilityNodeRetired_[ordinal]||
           !garagePageVisibilityRetirementCurrent(generation,page,scene,owner,*garageVisibilityRoot_)||
           !ownedGarageChild(c)||c.kind!=K::searchNode27F120||c.firstChild18||c.next14||
           c.parent1C!=garageVisibilityRoot_||c.previous10!=(ordinal?garageVisibilityNodes_[ordinal-1]:nullptr)||
           !linked(c,garageVisibilityRoot_)||!application_->entityAuthority_.canRetire(c))
            return {Status::topology,0x142590};
        for(std::size_t i=ordinal+1;i<3;++i)
            if(garageVisibilityNodes_[i]&&!garageVisibilityNodeRetired_[i])return {Status::phase,0x1424E0};
        return {Status::complete,0x142590};
    }
    Result beginGaragePageSearchNode142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,C& c)noexcept {
        const auto r=preflightGaragePageSearchNode27F180(generation,page,scene,owner,ordinal,c);
        if(!r.okay())return r;
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;garageVisibilityNodeRetiring_[ordinal]=true;return {Status::complete,0x104970};
    }
    bool garagePageSearchNodeBaseCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,const C& c)const noexcept {
        return ordinal<3&&garageVisibilityRoot_&&garageVisibilityNodes_[ordinal]==&c&&
            garageVisibilityNodeRetiring_[ordinal]&&!garageVisibilityNodeRetired_[ordinal]&&
            garagePageVisibilityRetirementCurrent(generation,page,scene,owner,*garageVisibilityRoot_)&&
            c.kind==K::searchNode27F120&&c.sessionGeneration==generation&&c.objectKey==key(&c)&&
            !c.reference00&&!c.firstChild18&&!c.next14&&c.parent1C==garageVisibilityRoot_&&
            c.previous10==(ordinal?garageVisibilityNodes_[ordinal-1]:nullptr)&&
            (!c.previous10||!c.previous10->next14)&&garageVisibilityRoot_->firstChild18!=&c&&
            !contains(application_->entityAuthority_.view(),&c)&&application_->objectCount423F60_;
    }
    Result finishGaragePageSearchNode1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,C& c)noexcept {
        if(!garagePageSearchNodeBaseCurrent(generation,page,scene,owner,ordinal,c))
            return {Status::phase,0x27F23C};
        --application_->objectCount423F60_;forgetGarageChild(c);
        garageVisibilityNodeRetiring_[ordinal]=false;garageVisibilityNodeRetired_[ordinal]=true;
        return {Status::complete,0x27F23C};
    }
    Result beginGaragePageVisibility142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,C& c)noexcept {
        if(!garagePageVisibilityRetirementCurrent(generation,page,scene,owner,c)||c.firstChild18||
           !application_->entityAuthority_.canRetire(c))return {Status::phase,0x1425B8};
        for(std::size_t i=0;i<3;++i)if(garageVisibilityNodes_[i]&&!garageVisibilityNodeRetired_[i])
            return {Status::phase,0x27F180};
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;garageVisibilityRootRetiring_=true;return {Status::complete,0x104970};
    }
    Result finishGaragePageVisibility1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const VisibilityOwner& owner,C& c)noexcept {
        if(!live().okay()||!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||garageVisibilityOwner_!=&owner||
           garageVisibilityRoot_!=&c||!garageVisibilityRootRetiring_||garageVisibilityRootRetired_||
           !pageSceneAttachmentCount_||pageSceneAttachments_[0].node!=&c||pageSceneAttachments_[0].retired||
           c.kind!=K::entity141300||c.reference00||c.firstChild18||c.next14||c.parent1C!=garagePage_||
           garagePage_->firstChild18==&c||(c.previous10&&c.previous10->next14)||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return {Status::phase,0x1413E0};
        --application_->objectCount423F60_;forgetGarageChild(c);
        garageVisibilityRootRetiring_=false;garageVisibilityRootRetired_=true;
        return {Status::complete,0x1413E0};
    }
    bool garageVisibilitySourceRetired(std::uint64_t generation,const SceneOwner& scene,
        const VisibilityOwner& owner,const C& root)const noexcept {
        if(!sceneOwnerCurrent(generation,&scene,true)||garageVisibilityOwner_!=&owner||
           garageVisibilityRoot_!=&root||!garageVisibilityRootRetired_||garageVisibilityRootRetiring_||
           root.reference00||root.firstChild18||contains(application_->entityAuthority_.view(),&root))return false;
        for(std::size_t i=0;i<3;++i)if(garageVisibilityNodes_[i]&&
           (!garageVisibilityNodeRetired_[i]||garageVisibilityNodeRetiring_[i]||
            garageVisibilityNodes_[i]->reference00))return false;
        return true;
    }
    // Typed normal ToyList ordinal8. The detached beginToyListRetirement
    // remains unchanged and still rejects an attached parent.
    Result beginGaragePageToyListRetirement142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageSceneTailCurrent(generation,page,scene,8,c)||c.firstChild18||
           c.kind!=K::toyList2D9B40||c.mask20!=0xffffffffu)
            return {Status::topology,0x1425B8};
        GarageChild* actual=nullptr;
        for(auto& row:garageChildren_)if(row.node==&c&&!row.model&&!row.toyListRetiring&&
            row.kind==K::toyList2D9B40)actual=&row;
        if(!actual||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;actual->toyListRetiring=true;
        return {Status::complete,0x104970};
    }
    bool garagePageToyListBaseRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||pageSceneAttachmentCount_!=10||
           pageSceneAttachments_[8].node!=&c||pageSceneAttachments_[8].retired||
           c.kind!=K::toyList2D9B40||c.sessionGeneration!=generation||c.objectKey!=key(&c)||
           c.reference00||c.firstChild18||c.next14||c.parent1C!=garagePage_||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_||
           garagePage_->firstChild18==&c||(c.previous10&&c.previous10->next14!=c.next14)||
           (!pageCameraOwner_||!pageCameraRetired_))return false;
        for(std::size_t i=9;i<pageSceneAttachmentCount_;++i)
            if(!pageSceneAttachments_[i].retired)return false;
        for(const auto& row:pageLateSceneAttachments_)if(row.node&&!row.retired)return false;
        for(const auto& row:garageChildren_)if(row.node==&c)
            return !row.model&&row.kind==K::toyList2D9B40&&row.toyListRetiring;
        return false;
    }
    Result finishGaragePageToyListRetirement1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageToyListBaseRetirementCurrent(generation,page,scene,c))
            return {Status::phase,0x2D9C20};
        return finishToyListRetirement(generation,c);
    }
    Result retireGaragePageAnimationManager101FE0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,OriginalGarageAnimationManagerFields50GN& manager,const C& toys)noexcept {
        auto& c=manager.entity;
        if(!garagePageSceneTailCurrent(generation,page,scene,7,c)||c.firstChild18||
           c.kind!=K::animationManager101330||c.mask20!=0xffffffffu||
           application_->garageAnimationManager3DF400_!=&manager||!application_->garageAnimationManagerLive_||
           manager.toys38!=&toys||pageSceneAttachments_[8].node!=&toys||
           !pageSceneAttachments_[8].retired||toys.kind!=K::toyList2D9B40||
           toys.reference00||toys.firstChild18||contains(application_->entityAuthority_.view(),&toys))
            return {Status::phase,0x101FE0};
        const auto step=releaseEmpty(c,K::animationManager101330,generation,garagePage_);
        if(!step.okay())return step;
        application_->garageAnimationManagerLive_=false;forgetGarageChild(c);
        //Source101FE0 keeps the actual3DF400 address and manager38 unchanged.
        return {Status::complete,0x102008};
    }
    // Only the two real Page list slots: Static3C4 ordinal5 and raw3CC
    //ordinal6. The Page still owns ref1 while142590 recursively retires its
    //children; Scene ref0 is a later1FA1B4 phase, not this predecessor.
    bool garagePageListRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(ordinal==5&&c.kind==K::staticModels27B610&&c.mask20==2u&&
           garageFailedPageSceneTailCurrent(generation,page,scene,ordinal,c))return true;
        if((ordinal!=5&&ordinal!=6)||pageSceneAttachmentCount_<10||!garageLatePageRowsRetired()||
           !garageScenePageChildCurrent(generation,page,scene,ordinal,c)||c.next14||
           c.mask20!=2u||c.kind!=(ordinal==5?K::staticModels27B610:K::entity141300)||
           (pageCameraOwner_&&!pageCameraRetired_))return false;
        for(std::size_t i=ordinal+1;i<pageSceneAttachmentCount_;++i)
            if(!pageSceneAttachments_[i].retired)return false;
        return true;
    }
    bool garagePageStaticListRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        return garagePageListRetirementCurrent(generation,page,scene,5,c);
    }
    bool garagePageCollisionListRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        return !pageCollisionListRetiring_&&garagePageListRetirementCurrent(generation,page,scene,6,c);
    }
    Result beginGaragePageStaticListRetirement142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageStaticListRetirementCurrent(generation,page,scene,c)||c.firstChild18)
            return {Status::topology,0x1425B8};
        GarageChild* actual=nullptr;
        for(auto& row:garageChildren_)if(row.node==&c&&!row.model&&!row.staticListRetiring&&
            row.kind==K::staticModels27B610)actual=&row;
        if(!actual||!application_->entityAuthority_.canRetire(c))return {Status::reference,0x104970};
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;actual->staticListRetiring=true;
        //Source142590 preserves dying c.parent/previous/next observations.
        return {Status::complete,0x104970};
    }
    bool garagePageListBaseRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if((ordinal!=5&&ordinal!=6)||!garagePage_||
           !garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||ordinal>=pageSceneAttachmentCount_||
           pageSceneAttachments_[ordinal].node!=&c||pageSceneAttachments_[ordinal].retired||
           c.reference00||c.firstChild18||c.next14||c.parent1C!=garagePage_||
           contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)
            return false;
        return (!c.previous10||c.previous10->next14==c.next14)&&garagePage_->firstChild18!=&c;
    }
    bool garagePageStaticListBaseRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        if(!garagePageListBaseRetirementCurrent(generation,page,scene,5,c))return false;
        for(const auto& row:garageChildren_)if(row.node==&c)
            return !row.model&&row.kind==K::staticModels27B610&&row.staticListRetiring;
        return false;
    }
    Result finishGaragePageStaticListRetirement1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageStaticListBaseRetirementCurrent(generation,page,scene,c))
            return {Status::phase,0x27B700};
        return finishStaticListRetirement(generation,c);
    }
    Result beginGaragePageCollisionListRetirement142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageCollisionListRetirementCurrent(generation,page,scene,c)||c.firstChild18||
           !application_->entityAuthority_.canRetire(c))return {Status::topology,0x1425B8};
        if(c.previous10)c.previous10->next14=c.next14;
        if(c.next14)c.next14->previous10=c.previous10;
        if(c.parent1C->firstChild18==&c)c.parent1C->firstChild18=c.next14;
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;pageCollisionListRetiring_=true;return {Status::complete,0x104970};
    }
    bool garagePageCollisionListBaseRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,const C& c)const noexcept {
        if(!pageCollisionListRetiring_||!garagePageListBaseRetirementCurrent(generation,page,scene,6,c)||
           c.kind!=K::entity141300||c.sessionGeneration!=generation||c.objectKey!=key(&c))return false;
        for(const auto& row:garageChildren_)if(row.node==&c)
            return !row.model&&row.kind==K::entity141300;
        return false;
    }
    Result finishGaragePageCollisionListRetirement1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePageCollisionListBaseRetirementCurrent(generation,page,scene,c))
            return {Status::phase,0x27BC58};
        --application_->objectCount423F60_;forgetGarageChild(c);pageCollisionListRetiring_=false;
        return {Status::complete,0x1413E0};
    }
    Result attachGarageScenePageChild1B48A0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c,std::uint32_t mask)noexcept {
        static constexpr std::array<K,10> kinds{{K::entity141300,K::entity141300,
            K::fog13E6B0,K::fogOff13F850,K::fogOn13F980,K::staticModels27B610,K::entity141300,
            K::animationManager101330,K::toyList2D9B40,K::particleManager160B10}};
        static constexpr std::array<std::uint32_t,10> masks{{0xffffffffu,2u,0xffffffffu,0xffffffffu,
            0xffffffffu,2u,2u,0xffffffffu,0xffffffffu,0xffffffffu}};
        static constexpr std::array<std::uint32_t,10> pcs{{0x1B48D4,0x1B48EC,0x1B4928,0x1B4940,
            0x1B49A8,0x1B49F0,0x1B4A38,0x1B4A68,0x1B4A80,0x1B4AE0}};
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||ordinal>=kinds.size()||
           ordinal!=pageSceneAttachmentCount_||pageCameraOwner_||!ownedGarageChild(c)||
           c.kind!=kinds[ordinal]||c.reference00!=1||mask!=masks[ordinal]||
           c.parent1C||c.previous10||c.next14||c.mask20)
            return {Status::topology,0x1B48A0};
        //The existing concrete constructor ledgers disambiguate equal
        //generic Entity prefix tags; source order alone is not identity.
        if((ordinal==1&&sceneLightsParent_!=&c)||
           (ordinal>=2&&ordinal<=4&&sceneFog_[ordinal-2].node!=&c)||
           (ordinal==6&&sceneCollisionList3CC_!=&c)||
           (ordinal==9&&sceneParticle_.node!=&c))return {Status::generation,pcs[ordinal]};
        C* tail=pageRouter_.node;
        if(!tail||!pageSceneChildLive(pageRouter_)||tail->parent1C!=garagePage_||
           garagePage_->firstChild18!=tail)return {Status::phase,0x1B48A0};
        for(std::size_t i=0;i<ordinal;++i){
            const auto& row=pageSceneAttachments_[i];
            if(!row.node||row.retired||!garageScenePageChildCurrent(generation,page,scene,i,*row.node)||
               tail->next14!=row.node||row.node->previous10!=tail)return {Status::topology,pcs[ordinal]};
            tail=row.node;
        }
        if(tail->next14)return {Status::topology,pcs[ordinal]};
        const auto step=application_->entityAuthority_.attachChildren(*garagePage_,c,mask,0);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        pageSceneAttachments_[ordinal]={&c,mask,false};++pageSceneAttachmentCount_;
        if(ordinal==1)pageLightListLifetime_={&c,K::entity141300,false,false,nullptr};
        return {Status::complete,pcs[ordinal]};
    }
    bool garageLatePageRowsRetired()const noexcept {
        for(const auto& row:pageLateSceneAttachments_)if(row.node&&!row.retired)return false;
        return true;
    }
    bool garageLateScenePageChildCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||ordinal>=pageLateSceneAttachmentCount_)return false;
        const auto& row=pageLateSceneAttachments_[ordinal];
        return row.node==&c&&!row.retired&&ownedGarageChild(c)&&c.reference00==1&&
            c.parent1C==garagePage_&&linked(c,garagePage_)&&c.mask20==row.mask;
    }
    Result attachGarageLateScenePageChild1B4CA0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c,std::uint32_t mask)noexcept {
        static constexpr std::array<std::uint32_t,3> masks{{0x402u,0u,0u}};
        static constexpr std::array<std::uint32_t,3> pcs{{0x1B4D24,0x1B4D84,0x1B4D9C}};
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||ordinal>=masks.size()||
           ordinal!=pageLateSceneAttachmentCount_||pageSceneAttachmentCount_!=10||
           !pageCameraOwner_||!pageCameraEntity_||pageCameraRetiring_||pageCameraRetired_||
           pageCameraEntity_->reference00!=1||pageCameraEntity_->parent1C!=garagePage_||
           !linked(*pageCameraEntity_,garagePage_)||!ownedGarageChild(c)||
           c.kind!=K::entity141300||c.reference00!=1||mask!=masks[ordinal]||
           c.parent1C||c.previous10||c.next14||c.mask20||
           (!ordinal&&sceneEmitterList_!=&c)||(ordinal&&c.firstChild18))
            return {Status::topology,0x1B4CA0};
        for(std::size_t i=0;i<pageSceneAttachments_.size();++i)
            if(!pageSceneAttachments_[i].node||!garageScenePageChildCurrent(generation,page,scene,i,
                *pageSceneAttachments_[i].node))return {Status::topology,pcs[ordinal]};
        C* tail=pageCameraEntity_;
        for(std::size_t i=0;i<ordinal;++i){const auto& row=pageLateSceneAttachments_[i];
            if(!row.node||!garageLateScenePageChildCurrent(generation,page,scene,i,*row.node)||
               tail->next14!=row.node||row.node->previous10!=tail)return {Status::topology,pcs[ordinal]};
            tail=row.node;
        }
        if(tail->next14)return {Status::topology,pcs[ordinal]};
        const auto step=application_->entityAuthority_.attachChildren(*garagePage_,c,mask,0);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        pageLateSceneAttachments_[ordinal]={&c,mask,false};++pageLateSceneAttachmentCount_;
        if(ordinal)pageLateAudioLifetimes_[ordinal-1]={&c,K::entity141300,false,false,nullptr};
        return {Status::complete,pcs[ordinal]};
    }
    bool garageLatePageListRetirementCurrent(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,const C& c)const noexcept {
        if(ordinal>=3||pageLateSceneAttachmentCount_!=3||
           !garageLateScenePageChildCurrent(generation,page,scene,ordinal,c)||c.next14||
           (ordinal==0&&sceneEmitterList_!=&c))return false;
        for(std::size_t i=ordinal+1;i<3;++i)if(!pageLateSceneAttachments_[i].retired)return false;
        return true;
    }
    Result beginGaragePageAudioList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garageLatePageListRetirementCurrent(generation,page,scene,ordinal,c))
            return {Status::topology,0x1425B8};
        return beginPageSceneChildRetirement(generation,pageLateAudioLifetimes_[ordinal-1],c,garagePage_);
    }
    Result finishGaragePageAudioList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
        if(ordinal<1||ordinal>2||!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||pageLateSceneAttachments_[ordinal].node!=&c)
            return {Status::owner,0x1413E0};
        return finishPageSceneChildRetirement(generation,pageLateAudioLifetimes_[ordinal-1],c,garagePage_);
    }
    Result beginGaragePageParticleList142590(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garageLatePageListRetirementCurrent(generation,page,scene,0,c))return {Status::topology,0x27DCD0};
        for(const auto& row:sceneEmitters_)if(row.node&&!row.retired)return {Status::phase,0x15A720};
        return beginPageSceneChildRetirement(generation,sceneEmitterListLedger_,c,garagePage_);
    }
    Result finishGaragePageParticleList1413E0(std::uint64_t generation,const PageOwner& page,
        const SceneOwner& scene,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&page,*garagePage_)||
           !sceneOwnerCurrent(generation,&scene,false)||pageLateSceneAttachments_[0].node!=&c||sceneEmitterList_!=&c)
            return {Status::owner,0x1413E0};
        return finishPageSceneChildRetirement(generation,sceneEmitterListLedger_,c,garagePage_);
    }
    bool garagePageOwnedCurrent(std::uint64_t generation,
        const rr::platform::OriginalUiGaragePageOwner50KG* owner,const C& c)const noexcept {
        return live().okay()&&owner&&owner==garagePageOwner_&&garagePage_==&c&&
            !garagePageRetiring_&&generation==managerGeneration_&&ownedGarageChild(c)&&
            c.kind==K::uiGaragePage1F9FF0&&linked(c,garagePageAttached_?&manager_:nullptr)&&
            // Detached Page already accumulates real child service masks
            // through141960. Do not mistake detachment for a zero mask.
            (!garagePageAttached_||c.mask20==0xffffffffu)&&
            (garagePageAttached_||(!c.parent1C&&!c.previous10&&!c.next14));
    }
    bool garagePageCurrent(std::uint64_t generation,const PageOwner* owner,const C& c)const noexcept {
        return !garagePageAttached_&&garagePageOwnedCurrent(generation,owner,c);
    }
    bool garageAttachedPageCurrent(std::uint64_t generation,const PageOwner* owner,const C& c)const noexcept {
        return garagePageAttached_&&garagePageOwnedCurrent(generation,owner,c);
    }
    // Exact authored manager child chain. No caller-provided child count,
    // ready bit, OnTop omission or replacement subtree establishes this.
    bool populatedGarageManagerForestCurrent(std::uint64_t generation,bool withPage)const noexcept {
        if(!live().okay()||!managerLive_||managerRetiring_||generation!=managerGeneration_||
           !managerSearchOwner_||!managerUiLightsOwner_||!managerFogOwner_||!managerWidget1Owner_||
           managerSearchNodeRetiring_||managerSearchNodeRetired_||managerSearchPayloadReleased_||
           managerUiLightsPayloadReleased_||managerFogPayloadReleased_||widgetPopulationRetiring_||
           widgetCursor_!=widgetRows_.size()||!managerWidget1Table_||managerWidget1TableReleased_||
           managerWidget1DetachedReleased_||managerFogAttachCursor_!=3)return false;
        const C* cursor=manager_.firstChild18;const C* previous=nullptr;
        auto take=[&](const C* node,K kind,std::uint32_t mask)noexcept {
            if(!node||node!=cursor||!ownedGarageChild(*node)||
               !application_->entityAuthority_.canRetire(*node)||node->kind!=kind||
               node->mask20!=mask||!linked(*node,&manager_)||node->previous10!=previous)return false;
            previous=node;cursor=node->next14;return true;
        };
        if(!take(managerSearchSelected_,K::model2403A0,2u)||
           !take(managerSearchNode_,K::searchNode27F120,2u))return false;
        for(std::size_t i=0;i<2;++i)
            if(managerUiLightsRetiring_[i]||managerUiLightsRetired_[i]||
               !take(managerUiLights_[i],K::light146720,0u))return false;
        for(std::size_t i=0;i<widgetRows_.size();++i){const auto& row=widgetRows_[i];
            if(!row.published||static_cast<C* const*>(managerWidget1Table_)[i]!=row.outer)return false;
            if(emptyRow(i)){if(!row.empty||row.outer||row.attached)return false;continue;}
            if(!row.attached||!row.outer||!widgetRowIdentityCurrent(generation,managerWidget1Owner_,i,*row.outer)||
               !take(row.outer,row.outer->kind,0xffffffffu))return false;
        }
        constexpr std::array<std::size_t,3> fogOrder{0,2,1};
        for(const auto i:fogOrder)
            if(!managerFogPublished_[i]||!managerFogAttached_[i]||managerFogRetiring_[i]||managerFogRetired_[i]||
               !take(managerFog_[i],fogKind(i),0xffffffffu))return false;
        if(withPage){
            if(!garagePage_||!garageAttachedPageCurrent(generation,garagePageOwner_,*garagePage_)||
               !take(garagePage_,K::uiGaragePage1F9FF0,0xffffffffu))return false;
        }else if(garagePageAttached_)return false;
        return cursor==nullptr;
    }
    Result setGaragePageForestVisibility(std::uint64_t generation,const PageOwner& owner,
        C& page,bool shown)noexcept {
        const std::uint32_t pc=shown?0x141630u:0x1415A0u;
        if(managerPublished_||managerForestRetiring_||
           !garageAttachedPageCurrent(generation,&owner,page)||
           !populatedGarageManagerForestCurrent(generation,true)||
           pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_!=3||
           !pageCameraOwner_||!pageCameraEntity_)return {Status::phase,pc};
        // The complete actual authority forest is checked by live(), above.
        // Walk real links without a second forest, allocation, new capacity,
        // renderer visibility substitute or constructor-default flags.
        const auto next=[&page](C* node,bool descend)noexcept -> C* {
            if(descend&&node->firstChild18)return node->firstChild18;
            while(node!=&page&&!node->next14)node=node->parent1C;
            return node==&page?nullptr:node->next14;
        };
        // The genuine Camera is Application-owned, not a Garage allocation.
        // Both private callers validate its typed owner before this walk.
        // Keep every other descendant under the original Garage ledger proof.
        if(garageChildren_.size()==std::numeric_limits<std::size_t>::max())
            return {Status::topology,pc};
        std::size_t remaining=garageChildren_.size()+1;
        bool cameraSeen=false;
        for(C* node=&page;node;node=next(node,true)){
            if(!remaining--)return {Status::topology,pc};
            if(node==pageCameraEntity_){
                if(cameraSeen||node->kind!=K::camera236180||node->reference00!=1||
                   node->firstChild18||node->descendants24)return {Status::topology,pc};
                cameraSeen=true;
            }else if(!ownedGarageChild(*node)||node->sessionGeneration!=generation||!node->reference00)
                return {Status::topology,pc};
        }
        if(!cameraSeen)return {Status::topology,pc};
        // Original ELF Entity tables for these concrete counted constructors
        // retain base slots1C=1415A0 and24=141630. They do NOT invoke the
        // distinct slot18/20 activity notifications. Own bits0/1 are written
        // first; each bit3 child skips its ENTIRE virtual recursion.
        for(C* node=&page;node;){
            if(node!=&page&&(node->flags29&8u)){node=next(node,false);continue;}
            node->flags29=static_cast<std::uint8_t>((node->flags29&~1u)|(shown?1u:0u));
            node->flags29=static_cast<std::uint8_t>((node->flags29&~2u)|(shown?2u:0u));
            node=next(node,true);
        }
        return {Status::complete,shown?0x1416BCu:0x14162Cu};
    }
    Result attachCompletedGaragePage1D1C40(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(managerPublished_||managerForestRetiring_||!garagePageCurrent(generation,&owner,c)||
           !populatedGarageManagerForestCurrent(generation,false))return {Status::phase,0x1D1C40};
        // Page owner privately invokes this only AFTER actual virtual84
        // return1FAAC8. Scope proves the exact receiving forest, not that load.
        const auto step=application_->entityAuthority_.attachChildren(manager_,c,0xffffffffu,
            application_->attachmentLock423F58_);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        garagePageAttached_=true;return {Status::complete,0x1D1C40};
    }
    Result publishPopulatedGarageManager1B8268(std::uint64_t generation)noexcept {
        if(managerPublished_||managerForestRetiring_||!dispatcherLive_||
           !populatedGarageManagerForestCurrent(generation,true)||
           manager_.parent1C||manager_.previous10||manager_.next14)return {Status::topology,0x1B8268};
        const auto step=application_->entityAuthority_.attachChildren(dispatcher_,manager_,0xffffffffu,
            application_->attachmentLock423F58_);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        managerPublished_=true;
        dispatcherCurrentManager30_=&manager_;
        dispatcherCurrentManagerGeneration30_=generation;
        return {Status::complete,0x1B8274};
    }
    bool publishedPopulatedGarageManagerCurrent(std::uint64_t generation)const noexcept {
        return managerPublished_&&!managerForestRetiring_&&
            dispatcherCurrentManager30_==&manager_&&dispatcherCurrentManagerGeneration30_==generation&&
            populatedGarageManagerForestCurrent(generation,true);
    }
    Result clearRetiredCurrentManager2A35C0(std::uint64_t generation)noexcept {
        if(!live().okay()||managerLive_||managerRetiring_||managerForestRetiring_||
           dispatcherCurrentManager30_!=&manager_||dispatcherCurrentManagerGeneration30_!=generation||
           manager_.reference00||manager_.firstChild18||manager_.parent1C||manager_.previous10||manager_.next14||
           contains(application_->entityAuthority_.view(),&manager_))return {Status::phase,0x2A35C0};
        dispatcherCurrentManager30_=nullptr;dispatcherCurrentManagerGeneration30_=0;
        return {Status::complete,0x2A35C0};
    }
    Result detachPopulatedGarageManager2A35A4(std::uint64_t generation)noexcept {
        if(!managerPublished_||managerForestRetiring_||
           !populatedGarageManagerForestCurrent(generation,true)||!linked(manager_,&dispatcher_))
            return {Status::topology,0x2A35A4};
        // This is ONLY the current-manager segment. Its caller must own the
        // prior actual Display20/24 and dispatcher34/38 hide operations.
        // Reuse genuine141A70 ancestor-mask repair, not manual pointer clear.
        const auto step=application_->entityAuthority_.detach(manager_,application_->attachmentLock423F58_);
        if(step.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,step.sourcePC};
        managerPublished_=false;managerForestRetiring_=true;
        // The manager remains enrolled/ref1 and current-manager30 is NOT
        // cleared here. Actual child callbacks precede104970/ref0 payload.
        return {Status::complete,0x2A35A4};
    }
    Result beginGaragePageRetirement142590(std::uint64_t generation,
        const rr::platform::OriginalUiGaragePageOwner50KG* owner,C& c)noexcept {
        if(!garagePageOwnedCurrent(generation,owner,c))return {Status::generation,0x142590};
        if(c.firstChild18||c.next14||!application_->entityAuthority_.canRetire(c))
            return {Status::topology,0x1425B8};
        if(garagePageAttached_){
            if(managerPublished_||!linked(c,&manager_))return {Status::topology,0x1425B8};
            // Actual142590 tail callback preserves the dying Page links.
            // Later fog/widgets cannot retire until this complete callback.
            garagePagePreviousAtRetire_=c.previous10;
            if(c.previous10)c.previous10->next14=c.next14;
            else manager_.firstChild18=c.next14;
        }
        if(!application_->entityAuthority_.retire(c))std::terminate();
        --c.reference00;garagePageRetiring_=true;
        //1413E0/count decrement must follow1FA170, never this last-ref step.
        return {Status::complete,0x104970};
    }
    Result beginGarageWattoBindingPageRetirement142590(std::uint64_t generation,
        const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
        // Exact real attached Page after late3/Camera/initial10/router teardown.
        // The private caller supplies its distinct pre-bind entry-failure identity; no
        // normal managerForestRetiring or missing attachment is synthesized.
        if(!garagePageOwnedCurrent(generation,&owner,c)||!garagePageAttached_||
           garageSceneOwner_!=&scene||!garageScene_||managerPublished_||managerForestRetiring_||
           !managerPageTableOwner_||!managerPageTable144Borrow_||managerPageTableCount_!=1||
           managerPageTableReleased_||managerPageTable144Borrow_[0]!=&c||pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_!=3||
           !pageCameraOwner_||!pageCameraEntity_||!pageCameraRetired_||pageCameraRetiring_||
           pageCameraEntity_->reference00||!pageRouter_.retired||c.firstChild18||
           wattoBindingPageRetirementOwner_||completedWattoBindingPageOwner_||
           loadedHidePageRetirementOwner_||completedLoadedHidePageOwner_||
           partialPageFailureOwner_||completedPartialPageOwner_||lateBorrowPageRetirementOwner_||
           completedLateBorrowPageOwner_||lateBorrowAudioPage_||failedSharedPageOwner_)
            return {Status::phase,0x1D4800};
        for(const auto& row:pageSceneAttachments_)if(!row.node||!row.retired)return {Status::phase,0x1425B8};
        for(const auto& row:pageLateSceneAttachments_)if(!row.node||!row.retired)return {Status::phase,0x1425B8};
        const auto step=beginGaragePageRetirement142590(generation,&owner,c);
        if(!step.okay())return step;
        wattoBindingPageRetirementOwner_=&owner;return step;
    }
    Result beginGarageLoadedHidePageRetirement142590(std::uint64_t generation,
        const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
        // Exact real attached Page after late3/Camera/initial10/router teardown.
        // The private caller supplies its distinct failed-hide identity; no
        // normal managerForestRetiring or missing attachment is synthesized.
        if(!garagePageOwnedCurrent(generation,&owner,c)||!garagePageAttached_||
           garageSceneOwner_!=&scene||!garageScene_||managerPublished_||managerForestRetiring_||
           !managerPageTableOwner_||!managerPageTable144Borrow_||managerPageTableCount_!=1||
           managerPageTableReleased_||managerPageTable144Borrow_[0]!=&c||pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_!=3||
           !pageCameraOwner_||!pageCameraEntity_||!pageCameraRetired_||pageCameraRetiring_||
           pageCameraEntity_->reference00||!pageRouter_.retired||c.firstChild18||
           loadedHidePageRetirementOwner_||completedLoadedHidePageOwner_||
           partialPageFailureOwner_||completedPartialPageOwner_||lateBorrowPageRetirementOwner_||
           completedLateBorrowPageOwner_||lateBorrowAudioPage_||failedSharedPageOwner_)
            return {Status::phase,0x1415A0};
        for(const auto& row:pageSceneAttachments_)if(!row.node||!row.retired)return {Status::phase,0x1425B8};
        for(const auto& row:pageLateSceneAttachments_)if(!row.node||!row.retired)return {Status::phase,0x1425B8};
        const auto step=beginGaragePageRetirement142590(generation,&owner,c);
        if(!step.okay())return step;
        loadedHidePageRetirementOwner_=&owner;return step;
    }

    Result beginGarageLateBorrowPageRetirement142590(std::uint64_t generation,
        const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
        if(!garageLateBorrowAudioListsRetired(generation,owner,scene)||garagePage_!=&c||
           lateBorrowPageRetirementOwner_||completedLateBorrowPageOwner_||completedPartialPageOwner_||
           partialPageFailureOwner_||!pageRouter_.retired||c.firstChild18)
            return {Status::phase,0x1B4D24};
        const auto step=beginGaragePageRetirement142590(generation,&owner,c);
        if(!step.okay())return step;
        lateBorrowPageRetirementOwner_=&owner;
        return step;
    }
    bool failedWattoBindingGaragePageRetired1413E0(std::uint64_t generation,const PageOwner& owner)const noexcept {
        return live().okay()&&generation==managerGeneration_&&completedWattoBindingPageOwner_==&owner&&
            completedWattoBindingPageGeneration_==generation&&completedWattoBindingPageEpoch_==managerEpoch_&&
            !loadedHidePageRetirementOwner_&&!completedLoadedHidePageOwner_&&
            !wattoBindingPageRetirementOwner_&&!lateBorrowPageRetirementOwner_&&!completedLateBorrowPageOwner_&&
            !lateBorrowAudioPage_&&!lateBorrowAudioScene_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garagePageAttached_&&
            !garageScene_&&!garageSceneOwner_&&!partialPageFailureOwner_&&!completedPartialPageOwner_&&
            !managerPublished_&&!managerForestRetiring_;
    }
    bool failedLoadedHideGaragePageRetired1413E0(std::uint64_t generation,const PageOwner& owner)const noexcept {
        return live().okay()&&generation==managerGeneration_&&completedLoadedHidePageOwner_==&owner&&
            completedLoadedHidePageGeneration_==generation&&completedLoadedHidePageEpoch_==managerEpoch_&&
            !loadedHidePageRetirementOwner_&&!lateBorrowPageRetirementOwner_&&!completedLateBorrowPageOwner_&&
            !lateBorrowAudioPage_&&!lateBorrowAudioScene_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garagePageAttached_&&
            !garageScene_&&!garageSceneOwner_&&!partialPageFailureOwner_&&!completedPartialPageOwner_&&
            !managerPublished_&&!managerForestRetiring_;
    }
    bool failedLateBorrowGaragePageRetired1413E0(std::uint64_t generation,const PageOwner& owner)const noexcept {
        return live().okay()&&generation==managerGeneration_&&completedLateBorrowPageOwner_==&owner&&
            completedLateBorrowPageGeneration_==generation&&completedLateBorrowPageEpoch_==managerEpoch_&&
            !lateBorrowPageRetirementOwner_&&!lateBorrowAudioPage_&&!lateBorrowAudioScene_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garagePageAttached_&&
            !garageScene_&&!garageSceneOwner_&&!partialPageFailureOwner_&&!completedPartialPageOwner_&&
            !managerPublished_&&!managerForestRetiring_;
    }
    bool failedPartialGaragePageRetired1413E0(std::uint64_t generation,const PageOwner& owner)const noexcept {
        return live().okay()&&generation==managerGeneration_&&completedPartialPageOwner_==&owner&&
            completedPartialPageGeneration_==generation&&completedPartialPageEpoch_==managerEpoch_&&
            !garagePage_&&!garagePageOwner_&&!garagePageRetiring_&&!garagePageAttached_&&
            !garageScene_&&!garageSceneOwner_&&!partialPageFailureOwner_&&
            !managerPublished_&&!managerForestRetiring_;
    }
    Result beginGaragePageRouterRetirement142590(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&owner,*garagePage_)||pageRouter_.node!=&c||
           garagePage_->firstChild18!=&c||c.parent1C!=garagePage_||c.previous10||c.next14||c.firstChild18)
            return {Status::topology,0x1425B8};
        return beginPageSceneChildRetirement(generation,pageRouter_,c,garagePage_);
    }
    Result finishGaragePageRouterRetirement1413E0(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(!garagePage_||!garagePageOwnedCurrent(generation,&owner,*garagePage_)||garagePage_->firstChild18)
            return {Status::topology,0x2A70B8};
        return finishPageSceneChildRetirement(generation,pageRouter_,c,garagePage_);
    }
    bool garagePageRetirementCurrent(std::uint64_t generation,const PageOwner* owner)const noexcept {
        if(!live().okay()||!owner||owner!=garagePageOwner_||!garagePage_||
           !garagePageRetiring_||generation!=managerGeneration_||!application_->objectCount423F60_)
            return false;
        const auto& c=*garagePage_;
        const bool retainedLinks=garagePageAttached_
            ?c.parent1C==&manager_&&c.previous10==garagePagePreviousAtRetire_&&!c.next14&&
             manager_.firstChild18!=&c&&(!c.previous10||
              (ownedGarageChild(*c.previous10)&&c.previous10->next14!=&c))
            :!c.parent1C&&!c.previous10&&!c.next14;
        return c.kind==K::uiGaragePage1F9FF0&&c.sessionGeneration==generation&&c.objectKey==key(&c)&&
            !c.reference00&&!c.firstChild18&&retainedLinks&&
            !contains(application_->entityAuthority_.view(),&c)&&
            std::find_if(garageChildren_.begin(),garageChildren_.end(),
                [&](const GarageChild& child){return child.node==&c;})!=garageChildren_.end();
    }
    Result beginGaragePageControlRetirement142590(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRetirementCurrent(generation,&owner)||ordinal>=pageControls_.size()||
           c.parent1C||c.previous10||c.next14)return {Status::phase,0x2A5D34};
        for(std::size_t i=0;i<ordinal;++i)if(!pageControls_[i].retired)return {Status::phase,0x2A5D34};
        return beginPageSceneChildRetirement(generation,pageControls_[ordinal],c,nullptr);
    }
    Result finishGaragePageControlRetirement1413E0(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRetirementCurrent(generation,&owner)||ordinal>=pageControls_.size())
            return {Status::phase,0x2A58D4};
        return finishPageSceneChildRetirement(generation,pageControls_[ordinal],c,nullptr);
    }
    Result beginGaragePageGroupRetirement142590(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRetirementCurrent(generation,&owner)||ordinal>=pageGroups_.size()||
           c.parent1C||c.previous10||c.next14)return {Status::phase,0x2A4BD0};
        for(const auto& slot:pageControls_)if(!slot.retired)return {Status::phase,0x2A4BD0};
        for(std::size_t i=0;i<ordinal;++i)if(!pageGroups_[i].retired)return {Status::phase,0x2A4BD0};
        return beginPageSceneChildRetirement(generation,pageGroups_[ordinal],c,nullptr);
    }
    Result finishGaragePageGroupRetirement1413E0(std::uint64_t generation,const PageOwner& owner,
        std::size_t ordinal,C& c)noexcept {
        if(!garagePageRetirementCurrent(generation,&owner)||ordinal>=pageGroups_.size())
            return {Status::phase,0x2A4BD0};
        return finishPageSceneChildRetirement(generation,pageGroups_[ordinal],c,nullptr);
    }
    Result finishGaragePageRetirement1413E0(std::uint64_t generation,const PageOwner& owner,C& c)noexcept {
        if(!garagePageRetirementCurrent(generation,&owner)||garagePage_!=&c||garageScene_||
           (pageRouter_.node&&!pageRouter_.retired))return {Status::phase,0x1413E0};
        for(const auto& slot:pageControls_)if(slot.node&&!slot.retired)return {Status::phase,0x1413E0};
        for(const auto& slot:pageGroups_)if(slot.node&&!slot.retired)return {Status::phase,0x1413E0};
        for(const auto& row:pageSceneAttachments_)if(row.node&&!row.retired)return {Status::phase,0x1413E0};
        for(const auto& row:pageLateSceneAttachments_)if(row.node&&!row.retired)return {Status::phase,0x1413E0};
        if(pageCameraOwner_&&!pageCameraRetired_)return {Status::phase,0x1413E0};
        if(partialPageFailureOwner_&&(partialPageFailureOwner_!=&owner||
           !partialPageFailureScene_||!partialPageFailureRawRetired_||partialPageFailureRawRetiring_||
           !partialPageFailureAudioRetired_[0]||!partialPageFailureAudioRetired_[1]||
           partialPageFailureAudioRetiring_[0]||partialPageFailureAudioRetiring_[1]||
           pageSceneAttachmentCount_!=6||pageLateSceneAttachmentCount_||pageCameraOwner_||
           garagePageAttached_||managerForestRetiring_||completedPartialPageOwner_))
            return {Status::phase,0x1B4A38};
        if(wattoBindingPageRetirementOwner_){
            if(wattoBindingPageRetirementOwner_!=&owner||!garagePageAttached_||managerPublished_||managerForestRetiring_||
               pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_!=3||
               !pageCameraOwner_||!pageCameraEntity_||!pageCameraRetired_||pageCameraRetiring_||
               pageCameraEntity_->reference00||partialPageFailureOwner_||completedPartialPageOwner_||
               lateBorrowPageRetirementOwner_||completedLateBorrowPageOwner_||lateBorrowAudioPage_||
               completedWattoBindingPageOwner_||completedLoadedHidePageOwner_||loadedHidePageRetirementOwner_||failedSharedPageOwner_)
                return {Status::phase,0x1D4800};
        }
        if(loadedHidePageRetirementOwner_){
            if(loadedHidePageRetirementOwner_!=&owner||!garagePageAttached_||managerPublished_||managerForestRetiring_||
               pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_!=3||
               !pageCameraOwner_||!pageCameraEntity_||!pageCameraRetired_||pageCameraRetiring_||
               pageCameraEntity_->reference00||partialPageFailureOwner_||completedPartialPageOwner_||
               lateBorrowPageRetirementOwner_||completedLateBorrowPageOwner_||lateBorrowAudioPage_||
               completedLoadedHidePageOwner_||failedSharedPageOwner_)
                return {Status::phase,0x1415A0};
        }
        if(lateBorrowPageRetirementOwner_||lateBorrowAudioPage_){
            if(lateBorrowPageRetirementOwner_!=&owner||lateBorrowAudioPage_!=&owner||
               !lateBorrowAudioScene_||!lateBorrowAudioLists_[0]||!lateBorrowAudioLists_[1]||
               lateBorrowAudioRetiring_[0]||lateBorrowAudioRetiring_[1]||
               !lateBorrowAudioRetired_[0]||!lateBorrowAudioRetired_[1]||
               pageSceneAttachmentCount_!=10||pageLateSceneAttachmentCount_||
               !pageCameraOwner_||!pageCameraEntity_||!pageCameraRetired_||pageCameraRetiring_||
               pageCameraEntity_->reference00||garagePageAttached_||managerForestRetiring_||
               partialPageFailureOwner_||completedPartialPageOwner_||completedLateBorrowPageOwner_)
                return {Status::phase,0x1B4D24};
        }
        --application_->objectCount423F60_;forgetGarageChild(c);
        if(wattoBindingPageRetirementOwner_==&owner){
            completedWattoBindingPageOwner_=&owner;completedWattoBindingPageGeneration_=generation;
            completedWattoBindingPageEpoch_=managerEpoch_;
        }
        if(loadedHidePageRetirementOwner_==&owner){
            completedLoadedHidePageOwner_=&owner;completedLoadedHidePageGeneration_=generation;
            completedLoadedHidePageEpoch_=managerEpoch_;
        }
        if(lateBorrowPageRetirementOwner_==&owner){
            completedLateBorrowPageOwner_=&owner;completedLateBorrowPageGeneration_=generation;
            completedLateBorrowPageEpoch_=managerEpoch_;
        }
        if(partialPageFailureOwner_==&owner){
            completedPartialPageOwner_=&owner;completedPartialPageGeneration_=generation;
            completedPartialPageEpoch_=managerEpoch_;
        }
        if(garagePageAttached_&&managerForestRetiring_){
            completedManagerPage1413E0_=garagePageOwner_;
            completedManagerPageGeneration_=generation;completedManagerPageEpoch_=managerEpoch_;
        }
        if(!garagePageAttached_&&!managerForestRetiring_&&garagePageOwner_==failedSharedPageOwner_&&
           failedSharedPageGeneration_==generation&&failedSharedPageEpoch_==managerEpoch_)
            failedSharedPageRetired_=true;
        wattoBindingPageRetirementOwner_=nullptr;
        loadedHidePageRetirementOwner_=nullptr;
        lateBorrowPageRetirementOwner_=nullptr;
        lateBorrowAudioPage_=nullptr;lateBorrowAudioScene_=nullptr;lateBorrowAudioLists_={};
        lateBorrowAudioRetiring_={};lateBorrowAudioRetired_={};
        garagePage_=nullptr;garagePageOwner_=nullptr;garagePageRetiring_=false;
        garagePageAttached_=false;garagePagePreviousAtRetire_=nullptr;
        pageRouter_={};pageControls_={};pageGroups_={};
        pageSceneAttachments_={};pageSceneAttachmentCount_=0;pageLightListLifetime_={};
        partialPageFailureOwner_=nullptr;partialPageFailureScene_=nullptr;partialPageFailureRawList_=nullptr;
        partialPageFailureAudio_={};partialPageFailureAudioRetiring_={};partialPageFailureAudioRetired_={};
        partialPageFailureRawRetiring_=partialPageFailureRawRetired_=false;
        pageLateSceneAttachments_={};pageLateSceneAttachmentCount_=0;pageLateAudioLifetimes_={};
        pageCameraOwner_=nullptr;pageCameraEntity_=nullptr;pageCameraRetiring_=false;pageCameraRetired_=false;
        return {Status::complete,0x1413E0};
    }
    Result preflightGarageManagerPayloadRetirement(std::uint64_t generation,const SearchOwner* owner)const noexcept {
        auto r=live();if(!r.okay())return r;
        if(!owner||owner!=managerSearchOwner_||!managerLive_||generation!=managerGeneration_||
           managerSearchNodeRetiring_||(managerSearchNode_&&!managerSearchNodeRetired_)||
           !garageChildren_.empty()||manager_.firstChild18||manager_.next14||
           !application_->entityAuthority_.canRetire(manager_)||
           !linked(manager_,managerPublished_?&dispatcher_:nullptr))return {Status::topology,0x142590};
        return {Status::complete,0x142590};
    }
    Result beginGarageManagerPayloadRetirement(std::uint64_t generation,const SearchOwner* owner)noexcept {
        auto r=preflightGarageManagerPayloadRetirement(generation,owner);if(!r.okay())return r;
        if(manager_.previous10)manager_.previous10->next14=manager_.next14;
        if(manager_.parent1C&&manager_.parent1C->firstChild18==&manager_)manager_.parent1C->firstChild18=manager_.next14;
        if(!application_->entityAuthority_.retire(manager_))std::terminate();
        --manager_.reference00;managerRetiring_=true;return {Status::complete,0x104970};
    }
    bool managerPayloadRetirementCurrent(std::uint64_t generation,const SearchOwner* owner)const noexcept {
        return owner&&owner==managerSearchOwner_&&managerLive_&&managerRetiring_&&
            generation==managerGeneration_&&live(true).okay()&&!manager_.reference00&&
            !manager_.firstChild18&&garageChildren_.empty();
    }
    Result noteManagerSearchPayloadReleased(std::uint64_t generation,const SearchOwner* owner)noexcept {
        if(!managerPayloadRetirementCurrent(generation,owner)||managerSearchPayloadReleased_)
            return {Status::phase,0x1CCADC};
        managerSearchPayloadReleased_=true;return {Status::complete,0x1CCADC};
    }
    Result finishGarageManagerPayloadRetirement(std::uint64_t generation,const SearchOwner* owner)noexcept {
        if(!managerPayloadRetirementCurrent(generation,owner)||!managerSearchPayloadReleased_||
           (managerUiLightsOwner_&&!managerUiLightsPayloadReleased_)||
           (managerFogOwner_&&!managerFogPayloadReleased_)||
           (managerWidget1Owner_&&!managerWidget1TableReleased_)||
           (managerPageTableOwner_&&(!managerPageTableReleased_||!managerBankRetired12E5E0_))||!application_->objectCount423F60_)return {Status::phase,0x1413E0};
        --application_->objectCount423F60_;
        managerLive_=false;managerPublished_=false;managerRetiring_=false;managerForestRetiring_=false;managerGeneration_=0;
        failedSharedPageOwner_=nullptr;failedSharedPageGeneration_=failedSharedPageEpoch_=0;failedSharedPageRetired_=false;
        completedWattoBindingPageOwner_=nullptr;completedWattoBindingPageGeneration_=completedWattoBindingPageEpoch_=0;
        completedLoadedHidePageOwner_=nullptr;completedLoadedHidePageGeneration_=completedLoadedHidePageEpoch_=0;
        completedLateBorrowPageOwner_=nullptr;completedLateBorrowPageGeneration_=completedLateBorrowPageEpoch_=0;
        completedPartialPageOwner_=nullptr;completedPartialPageGeneration_=completedPartialPageEpoch_=0;
        managerSearchOwner_=nullptr;managerSearchNode_=nullptr;managerSearchSelected_=nullptr;
        managerSearchNodeRetiring_=false;managerSearchNodeRetired_=false;managerSearchPayloadReleased_=false;
        managerUiLightsOwner_=nullptr;managerUiLights_={};managerUiLightsRetiring_={};
        managerUiLightsRetired_={};managerUiLightsPayloadReleased_=false;
        managerFogOwner_=nullptr;managerFog_={};managerFogPublished_={};
        managerFogAttached_={};managerFogAttachCursor_=0;managerFogPreviousAtRetire_={};
        managerFogRetiring_={};managerFogRetired_={};managerFogPayloadReleased_=false;
        managerPageTableOwner_=nullptr;managerPageTable144Borrow_=nullptr;
        managerPageTableCount_=0;managerPageTableReleased_=false;managerBankRetired12E5E0_=false;
        managerWidget1Owner_=nullptr;managerWidget1_=nullptr;managerWidget1Table_=nullptr;
        managerWidget1Retiring_=false;managerWidget1Retired_=false;
        managerWidget1DetachedReleased_=false;managerWidget1TableReleased_=false;
        managerTextOverlayOwner_=nullptr;managerTextOverlay_=nullptr;
        managerTextOverlayRetiring_=false;managerTextOverlayRetired_=false;
        managerWidget1Hidden_=false;managerWidget1RowPublished_=false;managerWidget1Attached_=false;
        widgetRows_={};widgetCursor_=0;widgetPopulationRetiring_=false;
        return {Status::complete,0x1413E0};
    }
    bool garageManagerPageRetired1413E0(std::uint64_t generation,const PageOwner* historical)const noexcept {
        return live().okay()&&historical&&historical==completedManagerPage1413E0_&&
            completedManagerPageGeneration_==generation&&completedManagerPageEpoch_==managerEpoch_&&
            generation==managerGeneration_&&managerLive_&&managerForestRetiring_&&!managerPublished_&&
            dispatcherCurrentManager30_==&manager_&&!garagePage_&&!garagePageOwner_&&
            !garagePageRetiring_&&!garageScene_&&!garageSceneOwner_;
    }
    // Native unpublished-construction ownership. This is NOT source manager
    // destruction and never reads source-unwritten tables/name/light fields.
    class OptionsConstruction final {
        friend class OriginalUiEntityScope50KG;
        friend class rr::platform::OriginalUiFrontend50KG;
        friend struct ::original_main_entry50KG::Video;
        enum class Phase : std::uint8_t {empty,beforeVideo2A3A90,nativeAborted,
            afterVideo2A3A90,nativeVideoAborted,derivedComplete1D04C0,
            cameraCold1D05C8,nativeCameraAborted};
        const void* videoEffect_=nullptr; // Only actual Video owner may bind it.
        std::shared_ptr<OriginalUiEntityScope50KG> scope_;
        const rr::platform::OriginalUiFrontend50KG* frontend_=nullptr;
        const void* managerOwner_=nullptr;
        std::uint64_t generation_=0,epoch_=0,priorEpoch_=0;
        std::uint32_t countBefore_=0; // Observation only; NEVER restored.
        Phase phase_=Phase::empty;
        // Actual base constructor writes. Safe backing of the omitted row10
        // is NOT source knowledge; mask records only rows00,20,30.
        std::array<std::array<std::uint32_t,16>,4> matrices_{};
        std::array<std::uint16_t,4> matrixKnown_{};
        std::uint32_t ancestry8_=0,word17C_=0;
        std::array<std::uint32_t,9> zeroWords_{}; //34,140,148,14C,174,15C,160,16C,170.
        bool basePrefixKnown_=false,derivedKnown_=false;
        std::array<std::uint32_t,9> derivedZeros_{}; //180,188,184,18C,190,2A0,2A4,2A8,2B4.
        const OriginalFrontendCameraCold50LA* cameraOwner_=nullptr;
        C* camera2B0_=nullptr; // Historical source borrow, NEVER zeroed by native abort.
        bool cameraAcquired_=false; // Native acquired-resource ledger, not source readiness.
        OptionsConstruction(std::shared_ptr<OriginalUiEntityScope50KG> scope,
            const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
            std::uint64_t generation)noexcept
            :scope_(std::move(scope)),frontend_(frontend),managerOwner_(manager),generation_(generation){}
        bool registrationCurrent()const noexcept {
            if(!scope_||!pending()||!frontend_||!managerOwner_||
               scope_->optionsConstruction_!=this||scope_->managerEpoch_!=epoch_||
               !scope_->live(false,true).okay()||scope_->managerLive_||scope_->managerPublished_||
               scope_->dispatcherCurrentManager30_||scope_->managerRetiring_||scope_->managerForestRetiring_)
                return false;
            const auto& c=scope_->manager_;const auto& app=*scope_->application_;
            if(cameraAcquired_&&(!cameraOwner_||!camera2B0_||
               camera2B0_->kind!=K::camera236180||camera2B0_->reference00!=1||
               camera2B0_->parent1C||camera2B0_->previous10||camera2B0_->next14||camera2B0_->firstChild18||
               app.objectCount423F60_<2||!app.entityAuthority_.canRetire(*camera2B0_)))return false;
            return c.kind==K::uiOptionsManager1D0470&&c.sessionGeneration==generation_&&
                c.objectKey==key(&c)&&c.reference00==1&&!c.previous10&&!c.next14&&!c.firstChild18&&
                !c.parent1C&&!c.mask20&&!c.descendants24&&app.objectCount423F60_&&
                app.entityAuthority_.canRetire(c);
        }
        bool acquiredCurrent()const noexcept {
            return registrationCurrent()&&!videoEffect_&&!cameraAcquired_&&
                (phase_==Phase::beforeVideo2A3A90||phase_==Phase::nativeVideoAborted);
        }
        bool issuedBy(const ::original_main_entry50KG::Application* root,
            const std::shared_ptr<OriginalUiEntityScope50KG>& scope)const noexcept {
            return root&&scope_==scope&&scope_->issuer_==root;
        }
        bool current(const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager)const noexcept {
            if(frontend_!=frontend||managerOwner_!=manager||!registrationCurrent()||
               (phase_!=Phase::beforeVideo2A3A90&&phase_!=Phase::afterVideo2A3A90&&
                phase_!=Phase::derivedComplete1D04C0&&phase_!=Phase::cameraCold1D05C8)||
               ((phase_!=Phase::beforeVideo2A3A90)!=bool(videoEffect_))||!basePrefixKnown_||word17C_!=1)
                return false;
            const bool derived=phase_==Phase::derivedComplete1D04C0||phase_==Phase::cameraCold1D05C8;
            if(derivedKnown_!=derived||ancestry8_!=(derived?0x3cfa20u:0x3d3ab0u)||
               cameraAcquired_!=(phase_==Phase::cameraCold1D05C8))return false;
            if(derived)for(auto word:derivedZeros_)if(word)return false;
            for(auto word:zeroWords_)if(word)return false;
            for(std::size_t m=0;m<4;++m){if(matrixKnown_[m]!=0xff0f)return false;
                for(std::size_t row:{std::size_t(0),std::size_t(2),std::size_t(3)})
                    for(std::size_t col=0;col<4;++col)
                        if(matrices_[m][row*4+col]!=(col==3?0x3f800000u:0u))return false;
            }
            return true;
        }
        bool videoOwnedPhase()const noexcept {
            return phase_==Phase::afterVideo2A3A90||phase_==Phase::derivedComplete1D04C0||
                phase_==Phase::cameraCold1D05C8||phase_==Phase::nativeCameraAborted;
        }
        bool completeDerived1D0470()noexcept {
            if(phase_==Phase::derivedComplete1D04C0||phase_==Phase::cameraCold1D05C8)
                return current(frontend_,managerOwner_);
            if(phase_!=Phase::afterVideo2A3A90||!current(frontend_,managerOwner_))return false;
            ancestry8_=0x3cfea0; //1D048C: actual common manager vtable.
            for(std::size_t i=0;i<8;++i)derivedZeros_[i]=0; //1D0490..1D04B4, authored order.
            ancestry8_=0x3cfa20; //1D04B8: Options vtable.
            derivedZeros_[8]=0; //1D04BC:2B4, after that vtable store.
            derivedKnown_=true;phase_=Phase::derivedComplete1D04C0;return true;
        }
        bool pending()const noexcept {return phase_!=Phase::empty&&phase_!=Phase::nativeAborted;}
        Result abortNative()noexcept {
            if(phase_==Phase::empty||phase_==Phase::nativeAborted)return {Status::complete,0};
            if(!acquiredCurrent())return {Status::phase,0};
            auto& app=*scope_->application_;auto& c=scope_->manager_;
            // Exact detached acquired registration only. No142590/1D04E0,
            // Video write, source-null invention or historical counter reset.
            if(!app.entityAuthority_.retire(c))return {Status::reference,0};
            --c.reference00;--app.objectCount423F60_;
            scope_->optionsConstruction_=nullptr;phase_=Phase::nativeAborted;
            return {Status::complete,0}; // Native terminal, NOT a source return PC.
        }
    public:
        OptionsConstruction(const OptionsConstruction&)=delete;
        OptionsConstruction& operator=(const OptionsConstruction&)=delete;
        ~OptionsConstruction()noexcept {
            if(pending()&&!abortNative().okay())std::terminate();
        }
    };
    Result beginOptionsConstructorPrefix(const ::original_main_entry50KG::Application* issuer,
        const std::shared_ptr<OriginalUiEntityScope50KG>& retained,
        const rr::platform::OriginalUiFrontend50KG* frontend,const void* manager,
        std::uint64_t generation,std::unique_ptr<OptionsConstruction>& output)noexcept {
        if(output||!issuer||issuer!=issuer_||retained.get()!=this||!frontend||!manager||!generation)
            return {Status::owner,0};
        auto result=live();if(!result.okay())return result;
        if(!dispatcherLive_||managerLive_||managerPublished_||managerRetiring_||managerForestRetiring_||
           dispatcherCurrentManager30_||manager_.reference00||optionsConstruction_||
           transitionDisplayPinned_||detachedMemoryCardOwner_||managerSearchOwner_||
           !garageChildren_.empty()||managerEpoch_==std::numeric_limits<std::uint64_t>::max()||
           application_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
           !linked(dispatcher_,&application_->root10_))return {Status::phase,0};
        std::unique_ptr<OptionsConstruction> next;
        try {next.reset(new OptionsConstruction(retained,frontend,manager,generation));}
        catch(...){return {Status::allocation,0};}
        auto& authority=application_->entityAuthority_;
        if(!authority.reserveAdditional(1))return {Status::allocation,0};
        if(!live().okay()||managerLive_||optionsConstruction_||manager_.reference00||
           managerPublished_||managerRetiring_||managerForestRetiring_||dispatcherCurrentManager30_||
           transitionDisplayPinned_||detachedMemoryCardOwner_||managerSearchOwner_||!garageChildren_.empty()||
           managerEpoch_==std::numeric_limits<std::uint64_t>::max()||
           application_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
           !linked(dispatcher_,&application_->root10_)||!authority.canEnroll(manager_,key(&manager_)))
            return {Status::phase,0};
        next->priorEpoch_=managerEpoch_;next->epoch_=managerEpoch_+1;
        next->countBefore_=application_->objectCount423F60_;
        // All allocations and registry capacity precede the first source count.
        manager_.~C();new(&manager_) C;++managerEpoch_;
        optionsConstruction_=next.get();
        const auto init=initializeOriginalWreckComponent50LA(manager_,K::uiOptionsManager1D0470,
            generation,key(&manager_),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(manager_))std::terminate();
        next->phase_=OptionsConstruction::Phase::beforeVideo2A3A90;
        next->ancestry8_=0x3d3ab0; //2A3A18.
        for(std::size_t m=0;m<4;++m){ //2A3A2C/2A3AB0: each real vf0 row.
            for(std::size_t row:{std::size_t(0),std::size_t(2),std::size_t(3)})
                for(std::size_t col=0;col<4;++col)next->matrices_[m][row*4+col]=col==3?0x3f800000u:0u;
            next->matrixKnown_[m]=0xff0f;
        }
        next->word17C_=1; //2A3A40, before the nine zero stores.
        for(auto& word:next->zeroWords_)word=0;
        next->basePrefixKnown_=true;
        // Stop BEFORE global Video44 at2A3A90 and derived1D0484 fields.
        // Do not claim completed1D0470, managerLive, name194 or dispatcher30.
        output=std::move(next);return {Status::complete,0x2A3A90};
    }
    Result prepareGarageManager(std::uint64_t generation)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!dispatcherLive_||managerLive_||dispatcherCurrentManager30_||!generation||managerEpoch_==std::numeric_limits<std::uint64_t>::max()||!linked(dispatcher_,&application_->root10_))
            return {Status::phase,0x1B823C};
        if(!application_->entityAuthority_.reserveAdditional(1))return {Status::allocation,0x1B822C};
        // Reuse native backing only after the prior actual last reference died.
        if(manager_.reference00)return {Status::reference,0x1B823C};
        if(!application_->entityAuthority_.canEnroll(manager_,key(&manager_)))return {Status::capacity,0x1B823C};
        completedManagerPage1413E0_=nullptr;
        completedManagerPageGeneration_=completedManagerPageEpoch_=0;
        manager_.~C();new(&manager_) C;managerGeneration_=generation;++managerEpoch_;
        const auto init=initializeOriginalWreckComponent50LA(manager_,K::uiGarageManager1D1860,
            generation,key(&manager_),true,application_->objectCount423F60_);
        if(init.status!=OriginalWreckComponentStatus50LA::complete||!application_->entityAuthority_.enroll(manager_))
            std::terminate();
        constexpr char originalName[]="UIGarage";
        for(std::size_t i=0;i<sizeof(originalName);++i)managerName194_[i]=originalName[i];
        managerName194Known_=sizeof(originalName);
        managerName194Generation_=generation;managerName194Epoch_=managerEpoch_;
        managerLive_=true;managerPublished_=false;return {Status::complete,0x1D18C0};
    }
    Result publishGarageManager(std::uint64_t generation)noexcept {
        auto r=live();if(!r.okay())return r;
        if(!dispatcherLive_||!managerLive_||managerPublished_||managerForestRetiring_||managerSearchOwner_||managerRetiring_||generation!=managerGeneration_)
            return {Status::phase,0x1B8268};
        //1B8268 supplies this one freshly loaded manager, never an incoming
        // sibling chain owned by another UI scope. Child ownership is joined
        // separately; the current prefix has not issued any child owner yet.
        if(manager_.previous10||manager_.next14||manager_.firstChild18||!garageChildren_.empty())
            return {Status::topology,0x1B8268};
        const auto result=application_->entityAuthority_.attachChildren(dispatcher_,manager_,0xffffffffu,0);
        if(result.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,result.sourcePC};
        managerPublished_=true;
        dispatcherCurrentManager30_=&manager_;
        dispatcherCurrentManagerGeneration30_=generation;
        return {Status::complete,0x1B8274};
    }
    Result releaseGarageManager(std::uint64_t generation)noexcept {
        if(managerSearchOwner_||managerRetiring_)return {Status::phase,0x1CCAA0};
        if(!managerLive_||generation!=managerGeneration_||!garageChildren_.empty())return {Status::generation,0x142590};
        auto r=releaseEmpty(manager_,K::uiGarageManager1D1860,generation,managerPublished_?&dispatcher_:nullptr);
        if(r.okay()){managerLive_=false;managerPublished_=false;managerGeneration_=0;
            dispatcherCurrentManager30_=nullptr;dispatcherCurrentManagerGeneration30_=0;}return r;
    }
    Result close()noexcept {
        if(optionsConstruction_)return {Status::phase,0};
        if(detachedMemoryCardOwner_||detachedMemoryCard_)return {Status::phase,0x1FEE40};
        if(transitionDisplayPinned_)return {Status::phase,0x2A3550};
        if(sharedDialogs_[0]||sharedDialogs_[1])return {Status::phase,0x1425B8};
        if(closed_)return {Status::complete,0x1413E0};
        if(managerLive_||dispatcherCurrentManager30_)return {Status::phase,0x1425B8};
        if(!dispatcherLive_){closed_=true;return {Status::complete,0};}
        auto r=releaseEmpty(dispatcher_,K::uiDispatcher1B7140,scopeGeneration_,&application_->root10_);
        if(r.okay()){dispatcherLive_=false;closed_=true;}return r;
    }
public:
    // A cold constructor capability, not a ready-page or render capability.
    // Only the retained scope issues it; real owners keep it through retirement.
    class GarageConstructionGrant final {
        friend class OriginalUiEntityScope50KG;
        friend struct ::original_main_entry50KG::Application;
        friend struct ::original_main_entry50KG::Frontend;
        friend class rr::platform::OriginalUiGarageSession50KG;
        friend class rr::platform::OriginalUiGarageActors50GN;
        friend class rr::platform::OriginalUiGarageWattoOwner50KG;
        friend class rr::platform::OriginalUiGarageWattoSoundGroup50KG;
        friend class rr::platform::OriginalUiGaragePodScene50KG;
        friend class rr::platform::OriginalUiGarageSceneCollision50KG;
        friend class rr::platform::OriginalUiGarageStaticModels50KG;
        friend class rr::platform::OriginalUiGaragePageOwner50KG;
        friend class rr::platform::OriginalUiGarageManagerSearch50KG;
        friend class rr::platform::OriginalUiGarageUiLights50KG;
        friend class rr::platform::OriginalUiGarageManagerFog50KG;
        friend class rr::platform::OriginalUiGarageWidget1Owner50KG;
        friend class rr::platform::OriginalUiGarageTextOverlay50KG;
        friend class rr::platform::OriginalUiGarageMovementOwner50KG;
        friend class rr::platform::OriginalAudioPendingWaves50KG;
        friend class rr::platform::OriginalUiGarageModelResources50KG;
        friend class rr::platform::OriginalUiGarageSelectedModel50KG;
        friend class rr::platform::OriginalUiGarageDrawContribution50KG;
        friend class rr::platform::OriginalUiGarageVisibility50KG;
        std::shared_ptr<OriginalUiEntityScope50KG> scope_;
        std::uint64_t generation_=0,epoch_=0;
        GarageConstructionGrant(std::shared_ptr<OriginalUiEntityScope50KG> s,std::uint64_t g)
            :scope_(std::move(s)),generation_(g),epoch_(scope_->managerEpoch_){}
        bool current()const noexcept{return scope_&&scope_->live().okay()&&scope_->managerLive_&&
            scope_->manager_.kind==K::uiGarageManager1D1860&&generation_==scope_->managerGeneration_&&epoch_==scope_->managerEpoch_;}
        std::shared_ptr<OriginalApplicationEntities50LA> application()const noexcept{return current()?scope_->application_:nullptr;}
        const ::original_main_entry50KG::Application* issuer()const noexcept{return current()?scope_->issuer_:nullptr;}
        C* manager()const noexcept{return current()?&scope_->manager_:nullptr;}
        const char* borrowManagerName194()const noexcept {
            constexpr char originalName[]="UIGarage";
            if(!current()||scope_->managerRetiring_||
               scope_->managerName194Known_!=sizeof(originalName)||
               scope_->managerName194Generation_!=generation_||scope_->managerName194Epoch_!=epoch_)
                return nullptr;
            for(std::size_t i=0;i<sizeof(originalName);++i)
                if(scope_->managerName194_[i]!=originalName[i])return nullptr;
            return scope_->managerName194_.data();
        }
        Result constructGaragePage1F9FF0(const rr::platform::OriginalUiGaragePageOwner50KG* owner,
            const Widget1Owner* widgets,const ManagerFogOwner* fog,C& c)noexcept {
            if(!current())return {Status::generation,0x1D1C14};
            return scope_->constructGaragePage1F9FF0(generation_,owner,widgets,fog,c);
        }
        bool garageManagerPageRetired1413E0()const noexcept {
            return current()&&scope_->garageManagerPageRetired1413E0(
                generation_,scope_->completedManagerPage1413E0_);
        }
        bool garageManagerPageRetired1413E0(const PageOwner* historical)const noexcept {
            return current()&&scope_->garageManagerPageRetired1413E0(generation_,historical);
        }
        bool garageAttachedPageCurrent(const PageOwner& owner,const C& c)const noexcept {
            return current()&&scope_->garageAttachedPageCurrent(generation_,&owner,c);
        }
        Result activateGaragePageForest141630(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x141630};
            if(!scope_->pageCameraOwner_||!garagePageCameraCurrent(owner,*scope_->pageCameraOwner_))
                return {Status::phase,0x141630};
            return scope_->setGaragePageForestVisibility(generation_,owner,c,true);
        }
        Result hideGaragePageForest1415A0(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1415A0};
            if(!scope_->pageCameraOwner_||!garagePageCameraCurrent(owner,*scope_->pageCameraOwner_))
                return {Status::phase,0x1415A0};
            return scope_->setGaragePageForestVisibility(generation_,owner,c,false);
        }
        Result attachCompletedGaragePage1D1C40(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1D1C40};
            return scope_->attachCompletedGaragePage1D1C40(generation_,owner,c);
        }
        Result attachGarageScenePageChild1B48A0(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,C& c,std::uint32_t mask)noexcept {
            if(!current())return {Status::generation,0x1B48A0};
            return scope_->attachGarageScenePageChild1B48A0(generation_,page,scene,ordinal,c,mask);
        }
        Result attachGarageLateScenePageChild1B4CA0(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,C& c,std::uint32_t mask)noexcept {
            if(!current())return {Status::generation,0x1B4CA0};
            return scope_->attachGarageLateScenePageChild1B4CA0(generation_,page,scene,ordinal,c,mask);
        }
        bool garageLateScenePageChildCurrent(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,const C& c)const noexcept {
            return current()&&scope_->garageLateScenePageChildCurrent(generation_,page,scene,ordinal,c);
        }
        bool garageLatePageListRetirementCurrent(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,const C& c)const noexcept {
            return current()&&scope_->garageLatePageListRetirementCurrent(generation_,page,scene,ordinal,c);
        }
        Result beginGaragePageAudioList142590(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageAudioList142590(generation_,page,scene,ordinal,c);
        }
        Result finishGaragePageAudioList1413E0(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageAudioList1413E0(generation_,page,scene,ordinal,c);
        }
        Result beginGaragePageParticleList142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageParticleList142590(generation_,page,scene,c);
        }
        Result finishGaragePageParticleList1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageParticleList1413E0(generation_,page,scene,c);
        }
        bool garagePageSceneTailCurrent(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,const C& c)const noexcept {
            return current()&&scope_->garagePageSceneTailCurrent(generation_,page,scene,ordinal,c);
        }
        Result beginGaragePageLightList142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageLightList142590(generation_,page,scene,c);
        }
        Result finishGaragePageLightList1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageLightList1413E0(generation_,page,scene,c);
        }
        Result noteGaragePageAttachmentFailure1B4A38(const PageOwner& page,const SceneOwner& scene,
            const std::array<C*,10>& nodes)noexcept {
            if(!current())return {Status::generation,0x1B4A38};
            return scope_->noteGaragePageAttachmentFailure1B4A38(generation_,page,scene,nodes);
        }
        bool garagePartialPageFailureCurrent(const PageOwner& page,const SceneOwner& scene)const noexcept {
            return current()&&scope_->garagePartialPageFailureCurrent(generation_,page,scene);
        }
        Result preflightGarageFailedCollisionList142590(const PageOwner& page,const SceneOwner& scene,
            const C& c)const noexcept {
            if(!current())return {Status::generation,0x27BB80};
            return scope_->preflightGarageFailedCollisionList142590(generation_,page,scene,c);
        }
        Result beginGarageFailedCollisionList142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x27BB80};
            return scope_->beginGarageFailedCollisionList142590(generation_,page,scene,c);
        }
        bool garageFailedCollisionListBaseCurrent(const PageOwner& page,const SceneOwner& scene,
            const C& c)const noexcept {
            return current()&&scope_->garageFailedCollisionListBaseCurrent(generation_,page,scene,c);
        }
        Result beginGarageLateBorrowAudioList142590(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::owner,0x142590};
            return scope_->beginGarageLateBorrowAudioList142590(generation_,page,scene,ordinal,c);
        }
        Result finishGarageLateBorrowAudioList1413E0(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::owner,0x1413E0};
            return scope_->finishGarageLateBorrowAudioList1413E0(generation_,page,scene,ordinal,c);
        }
        bool garageLateBorrowAudioListsRetired(const PageOwner& page,const SceneOwner& scene)const noexcept {
            return current()&&scope_->garageLateBorrowAudioListsRetired(generation_,page,scene);
        }
        Result beginGarageFailedAudioList142590(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::owner,0x142590};
            return scope_->beginGarageFailedAudioList142590(generation_,page,scene,ordinal,c);
        }
        Result finishGarageFailedAudioList1413E0(const PageOwner& page,const SceneOwner& scene,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::owner,0x1413E0};
            return scope_->finishGarageFailedAudioList1413E0(generation_,page,scene,ordinal,c);
        }
        Result finishGarageFailedCollisionList1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGarageFailedCollisionList1413E0(generation_,page,scene,c);
        }
        bool garageScenePageChildCurrent(const PageOwner& page,const SceneOwner& scene,
            std::size_t ordinal,const C& c)const noexcept {
            return current()&&scope_->garageScenePageChildCurrent(generation_,page,scene,ordinal,c);
        }
        Result bindGarageVisibility272E94(const VisibilityOwner& owner,const SceneOwner& scene,C& root,C& primary,C& secondary) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->bindGarageVisibility272E94(generation_,owner,scene,root,primary,secondary);
        }
        Result bindGarageEnvironment2730F8(const VisibilityOwner& owner,const SceneOwner& scene,C& c) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->bindGarageEnvironment2730F8(generation_,owner,scene,c);
        }
        bool garagePageVisibilityRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,const C& root)const noexcept {
            return current()&&scope_->garagePageVisibilityRetirementCurrent(generation_,page,scene,owner,root);
        }
        Result preflightGaragePageSearchNode27F180(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,const C& c)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightGaragePageSearchNode27F180(generation_,page,scene,owner,ordinal,c);
        }
        Result beginGaragePageSearchNode142590(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,C& c) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageSearchNode142590(generation_,page,scene,owner,ordinal,c);
        }
        bool garagePageSearchNodeBaseCurrent(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,const C& c)const noexcept {
            return current()&&scope_->garagePageSearchNodeBaseCurrent(generation_,page,scene,owner,ordinal,c);
        }
        Result finishGaragePageSearchNode1413E0(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,std::size_t ordinal,C& c) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGaragePageSearchNode1413E0(generation_,page,scene,owner,ordinal,c);
        }
        Result beginGaragePageVisibility142590(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,C& c) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageVisibility142590(generation_,page,scene,owner,c);
        }
        Result finishGaragePageVisibility1413E0(const PageOwner& page,const SceneOwner& scene,const VisibilityOwner& owner,C& c) noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGaragePageVisibility1413E0(generation_,page,scene,owner,c);
        }
        bool garageVisibilitySourceRetired(const SceneOwner& scene,const VisibilityOwner& owner,const C& root)const noexcept {
            return current()&&scope_->garageVisibilitySourceRetired(generation_,scene,owner,root);
        }
        Result beginGaragePageToyListRetirement142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageToyListRetirement142590(generation_,page,scene,c);
        }
        bool garagePageToyListBaseRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const C& c)const noexcept {
            return current()&&scope_->garagePageToyListBaseRetirementCurrent(generation_,page,scene,c);
        }
        Result finishGaragePageToyListRetirement1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageToyListRetirement1413E0(generation_,page,scene,c);
        }
        Result retireGaragePageAnimationManager101FE0(const PageOwner& page,const SceneOwner& scene,
            OriginalGarageAnimationManagerFields50GN& manager,const C& toys)noexcept {
            if(!current())return {Status::generation,0x101FE0};
            return scope_->retireGaragePageAnimationManager101FE0(generation_,page,scene,manager,toys);
        }
        bool garagePageStaticListRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const C& c)const noexcept {
            return current()&&scope_->garagePageStaticListRetirementCurrent(generation_,page,scene,c);
        }
        bool garagePageCollisionListRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const C& c)const noexcept {
            return current()&&scope_->garagePageCollisionListRetirementCurrent(generation_,page,scene,c);
        }
        bool garagePageStaticListBaseRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const C& c)const noexcept {
            return current()&&scope_->garagePageStaticListBaseRetirementCurrent(generation_,page,scene,c);
        }
        Result beginGaragePageStaticListRetirement142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageStaticListRetirement142590(generation_,page,scene,c);
        }
        Result finishGaragePageStaticListRetirement1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageStaticListRetirement1413E0(generation_,page,scene,c);
        }
        bool garagePageCollisionListBaseRetirementCurrent(const PageOwner& page,const SceneOwner& scene,const C& c)const noexcept {
            return current()&&scope_->garagePageCollisionListBaseRetirementCurrent(generation_,page,scene,c);
        }
        Result beginGaragePageCollisionListRetirement142590(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageCollisionListRetirement142590(generation_,page,scene,c);
        }
        Result finishGaragePageCollisionListRetirement1413E0(const PageOwner& page,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageCollisionListRetirement1413E0(generation_,page,scene,c);
        }
        //Defined with complete Camera/Page types by their genuine owner TU.
        //Camera keeps its own Application generation and enrollment.
        Result attachGaragePageCamera1FB620(PageOwner&,OriginalFrontendCameraCold50LA&)noexcept;
        bool garagePageCameraCurrent(const PageOwner&,const OriginalFrontendCameraCold50LA&)const noexcept;
        Result beginGaragePageCameraRetirement142590(PageOwner&,OriginalFrontendCameraCold50LA&)noexcept;
        Result finishGaragePageCameraRetirement1413E0(PageOwner&,OriginalFrontendCameraCold50LA&)noexcept;
        bool garagePageCurrent(const rr::platform::OriginalUiGaragePageOwner50KG* owner,const C& c)const noexcept {
            return current()&&scope_->garagePageCurrent(generation_,owner,c);
        }
        Result beginGarageWattoBindingPageRetirement142590(const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageWattoBindingPageRetirement142590(generation_,owner,scene,c);
        }
        Result beginGarageLoadedHidePageRetirement142590(const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageLoadedHidePageRetirement142590(generation_,owner,scene,c);
        }
        Result beginGarageLateBorrowPageRetirement142590(const PageOwner& owner,const SceneOwner& scene,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageLateBorrowPageRetirement142590(generation_,owner,scene,c);
        }
        bool failedWattoBindingGaragePageRetired1413E0(const PageOwner& owner)const noexcept {
            return current()&&scope_->failedWattoBindingGaragePageRetired1413E0(generation_,owner);
        }
        bool failedLoadedHideGaragePageRetired1413E0(const PageOwner& owner)const noexcept {
            return current()&&scope_->failedLoadedHideGaragePageRetired1413E0(generation_,owner);
        }
        bool failedLateBorrowGaragePageRetired1413E0(const PageOwner& owner)const noexcept {
            return current()&&scope_->failedLateBorrowGaragePageRetired1413E0(generation_,owner);
        }
        Result beginGaragePageRetirement142590(const rr::platform::OriginalUiGaragePageOwner50KG* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageRetirement142590(generation_,owner,c);
        }

        Result beginGaragePageRouterRetirement142590(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageRouterRetirement142590(generation_,owner,c);
        }
        Result finishGaragePageRouterRetirement1413E0(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGaragePageRouterRetirement1413E0(generation_,owner,c);
        }
        Result beginGaragePageControlRetirement142590(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageControlRetirement142590(generation_,owner,ordinal,c);
        }
        Result finishGaragePageControlRetirement1413E0(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGaragePageControlRetirement1413E0(generation_,owner,ordinal,c);
        }
        Result beginGaragePageGroupRetirement142590(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGaragePageGroupRetirement142590(generation_,owner,ordinal,c);
        }
        Result finishGaragePageGroupRetirement1413E0(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGaragePageGroupRetirement1413E0(generation_,owner,ordinal,c);
        }
        Result finishGaragePageRetirement1413E0(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishGaragePageRetirement1413E0(generation_,owner,c);
        }
        bool failedPartialGaragePageRetired1413E0(const PageOwner& owner)const noexcept {
            return current()&&scope_->failedPartialGaragePageRetired1413E0(generation_,owner);
        }
        bool garagePageRetirementCurrent(const PageOwner& owner)const noexcept {
            return current()&&scope_->garagePageRetirementCurrent(generation_,&owner);
        }
        Result constructGaragePodScene1FA308(const SceneOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1FA308};
            return scope_->constructGaragePodScene1FA308(generation_,owner,c);
        }
        bool podSceneOwnerCurrent(const SceneOwner* owner,const C& c)const noexcept {
            return current()&&scope_->garageScene_==&c&&scope_->sceneOwnerCurrent(generation_,owner,false);
        }
        bool scenePodRetirementCurrent(const C& c)const noexcept {
            return current()&&scope_->garageScene_==&c&&scope_->garageSceneRetiring_&&
                scope_->sceneOwnerCurrent(generation_,scope_->garageSceneOwner_,true);
        }
        Result preflightScenePodRetirement142590(const SceneOwner& owner,const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightScenePodRetirement142590(generation_,owner,c,parent);
        }
        Result beginScenePodRetirement142590(const SceneOwner& owner,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginScenePodRetirement142590(generation_,owner,c,parent);
        }
        Result finishScenePodRetirement1413E0(const SceneOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishScenePodRetirement1413E0(generation_,owner,c);
        }
        Result constructGarageSceneCollisionList27BB40(const SceneOwner* scene,C& c)noexcept {
            if(!current())return {Status::generation,0x27BB40};
            return scope_->constructGarageSceneCollisionList27BB40(generation_,scene,c);
        }
        Result constructGarageSceneCollision1B5590(const SceneOwner* scene,const CollisionOwner* owner,
            std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1B5610};
            return scope_->constructGarageSceneCollision1B5590(generation_,scene,owner,ordinal,c);
        }
        Result attachGarageSceneCollision1B577C(const SceneOwner* scene,const CollisionOwner* owner,
            std::size_t ordinal,C& c,C& list)noexcept {
            if(!current())return {Status::generation,0x1B577C};
            return scope_->attachGarageSceneCollision1B577C(generation_,scene,owner,ordinal,c,list);
        }
        Result preflightGarageSceneCollisionRetirement(const SceneOwner* scene,const CollisionOwner* owner,
            std::size_t ordinal,const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightGarageSceneCollisionRetirement(generation_,scene,owner,ordinal,c,parent);
        }
        Result beginGarageSceneCollisionRetirement(const SceneOwner* scene,const CollisionOwner* owner,
            std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageSceneCollisionRetirement(generation_,scene,owner,ordinal,c,parent);
        }
        Result finishGarageSceneCollisionRetirement(const SceneOwner* scene,const CollisionOwner* owner,
            std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGarageSceneCollisionRetirement(generation_,scene,owner,ordinal,c,parent);
        }
        Result preflightGarageSceneParticleList27DCD0(const SceneOwner* owner,const C& c)const noexcept {
            if(!current())return {Status::generation,0x27DCD0};
            return scope_->preflightGarageSceneParticleList27DCD0(generation_,owner,c);
        }
        Result beginGarageSceneParticleList27DCD0(const SceneOwner* owner,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x27DCD0};
            return scope_->beginGarageSceneParticleList27DCD0(generation_,owner,c,parent);
        }
        Result finishGarageSceneParticleList1413E0(const SceneOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x27DCD0};
            return scope_->finishGarageSceneParticleList1413E0(generation_,owner,c);
        }
        Result constructGarageSceneEmitter27C7B0(const SceneOwner* owner,std::size_t ordinal,C& c,C& list)noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->constructGarageSceneEmitter27C7B0(generation_,owner,ordinal,c,list);
        }
        Result enableGarageSceneEmitter27D7A0(const SceneOwner* owner,std::size_t ordinal,C& c,bool enabled)noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->enableGarageSceneEmitter27D7A0(generation_,owner,ordinal,c,enabled);
        }
        Result attachGarageSceneEmitter27C7B0(const SceneOwner* owner,std::size_t ordinal,C& c,C& list)noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->attachGarageSceneEmitter27C7B0(generation_,owner,ordinal,c,list);
        }
        Result preflightGarageSceneEmitter15A720(const SceneOwner* owner,std::size_t ordinal,const C& c)const noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->preflightGarageSceneEmitter15A720(generation_,owner,ordinal,c);
        }
        Result beginGarageSceneEmitter15A720(const SceneOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->beginGarageSceneEmitter15A720(generation_,owner,ordinal,c,parent);
        }
        Result finishGarageSceneEmitter15A720(const SceneOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x27C7B0};
            return scope_->finishGarageSceneEmitter15A720(generation_,owner,ordinal,c);
        }
        Result constructGarageSceneLight273AA0(const SceneOwner* owner,std::size_t ordinal,C& c,C& list)noexcept {
            if(!current())return {Status::generation,0x273AA0};
            return scope_->constructGarageSceneLight273AA0(generation_,owner,ordinal,c,list);
        }
        Result attachGarageSceneLight273B98(const SceneOwner* owner,std::size_t ordinal,C& c,C& list)noexcept {
            if(!current())return {Status::generation,0x273B98};
            return scope_->attachGarageSceneLight273B98(generation_,owner,ordinal,c,list);
        }
        Result constructGarageSceneFog273C30(const SceneOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x273C30};
            return scope_->constructGarageSceneFog273C30(generation_,owner,ordinal,c);
        }
        Result constructGarageSceneParticleManager1B4DC0(const SceneOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1B4DC0};
            return scope_->constructGarageSceneParticleManager1B4DC0(generation_,owner,c);
        }
        Result preflightGarageSceneParticleManager1B4DC0(const SceneOwner* owner,const C& c)const noexcept {
            if(!current()||!scope_->sceneOwnerCurrent(generation_,owner,true))return {Status::generation,0x1B4DC0};
            return scope_->preflightPageSceneChildRetirement(generation_,scope_->sceneParticle_,c,c.parent1C);
        }
        Result beginGarageSceneParticleManager1B4DC0(const SceneOwner* owner,C& c,const C* parent)noexcept {
            if(!current()||!scope_->sceneOwnerCurrent(generation_,owner,true))return {Status::generation,0x1B4DC0};
            return scope_->beginPageSceneChildRetirement(generation_,scope_->sceneParticle_,c,parent);
        }
        Result finishGarageSceneParticleManager1B4DC0(const SceneOwner* owner,C& c)noexcept {
            if(!current()||!scope_->sceneOwnerCurrent(generation_,owner,true))return {Status::generation,0x1B4DC0};
            return scope_->finishPageSceneChildRetirement(generation_,scope_->sceneParticle_,c,scope_->sceneParticle_.retirementParent);
        }
        Result preflightGarageSceneLightRetirement(const SceneOwner* owner,std::size_t ordinal,const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightGarageSceneLightRetirement(generation_,owner,ordinal,c,parent);
        }
        Result beginGarageSceneLightRetirement(const SceneOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageSceneLightRetirement(generation_,owner,ordinal,c,parent);
        }
        Result finishGarageSceneLightRetirement(const SceneOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGarageSceneLightRetirement(generation_,owner,ordinal,c,parent);
        }
        Result preflightGarageSceneFogRetirement(const SceneOwner* owner,std::size_t ordinal,const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightGarageSceneFogRetirement(generation_,owner,ordinal,c,parent);
        }
        Result beginGarageSceneFogRetirement(const SceneOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageSceneFogRetirement(generation_,owner,ordinal,c,parent);
        }
        Result finishGarageSceneFogRetirement(const SceneOwner* owner,std::size_t ordinal,C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->finishGarageSceneFogRetirement(generation_,owner,ordinal,c,parent);
        }
        Result constructGaragePageRouter2A6FD0(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1D6DAC};
            return scope_->constructGaragePageRouter2A6FD0(generation_,owner,c);
        }
        Result attachGaragePageRouter1D6DE4(const PageOwner& owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1D6DAC};
            return scope_->attachGaragePageRouter1D6DE4(generation_,owner,c);
        }
        Result constructGaragePageControl1D5D10(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1D6DAC};
            return scope_->constructGaragePageControl1D5D10(generation_,owner,ordinal,c);
        }
        Result constructGaragePageGroup2A4B70(const PageOwner& owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1D6DAC};
            return scope_->constructGaragePageGroup2A4B70(generation_,owner,ordinal,c);
        }
        bool garagePageCollectionsCurrent(const PageOwner& owner)const noexcept {
            return current()&&scope_->garagePageCollectionsCurrent(generation_,owner);
        }
        OriginalGarageCharacterShared50GN* characterShared()const noexcept{return current()?&scope_->application_->garageCharacterShared_:nullptr;}
        OriginalGarageSphereShared50GN* sphereShared()const noexcept{return current()?&scope_->application_->garageSphereShared_:nullptr;}
        OriginalGarageObserverShared50KG* observerShared()const noexcept{return current()?&scope_->application_->garageObserverShared_:nullptr;}
        OriginalGarageEmitterShared50KG* emitterShared()const noexcept{return current()?&scope_->application_->garageEmitterShared_:nullptr;}
        Result publishAnimationManager(OriginalGarageAnimationManagerFields50GN& manager)noexcept {
            if(!current()||!scope_->ownedGarageChild(manager.entity)||
               manager.entity.kind!=K::animationManager101330||scope_->application_->garageAnimationManagerLive_)
                return {Status::owner,0x101330};
            scope_->application_->garageAnimationManager3DF400_=&manager;
            scope_->application_->garageAnimationManagerLive_=true;
            return {Status::complete,0x101330};
        }
        OriginalGarageAnimationManagerFields50GN* animationManager()const noexcept {
            if(!current()||!scope_->application_->garageAnimationManagerLive_)return nullptr;
            auto* p=scope_->application_->garageAnimationManager3DF400_;
            return p&&scope_->ownedGarageChild(p->entity)?p:nullptr;
        }
        Result bindManagerSearchPayload(const SearchOwner* owner,const C& selected)noexcept {
            if(!current())return {Status::generation,0x1CCD18};
            return scope_->bindManagerSearchPayload(generation_,owner,selected);
        }
        Result constructManagerSearchNode(const SearchOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x27F120};
            return scope_->constructManagerSearchNode(generation_,owner,c);
        }
        const C* managerSearchNode2A4(const SearchOwner* owner)const noexcept {
            return current()?scope_->managerSearchNode2A4(generation_,owner):nullptr;
        }
        Result releaseUnconstructedSearchPredecessor(const SearchOwner* owner)noexcept {
            if(!current())return {Status::generation,0x1CCD44};
            return scope_->releaseUnconstructedSearchPredecessor(generation_,owner);
        }
        Result preflightManagerSearchNodeRetirement(const SearchOwner* owner,const C& c)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightManagerSearchNodeRetirement(generation_,owner,c);
        }
        Result beginManagerSearchNodeRetirement(const SearchOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginManagerSearchNodeRetirement(generation_,owner,c);
        }
        Result finishManagerSearchNodeRetirement(const SearchOwner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishManagerSearchNodeRetirement(generation_,owner,c);
        }
        Result bindGarageUiLightsPayload(const UiLightsOwner* owner,const SearchOwner* search)noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->bindGarageUiLightsPayload(generation_,owner,search);
        }
        Result constructGarageUiLight146720(const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->constructGarageUiLight146720(generation_,owner,ordinal,c);
        }
        Result attachGarageUiLight1CDC8C(const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->attachGarageUiLight1CDC8C(generation_,owner,ordinal,c);
        }
        Result preflightGarageUiLightRetirement(const UiLightsOwner* owner,std::size_t ordinal,const C& c)const noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->preflightGarageUiLightRetirement(generation_,owner,ordinal,c);
        }
        Result beginGarageUiLightRetirement(const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->beginGarageUiLightRetirement(generation_,owner,ordinal,c);
        }
        Result finishGarageUiLightRetirement(const UiLightsOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CDB10};
            return scope_->finishGarageUiLightRetirement(generation_,owner,ordinal,c);
        }
        bool stillBorrowingGarageUiLightsOwner(const UiLightsOwner* owner)const noexcept {
            return owner&&scope_&&epoch_==scope_->managerEpoch_&&scope_->managerUiLightsOwner_==owner;
        }
        bool managerUiLightsPayloadRetirementCurrent(const UiLightsOwner* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->managerUiLightsPayloadRetirementCurrent(generation_,owner);
        }
        Result noteGarageUiLightsPayloadReleased(const UiLightsOwner* owner)noexcept {
            if(!managerUiLightsPayloadRetirementCurrent(owner))return {Status::generation,0x1CCB20};
            return scope_->noteGarageUiLightsPayloadReleased(generation_,owner);
        }
        Result bindGarageManagerFogPayload(const ManagerFogOwner* owner,const UiLightsOwner* lights)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->bindGarageManagerFogPayload(generation_,owner,lights);
        }
        Result constructGarageManagerFogPrefix(const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->constructGarageManagerFogPrefix(generation_,owner,ordinal,c);
        }
        Result publishGarageManagerFogField(const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->publishGarageManagerFogField(generation_,owner,ordinal,c);
        }
        bool managerFogWidgetCustodyCurrent(const ManagerFogOwner* owner,const Widget1Owner* widget)const noexcept {
            return current()&&scope_->managerFogWidgetCustodyCurrent(generation_,owner,widget);
        }
        bool managerFogAttachedCurrent(const ManagerFogOwner* owner,const Widget1Owner* widget)const noexcept {
            return current()&&scope_->managerFogAttachedCurrent(generation_,owner,widget);
        }
        Result attachGarageManagerFog1CD080(const ManagerFogOwner* owner,const Widget1Owner* widget,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CD080};
            return scope_->attachGarageManagerFog1CD080(generation_,owner,widget,ordinal,c);
        }
        Result preflightGarageManagerFogRetirement(const ManagerFogOwner* owner,std::size_t ordinal,const C& c)const noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->preflightGarageManagerFogRetirement(generation_,owner,ordinal,c);
        }
        Result beginGarageManagerFogRetirement(const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->beginGarageManagerFogRetirement(generation_,owner,ordinal,c);
        }
        Result finishGarageManagerFogRetirement(const ManagerFogOwner* owner,std::size_t ordinal,C& c)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->finishGarageManagerFogRetirement(generation_,owner,ordinal,c);
        }
        Result noteGarageManagerFogPayloadReleased(const ManagerFogOwner* owner)noexcept {
            if(!current())return {Status::generation,0x1CCFA0};
            return scope_->noteGarageManagerFogPayloadReleased(generation_,owner);
        }
        bool stillBorrowingGarageManagerFogOwner(const ManagerFogOwner* owner)const noexcept {
            return owner&&scope_&&epoch_==scope_->managerEpoch_&&scope_->managerFogOwner_==owner;
        }
        Result constructGarageTextOverlay29C850(const Widget1Owner* widget,const TextOverlayOwner* owner,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x29C850};
            return scope_->constructGarageTextOverlay29C850(generation_,widget,owner,outer,child);
        }
        Result attachGarageTextOverlay2A9C64(const Widget1Owner* widget,const TextOverlayOwner* owner,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x29C850};
            return scope_->attachGarageTextOverlay2A9C64(generation_,widget,owner,outer,child);
        }
        Result preflightGarageTextOverlayRetirement(const Widget1Owner* widget,const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
            if(!current())return {Status::generation,0x29C850};
            return scope_->preflightGarageTextOverlayRetirement(generation_,widget,owner,outer,child);
        }
        Result beginGarageTextOverlayRetirement(const Widget1Owner* widget,const TextOverlayOwner* owner,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x29C850};
            return scope_->beginGarageTextOverlayRetirement(generation_,widget,owner,outer,child);
        }
        Result finishGarageTextOverlayRetirement(const Widget1Owner* widget,const TextOverlayOwner* owner,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x29C850};
            return scope_->finishGarageTextOverlayRetirement(generation_,widget,owner,outer,child);
        }
        bool garageTextOverlayCurrent(const Widget1Owner* widget,const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
            return current()&&scope_->garageTextOverlayCurrent(generation_,widget,owner,outer,child);
        }
        bool garageTextOverlayIdentityCurrent(const Widget1Owner* widget,const TextOverlayOwner* owner,const C& outer,const C& child)const noexcept {
            return current()&&scope_->garageTextOverlayIdentityCurrent(generation_,widget,owner,outer,child);
        }
        Result hideGarageWidget1_1CD908(const Widget1Owner* widget,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x1CD908};
            return scope_->hideGarageWidget1_1CD908(generation_,widget,outer,child);
        }
        Result publishGarageWidget1Row1_1CD918(const Widget1Owner* widget,C& outer,C& child)noexcept {
            if(!current())return {Status::generation,0x1CD908};
            return scope_->publishGarageWidget1Row1_1CD918(generation_,widget,outer,child);
        }
        Result attachGarageWidget1_1CD9F8(const Widget1Owner* widget,C& outer)noexcept {
            if(!current())return {Status::generation,0x1CD9F8};
            return scope_->attachGarageWidget1_1CD9F8(generation_,widget,outer);
        }
        bool garageWidgetOuterRetirementCurrent(const Widget1Owner* owner,const C& outer)const noexcept {
            return current()&&scope_->preflightWidgetOuterRetirementFinish(generation_,owner,outer,K::uiOverlayText2A8F00).okay();
        }
        bool garageWidgetRowCurrent(const Widget1Owner* owner,std::size_t row,const C& outer)const noexcept {
            return current()&&scope_->widgetRowCurrent(generation_,owner,row,outer);
        }
        // Readonly retained row identity survives the real manager1B8268
        // publication. Construction/show/retirement keep the cold query above.
        bool garageWidgetRowIdentityCurrent(const Widget1Owner* owner,std::size_t row,const C& outer)const noexcept {
            return current()&&scope_->widgetRowIdentityCurrent(generation_,owner,row,outer);
        }
        Result bindGarageWattoRow1CD8B4(const Widget1Owner* owner,
            const rr::platform::OriginalUiGarageWattoOwner50KG* watto,C& outer,
            const C& group,const C& animation,const C& emitter,const C& model)noexcept {
            if(!current())return {Status::generation,0x1CD8B4};
            return scope_->bindGarageWattoRow1CD8B4(generation_,owner,watto,outer,group,animation,emitter,model);
        }
        bool garageWattoRowRetirementCurrent(const Widget1Owner* owner,
            const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)const noexcept {
            return current()&&scope_->garageWattoRowRetirementCurrent(generation_,owner,watto,outer);
        }
        Result beginGarageWattoRowRetirement142590(const Widget1Owner* owner,
            const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageWattoRowRetirement142590(generation_,owner,watto,outer);
        }
        Result noteGarageWattoRowRetired1D4480(const Widget1Owner* owner,
            const rr::platform::OriginalUiGarageWattoOwner50KG* watto,const C& outer)noexcept {
            if(!current())return {Status::generation,0x1D4480};
            return scope_->noteGarageWattoRowRetired1D4480(generation_,owner,watto,outer);
        }
        Result constructGarageOverlayText2A8F00(const Widget1Owner* owner,std::size_t row,C& c)noexcept {
            if(!current())return {Status::generation,0x1CD860};
            return scope_->constructGarageOverlayText2A8F00(generation_,owner,row,c);
        }
        Result hideGarageWidgetRow1CD908(const Widget1Owner* owner,std::size_t row,C& c)noexcept {
            if(!current())return {Status::generation,0x1CD860};
            return scope_->hideGarageWidgetRow1CD908(generation_,owner,row,c);
        }
        Result activateGaragePageWidget90(const Widget1Owner* owner,const PageOwner& page,
            std::size_t row,C& c)noexcept {
            if(!current())return {Status::generation,0x2A5C30};
            return scope_->activateGaragePageWidget90(generation_,owner,page,row,c);
        }
        Result applyGaragePageTextVisibility2A7A00(const Widget1Owner* owner,const PageOwner& page,
            std::size_t row,C& c,bool shown)noexcept {
            if(!current())return {Status::generation,0x2A7A00};
            return scope_->applyGaragePageTextVisibility2A7A00(generation_,owner,page,row,c,shown);
        }
        Result applyGarageRuntimePromptVisibility2A7A00(const Widget1Owner* owner,const PageOwner& page,
            std::size_t row,C& c,bool shown)noexcept {
            if(!current())return {Status::generation,0x2A7A00};
            return scope_->applyGarageRuntimePromptVisibility2A7A00(generation_,owner,page,row,c,shown);
        }
        Result applyGarageDisplayTextVisibility2A7A00(const Widget1Owner* owner,const PageOwner& page,
            std::size_t row,C& c,bool shown)noexcept {
            if(!current())return {Status::generation,0x2A7A00};
            return scope_->applyGarageDisplayTextVisibility2A7A00(generation_,owner,page,row,c,shown);
        }
        Result publishGarageWidgetRow1CD918(const Widget1Owner* owner,std::size_t row,C& c)noexcept {
            if(!current())return {Status::generation,0x1CD860};
            return scope_->publishGarageWidgetRow1CD918(generation_,owner,row,c);
        }
        Result attachGarageWidgetRow1CD9F8(const Widget1Owner* owner,std::size_t row,C& c)noexcept {
            if(!current())return {Status::generation,0x1CD860};
            return scope_->attachGarageWidgetRow1CD9F8(generation_,owner,row,c);
        }
        Result noteGarageEmptyRow1CDA10(const Widget1Owner* owner,std::size_t row,C* const* table)noexcept {
            if(!current())return {Status::generation,0x1CDA10};
            return scope_->noteGarageEmptyRow1CDA10(generation_,owner,row,table);
        }
        Result advanceGarageWidgetRow1CDA1C(const Widget1Owner* owner,std::size_t row)noexcept {
            if(!current())return {Status::generation,0x1CDA1C};
            return scope_->advanceGarageWidgetRow1CDA1C(generation_,owner,row);
        }
        Result constructGarageMovement2AAF40(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->constructGarageMovement2AAF40(generation_,owner,movement,c);
        }
        Result preflightGarageMovementRetirement(const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->preflightGarageMovementRetirement(generation_,owner,movement,c);
        }
        Result beginGarageMovementRetirement(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->beginGarageMovementRetirement(generation_,owner,movement,c);
        }
        Result finishGarageMovementRetirement(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->finishGarageMovementRetirement(generation_,owner,movement,c);
        }
        Result hideGarageMovement1CD908(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->hideGarageMovement1CD908(generation_,owner,movement,c);
        }
        Result publishGarageMovementRow1CD918(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->publishGarageMovementRow1CD918(generation_,owner,movement,c);
        }
        Result attachGarageMovement1CD9F8(const Widget1Owner* owner,const MovementOwner* movement,C& c)noexcept {
            if(!current())return {Status::generation,0x2AAF40};
            return scope_->attachGarageMovement1CD9F8(generation_,owner,movement,c);
        }
        bool garageMovementCurrent(const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
            return current()&&scope_->garageMovementCurrent(generation_,owner,movement,c);
        }
        bool garageMovementIdentityCurrent(const Widget1Owner* owner,const MovementOwner* movement,const C& c)const noexcept {
            return current()&&scope_->garageMovementIdentityCurrent(generation_,owner,movement,c);
        }
        Result bindGarageWidget1Payload(const Widget1Owner* owner,const ManagerFogOwner* fog)noexcept {
            if(!current())return {Status::generation,0x1CD7A0};
            return scope_->bindGarageWidget1Payload(generation_,owner,fog);
        }
        Result publishGarageWidget1Table(const Widget1Owner* owner,const void* table)noexcept {
            if(!current())return {Status::generation,0x1CD850};
            return scope_->publishGarageWidget1Table(generation_,owner,table);
        }
        Result constructGarageWidget1Prefix(const Widget1Owner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x2A8F00};
            return scope_->constructGarageWidget1Prefix(generation_,owner,c);
        }
        Result preflightGarageWidget1Retirement(const Widget1Owner* owner,const C& c)const noexcept {
            if(!current())return {Status::generation,0x2A8F70};
            return scope_->preflightGarageWidget1Retirement(generation_,owner,c);
        }
        Result beginGarageWidget1Retirement(const Widget1Owner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x2A8F70};
            return scope_->beginGarageWidget1Retirement(generation_,owner,c);
        }
        Result finishGarageWidget1Retirement(const Widget1Owner* owner,C& c)noexcept {
            if(!current())return {Status::generation,0x2A8F70};
            return scope_->finishGarageWidget1Retirement(generation_,owner,c);
        }
        Result noteGarageWidget1DetachedReleased(const Widget1Owner* owner)noexcept {
            if(!current())return {Status::generation,0x2A8F70};
            return scope_->noteGarageWidget1DetachedReleased(generation_,owner);
        }
        bool stillBorrowingGarageWidget1Owner(const Widget1Owner* owner)const noexcept {
            return owner&&scope_&&epoch_==scope_->managerEpoch_&&scope_->managerWidget1Owner_==owner;
        }
        bool managerWidget1TableRetirementCurrent(const Widget1Owner* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->managerWidget1TableRetirementCurrent(generation_,owner);
        }
        Result noteGarageWidget1TableReleased(const Widget1Owner* owner)noexcept {
            if(!managerWidget1TableRetirementCurrent(owner))return {Status::generation,0x2A3B18};
            return scope_->noteGarageWidget1TableReleased(generation_,owner);
        }
        Result preflightGarageManagerPayloadRetirement(const SearchOwner* owner)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightGarageManagerPayloadRetirement(generation_,owner);
        }
        Result beginGarageManagerPayloadRetirement(const SearchOwner* owner)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginGarageManagerPayloadRetirement(generation_,owner);
        }
        bool stillBorrowingManagerSearchOwner(const SearchOwner* owner)const noexcept {
            // Native identity must survive payload release through final1413E0.
            // This observation intentionally works while ordinary current() is
            // false during the manager ref0 payload interval.
            return owner&&scope_&&epoch_==scope_->managerEpoch_&&
                scope_->managerSearchOwner_==owner;
        }
        //Normal1D18FC only: the actual manager has entered ref0 payload
        //retirement after its complete attached child forest. This query
        //deliberately does not use current(), which rejects ref0 payloads.
        bool garageFailedPartialScenePayloadCurrent(const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->failedGaragePartialScenePayloadCurrent(generation_,owner);
        }
        bool garageFailedWattoBindingPayloadCurrent(const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->failedGarageWattoBindingPayloadCurrent(generation_,owner);
        }
        bool garageFailedLoadedHidePayloadCurrent(const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->failedGarageLoadedHidePayloadCurrent(generation_,owner);
        }
        bool garageFailedLateBorrowPayloadCurrent(const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->failedGarageLateBorrowPayloadCurrent(generation_,owner);
        }
        bool garageFailedSharedLoaderPayloadCurrent(const rr::platform::OriginalUiGarageSession50KG* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->failedGarageSharedLoaderPayloadCurrent(generation_,owner);
        }
        bool garageTransferRetirement1D18FCCurrent()const noexcept {
            return scope_&&generation_==scope_->managerGeneration_&&epoch_==scope_->managerEpoch_&&
                scope_->managerForestRetiring_&&
                scope_->managerPayloadRetirementCurrent(generation_,scope_->managerSearchOwner_)&&
                scope_->completedManagerPage1413E0_&&
                scope_->completedManagerPageGeneration_==generation_&&scope_->completedManagerPageEpoch_==epoch_&&
                scope_->managerSearchNodeRetired_&&!scope_->managerSearchSelected_&&
                scope_->managerUiLightsRetired_[0]&&scope_->managerUiLightsRetired_[1]&&
                scope_->managerFogPayloadReleased_&&scope_->managerWidget1DetachedReleased_&&
                !scope_->managerWidget1TableReleased_&&!scope_->managerSearchPayloadReleased_&&
                !scope_->managerUiLightsPayloadReleased_&&scope_->managerPageTableOwner_&&
                !scope_->managerPageTableReleased_;
        }
        bool managerPayloadRetirementCurrent(const SearchOwner* owner)const noexcept {
            return scope_&&epoch_==scope_->managerEpoch_&&
                scope_->managerPayloadRetirementCurrent(generation_,owner);
        }
        Result noteManagerSearchPayloadReleased(const SearchOwner* owner)noexcept {
            if(!managerPayloadRetirementCurrent(owner))return {Status::generation,0x1CCADC};
            return scope_->noteManagerSearchPayloadReleased(generation_,owner);
        }
        Result finishGarageManagerPayloadRetirement(const SearchOwner* owner)noexcept {
            if(!managerPayloadRetirementCurrent(owner))return {Status::generation,0x1413E0};
            return scope_->finishGarageManagerPayloadRetirement(generation_,owner);
        }
        Result reservePopulation(std::size_t n)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->reserveGarageChildren(generation_,n);}
        Result constructActorLoopSound2795E0(const ActorOwner& owner,std::size_t ordinal,C& toys,C& c)noexcept {
            if(!current())return {Status::generation,0x2795E0};
            return scope_->constructActorLoopSound2795E0(generation_,owner,ordinal,toys,c);
        }
        Result retireActorLoopSoundReadFalse142590(const ActorOwner& owner,std::size_t ordinal,const C& toys,C& c)noexcept {
            if(!current())return {Status::generation,0x2D8FE4};
            return scope_->retireActorLoopSoundReadFalse142590(generation_,owner,ordinal,toys,c);
        }
        Result constructPrefix(C& c,K k)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->constructGaragePrefix(generation_,c,k);}
        Result enrollConstructedWattoAnimation(OriginalRacerAnimationCold50LA& a)noexcept {
            if(!current())return {Status::generation,0x1536F0};
            return scope_->enrollConstructedWattoAnimation(generation_,a);
        }
        Result enrollConstructedModel(OriginalRacerModelSceneOwner50LA& s)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->enrollGarageModel(generation_,s);}
        Result attach(C& p,C& c,std::uint32_t mask)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->attachGarageChild(generation_,p,c,mask);}
        Result attachModel(C& parent,OriginalRacerModelSceneOwner50LA& scene,std::uint32_t mask)noexcept {
            if(!current()||!scope_->modelUsesApplication(scene))return {Status::generation,0x141960};
            return scope_->attachGarageModel(generation_,parent,scene,mask);
        }
        Result retireEmptyPrefix(C& c,K k,const C* p)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->retireGaragePrefix(generation_,c,k,p);}
        Result beginStaticListRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginStaticListRetirement(generation_,c,nullptr);
        }
        Result finishStaticListRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishStaticListRetirement(generation_,c);
        }
        Result beginToyListRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginToyListRetirement(generation_,c,nullptr);
        }
        Result finishToyListRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishToyListRetirement(generation_,c);
        }
        Result preflightCharacterRetirement(const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightCharacterRetirement(generation_,c,parent);
        }
        Result beginCharacterRetirement(C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginCharacterRetirement(generation_,c,parent);
        }
        Result finishCharacterRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishCharacterRetirement(generation_,c);
        }
        Result preflightWattoAnimationRetirement(const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightWattoAnimationRetirement(generation_,c,parent);
        }
        Result beginWattoAnimationRetirement(C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginWattoAnimationRetirement(generation_,c,parent);
        }
        Result finishWattoAnimationRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishWattoAnimationRetirement(generation_,c);
        }
        // Same canonical Entity/25-slot algorithms as Watto; no second table
        // or separate capacity policy. Scene owns a detached parent=null group.
        bool scenePodPrefixCurrent(const C& c)const noexcept {
            return current()&&scope_->ownedGarageChild(c)&&c.kind==K::entity141300;
        }
        Result constructSceneSoundGroup2350D0(C& c,std::int32_t& slot)noexcept {
            return constructWattoSoundGroup2350D0(c,slot);
        }
        bool sceneSoundGroupCurrent(const C& c,std::int32_t slot)const noexcept {
            return wattoSoundGroupCurrent(c,slot);
        }
        bool noteSceneSoundInsert235270(const C& c,std::int32_t slot)noexcept {
            return noteWattoSoundInsert235270(c,slot);
        }
        Result constructWattoSoundGroup2350D0(C& c,std::int32_t& slot)noexcept {
            if(!current())return {Status::generation,0x2350D0};
            return scope_->constructWattoSoundGroup2350D0(generation_,c,slot);
        }
        bool wattoSoundGroupCurrent(const C& c,std::int32_t slot)const noexcept {
            return current()&&scope_->wattoSoundGroupCurrent(generation_,c,slot);
        }
        bool noteWattoSoundInsert235270(const C& c,std::int32_t slot)noexcept {
            return current()&&scope_->noteWattoSoundInsert235270(generation_,c,slot);
        }
        bool readWattoSoundInsert235270(const C& c,std::int32_t slot,
            std::uint8_t& guard,std::uint32_t& assertion)const noexcept {
            if(!wattoSoundGroupCurrent(c,slot))return false;
            guard=scope_->application_->soundInsertGuard881C_;
            assertion=scope_->application_->soundInsertAssertion434A88_;return true;
        }
        Result preflightWattoSoundGroupRetirement(const C& c,const C* parent,std::int32_t slot)const noexcept {
            if(!current())return {Status::generation,0x235150};
            return scope_->preflightWattoSoundGroupRetirement(generation_,c,parent,slot);
        }
        Result beginWattoSoundGroupRetirement(C& c,const C* parent,std::int32_t slot)noexcept {
            if(!current())return {Status::generation,0x235150};
            return scope_->beginWattoSoundGroupRetirement(generation_,c,parent,slot);
        }
        Result finishWattoSoundGroupRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishWattoSoundGroupRetirement(generation_,c);
        }
        Result preflightWattoWidgetRetirement(const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightWattoWidgetRetirement(generation_,c,parent);
        }
        Result beginWattoWidgetRetirement(C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginWattoWidgetRetirement(generation_,c,parent);
        }
        Result finishWattoWidgetRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishWattoWidgetRetirement(generation_,c);
        }
        Result preflightWattoEmitterRetirement(const C& c,const C* parent)const noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->preflightWattoEmitterRetirement(generation_,c,parent);
        }
        Result beginWattoEmitterRetirement(C& c,const C* parent)noexcept {
            if(!current())return {Status::generation,0x142590};
            return scope_->beginWattoEmitterRetirement(generation_,c,parent);
        }
        Result finishWattoEmitterRetirement(C& c)noexcept {
            if(!current())return {Status::generation,0x1413E0};
            return scope_->finishWattoEmitterRetirement(generation_,c);
        }
        Result retireModel(OriginalRacerModelSceneOwner50LA& s)noexcept{if(!current())return {Status::generation,0x1D1950};return scope_->retireGarageModel(generation_,s);}
    public:
        GarageConstructionGrant(const GarageConstructionGrant&)=delete;
        GarageConstructionGrant& operator=(const GarageConstructionGrant&)=delete;
    };
private:
    Result issueGarageConstruction(const std::shared_ptr<OriginalUiEntityScope50KG>& retained,
        std::uint64_t generation,std::shared_ptr<GarageConstructionGrant>& out)noexcept {
        auto r=live();if(!r.okay())return r;
        if(out||retained.get()!=this||!managerLive_||managerPublished_||managerForestRetiring_||generation!=managerGeneration_)
            return {Status::phase,0x1D1950};
        try{out=std::shared_ptr<GarageConstructionGrant>(new GarageConstructionGrant(retained,generation));}
        catch(...){return {Status::allocation,0x1D1950};}
        return {Status::complete,0x1D1950};
    }
public:
    ~OriginalUiEntityScope50KG(){if(!close().okay())std::terminate();}
    OriginalUiEntityScope50KG(const OriginalUiEntityScope50KG&)=delete;
    OriginalUiEntityScope50KG& operator=(const OriginalUiEntityScope50KG&)=delete;
    bool dispatcherLive()const noexcept{return dispatcherLive_&&!closed_;}
    bool garageManagerPublished()const noexcept{return managerLive_&&managerPublished_&&!closed_;}
};
}
