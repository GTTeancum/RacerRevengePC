# Independent source review: private blend publication and incoming promotion

Reviewed 2026-10-11. Scope: staged `PATCH.diff`, both staged renderer files, and both exact preimages from `f513a1f207a6eaf0b155fc853551a9f26529513a`.

## Result

No blocking defect or compile-level error was identified by source reading. This is a source-review conclusion only. No compilation, syntax check, test, sanitizer, GL probe, gameplay check, parser/importer/decoder, project execution, guest execution/emulation, or Git publication was performed. The active checkout and driver dependencies were not changed. Textual comparison found both staged preimages identical to their specified commit versions.

## Publication authority and invalidation

- The private receipt is renderer-issued and carries the renderer's weak lifetime identity, a nonzero private publication generation, viewport dimensions, and immutable lineage observations. Use checks also require the current committed UI target. Historical receipt getters do not independently grant authority.
- Every ordinary committed-image path invalidates private publication authority: source publication in `publishUiSource`, race commit in `Storage::commit`, ordinary RGB UI commit in `submitOriginalUiImpl`, and host-menu commit in `submitHostMenu`. These invalidations happen at private commit, including when a later ordinary window copy fails.
- A new private publication supersedes an earlier private generation. Cleanup clears private authority. An actual private completion failure clears authority without deleting raw promoted ownership.
- Source-only unpublished draws, source-owner retirement, texture-bank replacement, later blend samples, and blend retirement leave the independent opaque image untouched. They do not need to invalidate its receipt. A failed ordinary pending image likewise does not replace the previous committed image.
- Private completion requires the exact current receipt and a checked `glFinish`. Receipt-bearing private readback additionally requires the completed form. Distinct completion input/output storage is enforced.

## Failed copy and retry

- Private copies use `uiTargets[uiPublished ^ 1]`; publication indices, displayed target, committed dimensions, serial, and receipt are changed only after the copy and baseline checks succeed.
- A failed copy can affect only the pending opaque UI surface. The current UI target, or a currently displayed independent race/menu target, remains the previous committed image. The next attempt clears and overwrites the entire pending presentation surface.
- Failure returns empty new outputs, preserves the immutable pair and old borrow, and performs no incoming transfer. An existing raw promoted owner rejects another promotion before copying. Sample publication remains independent of that owner.
- The same caller variable used as an output is reset on failure, consistent with the surrounding receipt APIs; preservation concerns renderer ownership and prior publication state, rather than a promise to retain an overwritten output variable's value.

## Promotion and raw RGBA ownership

- Promotion copies directly from the entire immutable incoming `640 x 448` image made and actually completed at blend begin. It does not substitute an alpha-128 effect sample or rerender the reusable ordinary source target.
- Following successful opaque-copy acceptance, the transfer is only a non-fallible CPU move of the incoming framebuffer and texture names. Both blend names are zeroed; the promoted owner receives the original completed source lineage and the successful blend generation.
- Blend/sample authority is invalidated immediately after transfer. The old snapshot borrow and remaining effect objects stay with the blend until explicit retirement. Retiring that blend cannot delete the transferred names.
- Promoted raw readback authenticates the renderer lifetime and promoted generation without requiring the reusable ordinary source to remain current. Ordinary source draws, ordinary image publications, and source-owner retirement therefore cannot overwrite or falsely refresh the raw image.
- Successful blend generations increase monotonically and cannot wrap. A later promoted image uses a later successful blend generation. Close/recreate changes the weak lifetime identity, so matching numeric generations in another renderer cannot revive a stale receipt. Private publication serials likewise cannot wrap.
- A later blend may coexist with an already owned promoted raw image. Correct `glGen*` allocation supplies distinct live names. Adding explicit comparisons against promoted names would be optional defensive redundancy; their omission is not a correctness blocker under that GL contract.

## Completion, presentation, and alpha separation

- Private publication acceptance is recorded separately from source acceptance and from actual GPU completion. The private publication entry points call neither the default-framebuffer check nor `copyToWindow`; they do not report back-buffer modification, host swap, or display completion.
- Promotion's raw copy was completed at begin; its new opaque publication remains uncompleted until the explicit private completion call succeeds. Failure of that later completion does not remove the independently owned raw image.
- The copy helper writes only the independent opaque UI target. It blits the full incoming/effect image, then forces destination presentation alpha to 255 without writing source/effect alpha. Letterbox RGB is captured from the incoming source's clear color at begin and cannot be silently replaced by a later ordinary source's clear color.
- The full incoming borders are promoted from incoming storage. Effect samples retain their existing old-border semantics. No new sample rendering or source/page acceptance is performed by publication.

## Deletion and ordinary paths

- Promoted retirement checks idle/current state, checks outstanding completion, invalidates the raw receipt before deletion, restores bindings, and uses the existing conservative deletion helper. A failed deletion retains unresolved names, reports incomplete cleanup, and prevents a replacement promotion until they are retired.
- The documented no-name-reuse obligation after ambiguous deletion remains necessary. This is the same caller/context obligation used by existing retained/blend cleanup; it is not newly guaranteed by a generation counter.
- Close includes promoted names in resource accounting, the completion obligation, and deletion. Transferred names are deleted once, separately from the zeroed blend incoming names. A failed close preserves unresolved ownership for close retry.
- The old source-publication GL copy and post-copy baseline sequence is preserved in the extracted helper. Ordinary publication/window-copy ordering is unchanged; new invalidations are CPU-only. New APIs introduce no new ordinary-path shader loading, allocation, transition scheduling, driver integration, or source execution.
- Source reading found matching declarations/definitions, existing required includes and GL entry points, appropriate receipt friendship, bounded viewport/readback sizes, and no evident new C++ type/access error. This does not establish build or runtime success.

## Reviewed source identity and report wording

The source SHA-256 values independently read at review completion match the supplied preimage/candidate manifests:

- Preimage header: `d9bd702bd3fc942eab5a51b16896a188ec894985eb57f2904aca3f8432397ac9`
- Preimage implementation: `95cabbc3e263354acd458274af27bdf82ffa6a9a31841560ef98762b9586e04e`
- Candidate header: `b2aed242939f7424012f4f667151adf9b6327e03fac7b6367cb0db0aa804c793`
- Candidate implementation: `36509bad1517daa131bd10230f977d1074633296f689465e9efdf34eca717f08`
- `PATCH.diff`: `64cc74675bbd86de6ffcc909f127b9d388dc505dd18cb334a00d056f516398f7`

`REPORT.md` was also read. Its renderer-only, dormant, unintegrated scope; accepted-versus-completed and private-versus-host distinctions; immutable raw ownership; failed-copy preservation; and remaining execution/activation requirements agree with the reviewed source. No report correction is requested. Source hashes are integrity evidence, not runtime evidence.
