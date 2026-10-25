#pragma once
#include "game_host50er.h"
#include <array>
#include "host_audio_options50ik.h"
#include "host_track_selection50im.h"
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/original_racer_selection50ij.h"

namespace rr::platform {
// HOST navigation only. It neither represents an original menu object nor
// changes source gameplay, live sound groups, or native event flags. Audio
// options are a separate next-session host request, not a live source setter.
// The session owner admits each result at a whole-world boundary.
class HostSessionMenu50IF final {
public:
    enum class Result : std::uint8_t { None, StartPrepared, Restart, Quit, ReturnOriginalMain };
    enum class Page : std::uint8_t { Closed, Main, Pause, Controls, Restart, Leave, Exit, Audio, Tracks, IncompleteGC };

    explicit HostSessionMenu50IF(bool startAtMenu, bool race, std::uint32_t loadedRacer60,
        bool racerSelectionAvailable = false, bool audioOptionsAvailable = false,
        const game::RaceAudioSettings50IC& loadedAudio = game::freshOriginalAudioSettings50IC(),
        const game::RaceAudioSettings50IC& selectedAudio = game::freshOriginalAudioSettings50IC(),
        HostTrack50IM loadedTrack = HostTrack50IM::TA, HostTrack50IM selectedTrack = HostTrack50IM::TA,
        const HostTrackAdmission50IM& trackAdmission = {},
        bool nativeAllAiRestart = false, bool returnOriginalMain = false, bool nativeHumanRace = false) noexcept
        : page_(startAtMenu ? Page::Main : Page::Closed),
          loadedRacer60_(loadedRacer60), selectedRacer60_(loadedRacer60),
          race_(race), started_(!startAtMenu), racerChoice_(race && racerSelectionAvailable),
          audioChoice_(audioOptionsAvailable), audioOptions_(loadedAudio, selectedAudio),
          loadedTrack_(loadedTrack), selectedTrack_(selectedTrack), trackAdmission_(trackAdmission),
          nativeAllAiRestart_(nativeAllAiRestart), returnOriginalMain_(nativeAllAiRestart&&returnOriginalMain),
          nativeHumanRace_(nativeAllAiRestart&&nativeHumanRace) {
        audioChoice_ = audioChoice_ && audioOptions_.valid();
        if (nativeAllAiRestart_ || loadedTrack_ == HostTrack50IM::GA) {
            // Whole-world ALL8 restarts keep the loaded track/racer/audio.
            // Original Main is a separate application destination, never the
            // old host track menu or a mutation of the live source race.
            racerChoice_ = audioChoice_ = false;
            selectedTrack_ = loadedTrack_;
            returnPage_ = Page::Pause;
            if (startAtMenu) page_ = Page::Pause;
        }
    }
    void setHumanControls(std::uint32_t preset,bool twinEngine,bool physical) noexcept {
        humanPreset_=preset;humanTwinEngine_=twinEngine;humanPhysical_=physical;++revision_;
    }
    bool open() const noexcept { return page_ != Page::Closed; }
    Page page() const noexcept { return page_; }
    std::uint64_t revision() const noexcept { return revision_; }
    std::uint32_t selectedRacer60() const noexcept { return selectedRacer60_; }
    const game::RaceAudioSettings50IC& selectedAudioSettings() const noexcept { return audioOptions_.selected(); }
    bool audioChangesPending() const noexcept { return audioChoice_ && audioOptions_.pending(); }
    HostTrack50IM selectedTrack() const noexcept { return selectedTrack_; }
    bool trackChangesPending() const noexcept { return selectedTrack_ != loadedTrack_; }

    // Observe every host poll, including inactive polls, so reconnect/focus
    // changes cannot carry a held pad button into a menu action. These edges
    // belong only to this native Pause owner: no gameplay packet is altered.
    void observeController(const WindowInput50DQ& input,bool enabled) noexcept {
        controllerEdges_={};
        const auto& pad=input.controllers[0];
        if(!enabled||!input.gameControllersEnabled||!pad.connected||pad.instanceId<0){
            controllerInstance_=-1;controllerArmed_=false;return;
        }
        const bool changed=controllerInstance_!=pad.instanceId;
        if(changed){controllerInstance_=pad.instanceId;controllerArmed_=false;}
        if(!input.focused||input.minimized||input.activityLost||
           input.eventBudgetExhausted||input.quitRequested){controllerArmed_=false;return;}
        // SDL mapped D-pad up/down/left/right, Cross(A), Triangle(Y), Start.
        // Triangle remains the original frontend Back position. The native
        // confirmation owner still defaults to Cancel and requires a new edge.
        constexpr unsigned buttons[]={11,12,13,14,0,3,6};
        bool neutral=true;
        for(auto button:buttons){
            if(pad.buttons[button]>1||pad.pressed[button]>1||pad.released[button]>1){
                controllerArmed_=false;return;
            }
            neutral=neutral&&!pad.buttons[button]&&!pad.pressed[button];
        }
        if(changed||!controllerArmed_){controllerArmed_=neutral;return;}
        for(unsigned i=0;i<controllerEdges_.size();++i)
            controllerEdges_[i]=pad.pressed[buttons[i]]!=0;
    }

    // Called only for a focused, fully drained input snapshot. Escape/Enter
    // and row movement use fresh edges, never held keys or auto-repeat.
    Result poll(const GameHostInput50ER& input, bool canResume) noexcept {
        // Merge into a menu-local copy, never input.keys or a driving sample.
        auto key = input.keys.pressed;
        constexpr unsigned scancodes[]={82,81,80,79,40,41};
        for(unsigned i=0;i<6;++i)if(controllerEdges_[i])key[scancodes[i]]=1;
        const bool controllerStart=controllerEdges_[6];
        controllerEdges_={}; // A menu edge is consumed at most once.
        const bool back = key[41] != 0;
        if (input.menuRequested) {
            if (nativeAllAiRestart_ || loadedTrack_ == HostTrack50IM::GA) { change(Page::Pause); return Result::None; }
            staleSession_ = started_;
            change(Page::Main);
            return Result::None;
        }
        if (page_ == Page::Closed) {
            if (back || input.pauseRequested || controllerStart) change(Page::Pause);
            return Result::None;
        }
        if (page_ == Page::Pause && (back || input.resumeRequested || controllerStart)) {
            if (canResume) change(Page::Closed);
            return Result::None;
        }
        if (back) {
            if (page_ == Page::Main) confirm(Page::Exit);
            else if (page_ == Page::Tracks) { returnPage_ = Page::Main; change(Page::Main); }
            else if (page_ == Page::IncompleteGC) { returnPage_ = Page::Main; change(Page::Tracks); }
            else change(returnPage_);
            return Result::None;
        }
        if (page_ == Page::Controls) {
            if (key[40] || key[88] || key[44]) change(returnPage_);
            return Result::None;
        }
        const bool up = key[82] || key[26], down = key[81] || key[22];
        if (page_ == Page::IncompleteGC) {
            // Five explanatory rows are not actions. Cancel is always first.
            if (up != down) { selected_ = selected_ == 5u ? 6u : 5u; ++revision_; return Result::None; }
            if (up && down) return Result::None;
            if (key[40] || key[88] || key[44]) {
                returnPage_ = Page::Main;
                if (selected_ == 6u && hostTrackAdmitted50IM(HostTrack50IM::GC, trackAdmission_)) {
                    selectedTrack_ = HostTrack50IM::GC; change(Page::Main);
                } else change(Page::Tracks);
            }
            return Result::None;
        }
        const std::uint32_t count = page_ == Page::Main ? 4u + unsigned(racerChoice_) + unsigned(audioChoice_) :
            page_ == Page::Pause ? (nativeAllAiRestart_ ? 4u+unsigned(returnOriginalMain_) :
                loadedTrack_ == HostTrack50IM::GA ? 3u : 5u + unsigned(audioChoice_)) : page_ == Page::Audio ? 6u :
            page_ == Page::Tracks ? 2u + unsigned(trackAdmission_.gcIncompleteWorld) : 2u;
        if (up != down) {
            selected_ = up ? (selected_ + count - 1u) % count : (selected_ + 1u) % count;
            ++revision_;
            // A direction and activation drained together never skips through
            // a destructive row before it has appeared on screen.
            return Result::None;
        }
        // Opposed navigation is not permission to activate the current row.
        // This covers mixed keyboard/pad edges as well as one device.
        if (up && down) return Result::None;
        if (page_ == Page::Audio) {
            const bool left = key[80] != 0, right = key[79] != 0;
            if (left || right) {
                if (left != right && audioOptions_.adjust(selected_, right)) ++revision_;
                return Result::None;
            }
            if (key[40] || key[88] || key[44]) {
                if (selected_ == 4 && audioOptions_.restoreDefaults()) ++revision_;
                else if (selected_ == 5) change(returnPage_);
            }
            return Result::None;
        }
        if (!key[40] && !key[88] && !key[44]) return Result::None;
        if (page_ == Page::Tracks) {
            if (selected_ == 0) { selectedTrack_ = HostTrack50IM::TA; change(Page::Main); }
            else if (trackAdmission_.gcIncompleteWorld && selected_ == 1) {
                if (hostTrackAdmitted50IM(HostTrack50IM::GC, trackAdmission_)) {
                    returnPage_ = Page::Tracks; change(Page::IncompleteGC); selected_ = 5;
                } else ++revision_; // Acknowledge unavailable choice without changing selection.
            } else change(Page::Main);
            return Result::None;
        }
        if (page_ == Page::Main) {
            if (selected_ == 0) {
                if (!hostTrackAdmitted50IM(selectedTrack_, trackAdmission_)) return Result::None;
                if (staleSession_ || selectedRacer60_ != loadedRacer60_ || audioChangesPending() || trackChangesPending()) return Result::Restart;
                started_ = true; change(Page::Closed); return Result::StartPrepared;
            }
            if (racerChoice_ && selected_ == 1) {
                namespace roster=rr::game::original_racer_selection50IJ;
                std::size_t current=roster::entries.size();
                for(std::size_t i=0;i<roster::entries.size();++i)
                    if(roster::entries[i].racer60==selectedRacer60_)current=i;
                if(current==roster::entries.size())return Result::None;
                // Skip explicitly incomplete choices; never rewrite the
                // loaded identity or any live pod while browsing the menu.
                for(std::size_t step=1;step<=roster::entries.size();++step){
                    const auto& candidate=roster::entries[(current+step)%roster::entries.size()];
                    if(candidate.humanJoinImplemented){selectedRacer60_=candidate.racer60;break;}
                }
                ++revision_;
                return Result::None;
            }
            if (selected_ == (racerChoice_ ? 2u : 1u)) { returnPage_ = Page::Main; change(Page::Controls); }
            else if (audioChoice_ && selected_ == (racerChoice_ ? 3u : 2u)) { returnPage_ = Page::Main; change(Page::Audio); }
            else if (selected_ == 2u + unsigned(racerChoice_) + unsigned(audioChoice_)) { returnPage_ = Page::Main; change(Page::Tracks); }
            else confirm(Page::Exit);
        } else if (page_ == Page::Pause) {
            if (selected_ == 0) { if (canResume) change(Page::Closed); }
            else if (nativeAllAiRestart_) {
                if (selected_ == 1) confirm(Page::Restart);
                else if (returnOriginalMain_ && selected_ == 2) confirm(Page::Leave);
                else if (selected_ == 2u+unsigned(returnOriginalMain_)) { returnPage_ = Page::Pause; change(Page::Controls); }
                else confirm(Page::Exit);
            }
            else if (loadedTrack_ == HostTrack50IM::GA) {
                if (selected_ == 1) { returnPage_ = Page::Pause; change(Page::Controls); }
                else confirm(Page::Exit);
            }
            else if (selected_ == 1) confirm(Page::Restart);
            else if (selected_ == 2) confirm(Page::Leave);
            else if (selected_ == 3) { returnPage_ = Page::Pause; change(Page::Controls); }
            else if (audioChoice_ && selected_ == 4) { returnPage_ = Page::Pause; change(Page::Audio); }
            else confirm(Page::Exit);
        } else if (selected_ == 0) change(returnPage_);
        else if (page_ == Page::Restart) return Result::Restart;
        else if (page_ == Page::Leave) {
            if(returnOriginalMain_)return Result::ReturnOriginalMain;
            staleSession_ = true; change(Page::Main);
        }
        else if (page_ == Page::Exit) return Result::Quit;
        return Result::None;
    }

    VehicleWindowHostMenu50DS view(bool canResume) const noexcept {
        VehicleWindowHostMenu50DS out{};
        out.title = "RACER REVENGE"; out.subtitle = "RACE SETUP";
        out.footer = humanPhysical_ ? "DPAD SELECT  A CONFIRM  Y BACK" : "UP/DOWN SELECT  ENTER CONFIRM  ESC BACK";
        out.retainRaceImage = page_ == Page::Pause ||
            (page_ != Page::Main && returnPage_ == Page::Pause);
        out.selectedRow = selected_;
        if (nativeAllAiRestart_&&!(nativeHumanRace_&&page_==Page::Controls)) {
            out.title = nativeHumanRace_?"PAUSED":"NATIVE ALL-AI RACE";
            out.subtitle = canResume ? (nativeHumanRace_?"PLAYER 1 AND SEVEN ORIGINAL AI":"ALL 8 RACERS USE ORIGINAL AI") : "SESSION STOPPED. RESTART TO CONTINUE.";
            if(page_==Page::Controls){
                out.title="ALL-AI RACE CONTROLS";
                out.rows[0]="C          CYCLE ORIGINAL CAMERA";
                out.rows[1]="ESC        PAUSE / RESUME";
                out.rows[2]="F5         RESTART RACE FROM GRID";
                out.rows[3]="UP / DOWN  MENU SELECTION; ENTER CONFIRMS";
                out.rows[4]="ALL 8 RACERS RETAIN ORIGINAL AI CONTROLS";
                out.rows[5]="CHECKPOINT RESTORE UNAVAILABLE";
                out.rows[6]="BACK";out.rowCount=7;out.selectedRow=6;
                out.footer="ENTER OR ESC RETURNS TO THE MENU";
            }else if(page_==Page::Restart||page_==Page::Leave||page_==Page::Exit){
                out.title=page_==Page::Restart?"RESTART RACE":page_==Page::Leave?"RETURN TO ORIGINAL MAIN":"EXIT GAME";
                out.subtitle="CURRENT RACE PROGRESS WILL BE LOST";
                out.rows[0]="CANCEL";
                out.rows[1]=page_==Page::Restart?"RESTART FROM GRID":page_==Page::Leave?"RETURN TO ORIGINAL MAIN":"EXIT";
                out.rowCount=2;
            }else{
                out.rows[0]=canResume?"RESUME":"RESUME UNAVAILABLE";
                out.rows[1]="RESTART RACE";
                std::uint32_t row=2;
                if(returnOriginalMain_)out.rows[row++]="ORIGINAL MAIN MENU";
                out.rows[row++]="CONTROLS";out.rows[row++]="EXIT";out.rowCount=row;
            }
            return out;
        }
        if (loadedTrack_ == HostTrack50IM::GA && !nativeHumanRace_) {
            out.title = "GA NATIVE PREVIEW";
            out.subtitle = canResume ? "INCOMPLETE TRACK. ALL 8 RACERS USE ORIGINAL AI." : "SESSION STOPPED. EXIT TO START FRESH.";
            if (page_ == Page::Controls) {
                out.title = "GA PREVIEW CONTROLS";
                out.rows[0] = "ALL 8 RACERS USE ORIGINAL AI";
                out.rows[1] = "CAMERA RACER 0 ALSO USES ORIGINAL AI";
                out.rows[2] = "ESC        PAUSE / RESUME";
                out.rows[3] = "UP / DOWN  MENU SELECTION; ENTER CONFIRMS";
                out.rows[4] = "F5 / RESTART AND BACKSPACE / CHECKPOINT DISABLED";
                out.rows[5] = "NO MANUAL DRIVING OR TRACK LAUNCH";
                out.rows[6] = "EXIT AND RELAUNCH FOR A FRESH PREVIEW";
                out.rows[7] = "BACK"; out.rowCount = 8; out.selectedRow = 7;
                out.footer = humanPhysical_ ? "A OR Y RETURNS TO THE MENU" : "ENTER OR ESC RETURNS TO THE MENU";
            } else if (page_ == Page::Exit) {
                out.title = "EXIT GA PREVIEW";
                out.subtitle = "CURRENT SESSION PROGRESS WILL BE LOST";
                out.rows[0] = "CANCEL"; out.rows[1] = "EXIT"; out.rowCount = 2;
            } else {
                out.rows[0] = canResume ? "RESUME" : "RESUME UNAVAILABLE";
                out.rows[1] = "CONTROLS"; out.rows[2] = "EXIT"; out.rowCount = 3;
            }
            return out;
        }
        if (page_ == Page::Main) {
            out.title = "RACER REVENGE";
            out.subtitle = staleSession_ ? (race_ ? "START A NEW RACE" : "START A NEW DRIVE") :
                race_ ? "RACE SETUP  8 RACERS  3 LAPS" : "FREE DRIVE";
            out.rows[0] = trackChangesPending() ? (race_ ? "START RACE (APPLY TRACK)" : "START DRIVE (APPLY TRACK)") :
                audioChangesPending() ? (race_ ? "START RACE (APPLY AUDIO)" : "START DRIVE (APPLY AUDIO)") :
                race_ ? "START RACE" : "START DRIVE";
            std::uint32_t row = 1;
            if (racerChoice_) {
                const auto* entry=rr::game::original_racer_selection50IJ::findByRacer60(selectedRacer60_);
                out.rows[row++] = entry&&entry->humanJoinImplemented?entry->menuLabel:"RACER UNAVAILABLE";
            }
            out.rows[row++] = "CONTROLS";
            if (audioChoice_) out.rows[row++] = "AUDIO OPTIONS (NEXT SESSION)";
            out.rows[row++] = selectedTrack_ == HostTrack50IM::GC ? "TRACK: GC EXPERIMENTAL INCOMPLETE" :
                selectedTrack_ == HostTrack50IM::GA ? "TRACK: GA CANDIDATE (NO MENU LAUNCH)" : "TRACK: TA";
            out.rows[row++] = "EXIT"; out.rowCount = row;
        } else if (page_ == Page::Pause) {
            out.title = "PAUSED";
            out.subtitle = canResume ? (race_ ? "RACE PAUSED" : "DRIVE PAUSED") : "RACE STOPPED. RESTART TO CONTINUE.";
            out.rows[0] = canResume ? (audioChangesPending() ? "RESUME (CURRENT AUDIO)" : "RESUME") : "RESUME UNAVAILABLE";
            out.rows[1] = audioChangesPending() ? (race_ ? "RESTART RACE (APPLY AUDIO)" : "RESTART DRIVE (APPLY AUDIO)") :
                race_ ? "RESTART RACE" : "RESTART DRIVE";
            out.rows[2] = "MAIN MENU"; out.rows[3] = "CONTROLS";
            if (audioChoice_) out.rows[4] = "AUDIO OPTIONS (NEXT SESSION)";
            out.rows[audioChoice_ ? 5 : 4] = "EXIT";
            out.rowCount = audioChoice_ ? 6 : 5;
        } else if (page_ == Page::Audio) {
            audioOptions_.view(out);
        } else if (page_ == Page::Tracks) {
            out.title = "TRACK SELECTION";
            out.subtitle = trackAdmission_.gcIncompleteWorld ? "GC IS EXPERIMENTAL AND INCOMPLETE" : "AVAILABLE TRACK";
            out.rows[0] = selectedTrack_ == HostTrack50IM::TA ? "TA (SELECTED)" : "TA";
            if (trackAdmission_.gcIncompleteWorld) {
                out.rows[1] = !trackAdmission_.gcMode2 ? "GC UNAVAILABLE: REQUIRES MODE 2" :
                    selectedTrack_ == HostTrack50IM::GC ? "GC EXPERIMENTAL INCOMPLETE (SELECTED)" : "GC EXPERIMENTAL INCOMPLETE";
                out.rows[2] = "BACK"; out.rowCount = 3;
            } else { out.rows[1] = "BACK"; out.rowCount = 2; }
        } else if (page_ == Page::IncompleteGC) {
            out.title = "GC EXPERIMENTAL INCOMPLETE";
            out.subtitle = "MODE 2 ONLY. THESE PARTS ARE MISSING:";
            out.rows[0] = "GATE / LOSTAI TRIGGER DETECTION";
            out.rows[1] = "OPENING TRIGGER ACTIVATION";
            out.rows[2] = "AUTHORED LOSTAI BRANCH ACTIONS";
            out.rows[3] = "GATE SOUND";
            out.rows[4] = "MODE1 RECORD CONFIGURATION";
            out.rows[5] = "CANCEL"; out.rows[6] = "SELECT GC FOR NEXT SESSION"; out.rowCount = 7;
        } else if (page_ == Page::Controls) {
            out.title = "CONTROLS";
            out.subtitle = humanPhysical_ ? "SDL GAMEPAD  SELECTED ORIGINAL LAYOUT" :
                humanTwinEngine_ ? "KEYBOARD  TWIN ENGINE CONTROLS" : "KEYBOARD  NORMAL CONTROLS";
            if(humanPhysical_){
                const auto layout=humanPreset_%4u;
                out.rows[0]=humanTwinEngine_?"LEFT STICK Y   LEFT THROTTLE / BRAKE":
                    layout==1?"CIRCLE / B     THROTTLE":"CROSS / A      THROTTLE";
                out.rows[1]=humanTwinEngine_?"RIGHT STICK Y  RIGHT THROTTLE / BRAKE":
                    layout==1?"TRIANGLE / Y   BRAKE":layout==2?"CIRCLE / B     BRAKE":"SQUARE / X     BRAKE";
                out.rows[2]=humanTwinEngine_?"UNEQUAL ENGINE INPUT STEERS":"LEFT STICK X   STEERING";
                out.rows[3]=(layout==1||layout==3)?"R1 / RIGHT SHOULDER  BOOST":"R2 / RIGHT TRIGGER   BOOST";
                out.rows[4]=layout==1?"L1 / LEFT SHOULDER   REPAIR":layout==3?
                    "R2 / RIGHT TRIGGER   REPAIR":"L2 / LEFT TRIGGER    REPAIR";
                out.rows[5]=layout==1?"R2 / RIGHT TRIGGER   POWERSLIDE":layout==3?
                    "L2 / LEFT TRIGGER    POWERSLIDE":"R1 / RIGHT SHOULDER  POWERSLIDE";
            }else{
                out.rows[0]=humanTwinEngine_?"W / S      LEFT THROTTLE / BRAKE":"W / S      THROTTLE / BRAKE";
                out.rows[1]=humanTwinEngine_?"UP / DOWN  RIGHT THROTTLE / BRAKE":"LEFT / RIGHT  STEERING";
                out.rows[2]=humanTwinEngine_?"UNEQUAL ENGINE INPUT STEERS":"UP / DOWN  ORIGINAL LEFT STICK Y";
                out.rows[3]="SPACE      BOOST";out.rows[4]="R          HOLD TO REPAIR";
                out.rows[5]="LEFT SHIFT POWERSLIDE";
            }
            out.rows[6] = nativeHumanRace_ ? "C          CYCLE ORIGINAL CAMERA" :
                    race_ ? "BACKSPACE  RESTART ENTIRE RACE" : "BACKSPACE  RETURN TO CHECKPOINT";
            if(humanPhysical_)out.rows[6]=nativeHumanRace_?(humanPreset_%4u==1u?
                "SQUARE / X     CYCLE CAMERA":"TRIANGLE / Y   CYCLE CAMERA"):"OTHER ORIGINAL PAD ACTIONS UNJOINED";
            out.rows[7] = "F5         RESTART FROM GRID";
            out.rows[8] = humanPhysical_?"START / ESC  NATIVE PAUSE / RESUME":"ESC        PAUSE / RESUME";
            out.rows[9] = "BACK"; out.rowCount = 10; out.selectedRow = 9;
            out.footer = humanPhysical_ ? "A OR Y RETURNS TO THE MENU" : "ENTER OR ESC RETURNS TO THE MENU";
        } else {
            out.title = page_ == Page::Restart ? "RESTART SESSION" : page_ == Page::Leave ? "RETURN TO MAIN MENU" : "EXIT GAME";
            out.subtitle = started_ && !staleSession_ ? "CURRENT SESSION PROGRESS WILL BE LOST" : "CONFIRM YOUR CHOICE";
            out.rows[0] = "CANCEL";
            out.rows[1] = page_ == Page::Restart ? "RESTART FROM GRID" : page_ == Page::Leave ? "RETURN TO MAIN MENU" : "EXIT";
            out.rowCount = 2;
        }
        return out;
    }
private:
    void change(Page page) noexcept { page_ = page; selected_ = 0; ++revision_; }
    void confirm(Page page) noexcept { returnPage_ = page_; change(page); }
    Page page_ = Page::Closed, returnPage_ = Page::Main;
    std::uint32_t selected_ = 0, loadedRacer60_ = 1, selectedRacer60_ = 1;
    std::uint64_t revision_ = 1;
    bool race_ = true, started_ = false, staleSession_ = false, racerChoice_ = false;
    bool audioChoice_ = false;
    HostAudioOptions50IK audioOptions_;
    HostTrack50IM loadedTrack_ = HostTrack50IM::TA, selectedTrack_ = HostTrack50IM::TA;
    HostTrackAdmission50IM trackAdmission_{};
    bool nativeAllAiRestart_=false,returnOriginalMain_=false,nativeHumanRace_=false;
    std::uint32_t humanPreset_=4;
    bool humanTwinEngine_=true,humanPhysical_=false;
    std::array<bool,7> controllerEdges_{};
    std::int32_t controllerInstance_=-1;
    bool controllerArmed_=false;
};
}
