# Racer Arcade pods and pilots: exporter and formats

Nov 12 2026. One tool for all 18 kinds: `tools\pod_export.py` (run `python -I tools\pod_export.py [CODE ...]`
from the racer-arcade folder; all 18 take about 35 s). Output in
`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\pods\<CODE>\`, previews in
`export\pods\preview\` (`contact_a.png`, `contact_b.png` = all 18 at a glance; `tools\pod_contact.py`).
Placement and stats research: `notes\pod-placement.md` (helper), program tables: `notes\program-map.md`.

Kind order (index = pod = pilot): ALD ANA BEN EBE GAS MAR MAW RAT SEB TEE ODY ARK BOL CLE DUD ELA NEV WAN.

## What is exported per kind

| File | Content |
|---|---|
| `<CODE>_pod_tg<a>[-<b>].gltf/.bin` | the pod as glTF 2.0, one scene per LOD part 0..3 (scene 0 = LOD 0, the player-slot detail model). Each part (cockpit, binder, engines) is a node with its placement matrix and a child node with the object-node matrix. LOD 0/1 also carry the pilot in the cockpit frame (the game draws the pilot only for LOD 0/1). |
| `<CODE>_pod_lod0_tg<a>.obj/.mtl` | LOD 0 with placement baked in, per-corner UVs |
| `<CODE>_pilot[_player].gltf/.bin` | pilot: one node per skeleton node (flat under the root, bind pose as TRS, parent/child/sibling in node extras), meshes on the model nodes, every motion of the pilot table as a glTF animation named from its GMOT string |
| `<CODE>_pilot[_player]_bind.obj`, `_skeleton.json` | bind-pose mesh; hierarchy, bind matrices, bounding spheres |
| `textures\t<id>.png` | RGBA level 0 of every texture in the pod and pilot texture groups |
| `<CODE>.json` | tables used (addresses), texgroups, placement, part object ids, cable attach points, motion list (slot, id, name, frames, nodes) |

`_player` = the pilot of table 0C2A733C (flag 0), only for ANA, BEN, GAS, SEB (the four selectable pods,
which also have 11 extra motions: contact x4, No1goal A/B, No2goal A/B (EPROM "PATCH" motions E0/E1),
boost, timeout A/B). Without suffix = table 0C2A6C7C (flag 1, every other slot).

Colour variants: DefineDispPod `val` 0..3 picks texgroup slot0[val] (LOD 0) and other[val] (LODs 1-3).
In this ROM all four vals are identical for every kind, so each pod has one variant; the four selectable
pods use a different texgroup for LOD 0 (e.g. ANA 2) than for LODs 1-3 (ANA 3), hence `tg2-3`.

## Verified by looking (previews, all 18)

- All 18 pods and pilots render as recognisable, textured, connected models: e.g. ANA blue/white pod with
  Anakin in helmet, BEN four engines, SEB orange, GAS green with four-armed Gasgano, NEV with the engines
  built into the cockpit (0 engines), ELA asymmetric cockpit with side sphere.
- Six targets (ARK, BOL, EBE, ELA, NEV, RAT): pods, cockpit+pilot close-ups, pilot bind pose and a motion
  frame checked one by one (`preview\<CODE>_pod_lod0/_pod_lod1/_cockpit_pilot/_pilot_bind/_pilot_motion.png`).
  Pilots sit in the cockpit seats; motion frames keep the limb segments joined (joy pose: arms up).
- Not compared against reference art of the film pods: likeness is judged from shape and colour only.

## Formats established here

- **Texture binding**: a model's `TEX recall n` (0C3 with bit 12) indexes the texgroup list of the part
  (pod: texgroup from the pod table; pilot: texgroup from the pilot table). Evidence: for all of ALD's
  models the UV range of every mesh fits the size of texgroup entry n (e.g. n=7 128x32: u<=105.8 v<=31.2;
  n=23 16x16: u<=10.8 v<=9.8). Material recall indexes the model's own attribute block (pod parts) or the
  pilot root model's attribute block (pilots).
- **Texels**: 3D textures have the two texels of each 32-bit word swapped in ROM (see mask-map.md).
- **Object nodes** (104 B): +0 model, +4 parent, +8 child, +12 sibling (-1 = none), +20 bounding sphere
  (x, y, z, r), +36 flag, +40 4x4 matrix, row-vector convention (translation in row 3). Pilot node
  matrices are **object-space** bind transforms (all carry the 0.05 scale; not multiplied down the chain).
  Pod part objects are single identity nodes.
- **Motion** (motion table record -> 20-B ROM header): `K` animated node count, `N` frames, ptr to K node
  indices (u32), ptr to N root keys, ptr to N*K node keys (frame-major). Key = 36 B: scale xyz f32,
  translation xyz f32, rotation xyz as u32 angles (65536 = 360 deg). Rotation (column vectors) =
  Rz(c) Ry(b) Rx(a); node keys are object-space like the bind matrices. Evidence: the rotation of 3 of 8
  ARK nodes in frame 0 of L_crash equals the bind matrix exactly with this order (error 0.000), all other
  orders/signs fail; posed renders keep limbs connected. (`tools\racer_assets.py` calls K "F" and N "N"
  following dump_assets.py.) Root keys move the whole pilot in 86 of 440 pilot motions (e.g. joy: root
  lifts by 0.02-0.05); the export animates the pilot root with them, i.e. root applied after the node keys
  (**guess**: order not traced in the program).
- **Placement** (all 18): NPC table 0C2F904C: body (cockpit) offset and per-engine offset + roll; the
  pilot and binder are drawn in the cockpit frame (pod-placement.md). Forward is -z, y up.

## Handling stats

Only the four selectable pods have player physics blocks (pointer table 0C25C39C, 11 entries; blocks
ANA/default, BEN, GAS, SEB). Values and the few proven field meanings are in pod-placement.md section 3
(e.g. +116 int 947/940/823/829 for ANA/BEN/GAS/SEB, **guess** top-speed cap; +112 2500/2800/3300/3000).
The 14 AI-only kinds have no per-kind handling values beyond the NPC placement/spring table; AI driving
parameters were not traced. **No stats exist for ARK, BOL, EBE, ELA, NEV, RAT as player pods.**

## Wreck / damage pieces

None found. The unreferenced objects next to the pod objects are: first-person cockpit interiors, gauge
frames and needles and control levers for the selectable pods (e.g. objects 66-107, 120-197), Anakin's
engine flap animation frames (25-64, drawn by ANA's callback), a unit cable rod (65), and high-detail
select-screen pilots (19, 20: 48 nodes). Sheets: `export\scratch\objs_0_49.png`, `objs_50_107.png`,
`objs_116_197.png`. Unreferenced motions are select-screen, announcer, Jabba, Jar Jar, Watto, Kaadu
scene and result-screen motions (list via `racer_assets.motion`, ids 0-6, 29, 35, 36, 250-299, E2).

## Stand-ins and open points

- **Cables** (ANA, BEN, GAS, SEB): the game stretches a unit cable object between fixed cockpit and
  engine points every frame (pod-placement.md). Attach points are in `<CODE>.json`; the cables are not
  baked into the exported meshes (pairing of the +- points to engine 0/1 not established). Every pod's
  static binder model (objB) is exported.
- **Engine roll sign**: ALD's engines have roll +12 / -12 deg; applied as RotZ in the row-vector
  convention. The sign is not checked against the program's matrix routine.
- **Handedness**: Hikaru coordinates exported as-is (y up, forward -z). Whether the arcade frame is
  mirrored relative to glTF's right-handed frame is not verified (would mirror asymmetric pods such as ELA).
- **Motion rate**: 60 frames per second assumed (one key per video frame); not traced in the program.
- The player pod's engines are simulated points at run time (rest offsets = NPC offsets for ANA/BEN/GAS/SEB);
  the export uses the rest offsets.
- glTF files checked structurally (buffer ranges, index bounds, texture files present); not run through
  the Khronos validator (not installed).
