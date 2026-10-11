#pragma once
// Scalar source action bodies only. Caller supplies an admitted enter/exit
// event; these functions do not establish hit-backend provenance, recognize
// geometry or invent contacts. An explicit HOST adapter remains HOST-owned.
#include "terrain_query48j.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace rr::game {
enum class TriggerActionStatus50IN:std::uint8_t {Complete,Input,UnsupportedReset};
// Source101170's entire admitted Animation enter callback. The descriptor's
// active90[0] is the first sequence flag; A0/A8 are integer next-sequence and
// next-motion sentinels, NOT floating-point times. Exit has no effects.
inline void triggerAnimation50IN(bool entering,bool&sequenceActive90,
    std::int32_t&nextSequenceA0,std::int32_t&nextMotionA8)noexcept {
    if(entering){sequenceActive90=true;nextSequenceA0=-1;nextMotionA8=-1;}
}
struct LostAiAction50IN {std::uint32_t targetNodeIndex=0;bool reset=false;};
struct LostAiRacer50IN {
    std::uint32_t collider690=0;
    std::uint32_t*nodeC80=nullptr;
};
struct LostAiReceipt50IN {
    TriggerActionStatus50IN status=TriggerActionStatus50IN::Input;
    std::size_t changedRacers=0;
};
// Source1B27C0 traverses race order, matches racer690 to triggering collider,
// writes C80 then optionally2255B0. It does not change C84/C88/C8C or teleport
// on the authored Reset=0 route. All eight GC actions omit Reset (1B5A60
// explicitly defaults it to0). Nonzero Reset remains an explicit unjoined
// owner dependency, rejected before any writes. Source ignores exit callbacks.
inline LostAiReceipt50IN triggerLostAi50IN(bool entering,std::uint32_t collider,
    const LostAiAction50IN&action,const LostAiRacer50IN*racers,std::size_t count,
    std::uint32_t originalAiNodeCount)noexcept {
    LostAiReceipt50IN r;
    if(!entering){r.status=TriggerActionStatus50IN::Complete;return r;}
    if(!collider||count>128||(count&&!racers)||!originalAiNodeCount||
       action.targetNodeIndex>=originalAiNodeCount)return r;
    if(action.reset){r.status=TriggerActionStatus50IN::UnsupportedReset;return r;}
    for(std::size_t i=0;i<count;++i)
        if(racers[i].collider690==collider&&!racers[i].nodeC80)return r;
    for(std::size_t i=0;i<count;++i)if(racers[i].collider690==collider){
        *racers[i].nodeC80=action.targetNodeIndex;++r.changedRacers;
    }
    r.status=TriggerActionStatus50IN::Complete;return r;
}
struct PodSoundSource50IN {
    Vector3_48C position{};
    float minDistance=0.f,maxDistance=0.f;
};
struct PodSoundAdmission50IN {
    TriggerActionStatus50IN status=TriggerActionStatus50IN::Input;
    bool play=false;
    std::size_t listenerIndex=0;
    float nearestDistanceXZ=0.f;
};
namespace trigger_actions50IN_detail {
inline bool finite(const Vector3_48C&v)noexcept {
    return std::isfinite(v.v[0])&&std::isfinite(v.v[1])&&std::isfinite(v.v[2]);
}
inline float mul(float a,float b)noexcept {volatile float c=a*b;return c;}
inline float add(float a,float b)noexcept {volatile float c=a+b;return c;}
inline float sub(float a,float b)noexcept {volatile float c=a-b;return c;}
}
// Source1B5B00->104140->102950. The original listener position comes from
// listener-object290E10, not an invented player/trigger center.104140 computes
// nearest distance in XZ only;102950 requires distance STRICTLY less than Max.
// The caller passed sound=null, so computed attenuation/pan/doppler never reach
// the sound object. MinDistance is retained authoring, not an applied gain.
// On equal distances the first listener wins. No listeners/exit => no play.
// Bounded native normal-finite binary32 path, nearest rounding,no contraction.
// This returns an action decision, not an acknowledgement of actual playback.
inline PodSoundAdmission50IN triggerPodSoundAdmission50IN(bool entering,
    const PodSoundSource50IN&source,const Vector3_48C*listenerPositions,
    std::size_t count)noexcept {
    namespace d=trigger_actions50IN_detail;PodSoundAdmission50IN r;
    if(!entering||!count){r.status=TriggerActionStatus50IN::Complete;return r;}
    if(count>2||!listenerPositions||std::fegetround()!=FE_TONEAREST||
       !d::finite(source.position)||!std::isfinite(source.minDistance)||
       !std::isfinite(source.maxDistance)||source.minDistance<0.f||
       source.maxDistance<source.minDistance)return r;
    for(std::size_t i=0;i<count;++i){
        if(!d::finite(listenerPositions[i]))return r;
        const float dx=std::fabs(d::sub(listenerPositions[i].v[0],source.position.v[0]));
        const float dz=std::fabs(d::sub(listenerPositions[i].v[2],source.position.v[2]));
        const float squared=d::add(d::mul(dx,dx),d::mul(dz,dz));
        if(!std::isfinite(squared))return r;
        const float distance=std::sqrt(squared);
        if(!std::isfinite(distance))return r;
        if(!i||distance<r.nearestDistanceXZ){r.nearestDistanceXZ=distance;r.listenerIndex=i;}
    }
    r.play=r.nearestDistanceXZ<source.maxDistance;
    r.status=TriggerActionStatus50IN::Complete;return r;
}
// Source sound constructor232ED0 defaults volume38=100,pan3C=0,base pitch40
// supplied by clip.1B5B00 invokes233410(flags=2), selecting immediate native
// backend submission; mask0x2 here is NOT a loop flag. GC BIGDOOR has no embedded
// loop. Bind the host voice to source owner RaceAudioGroup50IC::World4D8 before
// play, using the existing shared group layer. Do not pre-multiply group gain.
struct GatePodSoundStart50IN {
    float gain=1.f,pitch=1.f,pan=0.f;
    bool loop=false;
    std::uint32_t sourceStartFlags=2,sourceCallback=0x1b5b00,sourcePlay=0x233410;
};
}
