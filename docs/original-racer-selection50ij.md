# Original player selection and POD03 audio, 50IJ

Status: staged source and read review only, 2026-10-09. No build, syntax check,
unit test, decoder, game, emulator, or runtime validation was executed. Root
owns integration and the sole rolling-hour execution check. Existing POD01
validation does not certify these changed bytes or Clegg as a human driver.

## Scope

`original_racer_selection50ij.h` is an allocation-free HOST menu/config seam
for the eight already-retained originals, in their existing roster order.
Display names are copied from `evidence/originalRoster50HW/raw-imports/roster.json`.
`entries` contains POD01/03/04/07/08/09/10/11. The bounded implementation permits
human IDs1 and3 only. It exposes browse, confirm, cancel, lookup and an eight-row
slot permutation. An unsupported confirmation leaves the prior human unchanged.
The six remaining pods are authentic opponents, with their human joins pending.

`buildRosterOrder(3, order)` swaps only original roster rows0 and1, preserving
all eight identities exactly once. It does not copy or mutate live vehicle
objects. The caller must create a fresh session with the selected original
rig, handling, type2 query, collision targets, materials, cables, flames and
audio in bank slot0. Hot swapping a moving racer is explicitly not supported.
This is a development HOST menu seam, not a reconstruction of original UI.

The separate Clegg-player stage owns the live client's graph/handling/query/
renderer selection and roster permutation. This stage edits no main or menu
renderer source and does not replace those changes.

## Original identity proof

The source values are independent of host slot indices:

- At 21079C, the constructor copies incoming `a1` into `s5`.
- At 21081C, it stores `s5` into the vehicle's `60` field.
- At 210820, it reloads that same field as formatting argument `a2`.
- 2107F0/2107F4 select the format string at 3B14C0; 210824 calls formatting at 308C90.
- The format string at 3B14C0 is `Pod%02d`, including its terminating null.
- 21082C..210844 use that result in the `Pods\` resource prefix.

Therefore original POD03 requires racer60=3. Assigning native slot0 as racer60
would be wrong, and retaining1 for Clegg would address Anakin's named bank.
Retained source: `evidence/frameBridge49W/assessment/frame-order/resource_prefix.txt`.
The address and semantic provenance is SLUS_202.68 SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`,
VA-to-file mapping `VA - 0x100000 + 0x80`. No new disassembly was run.

## POD03-specific audio

`selected_player_audio50ij_fixture.h` builds a cold semantic catalog. POD01
retains60 requests. POD03 owns48 requests in the same sixty-slot bank/key
layout. Twelve unauthored curse/taunt slots stay empty and reject commands;
they do not play Anakin or fabricate missing Clegg samples.

Clegg's original speech counts are2/3/3/3 curses,4/4 taunts,3/3/3 specials.
That means11 curse cues,8 taunts and9 specials. `pod_speech50hg.cpp` admits
exactly the two authenticated count domains, preserving the existing source
cursor, category offset, cooldown and shared-RNG logic without a second RNG.

POD03.RES is328378 bytes, SHA-256
`4ad6a07ac02bc9ef65e336777ff27e6823060d333b0a885e99f14f58bd3ee26e`.
Cold loading checks its own archive and selected original member hashes for
events, debris, wrecks, speech metadata and PHY. Event1 remains the same
authenticated `p_event.ini` bytes, loaded from the selected archive by both
the audible and explicitly disabled lap owners. Anakin's archive is not used
as a substitute for Clegg's event authoring.

The existing retained-reader still supplies the authenticated unchanged TA
intro and source literal boost request, so the current eight-racer package
continues to require POD01 data as well. For POD03 every pod-owned semantic
row is filled from POD03 before publication. The source PHY supplies
`sfx\race\gen1_ngn.vag`, `sfx\race\as_beam.vag`, idle pitch0.5, range2.12,
binder idle0.5, binder range2.38, engine volume100 and binder volume90. Native
normalized base pitch4096 and continuous loop policy remain explicit backend
choices. Anakin is now admitted by the existing opponent engine mapper when
it occupies Clegg's former bot slot.

Both source-mode1 and HOST-mode2 Dax readers authenticate POD03ANN.INI. Clegg
has30 events, including unjoined Track Favorite30; Pod01 has29. Existing
represented source request types/keys are unchanged. Named bank3 is used for
Clegg, while generic bank0 remains shared.

POD03 named keys12/13/14/15 reference the same original VAGs as generic
keys42/41/45/51. `source_announcer50ie_fixture.h` deduplicates immutable cold
PCM requests but retains all54 semantic key records, their per-bank history,
source RNG attempts and independent availability projection. It never infers
a new line from another bank or changes source selection due to an alias.
The selected owner rejects requests carrying another human's source ID.

## Integration contract

All added player-ID arguments default to1, so existing direct fixture callers
retain the POD01 route. The main join must pass its selected `playerRacer60`
and `playerArchive` on every cold call below:

- `live_audio50HF::Owner::prepare(discRoot, audioRoot, error, playerRacer60)`.
- Copy `liveAudio->speechCounts()` into the human speech owner after prepare.
- Use `liveAudio->engineParameters()` in the human engine update.
- `race_audio50HQ::Owner::prepare(discRoot, audioRoot, mixer, groups, announcer, error, playerRacer60)`.
- `source_announcer50IE::Owner::prepare(discRoot, audioRoot, mixer, groups, episode, lazy, clock, sharedRaw, error, playerRacer60)`.
- `lap_sound50GH::prepareDisabledFromPod01(owner, provider, error, playerArchive, playerRacer60)`.

The legacy lap function name is retained for call-site compatibility; its
new bounded selector authenticates POD01 or POD03. `pod_audio50HE` gains the
same trailing argument internally. Both use `const std::filesystem::path&`.

Mode1 roster roles and the player damage-speech input must carry the selected
source racer60. Bank slot0 remains the human, and existing actor handles,
humanIndex5C and native physics-control gates keep their separate meanings.
Menu Start/Restart must reconstruct the whole race; checkpoint recovery must
not transplant a prior racer's geometry or audio into a newly selected pod.

## Remaining boundaries and next original targets

POD04/07/08/09/10/11 are not silently promoted by this two-player join. A next
worker should extend the selected cold audio catalog from each pod's original
RES, its `AUDIO/SPEECH/PODxx/SOUNDS.INI` and response tables, own PHY engine
parameters, and `AUDIO/SPEECH/DAX/INRACE/PODxxANN.INI`, retaining exact counts,
source category offsets and semantic aliases. Check whether any authored
category exceeds the current60-slot semantic capacities before exposing it.
Only then extend the exact speech-count admissions, source announcer ID gate,
lap archive identity gate and menu human-readiness flags.

The already-owned rig/handling/collision adapters can provide real geometry
and tuning, but all human-only draw/recovery/camera/type2-probe paths must use
selected bindings. A skin or name change does not satisfy those joins.
Original AI, secondary dynamic racer contacts, collision damage attribution,
opponent speech and broader original race/UI reconstruction remain outside
this change. No sound is claimed heard merely because it was registered.

## Parent-owned verification targets

Cold-start POD01 and POD03 in the allowed execution window; verify all eight
distinct original names/rigs, no duplicate Clegg, correct player type2 geometry,
and POD01's own engine data when it becomes a bot. POD03 gameplay bank should
contain48 authored requests and no `speech/pod01` or `as_ngn` substitution.
Its source Dax owner should retain54 semantic keys backed by50 unique samples.
Check actual missing/invalid/unsupported counts rather than claiming playback.
Exercise player input, pause/resume, Restart, selection cancellation, blocked
remaining racers, results/lap voice identity and a return to default Anakin.
The stage itself ran none of these checks.
