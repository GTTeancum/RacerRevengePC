# Independent static review

Reviewed on 2026-10-11 (UTC). No blocking defect was identified in the reviewed source. This is a source-only review, not a compilation, launch approval, or runtime result.

## Scope and method

Read the complete staged recorder and diagnostic patch, then re-read the final refinements. Compared the staged native header with its retained preimage. Cross-checked the native capture/closed report code, frontend transitions, Rider session/profile/selection/panel owners, LightManager lifetime, host stdin handling, source renderer generations, and audio output/sample-clock implementation. Only source-reading shell commands and cryptographic file hashes were used; no project parser/decoder, import, syntax check, build, test, sanitizer, game process, or project script was executed. This review file is the only reviewer-authored artifact.

## Findings

- The native header change appends read-only observations. It does not alter routing, readiness, profile mutation, rendering, RNG, or PCM advancement. Main mode 4 legitimately makes the Rider selection snapshot unreadable; the report preserves that fact with `null` remembered fields.
- Startup Main's detached group −1/slot 0 is allowed only before settled input. Main's settings defaults, profile fields, and LightManager generation 1 agree with source. Settled Main requires the first slot and Song 9.
- The requested route is Main 4/1 → Rider 5/1, rider 19 → Main 4/1. Five fresh presses have separate image/state and release barriers. Each of Left, Right, and Enter must independently add exactly one unavailable result while reported profile, selection, bank, registry, and light generations stay unchanged.
- Rider slot 1 is supported by retained `provenance/rider.dat`: Group 1's first control is 16 and Control 16's PodIndex is 19. Its 6,495-byte SHA-256 matches the compiled archive catalog entry. No archive extraction or authentication code was executed during review.
- Back intentionally performs the remembered 19→19 write and increments Rider profile generation 1→2. LightManager generation must equal switches+1 across all three screen lifetimes. Successful Main return is correctly described as passing the source-required retirement gate, rather than an independent retirement receipt.
- All screens use `original_main_capture`; Rider uses null ACK selected_group and the following `original_rider_state`. During active recording, the recorder requires immediate matching report prefix, pairs frame/update/route/audio fields and Rider geometry/generations, and preserves the atomically published PPM before another capture request. After stop/failure, the prefix guard is disabled so a stale pending ACK does not obstruct later cleanup diagnostics; this does not waive frame pairing or success checks. Strict increases in both Rider source frame and target generations agree with the renderer implementation.
- The three-second idle, rejection dwells, and return tail use accepted source time. Each image record preserves its receipt's PCM boundary. Device-free 48 kHz stereo 16-bit WAV accounting, nanosecond remainder, source/output/captured counts, and finalized RIFF size are checked against native reports. No audible-quality or measured 30-fps claim is made.
- The recorder has one native `Popen`, one outstanding capture, bounded output and deadlines, explicit quit, and bounded TERM/KILL failure cleanup. Command allowlisting excludes resets, restart, gameplay, repeated entry, and other navigation. Every state and final closure require zero world ticks.

## Remaining limits

Source review cannot establish successful compilation, runtime feasibility, achieved capture cadence, visuals, audible quality, or executable/build correspondence. Build provenance and final ELF/header hashes still need verification by the authorized launch owner. Reported invariants do not expose all internal state or physical pointer identities. Transition sampling may legitimately fail the bounded observation rather than fabricate missing frames. The recorder's time gate does not independently enforce a shared hourly launch ledger.

## Reviewed SHA-256 identities

- `record_original_rider_30hz_recovery.py`: `0a71bc3ba46d63a88bb6f849bd8b5cdc26fb7dc31f71cb842cc41fdbf5963c83`
- `rider-observation-diagnostics.patch`: `f8d3fa27ebe0d1ac8f821cd7c5e297f822541351ccbb075791381c64c30d2d8c`
- `files/tests/native/original_ui_main_entry50kg.h`: `0556a4d4469ea81fe8be69c0ff9a77cd03df95ce5ad868e6dc6f1e617589dc24`
- `provenance/original_ui_main_entry50kg.h`: `b2869f529434320c198bb73a2634a6311677329d265ba7cf78b8856b79e55f7c`
- `provenance/terrain_interactive_drive50du.cpp`: `c31c33a2f72ddd43c90372ca70100bce3266b8a2a3e860fd146df320ffa48d91`
- `provenance/rider.dat`: `fc02bd935bcc540f69137a403f11751f71d9f9b87d764d03cc0df0117cff7cb0`

Retained source-commit label: `5f650ed11fbe6b57cb62174eefdb84c68ef9ccb9`. This label is recorded provenance, not an independent assertion that an ELF was built from it.
