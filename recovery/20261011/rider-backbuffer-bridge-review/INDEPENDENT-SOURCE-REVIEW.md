# Independent source review: dormant private-publication BACK-copy bridge

Date: 2026-10-11 UTC.

## Findings first

**No blocking source-level defect found in the pinned two-file candidate.** The change satisfies the bounded renderer-only slice in section 2 of `../native-blend-scheduler/PLAN.md`. This is a source-review conclusion, not a compile, test, GL behavior, presentation, or active-admission result.

The bridge remains dormant. It authenticates a completed current private publication, issues a full recorded-extent default-BACK blit, and returns only a historical command-acceptance observation. It does not authorize a later swap or establish copy GPU completion, swap submission, native presentation, scanout, or transition completion.

The existing publication and execution hold is unchanged. The future exclusive host owner still has to verify the live drawable dimensions, join an accepted copy to the correct immediate host operation with no intervening writes or duplicate use, and provide the separately reviewed native pacing/event and interruption contracts. This source slice alone admits no movement direction or scheduler.

## Scope and evidence pin

- Active checkout: `/workspace/shared/rr-recovery-git`.
- Observed checkout HEAD: `6dc2c8b1b3064f8fdeb5f12648fe4583119c1b9e`.
- Candidate and exact preimages: this report's sibling `candidate/` and `preimages/` trees, each containing only `platform/linux/native/vehicle_window_gl50ds.h` and `.cpp` when reviewed.
- Both preimages match the corresponding files read directly from the pinned Git object and the unchanged active working-tree files. A read-only name diff of those working-tree paths against the pinned commit was empty.
- Full unified diffs, relevant neighboring source, declaration/definition references, and SHA-256 digests were read. No candidate source was changed by this reviewer.

SHA-256 pins:

| Reviewed input | SHA-256 |
| --- | --- |
| Candidate `vehicle_window_gl50ds.h` | `7d8ebccdacffd555bcdadd1ae8eacbe4f0db64f0b5cc6ce11f7633aee85c29f8` |
| Candidate `vehicle_window_gl50ds.cpp` | `286d768c9fb979344e29384ebce542f1b057f89e3585d1e603be4e6b5fd4aaba` |
| Preimage/pinned baseline `vehicle_window_gl50ds.h` | `b2aed242939f7424012f4f667151adf9b6327e03fac7b6367cb0db0aa804c793` |
| Preimage/pinned baseline `vehicle_window_gl50ds.cpp` | `36509bad1517daa131bd10230f977d1074633296f689465e9efdf34eca717f08` |
| `../native-blend-scheduler/PLAN.md` | `bc0f41c5785e1f34e88ae5b8bd141806d9e715ee73ac18cf68ca1524fa1b7d33` |

Line references below refer to these candidate hashes. Any later source change requires reconciling this review with the new bytes.

## Contract review

### 1. Private issuance, copied lineage, and historical meaning

- Header lines 648–673 define the output kind and receipt. Only the renderer is a friend of the private issuance fields. A default value has kind `none`, sequence zero, and `commandsAccepted()==false`.
- The receipt contains a weak renderer lifetime, the entire authenticated private-publication value, an independent copy sequence, and an explicit `windowBack` kind. Width and height come directly from the copied publication, preserving all image identity, source/blend/sample lineage, alpha where applicable, and the original private completion observation.
- Header lines 651–657 expressly distinguish the completed private image from the later accepted BACK copy. The receipt is historical and does not pin GPU pixels or the renderer, confer current swap permission, or certify display/completion. Keeping `publication().completed()` therefore reports the prior private operation only; there is no new BACK `completed` field or completion claim.
- The new sequence in source lines 1002–1004 is separate from source, sample, and private-publication generations. It is incremented only after the full helper success path. Overflow is rejected before any new GL call or BACK write.
- There is no live-currentness query to invalidate incompletely across older window-write paths. That omission correctly preserves the explicitly historical scope required by the plan.

### 2. Admission and authentication precede GL work

- Source lines 4624–4627 clear diagnostics, reject the exact `publication == copied.publication()` storage alias before clearing the output, and clear the output for every non-alias attempt. This follows the neighboring private-completion convention at 4603–4607. An aliased call preserves the preexisting historical output rather than destroying its embedded input and issues no new observation. Callers must honor the false result rather than reinterpret that retained value as a new operation.
- Source line 4628 calls `originalUiSourceRgbaAvailable()`. Its existing definition at 4171–4175 requires initialization, explicit source-RGBA capability, the supported fetch/barrier path, and the actual source resources.
- Source line 4629 calls `Storage::current`, whose unchanged implementation at 1051–1055 checks the creating thread and the supplied current-context callback. The callback's existing contract in header lines 232–236 requires the exact live context and window drawable, and forbids GL calls, reentrancy, or context changes inside the callback.
- Source lines 4630–4637 reject active/frame-set/in-progress diagnostic state, non-window output, incomplete publication, failed private-publication authentication, nonpositive/out-of-bound viewport, and exhausted sequence before setting the operation diagnostic and querying GL state.
- `matchesOriginalUiPrivatePublication` at 4531–4537 authenticates a live matching weak lifetime, nonzero exact current private generation, an actual committed image, the selected private UI target, and dimensions equal to the committed publication. Numeric diagnostics cannot substitute for the private input type.
- The explicit `defaultBackBuffer` guard rejects HeadlessLive/offscreen output. The existing cold window check at 2967–2985 requires desktop GL and a nonstereo, double-buffered, nonmultisampled window framebuffer. The bridge does not create or alter a host/context.

### 3. Exact extent and unchanged ordinary paths

- The only change to the existing low-level helper is an overload: `copyToWindow()` forwards its original working `width,height` to `copyToWindow(I,I)` at 3682–3683. All existing ordinary call sites remain unchanged, with the same source target selection, GL operation order, error checks, and dimensions. No ordinary-path allocation or extra GL operation is introduced.
- The bridge explicitly calls the dimensional overload with `publication.width_, publication.height_` at 4645. Both source and destination rectangles in the helper use those parameters, with the existing color-only NEAREST blit at 3692.
- This distinction matters: the existing ordinary vehicle submit writes working `s.width/s.height` at 4100 before a potentially failing submission, then clears frame-set/diagnostic state at 4127. A failed submit need not replace the previous committed image or its dimensions. Using the authenticated publication dimensions avoids silently copying the later working extent and does not rewrite those working dimensions.
- The renderer has no live drawable-size getter. Header lines 817–820 state the future host's required equality check explicitly. The bounded viewport is therefore an exact recorded publication extent, not an independent guarantee that a resized current drawable has the same extent.

### 4. Error paths, baseline, and ownership

- Source line 4639 rejects pending GL error before invoking the existing default-framebuffer helper. The helper at 2893–2903 selects framebuffer zero/BACK, checks GL errors, and requires framebuffer completeness. It makes no pixel write.
- The reused copy helper at 3683–3695 prepares the source private read framebuffer and destination BACK draw framebuffer, checks preparation errors, marks `defaultFramebufferMayBeModified` immediately before the actual blit, checks its error result, then binds framebuffer zero and checks that restoration before reporting success.
- Preparation failure occurs before setting the potential-modification flag. A blit error or framebuffer-restoration error retains the flag and returns false. The bridge preserves that diagnostic and applies the existing `gl` to `presentation` error classification for copy-helper failure at 4646.
- Every non-alias failure leaves a default output receipt. No failed attempt increments the successful-copy sequence. Diagnostics can truthfully indicate a potentially modified BACK even without an output receipt; the documented response is not to swap.
- The bridge does not assign the selected private target, committed dimensions, private publication generation, source/sample generations, blend state, retained-old ownership, promoted-incoming ownership, or raw GL object names. It reads only from the selected private framebuffer and writes only to default BACK. Neither success nor failure invalidates the prior private receipt or changes its completed state.
- The operation's only persistent CPU mutation on success is the independent copy serial; the output stores copied observations. The operation diagnostic pointer is cleared on all paths after installation. There is no allocation, deletion, new texture/FBO, shader, finish/fence, swap, sleep, clock, event operation, or ownership transfer in the new method.
- Baseline restoration before successful return is the checked framebuffer-zero restoration performed by the existing helper. Failure does not falsely promise restored GL bindings or undamaged BACK pixels. The unchanged renderer contract excludes unrelated external state changes; the existing UI draw cleanup also resets its scissor/blend baseline on draw failure at 5126–5135.

### 5. Dormancy and scope

- Header declaration at 827–828 and source definition at 4622–4623 agree by source inspection.
- The full two-file diff contains only the receipt/type and API declaration, independent serial, explicit-extent helper overload, and dormant bridge definition.
- Source-reference search found no existing bridge caller in the active `platform/` or `tests/` trees. Candidate references are confined to the new type/declaration/definition and private issuance.
- No host, frontend, audio, recorder, build-list, movement-gate, race-input, physics, controller, or clock candidate is part of this slice. No current caller or gate was edited by this review or appears in this diff.

## Remaining host boundary and validation limits

The bridge does not fill the separate host obligations described in the pinned plan: real native presentation/event capability, exact host/window/context lifetime and live-size verification, one independently justified pacing boundary, authenticated copy-to-host sequencing without intervening BACK writes or duplicate use, interruption/resize/failure invalidation, and truthful separation of private completion, BACK command acceptance, host swap submission, and display feedback. The plan records that the current active runner requests interval zero and SDL's void swap certifies submission only; no event-paced transition is admitted by this review.

No build, compiler/syntax invocation, test, sanitizer, gameplay launch, GL probe, project parser/importer/decoder, native project execution, emulation, or guest instruction was performed. No active checkout file was edited, no Git commit/push/publication was performed, and no denied action was retried. Only source reads, source/reference comparison, hashing, and this outside-checkout review artifact were used. Runtime failure cases remain unexecuted under the existing hold.
