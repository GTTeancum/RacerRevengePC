# Authenticated Rider IPUM image cache reconstruction

Status: source-only candidate, authored and statically reviewed. No build,
syntax check, candidate parser, codec, fixture or native client was executed.
Only independent bounded standard-library reads/hashes of original files and
literal source-word inspection were run. The active repository was not edited.

## Useful result

The authentic `ui/textures/19c_1.pst` is revision1/flags0/kind2 with five stored
IPUM images, from16×16 through256×256. It is an intra image-codec stream, not
packed uncompressed1555 pixels. FFmpeg's native `AV_CODEC_ID_IPU` decoder
already handles it using host MPEG intra coefficient decoding and inverse DCT.
The existing repository50FC scalar helper is reused unchanged; no new codec,
guest execution, PS2/VU emulator or IPU device model is implemented.

Official codec references inspected:

- https://ffmpeg.org/doxygen/7.1/ipudec_8c_source.html
- https://raw.githubusercontent.com/FFmpeg/FFmpeg/n7.1/libavcodec/mpeg12dec.c

The flags-zero source selects FFmpeg's MPEG2 intra block path and planar
YUV420P output. The retained helper already fixes scalar simple IDCT,
single-thread decoding, strict error detection and exact one-frame output.
Only the final four-byte B1 container sequence terminator is excluded; the
B0 frame terminator and compressed coefficients remain unmodified.

## Public boundary

`OriginalUiIpumTextureCache50KG` is a cold, load-once, immutable typed owner.
It accepts an authenticated `OriginalUiArchive50KG` for Rider and five
untrusted RRTEX byte inputs in original ascending-mip order. It authenticates:

- Exact original archive/member identity and whole23,096-byte PST SHA256
- Every original header, dimensions, payload extent, flags and frame markers
- All five independently pinned original IPUM SHA256 values
- Compiled per-mip RRTEX and RGBA output hashes from the real cold conversion
- The pinned existing50FC converter source identity and generated codec record
- RGB5 high-bit expansion and the retained TEXA255/AEM1 cache alpha policy

Callers cannot provide expected hashes, a callback, alternate source identities,
replacement pixels with self-declared provenance, or a success override.
The generated output catalog is deliberately marked unavailable in this source
package. Until root generates genuine output hashes and compiles that catalog,
`load` returns `cacheUnavailable` and publishes nothing. There are no stand-in
textures or invented output hashes.

The owner retains every source mip and returns an immutable `Texture48E` with
kind2, original dimensions and original offsets. Its zero palette storage does
not represent an authored palette. `source_indices_offset` is the legacy field
used for the full IPUM payload start, including its16-byte header.
`colorPolicy()` returns `original50FcRgb5Texa255Aem1` only after a successful
authenticated load. It returns `unavailable` otherwise. The same typed value
is retained as provenance `colorPolicyKind` for immutable downstream owners.

`textureFor(member, 0, member.size)` accepts only the very same authenticated
archive member identity/data pointers and its whole PST range. The owner keeps
that archive alive. Identical bytes copied into another file/view do not grant
authority. This closure supports only actual19c_1; unrelated kind2 files remain
unsupported until separately authenticated and cataloged.

`OriginalUiScreenAssets50KG::load` adds one optional trailing typed cache pointer.
Existing two-argument calls retain their behavior. When the exact member is in
the supplied cache, the screen owner copies the complete texture and codec
provenance transactionally. Otherwise it calls the unchanged `decodeTexture48E`,
which still rejects kind2. `ipumProvenance(index)` returns copied immutable
codec/source/per-mip evidence, and existing exit `sourceProvenance` is preserved.
Destroying the input cache does not invalidate screen pixels/evidence.

## Required ordered dependency

The ScreenAssets pair is based on the exit package integrated as4c8ae0e.
`preimages-after-exit/src/assets/original_ui_screen_assets50kg.{h,cpp}` are the
exact required preimages and matched the active repository at18:09UTC:

- Header `c928f85f81e9b62f59bda8fea27e712fd7bf9ee6cce11c2d63a8768245179509`
- Implementation `338c3ddb8c7cacb8396f628f5b7b11d3240edc647217bdc3f261b15e4a9946bd`

Older pre-exit copies are retained separately under `preimages/` as historical
evidence and must not replace the exit dependency. The `import_texture48e.h`
candidate has comment-only changes with its own exact preimage there. The
`decodeTexture48E` implementation is untouched. A build containing ScreenAssets
must link the new `original_ui_ipum_texture_cache50kg.cpp` translation unit.

## Root-only conversion recipe, NOT EXECUTED

Run only in the root-owned authorized validation window. The output root must
not exist; no existing cache or catalog is overwritten.

```sh
python3 -B /workspace/shared/recovery-20261010/ipum-texture-reconstruction/files/tools/assets/recover_rider_ipum50kg.py \
  --repo /workspace/shared/rr-recovery-git \
  --source-pst /workspace/shared/recovery-20261010/ui-rider-controller-reconstruction/preview/members/ui/textures/19c_1.pst \
  --rider-archive /workspace/shared/recovery-20261010/ui-inputs/game-files/UI/UIRIDER/UIRIDER.RES \
  --elf /workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68 \
  --output-root /workspace/shared/recovery-20261010/rider-ipum-generated-01 \
  --execute-cold-conversion
```

The generator checks original hashes and the retained helper hash before
importing the existing helper. It reuses its `verify_elf`, `parse_resource` and
`decode_ipu` functions, with the same30-second decode limit per mip. It records
actual FFmpeg version/executable hash, exact arguments, generator/helper hashes,
original frame/payload hashes and every resulting YUV/RGBA/RRTEX hash. All five
frames must decode before output writes start. The enabling C++ catalog is
written last, after each image and manifest is read back.

Output images and manifest live under `evidence/ui-rider-ipum50kg`; the real
generated catalog lives at `src/assets/original_ui_ipum_texture_cache50kg_generated.inc`.
Root must copy that generated catalog into the same source tree as the new
owner before compiling the focused fixture. Do not merely flip the unavailable
boolean or invent the missing output hashes.

## Focused fixture recipe, NOT EXECUTED

After source integration and real catalog generation, the direct link recipe is:

```sh
g++ -std=c++17 -O1 -Wall -Wextra -Isrc/assets \
  tests/native/original_ui_ipum_texture_cache50kg.cpp \
  src/assets/original_ui_ipum_texture_cache50kg.cpp \
  src/assets/original_ui_archive50kg.cpp src/assets/original_ui_screen_assets50kg.cpp \
  src/assets/original_ui_font50kg.cpp src/assets/material_texture_assets50ea.cpp \
  src/assets/native_asset48b.cpp src/assets/import_decode48c.cpp \
  src/assets/import_texture48e.cpp -lz -o /tmp/original_ui_ipum_texture_cache50kg_check
/tmp/original_ui_ipum_texture_cache50kg_check \
  /workspace/shared/recovery-20261010/ui-inputs/game-files/UI/UIRIDER/UIRIDER.RES \
  /workspace/shared/recovery-20261010/rider-ipum-generated-01/evidence/ui-rider-ipum50kg
```

The authored fixture checks real source/cache identity, all15 single-mip
corruption/truncation/suffix cases, retry after failure, cancellation around
every mip, exact byte limits, load-once behavior, copied/partial member rejection,
five-mip metadata retention, unchanged default decoder rejection, ScreenAssets
source/codec evidence and owner-lifetime independence. It has not been compiled
or run; this is a proposed validation recipe, not a test result.

## Fidelity and remaining owner boundaries

All texture pixels must come from the exact compressed source through the
retained host codec. Hardware inverse-DCT rounding and original framebuffer
pixel parity are not measured. The cache's sampled-alpha representation follows
the existing50FC texture descriptor policy; final material TEXA packet ordering
is not certified here. Lighting separately restricts alpha-insensitive opaque
passes until that owner is recovered.

Retaining five original mips does not settle original mip selection. Static
source proves last stored256² is original level0, but binding24E790 selects
texture.F4. The original24E670/24E4A0 priority/residency logic dynamically writes
that field. MXL0 alone does not prove F4=0. This cache never invents a resident
level, sampling decision, material admission, texture-bank identity remap,
GPU readiness or complete Rider route. Root's chosen native renderer policy
may concretely upload the authenticated original level0 and keep it resident,
retaining all smaller original mips on CPU, without recreating historical
console memory pressure. That must be
reported as a verified native residency policy, not an observed historical F4.
See `evidence/mip-residency-findings.md`.
