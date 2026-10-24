#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../src/assets/original_standard_texture_lifetime50la.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <bit>
#include <exception>
#include <limits>
namespace rr::platform {
namespace {using Kind=OriginalUiWarningOperationKind50KG;}
bool OriginalUiSharedDialogs50KG::thirdPass470Matches(const OriginalUiGarageDrawContribution50KG& draw)const noexcept {
 const auto app=application_.lock();
 return thirdPass470Draw_==&draw&&thirdPass470Epoch_&&thirdPass470Phase_!=ThirdPass470Phase::idle&&
  draw.shared470ThirdPassOwner_.get()==this&&draw.shared470ThirdPassEpoch_==thirdPass470Epoch_&&
  app&&thirdPass470Update_==update470Count_&&!update470Pending_&&
  thirdPass470Cache_==std::array<std::uint32_t,4>{{dialog470_.primaryPresent29C,dialog470_.secondaryPresent2A0,
   app->courseUi474,app->courseUi478}}&&inputs_.context&&inputs_.context->textureLifetime_&&
  inputs_.context->textureLifetime_->nativeWarningDemandOwner_==this&&
  inputs_.context->textureLifetime_->nativeWarningDemandEpoch_==thirdPass470Epoch_;
}
bool OriginalUiSharedDialogs50KG::begin470ThirdPass(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};
 if(!loadedCurrent()||!draw.shared470ThirdPassReceiverCurrent(*this))return fail(d,Status::owner,0x204AC0);
 // No new epoch is inferred from a source update. Only this exact vtable+48
 // invocation can issue one, after its own completed +40 demand and tail.
 if(thirdPass470Draw_||thirdPass470Phase_!=ThirdPass470Phase::idle||nativeWarning470FifoPins_||
    firstPass470Draw_||demand470Pending_||update470Pending_||!update470Count_||
    draw.shared470ThirdPassOwner_||draw.shared470ThirdPassEpoch_||draw.shared470ThirdPassInheritedReturned_||
    !draw.shared470FirstPassCompletedEpoch_||draw.shared470FirstPassCompletedEpoch_!=firstPass470Completed_||
    draw.shared470FirstPassCompletedUpdate_!=update470Count_||
    thirdPass470Epoch_==std::numeric_limits<std::uint64_t>::max()||
    !inputs_.context||!inputs_.context->live()||!inputs_.context->textureLifetime_)
  return fail(d,Status::phase,0x204AC0);
 auto& lifetime=*inputs_.context->textureLifetime_;
 if(lifetime.nativeWarningDemandOwner_||lifetime.nativeWarningDemandEpoch_)return fail(d,Status::owner,0x204AC0);
 const auto app=application_.lock();if(!app||app->sharedDialogs.get()!=this)return fail(d,Status::owner,0x204AC0);
 OriginalUiWarningValues50KG values;
 values.delta290=dialog470_.delta290;values.timer294=dialog470_.timer294;values.active298=dialog470_.active298;
 values.primaryPresent29C=dialog470_.primaryPresent29C;values.secondaryPresent2A0=dialog470_.secondaryPresent2A0;
 values.primaryEnabled474=app->courseUi474;values.secondaryEnabled478=app->courseUi478;
 for(std::size_t i=0;i<3;++i){const auto& q=dialog470_.strips[i];auto& v=values.strips[i];
  v.positionX10=q.position10[0];v.positionY14=q.position10[1];v.height38=q.uv30[2];
  v.rotation3C=q.uv30[3];v.flags5CValue=q.flags5CValue;
 }
 const auto plan=planOriginalUiWarning204AC050KG(values);
 if(plan.status!=OriginalUiWarningGeometryStatus50KG::planned||!plan.operationCount||
    plan.operations[plan.operationCount-1].kind!=Kind::callInherited141D70)
  return fail(d,Status::source,plan.unsupportedPC?plan.unsupportedPC:0x204AC0);
 if(plan.operationCount>1){
  // All are the actual retained470 packets, not Garage page payloads. This
  // no-effects preflight does not synthesize GPU/FIFO success or roll back a
  // later genuine source prefix. Disabled process branches remain real calls.
  if(!warning470DimPacketCurrent2E21D0()||!dialog470_.message1B0||!dialog470_.alternate220||
     !dialog470_.message1B0->coldInputsCurrent()||!dialog470_.alternate220->coldInputsCurrent()||
     dialog470_.message1B0->nativeGarageFifoPins_||dialog470_.alternate220->nativeGarageFifoPins_)
   return fail(d,Status::resourcePending,0x2053F4);
  for(const auto& q:dialog470_.strips)
   if(!q.canMutatePacket2DF550()||(q.flags5CKnownMask&3u)!=3u)return fail(d,Status::resourcePending,0x204AC0);
 }
 ++thirdPass470Epoch_;thirdPass470Draw_=&draw;thirdPass470Phase_=ThirdPass470Phase::body;
 thirdPass470Update_=update470Count_;thirdPass470Cache_={{values.primaryPresent29C,values.secondaryPresent2A0,
  values.primaryEnabled474,values.secondaryEnabled478}};
 thirdPass470Plan_=plan;thirdPass470Next_=0;thirdPass470Effects_=false;
 draw.shared470ThirdPassOwner_=app->sharedDialogs;draw.shared470ThirdPassEpoch_=thirdPass470Epoch_;
 lifetime.nativeWarningDemandOwner_=this;lifetime.nativeWarningDemandEpoch_=thirdPass470Epoch_;
 d.sourcePC=0x204AC0;return true;
}
bool OriginalUiSharedDialogs50KG::warning470DimCurrent(const OriginalUiGarageDrawContribution50KG& draw)const noexcept {
 if(!thirdPass470Matches(draw)||thirdPass470Phase_!=ThirdPass470Phase::body||
    !draw.shared470ThirdPassReceiverCurrent(*this)||thirdPass470Next_>=thirdPass470Plan_.operationCount)return false;
 const auto& op=thirdPass470Plan_.operations[thirdPass470Next_];
 return op.kind==Kind::callDim2E21D0&&op.receiverOffset==0x160&&op.sourcePC==draw.sourcePC_;
}
bool OriginalUiSharedDialogs50KG::warning470TextCurrent(const OriginalUiGarageDrawContribution50KG& draw,const Text& text)const noexcept {
 if(!thirdPass470Matches(draw)||thirdPass470Phase_!=ThirdPass470Phase::body||
    !draw.shared470ThirdPassReceiverCurrent(*this)||thirdPass470Next_>=thirdPass470Plan_.operationCount)return false;
 const auto& op=thirdPass470Plan_.operations[thirdPass470Next_];
 return op.kind==Kind::callText2E0170&&op.sourcePC==draw.sourcePC_&&
  ((op.receiverOffset==0x1B0&&dialog470_.message1B0.get()==&text)||
   (op.receiverOffset==0x220&&dialog470_.alternate220.get()==&text));
}
bool OriginalUiSharedDialogs50KG::warning470QuadCurrent(const OriginalUiGarageDrawContribution50KG& draw,const Quad& quad)const noexcept {
 if(!thirdPass470Matches(draw)||thirdPass470Phase_!=ThirdPass470Phase::body||
    !draw.shared470ThirdPassReceiverCurrent(*this)||thirdPass470Next_>=thirdPass470Plan_.operationCount)return false;
 const auto& op=thirdPass470Plan_.operations[thirdPass470Next_];
 if(op.kind!=Kind::callStrip2DF550||op.sourcePC!=draw.sourcePC_)return false;
 for(std::size_t i=0;i<3;++i)if(&dialog470_.strips[i]==&quad)return op.receiverOffset==0x40u+i*0x60u;
 return false;
}
bool OriginalUiSharedDialogs50KG::continue470ThirdPass(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};const auto stop=[&](Status status,std::uint32_t pc)noexcept {
  d.status=status;d.sourcePC=pc;d.sourceEffectsCommitted=thirdPass470Effects_;return false;};
 if(!thirdPass470Matches(draw)||thirdPass470Phase_!=ThirdPass470Phase::body||
    !draw.shared470ThirdPassReceiverCurrent(*this))return stop(Status::owner,0x204AC0);
 for(;thirdPass470Next_<thirdPass470Plan_.operationCount;++thirdPass470Next_){
  if(!thirdPass470Matches(draw)||!draw.shared470ThirdPassReceiverCurrent(*this))return stop(Status::owner,0x204AC0);
  const auto& op=thirdPass470Plan_.operations[thirdPass470Next_];draw.sourcePC_=op.sourcePC;
  if(op.kind==Kind::callInherited141D70){
   if(thirdPass470Next_+1!=thirdPass470Plan_.operationCount)return stop(Status::source,op.sourcePC);
   ++thirdPass470Next_;thirdPass470Phase_=ThirdPass470Phase::inheritedPending;
   d.sourceEffectsCommitted=thirdPass470Effects_;d.sourcePC=op.sourcePC;return true;
  }
  if(op.kind==Kind::callDim2E21D0){
   const bool okay=drawWarning470Dim2E21D0(draw,d);thirdPass470Effects_|=d.sourceEffectsCommitted;
   if(!okay)return stop(d.status,d.sourcePC);continue;
  }
  if(op.kind==Kind::callText2E0170){
   Text* text=op.receiverOffset==0x1B0?dialog470_.message1B0.get():
     op.receiverOffset==0x220?dialog470_.alternate220.get():nullptr;
   if(!text)return stop(Status::source,op.sourcePC);
   const bool okay=text->drawWarning4702E0170(draw,*this,d.text);thirdPass470Effects_|=d.text.sourceEffectsCommitted;
   if(!okay)return stop(Status::resourcePending,d.text.sourcePC);continue;
  }
  if(op.kind==Kind::timer294){dialog470_.timer294=std::bit_cast<float>(op.valueBits);thirdPass470Effects_=true;continue;}
  if(op.receiverOffset<0x40||op.receiverOffset>0x100||(op.receiverOffset-0x40)%0x60)
   return stop(Status::source,op.sourcePC);
  auto& quad=dialog470_.strips[(op.receiverOffset-0x40)/0x60];
  if(op.kind==Kind::callStrip2DF550){
   const bool okay=quad.drawWarning4702DF550(draw,*this,d.quad);thirdPass470Effects_|=d.quad.sourceEffectsCommitted;
   if(!okay)return stop(Status::resourcePending,d.quad.sourcePC);continue;
  }
  if(!quad.canMutatePacket2DF550())return stop(Status::phase,op.sourcePC);
  switch(op.kind){
   case Kind::positionX10:quad.position10[0]=std::bit_cast<float>(op.valueBits);break;
   case Kind::positionY14:quad.position10[1]=std::bit_cast<float>(op.valueBits);break;
   case Kind::height38:quad.uv30[2]=std::bit_cast<float>(op.valueBits);break;
   case Kind::flags5C:quad.flags5CValue=static_cast<std::uint8_t>(op.valueBits);
     quad.flags5CKnownMask|=op.assertedDirtyBits;break;
   default:return stop(Status::source,op.sourcePC);
  }
  thirdPass470Effects_=true;
 }
 return stop(Status::source,0x2054A4); // Never invent inherited completion.
}
void OriginalUiSharedDialogs50KG::release470QueueFenceIfQuiescent()noexcept {
 if(nativeWarning470FifoPins_||thirdPass470Draw_)return;
 if(!inputs_.context||!inputs_.context->textureLifetime_)std::terminate();
 auto& lifetime=*inputs_.context->textureLifetime_;
 if(lifetime.nativeWarningDemandOwner_!=this||lifetime.nativeWarningDemandEpoch_!=thirdPass470Epoch_||firstPass470Draw_)
  std::terminate();
 lifetime.nativeWarningDemandOwner_=nullptr;lifetime.nativeWarningDemandEpoch_=0;
}
void OriginalUiSharedDialogs50KG::release470ThirdPass(OriginalUiGarageDrawContribution50KG& draw)noexcept {
 if(!thirdPass470Matches(draw))std::terminate();
 thirdPass470Draw_=nullptr;thirdPass470Phase_=ThirdPass470Phase::idle;
 thirdPass470Next_=0;thirdPass470Plan_={};thirdPass470Update_=0;thirdPass470Cache_={};thirdPass470Effects_=false;
 draw.shared470ThirdPassEpoch_=0;draw.shared470ThirdPassInheritedReturned_=false;draw.shared470ThirdPassOwner_.reset();
 release470QueueFenceIfQuiescent();
}
bool OriginalUiSharedDialogs50KG::complete470ThirdPass(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};if(!thirdPass470Matches(draw)||thirdPass470Phase_!=ThirdPass470Phase::inheritedPending||
    thirdPass470Next_!=thirdPass470Plan_.operationCount||!draw.shared470ThirdPassInheritedReturned_||
    !draw.shared470ThirdPassReceiverCurrent(*this)||dialog470_.entity.firstChild18||
    !scope_->sharedDialogPrefixCurrent(*this,0,dialog470_.entity))return fail(d,Status::owner,0x2054A4);
 d.sourceEffectsCommitted=thirdPass470Effects_;thirdPass470Completed_=thirdPass470Epoch_;
 release470ThirdPass(draw);d.sourcePC=0x2054BC;return true;
}
bool OriginalUiSharedDialogs50KG::abort470ThirdPassAfterNativeRelease(OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
 d={};if(!thirdPass470Matches(draw)||nativeWarning470FifoPins_||!draw.shared470ThirdPassFailedUnwindCurrent(*this))
  return fail(d,Status::retirementPending,0x204AC0);
 // Only no-queued-work native unwind. A failure after any append remains
 // pending until the real FIFO drain105990 releases every retained record.
 d.sourceEffectsCommitted=thirdPass470Effects_;thirdPass470Aborted_=thirdPass470Epoch_;
 release470ThirdPass(draw);d.sourcePC=draw.sourcePC_;return true;
}
}
