#pragma once
#include "original_ui_overlay_text_storage50kg.h"
#include "../../../tests/native/original_ui_source_timing50lh.h"
#include "original_ui_garage_manager_fog50kg.h"
#include "original_ui_garage_text_overlay50kg.h"
#include "original_ui_garage_movement_owner50kg.h"
#include "original_ui_garage_watto_owner50kg.h"
#include "../../../src/assets/original_application_localization50kg.h"
#include <atomic>
#include <thread>

namespace rr::platform {
enum class OriginalUiGarageWidget1Status50KG : std::uint8_t {
    complete,authority,phase,source,allocation,scope,selectedTextPending,textOverlay,wattoConstructorPending,watto,unsupportedType
};
struct OriginalUiGarageWidget1Diagnostic50KG {
    OriginalUiGarageWidget1Status50KG status=OriginalUiGarageWidget1Status50KG::complete;
    std::uint32_t sourcePC=0;
    assets::OriginalUiPageDiagnostic50KG parse{};
    assets::OriginalApplicationLocalizationDiagnostic50KG localization{};
    OriginalUiGarageTextOverlayDiagnostic50KG text{};
    OriginalUiGarageMovementDiagnostic50KG movement{};
    assets::OriginalStandardMaterialDiagnostic50LA watto{};
    std::size_t row=0,line=0;
};
// One actual manager154/158 and its retained source-ordered cold loop.
// The historical class identity is preserved; there is no second manager/DAT.
// All22 source slots share this one retained loop. Watto10 requires its real
// completed constructor. The separate Fog owner performs the later1CD080 join;
// Page/Scene readiness, update/draw and Garage admission remain separate.
class OriginalUiGarageWidget1Owner50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend class OriginalUiGarageTextOverlay50KG;
    friend class OriginalUiGarageManagerFog50KG;
    using C=game::OriginalWreckComponent50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Fog=OriginalUiGarageManagerFog50KG;
    using Authority=game::OriginalApplicationEntities50LA;
    using Dictionary=assets::OriginalApplicationLocalization50KG;
    using TextOverlay=OriginalUiGarageTextOverlay50KG;
    using Movement=OriginalUiGarageMovementOwner50KG;
    using Watto=OriginalUiGarageWattoOwner50KG;
    using Page=OriginalUiGaragePageOwner50KG;
    using D=OriginalUiGarageWidget1Diagnostic50KG;
    using S=OriginalUiGarageWidget1Status50KG;
public:
    enum class Stage:std::uint8_t {empty,bound,datLoaded,countStored,tablePublished,
        constructed,loading,selectedTextPending,detachedRetired,tableRetired,
        copyingText,lineCountStored,lineArrayPublished,lineConstructed,lineSlotPublished,
        lineLoaded,lineAttached,textCopied,lineConfigured,hidden,rowStored,rowAttached};
    struct Observation {
        Stage stage=Stage::empty;const C* entity=nullptr;const C* manager30=nullptr;
        std::int32_t count158=0;bool countKnown=false,tableKnown=false,tableAllocated=false;
        std::uintptr_t source154Identity=0;std::array<bool,22> slotsKnown{};
        bool constructed=false,retiring=false,detachedReleased=false,tableReleased=false;
        bool fontKnown=false,materialKnown=false,xKnown=false,yKnown=false,leadingKnown=false;
        bool justificationKnown=false,dynamicLengthKnown=false,typeKnown=false;
        std::array<char,128> font68{},materialE8{};
        std::uint32_t type34=0,field38=0,field3C=0,field40=0,field44=0,field48=0;
        std::uint32_t field4C=0,field50=0,field54=0,field58=0,field64=0;
        bool lineArrayNull=true;std::int32_t lineCount168=0,justify170=0,dynamic178=0;
        float leading174=0,depth190=0;std::int32_t x180=0,y184=0,height188=0;
        std::int32_t alpha18C=0;std::uint32_t color17C=0,highlight194=0,highlight198=0;
        std::uint32_t disabled19C=0,disabled1A0=0,field1A4=0;
        bool lookupReached=false,hashAssertionReached=false,idAssertionReached=false;
        std::int32_t lookupOrdinal=0;std::uint32_t pendingPC=0;
        const C* lineEntity=nullptr;std::uintptr_t source60Identity=0;
        bool lineSlotKnown=false,lineConstructed=false,lineLoaded=false,lineAttached=false;
        bool textCopied=false,lineConfigured=false,hidden=false,rowStored=false,outerAttached=false;
        bool selectedBorrowLive=false,lineRetired=false,lineArrayReleased=false;
    };
    OriginalUiGarageWidget1Owner50KG()noexcept=default;
    ~OriginalUiGarageWidget1Owner50KG()noexcept;
    OriginalUiGarageWidget1Owner50KG(const OriginalUiGarageWidget1Owner50KG&)=delete;
    OriginalUiGarageWidget1Owner50KG& operator=(const OriginalUiGarageWidget1Owner50KG&)=delete;
    // Means this pending prefix is retained, NEVER that Widget1 is loaded.
    bool currentPending()const noexcept;
    // Only this exact hidden cold row, never the complete22-row manager.
    bool currentColdRow1()const noexcept;
    bool currentColdPrefix9()const noexcept;
    // True only after all22 genuine slots and source DAT destruction1CDA34.
    // Never a Page, Scene, display, audio-entry or Garage-admission predicate.
    bool currentColdComplete()const noexcept;
    Observation inspectRow(std::size_t row)const noexcept;
    Observation inspect()const noexcept;
private:
    // Only the actual root may mint this immutable capability. Friendship of
    // the enclosing Widget1 does not grant access to this nested constructor.
    class RootInputs final {
        friend struct ::original_main_entry50KG::Application;
        const ::original_main_entry50KG::Application* const issuer_;
        const std::shared_ptr<Authority> authority_;
        const std::shared_ptr<const Dictionary> dictionary_;
        RootInputs(const ::original_main_entry50KG::Application* issuer,
            std::shared_ptr<Authority> authority,std::shared_ptr<const Dictionary> dictionary)noexcept
            :issuer_(issuer),authority_(std::move(authority)),dictionary_(std::move(dictionary)){}
    public:
        const ::original_main_entry50KG::Application* issuer()const noexcept{return issuer_;}
        const std::shared_ptr<Authority>& authority()const noexcept{return authority_;}
        const std::shared_ptr<const Dictionary>& dictionary()const noexcept{return dictionary_;}
    };
    //Exact manager154[authoredId-1] borrow for Page Control1D7284.
    bool borrowPageWidget1D7284(std::size_t authoredId,C*&,std::array<std::int32_t,2>&)const noexcept;
    // Exact1FBAB0 call site only. This is retained runtime custody, not a
    // public color setter, cold-ready substitute or Page admission token.
    bool setPageEntryColor2AA7F0(const Page&,std::size_t,std::uint32_t,D&)noexcept;
    bool pageEntryCustodyCurrent(const Page&)const noexcept;
    bool setPageDrawText2A9FF0(const Page&,OriginalUiGarageDrawContribution50KG&,std::size_t,std::string_view,D&)noexcept;
    bool setPageDrawVisibility2A7A00(const Page&,OriginalUiGarageDrawContribution50KG&,std::size_t,bool,D&)noexcept;
    bool setPageDrawPosition2AA1D0(const Page&,OriginalUiGarageDrawContribution50KG&,
        std::size_t,std::int32_t,std::int32_t,D&)noexcept;
    bool pageCustodyIdentityCurrent(const Page&)const noexcept;
    bool pageActivationCustodyCurrent(const Page&)const noexcept;
    bool activatePageWidget90(const Page&,std::size_t,D&)noexcept;
    bool pageWattoShowCurrent90(const Page&,const Watto&)const noexcept;
    static std::array<std::size_t,4> activationCallKey(const Page&)noexcept;
    bool applyPageEntryWidgetCall(const Page&,std::size_t,std::uint32_t,std::int32_t,D&)noexcept;
    static std::array<std::size_t,6> entryCallKey(const Page&)noexcept;
    using OverlayText=OriginalUiOverlayTextFields50KG;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::shared_ptr<Authority> authority_;std::shared_ptr<Grant> grant_;
    std::shared_ptr<Fog> fog_;std::shared_ptr<const Dictionary> dictionary_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    std::shared_ptr<assets::OriginalStandardArchiveProvider50LA> provider_;
    std::uint64_t providerRevision_=0;
    assets::OriginalUiDat50KG widgets_;
    std::unique_ptr<C*[]> managerTable154_; // No slot is read before its known bit.
    std::uintptr_t source154Identity_=0;std::int32_t count158_=0;
    std::array<bool,22> slotsKnown_{};
    struct LineRecord {
        std::unique_ptr<TextOverlay> owner;
        std::uintptr_t sourceIdentity=0;
        std::int32_t capacity=0;
        std::size_t inputOffset=0;
        bool slotKnown=false,constructed=false,loaded=false,attached=false;
        bool copied=false,configured=false,retired=false;
        std::uint32_t runtimeColor=0;bool runtimeColorKnown=false;
        bool setupTouched=false,setupEnabled=true;
    };
    struct RowRecord {
        std::unique_ptr<OverlayText> outer;
        std::unique_ptr<Movement> movement;
        std::shared_ptr<Watto> watto;C* wattoEntity=nullptr;
        std::unique_ptr<C*[]> array60;
        std::array<LineRecord,4> lines{};
        std::shared_ptr<const Dictionary::SelectedText> borrow;
        std::uintptr_t sourceOuterIdentity=0,source60Identity=0;
        std::size_t arrayCapacity=0,activeLine=0;
        std::uint32_t entryColor=0;std::size_t entryColorLines=0;
        bool entryColorStored=false,entryColorReturned=false;
        bool drawPositionKnown=false;std::int32_t drawX=0,drawY=0;
        std::uint32_t selected38=0;std::uint32_t runtimeDisabled4C=0; // Last actual entry callback store, initially constructor zero.
        bool activationShown=false,activationReturned=false;
        Stage stage=Stage::empty;
        bool reached=false,constructed=false,retiring=false,entityRetired=false;
        bool countReset=false,arrayPublished=false,arrayReleased=false;
        bool wantsKnown=false,hidden=false,stored=false,published=false,attached=false,empty=false;
        bool fontKnown=false,materialKnown=false,xKnown=false,yKnown=false,leadingKnown=false;
        bool justificationKnown=false,typeKnown=false,dynamicLengthKnown=false,dynamicCountKnown=false;
        bool lookupReached=false,hashAssertionReached=false,idAssertionReached=false;
        std::int32_t lookupOrdinal=0;std::uint32_t pendingPC=0;
        const C* previousAtRetire=nullptr;
    };
    //Fixed native bookkeeping only. Unreached source slots remain unknown.
    std::array<RowRecord,22> rows_{};
    std::size_t cursor_=0;
    // The genuine outer141ED0 cursor retains each reached line. A failure
    // cannot replay an earlier line/renderer service on the same publication.
    const C* updateOuter_=nullptr;const C* updateChild_=nullptr;
    std::uint64_t updateSerial_=0;
    bool updateReturned_=false,updateFailed_=false;
    std::uint32_t updateFailurePC_=0;
    bool drawTextChild29D3F0(const C&,OriginalUiGarageDrawContribution50KG&,D&)noexcept;
    bool updateTextOuter141ED0(const C&,
        const ::original_ui_source_timing50LH::PublicationView&,std::uint64_t,
        OriginalUiGarageRendererLease50KG&,D&)noexcept;
    const Page* runtimePage_=nullptr; // Identity only; never dereferenced during retirement.
    struct ActivationCall {
        std::array<std::size_t,4> key{};std::size_t row=0;
        bool started=false,showReturned=false,returned=false;
    } activationCall_;
    struct EntryCall {
        std::array<std::size_t,6> key{};
        std::size_t row=0,colorLines=0,setupLines=0;
        std::uint32_t operation=0;std::int32_t argument=0;
        bool started=false,selectionStored=false,visibilityReturned=false,returned=false;
    } entryCall_;
    enum class RetirementRoute:std::uint8_t {none,coldAbort,normalManager};
    RetirementRoute retirementRoute_=RetirementRoute::none;
    bool retirementStarted_=false,coldDatReleased_=false;
    bool bound_=false,countKnown_=false,tableKnown_=false;
    bool detachedReleased_=false,tableReleased_=false;
    //Exact historical row1 fixture aliases, never duplicated source state.
    std::unique_ptr<OverlayText>& pending_=rows_[0].outer;
    std::unique_ptr<C*[]>& lineArray60_=rows_[0].array60;
    std::unique_ptr<TextOverlay>& line_=rows_[0].lines[0].owner;
    std::shared_ptr<const Dictionary::SelectedText>& selectedBorrow_=rows_[0].borrow;
    std::uintptr_t& source60Identity_=rows_[0].source60Identity;
    std::uintptr_t& sourceLineIdentity_=rows_[0].lines[0].sourceIdentity;
    bool& lineCountReset_=rows_[0].countReset;
    bool& lineArrayPublished_=rows_[0].arrayPublished;
    bool& lineSlotKnown_=rows_[0].lines[0].slotKnown;
    bool& lineConstructed_=rows_[0].lines[0].constructed;
    bool& lineLoaded_=rows_[0].lines[0].loaded;
    bool& lineAttached_=rows_[0].lines[0].attached;
    bool& textCopied_=rows_[0].lines[0].copied;
    bool& lineConfigured_=rows_[0].lines[0].configured;
    bool& wantsInputKnown_=rows_[0].wantsKnown;
    bool& hidden_=rows_[0].hidden;
    bool& rowStored_=rows_[0].stored;
    bool& rowPublished_=rows_[0].published;
    bool& outerAttached_=rows_[0].attached;
    bool& lineRetired_=rows_[0].lines[0].retired;
    bool& lineArrayReleased_=rows_[0].arrayReleased;
    const C*& outerPreviousAtRetire_=rows_[0].previousAtRetire;
    Stage& stage_=rows_[0].stage;
    bool& constructed_=rows_[0].constructed;
    bool& retiring_=rows_[0].retiring;
    bool& entityRetired_=rows_[0].entityRetired;
    bool& fontKnown_=rows_[0].fontKnown;
    bool& materialKnown_=rows_[0].materialKnown;
    bool& xKnown_=rows_[0].xKnown;
    bool& yKnown_=rows_[0].yKnown;
    bool& leadingKnown_=rows_[0].leadingKnown;
    bool& justificationKnown_=rows_[0].justificationKnown;
    bool& typeKnown_=rows_[0].typeKnown;
    bool& lookupReached_=rows_[0].lookupReached;
    bool& hashAssertionReached_=rows_[0].hashAssertionReached;
    bool& idAssertionReached_=rows_[0].idAssertionReached;
    std::int32_t& lookupOrdinal_=rows_[0].lookupOrdinal;
    std::uint32_t& pendingPC_=rows_[0].pendingPC;
    static bool fail(D& d,S status,std::uint32_t pc)noexcept{d.status=status;d.sourcePC=pc;return false;}
    static std::atomic<std::uint8_t>& hashGuard8098()noexcept;
    static std::atomic<std::int32_t>& hashAssertion3D6B10()noexcept;
    static std::atomic<std::uint8_t>& idGuard809C()noexcept;
    static std::atomic<std::int32_t>& idAssertion3D6B18()noexcept;
    static bool authorityThread(const std::shared_ptr<Authority>&)noexcept;
    bool threadCurrent()const noexcept;
    bool identityCurrent()const noexcept;
    bool fieldsCurrent()const noexcept;
    bool rowFieldsCurrent(std::size_t)const noexcept;
    bool rowColdCurrent(std::size_t)const noexcept;
    bool rowCompleteCurrent(std::size_t,bool authoredOnly)const noexcept;
    // Private same-call row suffix after identityCurrent + fieldsCurrent.
    // No source callback, mutation, or return may separate that proof/use.
    bool rowCompleteAfterFieldsCurrent(std::size_t,bool authoredOnly)const noexcept;
    bool rowPendingCurrent(std::size_t)const noexcept;
    bool lineInputsCurrent(const TextOverlay*,const C&,std::string_view font,
        std::string_view material,std::int32_t capacity,bool& dynamic)const noexcept;
    // Only immediate Text/row validation after this same Widget's full field
    // proof; the ordinary lineInputsCurrent entry retains that full proof.
    bool lineInputsAfterFieldsCurrent(const TextOverlay*,const C&,std::string_view font,
        std::string_view material,std::int32_t capacity,bool& dynamic)const noexcept;
    bool prepareCurrentRow1CD860(std::shared_ptr<const assets::OriginalUiArchive50KG>,D&)noexcept;
    bool loadRow2A9000(std::size_t,D&)noexcept;
    bool continueStatic2A9540(std::size_t,std::shared_ptr<const Dictionary::SelectedText>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,D&)noexcept;
    bool constructDynamic2A955C(std::size_t,std::shared_ptr<const assets::OriginalUiArchive50KG>,D&)noexcept;
    bool finishRow1CD8B4(std::size_t,D&)noexcept;
    bool advanceRow1CDA1C(D&)noexcept;
    bool continueWattoRow1CD8B4(std::shared_ptr<Watto>,D&)noexcept;
    bool finishCold1CDA34(D&)noexcept;
    bool ownsWattoRow(const Watto* owner)const noexcept;
    bool retireRow2A8F70(std::size_t,D&)noexcept;
    bool selectedInputCurrent(std::size_t,std::string_view&)const noexcept;
    bool configurePosition(std::size_t,std::size_t,D&)noexcept;
    bool prepare1CD7A0(const RootInputs&,std::shared_ptr<Grant>,std::shared_ptr<Fog>,D&)noexcept;
    bool loadPending2A9000(D&)noexcept;
    bool continueCold2A9540(std::shared_ptr<const Dictionary::SelectedText>,
        std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,D&)noexcept;
    bool selectedInputCurrent(std::string_view&)const noexcept;
    bool retireDetached2A8F70(D&)noexcept;
    bool retireManagerRows2A8F70(D&)noexcept;
    bool retireRows2A8F70(RetirementRoute,D&)noexcept;
    bool preflightRows2A8F70(RetirementRoute,const C* actualLaterTail,D&)const noexcept;
    bool preflightFailedSharedPageRows(const OriginalUiGaragePageOwner50KG&,D&)const noexcept;

    bool retireManagerTable2A3B18(D&)noexcept;
};
}
