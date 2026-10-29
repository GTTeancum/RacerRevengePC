#pragma once
#include "original_ui_garage_quad50kg.h"
#include "original_ui_garage_screen_text50kg.h"
#include "original_ui_overlay_text_storage50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/original_wreck_components50la.h"
#include <array>
#include <memory>
#include <optional>
#include <thread>
namespace original_main_entry50KG {class RaceDisplayLifetime50LA;}
namespace rr::platform {
class OriginalUiCourseOwnedLifecycle50KG;
// Actual Course page438 payload, never the detached Options/Root singleton.
// Enclosing Course Scope owns the sole counted Entity and nullable publication.
class OriginalUiCoursePopupPayload50KG final {
    friend class OriginalUiCourseOwnedLifecycle50KG;
    friend class OriginalUiGarageScreenText50KG;
    friend class OriginalUiGarageQuad50KG;
    using Root=::original_main_entry50KG::Application;
    using Display=::original_main_entry50KG::RaceDisplayLifetime50LA;
    using C=game::OriginalWreckComponent50LA;
    using Quad=OriginalUiGarageQuad50KG;
    using Text=OriginalUiGarageScreenText50KG;
    using Font=assets::OriginalApplicationFonts50LA::Entry;
    struct SourceLine {
        const OriginalUiOverlayTextFields50KG* outer=nullptr;
        const OriginalUiTextOverlayFields50KG* line=nullptr;
        const C* outerEntity=nullptr;const C* lineEntity=nullptr;
        const Text* screen=nullptr;std::shared_ptr<const Font> font;
        std::size_t row=84,lineOrdinal=10;
    };
    struct Diagnostic {
        std::uint32_t sourcePC=0;bool sourceEffectsCommitted=false;
        Quad::Diagnostic quad{};OriginalUiGarageScreenTextDiagnostic50KG text{};
        VehicleWindowDiagnostic50DS renderer{};
    };
    enum class Phase:std::uint8_t {empty,bound,constructed,loading,loaded,nativeAborting,nativeRetired};
    OriginalUiCoursePopupPayload50KG()noexcept=default;
    ~OriginalUiCoursePopupPayload50KG()noexcept;
    OriginalUiCoursePopupPayload50KG(const OriginalUiCoursePopupPayload50KG&)=delete;
    OriginalUiCoursePopupPayload50KG& operator=(const OriginalUiCoursePopupPayload50KG&)=delete;
    // Native cleanup pin admission before the actual counted allocation interval.
    bool bindNative(OriginalUiCourseOwnedLifecycle50KG&,C&,std::shared_ptr<Display>,Diagnostic&)noexcept;
    // Called only after authentic Scope141300; contains no second count.
    bool construct1FEC80AfterCount(Diagnostic&)noexcept;
    // Source parent394 publication and seven original Quad calls precede
    // source328 template IDs,28 copied ScreenTexts and final flags34|=3.
    bool load1FF020(C& manager,const std::array<std::int32_t,9>&,Diagnostic&)noexcept;
    bool preflightNativeAbort(Diagnostic&)const noexcept;
    bool nativeAbortResources(Diagnostic&)noexcept; // Parent retires count afterward.
    bool current(bool retiring=false)const noexcept;
    bool loadQuadRole(std::size_t,Diagnostic&)noexcept;
    bool loadClones2037D0(Diagnostic&)noexcept;
    bool drainQuad(Quad&,Diagnostic&)noexcept;
    bool stop(Diagnostic&,std::uint32_t)noexcept;
    bool ownsClone(const Text*,std::size_t&,std::size_t&)const noexcept;
    bool cloneInputs(const Text*,std::int32_t&,std::shared_ptr<const Font>&)const noexcept;
    static constexpr std::array<std::int32_t,9> sourceWidgetIds_{{68,69,70,71,72,73,74,75,76}};
    static constexpr std::array<std::size_t,9> sourceLineCounts_{{1,7,1,1,1,1,3,10,3}};
    struct Clone {
        alignas(Text) std::array<std::byte,sizeof(Text)> storage;
        Text* text=nullptr;std::shared_ptr<const Font> font;
        std::int32_t capacity=0;bool loaded=false,retired=false;
    };
    struct CloneRow {
        std::unique_ptr<Clone[]> backing;
        std::optional<const void*> sourceArray34C;
        std::optional<std::uint32_t> sourceCount370;
        std::size_t constructed=0,completed=0,retired=0;
        bool allocationAttempted=false,arrayFreed=false;
    };
    const std::thread::id thread_=std::this_thread::get_id();
    OriginalUiCourseOwnedLifecycle50KG* owner_=nullptr;const Root* root_=nullptr;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> course_;
    C* entity_=nullptr;C* manager394_=nullptr;bool manager394Known_=false;
    std::shared_ptr<Display> display_;bool displayPinned_=false;
    Phase phase_=Phase::empty;std::uint32_t vtable_=0,stopPC_=0;
    std::array<Quad,7> quads_;std::size_t constructedQuads_=0,retiredQuads_=0;
    std::array<CloneRow,9> clones_;
    std::shared_ptr<const int> occurrence_;
    std::array<Quad::Primitive*,7> failedRetains_{};
    std::array<std::int32_t,7> failedRetainRows_{{-1,-1,-1,-1,-1,-1,-1}};
    std::size_t loadedRoles_=0;bool loadAttempted_=false,rendererStoreReached_=false;
    std::uint32_t flags34KnownMask_=0; // Upper source bits genuinely unproduced.

    std::array<std::int32_t,9> ids3BC_{};
    std::optional<std::array<std::int32_t,9>> templateIds328_;
    std::array<const void*,9> dynamicArrays_{};std::array<std::uint32_t,9> dynamicCounts_{};
    std::uint32_t flags34_=0;float timer398_=0;bool completed3A4_=false,animate3B0_=false;
};
}
