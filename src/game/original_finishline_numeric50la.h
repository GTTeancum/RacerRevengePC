#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Finishline numeric adaptation requires strict finite binary32 arithmetic"
#endif

namespace rr::game::finishline_numeric50LA {
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::radix==2&&std::numeric_limits<float>::digits==24,
    "Finishline numeric adaptation requires IEEE binary32");
using Vec3=std::array<float,3>;
struct Geometry {
    std::array<Vec3,4> positions{},normals{};
    std::array<std::array<float,2>,4> uv{};
};
namespace detail {
struct Math {
    std::uint32_t badPC=0;
    float keep(float value,std::uint32_t pc)noexcept {
        if(!badPC&&(!std::isfinite(value)||(value!=0.f&&std::fpclassify(value)==FP_SUBNORMAL)))badPC=pc;
        return value;
    }
    float add(float a,float b,std::uint32_t pc)noexcept {
        keep(a,pc);keep(b,pc);volatile float value=a+b;return keep(value,pc);
    }
    float sub(float a,float b,std::uint32_t pc)noexcept {
        keep(a,pc);keep(b,pc);volatile float value=a-b;return keep(value,pc);
    }
    float mul(float a,float b,std::uint32_t pc)noexcept {
        keep(a,pc);keep(b,pc);volatile float value=a*b;return keep(value,pc);
    }
    float reciprocalRoot(float square,std::uint32_t pc)noexcept {
        keep(square,pc);if(!(square>0.f)){if(!badPC)badPC=pc;return 0.f;}
        volatile float root=std::sqrt(square);keep(root,pc);
        volatile float inverse=1.f/root;return keep(inverse,pc);
    }
};
}
// Bounded native CPU adaptation of1B3DDC..1B4090. Caller must authenticate
// BOTH actual model28B1A0 XYZ outputs and the reached nonmissing-anchor branch
//1B3CE8..1B3DB8. This function confers no ownership/selector/strip readiness.
// Position/normal setters23C090/23C260 consume only XYZ: no invented W write.
// FE_TONEAREST admits the host implementation; original FCSR, EE ACC/RSQRT,
// VU cross rounding/flush behavior and universal retail parity remain unknown.
// Observable subnormal operands/results reject. FE_TONEAREST does NOT prove
// FTZ/DAZ control: caller's admitted actual range must not depend on subnormal
// preservation. This leaf does not establish universal underflow equivalence.
// No source callbacks, material work, draw, VU execution or flag writes occur.
// Transactional precomputation is native admission, NOT a receipt for original
// partial setter stores. Success guarantees every computed lane is finite and
// the normal is nonzero BEFORE any caller setter. Owner may use that explicitly
// native all-or-nothing preflight for the actual authenticated RA anchors. If
// it instead claims a reached source prefix on numeric failure, it MUST publish
// the actual four position stores BEFORE any fallible later normalization;
// this atomic helper cannot certify such partial-source progress. After success
// owner still commits to its actual strip in source order and retains any real
// earlier stores/resources if a separate owned setter/service subsequently fails.
inline bool prepare1B3DDC(const Vec3& actualLeft28B1A0,const Vec3& actualRight28B1A0,
    Geometry& output,std::uint32_t& pc)noexcept {
    pc=0x1b3ddc;
    if(std::fegetround()!=FE_TONEAREST)return false;
    detail::Math m;
    for(float x:actualLeft28B1A0)m.keep(x,pc);
    for(float x:actualRight28B1A0)m.keep(x,pc);
    if(m.badPC){pc=m.badPC;return false;}
    Geometry next;
    next.positions[0]=actualLeft28B1A0; //1B3DF0, group0 vertex0.
    next.positions[1]=actualLeft28B1A0;
    next.positions[1][1]=m.sub(next.positions[1][1],40.f,0x1b3e0c);
    next.positions[2]=actualRight28B1A0; //1B3E3C, group0 vertex2.
    next.positions[3]=actualRight28B1A0;
    next.positions[3][1]=m.sub(next.positions[3][1],40.f,0x1b3e58);
    //The stack40/50 operands are now the two LOWER vertices. Preserve the
    //source subtract/add chronology, including reconstruction of upper-left.
    const auto& left=next.positions[1];const auto& right=next.positions[3];
    Vec3 span{};
    span[2]=m.sub(right[2],left[2],0x1b3e90);
    span[1]=m.sub(right[1],left[1],0x1b3ea0);
    span[0]=m.sub(right[0],left[0],0x1b3eac);
    Vec3 above=left;above[1]=m.add(above[1],40.f,0x1b3ec4);
    Vec3 down{};
    down[0]=m.sub(left[0],above[0],0x1b3ee8);
    down[1]=m.sub(left[1],above[1],0x1b3eec);
    down[2]=m.sub(left[2],above[2],0x1b3ef4);
    Vec3 normal{};
    //13E454 VOPMULA.xyz then13E458 VOPMSUB.xyz, span cross down.
    //Separate host rounded products/subtractions are an explicit adaptation.
    for(unsigned k=0;k<3u;++k){const auto j=(k+1u)%3u,l=(k+2u)%3u;
        const float first=m.mul(span[j],down[l],0x13e454);
        const float second=m.mul(span[l],down[j],0x13e458);
        normal[k]=m.sub(first,second,0x13e458);
    }
    const float xx=m.mul(normal[0],normal[0],0x1b3f28);
    const float yy=m.mul(normal[1],normal[1],0x1b3f2c);
    const float xy=m.add(xx,yy,0x1b3f30); //Source ADDA.s ACC.
    const float zz=m.mul(normal[2],normal[2],0x1b3f34);
    const float square=m.add(xy,zz,0x1b3f34); //Source MADD.s result.
    if(m.badPC){pc=m.badPC;return false;}
    //Actual RA anchor pair is nondegenerate. A zero normal reaches a global
    //4342D0 copy1B3F48, whose currentness is deliberately not fabricated here.
    if(square==0.f){pc=0x1b3f48;return false;}
    const float inverse=m.reciprocalRoot(square,0x1b3f64);
    normal[0]=m.mul(normal[0],inverse,0x1b3f74);
    normal[1]=m.mul(normal[1],inverse,0x1b3f80);
    normal[2]=m.mul(normal[2],inverse,0x1b3f8c);
    if(m.badPC){pc=m.badPC;return false;}
    //Four +7C calls1B3FA8/FC4/FE0/FFC use this SAME vector, group0.
    for(auto& n:next.normals)n=normal;
    next.uv[0]={{0.f,0.f}};next.uv[1]={{0.f,1.f}};
    next.uv[2]={{1.f,0.f}};next.uv[3]={{1.f,1.f}};
    //Four +88 calls1B401C/4040/4064/4088, not a texture orientation guess.
    output=next;pc=0x1b4090;return true;
}
}
