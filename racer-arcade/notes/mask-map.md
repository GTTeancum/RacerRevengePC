# img\mask.bin region map (Star Wars Racer Arcade, Hikaru mask ROM, 256 MB)

Working from the bytes, cross-checked against pointer tables in `img\eprom.bin`. All offsets are
`mask.bin` byte offsets. The program addresses mask data as `0xA0000000 + offset` (SH-4 P2 alias of
bus `0x20000000 + offset`, see `notes\hikaru-hardware.md` 1.5), so every `0xAxxxxxxx` word below is a
mask pointer. Every region except the motion bank at 0x0C800000 is fully tiled by a program table
(start, size) with no gaps, so the boundaries below are exact, not guessed.

Confidence: **high** = boundaries from program tables + content looked at; **medium** = kind clear,
internal layout partly decoded; **low** = guess, flagged.

## Region table

| Offset | End | Size | Kind | Evidence | Conf. |
|---|---|---|---|---|---|
| 00000000 | 02BD690A | 45.8 MB | **3D textures**, 2143 mip-mapped 16-bit textures | Program texture table at eprom 0x159B38: 2143 records of 6 words {ptr, log16 w, log16 h, bytes, 000100C1, TEX 2C1 word}. Sorted, they tile 0..02BD690A with no gaps (32-byte alignment). Sizes = 2*w*h*4/3 (full mip chain). All 2143 rendered and looked at (`textures_*.png`). | high |
| 02BD690A | 02C00000 | | zero pad | | |
| 02C00000 | 03E73652 | 18.4 MB | **3D models: Hikaru GPU (Antarctic CP) display lists** + small per-model tables | Program model directory eprom 0x16DB50..~0x18xxxx: 6723 (ptr, size[, count]) pairs tile the region with no gaps. ~447k static vertex pushes (op 0x12C-0x12F), 9820 `0x303` words. Point cloud of the first list (02C00320, 0x9BC4 bytes, 1650 verts) shows two radial engine faces, i.e. a pod (`model_gpu_pointcloud.png`). | high (kind) |
| 03E73652 | 04000000 | | zero pad | | |
| 04000000 | 04E58000 | 14.3 MB | **2D images** (UI, fonts, HUD, course-select, attract, logos): linear 16-bit, any w x h, no mips | Records in eprom of 7 words {ptr, w, h, x, y, kind, ?} (e.g. course screens table eprom 0x298FE4) plus plain pointer lists (e.g. eprom 0x3D7D30, HUD frames every 0x8000). 137 records cover 6.0 of 14.3 MB; the rest is reached through pointer lists. Rendered: 4 course-select screens 640x480, LucasArts, "Now waiting for a new entry", GAME OVER, speedo frames. | high (kind), medium (coverage) |
| 04E58000 | 05000000 | | zero pad | | |
| 05000000 | 0AEE15EC | 94 MB | **Audio**: 21 SPSD streams (music/speech) + 14 DTPK sound banks | Program table eprom 0x159A10: 21 (ptr, bytes) SPSD entries; eprom 0x159AC0: 14 (ptr, bytes) DTPK entries. Together they tile every audio bank exactly (only zero padding between). See audio list below. | high |
| 0AEE15EC | 0B000000 | | zero pad | | |
| 0B000000 | 0C18B2C8 | 17.5 MB | **Motion / skeletal animation, pointer-linked** | Contains absolute 0xAB../0xAC.. pointers (900 internal). Clip header {nBones, nFrames, ptr bone-list, ptr, ptr, bone ids...}, e.g. 0B00C694 = 5 bones x 35 frames, 0BD3658C = 54 bones x 480 frames. Body = 9-float records (scale xyz, rotation xyz in degrees, translation xyz); word period 9 dominates the whole bank. Referenced by eprom table 0x230A80: {RAM object ptr 0x0C24xxxx, mask ptr, id}. | medium |
| 0C18B2C8 | 0C800000 | | zero pad | | |
| 0C800000 | 0D78D2B8 | 15.6 MB | **Motion / animation, no pointers** | Zero pointers inside. 9-float period again (scale 0.135..., rot 90, ...). 29 clip headers found by pattern {0, nFrames, nBones, n} e.g. 0C800000 = {0,100,20,100}, 0D3B3920 = {0,90,87,143}; clip length does not follow from the header alone yet. No eprom table found with 0xAC8.. pointers (loaded some other way). | medium (kind), low (layout) |
| 0D78D2B8 | 0E000000 | | zero pad | | |
| 0E000000 | 0E78A6C0 | 7.5 MB | **Course collision**, 5 blocks of 196-byte triangle records | Program table eprom 0x3D4ECC (5 x {start, footer ptr, bytes}) next to the "loading collision" string. Record = 49 words: centroid xyz, radius, v0/v1/v2 xyz, vertex normals, edge/plane data, ids. Each block ends with a 0x20-byte footer {ptr, count, ptr, block base}; pointers are absolute 0xAE... Top views show track ribbons (`collision_tops.png`). | high |
| 0E78A6C0 | 0F2166C0 | 10.6 MB | **12 recorded runs** on one course: 12 x 7200 frames x 128 bytes | One pointer pair in eprom 0x123D78 (AE78A6C0, AF2166C0). Each 128-byte frame holds a triangle (nose, rear-left, rear-right; per vertex 3 packed words + xyz float) = pod footprint pose. 7200 frames = 120 s at 60 fps. All 12 trace the same closed course, the one in collision block A (bboxes match to the unit). Likely attract-mode/AI demo runs (guess). | high (layout), low (purpose) |
| 0F2166C0 | 0F2167B6 | 0xF8 | runs directory | {12, then 12 x {id, 7200, 7199, 0xA48, RAM dest 0x0C29EF88 + id*0xE1000}}: the runs are copied to RAM. | high |
| 0F2167B6 | 10000000 | 13.9 MB | **filler**: 16-bit 0x0CC4 repeated | uniform to end of file | high |

## Textures (00000000-02BD690A)

- Record (eprom 0x159B38 + 24*i, `notes\mask-textures.tsv`, from `tools\mask_textab.py`):
  `ptr, wlog, hlog, bytes, 0x000100C1 (TEX 0C1), TEX 2C1 word`. Width = 16<<wlog, height = 16<<hlog.
- Format = 2C1 bits 28:26 (matches hikaru-hardware.md 3.7): **0 = ABGR1555** (1556 textures),
  **1 = ABGR4444** (587, the ones whose word has 0x04000000 set). Verified by eye: with R in bits 4:0
  sand is tan and sky blue; with R in 14:10 Tatooine turns blue. The flagged textures are noise in
  1555 and clean pod parts/glows in 4444.
- Layout: level 0 first, linear rows, then the smaller levels (pyramid down to short side 1). Only
  level 0 has been rendered and checked.
- **Texel pairs are swapped**: the two 16-bit texels of every 32-bit word are stored right-then-left
  (IDMA source format, hikaru-hardware.md 3.7). Swap each pair before use. Checked by eye at full size on
  font texture 923: unswapped edges zig-zag one texel, swapped edges are clean. The 2D images at
  04000000+ are **not** swapped (the 64x64 digits at 04857A80 are clean unswapped, zig-zag swapped).
  `tools\mask_tex.py sheet` now swaps; the earlier thumbnails were too small to show it.
- Rough content order (from the contact sheets, `textures_pods_pilots_0-255.png` and
  `textures_courses_1920-2142.png`): 0-~1300 pod and pilot skins (eyes, skin, engines, cockpits,
  cables), fonts (A-Z/0-9 in two styles around 911-943 and 1073-1108), HUD/menu pieces; ~1300-1920
  more pods, Sebulba-style parts, menu banners, explosions/fire/smoke/glow sprites (~1792-1919);
  **1920-2142 course scenery**: Tatooine sand, canyon rock, Mos Espa walls, sky (1920-2063);
  red/white chevron barriers, metal walkway panels, grass, trees, water (2064-2142). Course scenery
  also appears earlier in small runs (e.g. 1504-1535, 1584-1615 Tatooine rock/buildings).
  Which texture belongs to which course or pod comes from the model directory (program side).
- Course identity, from the course-select screens at 04D70000/04C44000/04CDA000/04BAE000
  (`course_select_screens.png`): **Tatooine "Bantha Tracks"** (easy, deserts/cliffs/canyons),
  **Etti IV "Smuggler's Cove"** (normal, moss-covered plains and shallow seas), **Malastare
  "Pixelito Challenge"** (hard, wastelands, mountains, methane lakes), **Tatooine "The Boonta
  Classic"** (expert). The blue/teal water texture #2084 (02823AA0, 512x512) is either Etti IV sea
  or a Malastare methane lake (not decided).

## Audio (05000000-0AEE15EC)

SPSD header (0x40 bytes): "SPSD" 01 01 00 04, +08 codec (**1 = 8-bit PCM**, **3 = AICA 4-bit
ADPCM**), +0C data bytes, +2A rate 44100. PCM8 spectrogram shows clear music (drums, harmonics);
ADPCM decoded with the standard Yamaha/AICA step table shows harmonics over noise
(`audio_spectrograms.png`); 10-second WAVs in `notes\mask-audio\` for a listen.

| SPSD | codec | seconds | | SPSD | codec | seconds |
|---|---|---|---|---|---|---|
| 05000000 | ADPCM | 177 | | 07DDF280 | ADPCM | 30 |
| 053B6B40 | PCM8 | 111 | | 07E82E80 | ADPCM | 26 |
| 06000000 | ADPCM | 138 | | 07F0CE80 | ADPCM | 21 |
| 062E6100 | ADPCM | 123 | | 08000000 | PCM8 | 94 |
| 0657D480 | ADPCM | 48 | | 083EF500 | ADPCM | 263 |
| 06681340 | ADPCM | 36 | | 08977200 | ADPCM | 55 |
| 067449C0 | PCM8 | 103 | | 08AA0DC0 | ADPCM | 19 |
| 07000000 | ADPCM | 31 | | 08B07340 | ADPCM | 20 |
| 070A5040 | ADPCM | 33 | | 09000000 | ADPCM | 633 |
| 071546C0 | ADPCM | 42 | | 0A000000 | ADPCM | 545 |
| 07235040 | ADPCM | 555 | | | | |

DTPK sound-driver banks (header "DTPK", +04 id, +08 bytes): 0585D900 (id1, 0xEDC), 0585E7DC (0),
0591BBB8 (4), 05A05AF0 (7), 05BF70A0 (5), 05D9C4A8 (6), 06B95D00 (2), 06C4ADE8 (9), 06D4F44C (10),
06DB83FC (8), 06EC5088 (0), 08B74A40 (0), 09D502C0 (0), 0AB730C0 (4). Internals not parsed
(sequence + tone data for the Sega sound driver; the 8-bit sample ramps in the string dump at
05A5xxxx are their PCM waveforms).

## Collision (0E000000-0E78A6C0)

Program table eprom 0x3D4ECC, in table order:

| # | start | bytes | tris | x/z extent (1-99 %) | note |
|---|---|---|---|---|---|
| A | 0E000000 | 18EAA0 | 8331 | x -3045..115, z -1852..2990 | same course as the 12 recorded runs |
| B | (eprom) | 131080 | 6374 | x -2484..45, z -1361..2113 | **table points into EPROM**: 0x91480000 = bus 11480000 = eprom.bin 0xC80000. The mask copy at 0E4A3F60 starts identically but differs in ~680k bytes (internal pointers relocated plus other changes): the live data is the EPROM copy, the mask copy is older or unused. |
| C | 0E620120 | 16A5A0 | 7572 | x -2233..398, z -1240..1327 | |
| D | 0E18EAA0 | 3154C0 | 16494 | x -7577..350, z -1459..4392 | largest course |
| E | 0E5D4FE0 | 4B140 | 1568 | x -186..19, z -198..150 | small oval about 350 units: not a race course (garage / select stage? guess) |

Course names for A-D are not assigned yet. The strings next to the table are SA_COURSE2_STAT,
SA_COURSE4_STAT, S_ETTI4_STAT (plus MALASTARE elsewhere); the program helper should resolve the
index mapping.

## Tools (all in `tools\`, run with `python -I`)

- `mask_scan.py` per-64KB stats (entropy, zero, float-likeness) -> `notes\mask-scan64k.tsv`.
- `mask_textab.py` extracts the texture table from eprom.bin -> `notes\mask-textures.tsv`.
- `mask_tex.py` texture/2D decode: `render <off> <w> <h> <fmt> <out.png>` (fmt 1555bgr = ABGR1555,
  4444bgr = ABGR4444 over checker), `sheet <tsv> <first> <n> auto <out>` (contact sheet, format from the
  2C1 word), `bank <n> <fmt> <out>` (1 MB-per-column overview of a 16 MB bank).
- `mask_ptrtabs.py <lo> <hi>` finds pointer tables into a mask range in eprom.bin.
- `mask_gpu.py <off> <size> <out>` point cloud of static vertex pushes (uses the 901 precision and
  int32>>10 positions from `tools\hikaru_gpu.py`).
- `mask_coll.py`, `mask_paths.py` top views of collision blocks and the 12 recorded runs.
- `mask_audio.py list|wav|spec` SPSD listing, PCM8/ADPCM decode to WAV, spectrogram.

Images: `notes\mask-images\` (`contact_sheet.png` = one tile per region kind).

## Open questions

1. Which collision block is which course (A-D), and what block E is. A = the recorded-runs course.
2. Texture-to-course and texture-to-pod/pilot grouping: needs the model directory's per-model
   tables (the 0x38 + 0x30*(n-1)-byte lists next to each display list look like material/texture
   bindings; not decoded).
3. Model directory record layout (eprom 0x16DB50..): mixes (ptr,size) and (ptr,size,count) entries;
   per-model boundaries and names not yet tied to DefineDispPod / LoadStaticModel.
4. Motion banks: exact clip layout of 0C800000 (no pointers) and the GMOT_* name -> clip mapping.
5. 2D image records: field 6 takes values 1,2,3,4,5,6,8,10. Only "4" (ABGR4444 digits at 04857A80)
   is verified; 1, 2, 8, 10 images decode correctly as ABGR1555 with R in bits 4:0. hikaru-hardware.md
   says 2D layers swap R/B relative to textures; these images use the same order as textures, so
   either they are uploaded as textures or the swap is elsewhere.
6. Audio: ADPCM decode needs a listen (nibble order and possible stereo interleave unconfirmed);
   PCM8 signedness unconfirmed; DTPK internals unparsed.
7. Recorded runs: meaning of the 3 packed words per vertex and of 0xA48 (2632) in the directory;
   whether these are attract demos or AI racing lines.
8. The texture records whose 2C1 word has bit 14 set (24 of them, "UVs in whole texels") are
   otherwise normal ABGR1555/4444; no special handling needed for decoding.
