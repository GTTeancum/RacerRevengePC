#pragma once
#include "lap_progress50gf.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <variant>

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
    LapJump, PendingLapCallback, CountdownMovement, FinishedLapMutation
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
    // observed is the official ranking snapshot; live continues after the
    // manager closes results so native progression is still validated.
    Observation observed{},live{},pending{};
    bool observedValid=false,sampled=false,finished=false;
    std::uint32_t place=0,finishPlace=0;
    // Optional one-way native presentation value. Copy the real CD4 marker;
    // retirement is independent of the CD8/list44 finish and never ranks or
    // advances this HOST observer. Zero leaves the existing HOST path alone.
    std::uint32_t retirementCD4=0;
    double remainingDistance=0,finishSeconds=0;
    std::int64_t distanceBucket=0;
};
// Shared actual scalar/racer rows for non-owning camera/HUD observers.
// A pointer to this base refers to the live session, never a copied projection.
struct SessionRows {
    Config config{};
    Phase phase=Phase::Uninitialized;
    Error error=Error::None;
    bool resultsClosed=false;
    std::uint32_t racerCount=0,nodeCount=0,checkpointCount=0,finishCount=0;
    std::uint32_t ordered[maxRacers]{};
    Racer racers[maxRacers]{};
    double countdownRemaining=0,raceSeconds=0;
};
template<std::size_t NodeCapacity> struct SessionStorage : SessionRows {
    static_assert(NodeCapacity==rr::game::maxUndertrackNodes50ET || NodeCapacity==265 || NodeCapacity==268 || NodeCapacity==270);
    static constexpr std::size_t nodeCapacity=NodeCapacity;
    float positions[NodeCapacity][3]{};
    double distanceToFinish[NodeCapacity]{};
    std::uint32_t nextCount[NodeCapacity]{};
    std::uint32_t next[NodeCapacity][rr::game::maxUndertrackNext50ET]{};
    double finishForward[2]{};
};
using Session=SessionStorage<rr::game::maxUndertrackNodes50ET>;
// Exact MB presentation extent. This HOST observer does not create lap samples.
// Only the real final Map view can keep its new route usable.
class MbSession final:public SessionStorage<265> {
    rr::game::TrackNavigationView50EU navigation_{};
    friend bool initialize(MbSession&,const rr::game::TrackNavigationView50EU&,
        const rr::game::LapProgressTrackView50MB&,std::uint32_t,const Config&) noexcept;
public:
    bool runtimeValid() const noexcept {
        return navigation_.runtimeValid() &&
            navigation_.role()==rr::game::NavigationRole50MB::Map && navigation_.count()==265;
    }
};
// Distinct exact TC observer retains the genuine final Map268 view.
class TcSession final:public SessionStorage<268> {
    rr::game::TrackNavigationView50TC navigation_{};
    friend bool initialize(TcSession&,const rr::game::TrackNavigationView50TC&,
        const rr::game::LapProgressTrackView50TC&,std::uint32_t,const Config&) noexcept;
public:
    bool runtimeValid() const noexcept {
        return navigation_.runtimeValid() &&
            navigation_.role()==rr::game::NavigationRole50TC::Map && navigation_.count()==268;
    }
};
// Distinct exact TB observer retains the genuine final Map270 view.
class TbSession final:public SessionStorage<270> {
    rr::game::TrackNavigationView50TB navigation_{};
    friend bool initialize(TbSession&,const rr::game::TrackNavigationView50TB&,
        const rr::game::LapProgressTrackView50TB&,std::uint32_t,const Config&) noexcept;
public:
    bool runtimeValid() const noexcept {
        return navigation_.runtimeValid() &&
            navigation_.role()==rr::game::NavigationRole50TB::Map && navigation_.count()==270;
    }
};
template<class T> concept SessionType=std::is_same_v<T,Session>||std::is_same_v<T,MbSession>||std::is_same_v<T,TcSession>||std::is_same_v<T,TbSession>;
namespace detail {
template<SessionType S> inline bool lifetimeValid(const S& s) noexcept {
    if constexpr(std::is_same_v<S,MbSession>||std::is_same_v<S,TcSession>||std::is_same_v<S,TbSession>) return s.runtimeValid();
    else return true;
}
template<class S> inline bool fail(S& s,Error e) noexcept {s.error=e;return false;}
template<class S> inline double edgeLength(const S& s,std::uint32_t a,std::uint32_t b) noexcept {
    const double x=double(s.positions[b][0])-s.positions[a][0];
    const double z=double(s.positions[b][2])-s.positions[a][2];
    return std::sqrt(x*x+z*z);
}
template<class S> inline bool linked(const S& s,std::uint32_t a,std::uint32_t b) noexcept {
    for(std::uint32_t i=0;i<s.nextCount[a];++i)if(s.next[a][i]==b)return true;
    return false;
}
template<class S> inline bool validObservation(const S& s,const Observation& o) noexcept {
    if(o.previousMapNode>=s.nodeCount||o.currentMapNode>=s.nodeCount||
       o.checkpointCursor>s.checkpointCount||
       !linked(s,o.previousMapNode,o.currentMapNode))return false;
    for(float v:o.position)if(!std::isfinite(v))return false;
    return true;
}
template<class S> inline double remaining(const S& s,const Observation& o) noexcept {
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
template<class S> inline double finishFraction(const S& s,const Racer& r) noexcept {
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
template<class S> inline bool ahead(const S& s,std::uint32_t a,std::uint32_t b) noexcept {
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
template<class S> inline void rank(S& s) noexcept {
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

namespace detail {
template<std::size_t Capacity,class Navigation>
inline bool initializeStorage(SessionStorage<Capacity>& out,const Navigation& map,
    const rr::game::LapProgressTrack50GF& laps,std::uint32_t racerCount,
    const Config& config) noexcept {
    if(!racerCount||racerCount>maxRacers||!config.lapTarget||config.lapTarget>999||
       !std::isfinite(config.countdownSeconds)||config.countdownSeconds<0.0||config.countdownSeconds>3600.0||
       !std::isfinite(config.placeQuantum)||config.placeQuantum<.0001||
       !map.gatesPrepared||!map.track.count||map.track.count>Capacity||
       !laps.prepared||laps.nodeCount!=map.track.count||
       laps.progressCount48>rr::game::maxLapProgressNodes50GF||
       laps.predecessorOfZero>=map.track.count)
        return detail::fail(out,Error::Configuration);
    SessionStorage<Capacity> s{};s.config=config;s.racerCount=racerCount;s.nodeCount=map.track.count;
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

} // namespace detail
inline bool initialize(Session& out,const rr::game::TrackNavigation50EU& map,
    const rr::game::LapProgressTrack50GF& laps,std::uint32_t racerCount,
    const Config& config={}) noexcept {
    return detail::initializeStorage(out,map,laps,racerCount,config);
}
inline bool initialize(MbSession& out,const rr::game::TrackNavigationView50EU& map,
    const rr::game::LapProgressTrackView50MB& laps,std::uint32_t racerCount,
    const Config& config={}) noexcept {
    if(!map.runtimeValid()||map.role()!=rr::game::NavigationRole50MB::Map||
       map.count()!=265||!laps.matches(map)) return detail::fail(out,Error::Configuration);
    // Non-owning read facade lives only during this synchronous copy. The final
    // view retained below owns the same immutable graph and actual live world.
    struct MapFacade {
        struct Graph {const rr::game::UndertrackNode50ET* nodes;std::uint32_t count;} track;
        const rr::game::TrackNavigationNode50EU* nodes;
        bool gatesPrepared;
    } facade{{map.track().nodes(),map.count()},map.nodes(),map.gatesPrepared()};
    MbSession candidate;
    if(!detail::initializeStorage(static_cast<SessionStorage<265>&>(candidate),
            facade,laps.data(),racerCount,config)) return detail::fail(out,candidate.error);
    candidate.navigation_=map;out=std::move(candidate);return true;
}

inline bool initialize(TcSession& out,const rr::game::TrackNavigationView50TC& map,
    const rr::game::LapProgressTrackView50TC& laps,std::uint32_t racerCount,
    const Config& config={}) noexcept {
    if(!map.runtimeValid()||map.role()!=rr::game::NavigationRole50TC::Map||
       map.count()!=268||!laps.matches(map)) return detail::fail(out,Error::Configuration);
    // Non-owning read facade lives only during this synchronous copy. The final
    // view retained below owns the same immutable graph and actual live world.
    struct MapFacade {
        struct Graph {const rr::game::UndertrackNode50ET* nodes;std::uint32_t count;} track;
        const rr::game::TrackNavigationNode50EU* nodes;
        bool gatesPrepared;
    } facade{{map.track().nodes(),map.count()},map.nodes(),map.gatesPrepared()};
    TcSession candidate;
    if(!detail::initializeStorage(static_cast<SessionStorage<268>&>(candidate),
            facade,laps.data(),racerCount,config)) return detail::fail(out,candidate.error);
    candidate.navigation_=map;out=std::move(candidate);return true;
}

inline bool initialize(TbSession& out,const rr::game::TrackNavigationView50TB& map,
    const rr::game::LapProgressTrackView50TB& laps,std::uint32_t racerCount,
    const Config& config={}) noexcept {
    if(!map.runtimeValid()||map.role()!=rr::game::NavigationRole50TB::Map||
       map.count()!=270||!laps.matches(map)) return detail::fail(out,Error::Configuration);
    // Non-owning read facade lives only during this synchronous copy. The final
    // view retained below owns the same immutable graph and actual live world.
    struct MapFacade {
        struct Graph {const rr::game::UndertrackNode50ET* nodes;std::uint32_t count;} track;
        const rr::game::TrackNavigationNode50EU* nodes;
        bool gatesPrepared;
    } facade{{map.track().nodes(),map.count()},map.nodes(),map.gatesPrepared()};
    TbSession candidate;
    if(!detail::initializeStorage(static_cast<SessionStorage<270>&>(candidate),
            facade,laps.data(),racerCount,config)) return detail::fail(out,candidate.error);
    candidate.navigation_=map;out=std::move(candidate);return true;
}

// Set the known authored/host grid order before the first committed sample.
// Front-to-back racer IDs must form one complete permutation. This only seeds
// stable place ties: it cannot alter positions, gates, checkpoints or laps.
// In particular, racers behind gate zero may all project to the same clamped
// edge start. Their known starting slots, not observation-array IDs, order them.
template<SessionType S> inline bool seedGridOrder(S& s,const std::uint32_t* frontToBack,
    std::size_t count) noexcept {
    if(!detail::lifetimeValid(s)) return detail::fail(s,Error::Configuration);
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
template<SessionType S> inline bool advanceCountdown(S& s,double seconds) noexcept {
    if(!detail::lifetimeValid(s)) return detail::fail(s,Error::Configuration);
    if(!std::isfinite(seconds)||seconds<0.0||seconds>1.0){detail::fail(s,Error::Configuration);return false;}
    if(s.phase==Phase::Countdown){
        s.countdownRemaining=std::max(0.0,s.countdownRemaining-seconds);
        if(s.countdownRemaining<1e-9){s.countdownRemaining=0;s.phase=Phase::Racing;}
        s.error=Error::None;return false;
    }
    // Results completion does not remove native vehicles from their update
    // owner. The source finish bit and controller choose post-finish behavior.
    s.error=Error::None;return s.phase==Phase::Racing||s.phase==Phase::Complete;
}
template<SessionType S> inline std::uint32_t countdownNumber(const S& s) noexcept {
    if(!detail::lifetimeValid(s)) return 0;
    return s.phase==Phase::Countdown?static_cast<std::uint32_t>(std::ceil(s.countdownRemaining)):0;
}
// Human race-input permission is separate from native world participation.
template<SessionType S> inline bool controlsAllowed(const S& s,std::uint32_t racer) noexcept {
    if(!detail::lifetimeValid(s)) return false;
    return s.phase==Phase::Racing&&racer<s.racerCount&&!s.racers[racer].finished;
}
template<SessionType S> inline bool simulationAllowed(const S& s,std::uint32_t racer) noexcept {
    if(!detail::lifetimeValid(s)) return false;
    return racer<s.racerCount&&(s.phase==Phase::Racing||s.phase==Phase::Complete);
}
template<SessionType S> inline bool sampleRacer(S& s,std::uint32_t index,const Observation& observation) noexcept {
    if(!detail::lifetimeValid(s)) return detail::fail(s,Error::Configuration);
    if(s.phase==Phase::Uninitialized||index>=s.racerCount||!detail::validObservation(s,observation))
        return detail::fail(s,Error::Observation);
    if(observation.lapCallbackPending)return detail::fail(s,Error::PendingLapCallback);
    auto& r=s.racers[index];
    if(r.observedValid){
        // Native CD8 bit0 suppresses additional lap increments after the result
        // seals. Continue sampling real poses/checkpoints, but reject a broken
        // finish projection instead of silently changing the sealed lap result.
        if(r.finished&&observation.completedLaps!=r.live.completedLaps)
            return detail::fail(s,Error::FinishedLapMutation);
        if(observation.completedLaps<r.live.completedLaps)return detail::fail(s,Error::LapRegression);
        if(observation.completedLaps-r.live.completedLaps>1)return detail::fail(s,Error::LapJump);
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
template<SessionType S> inline bool commitFrame(S& s,double seconds) noexcept {
    if(!detail::lifetimeValid(s)) return detail::fail(s,Error::Configuration);
    if(s.phase==Phase::Uninitialized||!std::isfinite(seconds)||seconds<0.0||seconds>1.0||
       (s.phase==Phase::Countdown&&seconds!=0.0))return detail::fail(s,Error::Configuration);
    for(std::uint32_t i=0;i<s.racerCount;++i){
        const auto& r=s.racers[i];
        if(!r.sampled)return detail::fail(s,Error::MissingSample);
        if(seconds==0&&r.observedValid&&r.pending.completedLaps!=r.live.completedLaps)
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
        r.live=r.pending;r.sampled=false;
        if(!s.resultsClosed){
            r.observed=r.pending;r.observedValid=true;
            r.remainingDistance=detail::remaining(s,r.observed);
            r.distanceBucket=static_cast<std::int64_t>(std::floor(r.remainingDistance/s.config.placeQuantum));
        }
    }
    if(s.phase==Phase::Racing)s.raceSeconds+=seconds;
    if(!s.resultsClosed)detail::rank(s);
    if(s.finishCount==s.racerCount)s.phase=Phase::Complete;
    s.error=Error::None;return true;
}
// Original manager closes official results after all humans have finished.
// Call at the accepted world boundary after any same-frame finish edges were
// sealed. This host has one human; it must not wait for remaining AI results.
// Live sampling/validation and simulation continue independently afterward.
template<SessionType S> inline bool closeResults(S& s) noexcept {
    if(!detail::lifetimeValid(s)) return detail::fail(s,Error::Configuration);
    if((s.phase!=Phase::Racing&&s.phase!=Phase::Complete)||!s.finishCount)
        return detail::fail(s,Error::Configuration);
    for(std::uint32_t i=0;i<s.racerCount;++i)
        if(!s.racers[i].observedValid||s.racers[i].sampled)
            return detail::fail(s,Error::MissingSample);
    s.resultsClosed=true;s.phase=Phase::Complete;s.error=Error::None;return true;
}
template<SessionType S> inline std::uint32_t displayedLap(const S& s,std::uint32_t racer) noexcept {
    if(!detail::lifetimeValid(s)) return 0;
    if(racer>=s.racerCount||!s.config.lapTarget)return 0;
    const auto completed=s.racers[racer].observed.completedLaps;
    return completed>=s.config.lapTarget?s.config.lapTarget:completed+1;
}
// Closed, non-owning borrow of an actual typed session. The containing
// camera/session owner must outlive this borrow, as for the old Session&.
// No default constructor, raw pointer or scalar-row constructor is exposed.
class SessionBorrow final {
    std::variant<Session*,MbSession*,TcSession*,TbSession*> owner_;
public:
    SessionBorrow(Session& s) noexcept:owner_(&s){}
    SessionBorrow(MbSession& s) noexcept:owner_(&s){}
    SessionBorrow(TcSession& s) noexcept:owner_(&s){}
    SessionBorrow(TbSession& s) noexcept:owner_(&s){}
    bool runtimeValid() const noexcept {
        return std::visit([](auto* s){return detail::lifetimeValid(*s);},owner_);
    }
    SessionRows& rows() const noexcept {
        return std::visit([](auto* s)->SessionRows& {return *s;},owner_);
    }
    bool advanceCountdown(double seconds) const noexcept {
        return std::visit([seconds](auto* s){
            return race_session50HM::advanceCountdown(*s,seconds);
        },owner_);
    }
};
} // namespace race_session50HM
