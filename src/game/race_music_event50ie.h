#pragma once
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {
// Bounded original1A46E0 music owner for category6, collision38..43 and practice44/45.
// Music backend transport is separate. No track selection or RNG occurs here.
struct RaceMusicState50IE {
    float timer128=0.0f;
    std::int32_t mode12C=0;
    std::int32_t latch130=-1;
};
enum class RaceMusicPlanStatus50IE : std::uint8_t {
    Play,Suppressed,UnsupportedEvent,InvalidInput,Refresh
};
struct RaceMusicPlan50IE {
    RaceMusicPlanStatus50IE status=RaceMusicPlanStatus50IE::InvalidInput;
    std::uint32_t song=0,admissionPC=0x1a4900u,playPC=0;
    std::int32_t nextMode12C=0;
    bool loop=false,writeLatchBeforeStop=false;
    float timer128=0.0f;
};
inline bool category6MusicSong50IE(std::uint32_t song)noexcept {
    return song==26u||song==27u||song==28u||song==29u||song==33u||song==34u;
}
inline bool validRaceMusicState50IE(const RaceMusicState50IE& s)noexcept {
    return std::isfinite(s.timer128)&&s.timer128>=0.0f&&
        s.mode12C>=0&&s.mode12C<=4&&(s.latch130==-1||s.latch130==1);
}
inline RaceMusicPlan50IE planCategory6Music50IE(const RaceMusicState50IE& s,
    std::uint32_t liveMode11258,std::uint32_t song,std::uint32_t argumentA2)noexcept {
    RaceMusicPlan50IE r;r.song=song;
    if(!category6MusicSong50IE(song)){
        r.status=RaceMusicPlanStatus50IE::UnsupportedEvent;return r;
    }
    // These category6 calls all pass0. Other1A46E0 A2 behavior is deliberately
    // outside this provider, rather than silently claiming the entire manager.
    if(argumentA2||!validRaceMusicState50IE(s))return r;
    const bool finalLap=song==29u||song==33u||song==34u;
    //1A4900: non-race mode admits only44/45; owner4 admits nothing.
    // Owner2 admits26..28 but no29..34. Owner3 admits29..34 but no26..28.
    if(liveMode11258!=1u||s.mode12C==4||
       (s.mode12C==2&&finalLap)||(s.mode12C==3&&!finalLap)){
        r.status=RaceMusicPlanStatus50IE::Suppressed;return r;
    }
    r.status=RaceMusicPlanStatus50IE::Play;
    r.loop=finalLap;r.nextMode12C=finalLap?2:1;
    r.writeLatchBeforeStop=!finalLap;
    r.playPC=finalLap?0x234900u:0x234880u;
    return r;
}
// Actual1AD3C0 startup request. The caller owns the single shared RNG draw;
// this helper neither chooses another song nor consumes random state.
inline bool practiceMusicSong50IE(std::uint32_t song)noexcept {
    return song==44u||song==45u;
}
inline RaceMusicPlan50IE planPracticeMusic50IE(const RaceMusicState50IE& s,
    std::uint32_t liveMode11258,std::uint32_t song,std::uint32_t argumentA2)noexcept {
    RaceMusicPlan50IE r;r.song=song;
    if(!practiceMusicSong50IE(song)){
        r.status=RaceMusicPlanStatus50IE::UnsupportedEvent;return r;
    }
    // Bound to the actual1AD3C0 A2=1 call, not arbitrary generic song requests.
    if(argumentA2!=1u||!validRaceMusicState50IE(s))return r;
    //1A491C..492C admits44/45 in non-race modes without consulting12C.
    // In mode1 the owner2/3 song allowlists omit44/45; owner4 rejects all.
    if(liveMode11258==1u&&s.mode12C>=2){
        r.status=RaceMusicPlanStatus50IE::Suppressed;return r;
    }
    //1A4874..489C: latch1, timer0, stop1A4AB0, mode1, then234900(loop).
    r.status=RaceMusicPlanStatus50IE::Play;r.nextMode12C=1;
    r.loop=true;r.writeLatchBeforeStop=true;r.timer128=0.f;r.playPC=0x234900u;
    return r;
}
// Source writes before the backend play call: generic1A48A8 first writes130=1,
// then1A4AB0 overwrites130=-1 and12C=0 while stopping.1A4820/1A48C4 writes
// the new12C. The backend start is ordered after these writes and the stop.
inline void beginCategory6MusicPlay50IE(const RaceMusicPlan50IE& p,
    RaceMusicState50IE& s)noexcept {
    if(p.writeLatchBeforeStop)s.latch130=1;
    s.timer128=p.timer128;s.latch130=-1;s.mode12C=0;
}
inline void finishCategory6MusicStop50IE(const RaceMusicPlan50IE& p,
    RaceMusicState50IE& s)noexcept {
    s.mode12C=p.nextMode12C;
}
//1A4AF0's relevant no-positive-timer branch observes actual transport activity.
inline void observeCategory6MusicActivity50IE(bool active,RaceMusicState50IE& s)noexcept {
    if(!active)s.mode12C=0;
}
// Source22A400's exact six collision songs. Shared state with category6;
// this is not a second music owner or a reset of category6 state.
inline bool collisionMusicSong50IW(std::uint32_t song) noexcept {
    return song>=38u&&song<=43u;
}
inline RaceMusicPlan50IE planCollisionMusic50IW(const RaceMusicState50IE& s,
    std::uint32_t liveMode11258,std::uint32_t song,std::uint32_t argumentA2) noexcept {
    RaceMusicPlan50IE r;r.song=song;
    if(!collisionMusicSong50IW(song)){
        r.status=RaceMusicPlanStatus50IE::UnsupportedEvent;return r;
    }
    if(argumentA2)return r;
    //1A4900 profile gate precedes any music-state read/write.
    if(liveMode11258!=1u){r.status=RaceMusicPlanStatus50IE::Suppressed;return r;}
    if(!validRaceMusicState50IE(s))return r;
    if(s.mode12C==2||s.mode12C==4){r.status=RaceMusicPlanStatus50IE::Suppressed;return r;}
    //1A47B0..D8 refreshes7 without stopping, replacing or replaying the song.
    r.timer128=7.0f;r.nextMode12C=3;r.loop=true;r.playPC=0x234900u;
    r.status=s.timer128>0.0f?RaceMusicPlanStatus50IE::Refresh:RaceMusicPlanStatus50IE::Play;
    return r;
}
// Bounded234440 prefix for the collision stream started in mode40=4 by
//234900. This is source state and a pending backend request, NOT a gain ramp.
// The original IOP timed-volume consumer is absent from the retained inputs.
struct RaceMusicLoopFade23444050IW {
    bool flag20=false;
    float retirement6C=0.f,duration70=0.f;
};
struct RaceMusicTimedVolume26C8F050IW {
    std::uint32_t sourcePC=0,helperPC=0;
    std::int32_t targetVolume=0,argumentA3=0,durationMilliseconds=0;
};
enum class RaceMusicLoopFadeStatus50IW : std::uint8_t {
    Inactive,NeedTimedVolume26C8F0,InvalidInput
};
struct RaceMusicLoopFadeResult50IW {
    RaceMusicLoopFadeStatus50IW status=RaceMusicLoopFadeStatus50IW::InvalidInput;
    RaceMusicTimedVolume26C8F050IW request{};
};
// The owning adapter must prove the initialized source stream and either its
// absent handle or its actual mode4 playback. It may call this prefix ONCE.
//234480's absent-handle branch returns0;1A4AF0 ignores that return. The active
// branch writes flag20,6C,70 before235060 constructs26C8F0's exact arguments.
// No backend acceptance, command19 enqueue, interpolation, timer advance,
// source234440 return1, or1A4B44 timer0 store is claimed by NeedTimedVolume.
inline RaceMusicLoopFadeResult50IW beginCollisionLoopFade23444050IW(
    bool hasOwnedStreamHandle,RaceMusicLoopFade23444050IW& s) noexcept {
    if(s.flag20||s.retirement6C!=0.f||s.duration70!=0.f)return {};
    if(!hasOwnedStreamHandle)return {RaceMusicLoopFadeStatus50IW::Inactive,{}};
    if(std::fegetround()!=FE_TONEAREST)return {};
    volatile float retirement=0.35f+1.5f; //23455C..578, f32 addition order.
    s.flag20=true;s.retirement6C=retirement;s.duration70=1.5f;
    //235078 converts0 to integer;23509C converts1000*1.5 to1500.
    // Those exact operands need no guessed general conversion or rounding.
    return {RaceMusicLoopFadeStatus50IW::NeedTimedVolume26C8F0,
        {0x2350a8u,0x26c8f0u,0,0,1500}};
}
//1A4AB0's ending caller passes1.0. The admitted stream modes are the SAME
//234900 loop(mode4) and234880 one-shot(mode1) already used by this owner.
// The adapter explicitly supplies native decoded-PCM remaining seconds for
// mode1; it must not claim reconstructed original stream64/68 clock parity.
// Source234598/5AC chooses the short-tail branch before any duration writes.
inline RaceMusicLoopFadeResult50IW beginEndingMusicFade23444050IW(
    bool actualLoop,float actualRemainingSeconds,RaceMusicLoopFade23444050IW& s) noexcept {
    if(s.flag20||s.retirement6C!=0.f||s.duration70!=0.f||
       std::fegetround()!=FE_TONEAREST||!std::isfinite(actualRemainingSeconds)||
       (!actualLoop&&actualRemainingSeconds<=0.f))return {};
    float duration=1.f,retirement=0.f;
    if(actualLoop){volatile float next=0.35f+1.f;retirement=next;}
    else if(actualRemainingSeconds<1.f){
        volatile float tailRetirement=0.9f*actualRemainingSeconds;
        volatile float tailDuration=0.8f*actualRemainingSeconds;
        retirement=tailRetirement;duration=tailDuration; //2346D4..23471C.
    }else{volatile float next=0.25f+1.f;retirement=next;} //234728..234760.
    volatile float milliseconds=1000.f*duration; //23509C before2350A0.
    if(!std::isfinite(retirement)||!std::isfinite(duration)||duration<=0.f||
       retirement<=0.f||milliseconds<0.f||milliseconds>1000.f)return {};
    const auto rounded=static_cast<std::int32_t>(std::nearbyint(milliseconds));
    s.flag20=true;s.retirement6C=retirement;s.duration70=duration;
    return {RaceMusicLoopFadeStatus50IW::NeedTimedVolume26C8F0,
        {0x2350a8u,0x26c8f0u,0,0,rounded}};
}
enum class RaceMusicUpdateStatus50IW : std::uint8_t {Complete,NeedFade234440,InvalidInput};
struct RaceMusicUpdate50IW {
    RaceMusicUpdateStatus50IW status=RaceMusicUpdateStatus50IW::InvalidInput;
    std::uint32_t sourcePC=0;
    float fadeSeconds=0.0f;
};
// Source1A4AF0 timer prefix. Only call once per declared world update. On
//expiry, the caller MUST retain this exact234440(1.5) request and stop until
//the real downstream provider returns (or its proven absent-handle branch).
//timer128's crossed value and the prior latch/mode writes stay committed;
//1A4B44's timer0 store has NOT happened.
inline RaceMusicUpdate50IW beginRaceMusicUpdate50IW(float factor,RaceMusicState50IE& s) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(factor)||factor<0.f||
       !validRaceMusicState50IE(s))return {};
    if(s.timer128>0.f){
        volatile float next=s.timer128-factor;
        if(!std::isfinite(next))return {};
        s.timer128=next;
        if(next<=0.f){
            s.latch130=-1;s.mode12C=0;
            return {RaceMusicUpdateStatus50IW::NeedFade234440,0x1a4b3cu,1.5f};
        }
    }
    // Caller performs the actual transport activity observation at1A4B48.
    return {RaceMusicUpdateStatus50IW::Complete,0x1a4b48u,0.0f};
}
} // namespace rr::game
