# Reuse the successful coverage tail's texture selector

UNTESTED source change. No build, syntax check, smoke, benchmark or gameplay run.
Base publication: 9aa6a5b6929e4435aacffce2c324be08a0d6d1ee.

In platform/linux/native/native_coverage_storage.inl, drawCoveragePairTriangle
now selects texture unit2 at cleanup only when accepted is false. The subsequent
snapshot texture unbind remains unconditional.

An accepted bridge necessarily reached the composition setup's unit2 selection,
snapshot binding, setup error gate, draw, and draw error gate. None of the
intervening operations changes the active texture selector. The owning check()
in vehicle_window_gl50ds.cpp reads glGetError and records diagnostics; it does
not invoke a callback or modify the selector. Thus the successful unbind still
targets unit2. Rejected payload, copy, setup, or draw paths retain the original
explicit selector retry. Empty regions return before setup as before.

The subsequent unit1 and unit0 selections, texture unbind/rebind, framebuffer,
program, VAO, depth/sample/scissor restoration and every error gate remain.
No state is cached across calls or contexts. RA composition is unchanged.

This removes one glActiveTexture call per successful racer coverage bridge.
It does not remove composition, change ordering, alter pixel ownership or prove
an FPS gain. Two independent source reviews accepted the guarded replacement;
the successor may validate rendering, rejection cleanup and performance.
