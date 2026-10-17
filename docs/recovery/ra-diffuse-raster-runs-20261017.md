# Adjacent diffuse raster validation runs

Status: independently source-reviewed; UNTESTED.
Base f6ac868. No build, syntax check, test, gameplay, profiling
or benchmark. Runtime benefit is unmeasured.

The private synchronous visitDiffuseSurface validation loop now remembers only
the immediately preceding successfully validated raster index. An identical next
index skips the repeated diffuseRasterCurrent and fill-bit lookup. The first row
and every changed index execute the original checks in original order. The local
remembered index becomes valid only after BOTH checks succeed. No persistent
cache, array allocation, name substitution or assumption of a fixed raster is
introduced. Public readDiffuseVertex is unchanged.

The loop has no callbacks or mutations: diffuseRasterCurrent reads current owner/
camera validity, raster metadata, resolve298E60's actual row and pass4C's current
fields. resolve298E60 writes only its local output/diagnostic; pass4C returns a
read-only view. Therefore repeated adjacent indices cannot change result within
this synchronous scan. Unrelated distinct material invalidation is still checked
before the visitor. Existing current checks before/after visitation remain.

Current source admission makes this useful without an asset decoder run:
ra_static_world50la.h retains psms_ in a normalized-name keyed map;
ra_static_world_images50la.inl appends one image family per retained PSM;
ra_static_world_draw_plan50la.inl appends one raster per family and accepts
DiffuseCompressed only for the authenticated textures/black.psm family. All
currently admitted diffuse surface rows consequently refer to that raster.
The optimization still handles future multiple raster indices by checking each
new run, including an index that appears again after a different one.

This reduces repeated deep validity/material reads within each selected-surface
scan, while retaining the scan's row ordering and the current source publication.
It does not change generated colors, geometry, fog, draw order or GPU state.
