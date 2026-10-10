#include "original_ui_screen_canvas50kg.h"

namespace rr::platform {
bool appendOriginalUiOverlay50KG(OriginalUiCanvas50KG& canvas,
    const game::OriginalUiOverlayQuad50KG& source,const OriginalUiResolvedDraw50KG& material) noexcept {
    if(!source.materialName||!material.materialName||std::strcmp(source.materialName,material.materialName))return false;
    constexpr unsigned order[]={0,1,3,2};std::array<assets::Vertex48B,4> vertices{};
    for(unsigned i=0;i<4;++i){const auto& v=source.vertices[order[i]];
        vertices[i].position[0]=v.x;vertices[i].position[1]=v.y;vertices[i].position[2]=0;
        vertices[i].uv[0]=v.u;vertices[i].uv[1]=v.v;
        for(unsigned c=0;c<4;++c)vertices[i].color[c]=source.sourcePacketColor[c];}
    return canvas.appendSprite(vertices,material.textureIndex,material.blend,material.alphaScale,material.colorScale);
}
bool appendOriginalUiTextLine50KG(OriginalUiCanvas50KG& canvas,
    const game::OriginalUiPositionedLine50KG& line,std::uint32_t currentPackedColor,
    std::int16_t currentAlpha,const OriginalUiResolvedDraw50KG& material) noexcept {
    return canvas.appendText(line.glyphs,material.textureIndex,{},
        game::originalUiTextPacketColor50KG(currentPackedColor,currentAlpha),
        material.blend,material.alphaScale,material.colorScale);
}
}
