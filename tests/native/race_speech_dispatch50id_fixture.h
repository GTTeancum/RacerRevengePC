#pragma once
#include "damage_event5_subscriber50id.h"
#include "lap_event6_subscriber50id.h"
#include "lap_config_ta50id.h"

namespace race_speech50ID {
using namespace rr::game;
using Queue=RaceEventQueue50ID<128>;
enum class Status:std::uint8_t {Complete,Blocked,InvalidState};
struct Receipt {
    Status status=Status::InvalidState;
    std::uint64_t ticket=0;
    std::uint32_t sourcePC=0;
    LapEventCall50ID lapHelper=LapEventCall50ID::None;
    DamageEventEffect50ID damageHelper=DamageEventEffect50ID::None;
};
struct Providers {
    void* context=nullptr;
    // Resolve the FIFO's exact actor, enter its bank slot if needed, then
    // call beginDamageEvent5Subscriber50ID with the existing50HG inputs,
    // speech state, counts and actual shared RNG. Do not retain arena pointers
    // after restoring that bank slot. Only selected-player event5 is currently
    // produced by50DU; this interface does not fabricate an AI subscriber.
    DamageEventStatus50ID (*beginDamage)(void*,const Queue&,DamageEventCursor50ID&) noexcept=nullptr;
    // Apply existing liveAudio source command OR the real original type12
    // announcer request. true only after that actual helper returned.
    bool (*damageEffect)(void*,const DamageEventCursor50ID&) noexcept=nullptr;
    // Announcer/music/seed/clock/draw provider for exact category6 requests.
    // All voiced categories must share the existing Dax owner; host50HQ
    // notification policy is not this original provider. Query v0 is actual.
    bool (*lapEffect)(void*,const LapEventRequest50ID&,std::uint32_t&) noexcept=nullptr;
};
struct Owner {
    DamageEventCursor50ID damage{};
    LapEventCursor50ID lap{};
    std::uint64_t activeTicket=0;
    std::uint32_t activeType=0;

    // Invoke only at the selected explicit event-manager flush boundary after
    // beginRaceEventFlush50ID. Flush source reads live racers after the earlier
    // manager rank/finish pass; never substitute queue-time snapshots.
    // Neither this owner nor its queue belongs in native_racer50HN::Bank.
    Receipt drain(Queue& queue,const RaceActorResolver50ID& actors,
        const LapConfigTa50ID& config,LapEventRandomOwner50ID& applicationRandom,
        const Providers& providers) noexcept {
        if(!queue.flushing||!config.initialized)return {};
        for(;;){
            const auto* entry=frontRaceEvent50ID(queue);
            if(!entry){
                if(activeTicket)return {};
                if(finishRaceEventFlush50ID(queue)!=RaceEventStatus50ID::Complete)return {};
                return {Status::Complete,0,0,LapEventCall50ID::None,DamageEventEffect50ID::None};
            }
            if(activeTicket&&activeTicket!=entry->ticket)return {};
            if(!activeTicket){
                if(entry->event.type4==5u){
                    if(!providers.beginDamage)return {Status::Blocked,entry->ticket,0x1a52b0u};
                    const auto started=providers.beginDamage(providers.context,queue,damage);
                    if(started!=DamageEventStatus50ID::Complete&&started!=DamageEventStatus50ID::NeedEffect)
                        return {Status::Blocked,entry->ticket,0x1a52b0u};
                }else if(entry->event.type4==6u){
                    const auto started=beginLapEvent6Subscriber50ID(entry->event,actors,lap);
                    if(started.status!=LapEventStatus50ID::Complete&&
                       started.status!=LapEventStatus50ID::Continue&&started.status!=LapEventStatus50ID::NeedHelper)
                        return {Status::Blocked,entry->ticket,started.sourcePC,started.request.kind};
                }else return {Status::Blocked,entry->ticket,0x1a4d30u};
                activeTicket=entry->ticket;activeType=entry->event.type4;
            }
            if(activeType==5u){
                if(!damage.complete){
                    if(!providers.damageEffect||!providers.damageEffect(providers.context,damage))
                        return {Status::Blocked,activeTicket,0x1a52b0u,LapEventCall50ID::None,damage.pending};
                    if(acceptDamageEventEffectReturn50ID(damage,activeTicket,damage.pending)!=DamageEventStatus50ID::Complete)
                        return {};
                }
            }else{
                auto step=advanceLapEvent6Subscriber50ID(lap,
                    static_cast<std::uint8_t>(config.packed1125C),applicationRandom);
                if(step.status==LapEventStatus50ID::NeedHelper){
                    std::uint32_t actualV0=0;
                    if(!providers.lapEffect||!providers.lapEffect(providers.context,step.request,actualV0))
                        return {Status::Blocked,activeTicket,step.sourcePC,step.request.kind};
                    if(!acceptLapEventHelperReturn50ID(lap,step.request.sequence,step.request.kind,actualV0))return {};
                    continue;
                }
                if(step.status!=LapEventStatus50ID::Complete)
                    return {Status::Blocked,activeTicket,step.sourcePC,step.request.kind};
            }
            if(acceptRaceEventReturn50ID(queue,activeTicket)!=RaceEventStatus50ID::Complete)return {};
            activeTicket=0;activeType=0;
        }
    }
};
}
