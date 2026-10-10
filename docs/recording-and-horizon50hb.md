# 50HB: authored horizon clear and bounded capture output

The live client retains the requested 5,400-unit world far plane and all 50HA
effects. The sky fixture now authenticates the original TA scene and exposes
its Fog.ColorRGB as normalized clear RGBA. Original setter13E830 and viewport
clear2B4878 share the renderer-global color field at offset28. Clearing to that
authored color replaces the old diagnostic dark background; it does not add
missing sky tiles, stretch imagery, or substitute terrain.

Capture now packs each RGB row into caller-owned cold storage and writes one
row at a time instead of one ostream call per pixel. Orientation and RGB bytes
are unchanged. The file is closed successfully before the capture acknowledgment.
A separate readback diagnostic preserves the preceding draw statistics.

The expensive whole-track geometry inspection associated with each capture is
now opt-in with RR_CAPTURE_VISIBILITY=1. Ordinary recordings still read back
the actual committed framebuffer, preserve wall-clock frame timestamps, and
retain native physics and rendering. Only the additional diagnostic ray scan
and JSON report are disabled by default. Both capture buffers allocate cold;
no new allocation is introduced by row conversion.

The prior 50HA recording measured substantial capture overhead. No particular
speedup is claimed from these edits alone; the integrated recording must be
measured separately from no-recording gameplay.
