#pragma once
#include "original_ui_game_options_controller50kg.h"
#include "announcer_callbacks50ic.h"

namespace rr::game {
// Audio belongs to authored options.dat, screen12/page4, groups2..6.
// This projection has no input, widgets, default-profile creation or save path.
struct OriginalUiAudioOptions50KG {
    std::uint32_t soundMode1101C=0;
    RaceAudioSettings50IC volumes{};
};
// Read only committed application fields, never the page's local edit buffer.
// Failure leaves out unchanged. No category gain or ducking is applied here.
bool readOriginalUiAudioOptions50KG(const OriginalUiGameSettingsProfile50KG&,
    OriginalUiAudioOptions50KG& out) noexcept;
}
