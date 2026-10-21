#include "original_ui_material_equivalence50kg.h"
#include "original_ui_controls_specmap50kg.h"
#include <utility>

namespace rr::assets {
namespace {
using E=OriginalUiMaterialEquivalenceError50KG;
using D=OriginalUiMaterialEquivalenceDiagnostic50KG;
char fold(char c) noexcept {return c>='A'&&c<='Z'?char(c+('a'-'A')):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool fail(D& d,E e,const char* field,std::uint32_t pc) noexcept {
    d.error=e;d.field=field;d.sourcePC=pc;return false;
}
bool ascii(const std::string& value,std::size_t limit) noexcept {
    if(value.size()>limit)return false;
    for(unsigned char c:value)if(c<32||c>=127)return false;
    return true;
}
const OriginalUiArchiveIdentity50KG* memberArchive(const OriginalUiMemberIdentity50KG* member) noexcept {
    if(!member)return nullptr;
    constexpr OriginalUiArchiveId50KG ids[]={OriginalUiArchiveId50KG::appStart,
        OriginalUiArchiveId50KG::localization,OriginalUiArchiveId50KG::overlays,
        OriginalUiArchiveId50KG::fonts,OriginalUiArchiveId50KG::mainMenu,
        OriginalUiArchiveId50KG::backgrounds,OriginalUiArchiveId50KG::controllerPopup,
        OriginalUiArchiveId50KG::course,OriginalUiArchiveId50KG::rider,
        OriginalUiArchiveId50KG::options,OriginalUiArchiveId50KG::podTextures,
        OriginalUiArchiveId50KG::hallOfFame,OriginalUiArchiveId50KG::tournament};
    for(auto id:ids){const auto* a=originalUiArchiveIdentity50KG(id);if(!a)continue;
        for(std::size_t i=0;i<a->memberCount;++i)if(originalUiMemberIdentity50KG(id,i)==member)return a;
    }
    return nullptr;
}
bool knownField(const std::string& key) noexcept {
    for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i)
        if(same(key,originalUiGsFieldName50KG(static_cast<OriginalUiGsField50KG>(i))))return true;
    constexpr const char* names[]={"microProgramName","EnableThisShader","totalTextureMaps",
        "TextureMap_1","TextureCoordinateIndex_1","ShaderPassGroup","inputAlphaD",
        "lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K",
        "TextureAnimation","animatedMapInitialFrame","animatedMapFramesPerSecond",
        "animatedMapLooping","animatedMapReverseLoopOnEnd","animatedMapLoopOnEndToFrame",
        "EnableFillColor","FillColorRed","FillColorGreen","FillColorBlue","FillColorAlpha"};
    for(const char* n:names)if(same(key,n))return true;
    return false;
}
bool textureMember(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiImageProvenance50KG& image) {
    const auto* m=image.memberIdentity;
    if(!memberArchive(m)||!image.pstBytes||image.pstOffset>m->bytes||
        image.pstBytes>m->bytes-image.pstOffset)return false;
    const std::string path=m->name;const auto slash=path.find_last_of("\\/");
    const auto leaf=path.substr(slash==std::string::npos?0:slash+1);
    if(leaf.size()<5)return false;
    const auto extension=leaf.substr(leaf.size()-4);
    if(!same(extension,".pst")&&!same(extension,".atm")&&
       !(same(extension,".tga")&&isOriginalUiControlsSpecmap50KG(m)&&image.pstOffset==0&&image.pstBytes==m->bytes))return false;
    if(!same(pass.textureName,leaf.substr(0,leaf.size()-4).c_str()))return false;
    // A standalone PST must be the complete member. An ATM's selected range
    // is authenticated by ScreenAssets but deliberately not an equality field.
    return !same(extension,".pst")||(image.pstOffset==0&&image.pstBytes==m->bytes);
}
}
bool prepareOriginalUiMaterialEquivalence50KG(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiImageProvenance50KG* image,OriginalUiMaterialEquivalenceKey50KG& out,D& d) noexcept {
    d={};
    try {
        const auto* app=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart);
        if(!app||memberArchive(pass.inheritedTemplate)!=app||
            (!same(pass.inheritedTemplate->name,"AppStart\\DefaultMT.psm")&&
             !same(pass.inheritedTemplate->name,"AppStart\\ScreenMT.psm"))||pass.pass<1||pass.pass>8)
            return fail(d,E::source,"source material/template",0x24714c);
        const bool fallback=pass.provenance.origin!=OriginalUiMaterialOrigin50KG::authoredPsm;
        if(!fallback){
            if(!pass.identity||!pass.provenance.archive||memberArchive(pass.identity)!=pass.provenance.archive||
                pass.provenance.creationPC!=0x247110)
                return fail(d,E::source,"authored source material",0x247110);
        }else if(pass.identity||!pass.provenance.archive||pass.pass!=1||pass.provenance.creationPC!=0x2475c0||
            !same(pass.inheritedTemplate->name,"AppStart\\ScreenMT.psm")||
            !pass.authored.values.empty()||!pass.authored.name.empty()||
            memberArchive(pass.provenance.fallbackTexture)!=pass.provenance.archive)
            return fail(d,E::source,"source missing-PSM constructor",0x2475c0);
        OriginalUiMaterialEquivalenceKey50KG next;
        constexpr const char* programs[]={"ScreenObject","Diffuse","DiffuseCompressed","SpecularMapCompressed"};
        for(std::uint8_t i=0;i<4;++i)if(same(pass.microprogram,programs[i]))next.program_=std::uint8_t(i+1);
        if(!next.program_)return fail(d,E::program,"source microprogram identity",0x2471bc);
        if(pass.textureCount<0||pass.textureCount>1)
            return fail(d,E::mapCount,"zero/one-map construction",0x247444);
        next.mapCount_=static_cast<std::uint8_t>(pass.textureCount);
        if(pass.textureCount==0){
            if(image||!pass.textureName.empty()||fallback)
                return fail(d,E::textureSource,"zero-map source",0x247350);
        }else{
            if(!image||!textureMember(pass,*image)||
                (fallback&&image->memberIdentity!=pass.provenance.fallbackTexture))
                return fail(d,E::textureSource,"actual registry PST/ATM source",0x2472f8);
            if(pass.textureCoordinateIndex<0||pass.textureCoordinateIndex>255)
                return fail(d,E::value,"TextureCoordinateIndex_1",0x247340);
            next.texture_=image->memberIdentity;next.uvIndex_=static_cast<std::uint8_t>(pass.textureCoordinateIndex);
        }
        for(const auto& f:pass.authored.values)if(f.sourceValuePresent&&!knownField(f.key))
            return fail(d,E::field,"uncovered authored field",0x247110);
        if(pass.enabled<0||pass.enabled>1)return fail(d,E::value,"EnableThisShader",0x2471f8);
        next.enabled_=pass.enabled!=0;
        if(pass.textureCount==0){
            std::int32_t fill=0;
            if(!originalUiInteger50KG(pass.authored,"EnableFillColor",0,fill,d.document,0x24737c)||fill<0||fill>1)
                return fail(d,E::value,"EnableFillColor",0x247390);
            next.fillColor_=fill!=0;
        }
        // 2476A0 is fixed-size and zero-initialized. All varying state inputs
        // occupy disjoint fields in these bounds, so fieldwise equality is
        // exactly its state-data equality without rebuilding any guest packet.
        // Even inactive blend/test/sampler inputs remain compared by249C84.
        constexpr std::int32_t maxima[]={1,3,255,255,1,3,3,3,3,255,
            3,3,1023,1023,1023,1023,1,1,1,1,1,1,1,1,1,1,1,1,7,255,3,1,1,3,1};
        static_assert(sizeof(maxima)/sizeof(maxima[0])==originalUiGsFieldCount50KG,"source state fields");
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
            const auto value=pass.gs.fields[i].value;
            if(value<0||value>maxima[i])return fail(d,E::value,
                originalUiGsFieldName50KG(static_cast<OriginalUiGsField50KG>(i)),pass.gs.fields[i].sourcePC);
            next.state_[i]=value;
        }
        if(pass.lodCalculationMethod<0||pass.lodCalculationMethod>1||
            pass.maximumMipLevel<0||pass.maximumMipLevel>7||pass.magFilter<0||pass.magFilter>1||
            pass.minFilter<0||pass.minFilter>7||pass.lodL<0||pass.lodL>3||pass.lodK< -2048||pass.lodK>2047)
            return fail(d,E::value,"source sampler fields",0x247ff0);
        next.sampler_={{pass.lodCalculationMethod,pass.maximumMipLevel,pass.magFilter,
            pass.minFilter,pass.lodL,pass.lodK}};
        const auto* animation=pass.authored.find("TextureAnimation");
        if(animation&&!same(animation->text,"none")&&!same(animation->text,"unique"))
            return fail(d,E::animation,"bounded none/unique texture animation",0x29b950);
        next.uniqueAnimation_=animation&&same(animation->text,"unique");
        if(pass.shaderPassGroup.empty()||!ascii(pass.shaderPassGroup,127))
            return fail(d,E::value,"ShaderPassGroup",0x247550);
        next.uniqueGroup_=same(pass.shaderPassGroup,"unique");
        next.group_=pass.shaderPassGroup;for(char& c:next.group_)c=fold(c);
        // 249BA0 does NOT compare material name/pass number, template identity,
        // provenance byte offsets, FillColor RGBA, unique instance addresses,
        // animation phase/rate or source texture frame. Never add those to key.
        next.prepared_=true;out=std::move(next);return true;
    }catch(...){return fail(d,E::allocation,"semantic key allocation",0);}
}
OriginalUiMaterialEquivalence50KG compareOriginalUiMaterialEquivalence50KG(
    const OriginalUiMaterialEquivalenceKey50KG& a,const OriginalUiMaterialEquivalenceKey50KG& b) noexcept {
    using R=OriginalUiMaterialEquivalence50KG;
    if(!a.prepared_||!b.prepared_)return R::unsupported;
    // These are merge exclusions, not equality tags. No &a==&b fast path.
    if(a.uniqueAnimation_||b.uniqueAnimation_||a.uniqueGroup_||b.uniqueGroup_)return R::notEquivalent;
    return a.program_==b.program_&&a.mapCount_==b.mapCount_&&a.texture_==b.texture_&&
        a.uvIndex_==b.uvIndex_&&a.enabled_==b.enabled_&&a.fillColor_==b.fillColor_&&
        a.state_==b.state_&&a.sampler_==b.sampler_&&a.group_==b.group_?R::equivalent:R::notEquivalent;
}
}
