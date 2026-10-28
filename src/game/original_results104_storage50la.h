#pragma once
// Implementation-only storage; never imported by a gameplay umbrella.
#include "original_results104_50la.h"
#include "original_pause_storage50la.h"
#include "../assets/original_results_primitive_references50la.h"
#include "../assets/native_asset48b.h"
#include "../../tests/native/original_ui_source_timing50lh.h"
#include <optional>
#include <limits>
namespace ra_cold_roster50LA { class Operation; }
namespace rr::game {
struct OriginalResults10450LA::Impl final {
    using D=OriginalResults104Diagnostic50LA;
    using S=D::Status;
    // Reuse the existing concrete ScreenObject storage representations, NOT
    // a Pause100 instance, its reference ledger, authority or completion.
    using Texture=OriginalPauseStorage50LA::Texture;
    using Text=OriginalPauseStorage50LA::Text;
    using Polygon=OriginalPauseStorage50LA::Polygon;
    using Packet=OriginalPauseStorage50LA::Packet;
    using Primitive=OriginalPauseStorage50LA::Primitive;
    using Glyph=OriginalPauseStorage50LA::Glyph;
    using TextPrimitive=OriginalPauseStorage50LA::TextPrimitive;
    using TextPacket=OriginalPauseStorage50LA::TextPacket;
    using PacketVertex=OriginalPauseStorage50LA::PacketVertex;
    using References=assets::OriginalResultsPrimitiveReferences50LA;
    using Role=References::Role;
    const std::thread::id nativeCaptureThread=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    OriginalWreckComponent50LA component;
    bool counted=false,constructed=false,loadAttempted=false,loadComplete=false,disabled=false,closed=false;
    std::shared_ptr<::original_main_entry50KG::ResultsRendererLease50LA> rendererLease;
    std::uint32_t nativeFrameBorrows=0;
    bool retirementStarted=false;
    std::uint32_t retirementStep=0;
    std::size_t retirementRemaining=std::numeric_limits<std::size_t>::max();
    bool loadedResourceTraversal=false;
    std::uint32_t mask=0;
    std::optional<Texture> center40;
    std::optional<std::array<Texture,2>> backdropsA0;
    std::optional<Text> base180;
    struct BaseFields {float opening30=.25f;std::uint32_t field34=0,hidden160=0;
        float transition164=0;std::uint32_t blue168=0x669bcc,yellow16C=0xe5d96c,grey170=0xcccccc;};
    std::optional<BaseFields> base;
    std::array<std::unique_ptr<Polygon>,2> polygons;
    std::optional<std::array<Texture,4>> icons210;
    std::optional<std::array<Text,2>> labels3A0;
    std::optional<std::array<Text,3>> titles490;
    std::optional<std::array<Texture,3>> medals5F0;
    std::optional<Text> text710,text780,text7F0,text860,text8D0,text940,text9B0,textA20;
    std::optional<std::array<Text,12>> tableAF0;
    std::optional<std::array<Texture,3>> separators1030;
    std::optional<std::array<Text,2>> text1150;
    std::optional<std::array<Text,8>> statistics1230;
    std::optional<std::array<Text,4>> text15B0;
    std::optional<std::array<Text,3>> text1770;
    struct DerivedFields {std::uint32_t acknowledgement20C=0,mode200=0,previous204=0,
        field390=0,fieldADC=0,fieldAE0=0;};
    std::optional<DerivedFields> derived;
    // Neither the recovered constructor nor loader establishes these lanes.
    // The separately chosen first-activation native visual policy below is
    // NOT original initialization. Later source arithmetic preserves its
    // initial provenance; this metadata never authorizes input or scheduling.
    std::optional<std::uint32_t> pulse394,direction398;
    enum class PulseStartProvenance : std::uint8_t {
        notObserved, nativeFirstActivationEndpoint,
        preservedKnownPair, preservedPartialUnknown
    };
    // "Preserved" records existing knownness, never its original-game origin.
    // Non-notObserved means the ONE actual activation boundary was observed,
    // not that phase is usable or that any source callback completed.
    PulseStartProvenance pulseStartProvenance=PulseStartProvenance::notObserved;
    struct UpdateCpuPreparation {
        UpdateCpuPrefix prefix=UpdateCpuPrefix::none;
        std::uint32_t nextSourcePC=0x1c8ae0;
        std::uint64_t publication=0;
        bool sourceEffectsCommitted=false;
    };
    std::optional<UpdateCpuPreparation> updateCpu;
    std::uint64_t lastUpdateAttemptPublication=0; //Native at-most-once attempt, not full update success.
    //208 is source dirty-layout knownness, NOT a render-disable flag. Its
    // constructor value is unknown;1CA8E0 writes1 only on a mode change.
    std::optional<std::uint32_t> dirtyLayout208;
    struct Slot48CpuPreparation {
        Slot48CpuPrefix prefix=Slot48CpuPrefix::none;
        std::uint32_t nextSourcePC=0x1c17a0;
        bool interrupted=false;
    };
    std::optional<Slot48CpuPreparation> slot48Cpu;
    std::uint64_t lastNativeSlot48AttemptPublication=0;
    std::uint64_t lastNativeDemandAttemptPublication=0;
    struct ModeCpuPreparation {
        ModeCpuPrefix prefix=ModeCpuPrefix::none;
        std::uint32_t nextSourcePC=0x1ca8e0,requestedMode=0,animateArgument=0;
        std::uint32_t actualHumanCount=0;
        bool changed=false,humanCountKnown=false;
    };
    std::optional<ModeCpuPreparation> modeCpu;
    // Native CPU preparation custody, NOT renderer lifetime or source state.
    // Entries point into THIS Impl's retained ScreenObjects/reference ledger.
    // There is intentionally no public span, generic submit callback, backend
    // handle, activation receipt or caller-constructed command surface.
    struct DisplayCpuPreparation {
        struct TextureCall {Texture* object=nullptr;std::uint32_t pc=0;};
        struct TextCall {Text* object=nullptr;std::uint32_t pc=0;};
        struct Transition {float offsetF20=0,offsetF21=0,offsetF31=0,stackD4=641.f;};
        std::array<TextureCall,6> textures{};
        std::array<TextCall,3> titles{};
        std::size_t textureCount=0,titleCount=0;
        // Exact per-row order is place, portrait, name, optional medal, time.
        // These remain THIS Results owner's resources, not copied draw data.
        struct RosterRow {
            TextCall place,name,time;
            TextureCall portrait,medal;
            std::size_t managerIndex=0;
        };
        std::array<TextCall,2> rosterHeaders{};
        std::array<RosterRow,8> rosterRows{};
        std::size_t rosterHeaderCount=0,rosterRowCount=0;
        std::uint64_t rosterPublication=0;
        struct RecordSummary {
            TextCall cheat,title,lapLabel,lapValue,raceLabel,raceValue,knockoutLabel,knockoutValue;
            std::uint64_t publication=0;
            std::size_t managerIndex=0;
            bool previousKeyReset=false,currentKeyColored=false;
        } records;
        struct KeyboardPage {
            std::array<TextCall,42> keys{}; //Reached key callbacks in source index order.
            TextCall name,cursor;
            std::size_t keyCount=0;
            bool selectedKeyScaled=false;
        } keyboard;
        struct CareerWinningsPage {
            TextCall heading;
            std::array<TextCall,12> table{};
            TextureCall medal;
            std::array<TextureCall,2> separators{};
            // Private observations for the following1C6F98 message suffix.
            // No earned-result, Profile mutation, display or scheduler receipt.
            std::uint64_t publication=0,profileGeneration=0;
            std::uint32_t profileMode34=0,courseOrder=0,courseClass=0;
            std::int32_t placeCCC=0,observedCD0=0,prize=0,bonus=0,total=0;
            std::uint32_t retirementCD4=0;
            std::uint8_t priorMedal=0;
            bool reached=false;
        } career;
        struct CareerSummaryPage {
            std::array<TextCall,3> messages{}; //Actual1770/17E0/1850 owners.
            std::array<TextCall,8> statistics{}; //Actual1230..1540 owners.
            TextureCall separator;
            std::array<float,3> lapCC0{};
            float timeCB8=0;
            std::uint8_t progress219C=0;
            bool positionsPrepared=false,qualificationReached=false,qualified=false;
        } careerSummary;
        Transition transition;
        const OriginalRaceConstructor50LA* race=nullptr;
        const OriginalWreckComponent50LA* raceEntity=nullptr;
        std::uint64_t session=0;
        std::uint32_t nextSourcePC=0;
        DisplayCpuPrefix prefix=DisplayCpuPrefix::none;
    };
    std::optional<DisplayCpuPreparation> displayCpu;
    // Concrete native CPU copies in reached source order, never borrowed
    // pointers into mutable Text/Texture/Polygon packets. No public command
    // surface, GPU handle, source queue or completion status is exposed.
    struct NativeDisplayCapture {
        struct Packet {
            std::vector<assets::Vertex48B> vertices; //Four per expanded native quad.
            References::NativePrimitive material;
            std::size_t reference=0;
            std::uint32_t caller=0;
        };
        const std::thread::id thread=std::this_thread::get_id();
        assets::OriginalStandardMaterialCollectorReceipt50LA receipt;
        std::vector<Packet> packets;
        const OriginalRaceConstructor50LA* race=nullptr; //Identity only; private same-call owner.
        std::uint64_t generation=0;
        std::uint32_t stoppedPC=0;
        bool building=false,boundedDerivedReturned=false;
        // Native source-ordered shared CPU provenance only, not Frame/draw
        // admission. The publication belongs to Root timing, not animationClock.
        std::uint64_t sharedRootPublication=0;
        const ::original_camera_session50KX::Session* acceptedCamera=nullptr;
        ::original_ui_source_timing50LH::PublicationView acceptedTiming;
        std::uint64_t acceptedCameraPublication=0,acceptedDisplayRevision=0;
        bool acceptedTextureGate=false,acceptedTextGate=false,acceptedPolygonGate=false;
        std::uint32_t sharedRootTimestamp=0;
        float sharedRaw250=0;
        bool boundedSharedReturned=false;
    };
    std::unique_ptr<NativeDisplayCapture> nativeDisplay;
    std::uint64_t lastNativeSharedAttemptPublication=0;

    // These are native custody envelopes, NOT source null stores. Only5E0
    // is constructor-written null.480/A90/A94/A98 acquire knownness at their
    // individual publication PCs, never from default native backing.
    std::unique_ptr<Text[]> rows5E0;
    std::unique_ptr<Texture[]> portraits480;
    std::unique_ptr<Text> nameA90,cursorA98;
    std::unique_ptr<std::unique_ptr<Text>[]> keysA94;
    std::size_t rowsCount=0,portraitsCount=0,keysConstructed=0;
    bool published5E0=false,published480=false,publishedA90=false,publishedA94=false,publishedA98=false;
    std::optional<std::string> editA9C;
    // Constructor does NOT initialize these source words. Semantic name/key/
    //record lanes gain knownness only at their actual reached source stores.
    //AB8/AB4 alone also admit the separately labeled native first-activation
    //visual bootstrap below; it initializes no other NameEntry field.
    struct NameEntryFields {
        std::optional<std::uint32_t> fieldABC,fieldAC0,fieldAB8,fieldAB4,fieldAD8;
        std::optional<std::int32_t> fieldAC4;
        std::optional<std::uint32_t> lapAC8,raceACC,knockoutAD0,sideAD4;
    };
    std::optional<NameEntryFields> nameEntry;
    enum class CursorBlinkStartProvenance : std::uint8_t {
        notObserved, nativeFirstActivationNameEntryEndpoint,
        preservedKnownPair, preservedPartialUnknown
    };
    // Initial-boundary history only. Actual1C9C60/64 and1CA8B8/BC resets may
    //later replace the values; this never certifies those routines, current
    //name/record readiness, input repeat, a Root update or an acknowledgment.
    CursorBlinkStartProvenance cursorBlinkStartProvenance=CursorBlinkStartProvenance::notObserved;
    struct NameEntryCpuPreparation {
        NameEntryCpuPrefix prefix=NameEntryCpuPrefix::none;
        std::uint32_t nextSourcePC=0x1c9b10;
        std::uint64_t session=0,publication=0,profileGeneration=0;
        std::uint32_t profileMode34=0; //Native same-Profile fence, not source state.
        std::size_t managerIndex=0;
        // Actual timing transport only, NOT earned-destruction ownership.
        bool observedTimingCountOnly=true;
    };
    std::optional<NameEntryCpuPreparation> nameEntryCpu;
    std::optional<std::uint32_t> mode1F0,device1F4;
    struct InputCpuPreparation {
        std::uint64_t pollRevision=0;
        std::uint32_t nextSourcePC=0x1c8c30;
        std::size_t eventsCompleted=0;
        bool sourceEffectsCommitted=false,blocked=false;
    };
    std::optional<InputCpuPreparation> inputCpu;
    std::uint64_t lastInputAttemptPoll=0;
    // Explicit native provenance of the latest queried availability; a bit3
    // skip or zero-human loop does not create a fake device observation.
    std::uint64_t nativeAvailabilityPoll=0;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context;
    std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts;
    std::shared_ptr<const assets::OriginalPauseSourceData50LA> sourceData;
    References references;
    assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
    assets::OriginalApplicationFontDiagnostic50LA fontDiagnostic;
    std::uint32_t depth=0;
    std::size_t extent=0;
    // Native provenance marker from the ACTUAL successful cold load. Weak:
    // neither partial loading nor retained Results extends RA/Bank lifetime.
    // A synchronous callback must lock it and recheck its destruction-bound
    // Bank registration; locking Operation alone does not keep Bank alive.
    std::weak_ptr<ra_cold_roster50LA::Operation> loadedRosterOperation;
    explicit Impl(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    bool detached()const noexcept;
    bool stop(D&,S,std::uint32_t,const char*)noexcept;
    bool texture(Texture&,Role,std::uint32_t,std::uint32_t,std::size_t,std::uint32_t,
        const std::string&,D&);
    bool text(Text&,Role,std::uint32_t,std::uint32_t,std::size_t,std::uint8_t,
        const std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry>&,
        std::string&,const char*,D&);
    bool baseLoad(std::string&,const std::string&,D&);
    bool derivedLoad(original_results_roster50LA::Read&,std::string&,const std::string&,D&);
    bool localized(Text&,std::size_t,D&);
    static void assign(Text&,const char*,std::size_t)noexcept;
    static void color(Text&,std::uint32_t)noexcept;
    static void color(Texture&,std::uint32_t)noexcept;
    static void position(Text&,float,float)noexcept;
    static void position(Texture&,float,float)noexcept;
    static void dimension(Texture&,float&,float)noexcept;
    static void justification(Text&,std::uint32_t)noexcept;
    static void scale(Text&,float)noexcept;
    bool projectTextureCpu2DF694(Texture&,std::uint32_t,D&)noexcept;
    bool projectTextCpu2E0170(Text&,std::uint32_t,D&)noexcept;
    bool copyNativeTexture2DF550(const Texture&,std::uint32_t,D&)noexcept;
    bool copyNativeText2E0170(const Text&,std::uint32_t,D&)noexcept;
    bool copyNativePolygon2E21D0(std::size_t,std::uint32_t,D&)noexcept;
    bool retainNativeOccurrence(std::size_t,std::uint32_t,
        std::vector<assets::Vertex48B>&&,D&)noexcept;
    bool textWidthCpu2E16F0(const Text&,std::int32_t&,D&)noexcept;
    bool prepareTexturesCpu1C21B0(std::uint8_t,std::uint32_t,D&)noexcept;
    bool transitionCpu1C2358(D&)noexcept;
    bool prepareTitlePrefixCpu1C2698(std::uint8_t,std::uint32_t,D&)noexcept;
    void closeText(Text&)noexcept;
    // Private numeric/owned-resource bodies for the eventual concrete native
    // Application/Frame owner. actual250 must be its genuine uncapped lane;
    // no fixed delta, source last-camera admission or callback receipt here.
    bool advanceBaseGeometryCpu1B9324(float actual250,D&)noexcept;
    bool projectPolygonCpu2E2398(std::size_t,std::uint32_t,D&)noexcept;
    void closeTexture(Texture&)noexcept;
    void closePolygon(std::unique_ptr<Polygon>&)noexcept;
    bool closeResources(D&)noexcept;
};
}
