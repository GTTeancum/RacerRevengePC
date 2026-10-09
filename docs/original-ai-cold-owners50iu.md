# 50IU: source AI cold metadata and one shared random owner

This is a separate source-only stage against frozen GC53d44876. All staged
code is NEW. No active checkout file was edited. No build, test, syntax check,
importer, game, emulator, guest execution, ee.bin, held49C or48U was used.
Original RES bytes were read/decompressed only to retain authored evidence.

## Headers and ownership

- `game/ai_cold_owners50iu.h`: typed profile, cold scalar writes, distinct real
  map/AI track binding, exact fresh-GC class/course-record metadata.
- `assets/ai_authoring50iu.h`: bounded load-time readers of actual selected
  `.ai`, `.spl` and `.tun` bytes. Reuses native49N/50AD number conversion.
- `game/original_shared_rng50iu.h`: seed/draw leaves and a bank registration
  helper which excludes the ENTIRE canonical noise/RNG owner.
- `game/ai_packing_reference50iu.h`: pure171B70 setter and1A0CA0 all-racer
  packing-reference update. This is a separate live join; it does not freeze
  constructor FF8=-1 or synthesize rank/remaining-distance inputs.

The central runtime supplement remains the only mutable AI owner. Copy the
cold values into it once. Do not keep a second live cooldown109C or create a
second target/list owner from these metadata records.

## Exact cold joins

1. Extract the chosen racer's actual member from its authenticated RES. Source
   spelling is `Pods\podNN\podNN.ai` (for example `Pods\pod03\pod03.ai`).
   Call `readAiProfile50IU`. There is no MyMaxSpeed/Skill policy here:16E630
   requests neither. Only consumed keys enter the typed profile.
2. The constructor establishes vehicle58=1, FE8=0, FF8=-1, FFC=1,104C=-1,
   1080/1084=-1,1090=1.5,109C=6 and the represented zero fields. Source create
   allocates five eight-entry pointer lists and clears counts1038..1048. The
   central runtime owns those real arrays, target and manager references.
3. Profile float reader158A70 and integer reader158920 skip destination stores
   on lookup failure. Thus an absent DriveAggression retains the proven0.5,
   absent RepairTime retains3, absent TargetPersistence retains0. This is not
   an assumed fallback: `profile-scalar-readers.txt` contains the skip branches.
4. Source16E580 calls187B10 and maps result<5 to class1, result<9 to class2,
   otherwiseclass3. This is independent of49N's track-name heuristics. For the
   admitted actualGC, `bindFreshGcAiCreateSelection50IU` provides source track
   ID10, ordered-index10 and class3. GC therefore retains loaded/default
   RepairTime; only source class1 overwrites1064 to2 at16E5CC.
5. FE0 is an ordinal, NOT a manager pointer.1AE4C4 forms opponent ordinal plus
   actual human count34;16E474 stores it and2107E8 stores it as vehicle5C.
   `bindAiCreateIdentity50IU` projects that exact input and the FE4 comparison.
6. FE4 compares pod60 with the selected COURSE-RECORD pod at original
   manager11280+12*(1125F&15), not automatically with the human-selected pod.
   Fresh originalGC metadata writes course-record pod17 at112F8 (17EE64).
   The provided GC helper explicitly represents that original fresh default.
   A loaded-save override has not been recovered here. Do not replace this
   value with the selected human ID or claim a saved profile was loaded.
7. Bind cold49N using mode58Nonzero=true and the actual PHY/class/profile
   selection before the first tick. Existing50IS `bindAiColdControls50IS`
   supplies full AI aero steering and source engine positions. This stage
   never flips a running player's vehicle58.
8. Use actual app28C.B8 as mapFF0 and app28C.BC as aiFF4. The source chain is
   1AA570/574 ->1AD520 ->1AE620 ->1AE370 ->16E4EC/4F4. These are distinct
   native track owners. While each actual SPL byte buffer is available beside
   `readTrackNavigation50EU`, call `readAiTrackMetadata50IU` with that SAME
   prepared geometry owner. After zone binding, `bindAiColdTracks50IU` checks
   the two owners and completed full-init C80=1,C84=0,C88=1,C8C=0.
9. Do not call `initializeTrackNavigationProgress50EU` during live undertrack
   recovery. Its actual217394..2173B8 writes belong to full initialization.
10. `resetAiColdState50IU` projects16E830's narrower reset: it preserves1004
    and list counts, retains profile values, clears B9C bit1 only and resets
    the explicitly written source timers/state fields. It is a typed cold
    reference, not permission to clear live lists at every frame.

## Track metadata and tuning

The original node constructor2A0F30 establishes SpeedLimit5C=0,
DistanceToFinish60=0,BoostScale64=1,branch68=0 and flags79=0. Optional lookups
retain those values. Original flags are set by successful integer-key presence,
even if the authored integer is0. Their bits are NodeBlocked0, NoCombatZone1,
ShortestBranch2, SkipPlayerTest3, DriveFullSpeed4, SecretBranch5, Pretty6,Ugly7.
ProgressNode belongs to the separate track progress list, not flags79.

BoostScale is loaded with158920 (integer) then CVT.S.W. The staged parser
admits the exactly representable integer subset[-16777216,16777216] and fails
explicitly outside it. It does not reinterpret authored decimals as floats.

TrackLength3C is computed at2A1BB8..2A1C5C as full XYZ distance from node0 to
its actual Next0, followed by ADD.S of that next node's DistanceToFinish60.
It is not a sum of HOST route segments or a hardcoded GC length. The reader
preserves scalar operation order, nearest rounding, finite ordinary inputs and
explicit failure for unsupported arithmetic. Geometry comes from50EU.

AiTrackTuning50IU includes actual source defaults from1AAB30 and authored
loads1AA5D0. Relevant GC values are:
- PodDamage: WaitToRepairEnvTime6C=5, RepairEnvUntil70=2,
  RepairEnvThreshold74=0.5
- AIDriving: Lookahead78=0.7, RacePace7C=100, RacePaceCutoffTime80=1,
  BehindPlayerClearance84=50, StartLineBoostTime88=0
- Packing: front/rear counts4/4; distances90/94/98/9C=1250/500/750/0;
  slowestA0=0.5,fastestA4=1

The loader stores packing counts with SB;1A0CA0 reads LBU. The parser retains
all eight bits, even though their typed storage is int8_t.

## Single shared RNG: exact main/bank joins

Immediately after creating the fresh runtime `f::Fixture x(0)` and binding
`s=x.state`, and BEFORE any consumer or cold audio initialization, invoke
`initializeOriginalSharedNoise50IU(s.noise)`. This replaces the old HOST
fixture seed0x12345678 with the static original C-runtime initial raw1.
The source ELF global38544C points to385160 and raw+58 is1. Continuing an
existing application stream is a different boundary: preserve its owner;
never reset it merely because a racer or checkpoint is created.

In `native_racer_owners50hn.inc`, REPLACE `racerBank.owner(s)` with
`registerRacerFrameExceptSharedNoise50IU(racerBank,s)`. Do not register both.
The helper registers two disjoint POD byte spans and excludes all of s.noise.
Delete the per-racer XOR seed statement in `native_racer_grid50hn.inc`.

Why the whole noise context:14EEC0 checks one global382B40, initializes the
shared tables once via14EFD0, and writes global scratch4332A8. Registering the
tables per racer would make cold RNG consume1791 draws per racer instead of
once. Exclusion preserves both tables and raw32 through every bank visit,
including read-only observation visits and exception restoration. The stable
field address remains valid for existing providers holding its reference.

All CURRENT participating consumers can keep taking this exact same
`s.noise.sharedRandomState` reference:
- gradient tables/secondary-body noise, visual noise and chariot noise
- impact effects50B; engine-tail50T (existing frame marshal/commit);
  flare50GV (existing frame marshal/commit)
- AI boost50IT and any reached native AI random branches
- pod speech50HG, material audio50HI, engine damage audio50IA
- lap event50ID and shared announcer50IE, including their real308020 reseeds
- GC animated-world service and its existing lazy initialization

Keep original source call order. Do not replay draws after an incomplete
external boundary. No per-AI seed is introduced, and the raw state retains
all32 bits while308030's return masks31 bits. Source-derived but not currently
joined TV camera consumers use TVCameraRandom49E; if later admitted they must
borrow/commit this same raw stream in source order rather than use their
standalone default-seeded object. This stage does not claim an entire original
boot chronology with all currently unimplemented startup consumers.

## Live packing join and genuine ranks

`updateAiPackingReferences50IU` belongs after the human source navigation
1A0DA0 and under1B0B80's actual gate: appB4.flags58 bit1 set and bit0 clear.
Original1A15E4 calls navigation;1A15EC calls1A0CA0.

It derives the inclusive rank interval using current human.CCC and actual
unsigned packing counts, clips its lower end to1 and upper to manager338
count30, and iterates EVERY actual source-ordered racer. Each vehicle58!=0
calls171B70 with current human5C if in range, otherwise-1. The setter assigns
FF8=-1 when CD8 bit0 is set. With two human racers, its exact retarget branch
compares genuine human0/1.CC4 and chooses0 iff human0.CC4<human1.CC4, else1.
Missing human distance owners fail at171BF0 instead of using fabricated0.

Native roster observations must carry original CCC/CC4, not HOST scoreboards.
Source manager1B0400 writes through racer+C90 subrecords: remainingCC4 is
record+34 at1B0658, equal-distance perturbation is1B0690, and CCC is record+3C
at1B0758/1B0798/1B07C8/1B08D4. The separate rank-owner stage owns that port.
Source CB8 is record+28 at1B01EC and is already represented by lap_timing50ID.
A missing or uncompleted source rank owner is an explicit join blocker.

## Actual profile evidence

`source-evidence/profiles-identity.json` authenticates all eight retained POD
archives and exact decoded .ai members. Common loaded values are
Combativeness=.5,HealthConsciousness=0,HoverBeforeAttackTime=2.5,
FrontHoverPosition=10,SideHoverPosition=3. Distinguishing values:

| Pod | MyMinSpeed108C | StartRaceNoCombatTime106C | WaitToRepair1060 | AttackRecovery1068 | CombatZone107C | CollisionRadius1020 |
|---|---:|---:|---:|---:|---:|---:|
| 01 | 100 | 9 | 10 | 3 | 1500 | 50 |
| 03 | 200 | 9 | 15 | 5 | 1500 | 141 |
| 04 | 200 | 7 | 13 | 5 | 1500 | 50 |
| 07 | 200 | 5 | 11 | 3 | 2000 | 50 |
| 08 | 200 | 7 | 13 | 7 | 1000 | 50 |
| 09 | 200 | 5 | 12 | 3 | 1500 | 50 |
| 10 | 200 | 1 | 7 | .5 | 3000 | 50 |
| 11 | 200 | 3 | 10 | 1 | 2000 | 50 |

Every retained profile omits TargetPersistence, RepairTime and DriveAggression;
their proven source constructor values remain0,3,.5 for GC. This table is
evidence only; the live join reads the selected original file.

## Admission and verification boundary

All readers have explicit bounded ASCII/number/duplicate/storage failures and
leave outputs unchanged on failure. The metadata tracks are limited to the
already admitted50EU Node0-based graph. Packing supports actual1/2-human,
up-to8-racer native owners; no additional mode/state branch is silently mapped.
Loaded save overrides, non-GC create-class selection, unsupported numeric forms,
and absent genuine manager/target/rank owners remain explicit boundaries.

Only source inspection and hashes were performed. These headers have NOT been
compiled, syntax-checked, tested, integrated into active code or gameplay-run.
They establish source-backed inputs and joins, not a successful0→4→2 drive.
