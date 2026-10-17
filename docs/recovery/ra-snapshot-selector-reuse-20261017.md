# Retain private RA snapshot fallback selectors

UNTESTED. No build, smoke, syntax check or FPS measurement.
Base publication d6651457efa7b8814151b0ca5fc936f9879d99e8.

In snapshotRaWorld's framebuffer-blit fallback, omit only the scene readBuffer
attachment0 and snapshot drawBuffer attachment0 reassertions. Keep both FBO
bindings, the exact bounded blit and common error check. CopyImage is unchanged.

createCoverageExtent initializes and checks scene READ0 and snapshot DRAW0
before publishing the live extent. These are per-object selectors, retained
across bindings and live/spare swaps. Full writer inspection finds no alternate
assignment: frame begin, RA final cleanup and resolve repeat scene READ0; the
snapshot has no different DRAW assignment. Private names do not alias UI or
single-sample presentation targets. Failed extent creation/restoration never
admits the new objects for drawing.

A failed snapshot still stops the primitive loop and reaches the existing
unconditional final scene selector restoration/error gate. No fallback retry or
error check is removed. Previous CopyImage work kept this fallback verbatim to
isolate that change, not because selector reassertion carried a separate rule.

Static saving: two selector calls per RA blended snapshot using the blit path.
Modern contexts taking CopyImage receive no change from this optimization.
Both candidate source reviews accepted the narrow omission. A successor may
validate the fallback, extent recreation and rejection cleanup independently.
