#include "original_ui_garage_model_view50kg.h"
#include <limits>
namespace rr::game::garage_model_view50KG {
namespace {
namespace n=original_racer_model_view_detail50LA;
bool ready()noexcept{return sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&std::fegetround()==FE_TONEAREST;}
bool product(const BodyMatrix48G& world,const std::array<float,16>& camera,
    std::array<float,16>& output)noexcept {
    for(unsigned r=0;r<4;++r)for(unsigned c=0;c<4;++c){
        float value=n::mul(camera[c],world.v[4*r]);
        value=n::add(value,n::mul(camera[4+c],world.v[4*r+1]));
        value=n::add(value,n::mul(camera[8+c],world.v[4*r+2]));
        value=n::add(value,n::mul(camera[12+c],world.v[4*r+3]));
        if(!std::isfinite(value))return false;
        output[4*r+c]=value;
    }
    return true;
}
float divide(float x,float y)noexcept{volatile float z=x/y;return z;}
}
bool select240C00(const BodyMatrix48G& world,const OriginalRacerAggregateBounds50LA& bounds,
    const std::array<float,16>& observer,const std::array<float,16>& combined,
    const std::array<float,16>& guard,const OriginalUiRiderVisibilityPolicy50KG& policy,
    std::uint8_t flags03,std::uint8_t flags191,std::uint8_t flags1B2,Selection& output)noexcept {
    if(!ready())return false;
    Selection next;next.flags191=flags191;next.flags1B2=flags1B2;
    if(flags03&4u){
        //26EFCC..26EFFC returns without calling240C00 or26F070.
        next.disabled=true;next.flags191=static_cast<std::uint8_t>(flags191&~1u);
        output=next;return true;
    }
    next.flags1B2=static_cast<std::uint8_t>((flags1B2|4u)&~0x88u);
    std::array<float,16> matrix{};OriginalUiRiderBoxClip50KG clip;
    //The genuine flat observer has one linked child04 and no next00. A caller
    //must not select this branch merely because membership happens to be absent.
    if(!product(world,policy.useMembership3833D8?observer:combined,matrix)||
       !projectOriginalUiRiderBoxClip50KG(matrix,bounds.center160,bounds.halfExtent170,clip))return false;
    next.selected=!clip.outside;next.coreMask=clip.unionMask;
    std::uint8_t remaining=clip.unionMask;
    if(next.selected){
        if(!remaining)next.flags1B2|=8u; //240F40..58.
        else {
            if(!product(world,guard,matrix)||
               !projectOriginalUiRiderBoxClip50KG(matrix,bounds.center160,bounds.halfExtent170,clip))return false;
            next.guardChecked=true;next.guardMask=clip.unionMask;remaining=clip.unionMask;
            //240FF4 ignores the second clip's outside result, consumes only mask.
            if(!remaining)next.flags1B2|=policy.guardInsideSets80_4354F0?0x80u:8u;
        }
    }
    next.remainingMask=static_cast<std::uint8_t>(remaining&~0x10u);
    if(next.selected)next.flags1B2=static_cast<std::uint8_t>((next.flags1B2&~5u)|(next.remainingMask?5u:0u));
    next.flags191=static_cast<std::uint8_t>((flags191&~1u)|(next.selected?1u:0u));
    output=next;return true;
}
bool selectLod26F070(const assets::Scene48D& scene,const LodInput& in,Lod& output)noexcept {
    if(!ready()||scene.lods.empty()||scene.lods.size()>256||
       !std::isfinite(in.actualRadius)||in.actualRadius<0||
       !std::isfinite(in.actualFocal1D8)||in.actualFocal1D8<=0||
       !std::isfinite(in.actualGeometryScale14)||in.actualGeometryScale14<=0||
       !std::isfinite(in.actualLodScale383360)||in.actualLodScale383360<=0||
       !std::isfinite(in.actualRadiusScale383368)||in.actualRadiusScale383368<=0)return false;
    Lod next;
    if(!projectOriginalRacerDistance26F070(in.actualCenter,in.actualLocalF0,
        in.actualCamera180,next.distance1BC))return false;
    const float gap=n::sub(next.distance1BC,in.actualRadius);
    if(!std::isfinite(gap))return false;
    const float denominator=gap<0.1f?0.1f:gap; //26F194 MAX.S(0x3DCCCCCD,gap).
    const float scaledFocal=n::mul(in.actualRadiusScale383368,in.actualFocal1D8);
    const float radius=n::mul(in.actualRadius,scaledFocal);
    const float diameter=n::mul(2.f,radius);
    next.scaleF20=divide(in.actualFocal1D8,denominator);
    next.projectedRadiusF21=divide(diameter,denominator);
    const float lodScale=n::mul(next.scaleF20,in.actualLodScale383360);
    if(!std::isfinite(scaledFocal)||!std::isfinite(radius)||!std::isfinite(diameter)||
       !std::isfinite(next.scaleF20)||!std::isfinite(next.projectedRadiusF21)||!std::isfinite(lodScale))return false;
    for(std::size_t i=0;i+1<scene.lods.size();++i){
        const auto stored=scene.lods[i].cutoff;
        const float cutoff=static_cast<float>(stored);
        if(!std::isfinite(cutoff)||static_cast<double>(cutoff)!=stored)return false;
        const float pixels=n::mul(lodScale,cutoff);
        const float scaled=n::mul(in.actualGeometryScale14,pixels);
        if(!std::isfinite(pixels)||!std::isfinite(scaled))return false;
        if(scaled<1.f)next.selected1B0=static_cast<std::uint8_t>(i+1u); //26F228 C.OLT.S.
    }
    output=next;return true;
}
}
