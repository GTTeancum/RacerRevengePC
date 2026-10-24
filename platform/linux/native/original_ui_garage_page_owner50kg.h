#pragma once
// Reached Garage1F9FF0 owner. Its ONE counted Entity is not PodScene4C0,
// the legacy panel projection, or a Page-ready credential.
#include "original_ui_garage_screen_text50kg.h"
#include "original_ui_garage_quad50kg.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_manager_search50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "../../../src/assets/original_ui_garage_page50kg.h"
#include "../../../src/game/original_frontend_camera_cold50la.h"
#include "../../../src/game/original_ui_garage_state50kg.h"
#include "../../../src/game/original_ui_garage_watto_lifecycle50kg.h"
#include <optional>
#include <cstddef>
#include <utility>

namespace rr::platform {
class OriginalUiGarageSceneLighting50KG;
class OriginalUiGaragePodScene50KG;
class OriginalUiGarageSession50KG;
class OriginalUiGarageRendererLease50KG;
class OriginalUiMainAudio50KG;
class OriginalUiGarageDroidAudio50GN;
class OriginalAudioPendingWaves50KG;
class OriginalUiGarageSharedPageDrawOwner50KG;
class OriginalUiSharedDialogs50KG;
class OriginalUiRiderProfileOwner50KG;
class OriginalUiGaragePageOwner50KG final {
    friend class OriginalUiGarageDrawContribution50KG;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class OriginalUiGarageCameraFrame50KG;
    friend class OriginalUiSharedDialogs50KG;
    friend class OriginalUiGarageWidget1Owner50KG;
    friend class OriginalUiGarageWattoOwner50KG;
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class game::OriginalUiEntityScope50KG;
    friend class OriginalUiGaragePodScene50KG;
    friend class OriginalUiGarageActors50GN;
    friend class OriginalUiGarageSceneLighting50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiMainAudio50KG;
    using C=game::OriginalWreckComponent50LA;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using Authority=game::OriginalApplicationEntities50LA;
    using Text=OriginalUiGarageScreenText50KG;
    using Widget=OriginalUiGarageWidget1Owner50KG;
    using Fog=OriginalUiGarageManagerFog50KG;
    using PageData=assets::OriginalUiGaragePageData50KG;
public:
    enum class Status:std::uint8_t {complete,input,phase,allocation,scope,
        sharedLoaderPending,scenePending,sceneOrder,childRetirementPending,rendererRetirementPending,
        loaderApplicationPending,loaderSelectedPending,loaderResourcesPending,
        activationShowPending,activationWidgetsPending,activationMoviePending,activationDisplayPending,
        entryProfilePending,entryAppearancePending,entryNumbersPending,entryWattoPending,entryControlsPending,
        entryRandomPending,entryCameraPending,entryPopupPending,entryBackgroundPending,runtimePending};
    enum class Stage:std::uint8_t {empty,bound,entity,baseConstructed,sharedConstructed,
        constructed,sharedManagerBound,sharedDatLoaded,sharedApertureLoading,sharedAperturesLoaded,sharedCameraConstructed,sharedCameraViewportPending,sharedCameraCallbackPending,sharedCameraObserverPending,sharedCameraPoseSet,sharedPromptsLoading,sharedPromptsLoaded,sharedTextLoading,sharedTextLoaded,sharedBackgroundLoaded,sharedRouterLoading,sharedCollectionsLoading,sharedControlsLoading,sharedGroupsLoading,sharedLoaderReturned,sceneConstructing,sceneConstructed,wattoBorrowed,costTextLoading,costTextLoaded,backgroundLoading,backgroundLoaded,presentationLoading,presentationLoaded,
        pageRefReleased,pagePayloadRetired};
    // Virtual84 loader suffix only. Page virtual74 entry1FAAF0 is later.
    enum class LoadReturnPhase:std::uint8_t {forestPending,application344Pending,
        selectedPending,selectedFailed,resourcesPending,resourcesFailed,returned};
    enum class ActivationPhase:std::uint8_t {notActivated,showPending,resetPending,widgetsPending,
        moviePending,displayPending,returned};
    enum class EntryPhase:std::uint8_t {notEntered,profilePending,appearancePending,
        statPending,deltaPending,cashPending,divisibility5Pending,divisibility4Pending,
        wattoBindingPending,wattoBindingFailed,wattoActivationPending,wattoActivationFailed,controlsPending,
        groupDisablePending,groupEnablePending,groupSelectionPending,explicitDeselectPending,
        randomFirstPending,randomSecondPending,cameraActivationPending,cameraSlotsPending,
        popupPending,backgroundPending,returned};
    struct Diagnostic {
        Status status=Status::complete;std::uint32_t sourcePC=0;
        bool sourceEffectsCommitted=false;
        game::OriginalUiEntityScope50KG::Result scope{};
        OriginalUiGarageScreenTextDiagnostic50KG text{};
        OriginalUiGarageQuad50KG::Diagnostic quad{};
        assets::OriginalUiPageDiagnostic50KG page{};
        game::OriginalFrontendCameraCold50LA::Result camera{};
        VehicleWindowDiagnostic50DS renderer{};
        assets::OriginalStandardMaterialDiagnostic50LA loadResources{};
        OriginalUiGarageWidget1Diagnostic50KG widget{};
    };
    struct QuadObservation {
        std::uint32_t objectOffset=0;
        std::array<float,4> position10{},size20{},uv30{};
        float field40=0,scale4C=0,scale50=0;std::uint32_t color44=0;
        std::uint16_t alpha48=0;std::uint8_t flags5CValue=0,flags5CKnownMask=0;
        bool material54Null=false,resource58Null=false;
    };
    struct Observation {
        Stage stage=Stage::empty;const C* entity=nullptr;
        LoadReturnPhase loadReturnPhase=LoadReturnPhase::forestPending;
        bool application344Known=false,selectedObserverAttached=false,sourceLoadReturned=false,managerAttached=false;
        std::int32_t application344=0;const C* selected4C4=nullptr;
        ActivationPhase activationPhase=ActivationPhase::notActivated;
        std::size_t activationGroup=0,activationControl=0,activationWidget=0;std::uint8_t activationPass=0;
        bool sourceActivationReturned=false;
        EntryPhase entryPhase=EntryPhase::notEntered;std::size_t entryAppearanceStep=0,entryStatCount=0;
        bool entryProfileKnown=false,wattoObserverAttached=false,sourceEntryReturned=false;
        std::uint64_t updatePublication=0;std::uint32_t updateSourcePC=0;
        std::size_t updatedPageChildren=0,updatedLights=0;bool updateFailed=false,updateReturned=false;
        std::uint8_t entryGroupPass=0,entrySelectionStep=0,entryCameraStep=0;
        std::size_t entryGroupControl=0,entryControlWidget=0,entryExplicitWidget=0;
        std::optional<std::uint32_t> information4DC,field4E0,entryCareer;
        std::array<std::optional<std::int32_t>,6> current4E8{},delta500{};
        std::optional<std::int32_t> available34B0;
        const C* manager130=nullptr;const C* manager270=nullptr;
        const OriginalUiGaragePodScene50KG* scene4C0=nullptr;
        std::size_t constructedQuads=0,constructedTexts=0;
        const C* eventRouter134=nullptr;std::size_t routerHeads=0,controlEntities=0,groupEntities=0;
        std::array<bool,8> routerMembershipKnown{};bool loaderDatClosed=false,sharedLoaderReturned=false;
        std::uint32_t sourceVtable=0,pendingPC=0;
        bool constructorReturned=false,managerBorrowsKnown=false;
        bool aperturePointersKnown=false,sceneReferenceReleased=false;
        bool pageReferenceReleased=false,pageEntityCountReleased=false;
        std::int32_t openSize274=0,left278=0,right280=0,top27C=0;
        std::int32_t left288=0,right290=0,bottom294=0,input268=0;
        float savedDelta298=0,time4B0=0,panelOffset4D8=0;
        std::array<std::int32_t,6> pairFirst{},pairSecond{};
        std::array<std::uint32_t,5> timer34B4{};
        std::array<std::uint32_t,2> enabled34C8{};
    };
    OriginalUiGaragePageOwner50KG()noexcept;
    ~OriginalUiGaragePageOwner50KG()noexcept;
    OriginalUiGaragePageOwner50KG(const OriginalUiGaragePageOwner50KG&)=delete;
    OriginalUiGaragePageOwner50KG& operator=(const OriginalUiGaragePageOwner50KG&)=delete;
    // Only concrete constructor custody, never load/entry/draw readiness.
    bool currentConstructor()const noexcept;
    Observation inspect()const noexcept;
    bool inspectQuad(std::size_t,QuadObservation&)const noexcept;
private:
    class RootInputs final {
        friend struct ::original_main_entry50KG::Application;
        const ::original_main_entry50KG::Application* const issuer_;
        const std::shared_ptr<Authority> authority_;
        const std::shared_ptr<Grant> grant_;
        const std::shared_ptr<Widget> widgets_;
        const std::shared_ptr<Fog> fog_;
        RootInputs(const ::original_main_entry50KG::Application* issuer,
            std::shared_ptr<Authority> authority,std::shared_ptr<Grant> grant,
            std::shared_ptr<Widget> widgets,std::shared_ptr<Fog> fog)noexcept
            :issuer_(issuer),authority_(std::move(authority)),grant_(std::move(grant)),
             widgets_(std::move(widgets)),fog_(std::move(fog)){}
    public:
        const ::original_main_entry50KG::Application* issuer()const noexcept{return issuer_;}
        const auto& authority()const noexcept{return authority_;}
        const auto& grant()const noexcept{return grant_;}
        const auto& widgets()const noexcept{return widgets_;}
        const auto& fog()const noexcept{return fog_;}
    };
    using Quad=OriginalUiGarageQuad50KG;
    // Fixed embedded storage. Source2E02F0 is invoked explicitly in source
    // order, without introducing two independent native heap allocations.
    //Canonical2E1920 backing, not an immutable aperture draw projection.
    struct Aperture {
        struct Vertex {
            std::array<float,4> authored00{{0,0,1,1}};
            std::uint32_t aliases10=0;
            std::array<Quad::Qword*,2> stq14{},rgba1C{},xyz24{};
        };
        struct Descriptor {
            std::unique_ptr<Quad::Qword[],Quad::PacketDelete> allocation00;
            Quad::Qword* vif04=nullptr;Quad::Qword* gif08=nullptr;Quad::Qword* tag0C=nullptr;
            bool pointersKnown=false,field10Known=false;
            Quad::Qword* dmaData4=nullptr;
            std::uint8_t sourcePrimitive=0;std::uint32_t sourceVertices=0;
        };
        std::unique_ptr<Vertex[]> vertices00;
        std::int32_t count04=0,count08=0,type0C=2;
        float scale10=1,field14=0,field18=1;
        std::array<float,4> offset20{{0,0,0,1}};
        std::unique_ptr<std::unique_ptr<Quad::Primitive>[]> primitives30;
        std::int32_t count34=0,owns38=0,dirty3C=0;
        std::uint8_t flags40=3;
        std::unique_ptr<Descriptor[]> packets44;
        std::int32_t count48=0,capacity4C=0;
        // Native lifetime of the exact retained packet/Primitive borrow.
        std::uint32_t nativeGarageFifoPins_=0;
        std::uint64_t packetRevision_=1;
    };
    std::array<std::unique_ptr<Aperture>,2> apertures_;
    struct AperturePacketBorrow {
        const Quad::Primitive* primitive=nullptr;
        const Quad::Qword* packet=nullptr;
        std::uint64_t revision=0;
    };
    std::size_t apertureDrawOrdinal_=0;
    std::uint64_t apertureDrawPublication_=0;
    std::uint32_t apertureDrawSourcePC_=0;
    bool aperturePacketCurrent2E21D0(std::size_t)const noexcept;
    bool canMutateAperture2E21D0(std::size_t)const noexcept;
    bool borrowAperturePacket2E21D0(std::size_t,AperturePacketBorrow&)const noexcept;
    bool drawAperture2E21D0(OriginalUiGarageDrawContribution50KG&,std::size_t,Diagnostic&)noexcept;
    bool lowerAperture327E90(std::size_t,std::uint64_t,
        const std::array<std::uint32_t,2>&,Quad::LoweredPacket327E90&,Diagnostic&)const noexcept;
    //One BSS pair per original source location, not per Page instance.
    struct PolygonAssertion {std::atomic<std::uint8_t> guard{0};std::atomic<std::uint32_t> word{0};};
    static PolygonAssertion& polygonDraw8F50()noexcept;
    static PolygonAssertion& polygonCount8F38()noexcept;
    static PolygonAssertion& polygonLimit8F3C()noexcept;
    static PolygonAssertion& polygonQuads8F44()noexcept;
    static PolygonAssertion& polygonAliases8F64()noexcept;
    bool loadAperture1D66F8(std::size_t,Diagnostic&)noexcept;
    std::shared_ptr<game::OriginalFrontendCameraCold50LA> sharedCamera_;
    bool retainManagerSearch1D7B3C(std::shared_ptr<OriginalUiGarageManagerSearch50KG>,Diagnostic&)noexcept;
    std::shared_ptr<OriginalUiGarageManagerSearch50KG> managerSearch_;
    bool beginSharedCamera1D7A90(Diagnostic&)noexcept;
    bool continueSharedCameraViewport114A00(Diagnostic&)noexcept;
    bool continueSharedCameraConstructor2361C0(Diagnostic&)noexcept;
    bool continueSharedCameraObserver114770(Diagnostic&)noexcept;
    bool continueSharedPrompts1D682C(Diagnostic&)noexcept;
    bool continueSharedText1D6AEC(Diagnostic&)noexcept;
    bool continueSharedBackground1D6CC4(Diagnostic&)noexcept;
    std::size_t sharedPromptCount_=0;
    std::shared_ptr<const assets::OriginalApplicationLocalization50KG::SelectedText> sharedText32C_;
    std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA> sharedCameraDisplay_;
    bool sharedCameraDisplayPinned_=false;
    std::uint8_t sharedViewportDrainPhase_=0,sharedCameraCallbackCount_=0;
    struct TextStorage {alignas(Text) std::array<std::byte,sizeof(Text)> bytes;Text* value=nullptr;};
    struct RouterNode {
        C* widget0=nullptr;std::uint32_t enabled4=0;
        std::unique_ptr<RouterNode> next8;
    };
    struct RouterHead {std::unique_ptr<RouterNode> first0;RouterNode* last4=nullptr;};
    struct PageRouter {
        C entity;std::uint32_t sourceVtable=0;
        std::optional<const C*> manager30,page34;
        std::unique_ptr<RouterHead*[]> table38;
        std::array<std::unique_ptr<RouterHead>,8> heads;
        std::array<bool,8> headKnown{},membershipKnown{};
        std::size_t visitedWidgets=0,retiredHeads=0;bool constructed=false,attached=false,retirementStarted=false;
    };
    struct PageControl {
        C entity;std::uint32_t sourceVtable=0;
        std::int32_t sectionPresent30=0,count34=0;std::unique_ptr<C*[]> widgets38;
        std::uint32_t field3C=0;std::size_t widgetsKnown=0;
        bool constructed=false,loaded=false,retirementStarted=false,payloadRetired=false;
    };
    struct PageGroup {
        C entity;std::uint32_t sourceVtable=0;
        std::optional<std::int32_t> enabled30,selection3C;
        std::int32_t count34=0;std::unique_ptr<PageControl*[]> controls38;
        PageControl* selected40=nullptr;std::int32_t associatedCount44=0;
        std::unique_ptr<PageControl*[]> associated48;std::int32_t keepEnabled4C=1;
        void* field50=nullptr;std::size_t controlsKnown=0,associatedKnown=0;
        bool constructed=false,loaded=false,retirementStarted=false,payloadRetired=false;
    };
    //2A66B0/2A66C8 initialize a flat selector/pointer pair, not a linked list.
    //2A6CC8 later stores the actual selected PageGroup in its second field.
    //malloc1D6E70 leaves payload unknown until Page90 writes it.
    struct ExtraHead {std::optional<std::int32_t> first0;std::optional<PageGroup*> last4;};
    std::unique_ptr<PageRouter> router_;
    std::array<std::unique_ptr<PageControl>,8> controls_;
    std::array<std::unique_ptr<PageGroup>,3> groups_;
    std::unique_ptr<PageControl*[]> controlTable_;
    std::unique_ptr<PageGroup*[]> groupTable_;
    std::unique_ptr<ExtraHead*[]> extraTable_;
    std::array<std::unique_ptr<ExtraHead>,2> extraHeads_;
    std::size_t controlConstructionCount_=0,groupConstructionCount_=0;
    std::array<bool,8> controlSlotsKnown_{};std::array<bool,3> groupSlotsKnown_{};
    bool collectionsLoaded_=false,loaderDatClosed1573B0_=false;
    bool continueSharedCollections1D6DAC(Diagnostic&)noexcept;
    bool appendRouterWidget2A71A0(C&,const std::array<std::int32_t,2>&,Diagnostic&);
    bool loadControl1D7180(std::size_t,Diagnostic&);
    bool loadGroup1D6F08(std::size_t,Diagnostic&);
    bool sharedCollectionsCurrent()const noexcept;
    bool sharedCollectionsPayloadCurrent()const noexcept;
    struct Base {
        std::uint32_t staticWidgetCount13C=0,controlCount140=0,groupCount148=0;
        PageControl** controls144=nullptr;PageGroup** groups14C=nullptr;ExtraHead** extra150=nullptr;
        void* backgroundTexture154=nullptr;void* moviePlayer158=nullptr;
        PageRouter* router134=nullptr;std::uint32_t field260=0,field264=0,input268=0;std::int32_t pageNumber138=0;
        std::uint8_t moviePathFirstByte15C=0;
        // Manager130 is not initialized by the constructor.
        std::optional<const C*> manager130;
    };
    struct Shared {
        std::optional<const C*> manager270;
        std::int32_t openSize274=0,left278=0,top27C=0,right280=0;
        std::optional<std::int32_t> top284;
        std::int32_t left288=0;std::optional<std::int32_t> bottom28C;
        std::int32_t right290=0,bottom294=0;float savedDelta298=0;
        std::uint32_t field420=0,field42C=0,field434=0;
        std::optional<std::uint32_t> backgroundColor430;
        //424/428 are genuinely UNWRITTEN until1D66E0, not known-null.
        std::array<std::optional<const void*>,2> apertures424;
        void* popup438=nullptr;float time4B0=0;
        std::optional<std::int32_t> state4B4;std::optional<float> speed4B8;
        void* camera4BC=nullptr;
    };
    const std::thread::id ownerThread_=std::this_thread::get_id();
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::shared_ptr<Authority> authority_;std::shared_ptr<Grant> grant_;
    std::shared_ptr<Widget> widgets_;std::shared_ptr<Fog> fog_;
    C entity_;Base base_;Shared shared_;
    std::array<Quad,129> quads_{};std::size_t quadCount_=0;
    TextStorage sharedText440_,costText3340_;std::size_t textCount_=0;
    std::uint32_t vtable_=0,pendingPC_=0;Stage stage_=Stage::empty;
    // Native backing retains constructor failures. Only scene4C0 is the
    // source field, assigned AFTER1B2870 returns, never copied from Session.
    std::unique_ptr<OriginalUiGaragePodScene50KG> sceneBacking_;
    // Native storage only, moved here AFTER real Scene2701CC/1FA1BC. Main's
    // accepted command aliases this Page control block, so destroying Scene
    // storage at the source pointer clear would leave that alias dangling.
    // Only completed normal-manager audio discharge may destroy this tombstone.
    std::unique_ptr<OriginalUiGaragePodScene50KG> retiredSceneBacking_;
    OriginalUiGaragePodScene50KG* scene4C0_=nullptr;
    const C* selected4C4_=nullptr;OriginalUiGarageWattoOwner50KG* watto33B0_=nullptr;
    float panelOffset4D8_=0;std::uint32_t field4E4_=0;
    // One genuine private1FDE70 invocation. The enclosing input consumer must
    // call once per translated source event, not replay a completed receipt.
    // A failure after any source prefix permanently closes this cursor.
    enum class InputPhase : std::uint8_t {idle,planned,stateWrite,group,request,watto,effect,transport,returned,failed};
    struct InputOutcome {
        enum class Status : std::uint8_t {unavailable,ignored,completed,failed};
        Status status=Status::unavailable;
        std::uint64_t serial=0,transportBoundary=0;
        game::OriginalUiRouteRequest50KG request{};
        bool sourceEffectsCommitted=false,requestWritten=false;
        // Source1FDE70 returns0; native operation success is not guest return1.
        std::uint32_t sourceReturn=0,sourcePC=0;
    };
    InputPhase inputPhase_=InputPhase::idle;
    std::uint64_t inputSerial_=0,inputTransportBoundary_=0;
    std::uint32_t inputSource_=0,inputController_=0,inputFailurePC_=0;
    bool inputEffectsCommitted_=false,inputRequestWritten_=false;
    game::OriginalUiGarageInputPlan50KG inputPlan_{};
    game::OriginalUiGarageWattoReceipt50KG inputWattoReceipt_{};
    game::OriginalUiGarageWattoLifecycleDiagnostic50KG inputWattoDiagnostic_{};
    bool dispatchInput1FDE70(std::uint32_t sourceInput,std::uint32_t controller,
        InputOutcome&,Diagnostic&)noexcept;
    bool inputGroupStepCurrent(std::int32_t direction)const noexcept;
    bool inputGroupStep2A50F0(std::int32_t direction,Diagnostic&)noexcept;

    std::array<std::int32_t,6> pairFirst_{},pairSecond_{};
    std::array<std::uint32_t,5> timer34B4_{};
    std::array<std::uint32_t,2> enabled34C8_{};
    bool entityLive_=false,entityRefReleased_=false,entityCountReleased_=false;
    bool sceneReferenceReleased_=false;
    // Existing immutable metadata/draw owners can be pinned here, but their
    // material/glyph projections are never substituted for source2DEF40,
    // ScreenText2E02F0, counted controls, or the loader's return boundary.
    std::shared_ptr<const PageData> sharedMetadata_;
    std::unique_ptr<assets::OriginalUiDat50KG> loaderDat_;
    std::string sourcePageFile_;
    std::optional<std::string> name30_;
    std::shared_ptr<const OriginalUiGarageSharedPageDrawOwner50KG> sharedDraw_;
    bool inputsCurrent()const noexcept;
    bool fail(Diagnostic&,Status,std::uint32_t)const noexcept;
    void constructQuad2DEF40(std::uint32_t)noexcept;
    void constructText2E02F0(TextStorage&)noexcept;
    bool prepare1F9FF0(const RootInputs&,Diagnostic&)noexcept;
    bool retainSharedColdResources(std::shared_ptr<const PageData>,
        std::shared_ptr<const OriginalUiGarageSharedPageDrawOwner50KG>,Diagnostic&)noexcept;
    //Actual shared loader, including owned parser/geometry/Camera/prompts/Text
    //and counted router/controls/groups. Return does not admit Page entry.
    bool beginSharedLoad1D63B0(Diagnostic&)noexcept;
    // Terminal native cleanup of the actual first prompt proof failure only.
    // This does not certify a loaded/attached Page or normal Scene retirement.
    enum class FailedSharedRetirement : std::uint8_t {notStarted,cameraRetiring,cameraRetired,payloadRetiring,returned};
    FailedSharedRetirement failedSharedRetirement_=FailedSharedRetirement::notStarted;
    bool failedFirstPrompt29B638_=false,failedLoaderDatReleased_=false;
    std::uint64_t failedPromptGeneration_=0,failedPromptEpoch_=0;
    bool sharedPromptFailurePrefixCurrent()const noexcept;
    bool preflightFailedSharedPrompt(Diagnostic&)const noexcept;
    bool retireFailedSharedPrompt(Diagnostic&)noexcept;
    bool finishFailedSharedBase2A5CF0(Diagnostic&)noexcept;
    bool preflightFailedSharedCamera231310(Diagnostic&)const noexcept;
    bool retireFailedSharedCamera231310(Diagnostic&)noexcept;
    bool preflightFailedPartialCamera231310(Diagnostic&)const noexcept;
    bool retireFailedPartialCamera231310(Diagnostic&)noexcept;
    bool preflightFailedManagerCamera231310(bool partialScene,Diagnostic&)const noexcept;
    bool retireFailedManagerCamera231310(bool partialScene,Diagnostic&)noexcept;

    bool constructScene1FA308(std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        std::shared_ptr<OriginalUiMainAudio50KG>,OriginalUiGarageDroidAudio50GN*,
        std::shared_ptr<OriginalAudioPendingWaves50KG>,Diagnostic&)noexcept;
    // Typed current-source borrow only. It does not transfer source ownership
    // to Session or permit attaching the detached PodScene Entity to Page.
    OriginalUiGaragePodScene50KG* borrowScene4C0()const noexcept;
    bool continuePageSpecific1FA318(Diagnostic&)noexcept;
    bool continuePresentation1FA4DC(Diagnostic&)noexcept;
    std::size_t presentationNotchesLoaded_=0,presentationArrowsLoaded_=0;
    std::uint8_t presentationStep_=0;
    bool preflightSceneLoad1FB0F0(Diagnostic&)const noexcept;
    bool preflightSceneAttachment1B48A0(Diagnostic&)const noexcept;
    bool failedSceneAttachment1B4A38_=false;
    std::uint64_t failedSceneAttachmentGeneration_=0,failedSceneAttachmentEpoch_=0;
    enum class FailedSceneRetirement : std::uint8_t {notStarted,rawRetiring,rawRetired,staticPending,staticRetired,detachedEmitters,detachedActors,detachedParticles,cameraRetiring,detachedOwnersRetired,searchRetiring,searchRetired,routerRetiring,payloadRetiring,returned};
    FailedSceneRetirement failedSceneRetirement_=FailedSceneRetirement::notStarted;
    std::size_t failedAttachedRetiredCount_=0;
    bool partialSceneAttachmentFailureCurrent()const noexcept;
    bool failedPartialCameraRetiredCurrent()const noexcept;
    bool preflightFailedSceneAttachment(Diagnostic&)const noexcept;
    bool retireFailedSceneAttachmentPrefix(Diagnostic&)noexcept;

    // Begin actual142590/104970 without declaring1FA170/1413E0 finished.
    // At present exact Video442F70 primitive cleanup is still unjoined.
    bool sceneAttachmentPrefixCurrent1B48A0()const noexcept;
    bool cameraRegistrationCurrent1FB608()const noexcept;
    bool continueSceneAttachments1FB5C0(Diagnostic&)noexcept;
    bool continueCameraHandoff1FB5E4(Diagnostic&)noexcept;
    bool continueLateSceneAttachments1FB648(OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
    // The real call sequence at1FAA88..1FAAC8, invoked only after the Scene
    // returned. It neither calls Page virtual74 nor grants Page entry/draw.
    bool continueLoadReturn1FAA88(OriginalUiSharedDialogs50KG&,Diagnostic&)noexcept;
    bool lateSceneAttachmentsCurrent1FB64C()const noexcept;
    // Actual initial Watto-binding rejection after completed activation and
    // the entry profile/appearance/stat prefix. No binding/Observer effects.
    bool failedWattoBinding1D4800_=false;
    std::uint64_t failedWattoBindingGeneration_=0,failedWattoBindingEpoch_=0;
    enum class FailedWattoBindingRetirement:std::uint8_t {
        notStarted,childrenRetiring,routerRetiring,payloadRetiring,returned};
    FailedWattoBindingRetirement failedWattoBindingRetirement_=FailedWattoBindingRetirement::notStarted;
    bool wattoBindingFailureCurrent()const noexcept;
    bool wattoBindingFailureOwnerCurrent()const noexcept;
    bool preflightFailedWattoBinding1D4800(Diagnostic&)const noexcept;
    bool retireFailedWattoBinding1D4800(Diagnostic&)noexcept;
    bool unboundWattoFailureOperandsCurrent()const noexcept;
    void retainWattoBindingFailure1D4800(const assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    bool failedLoadedPageHide1415A0_=false;
    std::uint64_t failedLoadedPageHideGeneration_=0,failedLoadedPageHideEpoch_=0;
    enum class FailedLoadedHideRetirement:std::uint8_t {
        notStarted,childrenRetiring,routerRetiring,payloadRetiring,returned};
    FailedLoadedHideRetirement failedLoadedHideRetirement_=FailedLoadedHideRetirement::notStarted;
    bool loadedPageHideFailureCurrent()const noexcept;
    bool loadedHideFailureOwnerCurrent()const noexcept;
    bool preflightFailedLoadedHide1415A0(Diagnostic&)const noexcept;
    bool retireFailedLoadedHide1415A0(Diagnostic&)noexcept;
    bool failedLateBorrow1B4D24_=false;
    std::uint64_t failedLateBorrowGeneration_=0,failedLateBorrowEpoch_=0;
    bool lateSceneBorrowFailureCurrent()const noexcept;
    enum class FailedLateBorrowRetirement:std::uint8_t {
        notStarted,initialRetiring,detachedAudioRetiring,routerRetiring,payloadRetiring,returned};
    FailedLateBorrowRetirement failedLateBorrowRetirement_=FailedLateBorrowRetirement::notStarted;
    bool lateBorrowFailureOwnerCurrent()const noexcept;
    bool preflightFailedLateBorrow1B4D24(Diagnostic&)const noexcept;
    bool retireFailedLateBorrow1B4D24(Diagnostic&)noexcept;
    bool preflightFailedLateBorrowCamera231310(Diagnostic&)const noexcept;
    bool retireFailedLateBorrowCamera231310(Diagnostic&)noexcept;
    bool retireFailedLateBorrowInitialChildren142590(Diagnostic&)noexcept;
    bool retireFailedLateBorrowDetachedAudio271470(Diagnostic&)noexcept;
    bool retireInitialSceneChildren142590(Diagnostic&)noexcept;
    bool selectedRegistrationCurrent1FB67C()const noexcept;
    bool sourceLoadReturned1FAAC8()const noexcept;
    bool attachToManager1D1C40(Diagnostic&)noexcept;
    LoadReturnPhase loadReturnPhase_=LoadReturnPhase::forestPending;
    std::optional<std::int32_t> loadApplication344_;
    std::shared_ptr<OriginalUiGarageSelectedModel50KG> loadSelectedOwner_;
    const void* loadSelectedObserver_=nullptr;
    const C* loadSelectedZone_=nullptr;
    assets::OriginalStandardMaterialCollectorReceipt50LA loadCollector_;
    assets::OriginalStandardMaterialDiagnostic50LA loadFailure_;
    bool managerAttached1D1C40_=false;
    //Manager78 calls Page90 first at2A3C44, then Page74 at2A3C58.
    bool continueActivation1D7390(Diagnostic&)noexcept;
    bool sourceActivationReturned2A680C()const noexcept;
    bool activationWidgetCallCurrent(std::size_t)const noexcept;
    ActivationPhase activationPhase_=ActivationPhase::notActivated;
    std::size_t activationGroup_=0,activationControl_=0,activationWidget_=0;
    std::uint8_t activationPass_=0;
    bool activationFailed_=false;OriginalUiGarageWidget1Diagnostic50KG activationFailure_;
    //Private virtual74 entry continuation. No request or public readiness token
    //can replace the actual attached Page and its retained profile owner.
    bool continueEntry1FAAF0(Diagnostic&)noexcept;
    bool continueEntryAppearance1FBAB0(Diagnostic&)noexcept;
    bool continueEntryControls1FAC24(Diagnostic&)noexcept;
    bool continueEntryCamera1156E0(Diagnostic&)noexcept;
    bool entryWidgetCallCurrent(std::size_t,std::uint32_t,std::int32_t)const noexcept;
    bool entryCameraActivationReturned115724()const noexcept;
    bool sourceEntryReturned1FAE00()const noexcept;
    OriginalUiGarageSceneLighting50KG* borrowRenderFog13EA10(const game::OriginalWreckComponent50LA&)const noexcept;
    bool renderFogEnd13E9C0(const game::OriginalWreckComponent50LA&,float&)const noexcept;
    bool renderEmptyPool162870(game::OriginalWreckComponent50LA&,std::uint64_t,std::uint32_t&)noexcept;
    std::shared_ptr<OriginalUiGarageVisibility50KG> renderVisibility114D28()const noexcept;
    //Actual manager child call, after its preceding Widget/Watto callbacks.
    //Consumes canonical Page timers and the already-prepared Root publication.
    bool updateSharedCamera2363B0(float,Diagnostic&) noexcept;
    bool continueUpdate1FB6A0(const OriginalUiGarageSession50KG&,OriginalUiGarageRendererLease50KG&,Diagnostic&)noexcept;
    bool actorSampleFailureCurrent()const noexcept;
    // Called only by the actual update after its canonical positive timer
    // crosses zero. Native prefix cursors prohibit replay, not source flags.
    bool sourceTimerFrameCurrent(const OriginalUiGarageSession50KG&)const noexcept;
    bool runPositiveTimer1FB708(const OriginalUiGarageSession50KG&,std::size_t,Diagnostic&)noexcept;
    bool stopSpeechTimer2924A0(const OriginalUiGarageSession50KG&,Diagnostic&)noexcept;
    std::uint64_t timerCallbackPublication_=0,speechStopPublication_=0;
    std::size_t timerCallbackOrdinal_=3,timerCallbackStep_=0;
    bool timerCallbackPending_=false,speechStopPending_=false;
    std::uint64_t updatePublication_=0;std::uint32_t updateSourcePC_=0;
    std::uint64_t completedUpdateFrames_=0,lastCompletedUpdatePublication_=0;
    C* updateChild_=nullptr;std::size_t updatedPageChildren_=0,updatedLights_=0;
    bool updateFailed_=false,updateReturned_=false;
    bool entryProfileCurrent()const noexcept;
    bool draw1FBE10(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool drawShared1D7550(OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    std::size_t sharedApertureOrdinal_=0;
    bool runtimeWidgetCallCurrent(std::size_t,std::uint32_t,std::int32_t)const noexcept;
    bool runtimeWidgetCall(C*,std::uint32_t,std::int32_t,std::uint32_t,Diagnostic&)noexcept;
    bool runtimeControlCall(PageControl*,std::uint32_t,std::int32_t,Diagnostic&)noexcept;
    bool runtimeGroupEnable(std::size_t,bool,Diagnostic&)noexcept;
    bool runSharedHook1FAF10(std::uint32_t,Diagnostic&)noexcept;
    bool beginSharedManagerTransition1FEA10(const OriginalUiGarageSession50KG&,Diagnostic&)noexcept;
    bool pollSharedManagerTransition1FE7C0(const OriginalUiGarageSession50KG&,bool&,Diagnostic&)const noexcept;
    bool advanceSharedApertures1D7550(const OriginalUiGarageSession50KG&,OriginalUiGarageDrawContribution50KG&,Diagnostic&)noexcept;
    bool updatePanel1FE860(const OriginalUiGarageSession50KG&,float,Diagnostic&)noexcept;
    bool setRouterActivity1415A0(bool,Diagnostic&)noexcept;
    bool sharedHookPending_=false;
    std::uint32_t sharedHookPC_=0;

    bool runtimeGroupSelect2A4F50(std::size_t,std::int32_t,Diagnostic&)noexcept;
    bool runtimeSetActiveGroup2A6BE0(std::size_t,std::int32_t,std::size_t,Diagnostic&)noexcept;
    // Actual synchronous source invocation custody, distinct from cold entry.
    // It is never a caller-provided row or route readiness credential.
    C* runtimeWidgetTarget_=nullptr;std::uint64_t runtimeWidgetSerial_=0;
    std::uint32_t runtimeWidgetOperation_=0,runtimeWidgetSourcePC_=0;
    std::int32_t runtimeWidgetArgument_=0;bool runtimeWidgetPending_=false;

    bool drawReceiverCurrent(const OriginalUiGarageDrawContribution50KG&,const Quad*)const noexcept;
    bool drawReceiverCurrent(const OriginalUiGarageDrawContribution50KG&,const Text*)const noexcept;
    std::uint64_t drawPublication_=0;bool drawPending_=false;
    std::uint32_t drawSourcePC_=0;
    bool wattoEntryBindingCurrent1FAC08(const OriginalUiGarageWattoOwner50KG&)const noexcept;
    struct EntryProfile {
        std::shared_ptr<OriginalUiRiderProfileOwner50KG> owner;
        std::uint64_t generation=0;std::uint32_t mode=0,career100C0=0;
        std::array<float,6> current1920{};
        std::uint32_t unspent2194=0,primaryColor=0,secondaryColor=0;
    };
    std::optional<EntryProfile> entryProfile_;
    EntryPhase entryPhase_=EntryPhase::notEntered;
    std::size_t entryAppearanceStep_=0,entryStatCount_=0;
    std::optional<std::uint32_t> information4DC_,entryField4E0_;
    std::array<std::optional<std::int32_t>,6> current4E8_{},delta500_{};
    std::optional<std::int32_t> available34B0_;
    const OriginalUiGarageModelResource50KG* entryWattoModel_=nullptr;
    const void* entryWattoObserver_=nullptr;const C* entryWattoZone_=nullptr;
    assets::OriginalStandardMaterialDiagnostic50LA entryWattoFailure_;
    bool entryWattoActivationStarted_=false;
    std::uint8_t entryGroupPass_=0,entrySelectionStep_=0,entryCameraStep_=0;
    std::size_t entryGroupControl_=0,entryControlWidget_=0,entryExplicitWidget_=0;
    bool entryGroupStored_=false,entryControlStored_=false;

    std::array<C*,3> sceneLateChildren_{};
    std::size_t sceneLateAttachCursor_=0;
    bool sceneLateBorrowCaptured_=false;
    bool retireLateSceneChildren142590(Diagnostic&)noexcept;
    std::size_t sceneLateRetiredCount_=0;
    bool retireSceneChildren142590(Diagnostic&)noexcept;
    std::size_t sceneInitialRetiredCount_=0;
    std::array<C*,10> scenePageChildren_{};
    std::size_t scenePageAttachCursor_=0;
    std::uint8_t cameraHandoffPhase_=0;
    bool scenePageBorrowCaptured_=false;
    bool retireCameraChild231310(Diagnostic&)noexcept;
    std::uint8_t cameraRetirementStep_=0;
    bool beginRetirement142590(Diagnostic&)noexcept;
    bool retireRouter2A7010(Diagnostic&)noexcept;
    bool continueBaseRetirement2A5CF0(Diagnostic&)noexcept;
    std::size_t retiredControls_=0,retiredGroups_=0,retiredExtraHeads_=0;
    std::uint8_t baseRetirementStep_=0;
    bool retireScene1FA1B4(Diagnostic&)noexcept;
    bool continuePayloadRetirement1FA170(Diagnostic&)noexcept;
    bool drainEmbedded2DEFE0(std::uint8_t&,Diagnostic&,std::uint32_t,std::uint32_t)noexcept;
    bool retireQuad2DEFE0(std::size_t,Diagnostic&)noexcept;
    bool retireAperture2E1980(std::size_t,Diagnostic&)noexcept;
    std::uint8_t payloadRetirementStep_=0;
    std::size_t retiredNotches_=0,retiredTabs_=0,retiredApertures_=0,retiredPrompts_=0;
    std::array<std::uint8_t,2> apertureRetirementDrains_{};
};
}
