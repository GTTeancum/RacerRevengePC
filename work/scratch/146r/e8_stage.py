from edlib import edit
edit('tests/native/original_sa_world_cold_prefix50la.h', [
('''    const auto* selected=original_course_targets50KX::target(launchSelection_);
    if(!selected||selected->track!=rr::platform::HostTrack50IM::SA)return false;''',
 '''    const auto* selected=original_course_targets50KX::target(launchSelection_);
    // ONE common route (146r): stock SA and every registry course hold this world.
    if(!selected||!original_course_targets50KX::commonRouteTarget(*selected))return false;'''),
('''    const auto* target=original_course_targets50KX::target(selected);
    if(!target||target->track!=rr::platform::HostTrack50IM::SA)return false;''',
 '''    const auto* target=original_course_targets50KX::target(selected);
    if(!target||!original_course_targets50KX::commonRouteTarget(*target))return false;'''),
])
edit('tests/native/original_consumed_sa_stage50du.h', [
('''           !launch.matches(*application_.riderProfile,session,rr::platform::HostTrack50IM::SA,racer))return false;''',
 '''           !(launch.matches(*application_.riderProfile,session,rr::platform::HostTrack50IM::SA,racer)||
             launch.matches(*application_.riderProfile,session,rr::platform::HostTrack50IM::Course,racer)))return false;'''),
])
edit('tests/native/original_race_world_join50la.h', [
('''    if(selected->track==rr::platform::HostTrack50IM::SA){''',
 '''    if(original_course_targets50KX::commonRouteTarget(*selected)){ // stock SA or a registry course (146r)'''),
])
edit('tests/native/original_course_race_targets50kx.h', [
('''inline const Target* target(const Selection& s) noexcept {''',
 '''// The targets that run the ONE common route: stock SA (stock profile = receipts)
// and the registry course target. Every other stock target keeps its own loader
// (PORT STAND-INS, to retire onto this route).
inline bool commonRouteTarget(const Target& t) noexcept {return t.track==Track::SA||t.track==Track::Course;}
inline const Target* target(const Selection& s) noexcept {'''),
])
