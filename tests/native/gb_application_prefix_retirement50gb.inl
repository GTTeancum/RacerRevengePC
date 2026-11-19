// Place after every GB source-owner declaration, before the first cold prepare.
// Later Bank/session guards drain all peers first. This final cold guard also
// covers an exception midway through prepareStorage, before a Bank exists.
 auto closeGbSourcePrefix=[&]()noexcept->bool {try{
  if(!isGbTrack)return true;
  if(gbQueryBinding)return false; // Its actual Bank must have been closed first.
  if(gbParticleRender&&!gbParticleRender->closed()&&!gbParticleRender->close())return false;
  if(trackAnimationBinding&&trackAnimationBinding->needsClose()&&!trackAnimationBinding->close(trackAnimationBindingDiagnostic))return false;
  if(gbCollisionWorld){
   if(!gbCollisionWorld->beginRetirement()&&gbCollisionWorld->phase()!=gb_collision_world50GB::Phase::Closed)return false;
   gbWorldBorrow.reset();
   auto retired=gbCollisionWorld->retirementBorrow();
   if(retired){auto* storage=retired.registrationStorage();if(!storage||storage->queue.count)return false;
    if(!storage->detachAllRows())return false; //every racer's query rows in the one world
    for(std::size_t i=0;i<retired.triggerCount();++i)if(auto* member=retired.triggerMembership(i)){
     const auto r=removeLiveZoneMembership50EN(storage->lifecycle,storage->world,member->state,storage->membershipWork);
     if(r.status!=LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
   }
  }
  if(!gbNavigation.close())return false;
  if(gbTriggerActions&&!gbTriggerActions->close())return false;
  if(gbParticleRuntime&&!gbParticleRuntime->quiesce())return false;
  if(gbTrackParticles&&!gbTrackParticles->close())return false;
  if(gbParticleRuntime&&!gbParticleRuntime->closeAfterPool())return false;
  if(gbPodSound&&!gbPodSound->close())return false;
  if(gbCollisionWorld&&!gbCollisionWorld->close())return false;
  return true;
 }catch(...){return false;}};
 struct GbSourcePrefixGuard {
  decltype(closeGbSourcePrefix)& close;
  ~GbSourcePrefixGuard()noexcept {if(!close()){
   std::fputs("GB source prefix retirement rejected; backing retained until fail-stop\n",stderr);std::terminate();}}
 } gbSourcePrefixGuard{closeGbSourcePrefix};
