#pragma once
#include "../../../src/assets/native_asset48b.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::platform {
struct OriginalUiCoverageBounds50KG {
    std::int32_t x=0,y=0,width=0,height=0;
    bool empty()const noexcept{return width==0||height==0;}
};
// Extreme positive W is still admitted by the source renderer, but its
// clip-coordinate multiplication/division may under/overflow. Restricting the
// work region then uses the full domain, without changing or dropping geometry.
inline bool originalUiCoverageRasterWBounded50KG(float w)noexcept {
    return std::isfinite(w)&&w>=0x1p-64f&&w<=0x1p64f;
}
// Original UI source raster is fixed640x448, with canvas Y downward. Restrict
// work, never geometry: include a one-pixel halo around the triangle AABB for
// native subpixel rounding and all four sample locations within each pixel.
// No area test: degenerate triangles retain conservative bounds. Extremely
// large finite coordinates use the whole domain instead of relying on clipped
// floating-point intersection arithmetic. This is not an AA/coverage fallback.
inline bool originalUiCoverageBounds50KG(const assets::Vertex48B& a,
    const assets::Vertex48B& b,const assets::Vertex48B& c,
    OriginalUiCoverageBounds50KG& output)noexcept {
    const assets::Vertex48B* vertices[]={&a,&b,&c};
    double loX=std::numeric_limits<double>::infinity(),loY=loX;
    double hiX=-loX,hiY=-loX;bool extreme=false;
    for(const auto* v:vertices){
        const double x=v->position[0],y=v->position[1];
        if(!std::isfinite(x)||!std::isfinite(y))return false;
        extreme=extreme||std::fabs(x)>1048576.||std::fabs(y)>1048576.;
        loX=std::min(loX,x);hiX=std::max(hiX,x);
        const double rasterY=448.-y;
        loY=std::min(loY,rasterY);hiY=std::max(hiY,rasterY);
    }
    OriginalUiCoverageBounds50KG next;
    if(extreme){next={0,0,640,448};output=next;return true;}
    // Clamp before integer conversion, including for fully offscreen inputs.
    const auto low=[](double v,double edge){return static_cast<std::int32_t>(std::floor(std::clamp(v-1.,0.,edge)));};
    const auto high=[](double v,double edge){return static_cast<std::int32_t>(std::ceil(std::clamp(v+1.,0.,edge)));};
    next.x=low(loX,640.);next.y=low(loY,448.);
    next.width=high(hiX,640.)-next.x;next.height=high(hiY,448.)-next.y;
    output=next;return true;
}
}
