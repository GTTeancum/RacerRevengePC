# Optional finer static-track culling 50GS

Status: source implementation only. This implementation worker did not build,
run tests, capture pixels, or measure performance. The coordinating task owns
those checks. The optimization remains explicitly opt-in; no performance or
render-equivalence result is claimed here.

## API and compatibility

50DS retains the existing [50GD conservative culling](static-culling50gd.md).
The optional `VehicleWindowTrack50DS::cullingChunkTriangles` ceiling adds finer
candidates for large required-texture source spans. Its default is zero, which
preserves the existing source-span draw boundaries, metadata allocation and
selection behavior. A caller can explicitly request, for example:

```cpp
request.track.enableFrustumCulling = true;
request.track.cullingChunkTriangles = 256;
```

Nonzero chunking requires an actual mesh, `callerMaterialTextures` track mode,
and enabled frustum culling. Other combinations reject as request errors rather
than silently ignoring the option. A nonzero value is a maximum triangle count
per required-texture chunk, not a spatial distance or a promise that its AABB
will be smaller. Even `UINT32_MAX` is safe: the remaining validated triangle
count is bounded before multiplication by three.

Neutral and source-vertex-color track modes still use their existing single
whole-mesh draw. Explicitly untextured spans within the textured mode are also
left whole. Vehicle, cable and HUD submission are unaffected; the current
wrong-way HUD and its 576-quad capacity remain unchanged.

## Original validation and separate cold chunks

The original source-span table is still validated in full. It must partition
the entire original vertex and index arrays in order; each span must be
nonempty, contain complete triangles, and reference only its declared source
vertex range. The original explicit material, texture-publication and alpha
policy checks remain. Chunking never weakens or replaces those requirements.

Only after those original checks, a separate count pass calculates the proposed
candidate total. A required-texture span contributes
`ceil((indexCount / 3) / cullingChunkTriangles)` candidates; an explicitly
untextured span contributes one. More than
`maxVehicleWindowTrackChunks50DS`, currently 65,536, rejects with a limits
diagnostic identifying the original source span. This happens before renderer
allocation or GL work. The original 65,536-source-span limit still applies.

Cold `create()` owns a separate array of internal records containing only:

- Original source span ID
- Contiguous unchanged EBO `indexStart`
- Nonempty `indexCount`, a multiple of three

The existing source material/span table is retained unchanged. Chunks are
emitted in source-span order and increasing index order within each span. Their
ranges partition that span exactly, including a smaller final chunk when
necessary. There is no vertex or index rewriting, rebasing, deduplication,
spatial sorting, alternate geometry, or GPU allocation for the chunks.

Each chunk's AABB is computed from precisely the actual indexed world-space
vertices in its range. Unreferenced vertices, display bounds and collision
proxies do not contribute. A complete retained-ID array and the bounds array
are allocated once at the candidate capacity. On the current binary32/uint32
layout, chunks, bounds and retained IDs use 12 + 28 + 4 = 44 bytes per candidate,
at most 2.75 MiB, plus allocation/owner overhead and the existing original
source-span/material table. Zero chunking allocates no chunk array; disabled
culling continues to allocate neither bounds nor retained IDs.

## Selection and draw order

Per-frame selection still calls the unchanged `collectVisibleStaticDraws50EI`
with the exact copied matrix bytes uploaded to GL, `negativeOneToOne` clip
depth and the `retain` uncertainty policy. It changes no floating-point mode.
The [50EI conservative proof and limitations](draw-visibility50ei.md) apply to
each candidate just as they did to each original source-span bound.

Selected IDs refer to the internal candidate ordinals. 50EI preserves their
input order. Each selected chunk resolves its material and alpha-discard policy
through its retained original source-span ID. Adjacent retained chunks from
the same source span reuse that span's already-bound material. They retain the
original triangle order, original texture parameters, opaque writable-depth
policy, alpha-zero discard behavior and base vertex zero. Chunking introduces
additional draw boundaries only; it never changes a primitive or fragment
policy, and never interleaves another material into a source span.

Every frame initially selects all candidates. A failed helper call retains the
complete candidate list, covering every original triangle in its original
order; partially written output or prior frame selections are never consumed.
This fallback uses chunk draw boundaries when chunking was requested. The
existing full original material-publication checks still include culled spans.
All-track-culled frames still clear and render the pod, cables and HUD normally.

The hot path adds no application allocation, geometry upload, readback or sort.
Cold preparation remains linear in source indices plus candidate count. Frame
selection and retained-index accounting remain linear in candidate count.
Smaller chunks can reduce submitted indices but increase CPU classification,
draw-command and GPU processing overhead. Original source order need not be
spatially local, so subdivision alone does not guarantee tighter useful bounds.

## Diagnostic interpretation

The existing `trackCandidateDraws`, `trackRetainedDraws`, `trackCulledDraws` and
`trackUncertainRetainedDraws` now refer to chunks when explicitly requested.
The existing `trackDraws`, `trackTexturedDraws`, `trackUntexturedDraws` and
`trackIndices` continue to count checked GL commands and their actual index
counts. `trackCulledIndices` is computed from the selected chunk ranges. It is
not a count of visible fragments or a GPU timing measurement.

`trackSpan` always identifies an original caller source span, including a
material/draw failure within one of its chunks. It never exposes a chunk ID.
The nested 50EI diagnostic's `drawOrdinal` continues to identify a candidate
ordinal, which is a chunk ordinal when enabled. Candidate = retained + culled;
fallback retains every candidate with zero culled/uncertain counts. As before,
require `batchAccepted` for a published private image.

## Verification required before enabling

Client option handling, build integration, actual-pixel checks and profiling
belong to the coordinating task. This renderer change does not enable chunking
for any existing caller. Keep zero as the default unless actual measurements
justify an explicit change.

Compare identical original assets, matrix bytes and frame sequences with
original source-span culling and with explicit chunk ceilings. Require exact
RGBA equality and unchanged pod/cable/HUD receipts. Also compare against full
unculled source rendering where practical. Include:

- Required-texture spans below, equal to and above the ceiling, including
  non-divisible tails and alpha-zero-discard textures
- Explicitly untextured spans interspersed with required-texture spans
- All retained, partially retained and all culled candidates, near-plane/eye
  crossings, boundary touching, and conservative uncertain retention
- Helper failure retaining the complete original triangle coverage
- Zero/default chunking, absent or nontextured tracks, and illegal nonzero
  chunk requests with culling disabled
- Malformed original partitions and escaping source-vertex indices, which
  must still fail before chunk preparation
- At-limit and over-limit candidate counts, large ceiling values, unchanged
  source buffers, complete source-order coverage and no hot allocation

Measure completed-frame timing and submitted index/draw counts under the same
renderer, workload and warmup. A smaller index receipt alone is insufficient
evidence of a speedup. Any decision to enable this optimization should retain
the exact pixel comparisons, configuration, timing method and observed tradeoff.
