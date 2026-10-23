#pragma once
#include "../../../src/game/original_ui_garage_prelit50kg.h"
#include "../../../src/assets/native_asset48b.h"
#include <array>
#include <cmath>
#include <cstdint>

namespace rr::platform {
// Native transport of ONE original339600/366430 polygon, not a model,
// material, collector, image-publication or complete-frame capability.
// A complete composer must still retain every original model/surface/pass,
// originating parameters and effective material history, and account these
// slots against the existing whole-frame capacity. No program or pass is
// inferred from the mere presence of stored colors.
struct OriginalUiGaragePrelitPacket50KG {
    // The existing source clipper bounds nine polygon vertices/seven fan
    // triangles. Each native four-slot unit encodes one genuine triangle
    // [a,b,c,c]; the second indexed triangle is exactly degenerate.
    std::array<assets::Vertex48B,28> vertices{};
    std::array<std::array<float,3>,28> stq{};
    std::array<float,28> rasterW{},fog{};
    std::uint32_t quadCount=0;
};
// Source sourceScreen140 for the sealed640x448 UI target incorporates the
// GS2048 origin. XYOFFSET is(2048-320,2048-224), not an imported canvas pose.
// This function consumes actual already-rounded packet positions/STQ/RGBA/F.
// It NEVER reconstructs Q from W, converts raw STQ to semantic UV, relights
// stored RGB, clamps the primitive to the canvas, or silently omits failures.
// Numeric input alone grants no Garage owner, submission or history authority.
inline game::garage_prelit50KG::Status lowerOriginalUiGaragePrelitPacket50KG(
    const game::garage_prelit50KG::Polygon& polygon,
    OriginalUiGaragePrelitPacket50KG& output) noexcept {
    using S=game::garage_prelit50KG::Status;
    if(polygon.count>polygon.vertices.size())return S::capacity;
    if(polygon.count && (polygon.count<3 || (!polygon.sourceTriangleFan && polygon.count!=3)))
        return S::input;
    OriginalUiGaragePrelitPacket50KG next;
    // count0 is the original clipper's empty output, not permission to drop
    // an unsupported or failed projection. Caller must first accept its status.
    if(!polygon.count){output=next;return S::complete;}
    for(std::uint32_t i=0;i<polygon.count;++i){
        const auto& v=polygon.vertices[i];
        for(unsigned k=0;k<3;++k)
            if(!std::isfinite(v.gsPosition[k]) || !std::isfinite(v.stq[k]))
                return S::unsupportedPacketRange;
        if(v.gsPosition[0]<0.f || v.gsPosition[0]>4095.f ||
           v.gsPosition[1]<0.f || v.gsPosition[1]>4095.f ||
           std::trunc(v.gsPosition[0]*16.f)!=v.gsPosition[0]*16.f ||
           std::trunc(v.gsPosition[1]*16.f)!=v.gsPosition[1]*16.f ||
           v.gsPosition[2]<0.f || v.gsPosition[2]>16777215.f ||
           std::trunc(v.gsPosition[2])!=v.gsPosition[2] || v.rgba[3]>128 ||
           !std::isfinite(v.rasterW) || v.rasterW<=0.f || v.stq[2]<=0.f ||
           !std::isfinite(v.stq[0]/v.stq[2]) || !std::isfinite(v.stq[1]/v.stq[2]))
            return S::unsupportedPacketRange;
    }
    const auto copy=[&](std::uint32_t source,std::uint32_t slot) noexcept {
        const auto& v=polygon.vertices[source];auto& n=next.vertices[slot];
        n.position[0]=v.gsPosition[0]-1728.f;
        n.position[1]=v.gsPosition[1]-1824.f;
        n.position[2]=v.gsPosition[2];
        n.uv[0]=n.uv[1]=0.f; // Not sampled by sourcePacketStq mode.
        for(unsigned c=0;c<4;++c)n.color[c]=v.rgba[c];
        next.stq[slot]=v.stq;next.rasterW[slot]=v.rasterW;
        next.fog[slot]=static_cast<float>(v.fogByte)/256.f;
    };
    for(std::uint32_t i=1;i+1<polygon.count;++i){
        const auto slot=next.quadCount*4u;
        copy(0,slot);copy(i,slot+1);copy(i+1,slot+2);copy(i+1,slot+3);
        ++next.quadCount;
    }
    output=next;return S::complete;
}
}
