#pragma once
#include "spring_records48n.h"
#include <array>
#include <memory>
#include <exception>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
// Same DC record storage interface for the admitted legacy embedded route and
// the full source295C20 allocation route. Only one backing is ever selected.
class OriginalWreckRecordArray50LA final {
    friend class OriginalWreckRacerStorage50LA;
    std::array<SpringRecord48N,50> legacy_{};
    std::unique_ptr<SpringRecord48N[]> source_;
    bool explicitSource_=false,attempted_=false,retired_=false;
    bool selectSource()noexcept {
        if(explicitSource_)return false;explicitSource_=true;return true;
    }
    bool allocate295C20()noexcept {
        if(!explicitSource_||attempted_||retired_)return false;attempted_=true;
        try {source_=std::make_unique<SpringRecord48N[]>(50);}catch(...){return false;}
        return true;
    }
    bool release295B90()noexcept {
        if(!explicitSource_||retired_)return false;
        // Constructor initialized source pointer NULL; a reached failed
        // allocation therefore takes the genuine destructor null branch.
        // This resets native allocation ownership. Source DC80/8C are stale
        // pointer words after free; no source pointer-clear store is asserted.
        source_.reset();retired_=true;return true;
    }
public:
    ~OriginalWreckRecordArray50LA(){if(source_)std::terminate();}
    SpringRecord48N* data()noexcept{return explicitSource_?source_.get():legacy_.data();}
    const SpringRecord48N* data()const noexcept{return explicitSource_?source_.get():legacy_.data();}
    std::size_t size()const noexcept{return explicitSource_?(source_?50u:0u):50u;}
    SpringRecord48N& operator[](std::size_t i)noexcept{return data()[i];}
    const SpringRecord48N& operator[](std::size_t i)const noexcept{return data()[i];}
};
} // namespace rr::game
