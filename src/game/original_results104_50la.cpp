#include "original_results104_storage50la.h"
#include "../../tests/native/original_results_renderer50la.h"
#include "../../tests/native/original_native_collector_fifo50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include <bit>
#include <exception>
#include <utility>
namespace rr::game {
OriginalResults10450LA::Impl::Impl(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t session)
    :application(std::move(a)) {
    if(!application||!application->application10()||!session||application->attachmentLock423F58()||
       application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
    const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
    if(!application->entityAuthority_.canEnroll(component,key))return;
    const auto r=initializeOriginalWreckComponent50LA(component,OriginalWreckComponentKind50LA::entity141300,
        session,key,true,application->objectCount423F60()); //1BE950 A1=1 from1A8AF4.
    if(r.status!=OriginalWreckComponentStatus50LA::complete)return;
    counted=true;
    if(!application->entityAuthority_.enroll(component))std::terminate();
    //1BE964 shared-base table stage, then actual embedded constructor order.
    // No guessed raw-vtable image is exposed by the native Entity kind.
    center40.emplace();backdropsA0.emplace();base180.emplace();base.emplace();
    //1BEA2C derived table stage; the concrete class is this owner, not a label
    // copied from a native snapshot. Empty embedded constructors cannot fail.
    icons210.emplace();labels3A0.emplace();titles490.emplace();medals5F0.emplace();
    text710.emplace();text780.emplace();text7F0.emplace();text860.emplace();
    text8D0.emplace();text940.emplace();text9B0.emplace();textA20.emplace();
    tableAF0.emplace();separators1030.emplace();text1150.emplace();
    statistics1230.emplace();text15B0.emplace();text1770.emplace();
    component.mask20=mask=0xffffffdfu; //1BEBE8, before the scalar suffix.
    derived.emplace(); //20C/200/204/390/ADC/AE0;5E0 source null is known.
    constructed=true;
}
OriginalResults10450LA::OriginalResults10450LA(std::shared_ptr<OriginalApplicationEntities50LA> a,
    std::uint64_t session):impl_(new Impl(std::move(a),session)){}
OriginalResults10450LA::~OriginalResults10450LA()noexcept {
    OriginalResults104Diagnostic50LA d;
    if(impl_&&!closeDetached(d))std::terminate();
}
bool OriginalResults10450LA::constructed()const noexcept{return impl_&&impl_->constructed&&!impl_->closed&&!impl_->retirementStarted;}
bool OriginalResults10450LA::Impl::detached()const noexcept {
    return counted&&application&&application->application10()&&!application->attachmentLock423F58()&&
        component.reference00==1&&component.kind==OriginalWreckComponentKind50LA::entity141300&&
        component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&component.sessionGeneration&&
        !component.parent1C&&!component.previous10&&!component.next14&&!component.firstChild18&&
        !component.descendants24&&component.mask20==mask&&component.category28==0xff&&
        (component.flags29&15u)==(disabled?0u:3u)&&application->entityAuthority_.canRetire(component);
}
bool OriginalResults10450LA::Impl::stop(D& d,S status,std::uint32_t pc,const char* operation)noexcept {
    d.status=status;d.sourcePC=pc;d.operation=operation;d.retainedPrimitives=references.associatedCount();return false;
}
OriginalWreckComponent50LA* OriginalResults10450LA::entity()noexcept {
    return constructed()?&impl_->component:nullptr;
}
bool OriginalResults10450LA::attachedMask(std::uint32_t mask)noexcept {
    if(!constructed()||!impl_->loadComplete||!impl_->disabled||impl_->component.mask20!=mask)return false;
    impl_->mask=mask;return true;
}
bool OriginalResults10450LA::disable1A8B1C()noexcept {
    if(!constructed()||!impl_->loadComplete||impl_->disabled||!impl_->detached())return false;
    //Actual slot18=141520 recursively clears both activity bits. The actual
    //Results Entity has no Entity children; ScreenObjects are owned resources.
    impl_->component.flags29=std::uint8_t(impl_->component.flags29&~3u);
    impl_->disabled=true;return true;
}
bool OriginalResults10450LA::loadedDisabled()const noexcept {
    return constructed()&&impl_->loadComplete&&impl_->disabled;
}
void OriginalResults10450LA::Impl::closeText(Text& t)noexcept {
    t.string8.reset();t.glyphsC.reset(); //2DFC60: storage before primitive loop.
    if(t.primitives4)for(std::size_t i=0;i<t.count0;++i)if(t.primitives4[i]){
        references.release299AC0(t.primitives4[i]->reference);t.primitives4[i].reset();
    }
    t.primitives4.reset();t.packets64.reset();t.font4C.reset();
}
bool OriginalResults10450LA::belongsTo(const std::shared_ptr<OriginalApplicationEntities50LA>& application,
    std::uint64_t generation)const noexcept {
    return constructed()&&impl_->application==application&&impl_->component.sessionGeneration==generation;
}
bool OriginalResults10450LA::retirementCurrent()const noexcept {
    if(!impl_||impl_->closed)return true;
    const auto& p=*impl_;const auto& e=p.component;
    if(!p.counted)return !p.constructed;
    return p.application&&!p.application->attachmentLock423F58()&&e.reference00==1&&
        e.kind==OriginalWreckComponentKind50LA::entity141300&&e.sessionGeneration&&
        e.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&e))&&
        !e.firstChild18&&!e.descendants24&&e.category28==0xff&&e.mask20==p.mask&&
        (e.flags29&15u)==(p.disabled?0u:3u)&&p.application->entityAuthority_.canRetire(e);
}
bool OriginalResults10450LA::detachedRetirementCurrent(const OriginalRaceConstructor50LA& race)const noexcept {
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_||
       impl_->nativeCaptureThread!=std::this_thread::get_id())return false;
    const auto& p=*impl_;const auto& e=p.component;
    if(!p.retirementStarted||p.closed||!p.counted||p.nativeFrameBorrows||
       race.results104_.get()!=this||race.application_!=p.application||!race.counted_||race.rolledBack_||
       race.entity_.sessionGeneration!=e.sessionGeneration||race.pause100_||race.nativeRootAttached_||
       race.entity_.parent1C||race.entity_.previous10||race.entity_.next14||
       !p.application||!p.application->application10()||p.application->attachmentLock423F58()||
       !p.application->objectCount423F60()||e.reference00||
       e.kind!=OriginalWreckComponentKind50LA::entity141300||!e.sessionGeneration||
       e.objectKey!=std::uint64_t(reinterpret_cast<std::uintptr_t>(&e))||
       e.parent1C||e.previous10||e.next14||e.firstChild18||e.descendants24||
       e.category28!=0xff||e.mask20!=p.mask||(e.flags29&15u)!=(p.disabled?0u:3u)||
       contains(p.application->entityAuthority_.view(),&e))return false;
    return !p.rendererLease||p.rendererLease->identityCurrent();
}
void OriginalResults10450LA::Impl::closeTexture(Texture& t)noexcept {
    t.buffer58.reset(); //2DF038: source buffer free BEFORE primitive/material release.
    if(t.primitive54){references.release299AC0(t.primitive54->reference);t.primitive54.reset();}
    t.atlas=nullptr; //Native invalidation, not a source store.
}
void OriginalResults10450LA::Impl::closePolygon(std::unique_ptr<Polygon>& p)noexcept {
    if(!p)return;
    if(p->primitives30)for(std::size_t i=0;i<p->count34;++i)if(p->primitives30[i]){
        references.release299AC0(p->primitives30[i]->reference);p->primitives30[i].reset();
    }
    //2E1A28 frees the primitive pointer allocation before packet backing,
    //then2E1A94 retires vertices. Do not rely on reverse C++ member order.
    p->primitives30.reset();p->packets44.reset();p->vertices00.reset();p.reset();
}
bool OriginalResults10450LA::Impl::closeResources(D& d)noexcept {
    nativeDisplay.reset(); //Copied CPU pixels/vertices are not source/GPU retirement authority.
    displayCpu.reset(); //Invalidate native CPU borrows before owned resources.
    nameEntryCpu.reset();nameEntry.reset(); //Private known fields, no renderer receipt.
    modeCpu.reset();dirtyLayout208.reset(); //Retired private state, not a source clear208.
    updateCpu.reset();pulse394.reset();direction398.reset(); //Native retirement, not source phase reset.
    // Preserve first-activation provenance as history; the closed owner cannot
    // activate again or reinterpret retired words as a fresh visual start.
    loadedRosterOperation.reset(); //Weak provenance only; no RA owner release.
    //Full1BEC40 order. Cursor advances only AFTER each actual free; a failed
    //renderer24 resumes that leaf, not its already-completed renderer20(1).
    const auto drain=[&](std::uint32_t a,std::uint32_t b){
        return !rendererLease||rendererLease->drainResource(a,b,d);
    };
    const auto freed=[&](){if(rendererLease)rendererLease->resourceFreed();};
    const auto texts=[&](auto& a){if(a){for(auto n=a->size();n;--n)closeText((*a)[n-1]);a.reset();}};
    const auto textures=[&](auto& a){
        if(!a)return true;
        if(retirementRemaining==std::numeric_limits<std::size_t>::max())retirementRemaining=a->size();
        while(retirementRemaining){
            if(!drain(0x2df018,0x2df030))return false;
            closeTexture((*a)[retirementRemaining-1]);freed();--retirementRemaining;
        }
        a.reset();return true;
    };
    while(retirementStep<22){
        switch(retirementStep){
        case 0: if(published5E0){for(auto n=rowsCount;n;--n)closeText(rows5E0[n-1]);rows5E0.reset();}break;
        case 1:
            if(published480&&portraits480){
                if(retirementRemaining==std::numeric_limits<std::size_t>::max())retirementRemaining=portraitsCount;
                while(retirementRemaining){
                    if(!drain(0x2df018,0x2df030))return false;
                    closeTexture(portraits480[retirementRemaining-1]);freed();--retirementRemaining;
                }
                portraits480.reset();
            }break;
        case 2: if(publishedA98&&cursorA98){closeText(*cursorA98);cursorA98.reset();}break;
        case 3: if(publishedA90&&nameA90){closeText(*nameA90);nameA90.reset();}break;
        case 4: if(publishedA94){for(std::size_t i=0;i<keysConstructed;++i){closeText(*keysA94[i]);keysA94[i].reset();}keysA94.reset();}break;
        case 5: texts(text1770);break;
        case 6: texts(text15B0);break;
        case 7: texts(statistics1230);break;
        case 8: texts(text1150);break;
        case 9: if(!textures(separators1030))return false;break;
        case 10: texts(tableAF0);break;
        case 11:
            for(auto* t:{&textA20,&text9B0,&text940,&text8D0,&text860,&text7F0,&text780,&text710})
                if(*t){closeText(**t);t->reset();}
            break;
        case 12: if(!textures(medals5F0))return false;break;
        case 13: texts(titles490);break;
        case 14: texts(labels3A0);break;
        case 15: if(!textures(icons210))return false;break;
        //1B8730: polygon38 then3C; text180;100,A0,40.
        case 16: case 17:
            if(polygons[retirementStep-16]){
                if(!drain(0x2e19b4,0x2e19cc))return false;
                closePolygon(polygons[retirementStep-16]);freed();
            }break;
        case 18: if(base180){closeText(*base180);base180.reset();}break;
        case 19: if(!textures(backdropsA0))return false;break;
        case 20: if(center40){
            if(!drain(0x2df018,0x2df030))return false;
            closeTexture(*center40);freed();center40.reset();
            }break;
        case 21:
            if(rendererLease&&!rendererLease->releaseAfterResources(d))return false;
            rendererLease.reset();fonts.reset();sourceData.reset();context.reset();domain.reset();break;
        }
        ++retirementStep;retirementRemaining=std::numeric_limits<std::size_t>::max();
    }
    return true;
}
bool OriginalResults10450LA::closeDetached(OriginalResults104Diagnostic50LA& d)noexcept {
    d={};if(!impl_)return true;auto& p=*impl_;
    if(p.nativeCaptureThread!=std::this_thread::get_id())return p.stop(d,Impl::S::owner,0x1bec40,"Results retirement owner thread");
    if(p.closed)return true;
    if(p.nativeFrameBorrows)return p.stop(d,Impl::S::owner,0x1bec40,"Results actual native Frame remains borrowed");
    if(!p.retirementStarted){
      if(p.counted&&!p.detached())return p.stop(d,Impl::S::owner,0x1bec40,"Results still attached or externally altered");
      if(p.counted){
        if(!p.application->objectCount423F60()||!p.application->entityAuthority_.retire(p.component))
            return p.stop(d,Impl::S::owner,0x104970,"Results Entity lifetime authority");
        --p.component.reference00; //Source enclosing reference release precedes destructor.
      }
      p.retirementStarted=true;
    }else if(p.counted&&(!p.application||!p.application->application10()||p.application->attachmentLock423F58()||
        p.component.reference00||p.component.parent1C||p.component.previous10||p.component.next14||
        p.component.firstChild18||p.component.descendants24||!p.application->objectCount423F60())){
        return p.stop(d,Impl::S::owner,0x1bec40,"Results detached in-progress retirement changed");
    }
    if(!p.closeResources(d))return false;
    if(p.counted)--p.application->objectCount423F60(); //1B8730→1413E0.
    p.loadedResourceTraversal=p.constructed&&p.loadComplete;
    p.closed=true;return true;
}
}

namespace original_main_entry50KG {
namespace {
using ResultsD=rr::game::OriginalResults104Diagnostic50LA;
bool resultsLeaseReject(ResultsD& d,std::uint32_t pc,const char* why)noexcept {
    d.status=ResultsD::Status::owner;d.sourcePC=pc;d.operation=why;return false;
}
}
ResultsRendererLease50LA::ResultsRendererLease50LA(std::shared_ptr<RaceDisplayLifetime50LA> display,
    std::shared_ptr<NativeCollectorFifo50LA> fifo,const Application& issuer,
    rr::game::OriginalResults10450LA& source)noexcept
    :display_(std::move(display)),fifo_(std::move(fifo)),issuer_(&issuer),source_(&source),
     displayGeneration_(display_->generation_){}
ResultsRendererLease50LA::~ResultsRendererLease50LA()noexcept {if(pinned_)std::terminate();}
bool Application::resultsRendererFifoCurrent50LA(const NativeCollectorFifo50LA& fifo)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    return nativeCollectorFifoOwner_.get()==&fifo&&fifo.application_==this&&
        fifo.ownerThread_==std::this_thread::get_id()&&!fifo.closed_&&!fifo.draining_&&
        !fifo.uiConstructor1B73BC_&&fifo.records_.empty()&&standardMaterialDomainOwner_&&
        fifo.table_==standardMaterialDomainOwner_->materials382B38();
}
bool ResultsRendererLease50LA::identityCurrent()const noexcept {
    if(!pinned_||!display_||!fifo_||!issuer_||!source_||!display_->ownerThread()||
       display_->application_!=issuer_||display_->generation_!=displayGeneration_||
       !display_->resultsPinnedDisplayIdentity50LA())return false;
    std::uint32_t ignored=0;
    if(!display_->applicationLifetime_.readPause11C(ignored))return false;
    const auto issued=issuer_->raceDisplayOwner50LA_.lock();
    return issued==display_&&issuer_->raceDisplayGeneration50LA_==displayGeneration_&&
        !issued.owner_before(display_)&&!display_.owner_before(issued)&&
        issuer_->videoOwner_==display_->video_&&
        display_->applicationLifetime_.sameOwner(issuer_->animationClock())&&
        issuer_->resultsRendererFifoCurrent50LA(*fifo_);
}
bool ResultsRendererLease50LA::drainResource(std::uint32_t call20,std::uint32_t call24,ResultsD& d)noexcept {
    if(!identityCurrent()||!source_->impl_||source_->impl_->rendererLease.get()!=this||
       !source_->impl_->retirementStarted||source_->impl_->nativeFrameBorrows||source_->impl_->component.reference00)
        return resultsLeaseReject(d,call20,"Results exact post104970 resource custody absent");
    const auto& dispatch=display_->dispatch_;
    if(dispatch.pendingCallbackDC||dispatch.count54||dispatch.count60)
        return resultsLeaseReject(d,drainPhase_?call24:call20,"Results source renderer queue/callback not represented");
    rr::platform::VehicleWindowDiagnostic50DS renderer;
    if(!drainPhase_){
        std::int32_t actualV0=0;
        const auto r=rr::game::serviceOriginalRenderer244D60(dispatch,true,display_->renderer_,renderer,
            actualV0,rr::game::OriginalCameraDrainPolicy::initializedContext);
        if(r.status!=rr::game::OriginalCameraStartStatus::complete)
            return resultsLeaseReject(d,r.sourcePC,"Results actual renderer20(1) incomplete");
        drainPhase_=1;
    }
    if(drainPhase_==1){
        if(!display_->renderer_.finishInitializedContextWork(renderer)||!renderer.contextWorkCompleted)
            return resultsLeaseReject(d,call24,"Results actual unconditional renderer24 incomplete");
        drainPhase_=2;
    }
    return true;
}
void ResultsRendererLease50LA::resourceFreed()noexcept {
    if(drainPhase_!=2)std::terminate();
    drainPhase_=0;
}
bool ResultsRendererLease50LA::releaseAfterResources(ResultsD& d)noexcept {
    if(!identityCurrent()||drainPhase_||!source_->impl_||source_->impl_->rendererLease.get()!=this||
       !source_->impl_->retirementStarted||source_->impl_->nativeFrameBorrows||
       source_->impl_->retirementStep!=21||source_->impl_->component.reference00)
        return resultsLeaseReject(d,0x1b8730,"Results retained post-resource retirement identity");
    rr::platform::VehicleWindowDiagnostic50DS renderer;
    if(!display_->renderer_.retireNativeResultsResources50LA(renderer))
        return resultsLeaseReject(d,0x1b8730,"Results native owned image/scratch cleanup incomplete");
    display_->unpin();pinned_=false;display_.reset();fifo_.reset();source_=nullptr;return true;
}
bool Application::retainConsumedResultsRenderer50LA(ResultsD& d)const noexcept {
    d={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return resultsLeaseReject(d,0x1bec40,"Results renderer Application thread");
    const auto race=sourceRace28COwner_;const auto display=raceDisplayOwner50LA_.lock();
    const auto fifo=nativeCollectorFifoOwner_;
    if(!race||race->resultsFeedbackThread_!=std::this_thread::get_id()||race->application_!=entitiesOwner_||
       !race->counted_||race->rolledBack_||!race->world74_||!race->results104_||
       !display||race->loadDisplay_!=display||display->application_!=this||
       display->generation_!=raceDisplayGeneration50LA_||!display->gameplayDisplayIdentityCurrent50LA()||
       !display->canRetain()||!fifo||race->loadFifo_!=fifo||!resultsRendererFifoCurrent50LA(*fifo))
        return resultsLeaseReject(d,0x1bec40,"Results actual counted Race/Display/FIFO ownership absent");
    if(!resultsPublishedRoot50LA(*race,d,false))return false;
    auto& source=*race->results104_;
    if(!source.impl_)return resultsLeaseReject(d,0x1bec40,"Results source storage absent");
    auto& p=*source.impl_;
    if(p.nativeCaptureThread!=std::this_thread::get_id()||!source.constructed()||!p.loadComplete||
       !source.retirementCurrent()||p.retirementStarted||p.rendererLease||p.nativeFrameBorrows||
       !source.belongsTo(entitiesOwner_,race->entity_.sessionGeneration)||p.domain!=standardMaterialDomainOwner_||
       !p.context||!p.context->live()||p.context->videoIdentity()!=videoOwner_.get())
        return resultsLeaseReject(d,0x1bec40,"Results actual unbound loaded source resources absent");
    try {
        auto lease=std::shared_ptr<ResultsRendererLease50LA>(new ResultsRendererLease50LA(display,fifo,*this,source));
        if(!display->canRetain())return resultsLeaseReject(d,0x1bec40,"Results Display changed before native pin");
        display->pin();lease->pinned_=true;p.rendererLease=std::move(lease);return true;
    }catch(...){return resultsLeaseReject(d,0x1bec40,"Results renderer custody allocation failed");}
}
}
