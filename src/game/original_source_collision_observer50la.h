#pragma once
#include "original_garage_observer_shared50kg.h"
#include "original_native_list_method_custody50la.h"
namespace rr::game {
//Exact280890 storage and bounded2809C0 primary prefix. One instance represents either the
//120070 stack observer or a separately nullable allocated heap observer.
//The record's native size is not the guest30 allocation size. Untouched18/1C
//remain unknown; zero-initialized backing is never source knownness.
class OriginalSourceCollisionObserver50LA final {
    friend class OriginalWorldTriggerInstance50LA; //Distinct trigger120070 mode1 occurrence.
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalSourcePhysicsModel50LA;
    friend class OriginalWorldCollisionObjects50LA; //Actual mode13 temporary120070 occurrence.
    using Shared=OriginalGarageObserverShared50KG;
    struct Record {
        const void* next00=nullptr;
        const void* zone04=nullptr;
        const void* method08=nullptr;
        std::uint32_t field0C=0,field10=0;
        std::optional<std::uint32_t> slot14; //Known numeric source bits, if any; constructor explicitly writes zero.
        std::array<std::optional<std::uint32_t>,2> unknown18To1C{};
        std::array<std::uint32_t,4> center20{};
    } record_;
    std::uint32_t* fogSlot14_=nullptr; //Typed Fog alternative; never a numeric ordinal or guest-pointer cast.
    bool fogSlot14AliasWithdrawn_=false;
    Shared* shared_=nullptr;
    const void* group_=nullptr;
    bool entered_=false,lease_=false,returned_=false,nativeWithdrawn_=false;
    bool vectorKnown_=false;
    std::uint8_t scalarMask_=0;
    std::uint32_t ctorPC_=0;
    bool destroyEntered_=false,countDecremented_=false,destroyReturned_=false,sourceDeleted_=false;
    std::int32_t destructionArgument_=0;
    std::uint32_t destroyPC_=0,countBeforeDestroy_=0;
    const void* identity()const noexcept {return &record_;}
    std::uint32_t pc()const noexcept {return destroyEntered_?destroyPC_:ctorPC_;}
    bool returned()const noexcept {return returned_;}
    struct SourcePrimary2809C0 {
        const void* occurrence=nullptr;
        const void* zone=nullptr; //Actual source zone identity, not a COL/Entity alias.
        const OriginalNativeListMethodCustody50LA* method=nullptr;
        std::array<std::uint32_t,4> center{};
        std::uint32_t slot=0,pc=0;
        std::optional<bool> fogSelectedAddressNonNull; //Native typed-address witness only; not source slot14 or legacy ordinal.
        bool entered=false,methodRead=false,slotRead=false,nativeWithdrawn=false;
        std::uint8_t stores=0;
        Shared::SourceAssertionHistory emptyAssertion,methodAssertion,slotAssertion;
    } primary_;
    using SecondaryLink=Shared::Link105720;
    using FogPoolAllocator=Shared::Allocator105620; //Private typed view; same actual443138 allocation, never a new pool.
    using FogPoolChunk=Shared::Chunk105870;
    struct FogPoolAllocation {
        const void* primaryOccurrence=nullptr;const void* secondaryOccurrence=nullptr;
        FogPoolAllocator* pool=nullptr;SecondaryLink* record=nullptr;
        bool entered=false,fromFree=false,nativeWithdrawn=false,accountingStored=false;
        bool slot14Stored=false,slot14AliasWithdrawn=false;
        std::uint32_t slot14NativeOrdinal=0; //Authenticated List cell identity, NEVER the guest slot14 address/value.
        bool slot14ReservationCommitted=false; //Native successful handoff only; source payload history is unchanged.
        const OriginalNativeListMethodCustody50LA* slot14Method=nullptr;const void* slot14Member=nullptr;
        std::uint8_t payloadMask=0;bool oldHeadRead=false;
        const OriginalWreckComponent50LA* child04=nullptr;const OriginalWreckComponent50LA* portal0C=nullptr;
        std::optional<std::uint32_t> distance10;
    } fogPoolAllocation_; //One actual consumed record and its distinct typed slot14 store; never a generic secondary entry.
    bool fogPoolSlot14BackingCurrentNative(const void* secondary,const OriginalNativeListMethodCustody50LA* method,
        const void* member,const std::uint32_t* entry,std::uint32_t nativeOrdinal,bool committed)const noexcept {
        if(!secondary||secondary==primary_.occurrence||!method||!primary_.method||method==primary_.method||!member||!entry||
           !nativeOrdinal||nativeOrdinal>1024u)return false;
        const auto& c=*method;const auto& e=c.sourceEntries_;const auto& s=c.source_;
        if(!c.sourceMode_||c.world_!=primary_.method->world_||c.application_!=primary_.method->application_||
           c.reference00_!=2u||c.ordinal_<10u||c.ordinal_>=20u||!s.constructorReturned||s.vtable08!=0x3d2d30u||
           !s.usageKnown||c.usage10_!=1u||!c.sourceCollisionEntriesCurrentNative()||(!committed&&e.terminal)||!e.ready||!e.words||
           !s.pointer3C||*s.pointer3C!=e.table.get()||!e.table||e.table[0]!=e.words.get())return false;
        if(committed){
            if(e.pending&&(e.pending==secondary||e.pendingIdentity==member||e.pendingSlot==nativeOrdinal))return false;
        }else if(e.pending!=secondary||e.pendingIdentity!=member||!e.pendingConsumed||e.pendingSlot!=nativeOrdinal)return false;
        const auto index=nativeOrdinal-1u; //Retained cell identity, not an ordinal reconstructed from a host/guest pointer.
        if(entry!=e.words.get()+index||e.members[index]!=member||e.borrowers[index]!=secondary||e.wordKnown[index])return false;
        unsigned pins=0;for(const auto* key:c.sourceCollisionRegistrationBorrowers_)if(key==secondary)++pins;
        return pins==1u;
    }
    bool fogPoolHeadCurrentNative()const noexcept {
        const auto& f=fogPoolAllocation_;
        const void* expected=f.payloadMask==63u&&!f.slot14AliasWithdrawn&&!f.nativeWithdrawn?f.record:nullptr;
        return record_.next00==expected; //Exactly one reached Fog head, or the genuine280B9C NULL baseline/native unlink.
    }
    bool fogPoolAllocationHistoryNative()const noexcept {
        const auto& f=fogPoolAllocation_;
        if((f.slot14Stored&&!f.accountingStored)||(f.slot14AliasWithdrawn&&!f.slot14Stored)||
           f.slot14Stored!=bool(f.slot14NativeOrdinal)||f.slot14NativeOrdinal>1024u||
           (f.slot14ReservationCommitted&&(!f.slot14Stored||f.payloadMask!=63u))||
           (f.payloadMask!=0u&&f.payloadMask!=1u&&f.payloadMask!=3u&&f.payloadMask!=7u&&
            f.payloadMask!=15u&&f.payloadMask!=31u&&f.payloadMask!=63u)||
           (f.payloadMask&&!f.slot14Stored)||(f.oldHeadRead&&f.payloadMask<15u)||
           ((f.payloadMask&16u)&&!f.oldHeadRead)||bool(f.distance10)!=bool(f.payloadMask&8u)||
           bool(f.child04)!=bool((f.payloadMask&1u)&&!f.slot14AliasWithdrawn)||
           bool(f.portal0C)!=bool((f.payloadMask&4u)&&!f.slot14AliasWithdrawn))return false;
        if(!f.entered)return !f.primaryOccurrence&&!f.secondaryOccurrence&&!f.pool&&!f.record&&!f.fromFree&&!f.nativeWithdrawn&&!f.accountingStored&&
            !f.slot14Stored&&!f.slot14AliasWithdrawn&&!f.slot14NativeOrdinal&&!f.slot14ReservationCommitted&&
            !f.slot14Method&&!f.slot14Member&&!f.payloadMask&&!f.oldHeadRead&&fogPoolHeadCurrentNative();
        if(!f.primaryOccurrence||f.primaryOccurrence!=primary_.occurrence||!f.secondaryOccurrence||f.secondaryOccurrence==f.primaryOccurrence||
           primary_.fogSelectedAddressNonNull!=true||primary_.stores!=31u||secondary_.entered||detached_.entered)return false;
        if(f.nativeWithdrawn)return !f.pool&&!f.record&&!f.slot14Method&&!f.slot14Member&&
            f.slot14AliasWithdrawn==f.slot14Stored&&fogPoolHeadCurrentNative();
        if(!shared_||!lease_||!returned_||nativeWithdrawn_||destroyEntered_||primary_.nativeWithdrawn||
           !shared_->sourceGroupCurrent(group_)||!shared_->ownsReturnedSourceObserver(identity())||
           f.pool!=shared_->allocator443138_.get()||f.pool!=shared_->sourceGroupRetainedAllocator_||
           !shared_->allocatedRecord(f.record))return false;
        if(f.record->nativeBorrower!=identity()||!fogPoolHeadCurrentNative())return false;
        if(((f.payloadMask&1u)&&f.record->child04!=f.child04)||
           ((f.payloadMask&2u)&&(f.record->method08||f.record->collisionMethod08!=f.slot14Method))||
           ((f.payloadMask&4u)&&f.record->portal0C!=f.portal0C)||
           ((f.payloadMask&8u)&&f.record->distance10!=*f.distance10)||
           ((f.payloadMask&16u)&&f.record->next00))return false;
        if(!f.slot14Stored||f.slot14AliasWithdrawn)return !f.record->fogSlot14&&!f.slot14Method&&!f.slot14Member;
        return fogPoolSlot14BackingCurrentNative(f.secondaryOccurrence,f.slot14Method,f.slot14Member,f.record->fogSlot14,
            f.slot14NativeOrdinal,f.slot14ReservationCommitted);
        //No numeric slot14 read: its old/default bits are not this Fog occurrence's stored source address.
    }
    bool fogPoolOwnerRecordsCurrentNative()const noexcept {
        if(!fogPoolAllocationHistoryNative())return false;
        const unsigned expected=fogPoolAllocation_.entered&&!fogPoolAllocation_.nativeWithdrawn?1u:0u;
        if(nativeWithdrawn_||!shared_)return !expected;
        unsigned actual=0;
        if(shared_->allocator443138_)for(const auto& chunk:shared_->allocator443138_->chunks)if(chunk)
            for(const auto& record:chunk->records)if(record.nativeBorrower==identity())++actual;
        return actual==expected;
    }
    bool fogPoolChunkCurrentNative(const void* occurrence,const FogPoolAllocator* pool,const FogPoolChunk* chunk)const noexcept {
        if(!chunk||!fogPoolAliasesCurrentNative(occurrence,pool,nullptr))return false;
        for(const auto& owned:pool->chunks)if(owned.get()==chunk)return true;
        return false; //Historical current18 may cease being current; its actual chunk remains owned.
    }
    bool admitFogPoolMutationNative(const void* occurrence,const void* secondary,FogPoolAllocator* pool,
        std::uint32_t oldUsed,bool fromFree,const SecondaryLink* freeAlias)const noexcept {
        if(!secondary||secondary==occurrence||fogPoolAllocation_.entered||!fogPoolAliasesCurrentNative(occurrence,pool,freeAlias)||
           !shared_->sourceLinkAccessCurrent(group_,identity())||pool->perBlock10!=169u||pool->itemSize24!=24u||
           pool->used20!=oldUsed||oldUsed>169u)return false;
        if(fromFree)return oldUsed==169u&&freeAlias&&pool->free2C==freeAlias&&!freeAlias->nativeBorrower;
        return oldUsed<169u&&!freeAlias&&pool->current18&&fogPoolChunkCurrentNative(occurrence,pool,pool->current18);
    }
    bool consumeFogPoolTail105754(const void* occurrence,const void* secondary,FogPoolAllocator* pool,
        FogPoolChunk* chunk,std::uint32_t oldUsed)noexcept {
        if(!admitFogPoolMutationNative(occurrence,secondary,pool,oldUsed,false,nullptr)||chunk!=pool->current18)return false;
        auto* record=&chunk->records[oldUsed];if(record->nativeBorrower)return false;
        //One non-failing commit: original used20 store and native custody of the newly consumed record.
        pool->used20=oldUsed+1u;record->nativeBorrower=identity();
        fogPoolAllocation_={occurrence,secondary,pool,record,true,false,false};return true;
    }
    bool consumeFogPoolFree105774(const void* occurrence,const void* secondary,FogPoolAllocator* pool,
        SecondaryLink* freeAlias,SecondaryLink* next,std::uint32_t oldUsed)noexcept {
        if(!admitFogPoolMutationNative(occurrence,secondary,pool,oldUsed,true,freeAlias)||freeAlias->next00!=next)return false;
        //One non-failing commit: original branch-delay free2C store and native custody. No payload initialization.
        pool->free2C=next;freeAlias->nativeBorrower=identity();
        fogPoolAllocation_={occurrence,secondary,pool,freeAlias,true,true,false};return true;
    }
    bool admitFogPoolAccountingNative(const void* occurrence,const void* secondary,const FogPoolAllocator* pool,const SecondaryLink* record)const noexcept {
        const auto& f=fogPoolAllocation_;
        return !f.accountingStored&&f.entered&&!f.nativeWithdrawn&&f.primaryOccurrence==occurrence&&f.secondaryOccurrence==secondary&&
            f.pool==pool&&f.record==record&&fogPoolAliasesCurrentNative(occurrence,pool,nullptr)&&
            shared_->sourceLinkAccessCurrent(group_,identity())&&!pool->allocatedBytes34;
    }
    bool storeFogUnknownAccounting105818(const void* occurrence,const void* secondary,FogPoolAllocator* pool,
        const SecondaryLink* record,std::uint32_t actualItemSize24)noexcept {
        if(!admitFogPoolAccountingNative(occurrence,secondary,pool,record)||actualItemSize24!=24u||pool->itemSize24!=actualItemSize24)return false;
        //Original SW of (unknown32 +24) modulo2^32: still unknown. Reset is the abstract store, not a fabricated zero.
        pool->allocatedBytes34.reset();fogPoolAllocation_.accountingStored=true;return true;
    }
    bool storeFogPoolSlot14280CD0(const void* occurrence,const void* secondary,FogPoolAllocator* pool,SecondaryLink* record,
        const OriginalNativeListMethodCustody50LA* method,const void* member,std::uint32_t* actualEntry)noexcept {
        auto& f=fogPoolAllocation_;
        const auto nativeOrdinal=method?method->sourceEntries_.pendingSlot:0u;
        if(!f.entered||f.nativeWithdrawn||!f.accountingStored||f.slot14Stored||f.slot14AliasWithdrawn||
           f.primaryOccurrence!=occurrence||f.secondaryOccurrence!=secondary||f.pool!=pool||f.record!=record||
           !fogPoolAliasesCurrentNative(occurrence,pool,nullptr)||!shared_->sourceLinkAccessCurrent(group_,identity())||
           !fogPoolSlot14BackingCurrentNative(secondary,method,member,actualEntry,nativeOrdinal,false))return false;
        //Non-failing original SW to this exact consumed record. Typed address, never a native ordinal or a guest DWORD cast.
        record->fogSlot14=actualEntry;f.slot14Method=method;f.slot14Member=member;
        f.slot14NativeOrdinal=nativeOrdinal;f.slot14Stored=true;return true;
    }
    bool publishFogPoolPayloadNative(std::uint32_t sourcePC,const void* occurrence,const void* secondary,SecondaryLink* record,
        const OriginalWreckComponent50LA* child,const OriginalNativeListMethodCustody50LA* method,
        const OriginalWreckComponent50LA* portal,std::optional<std::uint32_t> distance)noexcept {
        auto& f=fogPoolAllocation_;
        if(!f.entered||f.nativeWithdrawn||!f.slot14Stored||f.slot14AliasWithdrawn||f.primaryOccurrence!=occurrence||
           f.secondaryOccurrence!=secondary||f.record!=record||!preflightNative()||
           !shared_->sourceLinkAccessCurrent(group_,identity()))return false;
        if(sourcePC==0x280cd4u){if(f.payloadMask||!child||method||portal||distance)return false;
            record->child04=child;f.child04=child;f.payloadMask=1u;return true;}
        if(sourcePC==0x280cd8u){if(f.payloadMask!=1u||child||method!=f.slot14Method||portal||distance)return false;
            record->method08=nullptr;record->collisionMethod08=method;f.payloadMask=3u;return true;}
        if(sourcePC==0x280ce0u){if(f.payloadMask!=3u||child||method||!portal||distance)return false;
            record->portal0C=portal;f.portal0C=portal;f.payloadMask=7u;return true;}
        if(sourcePC==0x280ce8u){if(f.payloadMask!=7u||child||method||portal||!distance)return false;
            record->distance10=*distance;f.distance10=distance;f.payloadMask=15u;return true;}
        if(child||method||portal||distance||!f.oldHeadRead)return false;
        if(sourcePC==0x280cf0u){if(f.payloadMask!=15u||record_.next00)return false;
            record->next00=nullptr;f.payloadMask=31u;return true;}
        if(sourcePC==0x280cf4u){if(f.payloadMask!=31u||record_.next00||record->next00)return false;
            record_.next00=record;f.payloadMask=63u;return true;}
        return false; //No generic secondary ledger, portal return or additional-record admission.
    }
    bool readFogPoolHead280CEC(const void* occurrence,const void* secondary,bool& headNull)noexcept {
        auto& f=fogPoolAllocation_;
        if(headNull||!f.entered||f.nativeWithdrawn||f.slot14AliasWithdrawn||f.payloadMask!=15u||f.oldHeadRead||
           f.primaryOccurrence!=occurrence||f.secondaryOccurrence!=secondary||!preflightNative()||
           !shared_->sourceLinkAccessCurrent(group_,identity())||record_.next00)return false;
        headNull=record_.next00==nullptr;f.oldHeadRead=true;return true; //Fresh source read, justified by reached280B9C and no intervening head publication.
    }
    bool withdrawFogPoolSlot14AliasNative(const void* occurrence,const void* secondary)noexcept {
        auto& f=fogPoolAllocation_;
        if(!f.entered||f.nativeWithdrawn||!f.slot14Stored||f.primaryOccurrence!=occurrence||
           f.secondaryOccurrence!=secondary||!preflightNative())return false;
        if(f.slot14AliasWithdrawn)return !f.record->fogSlot14&&!f.slot14Method&&!f.slot14Member;
        if(f.payloadMask==63u)record_.next00=nullptr; //Native head unlink precedes every payload/List alias retirement.
        if(f.payloadMask&1u)f.record->child04=nullptr;
        if(f.payloadMask&2u){f.record->method08=nullptr;f.record->collisionMethod08=nullptr;}
        if(f.payloadMask&4u)f.record->portal0C=nullptr;
        if(f.payloadMask&16u)f.record->next00=nullptr;
        f.child04=f.portal0C=nullptr;
        f.record->fogSlot14=nullptr;f.slot14Method=nullptr;f.slot14Member=nullptr;f.slot14AliasWithdrawn=true;
        return true; //Native alias retirement only. Reached SW and unknown numeric address history are not rolled back.
    }
    bool releaseFogPoolAllocationNative(const void* occurrence,const void* secondary)noexcept {
        auto& f=fogPoolAllocation_;if(!f.entered)return true;
        if(f.nativeWithdrawn)return !f.pool&&!f.record;
        if(f.primaryOccurrence!=occurrence||f.secondaryOccurrence!=secondary||
           f.slot14AliasWithdrawn!=f.slot14Stored||!preflightNative())return false;
        std::uint32_t nativePC=0;
        if(!shared_->releaseSource105830(group_,identity(),f.record,nativePC))return false;
        f.pool=nullptr;f.record=nullptr;f.nativeWithdrawn=true;return true; //Native release mechanics, not a new source105830 call/return.
    }
    bool readFogPool443138Native(Shared& actualApplicationShared,const void* actualOccurrence,FogPoolAllocator*& pool)const noexcept {
        if(pool||!actualOccurrence||actualOccurrence!=primary_.occurrence||shared_!=&actualApplicationShared||
           primary_.fogSelectedAddressNonNull!=true||primary_.stores!=31u||primary_.nativeWithdrawn||
           destroyEntered_||nativeWithdrawn_||!lease_||!returned_||!preflightNative()||
           !shared_->sourceLinkAccessCurrent(group_,identity())||
           shared_->allocator443138_.get()!=shared_->sourceGroupRetainedAllocator_)return false;
        pool=shared_->allocator443138_.get();return pool!=nullptr; //Actual280CCC load; caller supplied no pool or group identity.
    }
    bool fogPoolAliasesCurrentNative(const void* actualOccurrence,const FogPoolAllocator* pool,const SecondaryLink* freeAlias)const noexcept {
        if(!pool||!actualOccurrence||actualOccurrence!=primary_.occurrence||primary_.fogSelectedAddressNonNull!=true||
           primary_.stores!=31u||primary_.nativeWithdrawn||destroyEntered_||nativeWithdrawn_||!lease_||!returned_||
           !preflightNative()||!shared_||!shared_->sourceGroupCurrent(group_)||!shared_->ownsReturnedSourceObserver(identity())||
           pool!=shared_->allocator443138_.get()||pool!=shared_->sourceGroupRetainedAllocator_)return false;
        //The existing observer lease retains allocator/chunk lifetime. Historical free-head identity need not remain free.
        return !freeAlias||shared_->allocatedRecord(freeAlias);
    }
    enum class SecondaryLinkStatus : std::uint8_t { nativeRefused,returnedNull,returnedRecord };
    struct SecondaryLinkResult {
        SecondaryLinkStatus status=SecondaryLinkStatus::nativeRefused;
        SecondaryLink* record=nullptr;
        std::uint32_t sourcePC=0x105720u;
    };
    struct SourceSecondaryLink {
        const void* occurrence=nullptr;
        SecondaryLinkResult allocation;
        SecondaryLink* oldHead=nullptr;
        SecondaryLink payload; //Immutable snapshot only after head publication.
        bool published=false,nativeWithdrawn=false;
    };
    struct SourceSecondary280BF0 {
        const void* occurrence=nullptr;
        bool entered=false,returned=false,nativeWithdrawn=false;
        std::uint32_t count=0,returnPC=0;
        //Existing World source domain admits at most20 portal memberships.
        std::array<SourceSecondaryLink,20> links{};
    } secondary_;
    //Disabled at the actual callback: only a completed mode1 registration owner
    //may call this private handoff. No source registration/return is inferred.
    struct DetachedListReceipt {
        OriginalNativeListMethodCustody50LA* method=nullptr;
        const void* borrower=nullptr;const void* member=nullptr;
        std::uint32_t slot=0;bool released=false;
    };
    struct DetachedCustody {
        bool entered=false,retired=false;
        const void* occurrence=nullptr;const void* member=nullptr;
        const ::ra_pod_scene50LA::Owner* world=nullptr;
        const OriginalApplicationEntities50LA* application=nullptr;
        std::uint32_t primaryZone=0;
        DetachedListReceipt primary;
        std::array<DetachedListReceipt,20> secondary{};
        std::uint32_t count=0;
    } detached_;
    static bool detachedListCurrentNative(const DetachedListReceipt& r)noexcept {
        if(!r.method||!r.borrower||!r.member||!r.slot||r.slot>1024)return false;
        if(r.released)return true; //Never dereference a retired List pin.
        const auto& c=*r.method;const auto& e=c.sourceEntries_;
        if(!c.sourceMode_||!c.world_||!c.application_||c.reference00_!=2u||
           !c.source_.constructorReturned||!c.source_.referenceKnown||!c.source_.constructorReference||
           c.arrayReference_||c.nodeReference_||c.begun_||c.nodeAliasWasAcquired_||
           c.usage10_!=1u||c.ordinal_<10u||c.ordinal_>=20u||
           c.source_.vtable08!=0x3d2d30u||!c.sourceCollisionEntriesCurrentNative()||
           !e.words||!c.source_.field40||
           e.members[r.slot-1]!=r.member||e.borrowers[r.slot-1]!=r.borrower||e.wordKnown[r.slot-1])return false;
        //A later callback can be bounded with a different pending selection on
        //this same List. Retained currentness must still admit reverse cleanup
        //of that later callback; it does not authorize free-head mutation here.
        if(e.pending&&(e.pending==r.borrower||e.pendingIdentity==r.member||e.pendingSlot==r.slot))return false;
        unsigned pins=0;for(const auto* p:c.sourceCollisionRegistrationBorrowers_)if(p==r.borrower)++pins;
        return pins==1;
    }
    bool detachedHistoryNative()const noexcept {
        const auto& d=detached_;
        if(!d.entered)return !d.retired&&!d.occurrence&&!d.member&&!d.count&&!d.primary.method&&!d.world&&!d.application&&!d.primaryZone;
        if(!destroyReturned_||lease_||destructionArgument_!=-1||d.occurrence!=primary_.occurrence||
           !d.member||!d.world||!d.application||d.primaryZone>=10u||d.count!=secondary_.count||d.count>d.secondary.size()||
           d.primary.method!=primary_.method||d.primary.borrower!=d.occurrence||
           d.primary.member!=d.member||d.primary.slot!=primary_.slot||
           d.primary.released!=primary_.nativeWithdrawn||!detachedListCurrentNative(d.primary))return false;
        if(!d.primary.released&&(d.primary.method->world_!=d.world||
           d.primary.method->application_!=d.application||d.primary.method->ordinal_!=10u+d.primaryZone))return false;
        bool retiredSuffix=false;
        for(std::uint32_t i=0;i<d.count;++i){const auto& r=d.secondary[i];const auto& e=secondary_.links[i];
            if(r.method!=e.payload.collisionMethod08||r.borrower!=e.occurrence||r.member!=d.member||
               r.slot!=e.payload.slot14||r.released!=e.nativeWithdrawn||!detachedListCurrentNative(r))return false;
            if(r.released)retiredSuffix=true;
            else if(retiredSuffix||d.primary.released||r.method->world_!=d.world||r.method->application_!=d.application)return false;
        }
        if(d.retired)return d.primary.released&&secondary_.nativeWithdrawn;
        return shared_&&shared_->detachedMembershipCurrent(group_,identity(),d.occurrence,&d);
    }
    bool destroyRegisteredMode1Temporary280920(const void* actualOccurrence,
        const void* actualMember,std::uint32_t actualPrimaryZone,
        const ::ra_pod_scene50LA::Owner* actualWorld,const OriginalApplicationEntities50LA* actualApplication)noexcept {
        //The callback must first prove2809C0,
        //280BF0,120190/1201F0 and the actual120200(-1) call, separately.
        if(detached_.entered||!actualOccurrence||!actualMember||!actualWorld||!actualApplication||actualPrimaryZone>=10u||
           !preflightNative()||!lease_||destroyEntered_||nativeWithdrawn_||
           primary_.occurrence!=actualOccurrence||primary_.stores!=31u||primary_.fogSelectedAddressNonNull||primary_.nativeWithdrawn||
           !secondary_.returned||secondary_.nativeWithdrawn||!shared_)return false;
        DetachedCustody next;next.entered=true;next.occurrence=actualOccurrence;
        next.member=actualMember;next.count=secondary_.count;next.world=actualWorld;next.application=actualApplication;next.primaryZone=actualPrimaryZone;
        next.primary={const_cast<OriginalNativeListMethodCustody50LA*>(primary_.method),actualOccurrence,actualMember,primary_.slot,false};
        if(!detachedListCurrentNative(next.primary)||next.primary.method->ordinal_!=10u+actualPrimaryZone||
           next.primary.method->world_!=actualWorld||next.primary.method->application_!=actualApplication)return false;
        for(std::uint32_t i=0;i<next.count;++i){const auto& e=secondary_.links[i];
            if(!e.published||e.nativeWithdrawn||!e.allocation.record||
               !e.payload.child04||!e.payload.portal0C||!ownsSecondaryLinkNative(e.allocation.record))return false;
            auto& r=next.secondary[i];
            r={const_cast<OriginalNativeListMethodCustody50LA*>(e.payload.collisionMethod08),e.occurrence,actualMember,e.payload.slot14,false};
            if(!detachedListCurrentNative(r)||r.method->application_!=next.primary.method->application_||
               r.method->world_!=next.primary.method->world_)return false;
            if(r.borrower==next.primary.borrower)return false;
            for(std::uint32_t j=0;j<i;++j)if(next.secondary[j].borrower==r.borrower||
                (next.secondary[j].method==r.method&&next.secondary[j].slot==r.slot))return false;
            if(r.method==next.primary.method&&r.slot==next.primary.slot)return false;
        }
        if(!shared_->reserveDetachedMembershipNative(identity()))return false;
        //All fallible work precedes source mutation. Stable record_, callback
        //occurrence, List borrower identities and pool tags are NOT rewritten.
        const auto count=shared_->observers443140_;
        detached_=next; //Install immutable native receipts BEFORE source destructor effects.
        if(!shared_->detachMembership28094C(identity(),actualOccurrence,&detached_)){
            detached_=DetachedCustody{};return false; //Transfer refusal has changed no source state.
        }
        destroyEntered_=true;destructionArgument_=-1;
        countBeforeDestroy_=count;countDecremented_=true;lease_=false;
        //280958 nonlast branch skips pool destruction; -1 skips source delete.
        //No281490/source unlink occurs. Native backing is now opaque custody.
        destroyReturned_=true;destroyPC_=0x2809b8u;return true;
    }
    static void releaseDetachedListSlotNative(DetachedListReceipt& r)noexcept {
        auto& c=*r.method;auto& e=c.sourceEntries_;const auto slot=r.slot;
        e.nativeFreeNext[slot-1]=*c.source_.field40;e.wordKnown[slot-1]=false;
        e.members[slot-1]=e.borrowers[slot-1]=nullptr;c.source_.field40=slot;
    }
    static void releaseDetachedListPinNative(DetachedListReceipt& r)noexcept {
        auto& pins=r.method->sourceCollisionRegistrationBorrowers_;
        for(auto i=pins.begin();i!=pins.end();++i)if(*i==r.borrower){pins.erase(i);break;}
        r.released=true;
    }
    bool withdrawDetachedMembershipNative()noexcept {
        auto& d=detached_;if(!d.entered)return false;if(d.retired)return true;
        if(!preflightNative()||!detachedHistoryNative()||!shared_||shared_->sourceObserver280890_)return false;
        //Mutation admission is stricter than retained currentness: a newer
        //callback's pending slot must be canceled before any free head changes.
        //Check the entire receipt set before releasing even one slot or pin.
        const auto canRelease=[](const DetachedListReceipt& r)noexcept {
            return r.released||!r.method->sourceEntries_.pending;
        };
        if(!canRelease(d.primary))return false;
        for(std::uint32_t i=0;i<d.count;++i)if(!canRelease(d.secondary[i]))return false;
        //Complete prevalidation above; native-only reverse release. No source
        //281490/280920 replay, no allocations, FP, or callbacks while mutating.
        for(std::uint32_t i=d.count;i>0;--i){auto& r=d.secondary[i-1];auto& e=secondary_.links[i-1];
            if(r.released)continue;
            releaseDetachedListSlotNative(r);
            auto* link=e.allocation.record;auto& pool=*shared_->allocator443138_;
            record_.next00=e.oldHead;link->next00=pool.free2C;pool.free2C=link;link->nativeBorrower=nullptr;
            e.nativeWithdrawn=true;releaseDetachedListPinNative(r);
        }
        secondary_.nativeWithdrawn=true;
        if(!d.primary.released){releaseDetachedListSlotNative(d.primary);releaseDetachedListPinNative(d.primary);}
        primary_.nativeWithdrawn=true;
        if(!shared_->releaseDetachedMembershipNative(identity(),&d))return false;
        d.retired=true;shared_=nullptr;nativeWithdrawn_=true;
        return true; //Backing may now retire even if the cohort root retires first.
    }
    const void* sourceGroupIdentityNative()const noexcept {return group_;}
    const void* secondaryHeadNative()const noexcept {return record_.next00;}
    bool secondaryAllocationRecordedNative(const void* actualSecondaryOccurrence)const noexcept {
        for(std::uint32_t i=0;i<secondary_.count;++i)
            if(secondary_.links[i].occurrence==actualSecondaryOccurrence)return true;
        return false;
    }
    bool ownsSecondaryLinkNative(const SecondaryLink* link)const noexcept {
        return shared_&&shared_->ownsSourceLink(group_,identity(),link);
    }
    bool secondaryHistoryNative()const noexcept {
        const auto& h=secondary_;
        if(!h.entered)return !h.occurrence&&!h.nativeWithdrawn&&!h.returned&&!h.returnPC&&!h.count&&fogPoolHeadCurrentNative()&&
            fogPoolOwnerRecordsCurrentNative();
        if(fogPoolAllocation_.entered||!h.occurrence||h.occurrence!=primary_.occurrence||primary_.stores!=31u||h.count>h.links.size()||
           (h.returned?h.returnPC!=0x280d38u:h.returnPC!=0u))return false;
        const SecondaryLink* head=nullptr;const SecondaryLink* historicalHead=nullptr;
        bool retiredSuffix=false;unsigned liveRecords=0;
        for(std::uint32_t i=0;i<h.count;++i){const auto& e=h.links[i];
            if(!e.occurrence)return false;
            for(std::uint32_t j=0;j<i;++j)if(h.links[j].occurrence==e.occurrence)return false;
            if(e.nativeWithdrawn)retiredSuffix=true;
            else if(retiredSuffix||h.nativeWithdrawn||nativeWithdrawn_)return false;
            const bool allocated=e.allocation.status==SecondaryLinkStatus::returnedRecord;
            if(allocated!=bool(e.allocation.record)||
               (e.allocation.status!=SecondaryLinkStatus::nativeRefused&&e.allocation.sourcePC!=0x105824u)||
               (e.published&&!allocated))return false;
            if(e.published){
                if(e.payload.fogSlot14)return false; //Generic ledger never acquires the Fog typed-address alternative.
                if(e.oldHead!=historicalHead||e.payload.next00!=historicalHead||
                   e.payload.nativeBorrower!=identity())return false;
                historicalHead=e.allocation.record;
            }
            if(h.returned&&!e.published)return false;
            if(!e.nativeWithdrawn&&allocated){
                if(!ownsSecondaryLinkNative(e.allocation.record)||e.allocation.record->fogSlot14)return false;++liveRecords;
                if(e.published){const auto& p=e.payload;const auto& c=*e.allocation.record;
                    if(e.oldHead!=head||p.next00!=e.oldHead||p.nativeBorrower!=identity()||
                       c.next00!=p.next00||c.child04!=p.child04||c.method08!=p.method08||
                       c.collisionMethod08!=p.collisionMethod08||c.portal0C!=p.portal0C||
                       c.distance10!=p.distance10||c.slot14!=p.slot14)return false;
                    head=e.allocation.record;
                }
            }
            //A failed allocation or unlinked allocation is a bounded final
            //iteration, not permission to continue a later source edge.
            if(!e.published&&i+1u<h.count)return false;
        }
        if(record_.next00!=head)return false;
        if(shared_&&!nativeWithdrawn_){
            //Pin every same-owner pool record, including an allocated record
            //whose source NULL/payload/head boundary has not returned yet.
            unsigned actual=0;
            if(shared_->allocator443138_)for(const auto& chunk:shared_->allocator443138_->chunks)if(chunk)
                for(const auto& link:chunk->records)if(link.nativeBorrower==identity())++actual;
            if(actual!=liveRecords)return false;
        }else if(liveRecords)return false;
        return true;
    }
    bool beginSecondary280BF0(const void* actualOccurrence)noexcept {
        if(secondary_.entered||!actualOccurrence||actualOccurrence!=primary_.occurrence||
           primary_.stores!=31u||primary_.fogSelectedAddressNonNull||primary_.nativeWithdrawn||nativeWithdrawn_||!preflightNative())return false;
        secondary_.entered=true;secondary_.occurrence=actualOccurrence;return true;
    }
    SecondaryLinkResult allocateSecondary105720(const void* actualSecondaryOccurrence)noexcept {
        auto& h=secondary_;
        if(!h.entered||h.returned||h.nativeWithdrawn||!actualSecondaryOccurrence||h.count==h.links.size()||
           (h.count&&(!h.links[h.count-1].published||h.links[h.count-1].nativeWithdrawn))||!preflightNative())return {};
        for(std::uint32_t i=0;i<h.count;++i)if(h.links[i].occurrence==actualSecondaryOccurrence)return {};
        auto& e=h.links[h.count++];e.occurrence=actualSecondaryOccurrence;
        const auto r=shared_->allocateSource105720(group_,identity());
        e.allocation={r.status==Shared::SourceLinkStatus::returnedRecord?SecondaryLinkStatus::returnedRecord:
            r.status==Shared::SourceLinkStatus::returnedNull?SecondaryLinkStatus::returnedNull:SecondaryLinkStatus::nativeRefused,
            r.record,r.sourcePC};return e.allocation;
    }
    bool publishSecondary280CF4(const void* actualSecondaryOccurrence,SecondaryLink* actualLink,
        const void* expectedOldHead)noexcept {
        auto& h=secondary_;
        if(!h.entered||h.returned||h.nativeWithdrawn||!h.count||!preflightNative())return false;
        auto& e=h.links[h.count-1];
        if(e.occurrence!=actualSecondaryOccurrence||e.published||e.nativeWithdrawn||!actualLink||
           e.allocation.record!=actualLink||!ownsSecondaryLinkNative(actualLink)||
           record_.next00!=expectedOldHead||actualLink->next00!=expectedOldHead||
           actualLink->method08||!actualLink->collisionMethod08||!actualLink->child04||
           !actualLink->portal0C||!actualLink->slot14||actualLink->fogSlot14)return false;
        //COL owns reached280CD0..280CF0 payload stores and exact edge/list
        //identity. Only after those stores may this helper publish head00.
        e.oldHead=actualLink->next00;e.payload=*actualLink;
        record_.next00=actualLink;e.published=true;return true; //Actual280CF4 store.
    }
    bool finishSecondary280D30(const void* actualOccurrence)noexcept {
        auto& h=secondary_;
        if(!h.entered||h.returned||h.nativeWithdrawn||actualOccurrence!=h.occurrence||!preflightNative())return false;
        for(std::uint32_t i=0;i<h.count;++i)if(!h.links[i].published||h.links[i].nativeWithdrawn)return false;
        //Only the COL traversal owner can prove actual portal cursorNULL.
        //No head/list/pool mutation is inferred from this control epilogue.
        h.returned=true;h.returnPC=0x280d38u;return true; //JR280D30/delay280D34.
    }
    bool releaseSecondaryNative(const void* actualSecondaryOccurrence)noexcept {
        auto& h=secondary_;
        if(detached_.entered||!h.entered||h.nativeWithdrawn||!actualSecondaryOccurrence||!preflightNative())return false;
        SourceSecondaryLink* selected=nullptr;
        for(std::uint32_t i=h.count;i>0;--i)if(!h.links[i-1].nativeWithdrawn){selected=&h.links[i-1];break;}
        if(!selected||selected->occurrence!=actualSecondaryOccurrence)return false;
        auto& e=*selected;
        //COL must cancel this exact method insertion before native pool
        //withdrawal. The shared release mechanics do not certify281490.
        if(e.allocation.record){
            std::uint32_t nativePC=0;
            if(!shared_->releaseSource105830(group_,identity(),e.allocation.record,nativePC))return false;
        }
        if(e.published)record_.next00=e.oldHead; //Native unlink, not a new guest store receipt.
        e.nativeWithdrawn=true;return true;
    }
    bool withdrawSecondaryNative(const void* actualOccurrence)noexcept {
        auto& h=secondary_;
        if(!h.entered)return actualOccurrence==nullptr;
        if(detached_.entered||actualOccurrence!=h.occurrence||!preflightNative())return false;
        for(std::uint32_t i=0;i<h.count;++i)if(!h.links[i].nativeWithdrawn)return false;
        if(record_.next00||shared_->ownerHasLinks(identity()))return false;
        h.nativeWithdrawn=true;return true;
    }
    std::uint32_t registrationPC()const noexcept {return primary_.pc;}
    bool registrationHistoryNative()const noexcept {
        const auto& h=primary_;const bool fog=h.fogSelectedAddressNonNull.has_value();
        const bool typedSlot=fog&&(h.stores&4u);
        if(!typedSlot&&(fogSlot14_||fogSlot14AliasWithdrawn_))return false;
        if(typedSlot&&(record_.slot14||(fogSlot14AliasWithdrawn_?fogSlot14_!=nullptr:fogSlot14_==nullptr)||
            (h.nativeWithdrawn&&!fogSlot14AliasWithdrawn_)))return false;
        if(!h.entered)return !secondary_.entered&&!h.occurrence&&!h.zone&&!h.method&&!h.slot&&!h.pc&&!h.methodRead&&!h.slotRead&&
            !h.nativeWithdrawn&&!h.stores&&!h.emptyAssertion.entered&&!h.methodAssertion.entered&&!h.slotAssertion.entered&&!fog;
        if(!h.occurrence||!returned_||!Shared::sourceAssertionHistoryCurrent(h.emptyAssertion)||
           !Shared::sourceAssertionHistoryCurrent(h.methodAssertion)||!Shared::sourceAssertionHistoryCurrent(h.slotAssertion)||
           (h.methodRead&&!h.emptyAssertion.passed)||(h.slotRead&&(!h.methodRead||!h.method||!h.methodAssertion.passed))||
           (!h.methodRead&&(h.method||h.methodAssertion.entered))||
           (!h.slotRead&&(h.slot||(h.slotAssertion.entered&&!fog)))||
           (fog&&(!*h.fogSelectedAddressNonNull||h.slotRead||h.slot||!h.methodRead||!h.method||
               !h.methodAssertion.passed||!h.slotAssertion.entered||secondary_.entered||detached_.entered)))return false;
        if(fog){if(h.stores!=0u&&h.stores!=1u&&h.stores!=3u&&h.stores!=7u&&h.stores!=15u&&h.stores!=31u)return false;}
        else if(h.stores!=0u&&h.stores!=31u)return false; //Legacy callers keep the prior atomic contract.
        if(h.emptyAssertion.conditionRead&&!h.emptyAssertion.passed)return false;
        if(h.methodAssertion.conditionRead&&h.methodAssertion.passed!=(h.method!=nullptr))return false;
        if(h.slotAssertion.conditionRead&&h.slotAssertion.passed!=h.fogSelectedAddressNonNull.value_or(h.slot!=0u))return false;
        if(h.stores){
            if(!h.zone||!h.method||!h.slotAssertion.passed)return false;
            if(fog){
                const std::uint32_t expected=h.stores==1u?0x280b94u:h.stores==3u?0x280b98u:
                    h.stores==7u?0x280b9cu:h.stores==15u?0x280ba0u:0x280bf0u;
                if(h.pc!=expected)return false;
            }else if(!h.slot||!h.slotRead||h.pc!=0x280bf0u)return false;
        }else if(!h.emptyAssertion.passed){if(h.pc!=0x2809e8u&&h.pc!=0x280a40u)return false;}
        else if(!h.methodRead){if(h.pc!=0x280a74u)return false;}
        else if(!h.methodAssertion.passed){if(h.pc!=0x280a80u&&h.pc!=0x280ab8u)return false;}
        else if(!h.slotRead&&!fog){if(h.pc!=0x280ae8u)return false;}
        else if(!h.slotAssertion.passed){if(h.pc!=0x280b08u&&h.pc!=0x280b40u)return false;}
        else if(h.pc!=(fog?0x280b70u:0x280b88u))return false;
        return secondaryHistoryNative()&&record_.zone04==((h.stores&1u)?h.zone:nullptr)&&
            record_.method08==((h.stores&2u)?h.method:nullptr)&&
            (typedSlot?!record_.slot14:record_.slot14==((h.stores&4u)?h.slot:0u))&&
            record_.center20==((h.stores&16u)?h.center:std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}});
    }
    //Fog-only phased handoffs retain the SAME observer/occurrence and process
    //assertion owners, while its caller records actual branch/call/return PCs.
    bool enterFogRegistration2809C0(const void* occurrence,const void* zone,
        const std::array<std::uint32_t,4>& center)noexcept {
        if(primary_.entered||!occurrence||!zone||!returned_||!lease_||destroyEntered_||nativeWithdrawn_||!preflightNative())return false;
        auto& h=primary_;h.entered=true;h.occurrence=occurrence;h.zone=zone;h.center=center;h.pc=0x2809e8u;return true;
    }
    bool beginFogEmptyAssertion2809E8()noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !h.entered||h.emptyAssertion.entered||h.pc!=0x2809e8u||!preflightNative())return false;
        return Shared::beginSourceAssertion(1,h.emptyAssertion);
    }
    bool finishFogEmptyAssertion280A40()noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !h.emptyAssertion.ready||h.emptyAssertion.passed||!preflightNative())return false;
        h.pc=0x280a40u;if(!Shared::finishSourceAssertion(h.emptyAssertion,record_.zone04==nullptr))return false;
        h.pc=0x280a74u;return true;
    }
    bool retainFogMethod280A7C(const OriginalNativeListMethodCustody50LA* method)noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !h.emptyAssertion.passed||h.methodRead||h.pc!=0x280a74u||!preflightNative())return false;
        h.method=method;h.methodRead=true;h.pc=0x280a80u;return true;
    }
    bool beginFogMethodAssertion280A80()noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !h.methodRead||h.methodAssertion.entered||h.pc!=0x280a80u||!preflightNative())return false;
        return Shared::beginSourceAssertion(2,h.methodAssertion);
    }
    bool finishFogMethodAssertion280AB8()noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !h.methodAssertion.ready||h.methodAssertion.passed||!preflightNative())return false;
        h.pc=0x280ab8u;if(!Shared::finishSourceAssertion(h.methodAssertion,h.method!=nullptr))return false;
        h.pc=0x280ae8u;return true;
    }
    bool beginFogSlotAssertion280B08(const std::uint32_t* actualEntry)noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !actualEntry||!h.methodRead||!h.method||!h.methodAssertion.passed||h.slotRead||h.slot||
            h.fogSelectedAddressNonNull||h.slotAssertion.entered||h.pc!=0x280ae8u||!preflightNative())return false;
        //The trigger owns the exact typed alias and exclusive pending List
        //reservation. Retain only nonnullness here; never narrow its address.
        h.fogSelectedAddressNonNull=true;h.pc=0x280b08u;
        return Shared::beginSourceAssertion(3,h.slotAssertion);
    }
    bool finishFogSlotAssertion280B40(const std::uint32_t* actualEntry)noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||
            !actualEntry||h.fogSelectedAddressNonNull!=true||!h.slotAssertion.ready||h.slotAssertion.passed||!preflightNative())return false;
        h.pc=0x280b40u;if(!Shared::finishSourceAssertion(h.slotAssertion,actualEntry!=nullptr))return false;
        h.pc=0x280b70u;return true; //Caller still owns every method+24 argument, call and callee effect.
    }
    bool publishFogScalar280B90(std::uint32_t sourcePC,std::uint32_t* actualEntry=nullptr)noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||fogSlot14AliasWithdrawn_||
            h.fogSelectedAddressNonNull!=true||!h.slotAssertion.passed||!h.zone||!h.method||
            (sourcePC!=0x280b98u&&actualEntry)||!preflightNative())return false;
        if(sourcePC==0x280b90u){if(h.stores||h.pc!=0x280b70u)return false;
            record_.zone04=h.zone;h.stores=1;h.pc=0x280b94u;return true;}
        if(sourcePC==0x280b94u){if(h.stores!=1u||h.pc!=0x280b94u)return false;
            record_.method08=h.method;h.stores=3;h.pc=0x280b98u;return true;}
        if(sourcePC==0x280b98u){if(h.stores!=3u||h.pc!=0x280b98u||!actualEntry)return false;
            record_.slot14.reset();fogSlot14_=actualEntry;h.stores=7;h.pc=0x280b9cu;return true;}
        if(sourcePC==0x280b9cu){if(h.stores!=7u||h.pc!=0x280b9cu)return false;
            record_.next00=nullptr;h.stores=15;h.pc=0x280ba0u;return true;}
        return false;
    }
    bool publishFogCenter280BBC(const std::array<std::uint32_t,4>& actualCenterV0)noexcept {
        auto& h=primary_;if(!lease_||destroyEntered_||nativeWithdrawn_||h.nativeWithdrawn||fogSlot14AliasWithdrawn_||
            h.fogSelectedAddressNonNull!=true||h.stores!=15u||h.pc!=0x280ba0u||
            actualCenterV0!=h.center||!preflightNative())return false;
        record_.center20=actualCenterV0;h.stores=31;h.pc=0x280bf0u;return true;
    }
    bool withdrawFogSlot14AliasNative(const void* actualOccurrence)noexcept {
        auto& h=primary_;if(!actualOccurrence||actualOccurrence!=h.occurrence||h.fogSelectedAddressNonNull!=true||
            !(h.stores&4u)||!preflightNative())return false;
        if(fogSlot14AliasWithdrawn_)return !fogSlot14_;
        fogSlot14_=nullptr;fogSlot14AliasWithdrawn_=true;
        return true; //Native alias only; source stores and unknown numeric slot14 remain unchanged.
    }
    bool beginRegistration2809C0(const void* actualOccurrence,const void* actualZone,
        const std::array<std::uint32_t,4>& actualCenter)noexcept {
        if(primary_.entered||!actualOccurrence||!returned_||!lease_||destroyEntered_||nativeWithdrawn_||!preflightNative())return false;
        auto& h=primary_;h.entered=true;h.occurrence=actualOccurrence;h.zone=actualZone;h.center=actualCenter;
        h.pc=0x2809e8u;if(!Shared::beginSourceAssertion(1,h.emptyAssertion))return false;
        h.pc=0x280a40u;if(!Shared::finishSourceAssertion(h.emptyAssertion,record_.zone04==nullptr))return false;
        h.pc=0x280a74u;return true; //Caller now performs actual27F590 mode2/+38 selection.
    }
    bool recordMethod280A80(const OriginalNativeListMethodCustody50LA* actualMethod)noexcept {
        auto& h=primary_;
        if(!h.entered||h.nativeWithdrawn||h.methodRead||h.pc!=0x280a74u||!preflightNative())return false;
        h.methodRead=true;h.method=actualMethod;h.pc=0x280a80u;
        if(!Shared::beginSourceAssertion(2,h.methodAssertion))return false;
        h.pc=0x280ab8u;if(!Shared::finishSourceAssertion(h.methodAssertion,actualMethod!=nullptr))return false;
        h.pc=0x280ae8u;return true; //Caller owns actual method+20/selection effects.
    }
    bool recordSelection280B08(std::uint32_t actualSlot)noexcept {
        auto& h=primary_;
        if(!h.entered||h.nativeWithdrawn||h.slotRead||h.fogSelectedAddressNonNull||h.pc!=0x280ae8u||!h.method||!preflightNative())return false;
        h.slotRead=true;h.slot=actualSlot;h.pc=0x280b08u;
        if(!Shared::beginSourceAssertion(3,h.slotAssertion))return false;
        h.pc=0x280b40u;if(!Shared::finishSourceAssertion(h.slotAssertion,actualSlot!=0u))return false;
        h.pc=0x280b88u;return true; //Caller owns actual method+24 insertion.
    }
    bool publishPrimary280B90()noexcept {
        auto& h=primary_;
        if(!h.entered||h.nativeWithdrawn||h.stores||h.pc!=0x280b88u||!h.zone||!h.method||!h.slot||!preflightNative())return false;
        //Private COL caller must have authenticated the exact method insertion
        //return before entering. No guest allocation occurs in these stores.
        h.pc=0x280b90u;record_.zone04=h.zone;h.stores|=1u;
        h.pc=0x280b94u;record_.method08=h.method;h.stores|=2u;
        h.pc=0x280b98u;record_.slot14=h.slot;h.stores|=4u;
        h.pc=0x280b9cu;record_.next00=nullptr;h.stores|=8u;
        h.pc=0x280bbcu;record_.center20=h.center;h.stores|=16u;
        h.pc=0x280bf0u;return true; //Portal callee boundary, NOT2809C0 return.
    }
    bool withdrawRegistrationNative(const void* actualOccurrence)noexcept {
        auto& h=primary_;
        if(!h.entered)return actualOccurrence==nullptr;
        if(detached_.entered||actualOccurrence!=h.occurrence||!preflightNative()||
           ((h.fogSelectedAddressNonNull&&(h.stores&4u))&&!fogSlot14AliasWithdrawn_)||
           (secondary_.entered&&!secondary_.nativeWithdrawn)||(fogPoolAllocation_.entered&&!fogPoolAllocation_.nativeWithdrawn))return false;
        //The COL owner must first cancel its exact pending/inserted method
        //receipt. This only releases native custody; source fields stay intact.
        h.nativeWithdrawn=true;return true;
    }
    bool construct280890(Shared& shared)noexcept {
        if(entered_||nativeWithdrawn_)return false;
        ctorPC_=0x280890u;
        //This route joins the already authenticated World/default source
        //cohort. It cannot adopt a legacy/native pool or mint another root.
        const auto* group=shared.sourceObserverGroup_;
        if(!group||shared.sourceObserver280890_||!shared.sourceGroupCurrent(group)||
           !shared.ownsReturnedSourceObserver(group)||!shared.allocator443138_||
           shared.allocator443138_.get()!=shared.sourceGroupRetainedAllocator_||
           shared.observers443140_==std::numeric_limits<std::uint32_t>::max()||
           !shared.beginSourceObserver280890(identity(),group))return false;
        shared_=&shared;group_=group;entered_=lease_=true;
        ctorPC_=0x2808a4u;record_.center20={{0,0,0,0x3f800000u}};vectorKnown_=true;
        if(!shared.sourceObserverAllocator2808AC(identity(),ctorPC_))return false;
        ctorPC_=0x2808e0u;record_.next00=nullptr;scalarMask_|=1u;
        ctorPC_=0x2808e4u;record_.zone04=nullptr;scalarMask_|=2u;
        ctorPC_=0x2808e8u;record_.method08=nullptr;scalarMask_|=4u;
        ctorPC_=0x2808ecu;record_.field0C=0;scalarMask_|=8u;
        ctorPC_=0x2808f0u;record_.field10=0;scalarMask_|=16u;
        ctorPC_=0x2808f4u;record_.slot14=0;scalarMask_|=32u;
        if(!shared.sourceObserverIncrement28090C(identity(),ctorPC_))return false;
        ctorPC_=0x280918u;if(!shared.finishSourceObserver280918(identity()))return false;
        returned_=true;ctorPC_=0x280920u;return true; //JR280918/delay28091C completed.
    }
    bool preflightNative()const noexcept {
        if(!registrationHistoryNative()||!detachedHistoryNative())return false;
        if(!entered_)return !shared_&&!group_&&!lease_&&!returned_&&!vectorKnown_&&!scalarMask_&&
            !destroyEntered_&&!countDecremented_&&!destroyReturned_&&!sourceDeleted_&&
            !destructionArgument_&&!destroyPC_&&!countBeforeDestroy_&&(ctorPC_==0||ctorPC_==0x280890u);
        if(!group_||record_.unknown18To1C[0]||record_.unknown18To1C[1]||scalarMask_>63u||
           !vectorKnown_||(!primary_.entered&&record_.center20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})||
           ((scalarMask_&1u)&&!secondary_.entered&&!fogPoolHeadCurrentNative())||((scalarMask_&2u)&&!primary_.entered&&record_.zone04)||
           ((scalarMask_&4u)&&!primary_.entered&&record_.method08)||((scalarMask_&8u)&&record_.field0C)||
           ((scalarMask_&16u)&&record_.field10)||((scalarMask_&32u)&&!primary_.entered&&record_.slot14!=0u)||
           (returned_&&(scalarMask_!=63u||ctorPC_!=0x280920u)))return false;
        if(destroyEntered_){
            if(!returned_||!countDecremented_||!destroyReturned_||countBeforeDestroy_<2u||
               destroyPC_!=0x2809b8u||sourceDeleted_!=(destructionArgument_>0)||lease_)return false;
        }else if(countDecremented_||destroyReturned_||sourceDeleted_||destructionArgument_||destroyPC_||countBeforeDestroy_)return false;
        if(nativeWithdrawn_)return !shared_&&!lease_;
        if(!shared_)return false;
        if(destroyReturned_)return !lease_&&shared_->sourceGroupCurrent(group_)&&!shared_->ownerRecorded(identity())&&
            (!detached_.entered||detached_.retired||shared_->detachedMembershipCurrent(group_,identity(),detached_.occurrence,&detached_));
        if(!lease_)return false;
        return returned_?shared_->ownsReturnedSourceObserver(identity()):shared_->sourceObserverCurrent280890(identity());
    }
    bool destroy280920(std::int32_t actualArgument)noexcept {
        if(primary_.entered||!entered_||!returned_||destroyEntered_||nativeWithdrawn_||!lease_||!preflightNative()||
           !shared_||shared_->sourceObserver280890_||!shared_->sourceGroupCurrent(group_)||
           group_==identity()||shared_->observers443140_<2u||shared_->ownerHasLinks(identity())||
           record_.next00||record_.zone04||record_.method08)return false;
        //Qualified non-NULL, nonlast, unregistered branch only. The retained
        //default cohort root ensures280960 pool-destructor branch is not taken.
        destroyEntered_=true;destructionArgument_=actualArgument;
        destroyPC_=0x280940u;countBeforeDestroy_=shared_->observers443140_;
        for(auto i=shared_->sourceReturnedOwners_.begin();i!=shared_->sourceReturnedOwners_.end();++i)
            if(*i==identity()){shared_->sourceReturnedOwners_.erase(i);break;}
        for(auto i=shared_->owners_.begin();i!=shared_->owners_.end();++i)
            if(*i==identity()){shared_->owners_.erase(i);break;}
        destroyPC_=0x28094cu;--shared_->observers443140_;countDecremented_=true;lease_=false;
        //280958 skips pool destruction. Signed-positive argument alone calls
        //2E8B00 at280998; retain native storage as an opaque source-death token.
        destroyPC_=0x280990u;if(actualArgument>0){destroyPC_=0x280998u;sourceDeleted_=true;}
        destroyReturned_=true;destroyPC_=0x2809b8u;return true;
    }
    bool canWithdrawActiveRegistrationNative()const noexcept {
        //Read-only LIFO admission before callback collection/root mutation.
        //Links are validated by preflight and separately removed first.
        if(nativeWithdrawn_)return true;
        if(!preflightNative()||detached_.entered||!lease_||!returned_||!shared_)return false;
        return !shared_->sourceObserver280890_&&shared_->sourceGroupCurrent(group_)&&
            shared_->ownsReturnedSourceObserver(identity())&&!shared_->sourceReturnedOwners_.empty()&&
            shared_->sourceReturnedOwners_.back()==identity();
    }
    bool canWithdrawUnregisteredNative()const noexcept {
        //Nonmutating admission for callback partial cleanup before any parent
        //link is detached. Currentness alone does not imply cohort LIFO order.
        if(nativeWithdrawn_)return true;
        if(!preflightNative()||primary_.entered)return false;
        if(!lease_)return true;
        if(!shared_||shared_->ownerHasLinks(identity()))return false;
        if(!returned_)return shared_->sourceObserverCurrent280890(identity());
        return !shared_->sourceObserver280890_&&shared_->sourceGroupCurrent(group_)&&
            shared_->ownsReturnedSourceObserver(identity())&&!shared_->sourceReturnedOwners_.empty()&&
            shared_->sourceReturnedOwners_.back()==identity();
    }
    bool withdrawNative()noexcept {
        if(nativeWithdrawn_)return true;
        if(!preflightNative()||(detached_.entered&&!detached_.retired)||(primary_.entered&&!primary_.nativeWithdrawn))return false;
        if(lease_){
            if(returned_){if(!shared_->retireReturnedSourceObserverNative(identity()))return false;}
            else if(!shared_->abortSourceObserver280890(identity()))return false;
        }
        //Reverse heap-before-stack cancellation is native only; it cannot
        //stand in for120200/280920 or write source-deletion/control receipts.
        lease_=false;shared_=nullptr;nativeWithdrawn_=true;return true;
    }
public:
    OriginalSourceCollisionObserver50LA()=default;
    OriginalSourceCollisionObserver50LA(const OriginalSourceCollisionObserver50LA&)=delete;
    OriginalSourceCollisionObserver50LA& operator=(const OriginalSourceCollisionObserver50LA&)=delete;
    ~OriginalSourceCollisionObserver50LA(){if(lease_||(detached_.entered&&!detached_.retired)||
        (fogPoolAllocation_.entered&&!fogPoolAllocation_.nativeWithdrawn))std::terminate();}
};
}
