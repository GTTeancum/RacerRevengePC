#include "original_ui_rider_draw_contribution50kg.h"
#include "../../../src/assets/original_ui_material_equivalence50kg.h"
#include "../../../src/assets/original_ui_material_binding50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>
namespace rr::platform {
namespace {
using D=OriginalUiRiderContributionDiagnostic50KG;using E=OriginalUiRiderContributionError50KG;
constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(D& d,E e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;return false;}
char fold(char c) noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
bool samplerSame(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b) noexcept {
    return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
}
struct Binding {
    std::string registeredName;
    const assets::OriginalUiMaterialPass50KG* pass=nullptr;
    const assets::OriginalUiImageProvenance50KG* provenance=nullptr;
    std::uint32_t image=unresolved,leaf=unresolved,registration=unresolved;
    OriginalUiMaterialSpan50KG prototype{};
    // Real PSM registry existence, independent of cold sampler/image policy.
    // False bindings can be uploaded but cannot redirect or receive packets.
    bool sourceRegistered=false;
};
struct Packet {std::uint32_t first=0,count=0,binding=0,ordinal=0;};
struct Registration {
    std::uint32_t rank=0,pass=0;
    const assets::OriginalUiMaterialPass50KG* source=nullptr;
    assets::OriginalUiMaterialBindingState50KG state;
};
bool material(const assets::OriginalUiMaterialPass50KG& pass,const assets::OriginalUiScreenAssets50KG& images,
    std::uint32_t index,OriginalUiMaterialSpan50KG& output,D& d) noexcept {
    const auto* textures=images.textures();if(!textures)return fail(d,E::bank);
    if(pass.textureCount==0){assets::OriginalUiNativeUntexturedMaterial50KG policy{};
        return assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,policy,d.materialDetail)&&
            convertOriginalUiUntexturedMaterialSpan50KG(policy,{0,1,1},output);}
    const auto* texture=images.decodedTexture(index);if(!texture)return fail(d,E::material);
    const OriginalUiMaterialSpanBinding50KG range{0,1,1,index,textures->textureCount()};
    if(same(pass.microprogram,"ScreenObject")){assets::OriginalUiNativeMaterial50KG policy{};
        return assets::prepareOriginalUiNativeMaterial50KG(pass,*texture,policy,d.materialDetail)&&
            convertOriginalUiMaterialSpan50KG(policy,range,output);}
    if(same(pass.textureName,"19_shdw")||same(pass.textureName,"20_shdw")){assets::OriginalUiRider19ShadowMaterial50KM policy{};
        return assets::prepareOriginalUiRider19ShadowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19ShadowMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.microprogram,"SpecularMapCompressed")){assets::OriginalUiRider19SpecularMaterial50KM policy{};
        return assets::prepareOriginalUiRider19SpecularMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19SpecularMaterialSpan50KM(policy,images,range,output);}
    if(texture->kind==2){assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiRider19IpumMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(texture->mips.size()>1){
        if(same(pass.textureName,"pod_wndw")){assets::OriginalUiRiderWindowMaterial50KM policy{};
            return assets::prepareOriginalUiRiderWindowMaterial50KM(pass,images,index,policy,d.materialDetail)&&
                convertOriginalUiRiderWindowMaterialSpan50KM(policy,images,range,output);}
        assets::OriginalUiRider19BodyMaterial50KM policy{};
        return assets::prepareOriginalUiRider19BodyMaterial50KM(pass,images,index,policy,d.materialDetail)&&
            convertOriginalUiRider19BodyMaterialSpan50KM(policy,images,range,output);
    }
    assets::OriginalUiLitMaterial50KM policy{};
    return assets::prepareOriginalUiLitMaterial50KM(pass,*texture,policy,d.materialDetail)&&
        convertOriginalUiLitMaterialSpan50KM(policy,range,output);
}
}
struct OriginalUiRiderDrawContribution50KG::Storage {
    std::shared_ptr<OriginalUiRiderDrawResources50KG> resources;
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> statAssets;
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binderAssets;
    game::OriginalUiRiderBackend50KG* backend=nullptr;
    game::OriginalUiRiderDynamicOwner50KG* dynamic=nullptr;
    game::OriginalUiLightManagerLifetime50KG* light=nullptr;
    std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition;
    std::array<const game::OriginalUiApertureOwner50KG*,2> pageApertures{};
    game::OriginalUiApertureSnapshot50KG apertureAtBegin{};
    std::uint8_t sourcePlayer=0;
    bool exitExpected=false,exitProjected=false;
    std::uint32_t staticOwner=0,statOwner=0;
    game::OriginalUiRiderStatOwner50KG stats;
    game::OriginalUiRiderStatFrame50KG statFrame{};
    OriginalUiRiderStatDraw50KG statDraw;
    game::OriginalUiLitGeometryWorkspace50KM workspace{};
    game::OriginalUiTextLayout50KG textScratch{};
    std::vector<game::OriginalUiTextureAnimation50KG> animations;
    std::vector<std::uint32_t> frames;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::shared_ptr<const MaterialTextureGl50EB> gpu;
    std::uint64_t bankGeneration=0,publication=0,frameGeneration=0;
    std::uint64_t registryGeneration=0;
    std::uint64_t acknowledgedGeneration=0;
    std::vector<Binding> bindings;
    std::vector<Registration> history,pendingHistory;
    std::vector<std::uint32_t> representatives;
    std::vector<MaterialTextureSampler50EB> samplers;
    std::vector<assets::Vertex48B> vertices,orderedVertices,scratchVertices;
    std::vector<float> w,fog,orderedW,orderedFog,scratchW,scratchFog;
    std::vector<Packet> packets;
    std::vector<std::uint32_t> order;
    std::vector<OriginalUiSpan50KG> spans;
    std::vector<OriginalUiNativeSpan50KG> states;
    game::OriginalUiRiderRenderContext50KG render{};
    game::OriginalUiRiderLighting50KG lighting{};
    OriginalUiRiderDrawClock50KG clock{};
    OriginalUiNativeDraw50KG draw{};
    bool active=false,ordinary=false,page=false,pageShown=false,prompts=false,top=false,finished=false,failed=false;
    bool submissionPending=false,historyPoisoned=false;
    std::uint8_t apertures=0;
    std::uint32_t find(const assets::OriginalUiMaterialPass50KG* pass,std::uint32_t image)const noexcept {
        for(std::size_t i=0;i<bindings.size();++i)if(bindings[i].sourceRegistered&&bindings[i].leaf!=unresolved&&bindings[i].pass==pass&&bindings[i].image==image)return static_cast<std::uint32_t>(i);
        return unresolved;
    }
    bool capacity(std::size_t quads)const noexcept {
        return quads<=originalUiMaximumSourceQuads50KM&&vertices.size()/4<=originalUiMaximumSourceQuads50KM-quads&&packets.size()<originalUiMaximumSpans50KG;
    }
    bool add(const std::string& name,const assets::OriginalUiMaterialPass50KG& pass,
        const assets::OriginalUiScreenAssets50KG& source,std::uint32_t image,
        const assets::OriginalUiImageBankMapping50KG* map,std::vector<Binding>& out,D& d,bool registered=true)const {
        Binding b;b.registeredName=name;b.pass=&pass;b.sourceRegistered=registered;
        if(pass.textureCount){
            const auto* from=source.sourceProvenance(image);
            const auto* to=map?bank->images()->sourceProvenance(map->combinedIndex):nullptr;
            const auto* sourceImage=source.textures()?source.textures()->texture(image):nullptr;
            const auto* combinedImage=map?bank->images()->textures()->texture(map->combinedIndex):nullptr;
            if(!from||!to||!sourceImage||!combinedImage||from->memberIdentity!=to->memberIdentity||
                from->celIdentity!=to->celIdentity||from->pstOffset!=to->pstOffset||from->pstBytes!=to->pstBytes||
                map->sourceIdentity!=sourceImage->identity||map->combinedIdentity!=combinedImage->identity)return fail(d,E::bank);
            b.image=map->combinedIndex;b.provenance=to;
        }else if(map||image!=unresolved)return fail(d,E::material);
        if(!material(pass,*bank->images(),b.image,b.prototype,d))return fail(d,E::material);
        out.push_back(std::move(b));return true;
    }
    // Defined below after source-backed pass key preparation. This always
    // resolves actual registry/pass barriers, never an arbitrary caller rank.
    bool schedule(std::vector<Binding>&,D&);
};
OriginalUiRiderDrawContribution50KG::OriginalUiRiderDrawContribution50KG() noexcept=default;
OriginalUiRiderDrawContribution50KG::~OriginalUiRiderDrawContribution50KG(){drain();}
bool OriginalUiRiderDrawContribution50KG::bind(std::shared_ptr<OriginalUiRiderDrawResources50KG> resources,
    std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> stat,
    std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> binder,
    game::OriginalUiRiderBackend50KG& backend,game::OriginalUiRiderDynamicOwner50KG& dynamic,
    game::OriginalUiLightManagerLifetime50KG& light,std::uint32_t staticOwner,std::uint32_t statOwner,
    std::shared_ptr<const game::OriginalUiTransitionOwner50KG> transition,
    const std::array<const game::OriginalUiApertureOwner50KG*,2>& apertures,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    if(!resources||!resources->loaded()||!stat||!stat->loaded()||!binder||!binder->loaded()||
        !backend.bank()||dynamic.poisoned()||!transition||!apertures[0]||
        !apertures[0]->snapshot().bound||(apertures[1]&&!apertures[1]->snapshot().bound))return fail(d,E::input);
    try {
        const assets::OriginalUiMemberIdentity50KG* indexIdentity=nullptr;
        const auto* archiveIdentity=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider);
        if(!archiveIdentity)return fail(d,E::resource);
        for(std::size_t i=0;i<archiveIdentity->memberCount;++i){const auto* member=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::rider,i);
            if(member&&std::strcmp(member->name,"UI\\UIRider\\ui.dat")==0)indexIdentity=member;
        }
        const auto* selection=resources->widgets()->selectionData();const auto* page=selection?selection->page():nullptr;
        for(const auto* aperture:apertures){if(!aperture)continue;const auto* config=aperture->config();
            if(!indexIdentity||!page||!config||config->indexIdentity!=indexIdentity||config->pageIdentity!=page->identity)return fail(d,E::resource);
        }
        auto s=std::make_unique<Storage>();s->resources=std::move(resources);s->statAssets=std::move(stat);s->binderAssets=std::move(binder);
        s->backend=&backend;s->dynamic=&dynamic;s->light=&light;s->staticOwner=staticOwner;s->statOwner=statOwner;
        s->transition=std::move(transition);s->pageApertures=apertures;
        if(!s->stats.bind(s->statAssets,d.stat))return fail(d,E::stat);
        const auto& materials=*s->resources->materials();s->animations.resize(materials.size());s->frames.resize(materials.size());
        for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&
           !game::initializeOriginalUiTextureAnimation50KG(s->animations[i],materials[i].animation,
                static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,E::clock);
        for(auto* list:{&s->vertices,&s->orderedVertices,&s->scratchVertices})list->reserve(originalUiMaximumSourceQuads50KM*4u);
        for(auto* list:{&s->w,&s->fog,&s->orderedW,&s->orderedFog,&s->scratchW,&s->scratchFog})list->reserve(originalUiMaximumSourceQuads50KM*4u);
        s->packets.reserve(originalUiMaximumSpans50KG);s->order.reserve(originalUiMaximumSpans50KG);
        s->spans.reserve(originalUiMaximumSpans50KG);s->states.reserve(originalUiMaximumSpans50KG);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderDrawContribution50KG::prepareBank(D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_)return fail(d,E::phase);
    if(s.historyPoisoned||s.submissionPending)return fail(d,E::schedule,0x249814);
    const auto bank=s.backend->bank();if(!bank||!bank->loaded()||!bank->images())return fail(d,E::bank);
    if(s.bankGeneration==s.backend->generation()&&s.registryGeneration==s.backend->registryGeneration()&&s.bank==bank&&!s.bindings.empty())return true;
    try {
        struct Rollback {
            Storage& s;std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
            std::vector<Registration> history;std::vector<std::uint32_t> representatives;bool committed=false;
            ~Rollback(){if(!committed){s.bank=std::move(bank);s.history=std::move(history);s.representatives=std::move(representatives);}}
        } rollback{s,s.bank,s.history,s.representatives};
        s.bank=bank;
        std::vector<Binding> bindings;
        const auto& materials=*s.resources->materials();
        for(std::size_t i=0;i<materials.size();++i){d.material=static_cast<std::uint32_t>(i);const auto& m=materials[i];
            if(m.material.passes.size()!=1)return fail(d,E::material);const auto& pass=m.material.passes[0];
            if(m.images.empty()){
                if(!s.add(m.registeredName,pass,*s.resources->images(),unresolved,nullptr,bindings,d))return false;
            }else for(auto image:m.images)
                if(!s.add(m.registeredName,pass,*s.resources->images(),image,s.backend->staticImage(s.staticOwner,image),bindings,d))return false;
        }
        const auto statImages=s.statAssets->images();
        const auto addStat=[&](const assets::OriginalUiRiderStatResource50KG* resource){
            if(!resource||resource->material.passes.size()!=1)return false;
            for(auto image:resource->images)if(!s.add(resource->registeredName,resource->material.passes[0],*statImages,
                image,s.backend->staticImage(s.statOwner,image),bindings,d))return false;
            return true;
        };
        for(unsigned i=0;i<=static_cast<unsigned>(assets::OriginalUiRiderStatAsset50KG::labelFont);++i)
            if(!addStat(s.statAssets->resource(static_cast<assets::OriginalUiRiderStatAsset50KG>(i))))return fail(d,E::material);
        for(std::uint8_t rider=1;rider<=26;++rider)if(const auto* portrait=s.statAssets->portrait(rider))
            if(!addStat(portrait))return fail(d,E::material);
        // Bank preflight retained current+new model images through callbacks.
        // Include those exact resource owners even if a model was just detached;
        // they supply sampler policy for every real image in this bank.
        for(std::uint16_t id=1;id<120;++id){
            const auto* m=s.backend->materials(id);if(!m||!s.backend->modelImage(id,0))continue;
            const assets::Scene48D* scene=nullptr;
            if(const auto cold=s.dynamic->podAssets(id))scene=cold->scene();
            else if(const auto cold=s.dynamic->characterAssets(id))scene=cold->scene();
            if(!scene||scene->materials.size()!=m->materialCount())return fail(d,E::model);
            for(std::uint32_t slot=0;slot<m->materialCount();++slot)for(std::uint32_t p=0;p<m->passCount(slot);++p){
                const auto* pass=m->pass(slot,p);if(!pass)return fail(d,E::material);
                const auto* map=pass->imageIndex==unresolved?nullptr:s.backend->modelImage(id,pass->imageIndex);
                if(!s.add("ui/textures/"+scene->materials[slot],pass->source,*m->images(),pass->imageIndex,map,bindings,d,s.backend->materialsRegistered(id)))return false;
            }
        }
        const auto* binderImages=s.binderAssets->images();const auto* binderPass=s.binderAssets->pass();
        if(!binderImages||!binderImages->textures()||!binderPass)return fail(d,E::resource);
        for(std::uint32_t i=0;i<binderImages->textures()->textureCount();++i)
            if(!s.add("ui/textures/binder",*binderPass,*binderImages,i,s.backend->binderImage(i),bindings,d,s.backend->binderMaterialRegistered()))return false;
        // A source material is one registry row, even when multiple native
        //cold owners/ATM frames refer to it. If any real owner populated the
        //row, all exact aliases share that row; other preflight-only PSMs stay
        //outside source grouping while retaining their genuine upload policy.
        for(auto& binding:bindings)if(!binding.sourceRegistered){
            for(const auto& owner:bindings)if(owner.sourceRegistered&&owner.pass->pass==binding.pass->pass&&
                same(owner.registeredName,binding.registeredName.c_str())){
                if(owner.pass->identity!=binding.pass->identity||owner.pass->inheritedTemplate!=binding.pass->inheritedTemplate||
                   owner.pass->provenance.origin!=binding.pass->provenance.origin||owner.pass->provenance.archive!=binding.pass->provenance.archive||
                   owner.pass->provenance.fallbackTexture!=binding.pass->provenance.fallbackTexture)return fail(d,E::schedule,0x14D370);
                binding.sourceRegistered=true;break;
            }
        }
        if(s.registryGeneration==s.backend->registryGeneration()&&!s.bindings.empty()){
            // GPU/CPU bank changes alone preserve the existing source group
            // graph, including redirects chosen before the last actual draw.
            for(auto& binding:bindings){if(!binding.sourceRegistered)continue;const Binding* previous=nullptr;
                for(const auto& old:s.bindings)if(same(old.registeredName,binding.registeredName.c_str())&&
                    old.pass->pass==binding.pass->pass){previous=&old;break;}
                if(!previous||!previous->sourceRegistered||previous->pass->identity!=binding.pass->identity||
                   previous->pass->provenance.fallbackTexture!=binding.pass->provenance.fallbackTexture)return fail(d,E::schedule,0x14D440);
                binding.leaf=previous->leaf;binding.registration=previous->registration;
            }
            std::vector<std::uint32_t> remapped(s.representatives.size(),unresolved);
            for(std::size_t leaf=0;leaf<s.representatives.size();++leaf){
                if(s.representatives[leaf]>=s.bindings.size())return fail(d,E::schedule);
                const auto& old=s.bindings[s.representatives[leaf]];
                for(std::size_t i=0;i<bindings.size();++i)if(bindings[i].registration==old.registration){remapped[leaf]=static_cast<std::uint32_t>(i);break;}
                if(remapped[leaf]==unresolved)return fail(d,E::schedule,0x14D440);
            }
            s.representatives=std::move(remapped);
        }else if(!s.schedule(bindings,d))return false;
        std::vector<MaterialTextureSampler50EB> samplers(bank->images()->textures()->textureCount());
        std::vector<bool> assigned(samplers.size(),false);
        for(const auto& binding:bindings)if(binding.image!=unresolved){
            if(binding.image>=samplers.size())return fail(d,E::bank);
            const auto& sampler=binding.prototype.native.sampler;
            if(assigned[binding.image]&&!samplerSame(samplers[binding.image],sampler))return fail(d,E::material);
            samplers[binding.image]=sampler;assigned[binding.image]=true;
        }
        for(bool known:assigned)if(!known)return fail(d,E::material);
        s.bindings=std::move(bindings);s.samplers=std::move(samplers);s.bankGeneration=s.backend->generation();
        s.registryGeneration=s.backend->registryGeneration();s.gpu.reset();s.publication=0;
        rollback.committed=true;return true;
    }catch(...){return fail(d,E::allocation);}
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiRiderDrawContribution50KG::bank()const noexcept {
    return storage_&&storage_->bankGeneration==storage_->backend->generation()&&
        storage_->registryGeneration==storage_->backend->registryGeneration()?storage_->bank:nullptr;
}
const std::vector<MaterialTextureSampler50EB>* OriginalUiRiderDrawContribution50KG::samplers()const noexcept {
    return bank()?&storage_->samplers:nullptr;
}
bool OriginalUiRiderDrawContribution50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,D& d) noexcept {
    d={};if(!storage_||storage_->active||frame_.native_||!bank()||!gpu||!gpu->initialized()||!gpu->publicationToken())return fail(d,E::publication);
    const auto* textures=storage_->bank->images()->textures();if(gpu->textureCount()!=textures->textureCount())return fail(d,E::publication);
    for(std::uint32_t i=0;i<gpu->textureCount();++i){const auto* a=gpu->asset(i);const auto* b=textures->texture(i);
        if(!a||!b||a->identity!=b->identity)return fail(d,E::publication);}
    storage_->gpu=std::move(gpu);storage_->publication=storage_->gpu->publicationToken();return true;
}
bool OriginalUiRiderDrawContribution50KG::begin(const OriginalUiRiderDrawClock50KG& clock,
    const game::OriginalUiRiderRenderContext50KG& render,const game::OriginalUiRiderLighting50KG& lighting,D& d) noexcept {
    d={};if(!storage_||!clock.known)return fail(d,E::input);auto& s=*storage_;
    if(s.active||frame_.native_||s.historyPoisoned||s.submissionPending||!bank()||!s.gpu||!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,E::publication);
    const auto lifecycle=s.transition->snapshot();
    if(!lifecycle.bound||lifecycle.current.screen!=5||lifecycle.current.page<1||lifecycle.current.page>2)return fail(d,E::phase);
    const auto player=static_cast<std::uint8_t>(lifecycle.current.page-1);
    // Unsupported page2 has no fabricated hook owner. Page1 is mandatory at
    // bind; every optional supplied owner was provenance-checked there.
    if(!s.pageApertures[player])return fail(d,E::phase);
    const auto aperture=s.pageApertures[player]->snapshot();
    if(!aperture.bound)return fail(d,E::phase);
    game::OriginalUiRiderVisibilityDiagnostic50KG visibility;
    game::OriginalUiLightManagerLifetimeDiagnostic50KG lights;
    if(!s.backend->beginFrame())return fail(d,E::phase);
    struct Lease {game::OriginalUiRiderBackend50KG* backend;bool retained=false;~Lease(){if(!retained)backend->endFrame();}} lease{s.backend};
    if(!s.light->publishRiderRegistry(lighting,lights)||!s.backend->visibility()||
       !s.backend->visibility()->prepareFrame(render.cameraAfterFog,{},visibility))return fail(d,E::model);
    lease.retained=true;
    s.clock=clock;s.render=render;s.lighting=lighting;s.active=true;
    s.sourcePlayer=player;s.apertureAtBegin=aperture;s.exitProjected=false;
    s.exitExpected=lifecycle.lifecycle==game::OriginalUiTransitionLifecycle50KG::waitScreenExit&&
        aperture.state==game::OriginalUiApertureState50KG::closing&&aperture.top284>=224&&aperture.bottom28C<=224;
    s.ordinary=s.page=s.pageShown=s.prompts=s.top=s.finished=s.failed=false;s.apertures=0;
    s.vertices.clear();s.w.clear();s.fog.clear();s.packets.clear();s.order.clear();s.spans.clear();s.states.clear();
    for(std::size_t i=0;i<s.animations.size();++i)if((*s.resources->materials())[i].animated&&
       !game::selectOriginalUiTextureAnimationFrame50KG(s.animations[i],s.frames[i])){s.failed=true;return fail(d,E::clock);}
    return true;
}
bool OriginalUiRiderDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& quad,
    std::uint32_t materialIndex,float z,D& d) noexcept {
    auto& s=*storage_;const auto& materials=*s.resources->materials();
    if(materialIndex>=materials.size()||!std::isfinite(z)||z<0||z>16777215.f||std::floor(z)!=z||
       quad.sourcePacketColor[3]>128)return fail(d,E::geometry);
    const auto& material=materials[materialIndex];
    if(material.animated&&s.clock.advanceAnimations&&
       !game::advanceOriginalUiTextureAnimation50KG(s.animations[materialIndex],s.clock.milliseconds,s.frames[materialIndex]))return fail(d,E::clock);
    const auto frame=s.frames[materialIndex];std::uint32_t image=unresolved;
    if(!material.images.empty()){
        if(frame>=material.images.size())return fail(d,E::clock);
        const auto* map=s.backend->staticImage(s.staticOwner,material.images[frame]);if(!map)return fail(d,E::bank);image=map->combinedIndex;
    }
    const auto binding=s.find(&material.material.passes[0],image);if(binding==unresolved)return fail(d,E::material);
    if(!s.capacity(1))return fail(d,E::capacity);
    std::array<assets::Vertex48B,4> vertices{};constexpr unsigned order[]={0,1,3,2};
    for(unsigned i=0;i<4;++i){const auto& v=quad.vertices[order[i]];
        if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(d,E::geometry);
        auto& out=vertices[i];out.position[0]=v.x;out.position[1]=v.y;out.position[2]=z;out.uv[0]=v.u;out.uv[1]=v.v;
        for(unsigned c=0;c<4;++c)out.color[c]=quad.sourcePacketColor[c];
    }
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    s.vertices.insert(s.vertices.end(),vertices.begin(),vertices.end());s.w.insert(s.w.end(),4,1.f);s.fog.insert(s.fog.end(),4,0.f);
    s.packets.push_back({first,1,binding,static_cast<std::uint32_t>(s.packets.size())});return true;
}
bool OriginalUiRiderDrawContribution50KG::model(std::uint16_t id,D& d) noexcept {
    auto& s=*storage_;if(!s.backend->attached(id)||!s.backend->shown(id))return true;
    game::OriginalUiRiderSelectedPacket50KG selected{};
    if(!game::prepareOriginalUiRiderSelectedPacket50KG(id,*s.dynamic,*s.backend,s.render,s.lighting,*s.light,selected,d.selected))return fail(d,E::model);
    if(selected.emitsGeometry){
        const auto& packet=selected.packet;const auto* materials=s.backend->materials(id);
        if(!packet.scene||!materials||packet.scene->lods.size()!=1)return fail(d,E::geometry);
        const auto& surfaces=packet.scene->lods[0].surfaces;
        for(std::uint32_t surface=0;surface<surfaces.size();++surface){
            const auto materialIndex=surfaces[surface].material_index;
            if(materialIndex>=materials->materialCount()||surfaces[surface].indices.size()%3)return fail(d,E::material);
            const auto count=surfaces[surface].indices.size()/3;
            if(!count)continue;
            if(count>originalUiMaximumSourceQuads50KM)return fail(d,E::capacity);
            s.scratchVertices.resize(count*4);s.scratchW.resize(count*4);s.scratchFog.resize(count*4);
            for(std::uint32_t passIndex=0;passIndex<materials->passCount(materialIndex);++passIndex){
                const auto* material=materials->pass(materialIndex,passIndex);if(!material)return fail(d,E::material);
                const auto* map=material->imageIndex==unresolved?nullptr:s.backend->modelImage(id,material->imageIndex);
                if(material->imageIndex!=unresolved&&!map)return fail(d,E::bank);
                const auto binding=s.find(&material->source,map?map->combinedIndex:unresolved);
                if(binding==unresolved)return fail(d,E::material);
                game::OriginalUiLitSurfaceOutput50KM projected{s.scratchVertices.data(),s.scratchW.data(),s.scratchFog.data(),static_cast<std::uint32_t>(count),0,0};
                const auto status=same(material->source.microprogram,"SpecularMapCompressed")?
                    game::projectOriginalUiSpecularMapCompressedSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,projected,s.workspace):
                    game::projectOriginalUiLitSceneSurface50KM(*packet.scene,0,surface,packet.palette,packet.context,packet.fog.computeRadialFog1B2,projected,s.workspace);
                if(status!=game::OriginalUiLitGeometryStatus50KM::complete||projected.materialIndex!=materialIndex)return fail(d,E::geometry);
                if(!s.capacity(projected.slotCount))return fail(d,E::capacity);
                if(!projected.slotCount)continue;const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
                const auto n=std::size_t(projected.slotCount)*4;
                s.vertices.insert(s.vertices.end(),s.scratchVertices.begin(),s.scratchVertices.begin()+n);
                s.w.insert(s.w.end(),s.scratchW.begin(),s.scratchW.begin()+n);s.fog.insert(s.fog.end(),s.scratchFog.begin(),s.scratchFog.begin()+n);
                s.packets.push_back({first,projected.slotCount,binding,static_cast<std::uint32_t>(s.packets.size())});
            }
        }
    }
    // Entity-child traversal is independent of the graphics model's191 clip
    // bit. Real enabled binder helpers still run after a culled model parent.
    auto* pod=s.dynamic->podScene(id);if(!pod)return true;
    for(std::uint8_t pair=0;pair<2;++pair){auto* animation=s.backend->binder(id,pair);if(!animation)continue;
        game::OriginalUiRiderBinderQuad50KG source{};
        if(!pod->projectBinder(pair,s.render.cameraAfterFog.binderCameraColumn,source,d.model))return fail(d,E::model);
        game::OriginalUiRiderBinderFrame50KG image;
        if(!(s.clock.advanceAnimations?animation->advanceAfterGeometry(s.clock.milliseconds,image):animation->current(image)))return fail(d,E::clock);
        game::original_racer_lighting50KM::LightMatrices global{};
        if(game::projectOriginalUiGlobalLightRows50KM(true,s.lighting.directionalLook78,s.lighting.directional3C,
            s.lighting.ambient50,global)!=game::original_racer_lighting50KM::Status::complete)return fail(d,E::model);
        game::OriginalUiLitContext50KM context{};
        if(!game::prepareOriginalUiRiderLitContext50KG(*s.backend->riderArchive(),s.render,global,context))return fail(d,E::model);
        game::OriginalUiLitBinderPacket50KM projected{};
        if(game::projectOriginalUiRiderBinderPacket50KM(source,context,s.lighting.ambient50,projected)!=game::OriginalUiLitGeometryStatus50KM::complete)return fail(d,E::geometry);
        const auto* map=s.backend->binderImage(image.imageIndex);if(!map)return fail(d,E::bank);
        const auto binding=s.find(image.pass,map->combinedIndex);if(binding==unresolved)return fail(d,E::material);
        if(!s.capacity(2))return fail(d,E::capacity);const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
        s.vertices.insert(s.vertices.end(),projected.vertices.begin(),projected.vertices.end());
        s.w.insert(s.w.end(),projected.rasterW.begin(),projected.rasterW.end());s.fog.insert(s.fog.end(),projected.fogFactors.begin(),projected.fogFactors.end());
        s.packets.push_back({first,2,binding,static_cast<std::uint32_t>(s.packets.size())});
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::widgets(bool top,bool shown,D& d) noexcept {
    auto& s=*storage_;auto& widgets=*s.resources->widgets();const auto* order=widgets.order(top);
    if(!order)return fail(d,E::widget);const auto& materials=*s.resources->materials();
    for(const auto id:*order){d.widget=id;const auto state=widgets.state(id);if(!shown||!state.shown)continue;
        if(state.kind==game::OriginalUiRiderWidgetKind50KG::pod||state.kind==game::OriginalUiRiderWidgetKind50KG::character){
            if(!model(id,d))return false;continue;
        }
        const auto materialIndex=s.resources->widgetMaterial(id);
        if(const auto* descriptor=widgets.textDescriptor(id)){
            const auto* font=s.resources->textFont(id);const auto* atlas=s.resources->textAtlas(id);const auto* lines=widgets.textLines(id);
            if(!font||!atlas||!lines||materialIndex>=materials.size()||materials[materialIndex].images.size()!=1)return fail(d,E::resource);
            const auto& material=materials[materialIndex];const auto* map=s.backend->staticImage(s.staticOwner,material.images[0]);
            if(!map)return fail(d,E::bank);const auto binding=s.find(&material.material.passes[0],map->combinedIndex);
            if(binding==unresolved)return fail(d,E::material);float z=0;if(!s.resources->textPacketDepth(id,z))return fail(d,E::geometry);
            for(std::size_t line=0;line<lines->size();++line){const auto& text=(*lines)[line];
                game::OriginalUiTextInput50KG input{font,atlas,reinterpret_cast<const std::uint8_t*>(text.data()),text.size(),0,0};
                if(!game::layoutOriginalUiText50KG(input,s.textScratch,d.text))return fail(d,E::geometry);
                std::int32_t width=0,height=0;
                if(!game::roundOriginalUiTextMetric50KG(s.textScratch.widthBeforeScaleAndRounding,1,width)||
                    !game::roundOriginalUiTextMetric50KG(s.textScratch.firstGlyphHeightBeforeScaleAndRounding,1,height))return fail(d,E::geometry);
                game::OriginalUiRiderTextProjection50KG live;
                if(!widgets.projectText(id,line,s.textScratch.fontLineHeight,width,height,shown,live,d.widgetDetail)){
                    if(d.widgetDetail.error==game::OriginalUiRiderWidgetError50KG::hidden)continue;return fail(d,E::widget);
                }
                if(live.descriptor!=descriptor||live.line.text!=&text||live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128)
                    return fail(d,E::geometry);
                input.localX=live.position.positionedX;input.localY=live.position.positionedY;
                if(!game::layoutOriginalUiText50KG(input,s.textScratch,d.text))return fail(d,E::geometry);
                const auto count=s.textScratch.count;if(!count)continue;if(!s.capacity(count))return fail(d,E::capacity);
                const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
                for(unsigned q=0;q<count;++q)for(const auto& glyph:s.textScratch.quads[q].vertices){
                    assets::Vertex48B vertex{};vertex.position[0]=glyph.x;vertex.position[1]=glyph.y;vertex.position[2]=z;
                    vertex.uv[0]=glyph.u;vertex.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)vertex.color[c]=live.line.packetColor[c];s.vertices.push_back(vertex);
                }
                s.w.insert(s.w.end(),std::size_t(count)*4,1.f);s.fog.insert(s.fog.end(),std::size_t(count)*4,0.f);
                s.packets.push_back({first,count,binding,static_cast<std::uint32_t>(s.packets.size())});
            }
        }else if(widgets.iconDescriptor(id)){
            game::OriginalUiRiderIconProjection50KG live;
            if(!widgets.projectIcon(id,shown,live,d.widgetDetail))return fail(d,E::widget);
            if(live.images!=s.resources->images().get()||materialIndex>=materials.size()||materials[materialIndex].images.empty()||
               live.imageIndex!=materials[materialIndex].images[0])return fail(d,E::resource);
            // Icon geometry keeps its constructor dimensions. append selects
            // the actual live ATM frame without replacing these dimensions.
            if(!append(live.layout.quad,materialIndex,live.layout.packetDepth,d))return false;
        }
    }
    return true;
}
bool OriginalUiRiderDrawContribution50KG::projectOrdinaryWidgets(bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||s.ordinary||s.page||s.failed)return fail(d,E::phase);
    if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectPage(const game::OriginalUiRiderStatInput50KG& input,bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||s.page||s.failed)return fail(d,E::phase);s.page=true;s.pageShown=shown;
    if(!shown)return true;
    struct Guard {bool& failed;bool complete=false;~Guard(){if(!complete)failed=true;}} guard{s.failed};
    if(!s.stats.applyAppearance(input,d.stat)||!s.stats.project(input,{s.clock.known,s.clock.advanceAnimations,s.clock.milliseconds},s.statFrame,d.stat)||
       !s.statDraw.project(s.statAssets,s.statFrame,s.bank,s.statOwner,d.statDraw)){s.failed=true;return fail(d,E::stat);}
    if(!s.resources->widgets()->setTextPosition(4,s.statFrame.lockedWidgetX,s.statFrame.lockedWidgetY,d.widgetDetail))return fail(d,E::widget);
    for(const auto& packet:s.statDraw.packets()){
        const auto binding=s.find(&packet.resource->material.passes[0],packet.imageIndex);
        if(binding==unresolved)return fail(d,E::material);if(!s.capacity(packet.quadCount))return fail(d,E::capacity);
        const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);const auto begin=s.statDraw.vertices().begin()+std::size_t(packet.firstQuad)*4;
        s.vertices.insert(s.vertices.end(),begin,begin+std::size_t(packet.quadCount)*4);
        s.w.insert(s.w.end(),std::size_t(packet.quadCount)*4,1.f);s.fog.insert(s.fog.end(),std::size_t(packet.quadCount)*4,0.f);
        s.packets.push_back({first,packet.quadCount,binding,static_cast<std::uint32_t>(s.packets.size())});
    }
    const auto* background=s.resources->background(static_cast<std::uint8_t>(input.selectedRider));
    if(!background)return fail(d,E::resource);
    for(const auto& quad:background->quadrants)if(!append(quad,background->material,16777200.f,d))return false;
    guard.complete=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertures||s.prompts||s.top||s.failed||s.exitProjected||!s.exitExpected)
        return fail(d,E::phase,0x1D75A4);
    struct Guard {bool& failed;bool complete=false;~Guard(){if(!complete)failed=true;}} guard{s.failed};
    if(hook.observed.state!=s.apertureAtBegin.state||hook.observed.top284!=s.apertureAtBegin.top284||
       hook.observed.bottom28C!=s.apertureAtBegin.bottom28C||!s.capacity(10))return fail(d,E::phase,0x1D75A4);
    auto* exit=s.resources->exitText(s.sourcePlayer);
    if(!exit||exit->snapshot().route.screen!=5||exit->snapshot().route.page!=s.sourcePlayer+1)return fail(d,E::resource);
    const auto materialIndex=s.resources->materialIndex("font/denmrk16top");
    const auto& materials=*s.resources->materials();if(materialIndex>=materials.size())return fail(d,E::material);
    const auto& material=materials[materialIndex];if(material.images.size()!=1||material.material.passes.size()!=1)return fail(d,E::material);
    const auto* map=s.backend->staticImage(s.staticOwner,material.images[0]);
    const auto binding=map?s.find(&material.material.passes[0],map->combinedIndex):unresolved;
    if(binding==unresolved)return fail(d,E::bank);
    game::OriginalUiExitTextProjection50KG live{};
    if(exit->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||!live.glyphs||
       live.images!=s.resources->images().get()||live.imageIndex!=material.images[0]||
       live.materialIdentity!=material.material.identity||live.assignedDepth!=1||live.packetColor[3]>128||live.glyphs->count>10)
        return fail(d,E::geometry,0x2E0170);
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    for(std::size_t i=0;i<live.glyphs->count;++i)for(const auto& glyph:live.glyphs->quads[i].vertices){
        assets::Vertex48B v{};v.position[0]=glyph.x;v.position[1]=glyph.y;v.position[2]=16777214.f;
        v.uv[0]=glyph.u;v.uv[1]=glyph.v;for(unsigned c=0;c<4;++c)v.color[c]=live.packetColor[c];s.vertices.push_back(v);
        s.w.push_back(1.f);s.fog.push_back(0.f);
    }
    if(live.glyphs->count)s.packets.push_back({first,live.glyphs->count,binding,static_cast<std::uint32_t>(s.packets.size())});
    s.exitProjected=true;guard.complete=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rectangle,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.prompts||s.top||s.failed||ordinal!=s.apertures||ordinal>1||
       rectangle.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rectangle.dirty3C||
       (s.exitExpected&&!s.exitProjected))return fail(d,E::phase);
    game::OriginalUiOverlayQuad50KG quad;quad.authoredDepth=1;quad.sourcePC=0x1D798C;quad.sourcePacketColor={{0,0,0,63}};
    constexpr unsigned order[]={0,2,1,3};for(unsigned i=0;i<4;++i){const auto& v=rectangle.vertices[order[i]];quad.vertices[i]={v.x,v.y,0,0};}
    if(!append(quad,s.resources->materialIndex("ui/textures/notexture"),16777214.f,d)){s.failed=true;return false;}
    ++s.apertures;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectPrompts(std::uint32_t mask,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.page||!s.pageShown||s.apertures!=2||s.prompts||s.top||s.failed||(mask&~15u))return fail(d,E::phase);
    for(const auto& prompt:*s.resources->prompts())if(mask&prompt.mask)
        if(!append(prompt.quad,prompt.material,16777214.f,d)){s.failed=true;return false;}
    s.prompts=true;return true;
}
bool OriginalUiRiderDrawContribution50KG::projectOnTopWidgets(bool shown,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||s.top||s.failed||(s.pageShown&&(!s.prompts||s.apertures!=2)))return fail(d,E::phase);
    if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}

bool OriginalUiRiderDrawContribution50KG::Storage::schedule(std::vector<Binding>& input,D& d) {
    // The cold resources pin the same authenticated APPSTART used by every
    // actual pass. Tokenization is source269B20: only space/CR/LF delimit.
    assets::OriginalUiMemberView50KG table{};
    const auto* inputs=resources->bankInput();
    if(!inputs||historyPoisoned||submissionPending)return fail(d,E::schedule,0x29BEA0);
    for(const auto& archive:inputs->archives)if(archive&&archive->loaded()&&
       archive->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart)) {
        for(std::size_t i=0;i<archive->memberCount();++i){const auto member=archive->member(i);
            if(member.identity&&same(std::string(member.identity->name),"AppStart/Mat_Stbl.txt"))table=member;
        }
    }
    if(!table.identity||!table.data||table.size!=70962)return fail(d,E::schedule);
    std::vector<std::uint32_t> ranks(input.size(),unresolved),sequence;sequence.reserve(input.size());
    std::size_t cursor=0;std::uint32_t ordinal=0;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(cursor<table.size){
        while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;
        const auto start=cursor;while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(std::size_t i=0;i<input.size();++i){const auto& name=input[i].registeredName;
            if(name.size()!=cursor-start)continue;std::size_t j=0;
            for(;j<name.size()&&fold(name[j])==fold(static_cast<char>(table.data[start+j]));++j){}
            if(j!=name.size())continue;
            if(ranks[i]!=unresolved){
                // Authenticated Mat_Stbl repeats pod_wndw. Original298C80
                // returns the first matching row;299CAC retains that token.
                // Admit only this exact later occurrence, never a new owner
                // or a replacement rank. Other duplicate names still reject.
                if(!same(name,"ui/textures/pod_wndw")||ranks[i]!=2490||
                   ordinal!=3221||start!=70650)return fail(d,E::schedule,0x298C80);
                continue;
            }
            ranks[i]=ordinal;
        }
        ++ordinal;
    }
    if(ordinal!=3235)return fail(d,E::schedule);
    for(std::size_t i=0;i<input.size();++i){
        if(ranks[i]==unresolved||!input[i].pass)return fail(d,E::schedule);
        if(input[i].sourceRegistered)sequence.push_back(static_cast<std::uint32_t>(i));
    }
    std::sort(sequence.begin(),sequence.end(),[&](std::uint32_t a,std::uint32_t b){
        if(ranks[a]!=ranks[b])return ranks[a]<ranks[b];
        if(input[a].pass->pass!=input[b].pass->pass)return input[a].pass->pass<input[b].pass->pass;
        return a<b;
    });
    struct Group {
        std::uint32_t binding=0,redirect=unresolved,limit=unresolved,rank=0,registration=unresolved;
    };
    std::vector<Group> groups;groups.reserve(sequence.size());
    auto nextHistory=history;
    std::uint32_t previousRank=unresolved,previousPass=0,previousGroup=unresolved;
    for(const auto index:sequence){auto& binding=input[index];const auto& pass=*binding.pass;
        // A repeated PSG slot or ATM frame is a reference to an existing
        // registry material/pass, never a second 29BED0 registration.
        if(previousRank==ranks[index]&&previousPass==pass.pass){
            const auto& original=*input[groups[previousGroup].binding].pass;
            if(pass.identity!=original.identity||pass.inheritedTemplate!=original.inheritedTemplate||
               pass.provenance.origin!=original.provenance.origin||pass.provenance.archive!=original.provenance.archive||
               pass.provenance.fallbackTexture!=original.provenance.fallbackTexture)return fail(d,E::schedule);
            binding.leaf=previousGroup;binding.registration=groups[previousGroup].registration;continue;
        }
        if((previousRank!=ranks[index]&&pass.pass!=1)||
           (previousRank==ranks[index]&&pass.pass!=previousPass+1))return fail(d,E::schedule,0x29BED0);
        Group next;next.binding=index;next.rank=ranks[index];
        for(std::size_t i=0;i<nextHistory.size();++i)if(nextHistory[i].rank==ranks[index]&&nextHistory[i].pass==pass.pass){
            const auto* prior=nextHistory[i].source;
            if(!prior||prior->identity!=pass.identity||prior->inheritedTemplate!=pass.inheritedTemplate||
               prior->provenance.archive!=pass.provenance.archive||prior->provenance.origin!=pass.provenance.origin||
               prior->provenance.fallbackTexture!=pass.provenance.fallbackTexture)return fail(d,E::schedule);
            next.registration=static_cast<std::uint32_t>(i);break;
        }
        if(next.registration==unresolved){
            assets::OriginalUiMaterialEquivalenceKey50KG key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,binding.provenance,key,detail))return fail(d,E::schedule,detail.sourcePC);
            Registration record;record.rank=ranks[index];record.pass=pass.pass;record.source=&pass;
            assets::OriginalUiMaterialBindingDiagnostic50KG bindingDetail;
            if(!record.state.initialize(key,bindingDetail))return fail(d,E::schedule,bindingDetail.sourcePC);
            next.registration=static_cast<std::uint32_t>(nextHistory.size());nextHistory.push_back(std::move(record));
        }
        const auto current=static_cast<std::uint32_t>(groups.size());
        //29C490 examines every earlier registration, including redirected ones.
        for(auto& prior:groups){
            const auto equal=nextHistory[next.registration].state.compare(nextHistory[prior.registration].state);
            if(equal==assets::OriginalUiMaterialEquivalence50KG::unsupported)return fail(d,E::schedule,0x249BA0);
            if(equal==assets::OriginalUiMaterialEquivalence50KG::equivalent&&current<prior.limit)prior.redirect=current;
        }
        groups.push_back(std::move(next));
        //29BED0 writes the previous pass's dependency barrier after adding
        // this pass. The earlier redirect check intentionally precedes it.
        if(previousRank==ranks[index])groups[previousGroup].limit=current;
        binding.leaf=current;binding.registration=groups[current].registration;previousRank=ranks[index];previousPass=pass.pass;previousGroup=current;
    }
    for(auto& binding:input){
        if(!binding.sourceRegistered){binding.leaf=unresolved;binding.registration=unresolved;continue;}
        std::size_t visits=0;
        while(binding.leaf<groups.size()&&groups[binding.leaf].redirect!=unresolved){
            const auto next=groups[binding.leaf].redirect;
            if(next<=binding.leaf||++visits>groups.size())return fail(d,E::schedule,0x29C150);
            binding.leaf=next;
        }
        if(binding.leaf>=groups.size())return fail(d,E::schedule);
    }
    // Bank membership is not proof that a retained registry object unloaded.
    // Until that lifecycle is supplied, never silently omit an earlier loaded
    // representative just because its model's images left the current bank.
    for(std::size_t i=0;i<history.size();++i){bool present=false;
        for(const auto& group:groups)if(group.registration==i){present=true;break;}
        if(!present)return fail(d,E::schedule,0x14D440);
    }
    std::vector<std::uint32_t> nextRepresentatives;nextRepresentatives.reserve(groups.size());
    for(const auto& group:groups)nextRepresentatives.push_back(group.binding);
    history=std::move(nextHistory);representatives=std::move(nextRepresentatives);return true;
}

bool OriginalUiRiderDrawContribution50KG::finish(D& d) noexcept {
    d={};if(!storage_)return fail(d,E::input);auto& s=*storage_;
    if(!s.active||!s.ordinary||!s.page||!s.top||s.failed||s.finished||
       (s.pageShown&&(!s.prompts||s.apertures!=2||s.exitExpected!=s.exitProjected)))return fail(d,E::unfinished);
    if(!bank()||!s.gpu||s.gpu->publicationToken()!=s.publication||
       s.vertices.size()!=s.w.size()||s.vertices.size()!=s.fog.size()||s.vertices.size()%4||
       s.vertices.empty()||s.vertices.size()/4>originalUiMaximumSourceQuads50KM||
       s.packets.size()>originalUiMaximumSpans50KG){s.failed=true;return fail(d,E::bank);}
    try {
        s.order.clear();
        for(std::size_t i=0;i<s.packets.size();++i){const auto& p=s.packets[i];
            if(p.binding>=s.bindings.size()||!s.bindings[p.binding].sourceRegistered||s.bindings[p.binding].leaf==unresolved||!p.count||
               p.first>=s.vertices.size()/4||p.count>s.vertices.size()/4-p.first){s.failed=true;return fail(d,E::schedule);}
            s.order.push_back(static_cast<std::uint32_t>(i));
        }
        //24B7B0 ascending groups,29C150 FIFO within the final redirect leaf.
        std::sort(s.order.begin(),s.order.end(),[&](std::uint32_t a,std::uint32_t b){
            const auto left=s.bindings[s.packets[a].binding].leaf,right=s.bindings[s.packets[b].binding].leaf;
            return left!=right?left<right:s.packets[a].ordinal<s.packets[b].ordinal;
        });
        s.orderedVertices.clear();s.orderedW.clear();s.orderedFog.clear();s.spans.clear();s.states.clear();
        for(const auto index:s.order){const auto& packet=s.packets[index];const auto& binding=s.bindings[packet.binding];
            auto span=binding.prototype.span;span.firstQuad=static_cast<std::uint32_t>(s.orderedVertices.size()/4);span.quadCount=packet.count;
            const auto start=std::size_t(packet.first)*4,count=std::size_t(packet.count)*4;
            s.orderedVertices.insert(s.orderedVertices.end(),s.vertices.begin()+start,s.vertices.begin()+start+count);
            s.orderedW.insert(s.orderedW.end(),s.w.begin()+start,s.w.begin()+start+count);
            s.orderedFog.insert(s.orderedFog.end(),s.fog.begin()+start,s.fog.begin()+start+count);
            s.spans.push_back(span);s.states.push_back(binding.prototype.native);
        }
        if(s.orderedVertices.size()!=s.vertices.size()||s.frameGeneration==std::numeric_limits<std::uint64_t>::max()){
            s.failed=true;return fail(d,E::capacity);
        }
        // Propose native binding updates before sealing. Only actual source
        // acceptance may commit them; a composed or rejected frame is no bind.
        s.pendingHistory=s.history;
        OriginalUiSourceTerminal50KN terminal{};
        std::uint32_t terminalLeaf=unresolved;
        for(const auto index:s.order){const auto leaf=s.bindings[s.packets[index].binding].leaf;
            if(leaf>=s.representatives.size()||s.representatives[leaf]>=s.bindings.size()){
                s.failed=true;return fail(d,E::schedule);
            }
            const auto& representative=s.bindings[s.representatives[leaf]];
            if(representative.registration>=s.pendingHistory.size()) {s.failed=true;return fail(d,E::schedule);}
            if(representative.pass->textureCount){
                assets::OriginalUiMaterialBindingDiagnostic50KG bindingDetail;
                // Native spans retain each actual selected ATM frame. The
                // representative records the last real bind in its FIFO; the
                // source unique-animation rule excludes merging regardless.
                const auto actualImage=s.bindings[s.packets[index].binding].image;
                if(!s.pendingHistory[representative.registration].state.recordDrainedBinding(*s.bank,actualImage,bindingDetail)){
                    s.failed=true;return fail(d,E::schedule,bindingDetail.sourcePC);
                }
            }
            // initialize() admits only a prepared key; recordDrainedBinding
            // preserves it. Keep each packet's state rejection without copying
            // arrays that a later actual drain would immediately overwrite.
            if(!s.pendingHistory[representative.registration].state.initialized()){
                s.failed=true;return fail(d,E::schedule);
            }
            terminalLeaf=leaf;
        }
        if(terminalLeaf!=unresolved){
            const auto& representative=s.bindings[s.representatives[terminalLeaf]];
            // The actual nonempty drain sequence selects the representative;
            // never use the highest registered row or the packet's prototype.
            const auto& source=*representative.pass;
            const auto& registration=s.pendingHistory[representative.registration];
            terminal.canonicalToken=registration.rank;terminal.pass=registration.pass;terminal.leaf=terminalLeaf;
            terminal.registryGeneration=s.registryGeneration;terminal.bankGeneration=s.bankGeneration;
            for(std::size_t field=0;field<source.gs.fields.size();++field){
                terminal.constructionFieldSourcePC[field]=source.gs.fields[field].sourcePC;
                terminal.constructionFieldByteOffset[field]=source.gs.fields[field].byteOffset;
            }
            terminal.enabled=source.enabled;terminal.textureCount=source.textureCount;
            terminal.textureCoordinateIndex=source.textureCoordinateIndex;
            terminal.lodCalculationMethod=source.lodCalculationMethod;terminal.maximumMipLevel=source.maximumMipLevel;
            terminal.magFilter=source.magFilter;terminal.minFilter=source.minFilter;terminal.lodL=source.lodL;terminal.lodK=source.lodK;
            // All binding updates have completed, including the final actual
            // image's IPUM TEXA mutation. Copy the final effective state once.
            if(!registration.state.copyEffectiveGs(terminal.gs)){
                s.failed=true;return fail(d,E::schedule);
            }
        }
        const auto* data=s.resources->widgets()->selectionData();const auto* page=data?data->page():nullptr;
        if(!page){s.failed=true;return fail(d,E::resource);}
        s.draw={};s.draw.draw.textures=s.gpu.get();s.draw.draw.texturePublication=s.publication;
        s.draw.draw.vertices=s.orderedVertices.data();s.draw.draw.quadCount=static_cast<std::uint32_t>(s.orderedVertices.size()/4);
        s.draw.draw.spans=s.spans.data();s.draw.draw.spanCount=static_cast<std::uint32_t>(s.spans.size());
        s.draw.draw.canvasWidth=640;s.draw.draw.canvasHeight=448;
        for(unsigned i=0;i<3;++i){s.draw.draw.clearRgba[i]=page->backgroundColor[i]/255.f;s.draw.particleFogColor[i]=s.render.sourceFogRgb[i]/255.f;}
        // The sealed renderer performs the actual raw-alpha128 source clear;
        // clearRgba alpha remains the established native presentation contract.
        s.draw.draw.clearRgba[3]=1;s.draw.draw.retainRaceImage=false;
        s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());
        s.draw.sourceRasterW=s.orderedW.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.orderedW.size());
        s.draw.particleFog=s.orderedFog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.orderedFog.size());
        s.draw.opaquePresentationAlphaOnly=false;
        s.finished=true;
        frame_.native_=&s.draw;frame_.owner_=this;frame_.frameGeneration_=++s.frameGeneration;frame_.terminal_=terminal;
        return true;
    }catch(...){s.failed=true;return fail(d,E::allocation);}
}
const OriginalUiSourceFrame50KM* OriginalUiRiderDrawContribution50KG::frame()const noexcept {
    return storage_&&storage_->active&&storage_->finished&&!storage_->failed&&
        !storage_->historyPoisoned&&!storage_->submissionPending&&
        storage_->bankGeneration==storage_->backend->generation()&&
        storage_->registryGeneration==storage_->backend->registryGeneration()&&frame_.native_?&frame_:nullptr;
}
void OriginalUiRiderDrawContribution50KG::drain() noexcept {
    frame_.native_=nullptr;frame_.owner_=nullptr;frame_.frameGeneration_=0;frame_.terminal_={};
    if(!storage_)return;auto& s=*storage_;
    if(s.submissionPending)s.historyPoisoned=true;
    if(s.active)s.backend->endFrame();s.active=false;s.finished=false;s.draw={};s.pendingHistory.clear();
}
bool OriginalUiRiderDrawContribution50KG::beginSourceSubmission(std::uint64_t generation)const noexcept {
    if(!storage_||!frame()||generation!=frame_.frameGeneration_)return false;
    auto& s=*storage_;if(s.submissionPending||s.historyPoisoned||generation<=s.acknowledgedGeneration||s.pendingHistory.size()!=s.history.size())return false;
    s.submissionPending=true;return true;
}
void OriginalUiRiderDrawContribution50KG::finishSourceSubmission(std::uint64_t generation,bool accepted,bool started)const noexcept {
    if(!storage_)return;auto& s=*storage_;
    if(!s.submissionPending||generation!=frame_.frameGeneration_){s.historyPoisoned=true;return;}
    s.submissionPending=false;
    if(accepted){s.history.swap(s.pendingHistory);s.acknowledgedGeneration=generation;s.pendingHistory.clear();}
    else if(started)s.historyPoisoned=true;
}
} // namespace rr::platform
