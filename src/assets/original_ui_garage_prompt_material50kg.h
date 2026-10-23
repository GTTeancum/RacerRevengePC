#pragma once
#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
namespace rr::assets {
struct OriginalUiGaragePromptDiagnostic50KG {
    enum class Error:std::uint8_t {none,alreadyLoaded,owner,source,templateState,image,allocation} error=Error::none;
    std::uint32_t sourcePC=0;OriginalUiMaterialDiagnostic50KG material{};
};
// Authenticated Garage base-constructor prompt2475C0, separate from every other
// page fallback issuer. No fabricated PSM/pass origin and no generic admission.
class OriginalUiGaragePromptFallback50KG final {
public:
    bool load(std::shared_ptr<const OriginalUiArchive50KG> garage,
        std::shared_ptr<const OriginalUiArchive50KG> appStart,
        std::shared_ptr<const OriginalUiArchive50KG> overlays,std::uint8_t ordinal,
        const OriginalUiMaterialRuntime50KG&,std::shared_ptr<const OriginalUiScreenAssets50KG>,
        std::uint32_t image,OriginalUiGaragePromptDiagnostic50KG&) noexcept;
    bool loaded()const noexcept{return bool(images_);}
    const OriginalUiMaterialTemplateState50KG* templateOwner()const noexcept{return templates_.get(OriginalUiTemplate50KG::screenMt);}
    const OriginalUiGsState50KG* effectiveState()const noexcept{return loaded()?&gs_:nullptr;}
    const OriginalUiNativeMaterial50KG* policy()const noexcept{return loaded()?&policy_:nullptr;}
    const OriginalUiMemberIdentity50KG* textureIdentity()const noexcept{return texture_;}
    const OriginalUiMemberIdentity50KG* pageIdentity()const noexcept{return page_;}
    std::uint32_t sourceCallerPC()const noexcept{return caller_;}
    std::uint32_t creationPC()const noexcept{return loaded()?0x2475C0:0;}
    const OriginalUiScreenAssets50KG* images()const noexcept{return images_.get();}
    std::uint32_t imageIndex()const noexcept{return image_;}
private:
    std::shared_ptr<const OriginalUiArchive50KG> garage_,appStart_,overlays_;
    OriginalUiMaterialTemplates50KG templates_;std::shared_ptr<const OriginalUiScreenAssets50KG> images_;
    OriginalUiGsState50KG gs_{};OriginalUiNativeMaterial50KG policy_{};
    const OriginalUiMemberIdentity50KG *texture_=nullptr,*page_=nullptr;
    std::uint32_t image_=0,caller_=0;
};
}
