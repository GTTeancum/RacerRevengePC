# Ordered RA diffuse row lookup

Status: independently source-reviewed; UNTESTED.
Base375083b. No build, test, syntax check, gameplay, profiling
or benchmark. No measured benefit is claimed.

visitDiffuseSurface keeps its full whole-publication raster validation scan
unchanged, then uses lower_bound instead of a second linear scan to locate the
requested surface. Exact identity equality is still required before invoking
the callback; missing rows reject. Successful callbacks still require current()
afterward. The public per-vertex lookup is unchanged. The header directly
includes algorithm rather than depending on a transitive include.

The actual producer establishes strict ordering:

- ra_static_world_draw_plan50la.inl iterates si from zero through surfaces_,
  assigns record.surface=si, and appends each draw-plan record in that order.
- Binding::prepareDiffuseStream iterates that plan and appends only its lit
  subset, assigning output.identity.surface=plan.surface. Skipping prelit rows
  introduces gaps without changing order or uniqueness.
- StaticDiffuseStream::surfaces_ is private to Binding; this preparation is its
  sole append producer. A publication replaces the whole stream and no operation
  reorders or mutates its row identities after publication.
- The existing synchronous visitor holds the same live Binding/world/camera and
  callback lifetime. Nothing is retained across frames or added to owner state.

This reduces only the lookup portion following validation from linear to binary
search. The required whole-publication invalidation pass remains linear and
unchanged; actual performance impact is unknown.
