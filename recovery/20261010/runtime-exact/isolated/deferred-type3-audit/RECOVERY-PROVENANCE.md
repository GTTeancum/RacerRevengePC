# Isolated deferred-Type3 diff recovery

This directory preserves a previously staged optimization separately from active-source recovery. It is not integrated into any checkout.

Literal source: the complete three-hunk `diff -u` readback retained by the source-audit worker around 10:27 UTC on 2026-10-10. `deferred-type3.readback.diff` preserves the readback's original absolute path labels, timestamps, context, additions, and removals. Its historical artifact hash was not recorded; its recovered SHA-256 is `24da6e6b045f9189621778cb722f1406792de7639af2c742617b259e5686726f`.

Expected preimage SHA-256: `4552de41b4a24dcd093755224f1545f30114ec94523671f50676b63cf30f143e`.

The complete preimage was recovered independently from the full historical source readback and matched that hash. It resides at:

`/workspace/shared/recovery-20261010/runtime-exact/active/pre-gate-query-audit/tests/native/extended_fallback_world50jm_fixture.h`

For an additional byte-identity check, this preimage was copied into this isolated directory and only the retained three hunks were applied to the copy. The resulting complete postimage is `files/tests/native/extended_fallback_world50jm_fixture.h`.

- Expected postimage SHA-256, recorded before loss: `8d434d0c7b8de41b760c116a6ff6fad0bd39ce6a1c13bfcac951d3c787c4cc7f`
- Recovered postimage SHA-256: `8d434d0c7b8de41b760c116a6ff6fad0bd39ce6a1c13bfcac951d3c787c4cc7f`
- Comparison: MATCH.

No partial source completion, guessed code, integration, Git operation, syntax check, build, test, importer, gameplay, UI, or PC access was performed. Hash checks establish the recovered bytes, not compilation or behavior.
