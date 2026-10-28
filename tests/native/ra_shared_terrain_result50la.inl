//Included inside OwnershipBridge. Same lexical terrain invocation only.
    bool sharedTerrainLimitedCurrent(const SharedTerrainInvocation& call)const noexcept {
        return call.entered_&&call.cut_==SharedTerrainInvocation::Cut::limited&&
            sharedLeaseCurrent(call.lease_)&&call.result_.nativeTerrainSeedPublished&&
            call.result_.projectionRevision==call.lease_.projection_&&call.row_.terrainResult90&&
            call.row_.terrainResult90->constructed_&&!call.row_.terrainResult90->revoked_&&
            call.row_.terrainResult90->resetKnown_;
    }
    //Pure geometry has returned. Publish the genuine one-ray selected/pair
    //effects BEFORE original candidate copies/E4/E8, never at whole-query end.
    bool commitSharedTerrainGeometry(SharedTerrainInvocation& call,
        const g::CollisionResult48I& hit)noexcept {
        if(!sharedTerrainLimitedCurrent(call)||
           call.row_.effectRevision==std::numeric_limits<std::uint64_t>::max())return false;
        auto& result=*call.row_.terrainResult90;
        if(result.count60_>1u||!call.row_.terrainResource)return false;
        float prior=0.f;std::memcpy(&prior,&result.words6C_[0],sizeof(prior));
        const auto vector=[](const g::Vector3_48C& xyz,float w){
            return g::SpringVector4_48N{{xyz.v[0],xyz.v[1],xyz.v[2],w}};
        };
        const auto& pair=call.row_.terrainResource->pair;
        if(!result.count60_||hit.nearestPair.fraction<prior){
            //1087E8..108898 first append, or108780..1087E4 nearer overwrite.
            //Original single ray tag8000 owns ONE slot, capacity remains3.
            if(!result.count60_){
                result.endpoints50_[0]=pair.first;result.endpoints50_[1]=pair.second;
                result.tags70_[0]=g::type2Leaf49X;result.count60_=1;
            }
            std::memcpy(&result.words6C_[0],&hit.nearestPair.fraction,sizeof(float));
            result.normals54_[0]=vector(hit.nearestPair.normal,2.f);
            result.surfacePoints58_[0]=vector(hit.nearestPair.point,1.f);
            result.words64_[0]=1; //Actual121590..12161C world conversion completed.
        }
        if(hit.selected.fraction<=result.selected_.fraction){
            auto& selected=result.selected_;
            selected.first=pair.first;selected.second=pair.second;
            selected.normal=vector(hit.selected.normal,2.f);
            selected.surfacePoint=vector(hit.selected.point,1.f);
            selected.correction=vector(hit.selected.correction,3.f);
            selected.fraction=hit.selected.fraction;
            selected.flags=static_cast<std::uint8_t>(selected.flags|1u);
            //These are native diagnostic identities, not extra guest stores.
            selected.candidateIdentity=hit.selected.candidateId;selected.face=hit.selected.face;
            selected.queryTag=g::type2Leaf49X;
        }
        auto& flags=call.row_.canonicalFilter->flagsF8;
        flags=static_cast<std::uint8_t>(flags|0x20u);
        storage_.get()->source[call.row_.reservedRef-1u].flagsF8=flags;
        ++call.row_.effectRevision;call.lease_.wrote_=true;return true;
    }
    bool applySharedTerrainCallback(SharedTerrainInvocation& call,
        const g::TerrainCandidate48J& candidate,std::optional<std::size_t> podSlot)noexcept {
        if(!sharedTerrainLimitedCurrent(call)||!actual_||
           !registeredRange(&actual_->frame.terrainProbe,sizeof(actual_->frame.terrainProbe))||
           !registeredRange(&actual_->frame.bodies.main.components.probe348,
               sizeof(actual_->frame.bodies.main.components.probe348)))return false;
        const auto& metadata=actual_->terrain.metadata;
        if(!metadata.terrainCallbackEnabled)return true; //Actual retained native callback binding.
        auto& probe=actual_->frame.bodies.main.components.probe348;
        auto& terrain=actual_->frame.terrainProbe;
        const auto category=candidate.filter.categoryA4;
        call.result_.stoppedPC=0x1b0e50;
        if((category&metadata.callbackMasks.collect)&&probe.field1E8<16u){
            terrain.collectedIdentities[probe.field1E8]=candidate.filter.identity;
            ++probe.field1E8; //Ordered duplicate occurrences remain duplicates.
        }
        call.result_.stoppedPC=0x1b0e94;
        if(category&metadata.callbackMasks.forceAngle)probe.field108=1;
        call.result_.stoppedPC=0x1b0eb0;
        if(category&metadata.callbackMasks.excludeOwner){
            if(podSlot){
                //Source1B0EC4 reads9C only HERE, after collect/108 effects.
                //21224C stored the racer, not212258's separate340/A0 body.
                call.result_.stoppedPC=0x1b0ec4;
                const auto& selected=sharedProbeSelection_;
                if(call.ordinal_>=selected.count||
                   selected.entries[call.ordinal_].kind!=SharedSelectedKind::pod694||
                   selected.entries[call.ordinal_].identity!=candidate.filter.identity||
                   selected.entries[call.ordinal_].index!=*podSlot*storage::Storage::rosterRoles+3u)return false;
                g::NativeRacerTerrainIdentity50LA owner;
                if(!readNativePodRacerOwner9C(call.lease_,*podSlot,owner))return false;
                probe.field12C=owner.value; //Distinct authenticated native racer token.
            }else probe.field12C=candidate.ownerIdentity9C;
        }
        return true; //No early owner read, bodyA0 alias or inferred null.
    }
    void finishSharedTerrainLimited(SharedTerrainInvocation& call)noexcept {
        if(!sharedTerrainLimitedCurrent(call))return;
        auto& out=call.result_;auto& probe=actual_->frame.bodies.main.components.probe348;
        out.stoppedPC=0x1b19b4;
        if((call.row_.canonicalFilter->flagsF8&0x20u)&&!probe.field12C){
            const auto& result=*call.row_.terrainResult90;
            if(!result.count60_||result.count60_>1u)return;
            const auto& selected=result.selected_;
            if(!probe.field110||!(selected.surfacePoint.v[1]<=probe.point150[1])){
                //Point150 retains the full source vector. Existing native normal170
                //owns XYZ only; do not manufacture a separate guest W lane.
                std::memcpy(probe.point150,selected.surfacePoint.v,sizeof(probe.point150));
                std::memcpy(actual_->frame.terrainProbe.normal170.v,selected.normal.v,3*sizeof(float));
                probe.field110=0;probe.field114=1;
            }
        }
        call.cut_=SharedTerrainInvocation::Cut::returned;out.stoppedPC=0x1b1a20;
        //Still inside the actual Context/owner lease, before its terminal guard.
        advanceSharedEngineGravity(call);
        //No219810 return, PostWorld transition or owner-lease completion.
    }
