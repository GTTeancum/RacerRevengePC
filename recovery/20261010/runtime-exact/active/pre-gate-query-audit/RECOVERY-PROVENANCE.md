# Literal query/fallback header recovery

Recovered on 2026-10-10 from complete source text retained in the audit worker's tool-result conversation. Original checkout at the time of those reads: `/workspace/scratch/74cd1d3d7a45/racerrevenge_solo_return/RacerRevengePC`.

This is an isolated recovery directory, not an integrated checkout. Its `pre-gate-query-audit` label was supplied by the recovery coordinator; it does not prove that every recovered header predates Gate changes. In particular, the vegetation dispatch header here includes Gate declarations present in the retained readback.

No partial source was completed, guessed, or synthesized. No compiler, syntax checker, tests, importer, gameplay executable, Git command, UI, or PC access was used. Only literal text materialization, an isolated copy of a recovered file, exact retained patch application to that isolated copy, and SHA-256 hashing were performed. The active checkout was not modified.

## Complete files and hash comparisons

Five files match full-file SHA-256 values recorded before the workspace loss:

1. `tests/native/extended_fallback_world50jm_fixture.h`
   - Source: complete 378-line readback from the first audit read, approximately 10:25 UTC; later line-numbered excerpts independently confirmed sections. Includes the already integrated lower-bound resolver, without the subsequently staged deferred-Type3 optimization.
   - Expected: `4552de41b4a24dcd093755224f1545f30114ec94523671f50676b63cf30f143e`
   - Recovered: `4552de41b4a24dcd093755224f1545f30114ec94523671f50676b63cf30f143e`
   - Comparison: MATCH.

2. `src/game/full_movement_query50jn.h`
   - Source: complete raw header readback around 10:26 UTC.
   - Expected: `903729e15369bf8346c58bbe307d6326d2294f59d189f55926fd65a72d61a0ed`
   - Recovered: `903729e15369bf8346c58bbe307d6326d2294f59d189f55926fd65a72d61a0ed`
   - Comparison: MATCH.

3. `src/game/group_query48u.h`
   - Source: complete raw header readback around 10:26 UTC.
   - Expected: `606c63fce8111f28ec9ebd70f7455370769797d24d5a749afb07c2efa669eaa7`
   - Recovered: `606c63fce8111f28ec9ebd70f7455370769797d24d5a749afb07c2efa669eaa7`
   - Comparison: MATCH.

4. `src/game/type3_raw_cursor50jn.h`
   - Source: complete line-numbered header readback around 10:28 UTC; only the display line-number prefixes were removed.
   - Expected: `20a616942edc5553581822b011bc881f2c673a45a91e9026612cbdf1429d88c2`
   - Recovered: `20a616942edc5553581822b011bc881f2c673a45a91e9026612cbdf1429d88c2`
   - Comparison: MATCH.

5. `tests/native/gc_animated_candidates50ig_fixture.h`
   - Source: complete raw header readback around 10:28 UTC.
   - Expected: `a4b3554dd04fe31a60a2cbe62b7ef66b06c010f46fe4f57bb47b823657deb0b5`
   - Recovered: `a4b3554dd04fe31a60a2cbe62b7ef66b06c010f46fe4f57bb47b823657deb0b5`
   - Comparison: MATCH.

One complete literal readback has no historical hash retained in this worker's evidence:

6. `src/game/vegetation_raw_dispatch50jl.h`
   - Source: complete raw header section from a larger combined audit read around 10:25-10:26 UTC. The containing tool result was truncated elsewhere, but this header begins at `#pragma once`, contains its complete declarations/comments, and ends with its namespace close before the following search output. No missing part of this header was inferred.
   - Expected: UNKNOWN in this worker's original evidence.
   - Recovered: `72bcac4269857ee35b4b4dd71ea318a4e80b9a2dddb28444c0d926cc21d84a19`
   - Comparison: NOT HISTORICALLY HASH-VERIFIED; preserve as a literal readback variant.
   - Per-file chronology reported by the recovery coordinator after the author's comparison: `72bcac42...` here is the Gate-containing variant, with the Gate include and optional context contract; the author's distinct `85e6e6ec855e870ec076da62f08cbf7db7f5c65b68bcbb63fc4db0fe4a6b9837` variant has the pre-Gate signature and is preserved separately under `owner-author/vector-author`. Thus this file must not be classified as pre-Gate merely because of this directory's name. Do not replace either variant or infer equivalence. This worker did not inspect or modify the author lane.

`EXPECTED-SHA256SUMS` contains only the five historically pinned files. `RECOVERED-SHA256SUMS` also includes the unpinned vegetation readback so its recovered bytes are fixed for subsequent comparison.

## Separately recovered isolated optimization

The exact three-hunk diff retained from the earlier staged optimization is saved under:

`/workspace/shared/recovery-20261010/runtime-exact/isolated/deferred-type3-audit/deferred-type3.readback.diff`

That directory also contains an isolated postimage at `files/tests/native/extended_fallback_world50jm_fixture.h`. It was made from this hash-matching recovered preimage and only the retained three hunks. Its expected and recovered SHA-256 both equal `8d434d0c7b8de41b760c116a6ff6fad0bd39ce6a1c13bfcac951d3c787c4cc7f`.

This optimization remains separate from the recovered active variant. The recovery does not authorize or perform integration.

## Missing or partial evidence: not materialized as complete files

- `src/game/full_movement_query50jn.cpp`: partial opening section plus its complete final `queryFullMovement50JN` function; substantial middle missing. Historical full-file SHA-256: `9e1ca88702a7ab3ad4b1f2e4ad078f8702ad608342707ebc86b14f9052661cd2`.
- `src/game/type3_raw_cursor50jn.cpp`: selected search hits only, not complete source. Historical full-file SHA-256: `edc5c49e68c82948ec9556802070dc6702f4e220d579d555ff68d0b1a45d3a68`.
- `tests/native/terrain_interactive_drive50du.cpp`: multiple setup and frame/caller excerpts, some long combined outputs truncated; not complete. Historical full-file SHA-256: `cfae542ee5e7f36015344ea4438e1d5cb4a6beedfe62a5f65e68bb548318e2cb`.
- `tests/native/fallback_quadtree50jk_fixture.h`: population, pool sizing, admission, and accessor excerpts only; no full historical hash recorded here.
- `src/game/vegetation_raw_dispatch50jl.cpp`: partial/truncated opening section only; no full historical hash recorded here.
- `tests/native/gc_gate_candidates50kg_fixture.h`: a few search-hit lines only. Historical full-file SHA-256: `15152ea8f37d74d0f1f71444e9f694ebf47dddbe0ec14274a50a4c1d4511f1a5`.
- `tests/native/track_session_assets50ig_fixture.h`: GC session excerpt only. Historical full-file SHA-256: `f066c6f65d04777f3ffa388b15cb5519f817cf8a2335d054753400ae09f9bc2d`.
- `tests/native/track_runtime_storage50ig_fixture.h`: count/preparation excerpt only. Historical full-file SHA-256: `9e8f1522e938f9367741e181d0945eaa8b22f878598d37cc846eccb2b31fd3bd`.
- `tests/native/gc_animated_world50ig_fixture.h`: search-hit excerpts only. Historical full-file SHA-256: `cec7c7db25666e03966034301c0189bea77314ccc0f54be0f75681b717c4ff4e`.
- `src/game/ai_reset_physics50je.h`: a ground refresh excerpt only; no full historical hash recorded here.
- Gate type2/sphere, action, animation, and audio implementation leaves: no complete source or original author identity retained by this audit worker.

The old independent review receipt's exact write payload remains in the conversation, but it is a review artifact, not missing implementation source. It was not needed to complete this bounded six-header and isolated-diff recovery.
