#pragma once
#include "original_ui_menu_warning_packet50kg.h"
#include "original_ui_menu_raster_inputs50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_controller_popup_resources50kg.h"
#include "original_ui_warning_geometry50kg.h"
#include "../../../src/game/original_ui_controller_popup50kg.h"
namespace rr::platform {
struct OriginalUiMainApplication50KG;
class OriginalUiSharedDialogMaterialHistory50KG;
class OriginalUiMainDrawResources50KG;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiMainDraw50KG;
class OriginalUiOptionsDrawContribution50KG;
class OriginalUiMainSession50KG;
class OriginalUiOptionsSession50KG;
// Native retained UI470/47C owners, not a second dispatcher or menu manager.
// Initial construction is a Root boot operation. A preexisting UI without this
// owner cannot be adopted by inventing prior470 activity or resetting47C.
class OriginalUiSharedDialogs50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class game::OriginalUiEntityScope50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiControllerPopupResources50KG;
    friend class OriginalUiSharedDialogMaterialHistory50KG;
    friend class OriginalUiMainSession50KG;
    friend class OriginalUiMainDraw50KG;
    friend class OriginalUiOptionsDrawContribution50KG;
    friend class OriginalUiOptionsSession50KG;
    friend class OriginalUiFrontend50KG;
    friend class VehicleWindowGl50DS;
    friend class OriginalUiGarageQuad50KG;
    friend class OriginalUiGarageScreenText50KG;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
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
        std::array<Quad,3> strips;
        std::array<Quad,2> icons;
        std::unique_ptr<Dim> dim220;
        std::unique_ptr<Text> message270,confirm2E0,cancel350;
        bool canonicalLoaded=false;
        bool loaded=false;
    } controller47C_;
    bool loadCanonical47C2055B0(Diagnostic&)noexcept;
    static bool canonical47CQuadPayloadRetired(const Quad&)noexcept;
    bool canonical47COwnersCurrent(std::array<const assets::OriginalStandardMaterialObject50LA*,3>&)const noexcept;
    bool canRenderGaragePurchase47C(const OriginalUiGaragePageOwner50KG&,Diagnostic&)const noexcept;
    bool canStartGaragePurchase2060A0(const OriginalUiGaragePageOwner50KG&,Diagnostic&)const noexcept;
    bool writeController47CLabels2060A0(const game::OriginalUiControllerPopupState50KG&,Diagnostic&)noexcept;
    bool startGaragePurchase2060A0(const OriginalUiGaragePageOwner50KG&,Diagnostic&)noexcept;
    bool garagePopupInputFailed_=false;
    std::shared_ptr<Scope> scope_;
    std::weak_ptr<OriginalUiMainApplication50KG> application_;
    // Root boot transfers the exact initial five47C histories into the six-role
    // owner. It survives47C payload and retires after actual470 final1413E0.
    std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG> sharedMaterialHistory_;
    bool sharedHistoryFinalRetirementCurrent()const noexcept;
    // Exact Scope completion only, independent of current Display authority.
    // A retry of pending history cleanup must never replay completed1413E0.
    bool sharedScopeFinalRetirementComplete()const noexcept;
    bool bootstrapInitialHistoryFromMain(const OriginalUiMainDrawResources50KG&,Diagnostic&)noexcept;
    bool sharedHistoryCanonicalOwnersCurrent(std::array<const assets::OriginalStandardMaterialObject50LA*,6>&)const noexcept;
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
    bool advance470Body2048F0(float,Diagnostic&)noexcept;
    // A genuine dispatcher470 callback, not a Garage page receiver. The Draw
    // owns the invocation lease; update count is never callback authority.
    enum class FirstPass470Phase:std::uint8_t {idle,body,inheritedPending};
    FirstPass470Phase firstPass470Phase_=FirstPass470Phase::idle;
    OriginalUiGarageDrawContribution50KG* firstPass470Draw_=nullptr;
    std::uint64_t firstPass470Epoch_=0,firstPass470Completed_=0,firstPass470Aborted_=0;
    bool firstPass470Matches(const OriginalUiGarageDrawContribution50KG&)const noexcept;
    bool begin470FirstPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool continue470FirstPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool complete470FirstPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool abort470AfterNativeRelease(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    void release470FirstPass(OriginalUiGarageDrawContribution50KG&)noexcept;
    // Own470 +48 invocation. Numeric transcript is not receiver authority:
    // each actual mutation/call rechecks this live callback and exact payload.
    // Actual47C callback custody. FIFO pins outlive the inherited tail.
    enum class Popup47CPhase:std::uint8_t {idle,first,body,inherited};
    Popup47CPhase popup47CPhase_=Popup47CPhase::idle;
    OriginalUiGarageDrawContribution50KG* popup47CDraw_=nullptr;
    std::uint64_t popup47CEpoch_=0;
    std::uint32_t nativePopup47CFifoPins_=0,popup47COperation_=0;
    bool begin47CCallback(OriginalUiGarageDrawContribution50KG&,std::uint32_t,Diagnostic&)noexcept;
    bool demand47C206430(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool draw47C2064C0(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool complete47CCallback(OriginalUiGarageDrawContribution50KG&,std::uint32_t,Diagnostic&)noexcept;
    bool abort47CAfterNativeRelease(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    void release47CQueueFenceIfQuiescent()noexcept;
    bool popup47CTextCurrent(const OriginalUiGarageDrawContribution50KG&,const Text&)const noexcept;
    bool popup47CQuadCurrent(const OriginalUiGarageDrawContribution50KG&,const Quad&)const noexcept;
    bool popup47CDimCurrent(const OriginalUiGarageDrawContribution50KG&)const noexcept;
    bool popup47CDimPacketCurrent2E21D0()const noexcept;
    bool drawPopup47CDim2E21D0(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool regeneratePopup47CDimPacket2E21D0(std::uint32_t,std::uint32_t,std::int32_t,Diagnostic&)noexcept;
    bool lowerPopup47CDim327E90(std::uint64_t,const std::array<std::uint32_t,2>&,Quad::LoweredPacket327E90&,Diagnostic&)const noexcept;
    bool lowerPopup47CDimPacketBody327E90(std::uint64_t,const std::array<std::uint32_t,2>&,Quad::LoweredPacket327E90&,Diagnostic&)const noexcept;
    enum class ThirdPass470Phase:std::uint8_t {idle,body,inheritedPending};
    ThirdPass470Phase thirdPass470Phase_=ThirdPass470Phase::idle;
    OriginalUiGarageDrawContribution50KG* thirdPass470Draw_=nullptr;
    std::uint64_t thirdPass470Epoch_=0,thirdPass470Completed_=0,thirdPass470Aborted_=0;
    std::uint64_t thirdPass470Update_=0;
    std::array<std::uint32_t,4> thirdPass470Cache_{};
    OriginalUiWarningGeometryPlan50KG thirdPass470Plan_;
    std::size_t thirdPass470Next_=0;
    bool thirdPass470Effects_=false;
    std::uint32_t nativeWarning470FifoPins_=0;
    bool thirdPass470Matches(const OriginalUiGarageDrawContribution50KG&)const noexcept;
    bool begin470ThirdPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool continue470ThirdPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool complete470ThirdPass(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool abort470ThirdPassAfterNativeRelease(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    void release470ThirdPass(OriginalUiGarageDrawContribution50KG&)noexcept;
    void release470QueueFenceIfQuiescent()noexcept;
    bool warning470TextCurrent(const OriginalUiGarageDrawContribution50KG&,const Text&)const noexcept;
    bool warning470QuadCurrent(const OriginalUiGarageDrawContribution50KG&,const Quad&)const noexcept;
    bool warning470DimCurrent(const OriginalUiGarageDrawContribution50KG&)const noexcept;
    bool warning470DimPacketCurrent2E21D0()const noexcept;
    bool drawWarning470Dim2E21D0(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool lowerWarning470Dim327E90(std::uint64_t,const std::array<std::uint32_t,2>&,
        Quad::LoweredPacket327E90&,Diagnostic&)const noexcept;
    // Material body only. The inherited tail and positive draw are separate.
    bool demand470204A00(Diagnostic&)noexcept;
    bool demand470Body204A00(Diagnostic&)noexcept;
    std::uint64_t demand470Update_=0;
    std::uint8_t demand470Next_=0;
    std::array<std::uint32_t,4> demand470Cache_{};
    bool demand470Pending_=false,demand470Effects_=false;
    bool invisible470204AC0(Diagnostic&)const noexcept;
    // Private readiness only. These neither poll devices nor issue callbacks,
    // raster/frame receipts, source completion or renderer retirement.
    bool controls2ObservedPresent(std::uint64_t& pollRevision)const noexcept;
    bool controls2MenuResourcesCurrent(const OriginalUiOptionsSession50KG&,
        const OriginalUiOptionsDrawContribution50KG&)const noexcept;
    bool admitControls2Menu470(const OriginalUiOptionsSession50KG&,
        const OriginalUiOptionsDrawContribution50KG&,Diagnostic&)const noexcept;
    std::uint64_t update470Count_=0,update470PollRevision_=0;
    bool update470Pending_=false,update470SampledPrimary_=false;
    // Bounded native menu callback receipt, NOT a shared first-pass or full
    // Root traversal. It covers470+40/tail,470+48/tail and ONE existing47C
    // semantic projection at its actual sibling point. Active47C+40 demand
    // remains outside this receipt and is never called inherited-only.
    // Native menu raster provenance, never a guest114A00/CameraFrame receipt.
    OriginalUiMenuRasterInputs50KG menuRaster_;
    bool menuRasterAttempted_=false,menuRasterFailed_=false;
    std::uint64_t menuRasterFrame_=0;
    const OriginalUiNativeDraw50KG* menuRasterNative_=nullptr;
    bool readMenu470RasterInputs(OriginalUiMenuRasterInputs50KG&)const noexcept;
    bool applyMenu470RasterPrelude(Diagnostic&)noexcept;
    bool menu470RasterCurrent()const noexcept;
    bool sealMenu470RasterFrame(std::uint64_t,Diagnostic&)noexcept;
    bool releaseMenu470RasterAfterAcceptance()noexcept;
    bool releaseMenu470RasterAfterRetirement(const VehicleWindowGl50DS&,Diagnostic&)noexcept;

// Add #include "original_ui_menu_warning_packet50kg.h" to Shared's header.
// Private Main/Options operation custody. No Garage callback/FIFO authority.
enum class MenuWarningPhase:std::uint8_t {idle,body,inheritedPending,complete,failed};
MenuWarningPhase menuWarningPhase_=MenuWarningPhase::idle;
OriginalUiWarningGeometryPlan50KG menuWarningPlan_;
std::size_t menuWarningNext_=0,menuWarningIssued_=0,menuWarningAppended_=0;
std::uint64_t menuWarningEpoch_=0,menuWarningInvocation_=0,menuWarningUpdate_=0;
std::array<std::uint32_t,4> menuWarningCache_{};
std::uint32_t menuWarningSourcePC_=0,nativeMenuWarning470Pins_=0;
bool menuWarningEffects_=false;
std::array<OriginalUiMenuWarningPacket50KG,8> menuWarningPackets_;
bool menuWarningIdentityCurrent()const noexcept;
bool menuWarningBodyCurrent()const noexcept;
bool menuWarningCompletedCurrent()const noexcept;
bool menuWarningOperationCurrent()const noexcept;
bool executeMenu470WarningBody(Diagnostic&)noexcept;
bool completeMenu470WarningTail(Diagnostic&)noexcept;
bool menuWarningPacketsCurrent()const noexcept;
bool menuWarningPacketsReleasable()const noexcept;
void releaseMenuWarningPackets()noexcept;
void resetMenuWarningBody()noexcept;
bool menuWarningPacketIdentity(const OriginalUiMenuWarningPacket50KG&,std::size_t)const noexcept;
bool menuWarningPacketPayloadMatches(const OriginalUiMenuWarningPacket50KG&)const noexcept;
bool prepareMenuWarningPacket(OriginalUiMenuWarningPacket50KG&,OriginalUiMenuWarningPacket50KG::Kind)noexcept;
bool appendMenuWarningPacket(OriginalUiMenuWarningPacket50KG&,const OriginalUiMenuWarningGeometry50KG&,Diagnostic&)noexcept;
bool menuWarning470PacketAppendCurrent(const OriginalUiMainDraw50KG&,const OriginalUiMenuWarningPacket50KG&)const noexcept;
bool menuWarning470PacketAppendCurrent(const OriginalUiOptionsDrawContribution50KG&,const OriginalUiMenuWarningPacket50KG&)const noexcept;
// Shared private declarations required by the retained leaf implementation.
// Parent owns actual operation/input/collector/current implementations.
struct MenuWarningTextInputs {
    std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;
    float videoDepthMaximum60=0;
};
struct MenuWarningQuadInputs {
    bool sourceQuadEnabled383678=false;
    std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;
};
struct MenuWarningDimInputs {
    bool sourceApertureEnabled383680=false;
    std::uint32_t viewportWidth1E8=0,viewportHeight1EC=0;
    std::int32_t videoDepthMaximum60=0;
};
bool menuWarning470TextCurrent(const Text&)const noexcept;
bool menuWarning470QuadCurrent(const Quad&)const noexcept;
bool menuWarning470DimCurrent()const noexcept;
bool readMenuWarningTextInputs2E0170(const Text&,MenuWarningTextInputs&,std::uint32_t&)const noexcept;
bool readMenuWarningQuadInputs2DF550(const Quad&,MenuWarningQuadInputs&,std::uint32_t&)const noexcept;
bool readMenuWarningDimInputs2E21D0(MenuWarningDimInputs&,std::uint32_t&)const noexcept;
bool menuWarning470PacketCaptureCurrent(const OriginalUiMenuWarningPacket50KG&)const noexcept;
bool menuWarning470PacketCurrent(const OriginalUiMenuWarningPacket50KG&)const noexcept;
bool menuWarning470PacketOffsetCurrent(const OriginalUiMenuWarningPacket50KG&,const std::array<std::uint32_t,2>&)const noexcept;
bool collectMenuWarning470Text(Text&,std::size_t,Text::Diagnostic&)noexcept;
bool collectMenuWarning470Quad(Quad&,Quad::Diagnostic&)noexcept;
bool collectMenuWarning470Dim(Diagnostic&)noexcept;
// Implemented by this leaf proposal in original_ui_shared_warning_dim50kg.cpp:
bool drawMenuWarning470Dim2E21D0(Diagnostic&)noexcept;
bool regenerateWarning470DimPacket2E21D0(std::uint32_t,std::uint32_t,std::int32_t,Diagnostic&)noexcept;
bool retainMenuWarningDimPacket(OriginalUiMenuWarningPacket50KG&,Diagnostic&)noexcept;
bool releaseMenuWarningDimPacket(OriginalUiMenuWarningPacket50KG&)noexcept;
bool menuWarningDimPacketMatches(const OriginalUiMenuWarningPacket50KG&)const noexcept;
bool lowerMenuWarningDimPacket(const OriginalUiMenuWarningPacket50KG&,const std::array<std::uint32_t,2>&,
    OriginalUiMenuWarningGeometry50KG&,Diagnostic&)const noexcept;
bool lowerWarning470DimPacketBody327E90(std::uint64_t,const std::array<std::uint32_t,2>&,
    Quad::LoweredPacket327E90&,Diagnostic&)const noexcept;

bool menuWarning470PacketImageCurrent(const OriginalUiMenuWarningPacket50KG&,const assets::OriginalUiMemberView50KG&)const noexcept;
bool admitMenuWarningCollection(std::int32_t,bool&,Diagnostic&)noexcept;

    enum class Menu470Phase:std::uint8_t {idle,callbacks,collectorPending,popupBody,popupProjection,cpuPending,framePrepared,submitting,failed};
    Menu470Phase menu470Phase_=Menu470Phase::idle;
    OriginalUiMainSession50KG* menuMainSession_=nullptr;
    OriginalUiOptionsSession50KG* menuOptionsSession_=nullptr;
    OriginalUiMainDraw50KG* menuMainDraw_=nullptr;
    OriginalUiOptionsDrawContribution50KG* menuOptionsDraw_=nullptr;
    const OriginalUiSourceOwner50KM* menuUpdateFailedOwner_=nullptr;
    bool menuUpdateAborted_=false; // Terminal failed update cannot be resumed by a new Session.
    OriginalUiMainDraw50KG* menuUpdateFailedMain_=nullptr;
    OriginalUiOptionsDrawContribution50KG* menuUpdateFailedOptions_=nullptr;
    std::uint64_t menu470Epoch_=0,menu470Invocation_=0,menu470Frame_=0,menu470Update_=0,menuUpdateEpoch_=0;
    std::uint8_t menu470CompletedSlots_=0;
    std::uint32_t menu470Slot_=0;
    const C* menu470Callback_=nullptr;
    const game::OriginalUiControllerPopupRender50KG* menuPopupPlan_=nullptr;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> menuDisplay_;
    bool menuDisplayPinned_=false;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> menuUpdateFailedDisplay_;
    bool menuUpdateFailedDisplayPinned_=false;
    bool menuTopologyCurrent()const noexcept;
    bool menu470Current()const noexcept;
    bool menu470RenderCurrent()const noexcept;
    const OriginalUiSourceOwner50KM* menu470Owner()const noexcept;
    bool menu470PendingFor(const OriginalUiSourceOwner50KM&)const noexcept;
    bool beginMenu470(OriginalUiMainSession50KG&,OriginalUiMainDraw50KG&,std::uint64_t,Diagnostic&)noexcept;
    bool beginMenu470(OriginalUiOptionsSession50KG&,OriginalUiOptionsDrawContribution50KG&,std::uint64_t,Diagnostic&)noexcept;
    bool beginMenu470Body(Diagnostic&)noexcept;
    bool continueMenu470AfterCollectorBegin(const OriginalUiSourceOwner50KM&,Diagnostic&)noexcept;
    bool advanceMenu470(OriginalUiMainSession50KG&,OriginalUiMainDraw50KG&,float,Diagnostic&)noexcept;
    bool advanceMenu470(OriginalUiOptionsSession50KG&,OriginalUiOptionsDrawContribution50KG&,float,Diagnostic&)noexcept;
    bool advanceMenu470Body(const OriginalUiSourceOwner50KM&,float,Diagnostic&)noexcept;
    bool walkMenu470Inherited(std::uint32_t,std::uint32_t,Diagnostic&)noexcept;
    bool serviceMenu141F3C(Diagnostic&)noexcept;
    bool renderMenuPopupSibling(game::OriginalUiControllerPopupRender50KG&,Diagnostic&)noexcept;
    bool menuPopupProjectionCurrent(const OriginalUiSourceOwner50KM&,const game::OriginalUiControllerPopupRender50KG&)const noexcept;
    bool completeMenuPopupProjection(const OriginalUiSourceOwner50KM&,const game::OriginalUiControllerPopupRender50KG&,Diagnostic&)noexcept;
    bool sealMenu470Frame(const OriginalUiSourceOwner50KM&,std::uint64_t,Diagnostic&)noexcept;
    bool menu470FrameCurrent(const OriginalUiSourceOwner50KM&,std::uint64_t)const noexcept;
    bool beginMenu470Submission(const OriginalUiSourceOwner50KM&,std::uint64_t,const VehicleWindowGl50DS&)noexcept;
    bool finishMenu470Submission(const OriginalUiSourceOwner50KM&,std::uint64_t,bool,bool)noexcept;
    void failMenu470Submission(const OriginalUiSourceOwner50KM&)noexcept;
    bool menu470CloseRendererCurrent(const VehicleWindowGl50DS&)const noexcept;
    bool menu470CloseOwnerCurrent(const OriginalUiMainSession50KG&,const VehicleWindowGl50DS&)const noexcept;
    bool menu470CloseOwnerCurrent(const OriginalUiOptionsSession50KG&,const VehicleWindowGl50DS&)const noexcept;
    bool retireMenu470AfterRenderer(const OriginalUiSourceOwner50KM&,const VehicleWindowGl50DS&,Diagnostic&)noexcept;
    void releaseMenu470()noexcept;
    bool loadedCurrent()const noexcept;
    bool fail(Diagnostic&,Status,std::uint32_t)noexcept;
};
}
