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
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C{};
        std::array<std::uint32_t,4> center20{};
    } record_;
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
        bool entered=false,methodRead=false,slotRead=false,nativeWithdrawn=false;
        std::uint8_t stores=0;
        Shared::SourceAssertionHistory emptyAssertion,methodAssertion,slotAssertion;
    } primary_;
    using SecondaryLink=Shared::Link105720;
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
           primary_.occurrence!=actualOccurrence||primary_.stores!=31u||primary_.nativeWithdrawn||
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
        if(!h.entered)return !h.occurrence&&!h.nativeWithdrawn&&!h.returned&&!h.returnPC&&!h.count&&!record_.next00&&
            (nativeWithdrawn_||!shared_||!shared_->ownerHasLinks(identity()));
        if(!h.occurrence||h.occurrence!=primary_.occurrence||primary_.stores!=31u||h.count>h.links.size()||
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
                if(e.oldHead!=historicalHead||e.payload.next00!=historicalHead||
                   e.payload.nativeBorrower!=identity())return false;
                historicalHead=e.allocation.record;
            }
            if(h.returned&&!e.published)return false;
            if(!e.nativeWithdrawn&&allocated){
                if(!ownsSecondaryLinkNative(e.allocation.record))return false;++liveRecords;
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
           primary_.stores!=31u||primary_.nativeWithdrawn||nativeWithdrawn_||!preflightNative())return false;
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
           !actualLink->portal0C||!actualLink->slot14)return false;
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
        const auto& h=primary_;
        if(!h.entered)return !secondary_.entered&&!h.occurrence&&!h.zone&&!h.method&&!h.slot&&!h.pc&&!h.methodRead&&!h.slotRead&&
            !h.nativeWithdrawn&&!h.stores&&!h.emptyAssertion.entered&&!h.methodAssertion.entered&&!h.slotAssertion.entered;
        if(!h.occurrence||!returned_||!Shared::sourceAssertionHistoryCurrent(h.emptyAssertion)||
           !Shared::sourceAssertionHistoryCurrent(h.methodAssertion)||!Shared::sourceAssertionHistoryCurrent(h.slotAssertion)||
           (h.methodRead&&!h.emptyAssertion.passed)||(h.slotRead&&(!h.methodRead||!h.method||!h.methodAssertion.passed))||
           (!h.methodRead&&(h.method||h.methodAssertion.entered))||(!h.slotRead&&(h.slot||h.slotAssertion.entered))||
           (h.stores!=0u&&h.stores!=31u))return false;
        if(h.emptyAssertion.conditionRead&&!h.emptyAssertion.passed)return false; //Fresh zone04 was NULL.
        if(h.methodAssertion.conditionRead&&h.methodAssertion.passed!=(h.method!=nullptr))return false;
        if(h.slotAssertion.conditionRead&&h.slotAssertion.passed!=(h.slot!=0u))return false;
        if(h.stores){
            if(!h.zone||!h.method||!h.slot||!h.slotRead||!h.slotAssertion.passed||h.pc!=0x280bf0u)return false;
        }else if(!h.emptyAssertion.passed){if(h.pc!=0x2809e8u&&h.pc!=0x280a40u)return false;}
        else if(!h.methodRead){if(h.pc!=0x280a74u)return false;}
        else if(!h.methodAssertion.passed){if(h.pc!=0x280a80u&&h.pc!=0x280ab8u)return false;}
        else if(!h.slotRead){if(h.pc!=0x280ae8u)return false;}
        else if(!h.slotAssertion.passed){if(h.pc!=0x280b08u&&h.pc!=0x280b40u)return false;}
        else if(h.pc!=0x280b88u)return false;
        //Primary source proof stays immutable. Only the independent exact
        //secondary receipt may account for later head00/shared-pool changes.
        return secondaryHistoryNative()&&record_.zone04==(h.stores?h.zone:nullptr)&&
            record_.method08==(h.stores?h.method:nullptr)&&record_.slot14==(h.stores?h.slot:0u)&&
            record_.center20==(h.stores?h.center:std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}});
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
        if(!h.entered||h.nativeWithdrawn||h.slotRead||h.pc!=0x280ae8u||!h.method||!preflightNative())return false;
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
           (secondary_.entered&&!secondary_.nativeWithdrawn))return false;
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
           ((scalarMask_&1u)&&!secondary_.entered&&record_.next00)||((scalarMask_&2u)&&!primary_.entered&&record_.zone04)||
           ((scalarMask_&4u)&&!primary_.entered&&record_.method08)||((scalarMask_&8u)&&record_.field0C)||
           ((scalarMask_&16u)&&record_.field10)||((scalarMask_&32u)&&!primary_.entered&&record_.slot14)||
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
    ~OriginalSourceCollisionObserver50LA(){if(lease_||(detached_.entered&&!detached_.retired))std::terminate();}
};
}
