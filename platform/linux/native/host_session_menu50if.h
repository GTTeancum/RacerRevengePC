#pragma once
#include "game_host50er.h"
#include "vehicle_window_gl50ds.h"

namespace rr::platform {
// HOST navigation only. It neither represents an original menu object nor
// changes source gameplay, sound groups, configuration, or native event flags.
// The session owner admits each result at a whole-world boundary.
class HostSessionMenu50IF final {
public:
    enum class Result : std::uint8_t { None, StartPrepared, Restart, Quit };
    enum class Page : std::uint8_t { Closed, Main, Pause, Controls, Restart, Leave, Exit };

    explicit HostSessionMenu50IF(bool startAtMenu, bool race, std::uint32_t loadedRacer60,
        bool racerSelectionAvailable = false) noexcept
        : page_(startAtMenu ? Page::Main : Page::Closed),
          loadedRacer60_(loadedRacer60), selectedRacer60_(loadedRacer60),
          race_(race), started_(!startAtMenu), racerChoice_(race && racerSelectionAvailable) {}
    bool open() const noexcept { return page_ != Page::Closed; }
    Page page() const noexcept { return page_; }
    std::uint64_t revision() const noexcept { return revision_; }
    std::uint32_t selectedRacer60() const noexcept { return selectedRacer60_; }

    // Called only for a focused, fully drained input snapshot. Escape/Enter
    // and row movement use fresh edges, never held keys or auto-repeat.
    Result poll(const GameHostInput50ER& input, bool canResume) noexcept {
        const auto& key = input.keys.pressed;
        const bool back = key[41] != 0;
        if (input.menuRequested) {
            staleSession_ = started_;
            change(Page::Main);
            return Result::None;
        }
        if (page_ == Page::Closed) {
            if (back || input.pauseRequested) change(Page::Pause);
            return Result::None;
        }
        if (page_ == Page::Pause && (back || input.resumeRequested)) {
            if (canResume) change(Page::Closed);
            return Result::None;
        }
        if (back) {
            if (page_ == Page::Main) confirm(Page::Exit);
            else change(returnPage_);
            return Result::None;
        }
        if (page_ == Page::Controls) {
            if (key[40] || key[88] || key[44]) change(returnPage_);
            return Result::None;
        }
        const bool up = key[82] || key[26], down = key[81] || key[22];
        const std::uint32_t count = page_ == Page::Main ? (racerChoice_ ? 4u : 3u) : page_ == Page::Pause ? 5u : 2u;
        if (up != down) {
            selected_ = up ? (selected_ + count - 1u) % count : (selected_ + 1u) % count;
            ++revision_;
            // A direction and activation drained together never skips through
            // a destructive row before it has appeared on screen.
            return Result::None;
        }
        if (!key[40] && !key[88] && !key[44]) return Result::None;
        if (page_ == Page::Main) {
            if (selected_ == 0) {
                if (staleSession_ || selectedRacer60_ != loadedRacer60_) return Result::Restart;
                started_ = true; change(Page::Closed); return Result::StartPrepared;
            }
            if (racerChoice_ && selected_ == 1) {
                selectedRacer60_ = selectedRacer60_ == 1u ? 3u : 1u;
                ++revision_;
                return Result::None;
            }
            if (selected_ == (racerChoice_ ? 2u : 1u)) { returnPage_ = Page::Main; change(Page::Controls); }
            else confirm(Page::Exit);
        } else if (page_ == Page::Pause) {
            if (selected_ == 0) { if (canResume) change(Page::Closed); }
            else if (selected_ == 1) confirm(Page::Restart);
            else if (selected_ == 2) confirm(Page::Leave);
            else if (selected_ == 3) { returnPage_ = Page::Pause; change(Page::Controls); }
            else confirm(Page::Exit);
        } else if (selected_ == 0) change(returnPage_);
        else if (page_ == Page::Restart) return Result::Restart;
        else if (page_ == Page::Leave) { staleSession_ = true; change(Page::Main); }
        else if (page_ == Page::Exit) return Result::Quit;
        return Result::None;
    }

    VehicleWindowHostMenu50DS view(bool canResume) const noexcept {
        VehicleWindowHostMenu50DS out{};
        out.title = "RACER REVENGE"; out.subtitle = "RACE SETUP";
        out.footer = "UP/DOWN SELECT  ENTER CONFIRM  ESC BACK";
        out.retainRaceImage = page_ == Page::Pause ||
            (page_ != Page::Main && returnPage_ == Page::Pause);
        out.selectedRow = selected_;
        if (page_ == Page::Main) {
            out.title = "RACER REVENGE";
            out.subtitle = staleSession_ ? (race_ ? "START A NEW RACE" : "START A NEW DRIVE") :
                race_ ? "RACE SETUP  8 RACERS  3 LAPS" : "FREE DRIVE";
            out.rows[0] = race_ ? "START RACE" : "START DRIVE";
            if (racerChoice_) {
                out.rows[1] = selectedRacer60_ == 3u ? "RACER: CLEGG" : "RACER: ANAKIN SKYWALKER";
                out.rows[2] = "CONTROLS"; out.rows[3] = "EXIT"; out.rowCount = 4;
            } else {
                out.rows[1] = "CONTROLS"; out.rows[2] = "EXIT"; out.rowCount = 3;
            }
        } else if (page_ == Page::Pause) {
            out.title = "PAUSED";
            out.subtitle = canResume ? (race_ ? "RACE PAUSED" : "DRIVE PAUSED") : "RACE STOPPED. RESTART TO CONTINUE.";
            out.rows[0] = canResume ? "RESUME" : "RESUME UNAVAILABLE";
            out.rows[1] = race_ ? "RESTART RACE" : "RESTART DRIVE";
            out.rows[2] = "MAIN MENU"; out.rows[3] = "CONTROLS"; out.rows[4] = "EXIT";
            out.rowCount = 5;
        } else if (page_ == Page::Controls) {
            out.title = "CONTROLS"; out.subtitle = "KEYBOARD  TWIN ENGINE CONTROLS";
            out.rows[0] = "W / S      LEFT THROTTLE / BRAKE";
            out.rows[1] = "UP / DOWN  RIGHT THROTTLE / BRAKE";
            out.rows[2] = "UNEQUAL ENGINE INPUT STEERS";
            out.rows[3] = "SPACE      BOOST";
            out.rows[4] = "R          HOLD TO REPAIR";
            out.rows[5] = "LEFT SHIFT POWERSLIDE";
            out.rows[6] = race_ ? "BACKSPACE  RESTART ENTIRE RACE" : "BACKSPACE  RETURN TO CHECKPOINT";
            out.rows[7] = "F5         RESTART FROM GRID";
            out.rows[8] = "ESC        PAUSE / RESUME";
            out.rows[9] = "BACK"; out.rowCount = 10; out.selectedRow = 9;
            out.footer = "ENTER OR ESC RETURNS TO THE MENU";
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
};
}
