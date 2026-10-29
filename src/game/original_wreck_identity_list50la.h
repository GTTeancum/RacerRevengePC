#pragma once
#include "candidate_collection50bx.h"
#include "original_wreck_components50la.h"
#include <array>
#include <exception>
#include <memory>

namespace rr::game {
// Source C0/C4 allocation ownership, distinct from cold transaction capacity.
//123EB0: duplicate scan, first hole, then realloc to exact high-water+1.
// The standalone constructor admits the two authentic mutual exclusions.
// Only the private actual212AE0 racer join admits a third actual694 identity;
// later group growth remains outside this bounded owner.
class OriginalWreckIdentityList50LA final {
    friend class OriginalWreckQueries50LA;
    std::unique_ptr<std::uint32_t[]> words_;
    std::size_t extent_=0;
    bool retired_=false;
    // Fresh canonical signed C0/C4/BC view. Encoded words are reserved native
    // identities authenticated by Queries, NOT truncated guest/host pointers.
    CollectionWords50BX* freshCollection_=nullptr;
    const OriginalWreckComponent50LA* component_=nullptr;
    std::array<const OriginalWreckComponent50LA*,2> targets_{};
    const std::uint32_t* historicalC0_=nullptr; // Compare only after retirement.
    bool selectFreshCollectionMode1123EA0(const OriginalWreckComponent50LA& owner,
        CollectionWords50BX& view)noexcept {
        if(freshCollection_||retired_||words_||extent_||view.values||view.capacity||view.count||view.mode!=1||
           owner.reference00!=1u||!owner.sessionGeneration||!owner.objectKey||owner.firstChild18||
           owner.kind!=OriginalWreckComponentKind50LA::collision11B190)return false;
        component_=&owner;freshCollection_=&view;view.mode=1;return true; //123EA4
    }
    bool matchesFreshCollection(const OriginalWreckComponent50LA& owner,
        const CollectionWords50BX& view)const noexcept {
        return !retired_&&component_==&owner&&freshCollection_==&view&&extent_<=2u&&
            view.values==words_.get()&&view.capacity==extent_&&view.count==static_cast<std::int32_t>(extent_)&&view.mode==1;
    }
    bool appendFreshCollection123EB0(CollectionWords50BX& view,
        const OriginalWreckComponent50LA& target,std::uint32_t reservedIdentity)noexcept {
        if(!component_||component_->reference00!=1u||component_->kind!=OriginalWreckComponentKind50LA::collision11B190||
           !matchesFreshCollection(*component_,view)||!reservedIdentity||&target==component_||
           target.reference00!=1u||target.kind!=OriginalWreckComponentKind50LA::collision11B190||
           target.firstChild18||!target.objectKey||target.sessionGeneration!=component_->sessionGeneration)return false;
        // Native binding consistency only; Queries authenticates exact live
        // Query slot/component and its reserved ID, without publishing rows.
        for(std::size_t i=0;i<extent_;++i){
            if(words_[i]==reservedIdentity)return targets_[i]==&target;
            if(targets_[i]==&target)return false;
        }
        for(std::size_t i=0;i<extent_;++i)if(!words_[i]){
            words_[i]=reservedIdentity;targets_[i]=&target;return true; //123F30
        }
        if(extent_==2u)return false;
        std::unique_ptr<std::uint32_t[]> next;
        try{next=std::make_unique<std::uint32_t[]>(extent_+1u);}catch(...){return false;}
        for(std::size_t i=0;i<extent_;++i)next[i]=words_[i];
        // Native realloc failure preserves all source lanes; no guest allocator
        // failure-equivalence claim. Successful source stores remain ordered.
        words_=std::move(next);view.values=words_.get(); //123F5C
        words_[extent_]=reservedIdentity; //123F70
        targets_[extent_]=&target; // Native custody, not another source list.
        ++extent_;view.count=static_cast<std::int32_t>(extent_); //123F7C
        view.capacity=extent_;return true;
    }
    bool releaseFreshCollectionAfterReferenceDrop11B4F4(const OriginalWreckComponent50LA& owner,
        const CollectionWords50BX& view)noexcept {
        if(owner.reference00||owner.firstChild18||!matchesFreshCollection(owner,view))return false;
        // Other queries can already be retired. Their addresses remain native
        // history only: never dereference targets_ here or require live refs.
        historicalC0_=view.values;words_.reset();retired_=true;return true;
        // Actual C0/C4/BC and capacity remain history, no source NULL invented.
    }
public:
    OriginalWreckIdentityList50LA()=default;
    OriginalWreckIdentityList50LA(const OriginalWreckIdentityList50LA&)=delete;
    OriginalWreckIdentityList50LA& operator=(const OriginalWreckIdentityList50LA&)=delete;
    ~OriginalWreckIdentityList50LA(){if(words_)std::terminate();}
    bool matches(const CollectionWords50BX& view)const noexcept {
        return !freshCollection_&&!retired_&&view.values==words_.get()&&view.capacity==extent_&&
            view.count==static_cast<std::int32_t>(extent_)&&view.mode==1;
    }
    bool append123EB0(CollectionWords50BX& view,std::uint32_t actualIdentity)noexcept {
        return appendBounded123EB0(view,actualIdentity,2u);
    }
private:
    bool appendNativeRacer123EB0(CollectionWords50BX& view,std::uint32_t actualIdentity)noexcept {
        return appendBounded123EB0(view,actualIdentity,3u);
    }
    bool appendBounded123EB0(CollectionWords50BX& view,std::uint32_t actualIdentity,std::size_t limit)noexcept {
        if(!actualIdentity||!matches(view))return false;
        for(std::size_t i=0;i<extent_;++i)if(words_[i]==actualIdentity)return true;
        for(std::size_t i=0;i<extent_;++i)if(!words_[i]){words_[i]=actualIdentity;return true;}
        if(extent_>=limit)return false;
        std::unique_ptr<std::uint32_t[]> next;
        try {next=std::make_unique<std::uint32_t[]>(extent_+1u);}catch(...){return false;}
        for(std::size_t i=0;i<extent_;++i)next[i]=words_[i];next[extent_]=actualIdentity;
        //Native realloc lowering: allocate/copy first, then commit the same
        //canonical pointer/count stores. Failure preserves the reached prefix.
        words_=std::move(next);++extent_;view={words_.get(),extent_,static_cast<std::int32_t>(extent_),1};return true;
    }
public:
    bool release11B4F4(CollectionWords50BX& view)noexcept {
        if(!matches(view))return false;
        words_.reset();view.values=nullptr;retired_=true;
        //C4 high-water and mode remain source history; only native alias ends.
        return true;
    }
    bool retired()const noexcept{return retired_;}
    std::size_t extent()const noexcept{return extent_;}
};
}
