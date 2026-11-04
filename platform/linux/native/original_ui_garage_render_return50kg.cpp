#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_ui_frontend50kg.h"
#include "../../../src/game/lap_host_clock50id.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <bit>
#include <chrono>
#include <limits>
namespace rr::platform {
bool OriginalUiGarageCameraFrame50KG::readFinalSearchInvocation2805F0(
 const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageVisibility50KG& visibility,
 const game::OriginalWreckComponent50LA& receiver,std::uint64_t& serial)const noexcept {
 return current()&&cursor_->finalSearchInvocation2805F0(page,visibility,receiver,serial);
}
bool OriginalUiGarageDrawContribution50KG::finalSearchInvocation2805F0(
 const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageVisibility50KG& visibility,
 const C& receiver,std::uint64_t& serial)const noexcept {
 if(phase_!=Phase::finalPass||issuer_->sourceApplicationMode22C_!=6||sourcePC_!=0x2805F0||
    callback_!=&receiver||page_.get()!=&page||page.renderVisibility114D28().get()!=&visibility)return false;
 serial=publication_;return true;
}
bool OriginalUiGarageDrawContribution50KG::terminalQueue105990Current()const noexcept {
 if(!issuer_||!issuer_->nativeCollectorFifoOwner_||!drainContextCaptured_||drainBatchPending_||drainBatchFailed_||
    drainCursor_!=drainOrder_.size())return false;
 const auto& fifo=*issuer_->nativeCollectorFifoOwner_;
 return fifo.application_==issuer_&&!fifo.closed_&&!fifo.draining_&&fifo.records_.empty()&&!fifo.garageRecordCount_&&
     fifo.epoch04_==static_cast<std::uint32_t>(drainEpoch_+1u);
}
bool OriginalUiGarageDrawContribution50KG::finishRenderer2440E0(Diagnostic& d)noexcept {
 if(!identityCurrent()||phase_!=Phase::rendererEnd2440E0||sourcePC_!=0x1407B0||!renderer_||
    !renderer_->display_||!issuer_->videoOwner_||!issuer_->animationClockBackend_)return fail(d,0x2440E0);
 auto& video=*issuer_->videoOwner_;const auto index=issuer_->sourceCameraIndex0C_;
 if(renderer_->display_->video_.get()!=&video||!video.sourceFrontendEndEffectsKnown_||index>=video.sourceEndViews_.size())
     return fail(d,0x25B110);
 auto& view=video.sourceEndViews_[index];auto& clock=*issuer_->animationClockBackend_;
 // Actual25B110 calls these four owners in order. Their real constructor
 // fields survive ordinary UI rendering and Garage reentry. A real race
 // invalidates this bounded frontend history instead of seeding it afresh.
 if(view.overlay40C)return fail(d,0x25B1D0); //Required25E610 overlay is not an inert return.
 if(view.overlay494)return fail(d,0x25B180); //Required25DBB0 image effect.
 if(view.completed300!=1u){
   if(view.covered2F4||view.increasing2F8||view.decreasing2FC)return fail(d,0x25B200);
   float now=0;if(!game::sampleHostFloatMilliseconds50ID(clock,now))return fail(d,0x25B334);
   view.lastMilliseconds3C4=now; //Actual inactive25B200 branch still writes time.
 }
 game::OriginalVideoFadeDraw fade;
 const auto result=game::advanceOriginalVideoFade25B5D0(video.sourceEndFade_,clock,
     static_cast<std::int32_t>(index),static_cast<std::int32_t>(issuer_->entities()->cameraCount264_),fade);
 if(result.status!=game::OriginalCameraStartStatus::complete)return fail(d,result.sourcePC);
 if(fade.reached25E610)return fail(d,0x25E610); //Retain the actual numeric prefix; no omitted overlay draw.
 if(issuer_->sourceDebugOverlay14_)return fail(d,0x244108);
 issuer_->sourceRenderStats4355F0_=0;issuer_->sourceRenderStats4355F8_=0;
 auto& display=*renderer_->display_;
 const std::uint32_t values[2]={display.sourceRendererCounterBC_,display.sourceRendererCounterC0_};
 for(std::size_t i=0;i<2;++i){
   std::uint32_t now=0;if(!game::sampleHostTimestamp50ID(clock,now))return fail(d,0x164CB0);
   auto& peak=issuer_->sourceRenderPeaks4355C0_[i];const auto value=std::bit_cast<std::int32_t>(values[i]);
   if(peak.value04<value||peak.lifetime08<static_cast<std::uint32_t>(now-peak.timestamp00)){
     peak.value04=value;peak.timestamp00=now; //164DA4/A8; source BSS lifetime08 stays its real0.
   }
 }
 bool complete=false;if(!renderer_->poll245030(complete,d.renderer))return fail(d,0x2442A0);
 if(complete){game::OriginalCameraStartResult service;
   if(!renderer_->drain1404BC(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x2442BC);}
 rendererEndReturned2442CC_=true;sourcePC_=0x2442CC;return true;
}
bool OriginalUiGarageDrawContribution50KG::walk141E20(C& parent,Diagnostic& d)noexcept {
 if(!(parent.descendants24&0x10u))return true;
 for(C* child=parent.firstChild18;child;){
   if(!identityCurrent()||child->parent1C!=&parent||!child->reference00)return fail(d,0x141E34);
   if((child->flags29&9u)==1u&&(child->mask20&0x10u)){
     game::OriginalCameraStartResult service;
     if(!renderer_->serviceEntity141F3C(d.renderer,service))return fail(d,service.sourcePC?service.sourcePC:0x141E88);
     if(!invoke141E98(*child,d))return false;
   }
   child=child->next14;
 }
 return true;
}
bool OriginalUiGarageDrawContribution50KG::invoke141E98(C& node,Diagnostic& d)noexcept {
 using K=game::OriginalWreckComponentKind50LA;callback_=&node;
 if(!identityCurrent()||phase_!=Phase::finalPass||issuer_->sourceApplicationMode22C_!=6)
     return rejectRenderCallback(d,0x141E98,"identity",node);
 // Actual Aural table3CC900+4C=141E20, with the same retained owner proof.
 if(inheritedAuralRenderReceiverCurrent(node))return walk141E20(node,d);
 bool shared=false;if(!invokeSharedDialogRender(node,0x4c,shared,d))return false;
 if(shared)return true;
 if(node.kind==K::searchNode27F120){
   auto visibility=page_->renderVisibility114D28();if(!visibility)return fail(d,0x2805F0);
   sourcePC_=0x2805F0;assets::OriginalStandardMaterialDiagnostic50LA material;
   if(!visibility->finishSearchNode2805F0(*page_,cameraFrame_,node,material))return fail(d,material.sourcePC?material.sourcePC:0x2805F0);
   sourcePC_=0x141EA0;return true;
 }
 // These actual concrete vtables all inherit141E20. Node kind alone is not
 // authority: the same intrusive live forest and exact receiver come from
 // the enclosing walk. A newly introduced callback is never silently generic.
 if(&node==issuer_->entities()->application26C()||&node==issuer_->entities()->application10()||
    &node==dispatcher_||&node==manager_||&node==&page_->entity_||&node==&camera_->entity_||
    &node==page_->scenePageChildren_[0]||&node==page_->scenePageChildren_[9]||
    node.kind==K::uiOverlayText2A8F00||node.kind==K::uiTextOverlay29C850||node.kind==K::uiMovementControl2AAF40||
    node.kind==K::wattoWidget1D4320||node.kind==K::soundGroup2350D0||node.kind==K::wattoEmitter2921C0||
    node.kind==K::animationManager101330||node.kind==K::toyList2D9B40||node.kind==K::uiRouter2A6FD0||
    node.kind==K::fog13E6B0||node.kind==K::fogOff13F850||node.kind==K::fogOn13F980)
     return walk141E20(node,d);
 return rejectRenderCallback(d,0x141E98,"receiver",node);
}
}
namespace rr::platform {
bool OriginalUiGarageDrawContribution50KG::publishReturnedTarget140A68(
 std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d)noexcept {
 const auto session=session_.lock();d={};
 if(shared470FirstPassOwner_||shared470FirstPassEpoch_||shared470ThirdPassOwner_||shared470ThirdPassEpoch_||!issuer_||!renderReturned140230Current(*issuer_)||!session||
    session->runtimeTailPhase_!=OriginalUiGarageSession50KG::RootTailPhase::timingCommitted||
    session->runtimeTailPublication_!=publication_||targetPublishAttempted_||targetPublished140A68_){
   d.error=VehicleWindowError50DS::frame;d.operation="Garage target requires its exact committed Root audio/timing tail";return false;
 }
 targetPublishAttempted_=true;
 if(!renderer_->display_->renderer_.publishGarageSourceTarget140A68(*this,width,height,d))return false;
 targetPublished140A68_=true;return true;
}
bool OriginalUiGarageDrawContribution50KG::rearmNextPublication140230(
 ::original_ui_source_timing50LH::PublicationView timing,std::uint64_t serial,Diagnostic& d)noexcept {
 d={};const auto session=session_.lock();::original_ui_source_timing50LH::Sample sample;std::uint64_t prepared=0;
 if(shared470FirstPassOwner_||shared470FirstPassEpoch_||shared470ThirdPassOwner_||shared470ThirdPassEpoch_||
    !issuer_||!renderReturned140230Current(*issuer_)||!session||
    session->runtimeTailPhase_!=OriginalUiGarageSession50KG::RootTailPhase::timingCommitted||
    session->runtimeTailPublication_!=publication_||!targetPublished140A68_||
    !timing.sameOwner(timing_)||!timing.sameOwner(session->entryTiming_)||
    !timing.readPrepared(sample,prepared)||!serial||serial<=publication_||prepared!=serial||
    !drainResourcesBound_||!drainGpu_||!drainGpu_->initialized()||!drainImageBank_||
    !textureCoordinator_||!session->entryFrontend_||&session->entryFrontend_->coordinator!=textureCoordinator_||
    activeText_||activePageQuad_||activePageCostText_||!textureCoordinator_->canRearmGarageTextureServices50KG(*this)){d.sourcePC=0x140230;return false;}
 if(!page_||!page_->scene4C0_||page_->sceneBacking_.get()!=page_->scene4C0_||
    !page_->scene4C0_->sceneParticles_||page_->scene4C0_->sceneParticles_->nativeGarageFifoPins_){
    d.sourcePC=0x105990;return false;
 }
 //No source fields or scene owner can be reset to establish this boundary.
 //The same real FIFO already executed105990/24B954; retain that next epoch.
 if(!renderer_->display_->renderer_.rearmGarageSourceTarget140230(*this,d.renderer)){d.sourcePC=0x140230;return false;}
 //The same owner-thread preflight above remains current; renderer rearm
 //changes no Frontend lease field. Preserve a failed unexpected prefix.
 if(!textureCoordinator_->renewGarageTexturePublication140230(*this,serial))return fail(d,0x140230);
 timing_=std::move(timing);publication_=serial;phase_=Phase::issued;sourcePC_=0x140230;
 shared470FirstPassCompletedEpoch_=0;shared470FirstPassCompletedUpdate_=0;
 disabledSlots_=0;demandedPageQuads_=0;callback_=nullptr;attempted_=false;textRestoreStep_=0;
 viewportDepthCleared_=false;cameraPhase4Cleared_=false;finalEnabledSlots_=0;
 finalPassReturned140844_=false;rendererEndReturned2442CC_=false;
 targetCompleted140A38_=false;targetPublished140A68_=false;targetPublishAttempted_=false;
 drainActiveGeometry_.reset();drainBatchCanonical_.reset();drainOrder_.clear();drainCursor_=0;
 drainParticleGeometry_.reset();drainParticleSpans_.clear();drainParticleRanges_.clear();drainParticleStates_.clear();
 drainActiveRecordOrdinal_=0;drainCommittedQuads_=0;drainContextCaptured_=false;
 drainBatchPending_=false;drainBatchSubmitted_=false;drainBatchFailed_=false;drainBatchNative_={};
 drainScreenVertices_.clear();drainScreenStq_.clear();drainScreenW_.clear();drainScreenFog_.clear();
 //Bank/model/particle policies and source material binding histories deliberately stay.
 //The caller next rearms its exact Session/Root/Page traversal before invoking
 //this new publication; identityCurrent remains false until that real join.
 d.sourcePC=0x140230;return true;
}
}
