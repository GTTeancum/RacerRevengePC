# Shared-edge map-2 batching remains held

Source/specification review only. No source implementation, build, test, benchmark
or gameplay execution was performed for this investigation.

## Required ownership

The current map-2 payload uses AA0 single-sample center rasterization, then its
pixel payload is composed across the destination's samples. A proof that triangles
have disjoint multisample sample coverage is not sufficient: the payload must
have disjoint AA0 pixel ownership, or retain each ordered payload separately.

OpenGL polygon rules provide exclusive shared-edge center ownership when the
actual rasterized endpoints are identical and triangle interiors lie on opposite
sides. That is a conditional theorem, not a sufficient host admission predicate.

## Missing portable proof

The current host operand helper estimates clip uncertainty with
32 * float_epsilon * magnitude + 0.0001, propagates quotient extrema, and pads
integer rectangles by two pixels. It does not establish a complete specification-
derived enclosure of every permitted shader transform, perspective divide,
viewport calculation and raster quantization result.

SUBPIXEL_BITS specifies precision, not a particular nearest-grid rounding rule
or an assumed half-ULP snapping bound. A robust determinant test would need to
prove strict opposite signs for every admitted post-transform/quantization
realization, preserve identical shared endpoints, and exclude clipping and
near-degeneracy uncertainty. A larger run also needs exclusion against every
prior member; adjacency to one triangle is insufficient.

An incorrect acceptance could overwrite an earlier payload/depth or consume stale
destination alpha, changing original ordered byte composition. Do not introduce
an arbitrary epsilon, relax existing checks, or revert the corrected CPU matrix
for apparent FPS gains. Existing conservative rectangle batching remains intact.

## Primary references and next steps

- [OpenGL 4.0 core, section 3.6.1](https://registry.khronos.org/OpenGL/specs/gl/glspec40.core.pdf): polygon rasterization and shared-edge ownership.
- [OpenGL 4.6 core, sections 2.3.4.1 and 13.8, table 23.53](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf): floating-point scope, coordinate transformation and subpixel precision.

A successor may research a complete portable enclosure or an explicitly admitted
backend-specific guarantee, then independently review and validate any candidate.
Until that exists, there is no approved shared-edge shortcut. No speedup is claimed.
