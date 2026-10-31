#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourcePostParticleModelsNative()const noexcept {
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::unreached)
        return !sourcePostParticleLoadingMode30_&&!sourceGeometryDirectoryStoreMask_&&!sourceEmptyInstancesReturnV0_&&
            !sourceStandardParentS3_&&!sourceStandardWrapperA0_;
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)
        return !sourceStandardParentS3_&&!sourceStandardWrapperA0_&&!sourceStandardTypeParentRead_;
    if(!sourceLoadApplication_||!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||sourceGeometryDirectoryStoreMask_>3)return false;
    if(sourceGeometryDirectoryStoreMask_&&(!sourcePostParticleLoadingMode30_||*sourcePostParticleLoadingMode30_))return false;
    if(sourceGeometryDirectoryStoreMask_==3&&sourceLoadApplication_->modelDirectory36B860_!="geometry\\")return false;
    if(sourceEmptyInstancesReturnV0_&&(*sourceEmptyInstancesReturnV0_!=1||!sourceEmptyInstancesCounterS1_||
       *sourceEmptyInstancesCounterS1_||!sourceEmptyInstancesCountRead_||*sourceEmptyInstancesCountRead_||
       !sourceEmptyInstancesPointerRead_||*sourceEmptyInstancesPointerRead_))return false;
    if(sourceStandardZoneStored_){
        if(!sourceStandardZoneS0_||*sourceStandardZoneS0_>10||!nativeModelZone354_||*nativeModelZone354_>=10||
           !sourceStandardZoneByteOffsetS1_||*sourceStandardZoneByteOffsetS1_!=0x160u*(*sourceStandardZoneS0_)||
           !sourceStandardZonePointerOffsetS2_||*sourceStandardZonePointerOffsetS2_!=4u*(*sourceStandardZoneS0_)||
           (*sourceStandardZoneS0_!=*nativeModelZone354_&&*sourceStandardZoneS0_!=*nativeModelZone354_+1u))return false;
    }else if(sourceStandardVisibilityMask_||sourceStandardVisibilityStored_)return false;
    if(sourceStandardVisibilityMask_>0x3ffu||
       (sourceStandardVisibilityMask_&(sourceStandardVisibilityMask_+1u)))return false;
    for(std::size_t z=0;z<10;++z){
        for(std::size_t collision=0;collision<2;++collision){const auto& n=sourceSearchNodes_[collision][z];
            if(!collision&&(sourceStandardVisibilityMask_&(1u<<z))){
                if(!sourceStandardZoneStored_||!sourceZones350_||sourceZonesLoaded_!=10||
                   !(sourceZones350_[z].knownFlags148&2u)||!n.zoneVisibility44||
                   *n.zoneVisibility44!=((sourceZones350_[z].flags148>>1)&1u))return false;
            }else if(n.zoneVisibility44)return false;
        }
        if(sourceStandardZoneS0_&&z<*sourceStandardZoneS0_&&!sourceStandardTypeReturned_[z])return false;
        if(nativeModelZone354_&&z>*nativeModelZone354_&&(sourceStandardCompletedCallbacks_[z]||sourceStandardTypeScanned_[z]||
           sourceStandardParentReturned_[z]||sourceStandardTypeReturned_[z]||sourceStandardDiagnosticAttempted_[z]))return false;
        const auto completed=sourceStandardCompletedCallbacks_[z],scanned=sourceStandardTypeScanned_[z];
        if(completed||scanned||sourceStandardParentReturned_[z]||sourceStandardTypeReturned_[z]||sourceStandardDiagnosticAttempted_[z]){
            if(!sourceStaticOwner_||z>=sourceStaticOwner_->parents_.size()||!sourceStaticOwner_->parents_[z])return false;
            const auto& parent=*sourceStaticOwner_->parents_[z];
            if(completed>parent.count30||scanned>parent.count30||completed>sourceStaticOwner_->sourceWrapperLedger_[z].size())return false;
            for(std::uint32_t i=0;i<completed;++i){const auto& w=sourceStaticOwner_->sourceWrapperLedger_[z][i];
                if(!w||!w->sourceModelCallbackEntered||!w->sourceCallbackReturnV0||*w->sourceCallbackReturnV0>1)return false;
            }
            for(std::uint32_t i=0;i<scanned;++i){const auto& w=sourceStaticOwner_->sourceWrapperLedger_[z][i];
                if(!w||!w->sourceTypeKnown||w->type00==2)return false; //No unimplemented PhysicsObject callback claimed.
            }
            if(sourceStandardDiagnosticAttempted_[z]&&(!(sourceStandardVisibilityMask_&(1u<<z))||completed!=parent.count30))return false;
            if(sourceStandardParentReturned_[z]&&(!parent.count30||completed!=parent.count30||!sourceStandardDiagnosticAttempted_[z]))return false;
            if(sourceStandardTypeReturned_[z]&&(scanned!=parent.count30||
               (parent.count30?!sourceStandardParentReturned_[z]:!sourceStandardDiagnosticAttempted_[z])))return false;
        }
    }
    if(sourceStandardParentS3_){
        if(!sourceStaticOwner_||!sourceStaticOwner_->sourceMode_||!nativeModelZone354_||
           *nativeModelZone354_>=sourceStaticOwner_->parents_.size()||
           sourceStandardParentS3_!=sourceStaticOwner_->parents_[*nativeModelZone354_].get()||!sourceStandardVisibilityStored_)return false;
    }
    if(sourceStandardWrapperA0_){
        if(!sourceStandardParentS3_||!sourceStandardWrapperSlot_||!sourceStandardSlotS0_||!sourceStandardSlotByteOffsetS1_||
           *sourceStandardSlotByteOffsetS1_!=4u*(*sourceStandardSlotS0_)||
           !nativeModelZone354_||!sourceStaticOwner_||*nativeModelZone354_>=10)return false;
        const auto z=*nativeModelZone354_,slot=*sourceStandardWrapperSlot_;
        if(slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size()||
           sourceStandardWrapperA0_!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||
           (*sourceStandardSlotS0_!=slot&&*sourceStandardSlotS0_!=slot+1u)||
           sourceStandardCompletedCallbacks_[z]!=*sourceStandardSlotS0_)return false;
    }
    if(sourceStandardTypeSlotS0_){
        if(!nativeModelZone354_||!sourceStandardTypeByteOffsetS1_||
           *sourceStandardTypeByteOffsetS1_!=4u*(*sourceStandardTypeSlotS0_)||
           sourceStandardTypeScanned_[*nativeModelZone354_]!=*sourceStandardTypeSlotS0_)return false;
    }
    if(sourceStandardTypeParentRead_||sourceStandardTypeCountRead_||sourceStandardTypeZoneRead_){
        if(!sourceStandardTypeSlotS0_||!sourceStandardTypeZoneRead_||!nativeModelZone354_||
           *sourceStandardTypeZoneRead_!=*nativeModelZone354_||!sourceStaticOwner_||
           *nativeModelZone354_>=sourceStaticOwner_->parents_.size()||!sourceStaticOwner_->parents_[*nativeModelZone354_])return false;
        const auto& p=*sourceStaticOwner_->parents_[*nativeModelZone354_];
        if(sourceStandardTypeParentRead_!=&p.entity||!sourceStandardTypeCountRead_||*sourceStandardTypeCountRead_!=p.count30)return false;
    }
    if(sourceStandardWorldReturnV0_){
        if(*sourceStandardWorldReturnV0_!=1||!sourceStandardZoneS0_||*sourceStandardZoneS0_!=10||sourceStandardVisibilityMask_!=0x3ff)return false;
        for(bool returned:sourceStandardTypeReturned_)if(!returned)return false;
    }
    return !sourceStandardCallbackEntered_||sourceStandardWrapperA0_;
}
bool Owner::abortSourcePostParticleModelsNative(std::uint32_t& pc)noexcept {
    if(!preflightSourcePostParticleModelsNative())return false;
    if(sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::unreached||
       sourcePostParticleModelsPhase_==SourcePostParticleModelsPhase::nativeClosed)return true;
    if(!retireSourceStandardModelsNative(pc))return false;
    //Borrowed stack identities retire before source parent/descriptor and node
    //owners. Do not restore the process directory or erase reached44/354 stores.
    sourceStandardWrapperA0_=nullptr;sourceStandardParentS3_=nullptr;sourceStandardTypeParentRead_=nullptr;
    sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::nativeClosed;pc=sourcePostParticleModelsPC_;return true;
}
bool Owner::continueSourcePostParticleModels1AA194(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostParticleModelsPhase_!=SourcePostParticleModelsPhase::unreached||
       sourcePostLightPhase_!=SourcePostLightPhase::particleReturned||sourcePostLightPC_!=0x1aa194||
       !sourceParticleStageReturnV0_||!sourceFogCallReturnV0_||*sourceFogCallReturnV0_!=1||
       !sourceLoadApplication_||!sourceLoadRace_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||!preflightSourceWorldTopologyCountNative()||
       !preflightSourceWorldInstanceMasterNative()||!preflightSourceWorldStaticModelsNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::bounded;return false;};
    sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::entered;
    at(0x1aa198);const auto& app=*sourceLoadApplication_;
    at(0x1aa19c);const auto& loading=app.sourceLoading330Owner_;
    if(!loading)return stop(0x1aa1a0);
    //Same constructor/initializer-produced Application330, not a native screen
    //or a readiness flag. The source30 read is retained even on nonzero branch.
    if(!loading->current()||loading->issuer_!=&app||!loading->managerInitialized_||
       !loading->slot330Published_||loading->retiring_||loading->retired_)return stop(0x1aa1a0);
    at(0x1aa1a0);sourcePostParticleLoadingMode30_=loading->mode30_;
    if(*sourcePostParticleLoadingMode30_)return stop(0x1aa1b8); //Real optional1CC900 remains unjoined.
    auto& directory=app.modelDirectory36B860_;
    //Native string backing preadmission only; source copies10 bytes to a raw
    //process lane. Existing storage must hold9 characters plus its terminator.
    //No allocation failure is passed off as an original branch or partial copy.
    if(directory.size()<9)return stop(0x1aa1d8);
    at(0x1aa1d8);std::memcpy(directory.data(),"geometry",8);sourceGeometryDirectoryStoreMask_|=1;
    at(0x1aa1dc);directory[8]='\\';directory.resize(9);sourceGeometryDirectoryStoreMask_|=2;
    at(0x1aa1ec);at(0x274080);
    if(sourceInstancePhase_!=SourceInstancePhase::returned||!nativeEmptyInstances3DC_||
       !nativeEmptyInstances3DC_->sourceMode||!nativeEmptyInstances3DC_->complete||
       !nativeEmptyInstances3DC_->sourceCallerPublished||!nativeInstanceCount3E0_||!nativeInstanceDescriptors3DC_)return stop(pc);
    at(0x27409c);sourceEmptyInstancesCounterS1_=0;
    at(0x274110);sourceEmptyInstancesCountRead_=*nativeInstanceCount3E0_;
    if(*sourceEmptyInstancesCountRead_!=0)return stop(0x2740a8); //No synthetic descriptor loop/adopted manager.
    at(0x274120);sourceEmptyInstancesPointerRead_=*nativeInstanceDescriptors3DC_;
    if(*sourceEmptyInstancesPointerRead_)return stop(0x274134); //No invented array allocation/deletion.
    at(0x274154);sourceEmptyInstancesReturnV0_=1;at(0x274158);
    at(0x1aa200);at(0x274178);
    if(!sourceModels3C4_||!*sourceModels3C4_||!sourceStaticOwner_||!sourceStaticOwner_->sourceAllDescriptorsReturned_||
       !sourceModelParents3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get()||sourceModelParentsPublished3C4_!=10||
       !worldZones34C_||*worldZones34C_!=10||!sourceZones350_||sourceZonesLoaded_!=10)return stop(pc);
    at(0x27418c);sourceStandardZoneS0_=0;at(0x274190);sourceStandardZoneByteOffsetS1_=0;
    at(0x274198);sourceStandardZonePointerOffsetS2_=0;
    while(*sourceStandardZoneS0_<*worldZones34C_){
        const auto z=*sourceStandardZoneS0_;if(z>=10)return stop(0x274220);
        at(0x27419c);nativeModelZone354_=z;sourceStandardZoneStored_=true;
        //Saved native argument receipts move to the new actual selected parent;
        //per-wrapper completion history remains in its distinct owned ledger.
        sourceStandardParentS3_=nullptr;sourceStandardWrapperA0_=nullptr;sourceStandardWrapperSlot_.reset();sourceStandardCallbackEntered_=false;
        sourceStandardSlotS0_.reset();sourceStandardSlotByteOffsetS1_.reset();
        sourceStandardTypeSlotS0_.reset();sourceStandardTypeByteOffsetS1_.reset();
        sourceStandardTypeZoneRead_.reset();sourceStandardTypeCountRead_.reset();sourceStandardTypeParentRead_=nullptr;
        at(0x2741ac);const auto& zone=sourceZones350_[z];if(!(zone.knownFlags148&2u))return stop(pc);
        at(0x2741b4);if(!sourceVisibility4C0_||!*sourceVisibility4C0_||!sourceSearchNodes_[0][z].owner||
           (*sourceVisibility4C0_)[z]!=sourceSearchNodes_[0][z].owner.get())return stop(pc);
        at(0x2741c0);sourceSearchNodes_[0][z].zoneVisibility44=(zone.flags148>>1)&1u;
        sourceStandardVisibilityStored_=true;sourceStandardVisibilityMask_|=static_cast<std::uint16_t>(1u<<z);
        at(0x2741cc);if(z>=sourceStaticOwner_->parents_.size())return stop(pc);const auto& parent=sourceStaticOwner_->parents_[z];
        if(!parent||sourceModelParents3C4_[z]!=&parent->entity||!parent->sourceMode||!parent->sourceCountKnown)return stop(pc);
        sourceStandardParentS3_=parent.get();at(0x2741d0);
        if(parent->count30>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return stop(pc);
        if(!parent->count30){
            at(0x2741e4);sourceStandardDiagnosticAttempted_[z]=true;
            sourceStandardDiagnosticWritten_[z]=std::fprintf(stdout,"Zone%d does not contain any static models.\n",static_cast<int>(z));
        }else{
            at(0x2741f8);at(0x27b950);sourceStandardSlotS0_=0;at(0x27b958);sourceStandardSlotByteOffsetS1_=0;
            while(*sourceStandardSlotS0_<parent->count30){
                const auto slot=*sourceStandardSlotS0_;at(0x27b964);
                if(!parent->sourceEntries34||slot>=parent->sourceEntriesKnown||
                   slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size())return stop(pc);
                auto* wrapper=parent->sourceEntries34[slot];
                if(!wrapper||wrapper!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get())return stop(pc);
                sourceStandardWrapperA0_=wrapper;sourceStandardWrapperSlot_=slot;sourceStandardCallbackEntered_=false;
                at(0x27b968);if(!wrapper->sourceMode||!wrapper->sourceTypeKnown)return stop(pc);
                if(wrapper->type00==2){
                    if(!sourceStaticOwner_->sourcePhysicsWrapperCurrentNative(*wrapper))return stop(pc);
                    at(0x27b970);sourceStandardCallbackEntered_=true;
                    auto& h=wrapper->sourcePhysics;
                    if(h.callbackEntered||!h.descriptor0C||!h.descriptorReadReturned||
                       worldEntity_.sessionGeneration!=sourceStaticOwner_->generation_||
                       sourceStaticOwner_->application_!=worldApplication_)return stop(0x27b3d0);
                    h.callbackEntered=true;at(0x27b3e0);const auto& descriptor=h.descriptor0C->common;
                    if(!descriptor.name180Known||descriptor.name180Known>descriptor.name180.size()||
                       descriptor.name180[descriptor.name180Known-1])return stop(0x27b3f8);
                    std::size_t length=0;while(length<descriptor.name180Known&&descriptor.name180[length])++length;
                    if(length==descriptor.name180Known||length>h.setupName.size()-5)return stop(0x27b3f8);
                    //Actual%s.slt stack formatting uses name180, NOT name00.
                    at(0x27b3f8);std::memcpy(h.setupName.data(),descriptor.name180.data(),length);
                    std::memcpy(h.setupName.data()+length,".slt",5);h.setupNameLength=length;h.nameFormatted=true;
                    auto& authority=worldApplication_->entityAuthority_;
                    if(authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                       !authority.reserveAdditional(1))return stop(0x27b400);
                    //Native control owner exists before the original160 allocation.
                    h.bodyCallEntered=true;h.bodyOwner.reset(new(std::nothrow)g::OriginalSourcePhysicsObject50LA);
                    if(!h.bodyOwner)return stop(0x27b400);
                    const auto made=h.bodyOwner->allocateSource27B400(worldApplication_,worldEntity_.sessionGeneration,h.body8);
                    if(auto* entity=h.bodyOwner->countedEntityNative()){
                        if(!authority.enroll(*entity))return stop(made.sourcePC);h.bodyEnrolled=true;
                    }
                    if(made.status!=g::OriginalSourcePhysicsObject50LA::Status::reached||made.sourcePC!=0x27b420u)
                        return stop(made.sourcePC);
                    h.bodyCallReturned=true;
                    //Source reads both actual selected topology arrays AFTER
                    //wrapper8 publication, even if the receiver is nowNULL.
                    at(0x27b420);if(!nativeModelZone354_)return stop(pc);h.collisionZone=*nativeModelZone354_;
                    at(0x27b424);if(!sourceCollision4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get())return stop(pc);
                    at(0x27b42c);h.terrainZero27B42C=true; //Actual caller t0=zero, distinct from selected collision.
                    at(0x27b43c);if(!*sourceCollision4C4_||*h.collisionZone>=sourceCollisionPublished_)return stop(pc);
                    h.collisionNode=(*sourceCollision4C4_)[*h.collisionZone];h.collisionRead=true;
                    at(0x27b444);if(!nativeModelZone354_)return stop(pc);h.visibilityZone=*nativeModelZone354_;
                    at(0x27b448);if(!sourceVisibility4C0_||*sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get())return stop(pc);
                    at(0x27b45c);if(!*sourceVisibility4C0_||*h.visibilityZone>=sourceVisibilityPublished_)return stop(pc);
                    h.visibilityNode=(*sourceVisibility4C0_)[*h.visibilityZone];h.visibilityRead=true;
                    //27B460/464/46C pass the SAME descriptor110/120/130;
                    //t0/t1 are zero, t2 is setupName, t3 visibility, stack collision.
                    at(0x27b468);h.setupEntered=true;
                    //Native admission precedes the genuine165C30 prefix;
                    //the source nullable body still reaches stack vector copies.
                    if(authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                       !authority.reserveAdditional(1))return stop(0x165c30);
                    h.nestedCallEntered=true;h.nestedModel.reset(new(std::nothrow)g::OriginalSourcePhysicsModel50LA);
                    if(!h.nestedModel)return stop(0x165c30);
                    const auto nested=h.nestedModel->beginSource165C30(*h.bodyOwner,descriptor.position110,
                        descriptor.look120,descriptor.up130,h.setupName.data());
                    if(auto* entity=h.nestedModel->countedModelEntityNative()){
                        if(!authority.enroll(*entity))return stop(nested.sourcePC);h.nestedModelEnrolled=true;
                    }
                    if(nested.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||nested.sourcePC!=0x165cc0u)
                        return stop(nested.sourcePC);
                    h.nestedCallReturned=true;
                    if(!h.terrainZero27B42C)return stop(0x165c50);
                    h.terrainSaved165C50=true; //Actual incomingt0→s3 save reached inside165C30.
                    if(!sourceLoadApplication_)return stop(0x165cc0);
                    h.nestedNameEntered=true;
                    const auto named=h.nestedModel->prepareSourceModelName26E848(
                        sourceLoadApplication_->modelDirectory36B860_,std::string_view(h.setupName.data(),h.setupNameLength+4));
                    if(named.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||named.sourcePC!=0x26e9acu)
                        return stop(named.sourcePC);
                    h.nestedNameReturned=true;at(0x26e9ac);
                    if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared()||
                       !sourceTrackFile_)return stop(pc);
                    const auto resolver=sourceTrackMountDomain_->resolver3E1A70();if(!resolver)return stop(pc);
                    h.nestedLookupEntered=true;a::OriginalStandardTextureDiagnostic50LA lookupDiagnostic;
                    const bool returned=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                        h.nestedModel->requestedPsg_,h.missingLookup,lookupDiagnostic);
                    if(!returned&&h.missingLookup&&h.missingLookup->found_&&
                       h.missingLookup->sourcePC_==0x132988u&&h.missingLookup->preflightNative()&&
                       !h.missingLookup->nativeClosed()){
                        //Authenticate native file crosswalk AFTER the actual
                        //source search; never replay132820 or treat provider
                        //absence as an original zero result.
                        const auto foundProvider=sourceTrackMountDomain_->archiveProvider3E1A70();
                        if(!foundProvider||foundProvider!=sourceTrackMountLease_.provider_)return stop(0x132988);
                        h.foundEntered=true;h.foundProvider=foundProvider;h.foundResolver=resolver;
                        const auto present=h.foundProvider->lookup132820(h.nestedModel->requestedPsg_,h.foundMember);
                        if(present!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
                           !h.foundMember.member||!h.foundMember.data||!h.foundMember.size)return stop(0x132988);
                        h.foundProviderResolved=true;a::OriginalStandardModelFileId50LA nativeId;
                        a::OriginalStandardTextureDiagnostic50LA foundDiagnostic;
                        try{std::vector<std::uint8_t> bytes(h.foundMember.data,h.foundMember.data+h.foundMember.size);
                            if(!resolver->bindModelMember(*h.foundMember.member,a::OriginalStandardModelFileKind50LA::psg,
                               bytes,nativeId,foundDiagnostic))return stop(0x132988);
                        }catch(...){return stop(0x132988);}
                        const bool nativeResolved=resolver->lookupModel132820(h.nestedModel->requestedPsg_,a::OriginalStandardModelFileKind50LA::psg,
                            h.foundNativeFile,foundDiagnostic);
                        h.foundGeometryHistory=h.foundNativeFile.geometry240110.get();
                        h.foundNativeFile.geometry240110.reset(); //No native strong resource adoption/retain, even on refusal.
                        if(!nativeResolved||h.foundNativeFile.id.resolver!=resolver.get()||
                           h.foundNativeFile.id.row!=nativeId.row||!h.foundNativeFile.bytes||
                           h.foundNativeFile.bytes->size()!=h.foundMember.size||
                           std::memcmp(h.foundNativeFile.bytes->data(),h.foundMember.data,h.foundMember.size))return stop(0x132988);
                        h.foundNativeResolved=true;h.foundCaptureEntered=true;
                        const bool opened=a::OriginalSourcePsgFile50LA::continueFound132988(h.missingLookup,
                            h.foundNativeFile.id,h.foundFile,foundDiagnostic);
                        if(!opened)return stop(foundDiagnostic.sourcePC?foundDiagnostic.sourcePC:0x132988u);
                        //Only genuinely cold output is currently issued here.
                        //AssociatedObject1C without exact warm creator remains
                        //the helper's bounded1329A0 path, pendingfile retained.
                        if(!h.foundFile||!h.foundFile->returnedCold()||!h.foundFile->current())return stop(0x1329e8);
                        h.foundHandle678=h.foundFile->sourceHandle();h.foundResource684=nullptr;
                        h.foundCaptureReturned=true;
                        at(0x26ebcc);
                        //Native capacity admission only: all actual source
                        //row constructors/read effects remain in28CAC0.
                        if(authority.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-(a::maxModelNodes49O-1)||
                           !authority.reserveAdditional(a::maxModelNodes49O-1))return stop(pc);
                        h.nestedRowsEntered=true;
                        const auto imported=h.nestedModel->importSourceRows28CAC0(h.foundResolver,h.foundNativeFile,h.foundFile);
                        //A constructor may count before source publication or
                        //type-registry refusal. Enrol every real child BEFORE
                        //inspecting returned/bounded status; Model0 is separate.
                        for(std::uint32_t row=1;row<a::maxModelNodes49O;++row){
                            auto* entity=h.nestedModel->countedChildEntityNative(row);if(!entity)continue;
                            if(h.nestedRowEnrolled[row]||!authority.enroll(*entity))return stop(imported.sourcePC);
                            h.nestedRowEnrolled[row]=entity;
                        }
                        if(imported.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||imported.sourcePC!=0x28cd5cu||
                           !h.nestedModel->rowsReturned_||!h.nestedModel->rowsReturnV0_)return stop(imported.sourcePC);
                        h.nestedRowsReturned=true;h.nestedRowsReturnV0=h.nestedModel->rowsReturnV0_;
                        at(0x26ed4c);at(0x26ed50);
                        if(!h.nestedModel->model_||(h.nestedModel->model_->fields_.flags1B3&4u))return stop(pc);
                        at(0x26ee18);
                        const auto& materialDomain=sourceLoadApplication_->standardMaterialDomainOwner_;
                        if(!materialDomain||materialDomain!=sourceTrackMountDomain_||!materialDomain->prepared()||
                           !sourceLoadApplication_->materialApplication)return stop(pc);
                        const auto materialTable=materialDomain->materials382B38();
                        if(!materialTable||!materialTable->sourceNames17CAD4Current()||
                           !materialTable->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return stop(pc);
                        const auto* materialPrefix=&sourceLoadApplication_->materialDirectory36B970();
                        a::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
                        h.nestedMaterialEntered=true;at(0x26ee1c);
                        const auto materials=h.nestedModel->loadSourceMaterials26EE18(materialTable,materialPrefix,materialDiagnostic);
                        if(materials.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||materials.sourcePC!=0x26e810u||
                           !h.nestedModel->materialReturned_)return stop(materials.sourcePC);
                        h.nestedMaterialReturned=true;at(0x26ee24);
                        h.nestedQuantizationEntered=true;at(0x26ee30);
                        const auto quantized=h.nestedModel->loadSourceQuantization26EE30();
                        if(quantized.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||quantized.sourcePC!=0x2406dcu||
                           !h.nestedModel->quantizationReturned_)return stop(quantized.sourcePC);
                        //Original caller ignores V0; actual reached-return is
                        //separate from any native helper success convention.
                        h.nestedQuantizationReturned=true;
                        at(0x26ee38);h.nestedGeometryEntered=true;
                        const auto geometry=h.nestedModel->allocateSourceGeometry26EE38();
                        if(geometry.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||geometry.sourcePC!=0x26ee64u||
                           !h.nestedModel->geometryAllocated_)return stop(geometry.sourcePC);
                        h.nestedGeometryAllocated=true;at(0x26ee64);h.nestedGeometryLoadEntered=true;
                        const auto lods=h.nestedModel->loadSourceGeometry26E170();
                        if(lods.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||lods.sourcePC!=0x24024cu||
                           !h.nestedModel->geometryLodsReturned_)return stop(lods.sourcePC);
                        h.nestedGeometryLodsReturned=true;at(0x24024c);h.nestedGeometryPaletteEntered=true;
                        const auto palette=h.nestedModel->finishSourceGeometry240210();
                        if(palette.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||palette.sourcePC!=0x240394u||
                           !h.nestedModel->geometryReturned_)return stop(palette.sourcePC);
                        h.nestedGeometryReturned=true;
                        at(0x26ee6c);h.nestedPrototypeEntered=true;
                        const auto prototype=h.nestedModel->publishSourceGeometryPrototype26EE74();
                        if(prototype.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||prototype.sourcePC!=0x26ee78u||
                           !h.nestedModel->prototypeReturned_)return stop(prototype.sourcePC);
                        h.nestedPrototypeReturned=true;
                        const auto& geometryOwner=h.nestedModel->model_->sourceGeometry_.owner;
                        a::OriginalStandardTextureDiagnostic50LA cacheDiagnostic;
                        at(0x26ee80);h.nestedCacheEntered=true;
                        if(!h.foundFile->associateGeometry1326A0(geometryOwner,h.nestedModel->model_.get(),cacheDiagnostic))
                            return stop(cacheDiagnostic.sourcePC?cacheDiagnostic.sourcePC:pc);
                        if(!h.foundFile->sourceAssociationStored()||!h.foundFile->nativeCacheAssociated()||
                           !h.foundFile->associatedGeometryCurrent(geometryOwner.get(),h.nestedModel->model_.get()))return stop(pc);
                        h.nestedCacheReturned=true;at(0x26ee88);
                        at(0x26ee90);h.nestedCloseEntered=true;
                        if(!h.foundFile->closeSource131040(cacheDiagnostic))
                            return stop(cacheDiagnostic.sourcePC?cacheDiagnostic.sourcePC:pc);
                        if(!h.foundFile->sourceCloseReturned())return stop(pc);
                        //Original archive131040 returns1 (delay1311C0), but
                        //26E820 ignores V0; native cancellation never mints it.
                        h.nestedCloseReturned=true;
                        at(0x26ee98);h.nestedRowTableEntered=true;
                        const auto rowTable=h.nestedModel->allocateClosedSourceRowTable26EE9C();
                        if(rowTable.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||rowTable.sourcePC!=0x26eee8u||
                           !h.nestedModel->rowTableReturned_||!h.nestedModel->model_||
                           !h.nestedModel->model_->preflightSourceRenderableReturn26E820Native())return stop(rowTable.sourcePC);
                        const auto* returnedEntity=h.nestedModel->model_->sourceRenderableEntity26E820Native();
                        if(!returnedEntity||returnedEntity!=h.nestedModel->renderableReturnedEntity_||
                           returnedEntity!=h.nestedModel->model_->wreckModelStorage())return stop(rowTable.sourcePC);
                        h.nestedRowTableReturned=true;h.nestedRenderableEntity=returnedEntity;
                        //Actual24075C sees nonNULL26E820 Entity+8; packet/flags
                        //suffix and final240740 return are distinct dependencies.
                        at(0x24076c);h.nestedPacketsEntered=true;
                        //Actual165CD8 delay slot passes raw a3=0 to240740;
                        //neither constructor a1=1 nor Standard dynamic144 applies.
                        const auto packets=h.nestedModel->constructSourcePackets240740();
                        if(packets.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||packets.sourcePC!=0x2407d8u||
                           !h.nestedModel->packetsReturned_||!h.nestedModel->loaderReturnedEntity_||
                           h.nestedModel->loaderReturnedEntity_!=h.nestedRenderableEntity)return stop(packets.sourcePC);
                        h.nestedPacketsReturned=true;h.nestedLoaderEntity=h.nestedModel->loaderReturnedEntity_;
                        h.nestedLoaderReturned=true;
                        //Actual165C30 ignores typed240740 V0. Only this
                        //genuinely live Model path may enter original141960;
                        //source-missing dead14C remains separately bounded.
                        at(0x165cdc);
                        if(!h.bodyOwner||!h.bodyEnrolled||!h.nestedModelEnrolled||
                           !h.bodyOwner->countedEntityNative()||!h.nestedModel->countedModelEntityNative()||
                           !authority.canRetire(*h.bodyOwner->countedEntityNative())||
                           !authority.canRetire(*h.nestedModel->countedModelEntityNative()))return stop(pc);
                        const auto attach=[&](g::OriginalWreckComponent50LA& parent,g::OriginalWreckComponent50LA& child,
                            std::uint32_t mask,std::uint32_t lock)noexcept {
                            return authority.attachChildren(parent,child,mask,lock);
                        };
                        h.nestedAttachEntered=true;at(0x165cf0);
                        const auto attached=h.nestedModel->attachSourceModel141960(attach);
                        if(attached.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||attached.sourcePC!=0x165cf8u||
                           !h.nestedModel->attachReturned_)return stop(attached.sourcePC);
                        h.nestedAttachReturned=true;
                        at(0x165cf8);h.nestedPlacementEntered=true;
                        const auto placed=h.nestedModel->placeSourceModel165CF8();
                        if(placed.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||placed.sourcePC!=0x165d94u||
                           !h.nestedModel->placementReturned_)return stop(placed.sourcePC);
                        h.nestedPlacementReturned=true;
                        at(0x165d94);
                        //Retained original165C30 arguments feed165E90:
                        //t0=actual selected visibility, t1=selected collision.
                        if(!h.visibilityRead||!h.collisionRead||!h.bodyOwner||!h.bodyOwner->object_||
                           !h.nestedModel->placementReturned_||h.sourceBodyD0||
                           authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        h.setup165E90Entered=true;
                        try{h.sourceBodyD0=std::make_unique<g::OriginalSourcePhysicsBody50LA>();}
                        catch(...){return stop(0x165e90);}
                        if(!h.terrainSaved165C50)return stop(0x165d98);
                        at(0x165d98);h.terrainForwarded165D98=true; //s3→a1, not B4/B8.
                        at(0x165eac);h.bodyOwner->sourceTerrain165E90_=nullptr;h.terrainSaved165EAC=true; //a1→s1.
                        at(0x165ebc);h.setupNoOpReturned=true; //Actual141C20 no-op, no retain.
                        at(0x165ec8);h.bodyOwner->object_->visibilityB4=h.visibilityNode;h.visibilityB4Stored=true;
                        at(0x165ed0);h.bodyOwner->object_->collisionB8=h.collisionNode;h.collisionB8Stored=true;
                        //B8 is the allocation-call delay slot, before nullable330.
                        h.sourceBodyCallEntered=true;
                        const auto body=h.sourceBodyD0->allocateSource165ECC(*h.bodyOwner);
                        if(auto* entity=h.sourceBodyD0->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(body.sourcePC);h.sourceBodyEnrolled=true;
                        }
                        if(body.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||body.sourcePC!=0x165eecu||
                           !h.sourceBodyD0->published_)return stop(body.sourcePC);
                        h.sourceBodyCallReturned=true;h.sourceBodyConfigEntered=true;at(0x165ef0);
                        const auto configured=h.sourceBodyD0->configureSource165EF8(*h.nestedModel);
                        if(configured.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||configured.sourcePC!=0x165f34u||
                           !h.sourceBodyD0->disableReturned_)return stop(configured.sourcePC);
                        h.sourceBodyConfigReturned=true;
                        at(0x165f34);h.sourceBodyAttachEntered=true;
                        const auto bodyAttached=h.sourceBodyD0->attachSourceBody141960(attach);
                        if(bodyAttached.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||bodyAttached.sourcePC!=0x165f44u||
                           !h.sourceBodyD0->attachReturned_)return stop(bodyAttached.sourcePC);
                        h.sourceBodyAttachReturned=true;at(0x165f44);
                        if(h.sourceGravityD4||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        try{h.sourceGravityD4=std::make_unique<g::OriginalSourcePhysicsGravity50LA>();}catch(...){return stop(pc);}
                        h.sourceGravityCallEntered=true;
                        const auto gravity=h.sourceGravityD4->allocateSource165F44(*h.bodyOwner,*h.sourceBodyD0);
                        if(auto* entity=h.sourceGravityD4->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(gravity.sourcePC);h.sourceGravityEnrolled=true;
                        }
                        if(gravity.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||gravity.sourcePC!=0x165f64u||
                           !h.sourceGravityD4->published_)return stop(gravity.sourcePC);
                        h.sourceGravityCallReturned=true;at(0x165f64);h.sourceGravityTargetEntered=true;
                        const auto target=h.sourceGravityD4->configureTargetSource165F70(*h.sourceBodyD0);
                        if(target.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||target.sourcePC!=0x165f8cu||
                           !h.sourceGravityD4->targetReturned_)return stop(target.sourcePC);
                        h.sourceGravityTargetReturned=true;at(0x165f8c);h.sourceGravityAttachEntered=true;
                        const auto gravityAttached=h.sourceGravityD4->attachSourceGravity141960(attach);
                        if(gravityAttached.status!=g::OriginalSourcePhysicsGravity50LA::Status::reached||gravityAttached.sourcePC!=0x165f9cu||
                           !h.sourceGravityD4->attachReturned_)return stop(gravityAttached.sourcePC);
                        h.sourceGravityAttachReturned=true;at(0x165f9c);h.collisionNameEntered=true;
                        //Actual distinct stack+60 scratch. Native256 capacity
                        //is an adapter bound; original strcpy is unbounded.
                        const auto setupLength=h.setupNameLength+4u;
                        if(setupLength>=h.collisionSetupName.size()||h.setupName[setupLength])return stop(0x165fa0);
                        std::memcpy(h.collisionSetupName.data(),h.setupName.data(),setupLength+1);
                        h.collisionSetupNameLength=setupLength;h.collisionNameCopied=true;
                        for(std::size_t i=setupLength;i>0;--i)if(h.collisionSetupName[i-1]=='.'){
                            h.collisionSetupDot=i-1;break;}
                        h.collisionDotSearched=true;at(0x166010);
                        if(!h.collisionSetupDot)return stop(pc); //Original NULLstrcpy destination, not append fallback.
                        if(*h.collisionSetupDot+sizeof("m.col")>h.collisionSetupName.size())return stop(pc);
                        std::memcpy(h.collisionSetupName.data()+*h.collisionSetupDot,"m.col",sizeof("m.col"));
                        h.collisionSetupNameLength=*h.collisionSetupDot+5u;h.collisionNameReturned=true;at(0x166018);
                        if(h.sourceCollisionE0||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                           !authority.reserveAdditional(1))return stop(pc);
                        try{h.sourceCollisionE0=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                        h.sourceCollisionCallEntered=true;
                        const auto collision=h.sourceCollisionE0->allocateSource166018(*h.bodyOwner,*h.sourceGravityD4);
                        if(auto* entity=h.sourceCollisionE0->countedEntityNative()){
                            if(!authority.enroll(*entity))return stop(collision.sourcePC);h.sourceCollisionEnrolled=true;
                        }
                        if(collision.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collision.sourcePC!=0x166038u||
                           !h.sourceCollisionE0->published_)return stop(collision.sourcePC);
                        h.sourceCollisionCallReturned=true;at(0x16603c);h.sourceCollisionConfigEntered=true;
                        const auto collisionConfig=h.sourceCollisionE0->configureSource16603C(*h.sourceBodyD0);
                        if(collisionConfig.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collisionConfig.sourcePC!=0x166074u||
                           !h.sourceCollisionE0->modeReturned_)return stop(collisionConfig.sourcePC);
                        h.sourceCollisionConfigReturned=true;at(0x166074);h.sourceCollisionNameEntered=true;
                        const auto collisionName=h.sourceCollisionE0->prepareSourcePath11B920(
                            std::string_view(h.collisionSetupName.data(),h.collisionSetupNameLength));
                        if(collisionName.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||collisionName.sourcePC!=0x11ba2cu||
                           !h.sourceCollisionE0->nameReturned_)return stop(collisionName.sourcePC);
                        h.sourceCollisionNameReturned=true;at(0x11ba2c);
                        if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackFile_||
                           sourceTrackMountDomain_!=sourceLoadApplication_->standardMaterialDomainOwner_||
                           sourceTrackMountDomain_->resolver3E1A70()!=resolver)return stop(pc);
                        h.sourceCollisionSearchEntered=true;
                        a::OriginalStandardTextureDiagnostic50LA collisionDiagnostic;
                        const bool collisionMissing=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                            h.sourceCollisionE0->lookupName_,h.collisionSearch,collisionDiagnostic);
                        if(!h.collisionSearch)return stop(collisionDiagnostic.sourcePC?collisionDiagnostic.sourcePC:pc);
                        //The retained generic search has no PSG file/open
                        //authority. COL pins it exclusively even on found/refusal.
                        const auto accepted=h.sourceCollisionE0->acceptSourceSearch11BA2C(h.collisionSearch,resolver.get(),sourceTrackFile_.get());
                        h.sourceCollisionSearchAccepted=h.sourceCollisionE0->searchAccepted_;
                        if(accepted.sourcePC==0x132988u&&h.sourceCollisionSearchAccepted&&h.collisionSearch->found_){
                            //Consume this SAME retained found occurrence; no
                            //replayed lookup or PSG/native-cache substitution.
                            h.sourceCollisionFoundEntered=true;
                            const auto found=h.sourceCollisionE0->continueSourceFound11BA34();
                            h.sourceCollisionFoundHeaderReturned=h.sourceCollisionE0->foundB0Stored_&&
                                h.sourceCollisionE0->headerValid_&&h.sourceCollisionE0->foundOutputReturned_&&found.sourcePC==0x124f80u;
                            if(!h.sourceCollisionFoundHeaderReturned)return stop(found.sourcePC);
                            at(0x124f80);h.sourceCollisionType0Entered=true;
                            const auto type0=h.sourceCollisionE0->continueSourceType0Prefix124F80();
                            if(type0.sourcePC!=0x125ab4u&&type0.sourcePC!=0x125ba0u)return stop(type0.sourcePC);
                            at(type0.sourcePC);h.sourceCollisionArraysEntered=true;
                            const auto arrays=h.sourceCollisionE0->continueSourceType0Arrays125AB4();
                            if(arrays.sourcePC!=0x12602cu&&arrays.sourcePC!=0x126030u)return stop(arrays.sourcePC);
                            at(arrays.sourcePC);h.sourceCollisionNodesEntered=true;
                            const auto nodes=h.sourceCollisionE0->continueSourceType0NodesFaces12602C();
                            if(nodes.status!=rr::game::OriginalSourcePhysicsCollision50LA::Status::reached||
                               nodes.sourcePC!=0x126410u)return stop(nodes.sourcePC);
                            h.sourceCollisionImportReturned=true;
                            at(0x125014);h.sourceCollisionResourceEntered=true;
                            const auto resource=h.sourceCollisionE0->continueSourceType0Resource125014();
                            if(resource.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               resource.sourcePC!=0x125488u||!h.sourceCollisionE0->loader124F80Returned_||
                               h.sourceCollisionE0->loader124F80ReturnV0_!=1)return stop(resource.sourcePC);
                            h.sourceCollisionPayloadReturned=true;at(0x11bb9c);h.sourceCollisionCloseEntered=true;
                            const auto close=h.sourceCollisionE0->closeSourceFound11BB9C();
                            if(close.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               close.sourcePC!=0x11bbbcu||!h.sourceCollisionE0->foundCloseReturned_||
                               !h.sourceCollisionE0->loader11B920Returned_||h.sourceCollisionE0->loader11B920ReturnV0_!=1)
                                return stop(close.sourcePC);
                            h.sourceCollisionLoaderReturned=true;
                            at(0x11bbe0);h.sourceCollisionResultEntered=true;
                            //Actual166078 supplies rawA3zero;11BBC0 ignores loaderV0.
                            const auto collisionResult=h.sourceCollisionE0->continueSourceResult11BBE0(0u);
                            if(collisionResult.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               collisionResult.sourcePC!=0x11bd30u)return stop(collisionResult.sourcePC);
                            h.sourceCollisionResultReturned=true;
                            at(0x16607c);h.sourceCollisionTransformEntered=true;
                            const auto transformed=h.sourceCollisionE0->continueSourceTransform11F570();
                            if(transformed.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               transformed.sourcePC!=0x11f678u||!h.sourceCollisionE0->transformReturned_)
                                return stop(transformed.sourcePC);
                            h.sourceCollisionTransformReturned=true;
                            at(0x166084);
                            if(!h.sourceCollisionEnrolled||!h.sourceCollisionE0->countedEntityNative()||
                               !authority.canRetire(*h.sourceCollisionE0->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceCollisionAttachEntered=true;at(0x166098);
                            const auto collisionAttached=h.sourceCollisionE0->attachSourceCollision141960(attach);
                            if(collisionAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               collisionAttached.sourcePC!=0x1660a0u||!h.sourceCollisionE0->attachReturned_)
                                return stop(collisionAttached.sourcePC);
                            h.sourceCollisionAttachReturned=true;
                            at(0x1660a0);
                            if(h.sourceContactDC||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceContactDC=std::make_unique<g::OriginalSourcePhysicsContact50LA>();}catch(...){return stop(pc);}
                            h.sourceContactCallEntered=true;
                            const auto contact=h.sourceContactDC->allocateSource1660A0(*h.bodyOwner,*h.sourceBodyD0,*h.sourceCollisionE0);
                            //Enroll every actual counted prefix before inspecting native/source outcome.
                            if(auto* entity=h.sourceContactDC->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(contact.sourcePC);h.sourceContactEnrolled=true;
                            }
                            if(contact.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               contact.sourcePC!=0x1660c0u||!h.sourceContactDC->published_)return stop(contact.sourcePC);
                            h.sourceContactCallReturned=true;at(0x1660c4);h.sourceContactCapacityEntered=true;
                            const auto capacity=h.sourceContactDC->configureCapacitySource295C20();
                            if(capacity.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               capacity.sourcePC!=0x1660ccu||!h.sourceContactDC->capacityReturned_)return stop(capacity.sourcePC);
                            h.sourceContactCapacityReturned=true;at(0x1660d0);h.sourceContactBodyEntered=true;
                            const auto body30=h.sourceContactDC->bindBodySource295C80(*h.sourceBodyD0);
                            if(body30.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               body30.sourcePC!=0x1660d8u||!h.sourceContactDC->bodyStored_)return stop(body30.sourcePC);
                            h.sourceContactBodyReturned=true;
                            at(0x1660d8);
                            if(!h.bodyOwner->object_||!h.bodyOwner->object_->modelKnown||
                               !h.bodyOwner->object_->model14C||!h.sourceContactEnrolled||
                               !h.sourceContactDC->countedEntityNative()||
                               !authority.canRetire(*h.sourceContactDC->countedEntityNative()))return stop(pc);
                            //Actual14C ModelBASE and E0 outer remain distinct from embedded Entity aliases.
                            const auto actualModel=h.bodyOwner->object_->model14C;
                            h.sourceContactPointsEntered=true;at(0x1660e4);
                            const auto points=h.sourceContactDC->selectSourcePoints296190(*h.sourceCollisionE0,1u,actualModel);
                            if(points.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               points.sourcePC!=0x2963d4u||!h.sourceContactDC->sourcePoints_.returned)return stop(points.sourcePC);
                            h.sourceContactPointsReturned=true;
                            at(0x1660ec);h.sourceContactScalarsEntered=true;
                            const auto scalars=h.sourceContactDC->continueSourceScalars1660EC();
                            if(scalars.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               scalars.sourcePC!=0x166160u||!h.sourceContactDC->suffixReturned_)return stop(scalars.sourcePC);
                            h.sourceContactScalarsReturned=true;at(0x166160);
                            if(!h.sourceContactEnrolled||!h.sourceContactDC->countedEntityNative()||
                               !authority.canRetire(*h.sourceContactDC->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceContactAttachEntered=true;at(0x166164);
                            const auto contactAttached=h.sourceContactDC->attachSourceContact141960(attach);
                            if(contactAttached.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                               contactAttached.sourcePC!=0x16616cu||!h.sourceContactDC->attachReturned_)return stop(contactAttached.sourcePC);
                            h.sourceContactAttachReturned=true;
                            at(0x16616c);
                            if(!h.terrainSaved165EAC||h.sourceDetectorD8||
                               authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceDetectorD8=std::make_unique<g::OriginalSourcePhysicsDetector50LA>();}catch(...){return stop(pc);}
                            h.sourceDetectorCallEntered=true;
                            const auto detector=h.sourceDetectorD8->allocateSource16616C(*h.bodyOwner,*h.sourceContactDC);
                            if(auto* entity=h.sourceDetectorD8->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(detector.sourcePC);h.sourceDetectorEnrolled=true;
                            }
                            if(detector.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detector.sourcePC!=0x16618cu||!h.sourceDetectorD8->published_)return stop(detector.sourcePC);
                            h.sourceDetectorCallReturned=true;at(0x1661a8);h.sourceDetectorConfigEntered=true;
                            //Original s1→a2 is the retained knownNULL above; a3/t0/t2 are actualzero selectors.
                            const auto detectorConfig=h.sourceDetectorD8->configureSource2953A0();
                            if(detectorConfig.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detectorConfig.sourcePC!=0x1661b0u||!h.sourceDetectorD8->configureReturned_)return stop(detectorConfig.sourcePC);
                            h.sourceDetectorConfigReturned=true;
                            at(0x1661b8);h.sourceDetectorEnableEntered=true;
                            const auto enabled=h.sourceDetectorD8->enableSource1661B8();
                            if(enabled.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               enabled.sourcePC!=0x1661bcu||!h.sourceDetectorD8->enabled64Stored_)return stop(enabled.sourcePC);
                            h.sourceDetectorEnableReturned=true;at(0x1661c0);h.sourceDetectorBindingEntered=true;
                            const auto binding=h.sourceDetectorD8->bindCollisionSource2955F0(*h.sourceCollisionE0);
                            if(binding.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               binding.sourcePC!=0x1661c8u||!h.sourceDetectorD8->bindingReturned_)return stop(binding.sourcePC);
                            h.sourceDetectorBindingReturned=true;at(0x1661c8);
                            if(!h.sourceDetectorEnrolled||!h.sourceDetectorD8->countedEntityNative()||
                               !authority.canRetire(*h.sourceDetectorD8->countedEntityNative())||
                               !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                            h.sourceDetectorAttachEntered=true;at(0x1661d0);
                            const auto detectorAttached=h.sourceDetectorD8->attachSourceDetector141960(attach);
                            if(detectorAttached.status!=g::OriginalSourcePhysicsDetector50LA::Status::reached||
                               detectorAttached.sourcePC!=0x1661d8u||!h.sourceDetectorD8->attachReturned_)return stop(detectorAttached.sourcePC);
                            h.sourceDetectorAttachReturned=true;
                            at(0x1661d8);h.directCollisionNameEntered=true;
                            //New originalSP+170 occurrence snapshots the saved setupName,
                            //not E0'sm.col scratch or the nested rewrittenPSG name.
                            const auto directSetupLength=h.setupNameLength+4u;
                            if(directSetupLength>=h.directCollisionSetupName.size()||h.setupName[directSetupLength])return stop(0x1661dc);
                            std::memcpy(h.directCollisionSetupName.data(),h.setupName.data(),directSetupLength+1);
                            h.directCollisionSetupNameLength=directSetupLength;h.directCollisionNameCopied=true;
                            for(std::size_t i=directSetupLength;i>0;--i)if(h.directCollisionSetupName[i-1]=='.'){
                                h.directCollisionSetupDot=i-1;break;}
                            h.directCollisionDotSearched=true;at(0x166248);
                            if(!h.directCollisionSetupDot)return stop(pc); //Actual NULL strcpy destination, no append fallback.
                            if(*h.directCollisionSetupDot+sizeof("f.col")>h.directCollisionSetupName.size())return stop(pc);
                            std::memcpy(h.directCollisionSetupName.data()+*h.directCollisionSetupDot,"f.col",sizeof("f.col"));
                            h.directCollisionSetupNameLength=*h.directCollisionSetupDot+5u;h.directCollisionNameReturned=true;at(0x166250);
                            if(h.sourceCollisionE4||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                               !authority.reserveAdditional(1))return stop(pc);
                            try{h.sourceCollisionE4=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                            h.sourceDirectCollisionCallEntered=true;
                            const auto direct=h.sourceCollisionE4->allocateSource166250(*h.bodyOwner,*h.sourceDetectorD8);
                            if(auto* entity=h.sourceCollisionE4->countedEntityNative()){
                                if(!authority.enroll(*entity))return stop(direct.sourcePC);h.sourceDirectCollisionEnrolled=true;
                            }
                            if(direct.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               direct.sourcePC!=0x166270u||!h.sourceCollisionE4->published_)return stop(direct.sourcePC);
                            h.sourceDirectCollisionCallReturned=true;at(0x166274);h.sourceDirectCollisionConfigEntered=true;
                            const auto directConfig=h.sourceCollisionE4->configureSource166274();
                            if(directConfig.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directConfig.sourcePC!=0x16628cu||!h.sourceCollisionE4->modeReturned_)return stop(directConfig.sourcePC);
                            h.sourceDirectCollisionConfigReturned=true;at(0x16628c);
                            if(!h.bodyOwner->object_||!h.bodyOwner->object_->modelKnown||!h.bodyOwner->object_->model14C)return stop(pc);
                            h.sourceDirectCollisionPathEntered=true;at(0x166294);
                            const auto directPath=h.sourceCollisionE4->prepareSourceDirectPath11B920(*h.sourceBodyD0,
                                h.bodyOwner->object_->model14C,std::string_view(h.directCollisionSetupName.data(),h.directCollisionSetupNameLength));
                            if(directPath.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directPath.sourcePC!=0x11ba2cu||!h.sourceCollisionE4->nameReturned_)return stop(directPath.sourcePC);
                            h.sourceDirectCollisionPathReturned=true;at(0x11ba2c);
                            if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackFile_||
                               sourceTrackMountDomain_!=sourceLoadApplication_->standardMaterialDomainOwner_||
                               sourceTrackMountDomain_->resolver3E1A70()!=resolver)return stop(pc);
                            h.sourceDirectCollisionSearchEntered=true;
                            a::OriginalStandardTextureDiagnostic50LA directDiagnostic;
                            const bool directMissing=a::OriginalSourcePsgMissingLookup50LA::capture132820(resolver,sourceTrackFile_,
                                h.sourceCollisionE4->lookupName_,h.directCollisionSearch,directDiagnostic);
                            if(!h.directCollisionSearch)return stop(directDiagnostic.sourcePC?directDiagnostic.sourcePC:pc);
                            const auto directAccepted=h.sourceCollisionE4->acceptSourceSearch11BA2C(h.directCollisionSearch,resolver.get(),sourceTrackFile_.get());
                            h.sourceDirectCollisionSearchAccepted=h.sourceCollisionE4->searchAccepted_;
                            if(directAccepted.sourcePC==0x132988u&&h.sourceDirectCollisionSearchAccepted&&h.directCollisionSearch->found_){
                                //Consume the SAME retained E4 found occurrence, never E0's file or cache.
                                h.sourceDirectCollisionFoundEntered=true;
                                const auto directFound=h.sourceCollisionE4->continueSourceFound11BA34();
                                h.sourceDirectCollisionHeaderReturned=h.sourceCollisionE4->foundB0Stored_&&
                                    h.sourceCollisionE4->headerValid_&&h.sourceCollisionE4->foundOutputReturned_&&directFound.sourcePC==0x124f80u;
                                if(!h.sourceDirectCollisionHeaderReturned)return stop(directFound.sourcePC);
                                at(0x124f80);h.sourceDirectCollisionType0Entered=true;
                                const auto directType0=h.sourceCollisionE4->continueSourceType0Prefix124F80();
                                if(directType0.sourcePC!=0x125ab4u&&directType0.sourcePC!=0x125ba0u)return stop(directType0.sourcePC);
                                at(directType0.sourcePC);h.sourceDirectCollisionArraysEntered=true;
                                const auto directArrays=h.sourceCollisionE4->continueSourceType0Arrays125AB4();
                                if(directArrays.sourcePC!=0x12602cu&&directArrays.sourcePC!=0x126030u)return stop(directArrays.sourcePC);
                                at(directArrays.sourcePC);h.sourceDirectCollisionNodesEntered=true;
                                const auto directNodes=h.sourceCollisionE4->continueSourceType0NodesFaces12602C();
                                if(directNodes.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directNodes.sourcePC!=0x126410u)
                                    return stop(directNodes.sourcePC);
                                h.sourceDirectCollisionImportReturned=true;at(0x125014);h.sourceDirectCollisionResourceEntered=true;
                                const auto directResource=h.sourceCollisionE4->continueSourceType0Resource125014();
                                if(directResource.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directResource.sourcePC!=0x125488u||
                                   !h.sourceCollisionE4->loader124F80Returned_||h.sourceCollisionE4->loader124F80ReturnV0_!=1)
                                    return stop(directResource.sourcePC);
                                h.sourceDirectCollisionPayloadReturned=true;at(0x11bb9c);h.sourceDirectCollisionCloseEntered=true;
                                const auto directClose=h.sourceCollisionE4->closeSourceFound11BB9C();
                                if(directClose.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||directClose.sourcePC!=0x11bbbcu||
                                   !h.sourceCollisionE4->foundCloseReturned_||!h.sourceCollisionE4->loader11B920Returned_||
                                   h.sourceCollisionE4->loader11B920ReturnV0_!=1)return stop(directClose.sourcePC);
                                h.sourceDirectCollisionCloseReturned=true;
                            }else {
                                if(directAccepted.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directAccepted.sourcePC!=0x11ba34u)return stop(directAccepted.sourcePC);
                                if(!directMissing||!h.directCollisionSearch->returnedMissing132820())return stop(directAccepted.sourcePC);
                                h.sourceDirectCollisionMissingEntered=true;
                                const auto directReturn=h.sourceCollisionE4->continueSourceMissing11BA34();
                                if(directReturn.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directReturn.sourcePC!=0x11bbbcu||!h.sourceCollisionE4->loader11B920Returned_)return stop(directReturn.sourcePC);
                            }
                            //Original E4 caller ignores genuine11B920V0, includingzero.
                            h.sourceDirectCollisionLoaderReturned=true;
                            at(0x16629c);h.sourceDirectCollisionLateEntered=true;
                            const auto directLate=h.sourceCollisionE4->publishSourceDirectCaller16629C();
                            if(directLate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                               directLate.sourcePC!=0x1662c0u||!h.sourceCollisionE4->directLateReturned_)return stop(directLate.sourcePC);
                            h.sourceDirectCollisionLateReturned=true;at(0x1662c0);
                            if(h.sourceDirectCollisionCloseReturned){
                                //Native dispatch to the exact qualified nonNULL source branch, not an original V0 condition.
                                h.sourceDirectCollisionFoundTransformEntered=true;
                                const auto directFoundTransform=h.sourceCollisionE4->beginSourceDirectFoundTransform11F570();
                                h.sourceDirectCollisionWorldCallReached=directFoundTransform.status==g::OriginalSourcePhysicsCollision50LA::Status::bounded&&
                                    directFoundTransform.sourcePC==0x28b550u&&h.sourceCollisionE4->directWorldCallReached_;
                                if(!h.sourceDirectCollisionWorldCallReached)return stop(directFoundTransform.sourcePC);
                                at(0x28b550);h.sourceDirectCollisionMatrixEntered=true;
                                const auto directMatrix=h.sourceCollisionE4->continueSourceDirectMatrix11F5B4();
                                if(directMatrix.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directMatrix.sourcePC!=0x11f678u||!h.sourceCollisionE4->directMatrix_.returned)return stop(directMatrix.sourcePC);
                                h.sourceDirectCollisionMatrixReturned=true;
                                at(0x1662c8);
                                if(!h.sourceDirectCollisionEnrolled||!h.sourceCollisionE4->countedEntityNative()||
                                   !authority.canRetire(*h.sourceCollisionE4->countedEntityNative())||
                                   !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                                h.sourceDirectCollisionAttachEntered=true;at(0x1662dc);
                                const auto directAttached=h.sourceCollisionE4->attachSourceCollision141960(attach);
                                if(directAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directAttached.sourcePC!=0x1662e4u||!h.sourceCollisionE4->attachReturned_)return stop(directAttached.sourcePC);
                                h.sourceDirectCollisionAttachReturned=true;
                                at(0x1662e4);
                                if(h.sourceCollisionE8||authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||
                                   !authority.reserveAdditional(1))return stop(pc);
                                try{h.sourceCollisionE8=std::make_unique<g::OriginalSourcePhysicsCollision50LA>();}catch(...){return stop(pc);}
                                h.sourceProceduralCollisionCallEntered=true;
                                const auto procedural=h.sourceCollisionE8->allocateSource1662E4(*h.bodyOwner,*h.sourceCollisionE4);
                                //Enroll every actual counted prefix before inspecting any source/native result.
                                if(auto* entity=h.sourceCollisionE8->countedEntityNative()){
                                    if(!authority.enroll(*entity))return stop(procedural.sourcePC);h.sourceProceduralCollisionEnrolled=true;
                                }
                                if(procedural.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   procedural.sourcePC!=0x166304u||!h.sourceCollisionE8->published_)return stop(procedural.sourcePC);
                                h.sourceProceduralCollisionCallReturned=true;
                                at(0x166308);h.sourceProceduralCollisionConfigEntered=true;
                                const auto proceduralMode=h.sourceCollisionE8->configureSource166308();
                                if(proceduralMode.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralMode.sourcePC!=0x166310u||!h.sourceCollisionE8->modeReturned_)return stop(proceduralMode.sourcePC);
                                h.sourceProceduralCollisionConfigReturned=true;
                                at(0x166310);
                                if(!h.sourceProceduralCollisionEnrolled||!h.sourceCollisionE8->countedEntityNative()||
                                   !authority.canRetire(*h.sourceCollisionE8->countedEntityNative())||
                                   !h.bodyOwner->object_||!authority.canRetire(h.bodyOwner->object_->entity))return stop(pc);
                                h.sourceProceduralCollisionAttachEntered=true;at(0x166324);
                                const auto proceduralAttached=h.sourceCollisionE8->attachSourceCollision141960(attach);
                                if(proceduralAttached.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralAttached.sourcePC!=0x16632cu||!h.sourceCollisionE8->attachReturned_)return stop(proceduralAttached.sourcePC);
                                h.sourceProceduralCollisionAttachReturned=true;
                                at(0x16632c);h.sourceProceduralCollisionLateEntered=true;
                                const auto proceduralLate=h.sourceCollisionE8->publishSourceProceduralCaller16632C();
                                if(proceduralLate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralLate.sourcePC!=0x166350u||!h.sourceCollisionE8->proceduralLateReturned_)return stop(proceduralLate.sourcePC);
                                h.sourceProceduralCollisionLateReturned=true;
                                at(0x166350);h.sourceProceduralCollisionShapeEntered=true;
                                const auto proceduralShape=h.sourceCollisionE8->beginSourceProceduralShape11C710();
                                h.sourceProceduralCollisionShapeResult=proceduralShape;
                                if(proceduralShape.status!=g::OriginalSourcePhysicsCollision50LA::Status::bounded||
                                   proceduralShape.sourcePC!=0x11c934u)return stop(proceduralShape.sourcePC);
                                at(0x11c934);h.sourceProceduralCollisionResultEntered=true;
                                const auto proceduralResult=h.sourceCollisionE8->continueSourceProceduralResult11C934();
                                h.sourceProceduralCollisionResultCall=proceduralResult;
                                if(proceduralResult.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   proceduralResult.sourcePC!=0x11ca80u||!h.sourceCollisionE8->proceduralResultReturned_)
                                    return stop(proceduralResult.sourcePC);
                                h.sourceProceduralCollisionResultReturned=true;
                                at(0x166358);
                                const auto filterCall=[&](unsigned index,std::uint32_t expectedPC,auto&& issue)noexcept {
                                    h.sourceLocalFilterEntered|=std::uint16_t(1u<<index);
                                    const auto result=issue();h.sourceLocalFilterCalls[index]=result;at(result.sourcePC);
                                    return result.status==g::OriginalSourcePhysicsCollision50LA::Status::reached&&result.sourcePC==expectedPC;
                                };
                                if(!filterCall(0,0x166364u,[&]()noexcept{return h.sourceCollisionE8->configureSourceCategoryMode124160();}))return stop(pc);
                                if(!filterCall(1,0x166374u,[&]()noexcept{return h.sourceCollisionE8->appendSourceCategory124170();}))return stop(pc);
                                if(!filterCall(2,0x166390u,[&]()noexcept{return h.sourceCollisionE8->enableSourceProceduralCollision16638C();}))return stop(pc);
                                if(!filterCall(3,0x16639cu,[&]()noexcept{return h.sourceCollisionE0->setSourceFilterMode123EA0(*h.sourceCollisionE8,*h.sourceCollisionE0);}))return stop(pc);
                                if(!filterCall(4,0x1663a8u,[&]()noexcept{return h.sourceCollisionE4->setSourceFilterMode123EA0(*h.sourceCollisionE8,*h.sourceCollisionE0);}))return stop(pc);
                                if(!filterCall(5,0x1663b4u,[&]()noexcept{return h.sourceCollisionE0->appendSourceExclusion123EB0(*h.sourceCollisionE4,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(6,0x1663c0u,[&]()noexcept{return h.sourceCollisionE0->appendSourceExclusion123EB0(*h.sourceCollisionE8,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(7,0x1663ccu,[&]()noexcept{return h.sourceCollisionE4->appendSourceExclusion123EB0(*h.sourceCollisionE0,*h.sourceCollisionE8);}))return stop(pc);
                                if(!filterCall(8,0x1663d8u,[&]()noexcept{return h.sourceCollisionE4->appendSourceExclusion123EB0(*h.sourceCollisionE8,*h.sourceCollisionE8);}))return stop(pc);
                                //Only original control epilogues remain after all genuine calls.
                                //1663D8..3F4 returns165E90;165DB0 explicitly returns same BASE.
                                h.sourceSetup165E90Returned=true;at(0x165db0);
                                if(!h.bodyOwner->preflightNative()||!h.bodyOwner->object_||!h.body8||
                                   *h.body8!=&h.bodyOwner->object_->entity)return stop(pc);
                                h.sourceSetupReturnedBase=&h.bodyOwner->object_->entity;
                                h.sourceSetup165C30Returned=true;at(0x27b470);
                                if(!h.sourceSetupReturnedBase)return stop(pc);
                                h.sourceSetupPositive27B470=true;
                                at(0x27b534);auto& config=h.configure166400;
                                if(config.entered)return stop(config.pc); //No replay of original argument/scratch prefix.
                                //Native exact-owner/knownness admission, before original caller stores.
                                if(!h.descriptor0C||h.descriptor0C->scalarKnown!=0x7ffu||
                                   !h.bodyOwner->object_||h.bodyOwner->nativeWithdrawn_||
                                   h.sourceSetupReturnedBase!=&h.bodyOwner->object_->entity||
                                   !h.body8||*h.body8!=h.sourceSetupReturnedBase||!h.nestedModel||
                                   h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   h.bodyOwner->object_->model14C!=&h.nestedModel->model_->model_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                const auto& descriptor=*h.descriptor0C;config.entered=true;config.descriptor=&descriptor;
                                config.receiver=*h.body8;config.nestedOwner=h.nestedModel.get();
                                at(0x27b538);const auto stackArgument=descriptor.scalar284[7];
                                at(0x27b53c);config.callerStack[0]=stackArgument;config.callerStackKnown|=1u;
                                for(unsigned i=1;i<6;++i){at(0x27b53cu+4u*i);config.callerStack[2*i]=0;
                                    config.callerStackKnown|=std::uint16_t(1u<<(2*i));}
                                //High32 halves of these six caller stack slots remain source-unknown.
                                at(0x27b55c);config.dispatch08=h.bodyOwner->object_->dispatch08;
                                for(unsigned i=1;i<8;++i){at(0x27b55cu+4u*i);
                                    config.argumentFpu[i]=descriptor.scalar284[i==7?8:i];config.argumentFpuKnown|=std::uint8_t(1u<<i);}
                                at(0x27b57c);config.target74=0x166400u; //Actual3CDF80+74 entry, not an alternate prepared method.
                                at(0x27b584);config.argumentFpu[0]=descriptor.scalar284[0];config.argumentFpuKnown|=1u;
                                config.argumentsPrepared=true;config.calleeEntered=true;at(0x166400);
                                //Original saves f27..f20 to stack1C..0 before replacing them; old values are unavailable, not zero.
                                config.savedOldFpuStores=0xffu;config.savedOldFpuKnown=0;
                                for(unsigned i=0;i<8;++i){at(0x166430u+4u*i);config.calleeFpu[7-i]=config.argumentFpu[i];
                                    config.calleeFpuKnown|=std::uint8_t(1u<<(7-i));}
                                const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
                                at(0x166454);config.center40=vf0;config.centerKnown=0x0fu;
                                at(0x16645c);config.half50=vf0;config.halfKnown=0x0fu;
                                at(0x166460); //Actual14D7F0 is an all-ZERO matrix constructor, not14D840 identity.
                                for(auto& word:config.scratchA0)word=0;
                                config.scratchKnown=0xffffu;config.scratchReturned=true;
                                at(0x166484);config.working60=config.scratchA0;config.workingKnown=0xffffu;config.scratchCopied=true;
                                at(0x166494);config.model14C=h.bodyOwner->object_->model14C;
                                config.boundsCallReached=true;config.pc=0x28d670u;
                                //Actual same-owner callee consumes the real stack40/50 temporaries.
                                if(h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   config.model14C!=&h.nestedModel->model_->model_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(config.pc);
                                config.boundsEntered=true;
                                const auto bounds=h.nestedModel->produceSourceLocalBounds28D670(config.center40,config.half50);
                                config.boundsResult=bounds;
                                if(bounds.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   bounds.sourcePC!=0x28d93cu||!h.nestedModel->localBounds_.returned)return stop(bounds.sourcePC);
                                config.boundsReturned=true;
                                at(0x1664a0);
                                if(!h.sourceBodyD0||!h.nestedModel||h.nestedModel->nativeWithdrawn_||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.dimensionsEntered=true;
                                const auto dimensions=h.sourceBodyD0->configureSourceDimensionsMass1664A0(
                                    *h.nestedModel,config.half50,config.argumentFpu[0]); //Actual f27=descriptor284 raw bits.
                                config.dimensionsResult=dimensions;
                                if(dimensions.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
                                   dimensions.sourcePC!=0x166558u||!h.sourceBodyD0->dimensionsMass_.returned||
                                   !h.sourceBodyD0->dimensionsMass_.massCallReturned)return stop(dimensions.sourcePC);
                                config.dimensionsReturned=true;
                                at(0x166558);
                                if(!h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.inertiaEntered=true;
                                const auto inertia=h.sourceBodyD0->configureSourceInertia166558(
                                    config.half50,config.working60,config.argumentFpu[1]); //Actual descriptor288/f26 raw bits.
                                config.inertiaResult=inertia;
                                if(inertia.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
                                   inertia.sourcePC!=0x166618u||!h.sourceBodyD0->inertia_.returned)return stop(inertia.sourcePC);
                                config.inertiaReturned=true;
                                at(0x166618);
                                if(!h.sourceContactDC||!h.bodyOwner||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                for(unsigned i=0;i<5;++i)config.contactArguments[i]=config.argumentFpu[i+2];
                                for(unsigned i=5;i<8;++i)config.contactArguments[i]=config.callerStack[0];
                                config.contactEntered=true;
                                const auto contact=h.sourceContactDC->configureSource295C90(config.contactArguments);
                                config.contactResult=contact;
                                if(contact.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                                   contact.sourcePC!=0x295dc8u||!h.sourceContactDC->tuning_.returned)return stop(contact.sourcePC);
                                config.contactReturned=true;at(0x166640);
                                config.frictionEntered=true;
                                const auto friction=h.sourceContactDC->frictionSource295DD0(config.argumentFpu[7]); //Actual signed index0.
                                config.frictionResult=friction;
                                if(friction.status!=g::OriginalSourcePhysicsContact50LA::Status::reached||
                                   friction.sourcePC!=0x295df4u||!h.sourceContactDC->tuning_.frictionReturned)return stop(friction.sourcePC);
                                config.frictionReturned=true;at(0x166650);
                                config.tailEntered=true;
                                const auto tail=h.bodyOwner->publishSourceConfigurationTail166650(
                                    *h.sourceContactDC,&config,config.callerStack,config.callerStackKnown);
                                config.tailResult=tail;
                                if(tail.status!=g::OriginalSourcePhysicsObject50LA::Status::reached||
                                   tail.sourcePC!=0x1666b8u||!h.bodyOwner->sourceConfigurationTail_.returned)return stop(tail.sourcePC);
                                config.returned74=true;
                                at(0x27b588);
                                if(!h.descriptor0C||config.descriptor!=h.descriptor0C.get()||!h.descriptor0C->vectorKnown||
                                   !h.bodyOwner||!h.sourceBodyD0||!h.sourceContactDC||
                                   !h.bodyOwner->sourceConfigurationTailReturnCurrentNative(*h.sourceContactDC,&config)||
                                   !h.sourceCollisionE0->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE4->sourceFilterLiveOwnerNative()||
                                   !h.sourceCollisionE8->sourceFilterLiveOwnerNative())return stop(pc);
                                config.updateEntered=true;
                                at(0x27b590);config.descriptorQuad=h.descriptor0C->vector2B0;config.descriptorQuadLoaded=true;
                                at(0x27b594);
                                if(!h.bodyOwner->object_->bodyD0)return stop(pc);
                                config.quadBodyD0=*h.bodyOwner->object_->bodyD0;
                                if(!config.quadBodyD0)return stop(0x27b598u);
                                if(config.quadBodyD0!=h.sourceBodyD0->publishedIdentity_)return stop(pc);
                                at(0x27b598);config.bodyQuadEntered=true;
                                const auto quad=h.sourceBodyD0->copySourceCenterOffset27B598(*h.sourceContactDC,&config,config.descriptorQuad);
                                config.bodyQuadResult=quad;
                                if(quad.status!=g::OriginalSourcePhysicsBody50LA::Status::reached||quad.sourcePC!=0x27b59cu||
                                   !h.sourceBodyD0->sourceCenterOffset_.returned)return stop(quad.sourcePC);
                                config.bodyQuadReturned=true;at(0x27b59c);
                                if(!h.body8||*h.body8!=config.receiver)return stop(pc);
                                config.updateDispatch08=h.bodyOwner->object_->dispatch08;
                                if(config.updateDispatch08!=0x3cdf80u)return stop(0x27b5a0u);
                                config.updateTarget78=0x1666c0u; //Actual original3CDFF8 dispatch word.
                                config.updateCallReached=true;config.updatePC=0x120290u;
                                at(config.updatePC);
                                g::OriginalSourcePhysicsCollision50LA* const unlinkOwners[2]={h.sourceCollisionE4.get(),h.sourceCollisionE0.get()};
                                for(unsigned index=0;index<2;++index){
                                    config.nullMembershipEntered|=std::uint8_t(1u<<index);
                                    const auto unlinked=unlinkOwners[index]->removeSourceNullMembership120290(&config);
                                    config.nullMembershipResults[index]=unlinked;
                                    if(unlinked.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                       unlinked.sourcePC!=0x1202d4u||!unlinkOwners[index]->nullMembership_.returned)return stop(unlinked.sourcePC);
                                    config.nullMembershipReturned|=std::uint8_t(1u<<index);
                                    at(index?0x1666e0u:0x1666d8u);
                                }
                                config.nullObserverEntered=true;
                                const auto observerRemoved=h.nestedModel->removeSourceNullObserver28EA80(&config);
                                config.nullObserverResult=observerRemoved;
                                if(observerRemoved.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   observerRemoved.sourcePC!=0x28eac4u||!h.nestedModel->nullObserver_.returned)return stop(observerRemoved.sourcePC);
                                config.nullObserverReturned=true;
                                at(0x1666e8);
                                //Actual a0=same Model14C,a1=NULL,a2=&same Base50. The callee
                                //owns the read after its process guard/parent branch; no eager raw input read here.
                                config.positionEntered=true;
                                const auto position=h.nestedModel->translateSourceNullRelative28ADD0(&config);
                                config.positionResult=position;
                                if(position.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   position.sourcePC!=0x28af90u||!h.nestedModel->translation_.returned)return stop(position.sourcePC);
                                config.positionReturned=true;
                                at(0x1666f8);
                                //Actual1666F8..166714: a0=same Model14C,a1=NULL,
                                //a2=&same Base60,a3=&same Base70,t0=0,t1=1,t2=0.
                                //The narrow callee owns raw Base reads after its independent
                                //process guard and saved-translation snapshot, not this caller.
                                at(0x166710);config.orientationEntered=true;
                                const auto orientation=h.nestedModel->orientSourceNullRelative28A090(&config);
                                config.orientationResult=orientation;
                                if(orientation.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
                                   orientation.sourcePC!=0x28a528u||!h.nestedModel->orientation_.returned)return stop(orientation.sourcePC);
                                config.orientationReturned=true;
                                at(0x166718);
                                //Actual16671C delay-slot argument is this BaseE4, not E0/E8.
                                //11FE50 owns a fresh28B550 getter and one11F680 pass;
                                //neither initialization getter nor either initialization pass is replayed.
                                config.directUpdateEntered=true;
                                const auto directUpdate=h.sourceCollisionE4->updateSourceDirectType0_11FE50(&config,
                                    [&](const void* updateOccurrence,const void* collisionOccurrence,
                                        std::array<std::uint32_t,16>& incoming,std::uint16_t& known)noexcept
                                        ->g::OriginalSourcePhysicsBody50LA::Result {
                                        const auto world=h.nestedModel->copySourcePostPoseWorld28B550(
                                            updateOccurrence,collisionOccurrence,incoming,known);
                                        using ModelStatus=g::OriginalSourcePhysicsModel50LA::Status;
                                        using BodyStatus=g::OriginalSourcePhysicsBody50LA::Status;
                                        return {world.status==ModelStatus::reached?BodyStatus::reached:
                                            world.status==ModelStatus::owner?BodyStatus::owner:BodyStatus::bounded,world.sourcePC};
                                    });
                                config.directUpdateResult=directUpdate;
                                if(directUpdate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   directUpdate.sourcePC!=0x11ff5cu||!h.sourceCollisionE4->postUpdate_.returned)return stop(directUpdate.sourcePC);
                                config.directUpdateReturned=true;
                                at(0x166720);
                                //Actual166724 delay-slot argument is this BaseE0. This distinct
                                //result-backed occurrence follows the genuine E4 control return;
                                //its new28B550 getter may copy the proven clean root, not replay E4.
                                config.resultUpdateEntered=true;
                                const auto resultUpdate=h.sourceCollisionE0->updateSourceResultType0_11FE50(
                                    &config,*h.sourceCollisionE4,
                                    [&](const void* updateOccurrence,const void* collisionOccurrence,
                                        std::array<std::uint32_t,16>& incoming,std::uint16_t& known)noexcept
                                        ->g::OriginalSourcePhysicsBody50LA::Result {
                                        const auto world=h.nestedModel->copySourcePostE4World28B550(
                                            updateOccurrence,collisionOccurrence,incoming,known);
                                        using ModelStatus=g::OriginalSourcePhysicsModel50LA::Status;
                                        using BodyStatus=g::OriginalSourcePhysicsBody50LA::Status;
                                        return {world.status==ModelStatus::reached?BodyStatus::reached:
                                            world.status==ModelStatus::owner?BodyStatus::owner:BodyStatus::bounded,world.sourcePC};
                                    });
                                config.resultUpdateResult=resultUpdate;
                                if(resultUpdate.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   resultUpdate.sourcePC!=0x11ff5cu||!h.sourceCollisionE0->postUpdate_.returned)return stop(resultUpdate.sourcePC);
                                config.resultUpdateReturned=true;
                                at(0x166728u);
                                //Actual E4 pointer reload, then typed BaseB8 at16672C.
                                //An absent optional is unknown, not sourceNULL; raw Fields
                                //storage cannot manufacture a guest pointer or this load.
                                if(!h.bodyOwner->object_||!h.sourceCollisionE4||!h.sourceCollisionE0||
                                   h.bodyOwner->object_->collisionE4!=std::optional<g::OriginalCollisionBaseOwner50LA*>(
                                       h.sourceCollisionE4->collision_.get()))return stop(0x166728u);
                                at(0x16672cu);
                                if(!h.bodyOwner->object_->collisionB8)return stop(0x16672cu);
                                config.directMembershipZone=*h.bodyOwner->object_->collisionB8;
                                //166734 delay slot supplies mode2; the later E0 registration also uses mode2.
                                config.directMembershipMode=2u;at(0x166730u);
                                config.directMembershipEntered=true;
                                const auto makeCollisionMethod=[&](bool resultBacked)noexcept {
                                    //Only these caller-owned occurrences can authorize callbacks;
                                    //capture the selected owner by value, never a mutable routing slot.
                                    auto* const target=resultBacked?h.sourceCollisionE0.get():h.sourceCollisionE4.get();
                                    return [&,target](const void* actualZone,const void* registrationOccurrence,
                                    const void* actualCOL,NativeListMethodCustody*& method,std::optional<std::uint32_t>& slot,
                                    bool& selectionPending,std::uint32_t& methodPC,bool select)noexcept {
                                    //Native same-world custody proof for the actual27B43C ->165ED0
                                    //collisionB8 argument. Never resolve it through visibility4C0/B4.
                                    methodPC=select?0x280afcu:0x27f5b4u;
                                    if(!h.collisionRead||!h.collisionB8Stored||!h.collisionZone||*h.collisionZone>=10||
                                       !h.bodyOwner||!h.bodyOwner->object_||!h.bodyOwner->object_->collisionB8||
                                       actualZone!=*h.bodyOwner->object_->collisionB8||actualZone!=h.collisionNode||
                                       !target||actualCOL!=target->collision_.get()||
                                       registrationOccurrence!=&target->membershipRegistration_||
                                       !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
                                       *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||!sourceCollisionSlots4C4_||
                                       *h.collisionZone>=sourceCollisionPublished_)return false;
                                    const auto zone=*h.collisionZone;auto& node=sourceSearchNodes_[1][zone];
                                    if(node.withdrawn||!node.published||!node.owner||!node.ordinalKnown||
                                       sourceCollisionSlots4C4_[zone]!=node.owner.get()||actualZone!=node.owner.get()||
                                       !preflightSourceSearchNodeNative(node,zone,true))return false;
                                    //Mode2 follows27F5B4, selecting collision dynamic method kind3
                                    //(World4D4), NOT collision static kind1/World4CC.
                                    if(!select)method=node.owner->nativeList38;
                                    if(method!=node.owner->nativeList38||!method||!nativeMethods4D4_||
                                       nativeMethods4D4_[zone]!=method||!sourceListMethodArrayCurrent(3,zone,method))return false;
                                    const auto& publication=sourceListMethods_[3][zone];
                                    if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
                                       publication.withdrawn||publication.aliasNode!=node.owner.get()||
                                       !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       method->application_!=worldApplication_.get()||method->ordinal_!=30u+zone||
                                       method->usage10_!=2||method->reference00_!=2||
                                       !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
                                       !sourceListEntriesCurrent(*method))return false;
                                    if(!select)return true;
                                    //Actual3D2D30+20=27E3B0 tailcalls27E3C0 with a3(NULL)
                                    //as requested slot. +24=27E570 is owned by COL insertion.
                                    return selectSourceObserverSlot27E3C0(*method,registrationOccurrence,actualCOL,
                                        slot,selectionPending,methodPC,2u);
                                };
                                };
                                const auto makeCollisionPortal=[&](bool resultBacked)noexcept {
                                    //Only these caller-owned occurrences can authorize callbacks;
                                    //capture the selected owner by value, never a mutable routing slot.
                                    auto* const target=resultBacked?h.sourceCollisionE0.get():h.sourceCollisionE4.get();
                                    return [&,target](unsigned phase,const void* registration,const void* actualCOL,
                                    const void* actualZone,const std::array<std::uint32_t,4>& center,const void* cursor,
                                    auto& edge,std::shared_ptr<const void>& graphPin,std::uint32_t& portalPC)noexcept {
                                    portalPC=phase==0?0x280c34u:phase==2?0x280c90u:phase==4?0x280cf8u:0x280c40u;
                                    if(phase>4||!target||registration!=&target->membershipRegistration_||
                                       actualCOL!=target->collision_.get()||!h.collisionRead||!h.collisionB8Stored||
                                       !h.collisionZone||*h.collisionZone>=10||!h.bodyOwner||!h.bodyOwner->object_||
                                       !h.bodyOwner->object_->collisionB8||actualZone!=*h.bodyOwner->object_->collisionB8||
                                       actualZone!=h.collisionNode||center!=target->membershipRegistration_.center||
                                       !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceCollision4C4_||
                                       *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||!sourceCollisionSlots4C4_||
                                       *h.collisionZone>=sourceCollisionPublished_)return false;
                                    const auto zone=*h.collisionZone;auto& root=sourceSearchNodes_[1][zone];
                                    if(root.withdrawn||!root.published||!root.owner||actualZone!=root.owner.get()||
                                       sourceCollisionSlots4C4_[zone]!=root.owner.get()||
                                       !preflightSourceSearchNodeNative(root,zone,true)||
                                       !sourcePortalWorldLinksCurrent(*root.owner,true,zone))return false;
                                    if(phase==0){
                                        if(graphPin||cursor)return false;
                                        for(const auto& old:sourceCollisionPortalBorrows_)
                                            if(old.registration==registration&&!old.token.expired())return false;
                                        //Allocate native custody before exposing the source head pointer.
                                        //Weak root records never keep a completed COL borrower alive.
                                        try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
                                            sourceCollisionPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
                                        }catch(...){return false;}
                                        edge.next=root.owner->portalLinks30.get();return true;
                                    }
                                    bool pinned=false;
                                    for(const auto& borrow:sourceCollisionPortalBorrows_)if(borrow.registration==registration){
                                        const auto live=borrow.token.lock();if(live&&live==graphPin){pinned=true;break;}}
                                    if(!pinned||!graphPin||!cursor)return false;
                                    std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=cursor)++index;
                                    //Odd links are collision namespace (stages1/3), never visibility0/2.
                                    if(index==40||!(index&1u))return false;
                                    auto& link=sourcePortalWorldLinks_[index/4][index%4];
                                    if(link.withdrawn||link.knownMask!=31||!link.published||link.from!=root.owner.get()||!link.to)return false;
                                    auto& raw=*link.published;const auto& portal=nativePortals_[index/4];
                                    if(!portal||!portal->sourceMode||raw.neighbor04!=&link.to->entity||
                                       raw.portal08!=&portal->entity||!portal->source.loadReturned||
                                       portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return false;
                                    for(unsigned i=0;i<4;++i)if(portal->source.edgeKnown[i]!=7||!portal->source.edgeNormalized[i])return false;
                                    if(phase==1){
                                        edge.entered=true;edge.edge=cursor;edge.edgeIndex=index;
                                        edge.neighbor=raw.neighbor04;edge.portal=raw.portal08;
                                        //Actual231150 uses center and f12=+0, not half/radius80.
                                        portalPC=0x231150u;
                                        const bool overlapOkay=g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
                                            portal->source.edges80,center,0u,edge.overlap);
                                        edge.overlapReturned=edge.overlap.returned;edge.hit=edge.overlap.hit;edge.distance=edge.overlap.outputDistance;
                                        if(!overlapOkay){portalPC=edge.overlap.pc;return false;}
                                        portalPC=0x280c54u;if(!edge.hit)return true;
                                        if(!edge.distance)return false;
                                        portalPC=0x280c60u;raw.field10=*edge.distance;link.distance10=edge.distance;edge.distanceStoreMask|=1u;
                                        portalPC=0x280c64u;raw.field0C=*edge.distance;link.distance0C=edge.distance;edge.distanceStoreMask|=2u;
                                    }else if(edge.edge!=cursor||edge.edgeIndex!=index||edge.neighbor!=raw.neighbor04||edge.portal!=raw.portal08||
                                             !g::source_portal_overlap231150::historyCurrent(edge.overlap))return false;
                                    if(phase==4){edge.next=raw.next00.get();portalPC=0x280cf8u;return true;}
                                    if(phase==3&&!edge.hit)return true;
                                    //The source keeps a method cursor: each hit reads its usage10,
                                    //then resolves the neighbor method using that same mode2.
                                    portalPC=phase==1?0x280c6cu:portalPC;
                                    const auto* prior=edge.previousMethod;
                                    if(!prior||!prior->sourceMode_||prior->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       prior->application_!=worldApplication_.get()||prior->usage10_!=2||prior->ordinal_<30||prior->ordinal_>=40||
                                       !sourceListMethodArrayCurrent(3,prior->ordinal_-30,prior))return false;
                                    std::size_t neighborZone=0;while(neighborZone<10&&sourceCollisionSlots4C4_[neighborZone]!=link.to)++neighborZone;
                                    if(neighborZone==10||neighborZone>=sourceCollisionPublished_)return false;
                                    auto& neighbor=sourceSearchNodes_[1][neighborZone];
                                    if(neighbor.withdrawn||!neighbor.published||neighbor.owner.get()!=link.to||
                                       !preflightSourceSearchNodeNative(neighbor,neighborZone,true))return false;
                                    if(phase==1){portalPC=0x27f5b4u;edge.method=link.to->nativeList38;edge.methodReturned=true;}
                                    auto* method=edge.method;
                                    if(!method||method!=link.to->nativeList38||!nativeMethods4D4_||nativeMethods4D4_[neighborZone]!=method||
                                       !sourceListMethodArrayCurrent(3,neighborZone,method)||method->usage10_!=2||method->reference00_!=2||
                                       !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
                                       method->application_!=worldApplication_.get()||method->ordinal_!=30u+neighborZone||
                                       !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
                                       !sourceListEntriesCurrent(*method))return false;
                                    const auto& publication=sourceListMethods_[3][neighborZone];
                                    if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
                                       publication.withdrawn||publication.aliasNode!=link.to)return false;
                                    if(phase==2){portalPC=0x280c90u;
                                        if(!edge.overlap.returned||!edge.overlap.hit||edge.distanceStoreMask!=3u||!edge.methodReturned)return false;
                                        return selectSourceObserverSlot27E3C0(*method,&edge,actualCOL,
                                            edge.slot,edge.selectionPending,portalPC,2u);}
                                    return phase==1||phase==3;
                                };
                                };
                                const auto membership=h.sourceCollisionE4->registerSourceDirectMembership120070(
                                    &config,*h.sourceCollisionE0,*config.directMembershipZone,config.directMembershipMode,makeCollisionMethod(false),makeCollisionPortal(false));
                                config.directMembershipResult=membership;
                                if(membership.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   membership.sourcePC!=0x120224u||!h.sourceCollisionE4->membershipRegistration_.returned)
                                    return stop(membership.sourcePC);
                                config.directMembershipReturned=true;
                                at(0x166738u);
                                //Actual E0 reload only after the E4 call genuinely returned120224.
                                //The result-backed COL has a distinct observer/list/pool occurrence.
                                if(!h.bodyOwner->object_||!h.sourceCollisionE0||
                                   h.bodyOwner->object_->collisionE0!=std::optional<g::OriginalCollisionBaseOwner50LA*>(
                                       h.sourceCollisionE0->collision_.get()))return stop(0x166738u);
                                at(0x16673cu);
                                if(!h.bodyOwner->object_->collisionB8)return stop(0x16673cu);
                                config.resultMembershipZone=*h.bodyOwner->object_->collisionB8;
                                config.resultMembershipMode=2u;at(0x166740u); //Mode2 in166744 delay slot.
                                config.resultMembershipEntered=true;
                                const auto resultMembership=h.sourceCollisionE0->registerSourceResultMembership120070(
                                    &config,*h.sourceCollisionE4,*config.resultMembershipZone,config.resultMembershipMode,
                                    makeCollisionMethod(true),makeCollisionPortal(true));
                                config.resultMembershipResult=resultMembership;
                                if(resultMembership.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                                   resultMembership.sourcePC!=0x120224u||!h.sourceCollisionE0->membershipRegistration_.returned)
                                    return stop(resultMembership.sourcePC);
                                config.resultMembershipReturned=true;
                                at(0x166748u);
                                //Actual ModelBASE14C is distinct from embedded Entity+8.
                                //Both preceding COL registrations genuinely returned before this call.
                                if(!config.directMembershipReturned||!h.bodyOwner->object_||!h.nestedModel||
                                   h.nestedModel->nativeWithdrawn_||!h.nestedModel->model_||
                                   h.bodyOwner->object_->model14C!=&h.nestedModel->model_->model_)return stop(pc);
                                at(0x16674cu);
                                if(!h.bodyOwner->object_->visibilityB4)return stop(pc);
                                config.modelObserverZone=*h.bodyOwner->object_->visibilityB4;
                                config.modelObserverMode=2u;at(0x166750u); //166754 delay-slot mode2.
                                config.modelObserverEntered=true;
                                const auto modelObserver=h.nestedModel->beginSourceModelObserver28E8A0(
                                    &config,*config.modelObserverZone,config.modelObserverMode);
                                config.modelObserverResult=modelObserver;
                                //The bounded prefix retains publication/constructor/guard effects,
                                //but fresh28E230 is still unissued. No Model return or Body+AC call.
                                return stop(modelObserver.sourcePC);
                            }
                            h.sourceDirectCollisionTransformEntered=true;
                            const auto directTransform=h.sourceCollisionE4->continueSourceDirectMissingTransform11F570();
                            h.sourceDirectCollisionNullModelBound=directTransform.status==g::OriginalSourcePhysicsCollision50LA::Status::bounded&&
                                directTransform.sourcePC==0x11f5b0u&&h.sourceCollisionE4->directNullModelBound_;
                            return stop(directTransform.sourcePC); //Actual NULLshape110 load; no28B550 or transform return.
                        }
                        if(accepted.status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||accepted.sourcePC!=0x11ba34u)
                            return stop(accepted.sourcePC);
                        if(!collisionMissing||!h.collisionSearch->returnedMissing132820())return stop(accepted.sourcePC);
                        h.sourceCollisionMissingEntered=true;
                        const auto missing=h.sourceCollisionE0->continueSourceMissing11BA34();
                        return stop(missing.sourcePC); //Actual missing11BBEC NULLdelay-store or honest refusal; no165E90 return.
                    }
                    //Nonzero-mode miss retains12DCB0 fallback, never source0.
                    if(!returned)return stop(lookupDiagnostic.sourcePC?lookupDiagnostic.sourcePC:0x132820u);
                    if(!h.missingLookup||!h.missingLookup->liveMissingNative())return stop(0x132a0c);
                    h.sourceLookupReturnV0=h.missingLookup->sourceReturnV0();h.nestedLookupReturned=true;
                    at(0x26e9b8);h.nestedReleaseEntered=true;
                    const auto retireMissing=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                        if(!h.nestedModelEnrolled||&entity!=h.nestedModel->countedModelEntityNative()||
                           !authority.canRetire(entity)||!authority.retire(entity))return false;
                        h.nestedModelEnrolled=false;return true;
                    };
                    const auto released=h.nestedModel->releaseSourceMissing26E9D4(h.missingLookup,
                        resolver.get(),sourceTrackFile_.get(),retireMissing);
                    if(released.status!=g::OriginalSourcePhysicsModel50LA::Status::reached||released.sourcePC!=0x165cdcu)
                        return stop(released.sourcePC);
                    if(!h.nestedModel->sourceReleaseReturned_||!h.nestedModel->sourceLoaderReturnV0_||
                       *h.nestedModel->sourceLoaderReturnV0_)return stop(released.sourcePC);
                    h.nestedLoaderReturnV0=h.nestedModel->sourceLoaderReturnV0_;h.nestedLoaderReturned=true;
                    //165C30 ignores this real240740 zero. BASE14C remains the
                    //original published ModelBASE, including source-dead history.
                    //No safeNULL or live141960 attachment is fabricated.
                    return stop(0x165cdc);
                }
                if(wrapper->type00!=1||wrapper->sourceVtable04!=0x3d26f0||
                   wrapper->dispatch!=ra_static_model_cold50LA::Owner::StandardWrapper::Dispatch::standard3D26F0)return stop(pc);
                at(0x27b970);sourceStandardCallbackEntered_=true;
                if(!continueSourceStandardModel27ABC0(pc))return stop(pc);
                if(!wrapper->sourceCallbackReturnV0||*wrapper->sourceCallbackReturnV0>1)return stop(pc);
                //Original ignores actual callback V0, including genuine filename-none0.
                //Native refusal never reaches these source increments.
                at(0x27b978);*sourceStandardSlotByteOffsetS1_+=4;
                at(0x27b97c);++*sourceStandardSlotS0_;sourceStandardCompletedCallbacks_[z]=*sourceStandardSlotS0_;
                at(0x27b980);
            }
            at(0x27b9a0);sourceStandardDiagnosticAttempted_[z]=true;
            sourceStandardDiagnosticWritten_[z]=std::fprintf(stdout,"\nLoaded %d static models from Zone%d.\n\n",
                static_cast<int>(*sourceStandardSlotS0_),static_cast<int>(*nativeModelZone354_+1u));
            at(0x27b9bc);sourceStandardParentReturned_[z]=true;at(0x27b9c4);
        }
        //Actual World3CF110+98 is274250: second pass invokes +14 only for
        //type2 PhysicsObject; known other types skip, not fabricated callbacks.
        at(0x274208);at(0x274268);sourceStandardTypeSlotS0_=0;at(0x274270);sourceStandardTypeByteOffsetS1_=0;
        for(;;){
            //Original2742A8 reloads World354/3C4 and parent30 on EACH test.
            at(0x2742a8);if(!nativeModelZone354_||!sourceModelParents3C4_||!sourceModels3C4_||
               *sourceModels3C4_!=sourceModelParents3C4_.get())return stop(pc);
            sourceStandardTypeZoneRead_=*nativeModelZone354_;
            if(*sourceStandardTypeZoneRead_!=z)return stop(pc);
            at(0x2742b8);sourceStandardTypeParentRead_=sourceModelParents3C4_[z];
            if(sourceStandardTypeParentRead_!=&parent->entity)return stop(pc);
            at(0x2742bc);sourceStandardTypeCountRead_=parent->count30;
            if(*sourceStandardTypeCountRead_>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))return stop(pc);
            at(0x2742c0);if(*sourceStandardTypeSlotS0_>=*sourceStandardTypeCountRead_)break;
            const auto slot=*sourceStandardTypeSlotS0_;at(0x274274);
            if(!parent->sourceEntries34||slot>=parent->sourceEntriesKnown||slot>=sourceStaticOwner_->sourceWrapperLedger_[z].size())return stop(pc);
            auto* wrapper=parent->sourceEntries34[slot];
            if(!wrapper||wrapper!=sourceStaticOwner_->sourceWrapperLedger_[z][slot].get()||!wrapper->sourceTypeKnown)return stop(0x274288);
            if(wrapper->type00==2)return stop(0x274298); //Genuine PhysicsObject +14 dispatch remains unjoined.
            at(0x27428c);*sourceStandardTypeByteOffsetS1_+=4;
            at(0x2742a4);++*sourceStandardTypeSlotS0_;sourceStandardTypeScanned_[z]=*sourceStandardTypeSlotS0_;
        }
        at(0x2742e0);sourceStandardTypeReturned_[z]=true;
        at(0x274210);*sourceStandardZoneByteOffsetS1_+=0x160;
        at(0x274214);*sourceStandardZonePointerOffsetS2_+=4;
        at(0x274218);++*sourceStandardZoneS0_;at(0x274220);
    }
    at(0x274230);sourceStandardWorldReturnV0_=1;at(0x27424c);
    //Actual next source call1B3AE0 remains separate. No World/player readiness.
    return stop(0x1aa208);
}
}
