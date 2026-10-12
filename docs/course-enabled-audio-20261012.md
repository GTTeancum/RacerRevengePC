# Native Course audio join

Source-only change against `efd14bda7fec5459678d450df8d88e3db4ae0779`.
No build, test, syntax/import check, decoder, sanitizer, playback, or gameplay
was run for this change. Runtime acceptance remains pending.

## Concrete owners

- The existing application-owned `OriginalUiMainAudio50KG` and dedicated
  `AudioOutput` remain the only frontend transport, mixer, device and PCM
  timeline. Music voice 0 now plays original Song9 (Main), Song7 (Rider), or
  Song11 (Course), replacing the preceding music without overlapping streams.
- Course `openingAC` calls Song11/restart1 at the existing authored widget
  step for `1DA924`. Its `closingB4` step calls the existing one-second fade
  at `1DAAA4`. The shared native linear-envelope/retirement policy is unchanged.
- Course Left/Right use original UIEVENTS3/4. Accepted launch uses event1
  (`1DEF18`), and accepted Back uses event5 (`1DF0E0`). The existing six effect
  slots and stop-before-restart behavior are reused. A rejected destination
  does not submit a Confirm sound or commit profile changes.
- The Course update preparation now services the same shared transport once,
  before the manager tick. Accepted elapsed time still reaches PCM only from
  the application output owner. No extra clock, RNG, render-driven audio
  advance, synthetic sound, low-level packet queue, or guest execution is used.

## Rider19 Confirm

The existing source selector still owns the one real shared RNG draw. All five
possible UIACCEPT events39..43 are authenticated and prepared before playback.
After the route/profile commit, the actual selected original utterance is
submitted first, followed by character selection and the existing event1.
Explicit mute preserves the same choice chronology and suppresses only output.

One additional native voice (slot7) supports the bounded once-per-Rider-page
Confirm operation. Flags0 does not stop or replace an active utterance. Its
native PCM delivery uses unity gain, normal pitch and centered routing through
the existing sound-mode/mixer owner. A private playback receipt ties cleanup
to the exact transport and mixer playback revision; private destination
preparation cannot stop the current page's speech. The actual Rider teardown
releases that native voice. There is no guessed0.35-second Confirm stop.

This is native preload delivery of original authoring marked `IsStream=1`,
consistent with the existing native asset/mixer service, not IOP streaming or
its timing. Exact original stream-backend gain and destruction timing have not
been independently established by the retained listings. Those are fidelity
limits, not substituted assets or successful-playback claims.

The existing seven-slot transport remains supported for Main/Rider callers.
The application output allocates eight slots, and Course preparation requires
that exact extended owner. The steady-Rider inspector separately checks the
extra slot and refuses to certify Confirm speech as steady navigation. Its
six-element effects observation stays bounded to voices1..6.

## Input assets and handoff

Enabled Course prepares original `MUSIC/UI/SCREENS/UI_TRKS.VAG`, plus
`SPEECH/UI/A2ASEL01.VAG` through `A2ASEL05.VAG`. Exact SHA256/size records are
compiled in the existing transport. Original UI_MUSIC.JUK/JUB and UIACCEPT.INI
are authenticated. Expected PCM extents are derived from original framing;
the existing decoders must confirm those extents at the future cold run.

The optional Course archive is now discovered in either explicit audio mode.
Absent Course assets leave Main/Rider Back available. Supplied malformed
archives or requested audio bytes still fail explicitly. Muted mode owns no
transport and needs none of the added audio bytes.

The existing Course-to-race boundary closes/finalizes the frontend device/WAV
and resets its voices before race output opens. To avoid reopening an existing
exclusive-create capture, frontend `RR_AUDIO_WAV=menu.wav` produces
`menu-race.wav` for the Course-launched race. Existing later-session suffixes
remain distinct. Files are never overwritten or appended implicitly.

No source-list or build-system additions are needed. Native race destination
restrictions, all-original-AI ownership, other Rider selection restrictions,
and unrelated camera integration are unchanged.

## Retained source locations

- `recovery/20261010/ui-route-reconstruction/evidence/course-input.txt`
- `recovery/20261010/ui-route-reconstruction/evidence/rider-input.txt`
- `recovery/20261010/ui-transition-callback-reconstruction/evidence/course-aperture-hooks.txt`
- `recovery/20261010/ui-transition-callback-reconstruction/evidence/rider-entity-update.txt`
- `src/game/original_ui_rider_voice_choice50kg.cpp`

