# Genuine TA timing, records and ordered speech adaptation

Source-only staging, updated 2026-10-09 14:42 UTC. Active checkout and mode2 remain unchanged.

**Bounded integration:** `docs/native-mode1-integration50id.md` and
`tests/native/race_mode1_host50id_fixture.h` describe and implement the native
adapter proposal. The original whole-scheduler requirement is superseded by
explicit HOST accepted-world scheduling, the existing HOST rank/finish observer,
known-null original HUD fields, and a monotonic native clock backend.
No compiler, syntax check, build, tests, emulator, game, disassembler, asset
decoder or validation program was run. Original bytes were read and manually
interpreted. Everything here is uncompiled and untested.

## Concrete deliverables

- `tests/native/lap_drive50gj_fixture.h` is a staged replacement
  for the current owner. Its default unbound configuration retains mode2.
  Null nativeConfig preserves mode2. Optional binding supplies genuine TA
  records,100-slot banked timing, real50GG query returns, mandatory effect
  boundaries, and exact category6 enqueue. Numeric-tail retries cannot enqueue
  the same event twice.
- `tests/native/race_speech_dispatch50id_fixture.h` is a
  resumable FIFO consumer for existing damage5 and recovered lap6. It retains
  committed RNG/effects and never skips a blocked head event.
- Nine headers under `src/game` implement record queries, exact TA defaults,
  timer cuts, shared FIFO, damage5 bridge, category6 control flow, clock/seed/draw
  arithmetic, known-null original HUD branches and the native host clock backend.
- `race_mode1_host50id_fixture.h` is the coherent HOST adapter, with cold binding,
  per-world timing/observer handoff, source event production and retained FIFO
  dispatch. The main patch binds the concrete sibling Dax/music providers.

## TA record/default-profile blocker resolved

The numeric defaults are in the executable; no save artifact is needed for a
fresh native numeric profile. This does not import or overwrite a user save.

- Constructor17E118..17E138 initializes selection1125F low nibble to0.
- Constructor17E9D8..17E9F8 copies `ta\0` from3A89E0 into the first track row
  at11278. Getter187330 uses12-byte rows and that selection nibble. TA is row0.
- Original language/default initialization17DF3C calls188870, which tail-jumps
  to1801D0. Its row0 direct-slot calls185910 at18030C/1803CC/18048C write lap
  words423F51EC/4250A3D7/426E0A3D.
- Its direct-slot calls185BC0 at18054C/18060C/1806CC write race words
  432B2148/43328A3D/4336A666.
- Nonzero slot arguments take185B1C/185E14, directly storing f12 at
  102EC+4*slot /10338+4*slot. These are genuine defaults, not false-query
  sentinels. The header copies exact bits without decimal re-rounding.

`lap_config_ta50id.h` represents TA's numeric row only, not localized names or
other tracks. Its caller explicitly chooses mode, lap count, humans and AI
using the real packed fields. One human/seven opponents/three laps gives
packed1125C=03A3. This is an explicit native configuration choice, not a claim
that original menu input was replayed.

## Exact category6 queue and subscriber

1A39B0 builds five words. The timing producer supplies `{0,6,0,actual racer,0}`.
1A64B0 checks actor3C, then appends or returns without insertion. It does not
synchronously select or play speech. Native actor IDs must resolve the real
racer; the multiplexed execution-arena pointer cannot identify an opponent.

The queue models initial logical capacity32 and32-entry growth at the exact
`count+1 >= capacity` threshold, before actor gating. Fixed128-entry native
reservation is an explicit bounded implementation; exhaustion returnsStorage.
Queue helper completion is never reported as subscriber/sound completion.

At flush1A4C9C, manager58 bit1 must be set and application1B0 bit4 clear. The
gate is captured once for the loop.1A5C40 clears count after dispatch, including
when that dispatch gate was false. Damage5 now shares this FIFO through a
replacement producer and the existing50HG selector, with retained continuations
for real audio/announcer effects. Do not run its old single-slot route too.

Category6 table3AA288 routes to1A55F0. It reads LIVE vehicle58,CCC,CC8,CD8:

- AI returns without Dax/music effects.
- Unfinished human nonleaders and unfinished racer60==16 return.
- Human leader laps1/2/3 request original2C2C20 types3/4/5, variant0.
- Finished humans request types6/7/8 with exact place/racer16 variants, then
  request1A46E0 key27 for first,28 for second/third,26 otherwise.
- At the last-lap count, after the Dax return, source performs its own lazy
  clock/seed path, shared-RNG draw and1A46E0 key34/29/33. No host round robin.

`lap_event6_subscriber50id.h` preserves all these helper boundaries. Source
rank/finish values must precede the FIFO flush so it sees the finished
flag and new rank. The bounded native adapter explicitly supplies these from
the current HOST observer, not queue-time snapshots or a claimed original
ranker. Dax remains the one shared announcer owner.

## Native clock and shared RNG

The retained source-only101A50 helper remains available in
`lap_event_random50id.h`. The bounded adapter uses the explicit host backend in
`lap_host_clock50id.h`: application-lifetime steady_clock nanoseconds become
576000Hz integer ticks, then the existing49J source conversion produces uint32
milliseconds. It does not require original console clock samples. Lap timing
separately consumes the same binary32 seconds used by the accepted native step,
currently `.016f`; it never copies HOST interpolated finishSeconds.

Source lazy80A8/seed3D6B30 is shared between category6 and original Dax selectors,
not specific to lap events despite the header's historical type name. Keep one
shared instance per HOST cold session, one application-lifetime host clock
backend and the same existing human native raw RNG across those owners. The
fresh native fixture resets raw RNG at each session, so its shared lazy slot
resets at that same boundary; original cross-race static parity is not claimed. The current HOST bot architecture
continues to own independent bot RNG state.

## Concrete join and remaining bodies

See `docs/native-mode1-integration50id.md` for complete cold/per-world calls. The new
Owner performs source numeric/event cuts at an explicitly declared HOST world
boundary, hands off to the existing HOST ranking/finish observer, then clears
consumed lap notifications and flushes FIFO order before the single shared mix.
It does not require original root-list/global-frame parity to activate the
represented mode1 route. Stable actor handles and real actor3C are preserved;
HOST roster role metadata avoids changing player-frame physics gates.

HUD pointers are now explicit native scene fields known to be null. The actual
22B170/22B370/22B760 null branches return without text/animation effects. If a
future native scene binds a nonnull original HUD object, its effect provider
becomes necessary; an absent function pointer alone is never callback success.

The required shared original2C2C20 Dax request/update/completion and1A46E0 music
transport/admission bodies are supplied by the sibling header sets described in
`docs/shared-announcer50ie.md` and `docs/race-music50ie.md`; main now binds their concrete APIs. No required body
remains missing for this bounded route. Dax near-leader state uses the proved
strict remaining/speed formula, genuine cached speed and root-approved explicit
HOST remaining-distance projection; no original distance parity is claimed.
Actual synchronous suppression/queue/playback returns may acknowledge a request;
a missing provider retains that exact head and prevents subsequent world work.

Root owns review, patch application and all validation. The staged main now
wires authenticated human Pod01 identity, one-world Dax updates, the existing
audio-command/speech-input wrappers and selectable mode2 fallback. Fresh defaults, source record queries,
timing storage, queue/event primitives, known-null HUD and native timer backend
need no further recovered function bodies for this bounded scene. Current mode2
fallback and all active sources remain unchanged.

## Provenance and files

ELF SHA256 c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
Mapping is fileOffset=VA-100000+80. Source word ledgers and static TA bytes
were preserved with the source staging review artifacts, not executed. Timing
finished-racer skip is manager bit5, not bit2. Source constructor allocates100
CC0 floats;50GG reads only the lapCount-1 prefix for its race query total.

This document records source-only staging. Integration and centralized
validation are separate; no compiled/tested result is asserted here.
