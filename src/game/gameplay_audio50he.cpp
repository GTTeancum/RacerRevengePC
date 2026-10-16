#include "gameplay_audio50he.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
float fp(std::uint32_t word) noexcept {float x;std::memcpy(&x,&word,4);return x;}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
float sourceSin(float x) noexcept {
    if(x>fp(0x3fc90fdbu))x=sub(fp(0x40490fdbu),x);
    else if(x<fp(0xbfc90fdbu))x=sub(fp(0xc0490fdbu),x);
    const float d=mul(x,x);
    float t=add(fp(0xb9500d01u),mul(fp(0x3638ef1du),d));
    t=add(fp(0x3c088889u),mul(d,t));t=add(fp(0xbe2aaaabu),mul(d,t));
    return add(x,mul(mul(x,d),t));
}
bool finite(float x) noexcept{return std::isfinite(x);}
bool pitchWord(float x,std::uint32_t& out) noexcept {
    if(!finite(x)||x<0.f||x>=2147483648.f)return false;
    out=static_cast<std::uint32_t>(x);return true;
}
AudioEventStatus50HE publish(const AudioCommand50HE* commands,std::size_t used,
    AudioCommand50HE* out,std::size_t capacity,std::size_t& count) noexcept {
    if(used>capacity||(used&&!out))return AudioEventStatus50HE::Capacity;
    for(std::size_t n=0;n<used;++n)out[n]=commands[n];
    count=used;return AudioEventStatus50HE::Complete;
}
}
AudioEventStatus50HE updatePodWarnings50HE(const PodWarningInput50HE& in,
    const PodWarningState50HE& before,PodWarningState50HE& after,
    AudioCommand50HE* out,std::size_t capacity,std::size_t& count) noexcept {
    PodWarningState50HE next=before;AudioCommand50HE commands[5]{};std::size_t n=0;
    const auto transition=[&](std::uint8_t bit,std::uint32_t key,bool active,
        std::uint32_t start,std::uint32_t stop) {
        if(active==bool(next.flags4A4&bit))return;
        next.flags4A4=static_cast<std::uint8_t>((next.flags4A4&~bit)|(active?bit:0));
        commands[n++]={active?AudioOperation50HE::Start:AudioOperation50HE::Stop,
            {AudioBank50HE::PodEvent,key},1.f,1.f,true,active?start:stop};
    };
    if(in.flagsCD8&1u) {
        transition(4,4,false,0,0x19f25c);transition(8,3,false,0,0x19f294);
        transition(16,5,false,0,0x19f2cc);transition(32,6,false,0,0x19f304);
    } else if(!(in.emitter22C&4u)&&!(in.manager1B0&16u)) {
        if(!finite(in.damage7D0)||!finite(in.damage7D4)||!finite(in.damage7DC)||
            in.vehicle64>2u)return AudioEventStatus50HE::Input;
        const float damage=std::max(std::max(in.damage7D0,in.damage7D4),in.damage7DC);
        transition(4,4,!(damage<fp(0x3f333333u)),0x19eff8,0x19f038);
        transition(8,3,in.vehicle64==2u,0x19f07c,0x19f0bc);
        transition(16,5,(in.flags10CC&2u)!=0,0x19f100,0x19f140);
        if(in.sprayInputKnown)transition(32,6,in.probe118!=0,0x19f184,0x19f1c4);
        if(in.sprayInputKnown&&(next.flags4A4&32u)) {
            const float percent=mul(100.f,in.speedRatio70);
            if(!finite(percent)||percent<=-2147483648.f||percent>=2147483648.f)
                return AudioEventStatus50HE::Input;
            const float gain=div(static_cast<float>(static_cast<std::int32_t>(percent)),100.f);
            commands[n++]={AudioOperation50HE::Controls,{AudioBank50HE::PodEvent,6},
                gain,1.f,true,0x19f210};
        }
    }
    const auto result=publish(commands,n,out,capacity,count);
    if(result==AudioEventStatus50HE::Complete)after=next;
    return result;
}
bool podEngineControls50HE(const PodEngineParameters50HE& p,const PodEngineInput50HE& in,
    PodEngineControls50HE& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!finite(in.speedRatio70)||in.speedRatio70<-2.f||
       in.speedRatio70>2.f||!finite(in.enginePitchScale484)||in.enginePitchScale484<0.f||
       !p.engineBasePitch||!p.binderBasePitch||p.engineBasePitch>0x7fffffu||p.binderBasePitch>0x7fffffu)
        return false;
    for(float x:{p.idlePitch,p.pitchRange,p.idleBinder,p.binderRange,p.engineVolume,p.binderVolume})
        if(!finite(x)||x<0.f)return false;
    // Original19F5FC is SQRT.S (46000204), not ABS.S. The retained
    // finite EE scalar-root domain takes the absolute input before sqrt.
    const float sine=sourceSin(mul(fp(0x3fc90fdbu),in.speedRatio70));
    volatile float amount=static_cast<float>(std::sqrt(static_cast<double>(std::fabs(sine))));
    const float engineIdle=mul(p.idlePitch,static_cast<float>(p.engineBasePitch));
    const float binderIdle=mul(p.idleBinder,static_cast<float>(p.binderBasePitch));
    const float engineRange=sub(mul(engineIdle,p.pitchRange),engineIdle);
    const float binderRange=sub(mul(binderIdle,p.binderRange),binderIdle);
    PodEngineControls50HE result{};
    if(!pitchWord(std::max(add(engineIdle,mul(engineRange,amount)),engineIdle),result.enginePitchWord)||
       !pitchWord(std::max(add(binderIdle,mul(binderRange,amount)),binderIdle),result.binderPitchWord))return false;
    if(in.boostPitch) {
        if(!pitchWord(mul(static_cast<float>(result.enginePitchWord),fp(0x3f933333u)),result.enginePitchWord)||
           !pitchWord(mul(static_cast<float>(result.binderPitchWord),fp(0x3f933333u)),result.binderPitchWord))return false;
    }
    if(!pitchWord(mul(static_cast<float>(result.enginePitchWord),in.enginePitchScale484),result.enginePitchWord))return false;
    result.enginePitch=div(static_cast<float>(result.enginePitchWord),static_cast<float>(p.engineBasePitch));
    result.binderPitch=div(static_cast<float>(result.binderPitchWord),static_cast<float>(p.binderBasePitch));
    result.engineGain=div(mul(fp(0x3ee66666u),p.engineVolume),100.f);
    result.binderGain=div(mul(fp(0x3ee66666u),p.binderVolume),100.f);
    if(!finite(result.engineGain)||!finite(result.binderGain)||!finite(result.enginePitch)||!finite(result.binderPitch))return false;
    out=result;return true;
}
AudioEventStatus50HE updatePodBoostAudio50HE(bool held,float heating,std::uint32_t previous64,
    const PodBoostAudioState50HE& before,PodBoostAudioState50HE& after,
    AudioCommand50HE* out,std::size_t capacity,std::size_t& count) noexcept {
    if(previous64>2u||(held&&!finite(heating)))return AudioEventStatus50HE::Input;
    auto next=before;AudioCommand50HE command{};std::size_t n=0;
    if(held) {
        if(heating>0.f&&previous64==0u) {
            next.flags22C=static_cast<std::uint8_t>(next.flags22C|2u);n=1;
            command={AudioOperation50HE::Start,{AudioBank50HE::PodEngine,3},.9f,1.f,false,0x1a01f4};
        }
    }else if(previous64!=0u)next.flags22C=static_cast<std::uint8_t>(next.flags22C&~2u);
    const auto result=publish(&command,n,out,capacity,count);
    if(result==AudioEventStatus50HE::Complete)after=next;
    return result;
}
AudioEventStatus50HE repairCompletionAudio50HE(std::uint8_t flags,std::uint32_t vehicle3C,
    const float (&damage)[4],AudioCommand50HE* out,std::size_t capacity,std::size_t& count) noexcept {
    if(!(flags&2u)){count=0;return AudioEventStatus50HE::Complete;}
    // Exact short-circuit source order7D4,7D0,7DC,7D8.
    for(unsigned index:{1u,0u,3u,2u}) {
        if(!finite(damage[index]))return AudioEventStatus50HE::Input;
        if(!(damage[index]<=0.f)){count=0;return AudioEventStatus50HE::Complete;}
    }
    if(vehicle3C){count=0;return AudioEventStatus50HE::Complete;}
    const AudioCommand50HE command{AudioOperation50HE::Start,{AudioBank50HE::PodEvent,2},1.f,1.f,false,0x1a24d8};
    return publish(&command,1,out,capacity,count);
}
}
