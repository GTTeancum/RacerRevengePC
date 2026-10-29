#pragma once
#include "candidate_collection50bx.h"
#include "original_wreck_components50la.h"
#include <exception>
#include <memory>

namespace rr::game {
class OriginalWreckQueries50LA;
class OriginalRacerQuery364Owner50LA;
class OriginalRacerQuery7A4Owner50LA;
class OriginalRacerTerrainQueryOwner50LA;
class OriginalCollisionBaseOwner50LA;
// Source D8/DC allocation ownership, distinct from cold transaction capacity.
//124170: duplicate scan, first hole, then realloc to exact high-water+1.
// This bounded owner admits at most two entries. Existing Wreck mode1 uses
// PHYSICSBODY/WATER; private actual364 uses POD. Other growth stays gated.
class OriginalWreckCategoryList50LA final {
    friend class OriginalWreckQueries50LA;
    friend class OriginalRacerQuery364Owner50LA;
    friend class OriginalRacerQuery7A4Owner50LA;
    friend class OriginalRacerTerrainQueryOwner50LA;
    friend class OriginalCollisionBaseOwner50LA;
    std::unique_ptr<std::uint32_t[]> words_;
    std::size_t extent_=0;
    bool retired_=false;
    //Private348/7A4 mode0 and364 mode1 routes bind ACTUAL stable source fields.
    //No second mutable CollectionWords projection; legacy API remains separate.
    bool mode0Route_=false,mode1Route_=false;
    std::uint32_t directMode_=0; //One actual D4 authority, never CollectionWords.
    const std::uint32_t* const* sourceD8_=nullptr;
    const std::uint32_t* sourceDC_=nullptr;
    const std::uint32_t* sourceD4_=nullptr;
    const std::uint32_t* historicalD8_=nullptr; //Compared only; never dereferenced after free.
    const OriginalWreckComponent50LA* component_=nullptr;
    //Fresh Wreck binds the SAME signed CollectionWords source lanes; no
    //reinterpret cast or second pointer/count authority. Capacity is native.
    CollectionWords50BX* freshCollection_=nullptr;
    bool selectFreshCollectionMode1124160(const OriginalWreckComponent50LA& component,
        CollectionWords50BX& view)noexcept {
        if(freshCollection_||mode0Route_||mode1Route_||retired_||words_||extent_||
           view.values||view.capacity||view.count||view.mode!=1||component.reference00!=1u||
           component.kind!=OriginalWreckComponentKind50LA::collision11B190||component.firstChild18)return false;
        component_=&component;freshCollection_=&view;view.mode=1;return true; //124164
    }
    bool matchesFreshCollection(const OriginalWreckComponent50LA& component,
        const CollectionWords50BX& view)const noexcept {
        return !retired_&&component_==&component&&freshCollection_==&view&&
            view.values==words_.get()&&view.capacity==extent_&&view.count==static_cast<std::int32_t>(extent_)&&view.mode==1;
    }
    bool appendFreshCollection124170(CollectionWords50BX& view,std::uint32_t category)noexcept {
        if(!component_||component_->reference00!=1u||!category||!matchesFreshCollection(*component_,view))return false;
        for(std::size_t i=0;i<extent_;++i)if(words_[i]==category)return true;
        for(std::size_t i=0;i<extent_;++i)if(!words_[i]){words_[i]=category;return true;}
        if(extent_==2u)return false;
        std::unique_ptr<std::uint32_t[]> next;
        try{next=std::make_unique<std::uint32_t[]>(extent_+1u);}catch(...){return false;}
        for(std::size_t i=0;i<extent_;++i)next[i]=words_[i];
        words_=std::move(next);view.values=words_.get(); //12421C
        words_[extent_]=category; //124230
        ++extent_;view.count=static_cast<std::int32_t>(extent_); //12423C
        view.capacity=extent_;return true; //Native extent only.
    }
    bool releaseFreshCollectionAfterReferenceDrop11B51C(const OriginalWreckComponent50LA& component,
        const CollectionWords50BX& view)noexcept {
        if(component.reference00||component.firstChild18||!matchesFreshCollection(component,view))return false;
        historicalD8_=view.values;words_.reset();retired_=true;return true;
        //Source D8/DC/D4 and native extent remain history; no stale dereference.
    }
    bool selectFreshDirectMode124160(const OriginalWreckComponent50LA& component,
        const std::uint32_t*& actualD8,std::uint32_t& actualDC,std::uint32_t& actualD4,std::uint32_t mode)noexcept {
        if(freshCollection_||mode>1u||mode0Route_||mode1Route_||retired_||words_||extent_||actualD8||actualDC||actualD4!=1u||
           component.reference00!=1u||component.firstChild18)return false;
        component_=&component;sourceD8_=&actualD8;sourceDC_=&actualDC;sourceD4_=&actualD4;
        actualD4=mode; //124164 ONLY source store; no pointer/count initialization.
        directMode_=mode;mode0Route_=mode==0u;mode1Route_=mode==1u;return true;
    }
    bool boundDirectFields(const std::uint32_t* const& actualD8,
        const std::uint32_t& actualDC,const std::uint32_t& actualD4)const noexcept {
        return (mode0Route_||mode1Route_)&&sourceD8_==&actualD8&&sourceDC_==&actualDC&&sourceD4_==&actualD4&&actualD4==directMode_;
    }
    bool matchesDirect(const std::uint32_t* const& actualD8,
        const std::uint32_t& actualDC,const std::uint32_t& actualD4)const noexcept {
        return !retired_&&boundDirectFields(actualD8,actualDC,actualD4)&&
            actualD8==words_.get()&&actualDC==extent_;
    }
    bool appendDirect124170(const std::uint32_t*& actualD8,std::uint32_t& actualDC,
        const std::uint32_t& actualD4,std::uint32_t actualCategory)noexcept {
        if(!component_||component_->reference00!=1u||!actualCategory||
           !matchesDirect(actualD8,actualDC,actualD4))return false;
        //124170 does NOT read D4. Direct-field admission above is native custody,
        //not a fabricated mode-dependent source algorithm.
        for(std::size_t i=0;i<extent_;++i)if(words_[i]==actualCategory)return true;
        for(std::size_t i=0;i<extent_;++i)if(!words_[i]){words_[i]=actualCategory;return true;}
        if(extent_==2u)return false; //Same bounded allocation extent as legacy.
        std::unique_ptr<std::uint32_t[]> next;
        try {next=std::make_unique<std::uint32_t[]>(extent_+1u);}catch(...){return false;}
        for(std::size_t i=0;i<extent_;++i)next[i]=words_[i];
        words_=std::move(next);actualD8=words_.get(); //12421C
        words_[extent_]=actualCategory;             //124230
        ++extent_;actualDC=static_cast<std::uint32_t>(extent_); //12423C
        return true;
    }
    bool releaseDirectAfterReferenceDrop11B51C(const OriginalWreckComponent50LA& component,
        const std::uint32_t* const& actualD8,const std::uint32_t& actualDC,
        const std::uint32_t& actualD4)noexcept {
        if(component_!=&component||component.reference00||component.firstChild18||
           !matchesDirect(actualD8,actualDC,actualD4))return false;
        historicalD8_=actualD8;words_.reset();retired_=true;
        //11B51C frees backing ONLY. ActualD8/DC/D4 survive unchanged as source
        //history. No native consumer may dereference the historical pointer.
        return true;
    }
    bool retiredDirectHistory(const OriginalWreckComponent50LA& component,
        const std::uint32_t* const& actualD8,const std::uint32_t& actualDC,
        const std::uint32_t& actualD4)const noexcept {
        return component_==&component&&!component.reference00&&retired_&&!words_&&
            boundDirectFields(actualD8,actualDC,actualD4)&&actualD8==historicalD8_&&actualDC==extent_;
    }
    //Existing mode0 callers retain their exact API. Mode1 is a distinct
    //private364 route over the SAME pointer/count owner, not a second view.
    bool selectFreshMode0124160(const OriginalWreckComponent50LA& c,
        const std::uint32_t*& d8,std::uint32_t& dc,std::uint32_t& d4)noexcept {
        return selectFreshDirectMode124160(c,d8,dc,d4,0u);
    }
    bool boundMode0Fields(const std::uint32_t* const& d8,const std::uint32_t& dc,
        const std::uint32_t& d4)const noexcept {return mode0Route_&&boundDirectFields(d8,dc,d4);}
    bool matchesMode0(const std::uint32_t* const& d8,const std::uint32_t& dc,
        const std::uint32_t& d4)const noexcept {return mode0Route_&&matchesDirect(d8,dc,d4);}
    bool appendMode0124170(const std::uint32_t*& d8,std::uint32_t& dc,
        const std::uint32_t& d4,std::uint32_t category)noexcept {
        return mode0Route_&&appendDirect124170(d8,dc,d4,category);
    }
    bool releaseMode0AfterReferenceDrop11B51C(const OriginalWreckComponent50LA& c,
        const std::uint32_t* const& d8,const std::uint32_t& dc,const std::uint32_t& d4)noexcept {
        return mode0Route_&&releaseDirectAfterReferenceDrop11B51C(c,d8,dc,d4);
    }
    bool retiredMode0History(const OriginalWreckComponent50LA& c,
        const std::uint32_t* const& d8,const std::uint32_t& dc,const std::uint32_t& d4)const noexcept {
        return mode0Route_&&retiredDirectHistory(c,d8,dc,d4);
    }
    bool selectFreshMode1124160(const OriginalWreckComponent50LA& c,
        const std::uint32_t*& d8,std::uint32_t& dc,std::uint32_t& d4)noexcept {
        return selectFreshDirectMode124160(c,d8,dc,d4,1u);
    }
    bool matchesMode1(const std::uint32_t* const& d8,const std::uint32_t& dc,
        const std::uint32_t& d4)const noexcept {return mode1Route_&&matchesDirect(d8,dc,d4);}
    bool appendMode1124170(const std::uint32_t*& d8,std::uint32_t& dc,
        const std::uint32_t& d4,std::uint32_t category)noexcept {
        return mode1Route_&&appendDirect124170(d8,dc,d4,category);
    }
    bool releaseMode1AfterReferenceDrop11B51C(const OriginalWreckComponent50LA& c,
        const std::uint32_t* const& d8,const std::uint32_t& dc,const std::uint32_t& d4)noexcept {
        return mode1Route_&&releaseDirectAfterReferenceDrop11B51C(c,d8,dc,d4);
    }
    bool retiredMode1History(const OriginalWreckComponent50LA& c,
        const std::uint32_t* const& d8,const std::uint32_t& dc,const std::uint32_t& d4)const noexcept {
        return mode1Route_&&retiredDirectHistory(c,d8,dc,d4);
    }
public:
    OriginalWreckCategoryList50LA()=default;
    OriginalWreckCategoryList50LA(const OriginalWreckCategoryList50LA&)=delete;
    OriginalWreckCategoryList50LA& operator=(const OriginalWreckCategoryList50LA&)=delete;
    ~OriginalWreckCategoryList50LA(){if(words_)std::terminate();}
    bool matches(const CollectionWords50BX& view)const noexcept {
        return !freshCollection_&&!mode0Route_&&!mode1Route_&&!retired_&&view.values==words_.get()&&view.capacity==extent_&&
            view.count==static_cast<std::int32_t>(extent_)&&view.mode==1;
    }
    bool append124170(CollectionWords50BX& view,std::uint32_t actualCategory)noexcept {
        if(!actualCategory||!matches(view))return false;
        for(std::size_t i=0;i<extent_;++i)if(words_[i]==actualCategory)return true;
        for(std::size_t i=0;i<extent_;++i)if(!words_[i]){words_[i]=actualCategory;return true;}
        if(extent_==2u)return false;
        std::unique_ptr<std::uint32_t[]> next;
        try {next=std::make_unique<std::uint32_t[]>(extent_+1u);}catch(...){return false;}
        for(std::size_t i=0;i<extent_;++i)next[i]=words_[i];next[extent_]=actualCategory;
        //Native realloc lowering: allocate/copy first, then commit the same
        //canonical pointer/count stores. Failure preserves the reached prefix.
        words_=std::move(next);++extent_;view={words_.get(),extent_,static_cast<std::int32_t>(extent_),1};return true;
    }
    bool release11B51C(CollectionWords50BX& view)noexcept {
        if(!matches(view))return false;
        words_.reset();view.values=nullptr;retired_=true;
        //DC high-water and mode remain source history; only native alias ends.
        return true;
    }
    bool retired()const noexcept{return retired_;}
    std::size_t extent()const noexcept{return extent_;}
};
}
