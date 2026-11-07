#pragma once
#include "../../platform/linux/native/original_ui_frontend50kg.h"
#include <cstdint>
#include <stdexcept>

// Opt-in native window evidence navigation only. This owner emits the same
// original Confirm event as Enter, at most three times. SA adds one original
// Next and optionally one Rider Next. Explicit rider/course navigation allows
// at most 26 Rider Nexts, followed by the original course sequence. GA takes
// three Course Nexts; Alder/POD10 or Ody/POD11 on MA takes two; Gasgano7/SA takes one; the explicit Mawhonic9/RA fixture takes four.
// It does not write a profile, select a screen, advance a transition, control a racer, or admit a
// missing destination. The caller owns the explicit native-window unattended opt-in,
// shared wall-time deadline, actual framebuffer capture, and normal teardown.
namespace original_ui_capture_navigation50LA {
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

    static void require(bool condition,const char* message) {
        if(!condition)throw std::runtime_error(message);
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
        gasganoInitial_(enabled&&gasganoSa),mawhonicRa_(enabled&&mawhonicRa),selectedRider_(mawhonicRa?9u:gasganoSa?7u:odyMa?11u:alderMa?10u:benGa?2u:4u),targetCourseMoves_(mawhonicRa?4u:gasganoSa?1u:(alderMa||odyMa)?2u:3u){}
    bool enabled()const noexcept{return enabled_;}
    std::uint8_t issuedActions()const noexcept{return static_cast<std::uint8_t>(issued_+
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
            if(occo_&&riderMoved_){
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
        }else{
            require(mainCaptured&&riderCaptured,
                "unattended Course arrived without the completed Main/Rider screenshots");
            if(sa_&&moved_){
                require(selectedSa(s),"unattended Course requires committed SA/3-lap/8-entrant selection");
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
            if(!courseCaptured)return -1;
            require(courseReady,"unattended Course race prerequisites are unavailable");
        }
        ++issued_;awaitingDispatch_=true;return 4;
    }

    // Call immediately after originalMain.input(action), BEFORE update/render.
    // No successful dispatch is inferred from the host having issued an event.
    void validateDispatch(const Snapshot& s) {
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
    bool enabled_=false,awaitingDispatch_=false,awaitingMainMove_=false;
    std::uint8_t mainMoves_=0,confirms_=0;
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
            require(mainGroup==1&&mainSlot==1+mainMoves_,"tournament route Main selection diverged");
            if(mainSlot<3){awaitingMainMove_=awaitingDispatch_=true;return 3;}
        }else if(confirms_==1){
            require(s.rider.rider4C4==19,"tournament route requires the fresh Rider19 entry");
            if(!s.rider.selectedCharacterReady)return -1;
        }else if(confirms_==3){
            require(s.course.selectedCourse44==0&&s.course.selectedPlanet40==0,
                "tournament route requires the first tournament course (TA)");
            if(!courseReady)return -1;
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
}
