#include "original_ui_rider_backend50kg.h"
#include <array>
#include <limits>
#include <utility>
namespace rr::game {
namespace {
using E=OriginalUiRiderBackendError50KG;
using K=OriginalUiRiderWidgetKind50KG;
using O=OriginalUiControlOperation50KG;
constexpr std::uint32_t noOwner=std::numeric_limits<std::uint32_t>::max();
bool valid(std::uint16_t id) noexcept{return id>0&&id<120;}
bool genuine(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id) noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
}
struct OriginalUiRiderBackend50KG::Storage {
    struct Entry {
        std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> materials;
        OriginalUiRiderVisibilityHandle50KG membership{};
        std::array<std::unique_ptr<OriginalUiRiderBinderAnimation50KG>,2> binders;
        std::uint32_t imageOwner=noOwner;
        bool attached=false,shown=false;
    };
    OriginalUiRiderBackendInput50KG input;
    OriginalUiRiderVisibilityOwner50KG visibility;
    std::array<Entry,120> entries;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::uint32_t binderOwner=noOwner;
    std::uint64_t generation=1;
    std::uint64_t registryGeneration=1;
    bool frame=false;
};
OriginalUiRiderBackend50KG::OriginalUiRiderBackend50KG() noexcept=default;
OriginalUiRiderBackend50KG::~OriginalUiRiderBackend50KG()=default;
bool OriginalUiRiderBackend50KG::bind(const OriginalUiRiderBackendInput50KG& input,OriginalUiRiderBackendDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(storage_)return fail(E::alreadyBound);
    if(!genuine(input.rider,assets::OriginalUiArchiveId50KG::rider)||
       !genuine(input.appStart,assets::OriginalUiArchiveId50KG::appStart)||
       !genuine(input.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||
       !input.binder||!input.binder->loaded()||!input.binder->images()||
       !input.materialRuntime.overrideKnown)return fail(E::input);
    try {
        auto next=std::make_unique<Storage>();next->input=input;
        if(!next->visibility.bind(input.rider,d.membership))return fail(E::membership);
        // Real model loading uses these same authenticated archive objects.
        for(const auto& archive:{input.rider,input.appStart,input.podTextures}){
            bool found=false;for(const auto& existing:next->input.staticImages.archives)if(existing==archive)found=true;
            if(!found)next->input.staticImages.archives.push_back(archive);
        }
        auto population=next->input.staticImages;
        next->binderOwner=static_cast<std::uint32_t>(population.owners.size());
        population.owners.emplace_back(input.binder,input.binder->images());
        auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();
        if(!bank->load(population,d.bank))return fail(E::bank);
        next->bank=std::move(bank);storage_=std::move(next);diagnostic_={};return true;
    }catch(...){return fail(E::allocation);}
}
OriginalUiRiderDynamicBackend50KG OriginalUiRiderBackend50KG::callbacks() noexcept {
    return {this,
        [](void* p,const OriginalUiRiderDynamicOwner50KG& o,const OriginalUiRiderWidgetCallback50KG* c,std::size_t n) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->preflight(o,c,n);},
        [](void* p,std::uint16_t id,OriginalUiRiderDynamicOwner50KG& o) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->attach(id,o);},
        [](void* p,std::uint16_t id,OriginalUiRiderDynamicOwner50KG& o) noexcept{static_cast<OriginalUiRiderBackend50KG*>(p)->detach(id,o);},
        [](void* p,std::uint16_t id,bool v) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->setShown(id,v);}};
}
bool OriginalUiRiderBackend50KG::preflight(const OriginalUiRiderDynamicOwner50KG& owner,
    const OriginalUiRiderWidgetCallback50KG* calls,std::size_t count) noexcept {
    diagnostic_={};const auto fail=[&](E e){diagnostic_.error=e;return false;};
    if(!storage_||(count&&!calls)||count>32768)return fail(E::input);
    auto& s=*storage_;if(s.frame)return fail(E::busy);
    if(s.generation==std::numeric_limits<std::uint64_t>::max())return fail(E::input);
    try {
        std::array<bool,120> included{};
        std::array<std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG>,120> candidates{};
        for(std::uint16_t id=1;id<120;++id){included[id]=s.entries[id].attached;candidates[id]=s.entries[id].materials;}
        for(std::size_t i=0;i<count;++i){const auto& call=calls[i];diagnostic_.widget=call.widget;
            if(!valid(call.widget))return fail(E::input);
            const auto state=owner.state(call.widget);
            if(state.kind!=K::pod&&state.kind!=K::character)continue;
            const bool needs=call.operation==O::select||(state.kind==K::character&&call.operation==O::clearSelection);
            if(!needs)continue;included[call.widget]=true;
            if(candidates[call.widget])continue;
            auto materials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
            const bool loaded=state.kind==K::character?
                materials->loadCharacter(owner.characterAssets(call.widget),s.input.rider,s.input.appStart,
                    s.input.materialRuntime,s.input.staticImages.ipumCache,diagnostic_.material):
                materials->loadPod(owner.podAssets(call.widget),s.input.appStart,s.input.podTextures,
                    s.input.materialRuntime,s.input.staticImages.ipumCache,diagnostic_.material);
            if(!loaded)return fail(E::material);
            candidates[call.widget]=std::move(materials);
        }
        auto population=s.input.staticImages;
        const auto binderOwner=static_cast<std::uint32_t>(population.owners.size());
        population.owners.emplace_back(s.input.binder,s.input.binder->images());
        std::array<std::uint32_t,120> indices{};indices.fill(noOwner);
        // This order is a deterministic BANK remap only, never render order.
        // Keep current and newly-needed models for the entire callback batch.
        for(std::uint16_t id=1;id<120;++id)if(included[id]){
            if(!candidates[id]||!candidates[id]->images())return fail(E::material);
            indices[id]=static_cast<std::uint32_t>(population.owners.size());
            population.owners.push_back(candidates[id]->images());
        }
        auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();
        if(!bank->load(population,diagnostic_.bank))return fail(E::bank);
        // Actual CPU resource population completes before attachment and IK
        // capture. GPU publication remains a separate complete-frame operation.
        for(std::uint16_t id=1;id<120;++id){s.entries[id].materials=std::move(candidates[id]);s.entries[id].imageOwner=indices[id];}
        s.bank=std::move(bank);s.binderOwner=binderOwner;++s.generation;diagnostic_={};return true;
    }catch(...){return fail(E::allocation);}
}
bool OriginalUiRiderBackend50KG::attach(std::uint16_t id,OriginalUiRiderDynamicOwner50KG& owner) noexcept {
    diagnostic_={};diagnostic_.widget=id;
    const auto fail=[&](E e){diagnostic_.error=e;return false;};
    if(!storage_||!valid(id))return fail(E::input);
    auto& s=*storage_;auto& entry=s.entries[id];
    if(s.frame)return fail(E::busy);
    if(entry.attached||!entry.materials||entry.imageOwner==noOwner||
       s.registryGeneration==std::numeric_limits<std::uint64_t>::max())return fail(E::input);
    const auto state=owner.state(id);
    try {
        std::array<std::unique_ptr<OriginalUiRiderBinderAnimation50KG>,2> binders;
        OriginalUiRiderVisibilityHandle50KG membership{};
        if(state.kind==K::pod){
            auto* scene=owner.podScene(id);if(!scene||!scene->loaded())return fail(E::input);
            for(std::uint8_t pair=0;pair<2;++pair)if(scene->binderPresent(pair)){
                binders[pair]=std::make_unique<OriginalUiRiderBinderAnimation50KG>();
                if(!binders[pair]->initialize(s.input.binder))return fail(E::binder);
            }
            if(!s.visibility.registerModel(owner.podAssets(id),*scene,state.shown,membership,diagnostic_.membership))return fail(E::membership);
        }else if(state.kind==K::character){
            auto* scene=owner.characterScene(id);if(!scene||!scene->scene())return fail(E::input);
            if(!s.visibility.registerModel(owner.characterAssets(id),*scene,state.shown,membership,diagnostic_.membership))return fail(E::membership);
        }else return fail(E::input);
        // Native entity ownership is fixed: widget -> model -> binder0,binder1.
        // Only actual named pairs create helpers. No sorting by source masks.
        entry.binders=std::move(binders);entry.membership=membership;
        entry.shown=state.shown;entry.attached=true;
        // Source successful attachment calls the real parent+A4 resource
        // synchronization before character capture/IK registration. Publish
        // that rebuild event separately from CPU image-bank generations.
        ++s.registryGeneration;diagnostic_={};return true;
    }catch(...){return fail(E::allocation);}
}
void OriginalUiRiderBackend50KG::detach(std::uint16_t id,OriginalUiRiderDynamicOwner50KG&) noexcept {
    if(!storage_||!valid(id))return;auto& s=*storage_;auto& entry=s.entries[id];
    // No model packet queue lives here. The synchronous frame lease must have
    // ended before callbacks/destruction; invalidate access even on misuse.
    s.frame=false;
    if(entry.attached){s.visibility.setShown(entry.membership,false);s.visibility.removeModel(entry.membership);}
    entry.binders[1].reset();entry.binders[0].reset();entry.membership={};
    entry.attached=false;entry.shown=false;
    if(s.generation!=std::numeric_limits<std::uint64_t>::max())++s.generation;
}
bool OriginalUiRiderBackend50KG::setShown(std::uint16_t id,bool shown) noexcept {
    if(!storage_||!valid(id)||storage_->frame)return false;
    auto& s=*storage_;auto& entry=s.entries[id];
    if(entry.attached&&!s.visibility.setShown(entry.membership,shown))return false;
    entry.shown=shown;return true;
}
const OriginalUiRiderBackendDiagnostic50KG& OriginalUiRiderBackend50KG::diagnostic()const noexcept{return diagnostic_;}
OriginalUiRiderVisibilityOwner50KG* OriginalUiRiderBackend50KG::visibility() noexcept{return storage_?&storage_->visibility:nullptr;}
std::shared_ptr<const assets::OriginalUiArchive50KG> OriginalUiRiderBackend50KG::riderArchive()const noexcept{return storage_?storage_->input.rider:nullptr;}
bool OriginalUiRiderBackend50KG::beginFrame() noexcept {
    if(!storage_||storage_->frame||!storage_->bank)return false;storage_->frame=true;return true;
}
void OriginalUiRiderBackend50KG::endFrame() noexcept{if(storage_)storage_->frame=false;}
std::uint64_t OriginalUiRiderBackend50KG::generation()const noexcept{return storage_?storage_->generation:0;}
std::uint64_t OriginalUiRiderBackend50KG::registryGeneration()const noexcept{return storage_?storage_->registryGeneration:0;}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiRiderBackend50KG::bank()const noexcept{return storage_?storage_->bank:nullptr;}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::staticImage(std::uint32_t owner,std::uint32_t image)const noexcept {
    return storage_&&owner<storage_->input.staticImages.owners.size()?storage_->bank->mapping(owner,image):nullptr;
}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::modelImage(std::uint16_t id,std::uint32_t image)const noexcept {
    return storage_&&valid(id)&&storage_->entries[id].imageOwner!=noOwner?
        storage_->bank->mapping(storage_->entries[id].imageOwner,image):nullptr;
}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::binderImage(std::uint32_t image)const noexcept {
    return storage_?storage_->bank->mapping(storage_->binderOwner,image):nullptr;
}
const assets::OriginalUiRiderModelMaterials50KG* OriginalUiRiderBackend50KG::materials(std::uint16_t id)const noexcept {
    return storage_&&valid(id)?storage_->entries[id].materials.get():nullptr;
}
const OriginalUiRiderVisibilityHandle50KG* OriginalUiRiderBackend50KG::membership(std::uint16_t id)const noexcept {
    return attached(id)?&storage_->entries[id].membership:nullptr;
}
bool OriginalUiRiderBackend50KG::attached(std::uint16_t id)const noexcept{return storage_&&valid(id)&&storage_->entries[id].attached;}
bool OriginalUiRiderBackend50KG::shown(std::uint16_t id)const noexcept{return storage_&&valid(id)&&storage_->entries[id].shown;}
OriginalUiRiderBinderAnimation50KG* OriginalUiRiderBackend50KG::binder(std::uint16_t id,std::uint8_t pair) noexcept {
    return attached(id)&&pair<2?storage_->entries[id].binders[pair].get():nullptr;
}
}
