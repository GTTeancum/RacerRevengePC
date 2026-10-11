# Linux Main / Options focused observation, 2026-10-11

Centralized slot began02:16:10.251731UTC at ddd4a3e73571feca60aa453bc43bfdcfd6795aec. Next ordinary slot03:17:10.251731UTC. No helper executed a check.

Native build passed:64 translation units compiled,196 hash/dependency-verified cached units reused, link exit0. Executable SHA256df2d497068c4de11288d3990d0e174e17ba3d585bb9f0f2b4137eac7dc1b4775.

The single bounded UI observation FAILED its requested Main→Options→Main round trip. Main opened and three separately acknowledged Down presses selected actual slot4. Enter could not prepare Options: outer options resource error5 identifies widget-bank binding. No GPU or gameplay transition occurred. Main retained saved slot1; no destination/profile success is claimed.

18 acknowledged640x448 frames were preserved over0.62999746seconds, about26.984 capture acknowledgments per second (maximum gap0.068473319seconds). This is a short partial sample, not a sustained30fps result. All world ticks, driving input and audio output remained zero. The recorder exited2 after the resource rejection; the native process accepted quit and exited0 without forced termination. Its raw final receipt says cleanup_complete=true,19 captures and565 frames. The recorder's cleanup check failed because it preserved18 frames and rejected the unavailable-destination state paired with capture19. Do not conflate that evidence-count mismatch with a native cleanup failure.

Partial delivered video: libfile_0e814d82a2b081919520575ed83e6819,18 actual source frames, no interpolation. It covers Main only and lasts under one second. Options opening, page4 controls, audio, Rider selection and gameplay remain unvalidated by this observation.

Next work: diagnose real widget-bank bind contract statically, retain detailed nested diagnostics, fix without bypassing source validation, then use the next eligible centralized slot for the same round trip.
