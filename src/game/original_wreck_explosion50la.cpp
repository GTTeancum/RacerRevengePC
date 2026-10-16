#include "original_wreck_explosion50la.h"
#include "water_wake50jc.h"
#include <cfenv>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
using Status=OriginalGcSprayStatus50KG;
using Receipt=OriginalGcSprayReceipt50KG;
using Section=assets::OriginalUiDatSection50KG;
using Math=water_wake_detail50jc::Math;
Receipt result(Status status,std::uint32_t pc) noexcept {
    Receipt r;r.status=status;r.sourcePC=pc;return r;
}
float word(std::uint32_t bits) noexcept {float f;std::memcpy(&f,&bits,4);return f;}
const Section& section(const std::vector<Section>& document,const char* name,const Section& absent) noexcept {
    for(const auto& s:document)if(s.name==name)return s;return absent;
}
}
OriginalGcSprayStatus50KG readOriginalWreckExplosionDefinition50LA(
    const std::vector<std::uint8_t>& bytes,OriginalWreckExplosionDefinition50LA& out,
    assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};try {
        std::vector<Section> document;
        if(!readOriginalGcParticleText50KG(bytes,1169,
           "69e5c5fa4c1394c15162580853149abbe4659e4a673f8100dfa512c20ddf69c4",document))
            return Status::assetIdentity;
        if(std::fegetround()!=FE_TONEAREST){d.sourcePC=0x15ef70;return Status::arithmetic;}
        const Section absent{};const auto& general=section(document,"General Info",absent);
        const auto& properties=section(document,"Properties",absent);
        const auto& animation=section(document,"Animation",absent);
        OriginalWreckExplosionDefinition50LA f;f.groundLevel=std::numeric_limits<float>::max();
        auto number=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        if(!assets::originalUiTriple50KG(general,"MinRange",{},f.minRange,d,0x15efb0)||
           !assets::originalUiTriple50KG(general,"MaxRange",{},f.maxRange,d,0x15efcc)||
           !assets::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d,0x15efe4)||
           !assets::originalUiInteger50KG(general,"ParticleGroup",-1,f.group,d,0x15aee0)||
           !number(general,"ParticleLife",1000,f.life,0x15af70)||
           !number(general,"InitialSize",1,f.size,0x15af8c)||
           !number(general,"Growth",0,f.growth,0x15afa4)||
           !number(general,"EmitRate",500,f.rate,0x15afc0)||
           !number(general,"AgeAtBirth",0,f.birthAge,0x15afd8)||
           !assets::originalUiTriple50KG(general,"Velocity",{},f.velocity,d,0x15b004)||
           !number(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !number(general,"GravityMultiplier",3,f.gravity,0x15b080)||
           !number(general,"Restitution",word(0x3f0ccccdu),f.restitution,0x15b0a0))return Status::authoredData;
        const auto timing=readOriginalGcCommonEmitterTiming50KG(bytes,f.timing,d);
        if(timing!=Status::complete)return timing;
        Math m;f.birthAge=m.mul(f.birthAge,f.life,0x15aff8);
        const char* keys[]{"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]{2,4,8,0x1000,0x2000,1,0x20,0x800};
        for(unsigned i=0;i<8;++i){bool value=i==7;
            if(!assets::originalUiBoolean50KG(properties,keys[i],i==7,value,d,0x15b1e8u+24u*i))
                return Status::authoredData;
            if(i==7)value=!value;if(value)f.flags|=masks[i];
        }
        if(!assets::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.mirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.mirrorY,d,0x15b2c0)||
           !number(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return Status::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.flags|=0x200;
        f.inverseFramesPerSet=m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.flags|=0x400;
        std::int32_t forcedSets=0;
        if(!assets::originalUiInteger50KG(animation,"NumberOfForcedSets",0,forcedSets,d,0x15b4c0))
            return Status::authoredData;
        // The authenticated podExplosion member has no forced-set list.
        if(forcedSets!=0){d.sourcePC=0x15b508;return Status::unsupported;}
        f.spreadVelocityScale220=f.velocity[1]; //15EFF8..15EFFC, before body motion.
        if(m.badPC){d.sourcePC=m.badPC;return Status::arithmetic;}
        f.parsed=true;out=f;return Status::complete;
    }catch(...){return Status::allocation;}
}
Receipt bindOriginalWreckExplosion50LA(OriginalWreckExplosionRuntime50LA& state,
    const OriginalWreckExplosionDefinition50LA& f,OriginalGcParticleManager50KG& manager,
    const EffectOwner50BE& owner) noexcept {
    if(state.bound||!f.parsed||!manager.prepared()||!owner.id||
       owner.profile!=EffectProfile50BE::Deferred15E800||owner.mode1D8!=f.timing.mode1D8||
       (owner.state.flags1D4&7u)!=1u)return result(Status::unprepared,0x15e8f0);
    for(unsigned i=0;i<3;++i)if(std::memcmp(&f.velocity[i],owner.state.velocity60+i,4))
        return result(Status::authoredData,0x15b004);
    const auto* group=manager.group(f.group);if(!group)return result(Status::authoredData,0x15f010);
    OriginalGcParticleEmitterState50KG common{};common.particleGroup=f.group;
    common.framesPerSet=f.framesPerSet;common.inverseFramesPerSet=f.inverseFramesPerSet;
    common.gravity=f.gravity;common.bouncePlane=f.groundLevel;common.bounceFactor=f.restitution;
    const auto id=manager.registerEmitter(common);if(!id)return result(Status::allocation,0x15f010);
    state.manager=&manager;state.ownerId=id;state.canonicalOwnerId=owner.id;
    state.firstFrame=group->first;state.lastFrame=group->last;state.bound=true;
    return result(Status::complete,0x15f04c);
}
Receipt advanceOriginalWreckExplosionAfterCommon50LA(OriginalWreckExplosionRuntime50LA& state,
    const OriginalWreckExplosionDefinition50LA& f,OriginalGcParticleManager50KG& manager,
    EffectOwner50BE& owner,float dt,std::uint32_t& raw,OriginalWreckExplosionGlobals50LA& globals,
    const OriginalWreckExplosionAttachmentGate50LA& attachment) noexcept {
    if(state.failed)return state.stoppedAt;
    if(!state.bound||state.manager!=&manager||!state.ownerId||!f.parsed||
       owner.id!=state.canonicalOwnerId||owner.profile!=EffectProfile50BE::Deferred15E800)
        return result(Status::unprepared,0x15eb08);
    auto r=result(Status::complete,0x15ef48);Math m;
    auto fail=[&](Status status,std::uint32_t pc){
        r.status=status;r.sourcePC=pc;state.failed=true;state.stoppedAt=r;return r;};
    auto arithmetic=[&](){return fail(Status::arithmetic,m.badPC);};
    if(!attachment.sourceBound||!attachment.attachment74Present)
        return fail(Status::unprepared,0x15eb08);
    if(attachment.attachment19CPresent){
        if(!attachment.attachment19CChild04Bound)return fail(Status::unprepared,0x15eb28);
        if(attachment.attachment19CChild04TagA4!=attachment.currentTag8BE0){
            r.sourcePC=0x15eb4c;return r;
        }
    }
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x15eb54);
    const auto demand=m.add(.5f,m.mul(f.rate,dt,0x15eb64),0x15eb68);
    auto count=m.cvtWord(demand,0x15eb6c);
    if(m.badPC)return arithmetic();
    if(count<=0)count=1; //15EB78..80 even for a zero emission product.
    if(!globals.lazy84CC){globals.emissionCounter433978=0;globals.lazy84CC=true;}
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    const auto& c=owner.state;
    for(std::int32_t index=0;index<count;++index){
        auto* p=manager.nextParticle();
        if(!p){r.poolFull=true;r.sourcePC=0x15ef28;return r;}
        ++globals.emissionCounter433978; //15EBC4..15EBD8, shared process counter.
        const float randomX=draw(),randomY=draw(),randomZ=draw();
        const auto spreadX=m.add(f.minRange[0],m.mul(randomX,
            m.sub(f.maxRange[0],f.minRange[0],0x15ec64),0x15ec68),0x15ec6c);
        const auto spreadZ=m.add(f.minRange[2],m.mul(randomZ,
            m.sub(f.maxRange[2],f.minRange[2],0x15ec80),0x15ec84),0x15ec88);
        if(m.badPC)return arithmetic();
        const std::int32_t set=randomZ<=.5f?0:1; //C.OLE.S15EC50, not a fresh draw.
        // The source copies previous40, preserving Y/W; range Y is not applied.
        std::memcpy(p->position.data(),c.previous40,12);
        std::memcpy(&p->positionW,c.previous40+3,4);
        p->position[0]=m.add(p->position[0],spreadX,0x15ed78);
        if(m.badPC)return arithmetic();
        p->position[2]=m.add(p->position[2],spreadZ,0x15ed88);
        if(m.badPC)return arithmetic();
        p->size=p->initialSize=f.size;p->ownerId=state.ownerId;
        const auto lifeStep=m.integer(static_cast<std::int32_t>(globals.emissionCounter433978&3u),0x15edb8);
        const auto lifeReduction=m.mul(.125f,m.mul(f.life,lifeStep,0x15edbc),0x15edc0);
        p->lifeMilliseconds=m.sub(f.life,lifeReduction,0x15edc4);
        if(m.badPC)return arithmetic();
        p->ageMilliseconds=f.birthAge;
        const auto frame=m.add(m.integer(state.firstFrame,0x15eddc),
            m.mul(m.integer(set,0x15ed40),f.framesPerSet,0x15ede0),0x15ede4);
        p->frame=m.cvtWord(frame,0x15ede8);
        if(m.badPC)return arithmetic();
        p->inverseLife=m.div(1,p->lifeMilliseconds,0x15edfc);
        if(m.badPC)return arithmetic();
        p->growth=f.growth;
        //15EE18's conditional zero-growth flag clear is overwritten at15EE44.
        p->flags=f.flags;
        if(f.mirrorX&&randomX>.5f)p->flags|=0x4000;else p->flags&=~0x4000u;
        if(f.mirrorY&&randomY>.5f)p->flags|=0x8000;else p->flags&=~0x8000u;
        std::memcpy(p->velocity.data(),c.velocity60,12);
        std::memcpy(&p->velocityW,c.velocity60+3,4);
        if(f.applySpreadToVelocity){
            p->velocity[1]=m.add(p->velocity[1],m.mul(f.spreadVelocityScale220,randomY,0x15ef00),0x15ef04);
            if(m.badPC)return arithmetic();
        }
        if(!manager.commitParticle(state.ownerId))return fail(Status::storage,0x15ef20);
        ++r.spawned;
    }
    // No derived reset of previous40,19C,18C or the one-shot duration exists.
    return r;
}
} // namespace rr::game
