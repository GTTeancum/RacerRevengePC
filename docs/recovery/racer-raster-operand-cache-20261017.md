# Group-local racer raster operand cache

UNTESTED. Base75fbe2e. Source-only inspection and independent review; no builds,
syntax checks, tests, benchmarks, gameplay or claimed performance result.

## Repeated work

Adjacent indexed strip triangles repeatedly calculate identical vertex clip
coordinates, dot-product magnitude/error bounds and six-plane masks in
racerTriangleRasterRegion. The actual original MSCNT group bounds these calls
within drawCoveragePairGroup. Its CPU lookahead and GL bridge calls contain no
callback that mutates the renderer matrix or the borrowed racer vertices.

The sole caller now brackets that synchronous group with begin/end scratch
scope. A fixed32-entry direct-mapped cache keys the actual vertex index, and
is additionally admitted only for the exact slot/component/index range.
Every group invalidates all entries; end disables admission after both success
and failure. No resource pointer, frame lease or publication is retained.
Static-world nativeTriangleRasterRegion calls never consult this cache.

## Unchanged arithmetic and rejection

nativeRasterVertex contains the previous column and term loops unchanged:
conversion to double, product, ordered clip/magnitude additions, error formula,
finite checks and plane-mask comparisons. nativeTriangleRasterOperands retains
triangle corner order, common-plane AND, low/high initialization, all eight
ratio evaluations per corner, finite checks, min/max order, floor/ceil and
pixel padding. There is no new bound or culling decision.

A cache miss computes exactly those operands from the actual source position.
A hit reuses only that vertex's immutable group operands. Invalid indices still
return full extent. A cached nonfinite result still returns full extent. NaN
and signed-zero source bytes are never normalized or used in approximate-key
comparison. Cache-slot collisions recompute; each triangle copies an operand
before a later corner can replace its slot. The group scope cannot survive an
ordinary false return because the caller always calls end after the noexcept
bridge function. No allocation or exception path is introduced.

## Cost and limits

For a strip of16 distinct vertices and14 triangles, the previous path computes
42 vertex operands; absent extra collision it now computes16. Triangle-level
ratio/min/max work is intentionally unchanged. Cache clears, integer lookups
and operand copies add work, so a real speedup is not established by source
inspection. This does not reduce GPU draws/blits, alter material order, remove
error checks, change original AI or resurrect shared-edge batching.
