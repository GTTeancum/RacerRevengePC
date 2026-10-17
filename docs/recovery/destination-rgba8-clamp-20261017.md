# Destination RGBA8 clamp removal

UNTESTED. Independently source-reviewed with no identified blocker. Source inspection only; no build, shader compilation, syntax check,
gameplay, benchmark or new instrumentation. Base794f104 has the same tree as
remote b90a8f9: d28d1afe877a2786333480d07b795fdfd22a4000.

Exactly two embedded fragment-shader expressions lose their destination-input
clamp: native_coverage_storage.inl coverage composition, and
ra_static_world_gl_storage.inl RA per-sample blending. The texelFetch, multiply
by255.0, add0.5, and floor remain in their original order. RGBA16F source payload,
negative-alpha sentinel/discard, depth, source arithmetic and final result clamps
are unchanged.

## Ownership, initialization and bounds

coverageTextureStorage allocates private storage and queries its width, height,
format, four samples and fixed-sample locations before attachment. Scene colors
and snapshotColor use RGBA8 at identical extents. The payload alone uses RGBA16F.
prepareCoverageExtent publishes a complete spare set only after allocation,
completeness and restoration succeed. ready() requires the current frame extent.

The only three destination binding sites are racer coverage, RA AA0 composition
and RA AA1 blending. They all bind coverageLive.snapshotColor on unit2, selected
by their private sampler uniform. There is no material-provided destination.

begin() sets the full viewport, disables scissor, enables all color channels and
clears the complete scene from finite [0,1] clear values before world/racer draws.
Its error gate must pass. Each checked four-sample snapshot blit copies the same
source/destination rectangle from this initialized scene. The consumer scissor
matches the copy; the non-RA racer region is the full target. Region bounds are
clamped to the frame extent. Contributing gl_FragCoord pixels therefore address
the copied rectangle and gl_SampleID is0..3. Snapshot and scene are distinct
storage, and cleanup unbinds the snapshot before later writes.

Discarded/helper invocations cannot publish a framebuffer result; neither changed
expression feeds derivatives or another invocation. No undefined destination read
is relied on for a contributing result.

## OpenGL basis

[OpenGL4.0 core](https://registry.khronos.org/OpenGL/specs/gl/glspec40.core.pdf)
section2.1.5 equation2.1 defines unsigned normalized conversion c/(2^b-1), within
[0,1]. Section3.8.3/table3.12 identifies RGBA8 as eight-bit unsigned normalized
components. Section2.11.8 covers texel/multisample fetches; sections4.2.3 and4.3.2
cover clears and framebuffer copies.

[GLSL4.00](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.00.pdf)
section7.1 defines window-relative gl_FragCoord and gl_SampleID in the range
0..gl_NumSamples-1; section8.3 defines clamp/floor.

Valid RGBA8 destination components are finite [0,1], so multiplication by255.0
already lies in [0,255]. The removed clamp is an identity, and existing rounding
and byte composition remain unchanged. This proof does not apply to floating
payloads or arbitrary external textures.

## Limits

Two redundant vector clamp expressions are removed. A driver may already remove
them; no GPU instruction reduction, pixel test or FPS improvement is claimed.
All GL calls, resource validation, draw order, error gates and restoration remain.
