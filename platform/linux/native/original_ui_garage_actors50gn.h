#pragma once
// Retained native Garage actor ownership. This is deliberately NOT a Page,
// draw, renderer or route capability. Construction stops at any unjoined
// source operation rather than completing ten records with skipped effects.
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
#include "../../../src/game/original_garage_character50gn.h"
#include "../../../src/game/original_garage_sequence50gn.h"
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <memory>

#include "../../../tests/native/original_ui_source_timing50lh.h"

namespace rr::platform {
struct OriginalUiMainApplication50KG;
class OriginalUiMainAudio50KG;
class OriginalUiGarageVisibility50KG;
class OriginalUiGarageSession50KG;
class OriginalUiGaragePodScene50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageRendererLease50KG;
class OriginalUiGarageActors50GN final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGaragePageOwner50KG;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using R=OriginalUiGarageModelResource50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
public:
    enum class Stage : std::uint8_t {
        empty,owners,character,animationCache,model,motion,placement,
        membership,initialPose,soundRows,complete,closed,toyDescriptors,pageCharacters,pageToyDescriptors,pageToyRetired,pageManagerRetired,loopSoundPrefix
    };
    struct Observation {
        Stage stage=Stage::empty;
        std::size_t actor=0,constructedCharacters=0,completedActors=0;
        const game::OriginalWreckComponent50LA* toyList3C8=nullptr;
        const game::OriginalGarageAnimationManagerFields50GN* manager49C=nullptr;
        const game::OriginalWreckComponent50LA* character290=nullptr;
        const game::OriginalGarageCharacterFields50GN* fields=nullptr;
        const R* model=nullptr;
        bool sourceManagerPublished=false,sourceOrdered=false,failed=false;
        std::size_t loopSoundDescriptor=0,retiredLoopSoundLists=0;
        const game::OriginalWreckComponent50LA* loopSoundEntity=nullptr;
        std::int32_t loopSoundCount30=0;
        bool loopSoundConstructed=false,loopSoundReadComplete=false,loopSoundReadFalse=false,loopSoundReleased=false;
    };
    OriginalUiGarageActors50GN()noexcept;
    ~OriginalUiGarageActors50GN();
    OriginalUiGarageActors50GN(const OriginalUiGarageActors50GN&)=delete;
    OriginalUiGarageActors50GN& operator=(const OriginalUiGarageActors50GN&)=delete;
    bool inspect(Observation&)const noexcept;
private:
    // Only the actual root Application may issue these exact retained inputs.
    // No production issuance exists until the live Session seam is joined.
    class RootInputs final {
        friend struct ::original_main_entry50KG::Application;
        friend class OriginalUiGarageActors50GN;
        const ::original_main_entry50KG::Application* issuer_;
        std::shared_ptr<Grant> grant_;
        std::shared_ptr<OriginalUiMainApplication50KG> application_;
        std::shared_ptr<game::OriginalMotionLibrary50LA> motions_;
        std::shared_ptr<OriginalUiMainAudio50KG> audio_;
        // Empty cold-only inputs cannot authorize runtime. The actual root must
        // bind its own pause publication and OriginalMain timing owner here.
        game::OriginalApplicationAnimationClockView50KG expectedPause_;
        ::original_ui_source_timing50LH::PublicationView expectedTiming_;
        std::shared_ptr<game::HostMonotonicClock50ID> expectedClock101A50_;
        RootInputs(const ::original_main_entry50KG::Application* issuer,
            std::shared_ptr<Grant> grant,std::shared_ptr<OriginalUiMainApplication50KG> application,
            std::shared_ptr<game::OriginalMotionLibrary50LA> motions,
            std::shared_ptr<OriginalUiMainAudio50KG> audio,
            game::OriginalApplicationAnimationClockView50KG pause={},
            ::original_ui_source_timing50LH::PublicationView timing={})noexcept;
    };
    // Issued only by the actual root at its prepared SourceTiming publication.
    // No raw250, caller delta, boolean renderer readiness or supplied callback.
    // Publication storage is retained and invalidated by owner destruction or
    // commit. Root integration has not issued this capability yet.
    class RuntimeFrame final {
        friend struct ::original_main_entry50KG::Application;
        friend class OriginalUiGarageActors50GN;
        std::shared_ptr<const RootInputs> roots_;
        game::OriginalApplicationAnimationClockView50KG pause_;
        ::original_ui_source_timing50LH::PublicationView timing_;
        std::uint64_t publication_=0;
        std::shared_ptr<OriginalUiGarageRendererLease50KG> renderer_;
        RuntimeFrame(std::shared_ptr<const RootInputs>,
            std::shared_ptr<OriginalUiGarageRendererLease50KG>)noexcept;
    };
    struct RuntimeReceipt {
        game::GarageSequenceReceipt50GN manager{};
        std::uint64_t publication=0;
        std::size_t visitedCharacters=0,updatedCharacters=0;
        bool managerComplete=false,toyListComplete=false,pendingRenderer=false;
    };
    struct Storage;
    std::unique_ptr<Storage> storage_;
    // The actual Scene source order splits TOY descriptor custody from the
    // later manager/character population. The entire grant is reserved once.
    // EARLY: 1FB280 ->271CF0 ->271D34 ->2713E0, before2729E0.
    bool prepareToyList2713F8(OriginalUiGaragePodScene50KG&,std::shared_ptr<Grant>,
        OriginalUiGarageModelResources50KG&,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,D&)noexcept;
    // LATE: 1FB3E4 ->1B40B0 ->2742F0, after274160 statics AND2743B0 collision population.
    bool prepareAnimationManager2742F0(std::shared_ptr<const RootInputs>,
        std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    // Compatibility ONLY for the existing detached component slice. It is
    // NOT the authentic Scene entry and cannot issue sourceOrdered custody.
    bool prepareOwners2713F8(std::shared_ptr<Grant>,
        OriginalUiGarageModelResources50KG&,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<const RootInputs>,
        std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    bool constructToyDescriptors2713F8(const assets::OriginalUiDat50KG&,std::shared_ptr<Grant>,
        OriginalUiGarageModelResources50KG&,
        std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,D&)noexcept;
    bool finishPodSceneActorScan1B40B0(OriginalUiGaragePodScene50KG&,D&)noexcept;
    bool borrowPageEntities1B4A68(const OriginalUiGaragePodScene50KG&,
        game::OriginalWreckComponent50LA*&,game::OriginalWreckComponent50LA*&)noexcept;
    bool scenePopulationCurrent2742F0(const OriginalUiGaragePodScene50KG&)const noexcept;
    bool constructManager274324(std::shared_ptr<const RootInputs>,
        std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    bool toyDescriptorsCurrent()const noexcept;
    // One authored record at a time. Before the next actor is constructed the
    // current one must have real mode2 membership, initial pose,28EA20 and its
    // concrete sound-row construction. No success-shaped callback is accepted.
    bool constructNext2D9060(D&)noexcept;
    bool continueInitial2D946C(D&)noexcept;
    // Entered source stages only, not the outer Garage Page's renderer prelude
    // or root scheduling. Manager completion remains pending until the exact
    // ToyList intrusive traversal and every renderer244D60(0) actually run.
    bool serviceManager101860(std::shared_ptr<const RuntimeFrame>,RuntimeReceipt&,D&)noexcept;
    bool continueToyList141ED0(std::shared_ptr<const RuntimeFrame>,RuntimeReceipt&,D&)noexcept;
    bool trigger101C10(const char*,const char*,D&)noexcept;
    static bool readTimestamp101A50(void*,std::uint32_t&)noexcept;
    static bool afterSample11AD80(void*,std::size_t,const game::GarageActorBinding50GN&,
        game::GarageSequenceDiagnostic50GN&)noexcept;
    bool currentRuntimeFrame(const RuntimeFrame&,float&,std::uint32_t&)const noexcept;
    void publishPlaybackFields()noexcept;
    // Exact synchronous post-sample10179C failure; never a completed frame.
    bool failedSampleFrameCurrent(const RuntimeFrame&,std::uint64_t)const noexcept;
    bool failedSampleFrameReleasedCurrent(std::uint64_t)const noexcept;
    bool failedSampleRetirementCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireFailedSampleToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retireFailedSampleAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    // Normal Page ordinal8 then ordinal7, never detached cold cleanup. The
    // real Page/Scene ledger proves the tail predecessor before mutation.
    bool retirePageToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retirePageAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    // Distinct failed late-borrow authority; the real attached destructor body
    // remains shared, but normal resolved-audio admission is not weakened.
    bool retireFailedLateBorrowToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retireFailedLateBorrowAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    // Distinct completed-load/hide-preflight failure. This does not assert
    // Page activation, entry, Sound resolution, or a zero-late attachment path.
    bool retireFailedLoadedPageToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retireFailedLoadedPageAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool failedLoadedPageActorAudioCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireFailedWattoBindingToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retireFailedWattoBindingAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool failedWattoBindingActorAudioCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool retireAttachedToyList142590(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retireAttachedAnimationManager101FE0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool failedLateBorrowActorAudioCurrent(const OriginalUiGaragePageOwner50KG&)const noexcept;
    // Live runtime counterpart of the retirement predicate below: the same
    // actual actor/model/Entity identity while the whole population is
    // attached in source order and no Page retirement has begun.
    bool pageActorModelLiveCurrent(const OriginalUiGaragePageOwner50KG&,std::size_t,const R&)const noexcept;
    bool pageActorModelRetirementCurrent(const OriginalUiGaragePageOwner50KG&,
        std::size_t,const R&)const noexcept;
    // Cold-only cleanup; no GPU submission/retirement is asserted here.
    bool closeCold142590(D&)noexcept;
};
}
