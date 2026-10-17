# Reuse adjacent identical RA scissor calculations

UNTESTED. No builds, tests, syntax checks, benchmarks or gameplay executed.

The source native draw producer computes one original scissor per zone, then
emits every selected model from that zone with the same viewport/scissor tuple.
Frame preparation converts each tuple to a native framebuffer rectangle. A local
one-entry cache now reuses that result for exactly equal adjacent four-lane
viewport and four-lane source-scissor arrays. Frame dimensions are fixed during
the call; no world/LOD/material selection is cached.

All original placement, authentic LOD, range, positive viewport and ordered
scissor validation runs before lookup. On a miss, the exact original double
expressions, floor/ceil, axis-to-extent mapping and clamps execute unchanged.
The zero-area check, record order, duplicate records and per-surface inventory
validation remain in their original positions.

A frame-draw push may allocate and invoke a new-handler. Before each push whose
size equals capacity, invalidate the cache; never restore it after that push.
Thus reuse cannot cross a potential allocator callback or its FP-mode effects.
The next record recomputes even for an equal tuple. The valid flag starts false
on every call; no result survives a frame boundary or failure. No allocation or
persistent cache is added.

Two independent source reviews accepted this bounded design. Source-level work
saved on a cache hit is the four bound transformations and four floor/ceil calls,
at the cost of integer tuple comparison and a growth check per appended surface.
No measured frequency, speedup or FPS gain is claimed. Successor validation should
compare exact selected rectangles and ordered surface records, including empty
rectangles, tuple changes, viewport changes and vector growth boundaries.
