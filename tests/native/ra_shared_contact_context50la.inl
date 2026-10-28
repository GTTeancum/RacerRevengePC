//Inside actual Context contact lambda, after real219420 prologue/getters.
    auto* producer=endCandidateProducer;
    if(!out.nativeContactStaticsEntered||!producer->activeSession||
       collectionWork.first.identities!=trackStorage.firstIds.data()||
       out.firstCount>static_cast<std::int32_t>(track_runtime_storage50IG::sourceSelectionCapacity))return;
    EndQueryBridge::SharedOwnerLease lease;if(!runtime->beginSharedOwnerLease(lease,out))return;
    //The original signed zero/negative branch still follows real statics and
    //1B4 getters above; this reads the actual first returned source count.
    if(out.firstCount<=0){
     if(!runtime->completeSharedOwnerLease(lease))return;
     out.nativeContactOrdinal=0;out.nativeContactReturned=true;out.stoppedPC=0x219c74;return;
    }
    EndQueryBridge::SharedSelectedList selected;
    if(!runtime->resolveSharedSelected(lease,collectionWork.first.identities,out.firstCount,selected))return;
    auto& spring=mainSpringForEnd;auto* records=mainRecordsForEnd;
    const auto* policy=trackStorage.nativeDetectorPolicy_.get();
    if(!policy||!policy->live_||!policy->registered_||policy->entries_[0].spring!=&spring||
       policy->entries_[0].query!=&mainQueryForEnd||&runtime->actual_->secondary!=&secondaryQuery||
       mainInitialForEnd.recordSlots!=records||mainInitialForEnd.recordCount!=40u||
       !runtime->registeredRange(&spring,sizeof(spring))||
       !runtime->registeredRange(&mainResultForEnd,sizeof(mainResultForEnd))||
       !runtime->registeredRange(&mainInitialForEnd,sizeof(mainInitialForEnd))||
       !runtime->registeredRange(records,40u*sizeof(*records)))return;
    for(std::size_t ordinal=0;ordinal<selected.count;++ordinal){
     out.nativeContactOrdinal=ordinal;out.stoppedPC=0x219500;
     const auto& item=selected.entries[ordinal];std::uint32_t category=0;
     if(item.kind==EndQueryBridge::SharedSelectedKind::staticCandidate){
      if(item.index>=terrainOwnerCount||trackStorage.candidates[item.index]!=&candidates[item.index]||
         candidates[item.index].candidate.collision.filter.identity!=item.identity)return;
      category=candidates[item.index].candidate.collision.filter.categoryA4;
     }else {
      if(item.index>=trackStorage.sharedRoster_->count*track_runtime_storage50IG::Storage::rosterRoles)return;
      const auto& row=trackStorage.sharedRoster_->objects[item.index];
      if(!row||row->identity!=item.identity||!row->coldFilter)return;
      category=row->coldFilter->view.categoryA4;
     }
     out.stoppedPC=0x219508;std::uint32_t mask=0;
     if(!runtime->readNativeContactPodCategory(app,mask))return;
     if(!(category&mask))continue;
     out.nativeContactMatchedIdentity=item.identity;out.stoppedPC=0x1202e0;
     if(item.kind!=EndQueryBridge::SharedSelectedKind::pod694||
        item.index%track_runtime_storage50IG::Storage::rosterRoles!=3u)return;
     const auto slot=item.index/track_runtime_storage50IG::Storage::rosterRoles;
     EndQueryBridge::SharedDirectPodView target;
     if(!runtime->borrowSharedDirectPod(lease,slot,target)||target.row->identity!=item.identity)return;
     Type0BoundsInput50EW boundsInput{};boundsInput.resource=target.row->type0Resource;
     boundsInput.composed00=target.row->membership.state.lastShapeWorld;
     const auto& cached=target.row->membership.state.bounds;
     std::memcpy(boundsInput.cachedCenterWords,cached.cachedCenter.v,sizeof(boundsInput.cachedCenterWords));
     std::memcpy(&boundsInput.cachedRadiusWord,&cached.cachedRadius,sizeof(boundsInput.cachedRadiusWord));
     CollectionBoundsResult48S bounds{};
     if(prepareType0Bounds50EW(boundsInput,bounds)!=CollectionBoundsStatus48S::Complete)return;
     out.stoppedPC=0x21952c;VehicleContactGeometryInput48R input{};
     std::memcpy(input.position.v,out.nativeContactPosition.data(),3*sizeof(float));
     std::memcpy(input.up.v,out.nativeContactUp.data(),3*sizeof(float));
     input.boundsHalf=bounds.half;input.cachedCenter=target.row->membership.state.bounds.cachedCenter;
     if(!runtime->readSharedContactScale(lease,app,input.radiusScaleWord)||
        !runtime->registeredRange(&secondaryQuery.sphere.radius,sizeof(secondaryQuery.sphere.radius)))return;
     input.vehicleRadius=secondaryQuery.sphere.radius; //Actual364->80, not candidate radius/body sphere.
     VehicleContactGeometryPrefix48R geometry{};
     if(!prepareVehicleContactGeometry48R(input,geometry))return;
     if(!geometry.hit)continue;
     SpringRecordAppend48N record{};record.normal20=geometry.normal;
     out.stoppedPC=0x2196f0;
     if(!runtime->prepareSharedContactLocalPoint(lease,input.position,record.field00))return;
     out.stoppedPC=0x219708;
     if(!runtime->prepareSharedContactLocalPoint(lease,geometry.vehicleSurface,record.localPoint10))return;
     out.stoppedPC=0x219710;
     if(!prepareVehicleContactCandidateSurface48R(input.cachedCenter,geometry.normal,
         geometry.candidateRadius,record.surfacePoint30))return;
     const auto& primary=runtime->stagedPods694_[producer->activeSlot];
     out.stoppedPC=0x219788;
     if(!runtime->markSharedDirectQueryEffect(lease))return;
     BodyContactListStatus48R status{};
     if(!runtime->applyNativeStagedBodyContact98(primary,*target.owner,status))return;
     out.stoppedPC=0x21979c;
     if(!runtime->applyNativeStagedBodyContact98(*target.owner,primary,status))return;
     out.stoppedPC=0x2197c0;
     if(!producer->bodyRegistry||!producer->bodyRegistry->prepared())return;
     const auto other=producer->bodyRegistry->bodyIdentityForSlot(slot);if(!other.value)return;
     record.other44={other.value};record.scene40={0};record.material4C=0; //Actual a6/a7 zero source operands.
     const auto appended=appendSpringRecord48N(record,records,40u,spring.records);
     if(appended!=SpringRecordStatus48N::Appended&&appended!=SpringRecordStatus48N::Saturated)return;
     //This is the same real main spring storage; mirror retained result state
     //only after the actual append. No source query/candidate loop is rerun.
     mainResultForEnd.records=spring.records;mainInitialForEnd.records=spring.records;
    }
    if(!runtime->completeSharedOwnerLease(lease))return;
    out.nativeContactOrdinal=selected.count;out.nativeContactReturned=true;out.stoppedPC=0x219c74;
