#include "original_ui_rider_backend50kg.h"
#include <algorithm>
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
        const void* coldModel=nullptr;
        OriginalUiRiderMaterialFootprint50KG footprint;
        bool attached=false,shown=false,everAttached=false;
    };
    // Declared before every token/live-row consumer; never reset or moved.
    OriginalUiRiderMaterialLifetime50KG lifetime;
    struct ColdRow {
        OriginalUiRiderMaterialRow50KG row;
        std::vector<const assets::OriginalUiMaterialPass50KG*> sources;
        std::vector<assets::OriginalUiMaterialEquivalenceKey50KG> keys;
        std::vector<const assets::OriginalUiImageProvenance50KG*> images;
    };
    OriginalUiRiderBackendInput50KG input;
    OriginalUiRiderMaterialFootprint50KG binderFootprint;
    std::vector<ColdRow> coldRows;
    std::vector<std::shared_ptr<OriginalUiRiderLiveMaterial50KG>> instances;
    const rr::platform::OriginalUiRiderDrawContribution50KG* consumer=nullptr;
    void* consumerContext=nullptr;
    bool (*consume)(void*,bool) noexcept=nullptr;
    std::uint64_t submittedWatermark=0,retiredWatermark=0,retiredFrontier=0;
    bool poisoned=false,tornDown=false;

    OriginalUiRiderVisibilityOwner50KG visibility;
    std::array<Entry,120> entries;
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::uint32_t binderOwner=noOwner;
    std::uint64_t generation=1;
    std::uint64_t registryGeneration=1;
    bool coldSealed=false,frame=false;
};
OriginalUiRiderBackend50KG::OriginalUiRiderBackend50KG() noexcept=default;
OriginalUiRiderBackend50KG::~OriginalUiRiderBackend50KG(){
    // A caller that skipped explicit source teardown must retain the ledger
    // and its tokens rather than destroy a published graph underneath them.
    if(storage_&&storage_->lifetime.hasOwners())storage_.release();
}
bool OriginalUiRiderBackend50KG::bind(const OriginalUiRiderBackendInput50KG& input,OriginalUiRiderBackendDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(storage_)return fail(E::alreadyBound);
    if(!genuine(input.rider,assets::OriginalUiArchiveId50KG::rider)||
       !genuine(input.appStart,assets::OriginalUiArchiveId50KG::appStart)||
       !genuine(input.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||
       !input.binder||!input.binder->loaded()||!input.binder->images()||
       !input.materialRuntime.overrideKnown||input.staticRows.empty())return fail(E::input);
    try {
        auto next=std::make_unique<Storage>();next->input=input;
        if(!next->visibility.bind(input.rider,d.membership))return fail(E::membership);
        if(!next->lifetime.bind(input.appStart,d.lifetime))return fail(E::lifetime);
        // Real model loading uses these same authenticated archive objects.
        for(const auto& archive:{input.rider,input.appStart,input.podTextures}){
            bool found=false;for(const auto& existing:next->input.staticImages.archives)if(existing==archive)found=true;
            if(!found)next->input.staticImages.archives.push_back(archive);
        }
        // No partial bank escapes bind. The private candidate's exact model
        // descriptors must exist before its single complete bank is sealed.
        storage_=std::move(next);diagnostic_={};return true;
    }catch(...){return fail(E::allocation);}
}
OriginalUiRiderDynamicBackend50KG OriginalUiRiderBackend50KG::callbacks() noexcept {
    return {this,
        [](void* p,const OriginalUiRiderDynamicOwner50KG& o,const OriginalUiRiderWidgetCallback50KG* c,std::size_t n) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->preflight(o,c,n);},
        [](void* p,std::uint16_t id,OriginalUiRiderDynamicOwner50KG& o) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->attach(id,o);},
        [](void* p,std::uint16_t id,OriginalUiRiderDynamicOwner50KG& o) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->detach(id,o);},
        [](void* p,std::uint16_t id,bool v) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->setShown(id,v);},
        [](void* p) noexcept->OriginalUiRiderMaterialLifetime50KG*{auto* b=static_cast<OriginalUiRiderBackend50KG*>(p);return b->storage_?&b->storage_->lifetime:nullptr;},
        [](void* p,std::uint16_t id) noexcept{return static_cast<OriginalUiRiderBackend50KG*>(p)->graphFootprint(id);}};
}
bool OriginalUiRiderBackend50KG::prepareColdAndSeal(const OriginalUiRiderDynamicOwner50KG& owner,
    OriginalUiRiderBackendDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(!storage_)return fail(E::input);auto& s=*storage_;
    if(s.coldSealed||s.bank||s.frame||s.lifetime.hasOwners()||s.lifetime.sequence()!=0||s.registryGeneration!=1||
       s.generation==std::numeric_limits<std::uint64_t>::max())return fail(E::input);
    for(const auto& entry:s.entries)if(entry.attached||entry.shown||entry.everAttached||
       entry.materials||entry.coldModel||entry.binders[0]||entry.binders[1])return fail(E::input);
    try {
        std::array<std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG>,120> candidates{};
        std::array<const void*,120> modelSources{};
        std::array<OriginalUiRiderMaterialFootprint50KG,120> footprints{};
        std::vector<Storage::ColdRow> coldRows;
        const auto addCold=[&](const std::string& name,const assets::OriginalUiMaterialPass50KG& pass,
            const assets::OriginalUiImageProvenance50KG* image){
            OriginalUiRiderMaterialRow50KG row;
            if(!OriginalUiRiderMaterialRow50KG::prepare(*s.input.appStart,name,row,d.lifetime))return false;
            for(const auto& existing:s.input.staticRows){
                if(!existing.prepared()||existing.tableIdentity()!=row.tableIdentity()||existing.same(row))return false;
            }
            auto at=std::find_if(coldRows.begin(),coldRows.end(),[&](const Storage::ColdRow& r){return r.row.same(row);});
            if(at==coldRows.end()){Storage::ColdRow added;added.row=row;coldRows.push_back(std::move(added));at=coldRows.end()-1;}
            if(!pass.pass)return false;
            assets::OriginalUiMaterialEquivalenceKey50KG key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
            if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,image,key,detail))return false;
            if(pass.pass<=at->sources.size()){
                const auto* priorImage=at->images[pass.pass-1];
                if(bool(priorImage)!=bool(image)||(image&&(priorImage->memberIdentity!=image->memberIdentity||
                   priorImage->celIdentity!=image->celIdentity||priorImage->pstOffset!=image->pstOffset||priorImage->pstBytes!=image->pstBytes)))return false;
                const auto relation=assets::compareOriginalUiMaterialEquivalence50KG(at->keys[pass.pass-1],key);
                if(relation==assets::OriginalUiMaterialEquivalence50KG::unsupported||
                   (relation==assets::OriginalUiMaterialEquivalence50KG::notEquivalent&&
                    (!key.excludesMerging()||!at->keys[pass.pass-1].excludesMerging())))return false;
                const auto& previous=*at->sources[pass.pass-1];
                return previous.identity==pass.identity&&previous.inheritedTemplate==pass.inheritedTemplate&&
                    previous.provenance.origin==pass.provenance.origin&&previous.provenance.archive==pass.provenance.archive&&
                    previous.provenance.fallbackTexture==pass.provenance.fallbackTexture;
            }
            if(pass.pass!=at->sources.size()+1)return false;
            at->sources.push_back(&pass);at->keys.push_back(std::move(key));at->images.push_back(image);return true;
        };
        const bool pod20=bool(owner.podAssets(28)),character20=bool(owner.characterAssets(54));
        if(!owner.podAssets(27)||!owner.characterAssets(53)||pod20!=character20)return fail(E::input);
        for(std::uint16_t id=1;id<120;++id){
            const auto pod=owner.podAssets(id);const auto character=owner.characterAssets(id);
            const auto state=owner.state(id);d.widget=id;
            if((state.kind==K::pod||state.kind==K::character)&&(state.selected||state.shown))return fail(E::input);
            if(!pod&&!character)continue;
            if((pod&&character)||(pod&&(id!=27&&id!=28))||(character&&(id!=53&&id!=54))||
               (pod&&state.kind!=K::pod)||(character&&state.kind!=K::character))return fail(E::input);
            auto materials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
            const bool loaded=character?
                materials->loadCharacter(character,s.input.rider,s.input.appStart,
                    s.input.materialRuntime,s.input.staticImages.ipumCache,d.material):
                materials->loadPod(pod,s.input.appStart,s.input.podTextures,
                    s.input.materialRuntime,s.input.staticImages.ipumCache,d.material);
            if(!loaded)return fail(E::material);
            if(!(character?OriginalUiRiderMaterialFootprint50KG::character(*s.input.appStart,*character,footprints[id],d.lifetime):
                OriginalUiRiderMaterialFootprint50KG::pod(*s.input.appStart,*pod,footprints[id],d.lifetime)))return fail(E::lifetime);
            const auto* scene=character?character->scene():pod->scene();
            if(!scene||scene->materials.size()!=materials->materialCount())return fail(E::material);
            for(std::uint32_t slot=0;slot<materials->materialCount();++slot)
                for(std::uint32_t ordinal=0;ordinal<materials->passCount(slot);++ordinal){
                    const auto* pass=materials->pass(slot,ordinal);if(!pass)return fail(E::material);
                    const auto* provenance=pass->source.textureCount?materials->images()->sourceProvenance(pass->imageIndex):nullptr;
                    if(!addCold("ui/textures/"+scene->materials[slot],pass->source,provenance))return fail(E::lifetime);
                }
            modelSources[id]=character?static_cast<const void*>(character.get()):static_cast<const void*>(pod.get());
            candidates[id]=std::move(materials);
        }
        OriginalUiRiderMaterialFootprint50KG binderFootprint;
        if(!OriginalUiRiderMaterialFootprint50KG::binder(*s.input.appStart,*s.input.binder,binderFootprint,d.lifetime)||
           !s.input.binder->pass()||!addCold("ui/textures/binder",*s.input.binder->pass(),s.input.binder->images()->sourceProvenance(0)))
            return fail(E::lifetime);
        // The only runtime batch can acquire graph27, its at-most-two named
        // helpers, and graph53 once. No callback/foreign factory can expand it.
        // Reserve fixed live-handle storage and prove the complete worst-case
        // journal/sequence budget before any graph or helper is constructed.
        const auto ownerRows=footprints[27].rowCount()+footprints[53].rowCount()+2*binderFootprint.rowCount();
        const auto batchEvents=4*ownerRows+4; // acquire + populate + worst cleanup + two distinct boundaries per attach
        if(ownerRows!=9||batchEvents>originalUiRiderMaterialEvents50KG||originalUiRiderMaterialOwners50KG<4||
           coldRows.size()>originalUiRiderMaterialRows50KG)return fail(E::lifetime);
        s.instances.reserve(coldRows.size());
        auto population=s.input.staticImages;
        const auto binderOwner=static_cast<std::uint32_t>(population.owners.size());
        population.owners.emplace_back(s.input.binder,s.input.binder->images());
        std::array<std::uint32_t,120> indices{};indices.fill(noOwner);
        // This order is a deterministic cold BANK remap only. No model graph,
        // helper, source registration, selected owner or render order changes.
        for(std::uint16_t id=1;id<120;++id)if(candidates[id]){
            if(!candidates[id]->images())return fail(E::material);
            indices[id]=static_cast<std::uint32_t>(population.owners.size());
            population.owners.push_back(candidates[id]->images());
        }
        auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();
        if(!bank->load(population,d.bank))return fail(E::bank);
        // Commit only to the still-private candidate. Collector policies and
        // actual GPU admission must succeed before the manager can escape.
        // No source material reference/lifetime is acquired by these caches.
        for(std::uint16_t id=1;id<120;++id){s.entries[id].materials=std::move(candidates[id]);
            s.entries[id].imageOwner=indices[id];s.entries[id].coldModel=modelSources[id];s.entries[id].footprint=footprints[id];}
        s.coldRows=std::move(coldRows);s.binderFootprint=binderFootprint;
        s.bank=std::move(bank);s.binderOwner=binderOwner;s.coldSealed=true;
        ++s.generation;diagnostic_={};d={};return true;
    }catch(...){return fail(E::allocation);}
}
bool OriginalUiRiderBackend50KG::preflight(const OriginalUiRiderDynamicOwner50KG& owner,
    const OriginalUiRiderWidgetCallback50KG* calls,std::size_t count) noexcept {
    diagnostic_={};const auto fail=[&](E e){diagnostic_.error=e;return false;};
    if(!storage_||!storage_->coldSealed||!storage_->bank||(count&&!calls)||count>32768)return fail(E::input);
    const auto& s=*storage_;if(s.frame||s.poisoned||s.tornDown)return fail(E::busy);
    if(s.generation==std::numeric_limits<std::uint64_t>::max())return fail(E::input);
    for(std::size_t i=0;i<count;++i){const auto& call=calls[i];diagnostic_.widget=call.widget;
        if(!valid(call.widget))return fail(E::input);const auto state=owner.state(call.widget);
        if(state.kind!=K::pod&&state.kind!=K::character)continue;
        const bool needs=call.operation==O::select||(state.kind==K::character&&call.operation==O::clearSelection);
        if(!needs)continue;const auto& entry=s.entries[call.widget];
        if((call.widget!=27&&call.widget!=53)||s.submittedWatermark||(!entry.attached&&entry.everAttached))return fail(E::input);
        const void* source=state.kind==K::character?static_cast<const void*>(owner.characterAssets(call.widget).get()):
            static_cast<const void*>(owner.podAssets(call.widget).get());
        if(!source||source!=entry.coldModel||!entry.materials||entry.imageOwner==noOwner||
           !s.bank->mapping(entry.imageOwner,0))return fail(E::material);
    }
    // No new descriptors, images, mappings, bank or generation publication.
    diagnostic_={};return true;
}
bool OriginalUiRiderBackend50KG::attach(std::uint16_t id,OriginalUiRiderDynamicOwner50KG& owner) noexcept {
    diagnostic_={};diagnostic_.widget=id;
    const auto fail=[&](E e){diagnostic_.error=e;if(storage_)storage_->poisoned=true;return false;};
    if(!storage_||(id!=27&&id!=53))return fail(E::input);
    auto& s=*storage_;auto& entry=s.entries[id];
    if(s.frame||s.submittedWatermark||s.poisoned||s.tornDown)return fail(E::busy);
    if(!s.coldSealed||!s.consume||entry.attached||entry.everAttached||!entry.materials||entry.imageOwner==noOwner||
       s.registryGeneration==std::numeric_limits<std::uint64_t>::max())return fail(E::input);
    const auto state=owner.state(id);
    // Each local candidate owns its wrapper token immediately. If later helper
    // construction or membership fails, destroy candidates in existing reverse
    // pair order. Never infer them from the not-yet-published entry array.
    std::array<std::unique_ptr<OriginalUiRiderBinderAnimation50KG>,2> candidates;
    const auto cleanup=[&]() noexcept {
        bool okay=true;for(int pair=1;pair>=0;--pair)if(candidates[pair]){
            if(candidates[pair]->destroy(diagnostic_.lifetime))candidates[pair].reset();
            else {entry.binders[pair]=std::move(candidates[pair]);okay=false;}
        }return okay;
    };
    try {
        OriginalUiRiderVisibilityHandle50KG membership{};
        if(state.kind==K::pod){
            auto* scene=owner.podScene(id);if(!scene||!scene->loaded())return fail(E::input);
            for(std::uint8_t pair=0;pair<2;++pair)if(scene->binderPresent(pair)){
                candidates[pair]=std::make_unique<OriginalUiRiderBinderAnimation50KG>();
                if(!candidates[pair]->initialize(s.input.binder,s.lifetime,s.binderFootprint,diagnostic_.lifetime)){
                    cleanup();return fail(E::binder);
                }
            }
            if(!s.visibility.registerModel(owner.podAssets(id),*scene,state.shown,membership,diagnostic_.membership)){
                cleanup();return fail(E::membership);
            }
        }else if(state.kind==K::character){
            auto* scene=owner.characterScene(id);if(!scene||!scene->scene())return fail(E::input);
            if(!s.visibility.registerModel(owner.characterAssets(id),*scene,state.shown,membership,diagnostic_.membership))return fail(E::membership);
        }else return fail(E::input);
        entry.binders=std::move(candidates);entry.membership=membership;
        entry.shown=state.shown;entry.attached=true;entry.everAttached=true;
        // Native row construction and actual grouping both finish before the
        // caller's character capture/IK boundary. Two attaches mean two syncs.
        if(!synchronizeMaterials())return false;
        diagnostic_={};return true;
    }catch(...){cleanup();return fail(E::allocation);}
}
bool OriginalUiRiderBackend50KG::detach(std::uint16_t id,OriginalUiRiderDynamicOwner50KG&) noexcept {
    if(!storage_||!valid(id))return false;auto& s=*storage_;auto& entry=s.entries[id];
    if(!cleanupEntitled()){diagnostic_.error=E::completion;return false;}
    // Never force-clear a live frame lease. Failed prerequisites preserve both
    // membership and every source owner. Partial cleanup records each phase.
    if(entry.attached){
        if(!s.visibility.setShown(entry.membership,false)||!s.visibility.removeModel(entry.membership)){
            s.poisoned=true;diagnostic_.error=E::membership;return false;
        }
        entry.membership={};entry.attached=false;entry.shown=false;
    }
    for(int pair=1;pair>=0;--pair)if(entry.binders[pair]){
        if(!entry.binders[pair]->destroy(diagnostic_.lifetime)){s.poisoned=true;diagnostic_.error=E::lifetime;return false;}
        entry.binders[pair].reset();
    }
    return true; // graph owner is released later, after actual scene invalidation
}
bool OriginalUiRiderBackend50KG::setShown(std::uint16_t id,bool shown) noexcept {
    if(!storage_||!valid(id)||storage_->frame||storage_->tornDown)return false;
    auto& s=*storage_;auto& entry=s.entries[id];
    if(entry.attached&&!s.visibility.setShown(entry.membership,shown))return false;
    entry.shown=shown;return true;
}
const OriginalUiRiderBackendDiagnostic50KG& OriginalUiRiderBackend50KG::diagnostic()const noexcept{return diagnostic_;}
OriginalUiRiderVisibilityOwner50KG* OriginalUiRiderBackend50KG::visibility() noexcept{return storage_?&storage_->visibility:nullptr;}
std::shared_ptr<const assets::OriginalUiArchive50KG> OriginalUiRiderBackend50KG::riderArchive()const noexcept{return storage_?storage_->input.rider:nullptr;}
bool OriginalUiRiderBackend50KG::beginFrame() noexcept {
    if(!storage_||storage_->frame||!storage_->bank||storage_->poisoned||storage_->tornDown)return false;storage_->frame=true;return true;
}
void OriginalUiRiderBackend50KG::endFrame() noexcept{if(storage_)storage_->frame=false;}
std::uint64_t OriginalUiRiderBackend50KG::generation()const noexcept{return storage_?storage_->generation:0;}
std::uint64_t OriginalUiRiderBackend50KG::registryGeneration()const noexcept{return storage_?storage_->registryGeneration:0;}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiRiderBackend50KG::bank()const noexcept{return storage_?storage_->bank:nullptr;}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::staticImage(std::uint32_t owner,std::uint32_t image)const noexcept {
    return storage_&&storage_->bank&&owner<storage_->input.staticImages.owners.size()?storage_->bank->mapping(owner,image):nullptr;
}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::modelImage(std::uint16_t id,std::uint32_t image)const noexcept {
    return storage_&&storage_->bank&&valid(id)&&storage_->entries[id].imageOwner!=noOwner?
        storage_->bank->mapping(storage_->entries[id].imageOwner,image):nullptr;
}
const assets::OriginalUiImageBankMapping50KG* OriginalUiRiderBackend50KG::binderImage(std::uint32_t image)const noexcept {
    return storage_&&storage_->bank?storage_->bank->mapping(storage_->binderOwner,image):nullptr;
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
const OriginalUiRiderMaterialFootprint50KG* OriginalUiRiderBackend50KG::graphFootprint(std::uint16_t id)const noexcept {
    if(!storage_||!storage_->coldSealed||(id!=27&&id!=53)||storage_->poisoned||storage_->tornDown||
       storage_->submittedWatermark||storage_->entries[id].everAttached)return nullptr;
    const auto& footprint=storage_->entries[id].footprint;return footprint.prepared()?&footprint:nullptr;
}
const OriginalUiRiderMaterialLifetime50KG& OriginalUiRiderBackend50KG::materialLifetime()const noexcept {
    return storage_->lifetime; // only the bound concrete collector/session calls this observer
}
bool OriginalUiRiderBackend50KG::canonicalMaterialRow(const std::string& name,OriginalUiRiderMaterialRow50KG& out)const noexcept {
    if(!storage_||!storage_->coldSealed)return false;OriginalUiRiderMaterialLifetimeDiagnostic50KG d;
    return OriginalUiRiderMaterialRow50KG::prepare(*storage_->input.appStart,name,out,d);
}
std::shared_ptr<OriginalUiRiderLiveMaterial50KG> OriginalUiRiderBackend50KG::materialInstance(
    const OriginalUiRiderMaterialRow50KG& row,std::uint64_t epoch)const noexcept {
    if(!storage_||!epoch)return nullptr;
    for(const auto& instance:storage_->instances)if(instance&&instance->ledger==&storage_->lifetime&&
        instance->epoch==epoch&&instance->row.same(row))return instance;
    return nullptr;
}
std::shared_ptr<OriginalUiRiderLiveMaterial50KG> OriginalUiRiderBackend50KG::liveMaterial(const std::string& name)const noexcept {
    OriginalUiRiderMaterialRow50KG row;OriginalUiRiderMaterialRowState50KG state;
    if(!canonicalMaterialRow(name,row)||!storage_->lifetime.rowState(row,state)||!state.ownerPresence||!state.epoch)return nullptr;
    return materialInstance(row,state.epoch);
}
bool OriginalUiRiderBackend50KG::bindMaterialConsumer(
    const rr::platform::OriginalUiRiderDrawContribution50KG* owner,void* context,bool(*consume)(void*,bool) noexcept) noexcept {
    if(!storage_||!storage_->coldSealed||!owner||!context||!consume||storage_->consumer||storage_->lifetime.sequence())return false;
    storage_->consumer=owner;storage_->consumerContext=context;storage_->consume=consume;return true;
}
bool OriginalUiRiderBackend50KG::sourceSubmissionStarted(
    const rr::platform::OriginalUiRiderDrawContribution50KG* owner,std::uint64_t generation) noexcept {
    if(!storage_||owner!=storage_->consumer||!generation||generation<=storage_->submittedWatermark||
       storage_->poisoned||storage_->tornDown||!storage_->frame||storage_->retiredWatermark||
       storage_->lifetime.populationActive()||storage_->lifetime.aborted())return false;
    storage_->submittedWatermark=generation;return true;
}
bool OriginalUiRiderBackend50KG::acceptSourceRetirement(
    const rr::platform::OriginalUiRiderDrawContribution50KG* owner,std::uint64_t generation,std::uint64_t frontier) noexcept {
    if(!storage_||owner!=storage_->consumer||generation!=storage_->submittedWatermark||
       frontier!=storage_->lifetime.sequence()||storage_->frame||storage_->lifetime.populationActive())return false;
    storage_->retiredWatermark=generation;storage_->retiredFrontier=frontier;return true;
}
bool OriginalUiRiderBackend50KG::cleanupEntitled()const noexcept {
    if(!storage_||storage_->frame||storage_->lifetime.populationActive())return false;
    // Zero is proven by the actual renderer submission entry point, never by a
    // cold bank, hidden widget, CPU lease or caller-supplied readiness flag.
    return !storage_->submittedWatermark||storage_->retiredWatermark==storage_->submittedWatermark;
}
bool OriginalUiRiderBackend50KG::synchronizeMaterials() noexcept {
    auto& s=*storage_;const auto fail=[&](E error){
        OriginalUiRiderMaterialLifetimeDiagnostic50KG abort;
        if(s.lifetime.populationActive())s.lifetime.abortPopulation(s.lifetime.sequence(),abort);
        s.poisoned=true;diagnostic_.error=error;return false;
    };
    if(!s.consume||s.frame||s.submittedWatermark||s.poisoned||s.tornDown)return fail(E::input);
    if(!s.lifetime.beginPopulation(s.lifetime.sequence(),diagnostic_.lifetime))return fail(E::lifetime);
    try {
        while(const auto* row=s.lifetime.nextPopulationRow()){
            const auto cold=std::find_if(s.coldRows.begin(),s.coldRows.end(),[&](const Storage::ColdRow& r){return r.row.same(*row);});
            if(cold==s.coldRows.end()||cold->sources.empty()||cold->sources.size()!=cold->keys.size()||
               s.instances.size()==s.instances.capacity())return fail(E::lifetime);
            auto instance=std::make_shared<OriginalUiRiderLiveMaterial50KG>();
            instance->ledger=&s.lifetime;instance->row=*row;instance->sources=cold->sources;
            instance->states.resize(cold->keys.size());
            for(std::size_t pass=0;pass<cold->keys.size();++pass){
                assets::OriginalUiMaterialBindingDiagnostic50KG detail;
                if(!cold->sources[pass]||cold->sources[pass]->pass!=pass+1||
                   !instance->states[pass].initialize(cold->keys[pass],detail))return fail(E::material);
            }
            // No row epoch exists until ALL native mutable passes succeeded.
            // Earlier completed rows remain real if this or a later row fails.
            OriginalUiRiderMaterialRowState50KG state;OriginalUiRiderMaterialMutation50KG mutation;
            if(!s.lifetime.commitConstructedRow(*row,s.lifetime.sequence(),state,mutation,diagnostic_.lifetime))return fail(E::lifetime);
            instance->epoch=state.epoch;s.instances.push_back(std::move(instance));
        }
        OriginalUiRiderMaterialMutation50KG population;
        if(!s.lifetime.finishPopulation(s.lifetime.sequence(),population,diagnostic_.lifetime))return fail(E::lifetime);
        if(!s.consume(s.consumerContext,true))return fail(E::material);
        OriginalUiRiderMaterialMutation50KG grouping;
        if(!s.lifetime.recordGroupRebuilt(s.lifetime.sequence(),grouping,diagnostic_.lifetime))return fail(E::lifetime);
        ++s.registryGeneration;
        if(!s.consume(s.consumerContext,false)||!finalizeMaterialEvents(s.lifetime.sequence()))return fail(E::lifetime);
        return true;
    }catch(...){return fail(E::allocation);}
}
bool OriginalUiRiderBackend50KG::finalizeMaterialEvents(std::uint64_t through) noexcept {
    if(!storage_||through!=storage_->lifetime.sequence()||storage_->lifetime.populationActive())return false;
    auto& s=*storage_;const auto first=s.lifetime.acknowledgedSequence();
    // Validate the entire prefix before changing either retained native rows or
    // the journal. The collector already accepted the same exact prefix.
    for(auto sequence=first;sequence<through;){OriginalUiRiderMaterialEvent50KG event;
        if(!s.lifetime.event(++sequence,event))return false;
        if(event.kind==OriginalUiRiderMaterialEventKind50KG::rowRetired&&
           (!s.lifetime.retirementCurrent(event.retirement)||!materialInstance(event.row,event.epoch)))return false;
    }
    for(auto sequence=first;sequence<through;){OriginalUiRiderMaterialEvent50KG event;s.lifetime.event(++sequence,event);
        if(event.kind!=OriginalUiRiderMaterialEventKind50KG::rowRetired)continue;
        s.instances.erase(std::remove_if(s.instances.begin(),s.instances.end(),[&](const auto& instance){
            return instance&&instance->row.same(event.row)&&instance->epoch==event.epoch;
        }),s.instances.end());
    }
    return s.lifetime.acknowledgeThrough(through,diagnostic_.lifetime);
}
bool OriginalUiRiderBackend50KG::finalizeTeardown(std::uint64_t consumed,OriginalUiRiderBackendDiagnostic50KG& d) noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(!cleanupEntitled()||s.lifetime.hasOwners()||s.visibility.registeredCount()||consumed!=s.lifetime.sequence()||
       !finalizeMaterialEvents(consumed)||!s.instances.empty()){
        s.poisoned=true;d=diagnostic_;d.error=E::lifetime;return false;
    }
    for(const auto& entry:s.entries)if(entry.attached||entry.binders[0]||entry.binders[1]){d.error=E::lifetime;return false;}
    s.tornDown=true;return true;
}

}
