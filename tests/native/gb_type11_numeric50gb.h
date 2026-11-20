#pragma once
// TypeID 11 (firework) derived emission, source 15F3C0..15FBC8, kept from the GB
// lane and re-pointed (common rewrite, area E) over the one track emitter owner's
// own definition type (track_particle_emitters50CR, common_track_emitters50cr.cpp).
// Every numeric line and source pc is unchanged. The GB Instance wrapper (its
// 62..67 attachment pins, descriptor pins, atlas pins and trigger-group coupling)
// is gone: that lifecycle (registration, StartOn, trigger TurnOn, common 15A7A0,
// completion gate source, fail latch) now lives in the owner, for every course.
//
// Definition: any row carrying the 15AEA0 common fields plus the 15FC10 Type11
// fields (minStreamLength200, maxStreamLength204, particleSeparation208,
// tailLife20C, tailThickness210, headThickness214, superbrightHead218,
// movingEmitter21C, minRange, maxRange). The owner proves the row is TypeID 11.
// A failed receipt keeps the completed source/RNG/particle prefix; the caller
// latches it (never retries).
#include "../../src/game/original_gc_pod_sparks50kg.h"
#include "../../src/game/original_gc_particle_manager50kg.h"
#include "../../src/game/water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace gb_type11_numeric50GB {
namespace g=rr::game;
using Status=g::OriginalGcSprayStatus50KG;
using Receipt=g::OriginalGcSprayReceipt50KG;
// Computed only by the enclosing owner from its completion source for an attached
// placement (an unattached placement has no 15F3F0 attachment gate: Current).
// It is not an original 16-bit portal-tag receipt.
enum class PlacementCompletion : std::uint8_t {Unavailable,Stale,Current};
namespace detail {
using Math=g::water_wake_detail50jc::Math;
inline float word(std::uint32_t x)noexcept {float f;std::memcpy(&f,&x,4);return f;}
inline float root(Math& m,float x,std::uint32_t pc)noexcept {m.read(x,pc);volatile float r=std::sqrt(x);return m.read(r,pc);}
}

// Exact 15F3C0 onward, only AFTER one genuine 15A7A0 returned zero (commonSkipped
// false). currentFrame_/firstFrame_/lastFrame_ are the placement's 190/1B0/1B4,
// carriedSize18C_ its 18C, managerOwnerId_ its registered manager row; globals is
// the process BSS 84D0/433980 shared by every ordinary emitter.
template<class Definition>
inline Receipt afterCommon(const Definition& f,g::OriginalGcParticleManager50KG& manager,
 std::uint32_t managerOwnerId_,g::EffectControlState50BC& controls_,std::int32_t& currentFrame_,
 std::int32_t firstFrame_,std::int32_t lastFrame_,float& carriedSize18C_,float dt,
 PlacementCompletion completion,std::uint32_t& raw,g::OriginalGcPodSparksGlobals50KG& globals)noexcept {
    using detail::Math;using detail::word;using detail::root;
    Receipt r;r.status=Status::complete;r.sourcePC=0x15fbc8;Math m;
    auto fail=[&](Status s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    auto arithmetic=[&](){return fail(Status::arithmetic,m.badPC);};
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x15f3c0);
    m.read(dt,0x15f450);if(m.badPC)return arithmetic();
    if(!(dt>0)||dt>1.f)return fail(Status::unsupported,0x15f450);
    auto& c=controls_;
    const auto timer=m.read(word(c.word19C),0x15f3c0);if(m.badPC)return arithmetic();
    if(timer>.25f){std::memcpy(c.previous40,c.position30,16);return r;}
    // The approved native completed-placement gate belongs to the owner's
    // completion source. It is checked AFTER the source timer shortcut.
    // This private scalar input is not a public original-tag authority.
    if(completion==PlacementCompletion::Unavailable)return fail(Status::unprepared,0x15f3f0);
    if(completion==PlacementCompletion::Stale){
        carriedSize18C_=f.initialSize184;c.word19C=0;r.sourcePC=0x15fbb8;return r;}
    if(completion!=PlacementCompletion::Current)return fail(Status::unprepared,0x15f3f0);
    const float emission=m.mul(f.emitRate188,dt,0x15f450);
    if(m.badPC)return arithmetic();
    if(emission<0||emission>=2147483648.f)return fail(Status::unsupported,0x15fba4);
    float velocity[3],position[3];for(unsigned i=0;i<3;++i){
        velocity[i]=m.read(word(c.velocity60[i]),0x15f454u+4u*i);
        position[i]=m.read(word(c.position30[i]),0x15f5c8u+8u*i);}
    float speedFactor=1;
    if(f.movingEmitter21C){
        const auto square=m.add(m.add(m.mul(velocity[0],velocity[0],0x15f464),
            m.mul(velocity[1],velocity[1],0x15f468),0x15f46c),m.mul(velocity[2],velocity[2],0x15f470),0x15f470);
        const auto speed=root(m,square,0x15f474);
        if(!globals.lazy84D0){globals.speedScale433980=20.f;globals.lazy84D0=true;}
        const auto scale=m.read(globals.speedScale433980,0x15f4a0);
        if(speed<scale)speedFactor=m.div(speed,scale,0x15f4bc);
    }
    if(m.badPC)return arithmetic();
    auto draw=[&](){++r.randomDraws;return g::drawOriginalGcParticleRandom50KG(raw);};
    auto finish=[&](){carriedSize18C_=f.initialSize184;c.word19C=0;return r;};
    // Source compares CVT.S(s0) directly to rate*dt: positive fractional
    // demand runs one last stream; do not truncate it as a particle count.
    for(std::int32_t stream=0;m.integer(stream,0x15fba0)<emission;++stream){
        const float randomX=draw(),randomY=draw(),randomZ=draw();
        const float random[3]{randomX,randomY,randomZ};float spread[3];
        for(unsigned axis=0;axis<3;++axis)spread[axis]=m.add(f.minRange[axis],
            m.mul(random[axis],m.sub(f.maxRange[axis],f.minRange[axis],0x15f550u+28u*axis),
                0x15f554u+28u*axis),0x15f558u+28u*axis);
        float point[3],particleVelocity[3];
        for(unsigned axis=0;axis<3;++axis){point[axis]=m.sub(position[axis],m.mul(dt,velocity[axis],
            0x15f5b8u+4u*axis),0x15f5e4u+4u*axis);particleVelocity[axis]=m.add(velocity[axis],spread[axis],0x15f6a8u+4u*axis);}
        const float growth=m.mul(f.growth180,draw(),0x15f630);
        const float life=m.mul(f.particleLife17C,draw(),0x15f67c);
        (void)m.div(1,life,0x15f694); // Source computes before the first capacity guard.
        std::uint32_t flags=f.particleFlags198;
        if(f.randomMirrorX&&randomX>.5f)flags|=0x4000;else flags&=~0x4000u;
        if(f.randomMirrorY&&randomY>.5f)flags|=0x8000;else flags&=~0x8000u;
        const auto squared=m.add(m.add(m.mul(particleVelocity[0],particleVelocity[0],0x15f780),
            m.mul(particleVelocity[1],particleVelocity[1],0x15f784),0x15f788),
            m.mul(particleVelocity[2],particleVelocity[2],0x15f78c),0x15f78c);
        float step[3]{};
        if(squared!=0){const auto inverse=m.div(1,root(m,squared,0x15f7bc),0x15f7bc);
            for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(particleVelocity[axis],inverse,0x15f7ccu+12u*axis);}
        const auto separation=m.mul(f.particleSeparation208,f.initialSize184,0x15f7fc);
        for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(step[axis],separation,0x15f800u+12u*axis);
        const float lengthRandom=draw();float length;
        if(f.movingEmitter21C){const auto spreadLength=m.mul(m.sub(f.maxStreamLength204,f.minStreamLength200,0x15f85c),lengthRandom,0x15f860);
            length=m.add(.5f,m.add(1.f,m.mul(speedFactor,m.add(m.sub(f.minStreamLength200,1.f,0x15f864),
                spreadLength,0x15f868),0x15f86c),0x15f870),0x15f87c);
        }else length=m.add(.5f,m.add(f.minStreamLength200,m.mul(m.sub(f.maxStreamLength204,f.minStreamLength200,
            0x15f8bc),lengthRandom,0x15f8c0),0x15f8c4),0x15f8c8);
        const auto sections=m.cvtWord(length,f.movingEmitter21C?0x15f880u:0x15f8ccu);
        const auto sectionFloat=m.integer(sections,0x15f8e0);
        if(m.badPC)return arithmetic();
        if(sections<=0)return fail(Status::unsupported,0x15f91c);
        const auto reciprocal=m.div(1,sectionFloat,0x15f91c);
        for(unsigned axis=0;axis<3;++axis)particleVelocity[axis]=m.add(particleVelocity[axis],
            m.mul(sectionFloat,step[axis],0x15f8f8u+4u*axis),0x15f928u+12u*axis);
        for(std::int32_t sectionIndex=0;sectionIndex<sections;++sectionIndex){
            const auto thickness=m.add(.5f,m.add(f.tailThickness210,m.mul(reciprocal,m.mul(
                m.integer(sectionIndex,0x15f95c),m.sub(f.headThickness214,f.tailThickness210,0x15f960),0x15f964),
                0x15f968),0x15f96c),0x15f978);
            auto copies=m.cvtWord(thickness,0x15f97c);
            if(sectionIndex==sections-1&&f.superbrightHead218){if(copies==std::numeric_limits<std::int32_t>::max())
                return fail(Status::unsupported,0x15f99c);++copies;}
            const auto ratio=m.div(m.integer(sectionIndex+1,0x15f9bc),sectionFloat,0x15f9c0);
            const float taper=std::max(word(0x3dcccccdu),ratio); //MAX.S15F9CC, not POW.
            const auto complement=m.sub(1,taper,0x15f9f4);
            if(m.badPC)return arithmetic();
            for(std::int32_t copy=0;copy<copies;++copy){
                auto* p=manager.nextParticle();if(!p){r.poolFull=true;return finish();}
                p->ageMilliseconds=0;p->position={point[0],point[1],point[2]};p->positionW=1;
                p->frame=currentFrame_;p->growth=growth;p->size=p->initialSize=f.initialSize184;p->ownerId=managerOwnerId_;
                //15FA48's zero-growth flag clear is overwritten unconditionally
                // by15FA9C. Do not propagate stale free-slot flags into a particle.
                p->lifeMilliseconds=m.sub(life,m.mul(complement,m.mul(f.tailLife20C,life,0x15fa74),0x15fa78),0x15fa7c);
                p->inverseLife=m.div(1,p->lifeMilliseconds,0x15fa88);
                p->velocity={particleVelocity[0],particleVelocity[1],particleVelocity[2]};p->velocityW=1;p->flags=flags;
                if(m.badPC)return arithmetic();
                if(!manager.commitParticle(managerOwnerId_))return fail(Status::storage,0x15fab0);
                ++r.spawned;
            }
            const auto taperSquared=m.mul(taper,taper,0x15fae0);
            particleVelocity[0]=m.sub(particleVelocity[0],m.mul(step[0],taperSquared,0x15faf4),0x15faf8);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(complement,step[1],0x15fb04),0x15fb08);
            particleVelocity[2]=m.sub(particleVelocity[2],m.mul(step[2],taperSquared,0x15fb14),0x15fb18);
            particleVelocity[1]=m.sub(particleVelocity[1],m.mul(step[1],taper,0x15fb24),0x15fb28);
            for(unsigned axis=0;axis<3;++axis)point[axis]=m.add(point[axis],step[axis],0x15fb34u+12u*axis);
            if(m.badPC)return arithmetic();
        }
        if(currentFrame_==std::numeric_limits<std::int32_t>::max())return fail(Status::unsupported,0x15fb68);
        ++currentFrame_;if(currentFrame_>lastFrame_)currentFrame_=firstFrame_;
    }
    if(m.badPC)return arithmetic();return finish();
}
} // namespace gb_type11_numeric50GB
