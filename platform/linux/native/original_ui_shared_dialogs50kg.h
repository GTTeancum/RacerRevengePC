#pragma once
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_controller_popup_resources50kg.h"
#include "../../../src/game/original_ui_controller_popup50kg.h"
namespace rr::platform {
struct OriginalUiMainApplication50KG;
// Native retained UI470/47C owners, not a second dispatcher or menu manager.
// Initial construction is a Root boot operation. A preexisting UI without this
// owner cannot be adopted by inventing prior470 activity or resetting47C.
class OriginalUiSharedDialogs50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class game::OriginalUiEntityScope50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiControllerPopupResources50KG;
    friend class OriginalUiMainSession50KG;
    friend class OriginalUiOptionsSession50KG;
    friend class OriginalUiFrontend50KG;
    using Root=::original_main_entry50KG::Application;
    using Scope=game::OriginalUiEntityScope50KG;
    using C=game::OriginalWreckComponent50LA;
    using Quad=OriginalUiGarageQuad50KG;
    using Text=OriginalUiGarageScreenText50KG;
    using Dim=OriginalUiGaragePageOwner50KG::Aperture;
    using Fonts=assets::OriginalApplicationFonts50LA;
    using Selected=assets::OriginalApplicationLocalization50KG::SelectedText;
public:
    enum class Status {complete,owner,phase,history,allocation,source,resourcePending,retirementPending};
    struct Diagnostic {Status status=Status::complete;std::uint32_t sourcePC=0;bool sourceEffectsCommitted=false;
        Scope::Result entity{};Quad::Diagnostic quad{};OriginalUiGarageScreenTextDiagnostic50KG text{};
        OriginalUiControllerPopupResourceDiagnostic50KG popup{};VehicleWindowDiagnostic50DS renderer{};};
    ~OriginalUiSharedDialogs50KG()noexcept;
    OriginalUiSharedDialogs50KG(const OriginalUiSharedDialogs50KG&)=delete;
    OriginalUiSharedDialogs50KG& operator=(const OriginalUiSharedDialogs50KG&)=delete;
private:
    OriginalUiSharedDialogs50KG()noexcept=default;
    struct Inputs final {
        friend struct ::original_main_entry50KG::Application;
        friend class OriginalUiSharedDialogs50KG;
        const Root* root=nullptr;
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain;
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context;
        std::shared_ptr<::original_main_entry50KG::Video> video;
        std::shared_ptr<const assets::OriginalUiArchive50KG> popup;
        std::shared_ptr<Fonts> fonts;
        std::shared_ptr<const Selected> selected32C;
        std::string* materialDirectory=nullptr;const std::string* fontDirectory=nullptr;
        Inputs()noexcept=default;
    } inputs_;
    enum class Phase {empty,bound,dialog470Constructed,dialog470DimLoaded,dialog470TextsLoaded,
        dialog470Loaded,controller47CConstructed,loaded,retiring,retired,failed};
    struct Dialog470 final {
        C entity;std::uint32_t vtable=0;std::array<Quad,3> strips;
        std::unique_ptr<Dim> dim160;
        std::unique_ptr<Text> message1B0,alternate220;
        const C* manager30=nullptr;
        float delta290=0,timer294=.25f;
        std::uint32_t active298=0,primaryPresent29C=0,secondaryPresent2A0=0;
        bool constructorStateOwned=false,loaded=false;
    } dialog470_;
    struct Controller47C final {
        C entity;std::uint32_t vtable=0;
        // Resource payload is the genuine retained native47C owner, not copied
        // bank indices or replacement material history. State stays in app.
        std::shared_ptr<OriginalUiControllerPopupResources50KG> resources;
        bool loaded=false;
    } controller47C_;
    std::shared_ptr<Scope> scope_;
    std::weak_ptr<OriginalUiMainApplication50KG> application_;
    Phase phase_=Phase::empty;
    bool successorConstructor_=false; // Only Application issues after exact retired-predecessor proof.
    std::size_t constructed470Quads_=0,loaded470Texts_=0,loaded470Quads_=0;
    std::uint8_t reorderStep_=0;
    std::uint64_t completedReorders_=0;
    bool bindInitial1B7BD0(std::shared_ptr<Scope>,std::shared_ptr<OriginalUiMainApplication50KG>,Inputs,Diagnostic&)noexcept;
    bool construct470203C90(Diagnostic&)noexcept;
    bool load470203D40(Diagnostic&)noexcept;
    bool construct47C2054D0(Diagnostic&)noexcept;
    bool bindLoaded47C2055B0(std::shared_ptr<OriginalUiControllerPopupResources50KG>,Diagnostic&)noexcept;
    const OriginalUiGaragePageOwner50KG* reorderedPage_=nullptr;
    const OriginalUiGaragePageOwner50KG* reorderingPage_=nullptr;
    std::weak_ptr<Scope::GarageConstructionGrant> reorderedGrant_;
    bool reappendInitialMain1D7CF4(Diagnostic&)noexcept;
    bool initialMainAttached_=false,initialMainResetPending_=false;
    bool reappendMainEntry1D7CF4(Diagnostic&)noexcept;
    bool reappendOptionsEntry1B86D0(Diagnostic&)noexcept;
    bool reappendNativeMenu1B86D0(Diagnostic&)noexcept;
    bool reappend1B86D0(const OriginalUiGaragePageOwner50KG&,Diagnostic&)noexcept;
    bool reorderCompleteFor1B872C(const OriginalUiGaragePageOwner50KG&)const noexcept;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> retirementDisplay_;
    bool retirementDisplayPinned_=false;
    std::uint8_t retiringOrdinal_=1,retirementPayloadStep_=0,retirementDrainPhase_=0;
    bool retireAttached142590(Diagnostic&)noexcept;
    bool controllerRetirementCurrent()const noexcept;
    bool nativeMaterialRetirementCurrent()const noexcept;
    bool drain2DEFE0(Diagnostic&,std::uint32_t,std::uint32_t)noexcept;
    bool retireDim2E1980(Diagnostic&)noexcept;
    bool current()const noexcept;
    // Actual UI470 update consumes only the retained successful GameHost poll.
    // Native keyboard transport supplies the first logical menu route. The
    // SDL second route remains unknown when enumeration/open was ambiguous.
    bool advance4702048F0(float,Diagnostic&)noexcept;
    bool invisible470204AC0(Diagnostic&)const noexcept;
    std::uint64_t update470Count_=0,update470PollRevision_=0;
    bool update470Pending_=false,update470SampledPrimary_=false;
    bool loadedCurrent()const noexcept;
    bool fail(Diagnostic&,Status,std::uint32_t)noexcept;
};
}
