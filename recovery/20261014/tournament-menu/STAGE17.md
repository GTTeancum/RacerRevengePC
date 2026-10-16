# Stage17: latch canonical marker composite failures after source effects

Incremental to stage16 commit2b2ea8f. Source-only correction requested by root review. No tests, builds, syntax checks, decoders, launches or project/guest execution performed.

The concrete OriginalUiTournamentCareerResources50KG::Storage now retains canonicalMarkerTerminal and the first failure diagnostic. sampleCanonicalMarker checks this latch before profile/clock/reference/collector work. Its single failure exit helper latches every failure whose accumulated diagnostic records sourceEffectsCommitted, including errors inside service, prepareTournamentMarkerFrame24B278 rejection after successful service, invalid child extent/dimensions, and failed CPU image correspondence. Existing controller/leaf effects remain intact; no rollback occurs. The existing material-table terminal behavior is unchanged.

On repeat, the composite returns false with the original diagnostic (including sourceEffectsCommitted), without servicing the controller, inspecting a replacement reference/receipt, reading a clock, projecting an image, or modifying output. A different timestamp, repaired collector, new reference or changed profile cannot clear this resource-owner latch. There is no reset API. current() returns false while terminal, also closing legacy sampleMarker and other current-gated packet paths. Read-only retained source/CPU metadata remains available for inspection; it does not certify rendering. canonicalMarkerTerminal() exposes the explicit failure state.

Failures before any source effect retain the previous retry behavior. Duplicate/pause early-outs have no source effects, so a subsequent projection rejection after those early-outs alone does not latch. Prior successful service calls are not retroactively treated as effects of an unrelated later no-effect attempt.

Cleanup: caller abandons the failed composite/session, retires all renderer and borrowed views, releases its canonical ScreenTexture references through the existing ordered cleanup, and destroys the resource owner. It must not bypass the failed owner by calling its separate material reference directly or reconstruct only the CPU wrapper around live failed-session state. A genuinely fresh construction uses the existing constructor/population lifecycle; this patch performs no automatic cleanup, stamp reset or shared resource rollback.

Exact changed files:
- platform/linux/native/original_ui_tournament_career_resources50kg.cpp
- platform/linux/native/original_ui_tournament_career_resources50kg.h
- recovery/20261014/tournament-menu/STAGE17.md

No demand forwarding, scheduling, clock owner, material table, frontend admission, profile/result or renderer implementation changes. No new translation units or build-list entries. Reviewed by source/diff reading only; root owns validation.
