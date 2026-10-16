# Original world Environment sky-selection owner

Source-only recovery and native implementation, 2026-10-14. This packet adds
`src/game/original_world_environment50kg.{h,cpp}` only. No build, test, syntax
check, import, sanitizer, project executable, gameplay, guest execution or
emulation was performed. Loader integration and validation remain root-owned.

## Actual source owner and fields

The source `270A58..270A84` allocates `0x550` bytes, invokes constructor
`276950`, publishes the actual EnvironmentInfo at world `+3A4`, and reads it
with the world's real parameter reader and world zone count `+34C`. Table
`3D2B50`, slot `+8`, resolves to `276C00`; the type pointers resolve to the
literal `EnvironmentInfo`. Constructor clears count `+0`, cube-name array `+4`
and zone-index array `+8`. This native class owns only that proven sky-selection
portion. It makes no replacement for the remaining EnvironmentInfo fields.

Recovered read semantics:

1. `276C2C` selects the actual Environment block; missing block fails.
2. `276C4C/276C54` reads NumberOfSkyCubes with default **1** into count `+0`.
   `276C60` skips the cube and zone allocations when count is nonpositive.
3. `276C68..276D34` allocates the name-pointer array and reads CubeFile1,
   CubeFile2, etc., default literal `none`. Each original filename allocation is
   **0x104 bytes (260 decimal)**. Only failure to read the first numbered key
   enters `276CD8..276D1C` and retries the unnumbered CubeFile key. A present
   literal `none` does not trigger that fallback.
4. `276D40..276E38` constructs one row per actual world zone. The key is
   CubeFileForZoneN, where N is the one-based authored zone label. `276D7C`
   supplies default **1**. `276DD8..276DE0` checks authored value <= cube count
   through the original diagnostic. `276E24/276E28` subtracts one and stores:
   authored 0 becomes **-1**, authored 1 becomes **0**.
5. Only after those rows exist, `276E40..276E88` collapses a sole literal `none`
   cube to count **0** and frees its filename. Existing zone rows are retained.

The source renderer namespace is separate: `274AE0` constructs actual SkyCube
children in world `+3F8`; it does not create canonical visibility zones `+4C0`.
`275894` reads the selected camera's actual **zero-based** zone `+48`.
`2758BC` updates actual sky child flags; `275914..275964` enables only a
nonnegative selected cube. The preparation is inside actual world slot40;
source order and child flag semantics are not certified by a host swap sample.

Static evidence is retained by the portal/render recovery worker in
`docs/recovery/portal-join-next-20261014.md` and the external
`portal-traversal-owner-map/render-entry114d00` field maps. The exact retained
source slice identities are:

- `276950..276BC0`, 624 bytes, SHA256
  `055257f5ffbaabafd7da77e95600bd8ad432365df92898871c83818733d6bf5d`
- `276C00..277240`, 1600 bytes, SHA256
  `a36faa9a42917bb197d169f7cb95c573ee95415954a1ce7a84dd7c9e19f2d2c7`

These are static original bytes and decoded fields, not execution evidence.

## Native contract

`rr::game::original_world_environment50KG::Owner::prepare` accepts the actual
selected authenticated `assets::World48F`. `World48F.parameters` already retains
every sorted original block/field and the exact `original_bytes`; no typed sky
fields are invented in that decoder. The owner deep-copies the whole world
once, checks exact reversible parameter serialization against those original
bytes, validates typed world semantics, and requires contiguous actual zone
labels. Only const source views are exposed. The caller's mutable world cannot
alter a prepared owner afterward. Failed preparation publishes nothing.

Exact serialization and semantic consistency do **not** authenticate an
archive, a selected track, or a digest. The real cold loader must authenticate
its selected SCB/archive before calling prepare. The owner is track-neutral;
it never chooses a different world's zones or files to pass admission.

Typed data comprises effective cube count, source cube filenames and signed
zero-based indices. Count-zero initialization owns no zone array, matching the
source early branch. Sole-`none` collapse preserves already-read zone indices
but has no live cube name. `selectCameraZone48` reports a valid disabled
selection separately from errors and leaves output unchanged on failure.

The native integer admission is deliberately narrower than the original
base-zero conversion: nonnegative decimal spelling, optional leading `+`,
no multi-digit leading zero, signed32 range. Unproven negative/octal/hex,
whitespace or annotated forms fail rather than being normalized or clamped.
Negative source values are not silently treated as authored zero. A native
1024-cube storage bound and the decoder's 256-zone bound are admission limits,
not recovered fixed source capacities. Names must fit the source 0x104 buffer.
Unused Environment fields stay retained and uninterpreted in the real source
document; terrain/water, fog, resource construction and Ecosystem are not
derived from convenient host defaults.

## RA and RC authored behavior

The exact RA and RC SCBs identified in
`docs/recovery/ryloth-runtime-binding-20261014.md` each author one cube.

- RA `tracks\ra.scb`: 540,795 bytes, SHA256
  `57a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81`,
  10 zones / 10 portals / 1,261 placements.
- RC `tracks\rc.scb`: 468,918 bytes, SHA256
  `1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a`,
  15 zones / 15 portals / 1,277 placements.

- RA CubeFile1=`ra.cub`; CubeFileForZone1/2/8 author **0**. Those authored
  labels become camera zone48 values **0/1/7**, whose typed rows are **-1**.
  RA's remaining actual zone rows author 1 and select cube0. No unrelated zone
  ordinal is substituted to keep its sky visible.
- RC CubeFile1=`rcSky.cub`; it has no zone override fields. All actual RC camera
  zones therefore receive the recovered **default1 minus1 = cube0**.

The implementation has no RA/RC hardcoded zone mask: those results arise only
from each retained real Environment block and the selected world's zone count.
Source authentication remains independent from this projection.

## Concrete loader integration proposal

The current root retains actual ground.world in
`tests/native/terrain_interactive_drive50du.cpp`, loads real sky resources with
`sky50gw_fixture::Owner`, and keeps an MA/SA-only
`original_track_sky50KX::Owner sourceSkySelection` at the cold sky setup. The
presentation block obtains the camera zone through the actual retained
`sourceCameraSession->readCameraPrimaryZone48`, then sets renderFrame.skyEnabled.

Root-owned next steps:

1. Add the new `.cpp` to the real native target's source list. Retain one new
   Environment owner beside the real session/sky owners for the full race
   lifetime; prepare it from the **same authenticated ground.world**, after
   selected-source admission. Do not prepare a temporary and discard it or
   publish only a Boolean mask. Do not change SessionAssets to pretend RA/RC
   gameplay admission is complete.
2. For the admitted RA/RC cold routes, use the owner's exact cube filename to
   cross-check the independently authenticated real cube/resource loader.
   Do not select a generated profile solely from a track enum when its source
   Environment disagrees. A multiple-cube source needs actual corresponding
   retained SkyCube resource owners and per-child draws; a single native sky
   mesh cannot stand in for those resources. Do not widen the single-mesh
   renderer's domain silently.
3. At the existing bounded camera-zone presentation callsite, obtain the
   actual camera zone48, call selectCameraZone48, and publish its enabled bit
   only after success. For a single real resource, also require its nonnegative
   cube index to be 0. A missing camera or out-of-range zone is an explicit
   failure, never zone0 fallback. Keep native sky clear/background colors under
   their existing real resource owner's contract.
4. Preserve current MA/SA admission until their authentication and callsite are
   deliberately migrated. The new source owner does not require deletion of
   their stricter existing fixture or widening unrelated worlds.
5. Separately join original phase3 preparation to actual world3F8 SkyCube
   child flags and original slot48 resource/model dispatch when the full real
   Entity forest is available. The presentation enabled bit is a useful
   bounded consumer, not proof that `275630` or retail rendering completed.

Root validation should establish: immutable retained bytes, one-shot and
transactional behavior, missing defaults, first-key fallback only, explicit0,
range rejection, count0 early path, sole-`none` collapse, and exact RA disabled
camera zones0/1/7 plus RC all cube0. Multi-cube selection should be validated
independently of any future multi-cube renderer. None was executed here.
