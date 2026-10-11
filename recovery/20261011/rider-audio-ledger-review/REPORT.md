# Dormant native UI active-time ledger and typed output adapter

Source-only staging, 2026-10-11. Root owns integration. Initial baseline was `f513a1f207a6eaf0b155fc853551a9f26529513a`; the root subsequently integrated unrelated renderer work as `764319037ad6361f0253aa749be7859773d05059`. The exact existing output-header preimage is unchanged between those observations.

## Result and limits

Two header candidates are staged outside the checkout:

- New `tests/native/original_ui_active_time_ledger50kg.h`: private application-owned ledger/data contract, checked projection/cold accounting, page tickets, accepted-page pending guard, separate page/audio watermarks, inactive exclusions and distinct successful/incomplete observations.
- Existing `tests/native/original_ui_main_output50kg.h`: private typed adapter on the actual dedicated output/transport/mixer owner, one-way dormant binding, real before/after counters, unchanged two-stage fractional policy and permanent no-replay output poisoning.

There is no Frontend member, host-loop caller, session/audio-event call, renderer/capture integration, movement admission or build-list change. The legacy `advanceAcceptedNanoseconds` remains the only existing caller path. New guards have their defaults false; only a future private typed bind selects the new path. Recording that the legacy API has ever been used prevents late switching to a ledger whose page history would be missing.

This is deliberately **not an admission-ready runtime feature**. Three actual-owner bridges are declaration-only: `recordCold(actualInputReceipt)`, `acceptPageInterval(ticket, actualUpdateReceipt, ...)`, and `advancePresentationAudio(actualLease, ...)`. The presentation lease has only a forward declaration, with no issuer. Calling those interfaces before a separately reviewed implementation would fail to link. There is no permissive stub, public readiness Boolean, synthetic accepted page or synthetic successful presentation. Other implemented private helpers exist for static review of the eventual joins.

## Clock and accounting ownership

- A cold one-time bind retains the actual application `HostMonotonicClock50ID` pointer and its unchanged epoch. It records one genuine initial inactive observation with active time zero. A stable application clock must outlive the ledger; neither owner may be copied or moved.
- Raw observations are exact differences between that epoch and newly sampled native steady-clock ticks. This Linux-specific candidate explicitly requires integer nanosecond ticks representable in `uint64_t`; unsupported clock representations fail at compilation rather than silently converting with overflow. Nonnegative/order-compatible ticks are checked before subtraction. No application epoch reset, estimated clock offset, `sampleClock` call, source timestamp conversion or material/RNG mutation is introduced.
- Ticket preparation samples once without committing an active observation. It checks the complete page interval against the existing one-second bound, independently from the PCM tail. An accepted-page acknowledgement moves only its actual page sequence/watermark and marks that page pending. While pending, another projection, cold receipt or page acknowledgement is rejected. PCM failure preserves the accepted-page fact.
- A successful checkpoint commits the active observation and consumes pending cold duration once. Only a positive PCM or explicit-muted interval increments the audio interval sequence. A zero delta returns `noChange`, does not call output and does not issue a fresh success receipt. Positive submicrosecond/subsample intervals still make the real low-level call and receive a successful accounted receipt when it succeeds, even if no frames result.
- Presentation checkpoints never move the page watermark. The private common completion helper requires the accepted-page/presentation cause shape and does not mistake pre-wait/budget checkpoints for a completed boundary. This shape check is not the absent live presentation-lease validation.
- Inactive observations exclude the full pause edge, preserve cumulative active time and fractional state, invalidate page baseline/tickets and advance an activity epoch on an actual activity change. Resume starts with a zero page interval. The actual interruption owner must still pause/flush and retire its pump lease; this core does not pretend to perform those operations.
- Last-audio raw provenance remains the last genuinely accounted endpoint across zero/inactive observations. A later interval may span excluded wall time; its admitted delta is the independent cumulative-active difference. Those raw endpoints do not imply all intervening wall time was mixed.

## Cold exclusion contract

The future concrete `recordCold` bridge must pass only the existing measured frontend input cold receipt, including failed preparation and cleanup. The helper checks same application epoch, nonnegative ordered begin/end, disjoint serialized order after the last observation, exact measured duration, repeated endpoint-pair rejection and checked sequence/sum additions. Committed observations validate pending cold duration against the actual wall gap before consuming it. Last cold identity/order remains known after consumption, preventing replay.

This uses the established serialized loading contract: no ledger checkpoint inside a cold operation. It does not treat blending, waiting, rendering, readback, capture, logging or arbitrary work as cold. Zero-length repeated identical endpoint pairs fail closed because the existing receipt has no independent serial; a future receipt-identity change must not silently weaken once-only behavior.

## Truthful output and failures

- Only the ledger can construct the adapter's private typed interval. Stable weak lifetime tokens follow existing renderer receipt patterns and prevent cross-owner reuse; expired output lifetime is rejected before dereferencing the output pointer.
- A one-time typed bind requires fresh counters and the actual enabled transport, or explicit mute with no attempted output/transport. Typed and legacy APIs cannot alternate. Preparation after typed binding is forbidden.
- Enabled output revalidates real transport readiness/pause/rate and its prior successful counter relationship. Time, sequence, carry and live frame/microsecond counter bounds are checked before the irreversible low-level call. Native PCM intervals remain bounded to one second; no stall splitting is added.
- The exact existing accounting remains `carried = deltaNs + remainderNs`, microseconds `carried / 1000`, remainder `carried % 1000`, followed by the unchanged real `AudioSampleClock50HD` microsecond-to-frame carry. There is no second mutable clock, direct ns-to-frame shortcut, resampling or WAV padding.
- A PCM receipt is issued only after `AudioOutput50HD::advanceMicroseconds` actually returns true and live before/after mixer/output/capture counters agree. The receipt copies actual source-mixer frames, mixed/captured frames, cumulative successful microseconds, nanosecond carry, device state/queue/consumed/underrun/overflow counts and returned dropped-frame diagnostic. These asynchronous device observations are diagnostics, not an audible-playback or exact physical synchronization proof.
- Explicit mute is distinctly tagged, retains no PCM counters and cannot certify Song7/event2/WAV. Zero time has no successful receipt. Device unavailability or dropped queued frames does not turn a successfully produced/captured interval into replayable time.
- A false low-level result can leave its sample clock advanced, mixer prefix consumed, full WAV blocks counted, uncounted partial payload bytes and device work. The adapter records the actual before/after counters and diagnostic in `IncompleteAudioAttempt`, leaves its success kind `none`, preserves prior successful ns/sequence/watermark and permanently poisons output plus ledger. Only safe pause/close/finalization remains; unpause and sound-mode changes through this owner reject. Existing transport references still exist, so future host admission must stop further source commands after the fault.
- Post-accept preflight failure also poisons, preserving the already accepted page and recording that no output call was made. Missing counter observations are explicitly distinguished. A rejected retry cannot erase the first incomplete attempt or replay that page/time.
- Close remains the real existing callback join/WAV finalize/reset path, including existing truncation to fully counted capture blocks and no guarantee of queued playback-tail completion.

## Mandatory next joins, not implemented

1. Add the one actual Frontend-owned ledger and initialize it before its first accepted page. Bridge the actual frontend receipt types without replacing their success/sequence with caller assertions. Keep the pending accepted page synchronous through PCM completion; every post-accept error must use its recorded failure path.
2. Replace only page-side cold accumulation with the real input receipt bridge. Keep material cold accounting and independently sampled material behavior unchanged.
3. Sample UI page endpoints at the actual post-input application-clock boundary; preserve ordinary pause/stall/quit behavior and gameplay clock ownership.
4. Supply the concrete successful scheduler/context/image-pair presentation lease and checkpoint/boundary identities. Revalidate actual steady Song7, effect ownership, sound-mode/transport revisions and output lifetime at every presentation checkpoint. No such capability is provided here.
5. Join source-order event2, direction/session/resource/deferred owners, renderer publication and capture separately. Add the corresponding low-level overload documentation only with genuine presentation admission. No movement or capture validator relaxation is authorized by this slice.
6. Obtain the held centralized verification/publication approval before any build, test, syntax check, parser/decoder/importer, probe, native launch, sanitizer, emulation or Git publication. This stage is not runtime evidence.

## Static inspection and artifacts

`SOURCE_PREIMAGES.json` pins the existing header and records the new path as absent. `STAGED_FILES.json` pins candidates. `audio-ledger.patch` is the exact unified diff. Supporting source hashes are in `evidence/SUPPORTING-SOURCE-SHA256SUMS`. The independent review is separate and must name the final candidate hashes. `DELIVERABLE-MANIFEST.json` and `SHA256SUMS` enumerate the completed package.

Only source reading, source editing, diff generation and hashing were performed. No compilation, syntax checking, tests, project parsers/importers/decoders, GL probes, native launches, sanitizers or emulation were run. The working checkout was not edited or committed by this worker, and nothing was published.
