# Exact Pod01 cable texture 50FC

`tests/native/source_cable_texture50fc_fixture.h` provides the header-only cold decoder `source_cable_texture50fc_fixture::decodeCableTexture50FC(source, rrtex, error)` for the actual Pod01 `textures\cable.pst`. It has no extra build dependency and does not change the frozen TA converter or its outputs.

The source is 2,140 bytes, SHA256 `e6b0abd320de50c784816a4e15dc6bfa056c8241a173eb26481243ff84dc301f`. Its layout is PST revision 1/kind 0, one 64x64 mip, 16 raw RGBA palette entries at offsets 12–75, mip header at 76, and 2,048 packed index bytes at 92–2139.

The native output is `evidence/cableTexture50FC/cable.rrtex`, SHA256 `10329f55fd0edd413b71c74915b6694700beee922c36255c410099eccd903d64`. All 4,096 source RGBA texels are preserved exactly. The decoder expands the low nibble first, then the high nibble, retaining row order and directly selecting the 16-entry palette. There is no RGB/alpha scaling, row flip, substituted texture or generated pixel content.

## Source proof

- Original SLUS `11A518–11A598` allocates 16 four-byte palette entries and reads them raw.
- `24EDA4–24EDF0` reads width*height*4/8 source index bytes and skips the optional kind-1 fast-upload rearrangement.
- `250DEC–250E28` uploads those bytes as PSMT4, with the raw palette pointer.
- `257CA8–257CC8` chooses an 8x2 PSMCT32 palette upload for PSMT4. `257D1C–257D78` encodes its dimensions and sends its raw words.
- `242298–2422BC` binds PSMT4, CPSM32, CSM1, CSA0 and CLD1.
- The physical addresses of the raw 8x2 PSMCT32 palette upload equal the T32I4 CSM1 lookup table for all 16 entries, so their composition is the identity mapping.
- The independent GS host-to-PSMT4 transfer reference consumes a byte's low nibble at x and high nibble at x+1. Upload and sampling use the same GS addressing, leaving the original linear source texels unchanged.

Opcode bytes, range hashes, table composition and reference links are retained in `evidence/cableTexture50FC/manifest.json`. No guest code or emulator was run or linked.

## Checks

`tests/native/source_cable_texture50fc_check.cpp` extracts the member from the pinned original POD01.RES through native48C, compares all native decoded bytes to the independent Python exporter, checks every source texel, rejects all 2,140 proper prefixes and all 2,140 single-byte bit-flips, and rejects a suffix byte. All 4,281 rejection cases preserve the caller's prior output. The real source passes.

Reproduce from the project root:

    python -B tools/assets/decode_cable_texture50fc.py
    g++ -std=c++17 -O1 -Isrc/assets src/assets/import_decode48c.cpp tests/native/source_cable_texture50fc_check.cpp -lz -o /tmp/cable_texture50fc_check
    /tmp/cable_texture50fc_check

The decoder deliberately accepts only this authenticated cable resource. It does not claim general kind-0 coverage, original shader implementation, filtering, or cable geometry fidelity.
