#include "original_ui_tournament_pod_owner50kg.h"
#include "original_ui_tournament_camera50kg.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
namespace rr::game {
namespace {
using D=OriginalUiTournamentPodDiagnostic50KG;
using E=OriginalUiTournamentPodError50KG;
bool fail(D& d,E e,std::uint32_t pc=0)noexcept {d.error=e;d.sourcePC=pc;return false;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id)noexcept {
    return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);
}
}
struct OriginalUiTournamentPodOwner50KG::Storage {
    // Ledger outlives every graph/helper/live-row handle, even failed cleanup.
    OriginalUiRiderMaterialLifetime50KG lifetime;
    OriginalUiTournamentPodInput50KG input;
    assets::OriginalUiImageBankInput50KG bankInput;
    std::shared_ptr<assets::OriginalUiRiderModelMaterials50KG> materials;
    std::shared_ptr<assets::OriginalUiRiderBinderAssets50KG> binderAssets;
    OriginalUiRiderMaterialFootprint50KG podFootprint,binderFootprint;
    struct ColdRow {
        OriginalUiRiderMaterialRow50KG row;
        std::vector<const assets::OriginalUiMaterialPass50KG*> sources;
        std::vector<assets::OriginalUiMaterialEquivalenceKey50KG> keys;
        std::vector<const assets::OriginalUiImageProvenance50KG*> images;
    };
    std::vector<ColdRow> coldRows;
    std::vector<std::shared_ptr<OriginalUiRiderLiveMaterial50KG>> instances;
    OriginalUiRiderPodScene50KG pod;
    std::array<std::unique_ptr<OriginalUiRiderBinderAnimation50KG>,2> binders;
    OriginalUiRiderVisibilityOwner50KG visibility;
    OriginalUiRiderVisibilityHandle50KG membership{};
    const rr::platform::OriginalUiTournamentDrawContribution50KG* consumer=nullptr;
    void* consumerContext=nullptr;
    bool (*consume)(void*,bool)noexcept=nullptr;
    std::uint64_t registryGeneration=1,submitted=0,retired=0,retiredFrontier=0,cameraSerial=0,updateSequence=0;
    bool attached=false,shown=false,everAttached=false,frame=false,poisoned=false,tornDown=false;
};
OriginalUiTournamentPodOwner50KG::OriginalUiTournamentPodOwner50KG()noexcept=default;
OriginalUiTournamentPodOwner50KG::~OriginalUiTournamentPodOwner50KG(){
    // No destructor may silently erase a live graph or renderer-borrowed epoch.
    if(storage_&&(storage_->lifetime.hasOwners()||!storage_->instances.empty()))storage_.release();
}
bool OriginalUiTournamentPodOwner50KG::prepareCold(const OriginalUiTournamentPodInput50KG& input,D& d)noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound,0x1D3590);
    const auto archive=input.pod?input.pod->archive():nullptr;
    if(!input.pod||!input.pod->loaded()||!input.pod->scene()||!input.pod->hierarchy()||
       !authentic(archive,assets::OriginalUiArchiveId50KG::tournament)||
       !authentic(input.appStart,assets::OriginalUiArchiveId50KG::appStart)||
       !authentic(input.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||
       !input.runtime.overrideKnown||input.currentStaticRows.empty())return fail(d,E::input,0x1D3590);
    try {
        auto s=std::make_unique<Storage>();s->input=input;
        assets::OriginalUiDat50KG widgets;assets::OriginalUiPageDiagnostic50KG widgetDiagnostic;
        bool enableOnSelect=false,disableOnSelect=false;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UITourn/Widgets.Dat",widgets,widgetDiagnostic,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,E::input,0x2A77E0);
        const auto* descriptor=widgets.section("Widget "+std::to_string(input.pod->widgetId()));
        if(!descriptor||!assets::originalUiBoolean50KG(*descriptor,"EnableOnSelect",false,enableOnSelect,widgetDiagnostic,0x2A7848)||
           !assets::originalUiBoolean50KG(*descriptor,"DisableOnSelect",false,disableOnSelect,widgetDiagnostic,0x2A7860)||
           !enableOnSelect||disableOnSelect)return fail(d,E::input,0x2A78C0);
        s->materials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
        s->binderAssets=std::make_shared<assets::OriginalUiRiderBinderAssets50KG>();
        if(!s->materials->loadTournamentPod(input.pod,input.appStart,input.podTextures,input.runtime,input.ipumCache,d.material))
            return fail(d,E::material,0x26E640);
        if(!s->binderAssets->loadTournament(archive,input.appStart,input.runtime,d.binder))return fail(d,E::binder,0x1D3E10);
        if(!OriginalUiRiderMaterialFootprint50KG::tournamentPod(*input.appStart,*input.pod,s->podFootprint,d.lifetime)||
           !OriginalUiRiderMaterialFootprint50KG::tournamentBinder(*input.appStart,*s->binderAssets,s->binderFootprint,d.lifetime)||
           !s->lifetime.bind(input.appStart,d.lifetime))return fail(d,E::lifetime,0x26E640);
        if(!s->visibility.bindTournament(archive,d.visibility))return fail(d,E::membership,0x1CCD44);
        const auto add=[&](const std::string& name,const assets::OriginalUiMaterialPass50KG& pass,
            const assets::OriginalUiImageProvenance50KG* image)->bool {
            OriginalUiRiderMaterialRow50KG row;
            if(!OriginalUiRiderMaterialRow50KG::prepare(*input.appStart,name,row,d.lifetime))return false;
            // All current static owners are retained. An alias is an unsupported
            // concurrent domain, not permission to omit it or reset its history.
            for(const auto& current:input.currentStaticRows)
                if(!current.prepared()||current.tableIdentity()!=row.tableIdentity()||current.same(row))return false;
            auto at=std::find_if(s->coldRows.begin(),s->coldRows.end(),[&](const auto& r){return r.row.same(row);});
            if(at==s->coldRows.end()){Storage::ColdRow r;r.row=row;s->coldRows.push_back(std::move(r));at=s->coldRows.end()-1;}
            assets::OriginalUiMaterialEquivalenceKey50KG key;assets::OriginalUiMaterialEquivalenceDiagnostic50KG detail;
            if(!pass.pass||!assets::prepareOriginalUiMaterialEquivalence50KG(pass,image,key,detail))return false;
            if(pass.pass<=at->sources.size()){
                const auto index=pass.pass-1;const auto* prior=at->images[index];
                if(bool(prior)!=bool(image)||(image&&(prior->memberIdentity!=image->memberIdentity||
                   prior->celIdentity!=image->celIdentity||prior->pstOffset!=image->pstOffset||prior->pstBytes!=image->pstBytes)))return false;
                const auto relation=assets::compareOriginalUiMaterialEquivalence50KG(at->keys[index],key);
                if(relation==assets::OriginalUiMaterialEquivalence50KG::unsupported||
                   (relation==assets::OriginalUiMaterialEquivalence50KG::notEquivalent&&
                    (!key.excludesMerging()||!at->keys[index].excludesMerging())))return false;
                const auto& previous=*at->sources[index];
                return previous.identity==pass.identity&&previous.inheritedTemplate==pass.inheritedTemplate&&
                    previous.provenance.origin==pass.provenance.origin&&previous.provenance.archive==pass.provenance.archive&&
                    previous.provenance.fallbackTexture==pass.provenance.fallbackTexture;
            }
            if(pass.pass!=at->sources.size()+1)return false;
            at->sources.push_back(&pass);at->keys.push_back(std::move(key));at->images.push_back(image);return true;
        };
        const auto* scene=input.pod->scene();
        if(scene->materials.size()!=s->materials->materialCount())return fail(d,E::material,0x26E640);
        for(std::uint32_t slot=0;slot<s->materials->materialCount();++slot)
            for(std::uint32_t ordinal=0;ordinal<s->materials->passCount(slot);++ordinal){
                const auto* pass=s->materials->pass(slot,ordinal);if(!pass)return fail(d,E::material,0x26E640);
                const auto images=s->materials->images();
                const auto* provenance=pass->source.textureCount&&images?images->sourceProvenance(pass->imageIndex):nullptr;
                if((pass->source.textureCount&&!provenance)||!add("ui/textures/"+scene->materials[slot],pass->source,provenance))
                    return fail(d,E::material,0x14D370);
            }
        if(!s->binderAssets->images()||!s->binderAssets->pass()||
           !add("ui/textures/binder",*s->binderAssets->pass(),s->binderAssets->images()->sourceProvenance(0)))
            return fail(d,E::binder,0x150450);
        if(s->coldRows.empty()||s->coldRows.size()>originalUiRiderMaterialRows50KG)return fail(d,E::lifetime,0x14D370);
        s->bankInput.archives={archive,input.appStart,input.podTextures};
        s->bankInput.ipumCache=input.ipumCache;
        if(!s->materials->images())return fail(d,E::material,0x26E640);
        s->bankInput.owners.push_back(s->materials->images());
        s->bankInput.owners.emplace_back(s->binderAssets,s->binderAssets->images());
        s->instances.reserve(s->coldRows.size());storage_=std::move(s);return true;
    }catch(...){return fail(d,E::allocation,0x1D3590);}
}
std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> OriginalUiTournamentPodOwner50KG::podAssets()const noexcept{return storage_?storage_->input.pod:nullptr;}
std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> OriginalUiTournamentPodOwner50KG::materials()const noexcept{return storage_?storage_->materials:nullptr;}
std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> OriginalUiTournamentPodOwner50KG::binderAssets()const noexcept{return storage_?storage_->binderAssets:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiTournamentPodOwner50KG::bankInput()const noexcept{return storage_?&storage_->bankInput:nullptr;}
const OriginalUiRiderMaterialLifetime50KG& OriginalUiTournamentPodOwner50KG::materialLifetime()const noexcept{return storage_->lifetime;}
std::uint64_t OriginalUiTournamentPodOwner50KG::registryGeneration()const noexcept{return storage_?storage_->registryGeneration:0;}
bool OriginalUiTournamentPodOwner50KG::attached()const noexcept{return storage_&&storage_->attached;}
bool OriginalUiTournamentPodOwner50KG::shown()const noexcept{return attached()&&storage_->shown;}
bool OriginalUiTournamentPodOwner50KG::teardownComplete()const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    return s.tornDown&&!s.frame&&!s.attached&&!s.binders[0]&&!s.binders[1]&&!s.pod.requiresModelCleanup()&&
        !s.lifetime.hasOwners()&&!s.lifetime.populationActive()&&!s.visibility.registeredCount()&&s.instances.empty()&&
        s.lifetime.sequence()==s.lifetime.acknowledgedSequence();
}
std::uint16_t OriginalUiTournamentPodOwner50KG::widgetId()const noexcept{return storage_?storage_->input.pod->widgetId():0;}
bool OriginalUiTournamentPodOwner50KG::binderPresent(std::size_t pair)const noexcept{return attached()&&pair<2&&bool(storage_->binders[pair]);}
bool OriginalUiTournamentPodOwner50KG::canonicalMaterialRow(const std::string& name,OriginalUiRiderMaterialRow50KG& out)const noexcept {
    if(!storage_)return false;OriginalUiRiderMaterialLifetimeDiagnostic50KG d;
    return OriginalUiRiderMaterialRow50KG::prepare(*storage_->input.appStart,name,out,d);
}
std::shared_ptr<OriginalUiRiderLiveMaterial50KG> OriginalUiTournamentPodOwner50KG::materialInstance(
    const OriginalUiRiderMaterialRow50KG& row,std::uint64_t epoch)const noexcept {
    if(!storage_||!epoch)return nullptr;
    for(const auto& i:storage_->instances)if(i&&i->ledger==&storage_->lifetime&&i->epoch==epoch&&i->row.same(row))return i;
    return nullptr;
}
std::shared_ptr<OriginalUiRiderLiveMaterial50KG> OriginalUiTournamentPodOwner50KG::liveMaterial(const std::string& name)const noexcept {
    OriginalUiRiderMaterialRow50KG row;OriginalUiRiderMaterialRowState50KG state;
    if(!canonicalMaterialRow(name,row)||!storage_->lifetime.rowState(row,state)||!state.ownerPresence||!state.epoch)return nullptr;
    return materialInstance(row,state.epoch);
}
bool OriginalUiTournamentPodOwner50KG::bindMaterialConsumer(
    const rr::platform::OriginalUiTournamentDrawContribution50KG* owner,void* context,bool(*consume)(void*,bool)noexcept)noexcept {
    if(!storage_||!owner||!context||!consume||storage_->consumer||storage_->lifetime.sequence()||storage_->everAttached)return false;
    storage_->consumer=owner;storage_->consumerContext=context;storage_->consume=consume;return true;
}
bool OriginalUiTournamentPodOwner50KG::attachSelected(D& d)noexcept {
    d={};if(!storage_)return fail(d,E::input,0x1D3590);auto& s=*storage_;
    if(!s.consumer||!s.consume||s.everAttached||s.attached||s.frame||s.poisoned||s.tornDown||s.lifetime.sequence())
        return fail(d,E::phase,0x1D3590);
    const auto reject=[&](E error,std::uint32_t pc){s.poisoned=true;return fail(d,error,pc);};
    // The graph/material token is acquired at the original load boundary.
    // Partial graph/helper construction is retained for explicit cleanup.
    s.everAttached=true;
    if(!s.pod.load(s.input.pod,s.lifetime,s.podFootprint,d.model))return reject(E::model,0x1D3590);
    try {
        // 1D33D0 already ran generic SELECT: authenticated EnableOnSelect=T,
        // DisableOnSelect=F make the widget selected/shown before 1D3590.
        // Source model attach141960 occurs at1D35F4 BEFORE binder creation.
        if(!s.visibility.registerModel(s.input.pod,s.pod,true,s.membership,d.visibility))return reject(E::membership,0x1D35F4);
        s.attached=true;s.shown=true;
        for(std::size_t pair=0;pair<2;++pair)if(s.pod.binderPresent(pair)){
            s.binders[pair]=std::make_unique<OriginalUiRiderBinderAnimation50KG>();
            if(!s.binders[pair]->initialize(s.binderAssets,s.lifetime,s.binderFootprint,d.lifetime))return reject(E::binder,0x1D3E10);
        }
        // Parent+A4 at1D37D4 follows both helper attachments. Only after it
        // returns do1D34E0 generic SELECT,1D34E8 unpause and1D34F4 show run.
        if(!synchronizeMaterials(d))return false;
        s.pod.selected();
        if(!setShown(true))return reject(E::membership,0x1D34F4);
        return true;
    }catch(...){return reject(E::allocation,0x1D3590);}
}
bool OriginalUiTournamentPodOwner50KG::setShown(bool shown)noexcept {
    if(!storage_||!storage_->attached||storage_->frame||storage_->tornDown||storage_->poisoned)return false;
    auto& s=*storage_;if(!s.visibility.setShown(s.membership,shown))return false;s.shown=shown;return true;
}
bool OriginalUiTournamentPodOwner50KG::advance(float inheritedDelta,std::uint64_t sequence,D& d)noexcept {
    d={};
    // Application140C54/88 caps f20 before140CF4's root slot38. The actual
    // manager2A3C9C/2A3FE0 and child141EEC/141F64 preserve that f12.
    // User_Rotate1D38D0 saves it in f21;1D3A38/80/C8 use that value.
    // It does not reload the distinct, uncapped Application250 word.
    if(!storage_||!storage_->attached||storage_->frame||storage_->poisoned||storage_->tornDown||
       !std::isfinite(inheritedDelta)||inheritedDelta<0||inheritedDelta>0.06f||
       storage_->updateSequence==std::numeric_limits<std::uint64_t>::max()||
       sequence!=storage_->updateSequence+1)return fail(d,E::phase,0x1D38B0);
    if(!storage_->pod.advance(inheritedDelta,d.model)){storage_->poisoned=true;return fail(d,E::model,0x1D38B0);}
    storage_->updateSequence=sequence;return true;
}
bool OriginalUiTournamentPodOwner50KG::beginFrame()noexcept {
    if(!storage_||!storage_->attached||storage_->frame||storage_->poisoned||storage_->tornDown||
       storage_->lifetime.populationActive()||storage_->lifetime.sequence()!=storage_->lifetime.acknowledgedSequence())return false;
    storage_->frame=true;storage_->cameraSerial=0;return true;
}
void OriginalUiTournamentPodOwner50KG::endFrame()noexcept {if(storage_){storage_->frame=false;storage_->cameraSerial=0;}}
bool OriginalUiTournamentPodOwner50KG::prepareSelected(const OriginalUiTournamentCameraOwner50KG& camera,
    const OriginalUiTournamentCameraFrame50KG& frame,OriginalUiTournamentPodPacket50KG& output,D& d)noexcept {
    d={};if(!storage_||!storage_->frame||!shown()||storage_->poisoned||!camera.isCurrent(frame)||
       camera.archive()!=storage_->input.pod->archive().get()||storage_->cameraSerial)
        return fail(d,E::phase,0x26EFC0);
    auto& s=*storage_;OriginalUiTournamentPodPacket50KG next;
    if(!s.visibility.prepareFrame(frame.cameraAfterFog(),{},d.visibility)||
       !s.visibility.selectModel(s.membership,next.selection,d.visibility)||!s.visibility.isCurrentSelection(next.selection))
        return fail(d,E::membership,0x26EFC0);
    s.cameraSerial=frame.serial();
    if(!next.selection.selected()){output=std::move(next);return true;}
    next.model.context=frame.context();
    if(!s.pod.projectPalette(next.selection.sourceModelFlags191,next.model.palette,d.model))return fail(d,E::model,0x2412E4);
    if(!prepareOriginalUiRiderModelFog50KG(*s.input.pod->hierarchy(),s.pod.graph(),frame.cameraAfterFog().position,
        frame.renderContext().fogStart,next.model.fog))return fail(d,E::model,0x26F070);
    if(!s.visibility.recordPacketFog(next.selection,next.model.fog.computeRadialFog1B2))return fail(d,E::membership,0x24136C);
    next.model.scene=s.input.pod->scene();next.model.sourceModelFlags191=next.selection.sourceModelFlags191;
    next.emitsGeometry=true;output=std::move(next);return true;
}
bool OriginalUiTournamentPodOwner50KG::projectBinder(std::size_t pair,const OriginalUiTournamentCameraOwner50KG& camera,
    const OriginalUiTournamentCameraFrame50KG& frame,std::uint32_t milliseconds,bool animate,
    OriginalUiTournamentBinderPacket50KG& output,D& d)noexcept {
    d={};if(!storage_||!storage_->frame||!shown()||!binderPresent(pair)||storage_->poisoned||
       !camera.isCurrent(frame)||camera.archive()!=storage_->input.pod->archive().get()||
       storage_->cameraSerial!=frame.serial()||(animate&&milliseconds!=frame.applicationCurrent230()))return fail(d,E::phase,0x1D3F90);
    OriginalUiRiderBinderQuad50KG source;OriginalUiTournamentBinderPacket50KG next;
    if(!storage_->pod.projectBinder(pair,frame.cameraAfterFog().binderCameraColumn,source,d.model))return fail(d,E::model,0x1D3F90);
    if(!(animate?storage_->binders[pair]->advanceAfterGeometry(milliseconds,next.image):storage_->binders[pair]->current(next.image)))
        return fail(d,E::binder,0x1D4240);
    if(projectOriginalUiRiderBinderPacket50KM(source,frame.context(),frame.lighting().ambient50,next.geometry)!=OriginalUiLitGeometryStatus50KM::complete)
        return fail(d,E::model,0x1D4240);
    output=std::move(next);return true;
}
bool OriginalUiTournamentPodOwner50KG::sourceSubmissionStarted(
    const rr::platform::OriginalUiTournamentDrawContribution50KG* owner,std::uint64_t generation)noexcept {
    if(!storage_||owner!=storage_->consumer||!generation||generation<=storage_->submitted||!storage_->frame||
       storage_->poisoned||storage_->tornDown||storage_->lifetime.populationActive()||storage_->lifetime.aborted())return false;
    storage_->submitted=generation;storage_->retired=0;storage_->retiredFrontier=0;return true;
}
bool OriginalUiTournamentPodOwner50KG::acceptSourceRetirement(
    const rr::platform::OriginalUiTournamentDrawContribution50KG* owner,std::uint64_t generation,std::uint64_t frontier)noexcept {
    if(!storage_||owner!=storage_->consumer||generation!=storage_->submitted||frontier!=storage_->lifetime.sequence()||
       storage_->frame||storage_->lifetime.populationActive())return false;
    storage_->retired=generation;storage_->retiredFrontier=frontier;return true;
}
bool OriginalUiTournamentPodOwner50KG::cleanupEntitled()const noexcept {
    return storage_&&!storage_->frame&&!storage_->lifetime.populationActive()&&
        (!storage_->submitted||storage_->retired==storage_->submitted);
}
bool OriginalUiTournamentPodOwner50KG::detachAfterRetirement(D& d)noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(!cleanupEntitled())return fail(d,E::completion,0x1D36F0);
    // Retain each incomplete phase for a real retry. Never force-clear leases.
    if(s.attached){
        if(!s.visibility.setShown(s.membership,false)||!s.visibility.removeModel(s.membership))return fail(d,E::membership,0x280A00);
        s.attached=false;s.shown=false;s.membership={};
    }
    for(int pair=1;pair>=0;--pair)if(s.binders[pair]){
        if(!s.binders[pair]->destroy(d.lifetime))return fail(d,E::lifetime,0x150590);
        s.binders[pair].reset();
    }
    if(!s.pod.modelDestroyedAfterDetach(d.model))return fail(d,E::model,0x1D36F0);
    return true;
}
bool OriginalUiTournamentPodOwner50KG::synchronizeMaterials(D& d)noexcept {
    auto& s=*storage_;const auto reject=[&](E error){
        OriginalUiRiderMaterialLifetimeDiagnostic50KG abort;
        if(s.lifetime.populationActive())s.lifetime.abortPopulation(s.lifetime.sequence(),abort);
        s.poisoned=true;return fail(d,error,0x14D370);
    };
    if(!s.consume||!cleanupEntitled()||s.poisoned||s.tornDown||s.registryGeneration==std::numeric_limits<std::uint64_t>::max())return reject(E::phase);
    if(!s.consume(s.consumerContext,false)||!finalizeMaterialEvents(s.lifetime.sequence(),d))return reject(E::lifetime);
    if(!s.lifetime.beginPopulation(s.lifetime.sequence(),d.lifetime))return reject(E::lifetime);
    try {
        while(const auto* row=s.lifetime.nextPopulationRow()){
            const auto cold=std::find_if(s.coldRows.begin(),s.coldRows.end(),[&](const auto& r){return r.row.same(*row);});
            if(cold==s.coldRows.end()||cold->sources.empty()||cold->sources.size()!=cold->keys.size()||
               s.instances.size()==s.instances.capacity())return reject(E::lifetime);
            auto instance=std::make_shared<OriginalUiRiderLiveMaterial50KG>();
            instance->ledger=&s.lifetime;instance->row=*row;instance->sources=cold->sources;instance->states.resize(cold->keys.size());
            for(std::size_t pass=0;pass<cold->keys.size();++pass){assets::OriginalUiMaterialBindingDiagnostic50KG detail;
                if(!cold->sources[pass]||cold->sources[pass]->pass!=pass+1||!instance->states[pass].initialize(cold->keys[pass],detail))return reject(E::material);
            }
            OriginalUiRiderMaterialRowState50KG state;OriginalUiRiderMaterialMutation50KG mutation;
            if(!s.lifetime.commitConstructedRow(*row,s.lifetime.sequence(),state,mutation,d.lifetime))return reject(E::lifetime);
            instance->epoch=state.epoch;s.instances.push_back(std::move(instance));
        }
        OriginalUiRiderMaterialMutation50KG mutation;
        if(!s.lifetime.finishPopulation(s.lifetime.sequence(),mutation,d.lifetime))return reject(E::lifetime);
        if(!s.consume(s.consumerContext,true))return reject(E::material);
        if(!s.lifetime.recordGroupRebuilt(s.lifetime.sequence(),mutation,d.lifetime))return reject(E::lifetime);
        ++s.registryGeneration;
        if(!s.consume(s.consumerContext,false)||!finalizeMaterialEvents(s.lifetime.sequence(),d))return reject(E::lifetime);
        return true;
    }catch(...){return reject(E::allocation);}
}
bool OriginalUiTournamentPodOwner50KG::finalizeMaterialEvents(std::uint64_t through,D& d)noexcept {
    if(!storage_||through!=storage_->lifetime.sequence()||storage_->lifetime.populationActive())return false;
    auto& s=*storage_;const auto first=s.lifetime.acknowledgedSequence();
    for(auto sequence=first;sequence<through;){OriginalUiRiderMaterialEvent50KG event;
        if(!s.lifetime.event(++sequence,event))return false;
        if(event.kind==OriginalUiRiderMaterialEventKind50KG::rowRetired&&
           (!s.lifetime.retirementCurrent(event.retirement)||!materialInstance(event.row,event.epoch)))return false;
    }
    for(auto sequence=first;sequence<through;){OriginalUiRiderMaterialEvent50KG event;s.lifetime.event(++sequence,event);
        if(event.kind!=OriginalUiRiderMaterialEventKind50KG::rowRetired)continue;
        s.instances.erase(std::remove_if(s.instances.begin(),s.instances.end(),[&](const auto& i){
            return i&&i->row.same(event.row)&&i->epoch==event.epoch;
        }),s.instances.end());
    }
    return s.lifetime.acknowledgeThrough(through,d.lifetime);
}
bool OriginalUiTournamentPodOwner50KG::finalizeTeardown(std::uint64_t consumed,D& d)noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(!cleanupEntitled()||s.attached||s.binders[0]||s.binders[1]||s.pod.requiresModelCleanup()||
       s.lifetime.hasOwners()||s.visibility.registeredCount()||consumed!=s.lifetime.sequence()||
       !finalizeMaterialEvents(consumed,d)||!s.instances.empty())return fail(d,E::lifetime,0x14D440);
    s.tornDown=true;return true;
}
}
