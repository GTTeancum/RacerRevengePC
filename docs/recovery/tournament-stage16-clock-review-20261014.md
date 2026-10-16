# Tournament stage16 clock/prototype review

2026-10-14. Reviewed supplied stage16 patch and inspected corresponding source now present in root working tree during review. No edits to production, tests/builds/syntax checks/imports/execution. Aggregate texture construction/lifetime review remains root-owned.

## Verdict and actionable issue

No blocking wrong-clock, wrong-pause, duplicate-stamp or unique-merge defect found in the **callback-free service itself**. The construction-context clock is authoritative; the supplied frame view cannot replace it. The implementation is a bounded null-key ScreenTexture prototype path and does not pretend to implement callbacks or racer clones.

**Integration/failure-contract issue:** `sampleCanonicalMarker` may complete service (writing leaf and controller stamps), then fail frame-binding preparation or CPU-image matching. It preserves `sourceEffectsCommitted`, but does not latch terminal state in itself, its reference or the material table. The table latch only covers a failure returned *inside* `serviceTournamentMarker24A740`. Thus the packet's broad statement that later failure after real state writes is terminal is not enforced by this composite API alone.

Concrete sequence: call sampleCanonicalMarker at a fresh timestamp with a stale/wrong collector receipt; service succeeds and stamps; prepareTournamentMarkerFrame rejects the receipt; caller receives false/effects=true. With a valid receipt a retry at the same timestamp may then succeed through the duplicate early-out. No state is rolled back, but no terminal condition prevents that retry. Either the real caller must fail-stop/latch terminal on false with effects=true, or the composite owner must latch it. Alternatively narrow the documented terminal guarantee to service-internal failures and explicitly define recoverable post-service binding failure. Do not advertise a stronger enforced guarantee than supplied. No real traversal caller is present in this packet to supply the missing policy automatically.

## Prototype owner

`OriginalStandardMaterialTextureAnimation50LA` is distinct from existing engine vertex instances. Its actual material owns one unique_ptr per pass; constructor is called only from that material after one-pass/one-map ScreenObject, ScreenMT inheritance, no vertex/pixel leaf and no parameters admission. It retains actual material identity and pass ordinal; the service checks both. No fabricated racer/model registration is introduced.

Authored constructor defaults match the existing recovered numeric contract: initial0, FPS1, looping=true, reverseLoopOnEnd=false, loopOnEndToFrame=-1, animatesWhilePaused=false. Count is intentionally deferred to the actual383400 aggregate. Controller08 and leaf.previousMilliseconds both startFFFFFFFF; reverse/stopped flags start false. Construction does not initialize with a guessed nine-frame bank.

Pass teardown releases parameter/map ownership before textureAnimation reset, consistent with the recovered base-animation retirement order. The animation's material/pass pointers are borrowed and require no callbacks at destruction.

## Clock provenance and gates

- The service verifies live sealed material construction context, SAME texture table, genuine pass prototype/material/pass association, and authenticated PSM/ATM bytes.
- `s.construction->applicationAnimationClock()` is the authoritative source. Frame-forwarded view must be readable and have matching230/11C/120, but is explicitly only a consistency check. Equal values from a foreign view are not claimed as identity; they cannot alter the authoritative source clock.
- Existing texture bindMaterialContext and material population binding preserve the same exact context capability, not merely equivalent video fields. Default, unpublished, retired or non-live-context clocks fail before mutation.
- Controller duplicate stamp is checked first, then true application pause11C against authored animatesWhilePaused. Neither early-out writes controller or leaf state.
- No use of legacy advanceAnimations, milliseconds, host focus, dt or generic scheduler pause in canonical sample/service. Popup/profile eligibility occurs before service at the higher-level API.
- Source global identity comes from the actual retained Application context; no new clock backend, epoch or material-local timestamp generator appears.

## Leaf and controller commits

The service is callback-free and main-thread-only. It reads live owner at entry, leaf29A118, leaf-commit29A16C and controller29AD48. It commits leaf previousMilliseconds from the second leaf read and controller08 from the final read, rather than reusing an entry snapshot.

The numeric helper gets the actual aggregate20 frame count. It runs on a candidate copy, validates selected actual child, then commits leaf state; effects=true is set at that commit. If the subsequent final read fails, the table latches demandTerminal and preserves effects. In this bounded no-callback single-thread operation, owner time cannot normally change between the four reads; the explicit boundaries nonetheless preserve the correct extension contract.

The helper has an inner previous-timestamp duplicate guard inherited from its standalone controller-plus-leaf use. In this bounded path leaf and controller stamps are committed together from an unchanging clock, so that guard cannot independently suppress an otherwise required source leaf update. This equivalence would not automatically survive future callback/reentrant publication support. Keep this path callback-free unless the leaf arithmetic is separated from standalone outer-gate assumptions.

An initialFFFFFFFF sample matches controller sentinel and skips; an already-running controller can later commitFFFFFFFF. No saturation clamp or artificial increment is present. Pause/unpause does not reset phase/stamps; elapsed time on next admitted service includes the actual source timestamp difference, as original.

Callback=false behavior is not falsely implemented: there is no callback setter/domain here. Comments correctly require future callback-false handling to reach the final stamp read. Do not generalize this bounded owner to callback materials without implementing that path.

## Unique nonmerge and frame binding

`equalCollectorPass249BA0` rejects if either participating texture prototype is unique, before returning equal. The collector's actual redirection scan uses that comparator for prior registrations; there is no earlier positive identity shortcut bypassing the check. Existing unique-vertex merge behavior is retained.

Normal representative preparation rejects a texture-animation pass unless explicitly using the Screen null-key entry. The Screen entry calls the actual material's selected prototype and actual aggregate child. With unique-texture nonmerge, the medstar collector representative remains its own pass; no unrelated equal-state pass supplies another prototype's phase. The returned binding remains a borrowed prospective map, not a residency/GPU receipt.

## Remaining boundaries

- `sampleCanonicalMarker` is introduced as an API, not connected by this packet to complete original inherited widget traversal or a rendered mode2 frontend path. Readiness claims must stay bounded to the owner/service implementation.
- Paused-only service scheduling remains absent; authored animatesWhilePaused can be evaluated only when a real service is scheduled.
- The composite failure-policy issue above must be resolved at its actual caller before relying on terminal behavior.
- The service validates resource/provenance prerequisites before source duplicate/pause gates. This is a fail-closed native admission choice, not exact original error-order parity for an invalid/unpopulated resource. It does not change admitted complete-resource success behavior.
- Numerical configuration restrictions and transactional preflight failure are native bounded admission, not arbitrary malformed-asset source-effect parity.

No independent runtime/build validation performed. Report references the source proof at `/workspace/shared/rr-application-animation-clock-proof.md` and prior owner review at `/workspace/shared/rr-clock-round7-review.md`.
