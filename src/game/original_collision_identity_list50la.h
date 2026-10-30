#pragma once
#include "original_wreck_components50la.h"
#include <memory>
#include <array>
#include <optional>
#include <exception>

namespace rr::game {
class OriginalCollisionBaseOwner50LA;
class OriginalRacerQuery690Owner50LA;
class OriginalRacerQuery694Owner50LA;
class OriginalRacerQuery364Owner50LA;
//Actual C0/C4 pointer-array ownership. Native typed query identities are never
//truncated into uint32/guest addresses and never mirrored into a second view.
//Only the actual bounded212918..21299C callers may append (max4 rows).
class OriginalCollisionIdentityList50LA final {
    friend class OriginalCollisionBaseOwner50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    friend class OriginalRacerQuery690Owner50LA;
    friend class OriginalRacerQuery694Owner50LA;
    friend class OriginalRacerQuery364Owner50LA;
    using C=OriginalWreckComponent50LA;using Q=OriginalCollisionBaseOwner50LA;
    std::unique_ptr<const Q*[]> rows_;
    const C* entity_=nullptr;
    const void* const* aliasAddress_=nullptr;
    const std::uint32_t* countAddress_=nullptr;
    std::uint32_t extent_=0,written_=0;
    const void* retiredAlias_=nullptr;
    bool bound_=false,retired_=false;
    bool sameFields(const C& entity,const void* const& alias,const std::uint32_t& count)const noexcept {
        return entity_==&entity&&aliasAddress_==&alias&&countAddress_==&count;
    }
    bool wordsCurrent(const C& entity,const void* const& alias,const std::uint32_t& count)const noexcept {
        if(sourceFilter_.bound||retired_)return false;
        if(!bound_)return !rows_&&!extent_&&!written_&&!alias&&!count;
        return sameFields(entity,alias,count)&&alias==rows_.get()&&count==extent_&&written_==extent_;
    }
    bool matches(const C& entity,const void* const& alias,const std::uint32_t& count)const noexcept {
        return entity.reference00==1u&&entity.objectKey&&entity.sessionGeneration&&wordsCurrent(entity,alias,count);
    }
    bool append123EB0(C& entity,const void*& alias,std::uint32_t& count,
        const Q& actualOther,const C& otherEntity)noexcept {
        if(!matches(entity,alias,count)||otherEntity.reference00!=1u||!otherEntity.objectKey||
           otherEntity.sessionGeneration!=entity.sessionGeneration)return false;
        if(!bound_){entity_=&entity;aliasAddress_=&alias;countAddress_=&count;bound_=true;}
        for(std::uint32_t i=0;i<count;++i)if(rows_[i]==&actualOther)return true; //123ED8..F00.
        for(std::uint32_t i=0;i<count;++i)if(!rows_[i]){rows_[i]=&actualOther;return true;} //123F30.
        if(count>=4u)return false; //Explicit native bounded caller domain; signed source count stays valid.
        std::unique_ptr<const Q*[]> next;
        try{next.reset(new const Q*[count+1u]);}catch(...){return false;}
        for(std::uint32_t i=0;i<count;++i)next[i]=rows_[i];
        //Native realloc policy: failure above preserves actual prior source
        //prefix; no guest mallocNULL equivalence. New last slot stays unknown
        //until its actual123F70 store, guarded by written_ rather than zero fill.
        rows_=std::move(next);extent_=count+1u;
        alias=rows_.get(); //123F5C BEFORE new element and count publication.
        rows_[count]=&actualOther;written_=count+1u; //123F70.
        ++count; //123F7C. Existing rows are stable typed identities, not retains.
        return true;
    }
    bool releaseAfterReferenceDrop11B4F4(const C& entity,const void*& alias,std::uint32_t& count)noexcept {
        if(entity.reference00||!wordsCurrent(entity,alias,count))return false;
        if(!bound_){entity_=&entity;aliasAddress_=&alias;countAddress_=&count;bound_=true;}
        retiredAlias_=alias;
        rows_.reset(); //Only actual nonnull C0 owns allocation; source fields are NOT cleared.
        retired_=true;return true;
    }
    bool retiredHistory(const C& entity,const void* const& alias,const std::uint32_t& count)const noexcept {
        return retired_&&bound_&&!rows_&&sameFields(entity,alias,count)&&alias==retiredAlias_&&
            count==extent_&&written_==extent_;
    }
    using Value=const Q*;
    using Alias=const void*;

    enum class SourceFilterStatus:std::uint8_t {owner,bounded,reached};
    struct SourceFilterResult {SourceFilterStatus status;std::uint32_t sourcePC;};
    struct SourceAppendReceipt {
        Value argument{};
        std::array<Value,2> beforeRows{};
        const void* beforeAlias=nullptr;
        const void* publishedAlias=nullptr;
        std::uint32_t beforeCount=0,afterCount=0,requestedBytes=0;
        unsigned duplicateReads=0,holeReads=0,slot=0;
        bool duplicate=false,hole=false,reallocated=false,pointerStored=false,slotStored=false,countStored=false,returned=false;
        std::optional<std::uint32_t> reloadForSlot,reloadForIncrement;
        std::uint32_t pc=0;
    };
    struct SourceFilterFacet {
        bool bound=false,nativeWithdrawn=false;
        const OriginalWreckComponent50LA* entity=nullptr;
        Alias* actualAlias=nullptr;
        std::uint32_t* actualCount=nullptr;
        std::array<SourceAppendReceipt,2> calls;
        unsigned callsEntered=0;
        std::uint32_t countHistory=0;
        std::unique_ptr<Value[]> orphan;
        const void* orphanIdentity=nullptr;
    } sourceFilter_;
    SourceFilterResult appendSource123EB0(const OriginalWreckComponent50LA& entity,
        Alias& alias,std::uint32_t& count,Value argument)noexcept {
        auto& s=sourceFilter_;
        if(s.nativeWithdrawn||entity.reference00!=1||entity.kind!=OriginalWreckComponentKind50LA::collision11B190||
           s.callsEntered>=2||(s.callsEntered&&!s.calls[s.callsEntered-1].returned))return {SourceFilterStatus::owner,0x123eb0u};
        if(!s.bound){
            if(bound_||retired_||extent_||written_||rows_||alias||count)return {SourceFilterStatus::owner,0x123eb0u};
            s.bound=true;s.entity=&entity;s.actualAlias=&alias;s.actualCount=&count;
        }
        if(s.entity!=&entity||s.actualAlias!=&alias||s.actualCount!=&count||!preflightSourceFilterNative())
            return {SourceFilterStatus::owner,0x123eb0u};
        auto& r=s.calls[s.callsEntered++];r.argument=argument;r.beforeAlias=alias;r.publishedAlias=alias;
        r.beforeCount=count;r.afterCount=count;r.pc=0x123ec0u;
        //Qualified counts0..2 are signed-positive source values; no arbitrary
        //negative/overflow count is reinterpreted as an unsigned host length.
        for(unsigned i=0;i<count;++i)r.beforeRows[i]=rows_[i];
        for(unsigned i=0;i<count;++i){
            ++r.duplicateReads;r.pc=0x123ee0u;
            if(rows_[i]==argument){r.duplicate=true;r.slot=i;r.returned=true;r.pc=0x123f94u;return {SourceFilterStatus::reached,r.pc};}
        }
        for(unsigned i=0;i<count;++i){
            ++r.holeReads;r.pc=0x123f18u;
            if(!rows_[i]){r.hole=true;r.slot=i;r.pc=0x123f30u;rows_[i]=argument;r.slotStored=true;
                r.returned=true;r.pc=0x123f94u;return {SourceFilterStatus::reached,r.pc};}
        }
        r.pc=0x123f54u;if(count>=2)return {SourceFilterStatus::bounded,r.pc};
        r.requestedBytes=(count+1u)*4u;r.reallocated=true;
        std::unique_ptr<Value[]> next;
        try{next.reset(new Value[count+1u]);}catch(...){next.reset();}
        if(next){
            //Successful realloc fallback copies old elements then frees old
            //backing internally; source pointer publication is still below.
            for(unsigned i=0;i<count;++i)next[i]=rows_[i];
            rows_=std::move(next);
        }else{
            //Original realloc failure leaves old block alive even though the
            //caller loses its source pointer. Keep exact native orphan custody.
            s.orphanIdentity=rows_.get();s.orphan=std::move(rows_);
        }
        r.pc=0x123f5cu;alias=rows_.get();r.publishedAlias=alias;r.pointerStored=true;
        r.reloadForSlot=count;r.slot=count;r.pc=0x123f70u;
        if(!rows_)return {SourceFilterStatus::bounded,r.pc};
        rows_[count]=argument;r.slotStored=true;
        r.reloadForIncrement=count;++count;r.countStored=true;r.afterCount=count;s.countHistory=count;
        r.returned=true;r.pc=0x123f94u;return {SourceFilterStatus::reached,r.pc};
    }
    bool preflightSourceFilterNative()const noexcept {
        const auto& s=sourceFilter_;
        if(!s.bound)return !s.nativeWithdrawn&&!s.entity&&!s.actualAlias&&!s.actualCount&&!s.callsEntered&&!s.orphan;
        if(s.callsEntered>2)return false;
        if(s.nativeWithdrawn)return !rows_&&!s.orphan&&!s.entity&&!s.actualAlias&&!s.actualCount;
        if(!s.entity||s.entity->reference00!=1||s.entity->kind!=OriginalWreckComponentKind50LA::collision11B190||
           !s.actualAlias||!s.actualCount)return false;
        std::array<Value,2> expected{};std::uint32_t count=0;const void* alias=nullptr;const void* orphan=nullptr;
        for(unsigned call=0;call<s.callsEntered;++call){
            const auto& r=s.calls[call];
            if(r.beforeCount!=count||r.beforeAlias!=alias||r.beforeRows!=expected||count>2)return false;
            unsigned duplicate=0;while(duplicate<count&&expected[duplicate]!=r.argument)++duplicate;
            if(duplicate<count){
                if(!r.duplicate||r.hole||r.slot!=duplicate||r.duplicateReads!=duplicate+1||r.holeReads||
                   r.reallocated||r.pointerStored||r.slotStored||r.countStored||!r.returned||r.pc!=0x123f94u||
                   r.reloadForSlot||r.reloadForIncrement||r.afterCount!=count||r.publishedAlias!=alias)return false;
                continue;
            }
            if(r.duplicate||r.duplicateReads!=count)return false;
            unsigned hole=0;while(hole<count&&expected[hole])++hole;
            if(hole<count){
                if(!r.hole||r.slot!=hole||r.holeReads!=hole+1||r.reallocated||r.pointerStored||!r.slotStored||
                   r.countStored||!r.returned||r.pc!=0x123f94u||r.reloadForSlot||r.reloadForIncrement||
                   r.afterCount!=count||r.publishedAlias!=alias)return false;
                expected[hole]=r.argument;continue;
            }
            if(r.hole||r.holeReads!=count)return false;
            if(count==2){
                if(r.reallocated||r.pointerStored||r.slotStored||r.countStored||r.returned||r.pc!=0x123f54u||
                   r.afterCount!=count||r.publishedAlias!=alias||call+1!=s.callsEntered)return false;
                continue;
            }
            if(!r.reallocated||r.requestedBytes!=(count+1)*4u||!r.pointerStored||r.slot!=count||
               r.reloadForSlot!=std::optional<std::uint32_t>(count))return false;
            alias=r.publishedAlias;
            if(!alias){
                if(r.slotStored||r.countStored||r.returned||r.reloadForIncrement||r.afterCount!=count||
                   r.pc!=0x123f70u||call+1!=s.callsEntered)return false;
                orphan=r.beforeAlias;continue;
            }
            if(!r.slotStored||!r.countStored||!r.returned||r.reloadForIncrement!=std::optional<std::uint32_t>(count)||
               r.pc!=0x123f94u||r.afterCount!=count+1)return false;
            expected[count++]=r.argument;
        }
        if(*s.actualAlias!=alias||*s.actualCount!=count||s.countHistory!=count||rows_.get()!=alias||
           s.orphanIdentity!=orphan||s.orphan.get()!=orphan)return false;
        const auto* backing=rows_?rows_.get():s.orphan.get();
        if(count&&!backing)return false;
        for(unsigned i=0;i<count;++i)if(backing[i]!=expected[i])return false;
        return true;
    }
    bool withdrawSourceFilterNative()noexcept {
        auto& s=sourceFilter_;if(!s.bound||s.nativeWithdrawn)return true;
        if(!preflightSourceFilterNative())return false;
        rows_.reset();s.orphan.reset();*s.actualAlias=nullptr;
        s.actualAlias=nullptr;s.actualCount=nullptr;s.entity=nullptr;s.nativeWithdrawn=true;return true;
    }
public:
    OriginalCollisionIdentityList50LA()=default;
    OriginalCollisionIdentityList50LA(const OriginalCollisionIdentityList50LA&)=delete;
    OriginalCollisionIdentityList50LA& operator=(const OriginalCollisionIdentityList50LA&)=delete;
    ~OriginalCollisionIdentityList50LA(){if(rows_||sourceFilter_.orphan||(sourceFilter_.bound&&!sourceFilter_.nativeWithdrawn))std::terminate();}
};
}
