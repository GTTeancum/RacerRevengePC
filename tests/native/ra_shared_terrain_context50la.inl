//Actual Context-owned persistent mode1 scratch and immutable static target
//inventory. No new collect/projection, vector copy/allocation or Bank visit.
  producer->terrainLimited=[&,runtime](EndQueryBridge::SharedTerrainInvocation& call){
   auto& lease=call.lease_;auto& out=call.result_;auto& row=call.row_;
   if(!runtime->sharedTerrainLimitedCurrent(call)||call.limitedEntered_)return;
   call.limitedEntered_=true;
   struct Stop {EndQueryBridge& owner;~Stop(){owner.sharedEffectsStopped_=true;}} stopped{*runtime};
   auto* producer=endCandidateProducer;
   const auto& selected=runtime->sharedProbeSelection_;
   const auto count=out.secondCount<0?std::size_t(-1):static_cast<std::size_t>(out.secondCount);
   auto& metadata=terrainOwner.metadata;auto& qfilter=metadata.filter;
   out.stoppedPC=0x1226b0;
   if(!producer||runtime->endCandidateProducer_!=producer||producer->owner.lock().get()!=runtime.get()||
      !producer->inCall||producer->storage!=&trackStorage||producer->activeRevision!=out.projectionRevision||
      runtime->storage_.get()!=&trackStorage||&runtime->actual_->terrain!=&terrainOwner||
      &runtime->actual_->frame!=&s||row.canonicalFilter!=&qfilter||
      !out.secondReturned||!out.merged||count!=selected.count||selected.projection!=out.projectionRevision||
      count>selected.entries.size()||count>trackStorage.secondIds.size()||
      secondIds!=trackStorage.secondIds.data()||probeCount!=count||
      collectionSource!=trackStorage.source.data()||collectionObjects!=trackStorage.objects.data()||
      terrainContext.targets!=trackStorage.terrainTargets.data()||terrainContext.targetCount!=terrainOwnerCount||
      terrainContext.queryWork.treeStack!=terrainStack.data()||
      terrainContext.queryWork.treeStackCapacity!=terrainStack.size()||
      trackStorage.terrainTargets.size()!=terrainOwnerCount||trackStorage.candidates.size()!=terrainOwnerCount||
      !runtime->registeredRange(&s.terrainProbe,sizeof(s.terrainProbe))||
      !runtime->registeredRange(&s.bodies.main.components.probe348,sizeof(s.bodies.main.components.probe348))||
      !runtime->registeredRange(metadata.copied40.data(),sizeof(metadata.copied40))||
      !runtime->registeredRange(metadata.copied50.data(),sizeof(metadata.copied50)))return;
   for(std::size_t i=0;i<count;++i){
    call.ordinal_=i;out.stoppedPC=0x1226b0;
    if(!runtime->sharedTerrainLimitedCurrent(call)||selected.entries[i].identity!=secondIds[i])return;
    const auto& item=selected.entries[i];
    if(item.kind==EndQueryBridge::SharedSelectedKind::otherRoster){
     if(!runtime->processSharedTerrainNoHitRole(call,item))return;
     continue;
    }
    EndQueryBridge::SharedDirectPodView pod;
    std::optional<std::size_t> podSlot;
    Type2StaticCandidate49X* selectedOwner=nullptr;
    TerrainCandidate48J* staticMirror=nullptr;
    std::size_t staticIndex=0;
    if(item.kind==EndQueryBridge::SharedSelectedKind::staticCandidate){
     staticIndex=item.index;
     if(staticIndex>=terrainOwnerCount||trackStorage.candidates[staticIndex]!=&candidates[staticIndex]||
        trackStorage.terrainTargets[staticIndex]!=&candidates[staticIndex].candidate.collision)return;
     selectedOwner=&candidates[staticIndex];staticMirror=&movementCandidates[staticIndex].collision;
     const auto& target=selectedOwner->candidate.collision;
     if(target.shapeType!=selectedOwner->shapeType||target.compound!=selectedOwner->compound||
        target.filter.identity!=item.identity||staticMirror->filter.identity!=item.identity||
        collectionObjects[staticIndex].identity!=item.identity||
        collectionSource[staticIndex].flagsF8!=target.filter.flagsF8)return;
    }else if(item.kind==EndQueryBridge::SharedSelectedKind::pod694){
     if(item.index%track_runtime_storage50IG::Storage::rosterRoles!=3u)return;
     podSlot=item.index/track_runtime_storage50IG::Storage::rosterRoles;
     //Same held terrain lease, actual target-slot current pose/filter/source
     //resource and saved/live effect destinations. No nested Bank visit.
     if(!runtime->borrowSharedDirectPod(lease,*podSlot,pod)||
        pod.row->identity!=item.identity)return;
     selectedOwner=&pod.geometry;
    }else return; //Other real selected roles remain unsupported, not fake static indices.
    auto& owner=*selectedOwner;auto& target=owner.candidate.collision;
    if(target.typeTag!=metadata.collisionTypeTag){
     if(!(qfilter.flagsF8&1u)&&target.typeTag==metadata.groupTypeTag){out.stoppedPC=0x1228b4;return;}
     continue;
    }
    if(!runtime->markSharedDirectQueryEffect(lease))return;
    //1226CC current identity and1226E8 mark occur even for later rejection.
    out.stoppedPC=0x1226cc;
    s.terrainProbe.currentCandidateIdentity=target.filter.identity;
    out.stoppedPC=0x1226e8;
    target.filter.flagsF8=static_cast<std::uint8_t>(target.filter.flagsF8|0x10u);
    if(podSlot){
     pod.effect.flagsF8=target.filter.flagsF8;
     if(!runtime->publishSharedDirectPod(lease,pod,pod.effect))return;
    }else{
     staticMirror->filter.flagsF8=target.filter.flagsF8;
     collectionSource[staticIndex].flagsF8=target.filter.flagsF8;
    }
    out.stoppedPC=0x126eb0;bool overlaps=false;
    if(!evaluateType2CandidateSphere50CC(terrainOwner.initial.sphere,target.sphere,overlaps))return;
    if(!overlaps)continue;
    out.stoppedPC=0x120e70;
    if(podSlot){
     //Empty carrier vectors are not target list authority. The actual cold
     //immutable target view remains borrowed through this same owner lease.
     bool passes=false;
     if(!passType2BorrowedCandidateFilter50CC(qfilter,pod.filter,passes))return;
     if(!passes)continue;
    }else if(!passType2CandidateFilter50CC(qfilter,target.filter))continue;
    out.stoppedPC=0x121434;
    if(!runtime->actual_->normalMode.acceptReachedSourceDispatch50MB(
        metadata.normalMode54,metadata.normalMode54Bound))return;
    out.stoppedPC=0x12143c;
    if(metadata.normalMode54!=1u)return;
    //Actual shape2 caller122770->121410: target5 dispatches121884/
    //108E90, not type3's126F40/1270BC no-hit. Sphere owner/body unjoined.
    if(owner.shapeType==5u){out.stoppedPC=0x121884;return;}
    if(owner.shapeType>1u||(owner.shapeType==1u&&!owner.compound)||
       target.geometry.id!=target.filter.identity)return;
    //Existing pure mode1 ordered geometry, not atomic whole122660 or E4.
    //Its native rejection remains BEFORE claiming any geometry return.
    const CollisionWorkspace48M work{terrainStack.data(),terrainStack.size(),nullptr,0};
    CollisionResult48I geometry{};std::size_t traceUsed=0;
    const auto status=owner.shapeType==1u?
        queryCompoundPair50IJ(terrainOwner.initial.segment,*owner.compound,target.geometry.id,work,geometry):
        queryOrderedCollisionBounded48M(terrainOwner.initial.segment,&target.geometry,1,work,geometry,traceUsed);
    if(status==CollisionQueryStatus48I::Miss)continue;
    if(status!=CollisionQueryStatus48I::Hit)return;
    if(!runtime->commitSharedTerrainGeometry(call,geometry))return;
    out.stoppedPC=0x122848;
    std::memcpy(target.copied40.data(),metadata.copied40.data(),sizeof(target.copied40));
    std::memcpy(target.copied50.data(),metadata.copied50.data(),sizeof(target.copied50));
    if(podSlot){
     std::memcpy(pod.effect.copied40.data(),target.copied40.data(),sizeof(target.copied40));
     std::memcpy(pod.effect.copied50.data(),target.copied50.data(),sizeof(target.copied50));
     if(!runtime->publishSharedDirectPod(lease,pod,pod.effect))return;
    }else{
     std::memcpy(staticMirror->copied40.data(),target.copied40.data(),sizeof(target.copied40));
     std::memcpy(staticMirror->copied50.data(),target.copied50.data(),sizeof(target.copied50));
    }
    out.stoppedPC=0x122870;
    if(!runtime->applySharedTerrainCallback(call,target,podSlot))return;
    out.stoppedPC=0x12287c;
    if(target.hasResponseCallbackE8)return; //Prior geometry/copies/E4 stay committed.
   }
   call.ordinal_=count;runtime->finishSharedTerrainLimited(call);
  };
