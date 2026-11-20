from edlib import edit
edit('tests/native/original_course_race_targets50kx.h', [
('''#include "../../src/game/dlc_rider_entries50dl.h"
''', '''#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../src/game/dlc_course_entries50dl.h"
'''),
('''    return track==Track::RA||track==Track::GC||track==Track::TA||track==Track::GA||track==Track::MA||track==Track::SA||track==Track::MB||track==Track::SC||track==Track::TC||track==Track::RC||track==Track::TB||track==Track::GB||track==Track::MC;
}''', '''    return track==Track::RA||track==Track::GC||track==Track::TA||track==Track::GA||track==Track::MA||track==Track::SA||track==Track::MB||track==Track::SC||track==Track::TC||track==Track::RC||track==Track::TB||track==Track::GB||track==Track::MC||track==Track::Course;
}'''),
('''inline const Target* target(const Selection& s) noexcept {
    if(!rr::game::originalCourseSelectionEncoding50KG(s)||
       !rr::game::nativeSingleHumanCourseMode50KG(s))return nullptr;
    for''', '''// PORT EXTENSION, NOT ORIGINAL (146r): the ONE common-route target. A free-play
// launch with an add-on course selected (port-owned, dlc_course_entries50dl.h;
// the profile's packed course/planet keep the last STOCK selection and are still
// checked by the original encodings above) races that course on the common
// route; its session row is built from the registry at consumption
// (track_session_assets50IG::registrySession), never a stock row.
inline const Target courseRouteTarget{Track::Course,nullptr,0xffffffffu,0xffffffffu};
inline const Target* target(const Selection& s) noexcept {
    if(!rr::game::originalCourseSelectionEncoding50KG(s)||
       !rr::game::nativeSingleHumanCourseMode50KG(s))return nullptr;
    if(rr::game::dlc50DL::addonCourseForLaunch(s.mode34,s.subtype))return &courseRouteTarget;
    for'''),
('''        if(!attempted_)return A::hostNotReady;
        const Target* selected=nullptr;
        for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet){selected=&t;break;}''', '''        if(!attempted_)return A::hostNotReady;
        const Target* selected=nullptr;
        // Common route (146r): the registry admitted the package file at startup
        // (listed size/SHA); the route availability is the only other condition.
        const bool addonCourse=rr::game::dlc50DL::addonCourseForLaunch(s.mode34,s.subtype)!=0;
        if(addonCourse){
            if(!rr::game::dlc50DL::addonCourseRoute().available||!directDisc_||
               runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty())return A::courseResourcesUnavailable;
            selected=&courseRouteTarget;
        }
        else for(const auto& t:targets)if(t.course==s.course&&t.planet==s.planet){selected=&t;break;}'''),
('''        if(!rosterPresent)return A::riderResourcesUnavailable;
        for''', '''        if(!rosterPresent)return A::riderResourcesUnavailable;
        if(addonCourse)return A::ready;
        for'''),
('''        nextTrack=target_->track;nextRacer=current.rider;
        target_=nullptr;outgoingGeneration_=0;return true;''', '''        // Common route (146r): latch the add-on code the session row is built from.
        if(target_==&courseRouteTarget){
            rr::game::dlc50DL::AddonCourseLoad load;
            const unsigned number=rr::game::dlc50DL::addonCourseForLaunch(current.mode34,current.subtype);
            if(!number||!rr::game::dlc50DL::addonCourseLoad(number,load)||
               !rr::platform::latchCourseRouteCode50IM(load.codeUpper.c_str()))return false;
            std::cerr<<"course_route_launch key="<<load.key<<" code="<<load.code<<" number="<<number
                <<" route=common mode34="<<current.mode34<<" subtype="<<current.subtype
                <<" profile_course="<<current.course<<" profile_planet="<<current.planet<<'\\n';
        }
        nextTrack=target_->track;nextRacer=current.rider;
        target_=nullptr;outgoingGeneration_=0;return true;'''),
])
