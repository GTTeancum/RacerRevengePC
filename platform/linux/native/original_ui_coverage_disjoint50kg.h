#pragma once
#include "original_ui_coverage_bounds50kg.h"
#include <array>
namespace rr::platform {
// Diagnostic-only greedy contiguous grouping. Rectangles are half-open pixel
// domains; touching boundaries do not share a native sample. No drawing policy.
struct OriginalUiCoverageDisjoint50KG {
    static constexpr std::uint32_t cap=64;
    std::uint64_t nonempty=0,empty=0,groups=0,rectangleArea=0,unionArea=0;
    std::uint64_t overlapFlushes=0,capFlushes=0,comparisons=0;
    std::uint32_t maxGroup=0;
    bool add(const OriginalUiCoverageBounds50KG& r)noexcept {
        if(r.x<0||r.y<0||r.width<0||r.height<0||r.x>640||r.y>448||
           r.width>640-r.x||r.height>448-r.y)return false;
        if(r.empty()){++empty;return true;}
        ++nonempty;rectangleArea+=std::uint64_t(r.width)*r.height;
        if(count_==cap){++capFlushes;finish();}
        bool overlap=false;
        for(std::uint32_t i=0;i<count_;++i){++comparisons;const auto& a=rectangles_[i];
            if(r.x<a.x+a.width&&a.x<r.x+r.width&&r.y<a.y+a.height&&a.y<r.y+r.height){overlap=true;break;}}
        if(overlap){++overlapFlushes;finish();}
        if(!count_)bounds_=r;
        else {const auto right=std::max(bounds_.x+bounds_.width,r.x+r.width);
            const auto top=std::max(bounds_.y+bounds_.height,r.y+r.height);
            bounds_.x=std::min(bounds_.x,r.x);bounds_.y=std::min(bounds_.y,r.y);
            bounds_.width=right-bounds_.x;bounds_.height=top-bounds_.y;}
        rectangles_[count_++]=r;return true;
    }
    void finish()noexcept {
        if(!count_)return;
        ++groups;unionArea+=std::uint64_t(bounds_.width)*bounds_.height;
        maxGroup=std::max(maxGroup,count_);count_=0;bounds_={};
    }
private:
    std::array<OriginalUiCoverageBounds50KG,cap> rectangles_{};
    OriginalUiCoverageBounds50KG bounds_{};
    std::uint32_t count_=0;
};
}
