#pragma once
#include "window_host50dq.h"

namespace rr::platform {
// Native client Results presentation input only. Visibility is supplied by
// the existing completed finish-countdown owner. This is not a source scene
// transition and has no simulation/audio pause or original event side effects.
class HostRaceResults50IL final {
public:
    enum class Action : std::uint8_t { None, Restart, MainMenu, Exit };
    std::uint32_t selectedAction() const noexcept { return selected_; }
    bool confirming() const noexcept { return confirming_; }
    bool confirmAccept() const noexcept { return confirmAccept_; }
    std::uint64_t revision() const noexcept { return revision_; }
    Action poll(const WindowInput50DQ& input, bool admitted) noexcept {
        if (!admitted) {
            visible_ = armed_ = false;
            if (confirming_) { confirming_ = confirmAccept_ = false; ++revision_; }
            return Action::None;
        }
        if (!visible_) { visible_ = true; armed_ = false; }
        // Require a fresh release after the panel appears, focus returns, or
        // Pause closes. An old boost/menu-confirm hold must not leave a race.
        if (!armed_) {
            constexpr std::uint16_t keys[] = {26,22,82,81,40,41,88,44};
            for (const auto key : keys)
                if (input.down[key] || input.pressed[key]) return Action::None;
            armed_ = true;
            return Action::None;
        }
        const bool up = input.pressed[82] || input.pressed[26];
        const bool down = input.pressed[81] || input.pressed[22];
        if (confirming_ && input.pressed[41]) {
            confirming_ = confirmAccept_ = false; ++revision_; return Action::None;
        }
        if (up != down) {
            if (confirming_) confirmAccept_ = !confirmAccept_;
            else selected_ = up ? (selected_ + 2u) % 3u : (selected_ + 1u) % 3u;
            ++revision_;
            return Action::None; // Never navigate and activate in one poll.
        }
        if (up && down) return Action::None;
        if (!input.pressed[40] && !input.pressed[88] && !input.pressed[44]) return Action::None;
        if (!confirming_) { confirming_ = true; confirmAccept_ = false; ++revision_; return Action::None; }
        if (!confirmAccept_) { confirming_ = false; ++revision_; return Action::None; }
        return selected_ == 0 ? Action::Restart : selected_ == 1 ? Action::MainMenu : Action::Exit;
    }
private:
    std::uint32_t selected_ = 0;
    std::uint64_t revision_ = 0;
    bool visible_ = false, armed_ = false, confirming_ = false, confirmAccept_ = false;
};
}
