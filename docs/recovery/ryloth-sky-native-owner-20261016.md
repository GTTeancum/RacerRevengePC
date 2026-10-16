# Native retained Ryloth sky and original texture preparation

Root constructed 48 genuine original CUB tile images on 2026-10-16 at 01:43–01:44 UTC using reviewed recover_ra_sky.py from commit97cf7c7, authenticated RA.RES and data-only SLUS, the unchanged pinned native50FC decoder and installed FFmpeg. No guest instructions, emulator, game, compiler, syntax check or smoke ran. Production asset conversion is not gameplay validation.

The generated index SHA256 is ee5bb784a1120b0cbf56f234f0e78cf7ea5dc03d910da11d5d34f9431f1f85b7. Full archive/member/mip/output/decoder/FFmpeg provenance is in ryloth-sky-cache-manifest-20261016.json. Files are staged under runtime-relative evidence/skyRA. Each image is 128x128 RRTEX001; only the 48 authored tile masks generate source geometry. Existing 12A8A0-derived cube vertices, source UVs, camera-relative offset and strip order are reused unchanged.

The native sky owner supports explicit RA source identity. A cold-only retainedArchive borrow lets the same RA aggregate supply its already-decoded original archive; no second RA world or archive decode is required. Exact CUB and SCB hashes and each source/output tile hash remain mandatory. Old tracks retain the original file-backed path. The source enum adds RA without opening host/Course admission or enabling a legacy track fallback.

Authenticated RA SCB is 540795 bytes, SHA25657a0c9a244714d2f5f7b1ffcfb66fb182808348673b25362482458253b78ae81. Its Fog.ColorRGB literal at byte38633 is 180,180,165 followed by NUL. Existing bounded clear-color parsing is reused. Sky enablement still comes from the aggregate's actual original per-zone cube selector and the current camera primary zone; no all-zones override or missing-face fill is added.

Aggregate prepareSkyImages owns the real image/mesh bank. skyAssets is borrowed only while the retained aggregate is alive; the actual renderer/session must retire GPU use before aggregate close. sampleSky consumes the existing authenticated selector. This is a cold owner/API implementation. Driver wiring and actual rendered sky, teardown, screenshots and baseline FPS remain untested.
