# How Racer Arcade draws pods: blend state, animation, two-sided faces (Nov 13 2026)

Read-only analysis. Program addresses are CPU addresses (eprom.bin offset + 0x0C00FC00). Hardware
semantics from Odin (`ref\odin-src\src\odin\machine\boards\hikaru\hikaru_draw.hpp`,
`host\renderer\hikaru\renderer_scene.cpp`, `renderer_translucent.cpp`,
`machine\chips\antarctic\antarctic_mesh.cpp`, `antarctic_list.hpp`). **Guess** marks anything not
proven by code or data.

Scripts (all under `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\analysis\pod-render\`):
`dump_states.py` (every mesh of every pod part model, all 18 kinds, LOD 0..3 -> `mesh_states.json`,
`states_all.txt`), `summarize.py`, `overlap.py` / `b2b_all.py` / `b2b_classify.py` (back-to-back
triangles -> `b2b_all.json`, `b2b_classified.json`), `glass_alpha.py` (alpha of the texels a
translucent mesh actually covers), `probe.py`, `litscan.py`, `psm_survey.py` / `psm_list.py` /
`psm_show.py` (original pod PSMs, read only), `render_ark.py`, `render_ring.py`.
Images: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\pods\render-re\`.

## 1. Summary

- **No pod part is animated by the draw path.** No spinning nodes, no texture panning, no texture
  swapping, no engine glow, on any of the 18 kinds. The only moving things are whole-part matrices
  (body, each engine) from the pod simulation, the pilot's motions, and for ANA/BEN/GAS/SEB the
  cable/flap extras of their per-kind callback (player pod only).
- **Every pod material is the same**: 881 = 0x000D0881 (Gouraud-lit, fog, textured, alpha mode off),
  A81 = 0x00000A81 (blend mode 0 = source alpha over), C81 = 0x00200C81 (alpha-test index 32);
  untextured LOD 3 boxes use 0x00050881. Poly alpha 255 and vertex alpha 255 on every pod vertex.
  So the **only** translucency source is poly type 6 together with ARGB4444 texture alpha.
- **Translucent (poly type 6) pod meshes exist only on ANA and ARK, and only on the cockpit** (plus
  ANA's whole LOD 1/2, see table). **No engine mesh of any kind is translucent.**
- **ARK's inner ring is an opaque, static surface.** It is not see-through and does not move. The
  z-fighting comes from the model itself: the tunnel wall is modelled twice, back to back, at the same
  place (one copy facing into the tunnel, one facing into the ring body). The arcade draws every
  triangle one-sided, so only one copy is ever seen. The port draws pods two-sided, so both copies
  show and fight. Most arcade pods have such back-to-back pairs (table in 5).

## 2. Hardware meaning of the bits (Odin)

| Item | Where | Meaning |
|---|---|---|
| Poly type | 103/113 bits 11:9; alpha 31:24 | 1 opaque (depth write, no blend, no alpha test); 4 punch-through (alpha > 0.5, depth write); 6 translucent: drawn after all opaque types, **no depth write**, depth test on, fragments with alpha <= 1/255 dropped, blended with the material's mode, all translucent fragments of a pixel sorted far to near (`pass_rules`, `draw_scene`, `renderer_translucent.cpp`) |
| Material blend | A81 bits 17:16 | 0 source-alpha over, 1 product, 2 source-alpha added, 3 source over (`material_blend`) |
| Material alpha mode | 881 bit 20 | off on every pod material |
| Alpha | | texture alpha x vertex alpha (header 31:24) x poly alpha (103 31:24); pods: texture alpha only |
| Texcoord offset slot | vertex header bits 19:18 | adds the 0D1 panning register of that slot to the UVs (`copy_texcoords`, antarctic_mesh.cpp ~300). **0 on every pod vertex** |
| No-cull | vertex header bit 23 | triangle emitted twice: front, and back with reversed winding and **negated normals** (lit as its own side), same texture and UVs (`add_triangle`, antarctic_mesh.cpp ~334-382). Every other triangle is one-sided. Front side = the side the vertex normals point to (checked: winding agrees with vertex normals on 352/352 ARK engine and 1516/1518 ALD cockpit triangles) |

## 3. Draw path and per-frame code

| Address | What | Animates anything? |
|---|---|---|
| 0C1447EC | pod draw (slot, record, lod, display state): body matrix, PutStaticObject(cockpit), pilot (lod < 2, 0C1330A0), 0C13755A name tag (**guess**), PutStaticObject(binder), then for i < 4: push, engine matrix (record +76 + i*64), PutStaticObject(engine[lod][i]), pop; 0C1401C2 water reflection; 0C1436AE shadows; callback (state +196) | no: matrices come from the simulation, objects are drawn as stored |
| 0C1493B0 | PutStaticObject: per node, node matrix (static ObjectInfo copy), 0C014DE0 attribute block (materials/texheads as authored), 0C013E60 geometry call | no |
| 0C1436AE / 0C1435F8 | drop shadows: ground probe (0C109300), then model 3782 (unit flat square, poly type 6, unlit, texgroup 95 texture 1920 alpha 0..136) scaled 4 x 6 under the body and 5 x 2.5 under each engine; NEV (kind 16) body only; skipped for lod > 2 | no |
| 0C143F62 / 0C1442E2 / 0C144434 / 0C14452C | per-kind callbacks ANA / BEN / GAS / SEB (pod table +132). r5 = lod: they draw only for lod 0 (the player pod). Cables stretched between fixed points (pod-placement.md); ANA additionally draws engine flap objects (objects 25-64, pods.md), SEB extra parts. Frame selection of ANA's flaps not traced here | ANA flaps and the 4 kinds' cables only |
| 0C015000 | the only 0D1 (texture panning) emitter. Callers: 0C147414 (course 1/2 water, race-data.md 18) and 0C148804 (@CommonInitialize: slots 0..3 set to 0). No other code writes 0x0s06D1 | course water only |
| 0C1494FA | ambSetObjectTextureGroup (texture swap on an object). Callers 0C10EA1C, 0C11B294, 0C11E4BC, 0C11EDDC, 0C121154: outside the race draw (menu/select code region, **guess** from address) | not in races |

Texgroups are loaded once by DefineDispPod (0C1439EE); nothing in 0C1436AE..0C144A46 changes them.

## 4. Translucent pod meshes (all of them)

Values a converter needs: pass = arcade poly type 6 (after opaque, depth test on, depth write off,
sorted), blend = source alpha over, alpha = texture alpha (ARGB4444, so steps of 1/15).

| Kind | LOD | Part / model / mesh | Tris | Texture | Alpha of the texels it covers |
|---|---|---|---|---|---|
| ARK | 0, 1 | cockpit m420 me6 (canopy sphere and spine) | 90 | t217 64x64 | 17/255 (6.7 %) on 2885 texels = glass; 255 on 245 = frame |
| ARK | 0, 1 | cockpit m420 me0 (side sail) | 5 | t211 128x64 | 0 on 1575 texels, 255 on 5266, 1-14/15 on a thin edge = cut-out |
| ARK | 0, 1 | cockpit m420 me1 (side sail) | 19 | t212 128x64 | 0 on 1503, 255 on 3916, soft edge = cut-out |
| ARK | 2 | cockpit m421 me0 / me1 / me6 | 2 / 10 / 16 (me6 all no-cull) | t211 / t212 / t217 | as LOD 0 |
| ANA | 0 | cockpit m74 me9 (canopy) | 40 | t64 64x32 | 17/255 everywhere |
| ANA | 1, 2 | every mesh of cockpit m164/m165, binder m158/m159, engines m160-m163 | 221 + 16 + 634 (LOD 1) | texgroup 3 (all ARGB4444) | 255 almost everywhere (a few 221/238 texels); canopy t93 51/255 (20 %) |

Images: `ARK_cockpit_lod0_translucent.png` (blue = translucent), `ARK_cockpit_lod0_translucent_only.png`.
ANA's distant LODs are drawn in the translucent pass although nearly opaque (**guess**: authoring
choice so the canopy sorts with them).

## 5. Per kind: animated parts, translucent meshes, two-sided and back-to-back triangles

"No-cull" = triangles with header bit 23 (all on LOD 2 only, for every kind). "Back-to-back" =
coplanar, overlapping, opposite-facing triangles of one model (both one-sided). With one-sided
drawing only the copy facing the viewer shows; with two-sided drawing both fight.

| Kind | Animated parts (draw path) | Translucent | No-cull tris, LOD 2 (cockpit / engines) | Back-to-back tris, LOD 0 (cockpit / each engine); other LODs |
|---|---|---|---|---|
| ALD | none | none | 4 / 0 | 67 / 8; L1 cockpit 24, L2 cockpit 4 |
| ANA | flaps + 2 cables (LOD 0) | canopy L0; all of L1/L2 | 0 / 0 | 43 / 0; L1 engines 8 each |
| BEN | 4 cables (LOD 0) | none | 10 / 0 | 56 / 0; L1 cockpit 18 |
| EBE | none | none | 2 / 52 | 14 / 0 (left), 3 (right); L1 engines 8, L2 4 / 2 |
| GAS | 2 cables (LOD 0) | none | 18 / 0 | 10 / 0; L1 cockpit 24 |
| MAR | none | none | 0 / 16 | 41 / 0; L1 cockpit 7, engines 16; L2 cockpit 4 |
| MAW | none | none | 0 / 14 | 15 / 2; L1 cockpit 12, engines 20; L2 engines 4 |
| RAT | none | none | 8 / 8 | 76 / 0; L1 cockpit 24, engines 16; L2 cockpit 14 |
| SEB | 2 cables + extra parts (LOD 0) | none | 4 / 17 | 6 / 0; L1 cockpit 20, engines 32; L2 cockpit 12 |
| TEE | none | none | 0 / 0 | 36 / 5; L1 cockpit 12, engines 46; L2 cockpit 12, engines 8 / 4 |
| ODY | none | none | 7 / 20 | 52 / 0; L1 cockpit 6 |
| ARK | none | cockpit glass + sails | 20 / 132 | 0 / **32** (ring tunnel) |
| BOL | none | none | 28 / 0 | 24 / 12 |
| CLE | none | none | 20 / 0 | 40 / 0 |
| DUD | none | none | 12 / 26 | 28 / 0; L2 cockpit 7, engine 2 |
| ELA | none | none | 5 / 20 | 0 / 0 |
| NEV | none | none | 69 (one body) | 144 (body; NEV has no engines) |
| WAN | none | none | 25 / 92 | 8 / **108** |

Every kind also gets the drop shadows (section 3). Rates for the converter: there is no rotation axis,
rad/frame, texels/frame or frame rate to carry for any pod part. (The pilot's motions are 60 Hz
keyframes, pods.md.)

## 6. ARK (kind 11) engine in detail

- Engine objects 200/202 (LOD 0 and 1), 201/203 (LOD 2), 1467 (LOD 3 box). LOD 0 model 416 (left) /
  418 (right): 352 triangles, 9 meshes, all poly type 1, material 0x000D0881 / blend 0, all textures
  ARGB1555 with alpha 255, no no-cull, no texcoord offset slot. Nothing about it is translucent or
  animated.
- Meshes (texgroup 8): me0 72 tris t202 (side intake bodies), me1 14 t203 (front fan faces of the
  two intakes: a **static** fan-blade texture), me2 24 t204, me3 28 t205, **me4 124 tris t206 64x32
  (the central octagonal ring and the two lower tubes)**, me5 16 t207, me6 30 t208, me7 12 t209,
  me8 32 t210. Picture: `ARK_engine_lod0_meshes.png` (me4 magenta), `ARK_engine_lod0_mesh4.png`.
- **Inner ring surface** = 32 triangles of me4, tex t206 (opaque), lining the tunnel through the ring:
  16 face the tunnel (normals toward the ring axis; the surface you see), 16 lie in exactly the same
  places facing the other way, into the ring body (normals away from the axis; e.g. triangles 171,
  172, 177, 179, 181, 183, 186, 188 against 242-261). With one-sided drawing the second set is never
  visible. Picture: `ARK_engine_lod0_backtoback.png` (red = back-to-back, stippled = the fight), and
  `ARK_ring_arcade_vs_twosided.png` (top: arcade one-sided; middle: two-sided; bottom: the ring-body
  copy in red showing through).
- LOD 2 (model 417): 156 tris, 66 of them no-cull (me2 16, me6 14, me8 16, me9 20: tubes and the ring
  at that detail). Picture: `ARK_engine_lod2_nocull.png`.
- How it moves: only with the whole engine (engine matrix from the simulation, pod-placement.md).

## 7. Two-sided (no-cull) triangles

Hikaru emits a no-cull triangle twice at the same place: front with its own normals, back with reversed
winding and negated normals, same texture and UVs. So the inside of a no-cull tube shows the same texture,
lit as if its normal pointed at the viewer. Only LOD 2 models use the bit (table 5).

## 8. Revenge side (original pod files, read only)

- PSM keys in the 23 original pods (`C:\Games\Star Wars Racer Revenge\PODS\PODnn\PODnn.RES`):
  blend = `alphaBlendingOnOff` + `inputColorA/B`, `inputAlphaC`, `inputAlphaD` (PS2 GS
  (A-B)*C+D) + `fixedAlphaValue`; `zWriteDisabled`; `alphaTestOnOff`; texture frame animation
  `TextureAnimation` + `animatedMapFramesPerSecond` (binder 16 fps, nn_1ghost 8 fps); UV transform
  `TexGenMatrixRow1..3` + `VertexAnimation` with `DiffuseTexTransCompressed` (flames). **No culling
  key** exists in PSM.
- The arcade translucent type with blend 0 maps exactly onto `alphaBlendingOnOff = 1`, `inputColorA = 0`,
  `inputColorB = 1`, `inputAlphaC = 0`, `inputAlphaD = 1` (Cs*As + Cd*(1-As)), `zWriteDisabled = 1`:
  the same keys as the originals' `concringnn.psm` and pass 1 of POD23 `pod_wndw.psm`. Texture alpha
  then has to sit in the page (PS2 0..128 scale: 17/255 -> about 9/128).
- Engine motions (MCF/MOT) and TextureAnimation/TexGen are **not needed** for arcade parts: the arcade
  has nothing to carry into them. The borrowed flames keep their own TexGen/VertexAnimation.
- The port's GL path draws pods two-sided: no `glEnable(GL_CULL_FACE)` anywhere under
  `D:\Programming\GitHub\RacerRevengePC\platform\`, only disables (e.g.
  `platform\linux\native\vehicle_window_gl50ds_draw.cpp:715, 2090, 2883`); `docs\native-progress48e.md`
  lists "opaque, two-sided" as pending fidelity. Whether the original PS2 Revenge culls pod triangles
  was not checked (**open**).

## 9. What the converter should do

1. **Back-to-back pairs (the ARK ring fault; present on 17 of 18 kinds)**: move every triangle that has a
   coplanar opposite-facing partner by a small distance along its own normal (suggest 0.01 ft after the
   x3 scale; depth precision at 100 ft with a 1 ft near plane and 24-bit depth is about 0.0006 ft).
   Then the copy whose front faces the viewer is always the nearer one, which is what the arcade's
   one-sided drawing shows, both in a two-sided renderer (the port today) and in a culling one.
   Flag it as a geometry stand-in for one-sided drawing. A pure "drop the inner copy" rule is not
   general: a ray test calls only 10 of ARK's 32 and 0 of NEV's 144 back-to-back triangles hidden
   (`b2b_classified.json`), because the shells are open.
2. **No-cull triangles (LOD 2 only)**: today they are written twice at the same place; in the port's
   two-sided drawing the two copies fight (ARK LOD 2 engines: 66 per engine). Write the back copy with
   reversed winding and negated normals (as Hikaru does) and apply the same small offset as rule 1, so
   the result is right whether or not the renderer culls.
3. **Translucent meshes** (only ARK and ANA cockpits, ANA LODs 1-2): material with source-alpha blend,
   depth write off (keys in 8), alpha from the arcade texels. ARK's 114 poly-type-6 triangles currently
   use POD23's `pod_wndw` texture (material names are global, pod-convert.md fix 4); the arcade glass is
   t217 at 17/255 with an opaque frame, and the side sails t211/t212 are cut-outs (alpha 0/255).
   Everything else stays opaque.
4. **No animation to convert**: no engine MOT tracks, no TextureAnimation, no TexGen scroll for arcade
   parts. The ARK ring and the intake fan faces are static in the arcade.
5. For all pods, one rule: the above are data-driven (poly type, no-cull bit, coplanar opposite
   faces), never keyed on a kind.
