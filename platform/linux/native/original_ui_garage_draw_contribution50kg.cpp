#include "../../../src/game/original_options_mutation_interlock50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_manager_fog50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <bit>
#include <cmath>
#include <cstdio>
#include <exception>
#include <limits>
#include "../../../src/assets/original_standard_texture_lifetime50la.h"
#include "../../../src/assets/original_standard_archive_provider50la.h"
namespace rr::platform {namespace {
template<std::size_t N>std::array<float,N> floats(const std::array<std::uint32_t,N>& words)noexcept {
    std::array<float,N> out{};for(std::size_t i=0;i<N;++i)out[i]=std::bit_cast<float>(words[i]);return out;
}
template<std::size_t N>std::array<std::uint32_t,N> words(const std::array<float,N>& values)noexcept {
    std::array<std::uint32_t,N> out{};for(std::size_t i=0;i<N;++i)out[i]=std::bit_cast<std::uint32_t>(values[i]);return out;
}
}
OriginalUiGarageDrawContribution50KG::OriginalUiGarageDrawContribution50KG(const Application& root,
    const std::shared_ptr<OriginalUiGarageSession50KG>& s,std::shared_ptr<OriginalUiGarageRendererLease50KG> renderer,
    ::original_ui_source_timing50LH::PublicationView timing,std::uint64_t serial,C* manager)noexcept
    :issuer_(&root),session_(s),page_(s->page_),renderer_(std::move(renderer)),timing_(std::move(timing)),
     publication_(serial),manager_(manager),dispatcher_(s->managerDispatcher2AC_),camera_(page_->sharedCamera_){}
OriginalUiGarageDrawContribution50KG::~OriginalUiGarageDrawContribution50KG()noexcept {
    //No incomplete frame is ever published. A future sealed owner must first
    //discharge the renderer's actual SourceOwner lifetime before destruction.
    if(shared47COwner_||shared47CEpoch_||shared470FirstPassOwner_||shared470FirstPassEpoch_||shared470ThirdPassOwner_||shared470ThirdPassEpoch_||frame_.owner_||textureCoordinator_||(drainGpu_&&drainGpu_->ownsResources())||
       (renderer_&&renderer_->ownsRenderPrelude(*this,publication_)))std::terminate();
}
bool OriginalUiGarageDrawContribution50KG::identityCurrent()const noexcept {
    const auto s=session_.lock();::original_ui_source_timing50LH::Sample sample;std::uint64_t serial=0;
    return nativeRetirementPhase_==NativeRetirementPhase::notStarted&&s&&renderer_&&renderer_->currentFor(issuer_)&&s->issuer_==issuer_&&
        s->renderCustodyCurrent140230(publication_)&&s->page_==page_&&
        timing_.sameOwner(s->entryTiming_)&&timing_.readPrepared(sample,serial)&&serial==publication_&&
        page_&&camera_&&page_->sharedCamera_==camera_&&camera_->nativeRegistrationIdentityCurrent()&&
        camera_->phase_==Camera::Phase::pageAttached1FB620&&camera_->entities_.get()==issuer_->entities().get();
}
bool OriginalUiGarageDrawContribution50KG::fail(Diagnostic& d,std::uint32_t pc)noexcept {
    sourcePC_=pc;phase_=Phase::unavailable;d.sourcePC=pc;return false;
}
bool OriginalUiGarageDrawContribution50KG::inheritedAuralRenderReceiverCurrent(const C& node)const noexcept {
    if(!issuer_)return false;
    const auto entities=issuer_->entities();const auto& owner=issuer_->sourceAural00Owner_;
    if(!entities||entities->aural00_!=&node||!owner||entities->auralNativeOwner_!=owner.get())return false;
    const auto observed=owner->inspect();
    // Actual Application00, not every same-kind Entity. current() preserves
    // generation/key/ref/forest and live list ownership after real103720;
    // no cold audio/zero-history or completed-frame predicate is substituted.
    return observed.entity==&node&&observed.attached&&!observed.retired&&observed.nativeInitialized&&
        node.parent1C==entities->application10()&&node.kind==game::OriginalWreckComponentKind50LA::auralScape102BE0&&
        node.mask20==0xffffffffu&&(node.flags29&9u)==1u&&owner->current();
}
bool OriginalUiGarageDrawContribution50KG::shared470FirstPassReceiverCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
 const auto session=session_.lock();const auto& node=shared.dialog470_.entity;
 return phase_==Phase::firstPass&&identityCurrent()&&issuer_&&issuer_->state&&
  issuer_->sourceApplicationMode22C_==3&&issuer_->state->sharedDialogs.get()==&shared&&
  callback_==&node&&session&&shared.application_.lock()==issuer_->state&&shared.inputs_.root==issuer_&&
  shared.scope_==session->scope_&&shared.loadedCurrent()&&!shared.reorderStep_&&!shared.reorderingPage_&&
  shared.completedReorders_&&shared.reorderedPage_==page_.get()&&shared.reorderedGrant_.lock()==session->grant_&&
  shared.dialog470_.manager30==dispatcher_&&node.parent1C==dispatcher_&&
  node.kind==game::OriginalWreckComponentKind50LA::entity141300&&node.reference00==1&&
  node.mask20==0xffffffffu&&(node.flags29&9u)==1u&&!node.firstChild18&&
  shared.dialog470_.vtable==0x3D0910u&&shared.controller47C_.vtable==0x3D0890u;
}
bool OriginalUiGarageDrawContribution50KG::shared470FailedUnwindCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
 const auto session=session_.lock();
 return nativeRetirementPhase_==NativeRetirementPhase::preludeReleased&&nativeRetirementOwnerCurrent()&&
  phase_==Phase::unavailable&&attempted_&&issuer_&&issuer_->state&&
  issuer_->state->sharedDialogs.get()==&shared&&shared.inputs_.root==issuer_&&shared.loadedCurrent()&&
  callback_==&shared.dialog470_.entity&&shared470FirstPassOwner_.get()==&shared&&
  session&&session->unsealedRenderFailureHistoryCurrent(*issuer_)&&
  !frame_.owner_&&!frame_.native_&&!drainBatchPending_&&!drainBatchFailed_&&
  !textureCoordinator_&&!drainGpu_&&!drainImageBank_&&!drainBatchCanonical_&&
  renderer_&&!renderer_->ownsRenderPrelude(*this,publication_);
}
bool OriginalUiGarageDrawContribution50KG::shared470ThirdPassReceiverCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
 const auto session=session_.lock();const auto& node=shared.dialog470_.entity;
 return !activeText_&&!activePageQuad_&&!activePageCostText_&&phase_==Phase::thirdPass&&identityCurrent()&&issuer_&&issuer_->state&&
  issuer_->sourceApplicationMode22C_==5&&issuer_->state->sharedDialogs.get()==&shared&&
  callback_==&node&&session&&shared.application_.lock()==issuer_->state&&shared.inputs_.root==issuer_&&
  shared.scope_==session->scope_&&shared.loadedCurrent()&&!shared.reorderStep_&&!shared.reorderingPage_&&
  shared.completedReorders_&&shared.reorderedPage_==page_.get()&&shared.reorderedGrant_.lock()==session->grant_&&
  shared.dialog470_.manager30==dispatcher_&&node.parent1C==dispatcher_&&
  node.kind==game::OriginalWreckComponentKind50LA::entity141300&&node.reference00==1&&
  node.mask20==0xffffffffu&&(node.flags29&9u)==1u&&!node.firstChild18&&
  shared.dialog470_.vtable==0x3D0910u&&shared.controller47C_.vtable==0x3D0890u;
}
bool OriginalUiGarageDrawContribution50KG::shared470ThirdPassFailedUnwindCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
 const auto session=session_.lock();
 return nativeRetirementPhase_==NativeRetirementPhase::preludeReleased&&nativeRetirementOwnerCurrent()&&
  phase_==Phase::unavailable&&attempted_&&issuer_&&issuer_->state&&
  issuer_->state->sharedDialogs.get()==&shared&&shared.inputs_.root==issuer_&&shared.loadedCurrent()&&
  callback_==&shared.dialog470_.entity&&shared470ThirdPassOwner_.get()==&shared&&
  session&&session->unsealedRenderFailureHistoryCurrent(*issuer_)&&
  !frame_.owner_&&!frame_.native_&&!drainBatchPending_&&!drainBatchFailed_&&
  !textureCoordinator_&&!drainGpu_&&!drainImageBank_&&!drainBatchCanonical_&&
  renderer_&&!renderer_->ownsRenderPrelude(*this,publication_);
}
bool OriginalUiGarageDrawContribution50KG::invokeSharedDialogRender(
    C& node,std::uint32_t slot,bool& matched,Diagnostic& d)noexcept {
    matched=false;
    if(!issuer_||!issuer_->state||!issuer_->state->sharedDialogs)return true;
    const auto shared=issuer_->state->sharedDialogs;
    const bool controller=&node==&shared->controller47C_.entity;
    if(!controller && &node!=&shared->dialog470_.entity)return true;
    matched=true;
    const auto session=session_.lock();
    const bool exactPhase=(slot==0x40&&phase_==Phase::firstPass)||
        (slot==0x44&&phase_==Phase::secondPass)||(slot==0x48&&phase_==Phase::thirdPass)||
        (slot==0x4c&&phase_==Phase::finalPass);
    if(!exactPhase||!identityCurrent()||callback_!=&node||!session||
       shared->application_.lock()!=issuer_->state||shared->inputs_.root!=issuer_||
       shared->scope_!=session->scope_||!shared->loadedCurrent()||shared->reorderStep_||
       shared->reorderingPage_||!shared->completedReorders_||shared->reorderedPage_!=page_.get()||
       shared->reorderedGrant_.lock()!=session->grant_||
       shared->dialog470_.manager30!=dispatcher_||node.parent1C!=dispatcher_||
       node.kind!=game::OriginalWreckComponentKind50LA::entity141300||node.reference00!=1||
       node.mask20!=0xffffffffu||(node.flags29&9u)!=1u||node.firstChild18||
       shared->dialog470_.vtable!=0x3D0910u||shared->controller47C_.vtable!=0x3D0890u)
        return fail(d,controller?0x206430u:0x204A00u);
    if(slot==0x40||slot==0x48){
        sourcePC_=controller?(slot==0x40?0x206430u:0x2064C0u):
            (slot==0x40?0x204A00u:0x204AC0u);
        if(controller){
            if(issuer_->state->controllerPopupState.active3CC){
                OriginalUiSharedDialogs50KG::Diagnostic sd;
                if(!shared->begin47CCallback(*this,slot,sd))return fail(d,sd.sourcePC);
                if(slot==0x40){
                    if(!shared->demand47C206430(*this,sd))return fail(d,sd.sourcePC);
                    sourcePC_=0x2064A0;if(!walk141C30(node,d))return false;
                }else{
                    if(!shared->draw47C2064C0(*this,sd))return fail(d,sd.sourcePC);
                    sourcePC_=0x207310;if(!walk141D70(node,d))return false;
                }
                if(!shared->complete47CCallback(*this,slot,sd))return fail(d,sd.sourcePC);
                return true;
            }
        }else{
            OriginalUiSharedDialogs50KG::Diagnostic sharedDiagnostic;
            if(slot==0x40){
                // Exact dispatcher470 receiver, independent of Page Quad/Text
                // authority. This real callback owns one distinct native epoch.
                if(!shared->begin470FirstPass(*this,sharedDiagnostic)||
                   !shared->continue470FirstPass(*this,sharedDiagnostic))
                    return fail(d,sharedDiagnostic.sourcePC);
                sourcePC_=0x204A9C;
                if(!walk141C30(node,d))return false;
                shared470InheritedReturned_=true;
                if(!shared->complete470FirstPass(*this,sharedDiagnostic))
                    return fail(d,sharedDiagnostic.sourcePC);
                sourcePC_=0x204AB8;return true;
            }
            // HARD ADMISSION BOUNDARY: canonical470 accepted histories are
            // not yet bridged bidirectionally to Main/Options. A primary
            // disconnect here must not bypass that boundary merely because
            // Controller2/page3 is disabled. Keep the actual source visibility
            // predicate before epoch issuance, mutation, FIFO or history work.
            // Only a separately authenticated common-history owner may replace
            // this gate; there is no caller readiness flag or public toggle.
            if(!shared->invisible470204AC0(sharedDiagnostic))
                return fail(d,sharedDiagnostic.sourcePC);
            // Actual shared470 +48 callback, never page geometry authority.
            if(!shared->begin470ThirdPass(*this,sharedDiagnostic)||
               !shared->continue470ThirdPass(*this,sharedDiagnostic))
                return fail(d,sharedDiagnostic.sourcePC);
            sourcePC_=0x2054A4;
            if(!walk141D70(node,d))return false;
            shared470ThirdPassInheritedReturned_=true;
            if(!shared->complete470ThirdPass(*this,sharedDiagnostic))
                return fail(d,sharedDiagnostic.sourcePC);
            sourcePC_=0x2054BC;return true;
        }
    }
    //Concrete tables only. The first/third callbacks reach these inherited
    //tails after controller47C's proven inactive branch; +44/+4C inherit directly.
    if(slot==0x40)return walk141C30(node,d);
    if(slot==0x44)return walk141CE0(node,d);
    if(slot==0x48)return walk141D70(node,d);
    return walk141E20(node,d);
}
bool OriginalUiGarageDrawContribution50KG::rejectRenderCallback(Diagnostic& d,std::uint32_t pc,
    const char* reason,const C& node)noexcept {
    const auto entities=issuer_?issuer_->entities():nullptr;
    const char* parentRole=!node.parent1C?"none":
        entities&&node.parent1C==entities->application26C()?"outer26C":
        entities&&node.parent1C==entities->application10()?"root10":
        node.parent1C==dispatcher_?"dispatcher":node.parent1C==manager_?"manager":
        page_&&node.parent1C==&page_->entity_?"page":camera_&&node.parent1C==&camera_->entity_?"camera":"other";
    // Bounded source identifiers only. objectKey is pointer-derived, so it
    // and all raw addresses are deliberately omitted from the diagnostic.
    std::fprintf(stderr,"original_garage_render_callback_rejected source_pc=%u reason=%s phase=%u kind=%u mask=%u flags=%u ref=%llu parent=%s publication=%llu\n",
        pc,reason,static_cast<unsigned>(phase_),static_cast<unsigned>(node.kind),node.mask20,
        static_cast<unsigned>(node.flags29),static_cast<unsigned long long>(node.reference00),parentRole,static_cast<unsigned long long>(publication_));
    return fail(d,pc);
}
bool OriginalUiGarageCameraFrame50KG::current()const noexcept {
    if(!cursor_||cursor_->phase_==OriginalUiGarageDrawContribution50KG::Phase::unavailable||
       !cursor_->identityCurrent())return false;
    const auto& display=*cursor_->renderer_->display_;
    return display.sourceCurrentCamera0Known_&&display.sourceSelectedCamera0_==cursor_->camera_&&
        cursor_->camera_->updateReturned_&&!cursor_->camera_->updateFailed_&&
        cursor_->camera_->updatePublication_==cursor_->publication_;
}
bool OriginalUiGarageCameraFrame50KG::read(Fields& out,std::uint32_t& pc)const noexcept {
    pc=0x114D28;if(!current())return false;
    const auto& f=cursor_->camera_->fields_;
    if(!f.coreKnown||!f.screen140Known||!f.guardProjection290Known||!f.combinedScreen250Known||!f.guardCombined2D0Known)return false;
    Fields next;next.viewC0=floats(f.viewC0);next.projection80=floats(f.projection80);next.screen140=floats(f.matrix140);
    next.combined100=floats(f.combined100);next.combinedScreen250=floats(f.combinedScreen250);
    next.guardProjection290=floats(f.guardProjection290);next.guardCombined2D0=floats(f.guardCombined2D0);
    next.position180=floats(f.position180);next.look190=floats(f.look190);next.up1A0=floats(f.up1A0);
    next.guardX334=std::bit_cast<float>(f.guardX334);next.guardY338=std::bit_cast<float>(f.guardY338);
    next.near1FC=std::bit_cast<float>(f.near1FC);next.far200=std::bit_cast<float>(f.far200);
    next.fov1D0=std::bit_cast<float>(f.fov1D0);next.aspect1F8=std::bit_cast<float>(f.aspect1F8);
    next.focal1D8=std::bit_cast<float>(f.focal1D8);next.width204=f.fullWidth204;next.height208=f.fullHeight208;
    const auto finite=[](const auto& values)noexcept {for(const float value:values)if(!std::isfinite(value))return false;return true;};
    if(!finite(next.viewC0)||!finite(next.projection80)||!finite(next.screen140)||!finite(next.combined100)||
       !finite(next.combinedScreen250)||!finite(next.guardProjection290)||!finite(next.guardCombined2D0)||
       !finite(next.position180)||!finite(next.look190)||!finite(next.up1A0)||
       !std::isfinite(next.guardX334)||!std::isfinite(next.guardY338)||!std::isfinite(next.near1FC)||
       !std::isfinite(next.far200)||!std::isfinite(next.fov1D0)||!std::isfinite(next.aspect1F8)||!std::isfinite(next.focal1D8))return false;
    out=next;pc=0;return true;
}
bool OriginalUiGarageCameraFrame50KG::searchInvocation114D28Current(const OriginalUiGaragePageOwner50KG& page,
    const OriginalUiGarageVisibility50KG& visibility)const noexcept {
    return current()&&cursor_->phase_==OriginalUiGarageDrawContribution50KG::Phase::search114D28&&
        cursor_->sourcePC_==0x114D28&&cursor_->callback_==&cursor_->camera_->entity_&&cursor_->page_.get()==&page&&
        page.renderVisibility114D28().get()==&visibility;
}
bool OriginalUiGarageCameraFrame50KG::readSearchPublication114D28(std::uint64_t& serial)const noexcept {
    if(!current()||cursor_->phase_!=OriginalUiGarageDrawContribution50KG::Phase::search114D28||
       cursor_->sourcePC_!=0x114D28||cursor_->callback_!=&cursor_->camera_->entity_)return false;
    serial=cursor_->publication_;return true;
}
bool OriginalUiGarageCameraFrame50KG::incrementModelCount26F050()const noexcept {
    if(!current()||cursor_->phase_!=OriginalUiGarageDrawContribution50KG::Phase::search114D28||
       cursor_->sourcePC_!=0x114D28||cursor_->callback_!=&cursor_->camera_->entity_)return false;
    ++cursor_->camera_->entities_->processedModels278_;return true; //Exact unsigned source wrap.
}
bool OriginalUiGarageCameraFrame50KG::drainSearchBlock27EC68(VehicleWindowDiagnostic50DS& d,std::uint32_t& pc)const noexcept {
    pc=0x27EC68;
    if(!current()||cursor_->phase_!=OriginalUiGarageDrawContribution50KG::Phase::search114D28||
       cursor_->sourcePC_!=0x114D28||cursor_->callback_!=&cursor_->camera_->entity_)return false;
    game::OriginalCameraStartResult r;
    if(!cursor_->renderer_->serviceEntity141F3C(d,r)){pc=r.sourcePC?r.sourcePC:pc;return false;}
    pc=0x27EC78;return true;
}
bool OriginalUiGarageCameraFrame50KG::renderInvocation114E00Current(const OriginalUiGaragePageOwner50KG& page,
    const OriginalUiGarageVisibility50KG& visibility)const noexcept {
    return current()&&cursor_->phase_==OriginalUiGarageDrawContribution50KG::Phase::modelRender114E00&&
        cursor_->sourcePC_==0x114E00&&cursor_->callback_==&cursor_->camera_->entity_&&cursor_->page_.get()==&page&&
        page.renderVisibility114D28().get()==&visibility;
}
bool OriginalUiGarageCameraFrame50KG::readRenderInputs114E00(ModelRenderInputs& out,
    std::uint64_t& serial,std::uint32_t& pc)const noexcept {
    pc=0x114E00;
    if(!current()||cursor_->phase_!=OriginalUiGarageDrawContribution50KG::Phase::modelRender114E00||
       cursor_->sourcePC_!=0x114E00||cursor_->callback_!=&cursor_->camera_->entity_)return false;
    if(!cursor_->readModelRenderInputs(out,pc))return false;
    serial=cursor_->publication_;return true;
}
bool OriginalUiGarageDrawContribution50KG::readModelRenderInputs(
    OriginalUiGarageCameraFrame50KG::ModelRenderInputs& out,std::uint32_t& pc)const noexcept {
    pc=0x114E00;if(!cameraFrame_.current()||!viewportDepthCleared_||!cameraPhase4Cleared_)return false;
    const auto& display=*renderer_->display_;const auto& f=camera_->fields_;
    if(!display.sourceFogRangeKnown_||!std::isfinite(display.sourceFogStartB0_)||
       !std::isfinite(issuer_->sourceSecondMapDistance382FA0_))return false;
    OriginalUiGarageCameraFrame50KG::ModelRenderInputs next;
    next.viewportX1E0=f.viewport1E0[0];next.viewportY1E4=f.viewport1E0[1];
    next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
    next.fogStartB0=display.sourceFogStartB0_;next.secondMapDistance382FA0=issuer_->sourceSecondMapDistance382FA0_;
    out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::viewport114A00(Diagnostic& d)noexcept {
    if(!cameraFrame_.current()||viewportDepthCleared_)return fail(d,0x114A00);
    auto& display=*renderer_->display_;auto& f=camera_->fields_;
    if(!display.sourceBufferCount0CInitialized_)return fail(d,0x114A18);
    f.count210=display.sourceBufferCount0C_+1u;f.count20C=display.sourceBufferCount0C_+1u;
    game::OriginalCameraStartResult service;
    if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x245900);
    if(!renderer_->complete245090(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x245910);
    if(!cameraFrame_.current()||!f.coreKnown)return fail(d,0x245918);
    game::OriginalCameraViewportFields supplied,current;
    supplied.prepared=current.prepared=true; //Native adapter consumes authenticated actual fields only.
    for(std::size_t i=0;i<4;++i)viewportWords_[i]=f.viewport1E0[i];
    if(viewportWords_[2]>0x7fffffffu||viewportWords_[3]>0x7fffffffu)return fail(d,0x245924);
    supplied.width1E8=static_cast<std::int32_t>(viewportWords_[2]);
    supplied.height1EC=static_cast<std::int32_t>(viewportWords_[3]);
    current.left318=std::bit_cast<float>(f.left318);current.right31C=std::bit_cast<float>(f.right31C);
    current.top320=std::bit_cast<float>(f.top320);current.bottom324=std::bit_cast<float>(f.bottom324);
    const auto zoom=game::updateOriginalViewportZoom2458C0(supplied,current);
    if(zoom.status!=game::OriginalCameraStartStatus::complete)return fail(d,zoom.sourcePC);
    f.zoom328=std::bit_cast<std::uint32_t>(current.zoom328);f.zoom32C=std::bit_cast<std::uint32_t>(current.zoom32C);
    //Only the reached whole source target is represented. No arbitrary host
    //window scaling, split-screen rectangle or constructor-reset shortcut.
    if(viewportWords_!=std::array<std::uint32_t,4>{0,0,640,448}||
       display.sourceWidth4_!=640||display.sourceHeight8_!=448)return fail(d,0x245B14);
    sourceXyOffset_={27648u,29184u}; //((2048-width/2-x)<<4), same forY.
    if(!renderer_->clearViewport2458C0(*this,publication_,false,0,d.renderer))return fail(d,0x245C60);
    viewportDepthCleared_=true;return true;
}
bool OriginalUiGarageDrawContribution50KG::beginRenderer244020(Diagnostic& d)noexcept {
    if(!identityCurrent()||!viewportDepthCleared_||!cameraPhase4Cleared_)return fail(d,0x244020);
    auto& display=*renderer_->display_;auto& dispatch=display.dispatch_;
    dispatch.flagsD8=static_cast<std::uint8_t>(dispatch.flagsD8|1u); //24404C.
    //Real empty native queues have no guest transport addresses to reset.
    //Never discard a pending source queue/callback to emulate244D20.
    if(dispatch.pendingCallbackDC||dispatch.count54||dispatch.count60)return fail(d,0x244048);
    dispatch.count54=0;dispatch.count60=0; //Two distinct244D20 calls, same retained owner.
    if(display.sourcePrimitiveResetCount68_>display.sourcePrimitiveResetList68_.size())return fail(d,0x2461BC);
    for(std::uint32_t i=0;i<display.sourcePrimitiveResetCount68_;++i)display.sourcePrimitiveResetList68_[i]=nullptr;
    display.sourcePrimitiveResetCount68_=0; //Actual2461C4, no resource destroy/ref decrement.
    if(issuer_->sourceDebugOverlay14_)return fail(d,0x2440B4);
    display.sourceRendererCounter64_=display.sourceRendererCounterBC_=display.sourceRendererCounterC0_=0;
    return true;
}
bool OriginalUiGarageDrawContribution50KG::walk141CE0(C& parent,Diagnostic& d)noexcept {
    if(!(parent.descendants24&4u))return true;
    for(C* child=parent.firstChild18;child;){
        if(!identityCurrent()||child->parent1C!=&parent||!child->reference00)return fail(d,0x141D08);
        if((child->flags29&9u)==1u&&(child->mask20&4u)&&!invoke141D40(*child,d))return false;
        child=child->next14;
    }
    return true;
}
bool OriginalUiGarageDrawContribution50KG::invoke141D40(C& node,Diagnostic& d)noexcept {
    using K=game::OriginalWreckComponentKind50LA;callback_=&node;
    if(!identityCurrent())return rejectRenderCallback(d,0x141D40,"identity",node);
    // Aural table3CC900 inherits this exact Entity walker.
    if(inheritedAuralRenderReceiverCurrent(node))return walk141CE0(node,d);
    bool shared=false;if(!invokeSharedDialogRender(node,0x44,shared,d))return false;
    if(shared)return true;
    if(&node==&page_->entity_){
        if(page_->base_.backgroundTexture154)return fail(d,0x2A6EC0);
        return walk141CE0(node,d); //Actual null154 branch, not a skipped background draw.
    }
    if(&node==&camera_->entity_){
        if(camera_->fields_.count20C){
            auto& display=*renderer_->display_;display.sourceClearCounters30_={}; //243FA8..243FBC.
            if(!viewportDepthCleared_||viewportWords_!=std::array<std::uint32_t,4>{0,0,640,448}||
               !display.sourceFogColor28Known_||!display.video_)return fail(d,0x243FDC);
            const bool color=display.sourceClearMode2C_!=0;
            if(!renderer_->clearViewport2458C0(*this,publication_,color,display.sourceFogColor28_,d.renderer))
                return fail(d,0x25C548);
            cameraPhase4Cleared_=true;
        }
        const auto& f=camera_->fields_;
        if((f.near1FC!=0x3f800000u||f.far200!=0x47c35000u)&&!setCameraRange114F20(1.f,100000.f,d))return false;
        return walk141CE0(node,d);
    }
    //Exact recovered+44=141CE0 methods. Other live receivers remain explicit.
    if(&node==issuer_->entities()->application26C()||&node==issuer_->entities()->application10()||
       &node==dispatcher_||&node==manager_||&node==page_->scenePageChildren_[0]||
       node.kind==K::uiOverlayText2A8F00||node.kind==K::uiTextOverlay29C850||node.kind==K::wattoWidget1D4320||
       node.kind==K::uiRouter2A6FD0||node.kind==K::fog13E6B0||node.kind==K::fogOff13F850||node.kind==K::fogOn13F980||
       node.kind==K::uiMovementControl2AAF40||node.kind==K::animationManager101330||node.kind==K::toyList2D9B40||
       node.kind==K::soundGroup2350D0||node.kind==K::wattoEmitter2921C0||&node==page_->scenePageChildren_[9])
        return walk141CE0(node,d);
    return rejectRenderCallback(d,0x141D40,"receiver",node);
}
bool OriginalUiGarageDrawContribution50KG::walk141D70(C& parent,Diagnostic& d)noexcept {
    if(!(parent.descendants24&8u))return true;
    for(C* child=parent.firstChild18;child;){
        if(!identityCurrent()||child->parent1C!=&parent||!child->reference00)return fail(d,0x141D98);
        if((child->flags29&9u)==1u&&(child->mask20&8u)){
            game::OriginalCameraStartResult service;
            if(!renderer_->serviceEntity141F3C(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x141DD8);
            if(!invoke141DE8(*child,d))return false;
        }
        child=child->next14;
    }
    return true;
}
bool OriginalUiGarageDrawContribution50KG::drawPageChildren141D70(OriginalUiGaragePageOwner50KG& page,std::uint32_t& pc)noexcept {
    pc=0x1D79BC;if(!identityCurrent()||phase_!=Phase::thirdPass||page_.get()!=&page||
        callback_!=&page.entity_||issuer_->sourceApplicationMode22C_!=5)return false;
    Diagnostic child;if(!walk141D70(page.entity_,child)){pc=child.sourcePC;return false;}
    pc=0x1D79C4;return true;
}
bool OriginalUiGarageDrawContribution50KG::invoke141DE8(C& node,Diagnostic& d)noexcept {
    using K=game::OriginalWreckComponentKind50LA;callback_=&node;
    if(!identityCurrent())return rejectRenderCallback(d,0x141DE8,"identity",node);
    // Aural table3CC900 inherits this exact Entity walker.
    if(inheritedAuralRenderReceiverCurrent(node))return walk141D70(node,d);
    bool shared=false;if(!invokeSharedDialogRender(node,0x48,shared,d))return false;
    if(shared)return true;
    //These are the actual next required native contributors. None may be
    //replaced with immutable menu geometry or omitted to manufacture a frame.
    if(&node==&page_->entity_){
        sourcePC_=0x1FBE10;OriginalUiGaragePageOwner50KG::Diagnostic pageDiagnostic;
        if(!page_->draw1FBE10(*this,pageDiagnostic))return fail(d,pageDiagnostic.sourcePC?pageDiagnostic.sourcePC:0x1FBE10);
        //Actual1FDE40 calls shared1D7550 AFTER1FBE10. The Page body is not
        //the inherited aperture/prompt/child callback and cannot replace it.
        sourcePC_=0x1D7550;
        if(!page_->drawShared1D7550(*this,pageDiagnostic))return fail(d,pageDiagnostic.sourcePC?pageDiagnostic.sourcePC:0x1D7550);
        return true; //The shared body itself owns the one real141D70 child walk.
    }
    if(&node==&camera_->entity_)return camera114D50(d);
    if(node.kind==K::uiTextOverlay29C850){
        sourcePC_=0x29D3F0;textRestoreStep_=0;
        OriginalUiGarageWidget1Diagnostic50KG widget;
        if(!page_->widgets_||!page_->widgets_->drawTextChild29D3F0(node,*this,widget))
            return fail(d,widget.sourcePC?widget.sourcePC:0x29D3F0);
        return true;
    }
    if(node.kind==K::wattoWidget1D4320){
        const auto watto=page_->widgets_->rows_[9].watto;
        if(!watto||watto->borrowChildDraw1D4E60(*page_,node)!=&node)return fail(d,0x1D4E60);
        return walk141D70(node,d);
    }
    if(node.kind==K::fog13E6B0||node.kind==K::fogOn13F980||node.kind==K::fogOff13F850)return fog13EA10(node,d);
    if(&node==page_->scenePageChildren_[9])return particleRender162D30(node,d);
    if(&node==issuer_->entities()->application26C()||&node==issuer_->entities()->application10()||
       &node==dispatcher_||&node==manager_||&node==page_->scenePageChildren_[0]||
       node.kind==K::uiOverlayText2A8F00||node.kind==K::uiMovementControl2AAF40||node.kind==K::uiRouter2A6FD0||
       node.kind==K::animationManager101330||node.kind==K::toyList2D9B40||
       node.kind==K::soundGroup2350D0||node.kind==K::wattoEmitter2921C0)return walk141D70(node,d);
    return rejectRenderCallback(d,0x141DE8,"receiver",node);
}
bool OriginalUiGarageDrawContribution50KG::fog13EA10(C& node,Diagnostic& d)noexcept {
    using K=game::OriginalWreckComponentKind50LA;
    if(!identityCurrent()||phase_!=Phase::thirdPass||callback_!=&node)return fail(d,0x13EA10);
    auto* scene=page_->borrowRenderFog13EA10(node);const auto session=session_.lock();
    auto manager=session?session->entryFog_:nullptr;
    if(!scene&&(!manager||!manager->currentAttached()||
       (&node!=manager->entity(0)&& &node!=manager->entity(1)&& &node!=manager->entity(2))))return fail(d,0x13EA10);
    if(node.kind==K::fogOff13F850){
        const auto& f=camera_->fields_;
        if((f.near1FC!=0x3f800000u||f.far200!=0x47c35000u)&&!setCameraRange114F20(1.f,100000.f,d))return false;
        //115310(null) returns1; actual245140(tag1C,0,0) returns without stores.
        return true;
    }
    auto apply=[&](auto& fog,auto& helper,float start,float end,std::uint32_t color,bool& proxyActive)noexcept {
        const bool proxy=node.kind==K::fogOn13F980;
        if(proxy){if(proxyActive||fog.field4C)return fail(d,0x13FA38);proxyActive=true;fog.field4C=1;}
        if(fog.entity.flags29&1u){
            if(!setCameraRange114F20(1.f,end,d))return false;
            auto& display=*renderer_->display_;
            if(!std::isfinite(start)||!std::isfinite(end))return fail(d,0x245140);
            display.sourceFogStartB0_=start;
            //The two consecutive245140 setters have no intervening read or
            //callback. An unknown old race B4 is not replaced with a default:
            //only the second genuine setter establishes the final native row.
            if(display.sourceFogRangeKnown_)
                display.sourceFogScaleB8_=start==display.sourceFogEndB4_?0.f:255.f/(start-display.sourceFogEndB4_);
            display.sourceFogEndB4_=end;
            display.sourceFogScaleB8_=start==end?0.f:255.f/(start-end);
            if(!std::isfinite(display.sourceFogScaleB8_))return fail(d,0x2451D4);
            display.sourceFogRangeKnown_=true;
            if(fog.dirty60){
                if(fog.dirty60!=1||!helper.current(color))return fail(d,0x13EAC8);
                if(!helper.packet){
                    //1340BC..CC actual retained128-DWORD allocation. Native
                    //renderer consumes only its genuine GSFOGCOL3D effect.
                    try{helper.packet=std::make_unique<OriginalUiGarageFogPacket50KG>(color);}
                    catch(...){return fail(d,0x1340C4);}
                    helper.buffer20=helper.packet->words.data();helper.count24=128;helper.count28=13;helper.field2C=1;
                }
                if(!renderer_->applyFogColor244A40(*this,publication_,color,d.renderer))return fail(d,0x13EBFC);
                helper.packet->applied=true;fog.dirty60=0; //13EC04 ONLY after real native command.
            }
        }
        if(proxy){fog.field4C=0;proxyActive=false;} //13FA58 after true return.
        return true;
    };
    if(scene)return apply(*scene->fog3EC_,*scene->fog3EC_->helper5C,*scene->fog3EC_->start34,
        *scene->fog3EC_->end38,*scene->fog3EC_->color30,scene->renderProxyActive_);
    return apply(*manager->fog_,*manager->helper_,manager->fog_->start34,manager->fog_->end38,
        manager->fog_->color30,manager->renderProxyActive_);
}
bool OriginalUiGarageDrawContribution50KG::camera114D50(Diagnostic& d)noexcept {
    if(!identityCurrent()||!cameraFrame_.current()||phase_!=Phase::thirdPass||
       callback_!=&camera_->entity_)return fail(d,0x114D50);
    if(!issuer_->sourceCameraSearch36ADA8_)return true; //114D6C skips children too.
    game::OriginalCameraStartResult service;
    if(issuer_->sourceCameraSynchronous435600_){
        if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x114D94);
        if(!renderer_->complete245090(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x114DAC);
    }
    const auto initialize=[&](std::size_t i)noexcept {
        if(!issuer_->sourceCameraRenderLazyGuards_[i]){
            issuer_->sourceCameraRenderLazyWords_[i]=-1;
            issuer_->sourceCameraRenderLazyGuards_[i]=true;
        }
    };
    initialize(0);initialize(1); //114DC8,114DE8, only on actual first use.
    if(camera_->fields_.parent38){
        const auto visibility=page_->renderVisibility114D28();
        if(!visibility)return fail(d,0x114E00);
        phase_=Phase::modelRender114E00;sourcePC_=0x114E00;
        assets::OriginalStandardMaterialDiagnostic50LA material;
        if(!visibility->renderCameraSearch114E00(*page_,cameraFrame_,material))
            return fail(d,material.sourcePC?material.sourcePC:0x114E00);
        phase_=Phase::thirdPass;
    }
    initialize(2); //114E1C, after actual Search/model return.
    if(issuer_->nativeCollectorFifoOwner_&&!drainCollector24B7B0(d))return false;
    if(issuer_->sourceCameraSynchronous435600_){
        initialize(3); //114E64, not initialized on the asynchronous branch.
        if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x114E80);
        if(!renderer_->complete245090(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x114E98);
    }else if(!renderer_->serviceEntity141F3C(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x114EB8);
    phase_=Phase::thirdPass;sourcePC_=0x114EC4;
    return walk141D70(camera_->entity_,d);
}
bool OriginalUiGarageDrawContribution50KG::select26F940(Diagnostic& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,0x26F940);

    if(!identityCurrent())return fail(d,0x26F940);
    auto& display=*renderer_->display_;
    if(!display.sourceCurrentCamera0Known_)return fail(d,0x26F940);
    display.selectFrontendCamera26F940(camera_); //Actual selection precedes target-dimension comparison.
    if(!display.sourceBufferCount0CInitialized_||display.sourceWidth4_<=0||display.sourceHeight8_<=0||
       static_cast<std::uint32_t>(display.sourceWidth4_)!=camera_->fields_.fullWidth204||
       static_cast<std::uint32_t>(display.sourceHeight8_)!=camera_->fields_.fullHeight208)return fail(d,0x26F978);
    return true;
}
bool OriginalUiGarageDrawContribution50KG::enable1156E0(Diagnostic& d)noexcept {
    if(!identityCurrent())return fail(d,0x1156E0);
    camera_->entity_.flags29=static_cast<std::uint8_t>(camera_->entity_.flags29|3u); //141560
    if(camera_->entity_.firstChild18)return fail(d,0x1414C0);
    if(!select26F940(d))return false;
    const auto& display=*renderer_->display_;
    if(!display.sourceBufferCount0CInitialized_)return fail(d,0x115714);
    camera_->fields_.count210=display.sourceBufferCount0C_+1u;return true;
}
bool OriginalUiGarageDrawContribution50KG::setCameraRange114F20(float nearPlane,float farPlane,Diagnostic& d)noexcept {
    if(!cameraFrame_.current()||!std::isfinite(nearPlane)||!std::isfinite(farPlane))return fail(d,0x114F20);
    auto& f=camera_->fields_;
    if(!f.coreKnown||!f.screen140Known||!f.guardProjection290Known||!f.combinedScreen250Known||!f.guardCombined2D0Known)
        return fail(d,0x237000);
    nearPlane=nearPlane<1.f?1.f:nearPlane;farPlane=farPlane>100000.f?100000.f:farPlane;
    f.near1FC=std::bit_cast<std::uint32_t>(nearPlane);f.far200=std::bit_cast<std::uint32_t>(farPlane); //114F54,114F68
    auto projection=floats(f.projection80),combined=floats(f.combined100),screen=floats(f.combinedScreen250);
    auto guard=floats(f.guardProjection290),guardCombined=floats(f.guardCombined2D0);
    const auto result=game::setOriginalCameraRange237000(nearPlane,farPlane,floats(f.viewC0),floats(f.matrix140),
        std::bit_cast<float>(f.guardX334),std::bit_cast<float>(f.guardY338),projection,combined,screen,guard,guardCombined);
    //Always retain the supported source store prefix on arithmetic failure.
    f.projection80=words(projection);f.combined100=words(combined);f.combinedScreen250=words(screen);
    f.guardProjection290=words(guard);f.guardCombined2D0=words(guardCombined);
    if(result.status!=game::OriginalCameraStartStatus::complete)return fail(d,result.sourcePC);
    //13E9F8/13F8EC invokes115310(a1=null), whose real body returns1 without copying.
    return true;
}
bool OriginalUiGarageDrawContribution50KG::walk141C30(C& parent,Diagnostic& d)noexcept {
    if(!(parent.descendants24&0x1000u))return true;
    for(C* child=parent.firstChild18;child;){
        if(!identityCurrent()||child->parent1C!=&parent||child->reference00==0)return fail(d,0x141C44);
        if((child->flags29&9u)==1u&&(child->mask20&0x1000u)){
            game::OriginalCameraStartResult service;
            if(!renderer_->serviceEntity141F3C(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x141C98);
            if(!invoke141CA8(*child,d))return false;
        }
        child=child->next14; //141CB0, only after the exact callback returned.
    }
    return true;
}
bool OriginalUiGarageDrawContribution50KG::invoke141CA8(C& node,Diagnostic& d)noexcept {
    using K=game::OriginalWreckComponentKind50LA;callback_=&node;
    const auto session=session_.lock();if(!session||!identityCurrent())return rejectRenderCallback(d,0x141CA8,"identity",node);
    // Aural table3CC900 inherits this exact Entity walker.
    if(inheritedAuralRenderReceiverCurrent(node))return walk141C30(node,d);
    bool shared=false;if(!invokeSharedDialogRender(node,0x40,shared,d))return false;
    if(shared)return true;
    if(&node==&page_->entity_){
        for(;demandedPageQuads_<121;++demandedPageQuads_){
            auto& quad=page_->quads_[6+demandedPageQuads_];OriginalUiGarageQuad50KG::Diagnostic qd;
            if(!quad.demand1FDD60(qd))return fail(d,qd.sourcePC?qd.sourcePC:0x1FDD80);
        }
        return walk141C30(node,d); //1FDE10
    }
    if(&node==&camera_->entity_){
        if(issuer_->sourceCameraSearch36ADA8_){
            if(camera_->fields_.parent38){
                auto visibility=page_->renderVisibility114D28();if(!visibility)return fail(d,0x114D28);
                phase_=Phase::search114D28;sourcePC_=0x114D28;
                assets::OriginalStandardMaterialDiagnostic50LA vd;
                if(!visibility->prepareCameraSearch114D28(*page_,cameraFrame_,vd))return fail(d,vd.sourcePC?vd.sourcePC:0x114D28);
                phase_=Phase::firstPass;
            }
        }
        return walk141C30(node,d); //114D34
    }
    if(node.kind==K::fog13E6B0||node.kind==K::fogOn13F980){
        float end=0;bool owned=page_->renderFogEnd13E9C0(node,end);
        if(!owned&&session->entryFog_&&session->entryFog_->currentAttached()){
            const auto fog=session->entryFog_->inspect();
            if(fog.rangeKnown&&(fog.live[0]==&node||fog.live[2]==&node)){end=fog.end38;owned=true;}
        }
        if(!owned)return fail(d,0x13E9C0);
        return setCameraRange114F20(1.f,end,d);
    }
    if(node.kind==K::fogOff13F850){
        //Only exact manager or Page reset identities can execute13F8C0.
        bool owned=page_->scenePageChildren_[3]==&node;
        if(!owned&&session->entryFog_&&session->entryFog_->currentAttached())owned=session->entryFog_->inspect().live[1]==&node;
        if(!owned)return fail(d,0x13F8C0);
        const auto& f=camera_->fields_;
        if(f.near1FC==0x3f800000u&&f.far200==0x47c35000u)return true; //114CE8, then115310(null).
        return setCameraRange114F20(1.f,100000.f,d);
    }
    if(&node==page_->scenePageChildren_[9])return particleProject162870(node,d);
    //These are recovered exact+40=141C30 tables, not a generic method
    //dispatcher. Every other actual receiver stays at its missing call.
    if(&node==issuer_->entities()->application26C()||&node==issuer_->entities()->application10()||
       &node==dispatcher_||&node==manager_||&node==page_->scenePageChildren_[0]||
       node.kind==K::uiOverlayText2A8F00||node.kind==K::uiTextOverlay29C850||node.kind==K::uiMovementControl2AAF40||
       node.kind==K::wattoWidget1D4320||node.kind==K::soundGroup2350D0||node.kind==K::wattoEmitter2921C0||
       node.kind==K::animationManager101330||node.kind==K::toyList2D9B40||node.kind==K::uiRouter2A6FD0)return walk141C30(node,d);
    return rejectRenderCallback(d,0x141CA8,"receiver",node);
}
bool OriginalUiGarageDrawContribution50KG::continue140230(Diagnostic& d)noexcept {
    d={};if(attempted_)return fail(d,sourcePC_);attempted_=true;
    if(!identityCurrent())return fail(d,0x140230);
    auto& history=issuer_->sourceRenderHistory140230_;
    for(std::size_t i=0;i<8;++i)if(!history.diagnosticGuard[i]){history.diagnostic[i]=-1;history.diagnosticGuard[i]=true;}
    for(std::size_t i=0;i<4;++i)if(!history.wordGuard[i]){history.words[i]=0;history.wordGuard[i]=true;}
    issuer_->sourcePreviousModels270_=issuer_->sourceRenderedModels274_;issuer_->sourceRenderedModels274_=0;
    issuer_->entities()->processedModels278_=0; //1403DC, not a replacement counter.
    phase_=Phase::disabling;
    for(;disabledSlots_<4;++disabledSlots_){
        std::array<C*,4> slots{};if(!issuer_->entities()->snapshotCameras140200(slots))return fail(d,0x1403F4);
        if(!slots[disabledSlots_])continue;
        if(slots[disabledSlots_]!=&camera_->entity_)return fail(d,0x1403F4);
        camera_->entity_.flags29=static_cast<std::uint8_t>(camera_->entity_.flags29&~3u); //115730→141520
        if(camera_->entity_.firstChild18)return fail(d,0x141460);
    }
    issuer_->sourceCameraIndex0C_=0;phase_=Phase::selecting; //140414
    while(issuer_->sourceCameraIndex0C_<4u){
        std::array<C*,4> slots{};if(!issuer_->entities()->snapshotCameras140200(slots))return fail(d,0x1407E8);
        const auto index=issuer_->sourceCameraIndex0C_;
        if(!slots[index]){++issuer_->sourceCameraIndex0C_;continue;}
        issuer_->sourceRenderedCamera268_=index;
        if(slots[index]!=&camera_->entity_)return fail(d,0x14046C);
        if(index){if(slots[index-1]!=&camera_->entity_)return fail(d,0x140450);
            camera_->entity_.flags29=static_cast<std::uint8_t>(camera_->entity_.flags29&~3u);
            if(camera_->entity_.firstChild18)return fail(d,0x141460);}
        if(!select26F940(d)||!enable1156E0(d))return false;
        bool complete=false;if(!renderer_->poll245030(complete,d.renderer))return fail(d,0x140498);
        if(complete){game::OriginalCameraStartResult service;
            if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x1404BC);}
        issuer_->sourceApplicationMode22C_=3;phase_=Phase::firstPass;sourcePC_=0x1404DC;
        if(!walk141C30(*issuer_->entities()->application26C(),d))return false;
        issuer_->sourceApplicationMode22C_=0;phase_=Phase::firstPassReturned; //1404E4
        //These are three distinct reached renderer services, in source order.
        //245090 is blocking242EE0(1,3), never a material collector receipt.
        game::OriginalCameraStartResult service;
        if(!renderer_->complete245090(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x1404F8);
        if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x140510);
        if(!renderer_->complete245090(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x140528);
        //The actual prior-frame Video/history branch is not a fresh-entry
        //assumption. Once reached, its real producer must run before viewport.
        if(index==0&&history.words[3]!=0)return fail(d,0x14053C);
        phase_=Phase::viewport;sourcePC_=0x14071C;
        if(!viewport114A00(d))return false;
        issuer_->sourceApplicationMode22C_=4;phase_=Phase::secondPass;sourcePC_=0x140738;
        if(!walk141CE0(*issuer_->entities()->application26C(),d))return false;
        issuer_->sourceApplicationMode22C_=0;phase_=Phase::secondPassReturned;
        if(!beginRenderer244020(d))return false;
        //Application+24=237960 is the recovered identity return1 leaf.
        issuer_->sourceApplicationMode22C_=5;phase_=Phase::thirdPass;sourcePC_=0x140788;
        if(!walk141D70(*issuer_->entities()->application26C(),d))return false;
        issuer_->sourceApplicationMode22C_=0;
        issuer_->sourceRenderDispatch423F40_=1; //1407A4.
        phase_=Phase::rendererEnd2440E0;sourcePC_=0x1407B0;
        if(!finishRenderer2440E0(d))return false;
        issuer_->sourceRenderedModels274_+=renderer_->display_->sourceRendererCounterC0_; //1407D4.
        ++issuer_->sourceCameraIndex0C_; //1407DC, only after the real renderer return.
        phase_=Phase::selecting;
    }
    for(;finalEnabledSlots_<4;++finalEnabledSlots_){
        std::array<C*,4> slots{};if(!issuer_->entities()->snapshotCameras140200(slots))return fail(d,0x140800);
        if(!slots[finalEnabledSlots_])continue;
        if(slots[finalEnabledSlots_]!=&camera_->entity_)return fail(d,0x140814);
        if(!enable1156E0(d))return false; //Actual Camera virtual20, not a flag-only substitute.
    }
    issuer_->sourceApplicationMode22C_=6;phase_=Phase::finalPass;sourcePC_=0x140844;
    if(!walk141E20(*issuer_->entities()->application26C(),d))return false;
    finalPassReturned140844_=true;issuer_->sourceApplicationMode22C_=0;sourcePC_=0x14084C;
    if(issuer_->sourceDebugCamera228Bit2_)return fail(d,0x140850);
    if(!rendererEndReturned2442CC_||!terminalQueue105990Current())return fail(d,0x140A38);
    sourcePC_=0x140A38;
    if(!renderer_->display_->renderer_.completeGarageSourceTarget140A38(*this,d.renderer))return fail(d,0x140A38);
    targetCompleted140A38_=true;
    phase_=Phase::renderReturned140230;sourcePC_=0x140A68;callback_=nullptr;
    d.sourcePC=sourcePC_;return true; //Scoped source return only; no presentation/timing commit here.
}
bool OriginalUiGarageDrawContribution50KG::nativeRetirementOwnerCurrent()const noexcept {
    const auto session=session_.lock();
    return issuer_&&session&&session->drawContribution_.get()==this&&session->issuer_==issuer_&&
        session->sameRoot(*issuer_)&&session->page_==page_&&page_&&camera_&&page_->sharedCamera_==camera_&&
        session->normalRetirementPhase_==OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted&&
        !session->retiring_&&renderer_&&renderer_->currentFor(issuer_)&&
        nativeRetirementPublication_==publication_&&session->renderCustodyCurrent140230(publication_);
}
bool OriginalUiGarageDrawContribution50KG::unsealedNativeRendererQuiescent()const noexcept {
    using R=NativeRetirementPhase;
    if(!attempted_||phase_!=Phase::unavailable||!issuer_||!publication_||
       frame_.owner_||frame_.native_||activeText_||activePageQuad_||activePageCostText_||
       drainBatchPending_||drainBatchFailed_||!renderer_||!renderer_->display_)
        return false;
    if(nativeRetirementPhase_==R::notStarted){
        if(!identityCurrent())return false;
    }else if((nativeRetirementPhase_!=R::admitted&&nativeRetirementPhase_!=R::preludeReleased)||
             !nativeRetirementOwnerCurrent())return false;
    const auto& display=*renderer_->display_;
    if(display.dispatch_.pendingCallbackDC||display.dispatch_.count54||display.dispatch_.count60)
        return false;
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(!fifo||fifo->application_!=issuer_||fifo->ownerThread_!=std::this_thread::get_id()||
       fifo->closed_||fifo->draining_||!fifo->records_.empty()||fifo->garageRecordCount_)
        return false;
    // An accepted batch is retained history, not an active submission. Admit
    // it only after the SAME real105990 epoch release; never clear its flag.
    if(drainContextCaptured_){if(!terminalQueue105990Current())return false;}
    else if(drainBatchSubmitted_||drainCursor_||!drainOrder_.empty())return false;
    // No GL work, state reset, SourceFrame seal or source callback permission.
    // Session must separately authenticate the actual post-continue unwind.
    return display.renderer_.garageUnsealedRenderQuiescent50KG(*this,publication_);
}
bool OriginalUiGarageDrawContribution50KG::releaseUnsealedBeforeRetirement()noexcept {
    using R=NativeRetirementPhase;
    if(nativeRetirementPhase_==R::released)return true; //This exact completed native release only.
    if(frame_.owner_||frame_.native_)return false;
    if(nativeRetirementPhase_==R::notStarted){
        if(!identityCurrent()&&(!issuer_||!renderReturned140230Current(*issuer_)))return false;
        nativeRetirementPublication_=publication_;
        if(!nativeRetirementOwnerCurrent())return false;
        // Native cleanup admission does not advance Source manager retirement.
        // Preserve it before the first real renderer operation, so a later
        // GPU failure cannot erase the original exact owner proof.
        nativeRetirementPhase_=R::admitted;
    }
    if(!nativeRetirementOwnerCurrent())return false;
    // Nothing submitted to the actual shared collector may be abandoned,
    // including an unexpected foreign record. Only105990 releases its pins.
    const auto fifo=issuer_->nativeCollectorFifoOwner_;
    if(fifo&&(fifo->draining_||!fifo->records_.empty()||fifo->garageRecordCount_))return false;
    if(drainBatchPending_||drainBatchFailed_)return false;
    VehicleWindowDiagnostic50DS retirement;
    if(nativeRetirementPhase_==R::admitted){
        if(!renderer_->releaseRenderPrelude(*this,publication_,retirement))return false;
        nativeRetirementPhase_=R::preludeReleased;
    }
    if(!releaseDrainResourcesBeforeRetirement(retirement))return false;
    if(shared470FirstPassOwner_){
        // This is AFTER exact prelude/FIFO/GPU release, BEFORE clearing Draw
        // identity. Failure preserves both owners and the native retry cursor.
        const auto shared=shared470FirstPassOwner_;OriginalUiSharedDialogs50KG::Diagnostic sharedDiagnostic;
        if(!shared->abort470AfterNativeRelease(*this,sharedDiagnostic))return false;
    }
    if(shared470ThirdPassOwner_){
        // Exact prelude/FIFO/GPU release already returned. Nonempty or failed
        // FIFO work still blocks above; abort never substitutes for105990.
        const auto shared=shared470ThirdPassOwner_;OriginalUiSharedDialogs50KG::Diagnostic sharedDiagnostic;
        if(!shared->abort470ThirdPassAfterNativeRelease(*this,sharedDiagnostic))return false;
    }
    if(shared47COwner_){
        const auto shared=shared47COwner_;OriginalUiSharedDialogs50KG::Diagnostic sd;
        if(!shared->abort47CAfterNativeRelease(*this,sd))return false;
    }
    // The Window has retired this exact native target lease; published pixels
    // remain available until the next real image. No Source field/count or
    // Page child has been retired by this native resource cleanup.
    phase_=Phase::unavailable;callback_=nullptr;manager_=nullptr;dispatcher_=nullptr;
    camera_.reset();page_.reset();renderer_.reset();session_.reset();issuer_=nullptr;
    timing_={};nativeRetirementPhase_=R::released;return true;
}
bool OriginalUiGarageDrawContribution50KG::beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&)const noexcept {
    return false; //No sealed complete frame exists at this bounded source prefix.
}
void OriginalUiGarageDrawContribution50KG::finishSourceSubmission(std::uint64_t,bool,bool)const noexcept {
    //Unreachable without the real begin authority. No pass/history commit.
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::readWarning470QuadInputs2DF550(
 const OriginalUiSharedDialogs50KG& shared,const OriginalUiGarageQuad50KG& quad,
 QuadRenderInputs& out,std::uint32_t& pc)const noexcept {
 pc=0x2DF550;if(!shared.warning470QuadCurrent(*this,quad))return false;
 QuadRenderInputs next;next.publication=publication_;
 next.sourceQuadEnabled383678=issuer_->entities()->screenQuadEnabled383678_!=0;
 if(!next.sourceQuadEnabled383678){out=next;pc=0x2DF8CC;return true;}
 if(!cameraFrame_.current()){pc=0x2DF608;return false;}
 next.viewportWidth1E8=camera_->fields_.viewport1E0[2];next.viewportHeight1EC=camera_->fields_.viewport1E0[3];
 if(!next.viewportWidth1E8||!next.viewportHeight1EC){pc=0x2DF608;return false;}
 out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectWarning470Quad246760(OriginalUiSharedDialogs50KG& shared,
 OriginalUiGarageQuad50KG& quad,assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
 if(!shared.warning470QuadCurrent(*this,quad)||!issuer_->nativeCollectorFifoOwner_){
  d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x246760;return false;
 }
 return issuer_->nativeCollectorFifoOwner_->collectShared470Quad246760(*this,shared,quad,d);
}
bool OriginalUiGarageDrawContribution50KG::readWarning470TextInputs2E0170(
 const OriginalUiSharedDialogs50KG& shared,const OriginalUiGarageScreenText50KG& text,
 TextRenderInputs& out,std::uint32_t& pc)const noexcept {
 pc=0x2E0F50;
 if(!shared.warning470TextCurrent(*this,text)||!cameraFrame_.current()||!camera_->fields_.coreKnown||
    !renderer_->display_->video_||renderer_->display_->video_.get()!=shared.inputs_.video.get()||
    !renderer_->display_->video_->host.current())return false;
 const auto& f=camera_->fields_;const auto depth=renderer_->display_->video_->packetZMaximum;
 if(!f.viewport1E0[2]||!f.viewport1E0[3]||depth<0)return false;
 TextRenderInputs next;next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
 next.videoDepthMaximum60=static_cast<float>(depth);next.publication=publication_;out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectWarning470Text246760(OriginalUiSharedDialogs50KG& shared,
 OriginalUiGarageScreenText50KG& text,std::size_t primitive,OriginalUiGarageScreenTextDiagnostic50KG& d)noexcept {
 if(!shared.warning470TextCurrent(*this,text)||!issuer_->nativeCollectorFifoOwner_){
  d.status=OriginalUiGarageScreenTextStatus50KG::authority;d.sourcePC=0x246760;return false;
 }
 if(!issuer_->nativeCollectorFifoOwner_->collectShared470Text246760(*this,shared,text,primitive,d.material)){
  d.status=OriginalUiGarageScreenTextStatus50KG::material;d.sourcePC=d.material.sourcePC;
  d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;return false;
 }
 d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::readPopup47CQuadInputs2DF550(
 const OriginalUiSharedDialogs50KG& shared,const OriginalUiGarageQuad50KG& quad,
 QuadRenderInputs& out,std::uint32_t& pc)const noexcept {
 pc=0x2DF550;if(!shared.popup47CQuadCurrent(*this,quad))return false;
 QuadRenderInputs next;next.publication=publication_;
 next.sourceQuadEnabled383678=issuer_->entities()->screenQuadEnabled383678_!=0;
 if(!next.sourceQuadEnabled383678){out=next;pc=0x2DF8CC;return true;}
 if(!cameraFrame_.current()){pc=0x2DF608;return false;}
 next.viewportWidth1E8=camera_->fields_.viewport1E0[2];next.viewportHeight1EC=camera_->fields_.viewport1E0[3];
 if(!next.viewportWidth1E8||!next.viewportHeight1EC){pc=0x2DF608;return false;}
 out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectPopup47CQuad246760(OriginalUiSharedDialogs50KG& shared,
 OriginalUiGarageQuad50KG& quad,assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
 if(!shared.popup47CQuadCurrent(*this,quad)||!issuer_->nativeCollectorFifoOwner_){
  d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x246760;return false;
 }
 return issuer_->nativeCollectorFifoOwner_->collectShared47CQuad246760(*this,shared,quad,d);
}
bool OriginalUiGarageDrawContribution50KG::readPopup47CTextInputs2E0170(
 const OriginalUiSharedDialogs50KG& shared,const OriginalUiGarageScreenText50KG& text,
 TextRenderInputs& out,std::uint32_t& pc)const noexcept {
 pc=0x2E0F50;
 if(!shared.popup47CTextCurrent(*this,text)||!cameraFrame_.current()||!camera_->fields_.coreKnown||
    !renderer_->display_->video_||renderer_->display_->video_.get()!=shared.inputs_.video.get()||
    !renderer_->display_->video_->host.current())return false;
 const auto& f=camera_->fields_;const auto depth=renderer_->display_->video_->packetZMaximum;
 if(!f.viewport1E0[2]||!f.viewport1E0[3]||depth<0)return false;
 TextRenderInputs next;next.viewportWidth1E8=f.viewport1E0[2];next.viewportHeight1EC=f.viewport1E0[3];
 next.videoDepthMaximum60=static_cast<float>(depth);next.publication=publication_;out=next;pc=0;return true;
}
bool OriginalUiGarageDrawContribution50KG::collectPopup47CText246760(OriginalUiSharedDialogs50KG& shared,
 OriginalUiGarageScreenText50KG& text,std::size_t primitive,OriginalUiGarageScreenTextDiagnostic50KG& d)noexcept {
 if(!shared.popup47CTextCurrent(*this,text)||!issuer_->nativeCollectorFifoOwner_){
  d.status=OriginalUiGarageScreenTextStatus50KG::authority;d.sourcePC=0x246760;return false;
 }
 if(!issuer_->nativeCollectorFifoOwner_->collectShared47CText246760(*this,shared,text,primitive,d.material)){
  d.status=OriginalUiGarageScreenTextStatus50KG::material;d.sourcePC=d.material.sourcePC;
  d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;return false;
 }
 d.sourceEffectsCommitted|=d.material.sourceEffectsCommitted;return true;
}
}

namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::shared47CReceiverCurrent(const OriginalUiSharedDialogs50KG& shared,std::uint32_t slot)const noexcept {
 const auto session=session_.lock();const auto& node=shared.controller47C_.entity;
 return identityCurrent()&&issuer_&&issuer_->state&&session&&callback_==&node&&
  ((slot==0x40&&phase_==Phase::firstPass&&issuer_->sourceApplicationMode22C_==3)||
   (slot==0x48&&phase_==Phase::thirdPass&&issuer_->sourceApplicationMode22C_==5))&&
  issuer_->state->sharedDialogs.get()==&shared&&shared.application_.lock()==issuer_->state&&
  shared.inputs_.root==issuer_&&shared.scope_==session->scope_&&shared.loadedCurrent()&&
  shared.reorderCompleteFor1B872C(*page_)&&shared.reorderedGrant_.lock()==session->grant_&&
  node.parent1C==dispatcher_&&shared.dialog470_.manager30==dispatcher_&&node.reference00==1&&
  node.kind==game::OriginalWreckComponentKind50LA::entity141300&&node.mask20==0xffffffffu&&
  (node.flags29&9u)==1u&&!node.firstChild18&&shared.controller47C_.vtable==0x3D0890u&&
  !activeText_&&!activePageQuad_&&!activePageCostText_;
}
bool OriginalUiGarageDrawContribution50KG::shared47CFailedUnwindCurrent(const OriginalUiSharedDialogs50KG& shared)const noexcept {
 const auto session=session_.lock();
 return nativeRetirementPhase_==NativeRetirementPhase::preludeReleased&&nativeRetirementOwnerCurrent()&&
  phase_==Phase::unavailable&&attempted_&&issuer_&&issuer_->state&&session&&
  issuer_->state->sharedDialogs.get()==&shared&&shared.inputs_.root==issuer_&&shared.loadedCurrent()&&
  callback_==&shared.controller47C_.entity&&shared47COwner_.get()==&shared&&
  session->unsealedRenderFailureHistoryCurrent(*issuer_)&&!frame_.owner_&&!frame_.native_&&
  !drainBatchPending_&&!drainBatchFailed_&&!textureCoordinator_&&!drainGpu_&&!drainImageBank_&&
  !drainBatchCanonical_&&renderer_&&!renderer_->ownsRenderPrelude(*this,publication_);
}
bool OriginalUiSharedDialogs50KG::canRenderGaragePurchase47C(const OriginalUiGaragePageOwner50KG& page,Diagnostic& d)const noexcept {
 d={};d.sourcePC=0x206458;
 std::array<const assets::OriginalStandardMaterialObject50LA*,3> owners{};
 const auto fifo=inputs_.root?inputs_.root->nativeCollectorFifoOwner_:nullptr;
 if(!loadedCurrent()||!reorderCompleteFor1B872C(page)||popup47CDraw_||nativePopup47CFifoPins_||
    !canonical47COwnersCurrent(owners)||!popup47CDimPacketCurrent2E21D0()||
    !fifo||fifo->application_!=inputs_.root||fifo->closed_||fifo->draining_||
    fifo->ownerThread_!=std::this_thread::get_id()||!inputs_.context||!inputs_.context->textureLifetime_||
    inputs_.context->textureLifetime_->nativeWarningDemandOwner_||
    inputs_.context->textureLifetime_->nativeWarningDemandEpoch_){d.status=Status::resourcePending;return false;}
 for(const auto& q:controller47C_.strips)if(!q.canMutatePacket2DF550()){d.status=Status::resourcePending;return false;}
 for(const auto& q:controller47C_.icons)if(!q.canMutatePacket2DF550()){d.status=Status::resourcePending;return false;}
 for(const auto* t:{controller47C_.message270.get(),controller47C_.confirm2E0.get(),controller47C_.cancel350.get()})
  if(!t||!t->coldInputsCurrent()||t->nativeGarageFifoPins_||t->nativeMenuWarningPins_||
     t->packetRevision_==std::numeric_limits<std::uint64_t>::max()){d.status=Status::resourcePending;return false;}
 if(controller47C_.dim220->nativeGarageFifoPins_||controller47C_.dim220->nativeMenuWarningPins_||
    controller47C_.dim220->packetRevision_==std::numeric_limits<std::uint64_t>::max()){
  d.status=Status::resourcePending;return false;}
 d.sourcePC=0;return true;
}
bool OriginalUiSharedDialogs50KG::begin47CCallback(OriginalUiGarageDrawContribution50KG& draw,std::uint32_t slot,Diagnostic& d)noexcept {
 d={};
 if(!draw.shared47CReceiverCurrent(*this,slot)||!draw.page_||
    !canRenderGaragePurchase47C(*draw.page_,d)||popup47CPhase_!=Popup47CPhase::idle||
    popup47CEpoch_==std::numeric_limits<std::uint64_t>::max()||draw.shared47COwner_||draw.shared47CEpoch_||
    (slot==0x48&&draw.shared47CDemandPublication_!=draw.publication_))return fail(d,Status::phase,slot==0x40?0x206430:0x2064C0);
 ++popup47CEpoch_;popup47CDraw_=&draw;popup47CPhase_=slot==0x40?Popup47CPhase::first:Popup47CPhase::body;
 popup47COperation_=0;draw.shared47COwner_=application_.lock()->sharedDialogs;draw.shared47CEpoch_=popup47CEpoch_;
 auto& lifetime=*inputs_.context->textureLifetime_;
 lifetime.nativeWarningDemandOwner_=this;lifetime.nativeWarningDemandEpoch_=popup47CEpoch_;
 return true;
}
bool OriginalUiSharedDialogs50KG::demand47C206430(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_!=&draw||popup47CPhase_!=Popup47CPhase::first||
  !draw.shared47CReceiverCurrent(*this,0x40)||draw.shared47CEpoch_!=popup47CEpoch_)return fail(d,Status::owner,0x206458);
 assets::OriginalUiMemberView50KG psm,pst;
 for(std::size_t i=0;i<inputs_.popup->memberCount();++i){const auto m=inputs_.popup->member(i);
  if(!m.identity||!m.identity->name)continue;
  if(assets::OriginalStandardArchiveProvider50LA::sameName(m.identity->name,"ui\\textures\\overlay1popup.psm"))psm=m;
  if(assets::OriginalStandardArchiveProvider50LA::sameName(m.identity->name,"ui\\textures\\overlay1.pst"))pst=m;
 }
 const auto table=inputs_.domain->materials382B38();const auto textures=inputs_.domain->textures383400();
 if(!table||!textures||!psm.data||!pst.data)return fail(d,Status::resourcePending,0x206458);
 bool effects=false;
 //206458..20649C: two genuine2DF8A0 calls, including the null-texture branch.
 for(;popup47COperation_<2;++popup47COperation_){auto& q=controller47C_.strips[popup47COperation_];
  if(!q.packetCurrent2DF550()||q.offset!=0x40+popup47COperation_*0x60||
     q.domain_!=inputs_.domain||q.context_!=inputs_.context||q.garage_!=inputs_.popup||
     !table->demandSharedWarning2DF8A0(q.material54->material00,*textures,psm,pst,d.quad.material)){
   d.sourceEffectsCommitted=effects||d.quad.material.sourceEffectsCommitted;
   d.status=Status::resourcePending;d.sourcePC=d.quad.material.sourcePC?d.quad.material.sourcePC:0x206458;return false;}
  effects|=d.quad.material.sourceEffectsCommitted;
 }
 popup47CPhase_=Popup47CPhase::inherited;d.sourceEffectsCommitted=effects;d.sourcePC=0x2064A0;return true;
}
bool OriginalUiSharedDialogs50KG::popup47CDimCurrent(const OriginalUiGarageDrawContribution50KG& draw)const noexcept {
 return popup47CDraw_==&draw&&popup47CPhase_==Popup47CPhase::body&&popup47COperation_==0x220&&
  inputs_.context&&inputs_.context->textureLifetime_&&
  inputs_.context->textureLifetime_->nativeWarningDemandOwner_==this&&
  inputs_.context->textureLifetime_->nativeWarningDemandEpoch_==popup47CEpoch_&&
  draw.shared47COwner_.get()==this&&draw.shared47CEpoch_==popup47CEpoch_&&
  draw.shared47CReceiverCurrent(*this,0x48)&&draw.sourcePC_==0x206DCC;
}
bool OriginalUiSharedDialogs50KG::popup47CTextCurrent(const OriginalUiGarageDrawContribution50KG& draw,const Text& text)const noexcept {
 if(popup47CDraw_!=&draw||popup47CPhase_!=Popup47CPhase::body||draw.shared47COwner_.get()!=this||
  draw.shared47CEpoch_!=popup47CEpoch_||!draw.shared47CReceiverCurrent(*this,0x48)||
  !inputs_.context||!inputs_.context->textureLifetime_||
  inputs_.context->textureLifetime_->nativeWarningDemandOwner_!=this||
  inputs_.context->textureLifetime_->nativeWarningDemandEpoch_!=popup47CEpoch_)return false;
 return (popup47COperation_==0x270&&controller47C_.message270.get()==&text&&draw.sourcePC_==0x206DFC)||
  (popup47COperation_==0x2E0&&controller47C_.confirm2E0.get()==&text&&draw.sourcePC_==0x2072B4)||
  (popup47COperation_==0x350&&controller47C_.cancel350.get()==&text&&draw.sourcePC_==0x2072D0);
}
bool OriginalUiSharedDialogs50KG::popup47CQuadCurrent(const OriginalUiGarageDrawContribution50KG& draw,const Quad& q)const noexcept {
 if(popup47CDraw_!=&draw||popup47CPhase_!=Popup47CPhase::body||draw.shared47COwner_.get()!=this||
  draw.shared47CEpoch_!=popup47CEpoch_||!draw.shared47CReceiverCurrent(*this,0x48)||
  !inputs_.context||!inputs_.context->textureLifetime_||
  inputs_.context->textureLifetime_->nativeWarningDemandOwner_!=this||
  inputs_.context->textureLifetime_->nativeWarningDemandEpoch_!=popup47CEpoch_)return false;
 for(std::size_t i=0;i<3;++i)if(&controller47C_.strips[i]==&q)return popup47COperation_==0x40+i*0x60&&draw.sourcePC_==0x2072F4;
 for(std::size_t i=0;i<2;++i)if(&controller47C_.icons[i]==&q)return popup47COperation_==0x160+i*0x60&&draw.sourcePC_==(i?0x2072C8u:0x2072ACu);
 return false;
}
bool OriginalUiSharedDialogs50KG::draw47C2064C0(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_!=&draw||popup47CPhase_!=Popup47CPhase::body||
  !draw.shared47CReceiverCurrent(*this,0x48))return fail(d,Status::owner,0x2064C0);
 const auto app=application_.lock();game::OriginalUiControllerPopupRender50KG plan;
 if(!app||game::OriginalUiControllerPopup50KG::render(app->controllerPopupState,plan)!=game::OriginalUiControllerPopupStatus50KG::ok)
  return fail(d,Status::source,0x2064DC);
 const auto position=[](Quad& q,float x,float y)noexcept {
  if(q.position10[0]!=x||q.position10[1]!=y){q.position10[0]=x;q.position10[1]=y;q.flags5CValue|=1;q.flags5CKnownMask|=1;
   if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}}
 };
 for(std::size_t i=0;i<3;++i){auto& q=controller47C_.strips[i];const auto& v=plan.strips[i];
  position(q,v.x,v.y);
  if(q.uv30[2]!=v.height){q.uv30[2]=v.height;q.flags5CValue|=1;q.flags5CKnownMask|=1;
   if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}}
 }
 // Preserve the actual source prefix if a later real packet/collector call fails.
 app->controllerPopupState=plan.state;d.sourceEffectsCommitted=true;
 popup47COperation_=0x220;draw.sourcePC_=0x206DCC;
 if(!drawPopup47CDim2E21D0(draw,d))return false;
 const auto textCall=[&](Text& text,std::uint32_t offset,std::uint32_t pc)noexcept {
  popup47COperation_=offset;draw.sourcePC_=pc;
  if(!text.drawPopup47C2E0170(draw,*this,d.text)){d.status=Status::resourcePending;d.sourcePC=d.text.sourcePC;return false;}return true;
 };
 const auto quadCall=[&](Quad& q,std::uint32_t offset,std::uint32_t pc)noexcept {
  popup47COperation_=offset;draw.sourcePC_=pc;
  if(!q.drawPopup47C2DF550(draw,*this,d.quad)){d.status=Status::resourcePending;d.sourcePC=d.quad.sourcePC;return false;}return true;
 };
 if(plan.drawMessage){
  if(!textCall(*controller47C_.message270,0x270,0x206DFC))return false;
  // Original dual/single placements follow the message call, before buttons.
  const bool dual=plan.drawConfirm&&plan.drawCancel;
  for(std::size_t i=0;i<2;++i){const float x=dual?(i?200.f:380.f):280.f;
   position(controller47C_.icons[i],x,247.f);
   auto& text=i?*controller47C_.cancel350:*controller47C_.confirm2E0;
   if(text.vector10_[0]!=x+32.f||text.vector10_[1]!=257.f){text.vector10_[0]=x+32.f;text.vector10_[1]=257.f;text.flags5A_|=3;}
  }
  if(plan.drawConfirm&&(!quadCall(controller47C_.icons[0],0x160,0x2072AC)||!textCall(*controller47C_.confirm2E0,0x2E0,0x2072B4)))return false;
  if(plan.drawCancel&&(!quadCall(controller47C_.icons[1],0x1C0,0x2072C8)||!textCall(*controller47C_.cancel350,0x350,0x2072D0)))return false;
 }
 for(std::size_t i=0;i<3;++i)if(!quadCall(controller47C_.strips[i],0x40+i*0x60,0x2072F4))return false;
 popup47COperation_=0;popup47CPhase_=Popup47CPhase::inherited;d.sourceEffectsCommitted=true;d.sourcePC=0x207310;return true;
}
void OriginalUiSharedDialogs50KG::release47CQueueFenceIfQuiescent()noexcept {
 if(popup47CDraw_||nativePopup47CFifoPins_)return;
 if(!inputs_.context||!inputs_.context->textureLifetime_)std::terminate();
 auto& lifetime=*inputs_.context->textureLifetime_;
 if(lifetime.nativeWarningDemandOwner_!=this||lifetime.nativeWarningDemandEpoch_!=popup47CEpoch_)std::terminate();
 lifetime.nativeWarningDemandOwner_=nullptr;lifetime.nativeWarningDemandEpoch_=0;
}
bool OriginalUiSharedDialogs50KG::complete47CCallback(OriginalUiGarageDrawContribution50KG& draw,std::uint32_t slot,Diagnostic& d)noexcept {
 if(popup47CDraw_!=&draw||popup47CPhase_!=Popup47CPhase::inherited||
    draw.shared47COwner_.get()!=this||draw.shared47CEpoch_!=popup47CEpoch_||
    !draw.shared47CReceiverCurrent(*this,slot)||controller47C_.entity.firstChild18||
    !scope_->sharedDialogPrefixCurrent(*this,1,controller47C_.entity))return fail(d,Status::owner,slot==0x40?0x2064A0:0x207310);
 if(slot==0x40)draw.shared47CDemandPublication_=draw.publication_;
 popup47CDraw_=nullptr;popup47CPhase_=Popup47CPhase::idle;popup47COperation_=0;
 draw.shared47CEpoch_=0;draw.shared47COwner_.reset();release47CQueueFenceIfQuiescent();return true;
}
bool OriginalUiSharedDialogs50KG::abort47CAfterNativeRelease(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_!=&draw||nativePopup47CFifoPins_||draw.shared47COwner_.get()!=this||
    draw.shared47CEpoch_!=popup47CEpoch_||!draw.shared47CFailedUnwindCurrent(*this))return fail(d,Status::retirementPending,0x2064C0);
 popup47CDraw_=nullptr;popup47CPhase_=Popup47CPhase::idle;popup47COperation_=0;
 draw.shared47CEpoch_=0;draw.shared47COwner_.reset();release47CQueueFenceIfQuiescent();return true;
}
}
