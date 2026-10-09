# Bounded original shared announcer: types3..8 and12

Staged source only,2026-10-09. No active edits or execution checks. This extends
the already tested group/host-episode bridge with an exclusive original-policy
mode. The existing mode2 host policy remains the default of the episode class.

## Source domain and actual request boundary

`shared_announcer50ie.h` represents original2C2C20 for selected human Pod01,
types3/4/5/6/7/8/12, variant0/1/2. Authenticated racer60=1 follows the original
constructor store21081C and its use in Pod%02d at210820..28; it is not inferred
from native racer slot0. The category6 subscriber supplies its exact live
payload; selected-player event5 supplies type12. Other categories/racers are
explicitly outside this provider, never filled by host place notifications.

The native `Owner::request` returns true when the source call actually returns:
source suppression, queue admission, selected playback or a genuine missing-PCM
return are all completed calls. It does not wait for speech completion. An
unavailable provider retains its exact phase, effects/history already committed
and RNG position; the surrounding FIFO must remain pending. Source assertions
become a persistent explicit fault, not a clamp/retry that silently changes RNG.

## Admission and persistent pending state

2C2DF0 first requires mode11258==1, then rejects signed pendingKey19C>=0. Ended
manager58 bit0 suppresses requests except types6/7/8. Priority table3C1A20 gives
all types3..8 priority9 and type12 priority1.

If B0<=0 and AC==0, admission stores the incoming priority. Otherwise low
priority1 is rejected; priority9 is admitted only when strictly greater than
current signed B4. If AC is active,2C2C20 calls queue helper2C4910 and returns
without stopping, restoring or starting a new episode. Thus source lap/finish
requests queue behind speech rather than inheriting host50HQ preemption.

Queue helper compares signed pendingPriority19E strictly against the incoming
priority. Variant0 attempts named selection and then generic fallback if that
fails; variant1 is generic and variant2 is named. No B8 probability draw occurs
on the queue path. Successful selection stores pending key/racer/priority and
updates history immediately, before deferred playback. A pending request is
never discarded or acknowledged as already spoken.

On an immediate request, stop-all2C4EF0 stops the actual represented emitter,
sets B4=-1 and calls end38. For types6..8 the following entry code sets pending
key/racer=-1 and pendingPriority=100. Then begin34 executes. That sticky100 is
retained until the source writes it again; it is not “fixed” to-1.

## Bank and variant selection

Immediate types3/4/5 and7/8 force named selection, regardless of variant. For
types6/12, variant1 is generic, variant2 named, and variant0 uses the source
B8/generic probability rule. A nonzero B8 forces generic without a chance draw;
otherwise one308030 fraction is compared strictly against authored BC=.80f.
Only this chance route writes B8 after the helper returns: generic0, named1.

| Type | Named original keys | Generic keys represented |
| --- | --- | --- |
|3|7|none in original generic selector|
|4|8|none|
|5|9..10|none|
|6|11|33..51|
|7|12..13|none|
|8|14..15|none|
|12|24..26|128..150|

Named selection branches through table3C19E0 to2C3AA8/3BB8/3CC8/3DD8/3EE8/
3FF8/4438. Generic type6 branches to2C31E0; type12 to2C3640. Even one-choice
named categories draw RNG. Named common tail2C47D8..480C permits count+1
attempts and discards the last attempt even if new. Generic tail2C37C8..3804
permits count attempts and also discards its last attempt. These limits differ.

History is27 rows of8 signed bytes.2C5560 compares signed LB against the
positive selected key, so generic128..150 never match their negative stored
history values. The source oddity is retained. A repeated named lap category
that exhausts selection and falls back to an unsupported generic category
reaches the original assertion boundary2C3758; it is an explicit fault rather
than an invented alternate line.

## Exactly one RNG and lazy seed owner

Every selector attempt uses the same application lazy80A8/seed3D6B30 as the
category6 last-lap music selection. If the lazy flag is clear, source sets it
and zeroes seed; if seed remains zero, it calls101A50 and308020 before drawing.
This check repeats before every attempt. The type12 immediate bank probability
draw itself occurs BEFORE this selector initialization.

The provider borrows the timing component's one `LapEventRandomOwner50ID`,
one `HostMonotonicClock50ID`, and the actual shared raw32 RNG reference. It
creates no private sound RNG and does not assume earlier unrelated RNG use
initialized the source lazy slot.308030 uses the exact raw32 LCG and low31-bit
return. The clock is explicitly a HOST monotonic backend in recovered source
units/49J quantization, not original console sample replay.

The original lazy fields are application statics. The integrated client instead
has an explicit HOST fresh-session policy: its existing physics raw RNG resets
with a new runSession, and the shared lazy owner resets alongside that raw RNG.
Dax and category6 still borrow exactly the same session lazy/raw owners outside
racer-bank snapshots; the HOST clock alone persists across session restarts.
Persisting a nonzero lazy seed across a separately reset raw RNG would be an
inconsistent hybrid. This adapter does not claim original cross-race RNG parity.

## Completion, delay and original priority5 tail

The source state initializes B0=2, B4=-1, B8=0, history/pending=-1, AC=0.
Authored SPCHPCT supplies BC=.80 and C0=7. `update` covers2C4C9C..2C4D7C:

1. Subtract the supplied source-seconds factor only while B0>0; preserve a
   crossing below zero.
2. If AC is active, inspect actual represented Dax activity. Once inactive,
   play an existing pending line and clear its three pending fields without
   end/begin, or call end38 and set B0=C0, B4=-1, AC=0 when none is pending.
3. Apply original2C4DA0 priority override5 when its strict condition holds.

The priority predicate was rechecked from raw2C4E24=46010043: it is DIV.S,
not subtraction. For the rank1 row it computes binary32
`row34 / vehicle->340->34 < 10`, then requires vehicle58==0. No lap-count test
is present. Cached body speed is genuine. Zero speed retains IEEE division
behavior: positive distance yields infinity;0/0 yields NaN; neither passes.

Original row34 comes from rank update1B0400. NodeC88's authored DistanceToFinish
is loaded by2A2540 (node+60; parser2A1610..163C; string3BE760). The writer adds
horizontal distance from the vehicle to the clamped segment formed by node+10/
node+20, then stores row34 at1B0658.1B0690 adds1 for ties with earlier rows.
Local raw evidence is `leader-speed-words.txt`; broader announcer words remain
in the parent recovery's `evidence/original-words.txt`, with the authenticated
SLUS SHA-256 recorded there.

The root-approved native adapter supplies its explicitly HOST remaining-distance
observer, genuine cachedSpeed34 and actual rank/human identity. Units and
direction match distance-to-finish/speed. The native edge projection and tie
arithmetic are not claimed to be the exact original row writer. The input's
`hostRemainingProjection` records this distinction. A missing row means no
priority override, matching the original initial max-distance/epsilon path.

## Native ownership and authoring

The cold reader authenticates the same original GEN_ANN.INI, POD01ANN.INI and
SPCHPCT.INI digests as50IC, selects54 relevant authored entries, and loads them
through the existing native original-PCM provider. No asset decoder was run in
this staging work. All selected cues preserve mono/nonloop/stream authoring.
Missing/unsupported/invalid PCM remains an explicit bank result.

The existing episode84 binding is borrowed, then cold-switched once to source
policy. Mode1 must disable ALL host Dax lap/place/finish requests while retaining
countdown/GO. The episode adapter also rejects `requestHost` in source mode.
Source mode separates stop/end/begin/play so pending speech stays in the same
episode and actual completion can be processed by the original state owner.
The tested persistent group gain and callbacks remain the only duck/restore
implementation. No second voice, group multiplier or private busy timer exists.
The older episode adapter's completion/preemption counters remain host-policy
diagnostics. In source mode observe original AC/B0/pending fields, actual voice
activity and end/group restoration, rather than treating those zero counters
as missing source completion.

The native one-emitter topology represents this exclusive selected-human/generic
subset on84. It is not a claim to materialize the original27 racer emitter
objects. Native PCM progression/activity supplies the backend completion; no
fake acknowledgements replace missing state. Original diagnostic/UI and base
scene traversal are not executed: scheduling is explicitly one accepted native
world step after common mixing, including countdown, never per racer/video/
paused frame. Root/sibling own the documented call-site integration.

## Music and other joins

The category6 helper returns from this announcer request before its later
music1A46E0 boundary. Music is an independently staged genuine provider owned
by the music worker, with shared Music118 ducking. Its helper must be resolved
through the ordered subscriber protocol; this owner does not discard or pretend
to complete the request. Lap1/2/3 requests, finish payloads, known-null native
HUD topology and accepted-world source timing remain the lap worker's join.

Mode2 host fallback remains available until the new combined join passes the
root-owned check. No test, build, syntax check, game, emulator or disassembler
was run here.
