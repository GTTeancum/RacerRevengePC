// Included once by the real shared-dispatcher owner translation unit.
namespace original_main_entry50KG {
rr::game::OriginalUiEntityScope50KG::Result Application::loadDispatcherGlobalWidgets2A35E0()const noexcept {
 using Scope=rr::game::OriginalUiEntityScope50KG;using S=Scope::Status;
 using Provider=rr::assets::OriginalStandardArchiveProvider50LA;
 const auto scope=sharedUiScopeOwner_;
 if(!scope||scope->issuer_!=this||!scope->live().okay()||!scope->dispatcherLive_||
    scope->managerLive_||scope->sharedDialogsOwner_||!standardMaterialDomainOwner_||
    !standardMaterialDomainOwner_->prepared())return {S::owner,0x2A35E0};
 const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
 if(scope->globalWidgetLoadPhase_==Scope::GlobalWidgetLoadPhase::absentReturned){
  // Source-time load history is not rerun on later mounts. The same real
  // namespace owner must survive; no claim about current absence is made.
  return provider&&provider==scope->globalWidgetProvider_&&
      scope->dispatcherGlobalCount34_==0&&!scope->dispatcherGlobalTable38_?
      Scope::Result{S::complete,0x2A3768}:Scope::Result{S::owner,0x2A3620};
 }
 scope->globalWidgetLoadPhase_=Scope::GlobalWidgetLoadPhase::unresolved;
 if(!provider)return {S::owner,0x2A3620};
 Provider::MemberView member;
 const auto found=provider->lookup132820("gwidgets.dat",member);
 if(found!=Provider::Lookup::absent)return {S::phase,0x2A3620};
 if(scope->dispatcherGlobalCount34_||scope->dispatcherGlobalTable38_)
  return {S::phase,0x2A3750};
 // Actual missing OPEN: no General/count parse, row allocation, constructor,
 // or hide call occurred. Temporary parser cleanup has no acquired backing.
 scope->globalWidgetProvider_=provider;scope->globalWidgetLookupRevision_=member.revision;
 scope->globalWidgetLoadPhase_=Scope::GlobalWidgetLoadPhase::absentReturned;
 return {S::complete,0x2A3768};
}
rr::game::OriginalUiEntityScope50KG::Result Application::continueDispatcherTransitionPrefix2A3510(
 std::uint64_t generation,rr::platform::VehicleWindowDiagnostic50DS& diagnostic)const noexcept {
 using Scope=rr::game::OriginalUiEntityScope50KG;using S=Scope::Status;
 diagnostic={};const auto scope=sharedUiScopeOwner_;
 if(!scope||scope->issuer_!=this||!scope->live().okay()||!generation||
    !scope->publishedPopulatedGarageManagerCurrent(generation)||!standardMaterialDomainOwner_||
    scope->globalWidgetLoadPhase_!=Scope::GlobalWidgetLoadPhase::absentReturned||
    scope->globalWidgetProvider_!=standardMaterialDomainOwner_->archiveProvider3E1A70()||
    scope->dispatcherGlobalCount34_||scope->dispatcherGlobalTable38_)
  return {S::phase,0x2A3510};
 if(scope->transitionManagerGeneration_!=generation){
  if(scope->transitionDisplayPinned_||scope->transitionDisplay_||
     (scope->transitionDisplayPhase_!=0&&scope->transitionDisplayPhase_!=3))
   return {S::generation,0x2A3538};
  scope->transitionManagerGeneration_=generation;scope->transitionDisplayPhase_=0;
 }
 if(scope->transitionDisplayPhase_==3)return {S::complete,0x2A35A4};
 if(!scope->transitionDisplay_){
  const auto display=raceDisplayOwner50LA_.lock();
  if(!display||display->application_!=this||!display->frontendOwnerCurrent50KG()||!display->canRetain())
   return {S::owner,0x2A3538};
  display->pin();scope->transitionDisplay_=display;scope->transitionDisplayPinned_=true;
 }
 auto& display=*scope->transitionDisplay_;
 if(!scope->transitionDisplayPinned_||display.application_!=this||
    !display.frontendOwnerCurrent50KG()||!display.canRetain())return {S::owner,0x2A3538};
 if(scope->transitionDisplayPhase_==0){
  std::int32_t actualV0=0;
  const auto serviced=rr::game::serviceOriginalRenderer244D60(display.dispatch_,true,
   display.renderer_,diagnostic,actualV0,rr::game::OriginalCameraDrainPolicy::initializedContext);
  if(serviced.status!=rr::game::OriginalCameraStartStatus::complete)return {S::phase,serviced.sourcePC};
  scope->transitionDisplayPhase_=1;
 }
 if(scope->transitionDisplayPhase_==1){
  // Always execute the source second slot24, including disabled-D8 first call.
  if(!display.renderer_.finishInitializedContextWork(diagnostic)||!diagnostic.contextWorkCompleted)
   return {S::phase,0x2A3550};
  scope->transitionDisplayPhase_=2;
 }
 //2A3588 sees the actual source count0 left by completed missing-file load.
 // No popup470/47C or manager widgets are substituted for global rows.
 if(scope->transitionDisplayPhase_!=2||scope->dispatcherGlobalCount34_||scope->dispatcherGlobalTable38_)
  return {S::phase,0x2A3588};
 scope->transitionDisplayPhase_=3;
 display.unpin();scope->transitionDisplayPinned_=false;scope->transitionDisplay_.reset();
 // Prefix only. No manager detach, child retirement, or current30 clear.
 return {S::complete,0x2A35A4};
}
}
