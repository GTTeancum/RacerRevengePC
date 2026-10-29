#include "original_gc_pod_sparks50kg.h"
#include "water_wake50jc.h"
#include <algorithm>
#include <cmath>
#include <cfenv>
#include <cstring>
#include <limits>
namespace rr::game { namespace {
using Status=OriginalGcSprayStatus50KG;
using Receipt=OriginalGcSprayReceipt50KG;
using Section=assets::OriginalUiDatSection50KG;
using Math=water_wake_detail50jc::Math;
Receipt result(Status status,std::uint32_t pc) noexcept {Receipt r;r.status=status;r.sourcePC=pc;return r;}
float word(std::uint32_t v) noexcept {float f;std::memcpy(&f,&v,4);return f;}
const Section& section(const std::vector<Section>& doc,const char* name,const Section& empty) noexcept {
    for(const auto& s:doc)if(s.name==name)return s;return empty;
}
float root(Math& m,float x,std::uint32_t pc) noexcept {m.read(x,pc);volatile float r=std::sqrt(x);return m.read(r,pc);}
}
OriginalGcSprayStatus50KG readOriginalGcPodSparksDefinition50KG(const std::vector<std::uint8_t>& bytes,
    OriginalGcPodSparksDefinition50KG& out,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    return readOriginalGcPodSparksDefinition50KG(bytes,OriginalGcPodSparksMember50KG::podSparks,out,d);
}
OriginalGcSprayStatus50KG readOriginalGcPodSparksDefinition50KG(const std::vector<std::uint8_t>& bytes,
    OriginalGcPodSparksMember50KG member,OriginalGcPodSparksDefinition50KG& out,
    assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};try {
        std::vector<Section> doc;
        const auto ordinal=static_cast<unsigned>(member);
        constexpr std::size_t sizes[]={1075,1369,1386};
        constexpr const char* hashes[]={
            "0a8e6c18e9f98992770f7046eea11a7541f286c387977aedcbbf277026d180d8",
            "036894066d5a9a50df9da7a4eb596bc35d9528097245de6d62cf61b07e99ac78",
            "0f7bd127076073b0d2d282614a4f5b23721d1d2368119182e08f8b0f143d1d46"};
        if(ordinal>=3||!readOriginalGcParticleText50KG(bytes,sizes[ordinal],hashes[ordinal],doc))return Status::assetIdentity;
        const Section empty{};const auto& g=section(doc,"General Info",empty);
        const auto& p=section(doc,"Properties",empty);const auto& a=section(doc,"Animation",empty);
        OriginalGcPodSparksDefinition50KG f;f.member=member;f.groundLevel=std::numeric_limits<float>::max();
        auto number=[&](const Section& s,const char* k,float fallback,float& v,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,k,fallback,v,d,pc);};
        if(!number(g,"MinStreamLength",3,f.minStreamLength,0x15fc50)||
           !number(g,"MaxStreamLength",5,f.maxStreamLength,0x15fc6c)||
           !number(g,"TailThickness",1,f.tailThickness,0x15fc88)||
           !number(g,"HeadThickness",2,f.headThickness,0x15fca4)||
           !number(g,"ParticleSeparationFactor",1.5f,f.separation,0x15fcc0)||
           !number(g,"TailLifeModifier",word(0x3f19999au),f.tailLife,0x15fce0)||
           !assets::originalUiBoolean50KG(g,"SuperbrightHead",true,f.superbrightHead,d,0x15fcf8)||
           !assets::originalUiTriple50KG(g,"MinRange",{},f.minRange,d,0x15fd14)||
           !assets::originalUiTriple50KG(g,"MaxRange",{},f.maxRange,d,0x15fd30)||
           !assets::originalUiBoolean50KG(g,"MovingEmitter",false,f.moving,d,0x15fd48)||
           !assets::originalUiInteger50KG(g,"ParticleGroup",-1,f.group,d,0x15aee0)||
           !number(g,"ParticleLife",1000,f.life,0x15af70)||
           !number(g,"InitialSize",1,f.size,0x15af8c)||
           !number(g,"Growth",0,f.growth,0x15afa4)||
           !number(g,"EmitRate",500,f.rate,0x15afc0)||
           !number(g,"AgeAtBirth",0,f.birthAge,0x15afd8)||
           !assets::originalUiTriple50KG(g,"Velocity",{},f.velocity,d,0x15b004)||
           !number(g,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !number(g,"GravityMultiplier",3,f.gravity,0x15b080)||
           !number(g,"Restitution",word(0x3f0ccccdu),f.restitution,0x15b0a0)||
           !number(a,"FramesPerSet",-1,f.framesPerSet,0x15b490))return Status::authoredData;
        auto timing=readOriginalGcCommonEmitterTiming50KG(bytes,f.timing,d);
        if(timing!=Status::complete)return timing;
        Math m;f.birthAge=m.mul(f.birthAge,f.life,0x15aff8);
        f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.flags|=0x400;
        const char* names[]{"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]{2,4,8,0x1000,0x2000,1,0x20,0x800};
        for(unsigned i=0;i<8;++i){bool v=i==7;
            if(!assets::originalUiBoolean50KG(p,names[i],i==7,v,d,0x15b1e8u+24u*i))return Status::authoredData;
            if(i==7)v=!v;if(v)f.flags|=masks[i];
        }
        if(f.groundLevel!=std::numeric_limits<float>::max())f.flags|=0x200;
        if(!assets::originalUiBoolean50KG(p,"RandomMirrorAboutX",false,f.mirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(p,"RandomMirrorAboutY",false,f.mirrorY,d,0x15b2c0))return Status::authoredData;
        std::int32_t forced=0;
        if(!assets::originalUiInteger50KG(a,"NumberOfForcedSets",0,forced,d,0x15b4c0))return Status::authoredData;
        if(forced){d.sourcePC=0x15b530;return Status::unsupported;}
        if(m.badPC){d.sourcePC=m.badPC;return Status::arithmetic;}
        if(f.life<=0||f.size<=0||f.rate<0){d.sourcePC=0x15f43c;return Status::unsupported;}
        f.parsed=true;out=f;return Status::complete;
    }catch(...){return Status::allocation;}
}
Receipt bindOriginalGcPodSparks50KG(OriginalGcPodSparksRuntime50KG& state,
    const OriginalGcPodSparksDefinition50KG& f,OriginalGcParticleManager50KG& manager,
    const EffectOwner50BE& owner) noexcept {
    if(state.bound||!f.parsed||!manager.prepared()||!owner.id||owner.profile!=EffectProfile50BE::Ordinary15F100||
       owner.mode1D8!=f.timing.mode1D8||(owner.state.flags1D4&7u)!=1u)
        return result(Status::unprepared,0x15f1e0);
    for(unsigned i=0;i<3;++i)if(std::memcmp(&f.velocity[i],owner.state.velocity60+i,4))
        return result(Status::authoredData,0x15b004);
    const auto* group=manager.group(f.group);if(!group)return result(Status::authoredData,0x15fd6c);
    OriginalGcParticleEmitterState50KG common{};common.particleGroup=f.group;
    common.framesPerSet=f.framesPerSet;common.inverseFramesPerSet=f.inverseFramesPerSet;
    common.gravity=f.gravity;common.bouncePlane=f.groundLevel;common.bounceFactor=f.restitution;
    const auto id=manager.registerEmitter(common);if(!id)return result(Status::allocation,0x15fd6c);
    state.manager=&manager;state.ownerId=id;state.canonicalOwnerId=owner.id;
    state.firstFrame=state.currentFrame=group->first;state.lastFrame=group->last;
    state.member=f.member;
    state.carriedSize18C=f.member!=OriginalGcPodSparksMember50KG::podSparks?0.f:f.size;
    state.bound=true;return result(Status::complete,0x15fdb4);
}
Receipt advanceOriginalGcPodSparksAfterCommon50KG(OriginalGcPodSparksRuntime50KG& state,
    const OriginalGcPodSparksDefinition50KG& f,OriginalGcParticleManager50KG& manager,
    EffectOwner50BE& owner,float dt,std::uint32_t& raw,OriginalGcPodSparksGlobals50KG& globals,
    const OriginalGcPodSparksAttachmentGate50KG* attachment) noexcept {
    if(state.failed)return state.stoppedAt;
    //The actual Racer810 member is admitted for load/Stop/base-disable only.
    //Its later common/virtual20 ownership is not granted by reader reuse.
    if(f.member==OriginalGcPodSparksMember50KG::racerExplWreckSparks)return result(Status::unsupported,0x15f380);
    if(!state.bound||state.manager!=&manager||!state.ownerId||!f.parsed||
       owner.id!=state.canonicalOwnerId||owner.profile!=EffectProfile50BE::Ordinary15F100||state.member!=f.member)
        return result(Status::unprepared,0x15f3c0);
    auto r=result(Status::complete,0x15fbc8);Math m;
    auto fail=[&](Status s,std::uint32_t pc){r.status=s;r.sourcePC=pc;state.failed=true;state.stoppedAt=r;return r;};
    auto arithmetic=[&](){return fail(Status::arithmetic,m.badPC);};
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x15f3c0);
    m.read(dt,0x15f450);if(m.badPC)return arithmetic();
    if(!(dt>0)||dt>1.f)return fail(Status::unsupported,0x15f450);
    auto& c=owner.state;
    const auto timer=m.read(word(c.word19C),0x15f3c0);if(m.badPC)return arithmetic();
    if(timer>.25f){std::memcpy(c.previous40,c.position30,16);return r;}
    // Vehicle74 is null; wreck groupC0 is attached to the actual primary model.
    // Source timer gate above precedes this real19C->04->A4 visibility test.
    if(f.member==OriginalGcPodSparksMember50KG::wreckSparks){
        if(!attachment||!attachment->sourceBound||!attachment->attachment74Present)
            return fail(Status::unprepared,0x15f3f0);
        if(attachment->attachment19CPresent){
            if(!attachment->attachment19CChild04Bound)return fail(Status::unprepared,0x15f410);
            if(attachment->attachment19CChild04TagA4!=attachment->currentTag8BE0){
                state.carriedSize18C=f.size;c.word19C=0;r.sourcePC=0x15fbb8;return r;}
        }
    }else if(attachment&&attachment->attachment74Present)return fail(Status::unsupported,0x15f3f0);
    const float emission=m.mul(f.rate,dt,0x15f450);
    if(m.badPC)return arithmetic();
    if(emission<0||emission>=2147483648.f)return fail(Status::unsupported,0x15fba4);
    float velocity[3],position[3];for(unsigned i=0;i<3;++i){
        velocity[i]=m.read(word(c.velocity60[i]),0x15f454u+4u*i);
        position[i]=m.read(word(c.position30[i]),0x15f5c8u+8u*i);}
    float speedFactor=1;
    if(f.moving){
        const auto square=m.add(m.add(m.mul(velocity[0],velocity[0],0x15f464),
            m.mul(velocity[1],velocity[1],0x15f468),0x15f46c),m.mul(velocity[2],velocity[2],0x15f470),0x15f470);
        const auto speed=root(m,square,0x15f474);
        if(!globals.lazy84D0){globals.speedScale433980=20.f;globals.lazy84D0=true;}
        const auto scale=m.read(globals.speedScale433980,0x15f4a0);
        if(speed<scale)speedFactor=m.div(speed,scale,0x15f4bc);
    }
    if(m.badPC)return arithmetic();
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    auto finish=[&](){state.carriedSize18C=f.size;c.word19C=0;return r;};
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
        const float growth=m.mul(f.growth,draw(),0x15f630);
        const float life=m.mul(f.life,draw(),0x15f67c);
        (void)m.div(1,life,0x15f694); // Source computes before the first capacity guard.
        std::uint32_t flags=f.flags;
        if(f.mirrorX&&randomX>.5f)flags|=0x4000;else flags&=~0x4000u;
        if(f.mirrorY&&randomY>.5f)flags|=0x8000;else flags&=~0x8000u;
        const auto squared=m.add(m.add(m.mul(particleVelocity[0],particleVelocity[0],0x15f780),
            m.mul(particleVelocity[1],particleVelocity[1],0x15f784),0x15f788),
            m.mul(particleVelocity[2],particleVelocity[2],0x15f78c),0x15f78c);
        float step[3]{};
        if(squared!=0){const auto inverse=m.div(1,root(m,squared,0x15f7bc),0x15f7bc);
            for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(particleVelocity[axis],inverse,0x15f7ccu+12u*axis);}
        const auto separation=m.mul(f.separation,f.size,0x15f7fc);
        for(unsigned axis=0;axis<3;++axis)step[axis]=m.mul(step[axis],separation,0x15f800u+12u*axis);
        const float lengthRandom=draw();float length;
        if(f.moving){const auto spreadLength=m.mul(m.sub(f.maxStreamLength,f.minStreamLength,0x15f85c),lengthRandom,0x15f860);
            length=m.add(.5f,m.add(1.f,m.mul(speedFactor,m.add(m.sub(f.minStreamLength,1.f,0x15f864),
                spreadLength,0x15f868),0x15f86c),0x15f870),0x15f87c);
        }else length=m.add(.5f,m.add(f.minStreamLength,m.mul(m.sub(f.maxStreamLength,f.minStreamLength,
            0x15f8bc),lengthRandom,0x15f8c0),0x15f8c4),0x15f8c8);
        const auto sections=m.cvtWord(length,0x15f880);
        const auto sectionFloat=m.integer(sections,0x15f8e0);
        if(m.badPC)return arithmetic();
        if(sections<=0)return fail(Status::unsupported,0x15f91c);
        const auto reciprocal=m.div(1,sectionFloat,0x15f91c);
        for(unsigned axis=0;axis<3;++axis)particleVelocity[axis]=m.add(particleVelocity[axis],
            m.mul(sectionFloat,step[axis],0x15f8f8u+4u*axis),0x15f928u+12u*axis);
        for(std::int32_t sectionIndex=0;sectionIndex<sections;++sectionIndex){
            const auto thickness=m.add(.5f,m.add(f.tailThickness,m.mul(reciprocal,m.mul(
                m.integer(sectionIndex,0x15f95c),m.sub(f.headThickness,f.tailThickness,0x15f960),0x15f964),
                0x15f968),0x15f96c),0x15f978);
            auto copies=m.cvtWord(thickness,0x15f97c);
            if(sectionIndex==sections-1&&f.superbrightHead){if(copies==std::numeric_limits<std::int32_t>::max())
                return fail(Status::unsupported,0x15f99c);++copies;}
            const auto ratio=m.div(m.integer(sectionIndex+1,0x15f9bc),sectionFloat,0x15f9c0);
            const float taper=std::max(word(0x3dcccccdu),ratio); //MAX.S15F9CC, not POW.
            const auto complement=m.sub(1,taper,0x15f9f4);
            if(m.badPC)return arithmetic();
            for(std::int32_t copy=0;copy<copies;++copy){
                auto* p=manager.nextParticle();if(!p){r.poolFull=true;return finish();}
                p->ageMilliseconds=0;p->position={point[0],point[1],point[2]};p->positionW=1;
                p->frame=state.currentFrame;p->growth=growth;p->size=p->initialSize=f.size;p->ownerId=state.ownerId;
                //15FA48's zero-growth flag clear is overwritten unconditionally
                // by15FA9C. Do not propagate stale free-slot flags into a particle.
                p->lifeMilliseconds=m.sub(life,m.mul(complement,m.mul(f.tailLife,life,0x15fa74),0x15fa78),0x15fa7c);
                p->inverseLife=m.div(1,p->lifeMilliseconds,0x15fa88);
                p->velocity={particleVelocity[0],particleVelocity[1],particleVelocity[2]};p->velocityW=1;p->flags=flags;
                if(m.badPC)return arithmetic();
                if(!manager.commitParticle(state.ownerId))return fail(Status::storage,0x15fab0);
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
        if(state.currentFrame==std::numeric_limits<std::int32_t>::max())return fail(Status::unsupported,0x15fb68);
        ++state.currentFrame;if(state.currentFrame>state.lastFrame)state.currentFrame=state.firstFrame;
    }
    if(m.badPC)return arithmetic();return finish();
}
} // namespace rr::game
