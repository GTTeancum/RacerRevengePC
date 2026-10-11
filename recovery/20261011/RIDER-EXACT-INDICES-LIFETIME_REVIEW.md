# Exact source-index lifetime review

Decision: **APPROVE, source-only and scoped to buffer ownership, bounds, upload lifetime, cleanup/retry, and VAO binding.** No blocking defect found in the identified candidate. This is not build, runtime, performance, or visual acceptance.

Reviewer: `review_exact_index_lifetime`, independent of the candidate author.
Review date: 2026-10-11 UTC.

## Exact identities

- File: `platform/linux/native/vehicle_window_gl50ds.cpp`
- Baseline SHA-256: `9947bea8f3719bdc174df6e065a731bfbfef1bc878c6fadccf2ba606df5a4516`
- Candidate SHA-256: `c30fa4f3553bb66c187a3ae923936e9b2069d5ae44743e404ad9ec8b86ab6a9e`
- Patch: `exact-indices.patch`
- Patch SHA-256: `44895fff5c99b2e994d22325c90b81c45859353762c80ca5ab66360f68e407a9`
- Reviewed candidate: `files/platform/linux/native/vehicle_window_gl50ds.cpp` relative to this report.
- Reviewed preimage: `exact-preimage/platform/linux/native/vehicle_window_gl50ds.cpp` relative to this report.

The preimage and active source both matched the stated baseline at the final source inspection. The reviewer read the full patch and independently compared preimage against staged candidate. The active source was not edited.

## Buffer and ownership map

The existing UI vertex, particle-fog, and source-W buffers retain their previous ownership and upload path. The existing `uiIndexBuffer` remains the cold static quad topology buffer used by ordinary RGB submissions. Initial UI VAO creation and `verifyLayout` still associate that legacy element buffer with `uiVertexArray`.

The new `uiSourceIndices` is a `Storage`-owned `unique_ptr<UiSourceIndexStream>`. It owns a fixed array of 104,448 32-bit indices and a fixed table of 2,048 `(firstIndex, indexCount)` ranges. These hold 417,792 index bytes and 16,384 range bytes, respectively. Neither stores a borrowed pointer into caller vertices or spans. The stream allocation occurs only during cold creation with `enableOriginalUiSourceRgba` enabled. There is no added frame-time heap allocation or large frame stack array.

The new `uiSourceIndexBuffer` is distinct from `uiIndexBuffer`. Cold allocation binds it as `arrayBuffer`, before UI VAO creation, so allocation does not replace the legacy VAO element association. `uiCapacity` is the source maximum in this branch, giving the GPU store the same 417,792-byte maximum as the CPU index array. Allocation uses `dynamicDraw`, checks GL errors and a nonzero name, and verifies size/usage. Ordinary RGB creation does not allocate this source stream or buffer.

## Capacity and initialized-prefix proof

Candidate lines 4074-4099 pack only after the existing full CPU input validation. That validation caps source quads at 17,408 and span count at 2,048. The span checks require a nonempty, exact in-order partition: each first quad equals the current endpoint, each count fits the remaining quads, and the final endpoint equals the total. Thus every source quad is visited exactly once.

Each visited quad appends at most two triples, or six indices. Consequently:

- `sourceIndexCount` never exceeds 104,448.
- Each uploaded prefix element has been written in this submission.
- Each stored vertex index is at most `17408 * 4 - 1`, or 69,631, within the uploaded vertex prefix.
- Each range starts at the prior packed endpoint and ends no later than the current total.
- Every range count is divisible by three.
- Each used span-table entry is rewritten before use; entries past the validated span count are unused.

The new static assertions cover signed GL draw-count and byte-size conversions. The established bounds also keep the quad, vertex, range-end, and byte-offset arithmetic within the relevant unsigned types.

The index array's unused tail need not be initialized: only `sourceIndexCount * sizeof(U)` bytes are uploaded, and all draws remain inside the recorded ranges. A zero-survivor span issues no draw or barrier. If the whole frame has zero surviving triples, the zero-byte upload uses a valid owned pointer and no draw can read stale GPU contents.

The rejection condition is specifically the existing repeated-equal-XY-point test. This review does not reinterpret it as a general collinearity or area test.

## Upload and draw lifetime

Candidate lines 4199-4217 bind the UI VAO, upload the existing vertex streams, bind the selected element buffer, upload the packed source index prefix once, and check preparation before allowing span work. `UiSourceIndexStream` remains owned by `Storage` throughout submission and is not mutated after this upload within the call. Draw pointers are byte offsets into the bound GPU buffer, not CPU pointers.

Subsequent source submissions may overwrite the same CPU and GPU storage. The CPU pointer is needed for the synchronous `bufferSubData` call, and ordinary ordered GL buffer-update semantics protect earlier draws from a later update. This introduces no persistent mapping, unsynchronized overwrite, deferred client pointer, or new fence lifetime. It may entail driver synchronization; no performance claim is established here.

Opaque spans use one draw for their exact nonempty packed range. Feedback spans walk that same range in steps of three and retain one texture barrier and one draw per triple. Neither path reads a neighboring range or unused tail. The range metadata remains stable for the whole draw loop.

## Failure, cleanup, and retry

- `originalUiSourceRgbaAvailable()` now requires both the CPU owner and GPU name. The source wrapper and internal source preflight both reach this availability check before packing.
- A failed cold CPU allocation returns through existing creation cleanup. A failed GL allocation/verification leaves any generated name tracked for cleanup.
- `hasGlResources()` includes `uiSourceIndexBuffer`, preventing cleanup from declaring completion while that name remains.
- The new name is in the existing `deleteName` chain after UI VAO deletion. A deletion error retains the name and sets cleanup incomplete; the established rule against host object reuse during failed close retries is unchanged.
- Creation failure retains `Storage` when cleanup is incomplete. Therefore the CPU owner also survives retries. Successful close or completed creation-failure cleanup destroys `Storage` and its CPU owner normally.
- Index-upload errors are observed by the existing preparation check before `sourceWorkStarted`. No new early return bypasses restoration after preparation begins.
- Existing source-owner retention after target-generation changes, receipt commit on `originalUiAccepted`, and history poisoning after started span work remain unchanged. The attempted texture-barrier counter remains unchanged as requested.

## VAO element association and restoration

The source element buffer is bound only after `uiVertexArray` is current. No intervening path changes the VAO during source span drawing or the source-to-presentation copy. Candidate lines 4315-4318 restore `uiIndexBuffer` while the UI VAO is still current, then unbind the VAO. This final restoration path is reached after upload failure, span failure, or copy failure as well as success, and its error is included in the existing finish check.

If restoration itself fails, acceptance is false. A later prepared submission explicitly selects its required element buffer again. The patch does not bind an element buffer against VAO zero or mutate another renderer VAO as part of this new path.

## Limitations

Only ordinary source reads, textual diff inspection, SHA-256 identity checks, and this staged review write were performed. No build, syntax check, test, project parser/decoder/importer/script, native program, game, guest execution, or GPU experiment was run. No runtime allocation-failure injection, deletion-failure injection, GPU upload readback, draw-call trace, visual equivalence check, or performance measurement was performed. Approval applies only to the exact candidate and patch hashes above and the scoped source-level findings.
