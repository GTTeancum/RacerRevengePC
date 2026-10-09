# Host finish presentation 50HZ

The interactive host holds native bodies at their actual finishing positions.
Submitting every held opponent left a pile of vehicles over the finish camera.
This change is an explicit host presentation policy, not recovered original
finish behavior or a collision-physics fix.

## Submission policy

- The player display is unchanged, including after the player finishes.
- An opponent is omitted only when its committed race-session `finished` flag
  is true. `race_session50HM::commitFrame` seals that flag and its finish place
  after an accepted final-lap increment. A pending lap, zero speed, inactive
  bank slot, failed native frame, or the player's finish does not hide an
  unfinished opponent.
- The host compacts the submitted opponent descriptors while retaining each
  original `racerArchetype`. The existing renderer draws vehicles, cables,
  transparent flame geometry and footprint-shadow casters only for submitted
  descriptors. Its per-frame shadow reset/count excludes obsolete casters.
- Hidden opponents do not prepare or commit flame presentation callbacks;
  their bank-local flame state and source-shared RNG remain held. Continuing
  racers retain their own callback/RNG path. Stopped unfinished opponents still
  submit their last complete display batch. Zero remaining opponents use the
  renderer's required null pointer/zero count.
- All eight named standings rows still come from the complete committed race
  order. Finished names and sealed places stay in the standings.

No body, pose, collision geometry, source lap/checkpoint counter, race clock,
finish time/order, contact scheduling, or audio path is changed. Remaining
unfinished racers continue through the existing native step and race-commit
path after the player finishes. The shared mixer and finish speech continue as
before. `RR_RACE=0` retains its existing single-player behavior. Restart creates
a new session with no retained presentation counters or hidden racers.

## Diagnostics and verification boundary

Startup identifies this host policy. The periodic `race` diagnostic preserves
`opponent_draws` (accepted opaque opponent commands, not visible pixels) and
adds the full `field_racers`/`field_opponents` counts and `sealed_finishes`.
`presentation_valid` marks whether a render transaction has been accepted;
`opponents_submitted`, `opponents_hidden_finished`,
`opponents_retained_stopped`, and `standings_rows` describe its last accepted
transaction. Stopped-retained opponents are a subset of submitted opponents.
Submitted plus hidden-finished opponents must account for the full seven.

Source-level expectations: countdown submits seven opponents; finishing an
opponent reduces the submitted count by one without removing its standings
row; finishing the player alone does not hide anyone else; a stopped unfinished
opponent remains submitted; all opponents finished produces zero opponent
submissions and eight standings rows. Pod identity is preserved when finishes
leave nonconsecutive racer indices in the submission array.

The 10:16 UTC Linux check on 2026-10-09 completed the full field and one
restart. The accepted finish frame had zero opponents submitted, seven hidden
finished and all eight standings rows. Restart restored seven submitted, zero
hidden, reset laps and all eight moving. Both captures were visually reviewed.
Native simulation remained unchanged; this is presentation coverage.
