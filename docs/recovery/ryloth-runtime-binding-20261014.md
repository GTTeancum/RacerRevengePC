# Original Ryloth native runtime binding, 2026-10-14

## Boundary and current status

This is a source-inspection plan and foundation packet. Root owns all builds,
checks, tests, native extraction execution, integration, Windows operations and
game launches. No helper executable, syntax check, test, sanitizer or gameplay
was run. The resumed source task explicitly permits bounded static RES
decompression and field analysis of the root-transferred original bytes.
No RA/RC host-track admission, Course destination, unlock or fallback is added.
The user's proposed Ryloth next-course alternative remains a separate choice.

The repository retains RA/RC SCB metadata, not a complete runtime session. Exact
source intake is described in `ryloth-intake-20261014.md`; course/archive/planet
identity is owned by the independent original-course mapping investigation.
A transferred archive and an authenticated member inventory establish source
availability only. Every consumer below must bind the actual same source.

## Authenticated input identity

RA: `TRACKS/RA.RES`, 4,604,561 bytes, 1,010 members, SHA256
`1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2`.
RC: `TRACKS/RC.RES`, 4,655,141 bytes, 1,121 members, SHA256
`ce1b41b845935210c288c86d76b3ae0b99f198fb2c29190889c005631c7d2a8b`.

RA SCB: `tracks\ra.scb`, 540,795 bytes, SHA256
`57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81`,
10 zones / 10 portals / 1,261 placements.
RC SCB: `tracks\rc.scb`, 468,918 bytes, SHA256
`1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a`,
15 zones / 15 portals / 1,277 placements.

Retained inventories identify RA `tracks\mra.spl`, `tracks\ara.spl`,
`tracks\ra.tun`, `skies\ra.cub`; RC `tracks\mrc.spl`, `tracks\arc.spl`,
`tracks\rc.tun`, `skies\rcSky.cub`. TOB, AUB, CUB, spline and tuning
payload identities/counts were subsequently read from the authenticated inventory
and actual unchanged bytes; the resumed-source section below records these
facts. No inferred generated visual/texture/sky profile hash is supplied.

## Dependency graph and admission order

1. Exact Full48F generation/reference plus exact original RA/RC archive
   -> authenticated native member intake -> immutable member name/hash/extent
   catalog. Use the reviewed RA/RC intake boundary; never relabel an MA/SA
   manifest or call their profile-specific tools with a different archive.
2. That catalog plus actual SCB -> native world decode -> exact zone/portal/
   placement metadata and section/owner classification. Classification must
   distinguish visual-only, collider-only, dynamic, terrain, water and trigger
   owners. Derived `geometry_member`/`collision_member` names are candidates,
   not proof that an original asset is absent.
3. SCB + each actual placed COL -> collision decode/radii/native placement/
   category -> stable per-placement owners -> static candidates and immutable
   `track_runtime_storage50IG` -> every registered query/membership owner.
   Count placements, not distinct COL names. Require complete original owner
   classification before pinning `SessionAssets::terrainOwners`.
4. Actual map SPL -> original grid/start pose and native map navigation;
   actual AI SPL -> native AI navigation -> shared lap/map/course runtime.
   Pin source counts only after reading these bytes. Actual `.tun` ->
   `readEnvironmentImpact49Y` / original-AI environment parameters. Selected
   pod PHY/AI/rig/collision owners remain the genuine roster owners.
5. Actual SCB Environment -> fresh height descriptor -> terrain330/water334
   constructor policy -> actual water resource/provider if required. Resource
   existence does not prove a working query provider; a missing required
   provider is a blocker, not `Absent`. The existing cold adapter may supply
   this dependency when its source-supported domain really covers RA/RC.
6. Actual TOB -> section table and numeric camera/start/static/trigger/
   animated-object fields. Prove which sections are present/absent before
   adopting source start-action behavior. Construct each genuine required
   actor, trigger and dynamic collision/render owner; absence is an original
   source fact, not a policy to omit objects.
7. Every actual referenced present PSG + PSM + PST/ATM -> native decode ->
   immutable scene/material/texture closure -> placed mesh and authored normals
   -> exact generated selection/export/profile hashes -> native materials,
   alpha/pass/sampling/UV policies and original live ATM owners. A present
   unsupported PSG/PSM/PST is fatal. A static first ATM frame is insufficient
   for a complete live animation owner.
8. Actual CUB + SCB sky/fog fields -> native static sky images and exact source
   profile -> camera-zone-authored cube enable selection. Prove face masks,
   tile/mip descriptors, palette bounds and cube count; do not assume the
   MA/SA 48-tile, one-cube shape.
9. Actual SCB numeric lights + world/source-camera membership -> original
   selected-light/render lifecycle. Authored positions/normals/colors and
   source numeric defaults remain unchanged. Joining this path requires the
   actual renderer/application light-domain lifetime, not an ambient preset.
10. Actual AUB + existing original AUDIO clips -> exact material row/default/
    assignment catalog -> PCM bank -> exclusive host and per-racer logical
    D44 sound owners. Muted host output does not waive ALL8 logical audio
    ownership. Allocate from measured row counts, not MA/SA voice constants.
11. Original course descriptor/order/record pod + selected actual roster ->
    fresh AI creation metadata, class/profile and race-manager identities ->
    all real physical/AI/collision/recovery owners -> original camera bank,
    start/action lifecycle, fog, visibility and presentation.
12. Only after every required source profile and consumer exists: add immutable
    SessionAssets; extend explicit host/source enums and dispatch; publish
    fresh cold capability; preserve actual Course lock/profile state; recheck
    unchanged profile after old-world destruction. Root performs all native
    validation before any gameplay-readiness claim or launch.

Branches 3–10 can be prepared independently after 1–2. Live publication waits
for their conjunction with 11; adding branch 12 early can route an unknown
track through the current TA/GC ternary fallbacks.

## Concrete reusable native mechanisms

Reuse implementations, with separate exact RA/RC source profiles:

- `decodeResource48C` / `extractResMember48C`, `decodeWorld48F`, original
  scene/material/texture/collision decoders, `readTrackStart50AD`, original
  navigation readers and impact authoring reader.
- `authored_runtime_ground50cp_fixture::load`,
  `track_static_candidates50IG::prepare`, `track_runtime_storage50IG::Owner`,
  source reset/ground/physical query owners and actual selected racer rigs.
  Preserve stable source order, per-placement ownership and real COL masks.
- Existing native material/texture bank, authored normals, original texture
  animation, original camera/query/storage owners, lap timing and real roster
  storage after each profile and source domain independently authenticates.
- MA/SA preparation provides source staging, exact reference comparison,
  native importer invocation, world-space root-bypass policy, per-pass material
  controls, exclusive cold outputs and generated-index pinning patterns.
  Its configuration and exact source-specific assertions are not RA/RC data.

The largest retained RA conversion limitation is 580 `scene.attribute_command`
plus 3 `scene.attribute_variant` PSG statuses; RC has 19 `pst.kind` statuses.
These are historical importer statuses, not fresh runtime failures. Inspect
whether the affected original members are genuinely referenced, then extend
native source decoding at its real command/type owner when required. Do not
skip them, change geometry, fabricate materials or accept untextured fallback.

## Exact production joins still guarded

- `tests/native/track_session_assets50ig_fixture.h`: no RA/RC SessionAssets;
  its existing `select` explicitly rejects metadata-only rows.
- `platform/linux/native/host_track_selection50im.h`: enum/code/admission are
  TA/GC/GA/MA/SA only. Preserve this gate until complete independent consumers.
- `tests/native/track_source_fixture.h`: source enum is also TA/GC/GA/MA/SA.
- `tests/native/track_visual_profile_fixture.h`,
  `source_texture50fc_fixture.h`, `track_materials50fa_fixture.h`,
  `track_normals50hk_fixture.h`, `material_alpha50gx_fixture.h`,
  `sky50gw_fixture.h`, `ma_sa_atm_animation_fixture.h`: exact-profile dispatch
  and generated includes need genuine RA/RC counterparts, not aliases.
- `tests/native/original_track_sky50kx_fixture.h`: its fixed 25-zone one-cube
  owner authenticates MA/SA only. RA/RC zone counts fit the array but that is
  not evidence their source cube-selection semantics fit this owner.
- `tests/native/material_audio_provider50hi_fixture.h`: independent Track
  enum/profile/row/voice ownership excludes RA/RC. ALL8 requires its original
  D44 logical resources even when the host mixer is muted.
- `src/game/original_gc_gameplay_lighting_candidate.h/.cpp`: per-track
  CachePolicy and `prepareMaNative`/`prepareSaNative` exact SCB/profile cases
  need separate RA/RC native source projections. Runtime fog fraction fields
  are also currently limited to existing tracks; read the Ryloth SCB authored
  values instead of inheriting MA/SA's1.0 or GC's0.5 constants.
- `tests/native/original_camera_assets50kx.h`: exact track whitelist, TOB
  selection and `OriginalCameraStartCourse` selection exclude RA/RC.
- `src/game/original_camera_start_effects.h` and
  `src/game/original_camera_start_authoring.cpp`: exact TOB bytes/SHA,
  `sourceSceneCutCount3E4_` and no-`321_anims` source proof are required before
  an RA/RC enum case can authenticate. No assumed zero-camera branch.
- `tests/native/original_course_race_targets50kx.h`: actual course+planet
  capability rows are separate from lock state and cold-ready source catalog.
- `tests/native/terrain_interactive_drive50du.cpp`: startup admission, fresh
  ALL8/camera/light policies, material/audio/source-texture ternaries, TOB
  actor owners, sky/ATM lifecycle, binder eligibility, resource budgets and
  final Course readiness predicate all need deliberate RA/RC dispatch.
  The existing final TA/GC arms must never receive a newly admitted Ryloth code.

## Safely separable fresh-AI foundation

`src/game/ai_cold_owners50iu.h` has a narrowly scoped, source-only binder.
It does not admit a host track or construct gameplay. It writes:

- selectedTrackId1125F from the original descriptor table (11278, stride12)
- selectedOrder187B10 by searching independent original freshCourseOrder
- actualRaceClass from order <5 / <9 / otherwise, source16E590..16E5B4
- freshCourseRecordPod11280 from original fresh initialization
- bound only after all those fields have been proved

Current retained fresh order is `{0,11,4,8,7,1,5,9,12,6,10,3,2}`. Therefore
confirmed RA descriptor7 gives order4 / class A; confirmed RC descriptor3 gives
order11 / class C. Original initialization proves RA record pod9 at
17EA78/17EA80 and RC record pod24 at17ED94/17ED9C. These exact source-only
cases are now added to this header; host/source enums, SessionAssets, Course
capabilities and runtime admission remain unchanged. Existing fresh order
search and class selection perform their unchanged real projection.

Proof: unchanged `SLUS_202.68`, 2,972,720 bytes, SHA256
`c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
ELF LOAD offset0x80 / virtual0x100000 gives file offset PC-0xFFF80. RA
17EA44 forms descriptor112CC=11278+12*7;17EA54..5C copies actual `ra`;
17EA78 word24020009 loads9;17EA80 worda02212d4 stores112D4. RC17ED64
forms descriptor1129C=11278+12*3;17ED6C..74 copies actual `rc`;
17ED94 word24020018 loads24;17ED9C worda02212a4 stores112A4.
The record stores were independently inspected as static bytes by this helper.
`ryloth-course-map-20261014.md` retains complete original UI/localization/
descriptor provenance. Root assigned this production header only after proof;
other production edits require a separate ownership assignment.

This source change is unbuilt and untested. Root should centrally check
upper/lower-case RA and RC exact outputs (7/4/A/9 and3/11/C/24), unchanged
TA/SA/MA/GA/GC outputs, rejection of unknown/non-two-byte inputs, and that
RA/RC still fail the host SessionAssets/admission gates. No checks were run
by this helper.

## Required source packet before camera/start foundation

From exact unchanged RA/RC TOB bytes retain: member spelling, byte length,
SHA256, complete section catalog, exact `[CSCameras].NumberOfCameras`, presence
or absence and count of `[321_Anims]`, and any records needed by the positive
cut/start branch. Match these against source parser/consumer semantics. The
current `OriginalCourseStartActions` represents the no-321-anims source path;
if either Ryloth TOB has that section, extend the real owner instead of forcing
its count to zero. This packet can be prepared without opening host admission.

## Resumed original-byte source analysis

Root supplied unchanged RA.RES/RC.RES through the approved central Windows
transfer. Bounded standard-library static RES3 block decompression authenticated
the exact archive SHA/length before analysis, every block's24576-byte output,
zlib stream end/no unused input,0xff boundaries, unique member names and exact
member extents. No native extractor, importer, test or project executable ran.

`ryloth-cold-source-metadata-20261014.json` retains the complete measured member
identities, source COL structural profiles, SPL headers/grid fields, TUN values
and relevant SCB field offsets. It is source evidence only, not admission.

RA decoded extent is29,687,808; RC31,703,040. These are the actual padded native
block extents, replacing the earlier minimum-end observations in the intake
packet. Source SCB counts match the unchanged retained metadata.

- RA map `tracks\mra.spl`:23,820 bytes,115 nodes, SHA256
  `99ac08267db1a7fe3a161c46f1e941200390b68c9300bdc59d9e42608aecc52c`.
  Node0's ordered Next0 predecessor is103. AI `tracks\ara.spl`:23,404 bytes,
  113 nodes, SHA256
  `a074570af7e199dadab63fabdad37cfa4ea1d36d537ff5ef4003dacadfc397f3`,
  predecessor101. Maximum next count2; all source indices are within each
  actual graph. No StartNode field is present in either original header.
- RC map `tracks\mrc.spl`:39,508 bytes,186 nodes, SHA256
  `f36306b7bdac88401b810add285785f770491987dbc55de7e0e5cd05319b270c`.
  Ordered Next0 predecessor130. AI `tracks\arc.spl`:43,780 bytes,209 nodes,
  SHA256 `5687db7ebe3929fcf455121222d4d98e617332091e7bbbc291886403e6135171`,
  predecessor145. Maximum next count2; all source indices are in range; both
  original headers omit StartNode.
- Actual tuning is RA922 bytes/SHA256
  `fb3e303604a27265585a498943d87099177d99f4701d1a75165d175e053044f8`
  and RC978 bytes/SHA256
  `3084d5216fb70e70d843f53ea7195f51c47fd3d096932534631dbadf07635c35`.
  Both retain EnvDamage,PodDamage,AIDriving,Packing sections. Preserve each
  original value; RC's PodDamage/repair/race-pace differs from RA.

### Original collision and initial-zone facts

Every SCB placement was joined against the exact source archive catalog:
RA178 placed COL owners/178 distinct members; RC112 placed owners/93 distinct
members. No placement has both its derived PSG and COL names absent.
Per-zone placed counts: RA20,10,13,35,14,31,10,13,19,13;
RC26,2,9,5,7,4,9,10,10,3,2,8,4,9,4. All authored generic SearchType tokens are
exactly `List`, so the existing generic-present zone-search route applies.

All selected original static COLs have version1/type0, reverse_mode0, rooted
nonempty full binary trees and exact body extents. RA107 unquantized/no-normal,
69 unquantized/normal,2 quantized/no-normal members; RC72,16,5 respectively.
No quantized-normal or single-leaf domain is needed. Largest tree depth is
RA34/RC28; max nodes1857/1299; max faces1858/1300; max vertices1385/878.
Filename classification with the actual current masks gives RA109 category4
and69 category20; RC96 category4 and16 category20. No source name adds the
water/tube category bits. Native source decode/query success remains untested.

RC has three authentic exporter-degenerate planes: Zone09_Wall_COL face113,
Zone13_Wall_COL faces153/156. Each has D bits7fc00000,normal bitsffc00000 in
all three lanes and coincident finite original vertices. This is exactly the
existing `import_collision48g.cpp::validate` evidenced source subset, with no
plane/face/tree removal. The data packet retains each member hash, face offset,
indices and positions. RA selected static COL planes are all finite.

Original fresh initial collision zone is the first actual collision table
entry, native reference1:1AE578 word8ea304c0 loads4C0,1AE57C word8ea204c4
loads4C4;1AE588/58C words8c670000/8c480000 load their first entries;
1AE590 word0c085a70 calls2169C0.216A2C wordac870c30 stores visibilityC30;
216A38 wordac880c34 stores collisionC34. These unchanged SLUS instructions
were read as static bytes. This proof covers fresh grid creation only.

`SessionAssets::gridRank=4` would preserve the explicit existing native neutral
center setup policy, not claim an authored Ryloth rank. Final genuine native
grid geometry comes from the selected actual M spline/gates via
`source_grid50IG::prepare` and current native rank assignment4,0,1,2,3,5,6,7.
Each actual selected PHY supplies lift and each ground query uses real COL
owners. No guessed X/Z placement, route-height substitution or full-grid query
success is claimed by this static packet.

### Height, sky and source-default closure

Both SCBs explicitly retain Environment.TerrainFile=`nat13.trn` and no WaterFile
field. Existing fresh1A9EE0/270550 game-track ownership clears terrain3FC and
does not construct terrain+12C; missing WaterFile gets actual158AD0 `none`
default and2745B0 bypasses water construction. Therefore this fresh route has
absent terrain330/water334 providers without pretending the authored terrain
filename is `none`. Both SceneInfo blocks omit UseEnvironmentNode, matching
actual270A1C default0 and the existing reset bridge's no443108 fallback route.

Both have NumberOfSkyCubes1. RA Cubefile1=`ra.cub` with zones1/2/8 authored0
and all other zones authored1; RC Cubefile1=`rcSky.cub` with no zone override
fields. Original276D7C word24060001 supplies default1;276E24 word2442ffff
subtracts1 before276E28 wordac620000 stores the index. Authored0 therefore
disables; missing RC fields genuinely select cube0. Presentation275894,
2758BC,275914..275964 uses the current camera zone and enables only the
nonnegative selected cube. These source semantics are already represented by
the exact-MA/SA-only `original_track_sky50KX::Owner`; separate RA/RC source
authentication is required before reuse. Assembly worker received the facts.

Actual TOB identities now available for the separately assigned camera worker:
RA `tracks\ra.tob`,95,558 bytes,SHA256
`7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524`;
RC `tracks\rc.tob`,124,930 bytes,SHA256
`71b1f9609c0a6ce84d71ff69355d9cfcbc8771eb7ddb8876473b4fddd6e23e6d`.
This helper no longer owns camera files and made no camera production edits.

No RA/RC SessionAssets is published yet: generated visual/material/normal/sky
profiles and complete live dynamic/trigger/light/audio ownership remain separate
work. An unused production metadata type was deliberately not introduced.
