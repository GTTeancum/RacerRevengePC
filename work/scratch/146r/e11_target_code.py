from edlib import edit
edit('tests/native/original_course_race_targets50kx.h', [
('''inline bool commonRouteTarget(const Target& t) noexcept {return t.track==Track::SA||t.track==Track::Course;}''',
 '''inline bool commonRouteTarget(const Target& t) noexcept {return t.track==Track::SA||t.track==Track::Course;}
// Code of a target for log lines: the stock row's code, or "course" for the
// common-route target (its row exists only once the launch is consumed).
inline const char* targetCode(const Target& t) noexcept {return t.assets?t.assets->world->code:"course";}'''),
])
edit('tests/native/original_frontend_session50la.h', [
('''    std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<target->assets->world->code''',
 '''    std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<original_course_targets50KX::targetCode(*target)'''),
])
edit('tests/native/terrain_interactive_loop50du.cpp', [
('''     std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<target->assets->world->code''',
 '''     std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<original_course_targets50KX::targetCode(*target)'''),
])
