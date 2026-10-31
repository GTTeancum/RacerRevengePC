// Included inside SAME ra_static_model_cold50LA::Owner. No alternate model graph.
    bool admitSourceNative(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation)noexcept {
        if(sourceMode_||attempted_||closed_||application_||!parents_.empty()||!models_.empty()||
           !application||!application->application10()||!generation||application->attachmentLock423F58()||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
        sourceMode_=true;sourceThread_=std::this_thread::get_id();
        application_=std::move(application);generation_=generation;
        //Native bookkeeping only, before271124 and any counted constructor.
        try{parents_.reserve(10);}catch(...){return false;}
        return true;
    }
    bool sourcePhysicsWrapperCurrentNative(const StandardWrapper& w)const noexcept {
        const auto& h=w.sourcePhysics;
        if(!w.sourceMode||w.dispatch!=StandardWrapper::Dispatch::physics3D26C0||w.sourceVtable04!=0x3d26c0||
           !h.factoryReturned||!h.body8||!h.descriptorLaneKnown||
           !w.sourceTypeKnown||w.type00!=2||w.sourceModel||w.model8||w.descriptor10||w.smoker0C||
           w.sourceModelCallbackEntered||w.sourceCallbackReturnV0||w.sourceDescriptorDisposed)return false;
        if(h.callbackEntered&&(!h.countedInParent||!h.descriptorReadReturned))return false;
        if(h.nameFormatted&&(!h.callbackEntered||h.setupNameLength>h.setupName.size()-5||
           std::memcmp(h.setupName.data()+h.setupNameLength,".slt",5)))return false;
        if(h.bodyCallEntered&&!h.nameFormatted)return false;
        if(h.bodyOwner){
            if(!h.bodyCallEntered||!h.bodyOwner->preflightNative()||
               (h.bodyOwner->allocationAttempted_&&!h.bodyOwner->nativeWithdrawn_&&(h.bodyOwner->application_!=application_||
                h.bodyOwner->generation_!=generation_||h.bodyOwner->wrapper08_!=&h.body8)))return false;
            if(h.bodyEnrolled&&!h.bodyOwner->countedEntityNative())return false;
            if(h.bodyOwner->nativeWithdrawn_&&*h.body8)return false;
            if(h.bodyCallReturned&&!h.bodyOwner->wrapperPublished_)return false;
        }else if(*h.body8||h.bodyEnrolled||h.bodyCallReturned)return false;
        if(h.nestedCallEntered&&!h.setupEntered)return false;
        if(h.nestedModel){
            if(!h.nestedCallEntered||!h.bodyOwner||!h.nestedModel->preflightNative())return false;
            const auto& nested=*h.nestedModel;
            if(nested.entered_&&!nested.nativeWithdrawn_&&
               (nested.base_!=h.bodyOwner.get()||nested.requestedName_!=h.setupName.data()))return false;
            if(h.nestedModelEnrolled&&!nested.countedModelEntityNative())return false;
            if(h.nestedCallReturned&&(!nested.published_||(!nested.nameEntered_&&nested.pc_!=0x165cc0u)))return false;
            if(h.nestedNameEntered&&!h.nestedCallReturned)return false;
            if(nested.nameEntered_&&!h.nestedNameEntered)return false;
            if(h.nestedNameReturned&&!nested.nameReturned_)return false;
            if(h.nestedReleaseEntered&&!h.nestedLookupReturned)return false;
            if(nested.missingEntered_&&(!h.nestedReleaseEntered||
               (!nested.nativeWithdrawn_&&nested.missing_!=h.missingLookup)))return false;
            if(h.nestedLoaderReturned){
                if(h.nestedLoaderEntity){
                    if(h.nestedReleaseEntered||h.nestedLoaderReturnV0||!h.nestedPacketsReturned||
                       h.nestedLoaderEntity!=nested.loaderReturnedEntity_)return false;
                }else if(!nested.sourceReleaseReturned_||!nested.sourceLoaderReturnV0_||
                    *nested.sourceLoaderReturnV0_||h.nestedLoaderReturnV0!=nested.sourceLoaderReturnV0_)return false;
            }
            if(h.nestedLoaderReturnV0&&!h.nestedLoaderReturned)return false;
            if(h.nestedRowsEntered&&(!h.foundCaptureReturned||h.nestedReleaseEntered))return false;
            if(nested.rowsEntered_&&!h.nestedRowsEntered)return false;
            if(nested.rowsEntered_&&!nested.nativeWithdrawn_){
                if(nested.rowsFile_!=h.foundFile)return false;
                if(!h.foundNativeWithdrawn&&(nested.rowsResolver_!=h.foundResolver||
                   nested.rowsNativeFile_.id.resolver!=h.foundNativeFile.id.resolver||
                   nested.rowsNativeFile_.id.row!=h.foundNativeFile.id.row||
                   nested.rowsNativeFile_.member!=h.foundNativeFile.member||
                   nested.rowsNativeFile_.bytes!=h.foundNativeFile.bytes))return false;
            }
            if(h.nestedRowsReturned&&(!nested.rowsReturned_||!nested.rowsReturnV0_||
               h.nestedRowsReturnV0!=nested.rowsReturnV0_))return false;
            if(h.nestedRowsReturnV0&&!h.nestedRowsReturned)return false;
            if(h.nestedMaterialEntered&&!h.nestedRowsReturned)return false;
            if(nested.materialEntered_&&!h.nestedMaterialEntered)return false;
            if(h.nestedMaterialReturned&&!nested.materialReturned_)return false;
            if(h.nestedQuantizationEntered&&!h.nestedMaterialReturned)return false;
            if(nested.quantizationEntered_&&!h.nestedQuantizationEntered)return false;
            if(h.nestedQuantizationReturned&&!nested.quantizationReturned_)return false;
            if(h.nestedGeometryEntered&&!h.nestedQuantizationReturned)return false;
            if(nested.geometryEntered_&&!h.nestedGeometryEntered)return false;
            if(h.nestedGeometryAllocated&&!nested.geometryAllocated_)return false;
            if(h.nestedGeometryLoadEntered&&!h.nestedGeometryAllocated)return false;
            if(nested.geometryLoadEntered_&&!h.nestedGeometryLoadEntered)return false;
            if(h.nestedGeometryLodsReturned&&!nested.geometryLodsReturned_)return false;
            if(h.nestedGeometryPaletteEntered&&!h.nestedGeometryLodsReturned)return false;
            if(nested.geometryPaletteEntered_&&!h.nestedGeometryPaletteEntered)return false;
            if(h.nestedGeometryReturned&&!nested.geometryReturned_)return false;
            if(h.nestedPrototypeEntered&&!h.nestedGeometryReturned)return false;
            if(nested.prototypeEntered_&&!h.nestedPrototypeEntered)return false;
            if(h.nestedPrototypeReturned&&!nested.prototypeReturned_)return false;
            if(h.nestedCacheEntered&&!h.nestedPrototypeReturned)return false;
            if(h.nestedCacheReturned&&(!h.foundFile||!h.foundFile->sourceAssociationStored()))return false;
            if(h.nestedCloseEntered&&!h.nestedCacheReturned)return false;
            if(h.nestedCloseReturned&&(!h.foundFile||!h.foundFile->sourceCloseReturned()))return false;
            if(h.nestedRowTableEntered&&!h.nestedCloseReturned)return false;
            if(nested.rowTableEntered_&&!h.nestedRowTableEntered)return false;
            if(h.nestedRowTableReturned&&(!nested.rowTableReturned_||!h.nestedRenderableEntity||
               h.nestedRenderableEntity!=nested.renderableReturnedEntity_))return false;
            if(h.nestedRenderableEntity&&!h.nestedRowTableReturned)return false;
            if(h.nestedPacketsEntered&&!h.nestedRowTableReturned)return false;
            if(nested.packetsEntered_&&!h.nestedPacketsEntered)return false;
            if(h.nestedPacketsReturned&&(!nested.packetsReturned_||!h.nestedLoaderReturned||!h.nestedLoaderEntity))return false;
            if(h.nestedLoaderEntity&&!h.nestedPacketsReturned)return false;
            if(h.nestedAttachEntered&&(!h.nestedLoaderReturned||!h.nestedLoaderEntity))return false;
            if(nested.attachEntered_&&!h.nestedAttachEntered)return false;
            if(h.nestedAttachReturned&&!nested.attachReturned_)return false;
            if(h.nestedPlacementEntered&&!h.nestedAttachReturned)return false;
            if(nested.placementEntered_&&!h.nestedPlacementEntered)return false;
            if(h.nestedPlacementReturned&&!nested.placementReturned_)return false;
            if(h.nestedRowEnrolled[0])return false;
            for(std::uint32_t row=1;row<a::maxModelNodes49O;++row){
                if(h.nestedRowEnrolled[row]&&(!h.nestedRowsEntered||
                   h.nestedRowEnrolled[row]!=nested.countedChildEntityNative(row)))return false;
            }
            if(nested.entered_&&!nested.nativeWithdrawn_&&h.descriptor0C&&
               (nested.position_!=h.descriptor0C->common.position110||nested.look_!=h.descriptor0C->common.look120||
                nested.up_!=h.descriptor0C->common.up130))return false;
        }else if(h.nestedModelEnrolled||h.nestedCallReturned)return false;
        if(h.setup165E90Entered&&!h.nestedPlacementReturned)return false;
        if(h.setupNoOpReturned&&!h.setup165E90Entered)return false;
        if(h.visibilityB4Stored&&!h.setupNoOpReturned)return false;
        if(h.collisionB8Stored&&!h.visibilityB4Stored)return false;
        if(h.bodyOwner&&!h.bodyOwner->nativeWithdrawn_&&h.bodyOwner->object_){
            const auto& base=*h.bodyOwner->object_;
            if(bool(base.visibilityB4)!=h.visibilityB4Stored||bool(base.collisionB8)!=h.collisionB8Stored||
               (base.visibilityB4&&*base.visibilityB4!=h.visibilityNode)||
               (base.collisionB8&&*base.collisionB8!=h.collisionNode))return false;
        }
        if(h.sourceBodyD0){
            const auto& body=*h.sourceBodyD0;
            if(!h.setup165E90Entered||!body.preflightNative()||
               (body.attempted_&&(!h.sourceBodyCallEntered||!h.collisionB8Stored||
                  (!body.nativeWithdrawn_&&body.base_!=h.bodyOwner.get()))))return false;
            if(h.sourceBodyCallReturned&&!body.published_)return false;
            if(h.sourceBodyConfigEntered&&!h.sourceBodyCallReturned)return false;
            if(body.configEntered_&&!h.sourceBodyConfigEntered)return false;
            if(body.modelBorrow_&&body.modelBorrow_!=h.nestedModel.get())return false;
            if(h.sourceBodyConfigReturned&&(!body.disableReturned_||body.pc_!=0x165f34u))return false;
            if(h.sourceBodyEnrolled&&!body.countedEntityNative())return false;
        }else if(h.sourceBodyCallEntered||h.sourceBodyEnrolled||h.sourceBodyConfigEntered)return false;
        if(h.sourceBodyAttachEntered&&!h.sourceBodyConfigReturned)return false;
        if(h.sourceBodyD0){
            if(h.sourceBodyD0->attachEntered_&&!h.sourceBodyAttachEntered)return false;
            if(h.sourceBodyAttachReturned&&!h.sourceBodyD0->attachReturned_)return false;
        }else if(h.sourceBodyAttachEntered||h.sourceBodyAttachReturned)return false;
        if(h.sourceGravityD4){const auto& gravity=*h.sourceGravityD4;
            if(!h.sourceBodyAttachReturned||!gravity.preflightNative()||
               (gravity.attempted_&&(!h.sourceGravityCallEntered||
                  (!gravity.nativeWithdrawn_&&gravity.base_!=h.bodyOwner.get()))))return false;
            if(h.sourceGravityCallReturned&&!gravity.published_)return false;
            if(h.sourceGravityTargetEntered&&!h.sourceGravityCallReturned)return false;
            if(gravity.targetEntered_&&!h.sourceGravityTargetEntered)return false;
            if(h.sourceGravityTargetReturned&&!gravity.targetReturned_)return false;
            if(gravity.targetOwner_&&gravity.targetOwner_!=h.sourceBodyD0.get())return false;
            if(h.sourceGravityAttachEntered&&!h.sourceGravityTargetReturned)return false;
            if(gravity.attachEntered_&&!h.sourceGravityAttachEntered)return false;
            if(h.sourceGravityAttachReturned&&!gravity.attachReturned_)return false;
            if(h.sourceGravityEnrolled&&!gravity.countedEntityNative())return false;
        }else if(h.sourceGravityCallEntered||h.sourceGravityEnrolled||h.sourceGravityTargetEntered||h.sourceGravityAttachEntered)return false;
        if(h.collisionNameEntered&&!h.sourceGravityAttachReturned)return false;
        if(h.collisionNameCopied){
            if(!h.collisionNameEntered||h.collisionSetupNameLength>=h.collisionSetupName.size()||
               h.collisionSetupName[h.collisionSetupNameLength])return false;
            const auto originalLength=h.setupNameLength+4u;
            std::optional<std::size_t> dot;
            for(std::size_t i=0;i<originalLength;++i)if(h.setupName[i]=='.')dot=i;
            if(h.collisionDotSearched&&h.collisionSetupDot!=dot)return false;
            if(h.collisionNameReturned){
                if(!h.collisionDotSearched||!dot||h.collisionSetupNameLength!=*dot+5||
                   std::memcmp(h.collisionSetupName.data(),h.setupName.data(),*dot)||
                   std::memcmp(h.collisionSetupName.data()+*dot,"m.col",6))return false;
            }else if(h.collisionSetupNameLength!=originalLength||
                std::memcmp(h.collisionSetupName.data(),h.setupName.data(),originalLength+1))return false;
        }else if(h.collisionDotSearched||h.collisionNameReturned||h.collisionSetupDot)return false;
        if(h.sourceCollisionE0){const auto& collision=*h.sourceCollisionE0;
            if(!h.collisionNameReturned||!collision.preflightNative()||
               (collision.attempted_&&(!h.sourceCollisionCallEntered||
                  (!collision.nativeWithdrawn_&&collision.base_!=h.bodyOwner.get()))))return false;
            if(h.sourceCollisionCallReturned&&!collision.published_)return false;
            if(h.sourceCollisionConfigEntered&&!h.sourceCollisionCallReturned)return false;
            if(collision.modeEntered_&&!h.sourceCollisionConfigEntered)return false;
            if(h.sourceCollisionConfigReturned&&!collision.modeReturned_)return false;
            if(collision.bodyBorrow_&&collision.bodyBorrow_!=h.sourceBodyD0.get())return false;
            if(h.sourceCollisionNameEntered&&!h.sourceCollisionConfigReturned)return false;
            if(collision.nameEntered_&&!h.sourceCollisionNameEntered)return false;
            if(h.sourceCollisionNameReturned&&!collision.nameReturned_)return false;
            if(collision.requestCaptured_&&(collision.requestName_.size()!=h.collisionSetupNameLength||
               std::memcmp(collision.requestName_.data(),h.collisionSetupName.data(),h.collisionSetupNameLength)))return false;
            if(h.sourceCollisionSearchEntered&&!h.sourceCollisionNameReturned)return false;
            if(collision.searchAccepted_&&!h.sourceCollisionSearchAccepted)return false;
            if(h.sourceCollisionSearchAccepted&&(!collision.searchAccepted_||
               (!collision.nativeWithdrawn_&&collision.search_!=h.collisionSearch)))return false;
            if(collision.missingEntered_&&!h.sourceCollisionMissingEntered)return false;
            if(h.sourceCollisionFoundEntered&&(!h.sourceCollisionSearchAccepted||h.sourceCollisionMissingEntered))return false;
            if(collision.foundEntered_&&!h.sourceCollisionFoundEntered)return false;
            if(h.sourceCollisionFoundHeaderReturned&&(!collision.foundB0Stored_||!collision.headerValid_||
               !collision.foundOutputReturned_||(!h.sourceCollisionType0Entered&&collision.pc_!=0x124f80u)))return false;
            if(h.sourceCollisionType0Entered&&!h.sourceCollisionFoundHeaderReturned)return false;
            if(collision.type0Entered_&&!h.sourceCollisionType0Entered)return false;
            if(h.sourceCollisionArraysEntered&&!h.sourceCollisionType0Entered)return false;
            if(collision.arrayEntered_&&!h.sourceCollisionArraysEntered)return false;
            if(h.sourceCollisionNodesEntered&&!h.sourceCollisionArraysEntered)return false;
            if(collision.nodeEntered_&&!h.sourceCollisionNodesEntered)return false;
            if(h.sourceCollisionImportReturned&&(!h.sourceCollisionNodesEntered||
               !collision.nodeCallReturned_||(!h.sourceCollisionResourceEntered&&collision.pc_!=0x126410u)))return false;
            if(h.sourceCollisionResourceEntered&&!h.sourceCollisionImportReturned)return false;
            if(collision.resourceEntered_&&!h.sourceCollisionResourceEntered)return false;
            if(h.sourceCollisionPayloadReturned&&(!h.sourceCollisionResourceEntered||
               !collision.loader124F80Returned_||collision.loader124F80ReturnV0_!=1))return false;
            if(h.sourceCollisionCloseEntered&&!h.sourceCollisionPayloadReturned)return false;
            if(collision.foundCloseEntered_&&!h.sourceCollisionCloseEntered)return false;
            if(h.sourceCollisionLoaderReturned&&(!h.sourceCollisionCloseEntered||!collision.foundCloseReturned_||
               !collision.loader11B920Returned_||collision.loader11B920ReturnV0_!=1))return false;
            if(h.sourceCollisionResultEntered&&!h.sourceCollisionLoaderReturned)return false;
            if(collision.resultEntered_&&!h.sourceCollisionResultEntered)return false;
            if(h.sourceCollisionResultReturned&&(!h.sourceCollisionResultEntered||
               !collision.resultCallReturned_||(!h.sourceCollisionTransformEntered&&collision.pc_!=0x11bd30u)))return false;
            if(h.sourceCollisionTransformEntered&&!h.sourceCollisionResultReturned)return false;
            if(collision.transformEntered_&&!h.sourceCollisionTransformEntered)return false;
            if(h.sourceCollisionTransformReturned&&(!h.sourceCollisionTransformEntered||
               !collision.transformReturned_||collision.pc_!=0x11f678u))return false;
            if(h.sourceCollisionAttachEntered&&!h.sourceCollisionTransformReturned)return false;
            if(collision.attachEntered_&&!h.sourceCollisionAttachEntered)return false;
            if(h.sourceCollisionAttachReturned&&(!h.sourceCollisionAttachEntered||
               !collision.attachReturned_||collision.attachPC_!=0x1660a0u))return false;
            if(h.sourceCollisionEnrolled&&!collision.countedEntityNative())return false;
        }else if(h.sourceCollisionCallEntered||h.sourceCollisionEnrolled||h.sourceCollisionConfigEntered)return false;
        if(h.sourceContactDC){const auto& contact=*h.sourceContactDC;
            if(!h.sourceCollisionAttachReturned||!contact.preflightNative()||
               (contact.attempted_&&(!h.sourceContactCallEntered||
                  (!contact.nativeWithdrawn_&&contact.base_!=h.bodyOwner.get()))))return false;
            if(h.sourceContactCallReturned&&(!h.sourceContactCallEntered||!contact.published_))return false;
            if(h.sourceContactCapacityEntered&&!h.sourceContactCallReturned)return false;
            if(contact.capacityEntered_&&!h.sourceContactCapacityEntered)return false;
            if(h.sourceContactCapacityReturned&&(!h.sourceContactCapacityEntered||!contact.capacityReturned_))return false;
            if(h.sourceContactBodyEntered&&!h.sourceContactCapacityReturned)return false;
            if(h.sourceContactBodyReturned&&(!h.sourceContactBodyEntered||!contact.bodyStored_))return false;
            if(contact.bodyStored_&&!h.sourceContactBodyEntered)return false;
            if(contact.bodyOwner_&&contact.bodyOwner_!=h.sourceBodyD0.get())return false;
            if(h.sourceContactPointsEntered&&!h.sourceContactBodyReturned)return false;
            if(contact.sourcePoints_.attempted&&!h.sourceContactPointsEntered)return false;
            if(h.sourceContactPointsReturned&&(!h.sourceContactPointsEntered||!contact.sourcePoints_.returned))return false;
            if(h.sourceContactScalarsEntered&&!h.sourceContactPointsReturned)return false;
            if(contact.suffixEntered_&&!h.sourceContactScalarsEntered)return false;
            if(h.sourceContactScalarsReturned&&(!h.sourceContactScalarsEntered||!contact.suffixReturned_||contact.suffixPC_!=0x166160u))return false;
            if(h.sourceContactAttachEntered&&!h.sourceContactScalarsReturned)return false;
            if(contact.attachEntered_&&!h.sourceContactAttachEntered)return false;
            if(h.sourceContactAttachReturned&&(!h.sourceContactAttachEntered||!contact.attachReturned_||contact.attachPC_!=0x16616cu))return false;
            if(h.sourceContactEnrolled&&!contact.countedEntityNative())return false;
        }else if(h.sourceContactCallEntered||h.sourceContactCallReturned||h.sourceContactEnrolled||
            h.sourceContactCapacityEntered||h.sourceContactCapacityReturned||h.sourceContactBodyEntered||h.sourceContactBodyReturned||
            h.sourceContactPointsEntered||h.sourceContactPointsReturned||h.sourceContactScalarsEntered||h.sourceContactScalarsReturned||
            h.sourceContactAttachEntered||h.sourceContactAttachReturned)return false;
        if(h.terrainSaved165C50&&(!h.terrainZero27B42C||!h.nestedCallReturned))return false;
        if(h.terrainForwarded165D98&&(!h.terrainSaved165C50||!h.nestedPlacementReturned))return false;
        if(h.terrainSaved165EAC&&(!h.terrainForwarded165D98||!h.setup165E90Entered||!h.bodyOwner))return false;
        if(h.bodyOwner&&(bool(h.bodyOwner->sourceTerrain165E90_)!=h.terrainSaved165EAC||
           (h.bodyOwner->sourceTerrain165E90_&&*h.bodyOwner->sourceTerrain165E90_!=nullptr)))return false;
        if(h.sourceDetectorD8){const auto& detector=*h.sourceDetectorD8;
            if(!h.sourceContactAttachReturned||!h.terrainSaved165EAC||!detector.preflightNative()||
               (detector.attempted_&&(!h.sourceDetectorCallEntered||
                  (!detector.nativeWithdrawn_&&(detector.base_!=h.bodyOwner.get()||detector.contact_!=h.sourceContactDC.get())))))return false;
            if(h.sourceDetectorCallReturned&&(!h.sourceDetectorCallEntered||!detector.published_))return false;
            if(h.sourceDetectorConfigEntered&&!h.sourceDetectorCallReturned)return false;
            if(detector.configureEntered_&&!h.sourceDetectorConfigEntered)return false;
            if(h.sourceDetectorConfigReturned&&(!h.sourceDetectorConfigEntered||!detector.configureReturned_))return false;
            if(h.sourceDetectorEnableEntered&&!h.sourceDetectorConfigReturned)return false;
            if(detector.enabled64Stored_&&!h.sourceDetectorEnableEntered)return false;
            if(h.sourceDetectorEnableReturned&&(!h.sourceDetectorEnableEntered||!detector.enabled64Stored_))return false;
            if(h.sourceDetectorBindingEntered&&!h.sourceDetectorEnableReturned)return false;
            if(detector.bindingEntered_&&!h.sourceDetectorBindingEntered)return false;
            if(h.sourceDetectorBindingReturned&&(!h.sourceDetectorBindingEntered||!detector.bindingReturned_||detector.bindingPC_!=0x1661c8u))return false;
            if(h.sourceDetectorAttachEntered&&!h.sourceDetectorBindingReturned)return false;
            if(detector.attachEntered_&&!h.sourceDetectorAttachEntered)return false;
            if(h.sourceDetectorAttachReturned&&(!h.sourceDetectorAttachEntered||!detector.attachReturned_||detector.attachPC_!=0x1661d8u))return false;
            if(h.sourceDetectorEnrolled&&!detector.countedEntityNative())return false;
        }else if(h.sourceDetectorCallEntered||h.sourceDetectorCallReturned||h.sourceDetectorEnrolled||
            h.sourceDetectorConfigEntered||h.sourceDetectorConfigReturned||h.sourceDetectorEnableEntered||h.sourceDetectorEnableReturned||
            h.sourceDetectorBindingEntered||h.sourceDetectorBindingReturned||h.sourceDetectorAttachEntered||h.sourceDetectorAttachReturned)return false;
        if(h.directCollisionNameEntered&&!h.sourceDetectorAttachReturned)return false;
        if(h.directCollisionNameCopied){
            if(!h.directCollisionNameEntered||h.directCollisionSetupNameLength>=h.directCollisionSetupName.size()||
               h.directCollisionSetupName[h.directCollisionSetupNameLength])return false;
            const auto originalLength=h.setupNameLength+4u;
            std::optional<std::size_t> dot;
            for(std::size_t i=0;i<originalLength;++i)if(h.setupName[i]=='.')dot=i;
            if(h.directCollisionDotSearched&&h.directCollisionSetupDot!=dot)return false;
            if(h.directCollisionNameReturned){
                if(!h.directCollisionDotSearched||!dot||h.directCollisionSetupNameLength!=*dot+5||
                   std::memcmp(h.directCollisionSetupName.data(),h.setupName.data(),*dot)||
                   std::memcmp(h.directCollisionSetupName.data()+*dot,"f.col",6))return false;
            }else if(h.directCollisionSetupNameLength!=originalLength||
                std::memcmp(h.directCollisionSetupName.data(),h.setupName.data(),originalLength+1))return false;
        }else if(h.directCollisionDotSearched||h.directCollisionNameReturned||h.directCollisionSetupDot)return false;
        if(h.sourceCollisionE4){const auto& direct=*h.sourceCollisionE4;
            if(!h.directCollisionNameReturned||!direct.preflightNative()||
               (direct.attempted_&&(!h.sourceDirectCollisionCallEntered||!direct.isDirectE4()||
                  (!direct.nativeWithdrawn_&&direct.base_!=h.bodyOwner.get()))))return false;
            if(direct.directPredecessorHistory_&&direct.directPredecessorHistory_!=h.sourceDetectorD8.get())return false;
            if(h.sourceDirectCollisionCallReturned&&(!h.sourceDirectCollisionCallEntered||!direct.published_))return false;
            if(h.sourceDirectCollisionConfigEntered&&!h.sourceDirectCollisionCallReturned)return false;
            if(direct.modeEntered_&&!h.sourceDirectCollisionConfigEntered)return false;
            if(h.sourceDirectCollisionConfigReturned&&(!h.sourceDirectCollisionConfigEntered||!direct.modeReturned_))return false;
            if(h.sourceDirectCollisionPathEntered&&!h.sourceDirectCollisionConfigReturned)return false;
            if(direct.nameEntered_&&!h.sourceDirectCollisionPathEntered)return false;
            if(h.sourceDirectCollisionPathReturned&&(!h.sourceDirectCollisionPathEntered||!direct.nameReturned_))return false;
            if(direct.bodyBorrow_&&direct.bodyBorrow_!=h.sourceBodyD0.get())return false;
            if(h.sourceDirectCollisionSearchEntered&&!h.sourceDirectCollisionPathReturned)return false;
            if(h.sourceDirectCollisionSearchAccepted!=direct.searchAccepted_)return false;
            if(h.sourceDirectCollisionSearchAccepted&&(!h.directCollisionSearch||
               (!direct.nativeWithdrawn_&&direct.search_!=h.directCollisionSearch)))return false;
            if(h.sourceDirectCollisionMissingEntered&&!h.sourceDirectCollisionSearchAccepted)return false;
            if(direct.missingEntered_&&!h.sourceDirectCollisionMissingEntered)return false;
            if(h.sourceDirectCollisionFoundEntered&&!h.sourceDirectCollisionSearchAccepted)return false;
            if(direct.foundEntered_&&!h.sourceDirectCollisionFoundEntered)return false;
            if(h.sourceDirectCollisionHeaderReturned&&(!h.sourceDirectCollisionFoundEntered||!direct.foundB0Stored_||
               !direct.headerValid_||!direct.foundOutputReturned_||(!h.sourceDirectCollisionType0Entered&&direct.pc_!=0x124f80u)))return false;
            if(h.sourceDirectCollisionType0Entered&&!h.sourceDirectCollisionHeaderReturned)return false;
            if(direct.type0Entered_&&!h.sourceDirectCollisionType0Entered)return false;
            if(h.sourceDirectCollisionArraysEntered&&!h.sourceDirectCollisionType0Entered)return false;
            if(direct.arrayEntered_&&!h.sourceDirectCollisionArraysEntered)return false;
            if(h.sourceDirectCollisionNodesEntered&&!h.sourceDirectCollisionArraysEntered)return false;
            if(direct.nodeEntered_&&!h.sourceDirectCollisionNodesEntered)return false;
            if(h.sourceDirectCollisionImportReturned&&(!h.sourceDirectCollisionNodesEntered||!direct.nodeCallReturned_))return false;
            if(h.sourceDirectCollisionResourceEntered&&!h.sourceDirectCollisionImportReturned)return false;
            if(direct.resourceEntered_&&!h.sourceDirectCollisionResourceEntered)return false;
            if(h.sourceDirectCollisionPayloadReturned&&(!h.sourceDirectCollisionResourceEntered||!direct.loader124F80Returned_||
               direct.loader124F80ReturnV0_!=1))return false;
            if(h.sourceDirectCollisionCloseEntered&&!h.sourceDirectCollisionPayloadReturned)return false;
            if(direct.foundCloseEntered_&&!h.sourceDirectCollisionCloseEntered)return false;
            if(h.sourceDirectCollisionCloseReturned&&(!h.sourceDirectCollisionCloseEntered||!direct.foundCloseReturned_||
               !direct.loader11B920Returned_||direct.loader11B920ReturnV0_!=1))return false;
            if(h.sourceDirectCollisionLoaderReturned&&(!direct.loader11B920Returned_||
               (h.sourceDirectCollisionMissingEntered?direct.loader11B920ReturnV0_!=0:
                (!h.sourceDirectCollisionCloseReturned||direct.loader11B920ReturnV0_!=1))||
               (!h.sourceDirectCollisionLateEntered&&direct.pc_!=0x11bbbcu)))return false;
            if(h.sourceDirectCollisionLateEntered&&!h.sourceDirectCollisionLoaderReturned)return false;
            if(direct.directLateEntered_&&!h.sourceDirectCollisionLateEntered)return false;
            if(h.sourceDirectCollisionLateReturned&&(!h.sourceDirectCollisionLateEntered||!direct.directLateReturned_||
               direct.directLatePC_!=0x1662c0u))return false;
            if(h.sourceDirectCollisionTransformEntered&&!h.sourceDirectCollisionLateReturned)return false;
            if(direct.directTransformEntered_&&!h.sourceDirectCollisionTransformEntered)return false;
            if(h.sourceDirectCollisionNullModelBound&&(!h.sourceDirectCollisionTransformEntered||!direct.directNullModelBound_||
               direct.directTransformPC_!=0x11f5b0u))return false;
            if(h.sourceDirectCollisionFoundTransformEntered&&(!h.sourceDirectCollisionLateReturned||!h.sourceDirectCollisionCloseReturned||
               h.sourceDirectCollisionTransformEntered))return false;
            if(direct.directFoundTransformEntered_&&!h.sourceDirectCollisionFoundTransformEntered)return false;
            if(h.sourceDirectCollisionWorldCallReached&&(!h.sourceDirectCollisionFoundTransformEntered||!direct.directWorldCallReached_||
               direct.directFoundTransformPC_!=0x28b550u))return false;
            if(h.sourceDirectCollisionMatrixEntered&&!h.sourceDirectCollisionWorldCallReached)return false;
            if(direct.directMatrix_.attempted&&!h.sourceDirectCollisionMatrixEntered)return false;
            if(h.sourceDirectCollisionMatrixReturned&&(!h.sourceDirectCollisionMatrixEntered||!direct.directMatrix_.returned||
               direct.directMatrix_.pc!=0x11f678u))return false;
            if(h.sourceDirectCollisionAttachEntered&&!h.sourceDirectCollisionMatrixReturned)return false;
            if(direct.attachEntered_&&!h.sourceDirectCollisionAttachEntered)return false;
            if(h.sourceDirectCollisionAttachReturned&&(!h.sourceDirectCollisionAttachEntered||
               !direct.attachReturned_||direct.attachPC_!=0x1662e4u))return false;
            if(h.sourceDirectCollisionEnrolled&&!direct.countedEntityNative())return false;
        }else if(h.sourceDirectCollisionCallEntered||h.sourceDirectCollisionCallReturned||h.sourceDirectCollisionEnrolled||
            h.sourceDirectCollisionConfigEntered||h.sourceDirectCollisionConfigReturned||h.sourceDirectCollisionPathEntered||
            h.sourceDirectCollisionPathReturned||h.sourceDirectCollisionSearchEntered||h.sourceDirectCollisionSearchAccepted||
            h.sourceDirectCollisionMissingEntered||h.sourceDirectCollisionLoaderReturned||h.sourceDirectCollisionLateEntered||
            h.sourceDirectCollisionLateReturned||h.sourceDirectCollisionTransformEntered||h.sourceDirectCollisionNullModelBound||
            h.sourceDirectCollisionFoundEntered||h.sourceDirectCollisionHeaderReturned||h.sourceDirectCollisionType0Entered||
            h.sourceDirectCollisionArraysEntered||h.sourceDirectCollisionNodesEntered||h.sourceDirectCollisionImportReturned||
            h.sourceDirectCollisionResourceEntered||h.sourceDirectCollisionPayloadReturned||h.sourceDirectCollisionCloseEntered||
            h.sourceDirectCollisionCloseReturned||h.sourceDirectCollisionFoundTransformEntered||h.sourceDirectCollisionWorldCallReached||
            h.sourceDirectCollisionMatrixEntered||h.sourceDirectCollisionMatrixReturned||
            h.sourceDirectCollisionAttachEntered||h.sourceDirectCollisionAttachReturned)return false;
        if(h.sourceCollisionE8){const auto& procedural=*h.sourceCollisionE8;
            if(!h.sourceDirectCollisionAttachReturned||!h.sourceCollisionE4||!procedural.preflightNative()||
               (procedural.attempted_&&(!h.sourceProceduralCollisionCallEntered||!procedural.isProceduralE8()||
                (!procedural.nativeWithdrawn_&&procedural.base_!=h.bodyOwner.get()))))return false;
            if(procedural.proceduralPredecessor_&&procedural.proceduralPredecessor_!=h.sourceCollisionE4.get())return false;
            if(h.sourceProceduralCollisionCallReturned&&(!h.sourceProceduralCollisionCallEntered||!procedural.published_))return false;
            if(h.sourceProceduralCollisionConfigEntered&&!h.sourceProceduralCollisionCallReturned)return false;
            if(procedural.modeEntered_&&!h.sourceProceduralCollisionConfigEntered)return false;
            if(h.sourceProceduralCollisionConfigReturned&&(!h.sourceProceduralCollisionConfigEntered||!procedural.modeReturned_))return false;
            if(h.sourceProceduralCollisionAttachEntered&&!h.sourceProceduralCollisionConfigReturned)return false;
            if(procedural.attachEntered_&&!h.sourceProceduralCollisionAttachEntered)return false;
            if(h.sourceProceduralCollisionAttachReturned&&(!h.sourceProceduralCollisionAttachEntered||
               !procedural.attachReturned_||procedural.attachPC_!=0x16632cu))return false;
            if(h.sourceProceduralCollisionLateEntered&&!h.sourceProceduralCollisionAttachReturned)return false;
            if(procedural.proceduralLateEntered_&&!h.sourceProceduralCollisionLateEntered)return false;
            if(h.sourceProceduralCollisionLateReturned&&(!h.sourceProceduralCollisionLateEntered||
               !procedural.proceduralLateReturned_||procedural.proceduralLatePC_!=0x166350u))return false;
            if(h.sourceProceduralCollisionShapeEntered&&!h.sourceProceduralCollisionLateReturned)return false;
            if(procedural.proceduralShapeEntered_&&!h.sourceProceduralCollisionShapeEntered)return false;
            if(h.sourceProceduralCollisionShapeResult){const auto& result=*h.sourceProceduralCollisionShapeResult;
                if(!h.sourceProceduralCollisionShapeEntered||
                   (result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner&&
                    result.status!=g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;
                if(procedural.proceduralShapeEntered_){
                    if(!procedural.proceduralShapeCallCompleted_||result.sourcePC!=procedural.proceduralShapePC_)return false;
                }else if(result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner||result.sourcePC!=0x11c710u)return false;
            }
            if(h.sourceProceduralCollisionResultEntered&&(!h.sourceProceduralCollisionShapeResult||
               h.sourceProceduralCollisionShapeResult->status!=g::OriginalSourcePhysicsCollision50LA::Status::bounded||
               h.sourceProceduralCollisionShapeResult->sourcePC!=0x11c934u))return false;
            if(procedural.proceduralResultEntered_&&!h.sourceProceduralCollisionResultEntered)return false;
            if(h.sourceProceduralCollisionResultCall){const auto& result=*h.sourceProceduralCollisionResultCall;
                if(!h.sourceProceduralCollisionResultEntered)return false;
                if(procedural.proceduralResultEntered_){
                    if(!procedural.proceduralResultCallCompleted_||result.sourcePC!=procedural.proceduralResultPC_)return false;
                }else if(result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner||result.sourcePC!=0x11c934u)return false;
            }
            if(h.sourceProceduralCollisionResultReturned&&(!h.sourceProceduralCollisionResultCall||
               h.sourceProceduralCollisionResultCall->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
               h.sourceProceduralCollisionResultCall->sourcePC!=0x11ca80u||!procedural.proceduralResultReturned_))return false;
            if(h.sourceProceduralCollisionEnrolled&&!procedural.countedEntityNative())return false;
        }else if(h.sourceProceduralCollisionCallEntered||h.sourceProceduralCollisionCallReturned||
            h.sourceProceduralCollisionEnrolled||h.sourceProceduralCollisionConfigEntered||h.sourceProceduralCollisionConfigReturned||
            h.sourceProceduralCollisionAttachEntered||h.sourceProceduralCollisionAttachReturned||
            h.sourceProceduralCollisionLateEntered||h.sourceProceduralCollisionLateReturned||
            h.sourceProceduralCollisionShapeEntered||h.sourceProceduralCollisionShapeResult||
            h.sourceProceduralCollisionResultEntered||h.sourceProceduralCollisionResultReturned||h.sourceProceduralCollisionResultCall)return false;
        if(h.sourceLocalFilterEntered&~std::uint16_t(0x1ffu))return false;
        const std::uint32_t filterReturnPC[9]={0x166364u,0x166374u,0x166390u,0x16639cu,0x1663a8u,
            0x1663b4u,0x1663c0u,0x1663ccu,0x1663d8u};
        const std::uint32_t filterOwnerPC[9]={0x124160u,0x124170u,0x16638cu,0x123ea0u,0x123ea0u,
            0x123eb0u,0x123eb0u,0x123eb0u,0x123eb0u};
        for(unsigned i=0;i<9;++i){
            const bool entered=(h.sourceLocalFilterEntered&(1u<<i))!=0;const auto& result=h.sourceLocalFilterCalls[i];
            if(!entered){if(result)return false;continue;}
            if(!h.sourceProceduralCollisionResultReturned||!h.sourceCollisionE0||!h.sourceCollisionE4||!h.sourceCollisionE8)return false;
            if(i&&(!h.sourceLocalFilterCalls[i-1]||h.sourceLocalFilterCalls[i-1]->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
               h.sourceLocalFilterCalls[i-1]->sourcePC!=filterReturnPC[i-1]))return false;
            const auto& e0=*h.sourceCollisionE0;const auto& e4=*h.sourceCollisionE4;const auto& e8=*h.sourceCollisionE8;
            bool issued=false,returned=false;std::uint32_t historyPC=0;
            switch(i){
            case 0:issued=returned=e8.filterD4Returned_;historyPC=e8.filterD4PC_;break;
            case 1:issued=e8.filterCategoryEntered_;returned=e8.filterCategoryReturned_;historyPC=e8.filterCategoryPC_;break;
            case 2:issued=returned=e8.filterEnabled_;historyPC=e8.filterEnablePC_;break;
            case 3:issued=returned=e0.filterBCReturned_;historyPC=e0.filterBCPC_;break;
            case 4:issued=returned=e4.filterBCReturned_;historyPC=e4.filterBCPC_;break;
            default:{
                const auto& owner=i<7?e0:e4;const unsigned index=(i-5)%2;
                if(owner.filterEdgeCount_>index){const auto& edge=owner.filterEdges_[index];
                    const auto* target=i==5?h.sourceCollisionE4.get():i==7?h.sourceCollisionE0.get():h.sourceCollisionE8.get();
                    if(edge.rawTarget!=target->publishedIdentity_||(edge.pinned&&edge.target!=target))return false;
                    issued=true;returned=edge.returned;historyPC=edge.pc;
                }break;
            }}
            if(result){
                if(issued){if(result->sourcePC!=historyPC||
                    ((result->status==g::OriginalSourcePhysicsCollision50LA::Status::reached)!=returned))return false;}
                else if(result->status!=g::OriginalSourcePhysicsCollision50LA::Status::owner||result->sourcePC!=filterOwnerPC[i])return false;
                if(returned&&result->sourcePC!=filterReturnPC[i])return false;
            }
        }
        if(h.sourceCollisionE8){const auto& e8=*h.sourceCollisionE8;
            if((e8.filterD4Returned_&&!(h.sourceLocalFilterEntered&1u))||
               (e8.filterCategoryEntered_&&!(h.sourceLocalFilterEntered&2u))||
               (e8.filterEnabled_&&!(h.sourceLocalFilterEntered&4u)))return false;
        }
        if(h.sourceCollisionE0){const auto& e0=*h.sourceCollisionE0;
            if((e0.filterBCReturned_&&!(h.sourceLocalFilterEntered&8u))||
               (e0.filterEdgeCount_>0&&!(h.sourceLocalFilterEntered&32u))||
               (e0.filterEdgeCount_>1&&!(h.sourceLocalFilterEntered&64u)))return false;
        }
        if(h.sourceCollisionE4){const auto& e4=*h.sourceCollisionE4;
            if((e4.filterBCReturned_&&!(h.sourceLocalFilterEntered&16u))||
               (e4.filterEdgeCount_>0&&!(h.sourceLocalFilterEntered&128u))||
               (e4.filterEdgeCount_>1&&!(h.sourceLocalFilterEntered&256u)))return false;
        }
        if(h.sourceSetup165E90Returned){
            if(h.sourceLocalFilterEntered!=0x1ffu)return false;
            for(unsigned i=0;i<9;++i)if(!h.sourceLocalFilterCalls[i]||
                h.sourceLocalFilterCalls[i]->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                h.sourceLocalFilterCalls[i]->sourcePC!=filterReturnPC[i])return false;
        }
        if(h.sourceSetup165C30Returned){
            if(!h.sourceSetup165E90Returned||!h.bodyOwner||!h.sourceSetupReturnedBase||
               h.sourceSetupReturnedBase!=h.bodyOwner->publishedIdentity_)return false;
            if(!h.bodyOwner->nativeWithdrawn_&&(!h.bodyOwner->object_||
               h.sourceSetupReturnedBase!=&h.bodyOwner->object_->entity||!h.body8||*h.body8!=h.sourceSetupReturnedBase))return false;
        }else if(h.sourceSetupReturnedBase||h.sourceSetupPositive27B470)return false;
        const auto& config=h.configure166400;
        if(h.nestedModel&&h.nestedModel->localBounds_.entered&&!config.boundsEntered)return false;
        if(h.sourceBodyD0&&h.sourceBodyD0->dimensionsMass_.entered&&!config.dimensionsEntered)return false;
        if(h.sourceBodyD0&&h.sourceBodyD0->inertia_.entered&&!config.inertiaEntered)return false;
        if(h.sourceContactDC&&h.sourceContactDC->tuning_.entered&&!config.contactEntered)return false;
        if(h.bodyOwner&&h.bodyOwner->sourceConfigurationTail_.entered&&!config.tailEntered)return false;
        if(h.sourceBodyD0&&h.sourceBodyD0->sourceCenterOffset_.entered&&!config.bodyQuadEntered)return false;
        if(h.sourceCollisionE4&&h.sourceCollisionE4->nullMembership_.entered&&!(config.nullMembershipEntered&1u))return false;
        if(h.sourceCollisionE0&&h.sourceCollisionE0->nullMembership_.entered&&!(config.nullMembershipEntered&2u))return false;
        if(h.nestedModel&&h.nestedModel->nullObserver_.entered&&!config.nullObserverEntered)return false;
        if(h.nestedModel&&h.nestedModel->translationCallEntered_&&!config.positionEntered)return false;
        if(h.sourceCollisionE4&&h.sourceCollisionE4->membershipRegistration_.entered&&!config.directMembershipEntered)return false;
        if(h.sourceCollisionE0&&h.sourceCollisionE0->membershipRegistration_.entered&&!config.resultMembershipEntered)return false;
        if(h.nestedModel&&h.nestedModel->modelObserver_.entered&&!config.modelObserverEntered)return false;
        if(config.entered){
            if(!h.sourceSetupPositive27B470||!h.sourceSetup165C30Returned||!h.descriptor0C||
               config.descriptor!=h.descriptor0C.get()||config.receiver!=h.sourceSetupReturnedBase||
               !h.bodyOwner||config.receiver!=h.bodyOwner->publishedIdentity_||!h.nestedModel||
               config.nestedOwner!=h.nestedModel.get()||!config.model14C||config.dispatch08!=0x3cdf80u||config.target74!=0x166400u||
               !config.argumentsPrepared||!config.calleeEntered||!config.scratchReturned||!config.scratchCopied||
               !config.boundsCallReached||config.pc!=0x28d670u||config.callerStackKnown!=0x555u||
               config.argumentFpuKnown!=0xffu||config.calleeFpuKnown!=0xffu||config.savedOldFpuStores!=0xffu||config.savedOldFpuKnown||
               config.centerKnown!=15u||config.halfKnown!=15u||config.scratchKnown!=0xffffu||config.workingKnown!=0xffffu)return false;
            for(unsigned i=0;i<6;++i)if(config.callerStack[2*i]!=(i?0u:h.descriptor0C->scalar284[7]))return false;
            for(unsigned i=0;i<8;++i)if(config.argumentFpu[i]!=h.descriptor0C->scalar284[i==7?8:i]||
                config.calleeFpu[7-i]!=config.argumentFpu[i])return false;
            const std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
            const auto& bounds=h.nestedModel->localBounds_;
            if(bounds.entered&&!config.boundsEntered)return false;
            if(config.boundsEntered&&!config.boundsCallReached)return false;
            if(config.boundsResult){const auto& result=*config.boundsResult;
                if(!config.boundsEntered)return false;
                if(bounds.entered){if(result.sourcePC!=bounds.pc||
                    ((result.status==g::OriginalSourcePhysicsModel50LA::Status::reached)!=bounds.returned)||
                    (!bounds.returned&&result.status!=g::OriginalSourcePhysicsModel50LA::Status::bounded))return false;}
                else if(result.sourcePC!=0x28d670u||result.status==g::OriginalSourcePhysicsModel50LA::Status::reached)return false;
            }
            if(config.boundsReturned&&(!config.boundsResult||!bounds.returned||
               config.boundsResult->status!=g::OriginalSourcePhysicsModel50LA::Status::reached||
               config.boundsResult->sourcePC!=0x28d93cu))return false;
            if(config.center40!=(bounds.centerStored?bounds.center:vf0))return false;
            const auto* dimensions=h.sourceBodyD0?&h.sourceBodyD0->dimensionsMass_:nullptr;
            if(config.dimensionsEntered&&(!config.boundsReturned||!dimensions))return false;
            if(dimensions&&dimensions->entered){
                if(!config.dimensionsEntered||!bounds.returned||dimensions->actualHalfIdentity!=&config.half50||
                   dimensions->rawWeight!=config.argumentFpu[0]||dimensions->halfBefore!=bounds.half)return false;
            }
            if(config.dimensionsResult){const auto& result=*config.dimensionsResult;
                if(!config.dimensionsEntered||!dimensions)return false;
                if(dimensions->entered){if(result.sourcePC!=dimensions->pc||
                    ((result.status==g::OriginalSourcePhysicsBody50LA::Status::reached)!=dimensions->returned)||
                    (!dimensions->returned&&result.status!=g::OriginalSourcePhysicsBody50LA::Status::bounded))return false;}
                else if(result.sourcePC!=0x1664a0u||result.status!=g::OriginalSourcePhysicsBody50LA::Status::owner)return false;
            }
            if(config.dimensionsReturned&&(!config.dimensionsResult||!dimensions||!dimensions->returned||
               !dimensions->massCallReturned||config.dimensionsResult->status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
               config.dimensionsResult->sourcePC!=0x166558u))return false;
            if(config.half50!=(dimensions&&dimensions->halfDoubledStored?dimensions->halfAfter:
                bounds.halfStored?bounds.half:vf0))return false;
            const auto* inertia=h.sourceBodyD0?&h.sourceBodyD0->inertia_:nullptr;
            if(config.inertiaEntered&&(!config.dimensionsReturned||!inertia))return false;
            if(inertia&&inertia->entered){
                if(!config.inertiaEntered||inertia->actualWorkingIdentity!=&config.working60||
                   inertia->rawInertia!=config.argumentFpu[1])return false;
                for(auto word:inertia->workingBefore)if(word)return false;
            }
            if(config.inertiaResult){const auto& result=*config.inertiaResult;
                if(!config.inertiaEntered||!inertia)return false;
                if(inertia->entered){if(result.sourcePC!=inertia->pc||
                    ((result.status==g::OriginalSourcePhysicsBody50LA::Status::reached)!=inertia->returned)||
                    (!inertia->returned&&result.status!=g::OriginalSourcePhysicsBody50LA::Status::bounded))return false;}
                else if(result.sourcePC!=0x166558u||result.status!=g::OriginalSourcePhysicsBody50LA::Status::owner)return false;
            }
            if(config.inertiaReturned&&(!config.inertiaResult||!inertia||!inertia->returned||
               config.inertiaResult->status!=g::OriginalSourcePhysicsBody50LA::Status::reached||
               config.inertiaResult->sourcePC!=0x166618u))return false;
            const auto* tuning=h.sourceContactDC?&h.sourceContactDC->tuning_:nullptr;
            if(config.contactEntered){
                if(!config.inertiaReturned||!tuning)return false;
                for(unsigned i=0;i<8;++i)if(config.contactArguments[i]!=(i<5?config.argumentFpu[i+2]:config.callerStack[0]))return false;
                if(tuning->entered&&tuning->input!=config.contactArguments)return false;
            }
            if(config.contactResult){const auto& result=*config.contactResult;
                if(!config.contactEntered||!tuning)return false;
                if(tuning->entered){if(result.sourcePC!=tuning->pc||
                    ((result.status==g::OriginalSourcePhysicsContact50LA::Status::reached)!=tuning->returned)||
                    (!tuning->returned&&result.status!=g::OriginalSourcePhysicsContact50LA::Status::bounded))return false;}
                else if(result.sourcePC!=0x295c90u||result.status!=g::OriginalSourcePhysicsContact50LA::Status::owner)return false;
            }
            if(config.contactReturned&&(!config.contactResult||!tuning||!tuning->returned||
                config.contactResult->status!=g::OriginalSourcePhysicsContact50LA::Status::reached||config.contactResult->sourcePC!=0x295dc8u))return false;
            if(config.frictionEntered&&(!config.contactReturned||!tuning))return false;
            if(tuning&&tuning->frictionEntered&&(!config.frictionEntered||tuning->frictionInput!=config.argumentFpu[7]))return false;
            if(config.frictionResult){const auto& result=*config.frictionResult;
                if(!config.frictionEntered||!tuning)return false;
                if(tuning->frictionEntered){if(!tuning->frictionReturned||result.sourcePC!=0x295df4u||
                    result.status!=g::OriginalSourcePhysicsContact50LA::Status::reached)return false;}
                else if(result.sourcePC!=0x295dd0u||result.status!=g::OriginalSourcePhysicsContact50LA::Status::owner)return false;
            }
            if(config.frictionReturned&&(!config.frictionResult||!tuning||!tuning->frictionReturned||
                config.frictionResult->status!=g::OriginalSourcePhysicsContact50LA::Status::reached||config.frictionResult->sourcePC!=0x295df4u))return false;
            const auto& tail=h.bodyOwner->sourceConfigurationTail_;
            if(config.tailEntered&&!config.frictionReturned)return false;
            if(tail.entered){
                if(!config.tailEntered||tail.occurrence!=&config||tail.contactIdentity!=h.sourceContactDC.get()||
                    tail.inputKnown!=config.callerStackKnown||!h.bodyOwner->sourceConfigurationTailHistoryNative())return false;
                for(unsigned i=0;i<5;++i)if(tail.arguments[i]!=config.callerStack[2*(i+1)])return false;
            }
            if(config.tailResult){const auto& result=*config.tailResult;
                if(!config.tailEntered)return false;
                if(tail.entered){if(!tail.returned||result.sourcePC!=0x1666b8u||
                    result.status!=g::OriginalSourcePhysicsObject50LA::Status::reached)return false;}
                else if(result.sourcePC!=0x166650u||result.status!=g::OriginalSourcePhysicsObject50LA::Status::owner)return false;
            }
            if(config.returned74&&(!config.tailResult||!tail.returned||
                config.tailResult->status!=g::OriginalSourcePhysicsObject50LA::Status::reached||config.tailResult->sourcePC!=0x1666b8u))return false;
            const auto* centerOffset=h.sourceBodyD0?&h.sourceBodyD0->sourceCenterOffset_:nullptr;
            if(config.updateEntered&&!config.returned74)return false;
            if(config.descriptorQuadLoaded&&(!config.updateEntered||!h.descriptor0C->vectorKnown||
                config.descriptorQuad!=h.descriptor0C->vector2B0))return false;
            if(config.quadBodyD0&&(!config.descriptorQuadLoaded||!h.sourceBodyD0||
                config.quadBodyD0!=h.sourceBodyD0->publishedIdentity_))return false;
            if(config.bodyQuadEntered&&(!config.quadBodyD0||!centerOffset))return false;
            if(centerOffset&&centerOffset->entered&&(!config.bodyQuadEntered||!centerOffset->returned||
                centerOffset->configurationOccurrence!=&config||centerOffset->raw!=config.descriptorQuad||centerOffset->pc!=0x27b59cu))return false;
            if(config.bodyQuadResult){const auto& result=*config.bodyQuadResult;
                if(!config.bodyQuadEntered||!centerOffset)return false;
                if(centerOffset->entered){if(result.sourcePC!=0x27b59cu||result.status!=g::OriginalSourcePhysicsBody50LA::Status::reached)return false;}
                else if(result.sourcePC!=0x27b598u||result.status!=g::OriginalSourcePhysicsBody50LA::Status::owner)return false;
            }
            if(config.bodyQuadReturned&&(!config.bodyQuadResult||!centerOffset||!centerOffset->returned||
                config.bodyQuadResult->status!=g::OriginalSourcePhysicsBody50LA::Status::reached||config.bodyQuadResult->sourcePC!=0x27b59cu))return false;
            if(config.updateCallReached&&(!config.bodyQuadReturned||config.updateDispatch08!=0x3cdf80u||
                config.updateTarget78!=0x1666c0u||config.updatePC!=0x120290u))return false;
            if(config.nullMembershipEntered>3u||(config.nullMembershipEntered&(config.nullMembershipEntered+1u))||
               (config.nullMembershipReturned&~config.nullMembershipEntered)||
               (config.nullMembershipEntered&&!config.updateCallReached)||
               ((config.nullMembershipEntered&2u)&&!(config.nullMembershipReturned&1u)))return false;
            const g::OriginalSourcePhysicsCollision50LA* const unlinkOwners[2]={h.sourceCollisionE4.get(),h.sourceCollisionE0.get()};
            for(unsigned index=0;index<2;++index){
                if(!unlinkOwners[index]){if((config.nullMembershipEntered&(1u<<index))||config.nullMembershipResults[index])return false;continue;}
                const auto& history=unlinkOwners[index]->nullMembership_;
                if(history.entered&&(!(config.nullMembershipEntered&(1u<<index))||history.updateOccurrence!=&config||
                    (history.returned?(!history.observedNull||history.pc!=0x1202d4u):(history.observedNull||history.pc!=0x1202acu))))return false;
                if(config.nullMembershipResults[index]){const auto& result=*config.nullMembershipResults[index];
                    if(!(config.nullMembershipEntered&(1u<<index)))return false;
                    if(history.entered){if(result.sourcePC!=history.pc||result.status!=(history.returned?g::OriginalSourcePhysicsCollision50LA::Status::reached:g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;}
                    else if(result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner||result.sourcePC!=0x120290u)return false;
                }
                if((config.nullMembershipReturned&(1u<<index))&&(!config.nullMembershipResults[index]||!history.returned||
                    config.nullMembershipResults[index]->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                    config.nullMembershipResults[index]->sourcePC!=0x1202d4u))return false;
            }
            const auto& observerRemoval=h.nestedModel->nullObserver_;
            if(config.nullObserverEntered&&config.nullMembershipReturned!=3u)return false;
            if(observerRemoval.entered&&(!config.nullObserverEntered||observerRemoval.updateOccurrence!=&config||
                (observerRemoval.returned?(!observerRemoval.observedNull||observerRemoval.pc!=0x28eac4u):(observerRemoval.observedNull||observerRemoval.pc!=0x28ea9cu))))return false;
            if(config.nullObserverResult){const auto& result=*config.nullObserverResult;
                if(!config.nullObserverEntered)return false;
                if(observerRemoval.entered){if(result.sourcePC!=observerRemoval.pc||result.status!=(observerRemoval.returned?g::OriginalSourcePhysicsModel50LA::Status::reached:g::OriginalSourcePhysicsModel50LA::Status::bounded))return false;}
                else if(result.status!=g::OriginalSourcePhysicsModel50LA::Status::owner||result.sourcePC!=0x28ea80u)return false;
            }
            if(config.nullObserverReturned&&(!config.nullObserverResult||!observerRemoval.returned||
                config.nullObserverResult->status!=g::OriginalSourcePhysicsModel50LA::Status::reached||config.nullObserverResult->sourcePC!=0x28eac4u))return false;
            const auto& translation=h.nestedModel->translation_;
            if(config.positionEntered&&!config.nullObserverReturned)return false;
            if(h.nestedModel->translationCallEntered_&&(!config.positionEntered||
                h.nestedModel->translationUpdateOccurrence_!=&config))return false;
            if(config.positionResult){const auto& result=*config.positionResult;
                if(!config.positionEntered)return false;
                if(h.nestedModel->translationCallEntered_){
                    if(result.sourcePC!=h.nestedModel->translationCallPC_||
                        result.status!=(translation.returned?g::OriginalSourcePhysicsModel50LA::Status::reached:g::OriginalSourcePhysicsModel50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x28add0u||result.status!=g::OriginalSourcePhysicsModel50LA::Status::owner)return false;
            }
            if(config.positionReturned&&(!config.positionResult||!translation.returned||
                config.positionResult->status!=g::OriginalSourcePhysicsModel50LA::Status::reached||config.positionResult->sourcePC!=0x28af90u))return false;
            const auto& orientation=h.nestedModel->orientation_;
            if(config.orientationEntered&&!config.positionReturned)return false;
            if(h.nestedModel->orientationCallEntered_&&(!config.orientationEntered||
                h.nestedModel->orientationUpdateOccurrence_!=&config))return false;
            if(config.orientationResult){const auto& result=*config.orientationResult;
                if(!config.orientationEntered)return false;
                if(h.nestedModel->orientationCallEntered_){
                    if(result.sourcePC!=h.nestedModel->orientationCallPC_||
                        result.status!=(orientation.returned?g::OriginalSourcePhysicsModel50LA::Status::reached:g::OriginalSourcePhysicsModel50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x28a090u||result.status!=g::OriginalSourcePhysicsModel50LA::Status::owner)return false;
            }
            if(config.orientationReturned&&(!config.orientationResult||!orientation.returned||
                config.orientationResult->status!=g::OriginalSourcePhysicsModel50LA::Status::reached||config.orientationResult->sourcePC!=0x28a528u))return false;
            const auto* directUpdate=h.sourceCollisionE4?&h.sourceCollisionE4->postUpdate_:nullptr;
            if(config.directUpdateEntered&&(!config.orientationReturned||!directUpdate))return false;
            if(directUpdate&&directUpdate->attempted&&(!config.directUpdateEntered||
                directUpdate->updateOccurrence!=&config))return false;
            if(h.nestedModel->postPoseWorldCallEntered_&&(!config.directUpdateEntered||
                h.nestedModel->postPoseWorldUpdateOccurrence_!=&config||
                h.nestedModel->postPoseWorldCollisionOccurrence_!=h.sourceCollisionE4.get()))return false;
            if(config.directUpdateResult){const auto& result=*config.directUpdateResult;
                if(!config.directUpdateEntered||!directUpdate)return false;
                if(directUpdate->attempted){
                    if(result.sourcePC!=directUpdate->pc||
                        result.status!=(directUpdate->returned?g::OriginalSourcePhysicsCollision50LA::Status::reached:g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x11fe50u||result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner)return false;
            }
            if(config.directUpdateReturned&&(!config.directUpdateResult||!directUpdate||!directUpdate->returned||
                config.directUpdateResult->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||config.directUpdateResult->sourcePC!=0x11ff5cu))return false;
            const auto* resultUpdate=h.sourceCollisionE0?&h.sourceCollisionE0->postUpdate_:nullptr;
            if(config.resultUpdateEntered&&(!config.directUpdateReturned||!resultUpdate))return false;
            if(resultUpdate&&resultUpdate->attempted&&(!config.resultUpdateEntered||
                resultUpdate->updateOccurrence!=&config))return false;
            if(h.nestedModel->postE4WorldCallEntered_&&(!config.resultUpdateEntered||
                h.nestedModel->postE4WorldUpdateOccurrence_!=&config||
                h.nestedModel->postE4WorldCollisionOccurrence_!=h.sourceCollisionE0.get()))return false;
            if(config.resultUpdateResult){const auto& result=*config.resultUpdateResult;
                if(!config.resultUpdateEntered||!resultUpdate)return false;
                if(resultUpdate->attempted){
                    if(result.sourcePC!=resultUpdate->pc||
                        result.status!=(resultUpdate->returned?g::OriginalSourcePhysicsCollision50LA::Status::reached:g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x11fe50u||result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner)return false;
            }
            if(config.resultUpdateReturned&&(!config.resultUpdateResult||!resultUpdate||!resultUpdate->returned||
                config.resultUpdateResult->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||config.resultUpdateResult->sourcePC!=0x11ff5cu))return false;
            const auto* membership=h.sourceCollisionE4?&h.sourceCollisionE4->membershipRegistration_:nullptr;
            if(config.directMembershipZone){
                if(!config.resultUpdateReturned||config.directMembershipMode!=2u)return false;
                if(h.bodyOwner->object_&&h.bodyOwner->object_->collisionB8!=config.directMembershipZone)return false;
            }else if(config.directMembershipMode||config.directMembershipEntered||config.directMembershipReturned||config.directMembershipResult)return false;
            if(config.directMembershipEntered&&(!config.resultUpdateReturned||!membership||!config.directMembershipZone))return false;
            if(membership&&membership->entered&&(!config.directMembershipEntered||membership->updateOccurrence!=&config||
                membership->zone!=*config.directMembershipZone||membership->mode!=config.directMembershipMode))return false;
            if(config.directMembershipResult){const auto& result=*config.directMembershipResult;
                if(!config.directMembershipEntered||!membership)return false;
                if(membership->entered){
                    if(result.sourcePC!=membership->pc||result.status!=(membership->returned?
                        g::OriginalSourcePhysicsCollision50LA::Status::reached:g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x120070u||result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner)return false;
            }
            if(config.directMembershipReturned&&(!config.directMembershipResult||!membership||!membership->returned||
                config.directMembershipResult->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                config.directMembershipResult->sourcePC!=0x120224u))return false;
            const auto* resultMembership=h.sourceCollisionE0?&h.sourceCollisionE0->membershipRegistration_:nullptr;
            if(config.resultMembershipZone){
                if(!config.directMembershipReturned||config.resultMembershipMode!=2u)return false;
                if(h.bodyOwner->object_&&h.bodyOwner->object_->collisionB8!=config.resultMembershipZone)return false;
            }else if(config.resultMembershipMode||config.resultMembershipEntered||config.resultMembershipReturned||config.resultMembershipResult)return false;
            if(config.resultMembershipEntered&&(!config.directMembershipReturned||!resultMembership||!config.resultMembershipZone))return false;
            if(resultMembership&&resultMembership->entered&&(!config.resultMembershipEntered||resultMembership->updateOccurrence!=&config||
                resultMembership->zone!=*config.resultMembershipZone||resultMembership->mode!=config.resultMembershipMode||
                !resultMembership->resultBacked||!h.sourceCollisionE4||
                resultMembership->precedingRegistrationHistory!=&h.sourceCollisionE4->membershipRegistration_||
                resultMembership->precedingRegistrationPC!=0x120224u))return false;
            if(config.resultMembershipResult){const auto& result=*config.resultMembershipResult;
                if(!config.resultMembershipEntered||!resultMembership)return false;
                if(resultMembership->entered){
                    if(result.sourcePC!=resultMembership->pc||result.status!=(resultMembership->returned?
                        g::OriginalSourcePhysicsCollision50LA::Status::reached:g::OriginalSourcePhysicsCollision50LA::Status::bounded))return false;
                }else if(result.sourcePC!=0x120070u||result.status!=g::OriginalSourcePhysicsCollision50LA::Status::owner)return false;
            }
            if(config.resultMembershipReturned&&(!config.resultMembershipResult||!resultMembership||!resultMembership->returned||
                config.resultMembershipResult->status!=g::OriginalSourcePhysicsCollision50LA::Status::reached||
                config.resultMembershipResult->sourcePC!=0x120224u))return false;
            const auto& modelObserver=h.nestedModel->modelObserver_;
            if(config.modelObserverZone){
                if(!config.directMembershipReturned||!config.resultMembershipReturned||config.modelObserverMode!=2u)return false;
                if(h.bodyOwner->object_&&h.bodyOwner->object_->visibilityB4!=config.modelObserverZone)return false;
            }else if(config.modelObserverMode||config.modelObserverEntered||config.modelObserverResult)return false;
            if(config.modelObserverEntered&&(!config.resultMembershipReturned||!config.directMembershipReturned||!config.modelObserverZone))return false;
            if(modelObserver.entered&&(!config.modelObserverEntered||modelObserver.updateOccurrence!=&config||
                modelObserver.zone!=*config.modelObserverZone||modelObserver.mode!=config.modelObserverMode))return false;
            if(config.modelObserverResult){const auto& result=*config.modelObserverResult;
                if(!config.modelObserverEntered)return false;
                if(modelObserver.entered){
                    if(result.sourcePC!=modelObserver.pc||result.status!=g::OriginalSourcePhysicsModel50LA::Status::bounded)return false;
                }else if(result.sourcePC!=0x28e8a0u||result.status!=g::OriginalSourcePhysicsModel50LA::Status::owner)return false;
            }
            for(unsigned i=0;i<16;++i){
                if(config.scratchA0[i])return false;
                const auto expected=inertia&&inertia->zeroCopyMask==15?inertia->workingAfter[i]:0u;
                if(config.working60[i]!=expected)return false;
            }
            if(!h.nestedModel->nativeWithdrawn_&&(!h.nestedModel->model_||
               config.model14C!=&h.nestedModel->model_->model_||!h.bodyOwner->object_||
               h.bodyOwner->object_->model14C!=config.model14C))return false;
        }else if(config.argumentsPrepared||config.calleeEntered||config.scratchReturned||config.scratchCopied||config.boundsCallReached||
            config.boundsEntered||config.boundsReturned||config.boundsResult||config.dimensionsEntered||config.dimensionsReturned||config.dimensionsResult||config.inertiaEntered||config.inertiaReturned||config.inertiaResult||
            config.contactEntered||config.contactReturned||config.contactResult||config.frictionEntered||config.frictionReturned||config.frictionResult||
            config.tailEntered||config.tailResult||config.returned74||config.updateEntered||config.descriptorQuadLoaded||
            config.bodyQuadEntered||config.bodyQuadReturned||config.bodyQuadResult||config.updateCallReached||config.quadBodyD0||
            config.updateDispatch08||config.updateTarget78||config.updatePC||config.nullMembershipEntered||config.nullMembershipReturned||
            config.nullMembershipResults[0]||config.nullMembershipResults[1]||config.nullObserverEntered||config.nullObserverReturned||config.nullObserverResult||config.positionEntered||config.positionReturned||config.positionResult||
            config.orientationEntered||config.orientationReturned||config.orientationResult||
            config.directUpdateEntered||config.directUpdateReturned||config.directUpdateResult||
            config.resultUpdateEntered||config.resultUpdateReturned||config.resultUpdateResult||
            config.directMembershipEntered||config.directMembershipReturned||config.directMembershipResult||
            config.directMembershipZone||config.directMembershipMode||
            config.resultMembershipEntered||config.resultMembershipReturned||config.resultMembershipResult||
            config.resultMembershipZone||config.resultMembershipMode||
            config.modelObserverEntered||config.modelObserverResult||config.modelObserverZone||config.modelObserverMode||
            config.descriptor||config.receiver||config.nestedOwner||config.model14C||config.dispatch08||config.target74||config.pc||
            config.callerStackKnown||config.argumentFpuKnown||config.calleeFpuKnown||config.savedOldFpuStores||config.savedOldFpuKnown||
            config.centerKnown||config.halfKnown||config.scratchKnown||config.workingKnown)return false;
        if(h.directCollisionSearch&&(!h.sourceDirectCollisionSearchEntered||!h.sourceCollisionE4||
           !h.directCollisionSearch->preflightNative()||h.directCollisionSearch->requestedName()!=h.sourceCollisionE4->lookupName_))return false;
        if(h.collisionSearch&&(!h.sourceCollisionSearchEntered||!h.sourceCollisionE0||
           !h.collisionSearch->preflightNative()||h.collisionSearch->requestedName()!=h.sourceCollisionE0->lookupName_))return false;
        if(h.nestedLookupEntered&&!h.nestedNameReturned)return false;
        if(h.missingLookup){
            if(!h.nestedLookupEntered||!h.nestedModel||!h.missingLookup->preflightNative()||
               h.missingLookup->requestedName()!=h.nestedModel->requestedPsg_)return false;
            if(h.nestedLookupReturned&&(!h.missingLookup->returnedMissing132820()||
               !h.sourceLookupReturnV0||*h.sourceLookupReturnV0!=0))return false;
        }else if(h.nestedLookupReturned||h.sourceLookupReturnV0)return false;
        if(h.foundEntered&&(!h.nestedNameReturned||!h.missingLookup||!h.missingLookup->found_||
           h.missingLookup->sourcePC_!=0x132988u||h.nestedLookupReturned||h.sourceLookupReturnV0))return false;
        if(h.foundCaptureEntered&&(!h.foundEntered||!h.foundNativeResolved))return false;
        if(h.foundFile){
            if(!h.foundCaptureEntered||!h.foundFile->preflightNative()||
               h.foundFile->foundPredecessorNative()!=h.missingLookup.get())return false;
            if(h.foundCaptureReturned&&(!h.foundFile->returnedCold()||!h.foundHandle678||
               *h.foundHandle678!=h.foundFile->sourceHandle()||!h.foundResource684||*h.foundResource684))return false;
        }else if(h.foundCaptureReturned||h.foundHandle678||h.foundResource684)return false;
        if(h.foundNativeWithdrawn&&(h.foundProvider||h.foundResolver||h.foundNativeFile.bytes||
           h.foundGeometryHistory||(h.foundFile&&!h.foundFile->nativeClosed())))return false;
        if(h.collisionZone&&!h.bodyCallReturned)return false;
        if(h.collisionRead&&!h.collisionZone)return false;
        if(h.visibilityZone&&!h.collisionRead)return false;
        if(h.visibilityRead&&!h.visibilityZone)return false;
        if(h.setupEntered&&(!h.visibilityRead||!h.collisionRead||!h.bodyCallReturned))return false;
        if(h.descriptorAllocationReturned&&!h.descriptorAttempted)return false;
        if(h.descriptorPublished&&!h.descriptorAllocationReturned)return false;
        if(h.descriptorReadEntered&&(!h.descriptorPublished||!h.descriptor0C))return false;
        if(h.descriptorReadReturned&&(!h.descriptorReadEntered||!h.descriptorReturnV0||*h.descriptorReturnV0!=1))return false;
        if(h.descriptorReturnV0&&!h.descriptorReadReturned)return false;
        if(h.countedInParent&&!h.descriptorReadReturned)return false;
        if(h.descriptor0C){const auto& d=*h.descriptor0C;const auto& f=d.common;
            if(!h.descriptorAllocationReturned||!f.constructed||f.vtable170!=0x3d2740||
               f.name180Known!=1||f.name180[0]||f.vectorKnown!=31u||
               d.scalarKnown!=0x7ffu||!d.vectorKnown||d.vector2B0!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
            for(auto word:d.scalar284)if(word)return false;
            if(f.loaded!=h.descriptorReadReturned)return false;
            if(h.nameFormatted&&(h.setupNameLength!=f.name180Known-1||
               std::memcmp(h.setupName.data(),f.name180.data(),h.setupNameLength)))return false;
        }else if(h.descriptorReadEntered||h.descriptorReadReturned)return false;
        return true;
    }
    bool sourceModelCurrentNative(const StandardWrapper& w)const noexcept {
        if(w.dispatch==StandardWrapper::Dispatch::physics3D26C0)return sourcePhysicsWrapperCurrentNative(w);
        if(w.sourceModelAllocationAttempted&&(!w.sourceModelNameFormatted||!w.sourceModelCallbackEntered))return false;
        if(w.sourceModelAllocationReturned&&!w.sourceModelAllocationAttempted)return false;
        if(w.sourceModelPublished&&!w.sourceModelAllocationReturned)return false;
        if(w.sourceDescriptorDisposed){
            if(!w.sourceSmokerRead||(*w.sourceSmokerRead&&(!w.sourceSmokerManagerRead||(w.sourceSmokerManager4A0&&
                  (!w.sourceModel||!w.sourceModel->smokerConfigurationReturned||!w.sourceModel->smokerManagerBound||
                   w.sourceModel->smokerManagerHistory!=w.sourceSmokerManager4A0))))||
               w.descriptor10||!w.sourceDescriptorNullStored||!w.sourceCallbackReturnV0||*w.sourceCallbackReturnV0!=1)return false;
            if(!w.sourceModelCleared&&(!w.sourceModel||!w.sourceModel->parentAttachReturned||
               w.sourceModel->loaderReturnedEntity!=&w.sourceModel->scene.model_.entity8))return false;
        }
        if(w.sourceCallbackReturnV0&&*w.sourceCallbackReturnV0>1)return false;
        if(!w.sourceModel)return !w.model8&&(!w.sourceCallbackReturnV0||
            (!*w.sourceCallbackReturnV0&&w.sourceNoneLogAttempted&&w.descriptor10));
        const auto& f=*w.sourceModel;
        if(!w.sourceModelAllocationAttempted||!w.sourceModelAllocationReturned||!f.graphCount||f.graphCount>a::maxModelNodes49O)return false;
        const auto& rows=f.scene.sourceDirectRows_;
        if(!rows.attempted&&!f.scene.sourceWarmClone_.entered&&f.graphCount!=1)return false;
        if(rows.attempted&&(!f.rowsCallEntered||!f.resolverResolved||f.file.geometry240110))return false;
        if(f.rowsCallReturned&&(!rows.returned||!rows.returnCount||!f.rowsReturnV0||*f.rowsReturnV0!=*rows.returnCount))return false;
        if(!f.sourceSmoker.preflightNative())return false;
        if(f.sourceSmoker.allocationAttempted_&&!f.smokerCallEntered)return false;
        if(f.smokerConfigurationReturned&&!f.sourceSmoker.configureReturned_)return false;
        if(f.smokerManagerBound&&(!f.smokerConfigurationReturned||!f.sourceSmoker.managerBound_||
           f.smokerManagerHistory!=f.sourceSmoker.managerIdentity_))return false;
        if(f.nativeRetired){
            if(f.smokerEnrolled||f.emitterEnrolled||f.smokerManagerBorrow||
               (f.smokerCallEntered&&!f.sourceSmoker.nativeWithdrawn_))return false;
            for(bool enrolled:f.rowEnrolled)if(enrolled)return false;
            for(const auto* enrolled:f.warmCloneEnrolled)if(enrolled)return false;
            return !f.counted&&!f.enrolled&&!w.model8&&!f.sourceFile&&!f.provider&&!f.resolver&&!f.materialTable&&!f.materialPrefix;
        }
        if(f.rowEnrolled[0]||f.warmCloneEnrolled[0])return false;
        const auto& packets=f.sourcePackets.sourcePackets_;
        if(!f.sourcePackets.preflightSourcePacketsNative())return false;
        if(f.packetCallEntered&&((!f.rowTableCallReturned&&!f.warmRowTableReturned)||!f.dynamicArgument))return false;
        if(packets.entered&&(!f.packetCallEntered||packets.model!=&f.scene||
           packets.rawA3!=static_cast<std::uint32_t>(*f.dynamicArgument)))return false;
        if(f.packetCallReturned&&(!packets.returned||packets.pc!=0x2407d8u||
           f.loaderReturnedEntity!=packets.returnedEntityV0))return false;
        if(f.loaderReturnedEntity&&!f.packetCallReturned)return false;
        const auto& child=f.scene.model_.entity8;
        const auto& smoker=f.sourceSmoker;
        const C* expectedSourceChild=smoker.outerAttached_&&!smoker.outerDetachedNative_&&!smoker.nativeWithdrawn_?
            (smoker.smoker_?&smoker.smoker_->entity:nullptr):nullptr;
        if(child.firstChild18!=expectedSourceChild||child.descendants24!=(expectedSourceChild?0xffffffffu:0u))return false;
        if(f.parentAttachEntered){
            if(!f.placement.orientationReturned||!f.parentAttachLock||
               f.parentAttachZone>=parents_.size()||!parents_[f.parentAttachZone]||
               f.parentAttachBorrow!=&parents_[f.parentAttachZone]->entity)return false;
            if(f.parentAttached&&(!f.parentAttachReturned||*f.parentAttachLock))return false;
        }else if(f.parentAttachReturned||f.parentAttached||f.parentDetachedNative||f.parentAttachLock||f.parentAttachBorrow)return false;
        if(f.parentAttached&&!f.parentDetachedNative){if(!parents_[f.parentAttachZone]->sourceAuthorityEnrolled||child.parent1C!=f.parentAttachBorrow||child.mask20)return false;}
        else if(child.parent1C||child.previous10||child.next14)return false;
        const auto& placement=f.placement;
        if(placement.entered){
            if(f.loaderReturnV0||f.loaderReturnedEntity!=&f.scene.model_.entity8||w.model8!=&f.scene||
               !g::OriginalStandardPlacementGlobals50LA::parentCurrent(placement.parentGuard)||
               !g::OriginalStandardPlacementGlobals50LA::orientationCurrent(placement.orientationGuard))return false;
            if(placement.shadowStored&&(!placement.shadow148||f.scene.fields_.flags1B3!=
               static_cast<std::uint8_t>((placement.priorFlags1B3&~0x10u)|
               ((static_cast<std::uint32_t>(*placement.shadow148)&1u)<<4))))return false;
            if(placement.nameStored&&(placement.nameLength>127||placement.copiedName[placement.nameLength]||
               f.scene.model_.name38.size()!=placement.nameLength||
               std::memcmp(f.scene.model_.name38.data(),placement.copiedName.data(),placement.nameLength)))return false;
            if(placement.positionReturned&&(!placement.parentChecked||!placement.parentNotSelf))return false;
            if(placement.orientationEntered&&(!placement.positionReturned||!placement.orientationGuard.entered))return false;
            if(placement.orientationReturned&&(!placement.orientationEntered||
               std::memcmp(&f.nodes[0].localC0,&placement.oriented,sizeof placement.oriented)))return false;
            if(placement.positionReturned&&!placement.orientationReturned)for(unsigned lane=0;lane<3;++lane){
                std::uint32_t bits;std::memcpy(&bits,&f.nodes[0].localC0.v[12+lane],4);
                if(bits!=placement.position[lane])return false;
            }
        }
        const auto& q=f.scene.sourceQuantization_;
        if(q.attempted&&(!f.quantizationCallEntered||!f.materialCallReturned))return false;
        if(f.quantizationCallReturned&&(!q.returned||q.pc!=0x2406dc||!q.flagStored||q.readMask!=255))return false;
        if((unsigned(q.readMask)&(unsigned(q.readMask)+1u))!=0u)return false;
        for(unsigned lane=0;lane<8;++lane)if(q.readMask&(1u<<lane)){
            std::uint32_t bits=0;
            if(lane<4)std::memcpy(&bits,&f.scene.fields_.scale1E0[lane],4);
            else if(lane<7)std::memcpy(&bits,&f.scene.fields_.offset1D0[lane-4],4);
            else bits=f.scene.fields_.offsetEnabled1DC;
            if(bits!=q.words[lane])return false;
        }
        if(q.yEqualsOne&&(!q.xEqualsOne||!*q.xEqualsOne))return false;
        if(q.flagStored){
            if(q.readMask!=255||!q.xEqualsOne||(*q.xEqualsOne&&!q.yEqualsOne))return false;
            const bool scaled=!*q.xEqualsOne||!*q.yEqualsOne;
            if(bool(f.scene.fields_.flags1B2&0x40u)!=scaled||!f.scene.quantizationLoaded_)return false;
        }
        const auto& geometry=f.scene.sourceGeometry_;
        if(!f.scene.preflightSourceGeometryNative())return false;
        if(f.geometryCallEntered&&!f.quantizationCallReturned)return false;
        if(geometry.attempted&&!f.geometryCallEntered)return false;
        if(f.geometryLoadCallEntered&&(!geometry.constructorReturned||!geometry.published||
           !geometry.published1B4))return false;
        if(geometry.loadAttempted&&!f.geometryLoadCallEntered)return false;
        if(f.geometryPaletteCallEntered&&(!f.geometryLoadCallEntered||!geometry.lodsReturned))return false;
        if(geometry.paletteAttempted&&!f.geometryPaletteCallEntered)return false;
        if(f.geometryPaletteCallReturned&&(!geometry.geometryCallReturned||
           (!geometry.prototypeAttempted&&geometry.pc!=0x240394u)))return false;
        if(f.geometryPrototypeCallEntered&&!f.geometryPaletteCallReturned)return false;
        if(geometry.prototypeAttempted&&!f.geometryPrototypeCallEntered)return false;
        if(f.geometryPrototypeCallReturned&&(!geometry.prototypeStored||geometry.pc!=0x26ee78u))return false;
        if(f.sourceCacheCallEntered&&(!f.geometryPrototypeCallReturned||!f.sourceFile))return false;
        if(f.sourceCacheCallReturned&&(!f.sourceCacheCallEntered||!f.sourceFile->sourceAssociationStored()))return false;
        if(f.sourceCloseCallEntered&&!f.sourceCacheCallReturned)return false;
        if(f.sourceCloseCallReturned&&(!f.sourceCloseCallEntered||!f.sourceFile->sourceCloseReturned()))return false;
        const auto& rowTable=f.scene.sourceRowTable_;
        if(!f.scene.preflightSourceRowTableNative())return false;
        if(f.rowTableCallEntered&&!f.sourceCloseCallReturned)return false;
        if(rowTable.attempted&&!f.rowTableCallEntered)return false;
        if(f.rowTableCallReturned&&(!rowTable.returned||rowTable.pc!=0x26eee8u||
           rowTable.returnedEntityV0!=&f.scene.model_.entity8||
           f.renderableReturnedEntity!=&f.scene.model_.entity8))return false;
        if(f.renderableReturnedEntity&&!f.rowTableCallReturned&&!f.warmRowTableReturned)return false;
        const auto& warm=f.scene.sourceWarmGeometry_;
        if(!f.scene.preflightSourceWarmGeometryNative())return false;
        if(f.warmLookupAttempted&&(!f.resolverResolved||!f.warmGeometryHistory||f.file.geometry240110||
           f.rowsCallEntered||geometry.attempted||f.materialCallEntered))return false;
        if(f.warmAcquireEntered&&(!f.warmLookupAttempted||!f.lookupReturn||!*f.lookupReturn))return false;
        const auto& warmRows=f.scene.sourceWarmRows_;const auto& warmMaterials=f.scene.sourceWarmMaterials_;
        const auto& warmReturn=f.scene.sourceWarmReturn_;
        if(f.warmRowsEntered&&!warm.prototypeRead)return false;
        const auto& clone=f.scene.sourceWarmClone_;
        if(f.warmCloneEntered&&(!f.warmRowsEntered||!warmRows.cloneParentCleared))return false;
        if(clone.entered&&(!f.warmCloneEntered||f.graphCount!=clone.slotCount||!f.scene.preflightSourceWarmCloneNative()))return false;
        if(f.warmCloneReturned&&(!clone.returned||clone.pc!=(warmRows.bit191?0x26eab0u:0x26eb24u)))return false;
        if(f.warmEnumerationEntered&&!f.warmCloneReturned)return false;
        if(f.scene.sourceWarmEnumeration_.entered&&!f.warmEnumerationEntered)return false;
        for(std::size_t physical=1;physical<f.warmCloneEnrolled.size();++physical){
            const auto* enrolled=f.warmCloneEnrolled[physical];if(!enrolled)continue;
            if(!clone.entered||physical>=clone.slotCount||
               physical>=rr::game::maxSceneNodes49P||f.rowEnrolled[physical])return false;
            const auto physicalIndex=static_cast<rr::game::SceneNodeIndex49P>(physical);
            bool found=false;
            for(std::uint32_t row=1;row<f.scene.binding_.sourceRowCount;++row){
                const auto* object=f.scene.sourceRowObject(row);
                if(object&&object->sceneIndex==physicalIndex&& &object->entity()==enrolled&&object->constructed&&!object->released)found=true;
            }
            if(!found)return false;
        }
        if(warmRows.entered&&!f.warmRowsEntered)return false;
        if(f.warmRowsReturned&&(!warmRows.enumerationReturned||!f.scene.preflightSourceWarmRowsNative()))return false;
        if(f.warmMaterialsEntered&&!f.warmRowsReturned)return false;
        if(warmMaterials.entered&&!f.warmMaterialsEntered)return false;
        if(f.warmMaterialsReturned&&(!warmMaterials.returned||!f.scene.preflightSourceWarmMaterialsNative()))return false;
        if(f.warmParametersEntered&&!f.warmMaterialsReturned)return false;
        if(warmReturn.parametersEntered&&!f.warmParametersEntered)return false;
        if(f.warmParametersReturned&&!warmReturn.parametersReturned)return false;
        if(f.warmRowTableEntered&&!f.warmParametersReturned)return false;
        if(warmReturn.rowAllocationAttempted&&!f.warmRowTableEntered)return false;
        if(f.warmRowTableReturned&&(!warmReturn.returned||warmReturn.returnedEntity!=&f.scene.model_.entity8||
           f.renderableReturnedEntity!=&f.scene.model_.entity8))return false;
        if(warm.attempted&&(!f.warmAcquireEntered||warm.geometry!=f.warmGeometryHistory||
           !f.warmCreator||warm.creator!=&f.warmCreator->scene))return false;
        if(!geometry.attempted&&f.scene.fields_.geometry1B4&&
           (!warm.attempted||!warm.published||warm.nativeWithdrawn||f.scene.fields_.geometry1B4!=warm.geometry))return false;
        const auto& materials=f.scene.sourceMaterials_;
        if(!f.scene.preflightSourceMaterialsNative())return false;
        if(materials.attempted&&(!f.materialCallEntered||!f.rowsCallReturned||!f.materialTable||
           !f.materialPrefix||materials.prefixAddress!=f.materialPrefix))return false;
        if(f.materialCallReturned&&(!materials.returned||materials.pc!=0x26e810))return false;
        if(materials.references&&materials.references->started()&&
           materials.references->table382B38()!=f.materialTable.get())return false;
        //Only exact staged owners may add1C4/1B4. Geometry's constructor
        //receipt is separate from absent native payload/cache readiness.
        if(materials.attempted&&((packets.entered?!f.sourcePackets.preflightSourcePacketsNative():
           bool(f.scene.fields_.packets1F0))||
           (rowTable.attempted?!f.scene.preflightSourceRowTableNative():
               bool(f.scene.fields_.sourceRows1B8||f.scene.rowTable_))||f.scene.fields_.field1C8||
           f.scene.model_.entity8.firstChild18!=expectedSourceChild))return false;
        if(rows.attempted&&(!rows.sourceFile||rows.sourceFile!=f.sourceFile||
           !rows.sourceFile->preflightNative()||!rows.bytes||rows.bytes!=f.file.bytes||rows.resolver!=f.resolver||
           rows.file.resolver!=f.file.id.resolver||rows.file.row!=f.file.id.row||
           rows.cursor>rows.bytes->size()||rows.nativeScratchWithdrawn))return false;
        if(f.scene.binding_.sourceRowCount){
            if(f.warmLookupAttempted){
                if(f.scene.model_.nodeCount1A0!=0)return false;
                if(clone.entered){
                    if(f.scene.binding_.sourceRowCount!=clone.slotCount||f.graphCount!=clone.slotCount)return false;
                    if(f.warmRowsReturned&&(!warmRows.enumerationReturned||!f.scene.sourceWarmEnumeration_.mapBound||
                       warmRows.count!=clone.slotCount))return false;
                }else if(f.graphCount!=1||f.scene.binding_.sourceRowCount!=1||!warmRows.enumerationReturned||
                         !warmRows.rowZeroStored)return false;
            }else if(f.scene.binding_.sourceRowCount!=f.graphCount||!rows.countStored||
               f.scene.model_.nodeCount1A0!=f.graphCount||!rows.rowZeroPublished||
               rows.publishedRows[0]!=&f.scene.model_)return false;
        }else if(f.graphCount!=1)return false;
        for(std::size_t row=1;row<a::maxModelNodes49O;++row){
            const auto* object=f.scene.sourceRowObject(static_cast<std::uint32_t>(row));
            if(f.rowEnrolled[row]&&(!object||!object->constructed||object->released))return false;
            if(object&&object->constructed&&!object->released){const auto& e=object->entity8;
                if(e.reference00!=1||e.sessionGeneration!=generation_||e.objectKey!=key(&e)||e.kind!=K::scene2893A0)return false;
            }
        }
        if(w.model8!=((w.sourceModelPublished&&!f.nativeAliasWithdrawn&&!w.sourceModelCleared)?&f.scene:nullptr))return false;
        if(f.counted!=(f.scene.sourceConstructor_.entityCounted&&!f.sourceSceneReleased)||
           (f.enrolled&&!f.counted))return false;
        if(f.sourceReferenceDropped){
            if(!f.releaseLock423F58||*f.releaseLock423F58||f.enrolled||!f.scene.referenceReleased_||
               f.scene.model_.entity8.reference00||!f.scene.preflightScene2896D0(f.graph()).okay())return false;
        }else if(materials.attempted||warm.attempted){
            if(!f.scene.preflightNativePartialScene2896D0(f.graph(),f.sourceObserver19C,expectedSourceChild).okay())return false;
        }else if(!f.scene.preflightFreshUnlinkedNative(f.graph()).okay())return false;
        if(f.sourceSceneReleased&&(!f.sourceReferenceDropped||!f.scene.released_||!f.scene.model_.released))return false;
        if((f.destructorStoreMask&128u)&&(!f.sourceSceneReleased||f.destructorVtable10!=0x3cc980u))return false;
        if(f.sourceOuterDeleted&&(!f.sourceSceneReleased||f.destructorStoreMask!=255||
           f.destructorVtable10!=0x3cc980u||f.destructorNullSkips!=255))return false;
        if(f.loaderReturnedEntity&&(f.loaderReturnV0||f.loaderReturnedEntity!=&f.scene.model_.entity8||
           !f.counted||f.sourceReferenceDropped||f.sourceOuterDeleted))return false;
        if(f.loaderReturnV0&&(*f.loaderReturnV0||!f.lookupReturn||*f.lookupReturn||!f.absentLogAttempted||
           !f.releaseReturnV0||*f.releaseReturnV0||!f.releaseLock423F58||
           (!*f.releaseLock423F58&&!f.sourceOuterDeleted)))return false;
        if(w.sourceModelCleared&&(!f.loaderReturnV0||*f.loaderReturnV0))return false;
        if(f.counted){const auto* e=&f.scene.model_.entity8;
            if(!e||e->reference00!=(f.sourceReferenceDropped?0u:1u)||e->sessionGeneration!=generation_||e->objectKey!=key(e)||
               e->kind!=K::model2403A0)return false;
        }
        if(f.nameStored&&(!f.scene.sourceConstructor_.noOp141C20Returned||!f.scene.directRootPrepared_||
           (!rows.attempted&&!f.placement.nameStored&&
             (clone.entered&&clone.rows[0].nameCopied?
                (!clone.rows[0].source||clone.rows[0].source!=warmRows.selectedCloneSource||
                 f.scene.model_.name38!=clone.rows[0].source->name38):
                f.scene.model_.name38!=f.requestedPsg))))return false;
        if(f.lookupEntered&&(!f.nameStored||!f.provider||!f.resolver))return false;
        if(f.resolverResolved&&(!f.providerResolved||
           f.file.id.resolver!=f.resolver.get()||!f.file.member||!f.file.bytes))return false;
        return true;
    }
    bool preflightSourceModelParentLinksNative()const noexcept {
        for(std::size_t z=0;z<parents_.size();++z)if(const auto& parent=parents_[z]){
            const C* previous=nullptr;const C* first=nullptr;std::uint32_t descendants=0;
            for(const auto& w:sourceWrapperLedger_[z])if(w&&w->sourceModel){
                const auto& f=*w->sourceModel;
                if(!f.parentAttached||f.parentDetachedNative||f.nativeRetired)continue;
                const auto& e=f.scene.model_.entity8;const auto& s=f.sourceSmoker;
                const C* expected=s.outerAttached_&&!s.outerDetachedNative_&&!s.nativeWithdrawn_?
                    (s.smoker_?&s.smoker_->entity:nullptr):nullptr;
                if(!s.preflightNative())return false;
                if(!f.parentAttachReturned||!f.parentAttachLock||*f.parentAttachLock||
                   f.parentAttachZone!=z||f.parentAttachBorrow!=&parent->entity||
                   e.parent1C!=&parent->entity||e.previous10!=previous||e.mask20||
                   e.descendants24!=(expected?0xffffffffu:0u)||e.firstChild18!=expected)return false;
                descendants|=e.mask20|e.descendants24;
                if(previous&&previous->next14!=&e)return false;
                if(!first)first=&e;previous=&e;
            }
            if(parent->entity.firstChild18!=first||(previous&&previous->next14)||parent->entity.descendants24!=descendants)return false;
        }
        return true;
    }
    bool canCloseSourceNative()const noexcept {
        if(!sourceMode_||sourceThread_!=std::this_thread::get_id())return false;
        if(closed_)return parents_.empty()&&!sourceWorld3C4Borrower_;
        if(!application_||!generation_||application_->attachmentLock423F58()||constructed_||
           !models_.empty()||materials_||resolver_||visibility_||resourcesAttempted_||observersAttempted_||
           nativeCollisionForest_||parents_.size()>10)return false;
        if(sourceLedgerPrepared_&&(!sourceLedgerAttempted_||!sourceFirstTransferred_))return false;
        const auto wrapperCurrent=[&](const StandardWrapper& w)noexcept {
            if(w.dispatch==StandardWrapper::Dispatch::physics3D26C0)return sourcePhysicsWrapperCurrentNative(w);
            if(!w.sourceMode||!sourceModelCurrentNative(w)||(w.smoker0C&&(!w.sourceModel||!w.sourceModel->sourceSmoker.wrapperPublished_))||w.sourceVtable04!=0x3d26f0||
               w.dispatch!=StandardWrapper::Dispatch::standard3D26F0||
               (w.sourceTypeKnown&&w.type00!=1)||
               (w.sourceDescriptorPublished&&!w.sourceDescriptorAllocationReturned))return false;
            return !w.descriptor10||(w.descriptor10->sourceFields&&w.sourceTypeKnown&&w.sourceDescriptorPublished);
        };
        std::size_t counted=0;
        for(std::size_t z=0;z<parents_.size();++z)if(const auto& p=parents_[z]){
            const auto count=sourceLedgerPrepared_?sourceZoneCounts_[z]:467u;
            if(!p->sourceMode||!p->entries34.empty()||p->released||p->zone!=z+1||
               p->sourceEntriesKnown>p->count30||
               (p->sourceEntriesKnown&&!p->sourceEntries34)||
               (p->sourceEntries34&&!p->sourceArrayReturned)||
               (p->sourceArrayReturned&&p->count30!=count))return false;
            if(p->constructed){const auto& e=p->entity;
                if(e.reference00!=1||e.kind!=K::staticModels27B610||e.sessionGeneration!=generation_||
                   e.objectKey!=key(&e)||e.nativeFlags29||e.parent1C||e.previous10||e.next14||
                   e.mask20||e.category28!=0xff||e.flags29!=3)return false;
                ++counted;
            }else if(p->entity.reference00)return false;
            if(p->sourceFirstWrapper){
                if(z||sourceFirstTransferred_||!wrapperCurrent(*p->sourceFirstWrapper))return false;
            }
            if(p->sourcePendingDescriptor&&!p->sourcePendingDescriptor->sourceFields)return false;
            std::uint32_t physicsCount=0;
            if(sourceLedgerPrepared_){
                if(sourceWrapperLedger_[z].size()!=count||p->sourceFirstWrapper)return false;
                for(std::size_t i=0;i<count;++i)if(const auto& w=sourceWrapperLedger_[z][i]){
                    if(i>p->sourceEntriesKnown||!wrapperCurrent(*w))return false;
                    if(w->sourcePhysics.countedInParent)++physicsCount;
                }
            }
            if(physicsCount!=sourcePhysicsCounts_[z])return false;
            for(std::size_t i=0;i<p->sourceEntriesKnown;++i){
                const auto* expected=sourceLedgerPrepared_?sourceWrapperLedger_[z][i].get():
                    (z==0&&i==0?p->sourceFirstWrapper.get():nullptr);
                if((!sourceLedgerPrepared_&&(z||i))||p->sourceEntries34[i]!=expected)return false;
            }
        }
        if(!sourceLedgerPrepared_)for(const auto& row:sourceWrapperLedger_)for(const auto& w:row)if(w)return false;
        return preflightSourceModelParentLinksNative()&&counted<=application_->objectCount423F60();
    }
    bool closeSourceNative()noexcept {
        if(!canCloseSourceNative()||sourceWorld3C4Borrower_)return false;
        if(closed_)return true;
        //Counted source model owners must already be retired by their actual
        //World authority, before wrapper8/descriptor/parent storage can die.
        for(const auto& row:sourceWrapperLedger_)for(const auto& w:row)
            if(w&&((w->sourceModel&&!w->sourceModel->nativeRetired)||
               (w->sourcePhysics.foundFile&&!w->sourcePhysics.foundFile->nativeClosed())||
               (w->sourcePhysics.missingLookup&&!w->sourcePhysics.missingLookup->nativeClosed())||
               (w->sourcePhysics.nestedModel&&w->sourcePhysics.nestedModel->entered_&&
                !w->sourcePhysics.nestedModel->nativeWithdrawn_)||
               (w->sourcePhysics.bodyOwner&&w->sourcePhysics.bodyOwner->allocationAttempted_&&
                !w->sourcePhysics.bodyOwner->nativeWithdrawn_)))return false;
        //Native ledger-only cleanup. No walk of unproduced raw slots and no
        //guest27B650 destructor receipt. All owned wrapper/descriptor payloads
        //precede the real Parent reference/count retirement.
        for(const auto& p:parents_)if(p&&p->sourceAuthorityEnrolled)return false;
        for(auto& row:sourceWrapperLedger_)row.clear();
        for(auto& p:parents_)if(p){
            p->sourcePendingDescriptor.reset();p->sourceFirstWrapper.reset();p->sourceEntries34.reset();
            if(p->constructed&&!p->released){
                --p->entity.reference00;--application_->objectCount423F60();p->released=true;
            }
        }
        parents_.clear();application_.reset();closed_=true;return true;
    }
