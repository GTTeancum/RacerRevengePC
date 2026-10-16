#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Original racer model view requires strict finite IEEE arithmetic"
#endif

namespace rr::game {
// The actual current multi-node49O result, retained with the same completed
// graphical model. These are source aggregate bounds, not the render AABB.
struct OriginalRacerAggregateBounds50LA {
    std::array<float,4> center160{},halfExtent170{};
    float radius198=0;
};

// Only the known241280 radial-fog/map2 subset0x0202. This is not a complete
// original packet flag word,240C00 clip state or owner1B3 bit08 completion.
struct OriginalRacerModelDistanceFlags50LA {
    float distance1BC=0;
    std::uint16_t flags20=0;
    bool secondEnabled()const noexcept{return !(flags20&0x0200u);}
};

// One actual launch-owned setting corresponding to1A6AA0/2410E0. Capture
// once from that session's launch subtype; draw consumers read the same value.
class OriginalRacerModelRenderGlobals50LA final {
public:
    explicit OriginalRacerModelRenderGlobals50LA(std::uint32_t launchSubtype11258)noexcept
        :secondMapDistance382FA0_(launchSubtype11258==3u?1000.f:130.f){}
    float secondMapDistance382FA0()const noexcept{return secondMapDistance382FA0_;}
private:
    float secondMapDistance382FA0_;
};

namespace original_racer_model_view_detail50LA {
inline float add(float a,float b)noexcept{volatile float value=a+b;return value;}
inline float sub(float a,float b)noexcept{volatile float value=a-b;return value;}
inline float mul(float a,float b)noexcept{volatile float value=a*b;return value;}
}

// Selected multi-node26F0DC..26F190 numeric leaf. The caller owns the actual
// model, its matching aggregate center160 and current camera publication.
// F0/F4/F8 is LOCAL translation, not an inferred world/physics position.
// Each multiply/add is separately rounded in source order. The center's W
// lane is not consumed. No source selection, bounds refresh, LOD callback or
// owner mutation is implied. Failure preserves output; no allocation/callback.
inline bool projectOriginalRacerDistance26F070(
    const std::array<float,4>& center160,
    const std::array<float,3>& localPosition,
    const std::array<float,3>& cameraPosition,float& output)noexcept {
    namespace detail=original_racer_model_view_detail50LA;
    if(sizeof(float)!=4||!std::numeric_limits<float>::is_iec559||
       std::fegetround()!=FE_TONEAREST)return false;
    std::array<float,3> displacement{};
    for(unsigned axis=0;axis<3;++axis){
        if(!std::isfinite(center160[axis])||!std::isfinite(localPosition[axis])||
           !std::isfinite(cameraPosition[axis]))return false;
        const float relative=detail::sub(center160[axis],cameraPosition[axis]);
        if(!std::isfinite(relative))return false;
        displacement[axis]=detail::add(relative,localPosition[axis]);
        if(!std::isfinite(displacement[axis]))return false;
    }
    const float x=detail::mul(displacement[0],displacement[0]);
    const float y=detail::mul(displacement[1],displacement[1]);
    if(!std::isfinite(x)||!std::isfinite(y))return false;
    const float xy=detail::add(x,y);
    if(!std::isfinite(xy))return false;
    const float z=detail::mul(displacement[2],displacement[2]);
    if(!std::isfinite(z))return false;
    const float xyz=detail::add(xy,z);
    if(!std::isfinite(xyz))return false;
    volatile float distance=static_cast<float>(std::sqrt(static_cast<double>(xyz)));
    if(!std::isfinite(distance))return false;
    output=distance;return true;
}
} // namespace rr::game
