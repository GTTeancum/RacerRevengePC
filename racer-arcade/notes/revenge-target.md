# Revenge target: what a course and a pod are, and what new content must supply

Research note, 2026-11-12. Read-only on the repo and on the original data. Sources:
`D:\Programming\GitHub\RacerRevengePC\CLAUDE.md`, `D:\Programming\GitHub\RacerRevengePC\docs\HANDOFF-20261106.md`,
`D:\Programming\GitHub\RacerRevengePC\docs\asset-decoder48b.md`, `D:\Programming\GitHub\RacerRevengePC\docs\authored-geometry49o.md`,
`D:\Programming\GitHub\RacerRevengePC\work\scratch\branch-inventory\PLAN.md`,
`D:\Programming\GitHub\RacerRevengePC\work\scratch\pod-load-all\PLAN.md`, src\assets, src\game, and the
original PS2 files. Facts below were read from the files unless marked **(guess)**.

## 0. Where the original data lives

* The port reads the unmodified disc folder: env `RR_ORIGINAL_ASSET_ROOT` = the folder that holds
  `SLUS_202.68` (`D:\Programming\GitHub\RacerRevengePC\platform\windows\native\Launch-Interactive50DU.ps1:69`).
  Installed copy used here: `C:\Games\Star Wars Racer Revenge\` (`TRACKS\`, `PODS\PODnn\`, `UI\`, `AUDIO\`, `LOC.RES`, `APPSTART.RES`).
* RES container (decoder `D:\Programming\GitHub\RacerRevengePC\src\assets\import_decode48c.cpp:147`):
  u32 3, u32 meta, u32 0x340, u32 0, u32 tableBytes; u16 compressed-size table (zero-terminated);
  u32 count, u32 indexBytes, then per member {u32 nameLen, name, u32 logicalOffset, u32 bytes};
  then one zlib stream per 24576-byte block, each followed by 0xFF. Members are addressed by their
  backslash path (case-folded lookup).
* The original builds every name from data: tracks `%s%s.res` with the two-letter course code; pods
  `pod%02d`, `pods\pod%02d\pod%02d.phy`, `%s%s\%s.ai`, `%s%s\%s.cam`, `%s%s%02d.vub`, `%sTVCam.pos`,
  `.scb/.scn`, `.tob/.toy`, `.aub`, `.spl`, `.tun`, `.wtt`, `.esb`, `.acb` (strings in SLUS_202.68 at
  0x2a7a88..0x2c2e70).

## 1. Revenge roster

### Courses (13)
Planet and track names from `LOC.RES` `loc\english\UI.loc` + `L_UI_ENG.bin` (keys `STR_PLANET_*`,
`STR_TRACK_<PLANET>_<A|B|C>`). Letter 1 of the code = planet (T Tatooine, R Ryloth, M Mon Calamari,
G Gamorr, S Sullust), letter 2 = A/B/C slot. Track ID = index in
`D:\Programming\GitHub\RacerRevengePC\src\game\original_course_descriptor50kg.h` (profile 11278, stride 12).
Order/class/record pod from `D:\Programming\GitHub\RacerRevengePC\src\game\ai_cold_owners50iu.h:35-47`.

| ID | Code | Planet | Track (game string) | Tournament order | AI class | Record/rival pod | RES bytes |
|---|---|---|---|---|---|---|---|
| 0 | TA | Tatooine | THE MOS ESPA OPEN | 0 | A | 19 | 3,334,562 |
| 1 | TB | Tatooine | THE BADLANDS | 5 | B | 7 | 3,749,557 |
| 2 | TC | Tatooine | BOONTA EVE CLASSIC | 12 | C | 18 | 3,607,868 |
| 3 | RC | Ryloth | THE NIGHTLANDS | 11 | C | 24 | 4,655,141 |
| 4 | MA | Mon Calamari | OROTORU G'AM | 2 | A | 20 | 3,691,592 |
| 5 | MB | Mon Calamari | THE BALLAST COMPLEX | 6 | B | 22 | 4,421,779 |
| 6 | MC | Mon Calamari | THE GRAND REEFS | 9 | C | 8 | 3,822,171 |
| 7 | RA | Ryloth | THE BRIGHTLANDS | 4 | A | 9 | 4,604,561 |
| 8 | GA | Gamorr | WATCHTOWER RUN | 3 | A | 10 | 2,834,942 |
| 9 | GB | Gamorr | RUINS OF CARNUSS GORGUUL | 7 | B | 23 | 4,088,586 |
| 10 | GC | Gamorr | THE CITADEL | 10 | C | 17 | 4,205,067 |
| 11 | SA | Sullust | SORO SUUB FACILITY | 1 | A | 12 | 3,786,696 |
| 12 | SC | Sullust | SERRES SARRANO | 8 | B | 16 | 2,974,353 |

Cut content still in the data: `STR_TRACK_SULLUST_B` = THE MINING SECTOR (no SB.RES), no Ryloth B
track string but RYB announcer VAGs exist, and planet `STR_PLANET_CORUSCANT` with announcer VAGs
`AUDIO\SPEECH\DAX\TRACKS\CORUSCNT\DAXCR{A,B,C}*.VAG` (SLUS 0x28bfd0) but no track. Not useful as
content, but it shows the original menu/announcer were authored for more than 13 tracks.

### Pods / pilots (23 on disc; 05, 06, 13 absent)
Names: PHY `[Pod Setup] Name` (in `PODS\PODnn\PODnn.RES`, member `Pods\Podnn\Podnn.phy`) and the
menu string `STR_RIDER_SELECTION_PAGE_*`. Wreck pieces = sum of `NumberOfParts` over the PHY
`[Wreckage] PartN` sections.

| Pod | PHY Name | Menu name | Wreck parts / pieces |
|---|---|---|---|
| 01 | Anakin Skywalker | YOUNG ANAKIN | 3 / 7 |
| 02 | Ben Quad | BEN QUADINAROS | 5 / **13** (two engine pairs, englt2/engrt2) |
| 03 | Clegg | CLEGG HOLDFAST | 3 / 7 |
| 04 | Dudbolt | DUD BOLT | 3 / 7 |
| 07 | Gasgano | GASGANO | 3 / 7 |
| 08 | Mars Guo | MARS GUO | 3 / 9 |
| 09 | Mawhonic | MAWHONIC | 3 / 7 |
| 10 | Alder Bedo | ALDAR BEEDO | 3 / 7 |
| 11 | Ody Mandell | ODY MANDRELL | 3 / 7 |
| 12 | Shrivel Braittrand | SHRIVEL BRAITTRAND | 3 / 9 |
| 14 | Sebulba | SEBULBA (EPISODE I) | 3 / 9 |
| 15 | Teemto | TEEMTO PAGALIES | 3 / 7 |
| 16 | Was Sandage | WAN SANDAGE | 3 / 9 |
| 17 | Scorch Zanales | SCORCH ZANALES | 3 / 9 |
| 18 | Sebulba 2 | SEBULBA | 3 / 9 |
| 19 | Anakin2 | ANAKIN SKYWALKER | 3 / 7 |
| 20 | Occo Ninebar | OCCO NINEBAR | 3 / 7 |
| 21 | Darth Maul | DARTH MAUL | 3 / 7 |
| 22 | Knire Dark | KNIRE DARK | 3 / 9 |
| 23 | Kraid Nemmeso | KRAID NEMMESO | 3 / 7 |
| 24 | Tzidik Wrantojo | TZIDIK WRANTOJO | 3 / 7 |
| 25 | Watto | WATTO | 3 / 7 |
| 26 | Darth Vader | DARTH VADER | 3 / 7 |

Note for the governing rule: CLAUDE.md and the pod-load plan say "7 or 9 pieces"; POD02 has 13
(chariot 1 + four engines x 3). Any code or budget sized on "max 9 per pod / 72 per field" is wrong
for a field containing POD02 (worst field is about 13 + 7x9 = 76).

## 2. What makes up a course

All members of `TRACKS\XX.RES` (counts from TA/GA/GC/MC/RA listings). xx = lower-case course code.

| Data | Member(s) | Format (as the port reads it) | Original reader | Port reader (examples) |
|---|---|---|---|---|
| Scene / world | `tracks\xx.scb` | "BINARY\0" + u32 1 + string table + labelled parameter blocks (compiled `.scn`). Blocks: sceneinfo, environment, fog, ghostopacity, lights/lightN, portalN, zoneN, zoneN-modelM (placed models), terrainzoneN, cscameras, sounds. GC also ships text `gc.scn`. | race world 1B2870 via 1A6BC0 (26FCF8 World4D8); "Begin Load for Scene" | `D:\Programming\GitHub\RacerRevengePC\src\assets\import_world48f.cpp` `decodeWorld48F`, `world_asset48f.*`, `world_index49b.*` |
| Geometry | `geometry\*.psg` (150-900) | PSG v3: 128-byte node names, 64-byte matrix, bounds, parent, materials, LODs, packed VIF strips (s16 pos, s8 normal, s16 UV, u8 colour) | 26E820 / 240740 model build, FileManager 132820 | `import_scene48d.cpp`, `model_geometry49o.cpp`, `original_source_psg_file50la.h`, `original_standard_model_resource50la.cpp` |
| Collision | `geometry\*.col` (36-320) | COL mesh per zone model (type 0/1) | 165E90-family / physics resource | `import_collision48g.cpp`, `collision_mesh48i.cpp`, `original_source_collision_file50la.h`, `track_collider50ao.cpp`, `track_collision_tags50an.cpp` |
| Zones / portals / visibility | inside SCB (zoneN, portalN, terrainzoneN) + SCB zone search kind (List/QuadTree) | | 275F74..276530 | `zone_search_authoring50cn.cpp`, `src\game\live_zone_membership50en.h` |
| Textures | `textures\*.pst` | PST rev 1: type, mip count @+8, 256-entry RGBA palette @+12, mips sized w*h*bpp/8 | 24E880 / 24EDF8 / 11A630 | `import_texture48e.cpp`, `original_standard_texture_resource50la.cpp` |
| Materials | `textures\*.psm` | ASCII INI, shader passes (PreLitCompressed etc.), texture names, AA flags | material table | `original_standard_material_source50la.cpp`, `..._object50la.cpp`, `..._table50la.cpp` |
| Animated textures | `textures\*.atm` | frame strips | 29A020 | `original_standard_material_animation50la.h`, `original_ui_atm50kg.cpp` |
| Objects, triggers, anims, cut cameras, emitters, sun flares | `tracks\xx.tob` (+ text `xx.toy` on GC) | BINARY parameter blocks: animationN(+loopsound), cscameraN, modelN, particleemitterN, podracer, staticmodelparams, sunflareN, triggerN + triggerN-actionM | 174DF0, 1B28EC; "Begin Load for Scene Toys" | `src\game\original_camera_start_authoring.cpp`, per-course `*_trigger_authoring*.h` (stand-ins) |
| Animated track objects | `anim\*.psg/.mcf/.mot/.vut/.col` | same as pod models | | `model_bindings49o.cpp`, `chariot_root_authoring49u.cpp` |
| Material audio | `tracks\xx.aub` | BINARY blocks: collision-object material sounds, drive-on sounds | "Begin Load for Scene Audio" | `D:\Programming\GitHub\RacerRevengePC\src\assets\material_audio50hi.cpp` |
| AI path | `tracks\axx.spl` | ASCII INI: `[Track] NumNodes`, `[NodeN] Position, Left/RightTrackWidth, speed...` | "Could not open ai spline file" 0x2aa800 | `src\game\track_navigation50eu.h`, `readCourseSpeedLimits50GK`, `ai_runtime50iv.h` |
| Map / lap spline | `tracks\mxx.spl` | same INI layout | 0x2aa7a0 | `track_navigation50eu.h`, `track_start50ad.cpp` (StartNode) |
| Course tuning | `tracks\xx.tun` | ASCII INI: `[EnvDamage]` hit scales, AIDriving Lookahead, etc. | | `host_course_driver50gk.cpp`, `dynamic_impact50iw.h`, `static_impact49y.cpp` |
| Intro cut cameras | `tracks\xxNN.vub` (one per TOB cscamera, 3-7) | binary camera keyframes ("tracks\xxNN.vue") | 1A95B0 camera controller | `D:\Programming\GitHub\RacerRevengePC\src\assets\original_track_cut_identity50la.h` (identity), cut authoring |
| TV cameras | `tracks\xxTVCam.pos` | ASCII INI `[CameraPositions] NumberOfCameras, SwitchEvery`, connectivity | SimpleTVCamera::LoadPositions | `D:\Programming\GitHub\RacerRevengePC\src\assets\tv_camera49e.cpp` |
| Sky | `skies\*.cub` (name from SCB environment `CubeFile` / `CubeFileForZoneN`; SC has none), GB also `skies\gb.acb` | paletted cube images | 131230, 276D60/276D7C, codec 14CBE0 | `original_source_sky_file50la.h`, `original_sky_palette_codec14cbe0_native50la.h` |
| Fog / lights | SCB fog + lightN blocks | Visibility, Haziness, RGB; light pos/target | | `src\game\original_gc_gameplay_lighting_candidate.cpp` (13 per-course prepares = stand-in) |
| Water (only if the SCB names one) | `tracks\xx.wtt` (GC, MC) via SCB `WaterFile` | ASCII INI: WaterBaseHeight, swell, channels, nodes | Water::Create 2B92F0; race water 22C21C/210804 | `water_authoring50jc.h`, `original_water_source_config50la.h`, `original_source_water_exists131a40.h` |
| Billboards | `tracks\xx.esb` (GA, TC, GB, GC) | binary sprite records (name, tga, floats) | ".esb" 0x2a3680 | not found as a dedicated reader |
| Particles | `particle\demoprt.prt` (+ .pst/.psm atlas), `particle\*.emt` | PRT: INI, TextureMap, NumFrames, NumGroups; EMT: INI per emitter | 1617A0, 160F60 | `original_particle_source_config50la.h`, `effect_authoring50bg.cpp`, `effect_cold50bj.cpp` |
| Intro audio | `audio\introAud.ini`, `audio\scrptLns.ini` | ASCII INI | | `original_ta_intro_authoring_candidate.h` |
| Announcer track lines | disc `AUDIO\SPEECH\DAX\TRACKS\<PLANET>\DAX<PL><A/B/C>nn.VAG` | VAG; paths are a table in SLUS (0x28bfd0..) | | `original_race_intro_pcm_identity50la.h` |

Race-load order (original): race 1A65C0 → 1A6AA0 load begin → 1A6D80 track → 1A6B5C groups →
1A6BC0 world (1B2870) → 1A6DC0/1A95B0 camera controller → 1A72C4 1AA510 racer manager → 1AA5A0
resources → 1A8A40 HUD/Pause/Results.

## 3. What makes up a pod

`PODS\PODnn\PODnn.RES` (89-112 members) + `PODS\PODnn\PODnn.CAM` loose file. Member names use the
PHY `Prefix` ("01") and `Pods\Podnn\`.

| Data | Member(s) | Notes |
|---|---|---|
| PHY | `Pods\Podnn\Podnn.phy` | ASCII INI. Sections: Pod Setup (Name, Prefix, Chariot/LeftEngine/RightEngine `.slt`, EngineMotion), Wreckage (NumberOfParts, PartN, Gravity), one section per wreck collection (NumberOfParts, MaxNumberOfDamageEmitters, spring/damping/restitution/friction, PartK, InertiaK, WeightK, BreakRateK, BreakScaleK, EmitSmokeK), Physical Properties, Repulsor, Roll Control, Engine (MaxThrustA/B/C, TopSpeed A/B/C, boost/repair), Engine Aerodynamics, Pod Animation, Collisions (incl. ExplosiveWreckSpeed, ResetSpeed), Joystick, CameraInfo, DamageRepair, Pod Audio (engine/beam VAG), BaseAbilities, MaxAbilities, Tuning. Readers: `native_phy_load50la.h`, `original_phy_cursor50la.h`, `pod_physics49n.cpp`, `src\game\authored_parameters49n.cpp` (2146C0..216034). |
| Chariot + pilot | `pods\podnn\chariot.psg` + `.vut` + `.mcf` + `nn_brklt/brkrt/forwd/back.mot` | The **pilot is part of the chariot PSG** (nodes Ani_torso, Ani_head, Ani_L/R_uparm, Ani_L/R_forearm, handlt/rt, l/r_hand_dummy, nullcg, cables, brakes, damage00). MCF lists the 4 motions; VUT renames nodes. IK for arms: `docs\character-mode0-ik50kg.md`. Readers: `model_bindings49o.cpp`, `chariot_root_authoring49u.cpp`, `engine_animation49q`. |
| Engines | `englt1/engrt1.psg/.mcf/.vut/.mot` (POD02 also englt2/engrt2) | EngineMotion selects the engine motion set (2136CC..213A24). |
| Wreck pieces | `englt1a/b/c.psg`, `engrt1a/b/c.psg` (+ more for 9/13-piece pods) and `*m.COL` / `*f.COL` per piece | Built by 2129C0 → 212A2C 22BF00 / 212A90 22BFD0 → per piece 22C13C → 2CB4B0 → 165C30 → 240740 → 26E820 model, **165E90** body/zones, 166018 E0, 295A40 DC/D8, 1661D8 E4, 1662E4 E8, 166390 filters, 22C21C water, 22C278 group, 212AAC append to racer370/374. Port: `src\game\original_wreck_assets50la.h`, `original_wreck_tuning50la.h`, `original_wreck_racer_storage50la.h`, `tests\native\original_pod_load50pl.*` (new). |
| Pod collision | `podboxm.col`, `podboxf.col`, `chariotm.col`, `chariotf.col` | `%spodboxm.col` etc. (SLUS 0x2b1828) |
| Textures / materials | `textures\nn_1/2/3.pst/.psm`, ghost PSMs, `flamennl/r.psm`, `concringnn.psm`, shared cable/binder/pod_wndw/specmap/Flame | |
| Particles | `particle\pod*.emt` (engine smoke/dust, sparks, fire, spray, wreck dust/splash/sparks/smoke, explosions, embers) | identical across pods |
| Cable shadows | `geometry\CableShadowL/Rnn.psg` | |
| AI personality | `Pods\podnn\podnn.ai` | ASCII INI [combat] etc. Reader `src\assets\ai_authoring50iu.h`. |
| Camera presets | loose `PODnn.CAM` | ASCII INI presets (AngleX/Y, distance, FOV) |
| Speech | `audio\speech\podnn\sounds/curses/taunts/special.ini` + disc VAGs `AUDIO\SPEECH\PODnn\*`; announcer `AUDIO\SPEECH\DAX\INRACE\PODnnANN.INI` (path table in SLUS 0x28cf00, 0x295d50) | read by 2C1170 |
| Audio events | `audio\p_event.ini`, `p_debris.ini`, `wrecks.txt` | |
| Menus (outside the pod RES) | UI\UIRIDER, UI\UIGARAGE: `ui\geometry\%02d.psg`, `ui\anim\%02dc.psg`, `ui\textures\%02d_%d`, `%02dc_%d`, `%02d_shdw`, `%02d_bkg`, `%02d_flrg`, garage engine `%02de.psg/_%d`; `UI\UIPODTEX.RES`; `artpod/artpil%02d` | needed only to pick the pod |
| Tournament win scene | `TRACKS\ENDnn.RES` (one per pod, 05/06/13 absent) | cut scene, font, sky |

Load path (original): human 1ADF80 → 1A0570/1A0848 → 210770 → 2129C0; AI 1AE3CC → 16E450 →
16E5F4 210770 → 2129C0; then 212ABC exclusions, 212B50 explosion/spark/ember emitters, handling
2146..215BB8 (215AF8 wreck tuning per collection), 212DF0 cables, 212ED0 spin. Pod number is
`racer60`, formatted `Pod%02d` (21079C/21081C/210820); id 0 or >= 100 rejected (210824).

## 4. What a new course or pod must supply, and the fixed-size places

### New course (data the common path needs)
1. `TRACKS\XX.RES` in the RES3 format above with, at minimum: `tracks\xx.scb` (environment with
   CubeFile/fog/lights, zones, portals, terrain zones, placed zone models, cscameras), all
   `geometry\*.psg` + `*.col`, `textures\*.pst/.psm(/.atm)`, `tracks\xx.tob` (cut cameras, triggers,
   animations, emitters, podracer start, static model params), `tracks\xxNN.vub` per cut camera,
   `tracks\axx.spl` + `tracks\mxx.spl`, `tracks\xx.tun`, `tracks\xx.aub`, `tracks\xxTVCam.pos`,
   `skies\*.cub`, `particle\demoprt.prt` + atlas, `audio\introAud.ini` + `audio\scrptLns.ini`;
   optional `tracks\xx.wtt` (water), `tracks\xx.esb` (billboards), `anim\*`.
2. Menu assets: UICOURSE `ui\anim\holo_xx.psg/.mcf`, `ui\textures\holo_xx.pst/.psm`; strings
   `STR_PLANET_<NEW>` and `STR_TRACK_<PLANET>_<X>` in all five languages of LOC.RES.
3. Announcer VAGs under `AUDIO\SPEECH\DAX\TRACKS\<PLANET>\` (or none) and race music choice.
4. **Choosing the code:** the PHY class selector 214954 (`src\game\authored_parameters49n.cpp:106`)
   picks class A if the code contains lower-case 'a', else B if 'b', else C, with exceptions sb→C,
   ra/sc→B. A new code silently picks a class by its letters. Codes in use: ta tb tc ra rc ma mb mc
   ga gb gc sa sc (also "end"). Malastare cannot use "MA" and Arcade's "SA_" prefix must not map to
   Revenge "SA" (Sullust A).

### Hard-coded course identity (must be extended)
| Place | Size | Notes |
|---|---|---|
| Course descriptor profile 11278 (`src\game\original_course_descriptor50kg.h`) | 13 x 12 bytes | code + rival pod; part of the save profile |
| Course order table 100C4 (stride 40), `originalTournamentCourseOrder50KG`, three more copies of `{0,11,4,8,7,1,5,9,12,6,10,3,2}` (`original_ui_course_widget_controller50kg.cpp:20,23`, `original_ui_hall_controller50kg.cpp:15`) | 13 | AI class is order < 5 → A, < 9 → B, else C (16E590) |
| Track id in game byte 1125F | **low nibble** (`&15`, 187B10, 1AC47C, 1E2038) | at most 16 courses; 13 used, so only 3 free IDs. 4 Arcade tracks do not fit without widening this field. |
| Prizes, medals 1938, remaining prizes 1946, unlock lap/race/knockout tables (`original_tournament_result50kg.h`, `original_unlock_checks50pd.h`), Hall records/widgets (`original_ui_hall_widgets50kg.h`, `original_ui_hall_records50kg.h`), course soultree ordinals (`original_ui_entity_scope50kg.h:3472`), course page packets (24) | 13 each | save format and menu layout |
| Tournament circuits: 3 circuits of 5/4/4 by order (Galactic Trials / Podracing Open / Hutt Championships) | | new courses need a circuit or a new mode |
| Port integrity tables: `original_track_cut_identity50la.h` (13 rows, size + SHA-256 + cut counts), `original_ta_intro_authoring_candidate.h:201-249`, `original_camera_start_authoring.cpp:72-101`, `audio_assets50hc.cpp:268`, `material_audio_provider50hi_fixture.h:61-85`, 13-course allowlists (`original_ta_intro_preload50la.h:21` etc.) | 13 | every original file is authenticated by size + hash; converted content needs its own identity rule |
| Port stand-ins keyed on course (branch inventory B-C01..B-C21): 13 `isXxTrack` flags, 13 consumed stages, 13 race-world members, 12 camera factories, 13 lighting prepares, per-course collision/navigation/trigger/material/particle/audio owners, RA-only Results/Pause/F8 | | **A new course will not load until these are retired into the common path.** This is the real blocker, not the data. |

### New pod (data the common path needs)
1. `PODS\PODnn\PODnn.RES` with PHY, chariot+pilot PSG/VUT/MCF/4 MOT, engine PSG/VUT/MCF/MOT, every
   wreck piece PSG + m/f COL, pod/chariot COL, textures/materials, pod EMTs, `.ai`, speech INIs;
   loose `PODnn.CAM`. Optional bottom engines (POD02 layout).
2. Speech/announcer: `AUDIO\SPEECH\PODnn\*` VAGs + INIs, `AUDIO\SPEECH\DAX\INRACE\PODnnANN.INI`
   (the path is a fixed table in the original exe, so the port needs a matching table entry).
3. Menus: UIRIDER/UIGARAGE models and textures (`%02d*`), UIPODTEX images, `STR_RIDER_SELECTION_PAGE_*`
   string, art gallery optional, `TRACKS\ENDnn.RES` win scene (or a rule to reuse one).

### Hard-coded pod identity (must be extended)
| Place | Size | Notes |
|---|---|---|
| Career/pod arrays: careers 27, career names 27, unlock10FAC 27, base/max abilities 27, diagonal medals 27x13, announcer recent 27x8 (`original_save_profile_stage50kg.h`, `original_tournament_result50kg.h`, `original_save_records_projection50kg.h`, `shared_announcer50ie.h`, `platform\linux\native\original_save_profile_owner50kg.h`) | 27 (index 0..26) | **free pod numbers inside this range are only 05, 06, 13.** UI loops careers 1..26 (1B7698) and skips 5/6/13 (inventory C1). More than 3 new pods means widening the save format. |
| Pod id check 210824 | < 100 | fine for `Pod%02d` |
| Racer field | 8 per race | unchanged |
| `nativeCourseRacers50KG` (`original_ui_course_race_domain50kg.h:12`) | 23 | port list of pods on the course screen |
| `original_racer_selection50ij.h` (8 + 1 + 14 entries), `original_player_identity50ik_fixture.h` (23 rows, RES size + hashes + speech counts) | | port tables; per-pod hashes are stand-ins (B-P02) |
| Per-pod rig exceptions (B-P01: pods 02, 09, 12, 18, 19, 20, 22, 25 in `native_racer_rig50hw_cold.cpp`, visual/animation fixtures) and POD01-only human gate (B-P03, `original_racer_manager_entity50la.h:138`) | | a new pod with any VUT/PSG name gap will be refused until these become the general rule |
| Original racer-keyed branches (keep): AI steering x1.25 for racers 4, 8, 18, 23 (1730D0); announcer special for racer 16 (1A5650) | | a new pod gets the default behaviour |
| Wreck budgets: 13 pieces worst case per pod (POD02), SearchList 1024 slots, wreck groups 40 x 15 members | | size from the roster, never 7/9 |

## 5. Arcade against Revenge

Planets (Arcade course names from the program strings; planet mapping per the brief, unconfirmed):

| Arcade course | Planet | In Revenge? |
|---|---|---|
| SA_COURSE2 | Tatooine **(guess from brief)** | yes (Tatooine has TA/TB/TC) |
| SA_COURSE4 | Tatooine **(guess from brief)** | yes |
| ETTI4 | Etti IV | **no** (port first) |
| MALASTARE | Malastare | **no** (port first) |

Revenge planets: Tatooine, Ryloth, Mon Calamari, Gamorr, Sullust (Coruscant string only). Two new
planets need new planet strings, new course codes that do not collide (not "ma"/"sa"), and free
track IDs: 2 of the 3 free IDs (13-15). All four Arcade tracks would need 17 IDs (> the 4-bit field).

Pilots (3-letter Arcade codes; full names are **(guess)** from Episode I rosters, to be confirmed in
the Arcade data):

| Arcade | Likely pilot | Revenge pod |
|---|---|---|
| ALD | Aldar Beedo | 10 |
| ANA | Anakin Skywalker (young) | 01 |
| ARK | Ark "Bumpy" Roose **(guess)** | **missing** |
| BEN | Ben Quadinaros | 02 (13-piece, four engines) |
| BOL | Boles Roor **(guess)** | **missing** |
| CLE | Clegg Holdfast | 03 |
| DUD | Dud Bolt | 04 |
| EBE | Ebe Endocott **(guess)** | **missing** |
| ELA | Elan Mak **(guess)** | **missing** |
| GAS | Gasgano | 07 |
| MAR | Mars Guo | 08 |
| MAW | Mawhonic | 09 |
| NEV | Neva Kee **(guess)** | **missing** |
| ODY | Ody Mandrell | 11 |
| RAT | Ratts Tyerell **(guess)** | **missing** |
| SEB | Sebulba (Episode I) | 14 (18 is the Revenge Sebulba) |
| TEE | Teemto Pagalies | 15 |
| WAN | Wan Sandage | 16 |

Six pilots are missing. Only three free pod numbers (05, 06, 13) fit the 27-entry career arrays,
so the other three need wider career/save arrays (or a decision to replace nothing and add ids
27-29 with a new save layout). Arcade data must be converted into Revenge formats (PSG v3, PST rev 1,
PSM INI, COL, PHY INI, SCB/TOB BINARY blocks, SPL INI) because the port reads only those.

## 6. Open questions
* Where Arcade stores per-pilot pod vs pilot models (Revenge merges the pilot into the chariot PSG).
* Whether the PHY class rule (letters a/b in the code) should be honoured for new codes or the new
  course gets an explicit class field (original behaviour is the letter test).
* ESB billboard reader in the port: not found; GA/TC/GB/GC carry ESB.
* Identity policy for non-original content (all current loaders authenticate by original SHA-256).
