#include "original_ui_rider_draw_resources50kg.h"
#include <cmath>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiRiderDrawResourceError50KG;
using Diagnostic=OriginalUiRiderDrawResourceDiagnostic50KG;
using Archive=assets::OriginalUiArchiveId50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
struct PopupMaterial {const char* name;const char* texture;std::uint32_t token;std::size_t offset;bool font;};
// Exact APPSTART rows for the existing root470/47C owners. These are not
// Rider widgets or substitute overlay materials.
constexpr PopupMaterial popupMaterials[]={
    {"ui/textures/dimpopup",nullptr,3228,70804,false},
    {"ui/textures/overlay1popup","overlay1",3229,70826,false},
    {"font/venus16popup","venus16",3230,70853,true},
    {"font/venus14popup","venus14",3231,70872,true},
    {"ui/textures/icon_x_popup","icon_x",3232,70891,false},
    {"ui/textures/icon_tri_popup","icon_tri",3233,70917,false}
};
bool fail(Diagnostic& d,Error error,std::uint32_t pc=0) noexcept {d.error=error;d.sourcePC=pc;return false;}
char fold(char c) noexcept {return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool riderIndex(unsigned r) noexcept {return r>=1&&r<=26&&r!=5&&r!=6&&r!=13;}
std::string backgroundName(unsigned r) {std::string name="00_bkg";name[0]=char('0'+r/10);name[1]=char('0'+r%10);return name;}
bool validVideo(const OriginalUiRiderVideoOwner50KG& v) noexcept {
    return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;
}
std::uint64_t imageIdentity(Archive id,const assets::OriginalUiMemberView50KG& member,std::size_t offset) noexcept {
    const auto* archive=assets::originalUiArchiveIdentity50KG(id);if(!archive||offset>0xffffffffu)return 0;
    for(std::size_t i=0;i<archive->memberCount;++i)if(assets::originalUiMemberIdentity50KG(id,i)==member.identity)
        return (std::uint64_t(static_cast<std::uint8_t>(id)+1u)<<56)|(std::uint64_t(i+1u)<<32)|std::uint64_t(offset);
    return 0;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
std::uint32_t find(const std::vector<OriginalUiRiderMaterialBinding50KG>& list,const std::string& name) noexcept {
    for(std::size_t i=0;i<list.size();++i)if(same(list[i].registeredName,name))return static_cast<std::uint32_t>(i);
    return unresolved;
}
bool registry(const assets::OriginalUiMemberView50KG& table,std::vector<OriginalUiRiderMaterialBinding50KG>& bindings) {
    if(!table.identity||!table.data||table.size!=70962)return false;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::vector<bool> found(bindings.size(),false);std::size_t cursor=0;std::uint32_t token=0;
    // The same authenticated Mat_Stbl token stream as Main/Options. Ordinals
    // retain the original table's separators/comments; do not sort by basename.
    while(cursor<table.size){
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(std::size_t i=0;i<bindings.size();++i){
            auto& b=bindings[i];if(b.registeredName.size()!=cursor-start)continue;
            std::size_t j=0;
            for(;j<b.registeredName.size()&&fold(char(table.data[start+j]))==fold(b.registeredName[j]);++j){}
            if(j!=b.registeredName.size())continue;
            if(found[i])return false;
            found[i]=true;b.registryToken=token;b.registryByteOffset=start;b.registryIdentity=table.identity;
        }
        ++token;
    }
    if(token!=3235)return false;
    for(bool f:found)if(!f)return false;
    return true;
}
bool animation(const assets::OriginalUiMaterialPass50KG& pass,
    game::OriginalUiTextureAnimationConfig50KG& out,Diagnostic& d) {
    const auto* mode=pass.authored.find("TextureAnimation");
    if(!mode||!same(mode->text,"unique"))return fail(d,Error::animation,0x29b950);
    game::OriginalUiTextureAnimationConfig50KG c;assets::OriginalUiPageDiagnostic50KG p;
    std::int32_t looping=1,reverse=0,loopTo=-1;
    if(!assets::originalUiNumber50KG(pass.authored,"animatedMapInitialFrame",0,c.initialFrame,p,0x29b950)||
        !assets::originalUiNumber50KG(pass.authored,"animatedMapFramesPerSecond",1,c.framesPerSecond,p,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLooping",1,looping,p,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapReverseLoopOnEnd",0,reverse,p,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,p,0x29b950))return fail(d,Error::animation);
    c.looping=looping!=0;c.reverseLoopOnEnd=reverse!=0;c.loopOnEndToFrame=loopTo;out=c;return true;
}
game::OriginalUiOverlayQuad50KG quad(const char* material,float x,float y,float w,float h,
    float u0,float v0,float u1,float v1,float depth,std::uint32_t pc) noexcept {
    game::OriginalUiOverlayQuad50KG q;q.materialName=material;q.authoredDepth=depth;q.sourcePC=pc;
    q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};return q;
}
}
struct OriginalUiRiderDrawResources50KG::Storage {
    OriginalUiRiderColdInput50KG owners;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::shared_ptr<assets::OriginalUiScreenAssets50KG> images;
    assets::OriginalUiImageBankInput50KG bankInput;
    game::OriginalUiRiderWidgets50KG widgets;
    std::vector<OriginalUiRiderMaterialBinding50KG> bindings;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,120> text;
    std::array<std::uint32_t,120> widgetMaterial{};
    std::array<OriginalUiRiderBackground50KG,27> backgrounds{};
    std::array<OriginalUiRiderPrompt50KG,4> prompts{};
    std::array<game::OriginalUiExitText50KG,2> exitText;
};
OriginalUiRiderDrawResources50KG::OriginalUiRiderDrawResources50KG() noexcept=default;
OriginalUiRiderDrawResources50KG::~OriginalUiRiderDrawResources50KG()=default;
bool OriginalUiRiderDrawResources50KG::load(const OriginalUiRiderColdInput50KG& in,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyLoaded);
    if(!authentic(in.appStart,Archive::appStart)||!authentic(in.rider,Archive::rider)||
        !authentic(in.fonts,Archive::fonts)||!authentic(in.backgrounds,Archive::backgrounds)||!authentic(in.overlays,Archive::overlays))
        return fail(d,Error::archive);
    if(!validVideo(in.video))return fail(d,Error::videoOwner);
    if(!in.english||!in.english->loaded()||!in.runtime.overrideKnown)return fail(d,Error::input);
    if(bool(in.controllerPopup)!=bool(in.retainedControllerPopup))return fail(d,Error::input,0x2055B0);
    if(in.controllerPopup&&!authentic(in.controllerPopup,Archive::controllerPopup))return fail(d,Error::archive,0x2055B0);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
        if(!s->widgets.bind(in.rider,in.english,d.widgets))return fail(d,Error::widgets);
        if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material);
        s->bindings.reserve(in.retainedControllerPopup?43:37);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        const auto addImages=[&](OriginalUiRiderMaterialBinding50KG& b,const assets::OriginalUiArchive50KG& owner,
            const std::string& name,bool font,bool animated)->bool {
            const auto member=owner.find((font?"font/":"ui/textures/")+name+(animated?".atm":".pst"));
            if(!member.identity||!member.data)return fail(d,Error::resource);
            b.animated=animated;
            if(animated){
                assets::OriginalUiAtm50KG atm;
                if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm))return fail(d,Error::resource);
                for(const auto& frame:atm.frames){
                    const auto identity=imageIdentity(owner.identity()->id,member,frame.pstOffset);
                    if(!identity)return fail(d,Error::resource);
                    b.images.push_back(static_cast<std::uint32_t>(sources.size()));
                    sources.push_back({identity,member,frame.pstOffset,frame.pstBytes,{}});
                }
            }else{
                const auto cel=font?owner.find("font/"+name+".cel"):assets::OriginalUiMemberView50KG{};
                if(font&&(!cel.identity||!cel.data))return fail(d,Error::resource);
                const auto identity=imageIdentity(owner.identity()->id,member,0);if(!identity)return fail(d,Error::resource);
                // Popup Venus14/16 reuse the exact retained font PST/CEL pair
                // already in Rider owner0. Never duplicate its image identity
                // or substitute the Rider/overlay archive's same-name icon.
                std::size_t index=0;
                for(;index<sources.size();++index){const auto& source=sources[index];
                    if(source.identity!=identity)continue;
                    if(source.member.identity!=member.identity||source.member.data!=member.data||
                        source.member.size!=member.size||source.pstOffset||source.pstBytes!=member.size||
                        source.cel.identity!=cel.identity||source.cel.data!=cel.data||source.cel.size!=cel.size)
                        return fail(d,Error::resource);
                    break;
                }
                if(index==sources.size())sources.push_back({identity,member,0,member.size,cel});
                b.images.push_back(static_cast<std::uint32_t>(index));
            }
            if(b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];
            if(pass.textureCount!=1||!same(pass.textureName,name))return fail(d,Error::material);
            if(animated){
                if(!animation(pass,b.animation,d)||!game::validOriginalUiTextureAnimationConfig50KG(b.animation,
                    static_cast<std::uint32_t>(b.images.size())))return fail(d,Error::animation);
            }else if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation);
            return true;
        };
        const auto authored=[&](const char* material,const char* texture,bool font,bool animated)->bool {
            OriginalUiRiderMaterialBinding50KG b;b.registeredName=material;
            if(!assets::readOriginalUiMaterial50KG(*in.rider,b.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
            if(texture&&!addImages(b,font?*in.fonts:*in.rider,texture,font,animated))return false;
            if(!texture&&(b.material.passes.size()!=1||b.material.passes[0].textureCount!=0))return fail(d,Error::material);
            s->bindings.push_back(std::move(b));return true;
        };
        if(!authored("font/denmrk16top","denmrk16",true,false)||
            !authored("font/venus20","venus20",true,false)||
            !authored("font/venus14top","venus14",true,false)||
            !authored("font/venus16top","venus16",true,false)||
            !authored("font/swhutt44","swhutt44",true,false)||
            !authored("font/denmrk14","denmrk14",true,false)||
            !authored("ui/textures/arrwlefttop","arrwleft",false,true)||
            !authored("ui/textures/arrwrghttop","arrwrght",false,true)||
            !authored("ui/textures/notexture",nullptr,false,false))return false;
        {
            OriginalUiRiderMaterialBinding50KG b;b.registeredName="ui/textures/nameblur";
            if(!assets::readOriginalUiRiderNameBlurFallbackMaterial50KG(*in.rider,s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
            if(!addImages(b,*in.rider,"nameblur",false,false))return false;
            s->bindings.push_back(std::move(b));
        }
        constexpr const char* promptNames[]={"icon_x","icon_tri","icon_sqr","icon_cir"};
        for(std::size_t i=0;i<4;++i){
            OriginalUiRiderMaterialBinding50KG b;b.registeredName=std::string("ui/textures/")+promptNames[i];
            if(!assets::readOriginalUiRiderPromptFallbackMaterial50KG(*in.rider,*in.overlays,
                static_cast<assets::OriginalUiMainPrompt50KG>(i),s->templates,assets::OriginalUiTemplate50KG::screenMt,
                in.runtime,b.material,d.material))return fail(d,Error::material);
            if(!addImages(b,i<2?*in.overlays:*in.rider,promptNames[i],false,false))return false;
            s->prompts[i].material=static_cast<std::uint32_t>(s->bindings.size());
            s->bindings.push_back(std::move(b));
        }
        for(std::uint8_t rider=1;rider<=26;++rider){
            if(!riderIndex(rider))continue;
            const auto name=backgroundName(rider);OriginalUiRiderMaterialBinding50KG b;b.registeredName="ui/textures/"+name;
            // UIRIDER supplies every authored %02d_bkg.psm; UIBKG supplies
            // the corresponding image. Preserve that real cross-archive pair.
            if(!assets::readOriginalUiMaterial50KG(*in.rider,b.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
            if(!addImages(b,*in.backgrounds,name,false,false))return false;
            s->backgrounds[rider].rider=rider;s->backgrounds[rider].material=static_cast<std::uint32_t>(s->bindings.size());
            s->bindings.push_back(std::move(b));
        }
        if(in.retainedControllerPopup){
            for(const auto& entry:popupMaterials){
                OriginalUiRiderMaterialBinding50KG b;b.registeredName=entry.name;
                if(!assets::readOriginalUiMaterial50KG(*in.controllerPopup,b.registeredName+".psm",s->templates,
                    assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
                if(entry.texture&&!addImages(b,entry.font?*in.fonts:*in.controllerPopup,entry.texture,entry.font,false))return false;
                if(!entry.texture&&(b.material.passes.size()!=1||b.material.passes[0].textureCount!=0))return fail(d,Error::material);
                s->bindings.push_back(std::move(b));
            }
        }
        const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");
        if(!registry(table,s->bindings))return fail(d,Error::registry,0x298724);
        if(in.retainedControllerPopup)for(const auto& entry:popupMaterials){
            const auto index=find(s->bindings,entry.name);
            if(index==unresolved||s->bindings[index].registryToken!=entry.token||
                s->bindings[index].registryByteOffset!=entry.offset||s->bindings[index].registryIdentity!=table.identity)
                return fail(d,Error::registry,0x298724);
        }
        s->images=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!s->images->load(sources,d.resource))return fail(d,Error::resource);
        const auto* bank=s->images->textures();if(!bank)return fail(d,Error::resource);
        s->samplers.resize(bank->textureCount());std::vector<bool> samplerSet(bank->textureCount(),false);
        for(std::size_t i=0;i<s->bindings.size();++i){
            d.materialIndex=static_cast<std::uint32_t>(i);auto& b=s->bindings[i];const auto& pass=b.material.passes[0];
            if(b.images.empty()){
                if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))return fail(d,Error::material);
                continue;
            }
            for(auto image:b.images){
                d.imageIndex=image;const auto* decoded=s->images->decodedTexture(image);assets::OriginalUiNativeMaterial50KG policy;
                if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))return fail(d,Error::material);
                OriginalUiMaterialSpan50KG converted;
                if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,image,bank->textureCount()},converted))return fail(d,Error::material);
                if(samplerSet[image]&&!sameSampler(s->samplers[image],converted.native.sampler))return fail(d,Error::samplerConflict);
                samplerSet[image]=true;s->samplers[image]=converted.native.sampler;b.policies.push_back(policy);
            }
        }
        for(bool set:samplerSet)if(!set)return fail(d,Error::resource);
        for(std::uint16_t id=1;id<120;++id){
            d.widgetId=id;
            if(const auto* text=s->widgets.textDescriptor(id)){
                const auto m=find(s->bindings,"font/"+text->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];
                if(b.images.size()!=1||!same(b.material.passes[0].textureName,text->fontName))return fail(d,Error::material);
                const auto index=b.images[0];const auto* font=s->images->font(index);const auto* atlas=bank->texture(index);
                if(!font||!atlas)return fail(d,Error::resource);
                const float z=text->depth<0?-text->depth:float(in.video.packetZMaximum)-text->depth;
                if(!std::isfinite(z)||z<0||z>float(in.video.packetZMaximum)||std::floor(z)!=z)return fail(d,Error::geometry);
                if(text->textSource!=assets::OriginalUiTextSource50KG::dynamic){
                    auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                    if(!geometry->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                    s->text[id]=std::move(geometry);
                }
                s->widgetMaterial[id]=m;
            }else if(const auto* icon=s->widgets.iconDescriptor(id)){
                const auto m=find(s->bindings,"ui/textures/"+icon->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];if(b.images.empty())return fail(d,Error::resource);
                // ScreenTexture captures the dimensions of the constructor's
                // first image. ATM selection never resizes its icon geometry.
                if(!s->widgets.prepareIcon(id,s->images,b.images[0],in.video.packetZMaximum,d.widgets))return fail(d,Error::widgets);
                s->widgetMaterial[id]=m;
            }
        }
        for(auto& background:s->backgrounds){
            if(!background.rider)continue;
            const auto& b=s->bindings[background.material];const auto* atlas=bank->texture(b.images[0]);
            if(!atlas||!atlas->texture.width||!atlas->texture.height)return fail(d,Error::resource);
            const float us=1.f/float(atlas->texture.width),vs=1.f/float(atlas->texture.height);
            const float u=319.f*us,v=223.f*vs;const auto* name=b.registeredName.c_str();
            // Native transcription of1E8F7C..1E9414: original source sample
            // endpoints319/223, mirrored 320x224 quadrants and depth15.
            background.quadrants[0]=quad(name,0,0,320,224,0,0,u,v,15,0x1ece68);
            background.quadrants[1]=quad(name,320,0,320,224,u,0,0,v,15,0x1ece68);
            background.quadrants[2]=quad(name,0,224,320,224,0,v,u,0,15,0x1ece68);
            background.quadrants[3]=quad(name,320,224,320,224,u,v,0,0,15,0x1ece68);
        }
        constexpr float promptX[]={484,397,285,160};
        constexpr std::uint32_t promptPC[]={0x1d7a2c,0x1d7a44,0x1d7a5c,0x1d7a74};
        for(std::size_t i=0;i<4;++i){
            auto& p=s->prompts[i];const auto& b=s->bindings[p.material];const auto* atlas=bank->texture(b.images[0]);
            if(!atlas)return fail(d,Error::resource);
            p.mask=static_cast<std::uint8_t>(1u<<i);
            p.quad=quad(b.registeredName.c_str(),promptX[i],378,float(atlas->texture.width),float(atlas->texture.height),0,0,1,1,1,promptPC[i]);
        }
        const auto denmark=find(s->bindings,"font/denmrk16top");
        if(denmark==unresolved||s->bindings[denmark].images.size()!=1)return fail(d,Error::resource);
        for(std::uint8_t player=0;player<2;++player)
            if(s->exitText[player].prepare({5,static_cast<std::uint8_t>(player+1)},in.rider,*in.english,s->images,
                s->bindings[denmark].images[0],d.exitText)!=game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1d6aec);
        s->bankInput.archives={in.appStart,in.rider,in.fonts,in.backgrounds,in.overlays};s->bankInput.owners={s->images};
        if(in.retainedControllerPopup){
            s->bankInput.archives.push_back(in.controllerPopup);
            // Retain the caller's actual root47C. Its checked rejoin verifies
            // all six keys against the existing canonical same-Video history.
            // No constructor, initialization or history reset occurs here.
            if(!in.retainedControllerPopup->prepareOrRejoin(in,s->images,s->bindings,d.controllerPopup))
                return fail(d,Error::resource,d.controllerPopup.sourcePC);
        }
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiRiderDrawResources50KG::loaded() const noexcept {return bool(storage_);}
std::shared_ptr<OriginalUiControllerPopupResources50KG> OriginalUiRiderDrawResources50KG::controllerPopup() const noexcept {
    return storage_?storage_->owners.retainedControllerPopup:nullptr;
}
game::OriginalUiRiderWidgets50KG* OriginalUiRiderDrawResources50KG::widgets() noexcept {return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiRiderWidgets50KG* OriginalUiRiderDrawResources50KG::widgets() const noexcept {return storage_?&storage_->widgets:nullptr;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiRiderDrawResources50KG::images() const noexcept {return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiRiderDrawResources50KG::bankInput() const noexcept {return storage_?&storage_->bankInput:nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiRiderDrawResources50KG::textures() const noexcept {return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiRiderDrawResources50KG::samplers() const noexcept {return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiRiderMaterialBinding50KG>* OriginalUiRiderDrawResources50KG::materials() const noexcept {return storage_?&storage_->bindings:nullptr;}
const OriginalUiRiderVideoOwner50KG* OriginalUiRiderDrawResources50KG::video() const noexcept {return storage_?&storage_->owners.video:nullptr;}
std::uint32_t OriginalUiRiderDrawResources50KG::widgetMaterial(std::uint16_t id) const noexcept {return storage_&&id<120?storage_->widgetMaterial[id]:unresolved;}
std::uint32_t OriginalUiRiderDrawResources50KG::materialIndex(const char* name) const noexcept {
    if(!storage_||!name)return unresolved;
    for(std::size_t i=0;i<storage_->bindings.size();++i){
        const auto& a=storage_->bindings[i].registeredName;std::size_t j=0;
        for(;j<a.size()&&name[j]&&fold(a[j])==fold(name[j]);++j){}
        if(j==a.size()&&!name[j])return static_cast<std::uint32_t>(i);
    }
    return unresolved;
}
const game::OriginalUiTextGeometry50KG* OriginalUiRiderDrawResources50KG::textGeometry(std::uint16_t id) const noexcept {
    return storage_&&id<120?storage_->text[id].get():nullptr;
}
const assets::OriginalUiFont50KG* OriginalUiRiderDrawResources50KG::textFont(std::uint16_t id) const noexcept {
    if(!storage_||!storage_->widgets.textDescriptor(id))return nullptr;
    const auto m=widgetMaterial(id);if(m>=storage_->bindings.size()||storage_->bindings[m].images.size()!=1)return nullptr;
    return storage_->images->font(storage_->bindings[m].images[0]);
}
const assets::Texture48B* OriginalUiRiderDrawResources50KG::textAtlas(std::uint16_t id) const noexcept {
    if(!textFont(id))return nullptr;
    const auto* image=textures()->texture(storage_->bindings[widgetMaterial(id)].images[0]);return image?&image->texture:nullptr;
}
const OriginalUiRiderBackground50KG* OriginalUiRiderDrawResources50KG::background(std::uint8_t rider) const noexcept {
    return storage_&&riderIndex(rider)?&storage_->backgrounds[rider]:nullptr;
}
const std::array<OriginalUiRiderPrompt50KG,4>* OriginalUiRiderDrawResources50KG::prompts() const noexcept {return storage_?&storage_->prompts:nullptr;}
game::OriginalUiExitText50KG* OriginalUiRiderDrawResources50KG::exitText(std::uint8_t player) noexcept {
    return storage_&&player<2?&storage_->exitText[player]:nullptr;
}
bool OriginalUiRiderDrawResources50KG::textPacketDepth(std::uint16_t id,float& out) const noexcept {
    if(!storage_)return false;const auto* text=storage_->widgets.textDescriptor(id);if(!text)return false;
    const float z=text->depth<0?-text->depth:float(storage_->owners.video.packetZMaximum)-text->depth;
    if(!std::isfinite(z)||z<0||z>float(storage_->owners.video.packetZMaximum)||std::floor(z)!=z)return false;
    out=z;return true;
}
}
