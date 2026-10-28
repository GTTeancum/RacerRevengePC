#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_options_session50kg.h"
#include "original_ui_shared_dialog_material_history50kg.h"
#include "../../../src/assets/original_standard_texture_lifetime50la.h"
#include "../../../src/assets/original_standard_material_table50la.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include <limits>
#include <cfenv>
#include <exception>
namespace rr::platform {
bool OriginalUiSharedDialogs50KG::controls2ObservedPresent(std::uint64_t& revision)const noexcept {
 revision=0;
 if(!loadedCurrent()||!inputs_.video)return false;
 const auto& host=inputs_.video->host;
 // This retained value is written only by successful GameHost::poll from the
 // real WindowHost slot1 observation. Caller flags never create this proof.
 if(!host.ownerThread()||!host.hasWindow()||!host.created()||!host.current()||
    !host.sharedDialogPollCurrent50KG_||!host.sharedDialogPollRevision50KG_||
    host.sharedDialogSecondPresence50KG_!=WindowControllerPresence50DQ::Present)return false;
 revision=host.sharedDialogPollRevision50KG_;return true;
}
bool OriginalUiSharedDialogs50KG::controls2MenuResourcesCurrent(
 const OriginalUiOptionsSession50KG& session,const OriginalUiOptionsDrawContribution50KG& draw)const noexcept {
 if(!menuTopologyCurrent()||std::fegetround()!=FE_TONEAREST||!inputs_.video||
    inputs_.video->width!=640||inputs_.video->height!=448||inputs_.video->colorBits!=32||
    inputs_.video->packetZMaximum!=0xFFFFFF||inputs_.video->zBufferFormat!=0x31||
    !draw.sharedMenuBoundCurrent(*this,session)||
    !session.sharedMenuLifetimeCurrent(*this,draw)||!sharedMaterialHistory_||
    !sharedMaterialHistory_->canonicalCurrent()||!inputs_.context||!inputs_.context->textureLifetime_||
    popup47CDraw_||nativePopup47CFifoPins_||firstPass470Draw_||thirdPass470Draw_||nativeWarning470FifoPins_||nativeMenuWarning470Pins_||
    menuRasterAttempted_||menuRasterFailed_||menuUpdateFailedOwner_||
    menuWarningPhase_!=MenuWarningPhase::idle||menuWarningIssued_||menuWarningAppended_||
    !warning470DimPacketCurrent2E21D0()||dialog470_.dim160->nativeGarageFifoPins_||
    dialog470_.dim160->nativeMenuWarningPins_||
    dialog470_.dim160->packetRevision_==std::numeric_limits<std::uint64_t>::max()||
    !dialog470_.message1B0||!dialog470_.alternate220||
    !dialog470_.message1B0->coldInputsCurrent()||!dialog470_.alternate220->coldInputsCurrent()||
    dialog470_.message1B0->nativeGarageFifoPins_||dialog470_.alternate220->nativeGarageFifoPins_||
    dialog470_.message1B0->nativeMenuWarningPins_||dialog470_.alternate220->nativeMenuWarningPins_)return false;
 for(const auto& q:dialog470_.strips)
  if(!q.canMutatePacket2DF550()||(q.flags5CKnownMask&3u)!=3u)return false;
 // Preflight the same bounded runtime/static-pass consumer as the actual
 // per-operation admission. Do not service prototypes or emit a packet here.
 const auto& runtime=inputs_.root->materialRuntime;
 const auto& retainedRuntime=inputs_.context->materialRuntime();
 if(!runtime.callbackOverrideKnown||runtime.callbackOverride435678||
    !retainedRuntime.callbackOverrideKnown||retainedRuntime.callbackOverride435678)return false;
 std::array<const assets::OriginalStandardMaterialObject50LA*,6> canonical{};
 const auto table=inputs_.domain->materials382B38();
 if(!table||!sharedHistoryCanonicalOwnersCurrent(canonical))return false;
 for(std::int32_t token=3228;token<=3230;++token){
  const assets::OriginalStandardMaterialObject50LA* material=nullptr;
  assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
  assets::OriginalStandardMaterialPassView50LA pass;
  if(!table->resolve298E60(token,material,diagnostic)||!material||
     material!=canonical[static_cast<std::size_t>(token-3228)]||!material->passCount48()||*material->passCount48()!=1||
     !material->pass4C(0,pass)||!pass.flags21||!pass.references04||*pass.references04<=0||
     !pass.mapCount20||*pass.mapCount20!=(token==3228?0u:1u)||
     ((*pass.flags21&2u)&&(pass.animation10||pass.textureAnimation10)))return false;
 }
 const auto display=inputs_.root->raceDisplayOwner50LA_.lock();
 if(!display||display->application_!=inputs_.root||display->video_!=inputs_.video||
    !display->frontendOwnerCurrent50KG()||!display->canRetain()||
    !display->sourceBufferCount0CInitialized_||display->sourceBufferCount0C_!=2u||
    display->sourceWidth4_!=640||display->sourceHeight8_!=448)return false;
 OriginalUiMenuRasterInputs50KG raster;
 const std::shared_ptr<const void> video=inputs_.video;
 // Read-only constructor/bank/publication proof, not applyMenu470RasterPrelude.
 return session.readSharedMenuRasterInputs(*this,draw,raster)&&
  raster.videoLifetime.get()==video.get()&&!raster.videoLifetime.owner_before(video)&&!video.owner_before(raster.videoLifetime)&&
  raster.viewport==std::array<std::uint32_t,4>{{0,0,640,448}}&&
  raster.xyOffsetFixed4==std::array<std::uint32_t,2>{{27648,29184}};
}
bool OriginalUiSharedDialogs50KG::admitControls2Menu470(const OriginalUiOptionsSession50KG& session,
 const OriginalUiOptionsDrawContribution50KG& draw,Diagnostic& d)const noexcept {
 // Preserve every old invisible-path phase/source failure, not just its result.
 if(invisible470204AC0(d))return true;
 if(d.status!=Status::resourcePending||d.sourcePC!=0x204A54)return false;
 const auto app=application_.lock();
 if(!app||app->controllerPopupState.active3CC||app->courseUi474!=0||app->courseUi478!=1||
    dialog470_.secondaryPresent2A0!=0||!session.controls2WarningCurrent(*this,draw)||
    !controls2MenuResourcesCurrent(session,draw))return false;
 // At +48 the real beginPage has reset and selected this collector's page.
 // At the prelease/+40 boundary its previous sourcePage is deliberately ignored.
 if(menu470Owner()&&(menuMainDraw_||menuOptionsDraw_!=&draw||menuOptionsSession_!=&session||
    (menu470Phase_!=Menu470Phase::idle&&menu470Phase_!=Menu470Phase::callbacks)||
    (menu470Phase_==Menu470Phase::callbacks&&menu470Slot_!=0x48)||
    (menu470Slot_==0x48&&!draw.controls2WarningCollectorCurrent(*this,menu470Invocation_))))return false;
 d={};d.sourcePC=0x204A54;return true;
}
bool OriginalUiSharedDialogs50KG::menuTopologyCurrent()const noexcept {
 if(menuUpdateAborted_||!loadedCurrent()||!scope_||scope_->managerLive_||scope_->managerRetiring_||
    reorderStep_||reorderingPage_||!initialMainAttached_||!completedReorders_||
    scope_->sharedDialogDetached_||scope_->sharedDialogReorderOrdinal_||
    scope_->sharedDialogsOwner_!=this||!scope_->dispatcherLive_)return false;
 const auto& dispatcher=scope_->dispatcher_;const auto& first=dialog470_.entity;const auto& second=controller47C_.entity;
 return dispatcher.firstChild18==&first&&first.parent1C==&dispatcher&&!first.previous10&&first.next14==&second&&
  second.parent1C==&dispatcher&&second.previous10==&first&&!second.next14&&dialog470_.manager30==&dispatcher&&
  dispatcher.mask20==0xffffffffu&&(dispatcher.flags29&9u)==1u&&
  first.mask20==0xffffffffu&&(first.flags29&9u)==1u&&second.mask20==0xffffffffu&&(second.flags29&9u)==1u&&
  dialog470_.vtable==0x3D0910&&controller47C_.vtable==0x3D0890&&
  scope_->sharedDialogPrefixCurrent(*this,0,first)&&scope_->sharedDialogPrefixCurrent(*this,1,second);
}
const OriginalUiSourceOwner50KM* OriginalUiSharedDialogs50KG::menu470Owner()const noexcept {
 return menuMainDraw_?static_cast<const OriginalUiSourceOwner50KM*>(menuMainDraw_):
  menuOptionsDraw_?static_cast<const OriginalUiSourceOwner50KM*>(menuOptionsDraw_):nullptr;
}
bool OriginalUiSharedDialogs50KG::menu470PendingFor(const OriginalUiSourceOwner50KM& owner)const noexcept {
 return menu470Owner()==&owner||menuUpdateFailedOwner_==&owner;
}
bool OriginalUiSharedDialogs50KG::menu470Current()const noexcept {
 if(!menu470Owner()||menu470Phase_==Menu470Phase::idle||!menu470Epoch_||!menu470Invocation_||
    !menuTopologyCurrent()||!menuDisplay_||!menuDisplayPinned_||menuDisplay_->application_!=inputs_.root||
    menuDisplay_->video_!=inputs_.video||!menuDisplay_->frontendOwnerCurrent50KG()||!menuDisplay_->canRetain()||
    !inputs_.context||!inputs_.context->textureLifetime_||
    inputs_.context->textureLifetime_->nativeWarningDemandOwner_!=this||
    inputs_.context->textureLifetime_->nativeWarningDemandEpoch_!=menu470Epoch_||menu470Update_!=update470Count_)
  return false;
 return menuMainDraw_?(menuMainSession_&&!menuOptionsSession_&&!menuOptionsDraw_&&
   menuMainSession_->sharedMenuLifetimeCurrent(*this,*menuMainDraw_)&&menuMainDraw_->sharedMenuInvocationCurrent(*this,menu470Invocation_)):
  (menuOptionsSession_&&!menuMainSession_&&menuOptionsSession_->sharedMenuLifetimeCurrent(*this,*menuOptionsDraw_)&&
   menuOptionsDraw_->sharedMenuInvocationCurrent(*this,menu470Invocation_));
}
bool OriginalUiSharedDialogs50KG::menu470RenderCurrent()const noexcept {
 return menu470Current()&&(menuMainDraw_?menuMainSession_->sharedMenuRenderCurrent(*this,*menuMainDraw_):
  menuOptionsSession_->sharedMenuRenderCurrent(*this,*menuOptionsDraw_));
}
bool OriginalUiSharedDialogs50KG::serviceMenu141F3C(Diagnostic& d)noexcept {
 if(!menu470Current())return fail(d,Status::owner,0x141F3C);
 std::int32_t actual=0;const auto result=game::serviceOriginalRenderer244D60(
  menuDisplay_->dispatch_,false,menuDisplay_->renderer_,d.renderer,actual);
 return result.status==game::OriginalCameraStartStatus::complete||fail(d,Status::resourcePending,result.sourcePC);
}
bool OriginalUiSharedDialogs50KG::walkMenu470Inherited(std::uint32_t mask,std::uint32_t pc,Diagnostic& d)noexcept {
 if(!menu470RenderCurrent()||menu470Phase_!=Menu470Phase::callbacks||
    !((mask==0x1000u&&pc==0x141C30u&&menu470Slot_==0x40u)||(mask==8u&&pc==0x141D70u&&menu470Slot_==0x48u))||
    menu470Callback_!=&dialog470_.entity||
    !scope_->sharedDialogPrefixCurrent(*this,0,dialog470_.entity))return fail(d,Status::owner,pc);
 // Actual inherited141C30/141D70 branch, not an inferred successful callback.
 // This bounded receiver is childless. Any new child must acquire its own
 // real service/callback owner rather than being silently skipped here.
 const auto& parent=dialog470_.entity;
 if(!(parent.descendants24&mask)){d.sourcePC=pc;return true;}
 for(const C* child=parent.firstChild18;child;child=child->next14)
  return fail(d,Status::owner,pc);
 d.sourcePC=pc;return true;
}
bool OriginalUiSharedDialogs50KG::beginMenu470(OriginalUiMainSession50KG& session,OriginalUiMainDraw50KG& draw,
 std::uint64_t invocation,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_||nativePopup47CFifoPins_||menu470Owner()||menuUpdateFailedOwner_||!invocation||!draw.sharedMenuBoundCurrent(*this,session)||
    !session.sharedMenuRenderCurrent(*this,draw))return fail(d,Status::owner,0x204A00);
 menuMainSession_=&session;menuMainDraw_=&draw;menu470Invocation_=invocation;
 return beginMenu470Body(d);
}
bool OriginalUiSharedDialogs50KG::beginMenu470(OriginalUiOptionsSession50KG& session,OriginalUiOptionsDrawContribution50KG& draw,
 std::uint64_t invocation,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_||nativePopup47CFifoPins_||menu470Owner()||menuUpdateFailedOwner_||!invocation||!draw.sharedMenuBoundCurrent(*this,session)||
    !session.sharedMenuRenderCurrent(*this,draw))return fail(d,Status::owner,0x204A00);
 menuOptionsSession_=&session;menuOptionsDraw_=&draw;menu470Invocation_=invocation;
 return beginMenu470Body(d);
}
bool OriginalUiSharedDialogs50KG::beginMenu470Body(Diagnostic& d)noexcept {
 const auto rejectUnissued=[&](Status status,std::uint32_t pc)noexcept {
  menuMainSession_=nullptr;menuOptionsSession_=nullptr;menuMainDraw_=nullptr;menuOptionsDraw_=nullptr;menu470Invocation_=0;
  return fail(d,status,pc);
 };
 if(popup47CDraw_||nativePopup47CFifoPins_||menu470Phase_!=Menu470Phase::idle||firstPass470Draw_||thirdPass470Draw_||nativeWarning470FifoPins_||nativeMenuWarning470Pins_||menuRasterAttempted_||
    demand470Pending_||update470Pending_||!menuTopologyCurrent()||
    menu470Epoch_==std::numeric_limits<std::uint64_t>::max()||!inputs_.context||!inputs_.context->textureLifetime_)
  return rejectUnissued(Status::phase,0x204A00);
 // Only the privately admitted Controls2 Session may add a secondary warning.
 // Every other consumer, including Main, retains the original invisible gate.
 if(!invisible470204AC0(d)&&(!menuOptionsSession_||!menuOptionsDraw_||
    !admitControls2Menu470(*menuOptionsSession_,*menuOptionsDraw_,d))){
  const auto status=d.status;const auto pc=d.sourcePC;return rejectUnissued(status,pc);}
 auto& lifetime=*inputs_.context->textureLifetime_;
 if(lifetime.nativeWarningDemandOwner_||lifetime.nativeWarningDemandEpoch_)return rejectUnissued(Status::owner,0x204A00);
 const auto display=inputs_.root->raceDisplayOwner50LA_.lock();
 if(!display||display->application_!=inputs_.root||display->video_!=inputs_.video||
    !display->frontendOwnerCurrent50KG()||!display->canRetain())return rejectUnissued(Status::owner,0x141F3C);
 menuDisplay_=display;display->pin();menuDisplayPinned_=true;
 ++menu470Epoch_;menu470Update_=update470Count_;menu470Frame_=0;menu470CompletedSlots_=0;
 menu470Phase_=Menu470Phase::callbacks;lifetime.nativeWarningDemandOwner_=this;lifetime.nativeWarningDemandEpoch_=menu470Epoch_;
 const auto failed=[&]()noexcept {
  if(d.status==Status::complete)fail(d,Status::owner,menu470Slot_==0x48?0x204AC0:0x204A00);
  menu470Phase_=Menu470Phase::failed;return false;
 };
 const auto& dispatcher=scope_->dispatcher_;auto& node=dialog470_.entity;
 // Only the represented470 callback is issued. This is deliberately NOT a
 // receipt for47C slot40, a complete dispatcher pass, or the absent Main Entity.
 if(!(dispatcher.descendants24&0x1000u)||!(node.mask20&0x1000u)){fail(d,Status::owner,0x141C30);return failed();}
 menu470Slot_=0x40;menu470Callback_=&node;
 if(!serviceMenu141F3C(d)||!menu470RenderCurrent()||!demand470Body204A00(d)||
    !walkMenu470Inherited(0x1000u,0x141C30,d))return failed();
 menu470CompletedSlots_|=1u;menu470Callback_=nullptr;menu470Slot_=0;
 // Keep the genuine +40 prefix, but do not project +48 packets into the old
 // collector buffers. Only that same Draw's completed begin can resume it.
 menu470Phase_=Menu470Phase::collectorPending;d.sourcePC=0x204AB8;return true;
}
bool OriginalUiSharedDialogs50KG::continueMenu470AfterCollectorBegin(
 const OriginalUiSourceOwner50KM& owner,Diagnostic& d)noexcept {
 d={};if(menu470Owner()!=&owner||!menu470RenderCurrent()||menu470Phase_!=Menu470Phase::collectorPending||
    menu470CompletedSlots_!=1u||menu470Callback_||menu470Slot_)
  return fail(d,Status::phase,0x204AC0);
 const auto failed=[&]()noexcept {
  if(d.status==Status::complete)fail(d,Status::owner,0x204AC0);
  menu470Phase_=Menu470Phase::failed;return false;
 };
 const bool initialized=menuMainDraw_?menuMainDraw_->sharedMenuCollectorInitialized(*this,menu470Invocation_):
  menuOptionsDraw_->sharedMenuCollectorInitialized(*this,menu470Invocation_);
 if(!initialized){fail(d,Status::owner,0x204AC0);return failed();}
 const auto& dispatcher=scope_->dispatcher_;auto& node=dialog470_.entity;
 if(!(dispatcher.descendants24&8u)||!(node.mask20&8u)){fail(d,Status::owner,0x141D70);return failed();}
 menu470Phase_=Menu470Phase::callbacks;menu470Callback_=&node;menu470Slot_=0x48;
 if(!serviceMenu141F3C(d)||
    (!invisible470204AC0(d)&&(!menuOptionsSession_||!menuOptionsDraw_||
      !admitControls2Menu470(*menuOptionsSession_,*menuOptionsDraw_,d)))||!executeMenu470WarningBody(d)||
    !walkMenu470Inherited(8u,0x141D70,d)||!completeMenu470WarningTail(d))return failed();
 menu470CompletedSlots_|=2u;menu470Callback_=nullptr;menu470Slot_=0;
 menu470Phase_=Menu470Phase::popupBody;d.sourcePC=0x2054BC;return true;
}
bool OriginalUiSharedDialogs50KG::renderMenuPopupSibling(game::OriginalUiControllerPopupRender50KG& plan,Diagnostic& d)noexcept {
 d={};if(!menu470RenderCurrent()||menu470Phase_!=Menu470Phase::popupBody||menu470CompletedSlots_!=3u)
  return fail(d,Status::phase,0x2064C0);
 // Consume the existing47C semantic body ONCE at its proven sibling point.
 // This does not assert206430 demand or a full47C vtable traversal.
 if(!serviceMenu141F3C(d)) {menu470Phase_=Menu470Phase::failed;return false;}
 const auto app=application_.lock();
 if(game::OriginalUiControllerPopup50KG::render(app->controllerPopupState,plan)!=game::OriginalUiControllerPopupStatus50KG::ok){
  menu470Phase_=Menu470Phase::failed;return fail(d,Status::source,0x2064C0);}
 app->controllerPopupState=plan.state; // Preserve real timer/geometry writes before fallible projection.
 menu470Callback_=&controller47C_.entity;menuPopupPlan_=&plan;menu470Phase_=Menu470Phase::popupProjection;
 d.sourceEffectsCommitted=true;d.sourcePC=0x2064C0;return true;
}
bool OriginalUiSharedDialogs50KG::menuPopupProjectionCurrent(const OriginalUiSourceOwner50KM& owner,
 const game::OriginalUiControllerPopupRender50KG& plan)const noexcept {
 return menu470Owner()==&owner&&menu470RenderCurrent()&&menu470Phase_==Menu470Phase::popupProjection&&
  menu470Callback_==&controller47C_.entity&&menuPopupPlan_==&plan&&menu470CompletedSlots_==3u;
}
bool OriginalUiSharedDialogs50KG::completeMenuPopupProjection(const OriginalUiSourceOwner50KM& owner,
 const game::OriginalUiControllerPopupRender50KG& plan,Diagnostic& d)noexcept {
 d={};if(!menuPopupProjectionCurrent(owner,plan))return fail(d,Status::phase,0x2064C0);
 menuPopupPlan_=nullptr;menu470Callback_=nullptr;menu470Phase_=Menu470Phase::cpuPending;d.sourcePC=0x2064C0;return true;
}
bool OriginalUiSharedDialogs50KG::sealMenu470Frame(const OriginalUiSourceOwner50KM& owner,std::uint64_t frame,Diagnostic& d)noexcept {
 d={};if(menu470Owner()!=&owner||!frame||!menu470RenderCurrent()||menu470Phase_!=Menu470Phase::cpuPending||
    !(menuMainDraw_?menuMainDraw_->sharedMenuFrameCurrent(*this,menu470Invocation_,frame):
      menuOptionsDraw_->sharedMenuFrameCurrent(*this,menu470Invocation_,frame)))return fail(d,Status::phase,0x24B7B0);
 if(!menuWarningPacketsCurrent()||!sealMenu470RasterFrame(frame,d))return false;
 menu470Frame_=frame;menu470Phase_=Menu470Phase::framePrepared;return true;
}
bool OriginalUiSharedDialogs50KG::menu470FrameCurrent(const OriginalUiSourceOwner50KM& owner,std::uint64_t frame)const noexcept {
 return menu470Owner()==&owner&&menu470Current()&&menu470Frame_==frame&&frame&&
  menu470Phase_==Menu470Phase::framePrepared&&menu470CompletedSlots_==3u&&
  (!menuRasterAttempted_||menu470RasterCurrent())&&menuWarningPacketsCurrent();
}
bool OriginalUiSharedDialogs50KG::beginMenu470Submission(const OriginalUiSourceOwner50KM& owner,std::uint64_t frame,
 const VehicleWindowGl50DS& renderer)noexcept {
 if(!menu470FrameCurrent(owner,frame)||&menuDisplay_->renderer_!=&renderer)return false;
 menu470Phase_=Menu470Phase::submitting;return true;
}
void OriginalUiSharedDialogs50KG::failMenu470Submission(const OriginalUiSourceOwner50KM& owner)noexcept {
 if(menu470Owner()==&owner)menu470Phase_=Menu470Phase::failed;
}
bool OriginalUiSharedDialogs50KG::finishMenu470Submission(const OriginalUiSourceOwner50KM& owner,std::uint64_t frame,
 bool accepted,bool started)noexcept {
 (void)started; // Either failed outcome retains the lease until real retirement.
 if(menu470Owner()!=&owner||!menu470Current()||menu470Phase_!=Menu470Phase::submitting||menu470Frame_!=frame){
  failMenu470Submission(owner);return false;}
 if(!accepted){menu470Phase_=Menu470Phase::failed;return true;}
 // Actual SourceOwner callback acceptance, not CPU drain/renderCount or final
 // presentation success. No unrepresented source pass is claimed complete.
 if(!menuWarningPacketsReleasable()||!releaseMenu470RasterAfterAcceptance()){menu470Phase_=Menu470Phase::failed;return false;}
 releaseMenuWarningPackets();releaseMenu470();return true;
}
void OriginalUiSharedDialogs50KG::releaseMenu470()noexcept {
 if(!menu470Owner()||menuRasterAttempted_||!inputs_.context||!inputs_.context->textureLifetime_)std::terminate();
 resetMenuWarningBody();
 auto& lifetime=*inputs_.context->textureLifetime_;
 if(lifetime.nativeWarningDemandOwner_!=this||lifetime.nativeWarningDemandEpoch_!=menu470Epoch_)std::terminate();
 lifetime.nativeWarningDemandOwner_=nullptr;lifetime.nativeWarningDemandEpoch_=0;
 demand470Pending_=false;demand470Next_=0;demand470Update_=0;demand470Cache_={};demand470Effects_=false;
 menu470Phase_=Menu470Phase::idle;menuMainSession_=nullptr;menuOptionsSession_=nullptr;menuMainDraw_=nullptr;menuOptionsDraw_=nullptr;
 menu470Invocation_=0;menu470Frame_=0;menu470Update_=0;menu470CompletedSlots_=0;menu470Slot_=0;menu470Callback_=nullptr;menuPopupPlan_=nullptr;
 if(menuDisplayPinned_){menuDisplay_->unpin();menuDisplayPinned_=false;}menuDisplay_.reset();
}
bool OriginalUiSharedDialogs50KG::menu470CloseRendererCurrent(const VehicleWindowGl50DS& renderer)const noexcept {
 const auto& display=menu470Owner()?menuDisplay_:menuUpdateFailedDisplay_;
 const bool pinned=menu470Owner()?menuDisplayPinned_:menuUpdateFailedDisplayPinned_;
 if(!display||!pinned||display->application_!=inputs_.root||display->video_!=inputs_.video||
    !display->frontendOwnerCurrent50KG()||!display->canRetain()||&display->renderer_!=&renderer||
    display->dispatch_.pendingCallbackDC||display->dispatch_.count54||display->dispatch_.count60||
    menu470Phase_==Menu470Phase::submitting||nativeWarning470FifoPins_)return false;
 const auto fifo=inputs_.root->nativeCollectorFifoOwner_;
 return !fifo||(!fifo->draining_&&fifo->records_.empty()&&!fifo->garageRecordCount_);
}
bool OriginalUiSharedDialogs50KG::menu470CloseOwnerCurrent(const OriginalUiMainSession50KG& session,
 const VehicleWindowGl50DS& renderer)const noexcept {
 if(!menu470CloseRendererCurrent(renderer))return false;
 if(menuMainSession_==&session&&menuMainDraw_)return menu470Current()&&session.sharedMenuUnwindEntryCurrent(*this,*menuMainDraw_);
 return !menu470Owner()&&menuUpdateFailedMain_&&menuUpdateFailedOwner_==menuUpdateFailedMain_&&
  session.sharedMenuUnwindEntryCurrent(*this,*menuUpdateFailedMain_);
}
bool OriginalUiSharedDialogs50KG::menu470CloseOwnerCurrent(const OriginalUiOptionsSession50KG& session,
 const VehicleWindowGl50DS& renderer)const noexcept {
 if(!menu470CloseRendererCurrent(renderer))return false;
 if(menuOptionsSession_==&session&&menuOptionsDraw_)return menu470Current()&&session.sharedMenuUnwindEntryCurrent(*this,*menuOptionsDraw_);
 return !menu470Owner()&&menuUpdateFailedOptions_&&menuUpdateFailedOwner_==menuUpdateFailedOptions_&&
  session.sharedMenuUnwindEntryCurrent(*this,*menuUpdateFailedOptions_);
}
bool OriginalUiSharedDialogs50KG::retireMenu470AfterRenderer(const OriginalUiSourceOwner50KM& owner,
 const VehicleWindowGl50DS& renderer,Diagnostic& d)noexcept {
 d={};if(!menu470PendingFor(owner))return true;
 const bool retired=menuMainDraw_?menuMainDraw_->sharedMenuRetiredCurrent(*this,renderer):
  menuOptionsDraw_?menuOptionsDraw_->sharedMenuRetiredCurrent(*this,renderer):false;
 if(menu470Owner()){
  if(!retired||!menu470Current()||menu470Phase_==Menu470Phase::submitting||&menuDisplay_->renderer_!=&renderer||
     menuDisplay_->dispatch_.pendingCallbackDC||menuDisplay_->dispatch_.count54||menuDisplay_->dispatch_.count60||
     nativeWarning470FifoPins_)return fail(d,Status::retirementPending,0x204A00);
  const auto fifo=inputs_.root->nativeCollectorFifoOwner_;
  if(fifo&&(fifo->draining_||!fifo->records_.empty()||fifo->garageRecordCount_))return fail(d,Status::retirementPending,0x105990);
  if(!menuWarningPacketsReleasable()||!releaseMenu470RasterAfterRetirement(renderer,d))
   return fail(d,Status::retirementPending,0x204AC0);
  releaseMenuWarningPackets();
  releaseMenu470(); // Native abort, never source callback/frame completion.
 }
 if(menuUpdateFailedOwner_==&owner){
  const bool updateRetired=menuUpdateFailedMain_?menuUpdateFailedMain_->sharedMenuRetiredCurrent(*this,renderer):
   menuUpdateFailedOptions_?menuUpdateFailedOptions_->sharedMenuRetiredCurrent(*this,renderer):false;
  if(!updateRetired||!menuUpdateFailedDisplay_||!menuUpdateFailedDisplayPinned_||
     menuUpdateFailedDisplay_->application_!=inputs_.root||menuUpdateFailedDisplay_->video_!=inputs_.video||
     !menuUpdateFailedDisplay_->frontendOwnerCurrent50KG()||!menuUpdateFailedDisplay_->canRetain()||
     &menuUpdateFailedDisplay_->renderer_!=&renderer||menuUpdateFailedDisplay_->dispatch_.pendingCallbackDC||
     menuUpdateFailedDisplay_->dispatch_.count54||menuUpdateFailedDisplay_->dispatch_.count60||nativeWarning470FifoPins_)
   return fail(d,Status::retirementPending,0x2048F0);
  const auto fifo=inputs_.root->nativeCollectorFifoOwner_;
  if(fifo&&(fifo->draining_||!fifo->records_.empty()||fifo->garageRecordCount_))return fail(d,Status::retirementPending,0x105990);
  menuUpdateAborted_=true;menuUpdateFailedOwner_=nullptr;menuUpdateFailedMain_=nullptr;menuUpdateFailedOptions_=nullptr;
  menuUpdateFailedDisplay_->unpin();menuUpdateFailedDisplayPinned_=false;menuUpdateFailedDisplay_.reset();
 }
 return true;
}
bool OriginalUiSharedDialogs50KG::advanceMenu470(OriginalUiMainSession50KG& session,OriginalUiMainDraw50KG& draw,
 float delta,Diagnostic& d)noexcept {
 d={};if(!draw.sharedMenuBoundCurrent(*this,session)||!session.sharedMenuUpdateCurrent(*this,draw))return fail(d,Status::owner,0x2048F0);
 const bool okay=advanceMenu470Body(draw,delta,d);
 if(!okay&&menuUpdateFailedOwner_==&draw)menuUpdateFailedMain_=&draw;
 return okay;
}
bool OriginalUiSharedDialogs50KG::advanceMenu470(OriginalUiOptionsSession50KG& session,OriginalUiOptionsDrawContribution50KG& draw,
 float delta,Diagnostic& d)noexcept {
 d={};if(!draw.sharedMenuBoundCurrent(*this,session)||!session.sharedMenuUpdateCurrent(*this,draw))return fail(d,Status::owner,0x2048F0);
 const bool okay=advanceMenu470Body(draw,delta,d);
 if(!okay&&menuUpdateFailedOwner_==&draw)menuUpdateFailedOptions_=&draw;
 return okay;
}
bool OriginalUiSharedDialogs50KG::advanceMenu470Body(const OriginalUiSourceOwner50KM& owner,float delta,Diagnostic& d)noexcept {
 d={};if(popup47CDraw_||nativePopup47CFifoPins_||menu470Owner()||menuUpdateFailedOwner_||!menuTopologyCurrent()||
    menuUpdateEpoch_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::phase,0x2048F0);
 const auto display=inputs_.root->raceDisplayOwner50LA_.lock();
 if(!display||display->application_!=inputs_.root||display->video_!=inputs_.video||
    !display->frontendOwnerCurrent50KG()||!display->canRetain())return fail(d,Status::owner,0x2048F0);
 display->pin();++menuUpdateEpoch_;
 const auto retainFailed=[&]()noexcept {
  menuUpdateFailedOwner_=&owner;menuUpdateFailedDisplay_=display;menuUpdateFailedDisplayPinned_=true;return false;
 };
 if(!advance470Body2048F0(delta,d))return retainFailed();
 // Existing47C update is consumed here once, not repeated in Session.
 const auto app=application_.lock();game::OriginalUiControllerPopupPlan50KG popup;
 if(!menuTopologyCurrent()||game::OriginalUiControllerPopup50KG::update(app->controllerPopupState,delta,popup)!=
    game::OriginalUiControllerPopupStatus50KG::ok){fail(d,Status::source,0x206420);return retainFailed();}
 app->controllerPopupState=popup.state;display->unpin();d.sourceEffectsCommitted=true;d.sourcePC=0x206420;return true;
}
}
