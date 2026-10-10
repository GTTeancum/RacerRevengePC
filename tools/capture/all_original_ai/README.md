# All-eight original-AI capture and prefix export

Unexecuted source-only helpers. No module import, syntax check, runtime, encoding or software test was performed for this backup. Earlier capture helpers are preserved separately.

## Runtime and local prerequisites

The collector is pinned to combined local source `d3d854b67d3cbcae8bd640c85e2e9b6756bbea5f`, published as `9bf910c7b86c0df08449a9c27efd88bbef81693c`. Required applied main SHA-256 is `f18e369d8c436cf3c94ee66539a698966a648314ca1572150d2be9e64fda202f`; the existing phase-timing header must match `815e00ed388fc1dc114a4bbc8f3453fdadd8fb28018cbaf368ad75e4f017748b`. The runtime is on a separate branch; this helper branch retains an older runtime baseline.

Exact reviewed cloud paths remain in the modules. The collector requires the existing central ledger, three immutable source manifests and their source files, the independently built matching binary, collision fixture and user-supplied game/audio data. These are not included here. A clone alone is not a portable execution bundle. The root build receipt must establish binary-to-source provenance; source hashes alone cannot do that.

## Sole collection

`capture_all_original_ai.py` accepts only the already reserved central slot, matching source commit/start time and zero previous launches. It uses the shared lock, compare-before-write ledger and pre-Popen charge, with no retry or replacement reservation. The frozen floor is 2026-10-10 01:20:53.750715 UTC; the rolling-hour policy still applies.

It clears inherited RR settings and selects all eight original-AI owners, zero humans, camera slot 0, direct GC/mode2, incomplete-world opt-in, menus off, real contacts/audio and `RR_AUTODRIVE=0`. Only capture/quit commands are sent. The 12-second window begins at ready, includes countdown and bounds requests/acceptance, not hard process lifetime. Existing cleanup bounds preserve an honest stopped prefix.

Actual ready and subsequent fleet groups must prove eight stable original identities, vehicle58=1, initialized/active/Ready owners and zero HOST-driver/controller/keyboard counts. Native ticks, AI dispatch and planner work must grow for every racer. State transitions, braking, combat and route counts are observations; no profile is forced to choose a brake or survive an unsupported boundary. Bounded ownership evidence and overall continuation acceptance are separate outcomes.

Fresh-image primary audio anchors come from that exact paired UI mixed-frame count. Asynchronous WAV size is diagnostic only. Retained/unpaired images have no primary anchor; finalized PCM must cover every fresh anchor for complete capture. Timestamps and native samples are unchanged. Exact anchors do not cure real simulation-versus-wall-clock drift.

`RR_TRACE_PHASES=1` enables the existing six native phases. Those rows exclude audio and external Python work and do not measure all worker CPU. The separate optional audio/countdown timing overlay is not in the pinned runtime and requires reviewed rebinding before use.

## Artifact-only export

`encode_all_original_ai_prefix.py` admits finished `rr-all-original-ai-capture-v1` receipts and preserves their actual commit and acceptance conclusions. Its earlier staging label does not constrain the recorded commit. Use only after the central run, with `--root-requested-encode` and a new output directory. It never builds, launches, reserves or retries gameplay.

Only a contiguous fresh paired image prefix before the earliest boundary, policy violation, stop or ready-window cutoff is exported. Frame timestamps keep the existing millisecond time base; no interpolation, duplication, speed adjustment or artificial continuation is introduced. Two images can form video; one yields a still; zero yields only disclosure/receipt.

Real PCM is included only within the existing 80 ms maximum/50 ms final skew bounds. Missing, incomplete, silent or misaligned audio is retained separately and omitted, or rejected by `--require-audio`. No stretching, resampling, header repair, normalization or synthesis is performed. Artifact verification never upgrades runtime acceptance or establishes a full race, complete GC world or original bootstrap parity.
