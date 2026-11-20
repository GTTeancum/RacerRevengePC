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

// Opt-in native window evidence navigation only. This owner emits the same
// original Confirm event as Enter, at most three times. SA adds one original
// Next and optionally one Rider Next. Explicit rider/course navigation allows
// at most 26 Rider Nexts, followed by the original course sequence. The free-play
// add-on rider test (RR_TEST_ADDON_RIDER=<package>/<item> on main-rider-course-ta,
// PORT EXTENSION, NOT ORIGINAL) presses Rider Next at most 26 + add-on count times
// until that add-on is highlighted; RR_TEST_ADDON_RIDER_HOLD=1 then stops there.
// RR_TEST_RIDER_HOLD=1 (no add-on) stops on the stock rider the route lands on
// (no Confirm) so the run holds on the Rider screen until its wall budget. GA takes
// three Course Nexts; Alder/POD10 or Ody/POD11 on MA takes two; Gasgano7/SA takes one; the explicit Mawhonic9/RA fixture takes four.
// The free-play add-on course test (RR_TEST_ADDON_COURSE_PICK=<package>/<item> on
// main-rider-course-ta, PORT EXTENSION, NOT ORIGINAL) presses Course Next at most
// 13 + add-on count times until that add-on course entry is highlighted (read from
// dlc50DL::highlightedAddonCourse), then the original Course Confirm launches it;
// RR_TEST_ADDON_COURSE_HOLD=1 stops there instead (no Confirm, the run holds on the
// Course screen until its wall budget).
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
    bool sa_=false,awaitingMove_=false,moved_=false;
    bool occo_=false,awaitingRiderMove_=false,riderMoved_=false;
    bool selectedRoute_=false,gasganoInitial_=false,mawhonicRa_=false;
    std::uint32_t selectedRider_=4;
    std::uint8_t targetCourseMoves_=3;
    std::uint8_t riderMoves_=0,courseMoves_=0;
    std::uint32_t priorRider_=19;
    std::uint64_t priorRiderSource_=0,priorRiderPublication_=0;
    std::uint8_t issued_=0;
    // PORT EXTENSION, NOT ORIGINAL (DLC add-on rider test, main-rider-course-ta only).
    bool addon_=false,addonHold_=false,awaitingAddonMove_=false,addonHoldLogged_=false;
    bool stockHold_=false,stockHoldLogged_=false; // test eye check: hold on the landed stock Rider
    const char* addonKey_=nullptr;
    unsigned addonTarget_=0,priorHighlight_=0;
    std::uint32_t addonMoves_=0;
    std::uint64_t priorAddonSource_=0;
    // PORT EXTENSION, NOT ORIGINAL (DLC add-on course test, main-rider-course-ta only).
    bool addonCourse_=false,addonCourseHold_=false,awaitingAddonCourseMove_=false,addonCourseHoldLogged_=false;
    const char* addonCourseKey_=nullptr;
    unsigned addonCourseTarget_=0,priorCourseHighlight_=0;
    std::uint32_t addonCourseMoves_=0,priorCourseSlot_=0;
    std::uint64_t priorCourseRenders_=0;
    // Event type for the add-on course test (147c, RR_TEST_ADDON_COURSE_SUBTYPE=1..3;
    // unset = the Course screen's own subtype, Single Race on a fresh profile).
    // Before any course move: original Down to the event-type row, Right until that
    // subtype shows, Up back to the course row. Same original inputs a player uses.
    std::uint32_t addonCourseSubtype_=0,addonEventMoves_=0,priorEventSubtype_=0;
    std::int32_t priorEventGroup_=-1;
    bool awaitingAddonEventMove_=false;

    static void require(bool condition,const char* message) {
        if(!condition)throw std::runtime_error(message);
    }
    static unsigned addonNumber(const char* key) noexcept {
        if(!key)return 0;
        for(std::size_t i=0;i<rr::game::dlc50DL::addonRiderCount();++i)
            if(const auto* pod=rr::game::dlc50DL::addonRiderAt(i);pod&&pod->key==key)return pod->number;
        return 0;
    }
    // Rider stage of the add-on test: -1 wait, 1 Rider Next, 2 go on to Confirm.
    int pollAddonRider(const Snapshot& s,bool riderCaptured,bool accepted,
        std::uint64_t source,std::uint64_t publication) {
        if(!addonTarget_){
            addonTarget_=addonNumber(addonKey_);
            require(addonTarget_!=0,"RR_TEST_ADDON_RIDER is not an admitted add-on pod (check RR_DLC_ROOT and the dlc_item lines)");
            require(!addonMoves_&&s.rider.rider4C4==19&&s.rider.slot4C8==1,
                "add-on rider route requires the actual fresh Rider19 entry");
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
    static unsigned addonCourseNumber(const char* key) noexcept {
        if(!key)return 0;
        for(std::size_t i=0;i<rr::game::dlc50DL::addonCourseCount();++i)
            if(const auto* c=rr::game::dlc50DL::addonCourseAt(i);c&&c->key==key)return c->number;
        return 0;
    }
    // Course stage of the add-on course test: -1 wait, 1 Course Next, 2 go on to Confirm,
    // or (event type first, 147c) 3 Down / 1 Right / 2 Up on the Course page rows.
    int pollAddonCourse(const Snapshot& s,bool courseCaptured) {
        if(!addonCourseTarget_){
            addonCourseTarget_=addonCourseNumber(addonCourseKey_);
            require(addonCourseTarget_!=0,"RR_TEST_ADDON_COURSE_PICK is not an admitted add-on course (check RR_DLC_ROOT and the dlc_course lines)");
            require(s.course.mode34==0,"add-on course route requires free play");
        }
        if(addonCourseSubtype_&&!addonCourseMoves_){
            if(addonEventMoves_&&s.course.renders<=priorCourseRenders_)return -1;
            const bool typed=s.course.subtypeSlot14F4==addonCourseSubtype_;
            if(!typed||s.course.controllerGroup!=1){
                require(!courseCaptured,"add-on course route captured the Course screen before its event type");
                require(addonEventMoves_<8u,"add-on course event type was not reached within 8 row/type moves");
                priorEventSubtype_=s.course.subtypeSlot14F4;priorEventGroup_=s.course.controllerGroup;
                priorCourseRenders_=s.course.renders;awaitingAddonEventMove_=awaitingDispatch_=true;
                const auto group=s.course.controllerGroup;
                if(!typed&&group==2)return 1;                        // Right on the event-type row
                if(!typed)return group<2?3:2;                        // Down / Up to the event-type row
                return group>1?2:3;                                  // Up (Down) back to the course row
            }
        }
        // A frame rendered after the last move (an add-on step defers nothing).
        if(addonCourseMoves_&&s.course.renders<=priorCourseRenders_)return -1;
        const auto highlighted=rr::game::dlc50DL::highlightedAddonCourse();
        if(highlighted!=addonCourseTarget_){
            require(!courseCaptured,"add-on course route captured the Course screen before its add-on course");
            require(addonCourseMoves_<13u+rr::game::dlc50DL::addonCourseCount(),
                "add-on course was not reached within the stock courses plus the add-on count");
            priorCourseSlot_=s.course.courseSlot14E4;priorCourseHighlight_=highlighted;priorCourseRenders_=s.course.renders;
            awaitingAddonCourseMove_=awaitingDispatch_=true;return 1;
        }
        if(!courseCaptured)return -1;
        if(addonCourseHold_){
            if(!addonCourseHoldLogged_){addonCourseHoldLogged_=true;
                std::cerr<<"addon_course_route hold key="<<addonCourseKey_<<" number="<<addonCourseTarget_<<" moves="<<addonCourseMoves_
                    <<" anchor_slot="<<s.course.courseSlot14E4<<" anchor_course="<<s.course.selectedCourse44
                    <<" (no Confirm; the run ends at its wall budget)\n";}
            return -1;
        }
        std::cerr<<"addon_course_route confirm key="<<addonCourseKey_<<" number="<<addonCourseTarget_<<" moves="<<addonCourseMoves_
            <<" anchor_slot="<<s.course.courseSlot14E4<<" anchor_course="<<s.course.selectedCourse44
            <<" subtype="<<s.course.subtypeSlot14F4<<'\n';
        return 2;
    }
    static bool same(rr::game::OriginalUiTransitionRoute50KG a,
        rr::game::OriginalUiTransitionRoute50KG b) noexcept {
        return a.screen==b.screen&&a.page==b.page;
    }
    static bool open(const rr::game::OriginalUiApertureSnapshot50KG& a) noexcept {
        return a.bound&&a.inputEnabled268&&a.state==Aperture::open;
    }
    static bool selectedSa(const Snapshot& s) noexcept {
        return s.course.courseSlot14E4==2&&s.course.selectedCourse44==11&&
            s.course.selectedPlanet40==4&&s.course.subtypeSlot14F4==1&&
            s.course.laps14EC==3&&s.course.entrants14F0==8;
    }
    static bool selectedOcco(const Snapshot& s) noexcept {
        return s.rider.rider4C4==20&&s.rider.slot4C8==2;
    }
    bool freshOccoSource(bool accepted,std::uint64_t source,std::uint64_t publication)const noexcept {
        // GPU publication identifies the retained bank, not a new rendered frame.
        return accepted&&source>priorRiderSource_&&publication&&publication==priorRiderPublication_;
    }
    static bool selectedFreshCourse(const Snapshot& s,unsigned move) noexcept {
        constexpr std::uint32_t courses[]{0,11,4,8,7},planets[]{0,4,5,1,2};
        return move<5&&s.course.courseSlot14E4==move+1&&
            s.course.selectedCourse44==courses[move]&&s.course.selectedPlanet40==planets[move]&&
            s.course.subtypeSlot14F4==1&&s.course.laps14EC==3&&s.course.entrants14F0==8;
    }
    int pollSelectedRoute(const Snapshot& s,std::int32_t mainGroup,std::int32_t mainSlot,
        bool mainCaptured,bool riderCaptured,bool courseCaptured,bool riderSourceAccepted,
        bool courseReady,std::uint64_t riderSource,std::uint64_t riderPublication) {
        require(!awaitingDispatch_,"selected-rider/course dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"selected-rider/course original UI became unavailable");
        if(issued_==3)return -1;
        const auto screen=static_cast<std::uint8_t>(4+issued_);
        const bool expected=s.route.screen==screen&&s.route.page==1&&s.switches==issued_;
        const bool outgoing=issued_&&s.route.screen==screen-1&&s.route.page==1&&s.switches==issued_-1;
        require(expected||outgoing,"unattended UI left the bounded Main/selected-rider/Course route");
        if(!expected||!settled(s))return -1;
        if(issued_==0){
            require(mainGroup==1&&mainSlot==1,"selected-rider/course requires fresh Main selection");
            if(!mainCaptured)return -1;
        }else if(issued_==1){
            require(mainCaptured,"Selected Rider arrived without Main capture");
            if(gasganoInitial_||mawhonicRa_)require(!riderMoves_&&s.rider.rider4C4==selectedRider_&&
                s.rider.slot4C8==(mawhonicRa_?11u:10u),
                "Explicit test profile requires its original remembered Rider/slot entry");
            else if(!riderMoves_)require(s.rider.rider4C4==19&&s.rider.slot4C8==1,
                "Selected route requires actual fresh Rider19 entry");
            if(!riderSourceAccepted||!riderSource||!riderPublication)return -1;
            if(riderMoves_&&!freshOccoSource(riderSourceAccepted,riderSource,riderPublication))return -1;
            if(s.rider.rider4C4!=selectedRider_){
                require(!riderCaptured&&riderMoves_<26,"Selected rider was not reached within original roster bound");
                priorRider_=s.rider.rider4C4;
                priorRiderSource_=riderSource;priorRiderPublication_=riderPublication;
                awaitingRiderMove_=awaitingDispatch_=true;return 1;
            }
            if(!riderCaptured)return -1;
        }else{
            require(mainCaptured&&riderCaptured,"Selected Course arrived without Main/selected Rider captures");
            require(selectedFreshCourse(s,courseMoves_),"Selected route course selection diverged from fresh original order");
            if(courseMoves_<targetCourseMoves_){
                require(!courseCaptured,"Selected route captured an intermediate course");
                awaitingMove_=awaitingDispatch_=true;return 1;
            }
            if(!courseCaptured)return -1;
            require(courseReady,mawhonicRa_?
                "Mawhonic9 / Ryloth Brightlands UI reached course7/planet2/slot5; RA cold target/runtime readiness is unavailable. No race Confirm or substitute dispatched":
                "Selected course original race prerequisites are unavailable");
        }
        ++issued_;awaitingDispatch_=true;return 4;
    }
public:
    explicit Route(bool enabled,bool sa=false,bool occo=false,bool dudGa=false,bool benGa=false,bool alderMa=false,
        bool odyMa=false,bool gasganoSa=false,bool mawhonicRa=false) noexcept:
        enabled_(enabled),sa_(enabled&&(sa||occo)),occo_(enabled&&occo),selectedRoute_(enabled&&(dudGa||benGa||alderMa||odyMa||gasganoSa||mawhonicRa)),
        gasganoInitial_(enabled&&gasganoSa),mawhonicRa_(enabled&&mawhonicRa),selectedRider_(mawhonicRa?9u:gasganoSa?7u:odyMa?11u:alderMa?10u:benGa?2u:4u),targetCourseMoves_(mawhonicRa?4u:gasganoSa?1u:(alderMa||odyMa)?2u:3u){
        // Free-play TA route only (no SA/Occo/selected fixture). Unset = stock route unchanged.
        const char* key=std::getenv("RR_TEST_ADDON_RIDER");
        addon_=enabled_&&!selectedRoute_&&!sa_&&!occo_&&key&&*key;
        addonKey_=addon_?key:nullptr;
        const char* hold=std::getenv("RR_TEST_ADDON_RIDER_HOLD");
        addonHold_=addon_&&hold&&std::strcmp(hold,"1")==0;
        const char* course=std::getenv("RR_TEST_ADDON_COURSE_PICK");
        addonCourse_=enabled_&&!selectedRoute_&&!sa_&&!occo_&&course&&*course;
        addonCourseKey_=addonCourse_?course:nullptr;
        const char* courseHold=std::getenv("RR_TEST_ADDON_COURSE_HOLD");
        addonCourseHold_=addonCourse_&&courseHold&&std::strcmp(courseHold,"1")==0;
        const char* courseSubtype=std::getenv("RR_TEST_ADDON_COURSE_SUBTYPE");
        if(addonCourse_&&courseSubtype&&*courseSubtype){
            if(courseSubtype[0]<'1'||courseSubtype[0]>'3'||courseSubtype[1])
                throw std::runtime_error("RR_TEST_ADDON_COURSE_SUBTYPE must be 1 (Single Race), 2 or 3 (Time Trial)");
            addonCourseSubtype_=std::uint32_t(courseSubtype[0]-'0');
        }
        const char* stockHold=std::getenv("RR_TEST_RIDER_HOLD");
        stockHold_=enabled_&&!addon_&&!selectedRoute_&&!sa_&&!occo_&&stockHold&&std::strcmp(stockHold,"1")==0;
    }
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(issued_+addonMoves_+addonCourseMoves_+addonEventMoves_+
        (selectedRoute_?courseMoves_+riderMoves_:(moved_?1u:0u)+(riderMoved_?1u:0u)));}
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

    // Gate requested captures too: initial Anakin/TA and intermediate refresh
    // frames must never satisfy the explicit Occo/SA or selected-rider/course stage receipts.
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
        // Add-on course test: only the settled frame with the requested add-on course highlighted.
        if(addonCourse_&&s.route.screen==6)return addonCourseTarget_&&!awaitingDispatch_&&issued_==2&&
            rr::game::dlc50DL::highlightedAddonCourse()==addonCourseTarget_&&settled(s)&&
            (!addonCourseMoves_||s.course.renders>priorCourseRenders_);
        if(selectedRoute_){
            if(s.route.screen==5){
                if(gasganoInitial_||mawhonicRa_)return !riderMoves_&&!awaitingDispatch_&&issued_==1&&
                    s.rider.rider4C4==selectedRider_&&s.rider.slot4C8==(mawhonicRa_?11u:10u)&&settled(s)&&
                    sourceAccepted&&source&&publication;
                return riderMoves_&&!awaitingDispatch_&&issued_==1&&
                    s.rider.rider4C4==selectedRider_&&settled(s)&&freshOccoSource(sourceAccepted,source,publication);
            }
            if(s.route.screen==6)return courseMoves_==targetCourseMoves_&&!awaitingDispatch_&&issued_==2&&
                selectedFreshCourse(s,targetCourseMoves_)&&settled(s);
        }
        if(occo_&&s.route.screen==5)return riderMoved_&&!awaitingDispatch_&&issued_==1&&
            selectedOcco(s)&&settled(s)&&freshOccoSource(sourceAccepted,source,publication);
        return !sa_||s.route.screen!=6||
            (moved_&&!awaitingDispatch_&&issued_==2&&selectedSa(s)&&settled(s));
    }

    // Call before the next update, using the previous completed frame. Capture
    // booleans mean successful file publication of the settled matching screen
    // in this frontend lifetime. courseReady is a host prerequisite check only;
    // the actual original Confirm dispatch retains all destination admission.
    // Return -1 while waiting, original Confirm4, or a one-shot Rider/Course Next1. A rejected or
    // changed route throws rather than retrying or choosing a substitute.
    int poll(const Snapshot& s,std::int32_t mainGroup,std::int32_t mainSlot,
        bool mainCaptured,bool riderCaptured,bool courseCaptured,
        bool riderSourceAccepted,bool courseReady,std::uint64_t riderSource=0,
        std::uint64_t riderPublication=0) {
        if(!enabled_)return -1;
        if(selectedRoute_)return pollSelectedRoute(s,mainGroup,mainSlot,mainCaptured,riderCaptured,courseCaptured,
            riderSourceAccepted,courseReady,riderSource,riderPublication);
        require(!awaitingDispatch_,"unattended UI dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"unattended original UI became unavailable");
        if(issued_==3)return -1;
        const auto expectedScreen=static_cast<std::uint8_t>(4+issued_);
        const bool expected=s.route.screen==expectedScreen&&s.route.page==1&&s.switches==issued_;
        const bool outgoing=issued_&&s.route.screen==expectedScreen-1&&s.route.page==1&&s.switches==issued_-1;
        require(expected||outgoing,occo_?"unattended UI left the bounded Main/Occo20/Course-SA route":sa_?"unattended UI left the bounded Main/Rider19/Course-SA route":
            "unattended UI left the bounded Main/Rider19/Course-TA route");
        if(!expected||!settled(s))return -1;
        if(issued_==0){
            require(mainGroup==1&&mainSlot==1,"unattended Main requires its actual fresh group1/slot1 selection");
            if(!mainCaptured)return -1;
        }else if(issued_==1){
            require(mainCaptured,"unattended Rider arrived without the completed Main screenshot");
            if(addon_){
                const int action=pollAddonRider(s,riderCaptured,riderSourceAccepted,riderSource,riderPublication);
                if(action!=2)return action;
            }else if(occo_&&riderMoved_){
                require(selectedOcco(s),"unattended Rider requires committed Occo20/slot2 selection");
                if(!freshOccoSource(riderSourceAccepted,riderSource,riderPublication))return -1;
            }else{
                require(s.rider.rider4C4==19&&s.rider.slot4C8==1,
                    "unattended Rider requires the actual fresh Rider19 selection");
                if(occo_){
                    require(!riderCaptured,"unattended Occo route already captured another Rider");
                    if(!riderSourceAccepted||!riderSource||!riderPublication)return -1;
                    priorRiderSource_=riderSource;priorRiderPublication_=riderPublication;
                    awaitingRiderMove_=awaitingDispatch_=true;return 1;
                }
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
            if(sa_&&moved_){
                require(selectedSa(s),"unattended Course requires committed SA/3-lap/8-entrant selection");
            }else if(addonCourse_&&(addonCourseMoves_||addonEventMoves_)){
                // The add-on course route has moved the event type or the course row (stock moves, then add-on entries).
            }else{
                require(s.course.courseSlot14E4==1&&s.course.selectedCourse44==0&&
                    s.course.selectedPlanet40==0&&s.course.subtypeSlot14F4==1&&
                    s.course.laps14EC==3&&s.course.entrants14F0==8,
                    "unattended Course requires actual fresh TA/3-lap/8-entrant selection");
                if(sa_){
                    require(!courseCaptured,"unattended SA route already captured another Course selection");
                    require(courseReady,"unattended Course race prerequisites are unavailable");
                    awaitingMove_=awaitingDispatch_=true;return 1;
                }
            }
            if(addonCourse_){
                const int action=pollAddonCourse(s,courseCaptured);
                if(action!=2)return action;
            }
            if(!courseCaptured)return -1;
            require(courseReady,"unattended Course race prerequisites are unavailable");
        }
        ++issued_;awaitingDispatch_=true;return 4;
    }

    // Call immediately after originalMain.input(action), BEFORE update/render.
    // No successful dispatch is inferred from the host having issued an event.
    void validateDispatch(const Snapshot& s) {
        if(awaitingAddonEventMove_){
            const auto& t=s.transition;
            require(enabled_&&addonCourse_&&awaitingDispatch_&&issued_==2&&!s.faulted&&!s.quitRequested&&
                s.route.screen==6&&s.route.page==1&&s.switches==2&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "add-on course route event-type move changed the route");
            require(s.course.controllerGroup!=priorEventGroup_||s.course.subtypeSlot14F4!=priorEventSubtype_,
                "add-on course route row/event-type move changed neither the row nor the event type");
            ++addonEventMoves_;awaitingAddonEventMove_=awaitingDispatch_=false;
            std::cerr<<"addon_course_route event_move="<<addonEventMoves_<<" row="<<s.course.controllerGroup
                <<" subtype="<<s.course.subtypeSlot14F4<<" target_subtype="<<addonCourseSubtype_<<'\n';
            return;
        }
        if(awaitingAddonCourseMove_){
            const auto& t=s.transition;
            require(enabled_&&addonCourse_&&awaitingDispatch_&&issued_==2&&!s.faulted&&!s.quitRequested&&
                s.route.screen==6&&s.route.page==1&&s.switches==2&&t.bound&&same(t.current,s.route)&&
                same(t.requested,s.route)&&t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue,
                "add-on course route Course Next changed the route");
            const auto highlighted=rr::game::dlc50DL::highlightedAddonCourse();
            // Either the original stock move committed, or the port-owned add-on entry moved.
            const bool stockMove=highlighted==0&&s.course.courseSlot14E4!=priorCourseSlot_&&
                s.course.deferred14D0==3&&!s.course.framePublished;
            const bool addonMove=highlighted!=0&&highlighted!=priorCourseHighlight_&&s.course.courseSlot14E4==priorCourseSlot_;
            require(stockMove||addonMove,"add-on course route Course Next moved neither a stock course nor an add-on entry");
            ++addonCourseMoves_;awaitingAddonCourseMove_=awaitingDispatch_=false;
            std::cerr<<"addon_course_route move="<<addonCourseMoves_<<" slot="<<s.course.courseSlot14E4
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
        if(selectedRoute_&&(awaitingRiderMove_||awaitingMove_)){
            const auto& t=s.transition;
            const bool rider=awaitingRiderMove_;
            require(enabled_&&awaitingDispatch_&&!s.faulted&&!s.quitRequested&&
                s.route.screen==(rider?5:6)&&s.route.page==1&&s.switches==(rider?1u:2u)&&
                issued_==(rider?1u:2u)&&t.bound&&same(t.current,s.route)&&same(t.requested,s.route)&&
                t.lifecycle==Life::idle&&t.phase==Phase::none&&!t.applicationRequestQueued&&
                !t.initialRestartPending&&!s.deferredQueue,"selected-rider/course original Next changed the route");
            if(rider){
                require(s.rider.rider4C4!=priorRider_&&s.rider.slot4C8>0&&
                    s.rider.selectionAttached&&s.rider.deferred4EC==3&&!s.rider.framePublished,
                    "Selected rider original Next did not commit a fresh rider refresh");
                ++riderMoves_;awaitingRiderMove_=false;
            }else{
                require(courseMoves_<targetCourseMoves_&&selectedFreshCourse(s,courseMoves_+1)&&
                    s.course.deferred14D0==3&&!s.course.framePublished,
                    "Selected course original Next did not commit the next authored course");
                ++courseMoves_;awaitingMove_=false;
            }
            awaitingDispatch_=false;return;
        }
        if(awaitingRiderMove_){
            const auto& t=s.transition;
            require(enabled_&&occo_&&awaitingDispatch_&&!riderMoved_&&issued_==1&&
                !s.faulted&&!s.quitRequested&&s.route.screen==5&&s.route.page==1&&s.switches==1&&
                t.bound&&same(t.current,s.route)&&same(t.requested,s.route)&&
                t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue&&
                selectedOcco(s)&&s.rider.selectionAttached&&s.rider.deferred4EC==3&&!s.rider.framePublished,
                "unattended original Next did not commit Occo and its real deferred refresh");
            riderMoved_=true;awaitingRiderMove_=awaitingDispatch_=false;return;
        }
        if(awaitingMove_){
            const auto& t=s.transition;
            require(enabled_&&sa_&&awaitingDispatch_&&!moved_&&issued_==2&&
                !s.faulted&&!s.quitRequested&&s.route.screen==6&&s.route.page==1&&s.switches==2&&
                t.bound&&same(t.current,s.route)&&same(t.requested,s.route)&&
                t.lifecycle==Life::idle&&t.phase==Phase::none&&
                !t.applicationRequestQueued&&!t.initialRestartPending&&!s.deferredQueue&&
                selectedSa(s)&&s.course.deferred14D0==3&&!s.course.framePublished,
                "unattended original Next did not commit SA and its real deferred refresh");
            moved_=true;awaitingMove_=awaitingDispatch_=false;return;
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
class TournamentRoute final {
    using Snapshot=rr::platform::OriginalUiFrontendSnapshot50KG;
    using Life=rr::game::OriginalUiTransitionLifecycle50KG;
    using Phase=rr::game::OriginalUiTransitionPhase50KG;
    using Aperture=rr::game::OriginalUiApertureState50KG;
    bool enabled_=false,awaitingDispatch_=false,awaitingMainMove_=false,courseLogged_=false;
    std::uint8_t mainMoves_=0,confirms_=0,firstSlot_=0;
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
    explicit TournamentRoute(bool enabled)noexcept:enabled_(enabled){}
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(mainMoves_+confirms_);}
    bool actionsDispatched()const noexcept{return confirms_==4&&!awaitingDispatch_;}
    // Return -1 while waiting, Main Next3, or Confirm4.
    int poll(const Snapshot& s,std::int32_t mainGroup,std::int32_t mainSlot,bool courseReady){
        if(!enabled_)return -1;
        require(!awaitingDispatch_,"tournament route dispatch receipt was not verified");
        require(!s.faulted&&!s.quitRequested,"tournament route original UI became unavailable");
        if(confirms_==4)return -1;
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
            require(s.rider.rider4C4==expectedEntryRider50LA(),
                "tournament route Rider did not open on the profile's remembered racer");
            if(!s.rider.selectedCharacterReady)return -1;
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
    explicit TournamentReturnRoute(bool enabled)noexcept:enabled_(enabled){}
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
        if(garageActions_==0){if(!garageSettled(s))return -1;awaitingDispatch_=true;return 6;}
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
