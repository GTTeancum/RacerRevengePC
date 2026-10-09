#pragma once
#include "race_event_queue50id.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
// The1A4D30 category6 table entry3AA288 targets1A4D64 ->1A55F0.
// This is its complete ordinary control flow at explicit source helper cuts.
// No host50HQ round robin, extra Dax voice, or assumed music no-op is used.
enum class LapEventCall50ID : std::uint8_t {
    None,Announcer2C2C20,Music1A46E0,Clock101A50,Seed308020,Draw308030
};
struct LapEventRequest50ID {
    LapEventCall50ID kind=LapEventCall50ID::None;
    std::uint64_t sequence=0;
    std::uint32_t callPC=0;
    // Announcer: a1=type,a2=actual racer60,a3=variant. Music:a1=event,a2=0.
    // Seed:argumentA0=the actual101A50 result. Clock/Draw have no arguments.
    std::uint32_t argumentA0=0,argumentA1=0,argumentA2=0,argumentA3=0;
};
enum class LapEventPhase50ID : std::uint8_t {
    Idle,AfterAnnouncer,LastLap,AfterClock,AfterSeed,AfterDraw,AfterMusic,Complete
};
struct LapEventCursor50ID {
    LapEventPhase50ID phase=LapEventPhase50ID::Idle;
    LapEventRequest50ID pending{};
    std::uint64_t lastSequence=0;
    RaceActorId50ID actor=0;
    std::uint32_t place=0,lap=0,racer60=0,lastReturn=0;
    bool finished=false;
};
// Shared source-static state, OUTSIDE per-racer snapshots, initially BSS-zero.
// The original last-lap route seeds the shared RNG through its own lazy slot;
// previous RNG use elsewhere does NOT prove this slot is initialized.
struct LapEventRandomOwner50ID {bool lazy80A8=false;std::uint32_t seed3D6B30=0;};
enum class LapEventStatus50ID : std::uint8_t {
    Complete,Continue,NeedHelper,AlreadyActive,WrongReturn,ActorUnavailable,
    UnsupportedEvent,UnsupportedNumeric,RandomRange,SequenceExhausted
};
struct LapEventResult50ID {
    LapEventStatus50ID status;
    std::uint32_t sourcePC;
    LapEventRequest50ID request{};
};
namespace lap_event_detail50ID {
inline LapEventResult50ID result(LapEventStatus50ID status,std::uint32_t pc) noexcept {
    return {status,pc,{}};
}
inline LapEventResult50ID pending(const LapEventCursor50ID& c) noexcept {
    return {LapEventStatus50ID::NeedHelper,c.pending.callPC,c.pending};
}
inline LapEventResult50ID issue(LapEventCursor50ID& c,LapEventCall50ID kind,
    std::uint32_t pc,LapEventPhase50ID next,std::uint32_t a1=0,
    std::uint32_t a2=0,std::uint32_t a3=0,std::uint32_t a0=0) noexcept {
    if(c.lastSequence==std::numeric_limits<std::uint64_t>::max())
        return result(LapEventStatus50ID::SequenceExhausted,pc);
    c.pending={kind,++c.lastSequence,pc,a0,a1,a2,a3};c.phase=next;return pending(c);
}
}
inline LapEventResult50ID beginLapEvent6Subscriber50ID(const RaceEvent50ID& event,
    const RaceActorResolver50ID& actors,LapEventCursor50ID& c) noexcept {
    using S=LapEventStatus50ID;using P=LapEventPhase50ID;using K=LapEventCall50ID;
    using namespace lap_event_detail50ID;
    if(c.pending.sequence||(c.phase!=P::Idle&&c.phase!=P::Complete))
        return result(S::AlreadyActive,0x1a55f0u);
    if(event.type4!=6u||event.word0||event.word8||!event.actor0C||event.actor10)
        return result(S::UnsupportedEvent,0x1a55f0u);
    RaceActorSnapshot50ID actor{};
    if(!actors.read||!actors.read(actors.context,event.actor0C,actor))
        return result(S::ActorUnavailable,0x1a560cu);
    c.actor=event.actor0C;c.place=actor.placeCCC;c.lap=actor.lapCC8;
    c.racer60=actor.vehicle60;c.finished=(actor.flagsCD8&1u)!=0;
    c.phase=P::Complete;
    // Source's AI branch returns before all speech/music behavior.
    if(actor.vehicle58!=0u)return result(S::Complete,0x1a5914u);
    if(c.finished){
        std::uint32_t type=6,variant=1,pc=0x1a5650u;
        if(c.racer60!=16u){
            if(c.place==1u){variant=2;pc=0x1a5670u;}
            else if(c.place==2u){type=7;variant=2;pc=0x1a568cu;}
            else if(c.place==3u){type=8;variant=2;pc=0x1a56a8u;}
            else pc=0x1a56c0u;
        }
        return issue(c,K::Announcer2C2C20,pc,P::AfterAnnouncer,type,c.racer60,variant);
    }
    if(c.place!=1u||c.racer60==16u)return result(S::Complete,0x1a5914u);
    if(c.lap>=1u&&c.lap<=3u){
        constexpr std::uint32_t pcs[3]{0x1a5754u,0x1a5770u,0x1a578cu};
        return issue(c,K::Announcer2C2C20,pcs[c.lap-1],P::AfterAnnouncer,
            c.lap+2u,c.racer60,0);
    }
    c.phase=P::LastLap;
    //No helper is claimed here; advance performs the live config check.
    return result(S::Continue,0x1a57b0u);
}
// Only after the exact helper really returned. Clock/Draw use actual v0;
// ignored-return helpers require0 to avoid mistaking a selected clip for v0.
inline bool acceptLapEventHelperReturn50ID(LapEventCursor50ID& c,
    std::uint64_t sequence,LapEventCall50ID kind,std::uint32_t actualV0) noexcept {
    const bool query=kind==LapEventCall50ID::Clock101A50||kind==LapEventCall50ID::Draw308030;
    if(!c.pending.sequence||c.pending.sequence!=sequence||c.pending.kind!=kind||
       (!query&&actualV0))return false;
    c.lastReturn=actualV0;c.pending={};return true;
}
inline LapEventResult50ID advanceLapEvent6Subscriber50ID(LapEventCursor50ID& c,
    std::uint8_t liveLapCount1125C,LapEventRandomOwner50ID& randomOwner) noexcept {
    using S=LapEventStatus50ID;using P=LapEventPhase50ID;using K=LapEventCall50ID;
    using namespace lap_event_detail50ID;
    if(c.pending.sequence)return pending(c);
    for(;;)switch(c.phase){
    case P::AfterAnnouncer:
        if(c.finished){
            const auto event=c.place==1u?27u:((c.place==2u||c.place==3u)?28u:26u);
            const auto pc=c.place==1u?0x1a56f8u:
                ((c.place==2u||c.place==3u)?0x1a570cu:0x1a5720u);
            return issue(c,K::Music1A46E0,pc,P::AfterMusic,event);
        }
        c.phase=P::LastLap;break;
    case P::LastLap:
        if(c.lap!=static_cast<std::uint32_t>(liveLapCount1125C&31u)-1u){
            c.phase=P::Complete;return result(S::Complete,0x1a5914u);
        }
        if(!randomOwner.lazy80A8){
            randomOwner.seed3D6B30=0;randomOwner.lazy80A8=true;
        }
        if(!randomOwner.seed3D6B30)
            return issue(c,K::Clock101A50,0x1a5804u,P::AfterClock);
        return issue(c,K::Draw308030,0x1a5828u,P::AfterDraw);
    case P::AfterClock:
        randomOwner.seed3D6B30=c.lastReturn;
        return issue(c,K::Seed308020,0x1a5818u,P::AfterSeed,0,0,0,
            randomOwner.seed3D6B30);
    case P::AfterSeed:
        return issue(c,K::Draw308030,0x1a5828u,P::AfterDraw);
    case P::AfterDraw:{
        if(std::fegetround()!=FE_TONEAREST||c.lastReturn>0x7fffffffu)
            return result(S::UnsupportedNumeric,0x1a5830u);
        volatile float draw=static_cast<float>(c.lastReturn);
        const std::uint32_t scaleWord=0x30000000u;float scale;
        std::memcpy(&scale,&scaleWord,4);
        volatile float fraction=scale*draw;
        volatile float selected=3.0f*fraction;
        if(!std::isfinite(selected)||selected<0.0f||selected>=3.0f)
            return result(S::RandomRange,0x1a5850u);
        const auto index=static_cast<std::uint32_t>(selected)+1u;
        const auto music=index==1u?34u:(index==2u?29u:33u);
        return issue(c,K::Music1A46E0,0x1a590cu,P::AfterMusic,music);
    }
    case P::AfterMusic:c.phase=P::Complete;return result(S::Complete,0x1a5914u);
    case P::Complete:return result(S::Complete,0x1a5914u);
    default:return result(S::UnsupportedEvent,0x1a55f0u);
    }
}
// The surrounding FIFO ticket is acknowledged only after Complete. Effects
// already returned stay committed if later source dependencies are unavailable.
// Seed/draw use the ONE actual shared RNG. Host group RNG is not a substitute.
}
