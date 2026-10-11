# Independent source review: dormant Rider audio capability

2026-10-11. Reviewed the four complete staged candidates against their exact staged preimages and against `INTERFACE-PLAN.md` section 4. This is a source-only review, not a compile, test, runtime, PCM, device, presentation, or gameplay validation.

## Result

No blocking source-review finding in this bounded candidate. The slice is suitable for parent review as dormant transport/mixer groundwork. It does not admit Rider direction movement, attach a presentation lease, pump PCM, or establish an audible result.

The checkout was initially at assigned base `6dc2c8b1b3064f8fdeb5f12648fe4583119c1b9e`. During review another task advanced HEAD to `6db1578b4d664d61ba1b7185e0be1ba176963476`; the four audio files still exactly matched this slice's preimages, and the checkout was clean at that observation. This reviewer made no repository changes.

## Reviewed candidate identities

All paths below are relative to `candidate/`:

- `platform/linux/native/audio_mixer50hd.cpp`: `3400d6b79c700018c1003aee66588adba0696983fddfd975cff6bae7954e1846`
- `platform/linux/native/audio_mixer50hd.h`: `a6b77e9ba9dd6ec2a95bb05f336b7a4982784d8441646de1c361e748a7e1d566`
- `platform/linux/native/original_ui_main_audio50kg.cpp`: `dc931a5b40e26dbabbb9ca60007963fbbeeca43425322894a576c36cfe38caa4`
- `platform/linux/native/original_ui_main_audio50kg.h`: `b67e95dd88c87a09669379c0df2fef230232f1410a218cb5f86b9f316860fc1a`

## Findings supporting the bounded result

1. **The live observation uses concrete mixer state.** `inspectVoice` copies the actual voice, per-slot playback revision, persistent group gain/known flag, and cached original left/right routing. It neither advances cursors nor changes bookkeeping. The steady inspector also checks exact seven-slot layout, preparation-time mixer epoch/count/rate, actual master gain, required-group policy, and group state on every slot. This addresses the hazards that `AudioVoice50HD::pan` alone cannot establish source routing and that `prepare` historically accepts more than seven slots.

2. **Song7 is established by ownership and actual playback identity.** The inspector requires owned active Song7, no active fade, expected gain and zero elapsed/duration/retirement fields, the retained authenticated Song7 PCM backing and metadata, the same actual clip pointer/metadata in voice 0, a matching successful playback generation, centered cached routing, pitch 1, loop true, and a finite in-range cursor. An identical-clip replay through the mixer changes the slot generation and fails this match. A music-song label alone is insufficient and is not used as sufficient evidence.

3. **Active effects are checked, completed effects are not revived.** Every active effect must have transport ownership, the expected per-slot playback generation, authenticated retained clip data, and fixed one-shot controls/routing. Naturally inactive slots remain acceptable even before a real page update clears `ownsEffect_`. No inspector calls `update`, `stop`, `play`, or the mixer producer. Static inspection of the original decoder confirms that mono preparation retains `sourceSha256` and clears loop bounds when no embedded loop exists, supporting the new retained-metadata checks without running a decoder.

4. **Event2 uses the exact existing effect operation.** `canPlayRiderAdvance2` requires the live steady Song7 inspection and authentic clip 6. `playRiderAdvance2` clears the supplied receipt, repeats concrete preflight, and delegates once to `playEffectClip(6)`, preserving its same-slot stop-before-play behavior. The issued receipt contains event 2, slot 6, transport/mixer/mode owner identities, command revisions before/after, mixer epoch, sound-mode revision, current mixed-frame count, and the successful voice snapshot. Its private issued state is set only after the actual operation. It is explicitly a historical command result, so later natural effect completion does not negate it.

5. **The independent sound owner is represented.** The capability requires a ready actual bound sound-mode owner equal to the pointer captured at cold preparation and records that owner's own revision and profile mode. It projects supported centered routing and checks the live cached routes separately. A direct mode-owner apply can therefore be detected by a future lease even if it bypasses the transport's command revision.

6. **Revision coverage follows mutations, not PCM time.** Mixer initialization, binding, successful starts/restarts, changed controls/loop/master/group state, actual stops, and group-policy changes update the mixer command revision. Transport preparation, starts/restarts, accepted mode application, fade commands/progression, actual pause changes, and reset changes update its revision. Per-slot playback identity changes on every successful play. The unchanged mixing loop and sample clock do not touch these revisions. Ordinary steady `update` and housekeeping after a naturally completed one-shot remain revision-neutral.

7. **Overflow handling preserves ordinary behavior without reusing authority.** New Rider playback preflights one transport revision and a conservative two mixer revisions before any stop/play mutation. Existing generic methods retain their original playback/return behavior if bookkeeping is exhausted, while the exhaustion flag permanently makes capability inspection unavailable; counters do not wrap. This is narrower than changing generic mixer command semantics. A final representable revision remains usable until an attempted further control change poisons it. No new command can reserve unavailable capacity.

8. **Legacy audio behavior remains structurally intact.** The compared source diff adds observation/revision bookkeeping and dormant Rider APIs. Existing audio formulas, clip dispatch allowlists, fade arithmetic and retirement ordering, actual control values, producer loop, sample-clock arithmetic, source mixing, and pause semantics retain their existing operations. There is no new host/page/session/output caller, active direction admission, scheduling wait, callback producer, device operation, asset decode path, RNG operation, or build-list change in this slice.

## Integration boundaries still open

- A copied observation is not an application capability. Future code must retain the actual stable transport, private mixer/storage, output, and sound-owner lifetimes; compare the mode identity to the application's real shared owner; and bind/revalidate transport, mixer, and sound-mode revisions at each checkpoint. Raw pointers or editable observation fields alone cannot establish that authority.
- The observation does not inspect output enabled/closed/paused/fault state or certify successful PCM/WAV/device work. The separate output owner must join those facts and its lifetime.
- The receipt does not establish a direction transaction, source call ordering, unique consumption by that transaction, source updates, immutable image pair, renderer completion, native presentation, or audibility. Future admission must join the successful command at the genuine input sound point and preserve the original `267BE0` boundary without introducing another audio advance.
- The final steady observation must follow the real third page update/incoming-source completion. Unexpected subsequent controls, mode changes, fades, pause/reset, mixer lifetime changes, or foreign playback must invalidate the lease before PCM advancement. Natural cursor advance/completion must remain allowed.
- The post-play snapshot failure branch is intentionally not rollback. Under the stated single-threaded lifetime and capacity guarantees it is unreachable after successful preflight; if those guarantees ever fail, callers must not treat a false return as proof that no sound command occurred or blindly replay it.
- No movement, nonzero deferred-state admission, pumping, capture protocol, publication, or central regression approval is supplied by this review.

## Verification performed and withheld

Performed only source/document reads, textual comparisons/searches, repository identity/status reads, SHA256 hashing, and this external review-file write. No candidate code was executed. No builds, tests, syntax checks, sanitizers, project parsers/importers/decoders, native/gameplay/GL probes, or guest execution were run. Runtime and compilation questions remain unverified under the explicit hold.
