#pragma once
#include "ai_cold_owners50iu.h"
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct AiPackingReferenceDiagnostic50IU {
    std::uint32_t sourcePC=0;
    std::size_t racer=0;
};
struct AiPackingHumanDistances50IU {
    // Needed only on171BF0's two-human retarget branch. Absence is explicit.
    const float* human0CC4=nullptr;
    const float* human1CC4=nullptr;
};
// Source171B70. requestedPlayer5C is either the actual player ID passed by
//1A0D3C or literal-1 from1A0D50. humanCount34 is genuine managerB0+34, not
// the size of the all-racer list. No live RNG draws, no synthetic reference.
inline bool assignAiPackingReference50IU(std::uint8_t flagsCD8,
    std::int32_t requestedPlayer5C,std::int32_t humanCount34,
    AiPackingHumanDistances50IU humans,std::int32_t& packingReferenceFF8,
    AiPackingReferenceDiagnostic50IU& d) noexcept {
    d={};
    if(flagsCD8&1u){packingReferenceFF8=-1;return true;}
    if(humanCount34<1||humanCount34>2){d.sourcePC=0x171bb0;return false;}
    if(requestedPlayer5C< -1||requestedPlayer5C>=humanCount34){d.sourcePC=0x171bbc;return false;}
    if(humanCount34==2&&((requestedPlayer5C==0&&packingReferenceFF8==1)||
       (requestedPlayer5C==1&&packingReferenceFF8==0))){
        if(!humans.human0CC4||!humans.human1CC4){d.sourcePC=0x171bf0;return false;}
        if(!std::isfinite(*humans.human0CC4)||!std::isfinite(*humans.human1CC4)){
            d.sourcePC=0x171c14;return false;}
        packingReferenceFF8=*humans.human0CC4<*humans.human1CC4?0:1;
        return true;
    }
    packingReferenceFF8=requestedPlayer5C;return true;
}
struct AiPackingRacerObservation50IU {
    std::uint32_t vehicle58=0;
    std::uint8_t flagsCD8=0;
    std::int32_t rankCCC=0;
    std::int32_t* packingReferenceFF8=nullptr;
};
// Source player1A0CA0, called1A15EC only after1B0B80 returned nonzero and
// player navigation1A0DA0 returned. The caller supplies actual source-ranked
// CCC observations in source manager338 order. HOST scoreboard ranks are not
// accepted as their provenance. The full source manager is required even when
// only one native58=1 racer is admitted. Non-AI58=0 entries are still observed
// and count toward manager.count30, but have no AI packing field to write.
// Native failures preserve already-written earlier racers, matching source
// chronology; never restart this call blindly after a partial failure.
inline bool updateAiPackingReferences50IU(std::int32_t player5C,
    std::int32_t playerRankCCC,std::int32_t humanCount34,
    const AiTrackTuning50IU& tuning,AiPackingHumanDistances50IU humans,
    const AiPackingRacerObservation50IU* racers,std::size_t count,
    AiPackingReferenceDiagnostic50IU& d) noexcept {
    d={};
    if(!tuning.bound||!racers||!count||count>8||player5C<0||
       player5C>=humanCount34||playerRankCCC<0||playerRankCCC>static_cast<std::int32_t>(count)){
        d.sourcePC=0x1a0ca0;return false;}
    //1A0CD8/1A0CDC use LBU; preserve truncation from source loader's SB.
    auto lower=playerRankCCC-static_cast<std::uint8_t>(tuning.numPackingFront8C);
    auto upper=playerRankCCC+static_cast<std::uint8_t>(tuning.numPackingRear8D);
    if(lower<=0)lower=1;
    if(upper>static_cast<std::int32_t>(count))upper=static_cast<std::int32_t>(count);
    for(std::size_t i=0;i<count;++i){const auto& racer=racers[i];
        if(!racer.vehicle58)continue;
        if(racer.vehicle58!=1||!racer.packingReferenceFF8){d={0x1a0d14,i};return false;}
        const auto requested=racer.rankCCC<lower||racer.rankCCC>upper?-1:player5C;
        if(!assignAiPackingReference50IU(racer.flagsCD8,requested,humanCount34,
             humans,*racer.packingReferenceFF8,d)){d.racer=i;return false;}
    }
    return true;
}
} // namespace rr::game
