#pragma once
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include "../../src/assets/dlc_registry50dl.h"
#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../src/game/dlc_course_entries50dl.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

// One free-play Course route for every course and every pod (common rewrite B-C21,
// writer R11; replaces the per-course/per-rider selected routes main-rider-course-sa,
// main-occo20-course-sa, main-dud04/ben02-course-ga, main-alder10/ody11-course-ma,
// main-gasgano07-course-sa and main-mawhonic09-course-ra and their fixed Next counts).
// Main (the profile's own slot 1) -> Rider (whatever racer the profile remembers for free
// play: RR_TEST_PLAYER_RACER when the runner seeded one, else the fresh Rider19) -> Course,
// where the route works the Course screen the way a player does, with the original inputs
// only (0 Left, 1 Right/Next, 2 Up, 3 Down, 4 Confirm):
//   * RR_TEST_COURSE_LAPS=<1..25>: Down to the laps row, Left/Right until it shows that many
//     laps (the original moveLaps 1DF5C4/1DF310 path; Single Race event type);
//   * RR_TEST_COURSE_SUBTYPE=<1|2|3>: the event-type row, Right until that type shows;
//   * RR_TEST_COURSE=<code> (a stock code or an add-on course code, both data: stock codes
//     from the original course descriptor rows, add-on codes from the registry) or
//     RR_TEST_COURSE=<package>/<item> (also read from RR_TEST_ADDON_COURSE_PICK): Up to the
//     course row, Course Next until that course is highlighted;
//   * then Confirm. RR_TEST_ADDON_COURSE_HOLD=1 holds on the Course screen instead.
// The add-on rider test (RR_TEST_ADDON_RIDER=<package>/<item>) presses Rider Next until that
// add-on is highlighted; RR_TEST_ADDON_RIDER_HOLD=1 holds there, RR_TEST_RIDER_HOLD=1 holds on
// the stock rider the route lands on. Which courses are available is the profile's own data
// (availability 11018 from the save); the route never unlocks anything.
// It does not write a profile, select a screen, advance a transition, control a racer, or admit a
// missing destination. The caller owns the explicit native-window unattended opt-in,
// shared wall-time deadline, actual framebuffer capture, and normal teardown.
namespace original_ui_capture_navigation50LA {
// Test only: the racer the seeded save remembers for this run
// (run_windows_takeover.py --player-racer N sets RR_TEST_PLAYER_RACER).
// TournamentRoute never selects it; it only checks that the original Rider
// screen opened on it. Unset means the fresh profile's own Rider19.
inline std::uint32_t expectedEntryRider50LA() {
    const char* value=std::getenv("RR_TEST_PLAYER_RACER");
    if(!value||!*value)return 19;
    std::uint32_t n=0;
    for(const char* p=value;*p;++p){
        if(*p<'0'||*p>'9'||n>26)throw std::runtime_error("RR_TEST_PLAYER_RACER must be an original racer id 1..26");
        n=n*10u+std::uint32_t(*p-'0');
    }
    if(n<1||n>26||n==5||n==6||n==13)throw std::runtime_error("RR_TEST_PLAYER_RACER is not an original racer id");
    return n;
}
class Route final {
    using Snapshot=rr::platform::OriginalUiFrontendSnapshot50KG;
    using Life=rr::game::OriginalUiTransitionLifecycle50KG;
    using Phase=rr::game::OriginalUiTransitionPhase50KG;
    using Aperture=rr::game::OriginalUiApertureState50KG;
    bool enabled_=false,awaitingDispatch_=false;
    std::uint8_t issued_=0;
    std::uint32_t entryRider_=19;
    // PORT EXTENSION, NOT ORIGINAL (DLC add-on rider test).
    bool addon_=false,addonHold_=false,awaitingAddonMove_=false,addonHoldLogged_=false;
    bool stockHold_=false,stockHoldLogged_=false; // test eye check: hold on the landed stock Rider
    const char* addonKey_=nullptr;
    unsigned addonTarget_=0,priorHighlight_=0;
    std::uint32_t addonMoves_=0,priorRider_=19;
    std::uint64_t priorAddonSource_=0;
    // Course screen targets (data: a course code or add-on key, laps, event type).
    struct CourseTarget { bool set=false,addon=false,resolved=false; unsigned number=0; std::string name; };
    CourseTarget course_;
    std::uint32_t lapsTarget_=0,subtypeTarget_=0;
    bool courseHold_=false,courseHoldLogged_=false,awaitingCourseMove_=false,courseConfirmLogged_=false;
    std::uint32_t courseMoves_=0;
    int moveKind_=-1;                      // the original input of the pending Course move
    std::int32_t priorGroup_=-1;
    std::uint32_t priorSubtype_=0,priorLaps_=0,priorSlot_=0;
    unsigned priorCourseHighlight_=0;
    std::uint64_t priorCourseRenders_=0;
    static constexpr int proceed=100;      // not an original input: Course targets reached

    static void require(bool condition,const char* message) {
        if(!condition)throw std::runtime_error(message);
    }
    static unsigned addonNumber(const char* key) noexcept {
        if(!key)return 0;
        for(std::size_t i=0;i<rr::game::dlc50DL::addonRiderCount();++i)
            if(const auto* pod=rr::game::dlc50DL::addonRiderAt(i);pod&&pod->key==key)return pod->number;
        return 0;
    }
    static std::string lower(std::string s){for(char& c:s)if(c>='A'&&c<='Z')c=char(c-'A'+'a');return s;}
    // The course named by RR_TEST_COURSE: a stock descriptor row (0..12, original table
    // order, the number the Course screen's selectedCourse44 holds) or an admitted add-on
    // course (registry number >= 32, the highlighted add-on entry). Data lookups only.
    void resolveCourse() {
        if(!course_.set||course_.resolved)return;
        const std::string want=lower(course_.name);
        if(want.find('/')!=std::string::npos){
            for(std::size_t i=0;i<rr::game::dlc50DL::addonCourseCount();++i)
                if(const auto* c=rr::game::dlc50DL::addonCourseAt(i);c&&c->key==want){course_.addon=true;course_.number=c->number;}
        }else{
            for(unsigned n=0;n<32u;++n){rr::assets::dlc50DL::CourseRow row;
                if(!rr::assets::dlc50DL::courseRow(n,row)||row.addon)break;
                if(row.code==want){course_.number=n;course_.resolved=true;return;}}
            for(std::size_t i=0;i<rr::game::dlc50DL::addonCourseCount();++i)
                if(const auto* c=rr::game::dlc50DL::addonCourseAt(i);c&&c->code==want){course_.addon=true;course_.number=c->number;}
        }
        require(course_.addon&&course_.number!=0,
            "RR_TEST_COURSE is neither a stock course code nor an admitted add-on course (check RR_DLC_ROOT and the dlc_course lines)");
        course_.resolved=true;
    }
    bool courseHighlighted(const Snapshot& s)const noexcept {
        if(!course_.set)return true;
        const auto addon=rr::game::dlc50DL::highlightedAddonCourse();
        return course_.addon?addon==course_.number:(addon==0&&s.course.selectedCourse44==course_.number);
    }
    bool lapsReady(const Snapshot& s)const noexcept {return !lapsTarget_||s.course.laps14EC==lapsTarget_;}
    bool subtypeReady(const Snapshot& s)const noexcept {return !subtypeTarget_||s.course.subtypeSlot14F4==subtypeTarget_;}
    bool courseTargetsReady(const Snapshot& s)const noexcept {
        return lapsReady(s)&&subtypeReady(s)&&courseHighlighted(s)&&(!courseMoves_||s.course.controllerGroup==1);
    }
    // Row move toward group g: Down (3) or Up (2).
    static int towards(std::int32_t group,std::int32_t g)noexcept {return group<g?3:2;}
    int arm(const Snapshot& s,int input) {
        priorGroup_=s.course.controllerGroup;priorSubtype_=s.course.subtypeSlot14F4;priorLaps_=s.course.laps14EC;
        priorSlot_=s.course.courseSlot14E4;priorCourseHighlight_=rr::game::dlc50DL::highlightedAddonCourse();
        priorCourseRenders_=s.course.renders;moveKind_=input;awaitingCourseMove_=awaitingDispatch_=true;return input;
    }
    // Course stage: -1 wait, an original input (0..3), or proceed. The order is laps (on the
    // Single Race type; the laps row is the entrants row in Practice and absent in Time
    // Trial), then the event type, then the course, then back to the course row.
    int pollCourse(const Snapshot& s,bool courseCaptured) {
        resolveCourse();
        if((course_.set||lapsTarget_||subtypeTarget_)&&!courseMoves_)
            require(s.course.mode34==0,"Course route targets require the free-play Course screen");
        if(courseMoves_&&s.course.renders<=priorCourseRenders_)return -1;
        if(courseTargetsReady(s))return proceed;
        require(!courseCaptured,"Course route captured the Course screen before its targets");
        require(courseMoves_<64u+13u+rr::game::dlc50DL::addonCourseCount(),
            "Course route targets were not reached within the bounded number of original moves");
        const auto group=s.course.controllerGroup;
        const auto subtype=s.course.subtypeSlot14F4;
        if(!lapsReady(s)){
            if(subtype!=1){if(group!=2)return arm(s,towards(group,2));return arm(s,1);}   // Single Race first
            if(group!=3)return arm(s,towards(group,3));
            return arm(s,s.course.laps14EC>lapsTarget_?0:1);
        }
        if(!subtypeReady(s)){if(group!=2)return arm(s,towards(group,2));return arm(s,1);}
        if(group!=1)return arm(s,towards(group,1));
        return arm(s,1);                                                                    // Course Next
    }
    static bool same(rr::game::OriginalUiTransitionRoute50KG a,
        rr::game::OriginalUiTransitionRoute50KG b) noexcept {
        return a.screen==b.screen&&a.page==b.page;
    }
    static bool open(const rr::game::OriginalUiApertureSnapshot50KG& a) noexcept {
        return a.bound&&a.inputEnabled268&&a.state==Aperture::open;
    }
    // Rider stage of the add-on test: -1 wait, 1 Rider Next, 2 go on to Confirm.
    int pollAddonRider(const Snapshot& s,bool riderCaptured,bool accepted,
        std::uint64_t source,std::uint64_t publication) {
        if(!addonTarget_){
            addonTarget_=addonNumber(addonKey_);
            require(addonTarget_!=0,"RR_TEST_ADDON_RIDER is not an admitted add-on pod (check RR_DLC_ROOT and the dlc_item lines)");
            require(!addonMoves_&&s.rider.rider4C4==entryRider_,
                "add-on rider route requires the profile's remembered Rider entry");
        }
        if(!accepted||!source||!publication||source<=priorAddonSource_)return -1;
        const auto highlighted=rr::game::dlc50DL::highlightedAddonRider();
        if(highlighted!=addonTarget_){
            require(!riderCaptured,"add-on rider route captured the Rider screen before its add-on rider");
            require(addonMoves_<26u+rr::game::dlc50DL::addonRiderCount(),
                "add-on rider was not reached within the stock roster plus the add-on count");
            priorRider_=s.rider.rider4C4;priorHighlight_=highlighted;priorAddonSource_=source;
            awaitingAddonMove_=awaitingDispatch_=true;return 1;
        }
        if(!riderCaptured)return -1;
        if(addonHold_){
            if(!addonHoldLogged_){addonHoldLogged_=true;
                std::cerr<<"addon_rider_route hold key="<<addonKey_<<" number="<<addonTarget_<<" moves="<<addonMoves_
                    <<" anchor_rider="<<s.rider.rider4C4<<" (no Confirm; the run ends at its wall budget)\n";}
            return -1;
        }
        return 2;
    }
public:
    explicit Route(bool enabled):enabled_(enabled){
        if(!enabled_)return;
        entryRider_=expectedEntryRider50LA();
        const char* key=std::getenv("RR_TEST_ADDON_RIDER");
        addon_=key&&*key;addonKey_=addon_?key:nullptr;
        const char* hold=std::getenv("RR_TEST_ADDON_RIDER_HOLD");
        addonHold_=addon_&&hold&&std::strcmp(hold,"1")==0;
        const char* stockHold=std::getenv("RR_TEST_RIDER_HOLD");
        stockHold_=!addon_&&stockHold&&std::strcmp(stockHold,"1")==0;
        const char* course=std::getenv("RR_TEST_COURSE");
        if(!course||!*course)course=std::getenv("RR_TEST_ADDON_COURSE_PICK");
        if(course&&*course){course_.set=true;course_.name=course;}
        const char* courseHold=std::getenv("RR_TEST_ADDON_COURSE_HOLD");
        courseHold_=course_.set&&courseHold&&std::strcmp(courseHold,"1")==0;
        if(const char* subtype=std::getenv("RR_TEST_COURSE_SUBTYPE");subtype&&*subtype){
            if(subtype[0]<'1'||subtype[0]>'3'||subtype[1])
                throw std::runtime_error("RR_TEST_COURSE_SUBTYPE must be 1 (Single Race), 2 or 3 (Time Trial)");
            subtypeTarget_=std::uint32_t(subtype[0]-'0');
        }
        if(const char* laps=std::getenv("RR_TEST_COURSE_LAPS");laps&&*laps){
            std::uint32_t n=0;
            for(const char* p=laps;*p;++p){
                if(*p<'0'||*p>'9'||n>25)throw std::runtime_error("RR_TEST_COURSE_LAPS must be 1..25 (the original Duration range)");
                n=n*10u+std::uint32_t(*p-'0');
            }
            if(n<1||n>25)throw std::runtime_error("RR_TEST_COURSE_LAPS must be 1..25 (the original Duration range)");
            lapsTarget_=n;
        }
    }
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(issued_+addonMoves_+courseMoves_);}
    // Dispatch completion is not proof of a loaded race or a captured track.
    bool actionsDispatched()const noexcept{return issued_==3&&!awaitingDispatch_;}

    // Use this SAME predicate before accepting an automatic menu screenshot.
    // For Rider the caller must additionally certify its actual accepted source
    // publication/model geometry; a first unattached entry frame is not enough.
    static bool settled(const Snapshot& s) noexcept {
        const auto& t=s.transition;
        if(!s.prepared||!s.active||s.faulted||s.quitRequested||!s.frameSubmitted||
           s.deferredQueue||!t.bound||t.initialRestartPending||t.applicationRequestQueued||
           t.lifecycle!=Life::idle||t.phase!=Phase::none||s.route.page!=1||
           !same(s.route,t.current)||!same(s.route,t.requested))return false;
        if(s.route.screen==4)return s.main.bound&&!s.main.faulted&&!s.main.quitRequested&&
            s.main.mainPageShown&&s.main.inputContainerShown&&s.main.framePublished&&open(s.main.aperture);
        if(s.route.screen==5)return s.rider.bound&&!s.rider.faulted&&!s.rider.quitRequested&&
            s.rider.pageShown&&s.rider.inputContainerShown&&s.rider.framePublished&&
            s.rider.selectionAttached&&!s.rider.blocked4E0&&!s.rider.bypass4E4&&
            !s.rider.deferred4EC&&s.rider.panelY4DC==0.0f&&s.rider.bankGeneration&&
            s.rider.registryGeneration&&open(s.rider.aperture);
        if(s.route.screen==6)return s.course.bound&&!s.course.faulted&&!s.course.quitRequested&&
            s.course.pageShown&&s.course.inputContainerShown&&s.course.framePublished&&
            !s.course.popupEnabled438&&!s.course.deferred14D0&&open(s.course.aperture);
        return false;
    }

    // Gate requested captures too: intermediate refresh frames never satisfy a stage receipt.
    bool captureAllowed(const Snapshot& s,bool sourceAccepted=false,
        std::uint64_t source=0,std::uint64_t publication=0)const noexcept {
        // A published model can still be in its authored entrance animation.
        // Wait for actual character readiness for the screenshot, without
        // skipping animation, advancing time, or changing original inputs.
        if(s.route.screen==5&&!s.rider.selectedCharacterReady)return false;
        // Add-on test: only the settled frame with the requested add-on highlighted.
        if(addon_&&s.route.screen==5)return addonTarget_&&!awaitingDispatch_&&issued_==1&&
            rr::game::dlc50DL::highlightedAddonRider()==addonTarget_&&settled(s)&&
            sourceAccepted&&source>priorAddonSource_&&publication;
        // Course: only the settled frame with every requested target shown.
        if(s.route.screen==6&&(course_.set||lapsTarget_||subtypeTarget_))
            return (!course_.set||course_.resolved)&&!awaitingDispatch_&&issued_==2&&courseTargetsReady(s)&&settled(s)&&
                (!courseMoves_||s.course.renders>priorCourseRenders_);
        return true;
    }

    // Call before the next update, using the previous completed frame. Capture
    // booleans mean successful file publication of the settled matching screen
    // in this frontend lifetime. courseReady is a host prerequisite check only;
    // the actual original Confirm dispatch retains all destination admission.
    // Return -1 while waiting, original Confirm4, or a one-shot original move. A rejected or
    // changed route throws rather than retrying or choosing a substitute.
    int poll(const Snapshot& s,std::int32_t mainGroup,std::int32_t mainSlot,
        bool mainCaptured,bool riderCaptured,bool courseCaptured,
        bool riderSourceAccepted,bool courseReady,std::uint64_t riderSource=0,
        std::uint64_t riderPublication=0) {
        if(!enabled_)return -1;
        require(!awaitingDispatch_,"unattended UI dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"unattended original UI became unavailable");
        if(issued_==3)return -1;
        const auto expectedScreen=static_cast<std::uint8_t>(4+issued_);
        const bool expected=s.route.screen==expectedScreen&&s.route.page==1&&s.switches==issued_;
        const bool outgoing=issued_&&s.route.screen==expectedScreen-1&&s.route.page==1&&s.switches==issued_-1;
        require(expected||outgoing,"unattended UI left the bounded Main/Rider/Course route");
        if(!expected||!settled(s))return -1;
        if(issued_==0){
            require(mainGroup==1&&mainSlot==1,"unattended Main requires its group1/slot1 selection (the runner seeds slot 1)");
            if(!mainCaptured)return -1;
        }else if(issued_==1){
            require(mainCaptured,"unattended Rider arrived without the completed Main screenshot");
            if(addon_){
                const int action=pollAddonRider(s,riderCaptured,riderSourceAccepted,riderSource,riderPublication);
                if(action!=2)return action;
            }else{
                require(s.rider.rider4C4==entryRider_,
                    "unattended Rider did not open on the profile's remembered free-play racer");
            }
            if(!riderCaptured||!riderSourceAccepted)return -1;
            if(stockHold_){
                if(!stockHoldLogged_){stockHoldLogged_=true;
                    std::cerr<<"rider_route hold rider="<<s.rider.rider4C4<<" slot="<<s.rider.slot4C8
                        <<" (no Confirm; the run ends at its wall budget)\n";}
                return -1;
            }
        }else{
            require(mainCaptured&&riderCaptured,
                "unattended Course arrived without the completed Main/Rider screenshots");
            const int action=pollCourse(s,courseCaptured);
            if(action!=proceed)return action;
            if(!courseCaptured)return -1;
            if(courseHold_){
                if(!courseHoldLogged_){courseHoldLogged_=true;
                    std::cerr<<"addon_course_route hold key="<<course_.name<<" number="<<course_.number<<" moves="<<courseMoves_
                        <<" anchor_slot="<<s.course.courseSlot14E4<<" anchor_course="<<s.course.selectedCourse44
                        <<" (no Confirm; the run ends at its wall budget)\n";}
                return -1;
            }
            if(!courseConfirmLogged_){courseConfirmLogged_=true;
                if(course_.addon)std::cerr<<"addon_course_route confirm key="<<course_.name<<" number="<<course_.number<<" moves="<<courseMoves_
                    <<" anchor_slot="<<s.course.courseSlot14E4<<" anchor_course="<<s.course.selectedCourse44
                    <<" subtype="<<s.course.subtypeSlot14F4<<'\n';
                std::cerr<<"course_route confirm course="<<(course_.set?course_.name:std::string("-"))<<" number="<<course_.number
                    <<" addon="<<(course_.addon?1:0)<<" selected_course="<<s.course.selectedCourse44<<" slot="<<s.course.courseSlot14E4
                    <<" subtype="<<s.course.subtypeSlot14F4<<" laps="<<s.course.laps14EC<<" entrants="<<s.course.entrants14F0
                    <<" rider="<<entryRider_<<" moves="<<courseMoves_<<'\n';}
            require(courseReady,"unattended Course race prerequisites are unavailable");
        }
        ++issued_;awaitingDispatch_=true;return 4;
    }

    // Call immediately after originalMain.input(action), BEFORE update/render.
    // No successful dispatch is inferred from the host having issued an event.
    void validateDispatch(const Snapshot& s) {
        if(awaitingCourseMove_){
            const auto& t=s.transition;
            require(enabled_&&awaitingDispatch_&&issued_==2&&!s.faulted&&!s.quitRequested&&
                s.route.screen==6&&s.route.page==1&&s.switches==2&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "Course route move changed the route");
            const auto highlighted=rr::game::dlc50DL::highlightedAddonCourse();
            const bool changed=s.course.controllerGroup!=priorGroup_||s.course.subtypeSlot14F4!=priorSubtype_||
                s.course.laps14EC!=priorLaps_||s.course.courseSlot14E4!=priorSlot_||highlighted!=priorCourseHighlight_;
            require(changed,"Course route original move changed neither the row, the event type, the laps nor the course");
            ++courseMoves_;awaitingCourseMove_=awaitingDispatch_=false;
            std::cerr<<"course_route move="<<courseMoves_<<" input="<<moveKind_<<" row="<<s.course.controllerGroup
                <<" subtype="<<s.course.subtypeSlot14F4<<" laps="<<s.course.laps14EC<<" slot="<<s.course.courseSlot14E4
                <<" course="<<s.course.selectedCourse44<<" addon="<<highlighted<<'\n';
            return;
        }
        if(awaitingAddonMove_){
            const auto& t=s.transition;
            require(enabled_&&addon_&&awaitingDispatch_&&issued_==1&&!s.faulted&&!s.quitRequested&&
                s.route.screen==5&&s.route.page==1&&s.switches==1&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "add-on rider route Rider Next changed the route");
            const auto highlighted=rr::game::dlc50DL::highlightedAddonRider();
            // Either the original stock move committed, or the port-owned add-on slot moved.
            const bool stockMove=s.rider.rider4C4!=priorRider_&&s.rider.slot4C8>0&&s.rider.selectionAttached&&
                s.rider.deferred4EC==3&&!s.rider.framePublished&&highlighted==0;
            const bool addonMove=highlighted!=priorHighlight_&&highlighted!=0&&s.rider.rider4C4==priorRider_;
            require(stockMove||addonMove,"add-on rider route Rider Next moved neither a stock nor an add-on rider");
            ++addonMoves_;awaitingAddonMove_=awaitingDispatch_=false;
            std::cerr<<"addon_rider_route move="<<addonMoves_<<" rider="<<s.rider.rider4C4<<" slot="<<s.rider.slot4C8
                <<" addon="<<highlighted<<'\n';
            return;
        }
        require(enabled_&&awaitingDispatch_&&issued_>=1&&issued_<=3,
            "unattended UI has no pending dispatch receipt");
        const auto source=static_cast<std::uint8_t>(3+issued_);
        const auto target=static_cast<std::uint8_t>(issued_<3?source+1:source);
        const auto& t=s.transition;
        require(!s.faulted&&!s.quitRequested&&s.route.screen==source&&s.route.page==1&&
            s.switches==issued_-1&&t.bound&&same(t.current,s.route)&&
            t.requested.screen==target&&t.requested.page==1&&t.phase==Phase::none&&
            t.lifecycle==(issued_<3?Life::requestScreenExit:Life::requestCourseExit)&&
            !t.applicationRequestQueued&&!t.initialRestartPending,
            "unattended original Confirm did not accept its exact destination");
        awaitingDispatch_=false;
    }
};

// Unattended fresh-profile Tournament route: Main slot3 (original mode2) ->
// Rider19 Confirm -> Tournament Continue -> Course (first tournament course,
// TA) Confirm. Emits only original Main Next3 and Confirm4 events; every
// destination is admitted by the actual frontend, never selected here.
// Test only (eyecheck R2-R5, DLC career; the runner sets these):
//   RR_TEST_ADDON_RIDER=<package>/<item>: on the Rider screen press Rider Next (1)
//     until that add-on is highlighted, then Confirm (the add-on Tournament
//     Confirm itself also needs RR_TEST_ADDON_TOURNAMENT=1, read by the Rider screen);
//   RR_TEST_TOURNAMENT_HOLD=1: hold on the Tournament screen (no Continue);
//   RR_TEST_TOURNAMENT_GARAGE=1: on the Tournament screen press the original Garage
//     input (event 5, 1F241C) and hold on the Garage once it has settled.
// A hold idles until the run's wall budget.
class TournamentRoute final {
    using Snapshot=rr::platform::OriginalUiFrontendSnapshot50KG;
    using Life=rr::game::OriginalUiTransitionLifecycle50KG;
    using Phase=rr::game::OriginalUiTransitionPhase50KG;
    using Aperture=rr::game::OriginalUiApertureState50KG;
    bool enabled_=false,awaitingDispatch_=false,awaitingMainMove_=false,courseLogged_=false;
    std::uint8_t mainMoves_=0,confirms_=0,firstSlot_=0;
    // Test only (see above).
    const char* addonKey_=nullptr;std::uint32_t addonTarget_=0,addonMoves_=0;bool awaitingRiderMove_=false;
    bool holdTournament_=false,garage_=false,garagePressed_=false,holdLogged_=false;
    static bool envOn(const char* name)noexcept{const char* v=std::getenv(name);return v&&v[0]=='1'&&!v[1];}
    static bool garageSettled(const Snapshot& s)noexcept{
        const auto& a=s.garageAperture;
        return s.prepared&&s.active&&!s.faulted&&!s.quitRequested&&s.frameSubmitted&&!s.deferredQueue&&
            s.route.screen==13&&s.route.page==1&&s.garageLoaded&&s.garageRootReturned&&s.garageTimingCommitted&&
            s.garageTargetPublished&&a.bound&&a.inputEnabled268&&a.state==Aperture::open&&
            s.garageManagerLifecycle==0&&s.garageManagerTransition==0;
    }
    static void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
    static bool same(rr::game::OriginalUiTransitionRoute50KG a,rr::game::OriginalUiTransitionRoute50KG b)noexcept{
        return a.screen==b.screen&&a.page==b.page;
    }
    static bool idle(const Snapshot& s)noexcept{
        const auto& t=s.transition;
        return s.prepared&&s.active&&!s.faulted&&!s.quitRequested&&s.frameSubmitted&&!s.deferredQueue&&
            t.bound&&!t.initialRestartPending&&!t.applicationRequestQueued&&t.lifecycle==Life::idle&&
            t.phase==Phase::none&&s.route.page==1&&same(s.route,t.current)&&same(s.route,t.requested);
    }
    static bool settled(const Snapshot& s)noexcept{
        if(s.route.screen==9){
            const auto& m=s.tournament;
            return idle(s)&&m.bound&&!m.faulted&&!m.quitRequested&&m.pageShown&&m.inputContainerShown&&
                m.framePublished&&m.aperture.bound&&m.aperture.inputEnabled268&&m.aperture.state==Aperture::open;
        }
        return Route::settled(s);
    }
    // Screen expected before each Confirm: Main, Rider, Tournament, Course.
    static constexpr std::uint8_t screens_[4]{4,5,9,6};
public:
    static bool settledPublic(const Snapshot& s)noexcept{return settled(s);}
    explicit TournamentRoute(bool enabled)noexcept:enabled_(enabled){
        if(!enabled_)return;
        if(const char* key=std::getenv("RR_TEST_ADDON_RIDER");key&&*key)addonKey_=key;
        holdTournament_=envOn("RR_TEST_TOURNAMENT_HOLD");garage_=envOn("RR_TEST_TOURNAMENT_GARAGE");
    }
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(mainMoves_+confirms_+addonMoves_+(garagePressed_?1u:0u));}
    bool actionsDispatched()const noexcept{return confirms_==4&&!awaitingDispatch_;}
    // Return -1 while waiting, Main Next3, or Confirm4.
    int poll(const Snapshot& s,std::int32_t mainGroup,std::int32_t mainSlot,bool courseReady){
        if(!enabled_)return -1;
        require(!awaitingDispatch_,"tournament route dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"tournament route original UI became unavailable");
        if(confirms_==4)return -1;
        // Test only: the Garage entered from the Tournament screen; hold there.
        if(garagePressed_){
            require(s.route.screen==13||s.route.screen==9,"tournament route Garage press left Tournament/Garage");
            if(garageSettled(s)&&!holdLogged_){holdLogged_=true;
                std::fprintf(stderr,"tournament_route hold screen=13 from=tournament (no further input; the run ends at its wall budget)\n");}
            return -1;
        }
        const auto screen=screens_[confirms_];
        const bool expected=s.route.screen==screen&&s.route.page==1&&s.switches==confirms_;
        const bool outgoing=confirms_&&s.route.screen==screens_[confirms_-1]&&s.switches==confirms_-1u;
        require(expected||outgoing,"tournament route left the bounded Main/Rider/Tournament/Course route");
        if(!expected||!settled(s))return -1;
        if(confirms_==0){
            // Main opens on the profile's saved slot (6E0, 1 when fresh).
            if(!mainMoves_&&!firstSlot_)firstSlot_=std::uint8_t(mainSlot);
            require(mainGroup==1&&firstSlot_>=1&&firstSlot_<=3&&mainSlot==firstSlot_+mainMoves_,"tournament route Main selection diverged");
            if(mainSlot<3){awaitingMainMove_=awaitingDispatch_=true;return 3;}
        }else if(confirms_==1){
            // Rider opens on the profile's remembered tournament racer (6FC):
            // 19 when fresh, or the seeded --player-racer. Never moved here.
            if(!addonMoves_)require(s.rider.rider4C4==expectedEntryRider50LA(),
                "tournament route Rider did not open on the profile's remembered racer");
            if(addonKey_){
                // Test only: Rider Next until the add-on is highlighted (P1 carousel).
                if(!addonTarget_){
                    for(std::size_t i=0;i<rr::assets::dlc50DL::addonPodCount();++i)
                        if(const auto* pod=rr::assets::dlc50DL::addonPodAt(i);pod&&pod->key==addonKey_)addonTarget_=pod->number;
                    require(addonTarget_!=0,"RR_TEST_ADDON_RIDER is not an admitted add-on pod (check RR_DLC_ROOT)");
                }
                if(rr::game::dlc50DL::highlightedAddonRider()!=addonTarget_){
                    require(addonMoves_<26u+rr::game::dlc50DL::addonRiderCount(),"tournament route add-on rider not reached");
                    awaitingRiderMove_=awaitingDispatch_=true;return 1;
                }
                std::fprintf(stderr,"tournament_route addon_rider key=%s number=%u moves=%u confirm\n",addonKey_,addonTarget_,addonMoves_);
            }else if(!s.rider.selectedCharacterReady)return -1;
        }else if(confirms_==2&&(holdTournament_||garage_)){
            // Test only: hold on the Tournament screen, or press its Garage input once.
            if(garage_){std::fprintf(stderr,"tournament_route garage press screen=9\n");awaitingDispatch_=true;return 5;}
            if(!holdLogged_){holdLogged_=true;
                std::fprintf(stderr,"tournament_route hold screen=9 (no Continue; the run ends at its wall budget)\n");}
            return -1;
        }else if(confirms_==3){
            // Whatever course the profile's career progress selects (the
            // mode2 Course entry places it); this route never moves it.
            require(s.course.mode34==2,"tournament route Course left tournament mode");
            if(!courseReady)return -1;
            if(!courseLogged_){courseLogged_=true;
                std::fprintf(stderr,"tournament_route course planet=%u course=%u\n",
                    unsigned(s.course.selectedPlanet40),unsigned(s.course.selectedCourse44));}
        }
        awaitingDispatch_=true;return 4;
    }
    void validateDispatch(const Snapshot& s){
        require(enabled_&&awaitingDispatch_,"tournament route has no pending dispatch receipt");
        const auto& t=s.transition;
        if(awaitingMainMove_){
            require(!s.faulted&&s.route.screen==4&&same(t.current,s.route)&&same(t.requested,s.route),
                "tournament route Main Next changed the route");
            ++mainMoves_;awaitingMainMove_=awaitingDispatch_=false;return;
        }
        if(awaitingRiderMove_){
            require(!s.faulted&&s.route.screen==5,"tournament route Rider Next changed the route");
            ++addonMoves_;awaitingRiderMove_=awaitingDispatch_=false;return;
        }
        if(garage_&&confirms_==2&&!garagePressed_){
            require(!s.faulted&&!s.quitRequested&&s.route.screen==9,"tournament route Garage press faulted the Tournament screen");
            garagePressed_=true;awaitingDispatch_=false;return;
        }
        const auto source=screens_[confirms_];
        // Course launch keeps Course as its requested route (as Route above).
        const bool launch=confirms_==3;
        require(!s.faulted&&!s.quitRequested&&s.route.screen==source&&t.bound&&same(t.current,s.route)&&
            (launch?same(t.requested,s.route)&&t.lifecycle==Life::requestCourseExit:
                !same(t.requested,s.route)&&t.lifecycle==Life::requestScreenExit)&&t.phase==Phase::none,
            "tournament route original Confirm did not accept a destination");
        ++confirms_;awaitingDispatch_=false;
    }
};
// PORT EXTENSION, NOT ORIGINAL (DLC Hall eye check, route main-options-hall-ta):
// Main -> Options (Main slot 4) -> Hall of Fame (Options slot 4) -> the Hall's
// first page, which must be TA (course 0, planet 0). Then it holds: no further
// input, the settled Hall frame is captured once, the run idles until its wall
// budget. Emits only original Main/Options Down (3), Hall Right (1, only if the
// Hall did not open on TA) and Confirm (4) events;
// every destination is admitted by the actual frontend, never selected here.
class HallRoute final {
    using Snapshot=rr::platform::OriginalUiFrontendSnapshot50KG;
    using Life=rr::game::OriginalUiTransitionLifecycle50KG;
    using Phase=rr::game::OriginalUiTransitionPhase50KG;
    using Aperture=rr::game::OriginalUiApertureState50KG;
    static constexpr std::uint8_t screens_[3]{4,12,11};
    bool enabled_=false,awaitingDispatch_=false,awaitingMove_=false,awaitingHallMove_=false,holdLogged_=false;
    std::uint8_t moves_=0,confirms_=0,hallMoves_=0;
    std::uint32_t courseAtMove_=0;
    std::int32_t slotAtMove_=0;
    static void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
    static bool same(rr::game::OriginalUiTransitionRoute50KG a,rr::game::OriginalUiTransitionRoute50KG b)noexcept{
        return a.screen==b.screen&&a.page==b.page;
    }
    static bool open(const rr::game::OriginalUiApertureSnapshot50KG& a)noexcept{
        return a.bound&&a.inputEnabled268&&a.state==Aperture::open;
    }
public:
    static bool settled(const Snapshot& s)noexcept{
        const auto& t=s.transition;
        if(!s.prepared||!s.active||s.faulted||s.quitRequested||!s.frameSubmitted||s.deferredQueue||!t.bound||
           t.initialRestartPending||t.applicationRequestQueued||t.lifecycle!=Life::idle||t.phase!=Phase::none||
           s.route.page!=1||!same(s.route,t.current)||!same(s.route,t.requested))return false;
        if(s.route.screen==4)return s.main.bound&&!s.main.faulted&&!s.main.quitRequested&&
            s.main.mainPageShown&&s.main.inputContainerShown&&s.main.framePublished&&open(s.main.aperture);
        if(s.route.screen==12)return s.options.bound&&!s.options.faulted&&!s.options.quitRequested&&
            s.options.pageShown&&s.options.inputContainerShown&&s.options.framePublished&&open(s.options.aperture);
        if(s.route.screen==11)return s.hall.bound&&!s.hall.faulted&&!s.hall.quitRequested&&
            s.hall.pageShown&&s.hall.inputContainerShown&&s.hall.framePublished&&open(s.hall.aperture);
        return false;
    }
    explicit HallRoute(bool enabled)noexcept:enabled_(enabled){}
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(moves_+confirms_+hallMoves_);}
    // The Hall stage frame: TA, settled, after both Confirms.
    bool captureAllowed(const Snapshot& s)const noexcept{
        return s.route.screen!=11||(confirms_==2&&!awaitingDispatch_&&settled(s)&&
            s.hall.courseIndex==0&&s.hall.planetIndex==0);
    }
    // -1 wait/hold, 3 Down, 4 Confirm.
    int poll(const Snapshot& s,std::int32_t group,std::int32_t slot,bool hallCaptured){
        if(!enabled_)return -1;
        require(!awaitingDispatch_,"Hall route dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"Hall route original UI became unavailable");
        const auto screen=screens_[confirms_<2?confirms_:2];
        const bool expected=s.route.screen==screen&&s.route.page==1&&s.switches==confirms_;
        const bool outgoing=confirms_&&s.route.screen==screens_[confirms_-1]&&s.switches==confirms_-1u;
        require(expected||outgoing,"Hall route left the bounded Main/Options/Hall route");
        if(!expected||!settled(s))return -1;
        if(confirms_==2){
            if(s.hall.courseIndex!=0){
                // Not opened on TA: step the Hall's course cycle (original Right, 1) until TA.
                require(hallMoves_<13,"Hall route: TA (course 0) not reached within the 13 Hall courses");
                courseAtMove_=s.hall.courseIndex;awaitingHallMove_=awaitingDispatch_=true;return 1;
            }
            require(s.hall.planetIndex==0,"Hall route: TA (course 0) is not on planet 0");
            if(!hallCaptured)return -1;
            if(!holdLogged_){holdLogged_=true;
                std::cerr<<"hall_route hold screen=11 course="<<s.hall.courseIndex<<" planet="<<s.hall.planetIndex
                    <<" (no further input; the run ends at its wall budget)\n";}
            return -1;
        }
        // Main and Options: walk Down to slot 4, then Confirm.
        require((confirms_==0?group==1:group>0)&&slot>=1&&slot<=6&&moves_<12,"Hall route menu selection diverged");
        if(slot!=4){slotAtMove_=slot;awaitingMove_=awaitingDispatch_=true;return 3;}
        awaitingDispatch_=true;return 4;
    }
    void validateDispatch(const Snapshot& s,std::int32_t group,std::int32_t slot){
        require(enabled_&&awaitingDispatch_,"Hall route has no pending dispatch receipt");
        const auto& t=s.transition;
        if(awaitingHallMove_){
            require(!s.faulted&&s.route.screen==11&&same(t.current,s.route)&&same(t.requested,s.route)&&
                s.hall.courseIndex!=courseAtMove_,"Hall route Right did not change the Hall course");
            ++hallMoves_;awaitingHallMove_=awaitingDispatch_=false;return;
        }
        if(awaitingMove_){
            require(!s.faulted&&s.route.screen==screens_[confirms_]&&same(t.current,s.route)&&same(t.requested,s.route)&&
                group>0&&slot!=slotAtMove_,"Hall route Down did not move the menu selection");
            ++moves_;awaitingMove_=awaitingDispatch_=false;return;
        }
        require(!s.faulted&&!s.quitRequested&&s.route.screen==screens_[confirms_]&&t.bound&&same(t.current,s.route)&&
            !same(t.requested,s.route)&&t.requested.screen==screens_[confirms_+1]&&
            t.lifecycle==Life::requestScreenExit&&t.phase==Phase::none,
            "Hall route original Confirm did not accept its exact destination");
        ++confirms_;awaitingDispatch_=false;
        std::cerr<<"hall_route confirm="<<unsigned(confirms_)<<" target_screen="<<unsigned(t.requested.screen)<<'\n';
    }
};
// The same unattended tournament run AFTER a race: the 1AB5BC request opened
// Hall11, Win7, Unlock10 or Garage13 as this frontend's first screen. Each
// Hall/Win/Unlock page takes Cross (4) and goes wherever the frontend admits
// (Win/Unlock/Garage, or an Unlock re-show); once in the Garage, Back (event
// 6, 1FE158) requests Tournament 9/1, whose Continue (4) opens Course 6/1,
// whose Confirm (4) launches the next tournament race. Every destination is
// the frontend's own; this route only presses and waits for the screen.
class TournamentReturnRoute final {
    using Snapshot=rr::platform::OriginalUiFrontendSnapshot50KG;
    using Life=rr::game::OriginalUiTransitionLifecycle50KG;
    using Phase=rr::game::OriginalUiTransitionPhase50KG;
    using Aperture=rr::game::OriginalUiApertureState50KG;
    bool enabled_=false,awaitingDispatch_=false,waitingChange_=false,ending_=false;
    // Test only (eyecheck R5): RR_TEST_GARAGE_HOLD=1 holds on the race-return Garage (no Back).
    bool garageHold_=false,garageHoldLogged_=false;
    std::uint8_t actions_=0,garageActions_=0,leadActions_=0;
    std::uint64_t switchesAtAction_=0,pollsSinceAction_=0;
    std::uint32_t activationsAtAction_=0;
    std::uint8_t screenAtAction_=0;
    static void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
    static bool same(rr::game::OriginalUiTransitionRoute50KG a,rr::game::OriginalUiTransitionRoute50KG b)noexcept{
        return a.screen==b.screen&&a.page==b.page;
    }
    static bool lead(std::uint8_t screen)noexcept{return screen==11||screen==7||screen==10;}
    static bool garageSettled(const Snapshot& s)noexcept{
        const auto& a=s.garageAperture;
        return s.prepared&&s.active&&!s.faulted&&!s.quitRequested&&s.frameSubmitted&&!s.deferredQueue&&
            s.route.screen==13&&s.route.page==1&&s.garageLoaded&&s.garageRootReturned&&s.garageTimingCommitted&&
            s.garageTargetPublished&&a.bound&&a.inputEnabled268&&a.state==Aperture::open&&
            // The Page's own opening slide (manager lifecycle 4, transition 1)
            // must have finished: the source ignores Back until 16C/170 are 0.
            s.garageManagerLifecycle==0&&s.garageManagerTransition==0;
    }
    // Hall/Win/Unlock: idle manager, page and input container shown, a
    // published frame and an open aperture accepting input.
    static bool leadSettled(const Snapshot& s)noexcept{
        const auto& t=s.transition;
        if(!(s.prepared&&s.active&&!s.faulted&&!s.quitRequested&&s.frameSubmitted&&!s.deferredQueue&&
             t.bound&&!t.applicationRequestQueued&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
             s.route.page==1&&same(s.route,t.current)&&same(s.route,t.requested)))return false;
        if(s.route.screen==11){const auto& h=s.hall;
            return h.bound&&!h.faulted&&!h.quitRequested&&h.pageShown&&h.inputContainerShown&&h.framePublished&&
                !h.confirmProposalPending&&h.aperture.bound&&h.aperture.inputEnabled268&&h.aperture.state==Aperture::open;}
        const bool bound=s.route.screen==7?s.winBound:s.unlockBound;
        return bound&&s.screenPageShown&&s.screenInputShown&&s.screenFramePublished&&
            s.screenAperture.bound&&s.screenAperture.inputEnabled268&&s.screenAperture.state==Aperture::open;
    }
    // Screen expected before each Garage-stage action: Garage, Tournament, Course.
    static constexpr std::uint8_t screens_[3]{13,9,6};
public:
    static constexpr std::uint8_t maximumLeadActions=8;
    explicit TournamentReturnRoute(bool enabled)noexcept:enabled_(enabled){
        const char* v=enabled_?std::getenv("RR_TEST_GARAGE_HOLD"):nullptr;garageHold_=v&&v[0]=='1'&&!v[1];
    }
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return actions_;}
    bool actionsDispatched()const noexcept{return garageActions_==3&&!awaitingDispatch_;}
    // Hall Cross after the last tournament course (order 12, 102CC set, 1st
    // place) is the original 1E40A0 end scene (17DE10 App,0x28,3,0), not a
    // page screen request; the port has no end-scene receiver yet and keeps
    // the Hall open. That is the route's end: there is no Garage after it.
    void reachEnding(const Snapshot& s){
        require(enabled_&&!awaitingDispatch_&&garageActions_==0&&s.route.screen==11,
            "tournament return route ending outside the Hall");
        std::fprintf(stderr,"tournament_return_route ending source_pc=1e40a0 screen=11; end scene not native, stopping\n");
        ending_=true;waitingChange_=false;
    }
    // Return -1 while waiting, Back6 in the Garage, else Cross/Confirm4.
    int poll(const Snapshot& s,bool courseReady){
        if(!enabled_||ending_)return -1;
        require(!awaitingDispatch_,"tournament return route dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"tournament return route original UI became unavailable");
        if(garageActions_==3)return -1;
        if(waitingChange_){
            // The pressed screen must leave (switch count) or, for an Unlock
            // re-show, activate its page again.
            const bool changed=s.switches!=switchesAtAction_||
                (screenAtAction_==10&&s.route.screen==10&&s.unlockActivations!=activationsAtAction_);
            if(!changed){
                // An accepted press always changes screen or re-shows; a press
                // that changed nothing for ~10 s is reported and repeated.
                if(lead(screenAtAction_)&&++pollsSinceAction_==600){
                    std::fprintf(stderr,"tournament_return_route no_change screen=%u lifecycle=%u; repeating\n",
                        unsigned(s.route.screen),unsigned(s.transition.lifecycle));
                    waitingChange_=false;
                }
                return -1;
            }
            waitingChange_=false;
            std::fprintf(stderr,"tournament_return_route arrived screen=%u page=%u switches=%llu\n",
                unsigned(s.route.screen),unsigned(s.route.page),static_cast<unsigned long long>(s.switches));
        }
        const auto screen=s.route.screen;
        if(lead(screen)){
            require(garageActions_==0,"tournament return route returned to Hall/Win/Unlock after the Garage");
            require(leadActions_<maximumLeadActions,"tournament return route: too many Hall/Win/Unlock presses");
            if(!leadSettled(s))return -1;
            if(screen==10)std::fprintf(stderr,"tournament_return_route unlock pod=%d first_pending=%d activations=%u grants=%llu\n",
                s.unlockPod2D40,s.unlockFirstPending,s.unlockActivations,static_cast<unsigned long long>(s.unlockGrants));
            if(screen==7)std::fprintf(stderr,"tournament_return_route win circuit=%u place=%u career=%u\n",
                s.winCircuit,s.winPlace,s.winCareer);
            if(screen==11)std::fprintf(stderr,"tournament_return_route hall mode=%u course=%u planet=%u\n",
                s.hall.entryMode34,s.hall.courseIndex,s.hall.planetIndex);
            ++leadActions_;awaitingDispatch_=true;return 4;
        }
        if(garageActions_&&screen!=screens_[garageActions_]&&screen==screens_[garageActions_-1])return -1;
        require(screen==screens_[garageActions_],"tournament return route left the bounded Hall/Win/Unlock/Garage/Tournament/Course route");
        if(garageActions_==0){if(!garageSettled(s))return -1;
            if(garageHold_){if(!garageHoldLogged_){garageHoldLogged_=true;
                std::fprintf(stderr,"tournament_return_route hold screen=13 from=race_return (no Back; the run ends at its wall budget)\n");}
                return -1;}
            awaitingDispatch_=true;return 6;}
        if(!TournamentRoute::settledPublic(s))return -1;
        if(garageActions_==2){
            require(s.course.mode34==2,"tournament return route Course left tournament mode");
            if(!courseReady)return -1;
        }
        awaitingDispatch_=true;return 4;
    }
    void validateDispatch(const Snapshot& s){
        require(enabled_&&awaitingDispatch_,"tournament return route has no pending dispatch receipt");
        require(!s.faulted&&!s.quitRequested,"tournament return route press faulted the original UI");
        ++actions_;awaitingDispatch_=false;
        if(lead(s.route.screen)&&garageActions_==0){
            const auto& t=s.transition;
            std::fprintf(stderr,"tournament_return_route cross screen=%u lifecycle=%u requested=%u/%u\n",
                unsigned(s.route.screen),unsigned(t.lifecycle),unsigned(t.requested.screen),unsigned(t.requested.page));
            waitingChange_=true;pollsSinceAction_=0;switchesAtAction_=s.switches;
            activationsAtAction_=s.unlockActivations;screenAtAction_=s.route.screen;return;
        }
        if(garageActions_==0){
            // The Garage page bypasses the transition owner: its Back writes
            // manager pending 9/1 and lifecycle 6 (1D6178) for its own tick.
            require(s.route.screen==13,"tournament return route Garage Back faulted");
        }else{
            const auto& t=s.transition;const bool launch=garageActions_==2;
            require(s.route.screen==screens_[garageActions_]&&t.bound&&same(t.current,s.route)&&
                (launch?same(t.requested,s.route)&&t.lifecycle==Life::requestCourseExit:
                    !same(t.requested,s.route)&&t.lifecycle==Life::requestScreenExit)&&t.phase==Phase::none,
                "tournament return route original Confirm did not accept a destination");
        }
        ++garageActions_;
        if(garageActions_<3){waitingChange_=true;pollsSinceAction_=0;switchesAtAction_=s.switches;
            activationsAtAction_=s.unlockActivations;screenAtAction_=s.route.screen;}
    }
};
}
