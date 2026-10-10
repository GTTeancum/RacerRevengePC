#pragma once
#include <cstdint>

namespace rr::game {
// Caller-owned projection of the REAL player and current game382B80->2DC.
// These are field names, not a guest memory overlay. Synchronize actual helper
// mutations before advancing; helpers may change fields read later in this call.
struct LapCompletionPlayer50GG {
    float timeCB0=0.0f,bestLapCB4=0.0f;
    const float* lapTimesCC0=nullptr;
    std::uint32_t lapTimeCount=0,lapCC8=0,word7C=0;
};
struct LapCompletionConfig50GG {
    std::uint32_t mode11258=0;
    std::uint8_t lapCount1125C=0,selection1125F=0;
};
enum class LapCompletionCall50GG : std::uint8_t {
    None,OwnerSelection292360,LapLookup1861A0,RaceLookup186280,
    LapRecord22B170,RaceRecord22B370,LastLap22B760,
    PlayerEnd217940,PlayerReset217420
};
enum class LapCompletionTarget50GG : std::uint8_t {
    Player,CurrentConfig2DC,PlayerD40Owner488
};
struct LapCompletionRequest50GG {
    LapCompletionCall50GG kind=LapCompletionCall50GG::None;
    LapCompletionTarget50GG target=LapCompletionTarget50GG::Player;
    std::uint64_t sequence=0;
    std::uint32_t callPC=0,calleePC=0,returnPC=0;
    std::uint32_t argumentA1=0,argumentA2=0,argumentA3=0;
    float argumentF12=0.0f;
    // Mask bits 0/1/2 indicate meaningful a1/a2/a3; bit3 indicates f12.
    // The target identifies a0's concrete owner, never a fabricated pointer.
    std::uint8_t argumentMask=0;
    bool query=false;
};
// An explicit completed return, not a request to simulate or skip a helper.
// originalV0 matters only for the two record queries; either nonzero means true.
struct LapCompletionReturn50GG {
    std::uint64_t sequence=0;
    LapCompletionCall50GG kind=LapCompletionCall50GG::None;
    std::uint32_t originalV0=0;
    bool query=false;
};
enum class LapCompletionPhase50GG : std::uint8_t {
    Idle,AfterOwnerSelection,AfterLapLookup,DecideRaceLookup,AfterRaceLookup,
    LapRecord,RaceRecord,LastLap,Tail,AfterPlayerEnd,AfterPlayerReset,Complete
};
struct LapCompletionCursor50GG {
    // Native control-flow storage. Never change these to skip a pending call.
    LapCompletionPhase50GG phase=LapCompletionPhase50GG::Idle;
    LapCompletionRequest50GG pending{};
    std::uint64_t lastSequence=0;
    std::uint32_t lastQueryReturn=0,selection=0;
    float roundedLap=0.0f,total=0.0f;
    bool lapRecord=false,raceRecord=false;
};
enum class LapCompletionStatus50GG : std::uint8_t {
    Complete,ReturnAccepted,NeedEffect,NeedQuery,NotStarted,AlreadyActive,InvalidState,
    Storage,UnsupportedNumeric,SequenceExhausted,WrongReturn
};
struct LapCompletionResult50GG {
    LapCompletionStatus50GG status=LapCompletionStatus50GG::NotStarted;
    std::uint32_t sourcePC=0;
    LapCompletionRequest50GG request{};
};
// Start1A2B80: immediately exposes the mandatory292360(1,0,0) on D40->488.
// No player timing/configuration read precedes this call. Reuse after Complete
// preserves monotonically increasing request IDs, so stale returns are rejected.
LapCompletionResult50GG beginLapCompletion50GG(LapCompletionCursor50GG&) noexcept;
// Advance only after synchronizing the concrete owners' helper mutations.
// A pending boundary is returned unchanged; no query default or no-op is used.
// Unsupported numeric/storage results leave the current phase retryable; earlier
// acknowledged external effects remain committed and must not be replayed.
LapCompletionResult50GG advanceLapCompletion50GG(LapCompletionPlayer50GG&,
    const LapCompletionConfig50GG&,LapCompletionCursor50GG&) noexcept;
// Call only AFTER this exact helper really returned. A mismatch changes nothing.
// Effects use query=false,originalV0=0. Queries require their actual v0 value.
// A successful return clears only the boundary. advance executes its continuation.
bool acceptLapCompletionReturn50GG(LapCompletionCursor50GG&,
    const LapCompletionReturn50GG&,LapCompletionResult50GG&) noexcept;

// Ordinary path requires finite normal-or-zero binary32 operands/results,
// FE_TONEAREST, no fast-math/contraction, and bounded CC0 backing for exactly
// max((lapCount1125C&31)-1,0) floats. No other lap slot is read. All records,
// descriptors and borrowed float storage are correctly typed, live, disjoint,
// and serially owned. Do not mutate between source steps except as actual helper
// effects require. Modes2/3 refer only to config+11258, not application flags.
// Complete means all requested calls were explicitly returned. It says nothing
// about whether the provider faithfully implements their still-external internals.
// Only after Complete may50GF resume and reset FF8 at1A0F2C.
}
