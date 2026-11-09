#include <cstdio>
#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "../../../src/game/original_camera_start_effects.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <algorithm>
#include <cstring>
#include <exception>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool emitterStop(D& d,S status,std::uint32_t pc)noexcept{d={};d.status=status;d.sourcePC=pc;return false;}
bool equalEmitterName(const char* a,const char* b)noexcept {
    if(!a||!b)return false;
    while(*a&&*b){unsigned char x=static_cast<unsigned char>(*a++),y=static_cast<unsigned char>(*b++);
        if(x>='a'&&x<='z')x=static_cast<unsigned char>(x-'a'+'A');
        if(y>='a'&&y<='z')y=static_cast<unsigned char>(y-'a'+'A');
        if(x!=y)return false;}
    return !*a&&!*b;
}
// Actual stack157320 file ownership for15AE00. This reached EMT reader has
//colon annotations/no-equals lines that the strict SCN/DAT parser cannot
//admit. Preserve the opened bytes; numeric15AEA0 uses its established bounded
//159440/158648 first-present parser. No preflight section object is borrowed.
class EmitterConfig157320 final {
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    std::shared_ptr<Provider> provider_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    Provider::MemberView member_{};
    assets::OriginalUiMemberView50KG authenticated_{};
    std::string requested_;
    std::size_t cursor_=0;
    bool opened_=false,closed_=false;
    bool current()const noexcept {
        if(!opened_||closed_||!provider_||!archive_||!authenticated_.identity||
           !member_.member||!member_.data||cursor_>member_.size)return false;
        Provider::MemberView now;
        return provider_->lookup132820(requested_,now)==Provider::Lookup::present&&
            now.member==member_.member&&now.data==member_.data&&now.size==member_.size&&
            now.mount==member_.mount&&now.ordinal==member_.ordinal&&now.revision==member_.revision;
    }
    bool read(void* destination,std::size_t count,std::size_t& actual)noexcept {
        actual=0;if(!current()||(!destination&&count))return false;
        actual=std::min(count,member_.size-cursor_);
        if(actual)std::memcpy(destination,member_.data+cursor_,actual);
        cursor_+=actual;return true;
    }
public:
    std::vector<std::uint8_t> readBuffer;
    bool open1577D0(std::shared_ptr<Provider> provider,
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive,const std::string& requested,D& d) {
        if(opened_||closed_||!provider||!archive||!archive->loaded())return emitterStop(d,S::phase,0x1577D0);
        Provider::MemberView found;
        if(provider->lookup132820(requested,found)!=Provider::Lookup::present)
            return emitterStop(d,S::source,0x15AE64); //No fabricated success/fallback.
        const auto original=archive->find(requested);
        if(!original.identity||!original.data||!found.data||original.size!=found.size||
           std::memcmp(original.data,found.data,found.size))return emitterStop(d,S::source,0x132820);
        provider_=std::move(provider);archive_=std::move(archive);member_=found;
        authenticated_=original;requested_=requested;opened_=true;
        std::uint8_t signature[7]{};std::size_t actual=0;
        if(!read(signature,sizeof(signature),actual))return emitterStop(d,S::phase,0x12F7A0);
        if(actual>=3&&signature[0]=='B'&&signature[1]=='P'&&signature[2]=='B')
            return emitterStop(d,S::source,0x1577D0);
        if(!current())return emitterStop(d,S::phase,0x12F640);
        cursor_=0;
        if(!found.size||found.size>32768)return emitterStop(d,S::source,0x159440);
        readBuffer.resize(found.size);
        if(!read(readBuffer.data(),readBuffer.size(),actual)||actual!=readBuffer.size())
            return emitterStop(d,S::phase,0x12F7A0);
        return true;
    }
    void close1573B0()noexcept {
        if(closed_)return;
        opened_=false;cursor_=0;member_={};authenticated_={};requested_.clear();
        archive_.reset();provider_.reset();std::vector<std::uint8_t>{}.swap(readBuffer);closed_=true;
    }
    ~EmitterConfig157320()noexcept{if(!closed_)std::terminate();}
};
}
OriginalUiGarageSceneEmitters50KG::~OriginalUiGarageSceneEmitters50KG()noexcept {
    for(const auto& row:rows)if(row&&(row->entityLive||row->retiring))std::terminate();
}
bool OriginalUiGaragePodScene50KG::populateEmitters274370(D& d)noexcept {
    d={};if(!currentPrefix()||populationStage_!=PopulationStage::actors||sceneEmitters_||
       !actorOwner3C8_||!toyFields_||!toyFields_->complete_||!toyFields_->particles3D8_||
       !sceneParticles_||!sceneParticles_->prepared||!sceneParticles_->entityLive||
       !sceneParticles_->numeric||!sceneParticles_->assetsOwner||!sceneParticles_->numeric->managerPrepared160E00()||
       !configResources_||!configResources_->domain_||!nativeApplication_)
        return emitterStop(d,S::phase,0x274370);
    auto& list=*toyFields_->particles3D8_;
    if(!list.live||list.count30!=7||!list.records34||list.entity.firstChild18)
        return emitterStop(d,S::phase,0x27DFD0);
    const auto* host=grant_->issuer();
    if(!host||host->state!=nativeApplication_||grant_->application()!=host->entities())
        return emitterStop(d,S::phase,0x27C84C);
    const auto failed=[&](S status,std::uint32_t pc){
        if(sceneEmitters_){sceneEmitters_->failed=true;sceneEmitters_->stoppedPC=pc;}
        sourceConstructionFailed_=true;return emitterStop(d,status,pc);
    };
    try {
        sceneEmitters_=std::make_unique<OriginalUiGarageSceneEmitters50KG>();
        for(std::size_t ordinal=0;ordinal<7;++ordinal){
            auto* wrapper=list.records34[ordinal];
            if(!wrapper||wrapper!=list.records[ordinal].get()||!wrapper->temp04||wrapper->live00)
                return failed(S::phase,0x27DFFC);
            const auto& descriptor=*wrapper->temp04;
            const auto id=static_cast<game::OriginalUiGarageEmitter50KG>(ordinal);
            const auto* definition=sceneParticles_->assetsOwner->emitter(id);
            const bool unattached=equalEmitterName(descriptor.attachToSoultree08.get(),"none");
            if(!definition||!descriptor.numInstances330||*descriptor.numInstances330!=1||
               !descriptor.startOn334||descriptor.typeID04!=definition->type||
               descriptor.startingZone00!=definition->zone||*descriptor.startOn334!=definition->startOn||
               !equalEmitterName(descriptor.emitterFilename210.get(),definition->filename.c_str())||
               unattached!=definition->attachMcf.empty()||
               (!unattached&&(!equalEmitterName(descriptor.attachToSoultree08.get(),definition->attachMcf.c_str())||
                !descriptor.childObject10C||!equalEmitterName(descriptor.childObject10C->c_str(),
                    definition->childName.empty()?"none":definition->childName.c_str()))))
                return failed(S::source,0x27C814);
            const auto& placement=unattached?definition->position:definition->offset;
            for(unsigned axis=0;axis<3;++axis)if(descriptor.position320[axis]!=placement[axis])
                return failed(S::source,0x27CC18);
            //27C8F8/27CA44/27CAA8/27CB08 pointer allocation precedes the ONE
            //actual derived Entity allocation/constructor for this descriptor.
            wrapper->unread08To10[0]=static_cast<std::uint32_t>(descriptor.typeID04); //27C81C
            sceneEmitters_->pendingPointerArray.reset(new C*[1]);
            sceneEmitters_->rows[ordinal]=std::make_unique<OriginalUiGarageSceneEmitters50KG::Row>();
            auto& row=*sceneEmitters_->rows[ordinal];
            auto entity=grant_->constructGarageSceneEmitter27C7B0(this,ordinal,row.entity,list.entity);
            if(!entity.okay())return failed(S::phase,entity.sourcePC);
            row.entityLive=true;sceneEmitters_->listBound=true;
            const auto numeric=sceneParticles_->numeric->constructEmitter15A5B0(id);
            if(!numeric.okay())return failed(S::source,numeric.sourcePC);
            row.numericConstructed=true;
            sceneEmitters_->pendingPointerArray[0]=&row.entity;
            wrapper->live00=std::move(sceneEmitters_->pendingPointerArray); //After constructor loop.
            wrapper->unread08To10[1]=1;wrapper->unread08To10[2]=*descriptor.startOn334?1u:0u;
            //27CC34 scans actual statics first. The bounded authenticated
            //Garage attaches only the live ToyList pd_fire/nozzle actors.
            if(!unattached){
                if(!resolveEmitterActor27CFF8(descriptor.attachToSoultree08.get(),
                    descriptor.childObject10C->c_str(),row.modelPin,row.attachmentNode,
                    row.authenticatedSourceRow,d))return failed(d.status,d.sourcePC);
                row.graphView=row.modelPin->graph();
                for(std::uint32_t index=0;index<row.modelPin->scene().sourceRowCount();++index){
                    const auto* object=row.modelPin->scene().sourceRow(index);
                    if(object&&object->sceneIndex==row.attachmentNode){row.attachment74=object;break;}
                }
                if(!row.attachment74)return failed(S::phase,0x27D098);
            }
            //141C20 is an identity return, not hide/disable or notification.
            row.manager70=&sceneParticles_->entity;
            if(!unattached&&!sceneParticles_->numeric->bindSourceAttachment15DEF0(id,row.graphView,
                row.attachmentNode,row.authenticatedSourceRow,*row.attachment74))return failed(S::phase,0x15DF30);
            const std::string requested=host->particleDirectory36B530()+descriptor.emitterFilename210.get();
            if(requested.size()>=row.filename78.size())return failed(S::source,0x15DFD8);
            std::memcpy(row.filename78.data(),requested.c_str(),requested.size()+1);row.filenameKnown=requested.size()+1;
            EmitterConfig157320 config;
            try {
                if(!config.open1577D0(configResources_->domain_->archiveProvider3E1A70(),assets_->archive(),requested,d)){
                    config.close1573B0();return failed(d.status,d.sourcePC);}
                const auto read=sceneParticles_->numeric->readEmitter15AEA0(id,config.readBuffer,
                    nativeApplication_->rawRandom308030);
                config.close1573B0(); //15AE50/54 before derived-loader return.
                if(!read.okay())return failed(S::source,read.sourcePC);
            }catch(...){config.close1573B0();throw;}
            row.loaded=true;
            const auto activate=sceneParticles_->numeric->activateEmitter27D7A0(id);
            if(!activate.okay())return failed(S::source,activate.sourcePC);
            entity=grant_->enableGarageSceneEmitter27D7A0(this,ordinal,row.entity,*descriptor.startOn334);
            if(!entity.okay())return failed(S::phase,entity.sourcePC);
            entity=grant_->attachGarageSceneEmitter27C7B0(this,ordinal,row.entity,list.entity);
            if(!entity.okay())return failed(S::phase,entity.sourcePC);
            row.attached=true;
            wrapper->temp04.reset(); //27D754 frees temp only after genuine attach.
            if(!sceneParticles_->numeric->finishEmitter27D75C(id))return failed(S::phase,0x27D75C);
            ++sceneEmitters_->completed;
        }
        sceneEmitters_->complete=true;populationStage_=PopulationStage::emitters;return true;
    }catch(...){return failed(S::allocation,0x27C7B0);}
}
bool OriginalUiGaragePodScene50KG::updatePageEmitters27DFC0(
    const OriginalUiGaragePageOwner50KG& page,float dt,OriginalUiGarageRendererLease50KG& renderer,
    VehicleWindowDiagnostic50DS& rendererDiagnostic,D& d)noexcept {
    d={};if(!grant_||!sceneEmitters_||!toyFields_||!toyFields_->particles3D8_||!sceneParticles_||
       !sceneParticles_->numeric||!sceneParticles_->numeric->prepared()||!sceneParticles_->assetsOwner||!nativeApplication_||
       page.grant_!=grant_||page.scene4C0_!=this||page.sceneBacking_.get()!=this||
       !page.updatePublication_||page.updateFailed_||page.updateReturned_||page.updatedPageChildren_!=12)
        return emitterStop(d,S::phase,0x27DFC0);
    auto& owned=*sceneEmitters_;auto& list=*toyFields_->particles3D8_;
    const auto* host=grant_->issuer();
    if(!host||page.issuer_!=host||host->state!=nativeApplication_||!renderer.currentFor(host)||
       !grant_->garageLateScenePageChildCurrent(page,*this,0,list.entity)||
       page.updateChild_!=&list.entity||page.sceneLateChildren_[0]!=&list.entity||
       !owned.complete||owned.failed||owned.retired||owned.listRetiring||owned.listRetired||
       owned.updateFailed||page.updatePublication_<=owned.updatePublication||
       (owned.updatePublication&&!owned.updateReturned)||!list.live||list.count30!=7||!list.records34)
        return emitterStop(d,S::phase,0x27DFC0);
    owned.updatePublication=page.updatePublication_;owned.updateReturned=false;
    owned.updatedRows=0;owned.updateSourcePC=0x141ED0;
    owned.updateChild=(list.entity.descendants24&2u)?list.entity.firstChild18:nullptr;
    bool invoked=false;
    const auto fail=[&](S status,std::uint32_t pc)noexcept {
        owned.updateFailed=true;owned.updateSourcePC=pc;
        emitterStop(d,status,pc);d.sourceEffectsCommitted=invoked;return false;
    };
    //27DFC0 is literally141ED0. Only genuine eligible direct children get
    //the renderer service and virtual38 call; never traverse Runtime.enabled.
    while(owned.updateChild){
        const auto ordinal=owned.updatedRows;if(ordinal>=owned.rows.size()||!owned.rows[ordinal])
            return fail(S::phase,0x141F00);
        auto& row=*owned.rows[ordinal];auto* child=owned.updateChild;
        if(child!=&row.entity||!row.entityLive||!row.numericConstructed||!row.loaded||!row.attached||
           row.retiring||row.retired||child->parent1C!=&list.entity||child->mask20!=0x402u||
           row.manager70!=&sceneParticles_->entity||!sceneParticles_->entityLive||
           !grant_->preflightGarageSceneEmitter15A720(this,ordinal,*child).okay())
            return fail(S::phase,0x141F00);
        //The named preflight above is read-only: exact enrolled identity,
        //ref1 and real immediate links, not a destructor or retirement claim.
        if((child->flags29&0x0du)==1u&&(child->mask20&2u)){
            owned.updateSourcePC=0x141F3C;game::OriginalCameraStartResult service;
            invoked=true; //Actual effectful renderer call, not an eligibility observation.
            if(!renderer.serviceEntity141F3C(rendererDiagnostic,service))
                return fail(S::source,service.sourcePC?service.sourcePC:0x141F3C);
            const auto id=static_cast<game::OriginalUiGarageEmitter50KG>(ordinal);
            const auto* definition=sceneParticles_->assetsOwner->emitter(id);
            if(!definition)return fail(S::phase,0x141F60);
            owned.updateSourcePC=0x15A7A0;
            const auto common=sceneParticles_->numeric->updateSourceCommon15A7A0(
                id,dt,host->animationPaused(),nativeApplication_->rawRandom308030);
            if(!common.okay())return fail(S::source,common.sourcePC);
            if(!common.commonSkipped){
                //Continue THIS actual Entity only, after its real common
                //prefix. The aggregate would repeat common and traverse a
                //different enabled set. Shared owners are never re-created.
                owned.updateSourcePC=definition->type==0?0x15BB40u:
                    definition->type==8?0x15E10Cu:definition->type==9?0x15EB08u:0x15F3C0u;
                if(definition->type==8){
                    const game::OriginalRacerSceneObject50LA* attachment=nullptr;
                    const auto prefix=sceneParticles_->numeric->beginSourceFire15E10C(id,dt,attachment);
                    if(!prefix.okay())return fail(S::source,prefix.sourcePC);
                    bool skipped=false;
                    if(attachment){
                        if(!row.modelPin||row.attachment74!=attachment||!visibility_){
                            std::fprintf(stderr,"garage_fire_attachment_rejected pc=15E198 pin=%d attachment=%d visibility=%d emitter=%u\n",int(bool(row.modelPin)),
                                int(row.attachment74==attachment),int(bool(visibility_)),unsigned(ordinal));
                            return fail(S::phase,0x15E198);}
                        if(!visibility_->skipFireObserver15E1B4(*this,*row.modelPin,
                            row.authenticatedSourceRow,*attachment,skipped,d))
                            return fail(S::source,d.sourcePC?d.sourcePC:0x15E1A4);
                    }
                    if(!skipped){
                        const auto derived=sceneParticles_->numeric->finishSourceFire15E1D0(id,dt,
                            nativeApplication_->rawRandom308030,nativeApplication_->fireSerial433970,
                            nativeApplication_->fireLazy84C8_);
                        if(!derived.okay())return fail(S::source,derived.sourcePC);
                    }
                }else{
                    const auto derived=sceneParticles_->numeric->continueSourceEmitterAfter15A7A0(
                        id,dt,nativeApplication_->rawRandom308030,
                        nativeApplication_->sharedRandomSelectors,nativeApplication_->clock101A50.get(),
                        nativeApplication_->explosionGlobals15E800_);
                    if(!derived.okay())return fail(S::source,derived.sourcePC);
                }
            }
            //Actual complete derived return, or common's source early return.
            //Fire/explosion remain closed at their individually unjoined branch.
        }
        ++owned.updatedRows;owned.updateChild=child->next14; //After callback return.
    }
    if((list.entity.descendants24&2u)&&owned.updatedRows!=owned.rows.size())return fail(S::phase,0x141F00);
    owned.updateReturned=true;owned.updateSourcePC=0x141F88;
    d.sourcePC=0x141F88;d.sourceEffectsCommitted=invoked;return true;
}
bool OriginalUiGaragePodScene50KG::retireSceneEmitters15A720(
    D& d,const OriginalUiGaragePageOwner50KG* attachedPage)noexcept {
    d={};if(!sceneEmitters_||sceneEmitters_->retired)return true;
    if(!grant_||!grant_->current()||!sceneParticles_||!sceneParticles_->numeric)
        return emitterStop(d,S::phase,0x142590);
    const auto* numericManager=sceneParticles_->numeric->manager();
    if(!numericManager)return emitterStop(d,S::phase,0x15A720);
    const bool retainedPool=numericManager->particleCount()!=0;
    if(retainedPool){
        // The live branch belongs only to the real Page tail after manager
        // detach and callback return. Detached constructor cleanup stays cold.
        const auto* manager=grant_->manager();
        if(!attachedPage||attachedPage->grant_!=grant_||attachedPage->scene4C0_!=this||
           attachedPage->sceneBacking_.get()!=this||!manager||manager->reference00!=1||
           manager->parent1C||manager->previous10||manager->next14||
           attachedPage->entity_.parent1C!=manager||!toyFields_||!toyFields_->particles3D8_||
           !grant_->garageLatePageListRetirementCurrent(*attachedPage,*this,0,toyFields_->particles3D8_->entity)||
           !attachedPage->updatePublication_||!attachedPage->completedUpdateFrames_||
           attachedPage->lastCompletedUpdatePublication_!=attachedPage->updatePublication_||
           !attachedPage->updateReturned_||attachedPage->updateFailed_||attachedPage->updateChild_||
           attachedPage->drawPending_||attachedPage->runtimeWidgetPending_||attachedPage->sharedHookPending_||
           attachedPage->timerCallbackPending_||attachedPage->speechStopPending_||
           !sceneEmitters_->complete||sceneEmitters_->failed||sceneEmitters_->listRetiring||
           sceneEmitters_->listRetired||sceneEmitters_->updateFailed||sceneEmitters_->updateChild||
           (sceneEmitters_->updatePublication&&(!sceneEmitters_->updateReturned||
             sceneEmitters_->updatePublication>attachedPage->updatePublication_))||
           !sceneParticles_->entityLive||sceneParticles_->retiring||sceneParticles_->retired||
           sceneParticles_->nativeGarageFifoPins_||sceneParticles_->updateFailed||
           (sceneParticles_->updatePublication&&(!sceneParticles_->updateReturned||
             sceneParticles_->updatePublication>attachedPage->updatePublication_))||
           !sceneParticles_->numeric->retainedPoolCurrentForEmitterRetirement15A720())
            return emitterStop(d,S::phase,0x15A720);
    }
    //142590 source sibling recursion reaches tail first. The list still owns
    //its wrappers/arrays here;27DCD0 releases those AFTER these destructors.
    for(std::size_t ordinal=7;ordinal>0;--ordinal){const auto index=ordinal-1;
        auto& owned=sceneEmitters_->rows[index];if(!owned)continue;auto& row=*owned;
        if(!row.entityLive)continue;
        if(!row.retiring){
            const auto preflight=grant_->preflightGarageSceneEmitter15A720(this,index,row.entity);
            if(!preflight.okay())return emitterStop(d,S::phase,preflight.sourcePC);
            const auto begun=grant_->beginGarageSceneEmitter15A720(this,index,row.entity,row.entity.parent1C);
            if(!begun.okay())return emitterStop(d,S::phase,begun.sourcePC);
            row.retiring=true;
        }
        //All authenticated EMTs have NumberOfForcedSets0, genuine null1C4.
        //No invented manager AddRef, observer unlink or renderer cleanup.
        if(row.numericConstructed){
            const auto id=static_cast<game::OriginalUiGarageEmitter50KG>(index);
            if(!(retainedPool?sceneParticles_->numeric->retireRetainedPoolEmitter15A720(id):
                 sceneParticles_->numeric->retireEmitter15A720(id)))return emitterStop(d,S::phase,0x15A748);
        }
        const auto finished=grant_->finishGarageSceneEmitter15A720(this,index,row.entity);
        if(!finished.okay())return emitterStop(d,S::phase,finished.sourcePC);
        row.modelPin.reset();row.entityLive=false;row.retiring=false;row.retired=true;
    }
    sceneEmitters_->complete=false;sceneEmitters_->retired=true;return true;
}
bool OriginalUiGaragePodScene50KG::releaseEmitterArrays27DD60(D& d)noexcept {
    d={};if(!sceneEmitters_)return true;
    if(sceneEmitters_->payloadReleased)return true;
    if(!sceneEmitters_->retired||!sceneEmitters_->listRetiring||!toyFields_||!toyFields_->particles3D8_||
       toyFields_->particles3D8_->entity.reference00||toyFields_->particles3D8_->entity.firstChild18)return emitterStop(d,S::phase,0x27DD60);
    //Called only by reached ParticleList27DCD0, after its reference-zero
    //transition and before list Entity base destruction. Native rows remain
    //as tombstones until the Scene owner is released.
    for(auto& wrapper:toyFields_->particles3D8_->records)if(wrapper){
        wrapper->temp04.reset();wrapper->live00.reset();wrapper.reset();
    }
    toyFields_->particles3D8_->records34.reset();
    sceneEmitters_->pendingPointerArray.reset(); //Only possible on retained constructor abort.
    sceneEmitters_->payloadReleased=true;return true;
}
bool OriginalUiGaragePodScene50KG::retirePageParticleList27DCD0(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!sceneEmitters_||!toyFields_||!toyFields_->particles3D8_||!grant_||!grant_->current())
        return emitterStop(d,S::phase,0x27DCD0);
    if(sceneEmitters_->listRetired)return true;
    auto& list=*toyFields_->particles3D8_;
    if(!sceneEmitters_->listRetiring){
        if(!grant_->garageLatePageListRetirementCurrent(page,*this,0,list.entity))
            return emitterStop(d,S::phase,0x1425B8);
        // Actual15A720 leaves live particles for the later initial child9
        //160C70. The typed Page branch proves their retained numeric custody.
        if(!retireSceneEmitters15A720(d,&page))return false;
        const auto begun=grant_->beginGaragePageParticleList142590(page,*this,list.entity);
        if(!begun.okay())return emitterStop(d,S::phase,begun.sourcePC);
        sceneEmitters_->listRetiring=true;
    }
    if(!releaseEmitterArrays27DD60(d))return false;
    const auto finished=grant_->finishGaragePageParticleList1413E0(page,*this,list.entity);
    if(!finished.okay())return emitterStop(d,S::phase,finished.sourcePC);
    list.live=false;sceneEmitters_->listRetiring=false;sceneEmitters_->listRetired=true;return true;
}
bool OriginalUiGaragePodScene50KG::retireSceneParticleList27DCD0(D& d)noexcept {
    d={};if(!sceneEmitters_||sceneEmitters_->listRetired)return true;
    if(!retireSceneEmitters15A720(d))return false;
    if(!toyFields_||!toyFields_->particles3D8_)return emitterStop(d,S::phase,0x27DCD0);
    auto& list=*toyFields_->particles3D8_;
    if(!sceneEmitters_->listBound){
        //Native allocation failed before the FIRST actual Entity constructor.
        //Release only uninitialized pointer arrays; existing strict detached
        //closeToyFieldsPartial271CF0 retains the untouched list/descriptors.
        for(auto& wrapper:list.records)if(wrapper)wrapper->live00.reset();
        sceneEmitters_->pendingPointerArray.reset();return true;
    }
    if(!sceneEmitters_->listRetiring){
        const auto ready=grant_->preflightGarageSceneParticleList27DCD0(this,list.entity);
        if(!ready.okay())return emitterStop(d,S::phase,ready.sourcePC);
        const auto begun=grant_->beginGarageSceneParticleList27DCD0(this,list.entity,list.entity.parent1C);
        if(!begun.okay())return emitterStop(d,S::phase,begun.sourcePC);
        sceneEmitters_->listRetiring=true;
    }
    if(!releaseEmitterArrays27DD60(d))return false;
    const auto finished=grant_->finishGarageSceneParticleList1413E0(this,list.entity);
    if(!finished.okay())return emitterStop(d,S::phase,finished.sourcePC);
    list.live=false;sceneEmitters_->listRetiring=false;sceneEmitters_->listRetired=true;return true;
}
}
