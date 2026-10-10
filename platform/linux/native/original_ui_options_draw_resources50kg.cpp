#include "original_ui_options_draw_resources50kg.h"
#include "original_ui_options_particle_adapter50kg.h"
#include <cmath>
#include <utility>
namespace rr::platform {
namespace {
using Error=OriginalUiOptionsDrawError50KG;
using Archive=assets::OriginalUiArchiveId50KG;
using Diagnostic=OriginalUiOptionsDrawDiagnostic50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
char fold(char c) noexcept {return c=='\\'?'/' : c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
bool archive(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool validVideo(const OriginalUiOptionsVideoOwner50KG& v) noexcept {
    return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;
}
std::uint64_t imageIdentity(Archive id,const assets::OriginalUiMemberView50KG& member,std::size_t offset) noexcept {
    const auto* owner=assets::originalUiArchiveIdentity50KG(id);if(!owner||offset>0xffffffffu)return 0;
    for(std::size_t i=0;i<owner->memberCount;++i)if(assets::originalUiMemberIdentity50KG(id,i)==member.identity)
        return (std::uint64_t(static_cast<std::uint8_t>(id)+1u)<<56)|(std::uint64_t(i+1u)<<32)|std::uint64_t(offset);
    return 0;
}
struct Image {const char* name;Archive archive;const char* member;const char* cel;bool atm;};
constexpr Image imageEntries[]={
    {"denmrk16",Archive::fonts,"font/denmrk16.pst","font/denmrk16.cel",false},
    {"venus16",Archive::fonts,"font/venus16.pst","font/venus16.cel",false},
    {"overlay1",Archive::controllerPopup,"ui/textures/overlay1.pst",nullptr,false},
    {"mmbkg",Archive::options,"ui/textures/mmbkg.pst",nullptr,false},
    {"arrwrght",Archive::options,"ui/textures/arrwrght.atm",nullptr,true},
    {"arrwleft",Archive::options,"ui/textures/arrwleft.atm",nullptr,true},
    {"icon_x",Archive::overlays,"ui/textures/icon_x.pst",nullptr,false},
    {"icon_tri",Archive::overlays,"ui/textures/icon_tri.pst",nullptr,false},
    {"mmprt",Archive::options,"particle/mmprt.pst",nullptr,false},
    {"icon_sqr",Archive::options,"ui/textures/icon_sqr.pst",nullptr,false}
};
struct Material {const char* name;const char* texture;std::uint32_t token;std::size_t offset;};
// Actual APPSTART registry order, projected to live Options menu materials.
constexpr Material materialEntries[]={
    {"ui/textures/mmbkg","mmbkg",2956,65593},
    {"ui/textures/overlay1","overlay1",2962,65677},
    {"ui/textures/arrwleft","arrwleft",2996,66421},
    {"ui/textures/arrwrght","arrwrght",2999,66491},
    {"font/venus16","venus16",3006,66635},
    {"ui/textures/notexture",nullptr,3155,69494},
    {"font/venus16top","venus16",3178,69882},
    {"font/denmrk16top","denmrk16",3180,69917},
    {"ui/textures/icon_sqr","icon_sqr",3215,70521},
    {"ui/textures/icon_tri","icon_tri",3216,70543},
    {"ui/textures/icon_x","icon_x",3217,70565}
};
const assets::OriginalUiArchive50KG& owner(const OriginalUiOptionsColdInput50KG& in,Archive id) noexcept {
    return *(id==Archive::fonts?in.fonts:id==Archive::controllerPopup?in.controllerPopup:
        id==Archive::overlays?in.overlays:in.options);
}
std::uint32_t find(const std::vector<OriginalUiOptionsMaterialBinding50KG>& list,const std::string& name) noexcept {
    for(std::size_t i=0;i<list.size();++i)if(same(list[i].registeredName,name))return static_cast<std::uint32_t>(i);
    return unresolved;
}
bool registry(const assets::OriginalUiMemberView50KG& table) noexcept {
    if(!table.identity||!table.data||table.size!=70962)return false;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::size_t cursor=0,matched=0;std::uint32_t token=0;bool particle=false;
    while(cursor<table.size) {
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        if(token==2902) {
            constexpr const char* name="particle/mmprt";std::size_t j=0;
            if(start!=64764)return false;
            for(;name[j];++j)if(start+j>=cursor||fold(char(table.data[start+j]))!=name[j])return false;
            if(start+j!=cursor)return false;particle=true;
        }
        if(matched<11&&token==materialEntries[matched].token) {
            const auto& m=materialEntries[matched];if(start!=m.offset)return false;
            std::size_t j=0;
            for(;m.name[j];++j)if(start+j>=cursor||fold(char(table.data[start+j]))!=m.name[j])return false;
            if(start+j!=cursor)return false;++matched;
        }
        ++token;
    }
    return particle&&matched==11&&token==3235;
}
bool animation(const assets::OriginalUiMaterialPass50KG& pass,
    game::OriginalUiTextureAnimationConfig50KG& out,Diagnostic& d) {
    const auto* mode=pass.authored.find("TextureAnimation");
    if(!mode||!same(mode->text,"unique"))return fail(d,Error::animation,0x29B950);
    game::OriginalUiTextureAnimationConfig50KG c;assets::OriginalUiPageDiagnostic50KG p;
    std::int32_t looping=1,reverse=0,loopTo=-1;
    if(!assets::originalUiNumber50KG(pass.authored,"animatedMapInitialFrame",0,c.initialFrame,p,0x29B950)||
        !assets::originalUiNumber50KG(pass.authored,"animatedMapFramesPerSecond",1,c.framesPerSecond,p,0x29B950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLooping",1,looping,p,0x29B950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapReverseLoopOnEnd",0,reverse,p,0x29B950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,p,0x29B950))return fail(d,Error::animation);
    c.looping=looping!=0;c.reverseLoopOnEnd=reverse!=0;c.loopOnEndToFrame=loopTo;out=c;return true;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
}
struct OriginalUiOptionsDrawResources50KG::Storage {
    OriginalUiOptionsColdInput50KG owners;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::shared_ptr<assets::OriginalUiScreenAssets50KG> images;
    game::OriginalUiOptionsMenuWidgets50KG widgets;
    std::vector<OriginalUiOptionsMaterialBinding50KG> bindings;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,168> text;
    std::array<std::uint32_t,168> widgetMaterial{};
    game::OriginalUiOptionsBackground50KG background,gameBackground;
    std::array<std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,102>,4> volumeText;
    game::OriginalUiOptionsCamera50KG camera;
    game::OriginalUiExitText50KG exitText,gameExitText;
    game::OriginalUiMainSceneOwner50KG scene;
    game::OriginalUiMainFireOwner50KG fire;
    game::OriginalUiMainParticleLighting50KG lighting;
    game::OriginalUiOptionsParticleRenderState50KG renderState;
    assets::OriginalUiMainParticleMaterial50KG particle;
    std::uint32_t particleImage=unresolved;
};
OriginalUiOptionsDrawResources50KG::OriginalUiOptionsDrawResources50KG() noexcept=default;
OriginalUiOptionsDrawResources50KG::~OriginalUiOptionsDrawResources50KG()=default;
bool OriginalUiOptionsDrawResources50KG::load(const OriginalUiOptionsColdInput50KG& in,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyLoaded);
    if(!archive(in.appStart,Archive::appStart)||!archive(in.options,Archive::options)||!archive(in.fonts,Archive::fonts)||
        !archive(in.controllerPopup,Archive::controllerPopup)||!archive(in.overlays,Archive::overlays))return fail(d,Error::archive);
    if(!validVideo(in.video))return fail(d,Error::videoOwner);
    if(!in.runtime.overrideKnown)return fail(d,Error::material);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
        if(s->widgets.bind(in.options,in.english,d.widgets)!=game::OriginalUiOptionsWidgetStatus50KG::ok)return fail(d,Error::widgets);
        if(game::prepareOriginalUiOptionsCamera50KG(*in.options,s->camera,d.camera)!=game::OriginalUiOptionsCameraStatus50KG::ok)
            return fail(d,Error::geometry,0x1D08B8);
        if(!s->scene.prepareOptionsMenu(*in.options,d.scene))return fail(d,Error::resource,0x1D06B4);
        d.fire=s->fire.initializeOptionsMenu(s->scene);
        d.lighting=game::prepareOriginalUiOptionsParticleLighting50KG(s->scene,s->lighting);
        if(d.fire!=game::OriginalUiMainFireStatus50KG::complete||d.lighting!=game::OriginalUiMainParticleLightingStatus50KG::complete)
            return fail(d,Error::geometry);
        d.renderState=game::prepareOriginalUiOptionsParticleRenderState50KG(s->scene,s->camera,s->lighting,s->renderState);
        if(d.renderState!=game::OriginalUiOptionsParticleRenderStateStatus50KG::complete)return fail(d,Error::geometry);
        if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material);
        const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");if(!registry(table))return fail(d,Error::registry,0x298724);
        std::vector<assets::OriginalUiImageSource50KG> sources;std::array<std::vector<std::uint32_t>,10> indices;
        for(std::size_t i=0;i<10;++i) {
            const auto& entry=imageEntries[i];const auto& sourceOwner=owner(in,entry.archive);const auto member=sourceOwner.find(entry.member);
            if(!member.identity)return fail(d,Error::resource);
            if(entry.atm) {
                assets::OriginalUiAtm50KG atm;if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm))return fail(d,Error::resource);
                for(const auto& frame:atm.frames) {
                    const auto identity=imageIdentity(entry.archive,member,frame.pstOffset);if(!identity)return fail(d,Error::resource);
                    indices[i].push_back(static_cast<std::uint32_t>(sources.size()));sources.push_back({identity,member,frame.pstOffset,frame.pstBytes,{}});
                }
            } else {
                const auto cel=entry.cel?sourceOwner.find(entry.cel):assets::OriginalUiMemberView50KG{};
                if(entry.cel&&!cel.identity)return fail(d,Error::resource);
                const auto identity=imageIdentity(entry.archive,member,0);if(!identity)return fail(d,Error::resource);
                indices[i].push_back(static_cast<std::uint32_t>(sources.size()));sources.push_back({identity,member,0,member.size,cel});
            }
        }
        s->images=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!s->images->load(sources,d.resource))return fail(d,Error::resource);
        const auto* bank=s->images->textures();s->samplers.resize(bank->textureCount());std::vector<bool> samplerSet(bank->textureCount(),false);
        for(std::size_t i=0;i<11;++i) {
            d.materialIndex=static_cast<std::uint32_t>(i);const auto& entry=materialEntries[i];OriginalUiOptionsMaterialBinding50KG b;
            b.registeredName=entry.name;b.registryToken=entry.token;b.registryIdentity=table.identity;
            const bool prompt=i>=8;bool read=false;
            if(i==8)read=assets::readOriginalUiGameOptionsPromptFallbackMaterial50KG(*in.options,*in.overlays,
                assets::OriginalUiMainPrompt50KG::square,s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            else if(prompt)read=assets::readOriginalUiOptionsMenuPromptFallbackMaterial50KG(*in.options,*in.overlays,
                i==9?assets::OriginalUiOptionsMenuPrompt50KG::triangle:assets::OriginalUiOptionsMenuPrompt50KG::cross,
                s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            else read=assets::readOriginalUiMaterial50KG(*in.options,b.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            if(!read||b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];
            if(entry.texture) {
                if(pass.textureCount!=1||!same(pass.textureName,entry.texture))return fail(d,Error::material);
                std::size_t image=0;for(;image<10;++image)if(same(imageEntries[image].name,entry.texture))break;
                if(image==10)return fail(d,Error::resource);b.images=indices[image];b.animated=imageEntries[image].atm;
                if(b.animated) {
                    if(!animation(pass,b.animation,d)||!game::validOriginalUiTextureAnimationConfig50KG(b.animation,
                        static_cast<std::uint32_t>(b.images.size())))return fail(d,Error::animation);
                } else if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation);
                for(auto index:b.images) {
                    d.imageIndex=index;assets::OriginalUiNativeMaterial50KG policy;const auto* decoded=s->images->decodedTexture(index);
                    if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))return fail(d,Error::material);
                    OriginalUiMaterialSpan50KG converted;
                    if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,index,bank->textureCount()},converted))return fail(d,Error::material);
                    if(samplerSet[index]&&!sameSampler(s->samplers[index],converted.native.sampler))return fail(d,Error::samplerConflict);
                    samplerSet[index]=true;s->samplers[index]=converted.native.sampler;b.policies.push_back(policy);
                }
            } else if(pass.textureCount!=0||!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))
                return fail(d,Error::material);
            s->bindings.push_back(std::move(b));
        }
        s->particleImage=indices[8][0];
        if(!assets::prepareOriginalUiOptionsParticleMaterial50KG(*in.options,s->templates,in.runtime,
            *s->images->decodedTexture(s->particleImage),s->particle,d.material))return fail(d,Error::material);
        if(!s->scene.particleMaterial()||s->particle.identity!=s->scene.particleMaterial()->identity())return fail(d,Error::material);
        for(std::uint8_t state=0;state<2;++state) {
            OriginalUiMaterialSpan50KG converted;
            if(!convertOriginalUiOptionsParticleSpan50KG(s->particle,state,{0,1,1,s->particleImage,bank->textureCount()},converted))
                return fail(d,Error::material);
            if(state&&!sameSampler(s->samplers[s->particleImage],converted.native.sampler))return fail(d,Error::samplerConflict);
            s->samplers[s->particleImage]=converted.native.sampler;
        }
        samplerSet[s->particleImage]=true;
        for(bool set:samplerSet)if(!set)return fail(d,Error::resource);
        for(auto id:game::originalUiOptionsManagerWidgetIds50KG) {
            d.widgetId=id;
            if(const auto* text=s->widgets.textDescriptor(id)) {
                const auto m=find(s->bindings,"font/"+text->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];if(b.images.size()!=1||!same(b.material.passes[0].textureName,text->fontName))return fail(d,Error::material);
                const auto index=b.images[0];const auto* font=s->images->font(index);const auto* atlas=bank->texture(index);
                if(!font||!atlas)return fail(d,Error::resource);
                if(text->depth!=1)return fail(d,Error::geometry);
                if(text->textSource==assets::OriginalUiTextSource50KG::dynamic){
                    const auto ordinal=id==38?0:id==112?1:id==114?2:id==165?3:4;
                    if(ordinal==4)return fail(d,Error::geometry);
                    for(std::uint32_t value=0;value<=101;++value){
                        std::vector<std::string> live{value==101?std::string():std::to_string(value)+"%"};
                        auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                        if(!geometry->prepareDynamic(*text,live,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                        s->volumeText[ordinal][value]=std::move(geometry);
                    }
                }else{
                    auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                    if(!geometry->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                    s->text[id]=std::move(geometry);
                }
                s->widgetMaterial[id]=m;
            } else if(const auto* icon=s->widgets.iconDescriptor(id)) {
                const auto m=find(s->bindings,"ui/textures/"+icon->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];if(b.images.size()!=1||b.animated)return fail(d,Error::material);
                if(s->widgets.prepareIcon(id,s->images,b.images[0],in.video.packetZMaximum,d.widgets)!=game::OriginalUiOptionsWidgetStatus50KG::ok)
                    return fail(d,Error::widgets);
                if(icon->depth!=1)return fail(d,Error::geometry);s->widgetMaterial[id]=m;
            }
        }
        const auto bg=find(s->bindings,"ui/textures/mmbkg");const auto* atlas=bank->texture(s->bindings[bg].images[0]);
        if(!atlas||!game::layoutOriginalUiOptionsBackground50KG(atlas->texture.width,atlas->texture.height,s->background))return fail(d,Error::geometry);
        if(!game::layoutOriginalUiGameOptionsBackground50KG(atlas->texture.width,atlas->texture.height,s->gameBackground))return fail(d,Error::geometry);
        if(!in.english||s->exitText.prepare({12,1},in.options,*in.english,s->images,indices[0][0],d.exitText)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        if(s->gameExitText.prepare({12,4},in.options,*in.english,s->images,indices[0][0],d.exitText)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        storage_=std::move(s);d={};return true;
    } catch(...) {return fail(d,Error::allocation);}
}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::exitText() noexcept {return storage_?&storage_->exitText:nullptr;}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::gameOptionsExitText() noexcept {return storage_?&storage_->gameExitText:nullptr;}
bool OriginalUiOptionsDrawResources50KG::loaded() const noexcept {return bool(storage_);}
game::OriginalUiOptionsMenuWidgets50KG* OriginalUiOptionsDrawResources50KG::widgets() noexcept {return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiOptionsMenuWidgets50KG* OriginalUiOptionsDrawResources50KG::widgets() const noexcept {return storage_?&storage_->widgets:nullptr;}
const assets::OriginalUiScreenAssets50KG* OriginalUiOptionsDrawResources50KG::images() const noexcept {return storage_?storage_->images.get():nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiOptionsDrawResources50KG::textures() const noexcept {return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiOptionsDrawResources50KG::samplers() const noexcept {return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiOptionsMaterialBinding50KG>* OriginalUiOptionsDrawResources50KG::materials() const noexcept {return storage_?&storage_->bindings:nullptr;}
const OriginalUiOptionsVideoOwner50KG* OriginalUiOptionsDrawResources50KG::video() const noexcept {return storage_?&storage_->owners.video:nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiOptionsDrawResources50KG::textGeometry(std::uint16_t id) const noexcept {
    if(!storage_||id>=168)return nullptr;
    const auto ordinal=id==38?0:id==112?1:id==114?2:id==165?3:4;
    if(ordinal<4){const auto value=storage_->widgets.gameOptionsRenderedVolume(id);
        return storage_->volumeText[ordinal][value<0?101:static_cast<std::size_t>(value)].get();}
    return storage_->text[id].get();
}
std::uint32_t OriginalUiOptionsDrawResources50KG::widgetMaterial(std::uint16_t id) const noexcept {return storage_&&id<168?storage_->widgetMaterial[id]:unresolved;}
std::uint32_t OriginalUiOptionsDrawResources50KG::materialIndex(const char* name) const noexcept {
    if(!storage_||!name)return unresolved;
    for(std::size_t i=0;i<storage_->bindings.size();++i) {
        const auto& value=storage_->bindings[i].registeredName;std::size_t j=0;
        for(;j<value.size()&&name[j]&&fold(value[j])==fold(name[j]);++j){}
        if(j==value.size()&&!name[j])return static_cast<std::uint32_t>(i);
    }
    return unresolved;
}
const game::OriginalUiOptionsBackground50KG* OriginalUiOptionsDrawResources50KG::background() const noexcept {return storage_?&storage_->background:nullptr;}
const game::OriginalUiOptionsBackground50KG* OriginalUiOptionsDrawResources50KG::gameOptionsBackground() const noexcept {return storage_?&storage_->gameBackground:nullptr;}
const game::OriginalUiOptionsCamera50KG* OriginalUiOptionsDrawResources50KG::camera() const noexcept {return storage_?&storage_->camera:nullptr;}
const game::OriginalUiMainSceneOwner50KG* OriginalUiOptionsDrawResources50KG::scene() const noexcept {return storage_?&storage_->scene:nullptr;}
game::OriginalUiMainFireOwner50KG* OriginalUiOptionsDrawResources50KG::fire() noexcept {return storage_?&storage_->fire:nullptr;}
const game::OriginalUiMainFireOwner50KG* OriginalUiOptionsDrawResources50KG::fire() const noexcept {return storage_?&storage_->fire:nullptr;}
const game::OriginalUiOptionsParticleRenderState50KG* OriginalUiOptionsDrawResources50KG::particleRenderState() const noexcept {return storage_?&storage_->renderState:nullptr;}
const assets::OriginalUiMainParticleMaterial50KG* OriginalUiOptionsDrawResources50KG::particleMaterial() const noexcept {return storage_?&storage_->particle:nullptr;}
std::uint32_t OriginalUiOptionsDrawResources50KG::particleImageIndex() const noexcept {return storage_?storage_->particleImage:unresolved;}
bool OriginalUiOptionsDrawResources50KG::selection(game::OriginalUiOptionsSelection50KG& output,Diagnostic& d) const noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);
    const auto slot=storage_->widgets.navigation()->groups[1].selectedSlot;const auto id=game::originalUiOptionsSelectedText50KG(slot);
    const auto* geometry=textGeometry(id);if(!geometry||geometry->lines().empty())return fail(d,Error::geometry);
    const auto right=materialIndex("ui/textures/arrwrght"),left=materialIndex("ui/textures/arrwleft");
    if(right==unresolved||left==unresolved)return fail(d,Error::material);
    const auto* r=textures()->texture(storage_->bindings[right].images[0]);const auto* l=textures()->texture(storage_->bindings[left].images[0]);
    if(!r||!l)return fail(d,Error::resource);
    const auto& text=geometry->lines()[0];
    if(!game::layoutOriginalUiOptionsSelection50KG({slot,text.position.positionedX,text.position.positionedY,text.measuredWidth,
        r->texture.width,r->texture.height,l->texture.width,l->texture.height},output))return fail(d,Error::geometry);
    return true;
}
bool OriginalUiOptionsDrawResources50KG::gameOptionsSelection(game::OriginalUiOptionsSelection50KG& output,Diagnostic& d) const noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);
    const auto id=storage_->widgets.gameOptionsSelectedValueWidget();const auto* geometry=textGeometry(id);
    if(!id||!geometry||geometry->lines().empty())return fail(d,Error::geometry);
    const auto right=materialIndex("ui/textures/arrwrght"),left=materialIndex("ui/textures/arrwleft");
    if(right==unresolved||left==unresolved)return fail(d,Error::material);
    const auto* r=textures()->texture(storage_->bindings[right].images[0]);const auto* l=textures()->texture(storage_->bindings[left].images[0]);
    if(!r||!l)return fail(d,Error::resource);const auto& t=geometry->lines()[0];
    if(!game::layoutOriginalUiGameOptionsSelection50KG(id,{0,t.position.positionedX,t.position.positionedY,t.measuredWidth,
        r->texture.width,r->texture.height,l->texture.width,l->texture.height},output))return fail(d,Error::geometry);
    return true;
}
}
