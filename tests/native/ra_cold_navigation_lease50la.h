#pragma once
#include "ra_cold_sources50la.h"
#include <memory>
#include <utility>

namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::game { class OriginalTunnelTrack50LA; }
namespace ra_cold_sources50LA {
// Immutable, same-source views only. These concrete records own inline arrays;
// no World48F/LiveZoneWorld, mutable source registry or parser scratch escapes.
// Keep this lease for as long as any returned reference/AI binding is borrowed.
class ColdNavigationLease final {
    friend class ra_cold_aggregate50LA::Owner;
    friend class rr::game::OriginalTunnelTrack50LA;
    std::shared_ptr<const Owner> source_;
    const DecodedSources& data_;
    explicit ColdNavigationLease(std::shared_ptr<const Owner> source)
        :source_(std::move(source)),data_(*source_->sourcesDecoded()){}
    // Exact constructor40=0 + optional2A13D8 NumShortcuts lookup.
    // Array44 contains shortcut pairs, NOT per-node zone indices. Admit its
    // genuine absence from these SAME retained authored bytes, never a flag.
    bool noShortcutAllocation2A13D8(bool map)const noexcept {
        namespace detail=rr::assets::ai_authoring_detail50IU;
        const auto& bytes=data_.memberBytes[map?1u:2u];
        rr::assets::AiAuthoringDiagnostic50IU diagnostic;
        bool trackSeen=false;
        auto visit=[&](detail::Slice section,detail::Slice key,detail::Slice value)noexcept {
            if(!detail::equal(section,"Track"))return true;
            trackSeen=true;
            if(!key.n||!detail::equal(key,"NumShortcuts"))return true;
            std::int32_t count=0;
            return detail::integer(value,count)&&count==0;
        };
        return detail::scan(reinterpret_cast<const char*>(bytes.data()),bytes.size(),
            g::maxUndertrackText50ET,0x2a13d8,visit,diagnostic)&&trackSeen;
    }
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
