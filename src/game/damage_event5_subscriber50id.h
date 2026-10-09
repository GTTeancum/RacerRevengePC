#pragma once
#include "race_event_queue50id.h"

namespace rr::game {
// Bridges the existing genuine50HG selector into the shared event FIFO.
// Retains committed RNG/selection across external audio/announcer pauses.
enum class DamageEventEffect50ID : std::uint8_t {None,AudioCommand,Announcer2C2C20};
enum class DamageEventStatus50ID : std::uint8_t {
    Complete,NeedEffect,AlreadyActive,WrongEvent,SourceRejected,WrongReturn
};
struct DamageEventCursor50ID {
    std::uint64_t lastTicket=0;
    bool active=false,complete=false;
    DamageEventEffect50ID pending=DamageEventEffect50ID::None;
    DamageSpeechReceipt50HG receipt{};
    AudioCommand50HE command{};
};
template<std::size_t N> inline DamageEventStatus50ID beginDamageEvent5Subscriber50ID(
    const RaceEventQueue50ID<N>& queue,const DamageSpeechInput50HG& liveActorInput,
    const PodSpeechCounts50HG& counts,std::uint32_t& actualSharedRawRandom,
    PodSpeechState50HG& actualActorSpeech,DamageEventCursor50ID& c) noexcept {
    if(c.active&&!c.complete)return DamageEventStatus50ID::AlreadyActive;
    const auto* entry=frontRaceEvent50ID(queue);
    if(!entry||entry->ticket<=c.lastTicket||entry->event.type4!=5u||
       entry->event.word0||!entry->event.actor0C||entry->event.actor10||
       entry->event.word8>1u)return DamageEventStatus50ID::WrongEvent;
    // Caller resolves this exact actor0C through its actual bank and supplies
    // its live damage/player/speech state. The source flush gate is captured
    // once at1A4C9C; later callbacks do not cause it to be reread per event.
    auto input=liveActorInput;
    input.config58=queue.dispatchGate58;input.manager1B0=queue.dispatchGate1B0;
    DamageSpeechEvent50HG event{true,entry->event.word8!=0u};
    AudioCommand50HE command{};std::size_t emitted=0;
    const auto receipt=dispatchDamageSpeechEvent50HG(input,counts,event,
        actualSharedRawRandom,actualActorSpeech,&command,1,emitted);
    if(receipt.status!=PodSpeechStatus50HG::Complete)return DamageEventStatus50ID::SourceRejected;
    c.lastTicket=entry->ticket;c.active=true;c.complete=false;
    c.receipt=receipt;c.command=command;
    if(emitted)c.pending=DamageEventEffect50ID::AudioCommand;
    else if(receipt.announcerRequested)c.pending=DamageEventEffect50ID::Announcer2C2C20;
    else {c.pending=DamageEventEffect50ID::None;c.complete=true;return DamageEventStatus50ID::Complete;}
    return DamageEventStatus50ID::NeedEffect;
}
// Actual low-damage selection and external fallback are mutually exclusive.
// One real return completes this subscriber, but does not acknowledge another
// queue ticket. Voice/Dax processing must use the one existing mixer/announcer.
inline DamageEventStatus50ID acceptDamageEventEffectReturn50ID(
    DamageEventCursor50ID& c,std::uint64_t ticket,DamageEventEffect50ID kind) noexcept {
    if(!c.active||c.complete||ticket!=c.lastTicket||kind==DamageEventEffect50ID::None||
       kind!=c.pending)return DamageEventStatus50ID::WrongReturn;
    c.pending=DamageEventEffect50ID::None;c.complete=true;
    return DamageEventStatus50ID::Complete;
}
}
