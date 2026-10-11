# Rider directional pod orientation transfer: inactive native source stage

Base: `93083c51a0e5cce4c8e98351b9e73e5ae187182a`. Prepared 2026-10-11.

## Result

Staged four existing source members, exact preimages and `POSE-TRANSFER.patch`. This adds value-only directional look/up capture, the distinct `289EC0(a3=0,t0=1,t1=0)` pose branch, live graph restore preserving the incoming local position, and loaded-pod scene entry points. No repository file was modified and no session, navigation, dynamic-owner, renderer, material, bank or lifetime policy was enabled or changed.

The cross-correction helper `originalUiUserRotatePose50KG` and its existing rotation callers are unchanged. Directional restore does not call it.

## New API and ownership

- `OriginalUiRiderPodOrientation50KG` contains two XYZ arrays only. It owns no graph pointer, source material, cache row, renderer packet or decoded asset. Captured values remain usable after actual outgoing pod destruction.
- `captureOriginalUiRiderPodOrientation50KG` reads the supplied live root's LOCAL look and up. It validates graph topology/index, then copies their bits without arithmetic or world refresh. Nonfinite values may be captured exactly; arithmetic admission is separate.
- `originalUiRiderPodOrientationPose50KG` is a pure, atomic pose builder. It normalizes Up and Look separately, in source order, and computes Right = Up cross Look without correcting either supplied axis or normalizing Right. The builder is available for failure-prone numeric admission before destructive callbacks; it does not alter the snapshot.
- `restoreOriginalUiRiderPodOrientation50KG` snapshots XYZ from the incoming root's actual local translation immediately before building its new pose. It never substitutes outgoing translation or the cold asset config. It applies the result through the existing `setLocalMatrix49P`, which owns the recovered flag propagation.
- `OriginalUiRiderPodScene50KG::captureDirectionalOrientation` and `restoreDirectionalOrientation` bind those operations to the actual added model root, node zero. Both reject an absent/destroyed model. Capture is logically const. Diagnostics identify `28A530` or `289EC0`.

These are graph operations, not attachment/selection/fence capability tokens. `loaded()` alone does not prove backend attachment, completed SELECT, no outstanding CPU borrow, or retired GPU work. The eventual owner must establish those facts. No fabricated readiness Boolean or success callback was added.

## Original evidence and exact branch

The original ELF is `/workspace/shared/recovery-20261010/native-assets-prepared-01/work/native/project/disc/SLUS_202.68`, SHA256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`. Eight relevant ranges were copied directly as static words into `ORIGINAL-WORDS.txt`; their exact byte identities are recorded in `ORIGINAL-RANGES.json`. Mapping is file offset = VA - 0x100000 + 0x80. No instructions were executed or interpreted.

1. Next `1EB208/1EB210` and previous `1EB388/1EB390` initialize two source stack vectors with XYZ zero/W one. The capture calls are `1EB234` and `1EB3B4`. They resolve current control -> first authored widget -> widget+60 actual scene.
2. `28A538..28A54C` copies local+E0/E4/E8 to saved Look; `28A558..28A56C` copies local+D0/D4/D8 to saved Up. This is not a world-space getter and neither copies outgoing position nor refreshes caches.
3. Final incoming control SELECT is `1EB304` / `1EB484`. After that call, source resolves its first widget again and invokes `289EC0` at `1EB338` / `1EB4B8` with a3=0, t0=1, t1=0.
4. Because t1=0, `289EF4` goes to `289F38`; source captures incoming local+F0/F4/F8 at `289F3C..289F54`. Because a3=0, `289F90` skips all cross-correction at `289F9C..289FC8`.
5. `289FCC..289FE8` computes the Up squared norm as separately rounded X square plus Y square plus Z square, obtains reciprocal square root, and scales Up. `289FEC..28A008` repeats for Look. `28A00C..28A010` forms Right = Up cross Look. `28A014..28A028` explicitly clears all basis W lanes and sets translation W to one. `28A02C..28A038` writes the complete local matrix.
6. `28A03C` calls `28C4F0`: set bit80 on the target and every represented ancestor, preserving other flag bits. `28A044..28A074` checks target ready01; only when it was set does source clear it and invalidate children through `289A20`. That helper visits a node's next sibling first, then clears/descends through that node only when it was ready. Dirty nodes deliberately skip their descendants. World matrices remain cached and unchanged until a later getter.

The existing `scene_graph49p.cpp` `setLocalMatrix49P` -> `afterLocalWrite` -> `markAncestors` / `invalidate` implements precisely that represented post-write control flow. No duplicate approximation or eager recursive world refresh was introduced.

## Source-global scope gap

`289ECC..289EE8` also lazily initializes a process guard and source global4433D0 to -1 before the pose branch. No later instruction in this routine reads that global. This package does not invent a native global owner for it or claim whole-process equivalence. The original initial pod load already calls289EC0 before directional movement, so the directional path may normally see a warm guard, but that is not asserted from native `loaded()`. Its owner/consumer role, if relevant to broader admission, remains to be established separately. The staged APIs deliberately claim represented graph/math behavior only.

## Native arithmetic and rejection boundary

The existing User_Rotate native scalar primitives are reused: separate volatile binary32 multiplies/adds/subtracts and host `std::sqrt`, with FE_TONEAREST required for restore. This is native math, not PS2/VU emulation. Physical PS2 reciprocal-square-root bit equivalence, denormal modes, overflow/underflow handling and host FTZ/DAZ are not certified by this stage. Existing helper policy was retained rather than silently replaced.

Restore rejects nonfinite XYZ input, zero/nonfinite squared axis lengths, nonfinite normalized vectors, invalid topology/index, or an unsupported host rounding mode. Those are native admission restrictions, not recovered source rejection branches. Nonzero parallel axes are allowed: source's a3=0 branch can generate a zero Right axis, and this stage does not add orthogonalization or a determinant gate. Capture preserves signed-zero/opaque bits; pose basis W is canonical zero and incoming translation W is canonical one, as required by this branch.

Capture and the pure builder assign output only after completion. Restore does all validation, translation capture and arithmetic before `setLocalMatrix49P`; that existing setter validates before the first write and cannot fail after its local write in the admitted serial domain. Thus failed calls leave the entire represented graph unchanged. Model-scene methods only change the supplied diagnostic on rejection.

## Required future call chronology

This stage is safe to integrate only as inactive operations. It does not close directional admission.

1. Resolve and validate the currently selected real pod, capture before any outgoing deselection, and use the pure builder to preflight numeric admission with an authenticated candidate position. Revalidate the actual prospective owner/graph assumptions at the transaction boundary.
2. Complete all other cold-resource, command-capacity, profile, text, audio and video/renderer preflight before source-visible mutation.
3. Execute actual outgoing deselection and lifetime retirement, page cursor/PodIndex publication, appearance and video suspension in the recovered order. Do not retain outgoing graph/cache/PSM lifetimes merely to keep an orientation snapshot.
4. Execute final incoming SELECT exactly once. After its real model load/attachment and source callbacks succeed, restore the saved axes on that incoming pod, preserving its live local position.
5. Continue source deferred work, sound2 and transport chronology. No step in this package supplies them.

The actual owner must prevent simultaneous graph mutation or outstanding mutable graph/packet borrows. If scene/cache/renderer work can fail after source callbacks begin, poison and drain through that owner's real cleanup path rather than undoing side effects or replaying callbacks. Source registration and material lifetime epochs remain with their existing owners; these functions do not touch them.

## Source-review cases for the next authorized validation window

No checks below were executed. They describe the distinct outcomes an authorized native-only harness should establish:

- Outgoing position A, incoming position B: resulting translation must be B; snapshot contains no A.
- Deliberately nonorthogonal but nonzero Look/Up: keep each independently normalized; do not match the cross-correction helper's different result; do not normalize resulting Right.
- Incoming old basis W/translation W arbitrary: new basis W zero, translation W one.
- Target ready: target/ancestors get80; target ready clears; siblings are visited before current child; ready descendants clear, already-dirty nodes stop descent.
- Target already dirty with an artificially ready descendant: descendant stays ready, matching source's gated path rather than repairing it.
- Cached world matrices and graph links are unchanged immediately after restore; unrelated flag bits survive.
- Missing/destroyed pod, bad root, invalid topology, unsupported rounding, zero/nonfinite axes, arithmetic failure: no graph/output mutation.
- Capture, destroy outgoing graph, load a different incoming graph, restore: no outgoing graph/material/reference is retained by the saved value.

## Verification performed and exclusions

Performed: source/README/build-guide reading, static original byte extraction/hashing, textual patch review, and source/preimage hash comparison. All original-byte work uses Python standard-library byte reads, integer formatting and hashing only. Source staging uses text operations, not a C++ parser.

Not performed: builds, tests, compiler/syntax checks, project parsers/importers/decoders, asset loading, GL probes, native or guest launches, emulator execution, sanitizers, runtime pose comparisons, renderer fences or audio work. No compilation, runtime correctness, activation or complete movement-readiness claim is made.

`SOURCE_PREIMAGES.json` records the four exact baseline and staged source identities. `DEPENDENCY-SHA256SUMS` pins the unchanged graph/rotation ownership context used in review. `SHA256SUMS` pins the delivered package members. Independent source review is complete in `INDEPENDENT-REVIEW.md`: no blocking defect found in the represented helper/native domain. Its limits match this report; it supplies no activation or runtime admission. `PREIMAGE-VERIFICATION.json` confirms that all four exact preimages also match the externally advanced current worktree at7a3e42280b85abbb691792761d5967225f13460f.
