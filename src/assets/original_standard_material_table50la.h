#pragma once
namespace original_local_flare50LA {class Emission;}
#include "original_memory_card_screen_fallback50kg.h"
namespace original_main_entry50KG {class NativeCollectorFifo50LA;class NativeF8BarPrefix50LA;}
namespace original_cable_selection50LA { class Binding; }
namespace original_position_stats_prestage50LA { class Owner; }
#include "original_standard_model_resource50la.h"
#include "original_standard_material_object50la.h"
#include <memory>
#include <vector>

namespace ra_static_world50LA {class Owner;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {class OriginalRaceF850LA; class OriginalResults10450LA; class OriginalPositionStatsResources50LA;class OriginalPauseMaterialAdmission50LA;}
namespace rr::platform { class OriginalUiSharedDialogMaterialHistory50KG; class OriginalUiTournamentPodReferences50KG; class OriginalUiSharedDialogs50KG; }
namespace rr::platform {class OriginalUiMemoryCardConstructor50KG;class OriginalUiGarageModelResource50KG;class OriginalUiGarageScreenText50KG;class OriginalUiGarageQuad50KG;class OriginalUiGaragePageOwner50KG;}
namespace rr::platform { class OriginalUiGarageCanonicalSubmission50KG; class OriginalUiGarageDrawContribution50KG; }
namespace rr::assets {
// A partial CPU callback outcome, never a renderer capability. The first
// enabled pass cannot return until24B9B0; subsequent passes remain untouched.
enum class OriginalCableMaterialCallbackStage50LA : std::uint8_t {
    blocked,materialReturned,rendererSubmissionRequired
};
class OriginalUiScreenMaterialReference50KG;
class OriginalStandardMaterialReferences50LA;
class OriginalStandardScreenMaterialReference50LA;
class OriginalPausePrimitiveReferences50LA;
class OriginalResultsPrimitiveReferences50LA;
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialDomain50LA;
class OriginalStandardMaterialAnimationInstance50LA;
// A sealed observation of this actual table's complete current registry.
// Initially construction-state; subsequent accepted native bindings remain
// on their REAL pass owners and participate in the next14D440 comparison.
// Only a whole-table rebuild mints this value. It is not a rendered-frame
// admission, effective249610 binding receipt or FIFO-drain receipt.
// The same application/table must outlive this borrowed value.
class OriginalStandardMaterialCollectorReceipt50LA final {
public:
    OriginalStandardMaterialCollectorReceipt50LA() noexcept=default;
private:
    friend class OriginalStandardMaterialTable50LA;
    const OriginalStandardMaterialTable50LA* owner_=nullptr;
    std::uint64_t generation_=0;
};
struct OriginalStandardMaterialCollectorEntryView50LA {
    // Borrowed observation only. Validate its seal again before reuse after
    // any table mutation; row release can destroy this material immediately.
    std::int32_t materialRow=-1,pass=-1,registration=-1,redirect18=-1,limit1C=-1,finalLeaf=-1;
    const OriginalStandardMaterialObject50LA* material=nullptr;
};
enum class OriginalStandardMaterialMapPreparation50LA : std::uint8_t {
    disabled,nullTexture,ready,absentMap
};
// Borrowed, prospective native binding inputs for one actual FINAL leaf map.
// This is not a seal, accepted bind/history mutation, selected resident mip,
// GPU upload, queue submission or FIFO drain. Discard this view before any
// table/resource mutation and reacquire through the checked preparation API;
// a collector seal alone does not version later texture population.
struct OriginalStandardMaterialMapBindingView50LA {
    OriginalStandardMaterialCollectorEntryView50LA origin{},representative{};
    OriginalStandardMaterialMapPreparation50LA preparation=OriginalStandardMaterialMapPreparation50LA::disabled;
    std::size_t map=0;
    bool nativeUntexturedRoute=false; //Exact zero-map249BA0 state, not an image binding.
    bool nativeParticleRoute=false; //Private retained ParticleCode two-context route.
    bool nativeScreenPrototypeRoute=false; //Actual private ScreenObject null-key route, not a source field.
    std::int16_t textureToken=-1;
    const OriginalStandardTextureResource50LA* texture=nullptr;
    // Constructor fields plus ONLY the proven per-map249610 TEXA overwrite.
    // No TEX0 address/residency or dynamic ZBUF state is manufactured here.
    OriginalStandardMaterialStateContext50LA contextAfter249610{};
};
// Stack-only prospective input inspection. No public constructor/accessor,
// submission method, accepted-state bit or source frame/scene default exists.
// The concrete strip callback retains its material reference throughout and
// acquires the actual display pin before native attribute inspection. This
// object cannot escape in the callback result.
class OriginalCableEmissionBinding50LA final {
    friend class ::original_local_flare50LA::Emission;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class OriginalStandardMaterialTable50LA;
    friend class ::original_cable_selection50LA::Binding;
    OriginalCableEmissionBinding50LA()=default;
    OriginalCableEmissionBinding50LA(OriginalCableEmissionBinding50LA&&)=default;
    OriginalCableEmissionBinding50LA(const OriginalCableEmissionBinding50LA&)=delete;
    OriginalCableEmissionBinding50LA& operator=(const OriginalCableEmissionBinding50LA&)=delete;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    OriginalStandardMaterialCollectorReceipt50LA registry_;
    OriginalStandardMaterialCollectorEntryView50LA origin_,representative_;
    OriginalStandardMaterialPassView50LA originatingPass_,effectivePass_;
    //24BA0C..24BA24 keeps the first/origin node's parameters across redirect.
    const OriginalStandardMaterialParameters50LA* originatingParameters_=nullptr;
    const OriginalStandardMaterialAnimationInstance50LA* vertexBase_=nullptr;
    const OriginalStandardMaterialTextureAnimation50LA* textureBase_=nullptr;
    std::array<OriginalStandardMaterialMapBindingView50LA,2> maps_{};
    std::size_t mapCount_=0;
};
enum class OriginalStandardMaterialStatus50LA : std::uint8_t {
    complete,unprepared,alreadyPrepared,input,archive,templates,table,name,
    missingName,range,reference,overflow,allocation,phase,missingMaterialObject,source,document,passCount,unsupportedAction,unsupportedPass,
    program,texture,runtime,video,duplicatePublication,arithmetic,demandTerminal,collector
};
struct OriginalStandardMaterialDiagnostic50LA {
    OriginalStandardMaterialStatus50LA status=OriginalStandardMaterialStatus50LA::complete;
    std::uint32_t sourcePC=0;
    std::int32_t row=-1;
    std::size_t slot=0,storedSlots=0,retainedSlots=0;
    bool sourceEffectsCommitted=false;
    std::size_t populationVisited=0,populationPublished=0,pass=0;
    std::size_t map=0,demandMapsVisited=0,nullTexturesSkipped=0,texturesDemanded=0;
    bool textureDemandWritten=false;
    const char* field=nullptr;
    OriginalUiPageDiagnostic50KG document{};
    OriginalStandardProgramDiagnostic50LA program{};
    OriginalStandardTextureDiagnostic50LA texture{};
};
struct OriginalStandardMaterialRowView50LA {
    const std::string* name00=nullptr;
    const OriginalStandardMaterialObject50LA* object04=nullptr;
    const std::int32_t* references08=nullptr;
    const OriginalUiMaterialTemplateState50KG* associated30=nullptr;
    std::uint32_t tableByteOffset=0;
};
// Borrowed staging observations only. Constructing/copying these values gives
// no mutation authority. Only the actual common history owner may hold the
// table's private lease and publish an accepted UI transaction. Fixed order is
// 3228/dim, 3229/overlay1, 3230/venus16, 3231/venus14, 3232/icon_x,
// 3233/icon_tri; all are their own actual pass0.
enum class OriginalStandardMaterialUiSharedHistoryWrite50LA : std::uint8_t {untextured,bound};
struct OriginalStandardMaterialUiSharedHistorySlot50LA {
    std::int32_t row=-1;
    std::size_t pass=0;
    const OriginalStandardMaterialObject50LA* object=nullptr;
    const OriginalStandardMaterialState50LA* state30=nullptr;
    const std::array<OriginalStandardMaterialNativeBinding50KG,2>* nativeBindings=nullptr;
    OriginalStandardMaterialStateContext50LA beforeContext{},afterContext{};
    std::array<OriginalStandardMaterialNativeBinding50KG,2> beforeNativeBindings{},afterNativeBindings{};
    bool write=false;
    OriginalStandardMaterialUiSharedHistoryWrite50LA writeKind=OriginalStandardMaterialUiSharedHistoryWrite50LA::untextured;
};
// Actual native reference/population portion of the source382B38 table, not the
// UI lifetime projection. Its one shared application owner must be retained
// across models/scenes. Only a fresh constructor -> first Mat_Stbl load is
// admitted; this cannot adopt/reset an already-running manager or infer that
// existing foreign owners are absent. Default construction is UNPREPARED.
// Serial and nonreentrant; no table mutation during scene/renderer borrows.
class OriginalStandardMaterialTable50LA final {
    friend class rr::platform::OriginalUiMemoryCardConstructor50KG;
    friend class rr::platform::OriginalUiGarageModelResource50KG;
    friend class rr::platform::OriginalUiGarageScreenText50KG;
    friend class rr::platform::OriginalUiGarageQuad50KG;
    friend class rr::platform::OriginalUiGaragePageOwner50KG;
    friend class rr::platform::OriginalUiSharedDialogs50KG;
public:
    OriginalStandardMaterialTable50LA() noexcept;
    ~OriginalStandardMaterialTable50LA();
    OriginalStandardMaterialTable50LA(const OriginalStandardMaterialTable50LA&)=delete;
    OriginalStandardMaterialTable50LA& operator=(const OriginalStandardMaterialTable50LA&)=delete;
    // Uses authenticated APPSTART and its real shared24A8E0/24A9A0 templates.
    // Populates every table row in file order, including duplicate names.
    // Null object and zero refs are source29888C/2988B0 stores, not defaults for
    // an unknown live manager. No PSM is loaded and no14D370 success is claimed.
    bool prepareFresh(std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<const OriginalUiMaterialTemplates50KG>,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    // Preferred join to the SAME actual application APPSTART/template owner.
    // This aliases its retained templates, never makes a second template copy.
    bool prepareFresh(std::shared_ptr<const OriginalUiMaterialApplication50LA>,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool prepared() const noexcept;
    std::size_t rowCount() const noexcept;
    bool row(std::int32_t,OriginalStandardMaterialRowView50LA&) const noexcept;
    // Genuine298C80 first-match, ASCII case-insensitive. Slash spelling stays
    // significant. Missing names reject; they never become row0 or new rows.
    bool find298C80(const std::string&,std::int32_t&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    // Genuine298E60 read of canonical row+4. A valid pending row returns true
    // with null output; caller MUST NOT call24A430 on that absent owner.
    bool resolve298E60(std::int32_t,const OriginalStandardMaterialObject50LA*&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    // Executes source14D370 in table order. Only positive-ref/null-object rows
    // construct. Prior published rows survive later terminal failure; partial
    // construction stays owned but unpublished. No collector/demand/render call.
    // The same application program/texture and sealed context bindings must
    // be supplied on every call. Actual source members may grow between calls.
    bool populate14D370(const OriginalStandardMaterialPopulationInput50LA&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool populationTerminal() const noexcept;
    //14D440 after the actual14D370/29E340 population pair. Registers EVERY
    // nonnull row and every declared pass, including disabled/foreign passes,
    // in canonical row/pass order. Unique texture prototypes never merge;
    // unique vertex animation still may merge. Group unique(-2) never merges;
    // none(-1) is ordinary. Named/nonnegative groups remain closed.
    // Preserves the source redirect/barrier algorithm and4096 registration cap.
    // Once clear starts, failure leaves any partial registry unsealed and no
    // prior receipt remains current. Rejection before clear changes no state.
    bool rebuildColdCollector14D440(OriginalStandardMaterialCollectorReceipt50LA&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool coldCollectorCurrent(const OriginalStandardMaterialCollectorReceipt50LA&) const noexcept;
    std::size_t coldCollectorCount(const OriginalStandardMaterialCollectorReceipt50LA&) const noexcept;
    bool coldCollectorEntry(const OriginalStandardMaterialCollectorReceipt50LA&,std::size_t,
        OriginalStandardMaterialCollectorEntryView50LA&) const noexcept;
    bool coldCollectorPass(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t row,std::size_t pass,OriginalStandardMaterialCollectorEntryView50LA&) const noexcept;
    // Read-only preparation from this exact sealed current registry. Resolves
    // the final entry FIRST, then its enabled pass, actual map token and SAME
    // texture table. Null resources remain a genuine no-bind result.
    // This ordinary path rejects texture animation: it has no null-key UI
    // caller proof. Native IPUM ownership is real; RenderTarget/residency
    // emulation remains closed. This does not itself bind a native image.
    // Output is unchanged on failure; success does not mutate source history.
    bool prepareColdRepresentativeMap249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t row,std::size_t pass,std::size_t map,
        OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    //Explicit ScreenTexture2DF864 null-object-key route. Unique texture leaves
    //select their SAME actual prototype, then102050 real child. No racer clone.
    //Read-only current-phase binding only; this never dispatches animation.
    bool prepareColdScreenPrototypeMap249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t row,std::size_t pass,std::size_t map,
        OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    // Authentic246810 ->24A430 over one immutable source surface. The override
    // is the model's retained1C4 surface-ordinal handle; only -1 uses entry+0.
    // Completes texture demand only, including the genuine null-resource skip.
    // This is never a collector, GPU upload, geometry or renderer receipt.
    bool demandSurface246810(const OriginalStandardModelResource50LA&,
        std::size_t lod,std::size_t surfaceOrdinal,std::int32_t materialOverride,
        float projectedRadius,float scale,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTerminal() const noexcept;
    // Null until the corresponding source lazy initialization actually ran.
    const float* demandScale4356E0() const noexcept;
    const std::uint32_t* demandUvAssertion4356E8() const noexcept;
    const OriginalStandardMaterialObject50LA* partialConstruction() const noexcept;
    // Read-only source14D370 next pending row in ascending table order.
    // This is inspection, never a population/completion callback.
    bool firstPending14D370(std::int32_t& row,const std::string*& name,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
private:
    friend class OriginalStandardMaterialDomain50LA;
    // Actual Application17CAD4 ->14D2F0 ->2985B0 consumes the service's decoded
    // full-item read only AFTER131040 closes its virtual member (not the persistent device handle). The domain is
    // the sole issuer and keeps this table private until template attachment.
    // Bytes must equal this SAME authenticated APPSTART Mat_Stbl member.
    bool prepareAppStartNames17CAD4(std::shared_ptr<const OriginalUiArchive50KG>,
        const std::vector<std::uint8_t>& sourceBytes,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    // Aliases the same actual application's templates after their success.
    // Promotes the SAME Storage/rows, without another parse or table copy.
    bool attachAppStartTemplates(std::shared_ptr<const OriginalUiMaterialApplication50LA>,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    friend class rr::platform::OriginalUiSharedDialogMaterialHistory50KG;
    using UiSharedHistoryOwner=rr::platform::OriginalUiSharedDialogMaterialHistory50KG;
    using UiSharedHistorySlots=std::array<OriginalStandardMaterialUiSharedHistorySlot50LA,6>;
    // Not a collector seal or FIFO receipt. There is no public constructor,
    // copy, move, setter, ready flag, registration or producer outside the
    // actual common owner and this table. Storage pins the receipt address as
    // well as its owner so a copied/reconstructed value cannot release a lease.
    class UiSharedHistoryReceipt final {
        friend class OriginalStandardMaterialTable50LA;
        friend class rr::platform::OriginalUiSharedDialogMaterialHistory50KG;
        UiSharedHistoryReceipt()noexcept=default;
        ~UiSharedHistoryReceipt()noexcept;
        UiSharedHistoryReceipt(const UiSharedHistoryReceipt&)=delete;
        UiSharedHistoryReceipt& operator=(const UiSharedHistoryReceipt&)=delete;
        const OriginalStandardMaterialTable50LA* table_=nullptr;
        const UiSharedHistoryOwner* owner_=nullptr;
        const OriginalStandardMaterialContext50LA* context_=nullptr;
        std::uint64_t leaseRevision_=0,bindingRevision_=0;
        UiSharedHistorySlots snapshots_{};
        std::array<OriginalStandardMaterialPassView50LA,6> passViews_{};
        std::array<std::int32_t,6> retainedReferences_{};
    };
    bool beginUiSharedHistory(const UiSharedHistoryOwner*,const OriginalStandardMaterialContext50LA*,
        UiSharedHistoryReceipt&,UiSharedHistorySlots&)noexcept;
    bool tableOwnerUiSharedHistory(const UiSharedHistoryOwner*,const OriginalStandardMaterialContext50LA*,
        const UiSharedHistoryReceipt&)const noexcept;
    bool bindingRevisionUiSharedHistory(const UiSharedHistoryOwner*,const UiSharedHistoryReceipt&,
        std::uint64_t&)const noexcept;
    bool uiSharedHistoryCurrent(const UiSharedHistoryOwner*,const UiSharedHistoryReceipt&)const noexcept;
    bool commitUiSharedHistory(const UiSharedHistoryOwner*,UiSharedHistoryReceipt&,
        const UiSharedHistorySlots&)noexcept;
    bool cancelUiSharedHistory(const UiSharedHistoryOwner*,UiSharedHistoryReceipt&)noexcept;
    bool releaseUiSharedHistory(const UiSharedHistoryOwner*,UiSharedHistoryReceipt&)noexcept;
    bool poisonUiSharedHistory(const UiSharedHistoryOwner*,const UiSharedHistoryReceipt&)noexcept;
    bool captureUiSharedHistorySlot(std::size_t,OriginalStandardMaterialUiSharedHistorySlot50LA&,
        OriginalStandardMaterialPassView50LA&,std::int32_t&)const noexcept;
    bool ownsUiSharedHistoryLease(const UiSharedHistoryOwner*,const UiSharedHistoryReceipt&)const noexcept;
    bool uiSharedHistoryQuiescent()const noexcept;
    // Read-only lifetime/poison observation, not a binding/renderer grant.
    // Actual canonical FIFO pins are permitted; no collector seal is minted.
    bool nativeBindingHistoryUsable()const noexcept;
    friend class rr::platform::OriginalUiGarageCanonicalSubmission50KG;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    bool prepareGarageUntexturedState249BA0(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool prepareGarageNativeBinding249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t row,std::size_t pass,OriginalStandardMaterialMapBindingView50LA&,
        OriginalUiGarageProgram50KG&,OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool garageNativeBindingsCurrent(const OriginalStandardMaterialCollectorReceipt50LA&,
        const std::vector<OriginalStandardMaterialMapBindingView50LA>&)const noexcept;
    void commitGarageNativeBindings249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        const std::vector<OriginalStandardMaterialMapBindingView50LA>&,
        const std::vector<OriginalStandardMaterialNativeBinding50KG>&)noexcept;
    void poisonGarageNativeBindings()noexcept;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::original_main_entry50KG::NativeF8BarPrefix50LA;
    friend class rr::game::OriginalPauseMaterialAdmission50LA;
    bool serviceRetainedScreenPrototype24A740(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool bindUiNativeCollector1B73BC(const ::original_main_entry50KG::NativeCollectorFifo50LA&)noexcept;
    bool canRetireNativeCollector29C300()const noexcept;
    void retireNativeCollector29C300()noexcept;
    //Exact native allocation identity of the registered29C0B0 entry. The
    //receipt expires before its backing can be reused; never dereference an
    //expired source1C8. Its queue heads/epoch are the SAME entry's fields.
    const void* nativeCollectorEntry29C0B0(const OriginalStandardMaterialCollectorReceipt50LA&,std::size_t)const noexcept;
    bool appendNativeCollector29C150(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::size_t,std::uint32_t,void* record,void*& previousTail)noexcept;
    bool pinNativeCollector(const OriginalStandardMaterialCollectorReceipt50LA&) noexcept;
    void unpinNativeCollector() noexcept;
    friend class ::ra_static_world50LA::Owner;
    // Only the actual retained full-world Placement proof may enter here.
    // No public caller supplies a row or model pointer for this route.
    bool serviceRetainedModelPrototype24A740(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,const rr::game::OriginalApplicationAnimationClockView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool prepareRetainedModelMap24B26C(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,std::size_t,OriginalStandardMaterialMapBindingView50LA&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool prepareGarageParticleMap249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    enum class PrototypeRoute {ordinary,screenNullKey,retainedModel,cableNullKey};
    bool prepareColdMap249610(const OriginalStandardMaterialCollectorReceipt50LA&,
        std::int32_t,std::size_t,std::size_t,PrototypeRoute,
        OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    friend class OriginalUiScreenMaterialReference50KG;
    bool serviceTournamentMarker24A740(std::int32_t,const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        const rr::game::OriginalApplicationAnimationClockView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool retainTournamentFallback14D590(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool screenApplicationMatches(const OriginalUiMaterialApplication50LA&) const noexcept;
    // Exact UI470 first pass, reachable only from its retained native owner.
    bool demandSharedWarning2DF8A0(std::int32_t,const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandGarage2DF8A0(std::int32_t,const OriginalStandardTextureTable50LA&,
        std::uint32_t storePC,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentFallback2DF8A0(std::int32_t,const OriginalStandardTextureTable50LA&,
        const OriginalUiArchive50KG&,std::uint32_t storePC,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentMarker2DF8A0(std::int32_t,const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool demandTournamentOutline2DF8A0(std::int32_t,const OriginalStandardTextureTable50LA&,
        const OriginalUiMemberView50KG&,const OriginalUiMemberView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    friend class rr::platform::OriginalUiTournamentPodReferences50KG;
    friend class OriginalStandardMaterialReferences50LA;
    friend class OriginalStandardScreenMaterialReference50LA;
    friend class OriginalRadarOverlayMaterialReference50LA;
    friend class rr::game::OriginalPositionStatsResources50LA;
    friend class ::original_position_stats_prestage50LA::Owner;
    friend class OriginalTimeStatsMaterialReference50LA;
    friend class OriginalSpeedBoostMaterialReference50LA;
    friend class OriginalDamageMaterialReference50LA;
    friend class OriginalPausePrimitiveReferences50LA;
    friend class OriginalResultsPrimitiveReferences50LA;
    friend class rr::game::OriginalRaceF850LA;
    friend class rr::game::OriginalResults10450LA;
    bool bindResultsPendingAfterRetain(std::int32_t,
        std::shared_ptr<const OriginalResultsScreenFallback50LA>,OriginalStandardMaterialDiagnostic50LA&)noexcept;

    bool bindResultsBaseBackdropAfterRetain(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool pauseBackdropFallback14D590(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,bool retain,OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool retainPauseBackdropFallback14D590(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool retainDamageFallback14D590(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool retainSpeedBoostFallback14D590(std::int32_t,std::shared_ptr<const OriginalUiArchive50KG>,
        std::shared_ptr<OriginalStandardTextureTable50LA>,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    friend class OriginalStandardParticleMaterialReference50LA;
    friend class OriginalStandardCableMaterialReference50LA;
    friend class ::ra_pod_scene50LA::Owner; //Concrete World SunFlare primitive retirement.
    friend class ::ra_cold_aggregate50LA::Owner; //Concrete World SunFlare retain and application checks.
    friend class ::original_local_flare50LA::Emission;
    bool inspectLocalFlareEmission24B9B0(std::int32_t,
        OriginalCableEmissionBinding50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    friend class ::original_cable_selection50LA::Binding;
    bool selectRacerMaterial29BFF0(std::int32_t,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool inspectCableEmission24B9B0(std::int32_t,std::size_t,
        OriginalCableEmissionBinding50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    OriginalCableMaterialCallbackStage50LA serviceCableNullKey24A620(
        std::int32_t,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    // All concrete caller families mutate the SAME row. Association is the actual
    // retained application template, selected only by these concrete owners.
    bool retain14D590(std::int32_t,OriginalUiTemplate50KG,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool retainDefault14D590(std::int32_t,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool bindMemoryCardPendingAfterRetain(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        std::shared_ptr<const OriginalMemoryCardScreenFallback50KG>,OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool clearMemoryCardPendingForRetirement(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool preflightMemoryCardPopulation(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        bool& firstPending,bool& noTexturePending,OriginalStandardMaterialDiagnostic50LA&,std::size_t role=0)const noexcept;
    bool preflightGarageLoadedSharedIcon(const rr::platform::OriginalUiGarageQuad50KG&,
        OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool preflightMemoryCardStripPopulation(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        bool&,bool&,OriginalStandardMaterialDiagnostic50LA&,std::size_t role=4)const noexcept;
    bool preflightMemoryCardStripBinding(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        OriginalStandardMaterialDiagnostic50LA&,std::size_t role=4)const noexcept;
    bool preflightMemoryCardIconBinding(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        std::size_t,OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool preflightMemoryCardFirstIconBinding(const rr::platform::OriginalUiMemoryCardConstructor50KG&,
        OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool preflightRetain14D590(std::int32_t,OriginalStandardMaterialDiagnostic50LA&)const noexcept;
    bool retainScreen14D590(std::int32_t,OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool bindGarageScreenFallbackAfterRetain(std::int32_t,
        std::shared_ptr<const OriginalGarageScreenFallback50KG>,OriginalStandardMaterialDiagnostic50LA&)noexcept;
    //Non-mutating exact14D500 eligibility; allows bounded concrete owners to
    //preflight BEFORE disposing dependent resources. No admission/population.
    bool canRelease14D500(std::int32_t) const noexcept;
    bool release14D500(std::int32_t) noexcept;
    struct Storage;
    // Every existing consumer uses storage_ as its template-ready boundary.
    // Names-only storage is unreachable through ALL public row/lookup APIs,
    // including prepared(), until the private attachment promotes it intact.
    std::unique_ptr<Storage> appStartNames_;
    std::unique_ptr<Storage> storage_;
};
// The ACTUAL caller-owned36B970 path text at26E6AC, borrowed only during the
// cold call. Null means unknown, not empty. Empty is an explicitly known source
// value. No track/UI prefix or slash normalization is inferred from filenames.
struct OriginalStandardMaterialPath50LA { const std::string* actualPrefix36B970=nullptr; };
// One model's canonical count1C0 and retained handle array1C4. Each slot owns
// one actual numeric source ref, including duplicate names. Partial acquisition
// is terminal; its real retained prefix survives until ordered destruction.
// No caller-set handle, reference count, or constructed-object setter exists.
class OriginalStandardMaterialReferences50LA final {
public:
    OriginalStandardMaterialReferences50LA() noexcept;
    ~OriginalStandardMaterialReferences50LA();
    OriginalStandardMaterialReferences50LA(const OriginalStandardMaterialReferences50LA&)=delete;
    OriginalStandardMaterialReferences50LA& operator=(const OriginalStandardMaterialReferences50LA&)=delete;
    bool acquire26E640(const OriginalStandardModelResource50LA&,
        std::shared_ptr<OriginalStandardMaterialTable50LA>,const OriginalStandardMaterialPath50LA&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    //26EB44..26EBA8: copy the live prototype's actual count/handles, then
    //retain every copied slot with DefaultMT. Do not resolve source names
    //again: prototype handles can differ after an admitted material change.
    bool clone26EB44(const OriginalStandardMaterialReferences50LA&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    //Explicit ordered26E460 phase. Preflight includes duplicate-slot counts
    //and never mutates the shared manager. Release is idempotent; owned handle
    //backing stays stable until native destruction, after geometry cleanup.
    bool canRelease26E4C0() const noexcept;
    bool release26E4C0(OriginalStandardMaterialDiagnostic50LA&) noexcept;
    bool released26E4C0() const noexcept;
    //26E5EC after geometry release and source-row-table disposal. Keeps the
    // model's source1C4 value stale; native slot inspection ends here.
    bool freeAllocation26E5EC() noexcept;
    bool started() const noexcept;
    bool complete() const noexcept;
    const std::uint32_t* count1C0() const noexcept;
    const std::int32_t* handle1C4(std::size_t surfaceOrdinal) const noexcept;
    //26E684 allocation identity only; unwritten slots are not valid handles.
    const std::int32_t* allocationBase1C4() const noexcept;
    const OriginalStandardMaterialTable50LA* table382B38() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
