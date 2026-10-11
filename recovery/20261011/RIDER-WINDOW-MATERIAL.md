# Exact Rider pod-window native base image policy

Source-only staged overlay on integrated composition2e24d9e. Three replacements and exact preimages; no active edits, build, test, syntax check, native decoder/project-parser execution, gameplay or emulation. Original-data header/palette/hash inspection used only bounded standard-library reads.

## Result

`prepareOriginalUiRiderWindowMaterial50KM` accepts only canonical Rider `ui\textures\pod_wndw.psm` DiffuseCompressed passes1 and3, and only the exact original `pod_wndw.pst` published by the authenticated image bank. It preserves A0/B1/C0/D1 source-alpha blending, ATE/GREATER/AREF0/KEEP, fog enable, GEQUAL depth with writes, DefaultMT ownership and the unchanged common strict sampler/register checks. The compatibility alias `inputAlphaD` is handled by the existing original material reader, not rewritten by this gate.

Native highest-resolution/base-only policy binds the actual64x64 largest original image and requires byte equality with the bank publication. All three original indexed mip images and palette bytes remain on CPU. The converter verifies the same immutable owner, index, identity, mip policy and retained sourcePass1/3 after any genuine combined-bank remapping. It does not claim historical residentF4 or upload absent mip levels; MXL0 and equal original min/mag texel filters remain mandatory.

Pass2 remains the separate actual SpecularMapCompressed FIX255 pass admitted by the integrated source-composition stage. All three original passes and their actual material-registry dependency/FIFO order must survive full-frame composition. This local material gate cannot seal a frame or justify omitting/reordering the window. Ordinary one-mip and kind2 admission is unchanged. Unknown fog/geometry/target cases retain their existing rejection.

## Exact original bytes

`evidence/window-source.json` records byte offsets and payload hashes without running the project decoder. Original Rider catalog member logical offset983040, length6460, PST SHA256 `8b05cf14c5e08c59849b3a54bfca9e2c1c7432481a2414664a5933ab9da310ac`.

Revision1, flags0, kind1, mip count3. Original256-entry RGBA palette begins12 and ends1036; its alpha spans108..255 and is never normalized. Stored mip headers at1036/1308/2348 describe16x16,32x32,64x64; their payloads start1052/1324/2364 and end exactly6460. The largest stored mip is original level0 under source24EAD4..24EB9C, already documented in the indexed-body/IPUM stages. Published GPU base-only selection is an explicit native policy.

Original PSM length7768, SHA256 `0b5ba9f38a5db1ca4670dcc41e91efd847b0ed9844a26c8b954d9bb6b9e28429`. Passes1/3 author the same DiffuseCompressed texture/source-alpha tuple and inherit normal DefaultMT depth/test/fog. The authentic DefaultMT file only overrides microprogram name and alphaReferenceValue0; original24A8E0 initial state supplies the other recovered defaults through the existing material fields owner.

## Integration and limits

`candidate.patch` adds this bounded policy and converter only. `manifest.json` retains exact2e24d9e preimage and replacement hashes. `FROZEN_SHA256SUMS` covers all delivery inputs. Another worker is adding Rider fallback material helpers on the same preimage; apply these nonoverlapping additive patches rather than replacing that worker's newer entire material pair.

No runtime acceptance, complete pod render or performance claim. Root owns the centralized future checks and captures.
