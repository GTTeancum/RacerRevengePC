#include "original_gc_fire_emission50kg.h"
#include "water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
OriginalGcFireEmissionReceipt50KG emitOriginalGcFireParticles50KG(
    const OriginalGcFireEmissionDefinition50KG& f,OriginalGcParticleManager50KG& manager,
    std::uint32_t owner,float& fractional,const std::array<float,3>& previous,
    std::uint32_t previousW,const std::array<float,3>& velocity,float dt,
    std::uint32_t& raw,std::uint32_t& serial,bool allowCarry) noexcept {
    using S=OriginalGcSprayStatus50KG;using Math=water_wake_detail50jc::Math;
    OriginalGcFireEmissionReceipt50KG r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    const auto* common=manager.emitter(owner);const auto* group=manager.group(f.particleGroup);
    if(!manager.prepared()||!owner||!common||!group)return fail(S::unprepared,0x15e2d0);
    if(common->particleGroup!=f.particleGroup||common->framesPerSet!=f.framesPerSet||
       group->first!=f.firstFrame||group->last!=f.lastFrame)return fail(S::storage,0x15e534);
    if(&raw==&serial)return fail(S::storage,0x15e2e8);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x15e1d0);
    Math m;auto failMath=[&](){return fail(S::arithmetic,m.badPC);};
    m.read(dt,0x15e238);m.read(fractional,0x15e260);
    if(m.badPC)return failMath();
    // Native arithmetic admission, not a replacement/clamp for caller time.
    if(dt<=0||dt>1||fractional<0||f.emitRate<0||f.particleLife<=0||f.framesPerSet<=0)
        return fail(S::unsupported,0x15e238);
    const auto xx=m.mul(velocity[0],velocity[0],0x15e1e0);
    const auto yy=m.mul(velocity[1],velocity[1],0x15e1e4);
    const auto xy=m.add(xx,yy,0x15e1e8);
    const auto square=m.add(xy,m.mul(velocity[2],velocity[2],0x15e1ec),0x15e1ec);
    if(m.badPC)return failMath();
    volatile float speed=std::sqrt(square);m.read(speed,0x15e1f0);
    const auto birthFactor=std::min(1.f,m.div(speed,650.f,0x15e214));
    const auto emission=m.mul(.6f,m.mul(f.emitRate,dt,0x15e238),0x15e23c);
    if(m.badPC)return failMath();
    std::int32_t requested=0;
    if(emission>1){requested=m.cvtWord(emission,0x15e250);if(m.badPC)return failMath();fractional=0;}
    else {
        const auto accumulated=m.add(fractional,emission,0x15e26c);
        if(m.badPC)return failMath();fractional=accumulated;
        if(accumulated==0)requested=0;
        else if(accumulated>1){requested=m.cvtWord(accumulated,0x15e29c);if(m.badPC)return failMath();fractional=0;}
        else {
            //15E288..298 really leaves s1 undefined for0<accumulated<=1.
            // Explicit approved native policy only; never claim source-exact
            // counts or reconstruct a preceding callee-saved register value.
            if(!allowCarry)return fail(S::unsupported,0x15e298);
            requested=m.cvtWord(accumulated,0x15e298);
            fractional=m.sub(accumulated,m.integer(requested,0x15e298),0x15e298);
            if(m.badPC)return failMath();r.nativeUndefinedCarryPolicy=true;
        }
    }
    r.status=S::complete;r.sourcePC=0x15e6a8;
    for(std::int32_t n=0;n<requested;++n){
        //15E2D0..E0 checks capacity before serial, RNG, or particle writes.
        auto* p=manager.nextParticle();
        if(!p){r.poolFull=true;r.sourcePC=0x15e688;return r;}
        ++serial;
        const auto x=drawOriginalGcParticleRandom50KG(raw);
        const auto y=drawOriginalGcParticleRandom50KG(raw);
        const auto z=drawOriginalGcParticleRandom50KG(raw);r.randomDraws+=3;
        const auto spreadX=m.add(f.minRange[0],m.mul(x,m.sub(f.maxRange[0],f.minRange[0],0x15e388),0x15e38c),0x15e390);
        const auto spreadZ=m.add(f.minRange[2],m.mul(z,m.sub(f.maxRange[2],f.minRange[2],0x15e3a4),0x15e3a8),0x15e3ac);
        if(m.badPC)return failMath();
        std::int32_t set=0;if(z>.33f)set=1;if(z>.66f)set=2;
        // Authenticated callers prove forced-set pointer1C4 null. A generic
        // forced-set branch is intentionally not fabricated by this kernel.
        p->position=previous;std::memcpy(&p->positionW,&previousW,4);
        p->position[0]=m.add(p->position[0],spreadX,0x15e4d0);
        if(m.badPC)return failMath();
        p->position[2]=m.add(p->position[2],spreadZ,0x15e4e0);
        if(m.badPC)return failMath();
        p->size=f.initialSize;p->initialSize=f.initialSize;p->ownerId=owner;
        const auto quarter=m.integer(static_cast<std::int32_t>(serial&3u),0x15e510);
        p->lifeMilliseconds=m.sub(f.particleLife,m.mul(.125f,m.mul(f.particleLife,quarter,0x15e514),0x15e518),0x15e51c);
        if(m.badPC)return failMath();
        p->ageMilliseconds=m.mul(m.mul(.5f,p->lifeMilliseconds,0x15e528),birthFactor,0x15e52c);
        if(m.badPC)return failMath();
        const auto frame=m.add(m.integer(f.firstFrame,0x15e53c),m.mul(m.integer(set,0x15e494),f.framesPerSet,0x15e540),0x15e544);
        const auto selected=m.cvtWord(frame,0x15e548);if(m.badPC)return failMath();
        if(selected<f.firstFrame||selected>f.lastFrame)return fail(S::authoredData,0x15e554);
        p->frame=selected;p->inverseLife=m.div(1,p->lifeMilliseconds,0x15e55c);
        if(m.badPC)return failMath();
        p->growth=f.growth;
        if(f.growth==0)p->flags&=~8u; //15E578..584 before the following overwrite.
        p->flags=f.particleFlags;
        if(x>.5f&&f.randomMirrorX)p->flags|=0x4000u;else p->flags&=~0x4000u;
        if(y>.5f&&f.randomMirrorY)p->flags|=0x8000u;else p->flags&=~0x8000u;
        p->velocity=velocity;p->velocityW=1;
        if(f.applySpreadToVelocity){
            p->velocity[1]=m.add(p->velocity[1],m.mul(f.authoredVelocityY220,y,0x15e660),0x15e664);
            if(m.badPC)return failMath();
        }
        if(!manager.commitParticle(owner))return fail(S::storage,0x15e680);
        ++r.spawned;
    }
    return r;
}
} // namespace rr::game
