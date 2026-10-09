#pragma once
#include "pod_speech50hg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>

namespace rr::game {
// Native identities resolve the actual racer through its existing bank. Never
// store the multiplexed execution-arena pointer as an opponent's identity.
using RaceActorId50ID=std::uint32_t; //0 is source null; actual actors are nonzero.
struct RaceActorSnapshot50ID {
    std::uint32_t vehicle3C,vehicle58,vehicle60,lapCC8,placeCCC;
    std::uint8_t flagsCD8;
};
using RaceActorResolve50ID=bool (*)(void*,RaceActorId50ID,RaceActorSnapshot50ID&) noexcept;
struct RaceActorResolver50ID {void* context=nullptr;RaceActorResolve50ID read=nullptr;};
// Exact1A39B0 word order, with native identity handles in the two actor lanes.
struct RaceEvent50ID {
    std::uint32_t word0=0,type4=0,word8=0;
    RaceActorId50ID actor0C=0,actor10=0;
};
struct RaceEventEntry50ID {RaceEvent50ID event{};std::uint64_t ticket=0;};
enum class RaceEventStatus50ID : std::uint8_t {
    Appended,ActorSuppressed,Empty,Ready,FlushSuppressed,Complete,
    Storage,ActorUnavailable,AlreadyFlushing,WrongReturn,SequenceExhausted
};
struct RaceEventAppend50ID {
    RaceEventStatus50ID status;
    std::uint64_t ticket=0;
    // Means actual queue helper returned, not subscriber/sound completion.
    bool helperReturned() const noexcept {
        return status==RaceEventStatus50ID::Appended||
            status==RaceEventStatus50ID::ActorSuppressed;
    }
};
// Fixed reservation is a bounded native implementation of source realloc.
// Logical capacity begins32 (1A3C90/94) and grows by32 at the exact source
// count+1>=capacity condition. Beyond reservation is explicit Storage.
// This shared queue stays OUTSIDE the per-racer snapshot bank.
template<std::size_t Reserved=128> struct RaceEventQueue50ID {
    static_assert(Reserved>=32&&Reserved<=0x7fffffffu,"bounded source queue reservation");
    std::array<RaceEventEntry50ID,Reserved> entries{};
    std::uint32_t count150=0,capacity154=32,dispatchIndex=0;
    std::uint64_t lastTicket=0;
    bool flushing=false;
    std::uint8_t dispatchGate58=0,dispatchGate1B0=0;
};
template<std::size_t N> inline RaceEventAppend50ID appendRaceEvent50ID(
    RaceEventQueue50ID<N>& queue,const RaceEvent50ID& event,
    const RaceActorResolver50ID& actors) noexcept {
    using S=RaceEventStatus50ID;
    if(queue.count150>=queue.capacity154||queue.capacity154>N)return {S::Storage,0};
    auto capacity=queue.capacity154;
    if(queue.count150+1u>=capacity){
        if(capacity>N-32u)return {S::Storage,0};
        capacity+=32u;
    }
    //1A64DC..FC expands before actor+3C checks. Unsupported resolution does
    // not acknowledge the helper; supported ActorSuppressed retains growth.
    std::uint32_t actorFlags=0;
    for(const auto actor:{event.actor0C,event.actor10})if(actor){
        RaceActorSnapshot50ID snapshot{};
        if(!actors.read||!actors.read(actors.context,actor,snapshot))
            return {S::ActorUnavailable,0};
        actorFlags|=snapshot.vehicle3C;
    }
    if(actorFlags){queue.capacity154=capacity;return {S::ActorSuppressed,0};}
    if(queue.lastTicket==std::numeric_limits<std::uint64_t>::max())
        return {S::SequenceExhausted,0};
    const auto ticket=++queue.lastTicket;
    queue.capacity154=capacity;
    queue.entries[queue.count150++]={event,ticket};
    return {S::Appended,ticket};
}
template<std::size_t N> inline RaceEventStatus50ID beginRaceEventFlush50ID(
    RaceEventQueue50ID<N>& queue,std::uint8_t raceManager58,
    std::uint8_t application1B0) noexcept {
    using S=RaceEventStatus50ID;
    if(queue.flushing)return S::AlreadyFlushing;
    if(queue.count150>=queue.capacity154||queue.capacity154>N)return S::Storage;
    //1A4C9C..1A4CC0;1A5C40 then clears count even when dispatch was gated.
    if(!(raceManager58&2u)||(application1B0&16u)){
        queue.count150=0;queue.dispatchIndex=0;return S::FlushSuppressed;
    }
    queue.dispatchIndex=0;queue.flushing=true;
    queue.dispatchGate58=raceManager58;queue.dispatchGate1B0=application1B0;
    return queue.count150?S::Ready:S::Empty;
}
template<std::size_t N> inline const RaceEventEntry50ID* frontRaceEvent50ID(
    const RaceEventQueue50ID<N>& queue) noexcept {
    return queue.flushing&&queue.dispatchIndex<queue.count150?
        &queue.entries[queue.dispatchIndex]:nullptr;
}
// Acknowledge only after this exact event's complete native subscriber chain
// returned. An unavailable Dax/music owner is not a completed event.
template<std::size_t N> inline RaceEventStatus50ID acceptRaceEventReturn50ID(
    RaceEventQueue50ID<N>& queue,std::uint64_t ticket) noexcept {
    const auto* entry=frontRaceEvent50ID(queue);
    if(!entry||entry->ticket!=ticket)return RaceEventStatus50ID::WrongReturn;
    ++queue.dispatchIndex;return RaceEventStatus50ID::Complete;
}
template<std::size_t N> inline RaceEventStatus50ID finishRaceEventFlush50ID(
    RaceEventQueue50ID<N>& queue) noexcept {
    if(!queue.flushing||queue.dispatchIndex!=queue.count150)
        return RaceEventStatus50ID::WrongReturn;
    queue.count150=0;queue.dispatchIndex=0;queue.flushing=false;
    return RaceEventStatus50ID::Complete;
}
// Replace the existing selected-player single-slot producer with this bridge,
// never call both. Event5 joins the SAME FIFO as manager-produced event6, in
// actual producer invocation order. Existing50HG still owns damage thresholds,
// repeat state and later speech selection; no RNG is consumed here.
template<std::size_t N> inline DamageSpeechReceipt50HG produceQueuedDamageSpeech50ID(
    const DamageSpeechInput50HG& input,UndertrackPlayerPrefix50ET& prefix,
    RaceActorId50ID actor,const RaceActorResolver50ID& actors,
    RaceEventQueue50ID<N>& queue) noexcept {
    DamageSpeechReceipt50HG bad{};
    if(!actor){bad.status=PodSpeechStatus50HG::Input;return bad;}
    auto nextPrefix=prefix;DamageSpeechEvent50HG local{};
    auto receipt=produceDamageSpeechEvent50HG(input,nextPrefix,local);
    if(receipt.status!=PodSpeechStatus50HG::Complete||!receipt.eventProduced)return receipt;
    const auto appended=appendRaceEvent50ID(queue,{0,5,local.repeated?1u:0u,actor,0},actors);
    if(!appended.helperReturned()){
        receipt.status=appended.status==RaceEventStatus50ID::Storage?
            PodSpeechStatus50HG::Capacity:PodSpeechStatus50HG::Input;
        return receipt;
    }
    receipt.eventAccepted=appended.status==RaceEventStatus50ID::Appended;
    prefix=nextPrefix;return receipt;
}
// For event5 at front, construct local {pending=true,repeated=word8!=0}, call
// dispatchDamageSpeechEvent50HG with that actor's LIVE inputs and actual RNG,
// and complete its returned external announcer request before acknowledging
// the FIFO ticket. Retain the subscriber continuation if that provider pauses;
// never rerun already-committed RNG or voice selection on resume.
}
