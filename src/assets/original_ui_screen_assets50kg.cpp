#include "original_ui_course_background50kg.h"
#include "original_ui_hall_background50kg.h"
#include "original_ui_unlock_background50pd.h"
#include "original_ui_controls_specmap50kg.h"
#include "original_ui_screen_assets50kg.h"
#include "original_ui_image_bank50kg.h" // OriginalUiImageBankAddonLimits50DL (one place)
#include <new>
#include <string>
#include <utility>

namespace rr::assets {
namespace {
bool fail(OriginalUiScreenAssetDiagnostic50KG& d,OriginalUiScreenAssetError50KG error,std::size_t i=0) noexcept {
    d.error=error;d.image=i;return false;
}
char folded(char c) noexcept { return c=='\\'?'/' : c>='A'&&c<='Z'?char(c+32):c; }
bool fontPair(const char* atlas,const char* cel) {
    if(!atlas||!cel)return false;
    const std::string a(atlas),c(cel);
    if(a.size()<4||a.size()!=c.size())return false;
    for(std::size_t i=0;i<a.size()-4;++i)if(folded(a[i])!=folded(c[i]))return false;
    const auto suffix=[&](const std::string& value,const char* ending){
        for(unsigned i=0;i<4;++i)if(folded(value[value.size()-4+i])!=ending[i])return false;
        return true;
    };
    return suffix(a,".pst")&&suffix(c,".cel");
}
}
struct OriginalUiScreenAssets50KG::Storage {
    MaterialTextureAssets50EA bank;
    // New reconstruction: retain the original decoder's complete result in
    // bank input order. RRTEX/Texture48B alone cannot prove source kind,
    // mip count or original palette. No source fields or alpha are rewritten.
    std::vector<Texture48E> decodedTextures;
    std::vector<OriginalUiFont50KG> fonts;
    std::vector<OriginalUiImageProvenance50KG> provenance;
    std::vector<OriginalUiIpumProvenance50KG> ipumEvidence;
};
OriginalUiScreenAssets50KG::OriginalUiScreenAssets50KG() noexcept=default;
OriginalUiScreenAssets50KG::~OriginalUiScreenAssets50KG()=default;
bool OriginalUiScreenAssets50KG::load(const std::vector<OriginalUiImageSource50KG>& sources,
    OriginalUiScreenAssetDiagnostic50KG& d,const OriginalUiIpumTextureCache50KG* ipumCache) noexcept {
    d={};using Error=OriginalUiScreenAssetError50KG;
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        // The stock owner capacity is 256 images. PORT EXTENSION, NOT ORIGINAL (DLC):
        // images whose member belongs to an add-on package archive take their own
        // capacity (OriginalUiImageBankAddonLimits50DL::sources) on top of it.
        // Without add-on images this is exactly the stock 256 check.
        std::size_t addonSources=0;
        for(const auto& s:sources)if(originalUiAddonMemberArchive50DL(s.member.identity))++addonSources;
        if(sources.empty()||sources.size()-addonSources>256||
           addonSources>OriginalUiImageBankAddonLimits50DL::sources)return fail(d,Error::sources);
        // Validate all ranges and font association before cold allocations.
        for(std::size_t i=0;i<sources.size();++i){
            const auto& s=sources[i];
            if(!s.member.identity||!s.member.data||s.member.size!=s.member.identity->bytes||
                !s.pstBytes||s.pstOffset>s.member.size||s.pstBytes>s.member.size-s.pstOffset||
                s.pstBytes>8u*1024u*1024u)return fail(d,Error::sources,i);
            if(s.cel.identity){
                if(!s.cel.data||s.cel.size!=s.cel.identity->bytes||s.cel.size>maxOriginalUiCelBytes50KG||
                    s.pstOffset||s.pstBytes!=s.member.size||!fontPair(s.member.identity->name,s.cel.identity->name))
                    return fail(d,Error::sources,i);
            }else if(s.cel.data||s.cel.size)return fail(d,Error::sources,i);
        }
        auto next=std::make_unique<Storage>();next->fonts.resize(sources.size());
        // Size once before taking any base_rrtex addresses. These results and
        // the bank stay private until all decoding and bank admission succeed.
        next->decodedTextures.resize(sources.size());
        next->provenance.resize(sources.size());
        next->ipumEvidence.resize(sources.size());
        std::vector<MaterialTextureAssetInput50EA> images(sources.size());
        std::size_t totalSource=0,totalRgba=0;
        for(std::size_t i=0;i<sources.size();++i){
            const auto& s=sources[i];
            next->provenance[i]={s.member.identity,s.cel.identity,s.pstOffset,s.pstBytes};
            const std::vector<std::uint8_t> pst(s.member.data+s.pstOffset,s.member.data+s.pstOffset+s.pstBytes);
            auto& decoded=next->decodedTextures[i];
            const auto* cached=ipumCache?ipumCache->textureFor(s.member,s.pstOffset,s.pstBytes):nullptr;
            if(cached){
                const auto* evidence=ipumCache->provenanceFor(s.member,s.pstOffset,s.pstBytes);
                if(!evidence||cached->kind!=2)return fail(d,Error::texture,i);
                decoded=*cached;next->ipumEvidence[i]=*evidence;
            }else if(!s.cel.identity&&!s.pstOffset&&s.pstBytes==s.member.size&&
                isOriginalUiCourseBackground50KG(s.member)){
                if(!decodeOriginalUiCourseBackground50KG(s.member,decoded,d.texture))return fail(d,Error::texture,i);
            }else if(!s.cel.identity&&!s.pstOffset&&s.pstBytes==s.member.size&&
                isOriginalUiHallBackground50KG(s.member)){
                if(!decodeOriginalUiHallBackground50KG(s.member,decoded,d.texture))return fail(d,Error::texture,i);
            }else if(!s.cel.identity&&!s.pstOffset&&s.pstBytes==s.member.size&&
                isOriginalUiUnlockBackground50PD(s.member)){
                if(!decodeOriginalUiUnlockBackground50PD(s.member,decoded,d.texture))return fail(d,Error::texture,i);
            }else if(!s.cel.identity&&!s.pstOffset&&s.pstBytes==s.member.size&&isOriginalUiControlsSpecmap50KG(s.member.identity)){
                if(!decodeOriginalUiControlsSpecmap50KG(s.member,decoded,d.texture))return fail(d,Error::texture,i);
            }else if(!decodeTexture48E(pst,decoded,d.texture))return fail(d,Error::texture,i);
            if(decoded.mips.empty())return fail(d,Error::texture,i);
            const auto& base=decoded.mips.back();
            if(base.rgba.size()>maxMaterialTextureRGBABytes50EA-totalRgba||
                decoded.base_rrtex.size()>maxMaterialTextureSourceBytes50EA-totalSource)
                return fail(d,Error::sources,i);
            totalRgba+=base.rgba.size();totalSource+=decoded.base_rrtex.size();
            images[i]={s.identity,&decoded.base_rrtex};
            if(s.cel.identity&&!decodeOriginalUiCel50KG(s.cel.data,s.cel.size,next->fonts[i],d.font))
                return fail(d,Error::font,i);
        }
        const auto count=static_cast<std::uint32_t>(sources.size());
        MaterialTextureAssetsInput50EA input{images.data(),count,nullptr,0,
            {count,0,totalSource,totalRgba,1024}};
        if(!next->bank.load(input,d.bank))return fail(d,Error::bank);
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiScreenAssets50KG::loaded() const noexcept{return bool(storage_);}
const MaterialTextureAssets50EA* OriginalUiScreenAssets50KG::textures() const noexcept{
    return storage_?&storage_->bank:nullptr;
}
const Texture48E* OriginalUiScreenAssets50KG::decodedTexture(std::uint32_t i) const noexcept{
    return storage_&&i<storage_->decodedTextures.size()?&storage_->decodedTextures[i]:nullptr;
}
const OriginalUiImageProvenance50KG* OriginalUiScreenAssets50KG::sourceProvenance(std::uint32_t i) const noexcept{
    return storage_&&i<storage_->provenance.size()?&storage_->provenance[i]:nullptr;
}
const OriginalUiIpumProvenance50KG* OriginalUiScreenAssets50KG::ipumProvenance(std::uint32_t i) const noexcept{
    return storage_&&i<storage_->decodedTextures.size()&&storage_->decodedTextures[i].kind==2?
        &storage_->ipumEvidence[i]:nullptr;
}
const OriginalUiFont50KG* OriginalUiScreenAssets50KG::font(std::uint32_t i) const noexcept{
    return storage_&&i<storage_->fonts.size()&&storage_->fonts[i].decoded()?&storage_->fonts[i]:nullptr;
}
}
