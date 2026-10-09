# Pending native track selection

This is host-native presentation and session admission. It does not reproduce
the original scene/menu sequence or claim that an incomplete world is complete.
The explicit menu list contains TA and conditionally GC; the other ten original
SCB metadata records are not runnable choices.

## Admission

- No RR_TRACK request still selects TA. TA retains the existing normal mode 1
  default and does not require an incomplete-world flag.
- Without RR_GC_INCOMPLETE_WORLD=1, the Tracks page shows only TA and Back.
- With that opt-in but without explicit RR_RACE_MODE=2, GC is visible as
  `GC UNAVAILABLE: REQUIRES MODE 2`. Enter acknowledges the row without changing
  either track or mode. The menu never changes RR_RACE_MODE.
- With both explicit flags, choosing GC opens a Cancel-first disclosure page.
  It names windmill animation/type1 collision, the triggered opening, authored
  LostAI branch triggers, Gate sound, and mode 1 record configuration as missing.
  Only a subsequent Select action changes the pending track. Cancel/Escape leave
  it unchanged. The existing cold GC incomplete-world and mode 1 guards remain.
- Explicit RR_TRACK still passes through the original strict asset lookup once
  at process entry. Invalid/unbound tracks fail rather than loading TA.
  A GC startup also requires both flags, including in free drive.

## Lifetime

main owns effectiveTrack and selectedTrack. runSession receives effectiveTrack
by value and captures loadedTrack before any menu input. The exact source asset
lookup consumes that loaded code instead of rereading RR_TRACK each restart.
No process environment is mutated.

The menu receives loaded and pending identities plus immutable admission flags.
Only explicit cold Start/Restart commits selectedTrack. Checkpoint recovery
retains effectiveTrack. Changing a track row alone never destroys/rebinds the
world, changes native state, advances a clock, or changes any audio bank. Returning
to Main uses the existing stopped-session semantics. The next Start cold-loads
all source providers for the selected track and fails on any missing provider.
The original GC projected-scene/material/audio interfaces remain untouched.

## Input and telemetry

Tracks is appended before Exit on Main. Existing Controls and Audio row indices
are unchanged: in a normal race with audio and racer selection, Track is row 4
and Exit row 5. With audio disabled, Track is row 3. All rows use existing fresh
arrow/W/S and Enter/Space edges. No new input command bypasses the menu.

Page numbers 0 through 7 are unchanged. Tracks is 8; the incomplete GC disclosure
is 9. Tracks starts on row 0 (TA), GC is row 1 when visible, and Back is last.
The disclosure has five informational rows, then Cancel at 5 and Select at 6.
Up/Down toggles only those two actions. Movement plus activation in one drained
snapshot cannot confirm the action.

Existing host_ui_state ready/transition/capture JSON adds loaded_track,
selected_track, track_pending, loaded_track_incomplete, gc_incomplete_opt_in,
and gc_mode2_admitted. The last field reports the explicit mode-2 flag only;
GC admission additionally requires the incomplete-world flag. Existing session,
audio_bank_id, tick, mixed_frames, audio settings, and results fields are retained.

Expected future central checks (not performed):

1. Default TA/mode 1: Tracks has no GC row; Back returns to Main.
2. Opt-in/mode 1: Enter on GC leaves selected_track TA and loaded_track TA.
3. Opt-in/mode 2: first GC Enter opens page 9 with Cancel selected; cancelling
   returns page 8 and leaves both track fields unchanged. Escape then returns Main.
4. Reopen GC disclosure; select the second action. Main reports loaded TA,
   selected GC, track_pending true with unchanged session/bank and clocks.
5. Start performs exactly one cold rebuild. New ready reports loaded GC,
   selected GC, pending false, incomplete true; original GC provider checks apply.
6. Main -> Tracks -> TA -> Start returns through one cold rebuild to TA.

Source-only staged work. No build, syntax/import check, test, game, decoder,
rendering, or capture was run for this patch.
