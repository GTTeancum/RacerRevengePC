# Original Game Options bounded capture candidate

Status: source-only candidate, not executed, built, syntax-checked, parsed, or tested. The active repository was not edited. The centralized next test may not start before 2026-10-11T03:17:10.251731Z, and only root owns that hourly slot. This package does not authorize a run.

## Files and integration

- Changed file: `files/tools/gameplay/record_original_ui_navigation_30hz_recovery.py`
- Exact replace preimage: `preimage/tools/gameplay/record_original_ui_navigation_30hz_recovery.py`
- Unified source patch: `candidate.patch`
- Reference-only original receipt entry: `reference/original_ui_main_entry50kg-b4a2d96.h`. This is the initial b4a2d96 receipt reference, not a C++ replacement. Root has subsequently changed startup/light ownership; no native entry or receipt producer is changed by this package.

Recorder preimage SHA256: a341e7ca175ebbd075c2731b8c46a43a8e69a878cf98e6c139172d4aae4c0f54

Candidate recorder SHA256: 5be063cffd13bef90e7b89034154aba7e9cb23aa6032578d110fe6b4a24377f5

Unified patch SHA256: 1af6e9b15d4e5f2b62c94b3f1fbd9e94de55ec158af573a72caa662ad43f2800

Apply only the changed recorder if its active preimage still matches. Do not restore the reference entry. Root must build the actual integrated native source and supply that reviewed executable's SHA256; runtime source hashes alone do not prove executable build provenance.

## Finite controls and expected observations

All commands go through the original UI's existing host-key mapping. Each down edge has its own committed-image/state capture; only that receipt authorizes its subsequent up edge plus a separate capture. The next press requires a fully open, observed exact checkpoint.

1. Main: Down, Down, Down, Enter. This selects Main slot 4 and enters Options.
2. Options: Down, Down, Enter. This selects Options slot 3 and enters Game Options page 4.
3. First edit: Down, Right, Down, Left. Sound group 2 changes source mode 1 to 2; SFX group 3 changes 50 to 40. The process profile and sound revision must remain unchanged.
4. Escape, Enter. Cancel remembers group 3, returns to Options slot 3, and re-entry must restore local sound mode 1 and SFX 50 from the unchanged profile/revision 1.
5. Second edit: Left, Up, Right. From saved group 3, SFX becomes 40; select group 2 and set mode 2 again.
6. Enter, Enter. Accept from group 2 must commit mode 2, SFX 40, saved group 2, Options accept count 1, and actual application sound revision 2. Re-entry must show those values again.
7. Escape, Escape. Leave the verified page without further edits, then return to fresh Main at saved slot 4. Shared application/profile values must remain accepted; per-Options-session accept/revision fields legitimately return to 0 because that session is gone.

This is exactly 20 fresh presses, 20 release barriers, and 21 settled checkpoints. Current route, requested route, page, switch count, saved slot/group, local values, profile, and source Apply counters are checked rather than assigned. The original Auto-save/split/vibration local flags and packed profile flags remain unchanged; this does not attest their external consumers.

## Native ownership and transition details

- Admitted routes are only Main4/1 and Options12/{1,4}; world ticks and race mixed frames must stay zero.
- Options page changes keep screen-switch count 1. Returning Main has switch count 2.
- Main and Options menu controllers detach controller 0 as group -1/slot 0 during their closed-input incoming entry phases, including the precisely identified initial pre-first-tick bound receipt. Only those measured entry phases permit this transient, which can never authorize a key. Game Options attaches its saved group in openingAC; Main attaches directly at its saved slot in openedB0.
- GPU tokens are per MaterialTextureGl owner, not globally unique. Each screen lifetime must have a nonzero stable token; Options page changes must retain its token. Capture/state receipts must agree. The recorder does not claim that token values alone identify different native bank objects.
- Both source closing and destination opening observations are required for each requested route transition. Missing sampled transitions result in partial evidence even if later checkpoints were reached.
- Capture/state matching includes the source accepted-update sequence, route, requested route, navigation, lifecycle, GPU token, and audio time/count fields.

## Audio and bounded resources

Audio is enabled through the genuine original UI transport with `RR_AUDIO_DEVICE=0`, `RR_ORIGINAL_MAIN_AUDIO=enabled`, and the supplied original AUDIO root. The native child writes `original-ui.wav` inside its new output directory. No audio source is synthesized or injected.

- Capture requests: at most 30 Hz and 1,220 captures.
- Live window: at most 40 seconds from actual first Main readiness; normal success stops after one second of stable returned Main.
- Every report after the complete return must remain at the exact open final checkpoint, and an actual captured endpoint at least one second later is required. Merely waiting a second does not establish this tail.
- Startup/acknowledgment/navigation/quit waits retain explicit upper bounds. Failure or late receipt never launches another child.
- WAV: at most 3,360,000 stereo 16-bit frames at 48 kHz, or 13,440,044 bytes. The child alone gets a hard RLIMIT_FSIZE ceiling of that size; core dumps are disabled. A limit failure preserves partial files and fails the observation.
- Preserved verified PPMs: at most 1,049,413,500 bytes. Raw log/event/frame-log limits remain 32/64/16 MiB. Stdin is limited to 16 KiB. Each launch manifest and summary is at most 2 MiB.
- Named evidence has a conservative byte bound in launch.json including hard-limited live/partial child files. Native home/tmp/cache directories are separate from this named-evidence bound; all individual child files share the hard file-size limit.
- Exact sample counts must agree between the source mixer, output, captured WAV, accepted source nanoseconds, and source microsecond clock. Physical-device counters must remain zero.
- After successful owned-process close only, the recorder compares the 44-byte PCM WAV header and file length to its final receipts, then hashes the bounded file. It does not decode, audition, mux, interpolate, or resample audio/video.

On any failure the existing bounded quit/SIGTERM/SIGKILL cleanup remains. Raw log prefix, preserved PPMs, frame records, first rejected receipt, source-state diagnostics, and partial WAV are retained. No file is erased or presented as a substitute successful run.

## Root-owned eventual invocation

The existing CLI contract is retained: one native ELF path, required reviewed SHA256, new output directory directly below `/workspace/shared/recovery-20261010/ui-navigation-recording`, `--root-reviewed-one-check`, and `--not-before 2026-10-11T03:17:10.251731Z` or a later permitted slot.

The candidate does not establish that the Options preparation fix works, that Accept succeeds at runtime, or that imagery/audio are correct. Those remain outcomes for root's one focused test and subsequent review of real artifacts. Original-AI-all-racers remains 1, autodrive remains 0, and no gameplay is requested.
