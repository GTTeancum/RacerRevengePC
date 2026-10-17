# Reuse RA's private coverage framebuffer selectors

UNTESTED. Source review only; no builds, syntax checks, smokes or FPS measurement.
Base RA storage blob: c9769d3408fbc7119c348ebad7e62c17b5b8753d.

## Narrow change

Within drawRaWorld, remove the per-primitive drawBuffers(2,...) after binding
coverageLive.raster and drawBuffer(attachment0) after binding coverageLive.scene
in each AA0 composition and direct-draw branch. Keep framebuffer binds and the
final unconditional scene draw/read selector restoration. The snapshot fallback
also keeps its original selectors.

createCoverageExtent initializes and checks raster draw attachments0/1 and
scene draw attachment0 before an extent becomes live. Framebuffer binding does
not reset per-object draw selectors. The full live/spare exchange preserves
these objects; failed creation is not admitted for rendering. Single-sample
presentation targets and UI framebuffers have independent names/state.

Other owners either retain those assignments or reassert the same values:
frame begin and RA cleanup select scene attachment0; snapshot and resolve read
scene attachment0; racer composition relies on the same raster/scene state.
The earlier native-coverage-fbo-selectors-20261016.txt review already identified
RA's reassertions as identical, while intentionally leaving them in place.

Both blended AA0 and direct AA1/opaque branches retain their original shaders,
texture bindings, sample/depth policy, draws and ordering. Failed snapshots
still stop before drawing. The existing primitive error gate and final cleanup
remain; this is not a global state cache or an error-recovery redesign.

Two selector API calls are removed per blended AA0 primitive, and one per direct
primitive iteration. No measured FPS improvement is established. A successor
may validate both coverage modes, extent recreation and failure cleanup.
