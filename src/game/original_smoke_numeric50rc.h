#pragma once
#include "original_gc_spray50kg.h"
#include "water_wake50jc.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game::original_smoke_numeric_detail50RC {
// Shared numerical leaf only. Original15BB40..15C324 and19A870..19B054
// have499/505 identical words; six differences address the same assertion
// strings. Constructor, authoring, common prefix, ownership and scheduler
// admission stay at their distinct callers. This template cannot create any
// actual emitter controls, manager registration or source identity.
using S=OriginalGcSprayStatus50KG;
using Math=water_wake_detail50jc::Math;
inline float bits(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
inline OriginalGcSprayReceipt50KG receipt(S status,std::uint32_t pc) noexcept {
    OriginalGcSprayReceipt50KG r;r.status=status;r.sourcePC=pc;return r;
}
inline float squareRoot(float x,Math& m,std::uint32_t pc) noexcept {
    m.read(x,pc);if(x<0){if(!m.badPC)m.badPC=pc;return 0;}
    volatile float r=std::sqrt(x);return m.read(r,pc);
}
inline float squareLength(const std::array<float,3>& a,Math& m,std::uint32_t pc) noexcept {
    const auto x=m.mul(a[0],a[0],pc),y=m.mul(a[1],a[1],pc+4);
    const auto xy=m.add(x,y,pc+8);
    return m.add(xy,m.mul(a[2],a[2],pc+12),pc+12);
}
inline std::array<float,3> normalize13E3F0(const std::array<float,3>& a,Math& m) noexcept {
    //13E400 vmul.xyz;13E408 vadday.x ACC;13E40C vmaddz.x;
    //13E410 vrsqrt Q,vf0.w,vf5.x;13E41C vmulq.xyz.
    const auto square=squareLength(a,m,0x13e400);
    const auto inverse=m.div(1,squareRoot(square,m,0x13e410),0x13e410);
    std::array<float,3> out{};
    for(unsigned k=0;k<3;++k)out[k]=m.mul(a[k],inverse,0x13e41c);
    return out;
}
inline std::array<float,3> cross13E440(const std::array<float,3>& a,
    const std::array<float,3>& b,Math& m) noexcept {
    // VOPMULA.xyz then VOPMSUB.xyz. Preserve product/subtract rounding.
    std::array<float,3> out{};
    for(unsigned k=0;k<3;++k){const auto j=(k+1)%3,l=(k+2)%3;
        const auto first=m.mul(a[j],b[l],0x13e454);
        out[k]=m.sub(first,m.mul(a[l],b[j],0x13e458),0x13e458);}
    return out;
}
template<std::uint32_t SourceBase,class Runtime,class Definition>
OriginalGcSprayReceipt50KG emit(Runtime& runtime,const Definition& f,
    OriginalGcParticleManager50KG& manager,EffectControlState50BC& c,float dt,
    std::uint32_t& raw,SharedRandomOwner50ID& shared,HostMonotonicClock50ID& clock) noexcept {
    static_assert(SourceBase==0x19a870u||SourceBase==0x15bb40u,"only the proved two source bodies");
    constexpr auto pc=[](std::uint32_t original)noexcept {
        return original>=0x19a870u&&original<0x19b060u?original-0x19a870u+SourceBase:original;
    };
    auto r=receipt(S::complete,pc(0x19b018u));Math m;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;
        runtime.failed=true;runtime.stoppedAt=r;return r;};
    auto failMath=[&](){return fail(S::arithmetic,m.badPC);};
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,pc(0x19a870u));
    std::array<float,3> displacement{},position{},baseVelocity{};
    const std::uint32_t subtractPC[]={pc(0x19a88cu),pc(0x19a894u),pc(0x19a89cu)};
    for(unsigned k=0;k<3;++k){position[k]=bits(c.position30[k]);
        displacement[k]=m.sub(position[k],bits(c.previous40[k]),subtractPC[k]);}
    const auto distance=squareRoot(squareLength(displacement,m,pc(0x19a8c0u)),m,pc(0x19a8d0u));
    const auto speed=m.div(distance,dt,pc(0x19a8e0u));
    if(m.badPC)return failMath();
    if(distance<=.25f){ //19A8F4 C.LE.S; short displacement keeps authored basis.
        for(unsigned k=0;k<3;++k)baseVelocity[k]=m.read(bits(c.velocity60[k]),pc(0x19aafcu));
    }else{
        const auto direction=normalize13E3F0(displacement,m);
        // Near-vertical direction uses the Z reference, otherwise the Y one.
        // The threshold is exact0x3C800000 = 1/64, not an epsilon policy.
        const std::array<float,3> reference=(std::fabs(direction[0])<.015625f&&
            std::fabs(direction[2])<.015625f)?std::array<float,3>{{0,0,1}}:
                std::array<float,3>{{0,1,0}};
        const auto right=normalize13E3F0(cross13E440(reference,direction,m),m);
        const auto up=normalize13E3F0(cross13E440(direction,right,m),m);
        //19AA34 first computes Z-scaled direction, then Y-scaled up, then X.
        std::array<float,3> z{},y{},x{};
        for(unsigned k=0;k<3;++k)z[k]=m.mul(bits(c.velocity60[2]),direction[k],pc(0x19aa34u)+4u*k);
        for(unsigned k=0;k<3;++k)y[k]=m.mul(bits(c.velocity60[1]),up[k],pc(0x19aa58u)+4u*k);
        for(unsigned k=0;k<3;++k)x[k]=m.mul(bits(c.velocity60[0]),right[k],pc(0x19aa7cu)+4u*k);
        for(unsigned k=0;k<3;++k){const auto sum=m.add(x[k],y[k],pc(0x19aaa8u)+4u*k);
            baseVelocity[k]=m.add(sum,z[k],pc(0x19aad4u)+4u*k);}
    }
    if(m.badPC)return failMath();
    if(speed<f.minimumEmissionSpeed23C){r.sourcePC=pc(0x19ab1cu);return r;}
    auto size=m.mul(f.initialSizeToSpeedRatio210,speed,pc(0x19ab28u));
    if(size<f.minimumInitialSize208)size=f.minimumInitialSize208;
    else if(!(size<=f.maximumInitialSize20C))size=f.maximumInitialSize20C;
    auto life=m.mul(f.lifeToSpeedRatio224,speed,pc(0x19ab64u));
    if(life<f.minimumLife214)life=f.minimumLife214;
    else if(!(life<=f.maximumLife218))life=f.maximumLife218;
    const auto minimum=m.mul(f.minimumEmitRate22C,dt,pc(0x19aba4u));
    const auto distancePart=m.mul(f.particlesToDistanceRatio234,distance,pc(0x19aba8u));
    auto demand=m.add(distancePart,m.mul(f.particlesToSpeedRatio238,speed,pc(0x19abacu)),pc(0x19abacu));
    if(demand<minimum)demand=minimum;
    else{const auto maximum=m.mul(f.maximumEmitRate230,dt,pc(0x19abccu));
        if(!(demand<=maximum))demand=maximum;}
    const auto scaled=m.mul(demand,bits(c.intensity204),pc(0x19abf4u));
    demand=m.add(scaled,runtime.remainder200,pc(0x19abf8u));
    if(m.badPC)return failMath();
    std::int32_t count=0;
    if(demand>1.f){
        count=m.cvtWord(demand,pc(0x19ac0cu));
        const auto remainder=m.sub(demand,m.integer(count,pc(0x19ac14u)),pc(0x19ac18u));
        if(m.badPC)return failMath();runtime.remainder200=remainder;
    }else{
        // Deliberately preserve the source double-carry: demand already
        // includes old+200, and19AC24 adds old+200 once more. Do not repair it.
        const auto remainder=m.add(runtime.remainder200,demand,pc(0x19ac24u));
        if(m.badPC)return failMath();runtime.remainder200=remainder;
    }
    std::array<float,3> step{};
    if(count){const auto inverse=m.div(1,m.integer(count,pc(0x19ac34u)),pc(0x19ac4cu));
        for(unsigned k=0;k<3;++k)step[k]=m.mul(displacement[k],inverse,pc(0x19ac58u)+4u*k);
        if(m.badPC)return failMath();
    }else{
        //19AC4C computes1/0 and dead local products before19B008 rejects the
        // loop. No owner/particle uses them. Keep count0 and its exact carry.
        r.deadZeroCountReciprocal=true;return r;
    }
    for(std::int32_t particle=0;particle<count;++particle){
        auto* p=manager.nextParticle();
        if(!p){r.poolFull=true;r.sourcePC=pc(0x19ac98u);return r;}
        // Retail only writes these fields. Keep pool reuse20/24/4C untouched.
        const auto age=m.mul(life,f.initialPercentAge228,pc(0x19acb0u));
        if(m.badPC)return failMath();p->ageMilliseconds=age;
        p->position=position;std::memcpy(&p->positionW,c.position30+3,4);
        p->frame=runtime.currentFrame190;p->initialSize=size;
        p->ownerId=runtime.managerOwnerId;p->size=size;
        // Four raw draws precede the shared lazy seed/selector. There is no
        // private RNG, mirror draw, random position spread, or extra size draw.
        const auto lifeRandom=draw();
        const auto product=m.mul(f.minimumPercentLifeConvergance220,lifeRandom,pc(0x19acfcu));
        const auto denominator=m.add(1,product,pc(0x19ad08u));
        const auto decay=m.div(1,denominator,pc(0x19ad0cu));
        const auto remaining=m.sub(1,f.minimumPercentLife21C,pc(0x19ad10u));
        const auto range=m.mul(life,remaining,pc(0x19ad14u));
        const auto tapered=m.mul(range,decay,pc(0x19ad18u));
        const auto actualLife=m.add(tapered,m.mul(life,f.minimumPercentLife21C,pc(0x19ad1cu)),pc(0x19ad1cu));
        const auto inverseLife=m.div(1,actualLife,pc(0x19ad24u));
        if(m.badPC)return failMath();p->lifeMilliseconds=actualLife;p->inverseLife=inverseLife;
        p->flags=f.particleFlags198;
        const auto growth=m.mul(size,f.growth180,pc(0x19ad40u));
        if(m.badPC)return failMath();p->growth=growth;if(growth==0)p->flags&=~8u;
        const auto x=draw(),y=draw(),z=draw();
        const float unit[]={x,y,z};std::array<float,3> noise{},velocity{};
        for(unsigned k=0;k<3;++k)noise[k]=m.sub(f.maximumVelocityNoise250[k],f.minimumVelocityNoise240[k],pc(0x19adf0u)+4u*k);
        for(unsigned k=0;k<3;++k)noise[k]=m.mul(noise[k],unit[k],pc(0x19ae1cu)+4u*k);
        for(unsigned k=0;k<3;++k)noise[k]=m.add(f.minimumVelocityNoise240[k],noise[k],pc(0x19ae48u)+4u*k);
        for(unsigned k=0;k<3;++k)velocity[k]=m.add(baseVelocity[k],noise[k],pc(0x19ae84u)+4u*k);
        if(m.badPC)return failMath();p->velocity=velocity;p->velocityW=1;
        // Publication is BEFORE next position and frame selection. A failing
        // clock/range assertion leaves this already-live particle published.
        if(!manager.commitParticle(runtime.managerOwnerId))return fail(S::storage,pc(0x19aeb4u));
        ++r.spawned;
        for(unsigned k=0;k<3;++k)position[k]=m.sub(position[k],step[k],pc(0x19aeccu)+12u*k);
        if(m.badPC)return failMath();
        if(!seedOriginalGcParticleRandom50KG(raw,shared,clock))return fail(S::clock,pc(0x19af18u));
        const auto wide=std::int64_t(runtime.lastFrame1B4)-std::int64_t(runtime.firstFrame1B0)+1;
        if(wide<1||wide>std::numeric_limits<std::int32_t>::max())return fail(S::storage,pc(0x19af3cu));
        const auto span=m.integer(static_cast<std::int32_t>(wide),pc(0x19af48u));
        const auto selectorRandom=draw();
        const auto offset=m.cvtWord(m.mul(span,selectorRandom,pc(0x19af68u)),pc(0x19af6cu));
        if(m.badPC)return failMath();
        const auto selectedWide=std::int64_t(runtime.firstFrame1B0)+offset;
        if(selectedWide<std::numeric_limits<std::int32_t>::min()||selectedWide>std::numeric_limits<std::int32_t>::max())
            return fail(S::arithmetic,pc(0x19af78u));
        const auto selected=static_cast<std::int32_t>(selectedWide);
        if(!checkOriginalGcParticleRandomRange50KG(selected,runtime.firstFrame1B0,runtime.lastFrame1B4,shared))
            return fail(S::sourceRangeAssertion,pc(0x19afccu));
        runtime.currentFrame190=selected; //19B004 is for the NEXT particle.
    }
    return r;
}
} // namespace rr::game::original_smoke_numeric_detail50RC
