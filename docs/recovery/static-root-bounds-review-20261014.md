# Direct-root static model bounds review

2026-10-14. Independent source-only review of the current uncommitted
`src/game/model_bounds49o.{h,cpp}` diff in `/workspace/shared/rr-recovery-git`,
against `/workspace/shared/rr-static-resource-owner-next.md`, the existing
HEAD numeric branch, and retained static field maps for
`26F4B0..26F780` and `28D940..28DEA0`.

## Verdict

No concrete blocker found in this additive numeric leaf. The new
`computeDirectRootModelBounds50KG` preserves own-root seed, aliased row0
projection using the exact ready rootWorld100, source order, finite/error
rules, and final output publication. The existing A2=1 numeric branch remains
unchanged in admission and arithmetic by source inspection.

No builds, tests, syntax checks, imports, sanitizers, project executables,
original execution, guest execution, or emulation were performed. No production
files were edited by this reviewer. This report does not certify runtime
behavior or a complete static observer/resource/world join.

## Source semantics and changed implementation

- Original ordinary-box `26F638..26F644` tests root own04.
  `26F654..26F6C8` then seeds minimum/maximum from the root's own140/150.
  The new shared helper retains the same `merge` before its source-row loop.
- `26F6D8..26F710` indexes actual1B8 in increasing source order and invokes
  `28DAF0` with root100 as the basis argument. The new helper iterates
  `i=0..count-1` and does not skip row0's box contribution. Direct admission
  requires own04 and identical row0/context flags and authored box bits.
- `workspace.world[0]=context.rootWorld100` is a true matrix copy. It neither
  multiplies row0.local by placed rootWorld100 nor substitutes an identity
  product. Thus row0 placement is not applied twice, and signed-zero lane bits
  in the actual ready root matrix are not changed by a needless product.
- After the copy, row0 enters the same point, direction, root-basis transpose,
  absolute-axis extent, and merge operations as later eligible rows. Own-root
  seeding therefore remains followed by the repeated root projection; it is
  not simplified away on the assumption that transpose means inverse.
- Later rows still use `local * workspace.world[parent]`, with the original
  binary32 operation order. Parent-before-child admission is unchanged, and
  the direct branch additionally excludes extra unparented rows after row0.
  With those bounded inputs every later row descends transitively from row0.
- Helpers, union operation order, center160, halfExtent170, squared-length and
  radius198 calculations, root flag result, and final validity check are all
  unchanged. The new branch does not bypass any projected-root overflow check.

## Error handling and publication

- Common admission still requires FE_TONEAREST, count<=64, non-null rows for
  nonzero count, no root sphere10, and a finite affine root matrix.
- All supplied locals remain finite-affine validated, including row0.local
  even though it is not multiplied. Only own04 boxes need finite centers and
  nonnegative finite half extents, matching the prior numeric contract.
- Direct count zero rejects before dereferencing row0; row0.parent must be -1.
  Flags and all three center/extent lanes must match the context exactly by
  float-bit comparison, so signed-zero inconsistencies are not silently erased.
- A copied root matrix is finite by prior admission. Every later world is still
  eagerly generated, including filtered rows, using Math's sticky valid flag.
  Projected centers/axes, merge arithmetic and final radius also use the same
  checked operations. Nonfinite intermediate failure cannot publish output.
- All admission checks precede workspace writes. Arithmetic failure may leave
  scratch workspace changed, as previously documented. Output is a local
  result until `if(!m.valid)return false; output=o;`, preserving unchanged output
  on failure under the documented disjoint-storage precondition.
- The branch remains allocation-free and noexcept. The new private helper is
  in an anonymous namespace inside rr::game; both public declarations match
  their definitions. Inspection found no member/type/include mismatch.

## Existing branch compatibility

`computeModelBounds49O` calls the shared helper with `directRoot=false`.
The additional direct admission block is therefore inactive. Every old loop
iteration takes the unchanged world-product path. Common checks, root seeding,
box selection, projection, merge, result arithmetic, error handling and final
assignment match HEAD. The old count-zero and multiple parent=-1 behaviors
are retained for this existing entry point. No change was made to
reset_model_bounds50ba or its racer-specific root04/10 rejection.

## Deliberate limits and remaining integration

This API validates a numeric row0 alias contract; it cannot prove canonical
model object identity or graph readiness from equal values. The eventual owner
must supply genuine source rows and the actual canonical ready rootWorld100.
The direct API deliberately admits 1..64 parent-connected rows with own04 and
no root sphere10. That is bounded admission, not proof that every original PSG
hierarchy shape or sphere route is supported.

The result is the root-local aggregate minimum/maximum plus160/170/198 numeric
production. Original28E230's later placed-world projection for observer bounds,
canonical publication, real19C initialization, world4C0 membership, source
resources and lifetime, and actual world parent remain separate work. No live
observer join or complete static visibility implementation is claimed here.

Root-owned validation should independently cover a translated/nonidentity
root, nonorthonormal finite basis (to expose retained repeated projection),
signed zeros, descendants with actual parent indexes, filtered intermediate
world overflow, admission failure and untouched output, and old A2=1 results.
These are validation recommendations only; none were executed by this reviewer.
