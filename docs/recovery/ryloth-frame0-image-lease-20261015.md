# One known RA crowd frame host-image lease

Isolated, source-only candidate. Frozen60bed38 central checkout untouched. No conversion, decoder, tests, builds, syntax check or gameplay was run. No existing RRTEX file was located/opened here; the known decoded-output identity comes from the already compiled source_texture50fc_index.inc. The verifier can succeed only when the caller later supplies those actual matching bytes.

## Exact proof

Existing tests/native/source_texture50fc_index.inc pins TEXTURES/CROWD.ATM,214578 bytes,SHA25676b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031, output crowd-frame0-mip4.rrtex,256x256,mip4,SHA256fcfeb0c00938ab1bbae791a24d705c18989b4b222e9d492ab4593d67dff79e98.

The retained authenticated RA member has exactly those source bytes. RA archive identity remains SHA2561e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2. No TA archive identity is assigned to RA.

Frame0 PST: offset12,35304 bytes,SHA2568c9c285f924853b221d9a1bf4a69f8c05f160dd256d73307f645d0bc09f6e219. Source mip ordinal4: header11623,payload11643,length23673,SHA2566a189a6e6f2ce8d0a253108477dc051c47d1b445bd8b24c73952b935bf7dbe7d. Dimensions256x256. Actual compressed stage's allocation00 must byte-equal that payload; codec18 must be base+16,length23657.

Unchanged50FC generation loops actual frame0 and source mips; final selected mip is4. Its file framing is16-byte RRTEX001+LE width/height, then262144 RGBA bytes. Original source RGB16+dither policy is host-decoded using FFmpeg and existing scalar CSC/high5-bit expansion/TEXA255/AEM1. This is the existing native image policy, not newly established hardware IDCT parity.

## Candidate API

New asset header defines the previously opaque `rr::assets::OriginalStandardIpumImageLease50LA`. Private constructor; immutable owned RRTEX backing; public source identity and read-only pixel borrows. The only friend is narrow native `ra_crowd_image50LA::Verifier`.

`verifyRaFrame0Mip4(actualResolver, actualSourceIndex, sharedCompressedStage, actualSuppliedRrtexBytes, out, diagnostic)` checks:
- Same actual resolver and source ordinal, complete six-frame compressed stage, RA mounted archive identity and exact member/source bytes.
- Exact frame0/PST/mip4 extents, source hashes, dimensions and actual copied compressed allocation equality.
- Exact262160-byte output extent, fixed compiled output hash, RRTEX framing and existing RGB5/TEXA policy.

It computes all hashes from actual bytes through existing pod_materials50ed_fixture SHA256 code. There is no caller-supplied expected hash, hash callback, image-bank decoder callback, permissive ordinal argument or fallback frame. Input pixels are copied into private immutable storage before the output hash is checked; no mutable caller alias survives publication. Out remains unchanged on failure.

The lease strongly retains its exact compressed-stage object; that stage retains actual resolver/provider. Its borrowed FrameView therefore remains valid through the lease. Pixel storage retires before compressed/source backing by member declaration order. No source resource is cloned, replaced or marked constructed; compressed storage and decoded RGBA remain separate representations.

## Explicitly closed integration

This lease certifies only one actual source frame/mip image. It has no complete-aggregate/render-ready method, canonical construction hook, material population, demand write, model callback, animation timing or GL publication. Canonical construct24DA30 still rejects kind2 before effects. Frames1..5 have no output hashes or minted image leases. Although some compressed payloads duplicate, each future child must retain its own original frame extent/identity; frame0 is never substituted for another ordinal.

Future canonical integration must collect exact leases for every source-required frame, retain each real compressed child allocation, preserve actual constructor/cache/list/service order and partial failure semantics, and satisfy native renderer retirement. The single-frame verifier cannot authorize six-frame construction by itself.

No source-level change to existing frame0 cache files/catalog, no generated outputs and no new hashes are proposed. Original source_texture50fc callback remains unchanged and is not called by this verifier. Optional future compile-only inclusion is `#include "ra_crowd_image50la.h"` in the actual main TU; no such frozen-central change is included in this candidate.
