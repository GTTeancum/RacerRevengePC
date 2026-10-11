# Game Options focused Linux build, 2026-10-11 03:17 UTC

The centralized hourly check started at 03:17:17.443011 UTC against local commit cd9a1d99a67336c4ce220a8b09374278fba3957b. Compilation stopped with exit 1 in original_ui_light_manager_lifetime50kg.cpp: four-lane ambient/directional colors were assigned directly to three-lane RegisteredLight RGB fields.

No executable was linked, no native UI process launched, and no new video, audio or gameplay evidence was produced. Incremental compilation stopped at its first failure; no complete-build object count or prior build receipt is reported as a new result. Existing Options edits, Cancel, Accept and audio remain runtime-unverified.

The source correction explicitly copies RGB lanes 0..2 to diffuse/specular fields. It preserves all four selected-zone ambient lanes and the existing three-lane direction. This correction received source review only and was not rebuilt. The next centralized check is no earlier than 04:18:17.443011 UTC.

Rider composer/session work staged separately was not included in this check. Native host execution only; no PS2/VU emulation or guest execution. Historical Windows results are unrelated to this Linux failure.
