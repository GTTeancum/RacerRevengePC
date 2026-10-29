#pragma once
#include "original_ui_garage_quad50kg.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_garage_page_owner50kg.h"
#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <thread>
namespace original_main_entry50KG {struct Application;class RaceDisplayLifetime50LA;}
namespace rr::platform {
class OriginalUiCourseOwnedLifecycle50KG;
// Private embedded backing only. The enclosing authentic Course constructor
// owns the ONE counted 141300 Page; this class cannot issue/attach an Entity.
// Never graft onto an already-live legacy Course session or certify its past.
class OriginalUiCoursePagePayload50KG final {
    friend class OriginalUiCourseOwnedLifecycle50KG;
    friend class OriginalUiGarageQuad50KG;
    using Root=::original_main_entry50KG::Application;
    using Display=::original_main_entry50KG::RaceDisplayLifetime50LA;
    using Quad=OriginalUiGarageQuad50KG;
    using Text=OriginalUiGarageScreenText50KG;
    struct Diagnostic {
        std::uint32_t sourcePC=0;
        bool sourceEffectsCommitted=false;
        Quad::Diagnostic quad{};
        OriginalUiGarageScreenTextDiagnostic50KG text{};
        VehicleWindowDiagnostic50DS renderer{};
    };
    enum class Phase:std::uint8_t {empty,bound,shared,derived,retiringDerived,beforeUnknownApertures,beforeBase,nativeAborting,nativeRetired};
    struct TextStorage {alignas(Text) std::array<std::byte,sizeof(Text)> bytes;Text* value=nullptr;};
    OriginalUiCoursePagePayload50KG()noexcept=default;
    OriginalUiCoursePagePayload50KG(const OriginalUiCoursePagePayload50KG&)=delete;
    OriginalUiCoursePagePayload50KG& operator=(const OriginalUiCoursePagePayload50KG&)=delete;
    ~OriginalUiCoursePagePayload50KG()noexcept;
    // Native custody only, before the enclosing source count. A failed bind
    // has no effects; the actual aggregate must keep this pin until cleanup.
    bool bindNativeDisplay(const Root&,std::shared_ptr<Display>,Diagnostic&)noexcept;
    bool displayCurrent()const noexcept;
    // Enclosing caller must first execute its real2A5C70 counted base stores.
    bool constructShared1D5EA0(Diagnostic&)noexcept;
    bool constructDerived1D8D70(Diagnostic&)noexcept;
    // Stops at20EACC for any unpublished424/428; after actual aperture and
    // shared-payload retirement stops at2A5CF0, before the counted base.
    bool retireDerived20E990(Diagnostic&)noexcept;
    // Explicit native rollback of this unexposed component backing, never
    // source20E990/142590 completion or an enclosing loader receipt.
    bool nativeAbortCold(Diagnostic&)noexcept;
    // Private component sequence only. No receipt for preceding1D63B0 loader,
    // page table, text load, camera, enclosing loader return or draw readiness.
    bool loadApertureComponent1D66C0(std::size_t,Diagnostic&)noexcept;
    bool apertureComponentCurrent(std::size_t)const noexcept;
    bool loadAperture2E1AE0(std::size_t,Diagnostic&)noexcept;
    bool retireAperture2E1980(std::size_t,Diagnostic&)noexcept;
    bool drainPolygon(std::uint8_t&,Diagnostic&)noexcept;
    using PolygonOwner=OriginalUiGaragePageOwner50KG;
    using Aperture=PolygonOwner::Aperture; //Canonical backing, not Garage authority.
    enum class AperturePhase:std::uint8_t {absent,published,loading,loaded,failed,retired};
    std::array<std::unique_ptr<Aperture>,2> apertures_;
    std::array<AperturePhase,2> aperturePhases_{};
    std::array<std::uint32_t,2> apertureStopPC_{{0x1D66C0,0x1D66C0}};
    std::array<std::uint8_t,2> apertureDrains_{};
    // Exact failed native retain custody, not a source material flag. Only
    // nativeAbortCold may dispose this stored-but-unacquired Primitive.
    std::array<Quad::Primitive*,2> failedRetainPrimitive_{};
    std::array<std::optional<std::int32_t>,2> failedRetainRows_{};
    // Exact native cleanup custody for an actually stored prompt row whose
    // retain failed. Never an exception to normal source Quad retirement.
    std::array<Quad::Primitive*,4> failedPromptRetainPrimitive_{};
    std::array<std::optional<std::int32_t>,4> failedPromptRetainRows_{};
    std::size_t retiredApertures_=0;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> apertureDomain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> apertureContext_;
    void constructQuad(std::uint32_t)noexcept;
    void constructText()noexcept;
    bool retireLastText(Diagnostic&)noexcept;
    bool retireLastQuad(Diagnostic&)noexcept;
    bool retireDerivedPayload(Diagnostic&)noexcept;
    bool stop(Diagnostic&,std::uint32_t)const noexcept;
    const std::thread::id thread_=std::this_thread::get_id();
    const Root* root_=nullptr;
    std::shared_ptr<Display> display_;
    bool pinned_=false;
    Phase phase_=Phase::empty;
    std::uint32_t vtable_=0;
    std::array<Quad,20> quads_;
    std::array<TextStorage,24> texts_;
    std::size_t quadCount_=0,textCount_=0;
    struct SharedFields {
        // Actual1D63B0 loader writes, distinct from cold constructor fields.
        std::optional<const void*> manager130,manager270;
        std::array<char,256> name30{};bool name30Known=false;
        std::optional<std::int32_t> closed284,closed28C,state4B4;
        std::optional<std::uint32_t> backgroundColor430; // Actual1D6D70 only.
        std::optional<float> speed4B8; // Actual1D6808 only, not constructor zero.

        std::int32_t openSize274=0,top27C=0,left288=0,left278=0,right290=0,right280=0,bottom294=0;
        std::int32_t savedDelta298=0,field420=0,field42C=0,field434=0,input268=0;
        float time4B0=0;
        const void* popup438=nullptr;const void* camera4BC=nullptr;
        // Absent means never written, NOT proven source NULL.
        std::array<std::optional<const void*>,2> apertures424;
    } shared_;
    std::array<std::uint32_t,5> zero14D0_{};
    std::optional<std::int32_t> field14FC_;
};
}
