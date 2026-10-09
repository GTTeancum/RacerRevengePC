#include "pod_speech50hg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {namespace {
float fp(std::uint32_t bits) noexcept {float v;std::memcpy(&v,&bits,4);return v;}
std::uint32_t bits(float v) noexcept {std::uint32_t b;std::memcpy(&b,&v,4);return b;}
float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
float add(float a,float b) noexcept {volatile float v=a+b;return v;}
bool finite(float v) noexcept{return std::isfinite(v);}
bool countsValid(const PodSpeechCounts50HG& c) noexcept {
    return c.lowDamage==4&&c.wallHit==4&&c.passed==6&&c.attacked==5&&
        c.passing==6&&c.attacking==6&&c.death==4&&c.win==3&&c.lose==2;
}
std::uint32_t draw(std::uint32_t& raw) noexcept {
    raw=raw*0x41c64e6du+0x3039u;return raw&0x7fffffffu;
}
float fraction(std::uint32_t& raw) noexcept {
    volatile float value=static_cast<float>(draw(raw));return mul(fp(0x30000000u),value);
}
bool randomIndex(std::uint32_t& raw,std::uint8_t count,std::uint8_t& index) noexcept {
    const auto value=mul(static_cast<float>(count),fraction(raw));
    if(!finite(value)||value<0.f||value>=static_cast<float>(count))return false;
    index=static_cast<std::uint8_t>(value);return true;
}
bool audible(const SpeechPosition50HG& source,const SpeechListeners50HG& listeners,bool& result) noexcept {
    if(listeners.count>2||(listeners.count&&!listeners.positions))return false;
    result=false;if(!listeners.count)return true;
    if(!finite(source.x)||!finite(source.z))return false;
    float nearest=0.f;
    for(std::size_t n=0;n<listeners.count;++n) {
        const auto& at=listeners.positions[n];if(!finite(at.x)||!finite(at.z))return false;
        const auto x=std::fabs(sub(at.x,source.x)),z=std::fabs(sub(at.z,source.z));
        const auto squared=add(mul(x,x),mul(z,z));
        if(!finite(squared))return false;
        volatile float distance=static_cast<float>(std::sqrt(static_cast<double>(squared)));
        if(!n||distance<nearest)nearest=distance;
    }
    result=nearest<200.f;return true;
}
bool validState(const PodSpeechCounts50HG& c,const PodSpeechState50HG& s) noexcept {
    const std::uint8_t sizes[]={c.lowDamage,c.wallHit,c.passed,c.attacked,c.passing,c.attacking};
    if(!finite(s.cooldown224))return false;
    for(unsigned n=0;n<6;++n)if(s.cursors[n]>=sizes[n])return false;
    return true;
}
}
PodSpeechStatus50HG initializePodSpeech50HG(const PodSpeechCounts50HG& c,
    std::uint32_t& raw,PodSpeechState50HG& out) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!countsValid(c)||out.initialized)return PodSpeechStatus50HG::Input;
    auto nextRaw=raw;PodSpeechState50HG state{};
    const std::uint8_t sizes[]={c.lowDamage,c.wallHit,c.passed,c.attacked,c.passing,c.attacking};
    for(unsigned n=0;n<6;++n)if(!randomIndex(nextRaw,sizes[n],state.cursors[n]))return PodSpeechStatus50HG::RandomRange;
    state.initialized=true;raw=nextRaw;out=state;return PodSpeechStatus50HG::Complete;
}
bool advanceSpeechTimers50HG(float dt,UndertrackPlayerPrefix50ET& prefix,PodSpeechState50HG& state) noexcept {
    const auto damageTimer=fp(prefix.counter109C);
    if(!state.initialized||std::fegetround()!=FE_TONEAREST||!finite(dt)||dt<0.f||
       !finite(damageTimer)||!finite(state.cooldown224))return false;
    const auto nextDamage=damageTimer>0.f?sub(damageTimer,dt):damageTimer;
    const auto nextSpeech=state.cooldown224>0.f?sub(state.cooldown224,dt):state.cooldown224;
    if(!finite(nextDamage)||!finite(nextSpeech))return false;
    prefix.counter109C=bits(nextDamage);state.cooldown224=nextSpeech;return true;
}
bool afterRepairSpeechState50HG(float a,float b,float c,UndertrackPlayerPrefix50ET& prefix) noexcept {
    if(!finite(a)||!finite(b)||!finite(c))return false;
    if(std::max(std::max(a,b),c)<fp(0x3f333333u)) {
        prefix.flags10CC=static_cast<std::uint8_t>(prefix.flags10CC&~4u);prefix.counter109C=bits(3.f);
    }
    return true;
}
PodSpeechReceipt50HG selectPodSpeech50HG(PodSpeechCategory50HG category,
    const PodSpeechCounts50HG& c,const SpeechPosition50HG& position,const SpeechListeners50HG& listeners,
    std::uint32_t& raw,PodSpeechState50HG& state,AudioCommand50HE* commands,
    std::size_t capacity,std::size_t& count) noexcept {
    PodSpeechReceipt50HG r{};
    if(!state.initialized){r.status=PodSpeechStatus50HG::Uninitialized;return r;}
    if(std::fegetround()!=FE_TONEAREST||!countsValid(c)||!validState(c,state)||
       static_cast<unsigned>(category)>static_cast<unsigned>(PodSpeechCategory50HG::Lose)||
       !audible(position,listeners,r.audible)) {r.status=PodSpeechStatus50HG::Input;return r;}
    const auto which=static_cast<unsigned>(category);
    if(!r.audible||(which<6&&state.cooldown224>0.f)){count=0;return r;}
    if(!capacity||!commands){r.status=PodSpeechStatus50HG::Capacity;return r;}
    auto next=state;auto nextRaw=raw;AudioCue50HE cue{};
    if(which<6) {
        const std::uint8_t sizes[]={c.lowDamage,c.wallHit,c.passed,c.attacked,c.passing,c.attacking};
        const std::uint32_t first[]={0u,c.lowDamage,std::uint32_t(c.lowDamage)+c.wallHit,
            std::uint32_t(c.lowDamage)+c.wallHit+c.passed,0u,c.passing};
        const auto selected=static_cast<std::uint32_t>(next.cursors[which])+1u;
        next.cursors[which]=static_cast<std::uint8_t>(selected%sizes[which]);
        cue={which<4?AudioBank50HE::PodCurses:AudioBank50HE::PodTaunts,first[which]+selected};
        next.cooldown224=5.f;r.sourceSite=which<4?0x2c1bd0:0x2c1cb8;
    } else {
        const std::uint8_t sizes[]={c.death,c.win,c.lose};
        const std::uint32_t first[]={0u,c.death,std::uint32_t(c.death)+c.win};
        std::uint8_t index=0;
        if(!randomIndex(nextRaw,sizes[which-6],index)){r.status=PodSpeechStatus50HG::RandomRange;return r;}
        cue={AudioBank50HE::PodSpecial,first[which-6]+index+1u};r.randomDraws=1;r.sourceSite=0x2c2028;
    }
    commands[0]={AudioOperation50HE::Start,cue,1.f,1.f,false,r.sourceSite};
    count=1;state=next;raw=nextRaw;r.emitted=true;return r;
}
DamageSpeechReceipt50HG produceDamageSpeechEvent50HG(const DamageSpeechInput50HG& in,
    UndertrackPlayerPrefix50ET& prefix,DamageSpeechEvent50HG& event) noexcept {
    DamageSpeechReceipt50HG r{};
    if(!finite(in.damage7D0)||!finite(in.damage7D4)||!finite(in.damage7DC)||!finite(fp(prefix.counter109C))) {
        r.status=PodSpeechStatus50HG::Input;return r;
    }
    const auto damage=std::max(std::max(in.damage7D0,in.damage7D4),in.damage7DC);
    if(damage<=fp(0x3f333333u)||fp(prefix.counter109C)>0.f)return r;
    if(event.pending){r.status=PodSpeechStatus50HG::Capacity;return r;}
    event.repeated=(prefix.flags10CC&4u)!=0;
    prefix.counter109C=bits(10.f);prefix.flags10CC=static_cast<std::uint8_t>(prefix.flags10CC|4u);
    r.eventProduced=true;r.eventAccepted=in.vehicle3C==0;
    event.pending=r.eventAccepted;return r;
}
DamageSpeechReceipt50HG dispatchDamageSpeechEvent50HG(const DamageSpeechInput50HG& in,
    const PodSpeechCounts50HG& counts,DamageSpeechEvent50HG& event,std::uint32_t& raw,
    PodSpeechState50HG& speech,AudioCommand50HE* commands,std::size_t capacity,std::size_t& count) noexcept {
    DamageSpeechReceipt50HG r{};
    if(!event.pending){count=0;return r;}
    if(!speech.initialized){r.status=PodSpeechStatus50HG::Uninitialized;return r;}
    if(std::fegetround()!=FE_TONEAREST||!countsValid(counts)||!validState(counts,speech)||
       !finite(in.playerSpeechChance138)||in.playerSpeechChance138<0.f||in.playerSpeechChance138>1.f) {
        r.status=PodSpeechStatus50HG::Input;return r;
    }
    auto nextSpeech=speech;auto nextRaw=raw;
    r.eventAccepted=true;r.eventDispatched=(in.config58&2u)&&!(in.manager1B0&16u);
    AudioCommand50HE pending{};std::size_t used=0;
    if(r.eventDispatched&&in.vehicle40==0u) {
        if(in.vehicle58==0u) {
            r.playerChancePassed=fraction(nextRaw)<in.playerSpeechChance138;++r.randomDraws;
        }
        if(r.playerChancePassed) {
            const auto selected=selectPodSpeech50HG(PodSpeechCategory50HG::LowDamage,counts,
                in.position,in.listeners,nextRaw,nextSpeech,&pending,1,used);
            if(selected.status!=PodSpeechStatus50HG::Complete){r.status=selected.status;return r;}
            r.randomDraws+=selected.randomDraws;r.voiceEmitted=selected.emitted;
        } else if(!event.repeated) {
            r.announcerRequested=true;r.announcerType=12;r.announcerArgumentKnown=in.vehicle60Known;
            r.announcerRacer=in.vehicle60Known?in.vehicle60:0xffffffffu;
            r.announcerVariant=in.vehicle60Known?(in.vehicle60==16u?1u:0u):0xffffffffu;
        }
    }
    if(used>capacity||(used&&!commands)){r.status=PodSpeechStatus50HG::Capacity;return r;}
    if(used)commands[0]=pending;
    count=used;speech=nextSpeech;raw=nextRaw;event.pending=false;return r;
}
DamageSpeechReceipt50HG damageSpeech50HG(const DamageSpeechInput50HG& in,
    const PodSpeechCounts50HG& counts,UndertrackPlayerPrefix50ET& prefix,
    std::uint32_t& raw,PodSpeechState50HG& speech,AudioCommand50HE* commands,
    std::size_t capacity,std::size_t& count) noexcept {
    auto nextPrefix=prefix;auto nextSpeech=speech;auto nextRaw=raw;DamageSpeechEvent50HG event{};
    auto produced=produceDamageSpeechEvent50HG(in,nextPrefix,event);
    if(produced.status!=PodSpeechStatus50HG::Complete)return produced;
    auto r=dispatchDamageSpeechEvent50HG(in,counts,event,nextRaw,nextSpeech,commands,capacity,count);
    if(r.status!=PodSpeechStatus50HG::Complete)return r;
    r.eventProduced=produced.eventProduced;r.eventAccepted=produced.eventAccepted;
    prefix=nextPrefix;speech=nextSpeech;raw=nextRaw;return r;
}
AudioEventStatus50HE trackIntroSpeech50HG(std::uint32_t config34,std::uint32_t camera,
    const TrackIntroLine50HG* lines,std::size_t lineCount,std::uint8_t& flags62,
    AudioCommand50HE* commands,std::size_t capacity,std::size_t& count) noexcept {
    if(config34!=2u||(!lines&&!lineCount)){count=0;return AudioEventStatus50HE::Complete;}
    if(!lines||lineCount!=3u)return AudioEventStatus50HE::Input;
    int span=(flags62>>1)&15;if(span&8)span-=16;
    if(span>0) {
        flags62=static_cast<std::uint8_t>((flags62&0xe1u)|((static_cast<unsigned>(span-1)&15u)<<1));
        count=0;return AudioEventStatus50HE::Complete;
    }
    if(!camera||camera>lineCount)return AudioEventStatus50HE::Input;
    const auto& line=lines[camera-1];
    if(line.eventKey<0){count=0;return AudioEventStatus50HE::Complete;}
    if(line.eventKey<1||line.eventKey>3||line.span>15)return AudioEventStatus50HE::Input;
    if(!commands||!capacity)return AudioEventStatus50HE::Capacity;
    commands[0]={AudioOperation50HE::Start,{AudioBank50HE::TrackIntro,static_cast<std::uint32_t>(line.eventKey)},
        1.f,1.f,false,0x175998};
    flags62=static_cast<std::uint8_t>((flags62&0xe1u)|((line.span&15u)<<1));count=1;
    return AudioEventStatus50HE::Complete;
}

}
