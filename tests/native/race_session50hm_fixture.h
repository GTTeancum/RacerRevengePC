#pragma once
#include "lap_progress50gf.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace race_session50HM {
// HOST race orchestration, not a recovered original race-manager projection.
// Navigation/lap state remains caller-owned: only completed source 50GF/50GG
// updates may be observed here. This component never moves a racer, advances a
// checkpoint, or increments a lap. Every racer must have an independent native
// vehicle, navigation and lap owner. Fixed storage; no allocation in a frame.
inline constexpr std::size_t maxRacers=16;
enum class Phase : std::uint8_t { Uninitialized, Countdown, Racing, Complete };
enum class Error : std::uint8_t {
    None, Configuration, Route, Observation, MissingSample, LapRegression,
    LapJump, PendingLapCallback, CountdownMovement
};
struct Config {
    std::uint32_t lapTarget=3;
    double countdownSeconds=3.0;
    // Source world units, not a claim about real-world metres. Quantized ties
    // retain prior place; no non-transitive epsilon comparator is used.
    double placeQuantum=.1;
};
struct Observation {
    std::uint32_t completedLaps=0,checkpointCursor=0;
    std::uint32_t previousMapNode=0,currentMapNode=1;
    float position[3]{};
    bool wrongWay=false;
    // A source increment before its synchronous 1A2B80 return is not a
    // completed lap. Callers using lap_drive50GJ::Owner normally supply false.
    bool lapCallbackPending=false;
};
struct Racer {
    Observation observed{},pending{};
    bool observedValid=false,sampled=false,finished=false;
    std::uint32_t place=0,finishPlace=0;
    double remainingDistance=0,finishSeconds=0;
    std::int64_t distanceBucket=0;
};
struct Session {
    Config config{};
    Phase phase=Phase::Uninitialized;
    Error error=Error::None;
    std::uint32_t racerCount=0,nodeCount=0,checkpointCount=0,finishCount=0;
    std::uint32_t ordered[maxRacers]{};
    Racer racers[maxRacers]{};
    double countdownRemaining=0,raceSeconds=0;
    float positions[rr::game::maxUndertrackNodes50ET][3]{};
    double distanceToFinish[rr::game::maxUndertrackNodes50ET]{};
    std::uint32_t nextCount[rr::game::maxUndertrackNodes50ET]{};
    std::uint32_t next[rr::game::maxUndertrackNodes50ET][rr::game::maxUndertrackNext50ET]{};
    double finishForward[2]{};
};
namespace detail {
inline bool fail(Session& s,Error e) noexcept {s.error=e;return false;}
inline double edgeLength(const Session& s,std::uint32_t a,std::uint32_t b) noexcept {
    const double x=double(s.positions[b][0])-s.positions[a][0];
    const double z=double(s.positions[b][2])-s.positions[a][2];
    return std::sqrt(x*x+z*z);
}
inline bool linked(const Session& s,std::uint32_t a,std::uint32_t b) noexcept {
    for(std::uint32_t i=0;i<s.nextCount[a];++i)if(s.next[a][i]==b)return true;
    return false;
}
inline bool validObservation(const Session& s,const Observation& o) noexcept {
    if(o.previousMapNode>=s.nodeCount||o.currentMapNode>=s.nodeCount||
       o.checkpointCursor>s.checkpointCount||
       !linked(s,o.previousMapNode,o.currentMapNode))return false;
    for(float v:o.position)if(!std::isfinite(v))return false;
    return true;
}
inline double remaining(const Session& s,const Observation& o) noexcept {
    const auto a=o.previousMapNode,b=o.currentMapNode;
    const double x=double(s.positions[b][0])-s.positions[a][0];
    const double z=double(s.positions[b][2])-s.positions[a][2];
    const double lengthSquared=x*x+z*z;
    const double along=((double(o.position[0])-s.positions[a][0])*x+
                        (double(o.position[2])-s.positions[a][2])*z)/lengthSquared;
    // Clamp to the source-accepted edge. A far-away or teleported pose cannot
    // claim an unaccepted gate simply by being closer to a later spline node.
    return s.distanceToFinish[b]+(1.0-std::clamp(along,0.0,1.0))*std::sqrt(lengthSquared);
}
inline double finishFraction(const Session& s,const Racer& r) noexcept {
    if(!r.observedValid)return 1.0;
    const auto distance=[&](const Observation& o){
        return (double(o.position[0])-s.positions[0][0])*s.finishForward[0]+
               (double(o.position[2])-s.positions[0][2])*s.finishForward[1];
    };
    const double before=distance(r.observed),after=distance(r.pending);
    // This refines finish order within one real fixed step only AFTER an
    // authoritative completed lap increment. It never detects/creates laps.
    if(before<=0.0&&after>=0.0&&after>before)
        return std::clamp(-before/(after-before),0.0,1.0);
    return 1.0;
}
inline bool ahead(const Session& s,std::uint32_t a,std::uint32_t b) noexcept {
    const auto& left=s.racers[a];const auto& right=s.racers[b];
    if(left.finished!=right.finished)return left.finished;
    if(left.finished)return left.finishPlace<right.finishPlace;
    if(left.observed.completedLaps!=right.observed.completedLaps)
        return left.observed.completedLaps>right.observed.completedLaps;
    if(left.observed.checkpointCursor!=right.observed.checkpointCursor)
        return left.observed.checkpointCursor>right.observed.checkpointCursor;
    if(left.distanceBucket!=right.distanceBucket)return left.distanceBucket<right.distanceBucket;
    return left.place<right.place;
}
inline void rank(Session& s) noexcept {
    // Insertion sort is bounded to sixteen racers. Prior place is a strict
    // tie key and the initial grid order is deterministic.
    for(std::uint32_t i=1;i<s.racerCount;++i){
        const auto racer=s.ordered[i];std::uint32_t j=i;
        while(j&&ahead(s,racer,s.ordered[j-1])){s.ordered[j]=s.ordered[j-1];--j;}
        s.ordered[j]=racer;
    }
    for(std::uint32_t i=0;i<s.racerCount;++i)s.racers[s.ordered[i]].place=i+1;
}
} // namespace detail

inline bool initialize(Session& out,const rr::game::TrackNavigation50EU& map,
    const rr::game::LapProgressTrack50GF& laps,std::uint32_t racerCount,
    const Config& config={}) noexcept {
    if(!racerCount||racerCount>maxRacers||!config.lapTarget||config.lapTarget>999||
       !std::isfinite(config.countdownSeconds)||config.countdownSeconds<0.0||config.countdownSeconds>3600.0||
       !std::isfinite(config.placeQuantum)||config.placeQuantum<.0001||
       !map.gatesPrepared||!map.track.count||map.track.count>rr::game::maxUndertrackNodes50ET||
       !laps.prepared||laps.nodeCount!=map.track.count||
       laps.progressCount48>rr::game::maxLapProgressNodes50GF||
       laps.predecessorOfZero>=map.track.count)
        return detail::fail(out,Error::Configuration);
    Session s{};s.config=config;s.racerCount=racerCount;s.nodeCount=map.track.count;
    s.checkpointCount=laps.progressCount48;s.countdownRemaining=config.countdownSeconds;
    s.phase=config.countdownSeconds>0?Phase::Countdown:Phase::Racing;
    for(std::uint32_t i=0;i<s.nodeCount;++i){
        const auto& node=map.track.nodes[i];
        if(!node.nextCount||node.nextCount>rr::game::maxUndertrackNext50ET)
            return detail::fail(out,Error::Route);
        s.nextCount[i]=node.nextCount;
        for(unsigned axis=0;axis<3;++axis){
            if(!std::isfinite(node.position[axis]))return detail::fail(out,Error::Route);
            s.positions[i][axis]=node.position[axis];
        }
        for(std::uint32_t j=0;j<node.nextCount;++j){
            if(node.next[j]>=s.nodeCount)return detail::fail(out,Error::Route);
            s.next[i][j]=node.next[j];
        }
        s.distanceToFinish[i]=std::numeric_limits<double>::infinity();
    }
    // Reverse shortest authored route distance to gate zero. Alternatives use
    // their real edge lengths; no fabricated node-index/nearest-node progress.
    s.distanceToFinish[0]=0.0;
    for(std::uint32_t pass=0;pass<s.nodeCount;++pass){
        bool changed=false;
        for(std::uint32_t i=0;i<s.nodeCount;++i)for(std::uint32_t j=0;j<s.nextCount[i];++j){
            const auto next=s.next[i][j];const double length=detail::edgeLength(s,i,next);
            if(!(length>0)||!std::isfinite(length)||length/config.placeQuantum>1e15)return detail::fail(out,Error::Route);
            const double distance=length+s.distanceToFinish[next];
            if(i!=0&&distance<s.distanceToFinish[i]){s.distanceToFinish[i]=distance;changed=true;}
        }
        if(!changed)break;
    }
    for(std::uint32_t i=0;i<s.nodeCount;++i)
        if(!std::isfinite(s.distanceToFinish[i])||s.distanceToFinish[i]/config.placeQuantum>1e15)
            return detail::fail(out,Error::Route);
    const auto predecessor=laps.predecessorOfZero;
    if(!detail::linked(s,predecessor,0))return detail::fail(out,Error::Route);
    const double incomingX=double(s.positions[0][0])-s.positions[predecessor][0];
    const double incomingZ=double(s.positions[0][2])-s.positions[predecessor][2];
    const double gateX=double(map.nodes[0].gate20[0])-map.nodes[0].gate10[0];
    const double gateZ=double(map.nodes[0].gate20[2])-map.nodes[0].gate10[2];
    if(!std::isfinite(gateX)||!std::isfinite(gateZ))return detail::fail(out,Error::Route);
    // Use the actual source gate's bisector, rather than assuming its plane is
    // perpendicular to the incoming segment. Zero-width gates cannot refine
    // crossing time; leave the fallback projection for diagnostic fixtures.
    s.finishForward[0]=incomingX;s.finishForward[1]=incomingZ;
    if(gateX*gateX+gateZ*gateZ>0.0){
        s.finishForward[0]=-gateZ;s.finishForward[1]=gateX;
        if(s.finishForward[0]*incomingX+s.finishForward[1]*incomingZ<0.0){
            s.finishForward[0]=-s.finishForward[0];s.finishForward[1]=-s.finishForward[1];
        }
    }
    for(std::uint32_t i=0;i<racerCount;++i){s.ordered[i]=i;s.racers[i].place=i+1;}
    out=s;return true;
}

// Set the known authored/host grid order before the first committed sample.
// Front-to-back racer IDs must form one complete permutation. This only seeds
// stable place ties: it cannot alter positions, gates, checkpoints or laps.
// In particular, racers behind gate zero may all project to the same clamped
// edge start. Their known starting slots, not observation-array IDs, order them.
inline bool seedGridOrder(Session& s,const std::uint32_t* frontToBack,
    std::size_t count) noexcept {
    if(s.phase==Phase::Uninitialized||!frontToBack||count!=s.racerCount||
       s.raceSeconds!=0.0||s.finishCount)return detail::fail(s,Error::Configuration);
    bool seen[maxRacers]{};
    for(std::uint32_t i=0;i<s.racerCount;++i){
        if(s.racers[i].observedValid||frontToBack[i]>=s.racerCount||seen[frontToBack[i]])
            return detail::fail(s,Error::Configuration);
        seen[frontToBack[i]]=true;
    }
    for(std::uint32_t i=0;i<s.racerCount;++i){
        s.ordered[i]=frontToBack[i];s.racers[frontToBack[i]].place=i+1;
    }
    s.error=Error::None;return true;
}

// Called once per unpaused planned fixed step BEFORE physics. A step that
// reaches zero is still held, so all racers start together on the next step.
// The caller must not advance any vehicle/navigation state when false. Focus
// pauses must not call this; their elapsed wall time cannot consume countdown.
inline bool advanceCountdown(Session& s,double seconds) noexcept {
    if(!std::isfinite(seconds)||seconds<0.0||seconds>1.0){detail::fail(s,Error::Configuration);return false;}
    if(s.phase==Phase::Countdown){
        s.countdownRemaining=std::max(0.0,s.countdownRemaining-seconds);
        if(s.countdownRemaining<1e-9){s.countdownRemaining=0;s.phase=Phase::Racing;}
        s.error=Error::None;return false;
    }
    s.error=Error::None;return s.phase==Phase::Racing;
}
inline std::uint32_t countdownNumber(const Session& s) noexcept {
    return s.phase==Phase::Countdown?static_cast<std::uint32_t>(std::ceil(s.countdownRemaining)):0;
}
inline bool controlsAllowed(const Session& s,std::uint32_t racer) noexcept {
    return s.phase==Phase::Racing&&racer<s.racerCount&&!s.racers[racer].finished;
}
inline bool sampleRacer(Session& s,std::uint32_t index,const Observation& observation) noexcept {
    if(s.phase==Phase::Uninitialized||index>=s.racerCount||!detail::validObservation(s,observation))
        return detail::fail(s,Error::Observation);
    if(observation.lapCallbackPending)return detail::fail(s,Error::PendingLapCallback);
    auto& r=s.racers[index];
    if(r.observedValid){
        if(observation.completedLaps<r.observed.completedLaps)return detail::fail(s,Error::LapRegression);
        if(observation.completedLaps-r.observed.completedLaps>1)return detail::fail(s,Error::LapJump);
        if(s.phase==Phase::Countdown&&(observation.completedLaps!=r.observed.completedLaps||
            observation.checkpointCursor!=r.observed.checkpointCursor||
            observation.currentMapNode!=r.observed.currentMapNode||
            observation.previousMapNode!=r.observed.previousMapNode))
            return detail::fail(s,Error::CountdownMovement);
        if(s.phase==Phase::Countdown)for(unsigned axis=0;axis<3;++axis)
            if(observation.position[axis]!=r.observed.position[axis])
                return detail::fail(s,Error::CountdownMovement);
    }else if(observation.completedLaps)return detail::fail(s,Error::LapJump);
    r.pending=observation;r.sampled=true;s.error=Error::None;return true;
}
// Seed all actual grid observations with seconds=0 before countdown. After
// that, sample ALL racers after one completed native step and commit exactly
// once. A failed/abandoned frame must not advance this clock or seal a finish.
// seconds=0 rebases actual poses after source checkpoint recovery, preserving
// laps, timer, place and any finish. Recovery must not replay the countdown.
inline bool commitFrame(Session& s,double seconds) noexcept {
    if(s.phase==Phase::Uninitialized||!std::isfinite(seconds)||seconds<0.0||seconds>1.0||
       (s.phase==Phase::Countdown&&seconds!=0.0))return detail::fail(s,Error::Configuration);
    for(std::uint32_t i=0;i<s.racerCount;++i){
        const auto& r=s.racers[i];
        if(!r.sampled)return detail::fail(s,Error::MissingSample);
        if(seconds==0&&r.observedValid&&r.pending.completedLaps!=r.observed.completedLaps)
            return detail::fail(s,Error::LapJump);
    }
    std::uint32_t newlyFinished[maxRacers]{};std::size_t newlyCount=0;
    for(std::uint32_t i=0;i<s.racerCount;++i){
        auto& r=s.racers[i];
        if(s.phase==Phase::Racing&&seconds>0&&r.observedValid&&!r.finished&&
           r.pending.completedLaps>r.observed.completedLaps&&
           r.pending.completedLaps>=s.config.lapTarget){
            r.finishSeconds=s.raceSeconds+seconds*detail::finishFraction(s,r);
            newlyFinished[newlyCount++]=i;
        }
    }
    // Finishes in the same native step are ordered by actual forward crossing
    // time where observable, then the prior stable place. Later laps/motion
    // cannot reorder an already sealed result.
    for(std::size_t i=1;i<newlyCount;++i){
        auto id=newlyFinished[i];std::size_t j=i;
        while(j){const auto prev=newlyFinished[j-1];const auto& a=s.racers[id];const auto& b=s.racers[prev];
            if(a.finishSeconds>b.finishSeconds||(a.finishSeconds==b.finishSeconds&&a.place>b.place))break;
            newlyFinished[j]=prev;--j;
        }
        newlyFinished[j]=id;
    }
    for(std::size_t i=0;i<newlyCount;++i){auto& r=s.racers[newlyFinished[i]];
        r.finished=true;r.finishPlace=++s.finishCount;}
    for(std::uint32_t i=0;i<s.racerCount;++i){auto& r=s.racers[i];
        r.observed=r.pending;r.observedValid=true;r.sampled=false;
        r.remainingDistance=detail::remaining(s,r.observed);
        r.distanceBucket=static_cast<std::int64_t>(std::floor(r.remainingDistance/s.config.placeQuantum));
    }
    if(s.phase==Phase::Racing)s.raceSeconds+=seconds;
    detail::rank(s);
    if(s.finishCount==s.racerCount)s.phase=Phase::Complete;
    s.error=Error::None;return true;
}
inline std::uint32_t displayedLap(const Session& s,std::uint32_t racer) noexcept {
    if(racer>=s.racerCount||!s.config.lapTarget)return 0;
    const auto completed=s.racers[racer].observed.completedLaps;
    return completed>=s.config.lapTarget?s.config.lapTarget:completed+1;
}
} // namespace race_session50HM
