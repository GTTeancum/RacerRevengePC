# Racer Arcade courses: formats and exporter (Nov 12 2026)

Tool: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\export_course.py N` (N = 0..4,
one code path for every course; `--no-preview` skips images). Libraries: `tools\hikaru_mesh.py` (display
list -> triangles, Odin rules), `tools\softrender.py` (numpy z-buffer previews), `tools\course_preview.py`,
`tools\mesh_check.py` (4-view render of one model, back faces red).
Output: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\course<N>\`.

| N | Name | landscape tris | sky tris | textures | collision tris | centre line (points, length) |
|---|---|---|---|---|---|---|
| 0 | SA_COURSE2 = Tatooine "Bantha Tracks" | 28 779 | 756 | 64 | 8 000 | 400, 12 982 |
| 1 | ETTI4 = Etti IV "Smuggler's Cove" | 40 022 | 240 | 29 | 6 184 | 500, 13 043 |
| 2 | MALASTARE = Malastare "Pixelito Challenge" | 36 219 | 240 | 34 | 7 352 | 500, 10 078 |
| 3 | SA_COURSE4 = Tatooine "Boonta Classic" | 59 937 | 216 | 72 | 16 000 | 800, 26 874 |
| 4 | Naboo-style oval with Kaadu (not a race course; guess: Kaadu scene) | 5 537 | 240 | 7 | 1 520 | 100, 783 |

Every course decodes with no unknown CP words, no jumps/LOD/matrix instructions, and every texhead index
inside the texset's list A. All five were looked at (previews below); collision and visual line up.

## Files per course

| File | Content |
|---|---|
| `landscape.obj` / `course.mtl` / `tex\t<id>.png` | all landscape objects (record +4..+16), node matrices applied; one material per (material, texture, poly type); `map_d` where the texture has transparent texels |
| `sky.obj` | landscape +20 object (one-node sky dome) |
| `collision.obj` | collision triangles grouped `surface_<code>` |
| `collision_tris.tsv` | per triangle: centre, radius, adjacency, w47, w48, surface, face normal, raw words 13..43 |
| `path.obj`, `path_table.tsv` | track centre line (closed loop), y taken from the collision ground |
| `stat.obj`, `lens.obj` | *_STAT / *_LENS quad lists |
| `course.json` | every table field, materials, per-mesh material/texhead/texture/polytype, texture formats and wrap bits, opcode counts, bounding boxes |
| `preview\top_textured.png`, `top_flat.png` | orthographic top view |
| `preview\overlay_collision_top.png` | collision triangles coloured by surface over the visual mesh, centre line yellow, path[0] circled |
| `preview\chase_0..3.png` (+ `_collision`) | 9 units above the centre line at 0, 1/4, 1/2, 3/4 of the lap, looking along it; the `_collision` twin draws the collision wireframe over the grey visual mesh |
| `preview\collision_vs_visual_height.txt` | collision vertex y minus top visual surface y |

## Display-list decode (hikaru_mesh.py), triangle rules from Odin antarctic_mesh.cpp

- Ring of 3 vertices; a position push (12x/1Ax/1Bx) rotates it; with header bit 1 (pivot) only slots 1
  and 2 swap, so slot 0 stays as fan centre. A 12x/1Ax push clears the vertex's UV.
- **The triangle is closed by whichever push has bits 11:9 = 7; in this game that is the 158 UV push**
  (`FF00E12C` vertex, then `00000F58` UV + close). The closing push's header replaces the vertex header,
  so the 158's bit 0 (winding) and bit 23 (no cull, emit back face too) decide. Winding bit set ->
  (0, 2, 1). Zero-area triangles are dropped. The ring is cleared only at list start.
- Before (helper's quick renderer) vs now, same 1518 / 1193 triangles: boundary edges 1201 -> 249 and
  775 -> 229, edges shared in the same direction (= flipped triangles) 1415 -> 41 and 1181 -> 0.
  `export\meshcheck\m02C13D20.png` (pod hull, model 3) and `m02C00320.png` (engine, model 1): closed,
  consistently wound. The correct model start is the record address (0x02C13D20), which carries the 901
  precision word; starting at the first vertex (0x02C13D3C) loses the scale.
- Output winding: counter-clockwise front faces in course coordinates, y up (as written to OBJ).

## Tables (all addresses CPU, eprom.bin offset = addr - 0x0C00FC00)

- Course table 0C3E4B30, 12 B: landscape id, collision id, stat id (identity 0..4).
- Landscape 0C3E7FF8, 52 B: +0 texset, +4..+16 objects (only +4 used), +20 sky object, +24 centre-line
  table index, +28/+32 floats (0: 2400/2600, 1: 1800/1900, 2: 1000/1200, 3: 3000/3100, 4: 2500/3000;
  guess: fog start/end or draw distance), +36 int (104, 32, 70, 45, 150; unknown), +40/+44/+48 callbacks.
- Object record (16 B): node array (RAM), node count, root model, flags (1). **Node, 104 B**: +0 model id,
  +4 parent, +8 first child, +12 next sibling, +16 always -1, +20 bounding sphere xyz + radius,
  +36 always 0, +40 4x4 float matrix (row vectors, translation in row 3). Landscape trees are flat:
  node 0 root, node 1 group, nodes 2.. one tile each; all matrices are identity in the five courses.
  **The root model has no geometry; its attribute block is the material list** (091/291/881/A81/C81/491/691
  then 084 commit n) shared by all geometry models of the object; geometry recalls 083 n with bit 12.
- Texhead index n (0C3 n with bit 12) = position n in the course texset's list A
  (f_0C14CD40 builds 0C1 / 2C1 / 4C1 and commits `10C4 | n<<16`). Texture record: addr, log16 w,
  log16 h, bytes, 0C1 word, 2C1 word; level 0 = first w*h*2 bytes; 2C1 bits 28:26 format (0 ABGR1555,
  1 ABGR4444), bits 22/23 repeat, 24/25 mirror, 14 whole-texel UVs. UV = s16/16 texels / texture size.
- **Texel pairs are swapped in ROM** (both mask and EPROM textures): the two 16-bit texels of each
  32-bit word are stored right-then-left (IDMA source order). export_course.py un-swaps through the shared
  `mask_tex.texel_words()` (switch `SWAP_TEXEL_PAIRS`). Checked at 4x
  (`export\texswap_check\before_after_4x.png`: chevron barrier 2112, railing 2130, EPROM chevron E158):
  unswapped edges zig-zag one texel and railing bars double, swapped they are clean. Numerically, the mean
  colour step across pair boundaries drops to the within-pair step (e.g. 2112: 39.2 -> 13.9 vs 13.1).
  All five courses were re-exported with it (Nov 12).
- Poly types used: 1 (opaque) everywhere, 4 and 6 (Odin: translucent-ish/punch-through types) on Etti IV,
  Malastare, Boonta and course 4 (water, gratings, trees, glows). Material flag bytes: 0C, 0D (textured,
  Gouraud, +fog), 1D (+alpha mode), 04.

## Collision (table 0C3E4ACC: block, root, size)

- **Triangle array** at the block start, 196-byte records; the count is where the spatial-index
  lists begin (first record whose word 3 is a small integer): 8000 / 6184 / 7352 / 16000 / 1520. Checks:
  adjacency and w47 stay below the count, w48 >> 16 equals the record's own index.
- Record words: 0-2 centre, 3 radius, 4-12 v0 v1 v2, 13-24 four normals (face normal at 22-24 equals
  13-15 in the samples), 25-27 another normal, 28-43 four planes (nx ny nz d; edge planes plus the face
  plane, guess from values), 44-46 neighbour triangle per edge (-1 = none), 47 a triangle index (meaning
  unknown), 48 = (own index << 16) | surface code.
- Surface codes: 0: 2 3 5 6 7; 1: 7 8 9 C D E F; 2: 5 6 7 A B; 3: 2 3 5 6 7; 4: 8; plus **0x8000 in every
  course = the side walls** (1600 / 2000 / 2000 / 3200 / 400: they are the magenta bands on both edges in
  the overlays). Course 1's 9 is the dock section, Malastare's 7 the tunnel/bridge stretch (by eye on
  the overlays). What each code means to the physics (speed, sound, damage): **open**.
- After the triangles: spatial index (index lists, then cells with 4 x/z corner pairs and pointers into
  the lists); root struct {cell count? 0x100/0x80/0x80/0x100/0x20, pointer, block base, 0}. Not decoded.
- Height check (collision vertex y vs topmost visual surface at the same x/z): median 0.0 on all five,
  within 2 units: 86 % / 71 % / 55 % / 77 % / 89 %; the rest are walls and ground under bridges/roofs
  where the topmost visual surface is something else.

## Other course data

- **Centre line** (landscape +24 -> table 0C1695E8 {RAM ptr, count}): (x, z) float pairs, closed loop,
  even steps (32.5 / 26.1 / 20.2 / 33.6 / 7.8 units), first point (0, 0). Its extent matches the
  collision extent, every point lies over collision ground, and it runs down the middle of the road in the
  overlays. Index map: course 0 -> table 0, 1 -> 2, 2 -> 4, 3 -> 1, 4 -> 3. Whether point 0 is the start
  line: likely (it is the origin), not proven.
- **LENS** (table 0C2F7A90, up to 4 lists per course) and **STAT** (f_0C142D98; courses 0, 1, 3 only):
  {name, count, array, 4 floats bbox (x min, x max, z min, z max), ...} with 68-byte records = plane
  (nx, ny, nz, d) + 4 corners + 1 word. Seen in `preview\stat_lens_top.png` (courses 0, 1): LENS quads wrap
  rocks, buildings and walls beside the track (guess: camera "lens" collision); STAT quads sit on small
  posts/pillars on the track (guess: static obstacle collision). Malastare has two LENS lists; course 4
  reuses Malastare's.
- **Sky**: landscape +20, a one-node object with a radius-52 000 dome (course 0) / similar, textured.
- Not found yet: start grid positions, lights, fog colour. Malastare is the only course with lightset
  recalls (043) in its geometry. The per-course callbacks (+40..+48) are the likely place for fog/light
  set-up.

## Open questions

1. Meaning of collision surface codes and of w47; the spatial index layout.
2. Start grid: not in the tables read so far; check the callbacks and the race-setup code (f_0C14ACEC).
3. Fog/light/colour values: landscape +28/+32/+36 and the callbacks; lightsets used by Malastare.
4. Handedness: exported as stored (y up). Mirror-sensitive texture text was not checked against the
   cabinet; a left-right mirror is still possible.
5. Poly types 4 and 6 and the 881 alpha-mode bit: previews use a plain alpha test; blending (additive
   glows on Malastare) is not reproduced.
6. Course 4: Naboo-style oval, loaded only by the Kaadu code; not a selectable race.
