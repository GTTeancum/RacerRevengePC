#pragma once
#include "candidate_collection50bx.h"
#include <exception>
#include <memory>

namespace rr::game {
// Source D8/DC allocation ownership, distinct from cold transaction capacity.
//124170: duplicate scan, first hole, then realloc to exact high-water+1.
// This constructor-only owner deliberately admits at most the two authentic
// category entries (PHYSICSBODY and WATER); other growth remains gated.
class OriginalWreckCategoryList50LA final {
    std::unique_ptr<std::uint32_t[]> words_;
    std::size_t extent_=0;
    bool retired_=false;
public:
    OriginalWreckCategoryList50LA()=default;
    OriginalWreckCategoryList50LA(const OriginalWreckCategoryList50LA&)=delete;
    OriginalWreckCategoryList50LA& operator=(const OriginalWreckCategoryList50LA&)=delete;
    ~OriginalWreckCategoryList50LA(){if(words_)std::terminate();}
    bool matches(const CollectionWords50BX& view)const noexcept {
        return !retired_&&view.values==words_.get()&&view.capacity==extent_&&
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
