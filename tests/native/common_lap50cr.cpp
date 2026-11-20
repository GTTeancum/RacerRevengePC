// Common rewrite (R4, PLAN 2.5): the ONE lap tracker for every course
// (common_lap50cr.h). The single lap algorithm of src\game\lap_progress50gf.cpp
// (2A1980/2A273C/2A26E0 markers, FF8, CC8++ at 1A0F18, 1A0F24 -> 1A2B80,
// 1A1108 scan) and the one wrong-way rule (1A2E30) over the one common map view.
// Per-racer storage is the race field (1AD520 limit 8); every slot is
// bounds-checked against the prepared field and refused past it.
#include "common_lap50cr.h"
#include "race_session50hm_fixture.h"

namespace common_lap50CR {
namespace {
namespace log=common_race_log50CR;
bool refuse(Diagnostic& d,std::uint32_t pc,std::size_t slot,const char* reason)noexcept {
    d.pc=pc;d.refused=true;d.slot=slot;d.reason=reason;
    try {log::load(log::Step::navigation2A1260,false,slot,reason,nullptr,pc);}catch(...){}
    return false;
}
}

bool Tracker::prepare(const common_track50CR::NavigationView& map,const common_track50CR::NavigationView& ai,
    std::size_t field,Diagnostic& d)noexcept {
    d={};
    if(prepared_)return refuse(d,0x2a1980,0,"lap tracker already prepared (release first)");
    if(!field||field>racers_.size())return refuse(d,0x1ad520,field,"race field outside 1..8 (1AD520)");
    if(!map.runtimeValid()||map.role()!=common_track50CR::NavigationRole::map||
       !ai.runtimeValid()||ai.role()!=common_track50CR::NavigationRole::ai||!map.sameOwner(ai))
        return refuse(d,0x2a1980,0,"lap tracker needs the runtime-valid map/AI views of one navigation store");
    rr::game::LapProgressTrack50GF markers{};
    if(!rr::game::prepareLapProgressTrack50GF(map,markers,d.progress))
        return refuse(d,d.progress.sourcePC?d.progress.sourcePC:0x2a1980u,d.progress.node,"2A1980 ProgressNode markers refused");
    map_=map;ai_=ai;markers_=markers;field_=field;
    for(auto& r:racers_)r=Racer{};
    for(std::size_t i=0;i<field_;++i)racers_[i].bound=true;
    prepared_=true;
    return true;
}

bool Tracker::resetRacer(std::size_t slot,Diagnostic& d)noexcept {
    d={};
    if(!prepared_||slot>=field_)return refuse(d,0x21669c,slot,"lap reset slot outside the race field");
    auto& r=racers_[slot];
    // 21669C..216704 race fields (CC8, CD8 bits0..2) and the 1A0360/1A258C
    // cursor (FF8); the pending 1A2B80 callback and the 1A2E30 state go with them.
    rr::game::initializePlayerLapRaceFields50GF(r.state);
    rr::game::initializePlayerLapCursor50GF(r.state);
    r.pending={};r.wrongWay={};r.wrongWayPending={};
    return true;
}

bool Tracker::applyCrossing(std::size_t slot,const rr::game::LapProgressCrossing50GF& crossing,
    const rr::game::LapTimingFields50ID& timing,rr::game::LapProgressReceipt50GF& receipt,Diagnostic& d)noexcept {
    d={};
    if(!prepared_||slot>=field_)return refuse(d,0x1a0e60,slot,"lap crossing slot outside the race field");
    auto& r=racers_[slot];
    if(!rr::game::applyPlayerLapCrossing50GF(map_,markers_,crossing,r.state,r.pending,receipt,d.progress)){
        d.pc=d.progress.sourcePC;d.refused=true;d.slot=slot;d.reason="1A0E60/1A10D0 lap crossing refused";
        return false;
    }
    if(receipt.lapCounterIncremented){
        // common_lap: CC8 after 1A0F18; time/best are the racer's 216690 fields
        // as the 1A2B80 callback receives them (CB8 current lap clock, CB4 best).
        try {log::lap(slot,r.state.vehicleCC8,timing.timeCB8,timing.bestLapCB4,r.state.vehicleFF8);}catch(...){}
    }
    return true;
}

bool Tracker::resumeAfterCallback(std::size_t slot,Diagnostic& d)noexcept {
    d={};
    if(!prepared_||slot>=field_)return refuse(d,0x1a0f2c,slot,"lap resume slot outside the race field");
    auto& r=racers_[slot];
    if(!rr::game::resumePlayerLapAfterCallback50GF(r.state,r.pending,d.progress)){
        d.pc=d.progress.sourcePC;d.refused=true;d.slot=slot;d.reason="1A0F2C resume without a pending 1A2B80";
        return false;
    }
    return true;
}

bool Tracker::beginWrongWay(std::size_t slot,const rr::game::WrongWayProgressInput50GO& input,
    rr::game::WrongWayProgressReceipt50GO& receipt,rr::game::WrongWayProgressDiagnostic50GO& d)noexcept {
    if(!prepared_||slot>=field_){d={rr::game::WrongWayProgressError50GO::Track,0x1a2e30};return false;}
    auto& r=racers_[slot];
    // The one rule: FF8 is the lap cursor shared with 1A0F18 (same vehicle field).
    r.wrongWay.vehicleFF8=r.state.vehicleFF8;
    if(!rr::game::beginWrongWayProgress50GO(map_,markers_,input,r.wrongWay,r.wrongWayPending,receipt,d))return false;
    r.state.vehicleFF8=r.wrongWay.vehicleFF8;
    return true;
}

bool Tracker::resumeWrongWay(std::size_t slot,rr::game::WrongWayProgressDiagnostic50GO& d)noexcept {
    if(!prepared_||slot>=field_){d={rr::game::WrongWayProgressError50GO::Track,0x1a3274};return false;}
    return rr::game::resumeWrongWayProgress50GO(racers_[slot].wrongWayPending,d);
}

void Tracker::release()noexcept {
    map_={};ai_={};markers_={};field_=0;prepared_=false;
    for(auto& r:racers_)r=Racer{};
}

} // namespace common_lap50CR

namespace race_session50HM {
// The one course race session (R4): node rows sized from the one map view's own
// count at cold (no fixed 293-node storage), markers from the one lap tracker.
bool initialize(CourseSession& out,const common_track50CR::NavigationView& map,
    const common_lap50CR::Tracker& laps,std::uint32_t racers,const Config& config)noexcept {
    if(!map.runtimeValid()||map.role()!=common_track50CR::NavigationRole::map||!map.count()||
       !laps.prepared()||!laps.map().sameOwner(map)||laps.markers().nodeCount!=map.count())
        return detail::fail(out,Error::Configuration);
    // Non-owning read facade lives only during this synchronous copy. The final
    // view retained below owns the same immutable graph and actual live world.
    struct MapFacade {
        struct Graph {const rr::game::UndertrackNode50ET* nodes;std::uint32_t count;} track;
        const rr::game::TrackNavigationNode50EU* nodes;
        bool gatesPrepared;
    } facade{{map.nodes(),map.count()},map.gates(),map.gatesPrepared()};
    try {
        CourseSession candidate;
        const std::size_t n=map.count();
        candidate.positions.assign(n,{});candidate.distanceToFinish.assign(n,0.0);
        candidate.nextCount.assign(n,0u);candidate.next.assign(n,{});candidate.nodeCapacity=n;
        if(!detail::fillStorage(candidate,facade,laps.markers(),racers,config,n))
            return detail::fail(out,candidate.error);
        candidate.navigation_=map;out=std::move(candidate);return true;
    }catch(...){return detail::fail(out,Error::Configuration);}
}
} // namespace race_session50HM
