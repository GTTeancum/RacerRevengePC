# Independent Environment sky owner review

2026-10-14. Reviewed integrated commit `cf93304`, specifically
`src/game/original_world_environment50kg.{h,cpp}`, against the real
`src/assets/import_world48f.{h,cpp}`, `original-world-environment-20261014.md`,
`portal-join-next-20261014.md`, and retained original instruction-field maps.

## Verdict

No concrete compile-by-inspection, API/member mismatch, ownership defect,
recovered-default error, or zone off-by-one error was found within this owner's
explicit bounded sky-selection contract. This is source inspection only, not a
build or runtime certification. Root owns integration and all validation.

No builds, tests, syntax checks, sanitizers, imports, project executables,
original executable execution, guest execution, or emulation were performed.
The only original-file operations were static byte reads and SHA256 calculation.
No source files or commits were changed by this review.

## Actual API and retained ownership

- `World48F.parameters`, `ParameterDocument48F.original_bytes`,
  `ParameterBlock48F.fields`, field `name/value/entry_offset/value_offset`, and
  `World48F.zones[i].zone` all exist with the types consumed here.
  `encodeParameters48F` and `validateWorldSemantics48F` have the called
  signatures and are in `rr::assets`.
- Decoder names are lowercase sorted ASCII. Literal `environment`,
  `numberofskycubes`, `cubefile`, and generated numbered keys agree with this
  representation; retail title-case names in explanatory prose do not imply
  a case-sensitive native mismatch.
- Encoding checks the bounded parameter shape and exact serialized content;
  world validation reconciles retained parameter fields with cached semantics.
  The additional contiguous zone check establishes that vector ordinal `i`
  corresponds to authored label `i+1`, rather than accepting an arbitrary
  reordered zone vector.
- Storage deep-copies the complete value-owned World48F before publication.
  No pointers into the caller's mutable world survive preparation. The local
  Environment/field pointers refer only to that new retained copy.
- The sole publication is at cpp line 111, after all checks and allocations.
  Early return and exceptions destroy the temporary owner; a failed first
  preparation can be retried. A successful owner rejects repeat preparation.
  Const getters expose retained source and rows for the owner's lifetime.
- Forward-declared Storage plus the out-of-line constructor/destructor is a
  valid unique_ptr ownership arrangement. Included headers cover the used
  types and operations. No compiler was invoked.

## Original semantics checked

The retained `276C00..277240` fields show:

1. `276C4C` passes NumberOfSkyCubes default 1; `276C60` branches over both
   name and zone allocation for a nonpositive count. Native admitted zero
   therefore leaves both arrays empty. Rejection of negative numeric spelling
   is documented narrower admission, not a recovered clamp.
2. `276C80` uses allocation size `0x104`, so native filename length `<260`
   leaves room for the terminator. This is not a 104-decimal capacity.
3. `276C94` formats numbered cube key `i+1`. `276CD8` branches on successful
   read and `276CE0` skips fallback for a nonzero ordinal. Only missing first
   numbered key falls back to unnumbered CubeFile. Native presence of literal
   `none` does not invoke that fallback, as required.
4. `276D60` formats zone key `i+1`; `276D7C` supplies default 1. The native
   authored upper-bound check corresponds to `276DD8..276DE0`. With admitted
   nonnegative values, `authored-1` cannot underflow signed32 and produces -1
   for authored zero, 0 for authored one.
5. `276E24/276E28` store the decremented zone rows before the single-name
   comparison at `276E40..276E6C`. The original then clears the sole filename
   pointer and count at `276E84/276E88`, retaining the zone-index allocation.
   Native clear of the name vector preserves the previously read rows.

The retained `275630..275990` fields show the actual camera chain
`renderer.currentCamera -> +38 -> +48` at `27587C..275884`; no subtraction
is performed on this camera ordinal. The selector directly indexes its row
with that zero-based value and does not remap to a racing/visibility zone.
A successful disabled result stays distinct from an invalid camera-zone error.
The output assignment follows all failure paths, so failed selection leaves
caller output unchanged.

Consequently, the documented RA authored labels 1/2/8 map to disabled camera
ordinals 0/1/7. RC's absent zone keys map through default 1 to cube index 0.
These follow the inspected algorithm; this review did not execute RA/RC or
independently decode their SCBs.

## Direct original-byte identity checks

Read-only `dd` slices from the supplied original SLUS file matched the retained
`.bin` hashes and range metadata exactly:

- VA `276950..276BC0`, file offset `0x1769d0`, 624 bytes:
  `055257f5ffbaabafd7da77e95600bd8ad432365df92898871c83818733d6bf5d`
- VA `276C00..277240`, file offset `0x176c80`, 1600 bytes:
  `a36faa9a42917bb197d169f7cb95c573ee95415954a1ce7a84dd7c9e19f2d2c7`

Instruction-field maps were read as static data, never executed.

## Boundaries root must preserve

These are existing contract limits, not blocking defects in this packet:

- The owner verifies consistency, not source/archive authentication. Its
  argument must remain the loader's same selected authenticated World48F.
  `validateWorldSemantics48F` is a cached-field reconciler, not a fresh full
  world decode or a substitute for cold-loader admission.
- Signed32 nonnegative canonical decimal spelling with optional plus is an
  intentionally smaller domain than the original base-zero reader. Likewise,
  1024 cubes and 256 zones are native admission bounds, not recovered source
  capacities. The broader original parsing domain was not newly certified.
- `enabled` expresses a logical Environment row selection. It does not prove
  that a SkyCube resource exists, loaded, or is usable. In particular, a
  multi-cube source can contain `none` names without the sole-none collapse;
  this owner correctly retains those names but cannot certify corresponding
  world3F8 children. Actual resource admission must be separate.
- A single native sky resource consumer must cross-check its authenticated
  filename and index 0, and reject unsupported multi-cube resource ownership.
  Do not feed it an arbitrary camera fallback or a track-derived Boolean mask.
- The original preparation has world4F0, Environment count, world3F8, and
  camera-zone gates, child flags33/29 updates, and descendant dispatch. This
  bounded owner does not implement those gates/flags/dispatch, fog blend,
  source scheduling, or queued portal/model emission. The current docs keep
  these responsibilities separate correctly.

Root-owned validation should cover defaults, exact first-key fallback,
explicit zero, out-of-range authored references, count-zero early admission,
sole-none retained rows, successful disabled versus failed selection,
immutable retained bytes and retry/one-shot behavior, followed by the actual
RA/RC callsite and resource cross-checks. No such validation was run here.
