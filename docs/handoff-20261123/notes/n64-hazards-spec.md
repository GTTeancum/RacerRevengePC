# N64 Racer track hazards: energy column (trigger 104), breakable asteroids (102), door (101), fan (looping animation)

Nov 15 2026. Spec for the port's canonical-N64 trigger actions, in the form of notes\n64-ice-physics.md. Evidence: N64 USA
disassembly `racer-n64\work\re\code.asm` (RAM = ROM + 0x7FFFF400), the ROM itself for data tables, PC names and
decompiled bodies from `ref\SW_RACER_RE` (facts only). **Verified** = read in N64 code (or in PC code that matches the
N64 jump-table target); **inferred** and **open** are marked. Track data: Vengeance export
(`racer-n64\export\145_Vengeance\triggers.json`, collision_surfaces.json, model 145). Scripts: `racer-n64\work\ven\`
(trig.py, door.py, anims.py, spot.py).

## Plain-language summary

- **Energy column ("death binder", trigger 104, Vengeance only).** An invisible gate stands across the racing line at the
  pink column in the open hall of the zero-g section. A pod that passes through it explodes on the spot: one engine
  blows (left or right, 50/50), the pod spins out and goes through the normal crash-and-respawn sequence. It works every
  lap and for every pod that crosses it, but the N64 only lets the local player and pods near a camera explode.
- **Breakable asteroids (trigger 102, Oovo IV).** Each asteroid has a gate in front of it. The first pod through the
  gate smashes the asteroid: the rock and its collision disappear for the rest of the race, the pod is slowed and takes
  a small amount of damage (about 0.15 of a part's health spread over three random parts; four times that while
  boosting), a debris effect plays for about 2 s with a crash sound. For the player the debris travels with the pod; for
  a computer pod a big debris burst stays where the asteroid was.
- **Door (trigger 101).** An animated iris door. Before anyone triggers it, it cycles on its own (13.3-s loop). The first
  pod to reach its trigger freezes it shut (its closed pose leaves the racing line itself clear but stands within about
  8-10 ft of it); a pod that triggers it more than 3 s later opens it in 0.67 s, the next one more than 3 s after that
  closes it again, and so on.
- **Fan (looping scenery).** A three-armed fan over the corridor before the zero-g section turns once every 24 s
  (15 deg/s) from the moment the track loads, its collision turning with it. Its arms pass within 2.6-10.9 ft of the
  racing line during the turn.
- **Revenge data proposal:** two new trigger action types (`ExplodePod`, `BreakObject`) and a few new trigger keys on the
  `.tob` TriggerN rows the original already has. No stock course uses them, so stock is untouched.

## 1. How the N64 stores and tests a trigger (all four use this)

### 1.1 Record (0x2C bytes, export `triggers.json`; n64-loader-re.md 4)
`center` (3 floats, N64 space, +Z up) | `direction` (3 floats) | `size_xy` | `size_z` | `affected_node` (pointer) |
`type` (s16 at +0x24) | `flags` (s16 at +0x26) | `next` (+0x28). Trigger lists hang off surface records (behaviour +0x3C);
the `meshes` list of the export = the collision meshes whose surface carries the trigger.

### 1.2 When a trigger is tested (verified, 0x800665A4, called from swrRace_UpdateSurfaceTag every frame)
- Only the triggers of **the surface under the pod** are walked: in normal driving the face the pod's ground ray hits; in
  zero-g flight the spline-baked track mesh under the pod's progress (n64-zero-g-physics.md 2.1). Plus the track's one
  global trigger (0x800A66D4; none on Vengeance).
- Skipped when (flags at +0x26): bit 0x1 (handled / disabled); bit 0x2 and pod speed (+0x1A0) < 150 (0x43160000);
  bit 0x20 and the pod is not LOCAL (flags0 0x20); bits 0x4 / 0x8 / 0x10 and the pod's lap (+0x1E70 -> +0x78) is 0 / 1 / 2.
  All Vengeance triggers have flags 0.
- `swrObjTrig_FindOrCreate` (N64 0x8007C4C8, PC 0x47C7D0) returns the trigger's live object, NULL if it has FIRED.

### 1.3 Volume (verified, 0x800666A0..0x80066A44)
Let h = size_xy / 2, v = size_z / 2, d = direction, and the **segment** A..B = center +- h (d.y, -d.x) (horizontal, across
the direction). C = closest point of the segment to the pod (x, y), t its parameter clamped to [0, 1].
- **Types 102 and 104 ("plane" triggers, 0x66 / 0x68):** fire when 0 < t < 1 (strictly inside the segment's span),
  |pod.z - center.z| < v, and the pod **crossed the plane through the centre with normal d** between its previous
  position (+0x16C) and its current one (+0x50), in either direction (sign test 0x8006692C..0x800669C0).
- **Every other type (101 among them, a "capsule" trigger):** fire when the horizontal distance from the pod to C < h
  and |pod.z - center.z| < v. (n64-loader-re.md 10.2 calls it a vertical cylinder; it is a vertical capsule around the
  segment; for d with |size_xy| small it is a cylinder.)
- Then the handler 0x8007CD50 (type dispatch: 100-108 jump table at 0x800ADBE4; 201-213 at 0x800ADBB0; 301-314 at
  0x800ADB78; 501 and 900 separately).

## 2. Energy column, trigger 104 ("kill (Vengeance)", PC name swrModel_TriggerType_KillPlayer)

### 2.1 Data (Vengeance)
- One record (off 0x3CC60): center N64 (-9961.3, 2220.5, -512.4), direction (-0.02, 1.0, 0), size_xy 39.6 (h = 19.8),
  size_z 156.4 (v = 78.2), flags 0, no affected node. Carried by mesh 0x3BF48: the column (80 collision faces, visual,
  no surface tags, node flags2 0x409F, admitted for planet track 0), N64 z -1,821..-79.
- The racing line crosses the plane at N64 (-9950.3, 2221.2, -532.9): **11.0 ft across** (inside h = 19.8) and **20.6 ft
  below the centre** (inside v = 78.2), heading almost exactly against d (cos -0.995). The straight-down ray from that
  point hits the column mesh 56 ft below. So the gate covers the racing line itself (section 2.4).

### 2.2 What happens (verified)
Handler 0x8007CF38: `r = rand() / 2^31` (0x80082BE0, /0x4F000000); `r < 0.5 ? swrRace_Explode(pod, 1) :
swrRace_Explode(pod, 2)` (N64 0x80070764, PC 0x004741D0 `swrRace_Explode(swrRace*, char)`, HANG in SW_RACER_RE). No
flag is set on the trigger, so **it fires on every crossing, every lap** (no one-shot).

swrRace_Explode (N64 0x80070764..):
- **Gate:** returns at once unless the pod is LOCAL (flags0 0x20) or FORCE_GROUND (flags1 0x04000000, "gates the full
  ground/spline update", see the flag writer in section 10.1); returns if flags0 has RESPAWN / RESPAWN_INVINC / DEAD (0x7000) or
  flags1 already EXPLODING (0x4000). **So a distant computer pod passes through unharmed** (verified).
- Clears flags0 BOOSTING (0x00800000) and CAN_CHARGE_BOOST (0x00200000).
- flags1 |= EXPLODING (0x4000). +0x310 = 2.0 + rand() (an explosion timer of 2-3 s, inferred from its use; its reader
  not decoded).
- Mode 2: flags1 |= EXPLODING_LEFT (0x8000), engine-explosion effect 0x80075648(pod, 1, ..., 0.3) at the left engine
  position (+0x3C0). Mode 1: EXPLODING_RIGHT (0x10000), effect 0x80075648(pod, 2, ..., 0.3) at the right engine (+0x400).
- Then two calls 0x80065810 (values 2.0 / 3.0 and 2.2; inferred: camera shake or rumble), and two random-pitch spatial
  sounds through 0x80008BC4 with range 10..500 ft (table 0x800AD804..; sound ids not decoded).
- After that the pod runs the game's ordinary explode -> death camera -> respawn on the spline sequence
  (swrRace_HandleDeathExplosion / swrObjcMan_UpdateDeathCamera, n64-loader-re.md 2.8); not decoded here.
- **Player vs AI:** no difference in the trigger; the Explode gate above (local player or near a camera) is the only one.
- **Reset:** nothing to reset (the trigger never disables itself).

### 2.3 Damage per part
Explode does not hand out part damage; it is the destroyed state itself (the same function the impact path calls:
callers 0x8006BC2C / 0x8006BC3C in the contact step, 0x8006D0A4).

### 2.4 Open
- Whether a pod exactly on the racing line explodes: geometrically the gate covers the line, so it would, **if** the
  spline-baked mesh at that progress is the column mesh (the down ray says yes for 2 of 20 samples on that segment).
  The N64 bake rule (`swrSpline_BakeSampleTrackMesh`, 0x8007FB08 region, surface word +0x00 bit 0x8 "spline-ambiguous")
  is not decoded. Needs the bake decoded or a look at the running N64 game before the port relies on it.

## 3. Breakable asteroids, trigger 102 ("asteroid break", swrModel_TriggerType_BreakDestructable_102)

### 3.1 Data (Vengeance; Executioner and The Gauntlet not surveyed)
23 records, all flags 0, all inside the zero-g section. Each `affected_node` is a Basic node holding one asteroid (one
20-face collision mesh + visual), 9-173 ft from the line. 20 triggers are carried by floor meshes under the line (Fast
floor, ZOn floor, untagged floor): in flight these are the baked surface, so a pod crossing the gate fires them. 3
(0x41F80, 0x454B0, 0x47690) are carried only by the asteroid's own mesh, which is never the surface under a pod in
flight, so those three never break in a race (they stay solid obstacles).

### 3.2 What happens (verified: jump table entry 2 -> 0x8007CEAC -> jal 0x8007C818, the handler of 100; PC
`swrObjTrig_HandleCrashHitTrigger` 0x47CA90 decompiled)
- On the pod: `accelThrust *= stdMath_Decelerator(2.5, dt)` (a one-frame thrust cut) and the pod event `'Hitt' 'T201'
  300.0`.
- **'Hitt' T201 on the pod** (N64 0x80071C80.., pod event handler; PC swrObjTest_F4 HANG): value 300 x 4 if the pod is
  WAS_BOOSTING (flags0 0x40000000), divided by 2000 -> 0.15 (0.6 boosting); split into 3 equal hits of 0.05 (0.2), each
  on a random part `rand() % 6` through 0x80071820 (inferred: per-part damage); +0x2B8 = 1.0; the local player's value
  also goes into the rumble queue (0x800A5B70); then a hit sound with random pitch (sound 0x37 when the pod's damage
  state +0x1998 is below a threshold; not fully decoded).
- On the asteroid: affected node visual off (NodeModifyFlags(node, 2, -4, 0x10, 3)) and **collision off**
  (NodeModifyFlags(node, 0, -3, 0x10, 3)); trigger object FLAG_ACTIVE | FLAG_FIRED.
- Debris effect: FX model slot 0 (swrObjTrig_ModelArray2[0]; the planet's break model, N64 Oovo IV model 291 at 0x8007E724) made visible with its animations started from time zero (EnableFXAnimation(0)).
  - **Player:** the FX is placed at the trigger centre, then every frame (swrObjTrig_F2, PC 0x47C500) at the pod's
    position + 12 x the pod's forward axis (vB), z offset 0 for 102: the debris travels with the pod.
  - **AI:** the FX gets a uniform scale of 8 at the trigger centre and stays there (test_obj_transform_ptr = NULL).
  - FX material alpha = (2 - t) / 2 x 255 for t <= 2 s, then 0. Model 291 finishes at 1 s, while the material is still about half opaque.
  - The pod's velocity at impact x 0.8 / dt is stored on the trigger object (no reader found in the decompiled F0/F2).
- Sound: spatial sound 0x39, pitch 0.25 + 0.15 x rand, at the pod, range 10..500 ft.
- **End:** swrObjTrig_F0 (0x47C390) waits until FX animation 0 is no longer active, then MaybeResetAnimation: trigger
  record flags |= 1 (**disabled for the rest of the race**), FX stopped and hidden, object freed. The asteroid stays
  hidden and without collision; nothing re-shows it during the race. **Reset only by reloading the track** (race
  restart: 'Load' / 'RSet' free the trigger objects and the model is loaded fresh).
- Player vs AI: same break and pod damage; only the FX placement differs (above). The trigger's own flags could restrict
  it to the local pod (0x20), none of Vengeance's do.

## 4. Door, trigger 101 ("door animation", swrModel_TriggerType_StartAnimDoor)

### 4.1 Data (Vengeance)
One record (0x54ACC), center (-5922.9, 5171.7, -506.7), direction (0.8, 0.59, 0), size_xy 80.4, size_z 145.0 (a capsule
trigger, 1.3), carried by mesh 0x5495C, 269 ft of line before the door. Affected node 0x2E534 (TransformedWithPivot,
3 collision meshes, 64 faces + visual), animation 0x644F8: axis-angle, 2 keys, t = 0 -> (0, 0, 1) -54.5 deg (= the node's
own matrix: closed), t = 13.33 -> (0.416, 0.808, 0.416) -102.1 deg (open); flags 0x11000030 = ENABLED | RESET | 0x20 |
LOOP (SW_RACER_RE ANIMATION_* values).

### 4.2 What happens (verified, 0x8007CE30)
- From load the animation runs on its own: ENABLED | LOOP at speed 1, so the door swings closed -> open over 13.33 s and
  jumps back, until the trigger first fires.
- First activation: FindOrCreate takes the node's type 8 / 9 animations and sets **time 0, speed 0** (door frozen at
  the closed pose); the handler sets the object's FLAG_ACTIVE (its timer +0x10 now runs in F0) and clears LOOP; the
  speed change needs timer > 3.0 s, so nothing more happens.
- Any later activation with the timer > 3.0 s: timer = 0; if the animation time <= 0 -> speed +20 (13.33 s / 20 =
  **0.67 s to open**), else speed -20 (0.67 s to close). Without LOOP it stops at the end it runs to.
- So **the door closes again**: it toggles on every activation that comes more than 3 s after the previous toggle (or
  after the first activation). A pod triggers it each frame it is inside the capsule and on the carrier surface.
- Closed pose vs the racing line: the line itself passes through the closed door's opening (no crossing on the exact
  line); offsets of 8 ft sideways or 10 ft up hit its collision.
- Correction to `notes\n2r-ven.md` / `n2r_anim.py`: they say the first pod meets the door open. By the reading above the
  first pod meets it frozen closed; later pods meet it open or closed by the 3-s toggle. The converter writes it open.

## 5. Fan (looping animated scenery, Vengeance node 0x3575C)

- Animation 0x6496C: axis-angle, 5 keys at 0, 6, 12, 18, 24 s, axes about +-(0.2, -0.9, -0.2), angles 94.0, 179.2, 92.4,
  27.4, 94.0 deg; flags 0x11000030 (ENABLED | RESET | LOOP): plays from **track load** (not race start), 24-s loop =
  one full turn, about **15 deg/s**, interpolated by quaternion slerp between keys with a linear factor (PC
  swrModel_AnimationInterpolateAxisAngle / swrModel_AnimationComputeInterpFactor).
- Three arms; 144 admitted collision faces under the node; the collision descent multiplies node transforms, so the
  collision turns with it (n64-loader-re.md 7). Clearance from the racing line over the loop (0.1-s steps): 2.6 to
  10.9 ft; never 15 ft. Not trigger-driven; no damage of its own (contact goes through the ordinary impact path).
- The converter today writes it frozen at 21.7 s of its loop (10.9 ft) and moves the AI lane 52 ft aside
  (`n2r_anim.lane_clear_poses`).

## 6. Revenge data the converter would write (proposal; inert on stock)

What Revenge already has (all 13 stock `.tob`, surveyed): `Triggers NumberOfTriggers`; `TriggerN`: Zone, Position,
Radius or Box + Yaw / Pitch / Roll, Name, IgnoreTrigger (PLAYER / AI / ANIMATION), StartEnabled, OneTimeOnly,
RandomTriggerPercent, NumberOfActions; `TriggerNActionM ActionType` = LostAI (TargetNodeIndex), Animation (OneTimeOnly,
AnimationNumber), PodSound (FileName, Position, MinDistance, MaxDistance), Sound (FileName, IsGlobalSound, Positional),
Particles (OneTimeOnly, EmitterNumber, InstanceNumber, TurnOn). **No damage or destroy action exists.**

Proposed (new keys and action types; no stock row has any of them, so every stock course runs unchanged; whether the
original Revenge reader tolerates an unknown ActionType value is not RE'd here: the port's reader must skip unknown
types and keys, as the original does for keys):

| N64 | Revenge row | Keys |
|---|---|---|
| plane volume (102/104) | `TriggerN` | Position = centre, Zone, `N64Volume = Plane`, `N64Direction = dx,dy,dz` (Revenge space), `N64HalfWidth` = size_xy/2, `N64HalfHeight` = size_z/2 (fires when the racer's step crosses the plane inside that rectangle, either direction) |
| capsule volume (101 etc.) | `TriggerN` | `N64Volume = Capsule`, same three keys (horizontal distance to the segment < half width, height within half height) |
| per-surface activation (1.2) | `TriggerN` | `SurfaceObject = <COL name>` (the trigger is tested only while that COL is the racer's ground body, or in zero-g flight the floor under the line); absent = always |
| trigger flags | `TriggerN` | `MinSpeed = 150` (bit 0x2), `LocalOnly = T` (0x20), `Laps = 1,2,3` (0x4 / 0x8 / 0x10); StartEnabled T; OneTimeOnly T for 102 (disabled after the first break), F for 104 / 101 |
| 104 explode | `TriggerNAction1` | `ActionType = ExplodePod`, `Side = Random` (N64: 50/50 left / right engine), `Gate = LocalOrNearCamera` (the N64 Explode gate; the port's own equivalent of FORCE_GROUND to be chosen) -> the port maps it to Revenge's own pod-destroyed path (the wreck the port already runs) |
| 102 break | `TriggerNAction1` | `ActionType = BreakObject`, `TargetObject = <name>` (the asteroid's own COL + visual pieces, hidden and collision off for the rest of the race), `ThrustDecelerator = 2.5`, `Damage = 300` (N64 'Hitt' T201 units: /2000, x4 when boosting, 3 random parts of 6), `Effect = <debris model>`, `EffectFollowsPlayer = 12` (ft ahead of the player; AI: scale 8 at the centre), `EffectFade = 2.0`, `Sound = <file>` |
| 101 door | `TriggerNAction1` | existing `ActionType = Animation` (AnimationNumber) plus `N64Toggle = 3.0` (s between toggles), `N64Speed = 20`; the door node as a Revenge `.tob AnimationN` with its collision (UseCollision) |
| fan | `.tob AnimationN` | looping animation with UseCollision T, 24-s loop, slerp keys; no trigger |

Converter consequences: for 102 the asteroids would stop being "collision removed" and become their own named COL +
visual pieces per asteroid; for 101 and the fan, the animated nodes become Revenge animation objects instead of one
static pose. Until the port reads these, the current static writing (sections 4.2 / 5) stays.

## 7. The N64 floor table under the spline ("surface under the spline cursor") - decoded

Evidence: N64 0x8007F098 (bake callback, PC `swrSpline_BakeSampleTrackMesh` 0x47EA20), PC `swrSpline_ForEachSample`
0x47EE20 (decompiled; the N64 bake is driven the same way, samplesPerNode = 10), N64 0x80080408 (lookup, PC
`swrRace_GetTrackMeshAtCursor` 0x47F890), N64 0x8003A568 (sample band). Script: `racer-n64\work\ven\bake.py`
(exact float32 replay for Vengeance), result `racer-n64\work\ven\bake_table.json`.

### 7.1 Bake (at track load, once)
- **Sampling:** for every control point, for every next link, the cursor parameter t starts at 0 and adds 1/10 = 0.1f
  in float32 while t < 1: **10 samples per link at t = 0, 0.1, ..., 0.9 of the Bezier parameter** (the start of each
  tenth, not its middle; the float32 sums 0.3000000119, 0.6000000238, ... all stay just above k/10, so each sample
  lands in its own slot on Vengeance: 970 of 970 slots written once, none empty, none twice).
- **Slot:** `trunc(float32(float32(band + t) x 10))`, band = 0x8003A568(cursor, 0): the node index on the main path
  (cursor +0x2C branch flags 0), the node's own band number from its branch table (+0x42) on a branch path.
- **Ray:** from the spline point itself (EvaluateToMatrix 0x8003B02C translation; **no lift**), direction (0, 0, -1),
  length 5000 (0x800ADC50), with the ray's facing test **switched off** (0x80000520(1) before, (0) after: both sides of
  every face count), closest hit over the track model (0x80005134). **No hit: the same ray straight up** (z = +1.0).
- **Store:** table[slot] = the hit mesh (also when NULL), table base 0x8011DCF8. Bookkeeping: mesh +0x3C = the first
  sample that hit it; a mesh hit from two traversal orders gets the "spline-ambiguous" bit 0x8 in its surface word
  (0x8007F1D0..0x8007F218, assigning a default surface record 0x800A6708 when it has none) - this bit only changes the
  cursor re-binding, not which mesh a sample holds.
- At bake time no trigger has fired: every asteroid is present (none of them is hit by any Vengeance sample).

### 7.2 Lookup (every frame, every pod)
`slot = band x 10 + trunc(float32(t x 10))` (for t < 0: trunc(t x 10 - 0.9999990), 0x800ADC60). So a cursor at
t in [k/10, (k+1)/10) reads the mesh found by the ray at **t = k/10, the start of that tenth**. The cursor t is the
spline parameter whose node plane the pod has passed (PC `swrRace_AdvanceSplineCursor`, steps of 0.01), the same for
every pod at the same progress whatever its sideways offset.

### 7.3 What it means for the energy column (trigger 104)
- Link 57 -> 58 (exact replay): slots 0-3 (rays at t = 0, 0.1, 0.2, 0.3) = the hall floor 0x39AB0 (2,208 ft below the
  line), **slot 4 (ray at t = 0.4) = the column 0x3BF48** (87.5 ft below), 5-6 = 0x39AB0, 7-9 = 0x3A578.
- The racing line crosses the gate plane at t = 0.3777. A pod there reads slot 3 = the hall floor, which carries the
  asteroid gates, not the column's. The gate's plane is almost square to the line (cos 0.995), so a pod crossing it
  anywhere inside its 19.8-ft half width does so at t = 0.37-0.38; reaching slot 4 would need t >= 0.4, about 0.022 of
  the link (some 30 ft of progress) later than any pod can cross that fixed plane.
- **Faithful result: on the N64 the column's kill never fires for a pod flying through the hall, on the line or off
  it.** (Outside flight the surface is the pod's own ground ray, but the hall lies wholly inside the zero-g section; a
  reset inside the section keeps the pod in flight, n64-zero-g-physics.md 3.) The gate is data the game carries but,
  with this bake, cannot trigger in a normal race on Vengeance.
- So the port, applying the N64 rule exactly, must give "no explosion". The main session's 10-sample result agrees,
  but **the port's table is not yet the N64 table** (`native_zero_gravity50zg.h` readGraph): it samples each
  converted map link at t = (k + 0.5)/10 (middle of each tenth, on the straight node-to-node link, from line + 1 ft,
  owner of the highest face below); the N64 samples each N64 Bezier link at t = k/10, from the point itself, both face
  sides, ray down then up, and indexes by trunc(t x 10). To be exact it should sample the start of each tenth; for
  the column the converted links and the mid-point rule happen to give the same answer, which is luck, not rule.
- Proposal for exactness without changing the port's rule for other courses: the converter can carry the N64 table
  itself (per converted link: the N64 sample owners in progress order), or the port's table can switch to start-of-
  tenth sampling on the N64 line (LineHeight points) - to be decided by the main session.

## 8. Proposal: the N64 floor table carried in the track data (for confirmation; not written yet)

### 8.1 What the N64 keeps (section 7)
Per bake slot (band x 10 + tenth) exactly **one mesh pointer** (or NULL), table 0x8011DCF8. Nothing else per slot: no
distance, no position, no up/down flag (the up ray only matters for which mesh is found). Per mesh, separately: the
first slot that hit it (mesh +0x3C) and the "spline-ambiguous" bit 0x8 (surface word). The proposal carries the mesh
reference exactly and adds what the port needs to place the slot on its own (converted) line plus two checking fields.

### 8.2 Where: a new member `tracks\<code>.nft` ("N64 floor table")
- Plain-text parameter file in the same syntax as the `.spl` (sections, `Key=Value`, keys read by name, unknown keys
  ignored). A separate member, so the `.spl` 64-KiB text cap is not touched and the AI / map splines stay byte-identical.
- Inert on stock: no stock archive has the member, the original never opens it (it reads members by name), and the
  port falls back to its own sampling when it is absent. The converter writes it only for tracks with a ZeroGravity row
  (the only reader is the zero-g line table and the in-flight trigger carrier test), behind the same rule as LineHeight.

### 8.3 Layout
```
[FloorTable]
Version=1
SamplesPerLink=10                 ; N64 ForEachSample step (0x47EE20 / 0x8007F098)
LineGraph=map                     ; the converted spline the positions below refer to (map and AI share node numbers in n2r)
NumSurfaces=<S>
NumSamples=<N>

[Surface<i>]                      ; i = 1..S, one per N64 mesh any slot holds
Mesh=0x39ab0                      ; N64 mesh offset (provenance)
Objects=sc_z07_fast_floor_col,... ; every static COL instance (SLT name to the first dot) holding faces of that mesh, all zones
Ambiguous=F                       ; N64 spline-ambiguous bit 0x8 after the bake
FirstSlot=573                     ; N64 mesh +0x3C (provenance)

[Samples]
S<n>=<band>,<tenth>,<from>,<to>,<t>,<surface>,<dist>,<dir>     ; n = 0..N-1 in N64 bake order
```
Fields of a sample:
| Field | Meaning |
|---|---|
| band, tenth | N64 slot = band x 10 + tenth (band = node index on the main path, the node's branch band on a branch path) |
| from, to, t | where the slot STARTS on the converted line: the converted link (node numbers of `LineGraph`) and the parameter on it of the N64 sample point (t = k/10 of the N64 Bezier link) projected onto that link. The port's lookup: on link (from, to) at parameter u, the sample with the largest start <= u, else the last sample of the links before - the N64's trunc(t x 10) rule moved onto the converted line |
| surface | 1..S, or 0 = NULL (no face within 5000 ft down or up) |
| dist | ray hit distance in ft (checking only; the N64 does not store it) |
| dir | D = found by the down ray, U = by the up retry (checking only) |

In flight the port takes the sample's surface as the floor under the line: its Objects give the static owner(s) for the
surface rules (ZeroGravity / SurfaceSpeedBonus / Slow) and for a trigger's SurfaceObject carrier test (with
`--n64-hazards` every carrier mesh has its own instances, so a carrier matches exactly). A mesh split over several
zone / surface COL instances lists all of them; any of them stands for the mesh.

### 8.4 Size estimate (text; S about 75, one sample line about 40-45 bytes, one surface block about 120-160 bytes)
| Track | N64 points / links | Samples (links x 10) | Surfaces | Estimate |
|---|---|---|---|---|
| Vengeance | 97 / 97 | 970 | 74 (exact replay) | about 50 KB |
| Spice Mine Run | 97 / 104 | 1,040 | about 70 | about 55 KB |
| Beedo's Wild Ride | 135 / 139 | 1,390 | about 90 | about 75 KB |
Beedo's and Spice Mine Run get one only if they ever carry ZeroGravity (they don't), so in practice only Oovo IV tracks.
All far under the port's member cap (8 MiB) and parameter-file cap (2 MiB). Open: if the track spline type walks the
3-deep lookahead paths (non-"flat" Bezier types in ForEachSample), a link can be baked once per band of each path
through it; the converter then writes one sample row per (band, tenth) and the count grows by the extra bands (none on
Vengeance, which has no forks).

### 8.5 Built (Nov 16; key confirmed by the port session)
- `tools\n2r\n2r_floor.py`, called at the end of every conversion: writes `tracks\<code>.nft` when the track has a
  ZeroGravity row (the only case where the floor under the line is read: zero-g surface rules and, in flight, the hazard
  gates' carrier test); refuses with a converter error when such a track has a forked segment (Executioner: forks at
  28, 79, 80, 81 -> refused, nothing written).
- Bake model: the collision at track load (state 'load': no trigger fired, no animation posed, every asteroid present),
  this track's admitted face set; independent of the stand-ins or `--n64-hazards`, so both Vengeance builds carry the
  same 970 slots (only the Objects names differ: with `--n64-hazards` the carrier meshes have their own instances).
- Vengeance: 970 samples, 74 surfaces, 0 NULL slots, 0 up rays, 50,458 bytes (default) / 50,763 (hazards). Three
  surfaces are asteroid meshes the bake hits at load (0x4C3F0, 0x47B68, 0x4771C): they have no static instance in
  either build (removed in the default build, animation rows with `--n64-hazards`), written `Objects=none`. None of the
  three is a gate carrier.
- Checked by an independent text reader (`racer-n64\work\ven\nft_check.py`): header, counts, every field's bounds,
  links against the archive's own map spline, every Objects name a COL member, FirstSlot, increasing starts per link,
  slots 0..969 each once: 0 failures; slot-by-slot equal to the exact float32 replay (`bake.py`): 970 / 970; the
  column crossing (slot 573) reads the hall floor 0x39AB0, slot 574 the column.
- Spice Mine Run and Beedo's: no ZeroGravity row, no member written, archives byte-identical.

## 9. Malastare: falling pillar (312) and methane (314) - Nov 19 (notes\n2r-dug.md 1)
- **312 falling pillar (Dug Derby only).** Set-up 0x8007D944: local player only (flags 0x20); the affected subtree's
  animations lose ENABLED and LOOP (0x8007B544(node, 0x10000010, 0, 1.0)), so it stands at rest. Handler 0x8007D248:
  one-shot; ENABLED again at speed 0.6 (falls once, 2.6 s, stays down 17 ft beside the line); camera shake 'Shak' (4.0).
  Revenge data proposal (inert on stock, behind --n64-hazards like 101): the node as a `.tob AnimationN` with its collision,
  a capsule trigger row `ActionType = Animation` plus `N64Once = 1`, `N64Speed = 0.6`, `N64LocalOnly = 1`, `N64Shake = 4.0`.
- **314 methane (all three Malastare tracks).** Set-up 0x8007D974: local player only; hides an empty marker node. Handler
  0x8007CC48(trigger, 0): one-shot; the planet FX slot 0 object (Malastare: model 293) is moved to the marker, shown and
  its animations started. Proposal: a capsule trigger row `ActionType = SpawnEffect` (model 293 as `.tob AnimationN`
  without collision, placed at the marker), `N64Once = 1`, `N64LocalOnly = 1`.
- Default build (switch off): pillar standing (load state), methane not placed.

## 10. Converter, Nov 21: hazards on by default (port cr-29; keys of work\scratch\n64-hazards\STATUS-20261121.txt 4)
`n2r_convert.py` now writes the N64 hazard rows by default (`--no-n64-hazards` writes the old static stand-ins; `--n64-hazards`
is accepted and does nothing). One rule per N64 trigger type, from the data (n2r_hazards.py):
- 104 energy column -> ExplodePod (Plane); 102 asteroids -> BreakObject (Plane, OneTimeOnly) + one row per asteroid + debris
  row; 101 door -> AnimationToggle (Capsule); looping animated scenery with collision (fan) -> looping animation row.
- **312 falling pillar** -> AnimationPlay on the pillar row (its own 1.57-s fall keyed every 0.1 s, SequenceActive F,
  LoopSequence F, UseCollision T, out of the static world), `Rate 0.6`, `Show F`, `OneTimeOnly T`, `LocalOnly T` (set-up flag
  0x20, 0x8007D95C), camera shake keys below.
- **314 methane** -> AnimationPlay on an FX row per marker: model 293 (Malastare FX slot 0, 0x8007E7A4) placed at the marker
  node's world transform, `StartHidden T`, UseCollision F, `Show T`, `Rate 1`, `OneTimeOnly T`, `LocalOnly T` (0x8007D988).
  Motion: the rise of the burst's centre puff (node 0x148 translation, 2.0 s, under the largest copy 0xA8, scale 2) [flag:
  the three copies' different rises, the side puffs' spread, their spin (axis-angle loops 0.25-1.0 s) and the texture
  scroll are not reproduced: one rigid row per marker].
- Trigger types not in this list keep their static stand-ins in hazards mode too (307 doors at their load pose, 306, 308,
  212, 211, 310); the trigger poses are now applied for every type the hazard rows do not take.
- Spatial values scale with the track (n2r_space.SCALE): N64HalfWidth / N64HalfHeight, NearCameraDistance, EffectAhead,
  MinDistance, MaxDistance.

### Bumpy's Breakers collapse (306), Nov 23

The staged NU conversion binds trigger `n64_306_75238` to the floor collision on both
sides of the zone 8/9 boundary. Its target `nu_collapse_77220` owns both the structure
and its collision. The splash uses Aquilaris FX slot 1 (model 290). At 2.2 scale,
320 sampled mesh poses from the packed motion differed from the original transforms
by at most 0.027 feet.

The original trigger is local-player-only and skips lap one. Once armed, it starts
camera shake, removes the structure and starts the splash after 1.5 seconds, then
hides the splash after another 2.2 seconds. The cr138 native run confirmed both
floor bindings and the target collision at load. The pilot wrecked on the first lap.
In-race removal of the collision and the visible splash sequence still need verification.

### 10.1 Values decided from the N64 code
- **Trigger speed.** N64 0x8006C24C..0x8006C2A8 divides completed-frame displacement by elapsed time.
  Ground uses horizontal displacement. Zero-gravity flight and recovery use all three axes.
  At 0x8006C2AC..0x8006C2F4 a negative dot product with the racer's forward axis negates the speed.
  The port uses the same calculation in feet per second with Revenge's vertical Y axis. A reversing
  racer and a grounded racer with only vertical motion both fail a positive minimum-speed gate.
- **Pillar camera shake (decoded):** handler 0x8007D248 sends cMan 'Shak' with (A = [0x800ADC0C] = 0.05, rate = 4.0). The
  camera event (0x800440F0) stores amplitude +0x3A4 = 0.05, rate +0x3A0 = 4.0, offset +0x39C = 0. Every frame (0x80043B80)
  while A > 0: offset += rate x dt; on passing +-A the rate flips and the offset is clamped (a triangle wave of amplitude A,
  4 A / rate = 0.05 s per cycle), added to camera +0x140 and half of it to +0x58, plus a controller rumble (0x80008B14).
  No decay. The 312 trigger object (dispatch 0x8007C008 -> 0x8007BDF4) sends 'Shak' (0, 0) once its timer passes 4.0 s.
  The converter writes `N64PillarShakeScale` from the track scale. The follow camera applies the full offset
  to target height and half to eye height. At scale 2.2 their signed peaks are 0.11 and 0.055 feet.
  In Dug Derby the AI pilot fired trigger 6. The attached-camera log in
  `evidence/cr96-nd-attached/RacerRevenge.log` recorded 249 eye offsets through 3.98419 seconds.
  All were within 0.000041 feet of the N64 update calculated from the printed timestamps. The attached
  camera still needs its target-angle response traced. Follow-camera movement, on-screen presentation,
  and the original controller rumble remain unverified.
- **RespawnDelay 2.5 s (decoded, with a flag):** the N64 death camera (mode 8/9, 0x800419EC) starts with its timer +0x70 = 1.0
  (0x80044098, state 0), then 1.5 s (0x80041A68, state 2), then respawns the pod (0x80041BF8: flags1 -0x4000, RESPAWN_INVINC
  0x2000 with 3.0 s at +0x30C). 1.0 + 1.5 = 2.5 s. [flag: the N64 exploding phase before the death camera is not timed; the
  port's destroyed state starts at Revenge's own wreck.]
- **Vengeance explosion eligibility.** Racer initialization clears flags1 at `0x80072B3C`. The explosion checks LOCAL and flags1 bit 26 at `0x80070774..0x80070798`. The scoring routine sets bit 26 at `0x80054384` after checking the last lap and LOCAL. A scan of the resident N64 code found this single direct constant OR of bit 26 into offset 0x64. The separate ground LOD check at `0x80067050..0x80067098` does not set this bit. The converted race therefore uses the local-racer gate. A future port of any additional writer must preserve its actual conditions.
- **Vengeance fan phase.** Both records start with current time zero, speed one and flags `0x11000030`. The 0x3575C fan has a 24-second period and the 0x2FE20 fan has a 13.333-second period. The original RStr handler calls `0x80051C80`. At `0x80051CA4` it clears the animation registry. Registration (`0x800309D8..0x800309EC`) sets a first-evaluation flag. The first update clears the flag without advancing time (`0x80006860..0x80006884`), then the clock advances by elapsed time. Converted N64 load loops start and reset at zero and run during the intro and countdown. Green-light phase depends on elapsed time.

### 10.2 Results (scale 2.5, packages rebuilt Nov 21)
- Vengeance: 1 ExplodePod, 23 BreakObject (+23 asteroid rows, 1 debris row), 1 AnimationToggle (door row), 2 looping rows
  (fan 0x3575C, 0x2FE20); 27 rows, 2,688 packed vertices of the 8,192 window.
- Dug Derby: 1 AnimationPlay pillar (with shake) + 6 AnimationPlay methane; 7 rows, 2,055 vertices.
- Malastare 100: 6 AnimationPlay methane; 6 rows, 1,920 vertices.
- Spice Mine Run: three looping collision-only nodes (0x50D64, 0x511D0, 0x5122C, no visual mesh) stay static at rest
  [flag: an animation row needs a drawn PSG; motion not reproduced].
- Bumpy's Breakers: the 307 door panels keep their static load pose (nodes a trigger set-up stops or poses are not treated as
  free-running loops). Beedo's, Scrapper's, Baroo Coast: no hazard rows.
### Vengeance asteroid impact audio (N64 USA)

Trigger 102 requests SFX 0x39 at 0x8007CB00. At 0x8007CB3C the call
passes channel class 6, pitch 0.25 plus the random contribution, unit gain
and the position pointer. At 0x80008400..0x80008418 the allocator reads
the queued sound ID and calls 0x80007FC8. This ID has bit 15 clear, so
0x80007CE4 uses the main bank at 0x800AFA54. 0x80007FFC..0x8000801C
reads instrument 0, validates the lower 15-bit index and allocates sound
0x39. The original bank loader uses ROM 0xAF4B0 (control) and 0xB8960
(sample table). This entry's sound record is at ROM 0xB22B0, wave at
0xB22C0, predictor book at 0xB22D8, sample data at 0x1E3520. Its 7992
encoded bytes contain 888 nine-byte N64 ADPCM frames or 14208 mono samples.
The book has order two and four predictors. The private conversion writes
PCM16 into the course archive. The tracked converter patch contains the decoder.
The audio manager calls the N64 AI frequency setter at 0x80007358 with
0x5622 (22,050 Hz) and supplies its return value to the synth config at
0x80007360. On play, 0x80008428..0x80008454 sends the per-voice pitch to
the sound player; the 44,100 Hz control-bank header alone does not establish
the effective playback rate. The native asset test decoded all 14,208 samples
and checked each of the four predictor tables separately against FFmpeg.

The staged NV.RES adds sound\n64_39.wav and changes only tracks\nv.tob among
existing members. All 14,208 packaged samples match the separate ROM decode
(PCM SHA-256 caff58777f6a8576ecdda67860bcb9ee1f1b55b4e418ddba6e7bb006480bb581).
The positional sound owner borrows that decoded clip for one world voice per
audible hit. The authored near and far distances are 22 and 1100 Revenge feet
at scale 2.2. Between those distances 0x80008CF4..0x80008D74 attenuates linearly.
The port supplies a horizontal listener bearing to the Revenge mixer's pan law.

At 0x8007C914..0x8007C934 trigger 102 multiplies the racer's stored accelThrust
(+0x1A4) by the result of 0x80081700. That function computes
1 - (dt / 0.03) / (dt / 0.03 + 2.5) using the constant at 0x800ADCB0.
The port applies the multiplier once to each engine's retained smoothed drive
(+0x70/+0x74), from which Revenge's 0x2E7720 resumes normal acceleration.

The N64 Oovo IV loader compares planet 6 at 0x8007E708, then passes model
0x123 (291) to 0x800305E8 at 0x8007E720. It stores the animation list in
FX slot 0 at 0x8007E738 and the root at 0x8007E754. The previous package
used model 82, based on a PC enum. The original effect has three distinct
fragment meshes. Eight transformed instances draw 24 animated fragments.
Three translations run once over one second and their child rotations loop.
At 0x8007B7BC the animation test returns true as soon as any enabled track
reaches its end or becomes disabled. The trigger then hides the shared model.

All three meshes share model material 0x2E4. Setup at 0x8007CC20 saves its
shader. The frame update at 0x8007C24C..0x8007C32C writes shader+0x25,
truncating 255 * (2 - age) / 2. The renderer packs that alpha into the
primitive-color command at 0x80035ECC..0x80035EEC. The authored first alpha
cycle is (TEXEL0 - ZERO) * PRIMITIVE + ZERO, and the second takes COMBINED.
The model therefore fades for one second and disappears on animation completion.
