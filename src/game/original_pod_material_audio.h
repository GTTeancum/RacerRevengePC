#pragma once
#include "original_ai_aural.h"
#include "original_human_engine_reset50lb.h"
#include "original_shared_rng50iu.h"
#include "race_event_queue50id.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Native rendering may be enabled explicitly without claiming the unresolved
// original IOP RPC16 sample-pitch metadata. This is an owner-wide cold policy,
// selected before the first dispatch and retained for cachedSlope20's lifetime.
enum class OriginalPodMaterialPitchPolicy : std::uint8_t {
    requireOriginalMetadata,nativeDecodedRateUnity4096
};
// Canonical D44 storage, outside racer snapshots. Each successful source
//292AF0/292CB0 append owns a DISTINCT logical sound, including duplicate files.
// These are borrowed native owners, never a replacement PCM/factory callback.
struct OriginalPodMaterialSound {
    platform::LogicalAudioOwner* nativeOwner=nullptr;
    platform::LogicalAudioOwner::Sound* nativeSound=nullptr;
    std::uint32_t authoredKey4=0;
    //232ED0 constructor50. Known only after the actual sample metadata join;
    // a decoded file's rate does not prove RPC16/26C240's returned source pitch.
    std::int32_t basePitch50=4096;
    bool basePitchKnown=false;
    // This is canonical otherwise-unrepresented sound80, initially0. Group44
    // is borrowed from the ONE actual shared Effects110 group owner.
    bool soundInhibited80=false;
    const std::uint32_t* groupState44=nullptr;
};
struct OriginalPodMaterialDriveRow {
    OriginalPodMaterialSound sound{};
    std::int32_t sampleSpeed8=0,minVolumeC=0,minSpeed10=0,maxSpeed14=0,resonant18=0;
    float slope1C=0.f;
    // In native policy this cache and the resulting pitch40/4C values are host
    // quantization policy, not recovered original sample-pitch arithmetic.
    std::int32_t cachedSlope20=-1;
};
// Only the source-backed numeric words of PodRaceAudioManager+50 consumed by
// this bounded material owner. Unread packet words and141C20/global object
// registration are deliberately unrepresented, not implied by setupApplied.
struct OriginalPodMaterialSetup {
    OriginalAiEmitterVector position170{},velocity180{},ancillary1A0{};
    std::uint32_t flags158=0;
    float near1B8=0.f,far1BC=0.f,trigger1DC=0.f,chance1E0=100.f;
};
struct OriginalPodMaterialAudioState {
    std::array<OriginalPodMaterialSound,32> collision{};
    std::array<OriginalPodMaterialDriveRow,20> drive{};
    std::size_t collisionCount=0,driveCount=0;
    std::int32_t activeDriveKey23C=-1,driveMaterial240=-1,driveKey244=-1;
    const OriginalAiEmitterIdentity* vehicle248=nullptr;
    std::uint32_t disabled24C=0;
    float distance1D0=0.f; //291134 stores zero;2C2850 later publishes nearest distance.
    std::uint8_t flags1D4=0;
    OriginalPodMaterialSetup setup{};
    OriginalPodMaterialPitchPolicy pitchPolicy=OriginalPodMaterialPitchPolicy::requireOriginalMetadata;
    bool setupApplied=false,blocked=false,inDispatch=false;
};
inline bool projectOriginalPodMaterialSetup2C21C0(OriginalPodMaterialAudioState& state)noexcept {
    if(state.setupApplied||state.vehicle248||state.collisionCount||state.driveCount||
       state.blocked||state.inDispatch)return false;
    //1A39F0 stores vf0 at manager+60/+70/+90.1A3C98..1A3CA0 writes
    // near0/far450.2C21C0 ->292A90 ->2911E0 copies these same numeric words.
    const OriginalAiEmitterVector vf0{{0.f,0.f,0.f,1.f}};
    state.setup.position170=state.setup.velocity180=state.setup.ancillary1A0=vf0;
    state.setup.flags158=2;state.setup.near1B8=0.f;state.setup.far1BC=450.f;
    state.setup.trigger1DC=20.f;state.setup.chance1E0=100.f;
    state.flags1D4=static_cast<std::uint8_t>(state.flags1D4&~8u);
    state.setupApplied=true;return true;
}
enum class OriginalPodMaterialAudioStatus : std::uint8_t {
    complete,input,owner,arithmetic,sourceSampleMetadata,nativeBackend,blocked,eventQueue
};
struct OriginalPodMaterialAudioReceipt {
    OriginalPodMaterialAudioStatus status=OriginalPodMaterialAudioStatus::input;
    std::uint32_t sourcePC=0;
    std::size_t randomDraws=0,started=0,stopped=0,controlled=0,
        inactiveOrMissing=0,spatialCalls=0;
};
struct OriginalPodMaterialAudioInput {
    std::uint32_t channelArgument=3,vehicle58=1;
    std::int32_t contactMaterialA9=-1;
    float speedRatio70=0.f;
    OriginalAiEmitterVector position{},velocity{};
};
namespace original_pod_material_detail {
using State=OriginalPodMaterialAudioState;
using Row=OriginalPodMaterialSound;
using Status=OriginalPodMaterialAudioStatus;
using Receipt=OriginalPodMaterialAudioReceipt;
using Channel=platform::LogicalAudioOwner;
inline float add(float a,float b)noexcept{volatile float c=a+b;return c;}
inline float sub(float a,float b)noexcept{volatile float c=a-b;return c;}
inline float mul(float a,float b)noexcept{volatile float c=a*b;return c;}
inline float div(float a,float b)noexcept{volatile float c=a/b;return c;}
inline bool scalar(float x)noexcept{return std::isfinite(x)&&(x==0.f||std::fpclassify(x)!=FP_SUBNORMAL);}
inline bool integer(float x,std::int32_t& out)noexcept {
    if(!scalar(x)||x< -2147483648.f||x>=2147483648.f)return false;
    out=static_cast<std::int32_t>(x);return true;
}
inline bool fail(State& s,Receipt& r,Status status,std::uint32_t pc)noexcept {
    s.blocked=true;r.status=status;r.sourcePC=pc;return false;
}
inline bool valid(const Row& row)noexcept {
    // The admitted material lifecycle never writes233A80/multiplier54; it
    // remains the actual232ED0 constructor1. Supporting independent material
    // multipliers requires backend pitch-only/volume-only control operations.
    return row.nativeOwner&&row.nativeSound&&row.groupState44&&row.nativeSound->multiplier54==1.f;
}
inline Row* collision(State& s,std::int32_t key)noexcept {
    return key>0&&static_cast<std::size_t>(key)<=s.collisionCount?&s.collision[static_cast<std::size_t>(key)-1]:nullptr;
}
inline OriginalPodMaterialDriveRow* drive(State& s,std::int32_t key)noexcept {
    return key>0&&static_cast<std::size_t>(key)<=s.driveCount?&s.drive[static_cast<std::size_t>(key)-1]:nullptr;
}
inline bool random(State& s,CoupledNoiseContext49M& shared,std::int32_t maximum,
    std::int32_t& out,Receipt& r,std::uint32_t pc)noexcept {
    if(maximum<0||maximum==std::numeric_limits<std::int32_t>::max())
        return fail(s,r,Status::arithmetic,pc);
    const auto word=drawOriginalSharedRng50IU(shared);++r.randomDraws;
    const auto value=mul(static_cast<float>(maximum+1),mul(0x1p-31f,static_cast<float>(word)));
    if(!integer(value,out)||out<0||out>maximum)return fail(s,r,Status::arithmetic,pc);
    return true;
}
inline bool effectiveBase(State& s,const Row& row,std::int32_t& base,Receipt& r)noexcept {
    if(s.pitchPolicy==OriginalPodMaterialPitchPolicy::nativeDecodedRateUnity4096){
        // Separate host pitch base. Never resolve or overwrite source50,40/4C
        // here, never mark source metadata known, and never draw extra RNG.
        base=4096;return true;
    }
    if(s.pitchPolicy!=OriginalPodMaterialPitchPolicy::requireOriginalMetadata)
        return fail(s,r,Status::input,0x233e90u);
    if(!row.basePitchKnown)return fail(s,r,Status::sourceSampleMetadata,0x233e90u);
    if(row.basePitch50<0||row.basePitch50>65535)return fail(s,r,Status::sourceSampleMetadata,0x26c240u);
    base=row.basePitch50;return true;
}
inline bool controls(State& state,Row& row,std::int32_t volume,std::int32_t pitch,
    Receipt& r,std::uint32_t pc)noexcept {
    std::int32_t base=0;if(!effectiveBase(state,row,base,r))return false;
    if(base==0||pitch<0||pitch>=16384||(pitch==0&&
       state.pitchPolicy!=OriginalPodMaterialPitchPolicy::nativeDecodedRateUnity4096))
        return fail(state,r,Status::nativeBackend,pc);
    // The mixer applies this ratio against the actual decoded PCM rate. Strict
    // mode divides by proven source50; native mode divides by its separate4096
    // policy base. Neither path fabricates original IOP sample metadata.
    // Native zero is explicit silent, cursor-preserving playback. The backend
    // must retain the active token so later source controls can resume it.
    if(row.nativeOwner->controlsSourceVolume(*row.nativeSound,volume,
        div(static_cast<float>(pitch),static_cast<float>(base)),row.nativeSound->angle48)!=Channel::Result::complete)
        return fail(state,r,Status::nativeBackend,pc);
    ++r.controlled;return true;
}
inline bool start233410(State& state,Row& row,Receipt& r)noexcept {
    auto& sound=*row.nativeSound;
    //233410 invokes233E90 before its sample-availability gate. Strict mode
    // requires that join; the explicit native policy makes no source claim.
    std::int32_t base=0;if(!effectiveBase(state,row,base,r))return false;
    if(!sound.available){++r.inactiveOrMissing;return true;}
    if(row.nativeOwner->active(sound))return fail(state,r,Status::nativeBackend,0x233544u);
    if(base==0||sound.pitch40<0||sound.pitch40>=16384||(sound.pitch40==0&&
       state.pitchPolicy!=OriginalPodMaterialPitchPolicy::nativeDecodedRateUnity4096))
        return fail(state,r,Status::nativeBackend,0x233514u);
    if(row.nativeOwner->startSourceVolume(sound,sound.volume38,
        div(static_cast<float>(sound.pitch40),static_cast<float>(base)),
        sound.loop1B0!=0,sound.angle3C)!=Channel::Result::complete)
        return fail(state,r,Status::nativeBackend,0x233544u);
    sound.pitch4C=sound.pitch40;sound.volume44=sound.volume38;sound.angle48=sound.angle3C;
    sound.elapsedBits24=0;sound.word7C=1;++r.started;return true;
}
inline bool stopOne(State& state,Row& row,Receipt& r,std::uint32_t pc)noexcept {
    if(!valid(row))return fail(state,r,Status::owner,pc);
    if(!row.nativeOwner->active(*row.nativeSound))return true;
    if(row.nativeOwner->stop233670(*row.nativeSound)!=Channel::Result::complete)
        return fail(state,r,Status::nativeBackend,pc);
    ++r.stopped;return true;
}
inline bool stopAll2933D0(State& state,Receipt& r)noexcept {
    //2933D0 iterates every constructed row, not only remembered244/23C.
    for(std::size_t i=0;i<state.driveCount;++i)
        if(!stopOne(state,state.drive[i].sound,r,0x293424u))return false;
    state.activeDriveKey23C=-1;return true;
}
inline bool stop2C27D0(State& state,Receipt& r)noexcept {
    if(!stopAll2933D0(state,r))return false;
    state.driveMaterial240=state.driveKey244=-1;return true;
}
inline bool nearest(State& state,const OriginalAiEmitterVector& position,
    const OriginalAiAudioListener* listeners,std::size_t count,
    std::size_t& index,float& distance,Receipt& r)noexcept {
    distance=std::numeric_limits<float>::max();index=0;
    for(std::size_t i=0;i<count;++i){const auto d=original_ai_aural_detail::distance(
        position.v[0],position.v[2],listeners[i].position[0],listeners[i].position[2]);
        if(!scalar(d))return fail(state,r,Status::arithmetic,0x2c28ecu);
        if(d<distance){distance=d;index=i;}}
    return true;
}
inline bool eligible2C2850(State& state,const OriginalAiEmitterVector& position,
    const OriginalAiAudioListener* listeners,std::size_t count,Receipt& r)noexcept {
    std::size_t index=0;float d=0;
    if(!nearest(state,position,listeners,count,index,d,r))return false;
    state.distance1D0=d;state.flags1D4=static_cast<std::uint8_t>(state.flags1D4&~2u);
    if(d<540.f)state.flags1D4|=2u;return true;
}
inline bool spatial104140(State& state,Row* row,const OriginalPodMaterialAudioInput& in,
    float farDistance,const OriginalAiAudioListener* listeners,std::size_t count,
    const OriginalSoundModeFields50KG& mode,bool& returned,Receipt& r)noexcept {
    returned=false;if(!count)return true; //1041CC..1041D4, no sound mutation.
    std::size_t index=0;float d=0;
    if(!nearest(state,in.position,listeners,count,index,d,r))return false;
    const auto& listener=listeners[index];float attenuation=0.f;std::int32_t angle=0;
    returned=d<farDistance;
    if(returned){
        attenuation=d<=0.f?100.f:sub(100.f,mul(100.f,div(d,farDistance)));
        float forward=0.f,target=0.f;
        if(!originalAngle48G(listener.forward[2],listener.forward[0],forward)||
           !originalAngle48G(sub(in.position.v[2],listener.position[2]),sub(in.position.v[0],listener.position[0]),target))
            return fail(state,r,Status::arithmetic,0x102a28u);
        const auto degrees=original_ai_aural_detail::literal(0x42652ee1u);
        std::int32_t f=0,t=0;
        if(!integer(mul(degrees,forward),f)||!integer(mul(degrees,target),t))
            return fail(state,r,Status::arithmetic,0x102a48u);
        angle=t-f;if(angle>180)angle-=359;else if(angle< -180)angle+=359;
    }
    if(!row)return true; //104378 skips233AE0 while preserving saved102950 V0.
    if(!valid(*row))return fail(state,r,Status::owner,0x104390u);
    ++r.spatialCalls;auto& sound=*row->nativeSound;
    //233AE0 reads previous sound.position60. Event position is not copied
    // until292B80/292DA0/292F30 reaches virtual3C later.
    float relative[4]={0.f,0.f,0.f,1.f};
    for(unsigned k=0;k<3;++k){relative[k]=sub(listener.position[k],sound.position60[k]);
        if(!scalar(relative[k]))return fail(state,r,Status::arithmetic,0x233b2cu);}
    if(row->soundInhibited80||*row->groupState44)return true;
    std::int32_t scaled=0;
    if(!integer(mul(static_cast<float>(sound.volume38),attenuation),scaled))
        return fail(state,r,Status::arithmetic,0x233c18u);
    sound.volume44=scaled/100;
    if(!projectOriginalSoundAngle23395050KG(mode,angle,sound.angle48))
        return fail(state,r,Status::input,0x233c68u);
    sound.multiplier58=1.f;
    std::memcpy(sound.relative90.data(),relative,sizeof relative);
    std::int32_t effectivePitch=0;
    if(!integer(mul(sound.multiplier54,mul(static_cast<float>(sound.pitch4C),sound.multiplier58)),effectivePitch))
        return fail(state,r,Status::arithmetic,0x233d18u);
    if(effectivePitch< -1||effectivePitch>=16384)return true; //Ignored233AE0 false.
    if(sound.channel2C<0)return true;
    // A naturally ended/stale channel makes the source backend reject its
    // numeric token.104140 ignores233AE0 V0; never restart an inactive sound.
    if(!row->nativeOwner->active(sound)){++r.inactiveOrMissing;return true;}
    return controls(state,*row,sound.volume44,effectivePitch,r,0x233d78u);
}
inline bool calculate293010(State& state,OriginalPodMaterialDriveRow& row,std::int32_t speed,
    CoupledNoiseContext49M& shared,std::int32_t& pitch,std::int32_t& volume,Receipt& r)noexcept {
    if(!valid(row.sound))return fail(state,r,Status::owner,0x29305cu);
    std::int32_t base=0;if(!effectiveBase(state,row.sound,base,r))return false;
    if(row.sampleSpeed8<=0||row.minSpeed10==row.resonant18||row.maxSpeed14==row.resonant18||
       !scalar(row.slope1C))return fail(state,r,Status::arithmetic,0x293088u);
    if(row.cachedSlope20==-1){
        if(!integer(mul(row.slope1C,static_cast<float>(base/row.sampleSpeed8)),row.cachedSlope20))
            return fail(state,r,Status::arithmetic,0x2930a8u);
    }
    std::int32_t draw=0;
    if(!random(state,shared,row.cachedSlope20,draw,r,0x293108u))return false;
    // EE MULT/SUBU wraps low32; do not turn a source wrap into C++ signed UB.
    const auto delta=static_cast<std::uint32_t>(speed)-static_cast<std::uint32_t>(row.sampleSpeed8);
    const auto offsetBits=delta*static_cast<std::uint32_t>(row.cachedSlope20)-static_cast<std::uint32_t>(draw);
    std::int32_t offset=0;std::memcpy(&offset,&offsetBits,sizeof offset);
    if(!integer(add(static_cast<float>(base),static_cast<float>(offset)),pitch))
        return fail(state,r,Status::arithmetic,0x2931d8u);
    float result=0.f;
    if(speed<=row.resonant18){
        const float gradient=div(sub(100.f,static_cast<float>(row.minVolumeC)),
            sub(static_cast<float>(row.resonant18),static_cast<float>(row.minSpeed10)));
        result=add(static_cast<float>(row.minVolumeC),sub(mul(gradient,static_cast<float>(speed)),mul(gradient,static_cast<float>(row.minSpeed10))));
    }else{
        const float gradient=div(-sub(100.f,static_cast<float>(row.minVolumeC)),
            sub(static_cast<float>(row.maxSpeed14),static_cast<float>(row.resonant18)));
        result=add(static_cast<float>(row.minVolumeC),sub(mul(gradient,static_cast<float>(speed)),mul(gradient,static_cast<float>(row.maxSpeed14))));
    }
    if(!scalar(result))return fail(state,r,Status::arithmetic,0x293298u);
    if(result<0.f)result=0.f;
    result=detail::scalarMin48O(result,100.f);result=detail::scalarMax48O(result,0.f);
    if(!integer(result,volume))return fail(state,r,Status::arithmetic,0x2932e8u);
    return true;
}
inline bool collision292B80(State& state,std::int32_t key,std::int32_t speed,
    const OriginalAiEmitterVector& position,Receipt& r)noexcept {
    auto* row=collision(state,key);if(!row){++r.inactiveOrMissing;return true;}
    if(!valid(*row))return fail(state,r,Status::owner,0x292be8u);
    if(row->nativeOwner->active(*row->nativeSound)){++r.inactiveOrMissing;return true;}
    auto& sound=*row->nativeSound;
    sound.volume38=speed<0?0:speed>100?100:speed; //233830, after active guard.
    std::memcpy(sound.position60.data(),position.v,3*sizeof(float)); //233DB0 preservesW.
    return start233410(state,*row,r);
}
inline bool start292DA0(State& state,std::int32_t key,std::int32_t speed,
    const OriginalAiEmitterVector& position,CoupledNoiseContext49M& shared,Receipt& r)noexcept {
    auto* row=drive(state,key);if(!row){++r.inactiveOrMissing;return true;}
    if(!valid(row->sound))return fail(state,r,Status::owner,0x292e30u);
    if(state.activeDriveKey23C!=key){auto* previous=drive(state,state.activeDriveKey23C);
        if(previous&&!valid(previous->sound))return fail(state,r,Status::owner,0x292e00u);
        if(previous&&previous->sound.nativeOwner->active(*previous->sound.nativeSound)){
            state.activeDriveKey23C=-1; //293330 before233710/233670.
            if(!stopOne(state,previous->sound,r,0x29339cu))return false;
        }}
    auto& sound=*row->sound.nativeSound;
    if(row->sound.nativeOwner->active(sound)){++r.inactiveOrMissing;return true;}
    std::memcpy(sound.position60.data(),position.v,3*sizeof(float));
    std::int32_t pitch=0,volume=0;
    if(!calculate293010(state,*row,speed,shared,pitch,volume,r))return false;
    sound.volume38=volume; //233830 always clamps/writes38.
    if(pitch>=0&&pitch<16384)sound.pitch40=pitch; //233A50 false is ignored.
    state.activeDriveKey23C=key; //Written BEFORE233410, even on source false.
    return start233410(state,row->sound,r);
}
inline bool continue292F30(State& state,std::int32_t key,std::int32_t speed,
    const OriginalAiEmitterVector& position,CoupledNoiseContext49M& shared,Receipt& r)noexcept {
    auto* row=drive(state,key);if(!row){++r.inactiveOrMissing;return true;}
    std::int32_t pitch=0,volume=0;
    // Unlike292DA0, NO active guard precedes293010. Inactive controls still
    // consume pitch RNG and update applicable logical fields; never restart.
    if(!calculate293010(state,*row,speed,shared,pitch,volume,r))return false;
    auto& sound=*row->sound.nativeSound;
    std::memcpy(sound.position60.data(),position.v,3*sizeof(float));
    if(pitch>=0&&pitch<16384&&!row->sound.soundInhibited80&&!*row->sound.groupState44){
        //2339A0 writes4C after the reached native pitch control.
        if(row->sound.nativeOwner->active(sound)&&!controls(state,row->sound,sound.volume44,pitch,r,0x233a10u))return false;
        sound.pitch4C=pitch;
    }
    sound.volume44=volume; //233750 writes44 even when inactive or inhibited.
    if(row->sound.nativeOwner->active(sound)&&!controls(state,row->sound,volume,sound.pitch4C,r,0x23380cu))return false;
    return true;
}
}

//224100 loads vehicle.D40;224130/224148 dispatch ITS virtualA8 with2/3.
// AI vtable3CE130+A8=16E200 jumps179070. Human3CECA0+A8=1A0120
// reaches the SAME179070 at1A0220 for arguments1..3, retaining its own22C.
// Writes and shared RNG commit in source order. On a reached unsupported
// operation, the canonical owner becomes blocked; do not replay the prefix.
namespace original_pod_material_detail {
template<class EnqueueEvent3> inline Receipt dispatch179070(
    State& state,const OriginalAiEmitterIdentity& identity,std::uint8_t& flags22C,
    bool actualOwnerReady,std::uint32_t actualVehicleClass58,
    const OriginalPodMaterialAudioInput& in,const OriginalAiAudioListener* listeners,
    std::size_t listenerCount,const OriginalSoundModeFields50KG& mode,
    CoupledNoiseContext49M& shared,EnqueueEvent3&& enqueueEvent3)noexcept {
    Receipt r;
    if(state.blocked){r.status=Status::blocked;return r;}
    if(!state.setupApplied||state.inDispatch||!actualOwnerReady||
       state.vehicle248!=&identity||!identity.session||
       state.collisionCount>state.collision.size()||state.driveCount>state.drive.size()){
        r.status=Status::owner;return r;}
    if(in.channelArgument<1||in.channelArgument>3||in.vehicle58!=actualVehicleClass58||
       in.contactMaterialA9< -1||in.contactMaterialA9>255||listenerCount>4||
       (listenerCount&&!listeners)||!validOriginalSoundModeFields50KG(mode)||
       std::fegetround()!=FE_TONEAREST)return r;
    for(unsigned k=0;k<4;++k)if(!scalar(in.position.v[k])||!scalar(in.velocity.v[k]))return r;
    for(std::size_t i=0;i<listenerCount;++i)for(unsigned k=0;k<3;++k)
        if(!scalar(listeners[i].position[k])||!scalar(listeners[i].forward[k]))return r;
    state.inDispatch=true;struct Leave{State& s;~Leave(){s.inDispatch=false;}}leave{state};
    auto finish=[&](){r.status=Status::complete;r.sourcePC=0x1792a8u;return r;};
    if(in.channelArgument==3){
        flags22C=static_cast<std::uint8_t>(flags22C&~8u);
        if(!stop2C27D0(state,r))return r;return finish();
    }
    if(in.contactMaterialA9<0)return finish(); //Lazy no-contact skip, evenbit3.
    const bool collisionOnset=in.channelArgument==1||!(flags22C&8u);
    if(in.channelArgument==2)flags22C|=8u;
    if(state.disabled24C)return finish();
    std::int32_t speed=0;
    if(!integer(mul(100.f,in.speedRatio70),speed)){
        fail(state,r,Status::arithmetic,0x17919cu);return r;}
    if(collisionOnset){
        if(!eligible2C2850(state,in.position,listeners,listenerCount,r))return r;
        if(state.flags1D4&2u){
            std::int32_t variant=0;if(!random(state,shared,3,variant,r,0x2c22f4u))return r;
            const auto key=4*in.contactMaterialA9+variant+1;
            bool spatialReturned=false;
            if(!spatial104140(state,collision(state,key),in,add(400.f,static_cast<float>(speed)),
                listeners,listenerCount,mode,spatialReturned,r))return r;
            if(spatialReturned){
                //2C23F8..2C245C: genuine human vehicle248/58, positive A9,
                // then1A39B0(type3,vehicle,0,0,0)->vehicle33C/1A64B0.
                // This precedes292B80 even when its selected sound is absent
                // or already active. Event3 consumes no RNG; source1A52A0
                // later returns without speech, music or controller effects.
                if(in.vehicle58==0u&&in.contactMaterialA9>0&&!enqueueEvent3()){
                    fail(state,r,Status::eventQueue,0x2c2458u);return r;
                }
                if(!collision292B80(state,key,speed<20?20:speed,in.position,r))return r;
            }
        }
    }
    if(in.channelArgument==2){
        //2C2530 performs its own fresh distance/eligibility update after the
        // collision call, including when the collision's tighter bound failed.
        if(!eligible2C2850(state,in.position,listeners,listenerCount,r))return r;
        if(!(state.flags1D4&2u))return finish();
        const bool changed=state.driveMaterial240!=in.contactMaterialA9;
        if(changed){
            if(!stop2C27D0(state,r))return r;
            std::int32_t variant=0;if(!random(state,shared,3,variant,r,0x2c2670u))return r;
            state.driveKey244=4*in.contactMaterialA9+variant+1;
            state.driveMaterial240=in.contactMaterialA9;
        }
        auto* row=drive(state,state.driveKey244);bool spatialReturned=false;
        if(!spatial104140(state,row?&row->sound:nullptr,in,450.f,listeners,listenerCount,mode,spatialReturned,r))return r;
        if(spatialReturned){const auto controlSpeed=speed<50?50:speed;
            if(changed){if(!start292DA0(state,state.driveKey244,controlSpeed,in.position,shared,r))return r;}
            else if(!continue292F30(state,state.driveKey244,controlSpeed,in.position,shared,r))return r;
        }
    }
    return finish();
}
} // namespace original_pod_material_detail

inline OriginalPodMaterialAudioReceipt dispatchOriginalPodMaterialAudio179070(
    OriginalPodMaterialAudioState& state,OriginalAiEngineEmitter& engine,
    const OriginalPodMaterialAudioInput& in,const OriginalAiAudioListener* listeners,
    std::size_t listenerCount,const OriginalSoundModeFields50KG& mode,
    CoupledNoiseContext49M& shared)noexcept {
    return original_pod_material_detail::dispatch179070(state,engine.identity,
        engine.flags22CLow4,engine.constructed&&engine.prepared&&!engine.blocked,1u,
        in,listeners,listenerCount,mode,shared,[]()noexcept{return false;});
}

template<std::size_t N> inline OriginalPodMaterialAudioReceipt dispatchOriginalPodMaterialAudio179070(
    OriginalPodMaterialAudioState& state,OriginalHumanEngineResetOwner50LB& engine,
    const OriginalPodMaterialAudioInput& in,const OriginalAiAudioListener* listeners,
    std::size_t listenerCount,const OriginalSoundModeFields50KG& mode,
    CoupledNoiseContext49M& shared,RaceEventQueue50ID<N>& actualEvents,
    const RaceActorResolver50ID& actualActors,bool actualEventOwner33CBound)noexcept {
    const auto& id=engine.identity();
    return original_pod_material_detail::dispatch179070(state,id,engine.flags22C(),
        engine.bound()&&id.managerOrdinal==0u,0u,
        in,listeners,listenerCount,mode,shared,[&]()noexcept{
            // Native actor handles are manager ordinal+1 in this real FIFO.
            // Resolve the SAME human at the reached enqueue, never an AI
            // proxy, camera-selected racer, or enqueue-time frame clone.
            const auto actor=id.managerOrdinal+1u;RaceActorSnapshot50ID live{};
            if(!actualEventOwner33CBound||!actualActors.read||
               !actualActors.read(actualActors.context,actor,live)||
               live.vehicle58!=0u||live.vehicle60!=id.pod60)return false;
            return appendRaceEvent50ID(actualEvents,{0u,3u,0u,actor,0u},actualActors).helperReturned();
        });
}
} // namespace rr::game
