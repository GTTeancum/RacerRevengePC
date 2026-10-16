# Stage 14 source review

Reviewed the supplied `0001-Connect-finite-Tournament-fallback-refs-to-actual-te.patch` against the integrated stage 13 sources. Observed root HEAD: `ea8741916b7a07c55f9f55339226f6f164eb805f` (later than the announced `0b6d98b`). No implementation edits, patch application, checks, tests, builds, imports, scripts, launches or original execution. Original evidence was verified by static word reads only.

## Verdict

No definite blocker found by source inspection. The new operation reaches a real canonical texture resource through the retained fallback reference and first pass map token. It performs one source-specific demand floor visit, with honest write/no-write diagnostics. It does not synthesize resource population, residency or frame readiness.

## Identity and binding

* Public callers cannot supply a texture pointer, floor value or store PC. Acquisition records one immutable store PC from the finite admitted kind/object slot: portrait `1F24FC`, course icons `1F2564`, award slots `1F25AC/1F25E4/1F261C`, notch `1F2690`.
* A raised demand requires the same retained reference, same application/APPSTART/template backing, positive actual numeric material row reference, actual ScreenMT association, same populated texture table, same row-pinned texture table and same row-pinned authentic archive object.
* The material repeats exact texture-table/archive pointer checks, requires constructed state and a live sealed construction context, rejects authored PSM ownership, requires one actual pass and retained fallback provenance, and checks its origin against the source caller family. For trophies it checks the actual immutable icon/award name family. The six store PCs are the original sites verified in `tournament-material-update.txt` and static executable bytes.
* The retained absence proof is rechecked. The PST identity must equal the provenance's authentic `fallbackTexture` identity; data and catalog byte count must be present and consistent. Lookup uses the already-owned provenance member name; archive lookup intentionally recognizes its forward-slash spelling.
* The actual pass must have one map, a nonnegative signed map1A token and no animation. `resolve298E60` resolves that token through the same live canonical texture table, and independently rejects active or terminal texture population. A pending null row fails. The actual populated resource's complete retained original bytes must match the authentic retained PST before mutation.
* The fallback recipe's pass index is safe here because stage 13 publishes `constructed` only after the private proof has exactly one immutable pass. Neither callers nor later demand code can resize that proof.

## Floor and effects

The resource helper is private and reachable through the existing canonical material friendship. It validates the finite six-site store whitelist, real constructed resource, positive resource refs, and live retained construction context. It uses the existing binary32 admission helper to reject unsupported host floating-point mode, special values, subnormal values and negative demand before mutation.

For admitted values it executes the literal source comparison/store: if `10.f <= demand10` is false, assign exactly `10.f`. Values already at or above ten remain unchanged. Negative zero is admitted and becomes ten. This is not projected-size conversion `104A00`: there is no logarithm, size cap, rounding or derived scalar. Rejecting exceptional numeric inputs is a narrower native domain than the original comparison, whose NaN behavior would take the store; the patch correctly does not claim those inputs.

All potentially fallible lookups, ownership/context checks, byte comparisons and arithmetic admission precede the sole scalar write. The subsequent primitive diagnostic assignments cannot fail. Allocation in the member-name lookup is caught before the write. A successful visit reports its actual store PC, pass/map zero, maps visited one and textures demanded one; `textureDemandWritten` and `sourceEffectsCommitted` are true only when the scalar changed. The already-high case is a successful visited resource without a fabricated write.

A failed visit writes no demand. A traversal prefix can nevertheless contain earlier successful real writes. Each public/table call resets its own diagnostic, so a session must accumulate the prefix explicitly rather than overwrite the prior effects with the last visit's failure. This API does not automatically mark the full session terminal or roll back previous visits; caller terminal cleanup remains required by the documented contract.

## Lifetimes, API and integration

The new demand methods are private behind the real reference/table/object/resource owners. Their declarations, existing friendships, enum origins, runtime/diagnostic fields, archive lookup and canonical resource methods match current source definitions. No syntax or compile check was performed. No new translation units or build-list entries are needed.

Numeric reference release behavior is unchanged from stage 13: every alias retains and releases separately, final material release invalidates collectors and destroys pass texture refs, and archive/table/context pins protect the borrowed identity while work is admitted. Callers still own actual renderer retirement before release.

This supplies real per-reference fallback demand consumers. It does not itself connect the retained stage 10 traversal lease to a complete current-generation session. The session must preserve original order and duplicate visits, actual trophy visibility4050, actual notch counts3D60, profile generation/phase gates, the mounted source-provider boundary, and retirement. No popup gate is invented in material update. Marker/unique ATM and inherited `141C30` traversal remain unresolved. A successful demand is not a texture-upload, residency, rendered-frame or frontend admission receipt.
