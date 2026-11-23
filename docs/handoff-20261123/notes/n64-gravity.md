# N64 Racer airborne gravity (for the Malastare 100 jump; RE Nov 21 2026)

N64 USA ROM, RAM = ROM + 0x7FFFF400, disassembly `racer-n64\work\re\code.asm`. PC names from `ref\SW_RACER_RE`
(`swrRace_ApplyGravity` 0x004774F0, swrRace.c 3410) for orientation only; every number below is read from the N64 code.

## 1. The constant
- **0x800A3080 = 32.0f** (read from the ROM; no N64 code writes it: the only reads are 0x80065E1C).
- **gravityScale (pod +0x190)**:
  - Set to 32.0 (0x42000000) at pod init / respawn: 0x800705E0..0x80070610 and 0x80072E10..0x80072E40.
  - Every frame outside zero-g, the move step (0x8006B63C, `jal 0x80065E18`) sets
    `+0x190 = [0x800A3080] x 0.25 (0x3E800000) = 8.0` and world gravity `+0x194 = (0, 0, -1)` (0x800A3084)
    (0x80065E18..0x80065E40). In ZON it is not called (n64-zero-g-physics.md 2.1).
  - So in a race, on the ground and in the air alike, **gravityScale = 8.0**. One value for every pod: no per-pod gravity
    (the handling table 0x800A368C has none; nothing else writes +0x190 for a pod).

## 2. The formula (ApplyGravity, N64 0x800672D4..0x80067714)
- dt = the frame step in seconds, a double at 0x80120BF0 (0x800672E8 / 0x800672F8).
- Airborne flag: ground distance > 30 (0x41F00000) -> flags1 |= 0x200 (0x8006740C..0x80067440).
- fallVelocity (pod +0x1B4), when the hover distance is above 12 (0x41400000, 0x80067444..):
  - speed (+0x1A0) >= 0: `fallVelocity += dt` (0x80067478..0x80067490);
  - reversing: `fallVelocity += 2.0 x dt` (0x80067494..0x800674B8).
  - Within 12 of the ground the hover spring term replaces it (0x800674BC..0x80067554).
- **Per-frame fall step** (pod +0x1B0), 0x80067558..0x8006758C:

      fallStep = dt x 30.0 (0x41F00000) x gravityScale (+0x190) x fallVelocity (+0x1B4)

- Glide: if pitch (+0x2FC) < 0, speed >= 0 and fallStep > 0: `fallStep x= (1 + 0.9 x pitch)` (0.9 = [0x800AD4C8])
  (0x80067590..0x800675D4). The step is clamped to the ground distance (hard landing -> 'HitBotm').
- Applied as a displacement: the motion vector at a2 += world gravity x fallStep (0x800676C0..0x80067704).

## 3. What it amounts to
fallVelocity grows by dt per second, so after t seconds in the air fallVelocity = t (plus what it carried at take-off,
near 0 after the hover band). The fall per second is fallStep / dt = 30 x 8 x t = 240 t:

    drop(t) = 120 t^2 N64 units        ->  N64 airborne gravity g64 = 240 units/s^2  (nose-up glide lowers it by up to 90 %)

- At 1 unit = 1 ft: **240 ft/s^2** (7.45 g). Revenge's own pods: AirborneGravity 10 x 32.2 = **322 ft/s^2** (PHY, e.g.
  Pod03; authored_parameters49n.cpp:127), Gravity 3..6 x 32.2 near the ground.
- **At the current track scale 2.5** (n64-scale.md 6: distances x 2.5, speeds unchanged, so a jump lasts 2.5x as long): for a
  pod to land where the N64 lands, `g = g64 / 2.5` = **96 ft/s^2** (2.98 g), i.e. about 3.4x weaker than Revenge's 322.
  General: g_revenge = 240 / SCALE ft/s^2 (SCALE = n2r_space.SCALE).
- **Written (Nov 21):** every converted N64 course carries `[Environment] AirborneGravity = 240 / SCALE` in its .scb
  (n2r_convert `N64_AIRBORNE_GRAVITY`, from the one scale setting n2r_space.SCALE; "96.0000" at 2.5). Port data: absent on
  every stock course, where pods keep their PHY AirborneGravity. The port replaces the airborne value (+37C) with it for
  every racer, player and AI (dlc-r1-n64-fog-gravity.txt 2b). Not reproduced (flag): the x2 growth while reversing, the
  nose-down glide term, the N64 hover spring within 12 units, far-AI line following (section 4).

## 4. Far-from-camera N64 AI cannot fall
- `swrRace_UpdateGroundContact`: a non-local pod without FORCE_GROUND more than about 1,000 units from every camera does no
  ground ray; it takes the spline height for z (spline eval 0x8003B184), is marked GROUND_CACHED and SPLINE_SNAP; between
  400 and 1,000 units its height is blended toward the spline height (`(lod - 400) / 600`) (n64-loader-re.md 2.7,
  n64-zero-g-physics.md 2.1).
- The Fall-surface respawn (`vehicle_reaction` 0x4000) only fires for a pod that is local, near a camera or FORCE_GROUND
  (n64-loader-re.md 2.8).
- So on the N64 an AI racer out of camera range rides the racing line over a pit like the Malastare 100 jump (the line
  peaks 196 ft above the pit floor there, n2r-m100.md) and never drops in; only the player and AI near the camera use the
  ballistic fall above. The port simulates every AI through the jump, so even with N64 gravity the AI result can differ.
