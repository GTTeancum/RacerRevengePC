# Existing-capture video export timing

The encoder correction was exercised only as artifact preparation. It did not launch a game, rebuild source, run a test or create replacement capture frames. The failed export, original images, capture receipt and PCM remain preserved outside Git.

The original GC export retained all 298 presentation timestamps, but B-frame reordering made its reported duration end before its final image presentation. The correction disables B frames, keeps variable frame timing and the 1 ms time base, and verifies stream/container endpoints. A required `--output` selects a fresh directory; existing output is rejected. Absolute source-image references allow a separate destination without copying or modifying captures.

The corrected MP4 retained each of the 298 original frames exactly once, in order. Its verified duration was 20.059 seconds against the measured 20.057674-second capture interval; maximum frame-time error was 0.632 ms. The final real image is displayed for the explicitly recorded 1 ms endpoint. No speed adjustment, interpolation or repeated source frame was introduced.

The artifact is silent. Actual simulation-timed PCM drifted from wall-video anchors by up to 141.639 ms and ended 121.674 ms short. It remains separate; the encoder neither stretches/resamples it nor claims synchronized playback. `--require-audio` can make this absence block delivery.

This demonstrates artifact timing, not complete GC driving. The HOST course drivers stopped at node 7 and no GC lap completed. Game behavior, manual usability and listening quality are outside this encoder correction. Existing runtime and source-control histories are unchanged by artifact preparation.
