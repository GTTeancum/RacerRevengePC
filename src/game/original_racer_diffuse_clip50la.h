#pragma once
#include "original_ui_rider_visibility50kg.h"
#include "original_racer_model_view50la.h"
#include <cfenv>
namespace rr::game {
struct OriginalRacerDiffuseClip50LA {
    bool selected=false,disabled=false,guardChecked=false;
    std::uint8_t coreMask=0,guardMask=0,remainingMask=0;
    bool clip04=false;
};
// Numeric 240E6C..2410B8 null-observer branch only. Its typed model caller
// establishes the actual normal gameplay selection phase and current inputs.
inline bool projectOriginalRacerDiffuseClip50LA(const BodyMatrix48G& world,
    const OriginalRacerAggregateBounds50LA& bounds,const std::array<float,16>& combined,
    const std::array<float,16>& guard,OriginalRacerDiffuseClip50LA& out)noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    namespace n=original_racer_model_view_detail50LA;
    const auto product=[&](const std::array<float,16>& view,std::array<float,16>& result){
        for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
            float x=n::mul(view[c],world.v[4*r]);
            x=n::add(x,n::mul(view[4+c],world.v[4*r+1]));
            x=n::add(x,n::mul(view[8+c],world.v[4*r+2]));
            x=n::add(x,n::mul(view[12+c],world.v[4*r+3]));
            if(!std::isfinite(x))return false;result[4*r+c]=x;
        }return true;
    };
    OriginalRacerDiffuseClip50LA next;next.clip04=true;
    OriginalUiRiderBoxClip50KG clip;std::array<float,16> matrix{};
    if(!product(combined,matrix)||!projectOriginalUiRiderBoxClip50KG(matrix,bounds.center160,bounds.halfExtent170,clip))return false;
    next.selected=!clip.outside;next.coreMask=clip.unionMask;
    auto remaining=clip.unionMask;
    if(next.selected&&remaining){
        if(!product(guard,matrix)||!projectOriginalUiRiderBoxClip50KG(matrix,bounds.center160,bounds.halfExtent170,clip))return false;
        next.guardChecked=true;next.guardMask=clip.unionMask;remaining=clip.unionMask;
        // Source ignores the guard projection's outside return.
    }
    next.remainingMask=static_cast<std::uint8_t>(remaining&~0x10u);
    if(next.selected)next.clip04=next.remainingMask!=0;
    out=next;return true;
}
}
