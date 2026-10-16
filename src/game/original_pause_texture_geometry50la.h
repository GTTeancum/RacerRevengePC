#pragma once
#include "original_pause_storage50la.h"
#include <cmath>

namespace rr::game {
// CPU packet projection only. Object0..2: source+A0,+100,+40;3:icon890;
//4/5:arrows210/270. Submission order/visibility remain original callback work.
class OriginalPauseTextureGeometry50LA final {
    using Storage=OriginalPauseStorage50LA;
public:
    struct Diagnostic {std::uint32_t sourcePC=0;std::size_t object=0;};
    struct Projection {
        // Actual source triangle-strip order TL,TR,BL,BR. A renderer using
        // triangles0,1,2/0,2,3 must reorder1,0,2,3; no UV/alpha conversion here.
        Storage::Packet packet;
        std::size_t reference=0;
    };
    static bool regenerate2DF550(Storage& owner,std::size_t object,
        Projection& out,Diagnostic& d)noexcept {
        d={0x2DF550,object};
        if(owner.stage_!=Storage::Stage::derivedResources||!owner.context_||
           !owner.context_->live()||object>=6)return false;
        auto& t=object<3?owner.backdrops_[object]:object==3?owner.icon_:owner.arrows_[object-4];
        constexpr std::size_t references[]{0,1,2,7,34,35};
        const auto reference=references[object];
        const auto* retained=owner.references_.reference(reference);
        if(!retained||!retained->retained||!t.primitive54||!t.buffer58||!t.atlas||
           t.primitive54->reference!=reference||t.primitive54->packet10!=t.buffer58.get()||
           t.field3C!=0||(t.flags5C&2u)||t.field30!=1.f||t.field40!=1.f||
           !std::isfinite(t.x10)||!std::isfinite(t.y14)||
           !std::isfinite(t.width34)||!std::isfinite(t.height38))return false;
        Projection next;next.reference=reference;next.packet=*t.buffer58;
        if(t.flags5C&1u){
            //2DF694..6C0 multiplies dimensions before adding position. The
            //current original Pause route admits scale1 and no rotation only.
            volatile float width=t.width34*t.field30,height=t.height38*t.field30;
            const float right=t.x10+width,bottom=t.y14+height;
            if(!std::isfinite(right)||!std::isfinite(bottom))return false;
            for(std::size_t v=0;v<4;++v){
                next.packet.vertices[v].xyz[0]=(v&1)?right:t.x10;
                next.packet.vertices[v].xyz[1]=(v&2)?bottom:t.y14;
            }
        }
        // Preserve source depth, W, RGBA and all STQ lanes verbatim. No
        // synthetic decoded atlas, host alpha normalization or readiness bit.
        for(const auto& v:next.packet.vertices){
            for(float f:v.xyz)if(!std::isfinite(f))return false;
            for(float f:v.stq)if(!std::isfinite(f))return false;
            if(v.xyz[2]!=16777214.f)return false;
        }
        *t.buffer58=next.packet;t.flags5C=std::uint8_t(t.flags5C&~1u);
        out=next;d.sourcePC=0;return true;
    }
};
}
