#include "original_ui_material_binding50kg.h"
#include <utility>

namespace rr::assets {
namespace {
using E=OriginalUiMaterialBindingError50KG;
using D=OriginalUiMaterialBindingDiagnostic50KG;
bool fail(D& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
bool dimension(std::uint32_t n) noexcept {return n>=16&&n<=1024&&!(n&(n-1));}
}
bool OriginalUiMaterialBindingState50KG::initialize(const OriginalUiMaterialEquivalenceKey50KG& key,D& d) noexcept {
    d={};if(initialized_)return fail(d,E::alreadyInitialized,0x29bea0);
    if(!key.prepared_)return fail(d,E::key,0x249ba0);
    try {
        OriginalUiMaterialBindingState50KG next;
        next.construction_=key;next.effective_=key;next.initialized_=true;
        *this=std::move(next);return true;
    }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiMaterialBindingState50KG::recordDrainedBinding(const OriginalUiImageBank50KG& bank,
    std::uint32_t index,D& d) noexcept {
    d={};d.imageIndex=index;
    if(!initialized_)return fail(d,E::uninitialized,0x249ba0);
    if(construction_.mapCount_!=1)return fail(d,E::untextured,0x24b1f0);
    if(!construction_.enabled_)return fail(d,E::disabled,0x24b1b0);
    if(!bank.loaded())return fail(d,E::bank,0x24b254);
    try {
        const auto images=bank.images();
        const auto* source=images?images->sourceProvenance(index):nullptr;
        const auto* texture=images?images->decodedTexture(index):nullptr;
        const auto* textureBank=images?images->textures():nullptr;
        const auto* published=textureBank?textureBank->texture(index):nullptr;
        if(!images||!images->loaded()||!source||!texture||!published)
            return fail(d,E::image,0x24b254);
        if(source->memberIdentity!=construction_.texture_||!source->memberIdentity||
            !source->pstBytes||source->pstOffset>source->memberIdentity->bytes||
            source->pstBytes>source->memberIdentity->bytes-source->pstOffset)
            return fail(d,E::provenance,0x2472f8);
        if(texture->revision!=1||texture->flags||texture->kind>2||
            texture->mips.empty()||texture->mips.size()>7)
            return fail(d,E::policy,0x24e790);
        const auto& base=texture->mips.back();
        if(!dimension(base.width)||!dimension(base.height)||
            std::uint64_t(base.width)*base.height*4!=base.rgba.size()||
            published->texture.width!=base.width||published->texture.height!=base.height||
            published->texture.rgba!=base.rgba)
            return fail(d,E::policy,0x249780);
        OriginalUiIpumColorPolicy50KG ipumPolicy=OriginalUiIpumColorPolicy50KG::unavailable;
        if(texture->kind==2){
            const auto* ipum=images->ipumProvenance(index);
            if(!ipum||ipum->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1||
                source->pstOffset||source->pstBytes!=source->memberIdentity->bytes||
                ipum->sourcePstSha256!=source->memberIdentity->sha256||
                texture->mips.size()!=originalUiIpumMipCount50KG)
                return fail(d,E::policy,0x249818);
            ipumPolicy=ipum->colorPolicyKind;
        }else if(images->ipumProvenance(index))return fail(d,E::policy,0x249818);
        // Stage all potentially allocating copies before publication. Resetting
        // effective from construction is safe only for this same immutable
        // resource/kind policy; source image mutation/replacement is not admitted.
        auto next=*this;
        next.effective_=construction_;
        if(texture->kind==2){
            // Actual one-map249818..24983C overwrites only TEXA. Do not compare
            // authored values that the binding no longer retains. Indexed PST
            // formats keep all three constructor values, even when inactive.
            using F=OriginalUiGsField50KG;
            next.effective_.state_[static_cast<std::size_t>(F::sourceAlphaValueWhenAis0)]=255;
            next.effective_.state_[static_cast<std::size_t>(F::sourceAlphaValueWhenAis1)]=255;
            next.effective_.state_[static_cast<std::size_t>(F::methodOfExpandingTextureAlpha)]=1;
        }
        next.member_=source->memberIdentity;next.pstOffset_=source->pstOffset;next.pstBytes_=source->pstBytes;
        next.storedMip_=texture->mips.size()-1;next.mipHeader_=base.source_header_offset;
        next.mipPayload_=base.source_indices_offset;next.width_=base.width;next.height_=base.height;
        next.kind_=texture->kind;next.ipumPolicy_=ipumPolicy;next.bound_=true;
        *this=std::move(next);return true;
    }catch(...){return fail(d,E::allocation,0);}
}
OriginalUiMaterialEquivalence50KG OriginalUiMaterialBindingState50KG::compare(
    const OriginalUiMaterialBindingState50KG& other) const noexcept {
    using R=OriginalUiMaterialEquivalence50KG;
    if(!initialized_||!other.initialized_)return R::unsupported;
    if(effective_.excludesMerging()||other.effective_.excludesMerging())return R::notEquivalent;
    // Cold TEX0 has only TCC/TFX. A valid16..1024 base image has nonzero
    // TW/TH in disjoint fields (249780..2497F4), so cold/bound cannot compare
    // equal. This is native state history, not an emulated texture packet.
    if(bound_!=other.bound_)return R::notEquivalent;
    if(bound_&&(member_!=other.member_||pstOffset_!=other.pstOffset_||pstBytes_!=other.pstBytes_||
        storedMip_!=other.storedMip_||mipHeader_!=other.mipHeader_||mipPayload_!=other.mipPayload_||
        width_!=other.width_||height_!=other.height_||kind_!=other.kind_||ipumPolicy_!=other.ipumPolicy_))
        return R::notEquivalent;
    // Same bound host image supplies the same immutable dimensions/format and
    // palette interpretation; historical device addresses are not represented.
    // All remaining GS/TEX1/program/resource/UV/flags/group semantics still use
    // the original249BA0 rules, including fields inactive in current rendering.
    return compareOriginalUiMaterialEquivalence50KG(effective_,other.effective_);
}
}
