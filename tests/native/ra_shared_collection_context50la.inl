//Private shared-only original219AF0/219B14/219B1C source order. Legacy run,
//ordinary collector and factory entry are unchanged. Capture actual Context.
  producer->sharedRun=[&,runtime](EndQueryBridge::EndCandidateResult& out){
   auto* producer=endCandidateProducer;
   if(!producer||!producer->inCall||producer->storage!=&trackStorage||
      !trackStorage.sharedRoster_||gcAnimatedWorld||gaAnimatedWorld||zoneQueue.count||
      sourceFollowQueries||runtime->follow_|| //No unimplemented native Follow subscriber is skipped.
      prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete||
      !runtime->projectSharedRegistered(out))return;
   auto input=collectionInput;
   auto& activeMain=*trackStorage.sharedRoster_->objects[producer->activeSlot*track_runtime_storage50IG::Storage::rosterRoles];
   if(input.query!=activeMain.reservedRef||input.type2Resource!=query.resource)return;
   const auto publish=[&](bool second,const CandidateCollectionBuffers50BX& accepted,
                           const CandidateCollectionResult50BX& result){
    //Actual static flags remain shared, not restored from another racer Bank.
    for(std::size_t i=0;i<terrainOwnerCount;++i){
     candidates[i].candidate.collision.filter.flagsF8=accepted.objects[i].flagsF8;
     movementCandidates[i].collision.filter.flagsF8=accepted.objects[i].flagsF8;
    }
    query.initialQueryCurrentIdentity=result.currentIdentity;
    //Each source-returned query writes the actual same saved/live roster
    //flags and compact effects BEFORE another query may run. Failure retains
    //all prior source effects; Expire latches incomplete first publication.
    if(!runtime->publishSharedRosterFlags(second,out))return false;
    std::memcpy(collectionSource,accepted.objects,input.objectCount*sizeof(*collectionSource));
    std::memcpy(treeSource,accepted.trees,input.treeCount*sizeof(*treeSource));
    return true;
   };
   input.methodMask=2;input.skipSphere=true;input.ignoreGroups=true;input.nominalCapacity=64;
   CandidateCollectionResult50BX first{};out.stoppedPC=0x219af0;
   if(collectCandidatesBounded50BX(input,collectionInitial,collectionWork.collector,
       collectionWork.first,first)!=CandidateCollectionStatus50BX::Complete)return;
   out.firstReturned=true;out.firstCount=first.count;out.stoppedPC=0x219af8;
   if(!publish(false,collectionWork.first,first))return;
   out.stoppedPC=0x219b14;
   if(query.resource!=input.type2Resource||!activeMain.membership.state.attached||
      activeMain.membership.state.object!=input.query||
      activeMain.membership.state.lastType2Resource!=query.resource)return;
   input.methodMask=1;input.skipSphere=false;input.ignoreGroups=false;
   input.initialCurrentIdentity=first.currentIdentity;input.initialActiveTree=first.activeTree;
   CandidateCollectionState50BX afterFirst{};
   afterFirst.objects=collectionWork.first.objects;afterFirst.objectCapacity=collectionWork.first.objectCapacity;
   afterFirst.trees=collectionWork.first.trees;afterFirst.treeCapacity=collectionWork.first.treeCapacity;
   afterFirst.pools=collectionWork.first.pools;afterFirst.poolCapacity=collectionWork.first.poolCapacity;
   afterFirst.poolBlocks=collectionWork.first.poolBlocks;afterFirst.poolBlockCapacity=collectionWork.first.poolBlockCapacity;
   afterFirst.poolEntries=collectionWork.first.poolEntries;afterFirst.poolEntryCapacity=collectionWork.first.poolEntryCapacity;
   afterFirst.shape0States=collectionWork.first.shape0States;afterFirst.shape0Capacity=collectionWork.first.shape0Capacity;
   CandidateCollectionResult50BX second{};
   if(collectCandidatesBounded50BX(input,afterFirst,collectionWork.collector,
       collectionWork.second,second)!=CandidateCollectionStatus50BX::Complete)return;
   out.secondReturned=true;out.secondCount=second.count;out.stoppedPC=0x219b1c;
   if(!publish(true,collectionWork.second,second))return;
   CandidateMergeResult48S merged{};
   const CandidateMergeInput48S merge{
       {first.count,collectionWork.first.identities,collectionWork.first.identityCapacity},
       {second.count,collectionWork.second.identities,collectionWork.second.identityCapacity}};
   if(mergeCandidates48S(merge,collectionWork.mergeScratch,collectionWork.merged,merged)!=CandidateMergeStatus48S::Complete)return;
   EndQueryBridge::SharedOwnerLease lease;if(!runtime->beginSharedOwnerLease(lease,out))return;
   if(!runtime->resolveSharedSelected(lease,mergedIds,merged.count,runtime->sharedMergedSelection_)||
      !runtime->resolveSharedSelected(lease,secondIds,second.count,runtime->sharedProbeSelection_))return;
   //Native counts describe actual source outputs; legacy registeredIndices
   //and probeIndices are NOT filled with fabricated static-owner ordinals.
   registeredCount=static_cast<std::size_t>(merged.count);probeCount=static_cast<std::size_t>(second.count);
   if(!runtime->completeSharedOwnerLease(lease))return; //Only release current native map lease.
   out.mergedCount=merged.count;out.merged=true;out.stoppedPC=0x219bd0;
  };
