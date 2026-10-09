#include "lap_completion50gg.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game { namespace {
using Status=LapCompletionStatus50GG;
using Phase=LapCompletionPhase50GG;
using Call=LapCompletionCall50GG;
using Target=LapCompletionTarget50GG;
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
              std::numeric_limits<float>::digits==24,"50GG requires binary32");
LapCompletionResult50GG result(Status status,std::uint32_t pc=0) noexcept {
    LapCompletionResult50GG r{};r.status=status;r.sourcePC=pc;return r;
}
LapCompletionResult50GG waiting(const LapCompletionCursor50GG& c) noexcept {
    auto r=result(c.pending.query?Status::NeedQuery:Status::NeedEffect,c.pending.callPC);
    r.request=c.pending;return r;
}
bool room(const LapCompletionCursor50GG& c) noexcept {
    return c.lastSequence!=std::numeric_limits<std::uint64_t>::max();
}
LapCompletionResult50GG issue(LapCompletionCursor50GG& c,Call kind,Target target,
    std::uint32_t pc,std::uint32_t callee,Phase next,std::uint8_t mask=0,
    std::uint32_t a1=0,float scalar=0.0f,bool query=false) noexcept {
    if(!room(c))return result(Status::SequenceExhausted,pc);
    LapCompletionRequest50GG q{};q.kind=kind;q.target=target;
    q.sequence=++c.lastSequence;q.callPC=pc;q.calleePC=callee;q.returnPC=pc+8u;
    q.argumentA1=a1;q.argumentF12=scalar;q.argumentMask=mask;q.query=query;
    c.pending=q;c.phase=next;return waiting(c);
}
bool ordinary(float value) noexcept {
    return std::isfinite(value)&&(value==0.0f||std::isnormal(value));
}
// CVT.W.S on the EE truncates toward zero and saturates signed overflow.
// NaNs/infinities and subnormal input/output arithmetic are not admitted here.
std::int32_t truncateSaturating(float value) noexcept {
    if(value>=2147483648.0f)return std::numeric_limits<std::int32_t>::max();
    if(value<=-2147483648.0f)return std::numeric_limits<std::int32_t>::min();
    return static_cast<std::int32_t>(value);
}
LapCompletionResult50GG prepareTimes(const LapCompletionPlayer50GG& p,
    const LapCompletionConfig50GG& e,LapCompletionCursor50GG& c) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!ordinary(p.timeCB0))
        return result(Status::UnsupportedNumeric,0x1a2be4u);
    const auto lapCount=static_cast<std::uint32_t>(e.lapCount1125C&31u);
    const auto needed=lapCount?lapCount-1u:0u;
    const auto lapAddress=reinterpret_cast<std::uintptr_t>(p.lapTimesCC0);
    if(needed&&(!p.lapTimesCC0||p.lapTimeCount<needed||lapAddress%alignof(float)||
       needed*sizeof(float)>std::numeric_limits<std::uintptr_t>::max()-lapAddress))
        return result(Status::Storage,0x1a2c44u);
    volatile float time=p.timeCB0,scale=100.0f;
    volatile float scaled=time*scale;
    if(!ordinary(scaled))return result(Status::UnsupportedNumeric,0x1a2bf8u);
    const auto centiseconds=truncateSaturating(scaled);
    volatile float integral=static_cast<float>(centiseconds);
    volatile float rounded=integral/scale;
    if(!ordinary(rounded))return result(Status::UnsupportedNumeric,0x1a2c30u);
    float total=rounded;
    for(std::uint32_t i=0;i<needed;++i) {
        if(!ordinary(p.lapTimesCC0[i]))return result(Status::UnsupportedNumeric,0x1a2c50u);
        volatile float left=total,right=p.lapTimesCC0[i];
        volatile float sum=left+right;
        if(!ordinary(sum))return result(Status::UnsupportedNumeric,0x1a2c58u);
        total=sum;
    }
    if(!room(c))return result(Status::SequenceExhausted,0x1a2c70u);
    c.roundedLap=rounded;c.total=total;c.selection=e.selection1125F&15u;
    c.lapRecord=false;c.raceRecord=false;
    return issue(c,Call::LapLookup1861A0,Target::CurrentConfig2DC,
        0x1a2c70u,0x1861a0u,Phase::AfterLapLookup,9u,c.selection,c.roundedLap,true);
}
} // namespace

LapCompletionResult50GG beginLapCompletion50GG(LapCompletionCursor50GG& c) noexcept {
    if(c.pending.kind!=Call::None||
       (c.phase!=Phase::Idle&&c.phase!=Phase::Complete))
        return result(Status::AlreadyActive,0x1a2b80u);
    return issue(c,Call::OwnerSelection292360,Target::PlayerD40Owner488,
        0x1a2bb4u,0x292360u,Phase::AfterOwnerSelection,7u,1u);
}

bool acceptLapCompletionReturn50GG(LapCompletionCursor50GG& c,
    const LapCompletionReturn50GG& returned,LapCompletionResult50GG& out) noexcept {
    const auto& q=c.pending;
    if(q.kind==Call::None||returned.sequence!=q.sequence||returned.kind!=q.kind||
       returned.query!=q.query||(!q.query&&returned.originalV0!=0u)) {
        out=result(Status::WrongReturn,q.callPC);return false;
    }
    if(q.query)c.lastQueryReturn=returned.originalV0;
    c.pending={};out=result(Status::ReturnAccepted);return true;
}

LapCompletionResult50GG advanceLapCompletion50GG(LapCompletionPlayer50GG& p,
    const LapCompletionConfig50GG& e,LapCompletionCursor50GG& c) noexcept {
    if(c.pending.kind!=Call::None)return waiting(c);
    for(;;) {
        switch(c.phase) {
        case Phase::Idle:return result(Status::NotStarted,0x1a2b80u);
        case Phase::AfterOwnerSelection:
            if(e.mode11258==2u){c.phase=Phase::Tail;break;}
            return prepareTimes(p,e,c);
        case Phase::AfterLapLookup:
            if(c.lastQueryReturn!=0u) {
                if(!ordinary(p.bestLapCB4))
                    return result(Status::UnsupportedNumeric,0x1a2c80u);
                // C.LE.S best,0; if false, C.LT.S rounded,best.
                c.lapRecord=p.bestLapCB4<=0.0f||c.roundedLap<p.bestLapCB4;
            }
            c.phase=Phase::DecideRaceLookup;break;
        case Phase::DecideRaceLookup:
            if((e.lapCount1125C&31u)==3u&&p.lapCC8==3u)
                return issue(c,Call::RaceLookup186280,Target::CurrentConfig2DC,
                    0x1a2cf0u,0x186280u,Phase::AfterRaceLookup,
                    9u,c.selection,c.total,true);
            c.phase=Phase::LapRecord;break;
        case Phase::AfterRaceLookup:
            c.raceRecord=c.lastQueryReturn!=0u;c.phase=Phase::LapRecord;break;
        case Phase::LapRecord:
            if(c.lapRecord)return issue(c,Call::LapRecord22B170,Target::Player,
                0x1a2d0cu,0x22b170u,Phase::RaceRecord,8u,0u,c.roundedLap);
            c.phase=Phase::RaceRecord;break;
        case Phase::RaceRecord:
            if(c.raceRecord)return issue(c,Call::RaceRecord22B370,Target::Player,
                0x1a2d20u,0x22b370u,Phase::LastLap,8u,0u,c.total);
            c.phase=Phase::LastLap;break;
        case Phase::LastLap:
            if(e.mode11258!=3u&&p.lapCC8+1u==static_cast<std::uint32_t>(e.lapCount1125C&31u))
                return issue(c,Call::LastLap22B760,Target::Player,
                    0x1a2d6cu,0x22b760u,Phase::Tail);
            c.phase=Phase::Tail;break;
        case Phase::Tail:
            if(e.mode11258==3u)return issue(c,Call::PlayerEnd217940,Target::Player,
                0x1a2d98u,0x217940u,Phase::AfterPlayerEnd);
            c.phase=Phase::Complete;return result(Status::Complete,0x1a2dc8u);
        case Phase::AfterPlayerEnd:
            if(!room(c))return result(Status::SequenceExhausted,0x1a2da4u);
            // Delay slot1A2DA8 is visible to217420 before it begins.
            p.word7C=0u;
            return issue(c,Call::PlayerReset217420,Target::Player,
                0x1a2da4u,0x217420u,Phase::AfterPlayerReset);
        case Phase::AfterPlayerReset:
            c.phase=Phase::Complete;return result(Status::Complete,0x1a2dc8u);
        case Phase::Complete:return result(Status::Complete,0x1a2dc8u);
        default:return result(Status::InvalidState,0x1a2b80u);
        }
    }
}
}
