# Original UI decoded-texture ownership candidate

New source-only reconstruction, 2026-10-10. This is not historical-source recovery and does not enable the UI.

## Deliverable

Copy the two files under `files/src/assets/` over their corresponding source paths, or review `changes.patch` against the supplied `preimages/`. The repository itself was not modified by this worker. Root owns integration and any future validation.

`OriginalUiScreenAssets50KG::decodedTexture(index)` returns immutable borrowed access to the complete successful `decodeTexture48E` result for that bank index. The result preserves revision, flags, original indexed kind, the original palette, every mip and its source offsets/raw RGBA bytes, and `base_rrtex`. It is never synthesized from `Texture48B` or normalized for alpha. The material preparer can now consume that same decoder result without guessing discarded fields.

The private decoded-texture vector is sized before any `base_rrtex` address is borrowed by the bank input table. It is not resized or moved again. The bank continues to parse the very same `base_rrtex` bytes and preserve input order. The old temporary encoded-vector table is removed; no second encoded-byte copy is introduced.

All source-range checks, decode rules, source/base-RGBA bank budgets, font pairing, bank identity checks, and publication order remain unchanged. Storage stays private until every source/font succeeds and the complete native bank loads. Failure destroys the candidate storage; an empty owner remains retryable and getters remain null. A failed reload leaves all previously published pointer identities/data intact. These are serial cold-call transaction guarantees, not concurrent-load synchronization.

## Provenance

- Preimage: `recovery/20261010/ui-exact/coordinator/transcript-complete/files/src/assets/original_ui_screen_assets50kg.{h,cpp}` in `/workspace/shared/rr-recovery-git`.
- The coordinator's `assembly/` contains screen-canvas and text-geometry sources, not this owner.
- Root integrated the same byte-identical screen-assets preimage at commit `3c04a9a3a7f7ba3e6438ddc1e457abafb96cb55a`. Both active source files were re-read and their hashes match the supplied preimages.
- Recovered cpp SHA-256: `7a64f1858d783c3238271b3b5d1aee8ea8d662f4db8c7616f3e86230ff5adf2b`.
- Recovered header SHA-256: `98dfa21df336d35d324d2266099db9fd8ec3d202fa9b8bb5b2c5039736039846`.
- The recovery manifest describes these preimages as complete literal transcription with no original per-file hash. Matching those recovered bytes does not establish historical hash verification.
- Dependency source evidence is recorded in `SOURCE-SHA256SUMS`. `Texture48E` owns format/palette/mips; `decodeTexture48E` retains raw, unscaled RGBA and serializes the base unchanged. `MaterialTextureAssets50EA` publishes matching input-order `Texture48B` entries. `prepareOriginalUiNativeMaterial50KG` requires the exact `Texture48E`, checks revision/flags/kind and one mip, and rejects raw alpha above 128.

## Limits and unchanged obligations

- Keeping the complete decoder result increases retained cold memory. Existing per-PST, mip/dimension, source-count and aggregate base-byte bounds still apply. With the current ascending-by-two mip rule, retained mip RGBA is less than 4/3 of the bounded aggregate base RGBA; retained RRTEX bytes and the native bank are additional owned storage. This is not a claim that the bank's RGBA budget now counts all copies.
- Multiple admitted source mips remain multiple mips. The strict material preparer can therefore reject them faithfully; the owner does not pretend their base is an authored single-level source.
- Raw alpha above 128 is preserved, so the preparer still rejects it where required. No clamping, recoloring or alpha normalization is added.
- A caller still must select the correct original material/texture, validate material admission, and establish depth, pass-group, animation and target-alpha ownership. No defaults or readiness claims are added.
- The renderer, app main, screen activation, build descriptions and other source files are untouched.

## Verification status

Source read-through, textual diff and SHA-256 provenance only. No compiler, syntax check, build, test, script/importer, gameplay or PC operation was run. Runtime behavior is unverified.

Future root-controlled coverage should check unloaded/out-of-range access, kind-0/kind-1 evidence and exact base-byte correspondence, multi-mip retention, raw alpha above 128, later-source/font failure rollback, duplicate-identity bank failure, retry after failure, pointer stability after rejected reload, and owner data independence from caller input lifetime. This is a checklist, not executed coverage.
