# Bounded mode1 native race integration

The 2026-10-09 Linux build and single mode1 race passed on source
`0c3810cadd4750b4f5d4785320f0f3d259abe50a`. Native integration uses recovered
lap/record/event/audio logic with explicitly HOST scheduling, roster roles and
rank/finish observation. Full original scene traversal and console-frame parity
remain unclaimed.

## Observed result and limits

Two units compiled, 191 reused and linking succeeded. The player completed
three laps at 160.928 host race seconds and all finish-continuation criteria
passed with normal exit. The source Dax bank had 54 ready clips; actual types
3, 4 and 6 started. Song 29 provided last-lap music, followed by finish song 27.
All three completed native lap times and best lap were visually checked in the
HUD. Captured PCM lasted 174.032 seconds; FPS ranged from 53.6133 to 62.6681.

The run used autodrive. Positive type12 damage announcements, positive engine
dips, pause and restart were not covered. This is bounded integration evidence,
not a pass for every supported branch. See [current validation](current-validation.md)
for source and executable identity.

## Entry and fallback

RR_RACE_MODE=1 is the race default after cold preparation succeeds. Preparation
requires the actual source announcer owner, all six last-lap/finish music clips
and the shared output backend. Any failure is explicit; no silent mode switch.
RR_RACE_MODE=2 selects the previous mode2/HOST audio path. Use that explicit
choice with RR_AUDIO=0. RR_RACE=0 retains the existing solo mode2 behavior.

The one existing Dax episode/voice84 remains attached by raceAudio.prepare;
sourceAnnouncer.prepare switches that same idle owner to original policy. In
mode1 the HOST race-audio owner emits countdown/GO only, avoiding duplicate lap,
finish and place speech. Mode2 keeps its prior HOST speech policy unchanged.
Music owns exclusive slot117 in the shared Music118 gain group. All existing
engine/material/character voices and the shared ducking layer remain intact.

## Cold state and fields

One mode1 Owner holds shared TA config, manager timing, FIFO and subscriber
outside the racer bank. Fresh TA selection0 and six record values are genuine
original defaults. One human/seven opponents/three laps uses packed1125C03A3.
Per-racer CB0/CB4/CB8 and100 CC0 floats live in lapContinuity and are included by
its existing bank registration. Real50GG query returns come from those tables.

The native scene explicitly has no original HUD objects. Its resolved original
HUD fields at app28C+134/+130 with28*vehicle5C stride and+164 with4*stride are
known-null. The real22B170/22B370/22B760 null branches therefore return without
text/animation effects. The HOST overlay is a separate owner. A future nonnull
original HUD field requires its real effect provider; null callbacks are never
used as pretend success.

Stable actor IDs are slot+1 handles, not borrowed arena pointers. Actual actor3C
controls source queue insertion. HOST human/opponent role metadata is explicit,
so the AI category6 early return does not depend on bot player-frame vehicle58.
Arena vehicle58 is unchanged. The actual human Pod01 racer60=1 is authenticated
by original21081C/210820; unknown opponent identity lanes are never consumed by
the human-only subscriber or guessed from filenames.

## Accepted-world order

1. The existing human post-repair stage produces damage5 into the shared FIFO.
   The old single-slot producer/dispatcher runs only in mode2. No duplicate
   damage threshold/repeat mutation or speech RNG draw is introduced.
2. All participating native frames and the existing world contact pass finish.
   The source numeric timing pass advances once with the same binary32 seconds
   used by physics, currently .016f, and appends lap6 in racer order. Paused,
   countdown, stopped opponent and abandoned player work does not advance it.
3. The existing HOST observer samples racers, ranks and seals finishes. Its
   actual native CD8 bit0 projection and result closure stay intact. Consumed
   lap CD8 bit1 clears only after its timing pass and that observer commit.
4. The shared FIFO flush reads the current actor state and explicitly HOST
   rank. Damage5 and lap6 are dispatched in production order. Earlier returned
   effects/RNG remain committed; a blocked head retains its cursor and prevents
   later events or new worlds. The development main logs and stops at the real
   unresolved helper instead of silently acknowledging it.
5. Existing opponent audio feeds the single shared16ms mix. Source Dax updates
   once after actual mixedFrames increases by768, including countdown ticks.
   A WAV write failure after mixing still processes completion/end/delay before
   reporting the error. A zero-frame failure does not advance source timers.
   Music activity is polled after every attempted shared step.

Dax remaining-distance priority uses its exact strict remainingDistance/speed<10
formula for the rank1 human, without a lap-count guard. cachedSpeed34 is the real
native field. Remaining distance is explicitly the HOST observer projection;
original authored node-distance/cross-section/tie arithmetic is not claimed.

## Clocks and RNG lifetime

Lap times are accumulated with the actual accepted binary32 physics factor.
No HOST double raceSeconds or interpolated finishSeconds is copied into them.
Recovered centisecond truncation, reset-to-delta and record comparison remain.

The native101A50 backend uses one application-lifetime steady_clock epoch. Its
nanoseconds become integer576000Hz ticks, then the existing49J original
unsigned64/binary32/saturating conversion yields uint32 milliseconds. This is a
host clock with source units, not console timer sample reproduction.

Original Dax selectors and category6 share the SAME lazy80A8/seed3D6B30 slot and
existing human native raw RNG. The HOST session reconstructs that raw RNG in its
fresh native Fixture, so the shared lazy slot explicitly resets at the same cold
session boundary, outside racer snapshots. Original cross-race static lifetime
parity is not claimed. There is no second audio RNG or overwrite of bot physics
ownership. Within the session, exact source seed/draw ordering is retained.

## Native downstream bodies and evidence

The source announcer implementation and authenticated Pod01/generic bank
selection are documented in `docs/shared-announcer50ie.md`. It distinguishes a
true source suppression or unavailable play-helper return from an unimplemented
request. The original music admission/transport and six authenticated stereo
clips are documented in `docs/race-music50ie.md`; missing transport/PCM is not a
silent music return. Both use actual mixer activity, not guessed clip duration.

Timing/default/profile details and source addresses are in
`docs/native-lap-timing50id.md`. Shared source ELF SHA256 is
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.

Telemetry exposes source Dax AC/B0/B4, its pending key and last request. Existing
shared_Dax completion/preemption counters are HOST-policy diagnostics and are
not source-policy completion proof. The focused result above uses the actual
mode1 source-policy telemetry and runtime observations.
