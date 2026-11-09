#include "original_ui_win_draw_resources50pd.h"
#include "original_ui_material_adapter50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "../../../src/assets/original_ui_win_material50pd.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiWinDrawError50PD;using Diagnostic=OriginalUiWinDrawDiagnostic50PD;using Archive=assets::OriginalUiArchiveId50KG;
using Kind=OriginalUiWinMaterialKind50PD;constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0)noexcept{d.error=e;if(pc)d.sourcePC=pc;return false;}
char fold(char c)noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b)noexcept{if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;}
std::string normalized(std::string s){for(auto& c:s)c=fold(c);return s;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id)noexcept{return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);}
bool validVideo(const OriginalUiWinVideoOwner50PD& v)noexcept{return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;}
std::uint64_t imageIdentity(Archive id,const assets::OriginalUiMemberView50KG& m,std::size_t off)noexcept{
    const auto* a=assets::originalUiArchiveIdentity50KG(id);if(!a||off>0xffffffffu)return 0;
    for(std::size_t i=0;i<a->memberCount;++i)if(assets::originalUiMemberIdentity50KG(id,i)==m.identity)
        return (std::uint64_t(static_cast<std::uint8_t>(id)+1u)<<56)|(std::uint64_t(i+1u)<<32)|std::uint64_t(off);
    return 0;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b)noexcept{return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;}
bool animationConfig(const assets::OriginalUiMaterialPass50KG& p,game::OriginalUiTextureAnimationConfig50KG& out,Diagnostic& d){
    const auto* mode=p.authored.find("TextureAnimation");if(!mode||!same(mode->text,"unique"))return fail(d,Error::animation,0x29B950);
    game::OriginalUiTextureAnimationConfig50KG c;assets::OriginalUiPageDiagnostic50KG pd;std::int32_t looping=1,reverse=0,loopTo=-1;
    if(!assets::originalUiNumber50KG(p.authored,"animatedMapInitialFrame",0,c.initialFrame,pd,0x29B950)||
       !assets::originalUiNumber50KG(p.authored,"animatedMapFramesPerSecond",1,c.framesPerSecond,pd,0x29B950)||
       !assets::originalUiInteger50KG(p.authored,"animatedMapLooping",1,looping,pd,0x29B950)||
       !assets::originalUiInteger50KG(p.authored,"animatedMapReverseLoopOnEnd",0,reverse,pd,0x29B950)||
       !assets::originalUiInteger50KG(p.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,pd,0x29B950))return fail(d,Error::animation,0x29B950);
    c.looping=looping!=0;c.reverseLoopOnEnd=reverse!=0;c.loopOnEndToFrame=loopTo;out=c;return true;
}
game::OriginalUiOverlayQuad50KG quad(const char* m,float x,float y,float w,float h,float u0,float v0,float u1,float v1,float depth,std::uint32_t pc)noexcept{
    game::OriginalUiOverlayQuad50KG q;q.materialName=m;q.authoredDepth=depth;q.sourcePC=pc;
    q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};return q;
}
std::string backgroundName(std::uint32_t c){std::string n="00_bkg";n[0]=char('0'+c/10);n[1]=char('0'+c%10);return n;}
bool backgroundCareer(std::uint32_t c)noexcept{return c>=1&&c<=26&&c!=5&&c!=6&&c!=13;}
}
// Same bounded conversion as convertOriginalUiRider19SpecularMaterial50KM, for
// the one depth-writing destination-alpha specular (UIWin trophy pass2).
bool convertOriginalUiWinTrophySpecularSpan50PD(const assets::OriginalUiRider19SpecularMaterial50KM& material,
    const assets::OriginalUiScreenAssets50KG& images,const OriginalUiMaterialSpanBinding50KG& binding,
    OriginalUiMaterialSpan50KG& output) noexcept {
    const auto* bank=images.textures();const auto* image=bank?bank->texture(binding.textureIndex):nullptr;
    if(material.images!=&images||!images.loaded()||!image||binding.textureIndex!=material.imageIndex||
       binding.textureCount!=bank->textureCount()||image->identity!=material.imageIdentity||
       material.material.program!=assets::OriginalUiLitProgram50KM::specularMapCompressed||
       material.material.raster.blend!=assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive||
       !material.material.raster.depthWrite||material.material.alphaTestGreaterZero)return false;
    auto common=material.material.raster;common.blend=assets::OriginalUiMaterialBlend50KG::additive;
    OriginalUiMaterialSpan50KG next;if(!convertOriginalUiMaterialSpan50KG(common,binding,next))return false;
    next.span.materialMode=OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
    next.span.blend=OriginalUiBlend50KG::destinationAlphaAdditive;next.span.alphaScale=1.f;
    next.native.sourceFogEnabled=material.material.fogEnabled;next.native.sourceAlphaTestGreaterZero=material.material.alphaTestGreaterZero;
    output=next;return true;
}
struct OriginalUiWinDrawResources50PD::Storage {
    OriginalUiWinColdInput50PD owners;assets::OriginalUiMaterialTemplates50KG templates;
    game::OriginalUiWinPage50PD page;
    std::array<game::OriginalUiWinModel50PD,6> models;std::array<std::vector<OriginalUiWinModelSurface50PD>,6> surfaces;
    game::OriginalUiWinCamera50PD camera;game::OriginalUiWinRenderContext50PD render;game::OriginalUiWinLighting50PD lights;game::OriginalUiLitContext50KM lit;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;assets::OriginalUiImageBank50KG imageBank;
    std::vector<OriginalUiWinMaterialBinding50PD> bindings;std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,game::originalUiWinWidgetStorage50PD> text;
    std::array<std::uint64_t,game::originalUiWinWidgetStorage50PD> textRevision{};
    std::array<std::uint32_t,game::originalUiWinWidgetStorage50PD> widgetMaterial{};
    std::array<OriginalUiWinBackground50PD,27> backgrounds{};
    std::array<OriginalUiWinPrompt50PD,4> prompts{};
    std::array<std::array<std::string,3>,3> messages; // [place-1][circuit-1]
    game::OriginalUiExitText50KG exitText;
    std::uint32_t find(const std::string& name,std::uint32_t pass)const noexcept{
        for(std::size_t i=0;i<bindings.size();++i)if(bindings[i].passIndex==pass&&same(bindings[i].registeredName,name))return static_cast<std::uint32_t>(i);
        return unresolved;}
};
OriginalUiWinDrawResources50PD::OriginalUiWinDrawResources50PD()noexcept=default;
OriginalUiWinDrawResources50PD::~OriginalUiWinDrawResources50PD()=default;
bool OriginalUiWinDrawResources50PD::load(const OriginalUiWinColdInput50PD& in,Diagnostic& d)noexcept{
    d={};if(storage_)return fail(d,Error::alreadyLoaded);
    if(!authentic(in.appStart,Archive::appStart)||!authentic(in.win,Archive::win)||!authentic(in.fonts,Archive::fonts)||
       !authentic(in.controllerPopup,Archive::controllerPopup)||!authentic(in.overlays,Archive::overlays)||!authentic(in.backgrounds,Archive::backgrounds))
        return fail(d,Error::archive,0x1CCBA0);
    if(!validVideo(in.video))return fail(d,Error::videoOwner);
    if(!in.english||!in.english->loaded()||!in.runtime.overrideKnown)return fail(d,Error::input);
    try{
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
        if(!s->page.bind(in.win,in.english,d.page))return fail(d,Error::page,d.page.sourcePC);
        if(!game::prepareOriginalUiWinCamera50PD(*in.win,s->camera,d.scene)||
           !game::prepareOriginalUiWinRenderContext50PD(*in.win,s->camera,s->render,d.scene)||
           !game::prepareOriginalUiWinLighting50PD(*in.win,s->lights,d.scene)||
           !game::prepareOriginalUiWinLitContext50PD(s->render,s->lights,s->lit))return fail(d,Error::scene,d.scene.sourcePC);
        for(std::uint16_t id=9;id<=14;++id){const auto* cfg=s->page.rotateConfig(id);d.widgetId=id;
            if(!cfg||!s->models[id-9].load(in.win,id,*cfg,d.scene))return fail(d,Error::model,d.scene.sourcePC);}
        if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material,0x17CAE8);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        const auto addSource=[&](const assets::OriginalUiImageSource50KG& x)->std::uint32_t{
            for(std::size_t i=0;i<sources.size();++i){const auto& o=sources[i];
                if(o.member.identity==x.member.identity&&o.pstOffset==x.pstOffset&&o.pstBytes==x.pstBytes&&o.cel.identity==x.cel.identity)return static_cast<std::uint32_t>(i);}
            sources.push_back(x);return static_cast<std::uint32_t>(sources.size()-1);};
        // Standalone PST (with same-name CEL for fonts) or ATM frames.
        const auto addImages=[&](OriginalUiWinMaterialBinding50PD& b,const std::shared_ptr<const assets::OriginalUiArchive50KG>& owner,
            const std::string& directory,const std::string& name,bool font,bool animated)->bool{
            const auto member=owner->find(directory+name+(animated?".atm":".pst"));if(!member.identity||!member.data)return fail(d,Error::resource,0x24DA30);
            const auto id=owner->identity()->id;b.animated=animated;
            if(animated){assets::OriginalUiAtm50KG atm;if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm))return fail(d,Error::resource,0x102230);
                for(const auto& f:atm.frames){const auto identity=imageIdentity(id,member,f.pstOffset);if(!identity)return fail(d,Error::resource);
                    b.images.push_back(addSource({identity,member,f.pstOffset,f.pstBytes,{}}));}}
            else{const auto cel=font?owner->find(directory+name+".cel"):assets::OriginalUiMemberView50KG{};if(font&&(!cel.identity||!cel.data))return fail(d,Error::resource,0x2DFAA0);
                const auto identity=imageIdentity(id,member,0);if(!identity)return fail(d,Error::resource);
                b.images.push_back(addSource({identity,member,0,member.size,cel}));}
            return true;};
        const auto screen=[&](const char* registered,const char* texture,const std::shared_ptr<const assets::OriginalUiArchive50KG>* owner,bool font)->bool{
            OriginalUiWinMaterialBinding50PD b;b.registeredName=registered;b.kind=texture?Kind::screen:Kind::untextured;
            if(!assets::readOriginalUiMaterial50KG(*in.win,b.registeredName+".psm",s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material,0x2476A0);
            if(b.material.passes.size()!=1)return fail(d,Error::material,0x2476A0);
            if(texture){if(b.pass().textureCount!=1||!same(b.pass().textureName,texture)||b.pass().authored.find("TextureAnimation"))return fail(d,Error::material,0x2476A0);
                if(!addImages(b,*owner,font?"font/":"ui/textures/",texture,font,false))return false;}
            else if(b.pass().textureCount!=0)return fail(d,Error::material,0x2476A0);
            s->bindings.push_back(std::move(b));return true;};
        if(!screen("font/denmrk16top","denmrk16",&in.fonts,true)||!screen("font/denmrk16","denmrk16",&in.fonts,true)||
           !screen("font/denmrk18","denmrk18",&in.fonts,true)||!screen("font/venus16top","venus16",&in.fonts,true)||
           !screen("ui/textures/overlay1","overlay1",&in.controllerPopup,false)||!screen("ui/textures/notexture",nullptr,nullptr,false))return false;
        // Shared page 1D63B0 prompts (HasPopup1D6698 -> 1D682C..1D6A4C).
        constexpr const char* promptNames[]={"icon_x","icon_tri","icon_sqr","icon_cir"};
        for(std::size_t i=0;i<4;++i){OriginalUiWinMaterialBinding50PD b;b.registeredName=std::string("ui/textures/")+promptNames[i];
            if(!assets::readOriginalUiWinPromptFallbackMaterial50PD(*in.win,*in.overlays,static_cast<assets::OriginalUiMainPrompt50KG>(i),
                s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material,0x1D6698);
            if(!addImages(b,i<2?in.overlays:in.win,"ui/textures/",promptNames[i],false,false))return false;
            s->prompts[i].material=static_cast<std::uint32_t>(s->bindings.size());s->bindings.push_back(std::move(b));}
        // Load 2077F0: sprintf("%02d_bkg", P+100C0) ScreenTextures. UIWIN supplies
        // every authored %02d_bkg.psm, UIBKG the image (careers 5,6,13 absent).
        for(std::uint32_t c=1;c<=26;++c){if(!backgroundCareer(c))continue;const auto name=backgroundName(c);
            OriginalUiWinMaterialBinding50PD b;b.registeredName="ui/textures/"+name;b.kind=Kind::screen;
            if(!assets::readOriginalUiMaterial50KG(*in.win,b.registeredName+".psm",s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material)||
               b.material.passes.size()!=1||b.pass().textureCount!=1||!same(b.pass().textureName,name))return fail(d,Error::material,0x2DF0B0);
            if(!addImages(b,in.backgrounds,"ui/textures/",name,false,false))return false;
            s->backgrounds[c].career=c;s->backgrounds[c].material=static_cast<std::uint32_t>(s->bindings.size());s->bindings.push_back(std::move(b));}
        // Model materials (DefaultMT): each authored PSM pass is its own binding.
        for(std::size_t m=0;m<6;++m){const auto* scene=s->models[m].scene();
            for(const auto& name:scene->materials){const auto registered="ui/textures/"+name;
                if(s->find(registered,0)!=unresolved)continue;
                assets::OriginalUiMaterial50KG material;
                if(!assets::readOriginalUiMaterial50KG(*in.win,registered+".psm",s->templates,assets::OriginalUiTemplate50KG::defaultMt,in.runtime,material,d.material))return fail(d,Error::material,0x2476A0);
                const bool star=same(name,"star");
                if(material.passes.size()!=(star?1u:2u))return fail(d,Error::material,0x2476A0);
                for(std::uint32_t p=0;p<material.passes.size();++p){
                    OriginalUiWinMaterialBinding50PD b;b.registeredName=registered;b.material=material;b.passIndex=p;
                    const auto& pass=b.pass();
                    if(pass.textureCount!=1||pass.textureCoordinateIndex!=0)return fail(d,Error::material,0x2476A0);
                    if(p==0){
                        if(!same(pass.microprogram,"DiffuseCompressed"))return fail(d,Error::material,0x2476A0);
                        b.kind=Kind::lit;const bool animated=pass.authored.find("TextureAnimation")!=nullptr;
                        if(animated!=star)return fail(d,Error::animation,0x29B950);
                        if(!addImages(b,in.win,"ui/textures/",pass.textureName,false,animated))return false;
                        if(animated&&(!animationConfig(pass,b.animation,d)||
                            !game::validOriginalUiTextureAnimationConfig50KG(b.animation,static_cast<std::uint32_t>(b.images.size()))))return fail(d,Error::animation,0x29B950);
                    }else{
                        if(!same(pass.microprogram,"SpecularMapCompressed")||pass.authored.find("TextureAnimation"))return fail(d,Error::material,0x2476A0);
                        b.kind=Kind::specular;if(!addImages(b,in.win,"ui/textures/",pass.textureName,false,false))return false;
                    }
                    s->bindings.push_back(std::move(b));
                }
            }
            for(const auto& surface:scene->lods[0].surfaces){const auto registered="ui/textures/"+scene->materials[surface.material_index];
                OriginalUiWinModelSurface50PD entry{s->find(registered,0),s->find(registered,1)};
                if(entry.first==unresolved||(entry.second==unresolved)!=same(scene->materials[surface.material_index],"star"))return fail(d,Error::material,0x240C00);
                s->surfaces[m].push_back(entry);}
        }
        // 29C490 registry order: authenticated APPSTART Mat_Stbl.txt first-match rank.
        {const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");if(!table.identity||!table.data)return fail(d,Error::registry,0x298724);
            std::map<std::string,std::uint32_t> ranks;std::size_t at=0;std::uint32_t token=0;
            const auto delim=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n'||c=='\t';};
            while(at<table.size){while(at<table.size&&delim(table.data[at]))++at;if(at==table.size)break;const auto first=at;
                while(at<table.size&&!delim(table.data[at]))++at;
                auto inserted=ranks.emplace(normalized(std::string(reinterpret_cast<const char*>(table.data+first),at-first)),++token);
                if(!inserted.second)inserted.first->second=0;}
            if(token!=3235)return fail(d,Error::registry,0x298724);
            for(auto& b:s->bindings){const auto it=ranks.find(normalized(b.registeredName));if(it==ranks.end()||!it->second)return fail(d,Error::registry,0x298724);b.registryToken=it->second;}}
        auto originalImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!originalImages->load(sources,d.resource))return fail(d,Error::resource,0x24DA30);
        assets::OriginalUiImageBankInput50KG bankInput;bankInput.archives={in.appStart,in.win,in.fonts,in.controllerPopup,in.overlays,in.backgrounds};bankInput.owners={originalImages};
        assets::OriginalUiImageBankDiagnostic50KG bankDiagnostic;if(!s->imageBank.load(bankInput,bankDiagnostic))return fail(d,Error::resource);
        for(std::uint32_t i=0;i<sources.size();++i){const auto* map=s->imageBank.mapping(0,i);if(!map||map->combinedIndex!=i)return fail(d,Error::resource);}
        s->images=s->imageBank.images();const auto* bank=s->images?s->images->textures():nullptr;if(!bank)return fail(d,Error::resource);
        s->samplers.resize(bank->textureCount());std::vector<bool> set(bank->textureCount(),false);
        const auto sampler=[&](std::uint32_t image,const MaterialTextureSampler50EB& value)->bool{
            if(image>=set.size())return false;if(set[image]&&!sameSampler(s->samplers[image],value))return fail(d,Error::samplerConflict);
            s->samplers[image]=value;set[image]=true;return true;};
        for(std::size_t i=0;i<s->bindings.size();++i){auto& b=s->bindings[i];d.materialIndex=static_cast<std::uint32_t>(i);const auto& pass=b.pass();
            if(b.kind==Kind::untextured){if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))return fail(d,Error::material);continue;}
            for(auto image:b.images){d.imageIndex=image;const auto* decoded=s->images->decodedTexture(image);if(!decoded)return fail(d,Error::resource);
                OriginalUiMaterialSpan50KG converted;const OriginalUiMaterialSpanBinding50KG range{0,1,1,image,bank->textureCount()};
                if(b.kind==Kind::screen){assets::OriginalUiNativeMaterial50KG policy;
                    if(!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material)||!convertOriginalUiMaterialSpan50KG(policy,range,converted))return fail(d,Error::material);
                    b.screenPolicies.push_back(policy);}
                else if(b.kind==Kind::lit){assets::OriginalUiLitMaterial50KM policy;
                    if(!assets::prepareOriginalUiLitMaterial50KM(pass,*decoded,policy,d.material)||!convertOriginalUiLitMaterialSpan50KM(policy,range,converted))return fail(d,Error::material);
                    b.litPolicies.push_back(policy);}
                else{if(b.images.size()!=1||!assets::prepareOriginalUiWinTrophySpecularMaterial50PD(pass,*s->images,image,b.specular,d.material)||
                        !convertOriginalUiWinTrophySpecularSpan50PD(b.specular,*s->images,range,converted))return fail(d,Error::material);}
                if(!sampler(image,converted.native.sampler))return false;}}
        for(bool v:set)if(!v)return fail(d,Error::resource);
        // Widget resources, ascending manager order.
        for(auto id:*s->page.order()){d.widgetId=id;
            if(const auto* text=s->page.textDescriptor(id)){const auto m=s->find("font/"+text->materialName,0);if(m==unresolved)return fail(d,Error::material,0x2A90B8);
                const auto& b=s->bindings[m];if(b.images.size()!=1||!same(b.pass().textureName,text->fontName))return fail(d,Error::material,0x2A90B8);
                const auto* font=s->images->font(b.images[0]);const auto* atlas=bank->texture(b.images[0]);if(!font||!atlas)return fail(d,Error::resource);
                if(text->depth!=1)return fail(d,Error::geometry,0x2A9000);
                if(text->textSource!=assets::OriginalUiTextSource50KG::dynamic){auto g=std::make_unique<game::OriginalUiTextGeometry50KG>();
                    if(!g->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry,0x2E0A30);s->text[id]=std::move(g);}
                s->widgetMaterial[id]=m;
            }else if(const auto* icon=s->page.iconDescriptor(id)){const auto m=s->find("ui/textures/"+icon->materialName,0);if(m==unresolved)return fail(d,Error::material,0x2AC6B0);
                const auto& b=s->bindings[m];if(b.images.empty()||icon->depth!=1)return fail(d,Error::resource);const auto* atlas=bank->texture(b.images[0]);if(!atlas)return fail(d,Error::resource);
                if(!s->page.prepareIcon(id,atlas->texture,in.video.packetZMaximum,d.page))return fail(d,Error::page,0x2AC590);s->widgetMaterial[id]=m;}}
        const auto denmark=s->find("font/denmrk16top",0);if(denmark==unresolved||s->bindings[denmark].images.size()!=1)return fail(d,Error::resource);
        if(s->exitText.prepare({7,1},in.win,*in.english,s->images,s->bindings[denmark].images[0],d.exitText)!=game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        // Load 2077F0..207D68: four 320x224 tiles; sample endpoints 319/223 times
        // the 2DF180 reciprocal dimensions; tile1 mirrors U, tile2 V, tile3 both.
        // ScreenTexture 2DF0B0 depth15 (packet Z = video max - 15).
        for(auto& bg:s->backgrounds){if(!bg.career)continue;const auto& b=s->bindings[bg.material];const auto* atlas=bank->texture(b.images[0]);
            if(!atlas||!atlas->texture.width||!atlas->texture.height)return fail(d,Error::resource);
            const float us=1.f/float(atlas->texture.width),vs=1.f/float(atlas->texture.height);const float u=319.f*us,v=223.f*vs;const auto* name=b.registeredName.c_str();
            bg.tiles[0]=quad(name,0,0,320,224,0,0,u,v,15,0x2DF550);bg.tiles[1]=quad(name,320,0,320,224,u,0,0,v,15,0x2DF550);
            bg.tiles[2]=quad(name,0,224,320,224,0,v,u,0,15,0x2DF550);bg.tiles[3]=quad(name,320,224,320,224,u,v,0,0,15,0x2DF550);}
        // Shared page 1D7A10 prompt positions (cross page+2A0 at 484,378).
        constexpr float promptX[]={484,397,285,160};constexpr std::uint32_t promptPC[]={0x1D7A2C,0x1D7A44,0x1D7A5C,0x1D7A74};
        for(std::size_t i=0;i<4;++i){auto& p=s->prompts[i];const auto& b=s->bindings[p.material];const auto* atlas=bank->texture(b.images[0]);if(!atlas)return fail(d,Error::resource);
            p.mask=static_cast<std::uint8_t>(1u<<i);
            p.quad=quad(b.registeredName.c_str(),promptX[i],378,float(atlas->texture.width),float(atlas->texture.height),0,0,1,1,1,promptPC[i]);}
        for(std::uint8_t place=1;place<=3;++place)for(std::uint8_t circuit=1;circuit<=3;++circuit){std::uint32_t pc=0;
            if(!game::buildOriginalUiWinMessage50PD(*in.english,{circuit,place},s->messages[place-1][circuit-1],pc))return fail(d,Error::resource,pc);}
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiWinDrawResources50PD::loaded()const noexcept{return bool(storage_);}
game::OriginalUiWinPage50PD* OriginalUiWinDrawResources50PD::page()noexcept{return storage_?&storage_->page:nullptr;}
const game::OriginalUiWinPage50PD* OriginalUiWinDrawResources50PD::page()const noexcept{return storage_?&storage_->page:nullptr;}
game::OriginalUiWinModel50PD* OriginalUiWinDrawResources50PD::model(std::uint16_t id)noexcept{return storage_&&id>=9&&id<=14?&storage_->models[id-9]:nullptr;}
const game::OriginalUiWinModel50PD* OriginalUiWinDrawResources50PD::model(std::uint16_t id)const noexcept{return storage_&&id>=9&&id<=14?&storage_->models[id-9]:nullptr;}
const std::vector<OriginalUiWinModelSurface50PD>* OriginalUiWinDrawResources50PD::modelSurfaces(std::uint16_t id)const noexcept{return storage_&&id>=9&&id<=14?&storage_->surfaces[id-9]:nullptr;}
const game::OriginalUiWinCamera50PD* OriginalUiWinDrawResources50PD::camera()const noexcept{return storage_?&storage_->camera:nullptr;}
const game::OriginalUiWinRenderContext50PD* OriginalUiWinDrawResources50PD::renderContext()const noexcept{return storage_?&storage_->render:nullptr;}
const game::OriginalUiWinLighting50PD* OriginalUiWinDrawResources50PD::lighting()const noexcept{return storage_?&storage_->lights:nullptr;}
const game::OriginalUiLitContext50KM* OriginalUiWinDrawResources50PD::litContext()const noexcept{return storage_?&storage_->lit:nullptr;}
game::OriginalUiExitText50KG* OriginalUiWinDrawResources50PD::exitText()noexcept{return storage_?&storage_->exitText:nullptr;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiWinDrawResources50PD::images()const noexcept{return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiWinDrawResources50PD::sourceImageBank()const noexcept{return storage_?&storage_->imageBank:nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiWinDrawResources50PD::textures()const noexcept{return storage_&&storage_->images?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiWinDrawResources50PD::samplers()const noexcept{return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiWinMaterialBinding50PD>* OriginalUiWinDrawResources50PD::materials()const noexcept{return storage_?&storage_->bindings:nullptr;}
const OriginalUiWinVideoOwner50PD* OriginalUiWinDrawResources50PD::video()const noexcept{return storage_?&storage_->owners.video:nullptr;}
std::uint32_t OriginalUiWinDrawResources50PD::materialIndex(const char* name,std::uint32_t pass)const noexcept{
    if(!storage_||!name)return unresolved;try{return storage_->find(name,pass);}catch(...){return unresolved;}}
std::uint32_t OriginalUiWinDrawResources50PD::widgetMaterial(std::uint16_t id)const noexcept{return storage_&&id<game::originalUiWinWidgetStorage50PD?storage_->widgetMaterial[id]:unresolved;}
const game::OriginalUiTextGeometry50KG* OriginalUiWinDrawResources50PD::textGeometry(std::uint16_t id)const noexcept{return storage_&&id<game::originalUiWinWidgetStorage50PD?storage_->text[id].get():nullptr;}
bool OriginalUiWinDrawResources50PD::prepareDynamicText(std::uint16_t id,Diagnostic& d)noexcept{
    d={};d.widgetId=id;if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    const auto* text=s.page.textDescriptor(id);const auto* lines=s.page.dynamicLines(id);
    if(!text||!lines||s.widgetMaterial[id]==unresolved)return fail(d,Error::input,0x2A9FF0);
    const auto revision=s.page.dynamicRevision(id);if(s.text[id]&&s.textRevision[id]==revision)return true;
    try{const auto& b=s.bindings[s.widgetMaterial[id]];const auto* font=s.images->font(b.images[0]);const auto* atlas=s.images->textures()->texture(b.images[0]);
        if(!font||!atlas)return fail(d,Error::resource);
        auto g=std::make_unique<game::OriginalUiTextGeometry50KG>();
        if(!g->prepareDynamic(*text,*lines,*font,atlas->texture,d.text))return fail(d,Error::geometry,0x2A9DE0);
        s.text[id]=std::move(g);s.textRevision[id]=revision;return true;
    }catch(...){return fail(d,Error::allocation);}
}
const OriginalUiWinBackground50PD* OriginalUiWinDrawResources50PD::background(std::uint32_t career)const noexcept{
    return storage_&&career<27&&storage_->backgrounds[career].career?&storage_->backgrounds[career]:nullptr;}
const std::array<OriginalUiWinPrompt50PD,4>* OriginalUiWinDrawResources50PD::prompts()const noexcept{return storage_?&storage_->prompts:nullptr;}
const std::string* OriginalUiWinDrawResources50PD::message(game::OriginalUiWinCircuit50PD c)const noexcept{
    return storage_&&c.circuit>=1&&c.circuit<=3&&c.place>=1&&c.place<=3?&storage_->messages[c.place-1][c.circuit-1]:nullptr;}
}
