// Include after racerBank.seal(raceRacerCount). Defines a cold-only placement
// lambda. Slot zero may refresh an alternate human's own lift/query owners.
// Seven opponents occupy two explicit HOST rows ahead of the retained
// rank-four center player pose. These are not claimed original lateral ranks.
// Native ground probes validate every start using each racer's authored lift.
// Four front starts and three back starts retain at least42 lateral or
//40 longitudinal units between pod centers, with player40 behind the back row.
auto placeOpponentAtGrid50HN=[&](std::size_t racer){
 requireMbRuntimeNavigation50MB();
 require50DU(racer<racerBank.count(),"invalid native grid slot");
 racerBank.visit(racer,[&](){
  require50DU(completedFrames==0,"opponent grid placement requires fresh state");
  constexpr float lateral[maxHostRaceRacers50HT]={0.f,-63.f,-21.f,21.f,63.f,-42.f,0.f,42.f};
  constexpr float forward[maxHostRaceRacers50HT]={0.f,80.f,80.f,80.f,80.f,40.f,40.f,40.f};
  BodyMatrix48G requested=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  if(trackSession->originalStaticCandidates){
   // Every position is original2A2880 geometry over this track's real M spline.
   // HOST roster rank assignment preserves the existing human rank4 and uses
   // every other source rank once. It is not an invented lateral grid layout.
   constexpr std::uint32_t nativeRank[maxHostRaceRacers50HT]={4,0,1,2,3,5,6,7};
   GridCenterResult50AC authoredGrid{};
   require50DU(selectedGridNavigation.prepare(nativeRank[racer],authoredGrid),
       "original complete starting-grid geometry unavailable");
   for(unsigned axis=0;axis<3;++axis){
    const float at=std::bit_cast<float>(authoredGrid.position[axis]);
    requested.v[12+axis]=axis==1?at+x.parameters.visual.verticalLimit7EC:at;
   }
  }else{
   // Preserve the existing explicit TA HOST grid unchanged.
   for(unsigned axis=0;axis<3;++axis)
    requested.v[12+axis]+=requested.v[axis]*lateral[racer]+requested.v[8+axis]*forward[racer];
  }
  BodyMatrix48G grounded{};
  require50DU(native_racer50HN::groundGridPose(ground,requested,
      x.parameters.visual.verticalLimit7EC,groundStack.data(),groundStack.size(),
      grounded),"AI grid has no actual ground hit");
  require50DU(removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,mainMembership,
      membershipWork).status==LiveZoneStatus50EN::Complete,"AI main grid detach failed");
  require50DU(removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,
      membershipWork).status==LiveZoneStatus50EN::Complete,"AI secondary grid detach failed");
  require50DU(removeLiveZoneMembership50EN(zoneLifecycle,liveWorld,terrainMembership,
      membershipWork).status==LiveZoneStatus50EN::Complete,"AI terrain grid detach failed");
  require50DU(native_racer50HN::placeColdGrid(s,x.bindings,grounded,rebaseModels,rebaseModelCount,
      poseHistory),"native cold AI grid placement failed");
  LiveZoneBounds50EN mainBounds{},secondaryBounds{};
  require50DU(prepareQueryCache(query,s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100,
      mainBounds)&&prepareQueryCache(secondaryQuery,
      s.nodes[static_cast<unsigned>(x.bindings.lifecycle.physical19C)].world100,secondaryBounds),
      "AI cold spring cache preparation failed");
  require50DU(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,mainMembership,
      mainMembershipStorage,mainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.f,
      mainBounds,membershipWork).status==LiveZoneStatus50EN::Complete,"AI main grid attach failed");
  require50DU(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,
      secondaryMembershipStorage,secondaryQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.f,
      secondaryBounds,membershipWork).status==LiveZoneStatus50EN::Complete,"AI secondary grid attach failed");
  TerrainPrepareInput48M terrainInput{};terrainInput.state=terrainOwner.initial;
  terrainInput.scene=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  terrainInput.lowerEndpoint140=terrainOwner.lowerEndpoint140;
  TerrainPrepareState48M terrainPrepared{};
  require50DU(prepareTerrainSinglePair48M(terrainInput,terrainPrepared)==TerrainPrepareStatus48M::QueryReady,
      "AI cold terrain cache preparation failed");
  const auto terrainBounds=live_terrain_probe50fr::bounds(terrainLiveOwner,terrainPrepared);
  require50DU(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,terrainMembership,
      terrainMembershipStorage,terrainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.f,
      terrainBounds,membershipWork).status==LiveZoneStatus50EN::Complete,"AI terrain grid attach failed");
  if(allAiEnabled||(humanCourseEnabled&&racer!=0)){
   // Genuine AI controls are initialized separately for every actual slot.
   // Grid placement owns geometry only; no HOST driver or keyboard policy.
   lapOwner.audioDispatch=nullptr;lapOwner.audioContext=nullptr;
   require50DU(s.fields.vehicle58==1u&&originalAi.initialized,
       "all-original-AI grid lacks an actual AI constructor");
   return;
  }
  if(!racer)return; // Human keeps its own controller, lap audio, RNG and driver policy.
  lapOwner.audioDispatch=nullptr;lapOwner.audioContext=nullptr;
  autoDriveEnabled=true;autoStopReported=false;courseState={};courseOutput={};
  // Distinct host-driver pace/lookahead only. Each native vehicle retains its
  // own authenticated engine, contact, drag, body and lift parameters.
  courseTuning.authoredSpeedOnly=true;
  const auto paceSteps=std::max<std::size_t>(1,racerBank.count()-2);
  const float pace=.94f+.06f*float(racer-1)/float(paceSteps);
  courseTuning.speedLimitToBodyScale=pace/
      std::bit_cast<float>(std::uint32_t{0x3f2e8bc1u});
  courseTuning.lookaheadBase=48.f+3.f*float((racer-1)%3);
  // Each state's source RNG remains independent after this explicit cold seed.
  if(!originalAiEnabled)s.noise.sharedRandomState^=0x9e3779b9u*static_cast<std::uint32_t>(racer);
  recoveryPaused=false;checkpointRequested=false;checkpointOutput={};
 });
};
