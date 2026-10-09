#pragma once
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/ai_race_rank50iv.h"
#include "../../src/game/ai_packing_reference50iu.h"
#include "../../src/game/original_shared_rng50iu.h"
#include "../../src/assets/ai_authoring50iu.h"

namespace native_ai50IV {
using namespace rr::game;
// Explicit native accepted-world manager seam, not the original root scheduler.
// All values come from actual racer owners. No HOST standings input is exposed.
struct Session {
    static constexpr std::size_t capacity=aiRaceRankCapacity50IV;
    LapTimingManager50ID manager{0.f,0u};
    AiRaceRankState50IV rank[capacity]{};
    AiRaceFields50IV fields[capacity]{};
    AiObservedRacer50IV observed[capacity]{};
    AiRaceRankInput50IV rankInput[capacity]{};
    AiRaceRankWorkspace50IV rankWork{};
    AiRaceRankDiagnostic50IV rankDiagnostic{};
    std::int32_t packingReferences[capacity]{};
    bool rankBound=false;
    std::uint32_t originalFinishedCount54=0;
};
inline void observe(Session&session,std::size_t slot,const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,const TrackNavigationProgress50EU&progress,
    std::uint32_t lapCC8,std::uint32_t retirementCD4,const TrackNavigation50EU&map,
    const AiTrackMetadata50IU&mapMetadata) {
    runtime_startup50fl::require(slot<Session::capacity&&mapMetadata.bound&&
        progress.reset.vehicleC88<map.track.count,"original AI manager observation missing");
    auto&o=session.observed[slot];o={};o.bound=true;o.managerIndex=static_cast<std::uint32_t>(slot);
    o.vehicle58=frame.fields.vehicle58;o.vehicle40=frame.bodies.main.vehicleState40;
    o.vehicleC80=progress.reset.vehicleC80;o.cachedSpeed34=frame.bodies.main.cachedSpeed34;
    o.progressCC4=session.rank[slot].remainingCC4;o.lapCC8=static_cast<std::int32_t>(lapCC8);
    o.flagsB9C=frame.bodies.main.controls.player.flagsB9C;o.flagsCD8=frame.fields.impactFlagsCD8;
    const auto&scene=frame.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
    const auto&physical=frame.nodes[static_cast<unsigned>(bindings.animation.bodyScene288)];
    const auto current=progress.reset.vehicleC88;auto&rank=session.rankInput[slot];rank={};
    rank.mapNode60=mapMetadata.nodes[current].distanceToFinish60;rank.lapCC8=o.lapCC8;
    rank.vehicleState40=o.vehicle40;rank.retirementCD4=retirementCD4;rank.flagsCD8=o.flagsCD8;
    for(unsigned axis=0;axis<3;++axis){o.position190[axis]=scene.localC0.v[12+axis];
        rank.bodyScene288Position[axis]=physical.localC0.v[12+axis];
        rank.mapNode10[axis]=map.nodes[current].gate10[axis];rank.mapNode20[axis]=map.nodes[current].gate20[axis];}
}
inline bool rank(Session&session,std::size_t count) noexcept {
    if(!updateAiRaceRank50IV(session.rankInput,count,session.originalFinishedCount54,
        session.rank,session.rankWork,session.rankDiagnostic))return false;
    for(std::size_t i=0;i<count;++i){session.fields[i]={session.rank[i].remainingCC4,session.rank[i].placeCCC,true};
        session.observed[i].progressCC4=session.rank[i].remainingCC4;}
    session.rankBound=true;return true;
}
// Fresh APPLICATION stream, retained across native session restarts. Live
// consumers still use the stable frame.noise address; only serial application
// entry/exit transports it. It is never stored in a racer snapshot.
struct ApplicationNoise {
    CoupledNoiseContext49M state{};AiApplicationGlobals50IV ai{};bool initialized=false;
};
inline ApplicationNoise& applicationNoise() {static ApplicationNoise owner;return owner;}
struct NoiseLifetime {
    CoupledNoiseContext49M* live=nullptr;
    explicit NoiseLifetime(CoupledNoiseContext49M&noise,bool enabled) noexcept {
        if(!enabled)return;auto&owner=applicationNoise();
        if(!owner.initialized){initializeOriginalSharedNoise50IU(owner.state);owner.initialized=true;}
        noise=owner.state;live=&noise;
    }
    ~NoiseLifetime(){if(live)applicationNoise().state=*live;}
};
} //namespace native_ai50IV
