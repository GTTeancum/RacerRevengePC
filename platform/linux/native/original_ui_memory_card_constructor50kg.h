#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include "../../../src/assets/original_memory_card_screen_fallback50kg.h"
#include "original_ui_garage_quad50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/original_ui_entity_scope50kg.h"
namespace original_main_entry50KG {class RaceDisplayLifetime50LA;}
namespace rr::platform {
// Actual detached1FEC80 owner with a private first-X retained resource prefix.
// No page438 publication, complete1FF020 load, attachment, input, pre-render
// or draw admission. First-X population/binding remains private and uncalled.
class OriginalUiMemoryCardConstructor50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class game::OriginalUiEntityScope50KG;
    friend class OriginalUiGarageQuad50KG;
    friend class OriginalUiFrontend50KG;
    friend class assets::OriginalStandardMaterialTable50LA;
    friend class assets::OriginalMemoryCardScreenFallback50KG;
    using Root=::original_main_entry50KG::Application;
    using Scope=game::OriginalUiEntityScope50KG;
    using C=game::OriginalWreckComponent50LA;
    using Quad=OriginalUiGarageQuad50KG;
public:
    ~OriginalUiMemoryCardConstructor50KG()noexcept;
    OriginalUiMemoryCardConstructor50KG(const OriginalUiMemoryCardConstructor50KG&)=delete;
    OriginalUiMemoryCardConstructor50KG& operator=(const OriginalUiMemoryCardConstructor50KG&)=delete;
private:
    enum class Phase : std::uint8_t {empty,bound,detachedConstructed,retiring,retired};
    struct Diagnostic {
        Scope::Result entity{};Quad::Diagnostic quad{};VehicleWindowDiagnostic50DS renderer{};
        std::uint32_t sourcePC=0;bool sourceEffectsCommitted=false,resourcePopulationPending=false,sourceInputsAdmitted=false;
    };
    OriginalUiMemoryCardConstructor50KG()noexcept=default;
    // Root is the only caller, and this factory remains uncalled. It pins the
    // actual existing scope/video/domain/context, not caller-ready scalar flags.
    static bool constructDetached1FEC80(const Root&,
        std::shared_ptr<OriginalUiMemoryCardConstructor50KG>&,Diagnostic&)noexcept;
    bool retireDetached142590(Diagnostic&)noexcept;
    // Component-only prefix, not invocation/completion of containing1FF020.
    // Always stops before first population2DF144; no later role is admitted.
    bool retainFirstIconPrefix1FF058(Diagnostic&)noexcept;
    struct LoadRole {std::size_t constructionIndex;std::uint32_t offset,callPC;const char* qualifiedName;};
    // Source call order, NOT construction order. No later role is admitted yet.
    static constexpr std::array<LoadRole,7> loadRoles_{{
        {3,0x160,0x1FF058,"ui\\textures\\icon_x"},
        {4,0x1C0,0x1FF110,"ui\\textures\\icon_tri"},
        {5,0x220,0x1FF1C8,"ui\\textures\\icon_sqr"},
        {6,0x280,0x1FF280,"ui\\textures\\icon_cir"},
        {0,0x40,0x1FF338,"ui\\textures\\overlay1top"},
        {1,0xA0,0x1FF4AC,"ui\\textures\\overlay1top"},
        {2,0x100,0x1FF68C,"ui\\textures\\overlay1top"}
    }};
    bool firstIconRetained_=false;
    bool resourcePrefixCurrent()const noexcept;
    // Actual same-Root Frontend supplies its own authenticated cold Options
    // bytes. This retains source custody only, not containing page authority.
    bool retainOptionsSources(const std::shared_ptr<const assets::OriginalUiArchive50KG>&)noexcept;
    std::shared_ptr<const assets::OriginalUiArchive50KG> optionsSource_;
    bool prepareFirstIconConstructionFamily(Diagnostic&)noexcept;
    // Private uncalled component step. Completes real global14D370/29E340,
    // stops at2DF158 before numeric resolution/dimensions, packet allocation
    //305500 at2DF1CC/publication2DF1D4, and any later role.
    bool populateFirstIconResources2DF144(Diagnostic&)noexcept;
    bool firstPopulationAttempted_=false,firstPopulationInputsAdmitted_=false;
    std::uint32_t firstPopulationStopPC_=0;
    bool firstIconRetirementIdentity(const assets::OriginalStandardMaterialTable50LA&,std::int32_t&)const noexcept;
    bool pendingFamilyRetired_=false;
    bool pendingFamilyOccurrenceCurrent(const assets::OriginalMemoryCardScreenFallback50KG&)const noexcept;
    bool firstIconOccurrenceCurrent(const assets::OriginalStandardMaterialTable50LA&,std::int32_t&)const noexcept;
    // Read-only component preflight, not permission to call population or
    // claim the containing1FF020 call/attachment occurred.
    bool preflightExistingFirstIconBinding(Diagnostic&)const noexcept;

    bool inputsCurrent()const noexcept;
    bool drainColdQuad2DEFE0(Diagnostic&)noexcept;
    const Root* root_=nullptr;std::shared_ptr<Scope> scope_;
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<::original_main_entry50KG::Video> video_;
    C entity_;
    Phase phase_=Phase::empty;
    std::uint32_t vtable_=0;
    std::array<Quad,7> quads_;
    std::size_t constructedQuads_=0,retiredQuads_=0;
    // Source-initialized fields only; other dialog words remain unrepresented.
    std::array<std::int32_t,9> ids3BC_{};
    std::array<void*,9> dynamicArrays_{};
    std::array<std::uint32_t,9> dynamicCounts_{};
    std::array<void*,9> staticArrays34C_{};
    //370 counts are NOT initialized by1FEC80. Destruction writes them to0.
    std::array<std::optional<std::uint32_t>,9> staticCounts370_{};
    float timer398_=0;bool completed3A4_=false,animate3B0_=false;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display_;
    bool displayPinned_=false;std::uint8_t drainPhase_=0;
};
}
