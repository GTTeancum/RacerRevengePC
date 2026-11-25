# Opponent damage meters and wreck explosions

Addresses refer to the USA PS2 executable `SLUS_202.68`, SHA-256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.

## Opponent damage meter

The racer constructor calls 199840 and 199950 at 21085C and 210878.
Each overlay owns two quads. All 23 inspected pod archives have the same
`textures/aidamage.psm`. It contains one untextured ScreenObject pass with
blending enabled, depth test method 2, and fog disabled.

* 199C10..199C28 admits type 1 (AI), and type 0 (human) when racer+5C
  differs from the current view index. 199AC0 is its corresponding draw
  routine. Both destruction handlers (2261DC, 226764) disable component 240
  through vtable slot 18. The current player's engine gauge is a different
  HUD leaf at 18F680.
* 199C38..199D84 measures squared distance between racer+CA0 and camera+180
  before adding 7 to the anchor's world Y. 228724 publishes CA0 from the
  body scene node's local translation. The meter is at full opacity through
  distance 200 and disappears at 300. Between those distances the fade uses
  squared distance. The anchor is clipped through 2B01D0 and projected
  through 14EAE0.
* 2234E0 takes the maximum of damage lanes 7D0, 7D4 and 7DC. In source
  canvas units, the backing spans X -16..+16 and Y -3..+5. The fill begins
  at X -14, spans Y -1..+3 and has width 28 x (1 - min(damage, 1)).
* 19A224..19A4C8 chooses RGB 65C96E below 40% damage, C4D868 below 70%,
  then CA6060. Finished flag CD8 bit 0 selects F0F0F0, ahead of B9C bit 1
  tint 6A9DD4. The base is 646464. 133F20 halves the RGB bytes for the GS.
  19A4C8 shifts the fade value (0..512) right by two for GS alpha.

## Wreck explosion

* 212B5C..212C40 constructs distinct wreck explosion and spark emitters.
  2261B0 starts both. The EMTs match across the inspected pod archives.
  The explosion uses group 5, two sets of eight frames, a 0.4-second burst
  and 1250 ms base life. The companion spark emitter uses group 8 for 0.1 s.
  On Tatooine these correspond to atlas frames 76..91 and 104 respectively.
* 15EBC4..15EBD8 increments a process-wide emission counter. 15EC50 picks
  the animation set using the third random sample. 15ED68..15ED8C applies
  X/Z spread to the previous emitter position. 15EDA4..15EDD0 shortens life
  in four counter phases, while retaining the age computed from authored
  life. 15EE48..15EEE0 reuses the X/Y draws for texture mirroring.
* 15F450..15FBB8 builds each spark stream from sections of overlapping atlas
  sprites. The random XYZ triple sets velocity spread and mirroring. Three
  further draws determine growth, lifetime and section count. Tail and head
  thickness control sprite count, with an extra copy for a superbright head.
  Position, velocity and lifetime vary along the stream. The atlas advances
  once per stream.
* 19BE48 and the clone initialization set the direction and launch gate of
  each of the ten ember children. Destruction tests the stored gate against
  0.4 and draws a lifetime for each launched child. 161180 advances live
  particles. 162230..162844 computes brightness and dispatches draws.

The native renderer rebuilds the quads for each camera. It uses the recovered
four-corner lighting routine, shared with the port's existing PS2 particle
renderer, and the 2500 source depth buckets. Smoke, fire and sparks change
blend state in depth order within the wreck queue. GS additive alpha above
128 multiplies RGB before additive blending. The host draws through OpenGL.
Ordering between this queue and unrelated particle systems, and full-frame
pixel equivalence against PS2 output, have not been verified.
