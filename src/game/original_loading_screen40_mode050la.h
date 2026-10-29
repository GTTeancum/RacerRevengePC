#pragma once
#include "original_application_entities50la.h"
#include "../assets/original_application_fonts50la.h"
#include "../assets/original_loading_texture_source50la.h"
#include "../assets/original_loading_font_source50la.h"
#include "../assets/original_loading_ra_file50la.h"
#include "original_loading_screen_layout50la.h"
#include "../../platform/linux/native/original_ui_garage_screen_text50kg.h"
namespace original_main_entry50KG {class OriginalLoadingCollector50LA;class RaceDisplayLifetime50LA;}
#include <array>
namespace rr::game {
// Bounded mode0 resource/Layout prefix owner, not17C320 activation.
// Creation remains private/unreached until the genuine manager pending34 and
// mode54 producer are joined. No manager40 publication or1CC900 capability.
class OriginalLoadingScreen40Mode0LA final {
    friend class OriginalLoadingManager33050LA;
    friend class OriginalLoadingScreenLayout50LA;
    friend class rr::platform::OriginalUiGarageScreenText50KG;
    using App=::original_main_entry50KG::Application;
    using Domain=rr::assets::OriginalStandardMaterialDomain50LA;
    using Context=rr::assets::OriginalStandardMaterialContext50LA;
    using Fonts=rr::assets::OriginalApplicationFonts50LA;
    using Archive=rr::assets::OriginalUiArchive50KG;
    using Resolver=rr::assets::OriginalStandardTextureResolver50LA;
    using Texture=rr::assets::OriginalStandardTextureResource50LA;
    using Text=rr::platform::OriginalUiGarageScreenText50KG;
    enum class Phase {fresh,entity,texture,collector,fonts,loadingTexts,population,bankClose,resources,lsiPreamble,lsiAllocationBoundary,lsiLayout,failed,retired};
    const App* const issuer_;
    const std::shared_ptr<OriginalApplicationEntities50LA> entities_;
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<Fonts> fonts_;
    std::shared_ptr<const Archive> fontArchive_;
    OriginalWreckComponent50LA entity_;
    std::uint32_t vtable_=0,mode30_=0,progress3C_=0;
    const void* layout38_=nullptr; // Actual constructorNULL or reached1CC628 publication.
    std::unique_ptr<OriginalLoadingScreenLayout50LA> layoutAttempt_;
    std::shared_ptr<Texture> texture34_; // Reached returned Resource publication1CAA94 only.
    std::shared_ptr<Texture> raPendingTexture_; // Native caller custody before24DA30 returns.
    std::shared_ptr<rr::assets::OriginalLoadingRaFile50LA> raFile_;
    std::shared_ptr<rr::assets::OriginalLoadingPstFile50LA> raAttempt_;
    std::shared_ptr<const rr::assets::OriginalLoadingTextureSource50LA> looseRa_;
    std::unique_ptr<::original_main_entry50KG::OriginalLoadingCollector50LA> collector_;
    Resolver::LoadingFontsBankLease fontBank_;
    std::shared_ptr<rr::assets::OriginalLoadingFontSource50LA> fontSource_;
    std::array<std::unique_ptr<Text>,11> texts_{};
    std::size_t nextText_=0;
    Phase phase_=Phase::fresh;
    bool enrolled_=false,attempted_=false,retiring_=false;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> lsiDisplay_;
    bool lsiPreambleAttempted_=false,lsiDisplayPinned_=false;
    std::uint8_t lsiDrainPhase_=0;
    bool lsiVideo494ClearReached_=false,lsiDirectoryWriteReached_=false;
    std::uint32_t stopPC_=0x1ca940;
    OriginalLoadingScreen40Mode0LA(const App*,std::shared_ptr<OriginalApplicationEntities50LA>,
        std::shared_ptr<Domain>,std::shared_ptr<const Context>)noexcept;
    bool current()const noexcept;
    bool ownsLoadingTextOccurrence(const Text*,std::size_t)const noexcept;
    std::string& actualMaterialDirectory()const noexcept;
    const std::string& actualFontDirectory()const noexcept;
    bool stageRaResources1CAA20(std::uint32_t&)noexcept;
    // Separate actual1CC580 entry. No fallthrough from1CADA8 and no manager
    // source3C/34/40 or scheduler grant is supplied by this private component.
    bool stageLsiPreamble1CC580(std::uint32_t&)noexcept;
    bool stageLsiLayout1CC610(std::uint32_t&)noexcept;
    const std::optional<std::string>& actualLoadingUiDirectory()const noexcept;
    std::shared_ptr<const Archive> readLoadingIconNative()const;
    bool retireRaNative()noexcept;
    bool retireNativeExclusive()noexcept;
public:
    ~OriginalLoadingScreen40Mode0LA()noexcept;
    OriginalLoadingScreen40Mode0LA(const OriginalLoadingScreen40Mode0LA&)=delete;
    OriginalLoadingScreen40Mode0LA& operator=(const OriginalLoadingScreen40Mode0LA&)=delete;
};
}
