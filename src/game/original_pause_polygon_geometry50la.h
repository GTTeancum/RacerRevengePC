#pragma once
#include "original_pause_storage50la.h"
#include "original_ui_text_position50kg.h"
#include <cmath>

namespace rr::game {
// Exact kind1/rotation0 portion of2E21D0 ->2E2DD0/2E2ED0.
class OriginalPausePolygonGeometry50LA final {
    using Storage=OriginalPauseStorage50LA;
public:
    struct Diagnostic {std::uint32_t sourcePC=0;std::size_t object=0;};
    struct Projection {Storage::Packet packet;std::size_t reference=0;};
    static bool regenerate2E21D0(Storage& owner,std::size_t object,
        std::uint32_t videoMaximum,Projection& out,Diagnostic& d)noexcept {
        d={0x2E2398,object};
        if(owner.stage_!=Storage::Stage::derivedResources||!owner.context_||!owner.context_->live()||
           object>=2||videoMaximum!=0xffffffu||!owner.polygons_[object])return false;
        auto& p=*owner.polygons_[object];
        if(p.type0C!=1||p.count04!=4||p.count08!=4||p.count34!=1||p.count48!=1||p.capacity4C!=1||
           p.scale10!=1.f||p.field14!=0.f||p.field18!=1.f||p.flags40!=3||
           !p.vertices00||!p.packets44||!p.primitives30||!p.primitives30[0]||
           p.primitives30[0]->reference!=3+object||p.primitives30[0]->packet10!=p.packets44.get())return false;
        auto packet=p.packets44[0];const float depth=static_cast<float>(videoMaximum)-p.field18;
        for(std::size_t i=0;i<4;++i){auto& v=p.vertices00[i];
            if(v.stq14!=&p.packets44[0].vertices[i].stq||v.rgba1C!=&p.packets44[0].vertices[i].rgba||
               v.xyz24!=&p.packets44[0].vertices[i].xyz)return false;
            if(p.dirty3C){
                std::int32_t x=0,y=0;
                if(!roundOriginalUiTextMetric50KG(v.authored00[0],p.scale10,x)||
                   !roundOriginalUiTextMetric50KG(v.authored00[1],p.scale10,y))return false;
                //2E3794/98 and2E37B4/B8;2E37C8;2E385C..3864.
                packet.vertices[i].stq[0]=static_cast<float>(x);
                packet.vertices[i].stq[1]=static_cast<float>(y);packet.vertices[i].stq[2]=depth;
                // Native TME0 consumes XYZ only. Preserve the existing fourth
                // lane; do not invent a value for unconsumed global scratch W.
            }
            for(std::size_t lane=0;lane<3;++lane)
                if(!std::isfinite(packet.vertices[i].stq[lane]))return false;
            if(packet.vertices[i].stq[2]!=depth)return false;
        }
        if(p.dirty3C){
            for(std::size_t i=0;i<4;++i)p.vertices00[i].authored00[2]=depth;
            p.packets44[0]=packet;p.dirty3C=0; //2E3898.
        }
        // Kind1 PRIM0xC is Gouraud triangle strip (2E2944..2978).
        // PacketXYZ is intentionally unchanged; native TME0 uses actual STQ.
        out.packet=packet;out.reference=3+object;d.sourcePC=0;return true;
    }
};
}
