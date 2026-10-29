#pragma once
#include "original_wreck_components50la.h"
#include <memory>
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
        if(retired_)return false;
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
public:
    OriginalCollisionIdentityList50LA()=default;
    OriginalCollisionIdentityList50LA(const OriginalCollisionIdentityList50LA&)=delete;
    OriginalCollisionIdentityList50LA& operator=(const OriginalCollisionIdentityList50LA&)=delete;
    ~OriginalCollisionIdentityList50LA(){if(rows_)std::terminate();}
};
}
