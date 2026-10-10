# Optional audio/countdown phase observation

Source-only, unintegrated, uncompiled and untested overlay on combined source d3d854b6. It appends countdown_service and audio_commit observations behind the existing RR_TRACE_PHASES=1 flag. Existing phase values are retained. No scheduler, timestep, sample-count, event order, catch-up cap, pause or voice-timing change is intended; clock-read overhead is unmeasured.

Countdown service ends before the actual shared mix. Audio commit covers existing mixer/WAV/device-ring work and subsequent source music/announcer service at its two existing call sites. Post-physics event dispatch, active-world opponent-audio control, external Python work and worker-thread CPU remain outside these scopes. The logger does not flush the final partial interval. Phase totals are not complete whole-loop accounting.

This overlay observes missing work; it does not fix simulation/audio time loss under the existing host catch-up cap. Proposed main SHA-256 is 0946d653f29e4fb7836ae8b950b9bf23c7a425c179dcbbff76ae014b267a5a07, and proposed timing header is f7b31fbad8f8dd29e50c65698cef59a34a875f46128d220c2cfabf7283453fdb. The current collector pins the prior hashes and must be reviewed/rebound if this source is later integrated. No active files, collector pins, test reservations or evidence were changed.
