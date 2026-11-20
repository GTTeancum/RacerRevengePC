// Common rewrite (R9): the ONE end-of-race dispatch and the ONE departure,
// PLAN.md 2.7, 2.8 (rows 9/10, Time Trial QUIT 1BDAB4), 2.1 step 18; F02-F05.
// Implements common_race_end50CR (common_race_end50cr.h, W0) and the
// Application entries departRace50CR / quitTimeTrialFromPause1BDAB4 /
// confirmTimeTrialRecord1CA574 (original_ui_main_entry50kg.h, W0).
//
// Every branch reads launch data only (A10: 11258, P+34, humans34, App2E0).
// No course, pod or pilot is named. Nothing writes guest memory.
#include "common_race_end50cr.h"
#include "common_race_start50cr.h"
#include "native_common_results50pd.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_race_hud_scope50la.h"
#include "original_position_stats_adapter50la.h"
#include "original_camera_session50kx.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include "../../src/game/original_results104_storage50la.h"
#include "../../src/game/original_race_end_entry50la.h"
#include "../../src/game/original_race_ack_route50pd.h"
#include <array>
#include <cstdio>
#include <optional>
#include <thread>

namespace common_race_end50CR {

Decision decide1A7DD0(const Launch& launch)noexcept {
    Decision d;
    switch(launch.subtype11258){
    case 1u:
        // 1A7DF8 199770, 1A7E18 Results104 vtable+20, 1A7E24 1AD230, 1AB520 ->
        // 1AB5BC rows. humans34 == 0 is the ALL8 spectator: the same dispatch.
        d.route=launch.humans34==0u?Route::spectator:Route::results;
        d.branch=0x1A7E18u;d.next=0x1AB5BCu;return d;
    case 3u: // 1AB55C -> 1ABBC0 -> 1ABBE8 199770 -> 1ABC0C 1AC8E0(race,0)
        d.route=Route::tt_restart;d.branch=0x1ABBE8u;d.next=0x1ABC0Cu;return d;
    case 2u: // 1ABC1C bit5, 199770, VCR, 288630, 1ABD28 1AC8E0(race,0), replay
        d.route=Route::practice;d.branch=0x1ABC1Cu;d.next=0x1ABD28u;return d;
    default:
        d.route=Route::refused;d.branch=0x1AB55Cu;d.next=0x17DE10u;return d;
    }
}

bool Owner::bind(const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,const Launch& launch,
    common_race_start50CR::Owner& start)noexcept {
    if(!race||!race_.expired()||departing_)return false;
    race_=race;launch_=launch;start_=&start;return true;
}
void Owner::unbind()noexcept {
    race_.reset();start_=nullptr;launch_={};departing_=false;
}

bool Owner::continue1A7DD0(const ::original_position_stats_adapter50LA::Owner& observer,
    const common_race_start50CR::Hooks& hooks,rr::game::OriginalRaceEndEntryDiagnostic50LA& d)noexcept {
    (void)observer; // the 1AB520 body binds its own Camera/manager observation (Registration)
    d={};
    auto& common=native_common_results50PD::state();
    const auto race=race_.lock();
    const auto decision=decide1A7DD0(launch_);
    const auto refuseToCourse=[&](std::uint32_t pc,const char* why)noexcept {
        if(!common.endRefusedLogged){
            common.endRefusedLogged=true;common.fallback=true;
            try{common_race_log50CR::raceEndRefused(launch_.subtype11258,pc,why?why:"");}catch(...){}
            // PLAN 2.7: a refused end departs to the Course screen through
            // step 18 (PORT failure handling, flagged; not an original screen).
            Departure departure;depart(Destination::course,departure);
        }
        return false;
    };
    if(!race||!start_||departing_){d.operation="common race end not bound";return false;}
    if(decision.route==Route::refused)return refuseToCourse(0x1AB55Cu,"launch subtype 11258 outside 1/2/3");
    application_.continueConsumedRaceEndEntry1A7DD0(d);
    ++common.endCalls;common.endActive=true;
    common.lastPC=d.sourcePC;common.lastStatus=static_cast<std::uint8_t>(d.status);common.lastOperation=d.operation;
    if(!common.endLogged){
        common.endLogged=true;
        try{common_race_log50CR::raceEnd(launch_.subtype11258,decision.branch,decision.route,decision.next);}catch(...){}
    }
    using S=rr::game::OriginalRaceEndEntryDiagnostic50LA::Status;
    using namespace common_race_start50CR;
    if(decision.route==Route::tt_restart&&d.status==S::callerReturned&&d.sourcePC==0x1abc0cu){
        // 1ABC0C 1AC8E0(race,0): in place, no session reload. 216690 keeps the
        // best lap CB4 because 11258==3 (lap_timing50id.h).
        Receipt receipt;
        return start_->begin(Request{Reason::restart,0u,callerTimeTrial1ABC0C,3u},hooks,receipt);
    }
    if(decision.route==Route::practice&&d.status==S::callerReturned&&d.sourcePC==0x1abd28u){
        // 1ABCBC..1ABCFC VCR channel loop / 1ABD04 288630, then 1ABD10..2C race1B0
        // bit4 (replay) set and 1ABD28 1AC8E0(race,0) (its bit4 branch: 17BCE0
        // per human, no countdown). The replay then plays from race78.
        // PORT STAND-IN (flagged): the VCR channel loop is the replay owner's
        // beginReplay (1ABAE8..1ABB68 shape), as for the Results-waiting replay.
        if(!hooks.backupRecordActor288630||!(*hooks.backupRecordActor288630)())
            return refuseToCourse(0x1ABD04u,"practice 288630 record actor backup unavailable");
        auto& r=*race;
        r.flags1B0KnownValue_=std::uint8_t(r.flags1B0KnownValue_|0x10u);
        r.flags1B0KnownMask_=std::uint8_t(r.flags1B0KnownMask_|0x10u);
        Receipt receipt;
        if(!start_->begin(Request{Reason::restart,0u,callerPractice1ABC1C,2u},hooks,receipt))
            return refuseToCourse(0x1ABD28u,receipt.reason?receipt.reason:"practice 1AC8E0 refused");
        const char* why=nullptr;
        if(!common.beginReplay||!common.beginReplay(why))
            return refuseToCourse(0x1ABD30u,why?why:"practice replay owner unavailable");
        common.replayActive=true;return true;
    }
    // Running: Results waiting / replay / acknowledged (1AB5BC rows are the
    // Results input's), or a reached-but-unrepresented later cut (logged by
    // the end entry's own diagnostic line).
    if(d.status==S::callerReturned||d.status==S::incomplete)return true;
    return refuseToCourse(d.sourcePC,d.operation);
}

bool Owner::depart(Destination destination,Departure& out)noexcept {
    out={};out.destination=destination;
    const auto race=race_.lock();
    if(!race||departing_){out.reason=race?"departure already running":"common race end not bound";
        common_race_log50CR::teardownRole("race_depart",false);return false;}
    departing_=true;
    const bool ok=application_.departRace50CR(*race,destination,out);
    if(!ok)departing_=false; // a refused departure may be retried by the caller's next row
    return ok;
}
} // namespace common_race_end50CR

namespace original_main_entry50KG {
namespace {
using Destination=common_race_end50CR::Destination;
bool feedbackOk(original_camera_cycle_audio50KX::Owner& audio,std::uint32_t key)noexcept {
    using A=original_camera_cycle_audio50KX::Owner::Result;
    const auto a=audio.dispatchPauseFeedback(key);
    return a==A::started||a==A::unavailable||a==A::muted;
}
}

// Step 18, in the original order of the calling row:
//   Pause row 10 / no record (1BDB5C): 26D070, 1A4AB0(0.5), 1A4B90(2), 267BE0,
//     2AED60(500), 1BE830, 17DE10(App,6,6,1) at 1BDBFC.
//   Pause row 9 (1BD9B8): 26D070, 1A4AB0(0.5), 1A4B90(2), 267BE0, 1BE830,
//     2AED60(500), 17DE10(App,6,4,1) at 1BDA64.
// The Hall (record confirm 1CA574..1CA6F0) and the tournament rows (1AB5BC)
// depart through their own Results/acknowledgement owners, not here.
bool Application::departRace50CR(rr::game::OriginalRaceConstructor50LA& race,
    common_race_end50CR::Destination destination,common_race_end50CR::Departure& out)const noexcept {
    out={};out.destination=destination;
    const auto refuse=[&](const char* role,std::uint32_t pc,const char* why)noexcept {
        out.ok=false;out.reachedPc=pc;out.reason=why;
        try{common_race_log50CR::teardownRole(role,false);}catch(...){}
        std::fprintf(stderr,"race_depart refused role=%s pc=%X reason=\"%s\"\n",role,unsigned(pc),why);
        return false;
    };
    const auto released=[&](const char* role,std::uint32_t pc)noexcept {
        out.reachedPc=pc;try{common_race_log50CR::teardownRole(role,true);}catch(...){}
    };
    if(destination==Destination::hall)
        return refuse("race_depart",0x1CA574u,"Hall departs through the Results record confirm 1CA574..1CA6F0");
    if(destination==Destination::tournament)
        return refuse("race_depart",0x1AB5BCu,"tournament rows depart through the 1AB5BC acknowledgement");
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||sourceRace28COwner_.get()!=&race||
       !race.counted_||race.rolledBack_)
        return refuse("race_depart",0x26D070u,"departure requires this Application's own counted race28C");
    const bool mainMenu=destination==Destination::mainMenu;
    const auto scope=race.endHud_.lock();const auto aural=sourceAural00Owner_;
    auto* cycle=native_common_results50PD::state().raceCycleAudio;
    if(!scope||!scope->music_||!scope->announcer_||!scope->gameplay_||!scope->engines_||
       !race.raceAudio_||!aural||!cycle)
        return refuse("aural_26D070",mainMenu?0x1BD9D4u:0x1BDB6Cu,"race audio owners (Aural, 10C scope, cycle audio) unavailable");
    rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
    if(!aural->stopResultsNative26D070(*this,*race.raceAudio_,*scope->music_,*cycle,*scope->announcer_,
        *scope->gameplay_,*scope->engines_,scope->countdown_,race.entity_.sessionGeneration,ad))
        return refuse("aural_26D070",mainMenu?0x1BD9D4u:0x1BDB6Cu,"26D070 Aural stop refused");
    released("aural_26D070",0x26D070u);
    if(!scope->music_->requestFade1A4AB0(0.5f).completed())
        return refuse("music_1A4AB0",0x1A4AB0u,"1A4AB0(0.5) music fade incomplete");
    released("music_1A4AB0",0x1A4AB0u);
    if(!feedbackOk(*cycle,2u))return refuse("feedback_1A4B90",0x1A4B90u,"1A4B90(2) feedback owner unavailable");
    released("feedback_1A4B90",0x1A4B90u);
    if(!aural->finishResultsNative267BE0(*this,*race.raceAudio_,ad))
        return refuse("aural_267BE0",0x267BE0u,"267BE0 Aural transport finish refused");
    released("aural_267BE0",0x267BE0u);
    const auto fade=[&]()noexcept {
        // 2AED60(500 ms): PORT STAND-IN (flagged, F05 remainder): the blocking
        // 500 ms fade over the race is not run here; the frontend receiver's
        // fade-in after the 17DE10 departure covers the screen change.
        std::fprintf(stderr,"race_depart step=2AED60 ms=500 performed_by=frontend_receiver stand_in=1\n");
        out.reachedPc=0x2AED60u;
    };
    const auto close=[&]()noexcept {
        // 1BE830 on this race's Pause100 (App11C = 0, Pause+200 = 0, 1ACF50 resume,
        // 142040(race,0), activity off). A Pause that is not open has nothing to close.
        const auto& pause=race.pause100_;
        if(!pause||!pause->active200_){released("pause_1BE830",0x1BE830u);return true;}
        setExplicitAnimationPause(false);pause->active200_=0;
        // PORT STAND-IN (flagged, 149p): a world whose 4D8 group the open could
        // not pause has nothing to resume; its refusal is logged, not fatal.
        if(!pauseConsumedRaceWorld1ACF50(race,false))
            std::fprintf(stderr,"race_depart step=1ACF50 resume=refused stand_in=1\n");
        if(!pauseConsumedRaceEntity142040(race,false)||!pause->entity_||!pause->entity_->setActivity141630(false))
            return refuse("pause_1BE830",0x1BE878u,"1BE830 Pause close refused");
        released("pause_1BE830",0x1BE830u);return true;
    };
    if(mainMenu){if(!close())return false;fade();}
    else{fade();if(!close())return false;}
    rr::game::OriginalRaceAckDestination50PD request;
    request.route=rr::game::OriginalRaceAckRoute50PD::course; // label only; the screen lane selects
    request.flags=6u;request.screen=mainMenu?4u:6u;request.page=1u;request.sourcePC=mainMenu?0x1bda64u:0x1bdbfcu;
    if(!commitRaceAcknowledgementTransfer50PD(race,request))
        return refuse("request_17DE10",request.sourcePC,"17DE10 request append refused");
    released("request_17DE10",0x17DE10u);
    out.ok=true;out.reachedPc=0x17DE10u;out.reason=nullptr;
    std::fprintf(stderr,"race_depart destination=%s screen=%u pc=%X\n",mainMenu?"main":"course",
        unsigned(request.screen),unsigned(request.sourcePC));
    return true;
}

// Pause row 10 in a Time Trial, 1BDA74 -> 1BDAB4 1ABFC0(race,1):
//   qualifies: Pause+200 = 0 and Pause activity off (1BDAC0..D0), 1A4B90(2),
//     199770(F8,0,1,1,1,1) (1BDB10), Results104 activate (1BDB30 vtable+20),
//     1CA8E0(Results,3,0) record/name page 3 (1BDB4C). The page's own input
//     (original_results104_input50la.cpp, 1CA340 -> 1CA574..1CA6F0) then
//     stores the name, stops the audio and queues the Hall 17DE10(6,11,1).
//   not: 1BDB5C, the step-18 departure to the Course screen.
bool Application::quitTimeTrialFromPause1BDAB4(rr::game::OriginalRaceConstructor50LA& race,
    bool& qualifies,std::uint32_t& pc)const noexcept {
    qualifies=false;pc=0x1BDAB4u;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||sourceRace28COwner_.get()!=&race||
       !race.counted_||race.rolledBack_||!race.profile_||race.profile_!=riderProfile||!race.results104_||
       !race.pause100_||!standardMaterialDomainOwner_)return false;
    try {
        const auto scope=race.endHud_.lock();
        const auto* camera=race.cameraNativeAdapter34_;
        if(!scope||!scope->manager_||!camera||camera!=scope->camera_)return false;
        using Observation=original_position_stats_adapter50LA::Owner;
        Observation observer;Observation::Diagnostic observed;
        if(!observer.bind(*camera,*scope->manager_,observed))return false;
        const auto* materials=standardMaterialDomainOwner_->materials382B38().get();
        const auto generation=race.entity_.sessionGeneration;
        Observation::EndObservation current;
        if(!observer.captureEndObservation1AB520(race.application_.get(),materials,generation,current,observed))return false;
        using Query=rr::game::OriginalRaceRecordQualification50KG;
        std::array<rr::game::OriginalRaceRecordQualificationRacerObservation50KG,2> rows{};
        std::size_t available=0;std::optional<Query> query;Query::Diagnostic qd;
        const std::optional<std::uint8_t> low2=(race.flags1B0KnownMask_&3u)==3u?
            std::optional<std::uint8_t>{std::uint8_t(race.flags1B0KnownValue_&3u)}:std::nullopt;
        while(!race.profile_->prepareRaceRecordQualification1ABFC0(1,low2,rows.data(),available,query,qd)){
            if(qd.status!=Query::Status::missingRacer||qd.managerIndex!=available||available>=rows.size()){pc=qd.sourcePC;return false;}
            Observation::ResultsRecordIdentity row;auto& value=rows[available];
            if(!observer.captureResultsRecordIdentity(race.application_.get(),materials,generation,available,row,observed)||
               row.publication!=current.publication||!observer.readEndRetirementCD4(row,value.retirementCD4,observed)){
                pc=qd.sourcePC;return false;}
            if(!value.retirementCD4&&
               (!observer.readResultsBestLapCB4(row,value.records.bestLapCB4,observed)||
                !observer.readResultsRaceTimeCB8(row,value.records.timeCB8,observed)||
                !observer.readResultsObservedTimingCD0(row,value.records.countCD0,observed))){pc=qd.sourcePC;return false;}
            ++available;
        }
        if(!query)return false;
        qualifies=query->originalV0();
        common_race_log50CR::pauseQuitRecord(qualifies);
        if(!qualifies){
            pc=0x1BDB5Cu;
            common_race_end50CR::Departure departure;
            const bool departed=departRace50CR(race,Destination::course,departure);
            pc=departure.reachedPc?departure.reachedPc:0x1BDB5Cu;return departed;
        }
        auto& pause=*race.pause100_;
        pc=0x1BDAC0u;pause.active200_=0;
        if(!pause.entity_||!pause.entity_->setActivity141630(false))return false;
        pc=0x1BDAE8u;
        auto* cycle=native_common_results50PD::state().raceCycleAudio;
        if(!cycle||!feedbackOk(*cycle,2u))return false;
        pc=0x1BDB10u;
        if(!race.f8Attached_||!race.f8Source_||race.f8Owner_.get()!=race.f8Source_||
           !race.f8Source_->visibility199770(0,1,1,1,1))return false;
        pc=0x1BDB30u;
        rr::game::OriginalResults104Diagnostic50LA rd;
        if(!race.results104_->activate1A7E18(race,rd)){pc=rd.sourcePC?rd.sourcePC:0x1BDB30u;return false;}
        pc=0x1BDB4Cu;
        rr::game::OriginalResults104Diagnostic50LA md;
        const auto mode=race.results104_->setModeCpuPrefix1CA8E0(race,observer,3u,0u,md);
        using Mode=rr::game::OriginalResults10450LA::ModeCpuPrefix;
        if(mode==Mode::none||mode==Mode::scalarPrefix){pc=md.sourcePC?md.sourcePC:0x1BDB4Cu;return false;}
        pc=0x1BDB54u;return true;
    }catch(...){return false;}
}

// 1CA574..1CA6F0 runs inside the Results page-3 input (the original 1CA340
// confirm handler, original_results104_input50la.cpp: name store 1CA528,
// 1ABFC0(race,2), 26D070, 1A4B90(2), 1A4AB0(0.5), 267BE0, Hall request
// prepareResultsHallTransfer50LA at 1CA678, then the blocking fade and the
// Hall 17DE10(6,11,1) at 1CA6F0 with 1AC440). This entry reports where that
// confirm stands for the counted Time Trial race; it never replays it.
bool Application::confirmTimeTrialRecord1CA574(rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t& pc)const noexcept {
    pc=0x1CA574u;
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||sourceRace28COwner_.get()!=&race||!race.results104_)return false;
    std::uint32_t subtype=0;
    if(!race.profile_||!race.profile_->resultsSubtype11258(subtype)||subtype!=3u){pc=0x1CA5F0u;return false;}
    if(resultsHallTransfer50LA_){pc=0x1CA678u;return true;}   // Hall request prepared: departing
    if(resultsInputContinuation50LA_){pc=0x1CA6A4u;return true;} // confirm suspended across its blocking fade
    return false;
}
} // namespace original_main_entry50KG
