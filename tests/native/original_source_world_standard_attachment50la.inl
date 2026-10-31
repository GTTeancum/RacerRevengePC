#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceStandardSmoker27AE68(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    if(!worldApplication_||!sourceStandardObserverReturnCurrent(w)||!w.sourceModel||
       !w.descriptor10||!w.descriptor10->sourceFields||!w.sourceSmokerRead||!*w.sourceSmokerRead||
       !w.sourceSmokerManagerRead||!w.sourceSmokerManager4A0||!sourceParticle4A0_||
       !trackParticles_||*sourceParticle4A0_!=trackParticles_.get()||
       w.sourceSmokerManager4A0!=trackParticles_.get()||nativeParticleManager4A0_!=trackParticles_.get()||
       !sourceParticleLoadReturned_||sourceParticleReturnedSelf_!=trackParticles_.get())return false;
    auto& f=*w.sourceModel;auto& s=f.sourceSmoker;auto& d=*w.descriptor10->sourceFields;
    if(f.smokerCallEntered||!f.parentAttachReturned||f.nativeRetired||
       f.loaderReturnedEntity!=&f.scene.model_.entity()||w.model8!=&f.scene||
       !d.loaded||(d.vectorKnown&24u)!=24u)return false;
    auto& authority=worldApplication_->entityAuthority_;
    if(authority.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-2u||
       !authority.reserveAdditional(2))return false;
    const auto enroll=[&]()noexcept {
        if(auto* e=s.sourceCountedSmokerNative())if(!f.smokerEnrolled){
            if(!authority.enroll(*e))return false;f.smokerEnrolled=true;}
        if(auto* e=s.sourceCountedEmitterNative())if(!f.emitterEnrolled){
            if(!authority.enroll(*e))return false;f.emitterEnrolled=true;}
        return true;
    };
    const auto attach=[&](g::OriginalWreckComponent50LA& parent,g::OriginalWreckComponent50LA& child,
        std::uint32_t mask,std::uint32_t lock)noexcept {
        return authority.attachChildren(parent,child,mask,lock);
    };
    f.smokerCallEntered=true;
    auto r=s.allocateSmoker27AE68(worldApplication_,worldEntity_.sessionGeneration,w.smoker0C);
    at(r.sourcePC);if(!enroll()||r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached)return false;
    r=s.configurePrefix288CE0(&f.scene,d.smokeOffset150,d.smokeVelocity160);
    at(r.sourcePC);if(!enroll()||r.sourcePC!=0x288f98u||!s.vector200Stored_)return false;
    r=s.continueDirectionScalars288F98(0x3f800000u,0x3f800000u,0x3f800000u);
    at(r.sourcePC);if(r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached||r.sourcePC!=0x288da0u)return false;
    r=s.attachEmitter141960(attach);at(r.sourcePC);
    if(r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached||r.sourcePC!=0x288da8u)return false;
    r=s.continueModelPoint288DB0(f.graph());at(r.sourcePC);
    if(r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached||r.sourcePC!=0x288df8u||
       !s.configurationReturnV0_||s.configurationReturnV0_!=s.sourceCountedSmokerNative())return false;
    f.smokerConfigurationReturned=true;at(0x27af4c);
    r=s.attachModel141960(attach);at(r.sourcePC);
    if(r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached||r.sourcePC!=0x27af60u)return false;
    //Native borrower admission precedes the actual15ACA0 field70 store.
    //The original store neither retains nor registers this ParticleManager.
    if(!sourceParticle4A0_||*sourceParticle4A0_!=trackParticles_.get()||
       trackParticles_->sourceSmokerManagerBorrowers_==std::numeric_limits<std::size_t>::max())return false;
    r=s.bindManager288E60(*sourceParticle4A0_);at(r.sourcePC);
    if(r.status!=g::OriginalSourceStandardSmoker50LA::Status::reached||r.sourcePC!=0x288e88u)return false;
    ++trackParticles_->sourceSmokerManagerBorrowers_;f.smokerManagerBorrow=trackParticles_.get();
    f.smokerManagerHistory=trackParticles_.get();f.smokerManagerBound=true;
    at(0x27af6c);return true;
}

bool Owner::continueSourceStandardAttachment27AE14(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       !sourceStandardObserverReturnCurrent(w)||!sourceStaticOwner_||!worldApplication_||
       !sourceStaticOwner_->sourceMode_||sourceStaticOwner_->closed_||
       sourceStaticOwner_->application_!=worldApplication_||sourceStaticOwner_->generation_!=worldEntity_.sessionGeneration||
       sourceStandardWrapperA0_!=&w||!sourceStandardCallbackEntered_||
       !w.sourceModel||!w.descriptor10||!w.descriptor10->sourceFields||w.sourceDescriptorDisposed||
       w.sourceCallbackReturnV0||!nativeModelZone354_||!sourceModelParents3C4_||
       !sourceModels3C4_||*sourceModels3C4_!=sourceModelParents3C4_.get())return false;
    auto& f=*w.sourceModel;const auto z=*nativeModelZone354_;
    if(f.parentAttachEntered||f.nativeRetired||f.loaderReturnedEntity!=&f.scene.model_.entity()||
       !f.placement.orientationReturned||w.model8!=&f.scene||z>=sourceStaticOwner_->parents_.size()||
       z>=sourceModelParentsPublished3C4_||!sourceStaticOwner_->parents_[z]||
       sourceModelParents3C4_[z]!=&sourceStaticOwner_->parents_[z]->entity||
       !sourceStaticOwner_->preflightSourceModelParentLinksNative())return false;
    auto& parent=*sourceStaticOwner_->parents_[z];auto& authority=worldApplication_->entityAuthority_;
    //Source-live proof, NOT native teardown admission: the real lock read
    //below may return nonzero and original141960 then returnsNULL harmlessly.
    const auto& e=parent.entity;
    if(!parent.sourceMode||!parent.constructed||parent.released||parent.zone!=z+1||
       !parent.sourceCountKnown||!parent.sourceArrayReturned||parent.sourceVtable08!=0x3d2640||
       e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::staticModels27B610||
       e.sessionGeneration!=sourceStaticOwner_->generation_||e.objectKey!=sourceStaticOwner_->key(&e)||
       e.nativeFlags29||e.category28!=0xff||e.flags29!=3||e.parent1C||e.previous10||e.next14||
       e.mask20||!worldApplication_->objectCount423F60()||
       sourceStandardParentS3_!=&parent||!sourceStandardSlotS0_||
       *sourceStandardSlotS0_>=parent.sourceEntriesKnown||!parent.sourceEntries34||
       parent.sourceEntries34[*sourceStandardSlotS0_]!=&w||
       z>=sourceStaticOwner_->sourceWrapperLedger_.size()||
       *sourceStandardSlotS0_>=sourceStaticOwner_->sourceWrapperLedger_[z].size()||
       sourceStaticOwner_->sourceWrapperLedger_[z][*sourceStandardSlotS0_].get()!=&w)return false;
    //Native membership for an already-counted source Parent; no guest retain
    //or constructor/count replay. Its storage remains the same3C4 owner.
    if(!parent.sourceAuthorityEnrolled){
        if(!authority.reserveAdditional(1)||!authority.canEnroll(parent.entity,parent.entity.objectKey)||
           !authority.enroll(parent.entity))return false;
        parent.sourceAuthorityEnrolled=true;
    }else if(!authority.canRetire(parent.entity))return false;
    at(0x27ae14);auto& child=*f.scene.wreckModelStorage();
    at(0x27ae34);f.parentAttachBorrow=sourceModelParents3C4_[z];f.parentAttachZone=z;
    at(0x27ae38);f.parentAttachEntered=true;at(0x141970);
    f.parentAttachLock=worldApplication_->attachmentLock423F58();
    if(!*f.parentAttachLock){
        const auto r=worldApplication_->entityAuthority_.attachChildren(*f.parentAttachBorrow,child,0u,*f.parentAttachLock);
        if(r.status!=g::OriginalWreckComponentStatus50LA::complete){at(r.sourcePC);return false;}
        f.parentAttached=true;
    } //Actual lockedNULL return is ignored; detached owner survives.
    f.parentAttachReturned=true;at(0x27ae40);
    //Success skips loader-zero-only27AE48: never clear the real wrapper8.
    auto& d=*w.descriptor10->sourceFields;at(0x27ae50);
    if(!d.loaded||!d.smoker14C)return false;w.sourceSmokerRead=*d.smoker14C;
    if(*w.sourceSmokerRead){
        at(0x27ae5c);if(!sourceParticle4A0_)return false;
        w.sourceSmokerManagerRead=true;w.sourceSmokerManager4A0=*sourceParticle4A0_;
        if(w.sourceSmokerManager4A0){at(0x27ae68);if(!continueSourceStandardSmoker27AE68(w,pc))return false;}
    }
    //Original false-smoker OR actualNULL4A0 joins raw descriptor free. Neither
    //branch writes the source-factory's unknown wrapper0C lane.
    if(sourceStaticModelParamFrame_&&sourceStaticModelParamFrame_->descriptorA0==w.descriptor10.get()){
        if(sourceStaticModelParamPhase_!=SourceStaticModelParamPhase::returned)return false;
        sourceStaticModelParamFrame_->descriptorA0=nullptr;
        sourceStaticModelParamFrame_->descriptorBorrowWithdrawn=true;
    }
    at(0x27af6c);w.descriptor10.reset();w.sourceDescriptorDisposed=true;
    at(0x27af74);w.sourceCallbackReturnV0=1;
    at(0x27af78);w.sourceDescriptorNullStored=true;
    at(0x27af90);return true; //Only wrapper return; original27B930 loop increment belongs to caller.
}
}
