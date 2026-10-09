# Original Pod01 speech selection and reachable damage voice (50HG)

50HG adds the actual Pod01 speech category selection, source-owned cooldowns,
shared-RNG initialization and special selection, a reachable player damage-event
producer, and the separate later event-dispatch stage. It emits existing 50HE
cue IDs for actual native playback. It does not schedule arbitrary speech clips
or invent opponents, race results, deaths, camera-intro transitions, or announcer
resources. The live client join is maintained by the root integration.

## Files and stable join

- `src/game/pod_speech50hg.h/.cpp`
- `tests/native/pod_speech_authoring50hg_fixture.h`
- `tests/native/pod_speech50hg.cpp`
- Focused evidence under `solo-audio50hg/`

Cold load actual counts with `pod_speech50HG::readPod01Counts`. It authenticates
Pod01.RES and parses `audio\speech\pod01\sounds.ini`. The nine counts are
4/4/6/5/6/6/4/3/2, covering 40 distinct Pod01 samples. The newly transferred
original `AUDIO/SPCHPCT.INI` must be read through `readPlayerSpeechChance`; its
`HumanPercent=0.35f` overrides the original constructor value0.5 at1A5F90.
Leaving0.5 as the effective probability after loading the original file would
be wrong. Other probability fields and announcer dispatch remain separate.

Initialize `PodSpeechState50HG` once with `initializePodSpeech50HG`, passing
**the existing** `s.noise.sharedRandomState`. Exactly six original308030 draws
initialize the rotating category cursors; there is no independent audio RNG,
clock-derived seed, reseed per frame, or random clip chosen on every update.
The update preserves the raw32 LCG state and masks only the returned31-bit
value, matching308030. Float conversion/multiplication/truncation follow the
source binary32 sequence. A rare rounded random value outside the admitted
category range rejects explicitly rather than clamping to a different clip.

1. Before player input, call `advanceSpeechTimers50HG(factor,prefix,speech)`.
   The existing `UndertrackPlayerPrefix50ET.counter109C` stores float bits,
   despite its historical field name. This is the sole existing reset-owned
   109C field. Both source timers decrement only while positive and may cross
   belowzero. Preserve all unrelated flags10CC bits.
2. After actual224370 repair, call `afterRepairSpeechState50HG` with current
   damage7D0/7D4/7DC. Source2234E0 does not read7D8. Below0.7, the original
   repair override resets109C to3seconds and clears only10CC bit2.
3. At the player effects-wrapper boundary, call
   `produceDamageSpeechEvent50HG(input,prefix,event)`. Above0.7, with109C<=0,
   source1A2300 produces event5, copies the repeated-bit value, sets109C=10,
   and sets bit2. Its queue actor3C gate is preserved. This function consumes
   **no RNG** and does not play speech.
4. Later, at the explicit event-flush phase, call
   `dispatchDamageSpeechEvent50HG(input,counts,event,sharedRng,speech,commands,capacity,count)`.
   The source queue1A64B0 must not be collapsed into a probability draw before
   223540's effects or the rest of the native frame. The host integration uses
   an explicit post-frame subscriber phase; the full original manager scheduler
   is still unjoined. Process commands through the genuine50HF/50HE provider.

The combined `damageSpeech50HG` convenience entry is for adjacent queue/dispatch
fixtures; live integration should use the two separate phases. Input/capacity
errors preserve the pending event and RNG, permitting an exact retry. Successful
flush consumes the pending event once. PCM absence does not erase a source
selection or change the source cooldown; the real provider reports absence.

## Actual damage voice path

Source1A52B0 accepts a queued event only in the appropriate manager phase.
Vehicle40 must be zero. For the player, vehicle58==0 consumes one shared RNG
draw and compares it with effective player chance138 (0.35 from original file).
A passing chance invokes curse category3 at2C1A90. This is the authored
low-damage group, keys1–4, `an1rdl01.vag` through`an1rdl04.vag`.

The speech owner then checks its real nearest-listener XZ distance, strictly
less than200, and its source5-second cooldown. At mosttwo actual listeners are
admitted; no listener means inaudible. Ordinary gameplay camera/listener
coordinates are supplied by the native client. This is the original boolean
speech audibility gate, not full spatial attenuation/Doppler reconstruction.

If probability fails on the first damage event, source1A52B0 instead requests
announcer2C2C20 type12, with racer60 and the corresponding variant. The adapter
exposes this request without substituting a Pod01 clip. The current native
frame does not represent original racer60; `vehicle60Known=false` preserves
unknown arguments as0xffffffff and reports `announcerArgumentKnown=false`.
It does not alter player chance or voice selection. A verified future racer
owner may supply the actual ID. Repeated damage events do not invent another
first-event announcer request.

## Category selection coverage

`selectPodSpeech50HG` implements the concrete selector once its genuine game
owner issues a category request:

| Category | Source selector | Bank/key range | Actual caller availability |
| --- | --- | --- | --- |
| LowDamage |2C1A90 argument3|PodCurses1–4|Damage event5 producer/dispatch supplied |
| WallHit |2C1A90 argument2|PodCurses5–8|No matching current game-event owner joined |
| Passed |2C1A90 argument0|PodCurses9–14|Opponent/pass event owner unjoined |
| Attacked |2C1A90 argument1|PodCurses15–19|Attacker/partner event owner unjoined |
| Passing |2C1C00 argument0|PodTaunts1–6|Opponent/pass event owner unjoined |
| Attacking |2C1C00 argument1|PodTaunts7–12|Attacker/partner event owner unjoined |
| Death |2C1CE0 argument2|PodSpecial1–4|Original destructive transition unjoined |
| Win |2C1CE0 argument0|PodSpecial5–7|Race-finish/rank owner unjoined |
| Lose |2C1CE0 argument1|PodSpecial8–9|Race-finish/rank owner unjoined |

Curses and taunts rotate their specific cursor after both gates pass, then
reset their shared speech cooldown to5seconds. They do not draw additional
RNG at selection. Special requests ignore that cooldown and consume one shared
RNG draw after audibility passes. Merely registering or exposing a selector
does not make the corresponding opponent/death/race event live.

TA `audio\scrptLns.ini` is authenticated separately by `readTAIntroLines`.
It maps actual intro camera1/2/3 to Dax intro Event1/2/3, with absent Span values
defaulting to zero. `trackIntroSpeech50HG` reproduces175910..1759D4, including
config34==2 and signed4-bit span handling. It must be called only at the real
course-intro camera transition. Current gameplay startup and ordinary camera
mode changes are not that event. The three TA lines therefore have a recovered
selection contract but remain unjoined until an actual intro owner exists.

## Checks and limits

The focused check authenticates original authoring, all40 Pod01 category
ranges, allthree TA intro mappings, six cold shared-RNG draws, probability,
rotation, cooldown, distance gating, source damage event production, delayed
flush, announcer-request boundaries, atomic failures, and zero hot allocations.
The actual SPCHPCT.INI check confirms0.35. ASan/UBSan results are recorded
separately; leak detection is disabled in this container.

These are native checks and immutable static SLUS inspections. No guest CPU,
PC-driven execution, ee.bin, or excluded49C/48U code is used. This is not a
complete race-event manager, all-racer voice implementation, music/UI system,
or proof that every voice context exists in the current single-player client.

The separate original-PCM supplied-state fixture selected `an1rdl03.vag` via
the real damage path and original0.35 probability, then mixed96,000 frames
through the genuine provider:77,884 nonzero scalar samples, peak30,920. All60
original clips were ready. This confirms original voice PCM reaches the mixer;
it is explicitly not a live-driving or physical-speaker listening claim.
