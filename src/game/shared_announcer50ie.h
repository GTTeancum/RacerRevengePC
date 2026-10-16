#pragma once
// Bounded original2C2C20 owner for bound original racers' requests1..8/12. Effects
// are synchronous provider calls; resumable phases retain already-returned
// effects/RNG if a later provider is unavailable. No host speech policy here.
#include <array>
#include "original_racer_selection50ij.h"
#include "shared_random_owner50id.h"
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {
struct AnnouncerParameters50IE {float genericPercentBC=0.f,speechDelayC0=0.f;};
struct AnnouncerState50IE {
    float timerB0=2.f,genericPercentBC=.7f,speechDelayC0=4.f;
    std::int32_t priorityB4=-1;
    std::uint32_t episodeAC=0,namedB8=0;
    std::int8_t pendingKey19C=-1,pendingRacer19D=-1,pendingPriority19E=-1;
    std::array<std::array<std::int8_t,8>,27> recentC4{};
    bool initialized=false;
};
struct AnnouncerRequest50IE {std::uint32_t type=0,racer60=0,variant=0;};
struct AnnouncerGate50IE {std::uint32_t mode11258=0;std::uint8_t managerFlags58=0;};
struct AnnouncerLeader50IE {
    bool exists=false;
    std::uint32_t vehicle58=0;
    float remainingDistance34=0.f,cachedSpeed34=0.f;
    // Provenance only: source row uses authored DistanceToFinish plus distance
    // to a horizontal node cross-section and +1 tie adjustment. The accepted
    // native world may supply its explicitly HOST remaining-distance observer.
    bool hostRemainingProjection=false;
};
inline bool announcerLeaderTail50IE(const AnnouncerLeader50IE&in,bool&out)noexcept {
    if(std::fegetround()!=FE_TONEAREST)return false;
    if(!in.exists){out=false;return true;}
    if(!std::isfinite(in.remainingDistance34)||!std::isfinite(in.cachedSpeed34))return false;
    // 2C4E24=46010043 DIV.S f1,f0,f1. Preserve strict comparison and IEEE
    // zero-speed infinity/NaN behavior; do not turn it into distance-minus-speed.
    volatile float seconds=in.remainingDistance34/in.cachedSpeed34;
    out=seconds<10.f&&in.vehicle58==0u;return true;
}
enum class AnnouncerStatus50IE:std::uint8_t {Complete,NeedProvider,Input,UnsupportedRequest,SourceAssertion};
enum class AnnouncerSuppression50IE:std::uint8_t {None,Mode,Pending,Ended,DelayOrPriority,QueuePriority};
enum class AnnouncerBoundary50IE:std::uint8_t {None,StopAll,End38,Begin34,SelectorSeed,Play,Activity,LeaderTail};
struct AnnouncerResult50IE {
    AnnouncerStatus50IE status=AnnouncerStatus50IE::Input;
    AnnouncerBoundary50IE boundary=AnnouncerBoundary50IE::None;
    std::uint32_t sourcePC=0;
};
struct AnnouncerProviders50IE {
    void* context=nullptr;
    bool (*stopAll)(void*)noexcept=nullptr;
    bool (*end38)(void*)noexcept=nullptr;
    bool (*begin34)(void*)noexcept=nullptr;
    // true means original play helper returned, including genuine unavailable
    // PCM. started reports actual mixer action separately; no fake clip needed.
    bool (*play)(void*,std::uint32_t bank,std::uint32_t key,bool& started)noexcept=nullptr;
    bool (*activity)(void*,bool& anyActive)noexcept=nullptr;
    // SAME application lazy80A8/seed3D6B30 and clock as category6. Called before
    // EVERY variant attempt. Probability308030 does not call this beforehand.
    bool (*ensureSelectorSeed)(void*,std::uint32_t& sharedRaw)noexcept=nullptr;
    bool (*leaderTail)(void*,bool& nearHumanLeader)noexcept=nullptr;
    // Optional only for legacy stand-alone consumers. Live source providers
    // borrow the SAME application80AC/3D6B38 as collision/lap selectors.
    DynamicContactRangeOwner50IW* sharedRange=nullptr;
};
enum class AnnouncerPhase50IE:std::uint8_t {
    Idle,Gate,QueueChoose,Stop,End,Begin,ImmediateChoose,Select,Selected,Play,CommitEpisode,Done,Fault
};
struct AnnouncerCursor50IE {
    AnnouncerPhase50IE phase=AnnouncerPhase50IE::Idle;
    AnnouncerRequest50IE request{};AnnouncerGate50IE gate{};
    AnnouncerSuppression50IE suppression=AnnouncerSuppression50IE::None;
    std::uint32_t priority=0,bank=0,key=0,attempt=0,draws=0;
    std::uint32_t faultPC=0;
    bool queue=false,fallbackGeneric=false,writeNamedB8=false;
    bool admitted=false,queued=false,selected=false,started=false;
};
namespace shared_announcer50IE_detail {
inline float mul(float a,float b)noexcept{volatile float v=a*b;return v;}
inline float sub(float a,float b)noexcept{volatile float v=a-b;return v;}
inline float draw(std::uint32_t&raw)noexcept {
    raw=raw*0x41c64e6du+0x3039u;
    volatile float value=static_cast<float>(raw&0x7fffffffu);
    return mul(value,0x1p-31f);
}
inline bool valid(const AnnouncerState50IE&s)noexcept {
    return s.initialized&&std::isfinite(s.timerB0)&&std::isfinite(s.genericPercentBC)&&
        s.genericPercentBC>=0.f&&s.genericPercentBC<=1.f&&
        std::isfinite(s.speechDelayC0)&&s.speechDelayC0>=0.f;
}
inline bool recent(const AnnouncerState50IE&s,unsigned bank,unsigned key)noexcept {
    for(const auto b:s.recentC4[bank])if(static_cast<int>(b)==static_cast<int>(key))return true;
    return false;
}
inline void remember(AnnouncerState50IE&s,unsigned bank,unsigned key)noexcept {
    auto&row=s.recentC4[bank];for(unsigned i=7;i;i--)row[i]=row[i-1];
    row[0]=static_cast<std::int8_t>(key<128u?static_cast<int>(key):static_cast<int>(key)-256);
}
inline AnnouncerResult50IE done(std::uint32_t pc=0x2c2dcc)noexcept{return {AnnouncerStatus50IE::Complete,AnnouncerBoundary50IE::None,pc};}
inline AnnouncerResult50IE need(AnnouncerBoundary50IE b,std::uint32_t pc)noexcept{return {AnnouncerStatus50IE::NeedProvider,b,pc};}
inline bool namedSpec(unsigned type,unsigned&base,unsigned&count)noexcept {
    switch(type){
    case 1:base=1;count=3;return true;case 2:base=4;count=3;return true;
    case 3:base=7;count=1;return true;case 4:base=8;count=1;return true;
    case 5:base=9;count=2;return true;case 6:base=11;count=1;return true;
    case 7:base=12;count=2;return true;case 8:base=14;count=2;return true;
    case 12:base=24;count=3;return true;default:return false;}
}
inline bool genericSpec(unsigned type,unsigned&base,unsigned&count)noexcept {
    if(type==1){base=1;count=4;return true;}
    if(type==2){base=5;count=28;return true;}
    if(type==6){base=33;count=19;return true;}
    if(type==12){base=128;count=23;return true;}return false;
}
inline void select(AnnouncerCursor50IE&c,bool named)noexcept {
    c.bank=named?c.request.racer60:0;c.key=0;c.attempt=0;c.phase=AnnouncerPhase50IE::Select;
}
}
inline bool initializeAnnouncer50IE(const AnnouncerParameters50IE&p,AnnouncerState50IE&out)noexcept {
    if(out.initialized||!std::isfinite(p.genericPercentBC)||p.genericPercentBC<0.f||p.genericPercentBC>1.f||
       !std::isfinite(p.speechDelayC0)||p.speechDelayC0<0.f)return false;
    AnnouncerState50IE s;s.genericPercentBC=p.genericPercentBC;s.speechDelayC0=p.speechDelayC0;
    for(auto&row:s.recentC4)row.fill(-1);
    s.initialized=true;out=s;return true;
}
inline bool beginAnnouncerRequest50IE(const AnnouncerRequest50IE&r,const AnnouncerGate50IE&g,
                                    AnnouncerCursor50IE&c)noexcept {
    if(c.phase!=AnnouncerPhase50IE::Idle&&c.phase!=AnnouncerPhase50IE::Done)return false;
    // Cold providers authenticate the actual racers' named banks.
    // Constructor21079C/21081C/210820 ties racer60 to original Pod%02d, not
    // the host's array position. The live provider separately binds its roster.
    if(!((r.type>=1&&r.type<=8)||r.type==12)||
        !original_racer_selection50IJ::findByRacer60(r.racer60)||r.variant>2)return false;
    //2C48B0 falls through to priority1 for collision types1/2 and type12.
    c={};c.request=r;c.gate=g;c.priority=(r.type<=2||r.type==12)?1:9;
    c.phase=AnnouncerPhase50IE::Gate;return true;
}
// Source2C2DF0 ->2C2C20 ->2C4910/4840/4AB0. A provider returning false leaves
// the exact current phase pending. Do not begin another request or update the
// owner until this cursor completes; the surrounding source FIFO stays held.
inline AnnouncerResult50IE resumeAnnouncerRequest50IE(AnnouncerState50IE&s,
    AnnouncerCursor50IE&c,const AnnouncerProviders50IE&p,std::uint32_t&sharedRaw)noexcept {
    using namespace shared_announcer50IE_detail;using P=AnnouncerPhase50IE;using B=AnnouncerBoundary50IE;
    if(std::fegetround()!=FE_TONEAREST||!valid(s))return {};
    for(;;)switch(c.phase){
    case P::Gate:{
        auto reject=[&](AnnouncerSuppression50IE why){c.suppression=why;c.phase=P::Done;};
        if(c.gate.mode11258!=1){reject(AnnouncerSuppression50IE::Mode);return done(0x2c2e28);}
        if(s.pendingKey19C>=0){reject(AnnouncerSuppression50IE::Pending);return done(0x2c2e3c);}
        if((c.gate.managerFlags58&1u)&&!(c.request.type>=6&&c.request.type<=8)){
            reject(AnnouncerSuppression50IE::Ended);return done(0x2c2e80);}
        if(!(s.timerB0<=0.f&&s.episodeAC==0)&&
           (c.priority<2||s.priorityB4>=static_cast<std::int32_t>(c.priority))){
            reject(AnnouncerSuppression50IE::DelayOrPriority);return done(0x2c2f1c);}
        s.priorityB4=static_cast<std::int32_t>(c.priority);c.admitted=true;
        c.queue=s.episodeAC!=0;c.phase=c.queue?P::QueueChoose:P::Stop;break;
    }
    case P::QueueChoose:
        if(static_cast<int>(s.pendingPriority19E)>=static_cast<int>(c.priority)){
            c.suppression=AnnouncerSuppression50IE::QueuePriority;c.phase=P::Done;return done(0x2c4a94);}
        c.fallbackGeneric=c.request.variant==0;
        select(c,c.request.variant!=1);break;
    case P::Stop:
        if(!p.stopAll||!p.stopAll(p.context))return need(B::StopAll,0x2c2c80);
        s.priorityB4=-1;c.phase=P::End;break;
    case P::End:
        if(!p.end38||!p.end38(p.context))return need(B::End38,0x2c4f74);
        if(c.request.type>=6&&c.request.type<=8){
            s.pendingKey19C=-1;s.pendingRacer19D=-1;s.pendingPriority19E=100;}
        c.phase=P::Begin;break;
    case P::Begin:
        if(!p.begin34||!p.begin34(p.context))return need(B::Begin34,0x2c2cb8);
        c.phase=P::ImmediateChoose;break;
    case P::ImmediateChoose:
        if((c.request.type>=3&&c.request.type<=5)||c.request.type==7||c.request.type==8){select(c,true);break;}
        if(c.request.variant==1){select(c,false);break;}
        if(c.request.variant==2){select(c,true);break;}
        c.writeNamedB8=true;
        if(s.namedB8){select(c,false);break;}
        {const auto fraction=draw(sharedRaw);++c.draws;select(c,!(fraction<s.genericPercentBC));}break;
    case P::Select:{
        unsigned base=0,count=0;
        if(!(c.bank?namedSpec(c.request.type,base,count):genericSpec(c.request.type,base,count))){
            c.faultPC=c.bank?0x2c4768u:0x2c3758u;c.phase=P::Fault;
            return {AnnouncerStatus50IE::SourceAssertion,B::None,c.faultPC};}
        if(!p.ensureSelectorSeed||!p.ensureSelectorSeed(p.context,sharedRaw))
            return need(B::SelectorSeed,c.bank?0x2c3830u:0x2c2f30u);
        const float chosen=mul(static_cast<float>(count),draw(sharedRaw));++c.draws;++c.attempt;
        // Original range guard initializes AFTER this committed draw. Do not
        // seed or draw again while retaining the same source selector attempt.
        bool checkRange=true;
        if(p.sharedRange){
            if(!p.sharedRange->lazy80AC){
                p.sharedRange->rangeCheck3D6B38=1;p.sharedRange->lazy80AC=true;
            }
            checkRange=p.sharedRange->rangeCheck3D6B38!=0;
        }
        if(!std::isfinite(chosen)||chosen<0.f||chosen>static_cast<float>(count)||
           (checkRange&&chosen>=static_cast<float>(count))){
            c.faultPC=0x133660;c.phase=P::Fault;
            return {AnnouncerStatus50IE::SourceAssertion,B::None,c.faultPC};}
        const unsigned candidate=base+static_cast<unsigned>(chosen);
        const bool seen=recent(s,c.bank,candidate);
        // Named: count+1 attempts, final attempt always discarded. Generic:
        // count attempts, final attempt always discarded (2C37F0 differs).
        const bool final=c.bank?c.attempt>count:c.attempt>=count;
        if(seen&&!final)break;
        c.key=final?0:candidate;c.phase=P::Selected;break;
    }
    case P::Selected:
        if(!c.key&&c.queue&&c.fallbackGeneric){c.fallbackGeneric=false;select(c,false);break;}
        c.selected=c.key!=0;
        if(c.queue){
            if(c.key){
                s.pendingKey19C=static_cast<std::int8_t>(c.key);
                s.pendingRacer19D=static_cast<std::int8_t>(c.bank);
                s.pendingPriority19E=static_cast<std::int8_t>(c.priority);
                remember(s,c.bank,c.key);c.queued=true;
            }
            c.phase=P::Done;return done(0x2c4a94);
        }
        if(c.key){remember(s,c.bank,c.key);c.phase=P::Play;}
        else c.phase=P::CommitEpisode;
        break;
    case P::Play:
        if(!p.play||!p.play(p.context,c.bank,c.key,c.started))return need(B::Play,c.bank?0x2c4b00u:0x2c4880u);
        c.phase=P::CommitEpisode;break;
    case P::CommitEpisode:
        if(c.writeNamedB8)s.namedB8=c.bank?1u:0u;
        s.episodeAC=1;c.phase=P::Done;return done();
    case P::Done:return done();
    case P::Fault:return {AnnouncerStatus50IE::SourceAssertion,B::None,c.faultPC};
    default:return {};
    }
}

enum class AnnouncerUpdatePhase50IE:std::uint8_t {Idle,Activity,PendingPlay,End,Leader,Done};
struct AnnouncerUpdateCursor50IE {
    AnnouncerUpdatePhase50IE phase=AnnouncerUpdatePhase50IE::Idle;
    bool played=false,finished=false;
};
// Native scheduler calls once per accepted world tick in SOURCE seconds. This
// is the2C4C9C..2C4D7C owner slice, not a claim about original global scene order.
inline bool beginAnnouncerUpdate50IE(float factor,AnnouncerState50IE&s,
                                    AnnouncerUpdateCursor50IE&c)noexcept {
    using P=AnnouncerUpdatePhase50IE;
    if((c.phase!=P::Idle&&c.phase!=P::Done)||!shared_announcer50IE_detail::valid(s)||
       std::fegetround()!=FE_TONEAREST||!std::isfinite(factor)||factor<0.f)return false;
    if(s.timerB0>0.f){const auto next=shared_announcer50IE_detail::sub(s.timerB0,factor);
        if(!std::isfinite(next))return false;s.timerB0=next;}
    c={};c.phase=s.episodeAC?P::Activity:P::Leader;return true;
}
inline AnnouncerResult50IE resumeAnnouncerUpdate50IE(AnnouncerState50IE&s,
    AnnouncerUpdateCursor50IE&c,const AnnouncerProviders50IE&p)noexcept {
    using namespace shared_announcer50IE_detail;using P=AnnouncerUpdatePhase50IE;using B=AnnouncerBoundary50IE;
    if(!valid(s))return {};
    for(;;)switch(c.phase){
    case P::Activity:{
        bool active=false;if(!p.activity||!p.activity(p.context,active))return need(B::Activity,0x2c4ccc);
        c.phase=active?P::Leader:(s.pendingKey19C>=0?P::PendingPlay:P::End);break;
    }
    case P::PendingPlay:
        if(!p.play||!p.play(p.context,s.pendingRacer19D>0?static_cast<unsigned>(s.pendingRacer19D):0u,
                static_cast<unsigned>(s.pendingKey19C),c.played))return need(B::Play,0x2c4d04);
        s.pendingKey19C=-1;s.pendingRacer19D=-1;s.pendingPriority19E=-1;c.phase=P::Leader;break;
    case P::End:
        if(!p.end38||!p.end38(p.context))return need(B::End38,0x2c4d44);
        s.timerB0=s.speechDelayC0;s.priorityB4=-1;s.episodeAC=0;c.finished=true;c.phase=P::Leader;break;
    case P::Leader:{
        bool leader=false;if(!p.leaderTail||!p.leaderTail(p.context,leader))return need(B::LeaderTail,0x2c4d64);
        if(leader)s.priorityB4=5;c.phase=P::Done;return done(0x2c4d7c);
    }
    case P::Done:return done(0x2c4d7c);
    default:return {};
    }
}
} // namespace rr::game
