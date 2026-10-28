#pragma once
// Concrete Widget10 constructor and private source-ordered Page entry/lifetime.
// Constructor completion alone never acquires a Scene zone or creates an Observer.
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include "../../../src/game/original_ui_garage_watto_lifecycle50kg.h"
#include "../../../tests/native/original_ui_source_timing50lh.h"
#include <memory>

namespace rr::game { class OriginalMotionLibrary50LA; }
namespace rr::platform {
struct OriginalUiMainApplication50KG;
class OriginalUiMainAudio50KG;
class OriginalUiGarageWattoSoundGroup50KG;
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageWidget1Owner50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalAudioPendingWaves50KG;
class OriginalUiGarageRendererLease50KG;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiGarageWattoOwner50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using R=OriginalUiGarageModelResource50KG;
    using C=game::OriginalWreckComponent50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
public:
    enum class Stage : std::uint8_t {
        empty,widget,audioGroup,animation,cache,model,motions,placement,
        sounds,prepared,entered,closed,wingPending,flushPending,reverbPending
    };
    struct Observation {
        Stage stage=Stage::empty;
        const C* widget=nullptr;
        const C* animation220=nullptr;
        const C* soundGroup=nullptr;
        const C* responseEmitter=nullptr;
        const R* model=nullptr;
        const game::OriginalUiGarageWattoFields50KG* fields=nullptr;
        const game::OriginalRacerAnimationColdFields50LA* animationFields=nullptr;
        bool selected38=false,failed=false;
        // Unrestricted ticks still need actual wet voice/DSP production.
        // A returned silent callback alone is not runtime admission.
        bool tickAvailable=false;
    };
    OriginalUiGarageWattoOwner50KG()noexcept;
    ~OriginalUiGarageWattoOwner50KG();
    OriginalUiGarageWattoOwner50KG(const OriginalUiGarageWattoOwner50KG&)=delete;
    OriginalUiGarageWattoOwner50KG& operator=(const OriginalUiGarageWattoOwner50KG&)=delete;
    bool inspect(Observation&)const noexcept;
private:
    // Same-loop manager row10 custody only, not Page/Visibility readiness.
    // The original constructor must actually return before these identities
    // can be consumed by the manager's common hide/table/attach suffix.
    struct ColdPublication {
        std::shared_ptr<Grant> grant;
        C* widget=nullptr;
        const C* animation220=nullptr;
        const C* modelEntity8=nullptr;
        const C* soundGroup=nullptr;
        const C* responseEmitter=nullptr;
    };
    bool constructorPublication(ColdPublication&)const noexcept;
    bool managerRowPublication(ColdPublication&)const noexcept;
    bool subtreeCurrent(bool constructorOnly)const noexcept;
    bool activatePageWidget90(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageWidget1Owner50KG&,D&)noexcept;
    bool enterPage1FAC1C(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    // Only the real retained Page transaction may issue source1D5320:
    //0 purchase,1 no-money,2 slider,3 no-purchase. Read-only preflight precedes
    // page mutations;0/3 execute after the actual transition request returns,
    // and2 executes before the slider delta/cash stores. No private audio clock.
    // Native bool reports a completed source call, not the source scratch v0
    // or a chosen response. The receipt distinguishes no-op/probability refusal.
    bool canRespondPage1D5320(const OriginalUiGaragePageOwner50KG&,std::uint32_t,
        game::OriginalUiGarageWattoLifecycleDiagnostic50KG&)const noexcept;
    bool respondPage1D5320(const OriginalUiGaragePageOwner50KG&,std::uint32_t,
        game::OriginalUiGarageWattoReceipt50KG&,game::OriginalUiGarageWattoLifecycleDiagnostic50KG&)noexcept;
    bool stopPageSpeech2924A0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool responseDuration1D5320(std::uint32_t,int&)const noexcept;
    bool remainingMotion1D5C10(const OriginalUiGaragePageOwner50KG&,float&,D&)const noexcept;
    //1D4E60 tail-jumps to inherited141D70. The actual Draw must perform its
    //existing renderer-service/child+48 traversal on this exact live receiver.
    //A successful borrow is NOT an empty draw or a completed source callback.
    C* borrowChildDraw1D4E60(const OriginalUiGaragePageOwner50KG&,const C&)const noexcept;
    bool constructorSubtreeCurrent()const noexcept;
    bool storeWants1CD8F4(std::size_t,bool,D&)noexcept;
    bool borrowPageWants50(const C*,std::array<std::int32_t,2>&)const noexcept;
    bool preflightColdRetirement142590(D&)const noexcept;
    bool preflightManagerRetirement142590(D&)const noexcept;
    bool closeManagerChildren142590(D&)noexcept;
    bool closeManager142590(D&)noexcept;
    bool preflightRetirement142590(bool normalManager,D&)const noexcept;
    bool closeChildren142590(bool normalManager,D&)noexcept;
    bool close142590(bool normalManager,D&)noexcept;
    bool retirementCustody(std::shared_ptr<Grant>&,const C*&,bool&)const noexcept;
    class RootInputs final {
        friend struct ::original_main_entry50KG::Application;
        friend class OriginalUiGarageWattoOwner50KG;
        const ::original_main_entry50KG::Application* issuer_;
        std::shared_ptr<Grant> grant_;
        std::shared_ptr<OriginalUiMainApplication50KG> application_;
        std::shared_ptr<game::OriginalMotionLibrary50LA> motions_;
        std::shared_ptr<OriginalUiMainAudio50KG> audio_;
        ::original_ui_source_timing50LH::PublicationView expectedTiming_;
        RootInputs(const ::original_main_entry50KG::Application* issuer,
            std::shared_ptr<Grant> grant,std::shared_ptr<OriginalUiMainApplication50KG> application,
            std::shared_ptr<game::OriginalMotionLibrary50LA> motions,
            std::shared_ptr<OriginalUiMainAudio50KG> audio,
            ::original_ui_source_timing50LH::PublicationView timing={})noexcept
            :issuer_(issuer),grant_(std::move(grant)),application_(std::move(application)),
             motions_(std::move(motions)),audio_(std::move(audio)),expectedTiming_(std::move(timing)){}
    };
    // Available even after a failed bounds update, so removal never requires
    // repairing a poisoned graph or pretending membership succeeded.
    struct VisibilityBinding {
        std::shared_ptr<Grant> grant;
        std::shared_ptr<R> model;
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> sceneAssets;
        std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets;
        std::shared_ptr<OriginalUiGarageVisibility50KG> visibility;
        const C* widget=nullptr;
        const C* animation220=nullptr;
        const void* animationModel04=nullptr;
        const C* primary4C0=nullptr;
    };
    bool visibilityBinding(VisibilityBinding&)const noexcept;
    //Read only the genuine normal manager/model destruction cursor. The
    //Widget and Animation remain live ref1 until this model's280920 tail.
    bool normalModelObserverRetirementCurrent(const OriginalUiGarageVisibility50KG&,
        const R&,const OriginalUiGaragePageOwner50KG*)const noexcept;
    bool bindPageVisibility1FAC08(const OriginalUiGaragePageOwner50KG&,
        std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    bool observerUpdateCall1D4DD0(const OriginalUiGarageVisibility50KG&,
        const OriginalUiGaragePageOwner50KG*&,std::uint64_t&)const noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    bool prepare1D4510(std::shared_ptr<Grant>,OriginalUiGarageModelResources50KG&,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>,
        std::shared_ptr<const RootInputs>,D&)noexcept;
    // Private resumptions at actual source calls, not ready flags. Root issues
    // its own26D9F0 capability only after the exact pre-flush predicate holds.
    bool continueWingRegistration1D464C(std::shared_ptr<OriginalAudioPendingWaves50KG>,D&)noexcept;
    bool pendingFlush232EC0(std::shared_ptr<OriginalAudioPendingWaves50KG>)const noexcept;
    bool continueAfterFlush1D4688(std::shared_ptr<OriginalAudioPendingWaves50KG>,D&)noexcept;
    // Genuine Page entry1FAC08, not part of1D4320/1D4510. This boundary stays
    // closed until the actual Page4C0 -> Scene4C0[0] owner can supply its zone;
    // a detached Visibility/PodScene prefix is not that capability. The later
    // widget+78=1D5C40 and per-frame28EA20 are separate source operations.
    bool enter1D4800(game::OriginalUiGarageWattoReceipt50KG&,
        game::OriginalUiGarageWattoLifecycleDiagnostic50KG&)noexcept;
    // Actual same-Page canonical tick, fed by the already prepared same-Root
    // timing publication. No new entry/capture, clock sample or audio owner.
    // A failed reached callback retains its prefix and cannot be replayed.
    bool advance1D4B10(const OriginalUiGaragePageOwner50KG&,
        const ::original_ui_source_timing50LH::PublicationView&,std::uint64_t,
        OriginalUiGarageRendererLease50KG&,game::OriginalUiGarageWattoReceipt50KG&,
        game::OriginalUiGarageWattoLifecycleDiagnostic50KG&)noexcept;
    bool closeCold142590(D&)noexcept;
    bool closeChildren142590(D&)noexcept;
    bool constructEmitter1D4850(D&)noexcept;
};
}
