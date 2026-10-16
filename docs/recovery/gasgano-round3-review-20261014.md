# Gasgano round-three texture cache source review

## Verdict

No blocking source defect found in the proposed bounded source-only change. The patch is suitable for parent consideration as exact input requirements, not as texture recovery, runtime readiness, or racer admission. This is a textual review only, not a build or execution result.

The important integration hazard is explicitly documented but not mechanically prevented: the generator produces a complete catalog for only the selected inputs. A later `--source 07c_2` output must not replace the current seven-source catalog wholesale.

## Scope and evidence

Reviewed the handed-off `ROUND3.patch`, `ROUND3-REPORT.md`, `ROUND3-RECEIPT.json`, and retained `ROUND2-EVIDENCE/original-data.json` against current source in `/workspace/shared/rr-recovery-git`, including the cache implementation/header, additional generated catalog header, generator, and native helper parser/decoder definitions. No patch was applied. No production file was changed. No tests, builds, syntax checks, parser execution, imports, decoder/converter runs, original-data decoding, gameplay, or SLUS execution occurred. No independent automated checks were run.

## Exact source and mip identities

The whole-member pin in both changed source files matches the retained evidence for `ui\\textures\\07c_2.pst`: 29,512 bytes, SHA-256 `cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce`.

Both the proposed C++ `gasganoSourceMips` and Python `GASGANO_MIPS` reproduce all five retained evidence records:

| Mip | Dimensions | Header | Payload | Payload bytes | SHA-256 |
| --- | --- | ---: | ---: | ---: | --- |
| 0 | 16 × 16 | 12 | 32 | 216 | 51c6ffdc77b9e50ffff5d9b7d00241f4da0c6a1d9de99cc84423f24e660321a2 |
| 1 | 32 × 32 | 248 | 268 | 776 | a93eec935f3de3fec135477ff4f22f9ebb9889acbeacc1d85f1197548ebef856 |
| 2 | 64 × 64 | 1044 | 1064 | 2392 | d525e93bf42b571ebaf2327a5f00bc4ab2c2b80b2dd242aeb43c79644cc2ba87 |
| 3 | 128 × 128 | 3456 | 3476 | 7101 | fbb471df280643efeb1afa45fdc8b9d29b44da9a12c270900ba83aa25e021424 |
| 4 | 256 × 256 | 10577 | 10597 | 18915 | 89b135287cd4fbc7a0ecbeda1105396abe6eb8ca1dfab643553f8416fb7f3e6d |

The C++ gate additionally requires exactly five mips and exact `07c_2-mip0.rrtex` through `07c_2-mip4.rrtex` filenames. The Python gate requires kind 2, no palette, exactly five mips, exact dimensions/offsets/lengths, and hashes of actual compressed payload bytes before decoding Gasgano. These remain compressed-input identities; they are not output hashes. The source archive and helper pins are unchanged.

## Admission and trust boundary

Adding a member to `additionalPins` does not add an `AdditionalSourceRecord`, change `additionalSourceCount`, create output files, or allocate a cache source index. The current generated catalog declares seven sources; the patch does not edit that file. The new checks apply only if a later compiled row names Gasgano.

Existing catalog gates remain: bounded count, duplicate-member rejection, pinned whole-member size/hash, valid output/provenance digest fields, exact retained converter identity and command, and nonempty codec version. For Gasgano the proposed checks further constrain every original mip and its retained filename.

The existing `loadAdditional` path still authenticates archive/member identity, whole source bytes, source header and payload extents, compressed payload hashes, every retained RRTEX and RGBA hash, image dimensions, and RGB5/alpha constraints before publishing transactional storage. Existing pointer/whole-member matching for `textureFor` and `provenanceFor` is unchanged.

Important existing boundary: syntactically valid compiled output digests and codec metadata are trusted integration inputs. Runtime does not independently prove a decoder run or rederive the YUV hash. This patch neither introduces nor removes that boundary. Genuine staged outputs and trustworthy provenance remain mandatory for later integration. No Course/voice/domain admission or gameplay hunk appears in the patch.

## Preservation and future generator integration

The original eight default input tuples are preserved as `DEFAULT_SOURCES`; omission of `--source` continues to select those inputs only. Gasgano is available solely through explicit selection. Existing Rider19 artifacts and repository catalogs are not directly overwritten by this generator: it requires a fresh output root and uses exclusive output creation.

The generator does not read or merge the previous additional catalog. Its output contains only `sources` built from the selected run. Consequently:

- A Gasgano-only run is appropriate as isolated staging, not as a replacement seven-plus-one catalog.
- A later combined run must explicitly select the prior inputs plus Gasgano, preserving the old source coverage. The documented nine-input selection is intended to yield eight kind-2 rows after the indexed input is skipped.
- Alternatively, integrate the authentic new Gasgano row while preserving all existing rows and their individual codec provenance.
- The new `len(sources) <= 8` check limits emitted kind-2 rows to current native capacity. It occurs after decoding and before writes; it is not an up-front work-limit check.

The patch's documentation accurately calls out this selected-only behavior. No merge feature is promised or provided. Existing row/output preservation during later publication is a parent-owned requirement.

## Current API compatibility

The current `SourceMipRecord` fields exactly match the new initializer order and comparisons: width, height, header, payload, bytes, SHA-256, filename. `originalUiIpumMipCount50KG` is five. Current `AdditionalSourceRecord` provides the mip arrays and codec fields used by the patch. The class retains eight additional storage slots and the validator already caps source count at eight. Public signatures, storage layout, and the legacy Rider19 API are unchanged; the header change is commentary only.

The current helper's `parse_resource` returns frames with `kind`, `palette`, `mips`, `start`, and `end`; each mip has `width`, `height`, `header_offset`, `data_offset`, and `data`, matching the proposed Python access pattern. `decode_ipu` still returns `(rgba, yuv_sha)` and remains untouched. The runtime provenance gate accepts each row's actual generator digest rather than pinning it to the previous generator digest, so modifying the preparer does not by itself invalidate existing compiled rows.

No incompatibility is apparent from source inspection. Patch applicability, compilation, runtime load success, conversion success, visual correctness, and full racer closure were not established and must not be reported as verified.
