#pragma once
#include <cmath>
#include <cstdint>

namespace rr::game {
// Bounded original1A46E0 music owner, for category6's exact six requests.
// Music backend transport is separate. No track selection or RNG occurs here.
struct RaceMusicState50IE {
    float timer128=0.0f;
    std::int32_t mode12C=0;
    std::int32_t latch130=-1;
};
enum class RaceMusicPlanStatus50IE : std::uint8_t {
    Play,Suppressed,UnsupportedEvent,InvalidInput
};
struct RaceMusicPlan50IE {
    RaceMusicPlanStatus50IE status=RaceMusicPlanStatus50IE::InvalidInput;
    std::uint32_t song=0,admissionPC=0x1a4900u,playPC=0;
    std::int32_t nextMode12C=0;
    bool loop=false,writeLatchBeforeStop=false;
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
// Source writes before the backend play call: generic1A48A8 first writes130=1,
// then1A4AB0 overwrites130=-1 and12C=0 while stopping.1A4820/1A48C4 writes
// the new12C. The backend start is ordered after these writes and the stop.
inline void beginCategory6MusicPlay50IE(const RaceMusicPlan50IE& p,
    RaceMusicState50IE& s)noexcept {
    if(p.writeLatchBeforeStop)s.latch130=1;
    s.timer128=0.0f;s.latch130=-1;s.mode12C=0;
}
inline void finishCategory6MusicStop50IE(const RaceMusicPlan50IE& p,
    RaceMusicState50IE& s)noexcept {
    s.mode12C=p.nextMode12C;
}
//1A4AF0's relevant no-positive-timer branch observes actual transport activity.
inline void observeCategory6MusicActivity50IE(bool active,RaceMusicState50IE& s)noexcept {
    if(!active)s.mode12C=0;
}
} // namespace rr::game
