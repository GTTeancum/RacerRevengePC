# Independent source review: dormant UI audio ledger/output

2026-10-11. Reviewed against sections 2, 3 and 9 of `../presentation-audio-clock/INTERFACE-PLAN.md`, with the native clock, frontend cold/update receipt, transport, output and sample-clock source read for context.

## Result

No blocking source-level defect found in the frozen **dormant staging slice**. This is a source-reading result, not compilation, execution, admission, audio parity or audible-output evidence. The actual accepted-page, cold-input and presentation admission bridges remain declaration-only, as explicitly required by the slice. These bytes must not be presented as a working presentation-audio path.

## Exact reviewed identities

- `staged/tests/native/original_ui_active_time_ledger50kg.h`: `93ab5c7f85ff0c317c8a2d897a648da282462411b84d014995b591be2210a8d9`
- `staged/tests/native/original_ui_main_output50kg.h`: `dfe133040d5267a00535aae8579272ce37d86f1694901d99a3a35c9492dd11e6`
- `preimages/tests/native/original_ui_main_output50kg.h`: `f94d57a9f946080796657d475bbc8d7b21f0c59988d305a8f6ad4781818aa965`

The output preimage was also byte-hash-matched to the repository file at review time. The plan's baseline is `f513a1f207a6eaf0b155fc853551a9f26529513a`; the owner reports a later renderer-only HEAD, which does not change this matched output preimage.

## Source findings

- **One clock domain:** Raw observations subtract the bound application `HostMonotonicClock50ID::epoch` directly. Native tick-period/representation assertions and nonnegative ordering checks make that subtraction/conversion explicit. The epoch is neither reset nor estimated from the older host-loop origin. Raw sampling does not call the material/source-clock sampler.
- **Independent watermarks:** Projection accumulates admitted active time. Page delta uses only the preceding accepted page endpoint; PCM delta uses the preceding accounted audio endpoint. Presentation completion does not move the page baseline. First/resumed page intervals remain zero and the page's one-second bound survives intervening PCM service.
- **Exactly-once progression:** Tickets bind private lifetime, projection revision and cold state. The final `acceptedPagePending_` gate prevents new observations, new cold records and second acknowledgements while a genuine page awaits PCM. Acknowledged page state survives subsequent failure. Successful completion increments revision and retires the pending page.
- **Cold exclusions:** The private measured-cold primitive checks same-epoch begin/end, nonnegative duration, serialized/nonoverlapping order, duplicate endpoints, exact measured duration and checked summation. The next committed observation consumes its pending duration once. It cannot itself establish that a caller supplied a genuine input/loading receipt; that authentication is intentionally deferred to `recordCold(actualInputReceipt)`.
- **Truthful result surfaces:** Successful receipt data is privately populated after live output success and observed counter consistency. Explicit mute is separately labeled and owns no output/transport/capture. Zero delta produces `noChange`, not a fabricated PCM receipt; a positive submicrosecond interval still advances admitted accounting with the actual possibly-zero frame result. Successful and incomplete result surfaces stay distinct.
- **Irreversible failure:** The adapter snapshots real native counters before and after `advanceMicroseconds`. A false return or inconsistent post-call counters latches output and ledger faults, preserves the last successful watermark/receipt and retains an incomplete attempt. Accepted-page failures before the output call also latch safely. Rejected retries preserve the first incomplete attempt. Pause/close remain available; typed and legacy advancement cannot replay a faulted typed interval.
- **Fractional policy:** Nanoseconds carry into integer microseconds exactly as before. The existing native `AudioSampleClock50HD` still performs the second carry into frames. No second mutable sample clock, direct nanosecond-to-frame rounding, padding, resampling or synthetic interval split is introduced.
- **Dormancy and C++ inspection:** No host-loop/coordinator/session call site, event command, callback, mixer implementation, renderer, capture path or build list is changed by these candidate files. The legacy entry remains the active path until private binding is explicitly wired. No obvious declaration/access/aggregate-initialization issue was found by reading, but this is not a syntax-check claim.

## Resolved during review

An earlier candidate allowed the same still-current page ticket to be acknowledged again before PCM completion and did not latch all early post-acknowledgement checkpoint failures. The frozen candidate adds the pending-page gate and post-accept rejection latch. The earlier observations are not outstanding findings against the hashes above.

## Required limits before any later admission

1. Implement the actual receipt bridges with the exact prepared page ticket and real accepted coordinator receipt. Private helper arguments are trusted application-owner data, not a public capability API. No declaration-only bridge currently proves those joins.
2. Supply and revalidate genuine scheduler/context/immutable-pair and steady-transport authority, including control revision, activity epoch and actual native boundary identity. `ready()` in the output adapter is not Song7/event2/steady-envelope or presentation certification.
3. Bind the application's explicit sound policy and stable lifetimes, and retain exclusive main-thread producer/control ownership. The application clock must outlive the ledger. Existing mutable transport aliases are not independently revoked by a ledger fault; the real owner must stop normal work and perform safe pause/close.
4. Wire genuine inactivity handling to the real pause/flush owner and retire the presentation lease. `observeInactive` alone does not do that. Startup/resume and cold exclusions must not be repurposed to hide rendering, waiting, capture or logging time.
5. Treat receipt raw begin/end as observations spanning the prior accounted endpoint and current endpoint. They may span excluded startup/pause/cold wall time or activity epochs; raw wall subtraction is not a PCM duration or physical synchronization claim. Accounted active delta is the PCM authority. No-change observations leave the last successful audio receipt intact.
6. Native partial WAV payload beyond completed blocks is not quantified by existing counters. The incomplete attempt honestly records available counters/diagnostics rather than claiming rollback or complete payload visibility. Device availability/drop/underrun observations likewise do not prove audibility.
7. Legacy `advanceAcceptedNanoseconds` accounting is intentionally retained; the new typed poison guarantees must not be advertised as a retrofit of the active legacy failure path.

No builds, tests, syntax checks, project parsers/decoders/importers, native launches, GL probes, emulation or Git publication were performed. Candidates were not modified by this reviewer; only this review was written outside the repository.
