#pragma once
#include "original_tunnel_track50la.h"

namespace rr::game {
// Native ownership of the complete represented raceC0 tuning, retaining the
// SAME authenticated source lease used by actual B8/BC. Source1AAB30 allocates
// uncounted data, so this owner must not construct another Entity or AI state.
class OriginalRaceTuning50LA final {
    friend struct ::original_main_entry50KG::Application;
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation_;
    const std::uint64_t sessionGeneration_;
    OriginalRaceTuning50LA(std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigation,
        std::uint64_t generation)noexcept
        :navigation_(std::move(navigation)),sessionGeneration_(generation){}
public:
    ~OriginalRaceTuning50LA()noexcept {release1A68D8();}
    OriginalRaceTuning50LA(const OriginalRaceTuning50LA&)=delete;
    OriginalRaceTuning50LA& operator=(const OriginalRaceTuning50LA&)=delete;
    // Borrowed views remain valid only while this owner is retained and unreleased.
    const StaticImpactTuning49Y* environment()const noexcept {
        return navigation_?&navigation_->aiImpact():nullptr;
    }
    const DynamicImpactTuning50IW* dynamic()const noexcept {
        return navigation_?&navigation_->dynamicImpact():nullptr;
    }
    const assets::FinalImpactAuthoring49Y* finalImpact()const noexcept {
        return navigation_?&navigation_->finalImpact():nullptr;
    }
    const AiTrackTuning50IU* ai()const noexcept {
        return navigation_?&navigation_->aiTuning():nullptr;
    }
    // Exact source C0 deallocation leaf follows1AB4F0 at1A68D8. The native
    // immutable arrays share source ownership; retire this actual borrow only.
    // Calling this leaf alone does not certify preceding manager reset or race cleanup.
    void release1A68D8()noexcept {navigation_.reset();}
};
}
namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalRaceTuning50LA> Application::retainRaceTuningCold1A7184()const noexcept {
    using T=rr::game::OriginalRaceTuning50LA;
    if(!raceTuningColdOwner50LA_.expired()||!entitiesOwner_)return {};
    const auto map=tunnelTrackColdOwners50LA_[0].lock();
    const auto ai=tunnelTrackColdOwners50LA_[1].lock();
    if(!map||!ai||map->application_!=entitiesOwner_||ai->application_!=entitiesOwner_||
       !map->detached()||!ai->detached()||!map->projection_||!ai->projection_||
       !map->navigation_||map->navigation_!=ai->navigation_||
       map->entity_.sessionGeneration!=ai->entity_.sessionGeneration)return {};
    const auto& source=*map->navigation_;
    if(map->projection_!=&source.map()||ai->projection_!=&source.ai()||
       !source.aiTuning().bound||!source.aiImpact().aiEnvironmentMultiplierBound||
       !source.dynamicImpact().bound)return {};
    try {
        auto owner=std::shared_ptr<T>(new T(map->navigation_,map->entity_.sessionGeneration));
        raceTuningColdOwner50LA_=owner;return owner;
    }catch(...){return {};}
}
}
