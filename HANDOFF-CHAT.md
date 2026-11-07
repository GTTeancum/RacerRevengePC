> Current handoff: [November 6, 2026](docs/HANDOFF-20261106.md). Its current constraints and checkpoint supersede the historical notes below.

# Additional runtime verification: October 17, 2026, 11:39 p.m. EDT

The same compiled checkpoint also passed a fresh normal-menu run through Anakin/Watchtower, real throttle, Pause, confirmed Restart, a fresh race countdown and player control, then Pause -> Original Main -> window close. Process exit0, no timeout. Two same-run screenshots were delivered. Full lap, finish/results and all-racer/all-track coverage remain unverified. Subsequent coverage runs must use a new racer/track pairing, prioritizing untested racers and tracks. This supersedes the pending Restart/return status below.

# Current tested checkpoint: October 17, 2026, 9:34 p.m. EDT

Linux application compiled all 354 translation units and linked successfully.
Tested executable SHA256: 539fbfc83d9f97bcfe2eaa38887b22867c7edba7deedb749a73ad0f757b5e8c3.

Runtime-confirmed through the normal player flow: Main -> Anakin Skywalker selection -> Watchtower Run selection -> loading -> original race intro/countdown -> real keyboard throttle movement -> Pause. Paired racer-selection and in-race screenshots were captured from the same run. The generated runtime root was absent; this GA route consumed the supplied original disc files.

The previous eligible-human AI combat boundary is repaired with source-derived candidate geometry, attack driving, state transitions, target observations and processed-contact ownership. The current run proceeded past that former failure point and reached Pause. Focused candidate/state/attack/contact tests and wide-steering assertions pass. This does not establish exhaustive combat coverage or retail scheduler parity.

Restart and return to Main are still being tested at this publication checkpoint. Full laps, finish/results, all racers/tracks and Windows runtime coverage remain incomplete. FPS tuning remains lower priority than the normal user flow. Existing HOST-ordered contact scheduling remains explicitly qualified.

This source was reconstructed after a cloud workspace reset, from verified repository and saved source inputs, then reviewed, rebuilt and freshly tested. Older results below are historical and do not supersede this checkpoint. No original disc assets or generated data packs are included in this source publication.

## Next work
Complete the live Restart/return checks, then restore and test the TA direct-disc player route. TA static CPU loading is checked in a separate proposal, but that live-route proposal is not included here. Keep two same-run proofs per GUI run, and publish meaningful tested wins with their exact verification limits.

---

# RacerRevengePC handoff

Updated October 17, 2026, 12:33 p.m. EDT / 16:33 UTC.

## Current result

The requested Linux build and startup test passed. Main -> Mawhonic character select -> Ryloth Brightlands -> actual ALL8 racing completed, with character-select and gameplay screenshots. The tested Ryloth route reads the user's original disc resources directly. It needs no externally generated game-data pack, historical collision corpus, Full48F cache, or image-cache files.

This is short startup/gameplay-entry validation, not a full lap/race, comprehensive functionality test, Windows test, or performance improvement claim. Other track routes have not been converted to the disc-only path. Windows source has an in-process decoder loader but has not been compiled or tested in this change.

## Repository state

Repository: https://github.com/GTTeancum/RacerRevengePC
Base main: 644f7708df71e3fe7f58fd53fb6237a91d021f8f.
All 25 other branches were already ancestors of main when inspected. There was no master branch and no outstanding branch content to merge; branches were preserved.
The changes described here are local and have not been pushed. The patch package applies against that base. Do not claim the GitHub branch contains these changes.

## Tested binary and evidence

Linux executable SHA256: 762acaea811f4b59efc76c2f14f2268f7a3ba33cd3f699674acaa9dd00bad324.
354 translation units compiled and linked, with dependency hashes rechecked before linking. Canonical command generator: tools/prepare_interactive_build50dv.py. Existing GCC Controls-model object-size warnings remain; build exit was 0.

Successful test ended October 17 at 12:31:52 p.m. EDT / 16:31:52 UTC:
- Exit 0; completed=true; timed_out=false; cleanup_complete=true.
- 640x448 gameplay capture: Mawhonic9, RA, race phase2, world tick4.
- Final: 32 ticks, 242 render commits, 241 swaps, 1,296 native calls.
- All eight original AI dispatched; zero human racers, keyboard packets, or host-driver calls.
- Contact handling: 31 passes, 1,736 tested pairs, zero reported failures.
- Software-rendered baseline: 0.568661 FPS, three completed swaps across 5.27555 seconds, after 7.48086 seconds excluded warmup. No hardware performance inference.

Earlier attempt hit its 300-second budget during the slow software-rendered intro/countdown and exited3 with clean cleanup. The successful attempt used the existing RA route's supported 900-second budget and finished in 411.255 seconds. No intro, timer, AI, or capture readiness was fabricated or bypassed.

Additional tests passed:
- In-process IPUM decoder matched 198 reference images byte-for-byte: 144 rider mips, 48 RA sky tiles, six crowd frames, on both build and GUI hosts.
- Original POD01 geometry/PHY/COL bootstrap, typed query bindings, failure atomicity and malformed archive rejection.
- Complete bootstrap Fixture and direct Rig/Visual construction for all 13 supported original pods.
- Six shared EMT effects loaded directly from POD01.RES and matched exact existing hashes.

## Implementation

RA bootstrap now obtains original POD01 PSG/VUT/MOT/PHY/COL in memory. Source-derived query metadata replaces the dependency on historical JSON fixtures; the existing mutable development arena and explicit host opaque-W policy remain documented.

Direct roster owners authenticate RES/PHY, retain original members, decode original scenes/materials/textures, resolve original names, and construct cables without fabricated Full48F provenance. Course admission verifies the original archives and restricts this direct route to RA. Shared effects load from original particle members.

Rider, sky, and crowd images decode in process with the existing source and pixel hashes still enforced. No Python or ffmpeg CLI is used at runtime. New entry: executable --assets ORIGINAL_DISC_DIRECTORY. Main defaults to explicit muted audio only if no audio policy was supplied; the automated ALL8 test uses its explicit original route settings.

The decoder currently requires matching FFmpeg7.1.5 native libraries: libavcodec61.19.101 and libavutil59.39.100. Linux loads installed libraries; Windows loader expects matching exe-adjacent avcodec-61.dll/avutil-59.dll with restricted search. Official public headers and notices are retained under third_party/ffmpeg-7.1.5. No codec binaries are included. Release library packaging/license review and Windows validation remain separate unfinished work; do not describe this as a turnkey shipping release.

## Retained workspace

Root: /workspace/scratch/780c66634d06/racer-linux-test
- source/: modified build closure; not a Git clone.
- source-pristine/: 2,179 original files reconstructed and Git-blob verified against the base.
- build-disc-direct/: command.json, build.py, link.py, object/dependency receipts, result.json, tested executable.
- disc-only-capture-03/: successful launch settings, logs, screenshots, individual receipts, FPS baseline and window-evidence-summary.json.
- disc-only-capture-01/02/: immutable prior failures.
- ipu-parity/, bootstrap-check/, effect-check/, roster-check/: focused tests and evidence.
- incoming/: all18 uploaded split volumes, preserved.
- extracted/: original BIN/CUE plus disc/ with 2,314 files (498,622,713 bytes).
- private-input-manifest.json, archive-extraction-receipt.json, disc-inventory.json: private input identity/recovery.

Original SLUS_202.68 SHA256: c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Original RA.RES SHA256: 1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2.
No original-game executable was executed. Do not publish original disc, extracted assets, generated proprietary textures, or private Library input manifests to GitHub.

## Resume

Read this handoff before old PERFORMANCE_HANDOFF content. Preserve the successful evidence and input archives. The exact successful invocation is recorded in disc-only-capture-03/launch.json and run_disc_capture.py; it supplies original roots and the supported Mawhonic/RA ALL8 route, but no generated-data or rider-cache root. GUI testing uses the cloud Linux desktop; its window manager required resizing both Main and race windows to640x448. The user did not authorize deployment to their PC.

Remaining project work: publish reviewed source when authorized, integrate the new dependency in release packaging, verify Windows, and convert other track paths before making a whole-game no-data-pack claim. Keep technical claims separate from full functional or performance parity.
