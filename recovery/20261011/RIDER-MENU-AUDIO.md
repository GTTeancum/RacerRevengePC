# Rider19 menu audio: source-only candidate

Staged against `4801bf52d0a4e5f5954265c0e27451dc7a02409d` on 2026-10-11.
The active checkout is untouched. Six exact preimages and corresponding full
candidate files are retained in `preimages/` and `files/`.

No build, syntax check, project parser/importer/decoder, test, sanitizer,
gameplay, original instruction execution, or chip emulation was run. Original
audio framing was inspected with independent Python standard-library byte reads
only. No PCM was generated, mixed, listened to, or captured by this work.
Root owns integration and all later validation.

## Implemented bounded scope

- The existing shared `OriginalUiMainAudio50KG` transport can cold-prepare the
  exact original Song7 `MUSIC/UI/SCREENS/UI_PEOPL.VAG`. Preparation is optional
  and must precede all playback/mixing; Main/Options-only startup reads exactly
  its previous audio set. A failed optional preparation does not mark Rider
  prepared or alter the existing Main/Options PCM.
- Rider page5/1 opening calls Song7 on the SAME owned music voice used by
  Main Song9, after appearance and before Group2 disable. It restarts at the
  beginning and cancels any outgoing music fade, matching234900(restart1).
- Normal Back checks its actual event5 playback capability before the private
  profile/route commit, checks again during proposal validation, and applies
  the concrete native event once after the source Back field writes. It uses
  the existing original BACK.VAG and its original sound object's restart rule.
- Rider closing applies the existing real shared-stream234440(1.0) native
  fade after hiding the selected name widget. No second mixer, output device,
  music stream, sample clock, or duplicate frame advance is created.
- The same audio prepare-update boundary as Main/Options runs once before the
  external manager tick. The application alone produces PCM for accepted time.
  Pause keeps the existing source-clock/PCM behavior.
- Frontend and native startup admit enabled Rider only with the existing full
  Rider19 resource/profile/cache owners AND actual optional Song7 PCM owner.
  Explicit muted mode still requires no audio object.
- Rider diagnostic output now reports real enabled/muted status, Song ID and
  concrete sample-clock/mixed/captured-frame observations rather than a fixed
  `audio_muted=true` string.

This does not admit direction, another rider, page2, confirm/Course, reset
popup, original save import or active legacy command-buffer work. No character
voice playback readiness is claimed. Initial entry and normal Back do not
request one; exact voice/RNG evidence is in `voice-evidence/`.

## Native backend boundary

Original effects292360 ->292810 ->233410 eventually queue a low-level backend
play packet, then Rider Back1EA860 calls267BE0 to submit that packet. The
original -1 early return at267BF0 is real but is not proof that retail Back
always takes it: the preceding original play can increment E00 from-1 to0.

The native implementation applies effect5 directly to the actual native mixer,
so there is no guest packet pending and the retained projection E00 remains-1.
It must not flush the host device, wait for playback completion, advance PCM,
pretend an active original packet queue was handled, or suppress character
motion RNG. Existing active-E00 rejection is retained. This is a native audio
backend substitution, not instruction/IOP execution or timing equivalence.

The pre-existing shared fade policy is retained: linear gain on accepted source
update boundaries, zero after1.0 second, silent retirement after single-precision
1.0+0.35. Source234540 requests the target0/1000ms envelope and this retirement
margin. Original IOP envelope granularity is not claimed. No music-duration
timer certifies a transition or character-ready condition.

## Original-byte evidence

ELF SHA256:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Static VA mapping: `file offset = VA - 0x100000 + 0x80`.

Song7 is named by original UI_MUSIC.JUK and authenticated together with the
unchanged UI_MUSIC.JUB. Its original file is709504 bytes, SHA256
`2ac7db7a81ebcad40afa37998ac3e1dba16d5cd774d0da464db03faa411ed9e4`.
The zero prefix is0x7E0, payload353744 bytes per channel,22050Hz; each stereo
pair interleaves two original16-byte compressed frames. Both channel end flags
are at index22107, followed by one original padding block. The existing
production ADPCM decoder will supply619024 non-padding frames per channel.
This is an expected result from byte structure, not a successful decode claim.

The source-only candidate generalizes the already-used authenticated Main
stereo framing helper only for the two hard-coded original records. It still
calls the existing production mono decoder for deinterleaved compressed bytes,
then owns interleaved PCM. There is no fixture import, synthetic sample,
resynthesized waveform, generic unverified stereo decoder, or asset fallback.

`evidence/original-audio-byte-identities.json` records exact metadata and VAG
byte hashes/extents/header/flag observations. The copied prior static music
listings establish loop/restart, source group volume40/100, effects100/100,
and shared fade control. `voice-evidence/` traces Rider create/entry/input/
update, character lifecycle, response-emitter, effect backend and flush.

## Integration and later central checks

Integrate all six candidate files together after checking exact preimages.
No CMake/source-list additions are needed. The native output already borrows
the same transport and seven-voice mixer; optional Rider preparation occurs
at cold startup only when the explicit Rider cache was requested.

The help text in `tests/native/terrain_interactive_drive50du.cpp` still says
"authenticated muted Rider19/Back" in the preimage. Root should change only
that phrase to "authenticated Rider19/Back" when integrating; it was left as
a documented one-line integration edit rather than staging a full unrelated
large gameplay source file. Existing runtime validation documents must remain
historical; append new observations only after actual checks.

Suggested later central checks, NOT RUN:

1. Main/Options without a Rider cache never reads Song7 and retains prior PCM,
   routing and sound-mode behavior.
2. Optional Rider preparation refuses wrong/missing Song7, changed metadata,
   symlinks, late/live preparation and a stale/reinitialized mixer. No partial
   Rider readiness. The Main-only prepared transport remains intact on failure.
3. Main9 -> Rider7 -> Main9 uses one music voice and one PCM clock. Outgoing
   fade retirement cannot later stop a restarted incoming clip. Back effect5
   overlaps the actual music in its existing independent effect voice.
4. Initial19 opening submits one music request, accepted normal Back submits
   event5 once, closing submits one fade. Rejected, duplicate and stale Back
   proposals cannot emit sound or commit a profile change.
5. Enabled and muted routes use identical source update/render schedules and
   original common RNG values. Idle/closing motion draws remain chronological;
   no audio seed, voice draw, confirm animation, readiness wait or timer is added.
6. Pause and resume retain source PCM cursors and source time; audio frame
   diagnostics match the concrete output clock. Only an actual central capture
   can establish audible output and frame counts.

All new implementation and end-to-end enabled Rider behavior remain unbuilt
and unvalidated.

Root integrated these exact source candidates after the passing 04:18 Options observation. Rider audio remains unbuilt and unvalidated; that earlier observation does not cover this change.
