# Native HUD ScreenTexture strip diagonal

Original `2DF0B0` constructs four ScreenTexture vertices in TL, TR, BL, BR
order. `2DF258` loads `0x1C`, `2DF25C` shifts it into the GIF PRIM field,
and `2DF2F4..300` installs it: textured, Gouraud triangle strip. Its two
triangles share the TR-BL edge.

The actual shared native UI index buffer in `vehicle_window_gl50ds.cpp`
uses `0,1,2,0,2,3`. TimeStats, PositionStats, Damage and SpeedBoost all bind
that same buffer. Their former texture perimeter TL,TR,BR,BL selected the
opposite TL-BR diagonal. The texture-only mapping is now TR,TL,BL,BR,
equivalent to original strip indices `1,0,2,3`, matching the existing Pause
texture adapter. The first native triangle covers original strip triangle0
and the second covers strip triangle1, preserving submission order.

PositionStats retains raw packet vertices and reorders every vertex's XY,
UV and RGBA together at submission. The other three owners retain already
expanded native texture storage; both their UV initialization/update and XY
regeneration now use the corrected perimeter. Their source texture colors
are uniform across all four corners (Damage rewrites every corner together),
and depth is retained per quad, so those values are unchanged. Glyph and
SpeedBoost three-vertex primitive paths are unchanged, as are every original
material, pass, viewport, draw order and lifetime.

The current affine, uniform-color rectangles are expected to retain their
visible appearance. This corrects source triangle ownership rather than adding
a new HUD element. Static source review only; no build or execution performed.
