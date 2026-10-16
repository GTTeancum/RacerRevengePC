# Ryloth assembly source review, 2026-10-14

Independent source-only review of `tools/assets/assemble_ryloth_runtime.py`,
`ryloth-visual-assembly-20261014.md`, and the invoked pinned decoder/readers.
No execution, import, syntax check, test, build, converter, gameplay, profile
change, unlock, or commit was performed. Shell reads and dependency hash reads
were used only to inspect source.

## Findings

1. **Output containment defect reported:** the initial script resolved input
   roots but used `output_root.absolute()` for lexical containment checks.
   A path containing `..`, such as
   `/tmp/../workspace/shared/rr-recovery-git/new`, can represent a repository
   descendant without containing the resolved repository path in its lexical
   parents. This violates the promised separate-output boundary. Reported to
   root and author; normalize the output path with `resolve(strict=False)`
   before checking it and use that normalized root for every write, or reject
   dot-dot path components explicitly. Final resolution is recorded below.
2. **Geometry blocker refinement:** root's authenticated original inspection
   establishes that RA/RC scenes are single-node and the existing signed24
   decoder can handle the 583 previously unsupported RA scenes. The remaining
   blocker is actual reviewed native regeneration and RRSCENE admission, rather
   than an established need for a new geometry decoder. The assembler properly
   fails on any still-unsupported present member and cannot consume retained
   unsupported status as if conversion had occurred.
3. **Runtime availability remains closed:** the current track visual, source
   texture, sky and animation fixtures do not admit RA/RC. Generated include
   fragments alone cannot make them available. New reviewed per-track fixture
   profiles and actual session/camera/material/animation ownership remain needed.

## Provenance and input closure

- The exact RA/RC archive bytes and hashes are independently checked. Full48F
  ZIP identity, generation member identity, retained reference identity and
  container extent are checked, not inferred from directory or completion names.
- The manifest must exactly equal the canonical member inventory regenerated
  from that authenticated generation. All intake members are authenticated by
  extent/hash before output creation. Normalized duplicate identities are fatal.
- Native import report and output-index identities are supplied explicitly by
  the reviewing root. The index binds track, archive and report digest; every
  indexed regular nonsymlink file is verified by extent/hash. Every source copy,
  RRSCENE, RRMESH, RRTEX and listed native mip referenced by the report must be
  in that index. Source copies, offsets and extents must agree with intake.
- This deliberately trusts the root-reviewed native output as conversion
  provenance. Hashes alone do not prove source-to-conversion correctness; root
  must review the authorized importer/decoder and generated report/index.
- All seven declared dependency digests match the inspected current source
  files. Their transitive project imports are covered by those pins. The script
  does not build or run a native importer. Future scalar IPUM conversion calls
  the installed `ffmpeg` through PATH; root should retain the actual binary/version
  provenance when it performs authorized preparation.

## Path and format admission

Inventory/native relative paths reject empty components, dot/dot-dot components,
NUL, backslash and colon. Reads reject symlink files and ancestors and enforce
individual/aggregate byte caps. ZIP files are read in memory by exact names,
without extracting paths; duplicate requested ZIP names and oversize entries fail.
Output is required to be new and its parent real; the lexical containment issue
above is the outstanding initial defect, not an input traversal defect.

The reused RRSCENE reader requires version1, one root node, identity-palette
flag0, one or four LODs, bounded material/surface/index/vertex counts, finite
vertices/matrices, exact LOD0 span order and companion bytes, and complete file
consumption. It preserves all scene LODs in cached records and exports placed
LOD0 geometry. Local geometry uses the original SCB root replacement; world-space
geometry bypasses that root. Export arithmetic is disclosed as ordered binary64
multiply/add followed by binary32 storage, not original-VU bit parity.

## Exact asset closure

- Every present referenced PSG must have a converted native geometry row.
  Original absence is retained as explicit non-drawn placement metadata, never
  used to hide a present unsupported member. Full decoded world/reference
  equality covers provenance fields, transforms and placement order.
- Every material named by every admitted scene is required from the same exact
  archive. All authored passes, programs, fields and texture slots survive the
  material record; no default material or pass dropping occurs. Runtime program
  compatibility is explicitly still required.
- Every referenced PST is parsed completely with supported version/kind/mip
  framing, bounded dimensions, complete RGBA extents and all mip objects. Indexed
  base images are compared with reviewed native decode. Kind2 uses authentic
  source IPUM decode, with the inherited disclosed FFmpeg IDCT limitation.
- An absent PST can join only its exact same-archive ATM. Such texture resolution
  remains explicitly unresolved in the ordinary static bank and records the
  separate exact source initial-frame owner. It is not falsely marked as a PST.
- All original ATM members, including dynamic-only material users, are retained.
  Authored starts and fps must be present; inconsistent starts fail rather than
  inventing a shared owner. The full bank exports every source frame's largest
  mip and retains frame/encoded/native identities and original material timing
  bindings. It does not claim every smaller mip or live playback. Per-frame
  native dimensions/bytes are validated by the inherited decoder.
- Sky binding requires the exact original CUB named by the exact environment.
  CUB framing checks all48 original tiles and complete input consumption; palette
  counts are track-specific226/126. Per-zone cube0/1 authored fields and original
  default/subtract-one semantics are retained. Fog remains exact per-zone authored
  fields. Camera-zone visibility is a future actual runtime join.
- Fixed original draw/missing-geometry/collision-presence counts and native
  vertex/index/texture/draw-span budgets are checked. Collision presence is an
  inventory closure check, not a collision conversion or runtime proof.

## Completion and integration

Only the final `ASSEMBLY-COMPLETE.json`, emitted after sky and full ATM banks,
marks successful preparation. Partial output may contain plausible selection,
mesh, manifest and include files before a later failure; consumers must require
the final sentinel and independently authenticate generated artifacts. The
completion explicitly states `gameplay_ready=false` and `game_launched=false`.

Still required: reviewed native regeneration; generated output validation/pins;
RA/RC material program/pass support; exact track-specific fixture profiles;
original camera-zone sky selection and live ATM ownership; collision/navigation
and genuine selected-track lifecycle. No source review here establishes a ready
Ryloth course or authorizes a launch/unlock.

## Final resolution

Author corrected output containment. Final source reread confirms explicit
dot-dot rejection, original-parent nonsymlink checks, strict existing-parent
resolution, canonical nonexistent-target recheck, containment comparison with
resolved inputs, and reassignment of `args.output_root` to that canonical path
before all writes. The original reported escape is closed in source.

Author also updated geometry documentation to the authenticated single-node /
existing signed24 finding and correctly retained native regeneration as the gate.
No further concrete defect was found in the reviewed source. This is not
execution or readiness validation.
