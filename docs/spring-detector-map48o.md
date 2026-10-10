# 48O: bounded original detector preparation map

## Scope and evidence

This is a source-only map of original `0x295630`, its point transform, the
optional reset, and query staging/dispatch gates. It does not run registration,
world traversal, the game, or any saved-memory replay. No guest RAM image was
read or reconstructed. It makes no current/live object-binding claim.

`evidence/springDetector48O/map/freeze_map.py` freezes 493 authentic instructions
(1,972 bytes) in seven ranges from the existing immutable ELF:

- `work/native/project/disc/SLUS_202.68`
- SHA-256 `c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30`
- Verified ELF32 little-endian PT_LOAD: file offset `0x80`, VA `0x100000`,
  file size `0x2D5A00`, memory size `0x34F600`; offset = VA - `0x100000` + `0x80`.

The adjacent `.bin`, `.txt`, and `original-words.json` files preserve raw words,
range hashes, readable annotations, and decoded branch targets. Prior 48K/48J/48H
instruction annotations are used only where their exact words match this ELF;
missing annotations use the small local opcode decoder. The original generated
source files need not exist. Existing 48N and earlier archives are untouched.

## Memory contract at the original entry

Original `0x295630` receives `a0 = detector`. The fields below are direct loads
or stores in the frozen instructions, not inferred current bindings.

| Object | Offset | Meaning at this seam |
| --- | --- | --- |
| detector | `+30` | Spring object pointer, nullable |
| detector | `+34` | Optional terrain provider pointer |
| detector | `+38` | Query object pointer, nullable |
| detector | `+40` | Center Vec4, overwritten only with pair-buffer preparation |
| detector | `+50` | Half extents Vec4, likewise |
| detector | `+60` | Mutable endpoint-pair buffer pointer; 32-byte pair stride |
| detector | `+68` | Nonzero enables the query path, together with nonnull `+38` |
| detector | `+6C` | Nonzero calls the spring reset after numeric preparation |
| detector | `+70` | Pointer to the candidate count, **not the count itself** |
| detector | `+74` | Candidate-pointer array; nonnull selects limited-list dispatch |
| spring | `+48` | Enables optional provider work when detector `+34` is nonnull |
| spring | `+7C` | Signed 32-bit persistent point count |
| spring | `+80` | Persistent point-record array; 96-byte record stride |
| point record | `+10` | Local point Vec4 input |
| point record | `+40` | Scene pointer for that point |
| scene | `+C0` | Local 16-float matrix, used by the dirty/no-parent branch |
| scene | `+100` | Cached world 16-float matrix |
| scene | `+180` | Parent scene pointer |
| scene | `+190` | Flag byte; bit 0 means cached world matrix is ready |

Original code supplies no buffer-size validation. Both point loops use signed
`slt`, reload spring `+7C`, and execute only while index < count. The initial
record pointer is read even for a zero count when the preparation gate passes;
record payloads are not read if the loop does not enter.

## Exact gates and ordering

1. `295644..29564C`: load detector `+30`; null jumps directly to the epilogue at
   `295A24`. Return value is always integer 1, including this no-spring branch.
   Neither static initializer, the reset, nor any detector-field write runs.
2. `295650..29566C`: read signed byte `gp-7360`. If zero, write `FFFFFFFF` to
   absolute `443590`, then write byte 1 to `gp-7360`. Any nonzero byte skips this.
3. `295670..295694`: enter point preparation iff
   `detector.pairs != null || (spring.providerEnabled != 0 && detector.provider != null)`.
   Otherwise branch to `295944`. Query presence/enabling does not gate numeric
   preparation.
4. `295698..2956F0`: read points; initialize temporary point to `(0,0,0,1)`,
   minimum XYZ to raw `7F7FFFFF`, maximum XYZ to raw `FF7FFFFF`, both W to 1.
   Branch to the loop test first, so zero/negative counts do not transform.
5. `2956F4..295858`: for each record, call `28BAB0(out, scene, localPoint)`.
   Optional provider work precedes second-endpoint storage and min/max updates.
   If pair buffer is null, skip endpoint/min/max updates and advance record.
6. `295864..295940`: only a nonnull pair buffer writes AABB center/half extents.
   Numeric preparation is complete at the instruction boundary `295944`.
7. `295944..295960`: read signed byte `gp-735C`. If zero, write `FFFFFFFF` to
   absolute `443598`, then byte 1 to `gp-735C`. Otherwise skip. These statics
   are observable global writes but do not modify detector or spring state.
8. `295964..295974`: if detector `+6C != 0`, execute `296490(spring)`.
9. `295978..29598C`: null query or zero enable skips to `295A24`, returning 1.
10. `295990..2959E8`: a nonnull pair buffer fills every first endpoint from the
    current detector center, then calls `11CA80(query, pairs, 0)`. A null pair
    buffer instead calls `11FF60(query)` at `2959EC`.
11. `2959F4..295A20`: nonnull candidate array calls
    `122660(query, *detector.countPointer, detector.candidates)`; null array calls
    `121920(query, 0)`. Both are outside this numeric/staging scope.
12. `295A24..295A3C`: restore registers and return 1. Query return values are not
    propagated; the original return value is not evidence that a collision hit.

The pair-first-endpoint loop is gated by both query presence and enable. With
query disabled, the newly computed second endpoints and AABB may exist while
the first endpoints retain their prior bytes. An adapter must not eagerly fill
the first endpoints on every numeric-preparation call.

## Point transformation, numerical order, and W lanes

Original helper `28BAB0` takes `a0=out`, `a1=scene`, `a2=localPoint`.

- `28BAD8..28BAE4`: prepared scene bit 0 jumps to `28BB60` with world matrix
  address `scene+100` in `v1`; this path does not read the parent pointer.
- Dirty/no-parent: `28BAE8..28BB58` copies exactly 64 raw bytes from `scene+C0`
  into `scene+100`, then preserves all other flag bits and sets bit 0. The
  caller-visible scene cache and flag change even if the eventual query is off.
- Dirty/parent-present: `28BAF4` calls `2897B0` to refresh the parent and `28BB08`
  calls `289940` to compose. This is an explicit unsupported scene-refresh
  boundary for the bounded 48O adapter/proof; do not silently substitute a cache.

Treat the matrix as four consecutive Vec4 columns `c0,c1,c2,t`. For each XYZ
lane, the actual instructions at `28BB7C..28BB88` perform:

1. VU ACC = `c0 * local.x`
2. VU ACC += `c1 * local.y`
3. output = VU ACC + `c2 * local.z`
4. output += `t`

The bounded host model rounds each multiply/add to binary32 separately. Do not
reassociate the expression, use host double intermediates across operations, or
fuse multiply-adds. Translation is added directly; local W does not multiply it.

`28BB78` is `4A20025B`, `vmulw.w vf9,vf0,vf0w`: output W is explicitly 1.
`28BB7C..28BB88` write XYZ only. Local-point W and all four matrix W lanes are
loaded but not used in the XYZ arithmetic; their raw bits cannot change the
transformed output W. The resulting full Vec4 is copied through the stack to
the output and then to the second pair endpoint.

At `29576C..295784`, pair i second endpoint is stored at `pairs + 32*i + 16`.
For each axis, the min/max instruction operands are `(point, priorBound)`:

- min X `295790`, min Y `2957B0`, min Z `2957D0`
- max X `2957F0`, max Y `295810`, max Z `295830`

These are scalar `MIN.S`/`MAX.S` instructions. The older `SpringOriginal`
interpreter formerly used host float comparisons and selected the prior bound on equality.
That is an interpreter behavior, **not instruction proof**, and gives the wrong
ordering for opposite signed zero. It must not be carried into the 48O detector
oracle or native comparison as an original-instruction guarantee.

The existing `tools/gameplay/contact_oracle48g.h` instead uses signed-word
ordering. PCSX2's primary implementation confirms this: `fp_min`/`fp_max` compare
signed raw words, reversing the selection when both operands are negative;
`MIN_S`/`MAX_S` call those helpers. Consequently, for mixed zeros, min selects
raw `80000000` (-0) and max selects raw `00000000` (+0), independent of operand
order. See [PCSX2 FPU.cpp](https://raw.githubusercontent.com/PCSX2/pcsx2/master/pcsx2/FPU.cpp),
lines 83–90 and 265–276, verified 2026-10-06. This establishes the cited emulator
model's signed-zero behavior, not a new physical-hardware measurement.

The48O detector oracle initially overrode only these scalar operations and
added mixed-zero probes. The subsequent audit corrected the active shared
interpreter too, while preserving its historical source and all prior fixture bytes. Ordinary add/multiply/subtract operations retain their separately
rounded host-binary32 scope. NaN behavior, EE denormal handling, physical
overflow, and other physical EE/VU exceptional modes remain outside this map's
proof claim.

At `29588C..2958D8`, center XYZ is individually rounded
`float32(0.5 * float32(max + min))`, **not** `min + half` and not
`0.5*max + 0.5*min`. At `2958F4..295940`, half extents XYZ is
`float32(0.5 * float32(max - min))`. Each helper `106790` constructs XYZ with
W=1; the detector then explicitly writes integer `3F800000` to the W of both
center and half before full 16-byte stores. Half-extents W is therefore 1 too.

There is no no-point guard or fallback. For count <= 0 and nonnull pairs, bounds
remain `min=+FLT_MAX`, `max=-FLT_MAX`. Under individually rounded host IEEE
arithmetic, center XYZ becomes +0 and half XYZ becomes -infinity; both W remain
1. The subtraction overflows *before* multiplication by 0.5. This edge can be
reported as a modeled raw result but is outside the finite-valid AABB contract
and is not a physical-EE overflow claim. If pairs is null, center/half are left
untouched, including their previous raw W words.

## Optional provider stop

`29570C..295724` tests detector `+34 != 0` and spring `+48 != 0` for each record.
If both pass, it copies the world point to record `+30`, then calls
`2E83D0(provider, &record.surfacePoint, &record.normal, 0, &temporaryByte)`.

The frozen `2E83D0..2E8448` is an assertion stub: its immutable ELF strings are
`TerrainStub.cpp` and `stub function - not implemented`, with assertion line 56
and a possible `break` instruction at `2E8438`. It does not supply a reliable
normal/material result. If control returns, detector reads an otherwise
uninitialized stack byte, masks it by 7, and stores record `+4C` and spring
`+D4`. This branch must be rejected or stopped before the provider call; it is
not a terrain-sampling implementation. For all positive-count fixture paths,
set provider pointer null or spring provider-enabled field zero.

## Reset and pair staging

Reset helper `296490..2964B4` has only these spring writes:

- `+84=0`, `+38=1`, `+130=0`, `+3C=0`, `+74=0`, `+78=0`, `+40=0`, `+44=0`

It neither changes persistent count `+7C` nor persistent records `+80`. It runs
after numeric preparation and before the query gate, even with query disabled.

`2959A4..2959D0` writes center Vec4 verbatim into `pairs + 32*i`; stop at
`2959D4` to retain full pair preparation without entering query staging.

`11CA80` is a leaf staging helper, without allocation or registration:

- Always sets `query+88=3`, including when query `+8C` is null.
- If shape `query+8C` is nonnull, loops over `2 * shape[+0]` endpoints.
- Copies each full Vec4 from the supplied pair array into `*(shape+8)`.
- Writes each corresponding word in `*(shape+4)` to zero because the detector
  supplies `a2=0`. With nonzero `a2`, it would copy those words instead.
- `shape+0` drives this copy; it is not re-read from spring `+7C`.

Therefore a bounded adapter that enables pair staging must validate source and
destination backing capacity for the independent query shape count. It need not
equal spring count. A larger query shape count reads beyond the newly prepared
endpoint range and copies pre-existing pair bytes; this is supportable only if
those extra source slots are initialized and within the supplied backing. A
smaller query shape count copies only the requested prefix. The original has no
count-agreement or capacity check. A null shape is an original no-copy branch, not a fully prepared query.
Tag 3 is the query's staged multi-pair shape tag; it must not be confused with
the static candidate tag 2 used by the callback.

For pair mode, a safe final stop is `2959F8`, before the candidate-array branch
dispatches either list or world execution. The `+74` pointer has already been
loaded by then. To inspect resolved limited-list call arguments without running
the callee, `295A08` is the call-instruction boundary: `a1` is the dereferenced
count and `a2` the array, and the original delay-slot load supplies `a0=query`.
For full-world mode the corresponding call boundary is `295A1C`, with `a1=0`
set by its delay slot. Do not treat the pre-delay register values as full call
arguments.

For no-pair mode, `11FF60` calls `11FE50` for scene preparation then calls
`120230` registration at `11FF74`. Stop at `11FF74`, before executing this call.
The detailed shape/query matrix preparation contract is separate from this
detector map. Numeric preparation may already have been skipped or reset may
already have run when this registration boundary is reached.

## Scene-matrix accessor used by the no-pair query path

The frozen helper `28B550..28B690` takes `a0=sourceScene`, `a1=relativeScene`,
`a2=outMatrix`. The bounded normal query path supplies `a1=0` and a nonnull source.

- If source flag bit 0 is set, `28B5A4` branches to `28B624`; its delay slot sets
  the copy source to `sourceScene+100`. Eight iterations at `28B62C..28B648` copy
  the full 64 bytes to the output, preserving every raw matrix word.
- If source is dirty with no parent, it first copies `+C0` to `+100` and sets
  bit 0, just as the point-transform helper does.
- With relativeScene zero, `28B64C` skips the relative-parent machinery and
  returns. A nonzero relativeScene can refresh its parent and call `289D50`;
  this is a separate unsupported relative-scene boundary.
- Source equal to relativeScene takes an identity path using `306478` memset,
  then explicitly sets four diagonal words to `3F800000`. This is not needed
  for the ordinary nonnull-source/zero-relative query seam.

## Bounded native API requirements

A value-based native API should expose the point count, per-point local Vec4
and scene matrix/cache state, existing center/half and pair bytes, reset state,
and query/enable/provider gates. It should report which phase ran and the next
unsupported boundary, rather than implying a world query completed.

For a finite numeric result, require a positive representable signed count,
valid source records/scenes and destination capacity, and ordinary binary32
inputs whose actually used arithmetic intermediates remain finite. Count zero
or negative can be retained only as an explicitly labeled original/model edge.
Reject dirty-parent scenes and active provider work unless their original
dependencies are separately implemented and proven.

The object-view contract must either forbid overlapping detector fields,
persistent records, endpoint output, scene caches, and query staging arrays, or
explicitly preserve original read/write ordering for each permitted overlap.
An all-inputs-first snapshot silently changes original alias behavior. Repeated
scene pointers across records are valid: a dirty/no-parent scene becomes cached
after the first transform and subsequent records use that cache. Pair staging
must have valid backing for its independent query shape count; do not overwrite
stale source slots merely to force count agreement. If aliases cannot be
validated safely by the API, use value-owned inputs and separate outputs.

Raw W is part of the output contract: transformed points, freshly computed
center/half, and filled first endpoints have W=1; disabled/skipped writes retain
their old bytes. Avoid normalizing untouched fields or fabricating extra writes.

The active shared spring instruction executor was subsequently corrected during
48O's prior-checkpoint audit. Its former source is preserved separately; this
map's description of float-comparison tie behavior refers only to that historical
version. See [the signed-zero audit](minmax-audit48o.md).
