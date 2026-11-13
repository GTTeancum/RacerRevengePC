# Arcade -> Revenge converter, Phase 1: geometry side (scale, handedness, PSG / PST / PSM)

2026-11-12. Scratch only; nothing in the repo was changed, the port was not built. One tool for all
five courses (no per-course code; the only per-course item is a data table of course codes).
Tools in `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\a2r\` (all `geo_*.py`,
run with `python -B -I`), output in `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\course<N>\`.

## 1. Scale and handedness

### 1.1 Scale: x3.0 (arcade unit -> Revenge foot)

**Pod size** (`geo_scale_pod.py`): arcade ANA (Anakin; `export\pods\ANA\ANA_pod_lod0_tg2.obj`, parts
placed by the game's own offsets) against Revenge POD01 (Young Anakin, same pod design), bind pose
through the PSG node matrices.

| Measure | Arcade | Revenge | Ratio |
|---|---|---|---|
| chariot / cockpit length | 3.728 | 10.900 ft | 2.92 |
| chariot width | 2.037 | 6.092 ft | 2.99 |
| chariot box half-diagonal | 2.166 | 6.405 ft | 2.96 |
| chariot radius 6.516 ft (authored-geometry49o.md) vs arcade half-diagonal | 2.166 | 6.516 ft | 3.01 |
| chariot height | 0.851 | 2.859 ft | 3.36 (Revenge chariot is taller in design) |
| engine-to-chariot rest length 20.78 ft vs arcade engine-centre-to-cockpit 7.50 | | | 2.77 |
| engine length / width / height | 5.46 / 0.99 / 1.17 | 25.2 / 3.9 / 4.0 ft | 4.6 / 3.9 / 3.4 (different engine model) |

The chariot is the only part that is the same design in both games: length, width, half-diagonal
and radius give 2.92-3.01.

**Track half-width** (`geo_scale_track.py`, result in `tools\a2r\scratch\scale_track.txt`):
Revenge AI-spline half-width, median of the 13 course medians 57 ft (25-119; RA, GA, GC often capped
at 160); Revenge map (lap) spline 115 ft (40-210). Arcade AI guide edges (race-paths.txt) median
30-60 units per course; arcade collision road (walked perpendicular to the centre line over non-wall
collision) median 29-140 units. Ratios: AI/AI 1.65 (0.96-2.53 per course), map/road 2.23 (0.82-3.93).
Track width is a design choice per course, so this measurement scatters by a factor of 4 and does not
agree with the pod within 10 %. It says only that at x3.0 the arcade roads come out wider than a
typical Revenge road (Etti IV road half-width ~125 ft, about Revenge's map-gate width).

**Chosen: 3.0** (pod measurement, the like-for-like one; 3.28 "metres to feet" would make Anakin's
chariot 10 % larger than Revenge's). One constant, `SCALE` in `geo_course.py`.

### 1.2 Handedness: arcade right-handed, Revenge left-handed -> negate Z

Revenge (from its own data): POD01 engines sit at z -3..22, the chariot at z -17.6..-6.7, so the pod
faces +z; the nodes named left (`englt1`, `Ani_L_uparm`, `handlt`, `thrlt`, `chariotcableleft1`,
PHY `LeftEnginePosition=-0.3`) are all at -x. Facing +z with left at -x means right = +x =
Up x Look: left-handed, as the original collision creator says (`src\assets\import_collision48g.h:81`).

Arcade:
1. The pod faces -z (cockpit at +z behind the engines; the centre line leaves the start heading -z,
   pod-placement.md, race-data.md).
2. The arcade's own left/right-named pilot motions (`geo_handedness.py`, output
   `tools\a2r\scratch\handedness.txt`): `L_crash`/`R_crash` and `L_pro`/`R_pro` move the body toward
   -x for L and +x for R in 14 of 16 cases (ANA, BEN, GAS, SEB; ANA's crash pair is not clean). With the
   pod facing -z, left = -x means right = +x = Look x Up: right-handed. (`L_chira`/`R_chira`, "glance",
   go the other way for 3 of 4 pilots: the joint that moves most there is an arm, not the body; not
   used as evidence.)
3. Odin (the Hikaru emulator, `ref\odin-src`) projects with a glFrustum matrix, eye looking down -z,
   x right, y up (`machine\boards\hikaru\hikaru_draw.hpp:152`), and its comments tie that to cabinet
   pictures (a negative distance is the left-right-flipped rear-view mirror of SGNASCAR). A right-handed
   eye space with rotation-only camera matrices gives a right-handed world.

No text exists on the arcade course textures (contact sheet `tools\a2r\scratch\course_tex_sheet.png`)
and the course-select images are promotional renders with no map, so those checks were not possible.
Still open: confirm on the running cabinet (Odin + BIOS sets).

**Conversion** (one formula for courses and pods): `revenge = (3x + ox, 3y + oy, -3z + oz)`, normals
`(nx, ny, -nz)`, triangle winding reversed. With this the arcade pod's forward (-z) becomes +z and its
left (-x) stays -x, matching POD01 directly. Courses: ox, oz put the landscape minimum corner at
(2000, 2000) ft; oy = +1000 ft (guess; Revenge ground heights 77-10,900). The offsets are stored in
each `manifest.json` (`transform`); collision, splines and grid must use the same transform.
Course 3 (Boonta) spans about 27k ft after scaling, above anything Revenge has.

## 2. Writers, proved on the original files

| Module | Format | Proof |
|---|---|---|
| `geo_res.py` | RES3 reader (read-only) | used for all the checks below |
| `geo_psg.py` | PSG v3 reader + writer (multi-node, LODs, surfaces, 16- and 32-bit positions, colour / no-colour surfaces, matrix palette) | `geo_roundtrip.py`: **5,877 PSG (5,700 unique) from all 13 tracks and all 23 pods re-encoded byte-identical** |
| `geo_pst.py` | PST rev 1 reader + writer (kind 0, 1, and kind 2 = IPU-compressed levels, each with a u32 byte count before an `ipum` stream); RGBA -> kind 1 encoder | **1,115 PST (886 unique) byte-identical**. Encoder (`geo_pst_check.py`, TA + POD01): 40 of 40 kind-1 textures decoded, re-encoded and decoded again by the repo decoder give the largest level pixel-exact and the same level sizes; regenerated smaller levels differ from the originals by 7 (median) of 255 per channel (different filter, same palette) |
| `geo_psm.py` | PSM (CRLF INI) reader + writer + canonical new-material writer | **2,438 PSM (1,876 unique) byte-identical**; 833 files are in the canonical layout (key padded to column 42), the rest keep their own tabs/spacing line by line |
| `geo_encode.py` | mesh -> PSG (own stripifier, quantiser, group packer) | `geo_encode_check.py`: **582 of 582** one-object PSGs of TA and GC rebuilt from their decoded triangles decode (repo scene decoder) to the same triangles, normals, UVs and colours, positions within one quantum; files 1 % larger than the originals |

Format facts measured on the originals and used by the encoder:
- Strip packing: strips go greedily into VIF groups of at most 16 vertices; a strip that does not fit
  the room left starts a new group; a strip longer than 16 is split and the next group repeats its last
  two vertices with the ADC flag; the header strip count counts source strips. This reproduces every
  surface of TA, GC, MC, RA and POD01 byte for byte (`scratch\pack.py`). The encoder caps strips at
  15 vertices so no continuation is ever needed: the port's strict decoder
  (`import_decode48c.cpp:510`) refuses continuations, and an exactly-16 strip can be mistaken for one by
  `tools\assets\decode_scene48c.py`.
- Quantisation: normals s8 x 1/127, UVs s16 x 1/2047 (so |u| <= 16), positions s16 x (largest
  |p - origin| / 32767). World pieces carry flag 1 and their own origin; local objects flag 0, origin 0.
  Original track pieces share one origin per course (TA quantum 0.209 ft).
- Colours: prelit RGB 0..127, alpha 63 = opaque (every TA/MC vertex; GC water edges use lower alpha).
- PST palettes: alpha 255 = opaque; the stored index is used with bits 3 and 4 swapped.
- `uvSize` drives texture-resolution demand (`original_standard_material_object50la.cpp:1473`); the
  originals track the largest world-length / UV-length edge ratio within ~5 %, the encoder writes that
  maximum.

## 3. Conversion of the courses (`geo_course.py N`)

Same code for every course: the arcade scene comes from `tools\export_course.py` (tables, display-list
decoder, texel decoder with the pair unswap); per triangle: transform, winding, baked colour, cut to a
2016-ft grid (Sutherland-Hodgman, so neighbours are cut at the same points), triangles spanning more
than 31 texture repeats cut at every 8 repeats (their neighbours first receive the same cut points, so
no T-junction is left), UVs shifted by whole repeats around the centre into range, then one PSG per
grid cell and material (one object, one material, one LOD: the narrowest layout every port decoder
takes). All world pieces of a course share one position lattice (step 1/32 ft, origins snapped to it;
a piece taller than 2,047 ft doubles the step), so a vertex shared by two files lands on the same point.
Textures: one PST (kind 1, 256 colours, mips down to 16 px) + PSM per arcade texture.

**Defect fixed (Malastare spire streaks, reported from `export\a2r\lc\verify\sky_chase_1.png`).**
Cause: texture 2129 is a cut-out decal of thin dark lines with light dots laid over 4,608 spire/rock
triangles, coplanar, with Hikaru 101 depth bias +16. Odin's rule (`depth_bias.hpp`, public source):
for opaque and cut-out meshes the 10-bit field (bits 25:16) brings a mesh nearer, larger = nearer;
translucent fragments are blended over what they are not behind. So the decal belongs ON TOP of the
rock. The earlier converter read the sign the other way (0.003 ft behind), leaving the decal
z-fighting and showing in broken streaks; the arcade reference preview hides the decal entirely
because its renderer ignores the bias and draw order favours the rock. Now each course's distinct bias
values become ordered layers, 1/16 ft apart along the vertex normals (opaque by field, translucent
above all opaque). Checked with `tools\a2r\geo_bias_check.py`: arcade data drawn with the bias rule and
the decoded conversion match line for line (`tools\a2r\scratch\bias_check_c2_v1.png`; Etti IV road
`bias_check_c1_v0.png`). The 76 / 120 UV-split triangles of Malastare / Etti IV were a second crack
source (T-junctions) and the per-file quantisation a third (seams); both fixed as above.
Texture sweep, all five courses: no alpha-test mismatches, sizes equal; textures with more than 256
colours differ by 0.3-3 of 255 on average (a few pixels up to 40); nothing streak-like comes from the
textures, mips, wrap or pair unswap. Whether the decal lines show on the real cabinet still rests on
Odin's (heuristic-marked) bias rule; the cabinet check is open with the BIOS sets.

| Course | Code | PSG | Textures | Triangles in -> out | Largest quantum | Geometry bytes |
|---|---|---|---|---|---|---|
| 0 Tatooine Bantha Tracks | ua (placeholder) | 168 | 20 | 28,779 -> 34,609 | 0.031 ft | 1.30 MB |
| 1 Etti IV | eb | 344 | 32 | 40,022 -> 60,184 | 0.031 ft | 2.30 MB |
| 2 Malastare | lc | 186 | 30 | 36,219 -> 41,553 | 0.031 ft | 1.56 MB |
| 3 Tatooine Boonta Classic | td (placeholder) | 316 | 26 | 59,937 -> 70,297 | 0.063 ft (one tall piece) | 2.63 MB |
| 4 Kaadu oval (cut content) | ka (placeholder) | 22 | 7 | 5,537 -> 6,566 | 0.031 ft | 0.26 MB |

Triangle counts rise from the grid and repeat cuts (Etti IV most: its long UV strips are cut and their neighbours get the same cut points). One sub-step sliver on Kaadu that turned over when rounded is dropped (encoder drops only triangles whose facing reverses on rounding). The sky dome is written as a local object
(`<code>_sky_*.psg`, quantum not held to 0.2 ft; Revenge uses a sky cube, a later step).

Generated items (no arcade source; listed in every `manifest.json`):
- **Vertex lighting**: colour = material ambient + diffuse x max(0, N.L) x 0.7 for lit materials,
  diffuse for unlit ones; sun direction (0.4, 1, 0.3) normalised; stored round(colour x 128) capped at
  127, alpha x 63/255. The result is brighter than Revenge's own tracks (their median vertex colour is
  about 50 of 127, ours about 115 on flat ground) because the arcade textures expect the arcade's
  lighting; the level needs a look in the game.
- Two-sided arcade faces become two triangles (the back one with reversed normals, as Odin does).
- Hikaru 101 depth bias (coplanar decal layers: Etti IV road field 513 / 1 with a translucent tint,
  Malastare line decal 16) becomes ordered layers 1/16 ft apart along the vertex normals (rule above).
- PSMs: `PreLitCompressed`; translucent poly type 6 adds the alpha-blend keys of TA `crowd.psm`. Cut-out
  textures (poly type 4) use the plain material, like GC `tree_branch_a.psm`.
- Placement offsets, grid names, mip levels, palettes, `uvSize`.

## 4. Verification (`geo_verify.py N`)

Every output file was decoded by:
- `geo_psg` / `geo_pst` / `geo_psm` (own readers; re-encode gives the same bytes);
- the repo's strict static decoder `tools\assets\decode_static48b.py decode_psg3` (Python twin of
  `src\assets\import_decode48c.cpp`: one object, identity matrix, one material, one LOD, stored bounds
  within a quantum, every triangle's winding agrees with its stored normals);
- the repo's scene decoder `tools\assets\decode_scene48c.py`;
- `tools\assets\decode_texture48d.py` `decode_texture` (levels >= 16, power of two, ascending) and
  `parse_material`.

Result, all five courses: **1,036 of 1,036 PSG, 115 of 115 PST and 115 of 115 PSM accepted by every
decoder; 0 triangles with winding against their normals.** Details in `export\a2r\course<N>\verify.json`.

Previews rendered from the decoded data (repo decoders' vertices and RGBA), drawn as a left-handed
renderer would show it, with the same top and chase cameras as `export\course<N>\preview` through the
same transform: `export\a2r\course<N>\preview\compare_original_vs_decoded.png` (left original export,
right decoded conversion). Looked at for all five courses: top views and all 20 chase views match
(layout, textures, UVs, which side things are on). One small difference: Etti IV chase 0 shows a darker
wall end at the left edge. `top_baked_light.png` shows the generated lighting alone (smooth, slopes
shaded, flat ground near full brightness).

## 5. Open items

- Handedness on the real cabinet (needs MAME sets hikaru + mie for Odin).
- Light level of the baked colours against Revenge's tracks (in-game look).
- Additive blend modes (Hikaru material blend 2/3, Malastare glows) are written as plain alpha blend.
- Arcade fog and light values not found yet (courses.md).
- Course codes for courses 0, 3, 4 are placeholders until Phase 5.
- Grid split is not yet the Revenge zone split (zones/portals come with the SCB step); collision (COL),
  splines, SCB, RES packing are other Phase-1 parts.
- Note for the parallel track-data helper: at 18:20 this work briefly overwrote its first
  `tools\a2r\res3.py` (name clash); it has since moved to `trk_*.py`. Geometry tools use `geo_*` only.
