#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_position50kg.h"
#include <cmath>

namespace rr::game {
namespace {
bool bounded(const assets::OriginalUiIconRect50KG& r) noexcept {
    for(auto v:{r.left,r.top,r.right,r.bottom})if(v<-32768||v>32767)return false;
    return true;
}
float multiply(float a,float b) noexcept{volatile float v=a*b;return v;}
float add(float a,float b) noexcept{volatile float v=a+b;return v;}
}
OriginalUiIconLayoutStatus50KG layoutOriginalUiIcon50KG(
    const assets::OriginalUiIcon50KG& icon,const assets::Texture48B& atlas,
    std::int32_t videoDepthMaximum,OriginalUiIconLayout50KG& output) noexcept {
    using S=OriginalUiIconLayoutStatus50KG;
    if(!icon.identity||icon.materialName.empty()||icon.alpha<-32768||icon.alpha>32767)return S::missingOwner;
    if(!atlas.width||!atlas.height||atlas.width>16384||atlas.height>16384)return S::atlas;
    if(!std::isfinite(icon.angle)||!std::isfinite(icon.depth))return S::arithmetic;
    // Source2ACB94 only installs rotation when angle is strictly positive.
    if(icon.angle>0)return S::needsRotation;
    if(videoDepthMaximum<300)return S::depth;
    OriginalUiIconLayout50KG next;
    const assets::OriginalUiIconRect50KG full{0,0,std::int32_t(atlas.width),std::int32_t(atlas.height)};
    next.partialRect=icon.hasPartialRect?icon.partialRect:full;
    if(!bounded(next.partialRect))return S::rectangle;
    if(icon.hasMapRect)next.mapRect=icon.mapRect;
    else {
        if(icon.x<-32768||icon.x>32767||icon.y<-32768||icon.y>32767)return S::rectangle;
        // Bounded operands make the source signed subtract/add well-defined.
        next.mapRect={icon.x,icon.y,icon.x+(next.partialRect.right-next.partialRect.left),
            icon.y+(next.partialRect.bottom-next.partialRect.top)};
    }
    if(!bounded(next.mapRect))return S::rectangle;
    auto& q=next.quad;q.materialName=icon.materialName.c_str();q.sourcePC=0x2df80c;
    q.authoredDepth=icon.depth<0?float(videoDepthMaximum-300):icon.depth;
    next.packetDepth=q.authoredDepth<0?-q.authoredDepth:add(float(videoDepthMaximum),-q.authoredDepth);
    q.sourcePacketColor=originalUiTextPacketColor50KG(icon.color,static_cast<std::int16_t>(icon.alpha));
    const float x=float(next.mapRect.left),y=float(next.mapRect.top);
    const float right=add(x,multiply(float(next.mapRect.right-next.mapRect.left),1));
    const float bottom=add(y,multiply(float(next.mapRect.bottom-next.mapRect.top),1));
    float u0=0,v0=0,u1=1,v1=1;
    if(icon.hasPartialRect){const float us=1.f/float(atlas.width),vs=1.f/float(atlas.height);
        u0=multiply(float(next.partialRect.left),us);u1=multiply(float(next.partialRect.right),us);
        v0=multiply(float(next.partialRect.top),vs);v1=multiply(float(next.partialRect.bottom),vs);}
    q.vertices={{{x,y,u0,v0},{right,y,u1,v0},{x,bottom,u0,v1},{right,bottom,u1,v1}}};
    if(!std::isfinite(next.packetDepth))return S::arithmetic;
    output=next;return S::complete;
}
}
