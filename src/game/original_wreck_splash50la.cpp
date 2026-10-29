// NEW 2026-10-13 native source reconstruction; no guest or VU execution.
#include "original_wreck_splash50la.h"
#include "water_wake50jc.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using S=OriginalGcSprayStatus50KG;
using D=assets::OriginalUiPageDiagnostic50KG;
using Section=assets::OriginalUiDatSection50KG;
using Math=water_wake_detail50jc::Math;
float bits(std::uint32_t word) noexcept {float f;std::memcpy(&f,&word,4);return f;}
void put(std::uint32_t& word,float f) noexcept {std::memcpy(&word,&f,4);}
OriginalGcSprayReceipt50KG receipt(S status,std::uint32_t pc) noexcept {
    OriginalGcSprayReceipt50KG r;r.status=status;r.sourcePC=pc;return r;
}
const Section* section(const std::vector<Section>& doc,const char* name) noexcept {
    for(const auto& s:doc)if(s.name==name)return &s;return nullptr;
}
float squareRoot(float x,Math& m,std::uint32_t pc) noexcept {
    m.read(x,pc);if(x<0){if(!m.badPC)m.badPC=pc;return 0;}
    volatile float result=std::sqrt(x);return m.read(result,pc);
}
} // namespace
OriginalGcSprayStatus50KG readOriginalWreckSplashDefinition50LA(
    const std::vector<std::uint8_t>& bytes,OriginalWreckSplashDefinition50LA& output,D& d) noexcept {
    OriginalWreckSplashSourceMetadata50LA ignored;
    return readOriginalWreckSplashSourceDefinition50LA(bytes,output,ignored,d);
}
OriginalGcSprayStatus50KG readOriginalWreckSplashSourceDefinition50LA(
    const std::vector<std::uint8_t>& bytes,OriginalWreckSplashDefinition50LA& output,
    OriginalWreckSplashSourceMetadata50LA& metadata,D& d) noexcept {
    d={};if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try {
        std::vector<Section> doc;
        // Original POD01.RES member19,1177 bytes. No colon annotations, so the
        // proven bounded PRT/EMT text grammar admits this exact authoring.
        if(!readOriginalGcParticleText50KG(bytes,1177,
           "53ed1da8eab1b70efc94b4dd7ba9502e677b1069dc05b28fa435f20080a877a6",doc))return S::assetIdentity;
        const auto* general=section(doc,"General Info");
        const auto* properties=section(doc,"Properties");
        const auto* animation=section(doc,"Animation");
        if(!general||!properties||!animation)return S::authoredData;
        OriginalWreckSplashDefinition50LA f;Math m;
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(*general,key,{},value,d,pc);};
        //19E9F0 reads the derived fields BEFORE common15AEA0 at19EB40.
        if(!v("MinRange",f.minRange220,0x19ea30)||!v("MaxRange",f.maxRange230,0x19ea4c)||
           !v("InitialPositionOffset",f.positionOffset240,0x19ea68)||
           !n(*general,"MinStreamHeight",1,f.minStreamHeight208,0x19ea84)||
           !n(*general,"MaxStreamHeight",3,f.maxStreamHeight20C,0x19eaa0)||
           !n(*general,"SizeModifier",.5f,f.sizeModifier200,0x19eabc)||
           !n(*general,"FillerLifeModifier",bits(0x3ecccccdu),f.fillerLifeModifier210,0x19eadc)||
           !n(*general,"FillerSizeModifier",bits(0x3f4ccccdu),f.fillerSizeModifier214,0x19eafc)||
           !n(*general,"FillerVelocityModifier",.5f,f.fillerVelocityModifier218,0x19eb18)||
           !n(*general,"SideVelocityModifier",2.5f,f.sideVelocityModifier21C,0x19eb34)||
           !i(*general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
           !n(*general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
           !n(*general,"InitialSize",1,f.initialSize184,0x15af8c)||
           !n(*general,"Growth",0,f.growth180,0x15afa4)||
           !n(*general,"EmitRate",500,f.emitRate188,0x15afc0)||
           !n(*general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
           !v("Velocity",f.velocity60,0x15b004))return S::authoredData;
        const float rawAge=f.ageAtBirth1A0;
        f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
        const auto timing=readOriginalGcCommonEmitterTiming50KG(bytes,f.timing,d);
        if(timing!=S::complete)return timing;
        if(!n(*general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !n(*general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
           !n(*general,"Restitution",bits(0x3f0ccccdu),f.restitution,0x15b0a0))return S::authoredData;
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(*properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;if(value)f.particleFlags198|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(*properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(*properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(*animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return S::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200;
        f.inverseFramesPerSet=m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.particleFlags198|=0x400;
        std::int32_t forced=0;
        if(!i(*animation,"NumberOfForcedSets",0,forced,0x15b4c0))return S::authoredData;
        if(forced!=0){d.sourcePC=0x15b508;return S::unsupported;}
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        // Actual member is one-shot0.25, zero delay/age, normal one-frame sets.
        // No defaults-only owner, changed authoring or missing manager fallback.
        if(f.timing.mode1D8!=1||f.timing.delayTime1E0!=0||f.timing.maxTimeOn1EC!=.25f||
           f.ageAtBirth1A0!=0||f.framesPerSet!=1||f.particleGroup1B8!=6){
            d.sourcePC=0x19e9f0;return S::unsupported;}
        OriginalWreckSplashSourceMetadata50LA source;
        source.rawAgeAtBirth=rawAge;source.forcedCount=forced;
        auto present=[&](const char* key){const auto* value=general->find(key);return value&&value->sourceValuePresent;};
        source.velocityPresent=present("Velocity");source.minRangePresent=present("MinRange");
        source.maxRangePresent=present("MaxRange");source.positionOffsetPresent=present("InitialPositionOffset");
        f.parsed=true;output=f;metadata=source;return S::complete;
    }catch(...){return S::allocation;}
}
OriginalGcSprayReceipt50KG bindOriginalWreckSplash50LA(
    OriginalWreckSplashRuntime50LA& runtime,const OriginalWreckSplashDefinition50LA& f,
    OriginalGcParticleManager50KG& manager,const EffectOwner50BE& owner) noexcept {
    if(runtime.bound||runtime.managerIdentity||runtime.managerOwnerId||runtime.canonicalOwnerId)
        return receipt(S::alreadyPrepared,0x19dcd0);
    if(!f.parsed||!manager.prepared())return receipt(S::unprepared,0x19ddcc);
    if(!owner.id||owner.profile!=EffectProfile50BE::Splash19DBE0||
       owner.mode1D8!=f.timing.mode1D8||owner.mode1D8!=1||
       bits(owner.duration1E4)!=f.timing.maxTimeOn1EC)
        return receipt(S::unsupported,0x19dcd0);
    const auto& c=owner.state;
    if((c.flags1D4&7u)!=1u||c.word19C!=0||c.intensity204!=0)
        return receipt(S::storage,0x2cb8ac);
    for(unsigned k=0;k<4;++k){const auto origin=k==3?0x3f800000u:0u;
        if(c.position30[k]!=origin||c.previous40[k]!=origin)return receipt(S::storage,0x15ac10);
        if(k==3){if(c.velocity60[k]!=0x3f800000u)return receipt(S::storage,0x15b004);}
        else if(bits(c.velocity60[k])!=f.velocity60[k])return receipt(S::storage,0x15b004);
    }
    const auto* group=manager.group(f.particleGroup1B8);
    if(!group)return receipt(S::unsupported,0x19eb58);
    OriginalGcParticleEmitterState50KG emitter;
    emitter.particleGroup=f.particleGroup1B8;emitter.framesPerSet=f.framesPerSet;
    emitter.inverseFramesPerSet=f.inverseFramesPerSet;emitter.gravity=f.gravityMultiplier;
    emitter.bouncePlane=f.groundLevel;emitter.bounceFactor=f.restitution;
    const auto id=manager.registerEmitter(emitter);
    if(!id)return receipt(S::capacity,0x19ddcc);
    runtime.managerIdentity=&manager;runtime.managerOwnerId=id;runtime.canonicalOwnerId=owner.id;
    runtime.firstFrame1B0=group->first;runtime.lastFrame1B4=group->last;
    runtime.currentFrame190=group->first;runtime.carriedSize18C=0; //15A6A0; full-pool writes happen later.
    runtime.bound=true;runtime.failed=false;return receipt(S::complete,0x19eb78);
}
OriginalGcSprayReceipt50KG resetOriginalWreckSplash88_50LA(
    const OriginalWreckSplashRuntime50LA& runtime,const OriginalWreckSplashDefinition50LA& f,
    EffectOwner50BE& owner,float& delay1DC) noexcept {
    if(runtime.failed)return runtime.stoppedAt;
    if(!runtime.bound||!f.parsed)return receipt(S::unprepared,0x15ad50);
    if(owner.id!=runtime.canonicalOwnerId||owner.profile!=EffectProfile50BE::Splash19DBE0||
       owner.mode1D8!=1||f.timing.mode1D8!=1)return receipt(S::unsupported,0x15ad6c);
    delay1DC=f.timing.delayTime1E0; //15AD68; actual authored delay0.
    put(owner.duration1E4,f.timing.maxTimeOn1EC); //15AD80; actual one-shot0.25.
    return receipt(S::complete,0x15ade0);
}
OriginalGcSprayReceipt50KG advanceOriginalWreckSplashAfterCommon50LA(
    OriginalWreckSplashRuntime50LA& runtime,const OriginalWreckSplashDefinition50LA& f,
    OriginalGcParticleManager50KG& manager,EffectOwner50BE& owner,float dt,
    std::uint32_t& raw,SharedRandomOwner50ID& shared,HostMonotonicClock50ID& clock) noexcept {
    if(runtime.failed)return runtime.stoppedAt;
    if(!runtime.bound||!f.parsed||!manager.prepared())return receipt(S::unprepared,0x19def4);
    if(runtime.managerIdentity!=&manager||owner.id!=runtime.canonicalOwnerId||
       owner.profile!=EffectProfile50BE::Splash19DBE0||owner.mode1D8!=f.timing.mode1D8||
       !manager.emitter(runtime.managerOwnerId))return receipt(S::unsupported,0x19def4);
    auto r=receipt(S::complete,0x19e9a4);Math m;auto& c=owner.state;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;
        runtime.failed=true;runtime.stoppedAt=r;return r;};
    auto failMath=[&](){return fail(S::arithmetic,m.badPC);};
    auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x19def4);
    std::array<float,3> delta{},position{},base{},step{};
    const std::uint32_t subtractPC[]={0x19df10,0x19df18,0x19df20};
    for(unsigned k=0;k<3;++k){position[k]=bits(c.position30[k]);
        delta[k]=m.sub(position[k],bits(c.previous40[k]),subtractPC[k]);}
    // Source computes this distance although the splash demand below never
    // consumes it. Preserve its scalar arithmetic; do not use spray's formula.
    const auto x2=m.mul(delta[0],delta[0],0x19df44);
    const auto y2=m.mul(delta[1],delta[1],0x19df48);
    const auto xy=m.add(x2,y2,0x19df4c);
    const auto z2=m.mul(delta[2],delta[2],0x19df50);
    (void)squareRoot(m.add(xy,z2,0x19df50),m,0x19df54);
    const auto demand=m.mul(dt,f.emitRate188,0x19df68);
    if(m.badPC)return failMath();
    std::int32_t count=0;
    if(demand>1){
        count=m.cvtWord(demand,0x19df7c);if(m.badPC)return failMath();c.intensity204=0;
    }else{
        const auto accumulated=m.add(bits(c.intensity204),demand,0x19df90);
        if(m.badPC)return failMath();put(c.intensity204,accumulated);
        if(accumulated>1){count=m.cvtWord(accumulated,0x19dfa8);
            if(m.badPC)return failMath();c.intensity204=0;}
    }
    // With count0,19DFD8 computes1/0 and dead local products; neither owner
    // nor particle observes them. Retain the accumulator and reached base
    // arithmetic but omit only those exceptional unused temporaries.
    if(count){const auto inverse=m.div(1,m.integer(count,0x19dfbc),0x19dfd8);
        const std::uint32_t pc[]={0x19dfdc,0x19dfe0,0x19dfe8};
        for(unsigned k=0;k<3;++k)step[k]=m.mul(inverse,delta[k],pc[k]);
    }else r.deadZeroCountReciprocal=true;
    const std::uint32_t basePC[]={0x19e018,0x19e01c,0x19e028};
    for(unsigned k=0;k<3;++k)base[k]=m.add(position[k],f.positionOffset240[k],basePC[k]);
    //2CB890 passes a3=null to19DCD0, so+250 stays source-null.19E044 jumps
    // directly19E108. Side velocity is the real zero vector from433F50;
    // do not manufacture a callback, cross product or attachment fallback.
    if(m.badPC)return failMath();
    auto full=[&](std::uint32_t pc){runtime.carriedSize18C=f.initialSize184;c.word19C=0;
        r.poolFull=true;r.sourcePC=pc;return r;};
    auto frame=[&](OriginalGcParticle50KG& p,bool filler)->bool {
        if(!seedOriginalGcParticleRandom50KG(raw,shared,clock)){
            r.status=S::clock;r.sourcePC=filler?0x19e4f8u:0x19e7f0u;return false;}
        const auto wide=std::int64_t(runtime.lastFrame1B4)-std::int64_t(runtime.firstFrame1B0)+1;
        if(wide<1||wide>std::numeric_limits<std::int32_t>::max()){
            r.status=S::storage;r.sourcePC=filler?0x19e51cu:0x19e814u;return false;}
        const auto span=m.integer(static_cast<std::int32_t>(wide),filler?0x19e528u:0x19e820u);
        const auto unit=draw();
        const auto offset=m.cvtWord(m.mul(span,unit,filler?0x19e548u:0x19e840u),filler?0x19e54cu:0x19e844u);
        if(m.badPC){r.status=S::arithmetic;r.sourcePC=m.badPC;return false;}
        const auto selectedWide=std::int64_t(runtime.firstFrame1B0)+offset;
        if(selectedWide<std::numeric_limits<std::int32_t>::min()||selectedWide>std::numeric_limits<std::int32_t>::max()){
            r.status=S::arithmetic;r.sourcePC=filler?0x19e558u:0x19e850u;return false;}
        const auto selected=static_cast<std::int32_t>(selectedWide);
        if(!checkOriginalGcParticleRandomRange50KG(selected,runtime.firstFrame1B0,runtime.lastFrame1B4,shared)){
            r.status=S::sourceRangeAssertion;r.sourcePC=filler?0x19e5acu:0x19e8a4u;return false;}
        p.frame=selected;return true;
    };
    for(std::int32_t stream=0;stream<count;++stream){
        // Exactly five raw pre-capacity draws per stream: X,Y,Z,growth,height.
        // Frame draws use the same application-owned lazy seed/range guards.
        const auto x=draw(),y=draw(),z=draw();const float unit[]={x,y,z};
        std::array<float,3> spread{},origin{},velocity{};
        const auto index=m.integer(stream,0x19e198),total=m.integer(count,0x19e1ac);
        const auto fraction=m.div(index,total,0x19e1bc);
        const auto sizeFactor=std::max(bits(0x3e4ccccdu),fraction); //MAX.S19E1FC.
        const std::uint32_t spreadPC[]={0x19e1b0,0x19e1d0,0x19e1ec};
        const std::uint32_t originMultiplyPC[]={0x19e230,0x19e234,0x19e23c};
        const std::uint32_t originAddPC[]={0x19e25c,0x19e260,0x19e268};
        for(unsigned k=0;k<3;++k){const auto pc=spreadPC[k];
            const auto span=m.sub(f.maxRange230[k],f.minRange220[k],pc);
            spread[k]=m.add(f.minRange220[k],m.mul(unit[k],span,pc+4),pc+8);
            origin[k]=m.add(base[k],m.mul(index,step[k],originMultiplyPC[k]),originAddPC[k]);
        }
        if(m.badPC)return failMath();
        const auto growthUnit=draw();
        const auto growth=m.mul(f.growth180,growthUnit,0x19e2b4);
        const auto size=m.mul(f.initialSize184,sizeFactor,0x19e2c0);
        const auto inverseLife=m.div(1,f.particleLife17C,0x19e2d8);
        const std::uint32_t velocityAddPC[]={0x19e2ec,0x19e2f0,0x19e2f8};
        const std::uint32_t sideAddPC[]={0x19e318,0x19e328,0x19e330};
        for(unsigned k=0;k<3;++k){const auto sum=m.add(bits(c.velocity60[k]),spread[k],velocityAddPC[k]);
            velocity[k]=m.add(sum,0,sideAddPC[k]);}
        std::uint32_t flags=f.particleFlags198;
        if(x>.5f&&f.randomMirrorX)flags|=0x4000u;else flags&=~0x4000u;
        if(y>.5f&&f.randomMirrorY)flags|=0x8000u;else flags&=~0x8000u;
        if(m.badPC)return failMath();
        const auto heightUnit=draw();
        const auto span=m.sub(f.maxStreamHeight20C,f.minStreamHeight208,0x19e418);
        const auto height=m.add(f.minStreamHeight208,m.mul(span,heightUnit,0x19e41c),0x19e420);
        const auto streamCount=m.cvtWord(m.add(.5f,height,0x19e424),0x19e428);
        if(m.badPC)return failMath();
        auto* p=manager.nextParticle();if(!p)return full(0x19e6a8);
        // Fill19E468..6A4. Preserve reused depth20,reserved24,next4C fields.
        p->ageMilliseconds=0;
        const auto halfway=m.sub(index,.5f,0x19e460);
        std::array<float,3> fillerPosition{};
        const std::uint32_t fillerMultiplyPC[]={0x19e478,0x19e47c,0x19e484};
        const std::uint32_t fillerAddPC[]={0x19e4a4,0x19e4a8,0x19e4b0};
        for(unsigned k=0;k<3;++k)fillerPosition[k]=m.add(base[k],
            m.mul(halfway,step[k],fillerMultiplyPC[k]),fillerAddPC[k]);
        if(m.badPC)return failMath();p->position=fillerPosition;p->positionW=1;
        if(!frame(*p,true))return fail(r.status,r.sourcePC);
        p->growth=growth;if(growth==0)p->flags&=~8u;
        const auto fillerSize=m.add(size,m.mul(f.growth180,f.fillerSizeModifier214,0x19e624),0x19e628);
        if(m.badPC)return failMath();p->size=fillerSize;p->ownerId=runtime.managerOwnerId;p->initialSize=p->size;
        const auto life=m.mul(f.particleLife17C,f.fillerLifeModifier210,0x19e644);
        if(m.badPC)return failMath();p->lifeMilliseconds=life;
        const auto inverse=m.div(1,life,0x19e650);if(m.badPC)return failMath();p->inverseLife=inverse;
        std::array<float,3> fillerVelocity{};
        const std::uint32_t fillerVelocityPC[]={0x19e668,0x19e66c,0x19e674};
        for(unsigned k=0;k<3;++k)fillerVelocity[k]=m.mul(f.fillerVelocityModifier218,velocity[k],fillerVelocityPC[k]);
        if(m.badPC)return failMath();p->velocity=fillerVelocity;p->velocityW=1;p->flags=flags;
        // Late flags assignment19E690 overwrites the preceding growth0 bit
        // clear. Preserve this source order rather than fixing its behavior.
        if(!manager.commitParticle(runtime.managerOwnerId))return fail(S::storage,0x19e6a4);
        ++r.spawned;
        for(std::int32_t vertical=0;vertical<streamCount;++vertical){
            const auto ratio=m.div(m.integer(vertical,0x19e6cc),m.integer(streamCount,0x19e6dc),0x19e6e4);
            //19E6E8..700 computes an unused vector before the capacity gate.
            const std::uint32_t unusedPC[]={0x19e6e8,0x19e6f0,0x19e6f8};
            for(unsigned k=0;k<3;++k)m.mul(ratio,velocity[k],unusedPC[k]);
            const auto complement=m.sub(1,ratio,0x19e720);
            if(m.badPC)return failMath();p=manager.nextParticle();if(!p)return full(0x19e96c);
            p->ageMilliseconds=0;std::array<float,3> streamPosition{};
            const std::uint32_t firstPC[]={0x19e74c,0x19e750,0x19e758};
            const std::uint32_t secondPC[]={0x19e76c,0x19e770,0x19e778};
            const std::uint32_t addPC[]={0x19e798,0x19e79c,0x19e7a4};
            for(unsigned k=0;k<3;++k){const auto first=m.mul(ratio,velocity[k],firstPC[k]);
                const auto second=m.mul(ratio,first,secondPC[k]);
                streamPosition[k]=m.add(origin[k],second,addPC[k]);}
            if(m.badPC)return failMath();p->position=streamPosition;p->positionW=1;
            if(!frame(*p,false))return fail(r.status,r.sourcePC);
            const auto streamGrowth=m.add(growth,m.mul(growth,ratio,0x19e8e4),0x19e8e8);
            if(m.badPC)return failMath();p->growth=streamGrowth;if(streamGrowth==0)p->flags&=~8u;
            const auto sizePart=m.mul(growth,complement,0x19e91c);
            const auto modified=m.mul(f.sizeModifier200,sizePart,0x19e920);
            const auto streamSize=m.add(size,modified,0x19e924);
            if(m.badPC)return failMath();p->size=streamSize;p->ownerId=runtime.managerOwnerId;p->initialSize=p->size;
            p->lifeMilliseconds=f.particleLife17C;p->inverseLife=inverseLife;
            p->velocity=velocity;p->velocityW=1;p->flags=flags;
            if(!manager.commitParticle(runtime.managerOwnerId))return fail(S::storage,0x19e968);
            ++r.spawned;
        }
    }
    return r;
}
} // namespace rr::game
