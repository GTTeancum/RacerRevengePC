#include "original_ui_memory_card_resources50kg.h"
#include "original_ui_options_draw_resources50kg.h"
namespace rr::platform {
bool OriginalUiMemoryCardResources50KG::prepare(const game::OriginalUiOptionsMenuWidgets50KG& widgets,
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,
    const std::vector<OriginalUiOptionsMaterialBinding50KG>& materials,OriginalUiMemoryCardResourceDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiMemoryCardResourceDiagnostic50KG::Error;
    if(loaded()){d.error=E::alreadyPrepared;return false;}
    if(!images||!images->loaded()||!widgets.memoryCardPage()){d.error=E::identity;return false;}
    // Actual shared registry objects, not new fallback materials claiming a
    // memcard origin. Menu/config construction provenance remains untouched.
    constexpr std::array<std::uint32_t,6> tokens{{3176,3180,3214,3215,3216,3217}};
    constexpr std::array<const char*,6> names{{"ui/textures/overlay1top","font/denmrk16top",
        "ui/textures/icon_cir","ui/textures/icon_sqr","ui/textures/icon_tri","ui/textures/icon_x"}};
    std::array<std::uint32_t,6> found;found.fill(assets::unresolvedMaterialTexture50EA);
    for(std::size_t i=0;i<materials.size();++i)for(std::size_t j=0;j<tokens.size();++j)if(materials[i].registryToken==tokens[j]){
        const auto& b=materials[i];d.registryToken=tokens[j];
        if(found[j]!=assets::unresolvedMaterialTexture50EA||b.registeredName!=names[j]||
            b.registryIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::appStart,0)||
            b.material.passes.size()!=1||b.images.size()!=1||b.animated){d.error=E::material;return false;}
        found[j]=static_cast<std::uint32_t>(i);
    }
    for(auto i:found)if(i==assets::unresolvedMaterialTexture50EA){d.error=E::material;return false;}
    const auto& overlay=materials[found[0]];
    if(overlay.material.identity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::options,47)){
        d.error=E::identity;d.sourcePC=0x1FF338;return false;}
    if(!geometry_.prepare(widgets,*images,materials[found[1]].images[0],overlay.images[0],d.geometry)){
        d.error=E::geometry;d.sourcePC=d.geometry.sourcePC;return false;}
    images_=std::move(images);overlayMaterial_=found[0];d={};return true;
}
bool OriginalUiMemoryCardResources50KG::project(const game::OriginalUiMemoryCardRender50KG& r,
    OriginalUiMemoryCardResourceDiagnostic50KG& d) noexcept {
    d={};if(!loaded()){d.error=OriginalUiMemoryCardResourceDiagnostic50KG::Error::identity;return false;}
    if(!geometry_.project(r,d.geometry)){d.error=OriginalUiMemoryCardResourceDiagnostic50KG::Error::geometry;
        d.sourcePC=d.geometry.sourcePC;return false;}
    return true;
}
}
