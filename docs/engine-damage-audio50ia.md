# Original player-engine damage modulation, 50IA

## Outcome and boundary

The player engine path previously hardcoded scale484=1. This integration supplies
the original changing scale using existing native damage7D0/7D4/7DC, actual
frameFactor, and the same raw RNG already consumed by physics, material audio
and speech. This is a modulation route for the existing original engine PCM,
not another sample or invented damage sound. Binder pitch is untouched.

The helper is called only where the selected player's existing engine update
already runs, after that native frame reaches Ready and before warnings. It
retains the client's explicit post-frame subscriber timing. It does not claim
to restore the full original D40 object or emitter-manager scheduling.

## Observed integration result

The 2026-10-09 check began at 13:17:03 UTC and compiled one unit, reused 192
and linked successfully. A single native race launched at 13:18:01 UTC on
source `c10a655a2764314f485998318dd03ad644cdaa5d` and exited normally.
Player completed three laps at 160.928 seconds; all finish-continuation criteria
passed, including continuing native owners, sealed results, rejected post-finish
manual takeover and retained opponent rendering/audio.

The last periodic modulation sample was 10,410 updates, 20,820 shared RNG draws,
zero dips, zero returns and zero completions, with scale484=1 and cooldown474=0.
This is a build/live-integration and healthy-state pass. No positive damage dip,
outward/return curve or damage-triggered cooldown was exercised.

The original mix finalized to 174 seconds of PCM (SHA-256
`e3518ac2b3626a4877e9584a72ecbeaadac5c45a57f0e02d151fb666c4eb1f0a`).
Completed-render FPS ranged from 52.0452 to 62.5956. The binary SHA-256 was
`342850cf7d1cbb510eac72f3e1ffc222e18baf078bb4a6a5acfb9cba4bbf7dae`.
Neither the recording nor these counters establishes listening quality.

## Original numeric behavior

- Constructor: cooldown474=0, scale484=target490=1, duration494=remaining498=
  velocity49C=acceleration4A0=0, low flags4A4 bits cleared.
- Cooldown decrements first and clamps below zero to zero.
- Active dip integration runs before new selection. Remaining time determines
  which half of the acceleration curve runs; scale advances using the updated
  velocity. At outward completion a return-to-1 curve starts with the same
  duration. At return completion scale is exactly 1 and both low flags clear.
- At zero cooldown, source max(damage7D0,damage7D4,damage7DC) is used. Damage7D8
  is deliberately not read by source2234E0.
- Two candidate draws occur even when damage <=0.3: duration is an integer
  50..75 divided by100, target is an integer50..80 divided by100.
- Damage >0.3 starts that curve. Damage >0.7 draws a 1..3-second cooldown;
  otherwise it draws a 5..8-second cooldown. Exact 0.3 produces no dip and
  exact 0.7 selects the longer interval. The third draw happens only on a dip.
- Raw RNG uses the existing308030 LCG, preserving all32 stored bits and
  masking only the returned31-bit result. No seed or independent RNG is added.
- Source scalar operations preserve separate binary32 rounding; compilation
  must retain the existing no-fast-math/no-contraction/nearest-rounding flags.
  A rare rounded draw outside the source asserted range rejects atomically.
- Rejected state/input/numeric output does not commit either state or RNG.
  The helper has fixed scalar storage and performs no allocation or I/O.

There is no new audio slot, cue, mixer call, decoder path, volume tuning,
sample-rate change, synthesizer or replacement PCM. Existing integer engine
pitch truncation remains in podEngineControls50HE.

## Source map

Retained original executable identity:
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

The already-retained static evidence is under
`/workspace/scratch/74cd1d3d7a45/restore50hs-evidence/archives/50HK/evidence/repository/solo-audio50he/`:

- `audio_construct-static.txt`:19EBCC and19EC88..19EC9C constructor numbers;
  19EEC8..19EEF4 cooldown;19EEF8..19EF20 integration-before-pitch call order
- `engine_update-static.txt`:19F6B0..19FB84 candidate draws, thresholds and
  cooldown;19FB88 engine-only multiplication by484;19FBF0..19FD14 integration;
  19FD20..19FDAC target/duration/acceleration setup
- `damage_average-static.txt`:2234E0 retained max-damage read set
- `source-manifest.json`: original executable/range identities

These existing text excerpts were read directly during source recovery; raw
instruction evidence and original assets are not included in this source export.

## Lifecycle and integration

The selected-player state lives beside the current speech state, outside the
opponent snapshot bank. Opponent/race-setup frame guards prevent mutation, so
no copied opponent state can replace it. It advances exactly with the existing
player engine subscriber. Pause, countdown and stopped player do not advance it. Finished-player
frames continue through the recovered finish-control owner, so this modulation
also continues on those accepted frames.

In-process restart constructs a fresh session and fresh constructor state.
Ordinary pause keeps its current curve. The finish transition preserves engine voices and the single shared mixer;
there is no global finish-time voice reset.
The host's separate recovery/restart policy remains unchanged.

Only the player's route is connected. Opponent engine input still explicitly
uses scale484=1. Their own independent emitter-local modulation state and exact
per-opponent subscriber phase are a separate extension.

## Remaining focused verification

The integration run observed healthy draws but did not isolate the numerical
boundaries. Cover constructor/healthy draws, exact0.3/0.7 thresholds, both cooldown ranges, outward
and return completion, shared-RNG ordering, alias-safe commit/failure, invalid
input, source-range rounding failure, and allocation-free repeated updates.
Confirm binder pitch remains unchanged and original engine PCM is retained.

The shared RNG sequence intentionally changes; a previous identical seeded
race is not expected to remain byte-for-byte equivalent. Check direct controls,
damage/repair, pause/resume, finish/restart, and the single shared mix boundary.
Monitor engine pitch admission and existing clipping counters. No listening
quality or full-game sound-coverage claim follows from a successful build.
