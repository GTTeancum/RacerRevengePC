#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <vector>
#include <exception>
namespace rr::platform {class OriginalUiGarageVisibility50KG;class OriginalUiGarageManagerSearch50KG;}
namespace rr::game {
// Actual Application lifetime for280890/280920's443138 allocator and443140
// observer count. Garage has no portals: no105720 link allocation is reached.
// This is NOT a general allocator implementation or a page-local ready flag.
class OriginalGarageObserverShared50KG final {
    friend class rr::platform::OriginalUiGarageVisibility50KG;
    friend class rr::platform::OriginalUiGarageManagerSearch50KG;
    friend class OriginalApplicationEntities50LA;
    struct Allocator105620 {
        std::uint64_t reference00=1; //1048F0.
        std::uint32_t perBlock10=(4096u-24u)/24u;
        const void* first14=nullptr;
        const void* current18=nullptr;
        const void* cached1C=nullptr;
        std::uint32_t used20=perBlock10,itemSize24=24,blockSize28=4096;
        const void* free2C=nullptr;
        const void* auxiliary30=nullptr;
        //105620 does NOT write34. Do not manufacture a zero source value.
        std::optional<std::uint32_t> allocatedBytes34;
    };
    std::unique_ptr<Allocator105620> allocator443138_;
    std::uint32_t observers443140_=0;
    std::vector<const void*> owners_;
    bool coherent()const noexcept {
        if(observers443140_!=owners_.size()||bool(allocator443138_)!=bool(observers443140_))return false;
        if(!allocator443138_)return true;
        const auto& a=*allocator443138_;
        return a.reference00==1&&a.perBlock10==169&&a.used20==169&&a.itemSize24==24&&a.blockSize28==4096&&
            !a.first14&&!a.current18&&!a.cached1C&&!a.free2C&&!a.auxiliary30&&!a.allocatedBytes34;
    }
    // Native retained-lifetime guard before actual Application root retirement.
    bool idleForApplicationRelease()const noexcept {
        return coherent()&&!observers443140_&&!allocator443138_&&owners_.empty();
    }
    bool owns(const void* owner)const noexcept {
        if(!owner||!coherent())return false;
        for(const auto* p:owners_)if(p==owner)return true;
        return false;
    }
    bool construct280890(const void* owner)noexcept {
        if(!owner||!coherent()||owns(owner)||observers443140_==std::numeric_limits<std::uint32_t>::max())return false;
        try {
            // Native bookkeeping allocation precedes any source mutation.
            owners_.reserve(owners_.size()+1u);
            if(!allocator443138_)allocator443138_=std::make_unique<Allocator105620>();
            owners_.push_back(owner);++observers443140_;return true;
        }catch(...){return false;}
    }
    bool release280920(const void* owner)noexcept {
        if(!owns(owner))return false;
        for(auto i=owners_.begin();i!=owners_.end();++i)if(*i==owner){owners_.erase(i);break;}
        --observers443140_;
        if(!observers443140_){
            //280970 releases the actual allocator last reference. Its cold
            //1058F0 free-list traversal is empty in this no-portal owner.
            --allocator443138_->reference00;allocator443138_.reset(); //280984.
        }
        return true;
    }
public:
    OriginalGarageObserverShared50KG()=default;
    ~OriginalGarageObserverShared50KG(){if(observers443140_||allocator443138_)std::terminate();}
    OriginalGarageObserverShared50KG(const OriginalGarageObserverShared50KG&)=delete;
    OriginalGarageObserverShared50KG& operator=(const OriginalGarageObserverShared50KG&)=delete;
};
}
