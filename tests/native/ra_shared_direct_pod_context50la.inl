//Included at the real Context producer capture. Called only by the private
//shared typed limited loop; factory remains uncalled, ordinary path unchanged.
  producer->directPod=[&,runtime,mainRecordsForEnd](bool secondary,std::size_t targetSlot,
      const SourceQueryIdentity50MB& identity,const SourceOuterResultLease50MB& outer,
      EndQueryBridge::EndCandidateResult& out)->bool {
   auto* producer=endCandidateProducer;
   const auto lane=secondary?1u:0u;
   auto& query=secondary?secondaryQuery:mainQueryForEnd;
   auto& qfilter=secondary?secondaryFilter:mainFilterForEnd;
   auto& spring=secondary?secondarySpring:mainSpringForEnd;
   auto& initial=secondary?si:mainInitialForEnd;auto& pairs=secondary?sip:mainPairsForEnd;
   auto& metadata=secondary?sim:mainMetadataForEnd;
   auto& result=secondary?secondaryResult:mainResultForEnd;auto* records=secondary?secondaryRecords:mainRecordsForEnd;
   const auto& work=secondary?sw:mainWorkspaceForEnd;const auto& destination=secondary?sd:mainDestinationForEnd;
   const auto ordinal=secondary?out.nativeSecondaryLimitedOrdinal:out.nativeLimitedOrdinal;
   const auto& selected=runtime->sharedMergedSelection_;
   out.stoppedPC=0x1226b0;
   if(!producer||!producer->inCall||producer->owner.lock().get()!=runtime.get()||
      producer->storage!=&trackStorage||identity.issuer!=producer||identity.serial!=producer->activeRevision||
      out.projectionRevision!=identity.serial||!out.merged||out.mergedCount<0||
      !(secondary?out.nativeSecondaryLimitedInitialized:out.nativeLimitedInitialized)||
      (secondary?out.nativeSecondaryLimitedReturned:out.nativeLimitedReturned)||
      !(secondary?out.nativeSecondaryQueryPrefixReturned:out.nativeQueryPrefixReturned)||
      ordinal>=registeredCount||registeredCount!=static_cast<std::size_t>(out.mergedCount)||
      registeredCount>trackStorage.mergedIds.size()||mergedIds!=trackStorage.mergedIds.data()||
      query.filter!=&qfilter||qfilter.identity!=identity.queryCollider||
      &(secondary?runtime->actual_->secondary:runtime->actual_->main)!=&query||
      selected.projection!=identity.serial||selected.count!=registeredCount||
      ordinal>=selected.count||selected.entries[ordinal].kind!=EndQueryBridge::SharedSelectedKind::pod694||
      selected.entries[ordinal].index!=targetSlot*track_runtime_storage50IG::Storage::rosterRoles+3u||
      initial.pairs!=pairs.data()||initial.metadata!=metadata.data()||initial.pairSlots!=pairs.size()||
      metadata.size()!=pairs.size()||initial.recordSlots!=records||initial.recordCount!=(secondary?60u:40u)||
      !runtime->registeredRange(records,initial.recordCount*sizeof(*records))||
      !runtime->registeredRange(&result,sizeof(result))||!runtime->registeredRange(&initial,sizeof(initial))||
      !runtime->registeredRange(pairs.data(),pairs.size()*sizeof(pairs[0]))||
      !runtime->registeredRange(metadata.data(),metadata.size()*sizeof(metadata[0]))||
      !producer->nativeLimitedResultBorrow.outerCurrent(outer,identity,query,result,pairs.data(),metadata.data(),pairs.size())||
      !producer->nativeLimitedResultBorrow.bindOuterScratch(outer,work,destination,nullptr,0,records,initial.recordCount))return false;
   //mergedIds is actual shared producer scratch, deliberately UNBANKED. Its
   //custody is the same Storage pointer/inCall revision/outer lease above,
   //not a manufactured racer span registration.
   //Same query revision/ordinal is never replayed, even after a pre-callback
   //partial result. Different static candidates may lie between POD ordinals.
   if(producer->directPodSerial[lane]==identity.serial&&ordinal<=producer->directPodOrdinal[lane])return false;
   EndQueryBridge::SharedOwnerLease lease;
   if(!runtime->beginSharedOwnerLease(lease,out))return false;
   EndQueryBridge::SharedDirectPodView target;
   if(!runtime->borrowSharedDirectPod(lease,targetSlot,target)||
      mergedIds[ordinal]!=target.row->identity||selected.entries[ordinal].identity!=target.row->identity)return false;
   producer->directPodSerial[lane]=identity.serial;producer->directPodOrdinal[lane]=ordinal;
   if(!runtime->markSharedDirectQueryEffect(lease))return false;
   ++result.candidateCount; //Native diagnostic, not a guest store.
   query.initialQueryCurrentIdentity=target.row->identity;result.currentCandidateIdentity=target.row->identity;
   target.effect.flagsF8=static_cast<std::uint8_t>(target.effect.flagsF8|0x10u);
   if(!runtime->publishSharedDirectPod(lease,target,target.effect))return false; //1226E8 before sphere.
   out.stoppedPC=0x126eb0;bool overlap=false;
   if(!evaluateType2CandidateSphere50CC(query.sphere,target.geometry.candidate.collision.sphere,overlap))return false;
   if(!overlap)return runtime->completeSharedOwnerLease(lease);
   out.stoppedPC=0x120e70;bool passed=false;
   if(!passType2BorrowedCandidateFilter50CC(qfilter,target.filter,passed))return false;
   if(!passed)return runtime->completeSharedOwnerLease(lease);
   out.stoppedPC=0x121434;
   if(!runtime->actual_->normalMode.acceptReachedSourceDispatch50MB(query.normalMode54,query.normalMode54Bound))return false;
   recordCollisionNormalMode50JA(result.normalModeWrite,query.normalMode54);
   out.stoppedPC=0x12143c;
   Type2StaticResult49X previous{};std::memcpy(&previous,&result,sizeof(previous));
   previous.records=spring.records;previous.queryFlagsF8=qfilter.flagsF8;--previous.candidateCount;
   initial.records=spring.records;
   //Geometry does not read callback current-category. Preserve zero as an
   //unused numerical carrier; source category is resolved only at294C00.
   const Type2WorldContinuation50CC continuation{&previous,0,nullptr,&target.effect};
   Type2WorldGeometryOutput50CC geometry{};
   const auto prepared=prepareType2BorrowedWorldGeometryPrefix50CC(query,target.geometry,target.filter,
       initial,work,destination,continuation,geometry);
   if(prepared.phase!=Type2WorldGeometryPhase50CC::GeometryPrefixReady)return false;
   //Publish exact geometry/pair/selected/hit/copied effects BEFORE E4. These
   //writes survive every later body/material/record/E8 boundary.
   std::memcpy(pairs.data(),destination.pairs,pairs.size()*sizeof(pairs[0]));
   std::memcpy(metadata.data(),destination.metadata,metadata.size()*sizeof(metadata[0]));
   std::memcpy(records,destination.records,initial.recordCount*sizeof(*records));
   std::memcpy(&result,&geometry.numerical,sizeof(result));spring.records=result.records;initial.records=result.records;
   std::memcpy(&initial.selected,&result.selected,sizeof(initial.selected));
   qfilter.flagsF8=result.queryFlagsF8;query.initialQueryCurrentIdentity=result.currentCandidateIdentity;
   if(!runtime->publishSharedActiveQueryFlags(lease,secondary))return false;
   if(!runtime->publishSharedDirectPod(lease,target,destination.effects[0]))return false;
   if(!geometry.reached.hit)return runtime->completeSharedOwnerLease(lease);
   if(geometry.reached.queryE4Pending){
    ++result.callbacks;out.stoppedPC=0x294bd8;
    const auto category=target.filter.lists->categoryA4;
    if(!(category&query.skipMask)){
     std::uint32_t previousCategory=0;const auto previousId=spring.records.currentCandidate130;
     if(previousId){
      out.stoppedPC=0x294c00;bool found=false;
      for(std::size_t n=0;n<trackStorage.collectionInput.objectCount;++n){
       const auto& object=trackStorage.objects[n];
       if(object.identity!=previousId)continue;
       if(found)return false;previousCategory=object.filter.categoryA4;found=true;
      }
      if(!found)return false;
     }
     if(!previousId||(previousCategory&query.dynamicMask)){
      spring.records.currentCandidate130=target.row->identity;
      result.records=spring.records;initial.records=spring.records; //294C18, not rolled back.
     }
     out.stoppedPC=0x294c2c;
     if(!(category&query.dynamicMask))return false; //Not the represented direct POD branch.
     //Both branches start AFTER actual geometry/copies/currentCandidate.
     //Secondary uses its real retained native body/token and separate lanes;
     //target POD A0 remains the original authenticated main340 owner.
     const auto phase=secondary?runtime->applyNativeDynamicSecondaryBodyPhase(*target.owner):
         runtime->applyNativeDynamicMainBodyPhase(
             runtime->stagedPods694_[producer->activeSlot],*target.owner);
     using Cut=EndQueryBridge::NativeDynamicBodyCut;
     switch(phase.cut){
      case Cut::primaryLookup294C44:out.stoppedPC=0x294c44;return false;
      case Cut::otherProcessed294C68:out.stoppedPC=0x294c68;return false;
      case Cut::primaryContact294C7C:out.stoppedPC=0x294c7c;return false;
      case Cut::otherContact294C90:out.stoppedPC=0x294c90;return false;
      case Cut::material294D04:out.stoppedPC=0x294d04;return false;
      case Cut::pairRecords294DD4:break;
     }
     out.stoppedPC=0x294dd4;
     if(!phase.suppressPairRecords){
      if(result.pairCount>pairs.size()||!producer->bodyRegistry||!producer->bodyRegistry->prepared())return false;
      const auto other=producer->bodyRegistry->bodyIdentityForSlot(targetSlot);
      if(!other.value)return false;
      //294E0C..294E74 iterates ALL cumulative pairs, not only this hit's pairs.
      for(std::size_t i=0;i<result.pairCount;++i){
       out.stoppedPC=0x294e58;SpringRecordAppend48N append{};
       std::memcpy(&append.field00,&pairs[i].first,sizeof(append.field00));
       std::memcpy(&append.localPoint10,&pairs[i].second,sizeof(append.localPoint10));
       std::memcpy(&append.normal20,&pairs[i].normal,sizeof(append.normal20));
       std::memcpy(&append.surfacePoint30,&pairs[i].surfacePoint,sizeof(append.surfacePoint30));
       append.scene40=query.queryScene;append.other44={other.value};append.material4C=phase.material;
       const auto status=appendSpringRecord48N(append,records,initial.recordCount,spring.records);
       if(status!=SpringRecordStatus48N::Appended&&status!=SpringRecordStatus48N::Saturated)return false;
       result.records=spring.records;initial.records=spring.records;
       if(status==SpringRecordStatus48N::Appended)++result.appended;else ++result.saturated;
      }
     }
    }
   }
   out.stoppedPC=0x122878;
   //Actual native694 retained constructor response lane, never an unpublished
   //owner treated as guest null. No setter or alternate E8 service exists here.
   if(geometry.reached.targetE8Pending||
      target.owner->responseE8!=EndQueryBridge::NativePodResponseE8::constructorNull11B350)return false;
   out.stoppedPC=0x122894;
   return runtime->completeSharedOwnerLease(lease); //This candidate only, not122660/219810/Race return.
  };
