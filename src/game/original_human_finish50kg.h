#pragma once
#include "ai_cold_owners50iu.h"
#include "lap_timing50id.h"
#include "original_camera_start_effects.h"
#include <cstddef>
#include <cstdint>

namespace original_race_hud_scope50LA { class Registration; }
namespace rr::game {
// Actual B4->3C manager records, in the same human-first B0->44 order used
// by1AF330. These are observations of real racer owners after1AFF50/1B0920;
// no HOST standings, estimated distances or replacement finish bits belong here.
struct OriginalHumanEndRow50KG {
    std::uint32_t class58=0,retirementCD4=0;
    std::uint8_t flagsCD8=0;
    std::int32_t placeCCC=0;
    float gapCBC=0.f;
};
enum class OriginalHumanEndStatus50KG : std::uint8_t {
    complete,owner,callerBoundary,numeric
};
struct OriginalHumanEndResult50KG {
    OriginalHumanEndStatus50KG status=OriginalHumanEndStatus50KG::owner;
    bool ended=false,wroteRace94=false;
    std::uint32_t sourcePC=0;
};
// Source1AF9C0 tail:1B0CB0,1B0A70,then1AFA4C..64. Called only after the
// complete live timing/rank/finish/gap pass, while bit1 set and bit0 clear.
// ALL8 retains its explicitly separate native completion policy; it must not
// use this retail human predicate (zero humans genuinely ends immediately).
inline OriginalHumanEndResult50KG finishOriginalHumanManagerPass50KG(
    LapTimingManager50ID& manager,const OriginalHumanEndRow50KG* rows,
    std::size_t count,std::uint32_t humanCount34,const AiTrackTuning50IU& track,
    float& race94) noexcept {
    using S=OriginalHumanEndStatus50KG;
    OriginalHumanEndResult50KG r;
    if(!rows||!count||count>8||humanCount34>2||humanCount34>count)return r;
    for(std::size_t i=0;i<count;++i)
        if((rows[i].class58==0)!=(i<humanCount34))return r;
    if(!(manager.flags58&2u)||(manager.flags58&1u)){
        r.status=S::callerBoundary;r.sourcePC=0x1afa18u;return r;
    }
    auto flags=manager.flags58;
    bool humanFirst=false,humanSecond=false;
    for(std::size_t i=0;i<humanCount34;++i){
        humanFirst=humanFirst||rows[i].placeCCC==1;
        humanSecond=humanSecond||rows[i].placeCCC==2;
    }
    if(!humanFirst)flags=static_cast<std::uint8_t>(flags|0x40u); //1B0D78
    else if(flags&0x40u){
        for(std::size_t i=humanCount34;i<count;++i){
            if(rows[i].placeCCC!=2&&
               !(humanCount34==2&&humanSecond&&rows[i].placeCCC==3))continue;
            if(!track.bound||!lap_timing_detail50ID::ordinary(track.racePaceCutoffTime80)||
               !lap_timing_detail50ID::ordinary(rows[i].gapCBC)){
                r.status=S::numeric;r.sourcePC=0x1b0ddcu;return r;
            }
            if(!(rows[i].gapCBC<track.racePaceCutoffTime80)){
                flags=static_cast<std::uint8_t>(flags&~0x40u);break; //1B0E14
            }
        }
    }
    //1B0A70 keeps two independent predicates. A retired but unfinished human
    //satisfies allGone, while a finished human clears noneFinished.
    bool allGone=true,noneFinished=true;
    for(std::size_t i=0;i<count;++i){
        if(rows[i].class58!=0)continue;
        if(!(rows[i].flagsCD8&1u)&&rows[i].retirementCD4==0)allGone=false;
        if(rows[i].flagsCD8&1u)noneFinished=false;
    }
    if(!(flags&0x20u)){
        if(noneFinished){race94=6.f;r.wroteRace94=true;} //1B0B20, even before end
        r.ended=allGone&&noneFinished;
    }else{
        r.ended=allGone;
        if(allGone){race94=noneFinished?6.f:4.f;r.wroteRace94=true;} //1B0B5C/68
    }
    manager.flags58=static_cast<std::uint8_t>((flags&~1u)|(r.ended?1u:0u));
    r.status=S::complete;r.sourcePC=0x1afa64u;return r;
}

// Exact fresh race fields1A65F8/1A6660. The caller may borrow an existing
//float owner for94, but must preserve this lifetime independently of manager30.
struct OriginalHumanEndClock50KG {
    float musicDelay98=0.f;
};
enum class OriginalHumanEndClockStatus50KG : std::uint8_t {
    unchanged,advanced,needMusicFade1A4AB0,needFade2AED60,
    needOriginalEndDispatch1A7DD0,callerBoundary,numeric,
    needMusicActivity1A4AD0,needMusicFading1A65A0,needAnnouncerActivity2C4E70
};
struct OriginalHumanEndClockResult50KG {
    OriginalHumanEndClockStatus50KG status=OriginalHumanEndClockStatus50KG::callerBoundary;
    std::uint32_t sourcePC=0;
    std::int32_t fadeMilliseconds=0;
};
// Actual2AED60(...,a2=1,a3=0,t0=0), reached from1A7F24. This queues
//the real manager state7; it neither draws nor acknowledges a completed fade.
//2AE7D0 later calls260320(duration,true,false) and storesA8=4. State4
//clears only when the actual video260300 covered/fading-out query is true.
inline OriginalCameraStartResult requestOriginalHumanEndFade2AED60(
    const OriginalVideoFadeState& video,std::int32_t& managerA8,
    std::int32_t& manager98,std::int32_t requestedMilliseconds) noexcept {
    using S=OriginalCameraStartStatus;
    if(managerA8)return {S::complete,0x2aee60u}; //Source returns0; caller ignores it.
    auto duration=requestedMilliseconds;
    if(video.covered3DC||(video.flags404Low6&16u)){
        if(!video.alpha3E8)return {S::unknownVideoAlpha,0x2aedd4u};
        if(std::fegetround()!=FE_TONEAREST)return {S::arithmetic,0x2aedd8u};
        volatile float alpha=static_cast<float>(*video.alpha3E8);
        volatile float fraction=alpha/128.f;
        volatile float scale=1.f-fraction;
        volatile float adjusted=scale*static_cast<float>(duration);
        if(!lap_timing_detail50ID::ordinary(adjusted)||adjusted<-2147483648.f||
           adjusted>=2147483648.f)return {S::arithmetic,0x2aedecu};
        duration=static_cast<std::int32_t>(adjusted);
        if(!duration)return {S::complete,0x2aee00u};
    }
    managerA8=7;manager98=duration;return {S::complete,0x2aee1cu};
}
// Native typed continuation around genuine1A7DA0. Side-effect cuts are
//acknowledged only by the caller after the ACTUAL music/video owner returns.
//Do not restart begin() while a cut is pending:98/94 stores already occurred.
//At old94<=0 return BEFORE the original end dispatch at1A7DD0: launch1
//calls199770 and race104 virtual+20, then every launch calls1AD230 (HUD text
//animation stops only), followed by race virtual+78. Those independent owners
//have NOT executed. An approved native Results substitute may take this cut
//only after genuine timer/audio/fade completion; it must not label1AD230 as
//retail Results or claim retail profile/save/progression side effects.
class OriginalHumanEndClockCursor50KG final {
    friend class ::original_race_hud_scope50LA::Registration;
    // The same existing cursor, with the actual source read cuts exposed only
    // to its concrete owner. Legacy wrappers below keep their existing API.
    enum class Phase : std::uint8_t { idle,musicActivity,musicFading,music,
        countdownMusic,announcer,fade };
    Phase phase_=Phase::idle;
    OriginalHumanEndClock50KG* clock_=nullptr;
    float* race94_=nullptr;
    std::uint8_t* flags1B0_=nullptr;
    float delta_=0.f;
    bool fadeManagerPresent_=false;
    OriginalHumanEndClockResult50KG continueCountdown() noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        volatile float next=*race94_-delta_;
        if(!lap_timing_detail50ID::ordinary(next))return {S::numeric,0x1a7e9cu,0};
        *race94_=next;
        if(next<=.5f&&!(*flags1B0_&4u)){
            phase_=Phase::countdownMusic;
            return {S::needMusicActivity1A4AD0,0x1a7ec4u,0};
        }
        phase_=Phase::idle;return {S::advanced,0x1a7f54u,0};
    }
    OriginalHumanEndClockResult50KG beginSource1A7DA0(OriginalHumanEndClock50KG& clock,
        float& race94,std::uint8_t& raceFlags1B0,std::uint8_t managerFlags58,
        float delta,bool fadeManagerPresent) noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        if(pending())return {S::callerBoundary,0x1a7da0u,0};
        if(!(managerFlags58&1u))return {S::unchanged,0x1a7dacu,0};
        if(std::fegetround()!=FE_TONEAREST||!lap_timing_detail50ID::ordinary(race94)||
           !lap_timing_detail50ID::ordinary(delta)||delta<0.f)return {S::numeric,0x1a7db4u,0};
        if(race94<=0.f)return {S::needOriginalEndDispatch1A7DD0,0x1a7dd0u,0};
        if(!lap_timing_detail50ID::ordinary(clock.musicDelay98))return {S::numeric,0x1a7e44u,0};
        volatile float next=clock.musicDelay98-delta;
        if(!lap_timing_detail50ID::ordinary(next))return {S::numeric,0x1a7e48u,0};
        clock.musicDelay98=next;
        clock_=&clock;race94_=&race94;flags1B0_=&raceFlags1B0;delta_=delta;
        fadeManagerPresent_=fadeManagerPresent;
        if(next<=0.f){
            phase_=Phase::musicActivity;return {S::needMusicActivity1A4AD0,0x1a7e5cu,0};
        }
        return continueCountdown();
    }
    OriginalHumanEndClockResult50KG afterMusicActivity1A4AD0(bool active) noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        if(phase_==Phase::musicActivity){
            if(!active)return continueCountdown();
            phase_=Phase::musicFading;return {S::needMusicFading1A65A0,0x1a7e6cu,0};
        }
        if(phase_!=Phase::countdownMusic)return {S::callerBoundary,0x1a7ec4u,0};
        if(active){*race94_=.5f;phase_=Phase::idle;return {S::advanced,0x1a7ef0u,0};}
        phase_=Phase::announcer;return {S::needAnnouncerActivity2C4E70,0x1a7ed8u,0};
    }
    OriginalHumanEndClockResult50KG afterMusicFading1A65A0(bool fading) noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        if(phase_!=Phase::musicFading)return {S::callerBoundary,0x1a7e6cu,0};
        if(fading)return continueCountdown();
        phase_=Phase::music;return {S::needMusicFade1A4AB0,0x1a7e84u,1000};
    }
    OriginalHumanEndClockResult50KG afterActualMusicFade1A4AB0() noexcept {
        if(phase_!=Phase::music||!clock_||!race94_||!flags1B0_)
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7e84u,0};
        return continueCountdown();
    }
    OriginalHumanEndClockResult50KG afterAnnouncerActivity2C4E70(bool active) noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        if(phase_!=Phase::announcer)return {S::callerBoundary,0x1a7ed8u,0};
        if(active){*race94_=.5f;phase_=Phase::idle;return {S::advanced,0x1a7ef0u,0};}
        if(fadeManagerPresent_){
            volatile float milliseconds=850.f * *race94_;
            if(!lap_timing_detail50ID::ordinary(milliseconds)||milliseconds<-2147483648.f||
               milliseconds>=2147483648.f)return {S::numeric,0x1a7f1cu,0};
            phase_=Phase::fade;
            return {S::needFade2AED60,0x1a7f24u,static_cast<std::int32_t>(milliseconds)};
        }
        phase_=Phase::idle;return {S::advanced,0x1a7f54u,0};
    }
    OriginalHumanEndClockResult50KG useLegacyObservations(OriginalHumanEndClockResult50KG r,
        bool musicActive,bool musicFading,bool announcerActive)noexcept {
        using S=OriginalHumanEndClockStatus50KG;
        for(;;)switch(r.status){
        case S::needMusicActivity1A4AD0:r=afterMusicActivity1A4AD0(musicActive);break;
        case S::needMusicFading1A65A0:r=afterMusicFading1A65A0(musicFading);break;
        case S::needAnnouncerActivity2C4E70:r=afterAnnouncerActivity2C4E70(announcerActive);break;
        default:return r;
        }
    }
public:
    bool pending()const noexcept{return phase_!=Phase::idle;}
    OriginalHumanEndClockResult50KG begin(OriginalHumanEndClock50KG& clock,
        float& race94,std::uint8_t& raceFlags1B0,std::uint8_t managerFlags58,
        float delta,bool musicActive,bool musicFading,bool announcerActive,
        bool fadeManagerPresent) noexcept {
        return useLegacyObservations(beginSource1A7DA0(clock,race94,raceFlags1B0,
            managerFlags58,delta,fadeManagerPresent),musicActive,musicFading,announcerActive);
    }
    // The synchronous1A4AB0 call has returned. Reread actual activity: do not
    //substitute false just because a fade request was accepted.
    OriginalHumanEndClockResult50KG afterMusicFade(bool musicActive,
        bool announcerActive,bool fadeManagerPresent) noexcept {
        fadeManagerPresent_=fadeManagerPresent;
        return useLegacyObservations(afterActualMusicFade1A4AB0(),musicActive,false,announcerActive);
    }
    OriginalHumanEndClockResult50KG afterVideoFadeRequest() noexcept {
        if(phase_!=Phase::fade||!flags1B0_)
            return {OriginalHumanEndClockStatus50KG::callerBoundary,0x1a7f24u,0};
        *flags1B0_=static_cast<std::uint8_t>(*flags1B0_|4u); //1A7F44 after helper return
        phase_=Phase::idle;return {OriginalHumanEndClockStatus50KG::advanced,0x1a7f54u,0};
    }
};
} // namespace rr::game
