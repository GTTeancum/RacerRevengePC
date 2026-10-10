# TA source textures 50FC

The eight previously unsupported TA resources now have genuine source-derived native textures: Track, Trackdark, Trackside, Trackwheel, cord, crawlerA, arrowred frame 0, and crowd frame 0. The client reads pinned RRTEX001 files; no codec, guest code, emulator, source texture substitution, or generated replacement runs in the frame loop.

## Integration

- `tests/native/source_texture50fc_fixture.h`: `source_texture50fc_fixture::decodeSourceTexture`, the 50FA source callback.
- `tests/native/source_texture50fc_index.inc`: pinned member names, input hashes, output hashes, dimensions and base-mip indices.
- `evidence/textures50FC/manifest.json`: every parsed frame extent, retained mip provenance and output hash.
- `evidence/textures50FC/*-frame0-mip*.rrtex`: 30 source mips. The eight selected base images occupy 720,896 RGBA bytes.
- `tools/assets/decode_source_textures50fc.py`: reproducible cold converter using installed FFmpeg's native IPU decoder.

Set the 50FA original archive to `work/native/project/disc/TRACKS/TA.RES`, callback to the function above, and explicitly enable `allowStaticAnimationFrame` to use real ATM frame 0. These ATM files contain five arrow frames and six crowd frames. Full frame boundaries are checked, but the current callback deliberately returns only the first stored frame and its largest authored mip. This does not implement animation timing.

The adapter checks both original member SHA256/extent and the pinned native output SHA256/extent/framing. Unknown names and changed source bytes fail without changing the caller's output or mip index. Existing 48E code was not modified.

## Original format and color evidence

Original executable: `SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`.
Original archive: `TA.RES`, SHA256 `b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8`.

Static original instruction inspection establishes:

- `24E994–24EA68`: kind 1 reads a 256-entry palette; kind 2 skips the palette and marks the compressed texture path. Kind 0, outside this converter's scope, is the 16-entry format. Kind 2 is not a 4-bit paletted texture.
- `24EB14–24EB3C`: each retained mip has a 16-byte header and dispatches into the kind-specific loader.
- `24F148–24F1C0`: kind 2 reads a byte count and that many bytes, then advances the compressed data pointer by the 16-byte IPUM header. Actual payloads carry `ipum`, little-endian size, width/height and one frame.
- `29E714–29E718` and `29E7D4–29E7D8`: kind 2 selects 16-bit/1555 output.
- `2575B8–257634`: source frame flags populate IPU control; IDEC uses output RGB16, dithering enabled, no signed RGB offset, and initial quantizer code 1. All accepted TA IPU frame flags are zero.
- `2F56C4–2F56D4`: initial SETTH command has both thresholds zero.
- `2421A4–2421FC`: kind 2 binding sets PSMCT16, TA0=255, TA1=255 and AEM=1.
- `2428B0–2428D8`: those descriptor bytes pack into TEXA register 0x3b at bit positions 0, 32 and 15, respectively.

FFmpeg decodes only the stored intra frame to planar YUV420P, with scalar simple IDCT, CPU-specific acceleration disabled and bit-exact mode selected for reproducibility. The last B1 sequence terminator is excluded from the decoder input because the raw FFmpeg parser otherwise presents it as an empty second packet. The B0 frame terminator and all actual encoded image bytes remain unchanged. Strict decoding has no errors and requires exactly one full YUV420P frame of the declared dimensions.

The cold converter applies the documented integer IPU BT.601 color coefficients, nearest 2x2 chroma use, fixed 4x4 dither, clamping and RGB5 quantization. RGB5 expands into the high five bits, with no gamma adjustment, row flip or GS upload rearrangement. TEXA/AEM produces alpha 255 for nonblack RGB16 values and zero for black. Original indexed arrow RGBA, including its alpha, is preserved exactly through the existing CSM1 palette permutation.

Reference implementations consulted for format facts and arithmetic:

- [FFmpeg IPU demuxer](https://ffmpeg.org/doxygen/7.1/ipudec_8c_source.html)
- [FFmpeg IPU decoder](https://ffmpeg.org/doxygen/7.1/mpeg12dec_8c_source.html)
- [IPU integer color coefficients](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/IPU/yuv2rgb.cpp)
- [IPU dither matrix](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/IPU/IPUdither.cpp)

No emulator code was built, linked or run. The converter expresses the documented format arithmetic independently.

## Validation and explicit fidelity limits

Focused checks passed:

- 543 actual-resource identity, full frame/mip boundary, malformed/truncated input, output-hash, RGB16 and TEXA invariant checks.
- Native 48C extraction and pinned callback for all eight resources.
- Sixteen source-substitution/unknown-name cases rejected transactionally.
- All 21,760 arrow frame-0 texels across four mip levels exactly match the independent native 48E decoder.
- A contact-sheet inspection showed authored arrow signs, cord, crawler body/side/wheel surfaces and crowd cutouts with coherent pixels.

The remaining precision limit is explicit: the PS2 hardware IDCT has not been compared pixel-for-pixel against FFmpeg's inverse-DCT implementation. Minor rounding differences may survive RGB16 quantization. Dither clamping follows the independent hardware reference rather than a captured original framebuffer. Thus kind-2 images are authentic compressed-source decodes with supported original color policy, not a claim of measured pixel-perfect PS2 output. Shader behavior, animation timing, filtering, mip choice and blend/cutout rendering belong to the client and were not certified by this converter.

## Reproduction

From the project root:

    python -B tools/assets/decode_source_textures50fc.py --index-header tests/native/source_texture50fc_index.inc
    python -B tools/assets/check_source_textures50fc.py
    g++ -std=c++17 -O1 -Isrc/assets src/assets/import_decode48c.cpp src/assets/import_texture48e.cpp tests/native/source_texture50fc_check.cpp -lz -o /tmp/source_texture50fc_check
    /tmp/source_texture50fc_check

The converter optionally accepts `--all-atm-frames` to export the other genuine stored frames. Do not replace a frozen runtime index or native texture while a live build is being checked; coordinate any regeneration with the integrating owner.
