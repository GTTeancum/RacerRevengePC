#pragma once
#include "ai_boost_decision50it.h"
#include "ai_cold_owners50iu.h"
#include "track_navigation50eu.h"
#include "navigation_consumer_bridge50mb.h"
#include "player_finish50ia.h"
#include "chariot_scene49u.h"
#include <algorithm>
#include <array>
#include <limits>
#include <cstdio>
#if defined(__SSE__)
#include <xmmintrin.h>
#endif

namespace rr::game {
// Native geometry and source-owned route selection for the bounded AI graph.
// Runtime owners pass authentic scene, route and roster observations. Native
// resource limits reject unsupported branches; they do not choose a substitute.
constexpr std::size_t aiObservedCapacity50IV=16;
struct AiObservedRacer50IV {
    bool bound=false;
    std::uint32_t managerIndex=0,vehicle58=0,vehicle40=0,vehicleC80=0;
    float position190[3]{},cachedSpeed34=0.f;
    float progressCC4=0.f;
    std::int32_t lapCC8=0;
    std::uint8_t flagsB9C=0,flagsCD8=0;
    // Actual serial manager rank and target owners consumed by1706C0,
    //171430,16EDD0 and16FB50. No HOST pose, rank or acceleration derivation.
    std::int32_t placeCCC=0;
    std::uint32_t vehicleC84=0;
    float forward190[3]{},up190[3]{},velocity70[3]{},linearAccelerationC0[3]{};
};
struct AiRacerLists50IV {
    std::array<std::uint32_t,aiObservedCapacity50IV> values[5]{};
    std::uint32_t counts[5]{}; //1038,103C,1040,1044,1048, in manager order.
};
struct AiNavigationState50IV {
    std::int32_t choice1080=-1,choice1084=-1;
    std::uint32_t lastZone7F4=0;
    std::uint64_t routeSelections=0,routeRandomDraws=0; // Observation only.
};
// Borrow canonical owners, so later lookahead calls see this same frame's
// state/packing writes and share the actual application-wide308030 stream.
struct AiRouteChoiceOwners50IY {
    const AiTrackMetadata50IU& metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// Strong immutable MB views retain their authentic storage and final world.
struct AiRouteChoiceOwners50MB {
    AiTrackMetadataView50IU metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// TC-CONSUMER-BEGIN 0
struct AiRouteChoiceOwners50TC {
    AiTrackMetadataView50TC metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// TC-CONSUMER-END 0

// TB-CONSUMER-BEGIN 0
struct AiRouteChoiceOwners50TB {
    AiTrackMetadataView50TB metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// TB-CONSUMER-END 0

// GB-CONSUMER-BEGIN 0
struct AiRouteChoiceOwners50GB {
    AiTrackMetadataView50GB metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// GB-CONSUMER-END 0

// MC-CONSUMER-BEGIN 0
struct AiRouteChoiceOwners50MC {
    AiTrackMetadataView50MC metadata;
    const std::uint32_t& fieldFE4;
    const std::uint32_t& aiStateFE8;
    const std::uint32_t& packingPolicy1000;
    std::uint32_t& sharedRandomState;
    // Borrow source-owned scalars: state/target selection can change after
    // this object is constructed and before same-frame171FE0 calls.
    const std::uint32_t* attackMode1050=nullptr;
    const bool* target1008Present=nullptr;
    const std::uint32_t* target1008ManagerIndex=nullptr;
    const AiObservedRacer50IV* racers=nullptr;
    std::size_t racerCount=0;
};
// MC-CONSUMER-END 0

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
// Steering-only observation. The original shared Math remains unchanged.
// Operation tags: 1 add, 2 sub, 3 mul, 4 div. Stage tags name source
// expression groups below; ordinal is the actual evaluated operation order.
struct SteeringMathObservation50IV {
    bool valid=true;
    std::uint32_t stage=0,ordinal=0,badStage=0,badOrdinal=0,badOperation=0;
    std::uint32_t aBits=0,bBits=0,resultBits=0;
    static std::uint32_t bits(float v) noexcept {std::uint32_t b;std::memcpy(&b,&v,sizeof b);return b;}
    void rejected(unsigned op,float a,float b,float result) noexcept {
        if(badOrdinal)return;
        badStage=stage;badOrdinal=ordinal;badOperation=op;
        aBits=bits(a);bBits=bits(b);resultBits=bits(result);
    }
    float keep(float value,unsigned op,float a,float b) noexcept {
        const bool before=valid;
        valid=valid&&ai_speed_detail50IR::admitted(value);
        if(before&&!valid)rejected(op,a,b,value);
        return value;
    }
    float add(float a,float b) noexcept {++ordinal;volatile float value=a+b;return keep(value,1,a,b);}
    float sub(float a,float b) noexcept {++ordinal;volatile float value=a-b;return keep(value,2,a,b);}
    float mul(float a,float b) noexcept {++ordinal;volatile float value=a*b;return keep(value,3,a,b);}
    //17300C actual cached-speed conversion: finite input/product native
    // policy, including when the original live gain branch consumes it.
    // Preserve actual multiply bits/ordinal and all later result guards.
    float mulSteeringSpeed(float a,float b) noexcept {
        ++ordinal;volatile float value=a*b;
        const bool before=valid;
        valid=valid&&std::isfinite(a)&&std::isfinite(b)&&std::isfinite(value);
        if(before&&!valid)rejected(3,a,b,value);
        return value;
    }
    float div(float a,float b) noexcept {
        ++ordinal;
        if(b==0.f){if(valid)rejected(4,a,b,0.f);valid=false;return 0.f;}
        volatile float value=a/b;return keep(value,4,a,b);
    }
};
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
//1718C0 and171630(policy4): first source-order minimum among nodes whose
// NodeBlocked79.0 and Pretty79.6 are clear. -1 is the source sentinel.
template<class Navigation,class Metadata>
inline bool shortestRouteShared50MB(const Navigation&source_t,const UndertrackNode50ET&node,
    const Metadata&source_metadata,std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiJoined(source_t,source_metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x2a2540u);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    const auto& metadata=navigation_consumer_detail50MB::read(source_metadata);
    if(!metadata.bound||metadata.count!=t.track.count)
        return fail(d,AiDrivingError50IV::Owner,0x2a2540u);
    float minimum=-1.f;ordinal=-1;
    for(std::uint32_t i=0;i<node.nextCount;++i){
        const auto successor=node.next[i];
        if(successor>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x2a2560u,successor);
        const auto flags=t.nodes[successor].flags79;
        if(flags&1u)continue;
        if(flags&0x40u)continue;
        const float distance=metadata.nodes[successor].distanceToFinish60;
        if(!ai_speed_detail50IR::admitted(distance))
            return fail(d,AiDrivingError50IV::Arithmetic,0x2a2540u,successor);
        if(minimum==-1.f||distance<minimum){minimum=distance;ordinal=static_cast<std::int32_t>(i);}
    }
    return true;
}
inline bool shortestRoute(const TrackNavigation50EU&t,const UndertrackNode50ET&node,
    const AiTrackMetadata50IU&metadata,std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    return shortestRouteShared50MB(t,node,metadata,ordinal,d);
}
//171430: for a human beyond a fork, infer the selected branch from its
// actual local190 forward. Scores include every successor, even blocked or
// Pretty nodes; only171320 subsequently applies those filters. Initial best
// score is0 and default branch is successor0, so all nonpositive scores and
// ties preserve that source choice. No RNG or route-cache writes occur here.
template<class Navigation>
inline bool predictAttackBranch171430Shared50MB(const Navigation&source_t,
    const AiObservedRacer50IV&target,std::int32_t&branch,
    AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiReady(source_t))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x17145cu);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    if(!t.track.count||t.track.count>navigation_consumer_detail50MB::capacity(t)||target.vehicleC84>=t.track.count)
        return fail(d,AiDrivingError50IV::Owner,0x17145cu,target.vehicleC84);
    const auto&previousNode=t.track.nodes[target.vehicleC84];
    if(!previousNode.nextCount||previousNode.nextCount>maxUndertrackNext50ET)
        return fail(d,AiDrivingError50IV::Capacity,0x171460u,target.vehicleC84);
    if(std::fegetround()!=FE_TONEAREST)
        return fail(d,AiDrivingError50IV::Arithmetic,0x171430u);
    std::array<float,maxUndertrackNext50ET> scores{};Math f;
    for(unsigned axis=0;axis<3;++axis){f.keep(target.position190[axis]);f.keep(target.forward190[axis]);}
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x1714a0u);
    for(std::uint32_t i=0;i<previousNode.nextCount;++i){
        const auto n=previousNode.next[i];
        if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x1714d4u,n);
        for(unsigned axis=0;axis<3;++axis)f.keep(t.track.nodes[n].position[axis]);
        if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x1714d4u,n);
        const V delta=sub(f,point(t.track.nodes[n].position),point(target.position190));
        const float norm=dot(f,delta,delta);
        //171538..171544 always executes VRSQRT then VMULQ. Retain the
        //same saturated-Q finite convention as native undertrack50ET;
        //a norm that rounded to0 does NOT synthesize a zero direction.
        const float q=norm==0.f?std::numeric_limits<float>::max():f.div(1.f,root(f,norm));
        const V direction=scale(f,delta,q);
        scores[i]=dot(f,direction,point(target.forward190));
        if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x171568u,n);
    }
    branch=t.nodes[previousNode.next[0]].branch68;float best=0.f;
    for(std::uint32_t i=0;i<previousNode.nextCount;++i){
        if(scores[i]<=best)continue;
        best=scores[i];branch=t.nodes[previousNode.next[i]].branch68;
    }
    return true;
}
inline bool predictAttackBranch171430(const TrackNavigation50EU&t,
    const AiObservedRacer50IV&target,std::int32_t&branch,
    AiDrivingDiagnostic50IV&d) noexcept {
    return predictAttackBranch171430Shared50MB(t,target,branch,d);
}
//171320: a completed source return-1 is distinct from a missing owner.
// The caller1719B0 genuinely continues to packing/random on that return.
template<class Navigation,class RouteOwners>
inline bool chooseAttackRoute171320Shared50MB(const Navigation&source_t,
    const UndertrackNode50ET&node,const RouteOwners&owners,
    std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiJoined(source_t,owners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171340u);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    ordinal=-1;
    if(!t.track.count||t.track.count>navigation_consumer_detail50MB::capacity(t))
        return fail(d,AiDrivingError50IV::Owner,0x171340u);
    if(!owners.target1008Present||!*owners.target1008Present||
       !owners.target1008ManagerIndex||!owners.racers||
       *owners.target1008ManagerIndex>=owners.racerCount)
        return fail(d,AiDrivingError50IV::Owner,0x171340u);
    const auto index=*owners.target1008ManagerIndex;
    const auto&target=owners.racers[index];
    if(!target.bound||target.managerIndex!=index)
        return fail(d,AiDrivingError50IV::Owner,0x17134cu,index);
    if(target.vehicle58==1u)return true; //171358 return-1, no target geometry.
    std::int32_t branch=0;
    if(target.vehicle58==0u){
        if(target.vehicleC84>=t.track.count)
            return fail(d,AiDrivingError50IV::Owner,0x171368u,target.vehicleC84);
        if(t.track.nodes[target.vehicleC84].nextCount>=2u){
            if(!predictAttackBranch171430Shared50MB(t,target,branch,d))return false;
        }else{
            if(target.vehicleC80>=t.track.count)
                return fail(d,AiDrivingError50IV::Owner,0x171394u,target.vehicleC80);
            branch=t.nodes[target.vehicleC80].branch68;
        }
    }else{
        if(target.vehicleC80>=t.track.count)
            return fail(d,AiDrivingError50IV::Owner,0x171394u,target.vehicleC80);
        branch=t.nodes[target.vehicleC80].branch68;
    }
    if(node.nextCount>maxUndertrackNext50ET)
        return fail(d,AiDrivingError50IV::Capacity,0x1713a4u);
    for(std::uint32_t i=0;i<node.nextCount;++i){
        const auto n=node.next[i];
        if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x1713b4u,n);
        const auto&candidate=t.nodes[n];
        if(candidate.flags79&1u)continue;
        if(candidate.flags79&0x40u)continue;
        if(candidate.branch68==branch){ordinal=static_cast<std::int32_t>(i);return true;}
    }
    return true;
}
inline bool chooseAttackRoute171320(const TrackNavigation50EU&t,
    const UndertrackNode50ET&node,const AiRouteChoiceOwners50IY&owners,
    std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    return chooseAttackRoute171320Shared50MB(t,node,owners,ordinal,d);
}
//1719B0's exact branch order: record shortest, state1/mode1 attack route,
// packing policy4 shortest, then171750 weighted draw. Only a genuine return-1
// falls through; failed owner/arithmetic recovery never invents a route.
template<class Navigation,class RouteOwners>
inline bool chooseRouteShared50MB(const Navigation&source_t,const UndertrackNode50ET&node,
    AiNavigationState50IV&state,const RouteOwners&owners,
    std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiJoined(source_t,owners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x1719b0u);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    ordinal=-1;
    if(owners.fieldFE4){
        if(!shortestRouteShared50MB(t,node,owners.metadata,ordinal,d))return false;
        if(ordinal!=-1)return true;
    }
    if(owners.aiStateFE8==1u){
        if(!owners.attackMode1050)return fail(d,AiDrivingError50IV::Owner,0x171a04u);
        if(*owners.attackMode1050==1u){
            if(!chooseAttackRoute171320Shared50MB(t,node,owners,ordinal,d))return false;
            if(ordinal!=-1)return true;
        }
    }
    if(owners.packingPolicy1000==4u){
        if(!shortestRouteShared50MB(t,node,owners.metadata,ordinal,d))return false;
        if(ordinal!=-1)return true;
    }
    if(std::fegetround()!=FE_TONEAREST)return fail(d,AiDrivingError50IV::Arithmetic,0x171750u);
    std::array<float,maxUndertrackNext50ET> weights{};
    std::int32_t eligible=0;
    for(std::uint32_t i=0;i<node.nextCount;++i){
        const auto successor=node.next[i];
        if(successor>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x2a2560u,successor);
        const auto flags=t.nodes[successor].flags79;
        if(flags&1u){weights[i]=0.f;continue;}
        if(flags&0x40u){weights[i]=0.f;continue;}
        ++eligible;weights[i]=1.f;
    }
    // Source308030 is unconditional after weights, even if eligible/count0.
    // Commit the one real shared draw before its conversion/arithmetic. Never
    // replace it with modulo selection or filter the zero-weight positions.
    auto&random=owners.sharedRandomState;
    random=random*0x41c64e6du+0x3039u;++state.routeRandomDraws;
    Math f;
    const float drawn=f.integer(static_cast<std::int32_t>(random&0x7fffffffu));
    const float count=f.integer(eligible);
    const float unit=f.mul(ai_speed_detail50IR::word(0x30000000u),drawn);
    const float threshold=f.mul(count,unit);
    float cumulative=0.f;
    for(std::uint32_t i=0;i<node.nextCount;++i){
        const float upper=f.add(cumulative,weights[i]);
        if(threshold<=upper){ordinal=static_cast<std::int32_t>(i);break;}
        cumulative=f.add(cumulative,weights[i]);
    }
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x171838u);
    return true;
}
inline bool chooseRoute(const TrackNavigation50EU&t,const UndertrackNode50ET&node,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50IY&owners,
    std::int32_t&ordinal,AiDrivingDiagnostic50IV&d) noexcept {
    return chooseRouteShared50MB(t,node,state,owners,ordinal,d);
}
template<class Navigation,class RouteOwners>
inline bool nextShared50MB(const Navigation&source_t,std::uint32_t n,AiNavigationState50IV&s,
    const RouteOwners&owners,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiJoined(source_t,owners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171a90u);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x171a90,n);
    const auto&node=t.track.nodes[n];
    if(node.nextCount>maxUndertrackNext50ET)return fail(d,AiDrivingError50IV::Capacity,0x2a2850u,n);
    if(node.nextCount==1u){out=node.next[0];return out<t.track.count||fail(d,AiDrivingError50IV::Owner,0x171ab8,n);}
    //171A90 checks both remembered nodes for each successor in list order.
    for(std::uint32_t i=0;i<node.nextCount;++i)
        if(static_cast<std::int32_t>(node.next[i])==s.choice1080||
           static_cast<std::int32_t>(node.next[i])==s.choice1084){
            out=node.next[i];return out<t.track.count||fail(d,AiDrivingError50IV::Owner,0x171ad8u,n);
        }
    std::int32_t ordinal=-1;
    if(!chooseRouteShared50MB(t,node,s,owners,ordinal,d))return false;
    if(ordinal<0||static_cast<std::uint32_t>(ordinal)>=node.nextCount)
        return fail(d,AiDrivingError50IV::RouteChoice,0x171b28u,n);
    out=node.next[static_cast<std::uint32_t>(ordinal)];
    if(out>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x171b3cu,n);
    if(s.choice1080==-1)s.choice1080=static_cast<std::int32_t>(out);
    else if(s.choice1084==-1)s.choice1084=static_cast<std::int32_t>(out);
    // Both occupied: source still returns the chosen node without replacing.
    ++s.routeSelections;return true;
}
inline bool next(const TrackNavigation50EU&t,std::uint32_t n,AiNavigationState50IV&s,
    const AiRouteChoiceOwners50IY&owners,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    return nextShared50MB(t,n,s,owners,out,d);
}
template<class Navigation>
inline bool previousShared50MB(const Navigation&source_t,std::uint32_t n,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiReady(source_t))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x2a26e0u);

    const auto& t=navigation_consumer_detail50MB::read(source_t);
    if(n>=t.track.count)return fail(d,AiDrivingError50IV::Owner,0x2a26e0,n);
    if(n&&t.nodes[n].branch68==t.nodes[n-1].branch68){out=n-1;return true;}
    if(!n){for(std::uint32_t i=0;i<t.track.count;++i)if(t.track.nodes[i].nextCount&&t.track.nodes[i].next[0]==0){out=i;return true;}}
    else for(std::uint32_t i=n;i>0;--i)for(std::uint32_t k=0;k<t.track.nodes[i].nextCount;++k)
        if(t.track.nodes[i].next[k]==n){out=i;return true;}
    return fail(d,AiDrivingError50IV::Predecessor,0x2a26e0,n);
}
inline bool previous(const TrackNavigation50EU&t,std::uint32_t n,std::uint32_t&out,AiDrivingDiagnostic50IV&d) noexcept {
    return previousShared50MB(t,n,out,d);
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
template<class Navigation,class Metadata,class RouteOwners>
inline bool advanceAiNavigation50IVShared50MB(const Navigation&source_map,const Navigation&source_ai,
    const Metadata&source_metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const RouteOwners&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::pairReady(source_map,source_ai)||!navigation_consumer_detail50MB::aiJoined(source_ai,source_metadata)||!navigation_consumer_detail50MB::aiJoined(source_ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0u);

    const auto& map=navigation_consumer_detail50MB::read(source_map);
    const auto& ai=navigation_consumer_detail50MB::read(source_ai);
    const auto& metadata=navigation_consumer_detail50MB::read(source_metadata);
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
        std::uint32_t successor;if(!nextShared50MB(ai,n,state,routeOwners,successor,d))return false;
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
inline bool advanceAiNavigation50IV(const TrackNavigation50EU&map,const TrackNavigation50EU&ai,
    const AiTrackMetadata50IU&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50IY&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
inline bool advanceAiNavigation50IV(const TrackNavigationView50EU&map,const TrackNavigationView50EU&ai,
    const AiTrackMetadataView50IU&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50MB&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::pairReady(map,ai)||!metadata.matches(ai)||!routeOwners.metadata.matches(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
// TC-CONSUMER-BEGIN 1
inline bool advanceAiNavigation50IV(const TrackNavigationView50TC&map,const TrackNavigationView50TC&ai,
    const AiTrackMetadataView50TC&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50TC&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::pairReady(map,ai)||!metadata.matches(ai)||!routeOwners.metadata.matches(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
// TC-CONSUMER-END 1

// TB-CONSUMER-BEGIN 1
inline bool advanceAiNavigation50IV(const TrackNavigationView50TB&map,const TrackNavigationView50TB&ai,
    const AiTrackMetadataView50TB&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50TB&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::pairReady(map,ai)||!metadata.matches(ai)||!routeOwners.metadata.matches(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
// TB-CONSUMER-END 1

// GB-CONSUMER-BEGIN 1
inline bool advanceAiNavigation50IV(const TrackNavigationView50GB&map,const TrackNavigationView50GB&ai,
    const AiTrackMetadataView50GB&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50GB&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::pairReady(map,ai)||!metadata.matches(ai)||!routeOwners.metadata.matches(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
// GB-CONSUMER-END 1

// MC-CONSUMER-BEGIN 1
inline bool advanceAiNavigation50IV(const TrackNavigationView50MC&map,const TrackNavigationView50MC&ai,
    const AiTrackMetadataView50MC&metadata,const float previousCA0[4],const float local190[4],
    std::uint32_t actualZoneOrdinal48,TrackNavigationProgress50EU&progress,
    std::uint32_t&lapCC8,std::uint8_t&flagsCD8,AiNavigationState50IV&state,
    AiBoostDecisionState50IT&boost,const AiRouteChoiceOwners50MC&routeOwners,
    AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::pairReady(map,ai)||!metadata.matches(ai)||!routeOwners.metadata.matches(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ffe0);
    return advanceAiNavigation50IVShared50MB(map,ai,metadata,previousCA0,local190,actualZoneOrdinal48,progress,lapCC8,flagsCD8,state,boost,routeOwners,d);
}
// MC-CONSUMER-END 1

//171FE0: segment/gate projection is intentionally not centerline interpolation.
template<class Navigation,class RouteOwners>
inline bool aiDesiredPosition50IVShared50MB(const Navigation&source_ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const RouteOwners&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiJoined(source_ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0u);

    const auto& ai=navigation_consumer_detail50MB::read(source_ai);
    using namespace ai_driving_detail50IV;Math f;const bool backwards=horizon<0.f;
    float remaining=std::fabs(horizon);V current=point(local190);std::uint32_t n=backwards?p.vehicleC84:p.reset.vehicleC80;
    V result=out;std::uint32_t visits=0;
    while(remaining>0.f){if(n>=ai.track.count)return fail(d,AiDrivingError50IV::Owner,0x171fe0,n);
        if(++visits>ai.track.count+1u)return fail(d,AiDrivingError50IV::Traversal,0x172344,n);
        const auto&g=ai.nodes[n];const V gate=closest(f,current,g.gate10,g.gate20);
        const V delta=sub(f,current,gate);const float distance=root(f,dot(f,delta,delta));
        if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172138,n);
        if(distance<=remaining){current=point(ai.track.nodes[n].position);
            std::uint32_t chosen;if(!(backwards?previousShared50MB(ai,n,chosen,d):nextShared50MB(ai,n,state,routeOwners,chosen,d)))return false;n=chosen;remaining=f.sub(remaining,distance);
        }else{result=add(f,current,scale(f,normalize(f,sub(f,point(ai.track.nodes[n].position),current)),remaining));remaining=0.f;}
    }
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172344,n);out=result;return true;
}
inline bool aiDesiredPosition50IV(const TrackNavigation50EU&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50IY&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
inline bool aiDesiredPosition50IV(const TrackNavigationView50EU&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50MB&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0);
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
// TC-CONSUMER-BEGIN 2
inline bool aiDesiredPosition50IV(const TrackNavigationView50TC&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50TC&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0);
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
// TC-CONSUMER-END 2

// TB-CONSUMER-BEGIN 2
inline bool aiDesiredPosition50IV(const TrackNavigationView50TB&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50TB&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0);
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
// TB-CONSUMER-END 2

// GB-CONSUMER-BEGIN 2
inline bool aiDesiredPosition50IV(const TrackNavigationView50GB&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50GB&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0);
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
// GB-CONSUMER-END 2

// MC-CONSUMER-BEGIN 2
inline bool aiDesiredPosition50IV(const TrackNavigationView50MC&ai,const TrackNavigationProgress50EU&p,
    AiNavigationState50IV&state,const AiRouteChoiceOwners50MC&routeOwners,
    const float local190[3],float horizon,VisualVector49R&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiJoined(ai,routeOwners.metadata))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x171fe0);
    return aiDesiredPosition50IVShared50MB(ai,p,state,routeOwners,local190,horizon,out,d);
}
// MC-CONSUMER-END 2

//170B50: all manager racers, including the human, retained in original order.
template<class Navigation>
inline bool buildAiRacerLists50IVShared50MB(const Navigation&source_ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiReady(source_ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50u);

    const auto& ai=navigation_consumer_detail50MB::read(source_ai);
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
inline bool buildAiRacerLists50IV(const TrackNavigation50EU&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
inline bool buildAiRacerLists50IV(const TrackNavigationView50EU&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50);
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
// TC-CONSUMER-BEGIN 3
inline bool buildAiRacerLists50IV(const TrackNavigationView50TC&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50);
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
// TC-CONSUMER-END 3

// TB-CONSUMER-BEGIN 3
inline bool buildAiRacerLists50IV(const TrackNavigationView50TB&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50);
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
// TB-CONSUMER-END 3

// GB-CONSUMER-BEGIN 3
inline bool buildAiRacerLists50IV(const TrackNavigationView50GB&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50);
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
// GB-CONSUMER-END 3

// MC-CONSUMER-BEGIN 3
inline bool buildAiRacerLists50IV(const TrackNavigationView50MC&ai,const TrackNavigationProgress50EU&p,
    const AiProfile50IU&profile,const AiObservedRacer50IV*racers,std::size_t count,
    std::uint32_t selfIndex,const float local190[3],const float localForward[3],
    AiRacerLists50IV&out,AiDrivingDiagnostic50IV&d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(ai))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x170b50);
    return buildAiRacerLists50IVShared50MB(ai,p,profile,racers,count,selfIndex,local190,localForward,out,d);
}
// MC-CONSUMER-END 3

//172E60 source steering. Main scene axes are LOCAL 28A530 outputs.
inline bool steerAi50IV(const float direction[3],const float forward[3],float upY,
    float cachedSpeed,float engineDifference,float angularA4,std::uint32_t racer60,
    PlayerReadResult48X&actions,AiDrivingDiagnostic50IV&d) noexcept {
    using namespace ai_driving_detail50IV;SteeringMathObservation50IV f;float desired,current;
    if(!player_finish_detail50IA::directionAngle(direction,desired)||!player_finish_detail50IA::directionAngle(forward,current))return fail(d,AiDrivingError50IV::Arithmetic,0x172df0);
    if(upY<0.f){desired=-desired;current=-current;}
    f.stage=1; // angle wrapping
    if(current>0.f&&desired<0.f)desired=f.add(desired,ai_speed_detail50IR::word(0x40c90fdbu));
    float error=f.sub(current,desired);if(error<ai_speed_detail50IR::word(0xc0490fdbu))error=f.add(error,ai_speed_detail50IR::word(0x40c90fdbu));
    f.stage=2; // speed and engine scaling
    float gain=3.f;const float k=ai_speed_detail50IR::word(0x3f2e8bc1u),speed=f.mulSteeringSpeed(k,cachedSpeed),basis=f.mul(k,engineDifference);
    f.stage=3; // speed-dependent gain
    if(std::fabs(error)>ai_speed_detail50IR::word(0x3e4ccccdu)){
        const float hi=f.add(basis,100.f),lo=f.sub(basis,200.f),offset=f.sub(speed,lo);
        gain=hi==lo?0.f:clamp(f.add(3.f,f.div(f.mul(offset,2.f),f.sub(hi,lo))),3.f,5.f);
    }
    f.stage=4; // racer gain adjustment
    if(racer60==8u||racer60==18u||racer60==23u||racer60==4u)gain=f.mul(gain,1.25f);
    f.stage=5; // live steering and normalization
    float steering=f.sub(f.mul(gain,error),angularA4);const float absolute=std::fabs(steering);if(absolute>1.f)steering=f.div(steering,absolute);
    if(!f.valid){
        const int rounding=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);
        unsigned mxcsrAvailable=0,mxcsr=0;
#if defined(__SSE__)
        mxcsrAvailable=1;mxcsr=_mm_getcsr();
#endif
        const auto bits=SteeringMathObservation50IV::bits;
        std::fprintf(stderr,"AI_STEERING_OPERAND_REJECT pc=00173154 racer=%u stage=%u ordinal=%u operation=%u a=%08x b=%08x result=%08x total_ops=%u direction=%08x,%08x,%08x forward=%08x,%08x,%08x up_y=%08x cached_speed=%08x engine_difference=%08x angular_a4=%08x desired=%08x current=%08x error=%08x gain=%08x speed=%08x basis=%08x steering=%08x absolute=%08x rounding=%d exceptions=%x mxcsr_available=%u mxcsr=%08x\n",
            racer60,f.badStage,f.badOrdinal,f.badOperation,f.aBits,f.bBits,f.resultBits,f.ordinal,
            bits(direction[0]),bits(direction[1]),bits(direction[2]),bits(forward[0]),bits(forward[1]),bits(forward[2]),
            bits(upY),bits(cachedSpeed),bits(engineDifference),bits(angularA4),bits(desired),bits(current),bits(error),bits(gain),bits(speed),bits(basis),bits(steering),bits(absolute),rounding,unsigned(exceptions),mxcsrAvailable,mxcsr);
        return fail(d,AiDrivingError50IV::Arithmetic,0x173154);
    }actions.raw.values[0]=steering;actions.raw.values[1]=0.f;return true;
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
