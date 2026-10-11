# Rider entry observation — 2026-10-11 05:20 UTC

The root-owned centralized check began at 05:20:12.762509 UTC. Its next permitted check is no earlier than 06:21:12.762509 UTC.

## Completed preparation and build

The pinned original Rider19 character texture conversion passed for all five authenticated IPUM mips. It produced 349,184 RGBA bytes through the existing native host codec/color conversion, preserving original inputs and all mip levels. Generated catalog SHA256: 957b0959e08c2facdd9ae99632f22d047ea76cb717b81db042e8f1bd676d8f92. No original hardware IDCT or framebuffer parity was established. No chip emulation or guest instruction execution occurred.

The generated catalog was committed as c69b59e2a0d7cbbc1768263b0da45f68ac56555e, the frozen build source. Linux build passed: 26 newly compiled sources, 251 dependency/object-hash verified reused objects, 277 total; link exit 0. Executable SHA256: 6fba4182235e7db30e4355c27aef19727a4ea099fdd9d50a670212105c11605c.

## Failed observation

Main opened normally. The first Enter from original Main slot 1 returned unavailable before Rider activation. Public frontend status was 2, Main status 20, source PC 0, GPU/renderer diagnostics 0. The old log did not expose nested Rider cold-resource/session diagnostics, so these values do not identify the exact rejecting owner. Main mode 4 and the unreadable Rider selection snapshot there are expected and are not themselves evidence of corruption.

Only 10 validated Main frames were preserved. No Rider image, Song7 playback, Rider idle animation, rejected movement invariants or Back return was observed. The planned five-action sequence did not pass. Recorder exited 2.

The raw native log independently records explicit quit, process exit 0, cleanup_complete=true, 799 native frames, 11 native capture acknowledgments, zero world ticks and matching 511,564 source/output/captured audio frames. The recorder rejected capture 11 after the unexpected unavailable result and later rejected its final count mismatch with 10 preserved images. Its missing accepted closed receipt must not be reported as native cleanup failure. No forced signal was needed. No driving or gameplay occurred.

## Next work

Preserve the converted cache and successful build evidence. Source diagnosis is ongoing. A later unbuilt diagnostic change preserves the actual cold-preparation stage and nested Rider resource/material/session fields rather than losing them when mapping to unavailable. Failure-cleanup reporting is being corrected separately; neither change retroactively makes this observation pass. No extra native run was made at this reporting checkpoint.
