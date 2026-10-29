#pragma once
#include "original_gc_spray50kg.h"
#include "water_wake50jc.h"
#include <cfenv>
#include <cstring>
#include <limits>

namespace rr::game {
// Immutable, authenticated authoring only. Neither an EffectProfile19A5E0
// owner nor a numeric emitter. Reached source writes and RNG are performed
// separately by the actual19BA50/15B860 owner; parsing consumes no draws.
struct OriginalRacerEmberDefinition50LA {
    OriginalGcCommonEmitterTiming50KG timing{};
    std::array<float,3> velocity60{},minimumVelocityNoise240{},maximumVelocityNoise250{};
    std::int32_t particleGroup1B8=-1;
    float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
    float gravityMultiplier=3,groundLevel=0,restitution=.55f;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    std::uint32_t particleFlags198=0;
    bool randomMirrorX=false,randomMirrorY=false;
    float minimumInitialSize208=0,maximumInitialSize20C=5,initialSizeToSpeedRatio210=.01f;
    float minimumLife214=500,maximumLife218=3500,minimumPercentLife21C=.5f;
    float minimumPercentLifeConvergance220=1,lifeToSpeedRatio224=5,initialPercentAge228=0;
    float minimumEmitRate22C=0,maximumEmitRate230=10,particlesToDistanceRatio234=.01f;
    float particlesToSpeedRatio238=0,minimumEmissionSpeed23C=0;
    std::array<float,3> minRange200{},maxRange210{};
    float authoredAgeFraction1A0=0,velocityY224=0,minDeath348=1,maxDeath34C=1,emberFrames220=7;
    bool applySpread228=true,smoke=false,parsed=false;
};
namespace original_racer_ember_definition_detail50LA {
inline float bits(std::uint32_t u)noexcept{float f;std::memcpy(&f,&u,4);return f;}
inline bool same(const std::string& a,const char* b)noexcept{
    if(a.size()!=std::strlen(b))return false;
    for(std::size_t i=0;i<a.size();++i){auto lower=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
        if(lower(a[i])!=lower(b[i]))return false;}return true;
}
// This bounded native adapter omits retail parser allocation, assertion
// process guards and logging, not claimed no-op guest calls. Exact original
// members and keys are required; colon annotations are NOT renamed aliases.
inline OriginalGcSprayStatus50KG read(const std::vector<std::uint8_t>& input,bool smoke,
    OriginalRacerEmberDefinition50LA& output,assets::OriginalUiPageDiagnostic50KG& d)noexcept{
    using S=OriginalGcSprayStatus50KG;using Section=assets::OriginalUiDatSection50KG;
    using Math=water_wake_detail50jc::Math;
    d={};if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    try{
        std::vector<Section> doc;
        if(!readOriginalGcParticleText50KG(input,smoke?1288u:853u,smoke?
            "fe4cea4faf473528b29997f72cad85763ebea14e1f0f20b9c0c41ec97291bf9a":
            "60a4cfae9bb4191046b802e20e0406755b744031eef707a39567fdd8103a6497",doc))return S::assetIdentity;
        const Section absent{};
        auto section=[&](const char* name)->const Section&{for(const auto& s:doc)if(same(s.name,name))return s;return absent;};
        const auto& general=section("General Info");const auto& properties=section("Properties");
        const auto& animation=section("Animation");OriginalRacerEmberDefinition50LA f;f.smoke=smoke;
        auto n=[&](const Section& s,const char* key,float fallback,float& value,std::uint32_t pc){
            return assets::originalUiNumber50KG(s,key,fallback,value,d,pc);};
        auto i=[&](const Section& s,const char* key,std::int32_t fallback,std::int32_t& value,std::uint32_t pc){
            return assets::originalUiInteger50KG(s,key,fallback,value,d,pc);};
        auto v=[&](const char* key,std::array<float,3>& value,std::uint32_t pc){
            return assets::originalUiTriple50KG(general,key,{},value,d,pc);};
        if(!smoke){
            std::array<float,3> velocity{};
            if(!v("Velocity",velocity,0x19c604u)||!v("MinRange",f.minRange200,0x19c628u)||
               !v("MaxRange",f.maxRange210,0x19c644u)||
               !assets::originalUiBoolean50KG(general,"ApplySpreadToVelocity",true,f.applySpread228,d,0x19c65cu)||
               !n(general,"MinDeathTime",1,f.minDeath348,0x19c678u)||
               !n(general,"MaxDeathTime",1,f.maxDeath34C,0x19c694u)||
               !n(animation,"FramesPerSet",7,f.emberFrames220,0x19c6fcu))return S::authoredData;
            f.velocityY224=velocity[1];
        }
        if(!i(general,"ParticleGroup",-1,f.particleGroup1B8,0x15aee0)||
           !n(general,"ParticleLife",1000,f.particleLife17C,0x15af70)||
           !n(general,"InitialSize",1,f.initialSize184,0x15af8c)||
           !n(general,"Growth",0,f.growth180,0x15afa4)||
           !n(general,"EmitRate",500,f.emitRate188,0x15afc0)||
           !n(general,"AgeAtBirth",0,f.ageAtBirth1A0,0x15afd8)||
           !v("Velocity",f.velocity60,0x15b004)||
           !n(general,"GroundLevel",std::numeric_limits<float>::max(),f.groundLevel,0x15b064)||
           !n(general,"GravityMultiplier",3,f.gravityMultiplier,0x15b080)||
           !n(general,"Restitution",bits(0x3f0ccccdu),f.restitution,0x15b0a0))return S::authoredData;
        const auto timing=readOriginalGcCommonEmitterTiming50KG(input,f.timing,d);
        if(timing!=S::complete)return timing;
        f.authoredAgeFraction1A0=f.ageAtBirth1A0;
        Math m;f.ageAtBirth1A0=m.mul(f.ageAtBirth1A0,f.particleLife17C,0x15aff8);
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        const char* keys[]={"Fades","HasVelocity","Grows","FadeNonLinear","Glows","Additive","Gravity","Lit"};
        const std::uint32_t masks[]={2,4,8,0x1000,0x2000,1,0x20,0x800};
        const std::uint32_t pcs[]={0x15b1e8,0x15b200,0x15b218,0x15b230,0x15b248,0x15b260,0x15b278,0x15b290};
        for(unsigned k=0;k<8;++k){bool value=k==7;
            if(!assets::originalUiBoolean50KG(properties,keys[k],k==7,value,d,pcs[k]))return S::authoredData;
            if(k==7)value=!value;if(value)f.particleFlags198|=masks[k];
        }
        if(!assets::originalUiBoolean50KG(properties,"RandomMirrorAboutX",false,f.randomMirrorX,d,0x15b2a8)||
           !assets::originalUiBoolean50KG(properties,"RandomMirrorAboutY",false,f.randomMirrorY,d,0x15b2c0)||
           !n(animation,"FramesPerSet",-1,f.framesPerSet,0x15b490))return S::authoredData;
        if(f.groundLevel!=std::numeric_limits<float>::max())f.particleFlags198|=0x200u;
        // The unused1/0 at15B4B0 has no consumer for the actual FramesPerSet0
        // members. Its flag400 is clear. Preserve that inactive admission.
        f.inverseFramesPerSet=f.framesPerSet==0?0:m.div(1,f.framesPerSet,0x15b4b0);
        if(f.framesPerSet>1)f.particleFlags198|=0x400u;
        std::int32_t forced=0;
        if(!i(animation,"NumberOfForcedSets",0,forced,0x15b4c0))return S::authoredData;
        if(forced>0){d.sourcePC=0x15b508;return S::unsupported;}
        if(smoke){
        if(!n(general,"MinimumPercentLife",.5f,f.minimumPercentLife21C,0x15c378)||
           !n(general,"MinimumPercentLifeConvergance",1,f.minimumPercentLifeConvergance220,0x15c438)||
           !n(general,"MinimumInitialSize",0,f.minimumInitialSize208,0x15c4d8)||
           !n(general,"MaximumInitialSize",5,f.maximumInitialSize20C,0x15c57c)||
           !n(general,"InitialSizeToSpeedRatio",bits(0x3c23d70au),f.initialSizeToSpeedRatio210,0x15c624)||
           !n(general,"MinimumLife",500,f.minimumLife214,0x15c6c8)||
           !n(general,"MaximumLife",3500,f.maximumLife218,0x15c770)||
           !n(general,"LifeToSpeedRatio",5,f.lifeToSpeedRatio224,0x15c814)||
           !n(general,"MinimumEmitRate",0,f.minimumEmitRate22C,0x15c8b4)||
           !n(general,"MaximumEmitRate",10,f.maximumEmitRate230,0x15c958)||
           !n(general,"ParticlesToDistanceRatio",bits(0x3c23d70au),f.particlesToDistanceRatio234,0x15ca00)||
           !n(general,"ParticlesToSpeedRatio",0,f.particlesToSpeedRatio238,0x15caa0)||
           !n(general,"InitialPercentAge",0,f.initialPercentAge228,0x15cb40)||
           !n(general,"MinimumEmissionSpeed",0,f.minimumEmissionSpeed23C,0x15cbe0)||
           !v("MinimumVelocityNoise",f.minimumVelocityNoise240,0x15cc84)||
           !v("MaximumVelocityNoise",f.maximumVelocityNoise250,0x15cca0))return S::authoredData;
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        // The authenticated smoke source member are in these asserted domains.
        // Keep bad authored values visible rather than silently clamp them.
        if(f.particleGroup1B8<0||f.minimumPercentLife21C<0||f.minimumPercentLife21C>1||
           f.minimumPercentLifeConvergance220<0||f.minimumInitialSize208<0||f.maximumInitialSize20C<0||
           f.initialSizeToSpeedRatio210<0||f.minimumLife214<0||f.maximumLife218<0||
           f.lifeToSpeedRatio224<0||f.minimumEmitRate22C<0||f.maximumEmitRate230<0||
           f.particlesToDistanceRatio234<0||f.particlesToSpeedRatio238<0||f.initialPercentAge228<0||
           f.minimumEmissionSpeed23C<0){d.sourcePC=0x15c3f4;return S::authoredData;}
        }
        if(m.badPC){d.sourcePC=m.badPC;return S::arithmetic;}
        f.parsed=true;output=f;return S::complete;
    }catch(...){return S::allocation;}
}
} // namespace original_racer_ember_definition_detail50LA
} // namespace rr::game
