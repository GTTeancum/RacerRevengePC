# Original opponent engine audio, staged 50HX

Status: exercised through full-field completion and one in-process restart
at 10:16 UTC on 2026-10-09. Original loops and race cues resumed in the new
session. Separate 232.912-second and 14.688-second WAVs were finalized, and
the first recording remained unchanged. Listening quality and clipping
improvements remain open. See current-validation.md.

## Exact source closure

The seven selected original PHY files each have a complete `[Pod Audio]`
section at lines 348–358. The helper authenticates the complete PHY bytes using
the original per-pod digests retained in
`evidence/originalRoster50HW/raw-imports/roster.json`, then parses all eight
required fields. Missing/unknown mappings fail preparation with an explicit
error. No Anakin defaults or replacement sample names are selected.

Display names below come from
`evidence/originalRoster50HW/original-eight-visuals/roster-visual-selection.json`.
The helper keys only on POD identifiers, never on a PHY name or display label.
All sample names are relative to the explicitly supplied AUDIO root.

| Pod | Display name | Engine | Binder | Engine idle/range | Binder idle/range | Engine/binder volume |
| --- | --- | --- | --- | --- | --- | --- |
| POD03 | Clegg Holdfast | sfx/race/gen1_ngn.vag | sfx/race/as_beam.vag | 0.5 / 2.12 | 0.5 / 2.38 | 100 / 90 |
| POD04 | Dud Bolt | sfx/race/gen2_ngn.vag | sfx/race/as_beam.vag | 0.84 / 1.89 | 0.47 / 1.59 | 85 / 100 |
| POD07 | Gasgano | sfx/race/gen2_ngn.vag | sfx/race/as_beam.vag | 0.84 / 1.89 | 0.47 / 1.59 | 85 / 100 |
| POD08 | Mars Guo | sfx/race/gen3_ngn.vag | sfx/race/sb_ngn.vag | 0.5 / 2.12 | 0.5 / 1.68 | 90 / 100 |
| POD09 | Mawhonic | sfx/race/gen1_ngn.vag | sfx/race/as_beam.vag | 0.5 / 2.12 | 0.5 / 2.38 | 100 / 90 |
| POD10 | Aldar Beedo | sfx/race/gen2_ngn.vag | sfx/race/as_beam.vag | 0.84 / 1.89 | 0.47 / 1.59 | 85 / 100 |
| POD11 | Ody Mandrell | sfx/race/gen3_ngn.vag | sfx/race/sb_ngn.vag | 0.5 / 2.12 | 0.5 / 1.68 | 90 / 100 |

SB_NGN is deliberately the authored binder for POD08 and POD11. The helper
preserves this seemingly unusual mapping. Shared engine families are likewise
original authoring, not clone substitutions. Five distinct VAG files satisfy
these fourteen voice mappings. Their files are present under the retained
`work/native/project/disc/AUDIO/SFX/RACE` tree; their new decode/playback path
has not been executed. File presence is not a ready-decoder or audibility claim.

The existing 50HC cold loader admits each exact name, deduplicates the five
requests before loading, and records missing/unsupported/invalid/ready states
without fabricated PCM. The new owner preserves original source rates and
embedded loop ranges, using the existing VAG decoder and mixer. A missing or
rejected clip remains unavailable without disabling a valid neighboring clip.
An entirely unavailable opponent bank is still an honest prepared negative
result. The existing player-bank/output opening policy is unchanged.

## Native ownership and mixer range

`tests/native/opponent_engine_audio50hx_fixture.h` supplies a noncopyable,
nonmovable `opponent_audio50HX::Owner`:

- `prepare(sources, audioRoot, mixer, error, policy)` is cold and one-shot.
  Each of the seven `Source` entries borrows the corresponding original rig's
  POD ID and retained PHY vector only for this call. The helper authenticates
  those bytes itself. Entries may be in any order but may not repeat a pod.
- Preparation owns one immutable decoded bank and fourteen borrowed clip views.
  Do not copy this owner or put its vector/string objects into the racer bank.
- Voice slots 96–109 belong exclusively to this helper, two per opponent in
  the caller's source order. Preparation rejects an undersized mixer or an
  already-active slot in that range. The owner never steals voices or changes
  the shared master gain.
- Existing ranges remain 0–59 player generic provider, 60–63 player lap,
  64–79 material sounds, and 80–95 race announcements. Slots 110–127 stay free.
  The integration patch adds compile-time range-separation assertions.
- `update(listener, inputs)` takes only fixed arrays and scalar inputs. It
  validates every active racer's inputs and planned controls before touching
  any mixer voice. All hot loops are bounded by seven racers/fourteen voices.
  Hot paths contain no asset lookup, string construction, file I/O, decoding,
  vectors, dynamic allocation, or shared RNG use. These properties are
  source-reviewed, not an executed allocation-proof claim.
- `reset()` stops only 96–109. `close()` also drops the mixer pointer; it is
  idempotent. Close/destroy this owner while the live mixer still exists.
  Its destructor is a fallback. Prepared PCM and statistics remain inspectable
  after close, but updates then reject as unprepared.

## Source controls and explicit host presentation policy

The existing `podEngineControls50HE` supplies the recovered speed-ratio pitch
law, separate original engine/binder parameters, integer pitch truncation,
and start gain `0.45 * authored volume / 100`. Base pitch 4096 remains the
existing native normalized representation, not recovered PS2 sound metadata.

The initial integration supplies the actual opponent `boostState.speedRatio70`
from its completed native snapshot. Opponent boost-pitch and randomized
damage-dip audio owners remain unjoined; it explicitly supplies false and 1.0
respectively. It never borrows the player's boost state or invents an opponent
boost event. Existing player controls, events and audio paths are untouched.

Distance attenuation is a clearly labeled HOST addition:

1. Listener and source are the committed player/opponent main-body positions.
   Three-dimensional Euclidean distance uses source world units, not metres.
   It is independent of render frequency and chase-camera framing.
2. Full relative distance gain through 35 units; zero at/after 600 units.
   Between them, t=(distance-35)/(600-35) and gain=(1-t)^2*(1+2*t).
3. Multiply original gains by distance gain and a host opponent mix scale 0.65.
4. If their sum exceeds 0.45, apply one common normalization to all ready
   opponent voices. Unavailable clips do not consume this budget. This limits
   their combined contribution and preserves relative gains without changing
   the player, material or announcer gain. It does not guarantee that the
   complete mixed output cannot clip; retain `clippedSamples()` diagnostics.
5. Stereo pan remains centered. Original spatial-owner behavior, Doppler,
   occlusion, panning, and PS2 hardware parity are not claimed.

Continuous loops start once and use mixer `controls()` thereafter, preserving
sample phase and embedded loops. Silent distant active racers keep advancing
with zero gain, so crossing a distance threshold does not restart a loop.
This deliberately trades up to fourteen bounded silent voices for continuity.

## Integration and frame/lifecycle behavior

The staged patch only changes `terrain_interactive_drive50du.cpp`; it adds no
new translation unit or build dependency. Existing linked 50HC, 50HE and 50HD
implementations are reused. The new helper is included by that main file.

Original rig construction occurs before audio preparation. The patch constructs
the opponent owner after the live owner, prepares it through the existing
shared mixer accessor, and closes it before `liveAudio->close()`.

Small engine input records are captured during existing `sampleRaceField()`
bank visits, after the world contact pass. There are no extra seven full-state
bank visits. They are never mixed until the race observation/commit completes.
Before the single existing shared world mix, the helper filters activity using
actual racer-bank state and race controls permission:

- Countdown: held opponents are silent; countdown speech/mix clock unchanged.
- GO/racing: each committed active unfinished opponent gets its own loops.
- Paused/recovery/failed player world step: no helper update or new mix occurs.
- Failed opponent: its bank slot is inactive; its loops stop on that world's
  next accepted audio update. No stale partial pose is published.
- Finished opponent: its two loops stop in the same accepted world commit.
- Player finish: existing global `resetVoices()` remains unchanged; the helper
  restarts only still-racing opponents afterward, before the shared mix.
  Finish speech retains its existing slot and trigger.
- Whole field complete: all opponent loops stop; result/countdown branch still
  advances the existing shared output once, keeping announcement tails intact.
- Restart/close: old opponent voices stop before their PCM or mixer dies;
  fresh session owners cold-load again, following the existing client policy.
- RR_AUDIO=0 or race-disabled mode: no opponent bank preparation or updates.

New logs report the exact POD/sample mapping, unique bank readiness counts,
updates/starts/controls/stops/unavailable/normalization counts, and rejections.
A fixed per-update allocation guard uses the existing main-thread counter.
No automatic speech, collision, warnings, boost onset or debris audio is added
for opponents in this bounded change.

## Root-owned future validation, not executed here

Root integrated this after the failed 04:15 UTC build. The next centralized
full-race validation is no earlier than 09:15:55 UTC. Useful coverage is:

- Cold parse of all seven authenticated PHY files and all five unique samples;
  verify exact POD08/POD11 SB_NGN binders and per-pod scalar values.
- Missing audio root and single unavailable clip: honest states, no substitutes,
  valid neighboring voices still usable; mismatched/unknown/duplicate PHY
  identities and occupied reserved slots reject.
- Near/intermediate/far/coincident positions, altitude, nonfinite inputs,
  source pitch outside admission, gain ceiling, and missing-clip budget.
  Rejected input must not partially mutate voices.
- Repeated control updates preserve phase; zero-distance gain and far silence
  are distinct from inactive/finished; global reset resumes eligible racers.
- Player/provider slots 0–95 and unused 110–127 remain unchanged by standalone
  opponent update/reset/close calls; existing player finish reset remains its
  own explicitly global operation.
- Eight-racer countdown → GO → pause/resume → stopped opponent → per-racer
  finish → player finish → field complete → restart, with one 768-frame mix
  per accepted 16 ms world step and no successful hot-path C++ allocations.
- Clipping/performance and listening quality using retained original assets.
  A successful compile or sample decode is not proof of audible quality.

Only root may execute the centralized software validation, at the user's
allowed cadence. This worker ran none of those checks.
