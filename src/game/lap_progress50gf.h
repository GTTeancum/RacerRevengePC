#pragma once
#include "track_navigation50eu.h"

namespace rr::game {
// Projection of the player's accepted map-gate effects inside 1A0DA0.
// No crossing tests, progression-index stores, race rules, timing or audio.
// Backed/disjoint caller records, serial ownership, no allocation or callbacks.
inline constexpr std::uint32_t maxLapProgressNodes50GF=10;
struct LapProgressTrack50GF {
    std::uint32_t nodeCount=0,progressCount48=0;
    std::uint32_t progressNodes4C[maxLapProgressNodes50GF]{};
    std::uint32_t predecessorOfZero=0;
    bool prepared=false;
};
enum class LapProgressError50GF : std::uint8_t {
    None,Input,Track,MissingPredecessor,CallbackPending,NoPendingCallback
};
struct LapProgressDiagnostic50GF {
    LapProgressError50GF error=LapProgressError50GF::None;
    std::uint32_t sourcePC=0,node=0;
};
// Call immediately after a successful readTrackNavigation50EU of the MAP,
// before reusing its scratch. Uses the reader's ProgressNode-presence bit,
// never NodeHeight, index decreases, or an assumed list of checkpoint nodes.
// Source 2A1980..19C0 appends present markers in ascending authored node order;
// the numeric ProgressNode value is not an ordering number.
bool prepareLapProgressTrack50GF(const TrackNavigationScratch50EU&,
    LapProgressTrack50GF&,LapProgressDiagnostic50GF&) noexcept;

struct LapProgressState50GF {
    std::uint32_t vehicleFF8=0,vehicleCC8=0;
    std::uint8_t vehicleCD8=0;
};
enum class LapProgressCrossingKind50GF : std::uint8_t {Sequential,Scan};
struct LapProgressCrossing50GF {
    LapProgressCrossingKind50GF kind=LapProgressCrossingKind50GF::Sequential;
    std::uint32_t crossedMapNode=0,currentC88=0;
    std::uint8_t vehicle10CC=0;
};
struct LapProgressCallbackRequest50GF {
    bool requested=false;
    std::uint32_t callPC=0,calleePC=0,returnPC=0;
    // a0 is the actual caller-owned player, not a synthesized pointer word.
    std::uint32_t argumentA1=0,argumentA2=0;
    std::uint32_t vehicleCC8AtCall=0,vehicleFF8AtCall=0;
    std::uint8_t vehicleCD8AtCall=0;
};
// Native control-flow bookkeeping, not an invented guest vehicle field.
// Keep this live until the actual callback has returned; do not clear it to
// make further crossings succeed. Other callback-owned player fields remain
// the responsibility of the caller's concrete native player owner.
struct LapProgressContinuation50GF {
    LapProgressCallbackRequest50GF pending{};
};
struct LapProgressReceipt50GF {
    bool progressMarkerConsumed=false,shortcutSuppressionSet=false;
    bool shortcutSuppressionCleared=false,lapCounterIncremented=false;
    LapProgressCallbackRequest50GF callback{};
};
// 1A0E60 (Sequential) or 1A10D0 (Scan), only for a source-accepted gate.
// currentC88 is the live value BEFORE that crossing's C8C/C88 stores.
// Sequential requires crossedMapNode==currentC88. Scan calls must retain the
// original ascending candidate order and reread live currentC88 each time.
// Success with callback.requested stops at 1A0F24: CC8 and CD8 are committed,
// FF8 is not yet reset, and C8C/C88 must not yet be advanced. Failure preserves
// state, continuation and receipt. Already-pending callbacks reject new work.
bool applyPlayerLapCrossing50GF(const LapProgressTrack50GF&,
    const LapProgressCrossing50GF&,LapProgressState50GF&,
    LapProgressContinuation50GF&,LapProgressReceipt50GF&,
    LapProgressDiagnostic50GF&) noexcept;
// Invoke ONLY after the actual native 1A2B80 behavior has completed, with all
// its mutations reflected in the caller's player/state. Executes 1A0F2C's FF8
// reset, consumes the pending request, and permits the caller to resume at
// 1A0F30. A missing callback implementation is not completion.
bool resumePlayerLapAfterCallback50GF(LapProgressState50GF&,
    LapProgressContinuation50GF&,LapProgressDiagnostic50GF&) noexcept;

// Two distinct source initialization effects. Preserve their real lifetimes:
// 21669C..216704 resets CC8 and CD8 bits0..2, but does not write FF8.
void initializePlayerLapRaceFields50GF(LapProgressState50GF&) noexcept;
// 1A0360 constructor, or admitted full-init store 1A258C. Never infer this
// reset from an undertrack recovery or route wrap.
void initializePlayerLapCursor50GF(LapProgressState50GF&) noexcept;
}
