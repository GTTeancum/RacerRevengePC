#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::finishSourceStandardRenderable24076C(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    if(!w.sourceModel||!w.descriptor10||w.sourceDescriptorDisposed)return false;
    auto& f=*w.sourceModel;
    if(f.nativeRetired||f.packetCallEntered||f.renderableReturnedEntity!=&f.scene.model_.entity8||
       (!f.rowTableCallReturned&&!f.warmRowTableReturned)||!f.scene.fields_.geometry1B4||
       !f.scene.preflightSourceRenderableReturn26E820Native())return false;
    //This is26E820's Entity+8, NOT240740's later returned Entity. Original
    //240740 resumes at24076C; raw dynamic flags/packets remain unjoined.
    at(0x24076cu);
    if(!f.dynamicArgument)return false;
    f.packetCallEntered=true;
    const auto packets=f.sourcePackets.constructSourcePackets240740(f.scene,
        *f.scene.fields_.geometry1B4,static_cast<std::uint32_t>(*f.dynamicArgument));
    at(packets.sourcePC);if(!packets.okay())return false;
    if(packets.sourcePC!=0x2407d8u||!f.sourcePackets.preflightSourcePacketsNative()||
       !f.sourcePackets.sourcePackets_.returned||
       f.sourcePackets.sourcePackets_.returnedEntityV0!=&f.scene.model_.entity8)return false;
    f.packetCallReturned=true;f.loaderReturnedEntity=f.sourcePackets.sourcePackets_.returnedEntityV0;
    //Actual27AC5C tests this typed nonNULL Entity+8, not an invented literal1.
    //Only now may the reviewed shadow/name/guard/pose prefix execute.
    at(0x27ac64u);if(!continueSourceStandardSuccess27AC64(w,pc))return false;
    //The actual source observer consumes the placed ModelBASE; only its
    //live same-node/List/pool return admits subsequent Entity+8 attachment.
    if(!continueSourceStandardObserver28E8A0(w,pc))return false;
    if(!sourceStandardObserverReturnCurrent(w))return false;
    return continueSourceStandardAttachment27AE14(w,pc); 
}

bool Owner::preflightSourceStandardModelsNative(std::size_t& count)const noexcept {
    count=0;if(!preflightSourceStandardObserversNative())return false;if(!sourceStaticOwner_)return true;
    if(!sourceStaticOwner_->canCloseSourceNative()||!worldApplication_)return false;
    const auto& authority=worldApplication_->entityAuthority_;std::size_t managerBorrowers=0;
    for(const auto& parent:sourceStaticOwner_->parents_)if(parent&&parent->sourceAuthorityEnrolled&&
        (!parent->constructed||parent->released||!authority.canRetire(parent->entity)))return false;
    //Base PhysicsObjects own the SAME Application count/authority, not a
    //prepared Wreck domain. Include genuine nullable/count prefixes too.
    for(const auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(const auto& w:row)if(w&&w->sourcePhysics.bodyOwner){
        const auto& h=w->sourcePhysics;const auto& body=*h.bodyOwner;
        if(!body.preflightNative())return false;
        if(const auto* entity=body.countedEntityNative()){
            if(count==std::numeric_limits<std::size_t>::max())return false;++count;
            if(h.bodyEnrolled&&!authority.canRetire(*entity))return false;
        }else if(h.bodyEnrolled)return false;
        if(h.foundEntered&&!h.foundNativeWithdrawn){
            if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
               h.foundProvider!=sourceTrackMountDomain_->archiveProvider3E1A70()||
               h.foundProvider!=sourceTrackMountLease_.provider_||
               h.foundResolver!=sourceTrackMountDomain_->resolver3E1A70())return false;
            if(h.foundProviderResolved){
                a::OriginalStandardArchiveProvider50LA::MemberView current;
                if(h.foundProvider->lookup132820(h.nestedModel->requestedPsg_,current)!=
                   a::OriginalStandardArchiveProvider50LA::Lookup::present||
                   current.member!=h.foundMember.member||current.data!=h.foundMember.data||
                   current.size!=h.foundMember.size||current.mount!=h.foundMember.mount||
                   current.ordinal!=h.foundMember.ordinal||current.bankGeneration!=h.foundMember.bankGeneration||
                   current.revision!=h.foundMember.revision)return false;
            }
            if(h.foundNativeResolved){
                a::OriginalStandardModelFileView50LA current;a::OriginalStandardTextureDiagnostic50LA diagnostic;
                if(!h.foundResolver->modelFile(h.foundNativeFile.id,current,diagnostic)||
                   current.member!=h.foundNativeFile.member||current.bytes!=h.foundNativeFile.bytes||
                   current.geometry240110.get()!=(h.foundFile&&h.foundFile->nativeCacheAssociated()&&h.nestedModel&&h.nestedModel->model_?
                       h.nestedModel->model_->sourceGeometry_.owner.get():h.foundGeometryHistory)||!current.bytes||
                   current.bytes->size()!=h.foundMember.size||!h.foundMember.data||
                   std::memcmp(current.bytes->data(),h.foundMember.data,h.foundMember.size))return false;
            }
        }
        if(h.foundFile&&h.foundFile->nativeCacheAssociated()){
            if(!h.nestedCacheEntered||!h.nestedModel||!h.nestedModel->model_||
               !h.foundFile->associatedGeometryCurrent(h.nestedModel->model_->sourceGeometry_.owner.get(),h.nestedModel->model_.get()))return false;
        }
        if(h.foundFile&&(!h.foundFile->preflightNative()||h.foundFile->root()!=sourceTrackFile_.get()||
           h.foundFile->resolver()!=sourceTrackMountDomain_->resolver3E1A70().get()))return false;
        if(h.missingLookup){
            if(!h.missingLookup->preflightNative()||!sourceTrackMountDomain_||
               h.missingLookup->resolver()!=sourceTrackMountDomain_->resolver3E1A70().get()||
               h.missingLookup->root()!=sourceTrackFile_.get())return false;
        }
        if(h.nestedModel){
            const auto& nested=*h.nestedModel;
            if(nested.materialEntered_&&!nested.nativeWithdrawn_){
                if(!sourceLoadApplication_)return false;
                const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
                if(!domain||domain!=sourceTrackMountDomain_||!domain->prepared()||
                   !sourceLoadApplication_->materialApplication||nested.materialTable_!=domain->materials382B38()||
                   nested.materialPrefix_!=&sourceLoadApplication_->materialDirectory36B970()||
                   !nested.materialTable_||!nested.materialTable_->sourceNames17CAD4Current()||
                   !nested.materialTable_->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return false;
            }
            if(!h.nestedModel->preflightNative())return false;
            if(h.nestedModel->directoryIdentity_&&(!sourceLoadApplication_||
               h.nestedModel->directoryIdentity_!=&sourceLoadApplication_->modelDirectory36B860_))return false;
            if(const auto* entity=h.nestedModel->countedModelEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.nestedModelEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.nestedModelEnrolled)return false;
            for(std::uint32_t row=1;row<a::maxModelNodes49O;++row){
                const auto* entity=h.nestedModel->countedChildEntityNative(row);
                if(entity){if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                    if(h.nestedRowEnrolled[row]&&(h.nestedRowEnrolled[row]!=entity||!authority.canRetire(*entity)))return false;
                }else if(h.nestedRowEnrolled[row])return false;
            }
        }
        if(h.sourceBodyD0){
            if(!h.sourceBodyD0->preflightNative())return false;
            if(auto* entity=h.sourceBodyD0->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceBodyEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceBodyEnrolled)return false;
        }
        if(h.sourceGravityD4){
            if(!h.sourceGravityD4->preflightNative())return false;
            if(auto* entity=h.sourceGravityD4->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceGravityEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceGravityEnrolled)return false;
        }
        if(h.sourceCollisionE0){
            if(!h.sourceCollisionE0->preflightNative())return false;
            if(auto* entity=h.sourceCollisionE0->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceCollisionEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceCollisionEnrolled)return false;
        }
        if(h.sourceContactDC){
            if(!h.sourceContactDC->preflightNative())return false;
            if(auto* entity=h.sourceContactDC->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceContactEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceContactEnrolled)return false;
        }
        if(h.sourceDetectorD8){
            if(!h.sourceDetectorD8->preflightNative())return false;
            if(auto* entity=h.sourceDetectorD8->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceDetectorEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceDetectorEnrolled)return false;
        }
        if(h.sourceCollisionE4){
            if(!h.sourceCollisionE4->preflightNative())return false;
            if(auto* entity=h.sourceCollisionE4->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceDirectCollisionEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceDirectCollisionEnrolled)return false;
        }
        if(h.sourceCollisionE8){
            if(!h.sourceCollisionE8->preflightNative())return false;
            if(auto* entity=h.sourceCollisionE8->countedEntityNative()){
                if(count==std::numeric_limits<std::size_t>::max())return false;++count;
                if(h.sourceProceduralCollisionEnrolled&&!authority.canRetire(*entity))return false;
            }else if(h.sourceProceduralCollisionEnrolled)return false;
        }
        if(h.directCollisionSearch&&(!h.directCollisionSearch->preflightNative()||!sourceTrackMountDomain_||
           h.directCollisionSearch->resolver()!=sourceTrackMountDomain_->resolver3E1A70().get()||
           h.directCollisionSearch->root()!=sourceTrackFile_.get()))return false;
        if(h.collisionSearch&&(!h.collisionSearch->preflightNative()||!sourceTrackMountDomain_||
           h.collisionSearch->resolver()!=sourceTrackMountDomain_->resolver3E1A70().get()||
           h.collisionSearch->root()!=sourceTrackFile_.get()))return false;
        if(h.collisionRead&&(!h.collisionZone||*h.collisionZone>=sourceCollisionPublished_||
           !sourceCollision4C4_||*sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
           !*sourceCollision4C4_||(*sourceCollision4C4_)[*h.collisionZone]!=h.collisionNode))return false;
        if(h.visibilityRead&&(!h.visibilityZone||*h.visibilityZone>=sourceVisibilityPublished_||
           !sourceVisibility4C0_||*sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||
           !*sourceVisibility4C0_||(*sourceVisibility4C0_)[*h.visibilityZone]!=h.visibilityNode))return false;
    }
    for(const auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(const auto& w:row)if(w&&w->sourceModel){
        const auto& f=*w->sourceModel;if(f.nativeRetired)continue;
        const auto& smoker=f.sourceSmoker;
        if(!smoker.preflightNative())return false;
        const auto countedSmoker=smoker.smokerCounted_&&!smoker.smokerRetired_;
        const auto countedEmitter=smoker.emitterCounted_&&!smoker.emitterRetired_;
        if(f.smokerEnrolled&&(!countedSmoker||!smoker.smoker_||!authority.canRetire(smoker.smoker_->entity)))return false;
        if(f.emitterEnrolled&&(!countedEmitter||!smoker.emitter_||!authority.canRetire(smoker.emitter_->entity)))return false;
        if(countedSmoker){if(count==std::numeric_limits<std::size_t>::max())return false;++count;}
        if(countedEmitter){if(count==std::numeric_limits<std::size_t>::max())return false;++count;}
        if(f.smokerManagerBorrow){
            if(!f.smokerManagerBound||!trackParticles_||f.smokerManagerBorrow!=trackParticles_.get()||
               f.smokerManagerHistory!=trackParticles_.get()||!sourceParticle4A0_||
               *sourceParticle4A0_!=trackParticles_.get()||smoker.nativeWithdrawn_||
               managerBorrowers==std::numeric_limits<std::size_t>::max())return false;
            ++managerBorrowers;
        }else if(f.smokerManagerBound&&!smoker.nativeWithdrawn_)return false;
        if(f.counted){
            if(count==std::numeric_limits<std::size_t>::max())return false;++count;
            const auto& e=f.scene.model_.entity8;if(f.enrolled&&!authority.canRetire(e))return false;
        }
        for(std::uint32_t row=1;row<f.scene.binding_.sourceRowCount;++row){
            const auto* object=f.scene.sourceRowObject(row);
            if(!object||!object->constructed||object->released)continue;
            if(count==std::numeric_limits<std::size_t>::max())return false;++count;
            if(f.rowEnrolled[row]&&!authority.canRetire(object->entity8))return false;
            if(f.scene.sourceWarmClone_.entered){
                if(object->sceneIndex>=f.warmCloneEnrolled.size())return false;
                const auto* enrolled=f.warmCloneEnrolled[object->sceneIndex];
                if(enrolled&&(enrolled!=&object->entity8||!authority.canRetire(*enrolled)))return false;
            }
        }
        if(f.sourceFile){
            if(!f.sourceFile->preflightNative()||f.sourceFile->resolver()!=f.resolver.get()||
               f.sourceFile->root()!=sourceTrackFile_.get()||
               f.sourceFile->nativeFile().resolver!=f.file.id.resolver||
               f.sourceFile->nativeFile().row!=f.file.id.row)return false;
            if(f.lookupReturn&&*f.lookupReturn){
                if(f.warmLookupAttempted){
                    if(!f.sourceFile->returnedWarm()||f.sourceFile->sourceHandle()!=-1||
                       (!f.sourceFileNativeClosed&&!f.scene.sourceWarmGeometry_.nativeLedgerWithdrawn&&(!f.warmCreator||
                        !f.sourceFile->warmGeometryCurrent(f.warmGeometryHistory,&f.warmCreator->scene))))return false;
                }else if(!f.sourceFile->returnedCold()||
                    (!f.sourceFileNativeClosed&&!f.sourceFile->sourceCloseReturned()&&!f.sourceFile->current()))return false;
            }
            if(f.scene.sourceDirectRows_.attempted&&f.scene.sourceDirectRows_.sourceFile!=f.sourceFile)return false;
            if(f.sourceFile->nativeCacheAssociated()&&(!f.sourceCacheCallEntered||
               !f.scene.sourceGeometry_.owner||!f.sourceFile->associatedGeometryCurrent(
                    f.scene.sourceGeometry_.owner.get(),&f.scene)))return false;
            if(f.sourceFile->sourceCloseReturned()&&(!f.sourceCloseCallEntered||!f.sourceCacheCallReturned))return false;
        }else if(f.lookupReturn&&*f.lookupReturn)return false;
        if(f.materialCallEntered){
            if(!sourceLoadApplication_||!sourceTrackMountDomain_||
               sourceLoadApplication_->standardMaterialDomainOwner_!=sourceTrackMountDomain_||
               f.materialTable!=sourceTrackMountDomain_->materials382B38()||
               !f.materialTable||!f.materialTable->sourceNames17CAD4Current()||
               !sourceLoadApplication_->materialApplication||
               !f.materialTable->screenApplicationMatches(*sourceLoadApplication_->materialApplication)||
               f.materialPrefix!=&sourceLoadApplication_->materialDirectory36B970())return false;
        }
        if(f.lookupEntered){
            if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||
               f.provider!=sourceTrackMountDomain_->archiveProvider3E1A70()||
               f.provider!=sourceTrackMountLease_.provider_||f.resolver!=sourceTrackMountDomain_->resolver3E1A70())return false;
            a::OriginalStandardArchiveProvider50LA::MemberView current;
            const auto result=f.provider->lookup132820(f.requestedPsg,current);
            if(f.providerResolved){
                if(result!=a::OriginalStandardArchiveProvider50LA::Lookup::present||
                   current.member!=f.member.member||current.data!=f.member.data||current.size!=f.member.size||
                   current.mount!=f.member.mount||current.ordinal!=f.member.ordinal||
                   current.bankGeneration!=f.member.bankGeneration||current.revision!=f.member.revision)return false;
            }else if(f.lookupReturn&&!*f.lookupReturn&&result!=a::OriginalStandardArchiveProvider50LA::Lookup::absent)return false;
            if(f.resolverResolved){
                a::OriginalStandardModelFileView50LA file;a::OriginalStandardTextureDiagnostic50LA diagnostic;
                if(!f.resolver->modelFile(f.file.id,file,diagnostic)||file.member!=f.file.member||file.bytes!=f.file.bytes||
                   file.geometry240110.get()!=(f.sourceFile&&f.sourceFile->nativeCacheAssociated()?
                       f.scene.sourceGeometry_.owner.get():(f.warmLookupAttempted?f.warmGeometryHistory:f.file.geometry240110.get()))||file.kind!=a::OriginalStandardModelFileKind50LA::psg||
                   !file.bytes||file.bytes->size()!=current.size||!current.data||
                   std::memcmp(file.bytes->data(),current.data,current.size))return false;
            }
        }
    }
    if((trackParticles_?trackParticles_->sourceSmokerManagerBorrowers_:0u)!=managerBorrowers)return false;
    return count<=worldApplication_->objectCount423F60();
}
bool Owner::retireSourceStandardModelsNative(std::uint32_t& pc)noexcept {
    std::size_t count=0;if(!preflightSourceStandardModelsNative(count))return false;
    if(!sourceStaticOwner_)return true;
    //Retire exact staged nested Model borrowers before their PhysicsObject base.
    //Withdraw counted membership before uncounted wrapper/descriptor storage.
    for(auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(auto& w:row)if(w&&w->sourcePhysics.bodyOwner){
        auto& h=w->sourcePhysics;
        const auto detachComponent=[&](g::OriginalWreckComponent50LA& entity,std::uint32_t lock)noexcept {
            return worldApplication_->entityAuthority_.detach(entity,lock);
        };
        //Break all directed local-list cycles before ANY target detach/retirement.
        //Incoming pins do not block this outgoing-only phase; each retry preserves history.
        if(h.sourceCollisionE0&&!h.sourceCollisionE0->withdrawSourceLocalFiltersNative())return false;
        if(h.sourceCollisionE4&&!h.sourceCollisionE4->withdrawSourceLocalFiltersNative())return false;
        if(h.sourceCollisionE8&&!h.sourceCollisionE8->withdrawSourceLocalFiltersNative())return false;
        //E8's exact predecessor pin exists before nullable construction/publication.
        //Retire every successful or partial E8 before E4 detach or payload retirement.
        if(h.sourceCollisionE8){
            if(!h.sourceCollisionE8->detachSourceCollisionNative(detachComponent))return false;
            const auto retireProceduralCollision=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceCollisionE8->countedEntityNative())return false;
                if(h.sourceProceduralCollisionEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceProceduralCollisionEnrolled=false;}return true;
            };
            if(!h.sourceCollisionE8->withdrawNative(retireProceduralCollision))return false;
        }
        //Direct E4 pins its Detector predecessor and a separate Body/Model
        //argument lease; none of these are the E0 occurrence or sourceA0 store.
        if(h.sourceCollisionE4){
            if(!h.sourceCollisionE4->detachSourceCollisionNative(detachComponent))return false;
            const auto retireDirectCollision=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceCollisionE4->countedEntityNative())return false;
                if(h.sourceDirectCollisionEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceDirectCollisionEnrolled=false;}return true;
            };
            if(!h.sourceCollisionE4->withdrawNative(retireDirectCollision))return false;
        }
        if(h.directCollisionSearch){a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!h.directCollisionSearch->closeNative(diagnostic)){pc=diagnostic.sourcePC;return false;}}
        //Detector holds Contact even before nullableD8 publication/first use.
        //Retire its exact borrower before Contact detach or point unpin.
        if(h.sourceDetectorD8){
            if(!h.sourceDetectorD8->detachSourceDetectorNative(detachComponent))return false;
            const auto retireDetector=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceDetectorD8->countedEntityNative())return false;
                if(h.sourceDetectorEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceDetectorEnrolled=false;}return true;
            };
            if(!h.sourceDetectorD8->withdrawNative(retireDetector))return false;
        }
        //Contact Body30 pins Body; retire it before COL attachment/payload
        //withdrawal and the existing Gravity/Body/Model reverse chain.
        if(h.sourceContactDC){
            if(!h.sourceContactDC->detachSourceContactNative(detachComponent))return false;
            const auto retireContact=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceContactDC->countedEntityNative())return false;
                if(h.sourceContactEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceContactEnrolled=false;}return true;
            };
            if(!h.sourceContactDC->withdrawNative(retireContact))return false;
        }
        //COL pins Body; Gravity target pins Body; Body pins Model. Retire
        //exact dependents first, retaining partial search and count histories.
        if(h.sourceCollisionE0){
            if(!h.sourceCollisionE0->detachSourceCollisionNative(detachComponent))return false;
            const auto retireCollision=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceCollisionE0->countedEntityNative())return false;
                if(h.sourceCollisionEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceCollisionEnrolled=false;}return true;
            };
            if(!h.sourceCollisionE0->withdrawNative(retireCollision))return false;
        }
        if(h.collisionSearch){a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!h.collisionSearch->closeNative(diagnostic)){pc=diagnostic.sourcePC;return false;}}
        if(h.sourceGravityD4){
            if(!h.sourceGravityD4->detachSourceGravityNative(detachComponent))return false;
            const auto retireGravity=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceGravityD4->countedEntityNative())return false;
                if(h.sourceGravityEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceGravityEnrolled=false;}return true;
            };
            if(!h.sourceGravityD4->withdrawNative(retireGravity))return false;
        }
        if(h.sourceBodyD0&&!h.sourceBodyD0->detachSourceBodyNative(detachComponent))return false;
        //Body raw288 borrows the nested Model from even the NULLD0 first
        //use boundary. Withdraw this exact occurrence before Model topology,
        //packet/sourcefile or resource/count retirement.
        if(h.sourceBodyD0){
            const auto retireBody=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity!=h.sourceBodyD0->countedEntityNative())return false;
                if(h.sourceBodyEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;
                    h.sourceBodyEnrolled=false;}return true;
            };
            if(!h.sourceBodyD0->withdrawNative(retireBody))return false;
        }
        //Withdraw actual Base↔nested Model reciprocal topology before any
        //packet/resource/count retirement; ignored locked source return has
        //no link to detach. Native detach still obeys current global lock.
        if(h.nestedModel){
            const auto detach=[&](g::OriginalWreckComponent50LA& child,std::uint32_t lock)noexcept {
                return worldApplication_->entityAuthority_.detach(child,lock);
            };
            if(!h.nestedModel->detachSourceModelNative(detach))return false;
        }
        //The source packet occurrence borrows geometry even before1F0
        //publication or a nullable packet result. Retire it before association,
        //file or materials; this never fabricates original packet destruction.
        if(h.nestedModel&&!h.nestedModel->withdrawSourcePacketsNative())return false;
        //Exact source row1C/native weak association must be withdrawn before
        //file/predecessor custody or geometry/prototype destruction. This is
        //native cancellation, never a source1327D0 return or count/ref mutation.
        if(h.foundFile&&h.foundFile->nativeCacheAssociated()){
            if(!h.nestedModel||!h.nestedModel->model_)return false;
            a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!h.foundFile->withdrawGeometryNative(h.nestedModel->model_->sourceGeometry_.owner.get(),
               h.nestedModel->model_.get(),diagnostic)){pc=diagnostic.sourcePC;return false;}
        }
        //A pending found continuation fences its predecessor even before
        //original131380 returns. Withdraw file/open custody next, no replay.
        if(h.foundFile){a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!h.foundFile->closeNative(diagnostic)){pc=diagnostic.sourcePC;return false;}}
        if(h.foundEntered){h.foundNativeWithdrawn=true;h.foundProvider.reset();h.foundResolver.reset();
            h.foundMember={};h.foundNativeFile={};h.foundGeometryHistory=nullptr;}
        //This is lookup-attempt withdrawal only, never source131040 or
        //rollback of actual shared FileManager search cursor effects.
        if(h.missingLookup){a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!h.missingLookup->closeNative(diagnostic)){pc=diagnostic.sourcePC;return false;}}
        //Nested Model owns a token on the PhysicsObject even before14C
        //publication. Its separate Entity+8 must retire before BASE Entity.
        if(h.nestedModel){
            const auto retireNested=[&](g::OriginalWreckComponent50LA& entity)noexcept {
                if(&entity==h.nestedModel->countedModelEntityNative()){
                    if(h.nestedModelEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;h.nestedModelEnrolled=false;}
                    return true;
                }
                for(std::uint32_t row=1;row<a::maxModelNodes49O;++row){
                    if(&entity!=h.nestedModel->countedChildEntityNative(row))continue;
                    if(h.nestedRowEnrolled[row]){if(h.nestedRowEnrolled[row]!=&entity||
                       !worldApplication_->entityAuthority_.retire(entity))return false;h.nestedRowEnrolled[row]=nullptr;}
                    return true;
                }
                return false;
            };
            if(!h.nestedModel->withdrawNative(retireNested))return false;
        }
        const auto retire=[&](g::OriginalWreckComponent50LA& entity)noexcept {
            if(h.bodyEnrolled){if(!worldApplication_->entityAuthority_.retire(entity))return false;h.bodyEnrolled=false;}
            return true;
        };
        if(!h.bodyOwner->withdrawNative(retire))return false;
    }
    //Withdraw genuine Parent child links before observers/packets/resources.
    //Native detach is not source141A70 execution; exact receipts keep retries safe.
    for(auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(auto& w:row)if(w&&w->sourceModel){
        auto& f=*w->sourceModel;if(!f.parentAttached||f.parentDetachedNative)continue;
        const auto r=worldApplication_->entityAuthority_.detach(f.scene.model_.entity8,
            worldApplication_->attachmentLock423F58());
        if(r.status!=g::OriginalWreckComponentStatus50LA::complete){pc=r.sourcePC;return false;}
        f.parentDetachedNative=true;
    }
    //The smoker owns both model40 and manager70 borrows. Withdraw its outer
    //and emitter child links/counts before observers, packets or ParticleManager.
    for(auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(auto& w:row)if(w&&w->sourceModel){
        auto& f=*w->sourceModel;auto& s=f.sourceSmoker;if(f.nativeRetired)continue;
        const auto detach=[&](g::OriginalWreckComponent50LA& child,std::uint32_t lock)noexcept {
            return worldApplication_->entityAuthority_.detach(child,lock);
        };
        const auto retire=[&](g::OriginalWreckComponent50LA& child)noexcept {
            bool* enrolled=nullptr;
            if(s.smoker_&& &child==&s.smoker_->entity)enrolled=&f.smokerEnrolled;
            else if(s.emitter_&& &child==&s.emitter_->entity)enrolled=&f.emitterEnrolled;
            else return false;
            if(*enrolled){if(!worldApplication_->entityAuthority_.retire(child))return false;*enrolled=false;}
            return true;
        };
        if(!s.withdrawNative(detach,retire))return false;
        if(f.smokerManagerBorrow){
            if(!trackParticles_||f.smokerManagerBorrow!=trackParticles_.get()||
               !trackParticles_->sourceSmokerManagerBorrowers_)return false;
            --trackParticles_->sourceSmokerManagerBorrowers_;f.smokerManagerBorrow=nullptr;
        }
    }
    if(!retireSourceStandardObserversNative(pc))return false;
    //Retire entire warm targets before their earlier creators, not just the
    //resource borrow. This keeps retry preflight independent of dead caches.
    for(auto row=sourceStaticOwner_->sourceWrapperLedger_.rbegin();row!=sourceStaticOwner_->sourceWrapperLedger_.rend();++row)
        for(auto slot=row->rbegin();slot!=row->rend();++slot)if(*slot&&(*slot)->sourceModel){
        auto& w=*slot;auto& f=*w->sourceModel;if(f.nativeRetired)continue;
        //Pending source packets borrow model/palette even before1F0 publication.
        //Withdraw them before material/resource/row owners on every partial path.
        if(!f.sourcePackets.withdrawSourcePacketsNative())return false;
        //Partial material retains/array borrow retire before any Entity
        //membership or scene storage, including failed lookup/read prefixes.
        if(f.warmLookupAttempted){
            //Real warm targets have their OWN material retains/table. Withdraw
            //those after packet borrowers but before actual warm resource refs.
            if(!f.scene.withdrawSourceWarmMaterialsNative()||!f.scene.withdrawSourceWarmGeometryNative())return false;
            if(f.scene.sourceWarmGeometry_.attempted&&f.scene.sourceWarmGeometry_.nativeWithdrawn)
                f.sourceFileNativeClosed=true; //Actual explicit native lookup withdrawal already returned.
            if(!f.scene.withdrawSourceWarmRowTableNative())return false;
        }else if(!f.scene.releaseSourceMaterialsNative())return false;
        //Both source member1C and native weak crosswalk borrow the prototype.
        //Withdraw them after materials but BEFORE geometry/model destruction.
        if(f.sourceFile&&f.sourceFile->sourceAssociationStored()&&!f.sourceFile->associationWithdrawnNative()){
            a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!f.sourceFile->withdrawGeometryNative(f.scene.sourceGeometry_.owner.get(),&f.scene,diagnostic)){
                pc=diagnostic.sourcePC;return false;}
        }
        if(!f.scene.releaseSourceGeometryNative())return false;
        //Actual1B8 is a separate row borrow; withdraw after geometry but
        //before file/authority/scene backing. No source destructor is invented.
        if(!f.scene.withdrawSourceRowTableNative())return false;
        //Native explicit file withdrawal AFTER every resource read borrower,
        //BEFORE counted scene retirement; failure leaves a retryable live owner.
        if(f.sourceFile&&!f.sourceFileNativeClosed){
            a::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!f.sourceFile->closeNative(diagnostic)){pc=diagnostic.sourcePC;return false;}
            f.sourceFileNativeClosed=true;
        }
        //Native withdrawal only, not original26E9D4/240450 destruction. Exact
        //wrapper alias and authority membership retire before counted backing.
        for(std::uint32_t row=f.scene.binding_.sourceRowCount;row>1;){--row;
            if(f.rowEnrolled[row]){
                auto* child=f.scene.wreckRowStorage(row);
                if(!child||!worldApplication_->entityAuthority_.retire(*child))return false;f.rowEnrolled[row]=false;
            }
        }
        //Clone enumeration may reorder native row owners. Withdraw exact
        //physical Entity identities, including counted but unpublished rows.
        for(std::size_t physical=f.warmCloneEnrolled.size();physical>1;){--physical;
            auto*& enrolled=f.warmCloneEnrolled[physical];if(!enrolled)continue;
            if(!worldApplication_->entityAuthority_.retire(*enrolled))return false;enrolled=nullptr;
        }
        if(f.enrolled){if(!worldApplication_->entityAuthority_.retire(*f.scene.wreckModelStorage()))return false;f.enrolled=false;}
        w->model8=nullptr;f.nativeAliasWithdrawn=true;
        const auto closed=f.sourceReferenceDropped&&!f.scene.released_?f.scene.releaseSceneBase2896D0(f.graph()):f.scene.closeFreshUnlinked(f.graph());if(!closed.okay()){pc=closed.sourcePC;return false;}
        f.counted=false;f.sourceFile.reset();f.file={};f.member={};f.provider.reset();f.resolver.reset();
        f.materialTable.reset();f.materialPrefix=nullptr;f.nativeRetired=true;
    }
    //Only after every model child/native borrower retires may membership of
    //the still-counted parent be withdrawn before its owning static close.
    for(auto& parent:sourceStaticOwner_->parents_)if(parent&&parent->sourceAuthorityEnrolled){
        if(parent->entity.firstChild18||!worldApplication_->entityAuthority_.retire(parent->entity))return false;
        parent->sourceAuthorityEnrolled=false;
    }
    return true;
}
bool Owner::releaseSourceAbsentModel26E9D4(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    if(!w.sourceModel||!worldApplication_)return false;auto& f=*w.sourceModel;auto& scene=f.scene;
    if(!f.lookupReturn||*f.lookupReturn||!f.absentLogAttempted||!f.counted||!f.enrolled||
       !f.scene.sourceConstructor_.modelReturned||f.rowsCallEntered||f.releaseLock423F58||
       !w.sourceModelPublished||w.model8!=&scene||scene.fields_.geometry1B4||scene.fields_.packets1F0||
       scene.fields_.sourceRows1B8||scene.fields_.materials1C4||scene.binding_.sourceRowCount||
       scene.model_.observer19C||scene.model_.configuration1A4)return false;
    at(0x26e9d4);at(0x142590);at(0x1425a8);
    f.releaseLock423F58=worldApplication_->attachmentLock423F58();
    if(*f.releaseLock423F58){
        //Genuine original early return. Ref/count/storage survive even when
        //the wrapper later erases8; custody remains in this same frame.
        f.releaseReturnV0=0;return true;
    }
    auto& authority=worldApplication_->entityAuthority_;
    const auto& e=scene.model_.entity8;
    if(e.parent1C||e.firstChild18||e.previous10||e.next14||e.reference00!=1||
       f.graphCount!=1||!scene.preflightModelReference142590InAuthority(authority.view()).okay()||
       !scene.preflightScene2896D0(f.graph()).okay()||!authority.canRetire(e))return false;
    //Retire exact native membership just before original104970's last drop.
    if(!authority.retire(*scene.wreckModelStorage()))return false;f.enrolled=false;
    at(0x142654);const auto dropped=scene.releasePreflightedModelReference142590();
    if(!dropped.okay()){at(dropped.sourcePC);return false;}f.sourceReferenceDropped=true;
    at(0x241920);at(0x240480);f.destructorVtable1A8=0x3d1f40;f.destructorStoreMask|=1;
    at(0x24048c);f.destructorVtable10=0x3d1f48;f.destructorStoreMask|=2;
    at(0x240494);f.destructorNullSkips|=1; //Known NULLgeometry, no row packet walk.
    at(0x24050c);f.destructorNullSkips|=2; //Known NULLpackets, NO free(NULL) call.
    at(0x240520);at(0x26e48c);f.destructorVtable1A8=0x3d2390;f.destructorStoreMask|=4;
    at(0x26e498);f.destructorVtable10=0x3d2398;f.destructorStoreMask|=8;
    at(0x26e4a0);f.destructorNullSkips|=4; //NULLgeometry skips materials/resource/cache release.
    at(0x26e5d0);f.destructorNullSkips|=8; //NULLrowtable: no arrayfree receipt.
    at(0x26e5e4);f.destructorNullSkips|=16; //NULLmaterials: no scalarfree receipt.
    at(0x26e5f8);at(0x2896f4);f.destructorVtable1A8=0x3d3190;f.destructorStoreMask|=16;
    at(0x289700);f.destructorVtable10=0x3d3198;f.destructorStoreMask|=32;
    at(0x289708);f.destructorNullSkips|=32; //Fresh scene parent NULL.
    at(0x28971c);f.destructorNullSkips|=64; //Fresh scene child NULL.
    at(0x289764);f.destructorNullSkips|=128; //Fresh observer NULL.
    at(0x289778);at(0x141404);f.destructorVtable10=0x3cd240;f.destructorStoreMask|=64;
    //Existing SAME scene destructor owns the one141420 global-count drop.
    //Reference is already zero; do not call native closeFreshUnlinked here.
    at(0x141420);const auto base=scene.releaseSceneBase2896D0(f.graph());
    if(!base.okay()){at(base.sourcePC);return false;}f.sourceSceneReleased=true;f.counted=false;
    //14141C calls104910 witha1=0. Its10493C branch-delay store still
    //installs base3CC980 at Entity+8 (Model+10), before outer deletion.
    at(0x10493c);f.destructorVtable10=0x3cc980;f.destructorStoreMask|=128;
    at(0x240538);f.sourceOuterDeleted=true; //Native tombstone retained, no remaining source storage authority.
    f.releaseReturnV0=0;return true;
}
bool Owner::continueSourceStandardModel27ABC0(std::uint32_t& pc)noexcept {
    using Static=ra_static_model_cold50LA::Owner;
    using Provider=a::OriginalStandardArchiveProvider50LA;
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    if(!sourceStandardCallbackEntered_||!sourceStandardWrapperA0_||!sourceStaticOwner_||!worldApplication_||
       !nativeModelZone354_||!sourceStandardZoneS0_||*nativeModelZone354_!=*sourceStandardZoneS0_||
       !sourceStandardSlotS0_||!sourceStandardWrapperSlot_||*sourceStandardWrapperSlot_!=*sourceStandardSlotS0_)return false;
    const auto zone=*nativeModelZone354_,slot=*sourceStandardSlotS0_;
    if(zone>=sourceStaticOwner_->sourceWrapperLedger_.size()||slot>=sourceStaticOwner_->sourceWrapperLedger_[zone].size()||
       !sourceStaticOwner_->sourceWrapperLedger_[zone][slot])return false;
    auto& w=*sourceStaticOwner_->sourceWrapperLedger_[zone][slot];
    if(sourceStandardWrapperA0_!=&w||w.sourceModelCallbackEntered)return false;
    w.sourceModelCallbackEntered=true;at(0x27abd8);
    if(!w.descriptor10||!w.descriptor10->sourceFields)return false;
    auto& d=*w.descriptor10->sourceFields;
    if(!d.loaded||!d.name00Known||d.name00Known>d.name00.size()||d.name00[d.name00Known-1])return false;
    const auto length=d.name00Known-1;
    //Retained known extent must contain the actual C string, not unread tail.
    for(std::size_t i=0;i<length;++i)if(!d.name00[i])return false;
    at(0x27abe4);
    if(length==4&&!std::memcmp(d.name00.data(),"none",4)){
        at(0x27abf8);
        static constexpr char message[]="Unable to load model because the filename is missing.\n";
        w.sourceNoneLogAttempted=true;w.sourceNoneLogWritten=std::fwrite(message,1,sizeof(message)-1,stdout);
        at(0x27ac04);w.sourceCallbackReturnV0=0;at(0x27af90);return true; //Descriptor retained on this distinct branch.
    }
    //Native admission bounds the original sprintf destination without changing
    //its authored suffix or treating overflow as an original failure return.
    if(length>d.name180.size()-5){at(0x27ac14);return false;}
    at(0x27ac14);std::memcpy(d.name180.data(),d.name00.data(),length);
    std::memcpy(d.name180.data()+length,".slt",5);d.name180Known=length+5;w.sourceModelNameFormatted=true;
    auto& authority=worldApplication_->entityAuthority_;
    if(worldApplication_->attachmentLock423F58()||
       worldApplication_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max()||
       authority.enrollmentSerial_==std::numeric_limits<std::uint64_t>::max()||!authority.reserveAdditional(1))return false;
    at(0x27ac1c);w.sourceModelAllocationAttempted=true;
    w.sourceModel.reset(new(std::nothrow)Static::SourceModelFrame);
    w.sourceModelAllocationReturned=true;
    if(w.sourceModel){
        auto& f=*w.sourceModel;
        if(!authority.canEnroll(*f.scene.wreckModelStorage(),ra_static_model_cold50LA::Owner::key(f.scene.wreckModelStorage())))return false;
        g::OriginalRacerModelSceneBinding50LA binding;
        binding.sessionGeneration=worldEntity_.sessionGeneration;
        binding.actualObjectCount423F60=&worldApplication_->objectCount423F60();
        binding.actualAttachmentLock423F58=&worldApplication_->attachmentLock423F58();
        binding.actualTypes383408=&worldApplication_->types383408();
        binding.modelIndex=static_cast<g::SceneNodeIndex49P>(0); //No geometry rows before actual lookup.
        at(0x27ac30);const auto constructed=f.scene.constructSourceDirectRoot2403A0(binding,f.graph(),true);
        f.counted=f.scene.sourceConstructor_.entityCounted;
        if(f.counted){if(!authority.enroll(*f.scene.wreckModelStorage()))return false;f.enrolled=true;}
        if(!constructed.okay()){at(constructed.sourcePC);return false;}
    }
    at(0x27ac38);w.model8=w.sourceModel?&w.sourceModel->scene:nullptr;w.sourceModelPublished=true;
    at(0x27ac48);if(!w.model8)return false; //Source dereference of nullable allocation, not graceful loader failure.
    auto& f=*w.sourceModel;at(0x27ac4c);if(!d.dynamic144)return false;f.dynamicArgument=*d.dynamic144;
    at(0x27ac54);at(0x240740);at(0x26e848);
    const auto noOp=f.scene.sourceNoOp141C20();if(!noOp.okay()){at(noOp.sourcePC);return false;}
    //Exact live directory and ASCII authored name, not prepared model metadata.
    //Native allocation/unsupported-character refusal preserves prior ctor stores.
    try{
        const auto& directory=sourceLoadApplication_->modelDirectory36B860_;
        f.requestedPsg=directory;
        if(!directory.empty()&&directory.back()!='\\')f.requestedPsg+='\\';
        f.requestedPsg.append(d.name180.data(),d.name180Known-1);
        for(char& c:f.requestedPsg){const auto u=static_cast<unsigned char>(c);if(u>=128){at(0x26e8d8);return false;}
            if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');}
        const auto dot=f.requestedPsg.find_last_of('.');
        if(dot==std::string::npos)f.requestedPsg+=".psg";else f.requestedPsg.replace(dot,std::string::npos,".psg");
    }catch(...){at(0x26e864);return false;}
    at(0x26e934);const auto named=f.scene.prepareDirectRoot26E934(f.graph(),f.requestedPsg);
    if(!named.okay()){at(named.sourcePC);return false;}f.nameStored=true;
    at(0x26e9ac);
    if(!sourceTrackMountDomain_||sourceTrackMountLease_.closed_||!sourceTrackMountDomain_->prepared())return false;
    f.provider=sourceTrackMountDomain_->archiveProvider3E1A70();f.resolver=sourceTrackMountDomain_->resolver3E1A70();
    if(!f.provider||!f.resolver||sourceTrackMountLease_.provider_!=f.provider)return false;
    f.lookupEntered=true;
    const auto lookup=f.provider->lookup132820(f.requestedPsg,f.member);
    if(lookup==Provider::Lookup::unknown)return false; //Unresolved is NOT source absent.
    //Provider absence is not original132820/12DCB0 fallback return0. The
    //earlier native-only lookup path is superseded: never issue source release
    //or wrapper retirement without a genuine source missing-return producer.
    if(lookup==Provider::Lookup::absent)return false;
    if(!f.member.member||!f.member.data||!f.member.size)return false;
    f.providerResolved=true;
    //PSG binding itself does not authenticate provider bytes. Pin/recheck the
    //actual mounted member first; use the SAME App resolver, never a new cache.
    a::OriginalStandardTextureDiagnostic50LA diagnostic;a::OriginalStandardModelFileId50LA id;
    try{
        std::vector<std::uint8_t> bytes(f.member.data,f.member.data+f.member.size);
        if(!f.resolver->bindModelMember(*f.member.member,a::OriginalStandardModelFileKind50LA::psg,bytes,id,diagnostic))return false;
    }catch(...){return false;}
    if(!f.resolver->lookupModel132820(f.requestedPsg,a::OriginalStandardModelFileKind50LA::psg,f.file,diagnostic)||
       f.file.id.resolver!=f.resolver.get()||f.file.id.row!=id.row||!f.file.bytes||
       f.file.bytes->size()!=f.member.size||std::memcmp(f.file.bytes->data(),f.member.data,f.member.size))return false;
    f.resolverResolved=true;
    if(f.file.geometry240110){
        //The native weak-cache lock identifies a candidate only. Release its
        //temporary shared ownership before real132820/104960; creator remains
        //the sole strong Geometry owner and the source retain is separate.
        f.warmGeometryHistory=f.file.geometry240110.get();f.file.geometry240110.reset();
        f.warmLookupAttempted=true;
        bool passedCurrent=false;
        for(const auto& row:sourceStaticOwner_->sourceWrapperLedger_)for(const auto& previous:row){
            if(previous.get()==&w){passedCurrent=true;continue;}
            if(passedCurrent||!previous||!previous->sourceModel)continue;
            auto& candidate=*previous->sourceModel;
            if(candidate.nativeRetired||candidate.scene.sourceGeometry_.owner.get()!=f.warmGeometryHistory)continue;
            if(f.warmCreator||!previous->sourceCallbackReturnV0||*previous->sourceCallbackReturnV0!=1||
               !previous->sourceDescriptorDisposed||previous->model8!=&candidate.scene||
               candidate.loaderReturnedEntity!=&candidate.scene.model_.entity8||
               !candidate.sourceFile||!candidate.sourceCloseCallReturned||
               !candidate.scene.preflightSourceRenderableReturn26E820Native()||
               !candidate.sourceFile->associatedGeometryCurrent(f.warmGeometryHistory,&candidate.scene))return false;
            f.warmCreator=&candidate;
        }
        if(!f.warmCreator)return false; //Native/prebuilt cache is not source prototype authority.
        if(!a::OriginalSourcePsgFile50LA::captureWarm132820(f.resolver,sourceTrackFile_,f.requestedPsg,
            f.file.id,f.warmCreator->sourceFile,f.warmGeometryHistory,&f.warmCreator->scene,f.sourceFile,diagnostic)){
            at(diagnostic.sourcePC);return false;}
        if(!f.sourceFile||!f.sourceFile->returnedWarm()||f.sourceFile->sourceHandle()!=-1||
           !f.sourceFile->warmGeometryCurrent(f.warmGeometryHistory,&f.warmCreator->scene))return false;
        f.lookupReturn=true;f.warmAcquireEntered=true;at(0x26e9f0);
        const auto warm=f.scene.acquireSourceWarmGeometry26E9F0(f.sourceFile);at(warm.sourcePC);
        if(warm.sourcePC!=0x26ea88u||!f.scene.sourceWarmGeometry_.prototypeRead||
           !f.scene.preflightSourceWarmGeometryNative())return false;
        f.warmRowsEntered=true;
        const auto warmRows=f.scene.continueSourceWarmRows26EA88(f.graph(),f.warmCreator->graph());
        at(warmRows.sourcePC);
        if(warmRows.sourcePC==0x28e5f0u&&f.scene.sourceWarmRows_.cloneParentCleared){
            //Native authority capacity only; source allocations/counts remain
            //the actual sibling-before-child28E5F0 calls below.
            if(authority.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-(a::maxModelNodes49O-1)||
               !authority.reserveAdditional(a::maxModelNodes49O-1))return false;
            f.warmCloneEntered=true;auto graph=f.graph();
            const auto cloned=f.scene.cloneSourceWarmHierarchy28E5F0(graph,a::maxModelNodes49O,f.warmCreator->graph());
            f.graphCount=graph.count;at(cloned.sourcePC);
            //Before inspecting source return, enrol every actual counted
            //allocation, including a constructor refusal before publication.
            for(std::uint32_t row=1;row<f.scene.binding_.sourceRowCount;++row){
                auto* object=f.scene.sourceRowObject(row);if(!object||!object->constructed||object->released)continue;
                const auto physical=object->sceneIndex;
                if(physical==0||physical>=f.warmCloneEnrolled.size()||f.warmCloneEnrolled[physical])return false;
                if(!authority.enroll(object->entity8))return false;
                f.warmCloneEnrolled[physical]=&object->entity8;
            }
            const auto cloneReturnPC=f.scene.sourceWarmRows_.bit191?0x26eab0u:0x26eb24u;
            if(!cloned.okay()||cloned.sourcePC!=cloneReturnPC||!f.scene.sourceWarmClone_.returned)return false;
            //Selected-child returns through26EAB0/26EAB4, whose delay-slot
            //literal1 seeds the same stack count. No new191 producer exists.
            //A NULL selected child still bounds at its actual dereference;
            //root siblings remain owned even if enumeration excludes them.
            f.warmCloneReturned=true;f.warmEnumerationEntered=true;
            const auto enumerated=f.scene.enumerateSourceWarmRows28C400(f.graph());at(enumerated.sourcePC);
            if(!enumerated.okay()||enumerated.sourcePC!=0x26eb40u||!f.scene.sourceWarmEnumeration_.mapBound)return false;
        }else if(!warmRows.okay()||warmRows.sourcePC!=0x26eb40u)return false;
        if(!f.scene.sourceWarmRows_.enumerationReturned||!f.scene.binding_.sourceRowCount||
           f.scene.binding_.sourceRowCount!=f.scene.sourceWarmRows_.count)return false;
        f.warmRowsReturned=true;f.warmMaterialsEntered=true;
        const auto warmMaterials=f.scene.copySourceWarmMaterials26EB40();at(warmMaterials.sourcePC);
        if(!warmMaterials.okay()||warmMaterials.sourcePC!=0x26ebb0u||!f.scene.sourceWarmMaterials_.returned)return false;
        f.warmMaterialsReturned=true;f.warmParametersEntered=true;
        const auto warmParameters=f.scene.copySourceWarmParameters2406E0();at(warmParameters.sourcePC);
        if(!warmParameters.okay()||warmParameters.sourcePC!=0x26ebc8u||!f.scene.sourceWarmReturn_.parametersReturned)return false;
        f.warmParametersReturned=true;f.warmRowTableEntered=true;
        const auto warmTable=f.scene.allocateSourceWarmRowTable26EE9C();at(warmTable.sourcePC);
        if(!warmTable.okay()||warmTable.sourcePC!=0x26eee8u||
           !f.scene.preflightSourceWarmRenderable26E820Native())return false;
        const auto* renderable=f.scene.sourceRenderableEntity26E820Native();
        if(renderable!=&f.scene.model_.entity8)return false;
        f.warmRowTableReturned=true;f.renderableReturnedEntity=renderable;
        return finishSourceStandardRenderable24076C(w,pc);
    }
    //Native file ID is NOT members150/current160. Capture authentic original
    //132820 search and131380 open against the same joined track and member.
    if(!a::OriginalSourcePsgFile50LA::capture132820(f.resolver,sourceTrackFile_,f.requestedPsg,
        f.file.id,f.sourceFile,diagnostic)){at(diagnostic.sourcePC);return false;}
    if(!f.sourceFile||!f.sourceFile->returnedCold()||!f.sourceFile->current())return false;
    f.lookupReturn=true;at(0x26ebcc);
    //Native capacity reservation only. Original signed row count and every
    //child allocation/read remain in the actual sequential importer.
    if(authority.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-(a::maxModelNodes49O-1)||
       !authority.reserveAdditional(a::maxModelNodes49O-1))return false;
    f.rowsCallEntered=true;auto graph=f.graph();
    const auto imported=f.scene.importSourceDirectRows28CAC0(graph,a::maxModelNodes49O,f.resolver,f.file,f.sourceFile);
    f.graphCount=graph.count; //Same fixed storage; no relocated root or early scene decode.
    //Enroll even a counted child whose type lookup/read did not return. Row0
    //is the already-enrolled Model, never a second Entity/reference/count.
    for(std::uint32_t row=1;row<f.scene.binding_.sourceRowCount;++row){
        auto* object=f.scene.sourceRowObject(row);if(!object||!object->constructed||object->released)continue;
        if(!authority.enroll(object->entity8)){at(imported.sourcePC);return false;}f.rowEnrolled[row]=true;
    }
    if(!imported.okay()){at(imported.sourcePC);return false;}
    const auto& rows=f.scene.sourceDirectRows_;
    if(!rows.returned||!rows.returnCount||imported.sourcePC!=0x28cd5c||*rows.returnCount!=f.graphCount)return false;
    f.rowsCallReturned=true;at(0x26ed4c);f.rowsReturnV0=*rows.returnCount;at(0x26ed50);
    if(f.scene.fields_.flags1B3&4u)return false; //Real instanced hierarchy continuation unjoined.
    at(0x26ee18);
    //Only current same-App382B38/36B970, not the native model preparation
    //namespace, a replacement textures prefix, or a decoded material list.
    const auto& domain=sourceLoadApplication_->standardMaterialDomainOwner_;
    if(!domain||domain!=sourceTrackMountDomain_||!domain->prepared()||!sourceLoadApplication_->materialApplication)return false;
    f.materialTable=domain->materials382B38();
    if(!f.materialTable||!f.materialTable->sourceNames17CAD4Current()||
       !f.materialTable->screenApplicationMatches(*sourceLoadApplication_->materialApplication))return false;
    f.materialPrefix=&sourceLoadApplication_->materialDirectory36B970();
    a::OriginalStandardMaterialPath50LA path{f.materialPrefix};a::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
    f.materialCallEntered=true;at(0x26ee1c);
    const auto materials=f.scene.loadSourceMaterials26E640(f.materialTable,path,materialDiagnostic);
    if(!materials.okay()){at(materials.sourcePC);return false;}
    if(!f.scene.sourceMaterials_.returned||materials.sourcePC!=0x26e810)return false;
    f.materialCallReturned=true;at(0x26ee24);
    //Actual next vtable1A8+AC dispatch is240570. Retained modelconstructor
    //identity authorizes this target, not quantization/resource readiness.
    if(f.scene.sourceConstructor_.vtable1A8!=0x3d1f40)return false;
    at(0x26ee30);f.quantizationCallEntered=true;
    const auto quantized=f.scene.loadSourceQuantization240570();
    if(!quantized.okay()){at(quantized.sourcePC);return false;}
    if(!f.scene.sourceQuantization_.returned||quantized.sourcePC!=0x2406dc)return false;
    //Original caller ignores v0. Internal status proves actual function return,
    //not an invented original boolean success/failure branch.
    f.quantizationCallReturned=true;at(0x26ee38);
    f.geometryCallEntered=true;
    const auto geometry=f.scene.allocateSourceGeometry26EE38();
    //The constructor method's phase result is deliberate: it exposes the
    //actual nullable1B4 store, not a loader boolean. NULL and constructor/control
    //refusals preserve their exact prefix; only the real nonNULL call proceeds.
    at(geometry.sourcePC);
    const auto& geometryState=f.scene.sourceGeometry_;
    if(geometry.sourcePC!=0x26ee64||!geometryState.constructorReturned||
       !geometryState.published||!geometryState.published1B4||
       f.scene.fields_.geometry1B4!=geometryState.published1B4)return false;
    f.geometryLoadCallEntered=true;
    const auto loaded=f.scene.loadSourceGeometry26E170();
    //Only genuine26E170 return admits the actual240210 palette suffix on
    //the SAME typed source file/cursor. Partial LOD/surface effects stay bounded.
    at(loaded.sourcePC);
    if(loaded.sourcePC!=0x24024c||!geometryState.lodsReturned)return false;
    f.geometryPaletteCallEntered=true;
    const auto palette=f.scene.finishSourceGeometry240210();
    at(palette.sourcePC);if(!palette.okay())return false;
    if(!geometryState.geometryCallReturned||palette.sourcePC!=0x240394)return false;
    f.geometryPaletteCallReturned=true;
    //Original240210 has no boolean success value; actual caller resumes26EE6C.
    at(0x26ee6c);f.geometryPrototypeCallEntered=true;
    const auto prototype=f.scene.publishSourceGeometryPrototype26EE74();
    at(prototype.sourcePC);if(!prototype.okay())return false;
    if(!geometryState.prototypeStored||prototype.sourcePC!=0x26ee78)return false;
    f.geometryPrototypeCallReturned=true;
    //Actual FileManager1326A0 uses the independent signed source handle.
    //The private service executes source guards/member1C before its native
    //weak crosswalk; no generic native sourceMode cache gate is opened.
    at(0x26ee80);f.sourceCacheCallEntered=true;
    a::OriginalStandardTextureDiagnostic50LA cacheDiagnostic;
    if(!f.sourceFile||!f.sourceFile->associateGeometry1326A0(geometryState.owner,&f.scene,cacheDiagnostic)){
        if(cacheDiagnostic.sourcePC)at(cacheDiagnostic.sourcePC);return false;}
    if(!f.sourceFile->sourceAssociationStored()||!f.sourceFile->nativeCacheAssociated()||
       !f.sourceFile->associatedGeometryCurrent(geometryState.owner.get(),&f.scene))return false;
    f.sourceCacheCallReturned=true;at(0x26ee88);
    //Original caller does not branch on131040 V0. False here is an unjoined
    //native/source boundary, never a fabricated original zero-return result.
    at(0x26ee90);f.sourceCloseCallEntered=true;
    if(!f.sourceFile->closeSource131040(cacheDiagnostic)){
        if(cacheDiagnostic.sourcePC)at(cacheDiagnostic.sourcePC);return false;}
    if(!f.sourceFile->sourceCloseReturned())return false;
    f.sourceCloseCallReturned=true;at(0x26ee98);
    //Original26EE9C allocates the source pointer table only AFTER actualclose.
    f.rowTableCallEntered=true;
    const auto rowsTable=f.scene.allocateClosedSourceRowTable26EE9C();
    at(rowsTable.sourcePC);if(!rowsTable.okay())return false;
    if(rowsTable.sourcePC!=0x26eee8u||!f.scene.preflightSourceRenderableReturn26E820Native())return false;
    const auto* renderable=f.scene.sourceRenderableEntity26E820Native();
    if(renderable!=&f.scene.model_.entity8)return false;
    f.rowTableCallReturned=true;f.renderableReturnedEntity=renderable;
    return finishSourceStandardRenderable24076C(w,pc);
}
}
