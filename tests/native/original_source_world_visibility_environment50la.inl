#pragma once
#include <new>
namespace ra_pod_scene50LA {
bool Owner::allocateSourceVisibilityTree272FE4(std::uint32_t& pc)noexcept {
    auto& f=sourceVisibilityEnvironment_;pc=f.pc=0x272fd4;
    if(f.attempted||f.withdrawn||!worldApplication_||worldApplication_->visibility443A98_||worldApplication_->activeTree442E18_||
       !sourceTopologyMode_||sourceTopologyWithdrawn_)return false;
    f.attempted=true;pc=f.pc=0x272fe4;
    f.tree.reset(new(std::nothrow) g::OriginalVisibilityQuadTree50LA);f.outerReturned=true;
    if(f.tree&&!f.tree->constructSource2AEF70(*worldApplication_,pc)){f.pc=pc;return false;}
    pc=f.pc=0x273000;worldApplication_->visibility443A98_=f.tree.get();f.treePublished=true;
    return true; //Including outerNULL: the source still reads Environment bounds.
}
bool Owner::finishSourceVisibilityEnvironment27303C(const std::array<std::uint32_t,4>& bounds,std::uint32_t& pc)noexcept {
    auto& f=sourceVisibilityEnvironment_;const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    if(!f.attempted||!f.outerReturned||!f.treePublished||f.bounds||f.withdrawn||
       !worldApplication_||worldApplication_->visibility443A98_!=f.tree.get())return false;
    f.bounds=bounds;at(0x27303c);
    if(!f.tree){
        //2AF0D0 ->2639B0 publishes its NULL this BEFORE first nodeCount store.
        at(0x2639b4);worldApplication_->activeTree442E18_=nullptr;f.activeNullPublished=true;
        at(0x2639bc);return false;
    }
    if(!f.tree->configureSource2AF0D0(bounds,pc)){f.pc=pc;return false;}
    at(0x2730bc);if(!f.tree->capacitySource2AF150(pc)){f.pc=pc;return false;}
    at(0x2730cc);
    if(worldApplication_->environment443108_){at(0x2730d0);return false;} //No unrelated raw-global adoption.
    if(!reserveSourceParticleListNative())return false; //Native ledger admission, no source count effect.
    auto& n=f.node;at(0x2730d8);n.attempted=true;n.owner.reset(new(std::nothrow) SearchNode);f.nodeAllocationReturned=true;
    if(n.owner&&!constructSourceSearchNode27F120(n,pc)){f.pc=pc;return false;}
    at(0x2730f8);worldApplication_->environment443108_=n.owner?&n.owner->entity:nullptr;
    n.published=true;f.nodePublished=true;
    at(0x273104);if(!n.owner)return false;
    n.owner->ordinal48=0;n.ordinalKnown=true;
    at(0x27310c);if(!sourceContainer4BC_||!*sourceContainer4BC_)return false;
    auto& z=*n.owner;auto* parent=*sourceContainer4BC_;
    if(!preflightSourceMethodNodeAttachmentsNative(parent))return false;
    at(0x273114);f.attachEntered=true;f.attachLock=worldApplication_->attachmentLock423F58();
    //Standing native mutation domain remains lock0. Source141960's ignored
    //locked return is not converted into a fabricated reciprocal attachment.
    at(0x141970);if(*f.attachLock)return false;
    const auto attached=worldApplication_->entityAuthority_.attachChildren(*parent,z.entity,0,*f.attachLock);
    at(attached.sourcePC);if(attached.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
    z.attached=true;f.attachReturned=true;
    at(0x27f434);if(z.nativeList34||z.sourceTree34)return false;
    at(0x27f454);f.tree->usage10_=1;
    at(0x27f458);z.sourceTree34=f.tree.get();f.alias34Stored=true;
    at(0x10496c);++f.tree->reference00_;f.alias34Retained=true;
    at(0x27f4a4);if(z.nativeList38||z.sourceTree38)return false;
    at(0x27f4c4);f.tree->usage10_=2;
    at(0x27f4c8);z.sourceTree38=f.tree.get();f.alias38Stored=true;
    at(0x10496c);++f.tree->reference00_;f.alias38Retained=true;
    at(0x27315c);--f.tree->reference00_;f.constructorReleased=true;
    at(0x273174);f.returned=true;return true;
}
bool Owner::preflightSourceVisibilityEnvironmentForestNative(const Entity& parent,const Entity*& child,const Entity*& previous)const noexcept {
    const auto& f=sourceVisibilityEnvironment_;
    if(f.withdrawn||!f.node.owner||!f.node.owner->attached)return true;
    const auto& e=f.node.owner->entity;
    if(!f.attachReturned||!f.node.enrolled||child!=&e||e.parent1C!=&parent||e.previous10!=previous||e.mask20||e.descendants24)return false;
    previous=&e;child=e.next14;return true;
}
bool Owner::preflightSourceVisibilityEnvironmentOwnerNative()const noexcept {
    const auto& f=sourceVisibilityEnvironment_;const auto& n=f.node;
    if(f.withdrawn)return !f.tree&&!n.owner&&!n.counted&&!n.enrolled;
    if(!f.attempted)return !f.tree&&!n.owner&&!f.outerReturned&&!f.treePublished&&!f.bounds&&!f.returned;
    if(!worldApplication_||!sourceTopologyMode_||sourceTopologyWithdrawn_||worldApplication_->attachmentLock423F58())return false;
    if(!f.outerReturned||f.treePublished&&f.tree&& !f.tree->sourceConstruction_.constructorReturned)return false;
    if(f.treePublished&&worldApplication_->visibility443A98_!=f.tree.get())return false;
    if(f.bounds&&!f.treePublished)return false;
    if(f.activeNullPublished&&(f.tree||!f.bounds||worldApplication_->activeTree442E18_))return false;
    if(f.tree){
        const auto& t=*f.tree;if(!t.preflightSourceConstructionNative())return false;
        const auto expected=std::uint64_t(t.sourceConstruction_.baseConstructed&&!f.constructorReleased&&!f.nativeConstructorReleased)+
            std::uint64_t(f.alias34Retained&&!f.nativeAliasesWithdrawn)+std::uint64_t(f.alias38Retained&&!f.nativeAliasesWithdrawn);
        if(t.reference00_!=expected||f.constructorReleased&&(!f.alias34Retained||!f.alias38Retained))return false;
        if(t.usage10_!=(f.alias38Stored?2u:f.alias34Stored?1u:0u))return false;
    }else if(n.attempted||f.alias34Stored||f.alias38Stored||f.constructorReleased||f.returned)return false;
    if(f.alias34Retained&&!f.alias34Stored||f.alias38Retained&&!f.alias38Stored||f.alias38Stored&&!f.alias34Retained)return false;
    if(f.nodePublished!=n.published||n.published&&!f.nodeAllocationReturned)return false;
    if(n.published&&worldApplication_->environment443108_!=(n.owner?&n.owner->entity:nullptr))return false;
    if(n.withdrawn)return !n.owner&&!n.counted&&!n.enrolled&&f.nativeAliasesWithdrawn;
    if(!n.owner)return !n.counted&&!n.enrolled&&!n.constructorMask&&!n.ordinalKnown&&!f.attachEntered&&!f.attachReturned&&
        !f.alias34Stored&&!f.alias38Stored&&!f.returned;
    if(!n.attempted||!f.nodeAllocationReturned||!f.tree||!f.tree->sourceConstruction_.capacityReturned||n.withdrawn)return false;
    const auto& z=*n.owner;const auto& e=z.entity;const auto& a=worldApplication_->entityAuthority_;
    if(z.portalLinks30||z.nativeList34||z.nativeList38||z.collectionBacking||z.exclusions3C||z.exclusionSource||z.exclusionsComplete||
       n.zoneVisibility44||n.exclusionAttempted||n.exclusionAllocationReturned||n.exclusionReturned||n.exclusionProduced||e.firstChild18||e.descendants24||e.mask20)return false;
    if(z.sourceTree34!=(f.alias34Stored&&!f.nativeAliasesWithdrawn?f.tree.get():nullptr)||z.sourceTree38!=(f.alias38Stored&&!f.nativeAliasesWithdrawn?f.tree.get():nullptr))return false;
    if(n.counted){if(e.reference00!=1||e.kind!=g::OriginalWreckComponentKind50LA::searchNode27F120||e.objectKey!=key(&e)||
       e.sessionGeneration!=worldEntity_.sessionGeneration||e.category28!=0xff||(e.flags29&15u)!=3u||e.nativeFlags29||
       !worldApplication_->objectCount423F60())return false;}
    else if(e.reference00||z.constructed)return false;
    if(n.enrolled){if(!n.counted||!a.canRetire(e))return false;}
    else for(std::size_t i=0;i<a.count_;++i)if(a.entries_[i].node==&e)return false;
    if(z.constructed){if(n.constructorMask!=511||!n.vtable08||*n.vtable08!=0x3d2dd0||!n.pointer3C||*n.pointer3C||
        !n.pointer44||*n.pointer44||!n.pointerA0||*n.pointerA0||!z.exclusionCount40||*z.exclusionCount40||z.ordinal48)return false;}
    else if(n.constructorMask||n.vtable08||n.pointer3C||n.pointer44||n.pointerA0||z.exclusionCount40)return false;
    if(f.attachReturned&&!f.nativeNodeDetached){if(!f.attachEntered||!f.attachLock||*f.attachLock||!z.attached||!sourceContainer4BC_||
        !*sourceContainer4BC_||e.parent1C!=*sourceContainer4BC_||e.next14)return false;}
    else if(z.attached||e.parent1C||e.previous10||e.next14)return false;
    if(f.returned&&(!f.tree->sourceConstruction_.capacityReturned||!f.nodePublished||!n.ordinalKnown||!f.attachReturned||
       !f.alias34Retained||!f.alias38Retained||!f.constructorReleased||f.pc!=0x273174))return false;
    return true;
}
bool Owner::retireSourceVisibilityEnvironmentNative(std::uint32_t& pc)noexcept {
    auto& f=sourceVisibilityEnvironment_;pc=f.pc;
    if(!preflightSourceVisibilityEnvironmentOwnerNative())return false;
    if(!f.attempted||f.withdrawn)return true;
    auto& n=f.node;
    //Whole-topology floor and authority checks precede mutation. This node is
    //the last source child; withdraw both exact method aliases BEFORE the tree.
    if(n.owner){auto& z=*n.owner;auto& e=z.entity;
        if(n.enrolled&&!worldApplication_->entityAuthority_.canRetire(e))return false;
        if(z.attached){if(!sourceContainer4BC_||!*sourceContainer4BC_||e.next14)return false;
            if(e.previous10)e.previous10->next14=nullptr;else (*sourceContainer4BC_)->firstChild18=nullptr;
            e.parent1C=e.previous10=nullptr;z.attached=false;f.nativeNodeDetached=true; //Mask0 cannot change ancestor OR.
        }
        if(!f.nativeAliasesWithdrawn){
            z.sourceTree38=nullptr;if(f.alias38Retained)--f.tree->reference00_;
            z.sourceTree34=nullptr;if(f.alias34Retained)--f.tree->reference00_;
            f.nativeAliasesWithdrawn=true;
        }
        if(n.enrolled){if(!worldApplication_->entityAuthority_.retire(e))return false;n.enrolled=false;}
        if(n.counted){--e.reference00;--worldApplication_->objectCount423F60();n.counted=false;}
    }
    if(f.nodePublished)worldApplication_->environment443108_=nullptr;
    n.owner.reset();n.withdrawn=true;f.nativeAliasesWithdrawn=true;
    if(f.tree){
        if(f.tree->sourceConstruction_.baseConstructed&&!f.constructorReleased&&!f.nativeConstructorReleased){
            --f.tree->reference00_;f.nativeConstructorReleased=true;
        }
        if(!f.tree->withdrawSourceConstructionNative())return false;
        f.tree.reset();
    }
    f.withdrawn=true;return true; //Native cancellation, never original destructor/return replay.
}
}
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceVisibilityEnvironmentNative()const noexcept {
    if(!preflightSourceVisibilityEnvironmentOwnerNative())return false;
    const auto& f=sourceVisibilityEnvironment_;
    if(!f.attempted)return !sourceVisibilityEnvironmentReturned_&&!sourceVisibilityEnvironmentReturnV0_;
    if(!sourceParticlePredecessorEntered_||!sourceParticleEnvironment35CWritten_||!sourceParticleVisibilityWasNull_||
       !*sourceParticleVisibilityWasNull_)return false;
    if(sourceVisibilityEnvironmentReturned_!=f.returned)return false;
    return sourceVisibilityEnvironmentReturned_?(sourceVisibilityEnvironmentReturnV0_&&*sourceVisibilityEnvironmentReturnV0_==1):
        !sourceVisibilityEnvironmentReturnV0_;
}
bool Owner::constructSourceVisibilityEnvironment272FC0(std::uint32_t& pc)noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourcePostLightPhase_!=SourcePostLightPhase::particleEntered||!sourceParticlePredecessorEntered_||
       !sourceParticleEnvironment35CWritten_||useEnvironment35C_!=1||!sourceParticleVisibilityWasNull_||
       !*sourceParticleVisibilityWasNull_||!sourceEnvironment3A4_||!sourceWorldConfig_||
       !nativeEnvironment3A4_||*nativeEnvironment3A4_!=sourceEnvironment3A4_.get()||
       sourceEnvironmentPhase_!=SourceEnvironmentPhase::returnedOne||
       !preflightSourceWorldTopologyCountNative())return false;
    if(!allocateSourceVisibilityTree272FE4(pc))return false;
    pc=sourceVisibilityEnvironment_.pc=0x273028;std::array<std::uint32_t,4> bounds;
    if(!sourceEnvironment3A4_->readSourceVisibilityBounds273028(worldEntity_,*sourceWorldConfig_,bounds))return false;
    if(!finishSourceVisibilityEnvironment27303C(bounds,pc))return false;
    sourceVisibilityEnvironmentReturnV0_=1;sourceVisibilityEnvironmentReturned_=true;
    return preflightSourceVisibilityEnvironmentNative();
}
}
