# Original Pause over a completed frozen race image

`VehicleWindowGl50DS::submitRetainedOriginalPause` is an inactive native render
entry point. It consumes the already bound authentic Pause draw owner and its
prepared CPU command frame. It does not prepare a camera, advance the world,
update materials/flames, run HUD preparation, or reapply gameplay blur.

The renderer records the source camera publication only when the corresponding
whole gameplay transaction commits. The retained race becomes eligible only
after the actual `completeGameplayDisplay` finish/swap (or headless equivalent)
succeeds. A new race commit invalidates that completion. A race containing Pause
geometry is rejected as an underlay, preventing cumulative overlays. The Pause
frame must carry the same nonzero camera publication. No new camera publication
is minted by this path; the caller remains responsible for genuine source
scheduler/camera authority before preparing that frame.

The method rejects resize: requested dimensions must equal the retained race
extent. It requires the current creating-thread context, idle renderer, no fade
or UI-byte-blend owner, completed optional Pause binding, two existing private
menu targets and the gameplay display-completion capability. No host glyphs or
host-menu row state are used.

The existing ordered Pause raster pass is factored to accept source/destination
FBO names. Full gameplay still uses its pending target for both. Frozen Pause
always reads the unchanged `targets[published]` race color and writes only the
inactive menu target through the existing separate Pause color/depth scratch.
Authentic material/depth/viewport/scissor and raw source-alpha rules are reused.
The retained race's GPU vertex bank and image never change. Unsupported original
destination-alpha/context raster modes remain rejected by existing binding.

A successful private publication follows existing menu presentation semantics:
window mode copies to the back buffer, headless mode marks pending display, and
the caller must use actual `completeGameplayDisplay`. That operation may update
real displayed-history blur storage, but never the retained original race source.
Failure does not certify a display. Existing renderer cleanup owns all resource
names and the strong Pause draw owner; no new allocation or lifetime lease is
introduced. Cleanup clears the additional retained-source metadata.

Parity scope: authentic prepared Pause geometry over the last completed native
race image, at unchanged drawable size. This is not full original Pause callback,
Application scheduling, animation/timer advancement, or split-screen parity.
No driver call is added. Source-only review; no builds/tests were executed.
