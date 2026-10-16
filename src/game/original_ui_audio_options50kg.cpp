#include "original_ui_audio_options50kg.h"

namespace rr::game {
bool readOriginalUiAudioOptions50KG(const OriginalUiGameSettingsProfile50KG& profile,
    OriginalUiAudioOptions50KG& out) noexcept {
    if(!validOriginalUiGameSettingsProfile50KG(profile))return false;
    OriginalUiAudioOptions50KG next;
    next.soundMode1101C=profile.soundMode1101C; // Load1F65F0 / store1F6744.
    // Source1F6760/677C/6798/67B4; same fields consumed by race groups.
    next.volumes={static_cast<std::int32_t>(profile.volume11020[0]),
        static_cast<std::int32_t>(profile.volume11020[1]),
        static_cast<std::int32_t>(profile.volume11020[2]),
        static_cast<std::int32_t>(profile.volume11020[3])};
    out=next;return true;
}
}
