#include "original_ui_options_menu_layout50kg.h"
#include <cmath>
namespace rr::game {
namespace {
bool dimension(std::uint32_t d) noexcept {return d && d<=16384;}
OriginalUiOverlayQuad50KG quad(const char* material,float x,float y,float w,float h,
    float u0,float v0,float u1,float v1,float depth,std::uint32_t pc) noexcept {
    OriginalUiOverlayQuad50KG q;q.materialName=material;q.authoredDepth=depth;q.sourcePC=pc;
    q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};return q;
}
}
std::uint16_t originalUiOptionsSelectedText50KG(std::uint16_t slot) noexcept {
    constexpr std::uint16_t ids[]={6,12,7,8,9,10};return slot&&slot<=6?ids[slot-1]:0;
}
bool layoutOriginalUiOptionsBackground50KG(std::uint32_t width,std::uint32_t height,
    OriginalUiOptionsBackground50KG& output) noexcept {
    if(!dimension(width)||!dimension(height))return false;
    const float reciprocalU=1.f/static_cast<float>(width),reciprocalV=1.f/static_cast<float>(height);
    const float u=319.f*reciprocalU,v=223.f*reciprocalV;
    OriginalUiOptionsBackground50KG b;
    // One loop calls2DF550 on page+4D0,+530,+590,+5F0 in this exact order.
    b.quadrants[0]=quad("mmbkg",0,0,320,224,0,0,u,v,15,0x1F9ABC);
    b.quadrants[1]=quad("mmbkg",320,0,320,224,u,0,0,v,15,0x1F9ABC);
    b.quadrants[2]=quad("mmbkg",0,224,320,224,0,v,u,0,15,0x1F9ABC);
    b.quadrants[3]=quad("mmbkg",320,224,320,224,u,v,0,0,15,0x1F9ABC);
    output=b;return true;
}
bool layoutOriginalUiOptionsSelection50KG(const OriginalUiOptionsSelectionInput50KG& in,
    OriginalUiOptionsSelection50KG& output) noexcept {
    const auto widget=originalUiOptionsSelectedText50KG(in.selectedSlot);
    if(!widget||!std::isfinite(in.textX)||!std::isfinite(in.textY)||in.sourceMeasuredTextWidth<0||
        !dimension(in.rightWidth)||!dimension(in.rightHeight)||!dimension(in.leftWidth)||!dimension(in.leftHeight))return false;
    const float before=2.f+(in.textX-static_cast<float>(in.rightWidth));
    const float after=3.f+(in.textX+static_cast<float>(in.sourceMeasuredTextWidth));
    const float y=in.textY-3.f;OriginalUiOptionsSelection50KG q;q.textWidgetId=widget;
    q.beforeText=quad("arrwrght",before,y,static_cast<float>(in.rightWidth),static_cast<float>(in.rightHeight),0,0,1,1,1,0x1F9C48);
    q.afterText=quad("arrwleft",after,y,static_cast<float>(in.leftWidth),static_cast<float>(in.leftHeight),0,0,1,1,1,0x1F9CE4);
    for(const auto* part:{&q.beforeText,&q.afterText})for(const auto& v:part->vertices)
        if(!std::isfinite(v.x)||!std::isfinite(v.y))return false;
    output=q;return true;
}
}
