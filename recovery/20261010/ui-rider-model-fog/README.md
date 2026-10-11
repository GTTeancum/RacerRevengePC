# Concrete Rider model fog preparation

Source-only new pair; no build, syntax check, test, project parser or guest execution.
No existing-file replacements. Uses integrated model_bounds49o and the actual
Rider scene layout; output is a render preparation snapshot, not model admission.

28CAC0 reads the PSG node count into source1A0; source26ED38 propagates that count
for the additional model root. Count1 selects stored sphere194; multi-node selects
computed aggregate center160/radius198. Actual 19 character32 and pod3 take the
latter. Single-node route remains explicitly unsupported by this narrow adapter.
26F070 distance is length((center160-camera180)+modelWorldTranslationF0), not
camera-space Z. 241320 sets model1B2 bit1 unless distance+radius is below actual
render-context fogStartB0. Equality enables radial fog. Materials retain their
independent inherited FGE; the no-radial-fog packet still carries source F255.

Current post-motion PSG locals, original authored bounds and source flags feed
existing shared26F4B0/28DAF0 aggregate owner. Actual root world is refreshed on a
snapshot. Never replace original PSG bounds with neutral boxes, model vertex
extents, cache-root radius or a constant 'Rider is close' assumption.

Join: actual cold hierarchy, scene.graph(), renderContext.cameraAfterFog.position,
renderContext.fogStart. Consume computeRadialFog1B2 separately from material FGE.
This does not replace source visibility/culling/LOD selection or authorize drawing
an unselected model. Source meshes, palettes, lights, clipping and materials remain
required. Root alone integrates and validates.
