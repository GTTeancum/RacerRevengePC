#pragma once
#include "gameplay_audio50he.h"
#include "undertrack_reposition50et.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct PodSpeechCounts50HG {
    std::uint8_t lowDamage=0,wallHit=0,passed=0,attacked=0,passing=0,attacking=0;
    std::uint8_t death=0,win=0,lose=0;
};
struct PodSpeechState50HG {
    // Original21D..222: one rotating cursor for each curse/taunt category.
    std::uint8_t cursors[6]{};
    float cooldown224=5.f; // Original2C1088 constructor value.
    bool initialized=false;
};
enum class PodSpeechCategory50HG : std::uint8_t {
    LowDamage,WallHit,Passed,Attacked,Passing,Attacking,Death,Win,Lose
};
struct SpeechPosition50HG {float x=0.f,z=0.f;};
struct SpeechListeners50HG {const SpeechPosition50HG* positions=nullptr;std::size_t count=0;};
enum class PodSpeechStatus50HG : std::uint8_t {Complete,Input,Capacity,Uninitialized,RandomRange};
struct PodSpeechReceipt50HG {
    PodSpeechStatus50HG status=PodSpeechStatus50HG::Complete;
    bool audible=false,emitted=false;
    std::uint32_t randomDraws=0,sourceSite=0;
};
// Exact count domains of the eight retained originals. Authenticate selected counts cold;
// this function does not silently replace absent authoring with defaultcounts.
// Source2C1170 initializes SIX rotating counters with shared308030 draws in
// lowdamage/wall/passed/attacked/passing/attacking order. No private seed/RNG.
// Atomically updates state and caller's same raw RNG used by physics/effects.
PodSpeechStatus50HG initializePodSpeech50HG(const PodSpeechCounts50HG&,
    std::uint32_t& sharedRawRandomState,PodSpeechState50HG&) noexcept;
// Source1A1590 positive109C countdown, plus2C20C0 positive224 countdown.
// Call once per source frame BEFORE input/repair, with actual factor. 109C is
// already owned by the existing reset prefix as rawfloat bits; do not duplicate
// it in the audio owner. Both countdowns deliberately may cross belowzero.
bool advanceSpeechTimers50HG(float factor,UndertrackPlayerPrefix50ET&,
    PodSpeechState50HG&) noexcept;
// Original repair override1A2404..1A244C, AFTER224370. Uses source max of
// 7D0/7D4/7DC; if<0.7, clears ONLY10CC bit2 and sets109C=3seconds.
// This does not invent a repair sound;50HE separately implements its soundtail.
bool afterRepairSpeechState50HG(float damage7D0,float damage7D4,float damage7DC,
    UndertrackPlayerPrefix50ET&) noexcept;
// A genuine category request (from its game-event owner). Source2C1A90/2C1C00
// rotates curse/tauntcursors only after audibility and5-second cooldown pass;
// 2C1CE0 specials ignore cooldown and draw sharedRNG once per audible request.
// Source2C2050/104140/102950 audibility uses nearest XZ listener distance<200,
// with at most2 actual listeners in this admitted native domain. No timer or
// inputkey is a request to invent a category. At mostone command, noallocation.
// Failures leave state/RNG/commands/count unchanged. PCM availability does not
// alter source selection/cooldown; the genuine provider handles missingbytes.
PodSpeechReceipt50HG selectPodSpeech50HG(PodSpeechCategory50HG,
    const PodSpeechCounts50HG&,const SpeechPosition50HG&,const SpeechListeners50HG&,
    std::uint32_t& sharedRawRandomState,PodSpeechState50HG&,AudioCommand50HE*,
    std::size_t capacity,std::size_t& count) noexcept;

struct DamageSpeechInput50HG {
    float damage7D0=0.f,damage7D4=0.f,damage7DC=0.f;
    std::uint32_t vehicle3C=0,vehicle40=0,vehicle58=0,vehicle60=0xffffffffu;
    bool vehicle60Known=false;
    std::uint8_t config58=2,manager1B0=0;
    float playerSpeechChance138=.5f; // Constructor only; actual SPCHPCT.INI overrides with.35.
    SpeechPosition50HG position{};
    SpeechListeners50HG listeners{};
};
struct DamageSpeechReceipt50HG {
    PodSpeechStatus50HG status=PodSpeechStatus50HG::Complete;
    bool eventProduced=false,eventAccepted=false,eventDispatched=false,playerChancePassed=false;
    bool voiceEmitted=false;
    // Original fallback2C2C20(type12,racer60,variant). Its announcer bank/event
    // owner remains unjoined; expose the real request rather than inventspeech.
    bool announcerRequested=false,announcerArgumentKnown=false;
    std::uint32_t announcerType=0,announcerRacer=0,announcerVariant=0,randomDraws=0;
};
// One caller-owned queue slot for the actual player's event5. The original
// payload retains the player identity and copies its repeated-bit value. The
// caller supplies LIVE vehicle fields and listener positions at later dispatch.
struct DamageSpeechEvent50HG {bool pending=false,repeated=false;};
// Queue boundary1A2300/1A64B0: no speech selection and NO RNG draws here.
DamageSpeechReceipt50HG produceDamageSpeechEvent50HG(const DamageSpeechInput50HG&,
    UndertrackPlayerPrefix50ET&,DamageSpeechEvent50HG&) noexcept;
// Later manager event-flush boundary1A4C90/1A52B0. Dispatch in original queued
// order, after prior native effects/physics have consumed their shared RNG.
// Capacity/input failure keeps the event pending and commits no RNG/selection.
// The host joins an explicit post-frame subscriber phase, not a recovered
// full original manager scheduler. Never silently consume a pending event twice.
DamageSpeechReceipt50HG dispatchDamageSpeechEvent50HG(const DamageSpeechInput50HG&,
    const PodSpeechCounts50HG&,DamageSpeechEvent50HG&,std::uint32_t& sharedRawRandomState,
    PodSpeechState50HG&,AudioCommand50HE*,std::size_t capacity,std::size_t& count) noexcept;

// Source1A2300 event5 prefix +1A64B0's actor3C gate +1A4C9C manager gate
// +1A52B0 dispatch. Invoke at player's effectswrapper boundary AFTERrepair and
// before base223540. Source>0.7 and109C<=0 emits a real damageevent, stores10sec
// and repeatbit2, then player40/58 and .5chance choose actuallowdamage speech.
// Convenience only when queue/dispatch are adjacent; live integrations use the
// two separate entry points above to preserve shared RNG scheduling.
// This is a bounded speech subscriber; it does not acknowledge the entire
// original eventmanager or silently complete a requested announcerfallback.
DamageSpeechReceipt50HG damageSpeech50HG(const DamageSpeechInput50HG&,
    const PodSpeechCounts50HG&,UndertrackPlayerPrefix50ET&,
    std::uint32_t& sharedRawRandomState,PodSpeechState50HG&,
    AudioCommand50HE*,std::size_t capacity,std::size_t& count) noexcept;
struct TrackIntroLine50HG {std::int8_t eventKey=-1;std::uint8_t span=0;};
// Source175910..1759D4, called ONLY when the actual course-intro camera owner
// reaches its authored camera transition. This does not create that owner or
// treat a normal gameplay camera-mode change/startup as a voice event.
// The camera number is1-based; flags62 preserves unrelated bits. Signed4-bit
// span semantics match the original extraction, including values8..15.
AudioEventStatus50HE trackIntroSpeech50HG(std::uint32_t config34,
    std::uint32_t currentIntroCamera,const TrackIntroLine50HG* lines,std::size_t lineCount,
    std::uint8_t& flags62,AudioCommand50HE*,std::size_t capacity,std::size_t& count) noexcept;

}
