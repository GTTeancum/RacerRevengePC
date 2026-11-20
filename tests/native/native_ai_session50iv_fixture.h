#pragma once
#include <type_traits>
#include "../../src/game/ai_runtime50iv.h"
#include "../../src/game/navigation_view50cr.h"
#include "../../src/game/ai_race_rank50iv.h"
#include "../../src/game/ai_packing_reference50iu.h"
#include "../../src/game/original_shared_rng50iu.h"
#include "../../src/assets/ai_authoring50iu.h"
#include "native_racer_observation50iz_fixture.h"

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
    OriginalAiFinishList finished44{};
    OriginalAiRetirementList retired40{};
    float trackLength38=0.f;
    bool managerBound=false,all8FinishAwaitingDisplay=false,all8ResultsComplete=false;
};
namespace detail {
template<class Map,class Metadata>
concept ExactObservationPair50TC=
 (std::is_same_v<Map,TrackNavigation50EU>&&std::is_same_v<Metadata,AiTrackMetadata50IU>)||
 (std::is_same_v<Map,NavigationView50CR>&&std::is_same_v<Metadata,AiTrackMetadataView50IU>);
inline bool mapReady(const TrackNavigation50EU&,const AiTrackMetadata50IU& m)noexcept{return m.bound;}
inline bool mapReady(const NavigationView50CR& map,const AiTrackMetadataView50IU& m)noexcept{
 return map.runtimeValid()&&map.role()==NavigationRole50CR::map&&m.matches(map);
}
inline std::uint32_t mapCount(const TrackNavigation50EU& m)noexcept{return m.track.count;}
inline std::uint32_t mapCount(const NavigationView50CR& m)noexcept{return m.count();}
inline const TrackNavigationNode50EU* mapNodes(const TrackNavigation50EU& m)noexcept{return m.nodes;}
inline const TrackNavigationNode50EU* mapNodes(const NavigationView50CR& m)noexcept{return m.gates();}
inline const AiTrackNodeMetadata50IU* metadataNodes(const AiTrackMetadata50IU& m)noexcept{return m.nodes;}
inline const AiTrackNodeMetadata50IU* metadataNodes(const AiTrackMetadataView50IU& m)noexcept{return m.nodes();}
}
template<class Map,class Metadata> requires detail::ExactObservationPair50TC<Map,Metadata>
inline void observeTyped(Session&session,std::size_t slot,const VehicleFrameState49W&frame,
    const VehicleFrameBindings49W&bindings,const TrackNavigationProgress50EU&progress,
    std::uint32_t lapCC8,std::uint32_t retirementCD4,const Map&map,
    const Metadata&mapMetadata) {
    runtime_startup50fl::require(slot<Session::capacity&&detail::mapReady(map,mapMetadata)&&
        progress.reset.vehicleC88<detail::mapCount(map),"original AI manager observation missing");
    auto&o=session.observed[slot];o={};o.bound=true;o.managerIndex=static_cast<std::uint32_t>(slot);
    o.vehicle58=frame.fields.vehicle58;o.vehicle40=frame.bodies.main.vehicleState40;
    o.vehicleC80=progress.reset.vehicleC80;o.cachedSpeed34=frame.bodies.main.cachedSpeed34;
    o.progressCC4=session.fields[slot].remainingCC4;o.placeCCC=session.fields[slot].placeCCC;
    o.vehicleC84=progress.vehicleC84;o.lapCC8=static_cast<std::int32_t>(lapCC8);
    o.flagsB9C=frame.bodies.main.controls.player.flagsB9C;o.flagsCD8=frame.fields.impactFlagsCD8;
    const auto&scene=frame.nodes[static_cast<unsigned>(bindings.lifecycle.main190)];
    const auto&physical=frame.nodes[static_cast<unsigned>(bindings.animation.bodyScene288)];
    const auto current=progress.reset.vehicleC88;auto&rank=session.rankInput[slot];rank={};
    rank.mapNode60=detail::metadataNodes(mapMetadata)[current].distanceToFinish60;rank.lapCC8=o.lapCC8;
    rank.vehicleState40=o.vehicle40;rank.retirementCD4=retirementCD4;rank.flagsCD8=o.flagsCD8;
    for(unsigned axis=0;axis<3;++axis){o.position190[axis]=scene.localC0.v[12+axis];
        o.forward190[axis]=scene.localC0.v[8+axis];o.up190[axis]=scene.localC0.v[4+axis];
        o.velocity70[axis]=frame.bodies.main.components.root.body.velocity.v[axis];
        o.linearAccelerationC0[axis]=frame.bodies.main.components.root.body.linearAcceleration.v[axis];
        rank.bodyScene288Position[axis]=physical.localC0.v[12+axis];
        rank.mapNode10[axis]=detail::mapNodes(map)[current].gate10[axis];rank.mapNode20[axis]=detail::mapNodes(map)[current].gate20[axis];}
}
inline void observe(Session& session,std::size_t slot,const VehicleFrameState49W& frame,
 const VehicleFrameBindings49W& bindings,const TrackNavigationProgress50EU& progress,
 std::uint32_t lapCC8,std::uint32_t retirementCD4,const TrackNavigation50EU& map,const AiTrackMetadata50IU& metadata){
 observeTyped(session,slot,frame,bindings,progress,lapCC8,retirementCD4,map,metadata);
}
inline void observe(Session& session,std::size_t slot,const VehicleFrameState49W& frame,
 const VehicleFrameBindings49W& bindings,const TrackNavigationProgress50EU& progress,
 std::uint32_t lapCC8,std::uint32_t retirementCD4,const NavigationView50CR& map,const AiTrackMetadataView50IU& metadata){
 observeTyped(session,slot,frame,bindings,progress,lapCC8,retirementCD4,map,metadata);
}
inline bool rank(Session&session,std::size_t count) noexcept {
    if(!session.managerBound||!updateAiRaceRank50IV(session.rankInput,count,session.finished44,session.retired40,
        session.rank,session.rankWork,session.rankDiagnostic))return false;
    for(std::size_t i=0;i<count;++i){session.fields[i]={session.rank[i].remainingCC4,session.rank[i].placeCCC,true};
        session.observed[i].progressCC4=session.fields[i].remainingCC4;
        session.observed[i].placeCCC=session.fields[i].placeCCC;}
    session.rankBound=true;return true;
}
// Scalar-only alternative to the arena observe() above. It reads the exact
// same canonical fields on each call and publishes the same external rows.
// Supplemental handles also serve readiness diagnostics and dynamic actors.
struct ObservationReads {
    template<class T> using Read=native_racer50HN::Bank::ReadHandle<T>;
    Read<bool> initialized;
    Read<std::uint32_t> managerIndexFE0,racer60,fieldFE4,sourceBoundary,frameSourceStage;
    Read<float> repairTime1064;
    Read<std::uint32_t> vehicleC80,vehicleC84,vehicleC88,lapCC8,retirementCD4;
    Read<SceneNodeIndex49P> main190,bodyScene288;
    void bind(const native_racer50HN::Bank& bank,const VehicleFrameBindings49W& bindings,
        const TrackNavigationProgress50EU& progress,const std::uint32_t& laps,
        const std::uint32_t& retirement,const AiRuntimeState50IV& ai,
        const AiRuntimeReceipt50IV& receipt,const VehicleFrameDiagnostic49W& diagnostic) {
        initialized=bank.bindRead(ai.initialized);managerIndexFE0=bank.bindRead(ai.managerIndexFE0);
        racer60=bank.bindRead(ai.racer60);fieldFE4=bank.bindRead(ai.fieldFE4);
        repairTime1064=bank.bindRead(ai.profile.repairTime1064);
        sourceBoundary=bank.bindRead(receipt.sourceBoundary);frameSourceStage=bank.bindRead(diagnostic.sourceStage);
        vehicleC80=bank.bindRead(progress.reset.vehicleC80);vehicleC84=bank.bindRead(progress.vehicleC84);vehicleC88=bank.bindRead(progress.reset.vehicleC88);
        lapCC8=bank.bindRead(laps);retirementCD4=bank.bindRead(retirement);
        main190=bank.bindRead(bindings.lifecycle.main190);bodyScene288=bank.bindRead(bindings.animation.bodyScene288);
    }
    template<class Map,class Metadata> requires detail::ExactObservationPair50TC<Map,Metadata>
    void observeTyped(Session& session,std::size_t slot,const native_racer50HN::Bank& bank,
        const native_racer50IZ::FrameReads& frame,const Map& map,
        const Metadata& mapMetadata) const {
        runtime_startup50fl::require(slot<Session::capacity&&detail::mapReady(map,mapMetadata),
            "original AI manager observation missing");
        const auto current=bank.read(slot,vehicleC88);
        runtime_startup50fl::require(current<detail::mapCount(map),"original AI manager observation missing");
        auto&o=session.observed[slot];o={};o.bound=true;o.managerIndex=static_cast<std::uint32_t>(slot);
        o.vehicle58=bank.read(slot,frame.vehicle58);o.vehicle40=bank.read(slot,frame.vehicle40);
        o.vehicleC80=bank.read(slot,vehicleC80);o.cachedSpeed34=bank.read(slot,frame.cachedSpeed34);
        o.progressCC4=session.fields[slot].remainingCC4;o.placeCCC=session.fields[slot].placeCCC;
        o.vehicleC84=bank.read(slot,vehicleC84);o.lapCC8=static_cast<std::int32_t>(bank.read(slot,lapCC8));
        o.flagsB9C=bank.read(slot,frame.flagsB9C);o.flagsCD8=bank.read(slot,frame.flagsCD8);
        const auto scene=bank.read(slot,main190),physical=bank.read(slot,bodyScene288);
        auto&rank=session.rankInput[slot];rank={};
        rank.mapNode60=detail::metadataNodes(mapMetadata)[current].distanceToFinish60;rank.lapCC8=o.lapCC8;
        rank.vehicleState40=o.vehicle40;rank.retirementCD4=bank.read(slot,retirementCD4);rank.flagsCD8=o.flagsCD8;
        for(unsigned axis=0;axis<3;++axis){o.position190[axis]=frame.position(bank,slot,scene,axis);
            o.forward190[axis]=frame.forward(bank,slot,scene,axis);o.up190[axis]=frame.up(bank,slot,scene,axis);
            o.velocity70[axis]=bank.read(slot,frame.velocity70[axis]);
            o.linearAccelerationC0[axis]=bank.read(slot,frame.linearAccelerationC0[axis]);
            rank.bodyScene288Position[axis]=frame.position(bank,slot,physical,axis);
            rank.mapNode10[axis]=detail::mapNodes(map)[current].gate10[axis];rank.mapNode20[axis]=detail::mapNodes(map)[current].gate20[axis];}
    }
    void observe(Session& session,std::size_t slot,const native_racer50HN::Bank& bank,
     const native_racer50IZ::FrameReads& frame,const TrackNavigation50EU& map,const AiTrackMetadata50IU& metadata)const{
     observeTyped(session,slot,bank,frame,map,metadata);
    }

    void observe(Session& session,std::size_t slot,const native_racer50HN::Bank& bank,
     const native_racer50IZ::FrameReads& frame,const NavigationView50CR& map,const AiTrackMetadataView50IU& metadata)const{
     observeTyped(session,slot,bank,frame,map,metadata);
    }

};
// Fresh APPLICATION stream, retained across native session restarts. Live
// consumers still use the stable frame.noise address; only serial application
// entry/exit transports it. It is never stored in a racer snapshot.
struct ApplicationNoise {
    CoupledNoiseContext49M state{};AiApplicationGlobals50IV ai{};bool initialized=false;
};
inline ApplicationNoise& applicationNoise() {static ApplicationNoise owner;return owner;}
struct NoiseLifetime {
    CoupledNoiseContext49M* live=nullptr;
    bool admitted=false;
    explicit NoiseLifetime(CoupledNoiseContext49M&noise,bool enabled) noexcept {
        if(noise.noise.initializationIncomplete49M)return;
        if(!enabled){admitted=true;return;}
        auto&owner=applicationNoise();
        if(owner.state.noise.initializationIncomplete49M)return;
        if(!owner.initialized){
            if(!initializeOriginalSharedNoise50IU(owner.state))return;
            owner.initialized=true;
        }
        noise=owner.state;live=&noise;admitted=true;
    }
    ~NoiseLifetime(){
        if(!live)return;auto& retained=applicationNoise().state;
        //A newly partial live prefix must survive session teardown. Conversely,
        //never overwrite an already partial Application with an older snapshot.
        if(!retained.noise.initializationIncomplete49M)retained=*live;
    }
};
} //namespace native_ai50IV
