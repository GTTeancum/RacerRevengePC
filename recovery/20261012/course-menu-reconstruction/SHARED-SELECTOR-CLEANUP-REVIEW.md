# Independent Course selector cleanup review

2026-10-12 03:11 UTC. Four-file excluded candidate under `/workspace/shared/course-recovery-20261012`, reviewed against its retained previous source in main's recovery tree. Shared owner definition is the already integrated `src/game/shared_random_owner50id.h` at main `9fba56eb1021eab9624b1756db2bef363077d025`.

## Result

No definite source/API/lifetime blocker found in this cleanup. Acceptance is limited to the excluded candidate; both `riderConfirmReviewClosed` and `coursePopupAncestorReviewClosed` remain false. No menu-to-race admission was added. No build, test, syntax/compiler probe, project script or gameplay execution was performed, and no live repository/candidate source was edited by this reviewer.

## Reviewed effects

- The voice selector no longer owns private copies of source80A8/3D6B30 and80AC/3D6B38. Its class is stateless, and its only changed API parameter is an explicit `SharedRandomOwner50ID&`.
- The noncopyable/nonmovable `OriginalUiMainApplication50KG` owns one aggregate, initialized once with the application. Page/session destruction cannot reset it. The application remains retained by each live session through its existing shared pointer.
- The actual accepted-confirm call borrows the aggregate from the same application that supplies the existing clock and `sharedNoise308030`; there is no copied cache, substituted raw RNG or new clock epoch.
- The entire integer choice calculation and exactly-one-draw placement are unchanged. Lazy guard initialization precedes the conditional clock call. Clock failure retains that initialized guard. Successful clock sampling stores the cache and seeds the same raw RNG before the draw; any later failure retains this completed prefix. A zero clock result remains zero and permits the source reseed branch on a later call.
- Range initialization and source conditional assertion now use the aggregate's range subobject in the same order. The receipt's seed value remains a readout, not another owner.
- Main startup's separate native raw seed does not initialize the lazy selector state. Current race owners remain separate under the existing explicitly bounded modes; future menu-to-race admission still must join the exact application owner, clock and raw RNG rather than copy them.
- The new include resolves to the integrated common header. Source comparison found only the intended ownership/signature/call-site edits in the four files.

This resolves the voice-private ownership concern identified in `course-shared-selector-review-20261012.md`; it does not broaden the original Course route's earlier static acceptance or establish hardware/runtime parity.

## Final exact SHA-256 pins

Paths are relative to the candidate root.

| Source | SHA-256 |
|---|---|
| rider/src/game/original_ui_rider_voice_choice50kg.h | 226345eceeb203234039da88fca747e87e50e3bd852ad7169edc8a8f285d373c |
| rider/src/game/original_ui_rider_voice_choice50kg.cpp | 7985815fcf15737c0c879f85ef056c72dc96a293dd1900b654517ae916611220 |
| rider/platform/linux/native/original_ui_main_session50kg.h | 3ad2a17957dfb52d8da26f6e7be05fc7cfe42d40fd7d32af4b156fe56b0633fd |
| rider/platform/linux/native/original_ui_rider_session50kg.cpp | fe16e18236ae914f3e87fef2b1d2126a05db6e69c3985ecceaeaacb02a772cb4 |
