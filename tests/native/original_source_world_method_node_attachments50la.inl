#pragma once
namespace ra_pod_scene50LA {
bool Owner::sourceMethodNodeAttachmentCurrent(const SearchNode& node,bool collision,std::size_t index)const noexcept {
    if(index>=10)return false;const auto& f=sourceMethodNodeAttachments_[index*2+std::size_t(collision)];
    const auto& c=node.entity;
    if(!f.attempted){if(f.identity||f.returned||f.withdrawn||node.attached)return false;}
    else if(f.identity!=&node||(!f.returned&&node.attached)||
        (f.withdrawn&&(!sourceMethodNodesDetaching_||node.attached)))return false;
    if(c.firstChild18||c.descendants24)return false;
    const auto expected=f.returned&&!collision?0x400u:0u;
    if(c.mask20!=expected)return false;
    if(f.returned&&!f.withdrawn){
        if(!node.attached||!sourceContainer4BC_||!(*sourceContainer4BC_)||c.parent1C!=*sourceContainer4BC_)return false;
    }else if(node.attached||c.parent1C||c.previous10||c.next14)return false;
    return true;
}
bool Owner::preflightSourceMethodNodeForestNative(const Entity& parent,const Entity*& child,
    const Entity*& previous,std::uint32_t& descendants)const noexcept {
    descendants=0;if(sourceMethodNodeNext_>20)return false;
    bool detached=false;
    for(std::size_t k=0;k<20;++k){const auto& f=sourceMethodNodeAttachments_[k];
        if(!f.attempted){if(f.identity||f.returned||f.withdrawn||k<sourceMethodNodeNext_)return false;continue;}
        if(k>sourceMethodNodeNext_||!f.identity||f.returned!=(k<sourceMethodNodeNext_)||
           (f.withdrawn&&!sourceMethodNodesDetaching_))return false;
        if(f.withdrawn){detached=true;continue;} //No historical pointer dereference.
        if(detached)return false; //Native withdrawal is strictly reverse-tail suffix.
        const bool collision=k&1u;const auto index=k/2;
        const auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(!slots||slots[index]!=f.identity)return false;const auto& node=*slots[index];const auto& c=node.entity;
        if(!sourceMethodNodeAttachmentCurrent(node,collision,index))return false;
        if(!f.returned)continue;
        if(child!=&c||c.parent1C!=&parent||c.previous10!=previous)return false;
        descendants|=c.mask20|c.descendants24;previous=&c;child=c.next14;
    }
    return preflightSourceVisibilityEnvironmentForestNative(parent,child,previous);
}
bool Owner::preflightSourceMethodNodeAttachmentsNative(const Entity* parent)const noexcept {
    bool live=false;for(const auto& f:sourceMethodNodeAttachments_)if(f.attempted&&!f.withdrawn)live=true;
    if(!parent){
        if(live||sourceMethodNodeNext_>20)return false;
        for(std::size_t k=0;k<20;++k){const auto& f=sourceMethodNodeAttachments_[k];
            if(f.attempted){if(!sourceMethodNodesDetaching_||!f.withdrawn||!f.identity||
                k>sourceMethodNodeNext_||f.returned!=(k<sourceMethodNodeNext_))return false;}
            else if(f.identity||f.returned||f.withdrawn||k<sourceMethodNodeNext_)return false;}
        return true;
    }
    if(!sourceContainer4BC_||*sourceContainer4BC_!=parent)return false;
    return preflightSourcePortalContainerNative(*parent,true,true);
}
bool Owner::attachSourceMethodNode141960(SourceSearchNode27F120& frame,bool collision,
    std::size_t index,Entity* parent,std::uint32_t& pc)noexcept {
    pc=collision?0x272eb0:0x272e94;const auto k=index*2+std::size_t(collision);
    if(index>=10||k!=sourceMethodNodeNext_||sourceMethodNodesDetaching_||
       sourceMethodNodeAttachments_[k].attempted||!sourceTopologyMode_||sourceTopologyWithdrawn_||
       !worldApplication_||!sourceContainer4BC_||*sourceContainer4BC_!=parent||!parent||
       !frame.owner||!frame.counted||!frame.enrolled||!frame.published||frame.withdrawn||!frame.ordinalKnown||
       !preflightSourceSearchNodeNative(frame,index,collision)||!preflightSourceMethodNodeAttachmentsNative(parent))return false;
    for(const auto& p:nativePortals_)if(!p||!p->sourceMode||!p->attached||!p->source.loadReturned||p->source.worldLinksProduced!=4)return false;
    auto* slots=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
    if(!slots||slots[index]!=frame.owner.get())return false;
    pc=0x141970;if(worldApplication_->attachmentLock423F58())return false;
    auto& node=*frame.owner;auto& f=sourceMethodNodeAttachments_[k];f.attempted=true;f.identity=&node;
    const auto result=worldApplication_->entityAuthority_.attachChildren(*parent,node.entity,collision?0u:0x400u,
        worldApplication_->attachmentLock423F58());
    pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
    node.attached=true;f.returned=true;++sourceMethodNodeNext_;return true;
}
bool Owner::detachSourceMethodNodesNative(Entity* parent,std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceMethodNodeAttachmentsNative(parent))return false;
    //Whole forest and all alias withdrawals are checked before any mutation.
    for(std::size_t k=0;k<20;++k){const auto& f=sourceMethodNodeAttachments_[k];if(!f.attempted||f.withdrawn)continue;
        const auto* slots=(k&1u)?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
        if(!slots||slots[k/2]!=f.identity||!worldApplication_||
           f.identity->nativeList34||f.identity->nativeList38||
           !worldApplication_->entityAuthority_.canRetire(f.identity->entity))return false;
    }
    sourceMethodNodesDetaching_=true;
    for(std::size_t k=20;k>0;--k){auto& f=sourceMethodNodeAttachments_[k-1];if(!f.attempted||f.withdrawn)continue;
        auto& node=*f.identity;auto& c=node.entity;
        if(f.returned){
            if(!parent||!node.attached||c.parent1C!=parent||c.next14)return false;
            if(c.previous10)c.previous10->next14=nullptr;else parent->firstChild18=nullptr;
            c.parent1C=nullptr;c.previous10=nullptr;node.attached=false;
            //mask20 is source history: visibility remains400 after native detach.
        }
        f.withdrawn=true;
        if(parent){std::uint32_t descendants=0;
            for(std::size_t j=0;j<k-1;++j)if(sourceMethodNodeAttachments_[j].returned&&!sourceMethodNodeAttachments_[j].withdrawn)
                descendants|=(j&1u)?0u:0x400u;
            parent->descendants24=descendants; //Native forest repair, not replay of guest stores.
        }
    }
    return true;
}
}
