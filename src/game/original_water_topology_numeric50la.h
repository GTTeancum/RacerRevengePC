#pragma once
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Water topology numeric adaptation requires strict binary32 arithmetic"
#endif
namespace rr::game::water_topology_numeric50LA {
using Vec3=std::array<float,3>;
struct Vertex {Vec3 position;std::array<float,2> uv;};
//Failure PC identifies the arithmetic group, not a replayed instruction stop.
struct Result {bool prepared=false;std::uint32_t pc=0x299420u;};
namespace detail {
struct Math {
    std::uint32_t bad=0;
    float keep(float v,std::uint32_t pc)noexcept {
        if(!bad&&(!std::isfinite(v)||(v!=0.f&&std::fpclassify(v)==FP_SUBNORMAL)))bad=pc;
        return v;
    }
    float add(float a,float b,std::uint32_t pc)noexcept {keep(a,pc);keep(b,pc);volatile float v=a+b;return keep(v,pc);}
    float sub(float a,float b,std::uint32_t pc)noexcept {keep(a,pc);keep(b,pc);volatile float v=a-b;return keep(v,pc);}
    float mul(float a,float b,std::uint32_t pc)noexcept {keep(a,pc);keep(b,pc);volatile float v=a*b;return keep(v,pc);}
    float div(float a,float b,std::uint32_t pc)noexcept {
        keep(a,pc);keep(b,pc);if(b==0.f){if(!bad)bad=pc;return 0.f;}
        volatile float v=a/b;return keep(v,pc);
    }
    Vec3 cross(const Vec3& a,const Vec3& b,std::uint32_t pc)noexcept {
        //VOPMULA first, then separately rounded VOPMSUB products/subtractions.
        Vec3 acc{mul(a[1],b[2],pc),mul(a[2],b[0],pc),mul(a[0],b[1],pc)};
        for(unsigned i=0;i<3;++i)acc[i]=sub(acc[i],mul(b[(i+1)%3],a[(i+2)%3],pc+4u),pc+4u);
        return acc;
    }
    Vec3 normalize(const Vec3& v,std::uint32_t pc)noexcept {
        const Vec3 square{mul(v[0],v[0],pc),mul(v[1],v[1],pc),mul(v[2],v[2],pc)};
        const float sum=add(add(square[0],square[1],pc+8u),mul(1.f,square[2],pc+12u),pc+12u);
        //Same bounded native convention as the recovered camera oracle:
        //VRSQRT of zero produces finite maximum Q, so zero XYZ remains zero.
        float q=std::numeric_limits<float>::max();
        if(sum!=0.f){volatile float root=std::sqrt(sum);q=div(1.f,keep(root,pc+16u),pc+16u);}
        return {mul(v[0],q,pc+28u),mul(v[1],q,pc+28u),mul(v[2],q,pc+28u)};
    }
};
}
// Native preparation of299420 XYZ outputs for a NEW zero-filled output array.
// Not a loader/allocator return, full vector/W write, or PS2 FPU/VU flag model.
// Caller must authenticate actual32-byte record XYZ/UV and16-byte input XYZ;
// unknown W is neither inspected nor invented here. Scalar operations round
// separately. FE_TONEAREST and observable finite-normal domains are required;
// this is not a proof of silicon, FTZ/DAZ, subnormal or exceptional equivalence.
// Failure leaves caller output unchanged. Such transactional preparation is
// not a receipt for the original routine's partial stores on failure.
inline Result prepare(std::span<const Vertex> vertices,std::span<const Vec3> input,
                      std::span<Vec3> output)noexcept {
    static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559);
    Result result;const auto n=vertices.size();
    if(n<2u||n>16384u||input.size()!=n||output.size()!=n||std::fegetround()!=FE_TONEAREST)return result;
    std::vector<Vec3> pending;
    try {pending.resize(n);}catch(...){return result;}
    detail::Math m;
    for(std::size_t i=0;i+2u<n;++i){
        const auto& a=vertices[i];const auto& b=vertices[i+1u];const auto& c=vertices[i+2u];
        Vec3 ab{},ac{},tangent{};
        for(unsigned k=0;k<3;++k){ab[k]=m.sub(b.position[k],a.position[k],0x2994ecu);ac[k]=m.sub(c.position[k],a.position[k],0x299514u);}
        const float bv=m.sub(b.uv[1],a.uv[1],0x299520u),cv=m.sub(c.uv[1],a.uv[1],0x29952cu);
        const float bu=m.sub(b.uv[0],a.uv[0],0x299544u),cu=m.sub(c.uv[0],a.uv[0],0x299524u);
        bool flip=false;
        if(cv!=0.f){
            const float ratio=m.div(-bv,cv,0x299550u);
            flip=m.add(bu,m.mul(ratio,cu,0x299554u),0x299558u)<0.f;
            for(unsigned k=0;k<3;++k)tangent[k]=m.add(ab[k],m.mul(ratio,ac[k],0x299560u),0x299564u);
        }else {
            const float ratio=m.div(-cv,bv,0x2995b8u);
            flip=m.add(cu,m.mul(ratio,bu,0x2995bcu),0x2995c0u)<0.f;
            for(unsigned k=0;k<3;++k)tangent[k]=m.add(ac[k],m.mul(ratio,ab[k],0x2995c8u),0x2995ccu);
        }
        if(flip)for(auto& lane:tangent)lane=-lane;
        tangent=m.normalize(tangent,0x29961cu);
        const auto orthogonal=m.normalize(m.cross(input[i],tangent,0x299650u),0x299660u);
        pending[i]=m.cross(orthogonal,input[i],0x299690u);
        if(m.bad){result.pc=m.bad;return result;}
    }
    //The two unconditional tails start with calloc-produced zero outputs.
    for(std::size_t i=n-2u;i<n;++i){
        const auto shift=i==n-2u?0u:0x60u;
        const auto orthogonal=m.normalize(m.cross(input[i],pending[i],0x2996d4u+shift),0x2996e4u+shift);
        pending[i]=m.cross(orthogonal,input[i],0x299714u+shift);
        if(m.bad){result.pc=m.bad;return result;}
    }
    for(std::size_t i=0;i<n;++i)output[i]=pending[i];
    result.prepared=true;result.pc=0x299780u;return result; //Before saved-register restoration.
}
} //namespace rr::game::water_topology_numeric50LA
