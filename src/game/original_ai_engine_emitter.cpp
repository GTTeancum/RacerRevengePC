#include "original_ai_engine_emitter.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using E=OriginalAiEngineEmitter;using V=OriginalAiEmitterVector;
using S=OriginalAiEmitterStatus;using R=OriginalAiEmitterReceipt;
float bits(std::uint32_t word) noexcept {float f;std::memcpy(&f,&word,4);return f;}
bool scalar(float value) noexcept {std::uint32_t u;std::memcpy(&u,&value,4);
    const auto e=u&0x7f800000u;return e!=0x7f800000u&&(e!=0||(u&0x7fffffu)==0);}
bool identity(const OriginalAiEmitterIdentity& i) noexcept {
    return i.session&&i.racer&&i.mainBody&&i.emitterD40&&i.managerOrdinal<8&&i.pod60>0&&i.pod60<=23;
}
bool same(const OriginalAiEmitterIdentity& a,const OriginalAiEmitterIdentity& b) noexcept {
    return a.session==b.session&&a.racer==b.racer&&a.mainBody==b.mainBody&&
        a.emitterD40==b.emitterD40&&a.managerOrdinal==b.managerOrdinal&&a.pod60==b.pod60;
}
bool sound(const OriginalAiEmitterSound* s,std::uint64_t session) noexcept {
    return s&&s->session==session&&s->soundKey&&s->multiplier54&&s->position60;
}
void copy(V& out,const V& in) noexcept {std::memcpy(&out,&in,sizeof out);}
R fail(E& e,R r,S status,std::uint32_t pc) noexcept {e.blocked=true;r.status=status;r.sourcePC=pc;return r;}
bool spatial(E& e,OriginalAiEmitterSpatial& target,const V& position,const V& velocity,R& r) noexcept {
    if(target.session!=e.identity.session||!target.emitterKey)return false;
    //291620 mutates these before examining1F0, including if it is null.
    copy(target.previous40,target.position170);copy(target.position170,position);
    copy(target.velocity180,velocity);++r.spatialWrites;
    if(!target.sound1F0)return true;
    if(!sound(target.sound1F0,e.identity.session))return false;
    //232DB0 constructs232ED0's exact3D1BB0 logical sound. Virtual3C is
    //233DB0: scalar copies XYZ only. Virtual40 is233DF0: returns0, no write.
    std::memcpy(target.sound1F0->position60,target.position170.v,3*sizeof(float));
    ++r.positionSoundCalls;++r.velocityNoopCalls;return true;
}
bool pitch(OriginalAiEmitterSound* target,std::uint64_t session,float speed,
    float range,float idle,R& r) noexcept {
    if(!sound(target,session)||!scalar(speed)||!scalar(range)||!scalar(idle))return false;
    volatile float product=speed*range;if(!scalar(product))return false;
    const float value=detail::scalarMax48O(product,idle);++r.pitchCalls;
    //233A80 returns false outside the range;16DA80 ignores this source V0.
    if(value>=0&&value<=bits(0x467ffc00u)){std::memcpy(target->multiplier54,&value,4);++r.pitchWrites;}
    return true;
}
}

bool projectFreshOriginalAiEngineEmitter(E& out,const OriginalAiEmitterIdentity& id) noexcept {
    if(out.constructed||!identity(id))return false;
    E next;next.identity=id;next.constructed=true;
    next.spatial.session=id.session;next.spatial.emitterKey=id.emitterD40;
    const V sentinel{{bits(0x7f7fffffu),bits(0x7f7fffffu),bits(0x7f7fffffu),1}};
    copy(next.spatial.previous40,sentinel);copy(next.spatial.position170,sentinel);
    //290FF4 memset160..1CF overwrites the earlier vf0 velocity180 with four0.
    next.spatial.velocity180={{0,0,0,0}};
    next.flags22CLow4=0;
    //290F10 already wrote158=0;16D930/34 keeps only bit3, still0.
    next.spatial.flags158=0;next.vector4A0=next.vector4B0=next.vector4D0=V{{0,0,0,1}};
    next.distance4EC=800;out=next;return true;
}

bool projectFreshOriginalAiSecondaryEmitter(OriginalAiEmitterSpatial& out,
    std::uint64_t session,std::uint64_t key) noexcept {
    if(out.session||out.emitterKey||!session||!key)return false;
    OriginalAiEmitterSpatial next;next.session=session;next.emitterKey=key;
    const V sentinel{{bits(0x7f7fffffu),bits(0x7f7fffffu),bits(0x7f7fffffu),1}};
    copy(next.previous40,sentinel);copy(next.position170,sentinel);
    next.velocity180={{0,0,0,0}};out=next;return true;
}

bool projectOriginalAiEmitterSpatialSetup2911E0(E& e,OriginalAiEmitterSpatial& secondary) noexcept {
    if(!e.constructed||e.prepared||e.blocked||e.inUpdate||e.spatial.setupApplied||
       secondary.setupApplied||secondary.session!=e.identity.session||!secondary.emitterKey||
       secondary.emitterKey==e.identity.emitterD40)return false;
    copy(e.spatial.position170,e.vector4A0);copy(e.spatial.velocity180,e.vector4B0);
    e.spatial.flags158=2;e.spatial.setupApplied=true;
    e.spatial.near1B8=bits(e.word4E8);e.spatial.far1BC=e.distance4EC;
    e.spatial.trigger1DC=20.f;e.spatial.chance1E0=100.f;
    e.spatial.flags1D4Low5=static_cast<std::uint8_t>(e.spatial.flags1D4Low5&~8u);
    copy(secondary.position170,e.vector4A0);copy(secondary.velocity180,e.vector4B0);
    secondary.flags158=8;secondary.setupApplied=true;
    secondary.near1B8=bits(e.word4E8);secondary.far1BC=e.distance4EC;
    secondary.trigger1DC=20.f;secondary.chance1E0=100.f;
    secondary.flags1D4Low5=static_cast<std::uint8_t>((secondary.flags1D4Low5|4u)&~8u);return true;
}

bool bindOriginalAiEngineEmitterAfterSetup(E& e,OriginalAiEmitterSound& engine,
    OriginalAiEmitterSpatial& secondary,const float& idle,const float& range,
    const float& binderIdle,const float& binderRange,OriginalAiEmitterSound* boost) noexcept {
    if(!e.constructed||e.prepared||e.blocked||e.inUpdate||!identity(e.identity)||
       !e.spatial.setupApplied||!secondary.setupApplied||
       !sound(&engine,e.identity.session)||secondary.session!=e.identity.session||
       !secondary.emitterKey||secondary.emitterKey==e.identity.emitterD40||
       !sound(secondary.sound1F0,e.identity.session)||
       (boost&&!sound(boost,e.identity.session))||
       !scalar(idle)||!scalar(range)||!scalar(binderIdle)||!scalar(binderRange))return false;
    e.engine224=&engine;e.spatial.sound1F0=&engine;e.secondary480=&secondary;e.boost228=boost;
    e.idle450=&idle;e.range460=&range;e.idle458=&binderIdle;e.range468=&binderRange;
    e.prepared=true;return true;
}

bool readOriginalAiEmitterRacerInput(const OriginalAiEmitterIdentity& id,
    const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    float ratio,OriginalAiEmitterRacerInput& out) noexcept {
    const auto node=static_cast<std::int32_t>(bindings.animation.bodyScene288);
    if(!identity(id)||(frame.phase!=VehicleFramePhase49W::AfterBodyChildren&&
       frame.phase!=VehicleFramePhase49W::BeforeGraphics)||
       frame.fields.vehicle58!=1u||node<0||static_cast<std::uint32_t>(node)>=frame.sceneNodeCount||
       frame.sceneNodeCount>maxSceneNodes49P)return false;
    OriginalAiEmitterRacerInput next;next.identity=id;next.phase=frame.phase;next.localPosition={{0,0,0,1}};
    std::memcpy(next.localPosition.v,frame.nodes[static_cast<std::uint32_t>(node)].localC0.v+12,12);
    std::memcpy(next.bodyVelocity70.v,frame.bodies.main.components.root.body.velocity.v,12);
    std::memcpy(next.bodyVelocity70.v+3,&frame.bodies.mainSphere.velocity70W,4);
    next.speedRatio70=ratio;next.elapsed=frame.bodies.main.frameFactor;out=next;return true;
}

bool bindOriginalAiNativeSound(OriginalAiEmitterSound& out,std::uint64_t session,
    std::uint64_t key,platform::LogicalAudioOwner& owner,std::size_t ordinal) noexcept {
    auto* actual=owner.sound(ordinal);
    if(out.session||out.soundKey||!session||!key||!actual)return false;
    out.session=session;out.soundKey=key;out.multiplier54=&actual->multiplier54;
    out.position60=actual->position60.data();out.nativeOwner=&owner;out.nativeSound=actual;
    return true;
}

R applyOriginalAiEmitterLifecycle(E& e,OriginalAiEmitterLifecycle operation) noexcept {
    R r;if(e.blocked){r.status=S::blocked;return r;}
    if(!e.constructed||!e.prepared||e.inUpdate||!identity(e.identity))return r;
    using L=OriginalAiEmitterLifecycle;
    if(operation!=L::startEngine16DB80&&operation!=L::stopEngine16DBC0&&
       operation!=L::startBinder16DC00&&operation!=L::stopBinder16DC10)return r;
    e.inUpdate=true;struct Leave {E& e;~Leave(){e.inUpdate=false;}} leave{e};
    auto enable=[](OriginalAiEmitterSpatial& s) noexcept {
        s.word1EC=1;s.word1ECKnown=true; //291A10, no sound start call.
    };
    auto stop=[&](OriginalAiEmitterSpatial& s) noexcept {return stopOriginalAiSpatial291A20(e,s);};
    switch(operation){
    case L::startEngine16DB80:
        enable(e.spatial);e.flags22CLow4=static_cast<std::uint8_t>(e.flags22CLow4|1u);
        r.sourcePC=0x16dba8u;break;
    case L::stopEngine16DBC0:
        if(!stop(e.spatial))return fail(e,r,S::soundLifecycle,0x2917f4u);
        e.flags22CLow4=static_cast<std::uint8_t>(e.flags22CLow4&~1u);
        r.sourcePC=0x16dbe8u;break;
    case L::startBinder16DC00:
        e.word504=1; // Source writes before dereferencing480.
        if(!e.secondary480||e.secondary480->session!=e.identity.session||
           !e.secondary480->emitterKey)return fail(e,r,S::secondaryOwner,0x16dc08u);
        enable(*e.secondary480);r.sourcePC=0x291a18u;break;
    case L::stopBinder16DC10:
        if(e.secondary480&&!stop(*e.secondary480))return fail(e,r,S::soundLifecycle,0x2917f4u);
        e.word504=0;r.sourcePC=0x16dc34u;break;
    }
    r.status=S::complete;return r;
}

bool stopOriginalAiSpatial291A20(E& e,OriginalAiEmitterSpatial& s) noexcept {
    if(!e.constructed||e.blocked||s.session!=e.identity.session||!s.emitterKey)return false;
    s.word1EC=0;s.word1ECKnown=true;
    s.flags1D4Low5=static_cast<std::uint8_t>(s.flags1D4Low5&~16u);
    if(!s.sound1F0)return true;
    s.word1E8=0;s.word1E8Known=true;
    auto& sound=*s.sound1F0;
    if(sound.session!=e.identity.session||!sound.soundKey||!sound.nativeOwner||!sound.nativeSound||
       sound.multiplier54!=&sound.nativeSound->multiplier54||
       sound.position60!=sound.nativeSound->position60.data())return false;
    return sound.nativeOwner->stop233670(*sound.nativeSound)==platform::LogicalAudioOwner::Result::complete;
}

R advanceOriginalAiEngineEmitter16DA80(E& e,const OriginalAiEmitterRacerInput& in,
    const OriginalAiEmitterChildPrelude& prelude,std::size_t maximumChildren) noexcept {
    R r;if(e.blocked){r.status=S::blocked;return r;}
    if(!e.constructed||!e.prepared||e.inUpdate||!same(e.identity,in.identity)||
       in.phase!=VehicleFramePhase49W::AfterBodyChildren||
       !identity(e.identity)||!maximumChildren||
       e.completedUpdates==std::numeric_limits<std::uint64_t>::max())return r;
    if(std::fegetround()!=FE_TONEAREST||!scalar(in.elapsed)||in.elapsed<0){r.status=S::arithmetic;return r;}
    e.inUpdate=true;struct Leave {E& e;~Leave(){e.inUpdate=false;}} leave{e};
    //16DEE0 first tests2915B0 (primary1F0), independent of22C pitch flags.
    if(e.spatial.sound1F0){
        if(!spatial(e,e.spatial,in.localPosition,in.bodyVelocity70,r))
            return fail(e,r,S::spatialSound,0x16df30u);
        //Source calls291620 on480 unconditionally in this taken branch.
        if(!e.secondary480)return fail(e,r,S::secondaryOwner,0x16df48u);
        if(!spatial(e,*e.secondary480,in.localPosition,in.bodyVelocity70,r))
            return fail(e,r,S::spatialSound,0x16df48u);
    }
    if(!(e.flags22CLow4&2u)&&e.engine224&&(e.flags22CLow4&1u)){
        if(!e.idle450||!e.range460)return fail(e,r,S::owner,0x16dad0u);
        if(!pitch(e.engine224,e.identity.session,in.speedRatio70,*e.range460,*e.idle450,r))
            return fail(e,r,S::arithmetic,0x16daf0u);
    }
    if(!(e.flags22CLow4&2u)&&e.secondary480&&e.word504){
        if(!e.idle458||!e.range468)return fail(e,r,S::owner,0x16db2cu);
        if(!pitch(e.secondary480->sound1F0,e.identity.session,in.speedRatio70,*e.range468,*e.idle458,r))
            return fail(e,r,S::arithmetic,0x16db4cu);
    }
    //178B00 tail-jumps141ED0. The explicit bound is a native admission limit;
    //no source child is omitted or declared successful because it is absent.
    if(e.childMask24&2u){
        auto* child=e.child18;std::size_t visited=0;
        while(child){
            if(visited++==maximumChildren)return fail(e,r,S::childCycleOrCapacity,0x141efcu);
            if(!child->objectKey)return fail(e,r,S::childOwner,0x141efcu);
            const auto flags=child->flags29;
            if((flags&1u)&&!(flags&0x0cu)&&(child->mask20&2u)){
                if(!prelude.beforeChild||!prelude.beforeChild(prelude.context))
                    return fail(e,r,S::childPrelude,0x141f4cu);
                if(!child->update38)return fail(e,r,S::childOwner,0x141f60u);
                if(!child->update38(child->context,in.elapsed))return fail(e,r,S::childUpdate,0x141f60u);
                ++r.childCalls;
            }
            child=child->next14;
        }
    }
    ++e.completedUpdates;r.status=S::complete;r.sourcePC=0x16db6cu;return r;
}

R advanceOriginalAiThermalWithD40(E& e,const OriginalAiEmitterRacerInput& frame,
    const BoostInput50FK& in,const BoostParameters50FK& parameters,BoostState50FK& actual,
    const OriginalAiEmitterBoostBackend& backend,BoostDiagnostic50FK& d) noexcept {
    R r;d={BoostError50FK::None,0};
    if(e.blocked){r.status=S::blocked;return r;}
    if(!e.constructed||!e.prepared||e.inUpdate||!same(e.identity,frame.identity)||
       frame.phase!=VehicleFramePhase49W::BeforeGraphics||in.vehicle58!=1u||!in.d40Present||
       std::memcmp(&in.frameFactor,&frame.elapsed,4)||!e.range460||!e.range468)return r;
    BoostState50FK next;
    if(!projectBoostThermalNumbers50FK(in,parameters,actual,next,d)){r.status=S::arithmetic;r.sourcePC=d.sourceStage;return r;}
    const auto old64=actual.vehicle64;const bool held=(in.flagsB9C&1u)&&!(in.flagsB9C&2u);
    float heating=0;
    if(held){volatile float sum=in.engine74+in.engine70;volatile float h=parameters.heatRate88*sum;heating=h;}
    e.inUpdate=true;struct Leave {E& e;~Leave(){e.inUpdate=false;}} leave{e};
    actual.vehicle68=old64;
    if(held){if(!(heating<=0))actual.vehicle64=1;}else actual.vehicle64=0;
    const bool onset=held&&heating>0&&old64==0,release=!held&&old64!=0;
    if(onset||release){
        auto literal=[&](OriginalAiEmitterSound* target,float range)->bool {
            if(!sound(target,e.identity.session)||!scalar(range))return false;
            float value=range;
            if(onset){volatile float added=bits(0x3e99999au)+range;value=added;if(!scalar(value))return false;}
            ++r.pitchCalls;
            if(value>=0&&value<=bits(0x467ffc00u)){std::memcpy(target->multiplier54,&value,4);++r.pitchWrites;}
            return true;
        };
        if(e.engine224&&!literal(e.engine224,*e.range460))return fail(e,r,S::arithmetic,onset?0x16dc7cu:0x16dd0cu);
        if(e.secondary480&&!literal(e.secondary480->sound1F0,*e.range468))
            return fail(e,r,S::arithmetic,onset?0x16dca4u:0x16dd24u);
        if(onset){
            //16DCB8 slotA8=16E200->179070, event4. The nonnull racer220 is
            //already the identity-bound current lease. No material1/2 lookup.
            if(e.boost228){
                if(!sound(e.boost228,e.identity.session)||!backend.attach104140)
                    return fail(e,r,S::boostAttach,0x179284u);
                bool returned=false;
                if(!backend.attach104140(backend.context,*e.boost228,frame.localPosition,
                    frame.bodyVelocity70,0,450,returned))return fail(e,r,S::boostAttach,0x179284u);
                if(returned&&(!backend.start233410||
                    !backend.start233410(backend.context,*e.boost228,5)))return fail(e,r,S::boostStart,0x1792a0u);
            }
            e.flags22CLow4=static_cast<std::uint8_t>(e.flags22CLow4|2u);
        }else e.flags22CLow4=static_cast<std::uint8_t>(e.flags22CLow4&~2u);
    }
    //Numeric result was staged from the same unmodified source operands.
    //Backend implementations must not mutate racer thermal operands/owners.
    actual=next;r.status=S::complete;r.sourcePC=0x217930u;return r;
}
}
