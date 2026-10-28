#include "original_race_f8_renderer50la.h"
#include "original_native_collector_fifo50la.h"
#include "original_race_constructor50la.h"
#include "ra_cold_aggregate50la.h"
#include "../../src/game/original_race_f8_storage50la.h"
#include <exception>
#include <thread>
#include <utility>

namespace original_main_entry50KG {
namespace {
using D=rr::game::OriginalRaceF8Diagnostic50LA;
using S=rr::game::OriginalCameraStartStatus;
bool reject(D& d,std::uint32_t pc,const char* operation)noexcept {
    d.sourcePC=pc;d.operation=operation;return false;
}
}
RaceF8RendererLease50LA::RaceF8RendererLease50LA(std::shared_ptr<RaceDisplayLifetime50LA> display,
    std::shared_ptr<NativeCollectorFifo50LA> fifo,const Application& issuer,
    rr::game::OriginalRaceF850LA& source)noexcept
    :display_(std::move(display)),fifo_(std::move(fifo)),issuer_(&issuer),source_(&source),
     displayGeneration_(display_->generation_){}
RaceF8RendererLease50LA::~RaceF8RendererLease50LA()noexcept {
    if(pinned_)std::terminate(); //Never discard undrained resource custody.
}
bool Application::f8RendererFifoCurrent50LA(const NativeCollectorFifo50LA& fifo)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    return nativeCollectorFifoOwner_.get()==&fifo&&fifo.application_==this&&
        fifo.ownerThread_==std::this_thread::get_id()&&!fifo.closed_&&!fifo.draining_&&
        !fifo.uiConstructor1B73BC_&&fifo.records_.empty()&&standardMaterialDomainOwner_&&
        fifo.table_==standardMaterialDomainOwner_->materials382B38();
}
bool RaceF8RendererLease50LA::identityCurrent()const noexcept {
    if(!pinned_||!display_||!fifo_||!issuer_||!source_||!display_->ownerThread()||
       display_->application_!=issuer_||display_->generation_!=displayGeneration_||
       !display_->f8PinnedDisplayIdentity50LA())return false;
    std::uint32_t ignoredPause=0;
    if(!display_->applicationLifetime_.readPause11C(ignoredPause))return false;
    const auto issued=issuer_->raceDisplayOwner50LA_.lock();
    return issued==display_&&issuer_->raceDisplayGeneration50LA_==displayGeneration_&&
        !issued.owner_before(display_)&&!display_.owner_before(issued)&&
        issuer_->videoOwner_==display_->video_&&
        display_->applicationLifetime_.sameOwner(issuer_->animationClock())&&
        issuer_->f8RendererFifoCurrent50LA(*fifo_);
}
bool RaceF8RendererLease50LA::serviceBeforeSlot48(D& d)noexcept {
    d={};
    if(!identityCurrent()||display_->retiring_||retirementCursor_||drainPhase_||
       issuer_->sourceApplicationMode22C_!=5||!source_->loadedCold()||
       !source_->impl_->currentLeaf()||source_->impl_->retirementStarted||
       (source_->impl_->component.flags29&9u)!=1u||!(source_->impl_->component.mask20&8u))
        return reject(d,0x141dc8,"actual F8/Display renderer20(0) prerequisites");
    rr::platform::VehicleWindowDiagnostic50DS renderer;std::int32_t actualV0=0;
    const auto service=rr::game::serviceOriginalRenderer244D60(
        display_->dispatch_,false,display_->renderer_,renderer,actualV0);
    if(service.status!=S::complete)return reject(d,service.sourcePC,"actual renderer20(0) remains incomplete");
    //244D60 return0 is no queue submitted. Never turn this into an F8 draw
    //acknowledgment, source48 completion, or a gameplay render-invocation token.
    d.sourcePC=0x141de0;return true;
}
bool RaceF8RendererLease50LA::drainResource(std::uint32_t call20,std::uint32_t call24,D& d)noexcept {
    if(!identityCurrent())return reject(d,call20,"F8 retained Display/FIFO resource lifetime");
    // A host finish cannot retire unrepresented source queues/callbacks or
    // semantic FIFO records. Observe those SAME owners; no supplied receipt.
    const auto& dispatch=display_->dispatch_;
    if(dispatch.pendingCallbackDC||dispatch.count54||dispatch.count60)
        return reject(d,drainPhase_?call24:call20,"F8 original renderer queue/callback retirement unavailable");
    rr::platform::VehicleWindowDiagnostic50DS renderer;
    if(!drainPhase_){
        std::int32_t actualV0=0;
        const auto service=rr::game::serviceOriginalRenderer244D60(dispatch,true,
            display_->renderer_,renderer,actualV0,rr::game::OriginalCameraDrainPolicy::initializedContext);
        if(service.status!=S::complete)return reject(d,service.sourcePC,"F8 actual renderer20(1) incomplete");
        drainPhase_=1;
    }
    if(drainPhase_==1){
        // Unconditional virtual24=245090, including disabled-D8 and unloaded
        // ScreenTexture objects. Do not call2450A0 (which also changes D8).
        if(!display_->renderer_.finishInitializedContextWork(renderer)||!renderer.contextWorkCompleted)
            return reject(d,call24,"F8 actual renderer24 completion unavailable");
        drainPhase_=2;
    }
    return true;
}
bool RaceF8RendererLease50LA::closeResources198510(rr::game::OriginalRaceF850LA& source,D& d)noexcept {
    if(&source!=source_||!source.impl_||source.impl_->rendererLease.get()!=this||
       !source.impl_->retirementStarted||source.impl_->component.reference00||!identityCurrent())
        return reject(d,0x198510,"F8 same detached post104970 renderer custody");
    auto& p=*source.impl_;
    // Native resumable cursor follows original present30,34,38,3C, then
    // embedded text170,100, then textureA0,40. No caller acknowledgment API.
    while(retirementCursor_<4){
        auto& polygon=p.polygons30[retirementCursor_];d.object=0x30+4*retirementCursor_;
        if(polygon){
            if(polygon->nativePacketPins)return reject(d,0x2e1980,"F8 actual packet borrow remains live");
            if(!drainResource(0x2e19b4,0x2e19cc,d))return false;
            if(polygon->primitives30)for(std::size_t n=0;n<polygon->count34;++n)
                if(polygon->primitives30[n]){
                    p.release(polygon->primitives30[n]->reference);polygon->primitives30[n].reset();
                }
            polygon->primitives30.reset();polygon->packets44.reset();
            polygon->vertices00.reset();polygon.reset();d.sourceEffectsCommitted=true;
        }
        ++retirementCursor_;drainPhase_=0;
    }
    if(retirementCursor_==4){
        if(p.texts100){for(std::size_t n=2;n;--n)p.closeText((*p.texts100)[n-1]);p.texts100.reset();}
        ++retirementCursor_;d.sourceEffectsCommitted=true;
    }
    while(retirementCursor_<7){
        const std::size_t index=6-retirementCursor_;d.object=0x40+0x60*index;
        if(p.textures40){
            if(!drainResource(0x2df018,0x2df030,d))return false;
            p.closeTexture((*p.textures40)[index]);d.sourceEffectsCommitted=true;
        }
        ++retirementCursor_;drainPhase_=0;
    }
    p.textures40.reset();p.nativeImageCache.reset();p.fonts.reset();p.sourceData.reset();p.context.reset();p.domain.reset();
    // Real resource releases have finished; release only this native pin.
    display_->unpin();pinned_=false;display_.reset();fifo_.reset();source_=nullptr;
    d.sourcePC=0x1985e0;return true;
}
bool Application::retainConsumedF8Renderer50LA(D& d)const noexcept {
    d={};
    if(garageAudioOwnerThread_!=std::this_thread::get_id())
        return reject(d,0x141dc8,"F8 renderer custody requires the Application owner thread");
    const auto race=sourceRace28COwner_;const auto display=raceDisplayOwner50LA_.lock();
    const auto fifo=nativeCollectorFifoOwner_;
    if(!race||race->application_!=entitiesOwner_||!race->counted_||race->rolledBack_||
       !race->world74_||!race->f8Attached_||!race->f8Owner_||race->f8Source_!=race->f8Owner_.get()||
       !display||race->loadDisplay_!=display||display->application_!=this||
       display->generation_!=raceDisplayGeneration50LA_||!display->gameplayDisplayIdentityCurrent50LA()||
       !display->canRetain()||!fifo||race->loadFifo_!=fifo||!f8RendererFifoCurrent50LA(*fifo))
        return reject(d,0x141dc8,"F8 actual consumed Race/Display/FIFO required for native custody");
    auto& f8=*race->f8Owner_;auto& p=*f8.impl_;
    if(!f8.loadedCold()||!p.currentLeaf()||p.retirementStarted||p.rendererLease||
       !f8.belongsTo(entitiesOwner_,race->entity_.sessionGeneration))
        return reject(d,0x198510,"F8 actual unbound loaded resource owner");
    for(std::size_t i=0;i<4;++i)if(!p.polygonCurrent(i)||p.polygons30[i]->nativePacketPins)
        return reject(d,0x2e1980,"F8 actual unborrowed polygon resources");
    const auto world=race->world74_;ra_cold_aggregate50LA::Diagnostic wd;
    const auto camera=world->borrowRuntimeSourceCamera(world,wd);
    std::array<std::int32_t,4> rectangle{};std::uint64_t revision=0;
    if(!camera||camera->generation_!=race->entity_.sessionGeneration||
       camera->actual_.applicationEntities!=entitiesOwner_||camera->actual_.humanCount34!=1||
       !camera->nativeTaIssuance_||camera->nativeTaIssuance_->issuer!=this||
       camera->nativeTaIssuance_->generation!=camera->generation_||
       camera->nativeTaIssuance_->course!=rr::game::OriginalCameraStartCourse::ra||
       !display->readGameplayViewport115990(*camera,rectangle,revision))
        return reject(d,0x26f940,"F8 real selected RA Camera required for native resource custody");
    try {
        // Allocate the control block BEFORE pinning. Allocation failure cannot
        // strand a Display pin or trigger a pretend resource retirement.
        auto lease=std::shared_ptr<RaceF8RendererLease50LA>(new RaceF8RendererLease50LA(display,fifo,*this,f8));
        if(!display->canRetain())return reject(d,0x141dc8,"F8 Display changed before native pin");
        display->pin();lease->pinned_=true;p.rendererLease=std::move(lease);
        return true;
    }catch(...){return reject(d,0x198510,"F8 native renderer custody allocation failed");}
}
}
namespace rr::game {
bool OriginalRaceF850LA::retireRendererResources198510(OriginalRaceF8Diagnostic50LA& d)noexcept {
    if(!impl_||!impl_->rendererLease)return false;
    const auto lease=impl_->rendererLease;
    if(!lease->closeResources198510(*this,d))return false;
    impl_->rendererLease.reset();return true;
}
}
