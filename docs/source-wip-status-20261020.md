# Tested source checkpoint: October 20, 2026

This cumulative source work-in-progress checkpoint advances published baseline 91f4f092 to the frozen shared-selection/load v22 snapshot. It includes earlier TB/GB/MC work and shared selection, initialization, camera, audio, renderer and source-backed UI timing corrections. It is not a complete release or full-fidelity certification.

## Watto bounded human flow and roster milestone

Watto (racer 25) / MC passed GUI29. The observed flow was original Main to Racer to Course, physical acceleration and steering, Pause, Restart from the grid, renewed acceleration, return to original Main and normal close. HUD speed reached 302 before Restart and 338 afterward; these are observations, not performance benchmarks. The process exited 0 without timeout. Original disc inputs were used and the generated runtime root remained absent. Same-run selected-racer and restarted-driving proof was retained and delivered. The pass was recorded on October 20, 2026, at approximately 22:35 UTC.

Across the retained tested snapshots, all 23 racers now have a bounded human gameplay-flow observation. This is cumulative racer coverage, not all 23 racers retested on v22, every racer/course combination, or proof of complete races. GC still lacks a complete bounded human track-flow pass; historical GC AI evidence does not fill that gap.

The exact tested v22 Application SHA-256 is 41b2b850c55eb9da68ac73557c59b9f1df304e023a1c5de8a12aafb205216dec. Its full build verified dependencies for all 359 translation units, compiled one fresh object, retained 358 verified objects, completed the link and passed its final postcheck. Build and subsequent GUI success are separate evidence.

## Source-backed timing correction and clock domains

Recovered original 0x140AC0 behavior computes an uncapped update quantity, then caps the dispatched binary32 update factor to 0x3d75c28f, with an optional 0x3d08850a cap. Verified original-code evidence traces propagation through the root, UI controller and screen manager to Main. Both native frontend loops now send that recovered factor to exactly one actual source update. Existing admissions receive the capped factor; there are no synthetic source ticks, repeated controls/events or hidden source catch-up updates.

The actual accepted wall interval remains separate for PCM and existing uncapped material time. The host elapsed helper preserves measured intervals longer than one second and records a long-interval flag instead of treating that duration as fatal. This corrects the time-domain contract; it does not establish faster rendering or original console clock sampling. First/resumed zero-delta and cold/focus accounting remain native orchestration, not new original-lifecycle claims.

PCM retains the full raw interval through a prevalidated plan preserving nanosecond and sample carries, using existing at-most-one-second transport calls. Failed irreversible work records its completed prefix and latches a fault rather than replaying. Existing device-ring overflow policy remains, so exact mixed/WAV time accounting does not guarantee lossless audible catch-up. Focused numeric, carry/failure and call-site tests accompany the correction; no renderer optimization or additional GPU work is claimed.

## Earlier evidence and failures remain bounded

Darth Vader/MC previously passed GUI26 on frozen v19, including physical driving, Pause/Restart, renewed driving, Main return and exit 0. Earlier Sebulba/TB, Kraid/GB, Young Anakin/MC, Episode I Sebulba/MC and Darth Maul/MC passes remain scoped to their own tested snapshots rather than full v22 regressions.

Watto GUI25 failed during Racer selection when native submission exceeded the then-active one-second policy, with no race or gameplay credit. Subsequent failed attempts are preserved, including GUI28's accepted renderer frame followed by timing-policy rejection. GUI29 is a new bounded success after the timing correction, not a relabeling of earlier failures. Likewise, Darth Maul GUI12's D48 initialization failure and earlier Young Anakin camera/audio failures remain historical failures followed by separately observed successful runs.

## Remaining verification and distribution limits

Lap completion and finish/Results progression, full visual/audio and behavior fidelity, physical controller/hardware feedback, original hardware rendering/timing equivalence, every racer/course combination, and complete failure/teardown behavior remain unverified. Selecting three laps does not mean completing them. No smooth-frame-rate claim is made. Existing native policies and MC water-surface limitations remain. The remaining GC human track-flow gap is separate from the completed cumulative racer milestone.

This checkpoint does not certify that the default repository CMake path builds the complete tested Application. Source integration, focused checks and bounded gameplay do not constitute a full release test suite.

Only source and this documentation are included. Original archives, loose/decoded/generated asset payloads, binaries, object files, Python bytecode, private inventories and runtime recordings are excluded. Users must supply their own original game assets. Existing regression measurements keep their non-independent status; no independent-reference claim is added.
