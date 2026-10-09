#pragma once
#include "ai_boost_decision50it.h"
#include "ai_cold_owners50iu.h"
#include "track_navigation50eu.h"
#include "player_finish50ia.h"
#include "chariot_scene49u.h"
#include <algorithm>
#include <array>
#include <limits>

namespace rr::game {
// Native branch-complete geometry for the bounded single-successor AI route.
// Runtime owners pass authentic scene, route and roster observations. Native
// resource limits reject unsupported branches; they do not choose a substitute.
constexpr std::size_t aiObservedCapacity50IV=16;
struct AiObservedRacer50IV {
    bool bound=false;
    std::uint32_t managerIndex=0,vehicle58=0,vehicle40=0,vehicleC80=0;
    float position190[3]{},cachedSpeed34=0.f;
    float progressCC4=0.f;
    std::uint8_t flagsB9C=0,flagsCD8=0;
};
struct AiRacerLists50IV {
    std::array<std::uint32_t,aiObservedCapacity50IV> values[5]{};
    std::uint32_t counts[5]{}; //1038,103C,1040,1044,1048, in manager order.
};
struct AiNavigationState50IV {
    std::int32_t choice1080=-1,choice1084=-1;
    std::uint32_t lastZone7F4=0;
};
enum class AiDrivingError50IV : std::uint8_t {
    None,Owner,Arithmetic,RouteChoice,Predecessor,Traversal,Capacity,Unsupported
};
struct AiDrivingDiagnostic50IV {
    AiDrivingError50IV error=AiDrivingError50IV::None;
    std::uint32_t sourcePC=0,node=0;
};
namespace ai_driving_detail50IV {
using V=VisualVector49R;
using Math=ai_speed_detail50IR::Math;
inline bool fail(AiDrivingDiagnostic50IV&d,AiDrivingError50IV e,std::uint32_t pc,std::uint32_t n=0) noexcept {d={e,pc,n};return false;}
inline V point(const float* v) noexcept {return {{v[0],v[1],v[2],1.f}};}
inline V sub(Math&f,const V&a,const V&b) noexcept {return {{f.sub(a.v[0],b.v[0]),f.sub(a.v[1],b.v[1]),f.sub(a.v[2],b.v[2]),1.f}};}
inline V add(Math&f,const V&a,const V&b) noexcept {return {{f.add(a.v[0],b.v[0]),f.add(a.v[1],b.v[1]),f.add(a.v[2],b.v[2]),1.f}};}
inline V scale(Math&f,const V&a,float k) noexcept {return {{f.mul(a.v[0],k),f.mul(a.v[1],k),f.mul(a.v[2],k),1.f}};}
inline float dot(Math&f,const V&a,const V&b) noexcept {return f.add(f.add(f.mul(a.v[0],b.v[0]),f.mul(a.v[1],b.v[1])),f.mul(a.v[2],b.v[2]));}
inline float root(Math&f,float v) noexcept {if(v<0.f){f.valid=false;return 0.f;}volatile float r=std::sqrt(v);return f.keep(r);}
inline V normalize(Math&f,const V&a) noexcept {const float n=dot(f,a,a);if(n==0.f)return {{0,0,0,1}};return scale(f,a,f.div(1.f,root(f,n)));}
inline float crossY(Math&f,const V&a,const V&b) noexcept {return f.sub(f.mul(a.v[2],b.v[0]),f.mul(a.v[0],b.v[2]));}
inline float clamp(float v,float lo,float hi) noexcept {return v<lo?lo:v>hi?hi:v;}
inline bool next(const TrackNavigation50EU&t,std::uint32_t n,const AiNavigationState50IV&s,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x171a90,n);
    const auto&node=t.track.nodes[n];
    if(node.nextCount==1u){out=node.next[0];return out<t.track.count||fail(d,AiDrivingError50IV::Owner,0x171ab8,n);}
    //171A90 honors a genuine previously selected route before selecting a new one.
    for(std::uint32_t i=0;i<node.nextCount;++i)if(static_cast<std::int32_t>(node.next[i])==s.choice1080||static_cast<std::int32_t>(node.next[i])==s.choice1084){out=node.next[i];return out<t.track.count;}
    return fail(d,AiDrivingError50IV::RouteChoice,0x1719b0,n);
}
inline bool previous(const TrackNavigation50EU&t,std::uint32_t n,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x2a26e0,n);
    if(n&&t.nodes[n].branch68==t.nodes[n-1].branch68){out=n-1;return true;}
    if(!n){for(std::uint32_t i=0;i<t.track.count;++i)if(t.track.nodes[i].nextCount&&t.track.nodes[i].next[0]==0){out=i;return true;}}
    else for(std::uint32_t i=n;i>0;--i)for(std::uint32_t k=0;k<t.track.nodes[i].nextCount;++k)
        if(t.track.nodes[i].next[k]==n){out=i;return true;}
    return fail(d,AiDrivingError50IV::Predecessor,0x2a26e0,n);
}
inline V closest(Math&f,const V&p,const float*left,const float*right) noexcept {
    const V a=point(left),v=sub(f,point(right),a);
    const float k=clamp(f.div(dot(f,v,sub(f,p,a)),dot(f,v,v)),0.f,1.f);
    return add(f,a,scale(f,v,k));
}
inline bool crossGate(Math&f,const float*previous,const float*current,const float*left,const float*right,bool&hit) noexcept {
    hit=false;const float px=previous[0],pz=previous[2];
    const float dx=f.sub(current[0],px),dz=f.sub(current[2],pz),gx=f.sub(right[0],left[0]),gz=f.sub(right[2],left[2]);
    const float denominator=f.sub(f.mul(dz,gx),f.mul(gz,dx));if(denominator==0.f)return f.valid;
    const float a=f.div(f.add(f.mul(gx,f.sub(left[2],pz)),f.mul(gz,f.sub(px,left[0]))),denominator);
    const float b=gx!=0.f?f.div(f.sub(f.add(px,f.mul(dx,a)),left[0]),gx):f.div(f.sub(f.add(pz,f.mul(dz,a)),left[2]),gz);
    hit=!(a<0.f||a>1.f||b<0.f||b>1.f);return f.valid;
}
}
//16FFE0 writes progression, real zone ordinal, lap edge and SAME109C consumed
// by173180. The caller supplies actual7F0->4->48, never a nearest-zone guess.
inline bool advanceAiNavigation50IV(const TrackNavigation50EU&map,const TrackNavigation50EU&ai,
    const AiTrackMetadata50IU&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;d={};Math f;
    if(!map.gatesPrepared||!ai.gatesPrepared||!metadata.bound||metadata.count!=ai.track.count||!boost.initialized)
        return fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    std::uint32_t visited=0;
    for(;;){const auto n=progress.reset.vehicleC80;
        if(n>=ai.track.count)return fail(d,AiDrivingError50IV::Owner,0x1700b8,n);
        bool hit=false;const auto&g=ai.nodes[n];
        if(!crossGate(f,previousCA0,local190,g.wideGate30,g.wideGate40,hit))return fail(d,AiDrivingError50IV::Arithmetic,0x170380,n);
        if(!hit)break;if(++visited>ai.track.count)return fail(d,AiDrivingError50IV::Traversal,0x1700c8,n);
        state.lastZone7F4=actualZoneOrdinal48;progress.vehicleC84=n;
        std::uint32_t successor;if(!next(ai,n,state,successor,d))return false;
        progress.reset.vehicleC80=successor;
        if(metadata.nodes[successor].boostScale64>1.f)boost.cooldown109C=0.f;
        if(static_cast<std::int32_t>(successor)==state.choice1080){state.choice1080=state.choice1084;state.choice1084=-1;}
    }
    auto mapCross=[&](std::uint32_t n,bool&hit) noexcept {
        if(n>=map.track.count)return false;const auto&g=map.nodes[n];
        if(!crossGate(f,previousCA0,local190,g.gate10,g.gate20,hit))return false;
        if(hit)hit=std::fabs(f.sub(map.track.nodes[n].position[1],local190[1]))<g.height50;
        return f.valid;
    };
    visited=0;
    for(;;){const auto n=progress.reset.vehicleC88;bool hit=false;
        if(!mapCross(n,hit))return fail(d,AiDrivingError50IV::Arithmetic,0x170230,n);
        if(!hit)break;if(++visited>map.track.count)return fail(d,AiDrivingError50IV::Traversal,0x170150,n);
        if(!n&&!(flagsCD8&1u)){++lapCC8;flagsCD8=static_cast<std::uint8_t>(flagsCD8|2u);}
        if(!map.track.nodes[n].nextCount)return fail(d,AiDrivingError50IV::Owner,0x170128,n);
        progress.reset.vehicleC8C=n;progress.reset.vehicleC88=map.track.nodes[n].next[0];
    }
    if(visited)return true;
    if(progress.reset.vehicleC88>=map.track.count)return fail(d,AiDrivingError50IV::Owner,0x170168);
    const auto branch=map.nodes[progress.reset.vehicleC88].branch68;
    for(std::uint32_t n=0;n<map.track.count;++n){const auto&g=map.nodes[n];if(g.branch68!=branch&&(g.flags79&8u))continue;
        bool hit=false;if(!mapCross(n,hit))return fail(d,AiDrivingError50IV::Arithmetic,0x1701b4,n);
        if(!hit)continue;if(!map.track.nodes[n].nextCount)return fail(d,AiDrivingError50IV::Owner,0x1701e0,n);
        state.lastZone7F4=actualZoneOrdinal48;progress.reset.vehicleC8C=n;progress.reset.vehicleC88=map.track.nodes[n].next[0];break;
    }return true;
}
//171FE0: segment/gate projection is intentionally not centerline interpolation.
inline bool aiDesiredPosition50IV(const TrackNavigation50EU&ai,const TrackNavigationProgress50EU&p,
    const AiNavigationState50IV&state,const float local190[3],float horizon,
    VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;Math f;const bool backwards=horizon<0.f;
    float remaining=std::fabs(horizon);V current=point(local190);std::uint32_t n=backwards?p.vehicleC84:p.reset.vehicleC80;
    V result=out;std::uint32_t visits=0;
    while(remaining>0.f){if(n>=ai.track.count)return fail(d,AiDrivingError50IV::Owner,0x171fe0,n);
        if(++visits>ai.track.count+1u)return fail(d,AiDrivingError50IV::Traversal,0x172344,n);
        const auto&g=ai.nodes[n];const V gate=closest(f,current,g.gate10,g.gate20);
        const V delta=sub(f,current,gate);const float distance=root(f,dot(f,delta,delta));
        if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172138,n);
        if(distance<=remaining){current=point(ai.track.nodes[n].position);
            std::uint32_t chosen;if(!(backwards?previous(ai,n,chosen,d):next(ai,n,state,chosen,d)))return false;n=chosen;remaining=f.sub(remaining,distance);
        }else{result=add(f,current,scale(f,normalize(f,sub(f,point(ai.track.nodes[n].position),current)),remaining));remaining=0.f;}
    }
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172344,n);out=result;return true;
}
//170B50: all manager racers, including the human, retained in original order.
inline bool buildAiRacerLists50IV(const TrackNavigation50EU&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;d={};Math f;AiRacerLists50IV result{};
    if(!racers||!count||count>aiObservedCapacity50IV||selfIndex>=count||p.reset.vehicleC80>=ai.track.count)
        return fail(d,AiDrivingError50IV::Owner,0x170b50);
    const auto n=p.reset.vehicleC80;const auto&node=ai.track.nodes[n];const auto&g=ai.nodes[n];
    if(!node.nextCount)return fail(d,AiDrivingError50IV::Owner,0x170d84,n);
    const V position=point(local190),gate=closest(f,position,g.gate10,g.gate20),gap=sub(f,position,gate);
    const float radius=f.integer(profile.collisionRadius1020);
    const bool nearGate=root(f,dot(f,gap,gap))<=radius;
    const V direction=scale(f,point(localForward),radius),up{{0,1,0,1}};
    V negative{},positive{},back{};
    if(!detail::rotateVector49U(up,direction,ai_speed_detail50IR::word(0xbec90fdbu),negative)||
       !detail::rotateVector49U(up,direction,ai_speed_detail50IR::word(0x3ec90fdbu),positive)||
       !detail::rotateVector49U(up,direction,ai_speed_detail50IR::word(0x3f490fdbu),back))
        return fail(d,AiDrivingError50IV::Arithmetic,0x142b80);
    back=scale(f,back,-1.f);
    for(std::size_t i=0;i<count;++i){const auto&r=racers[i];
        if(!r.bound||r.managerIndex!=i)return fail(d,AiDrivingError50IV::Owner,0x1af330,static_cast<std::uint32_t>(i));
        if(i==selfIndex)continue;
        if(r.vehicleC80!=n&&(!nearGate||r.vehicleC80!=node.next[0]))continue;
        const V delta=sub(f,point(r.position190),position);
        const float squared=f.add(f.mul(delta.v[0],delta.v[0]),f.mul(delta.v[2],delta.v[2]));
        if(squared>10000.f)continue;unsigned list=0;
        if(crossY(f,negative,delta)>0.f){
            if(crossY(f,positive,delta)>0.f){if(!(squared<2500.f))continue;list=2;}
            else list=crossY(f,direction,delta)>0.f?1u:0u;
        }else if(crossY(f,back,delta)>0.f){if(!(squared<2500.f))continue;list=4;}
        else{if(!(squared<2500.f))continue;list=3;}
        result.values[list][result.counts[list]++]=static_cast<std::uint32_t>(i);
    }
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x1711ac);out=result;return true;
}
//172E60 source steering. Main scene axes are LOCAL 28A530 outputs.
inline bool steerAi50IV(const float direction[3],const float forward[3],float upY,
    float cachedSpeed,float engineDifference,float angularA4,std::uint32_t racer60,
    PlayerReadResult48X&actions,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;Math f;float desired,current;
    if(!player_finish_detail50IA::directionAngle(direction,desired)||!player_finish_detail50IA::directionAngle(forward,current))return fail(d,AiDrivingError50IV::Arithmetic,0x172df0);
    if(upY<0.f){desired=-desired;current=-current;}
    if(current>0.f&&desired<0.f)desired=f.add(desired,ai_speed_detail50IR::word(0x40c90fdbu));
    float error=f.sub(current,desired);if(error<ai_speed_detail50IR::word(0xc0490fdbu))error=f.add(error,ai_speed_detail50IR::word(0x40c90fdbu));
    float gain=3.f;const float k=ai_speed_detail50IR::word(0x3f2e8bc1u),speed=f.mul(k,cachedSpeed),basis=f.mul(k,engineDifference);
    if(std::fabs(error)>ai_speed_detail50IR::word(0x3e4ccccdu)){
        const float hi=f.add(basis,100.f),lo=f.sub(basis,200.f),offset=f.sub(speed,lo);
        gain=hi==lo?0.f:clamp(f.add(3.f,f.div(f.mul(offset,2.f),f.sub(hi,lo))),3.f,5.f);
    }
    if(racer60==8u||racer60==18u||racer60==23u||racer60==4u)gain=f.mul(gain,1.25f);
    float steering=f.sub(f.mul(gain,error),angularA4);const float absolute=std::fabs(steering);if(absolute>1.f)steering=f.div(steering,absolute);
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x173154);actions.raw.values[0]=steering;actions.raw.values[1]=0.f;return true;
}
//1726F0 after its171FE0/172E60 direction. False attemptBoost is the genuine
// front-list branch, not a fabricated173180 return. Source keeps this separate.
inline bool aiStartupSpeed50IV(const AiTrackNodeMetadata50IU&node,const AiProfile50IU&profile,
    const AiRacerLists50IV&lists,const AiObservedRacer50IV*racers,std::size_t count,
    const float position[3],const float velocity[3],PlayerReadResult48X&actions,
    float&desired,bool&attemptBoost,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;Math f;float target=node.speedLimit5C,steer=actions.raw.values[0];
    attemptBoost=!(lists.counts[0]||lists.counts[1]);
    if(!attemptBoost){const auto slot=lists.values[lists.counts[1]?1:0][0];
        if(slot>=count||!racers[slot].bound)return fail(d,AiDrivingError50IV::Owner,0x172850,slot);
        const float distance=dot(f,sub(f,point(racers[slot].position190),point(position)),normalize(f,point(velocity)));
        const float gap=f.sub(f.sub(distance,25.f),100.f);
        target=f.mul(ai_speed_detail50IR::word(0x3f2e8bc1u),f.div(f.add(gap,f.mul(3.f,racers[slot].cachedSpeed34)),3.f));
        if(lists.counts[1]){if(!lists.counts[4])steer=std::max(-1.f,f.sub(steer,lists.counts[2]?.5f:.25f));}
        else if(!lists.counts[2])steer=std::min(1.f,f.add(steer,lists.counts[4]?.5f:.25f));
    }else if(lists.counts[2])steer=std::max(-1.f,f.sub(steer,.5f));
    else if(lists.counts[4])steer=std::min(1.f,f.add(steer,.5f));
    const float minimum=f.integer(profile.myMinSpeed108C);if(target<minimum)target=minimum;
    if(node.flags79&0x10u)target=node.speedLimit5C;
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172af8);actions.raw.values[0]=steer;desired=target;return true;
}
} //namespace rr::game
