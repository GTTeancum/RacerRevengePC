#pragma once
#include <new>
namespace ra_pod_scene50LA {
bool Owner::allocateSourceListMethod27E260(std::size_t kind,std::size_t zone,std::uint32_t& pc)noexcept {
    constexpr std::uint32_t allocation[]{0x2731c4,0x2733ec,0x273654,0x27387c};
    constexpr std::uint32_t returned[]{0x2731e0,0x273408,0x273670,0x273898};
    if(kind>=4||zone>=10||!worldApplication_||!worldApplication_->application10())return false;
    auto& r=sourceListMethods_[kind][zone];pc=allocation[kind];
    if(r.attempted||r.owner||r.withdrawn)return false;
    r.attempted=true;r.pc=pc;r.owner.reset(new(std::nothrow) NativeListMethodCustody);
    if(!r.owner){r.returned=true;pc=r.pc=returned[kind];return true;}
    auto& c=*r.owner;auto& s=c.source_;c.sourceMode_=true;c.world_=this;c.application_=worldApplication_.get();c.ordinal_=kind*10+zone;
    const auto at=[&](std::uint32_t p){pc=r.pc=p;};
    at(0x1048f8);s.vtable08=0x3cc980;
    at(0x104900);c.reference00_=1;s.referenceKnown=true;s.constructorReference=true;
    at(0x27e9f0);s.vtable08=0x3d2d80;
    at(0x27ed10);s.pointer1C=nullptr;
    at(0x27ed14);s.count18=0;
    at(0x27ed18);s.growth20=0;
    at(0x27ed1c);s.capacity24=0;
    at(0x27ed3c);s.ownedBitKnown=true;s.ownedBit=false; //Only bit0 known; other flag bits remain unknown.
    at(0x27ea0c);c.usage10_=0;s.usageKnown=true;
    //Exact fresh capacity0/request30 proof before the shared source guard entry.
    if(!s.capacity24||*s.capacity24!=0||!g::selected_array_guards27ED40::runFresh30(s.guards,pc)){r.pc=pc;return false;}
    at(0x27ee38);s.selected1C.reset(new(std::nothrow) const void*[30]);s.selectedAllocationReturned=true;
    at(0x27ee40);s.pointer1C=s.selected1C.get();
    at(0x27ee44);s.growth20=30;
    at(0x27ee48);s.capacity24=30;
    at(0x27ee60);s.ownedBit=true;
    //The original constructor ignores selected allocation v0, including NULL.
    at(0x27ea28);auto tag=worldApplication_->types383408_.findOrAppend2A31E0("SoultreeObject");
    if(!tag.okay()){at(tag.sourcePC);return false;}
    at(0x27ea3c);s.type2C=tag.type;
    at(0x27ea44);tag=worldApplication_->types383408_.findOrAppend2A31E0("SearchableGameObject");
    if(!tag.okay()){at(tag.sourcePC);return false;}
    at(0x27ea50);s.type2D=tag.type;
    at(0x27e284);s.vtable08=0x3d2d30;
    at(0x27e288);s.perBlock30=1024;
    at(0x27e28c);s.pointer3C=nullptr;
    at(0x27e290);s.blockCount34=0;
    at(0x27e294);s.field40=0;
    at(0x27e2a0);s.lastUsed38=*s.perBlock30;
    s.constructorReturned=true;r.returned=true;at(returned[kind]);return true;
}
bool Owner::sourceListMethodReturned(std::size_t kind,std::size_t zone,NativeListMethodCustody*& output)const noexcept {
    if(kind>=4||zone>=10)return false;const auto& r=sourceListMethods_[kind][zone];
    if(!r.attempted||!r.returned||r.withdrawn||(r.owner&&!r.owner->source_.constructorReturned))return false;
    output=r.owner.get();return true;
}
bool Owner::sourceListMethodArrayCurrent(std::size_t kind,std::size_t zone,const NativeListMethodCustody* actual)const noexcept {
    NativeListMethodCustody* returned=nullptr;return sourceListMethodReturned(kind,zone,returned)&&actual==returned;
}
bool Owner::aliasSourceListMethod27F420(std::size_t kind,std::size_t zone,SearchNode* node,std::uint32_t& pc)noexcept {
    if(kind>=4||zone>=10)return false;const bool dynamic=kind>=2,collision=kind&1u;
    auto& r=sourceListMethods_[kind][zone];pc=dynamic?0x27f4a4:0x27f434;
    if(!r.returned||!r.arrayPublished||r.withdrawn||r.aliasAttempted)return false;
    const auto* nodes=collision?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
    const auto* methods=kind==0?nativeMethods4C8_.get():kind==1?nativeMethods4CC_.get():kind==2?nativeMethods4D0_.get():nativeMethods4D4_.get();
    if(!nodes||node!=nodes[zone]||!methods||methods[zone]!=r.owner.get())return false;
    r.aliasAttempted=true;r.aliasNode=node;r.pc=pc;
    if(!node)return false; //Actual old34/38 load precedes method usage store.
    auto& alias=dynamic?node->nativeList38:node->nativeList34;
    if(alias){pc=r.pc=dynamic?0x27f4b0:0x27f440;return false;} //No unjoined old-method release.
    pc=r.pc=dynamic?0x27f4c4:0x27f454;if(!r.owner)return false;
    auto& c=*r.owner;if(!c.sourceMode_||!c.source_.constructorReturned||c.reference00_!=1)return false;
    c.usage10_=dynamic?2u:1u;
    pc=r.pc=dynamic?0x27f4c8:0x27f458;alias=&c;r.aliasPublished=true;
    pc=r.pc=0x10496c;++c.reference00_;r.aliasRetained=true;
    pc=r.pc=dynamic?0x27f4f0:0x27f480;return true;
}
bool Owner::sourceListMethodAliasesCurrent(const SearchNode& node,bool collision,std::size_t zone)const noexcept {
    if(zone>=10)return false;
    for(std::size_t dynamic=0;dynamic<2;++dynamic){const auto kind=dynamic*2+std::size_t(collision);
        const auto& r=sourceListMethods_[kind][zone];const auto* alias=dynamic?node.nativeList38:node.nativeList34;
        if(r.withdrawn){if(alias)return false;continue;}
        if(r.aliasPublished){if(!r.owner||r.aliasNode!=&node||alias!=r.owner.get())return false;}
        else if(alias)return false;
    }return true;
}
bool Owner::preflightSourceListMethodsNative()const noexcept {
    for(std::size_t kind=0;kind<4;++kind)for(std::size_t zone=0;zone<10;++zone){const auto& r=sourceListMethods_[kind][zone];
        if(r.withdrawn){if(r.owner)return false;continue;} //Never dereference historical array/node after withdrawal.
        if(!r.attempted){if(r.owner||r.returned||r.arrayPublished||r.aliasAttempted||r.aliasPublished||r.aliasRetained||r.aliasNode)return false;continue;}
        if(r.arrayPublished&&!r.returned)return false;
        if(r.aliasPublished&&(!r.aliasAttempted||!r.aliasNode||!r.owner))return false;
        if(r.aliasRetained&&!r.aliasPublished)return false;
        if(r.aliasAttempted){const auto* nodes=(kind&1u)?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get();
            if(!nodes||r.aliasNode!=nodes[zone])return false;}
        if(r.arrayPublished){const auto* array=kind==0?nativeMethods4C8_.get():kind==1?nativeMethods4CC_.get():kind==2?nativeMethods4D0_.get():nativeMethods4D4_.get();
            if(!array||array[zone]!=r.owner.get())return false;}
        if(!r.owner){if(!r.returned||r.aliasPublished||r.aliasRetained)return false;continue;}
        const auto& c=*r.owner;const auto& s=c.source_;
        if(!worldApplication_||!c.sourceMode_||c.world_!=this||c.application_!=worldApplication_.get()||c.ordinal_!=kind*10+zone||
           c.method_||c.registry_||c.selectionOwner_||c.arrayReference_||c.nodeReference_||c.begun_||c.nodeAliasWasAcquired_)return false;
        if(c.reference00_!=std::uint64_t(s.constructorReference)+std::uint64_t(r.aliasRetained)||
           (!s.referenceKnown&&c.reference00_)||s.constructorReference!=s.referenceKnown)return false;
        if(s.usageKnown&&c.usage10_!=(r.aliasPublished?(kind>=2?2u:1u):0u))return false;
        if(!g::selected_array_guards27ED40::preflight(s.guards))return false;
        if(s.selectedAllocationReturned){if(!s.guards.returned||!s.pointer1C||*s.pointer1C!=s.selected1C.get()||
            !s.growth20||*s.growth20!=30||!s.capacity24||*s.capacity24!=30||!s.ownedBitKnown||!s.ownedBit)return false;}
        else if(s.selected1C||s.ownedBit)return false;
        if(s.pointer1C&&*s.pointer1C!=s.selected1C.get())return false;
        if(s.count18&&*s.count18)return false;
        if(s.type2D&&!s.type2C)return false;
        if(!s.constructorReturned&&!c.sourceCollisionRegistrationBorrowers_.empty())return false;
        if(s.constructorReturned){if(!r.returned||!s.vtable08||*s.vtable08!=0x3d2d30||!s.type2C||!s.type2D||
            !s.perBlock30||*s.perBlock30!=1024||!s.pointer3C||!s.blockCount34||
            !s.field40||!s.lastUsed38||!s.selectedAllocationReturned||!sourceListEntriesCurrent(c)||
            !c.sourceCollisionEntriesCurrentNative())return false;}
        if(r.aliasNode&&!sourceListMethodAliasesCurrent(*r.aliasNode,kind&1u,zone))return false;
    }return true;
}
bool Owner::retireSourceListMethodsNative(std::uint32_t& pc)noexcept {
    pc=0x27e2c0;if(!preflightSourceListMethodsNative())return false;
    for(const auto& f:sourceModelObservers_)if(f&&!f->withdrawn)return false;
    for(const auto& frame:sourceTypedLights_)if(frame&&frame->flareObserverEntered&&!frame->flareObserverWithdrawn)return false;
    //No source List backing may die while a selected/member borrower exists.
    for(const auto& kinds:sourceListMethods_)for(const auto& row:kinds)if(row.owner){
        if(!row.owner->sourceCollisionRegistrationBorrowers_.empty())return false;
        const auto& entries=row.owner->sourceEntries_;if(entries.pending)return false;
        for(const auto* member:entries.members)if(member)return false;
    }
    //Parent has preflighted whole graph/count floor and will withdraw array access
    //before disposal. Here every acquired reference belongs to these exact two lanes.
    for(std::size_t zone=10;zone>0;--zone)for(std::size_t kind=4;kind>0;--kind){auto& r=sourceListMethods_[kind-1][zone-1];
        if(r.withdrawn)continue;
        if(r.owner){auto& c=*r.owner;
            if(r.aliasPublished){auto& alias=(kind-1)>=2?r.aliasNode->nativeList38:r.aliasNode->nativeList34;
                alias=nullptr; //Native usable alias withdrawal, not a source27F store.
                if(r.aliasRetained)--c.reference00_;
            }
            if(c.source_.constructorReference)--c.reference00_;
            c.source_.selected1C.reset();
        }
        r.withdrawn=true;r.owner.reset(); //Retained published/alias/ref history is not replayable.
    }return true;
}
}
