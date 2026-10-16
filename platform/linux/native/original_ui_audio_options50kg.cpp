#include "original_ui_audio_options50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_sound_mode50kg.h"

namespace rr::platform {
bool readOriginalUiAppliedAudioOptions50KG(const game::OriginalUiGameSettingsProfile50KG& profile,
    const OriginalUiMainAudio50KG& audio,const OriginalSoundMode50KG& mode,
    game::OriginalUiAudioOptions50KG& out) noexcept {
    game::OriginalUiAudioOptions50KG next;
    if(!game::readOriginalUiAudioOptions50KG(profile,next)||
        audio.soundModeOwner50KG()!=&mode||!mode.ready()||
        !game::validOriginalSoundModeFields50KG(mode.fields())||
        static_cast<std::uint32_t>(mode.fields().profile1101C)!=next.soundMode1101C)return false;
    out=next;return true;
}
bool canAcceptOriginalUiAudioOptions50KG(const game::OriginalUiGameSettingsProfile50KG& profile,
    const OriginalUiMainAudio50KG& audio,const OriginalSoundMode50KG& mode,
    std::uint32_t requestedMode) noexcept {
    game::OriginalUiAudioOptions50KG current;
    return readOriginalUiAppliedAudioOptions50KG(profile,audio,mode,current)&&
        audio.canAcceptGameOptionsAudio50KG(requestedMode);
}
}
