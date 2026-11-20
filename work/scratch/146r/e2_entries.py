from edlib import edit
edit('src/game/dlc_course_entries50dl.h', [
('''bool addonCourseAllowsMode(unsigned number,AddonCourseMode mode) noexcept;
''', '''bool addonCourseAllowsMode(unsigned number,AddonCourseMode mode) noexcept;
// Launch rule (146r): the add-on course selected right now races only in free
// play (mode34 0) and only in a mode its item allows: subtype 3 = Time Trial,
// subtypes 1/2 = Single Event. Returns the number that races, 0 = the stock
// course the profile holds (the original Course target).
unsigned addonCourseForLaunch(unsigned mode34,unsigned subtype) noexcept;
'''),
('''bool addonCourseLoad(unsigned number,AddonCourseLoad& out);
''', '''bool addonCourseLoad(unsigned number,AddonCourseLoad& out);
// Same rows by the course code ("nv" / "NV"); false for any non-add-on code.
bool addonCourseLoadByCode(const char* code,AddonCourseLoad& out);
'''),
('''// Which race route an add-on course runs. There must be ONE common route; the
// port still selects one of 13 stock-code loaders (track_session_assets50IG rows,
// HostTrack50IM, the isXxTrack admission in terrain_interactive_session50du.cpp),
// so no route admits a non-stock code yet: available=false with the reason. The
// launch of an add-on course is refused with that reason (never a stock route
// under another name).''', '''// Which race route an add-on course runs (146r): the ONE common route
// (HostTrack50IM::Course, track_session_assets50IG::registrySession, the common
// world/trigger/lighting/camera owners that SA also runs). Every member is named
// by the add-on code; no stock course's loader is chosen for it.'''),
])
edit('src/game/dlc_course_entries50dl.cpp', [
('''void applyTestAddonCourse() noexcept {''', '''unsigned addonCourseForLaunch(unsigned mode34,unsigned subtype) noexcept {
    const unsigned number=selectedAddonCourse();
    if(!number||mode34!=0)return 0;
    const auto mode=subtype==3?AddonCourseMode::timeTrial:AddonCourseMode::single;
    return (subtype>=1&&subtype<=3&&addonCourseAllowsMode(number,mode))?number:0u;
}
void applyTestAddonCourse() noexcept {'''),
('''std::string addonCourseLoadLine(const AddonCourseLoad& l){''', '''bool addonCourseLoadByCode(const char* code,AddonCourseLoad& out){
    out=AddonCourseLoad{};
    try{
        if(!code||!code[0]||!code[1]||code[2])return false;
        const auto* c=reg::addonCourseByCode(lowerCode(std::string(code,2)));
        return c&&addonCourseLoad(c->number,out);
    }catch(...){return false;}
}
std::string addonCourseLoadLine(const AddonCourseLoad& l){'''),
('''    // PORT STAND-IN (flagged, CR2 not done): the race load is still one loader per
    // stock code. The common data rules (MA/SA/GA chain: ma_sa_original_world50lm.h,
    // course_archive_receipt50ci.h receipts) read a converted archive, but that
    // chain is entered only for the codes MA/SA/GA and names its members by those
    // codes (scbMember "TRACKS/MA.SCB", track_source_fixture::Track::ma/sa/ga,
    // per-course trigger/ATM/visual-profile fixtures, the lighting storage layout
    // per scene). Until that chain takes the course code as data, an add-on
    // course is refused at launch instead of running a stock course's route.
    return {false,"none","race path still selects one of the 13 stock-code loaders (HostTrack50IM, track_session_assets50IG, isXxTrack admission); no code-independent route yet"};''',
 '''    // 146r: the ONE common route. Stock SA runs the same owners with its stock
    // profile as receipts; every other stock code keeps its own loader for now
    // (PORT STAND-INS, flagged where they are selected).
    return {true,"common",""};'''),
])
