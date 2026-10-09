# Gameplay audio coverage, 2026-10-09 13:17 UTC

This is an event/owner inventory, not an all-audio completion claim. Original
AUDIO files are already retained locally. Missing event state must not be
replaced by another transfer request, a random timer or a host key trigger.
Loaded PCM, a selected cue, successful mixing and heard speaker output are
separate facts. The50IA player route passed its Linux build and one native
race/finish-continuation integration check. Its last sample showed10,410 updates,
20,820 shared RNG draws and zero dips/returns/completions at pitch scale1.
Positive damage-dip behavior remains unverified; this run did not restart.

## Current live routes

| Route | Current usable event/state | What is connected | Remaining boundary |
| --- | --- | --- | --- |
| Player engine/binder | Native speedRatio70; original Pod01 tuning | Original two loops, source pitch law, boost multiplier | 50IA is integrated for the player; healthy-state execution passed, positive dips remain unverified. Original spatial/Doppler owner absent. |
| Seven opponent engine/binder pairs | Each committed active racer's native speedRatio70 and pose, including finished continuation; exact original PHY mappings | Fourteen loop voices, five unique original samples | Distance/mix policy is explicitly host presentation; boost pitch and damage scale still baseline. |
| Player boost onset | Actual boost boundary, prior vehicle64, B9C flags, source heating | PodEngine3, original boost.vag, plus pitch state | No opponent subscription; no synthetic boostmax onset. |
| Repair completion | Actual player repair tail, B9C&2, vehicle3C, all four damage values | PodEvent2, original boostmax.vag | This is the repair completion tail, not an arbitrary repair loop. |
| Overheat/damage/wrong-way warning | Actual vehicle64, native max damage, flags10CC and suppression | PodEvent3/4/5 loops and source transitions | Spray PodEvent6 still lacks its real water-probe118 input. |
| Lap beep | Recovered lap292360 ->233410 owner | PodEvent1, original lapcount.vag | Source lap sound owner is distinct from host announcer speech. |
| Player material collision/scrape | Actual timer830, main spring candidate130, authenticated contact A9, speed and listener distance | Sixteen authored collision/drive-on identities, thirteen unique PCM clips | Selected player only. Original pan/Doppler absent. Collision manager event3 is a source no-op; do not invent wall-hit voice. |
| Player low-damage character voice | Actual event5 producer, queued flush, native damage/timer/flags, original HumanPercent0.35 | PodCurses1..4 with original audibility, rotating cursor and cooldown | Only this character category is live. Its original Dax fallback is still unjoined. |
| Countdown/GO announcer | Real host race countdown/phase edges | Four original RACE_SND events | Explicit host race lifecycle, not restored original race manager. |
| Player race announcements | Committed host laps/place/finish | Original Pod01 Dax lap/first/second/third/place-change groups; eighteen authored speech rows | Variant rotation, priority and cooldown are explicitly host policies. Finish4+ has no joined authored response. |

The player's60-entry generic catalog therefore has12 identities with actual
joined client routes: five PodEvent keys1..5, three PodEngine keys1..3, and four
low-damage curse keys1..4. The catalog's older `hasJoinedTrigger` count includes
spray at identity level and excludes external speech subscribers; it is not a
live game coverage metric. Separate material, lap-owner and race-announcer
registries must not be added together as a unique-file total.

Voice ownership remains0..59 player generic,60..63 lap,64..79 player materials,
80..95 race announcements,96..109 opponents. Slots110..127 remain unused.
50IA changes none of these ranges or the one committed-world mix.

## Missing original routes and exact dependency

| Route | Required missing owner/input |
| --- | --- |
| Positive player engine damage dip | 50IA build/live integration passed with healthy-state draws only. Real positive-damage dip, thresholds, cooldown and return-curve behavior still need focused evidence. |
| Low-damage Dax fallback | Existing event5 dispatcher emits announcer type12, but current input explicitly sets vehicle60Known=false. Need the original racer60 identity mapping and announcer2C2C20 type12 bank/variant/chance/delay/priority owner. Host slot0 or POD01 label alone is not proof of racer60. |
| Wall-hit character voice | The current recovered material collision dispatch emits source event3, whose dispatcher is a no-op. Need an actual different source caller of the wall-hit category; raw contact or rumble is insufficient. |
| Passing/passed character voices | Original paired-racer pass event payload and queued dispatcher, with the correct source actor roles, audibility and Human/AI percentage path. Host standings changes do not recreate this payload. |
| Attacking/attacked character/Dax voices | Original attack/partner event producer and payload, actor IDs and queued dispatch/chance ownership. Existing physical racer-contact response records are not automatically attack events. |
| Character death / Dax death | Complete original destructive transition handlers2261B0/226750/227370/227A40 and genuine event2 production178B80->1A64B0. Current impact inspection stops before the first unsupported virtual transition; its diagnostic is not an executable death event. |
| Wreck/explosion and debris | Actual destruction/wreck object creation, emitter state and original event selection. Host damage recovery/checkpoint relocation is not this event; merely retaining podexpl/debris PCM does not join it. |
| Character win/lose | Original race-finish event owner with valid source result/rank/configuration arguments and character-special request. Current host result edges only feed the documented host Dax subscriber. |
| Track-intro Dax | Actual original intro-camera owner transition and camera index/flags62. Startup and ordinary chase-camera updates do not satisfy175910..1759D4. |
| Water spray | Actual selected player's water probe118 result/owner at the warning phase. Main terrain contact or wet-looking material is not a substitute. |
| Opponent boosts | Each opponent's genuine boost-boundary request and persistent audio boost state; original mapped onset provider per opponent. Existing native boost fields are available, but no audio subscriber owns those per-racer transitions yet. |
| Opponent collision/scrape | Each opponent's actual post-effect material input, persistent D44-equivalent audio state, appropriate immutable original material mapping and disjoint provider voices. Current native fields exist during bank visits; selected-player audio state must not be reused. |
| Other opponent character speech | Exact per-character authored counts/banks, original event payload and per-character cooldown/cursors. Current player Pod01 bank must never stand in for their voices. |
| Ambience, music and UI | Actual track ambience/soul-tree, jukebox/music and UI event owners and their authored records. No route joined by this gameplay patch. |

## Integration caution: RNG ownership is observable behavior

50IA reads/writes the currently live selected player's
`s.noise.sharedRandomState` at the existing player engine subscriber boundary:
after native player physics/material effects, before warning controls and the
later queued player speech flush. Two source draws occur on every zero-cooldown
update even when healthy; a dip adds a third. This necessarily changes later
native RNG-dependent outcomes and cannot be treated as output-only wiring.

The current HOST racer bank snapshots `s`, including a separate RNG history
per racer. That architecture is unchanged. The integrated route preserves ordering
against the player's already-joined subscribers; it does not claim to recover
the complete original cross-racer/global-manager scheduling. Integration accepts this bounded ownership model; full original scheduler parity remains unclaimed. Do not reseed,
silently suppress healthy draws, or derive a private sound-only sequence.

## What can honestly be reported now

The client plays a growing source-backed subset of original gameplay effects
and voices, with separate documented host race/presentation policies. It does
not yet implement every effect, every character context, all announcer logic,
all original owner scheduling, music/UI or complete3D audio. Present-file and
ready-bank counts are not evidence that those event routes exist.
