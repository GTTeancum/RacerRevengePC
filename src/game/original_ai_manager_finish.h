#pragma once
#include "lap_timing50id.h"
#include "scalar_minmax48o.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Native stable manager-row identities replace original C90 pointers. This is
// the actual finish list44/count54 owner, not a host standings approximation.
struct OriginalAiFinishList {
    std::array<std::uint32_t,8> managerRows{};
    std::uint32_t count54=0;
};
// Source40 is append-ordered retirement storage;50 is the number of racers
// not yet appended there (it starts at4C, not zero).
struct OriginalAiRetirementList {
    std::array<std::uint32_t,8> managerRows{};
    std::uint32_t activeCount50=0;
    bool bound=false;
};
enum class OriginalAiFinishStatus : std::uint8_t {
    Complete, Storage, Numeric, CallerBoundary
};
// Source1AF7F0 constructor,1A7750/58 ->1AF960 create, and the scalar/list
// portion of1AFC70 reset. The caller must have completed each racer's genuine
// cold/reset owner first. Only actual mode11258==1 enables finish sealing.
inline OriginalAiFinishStatus initializeOriginalAiManager(
    LapTimingManager50ID& manager,OriginalAiFinishList& finished,
    OriginalAiRetirementList& retired,std::size_t count,std::uint32_t mode11258,
    float node1Distance60,const float* node0Position,const float* node1Position,
    float& trackLength38) noexcept {
    using S=OriginalAiFinishStatus;
    if(!count||count>finished.managerRows.size()||!node0Position||!node1Position)
        return S::Storage;
    if(std::fegetround()!=FE_TONEAREST||!lap_timing_detail50ID::ordinary(node1Distance60))return S::Numeric;
    float delta[3];
    for(unsigned axis=0;axis<3;++axis){
        if(!lap_timing_detail50ID::ordinary(node0Position[axis])||
           !lap_timing_detail50ID::ordinary(node1Position[axis]))return S::Numeric;
        volatile float difference=node1Position[axis]-node0Position[axis];delta[axis]=difference;
    }
    volatile float xx=delta[0]*delta[0],yy=delta[1]*delta[1],zz=delta[2]*delta[2];
    volatile float xy=xx+yy,square=xy+zz;
    volatile float distance=static_cast<float>(std::sqrt(static_cast<double>(square)));
    volatile float length=node1Distance60+distance; //1AFC44/48, actual MAP B8
    if(!lap_timing_detail50ID::ordinary(length)||length<=0.f)return S::Numeric;
    manager.time30=0.f;
    manager.flags58=static_cast<std::uint8_t>(0x40u|(mode11258==1u?0x20u:0u));
    finished={};retired={};retired.activeCount50=static_cast<std::uint32_t>(count);retired.bound=true;
    trackLength38=length;return S::Complete;
}
// Source1B02F0. Caller retains1AFF50 order: timing for all, ranking for all,
// pending-lap finish calls, then clear bit1 and1B0920. An already finished
// racer must be skipped by that caller when manager bit5 is set.
inline OriginalAiFinishStatus finishOriginalAiManagerRow(
    std::uint8_t actualManagerFlags58,std::uint16_t actualPacked1125C,
    std::uint32_t actualManagerRow,std::uint32_t actualLapCC8,
    std::uint8_t& actualFlagsCD8,LapTimingFields50ID& timing,
    OriginalAiFinishList& finished) noexcept {
    using S=OriginalAiFinishStatus;
    if(!(actualManagerFlags58&0x20u))return S::Complete; //1B02FC
    const auto laps=static_cast<std::uint32_t>(actualPacked1125C&0x1fu);
    if(actualLapCC8!=laps)return S::Complete; //1B0338
    if(!(actualFlagsCD8&2u)||(actualFlagsCD8&1u))return S::CallerBoundary;
    if(actualManagerRow>=finished.managerRows.size()||finished.count54>=finished.managerRows.size()||
       laps>lapTimingCapacity50ID)return S::Storage;
    if(std::fegetround()!=FE_TONEAREST)return S::Numeric;
    // Explicit native finite admission before the first source store. The
    // live stores below preserve the original flag/list/count/time order.
    float sum=0.f;
    for(std::uint32_t i=0;i<laps;++i){
        if(!lap_timing_detail50ID::ordinary(timing.lapTimesCC0[i]))return S::Numeric;
        volatile float next=sum+timing.lapTimesCC0[i];sum=next;
        if(!lap_timing_detail50ID::ordinary(sum))return S::Numeric;
    }
    actualFlagsCD8=static_cast<std::uint8_t>(actualFlagsCD8|1u); //1B035C
    finished.managerRows[finished.count54]=actualManagerRow; //1B037C
    ++finished.count54; //1B0388
    timing.timeCB8=0.f; //1B039C
    for(std::uint32_t i=0;i<laps;++i){
        volatile float next=timing.timeCB8+timing.lapTimesCC0[i];
        timing.timeCB8=next; //1B03C8
    }
    return S::Complete;
}
struct OriginalAiGapRow {
    std::int32_t placeCCC=0,lapCC8=0;
    std::uint8_t flagsCD8=0;
    float remainingCC4=0.f,timeCB8=0.f,previousCBC=0.f;
};
// Source1B0920 numerical tail. Input is a current snapshot of actual manager
// rows after finish processing. Caller publishes returned CBC values to the
// same live rows; no host distance, interpolation or wall clock is consumed.
inline OriginalAiFinishStatus computeOriginalAiManagerGaps(
    float actualTrackLength38,const OriginalAiGapRow* rows,std::size_t count,
    float* outputCBC) noexcept {
    using S=OriginalAiFinishStatus;
    if(!rows||!outputCBC||!count||count>8)return S::Storage;
    if(std::fegetround()!=FE_TONEAREST||!lap_timing_detail50ID::ordinary(actualTrackLength38))return S::Numeric;
    std::size_t leader=count;
    for(std::size_t i=0;i<count;++i)if(rows[i].placeCCC==1){leader=i;break;}
    const bool foundLeader=leader!=count;
    if(!foundLeader)leader=0; //1B0928 row-zero fallback; its old CBC is retained.
    const auto& first=rows[leader];
    if(!lap_timing_detail50ID::ordinary(first.remainingCC4)||
       !lap_timing_detail50ID::ordinary(first.timeCB8)||first.timeCB8==0.f)return S::Numeric;
    volatile float lap=static_cast<float>(first.lapCC8);
    volatile float completed=actualTrackLength38*lap;
    volatile float remaining=actualTrackLength38-first.remainingCC4;
    volatile float distance=completed+remaining;
    volatile float rawSpeed=distance/first.timeCB8;
    if(!lap_timing_detail50ID::ordinary(distance)||!lap_timing_detail50ID::ordinary(rawSpeed))return S::Numeric;
    const float speed=detail::scalarMax48O(1.f,rawSpeed); //MAX.S1B09B0
    std::array<float,8> values{};
    if(!foundLeader){
        if(!lap_timing_detail50ID::ordinary(first.previousCBC))return S::Numeric;
        values[leader]=first.previousCBC;
    }
    for(std::size_t i=0;i<count;++i){
        if(i==leader)continue;
        if(!lap_timing_detail50ID::ordinary(rows[i].remainingCC4))return S::Numeric;
        volatile float racerLap=static_cast<float>(rows[i].lapCC8);
        volatile float racerCompleted=actualTrackLength38*racerLap;
        volatile float part=(rows[i].flagsCD8&4u)?rows[i].remainingCC4:
            actualTrackLength38-rows[i].remainingCC4;
        volatile float racerDistance=racerCompleted+part;
        volatile float difference=distance-racerDistance;
        volatile float gap=difference/speed;
        if(!lap_timing_detail50ID::ordinary(racerDistance)||!lap_timing_detail50ID::ordinary(gap))return S::Numeric;
        values[i]=gap;
    }
    for(std::size_t i=0;i<count;++i)outputCBC[i]=values[i];
    return S::Complete;
}
} // namespace rr::game
