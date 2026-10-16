#pragma once
#include "ra_cold_sources50la.h"
#include <memory>
#include <utility>

namespace ra_cold_aggregate50LA { class Owner; }
namespace ra_cold_sources50LA {
// Immutable, same-source views only. These concrete records own inline arrays;
// no World48F/LiveZoneWorld, mutable source registry or parser scratch escapes.
// Keep this lease for as long as any returned reference/AI binding is borrowed.
class ColdNavigationLease final {
    friend class ra_cold_aggregate50LA::Owner;
    std::shared_ptr<const Owner> source_;
    const DecodedSources& data_;
    explicit ColdNavigationLease(std::shared_ptr<const Owner> source)
        :source_(std::move(source)),data_(*source_->sourcesDecoded()){}
public:
    ColdNavigationLease(const ColdNavigationLease&)=delete;
    ColdNavigationLease& operator=(const ColdNavigationLease&)=delete;
    const g::TrackNavigation50EU& map()const noexcept{return data_.map;}
    const g::TrackNavigation50EU& ai()const noexcept{return data_.ai;}
    const g::LapProgressTrack50GF& lapTrack()const noexcept{return data_.mapLap;}
    const g::AiTrackMetadata50IU& mapMetadata()const noexcept{return data_.mapMetadata;}
    const g::AiTrackMetadata50IU& aiMetadata()const noexcept{return data_.aiMetadata;}
    const g::AiFreshCreateSelection50IU& freshSelection()const noexcept{return data_.freshSelection;}
    const g::AiTrackTuning50IU& aiTuning()const noexcept{return data_.aiTuning;}
    const g::StaticImpactTuning49Y& aiImpact()const noexcept{return data_.aiImpact;}
    const g::DynamicImpactTuning50IW& dynamicImpact()const noexcept{return data_.dynamicImpact;}
    const a::FinalImpactAuthoring49Y& finalImpact()const noexcept{return data_.finalImpact;}
    // Verifies actual already-initialized progress; never initializes a racer.
    bool bindColdTracks(const g::TrackNavigationProgress50EU& progress,
        g::AiColdTrackBindings50IU& out)const noexcept {
        return g::bindAiColdTracks50IU(data_.map,data_.ai,data_.mapMetadata,data_.aiMetadata,progress,out);
    }
};
} // namespace ra_cold_sources50LA
