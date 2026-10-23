#include "original_ui_garage_shared_page50kg.h"
#include <algorithm>
#include <cmath>
namespace rr::platform {namespace {
using D=OriginalUiGarageSharedDiagnostic50KG;using E=OriginalUiGarageSharedError50KG;using A=assets::OriginalUiArchiveId50KG;
constexpr auto missing=assets::unresolvedMaterialTexture50EA;
bool fail(D& d,E e,std::uint32_t pc=0)noexcept{d.error=e;d.sourcePC=pc;return false;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,A id)noexcept{
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);}
char fold(char c)noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b)noexcept{
    if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;}
bool registry(const assets::OriginalUiMemberView50KG& table,std::array<OriginalUiGarageSharedMaterial50KG,6>& bindings){
    if(!table.identity||!table.data||table.size!=70962)return false;
    std::array<bool,6> found{};std::size_t p=0;std::uint32_t token=0;
    const auto space=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(p<table.size){while(p<table.size&&space(table.data[p]))++p;if(p==table.size)break;
        const auto b=p;while(p<table.size&&!space(table.data[p]))++p;
        const std::string name(reinterpret_cast<const char*>(table.data+b),p-b);
        for(std::size_t i=0;i<bindings.size();++i)if(same(bindings[i].registeredName,name)){
            if(found[i])return false;found[i]=true;bindings[i].registryToken=token;bindings[i].registryIdentity=table.identity;}
        ++token;}
    return token==3235&&std::all_of(found.begin(),found.end(),[](bool b){return b;});
}
}
struct OriginalUiGarageSharedPageDrawOwner50KG::Storage {
    OriginalUiGarageSharedColdInput50KG owners;
    assets::OriginalUiMaterialTemplates50KG templates;
    assets::OriginalUiImageBank50KG bank;assets::OriginalUiImageBankInput50KG input;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
    std::array<OriginalUiGarageSharedMaterial50KG,6> materials;
    std::array<OriginalUiGarageSharedPrompt50KG,4> prompts;
    std::array<game::OriginalUiOverlayQuad50KG,2> apertures;
    std::array<OriginalUiGarageSharedPacket50KG,6> packets{};std::size_t count=0;
    std::vector<MaterialTextureSampler50EB> samplers;
    game::OriginalUiExitText50KG exitText;
};
OriginalUiGarageSharedPageDrawOwner50KG::OriginalUiGarageSharedPageDrawOwner50KG()noexcept=default;
OriginalUiGarageSharedPageDrawOwner50KG::~OriginalUiGarageSharedPageDrawOwner50KG()=default;
bool OriginalUiGarageSharedPageDrawOwner50KG::load(const OriginalUiGarageSharedColdInput50KG& in,D& d)noexcept{
    d={};if(storage_)return fail(d,E::alreadyLoaded,0x1FA2EC);
    if(!in.panel||!in.panel->loaded()||!in.panel->pageData()||!in.panel->bankInput()||!in.panel->samplers()||
       !in.panel->video()||!in.panel->video()->lifetime||!authentic(in.appStart,A::appStart)||!authentic(in.overlays,A::overlays)||
       !in.english||!in.english->loaded()||!in.runtime.overrideKnown||in.runtime.override435670)return fail(d,E::owner,0x1FA2EC);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;
        auto garage=in.panel->pageData()->archive();if(!authentic(garage,A::garage))return fail(d,E::owner,0x1FA2EC);
        const auto& pins=in.panel->bankInput()->archives;
        if(std::find(pins.begin(),pins.end(),in.appStart)==pins.end())return fail(d,E::owner,0x2DF10C);
        if(!s->templates.load(*in.appStart,d.materialDetail))return fail(d,E::material,0x2DF10C);
        constexpr const char* names[]{"font/denmrk16top","ui/textures/notexture","ui/textures/icon_x","ui/textures/icon_tri","ui/textures/icon_sqr","ui/textures/icon_cir"};
        for(std::size_t i=0;i<6;++i)s->materials[i].registeredName=names[i];
        for(std::size_t i=0;i<2;++i){auto& m=s->materials[i];
            if(!assets::readOriginalUiMaterial50KG(*garage,m.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,m.authored,d.materialDetail)||m.authored.passes.size()!=1)
                return fail(d,E::material,i?0x1D66F8:0x1D6BB0);}
        if(!registry(in.appStart->find("AppStart/Mat_Stbl.txt"),s->materials))return fail(d,E::registry,0x298C80);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        for(std::uint32_t i=0;i<4;++i){const auto& archive=i<2?*in.overlays:*garage;
            const auto member=archive.find(std::string(names[i+2])+".pst");
            if(!member.identity||!member.data)return fail(d,E::resource,0x2DF180);
            sources.push_back({0x4753500000000001ull+i,member,0,member.size,{}});}
        auto promptImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!promptImages->load(sources,d.image))return fail(d,E::resource,0x2DF180);
        s->input=*in.panel->bankInput();s->input.archives.push_back(in.overlays);
        s->input.owners={in.panel->images(),promptImages};
        if(!s->bank.load(s->input,d.bank))return fail(d,E::bank,0x2DF180);
        s->images=s->bank.images();const auto* bank=s->images?s->images->textures():nullptr;
        if(!bank||bank->textureCount()!=18)return fail(d,E::bank,0x2DF180);
        s->samplers.resize(18);
        for(std::uint32_t i=0;i<14;++i){const auto* m=s->bank.mapping(0,i);if(!m||m->combinedIndex!=i)return fail(d,E::bank);
            s->samplers[i]=in.panel->samplers()->at(i);}
        const auto& panelFont=in.panel->materials()->at(0);const auto* fontMap=s->bank.mapping(0,panelFont.images.at(0));
        if(!fontMap||s->materials[0].authored.identity!=panelFont.authored.identity)return fail(d,E::resource,0x1D6BB0);
        auto& font=s->materials[0];font.image=fontMap->combinedIndex;
        const auto* pixels=s->images->decodedTexture(font.image);
        if(!pixels||!assets::prepareOriginalUiNativeMaterial50KG(font.authored.passes[0],*pixels,font.textured,d.materialDetail))return fail(d,E::material,0x1D6BB0);
        auto& plain=s->materials[1];
        if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(plain.authored.passes[0],plain.untextured,d.materialDetail)||
           plain.untextured.depthMethod!=1||plain.untextured.blend!=assets::OriginalUiMaterialBlend50KG::opaque)
            return fail(d,E::material,0x1D66F8);
        // Garage1FA8EC overrides inherited triangle397 to380; retain full32px width.
        constexpr float x[]{484,380,285,160};constexpr std::uint32_t pos[]{0x1D6878,0x1FA8EC,0x1D69E8,0x1D6A98};
        constexpr std::uint32_t draw[]{0x1D7A2C,0x1D7A44,0x1D7A5C,0x1D7A74};
        for(std::uint32_t i=0;i<4;++i){auto& m=s->materials[i+2];d.material=i+2;
            const auto* map=s->bank.mapping(1,i);if(!map)return fail(d,E::bank);m.image=map->combinedIndex;
            m.prompt=std::make_unique<assets::OriginalUiGaragePromptFallback50KG>();
            if(!m.prompt->load(garage,in.appStart,in.overlays,std::uint8_t(i),in.runtime,s->images,m.image,d.prompt))return fail(d,E::material,d.prompt.sourcePC);
            m.textured=*m.prompt->policy();auto& sampler=s->samplers[m.image];
            sampler.minification=sampler.magnification=MaterialTextureFilter50EB::linear;
            sampler.s=sampler.t=MaterialTextureWrap50EB::clampToEdge;
            const auto* image=bank->texture(m.image);if(!image||image->texture.width!=32||image->texture.height!=32)return fail(d,E::resource,0x2DF180);
            auto& p=s->prompts[i];p.material=i+2;p.image=m.image;p.mask=1u<<i;p.objectOffset=0x2A0+0x60*i;p.positionPC=pos[i];
            auto& q=p.quad;q.materialName=names[i+2];q.authoredDepth=1;q.sourcePC=draw[i];
            q.vertices={{{x[i],378,0,0},{x[i]+32,378,1,0},{x[i],410,0,1},{x[i]+32,410,1,1}}};
        }
        if(s->exitText.prepare({13,1},garage,*in.english,s->images,font.image,d.exitText)!=game::OriginalUiExitTextStatus50KG::ok)
            return fail(d,E::resource,0x1D6BB0);
        // Export the immutable canonical bank, not transient local IDs.
        s->input.owners={s->images};storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiGarageSharedPageDrawOwner50KG::loaded()const noexcept{return bool(storage_);}
const OriginalUiGaragePanel50KG* OriginalUiGarageSharedPageDrawOwner50KG::panel()const noexcept{return storage_?storage_->owners.panel.get():nullptr;}
const std::array<OriginalUiGarageSharedMaterial50KG,6>* OriginalUiGarageSharedPageDrawOwner50KG::materials()const noexcept{return storage_?&storage_->materials:nullptr;}
const std::array<OriginalUiGarageSharedPrompt50KG,4>* OriginalUiGarageSharedPageDrawOwner50KG::prompts()const noexcept{return storage_?&storage_->prompts:nullptr;}
game::OriginalUiExitText50KG* OriginalUiGarageSharedPageDrawOwner50KG::exitText()noexcept{return storage_?&storage_->exitText:nullptr;}
const game::OriginalUiExitText50KG* OriginalUiGarageSharedPageDrawOwner50KG::exitText()const noexcept{return storage_?&storage_->exitText:nullptr;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiGarageSharedPageDrawOwner50KG::images()const noexcept{return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiGarageSharedPageDrawOwner50KG::sourceImageBank()const noexcept{return storage_?&storage_->bank:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiGarageSharedPageDrawOwner50KG::bankInput()const noexcept{return storage_?&storage_->input:nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiGarageSharedPageDrawOwner50KG::samplers()const noexcept{return storage_?&storage_->samplers:nullptr;}
bool OriginalUiGarageSharedPageDrawOwner50KG::projectAfterAperture(const game::OriginalUiApertureOwner50KG& aperture,
    std::uint32_t mask,std::uint32_t suppression,D& d)noexcept{
    d={};if(!storage_)return fail(d,E::notLoaded);auto& s=*storage_;
    const auto* config=aperture.config();const auto state=aperture.snapshot();const auto* geometry=aperture.geometry();
    if(!config||config->pageIdentity!=s.owners.panel->pageData()->pageDat()->identity()||!state.bound||
       !state.geometryPrepared||!geometry||geometry->sourceVertexAlpha!=63)return fail(d,E::aperture,0x1D7550);
    if(mask&~15u)return fail(d,E::mask,0x1D7A10);
    std::array<game::OriginalUiOverlayQuad50KG,2> quads;
    for(std::uint32_t i=0;i<2;++i){const auto& r=geometry->rectangles[i];
        if(!r.dirty3C||r.sourceObjectOffset!=0x424+4*i||r.left||r.right!=640||
           r.top!=(i?state.bottom28C:0)||r.bottom!=(i?448:state.top284))return fail(d,E::aperture,0x1D76D4);
        auto& q=quads[i];q.materialName="ui/textures/notexture";q.authoredDepth=1;q.sourcePC=0x1D798C;
        q.sourcePacketColor={{0,0,0,63}};constexpr unsigned order[]{0,2,1,3};
        for(unsigned j=0;j<4;++j){const auto& v=r.vertices[order[j]];if(!std::isfinite(v.x)||!std::isfinite(v.y))return fail(d,E::aperture,0x1D76D4);
            q.vertices[j]={v.x,v.y,0,0};}
    }
    s.apertures=quads;s.count=0;const auto z=float(s.owners.panel->video()->packetZMaximum)-1;
    for(std::uint32_t i=0;i<2;++i)s.packets[s.count++]={&s.apertures[i],1,missing,0x1D798C,0x424+4*i,z};
    if(!suppression)for(const auto& p:s.prompts)if(mask&p.mask)s.packets[s.count++]={&p.quad,p.material,p.image,p.quad.sourcePC,p.objectOffset,z};
    return true;
}
const OriginalUiGarageSharedPacket50KG* OriginalUiGarageSharedPageDrawOwner50KG::packets()const noexcept{return storage_&&storage_->count?storage_->packets.data():nullptr;}
std::size_t OriginalUiGarageSharedPageDrawOwner50KG::packetCount()const noexcept{return storage_?storage_->count:0;}
}
