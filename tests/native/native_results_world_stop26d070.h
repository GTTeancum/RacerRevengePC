#pragma once
// A course world-sound owner whose own voices the Results 26D070 stop claims
// and stops. Original 26D070 stops every voice; the native Aural stop admits
// only voices a typed owner claims, so each course one-shot World4D8 sound
// owner (GB door PodSound, TC trigger Sounds, ...) implements this. The race
// root registers it cold with the AI audio owner, whose close() clears it.
#include <cstddef>

namespace rr::platform { class AudioMixer50HD; class RaceAudioGroups50IC; }

namespace native_results_world_stop26D070 {
class Owner {
public:
    // True when the owner is current on this mixer/groups; the claimed voices
    // (possibly none) are then exactly its own active World4D8 playbacks.
    virtual bool canStopResults26D070(const rr::platform::AudioMixer50HD&,
        const rr::platform::RaceAudioGroups50IC&)const noexcept=0;
    virtual std::size_t resultsStopCount26D070()const noexcept=0;
    virtual std::size_t resultsStopVoice26D070(std::size_t)const noexcept=0;
    // Stops PCM only; one-shot played state and source bookkeeping survive.
    virtual void stopResultsPreflighted26D070()noexcept=0;
protected:
    ~Owner()=default;
};
}
