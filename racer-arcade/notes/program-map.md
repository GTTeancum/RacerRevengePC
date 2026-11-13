# Racer Arcade EPROM program map (img\eprom.bin, SH-4 LE)

Nov 12 2026. Tools: `tools\sh4dis.py` (disassembler, `--selftest`), `tools\sh4an.py` (xrefs, call
graph, recursive-descent function listing; cache `analysis\xref.pkl`), `tools\idtables.py`
(finds the asset-id getter functions), `tools\asset_ptrs.py` (lists ROM-address tables in data),
`tools\dump_assets.py` (writes `notes\asset-tables-{base,courses,pods}.txt`), `tools\findbase.py`.
Hardware background: `notes\hikaru-hardware.md` (another helper); everything below agrees with it.

Usage examples (run from `tools\`):
`python -I sh4an.py ..\img\eprom.bin func 0C142DDA --exact`,
`python -I sh4an.py ..\img\eprom.bin str "InitLandscape"`, `... callers 0C14D3C0`, `... refs A400001A`.

## 1. Load address and image layout

**CPU address = file offset + 0x0C00FC00** (file 0x400 -> 0x0C010000). Evidence:
- `findbase.py`: base 0x0C00FC00 makes 558 known string offsets appear as 32-bit literals;
  next best candidate 152.
- Header 0x390..0x3A3: `00 04 00 10 | 00 00 01 0C | 00 00 FF 01 | FF FF FF FF | 00 00 01 0C` =
  0x10000400 (ROM-bus address of file 0x400: EPROM bank 0x10), 0x0C010000 (destination),
  0x01FF0000 (= 0x0E000000 - 0x0C010000, i.e. up to end of 32 MB RAM; meaning as a size is a
  guess), -1, 0x0C010000 (entry). Header 0x130: `D0 07 06 01` = 2000-06-01 (guess: date).
- Entry 0x0C010000 / 0x0C010010 (master r14=1 / slave r14=0): `mov #-96,r1; shll16; shll8`
  -> r1=0xA0000000, `mov.l @(0x0C010100),r15` = 0x0E000000 (stack top), then jmp to
  0x0C010030|0xA0000000 (runs uncached). GBR loaded from 0x0C010108 = 0x0C016D40.
- BSS clear in the stub at 0x0C010114: 0x0C400000, 0x2C4C29 words (to 0x0CF130A4), and
  0x0CF130C0, 0x420 words. So the RAM image is file 0x400..0x3F0400 (CPU 0x0C010000..0x0C400000).

| File offset | CPU address | Contents |
|---|---|---|
| 0x000-0x3FF | (not loaded) | SAMURAI header, titles, ROM checksum table at 0x13C (see hikaru-hardware.md) |
| 0x400-0x7124 | 0C010000-0C016D24 | boot/runtime stub: init, MEMCTL, DMAC memcpy, malloc, RTC |
| 0x7124-0xF03FF | 0C016D24-0C0FFFFF | zero |
| 0xF0400-~0x150400 | 0C100000-~0C160000 | main game code (Hitachi-style prologues `2Fx6 ... 4F22`) |
| ~0x150400-0x3E7593 | 0C160000-0C3F7193 | data: asset tables, RAM-resident course data (STAT/LENS), strings |
| 0x3F0400 | 0C400000.. | BSS (cleared, not in file) to 0C F130A4; game state e.g. 0CEBEFCC landscape, 0C412F40 pilots |
| 0x400000-0x7D53A9 | n/a (ROM) | EPROM-resident assets, read through the ROM bus as 0x904xxxxx.. (bank 0x10) |
| 0x800000-0xFFFFFF | n/a (ROM) | EPROM bank 0x11 assets, bus 0x91xxxxxx (e.g. course 1 collision 0x91480000) |

## 2. Memory map as used by the code

| Address (as used) | Use, evidence |
|---|---|
| 0x0C000000-0x0DFFFFFF | 32 MB RAM; stack top 0x0E000000 (0x0C010004) |
| 0xA4000000 | MEMCTL (315-6154) regs, P2. 0xA4000010 aperture-0 page bytes, **0xA4000018+n = page byte of CPU window 0x0n000000**: 0xA400001A (window 0x02xxxxxx), 0xA400001B (0x03xxxxxx). Written by `f_0C011820(n, bus_addr)`: `mov.b (bus>>24),@(0xA4000018+n)`, returns 0xA0000000+(n<<24)+(bus&0xFFFFFF) |
| 0xA2000000-0xA2FFFFFF | CPU window 2 (uncached) = 16 MB page of the ROM board / external bus. All asset reads go here (`f_0C14D3C0`) |
| 0xA3000000-.. | window 3 (A3010000.. constants; sound/AICA etc., not traced) |
| 0xFFA00020..2C | SH-4 on-chip DMAC channel 2 (SAR/DAR/TCR/CHCR), used by memcpy `f_0C012F40` |
| 0xB4000000, 0xB5000000 | CMDRAM / Antarctic GPU regs (P2 of 0x14/0x15) |
| 0xBA000000 | Eurasia display regs (P2 of 0x1A) |
| 0xA0400000 | boot port (read/written at 0x0C010088) |

**ROM-board bus addresses inside the asset tables** (bit 31 set, as used for MEMCTL DMA / windows):
- `0xA0000000 + off` = mask ROM, **mask.bin offset = addr & 0x0FFFFFFF** (bus 0x20000000+off;
  16 MB page = one chip pair). Verified: collision root at mask 0x0E18EA90 holds pointers
  0xAE18BE90/0xAE000000 into its own block; motion headers in pair 11 point to their own
  contiguous frame/bone/key arrays; pair 11 contains only self-pointing 0xABxxxxxx words.
- `0x90000000 + off` = EPROM bank 0x10 = eprom.bin offset off (first 8 MB, chip pair ic29/ic30);
  `0x91000000 + off` = bank 0x11 = eprom.bin offset 0x800000+off (pair ic31/ic32). Verified:
  course-1 collision block 0x91480000 size 0x131080 has its root at 0x915B105C whose third word
  is 0x91480000 (eprom.bin 0xDB105C).

## 3. Loading path (no decompression)

`f_0C14D3C0(dst, src, size, wait)` is the single ROM reader (20 callers): if
`src & 0xFE000000 == 0x0C000000` it is RAM -> DMAC memcpy; else `src &= 0x7FFFFFFF` and it loops in
chunks that do not cross a 16 MB page: `f_0C011820(2, src)` sets window-2 page = src>>24, then
`f_0C012F40(dst, 0xA2000000 + (src & 0xFFFFFF), chunk, wait)` (SH-4 DMAC ch2). `f_0C14D458` is the
same loop with `f_0C011720` (32-byte block copy with movca/ocbp, to destinations at 0x0A000000+,
i.e. a different window; not traced further).

**No decompression routine exists in the load path**: data are copied raw and then pointer-fixed
(e.g. collision `f_0C142D04` adds `dst - src` to the root and per-entry pointers at +4, +8, +40).
No compression-related strings. Model geometry blocks are raw Hikaru GPU command words (e.g.
mask 0x02C00000: `00001083 000510C3 FF000303 ... FF00E12C`), textures raw 16-bit texels with
GPU texhead words in the table. (Some reads may go through the 315-5881 decryption window; nothing
in the load path selects one. The tables' addresses all decode as plain mask offsets.)

## 4. Asset id convention and tables

Every asset id is an index; **bit 30 (0x40000000) selects the EPROM-resident table** (addresses
0x90/0x91..), otherwise the mask-ROM table. Getter pairs ("valid?" + "record") per kind:

| Kind | Mask table, count x size | EPROM table, count | Getter / valid | Record layout |
|---|---|---|---|---|
| model | 0C17D750, 5240 x 20 | 0C244FC0, 745 | f_0C148E90 / f_0C148E64 | geom addr, geom size, attr addr, attr size, n_attr (LoadStaticModel f_0C148FE2 -> f_0C14903A) |
| object | 0C1970B0, 1529 x 16 | 0C2489F4, 130 | f_0C148EE6 / f_0C148EBA | RAM ptr to nodes (104 B each: +0 model id, +40 4x4 matrix), node count, root model id, flags (LoadStaticObject f_0C14919C, entry stubs f_0C14930E/14/1A/20) |
| texture | 0C169738, 2143 x 24 | 0C2432F8, 190 | f_0C14C7FC / f_0C14C7D0 | addr, log2(w/16), log2(h/16), byte size (16-bit + mips), texhead word 1, texhead word 2 |
| texgroup | 0C17CFA4, 97 x 20 | 0C244F08, 8 | f_0C14C856 / f_0C14C82A | 0, list ptr (12 B: texture id, x, y), count, 0, 0 ("CACHE TEXTURE GROUP") |
| texset (VRAM layout) | 0C17CE64, 16 x 20 | 0C244EE0, 2 | f_0C14C8C8 / f_0C14C880 | flag, list A (fixed textures, 12 B), count A, list B (16 B regions x,y,w,h, indexed by slot), count B |
| motion | 0C240678, 300 x 12 | 0C25C0BC, 3 | f_0C148AF4 / f_0C148AC8 | ROM addr of 20-B header {F, N, ptr F*4, ptr N*36, ptr F*N*36}, id/flags, name ptr ("GMOT_...") (loader f_0C148D00) |
| stream (music) | 0C169610, 21 x 8 | 0C25C13C, 1 | f_0C14C46C ("Stream ID") | addr, size |
| pack (sound) | 0C1696C0, 14 x 8 | 0C25C14C, 1 | f_0C14C4A8 ("Pack ID") -> f_0C1515EC (to sound board via window 2) | addr, size |

Full decoded dumps: `notes\asset-tables-base.txt` (every record, ROM offsets, object->model lists,
texgroup/texset->texture lists).

## 5. Courses

Course load = `f_0C142DDA(course)` (course 0..4). Course table **0C3E4B30**, 5 x 12 B
{landscape id, collision id, stat id} = identity (0,0,0)..(4,4,4).
- `InitLandscape f_0C147F8A(id)`: table **0C3E7FF8**, 5 x 52 B: +0 texset id (`f_0C14CE7C`),
  +4..+16 four landscape object ids (-1 = none), +20 one more object, +24 index into
  0C1695E8 (5 x {RAM ptr, count}; purpose unknown, guess: course path/spline), +28/+32 floats
  (guess: fog/draw distances), +36 int, +40/+44/+48 per-course callbacks (called with 1).
- collision `f_0C142D04(id)`: table **0C3E4ACC**, 5 x 12 B {block addr, root addr, size}.
- STAT `f_0C142D98(id)`: course 0 -> 0C3C2B64 "SA_COURSE2_STAT", 1 -> 0C3E4AA8 "S_ETTI4_STAT",
  3 -> 0C3E0924 "SA_COURSE4_STAT", 2/4 -> none. RAM structs {name, count, array ptr, floats...}
  (e.g. SA_COURSE2_STAT: count 0x7FC, array 0C39BD38).
- LENS table **0C2F7A90**, 5 x 16 B (up to 4 struct ptrs): 0 SA_COURSE2_LENS, 1 ETTI4_LENS,
  2 MALASTARE_LENS + MALASTARE_LENS2, 3 SA_COURSE4_LENS, 4 = same as 2. Read by f_0C135D8C.
  RAM structs {name, count, ptr, floats} (guess: camera "lens" data).
- Race setup `f_0C14ACEC(sel, ...)`: table **0C3E8AC8** {course, music stream} for sel 0..3:
  (0, 20), (1, 6), (2, 19), (3, 1). Course 4 is loaded only from f_0C1370EC / f_0C138470 (the
  Kaadu code: "KAADU MOT", "DEF KAADU ID"; guess: Naboo/Kaadu attract or bonus scene).
  f_0C100F94 loads course 2 directly (guess: demo/attract).

Decoded (`notes\asset-tables-courses.txt` has every block):

| Course | Name (from STAT/LENS) | texset | landscape objects | collision block | music |
|---|---|---|---|---|---|
| 0 | SA_COURSE2 | 11 | 1238, 1239 | mask 0x0E000000 size 0x18EAA0 | stream 20 mask 0x0A000000 |
| 1 | ETTI4 | E0 (EPROM) | E120, E121 | eprom 0xC80000 size 0x131080 | stream 6 mask 0x067449C0 |
| 2 | MALASTARE | 15 | E119, 1274 | mask 0x0E620120 size 0x16A5A0 | stream 19 mask 0x09000000 |
| 3 | SA_COURSE4 | 12 | 1253, 1255 | mask 0x0E18EAA0 size 0x3154C0 | stream 1 mask 0x053B6B40 |
| 4 | (Kaadu scene, guess) | E1 (EPROM) | E122, E123 | mask 0x0E5D4FE0 size 0x4B140 | n/a |

"SA" in SA_COURSE2/4 is presumably Tatooine (sand); not confirmed. Course 1 and 4 landscape data
and course-1 collision are EPROM-resident (bank 0x11).

## 6. Pods and pilots

18 kinds (0..17), same index for pod and pilot (`DefineDispPod` passes its kind to `DefinePilot`).
Pilot code from the motion names: 0 ALD, 1 ANA, 2 BEN, 3 EBE, 4 GAS, 5 MAR, 6 MAW, 7 RAT, 8 SEB,
9 TEE, 10 ODY, 11 ARK, 12 BOL, 13 CLE, 14 DUD, 15 ELA, 16 NEV, 17 WAN. 12 race slots (ids 0..11).
- `DefineDispPod f_0C1439EE(slot, kind, val 0..3)`: pod table **0C3F6790**, 18 x 0x88 B:
  +0 texgroup[4] (slot 0, chosen by val), +16 texgroup[4] (other slots), +32 engine count,
  +36 4 parts x 24 B {objA, objB, engine obj[4]} (loader f_0C14381E), +132 copied to pod
  state+196. Texgroups are loaded with `f_0C14CED6(group, texset 10, slot)`.
- `DefinePilot f_0C133134(slot, kind, flag)`: flag==0 table **0C2A733C**, flag!=0 table
  **0C2A6C7C**, 18 x 96 B: +0 texgroup, +4 object, +8 motion id[22] (-1 = none). The two tables
  differ only for some kinds (e.g. kind 1 texgroup 29/obj 340 vs 28/339); meaning of flag unknown.
- `DefinePodName f_0C143C40(kind, c, c, c)` stores the 3-letter code at state+0xA50.

Decoded per kind: `notes\asset-tables-pods.txt` (objects -> models -> geometry/attribute blocks,
texgroups -> textures, motions -> header/frame/bone/key blocks).

## 7. Key functions

| Address | Purpose |
|---|---|
| 0C010000 / 0C010010 | entry master / slave |
| 0C0116A0 / 0C011380 | malloc(size) / free(ptr) |
| 0C011820 | set CPU window n (0xA4000018+n) to ROM-bus page, return window address |
| 0C012F40 | memcpy via SH-4 DMAC ch2 (cache write-back/invalidate around it) |
| 0C011E00 | read RTC through window 2 (page 0x0C, 0xA2710000) |
| 0C14D3C0 | read ROM-bus / RAM range into RAM (all asset loads) |
| 0C14643C / 0C146434 / 0C146438 | debug printf / "invalid id" / "load failed" reporters |
| 0C142DDA | load course (landscape, collision, STAT) |
| 0C142D04 | load + relocate collision block |
| 0C142D98 | STAT struct for course |
| 0C147F8A | InitLandscape |
| 0C14CE7C / 0C14CED6 | load texset / load texgroup into VRAM slot |
| 0C14CD8A, 0C14CD40 | upload textures of a set, build texhead |
| 0C148FE2 / 0C14903A | LoadStaticModel / LoadStaticModelA |
| 0C14919C (stubs 0C14930E-0C149320) | LoadStaticObject(A) |
| 0C148D00 | load motion |
| 0C14C46C / 0C14C4A8 | stream record / load sound pack |
| 0C1515EC | send pack to sound board |
| 0C1439EE / 0C14381E | DefineDispPod / pod part loader |
| 0C133134 | DefinePilot |
| 0C143C40 | DefinePodName |
| 0C14ACEC | race setup (course + music from selection) |
| 0C14B6A4 | caller of race setup (selection at state+16) |
| 0C113FE0, 0C1142B0 | sel_course::ExecViewportEvent (course select screen) |
| 0C14F1B0 | bookkeeping screen (POD SELECTED / COURSE SELECTED counters) |
| 0C135D8C | reads the LENS table |
| 0C1370EC, 0C138470, 0C136844 | Kaadu scene (loads course 4) |

## 8. Unresolved

- Course display names (the bookkeeping screen uses textures, not strings); SA_COURSE2/4 = which
  Tatooine courses is not established.
- Purpose of landscape +24 array table (0C1695E8), +28/+32 floats, the three callbacks; full
  layout of STAT and LENS structs; DefinePilot flag; pod +132 field.
- Model geometry/attribute format beyond "GPU command words" (see hikaru-hardware.md / mesh work).
- Course textures used by models come through the model attribute blocks relative to the texset
  loaded by the course; only texset list A (fixed textures) is listed per course here.
- Whether any read goes through the 315-5881 decryption: not seen in the load path.
- Header word 0x01FF0000 at file 0x398 is interpreted only by guess.
- sh4dis prints FPU moves as frN regardless of FPSCR.SZ (pair moves under SZ=1 are drN/xdN).
