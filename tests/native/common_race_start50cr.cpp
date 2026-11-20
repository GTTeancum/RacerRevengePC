// Common rewrite (R9): the ONE race start, PLAN.md 2.6 (F01, F02).
// common_race_start50CR::Owner (declared by W0 in common_race_start50cr.h) and
// the Application entries bindRaceStart50CR / restartConsumedRace1AC8E0.
//
// Original 1AC8E0(race,a1), in its own order (dis_all.txt 1AC8E0..1ACE48):
//   1AC900 World74+3D0 vtable+80 (2DCC70 trigger reset)
//   1AC914 App11C: nonzero becomes zero
//   1AC950 103210 (App+0 Aural restart list)
//   1AC960 1A4AB0(race10C, 1.0)
//   1AC978 161F60 (World+4A0 particles)
//   1AC98C World74 vtable+74 (2702E0)
//   1AC994 race90 = 0
//   1AC9A0 1AEA70(B0, race, a1)   per racer: 2A2880 grid, 2169C0, racer virtuals
//                                 (216690 timing with the 11258 rule, 217420 damage)
//   1AC9A8 1AED30(B0)             start lock: 167A30(racer340,0,1,0) / 167A40(racer340,1,0,1)
//   1AC9BC.. per human 1148D0 / 2A2880 / 114770 / virtual+18 (camera targets)
//   race1B0 bit4 clear: 1AF330 per racer +3C = 0, 2AED60(raceFC,500) fade,
//                       race94 = 4.0, race1B0 bit2 clear
//   race1B0 bit4 set:   17BCE0 per human
//   1ACB18 11258==3 && race78 && !bit4: 287BE0, raceA0 = 0, B0+40 virtual+18 (record actor)
//   1ACB74 267BE0
//   1ACB7C race1B0 bit3 = 1 (tail; 1A7BD0 fades the restarted scene in)
//   1ACBA0.. per human 1AF330 / 1750A0 (camera racer binding)
//   a1 != 0: 175430(race48), 1A9750(race,1); a1 == 0: 1AFD80(B4), VCR 287B60/287C80
//   1ACDF0 1983A0 (raceD4), 1ACE04 194CC0 (raceD0)
// The step enum of common_race_start50cr.h (W0) labels these; the calls below
// run in the original order above (w0-requests\r9.txt asks W0 to reorder the
// enum to match; behaviour does not depend on it).
//
// Race-internal state is reached through `friend class ::common_race_start50CR::Owner;`
// (R1, original_race_constructor50la.h). Port-side racer/camera owners are
// reached only through Hooks (I3). Nothing writes guest memory.
#include "common_race_start50cr.h"
#include "common_roster50cr.h"
#include "common_lap50cr.h"
#include "native_common_results50pd.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_race_hud_scope50la.h"
#include "../../platform/linux/native/original_application_aural50kg.h"
#include "../../src/game/original_race_music_manager50la.h"
#include <cstdio>
#include <optional>
#include <thread>

namespace common_race_start50CR {
namespace {
using Race=rr::game::OriginalRaceConstructor50LA;

struct Run final {
    Receipt& out;
    const Request& request;
    bool fail(Step s,const char* why)noexcept {
        out.ok=false;out.reached=s;out.refusedPc=stepPc(s);out.reason=why;return false;
    }
    void enter(Step s)noexcept {out.reached=s;}
};

void logStart(const Request& request,const Receipt& r)noexcept {
    common_race_log50CR::RaceStart line;
    line.restart=request.reason==Reason::restart;line.a1=request.a1;line.caller=request.caller;
    line.bestLapCB4=r.bestLapCB4;line.racers=static_cast<std::uint32_t>(r.racers);
    line.ok=r.ok;line.refusedPc=r.ok?0u:r.refusedPc;line.reason=r.reason?r.reason:"";
    try{common_race_log50CR::raceStart(line);}catch(...){}
}

// Per-racer leaves of 1AEA70 (every racer of the field, humans first, 1AE280 order).
bool racers1AEA70(Run& run,const Hooks& hooks,std::size_t racers)noexcept {
    run.enter(Step::racers1AEA70);
    if(!hooks.resetRacerTiming216690)return run.fail(Step::racers1AEA70,"216690 timing reset owner absent");
    if(!hooks.resetRacerDamage217420)return run.fail(Step::racers1AEA70,"217420 damage reset owner absent");
    for(std::size_t slot=0;slot<racers;++slot){
        // 216690 resets CB0/CB8/CC0 and keeps the best lap CB4 when 11258==3
        // (lap_timing50id.h:96): the rule is the hook owner's, the subtype is ours.
        if(!(*hooks.resetRacerTiming216690)(slot,run.request.mode11258))
            return run.fail(Step::racers1AEA70,"216690 racer timing reset refused");
        if(!(*hooks.resetRacerDamage217420)(slot))
            return run.fail(Step::racers1AEA70,"217420 racer damage reset refused");
    }
    return true;
}
bool grid2A2880(Run& run,const Hooks& hooks,std::size_t racers)noexcept {
    run.enter(Step::grid2A2880);
    if(!hooks.placeOnGrid2A2880)return run.fail(Step::grid2A2880,"2A2880 grid placement owner absent");
    for(std::size_t slot=0;slot<racers;++slot)
        if(!(*hooks.placeOnGrid2A2880)(slot))return run.fail(Step::grid2A2880,"2A2880 grid placement refused");
    return true;
}
bool lock1AED30(Run& run,const Hooks& hooks)noexcept {
    run.enter(Step::startLock1AED30);
    if(!hooks.lockStart1AED30)return run.fail(Step::startLock1AED30,"1AED30 start lock owner absent");
    if(!(*hooks.lockStart1AED30)())return run.fail(Step::startLock1AED30,"1AED30 start lock refused");
    return true;
}
bool overlay197040(Run& run,const Hooks& hooks)noexcept {
    run.enter(Step::startOverlay197040);
    if(!hooks.showStartOverlay197040)return run.fail(Step::startOverlay197040,"197040 start overlay owner absent");
    if(!(*hooks.showStartOverlay197040)())return run.fail(Step::startOverlay197040,"197040 start overlay refused");
    return true;
}
} // namespace

bool Owner::bind(const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race,
    common_roster50CR::Owner& roster,common_lap50CR::Tracker& laps)noexcept {
    if(busy_||!race||bound())return false;
    race_=race;roster_=&roster;laps_=&laps;return true;
}
void Owner::unbind()noexcept {
    if(busy_)return;
    race_.reset();roster_=nullptr;laps_=nullptr;
}

bool Owner::begin(const Request& request,const Hooks& hooks,Receipt& out)noexcept {
    out=Receipt{};
    Run run{out,request};
    const auto race=race_.lock();
    const bool restart=request.reason==Reason::restart;
    if(busy_){run.fail(Step::triggerReset2DCC70,"re-entrant race start refused");logStart(request,out);return false;}
    if(!race||!roster_||!laps_||!roster_->built()){
        run.fail(restart?Step::triggerReset2DCC70:Step::grid2A2880,"race start not bound to a counted race and roster");
        logStart(request,out);return false;
    }
    busy_=true;
    struct Release{bool& b;~Release(){b=false;}} release{busy_};
    out.racers=roster_->count();
    auto& r=*race;
    if(application_.sourceRace28COwner_!=race||!r.counted_||r.rolledBack_||!r.world74_){
        run.fail(restart?Step::triggerReset2DCC70:Step::grid2A2880,"race start requires the Application's own counted race28C and its World74");
        logStart(request,out);return false;
    }
    if(r.resultsFeedbackThread_!=std::this_thread::get_id()){
        run.fail(restart?Step::triggerReset2DCC70:Step::grid2A2880,"race start requires the race construction thread");
        logStart(request,out);return false;
    }
    const auto bit4=(r.flags1B0KnownMask_&0x10u)&&(r.flags1B0KnownValue_&0x10u); // race1B0 bit4 (replay)
    const auto done=[&](bool ok)noexcept {
        if(ok){out.ok=true;out.reached=Step::done;out.refusedPc=0;out.reason=nullptr;}
        logStart(request,out);return ok;
    };
    if(!restart){
        // Fresh: 1A6AA0 already placed and loaded every racer (2.1 steps 1-13);
        // the shared tail: 2A2880 grid, 1AED30 start lock, 197040, race94.
        if(!grid2A2880(run,hooks,out.racers)||!lock1AED30(run,hooks)||!overlay197040(run,hooks))return done(false);
        run.enter(Step::countdown94);
        r.field94_=4.f; // race94 countdown seed (constructor 4.0, 1ACA98)
        return done(true);
    }

    // ---- 1AC8E0 restart body -------------------------------------------------
    // 1AC900 World74+3D0 2DCC70; 161F60 and 2702E0 run through the same one
    // world hook. PORT STAND-IN (flagged, existing): the track owners perform
    // the three World74 leaves together at this first position.
    run.enter(Step::triggerReset2DCC70);
    {
        const char* why=nullptr;auto& world=native_common_results50PD::state().restartWorld;
        if(!world)return run.fail(Step::triggerReset2DCC70,"World74 restart owner (2DCC70/161F60/2702E0) absent"),done(false);
        if(!world(why))return run.fail(Step::triggerReset2DCC70,why?why:"World74 restart refused"),done(false);
    }
    // 1AC914..938: the same App11C word; nonzero becomes zero, zero is not stored.
    run.enter(Step::app11CClear);
    {
        std::uint32_t pause=0;
        if(!application_.animationClock().readPause11C(pause))
            return run.fail(Step::app11CClear,"App11C owner unavailable"),done(false);
        if(pause)application_.setExplicitAnimationPause(false);
    }
    // 1AC944..950: 103210 on App+0 (the Application Aural restart list).
    run.enter(Step::aural103210);
    {
        const auto& aural=application_.sourceAural00Owner_;
        if(aural){
            std::optional<std::int32_t> index;std::optional<std::uint32_t> count;
            const rr::game::OriginalWreckComponent50LA* emitter=nullptr;
            rr::platform::OriginalApplicationAural50KG::Diagnostic ad;
            if(!rr::platform::OriginalApplicationAural50KG::restartOwnedList103210(application_,r.application_,
                index,count,emitter,ad))
                return run.fail(Step::aural103210,"103210 Aural restart list refused"),done(false);
        } // App+0 null: the original's beqz 1AC948 skips 103210.
    }
    // 1AC960 1A4AB0(race10C, 1.0).
    run.enter(Step::music1AC960);
    if(r.music10C_){
        const auto scope=r.endHud_.lock();rr::game::OriginalRaceMusicManagerDiagnostic50LA md;
        if(!scope)return run.fail(Step::music1AC960,"race HUD/music scope absent for 1AC960"),done(false);
        if(!r.music10C_->restartNativeFade1AC960(r,*scope,md))
            return run.fail(Step::music1AC960,md.operation?md.operation:"1AC960 music fade refused"),done(false);
    }else{
        // PORT STAND-IN (flagged, existing): a race without its 10C music
        // manager keeps the native race music owner; restartWorld restored it.
        std::fprintf(stderr,"race_start_step pc=1AC960 music10C=absent stand_in=1\n");
    }
    run.enter(Step::particles161F60);run.enter(Step::world2702E0); // ran in the world hook above
    run.enter(Step::race90);
    r.field90_=0; // 1AC994
    // 1AC9A0 1AEA70 (per racer; its 2A2880 placement is the grid hook), 1AC9A8 1AED30.
    if(!racers1AEA70(run,hooks,out.racers)||!grid2A2880(run,hooks,out.racers)||!lock1AED30(run,hooks))return done(false);
    if(!bit4){
        // 1ACA94..1ACAB8: race94 = 4.0, race1B0 bit2 clear. The 2AED60(raceFC,500)
        // fade is the race's own 1A7BD0 fade-in after bit3 below (PORT STAND-IN,
        // flagged: the 500 ms raceFC fade is not a separate native call here).
        run.enter(Step::countdown94);
        r.field94_=4.f;
        r.flags1B0KnownValue_=std::uint8_t(r.flags1B0KnownValue_&~0x04u);
        r.flags1B0KnownMask_=std::uint8_t(r.flags1B0KnownMask_|0x04u);
    }
    // 1ACD70..: VCR channels (287B60 / 287C80) on every restart with race78.
    run.enter(Step::vcrReset);
    if(!hooks.resetVcrChannels)return run.fail(Step::vcrReset,"VCR channel reset owner absent"),done(false);
    if(!(*hooks.resetVcrChannels)())return run.fail(Step::vcrReset,"VCR channel reset refused"),done(false);
    // 1ACB18: Time Trial (11258==3) with race78 and bit4 clear: 287BE0,
    // raceA0 = 0, B0+40 virtual+18 (record actor / ghost backup).
    if(request.mode11258==3u&&!bit4){
        run.enter(Step::recordBackup288630);
        if(!hooks.backupRecordActor288630)return run.fail(Step::recordBackup288630,"288630 record actor owner absent"),done(false);
        if(!(*hooks.backupRecordActor288630)())return run.fail(Step::recordBackup288630,"288630 record actor backup refused"),done(false);
        r.fieldA0_=0;
    }
    // 1ACB74 267BE0: PORT STAND-IN (flagged): the Aural transport flush is
    // admitted natively only after a Results 26D070 stop; the live race output
    // keeps its transport here.
    // 1ACB7C: race1B0 bit3 = 1 (both paths), so 1A7BD0 fades the scene back in.
    run.enter(Step::tail1ACB7C);
    r.flags1B0KnownValue_=std::uint8_t(r.flags1B0KnownValue_|0x08u);
    r.flags1B0KnownMask_=std::uint8_t(r.flags1B0KnownMask_|0x08u);
    // 1ACBA0.. per human 1AF330 / 1750A0; a1 != 0 adds 175430 / 1A9750(race,1).
    run.enter(Step::cameraReset);
    if(!hooks.resetCamera)return run.fail(Step::cameraReset,"camera session reset owner absent"),done(false);
    if(!(*hooks.resetCamera)())return run.fail(Step::cameraReset,"camera session reset refused"),done(false);
    if(!overlay197040(run,hooks))return done(false);
    // This race's next finish runs a fresh 1A7DD0 invocation (the suspended
    // native cursor of the previous one is retired; R1 owns the reset).
    r.resetEndInvocation1A7DD0();
    {
        auto& common=native_common_results50PD::state();
        common.endActive=false;common.endLogged=false;common.endRefusedLogged=false;common.fallback=false;
        common.acknowledged=false;common.routeChosen=false;common.replayActive=false;
        common.endCalls=0;common.lastPC=0;common.lastStatus=0xffu;common.lastOperation=nullptr;
    }
    return done(true);
}
} // namespace common_race_start50CR

namespace original_main_entry50KG {
bool Application::bindRaceStart50CR(const std::shared_ptr<common_race_start50CR::Owner>& start)const noexcept {
    if(garageAudioOwnerThread_!=std::this_thread::get_id())return false;
    if(start&&!start->bound())return false;
    raceStart50CR_=start;return true;
}
bool Application::restartConsumedRace1AC8E0(rr::game::OriginalRaceConstructor50LA& race,
    const common_race_start50CR::Request& request,const common_race_start50CR::Hooks& hooks,
    common_race_start50CR::Receipt& out)const noexcept {
    out={};
    const auto start=raceStart50CR_.lock();
    if(garageAudioOwnerThread_!=std::this_thread::get_id()||!start||!start->bound()||
       sourceRace28COwner_.get()!=&race||request.reason!=common_race_start50CR::Reason::restart){
        out.ok=false;out.refusedPc=0x1AC8E0u;out.reason="1AC8E0 requires the bound race start of this Application's race28C";
        common_race_log50CR::RaceStart line;line.restart=true;line.a1=request.a1;line.caller=request.caller;
        line.ok=false;line.refusedPc=0x1AC8E0u;line.reason=out.reason;
        try{common_race_log50CR::raceStart(line);}catch(...){}
        return false;
    }
    return start->begin(request,hooks,out);
}
} // namespace original_main_entry50KG
