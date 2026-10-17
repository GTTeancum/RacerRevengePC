# Coverage group invariant state (source-only, untested)

Base: `75fbe2e`. This change only reduces redundant GL state submissions in
`native_coverage_storage.inl`; it does not change triangles, regions, grouping,
shader math, destination snapshots, depth tests, alpha operations, or publication.

`drawCoveragePairTriangle` has one caller, `drawCoveragePairGroup`. A group is
synchronous and has no source/model callbacks between bridge invocations. Region
lookahead is CPU-only. Its new local `firstRun` starts true for each group and
becomes false only after a successful nonempty bridge. Empty groups make no new
GL calls, and rejected bridges return through the existing cleanup tail.

The first bridge establishes the existing five sample-policy disables through
`coverageAa0`, along with disabled framebuffer sRGB/blending, the full framebuffer
viewport, and the all-channel color mask at their existing setup positions.
No payload, copy, composition, or cleanup operation changes those nine values
within a group. Later bridges omit those redundant writes. The generic
`bindTrackCoverage` helper and its callback/span boundaries remain unchanged.

MULTISAMPLE is different: composition enables it, so each payload entry and each
successful cleanup tail still disable it. On any rejected bridge the tail calls
the full original `coverageAa0`, preserving all five additional cleanup attempts
even if first-run setup did not establish the intended state.

Every existing per-run GL error check and texture unbind remains in place. FBO,
program, pass selector, VAO, depth, and scissor restoration is still per bridge.
There is no cross-group cache or persistent state assumption. The first successful
bridge saves five redundant tail calls; each additional successful bridge saves
fourteen state calls. This is a source-level call-count reduction, not a measured
performance result.

No build, syntax check, test, gameplay run, or benchmark was performed.

## Combined composition invariants

The same firstRun scope also initializes composition mode zero and sampler-object
zero bindings on units 0, 1 and 2 only on the first bridge. Neither the bridge tail
nor CPU lookahead changes those values; the firstRun flag resets for every group.
All three active-texture selections and texture binds remain per bridge. This
saves four additional calls on subsequent bridges when sampler binding is loaded.
Combined successful-path reduction is five calls for a singleton group and
18 calls for each additional bridge, compared with the tested base. Error polls
and failure cleanup remain unchanged. This is not a measured speed result.
