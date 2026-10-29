#pragma once
#include "type2_resource49x.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {
class OriginalWreckQueries50LA;
// Tree-bearing source11ED60 /11F680 type2 branches only. References are to
// the actual retained payload and query lanes, authenticated by Queries.
// Native finite binary32, nearest rounding policy; NOT exact EE/VU arithmetic.
// Refusal retains all earlier source stores. No allocation or cache authority.
class OriginalWreckSourceType2Prepare50LA final {
    friend class OriginalWreckQueries50LA;
    static bool pass(bool second,const BodyMatrix48G& actualStackA0,
        const Type2Node49X& actualRoot,BodyMatrix48G& actualShape00,
        SpringVector4_48N& actualCenter60,float& actualRadius80,
        std::uint32_t& pc) noexcept {
        // Source eight iterations, each with two word stores. Byte copies do
        // not interpret NaN payloads or require an affine matrix.
        for(unsigned pair=0;pair<8;++pair){
            pc=second?0x11fa54u:0x11f138u;
            std::memcpy(&actualShape00.v[pair*2],&actualStackA0.v[pair*2],4);
            pc=second?0x11fa58u:0x11f13cu;
            std::memcpy(&actualShape00.v[pair*2+1],&actualStackA0.v[pair*2+1],4);
        }
        for(unsigned axis=0;axis<3;++axis){
            pc=(second?0x11fa7cu:0x11f160u)+axis*8u;
            std::memcpy(&actualCenter60.v[axis],&actualRoot.center[axis],4);
        }
        pc=second?0x11fa90u:0x11f174u;actualCenter60.v[3]=1.f;
        // The local center is not consumed yet: even a nonfinite center must
        // survive the subsequent radius store before transform refusal.
        pc=second?0x11fac4u:0x11f1a8u;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(actualRoot.half[0]))return false;
        volatile float xx=actualRoot.half[0]*actualRoot.half[0];
        if(!std::isfinite(xx))return false;
        pc=second?0x11fac8u:0x11f1acu;
        if(!std::isfinite(actualRoot.half[1]))return false;
        volatile float yy=actualRoot.half[1]*actualRoot.half[1];
        if(!std::isfinite(yy))return false;
        pc=second?0x11faccu:0x11f1b0u;
        volatile float xy=xx+yy;if(!std::isfinite(xy))return false;
        pc=second?0x11fad0u:0x11f1b4u;
        if(!std::isfinite(actualRoot.half[2]))return false;
        volatile float zz=actualRoot.half[2]*actualRoot.half[2];
        if(!std::isfinite(zz))return false;
        volatile float sum=xy+zz;if(!std::isfinite(sum))return false;
        pc=second?0x11fad4u:0x11f1b8u;
        volatile float radius=std::sqrt(static_cast<float>(sum));
        if(!std::isfinite(radius))return false;
        pc=second?0x11fae4u:0x11f1c8u;actualRadius80=radius;
        // VU computes XYZ before its single SQ; failed native arithmetic
        // therefore leaves query60 as the already stored LOCAL center.
        SpringVector4_48N transformed{{0.f,0.f,0.f,1.f}};
        for(unsigned axis=0;axis<3;++axis){
            pc=second?0x11fb00u:0x11f1e4u;
            const auto x=actualCenter60.v[0],y=actualCenter60.v[1],z=actualCenter60.v[2];
            const auto a=actualStackA0.v[axis],b=actualStackA0.v[4+axis];
            const auto c=actualStackA0.v[8+axis],d=actualStackA0.v[12+axis];
            if(!std::isfinite(x)||!std::isfinite(a))return false;
            volatile float ax=a*x;if(!std::isfinite(ax))return false;
            pc=second?0x11fb04u:0x11f1e8u;
            if(!std::isfinite(y)||!std::isfinite(b))return false;
            volatile float by=b*y;if(!std::isfinite(by))return false;
            volatile float ab=ax+by;if(!std::isfinite(ab))return false;
            pc=second?0x11fb08u:0x11f1ecu;
            if(!std::isfinite(z)||!std::isfinite(c))return false;
            volatile float cz=c*z;if(!std::isfinite(cz))return false;
            volatile float abc=ab+cz;if(!std::isfinite(abc))return false;
            pc=second?0x11fb0cu:0x11f1f0u;
            if(!std::isfinite(d))return false;
            volatile float value=abc+d;if(!std::isfinite(value))return false;
            transformed.v[axis]=value;
        }
        pc=second?0x11fb14u:0x11f1f4u;
        std::memcpy(&actualCenter60,&transformed,sizeof transformed);
        // First endpoint is its real nested11F680 call; second is return.
        pc=second?0x11fe28u:0x11f1fcu;return true;
    }
};
} // namespace rr::game
