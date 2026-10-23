#include "original_ui_garage_prompt_material50kg.h"
#include <utility>
namespace rr::assets {
bool OriginalUiGaragePromptFallback50KG::load(std::shared_ptr<const OriginalUiArchive50KG> garage,
    std::shared_ptr<const OriginalUiArchive50KG> app,std::shared_ptr<const OriginalUiArchive50KG> overlays,std::uint8_t ordinal,
    const OriginalUiMaterialRuntime50KG& runtime,std::shared_ptr<const OriginalUiScreenAssets50KG> images,
    std::uint32_t image,OriginalUiGaragePromptDiagnostic50KG& d) noexcept {
    using E=OriginalUiGaragePromptDiagnostic50KG::Error;using F=OriginalUiGsField50KG;
    d={};const auto fail=[&](E e,std::uint32_t pc){d.error=e;d.sourcePC=pc;return false;};
    if(loaded())return fail(E::alreadyLoaded,0x2475C0);
    if(!garage||!garage->loaded()||garage->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage)||
       !app||!app->loaded()||app->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart)||
       !overlays||!overlays->loaded()||overlays->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
       !runtime.overrideKnown||runtime.override435670||!images||!images->loaded())return fail(E::owner,0x2DF10C);
    if(ordinal>=4)return fail(E::source,0x1D682C);
    try {
        constexpr const char* names[]{"ui/textures/icon_x","ui/textures/icon_tri","ui/textures/icon_sqr","ui/textures/icon_cir"};
        constexpr std::uint32_t callers[]{0x1D682C,0x1D68DC,0x1D6994,0x1D6A4C};
        const std::string name=names[ordinal];const auto caller=callers[ordinal];
        const auto& textureOwner=ordinal<2?*overlays:*garage;
        const auto member=textureOwner.find(name+".pst"),page=garage->find("ui/uigarage/garage.dat");
        const auto* provenance=images->sourceProvenance(image);const auto* pixels=images->decodedTexture(image);
        if(!member.identity||!member.data||member.size!=member.identity->bytes||!page.identity||
           garage->find(name+".psm").identity||overlays->find(name+".psm").identity||
           (ordinal<2&&garage->find(name+".pst").identity))return fail(E::source,caller);
        if(!provenance||provenance->memberIdentity!=member.identity||provenance->celIdentity||
           provenance->pstOffset||provenance->pstBytes!=member.size||!pixels||pixels->revision!=1||
           pixels->flags||pixels->kind!=0||pixels->mips.size()!=1)return fail(E::image,0x2DF180);
        const auto size=32u;const auto& mip=pixels->mips[0];
        if(mip.width!=size||mip.height!=size||mip.rgba.size()!=std::size_t(size)*size*4)return fail(E::image,0x24E880);
        OriginalUiMaterialTemplates50KG templates;
        if(!templates.load(*app,d.material))return fail(E::templateState,0x2DF10C);
        const auto* base=templates.get(OriginalUiTemplate50KG::screenMt);
        if(!base||base->identity!=app->find("AppStart/ScreenMT.psm").identity||base->microprogram!="ScreenObject")
            return fail(E::templateState,0x2475C0);
        // Read the canonical template, then the source247C88 context override.
        // Exact35-field guard covers dormant test/alpha fields as well. Values
        // come from24A8E0 + authenticated ScreenMT; no visual substitute pass.
        auto gs=base->gs;gs.fields[std::size_t(F::environmentContext0or1)]={0,nullptr,0,0x247C88};
        constexpr std::int32_t expected[]{1,0,255,255,0,0,1,0,1,128,1,1,0,0,0,0,1,0,0,1,1,0,1,0,0,0,0,0,6,0,0,0,1,2,0};
        static_assert(std::size(expected)==originalUiGsFieldCount50KG);
        for(std::size_t i=0;i<std::size(expected);++i)if(gs.fields[i].value!=expected[i])return fail(E::templateState,gs.fields[i].sourcePC);
        OriginalUiNativeMaterial50KG policy;
        policy.blend=OriginalUiMaterialBlend50KG::sourceAlpha;
        //247F38..247FE8 defaults MMAG1/MMIN5,MXL0,LCM0,L0,K-75.
        policy.minification=policy.magnification=OriginalUiMaterialFilter50KG::linear;
        policy.u=policy.v=OriginalUiMaterialWrap50KG::clampToEdge;
        policy.depthMethod=2;policy.depthWrite=true;policy.environmentContext=0;
        policy.requiresOriginalDepthOwner=true;policy.requiresSeparateAlphaStore=true;
        garage_=std::move(garage);appStart_=std::move(app);overlays_=std::move(overlays);templates_=std::move(templates);
        gs_=gs;policy_=policy;texture_=member.identity;page_=page.identity;image_=image;
        caller_=caller;images_=std::move(images);return true;
    }catch(...){return fail(E::allocation,0x2475C0);}
}
}
