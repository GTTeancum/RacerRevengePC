# Isolated source-review snapshot recovery

Recovered on 2026-10-10 from complete literal file bodies retained in this reviewer's earlier tool output. The source location was `/workspace/shared/rr-native-canonical-frame50jq/files/tests/native/`. The original isolated draft checkpoint was source-reviewed only. These files do not have last-tested d546da8f provenance, and no claim is made that they are the final author checkpoint.

The payloads are frozen as observed. No corrections, synthesis from summaries, reconstruction from partial drafts, source edits, active-tree access, PC access, Git operations, builds, tests, syntax/import checks, or runtime validation were performed during this recovery. File-writing and SHA-256/byte/line accounting were the only recovery operations. No further source searching was performed.

## Payload manifest

| Original relative path | Bytes | Lines | Recovery SHA-256 |
| --- | ---: | ---: | --- |
| tests/native/native_racer_noise50jq_fixture.h | 5390 | 113 | 359b16a6e86e221d2d207754541c85821c4b1d1e5a5cedf131725db10773f0c0 |
| tests/native/native_racer_bank50hn_fixture.h | 15389 | 282 | f2c09eb174927c0bbc0096e84ab3785ad0fdff431dcd438d6b8f82600511b0e1 |
| tests/native/native_racer_owners50hn.inc | 7680 | 175 | d27566eebe1ae0b137b7a43237424d92424c3aba46a43d66b5dfe0cd41d3c1c9 |
| tests/native/native_racer_contact_bank50hw_fixture.h | 12743 | 213 | 859732960543821dce76de68e14fe872a8d72dc8212008fa1a84638485655c5d |

Noise was retained in full in the standalone read returning chunk `1d743c` at approximately 08:30 UTC. Bank and owners were each retained as complete file bodies within the multi-file read returning chunk `3c5989` at approximately 08:30 UTC; that combined output was truncated elsewhere, not within either of these two payloads. Contact-bank was retained in full in the read returning chunk `949809` at approximately 08:32 UTC. Times and chunk identifiers describe this reviewer's source reads, not tested-build provenance.

## Original hash pins

No original hash was recorded by this reviewer for any of these files. The parent subsequently supplied the final checkpoint pin for owners only:

- Expected final owners SHA-256: 1459230a943b0d28a0d0e0a3b6ee8e8fc2450262f2e6a4949f8dd323bbd4f607
- Recovered reviewed owners SHA-256: d27566eebe1ae0b137b7a43237424d92424c3aba46a43d66b5dfe0cd41d3c1c9
- Comparison: MISMATCH. This is preserved unchanged as the earlier reviewed snapshot and must not be substituted as the final pinned owners file.

The other three SHA-256 values are recovery hashes only; no retained original pin was available for comparison. Matching line counts for noise, bank, and owners agree with the earlier source-review `wc -l` output, but do not replace original hash verification.

## Unavailable or deliberately omitted

- `tests/native/native_racer_handling50hw_fixture.h`: unavailable as a single complete retained literal payload; overlapping/truncated reads were not assembled.
- Frame and physical headers: deliberately omitted because the parent reported they were already restored elsewhere.
- No dependency completion or compilation usability is claimed for this isolated four-file snapshot directory.
