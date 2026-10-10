# Zero-area CEL glyph reconstruction

This is a new, narrow source-derived correction, not an exact recovery of the lost final text overlay. Only `src/game/original_ui_text50kg.cpp` changes. The recovered originals and active repository are untouched.

## Evidence and correction

- `ui-exact/RECOVERY_INDEX.json` labels the recovered text implementation as `exact_older_author_version_final_zero_extent_overlay_missing`.
- `ui-exact/font-author/src/assets/original_ui_font50kg.h` describes the optional metric fields as signed 16-bit extent-minus-one values; the parser in the adjacent `.cpp` decodes and preserves these signed values, including -1.
- The recovered layout computes right/bottom from `int(widthMinusOne) + 1` and `int(heightMinusOne) + 1`. Consequently -1 produces a zero extent, whereas values below -1 produce a negative extent. Its old early `< 0` guard incorrectly rejects the zero case.
- Change only this admission guard to `< -1` for each dimension. Retain the later finite-coordinate and `right < left || bottom < top` checks, which reject nonfinite/reversed rectangles but correctly admit coincident edges.
- Preserve the original bearing addition, advance addition, measurement summation order, first glyph's height, atlas UV arithmetic, quad ordinal, and output count. In particular a zero-area first glyph still contributes its genuine first-glyph height for vertical positioning; its bearing and advance still affect following glyphs and horizontal justification.

## Consumer compatibility

`ui-exact/coordinator/assembly/files/src/game/original_ui_text_geometry50kg.cpp` uses width and first-glyph height to position each line, then lays it out again at that origin. `ui-exact/pages-author/files/src/game/original_ui_text_position50kg.cpp` permits zero measured height and keeps font line-leading separate. The correction does not substitute the next visible glyph's height or change line leading.

`ui-exact/coordinator/assembly/files/platform/linux/native/original_ui_screen_canvas50kg.h` expressly documents acceptance of genuine zero-area glyphs. `ui-exact/coordinator/transcript-complete/files/platform/linux/native/original_ui_canvas50kg.h` consumes `count` quads, validates finite coordinates, and does not reject equal edges. Thus finite, zero-area quads are part of the recovered consumer contract, not invalid negative-size geometry. Keeping them avoids inventing a new compacted-quad indexing/count contract. No consumer or parser modification is necessary for this specific defect.

The UV-only branch, strict UV ordering in the parser, mixed-metric rejection, and malformed negative-extent rejection remain unchanged. There is no evidence in this focused task to relax them.

## Remaining uncertainty and verification limits

The lost final overlay's exact bytes/hash and original archive font assets have not been recovered. This patch cannot be claimed byte-identical to that overlay or visually verified against actual fonts. Source inspection establishes compatibility with the recovered consumers; it does not establish runtime or rendering correctness. No tests, builds, syntax checks, importers, gameplay, PC access, or Git operations were performed. Artifact hashes are in `MANIFEST.json`; the changed file is under `files/`, and the unified patch applies to the exact older author version only.
