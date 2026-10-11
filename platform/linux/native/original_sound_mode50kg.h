#pragma once
#include "audio_mixer50hd.h"
#include "../../../src/game/original_sound_mode50kg.h"
#include <cstdint>
#include <limits>

namespace rr::platform {
// One stable, shared application owner. Cold bind the SAME owner to each
// actual frontend/race mixer; mixing/controls and edits use one producer thread.
// This owner never resets, steals or retunes another provider's voices/groups.
class OriginalSoundMode50KG final {
public:
    OriginalSoundMode50KG() noexcept=default;
    OriginalSoundMode50KG(const OriginalSoundMode50KG&)=delete;
    OriginalSoundMode50KG& operator=(const OriginalSoundMode50KG&)=delete;
    OriginalSoundMode50KG(OriginalSoundMode50KG&&)=delete;
    OriginalSoundMode50KG& operator=(OriginalSoundMode50KG&&)=delete;
    // Explicit imported/created profile value, never an implicit save fallback.
    // This is initial mode installation, not an Accept188740 receipt.
    bool initializeFromProfile1101C(std::uint32_t mode) noexcept {
        game::OriginalSoundModeFields50KG next;
        if(initialized_||!game::projectOriginalSoundMode18874050KG(mode,next))return false;
        fields_=next;initialized_=true;revision_=1;return true;
    }
    bool ready() const noexcept {return initialized_;}
    const game::OriginalSoundModeFields50KG& fields() const noexcept {return fields_;}
    std::uint64_t revision() const noexcept {return revision_;}
    // Requires the concrete installed PCM consumer. Mere profile/flag storage
    // cannot satisfy this check. Caller verifies this is its ACTUAL live mixer.
    bool canApply188740(const AudioMixer50HD& mixer,std::uint32_t mode) const noexcept {
        return initialized_&&mode<=2&&revision_!=std::numeric_limits<std::uint64_t>::max()&&
            mixer.count()!=0&&mixer.originalSoundMode50KG()==this;
    }
    // Preflight the full profile/route transaction before this single-threaded
    // commit. Mode setters preserve current voice gain, playback coordinates,
    // group gains and device queues; subsequent source controls consume it.
    bool apply188740(AudioMixer50HD& mixer,std::uint32_t mode) noexcept {
        game::OriginalSoundModeFields50KG next;
        if(!canApply188740(mixer,mode)||!game::projectOriginalSoundMode18874050KG(mode,next))return false;
        fields_=next;++revision_;return true;
    }
private:
    game::OriginalSoundModeFields50KG fields_{};
    std::uint64_t revision_=0;
    bool initialized_=false;
};
} // namespace rr::platform
