# Garage reentry: bounded v51 validation, 2026-10-24

## Scope and source
Candidate v51 is compared against published-v46 frozen source. The seven-file delta covers retained scene-group sound history, authenticated equal-byte font provenance after frontend reentry, diagnostic preservation, shared square/circle archive provenance, and queued retained-font atlas handling. This document records a bounded successful run, not comprehensive Garage or game correctness.

Expected publication base commit: 4efec7385eae23ad74d8e3a7b016dc23d338b83f.
Expected base tree: f2edc0365521eb0b2ba0df388bd808547290ce4b.
These base pins were supplied by the publication coordinator, not independently verified against the remote by this report worker.

## Build and independent evidence check
The build-51 terminal receipt reports PASS, 479 units, 2 fresh, 57.625535 seconds. Independently hashing the binary matched the launch receipt and build receipt:

3e77c773b509632a3c8a8222f97d30ff02278cb928efb0926d797492b6a4a015

GUI121 result.json reports exit 0, no timeout, elapsed 1233.203678 seconds, and generated runtime root absent. Independent read-only inspection of launch.json, result.json, stdout.log and stderr.log confirmed these recorded outcomes, two Garage screen-13 activations, successful resourcesReturned stages (status 0), subsequent screen-9 returns, a final quit at screen 4, and cleanup_complete=true. The worker did not rerun the program. Build and integration receipts retain runtime_tested=false because they predate this separate GUI validation; they are not standalone runtime-pass receipts.

## Bounded traversal observed by the test coordinator
In the same process, the coordinator observed:
1. Tzidik in normal Single Citadel, W/Up movement with HUD517, then Pause → Main confirmed.
2. Tournament with fresh Anakin → Career → R → first Garage rendering.
3. Escape/Cancel, then the authored closing Escape skip, returning to Career.
4. R → second Garage rendering, then unchanged Return/Accept and the authored closing Escape skip, returning to Career.
5. Back to Rider, back to Main, Tournament reentry with Anakin visible, back to Main, then a normal titlebar-X close.

Visual observations and exact input semantics above are coordinator observations, not independently reconstructed from the log alone. Saved PNG evidence includes selected-racer, in-race, garage-first-entry, garage-reentry, and career-after-accept. The result receipt’s required_same_run_proofs field names .jpg files, but the actual captured files are .png; the manifest pins the files that exist.

## Limits
This run supports only the described bounded cancel/reentry/unchanged-accept traversal. It does not verify purchases, changed Garage selections, upgrades, persistence across application restarts, all menus, all racers or tracks, a race finish, or audible/hardware audio output. Host lap state reports completed=0. Hardware audio was not verified; launch configuration had RR_AUDIO=0 and RR_AUDIO_DEVICE=0, and final device status was not_requested. Source audio bookkeeping or mixed frames must not be described as audible audio verification. The run used the recorded Tzidik test configuration and software-rendering environment, not unrestricted production-environment coverage.

Earlier GUI119/GUI120 failures are historical diagnostic runs and are not erased by this bounded result. This packet does not itself publish, authorize remote writes, or claim unrestricted Garage correctness. Publication and remote base-pin checks remain with the coordinator.
