#include "original_ui_options_draw_resources50kg.h"
#include "original_ui_options_particle_adapter50kg.h"
#include "original_ui_main_session50kg.h"
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
    {"icon_sqr",Archive::options,"ui/textures/icon_sqr.pst",nullptr,false},
    {"denmrk14",Archive::fonts,"font/denmrk14.pst","font/denmrk14.cel",false},
    {"denmrk12",Archive::fonts,"font/denmrk12.pst","font/denmrk12.cel",false},
    {"ps2_line1",Archive::options,"ui/textures/ps2_line1.pst",nullptr,false},
    {"ps2_line2",Archive::options,"ui/textures/ps2_line2.pst",nullptr,false},
    {"ps2_line3",Archive::options,"ui/textures/ps2_line3.pst",nullptr,false},
    {"ps2_line4",Archive::options,"ui/textures/ps2_line4.pst",nullptr,false},
    {"nameblur",Archive::options,"ui/textures/nameblur.pst",nullptr,false},
    {"icon_cir",Archive::options,"ui/textures/icon_cir.pst",nullptr,false},
    {"ps2_blue",Archive::options,"ui/textures/ps2_blue.pst",nullptr,false},
    {"ps2_bluedark",Archive::options,"ui/textures/ps2_bluedark.pst",nullptr,false},
    {"ps2_bluelight",Archive::options,"ui/textures/ps2_bluelight.pst",nullptr,false},
    {"ps2_buttons",Archive::options,"ui/textures/ps2_buttons.pst",nullptr,false},
    {"specmap3",Archive::options,"ui/textures/specmap3.tga",nullptr,false},
    {"denmrk14credits",Archive::fonts,"font/denmrk14credits.pst","font/denmrk14credits.cel",false},
    {"logorain",Archive::options,"ui/textures/logorain.pst",nullptr,false},
    {"logolec",Archive::options,"ui/textures/logolec.pst",nullptr,false},
    {"venus14",Archive::fonts,"font/venus14.pst","font/venus14.cel",false},
    {"popup_icon_x",Archive::controllerPopup,"ui/textures/icon_x.pst",nullptr,false},
    {"popup_icon_tri",Archive::controllerPopup,"ui/textures/icon_tri.pst",nullptr,false}
};
struct Material {const char* name;const char* texture;std::uint32_t token;std::size_t offset;Archive archive=Archive::options;const char* image=nullptr;};
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
    {"ui/textures/icon_x","icon_x",3217,70565},
    {"ui/textures/ps2_line1","ps2_line1",2992,66329},
    {"ui/textures/ps2_line2","ps2_line2",2993,66352},
    {"ui/textures/ps2_line3","ps2_line3",2994,66375},
    {"ui/textures/ps2_line4","ps2_line4",2995,66398},
    {"font/denmrk12","denmrk12",3008,66663},
    {"font/denmrk14","denmrk14",3011,66707},
    {"ui/textures/nameblur","nameblur",3175,69816},
    {"ui/textures/arrwlefttop","arrwleft",3210,70395},
    {"ui/textures/arrwrghttop","arrwrght",3212,70447},
    {"ui/textures/icon_cir","icon_cir",3214,70499},
    {"font/denmrk14credits","denmrk14credits",3012,66722},
    {"ui/textures/logorain","logorain",3003,66582},
    {"ui/textures/logolec","logolec",3002,66561},
    {"ui/textures/dimpopup",nullptr,3228,70804,Archive::controllerPopup},
    {"ui/textures/overlay1popup","overlay1",3229,70826,Archive::controllerPopup},
    {"font/venus14popup","venus14",3231,70872,Archive::controllerPopup},
    {"ui/textures/icon_x_popup","icon_x",3232,70891,Archive::controllerPopup,"popup_icon_x"},
    {"ui/textures/icon_tri_popup","icon_tri",3233,70917,Archive::controllerPopup,"popup_icon_tri"},
    {"ui/textures/overlay1top","overlay1",3176,69840}
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
        for(const auto& m:materialEntries)if(token==m.token) {
            if(start!=m.offset)return false;
            std::size_t j=0;
            for(;m.name[j];++j)if(start+j>=cursor||fold(char(table.data[start+j]))!=m.name[j])return false;
            if(start+j!=cursor)return false;++matched;
        }
        ++token;
    }
    return particle&&matched==sizeof(materialEntries)/sizeof(materialEntries[0])&&token==3235;
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
    std::shared_ptr<OriginalUiControllerPopupResources50KG> controllerPopup;
    assets::OriginalUiMaterialTemplates50KG templates;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
    assets::OriginalUiImageBank50KG imageBank;
    game::OriginalUiOptionsMenuWidgets50KG widgets;
    std::vector<OriginalUiOptionsMaterialBinding50KG> bindings;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,game::originalUiOptionsWidgetStorageCount50KG> text;
    std::array<std::uint32_t,game::originalUiOptionsWidgetStorageCount50KG> widgetMaterial{};
    game::OriginalUiOptionsBackground50KG background,gameBackground,controlsBackground,creditsBackground;
    std::array<game::OriginalUiOverlayQuad50KG,4> creditsLogos;
    struct LocalizedGeometry {std::uint16_t stringId=0;std::unique_ptr<game::OriginalUiTextGeometry50KG> geometry;};
    std::array<std::vector<LocalizedGeometry>,game::originalUiOptionsWidgetStorageCount50KG> localizedText;
    std::array<std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,102>,4> volumeText;
    game::OriginalUiOptionsCamera50KG camera;
    game::OriginalUiExitText50KG exitText,gameExitText,creditsExitText,memoryCardExitText;
    OriginalUiMemoryCardResources50KG memoryCard;
    std::array<game::OriginalUiExitText50KG,2> controlsExitText;
    game::OriginalUiMainSceneOwner50KG scene;
    game::OriginalUiControlsModel50KG controlsModel;
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
    if(!in.application||!in.application->known||!in.lightManager)return fail(d,Error::input);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
        if(s->widgets.bind(in.options,in.english,d.widgets)!=game::OriginalUiOptionsWidgetStatus50KG::ok)return fail(d,Error::widgets);
        if(game::prepareOriginalUiOptionsCamera50KG(*in.options,s->camera,d.camera)!=game::OriginalUiOptionsCameraStatus50KG::ok)
            return fail(d,Error::geometry,0x1D08B8);
        if(!s->scene.prepareOptionsMenu(*in.options,d.scene))return fail(d,Error::resource,0x1D06B4);
        if(!s->controlsModel.prepare(in.options,s->scene,d.controlsModel))return fail(d,Error::geometry,d.controlsModel.sourcePC);
        d.fire=s->fire.initializeOptionsMenu(s->scene);
        d.lighting=game::prepareOriginalUiOptionsParticleLighting50KG(s->scene,s->lighting);
        if(d.fire!=game::OriginalUiMainFireStatus50KG::complete||d.lighting!=game::OriginalUiMainParticleLightingStatus50KG::complete)
            return fail(d,Error::geometry);
        d.renderState=game::prepareOriginalUiOptionsParticleRenderState50KG(s->scene,s->camera,s->lighting,s->renderState);
        if(d.renderState!=game::OriginalUiOptionsParticleRenderStateStatus50KG::complete)return fail(d,Error::geometry);
        if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material);
        const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");if(!registry(table))return fail(d,Error::registry,0x298724);
        std::vector<assets::OriginalUiImageSource50KG> sources;std::array<std::vector<std::uint32_t>,sizeof(imageEntries)/sizeof(imageEntries[0])> indices;
        for(std::size_t i=0;i<indices.size();++i) {
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
        auto originalImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!originalImages->load(sources,d.resource))return fail(d,Error::resource);
        assets::OriginalUiImageBankInput50KG bankInput;bankInput.archives={in.appStart,in.options,in.fonts,in.controllerPopup,in.overlays};bankInput.owners={originalImages};
        assets::OriginalUiImageBankDiagnostic50KG bankDiagnostic;
        if(!s->imageBank.load(bankInput,bankDiagnostic))return fail(d,Error::resource);
        // One source bank: deduplication cannot reorder or alias these distinct
        // authenticated member/frame ranges. All projection owners use it.
        for(std::uint32_t i=0;i<sources.size();++i){const auto* map=s->imageBank.mapping(0,i);
            if(!map||map->combinedIndex!=i)return fail(d,Error::resource);}
        s->images=s->imageBank.images();
        const auto* bank=s->images->textures();s->samplers.resize(bank->textureCount());std::vector<bool> samplerSet(bank->textureCount(),false);
        for(std::size_t i=0;i<sizeof(materialEntries)/sizeof(materialEntries[0]);++i) {
            d.materialIndex=static_cast<std::uint32_t>(i);const auto& entry=materialEntries[i];OriginalUiOptionsMaterialBinding50KG b;
            b.registeredName=entry.name;b.registryToken=entry.token;b.registryIdentity=table.identity;
            const bool prompt=i>=8&&i<=10;bool read=false;
            if(i==8)read=assets::readOriginalUiGameOptionsPromptFallbackMaterial50KG(*in.options,*in.overlays,
                assets::OriginalUiMainPrompt50KG::square,s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            else if(prompt)read=assets::readOriginalUiOptionsMenuPromptFallbackMaterial50KG(*in.options,*in.overlays,
                i==9?assets::OriginalUiOptionsMenuPrompt50KG::triangle:assets::OriginalUiOptionsMenuPrompt50KG::cross,
                s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            else if(i==15||i==17||i==20)read=assets::readOriginalUiControlsFallbackMaterial50KG(*in.options,*in.fonts,
                i==15?assets::OriginalUiControlsFallback50KG::smallFont:i==17?assets::OriginalUiControlsFallback50KG::nameBlur:assets::OriginalUiControlsFallback50KG::circle,
                s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            else read=assets::readOriginalUiMaterial50KG(owner(in,entry.archive),b.registeredName+".psm",s->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material);
            if(!read||b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];
            if(entry.texture) {
                if(pass.textureCount!=1||!same(pass.textureName,entry.texture))return fail(d,Error::material);
                std::size_t image=0;for(;image<indices.size();++image)if(same(imageEntries[image].name,entry.image?entry.image:entry.texture))break;
                if(image==indices.size())return fail(d,Error::resource);b.images=indices[image];b.animated=imageEntries[image].atm;
                if(b.animated) {
                    if(!animation(pass,b.animation,d)||!game::validOriginalUiTextureAnimationConfig50KG(b.animation,
                        static_cast<std::uint32_t>(b.images.size())))return fail(d,Error::animation);
                } else if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation);
                for(auto index:b.images) {
                    d.imageIndex=index;assets::OriginalUiNativeMaterial50KG policy;const auto* decoded=s->images->decodedTexture(index);
                    if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))return fail(d,Error::material);
                    if(b.animated){const auto* first=bank->texture(b.images[0]);const auto* current=bank->texture(index);
                        if(!first||!current||first->texture.width!=current->texture.width||first->texture.height!=current->texture.height)
                            return fail(d,Error::geometry);}
                    OriginalUiMaterialSpan50KG converted;
                    if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,index,bank->textureCount()},converted))return fail(d,Error::material);
                    if(samplerSet[index]&&!sameSampler(s->samplers[index],converted.native.sampler))return fail(d,Error::samplerConflict);
                    samplerSet[index]=true;s->samplers[index]=converted.native.sampler;b.policies.push_back(policy);
                }
            } else if(pass.textureCount!=0||!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))
                return fail(d,Error::material);
            s->bindings.push_back(std::move(b));
        }
        struct ModelMaterial {const char* name;std::uint32_t token;std::size_t offset;};
        constexpr ModelMaterial modelEntries[]={
            {"ui/textures/ps2_blue",2977u,65993u},
            {"ui/textures/ps2_bluedark",2978u,66015u},
            {"ui/textures/ps2_bluelight",2979u,66041u},
            {"ui/textures/ps2_buttons",2980u,66068u},
            {"ui/textures/ps2_red",2981u,66093u}
        };
        for(const auto& entry:modelEntries){
            std::size_t length=0;for(;entry.name[length];++length)
                if(entry.offset+length>=table.size||fold(char(table.data[entry.offset+length]))!=entry.name[length])return fail(d,Error::registry);
            if(entry.offset+length>=table.size||(table.data[entry.offset+length]!=' '&&table.data[entry.offset+length]!='\r'&&table.data[entry.offset+length]!='\n'))return fail(d,Error::registry);
            assets::OriginalUiMaterial50KG material;
            if(!assets::readOriginalUiMaterial50KG(*in.options,std::string(entry.name)+".psm",s->templates,
                assets::OriginalUiTemplate50KG::defaultMt,in.runtime,material,d.material))return fail(d,Error::material);
            const bool red=same(entry.name,"ui/textures/ps2_red");
            if(material.passes.size()!=(red?1u:2u))return fail(d,Error::material);
            for(std::size_t p=0;p<material.passes.size();++p){
                const auto& pass=material.passes[p];std::size_t source=0;
                for(;source<indices.size();++source)if(same(pass.textureName,imageEntries[source].name))break;
                if(source==indices.size()||indices[source].size()!=1||pass.textureCount!=1)return fail(d,Error::material);
                const auto index=indices[source][0];const auto* texture=s->images->decodedTexture(index);
                OriginalUiOptionsMaterialBinding50KG binding;binding.registeredName=entry.name;binding.registryToken=entry.token;
                binding.registryIdentity=table.identity;binding.model=true;binding.specular=p==1;binding.images={index};
                binding.material.identity=material.identity;binding.material.provenance=material.provenance;binding.material.passes={pass};
                OriginalUiMaterialSpan50KG prototype;
                if(binding.specular){
                    if(!assets::prepareOriginalUiControlsSpecularMaterial50KG(pass,*s->images,index,binding.controlsSpecular,d.material)||
                        !convertOriginalUiControlsSpecularSpan50KG(binding.controlsSpecular,*s->images,{0,1,1,index,bank->textureCount()},prototype))return fail(d,Error::material);
                }else if(!texture||!assets::prepareOriginalUiLitMaterial50KM(pass,*texture,binding.lit,d.material)||
                    !convertOriginalUiLitMaterialSpan50KM(binding.lit,{0,1,1,index,bank->textureCount()},prototype))return fail(d,Error::material);
                if(samplerSet[index]&&!sameSampler(s->samplers[index],prototype.native.sampler))return fail(d,Error::samplerConflict);
                samplerSet[index]=true;s->samplers[index]=prototype.native.sampler;s->bindings.push_back(std::move(binding));
            }
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
                    if(ordinal==4){
                        for(std::uint16_t stringId=62;stringId<=85;++stringId){
                            const auto* lines=s->widgets.controlsLocalizedLines(id,stringId);if(!lines)continue;
                            auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                            if(!geometry->prepareDynamic(*text,*lines,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                            s->localizedText[id].push_back({stringId,std::move(geometry)});
                        }
                        if(s->localizedText[id].empty())return fail(d,Error::geometry);
                        auto empty=std::make_unique<game::OriginalUiTextGeometry50KG>();
                        if(!empty->prepareDynamic(*text,text->lines,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                        s->text[id]=std::move(empty);
                    }else for(std::uint32_t value=0;value<=101;++value){
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
                const auto& b=s->bindings[m];if(b.images.empty())return fail(d,Error::material);
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
        if(s->creditsExitText.prepare({12,6},in.options,*in.english,s->images,indices[0][0],d.exitText)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        s->creditsBackground=s->background;for(auto& q:s->creditsBackground.quadrants)q.sourcePC=0x1F89C0;
        // Credits loader1F7BF4..1F83BC owns four ScreenTextures independently.
        // The two permanent logolec layers sample the original top-left10x10
        // texels; the two fading logos retain their real natural dimensions.
        for(std::size_t i=0;i<s->creditsLogos.size();++i){
            const char* name=i==0?"ui/textures/logorain":"ui/textures/logolec";
            const auto m=find(s->bindings,name);if(m==unresolved||s->bindings[m].images.size()!=1)return fail(d,Error::material);
            const auto* image=bank->texture(s->bindings[m].images[0]);
            if(!image||!image->texture.width||!image->texture.height)return fail(d,Error::resource);
            const float width=i<2?float(image->texture.width):128.f;
            const float height=i<2?float(image->texture.height):i==2?128.f:448.f;
            const float x=46.f,y=i==3?0.f:160.f;
            const float u=i<2?1.f:10.f/float(image->texture.width),v=i<2?1.f:10.f/float(image->texture.height);
            auto& q=s->creditsLogos[i];q.materialName=name;q.authoredDepth=15;
            q.vertices={{{x,y,0,0},{x+width,y,u,0},{x,y+height,0,v},{x+width,y+height,u,v}}};
            q.sourcePacketColor=game::originalUiTextPacketColor50KG(0x00ffffff,i<2?0:i==2?255:89);
            q.sourcePC=i==0?0x1F8BFC:i==1?0x1F8D20:i==2?0x1F89DC:0x1F89E4;
        }
        s->controlsBackground=s->background;for(auto& q:s->controlsBackground.quadrants)q.sourcePC=0x1F3D28;
        for(std::uint8_t controller=0;controller<2;++controller)
            if(s->controlsExitText[controller].prepare({12,static_cast<std::uint8_t>(controller+2)},in.options,*in.english,s->images,indices[0][0],d.exitText)!=
                game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        if(s->memoryCardExitText.prepare({12,5},in.options,*in.english,s->images,indices[0][0],d.exitText)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1D6AEC);
        if(!s->memoryCard.prepare(s->widgets,s->images,s->bindings,d.memoryCard))
            return fail(d,Error::resource,d.memoryCard.sourcePC);
        s->controllerPopup=in.retainedControllerPopup?in.retainedControllerPopup:std::make_shared<OriginalUiControllerPopupResources50KG>();
        if(in.application->controllerPopupResources&&in.application->controllerPopupResources!=s->controllerPopup)
            return fail(d,Error::input,0x1B7C1C);
        if(!s->controllerPopup->prepareOrRejoin(in,s->images,s->bindings,d.controllerPopup))
            return fail(d,Error::resource,d.controllerPopup.sourcePC);
        in.application->controllerPopupResources=s->controllerPopup;
        storage_=std::move(s);d={};return true;
    } catch(...) {return fail(d,Error::allocation);}
}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::exitText() noexcept {return storage_?&storage_->exitText:nullptr;}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::gameOptionsExitText() noexcept {return storage_?&storage_->gameExitText:nullptr;}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::creditsExitText() noexcept {return storage_?&storage_->creditsExitText:nullptr;}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::memoryCardExitText() noexcept {return storage_?&storage_->memoryCardExitText:nullptr;}
OriginalUiMemoryCardResources50KG* OriginalUiOptionsDrawResources50KG::memoryCardResources() noexcept {return storage_?&storage_->memoryCard:nullptr;}
const OriginalUiMemoryCardResources50KG* OriginalUiOptionsDrawResources50KG::memoryCardResources() const noexcept {return storage_?&storage_->memoryCard:nullptr;}
bool OriginalUiOptionsDrawResources50KG::memoryCardReady() const noexcept {
    // The renderer-minted weak token expires on renderer close/reopen. Cold
    // resources alone never authorize entry or an initial-clear substitution.
    return storage_&&storage_->memoryCard.loaded()&&storage_->memoryCard.geometry()->prepared()&&
        storage_->owners.video.directVideoCapability.valid();
}
game::OriginalUiExitText50KG* OriginalUiOptionsDrawResources50KG::controlsExitText(std::uint8_t controller) noexcept {
    return storage_&&controller<2?&storage_->controlsExitText[controller]:nullptr;
}
bool OriginalUiOptionsDrawResources50KG::controlsModelReady() const noexcept {
    return storage_&&storage_->controlsModel.prepared();
}
bool OriginalUiOptionsDrawResources50KG::publishControlsRegistry(Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);
    if(!storage_->controlsModel.publishRegistry(*storage_->owners.lightManager,d.controlsModel))
        return fail(d,Error::geometry,d.controlsModel.sourcePC);
    return true;
}
bool OriginalUiOptionsDrawResources50KG::applyControlsModelEffect(const game::OriginalUiControlsEffect50KG& effect,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.controlsModel.apply(effect,s.owners.application->modelMotion,s.owners.application->modelMode0Ik,d.controlsModel))
        return fail(d,Error::geometry,d.controlsModel.sourcePC);
    return true;
}
bool OriginalUiOptionsDrawResources50KG::advanceControlsModel(float dt,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.controlsModel.advance(dt,s.owners.application->modelMotion,s.owners.application->modelMode0Ik,d.controlsModel))
        return fail(d,Error::geometry,d.controlsModel.sourcePC);
    return true;
}
bool OriginalUiOptionsDrawResources50KG::controlsAnimationObservation(bool& stopped10) const noexcept {
    return storage_&&storage_->controlsModel.observation(stopped10);
}
bool OriginalUiOptionsDrawResources50KG::prepareControlsModelPacket(game::OriginalUiControlsModelPacket50KG& packet,Diagnostic& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.controlsModel.preparePacket(s.camera,*s.owners.lightManager,packet,d.controlsModel))
        return fail(d,Error::geometry,d.controlsModel.sourcePC);
    return true;
}
bool OriginalUiOptionsDrawResources50KG::currentControlsModelSelection(const game::OriginalUiControlsModelSelection50KG& selection) const noexcept {
    return storage_&&storage_->controlsModel.isCurrentSelection(selection);
}
bool OriginalUiOptionsDrawResources50KG::loaded() const noexcept {return bool(storage_);}
std::shared_ptr<OriginalUiControllerPopupResources50KG> OriginalUiOptionsDrawResources50KG::controllerPopup() const noexcept {return storage_?storage_->controllerPopup:nullptr;}
game::OriginalUiOptionsMenuWidgets50KG* OriginalUiOptionsDrawResources50KG::widgets() noexcept {return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiOptionsMenuWidgets50KG* OriginalUiOptionsDrawResources50KG::widgets() const noexcept {return storage_?&storage_->widgets:nullptr;}
const assets::OriginalUiScreenAssets50KG* OriginalUiOptionsDrawResources50KG::images() const noexcept {return storage_?storage_->images.get():nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiOptionsDrawResources50KG::sourceImageBank() const noexcept {return storage_?&storage_->imageBank:nullptr;}
std::uint32_t OriginalUiOptionsDrawResources50KG::controlsMaterial(const std::string& name,std::uint32_t pass) const noexcept {
    if(!storage_)return unresolved;
    for(std::size_t i=0;i<storage_->bindings.size();++i){const auto& b=storage_->bindings[i];
        if(!b.model||b.material.passes.size()!=1||b.material.passes[0].pass!=pass)continue;
        if(same(b.registeredName,name))return static_cast<std::uint32_t>(i);
        // PSG uses bare original material names; avoid allocating a prefix in
        // the ordinary Widget36 draw. Cold binding already owns exact paths.
        constexpr std::size_t prefix=12; // "ui/textures/"
        if(b.registeredName.size()!=prefix+name.size())continue;
        bool equal=true;for(std::size_t j=0;j<name.size();++j)
            if(fold(b.registeredName[prefix+j])!=fold(name[j])){equal=false;break;}
        if(equal)return static_cast<std::uint32_t>(i);
    }
    return unresolved;
}
const assets::MaterialTextureAssets50EA* OriginalUiOptionsDrawResources50KG::textures() const noexcept {return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiOptionsDrawResources50KG::samplers() const noexcept {return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiOptionsMaterialBinding50KG>* OriginalUiOptionsDrawResources50KG::materials() const noexcept {return storage_?&storage_->bindings:nullptr;}
const OriginalUiOptionsVideoOwner50KG* OriginalUiOptionsDrawResources50KG::video() const noexcept {return storage_?&storage_->owners.video:nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiOptionsDrawResources50KG::textGeometry(std::uint16_t id) const noexcept {
    if(!storage_||id>=game::originalUiOptionsWidgetStorageCount50KG)return nullptr;
    const auto ordinal=id==38?0:id==112?1:id==114?2:id==165?3:4;
    if(ordinal<4){const auto value=storage_->widgets.gameOptionsRenderedVolume(id);
        return storage_->volumeText[ordinal][value<0?101:static_cast<std::size_t>(value)].get();}
    return storage_->text[id].get();
}
const game::OriginalUiPositionedLine50KG* OriginalUiOptionsDrawResources50KG::textGeometryLine(std::uint16_t id,std::size_t line) const noexcept {
    if(!storage_||id>=game::originalUiOptionsWidgetStorageCount50KG)return nullptr;
    const auto stringId=storage_->widgets.controlsRenderedString(id,line);
    for(const auto& value:storage_->localizedText[id])if(value.stringId==stringId)
        return line<value.geometry->lines().size()?&value.geometry->lines()[line]:nullptr;
    const auto* geometry=textGeometry(id);return geometry&&line<geometry->lines().size()?&geometry->lines()[line]:nullptr;
}
std::uint32_t OriginalUiOptionsDrawResources50KG::widgetMaterial(std::uint16_t id) const noexcept {return storage_&&id<game::originalUiOptionsWidgetStorageCount50KG?storage_->widgetMaterial[id]:unresolved;}
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
const game::OriginalUiOptionsBackground50KG* OriginalUiOptionsDrawResources50KG::controlsBackground() const noexcept {return storage_?&storage_->controlsBackground:nullptr;}
const game::OriginalUiOptionsBackground50KG* OriginalUiOptionsDrawResources50KG::creditsBackground() const noexcept {
    return storage_?&storage_->creditsBackground:nullptr;
}
const game::OriginalUiOverlayQuad50KG* OriginalUiOptionsDrawResources50KG::creditsLogo(OriginalUiCreditsLogo50KG logo) const noexcept {
    const auto index=static_cast<std::size_t>(logo);
    return storage_&&index<storage_->creditsLogos.size()?&storage_->creditsLogos[index]:nullptr;
}
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
