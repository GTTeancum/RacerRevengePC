#pragma once
#include "../../../src/game/original_ui_audio_options50kg.h"

namespace rr::platform {
class OriginalUiMainAudio50KG;
class OriginalSoundMode50KG;
// Main-thread-only read of the committed profile AND its actual frontend
// routing owner. This is not persistence or proof that race groups applied it.
bool readOriginalUiAppliedAudioOptions50KG(const game::OriginalUiGameSettingsProfile50KG&,
    const OriginalUiMainAudio50KG&,const OriginalSoundMode50KG&,
    game::OriginalUiAudioOptions50KG& out) noexcept;
// Original Accept must have room for188740 followed by SELECT1, with no
// intervening audio commands. Route/profile admission remains session-owned.
bool canAcceptOriginalUiAudioOptions50KG(const game::OriginalUiGameSettingsProfile50KG&,
    const OriginalUiMainAudio50KG&,const OriginalSoundMode50KG&,
    std::uint32_t requestedMode) noexcept;
}
