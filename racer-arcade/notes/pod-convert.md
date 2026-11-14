# Pod converter: arcade racer -> complete Revenge pod archive (Nov 13 2026)

Tool: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\pod_convert.py`
(one code path for every pod), with `pod_psg.py` (multi-node PSG encoder), `pod_formats.py` (MOT,
COL type 2, COL structure remap, atlas packing) and `pod_verify.py` (offline checks + renders).

    C:\Users\smmel\AppData\Local\Programs\Python\Python312\python.exe -I pod_convert.py ARK 04 BOL 15 EBE 23 ELA 24 NEV 11 RAT 17

Output per pod: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\pods\PODnn\`
(`install\PODS\PODnn\PODnn.RES` + `.CAM`, `members\`, `manifest.json`, `converted-content.fragment.json`,
`verify\compare*.png`, `verify\page_*.png`). Nothing is written to silo\ or the repo.

Siloed test mapping (overwrite of an existing pod number): ARK->04, BOL->15, EBE->23, ELA->24, NEV->11, RAT->17.

## Findings about the Revenge pod files (measured on the six targets)

- **nn_1 / nn_2 / nn_3 are texture pages, not colour variants**: engines use nn_1, chariot hull nn_2,
  pilot nn_3 (POD24 also nn_4 on the hull, POD23 pod_wndw). The arcade has one colour variant per pod
  (pods.md), so there is nothing to map onto variants.
- Page alpha is **not** coverage: the page PSMs are `SpecularMapDualCompressed` with alpha blend and
  alpha test off; nn_1/nn_2 alpha varies 0..255 (specular mask), nn_3 is 255 everywhere.
- Chariot node names differ per pod (POD04 27 nodes with upper_body/armlt, POD11 17 with forearmlt,
  POD23 10 with middle/arm_lt). VUT files are identity renames. Names looked up by the port's common
  path: nullcg, damage00, handlt/handrt (IK = hand, parent, grandparent), l/r_hand_dummy (IK targets),
  chariotcable*/drawcable*, enginecable*, binderleft1/right1.
- MOT tracks address **model binding rows** (sibling-group enumeration, model_bindings49o.h), not PSG
  indices or names; rotations replace the node's local rotation; a track needs >= 2 keys. ByteAxis
  angle k = k * 360/256 degrees (sineWords[64] = sin(pi/4)).
- `*m.COL` / `podboxm` / `chariotm` are **COL type 2** (ray query sets: rays from the part origin to a
  grid on its box; decoder `src\game\type2_resource49x.cpp`); `*f.COL` are type 0 boxes (8 vertices,
  12 faces; podboxf two boxes, 16 vertices). Frames: piece/chariot COLs in the part root frame,
  podbox* in the engine-pair-centre frame (POD04 podboxf z -37.14..13.53 = chariot rear .. engine front
  minus engine z 18.2).
- Pod PSGs: worldFlag 0, origin 0, componentWord 0x107 (no vertex colour) except some original wreck
  pieces with 0x00FF0107 (the port's Python decoder rejects that variant; the converter writes 0x107).
  LOD 0 skins the pilot on its nodes; LODs 1-3 are rigid on nullcg. LOD cutoffs 0.18 / 0.36 / 0.5 / max.
- Part roots carry the pod-frame position (chariot z negative, engines z positive, forward +z).
- Borrowed members that the port's Python decoders refuse in the target too: `textures\AIDamage.psm`
  (texture map count) and `textures\cable.pst` (kind 0) - same result on the originals.

## What each member is made from

| Member | Source | How |
|---|---|---|
| `pods\podnn\chariot.psg` | A + G | nullcg = chariot physics root on the main-body axis (rule below); cockpit, pilot and root-child nodes carry the arcade cockpit offset inside the chariot frame. Arcade see-through triangles (poly type 6) on the original shared window material `pod_wndw` (below). Every triangle nudged 0.01 ft along its normals (finding 6). Hull = arcade cockpit object (LOD i = arcade part i). Pilot = arcade pilot object posed by its `operate` motion frame 0 (the driving pose), LOD 0 skinned, LOD 1 rigid (arcade pilot-table flag 1), none at LOD 2/3 (arcade draws the pilot for LOD 0/1 only). Nodes by one role rule from the arcade skeleton (below). thrlt/thrrt + l/r_hand_dummy placed at the arcade hands (no arcade levers). chariotcable* = cockpit end of the arcade binder (static cable) geometry. damage00 mapped from the target's box. Arcade binder geometry itself left out (Revenge draws cables between nodes). |
| `englt1/engrt1.psg` | A + D + G | Engine body = arcade engine object (engine 0 = left = -x); NEV: cut out of the single body (below). Node list = the target engine's own list (names, order, parents, incl. empty flap nodes) so the borrowed engine MCF/VUT/MOT bind; positions mapped from the target engine box; enginecable* = engine end of the arcade binder. Flame meshes/materials copied from the target (D), flame nodes moved onto the arcade engine's own exits and sized to them, exhaust* (smoke) nodes at the exit centre (G, "Exits and flames" below). |
| wreck pieces `englt1a..` | G | Converted engine (LOD 0, no flames) cut into the target PHY's piece count along the axis the target's pieces are laid out on, borders at the target pieces' normalised borders (equal-count slabs if a border leaves a piece empty: EBE). Child nodes (damage00) mapped from the target piece box. |
| `podbox*`, `chariot*`, `*m/*f.COL` | G | Target file's structure (type, vertex/ray count, faces, tree topology) with every position mapped per axis from the target reference box (piece / chariot / engine pair or whole pod, chosen by which matches the target file's extent) onto the converted one; planes, node boxes, volumes recomputed. |
| `chariot.vut` | G | identity list of the converted nodes |
| chariot MOTs (as listed by the target MCF) | A / G | Forward / Back: the operate pose on every pilot node, held over the target's frame count and duration (arcade has no lean motion). Brake / Mid: rest pose held on thrlt / thrrt / body. Round trip of the ByteAxis keys against the PSG pose: <= 0.74 deg. |
| `textures\nn_1/2/3.pst` (pages the target uses for engines / hull / pilot) | A + G | Atlas of the arcade textures (texel pairs unswapped by pod_export.texture_rgba), repeats baked as tiles (max 4), mirror repeats flipped per tile, 2-texel padding when it fits, 256-colour palette, box mips. Always the target page size: MaxRects packing (shelf fallback), arcade texels scaled 1 / 0.875 / 0.75 / 0.625 / 0.5 until they fit; a larger page only if even half scale does not fit (none of the six). Alpha = the target page's median alpha (specular mask level) on opaque cells; see-through triangles are not on the page (pod_wndw). |
| `textures\pod_wndw.psm`, `pod_wndwGhost.psm`, `pod_wndw.pst` (added only when the arcade pod has see-through triangles: ARK) | D | byte copies of POD23 (the shared original see-through material, finding 4 below); inserted after the hull page PSM, the target order otherwise kept |
| PHY | D (edited) | target PHY with `[Pod Setup] Name` replaced (Ark 'Bumpy' Roose: inner quotes as single quotes) and `[CameraInfo] FirstPersonCamOffset` = converted head node + (target offset - target head node) (G; the offset is chariot-root relative, `src\game\original_attached_camera.h` model1B4) |
| everything else | D | target member unchanged: PSMs (incl. ghost), unused pages (nn_4, pod_wndw), particles, cable shadows, .ai, speech ini, audio ini, engine MCF/VUT/MOT, chariot MCF, `PODnn.CAM` |

### Pilot role rule (every pilot)
Arcade pilot = joints (no model) with one model node per segment; motion keys are object space on the
model nodes. Each model node becomes a chariot node; parent = model owned by the nearest ancestor joint
that owns exactly one model (else `body`). head = central model with the deepest joint; upper_body =
head's parent; arms = lateral models whose common ancestor with the head is deepest; per side by depth:
shoulder, forearm[, hand]. Two-segment arms (ARK, ELA) get `handlt/handrt` at the forearm's end joint.
Other meshes: `pilotNN` (arcade node index). IK chain therefore = hand, forearm, shoulder on every pod.

### Transform
`a2r_space`: x3, z mirrored, winding reversed, normals turned to the winding where they disagreed. Pod
origin: along z at the target's own fraction between chariot root and engine-pair centre (the borrowed
PHY / CAM were authored around that origin); along y at the engine nullcgs (only NEV moves: -3.63 ft) - generated.

### Chariot root (physics) and cockpit (drawn) - one rule for every pod
The chariot is a second physics body on a fixed-length joint from the main-body origin (x 0, y 0, z = right
engine nullcg z) to an anchor 3 ft ahead of the chariot nullcg (port `src\game\authored_geometry49o.cpp:36-44`,
original 2125D8..2126F4); the joint length is |nullcg - main origin| - 3. Every original: nullcg x ~0, y 0..5.15,
joint 13.14 (POD12) .. 38.31 (POD21) ft. Converter: nullcg = (0, 0, min(cockpit z, engine z - 3 - 13.14));
the arcade cockpit/pilot sit inside the chariot frame at their arcade z (NEV: +16.7 ft ahead of the nullcg);
the cockpit is drawn at engine height - STAND-IN, switch `COCKPIT_AT_ENGINE_HEIGHT` (False = arcade layout, 4.5..9 ft
above the engines); the arcade's raised cockpits cut through the borrowed chase cameras (runs fix13-look2-15 / -23). `FirstPersonCamOffset` = converted head + (target offset - target head).
## Size comparison (LOD 0 bounding boxes, ft, x * y * z)

| Pod | Target whole | Converted whole | Target chariot | Converted chariot | Target engine | Converted engine |
|---|---|---|---|---|---|---|
| ARK->04 | 21.5 x 8.5 x 50.2 | 25.9 x 12.5 x 31.9 | 6.0 x 3.1 x 11.5 | 3.0 x 5.4 x 5.5 | 7.4 x 8.5 x 26.6 | 10.3 x 8.0 x 13.4 |
| BOL->15 | 23.9 x 6.9 x 51.4 | 26.6 x 16.6 x 54.1 | 3.4 x 6.9 x 13.8 | 6.8 x 5.1 x 20.5 | 7.0 x 6.9 x 27.2 | 8.6 x 8.6 x 22.0 |
| EBE->23 | 34.9 x 8.1 x 33.0 | 21.3 x 11.9 x 46.7 | 4.5 x 2.6 x 6.4 | 8.6 x 5.6 x 9.2 | 13.6 x 8.1 x 18.6 | 8.0 x 8.2 x 28.6 |
| ELA->24 | 18.8 x 8.2 x 40.5 | 26.0 x 10.7 x 33.3 | 7.7 x 6.3 x 8.4 | 5.0 x 6.1 x 6.0 | 7.7 x 8.2 x 20.2 | 9.5 x 6.8 x 12.6 |
| NEV->11 | 23.0 x 10.1 x 42.3 | 15.9 x 6.0 x 21.5 | 9.4 x 3.3 x 8.5 | 4.9 x 3.6 x 11.3 | 9.2 x 10.1 x 21.3 | 7.4 x 6.0 x 20.4 |
| RAT->17 | 17.3 x 8.2 x 40.6 | 21.1 x 13.6 x 41.5 | 11.8 x 5.5 x 8.3 | 4.9 x 2.9 x 6.1 | 6.2 x 8.2 x 18.7 | 9.1 x 10.2 x 28.9 |

Whole-pod sizes are in the Revenge range (17-35 ft wide incl. gaps, 21-54 ft long). NEV is the most
compact (its engines are built into the body); its parts sit 0.6-6.7 ft above the pod origin
(arcade body offset y 1.2 -> 3.6 ft), the others straddle y = 0.

## Fixes of Nov 13 (fix13) and their in-game checks

Silo builds `silo-cv-12` (diagnostic log lines only), `-13` (+ [Binder] BinderMeshHalfHeight read from the
PHY), `-15` (+ UI archive rows, IPU skip by PST kind, converted pods' in-race name from their PHY).

1. **NEV (POD11) player shutdown**: cause = the converted `podboxf.col` (the main contact hull). The target hull
   is one L-shape over the engine pair and the chariot behind it; one affine map over the whole pod put the
   chariot half onto the rear half of NEV's engines (NEV's cockpit sits between them), with the chariot's
   width: the right engine's rear stuck out of the hull. At the start the right engine touches the wall
   (frame 145, same for the original POD11) and the contact response launched the pod 580 -> 985 -> 1972
   units in two frames; the engine-sound guard then ends the race. Proof: same build/data with only the
   original podboxf -> no launch (`fix13-exp11-podboxf`); new rule -> no launch (`fix13-sc-11f`).
   Rule now: every hull vertex maps with its own part (engine pair or chariot), chariot part from the
   chariot's rear to the engine rear, z order kept (`pod_formats.remap_podbox_parts`).
   Also fixed on the way (both real, found with the per-frame trace): NEV's chariot was ahead of its engines
   (joint 0.65 ft, rest geometry inconsistent) and a 1 ft joint blew up the solver (joint force 3e3 -> 3e6 in
   20 frames, `fix13-sc-11c`): joint >= 13.14 ft now (rule above).
   NEV is one rigid unit (FG 8T8: no binder band, no cables): data rule "arcade engine count 0" writes into
   its PHY ThrustForce/DragForce/LiftForce/Separation/Impact/GroundImpact/Yaw/Pitch/Roll/Powerslide torques/
   BinderSpring/BinderTorque/turbulence/OrientationScale = 0 (engine visual bodies stay on their rest point),
   CableForce / ChariotMaxYawAngle / ChariotMaxPowerslideYawAngle / chariot turbulence / ChariotOrientationScale
   = 0, BinderVolume = 0 (no band sound) and `[Binder] BinderMeshHalfHeight = 0` (original key read at 215BC8,
   default 2.25; the port refused any authored value and any 0 height: now admitted 0..64,
   `tests\native\gameplay_binder_candidate_fixture.h`, `src\game\original_gameplay_binder_candidate.cpp:32`).
   enginecable nodes on the engine surface nearest the chariot cable node (1.3 ft cables).
   **Port defect (not changed)**: `src\game\gameplay_audio50he.cpp:71-72` refuses a speed ratio outside
   [-2, 2] and `tests\native\terrain_interactive_frame_physics50du.cpp:351` (require at the engine-audio call)
   turns that into a session terminate; the original computes a pitch for any value. A physics spike
   should not end the race.
2. **ELA (POD24) AI wrecks**: see the AI-field run below. Converted ELA's width (26.0 ft) is inside the
   originals' range (POD08 34.7, POD22 26.8); 2 of the 3 earlier wrecks were at the known SC trouble spots that
   also wreck/catch originals (x 5577..5770 z 9298..10057; original POD24 wrecked at x 6391 z 7234). The
   podboxf rule (1.) also changed ELA's hull (it was mapped over the whole pod before).
3. **Flames**: the arcade has no exhaust/flame points (pod "light groups" are course lighting, race-data.md
   20). Exits read off the engine model (`engine_exits`): flat rear-facing round discs that cover their own
   centre and have nothing of the engine behind them; equal exits at one depth beyond the free flame groups
   become one ring exit. Target flame meshes/materials kept (D), nodes moved onto the exits, sized by exit /
   flame radius, front at the exit face; exhaust* nodes at the exit centre (G). ARK: the two big side tubes;
   BOL: the centre disc between the prongs; EBE/ELA/NEV: the rear nozzle; RAT: ring of vane ends round the
   nozzle. Offline: `tools\a2r\scratch\fix13\flames_offline.png`.
4. **See-through parts**: material names are global in the original (APPSTART.RES `AppStart\Mat_Stbl.txt`,
   3235 names); the port's material table refuses any other name (a new `04_2wndw` killed run
   `fix13-look-4`). `textures\pod_wndw` is the original see-through pod material and ONE shared member set:
   POD01/10/19/21/23/25 ship `pod_wndw.psm/.pst` byte-identical, and differing bytes under that name across
   mounted pods are refused (a generated arcade-glass `pod_wndw.pst` killed `fix13-look4-4` with POD10/19 in
   the field). So ARK's 114 poly-type-6 triangles (canopy glass + two cut-out sails, notes\pod-render.md 4)
   use `pod_wndw` with POD23's three members copied byte-identical (D). OPEN: the arcade glass (6.7 %
   opaque) and the sail cut-outs need a pod-own see-through material name = an APPSTART `Mat_Stbl.txt`
   conversion + port admission; the generator for it is in pod_convert.py (`WINDOW_OWN_TEXTURE`, off).
5. **Texture pages**: always the target page size now; MaxRects packing (shelf fallback) and arcade texel
   scale 1 / 0.875 / 0.75 / 0.625 / 0.5. Results: every page at its target size; pilot pages at 0.625 (ARK)
   or 0.75 (BOL, EBE, NEV, RAT); RAT's engine page fits 256x256 at full scale (was 256x512). Saves 16 KB per
   pilot page and 128 KB on RAT's engine page. Before/after: `tools\a2r\scratch\fix13\pages_before_after.png`.
6. **Z-fighting (ARK's ring insides)**: the arcade draws pod triangles one-sided; most arcade pods model shells
   twice back to back (ARK's ring tunnel lining) and nocull faces arrive as front + reversed back copy. The
   port draws pods two-sided (whether the PS2 original culls pods was not found in the port's draw path).
   STAND-IN for one-sided drawing: every triangle moves 0.01 ft along its own visible-side vertex normals
   (`FACE_NUDGE`); both copies kept. Look-check `fix13-look5-4`: ring insides clean.
## Open items / flags

- In-game results (silo-cv-13/15, SC with the coordinator's fog + two-sided-face track, muted logs-only,
  `--repair --unstick --results-confirm 30`): NEV player `fix13-sc3-11` 8th 343.3 s, 0 resets, no speed spike;
  ARK `fix13-sc2-4` 8th 293.6 s, 0 resets; BOL `fix13-sc-15` 8th 353.9 s, 0; RAT `fix13-sc2-17` 7th 346.0 s, 0;
  EBE `fix13-sc2-23` 7th 294.1 s, 1 reset; ELA `fix13-sc3-24` NOT finished in the 520 s run (a lap down at
  307 s although its average speed is the highest: a lap not counted?). FPS 62 average, minimum 42-47 (load
  screens). Malastare (assets-lc) `fix13-lc-3`: 8th 269.9 s, 0 resets, FPS 62.2 / 47.0.
- ELA (POD24) as AI still gets destroyed. Per-slot damage trace (build silo-cv-18, `racer_damage_trace`, logs only):
  in the SC field every AI picks slot 7 as its combat target (original behaviour, the same with the original
  POD24 in slot 7, run `fix14-ai-3-orig24`). The original POD24 took 8 damage events (total 0.16) in a whole race;
  the converted ELA took 51 (1.87) and was destroyed at 279 s (`fix14-ai-3`): side hits on the start straight in
  laps 1-2 (x 12840..12880, z 10900..13600, speed 600-700), then from ~150 s a steady rise of both engines'
  damage every 0.2-0.5 s along the climb (z 5700..10600) at a capped speed of ~440, which matches the port's
  periodic damage when `vehicle64 == 2` (impact_consumers49z.cpp:134; overheated/limp state, not yet logged).
  Why ELA is hit 6x more often while being targeted like the original is not found (candidates: its 26 ft
  width vs 18.8 against the AI's side-hover distance, which the .ai file scales by "EnginesWidth"). Not fixed.
  As player (`fix13-sc3-24`) the host lap counter reached 3 (`host_laps 3`) but the race manager kept it at 2
  laps, so no finish was sealed; not found either.
- Second camera view (cockpit / close view) is broken for converted pods: `PODnn.CAM` and the PHY
  [CameraInfo] presets are the replaced pod's, so the camera sits inside the new cockpit geometry (ARK: a big
  grey pyramid fills the view). Left as is until the converted pod becomes an independent pod (user).
- The borrowed chase cameras (`PODnn.CAM`, D) sit inside BOL's large cockpit (pilot's back fills the view,
  run `fix13-look3-15`); a generated CAM per pod size is not done.
- ARK's see-through cockpit uses the Revenge window texture, not the arcade glass (material names global).
- NEV keeps two short cable stubs (the port draws cables between the chariot and engine cable nodes; no
  original data path removes them).
- Brake / Mid motions hold the rest pose: the converted pods show no brake flaps or lever movement.
- Mirroring (z) mirrors any lettering on arcade textures (same as the tracks).
- Wreck tuning, stats, engine positions (`LeftEnginePosition`) and camera presets are the target's (D);
  stats are to be replaced by another helper.
