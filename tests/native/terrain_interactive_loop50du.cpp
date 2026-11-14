// The original active host loop, separated without changing its lifetime owners.
#include "native_last_failure50pd.h"
#include "native_wreck_debris_sim50pd.h"
#include "native_wreck_debris_gpu50pd.h"
#include "native_common_results50pd.h"
#include "native_race_replay50pd.h"
#include "../../platform/linux/native/test_results_input50pd.h"
#include "../../src/game/original_race_end_entry50la.h"
#include <cfenv>
#include "../../platform/linux/native/stall_watchdog50du.h"
#include "../../platform/linux/native/window_swap_stats50au.h"
#include "../../src/game/ps2_float_mode50la.h"
#include "../../src/game/native_destroyed_racers50pd.h"
#include "../../src/game/native_test_undertrack50ut.h"
#include "../../src/game/collision_mesh_admission50cc.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <functional>
#include <xmmintrin.h>
#include "terrain_interactive_loop50du.h"
#include "original_race_f8_native_bars50la.h"
#include "original_results_native_frame50la.h"
#include "../../src/game/original_race_f8_50la.h"
#include "../../src/game/original_results104_50la.h"
#include <chrono>
#include <optional>
#include <sstream>

// Diagnostic-only GPU vertex check switch, defined in vehicle_draw50cm.cpp.
namespace rr::game { extern bool vehicleDrawSkinReference50CM; }

namespace interactive_loop50DU {
namespace recovery50FI=checkpoint_recovery50fi_fixture;
using NativeRacerFramePart=interactive_frame50DU::Part;
namespace {
std::uint64_t replayFrames50PD=0;bool replayCameraReported50PD=false;std::uint32_t replayRecordLogged50PD=0,replayResultsPC50PD=0xffffffffu; //Results replay log cadence.
const char* replayDrawReported50PD=nullptr;bool replayDrawLogged50PD=false;unsigned replayF8Reported50PD=0;
// Always-on hitch recorder: four clock reads per frame. A frame slower than
// 100 ms writes one line splitting its wall time at the existing phase
// boundaries (input/audio, simulation, render submit, completion/swap).
struct FrameHitch50DU {
    using Clock = std::chrono::steady_clock;
    Clock::time_point start{}, physics{}, render{}, completion{};
    bool started = false;
};
FrameHitch50DU frameHitch50DU;
float testRouteX50DU=0.f; // Last HUD gate direction; unattended steering test only.
float testPositionX50DU=0.f,testPositionZ50DU=0.f; // Last player position; unattended unstick test only.
double hitchMs50DU(FrameHitch50DU::Clock::time_point a, FrameHitch50DU::Clock::time_point b) {
    if (a == FrameHitch50DU::Clock::time_point{} || b == FrameHitch50DU::Clock::time_point{} || b < a) return -1.0;
    return std::chrono::duration<double, std::milli>(b - a).count();
}
void require50DU(bool ok, const char* message) {
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
}
void addCounter50DU(std::uint64_t& counter, std::uint64_t amount = 1) noexcept {
    const auto room = std::numeric_limits<std::uint64_t>::max() - counter;
    counter += std::min(room, amount);
}
// Exact race frame recorder (measurement only; host memory, no game state).
// A frame is the wall time between two loop frame boundaries (the frame_hitch
// point) across which the gameplay window_swaps counter advanced, so loop
// iterations that did not present (fade waits, pauses) fold into the frame
// that did. Frames are recorded while race.phase is Racing; one
// race_fps_summary line is printed when Racing ends (or the loop exits).
// Camera fade presents are not gameplay swaps; they are counted separately
// from WindowHost50DQ::swap (window_swap_stats50au.h) as fade_swaps.
struct RaceFrameRecorder50AU {
    static constexpr std::size_t capacity=65536; // about 17 minutes at 62 FPS
    std::array<float,capacity> frameMs{};
    std::array<float,capacity> scratch{};
    std::size_t count=0;
    bool full=false,active=false;
    double totalMs=0.0,worstMs=0.0;
    std::uint64_t catchupSteps=0,discardedAtStartNs=0;
    std::uint64_t fadeSwaps=0;double fadeMaxGapMs=0.0;
    bool marked=false;FrameHitch50DU::Clock::time_point markTime{};
    std::uint64_t markSwaps=0,markTicks=0;
    // window_submit_fps interval: <=17/<=20/<=25/<=33/<=50/>50 ms, all phases.
    std::array<std::uint64_t,6> buckets{};double intervalWorstMs=0.0;
    std::uint32_t racesReported=0; // process-wide race ordinal
};
RaceFrameRecorder50AU raceFrames50AU;
// Step split (measurement only; host memory, no game state). Always on: two
// steady_clock reads per existing simulationTotals scope. A step runs from the
// start of one scheduled_total scope to the start of the next, so the scopes
// that follow a step and are not nested in it (racer contacts, race field,
// presentation) count toward that step. The slowest step of each 5 s window
// and its split are printed on the window_submit_fps line, for every course.
struct StepSplit50GG {
    static constexpr std::size_t parts=14,totalIndex=13,none=parts;
    const phase_timing50GK::Totals* base=nullptr; // run()'s simulationTotals[parts]
    std::array<double,parts> current{},worst{};
    double worstTotal=0.0,sumTotal=0.0;
    std::uint64_t steps=0;bool open=false;
    std::size_t indexOf(const phase_timing50GK::Totals* p) const noexcept {
        if(!base||p<base||p>=base+parts)return none;
        return static_cast<std::size_t>(p-base);
    }
    void add(std::size_t i,FrameHitch50DU::Clock::time_point a,FrameHitch50DU::Clock::time_point b) noexcept {
        if(i>=parts||!open||b<a)return; // bounds check before the write
        current[i]+=std::chrono::duration<double,std::milli>(b-a).count();
    }
    void endStep() noexcept {
        if(open){
            const double total=current[13]+current[10]+current[11]+current[12];
            ++steps;sumTotal+=total;
            if(total>worstTotal){worstTotal=total;worst=current;}
        }
        current={};open=true;
    }
    void resetWindow() noexcept {worst={};worstTotal=0.0;sumTotal=0.0;steps=0;}
};
StepSplit50GG stepSplit50GG;
void resetRaceFrameSession50AU() noexcept {
    auto& r=raceFrames50AU;
    r.count=0;r.full=false;r.active=false;r.totalMs=0.0;r.worstMs=0.0;r.catchupSteps=0;r.discardedAtStartNs=0;
    r.fadeSwaps=0;r.fadeMaxGapMs=0.0;r.marked=false;r.markTime={};r.markSwaps=0;r.markTicks=0;
    r.buckets={};r.intervalWorstMs=0.0;
}
void noteFadeSwaps50AU(std::uint64_t swaps,double maxGapMs) noexcept {
    auto& r=raceFrames50AU;
    addCounter50DU(r.fadeSwaps,swaps);
    if(maxGapMs>r.fadeMaxGapMs)r.fadeMaxGapMs=maxGapMs;
}
void reportRaceFrames50AU(const char* track,std::uint64_t discardedNs) {
    auto& r=raceFrames50AU;
    const std::size_t n=r.count;
    const double avgFps=n&&r.totalMs>0.0?double(n)*1000.0/r.totalMs:0.0;
    // Slowest window of consecutive frames lasting at least 5 s. A race
    // shorter than 5 s reports its whole-race average instead.
    double min5s=avgFps;bool anyWindow=false;
    {double sum=0.0;std::size_t j=0;
     for(std::size_t i=0;i<n;++i){
      while(j<n&&sum<5000.0)sum+=r.frameMs[j++];
      if(sum<5000.0)break;
      const double fps=double(j-i)*1000.0/sum;
      if(!anyWindow||fps<min5s){min5s=fps;anyWindow=true;}
      sum-=r.frameMs[i];
     }}
    // 1% / 0.1% lows: FPS over the mean of the slowest 1% / 0.1% of frames.
    double low1=0.0,low01=0.0;
    if(n){
     std::copy_n(r.frameMs.begin(),n,r.scratch.begin());
     const std::size_t k1=std::max<std::size_t>(1,n/100),k01=std::max<std::size_t>(1,n/1000);
     auto* first=r.scratch.data();
     std::nth_element(first,first+(k1-1),first+n,std::greater<float>());
     double sum1=0.0;for(std::size_t i=0;i<k1;++i)sum1+=first[i];
     std::nth_element(first,first+(k01-1),first+k1,std::greater<float>());
     double sum01=0.0;for(std::size_t i=0;i<k01;++i)sum01+=first[i];
     low1=sum1>0.0?double(k1)*1000.0/sum1:0.0;low01=sum01>0.0?double(k01)*1000.0/sum01:0.0;
    }
    const double droppedMs=discardedNs>=r.discardedAtStartNs?double(discardedNs-r.discardedAtStartNs)/1.0e6:0.0;
    char line[512];
    std::snprintf(line,sizeof line,"race_fps_summary track=%s race=%u frames=%zu avg_fps=%.1f min_5s_fps=%.1f"
        " low1_fps=%.1f low01_fps=%.1f worst_ms=%.1f catchup_steps=%llu dropped_ms=%.1f fade_swaps=%llu"
        " fade_max_gap_ms=%.1f recorder_full=%d\n",track?track:"unknown",++r.racesReported,n,avgFps,min5s,low1,low01,
        r.worstMs,static_cast<unsigned long long>(r.catchupSteps),droppedMs,static_cast<unsigned long long>(r.fadeSwaps),
        r.fadeMaxGapMs,r.full?1:0);
    std::cerr<<line;
    rr::game::reportCollisionAdmission50CC("race_end"); // Step A counters, same moment
    r.active=false;r.count=0;r.full=false;r.totalMs=0.0;r.worstMs=0.0;r.catchupSteps=0;
    r.fadeSwaps=0;r.fadeMaxGapMs=0.0; // the next race's fades start fresh
}
// Called once per loop iteration at the frame_hitch point.
void observeFrameBoundary50AU(FrameHitch50DU::Clock::time_point now,std::uint64_t swaps,std::uint64_t ticks,
    bool racing,std::uint64_t discardedNs,const char* track) {
    auto& r=raceFrames50AU;
    if(!r.marked){r.marked=true;r.markTime=now;r.markSwaps=swaps;r.markTicks=ticks;}
    else if(swaps!=r.markSwaps){
     const std::uint64_t frames=swaps>r.markSwaps?swaps-r.markSwaps:1;
     const std::uint64_t ticksDone=ticks>=r.markTicks?ticks-r.markTicks:0;
     const double totalMs=hitchMs50DU(r.markTime,now);
     const double perFrame=totalMs>=0.0?totalMs/double(frames):0.0;
     const unsigned bucket=perFrame<=17.0?0u:perFrame<=20.0?1u:perFrame<=25.0?2u:perFrame<=33.0?3u:perFrame<=50.0?4u:5u;
     addCounter50DU(r.buckets[bucket],frames);
     if(perFrame>r.intervalWorstMs)r.intervalWorstMs=perFrame;
     if(racing&&r.active){
      bool stored=true;
      for(std::uint64_t f=0;f<frames;++f){
       if(r.count>=RaceFrameRecorder50AU::capacity){ // bounds check before the write
        if(!r.full){r.full=true;std::cerr<<"race_fps_recorder_full capacity="<<RaceFrameRecorder50AU::capacity
            <<" track="<<(track?track:"unknown")<<" (recording stopped for this race)\n";}
        stored=false;break;
       }
       r.frameMs[r.count++]=static_cast<float>(perFrame);r.totalMs+=perFrame;
       if(perFrame>r.worstMs)r.worstMs=perFrame;
      }
      if(stored&&ticksDone>frames)addCounter50DU(r.catchupSteps,ticksDone-frames);
     }
     r.markTime=now;r.markSwaps=swaps;r.markTicks=ticks;
    }
    if(racing&&!r.active){
     r.active=true;r.count=0;r.full=false;r.totalMs=0.0;r.worstMs=0.0;r.catchupSteps=0;r.discardedAtStartNs=discardedNs;
    }else if(!racing&&r.active)reportRaceFrames50AU(track,discardedNs);
}
} // namespace

void Context::run() {
 std::uint32_t resultsInputStopPC50LA=0; //Native diagnostic suppression, not source/input state.
 resetRaceFrameSession50AU(); // measurement only; one recorder per race session
 auto sampleController=[&](bool advanceFeedback=false){stall_watchdog50DU::mark("sample_controller");return sampleControllerCall(advanceFeedback);};
 auto composePacket=[&](bool live,const rr::platform::WindowInput50DQ& sampled,std::size_t actualRacerSlot=0){stall_watchdog50DU::mark("compose_packet");return composePacketCall(live,sampled,actualRacerSlot);};
 auto applyCourseInput=[&](std::size_t racerSlot=0){stall_watchdog50DU::mark("apply_course_input");return applyCourseInputCall(racerSlot);};
 auto observeOriginalAiFleet=[&](const original_all8_frame::Schedule* schedule=nullptr){return observeOriginalAiFleetCall(schedule);};
 auto advanceOriginalAiManager=[&](float factor,const original_all8_frame::Schedule* schedule=nullptr){return advanceOriginalAiManagerCall(factor,schedule);};

 auto runNative=[&](auto&&fn,const char*label){
  // Native arithmetic requires round-to-nearest (set once at session start).
  // Host drivers/libraries may change the thread's FP mode between native
  // calls; restore it and record each repair instead of failing a later leaf.
  if(const int rounding=std::fegetround();rounding!=FE_TONEAREST){
   std::cerr<<"fp_rounding_repaired before="<<label<<" mode="<<rounding<<'\n';
   require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest restore failed");
  }
  if(rr::game::enforcePs2FloatMode50LA())
   std::cerr<<"fp_flush_mode_repaired before="<<label<<'\n';
  stall_watchdog50DU::mark(label);
  const bool ok=fn();addCounter50DU(nativeCalls);
  if(!ok){std::ostringstream message;message<<"native frame "<<completedFrames
   <<" "<<label<<" error="<<static_cast<unsigned>(x.diagnostic.error)
   <<" source-stage="<<std::hex<<x.diagnostic.sourceStage
   <<" body="<<static_cast<unsigned>(x.diagnostic.body);
   throw std::runtime_error(message.str());}
 };

 // Optional nested observations; all entries are subsets of physics, never
 // additional work to add to its total. No allocation or clock reads disabled.
 phase_timing50GK::Totals simulationTotals[14]{};
 static_assert(sizeof simulationTotals/sizeof simulationTotals[0]==StepSplit50GG::parts,"step split layout");
 stepSplit50GG=StepSplit50GG{};stepSplit50GG.base=simulationTotals;
 struct BaseRelease {~BaseRelease(){stepSplit50GG.base=nullptr;}} stepSplitBaseRelease; // no dangling base after run()
 struct SimulationScope {
  phase_timing50GK::Totals* total;
  phase_timing50GK::Sample before{};
  std::size_t splitIndex;FrameHitch50DU::Clock::time_point splitStart;
  SimulationScope(bool enabled,phase_timing50GK::Totals& value):total(enabled?&value:nullptr),
      splitIndex(stepSplit50GG.indexOf(&value)){
   if(splitIndex==StepSplit50GG::totalIndex)stepSplit50GG.endStep();
   splitStart=FrameHitch50DU::Clock::now();
   if(total)before=phase_timing50GK::readClocks();
  }
  ~SimulationScope(){if(total)total->record(before,phase_timing50GK::readClocks());
   stepSplit50GG.add(splitIndex,splitStart,FrameHitch50DU::Clock::now());}
 };
 auto advanceOriginalAll8TwoPhaseFrame=[&](original_all8_frame::Schedule& schedule,
     original_all8_frame::AudioIntegrationPolicy audioPolicy,
     auto&& remainingRaceChildren){
  SimulationScope totalTiming(phaseTiming.enabled,simulationTotals[13]);
  require50DU(originalAiEnabled&&nativeOriginalFleet&&originalAiHumanCount==(humanCourseEnabled?1u:0u)&&
      racerBank.count()==raceRacerCount,"two-phase entry requires the actual native roster and source manager");
  // The native AI policy services real D40/Aural owners without certifying
  // the missing intervening retail children or their complete RNG chronology.
  using AudioPolicy=original_all8_frame::AudioIntegrationPolicy;
  require50DU(audioPolicy==AudioPolicy::requireHostAudioDisabled||
      audioPolicy==AudioPolicy::retainHostSelectedPlayerDerivedSuffixPolicy||
      audioPolicy==AudioPolicy::nativeAiD40AtNativeAudioPhase,
      "two-phase audio integration policy is not recognized");
  require50DU(audioPolicy!=AudioPolicy::requireHostAudioDisabled||!audioEnabled,
      "explicit two-phase policy requires host audio disabled");
  require50DU((audioPolicy==AudioPolicy::nativeAiD40AtNativeAudioPhase)==originalAiAudioEnabled,
      "two-phase audio policy must match the actual bound audio owners");
  require50DU(schedule.begin(.016f,60.f),"two-phase outer frame admission rejected");
  struct RestoreOpponentFrame {bool& value;bool prior;~RestoreOpponentFrame(){value=prior;}};
  RestoreOpponentFrame restore{opponentFrame,opponentFrame};
  while(schedule.stage()==original_all8_frame::Stage::RacerChildren){
   if(!schedule.advanceNextChildren(
       [&](std::size_t,std::size_t slot){
        {SimulationScope timing(phaseTiming.enabled,simulationTotals[0]);observeOriginalAiFleet(&schedule);}
        if(dynamicContactEnabled){SimulationScope timing(phaseTiming.enabled,simulationTotals[1]);projectDynamicActorsForSlot(slot);}
        opponentFrame=slot!=0;return true;
       },[&](std::size_t,std::size_t slot){
        SimulationScope timing(phaseTiming.enabled,simulationTotals[2]);
        return advanceFramePart(slot,NativeRacerFramePart::Children,.016f,60.f);
       }))return false;
  }
  opponentFrame=false;
  if(humanCourseEnabled){
   // Source1A15EC is the sole human's packing publication. All AI prefixes
   // precede that Player in1AD684/1AD6BC order. Publish the original FF8
   // writes before any remaining child/manager rank pass, preserving the
   // previous CCC inputs and the next AI frame's genuine reference owners.
   observeOriginalAiFleet(&schedule);updateOriginalAiPacking();
  }
  {SimulationScope timing(phaseTiming.enabled,simulationTotals[3]);
   if(!schedule.advanceRemainingChildren(std::forward<decltype(remainingRaceChildren)>(remainingRaceChildren)))return false;}
  while(schedule.stage()==original_all8_frame::Stage::Physics){
   if(!schedule.advanceNextPhysics(
       [&](std::size_t,std::size_t slot){
        SimulationScope timing(phaseTiming.enabled,simulationTotals[4]);
        captureDynamicBeforeScheduledPhysics(schedule,slot);opponentFrame=slot!=0;return true;
       },[&](std::size_t,std::size_t slot,float factor,float scale){
        SimulationScope timing(phaseTiming.enabled,simulationTotals[5]);
        return advanceFramePart(slot,NativeRacerFramePart::Physics,factor,scale);
       }))return false;
  }
  require50DU(schedule.stage()==original_all8_frame::Stage::Complete,
      "two-phase outer frame stopped before physics completion");
  {SimulationScope timing(phaseTiming.enabled,simulationTotals[6]);requireOriginalAiFleetReady();}return true;
 };

 const auto withRace=[&](auto&& operation)->decltype(auto){
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  if(isMcTrack){requireMbRuntimeNavigation50MB();return operation(mcRace);}
  if(isGbTrack){requireMbRuntimeNavigation50MB();return operation(gbRace);}
  if(isTbTrack){requireMbRuntimeNavigation50MB();return operation(tbRace);}
  if(isTcTrack)return operation(tcRace);
  if(isMbTrack)return operation(mbRace);return operation(legacyRace);
 };

  for(;;){
   // A suspended source input call still owns its original poll. Do not
   // dispatch fresh input, run simulation or commit tournament work around it.
   if(originalMainApplication.pauseReturnInputPending50LA()){
    discardTrackBaseline();bool closeRequested=false;
    require50DU(host.pollLoading50DU(closeRequested,hostDiagnostic),"Pause retained input close pump failed");
    hostClock.pause(monotonicNow50DU());
    if(closeRequested)break; //Publication guard cancels backend before event storage unwinds.
    std::uint32_t pc=0;
    require50DU(originalMainApplication.advanceNativePauseReturn50LA(pc),
        "Pause retained return failed with original effects and event custody retained");
    captureCommitted();continue; //No fresh input/world step, including the successful return iteration.
   }
   bool pauseMenuDeparture=false;
   require50DU(originalMainApplication.beginNativePauseMenuDeparture50LA(pauseMenuDeparture),
       "Pause post-batch Main request consumption rejected");
   if(pauseMenuDeparture)break;
   if(originalMainApplication.resultsDonePending50LA()){
    discardTrackBaseline();bool closeRequested=false;
    require50DU(host.pollLoading50DU(closeRequested,hostDiagnostic),
        "Results retained input close pump failed");
    hostClock.pause(monotonicNow50DU());
    if(closeRequested)break; // Existing teardown cancels backend BEFORE pins.
    require50DU(bool(sourceFadeOut),"Results actual blocking backend absent");
    rr::game::OriginalResults104Diagnostic50LA continuation;
    require50DU(originalMainApplication.advanceResultsDone50LA(*sourceFadeOut,continuation),
        "Results retained input/fade continuation failed with effects retained");
    captureCommitted();
    // Both pending and completed iterations finish here. Completion has
    // consumed the entire retained batch; only the NEXT loop polls afresh.
    if(!originalMainApplication.resultsDonePending50LA())forceSceneRender=true;
    continue;
   }
   bool resultsHallDeparture=false;
   require50DU(originalMainApplication.beginNativeResultsHallDeparture50LA(resultsHallDeparture),
       "Results Hall post-poll request consumption rejected");
   if(resultsHallDeparture)break; // Actual closeWindow below; no fresh input/update.
   commitCompletedTournament();
   {
    {
     static bool floatModeReported50DU=false;
     if(!floatModeReported50DU){floatModeReported50DU=true;
      std::cerr<<"ps2_float_mode game_thread_active_before_check="<<rr::game::ps2FloatModeActive50LA()<<'\n';}
    }
    if(rr::game::enforcePs2FloatMode50LA()){
     static unsigned flushRepairs50DU=0;
     if(flushRepairs50DU++<20)std::cerr<<"fp_flush_mode_repaired before=host_frame\n";
    }
    auto& hitch=frameHitch50DU;const auto now=FrameHitch50DU::Clock::now();
    // Two frame budgets (33 ms); bounded so a sustained slowdown cannot flood.
    static unsigned hitchLines50DU=0;
    if(hitch.started&&hitchMs50DU(hitch.start,now)>33.0&&hitchLines50DU<200&&++hitchLines50DU)
     std::cerr<<"frame_hitch total_ms="<<hitchMs50DU(hitch.start,now)
         <<" input_audio_ms="<<hitchMs50DU(hitch.start,hitch.physics)
         <<" simulation_ms="<<hitchMs50DU(hitch.physics,hitch.render)
         <<" render_submit_ms="<<hitchMs50DU(hitch.render,hitch.completion)
         <<" completion_swap_ms="<<hitchMs50DU(hitch.completion,now)
         <<" tick="<<completedFrames<<'\n';
    observeFrameBoundary50AU(now,windowSwaps,completedFrames,race.phase==race_session50HM::Phase::Racing,
        discardedHostNs,rr::platform::hostTrackCode50IM(selectedTrack));
    hitch={};hitch.start=now;hitch.started=true;
   }
   phase_timing50GK::Scope inputTiming(phaseTiming,phase_timing50GK::Phase::Input);
   const bool traceFirstMainPoll=originalUiActive&&originalMain.traceStages&&
       !firstMainPollReported;
   const bool traceSecondMainPoll=originalUiActive&&originalMain.traceStages&&
       firstMainPollReported&&!secondMainPollReported;
   if(traceFirstMainPoll){
    firstMainPollReported=true;
    std::cerr<<"original_ui_first_poll {\"phase\":\"enter\"}\n";
   }else if(traceSecondMainPoll){
    secondMainPollReported=true;
    std::cerr<<"original_ui_second_poll {\"phase\":\"enter\"}\n";
   }
   stall_watchdog50DU::start();stall_watchdog50DU::mark("frame_poll");
   const bool polled=host.poll(hostInput,hostDiagnostic);
   stall_watchdog50DU::mark("frame_after_poll");
   if(traceFirstMainPoll)
    std::cerr<<"original_ui_first_poll {\"phase\":\"return\",\"okay\":"
        <<(polled?"true":"false")<<"}\n";
   else if(traceSecondMainPoll)
    std::cerr<<"original_ui_second_poll {\"phase\":\"return\",\"okay\":"
        <<(polled?"true":"false")<<"}\n";
   if(!polled){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("window event pump failed");
   }
   {
    // Unattended test only: a player holding pad A (accelerate) through the
    // green light, with the window focused as when a person plays. A is held
    // through the countdown and for 2 s of wall time after green, then let go.
    static const bool padHoldA50DU=[]{const char* v=std::getenv("RR_TEST_PAD_HOLD_A");return v&&std::string(v)=="1";}();
    static std::uint64_t padHoldGreenNs50DU=0;
    if(padHoldA50DU&&captureRun.unattended()){
     hostInput.keys.focused=true;hostInput.keys.activityLost=false;
     auto& pad=hostInput.keys.controllers[0];
     const auto nowNs=monotonicNow50DU();
     if(race.phase==race_session50HM::Phase::Racing&&!padHoldGreenNs50DU)padHoldGreenNs50DU=nowNs;
     const bool hold=race.phase==race_session50HM::Phase::Countdown||
         (padHoldGreenNs50DU&&nowNs-padHoldGreenNs50DU<2000000000ull);
     if(pad.connected){
      if(hold&&!pad.buttons[0])pad.pressed[0]=1;
      if(!hold&&pad.buttons[0])pad.released[0]=1;
      pad.buttons[0]=hold?1:0;
     }
     static bool padHoldReported50DU=false;
     if(!hold&&padHoldGreenNs50DU&&!padHoldReported50DU){padHoldReported50DU=true;std::cerr<<"test_pad_hold_a released\n";}
    }
   }
   {
    // Unattended test only: RR_TEST_RESULTS_CONFIRM=N taps Enter (150 ms)
    // every N seconds once the common-race Results replay is running, as a
    // player confirming the Results pages at a focused window.
    static const float resultsConfirm50PD=[]{const char* v=std::getenv("RR_TEST_RESULTS_CONFIRM");return v?static_cast<float>(std::atof(v)):0.f;}();
    static std::uint64_t confirmStartNs50PD=0;static unsigned confirmTaps50PD=0;
    auto& testResults50PD=rr::platform::testResultsInput50PD();
    testResults50PD.focus=false;
    if(resultsConfirm50PD>0.f&&captureRun.unattended()&&native_common_results50PD::state().replayActive){
     // Applied by the host at its next poll, before the Results observe it.
     testResults50PD.focus=true;
     const auto nowNs=monotonicNow50DU();
     if(!confirmStartNs50PD)confirmStartNs50PD=nowNs;
     const auto period=static_cast<std::uint64_t>(resultsConfirm50PD*1e9f);
     const auto elapsed=nowNs-confirmStartNs50PD;
     const bool on=period&&elapsed>=period&&elapsed%period<150000000ull;
     static bool confirmOn50PD=false;
     if(on&&!confirmOn50PD){testResults50PD.enter=true;
      std::cerr<<"test_results_confirm tap="<<++confirmTaps50PD<<'\n';}
     confirmOn50PD=on;
    }
   }
   {
    // Unattended test only: hold keyboard throttle (W) once the race is green,
    // so the human launch at the start light is exercised like a real player.
    static const bool holdThrottle50DU=[]{const char* v=std::getenv("RR_TEST_HOLD_THROTTLE");return v&&std::string(v)=="1";}();
    // Injected keys stand for a player at a focused window. The unattended
    // window is never focused, and unfocused input releases every mapped key.
    if(holdThrottle50DU&&captureRun.unattended()){hostInput.keys.focused=true;hostInput.keys.activityLost=false;}
    if(holdThrottle50DU&&captureRun.unattended()&&race.phase==race_session50HM::Phase::Racing){
     constexpr std::size_t throttleW=26,steerLeft=80,steerRight=79;
     // RR_TEST_STOP_START=1: 4 s throttle, 3 s coast to a stop, repeating
     // (stop-start driving into walls, as a player recovering from crashes).
     static const bool stopStart50DU=[]{const char* v=std::getenv("RR_TEST_STOP_START");return v&&std::string(v)=="1";}();
     bool throttleOn=!stopStart50DU||((monotonicNow50DU()/1000000000ull)%7ull)<4ull;
     // Steer toward the next authored gate (same vector as the HUD arrow).
     static const float steerSign50DU=[]{const char* v=std::getenv("RR_TEST_STEER_INVERT");return v&&std::string(v)=="1"?-1.f:1.f;}();
     float route=steerSign50DU*testRouteX50DU;
     // RR_TEST_UNSTICK=1: when wedged (moved under 40 units in 3 s) the bot
     // taps the player's manual reset key (T, 150 ms), as a player would.
     static const bool unstick50DU=[]{const char* v=std::getenv("RR_TEST_UNSTICK");return v&&std::string(v)=="1";}();
     static std::uint64_t stuckCheckNs50DU=0,resetUntilNs50DU=0;static float stuckX50DU=0,stuckZ50DU=0;
     if(unstick50DU){
      constexpr std::size_t resetT=23;
      const auto nowNs=monotonicNow50DU();
      if(!stuckCheckNs50DU){stuckCheckNs50DU=nowNs;stuckX50DU=testPositionX50DU;stuckZ50DU=testPositionZ50DU;}
      if(nowNs>=stuckCheckNs50DU+3000000000ull){
       const float dx=testPositionX50DU-stuckX50DU,dz=testPositionZ50DU-stuckZ50DU;
       if(dx*dx+dz*dz<40.f*40.f){resetUntilNs50DU=nowNs+150000000ull;
        std::cerr<<"test_unstick reset x="<<testPositionX50DU<<" z="<<testPositionZ50DU<<'\n';}
       stuckCheckNs50DU=nowNs;stuckX50DU=testPositionX50DU;stuckZ50DU=testPositionZ50DU;
      }
      const bool tap=nowNs<resetUntilNs50DU;
      if(tap&&!hostInput.keys.down[resetT])hostInput.keys.pressed[resetT]=1;
      if(!tap&&hostInput.keys.down[resetT])hostInput.keys.released[resetT]=1;
      hostInput.keys.down[resetT]=tap?1:0;
     }
     if(throttleOn&&!hostInput.keys.down[throttleW])hostInput.keys.pressed[throttleW]=1;
     if(!throttleOn&&hostInput.keys.down[throttleW])hostInput.keys.released[throttleW]=1;
     hostInput.keys.down[throttleW]=throttleOn?1:0;
     const bool left=route< -0.08f,right=route>0.08f;
     for(const auto [key,on]:{std::pair{steerLeft,left},std::pair{steerRight,right}}){
      if(on&&!hostInput.keys.down[key])hostInput.keys.pressed[key]=1;
      if(!on&&hostInput.keys.down[key])hostInput.keys.released[key]=1;
      hostInput.keys.down[key]=on?1:0;
     }
     // RR_TEST_REPAIR=1: hold the player's repair key (R) 2 s of every 6 s,
     // as a player keeping a scraped pod alive (the bot cannot read damage).
     static const bool repair50DU=[]{const char* v=std::getenv("RR_TEST_REPAIR");return v&&std::string(v)=="1";}();
     if(repair50DU){
      constexpr std::size_t repairR=21;
      const bool on=((monotonicNow50DU()/1000000000ull)%6ull)<2ull;
      if(on&&!hostInput.keys.down[repairR])hostInput.keys.pressed[repairR]=1;
      if(!on&&hostInput.keys.down[repairR])hostInput.keys.released[repairR]=1;
      hostInput.keys.down[repairR]=on?1:0;
     }
    }
   }
   if(captureRun.unattended()){
    // RR_TEST_DESTROY=0@N:left|right:reset: tap the manual reset key (T,
    // 150 ms) once, 30 limp frames (0.5 s) after the human's limp entry.
    static std::uint64_t limpResetUntilNs50PD=0;
    if(rr::game::nativeTestLimpResetArmed50PD&&rr::game::nativeDestroyedRacers50PD[0].limping&&
       rr::game::nativeDestroyedRacers50PD[0].limpFrames>=30u){
     rr::game::nativeTestLimpResetArmed50PD=false;limpResetUntilNs50PD=monotonicNow50DU()+150000000ull;
     std::cerr<<"test_limp_reset tap limp_frames="<<rr::game::nativeDestroyedRacers50PD[0].limpFrames<<'\n';
    }
    if(limpResetUntilNs50PD){
     hostInput.keys.focused=true;hostInput.keys.activityLost=false;
     constexpr std::size_t resetT=23;
     const bool tap=monotonicNow50DU()<limpResetUntilNs50PD;
     if(tap&&!hostInput.keys.down[resetT])hostInput.keys.pressed[resetT]=1;
     if(!tap&&hostInput.keys.down[resetT])hostInput.keys.released[resetT]=1;
     hostInput.keys.down[resetT]=tap?1:0;
     if(!tap)limpResetUntilNs50PD=0;
    }
   }
   {
    // Unattended test only: RR_TEST_CAMERA_CYCLE=1 taps the view key (C) 1 s
    // after each green light and every 15 s after, for 150 ms each. A fresh
    // test profile starts in the attached view (preference 5); players' saved
    // profiles may use the follow camera (mode 0), which this reaches through
    // the same keyboard camera-cycle binding a player uses.
    static const bool cameraCycle50DU=[]{const char* v=std::getenv("RR_TEST_CAMERA_CYCLE");return v&&std::string(v)=="1";}();
    static std::uint64_t cycleGreenNs50DU=0,cycleTapNs50DU=0;static unsigned cycleTaps50DU=0;
    if(cameraCycle50DU&&captureRun.unattended()){
     hostInput.keys.focused=true;hostInput.keys.activityLost=false; // As a focused player window.
     constexpr std::size_t viewC=6;
     const auto nowNs=monotonicNow50DU();
     const bool racing=race.phase==race_session50HM::Phase::Racing;
     if(!racing)cycleGreenNs50DU=0;else if(!cycleGreenNs50DU){cycleGreenNs50DU=nowNs;cycleTaps50DU=0;}
     bool hold=false;
     if(racing){
      const auto since=nowNs-cycleGreenNs50DU;
      const std::uint64_t due=1000000000ull+15000000000ull*cycleTaps50DU;
      if(since>=due){cycleTapNs50DU=nowNs;++cycleTaps50DU;
       std::cerr<<"test_camera_cycle tap="<<cycleTaps50DU<<" race_ms="<<since/1000000ull<<"\n";}
      hold=cycleTapNs50DU&&nowNs-cycleTapNs50DU<150000000ull;
     }
     if(hold&&!hostInput.keys.down[viewC])hostInput.keys.pressed[viewC]=1;
     if(!hold&&hostInput.keys.down[viewC])hostInput.keys.released[viewC]=1;
     hostInput.keys.down[viewC]=hold?1:0;
     static bool holdLogged50DU=false;
     if(hold&&!holdLogged50DU)std::cerr<<"test_camera_cycle hold_start focused="<<hostInput.keys.focused
         <<" activity_lost="<<hostInput.keys.activityLost<<" minimized="<<hostInput.keys.minimized<<"\n";
     holdLogged50DU=hold;
    }
   }
   if(captureRun.unattended()&&raceEnabled&&race.phase==race_session50HM::Phase::Racing){
    // Unattended test only (RR_TEST_DESTROY): arm each configured racer once
    // its race time passes; its next frame sets the canonical B9D bit0.
    for(auto& test:rr::game::nativeTestDestroys50PD)
     if(test.slot>=0&&!test.fired&&static_cast<std::size_t>(test.slot)<racerBank.count()&&
        race.raceSeconds>=test.seconds){
      test.fired=true;rr::game::nativeTestDestroyPending50PD[static_cast<std::size_t>(test.slot)]=true;
      rr::game::nativeTestDestroyAttacker50PD[static_cast<std::size_t>(test.slot)]=test.attacker;
      rr::game::nativeTestDestroyWreck50PD[static_cast<std::size_t>(test.slot)]=test.wreck;
      rr::game::nativeTestDestroyLimp50PD[static_cast<std::size_t>(test.slot)]=test.limp;
      if(test.slot==0&&test.limp&&test.limpReset)rr::game::nativeTestLimpResetArmed50PD=true;
      std::cerr<<"test_destroy due slot="<<test.slot<<" race_seconds="<<race.raceSeconds<<'\n';
     }
    // Unattended test only (RR_TEST_UNDERTRACK): arm each configured racer once.
    for(auto& test:rr::game::nativeTestUndertracks50UT)
     if(test.slot>=0&&!test.fired&&static_cast<std::size_t>(test.slot)<racerBank.count()&&
        race.raceSeconds>=test.seconds){
      test.fired=true;rr::game::nativeTestUndertrackPending50UT[static_cast<std::size_t>(test.slot)]=true;
      std::cerr<<"test_undertrack due slot="<<test.slot<<" race_seconds="<<race.raceSeconds<<'\n';
     }
   }
   if(!originalUiActive){
    // Revoke frontend-only button/stick/hold admission while another owner
    // has this poll. Inactive observe reads no device data and emits nothing;
    // place it before a canonical Results retained-input early continuation.
    platform::OriginalUiControllerNavigation50LA::Events ignored;
    require50DU(originalUiControllerNavigation.observe(input,false,ignored)&&
        originalUiSecondControllerNavigation.observe(input,false,ignored,1),
        "inactive frontend input latch retirement rejected");
   }
   //One actual poll, one owner. The original frontend keeps its entire stream.
   //Canonical Results admits only its real current idle Root/sibling path;
   //no caller-supplied event, mode, key or acknowledgment enters the bridge.
   // Native modal ownership is distinct from canonical Pause/shared-popup
   //state. These are the ACTUAL retained host owners and THIS poll's native
   //opening commands, observed before canonical claim or edge retirement.
   //A simultaneous Escape/Menu/Back/Start + Confirm/Dpad belongs wholly to that
   //native menu transaction, never partly to Results and partly to Pause.
   const auto& nativeMenuPad=input.controllers[0];
   const bool nativeMenuPadOpeningIntent=usePhysicalGameController&&input.gameControllersEnabled&&
       nativeMenuPad.connected&&nativeMenuPad.instanceId>=0&&
       // Start opens the menu; Triangle (3) is Back only once a menu is open
       // and otherwise belongs to the original camera view change.
       (nativeMenuPad.pressed[6]!=0||(sessionMenu.open()&&nativeMenuPad.pressed[3]!=0));
   const bool nativeMenuCommand=hostInput.menuRequested||hostInput.pauseRequested||
       hostInput.resumeRequested||hostInput.restartRequested||hostInput.checkpointRequested||
       input.pressed[41]||nativeMenuPadOpeningIntent;
   // The host Results panel is supplied by the genuine finish-countdown
   //owner. Preserve its first appearance/release-to-arm, navigation and live
   //confirmation; canonical Results must not take its same Enter/Dpad first.
   //This native presentation policy is NOT a source finish/20C observation.
   const bool nativeResultsOwnsInput=resultsUi.confirming()||
       ((humanCourseEnabled||allAiEnabled||!isGaTrack)&&humanResultsReady);
   const bool nativeModalOwnsInput=sessionMenu.open()||nativeResultsOwnsInput||
       sourcePauseParentAttached||originalMainApplication.animationPaused()||nativeMenuCommand;
   rr::game::OriginalResults104Diagnostic50LA resultsInputDiagnostic;
   const bool resultsInputOkay=(originalUiActive||nativeModalOwnsInput)?true:
       originalMainApplication.pressConsumedResults1C8C30(host,resultsInputDiagnostic);
   if(native_common_results50PD::state().replayActive&&nativeModalOwnsInput){
    static unsigned reportedModal=0xffffffffu;
    const unsigned modal=unsigned(sessionMenu.open())|unsigned(resultsUi.confirming())<<1|
        unsigned(humanResultsReady)<<2|unsigned(sourcePauseParentAttached)<<3|
        unsigned(originalMainApplication.animationPaused())<<4|unsigned(nativeMenuCommand)<<5;
    if(modal!=reportedModal){reportedModal=modal;
     std::cerr<<"race_replay results_input modal_owner menu="<<(modal&1u)<<" host_confirm="<<((modal>>1)&1u)
         <<" host_results="<<((modal>>2)&1u)<<" pause_parent="<<((modal>>3)&1u)<<" paused="<<((modal>>4)&1u)
         <<" menu_command="<<((modal>>5)&1u)<<'\n';}
   }
   const bool resultsInputClaimed=host.retireResultsEdges50LA(hostInput);
   if(native_common_results50PD::state().replayActive&&(resultsInputDiagnostic.operation||!resultsInputOkay)){
    static const char* reportedInput=nullptr;
    if(resultsInputDiagnostic.operation!=reportedInput){reportedInput=resultsInputDiagnostic.operation;
     std::cerr<<"race_replay results_input okay="<<resultsInputOkay<<" claimed="<<resultsInputClaimed<<" pc="<<std::hex
         <<resultsInputDiagnostic.sourcePC<<std::dec<<" operation="<<(resultsInputDiagnostic.operation?resultsInputDiagnostic.operation:"")
         <<" modal="<<nativeModalOwnsInput<<" ui="<<originalUiActive<<'\n';}
   }
   if(originalMainApplication.resultsDonePending50LA()){
    require50DU(resultsInputClaimed,"Results handoff did not retire exact synchronous output");
    hostClock.pause(monotonicNow50DU());discardTrackBaseline();continue;
   }
   if(resultsInputClaimed&&!resultsInputOkay){
    if(resultsInputStopPC50LA!=resultsInputDiagnostic.sourcePC){
     std::cerr<<"Canonical Results input stopped at source PC "<<std::hex<<resultsInputDiagnostic.sourcePC
      <<std::dec<<": "<<(resultsInputDiagnostic.operation?resultsInputDiagnostic.operation:"unavailable")<<'\n';
     resultsInputStopPC50LA=resultsInputDiagnostic.sourcePC;
    }
   }else resultsInputStopPC50LA=0;
   //Retirement removed ONLY claimed fixed UI controls from this exact output,
   //before host menus/results/driving. Quit/capture/unrelated keys survive;
   //unsupported owner/map or actual native modal ownership leaves fresh
   //controls alone. Prior claimed holds remain quarantined only until neutral,
   //so handback cannot replay an edge. Back/Start intent above consumes nothing:
   //the existing single observeController/poll below still authenticates its
   //physical identity/focus/neutral latch and consumes any accepted edge once.
   inputTiming.finish();
   // A changed or interrupted workload must obtain a fresh uninterrupted
   // warmup. OS focus is observed honestly, even in explicit unattended mode.
   if(!trackBaselineEligible()||(input.activityLost&&!captureRun.unattended())||input.resized||
       hostInput.captureRequested||(windowCapture.enabled()&&input.pressed[69])||
       hostInput.pauseRequested||hostInput.menuRequested||hostInput.restartRequested||
       hostInput.checkpointRequested||input.quitRequested)discardTrackBaseline();
   // Physical Pause navigation is owned by the native menu, not gameplay
   // descriptors. Observe even inactive polls to retire stale device/focus
   // edges; the original frontend and source spectator Pause keep their own
   // input owners. This also supplies D-pad/Confirm/Back, not just Start.
   sessionMenu.observeController(input,!originalUiActive&&!sourcePauseParentAttached&&usePhysicalGameController);
   // The human session owns its keyboard interruption barrier just as the
   // physical controller owns its neutral latch. Preserve the sampled held
   // state until the mapped keys are released, but retire this poll's action
   // edges before Pause, F5, or checkpoint navigation can consume them.
   // Original frontend and spectator Pause retain their separate owners.
   if(!originalUiActive&&!sourcePauseParentAttached&&
      (!input.focused||input.minimized||input.activityLost||input.eventBudgetExhausted)){
    suppressResumeKeys=true;
    input.pressed.fill(0);
    hostInput.restartRequested=false;hostInput.checkpointRequested=false;
   }
   if(hostInput.inputError){terminalInputError=hostInput.inputError;std::cerr<<"stdin input error "<<terminalInputError<<'\n';}
   windowCapture.request(input);
   if(captureRun.deadlineExpired()){
    if(originalUiActive)originalMain.requestQuit();
    else {composePacket(false,input);sampleController();}
    break;
   }
   if(originalUiActive){
    // Own the real input stream before ALL host menus, driving packets,
    // restart/checkpoint paths and world/audio steps. No fallthrough exists.
    if(input.quitRequested){
     originalMain.requestQuit();reportUiState("quit");break;
    }
    capturePending=capturePending||hostInput.captureRequested;
    if(hostInput.pauseRequested)originalMain.setHostPaused(true);
    if(hostInput.resumeRequested)originalMain.setHostPaused(false);
    if(hostInput.restartRequested||hostInput.checkpointRequested)
     std::cerr<<"Original Main stays active: restart/checkpoint has no admitted destination.\n";
    const auto& info=host.info();
    const bool drawable=info.drawableWidth>0&&info.drawableHeight>0&&
        info.drawableWidth<=maximumWidth&&info.drawableHeight<=maximumHeight;
    // Match the standalone frontend and pad adapters: a focus-loss/regain
    // batch must not dispatch keyboard Confirm/Back or advance menu PCM.
    const bool active=drawable&&input.focused&&!input.minimized&&!input.activityLost&&!input.eventBudgetExhausted&&
        !originalMain.hostPaused();
    if(originalMain.traceStages&&!firstMainActivityGateReported){
     firstMainActivityGateReported=true;
     std::cerr<<"original_ui_first_activity_gate {\"drawable_width\":"<<info.drawableWidth
         <<",\"drawable_height\":"<<info.drawableHeight<<",\"drawable\":"<<(drawable?"true":"false")
         <<",\"focused\":"<<(input.focused?"true":"false")
         <<",\"minimized\":"<<(input.minimized?"true":"false")
         <<",\"event_budget_exhausted\":"<<(input.eventBudgetExhausted?"true":"false")
         <<",\"host_paused\":"<<(originalMain.hostPaused()?"true":"false")
         <<",\"active\":"<<(active?"true":"false")<<"}\n";
    }
    // At most eight receipts for the first inactive iteration, plus the two
    // second-poll receipts above. Logging never supplies a source-clock value.
    platform::OriginalUiControllerNavigation50LA::Events menuEvents,secondMenuEvents;
    require50DU(originalUiControllerNavigation.observe(input,active,menuEvents),
        "original frontend physical controller snapshot invalid");
    // Separate physical identity and release-to-arm lifetime for port two.
    // An inactive Controls2 route disarms it; keyboard always remains port zero.
    const auto secondInputRoute=originalMain.coordinator.snapshot().route;
    require50DU(originalUiSecondControllerNavigation.observe(input,active&&
        secondInputRoute.screen==12&&secondInputRoute.page==3,secondMenuEvents,1),
        "original frontend second physical controller snapshot invalid");
    const bool traceFirstMainInactive=originalMain.traceStages&&!active&&!firstMainInactiveReported;
    if(traceFirstMainInactive){
     firstMainInactiveReported=true;
     std::cerr<<"original_ui_first_inactive_step {\"step\":\"sample_clock\",\"phase\":\"enter\"}\n";
    }
    const auto mainClock=originalMain.sampleClock(applicationClock,active);
    if(traceFirstMainInactive){
     std::cerr<<"original_ui_first_inactive_step {\"step\":\"sample_clock\",\"phase\":\"return\"}\n";
     std::cerr<<"original_ui_first_inactive_step {\"step\":\"audio_pause\",\"phase\":\"enter\"}\n";
    }
    originalMain.audioOutput.pause(!active);
    if(traceFirstMainInactive)
     std::cerr<<"original_ui_first_inactive_step {\"step\":\"audio_pause\",\"phase\":\"return\"}\n";
    if(!active){
     // Focus/pause time is not fed back as animation or world catch-up.
     originalMain.lastUpdateNanoseconds=0;
     if(traceFirstMainInactive)
      std::cerr<<"original_ui_first_inactive_step {\"step\":\"capture_committed\",\"phase\":\"enter\"}\n";
     captureCommitted();
     if(traceFirstMainInactive){
      std::cerr<<"original_ui_first_inactive_step {\"step\":\"capture_committed\",\"phase\":\"return\"}\n";
      std::cerr<<"original_ui_first_inactive_step {\"step\":\"sleep_8ms\",\"phase\":\"enter\"}\n";
     }
     std::this_thread::sleep_for(std::chrono::milliseconds(8));
     if(traceFirstMainInactive)
      std::cerr<<"original_ui_first_inactive_step {\"step\":\"sleep_8ms\",\"phase\":\"return\"}\n";
     continue;
    }
    const auto inputRoute=originalMain.coordinator.snapshot().route;
    const bool gameOptionsInput=inputRoute.screen==12&&inputRoute.page==4;
    const bool memoryCardInput=inputRoute.screen==12&&inputRoute.page==5;
    const bool controlsInput=inputRoute.screen==12&&(inputRoute.page==2||inputRoute.page==3);
    const bool riderInput=inputRoute.screen==5&&inputRoute.page==1;
    const bool courseInput=inputRoute.screen==6&&inputRoute.page==1;
    const bool hallInput=inputRoute.screen==11&&inputRoute.page==1;
    const bool tournamentInput=inputRoute.screen==9&&inputRoute.page==1;
    const bool garageInput=inputRoute.screen==13&&inputRoute.page==1;
    // Merge real keyboard/pad edges by source ID so a simultaneous confirm
    // dispatches once. Preserve the current keyboard ordering, including Back
    // after Controls configuration7. Source pages still own all admission.
    if(gameOptionsInput||controlsInput||memoryCardInput||riderInput||courseInput||hallInput||tournamentInput||garageInput){
     menuEvents[0]=menuEvents[0]||input.pressed[80];
     menuEvents[1]=menuEvents[1]||input.pressed[79];
     menuEvents[2]=menuEvents[2]||input.pressed[82]||input.pressed[26];
     menuEvents[3]=menuEvents[3]||input.pressed[81]||input.pressed[22];
    }else{
     menuEvents[2]=menuEvents[2]||input.pressed[82]||input.pressed[80]||input.pressed[26];
     menuEvents[3]=menuEvents[3]||input.pressed[81]||input.pressed[79]||input.pressed[22];
    }
    // Preserve the actual release-armed pad edge before merging keyboard input.
    // Keyboard wins simultaneous confirms, matching its explicit user action.
    const bool courseConfirmFromController=menuEvents[4]&&
        !(input.pressed[40]||input.pressed[88]||input.pressed[44]);
    menuEvents[4]=menuEvents[4]||input.pressed[40]||input.pressed[88]||input.pressed[44];
    menuEvents[5]=menuEvents[5]||((gameOptionsInput||controlsInput||memoryCardInput||riderInput||tournamentInput)&&input.pressed[21]);
    menuEvents[7]=menuEvents[7]||((controlsInput||memoryCardInput)&&input.pressed[43]);
    menuEvents[6]=menuEvents[6]||input.pressed[41]||hostInput.menuRequested;
    for(const unsigned event:{0u,1u,2u,3u,4u,5u,7u,6u})
     if(menuEvents[event]){
      const auto accepted=originalMain.input(static_cast<std::uint8_t>(event),input.secondControllerPresence);
      if(courseInput&&event==4&&accepted==platform::OriginalUiFrontendStatus50KG::routeAccepted)
       originalMainApplication.courseHumanController=courseConfirmFromController;
     }
    // Never merge two physical ports or retag keyboard input as controller one.
    if(secondInputRoute.screen==12&&secondInputRoute.page==3&&
       input.secondControllerPresence==platform::WindowControllerPresence50DQ::Present)
     for(const unsigned event:{0u,1u,2u,3u,4u,5u,7u,6u}){
      const auto currentRoute=originalMain.coordinator.snapshot().route;
      if(currentRoute.screen!=12||currentRoute.page!=3)break;
      if(secondMenuEvents[event])originalMain.input(static_cast<std::uint8_t>(event),input.secondControllerPresence,1);
     }
    const auto uiNow=monotonicNow50DU();
    const std::uint64_t elapsedNanoseconds=originalMain.acceptedElapsed(uiNow);
    const float elapsed=originalMain.preparedSourceUpdateSeconds();
    // Recovered capped Application dispatch factor, one real source update.
    // The complete measured wall interval still advances PCM independently;
    // material timestamps retain their existing uncapped source clock.
    platform::OriginalUiMainSessionUpdate50KG update;update.elapsedSeconds=elapsed;
    // The active/focus gate above applies to both edges and held levels.
    // Course alone owns the original countdown; SDL repeat events are unused.
    update.courseHorizontalHeld=originalUiControllerNavigation.courseHorizontalHeld();
    // This original-UI keyboard mapping has no original debug-button bindings.
    // Its four debug-query levels therefore remain actually released.
    const auto acceptedSource=originalMain.update(update,elapsedNanoseconds);
    // Garage Root already committed its exact source timing; PCM was still advanced once.
    if(!acceptedSource.sourceTimingCommitted)originalMain.commitSourceUpdateTiming();
    originalMain.render(mainClock,renderer,info.drawableWidth,info.drawableHeight);
    // video+84 suspends publication while the two genuine hidden incoming
    // frames are prepared. Do not swap stale/undefined window BACK contents or
    // report a hidden source submission as a newly captured displayed frame.
    if(!originalMain.coordinator.snapshot().rider.deferred4EC){
     const auto swapStart=originalMain.stageStart();
     if(host.hasWindow()&&!host.swapWindow(hostDiagnostic)){
      originalMain.reportStage("window_swap",swapStart,false);
      reportHost50DU(hostDiagnostic);throw std::runtime_error("original Main window swap failed");
     }
     if(host.hasWindow())originalMain.reportStage("window_swap",swapStart);
     originalMain.imagePublished=true;++originalMain.frameCount;
     if(host.hasWindow())++windowSwaps;
     if(originalMain.frameCount==1){
      J ready={{"native_frame",originalMain.frameCount},{"width",info.drawableWidth},{"height",info.drawableHeight}};
      std::cerr<<"original_main_ready "<<ready.dump()<<'\n';reportUiState("ready");
     }
     captureCommitted();
    }
    originalMain.commitDeferred(renderer);
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
    if(originalMain.coordinator.takeCourseLaunch(selected)){
     const auto* target=courseTargets.admits(selected);
     require50DU(courseRaceCold&&!courseRaceLaunched&&target&&(selected.laps==3||humanCourseEnabled),
         "Course queue selected an unavailable native destination");
     // RA uses the same typed cold-load exit as every other course; the former
     // RA-only loading prefix always stopped at 1465D0 and had no consumer.
     // Even an unchanged track/rider must rebuild: this retained UI path
     // prepared its race groups before any new Options Accept. Never retune
     // those live groups or reuse their stale ducking restore configuration.
     require50DU(courseColdLoad.publish(courseTargets,selected,sessionIndex+1),
         "actual Course exit could not publish its typed cold-load request");
     std::cerr<<"original_course_cold_load_queued course="<<selected.course<<" track="<<target->assets->world->code
         <<" rider="<<selected.rider<<" outgoing_world_ticks="<<completedFrames<<'\n';
     break; //Real closeWindow + local-owner destruction precede target loading.
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(8));continue;
   }
   // Window close, explicit stdin quit, and EOF remain terminal. Escape is
   // host navigation only, never a native vehicle action or an implicit exit.
   if(input.quitRequested){
    composePacket(false,input);sampleController(); // Route a final held release before cleanup.
    break;
   }
   if(sourcePauseParentAttached){
    if(input.quitRequested)break;
    const auto& pauseInfo=host.info();
    const bool active=input.focused&&!input.minimized&&!input.activityLost&&
        !input.eventBudgetExhausted&&sourceCameraSession&&sourceCameraSession->hasView()&&
        !sourceCameraSession->pendingDisplayWork()&&spectatorUnderlayPublication&&
        spectatorUnderlayPublication==sourceCameraSession->publicationCount();
    require50DU(spectatorMenuInput.observe(host,hostInput,active),"spectator UI actual poll custody rejected");
    if(!active)spectatorClockResetRequired=true;
    if(active&&spectatorClockResetRequired){
     require50DU(sourcePauseObservation.resetNativePresentationClock(),"spectator UI source clock reset rejected");
     spectatorClockResetRequired=false;
    }
    const bool wasOpen=sourcePauseObservation.nativeSpectatorMenuOpen();
    original_pause_observation50LA::Owner::Diagnostic inputDiagnostic;
    if(!sourcePauseObservation.dispatchNativeInputBatch(spectatorMenuInput,sourceFadeOut.get(),inputDiagnostic)){
     std::ostringstream message;message<<"original Pause input failed after reached source effects, pc="
         <<std::hex<<inputDiagnostic.sourcePC;
     throw std::runtime_error(message.str());
    }
    // Original Pause owns this admitted route. No host-menu/restart command
    // or human driving packet may consume the same physical edge.
    hostInput.pauseRequested=hostInput.resumeRequested=hostInput.menuRequested=false;
    hostInput.restartRequested=hostInput.checkpointRequested=false;
    if(originalMainApplication.pauseReturnInputPending50LA()){
     discardTrackBaseline();hostClock.pause(monotonicNow50DU());continue;
    }
    const bool open=sourcePauseObservation.nativeSpectatorMenuOpen();
    if(wasOpen&&!open){
     spectatorAudioLastNs=spectatorAudioCarryNs=0;
     hostClock.pause(monotonicNow50DU());forceSceneRender=true;suppressResumeKeys=true;
     lastStatusNs=monotonicNow50DU();lastStatusRenderCount=submittedDrawFrames;
     displayWallTotals.reportAndReset("original_pause_resume");
     phaseTiming.resetInterval();for(auto& total:simulationTotals)total={};renderDiagnostics.resetInterval();
    }
    if(open){
     discardTrackBaseline();
     require50DU(sourceCameraInput.sample(input,false,sourceCameraDisplay->rootFlags228),
         "spectator Pause camera release failed");
     composePacket(false,input);hostClock.pause(monotonicNow50DU());wasPaused=true;
     if(!wasOpen||!spectatorInputActive){stopFeedbackTransport();sampleController();}
     // Native UI audio uses elapsed host time, never a committed world step.
     // The explicit muted ALL8 route still owns a live logical mixer/output.
     if(audioEnabled&&liveAudio->outputOpen()){
      if(!active){
       liveAudio->pause(true);spectatorAudioLastNs=spectatorAudioCarryNs=0;
      }else{
       liveAudio->pause(false);
       const auto audioNow=monotonicNow50DU();
       if(!wasOpen||!spectatorAudioLastNs)spectatorAudioLastNs=audioNow;
       require50DU(audioNow>=spectatorAudioLastNs,"native menu audio clock regressed");
       const auto elapsed=audioNow-spectatorAudioLastNs;spectatorAudioLastNs=audioNow;
       // Match the host's bounded catch-up policy: at most64ms is serviced.
       // Excess wall time is discarded, never passed to physics or source250.
       spectatorAudioCarryNs=std::min<std::uint64_t>(64000000,
           spectatorAudioCarryNs+std::min<std::uint64_t>(elapsed,64000000));
       while(spectatorAudioCarryNs>=1000){
        const auto micros=static_cast<std::uint32_t>(std::min<std::uint64_t>(16000,spectatorAudioCarryNs/1000));
        // The counted ALL8 listener accounts for actual menu consumption only;
        // its world pose/velocity and music update remain held until Resume.
        std::uint32_t listenerPC=0;
        const bool serviced=sourceCameraSession&&originalMainApplication.serviceConsumedRaAll8MenuAudioNative(
            *sourceCameraSession,*liveAudio,micros,audioDiagnostic,listenerPC);
        if(!serviced)std::cerr<<"all8_menu_audio_stopped pc="<<listenerPC<<'\n';
        require50DU(serviced,"native menu audio service rejected; reached mix retained");
        spectatorAudioCarryNs-=std::uint64_t(micros)*1000;
       }
      }
     }

     const bool drawable=active&&pauseInfo.drawableWidth==spectatorUnderlayWidth&&
         pauseInfo.drawableHeight==spectatorUnderlayHeight;
     if(drawable){
      original_pause_observation50LA::Owner::Diagnostic d;
      OriginalPauseLayout50LA::Diagnostic layout;
      OriginalPauseNativeResources50LA::Diagnostic draw;
      require50DU(sourcePauseObservation.prepareNativePresentation(*sourcePause.drawing(),d,layout,draw),
          "original Pause presentation preparation failed");
      require50DU(renderer.submitRetainedOriginalPause(*sourcePause.drawing(),
          pauseInfo.drawableWidth,pauseInfo.drawableHeight,rendererDiagnostic),
          "original Pause retained-image submission failed");
      const bool capturePause=host.hasWindow()&&windowCapture.due(windows_capture50LD::Stage::other,false);
      require50DU(windowCapture.swapAndCapture(renderer,host,capturePause,windowSwaps+1,
          [&](){return renderer.completeGameplayDisplay(host,hostDiagnostic,completionDiagnostic);}),
          "original Pause actual display completion failed");
      if(host.hasWindow())addCounter50DU(windowSwaps);
      if(capturePause)windowCapture.record(renderer,host,windows_capture50LD::Stage::other,false,
          J{{"screen","original_pause"},{"window_swaps",windowSwaps},{"native_tick",completedFrames},
            {"camera_publication",spectatorUnderlayPublication},{"all_original_ai",allAiEnabled},
            {"original_ai_count",originalAiOpponentCount},{"human_count",originalAiHumanCount},
            {"source_pause_open",sourcePauseObservation.nativeSpectatorMenuOpen()},
            {"native_policy","explicit_muted_all8_spectator_menu"}});
     }else spectatorClockResetRequired=true;
     spectatorInputActive=active;
     captureCommitted();std::this_thread::sleep_for(std::chrono::milliseconds(8));continue;
    }
    spectatorInputActive=active;
   }
   // Unattended relaxes focus only. Hold even source display effects while
   // minimized, event-starved or outside the real drawable capacity.
   if(captureRun.unattended()&&(input.minimized||input.eventBudgetExhausted||
       host.info().drawableWidth<=0||host.info().drawableHeight<=0||
       host.info().drawableWidth>maximumWidth||host.info().drawableHeight>maximumHeight)){
    discardTrackBaseline();
    if(sourceCameraSession&&!humanCourseEnabled)require50DU(sourceCameraInput.sample(input,false,sourceCameraDisplay->rootFlags228),
        "unattended inactive camera input release failed");
    composePacket(false,input);sampleController();stopFeedbackTransport();
    hostClock.pause(monotonicNow50DU());wasPaused=true;
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    std::this_thread::sleep_for(std::chrono::milliseconds(8));continue;
   }
   if(sourceCameraSession&&(sourceCameraSession->blockingVideoPending()||sourceCameraSession->asyncVideoPending())){
    discardTrackBaseline();
    if(!humanCourseEnabled)require50DU(sourceCameraInput.sample(input,false,sourceCameraDisplay->rootFlags228),
        "camera-only fade release failed");
    const bool asynchronous=sourceCameraSession->asyncVideoPending();
    const bool endingFade=sourceCameraSession->endingVideoPending();
    auto* asyncBackend=endingFade?sourceEndingFade.get():sourceFadeIn.get();
    // Measurement only: swaps made by the fade backend inside this advance.
    const auto fadeSwapsBefore50AU=rr::platform::windowSwapTotal50AU();
    (void)rr::platform::windowSwapTakeMaxGapMs50AU(); // drop gaps of earlier (gameplay) swaps
    const auto result=asynchronous?sourceCameraSession->advanceAsyncVideo(asyncBackend):
        sourceCameraSession->advanceBlockingVideo(sourceFadeOut.get());
    noteFadeSwaps50AU(rr::platform::windowSwapTotal50AU()-fadeSwapsBefore50AU,rr::platform::windowSwapTakeMaxGapMs50AU());
    require50DU((asynchronous?asyncBackend->phase():sourceFadeOut->phase())!=platform::OriginalVideoNativePhase::failed,
        "original camera display backend failed with ownership retained");
    require50DU(result.status==sourceCamera::Status::complete||result.status==sourceCamera::Status::videoPending,
        "original camera display effect source boundary");
    // Effect samples have their own real clock and completion/swap. They are
    // not native race ticks and cannot contribute catch-up/countdown time.
    hostClock.pause(monotonicNow50DU());
    if(result.status==sourceCamera::Status::videoPending){
     require50DU(result.start.status==OriginalCameraStartStatus::videoClockWait,
         "original camera display stopped at a terminal source/backend boundary");
     if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
     captureCommitted();std::this_thread::sleep_for(std::chrono::milliseconds(1));continue;
    }
    if(suspendedTaCameraFrame&&sourceCameraSession->taSkipVideoResumePending()){
     require50DU(sourceCameraSession->advanceCameraChild(*suspendedTaCameraFrame,
         sourceCamera::Cut::heldBeforeStart).status==sourceCamera::Status::framePublished,
         "TA skip same-frame camera continuation rejected");
     suspendedTaCameraFrame.reset();
     require50DU(finishTaRoot(),"TA suspended root return rejected");
     publishSourceCameraFog();
    }
    if(endingFade)require50DU(sourceCameraSession->advanceHumanEndFadeManager().status==sourceCamera::Status::complete&&
        sourceCameraSession->endingVideoCovered(),"actual human ending fade did not complete its covered state");
    forceSceneRender=true;
   }
   // Native ALL8 restart is a full teardown/rebuild. Checkpoint restoration
   // remains unavailable for the genuine AI bank and source camera lifecycle.
   if((isGaTrack||originalCameraEnabled)&&hostInput.checkpointRequested){
    std::cerr<<"Native whole-race checkpoint restore unavailable; use F5 or Restart Race.\n";
    hostInput.checkpointRequested=false;
   }
   if(isGaTrack&&!originalCameraEnabled&&hostInput.restartRequested){
    std::cerr<<"GA restart requires the owned original camera/ALL8 route.\n";
    hostInput.restartRequested=false;
   }
   if(hostInput.restartRequested){
    composePacket(false,input);sampleController();
    // F5/stdin restart bypasses menu.poll, including its accepted-departure
    // pause clear below. Retire an open Pause/Controls/confirmation transaction
    // here too: Application11C survives whole-world destruction and otherwise
    // leaves the rebuilt race's original animation consumers paused.
    if(sessionMenu.open())originalMainApplication.setExplicitAnimationPause(false);
    restartRequested=true;
    break;
   }
   const auto resultsRevision=resultsUi.revision();
   const bool resultsAvailable=(humanCourseEnabled||allAiEnabled||!isGaTrack)&&humanResultsReady&&!sessionMenu.open()&&!recoveryPaused&&
       s.phase==VehicleFramePhase49W::Ready&&input.focused&&!input.minimized&&!input.eventBudgetExhausted;
   const bool resultsBack=resultsAvailable&&resultsUi.confirming()&&input.pressed[41]&&
       !hostInput.menuRequested&&!hostInput.pauseRequested;
   const bool resultsAdmitted=resultsAvailable&&!hostInput.menuRequested&&!hostInput.pauseRequested&&
       (!input.pressed[41]||resultsBack);
   const auto resultsAction=resultsUi.poll(input,resultsAdmitted);
   if(resultsBack)input.pressed[41]=0; // Escape cancels this live confirmation, not Pause.
   if(resultsUi.revision()!=resultsRevision){forceSceneRender=true;reportUiState("results");}
   if(resultsAction!=platform::HostRaceResults50IL::Action::None){
    std::cerr<<"host_results_action action="<<unsigned(resultsAction)<<" tick="<<completedFrames
        <<" mixed_frames="<<liveAudio->mixedFrames()<<'\n';
    if(resultsAction==platform::HostRaceResults50IL::Action::MainMenu){
     // This explicit, confirmed Results action takes the same full owned
     // teardown as Pause -> Original Main, under that destination's same
     // application/source-camera admission. A menuRequested pulse would
     // merely reopen Pause for an ALL8 source-camera race.
     if(originalUiStarted&&originalCameraEnabled){
      composePacket(false,input);sampleController();
      returnOriginalMainRequested=true;break;
     }
     hostInput.menuRequested=true;
    }else{
     composePacket(false,input);sampleController();
     restartRequested=resultsAction==platform::HostRaceResults50IL::Action::Restart;
     break;
    }
   }
   if(!allAiEnabled&&(humanCourseEnabled||!isGaTrack)&&humanResultsReady&&!sessionMenu.open()){
    // These keys belong to Results navigation. Native finish controls retain
    // sole ownership; UI arrows/Space never count as attempted manual driving.
    platform::releaseSelectedKeyboard50KY(input,humanControlInput);
   }
   const bool menuWasOpen=sessionMenu.open();
   const auto menuRevision=sessionMenu.revision();
   if(!sourcePauseParentAttached&&input.focused&&!input.minimized&&!input.activityLost&&!input.eventBudgetExhausted&&
       (!sourceCameraSession||!sourceCameraSession->pendingDisplayWork())){
    // Native HUD page is available only with the SAME Application controls,
    // actual one-human camera and all three genuinely bound HUD renderers.
    // This does not activate the separate original spectator Pause Entity.
    std::array<std::uint32_t,4> nativeHud{};
    const auto nativeHudCurrent=[&](){
     return humanCourseEnabled&&originalUiStarted&&!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready&&sourceCameraSession&&
         originalMainApplication.controlsProfile.get()==&controlPresets&&
         originalMainApplication.profile&&originalMainApplication.profile->known&&
         sourceTimeStatsRendering&&sourceTimeStats.owner&&sourceSpeedBoostRendering&&sourceSpeedBoost.owner&&
         sourcePositionStatsRendering&&sourcePositionStatsHud&&originalAiSession.managerBound&&
         originalAiSession.rankBound&&originalAiSession.retired40.bound&&
         sourceCameraSession->observeNativeHumanHud(controlPresets,originalMainApplication.animationClock(),
             originalMainApplication.profile->playerBits1125C,nativeHud);
    };
    const bool nativeHudAvailable=nativeHudCurrent();
    sessionMenu.observeNativeHud(nativeHudAvailable?&nativeHud:nullptr);
    // Borrow the SAME live provider mixer/group ledger and committed profile
    // that initialized it, not selectedAudioSettings (legacy next-session UI).
    const auto nativeAudioProfile=originalMainApplication.gameSettingsProfile;
    auto* nativeAudioMixer=liveAudio->mixerForMaterialOwner();
    auto* nativeAudioGroups=liveAudio->groupsForProviders();
    std::array<std::uint32_t,4> nativeAudio{};
    const auto nativeAudioCurrent=[&](){
     return nativeHudCurrent()&&audioEnabled&&liveAudio->prepared()&&!liveAudio->closed()&&liveAudio->outputOpen()&&
         nativeAudioProfile&&originalMainApplication.gameSettingsProfile==nativeAudioProfile&&
         nativeAudioMixer&&liveAudio->mixerForMaterialOwner()==nativeAudioMixer&&
         nativeAudioGroups&&liveAudio->groupsForProviders()==nativeAudioGroups&&
         originalMainApplication.soundMode&&originalMainApplication.soundMode->ready()&&
         rr::game::validOriginalSoundModeFields50KG(originalMainApplication.soundMode->fields())&&
         nativeAudioMixer->originalSoundMode50KG()==originalMainApplication.soundMode.get()&&
         std::uint32_t(originalMainApplication.soundMode->fields().profile1101C)==nativeAudioProfile->soundMode1101C&&
         nativeAudioGroups->observeNativePauseAudio(*nativeAudioProfile,originalMainApplication.entities(),
             *nativeAudioMixer,nativeAudio);
    };
    const bool nativeAudioAvailable=nativeAudioCurrent();
    sessionMenu.observeNativeRaceAudio(nativeAudioAvailable?&nativeAudio:nullptr);
    // nativeMenuCommand retains this poll's original intent before Results
    // retirement or unavailable-checkpoint suppression cleared hostInput.
    // HUD/live-audio admission uses it; no rejected host command is restored.
    using MenuResult=platform::HostSessionMenu50IF::Result;
    const bool originalPauseShown=sourcePauseObservation.hostOpen()&&sessionMenu.open()&&
        sessionMenu.page()==platform::HostSessionMenu50IF::Page::Pause;
    auto action=MenuResult::None;
    if(!originalPauseShown)action=sessionMenu.poll(hostInput,!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready,
        nativeMenuCommand);
    else{
     // Original Pause rows: 0 CONTINUE, 1 RESTART RACE, 2..4 HUD, 5..8
     // volumes, 9 QUIT TO MAIN MENU, 10 QUIT. Same native owners as the
     // host HUD/audio pages and the same session actions as its rows.
     const auto in=sessionMenu.pollOriginalPause(hostInput);
     if(in.up!=in.down)sourcePauseObservation.moveHostSelection(in.down);
     const auto row=sourcePauseObservation.hostSelection();
     if(in.left!=in.right&&row>=2&&row<=4&&nativeHudAvailable){
      require50DU(originalMainApplication.animationPaused()&&sourceCameraSession->adjustNativeHumanHud(
          controlPresets,originalMainApplication.animationClock(),
          originalMainApplication.profile->playerBits1125C,nativeHud,row-2,in.right),
          "original Pause HUD preference owner rejected");
      require50DU(nativeHudCurrent(),"original Pause HUD preference observation failed after commit");
      sessionMenu.observeNativeHud(&nativeHud);
      sourcePauseObservation.hostSettingFeedback(true,in.right);
     }
     if(in.left!=in.right&&row>=5&&row<=8&&nativeAudioAvailable){
      require50DU(originalMainApplication.animationPaused()&&nativeAudioGroups->adjustNativePauseAudio(
          *nativeAudioProfile,originalMainApplication.entities(),*nativeAudioMixer,
          originalMainApplication.animationClock(),nativeAudio,row-5,in.right),
          "original Pause live audio owner rejected");
      require50DU(nativeAudioCurrent(),"original Pause live audio observation failed after commit");
      sessionMenu.observeNativeRaceAudio(&nativeAudio);
      sourcePauseObservation.hostSettingFeedback(true,in.right);
     }
     const bool canResume=!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready;
     if((in.back||(in.confirm&&row==0))&&canResume)sessionMenu.closeExternal();
     else if(in.confirm&&row==1)action=MenuResult::Restart;
     else if(in.confirm&&row==9)action=MenuResult::ReturnOriginalMain;
     else if(in.confirm&&row==10)action=MenuResult::Quit;
    }
    // Every-track original Pause follows the host Pause page's lifetime.
    {
     const bool wanted=sourcePauseObservation.hostBound()&&sessionMenu.open()&&
         sessionMenu.page()==platform::HostSessionMenu50IF::Page::Pause;
     if(wanted&&!sourcePauseObservation.hostOpen()){
      if(!sourcePauseObservation.openHost()){
       std::cerr<<"original_pause_host open_rejected; host menu retained\n";
       sourcePauseObservation.unbind();
      }
     }else if(!wanted&&sourcePauseObservation.hostOpen())sourcePauseObservation.closeHost();
    }
    // Only accepted explicit native menu commands map to Application11C.
    // Focus/minimize/recovery/controller-loss suppression never writes it.
    if(menuWasOpen!=sessionMenu.open())
     originalMainApplication.setExplicitAnimationPause(sessionMenu.open());
    if(action==platform::HostSessionMenu50IF::Result::HudPrevious||
       action==platform::HostSessionMenu50IF::Result::HudNext){
     // Consume synchronously once. No request survives a focus interruption,
     // menu exit or world boundary; a stale owner fails before any mutation.
     require50DU(nativeHudAvailable&&sessionMenu.page()==platform::HostSessionMenu50IF::Page::Hud&&
         originalMainApplication.animationPaused()&&sourceCameraSession->adjustNativeHumanHud(
             controlPresets,originalMainApplication.animationClock(),
             originalMainApplication.profile->playerBits1125C,nativeHud,sessionMenu.nativeHudRow(),
             action==platform::HostSessionMenu50IF::Result::HudNext),
         "native Pause HUD preference owner rejected");
     require50DU(nativeHudCurrent(),"native Pause HUD preference observation failed after commit");
     sessionMenu.observeNativeHud(&nativeHud); //Actual post-store labels/revision.
     // Existing resume forces a new HUD frame. Do not redraw the paused world
     // or turn menu wall time into a simulation/animation step here.
    }
    if(action==platform::HostSessionMenu50IF::Result::RaceAudioPrevious||
       action==platform::HostSessionMenu50IF::Result::RaceAudioNext||
       action==platform::HostSessionMenu50IF::Result::RaceAudioDefaults){
     const bool defaults=action==platform::HostSessionMenu50IF::Result::RaceAudioDefaults;
     require50DU(nativeAudioAvailable&&sessionMenu.page()==platform::HostSessionMenu50IF::Page::RaceAudio&&
         originalMainApplication.animationPaused()&&
         (defaults?nativeAudioGroups->restoreNativePauseVolumes(
             *nativeAudioProfile,originalMainApplication.entities(),*nativeAudioMixer,
             originalMainApplication.animationClock(),nativeAudio):nativeAudioGroups->adjustNativePauseAudio(
             *nativeAudioProfile,originalMainApplication.entities(),*nativeAudioMixer,
             originalMainApplication.animationClock(),nativeAudio,sessionMenu.nativeRaceAudioRow(),
             action==platform::HostSessionMenu50IF::Result::RaceAudioNext)),
         "native Pause live audio owner rejected");
     require50DU(nativeAudioCurrent(),"native Pause live audio observation failed after commit");
     sessionMenu.observeNativeRaceAudio(&nativeAudio);
     // Native Pause keeps audio paused. No mixer clock, source feedback,
     // persistence or announcer begin/end is manufactured by this edit.
    }
    selectedRacer60=sessionMenu.selectedRacer60();
    selectedAudioSettings=sessionMenu.selectedAudioSettings();
    selectedTrack=sessionMenu.selectedTrack();
    if(action==platform::HostSessionMenu50IF::Result::Restart||
       action==platform::HostSessionMenu50IF::Result::ReturnOriginalMain||
       action==platform::HostSessionMenu50IF::Result::Quit){
     composePacket(false,input);sampleController();
     restartRequested=action==platform::HostSessionMenu50IF::Result::Restart;
     returnOriginalMainRequested=action==platform::HostSessionMenu50IF::Result::ReturnOriginalMain;
     // Explicit departure from this native Pause/menu transaction, not a
     // generic new-page/new-race reset of the process pause word.
     originalMainApplication.setExplicitAnimationPause(false);
     break;
    }
   }
   if(sessionMenu.revision()!=menuRevision){
    std::cerr<<"host_menu open="<<sessionMenu.open()<<" page="<<unsigned(sessionMenu.page())<<" tick="<<completedFrames
        <<" mixed_frames="<<liveAudio->mixedFrames()<<'\n';
    reportUiState("transition");
   }
   if(menuWasOpen&&!sessionMenu.open()){
    suppressResumeKeys=true;forceSceneRender=true;
    // Never spend the time in any host menu as catch-up simulation.
    hostClock.pause(monotonicNow50DU());
    // Menu wall time/frames are not part of gameplay FPS or phase intervals.
    lastStatusNs=monotonicNow50DU();lastStatusRenderCount=submittedDrawFrames;
    displayWallTotals.reportAndReset("menu_resume");
    phaseTiming.resetInterval();for(auto& total:simulationTotals)total={};renderDiagnostics.resetInterval();
   }
   if(hostInput.checkpointRequested&&!sessionMenu.open()){
    composePacket(false,input);sampleController();
    if(raceEnabled){
     std::cerr<<"Race restart requested: checkpoint rebuild restarts every racer and countdown together.\n";
     restartRequested=true;break;
    }
    if(recoveryPaused||s.phase!=VehicleFramePhase49W::Ready){
     std::cerr<<"Manual checkpoint unavailable at a paused native boundary; F5/restart returns to grid.\n";
    }else{
     recovery50FI::Plan manualCheckpoint{};recovery50FI::Diagnostic manualDiagnostic{};
     BodyMatrix48G manualGrounded{};
     require50DU(!isMbTrack&&!isTcTrack&&!isRcTrack&&!isTbTrack&&!isGbTrack&&!isMcTrack,"MB/TC/RC/TB/GB/MC native session cannot enter legacy HOST checkpoint prepareManualCheckpoint");
     const bool prepared=recovery50FI::prepareManualCheckpoint(*navigationMap,navigationProgress,s,
         x.bindings,x.parameters.visual.verticalLimit7EC,liveWorld,manualCheckpoint,manualDiagnostic);
     const bool grounded=prepared&&recovery50FI::resolveGround(ground,manualCheckpoint,16,0,
         groundStack.data(),groundStack.size(),manualGrounded,manualDiagnostic);
     if(grounded){
      checkpointOutput=manualCheckpoint;checkpointRequested=true;
      std::cerr<<"Manual checkpoint requested: anchor="<<manualCheckpoint.selection.anchorNode
          <<" zone="<<manualCheckpoint.primaryZone<<" damage="<<manualCheckpoint.damage.damage7D4
          <<'/'<<manualCheckpoint.damage.damage7D0<<". Progress/damage retained; held input released.\n";
      break;
     }
     recoveryPaused=true;
     std::cerr<<"Manual checkpoint preflight failed: error="<<unsigned(manualDiagnostic.error)
         <<". Paused on last committed image; F5/restart returns to grid.\n";
    }
   }
   capturePending=capturePending||hostInput.captureRequested;
   const auto now=monotonicNow50DU();
   const auto& info=host.info();
   if(sessionMenu.open()){
    discardTrackBaseline();
    if(sourceCameraSession&&!humanCourseEnabled)require50DU(sourceCameraInput.sample(input,false,sourceCameraDisplay->rootFlags228),
        "camera-only menu release failed");
    composePacket(false,input);
    if(!wasPaused||!menuWasOpen){stopFeedbackTransport();sampleController();}
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    hostClock.pause(now);wasPaused=true;
    const bool drawable=!info.minimized&&info.drawableWidth>0&&info.drawableHeight>0&&
        info.drawableWidth<=maximumWidth&&info.drawableHeight<=maximumHeight;
    if(drawable&&sourcePauseObservation.hostOpen()){
     // Animated original screen (opening slide, arrows): redraw every frame.
     original_pause_observation50LA::Owner::Diagnostic pauseObservation;
     OriginalPauseLayout50LA::Diagnostic pauseLayout;
     OriginalPauseNativeResources50LA::Diagnostic pauseDraw;
     const bool submitted=sourcePauseObservation.prepareHostPresentation(*sourcePause.drawing(),
         pauseObservation,pauseLayout,pauseDraw)&&
         renderer.submitRetainedOriginalPause(*sourcePause.drawing(),info.drawableWidth,info.drawableHeight,rendererDiagnostic);
     if(submitted){
      if(originalCameraEnabled){
       phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
       const bool displayAccepted=renderer.completeGameplayDisplay(host,hostDiagnostic,completionDiagnostic);
       completionTiming.finish();
       displayWallTotals.record(true,displayAccepted,completionDiagnostic);
       require50DU(displayAccepted,"original Pause actual displayed history completion failed");
      }else if(host.hasWindow()&&!host.swapWindow(hostDiagnostic)){
       reportHost50DU(hostDiagnostic);throw std::runtime_error("original Pause window swap failed");
      }
      menuImagePublished=true;drawnMenuRevision=0;
     }else{
      std::cerr<<"original_pause_host presentation_rejected layout_pc="<<pauseLayout.sourcePC
          <<" observation_pc="<<pauseObservation.sourcePC<<"; host menu retained\n";
      sourcePauseObservation.closeHost();sourcePauseObservation.unbind();drawnMenuRevision=0;
     }
    }else if(drawable&&(sessionMenu.revision()!=drawnMenuRevision||
       info.drawableWidth!=drawnMenuWidth||info.drawableHeight!=drawnMenuHeight||input.resized||capturePending)){
     const auto menu=sessionMenu.view(!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready);
     if(!renderer.submitHostMenu(menu,info.drawableWidth,info.drawableHeight,rendererDiagnostic)){
      reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("host menu submission failed; not swapped");
     }
     if(originalCameraEnabled){
      phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
      const bool displayAccepted=renderer.completeGameplayDisplay(host,hostDiagnostic,completionDiagnostic);
      completionTiming.finish();
      displayWallTotals.record(true,displayAccepted,completionDiagnostic);
      if(!displayAccepted)displayWallTotals.reportAndReset("failure");
      require50DU(displayAccepted,"host menu actual displayed history completion failed");
     }else if(host.hasWindow()&&!host.swapWindow(hostDiagnostic)){
      reportHost50DU(hostDiagnostic);throw std::runtime_error("host menu window swap failed");
     }
     menuImagePublished=true;drawnMenuRevision=sessionMenu.revision();
     drawnMenuWidth=info.drawableWidth;drawnMenuHeight=info.drawableHeight;
    }
    captureCommitted();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   if(recoveryPaused){
    discardTrackBaseline();
    stopFeedbackTransport();
    // Never prepare or submit a draw batch from the interrupted native state.
    // Readback uses only the renderer's already committed private target.
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    captureCommitted();hostClock.pause(now);
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   const bool oversize=info.drawableWidth>maximumWidth||info.drawableHeight>maximumHeight;
   if(oversize&&!oversizeReported){
    std::cerr<<"Paused: drawable exceeds "<<maximumWidth<<'x'<<maximumHeight<<"; resize smaller to resume.\n";
    oversizeReported=true;
   }else if(!oversize&&oversizeReported){
    std::cerr<<"Drawable fits again.\n";oversizeReported=false;
   }
   const bool humanControlsPermitted=!raceEnabled||withRace([&](auto& actualRace){return race_session50HM::controlsAllowed(actualRace,0);});
   bool paused=((!input.focused||input.activityLost)&&!captureRun.unattended())||input.minimized||input.eventBudgetExhausted||
       info.drawableWidth<=0||info.drawableHeight<=0||oversize||
       (usePhysicalGameController&&humanControlsPermitted&&(!input.controllers[0].connected||input.activityLost));
   // Every host transport pause releases the device packet. Arm the same
   // keyboard neutral barrier for drawable/oversize pauses too, not only
   // focus/menu interruption. Otherwise a resize back to a supported size
   // recreates held throttle, boost, reset or camera presses immediately.
   // Observe the actual keys below BEFORE clearing this poll's local copy.
   //ALL8 has no human keyboard binding. Its independent sourceCameraInput
   //owner already retains the actual C-only release-to-arm latch.
   if(allAiEnabled)suppressResumeKeys=false;
   else if(paused)suppressResumeKeys=true;
   if(suppressResumeKeys){
    suppressResumeKeys=platform::selectedKeyboardHeld50KY(input,humanControlInput);
    // Re-arm keyboard only after all mapped controls are released. The
    // physical pad has its own actual-instance/neutral latch below.
    platform::releaseSelectedKeyboard50KY(input,humanControlInput);
   }
   // The countdown is not an input interruption (original 1A1A20 reads held
   // pad state every frame); only a real pause/menu or focus loss is.
   platform::selectedControllerCountdownHold50KY=!paused&&!humanControlsPermitted;
   composePacket(!paused&&humanControlsPermitted,input);
   platform::selectedControllerCountdownHold50KY=false;
   // Native device-loss/reconnect policy holds the existing world clock. It
   // cannot call the still-unjoined original missing-controller popup.
   if(usePhysicalGameController&&(physicalControllerReceipt.status==
       platform::SelectedControllerStatus50KY::awaitingNeutral||physicalControllerReceipt.status==
       platform::SelectedControllerStatus50KY::unsupportedOriginalAction))paused=true;
   {
    // Hold diagnostic: log every change of the reason the world clock is held,
    // with the controller's held buttons/axes, so a stall names its cause.
    const auto& pad=input.controllers[0];
    std::uint32_t reasons=0;
    if((!input.focused||input.activityLost)&&!captureRun.unattended())reasons|=1u;
    if(input.minimized)reasons|=2u;
    if(input.eventBudgetExhausted)reasons|=4u;
    if(info.drawableWidth<=0||info.drawableHeight<=0||oversize)reasons|=8u;
    if(usePhysicalGameController&&humanControlsPermitted&&!pad.connected)reasons|=16u;
    if(usePhysicalGameController&&humanControlsPermitted&&input.activityLost)reasons|=32u;
    if(usePhysicalGameController&&physicalControllerReceipt.status==platform::SelectedControllerStatus50KY::awaitingNeutral)reasons|=64u;
    if(usePhysicalGameController&&physicalControllerReceipt.status==platform::SelectedControllerStatus50KY::unsupportedOriginalAction)reasons|=128u;
    static std::uint32_t lastReasons50DU=0;static unsigned holdLines50DU=0;
    if(reasons!=lastReasons50DU&&holdLines50DU<400){
     ++holdLines50DU;
     std::cerr<<"world_hold reasons=0x"<<std::hex<<reasons<<std::dec
         <<" focus_or_activity="<<((reasons&1u)?1:0)<<" minimized="<<((reasons&2u)?1:0)
         <<" event_budget="<<((reasons&4u)?1:0)<<" drawable="<<((reasons&8u)?1:0)
         <<" pad_disconnected="<<((reasons&16u)?1:0)<<" pad_activity_lost="<<((reasons&32u)?1:0)
         <<" pad_awaiting_neutral="<<((reasons&64u)?1:0)<<" pad_unsupported_action="<<((reasons&128u)?1:0)
         <<" controls_permitted="<<humanControlsPermitted<<" focused="<<input.focused
         <<" activity_lost="<<input.activityLost<<" pad_connected="<<pad.connected
         <<" race_phase="<<static_cast<unsigned>(race.phase)<<" tick="<<completedFrames<<" held=";
     bool any=false;
     for(std::size_t b=0;b<pad.buttons.size();++b)if(pad.buttons[b]){std::cerr<<(any?",":"")<<"b"<<b;any=true;}
     for(std::size_t a=0;a<4;++a){const auto v=platform::controllerStickByte50KY(pad.axes[a]);
      if(v<=102||v>=153){std::cerr<<(any?",":"")<<"axis"<<a<<"="<<static_cast<unsigned>(v);any=true;}}
     for(std::size_t a=4;a<6;++a){const auto v=platform::controllerTriggerByte50KY(pad.axes[a]);
      if(v>=26){std::cerr<<(any?",":"")<<"trigger"<<a<<"="<<static_cast<unsigned>(v);any=true;}}
     if(!any)std::cerr<<"none";
     std::cerr<<'\n';
     lastReasons50DU=reasons;
    }
   }
   if(paused){
    discardTrackBaseline();
    if(sourceCameraSession&&!humanCourseEnabled)require50DU(sourceCameraInput.sample(input,false,sourceCameraDisplay->rootFlags228),
        "camera-only pause release failed");
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    if(!wasPaused){stopFeedbackTransport();sampleController();} // Publish the declared release-all sample.
    hostClock.pause(now);wasPaused=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   if(wasPaused){hostClock.pause(now);wasPaused=false;if(audioEnabled){liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);}}
   // Manual input samples every host poll. The optional host course driver
   // instead emits one ordinary packet per actual 16 ms simulation step.
   if(!autoDriveEnabled)sampleController();
   if(sourceCameraSession&&!humanCourseEnabled)require50DU(sourceCameraInput.sample(input,
       (!captureRun.unattended()||(input.focused&&!input.activityLost))&&
       !sourceCameraSession->pendingDisplayWork()&&!originalAiSession.all8FinishAwaitingDisplay&&
       !originalAiSession.all8ResultsComplete,sourceCameraDisplay->rootFlags228),
       "original camera input source dispatch failed");
   platform::FixedStepPlan50DT plan{};
   require50DU(hostClock.plan(now,plan),"fixed-step host clock planning failed");
   addCounter50DU(discardedHostNs,plan.discardedNanoseconds);
   {
    // Green-light trace: first 3 race seconds (and the last countdown second),
    // one line per host loop: wall interval, planned/discarded steps and the
    // world tick reached. A frozen world shows as repeated tick values.
    static std::uint64_t greenTracePrevNs50DU=0;static unsigned greenTraceLines50DU=0;
    const bool window=(race.phase==race_session50HM::Phase::Racing&&race.raceSeconds<3.0)||
        (race.phase==race_session50HM::Phase::Countdown&&race.countdownRemaining<=1.0);
    if(window&&greenTraceLines50DU<400){
     ++greenTraceLines50DU;
     std::cerr<<"green_trace phase="<<static_cast<unsigned>(race.phase)<<" race_s="<<race.raceSeconds
         <<" countdown="<<race.countdownRemaining<<" wall_ms="<<(greenTracePrevNs50DU?(now-greenTracePrevNs50DU)/1.0e6:0.0)
         <<" steps="<<plan.steps<<" discarded_ms="<<plan.discardedNanoseconds/1.0e6
         <<" tick="<<completedFrames<<" camera_gate="<<(sourceCameraSession?1:0)<<'\n';
    }
    greenTracePrevNs50DU=now;
   }
   for(std::uint32_t step=0;step<plan.steps;++step){
    if(sourceCameraSession&&sourceCameraSession->pendingDisplayWork())break;
    stall_watchdog50DU::mark("step_begin");
    // End each accepted root on every normal loop departure. A source skip
    // retains its prepared publication until that same child call returns.
    // Held-step diagnostic: any segment over 30 ms is logged by name.
    auto heldLapPrev50DU=std::chrono::steady_clock::now();
    const auto heldLap50DU=[&](const char* tag){
     const auto t=std::chrono::steady_clock::now();
     const double ms=std::chrono::duration<double,std::milli>(t-heldLapPrev50DU).count();
     static unsigned heldLapLines50DU=0;
     if(ms>30.0&&heldLapLines50DU<200){++heldLapLines50DU;std::cerr<<"held_step_slow segment="<<tag<<" ms="<<ms<<" tick="<<completedFrames<<"\n";}
     heldLapPrev50DU=t;
    };
    require50DU(beginTaRoot(),"actual TA Application root audio service rejected");
    heldLap50DU("ta_root_begin");
    struct TaRootReturn {
     decltype(finishTaRoot)& finish;
     const std::optional<std::uint64_t>& suspended;
     int exceptions=std::uncaught_exceptions();
     ~TaRootReturn() noexcept(false){
      if(!suspended&&std::uncaught_exceptions()==exceptions)
       require50DU(finish(),"actual TA root return rejected");
     }
    } taRootReturn{finishTaRoot,suspendedTaCameraFrame};
    if((allAiEnabled&&(originalAiSession.all8FinishAwaitingDisplay||originalAiSession.all8ResultsComplete))||
       (humanCourseEnabled&&humanResultsReady)){
     // Completed native Results holds the last genuine world/camera frame.
     // ALL8 uses its separate zero-human policy; human Course arrives only
     // after the real ending clock/camera/audio/fade. The shared mixer keeps
     // real voice lifetimes without a substitute driving controller.
     if(audioEnabled&&liveAudio->outputOpen()){
      phase_timing50GK::Scope audioTiming(phaseTiming,phase_timing50GK::Phase::AudioCommit);
      const auto beforeMix=liveAudio->mixedFrames();
      const bool mixed=liveAudio->committedStep(audioDiagnostic);
      advanceSourceAudioWorld(beforeMix,true); //Actual completed-scene held route.
      require50DU(mixed,"native completed-scene audio service failed");
     }
     continue;
    }
    if(sourceCameraSession&&sourceCameraSession->pendingDisplayWork())break;
    if(humanCourseEnabled&&humanResultsActive){
     // Issued RA reads its actual registered providers only at their source
     // cuts AFTER98/94 stores. Other routes preserve the old snapshot policy.
     // Results replay frames: the end clock already reached 1A7DD0; the
     // race update goes straight to the Results/replay tail each frame.
     const bool replayFrame50PD=native_common_results50PD::state().replayActive&&!native_common_results50PD::state().fallback;
     auto ending=replayFrame50PD?OriginalHumanEndClockResult50KG{OriginalHumanEndClockStatus50KG::needOriginalEndDispatch1A7DD0}:
         sourceCameraSession->advanceHumanEndRootWithOwners(.016f,raceMusic.get(),sourceAnnouncer.get());
     if(ending.status==OriginalHumanEndClockStatus50KG::needFade2AED60){
      require50DU(sourceCameraSession->requestHumanEndFade(ending.fadeMilliseconds).status==sourceCamera::Status::complete,
          "human ending deferred video request did not return");
     }else if(ending.status==OriginalHumanEndClockStatus50KG::needOriginalEndDispatch1A7DD0){
      // A Results replay frame follows 1AC8E0's restart and its own fade-in.
      require50DU(replayFrame50PD||(sourceCameraSession->endingVideoCovered()&&!sourceCameraSession->pendingDisplayWork()),
          "native Results requires genuine ending-clock and displayed-fade completion");
      // The common race whose original Results root was published enters the
      // original end entry 1A7DD0 (F8 visibility, Results activation 1A7E18,
      // HUD stop 1AD230, 1AB520) and keeps resuming it each frame. A hard stop
      // falls back to the host panel below, logged.
      auto& common=native_common_results50PD::state();
      auto& replay=native_race_replay50PD::owner();
      if(common.rootPublished&&!common.fallback&&common.replayActive){
       // Results replay frame: 287C90 playback, 1A8158 loop at the end, every
       // racer posed from race78 (racer +3C = 1 skips its own update), then
       // the camera and world children as in an ordinary race tick.
       const char* why=nullptr;
       // 1A7BD0..1A7C18 of this race update: the restart left race1B0 bit3
       // set; the canonical fade-in prefix requests and services the fade.
       {const auto fade=originalMainApplication.advanceConsumedSessionFadeIn1A7BD0(*sourceCameraSession);
        static unsigned reportedFade=0xffu;
        if(static_cast<unsigned>(fade.status)!=reportedFade){reportedFade=static_cast<unsigned>(fade.status);
         std::cerr<<"race_replay fade_in status="<<reportedFade<<" pc="<<std::hex<<fade.sourcePC<<std::dec
             <<" covered="<<sourceCameraSession->endingVideoCovered()<<'\n';}}
       require50DU(replay.step(.016f,false,why),why?why:"race78 playback step rejected");
       if(replay.ended()){
        require50DU(!common.restartWorld||common.restartWorld(why),why?why:"replay loop world restart rejected");
        require50DU(replay.loop(why),why?why:"race78 replay loop rejected");
        std::cerr<<"race_replay loop="<<replay.stats().loops<<'\n';
       }
       require50DU(racerBank.visitAll([&](std::size_t slot){return replay.apply(slot,s,x.bindings);}),
           replay.stats().lastRejection?replay.stats().lastRejection:"race78 replay pose rejected");
       const auto frame=++sourceCameraTick;
       const auto scene=sourceCameraSession->advanceSceneChild(frame,sourceCamera::Cut::replayPose,.016f);
       advanceWorldAnimations();advanceNativeGcWater();advanceRcTrackParticles();advanceTbTrackParticles();
       const auto cameraChild=scene.status==sourceCamera::Status::sceneChildComplete?
           sourceCameraSession->advanceCameraChild(frame,sourceCamera::Cut::replayPose):scene;
       if(cameraChild.status==sourceCamera::Status::framePublished){
        publishSourceCameraFog();
        // The Results update (1C8AE0) and its draw capture follow this
        // replay frame's returned Camera, as on every race tick.
        rr::game::OriginalResults104Diagnostic50LA resultsUpdate;
        const bool updated=originalMainApplication.updateNativeResultsAfterRace50LA(*sourceCameraSession,resultsUpdate);
        const bool captured=updated&&finishTaRootWithResults();
        if(!updated||!captured||replayFrames50PD<=3||replayFrames50PD%120==0){
         if(!updated||!captured||resultsUpdate.sourcePC!=replayResultsPC50PD)
          std::cerr<<"race_replay results updated="<<updated<<" captured="<<captured<<" pc="<<std::hex
              <<resultsUpdate.sourcePC<<std::dec<<" status="<<static_cast<unsigned>(resultsUpdate.status)
              <<" operation="<<(resultsUpdate.operation?resultsUpdate.operation:"")<<'\n';
         replayResultsPC50PD=resultsUpdate.sourcePC;
        }
       }
       // 1A7B40 re-enters 1A7DD0..1AB520 every update; 1AB5B0 reads +20C.
       {rr::game::OriginalRaceEndEntryDiagnostic50LA ack;
        originalMainApplication.continueConsumedRaceEndEntry1A7DD0(ack);
        {static const char* reportedAck=nullptr;
         if(ack.operation!=reportedAck){reportedAck=ack.operation;
          std::cerr<<"race_replay end_entry status="<<static_cast<unsigned>(ack.status)<<" pc="<<std::hex<<ack.sourcePC<<std::dec
              <<" ack_read="<<ack.acknowledgement20CRead<<" ack="<<ack.observedAcknowledgement20C
              <<" operation="<<(ack.operation?ack.operation:"")<<'\n';}}
        if(ack.acknowledgement20CRead&&ack.observedAcknowledgement20C&&!common.acknowledged){
         common.acknowledged=true;
         std::cerr<<"race_replay acknowledged value="<<ack.observedAcknowledgement20C<<" pc="<<std::hex<<ack.sourcePC<<std::dec
             <<" operation="<<(ack.operation?ack.operation:"")<<'\n';
         // 1AB5BC (App2E0 is 1 outside the predefined-track debug boot).
         // Tournament: s1 = 187B10, 1875C0 + 187D80 + the 1AB648 save bit
         // (one transaction below), then 26D070, which the Results confirm's
         // tournament branch already reached for these voices.
         commitCompletedTournament();
         rr::game::OriginalRaceAckFacts50PD facts;
         require50DU(originalMainApplication.riderProfile&&
             originalMainApplication.riderProfile->raceAcknowledgementFacts1AB5BC(facts),
             "1AB5BC acknowledgement profile facts unavailable");
         const auto destination=rr::game::originalRaceAckDestination1AB5BC(facts);
         common.route=destination;common.routeChosen=true;
         std::cerr<<"race_ack route="<<rr::game::originalRaceAckRouteName50PD(destination.route)
             <<" flags="<<destination.flags<<" screen="<<destination.screen<<" page="<<destination.page
             <<" pc="<<std::hex<<destination.sourcePC<<std::dec<<" mode34="<<facts.mode34
             <<" order="<<facts.order187B10<<" boundary102CC="<<facts.boundary102CC<<" record7E4="<<facts.record7E4
             <<" final_medal="<<unsigned(facts.finalMedal)<<" unlock187E30="<<facts.unlockPending187E30<<'\n';
         // Native receivers exist for these destinations; the others keep
         // the Results replay running (no request) until their screens exist.
         // Tournament (mode34==2) only: Garage13, Hall11, Unlock10, and Win7
         // (its 234880 single-play trophy music uses the same Aural song
         // stream as every other menu song). The ending row (17DE10
         // App,0x24,3,0) and the mode0 Hall/Unlock/Course rows stay
         // unreceived and logged.
         using AckRoute=rr::game::OriginalRaceAckRoute50PD;
         const bool tournamentScreen=destination.tournament&&destination.flags==6u&&destination.page==1u;
         const bool received=destination.route==AckRoute::garage||
             (tournamentScreen&&(destination.route==AckRoute::hall||destination.route==AckRoute::unlock||
              destination.route==AckRoute::win));
         if(received){
          const auto& owned=originalMainApplication.sourceRace28COwner_;
          // 1AB680: 1AC440 immediately precedes the 17DE10 Hall request.
          if(destination.insertRecords1AC440)
           require50DU(owned&&originalMainApplication.commitRaceAcknowledgementRecords1AC440_50PD(*owned,destination),
               "1AB5BC 1AC440 record insertion rejected");
          require50DU(owned&&originalMainApplication.commitRaceAcknowledgementTransfer50PD(*owned,destination),
              "1AB5BC 17DE10 request rejected");
          std::cerr<<"race_ack request queued screen="<<destination.screen<<'\n';
         }else std::cerr<<"race_ack receiver_unavailable route="<<rr::game::originalRaceAckRouteName50PD(destination.route)<<'\n';
        }}
       // The race keeps its audio world running behind the Results (music,
       // announcer, UI feedback, pending fades), as the completed-scene hold.
       if(audioEnabled){
        // The display effect service pauses audio while a fade runs; this
        // real replay tick resumes it, as the countdown tick does.
        liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);
       }
       if(audioEnabled&&liveAudio->outputOpen()){
        phase_timing50GK::Scope audioTiming(phaseTiming,phase_timing50GK::Phase::AudioCommit);
        const auto beforeMix=liveAudio->mixedFrames();
        const bool mixed=liveAudio->committedStep(audioDiagnostic);
        advanceSourceAudioWorld(beforeMix,true);
        if(!mixed)std::cerr<<"race_replay audio_error="<<static_cast<unsigned>(audioDiagnostic.error)<<'\n';
        require50DU(mixed,"native Results replay audio step failed");
       }
       ++replayFrames50PD;
       if(replayFrames50PD<=3||replayFrames50PD%120==0||cameraChild.status!=sourceCamera::Status::framePublished){
        const auto& pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
        const auto st=replay.playTime();
        if(replayFrames50PD<=3||replayFrames50PD%120==0||!replayCameraReported50PD){
         std::cerr<<"race_replay play="<<st<<" frames="<<replayFrames50PD<<" applied="<<replay.stats().applied
             <<" loops="<<replay.stats().loops<<" scene="<<static_cast<unsigned>(scene.status)
             <<" camera="<<static_cast<unsigned>(cameraChild.status)<<" camera_pc="<<std::hex<<cameraChild.sourcePC<<std::dec
             <<" player=("<<pose.v[12]<<','<<pose.v[13]<<','<<pose.v[14]<<")\n";
         if(cameraChild.status!=sourceCamera::Status::framePublished)replayCameraReported50PD=true;
        }
       }
       forceSceneRender=true;break;
      }
      if(common.rootPublished&&!common.fallback){
       rr::game::OriginalRaceEndEntryDiagnostic50LA endDiagnostic;
       originalMainApplication.continueConsumedRaceEndEntry1A7DD0(endDiagnostic);
       using EndStatus=rr::game::OriginalRaceEndEntryDiagnostic50LA::Status;
       const bool changed=common.lastPC!=endDiagnostic.sourcePC||common.lastStatus!=static_cast<std::uint8_t>(endDiagnostic.status);
       ++common.endCalls;common.endActive=true;
       common.lastPC=endDiagnostic.sourcePC;common.lastStatus=static_cast<std::uint8_t>(endDiagnostic.status);
       common.lastOperation=endDiagnostic.operation;
       if(changed||common.endCalls<=3||common.endCalls%120==0)
        std::cerr<<"common_race_end_entry call="<<common.endCalls<<" status="<<static_cast<unsigned>(endDiagnostic.status)
            <<" pc="<<std::hex<<endDiagnostic.sourcePC<<std::dec
            <<" f8="<<endDiagnostic.f8VisibilityCommitted<<" results="<<endDiagnostic.resultsActivationCommitted
            <<" hud_stop="<<endDiagnostic.hudStopCommitted<<" ack_read="<<endDiagnostic.acknowledgement20CRead
            <<" waiting="<<endDiagnostic.waitingBit5Committed<<" dispatch_returned="<<endDiagnostic.dispatchReturned1AB520
            <<" operation="<<(endDiagnostic.operation?endDiagnostic.operation:"")<<'\n';
       if(endDiagnostic.status==EndStatus::incomplete||endDiagnostic.status==EndStatus::callerReturned){
        forceSceneRender=true;break;
       }
       common.fallback=true;
       std::cerr<<"common_race_end_entry fallback=host_panel\n";
      }
      // Explicit native presentation substitution at1A7DD0, before the
      // source launch1 UI199770/race104 virtual+20, HUD-stop1AD230 and
      // race virtual+78 dispatch.1AD230 is not a Results implementation.
      // No retail profile/save/progression receipt is issued by this gate.
      humanResultsReady=true;forceSceneRender=true;
      std::cerr<<"NATIVE HUMAN RESULTS: genuine source end predicate, countdown, audio wait and displayed fade complete; "
          "source standings retained. Native presentation substitutes at1A7DD0 before retail end dispatch; "
          "HUD-stop1AD230 and retail result/profile/save/progression owners were not executed.\n";
      break;
     }else require50DU(ending.status==OriginalHumanEndClockStatus50KG::advanced||
         ending.status==OriginalHumanEndClockStatus50KG::unchanged,"actual human ending root remains at a source boundary");
    }
    bool cameraAllowsSimulation=true;
    if(sourceCameraSession){
     if(isRcTrack)requireMbRuntimeNavigation50MB();
     heldLap50DU("before_start_gate");
     const auto gate=sourceCameraSession->advanceNativeStartGate(.016f);
     heldLap50DU("start_gate");
     require50DU(gate.status==sourceCamera::Status::simulationAllowed||gate.status==sourceCamera::Status::startHeld,
         "original camera single native start gate rejected");
     cameraAllowsSimulation=gate.status==sourceCamera::Status::simulationAllowed;
     if(!cameraAllowsSimulation){
      const auto frame=++sourceCameraTick;
      require50DU(sourceCameraSession->advanceSceneChild(frame,sourceCamera::Cut::heldBeforeStart,.016f).status==
          sourceCamera::Status::sceneChildComplete,"held original scene child rejected");
      heldLap50DU("held_scene_child");
      advanceWorldAnimations();
      advanceNativeGcWater();
      advanceRcTrackParticles();
      advanceTbTrackParticles();
      heldLap50DU("held_world_animation");
      if(sourceCameraSession->willUseFollowPose())
       require50DU(racerBank.visit(0,[&](){return refreshAnimatedCandidates();}),
          "Follow held frame actual animated collision refresh failed");
      heldLap50DU("held_follow_refresh");
      const auto cameraChild=sourceCameraSession->advanceCameraChild(frame,sourceCamera::Cut::heldBeforeStart);
      heldLap50DU("held_camera_child");
      if(cameraChild.status==sourceCamera::Status::videoPending){
       require50DU(sourceCameraSession->taSkipVideoPending()&&!suspendedTaCameraFrame,
           "only the actual TA synchronous skip may suspend a held camera frame");
       suspendedTaCameraFrame=frame;
       hostClock.pause(monotonicNow50DU());
       break;
      }
      if(cameraChild.status!=sourceCamera::Status::framePublished)
       std::cerr<<"held_camera_child_rejected status="<<static_cast<unsigned>(cameraChild.status)<<" pc="<<std::hex
           <<cameraChild.sourcePC<<std::dec<<" frame="<<frame<<'\n';
      require50DU(cameraChild.status==sourceCamera::Status::framePublished,"held original camera child rejected");
      publishSourceCameraFog();forceSceneRender=true;
     }
    }
    if(raceEnabled&&(sourceCameraSession?!cameraAllowsSimulation:!withRace([&](auto& actualRace){return race_session50HM::advanceCountdown(actualRace,.016);}))){
     phase_timing50GK::Scope countdownTiming(phaseTiming,phase_timing50GK::Phase::CountdownService);
     require50DU(race.error==race_session50HM::Error::None,"race countdown rejected");
     if(!sourceCameraSession)advanceWorldAnimations();
     if(audioEnabled){
      liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);
      if(!sourceCameraSession||sourceCameraSession->countdownInstalled())
       require50DU(withRace([&](auto& actualRace){return raceAudio->observe(actualRace);}).accepted,"countdown/result audio observation rejected");
      heldLap50DU("countdown_audio_observe");
      updateOpponentAudio(); // Only countdown holds native vehicle updates.
      heldLap50DU("countdown_opponent_audio");
      countdownTiming.finish(); // Keep countdown service disjoint from the actual shared mix/output.
      // Countdown has a real host clock while native vehicle bodies are held.
      // Advance the shared mixer ONCE; completed results keep normal world steps.
      if(liveAudio->outputOpen()){
       phase_timing50GK::Scope audioTiming(phaseTiming,phase_timing50GK::Phase::AudioCommit);
       const auto beforeMix=liveAudio->mixedFrames();
       const bool mixed=liveAudio->committedStep(audioDiagnostic);
       heldLap50DU("countdown_audio_mix");
       advanceSourceAudioWorld(beforeMix,true); //Actual countdown held route.
       heldLap50DU("countdown_audio_world");
       require50DU(mixed,"native countdown/result audio step failed");
      }
     }
     if(sourceCameraSession){
      // All real held Camera/countdown/audio callbacks above returned. This
      // is the native Race-return cut, not full source1A7B40 or141ED0. Earlier
      // partial/exception/suspended branches never reach this Results offer.
      rr::game::OriginalResults104Diagnostic50LA resultsUpdate;
      const bool updated=originalMainApplication.updateNativeResultsAfterRace50LA(
          *sourceCameraSession,resultsUpdate);
      require50DU(updated,resultsUpdate.operation?resultsUpdate.operation:
          "native held-root Results update rejected");
      heldLap50DU("held_results_update");
      require50DU(finishTaRootWithResults(),"native held-root accepted Results capture rejected");
      heldLap50DU("ta_root_finish");
     }
     // The final Intro pose must reach an actual displayed publication before
     // the next held tick may execute its exit/fade. Do not skip that image in
     // a multi-step catch-up plan. Unused slots are an explicit display hold.
     if(sourceCameraSession&&sourceCameraSession->introExitReady()&&
        renderedCameraPublication!=sourceCameraSession->publicationCount()){
      addCounter50DU(discardedHostNs,static_cast<std::uint64_t>(plan.steps-step-1u)*stepNs);break;
     }
     continue;
    }
    if(!sourceCameraSession&&originalAiEnabled&&!(originalAiSession.manager.flags58&2u)){
     // One actual started-byte owner for the whole native slice. The selected
     // HOST countdown publishes its GO edge once, before any racer executes.
     originalAiSession.manager.flags58=static_cast<std::uint8_t>(originalAiSession.manager.flags58|2u);
     for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
      s.fields.applicationByte58=originalAiSession.manager.flags58;
     });
    }
    stall_watchdog50DU::mark("step_after_countdown");
    const bool playerAdvancing=!raceEnabled||withRace([&](auto& actualRace){return race_session50HM::simulationAllowed(actualRace,0);});
    const bool playerControls=!raceEnabled||withRace([&](auto& actualRace){return race_session50HM::controlsAllowed(actualRace,0);});
    const bool playerFinishDriver=humanResultsActive&&(s.fields.impactFlagsCD8&1u);
    const bool manualRaceInputHeld=!allAiEnabled&&(usePhysicalGameController?
        platform::selectedControllerHeld50KY(input.controllers[0]):platform::selectedKeyboardHeld50KY(input,humanControlInput));
    if(audioEnabled){liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);}
    // Recompose at GO so held race input enters this exact native frame.
    // After the human finish, the recovered source driver owns controls while
    // the ordinary coupled-body/native frame keeps advancing.
    if(playerAdvancing&&!allAiEnabled){
     composePacket(playerControls,input);
     if(playerControls&&autoDriveEnabled)applyCourseInput();
     sampleController(true);
     // Countdown may have switched to GO inside this catch-up batch. Observe
     // real pad readiness again before any newly permitted human physics.
     if(usePhysicalGameController&&playerControls&&
        physicalControllerReceipt.status!=platform::SelectedControllerStatus50KY::live){
      // The release packet has already been dispatched and acknowledged.
      // This branch sets wasPaused before the outer pause path can stop the
      // motors. Stop the physical lease here as on every other pause entry;
      // do not replay releases or clear source feedback timers/actuator state.
      stopFeedbackTransport();
      if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
      hostClock.pause(now);wasPaused=true;break;
     }
    }
    if(sourceRaceHud.loaded()&&playerAdvancing)
     require50DU(sourceRaceHud.advanceAnimations29CDA0(.016f),"actual HUD animation update rejected");
    const auto nativeFrameAllocations=::allocations;
    frameHitch50DU.physics=FrameHitch50DU::Clock::now();
    phase_timing50GK::Scope physicsTiming(phaseTiming,phase_timing50GK::Phase::Physics);
    if(playerAdvancing&&!sourceCameraSession){
     captureDynamicBeforeFrame(0);
     // Source AI peer reads resolve live racers in serial update order. This
     // copies current owners only: manager timing/CC4/CCC do not advance here.
     if(originalAiEnabled)observeOriginalAiFleet();
    }
    const bool frameAdvanced=sourceCameraSession?
        advanceOriginalAll8TwoPhaseFrame(sourceFrameSchedule,
            originalAiAudioEnabled?original_all8_frame::AudioIntegrationPolicy::nativeAiD40AtNativeAudioPhase:
                original_all8_frame::AudioIntegrationPolicy::retainHostSelectedPlayerDerivedSuffixPolicy,
            [&](original_all8_frame::Schedule& schedule){
             if(isRcTrack)requireMbRuntimeNavigation50MB();
             const auto frame=++sourceCameraTick;
             {SimulationScope timing(phaseTiming.enabled,simulationTotals[7]);
             require50DU(sourceCameraSession->advanceSceneChild(frame,sourceCamera::Cut::afterAllRacerPrefixes,.016f).status==
                 sourceCamera::Status::sceneChildComplete,"original scene missed racer-prefix cut");
             advanceWorldAnimations();
             advanceNativeGcWater();
             advanceRcTrackParticles();
             advanceTbTrackParticles();
             }
             {SimulationScope timing(phaseTiming.enabled,simulationTotals[8]);
             advanceOriginalAiManager(.016f,&schedule);}
             SimulationScope cameraTiming(phaseTiming.enabled,simulationTotals[9]);
             if(sourceSpeedBoost.owner)sourceSpeedBoost.owner->invalidateNativeDraw();
             if(sourceDamage.owner)sourceDamage.owner->invalidateNativeDraw();
             if(sourceTimeStats.owner)
              require50DU(sourceTimeStats.owner->advanceNativeWorld(.016f),
                  "original TimeStats accepted-world animation update rejected");
             if(sourceCameraSession->willUseFollowPose())
              require50DU(racerBank.visit(0,[&](){return refreshAnimatedCandidates();}),
                 "Follow actual animated collision refresh failed");
             // 177AB0 cuts to 22DC90 on the destroy frame itself: the human's
             // debris (spawned inside this tick's racer prefix) is handed to
             // the death camera before this camera child, so no hold frame runs.
             if(rr::game::nativeRacerDestroyed50PD(0)&&!native_wreck_debris50PD::cameraViewBound(0)){
              const auto& identity=sourceCameraSession->deathIdentity();
              const bool bound=native_wreck_debris50PD::bindCameraView(0,identity.sessionGeneration,identity.racerKey);
              if(bound)native_wreck_debris50PD::updateCameraView();
              std::cerr<<"death_camera_debris bound="<<bound<<" pieces="<<native_wreck_debris50PD::cameraView().count
                  <<" collections="<<native_wreck_debris50PD::cameraView().view.count370<<" at=camera_child frame="<<frame<<'\n';
             }
             const auto cameraChild=sourceCameraSession->advanceCameraChild(frame,sourceCamera::Cut::afterAllRacerPrefixes);
             if(cameraChild.status!=sourceCamera::Status::framePublished){
              const auto progress=sourceCameraSession->nativeStartProgress();
              const auto& attached=cameraChild.stableAttached;const auto& follow=cameraChild.stableFollow;
              std::fprintf(stderr,
                  "original_camera_prephysics_rejected frame=%llu status=%u pc=%08x stage=%u frame_match=%u cut_match=%u cut_accepted=%u mode=%d player_state=%u player_phase=%u "
                  "start=%u/%08x follow_pose=%u/%08x attached_pose=%u/%08x retirement=%u/%08x death=%u/%08x spatial=%u "
                  "attached_valid=%u attached_stage=%u attached_control=%u attached_tick=%u attached_visibility=%u/%u/%u/%u attached_committed=%u/%u/%u "
                  "follow_valid=%u follow_stage=%u follow_control=%u follow_tick=%u follow_visibility=%u/%u/%u/%u follow_committed=%u/%u/%u\n",
                  static_cast<unsigned long long>(frame),unsigned(cameraChild.status),cameraChild.sourcePC,cameraChild.cameraChildStage,
                  unsigned(cameraChild.cameraFrameMatches),unsigned(cameraChild.cameraCutMatches),unsigned(cameraChild.cameraCutAccepted),
                  progress[0],unsigned(s.bodies.main.vehicleState40),unsigned(s.phase),
                  unsigned(cameraChild.start.status),cameraChild.start.sourcePC,unsigned(cameraChild.follow.status),cameraChild.follow.sourcePC,
                  unsigned(cameraChild.attached.status),cameraChild.attached.sourcePC,unsigned(cameraChild.retirement.status),cameraChild.retirement.sourcePC,
                  unsigned(cameraChild.death.status),cameraChild.death.sourcePC,unsigned(cameraChild.spatial.status),
                  unsigned(cameraChild.stableAttachedAttempted),unsigned(attached.status),unsigned(attached.control.result),unsigned(attached.tick),
                  unsigned(attached.visibility.status),unsigned(attached.visibility.boundsStatus),unsigned(attached.visibility.geometryStatus),unsigned(attached.visibility.registryStatus),
                  unsigned(attached.controlCommitted),unsigned(attached.poseCommitted),unsigned(attached.sceneTickCommitted),
                  unsigned(cameraChild.stableFollowAttempted),unsigned(follow.status),unsigned(follow.control.result),unsigned(follow.tick),
                  unsigned(follow.visibility.status),unsigned(follow.visibility.boundsStatus),unsigned(follow.visibility.geometryStatus),unsigned(follow.visibility.registryStatus),
                  unsigned(follow.controlCommitted),unsigned(follow.poseCommitted),unsigned(follow.sceneTickCommitted));
             }
             require50DU(cameraChild.status==sourceCamera::Status::framePublished,"original camera missed pre-physics cut");
             publishSourceCameraFog();return true;
            }):!playerAdvancing||advanceFrame(0);
    if(originalAiEnabled&&!frameAdvanced){
     std::cerr<<"ORIGINAL AI world boundary: slot=0 phase="<<static_cast<unsigned>(s.phase)
         <<" frame_source_stage="<<std::hex<<x.diagnostic.sourceStage<<std::dec
         <<"; native partial state retained; manager was not advanced.\n";
     require50DU(false,"original AI requires the camera-followed racer frame to complete");
    }
    if(frameAdvanced)require50DU(::allocations==nativeFrameAllocations,
        "successful player physics frame allocated C++ storage");
    if(frameAdvanced&&raceEnabled){
     if(playerFinishDriver){
      require50DU(finishDriverAppliedThisFrame,"finished player frame missed native control owner");
      ++finishDriverTicks;
      if(manualRaceInputHeld)++finishManualInputTicks;
     }
     if(originalAiEnabled&&!sourceCameraSession){observeOriginalAiFleet();updateOriginalAiPacking();}
     const platform::WindowInput50DQ noKeys{};
     if(!sourceCameraSession)for(std::size_t racer=1;racer<racerBank.count();++racer){
      if(!racerBank.active(racer)||!withRace([&](auto& actualRace){return race_session50HM::simulationAllowed(actualRace,static_cast<std::uint32_t>(racer));}))continue;
      opponentFrame=true;
      try {
       captureDynamicBeforeFrame(racer);
       // Outside Bank::step/visit: no nested arena lease, no stale earlier
       // racer position/C80/speed after that racer completed its native frame.
       if(originalAiEnabled)observeOriginalAiFleet();
       const bool advanced=racerBank.step(racer,true,[&](){
        if(s.fields.vehicle58==0u){composePacket(true,noKeys,racer);applyCourseInput(racer);sampleController();}
        const auto beforeOpponentFrame=::allocations;
        const bool completed=advanceFrame(racer);
        if(completed)require50DU(::allocations==beforeOpponentFrame,
            "successful opponent physics frame allocated C++ storage");
        return completed;
       });
       if(!advanced)std::cerr<<racerNames[racer]<<" stopped at a native source boundary"
           <<(originalAiEnabled?"; joined AI world cannot continue.\n":"; other racers continue.\n");
      }catch(const std::exception& error){
       racerBank.stop(racer);
       std::cerr<<racerNames[racer]<<" stopped: "<<error.what()
           <<(originalAiEnabled?". Joined AI world cannot continue.\n":". Other racers continue.\n");
      }
      opponentFrame=false;
      // Deliberately outside the per-opponent catch: the optional source-AI
      // world cannot swallow a requisite racer's failure and advance later
      // racers, animations, contacts, manager timing or rank from that frame.
      if(originalAiEnabled)requireOriginalAiFleetReady();
     }
     if(!sourceCameraSession)advanceWorldAnimations();
     // Exactly one host contact pass after all successful native participants
     // are Ready, before race observations and the single world audio mix.
     // A failed native slot never participates through an abandoned snapshot.
     // Common race recording (race78). 1AD9A0 channels bind once the Results
     // root (and with it race78) exists; 1AFE30 starts recording at GO (the
     // timing manager's racing bit); 287C90 then runs every fixed step.
     {auto& common=native_common_results50PD::state();auto& replay=native_race_replay50PD::owner();
      const auto& race=originalMainApplication.sourceRace28COwner_;
      if(common.rootPublished&&race&&native_race_replay50PD::Owner::commonRace(*race)&&sourceCameraSession){
       const char* why=nullptr;
       if(replay.phase()==native_race_replay50PD::Owner::Phase::idle){
        std::array<native_race_replay50PD::RacerTraits,native_race_replay50PD::maxRacers> traits{};
        const auto count=std::min<std::size_t>(racerBank.count(),traits.size());
        require50DU(racerBank.visitAll([&](std::size_t slot){
            if(slot<traits.size())traits[slot]={s.fields.vehicle58==1u,x.configurationEntryNonzero,
                x.bindings.visual.extra1C0Present,x.bindings.visual.extra1C4Present};return true;}),
            "race78 channel traits read rejected");
        const bool bound=replay.bind(race,traits.data(),count,why);
        std::cerr<<"race_replay bind="<<bound<<" racers="<<count<<" why="<<(why?why:"")<<'\n';
        if(!bound)common.fallback=true;
        common.restartWorld=[](const char*& reason){
         // TA's world has no RA trigger list or physics index: its native
         // world restart is the particle/animation clear below.
         reason=nullptr;native_wreck_effects50PD::reset();return true;};
        common.beginReplay=[this](const char*& reason){
         replayFrames50PD=0;replayCameraReported50PD=false;
         // The original race no longer opens its Pause once finished
         // (1A7CDC). Retire the host Pause drawing so Results' own 14D440
         // collector rebuild is not blocked by the Pause admission's pin.
         if(sourcePauseObservation.hostOpen())sourcePauseObservation.closeHost();
         if(sourcePauseObservation.hostBound())sourcePauseObservation.unbind();
         {rr::platform::VehicleWindowDiagnostic50DS pauseRetire;
          if(!renderer.retireOriginalPauseDerived(pauseRetire)||!sourcePause.retireDrawing()){
           reason="host Pause drawing retirement at Results start rejected";return false;}}
         const bool rearmed=sourceCameraSession&&sourceCameraSession->rearmFadeInAfterRestart50PD();
         std::cerr<<"race_replay fade_in_rearmed="<<rearmed<<'\n';
         const bool begun=native_race_replay50PD::owner().beginReplay(reason);
         const auto& owned=originalMainApplication.sourceRace28COwner_;
         const auto st=owned?native_race_replay50PD::Owner::vcrState(*owned):rr::game::OriginalSimpleVCR50LA::State{};
         std::cerr<<"race_replay begin="<<begun<<" channels="<<st.channels<<" bytes="<<st.bytesUsed<<"/"<<st.capacityBytes
             <<" keyframes="<<st.keyframes<<" deltas="<<st.deltas<<" escapes="<<st.escapes<<" full="<<st.full
             <<" recorded="<<st.time30<<" native_samples="<<native_race_replay50PD::owner().stats().nativeSamples
             <<" why="<<(reason?reason:"")<<'\n';
         return begun;};
       }
       if(replay.phase()==native_race_replay50PD::Owner::Phase::bound&&(originalAiSession.manager.flags58&2u)){
        const bool started=replay.startRecording(why);
        std::cerr<<"race_replay record="<<started<<" why="<<(why?why:"")<<'\n';
        if(!started)common.fallback=true;
       }
       if(replay.phase()==native_race_replay50PD::Owner::Phase::recording){
        if(replay.captureDue())
         require50DU(racerBank.visitAll([&](std::size_t slot){return replay.capture(slot,s,x.bindings);}),
             replay.stats().lastRejection?replay.stats().lastRejection:"race78 capture rejected");
        require50DU(replay.step(.016f,false,why),why?why:"race78 record step rejected");
        const auto st=native_race_replay50PD::Owner::vcrState(*race);
        const auto written=st.keyframes+st.deltas;
        if(written!=replayRecordLogged50PD&&(written==1||written%75==0)){replayRecordLogged50PD=written;
         const auto& pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
         std::cerr<<"race_replay rec="<<st.time30<<" frames="<<st.keyframes+st.deltas<<" bytes="<<st.bytesUsed
             <<" escapes="<<st.escapes<<" full="<<st.full<<" player=("<<pose.v[12]<<','<<pose.v[13]<<','<<pose.v[14]<<")\n";
        }
       }
      }}
     // Sealed results alone do not remove a Ready native vehicle from contacts.
     // Original 225C50 suffix of a destroy handler, at this world boundary:
     // attacker CD0 kill credit (225F34) and the 1A46E0 music song chosen at
     // the transition. The human's player-event CD0 is credited in the frame.
     for(std::size_t slot=0;slot<racerBank.count()&&slot<rr::game::nativeDestroyedRacers50PD.size();++slot){
      auto& destroyed=rr::game::nativeDestroyedRacers50PD[slot];
      if((!destroyed.destroyed&&!destroyed.limping)||destroyed.suffixApplied)continue;
      destroyed.suffixApplied=true;
      if(destroyed.killCreditPending){
       const auto attacker=static_cast<std::size_t>(destroyed.attackerD30);
       std::uint32_t kills=0;
       racerBank.visit(attacker,[&](){++lapContinuity.timing.countCD0;kills=lapContinuity.timing.countCD0;});
       std::cerr<<"kill_credit attacker="<<attacker<<" victim="<<slot<<" kills="<<kills<<'\n';
      }
      if(destroyed.musicSong){
       int result=-1;bool requested=false;
       if(raceMusic&&liveAudio->outputOpen()){
        const auto receipt=raceMusic->request(destroyed.musicSong,0u,dynamicMusicProfile.mode11258);
        result=static_cast<int>(receipt.result);requested=receipt.completed();
       }
       std::cerr<<"destroy_music song="<<destroyed.musicSong<<" victim="<<slot<<" requested="<<requested
           <<" result="<<result<<" output_open="<<liveAudio->outputOpen()<<" owner="<<(raceMusic?1:0)<<'\n';
      }
     }
     // Wreck groups' break tests and kicks draw from the race's shared 308030
     // stream (2CAE80.., 2CAF94..); the free pieces draw nothing.
     racerBank.visit(0,[&](){native_wreck_debris50PD::step(.016f,s.noiseOwner().sharedRandomState);});
     // The camera racer's debris becomes the death camera's 22D4B0 view
     // (rebound to the Session's prepared death identity) once it exists.
     if(rr::game::nativeRacerDestroyed50PD(0)&&sourceCameraSession&&!native_wreck_debris50PD::cameraViewBound(0)){
      const auto& identity=sourceCameraSession->deathIdentity();
      const bool bound=native_wreck_debris50PD::bindCameraView(0,identity.sessionGeneration,identity.racerKey);
      std::cerr<<"death_camera_debris bound="<<bound<<" pieces="<<native_wreck_debris50PD::cameraView().count
          <<" collections="<<native_wreck_debris50PD::cameraView().view.count370<<'\n';
     }
     native_wreck_debris50PD::updateCameraView();
     native_wreck_effects50PD::step(.016f);
     {const auto& fx=native_wreck_effects50PD::state();std::size_t embers=0;float low=1e30f,high=-1e30f,reach=0.f;
      for(const auto& e:fx.emitters)if(e.alive&&e.ballistic){++embers;low=std::min(low,e.position[1]);high=std::max(high,e.position[1]);
       reach=std::max(reach,std::hypot(e.velocity[0],e.velocity[2]));}
      if(embers&&fx.steps%15==1)std::cerr<<"wreck_embers step="<<fx.steps<<" alive="<<embers<<" y="<<low<<".."<<high
          <<" max_horizontal_speed="<<reach<<" launched_total="<<fx.embersLaunched<<'\n';}
     {const auto& debris=native_wreck_debris50PD::sim();
      if(!debris.pieces.empty()&&debris.steps%125==1){std::size_t settled=0;float lowest=1e30f,highest=-1e30f;
       for(const auto& p:debris.pieces){settled+=p.settled;lowest=std::min(lowest,p.position.y);highest=std::max(highest,p.position.y);}
       std::cerr<<"debris_state live="<<debris.pieces.size()<<" settled="<<settled<<" retired="<<debris.retired
           <<" contacts="<<debris.contacts<<" y="<<lowest<<".."<<highest<<" steps="<<debris.steps
           <<" queries="<<debris.queries<<" invalid="<<debris.invalid<<" capacity="<<debris.capacity
           <<" unsupported="<<debris.unsupported;
       for(const auto& p:debris.pieces){const float dx=p.position.x-p.spawn.x,dz=p.position.z-p.spawn.z;
        std::cerr<<" ["<<p.racer<<'.'<<p.index<<(p.settled?" S":p.retired?" R":" F")<<" dy="<<p.position.y-p.spawn.y
            <<" dh="<<std::sqrt(dx*dx+dz*dz)<<" c="<<p.contacts<<" last="<<p.lastContactStep
            <<" v="<<std::sqrt(p.velocity.x*p.velocity.x+p.velocity.y*p.velocity.y+p.velocity.z*p.velocity.z)<<']';}
       {const auto& fx=native_wreck_effects50PD::state();
        std::cerr<<" effects particles="<<fx.particles.size()<<" emitters="<<fx.emitters.size()
            <<" spawned="<<fx.spawned<<" dropped="<<fx.dropped;}
       if(rr::game::nativeRacerDestroyed50PD(0)&&sourceCameraSession){
        const auto& pose=sourceCameraSession->pose();const auto& view=native_wreck_debris50PD::cameraView();
        float cx=0,cy=0,cz=0;std::size_t n=0;
        for(std::size_t k=0;k<view.count;++k){const auto* m=view.nodes[k].localC0.v;cx+=m[12];cy+=m[13];cz+=m[14];++n;}
        if(n){cx/=float(n);cy/=float(n);cz/=float(n);}
        const float ex=pose.position.x-cx,ey=pose.position.y-cy,ez=pose.position.z-cz;
        std::cerr<<" camera_mode="<<pose.mode
            <<" camera_to_debris="<<std::sqrt(ex*ex+ey*ey+ez*ez)<<" camera_height="<<ey;
       }
       std::cerr<<'\n';}}
     if(racerContactsEnabled){
      SimulationScope timing(phaseTiming.enabled,simulationTotals[10]);
      std::array<bool,native_racer_contact50HW::maxRacers> contactParticipant{};
      std::array<native_racer_contact50HW::Pair,
          native_racer_contact50HW::maxRacers*(native_racer_contact50HW::maxRacers-1)> contactOrder{};
      std::size_t contactPairCount=0;
      for(std::size_t racer=0;racer<racerBank.count();++racer)
       contactParticipant[racer]=racerBank.active(racer)&&
           // A destroyed racer's 690/694/364 memberships are removed (2261B0/226750).
           !rr::game::nativeRacerDestroyed50PD(racer)&&
           withRace([&](auto& actualRace){return race_session50HM::simulationAllowed(actualRace,static_cast<std::uint32_t>(racer));});
      for(std::size_t primary=0;primary<racerBank.count();++primary)if(contactParticipant[primary])
       for(std::size_t other=0;other<racerBank.count();++other)
        if(primary!=other&&contactParticipant[other])contactOrder[contactPairCount++]={primary,other};
      if(contactPairCount){
       native_racer_contact50HW::Receipt contactReceipt{};
       native_racer_contact50HW::AiContactBoundary50IV aiContactBoundary{};
       const auto beforeRacerContacts=::allocations;
       const auto contactStatus=native_racer_contact50HW::applyAtWorldBoundary(
           racerBank,*racerContactOwner,s,x.bindings,x.parameters,racerContactIds,racerContactMaterials,
           contactOrder.data(),contactPairCount,contactReceipt,&aiContactBoundary,
           dynamicContactEnabled,dynamicContactEnabled?&dynamicRegistry:nullptr);
       racerContactTotals.lastStatus=contactStatus;
       if(contactStatus!=native_racer_contact50HW::Status::Complete){
        addCounter50DU(racerContactTotals.failures);
        std::cerr<<"RACER CONTACT FAILURE: player_tick="<<completedFrames<<" ordered_pairs="<<contactPairCount
            <<" status="<<static_cast<unsigned>(contactStatus)<<"; rejected body outputs were not committed.\n";
        if(aiContactBoundary.unsupported)std::cerr<<"ORIGINAL AI dynamic-contact owner boundary: slot="
            <<aiContactBoundary.racerSlot<<" pc="<<std::hex<<aiContactBoundary.sourcePC<<std::dec
            <<" contacts="<<aiContactBoundary.contactCount20C<<"; no pair results committed.\n";
        reportRacerContacts();
        require50DU(false,"native racer contact world pass rejected");
       }
       require50DU(collisionNormalMode.acceptCompleted(contactReceipt.normalModeWrite),"source collision normal-mode binding unavailable");
       require50DU(::allocations==beforeRacerContacts,"successful racer contact world pass allocated C++ storage");
       addCounter50DU(racerContactTotals.passes);
       addCounter50DU(racerContactTotals.testedPairs,contactReceipt.testedPairs);
       addCounter50DU(racerContactTotals.geometryPairs,contactReceipt.geometryPairs);
       addCounter50DU(racerContactTotals.suppressedPairs,contactReceipt.suppressedPairs);
       addCounter50DU(racerContactTotals.respondedPairs,contactReceipt.respondedPairs);
       addCounter50DU(racerContactTotals.generatedRecords,contactReceipt.generatedRecords);
       addCounter50DU(racerContactTotals.activeRecords,contactReceipt.activeRecords);
      }else addCounter50DU(racerContactTotals.noPairs);
     }
     if(originalAiEnabled&&!sourceCameraSession)advanceOriginalAiManager(m.frameFactor);
     if(mode1Enabled){
      const auto timing=mode1->beforeHostCommit(mode1->acceptedWorld+1u,.016f);
      require50DU(timing.status==race_mode1_host50ID::Status::AwaitHostCommit,
          "mode1 source timing boundary unresolved; accepted world not replayed");
     }
     {SimulationScope timing(phaseTiming.enabled,simulationTotals[11]);sampleRaceField();}
     SimulationScope presentationTiming(phaseTiming.enabled,simulationTotals[12]);
     if(nativeOriginalFleet){
      publishOriginalAiStandings();
      if(audioEnabled)require50DU(withRace([&](auto& actualRace){return raceAudio->observe(actualRace);}).accepted,
          "native ALL8 presentation audio observation rejected");
     }else{
     const auto previousFinishCount=race.finishCount;
     const bool playerWasFinished=race.racers[0].finished;
     require50DU(withRace([&](auto& actualRace){return race_session50HM::commitFrame(actualRace,.016);}),"completed race world frame rejected");
     // Original1B035C sets the finished record+48 bit0, alias vehicle+CD8.
     // Project each newly sealed edge into its independent native state before
     // another frame. No body pose/velocity or source lap counter is written.
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      if(race.racers[racer].finished&&race.racers[racer].finishPlace>previousFinishCount)
       racerBank.visit(racer,[&](){
        s.fields.impactFlagsCD8=static_cast<std::uint8_t>(s.fields.impactFlagsCD8|1u);
        lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
        observedNativeFinishFlags[racer]=s.fields.impactFlagsCD8;
       });
     if(race.racers[0].finished&&!playerWasFinished){
      //1AF9C0 closes the manager after its completed record pass: preserve all
      // genuine same-world-frame finish edges, then freeze official standings.
      require50DU(withRace([&](auto& actualRace){return race_session50HM::closeResults(actualRace);}),"human finish result closure rejected");
      std::cerr<<"RACE FINISH: player place="<<race.racers[0].finishPlace
          <<"/"<<race.racerCount<<" time="<<race.racers[0].finishSeconds<<" seconds.\n";
      // Source CD8 warning subscribers stop their own warning loops. Engine,
      // binder, material and other-racer voices keep their original owners;
      // never stop the shared mixer at a human finish.
      // Original manager ends once every human finishes: this host has one.
      // Its ended-bit projection belongs to the human bank. Native AI checks
      // only started-bit1; the unjoined AI's HOST player-frame adapters retain
      // their started gate2 so they can continue their real navigation/physics.
      s.fields.applicationByte58=static_cast<std::uint8_t>(s.fields.applicationByte58|1u);
      humanResultsActive=true;humanResultsCountdown=4.f;
     }
     if(mode1Enabled){
      // HOST menus admit no world/event flush at all. No original modal menu
      // or application1B0 projection is installed: explicit1B0 remains0 here.
      const auto boundary=mode1->afterHostCommit(mode1->acceptedWorld+1u,0);
      require50DU(boundary.status==race_mode1_host50ID::Status::NeedFlush||
          boundary.status==race_mode1_host50ID::Status::Complete,"mode1 HOST observer handoff failed");
     }
     if(audioEnabled)require50DU(withRace([&](auto& actualRace){return raceAudio->observe(actualRace);}).accepted,
         "committed race audio observation rejected");
     }
    }
    if(frameAdvanced&&!raceEnabled)advanceWorldAnimations();
    physicsTiming.finish();
    if(!frameAdvanced){
     if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
     composePacket(false,input);sampleController();
     hostClock.pause(monotonicNow50DU());wasPaused=true;
     break;
    }
    // Observe existing post-world subscribers separately from physics and mixing.
    phase_timing50GK::Scope postWorldTiming(phaseTiming,phase_timing50GK::Phase::PostWorldService);
    // Source player speech runs only on the human's accepted native step.
    if(mode1Enabled&&mode1->phase==race_mode1_host50ID::Phase::Flush){
     const auto flushed=mode1->flush(applicationClock,sessionRandom.seed,s.noiseOwner().sharedRandomState,
         speechCounts,speechState,mode1Effects);
     if(flushed.status!=race_mode1_host50ID::Status::Complete)
      std::cerr<<"MODE1 FIFO BLOCKED: source_pc=0x"<<std::hex<<flushed.sourcePC<<std::dec
          <<" ticket="<<flushed.subscriber.ticket<<" lap_helper="<<unsigned(flushed.subscriber.lapHelper)
          <<" damage_helper="<<unsigned(flushed.subscriber.damageHelper)<<"; no acknowledgement.\n";
     require50DU(flushed.status==race_mode1_host50ID::Status::Complete,
         "mode1 real downstream request unresolved; refusing later world events");
    }
    if(dynamicContactEnabled){
     // Declared accepted-world flush: all actual frames and the source manager
     // rank pass completed. Read actors afresh, not enqueue-time snapshots.
     if(dynamicEvents.endFlushPending50LA())
      dynamicBoundary(0x1a4c9cu,"actual End owns pending FIFO; no ordinary capture/flush replay");
     captureDynamicBeforeFrame(0);
     const auto began=beginRaceEventFlush50ID(dynamicEvents,originalAiSession.manager.flags58,0u);
     if(began!=RaceEventStatus50ID::FlushSuppressed){
      if(began!=RaceEventStatus50ID::Ready&&began!=RaceEventStatus50ID::Empty)
       dynamicBoundary(0x1a4c9cu,"original shared event flush admission");
      while(const auto* entry=frontRaceEvent50ID(dynamicEvents)){
       const auto ticket=entry->ticket;
       if(entry->event.type4==1u){
        using Status=DynamicContactEffectStatus50IW;
        if(beginDynamicContactEvent1Subscriber50IW(dynamicEvents,dynamicEventCursor).status!=Status::Continue)
         dynamicBoundary(0x1a5940u,"contact subscriber entry pending");
        for(;;){
         const auto step=advanceDynamicContactEvent1Subscriber50IW(dynamicEventCursor,
             dynamicActorResolver,dynamicEventBindings,s.noiseOwner().sharedRandomState);
         if(step.status==Status::Complete)break;
         if(step.status!=Status::NeedHelper)dynamicBoundary(step.sourcePC,"contact event source continuation");
         const auto& request=step.request;
         if(request.kind==DynamicContactEventCall50IW::PodTaunt2C1C00||
            request.kind==DynamicContactEventCall50IW::PodCurse2C1A90){
          DamageSpeechInput50HG live{};const PodSpeechCounts50HG* counts=nullptr;
          PodSpeechState50HG* state=nullptr;
          if(!resolveDynamicSpeech(request.actor,live,counts,state))
           dynamicBoundary(request.callPC,"actual callback receiver D48 ownership unavailable");
          auto selected=beginDynamicContactPodSpeech50IW(request,request.actor,*counts,
              live.position,live.listeners,s.noiseOwner().sharedRandomState,*state,dynamicPodEffect);
          if(selected.status==Status::NeedHelper){
           if(!applyDynamicSpeech(request.actor,dynamicPodEffect.command))
            dynamicBoundary(selected.sourcePC,"actual contact speech audio helper pending");
           if(!acceptDynamicContactPodAudioReturn50IW(dynamicPodEffect,request.sequence,request.actor,*state))
            dynamicBoundary(selected.sourcePC,"contact speech audio return mismatch");
           ++speechCues;
          }else if(selected.status!=Status::Complete)
           dynamicBoundary(selected.sourcePC,"actual contact speech selector pending");
         }else if(request.kind==DynamicContactEventCall50IW::Announcer2C2C20){
          // Original event1 selected this actual actor immediately before
          //2C2C20. The shared Dax owner retains its real priority1 gate, bank
          // history and RNG; a rejected/suppressed source call is a return.
          RaceActorSnapshot50ID actual{};
          if(!request.actor||!dynamicActorResolver.read(dynamicActorResolver.context,request.actor,actual)||
             actual.vehicle60!=request.racer60||!sourceAnnouncer->hasNamedBank(actual.vehicle60))
           dynamicBoundary(request.callPC,"actual collision announcer racer60 bank unavailable");
          if(!sourceAnnouncer->request(request.type,request.racer60,request.variant,
              dynamicMusicProfile.mode11258,originalAiSession.manager.flags58))
           dynamicBoundary(sourceAnnouncer->lastResult().sourcePC,
               "original collision announcer continuation pending");
         }else dynamicBoundary(request.callPC,"unknown original contact event helper");
         if(!acceptDynamicContactEventReturn50IW(dynamicEventCursor,request.sequence,request.kind))
          dynamicBoundary(request.callPC,"original contact event return mismatch");
        }
       }else if(entry->event.type4==3u){
        //2C2458 enqueued this SAME human's material collision before its
        // sound start.1A4D30 table3AA270[3]->1A4D94 calls1A52A0,
        // whose complete body is jr ra/nop. Preserve its FIFO position;
        // there is no wall-hit speech, RNG, music or controller helper.
        if(entry->event.word0||entry->event.word8||!entry->event.actor0C||entry->event.actor10)
         dynamicBoundary(0x1a52a0u,"unrepresented material event3 shape");
       }else if(entry->event.type4==5u){
        DamageSpeechInput50HG live{};const PodSpeechCounts50HG* counts=nullptr;
        PodSpeechState50HG* state=nullptr;const auto actor=entry->event.actor0C;
        if(!resolveDynamicSpeech(actor,live,counts,state))
         dynamicBoundary(0x1a52b0u,"actual event5 callback receiver D48 ownership unavailable");
        const auto started=beginDamageEvent5Subscriber50ID(dynamicEvents,live,*counts,
            s.noiseOwner().sharedRandomState,*state,dynamicDamageCursor);
        if(started!=DamageEventStatus50ID::Complete&&started!=DamageEventStatus50ID::NeedEffect)
         dynamicBoundary(0x1a52b0u,"original damage subscriber pending");
        if(!dynamicDamageCursor.complete){
         bool returned=false;
         if(dynamicDamageCursor.pending==DamageEventEffect50ID::AudioCommand){
          returned=applyDynamicSpeech(actor,dynamicDamageCursor.command);
          if(returned)++speechCues;
         }else if(dynamicDamageCursor.pending==DamageEventEffect50ID::Announcer2C2C20){
          const auto& request=dynamicDamageCursor.receipt;
          returned=request.announcerArgumentKnown&&sourceAnnouncer->request(request.announcerType,
              request.announcerRacer,request.announcerVariant,dynamicMusicProfile.mode11258,
              originalAiSession.manager.flags58);
         }
         if(!returned)dynamicBoundary(0x1a52b0u,"actual damage event effect helper pending");
         if(acceptDamageEventEffectReturn50ID(dynamicDamageCursor,ticket,dynamicDamageCursor.pending)!=
             DamageEventStatus50ID::Complete)dynamicBoundary(0x1a52b0u,"damage event helper return mismatch");
        }
       }else if(entry->event.type4==6u){
        const auto started=beginLapEvent6Subscriber50ID(entry->event,dynamicActorResolver,dynamicLapCursor);
        if(started.status!=LapEventStatus50ID::Complete&&started.status!=LapEventStatus50ID::Continue&&
           started.status!=LapEventStatus50ID::NeedHelper)
         dynamicBoundary(started.sourcePC,"original lap event6 subscriber entry pending");
        for(;;){
         const auto step=advanceLapEvent6Subscriber50ID(dynamicLapCursor,
             static_cast<std::uint8_t>(originalAiSettings1125C),sessionRandom.seed);
         if(step.status==LapEventStatus50ID::Complete)break;
         if(step.status!=LapEventStatus50ID::NeedHelper)
          dynamicBoundary(step.sourcePC,"original lap event6 continuation pending");
         const auto& request=step.request;std::uint32_t actualV0=0;bool returned=false;
         if(request.kind==LapEventCall50ID::Announcer2C2C20)
          returned=sourceAnnouncer->request(request.argumentA1,request.argumentA2,request.argumentA3,
              dynamicMusicProfile.mode11258,originalAiSession.manager.flags58);
         else if(request.kind==LapEventCall50ID::Music1A46E0)
          returned=liveAudio->outputOpen()&&raceMusic->request(request.argumentA1,request.argumentA2,
              dynamicMusicProfile.mode11258).completed();
         else returned=applyHostLapClockOrRandom50ID(request,applicationClock,
             s.noiseOwner().sharedRandomState,actualV0);
         if(!returned)dynamicBoundary(request.callPC,"actual lap event6 helper pending");
         if(!acceptLapEventHelperReturn50ID(dynamicLapCursor,request.sequence,request.kind,actualV0))
          dynamicBoundary(request.callPC,"original lap event6 helper return mismatch");
        }
        //Real AI vehicle58 returns at1A5914 before announcer/music/RNG. This
        //source suppression is consumed through the same real FIFO ticket.
       }else dynamicBoundary(0x1a4d30u,"unjoined actual shared event subscriber");
       if(acceptRaceEventReturn50ID(dynamicEvents,ticket)!=RaceEventStatus50ID::Complete)
        dynamicBoundary(0x1a5c40u,"original FIFO acknowledgement mismatch");
      }
      if(finishRaceEventFlush50ID(dynamicEvents)!=RaceEventStatus50ID::Complete)
       dynamicBoundary(0x1a5c40u,"original FIFO flush remains pending");
     }
    }
    if(audioEnabled&&playerAdvancing&&!mode1Enabled&&!dynamicContactEnabled){
     AudioCommand50HE speechCommand{};std::size_t speechCount=0;
     const auto dispatched=dispatchDamageSpeechEvent50HG(speechInput(),speechCounts,pendingSpeech,
         s.noiseOwner().sharedRandomState,speechState,&speechCommand,1,speechCount);
     require50DU(dispatched.status==PodSpeechStatus50HG::Complete,"native damage speech dispatch rejected");
     if(dispatched.announcerRequested)++announcerRequestsUnjoined;
     if(speechCount){
      require50DU(liveAudio->applySourceCommand(speechCommand).okay(),"genuine speech playback request rejected");
      ++speechCues;
     }
    }
    if(playerFinishDriver&&!humanCourseEnabled){
     // Normal source finish countdown is four seconds. Audio/event waiting and
     // the original menu transition remain unjoined. At zero the native client
     // offers a live Results overlay without stopping this scene or its audio.
     humanResultsCountdown=std::max(0.f,humanResultsCountdown-.016f);
     humanResultsReady=humanResultsCountdown<=0.f;
    }
    updateOpponentAudio(); // After the complete native world commit, before the one shared mix.
    postWorldTiming.finish();
    // All racer/audio subscribers feed one mixer. Continue the finish clip and
    // any remaining field while the human is finished; never mix per opponent.
    if(audioEnabled&&liveAudio->outputOpen()){
     phase_timing50GK::Scope audioTiming(phaseTiming,phase_timing50GK::Phase::AudioCommit);
     const auto beforeMix=liveAudio->mixedFrames();
     const bool mixed=liveAudio->committedStep(audioDiagnostic);
     advanceSourceAudioWorld(beforeMix,false); //Actual completed new world/camera.
     require50DU(mixed,"native mixed audio world step failed");
    }
    if(sourceCameraSession){
     // Real complete world, Camera and subscriber/audio return; still inside
     // this accepted mode2/23C Root. Do not put this in TaRootReturn: that guard
     // also closes normal EARLY departures with no completed native frame.
     rr::game::OriginalResults104Diagnostic50LA resultsUpdate;
     const bool updated=originalMainApplication.updateNativeResultsAfterRace50LA(
         *sourceCameraSession,resultsUpdate);
     require50DU(updated,resultsUpdate.operation?resultsUpdate.operation:
         "native complete-root Results update rejected");
     require50DU(finishTaRootWithResults(),"native complete-root accepted Results capture rejected");
    }
    if(humanCourseEnabled&&sourceCameraSession->endingFadeQueued())break;
   }
   if(recoveryPaused){discardTrackBaseline();captureCommitted();if(checkpointRequested)break;continue;}

   if(captureFirstWrongWay&&!capturedFirstWrongWay&&(lapContinuity.prefix.flags10CC&2u))capturePending=true;
   // The neutral racer setup tick precedes the camera Session and already
   // increments completedFrames. Only that Session's first real scene/camera
   // child publication admits original-camera rendering, including a zero-step
   // host poll immediately after Course entry or resume. Keep the pending
   // render request until the normal fixed-step owner publishes the view.
   if((sourceCameraSession?sourceCameraSession->hasView():completedFrames!=0)&&
       (!originalManualResetGraphicsDue||gcGraphicsSequence[0]>=originalManualResetGraphicsDue)&&
       !(sourceCameraSession&&(sourceCameraSession->blockingVideoPending()||sourceCameraSession->asyncVideoPending()))&&
       !(humanCourseEnabled&&sourceCameraSession->endingVideoCovered()&&!humanResultsReady&&
         !native_common_results50PD::state().replayActive)&&
       (plan.steps||input.resized||!presenter.hasPublished||capturePending||forceSceneRender||
        (sourceCameraSession&&renderedCameraPublication!=sourceCameraSession->publicationCount()))){
    frameHitch50DU.render=FrameHitch50DU::Clock::now();
    stall_watchdog50DU::mark("render_frame");
    phase_timing50GK::Scope renderTiming(phaseTiming,phase_timing50GK::Phase::RenderPrepareSubmit);
    if(sourcePauseParentAttached){
     original_pause_observation50LA::Owner::Diagnostic d;
     OriginalPauseLayout50LA::Diagnostic layout;OriginalPauseNativeResources50LA::Diagnostic draw;
     require50DU(sourcePauseObservation.prepareNativePresentation(*sourcePause.drawing(),d,layout,draw),
         "original disabled Pause presentation clock rejected");
    }

    {
     render_diagnostics50GQ::Scope censusTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::Census);
     renderDiagnostics.observeScene(s,completedFrames);
    }
    render_diagnostics50GQ::Scope bankTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DrawBank);
    const auto bank=static_cast<std::uint8_t>(presenter.hasPublished?(presenter.publishedCpuBank^1u):0u);
    VehicleDrawBatchInput50CO drawInput{};
    require50DU(playerDrawAssets.batchInput(bank,ModelPalettePhase49S::selectedRenderPreparation,
        ModelPaletteSceneState49S::completeCurrentScene,drawInput),"native draw bank preparation failed");
    {
     // GPU vertex path for the player pod (archetype 0), as admitted by the
     // renderer. One skin record per CPU bank: a retained published batch
     // keeps reading the other bank. Local bounds are rebuilt every frame
     // (one cheap pass), so no record outlives a model change. A destroyed
     // player keeps the CPU path, whose collapse below hides the model.
     static std::array<std::array<rr::game::VehicleDrawSkin50CM,rr::game::maxVehicleDrawComponents50CM>,2> playerSkins50LA;
     // 227370/227A40 hide the lost side's engines (28C6E0 on 1BC+1C4 or
     // 1B8+1C0 = components 1+3 or 2+4); their geometry is collapsed below,
     // which needs CPU vertices, so those components leave the GPU skin path.
     const std::uint32_t limpState=s.bodies.main.vehicleState40==1u||s.bodies.main.vehicleState40==2u?s.bodies.main.vehicleState40:0u;
     const std::uint32_t limpHidden=limpState==1u?((1u<<1)|(1u<<3)):limpState==2u?((1u<<2)|(1u<<4)):0u;
     const std::uint32_t playerSkinMask=rr::game::nativeRacerDestroyed50PD(0)?0u:(renderer.racerGpuSkinMask50LA(0)&~limpHidden);
     for(std::uint32_t c=0;c<drawInput.componentCount&&c<rr::game::maxVehicleDrawComponents50CM;++c){
      auto& skin=playerSkins50LA[bank&1u][c];
      skin.localBoundsReady=false;
      drawInput.staging[c].skin=(playerSkinMask>>c)&1u?&skin:nullptr;
     }
     // Diagnostic-only GPU/CPU vertex comparison every N rendered frames.
     static const std::uint64_t vertexCheckInterval=[]{const char* v=std::getenv("RR_GPU_POD_VERTEX_CHECK");
      const long long n=v?std::atoll(v):0;return n>0?static_cast<std::uint64_t>(n):0ull;}();
     rr::game::vehicleDrawSkinReference50CM=vertexCheckInterval&&submittedDrawFrames%vertexCheckInterval==0;
    }
    bankTiming.finish();
    render_diagnostics50GQ::Scope projectionTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::VehicleProjection);
    VehicleDrawBatch50CO next{};VehicleDrawBatchDiagnostic50CO drawDiagnostic{};
    {
    gcLighting::DrawLease lightLease;
    if(nativeSelectedLights)beginNativeLightDraw(0,lightLease);
    const bool originalBodyColors=bool(nativeModelDomain)&&nativeSelectedLights;
    VehicleDrawBatchSourceColors50LA sourceColors;
    if(originalBodyColors){
     // Source vertex colors require the exact selected rows before the same
     // current per-node palette is projected. This publishes no frame/RNG.
     if(!gcLightOwner.publishNativeDraw(lightLease,nativeLightingPolicy,gcDrawLights[0],gcLightDiagnostic))
      nativeLightFailure("source body-color light publication rejected",0,lightLease);
     require50DU(racerVisuals[0]->canonicalEngineSourceColors50LA(gcDrawLights[0],0,sourceColors),
         "actual player canonical body-color inputs rejected");
     // Borrow the existing per-model240740 owner only for this projection.
     // Completed geometry retains copied output, never these mutable slots.
     sourceColors.component[1].modelPackets50LA=nativeLeftModels[0]->modelPackets();
     sourceColors.component[2].modelPackets50LA=nativeRightModels[0]->modelPackets();
     require50DU(sourceColors.component[1].modelPackets50LA&&sourceColors.component[2].modelPackets50LA,
         "actual player model matrix packet owners unavailable");
     if(racerRigs[0]->modelCount==5){
      sourceColors.component[3].modelPackets50LA=nativeBottomLeftModels[0]->modelPackets();
      sourceColors.component[4].modelPackets50LA=nativeBottomRightModels[0]->modelPackets();
      require50DU(sourceColors.component[3].modelPackets50LA&&sourceColors.component[4].modelPackets50LA,
          "actual player lower model matrix packet owners unavailable");
     }
    }
    if(!buildVehicleDrawBatch50CO(s,x.bindings,drawInput,next,drawWorkspace,drawDiagnostic,
        originalBodyColors?&sourceColors:nullptr)){
     std::cerr<<"draw preparation component "<<drawDiagnostic.component<<" error "
              <<static_cast<unsigned>(drawDiagnostic.detail.error)<<'\n';
     throw std::runtime_error("actual vehicle mesh preparation failed");
    }
    if(nativeSelectedLights&&!originalBodyColors&&!gcLightOwner.publishNativeDraw(lightLease,
        nativeLightingPolicy,gcDrawLights[0],gcLightDiagnostic))
     nativeLightFailure("publishNativeDraw rejected",0,lightLease);
    } //229EF4 semantic publication ends; only completed copied packet survives.
    const bool playerDestroyed=rr::game::nativeRacerDestroyed50PD(0);
    const std::uint32_t limpDrawState=!playerDestroyed&&(s.bodies.main.vehicleState40==1u||s.bodies.main.vehicleState40==2u)?
        s.bodies.main.vehicleState40:0u;
    std::uint32_t limpHiddenDrawn=0;
    if(limpDrawState){
     // 2274E0/2274F4 (227BB0/227BC4): only the lost side's engine models hide.
     const std::uint32_t hiddenMask=limpDrawState==1u?((1u<<1)|(1u<<3)):((1u<<2)|(1u<<4));
     for(std::uint32_t c=0;c<next.componentCount&&c<32u;++c){
      if(!((hiddenMask>>c)&1u))continue;
      auto& part=next.component[c];
      require50DU(!part.skin,"limp hidden engine still on the GPU skin path");
      if(!part.vertices||!part.vertexCount)continue;
      auto* vertices=const_cast<rr::assets::Vertex48B*>(part.vertices);
      for(std::uint32_t v=1;v<part.vertexCount;++v)for(unsigned k=0;k<3;++k)vertices[v].position[k]=vertices[0].position[k];
      limpHiddenDrawn|=1u<<c;
     }
    }
    if(playerDestroyed){
     // Original 28C6E0 hides every model of the destroyed racer. Collapse
     // this frame's projected geometry (loop-owned staging) to one point so it
     // covers no pixels; spans, indices and counts stay the retained mesh.
     for(std::uint32_t c=0;c<next.componentCount;++c){
      auto& part=next.component[c];
      if(!part.vertices||!part.vertexCount)continue;
      auto* vertices=const_cast<rr::assets::Vertex48B*>(part.vertices);
      for(std::uint32_t v=1;v<part.vertexCount;++v)for(unsigned k=0;k<3;++k)vertices[v].position[k]=vertices[0].position[k];
     }
    }
    projectionTiming.finish();
    if(sourceFogRenderPhase.bound()){
     const auto fogPhase=sourceFogRenderPhase.prepare(sourceCameraSession.get());
     if(fogPhase.status!=sourceCamera::Status::complete)
      std::fprintf(stderr,"camera_fog_phase_rejected status=%u pc=0x%x\n",
          static_cast<unsigned>(fogPhase.status),static_cast<unsigned>(fogPhase.sourcePC));
     require50DU(fogPhase.status==sourceCamera::Status::complete,"actual shared camera fog render phase rejected");
    }
    render_diagnostics50GQ::Scope cameraTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::HostCamera);
    const auto& position=m.components.root.body.position;
    platform::VehicleViewCamera50DG camera{};
    platform::VehicleViewMatrix50DG view{},skyView{};
    Vector3_48C cameraBackward{};
    // Existing native HUD route heading still follows the actual pod; this
    // metadata does not alter the recovered camera projection.
    const auto& pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
    const float headingLength=std::sqrt(pose.v[8]*pose.v[8]+pose.v[10]*pose.v[10]);
    if(std::isfinite(headingLength)&&headingLength>0.0001f){
     viewForwardX=pose.v[8]/headingLength;viewForwardZ=pose.v[10]/headingLength;
    }
    if(isRaTrack){
     ra_cold_aggregate50LA::Diagnostic staticDiagnostic;
     ra_static_lighting50LA::StaticDiffuseDiagnostic diffuseDiagnostic;
     const bool fogPrepared=raWorld&&sourceCameraSession&&raWorld->prepareNativeStaticFogObservations(staticDiagnostic);
     if(!fogPrepared){
      std::cerr<<"RA fog failure="<<(staticDiagnostic.failure?staticDiagnostic.failure:"")
        <<" detail="<<(staticDiagnostic.runtimeQueries.message?staticDiagnostic.runtimeQueries.message:"")
        <<" camera="<<unsigned(staticDiagnostic.runtimeQueries.camera.status)
        <<" pc="<<staticDiagnostic.runtimeQueries.camera.sourcePC<<'\n';
      require50DU(false,"actual RA current camera/fog preparation failed");
     }
     require50DU(raWorld->prepareNativeStaticLighting(staticDiagnostic)&&
         raWorld->prepareNativeStaticDiffuseStream(diffuseDiagnostic),
         "actual RA current static lighting/diffuse preparation failed");
     const auto* plan=raWorld->coldStaticWorld()->surfaceDrawPlan();
     ra_static_fog50LA::CameraObservation sourceFogCamera;
     ra_static_fog50LA::PlacementObservation sourceFogPlacement;
     require50DU(plan&&!plan->empty()&&raWorld->readNativeStaticFogObservation(
         plan->front().placement,sourceFogCamera,sourceFogPlacement),
         "actual RA current fog material observation failed");
     sourceTrackFog=sourceFogCamera.source;
     // Fog preparation rebuilds the same actual camera projection. Consume
     // that publication below, after all source operands have been prepared.
    }
    if(sourceCameraSession){
     platform::OriginalCameraPresentation projected;
     const auto& core=sourceCameraSession->view();
     const auto& viewport=sourceCameraSession->viewport();
     const auto projection=platform::projectOriginalCameraPresentation(core,viewport,
         sourceCameraSession->publicationCount(),info.drawableWidth,info.drawableHeight,projected);
     if(projection.status!=platform::OriginalCameraPresentationStatus::complete){
      const auto matrixWords=[](const std::array<float,16>& matrix){
       std::array<std::uint32_t,16> words{};std::memcpy(words.data(),matrix.data(),sizeof words);return words;
      };
      J diagnostic={{"stage",static_cast<unsigned>(projection.status)},
          {"conversion",static_cast<unsigned>(projection.conversion)},
          {"sky_rebuild_status",static_cast<unsigned>(projection.skyRebuild.status)},
          {"sky_rebuild_source_pc",projection.skyRebuild.sourcePC},
          {"publication",sourceCameraSession->publicationCount()},{"camera_frame",sourceCameraSession->publishedFrame()},
          {"world_ticks",completedFrames},{"planned_steps",plan.steps},{"rounding",std::fegetround()},
          {"session_generation",sourceCameraSession->sessionGeneration()},
          {"viewport_generation",viewport.sessionGeneration},{"scene_key",viewport.sceneKey},
          {"prepared",viewport.prepared},{"matrices_ready",viewport.matricesReady},
          {"drawable",{info.drawableWidth,info.drawableHeight}},
          {"viewport",{viewport.x1E0,viewport.y1E4,viewport.width1E8,viewport.height1EC}},
          {"position",{core.position.x,core.position.y,core.position.z}},
          {"look",{core.sceneAfter.look.x,core.sceneAfter.look.y,core.sceneAfter.look.z}},
          {"up",{core.sceneAfter.up.x,core.sceneAfter.up.y,core.sceneAfter.up.z}},
          {"fov",core.sceneAfter.fovDegrees},{"aspect1F8",core.sceneAfter.aspect1F8},
          {"near",core.sceneAfter.nearPlane},{"far",core.sceneAfter.farPlane},{"roll",core.sceneAfter.rollRadians},
          {"view_u32",matrixWords(core.view)},{"projection_u32",matrixWords(core.projection)},
          {"combined_u32",matrixWords(core.combined)}};
      std::cerr<<"original_camera_projection_rejected "<<diagnostic.dump()<<'\n';
      throw std::runtime_error("original camera exact native projection rejected");
     }
     view=projected.world;skyView=projected.sky;
     for(unsigned axis=0;axis<3;++axis){
      camera.eye[axis]=projected.eye[axis];cameraBackward.v[axis]=projected.backward[axis];
      camera.target[axis]=projected.eye[axis]-projected.backward[axis];
     }
     const auto& scene=sourceCameraSession->view().sceneAfter;
     camera.up[0]=scene.up.x;camera.up[1]=scene.up.y;camera.up[2]=scene.up.z;
     camera.nearPlane=scene.nearPlane;camera.farPlane=scene.farPlane;
     camera.aspectWidthOverHeight=float(info.drawableWidth)/float(info.drawableHeight);
     camera.verticalFovRadians=scene.fovDegrees*0.017453292519943295f;
     // eye/target are metadata for fog/audio/read-only capture diagnostics;
     // world/sky matrices above remain the exact recovered source outputs.
     audioListener={camera.eye[0],camera.eye[2]};audioListenerReady=true;
     if(isGbTrack){const Vector3_48C committed{{camera.eye[0],0.f,camera.eye[2]}};
      require50DU(gbQueryBinding&&gbQueryBinding->commitListeners(&committed,1),"GB actual accepted camera listener commit rejected");}
    }else{
    // Host chase-camera policy follows the actual main scene heading. This
    // changes only presentation; it is not the original camera controller.
    camera.eye[0]=position.v[0]-viewForwardX*52.f;
    camera.eye[1]=position.v[1]+12.f;
    camera.eye[2]=position.v[2]-viewForwardZ*52.f;
    camera.target[0]=position.v[0]+viewForwardX*38.f;
    camera.target[1]=position.v[1]+3.f;
    camera.target[2]=position.v[2]+viewForwardZ*38.f;camera.up[1]=1.f;
    camera.verticalFovRadians=1.0471975512f;
    camera.aspectWidthOverHeight=float(info.drawableWidth)/float(info.drawableHeight);
    camera.nearPlane=1.f;camera.farPlane=5400.f; // Requested80% increase over3000.
    platform::VehicleViewCamera50DG framedCamera{};
    host_chase_camera50fn_fixture::Diagnostic framingDiagnostic{};
    require50DU(host_chase_camera50fn_fixture::framePod(camera,next,cameraFraming,
        framedCamera,framingDiagnostic),"host camera framing rejected prepared bounds");
    if(framingDiagnostic.adjusted)++cameraAdjustedFrames;
    if(framingDiagnostic.limited)++cameraLimitedFrames;
    camera=framedCamera;
    audioListener={camera.eye[0],camera.eye[2]};audioListenerReady=true;
    if(isGbTrack){const Vector3_48C committed{{camera.eye[0],0.f,camera.eye[2]}};
     require50DU(gbQueryBinding&&gbQueryBinding->commitListeners(&committed,1),"GB actual camera listener commit rejected");}
    platform::VehicleViewCameraError50DG cameraError{};
    require50DU(platform::composeVehicleViewCamera50DG(camera,view,cameraError),
                "host chase camera rejected live pose");
    auto skyCamera=camera;
    for(unsigned axis=0;axis<3;++axis){
     skyCamera.target[axis]=camera.target[axis]-camera.eye[axis];skyCamera.eye[axis]=0.f;
    }
    skyCamera.farPlane=100000.f; // Actual sky half-size25600; source Y+1024 is in mesh.
    require50DU(platform::composeVehicleViewCamera50DG(skyCamera,skyView,cameraError),
        "source-centered sky camera rejected");
    cameraBackward={{camera.eye[0]-camera.target[0],
        camera.eye[1]-camera.target[1],camera.eye[2]-camera.target[2]}};
    }
    cameraTiming.finish();
    render_diagnostics50GQ::Scope cablesTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::SourceCables);
    if(!playerDestroyed){
     runNative([&]{return buildPodCables50FD(s,x.bindings,playerCableBindings,cameraBackward,
         cableMesh,cableWorkspace,cableDiagnostic);},"source cable mesh generation");
     // 229D20: the Binder draws only while 48 != 0; 227370/227A40 clear 48.
     if(gameplayBinder.loaded()){
      if(limpDrawState)require50DU(gameplayBinder.hideRacer(0),"limping player Binder hide rejected");
      else require50DU(gameplayBinder.stageRacer(0,s,x.bindings,cameraBackward,gcLightOwner),
         "actual player Binder attachments or authored strip color unavailable");
     }
     if(limpDrawState){
      std::uint32_t cablesDrawn=0;
      for(std::uint32_t side=0;side<cableMesh.cableCount&&side<4u;++side){
       const auto& first=cableMesh.vertices[side*18u];const auto& last=cableMesh.vertices[side*18u+17u];
       if(first.position[0]!=last.position[0]||first.position[1]!=last.position[1]||first.position[2]!=last.position[2])cablesDrawn|=1u<<side;
      }
      static std::uint64_t limpDrawFrames=0;++limpDrawFrames;
      std::cerr<<"limp_draw frame="<<limpDrawFrames<<" state40="<<limpDrawState<<" hidden_components=0x"<<std::hex<<limpHiddenDrawn
          <<" cables_drawn=0x"<<cablesDrawn<<std::dec<<" binder="<<(gameplayBinder.loaded()?"hidden":"absent")<<'\n';
     }
    }else{
     // Destroyed: the cables hang from the hidden models. Keep the admitted
     // 2/4-cable topology, collapsed to one point; hide the Binder likewise.
     for(auto& vertex:cableMesh.vertices)for(unsigned k=0;k<3;++k)vertex.position[k]=cableMesh.vertices[0].position[k];
     if(gameplayBinder.loaded())require50DU(gameplayBinder.hideRacer(0),"destroyed player Binder hide rejected");
    }
    cablesTiming.finish();
    render_diagnostics50GQ::Scope hudTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::HostHud);
    platform::VehicleWindowHostHud50DS hud{};
    hud.heat01=std::clamp(boostState.heat7C,0.f,1.f);
    hud.leftDamage01=std::clamp(s.fields.damage7D4,0.f,1.f);
    hud.rightDamage01=std::clamp(s.fields.damage7D0,0.f,1.f);
    hud.boostActive=boostState.vehicle64==1||boostState.vehicle64==2;
    hud.overheated=boostState.vehicle64==2;
    // Host route cue only: C88 is the current source map target, not a
    // nearest invented waypoint. Camera framing changes translation only.
    hud.wrongWay=(lapContinuity.prefix.flags10CC&2u)!=0;
    hud.lapCountValid=true;hud.completedLaps=std::min(lapContinuity.state.vehicleCC8,
        platform::maxVehicleWindowCompletedLaps50DS);
    if(raceEnabled){
     hud.raceEnabled=true;
     hud.raceCountdown=withRace([&](auto& actualRace){return race_session50HM::countdownNumber(actualRace);});
     hud.raceGo=race.phase==race_session50HM::Phase::Racing&&race.raceSeconds<1.0;
     hud.raceFinished=race.racers[0].finished;hud.raceRetirementCD4=race.racers[0].retirementCD4;
     hud.raceResultsReady=humanResultsReady&&(humanCourseEnabled||allAiEnabled||!isGaTrack);
     if(humanResultsReady&&(humanCourseEnabled||allAiEnabled||!isGaTrack)){
      require50DU(race.resultsClosed&&(race.racers[0].finished||
          (humanCourseEnabled&&race.racers[0].retirementCD4)),
          "results overlay requires the genuine selected-racer finish or retirement");
      hud.resultsPanelVisible=true;
      // A native DNF has a real source rank and retirement marker, but no
      // finish-list ordinal or finish time. Leave those HUD fields empty.
      if(race.racers[0].finished){
       hud.resultsFinishPlace=race.racers[0].finishPlace;
       hud.resultsFinishSeconds=race.racers[0].finishSeconds;
      }
      hud.resultsSelectedAction=resultsUi.selectedAction();
      hud.resultsConfirming=resultsUi.confirming();hud.resultsConfirmAccept=resultsUi.confirmAccept();
     }
     hud.raceLap=withRace([&](auto& actualRace){return race_session50HM::displayedLap(actualRace,0);});
     hud.raceLapTarget=race.config.lapTarget;hud.racePlace=race.racers[0].place;
     hud.raceRacerCount=race.racerCount;
     if((mode1Enabled||nativeOriginalFleet)&&lapContinuity.timingInitialized){
      // Read the committed native fields, never HOST raceSeconds/finishSeconds.
      // The one-human manager closes on finish or retirement and leaves CB8
      // at that boundary. DNF elapsed time is never presented as a finish.
      hud.raceClocksValid=true;
      hud.raceElapsedSeconds=lapContinuity.timing.timeCB8;
      hud.currentLapSeconds=lapContinuity.timing.timeCB0;
      // Render only source-completed entries after the accepted world/timing
      // commit. CB0 has already restarted the next lap, including at finish.
      // The optional HOST list has three fixed rows. Longer genuine races
      // retain their source TimeStats/clocks and full native CC0 storage;
      // omit this list rather than truncate or invent a larger presentation.
      hud.completedLapTimesValid=race.config.lapTarget<=platform::maxVehicleWindowLapTimes50DS;
      if(hud.completedLapTimesValid){
       hud.completedLapTimeCount=std::min(race.racers[0].observed.completedLaps,race.config.lapTarget);
       for(std::uint32_t lap=0;lap<hud.completedLapTimeCount;++lap)
        hud.completedLapSeconds[lap]=lapContinuity.timing.lapTimesCC0[lap];
       hud.bestLapSeconds=lapContinuity.timing.bestLapCB4;
      }
     }
     require50DU(race.racerCount==raceRacerCount&&race.racerCount<=racerHudNames.size(),"standings require the actual complete live race field");
     hud.standingsCount=race.racerCount;hud.standingsPlayerIndex=0;
     hud.standingsSourceRanks=nativeOriginalFleet;
     for(std::uint32_t place=0;place<race.racerCount;++place){
      const auto racer=race.ordered[place];
      require50DU(racer<race.racerCount,"committed standings racer index rejected");
      const auto& source=race.racers[racer];auto& row=hud.standings[place];
      require50DU(source.observedValid&&(nativeOriginalFleet||source.place==place+1),
          "standings require committed racer observations");
      std::memcpy(row.displayName,racerHudNames[racer].data(),sizeof(row.displayName));
      row.racerIndex=racer;row.place=source.place;
      row.completedLaps=source.observed.completedLaps;row.finished=source.finished;
      row.retirementCD4=source.retirementCD4;
     }
     if(hud.standingsSourceRanks){
      // Source places (1B0814) rank a retired racer behind every active one;
      // the host distance order does not know retirement. List the rows in
      // source place order, which the renderer requires to be ascending.
      bool ascending=true;
      for(std::uint32_t i=1;i<hud.standingsCount;++i)if(hud.standings[i-1].place>hud.standings[i].place)ascending=false;
      if(!ascending){
       static unsigned reorderLogs50PD=0;
       if(reorderLogs50PD<3){++reorderLogs50PD;std::cerr<<"standings_source_order_applied places=";
        for(std::uint32_t i=0;i<hud.standingsCount;++i)std::cerr<<hud.standings[i].racerIndex<<':'<<hud.standings[i].place
            <<(hud.standings[i].retirementCD4?"R":"")<<(hud.standings[i].finished?"F":"")<<' ';
        std::cerr<<'\n';}
       std::stable_sort(hud.standings,hud.standings+hud.standingsCount,
           [](const auto& a,const auto& b){return a.place<b.place;});
      }
     }
    }
    const auto routeTarget=navigationProgress.reset.vehicleC88;
    if(routeTarget<selectedMapCount50MB()){
     const auto& nextGate=selectedMapNode50MB(routeTarget);
     const double dx=double(nextGate.position[0])-position.v[0];
     const double dz=double(nextGate.position[2])-position.v[2];
     const double right=-double(viewForwardZ)*dx+double(viewForwardX)*dz;
     const double ahead=double(viewForwardX)*dx+double(viewForwardZ)*dz;
     const double length=std::sqrt(right*right+ahead*ahead);
     if(std::isfinite(length)&&length>0.0001){
      hud.routeValid=true;hud.routeX=static_cast<float>(right/length);
      testRouteX50DU=hud.routeX; // Unattended steering test only.
      testPositionX50DU=static_cast<float>(position.v[0]);testPositionZ50DU=static_cast<float>(position.v[2]);
      hud.routeY=static_cast<float>(ahead/length);
     }
    }
    // Original viewport clear consumes the authenticated track Fog.ColorRGB.
    // Unauthored sky tiles remain absent; do not invent lower sky geometry.
    const float* clearColor=isScTrack?scZeroSky.clearRgba():isRaTrack?raSky->clearRgba():isMcTrack?mcSkyOwner->clearRgba():skyOwner->clearRgba();
    require50DU(clearColor,"authored track clear color unavailable");
    hudTiming.finish();
    EngineFlareOutput50GV flameOutput{};EngineFlareDiagnostic50GV flameDiagnostic{};
    const bool flameAccepted=advanceEngineFlare50GV(s,baseTail,coldFlame,
        static_cast<std::uint32_t>(submittedDrawFrames%0xffffffffull),flameOutput,flameDiagnostic);
    if(!flameAccepted)std::cerr<<"engine_flame_failure error="<<static_cast<unsigned>(flameDiagnostic.error)
        <<" side="<<flameDiagnostic.side<<" source_pc=0x"<<std::hex<<flameDiagnostic.sourceStage<<std::dec
        <<" rounding="<<std::fegetround()<<" mxcsr=0x"<<std::hex<<_mm_getcsr()<<std::dec
        <<" thrust0="<<s.bodies.main.components.engine.localThrust[0].v[0]<<','<<s.bodies.main.components.engine.localThrust[0].v[1]
        <<','<<s.bodies.main.components.engine.localThrust[0].v[2]
        <<" thrust1="<<s.bodies.main.components.engine.localThrust[1].v[0]<<','<<s.bodies.main.components.engine.localThrust[1].v[1]
        <<','<<s.bodies.main.components.engine.localThrust[1].v[2]
        <<" steer="<<s.bodies.main.controls.player.raw.values[0]
        <<" speed34="<<s.bodies.main.cachedSpeed34<<'\n';
    require50DU(flameAccepted,"actual engine flame callback rejected");
    for(auto& uv:playerFlameUvs){
     const unsigned model=static_cast<unsigned>(uv.component)-1u;
     require50DU(model<flameOutput.modelCount,"source flame per-model instance unavailable");
     float sourceUv[6]{};
     require50DU(exportEngineFlareUv50LA(flameOutput,model,sourceUv),
         "actual origin engine flame payload rejected");
     require50DU(sourceUv[1]==0.f&&sourceUv[3]==0.f,
         "source flame affine cross terms unsupported");
     uv.uScale=sourceUv[0];uv.uOffset=sourceUv[2];
     uv.vScale=sourceUv[4];uv.vOffset=sourceUv[5];
    }
    platform::VehicleWindowFrame50DS renderFrame{info.drawableWidth,info.drawableHeight,
        view.worldViewProjection,clearColor,&cableMesh,
        humanOriginalHud?(hud.resultsPanelVisible?&hud:nullptr):
        sourceCameraSession&&!sourceCameraSession->nativeHudVisible()?nullptr:&hud};
    renderFrame.playerHiddenDestroyed50PD=playerDestroyed;
    static std::array<platform::VehicleWindowDebrisInstance50DS,platform::maxVehicleWindowDebrisInstances50DS> debrisInstances50PD{};
    static std::array<rr::assets::Vertex48B,platform::maxVehicleWindowEffectQuads50DS*4u> effectVertices50PD{};
    if(sourceCameraSession&&native_wreck_effects50PD::state().cold&&!native_wreck_effects50PD::state().particles.empty()){
     // Billboards face the presented camera (its eye and backward axis), not
     // the pose target, which death-camera mode 0 leaves unwritten.
     const auto sprites=native_wreck_effects50PD::camera({camera.eye[0],camera.eye[1],camera.eye[2]},
         {camera.eye[0]-cameraBackward.v[0],camera.eye[1]-cameraBackward.v[1],camera.eye[2]-cameraBackward.v[2]},
         {camera.up[0],camera.up[1],camera.up[2]});
     std::uint32_t blended=0;
     renderFrame.effectQuads=native_wreck_effects50PD::build(sprites,effectVertices50PD.data(),
         platform::maxVehicleWindowEffectQuads50DS,blended);
     renderFrame.effectBlendedQuads=blended;
     renderFrame.effectVertices=renderFrame.effectQuads?effectVertices50PD.data():nullptr;
    }
    if(playerDestroyed&&native_wreck_debris50PD::cameraViewBound(0)){
     // How squarely the presented camera looks at the player's debris.
     static std::uint64_t debrisViewFrames=0;
     if(++debrisViewFrames%125==1){
      const auto& v=native_wreck_debris50PD::cameraView();float c[3]{};
      for(std::size_t k=0;k<v.count;++k)for(unsigned a=0;a<3;++a)c[a]+=v.nodes[k].localC0.v[12+a]/float(v.count);
      const float dx=c[0]-camera.eye[0],dy=c[1]-camera.eye[1],dz=c[2]-camera.eye[2];
      const float distance=std::sqrt(dx*dx+dy*dy+dz*dz);
      // Two independent checks: the presented camera's forward axis
      // (-backward) and the death camera's own look output, against the
      // direction to the debris (1 = dead centre, <0 = behind).
      const float bx=-cameraBackward.v[0],by=-cameraBackward.v[1],bz=-cameraBackward.v[2];
      const float bl=std::sqrt(bx*bx+by*by+bz*bz);
      const auto& pose=sourceCameraSession->pose();
      const float lx=pose.look.x,ly=pose.look.y,lz=pose.look.z,ll=std::sqrt(lx*lx+ly*ly+lz*lz);
      const float forwardCos=distance>0.f&&bl>0.f?(dx*bx+dy*by+dz*bz)/(distance*bl):0.f;
      const float lookCos=distance>0.f&&ll>0.f?(dx*lx+dy*ly+dz*lz)/(distance*ll):0.f;
      std::cerr<<"death_camera_view frame="<<debrisViewFrames
          <<" forward_cos="<<forwardCos<<" look_cos="<<lookCos<<" distance="<<distance
          <<" eye="<<camera.eye[0]<<','<<camera.eye[1]<<','<<camera.eye[2]<<" debris="<<c[0]<<','<<c[1]<<','<<c[2]
          <<" pose_mode="<<pose.mode<<'\n';
     }
    }
    {const auto& g=native_wreck_debris50PD::gpu();
     if(g.prepared){renderFrame.debrisCount=native_wreck_debris50PD::instances(debrisInstances50PD,g.firstPiece,g.pieceCount);
      renderFrame.debris=renderFrame.debrisCount?debrisInstances50PD.data():nullptr;}}
    if(renderRequest.track.rcCrystal){
     // The actual retained RC crystal source consumes this same Session view
     // even when no racer material selects the specular-pair capability.
     require50DU(sourceCameraSession&&sourceCameraSession->hasView(),
         "RC crystal rendering requires the actual published camera");
     renderFrame.sourceBodyCamera50LA=&sourceCameraSession->view();
     renderFrame.sourceBodyCameraOwner50LA=reinterpret_cast<std::uintptr_t>(sourceCameraSession.get());
     renderFrame.sourceBodyCameraPublication50LA=sourceCameraSession->publicationCount();
    }
    if(std::any_of(racerVisuals.begin(),racerVisuals.end(),[](const auto& visual){return visual&&visual->hasCanonicalBodyColors50LA();})){
     // One genuine camera publication is shared by all paired bodies, even
     // stopped racers whose geometry/light packets remain retained. This is
     // view-dependent model preparation below the existing gameplay pause
     // gates; it advances no geometry, material clock, AI or RNG.
     require50DU(sourceCameraSession&&sourceCameraSession->hasView(),
         "canonical specular rendering requires the actual published camera");
     renderFrame.sourceBodyCamera50LA=&sourceCameraSession->view();
     renderFrame.sourceBodyCameraOwner50LA=reinterpret_cast<std::uintptr_t>(sourceCameraSession.get());
     renderFrame.sourceBodyCameraPublication50LA=sourceCameraSession->publicationCount();
     renderFrame.sourceBodyFog50LA=&sourceTrackFog;
     renderFrame.sourceDiffuseCamera50LA=sourceCameraSession->racerDiffuseCamera();
     renderFrame.sourceBodyGlobals50LA=sourceModelRenderGlobals.get();
     if(racerVisuals[0]&&racerVisuals[0]->hasCanonicalBodyColors50LA()){
     renderFrame.sourceBodyModelPackets50LA={{nativeLeftModels[0]->modelPackets(),
         nativeRightModels[0]->modelPackets()}};
     if(racerRigs[0]->modelCount==5){
      renderFrame.sourceBodyModelPackets50LA[2]=nativeBottomLeftModels[0]->modelPackets();
      renderFrame.sourceBodyModelPackets50LA[3]=nativeBottomRightModels[0]->modelPackets();
     }
     }
    }
    // Results replay: race1B0 bit4 and the 1AD230 HUD stop skip every race
    // HUD display (18BBB0 and its siblings test bit4 / uiFlags before drawing).
    const bool replayHud50PD=native_common_results50PD::state().replayActive;
    if(sourceRaceHudRendering&&sourceCameraSession&&sourceCameraSession->nativeHudVisible()&&!replayHud50PD)
     renderFrame.originalRaceHud=&sourceRaceHud;
    if(sourceSpeedBoostRendering&&sourceCameraSession&&!replayHud50PD){
     OriginalSpeedBoostNativeFrame50LA speedFrame;
     original_speed_boost_adapter50LA::Owner::Diagnostic adapterDiagnostic;
     OriginalSpeedBoostDiagnostic50LA speedDiagnostic;
     require50DU(sourceSpeedBoostAdapter.capture(speedFrame,adapterDiagnostic),
         "actual Speed/Boost published racer snapshot rejected");
     const bool prepared=sourceSpeedBoost.owner->prepareNativeDisplay(speedFrame,speedDiagnostic);
     if(!prepared)std::cerr<<"original_speed_boost_display_rejected source_pc="<<speedDiagnostic.sourcePC
         <<" material_pc="<<speedDiagnostic.material.sourcePC<<'\n';
     require50DU(prepared,"original Speed/Boost display preparation rejected");
     renderFrame.originalSpeedBoost=sourceSpeedBoost.owner.get();
    }
    if(sourceTimeStatsRendering&&sourceCameraSession&&!replayHud50PD&&originalAiSession.managerBound&&
       originalAiSession.rankBound&&originalAiSession.retired40.bound){
     OriginalTimeStatsNativeFrame50LA timeFrame;
     original_time_stats_adapter50LA::Owner::Diagnostic adapterDiagnostic;
     OriginalTimeStatsDiagnostic50LA timeDiagnostic;
     require50DU(sourceTimeStatsAdapter.capture(timeFrame,adapterDiagnostic),
         "actual TimeStats published timing snapshot rejected");
     require50DU(sourceTimeStats.owner->prepareNativeDisplay(timeFrame,timeDiagnostic),
         "original TimeStats display preparation rejected");
     renderFrame.originalTimeStats=sourceTimeStats.owner.get();
    }
    if(sourceDamageRendering&&sourceCameraSession&&!replayHud50PD){
     OriginalDamageNativeFrame50LA damageFrame;
     OriginalDamageObservation50LA::Diagnostic observation;
     const bool captured=sourceDamageObservation.capture(damageFrame,observation);
     if(!captured)std::cerr<<"original_damage_observation_rejected status="
         <<static_cast<unsigned>(observation.status)<<" source_pc="<<observation.sourcePC
         <<" slot="<<observation.slot<<'\n';
     require50DU(captured,"original Damage observation rejected");
     const bool prepared=sourceDamage.owner->prepareNativeDisplay(damageFrame,sourceDamage.object);
     if(!prepared)std::cerr<<"original_damage_display_rejected source_pc="<<sourceDamage.object.sourcePC<<'\n';
     require50DU(prepared,"original Damage display preparation rejected");
     renderFrame.originalDamage=sourceDamage.owner.get();
    }
    if(sourcePositionStatsRendering&&sourceCameraSession&&!replayHud50PD&&originalAiSession.managerBound&&
       originalAiSession.rankBound&&originalAiSession.retired40.bound){
     if(isRcTrack)requireMbRuntimeNavigation50MB();
     const bool prepared=sourcePositionStatsHud->prepareNativeDisplay(originalMainApplication,
         *sourceCameraSession,originalAiSession,sourcePositionStatsResources.diagnostic);
     if(!prepared)std::cerr<<"original_position_stats_display_rejected source_pc="
         <<sourcePositionStatsResources.diagnostic.sourcePC<<" material_pc="
         <<sourcePositionStatsResources.diagnostic.material.sourcePC<<'\n';
     require50DU(prepared,"original PositionStats display preparation rejected");
     renderFrame.originalPositionStats=sourcePositionStatsHud.get();
     // Original HUD owners provide this admitted race view; ALL8 RA also
     // binds complete health/proximity through its native observation owner.
     // Do not cover them with the host standings/navigation/status panels.
     if(renderFrame.originalTimeStats&&renderFrame.originalSpeedBoost&&
        !(humanOriginalHud&&hud.resultsPanelVisible))renderFrame.hostHud=nullptr;
    }
    if(nativeGcWater){
     require50DU(nativeGcWater->active()&&sourceCameraSession,
         "GC particle render requires the actual active gameplay camera");
     const auto result=gcParticleRender.build(sourceCameraSession->view(),sourceCameraSession->viewport(),
         sourceTrackFog,gcLightOwner,gcDrawLights[0]);
     if(result!=OriginalGcParticleRenderStatus50KG::complete)
      std::cerr<<"GC particle render source boundary="<<static_cast<unsigned>(result)<<'\n';
     require50DU(result==OriginalGcParticleRenderStatus50KG::complete,
         "actual GC particle camera/light draw rejected");
     renderFrame.gcParticles=gcParticleRender.draw();
    }
    platform::GameplayBlurFrame50DS sourceBlurFrame{};
    if(sourceCameraSession){
     const auto& viewport=sourceCameraSession->viewport();
     sourceBlurFrame.pending=&sourceBlur;
     sourceBlurFrame.originalWidth=viewport.fullWidth204;sourceBlurFrame.originalHeight=viewport.fullHeight208;
     sourceBlurFrame.logicalView=0;sourceBlurFrame.physicalView=0;
     sourceBlurFrame.cameraRectangle={viewport.x1E0,viewport.y1E4,
         viewport.x1E0+viewport.width1E8,viewport.y1E4+viewport.height1EC};
     renderFrame.gameplayBlur=&sourceBlurFrame;
    }
    if(nativeSelectedLights){
     renderFrame.lightingPolicy=platform::NativeRacerLightingPolicy50DS::nativeGcSelectedLights;
     renderFrame.selectedLights=&gcDrawLights[0];
    }
    if(gcAnimatedWorld){
     require50DU(gcDynamicWindow.bindFrame(renderFrame,*gcAnimatedWorld),"original GC dynamic frame publication failed");
     require50DU(gcAtm.bindFrame(renderFrame),"original GC ATM frame selection rejected");
    }
    if(gaAnimatedWorld)require50DU(gaDynamicWindow.bindFrame(renderFrame,*gaAnimatedWorld),"original GA dynamic frame publication failed");
    if(isTcTrack)require50DU(tcGateRuntime&&tcDynamicWindow.bindFrame(renderFrame,*tcGateRuntime),"TC actual gate dynamic frame publication failed");
    if(isTbTrack)require50DU(tbDoorRuntime&&tbDynamicWindow.bindFrame(renderFrame,*tbDoorRuntime),"TB actual door dynamic frame publication failed");
    // Animated-world visual vertices are drawing work (original 241280 at
    // draw time): project them once per shown frame, after however many
    // catch-up steps ran, from the node worlds those steps left. Keyed on
    // "this course's animated world has a CPU-projected mesh", which today is
    // MC and GB; GC/GA/SA/TB/TC still project inside their step (port
    // stand-in, to retire the same way).
    if(mcAnimationRuntime){mc_animation_runtime50MC::Diagnostic drawDiagnostic;
     require50DU(mcAnimationRuntime->prepareDraw(drawDiagnostic),drawDiagnostic.message.empty()?"MC animated draw projection rejected":drawDiagnostic.message.c_str());}
    if(gbAnimationRuntime){gb_animation_runtime50GB::Diagnostic drawDiagnostic;
     require50DU(gbAnimationRuntime->prepareDraw(drawDiagnostic),drawDiagnostic.message.empty()?"GB animated draw projection rejected":drawDiagnostic.message.c_str());}
    if(isMcTrack)require50DU(mcAnimationRuntime&&mcDynamicWindow.bindFrame(renderFrame,*mcAnimationRuntime),"MC actual14-animation dynamic frame publication rejected");
    if(isGbTrack)require50DU(gbAnimationRuntime&&gbDynamicWindow.bindFrame(renderFrame,*gbAnimationRuntime),"GB actual four-animation dynamic frame publication failed");
    if(gaAtm.loaded())require50DU(gaAtm.bindFrame(renderFrame),"original GA ATM frame selection rejected");
    if(saAnimatedWorld)require50DU(saDynamicWindow.bindFrame(renderFrame,*saAnimatedWorld),"SA actual dynamic frame publication failed");
    if(maSaAtm.loaded())require50DU(maSaAtm.bindFrame(renderFrame),"MA/SA actual ATM frame selection rejected");
    if(mbAtm.loaded())require50DU(mbAtm.bindFrame(renderFrame),"MB actual ATM frame selection rejected");
    if(scAtm.loaded())require50DU(scAtm.bindFrame(renderFrame),"SC actual ATM frame selection rejected");
    if(tbAtm.loaded())require50DU(tbAtm.bindFrame(renderFrame),"TB actual ATM frame selection rejected");
    if(mcAtm.loaded())require50DU(mcAtm.bindFrame(renderFrame),"MC actual ATM frame selection rejected");
    if(gbAtm.loaded())require50DU(gbAtm.bindFrame(renderFrame),"GB actual ATM frame selection rejected");
    if(rcAtm.loaded())require50DU(rcAtm.bindFrame(renderFrame),"RC actual ATM frame selection rejected");
    if(tcAtm.loaded())require50DU(tcAtm.bindFrame(renderFrame),"TC actual ATM frame selection rejected");
    renderFrame.materialUvs=playerFlameUvs.data();
    renderFrame.materialUvCount=static_cast<std::uint32_t>(playerFlameUvs.size());
    // TC has an actual48-tile sky request and retains the matching matrix.
    // Only authenticated zero-sky SC omits both indices and matrix.
    renderFrame.skyWorldViewProjection=isScTrack?nullptr:skyView.worldViewProjection;
    if(isMaSaTrack||isMbTrack||isRaTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack){
     std::uint32_t cameraZone48=0;
     require50DU(sourceCameraSession&&sourceCameraSession->readCameraPrimaryZone48(cameraZone48)&&
         (isScTrack?scZeroSky.select(cameraZone48,renderFrame.skyEnabled):isRaTrack?raWorld->sampleSky(cameraZone48,renderFrame.skyEnabled):
             sourceSkySelection.select(cameraZone48,renderFrame.skyEnabled)),
         "actual camera primary-zone sky selection rejected");
    }
    renderFrame.worldFogCamera=camera.eye;
    platform::VehicleWindowOpponent50DS opponents[maxHostRaceOpponents50HT]{};
    RacePresentation50HZ nextRacePresentation{};
    if(raceEnabled){
     // Observe the complete existing opponent preparation once per frame.
     // This nests in render_prepare_submit, not driver_submit or player
     // vehicle_projection. It changes no racer admission or callback order.
     render_diagnostics50GQ::Scope opponentsTiming(renderDiagnostics,
         render_diagnostics50GQ::Subphase::OpponentPreparation);
     const auto prepareOpponent=[&](std::size_t racer,bool arenaLoaded){
      const auto index=racer-1;opponentFlamePrepared[index]=false;
      // Source result sealing does not remove a racer from the world.
      // Submit every original pod, including its cables/flames/shadow casters.
      if(race.racers[racer].finished)++nextRacePresentation.submittedFinishedOpponents;
      const auto prepareActiveOpponent=[&](){
       VehicleDrawBatchDiagnostic50CO opponentDiagnostic{};
       gcLighting::DrawLease lightLease;
       if(nativeSelectedLights)beginNativeLightDraw(static_cast<std::uint32_t>(racer),lightLease);
       const bool originalBodyColors=bool(nativeModelDomain)&&nativeSelectedLights;
       VehicleDrawBatchSourceColors50LA sourceColors;
       // Profiling-only nested rows; each scope closes before the next sub-step.
       std::optional<render_diagnostics50GQ::Scope> lightsTiming(std::in_place,renderDiagnostics,
           render_diagnostics50GQ::Subphase::OpponentLights);
       if(originalBodyColors){
        if(!gcLightOwner.publishNativeDraw(lightLease,nativeLightingPolicy,gcDrawLights[racer],gcLightDiagnostic))
         nativeLightFailure("source body-color light publication rejected",static_cast<std::uint32_t>(racer),lightLease);
        require50DU(racerVisuals[racer]->canonicalEngineSourceColors50LA(gcDrawLights[racer],
            static_cast<std::uint32_t>(racer),sourceColors),"actual opponent canonical body-color inputs rejected");
        sourceColors.component[1].modelPackets50LA=nativeLeftModels[racer]->modelPackets();
        sourceColors.component[2].modelPackets50LA=nativeRightModels[racer]->modelPackets();
        require50DU(sourceColors.component[1].modelPackets50LA&&sourceColors.component[2].modelPackets50LA,
            "actual opponent model matrix packet owners unavailable");
        if(racerRigs[racer]->modelCount==5){
         sourceColors.component[3].modelPackets50LA=nativeBottomLeftModels[racer]->modelPackets();
         sourceColors.component[4].modelPackets50LA=nativeBottomRightModels[racer]->modelPackets();
         require50DU(sourceColors.component[3].modelPackets50LA&&sourceColors.component[4].modelPackets50LA,
             "actual opponent lower model matrix packet owners unavailable");
        }
       }
       lightsTiming.reset();
       {render_diagnostics50GQ::Scope drawTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::OpponentDrawBatch);
       // Opponent archetype index is the racer slot (see the descriptor below).
       opponentDraw[index].setGpuSkinMask(renderer.racerGpuSkinMask50LA(static_cast<std::uint32_t>(racer)));
       drawWorkspace.timing=renderDiagnostics.enabled?&renderDiagnostics.opponentDrawTiming:nullptr;
       const bool prepared=opponentDraw[index].prepare(s,x.bindings,drawWorkspace,opponentDiagnostic,
           originalBodyColors?&sourceColors:nullptr);
       drawWorkspace.timing=nullptr;
       require50DU(prepared,"actual opponent component projection failed");}
       lightsTiming.emplace(renderDiagnostics,render_diagnostics50GQ::Subphase::OpponentLights);
       if(nativeSelectedLights&&!originalBodyColors&&!gcLightOwner.publishNativeDraw(lightLease,
           nativeLightingPolicy,gcDrawLights[racer],gcLightDiagnostic))
        nativeLightFailure("publishNativeDraw rejected",static_cast<std::uint32_t>(racer),lightLease);
       lightsTiming.reset();
       {render_diagnostics50GQ::Scope cableTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::OpponentCables);
       require50DU(buildPodCables50FD(s,x.bindings,racerCables[racer]->bindings(),cameraBackward,
           opponentDraw[index].cables,cableWorkspace,cableDiagnostic),"actual opponent cable mesh failed");}
       {render_diagnostics50GQ::Scope binderTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::OpponentBinder);
       if(gameplayBinder.loaded())require50DU(gameplayBinder.stageRacer(static_cast<std::uint32_t>(racer),
           s,x.bindings,cameraBackward,gcLightOwner)||(std::cerr<<"opponent_binder_rejected racer="<<racer
               <<" status="<<gameplayBinder.lastFailure<<" state40="<<s.bodies.main.vehicleState40
               <<" phase="<<static_cast<unsigned>(s.phase)<<"\n",false),"actual opponent Binder attachment projection failed");}
       render_diagnostics50GQ::Scope flameTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::OpponentFlame);
       EngineFlareDiagnostic50GV flareDiagnostic{};
       require50DU(advanceEngineFlare50GV(s,baseTail,coldFlame,
           static_cast<std::uint32_t>(submittedDrawFrames%0xffffffffull),opponentFlames[index],flareDiagnostic),
           "actual opponent engine flame callback failed");
       for(auto& uv:opponentUvs[index]){
        const unsigned model=static_cast<unsigned>(uv.component)-1u;
        require50DU(model<opponentFlames[index].modelCount,"opponent flame per-model instance unavailable");
        float source[6]{};
        require50DU(exportEngineFlareUv50LA(opponentFlames[index],model,source),
            "actual opponent origin flame payload rejected");
        require50DU(source[1]==0.f&&source[3]==0.f,"original opponent flame affine cross terms unsupported");
        uv.uScale=source[0];uv.uOffset=source[2];uv.vScale=source[4];uv.vOffset=source[5];
       }
       opponentFlamePrepared[index]=true;
      };
      // A destroyed racer (original 28C6E0 hid its models) keeps its last
      // complete batch for the fixed field but is not prepared or drawn.
      const bool destroyedRacer=rr::game::nativeRacerDestroyed50PD(racer);
      if(racerBank.active(racer)&&!destroyedRacer){
       if(arenaLoaded)prepareActiveOpponent();
       else racerBank.visit(racer,prepareActiveOpponent);
      }
      if(destroyedRacer&&gameplayBinder.loaded())
       require50DU(gameplayBinder.hideRacer(static_cast<std::uint32_t>(racer)),"destroyed opponent Binder hide rejected");
      // A stopped native opponent retains its last complete display
      // batch, never a palette from an abandoned native source-cut state.
      if(!racerBank.active(racer))++nextRacePresentation.retainedStoppedOpponents;
      require50DU(opponentDraw[index].batch(),"opponent has no completed display batch");
      // Keep the original racer archetype and immutable material ownership.
      opponents[nextRacePresentation.submittedOpponents++]={opponentDraw[index].batch(),&opponentDraw[index].cables,
          opponentUvs[index].data(),static_cast<std::uint32_t>(opponentUvs[index].size()),
          static_cast<std::uint32_t>(racer)};
      opponents[nextRacePresentation.submittedOpponents-1u].hiddenDestroyed50PD=destroyedRacer;
      if(racerVisuals[racer]&&racerVisuals[racer]->hasCanonicalBodyColors50LA()){
       opponents[nextRacePresentation.submittedOpponents-1u].sourceBodyModelPackets50LA=
           {{nativeLeftModels[racer]->modelPackets(),nativeRightModels[racer]->modelPackets()}};
       if(racerRigs[racer]->modelCount==5){
        auto& models=opponents[nextRacePresentation.submittedOpponents-1u].sourceBodyModelPackets50LA;
        models[2]=nativeBottomLeftModels[racer]->modelPackets();
        models[3]=nativeBottomRightModels[racer]->modelPackets();
       }
      }
      if(nativeSelectedLights){
       // Stopped racers retain BOTH their last complete geometry and its exact
       // light publication. Never select from their later interrupted frame.
       require50DU(gcDrawLights[racer].ready()&&gcDrawLights[racer].racerSlot()==racer,
           "retained opponent batch has no matching GC light publication");
       auto& submitted=opponents[nextRacePresentation.submittedOpponents-1u];
       submitted.lightingPolicy=platform::NativeRacerLightingPolicy50DS::nativeGcSelectedLights;
       submitted.selectedLights=&gcDrawLights[racer];
      }
     };
     // Preparation outputs belong to their racer or permanent asset owner;
     // no descriptor publication reads the player arena between peers. Keep
     // each light lease inside prepareActiveOpponent, before the next swap.
     // Stopped slots retain the original individual-visit path unchanged.
     std::array<std::size_t,maxHostRaceOpponents50HT> opponentOrder{};
     std::size_t opponentOrderCount=0;
     bool allOpponentsActive=true;
     for(std::size_t racer=1;racer<racerBank.count();++racer){
      opponentOrder[opponentOrderCount++]=racer;
      if(!racerBank.active(racer))allOpponentsActive=false;
     }
     if(allOpponentsActive){
      require50DU(racerBank.visitMany(opponentOrder.data(),opponentOrderCount,
          [&](std::size_t racer){prepareOpponent(racer,true);return true;}),
          "actual opponent preparation sweep rejected");
     }else for(std::size_t racer=1;racer<racerBank.count();++racer)
      prepareOpponent(racer,false);
     require50DU(nextRacePresentation.submittedOpponents==raceOpponentCount,
         "native continuation must submit the complete opponent field");
     nextRacePresentation.standingsRows=hud.standingsCount;
     renderFrame.opponentCount=nextRacePresentation.submittedOpponents;
     // Preserve the renderer's null/zero invariant; this eight-racer session
     // retains all seven opponent descriptors after their results seal.
     renderFrame.opponents=renderFrame.opponentCount?opponents:nullptr;
    }
    rr::game::OriginalApplicationAnimationClockSample50KG sourceApplicationAnimation;
    bool sourceApplicationTimestampPublished=sourceApplicationClock.read(sourceApplicationAnimation);
    if(gameplayBinder.loaded()){
     //24A740/24A758 advances the canonical material controller during render
     // preparation, not229B40's texture-residency refresh. Consume the actual
     // existing application publication; do not create another timer/epoch.
     // A first presentation may precede a world step. Sample the SAME actual
     // application clock in that case; literal zero is not a clock receipt.
     // This samples time only, without advancing world state or consuming RNG.
     if(!sourceApplicationTimestampPublished){
      std::uint32_t sampled=0;
      require50DU(originalMainApplication.sampleAnimationTimestamp(sampled),
          "first Binder render application timestamp unavailable");
      sourceApplicationTimestampPublished=sourceApplicationClock.read(sourceApplicationAnimation);
     }
     require50DU(sourceApplicationTimestampPublished&&gameplayBinder.advanceRaceRender(sourceApplicationAnimation.current230),
         "original Binder render-time controller rejected application publication");
     require50DU(gameplayBinder.bindRaceFrame(renderFrame),"Binder complete eight-racer frame join rejected");
    }
    // Bounded existing render services consume a read of the actual owner.
    // Canonical callbacks must instead re-read the live capability at each
    // original boundary, including29AD48 after a false callback return.
    const auto sourceApplicationTimestamp=sourceApplicationAnimation.current230;
    struct MaShieldRenderService {
     ma_shield_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } maShieldRender{&maShieldUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS maShieldUvFrame;
    if(maShieldUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"MA UV requires the shared application timestamp publication");
     maShieldUvFrame.owner=&maShieldRender;
     maShieldUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<MaShieldRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&maShieldUvFrame;
    }
    struct MbShieldRenderService {
     mb_shield_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } mbShieldRender{&mbShieldUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS mbShieldUvFrame;
    if(mbShieldUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"MB UV requires the shared application timestamp publication");
     mbShieldUvFrame.owner=&mbShieldRender;
     mbShieldUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<MbShieldRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&mbShieldUvFrame;
    }
    struct TbShieldRenderService {
     tb_shield_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } tbShieldRender{&tbShieldUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS tbShieldUvFrame;
    if(tbShieldUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"TB UV requires the shared application timestamp publication");
     tbShieldUvFrame.owner=&tbShieldRender;
     tbShieldUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<TbShieldRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&tbShieldUvFrame;
    }
    struct McShieldRenderService {
     mc_shield_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } mcShieldRender{&mcShieldUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS mcShieldUvFrame;
    if(mcShieldUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"MC UV requires the shared application timestamp publication");
     mcShieldUvFrame.owner=&mcShieldRender;
     mcShieldUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<McShieldRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&mcShieldUvFrame;
    }
    struct TcShieldRenderService {
     tc_shield_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } tcShieldRender{&tcShieldUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS tcShieldUvFrame;
    if(tcShieldUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"TC UV requires the shared application timestamp publication");
     tcShieldUvFrame.owner=&tcShieldRender;
     tcShieldUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<TcShieldRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&tcShieldUvFrame;
    }
    struct ScLavaRenderService {
     sc_lava_uv_fixture::Owner* owner;
     std::uint32_t timestamp;
     bool applicationPaused;
    } scLavaRender{&scLavaUv,sourceApplicationTimestamp,sourceApplicationAnimation.pause11C!=0};
    platform::VehicleWindowTrackUvFrame50DS scLavaUvFrame;
    if(scLavaUv.loaded()){
     require50DU(sourceApplicationTimestampPublished,"SC UV requires the shared application timestamp publication");
     scLavaUvFrame.owner=&scLavaRender;
     scLavaUvFrame.service=[](void* opaque,std::uint32_t span,float& u,float& v) noexcept {
      auto& input=*static_cast<ScLavaRenderService*>(opaque);
      if(!input.owner->service(span,input.timestamp,input.applicationPaused))return false;
      const auto* rows=input.owner->rows(span);
      if(!rows)return false;
      u=rows->rows[2][0];v=rows->rows[2][1];return true;
     };
     renderFrame.trackUvAnimation=&scLavaUvFrame;
    }
    // The existing consumed-intro selector chooses this native route only.
    // Actual same Session/Application/Race/Display/Frame checks issue the
    // scoped batch; ALL8, noncareer, other tracks and unissued paths stay as-is.
    // Frame was constructed earlier and remains unmoved through destruction.
    std::unique_ptr<original_main_entry50KG::NativeF8BarPrefix50LA> nativeF8Frame;
    std::unique_ptr<original_main_entry50KG::NativeResultsFrame50LA> nativeResultsFrame;
    const bool commonResultsDraw=native_common_results50PD::state().replayActive;
    if(((sourceConsumedIntroActivation&&isRaTrack)||commonResultsDraw)&&sourceCameraSession){
     rr::game::OriginalResults104Diagnostic50LA results;
     const bool preparedResults=originalMainApplication.prepareNativeResultsFrame50LA(*sourceCameraSession,
         renderFrame,nativeResultsFrame,results);
     if(!preparedResults&&!(sourceConsumedIntroActivation&&isRaTrack)){
      if(results.operation!=replayDrawReported50PD){replayDrawReported50PD=results.operation;
       std::cerr<<"race_replay results_frame_rejected pc="<<std::hex<<results.sourcePC<<std::dec
           <<" operation="<<(results.operation?results.operation:"")<<'\n';}
     }else require50DU(preparedResults,results.operation?results.operation:"native Results Frame rejected");
     if(commonResultsDraw&&preparedResults&&!nativeResultsFrame&&results.operation!=replayDrawReported50PD){
      replayDrawReported50PD=results.operation;
      std::cerr<<"race_replay results_frame none="<<(results.operation?results.operation:"")<<'\n';}
     if(commonResultsDraw&&nativeResultsFrame&&!replayDrawLogged50PD){replayDrawLogged50PD=true;
      std::cerr<<"race_replay results_frame prepared=1\n";}
    }
    if((sourceConsumedIntroActivation&&isRaTrack)||commonResultsDraw){
     OriginalRaceF8Diagnostic50LA f8;
     const bool prepared=sourceCameraSession&&sourceCameraSession->prepareNativeF8Frame50LA(renderFrame,nativeF8Frame,f8);
     if(!prepared&&(isRaTrack||!replayF8Reported50PD++))std::cerr<<"F8_NATIVE_FRAME_REJECT source_pc="<<f8.sourcePC
         <<" operation="<<(f8.operation?f8.operation:"actual Session missing")
         <<" source_effects="<<f8.sourceEffectsCommitted<<'\n';
     if(!prepared&&!(sourceConsumedIntroActivation&&isRaTrack)){}
     else require50DU(prepared,"actual RA native F8 Frame preparation rejected");
    }
    render_diagnostics50GQ::Scope submitTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DriverSubmit);
    bool submitted=false;
    if(isMcTrack){
     require50DU(sourceCameraSession&&mcParticleWindow&&mcParticleRender,"MC actual current particle draw owners unavailable");
     mc_particle_visual50MC::Diagnostic materialDiagnostic;
     bool materialCallbackEntered=false,rendererCallbackEntered=false;
     submitted=mcParticleWindow->withMaterial([&](const mc_particle_visual50MC::MaterialBorrow& material,const auto&){
      materialCallbackEntered=true;
      return mcParticleRender->withDraw(material,sourceCameraSession,mcParticleRenderDiagnostic,[&](const auto&){
       rendererCallbackEntered=true;renderFrame.mcParticles=mcParticleRender;
       struct End{platform::VehicleWindowFrame50DS& frame;~End(){frame.mcParticles.reset();}}end{renderFrame};
       return renderer.submit(next,renderFrame,rendererDiagnostic);
      });
     },materialDiagnostic);
     if(mcParticleRenderDiagnostic.skippedAuthoredFrame152&&!mcParticleFrame152Notice){
      mcParticleFrame152Notice=true;std::cerr<<"MC_NATIVE_SAFETY omitted_authored_unallocated_particle_frame=152 count="
       <<mcParticleRenderDiagnostic.skippedAuthoredFrame152<<" simulation_unchanged=1 synthetic_uv=0\n";
     }
     if(!submitted)std::cerr<<"MC_PARTICLE_SUBMIT_REJECT material_callback="<<materialCallbackEntered
         <<" renderer_callback="<<rendererCallbackEntered<<" material_message="<<materialDiagnostic.message
         <<" material_status="<<static_cast<unsigned>(materialDiagnostic.material.status)
         <<" material_pc="<<materialDiagnostic.material.sourcePC
         <<" numeric_status="<<static_cast<unsigned>(mcParticleRenderDiagnostic.status)
         <<" numeric_pc="<<mcParticleRenderDiagnostic.sourcePC<<" particles="<<mcParticleRenderDiagnostic.particles
         <<" quads="<<mcParticleRenderDiagnostic.quads<<" runs="<<mcParticleRenderDiagnostic.runs<<'\n';
    }else     if(isGbTrack){
     require50DU(sourceCameraSession&&gbParticleWindow&&gbParticleRender,"GB actual current particle draw owners unavailable");
     gb_particle_visual50GB::Diagnostic materialDiagnostic;
     bool materialCallbackEntered=false,rendererCallbackEntered=false;
     submitted=gbParticleWindow->withMaterial([&](const gb_particle_visual50GB::MaterialBorrow& material,const auto&){
      materialCallbackEntered=true;
      return gbParticleRender->withDraw(material,sourceCameraSession,gbParticleRenderDiagnostic,[&](const auto&){
       rendererCallbackEntered=true;renderFrame.gbParticles=gbParticleRender;
       struct End{platform::VehicleWindowFrame50DS& frame;~End(){frame.gbParticles.reset();}}end{renderFrame};
       return renderer.submit(next,renderFrame,rendererDiagnostic);
      });
     },materialDiagnostic);
     if(!submitted)std::cerr<<"GB_PARTICLE_SUBMIT_REJECT material_callback="<<materialCallbackEntered
         <<" renderer_callback="<<rendererCallbackEntered<<" material_message="<<materialDiagnostic.message
         <<" material_status="<<static_cast<unsigned>(materialDiagnostic.material.status)
         <<" material_pc="<<materialDiagnostic.material.sourcePC
         <<" numeric_status="<<static_cast<unsigned>(gbParticleRenderDiagnostic.status)
         <<" numeric_pc="<<gbParticleRenderDiagnostic.sourcePC<<" particles="<<gbParticleRenderDiagnostic.particles
         <<" quads="<<gbParticleRenderDiagnostic.quads<<" runs="<<gbParticleRenderDiagnostic.runs<<'\n';
    }else if(isRcTrack){
     require50DU(sourceCameraSession&&rcParticleWindow&&rcParticleRender,"RC actual particle draw owners unavailable");
     rc_particle_visual50RC::Diagnostic materialDiagnostic;
     bool materialCallbackEntered=false,rendererCallbackEntered=false;
     submitted=rcParticleWindow->withMaterial([&](const rc_particle_visual50RC::MaterialBorrow& material,const auto&){
      materialCallbackEntered=true;
      return rcParticleRender->withDraw(material,sourceCameraSession->view(),sourceCameraSession->viewport(),
          sourceTrackFog,gcDrawLights[0],rcParticleRenderDiagnostic,[&](const auto&){
       rendererCallbackEntered=true;
       renderFrame.rcParticles=rcParticleRender;
       struct End{platform::VehicleWindowFrame50DS& frame;~End(){frame.rcParticles.reset();}} end{renderFrame};
       return renderer.submit(next,renderFrame,rendererDiagnostic);
      });
     },materialDiagnostic);
     if(!submitted){
      std::cerr<<"RC_PARTICLE_SUBMIT_REJECT material_callback="<<materialCallbackEntered
          <<" renderer_callback="<<rendererCallbackEntered
          <<" material_message="<<materialDiagnostic.message
          <<" material_status="<<static_cast<unsigned>(materialDiagnostic.material.status)
          <<" material_pc="<<materialDiagnostic.material.sourcePC
          <<" material_row="<<materialDiagnostic.material.row
          <<" material_pass="<<materialDiagnostic.material.pass
          <<" material_map="<<materialDiagnostic.material.map
          <<" material_field="<<(materialDiagnostic.material.field?materialDiagnostic.material.field:"")
          <<" decode_kind="<<static_cast<unsigned>(materialDiagnostic.decode.kind)
          <<" decode_code="<<materialDiagnostic.decode.code
          <<" bank_error="<<static_cast<unsigned>(materialDiagnostic.bank.error)
          <<" numeric_status="<<static_cast<unsigned>(rcParticleRenderDiagnostic.status)
          <<" numeric_pc="<<rcParticleRenderDiagnostic.sourcePC
          <<" numeric_particles="<<rcParticleRenderDiagnostic.particles
          <<" numeric_quads="<<rcParticleRenderDiagnostic.quads
          <<" numeric_runs="<<rcParticleRenderDiagnostic.runs<<'\n';
     }
    }else submitted=renderer.submit(next,renderFrame,rendererDiagnostic);
    // Release on BOTH boolean outcomes before publication/fade/input work.
    // Exceptions unwind this local before the earlier Frame; no source state
    // setter or retirement is invoked while its native F8 borrow remains live.
    nativeF8Frame.reset();
    // These actual model/context borrows end with synchronous preparation;
    // later display completion or Session close cannot reuse them.
    renderFrame.sourceBodyModelPackets50LA.fill(nullptr);
    renderFrame.sourceBodyFog50LA=nullptr;renderFrame.sourceBodyGlobals50LA=nullptr;
    renderFrame.sourceDiffuseCamera50LA=nullptr;
    for(auto& opponent:opponents)opponent.sourceBodyModelPackets50LA.fill(nullptr);
    if(!submitted){
     reportRenderer50DU(rendererDiagnostic);
     throw std::runtime_error("vehicle submission failed; rejected frame was not swapped");
    }
    if(rendererDiagnostic.originalRaceHudRejected&&!sourceRaceHudRejectionReported){
     std::cerr<<"Original race lap HUD live projection rejected; native HUD retained.\n";
     sourceRaceHudRejectionReported=true;
    }
    if(raceEnabled){
     nextRacePresentation.valid=true;acceptedRacePresentation=nextRacePresentation;
    }
    commitEngineFlare50GV(flameOutput,s,baseTail,coldFlame);
    if(raceEnabled)for(std::size_t racer=1;racer<racerBank.count();++racer)
     if(opponentFlamePrepared[racer-1])racerBank.visit(racer,[&](){
      commitEngineFlare50GV(opponentFlames[racer-1],s,baseTail,coldFlame);
     });
    submitTiming.finish();
    renderTiming.finish();
    if(headlessLive&&capturePending&&captureVisibility&&!sourceCameraSession){
     phase_timing50GK::Scope captureTiming(phaseTiming,phase_timing50GK::Phase::Capture);
     capture_visibility50fo_fixture::Report visibility{};
     capture_visibility50fo_fixture::Diagnostic visibilityDiagnostic{};
     require50DU(capture_visibility50fo_fixture::inspect(trackMesh,next,camera,
         visibility,visibilityDiagnostic),"capture visibility evidence rejected");
     const auto pointJson=[](const auto& p){return nlohmann::json::array({p.x,p.y,p.z});};
     const auto hitJson=[&](const auto& h){return nlohmann::json{{"hit",h.hit},
         {"t",h.t},{"triangle",h.triangle},{"point",pointJson(h.point)}};};
     nlohmann::json receipt{{"tick",completedFrames},{"wrong_way",hud.wrongWay},{"hud_vertices",rendererDiagnostic.hostHudVertices},{"cockpit_center",pointJson(visibility.cockpitCenter)},
         {"camera_eye",pointJson(visibility.cameraEye)},
         {"camera_to_cockpit",hitJson(visibility.cameraToCockpit)},
         {"cockpit_above",hitJson(visibility.cockpitVertical.above)},
         {"cockpit_below",hitJson(visibility.cockpitVertical.below)},
         {"camera_above",hitJson(visibility.cameraVertical.above)},
         {"camera_below",hitJson(visibility.cameraVertical.below)},
         {"secondary_active",s.bodies.vehicle838},{"lifecycle_mode",s.lifecycle.vehicle83C},
         {"main_contact_ticks",mainContactFrames},{"secondary_contact_ticks",secondaryContactFrames},
         {"triangles",visibility.trianglesScanned}};
     std::cerr<<"capture_visibility "<<receipt.dump()<<'\n';
    }
    // Acceptance timing counts GPU-completed frames, not queued submits.
    // The non-camera headless path completes here; source-camera display
    // completes and is timed at its actual publication boundary below.
    if(headlessLive&&!sourceCameraSession){
     phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
     require50DU(renderer.finishSubmittedWork(completionDiagnostic),
        "GPU completion failed during headless timing");
    }
    // Publish CPU-bank ownership only after the renderer accepts this exact batch.
    presenter.published=next;presenter.publishedCpuBank=bank;presenter.hasPublished=true;
    addCounter50DU(presenter.committedFrames);
    bool actualGameplayImageCompleted=false;
#if defined(__linux__)
    // Select before the exact swap. A restored-camera private underlay and an
    // unfinished video sample are never eligible window images.
    const bool privateUnderlay=sourceCameraSession&&
        (sourceCameraSession->cameraMode()==5||sourceCameraSession->cameraMode()==0)&&!sourceMainBaseReady;
    //The phase may switch before the first running HUD/image is published.
    //Wait for one actual race second before calling this a settled capture.
    const bool captureWindowImage=host.hasWindow()&&!privateUnderlay&&
        !(sourceCameraSession&&(sourceCameraSession->blockingVideoPending()||sourceCameraSession->asyncVideoPending()))&&
        windowCapture.due(windows_capture50LD::Stage::race,
            race.phase==race_session50HM::Phase::Racing&&race.raceSeconds>=1.0f&&
            (allAiEnabled||humanCourseEnabled)&&race.racerCount==raceRacerCount&&
            racerBank.count()==raceRacerCount&&rendererDiagnostic.trackDraws>0);
    // Discard BEFORE readback or observation: even a capture at the exact FPS
    // threshold must not complete an interval containing readback latency.
    if(captureWindowImage){
     requireOriginalAiFleetReady();discardTrackBaseline();
     renderer.logRaCapturePixelProvenance();
    }
#else
    const bool captureWindowImage=false;
#endif
    if(sourceCameraSession){
     if((sourceCameraSession->cameraMode()==5||sourceCameraSession->cameraMode()==0)&&!sourceMainBaseReady){
      // Real private restored-camera underlay, GPU-completed but not yet swapped.
      // The next actual async overlay is the displayed image: no clear-frame
      // flash and no fabricated displayed-history advance for this underlay.
      phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
      require50DU(renderer.finishSubmittedWork(completionDiagnostic)&&completionDiagnostic.submittedWorkCompleted,
          "first restored-camera private underlay did not complete");
      completionTiming.finish();
      sourceMainBaseReady=true;
      require50DU(sourceCameraSession->advanceRaceFadeInPrefix().status==sourceCamera::Status::complete,
          "actual restored-camera publication did not admit original fade-in prefix");
     }else{
      // Includes history staging, existing GPU completion and the actual
      // window swap. This is not a GPU-only timer and adds no synchronization.
      frameHitch50DU.completion=FrameHitch50DU::Clock::now();
      stall_watchdog50DU::mark("present_gameplay_frame");
      phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
      const bool displayAccepted=windowCapture.swapAndCapture(renderer,host,captureWindowImage,windowSwaps+1,
          [&](){return renderer.completeGameplayDisplay(host,hostDiagnostic,completionDiagnostic);});
      completionTiming.finish();
      displayWallTotals.record(false,displayAccepted,completionDiagnostic);
      if(!displayAccepted)displayWallTotals.reportAndReset("failure");
      require50DU(displayAccepted,"original camera actual displayed image/history completion failed");
      actualGameplayImageCompleted=true;
      if(sourcePauseParentAttached){
       spectatorUnderlayPublication=sourceCameraSession->publicationCount();
       spectatorUnderlayWidth=info.drawableWidth;spectatorUnderlayHeight=info.drawableHeight;
      }

      if(host.hasWindow())addCounter50DU(windowSwaps);
      // This receipt excludes private unswapped underlays, fades, menu frames,
      // CPU loop iterations, and ordinary queued renderer submissions.
      if(!captureWindowImage&&host.hasWindow()&&completionDiagnostic.submittedWorkCompleted&&
          completionDiagnostic.gameplayWindowSwapSubmitted)observeTrackBaseline(completionDiagnostic);
     }
     renderedCameraPublication=sourceCameraSession->publicationCount();
     // Results replay: the restart's 1A7BD0 fade-in (manager 8) is serviced
     // after this actual completed gameplay publication of the restarted scene.
     if(native_common_results50PD::state().replayActive&&actualGameplayImageCompleted&&
        sourceCameraSession->pendingDisplayWork()){
      const auto fade=originalMainApplication.advanceConsumedSessionFadeIn1A7BD0(*sourceCameraSession);
      static unsigned reportedPresentedFade=0xffu;
      if(static_cast<unsigned>(fade.status)!=reportedPresentedFade){reportedPresentedFade=static_cast<unsigned>(fade.status);
       std::cerr<<"race_replay fade_in_presented status="<<reportedPresentedFade<<" pc="<<std::hex<<fade.sourcePC<<std::dec
           <<" pending="<<sourceCameraSession->pendingDisplayWork()<<'\n';}
     }
     if(humanCourseEnabled&&sourceCameraSession->endingFadeQueued()){
      require50DU(actualGameplayImageCompleted&&
          sourceCameraSession->advanceHumanEndFadeManager().status==sourceCamera::Status::complete,
          "human ending must fade its actual completed final gameplay publication");
     }
    }else if(host.hasWindow()){
     phase_timing50GK::Scope swapTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
     if(!windowCapture.swapAndCapture(renderer,host,captureWindowImage,windowSwaps+1,
         [&](){return host.swapWindow(hostDiagnostic);})){
      reportHost50DU(hostDiagnostic);throw std::runtime_error("window swap submission failed");
     }
     addCounter50DU(windowSwaps);
    }
    if(allAiEnabled&&originalAiSession.all8FinishAwaitingDisplay&&!sourceCameraSession){
     //The non-source-camera compatibility presentation must also retire real
     //GPU work before sealing; a queued submit or requested swap is insufficient.
     require50DU(renderer.finishSubmittedWork(completionDiagnostic)&&completionDiagnostic.submittedWorkCompleted,
         "ALL8 final real gameplay image did not complete");
     actualGameplayImageCompleted=true;
    }
    if(sourceRaceHud.loaded()&&actualGameplayImageCompleted&&sourceCameraSession->nativeHudVisible())
     require50DU(sourceRaceHud.commitNativePresentation(0,sourceCameraSession->viewport()),
         "native HUD accepted-display text-state commit rejected");
    if(actualGameplayImageCompleted&&rendererDiagnostic.originalRaceHudQuads&&!sourceRaceHudDisplayReported){
     std::cerr<<"ORIGINAL RACE LAP HUD: actual completed display quads="<<rendererDiagnostic.originalRaceHudQuads
         <<" leaves="<<rendererDiagnostic.originalRaceHudLeaves<<'\n';sourceRaceHudDisplayReported=true;
    }
    if(actualGameplayImageCompleted&&rendererDiagnostic.originalSpeedBoostPrimitives&&!sourceSpeedBoostDisplayReported){
     std::cerr<<"ORIGINAL SPEED BOOST: actual completed display primitives="<<rendererDiagnostic.originalSpeedBoostPrimitives
         <<" leaves="<<rendererDiagnostic.originalSpeedBoostLeaves
         <<" speed=original_body heat=original_boost blink=original_race scheduling=native_host"<<'\n';
     sourceSpeedBoostDisplayReported=true;
    }
    if(actualGameplayImageCompleted&&rendererDiagnostic.originalTimeStatsQuads&&!sourceTimeStatsDisplayReported){
     std::cerr<<"ORIGINAL TIME STATS: actual completed display quads="<<rendererDiagnostic.originalTimeStatsQuads
         <<" leaves="<<rendererDiagnostic.originalTimeStatsLeaves
         <<" timing=original_bank rank=original_ai_manager scheduling=native_host"<<'\n';
     sourceTimeStatsDisplayReported=true;
    }
    if(actualGameplayImageCompleted&&rendererDiagnostic.originalPositionStatsQuads&&!sourcePositionStatsDisplayReported){
     std::cerr<<"ORIGINAL POSITION STATS: actual completed display quads="<<rendererDiagnostic.originalPositionStatsQuads
         <<" leaves="<<rendererDiagnostic.originalPositionStatsLeaves
         <<" rank=original_ai_manager radar=original_map markers=original_bank scheduling=native_host"<<'\n';
     sourcePositionStatsDisplayReported=true;
    }
    if(actualGameplayImageCompleted&&rendererDiagnostic.originalDamageQuads&&!sourceDamageDisplayReported){
     std::cerr<<"ORIGINAL DAMAGE: actual completed display quads="<<rendererDiagnostic.originalDamageQuads
         <<" leaves="<<rendererDiagnostic.originalDamageLeaves
         <<" health=original_bank proximity=original_scan native_observation=all8_ai scheduling=native_host"<<'\n';
     sourceDamageDisplayReported=true;
    }
    if(nativeGcWater){
     platform::VehicleWindowTrackVisibilityReceipt50DS visibilityReceipt;
     require50DU(renderer.completedTrackSpanVisibility(nativeFireSpanIds.data(),nativeFireSpanDrawn.data(),
         static_cast<std::uint32_t>(nativeFireSpanIds.size()),visibilityReceipt,completionDiagnostic),
         "actual completed gameplay span visibility unavailable");
     std::array<bool,6> drawn{};
     for(std::size_t i=0;i<nativeFireSpanDrawn.size();++i)
      drawn[nativeFireSpanOwner[i]]=drawn[nativeFireSpanOwner[i]]||nativeFireSpanDrawn[i]!=0;
     require50DU(nativeGcWater->completedRender(visibilityReceipt.completedGameplayGeneration,drawn),
         "actual completed native water/fire publication rejected");
    }
    if(mcWaterOwner&&actualGameplayImageCompleted){
     platform::VehicleWindowTrackVisibilityReceipt50DS waterReceipt;
     require50DU(renderer.completedTrackSpanVisibility(nullptr,nullptr,0,waterReceipt,completionDiagnostic),
         "MC water requires actual completed gameplay-generation receipt");
     const auto published=mcWaterOwner->runtime().present(waterReceipt.completedGameplayGeneration,true);
     require50DU(published.status==WaterPresentationStatus50JC::Published||
         published.status==WaterPresentationStatus50JC::DuplicateFrame,
         "MC actual completed view water-origin publication rejected");
    }
    const bool publishAll8Results=allAiEnabled&&originalAiSession.all8FinishAwaitingDisplay&&actualGameplayImageCompleted;
    if(publishAll8Results){
     originalAiSession.manager.flags58=static_cast<std::uint8_t>(originalAiSession.manager.flags58|1u);
     for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
      s.fields.applicationByte58=originalAiSession.manager.flags58;
     });
     originalAiSession.all8FinishAwaitingDisplay=false;originalAiSession.all8ResultsComplete=true;
     race.resultsClosed=true;race.phase=race_session50HM::Phase::Complete;
     humanResultsReady=true; //Native overlay carrier only; never activates a human driver.
     std::cerr<<"ORIGINAL ALL8 RESULTS: final gameplay image completed; retaining native scene.\n";
    }
    addCounter50DU(submittedDrawFrames);
    menuImagePublished=false;forceSceneRender=publishAll8Results||originalAiSession.all8FinishAwaitingDisplay;
    lastCommittedTick=completedFrames;lastCommittedWrongWay=hud.wrongWay;
    // Capture only the exact submitted/swapped gameplay image, never the
    // private restored-camera underlay or an in-progress original fade.
    const bool windowImage=host.hasWindow()&&(actualGameplayImageCompleted||!sourceCameraSession)&&
        !(sourceCameraSession&&(sourceCameraSession->blockingVideoPending()||sourceCameraSession->asyncVideoPending()));
    const bool settledTrack=race.phase==race_session50HM::Phase::Racing&&race.raceSeconds>=1.0f&&
            (allAiEnabled||humanCourseEnabled)&&
        race.racerCount==raceRacerCount&&racerBank.count()==raceRacerCount&&rendererDiagnostic.trackDraws>0;
#if defined(__linux__)
    if(captureWindowImage&&windowImage){
#else
    if(windowImage&&windowCapture.due(windows_capture50LD::Stage::race,settledTrack)){
#endif
     requireOriginalAiFleetReady();
     J receipt={{"track",platform::hostTrackCode50IM(loadedTrack)},{"selected_rider",playerRacer60},
         {"world_ticks",completedFrames},{"render_commits",submittedDrawFrames},{"window_swaps",windowSwaps},
         {"race_phase",static_cast<unsigned>(race.phase)},{"race_seconds",race.raceSeconds},
         {"original_ai_racers",originalAiOpponentCount},{"human_racers",originalAiHumanCount},
         {"track_draws",rendererDiagnostic.trackDraws},{"camera_mode",sourceCameraSession?static_cast<int>(sourceCameraSession->cameraMode()):-1}};
     if(originalMainApplication.riderProfile){
      original_course_targets50KX::Selection selection;
      if(originalMainApplication.riderProfile->courseRaceSnapshot(selection))receipt["course_selection"]={
          {"course",selection.course},{"planet",selection.planet},{"rider",selection.rider},
          {"subtype",selection.subtype},{"laps",selection.laps},{"entrants",selection.entrants}};
     }
     windowCapture.record(renderer,host,windows_capture50LD::Stage::race,settledTrack,std::move(receipt));
     discardTrackBaseline();
    }
    // A post-display transition can suppress capture; do not carry that image
    // into another frame, a pause, a restart, or a later source session.
    windowCapture.discardPendingWindowImage();
    captureCommitted();
   }
   // Unattended long-race test: keep racing until the wall budget ends.
   static const bool raceFullBudget50DU=[]{const char* v=std::getenv("RR_TEST_RACE_FULL_BUDGET");return v&&std::string(v)=="1";}();
   if(!(raceFullBudget50DU&&captureRun.unattended())&&
      captureRun.evidenceComplete()&&trackBaseline.complete()&&captureRun.videoHoldComplete()){
    composePacket(false,input);sampleController();
    if(captureRun.takeNativeRestartRequest())restartRequested=true;
    break;
   }
   const auto afterWork=monotonicNow50DU();
   if(allAiEnabled&&afterWork-lastAllAiStatusNs>=1000000000ull){
    reportOriginalAiFleet("status");lastAllAiStatusNs=afterWork;
   }
   if(afterWork-lastStatusNs>=5000000000ull){
    if(sourceCameraSession&&phaseTiming.enabled){const auto progress=sourceCameraSession->nativeStartProgress();
     std::cerr<<"original_camera_start mode="<<progress[0]<<" next_route="<<progress[1]
       <<" route_count="<<progress[2]<<" countdown_installed="<<progress[3]
       <<" camera_ticks="<<sourceCameraTick<<'\n';
    }
    rr::game::OriginalApplicationAnimationClockSample50KG reportedApplicationAnimation;
    const bool reportedApplicationTimestampPublished=sourceApplicationClock.read(reportedApplicationAnimation);
    if(phaseTiming.enabled){
     for(std::size_t phase=0;phase<phase_timing50GK::phaseCount;++phase){
      const auto& total=phaseTiming.totals[phase];
      std::cerr<<"phase_timing "<<phase_timing50GK::phaseName(static_cast<phase_timing50GK::Phase>(phase))
          <<" calls="<<total.calls<<" wall_calls="<<total.wallCalls<<" cpu_calls="<<total.cpuCalls
          <<" wall_ns="<<total.wallNs<<" cpu_ns="<<total.cpuNs<<" max_wall_ns="<<total.maxWallNs
          <<" cpu_at_max_wall_ns="<<total.cpuAtMaxWallNs<<" cpu_at_max_valid="<<total.cpuAtMaxWallValid
          <<" clock_failure="<<total.clockFailure<<" overflow="<<total.overflow<<'\n';
     }
     constexpr const char* simulationNames[]={"fleet_observation","dynamic_projection","racer_children",
         "remaining_children","physics_preparation","racer_physics","fleet_ready",
         "scene_animation_particles","ai_manager","camera_and_hud","racer_contacts","sample_race_field",
         "race_presentation","scheduled_total"};
     for(std::size_t i=0;i<14;++i){const auto& total=simulationTotals[i];
      std::cerr<<"simulation_subphase "<<simulationNames[i]<<" nested_under="<<(i==7||i==8||i==9?"remaining_children":"physics")<<" calls="<<total.calls
          <<" wall_ns="<<total.wallNs<<" cpu_ns="<<total.cpuNs<<" max_wall_ns="<<total.maxWallNs
          <<" clock_failure="<<total.clockFailure<<" overflow="<<total.overflow<<'\n';
      simulationTotals[i]={};
     }
     displayWallTotals.reportAndReset("periodic");
     phaseTiming.resetInterval();
    }
    if(renderDiagnostics.enabled){
     for(std::size_t phase=0;phase<render_diagnostics50GQ::subphaseCount;++phase){
      const auto& total=renderDiagnostics.totals[phase];
      const auto& scene=renderDiagnostics.sceneAtMaxWall[phase];
      std::cerr<<"render_subphase "<<render_diagnostics50GQ::subphaseName(static_cast<render_diagnostics50GQ::Subphase>(phase))
          <<" calls="<<total.calls<<" wall_calls="<<total.wallCalls<<" cpu_calls="<<total.cpuCalls
          <<" wall_ns="<<total.wallNs<<" cpu_ns="<<total.cpuNs<<" max_wall_ns="<<total.maxWallNs
          <<" cpu_at_max_wall_ns="<<total.cpuAtMaxWallNs<<" cpu_at_max_valid="<<total.cpuAtMaxWallValid
          <<" clock_failure="<<total.clockFailure<<" overflow="<<total.overflow
          <<" scene_at_max_valid="<<scene.valid<<" scene_tick="<<scene.tick
          <<" subnormal_words="<<scene.subnormalWords
          <<" subnormal_local/ready_world/dirty_world="<<scene.localSubnormalWords<<'/'
          <<scene.readyWorldSubnormalWords<<'/'<<scene.dirtyWorldSubnormalWords
          <<" first_source="<<render_diagnostics50GQ::matrixSourceName(scene.firstSource)
          <<" first_node="<<scene.firstNode<<" first_lane="<<scene.firstLane<<" first_bits_u32="<<scene.firstBits
          <<" mxcsr_available="<<scene.mxcsrAvailable<<" mxcsr_u32="<<scene.mxcsr<<'\n';
     }
     {const auto& t=renderDiagnostics.opponentDrawTiming;
      std::cerr<<"opponent_draw_stages component_calls="<<t.calls<<" scene_palette_ns="<<t.ns[0]
          <<" positions_ns="<<t.ns[1]<<" source_colors_ns="<<t.ns[2]<<" normals_ns="<<t.ns[3]
          <<" specular_publish_ns="<<t.ns[4]<<'\n';}
     const auto& scene=renderDiagnostics.latestScene;
     std::cerr<<"render_census samples="<<renderDiagnostics.sceneSamples
         <<" frames_with_subnormal="<<renderDiagnostics.sceneFramesWithSubnormal
         <<" frames_with_nonfinite="<<renderDiagnostics.sceneFramesWithNonfinite
         <<" max_subnormal_words="<<renderDiagnostics.maxSubnormalWords
         <<" last_tick="<<scene.tick<<" last_valid="<<scene.valid
         <<" nodes="<<scene.nodes<<" words="<<scene.words<<" dirty_nodes="<<scene.dirtyNodes
         <<" last_subnormal_words="<<scene.subnormalWords<<" last_nonfinite_words="<<scene.nonfiniteWords
         <<" overflow="<<renderDiagnostics.overflow<<'\n';
     renderDiagnostics.resetInterval();
    }
    if(gcAnimatedWorld)std::cerr<<"GC WORLD RUNTIME: accepted_service_steps="<<gcAnimatedWorldSteps
        <<" source_application_ms="<<reportedApplicationAnimation.current230
        <<" windmill_source_time="<<gcAnimatedWorld->state(1).sequence.current34
        <<" opening_active="<<gcAnimatedWorld->state(0).sequence.active90
        <<" opening_source_time="<<gcAnimatedWorld->state(0).sequence.current34
        <<" gate_native_policy="<<gcNativeGate
        <<" trigger_ready="<<bool(gcTriggerActions&&gcTriggerActions->ready())
        <<" gate_fired="<<bool(gcTriggerActions&&(gcTriggerActions->state(gc_trigger_authoring50IN::gate).flags1B8&8u))
        <<" trigger_dispatches="<<(gcTriggerActions?gcTriggerActions->nativeGroup().dispatches:0u)
        <<" lost_ai_writes="<<(gcTriggerActions?gcTriggerActions->counters().lostAiWrites:0u)
        <<" gate_sound_starts="<<(gcTriggerActions?gcTriggerActions->counters().soundStarts:0u)
        <<" gate_sound_culled="<<(gcTriggerActions?gcTriggerActions->counters().soundCulled:0u)
        <<" compound_owners=2 ordered_parts=4 registered_candidates="<<terrainOwnerCount
        <<" dynamic_draws/vertices/indices="<<rendererDiagnostic.dynamicWorldDraws<<'/'
        <<rendererDiagnostic.dynamicWorldVerticesUploaded<<'/'<<rendererDiagnostic.dynamicWorldIndices
        <<" atm_unique=76 atm_images=35\n";
    const auto& drive=m.components.bindings;
    const auto& cockpitCg=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.model1B4)+1].localC0;
    float bodyDistanceSquared=0.f;
    for(unsigned axis=0;axis<3;++axis){
     const float delta=m.components.root.body.position.v[axis]-s.bodies.secondary.root.body.position.v[axis];
     bodyDistanceSquared+=delta*delta;
    }
    const auto& cockpitPose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.model1B4)].localC0;
    const auto& leftPose=s.nodes[static_cast<unsigned>(x.bindings.visual.left1BC)].localC0;
    const auto& rightPose=s.nodes[static_cast<unsigned>(x.bindings.visual.right1B8)].localC0;
    float renderDistanceSquared=0.f,enginePairSquared=0.f;
    for(unsigned axis=0;axis<3;++axis){
     const float delta=cockpitPose.v[12+axis]-(leftPose.v[12+axis]+rightPose.v[12+axis])*.5f;
     const float separation=leftPose.v[12+axis]-rightPose.v[12+axis];
     renderDistanceSquared+=delta*delta;enginePairSquared+=separation*separation;
    }
    const double measuredSeconds=double(afterWork-lastStatusNs)*1e-9;
    const double measuredFps=double(submittedDrawFrames-lastStatusRenderCount)/measuredSeconds;
    std::cerr<<(headlessLive?"gpu_completed_fps ":"window_submit_fps ")<<measuredFps
             <<" interval_s "<<measuredSeconds<<" ticks "<<completedFrames<<" render_commits "<<submittedDrawFrames<<" window_swaps "<<windowSwaps
             <<" track_draws/textured/untextured "<<rendererDiagnostic.trackDraws<<'/'
             <<rendererDiagnostic.trackTexturedDraws<<'/'<<rendererDiagnostic.trackUntexturedDraws
             <<" track_cull candidates/retained/culled/fallback "<<rendererDiagnostic.trackCandidateDraws<<'/'
             <<rendererDiagnostic.trackRetainedDraws<<'/'<<rendererDiagnostic.trackCulledDraws<<'/'<<rendererDiagnostic.trackCullingFallback
             <<" cable_draws/vertices/indices "<<rendererDiagnostic.cableDraws<<'/'
             <<rendererDiagnostic.cableVertices<<'/'<<rendererDiagnostic.cableIndices
             <<" binder_loaded/draws "<<gameplayBinder.loaded()<<'/'<<rendererDiagnostic.gameplayBinderDraws
             <<" binder_clock_published "<<reportedApplicationTimestampPublished
             <<" camera_adjusted/limited "<<cameraAdjustedFrames<<'/'<<cameraLimitedFrames
             <<" camera_shift "<<cameraFraming.right<<','<<cameraFraming.up<<','<<cameraFraming.retreat
             <<" terrain_queries/hits/supported "<<terrainQueries<<'/'<<terrainHitFrames<<'/'<<terrainSupportedFrames
             <<" terrain_state/hits "<<m.components.probe348.field104<<'/'<<m.components.probe348.field110<<'/'<<m.components.probe348.field114
             <<" hud_draws "<<rendererDiagnostic.hostHudDraws
             <<" position "<<m.components.root.body.position.v[0]<<','
             <<m.components.root.body.position.v[1]<<','<<m.components.root.body.position.v[2]
             <<" left/right drive "<<drive.engineDrive[0]<<'/'<<drive.engineDrive[1]
             <<" brake "<<drive.aeroBrakeEC
             <<" native_cached_speed34_internal "<<m.cachedSpeed34
             <<" boost held/state/active "<<controller.buttons().records[boostButtonCode].held
             <<'/'<<boostState.vehicle64<<'/'<<m.selectedEngineFlags.engine34
             <<" boost heat/duration "<<boostState.heat7C<<'/'<<boostState.duration6C
             <<" slide held/flag/D8 "<<controller.buttons().records[powerslideButtonCode].held
             <<'/'<<((m.controls.player.flagsB9C&4u)!=0)<<'/'<<m.components.bindings.aeroTriggerD8
             <<" aero timers140/144/low "<<m.components.aero.timer140<<'/'<<m.components.aero.timer144<<'/'<<m.components.aero.lowSpeedDc
             <<" repair held/active "<<controller.buttons().records[repairButtonCode].held
             <<'/'<<((m.controls.player.flagsB9C&2u)!=0)
             <<" repair_active_ticks "<<repairActiveTicks
             <<" main/secondary primary zone "<<mainMembership.storage.links[0].zone
             <<'/'<<secondaryMembership.storage.links[0].zone
             <<" main/secondary crossed portals "<<mainCrossedPortals<<'/'<<secondaryCrossedPortals
             <<" host_auto/status/node/error/turn/speed/target/left/right "<<autoDriveEnabled<<'/'
             <<unsigned(courseDiagnostic.status)<<'/'<<courseDiagnostic.node<<'/'<<courseDiagnostic.headingError<<'/'
             <<courseDiagnostic.turn<<'/'<<courseDiagnostic.bodySpeed<<'/'<<courseDiagnostic.targetSpeed<<'/'
             <<courseOutput.left<<'/'<<courseOutput.right
             <<" wrong_way/updates/active_ticks/dot/angle "<<((lapContinuity.prefix.flags10CC&2u)!=0)<<'/'
             <<wrongWayUpdates<<'/'<<wrongWayActiveTicks<<'/'<<wrongWayReceipt.directionDot<<'/'<<wrongWayReceipt.sourceAngle
             <<" host_laps/marker_cursor/callbacks/unavailable "<<lapContinuity.state.vehicleCC8<<'/'
             <<lapContinuity.state.vehicleFF8<<'/'<<lapOwner.completedCallbacks<<'/'<<lapOwner.unavailableSounds
             <<" nav C80/C84/C88/C8C "<<navigationProgress.reset.vehicleC80<<'/'
             <<navigationProgress.vehicleC84<<'/'<<navigationProgress.reset.vehicleC88<<'/'
             <<navigationProgress.reset.vehicleC8C
             <<" anchor_index78 "<<static_cast<unsigned>(
                 selectedMapIndex50MB(navigationProgress.reset.vehicleC8C))
             <<" nav map/AI sequential "<<navigationMapSequential<<'/'<<navigationAISequential
             <<" scan "<<navigationMapScan<<'/'<<navigationAIScan
             <<" left/right damage "<<s.fields.damage7D4<<'/'<<s.fields.damage7D0
             <<" cockpit_cg_local "<<cockpitCg.v[12]<<','<<cockpitCg.v[13]<<','<<cockpitCg.v[14]
             <<" secondary_mode "<<s.bodies.vehicle838<<'/'<<s.lifecycle.vehicle83C
             <<" dormant_or_active_body_spacing "<<std::sqrt(bodyDistanceSquared)
             <<" rendered_spacing "<<std::sqrt(renderDistanceSquared)
             <<" engine_pair_spacing "<<std::sqrt(enginePairSquared)
             <<" dropped host ms "<<discardedHostNs/1000000
             <<" frame_ms_buckets <=17/<=20/<=25/<=33/<=50/>50 "<<raceFrames50AU.buckets[0]<<'/'<<raceFrames50AU.buckets[1]
             <<'/'<<raceFrames50AU.buckets[2]<<'/'<<raceFrames50AU.buckets[3]<<'/'<<raceFrames50AU.buckets[4]
             <<'/'<<raceFrames50AU.buckets[5]<<" worst_ms "<<raceFrames50AU.intervalWorstMs;
    {const auto& split=stepSplit50GG;const auto& w=split.worst;
     std::cerr<<" step_ms steps/avg/worst "<<split.steps<<'/'<<(split.steps?split.sumTotal/double(split.steps):0.0)
         <<'/'<<split.worstTotal
         <<" worst_step children/racer_physics/world_service/ai_manager/camera/contacts/field/presentation/fleet_other "
         <<w[2]<<'/'<<w[5]<<'/'<<w[7]<<'/'<<w[8]<<'/'<<w[9]<<'/'<<w[10]<<'/'<<w[11]<<'/'<<w[12]
         <<'/'<<(w[0]+w[1]+w[4]+w[6])<<'\n';}
    stepSplit50GG.resetWindow();
    raceFrames50AU.buckets={};raceFrames50AU.intervalWorstMs=0.0;
    if(audioEnabled){
     const auto ai=liveAudio->info();
     std::cerr<<"audio ready/active/starts/mixed_frames/captured_frames/underruns "
         <<liveAudio->bank().ready<<'/'<<liveAudio->activeVoices()<<'/'
         <<liveAudio->counters().starts<<'/'<<ai.mixedFrames<<'/'<<ai.capturedFrames
         <<'/'<<ai.deviceUnderrunFrames<<" clipped_samples="<<liveAudio->clippedSamples()<<'\n';
     std::cerr<<"audio engine_damage updates/draws/dips/returns/completions "
         <<engineDamageUpdates<<'/'<<engineDamageDraws<<'/'<<engineDamageDips<<'/'
         <<engineDamageReturns<<'/'<<engineDamageCompletions
         <<" pitch_scale="<<engineDamage.scale484
         <<" cooldown="<<engineDamage.cooldown474<<'\n';
     std::cerr<<"audio speech_events/cues/unjoined_announcer "<<speechEvents<<'/'<<speechCues
         <<'/'<<announcerRequestsUnjoined<<'\n';
     if(raceEnabled){const auto& r=raceAudio->counters();
      std::cerr<<"audio race ready/starts/countdown/lap/finish/position/unavailable "
          <<raceAudio->bank().ready<<'/'<<r.starts<<'/'<<r.countdownEvents<<'/'<<r.lapEvents
          <<'/'<<r.finishEvents<<'/'<<r.positionEvents<<'/'<<r.unavailable<<'\n';
      const auto& episode=liveAudio->announcerCounters();
      std::cerr<<"audio shared_Dax starts/begins/ends/completions/preemptions/rejected "
          <<episode.starts<<'/'<<episode.begins<<'/'<<episode.ends<<'/'
          <<episode.completions<<'/'<<episode.preemptions<<'/'<<episode.rejected
          <<" group_failures="<<liveAudio->counters().groupFailures;
      if(const auto* groups=liveAudio->groupsForProviders()){
       std::cerr<<" group_levels=";
       for(const auto level:groups->levels().volume38)std::cerr<<level<<'/';
      }
      std::cerr<<'\n';
      const auto& o=opponentAudio->counters();
      std::cerr<<"audio opponent ready/updates/starts/controls/stops/unavailable/normalized "
          <<opponentAudio->bank().ready<<'/'<<o.updates<<'/'<<o.starts<<'/'<<o.controls
          <<'/'<<o.stops<<'/'<<o.unavailable<<'/'<<o.normalizedUpdates
          <<" rejected="<<o.rejected
          <<" boost_onsets/starts/unavailable="<<o.boostOnsets<<'/'<<o.boostStarts<<'/'<<o.boostUnavailable<<'\n';
     }
     std::cerr<<"audio material ready/starts/controls/stops/unavailable "<<materialAudio->bank().ready
         <<'/'<<materialAudio->counters().starts<<'/'<<materialAudio->counters().controls
         <<'/'<<materialAudio->counters().stops<<'/'<<materialAudio->counters().unavailable<<'\n';
     if(originalAiAudioEnabled)std::cerr<<"audio original_ai_material dispatches/shared_rng_draws "
         <<originalAiAudio->materialDispatches()<<'/'<<originalAiAudio->materialRandomDraws()
         <<" pitch_policy=native_decoded_rate_unity4096 source_sample_metadata=unknown\n";
    }
    if(mode1Enabled){
     const auto& dax=sourceAnnouncer->state();const auto& last=sourceAnnouncer->lastRequest();
     std::cerr<<"mode1 world="<<mode1->acceptedWorld<<" phase="<<unsigned(mode1->phase)
         <<" manager_seconds="<<mode1->manager.time30<<" lap_seconds="<<lapContinuity.timing.timeCB0
         <<" best_lap="<<lapContinuity.timing.bestLapCB4<<" fifo_count="<<mode1->events.count150
         <<" fifo_last_ticket="<<mode1->events.lastTicket<<" dax_ready="<<sourceAnnouncer->bank().ready
         <<" dax_last_type="<<last.request.type<<" dax_admitted="<<last.admitted
         <<" dax_started="<<last.started<<" dax_queued="<<last.queued
         <<" dax_pending="<<int(dax.pendingKey19C)<<" dax_episode_ac="<<dax.episodeAC
         <<" dax_delay_b0="<<dax.timerB0<<" dax_priority_b4="<<dax.priorityB4
         <<" music_song="<<raceMusic->currentSong()
         <<" music_starts="<<raceMusic->counters().starts<<" music_active="<<raceMusic->active()<<'\n';
    }
    if(dynamicContactEnabled){
     const auto& nativeFade=raceMusic->nativeFade();const auto& music=raceMusic->counters();
     std::cerr<<"collision music song="<<raceMusic->currentSong()<<" active="<<raceMusic->active()
         <<" native_fade_policy=linear_shared_mix phase="<<unsigned(nativeFade.phase)
         <<" gain="<<nativeFade.gain<<" admitted/updates/retired="<<music.nativeFadeAdmissions
         <<'/'<<music.nativeFadeUpdates<<'/'<<music.nativeFadeRetirements
         <<" source_iop_interpolation=unknown\n";
    }
    if(originalAiEnabled&&!allAiEnabled)reportOriginalAiFleet("status");
    if(raceEnabled){
     std::cerr<<"race phase="<<unsigned(race.phase)<<" countdown="<<withRace([&](auto& actualRace){return race_session50HM::countdownNumber(actualRace);})
         <<" seconds="<<race.raceSeconds<<" player_place="<<race.racers[0].place<<"/"<<race.racerCount
         <<" opponent_draws="<<rendererDiagnostic.opponentRacersDrawn
         <<" field_racers="<<race.racerCount<<" field_opponents="<<raceOpponentCount
         <<" sealed_finishes="<<race.finishCount
         <<" presentation_valid="<<acceptedRacePresentation.valid
         <<" opponents_submitted="<<acceptedRacePresentation.submittedOpponents
         <<" opponents_submitted_finished="<<acceptedRacePresentation.submittedFinishedOpponents
         <<" opponents_hidden_finished=0"
         <<" opponents_retained_stopped="<<acceptedRacePresentation.retainedStoppedOpponents
         <<" standings_rows="<<acceptedRacePresentation.standingsRows;
     for(std::size_t racer=0;racer<race.racerCount;++racer){const auto& r=race.racers[racer];
      std::cerr<<" racer"<<racer+1<<"["<<racerNames[racer]<<"]="<<r.place<<","<<r.observed.completedLaps<<","<<r.observed.checkpointCursor
          <<","<<r.observed.position[0]<<","<<r.observed.position[2]<<","<<(racer==0||racerBank.active(racer));
     }
     std::cerr<<'\n';
     std::size_t activeNative=0;
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      if(racerBank.active(racer))++activeNative;
     std::cerr<<"finish_continuation player_finished="<<race.racers[0].finished
         <<" player_cd8="<<unsigned(observedNativeFinishFlags[0])
         <<" player_manager_gate="<<unsigned(s.fields.applicationByte58)
         <<" finish_driver="<<humanResultsActive
         <<" finish_driver_ticks="<<finishDriverTicks
         <<" finish_manual_input_ticks="<<finishManualInputTicks
         <<" player_native_ticks="<<observedNativeTicks[0]
         <<" sealed_place="<<race.racers[0].finishPlace
         <<" sealed_time="<<race.racers[0].finishSeconds
         <<" sealed_laps="<<race.racers[0].observed.completedLaps
         <<" results_closed="<<race.resultsClosed
         <<" sealed_finish_count="<<race.finishCount
         <<" results_countdown="<<humanResultsCountdown
         <<" host_results_ready="<<humanResultsReady
         <<" active_native="<<activeNative
         <<" opponents_submitted="<<acceptedRacePresentation.submittedOpponents
         <<" standings_rows="<<acceptedRacePresentation.standingsRows;
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      std::cerr<<" racer"<<racer<<"_native_ticks="<<observedNativeTicks[racer]
          <<" racer"<<racer<<"_cd8="<<unsigned(observedNativeFinishFlags[racer])
          <<" racer"<<racer<<"_live_laps="<<race.racers[racer].live.completedLaps
          <<" racer"<<racer<<"_x="<<race.racers[racer].live.position[0]
          <<" racer"<<racer<<"_z="<<race.racers[racer].live.position[2];
     std::cerr<<'\n';
     reportRacerContacts();
    }
    lastStatusNs=afterWork;lastStatusRenderCount=submittedDrawFrames;
   }
   // Account for work already spent, requesting at most4ms active sleep
   // before polling again rather than sleeping a whole tick. Native/GL
   // work and OS scheduling can delay the next poll; no latency guarantee.
   const auto workNs=afterWork>=now?afterWork-now:0;
   if(plan.waitNanoseconds>workNs){
    phase_timing50GK::Scope sleepTiming(phaseTiming,phase_timing50GK::Phase::HostSleep);
    std::this_thread::sleep_for(std::chrono::nanoseconds(
        std::min(maximumSleepNs,plan.waitNanoseconds-workNs)));
   }
  }
 // A session that leaves the loop while still Racing reports what it recorded.
 if(raceFrames50AU.active)reportRaceFrames50AU(rr::platform::hostTrackCode50IM(selectedTrack),discardedHostNs);
}
} // namespace interactive_loop50DU
