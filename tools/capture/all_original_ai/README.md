# 12-or-60-second all-eight original-AI capture and prefix export

Unexecuted source-only helpers. No module import, syntax check, runtime, encoding or software test was performed for this backup. Earlier capture helpers are preserved separately.

## Runtime and local prerequisites

The collector is pinned to combined local source `bf42968ac607ff5bdd38522611d006276cdebd6a`, published as `51c6e7071df6b54d472f1aa16a86db0e6090f54a`. Required applied main SHA-256 is `e53c563303f311aded314f6342254d8c5e444f52e0a8887405d068ccb896ed02`; the existing phase-timing header must match `815e00ed388fc1dc114a4bbc8f3453fdadd8fb28018cbaf368ad75e4f017748b`. The runtime is on a separate branch; this helper branch retains an older runtime baseline.

Exact reviewed cloud paths remain in the modules. The collector requires the existing central ledger, five immutable source manifests and their source files, the independently built matching binary, collision fixture and user-supplied game/audio data. These are not included here. A clone alone is not a portable execution bundle. The root build receipt must establish binary-to-source provenance; source hashes alone cannot do that.

The five manifests cover all-racer ownership, fresh peer/combat observations, route choices, batching and typed scalar reads. Their formats explicitly use `sha256` for the first three and `staged_sha256` for the last two. Every final typed-stage source file must match the applied runtime, including the new scalar observation header. No later runtime change inherits these pins.

The exact capture bundle manifest SHA-256 is `0cd12b67705e37677a310045b7a964f4f37e881c7579086ee47231560f9eea07`; collector SHA-256 is `6095b3647c0c0daa392b55912e464ab80284d25b086d881e5551229d4586e5a4`. The matching encoder manifest is `bf413f7fb635894f5dbb70a827d62739e8cbc4043601c31cc67d2f6b39ba3f03`, with encoder SHA-256 `06cdbf2d551dea1b94e8ef48a18f6519737b998f2e25265326dce72566847d14`. The clock helpers retain their previous bytes. Collector, evidence report and encoder record the actual selected window; no source-boundary, timestamp, PCM or skew-policy change is included. These revisions are unexecuted.

## Sole collection

`capture_all_original_ai.py` accepts only the already reserved central slot, matching source commit/start time and zero previous launches. It uses the shared lock, compare-before-write ledger and pre-Popen charge, with no retry or replacement reservation. The frozen floor is 2026-10-10 02:21:13.272691 UTC; the rolling-hour policy still applies.

It clears inherited RR settings and selects all eight original-AI owners, zero humans, camera slot 0, direct GC/mode2, incomplete-world opt-in, menus off, real contacts/audio and `RR_AUTODRIVE=0`. Only capture/quit commands are sent. Exactly `--seconds 12` or `--seconds 60` is admitted; the default remains 12. The next requested central run selects 60 explicitly. The selected window begins at ready, includes countdown and bounds requests/acceptance, not hard process lifetime. It is recorded in slot, runtime evidence and artifact receipts. Existing cleanup bounds preserve an honest stopped prefix.

Actual ready and subsequent fleet groups must prove eight stable original identities, vehicle58=1, initialized/active/Ready owners and zero HOST-driver/controller/keyboard counts. Native ticks, AI dispatch and planner work must grow for every racer. State transitions, braking, combat and route counts are observations; no profile is forced to choose a brake or survive an unsupported boundary. Bounded ownership evidence and overall continuation acceptance are separate outcomes.

Fresh-image primary audio anchors come from that exact paired UI mixed-frame count. Asynchronous WAV size is diagnostic only. Retained/unpaired images have no primary anchor; finalized PCM must cover every fresh anchor for complete capture. Timestamps and native samples are unchanged. Exact anchors do not cure real simulation-versus-wall-clock drift.

`RR_TRACE_PHASES=1` enables the existing six native phases. Those rows exclude audio and external Python work and do not measure all worker CPU. The separate optional audio/countdown timing overlay is not in the pinned runtime and requires reviewed rebinding before use.

## Artifact-only export

`encode_all_original_ai_prefix.py` admits finished `rr-all-original-ai-capture-v1` receipts and preserves their actual commit and acceptance conclusions. Its earlier staging label does not constrain the recorded commit. Use only after the central run, with `--root-requested-encode` and a new output directory. It never builds, launches, reserves or retries gameplay.

Only a contiguous fresh paired image prefix before the earliest boundary, policy violation, stop or source-selected 12- or 60-second ready-window cutoff is exported. Frame timestamps keep the existing millisecond time base; no interpolation, duplication, speed adjustment or artificial continuation is introduced. Two images can form video; one yields a still; zero yields only disclosure/receipt.

Real PCM is included only within the existing 80 ms maximum/50 ms final skew bounds. Missing, incomplete, silent or misaligned audio is retained separately and omitted, or rejected by `--require-audio`. No stretching, resampling, header repair, normalization or synthesis is performed. Artifact verification never upgrades runtime acceptance or establishes a full race, complete GC world or original bootstrap parity.
