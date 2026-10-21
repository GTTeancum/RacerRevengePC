#include "original_ui_rider_backend50kg.h"
#include <algorithm>
#include <cstring>
#include <array>
#include <limits>
#include <utility>
#include "../assets/original_ui_ipum_texture_cache50kg.h"
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
bool sameDefinitionMember(const assets::OriginalUiMemberIdentity50KG* a,
    const assets::OriginalUiMemberIdentity50KG* b) noexcept {
    if(a==b)return true;
    return a&&b&&a->name&&b->name&&a->sha256&&b->sha256&&a->bytes==b->bytes&&
        std::strcmp(a->name,b->name)==0&&std::strcmp(a->sha256,b->sha256)==0;
}
bool samePredecessorDefinition(const OriginalUiTournamentPredecessorPass50KG& old,
    const assets::OriginalUiMaterialPass50KG& next,const assets::OriginalUiImageProvenance50KG* image,
    const assets::OriginalUiScreenAssets50KG* animatedImages) noexcept {
    const auto& a=old.source;const auto& b=next;
    // The finite cross-archive alias is authored Tournament -> Rider only.
    // Equal content is a cold definition proof, never a shader-history merge.
    if(a.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::authoredPsm||
       b.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::authoredPsm||
       a.provenance.archive!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::tournament)||
       b.provenance.archive!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider)||
       !a.identity||!b.identity||!sameDefinitionMember(a.identity,b.identity)||a.inheritedTemplate!=b.inheritedTemplate||
       a.pass!=b.pass||a.microprogram!=b.microprogram||a.textureName!=b.textureName||a.shaderPassGroup!=b.shaderPassGroup||
       a.enabled!=b.enabled||a.textureCount!=b.textureCount||a.textureCoordinateIndex!=b.textureCoordinateIndex||
       a.lodCalculationMethod!=b.lodCalculationMethod||a.maximumMipLevel!=b.maximumMipLevel||
       a.magFilter!=b.magFilter||a.minFilter!=b.minFilter||a.lodL!=b.lodL||a.lodK!=b.lodK||
       a.authored.name!=b.authored.name||a.authored.sourceOffset!=b.authored.sourceOffset||
       a.authored.values.size()!=b.authored.values.size()||bool(image)!=!old.images.empty())return false;
    for(std::size_t i=0;i<a.gs.fields.size();++i){const auto& x=a.gs.fields[i];const auto& y=b.gs.fields[i];
        if(x.value!=y.value||!sameDefinitionMember(x.member,y.member)||x.byteOffset!=y.byteOffset||x.sourcePC!=y.sourcePC)return false;}
    for(std::size_t i=0;i<a.authored.values.size();++i){const auto& x=a.authored.values[i];const auto& y=b.authored.values[i];
        if(x.key!=y.key||x.text!=y.text||x.keyOffset!=y.keyOffset||x.valueOffset!=y.valueOffset||x.sourceValuePresent!=y.sourceValuePresent)return false;}
    const auto sameImage=[](const assets::OriginalUiImageProvenance50KG& x,const assets::OriginalUiImageProvenance50KG& y){
        return sameDefinitionMember(x.memberIdentity,y.memberIdentity)&&sameDefinitionMember(x.celIdentity,y.celIdentity)&&
            x.pstOffset==y.pstOffset&&x.pstBytes==y.pstBytes;
    };
    if(animatedImages){
        // Binder has ten distinct stored-PST frames. The incoming cold row's
        // initial equivalence key names frame0 only, while predecessor evidence
        // must retain and authenticate the complete real frame multiset.
        const auto* textures=animatedImages->textures();
        if(!textures||old.images.size()!=textures->textureCount()||old.images.empty())return false;
        for(const auto& prior:old.images){std::size_t oldCount=0,newCount=0;
            for(const auto& occurrence:old.images)if(sameImage(prior,occurrence))++oldCount;
            for(std::uint32_t i=0;i<textures->textureCount();++i){const auto* candidate=animatedImages->sourceProvenance(i);
                if(!candidate)return false;if(sameImage(prior,*candidate))++newCount;}
            if(oldCount!=newCount)return false;
        }
    }else for(const auto& prior:old.images)if(!image||!sameImage(prior,*image))return false;
    return true;
}
// Inspect actual authored material references before widening the immutable
// bank. Only absent authenticated kind2 cache entries make an optional rider
// unavailable; malformed materials or ambiguous source members remain errors.
bool materialCacheReady(const assets::Scene48D& scene,bool pod,
    const OriginalUiRiderBackendInput50KG& input,bool& ready,
    assets::OriginalUiRiderModelMaterialDiagnostic50KG& d) {
    ready=true;d={};
    assets::OriginalUiMaterialTemplates50KG templates;
    if(!templates.load(*input.appStart,d.materialDetail))return false;
    const auto simple=[](const std::string& value){
        if(value.empty()||value.size()>127||value=="."||value=="..")return false;
        for(unsigned char c:value)if(c<32||c>=127||c=='/'||c=='\\'||c==':')return false;
        return true;
    };
    if(scene.materials.empty()||scene.materials.size()>256)return false;
    for(std::size_t i=0;i<scene.materials.size();++i){
        d.material=static_cast<std::uint32_t>(i);const auto& name=scene.materials[i];
        if(!simple(name))return false;
        assets::OriginalUiMaterial50KG material;
        if(!assets::readOriginalUiMaterial50KG(*input.rider,"ui/textures/"+name+".psm",templates,
            assets::OriginalUiTemplate50KG::defaultMt,input.materialRuntime,material,d.materialDetail))return false;
        for(std::size_t j=0;j<material.passes.size();++j){
            d.pass=static_cast<std::uint32_t>(j);const auto& pass=material.passes[j];
            if(!pass.textureCount)continue;
            if(pass.textureCount!=1||!simple(pass.textureName))return false;
            const auto prefix="ui/textures/"+pass.textureName;
            if(input.rider->find(prefix+".atm").identity||
               (pod&&input.podTextures->find(prefix+".atm").identity))return false;
            auto member=input.rider->find(prefix+".pst");
            const auto shared=pod?input.podTextures->find(prefix+".pst"):assets::OriginalUiMemberView50KG{};
            if(member.identity&&shared.identity)return false;
            if(shared.identity)member=shared;
            if(!member.identity||!member.data||member.size!=member.identity->bytes||member.size<8)return false;
            const auto* bytes=member.data;
            const std::uint32_t kind=std::uint32_t(bytes[4])|(std::uint32_t(bytes[5])<<8)|
                (std::uint32_t(bytes[6])<<16)|(std::uint32_t(bytes[7])<<24);
            if(kind==2&&(!input.staticImages.ipumCache||
               !input.staticImages.ipumCache->textureFor(member,0,member.size)))ready=false;
        }
    }
    return true;
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
    // The source restart tick submits a closed, static-only Rider frame before
    // openingAC creates either graph. Freeze initial acquisition only after a
    // submission has observed actual dynamic lifetime events. Keep the total
    // submission watermark above intact for renderer-first destruction.
    std::uint64_t submittedDynamicFrontier=0;
    bool poisoned=false,tornDown=false;

    OriginalUiRiderVisibilityOwner50KG visibility;
    std::array<Entry,120> entries;
    std::array<std::uint16_t,32> pods{},characters{};
    std::shared_ptr<const assets::OriginalUiImageBank50KG> bank;
    std::uint32_t binderOwner=noOwner;
    std::uint64_t generation=1;
    std::uint64_t registryGeneration=1;
    bool coldSealed=false,frame=false,tournamentPredecessorRetired=false;
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
    if(input.tournamentPredecessor){
        const auto& prior=*input.tournamentPredecessor;
        if(!prior.creator_||!prior.bank_||!prior.bankGeneration_||!prior.registryGeneration_||prior.passes_.empty())return fail(E::input);
        for(std::size_t i=0;i<prior.passes_.size();++i){const auto& pass=prior.passes_[i];OriginalUiRiderMaterialRow50KG row;
            if(!pass.row.prepared()||pass.registeredName.empty()||!pass.source.pass||
               !OriginalUiRiderMaterialRow50KG::prepare(*input.appStart,pass.registeredName,row,d.lifetime)||!pass.row.same(row))return fail(E::input);
            for(std::size_t j=0;j<i;++j)if(prior.passes_[j].row.same(pass.row)&&prior.passes_[j].source.pass==pass.source.pass)return fail(E::input);
        }
    }
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
        std::array<bool,120> included{};
        std::array<std::uint16_t,32> pods{},characters{};
        std::size_t pairCount=0;
        for(const auto rider:originalUiRiderRoster50KG){
            const auto pod=owner.podWidget(rider),character=owner.characterWidget(rider);
            const auto p=owner.podAssets(pod);const auto c=owner.characterAssets(character);
            if(bool(p)!=bool(c))return fail(E::input);
            if(!p){if(rider==19||owner.previewRequested(rider))return fail(E::input);continue;}
            if(!valid(pod)||!valid(character)||!p->loaded()||!c->loaded()||!p->scene()||!c->scene())return fail(E::input);
            bool podReady=false,characterReady=false;
            d.widget=pod;
            if(!materialCacheReady(*p->scene(),true,s.input,podReady,d.material))return fail(E::material);
            d.widget=character;
            if(!materialCacheReady(*c->scene(),false,s.input,characterReady,d.material))return fail(E::material);
            if(!podReady||!characterReady){
                // Every explicitly requested pair is mandatory;
                // never seal a requested preview with either cache absent.
                if(rider==3||rider==7||rider==19||rider==20||owner.previewRequested(rider))return fail(E::material);
                continue;
            }
            if(included[pod]||included[character])return fail(E::input);
            included[pod]=included[character]=true;pods[rider]=pod;characters[rider]=character;++pairCount;
        }
        for(std::uint16_t id=1;id<120;++id){
            const auto pod=owner.podAssets(id);const auto character=owner.characterAssets(id);
            const auto state=owner.state(id);d.widget=id;
            if((state.kind==K::pod||state.kind==K::character)&&(state.selected||state.shown))return fail(E::input);
            if(!included[id])continue;
            if((pod&&character)||
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
        if(s.input.tournamentPredecessor){
            // Keep every predecessor row, including nonoverlapping rows. Only
            // overlapping immutable definitions are compared here; no lifetime
            // event or live instance can occur before the retirement capability.
            for(const auto& prior:s.input.tournamentPredecessor->passes_){
                const auto at=std::find_if(coldRows.begin(),coldRows.end(),[&](const Storage::ColdRow& r){return r.row.same(prior.row);});
                if(at==coldRows.end())continue;
                if(prior.source.pass>at->sources.size()||
                   !samePredecessorDefinition(prior,*at->sources[prior.source.pass-1],at->images[prior.source.pass-1],
                       prior.registeredName=="ui/textures/binder"?s.input.binder->images():nullptr))return fail(E::lifetime);
                for(std::size_t pass=1;pass<=at->sources.size();++pass){bool found=false;
                    for(const auto& old:s.input.tournamentPredecessor->passes_)if(old.row.same(prior.row)&&old.source.pass==pass){found=true;break;}
                    if(!found)return fail(E::lifetime);
                }
            }
        }
        // Bound every prepared graph and both optional helpers, including pods
        // authored to remain resident after deselection. Events are consumed at
        // each real parent+A4 boundary; no unbounded journal accumulates per move.
        std::size_t ownerRows=0;
        for(std::size_t rider=0;rider<pods.size();++rider)if(pods[rider])
            ownerRows+=footprints[pods[rider]].rowCount()+footprints[characters[rider]].rowCount()+2*binderFootprint.rowCount();
        const auto batchEvents=4*ownerRows+4*pairCount;
        if(!pairCount||batchEvents>originalUiRiderMaterialEvents50KG||4*pairCount>originalUiRiderMaterialOwners50KG||
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
        // Explicit preview pairs require complete private candidates before seal.
        // Mapping alone never implies descriptor/material readiness.
        for(const auto rider:originalUiRiderRoster50KG)
            if(owner.previewRequested(rider)||(rider==3&&owner.podAssets(owner.podWidget(rider)))){
            if(!pods[rider]||!characters[rider])return fail(E::input);
            for(const auto id:{pods[rider],characters[rider]})
                if(!modelSources[id]||!candidates[id]||!candidates[id]->loaded()||
                   !footprints[id].prepared()||indices[id]==noOwner||!bank->mapping(indices[id],0))
                    return fail(E::material);
        }
        // Commit only to the still-private candidate. Collector policies and
        // actual GPU admission must succeed before the manager can escape.
        // No source material reference/lifetime is acquired by these caches.
        for(std::uint16_t id=1;id<120;++id){s.entries[id].materials=std::move(candidates[id]);
            s.entries[id].imageOwner=indices[id];s.entries[id].coldModel=modelSources[id];s.entries[id].footprint=footprints[id];}
        s.coldRows=std::move(coldRows);s.binderFootprint=binderFootprint;s.pods=pods;s.characters=characters;
        s.bank=std::move(bank);s.binderOwner=binderOwner;s.coldSealed=true;
        ++s.generation;diagnostic_={};d={};return true;
    }catch(...){return fail(E::allocation);}
}
bool OriginalUiRiderBackend50KG::predecessorRetired() const noexcept {
    return storage_&&(!storage_->input.tournamentPredecessor||storage_->tournamentPredecessorRetired);
}
bool OriginalUiRiderBackend50KG::acceptTournamentPredecessorRetirement(
    OriginalUiTournamentPredecessorRetirement50KG& token,OriginalUiRiderBackendDiagnostic50KG& d) noexcept {
    d={};if(!storage_){d.error=E::input;return false;}auto& s=*storage_;
    const auto prior=s.input.tournamentPredecessor;
    if(!prior||s.tournamentPredecessorRetired||!s.coldSealed||!s.bank||s.frame||s.poisoned||s.tornDown||
       s.lifetime.sequence()!=0||s.lifetime.hasOwners()||!s.instances.empty()||s.submittedWatermark||
       !token.issued()||token.inventory_!=prior||token.creator_!=prior->creator_||
       token.materialSequence_<prior->materialSequence_){d.error=E::completion;return false;}
    token.consumed_=true;s.tournamentPredecessorRetired=true;return true;
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
        if(!s.entries[call.widget].coldModel||(s.submittedDynamicFrontier&&!cleanupEntitled()))return fail(E::input);
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
    if(!storage_||!valid(id)||!storage_->entries[id].coldModel)return fail(E::input);
    auto& s=*storage_;auto& entry=s.entries[id];
    if(s.frame||!predecessorRetired()||(s.submittedDynamicFrontier&&!cleanupEntitled())||s.poisoned||s.tornDown)return fail(E::busy);
    if(!s.coldSealed||!s.consume||entry.attached||!entry.materials||entry.imageOwner==noOwner||
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
bool OriginalUiRiderBackend50KG::preparedRider(std::uint32_t rider)const noexcept {
    if(!storage_||!storage_->coldSealed||!storage_->bank||storage_->poisoned||storage_->tornDown||rider>=storage_->pods.size())return false;
    const auto pod=storage_->pods[rider],character=storage_->characters[rider];
    if(!pod||!character)return false;
    for(const auto id:{pod,character}){const auto& entry=storage_->entries[id];
        if(!entry.coldModel||!entry.materials||!entry.materials->loaded()||!entry.footprint.prepared()||
           entry.imageOwner==noOwner||!storage_->bank->mapping(entry.imageOwner,0))return false;
    }
    return true;
}
bool OriginalUiRiderBackend50KG::beginFrame() noexcept {
    // Cold static prototypes are local unbound copies, not live history leases.
    // Even a direct collector caller cannot publish them until the exact
    // Tournament predecessor has genuinely retired and its token was consumed.
    if(!predecessorRetired()||storage_->frame||!storage_->bank||storage_->poisoned||storage_->tornDown)return false;storage_->frame=true;return true;
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
    if(!storage_||!predecessorRetired()||!storage_->coldSealed||!valid(id)||!storage_->entries[id].coldModel||storage_->poisoned||storage_->tornDown||
       (storage_->submittedDynamicFrontier&&!cleanupEntitled())||storage_->entries[id].attached)return nullptr;
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
       storage_->poisoned||storage_->tornDown||!storage_->frame||
       storage_->lifetime.populationActive()||storage_->lifetime.aborted())return false;
    // Derived here from the real ledger at the authenticated renderer entry,
    // never from widget visibility, a cold cache, or a caller readiness flag.
    // Sequence is monotonic, so a failed dynamic attempt also closes the gate.
    storage_->submittedDynamicFrontier=storage_->lifetime.sequence();
    // The new submission closes the prior exact renderer retirement. A stale
    // completion cannot authorize destruction of this generation's graphs.
    storage_->retiredWatermark=0;storage_->retiredFrontier=0;
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
    if(!s.consume||s.frame||(s.submittedDynamicFrontier&&!cleanupEntitled())||s.poisoned||s.tornDown)return fail(E::input);
    // Deselect may have retired the outgoing rows. Consume their exact epochs
    // before beginPopulation can construct replacement rows at the same rank.
    // Otherwise vector capacity and mutable pass histories retain dead owners.
    if(!s.consume(s.consumerContext,false)||!finalizeMaterialEvents(s.lifetime.sequence()))return fail(E::lifetime);
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
