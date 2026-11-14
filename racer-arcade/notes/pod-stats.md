# Pod handling stats for the six arcade-only pilots

Pilots: ARK (Ark "Bumpy" Roose), BOL (Boles Roor), EBE (Ebe Endocott), ELA (Elan Mak),
NEV (Neva Kee), RAT (Ratts Tyerell). Racer Arcade has no per-pod handling for them, so the
values come from Star Wars Episode I: Racer (1999), translated with the pods both games share.
Every proposed value is **derived from Racer 1999 via fit**, not original Revenge data.

Machine-readable output: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\pods\derived-stats.json`
(`{code: {section: {key: value}}}`; a `_provenance` section per pod records the source, the Racer
percentiles used and the audio donor pod).

## 1. Racer 1999 per-pod stats (base, no upgrades)

Precise in-game values (decimal values, not 0-5 bars). Source for every number:
https://github.com/louriccia/Botto/blob/HEAD/src/data/sw_racer/racer.js (community bot data for the
SWE1R speedrunning community; raw file read from
https://raw.githubusercontent.com/louriccia/Botto/HEAD/src/data/sw_racer/racer.js).

Cross-checks (all agree, no conflicts found):
- Ben Quadinaros first seven stats (0.45/203/89/3.00/575/40/95): Accutron N64 FAQ,
  https://gamefaqs.gamespot.com/n64/198780-star-wars-episode-i-racer/faqs/22256 (seen via search snippet; site blocks fetches).
- Lowest turn response Bumpy 202, max turn rate range 80..120, highest boost Ben 400, Mars Guo 315:
  speedrun.com "Podracer 101", https://www.speedrun.com/swe1r/guides/tyzs9 (search snippet).
- Sebulba top speed 600: https://www.giantbomb.com/star-wars-episode-i-racer/3030-13526/
- The 15 per-pod stats and their order, PC table at 0x4C2BB0:
  https://github.com/everalert/swe1r-decomp/blob/master/idapro/notes/RaceData.txt
- Not checked: a direct dump of the PC table at 0x4C2BB0; whether the bot data is PC or N64
  (the Ben match against the N64 FAQ suggests they are the same).

AirBrakeInv and DecelInv are "interval" stats: lower is better. DmgImm = damage immunity.

| Pilot | Antiskid | TurnResp | MaxTurn | Accel | MaxSpeed | AirBrakeInv | DecelInv | Boost | Heat | Cool | Repair | Hover | BumpMass | DmgImm | Isect |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Anakin Skywalker | 0.50 | 300 | 110 | 3.00 | 490 | 30 | 60 | 200 | 13.0 | 9.0 | 0.40 | 4.99 | 50 | 0.60 | 5.0 |
| Teemto Pagalies | 0.50 | 260 | 90 | 1.70 | 479 | 30 | 80 | 195 | 14.0 | 6.0 | 0.43 | 4.99 | 50 | 0.50 | 8.0 |
| Sebulba | 0.38 | 228 | 95 | 3.20 | 600 | 38 | 50 | 185 | 9.0 | 2.0 | 0.19 | 4.99 | 80 | 0.30 | 7.0 |
| **Ratts Tyerell** | 0.35 | 238 | 90 | 4.00 | 520 | 33 | 80 | 300 | 12.0 | 5.0 | 0.30 | 4.99 | 55 | 0.45 | 7.0 |
| Aldar Beedo | 0.30 | 250 | 85 | 4.00 | 517 | 35 | 85 | 360 | 10.5 | 4.5 | 0.25 | 4.99 | 68 | 0.40 | 7.0 |
| Mawhonic | 0.36 | 224 | 100 | 3.75 | 480 | 41 | 80 | 350 | 13.0 | 7.0 | 0.20 | 4.99 | 60 | 0.48 | 7.0 |
| **Ark "Bumpy" Roose** | 0.30 | 202 | 85 | 1.00 | 485 | 30 | 80 | 210 | 10.5 | 6.5 | 0.10 | 6.50 | 70 | 0.25 | 6.0 |
| Wan Sandage | 0.80 | 294 | 95 | 1.90 | 480 | 25 | 70 | 180 | 9.0 | 3.0 | 0.19 | 7.00 | 60 | 0.50 | 7.0 |
| Mars Guo | 0.60 | 288 | 100 | 2.30 | 540 | 30 | 85 | 315 | 7.5 | 2.1 | 0.35 | 6.00 | 70 | 0.50 | 10.0 |
| **Ebe Endocott** | 0.60 | 294 | 100 | 2.50 | 500 | 40 | 70 | 190 | 15.2 | 11.0 | 0.45 | 4.99 | 45 | 0.70 | 4.8 |
| Dud Bolt | 0.54 | 215 | 80 | 3.00 | 505 | 35 | 90 | 230 | 8.6 | 2.5 | 0.20 | 4.99 | 70 | 0.35 | 5.5 |
| Gasgano | 0.43 | 238 | 82 | 3.30 | 510 | 43 | 83 | 310 | 12.5 | 1.7 | 0.40 | 4.99 | 63 | 0.43 | 4.2 |
| Clegg Holdfast | 0.50 | 252 | 89 | 1.75 | 495 | 45 | 80 | 303 | 11.5 | 5.0 | 0.31 | 6.00 | 55 | 0.43 | 7.0 |
| **Elan Mak** | 0.30 | 224 | 95 | 3.75 | 480 | 40 | 70 | 360 | 10.0 | 2.5 | 0.30 | 4.99 | 53 | 0.50 | 6.0 |
| **Neva Kee** | 0.80 | 230 | 115 | 1.00 | 480 | 30 | 70 | 280 | 11.5 | 3.3 | 0.32 | 4.99 | 55 | 0.60 | 7.0 |
| Bozzie Baranta | 0.33 | 294 | 90 | 2.10 | 485 | 42 | 83 | 275 | 11.8 | 3.5 | 0.30 | 4.99 | 60 | 0.55 | 7.0 |
| **Boles Roor** | 0.30 | 280 | 83 | 2.85 | 590 | 35 | 85 | 390 | 9.5 | 2.7 | 0.18 | 4.99 | 62 | 0.30 | 7.0 |
| Ody Mandrell | 0.45 | 238 | 90 | 1.80 | 475 | 30 | 65 | 240 | 11.0 | 4.4 | 0.40 | 5.00 | 57 | 0.60 | 5.2 |
| Fud Sang | 0.35 | 245 | 90 | 2.85 | 490 | 30 | 75 | 250 | 12.0 | 6.5 | 0.39 | 4.99 | 53 | 0.55 | 7.0 |
| Ben Quadinaros | 0.45 | 203 | 89 | 3.00 | 575 | 40 | 95 | 400 | 8.0 | 2.0 | 0.28 | 4.99 | 73 | 0.45 | 7.0 |
| Slide Paramita | 0.43 | 297 | 120 | 1.95 | 475 | 30 | 80 | 200 | 16.0 | 12.0 | 0.63 | 4.99 | 40 | 0.80 | 7.0 |
| Toy Dampner | 0.50 | 270 | 86 | 1.75 | 485 | 25 | 70 | 240 | 12.5 | 10.0 | 0.50 | 4.99 | 40 | 0.65 | 7.0 |
| "Bullseye" Navior | 0.70 | 322 | 120 | 1.80 | 480 | 25 | 70 | 300 | 15.0 | 11.0 | 0.55 | 4.99 | 45 | 0.77 | 7.0 |

(Secret pods Jinn Reeso = Mars Guo values, Cy Yunga = Bullseye values; not used.)

## 2. Revenge per-pod values

Re-extracted from every `C:\Games\Star Wars Racer Revenge\PODS\PODnn\PODnn.RES` member
`Pods\Podnn\Podnn.phy` (all 23 pods) with `trk_res3.read_res3`.

### [BaseAbilities] (0..1)

| Pod | Name | MaxThrust | TheoreticalTopSpeed | SpoilerYawMoment | Defense | BaseRepairAmount | BaseCoolingTime | Sum |
|---|---|---|---|---|---|---|---|---|
| 02 | Ben Quad | 0.3 | 0.1 | 0.2 | 0.1 | 0.25 | 0.25 | 1.20 |
| 03 | Clegg | 0.25 | 0.1 | 0.25 | 0.15 | 0.3 | 0.15 | 1.20 |
| 04 | Dudbolt | 0.25 | 0.25 | 0.1 | 0.25 | 0.15 | 0.2 | 1.20 |
| 07 | Gasgano | 0.25 | 0.2 | 0.15 | 0.15 | 0.2 | 0.25 | 1.20 |
| 08 | Mars Guo | 0.25 | 0.3 | 0.05 | 0.25 | 0.1 | 0.25 | 1.20 |
| 09 | Mawhonic | 0.15 | 0.25 | 0.15 | 0.2 | 0.1 | 0.25 | 1.10 |
| 10 | Alder Bedo | 0.15 | 0.15 | 0.3 | 0.25 | 0.2 | 0.15 | 1.20 |
| 11 | Ody Mandell | 0.1 | 0.25 | 0.25 | 0.2 | 0.1 | 0.3 | 1.20 |
| 15 | Teemto | 0.2 | 0.15 | 0.25 | 0.15 | 0.25 | 0.2 | 1.20 |
| 16 | Was Sandage | 0.25 | 0.2 | 0.2 | 0.25 | 0.15 | 0.15 | 1.20 |
| 01 | Anakin Skywalker | 0.9 | 1 | 1 | 0.8 | 1 | 0.9 | 5.60 |
| 14 | Sebulba | 1 | 1 | 0.8 | 0.9 | 0.9 | 1 | 5.60 |
| 12 | Shrivel Braittrand | 0.2 | 0.2 | 0.2 | 0.25 | 0.15 | 0.2 | 1.20 |
| 17 | Scorch Zanales | 0.15 | 0.2 | 0.25 | 0.25 | 0.1 | 0.25 | 1.20 |
| 18 | Sebulba 2 | 0.15 | 0.35 | 0.25 | 0.25 | 0.15 | 0.15 | 1.30 |
| 19 | Anakin2 | 0.2 | 0.25 | 0.2 | 0.2 | 0.2 | 0.15 | 1.20 |
| 20 | Occo Ninebar | 0.3 | 0.15 | 0.15 | 0.1 | 0.3 | 0.2 | 1.20 |
| 22 | Knire Dark | 0.1 | 0.2 | 0.3 | 0.15 | 0.15 | 0.3 | 1.20 |
| 23 | Kraid Nemmeso | 0.2 | 0.25 | 0.15 | 0.3 | 0.2 | 0.1 | 1.20 |
| 24 | Tzidik Wrantojo | 0.25 | 0.15 | 0.2 | 0.2 | 0.15 | 0.25 | 1.20 |
| 21 | Darth Maul | 1 | 1 | 1 | 1 | 1 | 1 | 6.00 |
| 25 | Watto | 0.9 | 0.8 | 0.85 | 0.9 | 1 | 1 | 5.45 |
| 26 | Darth Vader | 1 | 1.75 | 1 | 1 | 1 | 1 | 6.75 |

### Rules the files follow (regular pods = 02..24 except 14, 21)

- **Equal budget**: the six base abilities sum to **1.20** on 16 of 18 regular pods (Mawhonic 1.10,
  Sebulba 2 1.30). Values are on a 0.05 grid; observed ranges: MaxThrust 0.10..0.30,
  TopSpeed 0.10..0.35, SpoilerYaw 0.05..0.30, Defense 0.10..0.30, Repair 0.10..0.30, Cooling 0.10..0.30.
- **[MaxAbilities] = base + 0.70** for every ability on every regular pod, except Defense, which
  ends at 0.90 or 0.95 when base + 0.70 would exceed it (04, 08, 12 -> 0.90; 10, 16, 17, 18 -> 0.95;
  23 base 0.30 -> 0.90; Sebulba 2 TopSpeed 0.35 -> 1.00). Special pods 01, 14, 21, 25, 26 have
  Max = Base (fully upgraded from the start), so **01 Anakin and 14 Sebulba are excluded** from the fit.
- **A / B / C engine values** (A = base, B = base + 0.35, C = MaxAbility), linear in the ability t:
  - MaxThrust = 3750 + 5250 t (file Min/Max 3750 / 9000). Pod 08 has 5065.5 for 5062.5 (typo).
  - TheoreticalTopSpeed = 500 + 400 t (Min/Max 500 / 900); Vader 1.75 -> 1200.
  - SpoilerYawMoment = -5000 - 3000 t + SpoilerYawMomentOffset (offset 1000 on 04, 08, 23; 500 on 18;
    0 elsewhere). Pod 18 and 14 do not follow it exactly (hand-edited).
  - Defense = t (Min/Max 0 / 1). Pod 21 has base 1.0 but A/C 0.9.
  - BaseRepairAmount = 0.004 + 0.009 t (this does **not** use the file's Min/Max 0.0055 / 0.014).
    Exceptions: 09 C = 0.0121 (expected 0.0112).
  - BaseCoolingTime = 0.003 + 0.003 t (file Min/Max 0.0035 / 0.006 also not used).
    Exception: 07 Gasgano B/C = 0.00375 / 0.0048 (expected 0.0048 / 0.00585).
  - [Tuning] is identical on all pods (UseValues = 1, UpgradePercent = 0), so the game reads the
    abilities; whether it then uses the A/B/C rows or recomputes from Min/Max is not established
    here (Repair and Cooling A/B/C disagree with the Min/Max bounds, which matters if it recomputes).

### Other per-pod fields that differ

| Field | How it varies | Proposal for the six |
|---|---|---|
| [Pod Audio] Engine/Beam file, pitch, binder, volumes | three generic engine-sound bundles gen1 / gen2 / gen3 on regular pods (each bundle has fixed pitch/binder/volume values), unique sets for bosses | copy the whole bundle from the nearest shared pod in Racer stats (normalised 11-stat distance): ARK, NEV <- 11 Ody (gen3); BOL, RAT <- 10 Aldar (gen2); EBE <- 15 Teemto (gen1); ELA <- 09 Mawhonic (gen1). A judgment choice, not data-driven sound matching. |
| [CameraInfo] FirstPersonCamOffset | cockpit geometry (0,2,0 on most) | take from the converted cockpit model; not a handling value, not in the JSON |
| [Pod Setup] engines, [Wreckage] parts, engine piece counts (3 or 4), EmitSmoke, BreakRate | follow each pod's model (Ben has 4 engines, BreakRate 0.5) | from the converted model; note NEV's engines are built into the cockpit (0 separate engines) |
| [Engine Aerodynamics] SpoilerYawMomentOffset | 0 on 18 of 23 pods | 0 |
| [Pod Animation] RollTorque | -5 everywhere except Mars Guo -10 | -5 |
| [Collisions] ExplosiveWreckSpeed etc. | identical on all pods | unchanged |

## 3. Mapping and fit quality

Fit set: the 10 shared regular pods 02, 03, 04, 07, 08, 09, 10, 11, 15, 16 (01 and 14 excluded, see above).

**Result: Revenge's abilities do not follow Racer's stats.** Rank correlation of each Revenge ability
against the Racer stat that means the same thing:

| Revenge ability | Racer stat (same meaning) | Spearman, n = 10 |
|---|---|---|
| MaxThrust | Acceleration | -0.06 |
| TheoreticalTopSpeed | Max Speed | -0.16 |
| SpoilerYawMoment (turning) | Turn Response | +0.16 (Max Turn Rate -0.15) |
| Defense | Damage Immunity | -0.04 |
| BaseRepairAmount | Repair Rate | +0.22 |
| BaseCoolingTime | Cool Rate | -0.31 |

Over all 6 x 15 pairings the largest is MaxThrust vs Max Speed +0.63 (and vs Cool Rate -0.66); with 10
pods and 90 pairings, that is what chance alone produces, and none of them has a physical reason. With
Anakin and Sebulba added, no pairing exceeds 0.63 either. The Revenge designers rebalanced every pod
onto an equal 1.20 budget and did not carry Racer's numbers over.

Leave-out check of the chosen rule (below) on the 10 shared pods: mean error **0.060** per ability,
against **0.053** for simply guessing 0.20 everywhere. So the rule does not predict Revenge's own
choices; it is a consistent, explainable translation, not a fitted one.

### Rule used (most defensible given no correlation)

1. Pair each Revenge ability with the Racer stat of the same meaning (table above; higher = better
   in all six).
2. Take the new pod's percentile among all 23 Racer pods for that stat.
3. Map that percentile onto the distribution of that ability over Revenge's 18 regular pods
   (rank-preserving quantile mapping, so results stay inside Revenge's observed range).
4. Shift all six percentiles of the pod by one common amount until the abilities sum to Revenge's
   budget 1.20 (keeps the pod's own strong/weak profile while matching Revenge's balance rule),
   snap to the 0.05 grid, and fix any rounding remainder on the ability furthest from its raw value.
5. MaxAbilities = base + 0.70 (Defense capped at 0.95); A/B/C from the linear rules in section 2.

## 4. Proposed values for the six (all derived from Racer 1999 via fit)

### [BaseAbilities] (MaxAbilities = each + 0.70, Defense max capped 0.95, Cooling capped 1.0)

| Code | MaxThrust | TopSpeed | SpoilerYaw | Defense | Repair | Cooling | Sum | Flag |
|---|---|---|---|---|---|---|---|---|
| ARK | 0.20 | 0.25 | 0.15 | 0.15 | 0.15 | 0.30 | 1.20 | derived from Racer 1999 via fit |
| BOL | 0.25 | 0.30 | 0.25 | 0.15 | 0.10 | 0.15 | 1.20 | derived from Racer 1999 via fit |
| EBE | 0.15 | 0.20 | 0.20 | 0.25 | 0.15 | 0.25 | 1.20 | derived from Racer 1999 via fit |
| ELA | 0.30 | 0.20 | 0.15 | 0.25 | 0.15 | 0.15 | 1.20 | derived from Racer 1999 via fit |
| NEV | 0.15 | 0.20 | 0.20 | 0.25 | 0.20 | 0.20 | 1.20 | derived from Racer 1999 via fit |
| RAT | 0.25 | 0.25 | 0.20 | 0.15 | 0.15 | 0.20 | 1.20 | derived from Racer 1999 via fit |

Percentiles behind them (Racer roster, 0 = worst, 1 = best) and the common shift needed for the budget:

| Code | Accel | MaxSpeed | TurnResp | DmgImm | Repair | Cool | Shift |
|---|---|---|---|---|---|---|---|
| ARK | 0.04 | 0.41 | 0.02 | 0.02 | 0.02 | 0.70 | +0.29 |
| BOL | 0.57 | 0.93 | 0.67 | 0.09 | 0.07 | 0.28 | +0.02 |
| EBE | 0.50 | 0.63 | 0.80 | 0.89 | 0.85 | 0.91 | -0.30 |
| ELA | 0.87 | 0.24 | 0.17 | 0.52 | 0.41 | 0.22 | +0.10 |
| NEV | 0.04 | 0.24 | 0.28 | 0.76 | 0.54 | 0.37 | +0.11 |
| RAT | 0.96 | 0.80 | 0.37 | 0.35 | 0.41 | 0.57 | -0.05 |

ARK is the weakest Racer pod in four of six stats and EBE one of the strongest; the 1.20 budget lifts
ARK and trims EBE, as Revenge does for its own pods.

### Derived [Engine], [Engine Aerodynamics], [DamageRepair] (A / B / C)

| Code | MaxThrust | TopSpeed | SpoilerYawMoment | Defense | BaseRepairAmount | BaseCoolingTime |
|---|---|---|---|---|---|---|
| ARK | 4800 / 6637.5 / 8475 | 600 / 740 / 880 | -5450 / -6500 / -7550 | 0.15 / 0.5 / 0.85 | 0.00535 / 0.0085 / 0.01165 | 0.0039 / 0.00495 / 0.006 |
| BOL | 5062.5 / 6900 / 8737.5 | 620 / 760 / 900 | -5750 / -6800 / -7850 | 0.15 / 0.5 / 0.85 | 0.0049 / 0.00805 / 0.0112 | 0.00345 / 0.0045 / 0.00555 |
| EBE | 4537.5 / 6375 / 8212.5 | 580 / 720 / 860 | -5600 / -6650 / -7700 | 0.25 / 0.6 / 0.95 | 0.00535 / 0.0085 / 0.01165 | 0.00375 / 0.0048 / 0.00585 |
| ELA | 5325 / 7162.5 / 9000 | 580 / 720 / 860 | -5450 / -6500 / -7550 | 0.25 / 0.6 / 0.95 | 0.00535 / 0.0085 / 0.01165 | 0.00345 / 0.0045 / 0.00555 |
| NEV | 4537.5 / 6375 / 8212.5 | 580 / 720 / 860 | -5600 / -6650 / -7700 | 0.25 / 0.6 / 0.95 | 0.0058 / 0.00895 / 0.0121 | 0.0036 / 0.00465 / 0.0057 |
| RAT | 5062.5 / 6900 / 8737.5 | 600 / 740 / 880 | -5600 / -6650 / -7700 | 0.15 / 0.5 / 0.85 | 0.00535 / 0.0085 / 0.01165 | 0.0036 / 0.00465 / 0.0057 |

SpoilerYawMomentOffset = 0 for all six. Min/Max rows (MinMaxThrust 3750 etc.) stay the common values.

### Open points

- Racer's numbers are trustworthy (one data-mined source, spot-checked against three others); the
  translation is not a fit in the statistical sense, because Revenge's own pods show no link to Racer.
  The values are a reasoned, budget-balanced proposal; play-testing should judge them.
- Whether the game uses the A/B/C rows or recomputes from Min/Max is open (section 2); the A/B/C rows
  above follow the pattern of the existing files.
- Audio bundle choice is a judgment call (nearest shared pod by Racer stats).
