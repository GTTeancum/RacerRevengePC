#pragma once
#include <new>
#include <limits>
namespace ra_pod_scene50LA {
bool Owner::reserveSourcePortalCohortNative()noexcept {
    if(sourcePortalCohortAdmitted_||!worldApplication_||!worldApplication_->application10()||
       worldApplication_->portalViewportConstructed_||worldApplication_->attachmentLock423F58()||worldEntity_.reference00!=1||!worldEntity_.sessionGeneration||
       worldApplication_->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-10u||
       worldApplication_->entityAuthority_.enrollmentSerial_>std::numeric_limits<std::uint64_t>::max()-10u)return false;
    for(const auto& p:nativePortals_)if(p)return false;
    for(const auto& f:sourcePortalFrames_)if(f.attempted||f.loadAttempted||f.withdrawn||f.lastParameterLoad||f.parameterLoads)return false;
    if(!worldApplication_->entityAuthority_.reserveAdditional(10))return false;
    sourcePortalCohortAdmitted_=true;return true; //Native capacity only, not source allocation/constructor.
}
bool Owner::allocateSourcePortal230370(std::size_t i,std::uint32_t& pc)noexcept {
    pc=0x272b3c;if(i>=10||!sourcePortalCohortAdmitted_||sourcePortalFrames_[i].attempted||nativePortals_[i])return false;
    auto& f=sourcePortalFrames_[i];f.attempted=true;f.pc=pc;
    nativePortals_[i].reset(new(std::nothrow) NativePortal230370);
    pc=f.pc=0x272b48;if(!nativePortals_[i])return true; //Actual s3=NULL; loader is still called.
    auto& p=*nativePortals_[i];p.sourceMode=true;auto& s=p.source;auto& c=p.entity;auto& a=worldApplication_->entityAuthority_;
    pc=f.pc=0x23037c;if(!a.canEnroll(c,key(&c)))return false;
    const auto result=g::initializeOriginalWreckComponent50LA(c,g::OriginalWreckComponentKind50LA::entity141300,
        worldEntity_.sessionGeneration,key(&c),true,worldApplication_->objectCount423F60());
    p.counted=c.reference00!=0;if(result.status!=g::OriginalWreckComponentStatus50LA::complete){pc=f.pc=result.sourcePC;return false;}
    pc=f.pc=0x230398;s.vtable08=0x3d18f0;
    constexpr SourcePortal230450State::Words vf0{{0,0,0,0x3f800000}};
    for(std::size_t j=0;j<4;++j){pc=f.pc=0x1048e0;s.vertices30[j]=vf0;p.vertexW[j]=vf0[3];++s.constructorVertices;}
    pc=f.pc=0x2303bc;s.normal70=vf0;s.normalKnown=15;s.normalConstructor=true;
    for(std::size_t j=0;j<4;++j){pc=f.pc=0x1048e0;s.edges80[j]=vf0;s.edgeKnown[j]=15;++s.constructorEdges;}
    s.constructorReturned=true;pc=f.pc=0x2303ec;
    if(!a.enroll(c))return false;s.enrolled=true;return true;
}
bool Owner::attachSourcePortal141960(std::size_t i,Entity* parent,std::uint32_t& pc)noexcept {
    pc=0x141970;if(i>=10||!sourcePortalCohortAdmitted_||!nativePortals_[i]||!sourceContainer4BC_||
       *sourceContainer4BC_!=parent||!worldApplication_)return false;
    auto& p=*nativePortals_[i];if(!p.sourceMode||!p.source.loadReturned||!p.counted||!p.source.enrolled||p.attached)return false;
    if(worldApplication_->attachmentLock423F58())return false; //Unjoined lock-return branch; no invented append.
    pc=0x14198c;if(!parent)return false; //Actual first parent dereference after nonNULL child.
    const auto result=worldApplication_->entityAuthority_.attachChildren(*parent,p.entity,0,worldApplication_->attachmentLock423F58());
    pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
    p.attached=true;sourcePortalFrames_[i].pc=pc;return true;
}
bool Owner::linkSourcePortalWorld27F3B0(std::size_t i,std::size_t stage,std::uint32_t a,std::uint32_t b,std::uint32_t& pc)noexcept {
    pc=0x27f3b0;if(i>=10||stage>=4||a>=10||b>=10||a==b||!nativePortals_[i]||!sourcePortalCohortAdmitted_)return false;
    auto& p=*nativePortals_[i];auto& s=p.source;auto& r=sourcePortalWorldLinks_[i][stage];
    if(!p.sourceMode||!p.attached||!s.loadReturned||s.worldLinksProduced!=stage||r.attempted||
       !s.connectA||!s.connectB||*s.connectA!=a||*s.connectB!=b)return false;
    const bool collision=stage&1u;const auto from=stage<2?a:b,to=stage<2?b:a;
    const auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
    if(!slots)return false;r.from=slots[from];r.to=slots[to];
    //Native validation of the caller's already reached actual slot loads; no second guest read or retained node authority.
    pc=0x27f3d0;r.attempted=true;r.pending.reset(new(std::nothrow) NativePortalLink27F3B0);
    pc=0x27f3d8;if(!r.from)return false;r.sourceNext=r.from->portalLinks30.get();
    pc=0x27f3e4;if(!r.pending)return false;r.knownMask|=1; //Actual source next00, owned chain moves only at publication.
    pc=0x27f3e8;r.pending->neighbor04=r.to?&r.to->entity:nullptr;r.knownMask|=2;
    pc=0x27f3ec;r.pending->portal08=&p.entity;r.knownMask|=4;
    pc=0x27f3f0;r.pending->field0C=0;r.knownMask|=8;
    pc=0x27f3f4;r.pending->field10=0x7f7fffff;r.knownMask|=16;
    pc=0x27f3f8;r.pending->next00=std::move(r.from->portalLinks30);r.published=r.pending.get();
    r.from->portalLinks30=std::move(r.pending);++s.worldLinksProduced;
    pc=sourcePortalFrames_[i].pc=0x27f410;return true;
}
bool Owner::sourcePortalWorldLinksCurrent(const SearchNode& node,bool collision,std::size_t zone)const noexcept {
    if(zone>=10)return false;
    const auto* link=node.portalLinks30.get();
    if(!sourcePortalCohortAdmitted_)return !link;
    for(std::size_t i=10;i>0;--i)for(std::size_t stage=4;stage>0;--stage){const auto& r=sourcePortalWorldLinks_[i-1][stage-1];
        if(r.withdrawn||!r.published||r.from!=&node)continue;
        if(!nativePortals_[i-1])return false;
        const auto& source=nativePortals_[i-1]->source;
        if(!source.connectA||!source.connectB||*source.connectA>=10||*source.connectB>=10)return false;
        const auto from=(stage-1)<2?*source.connectA:*source.connectB;
        const auto to=(stage-1)<2?*source.connectB:*source.connectA;
        const auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(!slots||from!=zone||slots[from]!=&node||r.from!=slots[from]||r.to!=slots[to])return false;
        if(bool((stage-1)&1u)!=collision||!nativePortals_[i-1]->sourceMode||
           r.knownMask!=31||r.pending||!r.to||!link||link!=r.published||
           link->neighbor04!=&r.to->entity||link->portal08!=&nativePortals_[i-1]->entity||
           link->field0C!=r.distance0C.value_or(0)||link->field10!=r.distance10.value_or(0x7f7fffff)||link->next00.get()!=r.sourceNext)return false;
        link=link->next00.get();
    }
    return !link;
}
bool Owner::preflightSourcePortalContainerNative(const Entity& parent,bool counted,bool enrolled)const noexcept {
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& a=worldApplication_->entityAuthority_;
    if(counted){if(parent.reference00!=1||parent.kind!=g::OriginalWreckComponentKind50LA::entity141300||
       parent.sessionGeneration!=worldEntity_.sessionGeneration||parent.objectKey!=key(&parent)||parent.parent1C||parent.previous10||parent.next14||
       parent.mask20||parent.nativeFlags29||parent.category28!=0xff||(parent.flags29&15u)!=3u)return false;}
    else if(parent.reference00||parent.firstChild18)return false;
    if(enrolled){if(!counted||!a.canRetire(parent))return false;}
    else for(std::size_t j=0;j<a.count_;++j)if(a.entries_[j].node==&parent)return false;
    const Entity* child=parent.firstChild18;const Entity* previous=nullptr;
    for(const auto& owned:nativePortals_)if(owned&&owned->sourceMode&&owned->attached){const auto& c=owned->entity;
        if(child!=&c||c.parent1C!=&parent||c.previous10!=previous||c.mask20||c.descendants24||c.firstChild18)return false;
        previous=&c;child=c.next14;}
    std::uint32_t descendants=0;
    return preflightSourceMethodNodeForestNative(parent,child,previous,descendants)&&!child&&parent.descendants24==descendants;
}
bool Owner::preflightSourcePortalOwnersNative(const Entity* parent)const noexcept {
    if(!sourcePortalCohortAdmitted_){for(const auto& f:sourcePortalFrames_)if(f.attempted||f.loadAttempted||f.withdrawn)return false;return true;}
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||nativePortalNext_)return false;
    const auto& authority=worldApplication_->entityAuthority_;
    for(std::size_t i=0;i<10;++i){const auto& f=sourcePortalFrames_[i];const auto& owned=nativePortals_[i];
        if(f.withdrawn){if(owned)return false;continue;}
        if(!owned){if(f.parameterLoads>1)return false;continue;}
        const auto& p=*owned;const auto& s=p.source;const auto& c=p.entity;
        if(!f.attempted||!p.sourceMode||p.geometry||p.authored||p.viewport434940||p.loaded||p.lightLinksComplete||p.worldLinksPublished||
           s.constructorVertices>4||s.constructorEdges>4||s.copiedVertices>4||s.normalKnown>15||s.worldLinksProduced>4)return false;
        if(p.counted){if(c.reference00!=1||c.kind!=g::OriginalWreckComponentKind50LA::entity141300||c.objectKey!=key(&c)||
           c.sessionGeneration!=worldEntity_.sessionGeneration||c.category28!=0xff||(c.flags29&15u)!=3u||c.nativeFlags29||
           c.firstChild18||c.mask20||c.descendants24)return false;}
        else if(c.reference00||p.attached||s.constructorReturned)return false;
        if(s.enrolled){if(!p.counted||!authority.canRetire(c))return false;}
        else for(std::size_t j=0;j<authority.count_;++j)if(authority.entries_[j].node==&c)return false;
        if(p.attached){if(!parent||c.parent1C!=parent||!s.loadReturned)return false;}
        else if(c.parent1C||c.previous10||c.next14)return false;
        if(s.constructorReturned&&(!s.vtable08||*s.vtable08!=0x3d18f0||s.constructorVertices!=4||s.constructorEdges!=4||!s.normalConstructor))return false;
        if(s.normalCrossStored&&s.normalKnown!=7)return false;
        for(std::size_t j=0;j<4;++j){if(s.edgeKnown[j]>15||(s.edgeCrossStored[j]&&s.edgeKnown[j]!=7)||
           (s.edgeNormalized[j]&&!s.edgeCrossStored[j]))return false;}
        if(s.normalNormalized&&!s.normalCrossStored)return false;
        if(s.diagonalReturned!=bool(p.maximumDiagonalC0))return false;
        if(s.loadReturned&&(s.copiedVertices!=4||!s.quadReturned||!s.diagonalReturned||!s.normalNormalized||
           !s.edgeNormalized[0]||!s.edgeNormalized[1]||!s.edgeNormalized[2]||!s.edgeNormalized[3]))return false;
        for(std::size_t stage=0;stage<4;++stage){const auto& r=sourcePortalWorldLinks_[i][stage];
            if(!r.fogDistanceHistoryCurrentNative())return false;
            if(r.withdrawn){if(r.pending)return false;continue;}
            if(r.knownMask&~31u)return false;
            if((r.distance0C||r.distance10)&&(!r.published||r.knownMask!=31))return false;
            if(r.distance0C&&!r.distance10)return false;
            if(r.attempted){
                if(!s.connectA||!s.connectB||*s.connectA>=10||*s.connectB>=10)return false;
                const auto from=stage<2?*s.connectA:*s.connectB,to=stage<2?*s.connectB:*s.connectA;
                const auto* slots=(stage&1u)?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
                if(!slots||r.from!=slots[from]||r.to!=slots[to])return false;
            }
            if(r.published){if(!r.attempted||r.pending||r.knownMask!=31||stage>=s.worldLinksProduced||!r.from||!r.to||!p.attached)return false;}
            else if(stage<s.worldLinksProduced||r.knownMask||(r.pending&&!r.attempted))return false;
        }
    }
    bool allWithdrawn=true;for(const auto& f:sourcePortalFrames_)allWithdrawn=allWithdrawn&&f.withdrawn;
    if(allWithdrawn)return true; //No stale node/array dereference after parent destroys topology.
    for(std::size_t z=0;z<10;++z)for(bool collision:{false,true}){const auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(slots&&slots[z]&&!sourcePortalWorldLinksCurrent(*slots[z],collision,z))return false;}
    return true;
}
bool Owner::sourcePortalOwnersCounted(std::size_t& count)const noexcept {
    count=0;if(!sourcePortalCohortAdmitted_)return true;
    for(const auto& p:nativePortals_)if(p&&p->sourceMode&&p->counted)++count;
    return true; //Parent calls nonrecursive exact local preflight before checked floor addition.
}
bool Owner::withdrawSourcePortalWorldLinksNative(std::uint32_t& pc)noexcept {
    pc=0x27f180;
    if(sourceCollisionPortalBorrowedNative())return false;
    if(sourcePhysicsModelPortalBorrowedNative())return false;
    for(const auto& f:sourceModelObservers_)if(f&&!f->withdrawn)return false;
    for(const auto& f:sourceTypedLights_)if(f&&f->flareObserverEntered&&!f->flareObserverWithdrawn)return false;
    //Caller has preflighted the whole graph/count floor and withdrawn manager raw borrows.
    for(std::size_t z=0;z<10;++z)for(bool collision:{false,true}){auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(slots&&slots[z]&&!sourcePortalWorldLinksCurrent(*slots[z],collision,z))return false;}
    for(auto& rows:sourcePortalWorldLinks_)for(auto& r:rows)r.withdrawn=true; //Native access withdrawal BEFORE deletion.
    for(std::size_t z=0;z<10;++z)for(bool collision:{false,true}){auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(slots&&slots[z])slots[z]->portalLinks30.reset();}
    for(auto& rows:sourcePortalWorldLinks_)for(auto& r:rows)r.pending.reset();
    return true; //Historical heads/counts retained; not a guest NULL30 store.
}
bool Owner::retireSourcePortalsNative(Entity* parent,std::uint32_t& pc)noexcept {
    pc=0x2303f0;if(!preflightSourcePortalOwnersNative(parent))return false;
    if(sourceCollisionPortalBorrowedNative())return false;
    if(sourcePhysicsModelPortalBorrowedNative())return false;
    for(const auto& f:sourceTypedLights_)if(f&&f->flareObserverEntered&&!f->flareObserverWithdrawn)return false;
    for(const auto& f:sourceMethodNodeAttachments_)if(f.attempted&&!f.withdrawn)return false;
    for(const auto& rows:sourcePortalWorldLinks_)for(const auto& r:rows)if(r.attempted&&!r.withdrawn)return false;
    for(std::size_t i=10;i>0;--i){auto& owned=nativePortals_[i-1];auto& f=sourcePortalFrames_[i-1];if(f.withdrawn)continue;
        if(owned){auto& p=*owned;auto& c=p.entity;
            if(p.attached&&(!parent||c.next14))return false;
            //The only fallible retirement precedes attachment mutation. A refusal
            //therefore retains the exact caller's attached mask for retry.
            if(p.source.enrolled){if(!worldApplication_->entityAuthority_.retire(c))return false;p.source.enrolled=false;}
            if(p.attached){
                if(c.previous10)c.previous10->next14=nullptr;else parent->firstChild18=nullptr;
                c.parent1C=nullptr;c.previous10=nullptr;p.attached=false;}
            if(p.counted){--c.reference00;--worldApplication_->objectCount423F60();p.counted=false;}
            f.withdrawn=true;owned.reset();
        }else f.withdrawn=true;
    }
    return true; //Native exact ownership release, no source constructor/parameter replay.
}
}
