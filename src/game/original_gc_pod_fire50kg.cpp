#include "original_gc_pod_fire50kg.h"
#include "water_wake50jc.h"
#include <cfenv>
#include <cstring>
#include <limits>
#include <utility>

namespace rr::game {
namespace {
using S=OriginalGcSprayStatus50KG;
using Section=assets::OriginalUiDatSection50KG;
using D=assets::OriginalUiPageDiagnostic50KG;
float bits(std::uint32_t word) noexcept {float value;std::memcpy(&value,&word,4);return value;}
void put(std::uint32_t& word,float value) noexcept {std::memcpy(&word,&value,4);}
const Section* section(const std::vector<Section>& doc,const char* name) noexcept {
    for(const auto& s:doc)if(s.name==name)return &s;return nullptr;
}
OriginalGcPodFireReceipt50KG receipt(S status,std::uint32_t pc) noexcept {
    OriginalGcPodFireReceipt50KG r;r.status=status;r.sourcePC=pc;return r;
}
bool authored(const OriginalGcPodFireDefinition50KG& d) noexcept {
    const auto& f=d.emission;
    return d.parsed&&f.particleGroup==4&&f.firstFrame==52&&f.lastFrame==75&&
        f.particleLife==500&&f.initialSize==1&&f.growth==0&&f.emitRate==150&&f.framesPerSet==8&&
        f.minRange==std::array<float,3>{{-1.35f,0,-1.35f}}&&
        f.maxRange==std::array<float,3>{{1.35f,0,1.35f}}&&
        f.authoredVelocityY220==10&&f.particleFlags==0x1c05u&&
        f.randomMirrorX&&f.randomMirrorY&&f.applySpreadToVelocity&&
        d.authoredVelocity==std::array<float,3>{{0,10,0}}&&d.gravityMultiplier==3&&
        d.groundLevel==std::numeric_limits<float>::max()&&d.restitution==.55f;
}
}
OriginalGcSprayStatus50KG readOriginalGcPodFireDefinition50KG(
    const std::vector<std::uint8_t>& bytes,OriginalGcPodFireDefinition50KG& out,D& d) noexcept {
    OriginalGcPodFireSourceMetadata50KG ignored;
    return readOriginalGcPodFireSourceDefinition50KG(bytes,out,ignored,d);
}
OriginalGcSprayStatus50KG readOriginalGcPodFireSourceDefinition50KG(
    const std::vector<std::uint8_t>& bytes,OriginalGcPodFireDefinition50KG& out,
    OriginalGcPodFireSourceMetadata50KG& metadata,D& d) noexcept {
    d={};if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try {
        std::vector<Section> doc;
        // This exact member contains semicolon comments and no colon annotation
        // lines. The existing authenticated PRT/EMT reader admits its grammar.
        if(!readOriginalGcParticleText50KG(bytes,815,
           "b122aca0eb7779d48f5f90c93f2ab2d624ee38d2f824b1843ae62e790a5e44f8",doc))return S::assetIdentity;
        const auto* general=section(doc,"General Info");
        const auto* properties=section(doc,"Properties");const auto* animation=section(doc,"Animation");
        if(!general||!properties||!animation)return S::authoredData;
        OriginalGcPodFireDefinition50KG next;auto& f=next.emission;
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(*general,key,{},value,d,pc);};
        std::int32_t mode=0,forcedSets=0;float delay=0,birthFraction=0;
        //15E6D0 derived reads precede the common15AEA0 reader. The cached Y
        // field220 is copied from freshly authored controls64 at15E758..75C.
        if(!v("MinRange",f.minRange,0x15e710)||!v("MaxRange",f.maxRange,0x15e72c)||
           !assets::originalUiBoolean50KG(*general,"ApplySpreadToVelocity",true,f.applySpreadToVelocity,d,0x15e744)||
           !i(*general,"ParticleGroup",-1,f.particleGroup,0x15aee0)||
           !n(*general,"ParticleLife",1000,f.particleLife,0x15af70)||
           !n(*general,"InitialSize",1,f.initialSize,0x15af8c)||
           !n(*general,"Growth",0,f.growth,0x15afa4)||
           !n(*general,"EmitRate",500,f.emitRate,0x15afc0)||
           !n(*general,"AgeAtBirth",0,birthFraction,0x15afd8)||
           !v("Velocity",next.authoredVelocity,0x15b004)||
           !i(*general,"EmitMode",0,mode,0x15b01c)||
           !n(*general,"DelayTime",0,delay,0x15b03c)||
           !n(*general,"GroundLevel",std::numeric_limits<float>::max(),next.groundLevel,0x15b064)||
           !n(*general,"GravityMultiplier",3,next.gravityMultiplier,0x15b080)||
           !n(*general,"Restitution",.55f,next.restitution,0x15b0a0))return S::authoredData;
        // Authored mode0 delegates all existing mutable common state to50BE
        // and the common timing owner. No shadow delay/duration is installed.
        if(mode!=0||delay!=0||birthFraction!=0){d.sourcePC=0x15b01c;return S::unsupported;}
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(*properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;if(value)f.particleFlags|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(*properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(*properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(*animation,"FramesPerSet",-1,f.framesPerSet,0x15b490)||
           !i(*animation,"NumberOfForcedSets",0,forcedSets,0x15b4c0))return S::authoredData;
        if(next.groundLevel!=std::numeric_limits<float>::max())f.particleFlags|=0x200;
        if(f.framesPerSet>1)f.particleFlags|=0x400;
        if(forcedSets!=0){d.sourcePC=0x15b508;return S::unsupported;}
        f.authoredVelocityY220=next.authoredVelocity[1];
        // The concrete GC demoprt registry is also checked at actual binding.
        f.firstFrame=52;f.lastFrame=75;next.parsed=true;
        if(!authored(next)){d.sourcePC=0x15e6d0;return S::unsupported;}
        OriginalGcPodFireSourceMetadata50KG source;
        source.rawAgeAtBirth=birthFraction;source.delayTime=delay;
        source.mode=mode;source.forcedCount=forcedSets;
        auto present=[&](const char* key){const auto* v=general->find(key);return v&&v->sourceValuePresent;};
        source.velocityPresent=present("Velocity");source.minRangePresent=present("MinRange");
        source.maxRangePresent=present("MaxRange");
        out=std::move(next);metadata=source;return S::complete;
    }catch(...){return S::allocation;}
}
OriginalGcPodFireReceipt50KG bindOriginalGcPodFire50KG(
    OriginalGcPodFireRuntime50KG& runtime,const OriginalGcPodFireDefinition50KG& d,
    OriginalGcParticleManager50KG& manager,const EffectOwner50BE& owner) noexcept {
    if(runtime.manager||runtime.managerOwnerId||runtime.canonicalOwnerId)return receipt(S::alreadyPrepared,0x15def0);
    if(!manager.prepared()||!authored(d))return receipt(S::unprepared,0x15e020);
    if(!owner.id||owner.profile!=EffectProfile50BE::Fire15DE00||owner.mode1D8!=0||owner.duration1E4!=0)
        return receipt(S::storage,0x15de00);
    const auto& c=owner.state;
    if((c.flags1D4&7u)!=1u||c.word19C!=0)return receipt(S::storage,0x15ad00);
    for(unsigned lane=0;lane<4;++lane){
        const std::uint32_t origin=lane==3?0x3f800000u:0u;
        if(c.position30[lane]!=origin||c.previous40[lane]!=origin)return receipt(S::storage,0x15de3c);
        if(lane==3){if(c.velocity60[lane]!=0x3f800000u)return receipt(S::storage,0x15b004);}
        else if(bits(c.velocity60[lane])!=d.authoredVelocity[lane])return receipt(S::storage,0x15e758);
    }
    const auto& f=d.emission;const auto* group=manager.group(f.particleGroup);
    if(!group||group->type!=8||group->first!=f.firstFrame||group->last!=f.lastFrame)
        return receipt(S::authoredData,0x15e770);
    OriginalGcParticleEmitterState50KG common;
    common.particleGroup=f.particleGroup;common.framesPerSet=f.framesPerSet;common.inverseFramesPerSet=.125f;
    common.gravity=d.gravityMultiplier;common.bouncePlane=d.groundLevel;common.bounceFactor=d.restitution;
    const auto id=manager.registerEmitter(common);if(!id)return receipt(S::capacity,0x15df1c);
    runtime.manager=&manager;runtime.managerOwnerId=id;runtime.canonicalOwnerId=owner.id;runtime.fractional224=0;
    return receipt(S::complete,0x15e0ac);
}
OriginalGcPodFireReceipt50KG advanceOriginalGcPodFireAfterCommon50KG(
    OriginalGcPodFireRuntime50KG& runtime,const OriginalGcPodFireDefinition50KG& d,
    OriginalGcParticleManager50KG& manager,EffectOwner50BE& owner,float dt,
    std::uint32_t& raw,std::uint32_t& serial,bool allowCarry) noexcept {
    if(runtime.manager!=&manager||!runtime.managerOwnerId||!authored(d))return receipt(S::unprepared,0x15e10c);
    if(owner.id!=runtime.canonicalOwnerId||owner.profile!=EffectProfile50BE::Fire15DE00||owner.mode1D8!=0||
       (owner.state.flags1D4&3u)!=2u)return receipt(S::storage,0x15e10c);
    if(&raw==&serial)return receipt(S::storage,0x15e2e8);
    if(std::fegetround()!=FE_TONEAREST)return receipt(S::arithmetic,0x15e10c);
    water_wake_detail50jc::Math m;std::array<float,3> previous{},delta{},velocity{};
    auto& c=owner.state;
    constexpr std::uint32_t subtractPc[]={0x15e128,0x15e130,0x15e138};
    for(unsigned lane=0;lane<3;++lane){previous[lane]=bits(c.previous40[lane]);
        delta[lane]=m.sub(bits(c.position30[lane]),previous[lane],subtractPc[lane]);}
    const auto inverse=m.div(1,dt,0x15e140);
    // Source vector temporary is published to canonical60 in one SQ15E180.
    velocity[1]=m.mul(delta[1],inverse,0x15e160);
    velocity[0]=m.mul(delta[0],inverse,0x15e164);
    velocity[2]=m.mul(delta[2],inverse,0x15e16c);
    if(m.badPC)return receipt(S::arithmetic,m.badPC);
    for(unsigned lane=0;lane<3;++lane)put(c.velocity60[lane],velocity[lane]);
    c.velocity60[3]=0x3f800000u;
    // Actual vehicle fire attachment74 is null:15E188 goes directly15E1D0.
    // There is no scene/render owner to approximate or synthetic visible flag.
    return emitOriginalGcFireParticles50KG(d.emission,manager,runtime.managerOwnerId,
        runtime.fractional224,previous,c.previous40[3],velocity,dt,raw,serial,allowCarry);
}
} // namespace rr::game
