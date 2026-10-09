#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Stable authored identities. A cue is not a decoded sample or a playback claim.
enum class AudioBank50HE : std::uint8_t { PodEvent, PodEngine, PodCurses,
    PodTaunts, PodSpecial, PodDebris, PodWreck, TrackIntro };
struct AudioCue50HE { AudioBank50HE bank=AudioBank50HE::PodEvent; std::uint32_t key=0; };
enum class AudioOperation50HE : std::uint8_t { Start, Stop, Controls };
struct AudioCommand50HE {
    AudioOperation50HE operation=AudioOperation50HE::Start;
    AudioCue50HE cue{};
    float gain=1.f,pitch=1.f;
    bool loop=false;
    // Static provenance only. Never dispatch through this value.
    std::uint32_t sourceSite=0;
};
enum class AudioEventStatus50HE : std::uint8_t { Complete, Input, Capacity };

struct PodWarningState50HE { std::uint8_t flags4A4=0; };
struct PodWarningInput50HE {
    // Source2234E0 deliberately does not consume7D8.
    float damage7D0=0.f,damage7D4=0.f,damage7DC=0.f;
    float speedRatio70=0.f;
    std::uint32_t vehicle64=0,probe118=0;
    std::uint8_t flagsCD8=0,flags10CC=0,emitter22C=0,manager1B0=0;
    // Explicit partial subscriber scope when current native probe118 is absent.
    // False preserves spraybit and emits no spraystart/control;CD8stillstopsall.
    bool sprayInputKnown=true;
};
// Source19EF24..19F30C. Up to5 ordered commands: damage, heat, wrong-way,
// spray transitions followed by spray volume. CD8&1 stops all four in that order.
// No hot allocation. On error state/output/count remain unchanged. Disjoint
// caller storage; previous/output may alias exactly. Execute commands in order
// before the next update; generated commands do not acknowledge playback.
AudioEventStatus50HE updatePodWarnings50HE(const PodWarningInput50HE&,
    const PodWarningState50HE&,PodWarningState50HE&,AudioCommand50HE*,
    std::size_t capacity,std::size_t& count) noexcept;

struct PodEngineParameters50HE {
    float idlePitch=.32f,pitchRange=4.f,idleBinder=1.f,binderRange=1.f;
    float engineVolume=100.f,binderVolume=90.f;
    // Actual sound metadata50, returned by a genuine provider. Native backend
    // may choose normalized units4096, explicitly as host pitch representation.
    std::uint32_t engineBasePitch=4096,binderBasePitch=4096;
};
struct PodEngineInput50HE {
    float speedRatio70=0.f;
    // Current native dip owner484, constructor/reset1. Supplying1 explicitly
    // admits the undamaged baseline; it does not implement damage modulation.
    float enginePitchScale484=1.f;
    bool boostPitch=false;
};
struct PodEngineControls50HE {
    float engineGain=0.f,binderGain=0.f,enginePitch=1.f,binderPitch=1.f;
    std::uint32_t enginePitchWord=0,binderPitchWord=0;
};
// Source178FE0 /19F5D4..19F6B0 /19FB88, with13BAD0 polynomial,19F5FC scalar sqrt(abs(sine)), and integer
// pitch truncation. Uses retained SPEED ratio, not held throttle. Includes1.15
// boost pitch. Gain is original start method's0.45*authored volume/100.
// Does not claim the unjoined randomized damage-dip owner19F6CC..19FB84.
// Finite binary32, nearest rounding, no contraction; ratio restricted to[-2,2]
// (source polynomial's single-reflection range). Output unchanged on failure.
bool podEngineControls50HE(const PodEngineParameters50HE&,const PodEngineInput50HE&,
    PodEngineControls50HE&) noexcept;

struct PodBoostAudioState50HE { bool pitchBoost=false; };
// Source217760/2177BC ->19FF10/19FF60 ->1A0120. Call at the original boost
// boundary: heldBoost=(B9C&1)&&!(B9C&2), heating is the source88*(70+74),
// previous64 is the PREVIOUS state, before heat can make this tick state2.
// A new boost starts PodEngine key3(boost.vag); release clears pitch only,
// because19FF60 does not stop the clip. No synthetic boostmax event here.
AudioEventStatus50HE updatePodBoostAudio50HE(bool heldBoost,float heating,
    std::uint32_t previous64,const PodBoostAudioState50HE&,PodBoostAudioState50HE&,
    AudioCommand50HE*,std::size_t capacity,std::size_t& count) noexcept;
// Sound tail of actual player repair override1A23F0, AFTER base224370.
// A repair tick with all damage<=0,B9C&2,vehicle3C==0 dispatches Event2
// (authored boostmax.vag). No invented repair loop or damage-edge debounce.
AudioEventStatus50HE repairCompletionAudio50HE(std::uint8_t flagsB9C,
    std::uint32_t vehicle3C,const float (&damage7D0)[4],AudioCommand50HE*,
    std::size_t capacity,std::size_t& count) noexcept;
}
