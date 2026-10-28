//Same Context-owned main354 state and actual40-slot generated allocation.
//No Post354 phase or atomic response coordinator is used.
  producer->springResponse=[&,runtime,mainRecordsForEnd](EndQueryBridge::SharedTerrainInvocation& call){
   auto& lease=call.lease_;auto& out=call.result_;auto* producer=endCandidateProducer;
   if(call.springEntered_||!call.aeroEntered_||out.stoppedPC!=0x219cdc||
      !runtime->sharedLeaseCurrent(lease))return;
   call.springEntered_=true;out.stoppedPC=0x2964c0;
   auto& spring=mainSpringForEnd;auto* records=mainRecordsForEnd;
   const auto* policy=trackStorage.nativeDetectorPolicy_.get();
   if(!producer||producer!=runtime->endCandidateProducer_||!producer->inCall||
      producer->owner.lock().get()!=runtime.get()||producer->storage!=&trackStorage||
      producer->activeRevision!=out.projectionRevision||!producer->activeSession||
      !policy||!policy->live_||!policy->registered_||policy->sharedRetired_||
      policy->sharedWorld_!=&trackStorage||policy->frame_!=&s||
      policy->bindings_!=&x.bindings||policy->entries_[0].spring!=&spring||
      policy->entries_[0].settings!=&settings||policy->entries_[0].query!=&mainQueryForEnd||
      runtime->actual_->main.filter!=&mainFilterForEnd||
      &runtime->actual_->main!=&mainQueryForEnd||&runtime->actual_->frame!=&s||
      !runtime->gridInputsRegistered(*runtime->gridArena_)||
      mainInitialForEnd.recordSlots!=records||mainInitialForEnd.recordCount!=40u||
      !runtime->registeredRange(&spring,sizeof(spring))||!runtime->registeredRange(&settings,sizeof(settings))||
      !runtime->registeredRange(records,40u*sizeof(*records))||
      spring.records.generatedCount84<0||spring.records.generatedCapacity88<0||
      spring.records.generatedCount84>spring.records.generatedCapacity88||
      spring.records.generatedCapacity88>40||std::fegetround()!=FE_TONEAREST)return;
   //Current source count/policy, not a constructor-empty assumption.2964C0
   //has NO pending38 early-return gate. Raw center cache precedes both lists.
   if(settings.pointContacts48||spring.records.generatedCount84>0){
    out.stoppedPC=0x29650c;BodyMatrix48G scene;
    if(!runtime->readSharedSpringRawScene(lease,scene))return;
    const auto& local=settings.impact.customCenter70?settings.impact.customLocalCenter100:
        x.parameters.bodies.main.centerOffset;
    Vector3_48C world;
    if(!prepareTerrainSeedPoint48M(scene,local,world)||!runtime->markSharedDirectQueryEffect(lease))return;
    spring.worldCenter110=world; //Native XYZ of actual296558/296598 point cache.
   }
   if(!runtime->observeSharedSpringSentinel(call,0))return;
   if(settings.pointContacts48&&spring.records.pointCount7C){
    out.stoppedPC=0x2965d4;return; //Actual point80 backing/scene owner remains unjoined.
   }
   if(!runtime->observeSharedSpringSentinel(call,1))return;
   //Actual source re-reads84 at each tail; occurrences/order are not flattened.
   for(std::int32_t i=0;i<spring.records.generatedCount84;++i){
    out.stoppedPC=0x296884;
    if(!runtime->sharedLeaseCurrent(lease)||spring.records.generatedCount84<0||
       spring.records.generatedCount84>spring.records.generatedCapacity88||
       spring.records.generatedCapacity88>40||i>=40)return;
    auto& record=records[i];BodyMatrix48G scene;
    //Existing native scene40 selector names this exact main query/body scene.
    //Zero is the actual callback's no-override selector, not missing owner.
    if(record.scene40.value&&(!mainQueryForEnd.queryScene.value||
       record.scene40.value!=mainQueryForEnd.queryScene.value))return;
    if(!runtime->readSharedSpringRawScene(lease,scene))return;
    //Native generated point domain is explicit. Do not infer opaque W=1.
    if(record.localPoint10.v[3]!=1.f)return;
    Vector3_48C local,normal,surface;
    std::memcpy(local.v,record.localPoint10.v,3*sizeof(float));
    std::memcpy(normal.v,record.normal20.v,3*sizeof(float));
    std::memcpy(surface.v,record.surfacePoint30.v,3*sizeof(float));
    SpringRecordGeometryPrefix48K geometry;
    if(!prepareSpringRecordGeometry48K(scene,local,normal,surface,geometry))return;
    if(!runtime->markSharedDirectQueryEffect(lease))return;
    if(geometry.depth<=0.f){
     out.stoppedPC=0x296bc0;record.flags50=static_cast<std::uint8_t>(record.flags50&~2u);continue;
    }
    out.stoppedPC=0x296994;spring.records.contact3C=1;
    out.stoppedPC=0x296998;spring.records.generated78=1;
    if(record.other44.value){out.stoppedPC=0x2969ac;spring.records.other44=1;}
    else if(normal.v[1]>.8f){out.stoppedPC=0x2969d4;spring.records.upward40=1;}
    out.stoppedPC=0x2969f0;record.flags50=static_cast<std::uint8_t>(record.flags50|2u);
    out.stoppedPC=0x2969f4;Vector3_48C point,velocity;
    if(!runtime->prepareSharedSpringVelocity(call,geometry,normal,record.other44.value,
        spring.worldCenter110,point,velocity)||!runtime->markSharedDirectQueryEffect(lease))return;
    spring.contactVelocity120=velocity; //Native XYZ; source full vector writes W=1.
    out.stoppedPC=0x296a60;
    if(record.other44.value&&settings.otherSpring58){
     out.stoppedPC=0x296a84;
     if(!runtime->applySharedSpringLoad(call,EndQueryBridge::SharedSpringLoadKind::force297190,
         normal,point,geometry.depth,record.other44.value,settings,&response,spring.contactVelocity120))return;
     out.stoppedPC=0x296a8c;
     if(settings.friction50){
      out.stoppedPC=0x296a98;
      if(record.material4C>=settings.frictionByMaterial.size())return;
      const float coefficient=settings.frictionByMaterial[record.material4C];
      out.stoppedPC=0x296ab4;
      if(!runtime->applySharedSpringLoad(call,EndQueryBridge::SharedSpringLoadKind::friction297390,
          normal,point,geometry.depth,record.other44.value,settings,nullptr,spring.contactVelocity120,coefficient))return;
     }
     continue; //Actual alternate-other branch bypasses primary impact/secondary/4C.
    }
    out.stoppedPC=0x296ac4;
    if(settings.impact54){
     out.stoppedPC=0x296ae0;
     if(!runtime->advanceSharedSpringImpact(call,settings,spring,normal,point,geometry.depth,record.other44.value))return;
    }
    out.stoppedPC=0x296ae8;
    if(settings.secondary68){
     out.stoppedPC=0x296b00;if(!runtime->applySharedSpringInwardVelocity(call,spring,normal))return;
    }
    out.stoppedPC=0x296b08;
    if(settings.springForce4C){
     out.stoppedPC=0x296b14;
     if(!runtime->registeredRange(&response,sizeof(response)))return;
     if(response.secondaryAtLimit60){
      out.stoppedPC=0x296b20;
      if(!std::isfinite(response.maxPenetration90))return;
      if(geometry.depth>=response.maxPenetration90&&!settings.impact54){
       //Source may call this AGAIN after68; do not deduplicate/recompute120.
       out.stoppedPC=0x296b4c;if(!runtime->applySharedSpringInwardVelocity(call,spring,normal))return;
      }
     }
     out.stoppedPC=0x296b68;
     if(!runtime->applySharedSpringLoad(call,EndQueryBridge::SharedSpringLoadKind::force297190,
         normal,point,geometry.depth,record.other44.value,settings,&response,spring.contactVelocity120))return;
    }
    out.stoppedPC=0x296b70;
    if(settings.friction50){
     out.stoppedPC=0x296b7c;
     if(record.material4C>=settings.frictionByMaterial.size())return;
     const float coefficient=settings.frictionByMaterial[record.material4C];
     out.stoppedPC=0x296b98;
     if(!runtime->applySharedSpringLoad(call,EndQueryBridge::SharedSpringLoadKind::friction297390,
         normal,point,geometry.depth,record.other44.value,settings,nullptr,spring.contactVelocity120,coefficient))return;
    }
    out.stoppedPC=0x296bc8; //Only this actual completed occurrence advances the loop.
   }
   if(!runtime->markSharedDirectQueryEffect(lease))return;
   out.stoppedPC=0x296be0;spring.records.pending38=0;
   out.stoppedPC=0x219cf0; //Actual main354 exhaustion, same original held caller.
   runtime->advanceSharedTerrainResponse(call);
  };
