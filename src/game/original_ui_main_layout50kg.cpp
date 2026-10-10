#include "original_ui_main_layout50kg.h"
#include <cmath>

namespace rr::game {
namespace {
OriginalUiOverlayQuad50KG quad(const char* material,float x,float y,float w,float h,
    float u0,float v0,float u1,float v1,float depth,std::uint32_t pc) noexcept {
    OriginalUiOverlayQuad50KG q;
    q.materialName=material;q.authoredDepth=depth;q.sourcePC=pc;
    q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};
    return q;
}
bool dimension(std::uint32_t x) noexcept { return x>0 && x<=16384; }
}
bool layoutOriginalUiMainBackground50KG(std::uint32_t width,std::uint32_t height,
    OriginalUiMainBackground50KG& output) noexcept {
    if (!dimension(width) || !dimension(height)) return false;
    // Match reciprocal-then-multiply rather than collapsing to one division.
    const float uScale=1.0f/static_cast<float>(width);
    const float vScale=1.0f/static_cast<float>(height);
    const float u=319.0f*uScale,v=223.0f*vScale;
    OriginalUiMainBackground50KG result;
    result.quadrants[0]=quad("mmbkg",0,0,320,224,0,0,u,v,15,0x1E5708);
    result.quadrants[1]=quad("mmbkg",320,0,320,224,u,0,0,v,15,0x1E57F8);
    result.quadrants[2]=quad("mmbkg",0,224,320,224,0,v,u,0,15,0x1E58F8);
    result.quadrants[3]=quad("mmbkg",320,224,320,224,u,v,0,0,15,0x1E5A00);
    output=result;return true;
}
bool layoutOriginalUiMainArrows50KG(const OriginalUiMainArrowInput50KG& in,
    OriginalUiMainArrows50KG& output) noexcept {
    if (in.selectedSlot<1 || in.selectedSlot>5 || !std::isfinite(in.textX) ||
        !std::isfinite(in.textY) || in.sourceMeasuredTextWidth<0 ||
        !dimension(in.rightWidth) || !dimension(in.rightHeight) ||
        !dimension(in.leftWidth) || !dimension(in.leftHeight)) return false;
    const float xRight=2.0f+(in.textX-static_cast<float>(in.rightWidth));
    const float xLeft=3.0f+(in.textX+static_cast<float>(in.sourceMeasuredTextWidth));
    const float y=in.textY-3.0f;
    OriginalUiMainArrows50KG result;
    result.textWidgetId=static_cast<std::uint16_t>(in.selectedSlot+4);
    result.beforeText=quad("arrwrght",xRight,y,static_cast<float>(in.rightWidth),
        static_cast<float>(in.rightHeight),0,0,1,1,1,0x1E6554);
    result.afterText=quad("arrwleft",xLeft,y,static_cast<float>(in.leftWidth),
        static_cast<float>(in.leftHeight),0,0,1,1,1,0x1E65EC);
    for (const auto* q:{&result.beforeText,&result.afterText})
        for (const auto& v:q->vertices)
            if (!std::isfinite(v.x) || !std::isfinite(v.y)) return false;
    output=result;return true;
}
}
