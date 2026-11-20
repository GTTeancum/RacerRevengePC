#pragma once
#include "original_tunnel_track50la.h"
#include "../../src/assets/impact_authoring49y.h"

namespace rr::game {
// Native ownership of the complete represented raceC0 tuning (1A7184/1AAB30),
// decoded from the course's own .tun (common_track50CR::Track::documents.tuning,
// the same Track the race's map B8 / AI BC TunnelTracks keep alive). One rule for
// every course. Source1AAB30 allocates uncounted data, so this owner must not
// construct another Entity or AI state.
class OriginalRaceTuning50LA final {
    friend struct ::original_main_entry50KG::Application;
    std::shared_ptr<const ::common_track50CR::Track> track_;
    const std::uint64_t sessionGeneration_;
    StaticImpactTuning49Y environment_{};
    DynamicImpactTuning50IW dynamic_{};
    assets::FinalImpactAuthoring49Y finalImpact_{};
    AiTrackTuning50IU ai_{};
    bool decoded_=false;
    OriginalRaceTuning50LA(std::shared_ptr<const ::common_track50CR::Track> track,
        std::uint64_t generation)noexcept
        :track_(std::move(track)),sessionGeneration_(generation){}
    // The four 1AAB30 readers over the same retained .tun bytes; atomic: any
    // rejected reader leaves the owner undecoded (no defaults are supplied).
    bool decode()noexcept {
        if(decoded_||!track_)return false;
        const auto& tuning=track_->documents.tuning;
        if(tuning.empty())return false;
        const auto* text=reinterpret_cast<const char*>(tuning.data());
        assets::PodPhysicsDiagnostic49N impact;assets::AiAuthoringDiagnostic50IU ai;
        StaticImpactTuning49Y environment{};DynamicImpactTuning50IW dynamic{};
        assets::FinalImpactAuthoring49Y finalImpact{};AiTrackTuning50IU aiTuning{};
        if(!assets::readAIEnvironmentImpact49Y(text,tuning.size(),environment,impact)||
           !assets::readAiTrackTuning50IU(text,tuning.size(),aiTuning,ai)||
           !assets::readDynamicImpactAuthoring50IW(text,tuning.size(),dynamic,impact)||
           !assets::readFinalImpactAuthoring49Y(text,tuning.size(),finalImpact,impact))return false;
        if(!aiTuning.bound||!environment.aiEnvironmentMultiplierBound||!dynamic.bound)return false;
        environment_=environment;dynamic_=dynamic;finalImpact_=finalImpact;ai_=aiTuning;
        decoded_=true;return true;
    }
public:
    ~OriginalRaceTuning50LA()noexcept {release1A68D8();}
    OriginalRaceTuning50LA(const OriginalRaceTuning50LA&)=delete;
    OriginalRaceTuning50LA& operator=(const OriginalRaceTuning50LA&)=delete;
    // Borrowed views remain valid only while this owner is retained and unreleased.
    const StaticImpactTuning49Y* environment()const noexcept {return decoded_?&environment_:nullptr;}
    const DynamicImpactTuning50IW* dynamic()const noexcept {return decoded_?&dynamic_:nullptr;}
    const assets::FinalImpactAuthoring49Y* finalImpact()const noexcept {return decoded_?&finalImpact_:nullptr;}
    const AiTrackTuning50IU* ai()const noexcept {return decoded_?&ai_:nullptr;}
    // Exact source C0 deallocation leaf follows1AB4F0 at1A68D8. Retire this
    // actual borrow only. Calling this leaf alone does not certify preceding
    // manager reset or race cleanup.
    void release1A68D8()noexcept {decoded_=false;track_.reset();}
};
}
namespace original_main_entry50KG {
inline std::shared_ptr<rr::game::OriginalRaceTuning50LA> Application::retainRaceTuningCold1A7184()const noexcept {
    using T=rr::game::OriginalRaceTuning50LA;
    if(!raceTuningColdOwner50LA_.expired()||!entitiesOwner_)return {};
    const auto map=tunnelTrackColdOwners50LA_[0].lock();
    const auto ai=tunnelTrackColdOwners50LA_[1].lock();
    // Map B8 and AI BC over the same loaded common Track (every course).
    if(!map||!ai||map->application_!=entitiesOwner_||ai->application_!=entitiesOwner_||
       !map->detached()||!ai->detached()||!map->navigation()||!ai->navigation()||
       !map->track()||map->track()!=ai->track()||!map->track()->loaded()||
       map->entity_.sessionGeneration!=ai->entity_.sessionGeneration)return {};
    try {
        auto owner=std::shared_ptr<T>(new T(map->track(),map->entity_.sessionGeneration));
        if(!owner->decode())return {};
        raceTuningColdOwner50LA_=owner;return owner;
    }catch(...){return {};}
}
}
