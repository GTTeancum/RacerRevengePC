// Place after every MC source-owner declaration, before the first cold prepare.
// Later Bank/session guards drain all peers first. This final cold guard also
// covers an exception midway through prepareStorage, before a Bank exists.
 auto closeMcSourcePrefix=[&]()noexcept->bool {try{
  if(!isMcTrack)return true;
  if(mcQueryBinding)return false; // Its actual Bank must have been closed first.
  if(mcParticleRender&&!mcParticleRender->closed()&&!mcParticleRender->close())return false;
  if(mcAnimationBinding&&mcAnimationBinding->needsClose()&&!mcAnimationBinding->close(mcAnimationBindingDiagnostic))return false;
  if(mcCollisionWorld){
   if(!mcCollisionWorld->beginRetirement()&&mcCollisionWorld->phase()!=mc_collision_world50MC::Phase::Closed)return false;
   mcWorldBorrow.reset();
   auto retired=mcCollisionWorld->retirementBorrow();
   if(retired){auto* storage=retired.registrationStorage();if(!storage||storage->queue.count)return false;
    for(auto* member:{&storage->mainMembership,&storage->secondaryMembership,&storage->terrainMembership}){
     const auto r=removeLiveZoneMembership50EN(storage->lifecycle,storage->world,member->state,storage->membershipWork);
     if(r.status!=LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
    for(std::size_t i=0;i<21;++i)if(auto* member=retired.triggerMembership(i)){
     const auto r=removeLiveZoneMembership50EN(storage->lifecycle,storage->world,member->state,storage->membershipWork);
     if(r.status!=LiveZoneStatus50EN::Complete||member->state.attached||member->state.object)return false;}
   }
  }
  if(!mcNavigation.close())return false;
  if(mcTriggerActions&&!mcTriggerActions->close())return false;
  if(mcParticleRuntime&&!mcParticleRuntime->quiesce())return false;
  if(mcTrackParticles&&!mcTrackParticles->close())return false;
  if(mcParticleRuntime&&!mcParticleRuntime->closeAfterPool())return false;
  if(mcCollisionWorld&&!mcCollisionWorld->close())return false;
  return true;
 }catch(...){return false;}};
 struct McSourcePrefixGuard {
  decltype(closeMcSourcePrefix)& close;
  ~McSourcePrefixGuard()noexcept {if(!close()){
   std::fputs("MC source prefix retirement rejected; backing retained until fail-stop\n",stderr);std::terminate();}}
 } mcSourcePrefixGuard{closeMcSourcePrefix};
