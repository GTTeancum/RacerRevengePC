# Original Main one-off observation, 2026-10-10 17:55 UTC

The user explicitly authorized a one-off test to see menus/UI at17:55:21UTC.
This was the only additional centralized build/run. No helper executed checks.

Source commit: f89a3c53641cc45a236a3c88476f24d632e5f18d.
Fresh Linux incremental build passed:58 compiled,188 dependency/hash-validated
reused objects, no supplied prebuilt objects. Executable SHA256:
c08b6dc8074ace12a74824a7ba2780037fd1718fd1b24d327c5fdde5f612dd93.

One original Main process completed its20-second opening/idle observation:
-94 requested/acknowledged/preserved640x448 frames
-1636 actual native submissions through quit
-Observed actual aperture opening, then input enabled and lifecycle idle
-Zero race physics ticks, driving input, UI navigation input or mixed audio
-Muted explicitly; no audio validation
-Native and recorder exit0, cleanup complete, no forced termination

The earlier unexpected race startup tick was absent. The last captured image
was visually inspected: original title art, Main labels, original fonts/icons,
selection arrows and Continue prompt were present. This is actual native UI
capture, not gameplay, a mockup or navigation/submenu acceptance. Options and
Rider complete-screen transitions were not exercised.

Evidence directory: integrated-native-validation-04; capture directory:
ui-main-recording-reconstruction/main-ui-validation-02 beneath recovery staging.
Video retains94 frames with actual acknowledgment-interval VFR timing, H264,
640x448,19.88seconds, no interpolated frames or audio. Video and screenshot were
saved and delivered to the user at17:59UTC.

Next normal centralized slot:18:56:57.403928UTC, based on the one-off start at
17:55:57.403928UTC. Another reporting checkpoint does not authorize a test.
Historical Windows results remain separate.
