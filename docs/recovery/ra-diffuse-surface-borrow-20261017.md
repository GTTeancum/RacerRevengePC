# Synchronous RA diffuse surface preparation

Status: independently source-reviewed; UNTESTED. No builds, tests, syntax checks, gameplay,
profiling or benchmarks. Base86e4ec0. Performance benefit remains unmeasured.

The existing Binding::readDiffuseVertex validates every generated diffuse
surface's live material, then linearly finds the requested surface, for every
vertex. This preserves whole-publication invalidation but repeats the same
read-only work during the synchronous preparation of one selected surface.

The new private visitDiffuseSurface performs the identical current() check and
complete diffuseRasterCurrent/fill-bit scan before any callback or byte read.
It locates the actual generated row, lends its identity and const vertex vector
to one synchronous renderer callback, then checks current() again after success.
The aggregate forwarding method is private under its existing renderer friend.
The public per-vertex API and its validation semantics remain unchanged.

The renderer compares the exact generation/publication/frame to the current
surface fog observation, checks the complete required vertex extent, and copies
the same RGBA bytes in the same vertex order. Prelit color handling is unchanged.
Each original raw-position/fog observation and numeric operation remains in the
vertex loop. There is no dynamic value cache across surfaces or frames.

## Borrow lifetime and non-reentrancy

The Binding privately owns the generated stream and strongly retains its world,
lighting and camera owners. The actual renderer strongly retains the aggregate.
The callback contains only frame-vector writes, byte copies, existing numeric
fog leaves, and readNativeStaticRawPosition. That observation follows the retained
fog Binding and readStaticFogPublication/readStaticLightPublication checks; it
reads the live idle Bank and exact source camera identity without callbacks,
material service, stream rebuild, owner reset or allocation. The failure helper
sets the diagnostic and returns; it does not dispatch application code.

Crowd material service remains before the visit, as previously before the vertex
loop. No service or GL call occurs while the borrowed vector is in use. Therefore
any other material change preceding this selected surface still invalidates the
whole publication via the complete original scan. No concurrency or lock policy
is invented: the existing synchronous owner-thread renderer scope is required.
References are never stored outside the callback.

## Failure behavior

Missing/stale publication, changed material or absent surface rejects before
callback execution with the original diffuse diagnostic. A short vertex vector
now rejects before writing that surface, rather than at its first missing vertex;
both routes reject the whole frame and leave framePrepared false. Callback fog or
source failures preserve their original diagnostic. A failed post-visit current
check produces the diffuse diagnostic. Final frame/publication and later draw-time
checks remain unchanged. No successful partial preparation is published.
