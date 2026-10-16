# Gasgano07c_2 native IPUM requirements: source-only rounds three and four

Base Git `79fc223eda2da3d98929c9f3b1253745d17e7c2a`; delta baseline
`ROUND3-BASELINE`. No conversion, parser invocation, preparation or test ran.
Round four hardens the generator against `ROUND4-BASELINE`; its source patch
does not repeat the round-three cache changes.
Gasgano remains unadmitted. Generated `.inc` files and Course/voice lists are
unchanged. Source pins below are not decoded pixel identities.

## Original source evidence reused

`recovery/20261014/gasgano/original-evidence/original-data.json:1086` records actual original member
`ui\textures\07c_2.pst`: 29512 bytes, logical offset6316032, index file
offset7850, SHA-256
`cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce`.
The enclosing `UI/UIRIDER/UIRIDER.RES` is 5552683 bytes, SHA
`112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856`.
It occupies logical blocks257/258, whose compressed file ranges start at
1456936/1480692 and contain23755/4995 bytes. RES block framing is distinct
from the following **compressed IPUM payload** hashes inside the PST.

| Mip | Dimensions | PST header | Payload | Bytes | Payload SHA-256 |
| --- | --- | ---: | ---: | ---: | --- |
| 0 | 16x16 | 12 | 32 | 216 | 51c6ffdc77b9e50ffff5d9b7d00241f4da0c6a1d9de99cc84423f24e660321a2 |
| 1 | 32x32 | 248 | 268 | 776 | a93eec935f3de3fec135477ff4f22f9ebb9889acbeacc1d85f1197548ebef856 |
| 2 | 64x64 | 1044 | 1064 | 2392 | d525e93bf42b571ebaf2327a5f00bc4ab2c2b80b2dd242aeb43c79644cc2ba87 |
| 3 | 128x128 | 3456 | 3476 | 7101 | fbb471df280643efeb1afa45fdc8b9d29b44da9a12c270900ba83aa25e021424 |
| 4 | 256x256 | 10577 | 10597 | 18915 | 89b135287cd4fbc7a0ecbeda1105396abe6eb8ca1dfab643553f8416fb7f3e6d |

Prior observations establish revision1/kind2, no palette, five ascending mips,
and whole-member end29512. No original files needed rereading this round.
The original material `07c_2.psm` authors one DiffuseCompressed pass referencing
07c_2; it is not the indexed kind1 sibling07c_1. No palette conversion,
recoloring, substitute source or single-mip replacement is appropriate.

## Existing decoder contract, read only

`tools/assets/decode_source_textures50fc.py:56` requires the PST header and
ascending doubled dimensions. Each kind2 mip has a20-byte header: height,
width, two zero fields, compressed byte count. It requires payload magic
`ipum`, embedded size equal to payload bytes minus8, matching16-bit dimensions,
frame count1, byte16 zero, and terminal bytes `00 00 01 B0 00 00 01 B1`.
These are required parser conditions, not a claim that its parser ran here.

At line108 the helper passes the original payload except the last four bytes
(the final B1 container terminator) to native host FFmpeg, retaining B0.
The full original payload, including both terminators, supplies the source hash.
The existing command contract is:

```text
-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact -idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 -frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1
```

FFmpeg must return exactly width*height*3/2 YUV420P bytes with no error/stderr.
The original helper's line94 scalar transform uses nearest2x2 chroma, integer
BT.601 coefficients and the fixed4x4 dither at line93. With U/V centered on128,
`yy = floor(149*max(0,Y-16)/64)`, channel terms use204V, -104V/-50U and258U,
integer floor divisions and the final `/2` rounding step. Channels are clamped,
dithered, clamped again and quantized to RGB5 expanded into the high five bits.
Alpha is255 for any nonzero final RGB and0 for black (TA0=TA1=255, AEM=1).
There is no row flip. Reuse that exact helper; do not recreate its rounding in
another implementation. Native output is not proven PS2 inverse-DCT parity.

The helper is independently pinned to SHA
`e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe`.
Its `verify_elf` performs static SHA/anchor reads only. Neither that helper nor
FFmpeg nor SLUS was executed in this round.

## Native cache and generated provenance contract

`src/assets/original_ui_ipum_texture_cache50kg.cpp` uses compiled
`AdditionalSourceRecord` rows: exact member/name/size/hash, mip count,
`SourceMipRecord[5]`, `RetainedMipRecord[5]` and `RetainedCodecRecord`.
Each source mip records dimensions, original header/payload offsets, payload
bytes/hash and retained filename. Each generated mip must carry **real** RRTEX,
RGBA and YUV420P SHA-256 values. Codec provenance must retain the unchanged
helper hash, actual generator hash, installed FFmpeg version and binary hash,
and exact command string. The manifest schema remains
`RR_UNLOCKED_RIDER_IPUM_50KG_1`; `hardware_pixel_parity_verified` remains false.

An RRTEX is `RRTEX001`, little-endian width/height, then RGBA; its length must
be16+width*height*4. The five expected future filenames are
`07c_2-mip0.rrtex` through `07c_2-mip4.rrtex`. Names specify the contract only;
no such output or output hash was created. Runtime retains every mip and uses
the final256x256 RRTEX as base, with611344 retained bytes per this source
(all mip RGBA plus independent base RRTEX). Aggregate native limits stay intact.

`loadAdditional` still authenticates the actual Rider archive/member, full
source bytes, each compressed payload and retained RRTEX/RGBA hash, dimensions,
RGB5/alpha constraints and byte ranges before transactional publication. YUV
hashes are generator provenance: runtime does not re-decode YUV to verify them.
Review and trusted integration of generated rows are therefore necessary;
caller-supplied hashes are not an alternative. Source/member pointer identity
continues to control `textureFor` and `provenanceFor` publication.

## Bounded source changes

- Cache source: adds only the independently known whole-member/mip source
  requirements. A future generated Gasgano row must match all five exact
  layouts, payload hashes and filenames. Existing output/codec gates remain.
- Cache header: clarifies that a source pin cannot create a compiled source
  index. No storage capacity or API changed.
- Preparer: adds explicit `--source 07c_2`, preserves the original eight-input
  default, and compares all five original mip layouts/hashes before decoding
  Gasgano. The maximum of eight generated kind2 sources matches existing native
  storage. Neither a generated row nor fake output digest was added.
  Round four makes any selection containing07c_2 regenerate the complete
  original eight-input batch plus Gasgano, preserving all seven catalog members.

The current read-only generated catalog has seven rows (04c_1/04c_2,
10c_1/10c_2,12c_1,15c_1/15c_2); the original default also lists12c_4, which
does not appear in that generated kind2 catalog. One real Gasgano row therefore
fits the existing eighth slot. No capacity increase or assumption that all
source input rows become generated kind2 rows is needed.

## Parent-owned later conversion and publication

Approval for central execution is pending. This document does not authorize a
run. After separate authorization, parent must supply the exact flat07c_2 PST,
authenticate its origin in the pinned archive, and use the unchanged pinned
native helper to produce all five real pixel outputs and codec provenance in
a fresh staging root. The modified preparer records its actual new generator
hash when that later run occurs.

### Round-four additive generation guard

Selecting `--source 07c_2`, alone or alongside other choices, now expands the
effective input set to04c_1,04c_2,10c_1,10c_2,12c_1,12c_4,15c_1,15c_2 and07c_2
in the existing source order. Parent needs all nine authentic input files.
This is complete regeneration, not a merge of old pixel records or an append
to an installed catalog. No new CLI switch or caller-supplied baseline catalog
is required. Omitted `--source` keeps the original eight-input default;
explicit selections without07c_2 retain their existing selected-only behavior.

Every effective input's original size/hash must authenticate before helper
import or FFmpeg invocation. A missing or mismatched original file fails the
batch. In Gasgano mode, only12c_4 may be indexed kind1; every other required
input must be kind2, preventing the old skip branch from silently discarding
an existing catalog member. Existing whole-source, five-mip Gasgano, decoder,
output and codec authentication requirements remain in force.

Before any output write, the generated source list must equal the seven
retained members in order followed by07c_2; the skipped list must contain only
12c_4. A count of eight alone is insufficient. A conversion failure, wrong
source kind or incomplete/reordered source list cannot publish a partial
Gasgano catalog. Filesystem write failures may still leave partial staging
files; the generated catalog is written last. Parent must reject incomplete
staging and use a new output root for retry, as before.

The additive manifest retains schema `RR_UNLOCKED_RIDER_IPUM_50KG_1` and adds
`catalog_policy=gasgano_additive_regeneration`, `requested_sources`,
`effective_sources`, and `required_baseline_members`. Pixel hashes and codec
records for **all** rows come from actual conversions in that same future run.
No previous output hash or generator identity is copied and labeled as new.
Regenerated bytes may differ with an installed codec version; the real run's
provenance must be reviewed, not overwritten with historical digests.

Generated `.inc` integration and actual output/cache publication remain
parent-owned. The guard preserves this evidenced seven-member baseline, not
unknown additions on the parent's newer checkout. Parent must reconcile any
newer catalog membership before running or installing a regenerated catalog.

Parent must then establish actual image/material ownership, GPU publication,
whole Gasgano rig/PHY/COL/audio closure, and source UI profile/unlock behavior.
Only that later full closure can justify changing Course/voice admission.
All racers, including followed slot0, remain original in-game AI; host course
driver stays disabled. No AI/physics, guest or simulated-input path changed.

Remaining unknowns: actual YUV/RGBA/RRTEX digests, installed codec provenance,
native conversion success, cache load success, image rendering and full racer
destination closure. All tests and build-manifest integration belong to parent.
No software checks or project execution occurred; prior evidence and handoffs
and all generated catalogs were preserved.
