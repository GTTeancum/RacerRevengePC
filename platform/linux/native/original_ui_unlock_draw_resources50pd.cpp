#include "original_ui_unlock_draw_resources50pd.h"
#include "original_ui_material_adapter50kg.h"
#include "original_ui_unlock_model_material50pd.h"
#include <cmath>
#include <cstdio>
#include <utility>

namespace rr::platform {
namespace {
using Error=OriginalUiUnlockDrawError50PD;using Diagnostic=OriginalUiUnlockDrawDiagnostic50PD;using Archive=assets::OriginalUiArchiveId50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
// Failure receipts carry the source line to the log (diagnostic only).
bool failAt(int line,Diagnostic& d,Error e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;std::fprintf(stderr,"unlock_draw_resources failure line=%d error=%u pc=%x\n",line,unsigned(e),pc);return false;}
#define fail(...) failAt(__LINE__,__VA_ARGS__)
char fold(char c) noexcept {return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;
}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
bool validVideo(const OriginalUiUnlockVideoOwner50PD& v) noexcept {
    return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;
}
std::uint64_t imageIdentity(Archive id,const assets::OriginalUiMemberView50KG& m,std::size_t off) noexcept {
    const auto* a=assets::originalUiArchiveIdentity50KG(id);if(!a||off>0xffffffffu)return 0;
    for(std::size_t i=0;i<a->memberCount;++i)if(assets::originalUiMemberIdentity50KG(id,i)==m.identity)
        return (std::uint64_t(static_cast<std::uint8_t>(id)+1u)<<56)|(std::uint64_t(i+1u)<<32)|std::uint64_t(off);
    return 0;
}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
std::uint32_t find(const std::vector<OriginalUiUnlockMaterialBinding50PD>& l,const std::string& n) noexcept {
    for(std::size_t i=0;i<l.size();++i)if(same(l[i].registeredName,n))return static_cast<std::uint32_t>(i);return unresolved;
}
// Same authenticated Mat_Stbl token stream (3235 tokens) as every menu owner.
bool registry(const assets::OriginalUiMemberView50KG& t,std::vector<OriginalUiUnlockMaterialBinding50PD>& b) {
    if(!t.identity||!t.data||t.size!=70962)return false;
    const auto delim=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    std::vector<bool> found(b.size(),false);std::size_t cursor=0;std::uint32_t token=0;
    while(cursor<t.size){
        while(cursor<t.size&&delim(t.data[cursor]))++cursor;if(cursor==t.size)break;
        const auto start=cursor;while(cursor<t.size&&!delim(t.data[cursor]))++cursor;
        for(std::size_t i=0;i<b.size();++i){auto& row=b[i];if(row.registeredName.size()!=cursor-start)continue;
            std::size_t j=0;for(;j<row.registeredName.size()&&fold(char(t.data[start+j]))==fold(row.registeredName[j]);++j){}
            if(j!=row.registeredName.size())continue;
            if(found[i]){
                // 298C80 returns the FIRST matching token. Admit only the two
                // authenticated later occurrences (same rule as the Rider row
                // owner): Mars Guo's 08c_3 (pod 8) and Watto's pod_wndw (pod 25).
                // The first token stays; nothing else is accepted.
                const bool mars=same(row.registeredName,"ui/textures/08c_3")&&row.registryToken==2386&&token==2387&&start==55274;
                const bool window=same(row.registeredName,"ui/textures/pod_wndw")&&row.registryToken==2490&&token==3221&&start==70650;
                if(!mars&&!window)return false;continue;}
            found[i]=true;row.registryToken=token;row.registryIdentity=t.identity;}
        ++token;
    }
    if(token!=3235)return false;for(bool f:found)if(!f)return false;return true;
}
game::OriginalUiOverlayQuad50KG quad(const char* m,float x,float y,float w,float h,float u0,float v0,float u1,float v1,
    float depth,std::uint32_t pc) noexcept {
    game::OriginalUiOverlayQuad50KG q;q.materialName=m;q.authoredDepth=depth;q.sourcePC=pc;
    q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};return q;
}
std::string backgroundName(unsigned r) {std::string name="00_bkg";name[0]=char('0'+r/10);name[1]=char('0'+r%10);return name;}
}
struct OriginalUiUnlockDrawResources50PD::Storage {
    OriginalUiUnlockColdInput50PD owners;assets::OriginalUiMaterialTemplates50KG templates;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;assets::OriginalUiImageBank50KG imageBank;
    game::OriginalUiUnlockPage50PD page;game::OriginalUiUnlockWidgets50PD widgets;
    std::vector<OriginalUiUnlockMaterialBinding50PD> bindings;std::vector<MaterialTextureSampler50EB> samplers;
    std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,game::originalUiUnlockWidgetStorageCount50PD> text;
    std::array<std::uint32_t,game::originalUiUnlockWidgetStorageCount50PD> widgetMaterial{};
    std::array<OriginalUiUnlockSprites50PD,27> sprites{};
    std::array<OriginalUiUnlockPrompt50PD,4> prompts{};
    std::array<std::uint32_t,4> galleryImages{{unresolved,unresolved,unresolved,unresolved}};
    // Page+154 starts null; absent PageInfo.Background is *INVALID* (2A5F4C).
    std::uint32_t currentBackgroundImage=unresolved;
    game::OriginalUiExitText50KG exitText;
    game::OriginalUiUnlockCamera50PD camera;
    game::OriginalUiUnlockModels50PD models;
    struct ModelKey {const assets::OriginalUiRiderModelMaterials50KG* materials=nullptr;std::uint32_t material=0,pass=0,binding=0;};
    std::vector<ModelKey> modelKeys;
    struct ModelOwner {const assets::OriginalUiRiderModelMaterials50KG* materials=nullptr;std::uint32_t owner=0;};
    std::vector<ModelOwner> modelOwners;
    std::uint32_t binderBinding=unresolved,binderOwner=0;
    std::vector<std::uint32_t> binderImages;
};
OriginalUiUnlockDrawResources50PD::OriginalUiUnlockDrawResources50PD() noexcept=default;
OriginalUiUnlockDrawResources50PD::~OriginalUiUnlockDrawResources50PD()=default;
bool OriginalUiUnlockDrawResources50PD::load(const OriginalUiUnlockColdInput50PD& in,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyLoaded);
    if(!authentic(in.appStart,Archive::appStart)||!authentic(in.unlock,Archive::unlock)||!authentic(in.fonts,Archive::fonts)||
       !authentic(in.backgrounds,Archive::backgrounds)||!authentic(in.controllerPopup,Archive::controllerPopup)||
       !authentic(in.overlays,Archive::overlays))return fail(d,Error::archive);
    if(!validVideo(in.video))return fail(d,Error::videoOwner);
    if(!in.english||!in.english->loaded()||!in.runtime.overrideKnown)return fail(d,Error::input);
    try {
        auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
        if(s->page.bind(in.unlock,d.page)!=game::OriginalUiOptionsMenuStatus50KG::ok)return fail(d,Error::page);
        if(!s->widgets.bind(in.unlock,in.english,d.widgets))return fail(d,Error::widgets);
        if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material);
        std::vector<assets::OriginalUiImageSource50KG> sources;
        const auto addSource=[&](const assets::OriginalUiImageSource50KG& x)->std::uint32_t{
            for(std::size_t i=0;i<sources.size();++i){const auto& o=sources[i];
                if(o.member.identity==x.member.identity&&o.pstOffset==x.pstOffset&&o.pstBytes==x.pstBytes&&o.cel.identity==x.cel.identity)
                    return static_cast<std::uint32_t>(i);}
            sources.push_back(x);return static_cast<std::uint32_t>(sources.size()-1);};
        const auto addImages=[&](OriginalUiUnlockMaterialBinding50PD& b,const assets::OriginalUiArchive50KG& owner,
            const std::string& name,bool font)->bool{
            const auto member=owner.find((font?"font/":"ui/textures/")+name+".pst");if(!member.identity||!member.data)return fail(d,Error::resource);
            const auto cel=font?owner.find("font/"+name+".cel"):assets::OriginalUiMemberView50KG{};
            if(font&&(!cel.identity||!cel.data))return fail(d,Error::resource);
            const auto identity=imageIdentity(owner.identity()->id,member,0);if(!identity)return fail(d,Error::resource);
            b.images.push_back(addSource({identity,member,0,member.size,cel}));
            if(b.material.passes.size()!=1)return fail(d,Error::material);const auto& pass=b.material.passes[0];
            if(pass.textureCount!=1||!same(pass.textureName,name))return fail(d,Error::material);
            if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation,0x29b950);
            return true;};
        const auto authored=[&](const std::string& material,const char* texture,const assets::OriginalUiArchive50KG* owner,bool font)->bool{
            OriginalUiUnlockMaterialBinding50PD b;b.registeredName=material;
            if(!assets::readOriginalUiMaterial50KG(*in.unlock,b.registeredName+".psm",s->templates,assets::OriginalUiTemplate50KG::screenMt,
                in.runtime,b.material,d.material))return fail(d,Error::material);
            if(texture&&!addImages(b,*owner,texture,font))return false;
            if(!texture&&(b.material.passes.size()!=1||b.material.passes[0].textureCount!=0))return fail(d,Error::material);
            s->bindings.push_back(std::move(b));return true;};
        // Every authored UIUnlock PSM used by the widgets, the overlay band
        // (overlay1 PST lives in CONTPOP) and the aperture's NoTexture.
        if(!authored("font/denmrk16top","denmrk16",in.fonts.get(),true)||!authored("font/denmrk16","denmrk16",in.fonts.get(),true)||
           !authored("font/denmrk18","denmrk18",in.fonts.get(),true)||!authored("font/venus16top","venus16",in.fonts.get(),true)||
           !authored("font/swhutt44","swhutt44",in.fonts.get(),true)||
           !authored("ui/textures/overlay1","overlay1",in.controllerPopup.get(),false)||
           !authored("ui/textures/notexture",nullptr,nullptr,false))return false;
        // Shared page constructor1D63B0 prompts (no PSM; OVERLAYS cross/triangle).
        constexpr const char* promptNames[]={"icon_x","icon_tri","icon_sqr","icon_cir"};
        for(std::size_t i=0;i<4;++i){
            OriginalUiUnlockMaterialBinding50PD b;b.registeredName=std::string("ui/textures/")+promptNames[i];
            if(!assets::readOriginalUiUnlockPromptFallbackMaterial50KG(*in.unlock,*in.overlays,static_cast<assets::OriginalUiMainPrompt50KG>(i),
                s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
            if(!addImages(b,i<2?*in.overlays:*in.unlock,promptNames[i],false))return false;
            s->prompts[i].material=static_cast<std::uint32_t>(s->bindings.size());s->bindings.push_back(std::move(b));
        }
        // 20A000 sprites. UIUnlock owns the %02d_bkg PSMs, UIBKG their PSTs.
        // Init builds them for the rivals of course orders 4..12 and pods
        // 3,1,14,25,21,26 (209C2C..209CC4). in.spritePods carries that exact
        // set from the profile; zero keeps the stand-in (every authored PSM).
        if(in.spritePods){
            if(in.spritePods&1u||(in.spritePods>>27))return fail(d,Error::input,0x209C2C);
            for(auto pod:game::originalUiUnlockFixedSpritePods50PD)if(!(in.spritePods&(1u<<pod)))return fail(d,Error::input,0x209C80);
        }
        for(std::uint8_t pod=1;pod<=26;++pod){
            const auto name=backgroundName(pod);
            if(in.spritePods&&!(in.spritePods&(1u<<pod)))continue;
            if(!in.unlock->find("ui/textures/"+name+".psm").identity){
                if(in.spritePods)return fail(d,Error::resource,0x20A04C);continue;}
            OriginalUiUnlockMaterialBinding50PD b;b.registeredName="ui/textures/"+name;
            if(!assets::readOriginalUiMaterial50KG(*in.unlock,b.registeredName+".psm",s->templates,assets::OriginalUiTemplate50KG::screenMt,
                in.runtime,b.material,d.material))return fail(d,Error::material,0x20A04C);
            if(!addImages(b,*in.backgrounds,name,false))return false;
            s->sprites[pod].pod=pod;s->sprites[pod].built=true;s->sprites[pod].material=static_cast<std::uint32_t>(s->bindings.size());
            s->bindings.push_back(std::move(b));
        }
        for(auto pod:game::originalUiUnlockFixedSpritePods50PD)if(!s->sprites[pod].built)return fail(d,Error::resource,0x209C80);
        // Raw +8C page backgrounds (2A6530/2A6E90): UIUnlock art PSTs, no PSM.
        for(unsigned tier=1;tier<=3;++tier){
            const auto* name=game::originalUiUnlockGalleryTexture50PD(static_cast<game::OriginalUiUnlockGallery50PD>(tier));
            const auto raw=in.unlock->find(std::string("ui/textures/")+name+".pst");const auto identity=imageIdentity(Archive::unlock,raw,0);
            if(!raw.identity||!raw.data||raw.size!=287772||!identity||in.unlock->find(std::string("ui/textures/")+name+".psm").identity)
                return fail(d,Error::resource,0x2A6588);
            s->galleryImages[tier]=addSource({identity,raw,0,raw.size,{}});
        }
        // 3D widgets: camera/fog/lights, then the cold pod+pilot population of
        // every candidate pod, one binding per ORIGINAL pass (registry rows
        // "ui/textures/<scene material>"), plus the binder helper material.
        if(!s->camera.bind(in.unlock,d.camera))return fail(d,Error::geometry,d.camera.sourcePC);
        {game::OriginalUiUnlockModelInput50PD modelInput;modelInput.unlock=in.unlock;modelInput.appStart=in.appStart;
            modelInput.podTextures=in.podTextures;modelInput.runtime=in.runtime;modelInput.ipumCache=in.ipumCache;
            modelInput.page=&s->page;modelInput.podMask=in.candidatePods;
            if(!s->models.prepareCold(modelInput,d.models))return fail(d,Error::resource,d.models.sourcePC);}
        struct PendingImage {const assets::OriginalUiRiderModelMaterials50KG* materials=nullptr;std::uint32_t local=unresolved;bool binder=false;};
        std::vector<PendingImage> pendingImages(s->bindings.size());
        const auto addModel=[&](const assets::OriginalUiRiderModelMaterials50KG& materials,const assets::Scene48D& scene)->bool{
            if(scene.materials.size()!=materials.materialCount())return fail(d,Error::material,0x26E640);
            for(std::uint32_t m=0;m<materials.materialCount();++m){
                const auto name="ui/textures/"+scene.materials[m];
                const auto existing=find(s->bindings,name);
                for(std::uint32_t p=0;p<materials.passCount(m);++p){
                    const auto* pass=materials.pass(m,p);if(!pass||pass->source.pass!=p+1)return fail(d,Error::material,0x26E640);
                    const auto* provenance=pass->imageIndex==unresolved||!materials.images()?nullptr:materials.images()->sourceProvenance(pass->imageIndex);
                    if(pass->imageIndex!=unresolved&&!provenance)return fail(d,Error::resource,0x26E640);
                    std::uint32_t binding=unresolved;
                    if(existing!=unresolved){
                        // Same registered row from another model or slot: the SAME
                        // authored pass and image member are required, never a merge.
                        binding=existing+p;
                        if(binding>=s->bindings.size()||!s->bindings[binding].model||!same(s->bindings[binding].registeredName,name)||
                           s->bindings[binding].material.passes[0].identity!=pass->source.identity||
                           s->bindings[binding].material.passes[0].pass!=pass->source.pass)return fail(d,Error::material,0x26E640);
                        const auto& prior=pendingImages[binding];
                        const auto* priorProvenance=prior.local==unresolved||!prior.materials->images()?nullptr:prior.materials->images()->sourceProvenance(prior.local);
                        if(bool(priorProvenance)!=bool(provenance)||(provenance&&(priorProvenance->memberIdentity!=provenance->memberIdentity||
                           priorProvenance->pstOffset!=provenance->pstOffset)))return fail(d,Error::material,0x26E640);
                    }else{
                        OriginalUiUnlockMaterialBinding50PD b;b.registeredName=name;b.model=true;b.material.passes.push_back(pass->source);
                        binding=static_cast<std::uint32_t>(s->bindings.size());s->bindings.push_back(std::move(b));
                        pendingImages.push_back({&materials,pass->imageIndex,false});
                    }
                    s->modelKeys.push_back({&materials,m,p,binding});
                }
            }
            return true;};
        for(const auto& c:*s->models.cold()){
            if(!addModel(*c.podMaterials,*c.podAssets->scene())||!addModel(*c.characterMaterials,*c.characterAssets->scene()))return false;
        }
        if(const auto binder=s->models.binderAssets()){
            OriginalUiUnlockMaterialBinding50PD b;b.registeredName="ui/textures/binder";b.model=true;b.material.passes.push_back(*binder->pass());
            s->binderBinding=static_cast<std::uint32_t>(s->bindings.size());s->bindings.push_back(std::move(b));
            pendingImages.push_back({nullptr,unresolved,true});
        }
        if(!registry(in.appStart->find("AppStart/Mat_Stbl.txt"),s->bindings))return fail(d,Error::registry,0x298724);
        auto originalImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
        if(!originalImages->load(sources,d.resource)){
            const auto& r=d.resource;const auto image=r.image<sources.size()&&sources[r.image].member.identity?sources[r.image].member.identity->name:"?";
            std::fprintf(stderr,"unlock_draw_resources page images error=%u image=%zu member=%s bytes=%zu texture=%s/%s\n",
                unsigned(r.error),r.image,image,r.image<sources.size()?sources[r.image].pstBytes:0,r.texture.code.c_str(),r.texture.detail.c_str());
            return fail(d,Error::resource);}
        assets::OriginalUiImageBankInput50KG bankInput;
        bankInput.archives={in.appStart,in.unlock,in.fonts,in.backgrounds,in.controllerPopup,in.overlays};bankInput.owners={originalImages};
        if(in.podTextures)bankInput.archives.push_back(in.podTextures);
        bankInput.ipumCache=in.ipumCache;
        for(const auto& c:*s->models.cold())for(const auto* materials:{c.podMaterials.get(),c.characterMaterials.get()})
            if(materials->images()){s->modelOwners.push_back({materials,static_cast<std::uint32_t>(bankInput.owners.size())});
                bankInput.owners.push_back(materials->images());}
        if(const auto binder=s->models.binderAssets()){
            s->binderOwner=static_cast<std::uint32_t>(bankInput.owners.size());
            bankInput.owners.emplace_back(binder,binder->images());
        }
        assets::OriginalUiImageBankDiagnostic50KG bankDiagnostic;
        if(!s->imageBank.load(bankInput,bankDiagnostic))return fail(d,Error::resource);
        for(std::uint32_t i=0;i<sources.size();++i){const auto* map=s->imageBank.mapping(0,i);if(!map||map->combinedIndex!=i)return fail(d,Error::resource);}
        // Resolve every 3D pass image to its combined bank index.
        for(std::size_t i=0;i<s->bindings.size();++i){auto& b=s->bindings[i];if(!b.model)continue;const auto& pending=pendingImages[i];
            if(pending.binder){const auto binder=s->models.binderAssets();const auto frames=binder->images()->textures()->textureCount();
                for(std::uint32_t f=0;f<frames;++f){const auto* map=s->imageBank.mapping(s->binderOwner,f);if(!map)return fail(d,Error::resource,0x150450);
                    b.images.push_back(map->combinedIndex);s->binderImages.push_back(map->combinedIndex);}
            }else if(pending.local!=unresolved){
                std::uint32_t owner=unresolved;for(const auto& o:s->modelOwners)if(o.materials==pending.materials)owner=o.owner;
                const auto* map=owner==unresolved?nullptr:s->imageBank.mapping(owner,pending.local);if(!map)return fail(d,Error::resource,0x26E640);
                b.images.push_back(map->combinedIndex);
            }
        }
        s->images=s->imageBank.images();const auto* bank=s->images->textures();if(!bank)return fail(d,Error::resource);
        s->samplers.resize(bank->textureCount());std::vector<bool> samplerSet(bank->textureCount(),false);
        for(std::size_t i=0;i<s->bindings.size();++i){
            d.materialIndex=static_cast<std::uint32_t>(i);auto& b=s->bindings[i];const auto& pass=b.material.passes[0];
            if(b.model){
                // Rider pass dispatch; the shared atlas keeps one deterministic
                // default sampler (first use), as the Rider resource owner does.
                if(b.images.empty()){OriginalUiMaterialSpan50KG converted;
                    if(!convertOriginalUiUnlockModelPass50PD(pass,*s->images,unresolved,1,converted,d.material))return fail(d,Error::material,0x26E640);continue;}
                for(auto image:b.images){d.imageIndex=image;OriginalUiMaterialSpan50KG converted;
                    if(!convertOriginalUiUnlockModelPass50PD(pass,*s->images,image,1,converted,d.material))return fail(d,Error::material,0x26E640);
                    if(!samplerSet[image])s->samplers[image]=converted.native.sampler;samplerSet[image]=true;}
                continue;
            }
            if(b.images.empty()){if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))return fail(d,Error::material);continue;}
            for(auto image:b.images){
                d.imageIndex=image;const auto* decoded=s->images->decodedTexture(image);assets::OriginalUiNativeMaterial50KG policy;
                if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))return fail(d,Error::material);
                OriginalUiMaterialSpan50KG converted;
                if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,image,bank->textureCount()},converted))return fail(d,Error::material);
                if(samplerSet[image]&&!sameSampler(s->samplers[image],converted.native.sampler))return fail(d,Error::samplerConflict);
                samplerSet[image]=true;s->samplers[image]=converted.native.sampler;b.policies.push_back(policy);
            }
        }
        for(auto image:s->galleryImages)if(image!=unresolved){
            const auto* texture=bank->texture(image);
            if(!texture||texture->texture.width!=640||texture->texture.height!=448)return fail(d,Error::resource,0x2A6E90);
            auto& sampler=s->samplers[image];
            sampler.minification=sampler.magnification=MaterialTextureFilter50EB::linear;
            // Same explicit native raw-texture adaptation as the Hall raw path.
            sampler.s=sampler.t=MaterialTextureWrap50EB::clampToEdge;samplerSet[image]=true;
        }
        for(bool initialized:samplerSet)if(!initialized)return fail(d,Error::resource);
        for(std::uint16_t id=1;id<game::originalUiUnlockWidgetStorageCount50PD;++id){
            d.widgetId=id;
            if(const auto* text=s->widgets.textDescriptor(id)){
                const auto m=find(s->bindings,"font/"+text->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];if(b.images.size()!=1||!same(b.material.passes[0].textureName,text->fontName))return fail(d,Error::material);
                const auto* font=s->images->font(b.images[0]);const auto* atlas=bank->texture(b.images[0]);if(!font||!atlas)return fail(d,Error::resource);
                // Authored depths: 1 for 1..4, -5.0 for the Huttese names.
                if(text->depth!=1&&text->depth!=-5)return fail(d,Error::geometry);
                if(text->textSource!=assets::OriginalUiTextSource50KG::dynamic){
                    auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
                    if(!geometry->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry);
                    s->text[id]=std::move(geometry);
                }
                s->widgetMaterial[id]=m;
            }else if(const auto* icon=s->widgets.iconDescriptor(id)){
                const auto m=find(s->bindings,"ui/textures/"+icon->materialName);if(m==unresolved)return fail(d,Error::material);
                const auto& b=s->bindings[m];if(b.images.empty()||icon->depth!=1)return fail(d,Error::resource);
                if(!s->widgets.prepareIcon(id,s->images,b.images[0],in.video.packetZMaximum,d.widgets))return fail(d,Error::widgets);
                s->widgetMaterial[id]=m;
            }
        }
        for(auto& sprite:s->sprites){
            if(!sprite.built)continue;
            const auto& b=s->bindings[sprite.material];const auto* atlas=bank->texture(b.images[0]);
            if(!atlas||!atlas->texture.width||!atlas->texture.height)return fail(d,Error::resource);
            // 2DF180 reciprocal texel scale, 20A150..20A1AC endpoints 319/223.
            const float us=1.f/float(atlas->texture.width),vs=1.f/float(atlas->texture.height);
            const float u=319.f*us,v=223.f*vs;const auto* name=b.registeredName.c_str();
            sprite.quadrants[0]=quad(name,0,0,320,224,0,0,u,v,15,0x20C9DC);
            sprite.quadrants[1]=quad(name,320,0,320,224,u,0,0,v,15,0x20C9DC);
            sprite.quadrants[2]=quad(name,0,224,320,224,0,v,u,0,15,0x20C9DC);
            sprite.quadrants[3]=quad(name,320,224,320,224,u,v,0,0,15,0x20C9DC);
        }
        constexpr float promptX[]={484,397,285,160};
        constexpr std::uint32_t promptPC[]={0x1d7a2c,0x1d7a44,0x1d7a5c,0x1d7a74};
        for(std::size_t i=0;i<4;++i){
            auto& p=s->prompts[i];const auto& b=s->bindings[p.material];const auto* atlas=bank->texture(b.images[0]);
            if(!atlas)return fail(d,Error::resource);p.mask=static_cast<std::uint8_t>(1u<<i);
            p.quad=quad(b.registeredName.c_str(),promptX[i],378,float(atlas->texture.width),float(atlas->texture.height),0,0,1,1,1,promptPC[i]);
        }
        const auto denmark=find(s->bindings,"font/denmrk16top");
        if(denmark==unresolved||s->bindings[denmark].images.size()!=1)return fail(d,Error::resource);
        if(s->exitText.prepare({10,1},in.unlock,*in.english,s->images,s->bindings[denmark].images[0],d.exitText)!=
            game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1d6aec);
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,Error::allocation);}
}
bool OriginalUiUnlockDrawResources50PD::loaded() const noexcept {return bool(storage_);}
const game::OriginalUiUnlockPage50PD* OriginalUiUnlockDrawResources50PD::page() const noexcept {return storage_?&storage_->page:nullptr;}
game::OriginalUiUnlockWidgets50PD* OriginalUiUnlockDrawResources50PD::widgets() noexcept {return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiUnlockWidgets50PD* OriginalUiUnlockDrawResources50PD::widgets() const noexcept {return storage_?&storage_->widgets:nullptr;}
game::OriginalUiExitText50KG* OriginalUiUnlockDrawResources50PD::exitText() noexcept {return storage_?&storage_->exitText:nullptr;}
const assets::OriginalUiScreenAssets50KG* OriginalUiUnlockDrawResources50PD::images() const noexcept {return storage_?storage_->images.get():nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiUnlockDrawResources50PD::sourceImageBank() const noexcept {return storage_?&storage_->imageBank:nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiUnlockDrawResources50PD::textures() const noexcept {return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiUnlockDrawResources50PD::samplers() const noexcept {return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiUnlockMaterialBinding50PD>* OriginalUiUnlockDrawResources50PD::materials() const noexcept {return storage_?&storage_->bindings:nullptr;}
const OriginalUiUnlockVideoOwner50PD* OriginalUiUnlockDrawResources50PD::video() const noexcept {return storage_?&storage_->owners.video:nullptr;}
std::uint32_t OriginalUiUnlockDrawResources50PD::widgetMaterial(std::uint16_t id) const noexcept {
    return storage_&&id<game::originalUiUnlockWidgetStorageCount50PD?storage_->widgetMaterial[id]:unresolved;
}
std::uint32_t OriginalUiUnlockDrawResources50PD::materialIndex(const char* name) const noexcept {
    if(!storage_||!name)return unresolved;
    for(std::size_t i=0;i<storage_->bindings.size();++i){const auto& a=storage_->bindings[i].registeredName;std::size_t j=0;
        for(;j<a.size()&&name[j]&&fold(a[j])==fold(name[j]);++j){}if(j==a.size()&&!name[j])return static_cast<std::uint32_t>(i);}
    return unresolved;
}
const game::OriginalUiTextGeometry50KG* OriginalUiUnlockDrawResources50PD::textGeometry(std::uint16_t id) const noexcept {
    return storage_&&id<game::originalUiUnlockWidgetStorageCount50PD?storage_->text[id].get():nullptr;
}
bool OriginalUiUnlockDrawResources50PD::textPacketDepth(std::uint16_t id,float& out) const noexcept {
    if(!storage_)return false;const auto* text=storage_->widgets.textDescriptor(id);if(!text)return false;
    const float z=text->depth<0?-text->depth:float(storage_->owners.video.packetZMaximum)-text->depth;
    if(!std::isfinite(z)||z<0||z>float(storage_->owners.video.packetZMaximum)||std::floor(z)!=z)return false;
    out=z;return true;
}
bool OriginalUiUnlockDrawResources50PD::setDynamicText(std::uint16_t id,const std::string& value,Diagnostic& d) noexcept {
    d={};d.widgetId=id;if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
    const auto* text=s.widgets.textDescriptor(id);if(!text||!s.widgets.dynamicText(id))return fail(d,Error::input,0x2A9FF0);
    std::vector<std::string> pieces;if(!s.widgets.preflightDynamicText(id,value,pieces,d.widgets))return fail(d,Error::widgets,0x2A9DE0);
    const auto m=s.widgetMaterial[id];if(m>=s.bindings.size()||s.bindings[m].images.size()!=1)return fail(d,Error::material);
    const auto image=s.bindings[m].images[0];const auto* font=s.images->font(image);const auto* atlas=textures()->texture(image);
    if(!font||!atlas)return fail(d,Error::resource);
    try {
        // Geometry of the lines the widget will hold after 2A9DE0.
        const auto* lines=s.widgets.dynamicLines(id);if(!lines)return fail(d,Error::widgets);
        std::vector<std::string> next(*lines);
        for(std::size_t i=0;i<next.size();++i){const auto* state=s.widgets.textWidget(id)->line(i);if(!state)return fail(d,Error::widgets);
            if(state->locked)continue;next[i]=i<pieces.size()?pieces[i]:std::string{};}
        auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
        if(!geometry->prepareDynamic(*text,next,*font,atlas->texture,d.text))return fail(d,Error::geometry,0x2A9DE0);
        if(!s.widgets.setDynamicText(id,value,d.widgets))return fail(d,Error::widgets,0x2A9DE0);
        s.text[id]=std::move(geometry);return true;
    }catch(...){return fail(d,Error::allocation);}
}
const OriginalUiUnlockSprites50PD* OriginalUiUnlockDrawResources50PD::sprites(std::int32_t pod) const noexcept {
    return storage_&&pod>=0&&pod<27&&storage_->sprites[std::size_t(pod)].built?&storage_->sprites[std::size_t(pod)]:nullptr;
}
const std::array<OriginalUiUnlockPrompt50PD,4>* OriginalUiUnlockDrawResources50PD::prompts() const noexcept {return storage_?&storage_->prompts:nullptr;}
std::uint32_t OriginalUiUnlockDrawResources50PD::galleryImage(game::OriginalUiUnlockGallery50PD g) const noexcept {
    const auto i=static_cast<std::size_t>(g);return storage_&&i>=1&&i<=3?storage_->galleryImages[i]:unresolved;
}
std::uint32_t OriginalUiUnlockDrawResources50PD::currentBackgroundImage() const noexcept {return storage_?storage_->currentBackgroundImage:unresolved;}
game::OriginalUiUnlockModels50PD* OriginalUiUnlockDrawResources50PD::models() noexcept {return storage_?&storage_->models:nullptr;}
const game::OriginalUiUnlockCamera50PD* OriginalUiUnlockDrawResources50PD::camera() const noexcept {return storage_?&storage_->camera:nullptr;}
std::uint32_t OriginalUiUnlockDrawResources50PD::modelBinding(const assets::OriginalUiRiderModelMaterials50KG* materials,
    std::uint32_t material,std::uint32_t pass) const noexcept {
    if(!storage_)return unresolved;
    for(const auto& k:storage_->modelKeys)if(k.materials==materials&&k.material==material&&k.pass==pass)return k.binding;
    return unresolved;
}
std::uint32_t OriginalUiUnlockDrawResources50PD::modelImage(const assets::OriginalUiRiderModelMaterials50KG* materials,
    std::uint32_t local) const noexcept {
    if(!storage_)return unresolved;
    for(const auto& o:storage_->modelOwners)if(o.materials==materials){const auto* map=storage_->imageBank.mapping(o.owner,local);return map?map->combinedIndex:unresolved;}
    return unresolved;
}
std::uint32_t OriginalUiUnlockDrawResources50PD::binderBinding() const noexcept {return storage_?storage_->binderBinding:unresolved;}
std::uint32_t OriginalUiUnlockDrawResources50PD::binderImage(std::uint32_t local) const noexcept {
    return storage_&&local<storage_->binderImages.size()?storage_->binderImages[local]:unresolved;
}
bool OriginalUiUnlockDrawResources50PD::setPageBackground8C(game::OriginalUiUnlockGallery50PD g) noexcept {
    const auto image=galleryImage(g);if(image==unresolved)return false;storage_->currentBackgroundImage=image;return true;
}
}
