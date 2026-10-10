#pragma once
#include "water_height50jc.h"
#include <limits>

namespace rr::game {
// Source-backed scalar pieces of2B1690 and2B1760, including a bounded
// native120x120 /88x88 integer lattices derived from2B1BB0..2B1D0C.
// Proposals never mutate canonical buffers, selectors, origins or shared time.
// Caller precondition for every API below: writable proposal/workspace/scratch
// storage is disjoint from ALL borrowed canonical owners, scalar fields and
// coefficient arrays. Partial runtime overlap guards do not relax this rule.
enum class WaterWakeStatus50JC : std::uint8_t {
    Complete,Paused,LatticeComputed,ForceProjected,NeedsBaseChildren141ED0,NeedsLatticeKernel,
    Owner,Arithmetic,Storage,Unsupported
};
struct WaterWakeResult50JC {
    WaterWakeStatus50JC status=WaterWakeStatus50JC::Owner;
    std::uint32_t sourcePC=0;
    std::size_t bucket=0;
};
struct WaterWakeBucket50JC {
    const float* cellSize00=nullptr;
    const float* inverseCellSize04=nullptr;
    const float* origin10=nullptr; // Four original vector words.
    const float* anchor20=nullptr; // Four original vector words.
    const std::int32_t* activeBuffer30=nullptr;
    WaterBytes50JC signedBuffers38[2]{};
    WaterBytes50JC texture34{}; // Actual writable native virtual+44 view owner.
    bool sourceBound=false;
};
struct WaterWakeProvider50JC {
    const WaterWakeBucket50JC* buckets40=nullptr;
    std::int32_t count4C=0,dimensionB0=0,blocksPerRowB4=0;
    const float* halfDimensionB8=nullptr;
    // Exact inclusive source pointer range+44..+48, projected to array indices.
    std::size_t first44=0,last48=0;
    bool selectedRangeBound=false;
    bool sourceBound=false;
};
struct WaterWakeGlobals50JC {
    const std::uint32_t* application11C=nullptr;
    const std::uint32_t* wakeEnabled383418=nullptr;
    const std::uint8_t* lazy8D38=nullptr;
    const std::int32_t* sentinel443BB8=nullptr;
    const float* time443BB0=nullptr;
    const float* swellPeriod443E88=nullptr;
    const float* swellHeight443E80=nullptr;
    const float* baseHeight443E90=nullptr;
    const float* anchorDirection443BA0=nullptr; // XYZ at+0/+4/+8.
    const float* rebaseDelta443B40=nullptr; // Four real source vector words.
};
// Explicit native presentation policy, independent of human count/vehicle58.
// The adapter binds the actual camera-followed pod's canonical scene position.
// This record does not manufacture a human, change the racer roster, or claim
// the original game admitted a different player count.
struct WaterPresentationView50JC {
    const float* followedPodScenePosition=nullptr;
    std::uint32_t followedPodIndex=0;
    bool cameraFollowedPodBound=false;
    bool nativeOneViewPolicyAdmitted=false;
};
struct WaterWakeAnchorProposal50JC {
    bool writesAnchor=false;
    std::size_t bucket=0;
    float anchor20[4]{};
};
struct WaterWakeUpdateProposal50JC {
    bool writesLazySentinel=false;
    std::uint8_t lazy8D38=0;
    std::int32_t sentinel443BB8=0;
    bool writesSwell=false;
    float time443BB0=0,baseHeight3C=0;
    std::size_t first44=0,last48=0;
};
struct WaterWakeBucketProposal50JC {
    std::size_t bucket=0;
    std::int32_t sourceBuffer=0,destinationBuffer=0;
    bool rebasesOriginBeforeKernel=false;
    float originBeforeKernel10[4]{},originAfterKernel10[4]{};
    std::int32_t xBlockShift=0,zCellShift=0;
    // Native byte offsets replace source's0x30000000 uncached pointer alias.
    // The original views/bytes and write order still need a real kernel owner.
    std::uint64_t signedBufferBytes=0;
    std::uint64_t firstDestination=0,destinationLimit=0;
    std::uint64_t firstSource=0,firstRowLimit=0,rowStride=0;
    std::uint64_t textureFirst=0x1220u,textureRowStride=0x200u;
};
namespace water_wake_detail50jc {
using Status=WaterWakeStatus50JC;
inline WaterWakeResult50JC result(Status s,std::uint32_t pc,std::size_t i=0) noexcept {
    return {s,pc,i};
}
struct Math : water_height_detail50jc::Math {
    float div(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);
        if(y==0.f){if(!badPC)badPC=pc;return 0.f;}
        volatile float z=x/y;return read(z,pc);
    }
};
inline float bits(std::uint32_t b) noexcept {
    float x=0;std::memcpy(&x,&b,sizeof(x));return x;
}
inline WaterWakeResult50JC arithmetic(const Math& m) noexcept {
    return result(Status::Arithmetic,m.badPC);
}
inline std::int32_t arithmeticShift3(std::int32_t v) noexcept {
    // Portable signed SRA3, including negative values truncated by CVT.W.S.
    const std::int64_t w=v;
    return static_cast<std::int32_t>(w>=0?w/8:-((-w+7)/8));
}
inline std::int32_t direction(std::int32_t v) noexcept {return v<0?-1:v>0?1:0;}
inline float sourceSine13BAD0(float x,Math& m) noexcept {
    x=m.read(x,0x13bae0u);
    if(x>bits(0x3fc90fdbu))x=m.sub(bits(0x40490fdbu),x,0x13bb00u);
    else if(x<bits(0xbfc90fdbu))x=m.sub(bits(0xc0490fdbu),x,0x13bb34u);
    const float square=m.mul(x,x,0x13bb4cu);
    float a=m.mul(bits(0x3638ef1du),square,0x13bb5cu);
    a=m.add(bits(0xb9500d01u),a,0x13bb6cu);
    a=m.mul(square,a,0x13bb70u);
    a=m.add(bits(0x3c088889u),a,0x13bb80u);
    a=m.mul(square,a,0x13bb84u);
    a=m.add(bits(0xbe2aaaabu),a,0x13bb88u);
    const float cube=m.mul(x,square,0x13bb8cu);
    return m.add(x,m.mul(cube,a,0x13bb90u),0x13bb98u);
}
}
// Actual2B1690. Invalid bucket index is the source's no-write return. On any
// failure the proposal is unchanged. The caller owns any eventual+20 commit.
inline WaterWakeResult50JC projectWaterWakeAnchor50JC(
    const WaterWakeProvider50JC& p,const WaterWakeGlobals50JC& g,
    std::int32_t index,const float (&viewPoint)[3],WaterWakeAnchorProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    if(!p.sourceBound||p.count4C<0)return result(Status::Owner,0x2b16a8u);
    WaterWakeAnchorProposal50JC next;
    if(index<0||index>=p.count4C){out=next;return result(Status::Complete,0x2b1750u);}
    if(std::fegetround()!=FE_TONEAREST)return result(Status::Arithmetic,0x2b1690u);
    if(!p.buckets40||!p.halfDimensionB8||!g.anchorDirection443BA0)
        return result(Status::Owner,0x2b16c8u);
    const auto& b=p.buckets40[index];
    if(!b.sourceBound||!b.cellSize00)return result(Status::Owner,0x2b16e4u,std::size_t(index));
    Math m;
    const float halfWidth=m.mul(*p.halfDimensionB8,*b.cellSize00,0x2b16f4u);
    constexpr std::uint32_t multiplyPC[3]={0x2b16f8u,0x2b16fcu,0x2b1704u};
    constexpr std::uint32_t subtractPC[3]={0x2b1724u,0x2b1728u,0x2b1730u};
    for(std::size_t c=0;c<3;++c)
        next.anchor20[c]=m.sub(viewPoint[c],m.mul(halfWidth,g.anchorDirection443BA0[c],multiplyPC[c]),subtractPC[c]);
    if(m.badPC)return arithmetic(m);
    next.anchor20[3]=1.f;next.bucket=std::size_t(index);next.writesAnchor=true;
    out=next;return result(Status::Complete,0x2b1744u,next.bucket);
}
// Authorized native wrapper: one presentation view uses bucket0 and the
// genuine2B1690 math. It never derives view count from a human/player count.
inline WaterWakeResult50JC projectPresentationWaterAnchor50JC(
    const WaterWakeProvider50JC& p,const WaterWakeGlobals50JC& g,
    const WaterPresentationView50JC& view,WaterWakeAnchorProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    if(!view.nativeOneViewPolicyAdmitted||!view.cameraFollowedPodBound||
        !view.followedPodScenePosition||p.count4C<1)
        return result(Status::Owner,0x2b1690u);
    const float point[3]={view.followedPodScenePosition[0],view.followedPodScenePosition[1],view.followedPodScenePosition[2]};
    return projectWaterWakeAnchor50JC(p,g,0,point,out);
}
// Source2B1760 gate/swell prefix. NeedsLatticeKernel is an incomplete original
// update, even when this scalar proposal is available. Do not commit the clock
// or base height as if the original whole update had completed. Delta must be
// the actual virtual+38 argument, not a wall clock or a fixed substitute.
inline WaterWakeResult50JC projectWaterWakeUpdate50JC(
    const WaterWakeProvider50JC& p,const WaterWakeGlobals50JC& g,
    float delta,bool sourceVirtual38DeltaBound,WaterWakeUpdateProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    if(!g.application11C)return result(Status::Owner,0x2b1798u);
    WaterWakeUpdateProposal50JC next;
    if(*g.application11C){out=next;return result(Status::Paused,0x2b17a8u);}
    if(!g.wakeEnabled383418)return result(Status::Owner,0x2b17b0u);
    if(!*g.wakeEnabled383418){out=next;return result(Status::NeedsBaseChildren141ED0,0x2b17bcu);}
    if(std::fegetround()!=FE_TONEAREST)return result(Status::Arithmetic,0x2b1760u);
    if(!sourceVirtual38DeltaBound||!g.lazy8D38||!g.sentinel443BB8||
        !g.time443BB0||!g.swellPeriod443E88||!g.swellHeight443E80||!g.baseHeight443E90)
        return result(Status::Owner,0x2b17ccu);
    if(!p.sourceBound||!p.selectedRangeBound||!p.buckets40||p.count4C<=0||
        p.first44>p.last48||p.last48>=std::size_t(p.count4C))
        return result(Status::Owner,0x2b1870u);
    Math m;
    next.time443BB0=m.add(*g.time443BB0,delta,0x2b17fcu);
    const float phase=m.div(next.time443BB0,*g.swellPeriod443E88,0x2b1800u);
    if(m.badPC)return arithmetic(m);
    // Original302938(double(phase),0x401921FB60000000). This is the
    // exact widening of binary32 0x40C90FDB, not host double2*pi.
    volatile float reduced=static_cast<float>(std::fmod(double(phase),double(bits(0x40c90fdbu))));
    const float wave=sourceSine13BAD0(m.read(reduced,0x2b1838u),m);
    const float scaled=m.mul(*g.swellHeight443E80,wave,0x2b1860u);
    next.baseHeight3C=m.add(*g.baseHeight443E90,scaled,0x2b1864u);
    if(m.badPC)return arithmetic(m);
    next.writesLazySentinel=!*g.lazy8D38;
    next.lazy8D38=next.writesLazySentinel?1:*g.lazy8D38;
    next.sentinel443BB8=next.writesLazySentinel?-1:*g.sentinel443BB8;
    next.writesSwell=true;next.first44=p.first44;next.last48=p.last48;
    out=next;return result(Status::NeedsLatticeKernel,0x2b1bb0u,p.first44);
}
// Prepare one source bucket's scroll and buffer addresses. Exact packed-MMI
// lattice computation, cross-block carries, and texture writes follow in
// computeWaterWakeLattice50JC; this function alone does not perform them.
// A valid proposal is NOT evidence that new signed samples exist. All spans
// remain const and activeBuffer30/origin10 are not changed by this function.
inline WaterWakeResult50JC prepareWaterWakeBucket50JC(
    const WaterWakeProvider50JC& p,const WaterWakeGlobals50JC& g,
    std::size_t index,WaterWakeBucketProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    if(std::fegetround()!=FE_TONEAREST)return result(Status::Arithmetic,0x2b1878u,index);
    if(!p.sourceBound||!p.selectedRangeBound||!p.buckets40||p.count4C<=0||
        index<p.first44||index>p.last48||index>=std::size_t(p.count4C))
        return result(Status::Owner,0x2b1878u,index);
    const auto& b=p.buckets40[index];
    if(!b.sourceBound||!b.cellSize00||!b.inverseCellSize04||!b.origin10||
        !b.anchor20||!b.activeBuffer30||!g.rebaseDelta443B40)
        return result(Status::Owner,0x2b1880u,index);
    if(p.dimensionB0<=0||p.dimensionB0>0x01000000||p.blocksPerRowB4<=2||
        std::int64_t(p.blocksPerRowB4)*8!=p.dimensionB0||
        *b.activeBuffer30<0||*b.activeBuffer30>1)
        return result(Status::Unsupported,0x2b18f8u,index);
    WaterWakeBucketProposal50JC next;next.bucket=index;
    Math m;float shift[4]{};
    for(std::size_t c=0;c<3;++c)
        shift[c]=m.mul(*b.inverseCellSize04,m.sub(b.anchor20[c],b.origin10[c],0x2b189cu),0x2b18c0u);
    shift[3]=1.f;
    for(std::size_t c=0;c<4;++c)next.originBeforeKernel10[c]=b.origin10[c];
    const float dimension=m.integer(p.dimensionB0,0x2b18fcu);
    if(m.badPC)return arithmetic(m);
    if(std::fabs(shift[0])>dimension||std::fabs(shift[2])>dimension){
        next.rebasesOriginBeforeKernel=true;
        for(std::size_t c=0;c<4;++c){shift[c]=g.rebaseDelta443B40[c];next.originBeforeKernel10[c]=b.anchor20[c];}
    }
    const auto x=m.cvtWord(shift[0],0x2b1960u),z=m.cvtWord(shift[2],0x2b1968u);
    if(m.badPC)return arithmetic(m);
    next.xBlockShift=direction(arithmeticShift3(x));
    next.zCellShift=8*direction(arithmeticShift3(z));
    next.sourceBuffer=*b.activeBuffer30;next.destinationBuffer=(next.sourceBuffer+1)&1;
    const std::uint64_t blocks=std::uint32_t(p.blocksPerRowB4);
    next.signedBufferBytes=std::uint64_t(std::uint32_t(p.dimensionB0))*blocks*64u;
    next.rowStride=blocks*64u;
    next.firstDestination=(9u*blocks+1u)*64u;
    if(next.signedBufferBytes>0x7fffffffu||next.signedBufferBytes<=2u*next.firstDestination)
        return result(Status::Unsupported,0x2b1aa0u,index);
    next.destinationLimit=next.signedBufferBytes-next.firstDestination;
    next.firstRowLimit=next.firstDestination+next.rowStride-128u;
    const std::int64_t source=std::int64_t(next.firstDestination)+
        std::int64_t(next.zCellShift)*std::int64_t(next.rowStride)+
        std::int64_t(next.xBlockShift)*64;
    if(source<0||std::uint64_t(source)>=next.signedBufferBytes)
        return result(Status::Unsupported,0x2b1b10u,index);
    next.firstSource=std::uint64_t(source);
    for(std::size_t i=0;i<2;++i)
        if(!b.signedBuffers38[i].bound||!b.signedBuffers38[i].data||
            b.signedBuffers38[i].size<next.signedBufferBytes)
            return result(Status::Storage,0x2b1a7cu,index);
    if(!b.texture34.bound||!b.texture34.data||b.texture34.size<0x10000u)
        return result(Status::Storage,0x2b1a2cu,index);
    // Tail2B1D50..2B1DE8 executes ONLY after the real kernel has completed.
    const float advance[3]={m.integer(next.xBlockShift*8,0x2b1d64u),0.f,
        m.integer(next.zCellShift,0x2b1d70u)};
    for(std::size_t c=0;c<3;++c)
        next.originAfterKernel10[c]=m.add(next.originBeforeKernel10[c],
            m.mul(*b.cellSize00,advance[c],0x2b1d98u),0x2b1dc0u);
    next.originAfterKernel10[3]=next.originBeforeKernel10[3];
    if(m.badPC)return arithmetic(m);
    out=next;return result(Status::NeedsLatticeKernel,0x2b1bb0u,index);
}
} // namespace rr::game

namespace rr::game {
struct WaterWakeCoefficients50JC {
    // Eight signed halfwords per original shared vector. Borrow the genuine
    // source data owners; no tuning literal or inferred cold value is injected.
    const std::int16_t* minimumHeight383430=nullptr;
    const std::int16_t* maximumHeight383440=nullptr;
    const std::int16_t* minimumVelocity383450=nullptr;
    const std::int16_t* maximumVelocity383460=nullptr;
    const std::int16_t* quietHeightLow383470=nullptr;
    const std::int16_t* quietHeightHigh383480=nullptr;
    const std::int16_t* quietVelocityLow383490=nullptr;
    const std::int16_t* quietVelocityHigh3834A0=nullptr;
    const std::int16_t* velocityDecay3834B0=nullptr;
    const std::int16_t* neighbors3834C0=nullptr;
    const std::int16_t* center3834D0=nullptr;
    bool sourceBound=false;
};
struct WaterWakeScratch50JC {
    std::uint8_t* nextSigned=nullptr;
    std::size_t nextSignedSize=0;
    std::uint8_t* texture=nullptr;
    std::size_t textureSize=0;
};
namespace water_wake_detail50jc {
inline std::int32_t saturate16(std::int64_t x) noexcept {
    return x>32767?32767:x< -32768?-32768:static_cast<std::int32_t>(x);
}
inline std::int32_t wrap16(std::int32_t x) noexcept {
    const auto u=static_cast<std::uint32_t>(x)&0xffffu;
    return u<0x8000u?std::int32_t(u):std::int32_t(u)-65536;
}
inline std::int32_t signedShift(std::int32_t x,unsigned n) noexcept {
    const std::int64_t value=x,divisor=std::int64_t(1)<<n;
    return static_cast<std::int32_t>(value>=0?value/divisor:-((-value+divisor-1)/divisor));
}
inline std::int32_t halfword(const std::uint8_t* bytes,std::size_t offset) noexcept {
    const std::uint32_t u=std::uint32_t(bytes[offset])|(std::uint32_t(bytes[offset+1])<<8u);
    return u<0x8000u?std::int32_t(u):std::int32_t(u)-65536;
}
inline void putHalfword(std::uint8_t* bytes,std::size_t offset,std::int32_t value) noexcept {
    const auto u=static_cast<std::uint32_t>(value)&0xffffu;
    bytes[offset]=static_cast<std::uint8_t>(u);bytes[offset+1]=static_cast<std::uint8_t>(u>>8u);
}
inline bool wordRange(std::int64_t x) noexcept {return x>=-2147483648ll&&x<=2147483647ll;}
inline bool overlap(const void* a,std::size_t as,const void* b,std::size_t bs) noexcept {
    const auto aa=reinterpret_cast<std::uintptr_t>(a),bb=reinterpret_cast<std::uintptr_t>(b);
    if(aa>std::numeric_limits<std::uintptr_t>::max()-as||
        bb>std::numeric_limits<std::uintptr_t>::max()-bs)return true;
    return aa<bb+bs&&bb<aa+as;
}
}
// Native domain kernel, no instructions/register file/guest addresses involved.
// Admits genuine loader lattices120x120 (fifteen8-cell blocks per row)
// and88x88 (eleven blocks). The render texture remains128x128. Other layouts
// fail explicitly. Current input, prior next-buffer border bytes and
// prior texture bytes are authentic source views. They remain unchanged.
// Success places the full next signed image and texture image in disjoint
// caller scratch and returns LatticeComputed. The caller must still perform
// exact2B1D50..2B1DE8 origin/selector commit, native texture-view release, and
// the +44..+48 inclusive loop. Failure may alter scratch only.
inline WaterWakeResult50JC computeWaterWakeLattice50JC(
    const WaterWakeProvider50JC& p,const WaterWakeGlobals50JC& g,
    const WaterWakeCoefficients50JC& c,std::size_t index,
    WaterWakeScratch50JC scratch,WaterWakeBucketProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    WaterWakeBucketProposal50JC plan;
    const auto prepared=prepareWaterWakeBucket50JC(p,g,index,plan);
    if(prepared.status!=Status::NeedsLatticeKernel)return prepared;
    if(!((p.dimensionB0==120&&p.blocksPerRowB4==15)||
        (p.dimensionB0==88&&p.blocksPerRowB4==11)))
        return result(Status::Unsupported,0x2b1b30u,index);
    if(!c.sourceBound||!c.minimumHeight383430||!c.maximumHeight383440||
        !c.minimumVelocity383450||!c.maximumVelocity383460||
        !c.quietHeightLow383470||!c.quietHeightHigh383480||
        !c.quietVelocityLow383490||!c.quietVelocityHigh3834A0||
        !c.velocityDecay3834B0||!c.neighbors3834C0||!c.center3834D0)
        return result(Status::Owner,0x2b1b58u,index);
    const std::size_t signedSize=std::size_t(p.dimensionB0)*std::size_t(p.blocksPerRowB4)*64u;
    constexpr std::size_t textureSize=128u*128u*4u;
    if(!scratch.nextSigned||scratch.nextSignedSize<signedSize||
        !scratch.texture||scratch.textureSize<textureSize)
        return result(Status::Storage,0x2b1bb0u,index);
    const auto& b=p.buckets40[index];
    const auto* source=b.signedBuffers38[plan.sourceBuffer].data;
    const auto* priorNext=b.signedBuffers38[plan.destinationBuffer].data;
    const auto* priorTexture=b.texture34.data;
    if(overlap(source,signedSize,priorNext,signedSize)||
        overlap(source,signedSize,priorTexture,textureSize)||
        overlap(priorNext,signedSize,priorTexture,textureSize)||
        overlap(scratch.nextSigned,signedSize,scratch.texture,textureSize)||
        overlap(scratch.nextSigned,signedSize,source,signedSize)||
        overlap(scratch.nextSigned,signedSize,priorNext,signedSize)||
        overlap(scratch.nextSigned,signedSize,priorTexture,textureSize)||
        overlap(scratch.texture,textureSize,source,signedSize)||
        overlap(scratch.texture,textureSize,priorNext,signedSize)||
        overlap(scratch.texture,textureSize,priorTexture,textureSize))
        return result(Status::Storage,0x2b1bb0u,index);
    // Preserve untouched original border planes and texture pixels.
    std::memcpy(scratch.nextSigned,priorNext,signedSize);
    std::memcpy(scratch.texture,priorTexture,textureSize);
    const std::size_t rowStride=std::size_t(p.blocksPerRowB4)*64u;
    std::size_t rowNumber=0;
    for(std::size_t row=std::size_t(plan.firstDestination);
        row<std::size_t(plan.destinationLimit);row+=rowStride,++rowNumber){
        std::int32_t previousHeight[8]{}; // Actual source stack+140 row clear.
        const auto rowLimit=row+rowStride-128u;
        std::size_t blockNumber=0;
        std::size_t lastDestination=row;
        for(std::size_t destination=row;destination<rowLimit;
            destination+=64u,++blockNumber){
            const std::int64_t origin=std::int64_t(plan.firstSource)+
                std::int64_t(rowNumber*rowStride+blockNumber*64u);
            if(origin<std::int64_t(rowStride)||origin+std::int64_t(rowStride+16u)>std::int64_t(signedSize))
                return result(Status::Storage,0x2b1bb8u,index);
            const auto src=std::size_t(origin);
            const auto texture=std::size_t(plan.textureFirst)+rowNumber*0x200u+blockNumber*32u;
            if(destination+64u>signedSize||texture+32u>textureSize)
                return result(Status::Storage,0x2b1c7cu,index);
            std::int32_t height[8]{},velocity[8]{};
            for(std::size_t lane=0;lane<8;++lane){
                const auto offset=lane*2u;
                const auto center=halfword(source,src+offset);
                const auto oldVelocity=halfword(source,src+0x30u+offset);
                const auto left=halfword(source,src+0x10u+offset);
                const auto right=halfword(source,src+0x20u+offset);
                const auto north=halfword(source,src-rowStride+offset);
                const auto south=halfword(source,src+rowStride+offset);
                // Three source PADDSH operations before the full-width terms.
                const auto horizontal=saturate16(std::int64_t(left)+right);
                const auto vertical=saturate16(std::int64_t(south)+north);
                const auto neighbors=saturate16(std::int64_t(horizontal)+vertical);
                std::int64_t force=std::int64_t(neighbors)*c.neighbors3834C0[lane];
                force+=std::int64_t(center)*c.center3834D0[lane];
                if(!wordRange(force))return result(Status::Unsupported,0x2b1bdcu,index);
                force+=std::int64_t(signedShift(oldVelocity,6))*c.velocityDecay3834B0[lane];
                if(!wordRange(force))return result(Status::Unsupported,0x2b1becu,index);
                // PMFHL.SH is signed saturation, not low-half truncation.
                auto v=saturate16(std::int64_t(signedShift(saturate16(force),2))+oldVelocity);
                if(v<c.minimumVelocity383450[lane])v=c.minimumVelocity383450[lane];
                if(v>c.maximumVelocity383460[lane])v=c.maximumVelocity383460[lane];
                auto h=saturate16(std::int64_t(center)+signedShift(v,8));
                if(h<c.minimumHeight383430[lane])h=c.minimumHeight383430[lane];
                if(h>c.maximumHeight383440[lane])h=c.maximumHeight383440[lane];
                // PCGTH masks are -1/0; PMULTH + PMFHL.SH turn each paired
                // strict comparison into1/0, then multiply both results.
                const std::int32_t quietHeight=(h>c.quietHeightLow383470[lane]&&c.quietHeightHigh383480[lane]>h)?1:0;
                const std::int32_t quietVelocity=(v>c.quietVelocityLow383490[lane]&&c.quietVelocityHigh3834A0[lane]>v)?1:0;
                const auto quiet=quietHeight*quietVelocity;
                // Literal source2B1C64..78: the intervening products' HI/LO
                // are not read. PADDUB copies quiet; PSUBH subtracts quiet.
                // Preserve this one-unit subtraction, including at h=v=0.
                h=wrap16(h-quiet);v=wrap16(v-quiet);
                height[lane]=h;velocity[lane]=v;
                putHalfword(scratch.nextSigned,destination+offset,h);
                putHalfword(scratch.nextSigned,destination+0x30u+offset,v);
                const auto texel=saturate16(std::int64_t(h)+c.maximumHeight383440[lane]);
                // PEXTLH/UH with zero interleave one signed-halfword bit
                // pattern and zero into each32-bit destination texture pixel.
                putHalfword(scratch.texture,texture+lane*4u,texel);
                putHalfword(scratch.texture,texture+lane*4u+2u,0);
            }
            // MTSAH0,1 / QFSRV: previous[1..7],current[0] -> previous right.
            // MTSAH0,15 / QFSRV: previous[7],current[0..6] -> current left.
            for(std::size_t lane=0;lane<8;++lane){
                const auto right=lane<7?previousHeight[lane+1]:height[0];
                const auto left=lane?height[lane-1]:previousHeight[7];
                putHalfword(scratch.nextSigned,destination-64u+0x20u+lane*2u,right);
                putHalfword(scratch.nextSigned,destination+0x10u+lane*2u,left);
            }
            if(destination==row)putHalfword(scratch.nextSigned,destination-64u+0x2eu,0);
            for(std::size_t lane=0;lane<8;++lane)previousHeight[lane]=height[lane];
            lastDestination=destination;
        }
        // Row-final QFSRV has zero for the next block; no synthetic wrap.
        for(std::size_t lane=0;lane<8;++lane)
            putHalfword(scratch.nextSigned,lastDestination+0x20u+lane*2u,
                lane<7?previousHeight[lane+1]:0);
    }
    out=plan;return result(Status::LatticeComputed,0x2b1d50u,index);
}
} // namespace rr::game

namespace rr::game {
struct WaterWakeForceInput50JC {
    float point[3]{};
    float amplitude=0;
    bool sourceCallBound=false; // Actual original2B1370 point/f12 and ordering.
};
struct WaterWakeForceGlobals50JC {
    const float* forceScale383428=nullptr;
    const std::int16_t* shiftGP8024=nullptr; // Source LH -32732($gp).
};
struct WaterWakeForceWrite50JC {
    std::size_t bucket=0;
    std::int32_t activeBuffer=0;
    std::uint64_t byteOffset=0;
    std::uint16_t before=0,after=0;
    std::uint32_t shiftedDelta=0,sourcePC=0;
};
struct WaterWakeForceWorkspace50JC {
    WaterWakeForceWrite50JC* writes=nullptr;
    std::size_t capacity=0;
};
struct WaterWakeForceProposal50JC {
    std::size_t writeCount=0;
    std::size_t visitedBuckets=0;
};
// Actual2B1370, as ordered velocity-write proposals. This source routine visits
// bucket0 through inclusive provider+48; it does NOT start at selected+44, and
// has no vehicle58/pause/global383418 gate of its own. Caller eligibility and
// call order must remain those of the real invoking routine. In particular,
// AI force calls are not removed merely because the human-only anchor skipped.
// Success leaves canonical storage unchanged and exposes every source SH,
// including zero-delta writes. Commit against the captured prior values before
// the next force call/query/lattice use; never aggregate against stale buffers.
// Workspace must be disjoint from canonical source owners and the proposal;
// represented owner/storage overlaps are rejected before the first write.
// Failure may modify workspace entries, but leaves the public proposal intact.
inline WaterWakeResult50JC projectWaterWakeForce50JC(
    const WaterWakeProvider50JC& p,const WaterWakeForceGlobals50JC& g,
    const WaterWakeForceInput50JC& in,WaterWakeForceWorkspace50JC workspace,
    WaterWakeForceProposal50JC& out) noexcept {
    using namespace water_wake_detail50jc;
    if(std::fegetround()!=FE_TONEAREST)return result(Status::Arithmetic,0x2b1370u);
    if(!in.sourceCallBound||!p.sourceBound||!p.selectedRangeBound||!p.buckets40||
        p.count4C<=0||p.last48>=std::size_t(p.count4C)||
        !g.forceScale383428||!g.shiftGP8024)
        return result(Status::Owner,0x2b13b4u);
    if(!((p.dimensionB0==120&&p.blocksPerRowB4==15)||
        (p.dimensionB0==88&&p.blocksPerRowB4==11)))
        return result(Status::Unsupported,0x2b1450u);
    const std::size_t visited=p.last48+1u;
    if(visited>std::numeric_limits<std::size_t>::max()/(4u*sizeof(WaterWakeForceWrite50JC))||
        !workspace.writes||workspace.capacity<visited*4u)
        return result(Status::Storage,0x2b1554u);
    const std::size_t workspaceBytes=visited*4u*sizeof(WaterWakeForceWrite50JC);
    if(overlap(workspace.writes,workspaceBytes,&p,sizeof(p))||
        overlap(workspace.writes,workspaceBytes,&g,sizeof(g))||
        overlap(workspace.writes,workspaceBytes,&in,sizeof(in))||
        overlap(workspace.writes,workspaceBytes,&out,sizeof(out))||
        overlap(workspace.writes,workspaceBytes,p.buckets40,visited*sizeof(WaterWakeBucket50JC))||
        overlap(workspace.writes,workspaceBytes,g.forceScale383428,sizeof(float))||
        overlap(workspace.writes,workspaceBytes,g.shiftGP8024,sizeof(std::int16_t)))
        return result(Status::Storage,0x2b1554u);
    const std::size_t signedBytes=std::size_t(p.dimensionB0)*std::size_t(p.blocksPerRowB4)*64u;
    const std::size_t rowStride=std::size_t(p.blocksPerRowB4)*64u;
    // The source normally owns distinct allocated bucket buffers. Reject
    // overlapping projections so one proposal never reads a stale aliased SH.
    for(std::size_t i=0;i<visited;++i){
        const auto& b=p.buckets40[i];
        if(!b.sourceBound||!b.inverseCellSize04||!b.origin10||!b.activeBuffer30||
            *b.activeBuffer30<0||*b.activeBuffer30>1)
            return result(Status::Owner,0x2b13e4u,i);
        if(overlap(workspace.writes,workspaceBytes,b.inverseCellSize04,sizeof(float))||
            overlap(workspace.writes,workspaceBytes,b.origin10,4u*sizeof(float))||
            overlap(workspace.writes,workspaceBytes,b.activeBuffer30,sizeof(std::int32_t)))
            return result(Status::Storage,0x2b1554u,i);
        for(const auto& storage:b.signedBuffers38)
            if(storage.data&&overlap(workspace.writes,workspaceBytes,storage.data,storage.size))
                return result(Status::Storage,0x2b1554u,i);
        if(b.texture34.data&&overlap(workspace.writes,workspaceBytes,b.texture34.data,b.texture34.size))
            return result(Status::Storage,0x2b1554u,i);
        const auto& bytes=b.signedBuffers38[*b.activeBuffer30];
        if(!bytes.bound||!bytes.data||bytes.size<signedBytes)
            return result(Status::Storage,0x2b1510u,i);
        for(std::size_t j=0;j<i;++j){
            const auto& before=p.buckets40[j];
            if(overlap(bytes.data,signedBytes,before.signedBuffers38[*before.activeBuffer30].data,signedBytes))
                return result(Status::Unsupported,0x2b1510u,i);
        }
    }
    Math m;WaterWakeForceProposal50JC next;next.visitedBuckets=visited;
    for(std::size_t i=0;i<visited;++i){
        const auto& b=p.buckets40[i];float relative[3]{};
        for(std::size_t axis=0;axis<3;++axis)
            relative[axis]=m.mul(*b.inverseCellSize04,m.sub(in.point[axis],b.origin10[axis],0x2b13e8u),0x2b1408u);
        const auto x=m.cvtWord(relative[0],0x2b1434u),z=m.cvtWord(relative[2],0x2b143cu);
        if(m.badPC)return arithmetic(m);
        if(x<8||x>=p.dimensionB0-10||z<9||z>=p.dimensionB0-10)continue;
        const float dx=m.sub(relative[0],m.integer(x,0x2b1484u),0x2b1494u);
        const float dz=m.sub(relative[2],m.integer(z,0x2b1490u),0x2b1498u);
        const float w1=m.mul(dx,dz,0x2b14a0u);
        const float oneZ=m.sub(1.f,dz,0x2b14a4u);
        const float oneX=m.sub(1.f,dx,0x2b14a8u);
        const float w2=m.mul(oneX,dz,0x2b14acu);
        const float w3=m.mul(dx,oneZ,0x2b14b0u);
        const float w4=m.mul(oneX,oneZ,0x2b14b4u);
        float force=m.read(in.amplitude,0x2b14c0u);
        if(force< -1.f)force=-1.f; // Actual MAX.S(force,-1).
        if(force>1.f)force=1.f; // Actual MIN.S(previous,+1).
        force=m.mul(*g.forceScale383428,force,0x2b150cu);
        if(m.badPC)return arithmetic(m);
        const std::size_t base=(std::size_t(z)*std::size_t(p.blocksPerRowB4)+std::size_t(x/8))*64u;
        const auto lane=std::size_t(x%8);
        std::size_t offsets[4]{};float weights[4]{};std::uint32_t pcs[4]{};
        offsets[0]=base+0x30u+lane*2u;weights[0]=w1;pcs[0]=0x2b1554u;
        if(lane<7u){
            offsets[1]=offsets[0]+2u;weights[1]=w2;pcs[1]=0x2b1578u;
            offsets[2]=offsets[0]+rowStride;weights[2]=w3;pcs[2]=0x2b15b0u;
            offsets[3]=offsets[2]+2u;weights[3]=w4;pcs[3]=0x2b15c8u;
        }else{
            offsets[1]=base+64u+0x30u;weights[1]=w2;pcs[1]=0x2b15f0u;
            // Block crossing stores bottom-right BEFORE bottom-left.
            offsets[2]=base+64u+rowStride+0x30u;weights[2]=w4;pcs[2]=0x2b1620u;
            offsets[3]=base+rowStride+0x3eu;weights[3]=w3;pcs[3]=0x2b1640u;
        }
        const auto selected=*b.activeBuffer30;
        const auto* bytes=b.signedBuffers38[selected].data;
        for(std::size_t j=0;j<4;++j){
            const auto increment=m.cvtWord(m.mul(weights[j],force,0x2b1520u),0x2b1534u);
            if(m.badPC)return arithmetic(m);
            const unsigned shift=static_cast<std::uint16_t>(*g.shiftGP8024)&31u;
            const std::uint32_t delta=static_cast<std::uint32_t>(increment)<<shift;
            if(offsets[j]+2u>signedBytes)return result(Status::Storage,pcs[j],i);
            const auto prior=halfword(bytes,offsets[j]);
            const auto changed=static_cast<std::uint32_t>(prior)+delta; // ADDU/SH wrap.
            WaterWakeForceWrite50JC write;
            write.bucket=i;write.activeBuffer=selected;write.byteOffset=offsets[j];
            write.before=static_cast<std::uint16_t>(static_cast<std::uint32_t>(prior));
            write.after=static_cast<std::uint16_t>(changed);write.shiftedDelta=delta;
            write.sourcePC=pcs[j];workspace.writes[next.writeCount++]=write;
        }
    }
    out=next;return result(Status::ForceProjected,0x2b1668u);
}
} // namespace rr::game
