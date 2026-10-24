#include "original_ui_main_draw50kg.h"
#include "original_ui_menu_warning_packet50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_shared_dialog_material_history50kg.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/original_ui_controller_popup_geometry50kg.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <new>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiMainDrawError50KG;
using Archive=assets::OriginalUiArchiveId50KG;
using WidgetStatus=game::OriginalUiMainWidgetStatus50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(OriginalUiMainDrawDiagnostic50KG& d,Error e,std::uint32_t pc=0) noexcept {
    d.error=e;d.sourcePC=pc;return false;
}
char fold(char c) noexcept { return c=='\\'?'/' : c>='A'&&c<='Z'?char(c+32):c; }
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
bool archive(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
std::uint64_t sourceImageIdentity(Archive archiveId,const assets::OriginalUiMemberView50KG& member,
    std::size_t pstOffset) noexcept {
    const auto* owner=assets::originalUiArchiveIdentity50KG(archiveId);
    if(!owner||pstOffset>0xffffffffu)return 0;
    for(std::size_t i=0;i<owner->memberCount;++i)
        if(assets::originalUiMemberIdentity50KG(archiveId,i)==member.identity){
            if(i>=0xffffffu)return 0;
            return (std::uint64_t(static_cast<std::uint8_t>(archiveId)+1u)<<56)|
                (std::uint64_t(i+1u)<<32)|std::uint64_t(pstOffset);
        }
    return 0;
}
bool validVideo(const OriginalUiMainVideoOwner50KG& v) noexcept {
    return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&
        v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;
}
bool packetDepth(const OriginalUiMainVideoOwner50KG& v,float assigned,float& z) noexcept {
    // Bounded original SUB.S then FTOI0; positive integer subtraction is exact.
    // No silently host-rounded fractional assigned-depth path is admitted.
    if(!validVideo(v)||!std::isfinite(assigned))return false;
    if(assigned>=0&&std::floor(assigned)!=assigned)return false;
    const float value=assigned<0?-assigned:float(v.packetZMaximum)-assigned;
    if(!std::isfinite(value)||value<0||value>16777215.f)return false;
    z=float(static_cast<std::uint32_t>(value));return true;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
bool animationConfig(const assets::OriginalUiMaterialPass50KG& pass,
    game::OriginalUiTextureAnimationConfig50KG& out,OriginalUiMainDrawDiagnostic50KG& d) {
    const auto* mode=pass.authored.find("TextureAnimation");
    if(!mode||!same(mode->text,"unique"))return fail(d,Error::animation,0x29b950);
    auto config=game::OriginalUiTextureAnimationConfig50KG{};
    assets::OriginalUiPageDiagnostic50KG diag;
    std::int32_t looping=1,reverse=0,loopTo=-1;
    if(!assets::originalUiNumber50KG(pass.authored,"animatedMapInitialFrame",0,config.initialFrame,diag,0x29b950)||
        !assets::originalUiNumber50KG(pass.authored,"animatedMapFramesPerSecond",1,config.framesPerSecond,diag,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLooping",1,looping,diag,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapReverseLoopOnEnd",0,reverse,diag,0x29b950)||
        !assets::originalUiInteger50KG(pass.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,diag,0x29b950))
        return fail(d,Error::animation,0x29b950);
    config.looping=looping!=0;config.reverseLoopOnEnd=reverse!=0;config.loopOnEndToFrame=loopTo;
    out=config;return true;
}
struct ImageEntry { const char* name;Archive archive;const char* member;const char* cel;bool atm; };
const ImageEntry imageEntries[]={
    {"denmrk16",Archive::fonts,"font/denmrk16.pst","font/denmrk16.cel",false},
    {"venus16",Archive::fonts,"font/venus16.pst","font/venus16.cel",false},
    {"helvcondblack14",Archive::fonts,"font/helvcondblack14.pst","font/helvcondblack14.cel",false},
    {"denmrk20",Archive::fonts,"font/denmrk20.pst","font/denmrk20.cel",false},
    {"overlay1",Archive::controllerPopup,"ui/textures/overlay1.pst",nullptr,false},
    {"mmlogo",Archive::mainMenu,"ui/textures/mmlogo.pst",nullptr,false},
    {"mmbkg",Archive::mainMenu,"ui/textures/mmbkg.pst",nullptr,false},
    {"arrwrght",Archive::mainMenu,"ui/textures/arrwrght.atm",nullptr,true},
    {"arrwleft",Archive::mainMenu,"ui/textures/arrwleft.atm",nullptr,true},
    {"icon_x",Archive::overlays,"ui/textures/icon_x.pst",nullptr,false},
    {"icon_tri",Archive::overlays,"ui/textures/icon_tri.pst",nullptr,false},
    {"icon_sqr",Archive::overlays,"ui/textures/icon_sqr.pst",nullptr,false},
    {"icon_cir",Archive::overlays,"ui/textures/icon_cir.pst",nullptr,false},
    {"mmprt",Archive::mainMenu,"particle/mmprt.pst",nullptr,false},
    {"venus14",Archive::fonts,"font/venus14.pst","font/venus14.cel",false},
    {"popup_icon_x",Archive::controllerPopup,"ui/textures/icon_x.pst",nullptr,false},
    {"popup_icon_tri",Archive::controllerPopup,"ui/textures/icon_tri.pst",nullptr,false}
};
struct MaterialEntry { const char* registry;const char* texture;const char* sourceTexture=nullptr; };
// Resource table order is an internal immutable index, NOT source draw order.
const MaterialEntry materialEntries[]={
    {"font/denmrk16top","denmrk16"},{"font/venus16","venus16"},
    {"font/helvcondblack14","helvcondblack14"},{"font/denmrk20","denmrk20"},
    {"font/venus16top","venus16"},{"ui/textures/overlay1","overlay1"},
    {"ui/textures/mmlogo","mmlogo"},{"ui/textures/mmbkg","mmbkg"},
    {"ui/textures/arrwrght","arrwrght"},{"ui/textures/arrwleft","arrwleft"},
    {"ui/textures/notexture",nullptr},
    {"ui/textures/icon_x","icon_x"},{"ui/textures/icon_tri","icon_tri"},
    {"ui/textures/icon_sqr","icon_sqr"},{"ui/textures/icon_cir","icon_cir"},
    {"ui/textures/dimpopup",nullptr},{"ui/textures/overlay1popup","overlay1"},
    {"font/venus14popup","venus14"},{"ui/textures/icon_x_popup","popup_icon_x","icon_x"},
    {"ui/textures/icon_tri_popup","popup_icon_tri","icon_tri"},
    // Append the sixth shared material: legacy icon indices18/19 stay exact.
    {"font/venus16popup","venus16"}
};
// Common six-token order is deliberately distinct from bank order and the
// unchanged five-role controller47C geometry enum/popupIndices.
struct SharedDialogMaterial {std::size_t index;std::uint32_t token;std::size_t offset;};
constexpr SharedDialogMaterial sharedDialogMaterials[]={
    {15,3228,70804},{16,3229,70826},{20,3230,70853},
    {17,3231,70872},{18,3232,70891},{19,3233,70917}
};
const SharedDialogMaterial* sharedDialogMaterial(std::size_t index)noexcept {
    for(const auto& entry:sharedDialogMaterials)if(entry.index==index)return &entry;
    return nullptr;
}
bool popupRegistry(const assets::OriginalUiMemberView50KG& table) noexcept {
    if(!table.identity||!table.data||table.size!=70962)return false;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::size_t cursor=0,matched=0;std::uint32_t token=0;
    while(cursor<table.size){while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(const auto& entry:sharedDialogMaterials)if(token==entry.token){
            const char* name=materialEntries[entry.index].registry;std::size_t j=0;if(start!=entry.offset)return false;
            for(;name[j];++j)if(start+j>=cursor||fold(char(table.data[start+j]))!=name[j])return false;
            if(start+j!=cursor)return false;++matched;
        }++token;
    }return matched==sizeof(sharedDialogMaterials)/sizeof(sharedDialogMaterials[0]);
}
const assets::OriginalUiArchive50KG* selectArchive(const OriginalUiMainColdInput50KG& in,Archive id) noexcept {
    return id==Archive::fonts?in.fonts.get():id==Archive::controllerPopup?in.controllerPopup.get():
        id==Archive::overlays?in.overlays.get():in.mainMenu.get();
}
std::uint32_t findMaterial(const std::vector<OriginalUiMainMaterialBinding50KG>& materials,
    const std::string& name) noexcept {
    for(std::size_t i=0;i<materials.size();++i)
        if(same(materials[i].registeredName,name))return static_cast<std::uint32_t>(i);
    return unresolved;
}
}
struct OriginalUiMainDrawResources50KG::Storage {
    OriginalUiMainColdInput50KG owners;
    assets::OriginalUiMaterialTemplates50KG templates;
    assets::OriginalUiMainMaterialSchedule50KG schedule;
    assets::OriginalUiMainParticleMaterial50KG particle;
    std::uint32_t particleImage=unresolved;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
    assets::OriginalUiImageBank50KG imageBank;
    std::shared_ptr<OriginalUiControllerPopupResources50KG> popup;
    game::OriginalUiMainWidgetManager50KG manager;
    std::vector<OriginalUiMainMaterialBinding50KG> materials;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,50> text;
    std::array<std::uint32_t,50> widgetMaterial{};
    game::OriginalUiMainBackground50KG background;
    game::OriginalUiExitText50KG exitText;
};
// The Root calls this only between actual47C binding and its first reappend.
// The bank supplies its retained cold inputs, not caller-rebuilt credentials or
// a copied/default history. The six-history owner checks the initial prefix
// and transfers the exact unbound five47C records only after all six validate.
bool OriginalUiSharedDialogs50KG::bootstrapInitialHistoryFromMain(
    const OriginalUiMainDrawResources50KG& resources,Diagnostic& d)noexcept {
    d={};const auto* bank=resources.storage_.get();
    if(!bank||!controller47C_.resources||bank->popup!=controller47C_.resources)
        return fail(d,Status::owner,0x2055B0);
    if(!OriginalUiSharedDialogMaterialHistory50KG::bootstrapInitial(
        *this,bank->owners,bank->images,bank->materials,d.popup))
        return fail(d,Status::history,d.popup.sourcePC);
    d.sourceEffectsCommitted=true;d.sourcePC=0x2055B0;return true;
}
OriginalUiMainDrawResources50KG::OriginalUiMainDrawResources50KG() noexcept=default;
OriginalUiMainDrawResources50KG::~OriginalUiMainDrawResources50KG()=default;
bool OriginalUiMainDrawResources50KG::load(const OriginalUiMainColdInput50KG& in,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        if(!archive(in.appStart,Archive::appStart)||!archive(in.mainMenu,Archive::mainMenu)||
            !archive(in.fonts,Archive::fonts)||!archive(in.controllerPopup,Archive::controllerPopup)||
            !archive(in.overlays,Archive::overlays))
            return fail(d,Error::archive);
        if(!validVideo(in.video))return fail(d,Error::videoOwner,0x25bad4);
        if(!in.runtime.overrideKnown)return fail(d,Error::material,0x24714c);
        auto next=std::make_unique<Storage>();next->owners=in;
        next->owners.english=nullptr; // descriptor resolution is the only borrow
        next->widgetMaterial.fill(unresolved);
        if(next->manager.bindMain(in.mainMenu,in.english,d.manager)!=WidgetStatus::ok)
            return fail(d,Error::manager);
        if(!next->templates.load(*in.appStart,d.material))return fail(d,Error::material);
        if(!next->schedule.load(*in.appStart,*in.mainMenu,*in.overlays,d.registry))return fail(d,Error::registry);
        const auto table=in.appStart->find("AppStart/Mat_Stbl.txt");
        if(!popupRegistry(table))return fail(d,Error::registry,0x298724);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        constexpr auto imageCount=sizeof(imageEntries)/sizeof(imageEntries[0]);
        std::array<std::vector<std::uint32_t>,imageCount> imageIndices;
        for(std::size_t i=0;i<imageCount;++i){
            const auto& entry=imageEntries[i];const auto* owner=selectArchive(in,entry.archive);
            const auto member=owner->find(entry.member);
            if(!member.identity)return fail(d,Error::resource);
            if(entry.atm){
                assets::OriginalUiAtm50KG atm;
                if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm))
                    return fail(d,Error::resource,0x102230);
                for(const auto& frame:atm.frames){
                    const auto index=static_cast<std::uint32_t>(sources.size());
                    imageIndices[i].push_back(index);
                    const auto identity=sourceImageIdentity(entry.archive,member,frame.pstOffset);
                    if(!identity)return fail(d,Error::resource);
                    sources.push_back({identity,member,frame.pstOffset,frame.pstBytes,{}});
                }
            }else{
                const auto cel=entry.cel?owner->find(entry.cel):assets::OriginalUiMemberView50KG{};
                if(entry.cel&&!cel.identity)return fail(d,Error::resource);
                const auto index=static_cast<std::uint32_t>(sources.size());
                imageIndices[i].push_back(index);
                const auto identity=sourceImageIdentity(entry.archive,member,0);
                if(!identity)return fail(d,Error::resource);
                sources.push_back({identity,member,0,member.size,cel});
            }
        }
        auto originalImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!originalImages->load(sources,d.resource))return fail(d,Error::resource);
        assets::OriginalUiImageBankInput50KG bankInput;bankInput.archives={in.appStart,in.mainMenu,in.fonts,in.controllerPopup,in.overlays};bankInput.owners={originalImages};
        assets::OriginalUiImageBankDiagnostic50KG bankDiagnostic;
        if(!next->imageBank.load(bankInput,bankDiagnostic))return fail(d,Error::resource);
        for(std::uint32_t i=0;i<sources.size();++i){const auto* map=next->imageBank.mapping(0,i);
            if(!map||map->combinedIndex!=i)return fail(d,Error::resource);}
        next->images=next->imageBank.images();
        const auto* bank=next->images->textures();
        next->samplers.resize(bank->textureCount());
        std::vector<bool> samplerSet(bank->textureCount(),false);
        next->materials.reserve(sizeof(materialEntries)/sizeof(materialEntries[0]));
        for(std::size_t i=0;i<sizeof(materialEntries)/sizeof(materialEntries[0]);++i){
            d.materialIndex=static_cast<std::uint32_t>(i);
            const auto& entry=materialEntries[i];const auto* shared=sharedDialogMaterial(i);
            OriginalUiMainMaterialBinding50KG binding;
            binding.registeredName=entry.registry;
            const bool logo=same(binding.registeredName,"ui/textures/mmlogo");
            bool read=false;
            if(shared)read=assets::readOriginalUiMaterial50KG(*in.controllerPopup,binding.registeredName+".psm",
                next->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,binding.material,d.material);
            else if(i>=11)read=assets::readOriginalUiMainPromptFallbackMaterial50KG(*in.mainMenu,*in.overlays,
                static_cast<assets::OriginalUiMainPrompt50KG>(i-11),next->templates,
                assets::OriginalUiTemplate50KG::screenMt,in.runtime,binding.material,d.material);
            else if(logo)read=assets::readOriginalUiMainLogoFallbackMaterial50KG(*in.mainMenu,
                next->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,binding.material,d.material);
            else read=assets::readOriginalUiMaterial50KG(*in.mainMenu,binding.registeredName+".psm",
                next->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,binding.material,d.material);
            if(!read||binding.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=binding.material.passes[0];
            binding.registryIdentity=table.identity;
            if(shared){binding.registryToken=shared->token;}
            else {const auto* scheduled=next->schedule.find(binding.registeredName);
                if(!scheduled||scheduled->shaderPassCount!=1||scheduled->textureMapCount!=pass.textureCount||
                    scheduled->psmIdentity!=binding.material.identity)return fail(d,Error::registry);
                binding.projectedLeafOrdinal=scheduled->projectedLeafOrdinal;binding.registryToken=scheduled->registryTokenOrdinal;}

            if(entry.texture){
                const char* sourceTexture=entry.sourceTexture?entry.sourceTexture:entry.texture;
                if(pass.textureCount!=1||!same(pass.textureName,sourceTexture))return fail(d,Error::material);
                std::size_t map=0;for(;map<imageCount;++map)if(same(imageEntries[map].name,entry.texture))break;
                if(map==imageCount)return fail(d,Error::resource);
                binding.images=imageIndices[map];binding.animated=imageEntries[map].atm;
                if(binding.animated){
                    if(!animationConfig(pass,binding.animation,d)||
                        !game::validOriginalUiTextureAnimationConfig50KG(binding.animation,
                            static_cast<std::uint32_t>(binding.images.size())))return fail(d,Error::animation);
                }else if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation);
                for(const auto index:binding.images){
                    d.imageIndex=index;
                    const auto* decoded=next->images->decodedTexture(index);
                    assets::OriginalUiNativeMaterial50KG policy;
                    if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))
                        return fail(d,Error::material);
                    OriginalUiMaterialSpan50KG converted;
                    if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,index,bank->textureCount()},converted))
                        return fail(d,Error::material);
                    if(samplerSet[index]&&!sameSampler(next->samplers[index],converted.native.sampler))
                        return fail(d,Error::samplerConflict);
                    next->samplers[index]=converted.native.sampler;samplerSet[index]=true;
                    binding.policies.push_back(policy);
                }
            }else if(pass.textureCount!=0||
                !assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,binding.untextured,d.material))
                return fail(d,Error::material);
            next->materials.push_back(std::move(binding));
        }
        next->particleImage=imageIndices[13][0];
        if(!assets::prepareOriginalUiMainParticleMaterial50KG(*in.mainMenu,next->templates,in.runtime,
            *next->images->decodedTexture(next->particleImage),next->particle,d.material))return fail(d,Error::material);
        const auto* particleSchedule=next->schedule.find("particle/mmprt");
        if(!particleSchedule||particleSchedule->projectedLeafOrdinal!=0||
            particleSchedule->psmIdentity!=next->particle.identity||!next->particle.noDestinationAlphaConsumer)
            return fail(d,Error::registry);
        for(std::uint8_t state=0;state<2;++state){
            OriginalUiMaterialSpan50KG converted;
            if(!convertOriginalUiMainParticleSpan50KG(next->particle,state,
                {0,1,1,next->particleImage,bank->textureCount()},converted))return fail(d,Error::material);
            if(state&&!sameSampler(next->samplers[next->particleImage],converted.native.sampler))return fail(d,Error::samplerConflict);
            next->samplers[next->particleImage]=converted.native.sampler;
        }
        samplerSet[next->particleImage]=true;
        for(bool present:samplerSet)if(!present)return fail(d,Error::resource);
        for(std::uint16_t id=1;id<=49;++id){
            d.widgetId=id;
            if(const auto* text=next->manager.textDescriptor(id)){
                const auto m=findMaterial(next->materials,"font/"+text->materialName);
                if(m==unresolved)return fail(d,Error::material);
                const auto& binding=next->materials[m];
                if(!same(binding.material.passes[0].textureName,text->fontName)||binding.images.size()!=1)
                    return fail(d,Error::material);
                const auto index=binding.images[0];
                const auto* font=next->images->font(index);const auto* atlas=bank->texture(index);
                if(!font||!atlas)return fail(d,Error::resource);
                auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                if(!geometry->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                float z=0;if(!packetDepth(in.video,text->depth,z))return fail(d,Error::unsupportedDepth,0x2e10dc);
                next->text[id]=std::move(geometry);next->widgetMaterial[id]=m;
            }else if(const auto* icon=next->manager.iconDescriptor(id)){
                const auto m=findMaterial(next->materials,"ui/textures/"+icon->materialName);
                if(m==unresolved)return fail(d,Error::material);
                const auto& binding=next->materials[m];
                if(binding.images.size()!=1||binding.animated)return fail(d,Error::material);
                if(next->manager.prepareIcon(id,next->images,binding.images[0],in.video.packetZMaximum,d.manager)!=WidgetStatus::ok)
                    return fail(d,Error::manager);
                float z=0;
                if(!packetDepth(in.video,icon->depth<0?float(in.video.packetZMaximum-300):icon->depth,z))
                    return fail(d,Error::unsupportedDepth,0x2aca44);
                next->widgetMaterial[id]=m;
            }
        }
        const auto exitMaterial=findMaterial(next->materials,"font/denmrk16top");
        if(exitMaterial==unresolved || !in.english)return fail(d,Error::resource,0x1d6ba8);
        const auto& exitBinding=next->materials[exitMaterial];
        if(exitBinding.animated || exitBinding.images.size()!=1 ||
            next->exitText.prepare({4,1},in.mainMenu,*in.english,next->images,
                exitBinding.images[0],d.exitText)!=game::OriginalUiExitTextStatus50KG::ok)
            return fail(d,Error::geometry,0x1d6bb0);
        const auto backgroundMaterial=findMaterial(next->materials,"ui/textures/mmbkg");
        const auto* atlas=bank->texture(next->materials[backgroundMaterial].images[0]);
        if(!atlas||!game::layoutOriginalUiMainBackground50KG(atlas->texture.width,atlas->texture.height,next->background))
            return fail(d,Error::geometry);
        next->popup=in.retainedControllerPopup?in.retainedControllerPopup:std::make_shared<OriginalUiControllerPopupResources50KG>();
        if(!next->popup->prepareOrRejoin(in,next->images,next->materials,d.controllerPopup))return fail(d,Error::resource,d.controllerPopup.sourcePC);
        storage_=std::move(next);d={};return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::input);}
}
std::shared_ptr<OriginalUiControllerPopupResources50KG> OriginalUiMainDrawResources50KG::controllerPopup()const noexcept{return storage_?storage_->popup:nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiMainDrawResources50KG::sourceImageBank()const noexcept{return storage_?&storage_->imageBank:nullptr;}
bool OriginalUiMainDrawResources50KG::loaded()const noexcept{return bool(storage_);}
game::OriginalUiMainWidgetManager50KG* OriginalUiMainDrawResources50KG::manager()noexcept{return storage_?&storage_->manager:nullptr;}
const game::OriginalUiMainWidgetManager50KG* OriginalUiMainDrawResources50KG::manager()const noexcept{return storage_?&storage_->manager:nullptr;}
const assets::OriginalUiScreenAssets50KG* OriginalUiMainDrawResources50KG::images()const noexcept{return storage_?storage_->images.get():nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiMainDrawResources50KG::textures()const noexcept{return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiMainDrawResources50KG::samplers()const noexcept{return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiMainMaterialBinding50KG>* OriginalUiMainDrawResources50KG::materials()const noexcept{return storage_?&storage_->materials:nullptr;}
const OriginalUiMainVideoOwner50KG* OriginalUiMainDrawResources50KG::video()const noexcept{return storage_?&storage_->owners.video:nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiMainDrawResources50KG::textGeometry(std::uint16_t id)const noexcept{return storage_&&id<50?storage_->text[id].get():nullptr;}
std::uint32_t OriginalUiMainDrawResources50KG::widgetMaterial(std::uint16_t id)const noexcept{return storage_&&id<50?storage_->widgetMaterial[id]:unresolved;}
std::uint32_t OriginalUiMainDrawResources50KG::materialIndex(const char* name)const noexcept{
    if(!storage_||!name)return unresolved;
    // Do not construct a temporary std::string from a noexcept lookup.
    for(std::size_t i=0;i<storage_->materials.size();++i){
        const auto& a=storage_->materials[i].registeredName;std::size_t j=0;
        for(;j<a.size()&&name[j]&&fold(a[j])==fold(name[j]);++j){}
        if(j==a.size()&&!name[j])return static_cast<std::uint32_t>(i);
    }
    return unresolved;
}
game::OriginalUiExitText50KG* OriginalUiMainDrawResources50KG::exitText() noexcept{return storage_?&storage_->exitText:nullptr;}
const game::OriginalUiExitText50KG* OriginalUiMainDrawResources50KG::exitText() const noexcept{return storage_?&storage_->exitText:nullptr;}
const game::OriginalUiMainBackground50KG* OriginalUiMainDrawResources50KG::background()const noexcept{return storage_?&storage_->background:nullptr;}

const assets::OriginalUiMainParticleMaterial50KG* OriginalUiMainDrawResources50KG::particleMaterial()const noexcept{return storage_?&storage_->particle:nullptr;}
std::uint32_t OriginalUiMainDrawResources50KG::particleImageIndex()const noexcept{return storage_?storage_->particleImage:unresolved;}

struct OriginalUiMainDraw50KG::Storage {
    std::shared_ptr<OriginalUiControllerPopupResources50KG> popup;
    std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG> common;
    std::array<std::uint32_t,6> historyIndices{};
    std::array<assets::OriginalUiMaterialBindingState50KG,6> commonPending;
    std::vector<OriginalUiSharedDialogMaterialHistory50KG::MenuUse> menuUses;
    bool historyCurrent(const OriginalUiSourceOwner50KM* owner)const noexcept {
        return common?common->current(owner,popupRevision):popup->current(owner,popupRevision);
    }
    void poisonHistory(const OriginalUiSourceOwner50KM* owner)noexcept {
        historyPoisoned=true;if(common)common->poison(owner);else popup->poison(owner);
    }
    std::array<std::uint32_t,5> popupIndices{};
    std::array<assets::OriginalUiMaterialBindingState50KG,5> popupPendingHistory;
    std::vector<assets::OriginalUiMaterialBindingState50KG> history,pendingHistory;
    std::vector<std::uint32_t> leaf,representatives;
    std::uint64_t popupRevision=0,frameGeneration=0,attemptedGeneration=0,acknowledgedGeneration=0;
    const VehicleWindowGl50DS* submittedRenderer=nullptr;
    bool popupLeased=false,popupVisited=false,submissionPending=false,historyPoisoned=false;
    bool prepareGroups(const OriginalUiSourceOwner50KM* owner,OriginalUiMainDrawDiagnostic50KG& d){
        const auto& bindings=*resources->materials();const auto count=bindings.size();
        popup=resources->controllerPopup();if(!popup)return fail(d,Error::resource);popupIndices.fill(unresolved);historyIndices.fill(unresolved);
        history.clear();pendingHistory.clear();leaf.clear();representatives.clear();
        std::vector<std::uint32_t> sequence;sequence.reserve(count);history.resize(count);leaf.resize(count);
        for(std::size_t i=0;i<count;++i){const auto& b=bindings[i];
            if(b.material.passes.size()!=1)return fail(d,Error::material);
            const auto& pass=b.material.passes[0];assets::OriginalUiMaterialEquivalenceKey50KG key;
            assets::OriginalUiMaterialEquivalenceDiagnostic50KG keyDetail;assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            const auto* source=b.images.empty()?nullptr:resources->images()->sourceProvenance(b.images[0]);
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,source,key,keyDetail))
                return fail(d,Error::material,keyDetail.sourcePC);
            if(common&&b.registryToken>=3228&&b.registryToken<=3233){
                const auto role=b.registryToken-3228;
                if(historyIndices[role]!=unresolved||!common->copyHistory(owner,popupRevision,b.registryToken,key,history[i],d.controllerPopup))
                    return fail(d,Error::material,d.controllerPopup.sourcePC);
                historyIndices[role]=static_cast<std::uint32_t>(i);
            }else if(!history[i].initialize(key,detail))return fail(d,Error::material,detail.sourcePC);
            for(std::size_t role=0;role<popupIndices.size();++role)if(b.registryToken==OriginalUiControllerPopupResources50KG::registryTokens[role]){
                if(popupIndices[role]!=unresolved||(!common&&!popup->copyHistory(b.registryToken,key,history[i],d.controllerPopup)))
                    return fail(d,Error::material,d.controllerPopup.sourcePC);
                popupIndices[role]=static_cast<std::uint32_t>(i);
            }
            sequence.push_back(static_cast<std::uint32_t>(i));
        }
        for(auto index:popupIndices)if(index==unresolved)return fail(d,Error::material);
        if(common)for(auto index:historyIndices)if(index==unresolved)return fail(d,Error::material);
        std::sort(sequence.begin(),sequence.end(),[&](std::uint32_t a,std::uint32_t b){
            return bindings[a].registryToken!=bindings[b].registryToken?bindings[a].registryToken<bindings[b].registryToken:
                bindings[a].material.passes[0].pass<bindings[b].material.passes[0].pass;
        });
        struct Group {std::uint32_t binding=0,redirect=unresolved,limit=unresolved;};
        std::vector<Group> groups;groups.reserve(count);
        std::uint32_t previous=unresolved;
        for(const auto index:sequence){const auto current=static_cast<std::uint32_t>(groups.size());
            for(auto& prior:groups){const auto equal=history[index].compare(history[prior.binding]);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::unsupported)return fail(d,Error::material,0x249BA0);
                if(equal==assets::OriginalUiMaterialEquivalence50KG::equivalent&&current<prior.limit)prior.redirect=current;}
            groups.push_back({index,unresolved,unresolved});
            if(previous!=unresolved&&bindings[previous].registryToken==bindings[index].registryToken){
                if(bindings[index].material.passes[0].pass!=bindings[previous].material.passes[0].pass+1)return fail(d,Error::material,0x29BED0);
                groups[current-1].limit=current;
            }else if(bindings[index].material.passes[0].pass!=1)return fail(d,Error::material,0x29BED0);
            leaf[index]=current;previous=index;
        }
        for(auto& value:leaf){std::size_t steps=0;while(groups[value].redirect!=unresolved){
            const auto next=groups[value].redirect;if(next<=value||++steps>count)return fail(d,Error::material,0x29C150);value=next;}}
        for(const auto& group:groups)representatives.push_back(group.binding);
        pendingHistory=history;return true;
    }

    std::shared_ptr<OriginalUiMainDrawResources50KG> resources;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::uint64_t publication=0;
    std::vector<game::OriginalUiTextureAnimation50KG> animations;
    std::vector<std::uint32_t> frames;
    std::vector<assets::Vertex48B> submittedVertices,drainedVertices;
    std::vector<float> submittedFog,drainedFog;
    std::array<float,3> fogColor{};
    std::vector<OriginalUiMainSubmittedPacket50KG> packets;
    std::vector<std::uint32_t> order;
    std::vector<OriginalUiSpan50KG> spans;
    std::vector<OriginalUiNativeSpan50KG> states;
    OriginalUiNativeDraw50KG draw;
    OriginalUiMainFrameClock50KG clock;
    bool active=false,ordinary=false,main=false,prompts=false,top=false,scene=false,drained=false,failed=false;
    std::uint8_t apertureCount=0;
    bool exitText=false;
    std::uint32_t nextOrdinal=0;
};
bool OriginalUiMainDraw50KG::historyRendererRetired(const VehicleWindowGl50DS& renderer)const noexcept {
    return storage_&&menuRetiredRenderer_==&renderer&&!storage_->submittedRenderer&&!storage_->submissionPending;
}
bool OriginalUiMainDraw50KG::bindSharedMenu470(OriginalUiMainSession50KG& session,std::shared_ptr<OriginalUiSharedDialogs50KG> shared)noexcept {
    if(menuShared_||menuSession_||!storage_||!shared||storage_->active||storage_->popupLeased||
       storage_->resources->controllerPopup()!=shared->controller47C_.resources)return false;
    menuSession_=&session;menuShared_=std::move(shared);return true;
}
bool OriginalUiMainDraw50KG::sharedMenuBoundCurrent(const OriginalUiSharedDialogs50KG& shared,const OriginalUiMainSession50KG& session)const noexcept {
    return storage_&&menuShared_.get()==&shared&&menuSession_==&session&&
        storage_->resources&&storage_->resources->controllerPopup()==shared.controller47C_.resources&&
        storage_->gpu&&storage_->gpu->initialized()&&storage_->publication&&
        storage_->gpu->publicationToken()==storage_->publication;
}
bool OriginalUiMainDraw50KG::sharedMenuInvocationCurrent(const OriginalUiSharedDialogs50KG& shared,std::uint64_t epoch)const noexcept {
    return menuSession_&&sharedMenuBoundCurrent(shared,*menuSession_)&&epoch&&epoch==menuInvocationEpoch_;
}
bool OriginalUiMainDraw50KG::sharedMenuCollectorInitialized(const OriginalUiSharedDialogs50KG& shared,std::uint64_t epoch)const noexcept {
    if(!sharedMenuInvocationCurrent(shared,epoch)||!storage_)return false;const auto& s=*storage_;
    return s.active&&!s.failed&&!s.drained&&!s.submissionPending&&!s.historyPoisoned&&s.popupLeased&&s.historyCurrent(this)&&
        !s.popupVisited&&!s.ordinary&&!s.prompts&&!s.top&&!s.scene&&!s.exitText&&!s.apertureCount&&
        !frame_.owner_&&!frame_.native_&&!frame_.frameGeneration_&&s.packets.empty()&&s.order.empty()&&
        s.drainedVertices.empty()&&s.drainedFog.empty()&&s.spans.empty()&&s.states.empty()&&!s.main&&!s.nextOrdinal&&s.submittedVertices.empty()&&s.submittedFog.empty();
}
bool OriginalUiMainDraw50KG::sharedMenuRasterResourcesCurrent(const OriginalUiSharedDialogs50KG& shared,const void* resources,
 const MaterialTextureGl50EB* gpu,std::uint64_t publication)const noexcept {
    return menuSession_&&sharedMenuBoundCurrent(shared,*menuSession_)&&storage_->resources.get()==resources&&
        storage_->gpu.get()==gpu&&storage_->publication==publication;
}
bool OriginalUiMainDraw50KG::appendMenuWarning470(const OriginalUiSharedDialogs50KG& shared,
 const OriginalUiMenuWarningPacket50KG& receipt,const OriginalUiMenuWarningGeometry50KG& geometry,OriginalUiMainDrawDiagnostic50KG& d)noexcept {
    d={};if(!storage_)return fail(d,Error::sourceOrder);auto& s=*storage_;
    if(menuShared_.get()!=&shared||!shared.menuWarning470PacketAppendCurrent(*this,receipt)||
       !s.active||s.failed||s.drained||s.popupVisited||s.ordinary||s.top||s.scene||!s.common||
       !s.popupLeased||!s.historyCurrent(this)||receipt.materialToken_<3228||receipt.materialToken_>3230)
        return fail(d,Error::sourceOrder,receipt.sourcePC_);
    const auto count=std::size_t(geometry.quadCount)*4;
    if(!geometry.quadCount||geometry.vertices.size()!=count||geometry.stq.size()!=count||
       geometry.rasterW.size()!=count||geometry.fog.size()!=count||s.submittedVertices.size()%4||
       s.submittedVertices.size()/4>originalUiMaximumQuads50KG||geometry.quadCount>originalUiMaximumQuads50KG-s.submittedVertices.size()/4||s.packets.size()>=originalUiMaximumSpans50KG)
        return fail(d,Error::capacity,receipt.sourcePC_);
    const auto material=s.historyIndices[static_cast<std::size_t>(receipt.materialToken_-3228)];
    const auto& materials=*s.resources->materials();
    if(material>=materials.size()||materials[material].registryToken!=static_cast<std::uint32_t>(receipt.materialToken_))
        return fail(d,Error::material,receipt.sourcePC_);
    const auto& binding=materials[material];
    if(binding.animated||binding.material.passes.size()!=1||!binding.material.passes[0].enabled||
       (receipt.materialToken_==3228?!binding.images.empty():binding.images.size()!=1))
        return fail(d,Error::material,receipt.sourcePC_);
    try {
    if(receipt.materialToken_!=3228){
        const auto* bank=s.resources->sourceImageBank();
        assets::OriginalStandardMaterialNativeBinding50KG verifiedImage;
        OriginalUiControllerPopupResourceDiagnostic50KG imageDiagnostic;
        if(!bank||material>=s.history.size()||
           !shared.menuWarning470PacketImageCurrent(receipt,bank->sourceMember(binding.images[0])))
            return fail(d,Error::material,receipt.sourcePC_);
        auto candidate=s.history[material];
        assets::OriginalUiMaterialBindingDiagnostic50KG bindingDiagnostic;
        if(!candidate.recordDrainedBinding(*bank,binding.images[0],bindingDiagnostic))
            return fail(d,Error::material,bindingDiagnostic.sourcePC);
        if(!s.common->authenticateImage(
            static_cast<std::size_t>(receipt.materialToken_-3228),*bank,binding.images[0],
            candidate,verifiedImage,imageDiagnostic))
            return fail(d,Error::material,imageDiagnostic.sourcePC?imageDiagnostic.sourcePC:receipt.sourcePC_);
    }
    // These actual ScreenObject packets have Q1 and W1. Preserve ordinary
    // screen UV interpolation, never opt into the unrelated 3D raw-STQ mode.
    for(std::size_t i=0;i<count;++i){const auto& v=geometry.vertices[i];const auto& stq=geometry.stq[i];
        if(!std::isfinite(v.position[0])||!std::isfinite(v.position[1])||!std::isfinite(v.position[2])||
           v.position[2]<0||v.position[2]>16777215.f||std::floor(v.position[2])!=v.position[2]||v.color[3]>128||
           !std::isfinite(v.uv[0])||!std::isfinite(v.uv[1])||!std::isfinite(stq[0])||!std::isfinite(stq[1])||
           stq[2]!=1.f||v.uv[0]!=stq[0]||v.uv[1]!=stq[1]||geometry.rasterW[i]!=1.f||geometry.fog[i]!=0.f)
            return fail(d,Error::geometry,receipt.sourcePC_);
    }
        const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4);
        const auto image=binding.images.empty()?unresolved:binding.images[0];
        s.submittedVertices.insert(s.submittedVertices.end(),geometry.vertices.begin(),geometry.vertices.end());
        s.submittedFog.insert(s.submittedFog.end(),geometry.fog.begin(),geometry.fog.end());
        
        s.packets.push_back({first,geometry.quadCount,material,image,0,s.nextOrdinal++,receipt.sourcePC_,0,OriginalUiMainDrawSegment50KG::warning470,&receipt});
        return true;
    }catch(...){s.failed=true;return fail(d,Error::allocation,receipt.sourcePC_);}
}
bool OriginalUiMainDraw50KG::sharedMenuFrameCurrent(const OriginalUiSharedDialogs50KG& shared,std::uint64_t epoch,std::uint64_t generation)const noexcept {
    return sharedMenuInvocationCurrent(shared,epoch)&&storage_->drained&&!storage_->failed&&
        frame_.owner_==this&&frame_.native_==&storage_->draw&&generation&&
        generation==frame_.frameGeneration_&&generation==storage_->frameGeneration;
}
bool OriginalUiMainDraw50KG::sharedMenuRetiredCurrent(const OriginalUiSharedDialogs50KG& shared,const VehicleWindowGl50DS& renderer)const noexcept {
    return menuSession_&&sharedMenuBoundCurrent(shared,*menuSession_)&&menuRetiredRenderer_==&renderer&&
        !storage_->submittedRenderer&&!storage_->submissionPending;
}
OriginalUiMainDraw50KG::OriginalUiMainDraw50KG()noexcept=default;
OriginalUiMainDraw50KG::~OriginalUiMainDraw50KG(){
    if(storage_&&storage_->popup){auto& s=*storage_;
        if(s.common){if(s.popupLeased)s.common->poison(this);}
        else s.popup->release(this,!s.submissionPending&&!s.submittedRenderer);
    }
}
bool OriginalUiMainDraw50KG::bind(std::shared_ptr<OriginalUiMainDrawResources50KG> resources,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        if(!resources||!resources->loaded())return fail(d,Error::notLoaded);
        auto next=std::make_unique<Storage>();next->resources=std::move(resources);
        next->popup=next->resources->controllerPopup();if(!next->popup)return fail(d,Error::resource);
        const auto& materials=*next->resources->materials();
        next->animations.resize(materials.size());next->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
            !game::initializeOriginalUiTextureAnimation50KG(next->animations[i],materials[i].animation,
                static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,Error::animation);
        next->submittedVertices.reserve(originalUiMaximumQuads50KG*4u);
        next->drainedVertices.reserve(originalUiMaximumQuads50KG*4u);
        next->submittedFog.reserve(originalUiMaximumQuads50KG*4u);next->drainedFog.reserve(originalUiMaximumQuads50KG*4u);
        next->packets.reserve(originalUiMaximumSpans50KG);next->order.reserve(originalUiMaximumSpans50KG);
        next->spans.reserve(originalUiMaximumSpans50KG);next->states.reserve(originalUiMaximumSpans50KG);
        storage_=std::move(next);return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiMainDraw50KG::bind(std::shared_ptr<OriginalUiMainDrawResources50KG> resources,
    std::shared_ptr<const MaterialTextureGl50EB> gpu,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyLoaded);
    if(!resources||!resources->loaded())return fail(d,Error::notLoaded);
        if(!gpu||!gpu->initialized()||!gpu->publicationToken()||
            gpu->textureCount()!=resources->textures()->textureCount())return fail(d,Error::publication);
        for(std::uint32_t i=0;i<gpu->textureCount();++i){
            const auto* actual=gpu->asset(i);const auto* source=resources->textures()->texture(i);
            if(!actual||!source||actual->identity!=source->identity)return fail(d,Error::publication);
        }
    if(!bind(std::move(resources),d))return false;
    storage_->publication=gpu->publicationToken();storage_->gpu=std::move(gpu);return true;
}
bool OriginalUiMainDraw50KG::begin(const OriginalUiMainFrameClock50KG& clock,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);
    auto& s=*storage_;
    if(!clock.known)return fail(d,Error::animation);
    if(s.gpu&&(s.gpu->publicationToken()!=s.publication||!s.gpu->initialized()))return fail(d,Error::publication);
    if(s.submissionPending||s.historyPoisoned)return fail(d,Error::sourceOrder);
    if(menuShared_&&menuShared_->menu470PendingFor(*this))return fail(d,Error::sourceOrder);
    if(s.common&&s.common->menuStagePending(this))return fail(d,Error::sourceOrder);
    // Reject positive warning demand before even acquiring a common lease or
    // advancing this collector's invocation. Shared repeats the gate at issuance.
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuShared_->invisible470204AC0(sd))return fail(d,Error::sourceOrder,sd.sourcePC);}
    if(menuInvocationEpoch_==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::sourceOrder);
    if(!s.popupLeased){try{
        s.common=s.popup->sharedHistory();
        if(s.common&&!menuShared_)return fail(d,Error::sourceOrder);
        if(s.common){
            if(!s.common->acquire(this,s.popupRevision,d.controllerPopup))return fail(d,Error::sourceOrder,d.controllerPopup.sourcePC);
            s.popupLeased=true;
            if(!s.prepareGroups(this,d)){s.poisonHistory(this);return false;}
        }else{
            if(!s.prepareGroups(this,d)||!s.popup->acquire(this,s.popupRevision))return fail(d,Error::sourceOrder);
            s.popupLeased=true;
        }
        }catch(...){s.poisonHistory(this);return fail(d,Error::allocation);}}
    if(!s.historyCurrent(this))return fail(d,Error::sourceOrder);
    ++menuInvocationEpoch_;menuRetiredRenderer_=nullptr;
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuSession_||!menuShared_->beginMenu470(*menuSession_,*this,menuInvocationEpoch_,sd)){
            s.failed=true;return fail(d,Error::sourceOrder,sd.sourcePC);}}
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    s.popupVisited=false;
    s.clock=clock;s.active=true;s.ordinary=s.main=s.prompts=s.top=s.scene=s.drained=s.failed=false;
    s.apertureCount=0;s.exitText=false;s.nextOrdinal=0;s.draw={};
    s.submittedVertices.clear();s.drainedVertices.clear();s.submittedFog.clear();s.drainedFog.clear();s.packets.clear();s.order.clear();s.spans.clear();s.states.clear();
    for(std::size_t i=0;i<s.animations.size();++i){
        if((*s.resources->materials())[i].animated&&
            !game::selectOriginalUiTextureAnimationFrame50KG(s.animations[i],s.frames[i])){
            s.failed=true;return fail(d,Error::animation);
        }
    }
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuShared_->continueMenu470AfterCollectorBegin(*this,sd)){
            s.failed=true;return fail(d,Error::sourceOrder,sd.sourcePC);}}
    return true;
}
bool OriginalUiMainDraw50KG::appendOverlay(const game::OriginalUiOverlayQuad50KG& quad,
    std::uint32_t material,float z,OriginalUiMainDrawSegment50KG segment,std::uint16_t widget,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(material>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z)
        return fail(d,Error::geometry);
    if(s.submittedVertices.size()/4>=originalUiMaximumQuads50KG||s.packets.size()>=originalUiMaximumSpans50KG)
        return fail(d,Error::capacity);
    const auto& binding=materials[material];
    if(binding.animated&&s.clock.advanceAnimations&&
        !game::advanceOriginalUiTextureAnimation50KG(s.animations[material],s.clock.milliseconds,s.frames[material]))
        return fail(d,Error::animation);
    const auto frame=s.frames[material];
    if(!binding.images.empty()&&frame>=binding.images.size())return fail(d,Error::animation);
    const auto image=binding.images.empty()?unresolved:binding.images[frame];
    constexpr unsigned permutation[]={0,1,3,2};
    std::array<assets::Vertex48B,4> vertices{};
    for(unsigned i=0;i<4;++i){
        const auto& source=quad.vertices[permutation[i]];auto& vertex=vertices[i];
        if(!std::isfinite(source.x)||!std::isfinite(source.y)||!std::isfinite(source.u)||!std::isfinite(source.v))
            return fail(d,Error::geometry);
        vertex.position[0]=source.x;vertex.position[1]=source.y;vertex.position[2]=z;
        vertex.uv[0]=source.u;vertex.uv[1]=source.v;
        for(unsigned c=0;c<4;++c)vertex.color[c]=quad.sourcePacketColor[c];
        if(vertex.color[3]>128)return fail(d,Error::projection);
    }
    const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4);
    s.submittedVertices.insert(s.submittedVertices.end(),vertices.begin(),vertices.end());
    s.submittedFog.insert(s.submittedFog.end(),4,0.f);
    s.packets.push_back({first,1,material,image,frame,s.nextOrdinal++,quad.sourcePC,widget,segment});
    return true;
}
bool OriginalUiMainDraw50KG::projectControllerRequiredPopup(const game::OriginalUiControllerPopupRender50KG& render,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||s.popupVisited||s.ordinary||s.main||s.top||s.scene||s.drained||s.failed||
       !s.historyCurrent(this))return fail(d,Error::sourceOrder,0x2064C0);
    if(menuShared_&&!menuShared_->menuPopupProjectionCurrent(*this,render))return fail(d,Error::sourceOrder,0x2064C0);
    if(!s.popup->project(render,d.controllerPopup)){s.failed=true;return fail(d,Error::projection,d.controllerPopup.sourcePC);}
    const auto* geometry=s.popup->geometry();if(!geometry)return fail(d,Error::resource);
    for(std::size_t i=0;i<geometry->packetCount();++i){const auto& packet=geometry->packets()[i];
        const auto role=static_cast<std::size_t>(packet.material);if(role>=s.popupIndices.size()){s.failed=true;return fail(d,Error::material);}
        const auto material=s.popupIndices[role];
        if(bool(packet.quad)==bool(packet.text)){s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        if(packet.quad){if(!appendOverlay(*packet.quad,material,packet.packetDepth,OriginalUiMainDrawSegment50KG::controllerPopup,0,d)){s.failed=true;return false;}continue;}
        const auto& text=*packet.text;const auto& binding=(*s.resources->materials())[material];
        if(binding.images.size()!=1||binding.animated||text.count>text.quads.size()||packet.packetColor[3]>128||packet.packetDepth!=16777214.f){
            s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        if(!text.count)continue;
        if(s.submittedVertices.size()/4+text.count>originalUiMaximumQuads50KG||s.packets.size()>=originalUiMaximumSpans50KG){s.failed=true;return fail(d,Error::capacity);}
        for(std::size_t q=0;q<text.count;++q)for(const auto& v:text.quads[q].vertices)
            if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v)){s.failed=true;return fail(d,Error::geometry,packet.sourcePC);}
        const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4),ordinal=s.nextOrdinal++;
        for(std::size_t q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){assets::Vertex48B v{};
            v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=packet.packetDepth;v.uv[0]=glyph.u;v.uv[1]=glyph.v;
            for(unsigned c=0;c<4;++c)v.color[c]=packet.packetColor[c];s.submittedVertices.push_back(v);s.submittedFog.push_back(0);}
        s.packets.push_back({first,text.count,material,binding.images[0],0,ordinal,packet.sourcePC,0,OriginalUiMainDrawSegment50KG::controllerPopup});
    }
    s.popupVisited=true;
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuShared_->completeMenuPopupProjection(*this,render,sd)){s.failed=true;return fail(d,Error::sourceOrder,sd.sourcePC);}}
    return true;
}
bool OriginalUiMainDraw50KG::projectWidgets(bool onTop,bool ancestorsShown,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    auto& s=*storage_;auto* manager=s.resources->manager();const auto* order=manager->managerWidgetOrder();
    if(!order)return fail(d,Error::manager);
    const auto segment=onTop?OriginalUiMainDrawSegment50KG::onTopWidgets:OriginalUiMainDrawSegment50KG::ordinaryWidgets;
    const auto& widgets=onTop?order->onTopWidgets:order->ordinaryWidgets;
    for(const auto id:widgets){
        d.widgetId=id;const auto material=s.resources->widgetMaterial(id);
        if(const auto* geometry=s.resources->textGeometry(id)){
            const auto* descriptor=manager->textDescriptor(id);float z=0;
            if(!descriptor||!packetDepth(*s.resources->video(),descriptor->depth,z))return fail(d,Error::geometry);
            const auto& binding=(*s.resources->materials())[material];
            for(std::size_t lineIndex=0;lineIndex<geometry->lines().size();++lineIndex){
                const auto& line=geometry->lines()[lineIndex];game::OriginalUiMainTextProjection50KG live;
                const game::OriginalUiMainTextMetrics50KG metrics{line.glyphs.fontLineHeight,line.measuredWidth,line.measuredHeight};
                const auto status=manager->projectText(id,lineIndex,metrics,ancestorsShown,live,d.manager);
                if(status==WidgetStatus::hidden)continue;
                if(status!=WidgetStatus::ok)return fail(d,Error::projection);
                if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128||
                    live.position.positionedX!=line.position.positionedX||live.position.positionedY!=line.position.positionedY||
                    live.line.text!=&descriptor->lines[lineIndex])return fail(d,Error::projection);
                // An empty visible line has already prepared source state above.
                // Zero-area ORIGINAL glyphs remain present; do not drop spaces.
                if(!line.glyphs.count)continue;
                if(s.submittedVertices.size()/4+line.glyphs.count>originalUiMaximumQuads50KG||
                    s.packets.size()>=originalUiMaximumSpans50KG)return fail(d,Error::capacity);
                const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4);
                for(std::size_t q=0;q<line.glyphs.count;++q)for(const auto& source:line.glyphs.quads[q].vertices){
                    assets::Vertex48B vertex{};vertex.position[0]=source.x;vertex.position[1]=source.y;vertex.position[2]=z;
                    vertex.uv[0]=source.u;vertex.uv[1]=source.v;
                    for(unsigned c=0;c<4;++c)vertex.color[c]=live.line.packetColor[c];
                    s.submittedVertices.push_back(vertex);s.submittedFog.push_back(0.f);
                }
                s.packets.push_back({first,line.glyphs.count,material,binding.images[0],0,s.nextOrdinal++,0x2e0a30,id,segment});
            }
        }else if(manager->iconDescriptor(id)){
            game::OriginalUiMainIconProjection50KG live;
            const auto status=manager->projectIcon(id,ancestorsShown,live,d.manager);
            if(status==WidgetStatus::hidden)continue;
            if(status!=WidgetStatus::ok)return fail(d,Error::projection);
            const auto& binding=(*s.resources->materials())[material];
            if(live.images!=s.resources->images()||live.initialImageIndex!=binding.images[0])return fail(d,Error::resource);
            if(!appendOverlay(live.layout.quad,material,live.layout.packetDepth,segment,id,d))return false;
        }
        // Empty ID slots are absent from attachment order; Movement has no draw.
    }
    return true;
}
bool OriginalUiMainDraw50KG::projectOrdinaryWidgets(bool shown,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.popupVisited||s.ordinary||s.top||s.failed||s.drained)return fail(d,Error::sourceOrder);
    if(!projectWidgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiMainDraw50KG::projectMainPage(bool shown,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.main||s.top||s.failed||s.drained)return fail(d,Error::sourceOrder);
    s.main=true;if(!shown)return true;
    const auto segment=OriginalUiMainDrawSegment50KG::mainPage;
    const auto bg=s.resources->materialIndex("ui/textures/mmbkg");float z=0;
    if(!packetDepth(*s.resources->video(),15,z)){s.failed=true;return fail(d,Error::geometry);}
    for(const auto& quad:s.resources->background()->quadrants)
        if(!appendOverlay(quad,bg,z,segment,0,d)){s.failed=true;return false;}
    const auto* nav=s.resources->manager()->navigation(1);
    if(!nav){s.failed=true;return fail(d,Error::manager);}
    const auto selected=nav->groups[1].selectedSlot;
    // Source1E64AC falls through without initializing its selected text pointer.
    // Reject that unsupported source state instead of inventing hidden arrows.
    if(selected<1||selected>5){s.failed=true;return fail(d,Error::input,0x1e64ac);}
    const auto* text=s.resources->textGeometry(static_cast<std::uint16_t>(selected+4));
    if(!text||text->lines().empty()){s.failed=true;return fail(d,Error::geometry);}
    const auto right=s.resources->materialIndex("ui/textures/arrwrght");
    const auto left=s.resources->materialIndex("ui/textures/arrwleft");
    const auto& mats=*s.resources->materials();
    // ScreenTexture dimensions/reciprocals are established at cold creation;
    // source material frame selection does not rebind those object dimensions.
    const auto* rightAtlas=s.resources->textures()->texture(mats[right].images[0]);
    const auto* leftAtlas=s.resources->textures()->texture(mats[left].images[0]);
    const auto& line=text->lines()[0];
    const game::OriginalUiMainArrowInput50KG input{selected,line.position.positionedX,line.position.positionedY,
        line.measuredWidth,rightAtlas->texture.width,rightAtlas->texture.height,leftAtlas->texture.width,leftAtlas->texture.height};
    game::OriginalUiMainArrows50KG arrows;
    if(!game::layoutOriginalUiMainArrows50KG(input,arrows)||!packetDepth(*s.resources->video(),1,z)){
        s.failed=true;return fail(d,Error::geometry);
    }
    if(!appendOverlay(arrows.beforeText,right,z,segment,arrows.textWidgetId,d)||
        !appendOverlay(arrows.afterText,left,z,segment,arrows.textWidgetId,d)){s.failed=true;return false;}
    return true;
}
bool OriginalUiMainDraw50KG::projectExitText(const game::OriginalUiApertureHook50KG& event,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active || !s.main || s.exitText || s.apertureCount || s.prompts || s.top || s.failed || s.drained)
        return fail(d,Error::sourceOrder,0x1d75a4);
    auto* owner=s.resources->exitText();
    const auto material=s.resources->materialIndex("font/denmrk16top");
    const auto& materials=*s.resources->materials();
    if(!owner || !owner->prepared() || material>=materials.size())return fail(d,Error::resource,0x1d6bb0);
    const auto count=owner->snapshot().characterCount5C;
    float z=0;
    if(!packetDepth(*s.resources->video(),1,z))return fail(d,Error::unsupportedDepth,0x2e10dc);
    if(s.submittedVertices.size()/4+count>originalUiMaximumQuads50KG || s.packets.size()>=originalUiMaximumSpans50KG)
        return fail(d,Error::capacity,0x2e02b0);
    game::OriginalUiExitTextProjection50KG live;
    if(owner->projectRender2E0170(event,live)!=game::OriginalUiExitTextStatus50KG::ok)
        return fail(d,Error::projection,0x2e0170);
    const auto& binding=materials[material];
    if(!live.glyphs || live.glyphs->count!=count || live.images!=s.resources->images() ||
        live.materialIdentity!=binding.material.identity || binding.animated || binding.images.size()!=1 ||
        live.imageIndex!=binding.images[0] || live.assignedDepth!=1 || live.packetColor[3]>128){
        s.failed=true;return fail(d,Error::resource,0x2e02b0);
    }
    const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4);
    for(std::size_t q=0;q<live.glyphs->count;++q)for(const auto& source:live.glyphs->quads[q].vertices){
        assets::Vertex48B vertex{};
        vertex.position[0]=source.x;vertex.position[1]=source.y;vertex.position[2]=z;
        vertex.uv[0]=source.u;vertex.uv[1]=source.v;
        for(unsigned c=0;c<4;++c)vertex.color[c]=live.packetColor[c];
        s.submittedVertices.push_back(vertex);s.submittedFog.push_back(0.f);
    }
    // Capacity10 fits one original <=36-character ScreenText packet. Retain
    // one source submission, including original zero-area space glyphs.
    s.packets.push_back({first,count,material,live.imageIndex,0,s.nextOrdinal++,live.submitPC,0,
        OriginalUiMainDrawSegment50KG::exitText});
    s.exitText=true;return true;
}
bool OriginalUiMainDraw50KG::projectAperture(std::uint8_t ordinal,
    const game::OriginalUiApertureRectangle50KG& rectangle,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.main||s.top||s.failed||s.drained||ordinal!=s.apertureCount||ordinal>1||
        rectangle.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rectangle.dirty3C)
        return fail(d,Error::sourceOrder,0x1d798c);
    game::OriginalUiOverlayQuad50KG quad;quad.materialName="NoTexture";quad.authoredDepth=1;
    quad.sourcePacketColor={{0,0,0,63}};quad.sourcePC=0x1d798c;
    // Primitive TL,BL,TR,BR -> intermediate ScreenTexture TL,TR,BL,BR.
    constexpr unsigned permutation[]={0,2,1,3};
    for(unsigned i=0;i<4;++i){const auto& v=rectangle.vertices[permutation[i]];quad.vertices[i]={v.x,v.y,0,0};}
    float z=0;if(!packetDepth(*s.resources->video(),1,z)||
        !appendOverlay(quad,s.resources->materialIndex("ui/textures/notexture"),z,
            OriginalUiMainDrawSegment50KG::aperture,0,d)){s.failed=true;return false;}
    ++s.apertureCount;return true;
}
bool OriginalUiMainDraw50KG::projectMainBasePrompts(std::uint8_t mask,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.main||s.apertureCount!=2||s.prompts||s.top||s.failed||s.drained||mask>15)
        return fail(d,Error::sourceOrder,0x1d7a10);
    // Real Main callback mask is0/1. Other shared-page masks are not admitted
    // by this Main session, even though all four cold objects are genuine.
    if(mask>1)return fail(d,Error::input,0x1e6900);
    static const char* names[]={"ui/textures/icon_x","ui/textures/icon_tri","ui/textures/icon_sqr","ui/textures/icon_cir"};
    constexpr float x[]={484,397,285,160};
    constexpr std::uint32_t pcs[]={0x1d7a2c,0x1d7a44,0x1d7a5c,0x1d7a74};
    float z=0;if(!packetDepth(*s.resources->video(),1,z))return fail(d,Error::geometry);
    for(unsigned i=0;i<4;++i)if(mask&(1u<<i)){
        const auto m=s.resources->materialIndex(names[i]);const auto& binding=(*s.resources->materials())[m];
        const auto* atlas=s.resources->textures()->texture(binding.images[0]);
        const float w=float(atlas->texture.width),h=float(atlas->texture.height);
        game::OriginalUiOverlayQuad50KG q;q.materialName=names[i];q.authoredDepth=1;q.sourcePC=pcs[i];
        q.vertices={{{x[i],378,0,0},{x[i]+w,378,1,0},{x[i],378+h,0,1},{x[i]+w,378+h,1,1}}};
        if(!appendOverlay(q,m,z,OriginalUiMainDrawSegment50KG::basePrompts,0,d)){s.failed=true;return false;}
    }
    s.prompts=true;return true;
}
OriginalUiMainSceneInsertion50KG OriginalUiMainDraw50KG::sceneInsertion()const noexcept {
    return {storage_?storage_->nextOrdinal:0};
}
bool OriginalUiMainDraw50KG::recordSceneInsertion(OriginalUiMainSceneInsertion50KG& out,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.top||s.failed||s.drained||s.nextOrdinal==std::numeric_limits<std::uint32_t>::max())
        return fail(d,Error::sourceOrder);
    out={s.nextOrdinal++};return true;
}
bool OriginalUiMainDraw50KG::projectOnTopWidgets(bool shown,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.top||s.failed||s.drained)return fail(d,Error::sourceOrder);
    if(s.apertureCount==1)return fail(d,Error::unfinished);
    if(!projectWidgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiMainDraw50KG::projectMainParticles(game::OriginalUiMainFireOwner50KG& fire,
    const game::OriginalUiMainSceneOwner50KG& scene,const game::OriginalUiMainParticleRenderState50KG& renderState,
    OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.top||s.scene||s.failed||s.drained)return fail(d,Error::sourceOrder);
    const auto* definition=scene.definition();const auto* material=scene.particleMaterial();
    if(!scene.prepared()||!definition||!material||material->identity()!=s.resources->particleMaterial()->identity)
        return fail(d,Error::resource);
    // The scene fog entity runs after the manager fog entity and rewrites the
    // source camera near/far before ParticleCode sees its constants. Consume
    // that concrete prepared state, including exact packed FOGCOL conversion.
    for(unsigned c=0;c<3;++c){
        const auto value=renderState.fogColor[c];
        if(!std::isfinite(value)||value<0||value>1)return fail(d,Error::input);
        s.fogColor[c]=value;
    }
    const auto& camera=renderState.particleCamera;const auto& constants=renderState.constants;
    const game::OriginalUiMainParticleView50KG view{{camera.combinedSourceScreen250[2],camera.combinedSourceScreen250[6],
        camera.combinedSourceScreen250[10],camera.combinedSourceScreen250[14]},camera.focalPixels1D8};
    d.fire=fire.preparePackets(view);
    if(d.fire!=game::OriginalUiMainFireStatus50KG::complete){s.failed=true;return fail(d,Error::projection);}
    const auto count=fire.packetCount();const auto* source=fire.packets();
    if(count>game::originalUiMainParticleCapacity50KG||(count&&!source)||
        s.submittedVertices.size()/4+count>originalUiMaximumQuads50KG)return fail(d,Error::capacity);
    // Each source run is at most12 and chooses ONE of the two material states.
    // Header validation occurs before projecting any of this batch.
    for(std::size_t start=0;start<count;){
        const auto signedCount=source[start].signedRunCount;
        if(signedCount==0||signedCount<-12||signedCount>12)return fail(d,Error::input);
        const auto n=static_cast<std::size_t>(signedCount<0?-signedCount:signedCount);
        if(n>count-start)return fail(d,Error::input);
        for(std::size_t j=1;j<n;++j)if(source[start+j].signedRunCount)return fail(d,Error::input);
        start+=n;
    }
    for(std::size_t start=0;start<count;){
        const auto signedCount=source[start].signedRunCount;
        const auto n=static_cast<std::size_t>(signedCount<0?-signedCount:signedCount);
        const std::uint8_t state=signedCount<0?1:0;
        const auto first=static_cast<std::uint32_t>(s.submittedVertices.size()/4);
        for(std::size_t j=0;j<n;++j){
            game::OriginalUiMainParticleQuad50KG quad;
            d.particle=game::projectOriginalUiMainParticleQuad50KG(source[start+j],state,camera,constants,quad);
            if(d.particle!=game::OriginalUiMainParticleGeometryStatus50KG::complete){s.failed=true;return fail(d,Error::projection);}
            if(!quad.visible)continue;
            s.submittedVertices.insert(s.submittedVertices.end(),quad.vertices.begin(),quad.vertices.end());
            s.submittedFog.insert(s.submittedFog.end(),quad.fogFactors.begin(),quad.fogFactors.end());
        }
        const auto quads=static_cast<std::uint32_t>(s.submittedVertices.size()/4)-first;
        if(quads){
            if(s.packets.size()>=originalUiMaximumSpans50KG){s.failed=true;return fail(d,Error::capacity);}
            s.packets.push_back({first,quads,unresolved,s.resources->particleImageIndex(),state,s.nextOrdinal++,
                0x162248,0,OriginalUiMainDrawSegment50KG::particles});
        }
        start+=n;
    }
    s.scene=true;return true;
}
bool OriginalUiMainDraw50KG::drain(OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    if(!s.active||!s.popupVisited||!s.ordinary||!s.main||!s.prompts||!s.top||!s.scene||s.failed||s.drained||s.apertureCount!=2||s.historyPoisoned||!s.historyCurrent(this)||s.frameGeneration==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::unfinished);
    try {
    const auto& materials=*s.resources->materials();
    if(!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,Error::publication);
    s.pendingHistory=s.history;s.menuUses.clear();
    s.order.clear();
    for(std::size_t i=0;i<s.packets.size();++i)
        if(s.packets[i].materialIndex==unresolved||materials[s.packets[i].materialIndex].material.passes[0].enabled!=0)s.order.push_back(static_cast<std::uint32_t>(i));
    std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
        const auto ga=s.packets[a].materialIndex==unresolved?0:s.leaf[s.packets[a].materialIndex]+1;
        const auto gb=s.packets[b].materialIndex==unresolved?0:s.leaf[s.packets[b].materialIndex]+1;
        return ga!=gb?ga<gb:s.packets[a].submissionOrdinal<s.packets[b].submissionOrdinal;
    });
    std::uint32_t total=0;for(const auto i:s.order)total+=s.packets[i].quadCount;
    if(!total)return fail(d,Error::unfinished);
    s.drainedVertices.clear();s.drainedFog.clear();s.spans.clear();s.states.clear();
    std::uint32_t terminalMaterial=unresolved;
    for(const auto i:s.order){
        const auto& p=s.packets[i];
        if(p.warning470&&(!menuShared_||!menuShared_->menuWarning470PacketCurrent(*p.warning470)||
           p.warning470->sourceOwner_!=this||p.materialIndex>=materials.size()||
           materials[p.materialIndex].registryToken!=static_cast<std::uint32_t>(p.warning470->materialToken_))){
            s.failed=true;return fail(d,Error::sourceOrder,p.sourcePC);}
        const auto first=static_cast<std::uint32_t>(s.drainedVertices.size()/4);
        OriginalUiMaterialSpan50KG converted;bool okay=false;
        if(p.materialIndex==unresolved)okay=convertOriginalUiMainParticleSpan50KG(*s.resources->particleMaterial(),
            static_cast<std::uint8_t>(p.frameIndex),{first,p.quadCount,total,p.imageIndex,s.gpu->textureCount()},converted);
        else {
            const auto& material=materials[p.materialIndex];
            okay=material.images.empty()?
                convertOriginalUiUntexturedMaterialSpan50KG(material.untextured,{first,p.quadCount,total},converted):
                convertOriginalUiMaterialSpan50KG(material.policies[p.frameIndex],
                    {first,p.quadCount,total,p.imageIndex,s.gpu->textureCount()},converted);
            const auto representative=s.representatives[s.leaf[p.materialIndex]];
            const auto& source=materials[representative].material.passes[0];
            assets::OriginalUiMaterialBindingDiagnostic50KG detail;
            if(source.textureCount&&(!s.resources->sourceImageBank()||
                !s.pendingHistory[representative].recordDrainedBinding(*s.resources->sourceImageBank(),p.imageIndex,detail)))
                return fail(d,Error::material,detail.sourcePC);
            if(s.common)s.menuUses.push_back({material.registryToken,materials[representative].registryToken,
                static_cast<std::uint32_t>(s.spans.size()),p.imageIndex});
            terminalMaterial=representative;
        }
        if(!okay){s.failed=true;return fail(d,Error::material);}
        const auto begin=s.submittedVertices.begin()+std::size_t(p.firstQuad)*4;
        s.drainedVertices.insert(s.drainedVertices.end(),begin,begin+std::size_t(p.quadCount)*4);
        const auto fogBegin=s.submittedFog.begin()+std::size_t(p.firstQuad)*4;
        s.drainedFog.insert(s.drainedFog.end(),fogBegin,fogBegin+std::size_t(p.quadCount)*4);
        s.spans.push_back(converted.span);s.states.push_back(converted.native);
    }
    s.draw={};
    s.draw.draw.textures=s.gpu.get();s.draw.draw.texturePublication=s.publication;
    s.draw.draw.vertices=s.drainedVertices.data();s.draw.draw.quadCount=total;
    s.draw.draw.spans=s.spans.data();s.draw.draw.spanCount=static_cast<std::uint32_t>(s.spans.size());
    s.draw.draw.canvasWidth=640;s.draw.draw.canvasHeight=448;
    // Actual Main PageInfo background, retained in the authenticated page owner.
    const auto* page=s.resources->manager()->page(1);
    for(unsigned c=0;c<3;++c)s.draw.draw.clearRgba[c]=page->backgroundColor[c]/255.f;
    s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
    s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
    s.draw.particleFog=s.drainedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.drainedFog.size());
    for(unsigned c=0;c<3;++c)s.draw.particleFogColor[c]=s.fogColor[c];
    s.draw.opaquePresentationAlphaOnly=false;
    if(terminalMaterial==unresolved)return fail(d,Error::unfinished);
    const auto& terminalBinding=materials[terminalMaterial];const auto& terminalPass=terminalBinding.material.passes[0];
    OriginalUiSourceTerminal50KN terminal{};terminal.canonicalToken=terminalBinding.registryToken;
    terminal.pass=terminalPass.pass;terminal.leaf=s.leaf[terminalMaterial];
    // This immutable Main owner has one successful cold bank publication
    // and one source group construction. Neither is rebuilt on page changes;
    // the separate frameGeneration records each real accepted submission.
    terminal.registryGeneration=1;terminal.bankGeneration=1;
    if(!s.pendingHistory[terminalMaterial].copyEffectiveGs(terminal.gs))return fail(d,Error::material);
    for(std::size_t f=0;f<terminalPass.gs.fields.size();++f){terminal.constructionFieldSourcePC[f]=terminalPass.gs.fields[f].sourcePC;
        terminal.constructionFieldByteOffset[f]=terminalPass.gs.fields[f].byteOffset;}
    terminal.enabled=terminalPass.enabled;terminal.textureCount=terminalPass.textureCount;terminal.textureCoordinateIndex=terminalPass.textureCoordinateIndex;
    terminal.lodCalculationMethod=terminalPass.lodCalculationMethod;terminal.maximumMipLevel=terminalPass.maximumMipLevel;
    terminal.magFilter=terminalPass.magFilter;terminal.minFilter=terminalPass.minFilter;terminal.lodL=terminalPass.lodL;terminal.lodK=terminalPass.lodK;
    for(std::size_t i=0;i<s.popupIndices.size();++i)s.popupPendingHistory[i]=s.pendingHistory[s.popupIndices[i]];
    s.drained=true;frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;
    if(s.common){
        if(!s.resources->sourceImageBank()){s.failed=true;return fail(d,Error::resource);}
        for(std::size_t i=0;i<s.historyIndices.size();++i)s.commonPending[i]=s.pendingHistory[s.historyIndices[i]];
        if(!s.common->stageMenu(this,s.popupRevision,frame_,*s.resources->sourceImageBank(),s.menuUses,s.commonPending,d.controllerPopup)){
            s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,Error::material,d.controllerPopup.sourcePC);}}
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuShared_->sealMenu470Frame(*this,s.frameGeneration,sd)){
            s.failed=true;return fail(d,Error::sourceOrder,sd.sourcePC);}}
    return true;
    }catch(...){s.failed=true;frame_.native_=nullptr;frame_.owner_=nullptr;return fail(d,Error::allocation);}
}
const OriginalUiNativeDraw50KG* OriginalUiMainDraw50KG::draw()const noexcept{return storage_&&storage_->drained&&!storage_->failed?&storage_->draw:nullptr;}
const OriginalUiSourceFrame50KM* OriginalUiMainDraw50KG::sourceFrame() const noexcept {
    if(!storage_)return nullptr;const auto& s=*storage_;
    if(!s.drained||s.failed||s.historyPoisoned||s.submissionPending||!s.popupLeased||
       !s.historyCurrent(this)||!frame_.native_)return nullptr;
    if(menuShared_&&!menuShared_->menu470FrameCurrent(*this,frame_.frameGeneration_)&&
       !(menuAcceptedEpoch_==menuInvocationEpoch_&&s.acknowledgedGeneration==frame_.frameGeneration_))return nullptr;
    return &frame_;
}
bool OriginalUiMainDraw50KG::sourceTeardownPending() const noexcept {
    return storage_&&(storage_->submittedRenderer||(storage_->common&&storage_->popupLeased)||
        (menuShared_&&menuShared_->menu470PendingFor(*this)));
}
bool OriginalUiMainDraw50KG::retireSourceRenderer(VehicleWindowGl50DS& renderer,OriginalUiMainDrawDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.submissionPending||(s.submittedRenderer&&s.submittedRenderer!=&renderer))return fail(d,Error::sourceOrder);
    VehicleWindowDiagnostic50DS detail;
    if(!renderer.retireOriginalUiSourceOwner(*this,detail))return fail(d,Error::publication);
    s.submittedRenderer=nullptr;menuRetiredRenderer_=&renderer;
    if(menuShared_){OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!menuShared_->retireMenu470AfterRenderer(*this,renderer,sd))return fail(d,Error::sourceOrder,sd.sourcePC);}
    if(s.common){if(!s.common->releaseMenu(this,&renderer))return fail(d,Error::sourceOrder);}
    else s.popup->release(this,true);
    s.popupLeased=false;frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;return true;
}
bool OriginalUiMainDraw50KG::beginSourceSubmission(std::uint64_t generation,const VehicleWindowGl50DS& renderer) const noexcept {
    if(!storage_||!sourceFrame()||generation!=frame_.frameGeneration_)return false;auto& s=*storage_;
    if(s.submissionPending||s.historyPoisoned||(s.submittedRenderer&&s.submittedRenderer!=&renderer)||
        generation<=s.attemptedGeneration||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    if(s.common&&!s.common->beginMenu(this,s.popupRevision,frame_,renderer))return false;
    if(menuShared_&&!menuShared_->beginMenu470Submission(*this,generation,renderer)){
        if(s.common&&!s.common->cancelMenuBegin(this,s.popupRevision,frame_,renderer))s.poisonHistory(this);
        menuShared_->failMenu470Submission(*this);return false;}
    s.submittedRenderer=&renderer;s.attemptedGeneration=generation;s.submissionPending=true;return true;
}
void OriginalUiMainDraw50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started) const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){
        s.poisonHistory(this);if(menuShared_)menuShared_->failMenu470Submission(*this);return;}
    s.submissionPending=false;
    bool historyOkay=s.pendingHistory.size()==s.history.size();
    if(!historyOkay)s.poisonHistory(this); // Preflight every local invariant before either accepted store.
    if(s.common)historyOkay=s.common->finishMenu(this,s.popupRevision,frame_,accepted,started)&&historyOkay;
    else if(accepted)historyOkay=historyOkay&&s.popup->commit(this,s.popupRevision,s.popupPendingHistory);
    if(!historyOkay){s.poisonHistory(this);if(menuShared_)menuShared_->failMenu470Submission(*this);return;}
    if(accepted){s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;}
    else if(started)s.poisonHistory(this);
    if(menuShared_){if(!menuShared_->finishMenu470Submission(*this,generation,accepted,started)){
            s.poisonHistory(this);menuShared_->failMenu470Submission(*this);return;}
        if(accepted)menuAcceptedEpoch_=menuInvocationEpoch_;}
}
const std::vector<OriginalUiMainSubmittedPacket50KG>* OriginalUiMainDraw50KG::packets()const noexcept{return storage_?&storage_->packets:nullptr;}
const std::vector<std::uint32_t>* OriginalUiMainDraw50KG::drainedPacketIndices()const noexcept{return storage_&&storage_->drained?&storage_->order:nullptr;}
}
