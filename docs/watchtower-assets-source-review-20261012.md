# Watchtower combined integration: independent source review

Reviewed 2026-10-12. This is a source-only review of the isolated assets, material-audio, and original-AI integration. It is not build, runtime, rendering, audio, or gameplay validation.

## Result

**No definite blocking defect found in the existing prepared combined code.** The reviewed staging can remain the source-integration candidate. GA admission must remain closed until its separately owned dependencies and authorized validation are complete.

The absent generated profiles, separate native animated-world/action join, and separate original-camera owner are known scope boundaries, not newly discovered bugs. This review does not authorize opening GA admission or executing preparation.

## Exact inputs and integrity

- Base: `/workspace/shared/rr-recovery-git`, clean `c6814e73ccfd179c26c700a4337f5005a1d8b0b2`.
- Combined overlay: `/workspace/shared/watchtower-root-staging-20261012`.
- All 30 source paths listed in `BASE.json` exist in staging. Every recorded base preimage matches the base file or expected absence.
- Every non-main staged asset, audio, and AI file matches its corresponding candidate manifest hash. The shared main-file diff was read as the combined integration, rather than compared to any worker's independent full-file snapshot.
- Every dependency listed in `tools/assets/ga-preparation-source-pins.json` matches its named base or staged candidate file.
- Asset patch SHA-256: `a70a6f4c611493a4d01288828fc0fc81113c063c4c5070702ef9a80b9ad28c6a`.
- Audio patch SHA-256: `af2e80ae85e6b54a5830f90142828f33934e0cb343fade7d2cca82b733b15c0e`.
- AI patch SHA-256: `8e5ac4ac799753350458fc5bdbc530887544f89e07f5c3e0c9281e32192f367f`.
- Combined `tests/native/terrain_interactive_drive50du.cpp` SHA-256: `655f1e212e320ef4a04da7da1c7c4a7a91b1f7753a98ef0d2e9181fbc68b8fab`.

One nonblocking packaging observation: staging also contains `tests/native/terrain_interactive_drive50du.cpp.orig`, outside the 30-path manifest. It is a backup, not a reviewed integration input. Any later transfer should use the explicit manifest rather than recursively treating all staging files as deliverables. Nothing was removed or changed.

## API and header integration

- `Track::ga` is added without renumbering TA/GC. Every GA consumer reached from the combined main's selected `trackSource` has a GA route: visual profile, material owner, original source-image callback, normal sidecar, alpha policy, and sky owner. The remaining unmodified GC dynamic-material and GC ATM consumers remain behind explicit GC ownership.
- `Owner::prepareScene`'s new GA boolean agrees between declaration, definition, and all three public preparation entry points. The new GA cache-policy enum is selected by main and checked by the owner before publication.
- `currentGcGlobalAmbient50` keeps its GC-only meaning. The added `currentSourceGlobalAmbient50` admits authenticated GC/GA, rejects TA/unprepared/invalidated owners, and is the method now consumed by the Binder fixture.
- The Light9 helper and GA ATM source/window owners are header-only. Their changed callers reference existing base renderer, texture-bank, animation, and parameter APIs with matching field/type layouts on inspection.
- New original-AI observation handles use the actual `uint32_t` racer60/FE4 fields and `float` repair time. They are scalar subranges of the already registered `originalAi` state and are bound after the racer bank is sealed. No additional state owner is introduced.

## Fail-closed admission and source profiles

- `track_session_assets50ig_fixture.h` retains `retainedGA` as metadata only. `sessionAssets()` still returns only TA or GC. Host/menu admission is not opened by this candidate.
- No generated GA include is present in either reviewed staging or the base native fixture directory.
- Missing `ga_visual_profile.inc` causes an explicit unavailable-profile exception. Missing `sky_ga_index.inc` similarly rejects GA sky selection. Missing `source_texture_ga_index.inc` returns an explicit source-image failure. The ATM owner requires both generated tables and rejects at the start of `load()` when either is absent, before accessing fallback entries.
- Generated data is still a future review gate. The cold-preparation command authenticates source/dependency hashes and all original member bytes, writes to a new separate output directory, and quarantines partial failure. Its code was read, not invoked.
- The assembly profile initializer has the same field order as `Profile`. Its 500,000-vertex/1,500,000-index bounds fit the combined main's 16 MiB vertex, 8 MiB index, and 8 MiB normal pools by the existing native layouts. Actual generated sizes and bytes have not been established.

## GC/TA behavior and routing

- The GC native Gate, GC incomplete-world opt-in, and GC animated-world constructor now use `isGcTrack`. GA therefore does not reach a GC archive/geometry owner through the generic `originalStaticCandidates` flag.
- Remaining shared static paths legitimately consume the selected session: original static collision preparation, AUB assignments, required textures/all authored passes, projected UV publication, wrap samplers, and source vertex-color policy.
- The audio branch selects GA first, then retains GC/TA behavior. This combined main includes the independent audio hunk; it does not retain the assets-only snapshot's GC selection for GA.
- TA still has its prior visual budgets, alpha route, 17-spot iteration count, and 8-row audio profile. GC retains its prior budgets, two-light preparation route, 12-row audio profile, and existing dynamic owners. Increasing backing portal capacity to 18 does not change the exact accepted TA/GC world counts.
- The new AI readiness reads execute only when `allAiEnabled`. Existing mixed-GC readiness does not acquire the new racer60/FE4/repair-time value dependencies.

## Fixed material-audio ownership

Source references: `material_audio_provider50hi_fixture.h:9-12,56-67`; combined main `:925-948`; `audio_mixer50hd.h:8`; `live_audio50hf_fixture.h:18`.

- Mixer maximum and live voice backing both become 160. Mixer routing, group, and playback-revision arrays use the same maximum constant; the base implementation iterates actual `count_` and checks slot bounds.
- TA stays at collision 64..71 and drive 72..79.
- GC extra collision stays 118..121 and extra drive 122..125. Existing Gate 126 and unassigned 127 are untouched.
- GA adds collision 128..139 and drive 140..151, with no overlap with either the 16 shared base material slots or existing other providers. Slots 152..159 remain unused by this provider.
- Provider storage grows to 40 sounds/slots; GA requires the exact 20-row/4-material profile and 37 unique original requests. Extra group binding uses `extraVoiceFirst()` plus the actual extra count, avoiding the old hardcoded GC start.
- Every one of the 20 cached pitch slopes initializes to -1. Existing four-variant key arithmetic, source assignment behavior, provider restart guards, and unavailable-PCM semantics are retained. Actual PCM readiness/playback is unverified.

## GA ATM ownership, memory, and renderer join

Source references: `ga_atm_animation_fixture.h:9,33-48,96,115,170-194`; `ga_atm_window_fixture.h:24-67`; combined main `:2817-2842,2899-2902,3249-3252,4232`.

- Four authenticated source streams map to 35 actual stored images and 57 independent placed-material animation states. The generator checks placement distribution 20/5/22/10, initial frames 0/1/5/3, rates 5/14/12/15, and total RGBA extent 2,293,760 bytes.
- The 512 KiB ATM extraction bound admits the documented flag-stream sizes. The CPU image/source bank and GPU request both use 4 MiB caps; 35 RRTEX headers add only 560 bytes to the cited RGBA extent.
- Load authenticates archive, ATM, PSM, and individual PST/native identities, associates the exact static source texture, checks the authored initial image, and rejects repeated material/placement owners or an incorrect 57-owner count.
- Window descriptors match `VehicleWindowTrackAnimation50DS`; 57 owners are below the base renderer's 128-owner maximum and each 5/10-frame stream is below its 64-frame maximum. Selected-frame array counts agree across owner and renderer bindings.
- The shared-frame sampling policy must agree across owners. GPU publication is required before render request/frame binding. Cleanup closes the renderer before the GA texture bank and before context teardown.
- Animation advancement is transactional and allocation-free by source structure: local fixed arrays are committed only after all 57 advances succeed. The combined main advances it at the existing accepted-world service boundary using the shared application timestamp.
- The accepted-world timestamp cadence is an explicitly documented native integration policy. Retail material-call cadence and timing parity are not established by this code or review.

## Authored GA fog, lighting, and Light9

Source references: combined main `:228-248,1322-1339,3082-3097`; lighting owner `:264-329,430-446,708-717`; helper `:30-43,101-186`; Binder fixture `:56-60`.

- Fresh admitted GA races default to their GA light policy; explicit diagnostic opt-out remains available. Matching track and fresh/non-checkpoint/non-Main constraints are retained.
- Default world fog uses authenticated GA RGB 117/107/66, visibility 0.4, and the existing 0.5 haziness policy. The sky color reader now handles the two-digit blue field correctly; the unchanged three-digit TA/GC strings retain the same parsed values.
- GA lighting authenticates its own SCB and exact 12-zone/18-portal shape. It requires 38 lights and iterates all 36 authored spotlights, while TA retains exactly 17.
- Light9 is not repaired from its neighboring malformed section. The helper reads only the selected block, preserves zero position/raw look and one-based zone 1, and preserves the constructor's distinct effective +X direction when the zero-look equality branch skips normalization. The GA consumer uses `positionA0`, `effectiveLook78`, and `initialZone` directly and does not normalize again.
- Other source fields, including negative light colors, are retained through the existing RGB-halving and selection policy. The helper's field precedence and transactional failure paths are coherent on inspection.
- First ambient RGB is halved to source global ambient (54,54,41.5,64). GA rejects the ambient-flash enabling field; Binder receives this global value, not the later negative-light-adjusted selected row. Existing GC-only access remains isolated.
- The authored-zone/top-two spotlight policy and native arithmetic are explicitly bounded policies. Retail light-volume/cache ownership, EE/VU rounding, and rendered appearance remain unverified.

## Original-AI selection integration

- The fresh binder keeps selected track ID, course-order position, and course-record pod separate: GA 8/3/10 selects class A=1; GC 10/10/17 selects class C=3. Existing strict GC compatibility API is unchanged.
- GA's RepairTime1064=2 override is applied after each actual racer's original `.ai` profile load. FE4 compares the actual pod identity to the selected course-record pod, with a consistency check against runtime initialization.
- Future GA admission requires fresh race mode, all eight original-AI owners including slot 0, no checkpoint, and no menu/Main start. This is not a substitute for the still-separate original-camera implementation.
- The added readiness reads do not mutate runtime state and preserve the existing all-racer failure boundary. They do not certify any lap, reset/contact continuation, or gameplay outcome.

## Verification limits and handoff

Only text/source inspection, static JSON metadata reads, byte hashing, repository status inspection, and writing this review were performed. No compiler, linker, build, software test, syntax/import checker, candidate script, asset decoder, native program, renderer, audio playback, or gameplay was run. The base checkout and staging code were not edited.

Before any runnable claim, the root still needs the separate source joins, review of authorized cold-preparation output, and authorized integrated validation. Friday 2026-10-16 remains the supplied user deadline; this report does not claim those remaining steps are complete.
