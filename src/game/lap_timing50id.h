#pragma once
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>

namespace rr::game {
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"50ID timing requires binary32");
// Source-only, uncompiled numeric cuts from 1AFF50. This is NOT the complete
// race manager: event delivery, rank, finish sealing and flag clearing are
// separate boundaries. The bounded50ID HOST adapter supplies an explicitly
// labeled accepted-world scheduler/observer; it claims no full original root
// order. State is per racer; manager input is shared and live.
inline constexpr std::uint32_t lapTimingCapacity50ID=100;
struct LapTimingFields50ID {
    float timeCB0,bestLapCB4,timeCB8,valueCBC;
    std::uint32_t countCD0,wordCD4;
    float lapTimesCC0[lapTimingCapacity50ID];
};
struct LapTimingManager50ID {
    float time30;
    std::uint8_t flags58;
};
enum class LapManagerClockStatus50ID : std::uint8_t {
    GlobalBlocked,OtherManagerPath,NeedRacerPass,UnsupportedNumeric
};
// Numerical clock prefix1AF9C0 only, before1AFF50. The input is the actual
// float factor supplied to that native update; no host accumulated clock or
// finish interpolation is read. The surrounding original update placement and
// later manager calls remain explicit caller-owned boundaries.
inline LapManagerClockStatus50ID beginLapManagerClock50ID(
    LapTimingManager50ID& manager,float frameDelta,bool global11CPresent) noexcept {
    using S=LapManagerClockStatus50ID;
    if(global11CPresent)return S::GlobalBlocked;
    const auto flags=static_cast<std::uint8_t>(manager.flags58&~0x18u);
    if(!(flags&2u)||(flags&1u)){
        manager.flags58=flags;return S::OtherManagerPath;
    }
    const auto ordinary=[](float x) noexcept {
        return std::isfinite(x)&&(x==0.0f||std::isnormal(x));
    };
    if(std::fegetround()!=FE_TONEAREST||!ordinary(frameDelta)||!ordinary(manager.time30))
        return S::UnsupportedNumeric;
    volatile float previous=manager.time30,delta=frameDelta;
    volatile float sum=previous+delta;
    if(!ordinary(sum))return S::UnsupportedNumeric;
    manager.flags58=flags;manager.time30=sum;return S::NeedRacerPass;
}
struct LapTimingInput50ID {
    float frameSeconds;
    std::uint32_t mode11258,vehicle58,vehicle5C,lapCC8;
    std::uint8_t flagsCD8;
};
enum class LapTimingStatus50ID : std::uint8_t {
    Complete, NeedLapEvent, AlreadyPending, WrongReturn,
    Storage, UnsupportedNumeric, SequenceExhausted
};
struct LapTimingRequest50ID {
    std::uint64_t sequence=0;
    // Construct original event (type6, this racer, 0,0,0) at1B0180 using
    // 1A39B0, then deliver it to actual application28C+10C at1B01B8 via
    // 1A64B0. This request does not fabricate either native owner.
    std::uint32_t constructorCallPC=0,dispatchCallPC=0,eventType=0;
};
struct LapTimingCursor50ID {
    std::uint64_t lastSequence=0;
    const LapTimingFields50ID* owner=nullptr;
    LapTimingRequest50ID pending{};
};
struct LapTimingResult50ID {
    LapTimingStatus50ID status;
    std::uint32_t sourcePC;
    LapTimingRequest50ID request{};
};
namespace lap_timing_detail50ID {
inline bool ordinary(float x) noexcept {
    return std::isfinite(x)&&(x==0.0f||std::isnormal(x));
}
inline LapTimingResult50ID result(LapTimingStatus50ID status,
    std::uint32_t pc) noexcept {return {status,pc,{}};}
inline std::int32_t truncateSaturating(float value) noexcept {
    if(value>=2147483648.0f)return std::numeric_limits<std::int32_t>::max();
    if(value<=-2147483648.0f)return std::numeric_limits<std::int32_t>::min();
    return static_cast<std::int32_t>(value);
}
}
// Timing portion ONLY of fresh-race initializer216690. The full initializer
// additionally resets CC8/CCC/CD8 and invokes damage reset217420 and other
// vehicle effects. Do not use this on checkpoint/undertrack recovery.
inline void initializeLapTimingFields50ID(LapTimingFields50ID& fields,
    std::uint32_t mode11258) noexcept {
    fields.timeCB0=0.0f;fields.timeCB8=0.0f;fields.valueCBC=0.0f;
    fields.countCD0=0;fields.wordCD4=0;
    if(mode11258!=3u)fields.bestLapCB4=0.0f;
    for(auto& lap:fields.lapTimesCC0)lap=0.0f;
}
// Invoke at the caller's declared manager pass, only after the corresponding
// synchronous50GG callback has returned, with manager+30 already advanced by
// 1AF9C0. Placement of that manager pass within the original root update order
// still belongs to the integrating owner. The bounded HOST adapter declares
// its own accepted-world order instead of claiming original scheduler parity.
// This does not advance the manager clock itself. Do not
// replace manager.time30 with host50HM's double raceSeconds/finishSeconds.
// Unsupported inputs leave state/cursor/manager unchanged. Earlier committed
// native work outside this function is not rolled back.
inline LapTimingResult50ID beginRacerLapTiming50ID(LapTimingFields50ID& fields,
    LapTimingManager50ID& manager,const LapTimingInput50ID& input,
    LapTimingCursor50ID& cursor) noexcept {
    using S=LapTimingStatus50ID;
    using namespace lap_timing_detail50ID;
    if(cursor.pending.sequence)return result(S::AlreadyPending,0x1b01b8u);
    // Finished skip is before any timing operand is read.
    if((input.flagsCD8&1u)&&(manager.flags58&0x20u))
        return result(S::Complete,0x1b01f4u);
    if(std::fegetround()!=FE_TONEAREST||!ordinary(input.frameSeconds)||
       !ordinary(fields.timeCB0))return result(S::UnsupportedNumeric,0x1b002cu);
    const bool lap=(input.flagsCD8&2u)!=0;
    const bool event=lap&&input.mode11258==1u;
    if(!event&&!ordinary(manager.time30))
        return result(S::UnsupportedNumeric,0x1b01e0u);
    float completedLap=0.0f,newBest=0.0f,newTime=0.0f;
    if(lap){
        if(input.lapCC8==0u||input.lapCC8>lapTimingCapacity50ID)
            return result(S::Storage,0x1b0040u);
        if(!ordinary(fields.bestLapCB4))return result(S::UnsupportedNumeric,0x1b00a8u);
        if(event&&cursor.lastSequence==std::numeric_limits<std::uint64_t>::max())
            return result(S::SequenceExhausted,0x1b0180u);
        volatile float time=fields.timeCB0,scale=100.0f;
        volatile float scaled=time*scale;
        if(!ordinary(scaled))return result(S::UnsupportedNumeric,0x1b0064u);
        const auto centiseconds=truncateSaturating(scaled);
        volatile float converted=static_cast<float>(centiseconds);
        volatile float quantized=converted/scale;
        if(!ordinary(quantized))return result(S::UnsupportedNumeric,0x1b0094u);
        completedLap=quantized;
        newBest=fields.bestLapCB4==0.0f?completedLap:
            detail::scalarMax48O(0.0f,
                detail::scalarMin48O(fields.bestLapCB4,completedLap));
        newTime=input.frameSeconds; // Source1B0140 is not a zero reset.
    }else{
        volatile float previous=fields.timeCB0,delta=input.frameSeconds;
        volatile float sum=previous+delta;
        if(!ordinary(sum))return result(S::UnsupportedNumeric,0x1b01ccu);
        newTime=sum;
    }
    if(lap){
        if(input.vehicle58==0u){
            const auto human=static_cast<std::uint8_t>(input.vehicle5C+1u);
            const auto accumulated=((manager.flags58>>3u)&3u)+human;
            manager.flags58=static_cast<std::uint8_t>((manager.flags58&~0x18u)|
                ((accumulated&3u)<<3u));
        }
        // Source writes raw time, scaled time, then quantized time to the same
        // slot without an intervening observer/helper. Publish final value.
        fields.lapTimesCC0[input.lapCC8-1u]=completedLap;
        fields.bestLapCB4=newBest;
    }
    fields.timeCB0=newTime;
    if(event){
        cursor.owner=&fields;
        cursor.pending={++cursor.lastSequence,0x1b0180u,0x1b01b8u,6u};
        return {S::NeedLapEvent,0x1b0180u,cursor.pending};
    }
    fields.timeCB8=manager.time30; //1B01EC, before rank/finish processing.
    return result(S::Complete,0x1b01f0u);
}
// Call only after the exact requested1A39B0/1A64B0 effects have really returned.
// Refresh the concrete manager after those effects:1B01E0 rereads live+30.
// This acknowledges neither rank1B0400, finish1B02F0, nor CD8 bit1 clearing.
inline LapTimingResult50ID resumeRacerLapTimingAfterEvent50ID(
    LapTimingFields50ID& fields,const LapTimingManager50ID& liveManager,
    LapTimingCursor50ID& cursor,std::uint64_t returnedSequence) noexcept {
    using S=LapTimingStatus50ID;
    using namespace lap_timing_detail50ID;
    if(!cursor.pending.sequence||cursor.pending.sequence!=returnedSequence||
       cursor.owner!=&fields)return result(S::WrongReturn,0x1b01c0u);
    if(!ordinary(liveManager.time30))return result(S::UnsupportedNumeric,0x1b01e0u);
    fields.timeCB8=liveManager.time30;cursor.pending={};cursor.owner=nullptr;
    return result(S::Complete,0x1b01f0u);
}
// The remaining1AFF50 order is: all racer numeric/event cuts,1B0400 ranking,
// all eligible pending-lap1B02F0 finish checks, CD8 bit1 clear,1B0920 tail.
// Never clear the bit here to hide an unrepresented downstream dependency.
// Binary32 ordinary finite operands, nearest-even arithmetic, no fast math or
// contraction, typed/disjoint backing and serial ownership are required.
}
