#pragma once
// Original race-announcer callbacks, scalar group state only. This is not a
// mixer patch or authorization to change live mode11258. See callback source map.
#include "gameplay_audio50he.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Stable source OWNER identities, not original dynamically allocated SPU group
// numbers and not host mixer slots. The 2351F0 allocator chooses runtime IDs.
enum class RaceAudioGroup50IC:std::uint8_t {
    Effects110,Engines114,Character11C,Music118,World4D8,Announcer120,Auxiliary124,
    Count,Unrepresented
};
inline constexpr std::size_t raceAudioGroupCount50IC=7;
struct RaceAudioSettings50IC {
    std::int32_t effects11020=0,music11024=0,character11028=0,announcer1102C=0;
};
// Source fresh-configuration reset1865CC/DC/EC/F8. This is an initialization
// value, not evidence of an existing user's settings or an arbitrary reset.
inline constexpr RaceAudioSettings50IC freshOriginalAudioSettings50IC()noexcept {
    return {50,100,80,80};
}
struct RaceAudioGroups50IC {
    // Source volume+38 is signed64 storage. The admitted original setter domain
    // is integer0..100; int32 exactly represents every supported value.
    std::array<std::int32_t,raceAudioGroupCount50IC> volume38{};
    bool initialized=false;
};
struct AnnouncerDuckingState50IC {
    // Event-manager flags F0/F8/108/100/120, in callback traversal order.
    // Deltas F4/FC/10C/104/124 are written on begin and retained on end.
    std::array<std::uint32_t,5> flags{};
    std::array<std::int32_t,5> deltas{};
};
struct RaceAudioGroupWrite50IC {
    RaceAudioGroup50IC group=RaceAudioGroup50IC::Unrepresented;
    std::int32_t volume=0;
    std::uint32_t callSite=0;
};
struct AnnouncerDuckingReceipt50IC {
    bool complete=false;
    std::size_t count=0;
    std::array<RaceAudioGroupWrite50IC,raceAudioGroupCount50IC> writes{};
};
namespace announcer_callbacks50IC_detail {
inline constexpr RaceAudioGroup50IC groups[]={RaceAudioGroup50IC::Effects110,
    RaceAudioGroup50IC::Engines114,RaceAudioGroup50IC::Character11C,
    RaceAudioGroup50IC::Music118,RaceAudioGroup50IC::World4D8};
inline constexpr std::uint32_t beginSites[]={0x1a3e14,0x1a3e44,0x1a3e74,0x1a3ea4,0x1a3ed8};
inline constexpr std::uint32_t endSites[]={0x1a3f5c,0x1a3fb8,0x1a4014,0x1a4070,0x1a40d0};
inline bool volume(std::int32_t v)noexcept{return v>=0&&v<=100;}
inline bool settings(const RaceAudioSettings50IC&s)noexcept {
    return volume(s.effects11020)&&volume(s.music11024)&&
        volume(s.character11028)&&volume(s.announcer1102C);
}
inline bool valid(const RaceAudioGroups50IC&s)noexcept {
    if(!s.initialized)return false;
    for(const auto v:s.volume38)if(!volume(v))return false;
    return true;
}
inline std::int32_t scale(std::int32_t value,float fraction)noexcept {
    volatile float product=static_cast<float>(value)*fraction;
    return static_cast<std::int32_t>(product);
}
inline std::int32_t restored(RaceAudioGroup50IC g,const RaceAudioSettings50IC&s)noexcept {
    switch(g){
        case RaceAudioGroup50IC::Effects110:return scale(s.effects11020,.90f);
        case RaceAudioGroup50IC::Engines114:return scale(s.effects11020,.85f);
        case RaceAudioGroup50IC::Character11C:return scale(s.character11028,.80f);
        case RaceAudioGroup50IC::Music118:return scale(s.music11024,.70f);
        case RaceAudioGroup50IC::World4D8:return scale(s.effects11020,.90f);
        case RaceAudioGroup50IC::Announcer120:return s.announcer1102C;
        case RaceAudioGroup50IC::Auxiliary124:return 100;
        default:return -1;
    }
}
inline void write(RaceAudioGroups50IC&s,AnnouncerDuckingReceipt50IC&r,
    RaceAudioGroup50IC group,std::int32_t value,std::uint32_t callSite)noexcept {
    s.volume38[static_cast<std::size_t>(group)]=value;
    r.writes[r.count++]={group,value,callSite};
}
}
// Original race setup1A78E4..1A7A9C. Call only on actual new group ownership.
// Every group gets its source initial level, including the un-ducked announcer.
inline AnnouncerDuckingReceipt50IC initializeRaceAudioGroups50IC(
    const RaceAudioSettings50IC&settings,RaceAudioGroups50IC&out)noexcept {
    namespace d=announcer_callbacks50IC_detail;
    AnnouncerDuckingReceipt50IC r;
    if(out.initialized||std::fegetround()!=FE_TONEAREST||!d::settings(settings))return r;
    RaceAudioGroups50IC next;
    constexpr RaceAudioGroup50IC order[]={RaceAudioGroup50IC::Effects110,
        RaceAudioGroup50IC::Engines114,RaceAudioGroup50IC::World4D8,
        RaceAudioGroup50IC::Music118,RaceAudioGroup50IC::Character11C,
        RaceAudioGroup50IC::Announcer120,RaceAudioGroup50IC::Auxiliary124};
    constexpr std::uint32_t sites[]={0x1a7928,0x1a7970,0x1a79bc,0x1a7a04,
        0x1a7a4c,0x1a7a84,0x1a7a98};
    for(std::size_t i=0;i<raceAudioGroupCount50IC;++i)
        d::write(next,r,order[i],d::restored(order[i],settings),sites[i]);
    next.initialized=true;out=next;r.complete=true;return r;
}
// Actual begin34=1A39D0 ->1A3DF0. For each not-yet-ducked group, lower the
// current integer volume using source1A40F0 (binary32 *.65 then truncate),
// store old-minus-new delta and flag. Repeating begin is idempotent.
// Execute returned group writes before starting speech or mixing. No RNG.
inline AnnouncerDuckingReceipt50IC beginAnnouncerDucking50IC(
    RaceAudioGroups50IC&groups,AnnouncerDuckingState50IC&state)noexcept {
    namespace d=announcer_callbacks50IC_detail;
    AnnouncerDuckingReceipt50IC r;
    if(std::fegetround()!=FE_TONEAREST||!d::valid(groups))return r;
    auto next=groups;auto flags=state;
    for(std::size_t i=0;i<5;++i){
        if(flags.flags[i])continue;
        const auto old=next.volume38[static_cast<std::size_t>(d::groups[i])];
        const auto lowered=d::scale(old,.65f);
        d::write(next,r,d::groups[i],lowered,d::beginSites[i]);
        flags.deltas[i]=old-lowered;flags.flags[i]=1;
    }
    groups=next;state=flags;r.complete=true;return r;
}
// Actual end38=1A39E0 ->1A3F00. Restore from CURRENT configured settings, not
// by adding saved deltas and not by multiplying the ducked value by1/.65.
// Deliberately leave deltas unchanged. Source stop invokes end before begin;
// normal all-emitter completion invokes end again before setting B0=C0.
inline AnnouncerDuckingReceipt50IC endAnnouncerDucking50IC(
    const RaceAudioSettings50IC&settings,RaceAudioGroups50IC&groups,
    AnnouncerDuckingState50IC&state)noexcept {
    namespace d=announcer_callbacks50IC_detail;
    AnnouncerDuckingReceipt50IC r;
    if(std::fegetround()!=FE_TONEAREST||!d::valid(groups)||!d::settings(settings))return r;
    auto next=groups;auto flags=state;
    for(std::size_t i=0;i<5;++i){
        if(!flags.flags[i])continue;
        d::write(next,r,d::groups[i],d::restored(d::groups[i],settings),d::endSites[i]);
        flags.flags[i]=0;
    }
    groups=next;state=flags;r.complete=true;return r;
}

// Source-constructor bindings for the currently authored Pod01 gameplay
// banks. Unknown inactive categories are explicit, never assigned by filename.
inline RaceAudioGroup50IC gameplayAudioGroup50IC(AudioCue50HE cue)noexcept {
    if(cue.bank==AudioBank50HE::PodEvent&&cue.key>=1&&cue.key<=6)
        return RaceAudioGroup50IC::Effects110;
    if(cue.bank==AudioBank50HE::PodEngine&&cue.key>=1&&cue.key<=3)
        return RaceAudioGroup50IC::Engines114;
    if((cue.bank==AudioBank50HE::PodCurses&&cue.key>=1&&cue.key<=19)||
       (cue.bank==AudioBank50HE::PodTaunts&&cue.key>=1&&cue.key<=12)||
       (cue.bank==AudioBank50HE::PodSpecial&&cue.key>=1&&cue.key<=9))
        return RaceAudioGroup50IC::Character11C;
    return RaceAudioGroup50IC::Unrepresented;
}
} // namespace rr::game
