#pragma once
// HOST next-session settings editor. The values and initial defaults are the
// admitted original configuration domain; five-point key steps are host UI.
// No mixer, callback, source config object, clock, or persistent file is edited.
#include "vehicle_window_gl50ds.h"
#include "../../../src/game/announcer_callbacks50ic.h"
#include <cstdio>

namespace rr::platform {
class HostAudioOptions50IK final {
public:
    HostAudioOptions50IK(const game::RaceAudioSettings50IC& loaded,
        const game::RaceAudioSettings50IC& selected) noexcept
        : loaded_(loaded), selected_(selected) {}

    bool valid() const noexcept {
        return game::announcer_callbacks50IC_detail::settings(loaded_) &&
            game::announcer_callbacks50IC_detail::settings(selected_);
    }
    const game::RaceAudioSettings50IC& selected() const noexcept { return selected_; }
    bool pending() const noexcept { return !equal(loaded_, selected_); }
    bool adjust(std::uint32_t row, bool increase) noexcept {
        if (!valid()) return false;
        auto* value = field(row);
        if (!value) return false;
        const auto next = increase ? (*value > 95 ? 100 : *value + 5) :
            (*value < 5 ? 0 : *value - 5);
        if (next == *value) return false;
        *value = next;
        return true;
    }
    bool restoreDefaults() noexcept {
        const auto defaults = game::freshOriginalAudioSettings50IC();
        if (equal(selected_, defaults)) return false;
        selected_ = defaults;
        return true;
    }
    // Returned view borrows these bounded row buffers until the next view or
    // destruction. The existing renderer consumes each view synchronously.
    void view(VehicleWindowHostMenu50DS& out) const noexcept {
        out.title = "AUDIO OPTIONS";
        out.subtitle = "NEXT START/RESTART ONLY. NOT SAVED TO DISK.";
        out.footer = "UP/DOWN SELECT  LEFT/RIGHT EDIT  ESC BACK";
        std::snprintf(rows_[0], sizeof rows_[0], "EFFECTS: %d / 100", int(selected_.effects11020));
        std::snprintf(rows_[1], sizeof rows_[1], "MUSIC: %d / 100", int(selected_.music11024));
        std::snprintf(rows_[2], sizeof rows_[2], "CHARACTER: %d / 100", int(selected_.character11028));
        std::snprintf(rows_[3], sizeof rows_[3], "DAX: %d / 100", int(selected_.announcer1102C));
        for (std::uint32_t i = 0; i < 4; ++i) out.rows[i] = rows_[i];
        out.rows[4] = "RESTORE ORIGINAL DEFAULTS";
        out.rows[5] = "BACK (KEEP NEXT SESSION SETTINGS)";
        out.rowCount = 6;
    }
private:
    static bool equal(const game::RaceAudioSettings50IC& a,
        const game::RaceAudioSettings50IC& b) noexcept {
        return a.effects11020 == b.effects11020 && a.music11024 == b.music11024 &&
            a.character11028 == b.character11028 && a.announcer1102C == b.announcer1102C;
    }
    std::int32_t* field(std::uint32_t row) noexcept {
        switch (row) {
            case 0: return &selected_.effects11020;
            case 1: return &selected_.music11024;
            case 2: return &selected_.character11028;
            case 3: return &selected_.announcer1102C;
            default: return nullptr;
        }
    }
    const game::RaceAudioSettings50IC loaded_;
    game::RaceAudioSettings50IC selected_;
    mutable char rows_[4][maxVehicleWindowMenuText50DS + 1]{};
};
} // namespace rr::platform
