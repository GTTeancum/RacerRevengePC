#pragma once
#include "original_wreck_frame50la.h"

namespace rr::game {
// The real application14 debug object remains a separate, unjoined owner.
struct OriginalWreckDebugOwner50LA;
struct OriginalWreckDerivedGlobals50LA {
    std::uint8_t initializedGp8E24=0,initializedGp8E28=0;
    std::uint8_t initializedGp8E2C=0,initializedGp8E30=0;
    float pieceDustSpeed444290=0.f,pieceDustTime444298=0.f;
    float groupDustSpeed4442E0=0.f,groupDustTime4442E8=0.f;
};
struct OriginalWreckDerivedBinding50LA {
    OriginalWreckFrameBinding50LA base{};
    OriginalWreckDerivedGlobals50LA* globals=nullptr; //ONE application projection
    const std::uint32_t* actualParticleGate51C=nullptr; //actual app28C->74->51C
    //Missing field is unbound; a bound field whose value is null is the
    //genuine source-null debug branch. Never a caller-supplied skipped flag.
    const OriginalWreckDebugOwner50LA* const* actualApplication14Field=nullptr;
};
enum class OriginalWreckDerivedStatus50LA : std::uint8_t {
    complete,owner,base,binding,graph,arithmetic,effect,component,water,
    damageEmitterArrays,floorRegistry11AE30,debugOwner133A20,attachedGroupPieces
};
struct OriginalWreckDerivedResult50LA {
    OriginalWreckDerivedStatus50LA status=OriginalWreckDerivedStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool nativeReturnKnown=false,underworldDeactivated=false;
    std::int32_t nativeV0=0;
    std::size_t controlsCommitted=0,positionsCommitted=0;
    bool globalsCommitted=false,dustTimerCommitted=false;
    OriginalWreckFrameResult50LA base{};
    OriginalWreckEffectsReceipt50LA effect{};
    OriginalWreckComponentResult50LA component{};
    WaterHeightResult50JC water{};
};
namespace original_wreck_derived_detail50LA {
using S=OriginalWreckDerivedStatus50LA;using Slot=OriginalWreckEffectSlot50LA;
using R=OriginalWreckDerivedResult50LA;
inline bool fail(R& r,S status,std::uint32_t pc)noexcept {r.status=status;r.sourcePC=pc;return false;}
inline R done(R r,std::uint32_t pc)noexcept {
    r.status=S::complete;r.sourcePC=pc;r.nativeReturnKnown=true;r.nativeV0=r.base.nativeV0;return r;
}
inline bool numeric(float x)noexcept {return original_wreck_settling_detail50LA::admitted(x);}
inline bool arithmetic(R& r,std::uint32_t pc)noexcept {
    return std::fegetround()==FE_TONEAREST||fail(r,S::arithmetic,pc);
}
inline bool control(OriginalOnePieceWreckOwner50LA& o,Slot slot,unsigned method,R& r)noexcept {
    r.effect=method==0x80?o.effectOwners.start80(slot):method==0x84?
        o.effectOwners.stop84(slot):o.effectOwners.reset88(slot);
    if(!r.effect.okay())return fail(r,S::effect,r.effect.sourcePC);
    ++r.controlsCommitted;return true;
}
inline void position(EffectOwner50BE& e,const SpringVector4_48N& p,R& r)noexcept {
    std::uint32_t words[4];std::memcpy(words,p.v,16);positionEffect50BC(e.state,words);++r.positionsCommitted;
}
inline void rawPosition(EffectOwner50BE& e,const SpringVector4_48N& p,R& r)noexcept {
    std::memcpy(e.state.position30,p.v,16);++r.positionsCommitted;
}
inline bool world(OriginalOnePieceWreckOwner50LA& o,SceneNodeIndex49P node,
    SpringVector4_48N& p,R& r,std::uint32_t pc)noexcept {
    const auto g=o.assets.graph();
    if(!validateSceneGraph49P(g)||node==SceneNodeIndex49P::none||
       static_cast<std::uint32_t>(node)>=g.count||
       !original_wreck_flight_detail50LA::position(g,node,p))return fail(r,S::graph,pc);
    original_wreck_flight_detail50LA::projection(o.physical,g);return true;
}
// Real28C590 strcmp(name38,"damage00") over the SAME original decoded PSG
//names and canonical graph. The ordering is self, sibling subtree, child
//subtree. No marker boolean or supplied successful lookup is accepted.
inline bool anchor(OriginalOnePieceWreckOwner50LA& o,SceneNodeIndex49P& out,R& r)noexcept {
    const auto g=o.assets.graph();const auto& names=o.assets.scene().nodes;
    if(!validateSceneGraph49P(g)||names.size()!=g.count||
       o.physical.model288!=o.assets.model14C())return fail(r,S::graph,0x2cb93cu);
    SceneNodeIndex49P pending[maxSceneNodes49P];std::size_t count=0;
    pending[count++]=o.physical.model288;out=SceneNodeIndex49P::none;
    while(count){
        const auto i=pending[--count];const auto index=static_cast<std::uint32_t>(i);
        if(names[index].name=="damage00"){out=i;break;}
        const auto& n=g.nodes[index];
        if(n.firstChild184!=SceneNodeIndex49P::none)pending[count++]=n.firstChild184;
        if(n.nextSibling188!=SceneNodeIndex49P::none)pending[count++]=n.nextSibling188;
    }
    if(out==SceneNodeIndex49P::none)out=o.physical.model288; //2CB94C..2CB950
    return true;
}
inline SpringVector4_48N bodyPosition(const OriginalOnePieceWreckOwner50LA& o)noexcept {
    const auto& p=o.physical;return {{p.body.root.body.position.v[0],p.body.root.body.position.v[1],
        p.body.root.body.position.v[2],p.extra.position60W}};
}
inline bool water(OriginalOnePieceWreckOwner50LA& o,const SpringVector4_48N& position,
    float& y,R& r,std::uint32_t pc)noexcept {
    if(!o.actualWaterB0||!o.actualWaterGlobals)return fail(r,S::water,pc);
    float xyz[3];std::memcpy(xyz,position.v,12);
    r.water=queryWaterHeight50JC(*o.actualWaterB0,*o.actualWaterGlobals,xyz,nullptr);
    if(r.water.status!=WaterHeightStatus50JC::Complete)return fail(r,S::water,r.water.sourcePC);
    y=xyz[1];return true;
}
inline R groupReturn(const OriginalWreckDerivedBinding50LA& b,R r)noexcept {
    if(!b.actualApplication14Field){fail(r,S::binding,0x2cc8f0u);return r;}
    if(*b.actualApplication14Field){fail(r,S::debugOwner133A20,0x2cc8fcu);return r;}
    return done(r,0x2cc994u);
}
}

// Concrete2CB8F0 virtual38, count174=0. Calls the actual base once, then
//executes source controls directly on the canonical effect owners. Paused
//base return does NOT suppress this derived suffix. No particle child update
//is hidden here: those remain children of their actual external particle forest.
inline OriginalWreckDerivedResult50LA advanceOriginalWreckPiece2CB8F0_50LA(
    OriginalOnePieceWreckOwner50LA& o,const OriginalWreckDerivedBinding50LA& b,float dt)noexcept {
    using namespace original_wreck_derived_detail50LA;R r;
    if(!o.prepared()){fail(r,S::owner,0x2cb8f0u);return r;}
    r.base=advanceOriginalWreckPiece166E40_50LA(o,b.base,dt);
    if(r.base.status!=OriginalWreckFrameStatus50LA::complete){fail(r,S::base,r.base.sourcePC);return r;}
    if(!o.physical.body.enabled)return done(r,0x2cbe8cu);
    SpringVector4_48N point{{0.f,0.f,0.f,1.f}};SceneNodeIndex49P model=SceneNodeIndex49P::none;
    if(!anchor(o,model,r)||!world(o,model,point,r,0x2cb960u))return r;
    if(!numeric(point.v[1])){fail(r,S::arithmetic,0x2cb974u);return r;}
    if(point.v[1]<0.f){
        if(o.effects.count174){fail(r,S::damageEmitterArrays,0x2cbf80u);return r;}
        //2CBEB0 count0 skips ALL emitter-loop work, including168/16C stops.
        r.component=setOriginalWreckBaseActivity50LA(o.domain,o.pieceComponent(),false);
        if(r.component.status!=OriginalWreckComponentStatus50LA::complete){fail(r,S::component,r.component.sourcePC);return r;}
        r.underworldDeactivated=true;r=done(r,0x2cb998u);r.nativeV0=1;return r;
    }
    if(o.effects.count174){fail(r,S::damageEmitterArrays,0x2cbaa4u);return r;}
    auto* splash=o.effectOwners.control(Slot::splash16C);
    if(splash){
        if(o.piece.water3C||o.piece.water40){
            const float vy=o.physical.body.root.body.velocity.v[1];
            if(!numeric(vy)){fail(r,S::arithmetic,0x2cbae0u);return r;}
            if(vy<0.f){
                const auto p=bodyPosition(o);float surface=0.f;
                if(!water(o,p,surface,r,0x2cbb00u)||!arithmetic(r,0x2cbb1cu))return r;
                const float depth=original_wreck_settling_detail50LA::sub(surface,p.v[1]);
                if(!numeric(surface)||!numeric(p.v[1])||!numeric(depth)){fail(r,S::arithmetic,0x2cbb1cu);return r;}
                if(depth<10.f){
                    if(!(splash->state.flags1D4&2u)){
                        rawPosition(*splash,p,r); //piece raw30 precedes88
                        if(!control(o,Slot::splash16C,0x88,r))return r;
                    }
                    if(!control(o,Slot::splash16C,0x80,r))return r;
                    position(*splash,bodyPosition(o),r);
                }else if(!control(o,Slot::splash16C,0x84,r))return r;
            }else if(!control(o,Slot::splash16C,0x84,r))return r;
        }else if(!control(o,Slot::splash16C,0x84,r))return r;
    }
    //Actual app28C->74->51C is read before the nullable168 pointer.
    if(!b.actualParticleGate51C){fail(r,S::binding,0x2cbc70u);return r;}
    if(!*b.actualParticleGate51C)return done(r,0x2cbe88u);
    auto* dust=o.effectOwners.control(Slot::dust168);
    if(!dust)return done(r,0x2cbe88u);
    if(!b.globals){fail(r,S::binding,0x2cbc88u);return r;}
    auto& g=*b.globals;
    if(!g.initializedGp8E24){g.pieceDustSpeed444290=15.f;g.initializedGp8E24=1;}
    if(!g.initializedGp8E28){g.pieceDustTime444298=.5f;g.initializedGp8E28=1;}
    r.globalsCommitted=true;
    if(!world(o,o.physical.model288,point,r,0x2cbcd4u))return r;
    if(o.effects.dustActive178){
        const float old=o.effects.dustTime17C;
        if(!numeric(old)){fail(r,S::arithmetic,0x2cbcf4u);return r;}
        if(old<=0.f)return done(r,0x2cbe88u); //source leaves178 untouched
        if(!arithmetic(r,0x2cbd04u)||!numeric(dt)){fail(r,S::arithmetic,0x2cbd04u);return r;}
        const float next=original_wreck_settling_detail50LA::sub(old,dt);
        if(!numeric(next)){fail(r,S::arithmetic,0x2cbd04u);return r;}
        o.effects.dustTime17C=next;r.dustTimerCommitted=true;position(*dust,point,r);
        if(next<=0.f)o.effects.dustActive178=0;
        return done(r,0x2cbe88u);
    }
    //2CBD48 calls the real global37ADB8 name registry for literal FLOOR
    //BEFORE reading E0.F8 or material3B0. No fabricated floor-mask stamp,
    //contact count or miss can bypass this missing actual registry binding.
    fail(r,S::floorRegistry11AE30,0x2cbd48u);return r;
}

// Concrete2CC1B0 virtual38, actual capacity34=0/count174=0 owner. The full
//enabled water/splash and group sparks/explosion Stop84 routes are native;
//no attached fragment exists to reach dust/material or fracture emitters.
//A real nonnull application14 debug owner remains explicit before its writes.
inline OriginalWreckDerivedResult50LA advanceOriginalWreckGroup2CC1B0_50LA(
    OriginalOnePieceWreckOwner50LA& o,const OriginalWreckDerivedBinding50LA& b,float dt)noexcept {
    using namespace original_wreck_derived_detail50LA;R r;
    if(!o.prepared()){fail(r,S::owner,0x2cc1b0u);return r;}
    r.base=advanceOriginalWreckGroup2CA840_50LA(o,b.base,dt);
    if(r.base.status!=OriginalWreckFrameStatus50LA::complete){fail(r,S::base,r.base.sourcePC);return r;}
    if(!o.physical.body.enabled)return groupReturn(b,r);
    if(!b.globals){fail(r,S::binding,0x2cc1f8u);return r;}
    auto& g=*b.globals;
    if(!g.initializedGp8E2C){g.groupDustSpeed4442E0=25.f;g.initializedGp8E2C=1;}
    if(!g.initializedGp8E30){g.groupDustTime4442E8=.5f;g.initializedGp8E30=1;}
    r.globalsCommitted=true;
    if(o.group.capacity34){fail(r,S::attachedGroupPieces,0x2cc52cu);return r;}
    const auto graph=o.assets.graph();const auto model=o.physical.model288;
    if(!validateSceneGraph49P(graph)||model!=o.assets.model14C()){
        fail(r,S::graph,0x2cc544u);return r;}
    SpringVector4_48N local{{0.f,0.f,0.f,1.f}};
    std::memcpy(local.v,graph.nodes[static_cast<std::int32_t>(model)].localC0.v+12,12); //28B180
    auto* splash=o.effectOwners.control(Slot::splash16C);
    bool submerged=false;
    if(splash){
        SpringVector4_48N point{{0.f,0.f,0.f,1.f}};
        if(!world(o,o.assets.model14C(),point,r,0x2cc56cu))return r;
        if(o.actualWaterB0){
            float surface=0.f;if(!water(o,point,surface,r,0x2cc58cu))return r;
            if(!numeric(surface)||!numeric(point.v[1])){fail(r,S::arithmetic,0x2cc59cu);return r;}
            if(surface<point.v[1]){if(!control(o,Slot::splash16C,0x84,r))return r;}
            else{
                if(!arithmetic(r,0x2cc5acu))return r;
                const float depth=original_wreck_settling_detail50LA::sub(surface,point.v[1]);
                if(!numeric(depth)){fail(r,S::arithmetic,0x2cc5acu);return r;}
                submerged=true; //2CC5C8, including depth>=10 route
                if(depth<10.f){
                    if(!(splash->state.flags1D4&2u)){
                        if(!control(o,Slot::splash16C,0x88,r))return r;
                        rawPosition(*splash,point,r); //group88 precedes raw30
                    }
                    if(!control(o,Slot::splash16C,0x80,r))return r;
                    position(*splash,point,r);
                }
                if(o.effects.count174){fail(r,S::damageEmitterArrays,0x2cc698u);return r;}
            }
        }else if(!control(o,Slot::splash16C,0x84,r))return r;
    }
    if(!submerged&&o.effects.count174){fail(r,S::damageEmitterArrays,0x2cc7e8u);return r;}
    auto* sparks=o.effectOwners.control(Slot::sparksC0);
    if(sparks){
        auto* explosion=o.effectOwners.control(Slot::explosionC4);
        if(explosion){
            //Regardless group38, capacity0 makes the break-slot loop empty;
            //source a0 stays0 and both actual controls take84, never88/80.
            if(!control(o,Slot::sparksC0,0x84,r)||!control(o,Slot::explosionC4,0x84,r))return r;
        }
    }
    return groupReturn(b,r);
}
} // namespace rr::game
