# Sega Hikaru hardware facts for decoding Star Wars Racer Arcade (swracer)

Nov 12 2026. Sources read (text only, saved under `ref\` in this folder):

- Odin, github.com/Tovarichtch/Odin @ 6ebf2b40 (2026-11-08), GPL-3, fork of Valkyrie.
  Local copy: `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\ref\odin-src\`.
  Key files: `src\odin\machine\boards\hikaru\{hikaru_bus.cpp, hikaru.cpp, hikaru.hpp, rom_board.cpp,
  rom_board.hpp, hikaru_gpu.hpp}`, `src\odin\machine\chips\{memctl.cpp, memctl.hpp, crypt_315_5881.hpp}`,
  `src\odin\machine\chips\antarctic\{antarctic.hpp, antarctic.cpp, antarctic_ops.cpp, antarctic_io.cpp,
  antarctic_mesh.cpp, antarctic_list.hpp, antarctic_list.cpp, antarctic_decode.hpp}`,
  `src\odin\host\renderer\hikaru\{renderer_textures.cpp, drawing.hpp, renderer_programs.*, gl\drawing_gl.cpp}`,
  `src\odin\host\app\games.cpp` (ROM interleave), `src\odin\machine\chips\aica\aica_voice.cpp`.
- MAME `src/mame/sega/hikaru.cpp` @ 774a180d (2026-08-02), local copy
  `D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\ref\mame\hikaru.cpp`.
  MAME's GPU emulation is a stub; it is only useful for the ROM layout and the bus maps.
- Valkyrie itself was not read separately: Odin keeps its comments (Stefano Teso) and extends them.

Odin's own word "HEURISTIC" marks guesses in its source; they are flagged here as (Odin heuristic).
Things verified against our images are marked **[verified]** with offsets.

---------------------------------------------------------------------------------------------------

## 1. Memory maps

### 1.1 Master SH-4 (hikaru_bus.cpp `Hikaru::master_read/master_write`, MAME `sh4_map_main`)

| SH-4 address (area) | What |
|---|---|
| 00000000-001FFFFF | Boot ROM (2 MB, `hikaru_0.96.bin` etc., MAME set `hikaru`) |
| 00400000-00400001 | unknown 16/32-bit port (MAME: "OKB_CS" PLD trigger) |
| 00800000-0083FFFF | MIE (315-6146 Z80): ports at 00800000+2n, Z80 memory at 00820000+2n |
| 00C00000-00C0FFFF | Backup SRAM 64 KB |
| 01000000-010001FF | MIE bus request |
| **02000000-02FFFFFF** | Aperture 02 -> external-bus page given by MEMCTL reg byte +0x1A |
| **03000000-03FFFFFF** | Aperture 03 -> page in MEMCTL byte +0x1B |
| 04000000-0400003F | Memory controller (315-6154) registers, rest of 04xxxxxx reads 0 |
| 0C000000-0DFFFFFF | Master RAM, 32 MB |
| 14000000-1400002F | Master/slave comm box |
| 14000030-143FFFFF | Command RAM (CMDRAM) 4 MB (bus 48000000); mirrors to 14FFFFFF |
| 15000000-150FFFFF | Command processor "Antarctic" registers (see 3.6) |
| **16000000-16FFFFFF** | Aperture 16 -> page in MEMCTL byte +0x12 |
| **17000000-17FFFFFF** | Aperture 17 -> page in MEMCTL byte +0x13 |
| 18001000-1800101F | Display chip PCI config (17C7:11DB) |
| 1A000000-1A0FFFFF | Display chip "Eurasia" (315-6087) registers |
| 1B000000-1B7FFFFF | Frame buffer RAM 8 MB |

swracer reaches all of these through SH-4 P2 (uncached, `A0000000 |` addr): the program holds
B5000000 (GPU regs, 83 refs), B5000058, B500008C, B43FC000 (IDMA table), A2000000/A3000000 (apertures),
AA000000 **[verified: eprom.bin dword constants]**.

### 1.2 Slave SH-4 (hikaru_bus.cpp `slave_read/slave_write`, MAME `sh4_map_slave`)

| SH-4 address | What |
|---|---|
| 00000000-001FFFFF | Boot ROM |
| 04000000-0400003F | its own memory controller |
| 0C000000-0DFFFFFF | Slave RAM 32 MB (bus 40000000-41FFFFFF) |
| 10000000-1000002F | comm box |
| 10000100-10FFFFFF | Command RAM (same 4 MB) |
| 14000800-1400083F | CP PCI config space (reads 17C311DB) |
| 1A800000 / 1B000100 | MIE ports / bus request |

MAME gives the slave apertures 14000000-17FFFFFF and 18000000-1BFFFFFF too (`aperture_r<0/1>`).

### 1.3 External bus (memctl.cpp header comment, hikaru_bus.cpp `bus_read`)

| Bus address | What |
|---|---|
| 04000000-043FFFFF | Texture RAM bank 0 (4 MB = 2048x1024 x 16 bit) |
| 06000000-063FFFFF | Texture RAM bank 1 |
| 0A000000-0A00FFFF | ROM-board local RAM 64 KB (the 315-5881 input), MAME "protram" |
| 0A010000-0A01000F | 315-5881 registers |
| 0C000000-0CFFFFFF | Sound board 1: AICA regs 0C700000, RTC 0C710000, wave RAM 0C800000-0CFFFFFF (8 MB) |
| 0D000000-0DFFFFFF | Sound board 2 (optional) |
| 0E000000-0E03FFFF | Network board (optional) |
| 10000000-3FFFFFFF | ROM board (EPROM, mask ROM, EEPROM) |
| 40000000-41FFFFFF | Slave RAM |
| 48000000-483FFFFF | Command RAM (4C000000 answers too) |
| 70000000-71FFFFFF | Master RAM |

Bit 31 of a bus address is ignored for these (the games use 9xxxxxxx for EPROM, Axxxxxxx for mask).

### 1.4 Memory controller 315-6154 registers (memctl.cpp, MAME `315-6154.cpp`)

| Reg | Bits | Meaning |
|---|---|---|
| +00 | ID | 0 master, FFFFFFFF slave |
| +04 | `---u---- -------- ---sEEEE EEFFFFFF` | DMA status: bit 12 = transfer done (IRL1 on master), E/F bus-error bits; write 1 to clear |
| +10..+13 | one byte per window | +12 = page for SH-4 16xxxxxx, +13 = page for 17xxxxxx |
| +18..+1B | | +1A = page for 02xxxxxx, +1B = page for 03xxxxxx |
| +1C | | page for 18xxxxxx (master) |
| +26 | bit 3 | ROM-board local bus 32 bits wide (Odin heuristic; swracer writes F4/F6 = 16-bit for the 315-5881) |
| +30 | `SSSS...S---- -` | DMA source bus address |
| +34 | | DMA destination bus address |
| +38 | `-------C LLLL...L` | bit 24 = start, L = length in 32-bit words |

"Page" = top byte of the external bus address; the window maps SH-4 `xx000000 + offs` to bus
`page<<24 | offs` (`MemCtl::window_bank`). DMA copies whole 32-bit words bus->bus at once
(`MemCtl::run_transfer`); destinations seen: slave RAM, CMDRAM, texture RAM, wave RAM.

### 1.5 ROM board (rom_board.cpp, MAME ROM_START(swracer))

swracer uses the type-2 board 837-13403 (MAME "ROM PCB Type 2"), 315-5881 stamped 317-0277-COM, key
2903DAD5. Odin config `rom_board: { eprom_mb: 4, "315-5881": "2903DAD5" }` (odin-games.json).

| Bus pages | Content | Our image |
|---|---|---|
| 10 | EPROM pair ic29+ic30 (2 x 4 MB = 8 MB; Odin mirrors it in the upper 8 MB of the page, MAME reads 0 there) | `img\eprom.bin` 0x000000-0x7FFFFF |
| 11 | EPROM pair ic31+ic32 | `img\eprom.bin` 0x800000-0xFFFFFF (= bus 11000000 + x) |
| 12, 13 | ic33-36, not fitted | - |
| 14 | X76F100 EEPROM (bit 0 SDA, 1 SCL, 2 /CS, 3 /RST) | - |
| 20-2F | mask ROM, 16 MB per page, pairs (ic37,ic38)...(ic67s,ic68s) | `img\mask.bin` offset = bus - 0x20000000 |

- **The "s" chips are not a separate bank.** ic53s..ic68s are simply pairs 9-16 = pages 28-2F
  (mask.bin 0x8000000-0xFFFFFFF). The "s" means solder side: MAME's board drawing says "these on other
  side of PCB". Both MAME (`ROM_LOAD32_WORD ... 0x8000000..0xf000002`) and Odin (`maskrom_bank 0x20..0x2F`)
  map them linearly.
- Layout table in the EPROM at +0x13C: 4 + 16 entries of 10 bytes {log2(size/MB), byte sum lo, word sum lo,
  byte sum hi, word sum hi}; FFFF = empty (rom_board.cpp comment, bootrom @0C004ED2). **[verified]**
  eprom.bin 0x13C: entry0 log 3 (8 MB, ic29/30), 0x146 entry1 log 3 (ic31/32), entries 2,3 FFFF,
  0x164.. 16 entries log 4 (16 MB each) = 256 MB mask.
- Interleave **[verified]**: Odin `games.cpp` "interleave amnt 2": file i goes at
  `base + (i&1)*2 + j*2` in 16-bit steps, pair after pair; MAME `ROM_LOAD32_WORD` even chip +0, odd chip +2.
  Our `tools\interleave.py` does the same (x = even chip at word 0, y = odd at word 1). Pair 2 of the
  EPROM lands at image offset 8 MB, which is bus 11000000 (MAME region offset 0x1000000).

### 1.6 315-5881 decryption: what goes through it

(crypt_315_5881.hpp header; rom_board.cpp `read_guard`; MAME hikaru.cpp `localbus_map_5881`, `prot_ram_r`.)

- The chip does **not** sit between the SH-4 and the mask ROM. Its input is the 64 KB ROM-board RAM at
  bus 0A000000: the game writes ciphertext there, then reads plaintext from 0A01000E.
- Registers (16-bit): +0 read busy (bit 0); +8 write address low (restarts stream); +A address high (taken
  as 0); +C write sequence key (subkey); +E read next plaintext word. First read after an address write
  is a dummy 0.
- Stream: substreams with an 18-bit header (1 compressed flag + 9-bit A + 8-bit B, length (A+1)*(B+1)
  bytes); compressed substreams are a run-length / previous-line code, A+1 bytes per line, B+1 lines
  (an image-like codec). Odin and MAME run it through the same code (BSD-3, Naive/Galibert/Haywood).
- **swracer**: the mask data is read plain (Odin `RomBoard::read` returns raw bytes for pages 20-2F, no
  crypt). Our mask.bin contains plain Antarctic display lists **[verified, section 3.8]**. The program has
  only 2 references to the 5881 register page (AA010000-ish constants at 0C3E21B6/0C3E21FA): whatever it
  decrypts is small and must pass through the 64 KB local RAM. Which data that is: **unresolved**.

---------------------------------------------------------------------------------------------------

## 2. Boot: how the program gets to RAM

- The boot ROM probes bus pages 10-1B for "SAMURAI" (base 0 if at 10000000, 8 if at 18000000) and reads
  the layout table (rom_board.cpp comment, bootrom @0C00B834, @0C004ED2).
- swracer header **[verified, eprom.bin]**:
  - 0x000 "SAMURAI", 0x010 "SEGA ENTERPRISES,LTD.(R&D#5)", 0x030-0x12F title per region
    (JPN USA EXP AUS KOR JAMMA LOCA DEBUG), 0x13C ROM table.
  - 0x390: `10000400` (source bus address = EPROM page 10 + 0x400), 0x394: `0C010000` (destination),
    0x398: `01FF0000` (length, bytes; 0C010000 + 01FF0000 = 0E000000 = end of master RAM),
    0x39C: FFFFFFFF, 0x3A0: `0C010000` (entry point). Field names are my reading; the values are facts.
  - 0x400: SH-4 code (`E1A0 4128 DF3E 4118 ...`), the entry.
- **Load mapping [verified]**: master RAM address = `0x0C00FC00 + eprom.bin offset` for offsets
  0x400-0x7FFFFF. Evidence: histogram of pointer-minus-string deltas peaks at 0C00FC00 (330 of 400 sampled
  strings); "SA_COURSE2_LENS" at eprom 0x2E7EE0 is referenced as 0C2F7AE0, "loading collision" 0x3D4F08 as
  0C3E4B08, *_STAT 0x3D4F6C as 0C3E4B6C (one pointer each).
- Odin's TODO says the copy of swracer/sgnascar reads past the 16 MB window into the bridge page
  (04000040-04FF03FF reads 0): i.e. the boot ROM copies through aperture 03 from 03000400 for 01FF0000
  bytes, so only bus page 10 (EPROM pair 1, plus its mirror/zero upper half) is copied; **EPROM pair 2
  (ic31/32, eprom.bin 0x800000+) is not part of the boot copy**. It holds data the game reads itself over
  the bus (it contains ~75 000 static-vertex headers, section 3.8).
- Odin `Hikaru::program()` comment says "as the BOOTROM copies it from 0C000000 on": that is the region,
  the actual image base is 0C00FC00 as above.
- The slave is released by the boot ROM through MIE port D (bit 2), runs an idle loop at 0C002030 and
  takes jobs from the comm box (hikaru.cpp comment near `set_resets`).

---------------------------------------------------------------------------------------------------

## 3. Graphics

### 3.1 Chips (hikaru_gpu.hpp)

- 315-6083 **Antarctic** = command processor (CP): executes display lists, owns the texture transfer (IDMA).
- 315-6087 **Eurasia** = display chip: CRTC, two 2D bitmap layers, blitter in FB RAM, at 1A000000.
- Rasteriser chips 315-6086 Africa, 2x 315-6197 Atlantis, 315-6085 America, 2x 315-6084 Australia
  (texture RAM). Odin renders on the host instead of emulating them.

### 3.2 Where the CP reads lists

- Only from **CMDRAM (bus 48xxxxxx/4Cxxxxxx)** or **slave RAM (bus 40xxxxxx-41xxxxxx)**
  (`Antarctic::fetch`, antarctic.cpp). Never from ROM. Anything in the mask ROM must be copied into one of
  those RAMs first (MEMCTL DMA or SH-4 copy).
- Start: 15000070 = start PC (bus address), 15000074/78 = two stack pointers (call stack lives in CMDRAM,
  bus 48xxxxxx), 15000058 = 3 launches; end interrupt 15000088 bit 2.
- Jump/call targets are word addresses (`A*4`), relative to the instruction when bit 11 is set
  (`jump_address`, antarctic_decode.hpp).

### 3.3 Instruction encoding

- Little-endian 32-bit words. Opcode = low 9 bits of word 0. Size = `1 << (((w>>4)&3)+2)` bytes
  (bits 5:4: 0 -> 4, 1 -> 8, 2 -> 16, 3 -> 32 bytes).
- Many opcodes have a "form" in bits 11:9 (e.g. 0C1/2C1/4C1 are opcode 0C1, forms 0/2/4). Tables below
  name them by the low 12 bits as the Odin source does.
- Bit layouts are MSB left, 32 bits: `bits 31..24 23..16 15..8 7..0`. `o` = opcode bits.

### 3.4 Opcode table (antarctic_ops.cpp `Antarctic::insns`)

| Op | Size | Name | Word 0 layout / extra words |
|---|---|---|---|
| 000 | 4 | nop | (zero words in lists are NOPs) |
| 012 | 8 | jump | `IIIIIIII IIIIIIII CCCCR--o oooooooo`, w1 = target in words. C: 0 always, 1 branch-id != I, 5 LOD size >= I, 9 size < I, 6/7 on 095 flag, D. R relative |
| 052 | 8 | call | `-------- -------- CCCCR--o oooooooo`, w1 target. C: 0, 4 (!cond), 8 (cond) |
| 082 | 4 | return | `CCCC` in bits 15:12 as call |
| 1C2 | 4 | end (kill) | |
| 088 | 4 | flush | last instruction of a frame list (ignored) |
| 005 | 4 | LOD test sphere | bits 31:12 = truncated float radius; visibility of sphere at the 561 centre |
| 055 | 8 | LOD radius | w1 float |
| 095 | 8 | LOD compare | `CCCC` form; w1 = H(31:16) L(15:0) sizes in 1/16 px |
| 021 | 16 | VP Z clip | w1 F (x proj. distance, px, float), w2 f (y), w3 near |
| 221 | 16 | VP XY clip/offset | w1 Y(31:16) X(15:0) signed offset; w2 B(29:16) L(14:0); w3 T R |
| 421 | 16 | VP depth range | w1 min, w2 max (floats), w3 bits 31:29 viewport number |
| 621 | 16 | VP depth queue (fog) | `---TTTDW` in 23:16; w1 ABGR fog colour; w2 P = 1/(end-start); w3 Q |
| 011 | 8 | VP ambient | r = w0 31:16; w1 b(31:16) g(15:0) |
| 191 | 8 | VP clear colour | w1 `-------a bbbbbbbb gggggggg rrrrrrrr` |
| 004 | 4 | VP commit | index bits 18:16 |
| 003 | 4 | VP recall | index 18:16, bit 13 push, bit 14 pop |
| 161 | 16 | matrix row | `-------- ----MPNN -EL----o`: NN column/row index 17:16, P(18) second modelview, L(13) local matrix, E(14) last row, M(19) unknown; w1-3 float X Y Z |
| 561 | 16 | LOD sphere centre | w1-3 floats |
| 961 / B61 | 16 | light vector (direction / position / spot axis) | T bit(s) select which |
| 091 | 8 | MAT diffuse | w1 `xxxxxxxx BBBBBBBB GGGGGGGG RRRRRRRR` |
| 291 | 8 | MAT colour 1 | w1 `-- B G R` |
| 491 | 8 | MAT specular | w1 `S B G R`, S = shininess |
| 691 | 8 | MAT ambient | R = w0 31:16; w1 B(31:16) G(15:0), 16 bit/channel, 255 = 1.0 |
| A91 | 8 | MAT blend mode (BRAVEFF only) | |
| 081 | 4 | MAT unknown | m 19:16, n bit 12 |
| 881 | 4 | MAT flags | `XXXXXXXX xyhatzSS`: SS(17:16) shading 0 unlit 1 Gouraud 2 flat; z(18) fog/depth blend; t(19) textured; a(20) alpha mode; h(21) highlight; x,y(23,22) unknown; 31:24 sign copies of x |
| A81 | 4 | MAT blend mode | bits 17:16 |
| C81 | 4 | MAT alpha-test index | bits 21:16 into the 154 table |
| 084 | 4 | MAT commit | index bits 25:16 (+ base) |
| 083 | 4 | MAT recall | index 31:16; bit 12 = recall (else set base) |
| 0C1 | 4 | TEX bias/filter | `----BBBB AAAA--MM`: MM filter 0 nearest, 1 trilinear, 2 bilinear; bias 27:20 signed |
| 2C1 | 4 | TEX format/size | `UUUFFFrr wwHHHWWW ba-----o`: W(18:16) log16 width, H(21:19) log16 height, w(22,23) repeat U,V, r(24,25) mirror U,V, F(28:26) format, U(31:29) texel/colour combine mode, a(14) UVs in whole texels (else 12.4) |
| 4C1 | 4 | TEX slot | `nnnnnnnn mmmmmmmm ---b---o`: Y slot 31:24, X slot 23:16, bank bit 12 |
| 0D1 | 8 | TEX panning | slot 17:16; w1 Y(31:16) X(15:0) in 1/16 texel |
| 0C4 | 4 | TEX commit | index 25:16 (+ base), bit 12 always 1 |
| 0C3 | 4 | TEX recall | index 31:16 (+ base); bit 12 = recall (else set base) |
| 061 | 16 | LIGHT attenuation | type 17:16 (0 linear, 1 square, 2 inv linear, 3 inv square); w1 P, w2 Q floats |
| 051 | 8 | LIGHT diffuse | A ambient 19:16; w1 10-bit B(29:20) G(19:10) R(9:0) |
| 451 | 8 | LIGHT spot cone | bit 24 = no cone; w1 C cos(outer) 1.15 (15:0), W(23:16) 8-bit float scale |
| 104 | 4 | LIGHT commit | index 25:16 |
| 064 | 16 | LIGHTSET commit | set 23:16; w1 b(25:16) a(9:0); w2 d c |
| 043 | 4 | LIGHTSET recall | D mask 27:24, n 23:16, A bit 12 |
| 006 / 046 | 4 | unknown (swracer sends them in hand-written lists; ignored) | |
| 101 | 4 | MESH depth bias | 11-bit F:B in 26:16 (swracer: none) |
| 301 | 4 | MESH unknown | |
| 501 | 4 | MESH pattern | 20:16: 4x4 screen dither pattern; swracer fades far scenery with it |
| 901 | 4 | MESH static precision | p = bits 23:16: unit = 2^(p-0x8D) (Odin note: swracer sky dome has 0x8E) |
| 103 / 113 | 4 / 8 | poly type | type bits 11:9 (1, 4, 5, 6 seen; 2/3 shadows), base alpha 31:24 |
| 12C-12F | 16 | push vertex, static | header + 3 packed words (3.5) |
| 1AC-1AF | 16 | push vertex, dynamic | header + float x y z |
| 1B8-1BF | 32 | push vertex, all | header, x y z floats, w4 = t(31:16) s(15:0) 12.4, w5-7 normal floats |
| 158-15B | 8 | push texcoords x1 | header (same bits) + w1 v(31:16) u(15:0) signed 12.4 |
| 0E8-0E9 | 16 | push texcoords x3 | texhead index (from base) 31:16; w1-3 three UVs |
| 154 | 8 | alpha threshold table | index 21:16; w1 H(31:8) L(7:0) |
| 194 | 8 | light table | |
| 181 | 4 | FB blending | E bit 24, factor 23:16 |
| 781 | 4 | FB combiner | |
| 1A1 (3A1/5A1) | 16 | FB buffer addresses (ignored) | |

Odin decodes nothing else; any other opcode stops its CP (`invalid instruction`).

### 3.5 Vertex encoding (antarctic_ops.cpp `op_push_*`, antarctic_mesh.cpp `push_vertices`)

Common header (12x, 1Ax, 1Bx, and 15x/0E8 use the same bit positions):

```
AAAAAAAA C---OO-- uPRSTTTo oooootpW
A  31:24 vertex alpha             C  23   no cull
OO 19:18 texcoord offset slot (0D1)  u 15  unknown (set on all but the last 3 of a list)
P  14 add modelview translation   R  13   apply modelview 3x3
S  12 use second modelview (161 bit 18)
TTT 11:9 0 = just push, 7 = close a triangle with the two previous vertices
t  2 texcoord pivot (?)           p  1    position pivot: keep vertex -2 (triangle fan)
W  0 winding: 0 -> (0,-1,-2), 1 -> (0,-2,-1)
```

- Strip/fan model: a 3-entry vertex window. A position push rotates the window (or, with p, keeps the
  oldest as fan pivot); a push with TTT=7 emits a triangle.
- **In swracer data the triangle bit is on the 158 texcoord push**, not the 12C: e.g. `FF00E12C` (vertex,
  T=0) followed by `00000F58` (texcoords, T=7). The 158 attaches its UV to the last vertex and closes the
  triangle.
- **Static vertex (12C-12F), 3 words after the header**, each `pppppppp pppppppp pppppp nn nnnnnnnn`:
  position component = `(int32(word) >> 10) * 2^(p-0x8D) / 64` (22-bit signed, p from the last 901);
  normal component = `int16((word & 0x3FF) << 6) / 16384` (10-bit signed, /256).
- Texcoords: signed 16-bit, 1/16 texel (12.4), unless 2C1 bit 14 says whole texels.

### 3.6 CP registers (antarctic_io.cpp)

| Reg | Meaning |
|---|---|
| 15000004 | display mode 0 = 640x480 31 kHz, 1 = 496x384 24 kHz |
| 1500000C | IDMA table bus address (default 483FC000 = master 143FC000; swracer constant B43FC000 **[verified]**) |
| 15000010 | IDMA number of entries |
| 15000014 | IDMA control: bit 0 start / busy |
| 15000018-54 | two banks of unknown config words (CMDRAM layout?) |
| 15000058 | CP control: 3 = start list |
| 15000070/74/78 | CP start PC, stack pointer 0, stack pointer 1 |
| 15000084 / 88 | IRQ mask / status: 01 IDMA done, 04 list read, 80 display chip |
| 1500008C | TEXRAM mode: 02020202 bus writes are twiddled by hardware (linear for software), 01010101 raw |
| 15002000 | status; bit 0 = main board version |
| 15002800-48 | primitive/texhead/material/light counters |

### 3.7 Textures

- **Texture RAM**: 2 banks x 4 MB, each a 2048 x 1024 picture of 16-bit texels (`Antarctic::TEXRAM_*`).
- **Slots**: 16x16-texel cells. Slot (x, y) with x >= 0x80, y >= 0xC0 maps to texel
  `((x-0x80)*16, (y-0xC0)*16)` (`odin_tex_coords`). So x 80..FF, y C0..FF cover the bank.
- **Sizes**: width/height = 16 << log (log 0..4 seen: 16..256; 5-7 unused?).
- **Formats** (2C1 bits 28:26 = IDMA entry +08 bits 28:26):

| F | Name | Texel layout (16-bit little-endian word) |
|---|---|---|
| 0 | ABGR1555 | A bit 15, B 14:10, G 9:5, R 4:0 (Odin GL: `GL_UNSIGNED_SHORT_1_5_5_5_REV`, no swizzle) |
| 1 | ABGR4444 | A 15:12, B 11:8, G 7:4, R 3:0 (`4_4_4_4_REV`) |
| 2 | I4 | one 16-bit word = 2x2 texels: top-left 15:12, top-right 11:8, bottom-left 7:4, bottom-right 3:0; value v -> all channels v*0x11 (Odin heuristic). A w x h-word texhead is 2w x 2h texels |
| 4 | LA8 | byte 0 luminance, byte 1 alpha (Odin heuristic) |
| 8* | A2BGR10 | 2D layers only (32-bit), not textures |

  2D layers (Eurasia) use ABGR1555 with R and B swapped relative to textures (Odin swizzles them).
- **Mipmaps**: a texture's pyramid goes down to the level where the short side is 1. Each next level is
  placed halfway to the right/bottom edge of the bank (`x += (2048-x)/2`, `y += (1024-y)/2`) **in the other
  bank** (`Antarctic::copy_texture`; Odin notes swracer keeps textures where level 1 of the same bank
  would land, which is why the bank alternation was adopted).
- **IDMA (texture transfer)**: table in CMDRAM (1500000C), 16-byte entries:

```
+00 AAAAAAAA AAAAAAAA AAAAAAAA AAAAAAAA   source bus address: slave RAM 40/41xxxxxx or CMDRAM 48xxxxxx only
+04 -------- -------- SSSSSSSS SSSSSSSS   bytes, including all mip levels (64x64 full pyramid = 0x2AAA)
+08 ---fff-- --hhhwww yyyyyyyy xxxxxxxx   format, log16 h, log16 w, slot y, slot x (= 2C1 / 4C1 fields)
+0C                            -------b   bank
```

  Source texels are linear, row-major, 2 bytes each, level after level; Odin stores source texel i at
  TEXRAM byte `(row*4096 + x*2) ^ 2`, i.e. **the two texels of each 32-bit source word are swapped**
  (`Antarctic::copy_level`). So in ROM/RAM the left texel of a pair is the high half of the LE dword.
- Odin's notes: IDMA cannot read ROM; the games first DMA texture data into slave RAM or CMDRAM with the
  MEMCTL, then run the IDMA. BRAVEFF instead writes raw (twiddled) texels straight from mask ROM into TEXRAM
  via MEMCTL DMA with 1500008C = 01010101. In that mode the bus offset (in texels) is a Morton/Z-order
  interleave: offset bit 2k = x bit k, bit 2k+1 = y bit k (x has 11 bits, y 10), and each texel is
  byte-swapped (hikaru_bus.cpp TWIDDLE table, `Hikaru::texram_store`).
- swracer program strings: "TEXTURE CACHE", "CACHE TEXTURE ID", "CACHE TEXTURE GROUP",
  "TEXTURE SPOOL EXHAUST" (eprom 0x3D9D5C-0x3D9DD8): it streams textures into TEXRAM slots at run time.

### 3.8 What swracer's ROM data looks like [verified]

- `img\mask.bin` holds **Antarctic display lists verbatim**, plain (not encrypted). 0xFF00E12C (static
  vertex, alpha FF, R+P+u) occurs 405 765 times. Example at mask.bin 0x02C00004 (bus 22C00004):

```
00001083  MAT recall n=0           000510C3  TEX recall n=5
FF000303  poly type 1, alpha FF    00000101  depth bias 0
00000501  pattern 0                00820901  static precision 0x82 -> 2^-11 units /64
FF00E12C 099CDF24 FBFFF056 F1943861   vertex pos=(1.2016 -0.5000 -1.8026) n=(-0.859 0.336 0.379)
00000158 00AF0137                     uv=(19.44 10.94)
...  29 vertices, 24 triangles, then 00000082 return
```

  Material/texhead indices are relative to bases the game sets at run time (083/0C3 without bit 12).
  Between meshes sit material blocks: `00000091 00D8D8D8 / 00000291 00D8D8D8 / 000D0881 / 00000A81 /
  00200C81 / 00000491 F2191919 / 00660691 00660066 / 000n1084` (diffuse, colour1, flags 0D = textured+fog+Gouraud,
  blend 0, alpha index 32, specular, ambient, commit material n) and zero (nop) padding.
- Distribution of static-vertex headers: mask.bin 0x02C00000-0x03EFFFFF (pages 22-23, ~19 MB) and
  eprom.bin 0x800000-0xAFFFFF (EPROM pair 2, bus 11000000+) plus a few in 0x2E9898+ (the hand-written
  lists in the program). 11 749 mesh starts (901 directly followed by 12x) in the mask, precisions 0x7A-0x8E.
  The rest of the mask (0-0x2BFFFFF and 0x3F00000-0xFFFFFFF) is mostly not CP code: textures and sound are
  the likely content (unverified).
- Odin's shader cache `odin\...\shaders\swracer.txt` is a list of host shader variant keys
  (`MeshVariant::key`, renderer_programs.cpp: bit 0 textured, 1 lit, 3:2 shading, 4 alpha mode, 7:5
  texmode, then 6 bits per light x4, fog, cue, collect, pattern, native, families). It is not game data;
  it only says which material features swracer uses (textured, Gouraud, up to 4 lights, fog, patterns).

### 3.9 Tool

`D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\tools\hikaru_gpu.py` disassembles a CP
stream from any file offset and decodes static vertices:

```
python -I tools\hikaru_gpu.py img\mask.bin --offset 2C00004 --stop-on-return [--quiet-verts]
python -I tools\hikaru_gpu.py img\mask.bin --scan --offset 2C00000 --end 2D00000
```

---------------------------------------------------------------------------------------------------

## 4. Sound (brief)

- Each sound board: AICA (315-6232) + ARM7 + 8 MB SDRAM wave memory at bus 0C800000-0CFFFFFF
  (board 2 at 0D...). Registers 0C700000 (64 channels x 0x80), DSP 0C703000+, RTC 0C710000
  (hikaru_bus.cpp "Sound Boards", MAME `aica0_map`, `localbus_map`).
- AICA sample formats (aica_voice.cpp, PCMS field in channel reg +0 bits 8:7): 0 PCM16 (LE), 1 PCM8 (signed),
  2 Yamaha 4-bit ADPCM, 3 ADPCM stream. Start address SA 23 bits (reg +0 bits 6:0 and +4), loop LSA (+8),
  end LEA (+C), in samples.
- The sound driver is AM2's: string "AM2/AICAsoundDrv000111/Ver1.78" at eprom.bin 0x146A5C (RAM 0C15665C).
  The program uploads it and the samples into wave RAM over the bus (MEMCTL DMA or SH-4 writes).
  Where the samples sit in the mask, and their bank/table format: **unresolved**.

---------------------------------------------------------------------------------------------------

## 5. Unresolved

1. Which data swracer passes through the 315-5881 (2 references in the program). Possibly small
   compressed tables; models are plain.
2. Exact boot-copy semantics of header fields 0x390-0x3A0 (values certain, meaning inferred) and whether the
   hardware mirrors EPROM pair 1 into bus 10800000 (Odin) or reads 0 (MAME). Irrelevant for decoding.
3. Where in the mask the textures are and whether they are stored as IDMA-ready linear 16-bit pyramids
   (most likely: IDMA source format) or twiddled TEXRAM images (BRAVEFF style). Find the code that builds
   IDMA entries (refs to B43FC000 at 0C011CA4) and the MEMCTL DMA source addresses.
4. How the game indexes models in the mask (no file names): the program must hold tables of mask bus
   addresses (A2xxxxxx/22xxxxxx etc.) and sizes; not searched yet.
5. Several CP fields are Odin guesses (I4/LA8 channel mapping, 881 x/y, 101 semantics, 006/046).
   They do not affect geometry extraction.
6. Sample format/bank layout of the AICA data.
